#include "../../../../include/cpp-quant/valuation/marketdata/termstructure/discountcurve.hpp"

DiscountCurve::DiscountCurve(const DateTime& referenceTime, const std::shared_ptr<Svensson>& nssYieldObject): 
TermStructure(referenceTime), useInterpolation_(false), interpolationMethod_(std::nullopt), 
svenssonYieldObject_(nssYieldObject), interpolatedLogDiscountPrice_(nullptr), calibrationTime_(0.0) {};

DiscountCurve::DiscountCurve(const DateTime& referenceTime, const std::map<double, double>& data, const InterpolationMethod& interpolationMethod, const InterpolationVariable& dataType): 
TermStructure(referenceTime), useInterpolation_(true), interpolationMethod_(interpolationMethod), 
svenssonYieldObject_(nullptr), interpolatedLogDiscountPrice_(nullptr), calibrationTime_(0.0)  {classSetter(data,interpolationMethod,dataType);}

DiscountCurve::DiscountCurve(const DateTime& referenceTime, const std::map<Tenor, double>& data, const Scheduler& scheduler, const InterpolationMethod& interpolationMethod, const InterpolationVariable& dataType):
TermStructure(referenceTime), useInterpolation_(true), interpolationMethod_(interpolationMethod), 
svenssonYieldObject_(nullptr), interpolatedLogDiscountPrice_(nullptr), calibrationTime_(0.0)  {classSetter(data,scheduler,interpolationMethod,dataType);}

DiscountCurve::DiscountCurve(const DateTime& referenceTime, const std::map<DateTime, double>& data, const Scheduler& scheduler, const InterpolationMethod& interpolationMethod, const InterpolationVariable& dataType):
TermStructure(referenceTime), useInterpolation_(true), interpolationMethod_(interpolationMethod), 
svenssonYieldObject_(nullptr), interpolatedLogDiscountPrice_(nullptr), calibrationTime_(0.0)  {classSetter(data,scheduler,interpolationMethod,dataType);}

std::optional<DiscountCurve::InterpolationMethod> DiscountCurve::getInterpolationMethod() const{return interpolationMethod_;}

std::shared_ptr<Svensson> DiscountCurve::getSvenssonObject() const {return svenssonYieldObject_;}

double DiscountCurve::_getValue(double t) const
{
    // T can be equal to 0 here (safe)
    if (useInterpolation_) {
        double tMax = interpolatedLogDiscountPrice_->getUpperBoundX();
        if (t<=tMax) return std::exp(interpolatedLogDiscountPrice_->evaluate(t));
        else return std::exp(-t*svenssonYieldObject_->getRate(t));
    }
    else return std::exp(-t*svenssonYieldObject_->getRate(t));
}

double DiscountCurve::getShortRate() const {return svenssonYieldObject_->getBeta0()+svenssonYieldObject_->getBeta1();}

double DiscountCurve::getSimpleRate(double t) const {checkYearFraction(t); return t==0.0 ? getShortRate() : (1/getValue(t)-1)/t;}

double DiscountCurve::getContinuousRate(double t) const {checkYearFraction(t); return t==0.0 ? getShortRate() : log(getValue(t))/-t;}

double DiscountCurve::getForwardValue(double t1, double t2) const{checkForwardYearFraction(t1,t2); return getValue(t2)/getValue(t1);}

double DiscountCurve::getInstantaneousForwardRate(double t) const
{
    checkYearFraction(t); 
    if (t==0.0) return getShortRate(); 
    if (useInterpolation_) {
        double tMax = interpolatedLogDiscountPrice_->getUpperBoundX();
        if (t>tMax) return svenssonYieldObject_->getInstantaneousForwardRate(t);
        else return -interpolatedLogDiscountPrice_->evaluateFirstDerivative(t);
    }
    else return svenssonYieldObject_->getInstantaneousForwardRate(t);

}

double DiscountCurve::getDerivativeInstantaneousForwardRate(double t) const
{
    checkYearFraction(t); 
    if (t==0.0) return 0.0; 
    if (useInterpolation_) {
        double tMax = interpolatedLogDiscountPrice_->getUpperBoundX();
        if (t>tMax) return svenssonYieldObject_->getDerivativeInstantaneousForwardRate(t);
        else return -interpolatedLogDiscountPrice_->evaluateSecondDerivative(t);
    }
    else return svenssonYieldObject_->getDerivativeInstantaneousForwardRate(t);
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

DiscountCurve::CurveParameters::CurveParameters(const Scheduler& scheduler, const std::set<Tenor>& tenorList, bool useSimpleRate, const InterpolationMethod& interpolationMethod):
scheduler_(scheduler), tenorList_(tenorList), basisPointParralelBump_(0.0), tenorMappedBasisPointBump_(std::vector<double>(tenorList.size())), 
useSimpleRate_(useSimpleRate), interpolationMethod_(interpolationMethod){};

DiscountCurve::CurveParameters::CurveParameters(const Scheduler& scheduler, const std::set<Tenor>& tenorList, bool useSimpleRate, const InterpolationMethod& interpolationMethod, double basisPointParralelBump):
scheduler_(scheduler), tenorList_(tenorList), basisPointParralelBump_(basisPointParralelBump), tenorMappedBasisPointBump_(std::vector<double>(tenorList.size())), 
useSimpleRate_(useSimpleRate), interpolationMethod_(interpolationMethod){};

DiscountCurve::CurveParameters::CurveParameters(const Scheduler& scheduler, const std::set<Tenor>& tenorList, bool useSimpleRate, const InterpolationMethod& interpolationMethod, const std::vector<double>& tenorMappedBasisPointBump_):
scheduler_(scheduler), tenorList_(tenorList), basisPointParralelBump_(0.0), tenorMappedBasisPointBump_(tenorMappedBasisPointBump_), 
useSimpleRate_(useSimpleRate), interpolationMethod_(interpolationMethod){if (tenorMappedBasisPointBump_.size() != tenorList_.size()) throw QuantErrorRegistry::Valuation::MarketData::TermStructure::DiscountCurve::MismatchTenorBumpSizeError();};

DiscountCurve DiscountCurve::getInterpolatedCurve(const DiscountCurve::CurveParameters& curveParameters) const
{
    std::map<double, double> data; 
    int i = 0;
    for (const Tenor& tenor: curveParameters.tenorList_)
    {
        DateTime fwdDate = curveParameters.scheduler_.getForwardDateTime(getReferenceTime(),tenor);
        double t = curveParameters.scheduler_.getYearFraction(getReferenceTime(), fwdDate);
        double rate = curveParameters.useSimpleRate_ ? getSimpleRate(t) : getContinuousRate(t);
        data[t] = rate + (curveParameters.basisPointParralelBump_ + curveParameters.tenorMappedBasisPointBump_[i])/10000.0;
        i++;
    }
    InterpolationVariable interpolationVariable = curveParameters.useSimpleRate_ ? InterpolationVariable::ZC_SIMPLE_YIELD : InterpolationVariable::ZC_CONTINUOUS_YIELD;
    return DiscountCurve(getReferenceTime(), data, curveParameters.interpolationMethod_, interpolationVariable);
}

DiscountCurve DiscountCurve::getSvenssonCurve(const DiscountCurve::CurveParameters& curveParameters) const
{
    DiscountCurve interpolatedCurve = getInterpolatedCurve(curveParameters); 
    return DiscountCurve(getReferenceTime(), interpolatedCurve.getSvenssonObject());
}

DiscountCurve DiscountCurve::getSvenssonCurve() const
{
    return DiscountCurve(getReferenceTime(), getSvenssonObject());
}

void DiscountCurve::classSetter(const std::map<double, double>& data, const InterpolationMethod& interpolationMethod, const InterpolationVariable& dataType)
{
    auto start = std::chrono::high_resolution_clock::now();
    std::map<double, double> logPrices; 
    std::map<double, double> continuousYields; 
    double df, i;
    for (const auto& [k, v] : data)
    {
        if (k==0) break;
        if (k<0) throw QuantErrorRegistry::NegativeYearFractionError();
        switch (dataType)
        {
            case InterpolationVariable::ZC_CONTINUOUS_YIELD: df = std::exp(-k*v);break;
            case InterpolationVariable::ZC_SIMPLE_YIELD: df = 1.0/(1+v*k);break;
            case InterpolationVariable::ZC_LOG_PRICE: df = std::exp(v);break;
            case InterpolationVariable::ZC_PRICE: df = v; break;
        }
        logPrices[k] = std::log(df);
        continuousYields[k] = std::log(df)/-k;
    }
    logPrices[0.0] = 0.0;
    NelsonSiegelCalibration nsCalib(continuousYields,true); 
    nsCalib.setGridSize(1.5);
    svenssonYieldObject_ = std::dynamic_pointer_cast<Svensson>(nsCalib.fitSvensson());
    interpolationMethod_ = interpolationMethod;
    switch (interpolationMethod_.value())
    {
        case InterpolationMethod::CUBIC_SPLINE: interpolatedLogDiscountPrice_ = std::make_shared<CubicSpline>(logPrices);break;
        case InterpolationMethod::LINEAR : interpolatedLogDiscountPrice_ = std::make_shared<LinearInterpolation>(logPrices);break;
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    calibrationTime_ = elapsed.count();
}

void DiscountCurve::classSetter(const std::map<Tenor, double>& data, const Scheduler& scheduler, const InterpolationMethod& interpolationMethod, const InterpolationVariable& dataType) 
{
    std::map<double, double> output; 
    for (const auto& d: data)
    {
        DateTime fwdDate = scheduler.getForwardDateTime(getReferenceTime(), d.first);
        double t = scheduler.getYearFraction(getReferenceTime(), fwdDate);
        output[t] = d.second;
    }
    classSetter(output,interpolationMethod,dataType);
}

void DiscountCurve::classSetter(const std::map<DateTime, double>& data, const Scheduler& scheduler, const InterpolationMethod& interpolationMethod, const InterpolationVariable& dataType) 
{
    std::map<double, double> output; 
    for (const auto& d: data)
    {
        double t = scheduler.getYearFraction(getReferenceTime(), d.first);
        output[t] = d.second;
    }
    classSetter(output,interpolationMethod,dataType);
}

double DiscountCurve::getYearFraction(const Tenor& tenor, const Scheduler& scheduler) const
{
    return scheduler.getYearFraction(getReferenceTime(), scheduler.getForwardDateTime(getReferenceTime(),tenor));
}
double DiscountCurve::getYearFraction(const DateTime& referenceTime, const Scheduler& scheduler) const
{
    return scheduler.getYearFraction(getReferenceTime(), referenceTime);
}

double DiscountCurve::getInstantaneousForwardRate(const Tenor& tenor, const Scheduler& scheduler) const 
{
    return getInstantaneousForwardRate(getYearFraction(tenor,scheduler));
}

double DiscountCurve::getDerivativeInstantaneousForwardRate(const Tenor& tenor, const Scheduler& scheduler) const
{
    return getDerivativeInstantaneousForwardRate(getYearFraction(tenor,scheduler));
}

double DiscountCurve::getForwardValue(const Tenor& startTenor, const Tenor& endTenor, const Scheduler& scheduler) const
{
    return getForwardValue(getYearFraction(startTenor,scheduler), getYearFraction(endTenor,scheduler));
}

double DiscountCurve::getSimpleRate(const Tenor& tenor, const Scheduler& scheduler) const
{
    return getSimpleRate(getYearFraction(tenor,scheduler));
}

double DiscountCurve::getContinuousRate(const Tenor& tenor, const Scheduler& scheduler) const
{
    return getContinuousRate(getYearFraction(tenor,scheduler));
}

double DiscountCurve::getSimpleForwardRate(const Tenor& startTenor, const Tenor& endTenor, const Scheduler& scheduler) const
{
    return getSimpleForwardRate(getYearFraction(startTenor,scheduler), getYearFraction(endTenor,scheduler));
}

double DiscountCurve::getContinuousForwardRate(const Tenor& startTenor, const Tenor& endTenor, const Scheduler& scheduler) const
{
    return getContinuousForwardRate(getYearFraction(startTenor,scheduler), getYearFraction(endTenor,scheduler));
}

double DiscountCurve::getInstantaneousForwardRate(const DateTime& referenceTime, const Scheduler& scheduler) const{ return getInstantaneousForwardRate(getYearFraction(referenceTime,scheduler));}

double DiscountCurve::getDerivativeInstantaneousForwardRate(const DateTime& referenceTime, const Scheduler& scheduler) const  const{ return getDerivativeInstantaneousForwardRate(getYearFraction(referenceTime,scheduler));}

double DiscountCurve::getForwardValue(const DateTime& startTime, const DateTime& endTime, const Scheduler& scheduler) const
{
    return getContinuousForwardRate(getYearFraction(startTime,scheduler), getYearFraction(endTime,scheduler));
}

double DiscountCurve::getSimpleRate(const DateTime& referenceTime, const Scheduler& scheduler) const const{ return getSimpleRate(getYearFraction(referenceTime,scheduler));}
double DiscountCurve::getContinuousRate(const DateTime& referenceTime, const Scheduler& scheduler) const const{ return getContinuousRate(getYearFraction(referenceTime,scheduler));}
double DiscountCurve::getSimpleForwardRate(const DateTime& startTime, const DateTime& endTime, const Scheduler& scheduler) const
{
    return getSimpleForwardRate(getYearFraction(startTime,scheduler), getYearFraction(endTime,scheduler));
}

double DiscountCurve::getContinuousForwardRate(const DateTime& startTime, const DateTime& endTime, const Scheduler& scheduler) const
{
    return getContinuousForwardRate(getYearFraction(startTime,scheduler), getYearFraction(endTime,scheduler));
}




