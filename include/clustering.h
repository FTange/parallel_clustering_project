#ifndef __CLUSTERING_H_INCLUDED
#define __CLUSTERING_H_INCLUDED

#include <vector>
#include <cmath>
#include <functional>

#include "util.h"

using namespace std;

vector<int> dbscan(vector<vector<float>> &db, double eps, int minPts);

/*
 * calculate the neighors of point in db with dist less then eps
 * and store those neighobors in neighborhood and return the number found
 */
int neighborhood(vector<vector<float>> &db,
			     int point, /* index of point in db to find neighbors of */
				 double eps,
				 function<double (vector<float> &, vector<float> &)> dist, /* distance function */
				 int *neighborhood); /* array of size n to store the neighbors in */

/*
 * Extend a cluster by finding neighbors of point, and if point is core 
 * it adds it neighbors in clustering to cluster, and recursively
 * extends the cluster on them if they are not already clustered.
 *
 * Assumes clustering[point] == cluster
 */
void extend_cluster(vector<vector<float>> &db, int point, double eps, int minPts,
					function<double (vector<float> &, vector<float> &)> dist, 
					vector<int> &clustering, int cluster);

double euclid_dist(vector<float> &p1, vector<float> &p2);


#endif
