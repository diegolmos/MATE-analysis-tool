#include "DateSelector.hpp"
#include "DateUtils.hpp"     

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <iostream>
#include <regex>
#include <set>
#include <sstream>

namespace fs = std::filesystem;


std::vector<std::string>
selectDatePrefixes(const std::vector<std::string>& valid)
{
    //fechas dsiponibles
    std::cout << "\nFechas disponibles:\n";
    for (const auto& p : valid)
        std::cout << "  • " << DateUtils::prettyDate(p) << '\n';

    //entrada del usuario
    std::cout << "\nSeleccione fechas\n"
                 "  ENTER                → Todas las fechas\n"
                 "  lista  YYYYMMDD,...  → 20240921,20240923\n"
                 "  rango  YYYYMMDD-YYYYMMDD → 20240921-20240925\n"
                 "Selección: ";

    std::string input;
    std::getline(std::cin, input);

    if (input.empty()) return valid;                 // caso “todas”

    // Normalizar: quitar espacios
    input.erase(std::remove_if(input.begin(), input.end(), ::isspace),
                input.end());

    std::set<std::string> wanted;                    //prefijos elegidos
    std::regex rangeRe(R"(^(\d{8})-(\d{8})$)");
    std::smatch m;

    if (std::regex_match(input, m, rangeRe)) {
        std::string start = m[1], end = m[2];

        for (const auto& p : valid) {
            std::string d = DateUtils::digitsDate(p);   // YYYYMMDD
            if (start <= d && d <= end) wanted.insert(p);
        }
    }

    else {
        auto cleanToken = [](std::string t){
            t.erase(std::remove_if(t.begin(), t.end(),
                                   [](char c){ return !std::isdigit(c); }),
                     t.end());
            return t;
        };
        std::stringstream ss(input); std::string tok;
        while (std::getline(ss, tok, ',')) {
            tok = cleanToken(tok);
            for (const auto& p : valid)
                if (DateUtils::digitsDate(p) == tok)
                    { wanted.insert(p); break; }
        }}
    std::vector<std::string> selected;
    for (const auto& p : valid)
        if (wanted.count(p)) selected.push_back(p);

    return selected;
}
