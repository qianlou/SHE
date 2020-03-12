/**
* Implementation of approximate logistic regression.
* Approximates sigmoid function with n-degree polynomial.
* Coefficients are estimated using Remez's algorithm, as implemented in the lolremez tool (https://github.com/samhocevar/lolremez)
* Assumes weights have already been optimized in the clear

Components:
1. Data
 - Data will be in form of Nxd array
 - Data will be encrypted by the time it gets to this module

2. Outputs
  - Outputs will be in form of bits. Each bit will give target estimate

TODO: replace raw pointers with smart
*/

#pragma once

#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>
#include <cstddef>
#include <vector>
#include <string>

class ApproxLogRegression {
  private:

    /* Logistic regression-related members */
    std::string weight_path;  // path to weights
    std::string coefs_path;  // path to polynomial coefficients
    LweSample **weights;  // regression weights
    int dim;  // input data dimension
    LweSample **coefs;  // TODO optimize by accounting for null coefficients
    uint8_t degree;  // polynomial degree
    /* HE-related members */
    const TFheGateBootstrappingCloudKeySet* ck;  // cloud key set
    size_t size;  // number of bits of precision
    size_t scale_factor;  // factor for fixed-float conversions
    bool mode_clip;  // If true, use range [-2^(n-1), 2^(n-1)-1] and clip to range, else use [-2^(n-2), 2^(n-2)-1] instead

  public:

    ApproxLogRegression(std::string weight_path, std::string coefs_path, int dim, const TFheGateBootstrappingCloudKeySet* ck, size_t size, size_t scale_factor, bool mode_clip=true);

    ApproxLogRegression(std::vector<double> weights_in, std::vector<double> coefs_in, int dim, const TFheGateBootstrappingCloudKeySet* ck, size_t size, size_t scale_factor, bool mode_clip=true);

    /**
      Run inference on given sample X
    */
    void predict(LweSample* y, LweSample** X);

    /**
      Compute polynomial approximation to sigmoid
    */
    void approxSigmoid(LweSample* y, LweSample* X);

    /**
      Compute single forward pass of logistic regression
    */
    void forward(LweSample* y, LweSample** X);

    /**
      Compute preactivation of forward pass
    */
    void preactivation(LweSample* y, LweSample** X);


};
