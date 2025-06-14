#ifndef DATAFILTER_HPP
#define DATAFILTER_HPP

#include <string>

class TTree;  


struct FilterResult {
    TTree* tree;      
    bool   ownsTree;  
};


/// \param tree Árbol base sobre el que aplicar los filtros.
/// \return  FilterResult con la nueva vista o árbol.
FilterResult runInteractive(TTree* tree);

#endif /* DATAFILTER_HPP */
