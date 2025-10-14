#include "../../include/cpp-quant/tools/nss.hpp"

NelsonSiegel::NelsonSiegel(double b0, double b1, double b2, double tau): b0_(b0), b1_(b1), b2_(b2), tau_(tau){};

Svensson::Svensson(double b0, double b1, double b2, double b3, double tau1, double tau2): 
NelsonSiegel(b0,b1,b2,tau1), b3_(b3), tau2_(tau2){};

double NelsonSiegel::getBeta1() const {return b1_;}
double NelsonSiegel::getBeta2() const  {return b2_;}
double NelsonSiegel::getBeta0() const  {return b0_;}
double NelsonSiegel::getTau() const  {return tau_;}

void NelsonSiegel::setBeta0(double b0) {b0_=b0;}
void NelsonSiegel::setBeta1(double b1) {b1_=b1;}
void NelsonSiegel::setBeta2(double b2) {b2_=b2;}
void NelsonSiegel::setTau(double tau) {tau_=tau;}

double Svensson::getBeta3() const {return b3_;}
double Svensson::getTau2() const {return tau2_;}
void Svensson::setBeta3(double b3) {b3_ = b3;}
void Svensson::setTau2(double tau2) {tau2_ = tau2;}

double NelsonSiegel::getF1(double t) const {
    return (1 - exp(-t / tau_)) / (t / tau_);
}

double NelsonSiegel::getF2(double t) const {
    return getF1(t) - exp(-t / tau_);
}

double NelsonSiegel::getRate(double t) const {
    return b0_ + b1_ * getF1(t) + b2_ * getF2(t);
}

double NelsonSiegel::getInstantaneousForwardRate(double t) const {
    return b0_ + b1_ * exp(-t / tau_) + b2_ * exp(-t / tau_) *  t / tau_;
}

double NelsonSiegel::getDerivativeInstantaneousForwardRate(double t) const {
    return b1_ * exp(-t / tau_)/tau_ + b2_ * exp(-t / tau_)/tau_ *  (1-t/tau_);
}

double Svensson::getF3(double t) const {
    double t1 = t / tau2_;
    return (1 - exp(-t1)) / t1 - exp(-t1);;
}

double Svensson::getRate(double t) const {
    
    return NelsonSiegel::getRate(t) + b3_ * getF3(t);
}

double Svensson::getInstantaneousForwardRate(double t) const {
    double t1 = t / tau2_;
    return NelsonSiegel::getInstantaneousForwardRate(t) + b3_ * exp(-t1) * t / tau2_; 
}

double Svensson::getDerivativeInstantaneousForwardRate(double t) const {
    return NelsonSiegel::getDerivativeInstantaneousForwardRate(t) + b3_*exp(-t / tau2_)/tau2_ *  (1-t/tau2_);
}

NelsonSiegelCalibration::NelsonSiegelCalibration(const std::map<double, double>& data, bool isSpotRate, bool useSvensson, double estimationTau1, double estimationTau2):
data_(data), isSpotRate_(isSpotRate), useSvensson_(useSvensson), estimationTau1_(std::max(estimationTau1, 0.1)), estimationTau2_(std::max(estimationTau2, 0.1)){};

NelsonSiegelCalibration::NelsonSiegelCalibration(const std::map<double, double>& data, bool isSpotRate, bool useSvensson):
data_(data), isSpotRate_(isSpotRate), useSvensson_(useSvensson), estimationTau1_(2.0), estimationTau2_(10.0){};

OrdinaryLeastSquare NelsonSiegelCalibration::getProxy() const
{
    std::vector<double> y; 
    std::vector<std::vector<double>> x;
    for (const auto& d : data_) 
    {
        y.push_back(d.second);
        double et1 = std::exp(-d.first/estimationTau1_); 
        double et2 = std::exp(-d.first/estimationTau2_); 
        if (isSpotRate_){
            
            x[0].push_back((1-et1)/(d.first/estimationTau1_));
            x[1].push_back((1-et1)/(d.first/estimationTau1_) - estimationTau1_);
            if (useSvensson_)
            {
                x[2].push_back((1-et2)/(d.first/estimationTau2_) - et2);
            }
        }else {
            x[0].push_back(et1);
            x[1].push_back(d.first*et1/estimationTau1_);
            if (useSvensson_)
            {
                x[2].push_back(d.first*et2/estimationTau2_);
            }
        }
        
    }
    OrdinaryLeastSquare ols(y,x,true);
    return ols;
}

EstimatorLoss NelsonSiegelCalibration::getLoss(std::shared_ptr<NelsonSiegel> nss) const
{
    std::vector<double> estimates; 
    std::vector<double> trueValues;
    for (const auto& d: data_)
    {
        estimates.push_back(isSpotRate_ ? nss->getRate(d.first) : nss->getInstantaneousForwardRate(d.first));
        trueValues.push_back(d.second);
    }
    return EstimatorLoss(estimates,trueValues);
}

double NelsonSiegelCalibration::getTargetFunction(std::vector<double> params) const
{
    if (useSvensson_){
        EstimatorLoss loss = getLoss(std::make_shared<Svensson>(params[0], params[1], params[2], params[3], params[4], params[5]));
        return loss.getRMSE();
    }else{
        EstimatorLoss loss = getLoss(std::make_shared<NelsonSiegel>(params[0], params[1], params[2], params[3]));
        return loss.getRMSE();
    }
}

NelderMead NelsonSiegelCalibration::getNelderMeadCalibrated() const
{
    OrdinaryLeastSquare proxyOLS = getProxy();
    std::vector<double> x0 = proxyOLS.getCoefficients(); 
    x0.insert(x0.begin(), proxyOLS.getIntercept());
    x0.push_back(estimationTau1_);
    if (useSvensson_) {x0.push_back(estimationTau2_);}
    std::function<double(std::vector<double>)> targetFunction = [*this](std::vector<double> params){ return getTargetFunction(params);};
    return NelderMead(x0,targetFunction);
}

