#include "../../include/cpp-quant/tools/black.hpp"

namespace BlackTools 
{
    int getPutCallFlag(bool isCall) { return isCall ? 1:-1;}

    double getNormalizedIntrisicValue(double x, bool isCall)
    {
        if (getPutCallFlag(isCall)*x <= 0) return 0;
        double bm = exp(.5*x);
        return std::max(getPutCallFlag(isCall)*(bm - 1/bm), 0.0);
    }

    // Normalized version of Lewis price to match normalized version (log-moneyness and total variance) of black-76 price
    double getLewisUndiscountedPrice(
        double x, 
        double isCall, 
        const std::function<std::complex<double>(std::complex<double>)>& characteristicFunction, 
        GaussLaguerreQuadrature& gaussLaguerreQuadrature_)
    {
        int n = gaussLaguerreQuadrature_.getPoints();
        std::vector<double> laguerreNodes = gaussLaguerreQuadrature_.getRoots();
        std::vector<double> laguerreWeights = gaussLaguerreQuadrature_.getWeights();
        std::complex<double> i(0.0, 1.0);
        double sum = 0.0;
        for (int j = 0; j < n; ++j) {
            double u = laguerreNodes[j];
            std::complex<double> integrand = std::exp(i * u * x) * characteristicFunction(std::complex<double>(u, -0.5));
            double rintegrand = abs(integrand.real()) < DBL_MIN ? 0.0 : integrand.real();
            if (rintegrand == 0.0 or abs(laguerreWeights[j])<1e-20) sum += 0.0;
            else sum += laguerreWeights[j] * std::exp(u) * rintegrand / (u * u + 0.25);
        }
        double call = std::exp(x / 2.0) - sum / M_PI;
        return isCall ? call : call - getNormalizedIntrisicValue(x, true);
    }

}

namespace UndiscountedBlack
{
    double getCallPriceRegion1(double h, double t)
    {
        const double e=(t/h)*(t/h), r=((h+t)*(h-t)), q=(h/r)*(h/r);
        const double asymptotic_expansion_sum = (2.0+q*(-6.0E0-2.0*e+3.0*q*(1.0E1+e*(2.0E1+2.0*e)+5.0*q*(-1.4E1+e*(-7.0E1+e*(-4.2E1-2.0*e))+7.0*q*(1.8E1+e*(1.68E2+e*(2.52E2+e*(7.2E1+2.0*e)))+9.0*q*(-2.2E1+e*(-3.3E2+e*(-9.24E2+e*(-6.6E2+e*(-1.1E2-2.0*e))))+1.1E1*q*(2.6E1+e*(5.72E2+e*(2.574E3+e*(3.432E3+e*(1.43E3+e*(1.56E2+2.0*e)))))+1.3E1*q*(-3.0E1+e*(-9.1E2+e*(-6.006E3+e*(-1.287E4+e*(-1.001E4+e*(-2.73E3+e*(-2.1E2-2.0*e))))))+1.5E1*q*(3.4E1+e*(1.36E3+e*(1.2376E4+e*(3.8896E4+e*(4.862E4+e*(2.4752E4+e*(4.76E3+e*(2.72E2+2.0*e)))))))+1.7E1*q*(-3.8E1+e*(-1.938E3+e*(-2.3256E4+e*(-1.00776E5+e*(-1.84756E5+e*(-1.51164E5+e*(-5.4264E4+e*(-7.752E3+e*(-3.42E2-2.0*e))))))))+1.9E1*q*(4.2E1+e*(2.66E3+e*(4.0698E4+e*(2.3256E5+e*(5.8786E5+e*(7.05432E5+e*(4.0698E5+e*(1.08528E5+e*(1.197E4+e*(4.2E2+2.0*e)))))))))+2.1E1*q*(-4.6E1+e*(-3.542E3+e*(-6.7298E4+e*(-4.90314E5+e*(-1.63438E6+e*(-2.704156E6+e*(-2.288132E6+e*(-9.80628E5+e*(-2.01894E5+e*(-1.771E4+e*(-5.06E2-2.0*e))))))))))+2.3E1*q*(5.0E1+e*(4.6E3+e*(1.0626E5+e*(9.614E5+e*(4.08595E6+e*(8.9148E6+e*(1.04006E7+e*(6.53752E6+e*(2.16315E6+e*(3.542E5+e*(2.53E4+e*(6.0E2+2.0*e)))))))))))+2.5E1*q*(-5.4E1+e*(-5.85E3+e*(-1.6146E5+e*(-1.77606E6+e*(-9.37365E6+e*(-2.607579E7+e*(-4.01166E7+e*(-3.476772E7+e*(-1.687257E7+e*(-4.44015E6+e*(-5.9202E5+e*(-3.51E4+e*(-7.02E2-2.0*e))))))))))))+2.7E1*q*(5.8E1+e*(7.308E3+e*(2.3751E5+e*(3.12156E6+e*(2.003001E7+e*(6.919458E7+e*(1.3572783E8+e*(1.5511752E8+e*(1.0379187E8+e*(4.006002E7+e*(8.58429E6+e*(9.5004E5+e*(4.7502E4+e*(8.12E2+2.0*e)))))))))))))+2.9E1*q*(-6.2E1+e*(-8.99E3+e*(-3.39822E5+e*(-5.25915E6+e*(-4.032015E7+e*(-1.6934463E8+e*(-4.1250615E8+e*(-6.0108039E8+e*(-5.3036505E8+e*(-2.8224105E8+e*(-8.870433E7+e*(-1.577745E7+e*(-1.472562E6+e*(-6.293E4+e*(-9.3E2-2.0*e))))))))))))))+3.1E1*q*(6.6E1+e*(1.0912E4+e*(4.74672E5+e*(8.544096E6+e*(7.71342E7+e*(3.8707344E8+e*(1.14633288E9+e*(2.07431664E9+e*(2.33360622E9+e*(1.6376184E9+e*(7.0963464E8+e*(1.8512208E8+e*(2.7768312E7+e*(2.215136E6+e*(8.184E4+e*(1.056E3+2.0*e)))))))))))))))+3.3E1*(-7.0E1+e*(-1.309E4+e*(-6.49264E5+e*(-1.344904E7+e*(-1.4121492E8+e*(-8.344518E8+e*(-2.9526756E9+e*(-6.49588632E9+e*(-9.0751353E9+e*(-8.1198579E9+e*(-4.6399188E9+e*(-1.6689036E9+e*(-3.67158792E8+e*(-4.707164E7+e*(-3.24632E6+e*(-1.0472E5+e*(-1.19E3-2.0*e)))))))))))))))))*q)))))))))))))))));
        return BlackTools::ONE_OVER_SQRT_TWO_PI*exp((-0.5*(h*h+t*t)))*(t/r)*asymptotic_expansion_sum;
    }

    double getCallPriceRegion2(double h, double t)
    {
        const double a = 1+h*(0.5*BlackTools::SQRT_TWO_PI)*GaussianTool::erfcxCody(-BlackTools::ONE_OVER_SQRT_TWO*h), w=t*t, h2=h*h;
        const double expansion = 2*t*(a+w*((-1+3*a+a*h2)/6+w*((-7+15*a+h2*(-1+10*a+a*h2))/120+w*((-57+105*a+h2*(-18+105*a+h2*(-1+21*a+a*h2)))/5040+w*((-561+945*a+h2*(-285+1260*a+h2*(-33+378*a+h2*(-1+36*a+a*h2))))/362880+w*((-6555+10395*a+h2*(-4680+17325*a+h2*(-840+6930*a+h2*(-52+990*a+h2*(-1+55*a+a*h2)))))/39916800+((-89055+135135*a+h2*(-82845+270270*a+h2*(-20370+135135*a+h2*(-1926+25740*a+h2*(-75+2145*a+h2*(-1+78*a+a*h2))))))*w)/6227020800.0))))));
        return BlackTools::ONE_OVER_SQRT_TWO_PI*exp((-0.5*(h*h+t*t)))*expansion;
    } 

    double getCallPriceRegion3(double h, double t)
    {
        Gaussian norm = Gaussian();
        return norm.cdf(h+t)*exp(h*t) - norm.cdf(h-t)/exp(h*t);
    }

    double getCallPriceRegion4(double h, double t)
    {
        return 0.5 * exp(-0.5*(h*h+t*t)) * (GaussianTool::erfcxCody(-BlackTools::ONE_OVER_SQRT_TWO*(h+t)) - GaussianTool::erfcxCody(-BlackTools::ONE_OVER_SQRT_TWO*(h-t)));
    }

    double getCallPrice(double x, double normalizedSigma)
    {
        if (x>0) return BlackTools::getNormalizedIntrisicValue(x, true)+getCallPrice(-x,normalizedSigma);
        if (normalizedSigma<=0) return BlackTools::getNormalizedIntrisicValue(x, true);
        if ( x < normalizedSigma*BlackTools::H_LARGE  &&  0.5*normalizedSigma*normalizedSigma+x < normalizedSigma*(BlackTools::T_SMALL+BlackTools::H_LARGE)) return getCallPriceRegion1(x/normalizedSigma, .5*normalizedSigma);
        if ( 0.5*normalizedSigma < BlackTools::T_SMALL) return getCallPriceRegion2(x/normalizedSigma, .5*normalizedSigma);
        if (x+0.5*normalizedSigma*normalizedSigma > normalizedSigma*0.85) return getCallPriceRegion3(x/normalizedSigma, .5*normalizedSigma);
        return getCallPriceRegion4(x/normalizedSigma, .5*normalizedSigma);
    }

    double getVega(double x, double normalizedSigma)
    {
        if (normalizedSigma<=0) return 0;
        return BlackTools::ONE_OVER_SQRT_TWO_PI*exp(-0.5*(x*x/(normalizedSigma*normalizedSigma)+0.25*normalizedSigma*normalizedSigma));
    }

    double getVolga(double x, double normalizedSigma)
    {
        if (normalizedSigma<=0) return 0;
        return (x/(normalizedSigma*normalizedSigma*normalizedSigma) - .125)*getVega(x, normalizedSigma);
    }

    double getPrice(double x, double normalizedSigma, bool isCall)
    {
        if (normalizedSigma<=0) return BlackTools::getNormalizedIntrisicValue(x, isCall);
        if (!isCall) x = -x;
        return getCallPrice(x,normalizedSigma);
    }

    std::complex<double> getCharacteristicFunction(std::complex<double> u, double x, double normalizedSigma)
    {
        return std::exp(-0.5*u*(u+std::complex<double>(0.0,1.0))*normalizedSigma*normalizedSigma);
    }

    double getLewisPrice(double x, double normalizedSigma, bool isCall, const GaussLaguerreQuadrature& gaussLaguerreQuadrature_)
    {
        std::function<std::complex<double>(std::complex<double>)> cf = [normalizedSigma, x](std::complex<double> u) {
            return getCharacteristicFunction(u, x, normalizedSigma);
        };
        return BlackTools::getLewisUndiscountedPrice(x, isCall, cf, gaussLaguerreQuadrature_);
    }

}

namespace ImpliedVolatilitySolver
{
    double getRationalCubicInterpolate(double x, double x0, double x1,double y0, double y1,double dy0, double dy1,double r)
    {
        double h = x1 - x0;
        double s = (x - x0) / h;

        double numerator =
            y1 * s * s * s + 
            (r * y1 - h * dy1) * s * s * (1 - s) +
            (r * y0 + h * dy0) * s * (1 - s) * (1 - s) +
            y0 * (1 - s) * (1 - s) * (1 - s);

        double denominator = 1 + (r - 3) * s * (1 - s);

        return numerator / denominator;
    }

    namespace RightAsymptotic
    {

        double get(double sigma){return Gaussian().cdf(-sigma/2);}

        double getFirstDerivative(double x, double sigma){return (abs(x)<DBL_MIN) ? -.5 : -.5 * exp(.5*x*x/(sigma*sigma));}

        double getSecondDerivative(double x, double sigma)
        {
            double sigsq = sigma*sigma;
            return (abs(x)<DBL_MIN) ? 0 : exp(sigsq / 8 +  x*x/sigsq)*sqrt(BlackTools::PI/2) * x * x / (sigsq * sigma);
        }

        double getR(double x0, double x1,double y0, double y1,double dy0, double dy1, double ddy0)
        {
            double h = x1 - x0;
            double delta = (y1 - y0) / h;
            double denominator = (delta - dy0);
            double numerator =  (.5*h*ddy0 + dy1 - dy0);
            if (abs(denominator) < DBL_MIN) return (numerator > 0) ? maxR : minR;
            else return numerator / denominator;
        }
    }

    namespace LeftAsymptotic
    {
        double getR(double x0, double x1,double y0, double y1,double dy0, double dy1, double ddy1)
        {
            double h = x1 - x0;
            double delta = (y1 - y0) / h;
            double denominator = (dy1 - delta);
            double numerator =  (.5*h*ddy1 + dy1 - dy0);
            if (abs(denominator) < DBL_MIN) return (numerator > 0) ? maxR : minR;
            else return numerator / denominator;
        }

        double get(double x, double sigma)
        {
            double cdfz = Gaussian().cdf(getZ(x, sigma));
            return (2*BlackTools::PI*cdfz*cdfz*cdfz*abs(x))/(3*sqrt(3));
        }
        
        double getFirstDerivative(double x, double sigma)
        {
            double z = getZ(x, sigma);
            double cdfz = Gaussian().cdf(z);
            return 2*BlackTools::PI*z*z*cdfz*cdfz*exp(z*z+sigma*sigma/8.0);
            }
        
        double getSecondDerivative(double x, double sigma)
        {
            double z = getZ(x, sigma);
            double cdfz = Gaussian().cdf(z);
            double pdfz = Gaussian().pdf(z);
            double sigmasq = sigma*sigma;
            double term1 = (BlackTools::PI/6)*(z*z/(sigmasq*sigma))*cdfz*exp(2*z*z+sigmasq/4.0);
            double term2 = 8*sqrt(3)*sigma*abs(x)+(3*sigmasq*(sigmasq-8)-8*x*x)*cdfz/pdfz;
            return term1*term2;
        }
        
        double getZ(double x, double sigma){return -abs(x)/(sqrt(3)*sigma);}
        
    }

    std::tuple<double, double, double, double, double, double, double, double, double, double, bool> getInitialData(double beta, double x, double isCall)
    {
        int c = isCall ? 1 : -1;
        if (c*x>0){
            beta = fabs(std::max(beta-BlackTools::getNormalizedIntrisicValue(x, isCall),0.));
            isCall = not isCall;
        }
        if (!isCall) x = -x; isCall = true;
        double b_max = exp(.5*x);
        double sigma_c = sqrt(2*abs(x));
        double b_c = UndiscountedBlack::getPrice(x, sigma_c, isCall);
        double v_c = UndiscountedBlack::getVega(x, sigma_c);
        double sigma_u = (v_c>DBL_MIN) ? sigma_c + (b_max - b_c)/v_c : sigma_c;
        double sigma_l = (v_c>DBL_MIN) ? sigma_c - b_c/v_c : sigma_c;
        double b_u = UndiscountedBlack::getPrice(x, sigma_u, isCall);
        double b_l = UndiscountedBlack::getPrice(x, sigma_l, isCall);
        return std::make_tuple(beta,x,b_max,sigma_l, sigma_c, sigma_u, b_l, b_c, b_u, v_c, isCall); 
    }

    double getInitialGuess(double beta, double x, bool isCall)
    {
        auto [beta_,x_,b_max,sigma_l, sigma_c, sigma_u, b_l, b_c, b_u, v_c, is_call_] = getInitialData(beta, x, isCall);
        if (beta_ < b_l && beta_ >= 0) {
            double fl = LeftAsymptotic::get(x_, sigma_l);
            double dfl = LeftAsymptotic::getFirstDerivative(x_, sigma_l);
            double ddfl = LeftAsymptotic::getSecondDerivative(x_, sigma_l);
            double r = LeftAsymptotic::getR(0, b_l, 0, fl, 1, dfl, ddfl);
            double frc = getRationalCubicInterpolate(beta_, 0, b_l, 0, fl, 1, dfl, r);
            double sq3 = sqrt(3), a = frc/(2*BlackTools::PI*abs(x_));
            return abs(x_/(sq3*Gaussian().quantile(sq3*std::pow(a, 1.0/3.0))));
        }
        else if (beta_ <= b_c && beta_ >= b_l){
            double v_l = UndiscountedBlack::getVega(x_, sigma_l);
            double r = LeftAsymptotic::getR(b_l, b_c, sigma_l, sigma_c, 1/v_l, 1/v_c, 0);
            return getRationalCubicInterpolate(beta_, b_l, b_c, sigma_l, sigma_c, 1/v_l, 1/v_c, r);
        }
        else if (beta_ <= b_u && beta_ > b_c){
            double v_u = UndiscountedBlack::getVega(x_, sigma_u);
            double r = RightAsymptotic::getR(b_c, b_u, sigma_c, sigma_u, 1/v_c, 1/v_u, 0);
            return getRationalCubicInterpolate(beta_, b_c, b_u, sigma_c, sigma_u, 1/v_c, 1/v_u, r);
        }
        else {
            double fu = RightAsymptotic::get(sigma_u);
            double dfu = RightAsymptotic::getFirstDerivative(x_, sigma_u);
            double ddfu = RightAsymptotic::getSecondDerivative(x_, sigma_u);
            double r = RightAsymptotic::getR(b_u, b_max, fu, 0, dfu, -.5, ddfu);
            double frc = getRationalCubicInterpolate(beta_, b_u, b_max, fu, 0, dfu, -.5, r);
            return -2.0*Gaussian().quantile(frc);
        }
    } 

    std::function<double(double)> getTarget(double beta, double x, double bLower, double bUpper)
    {
        double bMax = exp(.5*x);
        double bUpper2 = std::max(bUpper, bMax/2);
        if (beta >= 0 && beta < bLower) return [beta, x](double s) {return log(1/UndiscountedBlack::getCallPrice(x, s)) - log(1/beta);};
        else if (beta >= bLower && beta <= bUpper2) return [beta, x](double s) {return UndiscountedBlack::getCallPrice(x, s) - beta;};
        else return [bMax, beta, x](double s) {return log((bMax-beta)/(bMax-UndiscountedBlack::getCallPrice(x, s)));};
    }

    std::function<double(double)> getTargetFirstDerivative(double beta, double x, double bLower, double bUpper)
    {
        double bMax = exp(.5*x);
        double bUpper2 = std::max(bUpper, bMax/2);
        if (beta >= 0 && beta < bLower) return [x](double s) {return -UndiscountedBlack::getVega(x, s)/UndiscountedBlack::getCallPrice(x, s);};
        else if (beta >= bLower && beta <= bUpper2) return [x](double s) {return UndiscountedBlack::getVega(x, s);};
        else return [bMax, x](double s) {return UndiscountedBlack::getVega(x, s)/(bMax - UndiscountedBlack::getCallPrice(x, s));};
    }

    double getNewtonNormalizedVolatility(double beta, double x, bool isCall)
    {
        auto [beta_,x_,b_max,sigma_l, sigma_c, sigma_u, b_l, b_c, b_u, v_c, is_call_] = getInitialData(beta, x, isCall);
        if (beta_<=0) return 0.0;
        NewtonRaphson newton(
            getInitialGuess(beta, x, isCall),
            getTarget(beta_, x_, b_l, b_u),
            getTargetFirstDerivative(beta_, x_, b_l, b_u)
        );
        newton.setToleranceThreshold(1e-20);
        newton.setMaximumIterations(100);
        newton.optimize(); 
        if (!newton.getError()) return newton.getResult();
        else return NAN;
    }

    double getBlackImpliedVolatility(double undiscountedPrice, double x, double timeToMaturity, bool isCall)
    {
        if (undiscountedPrice<0) return NAN;

        try{
            double beta = undiscountedPrice/sqrt(std::exp(x));
            if (abs(UndiscountedBlack::getVega(x, getInitialGuess(beta, x, isCall)))<1e-5) return NAN;
            else{
                double iv = getNewtonNormalizedVolatility(beta, x, isCall)/sqrt(timeToMaturity);
                if (iv<=0.0) return NAN;
                else return iv; 
            }
            
        }catch (const std::exception& e)
        {
            return NAN;
        }

    }

}

namespace Heston 
{
    Parameters::Parameters(double kappa, double theta, double eta, double rho, double v0):  kappa_(kappa), theta_(theta), eta_(eta), rho_(rho), v0_(v0){};
    bool Parameters::isFellerConditionSatisfied() const {return 2.0 * kappa_ * theta_ > eta_ * eta_;}

    std::complex<double> getCharacteristicFunction(std::complex<double> u, double T, const Parameters& hestonParams)
    {
        std::complex<double> i(0.0, 1.0);
        double e = hestonParams.eta_, k = hestonParams.kappa_, ro = hestonParams.rho_;
        std::complex<double> beta = k - ro*e*i*u;
        std::complex<double> d = sqrt(beta*beta + e*e*(i*u + u*u));
        std::complex<double> g = (beta - d)/(beta + d); 
        std::complex<double> D = (beta - d)*(1.0-exp(-d*T))/(e*e*(1.0-g*exp(-d*T)));
        std::complex<double> C = k*(T*(beta-d) - 2.0*log((1.0-g*exp(-d*T))/(1.0-g)))/(e*e); 
        return exp(C*hestonParams.theta_+D*hestonParams.v0_); 
    }

    double getUndiscountedLewisPrice(double x, double T, const Parameters& hestonParams, bool isCall, const GaussLaguerreQuadrature& gaussLaguerreQuadrature_)
    {
        std::function<std::complex<double>(std::complex<double>)> cf = [T,hestonParams](std::complex<double> u) {
            return getCharacteristicFunction(u, T, hestonParams);
        };
        return BlackTools::getLewisUndiscountedPrice(x, isCall, cf, gaussLaguerreQuadrature_);
    }

    double getImpliedVolatility(double x, double T, const Parameters& hestonParams, const GaussLaguerreQuadrature& gaussLaguerreQuadrature_)
    {
        double undiscountedPrice = getUndiscountedLewisPrice(x,T,hestonParams, true, gaussLaguerreQuadrature_); 
        return ImpliedVolatilitySolver::getBlackImpliedVolatility(undiscountedPrice,x,T,true);
    }


}