#pragma once
#include <string_view>
#include <charconv>
#include <array>

class CsvRow {
public:
    CsvRow(std::string_view row) {
        size_t start = 0;

        for (int i = 0; i < 5; i++) {
            size_t end = row.find(',', start);
            if (end == std::string_view::npos) break;

            columns[i] = row.substr(start, end - start);
            start = end + 1;
        }
        columns[5] = row.substr(start);

        if (!columns[5].empty() && columns[5].back() == '\r') {
            columns[5].remove_suffix(1);
        }
    }

    std::string_view getString(size_t index) const {
        return columns[index];
    }

    int getInt(size_t index) const {
        int value = 0;
        auto col = columns[index];
        std::from_chars(col.data(), col.data() + col.size(), value);
        return value;
    }

    double getDouble(size_t index) const {
        double value = 0.0;
        auto col = columns[index];
        std::from_chars(col.data(), col.data() + col.size(), value);
        return value;
    }

private:
    std::array<std::string_view, 6> columns;
};