/*
Implements higher level encryption functions to make using tfhe easier
*/

#include <cstddef>
#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>
#include <vector>
#include "omp_constants.hpp"


using namespace std;
/*Encrypt plaintext sample given secret key*/
template<typename T>

void encrypt(LweSample* cipher, T plaintext, const TFheGateBootstrappingSecretKeySet* sk) {
  uint8_t type_size = sizeof(T) * 8;
  for(int i = 0; i < type_size; i++) {
    bootsSymEncrypt(&cipher[i], (plaintext >> i) & 1, sk);
  }
}

template<typename T>
void encrypt(LweSample** cipher, vector<T>& plaintext, const TFheGateBootstrappingSecretKeySet* sk) {
  uint8_t type_size = sizeof(T) * 8;
  // #pragma omp parallel for num_threads(NUM_THREADS)
  for(int i = 0; i < plaintext.length(); i++) {
    encrypt(cipher[i], plaintext[i], sk);
  }
}

/*Decrypt cipher given secret key*/
template<typename T>
T decrypt(LweSample* cipher, const TFheGateBootstrappingSecretKeySet* sk) {
  uint8_t type_size = sizeof(T) * 8;
  T plaintext = 0;
  for(int i = 0; i < type_size; i++) {
    plaintext |= bootsSymDecrypt(&cipher[i], sk) << i;
  }
  return plaintext;
}

template<typename T>
vector<T> decrypt(LweSample** cipher, int length, const TFheGateBootstrappingSecretKeySet* sk) {
  uint8_t type_size = sizeof(T) * 8;
  vector<T> plaintext(length, 0);
  // #pragma omp parallel for num_threads(NUM_THREADS)
  for(int i = 0; i < length; i++) {
    for(int j = 0; j < type_size; j++) {
      plaintext[i] |= bootsSymDecrypt(&cipher[i][j], sk) << j;
    }
  }
  return plaintext;
}
