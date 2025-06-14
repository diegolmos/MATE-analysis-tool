#ifndef DATAPROCESSOR_HPP
#define DATAPROCESSOR_HPP

#include "PathConfig.hpp"        //rutas globales (dataDir, badDir, reportFile)
#include <string>
#include <unordered_map>
#include <vector>

///Convierte triadas mate-m10{1,2,3}.txt en un TTree ROOT y un .txt combinado.
class DataProcessor {
public:
    ///Procesa **todas** las fechas encontradas en cfg.dataDir (orden cronológico).
    static void processAll(const PathConfig& cfg);
    static bool processDate(const PathConfig& cfg,
                            const std::string& prefix);

private:
    static std::string hexToBin(const std::string& hexValue,
                                int lineNumber,
                                std::unordered_map<std::string, std::string>& cache);
    static std::vector<int> binToPositions(const std::string& binValue);
    static std::string      positionsToString(const std::vector<int>& positions);

    DataProcessor() = delete;   
};

#endif  // DATAPROCESSOR_HPP
