#ifndef __UTIL_H_INCLUDED
#define __UTIL_H_INCLUDED

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

vector<vector<float>> read_csv(string filename);

void print_vector_of_vectors(vector<vector<float>> &rows);

void print_vector(vector<float> &row);

#endif
