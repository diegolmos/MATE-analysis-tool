#include "PathConfig.hpp"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

PathConfig PathConfig::makeInteractive(PathConfig cfg)
{
    auto ask = [](const std::string& msg, std::string& val) {
        if (val.empty()) {
            std::cout << msg << " (enter = .): ";
            std::getline(std::cin, val);
            if (val.empty()) val = ".";
        }
    };

    ask("Directorio con datos (.txt / .root)",         cfg.dataDir);
    ask("Directorio para archivos problemáticos",      cfg.badDir);
    ask("Nombre del archivo de reporte",               cfg.reportFile);

    if (!cfg.badDir.empty()) fs::create_directories(cfg.badDir);
    return cfg;
}
