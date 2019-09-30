/**
* Implements basic matrix operations on LweSamples
* Operations supported are:
  1. addition
  2. multiplication
  3. dot product (vectors)

*/
#pragma once


#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>
#include <cstddef>

void mat_add(LweSample*** sum, LweSample*** a, LweSample*** b, const int rows, const int cols, const TFheGateBootstrappingCloudKeySet* ck, const size_t size);
void elem_mult(LweSample*** sum, LweSample*** a, LweSample** b, const int rows, const int cols, const TFheGateBootstrappingCloudKeySet* ck, const size_t size);
void elem_mult(LweSample*** prod, LweSample** a, LweSample*** b, const int rows, const int cols, const TFheGateBootstrappingCloudKeySet* ck, const size_t size);
void elem_mult(LweSample*** prod, LweSample** a, LweSample** b, const int cols, const TFheGateBootstrappingCloudKeySet* ck, const size_t size);
void mat_mult(LweSample*** prod, LweSample*** a, LweSample*** b, const int rows, const int cols, const TFheGateBootstrappingCloudKeySet* ck, const size_t size);
void dot(LweSample* prod, LweSample** a, LweSample** b, const int cols, const TFheGateBootstrappingCloudKeySet* ck, const size_t size);
void elem_shift(LweSample** prod, LweSample** a, int* b, const int cols, const TFheGateBootstrappingCloudKeySet* ck, const size_t size);
void shiftDot(LweSample* result, LweSample** a, int* b, const int cols, const TFheGateBootstrappingCloudKeySet* ck, const size_t size);
void transpose(LweSample*** transpose, const LweSample*** source, const TFheGateBootstrappingCloudKeySet* ck, size_t size);
