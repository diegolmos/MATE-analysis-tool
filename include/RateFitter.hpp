#ifndef RATEFITTER_HPP
#define RATEFITTER_HPP

#include <string>

class TTree;


struct FitResult {double A; double err_A; double lambda; double err_lambda; double period; double err_period;};


FitResult runRateFit(TTree* tree, const std::string& ts2branch = "ts2_clean", double histRangeS = 2.0, int nBins = 20);

#endif /* RATEFITTER_HPP */
