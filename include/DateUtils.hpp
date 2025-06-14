#ifndef DATEUTILS_HPP
#define DATEUTILS_HPP

#include <string>
#include <filesystem>
#include <regex>
#include <algorithm>
#include <cctype>

namespace DateUtils {
inline std::string prettyDate(const std::string& prefix)
{
    std::string stem = std::filesystem::path(prefix).stem().string();
    std::regex  re(R"((\d{4})[_-]?(\d{2})[_-]?(\d{2}))");
    std::smatch m;
    if (std::regex_search(stem, m, re))
        return m[1].str() + "_" + m[2].str() + "_" + m[3].str();
    return stem;                          
}
inline std::string digitsDate(const std::string& prefix)
{
    std::string s = prettyDate(prefix);     
    s.erase(std::remove_if(s.begin(), s.end(),
                           [](char c){ return !std::isdigit(c); }),
            s.end());
    return s;                               
}

} // namespace DateUtils
#endif /* DATEUTILS_HPP */
