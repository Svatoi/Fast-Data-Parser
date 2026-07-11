#include <iostream>
#include <string_view>
#include "MemoryMappedFile.h"
#include "CsvParser.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_csv>" << std::endl;
        return 1;
    }

    try {
        MemoryMappedFile mmapFile(argv[1]);
        std::string_view fileView(mmapFile.data(), mmapFile.size());

        size_t firstNewline = fileView.find('\n');
        if (firstNewline == std::string_view::npos) return 0;

        size_t secondNewline = fileView.find('\n', firstNewline + 1);
        if (secondNewline == std::string_view::npos) return 0;

        std::string_view firstDataRow = fileView.substr(
            firstNewline + 1,
            secondNewline - firstNewline - 1
        );

        std::cout << "Raw row: " << firstDataRow << "\n\n";

        CsvRow row(firstDataRow);

        std::cout << "--- Parsed Data ---\n";
        std::cout << "Transaction ID : " << row.getString(0) << "\n";
        std::cout << "User ID        : " << row.getInt(2) << "\n";
        std::cout << "Amount         : " << row.getDouble(3) << "\n";
        std::cout << "Currency       : " << row.getString(4) << "\n";
        std::cout << "Status         : " << row.getString(5) << "\n";

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}