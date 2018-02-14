#include "clustering.h"

vector<int> dbscan(vector<vector<float>> &db, double eps, int minPts) {
	int n = db.size();
	vector<int> clustering(n);
	int neighborhood_i[n-1];

	int numCluster = 0;
	int neighborPts = 0;
	double dist_ij;
	for (int i = 0; i < n; i++ ) {
		if (clustering[i] != 0) { continue; } /* i is already classified */
		neighborPts = neighborhood(db, i, eps, euclid_dist, neighborhood_i);
		if (neighborPts < minPts) { continue; } /* i is not core point */

		int newCluster = ++numCluster; /* new cluster found, (inc first) */
		clustering[i] = newCluster;
		/* go through the neighbors of i */
		// add all of i's neighbors to the current cluster
		for (int j = 0; j < neighborPts; j++) {
			int p2 = neighborhood_i[j];
			if (clustering[p2] != 0) { continue; } /* already clustered */
			clustering[p2] = newCluster;
		}
		// call recursively on all points in the neighborhood of i
		// count all points, but only consider those not already classified
		// for further investigation
		for (int j = 0; j < neighborPts; j++) {
			int p2 = neighborhood_i[j];
			if (clustering[p2] != newCluster) { continue; } /* already clustered */
			extend_cluster(db, p2, eps, minPts, euclid_dist, clustering, newCluster);
		}
	}
	cout << "number of iterations is " << iterations << endl;

	return clustering;
}

int neighborhood(vector<vector<float>> &db, int point, double eps,
				 function<double (vector<float> &, vector<float> &)> dist,
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

void extend_cluster(vector<vector<float>> &db, int point, double eps, int minPts,
					function<double (vector<float> &, vector<float> &)> dist, 
					vector<int> &clustering, int cluster) {
	int neighborhood_size = 0;
	vector<int> unclustered_neighbors;
	// go through all points and see if the current point is a core point
	// and find all unclassified neighbors
	for (int i = 0; i < db.size(); i++) {
		if (i == point) { continue; }
		double dist_i = dist(db[point], db[i]);
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
		for (int neighbor : unclustered_neighbors) {
			if (clustering[neighbor] != 0) {
				cout << "why are we here" << endl;
			}
			clustering[neighbor] = cluster;
		}
		for (int neighbor : unclustered_neighbors) {
			
			// recursively extend cluster by going through unclustered points in neighborhood
			extend_cluster(db, neighbor, eps, minPts, dist, clustering, cluster);
		}
	}
}


double euclid_dist(vector<float> &p1, vector<float> &p2) {
	double distance = 0;
	for(int i = 0, n = p1.size(); i < n; i++) {
		double difference = p1[i] - p2[i];
		distance += difference * difference;
	}

	return sqrt(distance);
}
