
#ifndef PRICEHISTORY_H_
#define PRICEHISTORY_H_

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cstdlib>        // rand()
#include "csv.h"
#include "isearch.h"
#include "constants.h"

using namespace std;

class Bar {
public:
  static const double MIN_PRICE;

  ULL timestamp;
  int date, time;
  double open, high, low, close;

  Bar() {
    timestamp = 0;
  }

  // assume p_date and p_time will be 8 and 6 digits, respectively.
  Bar(int p_date, int p_time, double p_open, double p_high, double p_low, double p_close) {
    timestamp = (ULL)p_date * 1000000 + (ULL)p_time;
    date = p_date;
    time = p_time;
    open = p_open;
    high = p_high;
    low = p_low;
    close = p_close;
  }

  virtual ~Bar() {
  }

  string to_s() {
    stringstream retval;
    retval << timestamp << "," << date << "," << time << "," << open << "," << high << "," << low << "," << close;
    return retval.str();
  }
};

extern ULL bar_extractor(Bar& obj);

class PriceHistory {
public:
  vector<Bar> bars;
  
  PriceHistory() {
  }

  PriceHistory(vector<Bar> p_bars) {
    bars = p_bars;
  }

  PriceHistory(string filename) {
    string line;
    ifstream inFile(filename.c_str());
    Csv csv = Csv(inFile);

    while (csv.getline(line) != 0) {
      bars.push_back(Bar(csv.getint(0), csv.getint(1), csv.getdouble(2), csv.getdouble(3), csv.getdouble(4), csv.getdouble(5)));
    }

    inFile.close();
  }

  virtual ~PriceHistory() {
  }

  long search_index(ULL timestamp, int not_found_direction = DIR_LEFT) {
    return interpolation_search(bars, timestamp, not_found_direction, bar_extractor);
    return 0;
  }
  
  Bar* search(ULL timestamp, int not_found_direction = DIR_LEFT) {
    long i = search_index(timestamp, not_found_direction);
    if(i >= 0) {
      return &bars[i];
    } else {
      return NULL;
    }
  }

  Bar& rand() {
    return bars[std::rand() % bars.size()];
  }

  Bar& last() {
    return bars[bars.size() - 1];
  }
};

extern int test_price_history();

#endif /* PRICEHISTORY_H_ */
