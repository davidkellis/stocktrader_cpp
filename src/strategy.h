
#ifndef STRATEGY_H_
#define STRATEGY_H_

#include <iostream>
#include <vector>
#include "broker.h"
#include "time.h"
#include "constants.h"

using namespace std;

class Strategy {
public:
  static const int ABORT_STRATEGY = 1;
  static const int TRADING_DAYS[];
  static const Interval TRADING_HOURS[];

  Account* account;
  vector<string> tickers_to_trade;
  double amount_per_company;
  bool verbose;

  Strategy() {
  }

  Strategy(Account* p_account, vector<string> p_tickers_to_trade) {
    account = p_account;
    tickers_to_trade = p_tickers_to_trade;
    amount_per_company = account->cash / tickers_to_trade.size();
    verbose = false;
  }

  virtual ~Strategy() {
  }

  void run(ULL start_time, ULL end_time, Interval time_increment) {
    ULL t = start_time;

    if(!(within_trading_days(t) && within_trading_hours(t))) {
      t = next_trading_period(t);
    }

    try {
      while(t <= end_time) {
        amount_per_company = account->cash / tickers_to_trade.size();

        for(unsigned int i = 0; i < tickers_to_trade.size(); i++) {
          trade(tickers_to_trade[i], t);
        }

        t = increment_timestamp(t, time_increment);
      }
    } catch(int abort_code) {
      if(abort_code != ABORT_STRATEGY) {
        cout << "Abort code, " << abort_code << ", is undefined.";
      }
    }
  }

  virtual void trade(string ticker, ULL timestamp) {
  }

private:
  ULL increment_timestamp(ULL timestamp, Interval increment) {
    timestamp = timestamp + increment;
    if(!(within_trading_days(timestamp) && within_trading_hours(timestamp))) {
      timestamp = next_trading_period(timestamp);
    }
    return timestamp;
  }

  ULL next_trading_period(ULL t) {
    ULL ntp;
    int day_of_week = getDayOfWeek(t, 1);

    if(within_trading_days(t)) {
      Interval t_cmp = Interval(join_timestamp(0, timestamp_time(t)));
      if(t_cmp < TRADING_HOURS[0]) {
        // if t is a weekday and is before the start of trading hours, set ntp to the start of the trading hours for that day
        ntp = join_timestamp(timestamp_date(t), 0) + TRADING_HOURS[0];
      } else if(t_cmp > TRADING_HOURS[1]) {
        if(day_of_week == TRADING_DAYS[1]) {
          // if t is the last day in the trading week and is after the end of trading hours,
          //   set ntp to the first trading day of the following week
          ntp = Interval(0, 0, (TRADING_DAYS[0] + (7 - day_of_week)), 0, 0, 0) + t;
        } else {
          // if t is a weekday (but not the last day in the trading week) and is after the end of trading hours,
          //   set ntp to the following day
          ntp = Interval(0, 0, 1, 0, 0, 0) + t;
        }
        // set ntp to the start of the trading hours for the weekday that it represents
        ntp = join_timestamp(timestamp_date(ntp), 0) + TRADING_HOURS[0];
      }
    } else {
      if(day_of_week < TRADING_DAYS[0]) {
        // if t is one of the days before the first day in the trading week (e.g. Sunday),
        //   set ntp to the first day of this trading week
        ntp = Interval(0, 0, (TRADING_DAYS[0] - day_of_week), 0, 0, 0) + t;
      } else if (day_of_week > TRADING_DAYS[1]) {
        // if t is one of the days after the last day in the trading week (e.g. Saturday),
        //   set ntp to the first trading day of the following week
        ntp = Interval(0, 0, (TRADING_DAYS[0] + (7 - day_of_week)), 0, 0, 0) + t;
      }
      // set ntp to the start of the trading hours for the weekday that it represents
      ntp = join_timestamp(timestamp_date(ntp), 0) + TRADING_HOURS[0];
    }

    return ntp;
  }

  bool within_trading_days(ULL t) {
    int day_of_week = getDayOfWeek(Interval(t), 1);
    return TRADING_DAYS[0] <= day_of_week && day_of_week <= TRADING_DAYS[1];
  }

  bool within_trading_hours(ULL t) {
    Interval i = Interval(t);
    i.years = i.months = i.days = 0;
    return TRADING_HOURS[0] <= i && i <= TRADING_HOURS[1];
  }
};

#endif /* STRATEGY_H_ */
