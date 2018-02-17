#include <cmath>
#include <vector>
#include <omp.h>

#include "subspace.h"

using std::vector;

void subspace_clustering(vector<vector<float> > &db, int dim, double epsilon, int minPts) {
	int n = pow(2, dim);
	#pragma omp parallel for num_threads(1)
	for (unsigned int i = 1; i < n; i++) {
		auto dist_with_subspace = [&] (vector<float> p1, vector<float> p2) {
			return euclid_dist_subspace(&i, p1, p2); 
		};
		vector<int> clustering = dbscan(db, epsilon, minPts, dist_with_subspace);
		vector<std::set<int> > sets = clusters_as_sets(clustering);

		/*
		// print the vector of sets
		for (int i = 0; i < sets.size(); i++) {
			std::cout << "cluter " << i << std::endl;
			for (auto j : sets[i]) {
				std::cout << j << " ";
			}
			std::cout << std::endl;
		}

		for (auto x : clustering) {
			std::cout << x << " ";
		}
		std::cout << std::endl;
		*/
	}
}
