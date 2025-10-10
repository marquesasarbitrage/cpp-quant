#include "../../include/cpp-quant/tools/scheduler.hpp"

Tenor::Tenor(int value, const TenorType& tenor_type): value_(abs(value)), tenor_type_(tenor_type){}

int Tenor::getValue() const {return value_;}
TenorType Tenor::getTenorType() const {return tenor_type_;}
std::string Tenor::asString() const
{
    if (value_ == 0) return "0D";
    else if (value_ == 1 and tenor_type_ == TenorType::DAYS) return "ON";
    else if (value_ == 2 and tenor_type_ == TenorType::DAYS) return "SN";
    else if (value_ == 3 and tenor_type_ == TenorType::DAYS) return "TN";
    else 
    {
        switch (tenor_type_)
        {
            case TenorType::DAYS: return std::to_string(value_) + "D";;
            case TenorType::WEEKS: return std::to_string(value_) + "W";
            case TenorType::MONTHS: return std::to_string(value_) + "M";
            case TenorType::YEARS: return std::to_string(value_) + "Y";
        }
    }
}

TimeDelta Tenor::getRawTimeDelta() const
{
    DateTime now = DateTime();
    std::tm t = now.getCTimeObject();
    switch (getTenorType()){
        case TenorType::DAYS: {t.tm_mday += getValue();}; break;
        case TenorType::WEEKS: {t.tm_mday += 7*getValue();}; break;
        case TenorType::MONTHS: {t.tm_mon += getValue();}; break;
        case TenorType::YEARS: {t.tm_year += getValue();}; break;
    }
    DateTime forwardDate = DateTime(static_cast<long long int>(mktime(&t)), EpochTimestampType::SECONDS);
    return forwardDate - now;

}

bool Tenor::operator==(const Tenor& other) const {return (getRawTimeDelta() == other.getRawTimeDelta()) ? true : false;}
bool Tenor::operator<(const Tenor& other) const {return (getRawTimeDelta() < other.getRawTimeDelta()) ? true : false;}
bool Tenor::operator<=(const Tenor& other) const {return (getRawTimeDelta() <= other.getRawTimeDelta()) ? true : false;}
bool Tenor::operator!=(const Tenor& other) const{return (!operator==(other));}
bool Tenor::operator>(const Tenor& other) const {return (!operator<=(other));}
bool Tenor::operator>=(const Tenor& other) const {return (!operator<(other));}
Tenor Tenor::operator*(int n) const{return Tenor(value_*abs(n), tenor_type_);}
double Tenor::operator/(const Tenor& other) const{return double(getRawTimeDelta().getTotalNanoSeconds())/double(other.getRawTimeDelta().getTotalNanoSeconds());}

Scheduler::Scheduler(const bool businessDayAdjusted, const HolidayCalendar& holidayCalendar, const DayCountConvention& dayCountConvention):
businessDayAdjusted_(businessDayAdjusted), holidayCalendar_(holidayCalendar), dayCountConvention_(dayCountConvention){};

Scheduler::Scheduler():businessDayAdjusted_(false), holidayCalendar_(HolidayCalendar::NONE), dayCountConvention_(DayCountConvention::ACTUAL_360){};

void Scheduler::setDayCountConvention(const DayCountConvention& dayCountConvention){dayCountConvention_=dayCountConvention;}
void Scheduler::setHolidayCalendar(const HolidayCalendar& holidayCalendar){holidayCalendar_=holidayCalendar;}
void Scheduler::setIsBusinessDayAdjusted(bool businessDayAdjusted){businessDayAdjusted_=businessDayAdjusted;}

DayCountConvention Scheduler::getDayCountConvention()const{return dayCountConvention_;}
HolidayCalendar Scheduler::getHolidayCalendar()const{return holidayCalendar_;}
bool Scheduler::getIsBusinessDayAdjusted()const{return businessDayAdjusted_;}

bool Scheduler::isWeekEnd(const DateTime& referenceDate) const
{
    std::tm time_info = referenceDate.getCTimeObject();
    if (time_info.tm_wday==0){return true;}
    if (time_info.tm_wday==6){return true;}
    return false;
}

DateTime Scheduler::getNextBusinessDay(const DateTime& referenceDate) const
{
    switch (holidayCalendar_)
    {
    case HolidayCalendar::NONE: return getNextBusinessDayNone(referenceDate); 
    default: 
        //holidayCalendar_ = HolidayCalendar::NONE;
        return getNextBusinessDayNone(referenceDate); 
    }
}

DateTime Scheduler::getNextBusinessDayNone(const DateTime& referenceDate) const
{
    DateTime outDate = DateTime(referenceDate.getTimestamp(), referenceDate.getTimestampType());
    if (isWeekEnd(referenceDate)){
        int day_of_week = outDate.getCTimeObject().tm_wday;
        if (day_of_week == 6) {outDate += TimeDelta(2,0,0,0,0,0,0);};
        if (day_of_week == 0) {outDate += TimeDelta(1,0,0,0,0,0,0);};
    };
    return outDate;
}

double Scheduler::getYearFraction(const DateTime& start, const DateTime& end) const
{
    if (start>end) return 0.0;
    return double((end-start).getTotalNanoSeconds())/(static_cast<double>(dayCountConvention_)*static_cast<double>(EpochTimestampType::NANOSECONDS)*24.0*60.0*60.0);
}

DateTime Scheduler::getForwardDateTime(const DateTime& referenceDate, const Tenor& tenor) const
{
    std::tm t = referenceDate.getCTimeObject();
    switch (tenor.getTenorType()){
        case TenorType::DAYS: {t.tm_mday += tenor.getValue();}; break;
        case TenorType::WEEKS: {t.tm_mday += 7*tenor.getValue();}; break;
        case TenorType::MONTHS: {t.tm_mon += tenor.getValue();}; break;
        case TenorType::YEARS: {t.tm_year += tenor.getValue();}; break;
    }
    DateTime forwardDate = DateTime(static_cast<long long int>(mktime(&t)), EpochTimestampType::SECONDS);
    return businessDayAdjusted_ ? getNextBusinessDay(forwardDate) : forwardDate;
}

std::vector<DateTime> Scheduler::getDateTimeSequence(const DateTime& start, const Tenor& frequence, const Tenor& maturity) const
{
    DateTime reference_date = businessDayAdjusted_ ? getNextBusinessDay(start): start;
    DateTime maturity_date = getForwardDateTime(start, maturity);
    int n = static_cast<int>(round(maturity/frequence));
    std::vector<DateTime> dateSequence = {reference_date}; 
    for (int i = 0; i<n-1; i++)
    {
        reference_date = getForwardDateTime(reference_date, frequence);
        dateSequence.push_back(reference_date);
    }
    dateSequence.push_back(maturity_date);
    return dateSequence;
}
