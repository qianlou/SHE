# SHE: A Fast and Accurate Deep Neural Network for Encrypted Data. 
This code is used for testing shiftDotProduction, ReLU, and max pooling operations on encrypted data based on fully HE. The ReLU and other circuits based on leveled HE has additional constraints on how gates can be combined. For example, the Cmux gate is usually used in leveled HE. 

# Dependency:
Please install TFHE library firstly. https://github.com/tfhe/tfhe

# Install:
./src/make SHE
# Run
./src/SHE
# References:
TFHE-ML.



