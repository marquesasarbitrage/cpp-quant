#pragma once 
#include <iostream>
#include "../../../include/cpp-quant/valuation/models/models.hpp"

class Instrument
{
    public: 
        Instrument(); 
        virtual ~Instrument() = default; 

        Quote getValuation(const std::shared_ptr<ValuationModel>& valuationModel) const;
        Quote getMarketValuation(const DateTime& refenceTime, double bid, double ask) const;
        virtual bool isSettled(const DateTime& refenceTime) const = 0;
    
    protected: 
        virtual Quote _getValuation(const std::shared_ptr<ValuationModel>& valuationModel) const = 0; 
}; 

class Quote
{
    public: 
        Quote(const std::shared_ptr<Instrument>& instrument, const DateTime& referenceTime, double bid, double ask);
        Quote(const std::shared_ptr<Instrument>& instrument, const DateTime& referenceTime, double mid);
        Quote(const std::shared_ptr<Instrument>& instrument, double mid, const std::shared_ptr<ValuationModel>& valuationModel);
        Quote(const std::shared_ptr<Instrument>& instrument, std::exception_ptr error, const std::shared_ptr<ValuationModel>& valuationModel);
        virtual ~Quote() = default; 

        std::shared_ptr<Instrument> getInstrument() const; 
        DateTime getReferenceTime() const; 
        std::shared_ptr<ValuationModel> getValuationModel() const; 
        std::exception_ptr getError() const; 
        double getBid() const; 
        double getAsk() const; 
        double getMid() const; 
        double getBidAskSpread() const; 
        double getBidAskSpreadPercentage() const; 
        double getMidAskSpread() const; 
        double getMidAskSpreadPercentage() const; 
        double getBidMidSpread() const; 
        double getBidMidSpreadPercentage() const; 
    
    private: 
        const std::shared_ptr<Instrument> instrument_; 
        const DateTime referenceTime_;
        const std::shared_ptr<ValuationModel>& valuationModel_; 
        double bid_; 
        double ask_; 
        std::exception_ptr error_;
        
};

class Bond : public Instrument
{
    public:
        Bond(double notional, const DateTime& maturityDate, const DayCountConvention& dayCountConvention); 
        virtual ~Bond() = default; 
    
        Scheduler getScheduler() const; 
        DateTime getMaturityDate() const;
        double getNotional() const; 
        bool isSettled(const DateTime& referenceTime) const override;
    
    protected: 
        Quote _getValuation(const std::shared_ptr<ValuationModel>& valuationModel) const override;
        virtual Quote getRiskFreeValuation(const std::shared_ptr<RiskFreeRateValuationModel>& valuationModel) const = 0;

    private: 
        const double notional_;
        const Scheduler scheduler_;
        const DateTime maturityDate_;
    
}; 

