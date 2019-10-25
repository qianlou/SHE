#include <vector>

double calculateRecall(std::vector<double> ground_truth_classes, std::vector<double> predicted_classes);
double calculatePrecision(std::vector<double> ground_truth_classes, std::vector<double> predicted_classes);
double calculateAccuracy(std::vector<double> ground_truth_classes, std::vector<double> predicted_classes);
int calculateTP(std::vector<double> ground_truth_classes, std::vector<double> predicted_classes);
int calculateTN(std::vector<double> ground_truth_classes, std::vector<double> predicted_classes);
int calculateFP(std::vector<double> ground_truth_classes, std::vector<double> predicted_classes);
int calculateFN(std::vector<double> ground_truth_classes, std::vector<double> predicted_classes);
