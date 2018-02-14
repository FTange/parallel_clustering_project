#include "util.h"

vector<vector<float>> read_csv(string filename) {
	vector<vector<float>> rows;
	ifstream infile(filename);
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
		istringstream s(line);
		while (getline(s, field, ',')) {
			field_f = stod(field);
			row[i++] = field_f;
		}
		rows.push_back(row);

	} while (getline(infile, line));

	// print_vector_of_vectors(rows);

	return rows;
}

void print_vector_of_vectors(vector<vector<float>> &rows) {
	for (vector<vector<float>>::iterator it = rows.begin(); it != rows.end(); it++) {
		for (vector<float>::iterator it2 = (*it).begin(); it2 != (*it).end(); it2++) {
			cout << *it2 << " ";
		}
		cout << endl;
	}
}

// use template for vector of ints, double...
void print_vector(vector<float> &row) {
	// for (vector<vector<float>>::iterator it = row.begin(); it != row.end(); it++) {
	for (auto elem : row) {
		cout << elem << " ";
	}
	cout << endl;
}
