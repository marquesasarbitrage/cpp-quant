#pragma once 
#include <set>
#include "instruments.hpp"

class ZeroCouponBond final: public Bond
{
    public:
        ZeroCouponBond(double notional, const DateTime& maturityDate, const DayCountConvention& dayCountConvention); 
        ZeroCouponBond(double notional, const DateTime& startDate, const Tenor& tenor, const DayCountConvention& dayCountConvention); 
        ~ZeroCouponBond() = default; 
    
    protected: 
        Quote getRiskFreeValuation(const std::shared_ptr<RiskFreeRateValuationModel>& valuationModel) const override final;    

};

class CouponBond : public Bond 
{
    public: 
        CouponBond(double notional, const DateTime& startDate, const std::set<DateTime>& couponDates, const std::map<DateTime, double> redemptionFactorMap, const DayCountConvention& dayCountConvention); 
        virtual ~CouponBond() = default;

        DateTime getStartDate() const;
        std::set<DateTime> getCoupondDates() const;
        bool isQuoteDirty() const;

        std::shared_ptr<DateTime> getNextCouponDate(const DateTime& referenceTime) const; 
        std::shared_ptr<DateTime> getLastCouponDate(const DateTime& referenceTime) const; 
        double getOutstandingPrincipal(const DateTime& referenceTime) const;

        void setQuoteDirty(bool value);
    
    protected: 
        virtual Quote getRiskFreeValuation(const std::shared_ptr<RiskFreeRateValuationModel>& valuationModel) const override = 0;  
    
    private: 
        const DateTime startDate_;
        bool quoteDirty_;
        std::set<DateTime> couponDates_;
        std::map<DateTime, double> cumulativeRedemptionFactorMap_;
        std::vector<ZeroCouponBond> redemptionBonds_;

        void couponBondSetter(const std::set<DateTime>& couponDates, const std::map<DateTime, double> redemptionFactorMap);
        DateTime getBondMaturityDate(const std::map<DateTime, double> redemptionFactorMap) const;
};

// class FixedCouponBond 
// class VanillaFixed

class FixedCouponBond : public CouponBond
{
    public: 
        FixedCouponBond(double notional, const DateTime& startDate, const std::map<DateTime, double>& couponRateMap, const std::map<DateTime, double> redemptionFactorMap, const DayCountConvention& dayCountConvention); 
        virtual ~FixedCouponBond() = default;

    protected: 
        virtual Quote getRiskFreeValuation(const std::shared_ptr<RiskFreeRateValuationModel>& valuationModel) const override;  

    private: 


};

class VanillaCouponBond final: public Bond 
{
    public: 
        VanillaCouponBond(double notional, double couponValue, const DateTime& startDate, const Tenor& maturityTenor, const Tenor& frequencyTenor, const DayCountConvention& dayCountConvention);
        ~VanillaCouponBond() = default; 

        DateTime getStartDate() const;
        std::set<DateTime> getCoupondDates() const;
        double getCouponValue() const;
        bool isQuoteDirty() const;
        void setQuoteDirty(bool value);
        double getAccruedInterest(const DateTime& referenceTime) const; 
        std::shared_ptr<DateTime> getNextCouponDate(const DateTime& referenceTime) const; 
        std::shared_ptr<DateTime> getLastCouponDate(const DateTime& referenceTime) const; 

        
    protected: 
        Quote getRiskFreeValuation(const std::shared_ptr<RiskFreeRateValuationModel>& valuationModel) const override final;
    
    private: 
        const DateTime startDate_;
        bool quoteDirty_;
        const double couponValue_; 
        const std::set<DateTime> couponDates_;
        std::set<DateTime> getInitialCouponDates(const DateTime& startDate, const Tenor& maturityTenor, const Tenor& frequencyTenor) const;
        
};