#include "../../../include/cpp-quant/valuation/termstructure/termstructure.hpp"

TermStructure::TermStructure(const DateTime& referenceTime): referenceTime_(referenceTime){}; 

double TermStructure::getValue(double t) const {checkYearFraction(t); return _getValue(t);}

double TermStructure::getValue(const DateTime& datetime, const Scheduler& scheduler) const
{
    double t = scheduler.getYearFraction(getReferenceTime(), datetime);
    return getValue(t);
}

double TermStructure::getValue(const Tenor& tenor, const Scheduler& scheduler) const
{
    double t = scheduler.getYearFraction(getReferenceTime(), scheduler.getForwardDateTime(getReferenceTime(),tenor));
    return getValue(t);
}

DateTime TermStructure::getReferenceTime() const {return referenceTime_;}

void TermStructure::checkYearFraction(double t) const {if (t<0) throw QuantErrorRegistry::NegativeYearFractionError();}
void TermStructure::checkForwardYearFraction(double tStart, double tEnd) const 
{
    checkYearFraction(tStart);
    checkYearFraction(tEnd);
    if (tStart>tEnd) throw QuantErrorRegistry::NegativeForwardYearFractionError();

}