
#ifndef BROKER_H_
#define BROKER_H_

#include <vector>
#include <algorithm>    // min()
#include <cmath>        // floor()
#include <map>
#include <sstream>
#include <string>
#include "ext.h"
#include "exchange.h"

using namespace std;

const ULL MAX_TIMESTAMP = 99999999999999ULL;

class Broker;

class Account {
public:
  Broker *broker;
  map<string,int> portfolio;    // ticker, share_count
  double initial_cash;
  double cash;
  double commission_paid;
  
  Account() {
    broker = NULL;
    cash = 0;
    initial_cash = cash;
    commission_paid = 0;
  }
  
  Account(Broker *p_broker, double p_cash) {
    broker = p_broker;
    cash = p_cash;
    initial_cash = cash;
    commission_paid = 0;
  }
  
  virtual ~Account() {
  }
  
  void reset() {
    cash = initial_cash;
    portfolio.clear();
    commission_paid = 0;
  }
  
  double value(ULL timestamp);

  int buy(string ticker, int shares, ULL timestamp, bool on_margin = false);

  int buy_amap(string ticker, ULL timestamp, double amount = -1);

  int sell(string ticker, int shares, ULL timestamp);

  int sell_short(string ticker, int shares, ULL timestamp);

  int sell_short_amt(string ticker, ULL timestamp, double amount = 0.0);

  int sell_all(string ticker, ULL timestamp);

  string to_s(ULL timestamp = MAX_TIMESTAMP) {
    stringstream retval;
    retval << "cash: " << cash << endl;
    retval << "commission_paid: " << commission_paid << endl;
    retval << "portfolio holdings as of " << timestamp << ": ";
    for(map<string,int>::const_iterator it = portfolio.begin(); it != portfolio.end(); ++it) {
      retval << " " << it->first << " -> " << it->second << endl;
    }
    retval << "value: " << value(timestamp);
    return retval.str();
  }
};

class Broker {
public:
  Exchange *exchange;
  vector<Account*> new_accounts;
  double buy_commission;
  double sell_commission;
  
  Broker() {
    exchange = NULL;
  }
  
  Broker(Exchange *p_exchange, double p_buy_commission, double p_sell_commission) {
    exchange = p_exchange;
    buy_commission = p_buy_commission;
    sell_commission = p_sell_commission;
  }
  
  virtual ~Broker() {
    for(unsigned int i = 0; i < new_accounts.size(); i++) {
      delete new_accounts[i];
    }
  }
  
  Account* new_account(double cash) {
    Account *a = new Account(this, cash);
    new_accounts.push_back(a);
    return a;
  }

  Bar* bar(string ticker, ULL timestamp) {
    return exchange->bar(ticker, timestamp);
  }

  double quote(string ticker, ULL timestamp, double not_found_value = 0.0) {
    return exchange->quote(ticker, timestamp, not_found_value);
  }

  int buy(Account& account, string ticker, int shares, ULL timestamp, bool on_margin = false) {
    double cost = exchange->quote(ticker, timestamp, 0.0) * shares;
    int retval = 0;
    if(cost >= 0 && (account.cash >= (cost + buy_commission) || on_margin)) {
      account.cash -= cost + buy_commission;
      account.portfolio[ticker] += shares;
      account.commission_paid += buy_commission;
      retval = shares;
    }
    return retval;
  }

  int buy_amap(Account& account, string ticker, ULL timestamp, double amount = -1) {
    int retval = 0;
    if(amount < 0) {
      amount = account.cash;
    }
    amount = min(amount, account.cash);
    double quote = exchange->quote(ticker, timestamp, 0.0);
    if(quote > 0) {
      int max_shares = (int)floor((amount - buy_commission) / quote);
      double cost = quote * max_shares;
      if(max_shares > 0 && account.cash >= cost + buy_commission) {
        account.cash -= cost + buy_commission;
        account.portfolio[ticker] += max_shares;
        account.commission_paid += buy_commission;
        retval = max_shares;
      }
    }
    return retval;
  }

  int sell(Account& account, string ticker, int shares, ULL timestamp) {
    int retval = 0;
    int shares_held = map_get(account.portfolio, ticker, 0);
    shares = min(shares, shares_held);
    double gross_profit = exchange->quote(ticker, timestamp, 0.0) * shares;
    double post_sale_cash_balance = account.cash + gross_profit;
    if(gross_profit >= 0 && post_sale_cash_balance >= sell_commission && shares_held > 0 && shares >0) {
      account.cash = post_sale_cash_balance - sell_commission;
      account.portfolio[ticker] -= shares;
      account.commission_paid += sell_commission;
      retval = shares;
    }
    return retval;
  }

  int sell_short(Account& account, string ticker, int shares, ULL timestamp) {
    int retval = 0;
    double gross_profit = exchange->quote(ticker, timestamp, 0.0) * shares;
    double post_sale_cash_balance = account.cash + gross_profit;
    if(gross_profit >= 0 && post_sale_cash_balance >= sell_commission && shares > 0) {
      account.cash = post_sale_cash_balance - sell_commission;
      account.portfolio[ticker] -= shares;
      account.commission_paid += sell_commission;
      retval = shares;
    }
    return retval;
  }

  int sell_short_amt(Account& account, string ticker, ULL timestamp, double amount = 0.0) {
    int shares = 0;
    if(amount == 0 && account.cash > 0) {
      amount = account.cash;
    }
    int max_shares = (int)floor((double)(amount - sell_commission) / exchange->quote(ticker, timestamp, -1));
    if(max_shares > 0) {
      shares = sell_short(account, ticker, max_shares, timestamp);
    }
    return shares;
  }

  int sell_all(Account& account, string ticker, ULL timestamp) {
    return sell(account, ticker, map_get(account.portfolio, ticker, 0), timestamp);
  }
};


#endif /* BROKER_H_ */
