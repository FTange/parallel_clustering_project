#include "main.h"

int main() {
	vector<vector<float>> data = read_csv("x.csv");

	vector<int> clustering = dbscan(data, 0.5, 5);

	for (auto x : clustering) {
		cout << x << endl;
	}
}

