/*
 * csv.cpp
 *
 * Taken from: http://cm.bell-labs.com/cm/cs/tpop/csvgetlinec++.c
 */

#ifndef CSV_H_
#define CSV_H_

/* Copyright (C) 1999 Lucent Technologies */
/* Excerpted from 'The Practice of Programming' */
/* by Brian W. Kernighan and Rob Pike */

#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <cstdlib>

using namespace std;

// read and parse comma-separated values
// sample input: "LU",86.25,"11/4/1998","2:19PM",+4.0625
class Csv {
public:
  Csv(istream& fin = cin, string sep = ",") : fin(fin), fieldsep(sep) {
  }

  int getline(string&);

  string getfield(int n);

  int getint(int n);

  double getdouble(int n);

  int getnfield() const {
    return nfield;
  }

private:
  istream& fin;           // input file pointer
  string line;            // input line
  vector<string> field;   // field strings
  int nfield;             // number of fields
  string fieldsep;        // separator characters

  int split();
  int endofline(char);
  int advplain(const string& line, string& fld, int);
  int advquoted(const string& line, string& fld, int);
};

// Csvtest main: test Csv class
extern int test_csv();

#endif /* CSV_H_ */
