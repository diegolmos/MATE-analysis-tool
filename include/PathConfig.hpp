#ifndef PATHCONFIG_HPP
#define PATHCONFIG_HPP

#include <string>

struct PathConfig {
    std::string dataDir;     ///< Directorio con .txt / .root
    std::string badDir;      ///< Directorio para archivos problemáticos
    std::string reportFile;  ///< Informe de validación

    /// Rellena los campos vacíos preguntando al usuario.
    /// Crea 'badDir' si no existe.
    static PathConfig makeInteractive(PathConfig cfg = {});
};

#endif
