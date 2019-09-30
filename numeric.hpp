/**
  * Code that handles precision conversions, etc
  * Core functions:
    1. Convert float to fixed point, given precision
    2. Convert fixed point to float, given precision
*/

#include <cmath>
#include <vector>
#include <iostream>


using namespace std;


int get_min(int maxbits) {
  return -pow(2, maxbits-1);
}

int get_max(int maxbits) {
  // assumes signed
  return pow(2, maxbits-1) - 1;
}

template<typename T>
T float_to_fixed(double flt, int maxbits, int factor, bool clip=true) {
  int max = get_max(maxbits),
      min = get_min(maxbits);
  T fixed = flt * factor;
  if(fixed > max)
    fixed = max;
  else if(fixed < min)
    fixed = min;
  return fixed;
}

template<typename T>
vector<T> float_to_fixed(vector<double> flt, int maxbits, int factor, bool clip=true) {
  vector<T> fixed;
  for(double f: flt){
    fixed.push_back(float_to_fixed<T>(f, maxbits, factor, clip));
  }
  return fixed;
}


double fixed_to_float(int fixed, int factor) {
  return ((float) fixed) / factor;
}

template<typename T>
vector<double> fixed_to_float(vector<T> fixed, int factor) {
  vector<double> flt;
  for(int f: fixed) {
    flt.push_back(fixed_to_float(f, factor));
  }
  return flt;
}
