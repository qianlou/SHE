#include "encryption.hpp"
#include "alu.hpp"
#include "matrix.hpp"
#include <iostream>
#include <sys/time.h>


int ShiftDotProduct(int* inputs, int * We,const int cols){
	int result=0;
	for(int i=0; i<cols; i++){
		if(We[i]<0){
			for( int j=0; j<-We[i]; j++){
				inputs[i]=inputs[i]>>1;
			}
		}
		else if(We[i]>0){
			for( int j=0; j<We[i]; j++){
				inputs[i]=inputs[i]<<1;
			}} 
		result=result+inputs[i];
	}
	return result;
}


//Plaintext Max operations and verification
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
    for (int i=0; i<nb_bits-1; i++) {
        compare_bit(&tmps[0], &a[i], &b[i], &tmps[0], &tmps[1], bk);
    }
   //we need to handel the comparison between positive number and negative number
    LweSample* msb_nota = new_gate_bootstrapping_ciphertext_array(1, bk->params);
    LweSample* msb_notb = new_gate_bootstrapping_ciphertext_array(1, bk->params);
    LweSample* msb_nota_and_b = new_gate_bootstrapping_ciphertext_array(1, bk->params);
    LweSample* msb_notb_and_a = new_gate_bootstrapping_ciphertext_array(1, bk->params);
    LweSample* msb_notb_and_a_or_msb_notb_and_a = new_gate_bootstrapping_ciphertext_array(1, bk->params);
    LweSample* not_tmps = new_gate_bootstrapping_ciphertext_array(1, bk->params);
    bootsNOT(msb_nota, &a[nb_bits-1], bk);
    bootsNOT(msb_notb, &b[nb_bits-1], bk);
    bootsAND(msb_nota_and_b, msb_nota, &b[nb_bits-1], bk);
    bootsAND(msb_notb_and_a, msb_notb, &a[nb_bits-1], bk);
    bootsOR(msb_notb_and_a_or_msb_notb_and_a, msb_notb_and_a, msb_nota_and_b, bk);
    bootsNOT(not_tmps, &tmps[0], bk);
    bootsMUX(&tmps[0], msb_notb_and_a_or_msb_notb_and_a, not_tmps, &tmps[0], bk);
    //tmps[0] is the result of the comparaison: 0 if a is larger, 1 if b is larger
    //select the max and copy it to the result
    for (int i=0; i<nb_bits; i++) {
        bootsMUX(&result[i], &tmps[0], &a[i], &b[i], bk);
    }
    delete_gate_bootstrapping_ciphertext_array(2, tmps);    
}

void ReLU(LweSample* result, const LweSample* a,const int bits,const TFheGateBootstrappingCloudKeySet* ck){
	LweSample* b=new_gate_bootstrapping_ciphertext_array(bits, ck->params);
	zero(b, ck, bits);
	maximum(result, a, b, bits, ck);
}


int verify(int A, int B){
    if (A==B){printf("Verify Sucess!");}
    else{printf("There is difference between plaintext result and decrypted result!");}
}

int main(){
        const double clocks2seconds = 1. / CLOCKS_PER_SEC;
	// setup parameters
	typedef int8_t num_type ;
	size_t bits = sizeof(num_type) * 8;
	const int minimum_lambda = 80;
	TFheGateBootstrappingParameterSet* params = new_default_gate_bootstrapping_parameters(minimum_lambda);
	const TFheGateBootstrappingSecretKeySet* sk = new_random_gate_bootstrapping_secret_keyset(params);
	const TFheGateBootstrappingCloudKeySet* ck = &sk->cloud;
	
        printf("######## 1. shiftDot(A[0:input_size-1], Be[0:input_size-1]) Verification#######\n");
        //Unencrypted DotProduct between inputs A[input_size] and B[input_size]
        int input_size=8;
        int result=0;
	int *A=new int[input_size];
	int *Be=new int[input_size];
        printf("A=[");
	for(int i=0; i< input_size; i++){
		A[i]=i;
                printf(" %d", A[i]);
	}
        
        printf("]\nBe=[");
	for(int i=0; i< input_size; i++){
		Be[i]=-1;
	        printf(" %d", Be[i]);
	}
        printf("]\n");
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
        shiftDot(Enc_result, Enc_A, Be, input_size, ck, bits);
        num_type plain_result=decrypt<num_type>(Enc_result, sk);

        
        
        printf("Decrypted Result:%d\n",plain_result);
        //cout<<"Decrypted Result"<<plain_hidden_result;
	printf("Plaintext Result: %d\n",result);


        
        int A1=0, B1=-4;
        printf("######## 2. Max(A=%d, B=%d) Verification ####### \n", A1, B1);
        LweSample * Max_Enc_A=new_gate_bootstrapping_ciphertext_array(bits, ck->params);
        encrypt<num_type>(Max_Enc_A, A1, sk);
        LweSample * Max_Enc_B=new_gate_bootstrapping_ciphertext_array(bits, ck->params);
        encrypt<num_type>(Max_Enc_B, B1, sk);
        LweSample * Max_Enc_Result=new_gate_bootstrapping_ciphertext_array(bits, ck->params);
        maximum(Max_Enc_Result, Max_Enc_A, Max_Enc_B, bits, ck);
        num_type Max_plain_result=decrypt<num_type>(Max_Enc_Result, sk);
        printf("Decrypted Result: %d\n",Max_plain_result);
        printf("Plaintex Result: %d\n",max(A1,B1));
        


	int A2=-5;
        printf("######## 3. ReLU(A=%d) Verification######## \n", A2);
        LweSample * ReLU_Enc_A=new_gate_bootstrapping_ciphertext_array(bits, ck->params);
        encrypt<num_type>(ReLU_Enc_A, A2, sk);
        LweSample * ReLU_Enc_Result=new_gate_bootstrapping_ciphertext_array(bits, ck->params);
        ReLU(ReLU_Enc_Result, ReLU_Enc_A,bits,ck);
        num_type ReLU_plain_result=decrypt<num_type>(ReLU_Enc_Result, sk);
        printf("Decrypted Result: %d\n",ReLU_plain_result);
        printf("Plaintex Result: %d\n",max(A2,0));
        //verify(ReLU_plain_result, max(A,0));

}


