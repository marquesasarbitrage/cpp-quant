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

        enum class InterpolationMethod {LINEAR, CUBIC_SPLINE}; 
        enum class InterpolationVariable {ZC_CONTINUOUS_YIELD, ZC_SIMPLE_YIELD, ZC_PRICE, ZC_LOG_PRICE}; 

        struct CurveParameters
        {
            CurveParameters(const Scheduler& scheduler, const std::set<Tenor>& tenorList, bool useSimpleRate, bool useSvensson, bool useForward); 
            CurveParameters(const Scheduler& scheduler, const std::set<Tenor>& tenorList, bool useSimpleRate, const InterpolationMethod& interpolationMethod);
            CurveParameters(const Scheduler& scheduler, const std::set<Tenor>& tenorList, bool useSimpleRate, bool useSvensson, bool useForward, double basisPointParralelBump);
            CurveParameters(const Scheduler& scheduler, const std::set<Tenor>& tenorList, bool useSimpleRate, const InterpolationMethod& interpolationMethod, double basisPointParralelBump);
            CurveParameters(const Scheduler& scheduler, const std::set<Tenor>& tenorList, bool useSimpleRate, bool useSvensson, bool useForward, const std::vector<double>& tenorMappedBasisPointBump);
            CurveParameters(const Scheduler& scheduler, const std::set<Tenor>& tenorList, bool useSimpleRate, const InterpolationMethod& interpolationMethod, const std::vector<double>& tenorMappedBasisPointBump);
            Scheduler scheduler_;
            std::set<Tenor> tenorList_; 
            double basisPointParralelBump_;
            std::vector<double> tenorMappedBasisPointBump_; 
            bool useForward_;
            bool useSvensson_; 
            bool useSimpleRate_;
            InterpolationMethod interpolationMethod_;
        };


        DiscountCurve(const DateTime& referenceTime, const std::shared_ptr<NelsonSiegelFamily>& nssYieldObject);
        DiscountCurve(const DateTime& referenceTime, const std::map<double, double>& data, const InterpolationMethod& interpolationMethod, const InterpolationVariable& dataType);
        DiscountCurve(const DateTime& referenceTime, const std::map<Tenor, double>& data, const Scheduler& scheduler, const InterpolationMethod& interpolationMethod, const InterpolationVariable& dataType);
        DiscountCurve(const DateTime& referenceTime, const std::map<DateTime, double>& data, const Scheduler& scheduler, const InterpolationMethod& interpolationMethod, const InterpolationVariable& dataType);
        ~DiscountCurve() = default;
        
        std::optional<InterpolationMethod> getInterpolationMethod() const;

        double getShortRate() const;
        double getInstantaneousForwardRate(double t) const;
        double getDerivativeInstantaneousForwardRate(double t) const;
        double getForwardValue(double t1, double t2) const; 
        double getSimpleRate(double t) const;
        double getContinuousRate(double t) const;
        double getSimpleForwardRate(double t1, double t2) const;
        double getContinuousForwardRate(double t1, double t2) const;

        DiscountCurve getInterpolatedCurve(const CurveParameters& curveParameters) const;
        DiscountCurve getNelsonSiegelCurve(const CurveParameters& curveParameters) const;

    protected: 
        virtual double _getValue(double t) const override;
    
    private: 
        const std::optional<InterpolationMethod> interpolationMethod_;
        const std::shared_ptr<NelsonSiegelFamily> nssYieldObject_; 
        const std::shared_ptr<CurveInterpolation> interpolatedLogDiscountPrice_; 

        std::shared_ptr<CurveInterpolation> getInterpolatedLogDiscountPrice(const std::map<double, double>& data, const InterpolationMethod& interpolationMethod, const InterpolationVariable& dataType) const;
        std::shared_ptr<CurveInterpolation> getInterpolatedLogDiscountPrice(const std::map<Tenor, double>& data, const Scheduler& scheduler, const InterpolationMethod& interpolationMethod, const InterpolationVariable& dataType) const;
        std::shared_ptr<CurveInterpolation> getInterpolatedLogDiscountPrice(const std::map<DateTime, double>& data, const Scheduler& scheduler, const InterpolationMethod& interpolationMethod, const InterpolationVariable& dataType) const;
};




