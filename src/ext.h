
#ifndef EXT_H_
#define EXT_H_

#include <map>

using namespace std;

template<class K, class V>
bool map_key_exists(map<K,V>& m, K& k) {
  typename map<K,V>::iterator it;
  it = m.find(k);
  if(it == m.end()) {         // key not found in the map
    return false;
  } else {                    // key IS found in the map
    return true;
  }
}

template<class K, class V>
V map_get(map<K,V>& m, K& key, V def) {
  typename map<K,V>::iterator it;
  it = m.find(key);
  if(it == m.end()) {         // key not found in the map
    return def;
  } else {                    // key IS found in the map
    return ((*it).second);
  }
}

#endif /* EXT_H_ */
