#include "DataValidator.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
namespace fs = std::filesystem;

std::vector<std::string>
DataValidator::validateDirectory(const PathConfig& cfg, bool promptMove)
{std::vector<std::string> validPrefixes; std::vector<DateReport>  reports;
     auto prefixes = listPrefixes(cfg.dataDir);
    std::sort(prefixes.begin(), prefixes.end());
    for (const auto& pre : prefixes) 
        {DateReport rep = validateTriad(cfg, pre);
        reports.push_back(rep);
        if (rep.valid) validPrefixes.push_back(pre);}

  
    {
        std::ofstream ofs(cfg.reportFile);
        for (const auto& r : reports) {
            ofs << (r.valid ? "[OK]  " : "[BAD] ") << r.prefix << '\n';
            for (const auto& iss : r.issues)
                ofs << "        - " << iss << '\n';
        }
        std::cout << "Informe guardado en '" << cfg.reportFile << "'.\n";
    }

  
    if (promptMove) {
        bool hayMalos = std::any_of(reports.begin(), reports.end(),
                                    [](auto& r){ return !r.valid; });
        if (hayMalos) {
            std::cout << "¿Mover fechas problemáticas a '" << cfg.badDir
                        << "'? [s/N]: ";
            char ans; std::cin >> ans;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // ← DESCARTA \n

            if (ans=='s'||ans=='S') {
                fs::create_directories(cfg.badDir);
                for (auto& r : reports) {
                    if (r.valid) continue;
                    for (const char* tag : { "mate-m101.txt",
                                             "mate-m102.txt",
                                             "mate-m103.txt" }) {
                        fs::path src = r.prefix + tag;
                        if (fs::exists(src))
                            fs::rename(src, fs::path(cfg.badDir) / src.filename());
                    }
                }
                std::cout << "Archivos movidos a '" << cfg.badDir << "'.\n";
            }
        }
    }

    return validPrefixes;
}


DateReport DataValidator::validateTriad(const PathConfig& /*cfg*/,
                                        const std::string& pre)
{DateReport rep; rep.prefix = pre;
    const std::string files[3] = {
        pre + "mate-m101.txt",
        pre + "mate-m102.txt",
        pre + "mate-m103.txt"
    };
    for (const auto& f : files)
        if (!fileExistsReadable(f))
            rep.issues.emplace_back("archivo faltante o ilegible: " +
                                    fs::path(f).filename().string());
    if (!rep.issues.empty()) { rep.valid = false; return rep; }


    std::ifstream in1(files[0]), in2(files[1]), in3(files[2]);
    std::string l1,l2,l3;
    size_t lineN   = 0;
    int prevEvt = -1;

    auto split = [](const std::string& s){
        std::vector<std::string> v; std::stringstream ss(s); std::string tok;
        while (std::getline(ss, tok, ',')) v.push_back(tok);
        return v;};

    constexpr int EXPECTED_COLS = 6;          //ahora 6–6–6
    const std::regex HEX_RE("^[0-9A-Fa-f]{1,2}$");

    while (std::getline(in1,l1)) {
        ++lineN;
        if (!std::getline(in2,l2) || !std::getline(in3,l3)) {
            rep.issues.emplace_back("línea " + std::to_string(lineN) +
                                    ": número de líneas distinto entre archivos");
            break;
        }

        auto v1=split(l1), v2=split(l2), v3=split(l3);

        bool badCols = false, badHex = false, badEvt = false;

 
        if ( (int)v1.size()!=EXPECTED_COLS ||
             (int)v2.size()!=EXPECTED_COLS ||
             (int)v3.size()!=EXPECTED_COLS )
            badCols = true;


        const std::string hexFields[] = { v1[1], v1[2], v1[3],
                                          v2[1], v2[2], v2[3],
                                          v3[1], v3[2], v3[3] };
        for (const auto& h: hexFields)
            if (!std::regex_match(h, HEX_RE)) { badHex = true; break; }


        try {
            int evt = std::stoi(v1[5]);
            if (prevEvt!=-1 && evt!=prevEvt+1) badEvt = true;
            prevEvt = evt;
        } catch (...) { badEvt = true; }


        if (badCols)
            rep.issues.emplace_back("línea " + std::to_string(lineN) +
                                    ": columnas faltantes");
        if (badHex)
            rep.issues.emplace_back("línea " + std::to_string(lineN) +
                                    ": hex inválido");
        if (badEvt)
            rep.issues.emplace_back("línea " + std::to_string(lineN) +
                                    ": eventos no consecutivos");
    }


    if (std::getline(in2,l2) || std::getline(in3,l3))
        rep.issues.emplace_back("número de líneas distinto al final");

    rep.valid = rep.issues.empty();
    return rep;}


bool DataValidator::fileExistsReadable(const std::string& path)
{std::ifstream f(path);
    return f.good();}

std::vector<std::string> DataValidator::listPrefixes(const std::string& dataDir)
{   std::vector<std::string> pref;
    const std::string tag = "mate-m101.txt";
    for (const auto& e : fs::directory_iterator(dataDir)) {
        const std::string p = e.path().string();
        if (p.size() >= tag.size() &&
            p.compare(p.size()-tag.size(), tag.size(), tag)==0)
            pref.emplace_back(p.substr(0, p.size()-tag.size()));
    }
    return pref;}

bool DataValidator::isValidHexByte(const std::string& s)
{static const std::regex re("^[0-9A-Fa-f]{1,2}$");
    return std::regex_match(s, re);}
