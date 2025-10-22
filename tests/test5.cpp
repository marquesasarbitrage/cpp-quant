#include "../include/cpp-quant/valuation/marketdata/marketdata.hpp"
#include <cassert>

// Test average overnight ccopound rate from September 3rd, 2025 to September 16th, 2025 (https://ir.calc.gryt.cloud/)
void runTest1()
{
    DateTime refDate(1756857600, EpochTimestampType::SECONDS); 
    std::vector<int> deltaDays = {1,1,3,1,1,1,1,3,1};
    std::vector<double> rates = {1.9250/100, 1.9230/100, 1.9220/100, 1.9230/100, 1.9220/100, 1.9250/100, 1.9250/100, 1.9260/100, 1.9260/100}; 
    std::map<DateTime, double> data ={{refDate, 1.9230/100}};
    for (int i = 0; i<deltaDays.size(); i++)
    {
        refDate += TimeDelta(deltaDays[i],0,0,0,0,0,0);
        data[refDate] = rates[i];
    }
    AverageOvernightRate aor(data); 
    Scheduler scheduler = Scheduler(false,HolidayCalendar::NONE,DayCountConvention::ACTUAL_360);
    assert(aor.getAnnualizedFixingRates().rbegin()->first.getTimestamp() == 1757980800);

    DateTime startDate = DateTime(1756857600, EpochTimestampType::SECONDS);
    DateTime endDate = DateTime(1757980800, EpochTimestampType::SECONDS);

    assert(scheduler.getYearFraction(startDate,endDate)==13.0/360.0);
    assert(aor.getObservations(startDate,endDate).size()==data.size());
    assert(aor.getFixingRate(endDate)==1.9260/100);
    assert(aor.getFixingRate(startDate + TimeDelta(5,0,0,0,0,0,0))==1.9220/100);
    assert(aor.getFixingRate(startDate - TimeDelta(5,0,0,0,0,0,0))==1.9230/100);

    std::cout << aor.getAnnualizedAverageRate(startDate,endDate,scheduler) << std::endl;
    assert(aor.getAnnualizedAverageRate(startDate,endDate,scheduler)==0.0192442);
};


int main()
{
    runTest1();

    return 0; 
}