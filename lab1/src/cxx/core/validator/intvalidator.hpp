#pragma once
#include <string>

class IntValidator {
public:
    explicit IntValidator() = default;

    static bool valid(const std::string& str);

};