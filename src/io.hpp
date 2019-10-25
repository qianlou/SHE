#pragma once

#include <vector>
#include <string>
// COULD use a template but the use case is quite specific
std::vector<std::vector<double>> readFile(std::string filepath, char delimiter=',');
int writeFile(std::vector<std::vector<double>> write_from, std::string filepath, char delimiter=',', std::string header="");
int writeFile(std::vector<double> write_from, std::string filepath, char delimiter=',', std::string header="");
std::vector<double> parseLine(std::string line, char delimiter=',');
std::string rowToString(std::vector<double> row, char delimiter=',');
