#pragma once 
#include <iostream>
#include "../../../../include/cpp-quant/tools/scheduler.hpp"
#include "../../../../include/cpp-quant/errors.hpp"

class TermStructure
{
    public: 
        TermStructure(const DateTime& referenceTime); 
        virtual ~TermStructure() = default; 

        double getValue(double t) const; 
        double getValue(const DateTime& datetime, const Scheduler& scheduler) const; 
        double getValue(const Tenor& tenor, const Scheduler& scheduler) const; 
        DateTime getReferenceTime() const; 

    protected: 
        virtual double _getValue(double t) const = 0; 
        void checkYearFraction(double t) const; 
        void checkForwardYearFraction(double tStart, double tEnd) const; 
    
    private: 
        const DateTime& referenceTime_; 
        
    
}; 

