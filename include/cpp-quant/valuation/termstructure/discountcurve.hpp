#pragma once 
#include <map>
#include <set>
#include <optional>
#include "termstructure.hpp"
#include "../../../../include/cpp-quant/tools/nss.hpp"
#include "cpp-math/curveinterpolation.hpp"

class DiscountCurve final: public TermStructure
{
    public: 

        enum class YieldType {CONTINUOUS, SIMPLE};
        enum class InterpolationMethod {LINEAR, CUBIC_SPLINE}; 
        enum class InterpolationVariable {ZC_CONTINUOUS_YIELD, ZC_SIMPLE_YIELD, ZC_PRICE, ZC_LOG_PRICE}; 

        DiscountCurve(const DateTime& referenceTime, std::shared_ptr<NelsonSiegel> nssYieldObject, const YieldType& yieldType);
        DiscountCurve(const DateTime& referenceTime, std::map<double, double> data, const InterpolationMethod& interpolationMethod, const InterpolationVariable& dataType);
        //DiscountCurve(const DateTime& referenceTime, std::map<Tenor, double> data, const YieldCurveInterpolationMethod& method);
        //DiscountCurve(const DateTime& referenceTime, std::map<DateTime, double> data, const YieldCurveInterpolationMethod& method);
        ~DiscountCurve() = default;
        
        //double getImpliedShortRate() const;

        std::optional<YieldType> getYieldType() const;
        std::optional<InterpolationMethod> getInterpolationMethod() const;

        double getShortRate() const;
        double getInstantaneousForwardRate(double t) const;
        double getDerivativeInstantaneousForwardRate(double t) const;

        double getForwardValue(double t1, double t2) const; 

        double getSimpleRate(double t) const;
        double getContinuousRate(double t) const;
        
        double getSimpleForwardRate(double t1, double t2) const;
        double getContinuousForwardRate(double t1, double t2) const;


        //DiscountCurve getNelsonSiegelCurve() const; 
        //DiscountCurve getInterpolatedCurve() const; 

        //DiscountCurve getNelsonSiegelParralelStress(double bump); 
        //DiscountCurve getInterpolatedParralelStress(double bump); 

        //bool isArbitrageFreeProxy();
    
    protected: 
        virtual double _getValue(double t) const override;
    
    private: 
        const std::optional<YieldType> yieldType_; 
        const std::optional<InterpolationMethod> interpolationMethod_;
        std::shared_ptr<NelsonSiegel> nssYieldObject_; 
        std::shared_ptr<CurveInterpolation> interpolatedLogDiscountPrice_; 

        double getEffectiveYearFraction(double t) const;
        static double getDiscountFactorSimple(double yield, double t);
        static double getDiscountFactorContinuous(double yield, double t);
        std::shared_ptr<CurveInterpolation> getInterpolatedLogDiscountPrice(std::map<double, double> data, const InterpolationMethod& interpolationMethod, const InterpolationVariable& dataType);
        
};




