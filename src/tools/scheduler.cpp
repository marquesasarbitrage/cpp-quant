#include "../../include/cpp-quant/tools/scheduler.hpp"

Tenor::Tenor(int value, const TenorType& tenorType): value_(abs(value)), tenorType_(tenorType){}

int Tenor::getValue() const {return value_;}
TenorType Tenor::getTenorType() const {return tenorType_;}

std::string Tenor::asString() const
{
    if (value_ == 0) return "0D";
    else if (value_ == 1 and tenorType_ == TenorType::DAYS) return "ON";
    else if (value_ == 2 and tenorType_ == TenorType::DAYS) return "SN";
    else if (value_ == 3 and tenorType_ == TenorType::DAYS) return "TN";
    else 
    {
        switch (tenorType_)
        {
            case TenorType::DAYS: return std::to_string(value_) + "D";;
            case TenorType::WEEKS: return std::to_string(value_) + "W";
            case TenorType::MONTHS: return std::to_string(value_) + "M";
            case TenorType::YEARS: return std::to_string(value_) + "Y";
        }
    }
}

TimeDelta Tenor::getTimeDelta() const{ DateTime now = DateTime();return getForwardDate(now) - now;}

DateTime Tenor::getForwardDate(const DateTime& startDate) const
{
    std::tm t = startDate.getCTimeObject();
    switch (getTenorType()){
        case TenorType::DAYS: {t.tm_mday += getValue();}; break;
        case TenorType::WEEKS: {t.tm_mday += 7*getValue();}; break;
        case TenorType::MONTHS: {t.tm_mon += getValue();}; break;
        case TenorType::YEARS: {t.tm_year += getValue();}; break;
    }
    DateTime newDate = DateTime(static_cast<long long int>(mktime(&t)), EpochTimestampType::SECONDS);
    if (DateTimeTools::isEndMonth(startDate) and startDate.getCTimeObject().tm_mday == 31){
        if (newDate.getCTimeObject().tm_mday == 1) return newDate - TimeDelta(1,0,0,0,0,0,0);
        else return newDate;
    } else return newDate;
}

bool Tenor::operator==(const Tenor& other) const {return (getTimeDelta() == other.getTimeDelta()) ? true : false;}
bool Tenor::operator<(const Tenor& other) const {return (getTimeDelta() < other.getTimeDelta()) ? true : false;}
bool Tenor::operator<=(const Tenor& other) const {return (getTimeDelta() <= other.getTimeDelta()) ? true : false;}
bool Tenor::operator!=(const Tenor& other) const{return (!operator==(other));}
bool Tenor::operator>(const Tenor& other) const {return (!operator<=(other));}
bool Tenor::operator>=(const Tenor& other) const {return (!operator<(other));}
Tenor Tenor::operator*(int n) const{return Tenor(value_*abs(n), getTenorType());}
double Tenor::operator/(const Tenor& other) const{return double(getTimeDelta().getTotalNanoSeconds())/double(other.getTimeDelta().getTotalNanoSeconds());}

Scheduler::Scheduler(const DayCountConvention& dayCountConvention, const BusinessDayConvention& businessdayConvention, const HolidayCalendar& holidayCalendar):
dayCountConvention_(dayCountConvention), businessdayConvention_(businessdayConvention), holidayCalendar_(holidayCalendar){holidayList_ = loadHolidayList(holidayCalendar);}

Scheduler::Scheduler(const DayCountConvention& dayCountConvention): 
dayCountConvention_(dayCountConvention), businessdayConvention_(BusinessDayConvention::NONE), holidayCalendar_(HolidayCalendar::NONE){}

void Scheduler::setDayCountConvention(const DayCountConvention& dayCountConvention){dayCountConvention_ = dayCountConvention;}
void Scheduler::setHolidayCalendar(const HolidayCalendar& holidayCalendar){holidayCalendar_ = holidayCalendar; holidayList_ = loadHolidayList(holidayCalendar);}
void Scheduler::setBusinessDayConvention(const BusinessDayConvention& businessdayConvention){businessdayConvention_ = businessdayConvention;}

DayCountConvention Scheduler::getDayCountConvention() const {return dayCountConvention_;}
HolidayCalendar Scheduler::getHolidayCalendar() const {return holidayCalendar_;}
BusinessDayConvention Scheduler::getBusinessDayConvention() const{return businessdayConvention_;}

std::set<DateTime> Scheduler::loadHolidayList(const HolidayCalendar& holidayCalendar) const 
{
    std::set<DateTime> holidayList = {};
    std::set<DateTime> output = {};
    for (const DateTime& h: holidayList) output.insert(DateTimeTools::getMidnightDateTime(h));
    return output;
} 

DateTime Scheduler::getFollowingAjustedBusinessDay(const DateTime& date) const
{
    std::tm t = date.getCTimeObject();
    int weekDay = t.tm_wday;
    int adjFollowing = weekDay==0 ? 1 : weekDay==6 ? 2 : 0; 
    DateTime newDate = date + TimeDelta(adjFollowing, 0,0,0,0,0,0);
    while (holidayList_.find(DateTimeTools::getMidnightDateTime(newDate)) != holidayList_.end() or DateTimeTools::isWeekEnd(newDate)) {
        newDate += TimeDelta(1,0,0,0,0,0,0);
    };
    return newDate;

}

DateTime Scheduler::getPrecedingAjustedBusinessDay(const DateTime& date) const
{
    std::tm t = date.getCTimeObject();
    int weekDay = t.tm_wday;
    int adjPreceding = weekDay==0 ? -2 : weekDay==6 ? -1 : 0; 
    DateTime newDate = date + TimeDelta(adjPreceding, 0,0,0,0,0,0);
    while (holidayList_.find(DateTimeTools::getMidnightDateTime(newDate)) != holidayList_.end() or DateTimeTools::isWeekEnd(newDate)) {
        newDate -= TimeDelta(1,0,0,0,0,0,0);
    };
    return newDate;
}

DateTime Scheduler::getBusinessAdjustedDate(const DateTime& date) const
{
    switch (businessdayConvention_)
    {
    case BusinessDayConvention::NONE: return date;
    case BusinessDayConvention::FOLLOWING: return getFollowingAjustedBusinessDay(date);
    case BusinessDayConvention::PRECEDING: return getPrecedingAjustedBusinessDay(date);
    case BusinessDayConvention::MODIFIED_FOLLOWING: {
        DateTime modFol = getFollowingAjustedBusinessDay(date); 
        if (modFol.getCTimeObject().tm_mon == date.getCTimeObject().tm_mon) return modFol; 
        else return getPrecedingAjustedBusinessDay(date);
    }
    case BusinessDayConvention::MODIFIED_PRECEDING: {
        DateTime modPrec = getPrecedingAjustedBusinessDay(date); 
        if (modPrec.getCTimeObject().tm_mon == date.getCTimeObject().tm_mon) return modPrec; 
        else return getFollowingAjustedBusinessDay(date);
    }
    default: return date;
    }
}

double Scheduler::getYearFraction(const DateTime& startDate, const DateTime& endDate) const
{
    DateTime d0 = getBusinessAdjustedDate(startDate); 
    DateTime d1 = getBusinessAdjustedDate(endDate); 
    switch (dayCountConvention_)
    {
    case DayCountConvention::ACTUAL_360: return double((d1-d0).getTotalNanoSeconds())/Scheduler::FACTOR360;
    case DayCountConvention::ACTUAL_365: return double((d1-d0).getTotalNanoSeconds())/Scheduler::FACTOR365;
    case DayCountConvention::ACTUAL_364: return double((d1-d0).getTotalNanoSeconds())/Scheduler::FACTOR364;
    case DayCountConvention::ACTUAL_ACTUAL: {
        long long leap = double(DateTimeTools::getTimeInLeapYear(startDate, endDate).getTotalNanoSeconds());
        long long total = double((endDate-startDate).getTotalNanoSeconds()); 
        return double(leap)/Scheduler::FACTOR366 + double(total-leap)/Scheduler::FACTOR365;
    }
    case DayCountConvention::BOND_BASIS30_360: {
        int d2 = endDate.getCTimeObject().tm_mday, d1 = startDate.getCTimeObject().tm_mday; 
        d1 = std::min(30,d1);
        d2 = (d1==30) ? std::min(30,d2) : d2;
        return (get30360BaseCount(startDate,endDate) + (d2-d1))/360.0;
    }
    case DayCountConvention::E30_360: {
        int d2 = endDate.getCTimeObject().tm_mday, d1 = startDate.getCTimeObject().tm_mday; 
        return (get30360BaseCount(startDate,endDate) + (std::min(30,d2)-std::min(30,d1)))/360.0;
    }
    }

}

double Scheduler::get30360BaseCount(const DateTime& startDate, const DateTime& endDate) const
{
    int y2 = endDate.getCTimeObject().tm_year, y1 = startDate.getCTimeObject().tm_year; 
    int m2 = endDate.getCTimeObject().tm_mon, m1 = startDate.getCTimeObject().tm_mon; 
    return 360.0*(y2-y1) + 30.0*(m2-m1);
}

double Scheduler::getYearFraction(const DateTime& startDate, const Tenor& tenor) const
{
    return getYearFraction(startDate,tenor.getForwardDate(startDate));
}

std::set<DateTime> Scheduler::getSchedule(const DateTime& referenceDate, const Tenor& frequence, int sequenceLength) const
{
    std::set<DateTime> output = {getBusinessAdjustedDate(referenceDate)}; 
    DateTime d0 = referenceDate;
    
    for (int i = 0; i<sequenceLength; i++)
    {
        d0 = frequence.getForwardDate(d0);
        output.insert(getBusinessAdjustedDate(d0));
    }
    return output;
};
