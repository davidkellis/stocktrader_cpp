
#include "expectation_mean_experiment.h"

string first_line_of_file(string filename) {
  ifstream is;
  string line;

  is.open(filename.c_str(), ios::in);
  getline(is, line);
  is.close();

  line += "\n";
  
  return line;
}

string last_line_of_file(string filename) {
  ifstream is;
  int temp_length = CHUNK_SIZE;
  int offset = 0;
  int pos = -1;
  char buffer[CHUNK_SIZE];
  string line;

  is.open(filename.c_str(), ios::in);

  // get length of file
  is.seekg(0, ios::end);
  int length = is.tellg();
  
  // if last character is \n, increment the initial offset
  if(length > 0) {
    is.seekg(-1, ios::end);
    if(is.peek() == 10) {           // if last character is a newline character (0x0A on Unix/Mac OS X) (won't work on windows)
      offset++;
    }
  }

  while((pos = line.find_last_of("\n")) < 0 && line.size() < length) {
    offset += CHUNK_SIZE;
    memset(&buffer, 0, CHUNK_SIZE);
    if(offset > length) {
      temp_length = CHUNK_SIZE - (offset - length);
      offset = length;
      is.seekg(-offset, ios::end);         // seek to some offset before the end of the file.
      is.read(buffer, temp_length);
    } else {
      is.seekg(-offset, ios::end);         // seek to some offset before the end of the file.
      is.read(buffer, CHUNK_SIZE);
      temp_length = CHUNK_SIZE;            // I don't think this is needed.
    }
    // line = buffer + line;
    line.insert(0, buffer, temp_length);
  }
  is.close();

  if(pos >= 0) {
    line.erase(0, pos + 1);
  }
  
  line += "\n";

  return line;
}

vector<string> select_tickers_with_enough_price_history(vector<string> ticker_price_histories, Interval minimum_time_span) {
  int date, time;
  ULL timestamp1, timestamp2;
  string first, last, temp;
  stringstream ss;
  Csv csv(ss);
  Interval duration;
  vector<string> valid_tickers;

  for(unsigned int i = 0; i < ticker_price_histories.size(); i++) {
    first = first_line_of_file(ticker_price_histories[i]);
    last = last_line_of_file(ticker_price_histories[i]);

//    cout << "first: " << first << endl;
//    cout << "last: " << last << endl;

    ss.str(first);
    csv.getline(temp);
    date = csv.getint(0);
    time = csv.getint(1);
    timestamp1 = join_timestamp(date, time);

    ss.str(last);
    csv.getline(temp);
    date = csv.getint(0);
    time = csv.getint(1);
    timestamp2 = join_timestamp(date, time);
    
    // compute the duration between the oldest and the most recent price quote timestamps
    duration = Interval::between(timestamp1, timestamp2);

//    cout << timestamp2 << " - " << timestamp1 << " = " << duration.to_s() << endl;
//    cout << duration.to_s() << " >= " << minimum_time_span.to_s() << " => " << (duration >= minimum_time_span) << endl;

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
  
  if(length > 0) {
    for(UL i = 0; i < sequence_length; i++) {
      ticker_counts[tickers[rand() % length]]++;
    }

    // build a sequence of tickers consisting of subsequences of the same ticker symbol
    cout << "Ticker visitation sequence:" << endl;
    for(map<string,int>::iterator it = ticker_counts.begin(); it != ticker_counts.end(); it++) {
      cout << (*it).first << " -> " << (*it).second << " times." << endl;
      for(int i = 0; i < (*it).second; i++) {
        sequence.push_back((*it).first);
      }
    }
  }

  return sequence;
}

vector<string> read_tickers_from_file(string filename) {
  vector<string> tickers;
  ifstream read_tickers;
  string path = "";

  //cout << filename << endl;
  read_tickers.open(filename.c_str());
  if(read_tickers.good()) {
    /*
    cout << read_tickers.fail() << endl;
    cout << read_tickers.bad() << endl;
    cout << read_tickers.eof() << endl;
    cout << read_tickers.good() << endl;
    cout << read_tickers.rdstate() << endl;
    */
    while(!read_tickers.eof()) {
      read_tickers >> path;
      tickers.push_back(path);
    }
    read_tickers.close();
  }

  return tickers;
}

void run_expectation_mean_experiment(string filename) {
  vector<string> tickers = read_tickers_from_file(filename);

  Interval max_trading_period = Interval(1, 0, 0, 0, 0, 0);     // 1 year

  // select the subset of the tickers that have at least max_trading_period worth of price history in their price history file.
  tickers = select_tickers_with_enough_price_history(tickers, max_trading_period);

  double commission_fees = 7.0;
  double initial_deposit = 10000;
  UL trial_count = 10000;

  double small_gain = 0.01;
  double large_gain = 0.1;
  double small_loss = -0.01;
  double large_loss = -0.1;

  Exchange e = Exchange();
  Broker scottrade = Broker(&e, commission_fees, commission_fees);
  Account* a = scottrade.new_account(initial_deposit);
  map<Interval,vector<ExpectationMeanResult> > strategy_results;

  Interval timestamp_increment = Interval(0, 0, 0, 0, 1, 0);      // 1 minutes timestamp increment
  vector<Interval> trading_intervals;
  trading_intervals.push_back(Interval(1, 0, 0, 0, 0, 0));        // 1 year trading interval

  time_t start_time, end_time;
  time(&start_time);
  ExpectationMeanStrategy strategy;
  ExpectationMeanResult result;
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
        cout << "Unloading price data for " << last_symbol << " and loading price data for " << symbol << endl;
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
      
      //cout << trading_start << " to " << trading_end << endl;

      a->reset();
      tickers_to_trade.clear();
      tickers_to_trade.push_back(symbol);
      strategy = ExpectationMeanStrategy(a, tickers_to_trade, small_gain, large_gain, small_loss, large_loss);
      strategy.run(trading_start, trading_end, timestamp_increment);
      t = strategy.exit_timestamp;
      if(t == 0) {
        t = trading_end;
      }

      result.gain = a->value(t) - initial_deposit;
      result.entry_to_exit = Interval::between(strategy.entry_timestamp, strategy.exit_timestamp);
      //cout << "trial result: gain=" << result.gain << " ; entry-to-exit=" << result.entry_to_exit.to_s() << endl;
      strategy_results[trading_interval].push_back(result);
    }
  }
  
  time(&end_time);
  cout << "Finished " << trial_count << " trials in " << difftime(end_time, start_time) << " seconds." << endl;
  
  // iterate over the strategy results, average the gains/losses as well as the entry-to-exit intervals, and then print them
  double total_gain;
  Interval total_duration;
  const Interval* duration;
  const vector<ExpectationMeanResult>* em_results;
  for(map<Interval,vector<ExpectationMeanResult> >::iterator it = strategy_results.begin(); it != strategy_results.end(); it++) {
    total_gain = 0;
    duration = &((*it).first);
    em_results = &((*it).second);
    for(UL i = 0; i < em_results->size(); i++) {
      total_gain += em_results->operator[](i).gain;
      total_duration = total_duration + em_results->operator[](i).entry_to_exit;
    }
    cout << duration->to_s() << ":" << endl;
    cout << "  average gain: " << total_gain / em_results->size() << endl;
    cout << "  average entry-to-exit: " << (total_duration / em_results->size()).to_s() << endl;
  }
  
  return;
}
