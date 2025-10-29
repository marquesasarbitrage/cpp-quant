#include <cassert>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <iomanip> 
#include <filesystem>
#include "../../../../include/cpp-quant/valuation/marketdata/termstructures/discountcurve.hpp"

// Zero yields data of Bank of Canada as of September 24th, 2025 (https://www.bankofcanada.ca/rates/interest-rates/bond-yield-curves/)
namespace CanadianZeroYieldData
{
    DateTime REFERENCE_DATE = DateTime(1758704936, EpochTimestampType::SECONDS);

    DiscountCurve::InterpolationVariable interpolationVariable = DiscountCurve::InterpolationVariable::ZC_SIMPLE_YIELD;

    std::map<double, double> getData()
    {
        std::vector<double> rawData = {
            0.0242277000,0.0240667000,0.0239455000,0.0239879000,	
            0.0242138000,0.0242958000,0.0244047000,0.0245381000,	
            0.0246938000,0.0248696000,0.0250637000,0.0252740000,
            0.0254988000,0.0257363000,0.0259849000,0.0262429000,	
            0.0265089000,0.0267814000,0.0270591000,0.0273407000,	
            0.0276251000,0.0279111000,0.0281979000,0.0284843000,
            0.0287696000,0.0290529000,0.0293336000,0.0296110000,	
            0.0298844000,0.0301535000,0.0304176000,0.0306763000,	
            0.0309294000,0.0311764000,0.0314171000,0.0316513000,
            0.0318788000,0.0320994000,0.0323131000,0.0325197000,	
            0.0327192000,0.0329116000,0.0330970000,0.0332752000,
            0.0334465000,0.0336109000,0.0337684000,0.0339193000,
            0.0340636000,0.0342015000,0.0343332000,0.0344587000,
            0.0345784000,0.0346924000,0.0348009000,0.0349041000,
            0.0350022000,0.0350955000,0.0351840000,0.0352681000,
            0.0353480000,0.0354238000,0.0354958000,0.0355642000,
            0.0356293000,0.0356911000,0.0357499000,0.0358059000,
            0.0358593000,0.0359103000,0.0359590000,0.0360057000,
            0.0360504000,0.0360934000,0.0361347000,0.0361746000,
            0.0362132000,0.0362506000,0.0362869000,0.0363222000,
            0.0363567000,0.0363904000,0.0364235000,0.0364560000,
            0.0364880000,0.0365196000,0.0365508000,0.0365818000,
            0.0366124000,0.0366429000,0.0366732000,0.0367033000,
            0.0367334000,0.0367633000,0.0367932000,0.0368230000,
            0.0368527000,0.0368823000,0.0369119000,0.0369414000,
            0.0369708000,0.0370000000,0.0370291000,0.0370580000,
            0.0370868000,0.0371152000,0.0371434000,0.0371713000,
            0.0371988000,0.0372260000,0.0372526000,0.0372787000,
            0.0373043000,0.0373293000,0.0373535000,0.0373771000,
            0.0373998000,0.0374217000,0.0374427000,0.0374627000
        };
        std::map<double, double> data; 
        double t = 0; 
        for (const double y: rawData){t+=.25;data[t] = y;}
        return data;
    }

    std::map<Tenor, double> getTenorMapped()
    {
        std::map<double, double> rawData = getData();
        std::map<Tenor, double> data; 
        Scheduler scheduler = Scheduler(DayCountConvention::ACTUAL_360);
        double m = 0, t; 
        for (const auto& y: rawData)
        {
            m+=3;
            t = scheduler.getYearFraction(REFERENCE_DATE,Tenor(m,TenorType::MONTHS));
            data[Tenor(m,TenorType::MONTHS)] = y.second;
        }
        return data;
    }

    std::map<DateTime, double> getDatetimeMapped()
    {
        std::map<double, double> rawData = getData();
        std::map<DateTime, double> data; 
        DateTime dt = REFERENCE_DATE;
        Scheduler scheduler = Scheduler(DayCountConvention::ACTUAL_360);
        double t;
        for (const auto& y: rawData)
        {
            dt = Tenor(3,TenorType::MONTHS).getForwardDate(dt);
            t = scheduler.getYearFraction(REFERENCE_DATE,dt);
            data[dt] = y.second;
        }
        return data;
    }

};

// Daily Treasury Par Yield Curve Rates as of October 17th, 2025 (https://home.treasury.gov/resource-center/data-chart-center/interest-rates/TextView?type=daily_treasury_yield_curve&field_tdr_date_value=2025)
namespace USTreasuryParYieldData 
{
    DateTime REFERENCE_DATE = DateTime(1760721344, EpochTimestampType::SECONDS);

    DiscountCurve::InterpolationVariable interpolationVariable = DiscountCurve::InterpolationVariable::ZC_SIMPLE_YIELD;

    std::map<Tenor,double> getData()
    {
        std::map<Tenor,double> data = 
        {
            {Tenor(1,TenorType::MONTHS), 4.18/100.0},	
            {Tenor(6,TenorType::WEEKS), 4.15/100.0},
            {Tenor(2,TenorType::MONTHS), 4.08/100.0},	
            {Tenor(3,TenorType::MONTHS), 4.00/100.0},	
            {Tenor(4,TenorType::MONTHS), 3.95/100.0},	
            {Tenor(6,TenorType::MONTHS), 3.79/100.0},	
            {Tenor(1,TenorType::YEARS), 3.56/100.0},	
            {Tenor(2,TenorType::YEARS), 3.46/100.0},	
            {Tenor(3,TenorType::YEARS), 3.47/100.0},	
            {Tenor(5,TenorType::YEARS), 3.59/100.0},	
            {Tenor(7,TenorType::YEARS), 3.78/100.0},	
            {Tenor(10,TenorType::YEARS), 4.02/100.0},	
            {Tenor(20,TenorType::YEARS), 4.58/100.0},	
            {Tenor(30,TenorType::YEARS), 4.60/100.0},	
        };

        return data;
    }
}

void writeDiscountCurve(const DiscountCurve& dc, std::string fileName) {

    // Open file for writing
    std::filesystem::create_directory("output-discountcurve-test");
    std::ofstream file("output-discountcurve-test/" + fileName +".csv");
    if (!file.is_open()) {
        std::cerr << "Error: could not open file." << std::endl;
    }

    // Write column headers
    file << "timeToMaturity,discountPrice,logDiscountPrice,simpleRate,continuousRate,instantaneousForwardRate,derivativeInstantaneousForwardRate, \n";

    // Write data rows
    file << std::fixed << std::setprecision(6);  // control decimal places
    double t = 0;
    for (int i = 0; i < 299; ++i) {
        t+=0.1;
        file << t << "," << dc.getValue(t) << "," << log(dc.getValue(t)) << "," << dc.getSimpleRate(t) << "," << dc.getContinuousRate(t) << "," << dc.getInstantaneousForwardRate(t) <<  "," << dc.getDerivativeInstantaneousForwardRate(t) <<"\n";  // CSV format
    }

    file.close();

}

bool isClose(double a, double b, double eps = 1e-6){return std::abs(a-b)<eps;}

void testCurveMethods(const DiscountCurve& curve, std::map<double, double> effectiveSimpleYields)
{
    //std::map<double, double> dataTest = {{.25, 0.0242277000}, {10.0, 0.0325197000}};

    std::function<double(double, double, const DiscountCurve::InterpolationVariable&)> getValueFromSimple = []
    (double t, double y, const DiscountCurve::InterpolationVariable& interpolationVariable)
    {
        switch (interpolationVariable)
        {
        case DiscountCurve::InterpolationVariable::ZC_CONTINUOUS_YIELD: return log(1/(1+y*t))/-t;
        case DiscountCurve::InterpolationVariable::ZC_LOG_PRICE: return log(1/(1+y*t));
        case DiscountCurve::InterpolationVariable::ZC_PRICE: return 1/(1+y*t);
        case DiscountCurve::InterpolationVariable::ZC_SIMPLE_YIELD : return y;
        }
    };

    for (const auto& d: effectiveSimpleYields)
    {
        double t = d.first; 
        double y = d.second;
        assert(isClose(curve.getContinuousRate(t),getValueFromSimple(t,y,DiscountCurve::InterpolationVariable::ZC_CONTINUOUS_YIELD),1e-4));
        assert(isClose(std::log(curve.getValue(t)),getValueFromSimple(t,y,DiscountCurve::InterpolationVariable::ZC_LOG_PRICE),1e-4));
        assert(isClose(curve.getValue(t),getValueFromSimple(t,y,DiscountCurve::InterpolationVariable::ZC_PRICE),1e-4));
        assert(isClose(curve.getSimpleRate(t),getValueFromSimple(t,y,DiscountCurve::InterpolationVariable::ZC_SIMPLE_YIELD),1e-4));
    }

}

void testConstructors()
{
    std::map<double, double> dataTest = {{.25, 0.0242277000}, {10.0, 0.0325197000}};
    DiscountCurve dc1(CanadianZeroYieldData::REFERENCE_DATE, CanadianZeroYieldData::getData(), DiscountCurve::InterpolationMethod::LINEAR, CanadianZeroYieldData::interpolationVariable);
    testCurveMethods(dc1, dataTest);
    DiscountCurve dc2(CanadianZeroYieldData::REFERENCE_DATE, CanadianZeroYieldData::getData(), DiscountCurve::InterpolationMethod::CUBIC_SPLINE, CanadianZeroYieldData::interpolationVariable);
    testCurveMethods(dc2, dataTest);
    DiscountCurve dc3(CanadianZeroYieldData::REFERENCE_DATE, CanadianZeroYieldData::getTenorMapped(), Scheduler(DayCountConvention::ACTUAL_360), DiscountCurve::InterpolationMethod::CUBIC_SPLINE, CanadianZeroYieldData::interpolationVariable);
    testCurveMethods(dc3, {{.25, 0.0242277000}});
    DiscountCurve dc4(CanadianZeroYieldData::REFERENCE_DATE, CanadianZeroYieldData::getDatetimeMapped(), Scheduler(DayCountConvention::ACTUAL_360), DiscountCurve::InterpolationMethod::CUBIC_SPLINE, CanadianZeroYieldData::interpolationVariable);
    testCurveMethods(dc4, {{.25, 0.0242277000}});
}

void testErrors()
{
    DiscountCurve dc(CanadianZeroYieldData::REFERENCE_DATE, CanadianZeroYieldData::getData(), DiscountCurve::InterpolationMethod::LINEAR, CanadianZeroYieldData::interpolationVariable);
    try{dc.getValue(-1); assert(false);}
    catch(const QuantErrorRegistry::NegativeYearFractionError& e){assert(true);}
    catch(const std::exception& ee){assert(false);}

}

void testNelsonSiegelCurve()
{
    DiscountCurve canadianCubic(CanadianZeroYieldData::REFERENCE_DATE, CanadianZeroYieldData::getData(), DiscountCurve::InterpolationMethod::CUBIC_SPLINE, CanadianZeroYieldData::interpolationVariable);
    writeDiscountCurve(canadianCubic, "Canada::24092025::CubicSpline");
    DiscountCurve canadianLinear(CanadianZeroYieldData::REFERENCE_DATE, CanadianZeroYieldData::getData(), DiscountCurve::InterpolationMethod::LINEAR, CanadianZeroYieldData::interpolationVariable);
    writeDiscountCurve(canadianLinear, "Canada::24092025::Linear");
    DiscountCurve canadianSvensson = canadianLinear.getSvenssonCurve();
    writeDiscountCurve(canadianSvensson, "Canada::24092025::Svensson");

    DiscountCurve USTCubic(USTreasuryParYieldData::REFERENCE_DATE, USTreasuryParYieldData::getData(), Scheduler(DayCountConvention::ACTUAL_360), DiscountCurve::InterpolationMethod::CUBIC_SPLINE, USTreasuryParYieldData::interpolationVariable);
    writeDiscountCurve(USTCubic, "USTreasury::17102025::CubicSpline");
    DiscountCurve USTLinear(USTreasuryParYieldData::REFERENCE_DATE, USTreasuryParYieldData::getData(), Scheduler(DayCountConvention::ACTUAL_360), DiscountCurve::InterpolationMethod::LINEAR, USTreasuryParYieldData::interpolationVariable);
    writeDiscountCurve(USTLinear, "USTreasury::17102025::Linear");
    DiscountCurve USTSvensson = USTLinear.getSvenssonCurve();
    writeDiscountCurve(USTSvensson, "USTreasury::17102025::Svensson");

}

int main()
{
    testConstructors(); 
    testErrors(); 
    testNelsonSiegelCurve();
    std::cout << "All tests for the discount curve has been passed successfully!" << std::endl;
    return 0; 
}


