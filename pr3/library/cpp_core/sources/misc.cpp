#include "misc.hpp"


std::string lowercase(const std::string& string) {
    std::string lower = string;
    std::transform(string.begin(), string.end(), lower.begin(), [](unsigned char c){ return std::tolower(c); });
    return string;
}
