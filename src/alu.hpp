/**
    * Implements addition, subtraction, and multiplication operations, as well as logical operations

*/

#pragma once


#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>
#include <cstddef>
#include "omp_constants.hpp"
//__cplusplus=false;
//void bootsCOPYPointer(LweSample *result, LweSample *ca);

void add(LweSample* sum, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* ck, const size_t size);
void leftRotate(LweSample* result, const LweSample* a, const TFheGateBootstrappingCloudKeySet* ck, const size_t size, int amnt);
void leftShift(LweSample* result, const LweSample* a, const TFheGateBootstrappingCloudKeySet* ck, const size_t size, int amnt);
void rightRotate(LweSample* result, const LweSample* a, const TFheGateBootstrappingCloudKeySet* ck, const size_t size, int amnt);
void rightShift(LweSample* result, const LweSample* a, const TFheGateBootstrappingCloudKeySet* ck, const size_t size, int amnt);
void sub(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* ck, const size_t size);
void mult(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* ck, const size_t size);
void power(LweSample* result, const LweSample* a, int n, const TFheGateBootstrappingCloudKeySet* ck, const size_t size);
void twosComplement(LweSample* result, const LweSample* a, const TFheGateBootstrappingCloudKeySet* ck, const size_t size);
//void copyPointer(LweSample* dest,  LweSample* source, const TFheGateBootstrappingCloudKeySet* ck, const size_t size);
void full_adder(LweSample *sum, const LweSample *x, const LweSample *y, const int32_t nb_bits,
                const TFheGateBootstrappingCloudKeySet *keyset);


void copy(LweSample* dest, const LweSample* source, const TFheGateBootstrappingCloudKeySet* ck, const size_t size);
void zero(LweSample* result, const TFheGateBootstrappingCloudKeySet* ck, const size_t size);
void NOT(LweSample* result, const LweSample* a, const TFheGateBootstrappingCloudKeySet* ck, const size_t size);
void OR(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* ck, const size_t size);
void AND(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* ck, const size_t size);
void NAND(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* ck, const size_t size);
void NOR(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* ck, const size_t size);
void XOR(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* ck, const size_t size);
void XNOR(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* ck, const size_t size);
void ANDNY(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* ck, const size_t size);
void ANDYN(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* ck, const size_t size);
void ORNY(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* ck, const size_t size);
void ORYN(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* ck, const size_t size);
void MUX(LweSample* result, const LweSample* a, const LweSample* b, const LweSample* c, const TFheGateBootstrappingCloudKeySet* ck, const size_t size);
void CONSTANT(LweSample* result, const int& a, const TFheGateBootstrappingCloudKeySet* ck, const size_t size);


void reduce_add(LweSample* result, LweSample** arrays, int num_arrays, const TFheGateBootstrappingCloudKeySet* ck, const size_t size);
void reduce_add_4(LweSample* result, LweSample** arrays, int num_arrays, const TFheGateBootstrappingCloudKeySet* ck, const size_t size);
void reduce_add_8(LweSample* result, LweSample** arrays, int num_arrays, const TFheGateBootstrappingCloudKeySet* ck, const size_t size);

void seq_add(LweSample* result, LweSample** arrays, int num_arrays, const TFheGateBootstrappingCloudKeySet* ck, const size_t size);

void bootsCOPYPointer(LweSample *result,  LweSample *ca);
void copyPointer(LweSample** dest,  LweSample** source, const size_t size);
void leftShiftP(LweSample* result,  LweSample* a, const TFheGateBootstrappingCloudKeySet* ck, const size_t size, int amnt);
void rightShiftP(LweSample* result,  LweSample* a, const TFheGateBootstrappingCloudKeySet* ck, const size_t size, int amnt);
