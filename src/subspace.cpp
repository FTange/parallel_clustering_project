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
    auto dist_with_subspace = [&] (vector<float> p1, vector<float> p2) {
        return euclid_dist_subspace(&subspace, p1, p2); 
    };
    // std::cout << omp_get_thread_num() << " is clustering in " << 
    vector<int> clustering = dbscan(db, epsilon, minPts, dist_with_subspace);

    // TODO the clustering will not be saved as it is local to the function
    clusterings[subspace] = &clustering;

    for (int i = 0; i < dim; i++) {
        unsigned int superspace = subspace | (1 << i);
        if (superspace > subspace) {
            // acquire lock
            // superspace not already handled
            if (spaces[superspace] < superspace) {
                spaces[superspace] = spaces[superspace] | subspace;
                /*
                if (superspace == 7) {
                    std::cout << superspace << ", " << subspace << ", "
                        << &spaces[superspace] << std::endl;
                } */
                // should be able to release lock here since we don't change it anymore
                // superspace still not ready to be handled
                if (spaces[superspace] < superspace) { continue; }
                // check if we can prune superspace before we create task
               #pragma omp task shared(spaces, clusterings) firstprivate(superspace)
                {
                cluster_in_subspace(db, dim, epsilon, minPts, superspace, 
                                    clusterings, spaces);
                }
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
