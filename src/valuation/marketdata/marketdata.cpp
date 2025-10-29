#include "../../../include/cpp-quant/valuation/marketdata/marketdata.hpp"

MarketData::MarketData(const DateTime& referenceTime): referenceTime_(referenceTime){}; 
MarketData::~MarketData() = default;

DateTime MarketData::getReferenceTime() const{return referenceTime_;}

void MarketData::checkYearFraction(double t) {if (t<0) throw QuantErrorRegistry::NegativeYearFractionError();}
void MarketData::checkForwardYearFraction(double tStart, double tEnd) 
{
    if (tStart<0) throw QuantErrorRegistry::NegativeYearFractionError();
    if (tEnd<0) throw QuantErrorRegistry::NegativeYearFractionError();
    if (tStart>tEnd) throw QuantErrorRegistry::NegativeForwardYearFractionError();

}

AverageOvernightRate::AverageOvernightRate(std::map<DateTime, double> annualizedFixingRates): MarketData(getLastDate(annualizedFixingRates)), annualizedFixingRates_(annualizedFixingRates){};

std::map<DateTime, double> AverageOvernightRate::getAnnualizedFixingRates() const{return annualizedFixingRates_;}

DateTime AverageOvernightRate::getLastDate(std::map<DateTime, double> annualizedFixingRates)
{
    if (!annualizedFixingRates.empty()) return annualizedFixingRates.rbegin()->first;
    else throw QuantErrorRegistry::Valuation::MarketData::EmptyOvernightAverageRateError();
}

double AverageOvernightRate::getFixingRate(const DateTime& referenceTime) const
{
    auto it = annualizedFixingRates_.lower_bound(referenceTime);
    if (it != annualizedFixingRates_.end() && !(referenceTime < it->first)) return it->second;
    if (it == annualizedFixingRates_.begin()) return annualizedFixingRates_.begin()->second;
    --it;
    return it->second;
}

std::map<DateTime, double> AverageOvernightRate::getObservations(const DateTime& startTime, const DateTime& endTime) const
{
    std::map<DateTime, double> obs_ = {{startTime, getFixingRate(startTime)}, {endTime, getFixingRate(endTime)}}; 
    auto it = annualizedFixingRates_.lower_bound(startTime);
    ++it;
    while (it != annualizedFixingRates_.end() && it->first < endTime) {obs_.insert(*it);++it;}
    return obs_;

}

double AverageOvernightRate::getAnnualizedAverageRate(const DateTime& startTime, const DateTime& endTime, const Scheduler& scheduler) const
{
    double T = scheduler.getYearFraction(startTime, endTime);
    double rate = 1.0; 
    std::map<DateTime, double> obs = getObservations(startTime, endTime);
    double t;
    for (const auto& o: obs)
    {
        if (o.first==endTime) break;
        else t = scheduler.getYearFraction(o.first, obs.upper_bound(o.first)->first);
        rate *= (1+o.second*t);
    }
    return (rate-1.0)/T;

}

TermStructure::TermStructure(const DateTime& referenceTime): MarketData(referenceTime){}; 

double TermStructure::getValue(double t) const {MarketData::checkYearFraction(t); return _getValue(t);}

double TermStructure::getValue(const DateTime& datetime, const Scheduler& scheduler) const{return getValue(scheduler.getYearFraction(getReferenceTime(), datetime));}

double TermStructure::getValue(const Tenor& tenor, const Scheduler& scheduler) const{return getValue(scheduler.getYearFraction(getReferenceTime(),tenor));}

