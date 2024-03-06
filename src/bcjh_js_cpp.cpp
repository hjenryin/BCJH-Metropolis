#include "bcjh_js.hpp"
#include "../data/data.hpp"
#include <iostream>
#include <codecvt>
#include <locale>

std::string unicodeToString(const std::string &str) {
    std::string u8str;
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
    for (size_t i = 0; i < str.length();) {
        char32_t uhan = strtol(str.substr(i, 4).c_str(), nullptr, 16);
        u8str += converter.to_bytes(uhan);
        i += 4;
    }
    return u8str;
}

std::string decodeUnicodeEscape(const std::string &input) {
    std::string result;
    for (size_t i = 0; i < input.size(); ++i) {
        if (input[i] == '\\' && i + 1 < input.size() && input[i + 1] == 'u') {
            // Extract the hexadecimal value after "\\u"
            std::string hexValue = input.substr(i + 2, 4);
            try {
                result += unicodeToString(hexValue);
                // Skip the next 5 characters (including "\\u")
                i += 5;
            } catch (const std::invalid_argument &) {
                // Invalid hexadecimal value, skip this escape sequence
                result += input[i];
            }
        } else if (input[i] == '\\' && i + 1 < input.size() &&
                   input[i + 1] == 't') {
            result += '\t';
            i += 1;
        } else if (input[i] == '\\' && i + 1 < input.size() &&
                   input[i + 1] == 'n') {
            result += '\n';
            i += 1;
        } else {
            // Not an escape sequence, just append the character
            result += input[i];
        }
    }
    return result;
}

int main() {
    int8_t progress = 0;
    auto str = runjs(userData_CONST, ruleData_CONST, 3170000, 50, 10, true);
    // encode all unicodes in the string
    str = decodeUnicodeEscape(str);
    std::cout << str << std::endl;

    std::string input = "\\u968f\\u673"; // Your input string
    std::string decoded = decodeUnicodeEscape(input);
    std::cout << "Decoded string: " << decoded << std::endl;
    return 0;
}