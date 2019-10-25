#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#include "io.hpp"

using namespace std;

vector<vector<double>> readFile(string filepath, char delimiter) {
  string line;
  ifstream file(filepath);
  vector<vector<double>> read_to;
  if(file.is_open()) {
    while(getline(file, line)) {
      read_to.push_back(parseLine(line));
    }
    file.close();
  }
  else {
    cout << "Error: failed to open file." << endl;
  }
  return read_to;
}

int writeFile(vector<vector<double>> write_from, string filepath, char delimiter, string header) {
  ofstream file(filepath);
  if(file.is_open()) {
    if(header != "")
      file << header << "\n";
    for(vector<vector<double>>::iterator it = write_from.begin(); it != write_from.end(); it++) {
      file << rowToString(*it, delimiter) << "\n";
    }
    file.close();
  }
  else {
    cout << "Error: failed to open file." << endl;
    return -1;
  }
  return 0;

}

int writeFile(vector<double> write_from, string filepath, char delimiter, string header) {
  ofstream file(filepath);
  if(file.is_open()) {
    if(header != "")
      file << header << "\n";
    file << rowToString(write_from, delimiter) << "\n";
    file.close();
  }
  else {
    cout << "Error: failed to open file." << endl;
    return -1;
  }
  return 0;
}

vector<double> parseLine(string line, char delimiter) {
  size_t next = 0;
  string elem;
  vector<double> parsed;
  do {
    next = line.find(delimiter);
    elem = line.substr(0, next);
    parsed.push_back(stod(elem));
    line.erase(0, next + 1);
  } while(next != string::npos);
  return parsed;
}

string rowToString(vector<double> row, char delimiter) {
  string line = "";
  for(double i: row) {
    line += to_string(i) + delimiter;
  }
  line.erase(line.length()-1);
  return line;

}
/*
int main() {
  string line = "32.43342,43.90943,3,24,3";
  vector<double> parsed = parseLine(line);
  for(double i: parsed)
    cout << " " << i;
  cout << endl;
  return 0;
  // cout << rowToString(parsed) << endl;
  vector<vector<double>> vec = readFile("testcsv.csv");
  for(vector<vector<double>>::iterator it = vec.begin(); it != vec.end(); it++) {
    cout << rowToString(*it) << endl;
  }
  // vec.push_back(parsed);
  // vec.push_back(parsed);
  // cout << writeFile(vec, "testcsv.csv") << endl;
}
*/
