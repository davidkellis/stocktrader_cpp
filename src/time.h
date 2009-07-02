/*
 * time.h
 *
 *  Created on: Jun 18, 2009
 *      Author: davidkellis
 */

#ifndef TIME_H_
#define TIME_H_

#include <iostream>
#include <cmath>            // floor()
#include <sstream>
#include <string>
#include "constants.h"

using namespace std;

class Interval;

int leg(const Interval& lhs, const Interval& rhs);
bool operator<(const Interval& lhs, const Interval& rhs);
bool operator<=(const Interval& lhs, const Interval& rhs);
bool operator>(const Interval& lhs, const Interval& rhs);
bool operator>=(const Interval& lhs, const Interval& rhs);
bool operator==(const Interval& lhs, const Interval& rhs);
bool operator!=(const Interval& lhs, const Interval& rhs);

Interval operator+(const Interval& lhs, const Interval& rhs);
Interval operator-(const Interval& lhs, const Interval& rhs);
Interval operator/(const Interval& lhs, const long &rhs);
ULL operator+(const Interval& i, const ULL& ts);
ULL operator+(const ULL& ts, const Interval& i);
ULL operator-(const ULL& ts, const Interval& i);

int days_in_month(int year, int month);
bool leap_year(int year);

void split_timestamp(ULL timestamp, int& out_date, int& out_time);
void split_timestamp(ULL timestamp, int& out_year, int& out_month, int& out_day, int& out_hour, int& out_minute, int& out_second);
ULL join_timestamp(int date, int time);
ULL join_timestamp(int year, int month, int day, int hour, int minute, int second);
int timestamp_date(ULL timestamp);
int timestamp_time(ULL timestamp);
void date_to_ymd(int p_date, int& out_year, int& out_month, int& out_day);
void time_to_hms(int p_time, int& out_hour, int& out_minute, int& out_second);

int getDayOfWeek(int month, int day, int year, int CalendarSystem);
int getDayOfWeek(Interval i, int CalendarSystem);
int getDayOfWeek(ULL timestamp, int CalendarSystem);

void test_date_time_implementation();


class Interval {
public:
  long years;
  long months;
  long days;
  long hours;
  long minutes;
  long seconds;

  static Interval between(ULL lhs, ULL rhs) {
    Interval l = Interval(lhs);
    Interval r = Interval(rhs);
    
    Interval i;
    //cout << leg(l,r) << endl;
    if(l > r) {
      i = l - r;
    } else if(l < r) {
      i = r - l;
    } else {
      // i is already initialized with the default constructor, so the initial values are all 0 (which is what we want).
    }
    return i;
  }

  Interval() {
    years = months = days = hours = minutes = seconds = 0;
  }

  Interval(long p_years, long p_months, long p_days, long p_hours, long p_minutes, long p_seconds) {
    seconds = p_seconds;
    minutes = p_minutes;
    hours = p_hours;
    days = p_days;
    months = p_months;
    years = p_years;
  }

  /*
   * Assuming timestamp is in the form YYYYMMDDhhmmss, extract the constituent parts.
   */
  Interval(ULL timestamp) {
    years = timestamp / 10000000000ULL;
    timestamp %= 10000000000ULL;

    months = timestamp / 100000000;
    timestamp %= 100000000;

    days = timestamp / 1000000;
    timestamp %= 1000000;

    hours = timestamp / 10000;
    timestamp %= 10000;

    minutes = timestamp / 100;
    timestamp %= 100;

    seconds = timestamp;
  }

  virtual ~Interval() {
  }

  Interval operator-() const {
    Interval i = Interval(-years, -months, -days, -hours, -minutes, -seconds);
    return i;
  }
  
  string to_s() const {
    stringstream ss;
    ss << years << " years " << months << " months " << days << " days " << hours << " hours " << minutes << " minutes " << seconds << " seconds";
    return ss.str();
  }
};

#endif /* TIME_H_ */
