
#include "strategy.h"

const int Strategy::TRADING_DAYS[] = {1, 5};                                      // Monday-Friday
const Interval Strategy::TRADING_HOURS[] = {Interval(83000), Interval(150000)};   // 8:30 AM CST to 4:00 PM (15:00) CST
