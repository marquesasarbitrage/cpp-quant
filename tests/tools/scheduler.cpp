#include <cassert>
#include <iomanip>
#include <iostream>
#include "../include/cpp-quant/tools/scheduler.hpp"

bool isClose(double value1, double value2, double eps) {return (std::abs(value2-value1)<eps);}

void tenorTest()
{
    // test : 30 Octobre 2025 21:00:00 
    DateTime referenceTime = DateTime(1761858000,EpochTimestampType::SECONDS); 
    Tenor tenor(6,TenorType::MONTHS); 

    // test : 30 Avril 2025 21:00:00 
    DateTime fwdDate = tenor.getForwardDate(referenceTime); 

    assert(fwdDate == DateTime(1777582800, EpochTimestampType::SECONDS));

    std::cout << "All tests passed for the object Tenor" << std::endl;
}

void schedulerBusinessConventionTest()
{
    Scheduler schedulerNone = Scheduler(DayCountConvention::ACTUAL_360, BusinessDayConvention::NONE,HolidayCalendar::NONE); 
    Scheduler schedulerFollow = Scheduler(DayCountConvention::ACTUAL_360, BusinessDayConvention::FOLLOWING,HolidayCalendar::NONE); 
    Scheduler schedulerModFollow = Scheduler(DayCountConvention::ACTUAL_360, BusinessDayConvention::MODIFIED_FOLLOWING,HolidayCalendar::NONE); 
    Scheduler schedulerPreceding = Scheduler(DayCountConvention::ACTUAL_360, BusinessDayConvention::PRECEDING,HolidayCalendar::NONE); 
    Scheduler schedulerModPreceding = Scheduler(DayCountConvention::ACTUAL_360, BusinessDayConvention::MODIFIED_PRECEDING,HolidayCalendar::NONE); 

    // test : 30 Octobre 2027 21:00:00 
    DateTime referenceTime = DateTime(1824930000,EpochTimestampType::SECONDS); 
    assert(schedulerNone.getBusinessAdjustedDate(referenceTime) == referenceTime); 
    assert(schedulerFollow.getBusinessAdjustedDate(referenceTime) == DateTime(1825102800,EpochTimestampType::SECONDS)); 
    assert(schedulerModFollow.getBusinessAdjustedDate(referenceTime) == DateTime(1824843600,EpochTimestampType::SECONDS)); 
    assert(schedulerPreceding.getBusinessAdjustedDate(referenceTime) == DateTime(1824843600,EpochTimestampType::SECONDS)); 
    assert(schedulerModPreceding.getBusinessAdjustedDate(referenceTime) == DateTime(1824843600,EpochTimestampType::SECONDS)); 

    // test : 30 Octobre 2033 21:00:00 
    DateTime referenceTime2 = DateTime(2014318800,EpochTimestampType::SECONDS); 
    assert(schedulerNone.getBusinessAdjustedDate(referenceTime2) == referenceTime2); 
    assert(schedulerFollow.getBusinessAdjustedDate(referenceTime2) == DateTime(2014405200,EpochTimestampType::SECONDS)); 
    assert(schedulerModFollow.getBusinessAdjustedDate(referenceTime2) == DateTime(2014405200,EpochTimestampType::SECONDS)); 
    assert(schedulerPreceding.getBusinessAdjustedDate(referenceTime2) == DateTime(2014146000,EpochTimestampType::SECONDS)); 
    assert(schedulerModPreceding.getBusinessAdjustedDate(referenceTime2) == DateTime(2014146000,EpochTimestampType::SECONDS));

    std::cout << "All business convention adjustement tests are passed for the Scheduler object." <<std::endl;


}

void schedulerYearFractionTest()
{
    Scheduler schedulerACT360 = Scheduler(DayCountConvention::ACTUAL_360,BusinessDayConvention::NONE,HolidayCalendar::NONE);
    Scheduler schedulerACT365 = Scheduler(DayCountConvention::ACTUAL_365,BusinessDayConvention::NONE,HolidayCalendar::NONE);
    Scheduler schedulerACT364 = Scheduler(DayCountConvention::ACTUAL_364,BusinessDayConvention::NONE,HolidayCalendar::NONE);
    Scheduler schedulerACTACT = Scheduler(DayCountConvention::ACTUAL_ACTUAL,BusinessDayConvention::NONE,HolidayCalendar::NONE);
    Scheduler schedulerE30360 = Scheduler(DayCountConvention::E30_360,BusinessDayConvention::NONE,HolidayCalendar::NONE);
    Scheduler schedulerBB30360 = Scheduler(DayCountConvention::BOND_BASIS30_360,BusinessDayConvention::NONE,HolidayCalendar::NONE);
    // 25 Août 2025 21:00:00 
    DateTime referenceTime = DateTime(1756155600,EpochTimestampType::SECONDS); 
    // 14 Novembre 2026 21:00:00 
    DateTime d2 = DateTime(1794690000,EpochTimestampType::SECONDS); 
    // 18 Mai 2027 21:00:00 
    DateTime d3 = DateTime(1810674000,EpochTimestampType::SECONDS); 
    // 31 Octobre 2028 21:00:00 
    DateTime d4 = DateTime(1856638800,EpochTimestampType::SECONDS); 
    // 30 Septemmbre 2029 21:00:00 
    DateTime d5 = DateTime(1885496400,EpochTimestampType::SECONDS);

    assert(isClose(schedulerACT360.getYearFraction(referenceTime,d2),1.238888889, 1e-7));
    assert(isClose(schedulerACT360.getYearFraction(referenceTime,d3),1.752777778, 1e-7));
    assert(isClose(schedulerACT360.getYearFraction(referenceTime,d4),3.230555556, 1e-7));
    assert(isClose(schedulerACT360.getYearFraction(referenceTime,d5),4.158333333, 1e-7));

    assert(isClose(schedulerACT365.getYearFraction(referenceTime,d2),1.221917808, 1e-7));
    assert(isClose(schedulerACT365.getYearFraction(referenceTime,d3),1.728767123, 1e-7));
    assert(isClose(schedulerACT365.getYearFraction(referenceTime,d4),3.18630137, 1e-7));
    assert(isClose(schedulerACT365.getYearFraction(referenceTime,d5),4.101369863, 1e-7));

    assert(isClose(schedulerACT364.getYearFraction(referenceTime,d2),1.225274725, 1e-7));
    assert(isClose(schedulerACT364.getYearFraction(referenceTime,d3),1.733516484, 1e-7));
    assert(isClose(schedulerACT364.getYearFraction(referenceTime,d4),3.195054945, 1e-7));
    assert(isClose(schedulerACT364.getYearFraction(referenceTime,d5),4.112637363, 1e-7));

    assert(isClose(schedulerBB30360.getYearFraction(referenceTime,d2),1.219444444, 1e-7));
    assert(isClose(schedulerBB30360.getYearFraction(referenceTime,d3),1.730555556, 1e-7));
    assert(isClose(schedulerBB30360.getYearFraction(referenceTime,d4),3.183333333, 1e-7));
    assert(isClose(schedulerBB30360.getYearFraction(referenceTime,d5),4.097222222, 1e-7));

    assert(isClose(schedulerE30360.getYearFraction(referenceTime,d2),1.219444444, 1e-7));
    assert(isClose(schedulerE30360.getYearFraction(referenceTime,d3),1.730555556, 1e-7));
    assert(isClose(schedulerE30360.getYearFraction(referenceTime,d4),3.180555556, 1e-7));
    assert(isClose(schedulerE30360.getYearFraction(referenceTime,d5),4.097222222, 1e-7));

    assert(isClose(schedulerACTACT.getYearFraction(referenceTime,d2),1.221917808, 1e-7));
    assert(isClose(schedulerACTACT.getYearFraction(referenceTime,d3),1.728767123, 1e-7));
    assert(isClose(schedulerACTACT.getYearFraction(referenceTime-TimeDelta(0,21,0,0,0,0,0),d4-TimeDelta(0,21,0,0,0,0,0)),3.18402575, 1e-7));
    assert(isClose(schedulerACTACT.getYearFraction(referenceTime,d5),4.098630137, 1e-7));
    
    std::cout << "All year fraction calculation tests are passed for the Scheduler object." <<std::endl;
}

void schedulerSchedulTestTemplate(const BusinessDayConvention& bdc, const std::set<int>& result)
{
    Scheduler scheduler = Scheduler(DayCountConvention::ACTUAL_360, bdc,HolidayCalendar::NONE); 
    DateTime startDate = DateTime(1761868800,EpochTimestampType::SECONDS); 

    std::set<DateTime> schedule = scheduler.getSchedule(startDate, Tenor(6,TenorType::MONTHS), 20);
    assert(result.size()==schedule.size());
    std::set<int> scheduleTimestamp;
    for (const DateTime& d: schedule){scheduleTimestamp.insert(d.getTimestamp());}
    assert(scheduleTimestamp==result);
}

void schedulerScheduleTest()
{
    std::set<int> resultNone = {
        1761868800, 1777507200, 1793318400, 1809043200, 1824854400,
        1840665600, 1856476800, 1872201600, 1888012800, 1903737600,
        1919548800, 1935273600, 1951084800, 1966896000, 1982707200,
        1998432000, 2014243200, 2029968000, 2045779200, 2061504000,
        2077315200
    };

    std::set<int> resultFollow = {
        1761868800, 1777507200, 1793318400, 1809043200, 1825027200,
        1840752000, 1856476800, 1872201600, 1888012800, 1903737600,
        1919548800, 1935273600, 1951084800, 1966896000, 1982880000,
        1998604800, 2014329600, 2030054400, 2045779200, 2061504000,
        2077315200
    };

    std::set<int> resultPreceding = {
        1761868800, 1777507200, 1793318400, 1809043200, 1824768000,
        1840492800, 1856476800, 1872201600, 1888012800, 1903737600,
        1919548800, 1935273600, 1951084800, 1966896000, 1982620800,
        1998345600, 2014070400, 2029795200, 2045779200, 2061504000,
        2077315200
    };

    std::set<int> resultModFollow = {
        1761868800, 1777507200, 1793318400, 1809043200, 1824768000,
        1840492800, 1856476800, 1872201600, 1888012800, 1903737600,
        1919548800, 1935273600, 1951084800, 1966896000, 1982620800,
        1998345600, 2014329600, 2029795200, 2045779200, 2061504000,
        2077315200
    };

    std::set<int> resultModPreceding = {
        1761868800, 1777507200, 1793318400, 1809043200, 1824768000,
        1840492800, 1856476800, 1872201600, 1888012800, 1903737600,
        1919548800, 1935273600, 1951084800, 1966896000, 1982620800,
        1998345600, 2014070400, 2029795200, 2045779200, 2061504000,
        2077315200
    };

    schedulerSchedulTestTemplate(BusinessDayConvention::NONE, resultNone); 
    schedulerSchedulTestTemplate(BusinessDayConvention::FOLLOWING, resultFollow); 
    schedulerSchedulTestTemplate(BusinessDayConvention::PRECEDING, resultPreceding); 
    schedulerSchedulTestTemplate(BusinessDayConvention::MODIFIED_FOLLOWING, resultModFollow); 
    schedulerSchedulTestTemplate(BusinessDayConvention::MODIFIED_PRECEDING, resultModPreceding); 
    
    std::cout << "All scheduling tests are passed from the Scheduler object " << std::endl;
}

int main()
{
    tenorTest();
    schedulerBusinessConventionTest();
    schedulerYearFractionTest();
    schedulerScheduleTest();
    return 0; 
}