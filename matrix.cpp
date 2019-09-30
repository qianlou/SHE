#include "alu.hpp"
#include "matrix.hpp"
/**
Element-wise addition

TODO implement integer and matrix classes to simplify all this code

*/
void mat_add(LweSample*** sum, LweSample*** a, LweSample*** b, const int rows, const int cols, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  for(int i = 0; i < rows; i++) {
    for(int j = 0; j < cols; j++) {
      add(sum[i][j], a[i][j], b[i][j], ck, size);
    }
  }
}

void elem_mult(LweSample*** prod, LweSample*** a, LweSample*** b, const int rows, const int cols, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  for(int i = 0; i < rows; i++) {
    for(int j = 0; j < cols; j++) {
      mult(prod[i][j], a[i][j], b[i][j], ck, size);
    }
  }
}

/**
Multiply each row of matrix elementwise by vector
*/
void elem_mult(LweSample*** prod, LweSample*** a, LweSample** b, const int rows, const int cols, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  for(int i = 0; i < rows; i++) {
    for(int j = 0; j < cols; j++) {
      mult(prod[i][j], a[i][j], b[j], ck, size);
    }
  }
}

void elem_mult(LweSample*** prod, LweSample** a, LweSample*** b, const int rows, const int cols, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  for(int i = 0; i < rows; i++) {
    for(int j = 0; j < cols; j++) {
      mult(prod[i][j], a[j], b[i][j], ck, size);
    }
  }
}

void elem_mult(LweSample** prod, LweSample** a, LweSample** b, const int cols, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  for(int j = 0; j < cols; j++) {
    mult(prod[j], a[j], b[j], ck, size);
  }
}

void mat_mult(LweSample*** prod, LweSample*** a, LweSample*** b, const int rows, const int cols, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  LweSample ***b_transpose = new LweSample**[rows];
  for(int i = 0; i < rows; i++) {
    b_transpose[i] = new LweSample*[cols];
    for(int j = 0; j < cols; j++) {
      b_transpose[i][j] = new_gate_bootstrapping_ciphertext_array(size, ck->params);
      copy(b_transpose[i][j], b[j][i], ck, size);
    }
  }

  LweSample ***temp = new LweSample**[rows];
  for(int i = 0; i < rows; i++) {
    temp[i] = new LweSample*[cols];
    for(int j = 0; j < cols; j++) {
      temp[i][j] = new_gate_bootstrapping_ciphertext_array(size, ck->params);
    }
  }

  for(int i = 0; i < rows; i++) {
    elem_mult(temp, b_transpose, a[i], rows, cols, ck, size);
    for(int j = 0; j < cols; j++) {
      reduce_add(prod[i][j], temp[i], cols, ck, size);
    }
  }

  for(int i = 0; i < rows; i++) {
    for(int j = 0; j < cols; j++) {
      delete_gate_bootstrapping_ciphertext_array(size, b_transpose[i][j]);
      delete_gate_bootstrapping_ciphertext_array(size, temp[i][j]);
    }
    delete[] b_transpose[i];
    delete[] temp[i];
  }
  delete[] b_transpose;
  delete[] temp;




}

void dot(LweSample* result, LweSample** a, LweSample** b, const int cols, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  LweSample **temp = new LweSample*[cols];
  for(int i = 0; i < cols; i++) 
  {
    temp[i] = new_gate_bootstrapping_ciphertext_array(size, ck->params);
  }
  elem_mult(temp, a, b, cols, ck, size);
  reduce_add(result, temp, cols, ck, size);
  for(int i = 0; i < cols; i++) {
    delete_gate_bootstrapping_ciphertext_array(size, temp[i]);
  }
  delete[] temp;
}

void elem_shift(LweSample** prod, LweSample** a, int* b, const int cols, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  for(int j = 0; j < cols; j++) 
   {
    if(b[j]<0)
    {
        rightShift(prod[j], a[j], ck, size,-b[j]);
    }else if (b[j]>0)
        {
        leftShift(prod[j], a[j], ck, size,b[j]);
        }
    else
       {copy(prod[j], a[j], ck, size);
        //copyPointer(&prod[j], &a[j], size);
       }
    }
}

void shiftDot(LweSample* result, LweSample** a, int* b, const int cols, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  const double clocks2seconds = 1. / CLOCKS_PER_SEC;
  LweSample **temp = new LweSample*[cols];
  for(int i = 0; i < cols; i++) {
    temp[i] = new_gate_bootstrapping_ciphertext_array(size, ck->params);
  }
  //elem_mult(temp, a, b, cols, ck, size);
  // Timings
  clock_t bs_begin, bs_end;
  bs_begin=clock();
  elem_shift(temp, a, b, cols, ck, size);
  bs_end=clock();
  printf("elem_shift time:%f\n",(bs_end-bs_begin)*clocks2seconds/2);

  bs_begin=clock();
  reduce_add(result, temp, cols, ck, size);
  bs_end=clock();
  printf("reduce_add time:%f\n",(bs_end-bs_begin)*clocks2seconds/2);
  /*for(int i = 0; i < cols; i++) {
    delete_gate_bootstrapping_ciphertext_array(size, temp[i]);
  }
  delete[] temp;*/
}

void transpose(LweSample*** transpose, const LweSample*** source, const TFheGateBootstrappingCloudKeySet* ck, size_t size) {
  // LweSample ***b_transpose = new LweSample**[array_size];
  // for(int i = 0; i < array_size; i++) {
  //   b_transpose[i] = new LweSample*[array_size];
  //   for(int j = 0; j < array_size; j++) {
  //     b_transpose[i][j] = new_gate_bootstrapping_ciphertext_array(size, params);
  //     copy(b_transpose[i][j], b[j][i], ck, size);
  //   }
  // }
}
