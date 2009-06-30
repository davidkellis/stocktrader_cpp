
#ifndef EXCHANGE_H_
#define EXCHANGE_H_

#include <string>
#include <map>
#include <vector>
#include "pricehistory.h"

using namespace std;

class Exchange {
public:
  map<string,PriceHistory> price_histories;
  
  Exchange() {
  }
  
  Exchange(vector<string> tickers, string filename_prefix = "", string filename_suffix = ".csv") {
    for(unsigned int i = 0; i < tickers.size(); i++) {
      price_histories[tickers[i]] = PriceHistory(filename_prefix + tickers[i] + filename_suffix);
    }
  }
  
  virtual ~Exchange() {
  }

  void add_price_history(string ticker, string filename_prefix = "", string filename_suffix = "") {
    price_histories[ticker] = PriceHistory(filename_prefix + ticker + filename_suffix);
  }

  void add_price_history(vector<string> tickers, string filename_prefix = "", string filename_suffix = "") {
    for(unsigned int i = 0; i < tickers.size(); i++) {
      price_histories[tickers[i]] = PriceHistory(filename_prefix + tickers[i] + filename_suffix);
    }
  }
  
  void remove_price_history(string ticker) {
    price_histories.erase(ticker);
  }

  void remove_price_history(vector<string> tickers) {
    for(unsigned int i = 0; i < tickers.size(); i++) {
      price_histories.erase(tickers[i]);
    }
  }
  
  Bar* bar(string ticker, ULL timestamp) {
    map<string,PriceHistory>::iterator it;
    it = price_histories.find(ticker);
    if(it == price_histories.end()) {       // ticker not found in the exchange
      return NULL;
    } else {                                // ticker does exist in the exchange
      return it->second.search(timestamp);
    }
  }
  
  double quote(string ticker, ULL timestamp, double not_found_value = 0.0) {
    Bar *b = bar(ticker, timestamp);
    if(b) {       // bar was found and returned
      return b->close;
    } else {      // bar not found
      return not_found_value;
    }
  }
};

#endif /* EXCHANGE_H_ */
