#include <cassert>
#include <iostream>
#include "../include/cpp-quant/tools/scheduler.hpp"

int main() {
    // Setup: utils with no holidays, ACT/365, business day adjustment = true
    Scheduler utils(true, HolidayCalendar::NONE, DayCountConvention::ACTUAL_365);

    // 1. Check weekend detection
    // Epoch 0 (Jan 1, 1970) is Thursday → not weekend
    DateTime jan1_1970(0, EpochTimestampType::SECONDS);
    assert(utils.isWeekEnd(jan1_1970) == false);

    // Let's pick Jan 3, 1970 (Saturday)
    DateTime jan3_1970(2 * 24 * 3600, EpochTimestampType::SECONDS); 
    assert(utils.isWeekEnd(jan3_1970) == true);

    // 2. Check getNextBusinessDay
    DateTime next_bd = utils.getNextBusinessDay(jan3_1970); 
    // Jan 3, 1970 = Saturday → next business day = Monday Jan 5, 1970
    DateTime expected_mon(4 * 24 * 3600, EpochTimestampType::SECONDS);
    assert(next_bd == expected_mon);

    // 3. Check year fraction (ACT/365 convention)
    DateTime start(0, EpochTimestampType::SECONDS);                          // Jan 1, 1970
    DateTime end(365 * 24 * 3600, EpochTimestampType::SECONDS);              // Jan 1, 1971
    double yf = utils.getYearFraction(start, end);
    assert(std::abs(yf - 1.0) < 1e-9);

    // 4. Check forward date calculation with tenor (days)
    Tenor t1(11, TenorType::DAYS);
    DateTime forward = utils.getForwardDateTime(start, t1);
    DateTime expected_forward(11 * 24 * 3600, EpochTimestampType::SECONDS);
    assert(forward == expected_forward);

    // 5. Check sequence generation (monthly tenor for 6 months)
    DateTime ref(0, EpochTimestampType::SECONDS);  // Jan 1, 1970
    Tenor freq(1, TenorType::MONTHS);
    Tenor mat(6, TenorType::MONTHS);
    std::vector<DateTime> seq = utils.getDateTimeSequence(ref, freq, mat);
    assert(seq.size() == 7);   // expect 6 points


    std::cout << "All Scheduler tests passed!" << std::endl;
    return 0;
}

