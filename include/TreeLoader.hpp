#ifndef TREELOADER_HPP
#define TREELOADER_HPP

#include "PathConfig.hpp"

class TTree;  
/// \return `TTree*` cargado de los archivos .root

TTree* loadRootChainInteractive(const PathConfig& cfg);

#endif /* TREELOADER_HPP */
