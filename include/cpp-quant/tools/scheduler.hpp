#pragma once 
#include <iostream>
#include <set>
#include "cpp-datetime/tools.hpp"

enum class TenorType {DAYS, WEEKS, MONTHS, YEARS};
enum class DayCountConvention {ACTUAL_360, ACTUAL_365, ACTUAL_364, ACTUAL_ACTUAL, E30_360, BOND_BASIS30_360};
enum class BusinessDayConvention {NONE, FOLLOWING, PRECEDING, MODIFIED_FOLLOWING, MODIFIED_PRECEDING};
enum class HolidayCalendar {NONE};

class Tenor
{
    public:
        Tenor(int value, const TenorType& tenorType);
        ~Tenor(){};

        int getValue() const;
        TenorType getTenorType() const;
        std::string asString() const;
        DateTime getForwardDate(const DateTime& startDate) const; 
        TimeDelta getTimeDelta() const;

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
        TenorType tenorType_;
};

class Scheduler
{

    public: 
        Scheduler(const DayCountConvention& dayCountConvention, const BusinessDayConvention& businessdayConvention, const HolidayCalendar& holidayCalendar); 
        Scheduler(const DayCountConvention& dayCountConvention); 
        ~Scheduler() = default;

        void setDayCountConvention(const DayCountConvention& dayCountConvention);
        void setHolidayCalendar(const HolidayCalendar& holidayCalendar);
        void setBusinessDayConvention(const BusinessDayConvention& businessdayConvention);

        DayCountConvention getDayCountConvention() const;
        HolidayCalendar getHolidayCalendar() const;
        BusinessDayConvention getBusinessDayConvention() const;

        DateTime getBusinessAdjustedDate(const DateTime& date) const;
        double getYearFraction(const DateTime& startDate, const DateTime& endDate) const;
        double getYearFraction(const DateTime& startDate, const Tenor& tenor) const;
        std::set<DateTime> getSchedule(const DateTime& referenceDate, const Tenor& frequence, int sequenceLength) const;

    private: 
        DayCountConvention dayCountConvention_;
        BusinessDayConvention businessdayConvention_;
        HolidayCalendar holidayCalendar_; 
        std::set<DateTime> holidayList_;

        std::set<DateTime> loadHolidayList(const HolidayCalendar& holidayCalendar) const;
        DateTime getFollowingAjustedBusinessDay(const DateTime& date) const;
        DateTime getPrecedingAjustedBusinessDay(const DateTime& date) const;
        double get30360BaseCount(const DateTime& startDate, const DateTime& endDate) const;

        static constexpr double FACTOR365 = 365.0*DateTimeTools::dayInNanoSeconds;
        static constexpr double FACTOR366 = 366.0*DateTimeTools::dayInNanoSeconds;
        static constexpr double FACTOR364 = 364.0*DateTimeTools::dayInNanoSeconds;
        static constexpr double FACTOR360 = 360.0*DateTimeTools::dayInNanoSeconds;

};