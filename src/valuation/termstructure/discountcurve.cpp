#include "../../../include/cpp-quant/valuation/termstructure/discountcurve.hpp"

double DiscountCurve::getDiscountFactorSimple(double yield, double t){return 1.0/(1+yield*t);}
double DiscountCurve::getDiscountFactorContinuous(double yield, double t){return std::exp(-t*yield);}

DiscountCurve::DiscountCurve(const DateTime& referenceTime, std::shared_ptr<NelsonSiegel> nssYieldObject, const YieldType& yieldType): 
TermStructure(referenceTime), yieldType_(yieldType), interpolationMethod_(std::nullopt), 
nssYieldObject_(nssYieldObject), interpolatedLogDiscountPrice_(nullptr) {};

DiscountCurve::DiscountCurve(const DateTime& referenceTime, std::map<double, double> data, const InterpolationMethod& interpolationMethod, const InterpolationVariable& dataType): 
TermStructure(referenceTime), yieldType_(std::nullopt), interpolationMethod_(interpolationMethod), 
nssYieldObject_(nullptr), interpolatedLogDiscountPrice_(getInterpolatedLogDiscountPrice(data,interpolationMethod,dataType)) {};

std::shared_ptr<CurveInterpolation> DiscountCurve::getInterpolatedLogDiscountPrice(std::map<double, double> data, const InterpolationMethod& interpolationMethod, const InterpolationVariable& dataType)
{
    std::map<double, double> logPrices; 
    double df;
    for (const auto& [k, v] : data)
    {
        if (k==0) break;
        if (k<0) throw QuantErrorRegistry::NegativeYearFractionError();
        switch (dataType)
        {
            case InterpolationVariable::ZC_CONTINUOUS_YIELD: df = getDiscountFactorContinuous(v,k);break;
            case InterpolationVariable::ZC_SIMPLE_YIELD: df = getDiscountFactorSimple(v,k);break;
            case InterpolationVariable::ZC_LOG_PRICE: df = std::exp(v);break;
            case InterpolationVariable::ZC_PRICE: df = v;break;
        }
        logPrices[k] = std::log(df);
    }
    switch (interpolationMethod)
    {
        case InterpolationMethod::CUBIC_SPLINE: return std::make_shared<CubicSpline>(logPrices);
        case InterpolationMethod::LINEAR : return std::make_shared<LinearInterpolation>(logPrices);
    }
}

std::optional<DiscountCurve::YieldType> DiscountCurve::getYieldType() const{return yieldType_;}
std::optional<DiscountCurve::InterpolationMethod> DiscountCurve::getInterpolationMethod() const{return interpolationMethod_;}

 double DiscountCurve::getEffectiveYearFraction(double t) const {
    if (!nssYieldObject_) return std::min(std::max(t, interpolatedLogDiscountPrice_->getLowerBoundX()), interpolatedLogDiscountPrice_->getUpperBoundX());
    else return t;
 }

double DiscountCurve::_getValue(double t) const
{
    // T can be equal to 0 here (safe)
    if (!nssYieldObject_)
    {
        return std::exp(interpolatedLogDiscountPrice_->evaluate(getEffectiveYearFraction(t)));
    }
    else
    {
        switch (yieldType_.value())
        {
        case YieldType::CONTINUOUS: return std::exp(-t*nssYieldObject_->getRate(t));
        case YieldType::SIMPLE: return 1.0/(1.0+t*nssYieldObject_->getRate(t));
        }
        return nssYieldObject_->getRate(t);
    }
}

double DiscountCurve::getShortRate() const
{

    if (!nssYieldObject_) return interpolatedLogDiscountPrice_->evaluate(getEffectiveYearFraction(0.0))/-getEffectiveYearFraction(0.0);
    else return nssYieldObject_->getBeta0()+nssYieldObject_->getBeta1();
}

double DiscountCurve::getSimpleRate(double t) const {checkYearFraction(t); return t==0.0 ? getShortRate() : (1/getValue(t)-1)/t;}
double DiscountCurve::getContinuousRate(double t) const {checkYearFraction(t); return t==0.0 ? getShortRate() : log(getValue(t))/-t;}
double DiscountCurve::getForwardValue(double t1, double t2) const{checkForwardYearFraction(t1,t2); return getValue(t2)/getValue(t1);}
double DiscountCurve::getInstantaneousForwardRate(double t) const
{
    checkYearFraction(t); 
    if (t==0.0) return getShortRate(); 
    if (!nssYieldObject_) return -interpolatedLogDiscountPrice_->evaluateFirstDerivative(getEffectiveYearFraction(t));
    else return nssYieldObject_->getInstantaneousForwardRate(t);

}
double DiscountCurve::getDerivativeInstantaneousForwardRate(double t) const
{
    checkYearFraction(t); 
    if (t==0.0) return 0.0; 
    if (!nssYieldObject_) return -interpolatedLogDiscountPrice_->evaluateSecondDerivative(getEffectiveYearFraction(t));
    else return nssYieldObject_->getDerivativeInstantaneousForwardRate(t);
}
double DiscountCurve::getSimpleForwardRate(double t1, double t2) const 
{
    checkForwardYearFraction(t1,t2); 
    double deltaT = t2-t1;
    if (deltaT == 0.0) return getInstantaneousForwardRate(t1);
    return (getValue(t1)/getValue(t2) - 1.0)/(t2-t1);
}
double DiscountCurve::getContinuousForwardRate(double t1, double t2) const 
{
    checkForwardYearFraction(t1,t2);
    double deltaT = t2-t1;
    if (deltaT == 0.0) return getInstantaneousForwardRate(t1);
    return log(getForwardValue(t1,t2))/-deltaT;
}
