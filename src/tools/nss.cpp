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