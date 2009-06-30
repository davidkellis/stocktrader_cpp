
#ifndef EXPECTATION_MEAN_EXPERIMENT_H_
#define EXPECTATION_MEAN_EXPERIMENT_H_

#include "expectationmeanstrategy.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <cstdlib>      // rand()
#include <ctime>        // difftime
#include "csv.h"
#include "exchange.h"
#include "broker.h"

using namespace std;

static const int CHUNK_SIZE = 256;

string first_line_of_file(string filename);
string last_line_of_file(string filename);

vector<string> create_ticker_visitation_sequence(vector<string> tickers, UL sequence_length);
vector<string> select_tickers_with_enough_price_history(vector<string> ticker_price_histories, Interval minimum_time_span);
vector<string> read_tickers_from_file(string filename);

void run_expectation_mean_experiment(string filename);

#endif /* EXPECTATION_MEAN_EXPERIMENT_H_ */
