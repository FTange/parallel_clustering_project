#include <cmath>
#include <vector>
#include <omp.h>

#include "subspace.h"

#include <bitset>

using std::vector;

void cluster_in_subspace(vector<vector<float> > &db, int dim, double epsilon, int minPts,
                unsigned int subspace, vector<int> *clusterings[], vector<long> &spaces);

void subspace_clustering(vector<vector<float> > &db, int dim, double epsilon, int minPts) {
	int n = pow(2, dim);
	vector<long> spaces(n); // supports up to 64 dimensions
	// vector<vector<int> > clusterings(n);
	vector<int> *clusterings[n] = {NULL};
   #pragma omp parallel
	{
   #pragma omp single
    {
        for (int i = 0; i < dim; i++) {
            unsigned int subspace = 1 << i;
           #pragma omp task firstprivate(subspace)
            {
                cluster_in_subspace(db, dim, epsilon, minPts, subspace, 
                                    clusterings, spaces);
            }
        }
	}
    }
    std::cout << "got to here" << std::endl;
    for (int i = 1; i < n; i++) {
        std::cout << i << " " << spaces[i] << " " << __builtin_popcountll(i) << std::endl;
    }

    /*
    // std::cout << "clustered " << clusterings_completed << " " << std::endl;
    for (int i = 0; i < dim; i++) {
        int subspace = 1 << i;
        std::cout << "printing clustering in dimension " << subspace << std::endl;
        for (auto c : *clusterings[subspace]) {
            std::cout << c << " "; 
        }
        std::cout << std::endl;
    }
    */
}

// local function to handle a single subspace in subspace clustering
void cluster_in_subspace(vector<vector<float> > &db, int dim, double epsilon, int minPts,
                unsigned int subspace, vector<int> *clusterings[], vector<long> &spaces) {

    // create distance function for the subspace
    auto dist_with_subspace = [&] (vector<float> p1, vector<float> p2) {
        return euclid_dist_subspace(&subspace, p1, p2); 
    };

    vector<int> *clustering;
    // if we already have allocated a subspace vector use it
    if (clusterings[subspace] == NULL) {
        clustering = new vector<int>(db.size());
    } else {
        clustering = clusterings[subspace];
    }

    dbscan(db, epsilon, minPts, dist_with_subspace, *clustering);
    // check that clustering contains one cluster
    clusterings[subspace] = clustering;

    for (int i = 0; i < dim; i++) {
        unsigned int superspace = subspace | (1 << i);
        if (superspace > subspace) { // space of one more dimension
            // acquire lock
            #pragma omp atomic
                spaces[superspace]++;

            if (superspace == 1022) {
                std::cout << subspace << " " << __builtin_popcount(subspace) << std::endl;
            }
            // superspace not ready to be handled
            if (spaces[superspace] != __builtin_popcount(spaces[superspace])) { continue; }


            // check if we can prune superspace before we create task
           #pragma omp task shared(spaces, clusterings) firstprivate(superspace)
            {
            cluster_in_subspace(db, dim, epsilon, minPts, superspace, 
                                clusterings, spaces);
            }
        } 
    }
}

void subspace_clustering_enum(vector<vector<float> > &db, int dim, double epsilon, int minPts) {
	int n = pow(2, dim);
	#pragma omp parallel for num_threads(1)
	for (unsigned int i = 1; i < n; i++) {
		auto dist_with_subspace = [&] (vector<float> p1, vector<float> p2) {
			return euclid_dist_subspace(&i, p1, p2); 
		};
		vector<int> clustering = dbscan(db, epsilon, minPts, dist_with_subspace);
		vector<std::set<int> > sets = clusters_as_sets(clustering);

		/*
		for (int j = 0: j < dim; j++) {
			if ((i | (1 << j)) > i) {
				// (i | (1 << j) is a superspace of i with one more dimension
			}
		}
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
