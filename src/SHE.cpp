#include "encryption.hpp"
#include "alu.hpp"
#include "matrix.hpp"
#include <iostream>
#include <sys/time.h>
// using namespace std;

//Shift Dot Product operations and verification
//int Homo_ShiftDotProduct(LweSample* result, LweSample** a, int* b, const int cols, const TFheGateBootstrappingCloudKeySet* ck, const size_t size){
 //      shiftDot(hidden_result[j], enc_inputs, weights[j], input_size, ck,size);
//}
int ShiftDotProduct(int* inputs, int * We,const int cols){
	int result=0;
	for(int i=0; i<cols; i++){
		//Converting We into W;
		int W=1;
		if(We[i]<0){
			for( int j=0; j<-We[i]; j++){
				W=W*(1/2);
			}
		}
		else if(We[i]>0){W=W*(2);} 
		result=result+inputs[i]*W;

	}

	return result;
}

//Max operations and verification
int max(int A, int B){
if(A>B) return A;
else return B;
}


// elementary full comparator gate that is used to compare the i-th bit:
//   input: ai and bi the i-th bit of a and b
//          lsb_carry: the result of the comparison on the lowest bits
//   algo: if (a==b) return lsb_carry else return b 
void compare_bit(LweSample* result, const LweSample* a, const LweSample* b, const LweSample* lsb_carry, LweSample* tmp, const TFheGateBootstrappingCloudKeySet* bk) {
    bootsXNOR(tmp, a, b, bk);
    bootsMUX(result, tmp, lsb_carry, a, bk);
}
// this function compares two multibit words, and puts the max in result
void maximum(LweSample* result, const LweSample* a, const LweSample* b, const int nb_bits, const TFheGateBootstrappingCloudKeySet* bk) {
    LweSample* tmps = new_gate_bootstrapping_ciphertext_array(2, bk->params);
    
    //initialize the carry to 0
    bootsCONSTANT(&tmps[0], 0, bk);
    //run the elementary comparator gate n times
    for (int i=0; i<nb_bits; i++) {
        compare_bit(&tmps[0], &a[i], &b[i], &tmps[0], &tmps[1], bk);
    }
    //tmps[0] is the result of the comparaison: 0 if a is larger, 1 if b is larger
    //select the max and copy it to the result
    for (int i=0; i<nb_bits; i++) {
        bootsMUX(&result[i], &tmps[0], &a[i], &b[i], bk);
    }

    delete_gate_bootstrapping_ciphertext_array(2, tmps);    
}

void Relu(LweSample* result, const LweSample* a,const int bits,const TFheGateBootstrappingCloudKeySet* ck){

LweSample* b=new_gate_bootstrapping_ciphertext_array(bits, ck->params);
zero(b, ck, bits);
maximum(result, a, b, bits, ck);

}




//int DotProduct(){}
int main(){
        const double clocks2seconds = 1. / CLOCKS_PER_SEC;
	// setup parameters
	typedef int8_t num_type ;
	size_t bits = sizeof(num_type) * 8;
	const int minimum_lambda = 80;
	TFheGateBootstrappingParameterSet* params = new_default_gate_bootstrapping_parameters(minimum_lambda);
	const TFheGateBootstrappingSecretKeySet* sk = new_random_gate_bootstrapping_secret_keyset(params);
	const TFheGateBootstrappingCloudKeySet* ck = &sk->cloud;

        //Unencrypted DotProduct between inputs A[input_size] and B[input_size]
        int input_size=10;
        int result=0;
	int *A=new int[input_size];
	int *Be=new int[input_size];
	for(int i=0; i< input_size; i++){
		A[i]=i;
		Be[i]=1;
	}
	result=ShiftDotProduct(A,Be,input_size);

	//Encrypted DotProduct between Enc_A[input_size][bits] and B[input_size]
	LweSample **Enc_A=new LweSample*[input_size];
	LweSample *Enc_result = new_gate_bootstrapping_ciphertext_array(bits, ck->params);
	for(int i1 = 0; i1 < input_size; i1++) {
		Enc_A[i1] = new_gate_bootstrapping_ciphertext_array(bits, ck->params);
	}
        Enc_result = new_gate_bootstrapping_ciphertext_array(bits, ck->params);

	for(num_type i=0; i<input_size;i++){
	encrypt<num_type>(Enc_A[i], i, sk);
	}
        //num_type plain_hidden_result=decrypt<num_type>(enc_inputs[3], sk);
        shiftDot(Enc_result, Enc_A, Be, input_size, ck,bits);
        num_type plain_result=decrypt<num_type>(Enc_result, sk);
        printf("Shift Dot Product Verification:%d\n");
        printf("Decrypted Result:%d\n",plain_result);
        //cout<<"Decrypted Result"<<plain_hidden_result;
	printf("Result:%d\n",result);


        printf("Max Verification:\n");
        LweSample * Max_Enc_A=new_gate_bootstrapping_ciphertext_array(bits, ck->params);
        encrypt<num_type>(Max_Enc_A, -1, sk);
        LweSample * Max_Enc_B=new_gate_bootstrapping_ciphertext_array(bits, ck->params);
        encrypt<num_type>(Max_Enc_B, -4, sk);
        LweSample * Max_Enc_Result=new_gate_bootstrapping_ciphertext_array(bits, ck->params);
        maximum(Max_Enc_Result, Max_Enc_A, Max_Enc_B, bits, ck);
        num_type Max_plain_result=decrypt<num_type>(Max_Enc_Result, sk);
        printf("Decrypted Result:%d\n",Max_plain_result);

        printf("ReLU Verification:\n");
        LweSample * Relu_Enc_A=new_gate_bootstrapping_ciphertext_array(bits, ck->params);
        encrypt<num_type>(Relu_Enc_A, -5, sk);
        LweSample * Relu_Enc_Result=new_gate_bootstrapping_ciphertext_array(bits, ck->params);
        Relu(Relu_Enc_Result, Relu_Enc_A,bits,ck);
        num_type Relu_plain_result=decrypt<num_type>(Relu_Enc_Result, sk);
        printf("Decrypted Result:%d\n",Relu_plain_result);

}


