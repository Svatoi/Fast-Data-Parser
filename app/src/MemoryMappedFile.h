#pragma once
#include <windows.h>
#include <string>
#include <stdexcept>
#include <iostream>

class MemoryMappedFile {
public:
    MemoryMappedFile(const std::string& filepath) {
        hFile = CreateFileA(filepath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE) {
            throw std::runtime_error("Failed to open file");
        }

        LARGE_INTEGER size;
        GetFileSizeEx(hFile, &size);
        fileSize = size.QuadPart;

        hMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
        if (hMapping == NULL) {
            CloseHandle(hFile);
            throw std::runtime_error("Failed to create file mapping");
        }

        mappedData = (const char*)MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);
        if (mappedData == NULL) {
            CloseHandle(hMapping);
            CloseHandle(hFile);
            throw std::runtime_error("Failed to map view of file");
        }
    }

    ~MemoryMappedFile() {
        if (mappedData) UnmapViewOfFile(mappedData);
        if (hMapping) CloseHandle(hMapping);
        if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
    }

    MemoryMappedFile(const MemoryMappedFile&) = delete;
    MemoryMappedFile& operator=(const MemoryMappedFile&) = delete;

    const char* data() const { return mappedData; }
    size_t size() const {return fileSize; }

private:
    HANDLE hFile = INVALID_HANDLE_VALUE;
    HANDLE hMapping = NULL;
    const char* mappedData = nullptr;
    size_t fileSize = 0;
};
