#pragma once 
#include "instruments.hpp"

class ZeroCouponBond final: public Bond
{
    public:
        ZeroCouponBond(double notional, const DateTime& maturityDate, const DayCountConvention& dayCountConvention); 
        ZeroCouponBond(double notional, const DateTime& startDate, const Tenor& tenor, const DayCountConvention& dayCountConvention); 
        ~ZeroCouponBond() = default; 
    
    protected: 
        Quote _getValuation(const std::shared_ptr<ValuationModel>& valuationModel) const override;
    
    private: 
        Quote getRiskFreeValuation(const std::shared_ptr<RiskFreeRateValuationModel>& valuationModel) const;
};

//class VanillaCouponBond; 