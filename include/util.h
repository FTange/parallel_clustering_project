#ifndef __UTIL_H_INCLUDED
#define __UTIL_H_INCLUDED

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <set>

std::vector<std::vector<float> > read_csv(std::string filename);

void print_vector_of_vectors(std::vector<std::vector<float> > &rows);

void print_vector(std::vector<float> &row);

std::vector<std::set<int> > clusters_as_sets(std::vector<int> clustering);

#endif
