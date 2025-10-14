// TEST DISCOUNT CURVE 
#include <cassert>
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip> 
#include <filesystem>
#include "../include/cpp-quant/valuation/termstructure/discountcurve.hpp"

bool isClose(double a, double b, double eps = 1e-6){return std::abs(a-b)<eps;}

namespace DiscountCurveTestTool
{
    double modifySimpleYield(double t, double y, const DiscountCurve::InterpolationVariable& interpolationVariable)
    {
        switch (interpolationVariable)
        {
        case DiscountCurve::InterpolationVariable::ZC_CONTINUOUS_YIELD: return log(1/(1+y*t))/-t;
        case DiscountCurve::InterpolationVariable::ZC_LOG_PRICE: return log(1/(1+y*t));
        case DiscountCurve::InterpolationVariable::ZC_PRICE: return 1/(1+y*t);
        case DiscountCurve::InterpolationVariable::ZC_SIMPLE_YIELD : return y;
        }
    }

    void writeDiscountCurveData(const DiscountCurve& dc, std::string fileName) {

        // Open file for writing
        std::filesystem::create_directory("output");
        std::ofstream file("output/" + fileName +".csv");
        if (!file.is_open()) {
            std::cerr << "Error: could not open file." << std::endl;
        }

        // Write column headers
        file << "timeToMaturity,discountPrice,logDiscountPrice,simpleRate,continuousRate,instantaneousForwardRate,derivativeInstantaneousForwardRate, \n";

        // Write data rows
        file << std::fixed << std::setprecision(6);  // control decimal places
        double t = 0;
        for (int i = 0; i < 700; ++i) {
            t+=0.05;
            file << t << "," << dc.getValue(t) << "," << log(dc.getValue(t)) << "," << dc.getSimpleRate(t) << "," << dc.getContinuousRate(t) << "," << dc.getInstantaneousForwardRate(t) <<  "," << dc.getDerivativeInstantaneousForwardRate(t) <<"\n";  // CSV format
        }

        file.close();
        std::cout << "Discount curve data written successfully!" << std::endl;

    }

}

namespace CanadianZeroCurveTest
{
    namespace StaticData
    {
        // Zero yields data of Bank of Canada as of September 24th, 2025 (https://www.bankofcanada.ca/rates/interest-rates/bond-yield-curves/)
        DateTime REFERENCE_DATE = DateTime(1758704936, EpochTimestampType::SECONDS);

        std::vector<double> DATA = {
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

        double RATE_10_YEARS = 0.0325197000;
        double RATE_3_MONTHS = 0.0242277000;

        std::map<double, double> yearFractionMappedData(const DiscountCurve::InterpolationVariable& interpolationVariable)
        {
            std::map<double, double> data; 
            double t = 0; 
            for (const double y: DATA)
            {
                t+=.25;
                data[t] = DiscountCurveTestTool::modifySimpleYield(t,y,interpolationVariable);
            }
            return data;
        }

        std::map<Tenor, double> tenorMappedData()
        {
            std::map<Tenor, double> data; 
            Scheduler scheduler = Scheduler();
            double m = 0, t; 
            for (const double y: DATA)
            {
                m+=3;
                t = scheduler.getYearFraction(REFERENCE_DATE,scheduler.getForwardDateTime(REFERENCE_DATE,Tenor(m,TenorType::MONTHS)));
                data[Tenor(m,TenorType::MONTHS)] = y;
            }
            return data;
        }

        std::map<DateTime, double> datetimeMappedData()
        {
            std::map<DateTime, double> data; 
            DateTime dt = REFERENCE_DATE;
            Scheduler scheduler = Scheduler();
            double t;
            for (const double y: DATA)
            {
                dt = scheduler.getForwardDateTime(dt,Tenor(3,TenorType::MONTHS));
                t = scheduler.getYearFraction(REFERENCE_DATE,dt);
                data[dt] = y;
            }
            return data;
        }

    }
    
    namespace Tests 
    {
        void testInterpolationVariable(DiscountCurve::InterpolationVariable interpVar)
        {
            DiscountCurve dc1(StaticData::REFERENCE_DATE, StaticData::yearFractionMappedData(interpVar), DiscountCurve::InterpolationMethod::LINEAR, interpVar);
            DiscountCurve dc2(StaticData::REFERENCE_DATE, StaticData::yearFractionMappedData(interpVar), DiscountCurve::InterpolationMethod::CUBIC_SPLINE, interpVar);

            assert(isClose(dc1.getContinuousRate(.25),DiscountCurveTestTool::modifySimpleYield(.25,0.0242277000,DiscountCurve::InterpolationVariable::ZC_CONTINUOUS_YIELD),1e-4));
            assert(isClose(dc2.getContinuousRate(.25),DiscountCurveTestTool::modifySimpleYield(.25,0.0242277000,DiscountCurve::InterpolationVariable::ZC_CONTINUOUS_YIELD),1e-4));
            assert(isClose(std::log(dc1.getValue(.25)),DiscountCurveTestTool::modifySimpleYield(.25,0.0242277000,DiscountCurve::InterpolationVariable::ZC_LOG_PRICE),1e-4));
            assert(isClose(std::log(dc2.getValue(.25)),DiscountCurveTestTool::modifySimpleYield(.25,0.0242277000,DiscountCurve::InterpolationVariable::ZC_LOG_PRICE),1e-4));
            assert(isClose(dc1.getValue(.25),DiscountCurveTestTool::modifySimpleYield(.25,0.0242277000,DiscountCurve::InterpolationVariable::ZC_PRICE),1e-4));
            assert(isClose(dc2.getValue(.25),DiscountCurveTestTool::modifySimpleYield(.25,0.0242277000,DiscountCurve::InterpolationVariable::ZC_PRICE),1e-4));
            assert(isClose(dc1.getSimpleRate(.25),DiscountCurveTestTool::modifySimpleYield(.25,0.0242277000,DiscountCurve::InterpolationVariable::ZC_SIMPLE_YIELD),1e-4));
            assert(isClose(dc2.getSimpleRate(.25),DiscountCurveTestTool::modifySimpleYield(.25,0.0242277000,DiscountCurve::InterpolationVariable::ZC_SIMPLE_YIELD),1e-4));

            assert(isClose(dc1.getContinuousRate(10.0),DiscountCurveTestTool::modifySimpleYield(10.0,StaticData::RATE_10_YEARS,DiscountCurve::InterpolationVariable::ZC_CONTINUOUS_YIELD),1e-4));
            assert(isClose(dc2.getContinuousRate(10.0),DiscountCurveTestTool::modifySimpleYield(10.0,StaticData::RATE_10_YEARS,DiscountCurve::InterpolationVariable::ZC_CONTINUOUS_YIELD),1e-4));
            assert(isClose(std::log(dc1.getValue(10.0)),DiscountCurveTestTool::modifySimpleYield(10.0,StaticData::RATE_10_YEARS,DiscountCurve::InterpolationVariable::ZC_LOG_PRICE),1e-4));
            assert(isClose(std::log(dc2.getValue(10.0)),DiscountCurveTestTool::modifySimpleYield(10.0,StaticData::RATE_10_YEARS,DiscountCurve::InterpolationVariable::ZC_LOG_PRICE),1e-4));
            assert(isClose(dc1.getValue(10.0),DiscountCurveTestTool::modifySimpleYield(10.0,StaticData::RATE_10_YEARS,DiscountCurve::InterpolationVariable::ZC_PRICE),1e-4));
            assert(isClose(dc2.getValue(10.0),DiscountCurveTestTool::modifySimpleYield(10.0,StaticData::RATE_10_YEARS,DiscountCurve::InterpolationVariable::ZC_PRICE),1e-4));
            assert(isClose(dc1.getSimpleRate(10.0),DiscountCurveTestTool::modifySimpleYield(10.0,StaticData::RATE_10_YEARS,DiscountCurve::InterpolationVariable::ZC_SIMPLE_YIELD),1e-4));
            assert(isClose(dc2.getSimpleRate(10.0),DiscountCurveTestTool::modifySimpleYield(10.0,StaticData::RATE_10_YEARS,DiscountCurve::InterpolationVariable::ZC_SIMPLE_YIELD),1e-4));

        }

        void testNegativeYearFractionError()
        {
            DiscountCurve dc(StaticData::REFERENCE_DATE, StaticData::yearFractionMappedData(DiscountCurve::InterpolationVariable::ZC_PRICE), DiscountCurve::InterpolationMethod::LINEAR, DiscountCurve::InterpolationVariable::ZC_PRICE);
            try{dc.getValue(-1); assert(false);}
            catch(const QuantErrorRegistry::NegativeYearFractionError& e){assert(true);}
            catch(const std::exception& ee){assert(false);}

        }

        void testOtherConstructors()
        {
            Scheduler scheduler = Scheduler();
            DiscountCurve dc1(StaticData::REFERENCE_DATE, StaticData::tenorMappedData(), scheduler, DiscountCurve::InterpolationMethod::LINEAR,DiscountCurve::InterpolationVariable::ZC_SIMPLE_YIELD);
            DiscountCurve dc2(StaticData::REFERENCE_DATE, StaticData::datetimeMappedData(), scheduler, DiscountCurve::InterpolationMethod::LINEAR,DiscountCurve::InterpolationVariable::ZC_SIMPLE_YIELD);
            assert(isClose(dc1.getSimpleRate(.25),DiscountCurveTestTool::modifySimpleYield(.25,0.0242277000,DiscountCurve::InterpolationVariable::ZC_SIMPLE_YIELD),1e-2));
            assert(isClose(dc2.getSimpleRate(.25),DiscountCurveTestTool::modifySimpleYield(.25,0.0242277000,DiscountCurve::InterpolationVariable::ZC_SIMPLE_YIELD),1e-2));
            assert(isClose(dc1.getSimpleRate(10.0),DiscountCurveTestTool::modifySimpleYield(10.0,StaticData::RATE_10_YEARS,DiscountCurve::InterpolationVariable::ZC_SIMPLE_YIELD),1e-2));
            assert(isClose(dc2.getSimpleRate(10.0),DiscountCurveTestTool::modifySimpleYield(10.0,StaticData::RATE_10_YEARS,DiscountCurve::InterpolationVariable::ZC_SIMPLE_YIELD),1e-2));
        }

        void main()
        {
            testInterpolationVariable(DiscountCurve::InterpolationVariable::ZC_CONTINUOUS_YIELD); 
            testInterpolationVariable(DiscountCurve::InterpolationVariable::ZC_LOG_PRICE); 
            testInterpolationVariable(DiscountCurve::InterpolationVariable::ZC_PRICE); 
            testInterpolationVariable(DiscountCurve::InterpolationVariable::ZC_SIMPLE_YIELD); 
            testNegativeYearFractionError();
            testOtherConstructors();
            DiscountCurve dc(StaticData::REFERENCE_DATE, StaticData::yearFractionMappedData(DiscountCurve::InterpolationVariable::ZC_PRICE), DiscountCurve::InterpolationMethod::LINEAR, DiscountCurve::InterpolationVariable::ZC_PRICE);
            DiscountCurveTestTool::writeDiscountCurveData(dc, "data");
            std::cout << "All tests using the Canadian ZC Curve (September 24th, 2025) are passed!" << std::endl;
        }
    }
};

int main()
{
    CanadianZeroCurveTest::Tests::main();
    std::cout << "All tests have been successfully passed for the discount curve." << std::endl;
    return 0;
}