#include "../../include/cpp-quant/instruments/bonds.hpp"

ZeroCouponBond::ZeroCouponBond(double notional, const DateTime& maturityDate, const DayCountConvention& dayCountConvention): 
Bond(notional,maturityDate,dayCountConvention){}; 

ZeroCouponBond::ZeroCouponBond(double notional, const DateTime& startDate, const Tenor& tenor, const DayCountConvention& dayCountConvention):
Bond(notional,Scheduler(true,HolidayCalendar::NONE,dayCountConvention).getForwardDateTime(startDate,tenor),dayCountConvention){}; 

Quote ZeroCouponBond::getRiskFreeValuation(const std::shared_ptr<RiskFreeRateValuationModel>& valuationModel) const
{
    double t = getScheduler().getYearFraction(valuationModel->getReferenceTime(), getMaturityDate());
    double df = valuationModel->getDiscountCurve().getValue(t); 
    return Quote(std::make_shared<Instrument>(*this), getNotional()*df, valuationModel);
}

CouponBond::CouponBond(double notional, const DateTime& startDate, const std::set<DateTime>& couponDates, const std::map<DateTime, double> redemptionFactorMap, const DayCountConvention& dayCountConvention): 
Bond(notional, getBondMaturityDate(redemptionFactorMap),dayCountConvention), startDate_(startDate), quoteDirty_(false) {couponBondSetter(couponDates,redemptionFactorMap);}

DateTime CouponBond::getBondMaturityDate(const std::map<DateTime, double> redemptionFactorMap) const
{
    if (redemptionFactorMap.size() == 0) throw QuantErrorRegistry::Instruments::Bond::InvalidRedemptionMapError();
    return redemptionFactorMap.end()->first;
}

void CouponBond::couponBondSetter(const std::set<DateTime>& couponDates, const std::map<DateTime, double> redemptionFactorMap)
{
    DateTime maturityDate = getMaturityDate(); 
    for (const DateTime& c : couponDates) {
        if (c<= maturityDate && c>startDate_) couponDates_.insert(c); 
        else throw QuantErrorRegistry::Instruments::Bond::InvalidCouponDateError(); 
    }
    double cumulFactor = 0.0;
    for (const auto& rfm : redemptionFactorMap){
        if (rfm.second<0 or rfm.second >1) throw QuantErrorRegistry::Instruments::Bond::InvalidRedemptionMapError();
        if (rfm.first>maturityDate or rfm.first <= startDate_) throw QuantErrorRegistry::Instruments::Bond::InvalidRedemptionMapError();
        cumulFactor += rfm.second; 
        cumulativeRedemptionFactorMap_[rfm.first] = cumulFactor;
        redemptionBonds_.push_back(ZeroCouponBond(getNotional()*rfm.second,rfm.first, getScheduler().getDayCountConvention()));
    }
    if (cumulativeRedemptionFactorMap_.end()->second!=1.0) throw QuantErrorRegistry::Instruments::Bond::InvalidRedemptionMapError();

}

DateTime CouponBond::getStartDate() const {return startDate_;}
std::set<DateTime> CouponBond::getCoupondDates() const {return couponDates_;}
bool CouponBond::isQuoteDirty() const {return quoteDirty_;}

std::shared_ptr<DateTime> CouponBond::getNextCouponDate(const DateTime& referenceTime) const
{
    if (couponDates_.empty()) return nullptr; 
    if (referenceTime<=*couponDates_.begin()) return std::make_shared<DateTime>(*couponDates_.begin());
    if (referenceTime>*couponDates_.end()) return nullptr; 
    if (referenceTime==*couponDates_.end()) return std::make_shared<DateTime>(*couponDates_.end());
    auto it = couponDates_.lower_bound(referenceTime);
    return std::make_shared<DateTime>(*it);
}

std::shared_ptr<DateTime> CouponBond::getLastCouponDate(const DateTime& referenceTime) const
{
    if (couponDates_.empty()) return nullptr; 
    if (referenceTime<*couponDates_.begin()) return nullptr;
    if (referenceTime==*couponDates_.begin()) return std::make_shared<DateTime>(*couponDates_.begin());
    if (referenceTime>=*couponDates_.end()) return std::make_shared<DateTime>(*couponDates_.end());
    auto it = couponDates_.lower_bound(referenceTime);
    --it;
    return std::make_shared<DateTime>(*it);
}

double CouponBond::getOutstandingPrincipal(const DateTime& referenceTime) const
{
    if (referenceTime<=cumulativeRedemptionFactorMap_.begin()->first) return 1.0;
    if (referenceTime>cumulativeRedemptionFactorMap_.end()->first) return 0.0;
    auto it = cumulativeRedemptionFactorMap_.lower_bound(referenceTime);
    --it; 
    return 1.0 - it->second;
}

void CouponBond::setQuoteDirty(bool value) {quoteDirty_ = value;}


