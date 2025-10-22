#pragma once 
#include <map>
#include <set>
#include <optional>
#include "../../../../../include/cpp-quant/valuation/marketdata/marketdata.hpp"
#include "../../../../../include/cpp-quant/tools/nss.hpp"
#include "cpp-math/curveinterpolation.hpp"

class DiscountCurve final: public TermStructure
{
    public: 

        enum class InterpolationMethod {LINEAR = 0, CUBIC_SPLINE = 1}; 
        enum class InterpolationVariable {ZC_CONTINUOUS_YIELD, ZC_SIMPLE_YIELD, ZC_PRICE, ZC_LOG_PRICE}; 

        struct CurveParameters
        {
            CurveParameters(const Scheduler& scheduler, const std::set<Tenor>& tenorList, bool useSimpleRate, const InterpolationMethod& interpolationMethod);
            CurveParameters(const Scheduler& scheduler, const std::set<Tenor>& tenorList, bool useSimpleRate, const InterpolationMethod& interpolationMethod, double basisPointParralelBump);
            CurveParameters(const Scheduler& scheduler, const std::set<Tenor>& tenorList, bool useSimpleRate, const InterpolationMethod& interpolationMethod, const std::vector<double>& tenorMappedBasisPointBump);
            Scheduler scheduler_;
            std::set<Tenor> tenorList_; 
            double basisPointParralelBump_;
            std::vector<double> tenorMappedBasisPointBump_; 
            bool useSimpleRate_;
            InterpolationMethod interpolationMethod_;
        };

        DiscountCurve(const DateTime& referenceTime, const std::shared_ptr<Svensson>& nssYieldObject);
        DiscountCurve(const DateTime& referenceTime, const std::map<double, double>& data, const InterpolationMethod& interpolationMethod, const InterpolationVariable& dataType);
        DiscountCurve(const DateTime& referenceTime, const std::map<Tenor, double>& data, const Scheduler& scheduler, const InterpolationMethod& interpolationMethod, const InterpolationVariable& dataType);
        DiscountCurve(const DateTime& referenceTime, const std::map<DateTime, double>& data, const Scheduler& scheduler, const InterpolationMethod& interpolationMethod, const InterpolationVariable& dataType);
        ~DiscountCurve() = default;
        
        std::optional<InterpolationMethod> getInterpolationMethod() const;
        std::shared_ptr<Svensson> getSvenssonObject() const;

        double getShortRate() const;
        double getInstantaneousForwardRate(double t) const;
        double getDerivativeInstantaneousForwardRate(double t) const;
        double getForwardValue(double t1, double t2) const; 
        double getSimpleRate(double t) const;
        double getContinuousRate(double t) const;
        double getSimpleForwardRate(double t1, double t2) const;
        double getContinuousForwardRate(double t1, double t2) const;

        DiscountCurve getInterpolatedCurve(const CurveParameters& curveParameters) const;
        DiscountCurve getSvenssonCurve(const CurveParameters& curveParameters) const;
        DiscountCurve getSvenssonCurve() const;

    protected: 
        virtual double _getValue(double t) const override;
    
    private: 
        bool useInterpolation_;
        std::optional<InterpolationMethod> interpolationMethod_;
        std::shared_ptr<Svensson> svenssonYieldObject_; 
        std::shared_ptr<CurveInterpolation> interpolatedLogDiscountPrice_; 
        double calibrationTime_;

        void classSetter(const std::map<double, double>& data, const InterpolationMethod& interpolationMethod, const InterpolationVariable& dataType);
        void classSetter(const std::map<Tenor, double>& data, const Scheduler& scheduler, const InterpolationMethod& interpolationMethod, const InterpolationVariable& dataType);
        void classSetter(const std::map<DateTime, double>& data, const Scheduler& scheduler, const InterpolationMethod& interpolationMethod, const InterpolationVariable& dataType);

};




