#ifndef TREELOADER_HPP
#define TREELOADER_HPP

#include "PathConfig.hpp"

class TTree;  
/// \return `TTree*`  (puede ser un `TChain`, que hereda de `TTree`).<br>

TTree* loadRootChainInteractive(const PathConfig& cfg);

#endif /* TREELOADER_HPP */
