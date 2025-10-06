#pragma once 
#include <iostream>
#include <complex>
#include "cpp-math/probability/distributions.hpp"
#include "cpp-math/quadratures.hpp"
#include "cpp-math/optim.hpp"

// Model references 
// The pricing of options and corporate liabilities - Black and Scholes (1973) : https://www.jstor.org/stable/1831029
// The pricing of commodity contract - Black (1976) : https://www.sciencedirect.com/science/article/abs/pii/0304405X76900246
// Let's be rational - Jäckel (2015) : http://www.jaeckel.org/
// Option valuation under stochastic volatility - Lewis (2000) : https://econpapers.repec.org/bookchap/vsvvbooks/ovsv.htm
// The volatility surface, a practitioner’s guide - Gatheral (2006) : https://link.springer.com/article/10.1007/s11408-007-0072-4
// The Heston Model and its Extensions in Matlab and C# - Fabrice Douglas Rouah (2013): https://onlinelibrary.wiley.com/doi/book/10.1002/9781118656471

namespace BlackTools 
{
    constexpr double H_LARGE = -10.0;
    constexpr double T_SMALL = 0.21;
    constexpr double PI = 3.14159265358979323846;
    constexpr double SQRT_TWO_PI = 2.50662827463100050242;
    constexpr double ONE_OVER_SQRT_TWO = 0.7071067811865475244008443621048490392848359376887;
    constexpr double ONE_OVER_SQRT_TWO_PI = 0.3989422804014326779399460599343818684758586311649;
    
    int getPutCallFlag(bool isCall); 
    double getNormalizedIntrisicValue(double x, bool isCall);
    // Normalized version of Lewis price to match normalized version (log-moneyness and total variance) of black-76 price
    double getLewisUndiscountedPrice(
        double x, 
        double isCall, 
        const std::function<std::complex<double>(std::complex<double>)>& characteristicFunction, 
        GaussLaguerreQuadrature& gaussLaguerreQuadrature_); 
    
}

namespace UndiscountedBlack
{
    // Some of the code is sourced from https://github.com/vollib/lets_be_rational/
    double getCallPriceRegion1(double h, double t); 
    double getCallPriceRegion2(double h, double t); 
    double getCallPriceRegion3(double h, double t); 
    double getCallPriceRegion4(double h, double t); 

    double getCallPrice(double x, double normalizedSigma);
    double getVega(double x, double normalizedSigma);
    double getVolga(double x, double normalizedSigma);
    double getPrice(double x, double normalizedSigma, bool isCall);

    std::complex<double> getCharacteristicFunction(std::complex<double> u, double x, double normalizedSigma);
    double getLewisPrice(double x, double normalizedSigma, bool isCall, GaussLaguerreQuadrature& gaussLaguerreQuadrature_);
}

namespace ImpliedVolatilitySolver
{
    const double minR = -(1 - sqrt(DBL_EPSILON));
    const double maxR = 2 / (DBL_EPSILON * DBL_EPSILON);

    double getRationalCubicInterpolate(double x, double x0, double x1,double y0, double y1,double dy0, double dy1,double r); 

    namespace RightAsymptotic
    {
        double getR(double x0, double x1,double y0, double y1,double dy0, double dy1, double ddy1);
        double get(double sigma); 
        double getFirstDerivative(double x, double sigma); 
        double getSecondDerivative(double x, double sigma); 
    }

    namespace LeftAsymptotic
    {
        double getR(double x0, double x1,double y0, double y1,double dy0, double dy1, double ddy0);
        double get(double x, double sigma); 
        double getFirstDerivative(double x, double sigma); 
        double getSecondDerivative(double x, double sigma); 
        double getZ(double x, double sigma);
    }

    std::tuple<double, double, double, double, double, double, double, double, double, double, bool> getInitialData(double beta, double x, double isCall);

    double getInitialGuess(double beta, double x, bool isCall); 
    std::function<double(double)> getTarget(double beta, double x, double bLower, double bUpper); 
    std::function<double(double)> getTargetFirstDerivative(double beta, double x, double bLower, double bUpper); 

    double getNewtonNormalizedVolatility(double beta, double x, bool isCall);

    double getBlackImpliedVolatility(double undiscountedPrice, double F, double K, double timeToMaturity, bool isCall);
}

namespace Heston
{
    struct Parameters
    {
        double kappa_, theta_, eta_, rho_, v0_; 
        Parameters(double kappa, double theta, double eta, double rho, double v0); 
        bool isFellerConditionSatisfied() const;
    };

    std::complex<double> getCharacteristicFunction(std::complex<double> u, double T, const Parameters& hestonParams);
    double getUndiscountedLewisPrice(double x, double T, const Parameters& hestonParams, bool isCall, GaussLaguerreQuadrature& gaussLaguerreQuadrature_);
    double getImpliedVolatility(double x, double T, const Parameters& hestonParams, GaussLaguerreQuadrature& gaussLaguerreQuadrature_);
}

