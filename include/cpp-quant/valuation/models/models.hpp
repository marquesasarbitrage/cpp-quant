#pragma once 
#include <iostream>
#include "../../../../include/cpp-quant/errors.hpp"
#include "../../../../include/cpp-quant/tools/scheduler.hpp"
#include "../../../../include/cpp-quant/valuation/marketdata/termstructures/discountcurve.hpp"

class ValuationModel
{
    public: 
        ValuationModel(const DateTime& referenceTime); 
        virtual ~ValuationModel() = 0; 

        DateTime getReferenceTime() const;  
    
    private: 
        DateTime referenceTime_; 
        
};

class RiskFreeRateValuationModel : public ValuationModel 
{
    public: 
        RiskFreeRateValuationModel(const DiscountCurve& discountCurve, const AverageOvernightRate& averageOvernightRate); 
        virtual ~RiskFreeRateValuationModel() = default; 

        DiscountCurve getDiscountCurve() const; 
        AverageOvernightRate getAverageOvernightRate() const;
    
    private: 
        const DiscountCurve discountCurve_;
        const AverageOvernightRate averageOvernightRate_;
};
