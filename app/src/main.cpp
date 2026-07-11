#include <iostream>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <algorithm>

#include "MemoryMappedFile.h"
#include "CsvParser.h"
#include "ThreadPool.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_csv>" << std::endl;
        return 1;
    }

    try {
        auto start_time = std::chrono::high_resolution_clock::now();

        MemoryMappedFile mmapFile(argv[1]);
        std::string_view fileView(mmapFile.data(), mmapFile.size());

        size_t headerEnd = fileView.find('\n');
        if (headerEnd == std::string_view::npos) return 0;
        std::string_view dataView = fileView.substr(headerEnd + 1);

        unsigned int num_threads = std::thread::hardware_concurrency();
        size_t chunkSize = dataView.size() / num_threads;

        std::vector<std::string_view> chunks;
        size_t current_start = 0;

        for (unsigned int i = 0; i < num_threads; i++) {
            size_t current_end = current_start + chunkSize;

            if (i == num_threads - 1 || current_end >= dataView.size()) {
                current_end = dataView.size();
            } else {
                size_t nl = dataView.find('\n', current_end);
                if (nl != std::string_view::npos) {
                    current_end = nl + 1;
                } else {
                    current_end = dataView.size();
                }
            }
            chunks.push_back(dataView.substr(current_start, current_end - current_start));
            current_start = current_end;
        }

        std::vector<std::unordered_map<int, double>> localResults(num_threads);

        std::cout << "Processing " << mmapFile.size() / (1024*1024) 
                << " MB across " << num_threads << " threads...\n";

        {
            ThreadPool pool(num_threads);
            for (unsigned int i = 0; i < num_threads; i++) {
                pool.enqueue([i, chunk=chunks[i], &localMap=localResults[i]] {
                    size_t pos = 0;
                    while (pos < chunk.size()) {
                        size_t endLine = chunk.find('\n', pos);
                        if (endLine == std::string_view::npos) endLine = chunk.size();

                        std::string_view rowStr = chunk.substr(pos, endLine - pos);
                        pos = endLine + 1;

                        if (rowStr.empty() || rowStr == "\r") continue;

                        CsvRow row(rowStr);

                        if (row.getString(5) == "SUCCESS") {
                            int userId = row.getInt(2);
                            double amount = row.getDouble(3);
                            localMap[userId] += amount;
                        }
                    }
                });
            }
        }

        std::unordered_map<int, double> globalTotals;
        for (const auto& localMap : localResults) {
            for (const auto& [userId, amount] : localMap) {
                globalTotals[userId] += amount;
            }
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end_time - start_time;

        std::vector<std::pair<int, double>> topUsers(globalTotals.begin(), globalTotals.end());

        std::sort(topUsers.begin(), topUsers.end(),
        [](const auto& a, const auto& b) {return a.second > b.second; });

        std::cout << "========================================\n";
        std::cout << "Done in: " << duration.count() << " seconds\n";
        std::cout << "Unique users with SUCCESS: " << globalTotals.size() << "\n";
        std::cout << "========================================\n";
        std::cout << "Top 5 Users:\n";

        for (size_t i = 0; i < std::min<size_t>(5, topUsers.size()); i++) {
            std::cout << "User " << topUsers[i].first << ": " << topUsers[i].second << "\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}