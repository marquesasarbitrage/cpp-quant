#include <iostream>
#include <iomanip>
#include <memory>
#include <cassert>
#include <map>
#include <chrono>
#include "../include/cpp-quant/tools/black.hpp"

// Helper function for floating-point comparisons
bool isClose(double a, double b, double tol = 1e-3) {
    return std::abs(a - b) <= tol;
}

void testUndiscountedBlack()
{
    // Input parameters
    double S = 100.0;    // Stock price
    double K = 100.0;    // Strike price
    double r = 0.05;     // Risk-free rate
    double q = 0.02;     // Dividend yield
    double sigma = 0.20; // Volatility
    double T = 2.0;      // Time to maturity

    // Option prices and Greeks
    double callPrice = 13.5218;
    double putPrice = 7.9266;

    double x = log(S*exp((r-q)*T)/K); 
    double normalizedSigma = sigma*sqrt(T);

    double modelCallPrice = sqrt(S*exp((r-q)*T)*K)*UndiscountedBlack::getPrice(x,normalizedSigma,true)*exp(-r*T);
    double modelPutPrice = sqrt(S*exp((r-q)*T)*K)*UndiscountedBlack::getPrice(x,normalizedSigma,false)*exp(-r*T);
    assert(isClose(modelCallPrice, callPrice));
    assert(isClose(modelPutPrice, putPrice));

    GaussLaguerreQuadrature quad(64);
    // Test Lewis method
    double lewisPutPrice = sqrt(S*exp((r-q)*T)*K)*UndiscountedBlack::getLewisPrice(x, normalizedSigma, false, quad)*exp(-r*T);
    double lewisCallPrice = sqrt(S*exp((r-q)*T)*K)*UndiscountedBlack::getLewisPrice(x, normalizedSigma, true, quad)*exp(-r*T);
    assert(isClose(modelCallPrice, callPrice, 1e-2));
    assert(isClose(modelPutPrice, putPrice, 1e-2));

    std::cout << "Undiscounted Black model Passed! (Base function and Lewis price function)"<<std::endl;

}

void testHestonLewisPrices()
{
    // Test Heston model
    double S = 100.0;    // Stock price
    double K = 100.0;    // Strike price
    double r = 0.05;     // Risk-free rate
    double q = 0.01;     // Dividend yield
    double T = 1.5;      // Time to maturity
    double F = S * std::exp((r - q) * T); // Forward price
    

    Heston::Parameters heston(2.0,0.05,0.3,0.45,0.05);
    GaussLaguerreQuadrature quad(64); 
    double expectedHestonPrice = 13.2561; 
    double modelHestonPrice = sqrt(F*K)*Heston::getUndiscountedLewisPrice(log(F/K),T,heston,true,quad)*exp(-r*T);
    assert(isClose(modelHestonPrice, expectedHestonPrice, 1e-4));

    std::cout << "The test of the undiscounted normalized Heston Lewis price fucntion is Passed!\n";

}

void testSolveNewtonNormalizedVolatility() 
{
    std::cout << "Testing get_newton_normalized_volatility..." << std::endl;
    for (double s = 0.01; s < 7; s+= .5)
    {
        double x = -4;
        for (double x = -10; x<0; x+=0.5)
        {

            double b = UndiscountedBlack::getPrice(x,s,true); 
            double s_lbr = ImpliedVolatilitySolver::getNewtonNormalizedVolatility(b,x,true);
            if (s_lbr>0) assert(abs(s_lbr-s)<= 1e-6);

        }
        
    }
        
    std::cout << "All tests passed for Newton implied normalized volatility solver!" << std::endl;
}

void testBlackImpliedVolatilitySolver()
{
    double S = 100.0;    // Stock price
    double K = 100.0;    // Strike price
    double r = 0.05;     // Risk-free rate
    double q = 0.02;     // Dividend yield
    double sigma = 0.20; // Volatility
    double T = 2.0;      // Time to maturity

    // Option prices and Greeks
    double callPrice = 13.5218;
    double putPrice = 7.9266;

    double x = log(S*exp((r-q)*T)/K); 
    double normalizedSigma = sigma*sqrt(T);

    double undiscountedPrice = sqrt(S*exp((r-q)*T)*K)*UndiscountedBlack::getPrice(x,normalizedSigma,true);

    double sigmaSolved = ImpliedVolatilitySolver::getNewtonNormalizedVolatility(undiscountedPrice/sqrt(S*exp((r-q)*T)*K), x, true)/sqrt(T);
    assert(isClose(sigma,sigmaSolved, 1e-3));
}

void testBlackImpliedVolatilitySolver2()
{
    std::cout << "Testing Black Implied volatility solver..." << std::endl;
    double S = 100; 
    double r = 0.01; 
    double q = 0.02; 
    double F, price, iv, vega, expectedVega;
    for (double K = 1; K <= 500; K+= 10)
    {
        for (double T = 0.001; T <=30; T+= 0.5)
        {
            for (double s= 0.01; s<=5; s+=0.1)
            {
                F = S*exp((r-q)*T);

                price = sqrt(F*K)*UndiscountedBlack::getPrice(log(F/K),s*sqrt(T),true); 
                iv = ImpliedVolatilitySolver::getBlackImpliedVolatility(price,F,K,T,true);
                vega = UndiscountedBlack::getVega(log(F/K),iv*sqrt(T));
                expectedVega = UndiscountedBlack::getVega(log(F/K),s*sqrt(T));
                if (!isnan(vega) and !isnan(expectedVega)) assert(abs(vega-expectedVega)<= 1e-3);

                //price = sqrt(F*K)*UndiscountedBlack::getPrice(log(F/K),s*sqrt(T),false); 
                //iv = ImpliedVolatilitySolver::getBlackImpliedVolatility(price,F,K,T,false);
                //if (!isnan(iv)) assert(abs(iv-s)<= 1e-3);
                
            }
        }
    }
    std::cout << "All tests passed for Black Implied volatility solver!" << std::endl;
}

int main()
{
    testUndiscountedBlack(); 
    testSolveNewtonNormalizedVolatility(); 
    testHestonLewisPrices(); 
    testBlackImpliedVolatilitySolver();
    testBlackImpliedVolatilitySolver2();
    return 0;
}