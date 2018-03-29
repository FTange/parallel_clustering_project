#ifndef __SUBSPACE_H_INCLUDED
#define __SUBSPACE_H_INCLUDED

#include "util.h"
#include "clustering.h"

int subspace_clustering(std::vector<std::vector<float> > &db, int dim, 
					double epsilon, int minPts, int nthreads);

// enumerate all subspaces
void subspace_clustering_enum(std::vector<std::vector<float> > &db, int dim, 
					double epsilon, int minPts);

#endif
