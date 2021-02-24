#include "utils.h"

std::vector<std::string> split(std::string str, std::string token) {
    std::vector<std::string> result;
    while (str.size()) {
        int index = str.find(token);
        if (index != std::string::npos) {
            result.push_back(str.substr(0, index));
            str = str.substr(index + token.size());
            if (str.size() == 0) result.push_back(str);
        } else {
            result.push_back(str);
            str = "";
        }
    }
    return result;
}

void findAndReplaceAll(std::string &data, std::string toSearch, std::string replaceStr) {
    // Get the first occurrence
    size_t pos = data.find(toSearch);
    // Repeat till end is reached
    while (pos != std::string::npos) {
        // Replace this occurrence of Sub String
        data.replace(pos, toSearch.size(), replaceStr);
        // Get the next occurrence from the current position
        pos = data.find(toSearch, pos + replaceStr.size());
    }
}

AddressingModeSize GetSizeFromAddressingMode(AddressingMode mode) {
    switch (mode) {
        case AddressingMode::Implied:
            return AddressingModeSize::NoBytes;
        default:
        case AddressingMode::Immediate:
        case AddressingMode::ZeroPage:
        case AddressingMode::XIndexed:
        case AddressingMode::YIndexed:
        case AddressingMode::Registry:
            return AddressingModeSize::SingleByte;

        case AddressingMode::Absolute:
        case AddressingMode::XIndexedAbsolute:
        case AddressingMode::YIndexedAbsolute:
            return AddressingModeSize::DoubleByte;
    }
}