
#include "expectation_mean_experiment.h"

string first_line_of_file(string filename) {
  ifstream is;
  string line;

  is.open(filename.c_str(), ios::in);
  getline(is, line);
  is.close();

  return line;
}

string last_line_of_file(string filename) {
  ifstream is;
  int offset = 0;
  int pos = -1;
  char buffer[CHUNK_SIZE];
  string line;

  is.open(filename.c_str(), ios::in);

  // get length of file
  is.seekg(0, ios::end);
  int length = is.tellg();

  while((pos = line.find_last_of("\n")) < 0 && line.size() < length) {
    offset += CHUNK_SIZE;
    if(offset > length) {
      int remainder = CHUNK_SIZE - (offset - length);
      offset = length;
      is.seekg(-offset, ios::end);         // seek to some offset before the end of the file.
      is.read(buffer, remainder);
    } else {
      is.seekg(-offset, ios::end);         // seek to some offset before the end of the file.
      is.read(buffer, CHUNK_SIZE);
    }
    line = buffer + line;
  }
  is.close();

  if(pos >= 0) {
    line.erase(0, pos + 1);
  }

  return line;
}

vector<string> select_tickers_with_enough_price_history(vector<string> ticker_price_histories, Interval minimum_time_span) {
  int date, time;
  ULL timestamp1, timestamp2;
  string first, last;
  Csv csv;
  Interval duration;
  vector<string> valid_tickers;

  for(unsigned int i = 0; i < ticker_price_histories.size(); i++) {
    first = first_line_of_file(ticker_price_histories[i]);
    last = last_line_of_file(ticker_price_histories[i]);

    csv.getline(first);
    date = csv.getint(0);
    time = csv.getint(1);
    timestamp1 = join_timestamp(date, time);

    csv.getline(last);
    date = csv.getint(0);
    time = csv.getint(1);
    timestamp2 = join_timestamp(date, time);

    // compute the duration between the oldest and the most recent price quote timestamps
    duration = Interval::between(timestamp1, timestamp2);

    if(duration >= minimum_time_span) {
      valid_tickers.push_back(ticker_price_histories[i]);
    }
  }

  return valid_tickers;
}

vector<string> create_ticker_visitation_sequence(vector<string> tickers, UL sequence_length) {
  map<string,int> ticker_counts;
  vector<string> sequence;
  UL length = tickers.size();

  for(UL i = 0; i < sequence_length; i++) {
    ticker_counts[tickers[rand() % length]]++;
  }

  // build a sequence of tickers consisting of subsequences of the same ticker symbol
  for(map<string,int>::iterator it = ticker_counts.begin(); it != ticker_counts.end(); it++) {
    for(int i = 0; i < (*it).second; i++) {
      sequence.push_back((*it).first);
    }
  }

  return sequence;
}

void run_expectation_mean_experiment() {
  vector<string> tickers;

  Interval max_trading_period = Interval(1, 0, 0, 0, 0, 0);     // 1 year

  // select the subset of the tickers that have at least max_trading_period worth of price history in their price history file.
  tickers = select_tickers_with_enough_price_history(tickers, max_trading_period);

  double commission_fees = 7.0;
  double initial_deposit = 10000;
  UL trial_count = 1;

  double small_gain = 0.05;
  double large_gain = 0.1;
  double small_loss = -0.05;
  double large_loss = -0.1;

  Exchange e = Exchange();
  Broker scottrade = Broker(&e, commission_fees, commission_fees);
  Account* a = scottrade.new_account(initial_deposit);
  map<Interval,vector<double> > strategy_results;

  Interval timestamp_increment = Interval(0, 0, 0, 0, 1, 0);      // 1 minutes timestamp increment
  vector<Interval> trading_intervals;
  trading_intervals.push_back(Interval(1, 0, 0, 0, 0, 0));        // 1 year trading interval

  time_t start_time, end_time;
  time(&start_time);
  ExpectationMeanStrategy strategy;
  vector<string> tickers_to_trade;
  ULL t, trading_start, trading_end;
  Interval trading_interval;
  string symbol;
  string last_symbol;
  vector<string> ticker_visitation_sequence;
  for(unsigned int i = 0; i < trading_intervals.size(); i++) {
    trading_interval = trading_intervals[i];
    ticker_visitation_sequence = create_ticker_visitation_sequence(tickers, trial_count);

    for(UL j = 0; j < ticker_visitation_sequence.size(); j++) {
      symbol = ticker_visitation_sequence[j];

      if(symbol != last_symbol) {
        e.remove_price_history(last_symbol);
        e.add_price_history(symbol);
        last_symbol = symbol;
      }

      trading_start = e.price_histories[symbol].rand().timestamp;
      trading_end = trading_start + trading_interval;
      if(trading_end > e.price_histories[symbol].last().timestamp) {
        trading_end = e.price_histories[symbol].last().timestamp;
        trading_start = trading_end - trading_interval;
      }

      a->reset();
      tickers_to_trade.clear();
      tickers_to_trade.push_back(symbol);
      strategy = ExpectationMeanStrategy(a, tickers_to_trade, small_gain, large_gain, small_loss, large_loss);
      strategy.run(trading_start, trading_end, timestamp_increment);
      t = strategy.exit_timestamp;
      if(t == 0) {
        t = trading_end;
      }
      strategy_results[trading_interval].push_back(a->value(t) - initial_deposit);
    }
  }
  
  time(&end_time);
  cout << "Finished trials in " << difftime(end_time, start_time) << " seconds.";
  
  // iterate over the strategy results, average them, and then print them
  double total;
  const Interval* duration;
  const vector<double>* v_d;
  for(map<Interval,vector<double> >::iterator it = strategy_results.begin(); it != strategy_results.end(); it++) {
    total = 0;
    duration = &((*it).first);
    v_d = &((*it).second);
    for(UL i = 0; i < v_d->size(); i++) {
      total += v_d->operator[](i);
    }
    cout << duration->to_s() << ": " << total / v_d->size() << endl;
  }
}
