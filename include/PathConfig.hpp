#ifndef PATHCONFIG_HPP
#define PATHCONFIG_HPP

#include <string>

struct PathConfig {
    std::string dataDir;     ///Directorio con .txt / .root
    std::string badDir;      /// Directorio para archivos problemáticos
    std::string reportFile;  ///Informe de validación

    static PathConfig makeInteractive(PathConfig cfg = {});
};

#endif
