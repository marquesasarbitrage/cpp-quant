#include "../../include/cpp-quant/instruments/bonds.hpp"

ZeroCouponBond::ZeroCouponBond(double notional, const DateTime& maturityDate, const DayCountConvention& dayCountConvention): 
Bond(notional,maturityDate,dayCountConvention){}; 

ZeroCouponBond::ZeroCouponBond(double notional, const DateTime& startDate, const Tenor& tenor, const DayCountConvention& dayCountConvention):
Bond(notional,startDate,tenor,dayCountConvention){}; 

Quote ZeroCouponBond::getRiskFreeValuation(const std::shared_ptr<RiskFreeRateValuationModel>& valuationModel) const
{
    double t = getScheduler().getYearFraction(valuationModel->getReferenceTime(), getMaturityDate());
    double df = valuationModel->getDiscountCurve().getValue(t); 
    return Quote(std::make_shared<Instrument>(*this), getNotional()*df, valuationModel);
}

Quote ZeroCouponBond::_getValuation(const std::shared_ptr<ValuationModel>& valuationModel) const
{
    if (auto d = std::dynamic_pointer_cast<RiskFreeRateValuationModel>(valuationModel)) {
            return getRiskFreeValuation(d);
        } else {
            throw QuantErrorRegistry::Instruments::IncorrectModelError();
    }
}