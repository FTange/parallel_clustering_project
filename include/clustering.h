#ifndef __CLUSTERING_H_INCLUDED
#define __CLUSTERING_H_INCLUDED

#include <iostream>
#include <vector>
#include <cmath>
#include <functional>

#include "util.h"

std::vector<int> dbscan(std::vector<std::vector<float> > &db, double eps, int minPts, 
				 std::function<double (std::vector<float> &, 
									   std::vector<float> &)> dist /* distance function */
		);

// stores result in vector<int> clustering instead of returning it
void dbscan(std::vector<std::vector<float> > &db, double eps, int minPts, 
				 std::function<double (std::vector<float> &, 
									   std::vector<float> &)> dist, /* distance function */
                 std::vector<int> &clustering
		);
/*
 * calculate the neighors of point in db with dist less then eps
 * and store those neighobors in neighborhood and return the number found
 */
int neighborhood(std::vector<std::vector<float> > &db,
			     int point, /* index of point in db to find neighbors of */
				 double eps,
				 std::function<double (std::vector<float> &, std::vector<float> &)> dist, /* distance function */
				 int *neighborhood); /* array of size n to store the neighbors in */

/*
 * Extend a cluster by finding neighbors of point, and if point is core 
 * it adds it neighbors in clustering to cluster, and recursively
 * extends the cluster on them if they are not already clustered.
 *
 * Assumes clustering[point] == cluster
 */
void extend_cluster(std::vector<std::vector<float> > &db, int point, double eps, int minPts,
					std::function<double (std::vector<float> &, std::vector<float> &)> dist, 
					std::vector<int> &clustering, int currCluster);

/*
 * Only calculate dist in the subspace
 */
double euclid_dist_subspace(unsigned int *subspace, std::vector<float> &p1, std::vector<float> &p2);

double euclid_dist(std::vector<float> &p1, std::vector<float> &p2);


#endif
