
#ifndef EXPECTATIONMEANSTRATEGY_H_
#define EXPECTATIONMEANSTRATEGY_H_

#include <string>
#include "pricehistory.h"
#include "constants.h"
#include "strategy.h"

using namespace std;

class ExpectationMeanStrategy: public Strategy {
public:
  enum MODES {LONG = 0, SHORT = 1};

  ULL origin_timestamp, exit_timestamp;
  double small_gain, large_gain, small_loss, large_loss;
  double origin_price;
  int origin_shares;
  int state;
  int mode;

  ExpectationMeanStrategy() {
    origin_timestamp = exit_timestamp = 0;
    small_gain = large_gain = small_loss = large_loss = 0;

    reset();
  }

  ExpectationMeanStrategy(Account* p_account, vector<string> p_tickers_to_trade, double p_small_gain,
                          double p_large_gain, double p_small_loss, double p_large_loss) : Strategy(p_account, p_tickers_to_trade) {
    origin_timestamp = exit_timestamp = 0;
    small_gain = p_small_gain;
    large_gain = p_large_gain;
    small_loss = p_small_loss;
    large_loss = p_large_loss;

    reset();
  }

  virtual ~ExpectationMeanStrategy() {
  }

  void trade(string ticker, ULL timestamp) {
    if(state == 0) {
      enter_position(ticker, timestamp);
      state = 1;
    } else if(state == 1) {
      double gain = current_gain(ticker, timestamp);
      if(gain >= small_gain || gain <= large_loss) {
        exit_position(ticker, timestamp);
      } else if(gain <= small_loss) {
        state = 2;
      }
    } else if(state == 2) {
      double gain = current_gain(ticker, timestamp);
      if(gain >= large_gain || gain <= large_loss) {
        exit_position(ticker, timestamp);
      }
    }
  }

  void reset() {
    account->reset();
    state = 0;
    mode = rand() % 2;
    origin_price = 0;
  }

  double current_gain(string ticker, ULL timestamp) {
    double retval = 0;
    double price = account->broker->quote(ticker, timestamp);
    switch(mode) {
    case LONG:
      retval = (price / origin_price) - 1;
      break;
    case SHORT:
      if(price > 0) {
        retval = (origin_price / price) - 1;
      } else {
        retval = (origin_price / Bar::MIN_PRICE) - 1;
      }
      break;
    }
    return retval;
  }

  void enter_position(string ticker, ULL timestamp) {
    int s = 0;
    switch(mode) {
    case LONG:
      s = account->buy_amap(ticker, timestamp, amount_per_company);
      break;
    case SHORT:
      s = account->sell_short_amt(ticker, timestamp, amount_per_company);
      break;
    }

    origin_timestamp = timestamp;
    origin_price = account->broker->quote(ticker, timestamp);
    origin_shares = s;
  }

  void exit_position(string ticker, ULL timestamp) {
    int s = 0;
    switch(mode) {
    case LONG:
      s = account->sell_all(ticker, timestamp);
      break;
    case SHORT:
      s = account->buy(ticker, origin_shares, timestamp, true);         // buy [potentially] on margin
      break;
    }

    exit_timestamp = timestamp;

    throw ABORT_STRATEGY;
  }
};

#endif /* EXPECTATIONMEANSTRATEGY_H_ */
