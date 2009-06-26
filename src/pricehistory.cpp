
#include "pricehistory.h"

const double Bar::MIN_PRICE = 0.000001;

ULL bar_extractor(Bar& obj) {
  return (ULL) obj.timestamp;
}

int test_price_history() {
  PriceHistory h = PriceHistory("/Users/davidkellis/Projects/c++/eclipse_workspace/stocktrader/intraday/A.csv");
  Bar *b;

  cout << h.bars.size() << " bars." << endl;

  b = &h.bars[0];
  cout << 0 << ": " << b->to_s() << endl;

  for(int i = 99; i < h.bars.size(); i+=100) {
    b = &h.bars[i];
    cout << i << ": " << b->to_s() << endl;
  }

  return 0;
}
