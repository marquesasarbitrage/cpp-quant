#include "../../include/cpp-quant/tools/nss.hpp"

double NelsonSiegelFamily::rateFuntion1(double t, double tau){return tau*(1-std::exp(-t/tau))/t;}
double NelsonSiegelFamily::rateFuntion2(double t, double tau){return tau*(1-std::exp(-t/tau))/t - std::exp(-t/tau);}
double NelsonSiegelFamily::forwardRateFuntion1(double t, double tau){return std::exp(-t/tau);}
double NelsonSiegelFamily::forwardRateFuntion2(double t, double tau){return t*std::exp(-t/tau)/tau;}

NelsonSiegel::NelsonSiegel(double b0, double b1, double b2, double tau): b0_(b0), b1_(b1), b2_(b2), tau_(tau){};
Svensson::Svensson(double b0, double b1, double b2, double b3, double tau1, double tau2): 
b0_(b0), b1_(b1), b2_(b2), b3_(b3), tau1_(tau1), tau2_(tau2){};

double NelsonSiegel::getBeta1() const {return b1_;}
double NelsonSiegel::getBeta2() const  {return b2_;}
double NelsonSiegel::getBeta0() const  {return b0_;}
void NelsonSiegel::setBeta0(double b0) {b0_=b0;}
void NelsonSiegel::setBeta1(double b1) {b1_=b1;}
void NelsonSiegel::setBeta2(double b2) {b2_=b2;}

double Svensson::getBeta1() const {return b1_;}
double Svensson::getBeta2() const  {return b2_;}
double Svensson::getBeta0() const  {return b0_;}
double Svensson::getBeta3() const {return b3_;}
void Svensson::setBeta0(double b0) {b0_=b0;}
void Svensson::setBeta1(double b1) {b1_=b1;}
void Svensson::setBeta2(double b2) {b2_=b2;}
void Svensson::setBeta3(double b3) {b3_ = b3;}

double NelsonSiegel::getTau() const  {return tau_;}
void NelsonSiegel::setTau(double tau) {tau_=tau;}

double Svensson::getTau1() const {return tau1_;}
double Svensson::getTau2() const {return tau2_;}
void Svensson::setTau1(double tau1) {tau1_ = tau1;}
void Svensson::setTau2(double tau2) {tau2_ = tau2;}

double NelsonSiegel::getRate(double t) const {
    return b0_ + b1_ * rateFuntion1(t,tau_) + b2_ * rateFuntion2(t,tau_);
}

double NelsonSiegel::getInstantaneousForwardRate(double t) const {
    return b0_ + b1_ * forwardRateFuntion1(t,tau_) + b2_ * forwardRateFuntion2(t,tau_);
}

double NelsonSiegel::getDerivativeInstantaneousForwardRate(double t) const {
    return forwardRateFuntion1(t, tau_)*(b2_*(1-t/tau_) - b1_);
}

double Svensson::getRate(double t) const {
    return b0_ + b1_ * rateFuntion1(t,tau1_) + b2_ * rateFuntion2(t,tau1_) + b3_ * rateFuntion2(t,tau2_);
}

double Svensson::getInstantaneousForwardRate(double t) const {
    return b0_ + b1_ * forwardRateFuntion1(t,tau1_) + b2_ * forwardRateFuntion2(t,tau1_) + b3_ * forwardRateFuntion2(t,tau2_);
}

double Svensson::getDerivativeInstantaneousForwardRate(double t) const {
    return forwardRateFuntion1(t, tau1_)*(b2_*(1-t/tau1_) - b1_) + b3_*forwardRateFuntion1(t, tau2_)*(1-t/tau2_)/tau2_;
}

NelsonSiegelCalibration::NelsonSiegelCalibration(const std::map<double, double>& data, bool isSpotRate):data_(data), isSpotRate_(isSpotRate), gridSize_(.5){};

std::shared_ptr<NelsonSiegelFamily> NelsonSiegelCalibration::fitOLS(double tau1, double tau2, bool useSvensson) const
{
    std::vector<double> y; 
    std::vector<std::vector<double>> x;
    for (const auto& d : data_) 
    {
        y.push_back(d.second);
        std::vector<double> X = {};
        if (isSpotRate_){
            X.push_back(NelsonSiegel::rateFuntion1(d.first,tau1));
            X.push_back(NelsonSiegel::rateFuntion2(d.first,tau1));
            if (useSvensson) X.push_back(NelsonSiegel::rateFuntion2(d.first,tau2));
        }else {
            X.push_back(NelsonSiegel::forwardRateFuntion1(d.first,tau1));
            X.push_back(NelsonSiegel::forwardRateFuntion2(d.first,tau1));
            if (useSvensson)X.push_back(NelsonSiegel::forwardRateFuntion2(d.first,tau2));
        }
        x.push_back(X);
        
    }
    OrdinaryLeastSquare ols(y,x,true);
    double beta0 = ols.getIntercept(); 
    std::vector<double> betas = ols.getCoefficients();
    std::shared_ptr<NelsonSiegelFamily> nss = nullptr;
    if (useSvensson) nss = std::make_shared<Svensson>(beta0,betas[0],betas[1],betas[2],tau1,tau2);
    else nss = std::make_shared<NelsonSiegel>(beta0,betas[0],betas[1],tau1);
    return nss;
}

EstimatorLoss NelsonSiegelCalibration::getLoss(double tau1, double tau2, bool useSvensson) const
{
    std::shared_ptr<NelsonSiegelFamily> nss = fitOLS(tau1,tau2, useSvensson);
    std::vector<double> estimates; 
    std::vector<double> trueValues;
    for (const auto& d: data_)
    {
        estimates.push_back(isSpotRate_ ? nss->getRate(d.first) : nss->getInstantaneousForwardRate(d.first));
        trueValues.push_back(d.second);
    }
    return EstimatorLoss(estimates,trueValues);
}

double NelsonSiegelCalibration::getNelsonSiegelIniatialTau() const
{
    double tau, meanSquaredError, tauWinner, minMSE;
    double tMax = std::prev(data_.end())->first;
    while (tau<=tMax)
    {
        tau += gridSize_; 
        meanSquaredError = getLoss(tau,10.0,false).getMSE();
        std::cout << meanSquaredError<< std::endl;
        if (tau == gridSize_){
            tauWinner = tau;
            minMSE = meanSquaredError;
        }else{
            if (meanSquaredError<minMSE){
                tauWinner = tau; 
                minMSE = meanSquaredError;
            }
        }
    }
    return tauWinner;
}

std::vector<double> NelsonSiegelCalibration::getSvenssonIniatialTau() const
{
    double tau1, tau2, meanSquaredError, tauWinner1, tauWinner2, minMSE;
    double tMax = std::prev(data_.end())->first;
    while (tau1<=tMax)
    {
        tau1 += gridSize_;
        while (tau2<=tMax)
        {
            tau2 += gridSize_; 
            meanSquaredError = getLoss(tau1,tau2,true).getMSE();
            if (tau1 == gridSize_ and tau2 ==gridSize_){
                tauWinner1 = tau1;
                tauWinner2 = tau2;
                minMSE = meanSquaredError;
            }else{
                if (meanSquaredError<minMSE){
                    tauWinner1 = tau1;
                    tauWinner2 = tau2; 
                    minMSE = meanSquaredError;
                }
            }
        }
            
    }
    return {tauWinner1,tauWinner2};
}

void NelsonSiegelCalibration::setGridSize(double value) {gridSize_ = value;}

std::shared_ptr<NelsonSiegelFamily> NelsonSiegelCalibration::fitNelsonSiegel() const
{
    std::function<double(std::vector<double>)> targetFunction = [*this](std::vector<double> params)
    { 
        std::cout << params[0] << std::endl;
        if (params[0]==0.0) return 1e10;
        EstimatorLoss loss = getLoss(params[0],10.0, false);
        return loss.getMSE();
    };

    std::vector<double> initialTau = {getNelsonSiegelIniatialTau()};
    NelderMead nm = NelderMead(initialTau,targetFunction);
    nm.setInitSimplexMethod(NelderMead::InitSimplexMethod::SYMMETRIC); 
    nm.setPerturbationParam(gridSize_);
    nm.optimize();
    if (!nm.getError()) return fitOLS(nm.getResult()[0], 10.0, false);
    else return fitOLS(initialTau[0], 10.0, false);
}

std::shared_ptr<NelsonSiegelFamily>  NelsonSiegelCalibration::fitSvensson() const
{
    
    std::function<double(std::vector<double>)> targetFunction = [*this](std::vector<double> params)
    { 
        //std::cout << "Tau1: " <<params[0] << std::endl;
        //std::cout << "Tau2: " <<params[1] << std::endl;
        if (params[0]==0.0 or params[1]==0.0) return 1e10;
        EstimatorLoss loss = getLoss(params[0], params[1], true);
        return loss.getMSE();
    };
    std::vector<double> params0 = getSvenssonIniatialTau(); 
    NelderMead nm = NelderMead(params0,targetFunction);
    nm.optimize();
    nm.setInitSimplexMethod(NelderMead::InitSimplexMethod::SYMMETRIC); 
    nm.setPerturbationParam(gridSize_);
    if (!nm.getError()) return fitOLS(nm.getResult()[0], nm.getResult()[1], true);
    else return fitOLS(params0[0], params0[1], true);
}