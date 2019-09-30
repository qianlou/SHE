/**
  Implements machine learning performance metrics
*/
#include "metrics.hpp"

using namespace std;

double calculateRecall(vector<double> ground_truth_classes, vector<double> predicted_classes) {
  double tp = calculateTP(ground_truth_classes, predicted_classes),
         fn = calculateFN(ground_truth_classes, predicted_classes);
  return tp / (tp + fn);
}

double calculatePrecision(vector<double> ground_truth_classes, vector<double> predicted_classes) {
  double tp = calculateTP(ground_truth_classes, predicted_classes),
         fp = calculateFP(ground_truth_classes, predicted_classes);
  return tp / (tp + fp);
}

double calculateAccuracy(vector<double> ground_truth_classes, vector<double> predicted_classes) {
  double tp = calculateTP(ground_truth_classes, predicted_classes),
         tn = calculateTN(ground_truth_classes, predicted_classes),
         fn = calculateFN(ground_truth_classes, predicted_classes),
         fp = calculateFP(ground_truth_classes, predicted_classes);
  return (tp + tn) / (tp + tn + fp + fn);
}

int calculateTP(vector<double> ground_truth_classes, vector<double> predicted_classes) {
  int tp = 0;
  for(int i = 0; i < ground_truth_classes.size(); i++) {
    if(ground_truth_classes[i] == predicted_classes[i] == 1)
      tp++;
  }
  return tp;
}

int calculateTN(vector<double> ground_truth_classes, vector<double> predicted_classes) {
  int tn = 0;
  for(int i = 0; i < ground_truth_classes.size(); i++) {
    if(ground_truth_classes[i] == 0 && predicted_classes[i] == 0)
      tn++;
  }
  return tn;
}

int calculateFP(vector<double> ground_truth_classes, vector<double> predicted_classes) {
  int fp = 0;
  for(int i = 0; i < ground_truth_classes.size(); i++) {
    if(ground_truth_classes[i] == 0 &&  predicted_classes[i] == 1)
      fp++;
  }
  return fp;
}

int calculateFN(vector<double> ground_truth_classes, vector<double> predicted_classes) {
  int fn = 0;
  for(int i = 0; i < ground_truth_classes.size(); i++) {
    if(ground_truth_classes[i] == 1 && predicted_classes[i] == 0)
      fn++;
  }
  return fn;
}
