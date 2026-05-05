#include <intvalidator.hpp>

bool IntValidator::valid(const std::string &str)
{
    bool result = std::all_of(str.begin(), str.end(), [](const char& ch ){
        return std::isdigit(static_cast<unsigned char>(ch));
    });
    return result;
}