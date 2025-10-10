#pragma once 
#include "cpp-datetime/datetime.hpp"

enum class TenorType {DAYS, WEEKS, MONTHS, YEARS};
enum class DayCountConvention {ACTUAL_360 = 360, ACTUAL_365 = 365, ACTUAL_364 = 364};
enum class HolidayCalendar {NONE};

class Tenor
{
    public:
        Tenor(int value, const TenorType& tenor_type);
        ~Tenor(){};

        int getValue() const;
        TenorType getTenorType() const;
        std::string asString() const;
        TimeDelta getRawTimeDelta() const;

        bool operator==(const Tenor& other) const;
        bool operator<(const Tenor& other) const;
        bool operator<=(const Tenor& other) const;
        bool operator!=(const Tenor& other) const;
        bool operator>(const Tenor& other) const;
        bool operator>=(const Tenor& other) const;
        Tenor operator*(int n) const;
        double operator/(const Tenor& other) const;

    private: 
        int value_; 
        TenorType tenor_type_;
};

class Scheduler
{
    public: 
        Scheduler(const bool businessDayAdjusted, const HolidayCalendar& holidayCalendar, const DayCountConvention& dayCountConvention); 
        Scheduler(); 

        void setDayCountConvention(const DayCountConvention& dayCountConvention);
        void setHolidayCalendar(const HolidayCalendar& holidayCalendar);
        void setIsBusinessDayAdjusted(bool businessDayAdjusted);

        DayCountConvention getDayCountConvention() const;
        HolidayCalendar getHolidayCalendar() const;
        bool getIsBusinessDayAdjusted() const;

        bool isWeekEnd(const DateTime& referenceDate) const;
        DateTime getNextBusinessDay(const DateTime& referenceDate) const;
        double getYearFraction(const DateTime& start, const DateTime& end) const;
        DateTime getForwardDateTime(const DateTime& referenceDate, const Tenor& tenor) const; 
        std::vector<DateTime> getDateTimeSequence(const DateTime& referenceDate, const Tenor& frequence, const Tenor& maturity) const;

    private: 
        bool businessDayAdjusted_; 
        HolidayCalendar holidayCalendar_; 
        DayCountConvention dayCountConvention_;

        DateTime getNextBusinessDayNone(const DateTime& referenceDate) const;
};