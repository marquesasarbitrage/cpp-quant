#include "../../../include/cpp-quant/valuation/termstructure/discountcurve.hpp"

DiscountCurve::DiscountCurve(const DateTime& referenceTime, const std::shared_ptr<NelsonSiegelFamily>& nssYieldObject): 
TermStructure(referenceTime), interpolationMethod_(std::nullopt), nssYieldObject_(nssYieldObject), interpolatedLogDiscountPrice_(nullptr) {};

DiscountCurve::DiscountCurve(const DateTime& referenceTime, const std::map<double, double>& data, const InterpolationMethod& interpolationMethod, const InterpolationVariable& dataType): 
TermStructure(referenceTime), interpolationMethod_(interpolationMethod), 
nssYieldObject_(nullptr), interpolatedLogDiscountPrice_(getInterpolatedLogDiscountPrice(data,interpolationMethod,dataType)) {};

DiscountCurve::DiscountCurve(const DateTime& referenceTime, const std::map<Tenor, double>& data, const Scheduler& scheduler, const InterpolationMethod& interpolationMethod, const InterpolationVariable& dataType):
TermStructure(referenceTime), interpolationMethod_(interpolationMethod), 
nssYieldObject_(nullptr), interpolatedLogDiscountPrice_(getInterpolatedLogDiscountPrice(data,scheduler,interpolationMethod,dataType)) {};

DiscountCurve::DiscountCurve(const DateTime& referenceTime, const std::map<DateTime, double>& data, const Scheduler& scheduler, const InterpolationMethod& interpolationMethod, const InterpolationVariable& dataType):
TermStructure(referenceTime), interpolationMethod_(interpolationMethod), 
nssYieldObject_(nullptr), interpolatedLogDiscountPrice_(getInterpolatedLogDiscountPrice(data,scheduler,interpolationMethod,dataType)) {};

std::shared_ptr<CurveInterpolation> DiscountCurve::getInterpolatedLogDiscountPrice(const std::map<double, double>& data, const InterpolationMethod& interpolationMethod, const InterpolationVariable& dataType) const
{
    std::map<double, double> logPrices; 
    double df;
    for (const auto& [k, v] : data)
    {
        if (k==0) break;
        if (k<0) throw QuantErrorRegistry::NegativeYearFractionError();
        switch (dataType)
        {
            case InterpolationVariable::ZC_CONTINUOUS_YIELD: df = std::exp(-k*v);break;
            case InterpolationVariable::ZC_SIMPLE_YIELD: df = 1.0/(1+v*k);break;
            case InterpolationVariable::ZC_LOG_PRICE: df = std::exp(v);break;
            case InterpolationVariable::ZC_PRICE: df = v;break;
        }
        logPrices[k] = std::log(df);
    }
    logPrices[0.0] = 0.0;
    switch (interpolationMethod)
    {
        case InterpolationMethod::CUBIC_SPLINE: return std::make_shared<CubicSpline>(logPrices);
        case InterpolationMethod::LINEAR : return std::make_shared<LinearInterpolation>(logPrices);
    }
}

std::shared_ptr<CurveInterpolation> DiscountCurve::getInterpolatedLogDiscountPrice(const std::map<Tenor, double>& data, const Scheduler& scheduler, const InterpolationMethod& interpolationMethod, const InterpolationVariable& dataType) const
{
    std::map<double, double> output; 
    for (const auto& d: data)
    {
        DateTime fwdDate = scheduler.getForwardDateTime(getReferenceTime(), d.first);
        double t = scheduler.getYearFraction(getReferenceTime(), fwdDate);
        output[t] = d.second;
    }
    return getInterpolatedLogDiscountPrice(output,interpolationMethod,dataType);
}

std::shared_ptr<CurveInterpolation> DiscountCurve::getInterpolatedLogDiscountPrice(const std::map<DateTime, double>& data, const Scheduler& scheduler, const InterpolationMethod& interpolationMethod, const InterpolationVariable& dataType) const
{
    std::map<double, double> output; 
    for (const auto& d: data)
    {
        double t = scheduler.getYearFraction(getReferenceTime(), d.first);
        output[t] = d.second;
    }
    return getInterpolatedLogDiscountPrice(output,interpolationMethod,dataType);
}

std::optional<DiscountCurve::InterpolationMethod> DiscountCurve::getInterpolationMethod() const{return interpolationMethod_;}

double DiscountCurve::_getValue(double t) const
{
    // T can be equal to 0 here (safe)
    if (!nssYieldObject_) {
        double tMax = interpolatedLogDiscountPrice_->getUpperBoundX();
        if (t<=tMax) return std::exp(interpolatedLogDiscountPrice_->evaluate(t));
        else {
            double simpleRate = (1.0/std::exp(interpolatedLogDiscountPrice_->evaluate(tMax)) -1.0)/tMax;
            return 1.0/(1.0 + simpleRate*t);
        }
    }
    else return std::exp(-t*nssYieldObject_->getRate(t));
}

double DiscountCurve::getShortRate() const
{
    if (!nssYieldObject_) {
        double tMin = interpolatedLogDiscountPrice_->getLowerBoundX();
        double impliedContinuousRate = interpolatedLogDiscountPrice_->evaluate(tMin)/-tMin; 
        double impliedSimpleRate = (1.0/std::exp(interpolatedLogDiscountPrice_->evaluate(tMin)) - 1.0)/tMin;
        return .5*(impliedContinuousRate+impliedSimpleRate);
    }
    else return nssYieldObject_->getBeta0()+nssYieldObject_->getBeta1();
}

double DiscountCurve::getSimpleRate(double t) const {checkYearFraction(t); return t==0.0 ? getShortRate() : (1/getValue(t)-1)/t;}

double DiscountCurve::getContinuousRate(double t) const {checkYearFraction(t); return t==0.0 ? getShortRate() : log(getValue(t))/-t;}

double DiscountCurve::getForwardValue(double t1, double t2) const{checkForwardYearFraction(t1,t2); return getValue(t2)/getValue(t1);}

double DiscountCurve::getInstantaneousForwardRate(double t) const
{
    checkYearFraction(t); 
    if (t==0.0) return getShortRate(); 
    if (!nssYieldObject_) {
        double tMax = interpolatedLogDiscountPrice_->getUpperBoundX();
        if (t>tMax) {
            double epsilon = 0.01;
            return - (std::log(getValue(t + epsilon)) - std::log(getValue(t)))/epsilon; 
        }
        else return -interpolatedLogDiscountPrice_->evaluateFirstDerivative(t);
    }
    else return nssYieldObject_->getInstantaneousForwardRate(t);

}

double DiscountCurve::getDerivativeInstantaneousForwardRate(double t) const
{
    checkYearFraction(t); 
    if (t==0.0) return 0.0; 
    if (!nssYieldObject_) {
        double tMax = interpolatedLogDiscountPrice_->getUpperBoundX();
        if (t>tMax) {
            double epsilon = 0.01;
            return -(getInstantaneousForwardRate(t + epsilon) - getInstantaneousForwardRate(t))/epsilon; 
        }
        else return -interpolatedLogDiscountPrice_->evaluateSecondDerivative(t);
    }
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

DiscountCurve::CurveParameters::CurveParameters(const Scheduler& scheduler, const std::set<Tenor>& tenorList, bool useSimpleRate, bool useSvensson, bool useForward): 
scheduler_(scheduler), tenorList_(tenorList), basisPointParralelBump_(0.0), tenorMappedBasisPointBump_(std::vector<double>(tenorList.size())), 
useForward_(useForward), useSvensson_(useSvensson),useSimpleRate_(useSimpleRate), interpolationMethod_(InterpolationMethod::CUBIC_SPLINE){};

DiscountCurve::CurveParameters::CurveParameters(const Scheduler& scheduler, const std::set<Tenor>& tenorList, bool useSimpleRate, const InterpolationMethod& interpolationMethod):
scheduler_(scheduler), tenorList_(tenorList), basisPointParralelBump_(0.0), tenorMappedBasisPointBump_(std::vector<double>(tenorList.size())), 
useForward_(false),useSvensson_(false),useSimpleRate_(useSimpleRate), interpolationMethod_(interpolationMethod){};

DiscountCurve::CurveParameters::CurveParameters(const Scheduler& scheduler, const std::set<Tenor>& tenorList, bool useSimpleRate, bool useSvensson, bool useForward, double basisPointParralelBump):
scheduler_(scheduler), tenorList_(tenorList), basisPointParralelBump_(basisPointParralelBump), tenorMappedBasisPointBump_(std::vector<double>(tenorList.size())), 
useForward_(useForward),useSvensson_(useSvensson),useSimpleRate_(useSimpleRate), interpolationMethod_(InterpolationMethod::CUBIC_SPLINE){};

DiscountCurve::CurveParameters::CurveParameters(const Scheduler& scheduler, const std::set<Tenor>& tenorList, bool useSimpleRate, const InterpolationMethod& interpolationMethod, double basisPointParralelBump):
scheduler_(scheduler), tenorList_(tenorList), basisPointParralelBump_(basisPointParralelBump), tenorMappedBasisPointBump_(std::vector<double>(tenorList.size())), 
useForward_(false),useSvensson_(false),useSimpleRate_(useSimpleRate), interpolationMethod_(interpolationMethod){};

DiscountCurve::CurveParameters::CurveParameters(const Scheduler& scheduler, const std::set<Tenor>& tenorList, bool useSimpleRate, bool useSvensson, bool useForward, const std::vector<double>& tenorMappedBasisPointBump_):
scheduler_(scheduler), tenorList_(tenorList), basisPointParralelBump_(0.0), tenorMappedBasisPointBump_(tenorMappedBasisPointBump_), 
useForward_(useForward),useSvensson_(useSvensson),useSimpleRate_(useSimpleRate), interpolationMethod_(InterpolationMethod::CUBIC_SPLINE){if (tenorMappedBasisPointBump_.size() != tenorList_.size()) throw QuantErrorRegistry::TermStructure::DiscountCurve::MismatchTenorBumpSizeError();};

DiscountCurve::CurveParameters::CurveParameters(const Scheduler& scheduler, const std::set<Tenor>& tenorList, bool useSimpleRate, const InterpolationMethod& interpolationMethod, const std::vector<double>& tenorMappedBasisPointBump_):
scheduler_(scheduler), tenorList_(tenorList), basisPointParralelBump_(0.0), tenorMappedBasisPointBump_(tenorMappedBasisPointBump_), 
useForward_(false),useSvensson_(false),useSimpleRate_(useSimpleRate), interpolationMethod_(interpolationMethod){if (tenorMappedBasisPointBump_.size() != tenorList_.size()) throw QuantErrorRegistry::TermStructure::DiscountCurve::MismatchTenorBumpSizeError();};

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

DiscountCurve DiscountCurve::getNelsonSiegelCurve(const DiscountCurve::CurveParameters& curveParameters) const
{
    DiscountCurve interpolatedCurve = getInterpolatedCurve(curveParameters); 
    std::map<double, double> data; 
    for (const Tenor& tenor: curveParameters.tenorList_)
    {
        DateTime fwdDate = curveParameters.scheduler_.getForwardDateTime(getReferenceTime(),tenor);
        double t = curveParameters.scheduler_.getYearFraction(getReferenceTime(), fwdDate);
        data[t] = curveParameters.useForward_ ? interpolatedCurve.getInstantaneousForwardRate(t) : interpolatedCurve.getContinuousRate(t) ;
    }

    NelsonSiegelCalibration nsCalibration = NelsonSiegelCalibration(data,!(curveParameters.useForward_));
    nsCalibration.setGridSize(1.0);
    DateTime dt = getReferenceTime();
    std::shared_ptr<NelsonSiegelFamily> nss = nullptr;
    if (curveParameters.useSvensson_) return DiscountCurve(dt,nsCalibration.fitSvensson());
    else return DiscountCurve(dt,nsCalibration.fitNelsonSiegel());   
}


