#ifndef DATAVALIDATOR_HPP
#define DATAVALIDATOR_HPP

#include "PathConfig.hpp"      
#include <string>
#include <vector>


struct DateReport {
    std::string prefix;              
    bool        valid  = false;      
    std::vector<std::string> issues; 
};


class DataValidator {
public:
    /// \return prefijos válidos (cada uno termina en “…/YYYYMMDD...”).
    static std::vector<std::string>
    validateDirectory(const PathConfig& cfg, bool promptMove = true);
private:
    static DateReport      validateTriad(const PathConfig& cfg,
                                         const std::string& prefix);
    static bool            fileExistsReadable(const std::string& path);
    static bool            isValidHexByte(const std::string& s);
    static std::vector<std::string>
                           listPrefixes(const std::string& dataDir);

    DataValidator() = delete;};

#endif   // DATAVALIDATOR_HPP
