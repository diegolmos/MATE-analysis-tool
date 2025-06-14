#ifndef DATESELECTOR_HPP
#define DATESELECTOR_HPP

#include <string>
#include <vector>
/// \param validPrefixes lista de prefijos válidos, **ordenada**.
/// \return sub-lista preservando el mismo orden.
std::vector<std::string>
selectDatePrefixes(const std::vector<std::string>& validPrefixes);

#endif /* DATESELECTOR_HPP */
    