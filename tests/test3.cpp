// TEST DISCOUNT CURVE 
#include <cassert>
#include <iostream>
#include "../include/cpp-quant/valuation/termstructure/discountcurve.hpp"

DateTime getYieldsDataReferenceTime() {return DateTime(1758704936, EpochTimestampType::SECONDS);}

std::vector<double> getBankOfCanadaYieldsData()
{
    // Zero yields data of Bank of Canada as of September 24th, 2025 (https://www.bankofcanada.ca/rates/interest-rates/bond-yield-curves/)
    return {
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

}

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

std::map<double, double> getDataWithYearFraction(const DiscountCurve::InterpolationVariable& interpolationVariable)
{
    std::map<double, double> data; 
    std::vector<double> yields = getBankOfCanadaYieldsData();
    double t = 0; 
    for (const double y: yields)
    {
        t+=.25;
        data[t] = modifySimpleYield(t,y,interpolationVariable);
    }
    return data;
}

std::map<Tenor, double> getDataWithTenor(const DiscountCurve::InterpolationVariable& interpolationVariable)
{
    std::map<Tenor, double> data; 
    std::vector<double> yields = getBankOfCanadaYieldsData();
    Scheduler scheduler = Scheduler();
    double m = 0, t; 
    DateTime dt = getYieldsDataReferenceTime();
    for (const double y: yields)
    {
        m+=3;

        t = scheduler.getYearFraction(dt,scheduler.getForwardDateTime(dt,Tenor(m,TenorType::MONTHS)));
        data[Tenor(m,TenorType::MONTHS)] = modifySimpleYield(t,y,interpolationVariable);
    }
    return data;
}

std::map<DateTime, double> getDataWithDatetimes(const DiscountCurve::InterpolationVariable& interpolationVariable)
{
    std::map<DateTime, double> data; 
    std::vector<double> yields = getBankOfCanadaYieldsData();
    DateTime dt = getYieldsDataReferenceTime();
    Scheduler scheduler = Scheduler();
    double t;
    for (const double y: yields)
    {
        dt = scheduler.getForwardDateTime(dt,Tenor(3,TenorType::MONTHS));
        t = scheduler.getYearFraction(getYieldsDataReferenceTime(),dt);
        data[dt] = modifySimpleYield(t,y,interpolationVariable);;
    }
    return data;
}

bool isClose(double a, double b, double eps = 1e-6){return std::abs(a-b)<eps;}

void runInitialTest(const DiscountCurve::InterpolationVariable& interpolationVariable)
{
    DateTime dt = getYieldsDataReferenceTime();
    std::map<double, double> data = getDataWithYearFraction(interpolationVariable); 
    DiscountCurve dc(dt, data, DiscountCurve::InterpolationMethod::LINEAR, interpolationVariable);
    assert(isClose(dc.getValue(.25),1.0/(1.0+.25*0.0242277000),1e-3));

    try{dc.getValue(-1); assert(false);}
    catch(const QuantErrorRegistry::NegativeYearFractionError& e){assert(true);}
    catch(const std::exception& ee){assert(false);}
    
}

int main()
{
    runInitialTest(DiscountCurve::InterpolationVariable::ZC_SIMPLE_YIELD);
    runInitialTest(DiscountCurve::InterpolationVariable::ZC_LOG_PRICE);
    runInitialTest(DiscountCurve::InterpolationVariable::ZC_CONTINUOUS_YIELD);
    runInitialTest(DiscountCurve::InterpolationVariable::ZC_PRICE);
    std::cout << "All tests have been successfully passed for the discount curve." << std::endl;
    return 0;
}