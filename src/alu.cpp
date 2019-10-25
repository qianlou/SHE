#include "alu.hpp"
/*
Implements bitwise full-adder circuit on two n-bit integers
Parallel implementation gives ~0.65x speedup, which close to theoretical circuit speedup of 0.6
Reference: https://en.wikibooks.org/wiki/Microprocessor_Design/Add_and_Subtract_Blocks
Pseudocode

module full_adder(a, b, cin, cout, s);
   input a, b, cin;
   output cout, s;
   wire temp;
   temp = a ^ b;
   s = temp ^ cin;
   cout = (cin & temp) | (a & b);
endmodule
*/

/*
 * Homomorphic bootstrapped OR gate
 * Takes in input 2 LWE samples (with message space [-1/8,1/8], noise<1/16)
 * Outputs a LWE bootstrapped sample (with message space [-1/8,1/8], noise<1/16)
*/
EXPORT void
bootsOR_r(LweSample *result, const LweSample *ca, const LweSample *cb, const TFheGateBootstrappingCloudKeySet *bk) {
    static const Torus32 MU = modSwitchToTorus32(1, 8);
    const LweParams *in_out_params = bk->params->in_out_params;

    LweSample *temp_result = new_LweSample(in_out_params);

    //compute: (0,1/8) + ca + cb
    static const Torus32 OrConst = modSwitchToTorus32(1, 8);
    lweNoiselessTrivial(temp_result, OrConst, in_out_params);
    lweAddTo(temp_result, ca, in_out_params);
    lweAddTo(temp_result, cb, in_out_params);

    //if the phase is positive, the result is 1/8
    //if the phase is positive, else the result is -1/8
    delete_LweSample(temp_result);
}


/*
 * Homomorphic bootstrapped AND gate
 * Takes in input 2 LWE samples (with message space [-1/8,1/8], noise<1/16)
 * Outputs a LWE bootstrapped sample (with message space [-1/8,1/8], noise<1/16)
*/
EXPORT void
bootsAND_r(LweSample *result, const LweSample *ca, const LweSample *cb, const TFheGateBootstrappingCloudKeySet *bk) {
    static const Torus32 MU = modSwitchToTorus32(1, 8);
    const LweParams *in_out_params = bk->params->in_out_params;

    LweSample *temp_result = new_LweSample(in_out_params);

    //compute: (0,-1/8) + ca + cb
    static const Torus32 AndConst = modSwitchToTorus32(-1, 8);
    lweNoiselessTrivial(temp_result, AndConst, in_out_params);
    lweAddTo(temp_result, ca, in_out_params);
    lweAddTo(temp_result, cb, in_out_params);

    //if the phase is positive, the result is 1/8
    //if the phase is positive, else the result is -1/8
    //tfhe_bootstrap_FFT(result, bk->bkFFT, MU, temp_result);

    delete_LweSample(temp_result);
}


/*
 * Homomorphic bootstrapped XOR gate
 * Takes in input 2 LWE samples (with message space [-1/8,1/8], noise<1/16)
 * Outputs a LWE bootstrapped sample (with message space [-1/8,1/8], noise<1/16)
*/
EXPORT void
bootsXOR_r(LweSample *result, const LweSample *ca, const LweSample *cb, const TFheGateBootstrappingCloudKeySet *bk) {
    static const Torus32 MU = modSwitchToTorus32(1, 8);
    const LweParams *in_out_params = bk->params->in_out_params;

    LweSample *temp_result = new_LweSample(in_out_params);

    //compute: (0,1/4) + 2*(ca + cb)
    static const Torus32 XorConst = modSwitchToTorus32(1, 4);
    lweNoiselessTrivial(temp_result, XorConst, in_out_params);
    lweAddMulTo(temp_result, 2, ca, in_out_params);
    lweAddMulTo(temp_result, 2, cb, in_out_params);

    //if the phase is positive, else the result is -1/8
    //tfhe_bootstrap_FFT(result, bk->bkFFT, MU, temp_result);

    delete_LweSample(temp_result);
}

void full_adder(LweSample *sum, const LweSample *x, const LweSample *y, const int32_t nb_bits,
                const TFheGateBootstrappingSecretKeySet *keyset) {
    const LweParams *in_out_params = keyset->params->in_out_params;
    // carries
    LweSample *carry = new_LweSample_array(2, in_out_params);
    bootsSymEncrypt(carry, 0, keyset); // first carry initialized to 0
    // temps
    LweSample *temp = new_LweSample_array(3, in_out_params);

    for (int32_t i = 0; i < nb_bits; ++i) {
        //sumi = xi XOR yi XOR carry(i-1) 
        bootsXOR(temp, x + i, y + i, &keyset->cloud); // temp = xi XOR yi
        bootsXOR(sum + i, temp, carry, &keyset->cloud);

        // carry = (xi AND yi) XOR (carry(i-1) AND (xi XOR yi))
        bootsAND(temp + 1, x + i, y + i, &keyset->cloud); // temp1 = xi AND yi
        bootsAND(temp + 2, carry, temp, &keyset->cloud); // temp2 = carry AND temp
        bootsXOR(carry + 1, temp + 1, temp + 2, &keyset->cloud);
        bootsCOPY(carry, carry + 1, &keyset->cloud);
    }
    bootsCOPY(sum + nb_bits, carry, &keyset->cloud);

    delete_LweSample_array(3, temp);
    delete_LweSample_array(2, carry);
}

void add(LweSample* sum, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  LweSample *carry = new_gate_bootstrapping_ciphertext(ck->params),
            *tmp_s = new_gate_bootstrapping_ciphertext(ck->params),
            *tmp_c = new_gate_bootstrapping_ciphertext(ck->params);



  // first iteration
//#pragma omp parallel sections num_threads(4)



  bootsXOR(&sum[0], &a[0], &b[0], ck);
  bootsAND(carry, &a[0], &b[0], ck);


  for(int i = 1; i < size; i++) {

    #pragma omp parallel sections num_threads(2)
    {
      #pragma omp section
      bootsXOR(tmp_s, &a[i], &b[i], ck);
      #pragma omp section
      bootsAND(tmp_c, &a[i], &b[i], ck);
    }
    #pragma omp parallel sections num_threads(2)
    {
      #pragma omp section
      bootsXOR(&sum[i], tmp_s, carry, ck);
      #pragma omp section
      bootsAND(carry, carry, tmp_s, ck);
    }
    bootsOR(carry, carry, tmp_c, ck);
  }

  // clean up
  delete_gate_bootstrapping_ciphertext(carry);
  delete_gate_bootstrapping_ciphertext(tmp_s);
  delete_gate_bootstrapping_ciphertext(tmp_c);

}


/**
  Sequential array sum implementation. Included for completeness and testing
*/
void seq_add(LweSample* result, LweSample** arrays, int num_arrays, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  zero(result, ck, size);
  for(int i = 0; i < num_arrays; i++) {
    add(result, result, arrays[i], ck, size);
  }
}


/**
Parallel reduce sum implementation. Gives ~1.6x speedup over sequential
TODO: use std::vector

*/
void reduce_add(LweSample* result, LweSample** arrays, int num_arrays, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {

  if(num_arrays == 1) {
    copy(result, arrays[0], ck, size);
    return;
  }
  if(num_arrays == 2) {
  //bootsXOR(&result[0], &arrays[0][0], &arrays[1][0], ck);
  //bootsAND(&result[0],  &arrays[0][0], &arrays[1][0], ck);

  //bootsXOR(&result[0], &arrays[0][1], &arrays[1][1], ck);
  //bootsAND(&result[0],  &arrays[0][1], &arrays[1][1], ck);

   add(result, arrays[0], arrays[1], ck, size);
    return;
  }
  int mid_point = num_arrays / 2;
  LweSample *result1 = new_gate_bootstrapping_ciphertext_array(size, ck->params);
  #pragma omp parallel sections num_threads(4)
  {
    #pragma omp section
    reduce_add(result, arrays, mid_point, ck, size);
    #pragma omp section
    reduce_add(result1, &arrays[mid_point], num_arrays-mid_point, ck, size);
  }
  add(result, result, result1, ck, size);
  //bootsXOR(&result[0], &arrays[0][0], &arrays[1][0], ck);
  //bootsAND(&result[0],  &arrays[0][0], &arrays[1][0], ck);
  delete_gate_bootstrapping_ciphertext_array(size, result1);
  //add(result, arrays[0], arrays[1], ck, size);
}




void reduce_add_4(LweSample* result, LweSample** arrays, int num_arrays, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {

  if(num_arrays == 1) {
    copy(result, arrays[0], ck, size);
    return;
  }
  if(num_arrays == 2) {
    add(result, arrays[0], arrays[1], ck, size);
    return;
  }
    if(num_arrays == 3) {
    LweSample *temp = new_gate_bootstrapping_ciphertext_array(size, ck->params);
    add(temp, arrays[0], arrays[1], ck, size);
    add(result, temp, arrays[2], ck, size);
    delete_gate_bootstrapping_ciphertext_array(size, temp);
    return;
  }
    if(num_arrays == 4) {
    LweSample *temp = new_gate_bootstrapping_ciphertext_array(size, ck->params);
    add(result, arrays[0], arrays[1], ck, size);
    add(temp, arrays[2], arrays[3], ck, size);
    add(result,result , temp, ck, size);
    delete_gate_bootstrapping_ciphertext_array(size, temp);
    return;
  }
  
  int fo_point = num_arrays / 4;
  LweSample *result1 = new_gate_bootstrapping_ciphertext_array(size, ck->params);
  LweSample *result2 = new_gate_bootstrapping_ciphertext_array(size, ck->params);
  LweSample *result3 = new_gate_bootstrapping_ciphertext_array(size, ck->params);

  #pragma omp parallel sections num_threads(4)
  {
    #pragma omp section
    reduce_add_4(result, arrays, fo_point, ck, size);
    #pragma omp section
    reduce_add_4(result1, &arrays[fo_point], fo_point, ck, size);
    #pragma omp section
    reduce_add_4(result2, &arrays[2*fo_point], fo_point, ck, size);
    #pragma omp section
    reduce_add_4(result3, &arrays[3*fo_point], num_arrays-3*fo_point, ck, size);
  }

  add(result, result, result1, ck, size);
  add(result, result, result2, ck, size);
  add(result, result, result3, ck, size);

  delete_gate_bootstrapping_ciphertext_array(size, result1);
  delete_gate_bootstrapping_ciphertext_array(size, result2);
  delete_gate_bootstrapping_ciphertext_array(size, result3);
}

/**
Parallel reduce sum implementation. Gives ~1.6x speedup over sequential
TODO: use std::vector

*/
void reduce_add_8(LweSample* result, LweSample** arrays, int num_arrays, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {

  if(num_arrays == 1) {
    copy(result, arrays[0], ck, size);
    return;
  }
  if(num_arrays == 2) {
    add(result, arrays[0], arrays[1], ck, size);
    return;
  }
  int ei_point = num_arrays / 8;
  LweSample *result1 = new_gate_bootstrapping_ciphertext_array(size, ck->params);
  LweSample *result2 = new_gate_bootstrapping_ciphertext_array(size, ck->params);
  LweSample *result3 = new_gate_bootstrapping_ciphertext_array(size, ck->params);
  LweSample *result4 = new_gate_bootstrapping_ciphertext_array(size, ck->params);
  LweSample *result5 = new_gate_bootstrapping_ciphertext_array(size, ck->params);
  LweSample *result6 = new_gate_bootstrapping_ciphertext_array(size, ck->params);
  LweSample *result7 = new_gate_bootstrapping_ciphertext_array(size, ck->params);
  #pragma omp parallel sections num_threads(8)
  {
    #pragma omp section
    reduce_add(result, arrays, ei_point, ck, size);
    #pragma omp section
    reduce_add(result1, &arrays[ei_point], 2*ei_point, ck, size);
    #pragma omp section
    reduce_add(result2, &arrays[2*ei_point], 3*ei_point, ck, size);
    #pragma omp section
    reduce_add(result3, &arrays[3*ei_point], 4*ei_point, ck, size);
    #pragma omp section
    reduce_add(result4, &arrays[4*ei_point], 5*ei_point, ck, size);
    #pragma omp section
    reduce_add(result5, &arrays[5*ei_point], 6*ei_point, ck, size);
    #pragma omp section
    reduce_add(result6, &arrays[6*ei_point], 7*ei_point, ck, size);
    #pragma omp section
    reduce_add(result7, &arrays[7*ei_point], num_arrays-7*ei_point, ck, size);
  }

  add(result, result, result1, ck, size);
  add(result, result, result2, ck, size);
  add(result, result, result3, ck, size);
  add(result, result, result4, ck, size);
  add(result, result, result5, ck, size);
  add(result, result, result6, ck, size);
  add(result, result, result7, ck, size);
  delete_gate_bootstrapping_ciphertext_array(size, result1);
  delete_gate_bootstrapping_ciphertext_array(size, result2);
  delete_gate_bootstrapping_ciphertext_array(size, result3);
  delete_gate_bootstrapping_ciphertext_array(size, result4);
  delete_gate_bootstrapping_ciphertext_array(size, result5);
  delete_gate_bootstrapping_ciphertext_array(size, result6);
  delete_gate_bootstrapping_ciphertext_array(size, result7);
}

/**
Python code: lr = lambda x, n, amnt: ((x << amnt) | (x >> (n-amnt)))&(2**n-1)
*/
void leftRotate(LweSample* result, const LweSample* a, const TFheGateBootstrappingCloudKeySet* ck, const size_t size, int amnt) {
  leftShift(result, a, ck, size, amnt);
  copy(result, &a[size-amnt], ck, amnt);
}







/*
 * Homomorphic bootstrapped COPY gate (doesn't need to be bootstrapped)
 * Takes in input 1 LWE samples (with message space [-1/8,1/8], noise<1/16)
 * Outputs a LWE sample (with message space [-1/8,1/8], noise<1/16)
*/
void bootsCOPYPointer(LweSample *result,  LweSample *a) {
    //lweCopyPointer(result, ca);
     *result= *a;
}

void copyPointer(LweSample**dest,  LweSample** source, const size_t size) {
*dest=*source;

/*  #pragma omp parallel for num_threads(NUM_THREADS)
  for(int i = 0; i < size; i++) {
    bootsCOPYPointer(dest[i], source[i]);
    
  }*/
}

/*
Numbers are assumed to be encoded in little-endian. I.e, with the LSB at the lowest position. In terms of bit arrays, this means
element 0 is the LSB. A left shift then corresponds to moving i to i+1, etc.
Note the left most bit is set to 0
*/
void leftShift(LweSample* result, const LweSample* a, const TFheGateBootstrappingCloudKeySet* ck, const size_t size, int amnt) {
  zero(&result[0], ck, size);
  copy(&result[amnt], a, ck, size-amnt);
}

/*void copy1(LweSample *b, LweSample *a){
*b=*a;
//printf("%d \n",*a);
//printf("%d \n",*b);
}*/
//###########
void leftShiftP(LweSample* result,  LweSample* a, const TFheGateBootstrappingCloudKeySet* ck, const size_t size, int amnt) {
  //zero(&result[0], ck, size);
  zero(result, ck, size);
  #pragma omp parallel for num_threads(NUM_THREADS)
  for(int i = amnt; i < size-1; i++) {
    //printf("size:%d ; amnt: %d\n",size,amnt);
    bootsCOPYPointer(&result[i], &a[i-amnt]);
  }
  //copyPointer(&result[amnt], a, size-amnt);
}

/***/
void rightShift(LweSample* result, const LweSample* a, const TFheGateBootstrappingCloudKeySet* ck, const size_t size, int amnt) {
  zero(result, ck, size);
  #pragma omp parallel for num_threads(NUM_THREADS)
  for(int i = size-1; i > (amnt-1); i--) {
    bootsCOPY(&result[i-amnt], &a[i], ck);
  }
}

//###########
void rightShiftP(LweSample* result,  LweSample* a, const TFheGateBootstrappingCloudKeySet* ck, const size_t size, int amnt) {
  zero(result, ck, size);
  #pragma omp parallel for num_threads(NUM_THREADS)
  for(int i = size-1; i > (amnt-1); i--) {
    bootsCOPYPointer(&result[i-amnt], &a[i]);
  }
}



/**
Python code: rr = lambda x, n, amnt: ((x >> amnt) | (x << (n-amnt)))&(2**n-1)
*/
void rightRotate(LweSample* result, const LweSample* a, const TFheGateBootstrappingCloudKeySet* ck, const size_t size, int amnt) {
  rightShift(result, a, ck, size, amnt);
  copy(&result[size-amnt], a, ck, amnt);
}



/**/
void sub(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  LweSample *c = new_gate_bootstrapping_ciphertext_array(size, ck->params);
  twosComplement(c, b, ck, size);
  add(result, a, c, ck, size);

  // clean up
  delete_gate_bootstrapping_ciphertext_array(size, c);
}

/**
Implements simple shift and add algorithm:
Let A and B be the operands, s.t P = AxB
Then P = Axb_0 << 0 + Axb_1 << 1 + ... + Axb_{n-1} << {n-1}
1. Multiply each bit of A by each bit of B, to get n n-bit arrays.
  - Note that for full precision, we would need n 2n-bit arrays. But we are dealing with fixed precision.
2. Reduce sum each of the n arrays

Space complexity is O(n^2)

*/
void mult(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  // to store the intermediate results of final result. Note intermediate result has 2n bits
  // p = p_0 * 2^0 + .. + p_{n-1} * 2^{n-1}
  // Create n n-bit arrays
  LweSample **p = new LweSample*[size];
  for(int i = 0; i < size; i++) {
    p[i] = new_gate_bootstrapping_ciphertext_array(size, ck->params);
    zero(p[i], ck, size);
  }
  #pragma omp parallel for num_threads(NUM_THREADS)
  for(int i = 0; i < size; i++) {
    for(int j = 0; j < size-i; j++) {
      bootsAND(&p[i][i+j], &a[j], &b[i], ck);
    }
  }
  reduce_add(result, p, size, ck, size);

  // clean up
  for(int i = 0; i < size; i++)
    delete_gate_bootstrapping_ciphertext_array(size, p[i]);
  delete[] p;
}

// NOTE assumes n >= 0
void power(LweSample* result, const LweSample* a, int n, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  if(n == 0) {
    zero(result, ck, size);
    bootsCONSTANT(&result[0], 1, ck);
  }
  else if(n == 1) {
    copy(result, a, ck, size);
  }

  for(int i = 0; i < n; i++) {
    mult(result, a, a, ck, size);
  }
}

/* Implements two's complement*/
void twosComplement(LweSample* result, const LweSample* a, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  LweSample *one = new_gate_bootstrapping_ciphertext_array(size, ck->params),
            *c = new_gate_bootstrapping_ciphertext_array(size, ck->params);;
  zero(one, ck, size);
  bootsCONSTANT(&one[0], 1, ck);

  NOT(c, a, ck, size);
  add(result, c, one, ck, size);

  // clean up
  delete_gate_bootstrapping_ciphertext_array(size, one);
  delete_gate_bootstrapping_ciphertext_array(size, c);
}

void NOT(LweSample* result, const LweSample* a, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  #pragma omp parallel for num_threads(NUM_THREADS)
  for(int i = 0; i < size; i++) {
    bootsNOT(&result[i], &a[i], ck);
  }
}

/** result = sample */
EXPORT void lweCopyPointer(LweSample* result, LweSample* sample){
    result=sample;
}




void copy(LweSample* dest, const LweSample* source, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  #pragma omp parallel for num_threads(NUM_THREADS)
  for(int i = 0; i < size; i++) {
    bootsCOPY(&dest[i], &source[i], ck);
  }
}

void zero(LweSample* result, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  #pragma omp parallel for num_threads(NUM_THREADS)
  for(int i = 0; i < size; i++) {
    bootsCONSTANT(&result[i], 0, ck);
  }
}

void OR(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  #pragma omp parallel for num_threads(NUM_THREADS)
  for(int i = 0; i < size; i++) {
    bootsOR(&result[i], &a[i], &b[i], ck);
  }
}

void AND(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  #pragma omp parallel for num_threads(NUM_THREADS)
  for(int i = 0; i < size; i++) {
    bootsAND(&result[i], &a[i], &b[i], ck);
  }
}

void NAND(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  #pragma omp parallel for num_threads(NUM_THREADS)
  for(int i = 0; i < size; i++) {
    bootsNAND(&result[i], &a[i], &b[i], ck);
  }
}

void NOR(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  #pragma omp parallel for num_threads(NUM_THREADS)
  for(int i = 0; i < size; i++) {
    bootsNOR(&result[i], &a[i], &b[i], ck);
  }
}

void XOR(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  #pragma omp parallel for num_threads(NUM_THREADS)
  for(int i = 0; i < size; i++) {
    bootsXOR(&result[i], &a[i], &b[i], ck);
  }
}

void XNOR(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  #pragma omp parallel for num_threads(NUM_THREADS)
  for(int i = 0; i < size; i++) {
    bootsXNOR(&result[i], &a[i], &b[i], ck);
  }
}
void ANDNY(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  #pragma omp parallel for num_threads(NUM_THREADS)
  for(int i = 0; i < size; i++) {
    bootsANDNY(&result[i], &a[i], &b[i], ck);
  }
}
void ANDYN(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  #pragma omp parallel for num_threads(NUM_THREADS)
  for(int i = 0; i < size; i++) {
    bootsANDYN(&result[i], &a[i], &b[i], ck);
  }
}
void ORNY(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  #pragma omp parallel for num_threads(NUM_THREADS)
  for(int i = 0; i < size; i++) {
    bootsORNY(&result[i], &a[i], &b[i], ck);
  }
}
void ORYN(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  #pragma omp parallel for num_threads(NUM_THREADS)
  for(int i = 0; i < size; i++) {
    bootsORYN(&result[i], &a[i], &b[i], ck);
  }
}
void MUX(LweSample* result, const LweSample* a, const LweSample* b, const LweSample* c, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  #pragma omp parallel for num_threads(NUM_THREADS)
  for(int i = 0; i < size; i++) {
    bootsMUX(&result[i], &a[i], &b[i], &c[i], ck);
  }
}

void CONSTANT(LweSample* result, const int& a, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  #pragma omp parallel for num_threads(NUM_THREADS)
  for(int i = 0; i < size; i++) {
    bootsCONSTANT(&result[i], (a >> i) & 1, ck);
  }
}
