#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <set>

#include "util.h"

using std::vector;
using std::string;

vector<vector<float> > read_csv(string filename) {
	vector<vector<float> > rows;
	std::ifstream infile(filename);
	if (!infile.is_open()) {
		printf("unable to open file");
		return rows;
	}

	string line; string field;
	float field_f;
	getline(infile, line);
	int dimensions = 1 + count(line.begin(), line.end(), ',');

	do {
		int i = 0;
		vector<float> row(dimensions);
		std::istringstream s(line);
		while (getline(s, field, ',')) {
			field_f = stod(field);
			row[i++] = field_f;
		}
		rows.push_back(row);

	} while (getline(infile, line));

	// print_vector_of_vectors(rows);

	return rows;
}

void print_vector_of_vectors(vector<vector<float> > &rows) {
	for (vector<vector<float> >::iterator it = rows.begin(); it != rows.end(); it++) {
		for (vector<float>::iterator it2 = (*it).begin(); it2 != (*it).end(); it2++) {
			std::cout << *it2 << " ";
		}
		std::cout << std::endl;
	}
}

// use template for vector of ints, double...
void print_vector(vector<float> &row) {
	// for (vector<vector<float> >::iterator it = row.begin(); it != row.end(); it++) {
	for (auto elem : row) {
		std::cout << elem << " ";
	}
	std::cout << std::endl;
}

vector<std::set<int> > clusters_as_sets(vector<int> clustering) {
	int clusters = 1 + *std::max_element(clustering.begin(), clustering.end());
	vector<std::set<int> > sets(clusters);
	for (int i = 0; i < clusters; i++) {
		std::set<int> cluster_i;
		sets[i] = cluster_i;
	}
	for (int i = 0; i < clustering.size(); i++) {
		sets[clustering[i]].insert(i);
	}

	return sets;
}
