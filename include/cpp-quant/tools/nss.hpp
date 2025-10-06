#pragma once 
#include <iostream>

// Model refrences 
// Parsimonious Modelling of Yield Curve - Nelson and Siegel (1987) : https://www.jstor.org/stable/2352957
// Estimating Forward Interest Rates with the Extended Nelson & Siegel Method - Svensson (1995) : https://larseosvensson.se/papers/95QRabs/

class NelsonSiegel 
{
    public: 
        NelsonSiegel(double b0, double b1, double b2, double tau); 
        virtual ~NelsonSiegel() = default; 

        double getRate(double t) const; 
        double getInstantaneousForwardRate(double t) const;

        double getBeta1() const; 
        double getBeta2() const; 
        double getBeta0() const; 
        double getTau() const; 

        void setBeta0(double b0);
        void setBeta1(double b1);
        void setBeta2(double b2);
        void setTau(double tau);

    private:
        double b0_;  
        double b1_;  
        double b2_;  
        double tau_;
        double getF1(double t) const; 
        double getF2(double t) const; 
}; 

class Svensson final: public NelsonSiegel
{
    public: 
        Svensson(double b0, double b1, double b2, double b3, double tau1, double tau2); 
        ~Svensson() = default;

        double getRate(double t) const; 
        double getInstantaneousForwardRate(double t) const;
        double getBeta3() const;
        double getTau2() const;
        void setBeta3(double b3);
        void setTau2(double tau2);
    private:
        double b3_; 
        double tau2_;
        double getF3(double t) const;
};

