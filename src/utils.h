#pragma once
#include <vector>
#include <string>
#include "structs.h"

std::vector<std::string> split(std::string str, std::string token);

void findAndReplaceAll(std::string &data, std::string toSearch, std::string replaceStr);

AddressingModeSize GetSizeFromAddressingMode(AddressingMode mode);