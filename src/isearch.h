/*
 * isearch.h - Interpolation Search
 *
 *  Created on: Jun 10, 2009
 *      Author: davidkellis
 */

#ifndef ISEARCH_H_
#define ISEARCH_H_

#include <iostream>
#include <vector>
#include "constants.h"

using namespace std;

template<class T>
ULL identity_extractor(T& obj) {
  return (ULL) obj;
}

template<class T>
long interpolation_search(vector<T>& l, ULL search_item, int direction = NOT_FOUND, ULL (*extractor)(T&) = &identity_extractor<T>) {
  if(l.size() == 0)
    return NOT_FOUND;
  
  long retval = NOT_FOUND;
  
  long i = 0;
  long j = l.size() - 1;
  long m = 0;
  
  ULL low = extractor(l[i]);
  ULL high = extractor(l[j]);
  ULL mid = 0;
  
  /*
   * This while loop could be written as:
   *   while((c - b)*(b - a) > 0) { ... } instead of while(a < b && b < c) { ... }
   * because:
   *   (1) a < b === 0 < b - a === (b - a) > 0
   *   (2) b < c === 0 < c - b === (c - b) > 0
   * and if (1) and (2) are both true, then (b - a)*(c - b) > 0.
   *
   * I'm not sure if either way is more advantageous than the other way.
   */
  while(low < search_item && search_item < high) {
    m = i + (search_item - low) * (j - i) / (high - low);
    mid = extractor(l[m]);
    
    if(search_item > mid) {
      i = m + 1;
      low = extractor(l[i]);
    } else if(search_item < mid) {
      j = m - 1;
      high = extractor(l[j]);
    } else {
      return m;
    }
  }
  
  // first 2 ifs: check for search_item equal to low or high
  // rest: does search_item lie exclusively outside of the range between low and high.
  if(low == search_item) {
    retval = i;
  } else if(high == search_item) {
    retval = j;
  } else if(search_item < low) {
    if(direction == NOT_FOUND) {
      retval = NOT_FOUND;
    } else if(direction == DIR_LEFT) {
      if(i > 0) {
        retval = i - 1;
      } else {
        retval = NOT_FOUND;
      }
    } else if(direction == DIR_RIGHT) {
      retval = i;
    }
  } else if(search_item > high) {
    if(direction == NOT_FOUND) {
      retval = NOT_FOUND;
    } else if(direction == DIR_LEFT) {
      retval = j;
    } else if(direction == DIR_RIGHT) {
      if(j < l.size() - 1) {
        retval = j + 1;
      } else {
        retval = NOT_FOUND;
      }
    }
  }
  
  return retval;
}



/*
 // Returns the index at which search_item is found in list, l, (which must be a sorted vector). If search_item isn't found, -1 is returned.
 int interpolation_search(vector<int>& l, int search_item, int direction = NOT_FOUND) {
 if(l.size() == 0)
 return NOT_FOUND;
 
 int retval = NOT_FOUND;
 
 int i = 0;
 int j = l.size() - 1;
 int m = 0;
 
 int low = l[i];
 int high = l[j];
 int mid = 0;
 
 while(low < search_item && search_item < high) {
 m = i + (search_item - low) * (j - i) / (high - low);
 mid = l[m];
 
 if(search_item > mid) {
 i = m + 1;
 low = l[i];
 } else if(search_item < mid) {
 j = m - 1;
 high = l[j];
 } else {
 return m;
 }
 }
 
 // first 2 ifs: check for search_item equal to low or high
 // rest: does search_item lie exclusively outside of the range between low and high.
 if(low == search_item) {
 retval = i;
 } else if(high == search_item) {
 retval = j;
 } else if(search_item < low) {
 if(direction == NOT_FOUND) {
 retval = NOT_FOUND;
 } else if(direction == DIR_LEFT) {
 if(i > 0) {
 retval = i - 1;
 } else {
 retval = NOT_FOUND;
 }
 } else if(direction == DIR_RIGHT) {
 retval = i;
 }
 } else if(search_item > high) {
 if(direction == NOT_FOUND) {
 retval = NOT_FOUND;
 } else if(direction == DIR_LEFT) {
 retval = j;
 } else if(direction == DIR_RIGHT) {
 if(j < l.size() - 1) {
 retval = j + 1;
 } else {
 retval = NOT_FOUND;
 }
 }
 }
 
 return retval;
 }
 */

#endif /* ISEARCH_H_ */
