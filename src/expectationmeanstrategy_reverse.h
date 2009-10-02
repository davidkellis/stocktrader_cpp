
#ifndef EXPECTATIONMEANSTRATEGY_REVERSE_H_
#define EXPECTATIONMEANSTRATEGY_REVERSE_H_

#include <string>
#include "pricehistory.h"
#include "constants.h"
#include "strategy.h"
#include "expectationmeanstrategy.h"

using namespace std;


class ExpectationMeanStrategyReverse: public ExpectationMeanStrategy {
public:

  ExpectationMeanStrategyReverse() : ExpectationMeanStrategy() {
  }

  ExpectationMeanStrategyReverse(Account* p_account, vector<string> p_tickers_to_trade, double p_small_gain,
                          double p_large_gain, double p_small_loss, double p_large_loss)
                          : ExpectationMeanStrategy(p_account, p_tickers_to_trade, p_small_gain, p_large_gain, p_small_loss, p_large_loss) {
  }

  ~ExpectationMeanStrategyReverse() {
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
    if(account) {
      account->reset();
    }
    state = 0;
    mode = rand() % 2;
    entry_price = 0;
  }

  double current_gain(string ticker, ULL timestamp) {
    double retval = 0;
    double price = account->broker->quote(ticker, timestamp);
    switch(mode) {
    case LONG:
      retval = (price / entry_price) - 1;
      break;
    case SHORT:
      if(price > 0) {
        retval = (entry_price / price) - 1;
      } else {
        retval = (entry_price / Bar::MIN_PRICE) - 1;
      }
      break;
    }
    return retval;
  }

  void enter_position(string ticker, ULL timestamp) {
    int s = 0;
    switch(mode) {
    case LONG:
      s = account->sell_short_amt(ticker, timestamp, amount_per_company);
      //cout << "enter: shorted " << s << " shares of " << ticker << " on " << timestamp << " at $" << account->broker->quote(ticker, timestamp, 0) << "/share." << endl;
      break;
    case SHORT:
      s = account->buy_amap(ticker, timestamp, amount_per_company);
      //cout << "enter: bought " << s << " shares of " << ticker << " on " << timestamp << " at $" << account->broker->quote(ticker, timestamp, 0) << "/share." << endl;
      break;
    }

    entry_timestamp = timestamp;
    entry_price = account->broker->quote(ticker, timestamp);
    entry_shares = s;
  }

  void exit_position(string ticker, ULL timestamp) {
    int s = 0;
    switch(mode) {
    case LONG:
      s = account->buy(ticker, entry_shares, timestamp, true);         // buy [potentially] on margin
      //cout << "exit: re-purchased " << s << " shares of " << ticker << " on " << timestamp << " at $" << account->broker->quote(ticker, timestamp, 0) << "/share." << endl;
      break;
    case SHORT:
      s = account->sell_all(ticker, timestamp);
      //cout << "exit: sold " << s << " shares of " << ticker << " on " << timestamp << " at $" << account->broker->quote(ticker, timestamp, 0) << "/share." << endl;
      break;
    }

    exit_timestamp = timestamp;

    throw ABORT_STRATEGY;
  }
};

#endif /* EXPECTATIONMEANSTRATEGY_REVERSE_H_ */
