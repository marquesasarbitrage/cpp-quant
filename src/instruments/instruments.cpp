#include "../../include/cpp-quant/instruments/instruments.hpp"

Instrument::Instrument(){}; 

Quote Instrument::getValuation(const std::shared_ptr<ValuationModel>& valuationModel) const
{
    try {
        if (isSettled(valuationModel->getReferenceTime())) throw QuantErrorRegistry::Instruments::SettledInstrumentError();
        return _getValuation(valuationModel);
    } catch (const std::exception& e) {
        return Quote(std::make_shared<Instrument>(*this), std::current_exception(), valuationModel);
    }
};

Quote Instrument::getMarketValuation(const DateTime& refenceTime, double bid, double ask) const
{
    return Quote(std::make_shared<Instrument>(*this), refenceTime, bid, ask);
}

Quote::Quote(const std::shared_ptr<Instrument>& instrument, const DateTime& referenceTime, double bid, double ask): 
instrument_(instrument), referenceTime_(referenceTime), valuationModel_(nullptr), bid_(bid), ask_(ask), error_(nullptr)
{
    try{ 
        if (getBidAskSpread()<0) throw QuantErrorRegistry::Instruments::NegativeBidAskSpreadError();
    } catch (...) {
        error_ = std::current_exception();
        bid_ = NAN; 
        ask_ = NAN;
    }
};

Quote::Quote(const std::shared_ptr<Instrument>& instrument, const DateTime& referenceTime, double mid): 
instrument_(instrument), referenceTime_(referenceTime), valuationModel_(nullptr), bid_(mid), ask_(mid), error_(nullptr){};

Quote::Quote(const std::shared_ptr<Instrument>& instrument, double mid, const std::shared_ptr<ValuationModel>& valuationModel): 
instrument_(instrument), referenceTime_(valuationModel->getReferenceTime()), valuationModel_(valuationModel), bid_(mid), ask_(mid), error_(nullptr){};

Quote::Quote(const std::shared_ptr<Instrument>& instrument, std::exception_ptr error, const std::shared_ptr<ValuationModel>& valuationModel): 
instrument_(instrument), referenceTime_(valuationModel->getReferenceTime()), valuationModel_(valuationModel), bid_(NAN), ask_(NAN), error_(error){};

std::shared_ptr<Instrument> Quote::getInstrument() const {return instrument_;}
DateTime Quote::getReferenceTime() const {return referenceTime_;}
std::shared_ptr<ValuationModel> Quote::getValuationModel() const {return valuationModel_;}
std::exception_ptr Quote::getError() const{return error_;}
double Quote::getBid() const {return bid_;}
double Quote::getAsk() const {return ask_;} 
double Quote::getMid() const {return .5*(bid_+ask_);}  
double Quote::getBidAskSpread() const {return ask_-bid_;}  
double Quote::getBidAskSpreadPercentage() const {try{return getBidAskSpread()/bid_;} catch (const std::exception& e){return NAN;}}
double Quote::getMidAskSpread() const {return ask_-getMid();} 
double Quote::getMidAskSpreadPercentage() const {try{return getMidAskSpread()/getMid();} catch (const std::exception& e){return NAN;}}
double Quote::getBidMidSpread() const {return getMid()-bid_;} 
double Quote::getBidMidSpreadPercentage() const {try{return getBidMidSpread()/bid_;} catch (const std::exception& e){return NAN;}}

Bond::Bond(double notional, const DateTime& maturityDate, const DayCountConvention& dayCountConvention):
notional_(notional), scheduler_(Scheduler(true,HolidayCalendar::NONE,dayCountConvention)), maturityDate_(maturityDate)
{
    if (notional_<0) throw QuantErrorRegistry::Instruments::NegativeNotionalError();
};

bool Bond::isSettled(const DateTime& referenceTime) const
{
    if (referenceTime >= getMaturityDate()) return true; 
    else return false;
}

Scheduler Bond::getScheduler() const {return scheduler_;}
DateTime Bond::getMaturityDate() const {return maturityDate_;}
double Bond::getNotional() const {return notional_;}

Quote Bond::_getValuation(const std::shared_ptr<ValuationModel>& valuationModel) const
{
    if (auto d = std::dynamic_pointer_cast<RiskFreeRateValuationModel>(valuationModel)) return getRiskFreeValuation(d);
    else throw QuantErrorRegistry::Instruments::IncorrectModelError();
}