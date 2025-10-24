#include "../../../include/cpp-quant/valuation/models/models.hpp"

ValuationModel::ValuationModel(const DateTime& referenceTime): referenceTime_(referenceTime){}; 
ValuationModel::~ValuationModel(){}; 

DateTime ValuationModel::getReferenceTime() const {return referenceTime_;}

RiskFreeRateValuationModel::RiskFreeRateValuationModel(const DiscountCurve& discountCurve, const AverageOvernightRate& averageOvernightRate): 
ValuationModel(discountCurve.getReferenceTime()), discountCurve_(discountCurve_), averageOvernightRate_(averageOvernightRate){}; 

DiscountCurve RiskFreeRateValuationModel::getDiscountCurve() const {return discountCurve_;}
AverageOvernightRate RiskFreeRateValuationModel::getAverageOvernightRate() const {return averageOvernightRate_;}