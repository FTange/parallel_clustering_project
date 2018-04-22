#include <algorithm>

#include <omp.h>

#include "clustering.h"

using std::vector;
using std::string;
using std::cout;
using std::endl;

vector<int> dbscan(vector<vector<float> > &db, double eps, int minPts,
				 std::function<double (vector<float> &, vector<float> &)> dist) {
	int n = db.size();
	vector<int> clustering(n);

	int numCluster = 1;
	for (int i = 0; i < n; i++ ) {
		if (clustering[i] != 0) { continue; } /* i is already classified */

		// if i has been added to new cluster, increment numCluster
		extend_cluster(db, i, eps, minPts, dist, clustering, numCluster);
		// point i and everything density-reachable from it has been added to numCluster
		if (clustering[i] == numCluster) {
			numCluster++;
		}
	}

	return clustering;
}

// saves result in vector<int> clustering
void dbscan(vector<vector<float> > &db, double eps, int minPts,
				 std::function<double (vector<float> &, vector<float> &)> dist,
				 std::vector<int> &clustering) {
	int n = db.size();

	int numCluster = 1;
	for (int i = 0; i < n; i++ ) {
		if (clustering[i] != 0) { continue; } /* i is already classified */

		// if i has been added to new cluster, increment numCluster
		extend_cluster(db, i, eps, minPts, dist, clustering, numCluster);
		// point i and everything density-reachable from it has been added to numCluster
		if (clustering[i] == numCluster) {
			numCluster++;
		}
	}
}




// ------------------------------------------------------------
// versions for inscy working with vector<point>...
void extend_cluster(vector<point> &db, int point, double eps, int minPts,
					std::function<double (vector<float> &, vector<float> &)> dist, 
					vector<int> &clustering, int currCluster) {
	int neighborhood_size = 0;
	vector<int> unclustered_neighbors;
	// go through all points and see if the current point is a core point
	// and find all unclassified neighbors
	for (int i = 0; i < db.size(); i++) {
		if (i == point) { continue; }
		double dist_i = dist(db[point].values, db[i].values);
		// cout << dist_i << endl;
		if (dist_i <= eps) {
			neighborhood_size++;
			// the neighbor we have found is unclassified
			if (clustering[i] == 0) {
				unclustered_neighbors.push_back(i);
			}
		}
	}

	// if point is a new core point, go through all its unclustered neighbors
	if (neighborhood_size >= minPts) {
		clustering[point] = currCluster;
		for (int neighbor : unclustered_neighbors) {
			if (clustering[neighbor] != 0) {
				cout << "why are we here" << endl;
			}
			clustering[neighbor] = currCluster;
		}
		for (int neighbor : unclustered_neighbors) {
			// recursively extend cluster by going through unclustered points in neighborhood
			extend_cluster(db, neighbor, eps, minPts, dist, clustering, currCluster);
		}
	}
}


vector<int> dbscan(vector<point> &db, double eps, int minPts,
				 std::function<double (vector<float> &, vector<float> &)> dist) {
	int n = db.size();
	vector<int> clustering(n);

	int numCluster = 1;
	for (int i = 0; i < n; i++ ) {
		if (clustering[i] != 0) { continue; } /* i is already classified */

		// if i has been added to new cluster, increment numCluster
		extend_cluster(db, i, eps, minPts, dist, clustering, numCluster);
		// point i and everything density-reachable from it has been added to numCluster
		if (clustering[i] == numCluster) {
			numCluster++;
		}
	}

	return clustering;
}

double euclid_dist_inscy(vector<restriction> restr_dim, 
                         vector<float> &p1, vector<float> &p2) {
	double distance = 0;
	for (restriction d : restr_dim) {
		double difference = p1[d.dim-1] - p2[d.dim-1];
		distance += difference * difference;
	}

	return sqrt(distance);
}

void dbscan_inscy(vector<point> &db, double eps, int minPts,
                  vector<restriction> restricted_dimensions,
				  vector<cluster> &clustering) {
    auto restricted_dist = [&] (vector<float> &a, vector<float> &b) {
        return euclid_dist_inscy(restricted_dimensions, a, b); 
    };

    // vector<int> clusters = dbscan(db, eps, minPts, restricted_dist);
    vector<int> clusters = dbscan(db, eps, minPts, euclid_dist);

    int max = *std::max_element(clusters.begin(), clusters.end());
    for (int i = 1; i <= max; i++) {
        cluster new_cluster_i;
        new_cluster_i.subspace = restricted_dimensions;
        for (int j = 0; j < db.size(); j++) {
            if (clusters[j] == i) {
                new_cluster_i.points.push_back(db[j].index);
            }
        }
        clustering.push_back(new_cluster_i);
    }

    // store the vector<restriction> together with any clusters found,
    // wastes some space but it is available and you store it together with a cluster
    // so it is d space together with an entire cluster, depends on the expected size of a cluster
}

// ------------------------------------------------------------




/*
 * Should unclustered_neighbors be a parameter so it can be reused?
 */
void extend_cluster(vector<vector<float> > &db, int point, double eps, int minPts,
					std::function<double (vector<float> &, vector<float> &)> dist, 
					vector<int> &clustering, int currCluster) {
	int neighborhood_size = 0;
	vector<int> unclustered_neighbors;
	// go through all points and see if the current point is a core point
	// and find all unclassified neighbors
	for (int i = 0; i < db.size(); i++) {
		if (i == point) { continue; }
		double dist_i = dist(db[point], db[i]);
		// cout << dist_i << endl;
		if (dist_i <= eps) {
			neighborhood_size++;
			// the neighbor we have found is unclassified
			if (clustering[i] == 0) {
				unclustered_neighbors.push_back(i);
			}
		}
	}

	// if point is a new core point, go through all its unclustered neighbors
	if (neighborhood_size >= minPts) {
		clustering[point] = currCluster;
		for (int neighbor : unclustered_neighbors) {
			if (clustering[neighbor] != 0) {
				cout << "why are we here" << endl;
			}
			clustering[neighbor] = currCluster;
		}
		for (int neighbor : unclustered_neighbors) {
			// recursively extend cluster by going through unclustered points in neighborhood
			extend_cluster(db, neighbor, eps, minPts, dist, clustering, currCluster);
		}
	}
}

int neighborhood(vector<vector<float>> &db, int point, double eps,
				 std::function<double (vector<float> &, vector<float> &)> dist,
				 int *neighborhood) {
	int neighborhood_size = 0;
	for (int i = 0; i < db.size(); i++) {
		if (i == point) { continue; }
		double dist_i = dist(db[point], db[i]);
		if (dist_i <= eps) {
			neighborhood[neighborhood_size] = i;
			neighborhood_size++;
		}
	}
	return neighborhood_size;
}


double euclid_dist_subspace(unsigned int *subspace, vector<float> &p1, vector<float> &p2) {
	int dim_per_int = sizeof(int) * 8, subspace_shift = 0, curr_subspace = subspace[0];
	double distance = 0, difference;
	for(int i = 0, n = p1.size(); i < n; i++) {
		/* we have gone through all subspaces in current int */
		if (subspace_shift == dim_per_int) {
			curr_subspace = subspace[i / dim_per_int];
			subspace_shift = 0;
		}
		if (1 & (curr_subspace >> subspace_shift)) {
			difference = p1[i] - p2[i];
			distance += difference * difference;
		}
		subspace_shift += 1;
	}

	return sqrt(distance);
}

/*
double euclid_dist_subspace(unsigned int *subspace, vector<float> &p1, vector<float> &p2) {
	int dim_per_int = sizeof(int) * 8, int_index, subspace_shift = 0;
	cout << dim_per_int << endl;
	double distance = 0, difference;
	for(int i = 0, n = p1.size(); i < n; i++) {
		int_index = i / dim_per_int;
		subspace_shift = i % dim_per_int;
		if (1 & (subspace[int_index] >> subspace_shift)) {
			printf("i is %2d, int_index: %2d, subspace_shift: %2d\n", i, int_index, subspace_shift);
			difference = p1[i] - p2[i];
			distance += difference * difference;
		}
	}

	return sqrt(distance);
}
*/

double euclid_dist(vector<float> &p1, vector<float> &p2) {
	double distance = 0;
	for(int i = 0, n = p1.size(); i < n; i++) {
		double difference = p1[i] - p2[i];
		distance += difference * difference;
	}

	return sqrt(distance);
}
