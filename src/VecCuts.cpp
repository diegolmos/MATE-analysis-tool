#include "VecCuts.hpp"



bool No56(const std::vector<int>* v)
{
    for (int x : *v)
        if (x == 5 || x == 6)
            return false;              
    return true;                       
}


