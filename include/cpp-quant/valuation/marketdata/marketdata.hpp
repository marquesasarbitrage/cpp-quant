#pragma once 
#include <iostream>
#include <map>
#include "../../../../include/cpp-quant/errors.hpp"
#include "../../../../include/cpp-quant/tools/scheduler.hpp"

class MarketData
{
    public: 
        MarketData(const DateTime& referenceTime); 
        virtual ~MarketData() = 0; 

        DateTime getReferenceTime() const; 
        static void checkYearFraction(double t); 
        static void checkForwardYearFraction(double tStart, double tEnd); 
    
    private: 
        DateTime referenceTime_; 
        
};

class AverageOvernightRate : public MarketData
{
    public: 
        AverageOvernightRate(std::map<DateTime, double> annualizedFixingRates); 
        virtual ~AverageOvernightRate() = default;

        std::map<DateTime, double> getAnnualizedFixingRates() const;
        virtual double getAnnualizedAverageRate(const DateTime& startTime, const DateTime& endTime, const Scheduler& scheduler) const;
        std::map<DateTime, double> getObservations(const DateTime& startTime, const DateTime& endTime) const;
        double getFixingRate(const DateTime& referenceTime) const;
    
    private: 
        std::map<DateTime, double> annualizedFixingRates_;
        DateTime getLastDate(std::map<DateTime, double> annualizedFixingRates);
        
};

class TermStructure : public MarketData
{
    public: 
        TermStructure(const DateTime& referenceTime); 
        virtual ~TermStructure() = default; 

        double getValue(double t) const; 
        double getValue(const DateTime& datetime, const Scheduler& scheduler) const; 
        double getValue(const Tenor& tenor, const Scheduler& scheduler) const; 

    protected: 
        virtual double _getValue(double t) const = 0; 
        
}; 
