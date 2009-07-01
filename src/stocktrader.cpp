//============================================================================
// Author      : David Ellis
// Copyright   : Copyright 2009 David Kerbow Ellis
//============================================================================

#include <iostream>
#include <ctime>          // time()
#include <cstdlib>        // srand() and rand()
#include "expectation_mean_experiment.h"
#include "csv.h"
#include "pricehistory.h"
#include "broker.h"
#include "isearch.h"
#include "time.h"

using namespace std;

// this function tests out interpolation search.
void test_interpolation_search() {
  cout << "Test 1: 30 elements, ranging between 1-80000000" << endl;
  
  vector<int> l;
  l.push_back(1);
  l.push_back(2);
  l.push_back(3);
  l.push_back(4);
  l.push_back(5);
  l.push_back(6);
  l.push_back(7);
  l.push_back(8);
  l.push_back(9);
  l.push_back(11);
  l.push_back(22);
  l.push_back(33);
  l.push_back(43);
  l.push_back(44);
  l.push_back(45);
  l.push_back(45);
  l.push_back(46);
  l.push_back(46);
  l.push_back(46);
  l.push_back(46);
  l.push_back(47);
  l.push_back(990);
  l.push_back(1001);
  l.push_back(2002);
  l.push_back(3003);
  l.push_back(40000);
  l.push_back(50000);
  l.push_back(600000);
  l.push_back(7000000);
  l.push_back(80000000);
  
  cout << "interpolation_search(l, 0) => " << interpolation_search<int>(l, 0, NOT_FOUND) << endl;
  cout << "interpolation_search(l, 0, DIR_LEFT) => " << interpolation_search(l, (ULL)0, DIR_LEFT, identity_extractor<int>) << endl;
  cout << "interpolation_search(l, 0, DIR_RIGHT) => " << interpolation_search(l, 0, DIR_RIGHT, &identity_extractor<int>) << endl;
  
  cout << "interpolation_search(l, 1) => " << interpolation_search<int>(l, 1) << endl;
  cout << "interpolation_search(l, 1, DIR_LEFT) => " << interpolation_search(l, 1, DIR_LEFT) << endl;
  cout << "interpolation_search(l, 1, DIR_RIGHT) => " << interpolation_search(l, 1, DIR_RIGHT) << endl;
  
  cout << "interpolation_search(l, 2) => " << interpolation_search(l, 2) << endl;
  cout << "interpolation_search(l, 2, DIR_LEFT) => " << interpolation_search(l, 2, DIR_LEFT) << endl;
  cout << "interpolation_search(l, 2, DIR_RIGHT) => " << interpolation_search(l, 2, DIR_RIGHT) << endl;
  
  cout << "interpolation_search(l, 10) => " << interpolation_search(l, 10) << endl;
  cout << "interpolation_search(l, 10, DIR_LEFT) => " << interpolation_search(l, 10, DIR_LEFT) << endl;
  cout << "interpolation_search(l, 10, DIR_RIGHT) => " << interpolation_search(l, 10, DIR_RIGHT) << endl;
  
  cout << "interpolation_search(l, 45) => " << interpolation_search(l, 45) << endl;
  cout << "interpolation_search(l, 45, DIR_LEFT) => " << interpolation_search(l, 45, DIR_LEFT) << endl;
  cout << "interpolation_search(l, 45, DIR_RIGHT) => " << interpolation_search(l, 45, DIR_RIGHT) << endl;
  
  cout << "interpolation_search(l, 46) => " << interpolation_search(l, 46) << endl;
  cout << "interpolation_search(l, 46, DIR_LEFT) => " << interpolation_search(l, 46, DIR_LEFT) << endl;
  cout << "interpolation_search(l, 46, DIR_RIGHT) => " << interpolation_search(l, 46, DIR_RIGHT) << endl;
  
  cout << "interpolation_search(l, 48) => " << interpolation_search(l, 48) << endl;
  cout << "interpolation_search(l, 48, DIR_LEFT) => " << interpolation_search(l, 48, DIR_LEFT) << endl;
  cout << "interpolation_search(l, 48, DIR_RIGHT) => " << interpolation_search(l, 48, DIR_RIGHT) << endl;
  
  cout << "interpolation_search(l, 100) => " << interpolation_search(l, 100) << endl;
  cout << "interpolation_search(l, 100, DIR_LEFT) => " << interpolation_search(l, 100, DIR_LEFT) << endl;
  cout << "interpolation_search(l, 100, DIR_RIGHT) => " << interpolation_search(l, 100, DIR_RIGHT) << endl;
  
  cout << "interpolation_search(l, 80000000) => " << interpolation_search(l, 80000000) << endl;
  cout << "interpolation_search(l, 80000000, DIR_LEFT) => " << interpolation_search(l, 80000000, DIR_LEFT) << endl;
  cout << "interpolation_search(l, 80000000, DIR_RIGHT) => " << interpolation_search(l, 80000000, DIR_RIGHT) << endl;
  
  cout << "interpolation_search(l, 80000001) => " << interpolation_search(l, 80000001) << endl;
  cout << "interpolation_search(l, 80000001, DIR_LEFT) => " << interpolation_search(l, 80000001, DIR_LEFT) << endl;
  cout << "interpolation_search(l, 80000001, DIR_RIGHT) => " << interpolation_search(l, 80000001, DIR_RIGHT) << endl;
  
  cout << endl << "Test 2: 10 elements, ranging between 1-100000" << endl;
  
  vector<long> l2 = vector<long>();
  l2.push_back(1);
  l2.push_back(1);
  l2.push_back(1);
  l2.push_back(1);
  l2.push_back(1);
  l2.push_back(1);
  l2.push_back(1);
  l2.push_back(1);
  l2.push_back(10000);
  l2.push_back(100000);
  
  cout << "interpolation_search(l2, 0) => " << interpolation_search(l2, 0) << endl;
  cout << "interpolation_search(l2, 0, DIR_LEFT) => " << interpolation_search(l2, 0.0, DIR_LEFT) << endl;
  cout << "interpolation_search(l2, 0, DIR_RIGHT) => " << interpolation_search(l2, 0, DIR_RIGHT) << endl;
  
  cout << "interpolation_search(l2, 1) => " << interpolation_search(l2, 1) << endl;
  cout << "interpolation_search(l2, 1, DIR_LEFT) => " << interpolation_search(l2, 1, DIR_LEFT) << endl;
  cout << "interpolation_search(l2, 1, DIR_RIGHT) => " << interpolation_search(l2, 1, DIR_RIGHT) << endl;
  
  cout << "interpolation_search(l2, 2) => " << interpolation_search(l2, 2) << endl;
  cout << "interpolation_search(l2, 2, DIR_LEFT) => " << interpolation_search(l2, 2, DIR_LEFT) << endl;
  cout << "interpolation_search(l2, 2, DIR_RIGHT) => " << interpolation_search(l2, 2, DIR_RIGHT) << endl;
  
  cout << "interpolation_search(l2, 10000) => " << interpolation_search(l2, 10000) << endl;
  cout << "interpolation_search(l2, 10000, DIR_LEFT) => " << interpolation_search(l2, 10000, DIR_LEFT) << endl;
  cout << "interpolation_search(l2, 10000, DIR_RIGHT) => " << interpolation_search(l2, 10000, DIR_RIGHT) << endl;
  
  cout << "interpolation_search(l2, 100000) => " << interpolation_search(l2, 100000) << endl;
  cout << "interpolation_search(l2, 100000, DIR_LEFT) => " << interpolation_search(l2, 100000, DIR_LEFT) << endl;
  cout << "interpolation_search(l2, 100000, DIR_RIGHT) => " << interpolation_search(l2, 100000, DIR_RIGHT) << endl;
  
  cout << "interpolation_search(l2, 100001) => " << interpolation_search(l2, 100001) << endl;
  cout << "interpolation_search(l2, 100001, DIR_LEFT) => " << interpolation_search(l2, 100001, DIR_LEFT) << endl;
  cout << "interpolation_search(l2, 100001, DIR_RIGHT) => " << interpolation_search(l2, 100001, DIR_RIGHT) << endl;
}

int main(int argc, char* argv[]) {
  // seed the random number generator
  srand(time(0));

  //test_price_history();
  //test_csv();
  //test_interpolation_search();
  //test_split_timestamp();
  run_expectation_mean_experiment(argv[1]);
  
  return 0;
}
