#include <cmath>
#include <vector>

#include "subspace.h"

using std::vector;

void subspace_clustering(vector<vector<float> > &db, int dim, double epsilon, int minPts) {
	for (unsigned int i = 1, n = pow(2, dim); i < n; i++) {
		auto dist_with_subspace = [&] (vector<float> p1, vector<float> p2) {
			return euclid_dist_subspace(&i, p1, p2); 
		};
		vector<int> clustering = dbscan(db, epsilon, minPts, dist_with_subspace);
		/*
		for (auto x : clustering) {
			std::cout << x << " ";
		}
		*/
		// std::cout << std::endl;
	}
}
