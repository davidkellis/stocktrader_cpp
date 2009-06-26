
#include "broker.h"

// This has to go here because C++ considers it a circular dependency problem if implemented in the class definition.
double Account::value(ULL timestamp) {
  double stock_value = 0;
  map<string,int>::iterator it;
  
  for(it = portfolio.begin(); it != portfolio.end(); it++) {
    // stock_value += quote(ticker, timestamp) * share_count
    stock_value += broker->exchange->quote((*it).first, timestamp, 0.0) * (*it).second;
  }
  
  return stock_value + cash;
}

int Account::buy(string ticker, int shares, ULL timestamp, bool on_margin) {
  return broker->buy((*this), ticker, shares, timestamp, on_margin);
}

int Account::buy_amap(string ticker, ULL timestamp, double amount) {
  return broker->buy_amap((*this), ticker, timestamp, amount);
}

int Account::sell(string ticker, int shares, ULL timestamp) {
  return broker->sell((*this), ticker, shares, timestamp);
}

int Account::sell_short(string ticker, int shares, ULL timestamp) {
  return broker->sell_short((*this), ticker, shares, timestamp);
}

int Account::sell_short_amt(string ticker, ULL timestamp, double amount) {
  return broker->sell_short_amt((*this), ticker, timestamp, amount);
}

int Account::sell_all(string ticker, ULL timestamp) {
  return broker->sell_all((*this), ticker, timestamp);
}
