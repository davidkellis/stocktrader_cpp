/*
 * time.cpp
 *
 *  Created on: Jun 18, 2009
 *      Author: davidkellis
 */

#include "time.h"

// lesser-equal-greater (aka leg) behaves like Ruby's <=> operator.
//   returns:
//     -1 if lhs < rhs
//      0 if lhs == rhs
//      1 if lhs > rhs
int leg(const Interval& lhs, const Interval& rhs) {
  ULL timestamp = 20000101000000ULL;      // the start of the year 2000, at midnight
  
  // how far is lhs from 2000-01-01 @ 00:00:00 ?
  ULL lhs_ts = timestamp + lhs;
  
  // how far is rhs from 2000-01-01 @ 00:00:00 ?
  ULL rhs_ts = timestamp + rhs;
  
  //cout << timestamp << " + " << lhs.to_s() << " = " << lhs_ts << endl;
  //cout << timestamp << " + " << rhs.to_s() << " = " << rhs_ts << endl;
  
  int retval;
  if(lhs_ts > rhs_ts) {                 // lhs is a larger interval than rhs
    retval = 1;
  } else if(lhs_ts < rhs_ts) {          // lhs is a smaller interval than rhs
    retval = -1;
  } else {                              // the intervals are equal
    retval = 0;
  }
  return retval;
}

bool operator<(const Interval& lhs, const Interval& rhs) {
  return leg(lhs, rhs) < 0;
}

bool operator<=(const Interval& lhs, const Interval& rhs) {
  return leg(lhs, rhs) <= 0;
}

bool operator>(const Interval& lhs, const Interval& rhs) {
  return leg(lhs, rhs) > 0;
}

bool operator>=(const Interval& lhs, const Interval& rhs) {
  return leg(lhs, rhs) >= 0;
}

bool operator==(const Interval& lhs, const Interval& rhs) {
  return leg(lhs, rhs) == 0;
}

bool operator!=(const Interval& lhs, const Interval& rhs) {
  return leg(lhs, rhs) != 0;
}

Interval operator+(const Interval& lhs, const Interval& rhs) {
  Interval i;
  i.years = lhs.years + rhs.years;
  i.months = lhs.months + rhs.months;
  i.days = lhs.days + rhs.days;
  i.hours = lhs.hours + rhs.hours;
  i.minutes = lhs.minutes + rhs.minutes;
  i.seconds = lhs.seconds + rhs.seconds;
  return i;
}

Interval operator-(const Interval& lhs, const Interval& rhs) {
  Interval i;
  i.years = lhs.years - rhs.years;
  i.months = lhs.months - rhs.months;
  i.days = lhs.days - rhs.days;
  i.hours = lhs.hours - rhs.hours;
  i.minutes = lhs.minutes - rhs.minutes;
  i.seconds = lhs.seconds - rhs.seconds;
  return i;
}

Interval operator/(const Interval& lhs, const long& divisor) {
  Interval i;
  if(divisor != 0) {
    LL s = 0;
    s += (LL)(31556926 * (LL)lhs.years + 2629744 * (LL)lhs.months + 86400 * (LL)lhs.days + 3600 * (LL)lhs.hours + 60 * (LL)lhs.minutes + (LL)lhs.seconds);
    s /= divisor;
    i.years = s / 31556926;
    s %= 31556926;
    i.months = s / 2629744;
    s %= 2629744;
    i.days = s / 86400;
    s %= 86400;
    i.hours = s / 3600;
    s %= 3600;
    i.minutes = s / 60;
    s %= 60;
    i.seconds = s;
  }
  return i;
}

ULL operator+(const Interval& i, const ULL& ts) {
  return ts + i;
}

/*
 * Normalization of hours, minutes, and seconds is different than normalization of years, months, and day.
 * A specific time is a number of hours, minutes, and seconds that has already elapsed (e.g. 3:00 PM means
 * that 15 hours of the day have elapsed). A specific date is not a number of years, months, and days that
 * have elapsed, they indicate the current year, month, and day -- they indicate which year, month, and day
 * that we are in (e.g. 12/3/2009 at 8:00 AM means that 2008 years, 11 months, 2 days, 8 hours have elapsed).
 */
ULL operator+(const ULL& ts, const Interval& i) {
//  ULL t = 0;
//  int year,month,day,hours,minutes,seconds;
//  year = month = day = hours = minutes = seconds = 0;

  ULL t = 0;
  int year,month,day,hours,minutes,seconds;
  long inc = 0;
  long multiple;
  year = month = day = hours = minutes = seconds = 0;

  date_to_ymd(timestamp_date(ts), year, month, day);
  time_to_hms(timestamp_time(ts), hours, minutes, seconds);

  // add the interval's seconds, minutes, hours, and years to the values extracted from the timestamp (ts);
  //   we'll add the months and days later
  seconds += i.seconds;
  minutes += i.minutes;
  hours += i.hours;
  year += i.years;

  // Since adding a number of months might affect the day, we want to normalize the months before
  //   we go into normalizing the days.
  // For example: 1/29/2009 + 1 month = 1/28/2009
  //         but: 1/29/2008 + 1 month = 1/29/2009
  //        Also: 1/30/2009 + 1 month = 1/28/2009
  //         and: 1/31/2009 + 2 months = 3/31/2009
  // ** I am assuming the the day we fall on depends on how many days the target month has in it -- If  **
  // ** the target month has fewer days in it than the origin month, and the origin day is a day that   **
  // ** the target month doesn't have, then the target day will be the last day of the target month.    **

  // normalize month count
  //int origin_month_length = days_in_month(year, month);
  month += i.months;
  // figure out how many years worth of months we can increment our year count by,
  //   and determine which month we will be in after we remove that number of years worth of months from the month count.
  /*
  if(month > 12) {                                     
    year += (month - 1) / 12;
    month = ((month - 1) % 12) + 1;
  }
  */
  
  // see http://tinyurl.com/nu97yg for a demonstration of why this math works
  multiple = 12;
  inc = floor((double)(month - 1) / multiple);
  year += inc;
  month = ((month - 1) - multiple*inc) + 1;

  int target_month_length = days_in_month(year, month);
  if(/* target_month_length < origin_month_length && */ day > target_month_length) {     // adjust the day of the month if we need to
    day = target_month_length;                        // set the day to the last day in the (shorter) target month
  }

  /*
  // normalize seconds, minutes, and hours.
  minutes += seconds / 60;
  seconds %= 60;

  hours += minutes / 60;
  minutes %= 60;

  // we've already normalized the month count so adding to the day count is ok here (we'll normalize the day count afterward).
  day += hours / 24;
  hours %= 24;
  */

  // normalize seconds, minutes, and hours.
  multiple = 60;
  inc = floor((double)seconds / multiple);
  minutes += inc;
  seconds = seconds - multiple*inc;

  inc = floor((double)minutes / multiple);
  hours += inc;
  minutes = minutes - multiple*inc;

  // we've already normalized the month count so adding to the day count is ok here (we'll normalize the day count afterward).
  multiple = 24;
  inc = floor((double)hours / multiple);
  day += inc;
  hours = hours - multiple*inc;

  // normalize day count
  multiple = 12;
  day += i.days;
  int dpm = days_in_month(year, month);
  //cout << day << " " << dpm << endl;
  while(day > dpm) {
    day -= dpm;
    month++;

    // re-normalize month if needed
    inc = floor((double)(month - 1) / multiple);
    year += inc;
    month = ((month - 1) - multiple*inc) + 1;

    dpm = days_in_month(year, month);
  }
  while(day < 1) {
    month--;
    dpm = days_in_month(year, month);
    day += dpm;

    // re-normalize month if needed
    inc = floor((double)(month - 1) / multiple);
    year += inc;
    month = ((month - 1) - multiple*inc) + 1;

    //dpm = days_in_month(year, month);
  }

  t = join_timestamp(year, month, day, hours, minutes, seconds);

  return t;
}

/*
 * Normalization of hours, minutes, and seconds is different than normalization of years, months, and day.
 * A specific time is a number of hours, minutes, and seconds that has already elapsed (e.g. 3:00 PM means
 * that 15 hours of the day have elapsed). A specific date is not a number of years, months, and days that
 * have elapsed, they indicate the current year, month, and day -- they indicate which year, month, and day
 * that we are in (e.g. 12/3/2009 at 8:00 AM means that 2008 years, 11 months, 2 days, 8 hours have elapsed).
 */
ULL operator-(const ULL& ts, const Interval& i) {
  return ts + -i;
  
  /*
  ULL t = 0;
  int year,month,day,hours,minutes,seconds;
  long inc = 0;
  long multiple;
  year = month = day = hours = minutes = seconds = 0;

  date_to_ymd(timestamp_date(ts), year, month, day);
  time_to_hms(timestamp_time(ts), hours, minutes, seconds);

  // add the interval's seconds, minutes, hours, and years to the values extracted from the timestamp (ts);
  //   we'll add the months and days later
  seconds -= i.seconds;
  minutes -= i.minutes;
  hours -= i.hours;
  year -= i.years;

  // Since adding a number of months might affect the day, we want to normalize the months before
  //   we go into normalizing the days.
  // For example: 1/29/2009 + 1 month = 1/28/2009
  //         but: 1/29/2008 + 1 month = 1/29/2009
  //        Also: 1/30/2009 + 1 month = 1/28/2009
  //         and: 1/31/2009 + 2 months = 3/31/2009
  // ** I am assuming the the day we fall on depends on how many days the target month has in it -- If  **
  // ** the target month has fewer days in it than the origin month, and the origin day is a day that   **
  // ** the target month doesn't have, then the target day will be the last day of the target month.    **

  // normalize month count
  //int origin_month_length = days_in_month(year, month);
  month -= i.months;
  // figure out how many years worth of months we can decrement our year count by,
  //   and determine which month we will be in after we add that number of years worth of months from the month count.
  multiple = 12;
  inc = floor((double)(month - 1) / multiple);
  year += inc;
  month = ((month - 1) - multiple*inc) + 1;

  int target_month_length = days_in_month(year, month);
  if(day > target_month_length) {     // adjust the day of the month if we need to
    day = target_month_length;                        // set the day to the last day in the (shorter) target month
  }

  // normalize seconds, minutes, and hours.
  multiple = 60;
  inc = floor((double)seconds / multiple);
  minutes += inc;
  seconds = seconds - multiple*inc;

  inc = floor((double)minutes / multiple);
  hours += inc;
  minutes = minutes - multiple*inc;

  // we've already normalized the month count so adding to the day count is ok here (we'll normalize the day count afterward).
  multiple = 24;
  inc = floor((double)hours / multiple);
  day += inc;
  hours = hours - multiple*inc;

  // normalize day count
  multiple = 12;
  day -= i.days;
  int dpm = days_in_month(year, month);

//  while(day < 1) {
//    day += dpm;
//    month--;
//
//    // re-normalize month if needed
//    inc = floor((double)(month - 1) / multiple);
//    year += inc;
//    month = ((month - 1) - multiple*inc) + 1;
//
//    dpm = days_in_month(year, month);
//  }

  while(day < 1) {
    month--;
    dpm = days_in_month(year, month);
    day += dpm;
    
    // re-normalize month if needed
    inc = floor((double)(month - 1) / multiple);
    year += inc;
    month = ((month - 1) - multiple*inc) + 1;
    
    //dpm = days_in_month(year, month);
  }
  while(day > dpm) {
    day -= dpm;
    month++;

    // re-normalize month if needed
    inc = floor((double)(month - 1) / multiple);
    year += inc;
    month = ((month - 1) - multiple*inc) + 1;

    dpm = days_in_month(year, month);
  }

  t = join_timestamp(year, month, day, hours, minutes, seconds);

  return t;
   */
}


// month should be between 1 and 12, inclusive.
int days_in_month(int year, int month) {
  if(month == 2) {
    if(leap_year(year)) {
      return 29;
    } else {
      return 28;
    }
  } else if(month == 4 || month == 6 || month == 9 || month == 11) {
    return 30;
  } else {
    return 31;
  }
}

bool leap_year(int year) {
  if(year % 4 == 0) {               // Every year that is exactly divisible by four is a leap year,
    if(year % 100 == 0) {           // except for years that are exactly divisible by 100;
      if(year % 400 == 0) {         // centurial years that are exactly divisible by 400 are still leap years
        return true;                // divisible by 4, 100, and 400 -> leap year
      }
      return false;                 // divisible by 100 and not centurial year -> not leap year
    }
    return true;                    // divisible by 4 and not 100 -> leap year
  }
  return false;                     // not divisible by 4 -> not leap year
}

/*
 * Assuming timestamp is in the form YYYYMMDDhhmmss, extract out the date and time.
 */
void split_timestamp(ULL timestamp, int& out_date, int& out_time) {
  out_date = 0;
  out_time = 0;

  out_date = timestamp / 1000000;
  out_time = timestamp % 1000000;
}

void split_timestamp(ULL timestamp, int& out_year, int& out_month, int& out_day, int& out_hour, int& out_minute, int& out_second) {
  date_to_ymd(timestamp_date(timestamp), out_year, out_month, out_day);
  time_to_hms(timestamp_time(timestamp), out_hour, out_minute, out_second);
}

ULL join_timestamp(int date, int time) {
  ULL t = 0;
  t = date * 1000000ULL;
  t += time;
  return t;
}

ULL join_timestamp(int year, int month, int day, int hour, int minute, int second) {
  ULL t = 0;
  t = year * 10000000000ULL;
  t += month * 100000000;
  t += day * 1000000;
  t += hour * 10000;
  t += minute * 100;
  t += second;
  return t;
}

int timestamp_date(ULL timestamp) {
  return timestamp / 1000000;
}

int timestamp_time(ULL timestamp) {
  return timestamp % 1000000;
}

/*
 * Assuming p_date is in the form YYYYMMDD, extract out the year, month, and day.
 */
void date_to_ymd(int p_date, int& out_year, int& out_month, int& out_day) {
  out_year = 0;
  out_month = 0;
  out_day = 0;

  out_year = p_date / 10000;
  p_date %= 10000;                      //p_date -= out_year * 10000;
  out_month = p_date / 100;
  p_date %= 100;                        //p_date -= out_month * 100;
  out_day = p_date;
}

/*
 * Assuming p_time is in the form HHMMDD, extract out the hours, minutes, and seconds.
 */
void time_to_hms(int p_time, int& out_hour, int& out_minute, int& out_second) {
  out_hour = 0;
  out_minute = 0;
  out_second = 0;

  out_hour = p_time / 10000;
  p_time %= 10000;                      //p_time -= out_hour * 10000;
  out_minute = p_time / 100;
  p_time %= 100;                        //p_time -= out_minute * 100;
  out_second = p_time;
}

// Returns an integer representing the day of the week, 0..6, with Sunday == 0.
//   Behaves the same as Ruby's Time#wday method.
// This function found at http://everything2.com/node/1023392. Commment by "in10se".
// This function is also implemented similarly in the BOOST Date Time library.
int getDayOfWeek(int month, int day, int year, int CalendarSystem) {
  // CalendarSystem = 1 for Gregorian Calendar
  if (month < 3) {
    month = month + 12;
    year = year - 1;
  }

  // magic:
  return (day
          + (2 * month)
          + int(6 * (month + 1) / 10)
          + year
          + int(year / 4)
          - int(year / 100)
          + int(year / 400)
          + CalendarSystem
          ) % 7;
}

int getDayOfWeek(Interval i, int CalendarSystem) {
  return getDayOfWeek(i.months, i.days, i.years, CalendarSystem);
}

int getDayOfWeek(ULL timestamp, int CalendarSystem) {
  int year, month, day;
  year = month = day = 0;
  date_to_ymd(timestamp_date(timestamp), year, month, day);
  return getDayOfWeek(month, day, year, CalendarSystem);
}

void test_date_time_implementation() {
  ULL timestamp = 20090618001732ULL;
  int dt;
  int tm;
  int Y,M,D,h,m,s;

  split_timestamp(timestamp, dt, tm);
  date_to_ymd(dt, Y, M, D);
  time_to_hms(tm, h, m, s);

  cout << "Timestamp: " << timestamp << endl;
  cout << "Date: " << dt << " -> " << Y << "/" << M << "/" << D << endl;
  cout << "Time: " << tm << " -> " << h << ":" << m << ":" << s << endl << endl;

  UL seconds = 31834133;
  Interval i = Interval(0,0,0,0,0,seconds);
  cout << "20080101 + 31834133 seconds  = " << join_timestamp(20080101, 0) + i << endl;
  cout << "20080101 - -31834133 seconds = " << join_timestamp(20080101, 0) - -i << endl;
  cout << "20080101 - 31834133 seconds  = " << join_timestamp(20080101, 0) - i << endl;
  cout << "20080101 + -31834133 seconds = " << join_timestamp(20080101, 0) + -i << endl;

  cout << endl;
  
  i = Interval(1, 3, 3, 25, 7, 38);
  cout << "20090601 + (1 year 3 months 3 days 25 hours 7 minutes 38 seconds)  = " << join_timestamp(20090601, 0) + i << endl;
  cout << "20090601 - -(1 year 3 months 3 days 25 hours 7 minutes 38 seconds) = " << join_timestamp(20090601, 0) - (-i) << endl;
  cout << "20090601 - (1 year 3 months 3 days 25 hours 7 minutes 38 seconds)  = " << join_timestamp(20090601, 0) - i << endl;
  cout << "20090601 + -(1 year 3 months 3 days 25 hours 7 minutes 38 seconds) = " << join_timestamp(20090601, 0) + (-i) << endl;

  cout << endl;
  
  Interval l = Interval(2, 3, 32, 25, 7, 112);
  Interval r = Interval(1, 15, 33, 1, 8, 52);
  cout << "(2 years 3 months 32 days 25 hours 7 minutes 112 seconds) <=> (1 year 15 months 33 days 1 hour 8 minutes 52 seconds) = " << leg(l,r) << endl;

  cout << endl;
  
  l = Interval(0,0,0,0,0,31834133);
  r = Interval(0, 0, 368, 10, 48, 52);
  cout << "(31834133 seconds) <=> (368 days 10 hours 48 minutes 52 seconds) = " << leg(l,r) << endl;
  cout << "(31834133 seconds) > (368 days 10 hours 48 minutes 52 seconds) = " << ((l > r) ? "true" : "false") << endl;
  r = Interval(0, 0, 368, 10, 48, 53);
  cout << "(31834133 seconds) <=> (368 days 10 hours 48 minutes 53 seconds) = " << leg(l,r) << endl;
  cout << "(31834133 seconds) == (368 days 10 hours 48 minutes 53 seconds) = " << ((l == r) ? "true" : "false") << endl;
  r = Interval(0, 0, 368, 10, 48, 54);
  cout << "(31834133 seconds) <=> (368 days 10 hours 48 minutes 54 seconds) = " << leg(l,r) << endl;
  cout << "(31834133 seconds) < (368 days 10 hours 48 minutes 54 seconds) = " << ((l < r) ? "true" : "false") << endl;
  
  cout << endl;
  
  ULL t1 = 19831129031500ULL;       // my birth date (time?)
  ULL t2 = 20090626005206ULL;       // now
  i = Interval::between(t1, t2);
  l = Interval(25, 6, 27, 21, 37, 6);
  r = Interval(0, 0, 9340, 21, 37, 6);
  cout << "Interval between 1983-11-29 03:15:00 and 2009-06-26 00:52:06 = " << i.to_s() << endl;
  cout << i.to_s() << " <=> " << l.to_s() << " = " << leg(i, l) << endl;
  cout << i.to_s() << " <=> " << r.to_s() << " = " << leg(i, r) << endl;
  cout << l.to_s() << " <=> " << r.to_s() << " = " << leg(l, r) << endl;
  
  cout << endl;
  
  l = Interval(0, 1, -29, 4, -29, 0);
  r = Interval(0, 0, 2, 3, 31, 0);
  cout << l.to_s() << " <=> " << r.to_s() << " = " << leg(l, r) << endl;
  cout << l.to_s() << " < " << r.to_s() << "  -> " << ((l < r) ? "true" : "false") << endl;
  cout << l.to_s() << " == " << r.to_s() << " -> " << ((l == r) ? "true" : "false") << endl;
  cout << l.to_s() << " > " << r.to_s() << "  -> " << ((l > r) ? "true" : "false") << endl;
  
  cout << endl;
  
  i = Interval(390, -1354, -3395, -6671, -5719, 0);       // ~9.7556 days
  long divisor = 10000;
  cout << i.to_s() << " / " << divisor << " = " << (i / divisor).to_s() << endl;
  
  cout << endl;
  
  cout << "Day of week of 20090623 is a " << getDayOfWeek(6, 23, 2009, 1);
}
