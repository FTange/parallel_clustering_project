#include <cmath>
#include <vector>
#include <omp.h>

#include "subspace.h"

#include <bitset>

using std::vector;

void cluster_in_subspace(vector<vector<float> > &db, int dim, double epsilon, int minPts,
                unsigned int subspace, vector<int> *clusterings[], vector<long> &spaces, 
                omp_lock_t locks[]);

int it = 0;

int subspace_clustering(vector<vector<float> > &db, int dim, double epsilon, int minPts, 
        int nthreads) {
	int n = pow(2, dim);
	vector<long> spaces(n); // supports up to 64 dimensions
	// vector<vector<int> > clusterings(n);
	vector<int> *clusterings[n] = {NULL};
    omp_lock_t locks[n];
    it = 0; 
   #pragma omp parallel num_threads(nthreads)
	{
    if (omp_get_num_threads() != nthreads) {
        std::cout << "unable to allocate " << nthreads << std::endl;
    }
    int id = omp_get_thread_num();
    // make master create tasks for 1d spaces, while the others create locks
    if (id == 0) {
        for (int i = 0; i < dim; i++) {
            unsigned int subspace = 1 << i;
           #pragma omp task firstprivate(subspace)
            {
                cluster_in_subspace(db, dim, epsilon, minPts, subspace, 
                                    clusterings, spaces, locks);
            }
        }
	}
   #pragma omp for schedule(dynamic)
    for (int i = 1; i < n; i++) {
        omp_init_lock(&(locks[i]));
    }
    }

    for (int i = 1; i < n; i++) {
        omp_destroy_lock(&(locks[i]));
    }
    for (int i = 1; i < n; i++) {
        delete clusterings[i];
    }

    return it;
}

// local function to handle a single subspace in subspace clustering
void cluster_in_subspace(vector<vector<float> > &db, int dim, double epsilon, int minPts,
                unsigned int subspace, vector<int> *clusterings[], vector<long> &spaces,
                omp_lock_t locks[]) {

    // create distance function for the subspace
    auto dist_with_subspace = [&] (vector<float> p1, vector<float> p2) {
        return euclid_dist_subspace(&subspace, p1, p2); 
    };
    it++;

    vector<int> *clustering;
    // if we already have allocated a subspace vector use it
    if (clusterings[subspace] == NULL) {
        clustering = new vector<int>(db.size());
    } else {
        clustering = clusterings[subspace];
    }

    dbscan(db, epsilon, minPts, dist_with_subspace, *clustering);
    bool contains_cluster = false;
    for (int c : *clustering) {
        if (c >= 1) {
            contains_cluster = true;
            break;
        }
    }
    if (!contains_cluster) {
        return;
    }
    clusterings[subspace] = clustering;

    for (int i = 0; i < dim; i++) {
        unsigned int superspace = subspace | (1 << i);
        if (superspace > subspace) { // space of one more dimension
            // acquire lock
            omp_set_lock(&(locks[superspace]));
            spaces[superspace]++;
            if (clusterings[superspace] == NULL) {
                clusterings[superspace] = new vector<int>(db.size());
            }
            vector<int> &superspace_cluster = *clusterings[superspace];
            long subspaces_completed = spaces[superspace];
            omp_unset_lock(&(locks[superspace]));
            vector<int> &subspace_cluster = *clustering;
            for (int j = 0; j < db.size(); j++) {
                if (subspace_cluster[i] == 0) {
                    superspace_cluster[i] = -1;
                }
            }

            // superspace not ready to be handled
            if (subspaces_completed < __builtin_popcount(superspace)) { continue; }

            // check if we can prune superspace before we create task
           #pragma omp task shared(spaces, clusterings) firstprivate(superspace)
            {
                cluster_in_subspace(db, dim, epsilon, minPts, superspace, 
                                    clusterings, spaces, locks);
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
