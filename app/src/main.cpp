#include <iostream>
#include <string_view>
#include "MemoryMappedFile.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_csv>" << std::endl;
        return 1;
    }

    try {
        std::string filepath = argv[1];
        std::cout << "Mapping file into memory..." << std::endl;

        MemoryMappedFile mmapFile(filepath);

        std::cout << "File mapped successfully!" << std::endl;
        std::cout << "File size: " << mmapFile.size() / (1024 * 1024) << " MB" << std::endl;

        std::string_view head(mmapFile.data(), std::min<size_t>(mmapFile.size(), 100));
        std::cout << "--- Head of file ---\n" << head << "\n--------------------\n";
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}