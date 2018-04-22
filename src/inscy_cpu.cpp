#include <iostream>
#include <vector>
#include <chrono>
#include <omp.h>
#include <deque>

#include "inscy.h"
#include "clustering.h"
#include "math_unsigned.cpp"

using std::vector;
using std::deque;
using std::cout;
using std::endl;

deque<inscy_node *> trees;

void dbscan_inscy_cpu(vector<point> &db, double eps, int minPts,
                  vector<restriction> restricted_dimensions,
				  deque<cluster> &clustering) {
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
       #pragma omp critical
        {
        clustering.push_back(new_cluster_i);
        }
    }

    // store the vector<restriction> together with any clusters found,
    // wastes some space but it is available and you store it together with a cluster
    // so it is d space together with an entire cluster, depends on the expected size of a cluster
}

void inscy_algorithm_cpu2(inscy_node *t1, std::vector<std::vector<float> > &db, 
        vector<restriction> &restricted_dimensions,
        deque<cluster> &clusters,
        double eps, double delta, int minPts, int num_threads) {

   #pragma omp parallel num_threads(num_threads) shared(db, clusters)

   #pragma omp single
    {
        if (omp_get_num_threads() != num_threads) {
            cout << "only able to allocate " << omp_get_num_threads() << " not " 
                << num_threads << endl;
        }
        eDusc_dim_cpu(t1, minPts, restricted_dimensions, 0, db[0].size(), db, 
                eps, delta, clusters);
    }
}

void inscy_algorithm_cpu(std::vector<std::vector<float> > &db, 
        double eps, double delta, int minPts, int num_threads) {

    inscy_node *t1 = init_scy_tree(db, eps);

    vector<restriction> restricted_dimensions;
    // vector<cluster> clusters = *(new vector<cluster>);
    deque<cluster> clusters;

    auto start = std::chrono::system_clock::now();
    inscy_algorithm_cpu2(t1, db, restricted_dimensions, clusters, eps, delta, minPts, num_threads);
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    cout << "found " << clusters.size() << " clusters in " << elapsed.count() << endl;
    delete_scy_tree(t1);
    // cout << trees.size() << " trees to be deleted" << endl;
    for (inscy_node *tree : trees) {
        delete_scy_tree(tree);
    }
}

void eDusc_dim_cpu(inscy_node *root,
                   int minPts,
                   vector<restriction> &restricted_dims,
                   int lvl,
                   int dims,
                   vector<vector<float> > &db,
                   double eps,
                   double delta,
                   deque<cluster> &clusters) {

    int next_dim;
    if (restricted_dims.size() == 0) {
        next_dim = 1;
    } else {
        next_dim = restricted_dims.back().dim+1;
    }

    for (short dim = next_dim; dim <= dims; dim++) {
        vector<bool> occupied_borders = occupied_connectivity_borders(root, dim);
        for (int i = 0, j = 0; i < bins; i++) {
            while (j < bins && occupied_borders[j++] == 1) { }
            // only create a new task if there is enough dimensions remaining for the 
            // remaining work to be significant
            if (dims - lvl > 3) {
               #pragma omp task firstprivate(restricted_dims) shared(clusters, db)
                {
                eDusc_cpu(root, minPts, restricted_dims, lvl, dims, dim, db, eps, delta, 
                          clusters, i, j);
                }
            } else {
                eDusc_cpu(root, minPts, restricted_dims, lvl, dims, dim, db, eps, delta, 
                          clusters, i, j);
            }
            i = j-1;
        }
    }
}

void eDusc_cpu(inscy_node *root,
               int minPts,
               vector<restriction> &restricted_dims,
               int lvl,
               int dims,
               short curr_dim,
               vector<vector<float> > &db,
               double eps,
               double delta,
               deque<cluster> &clusters,
               short first_bin, short last_bin) {

    inscy_node *restricted_tree = restrict_tree(root, descriptor({curr_dim, first_bin, 0}));
    // cout << "looking in dim " << curr_dim << " from " << first_bin << " to " << last_bin;

    // construct tree for the entire region [first_bin..last_bin[
    for (short interv = first_bin+1; interv < last_bin; interv++) {
        inscy_node *new_restricted_tree = restrict_tree(root, descriptor({curr_dim, interv, 0}));
        inscy_node *tmp = merge_trees(restricted_tree, new_restricted_tree);
        delete_scy_tree(new_restricted_tree);
        delete_scy_tree(restricted_tree);
        restricted_tree = tmp;
    }

    restriction restr = {curr_dim, first_bin, last_bin};
    // cout << " with " << ((restricted_tree == NULL) ? 0 : restricted_tree->count )
            // << " points" <<  " restrictions: ";
    // for (restriction r : restricted_dims) {
        // cout << r.dim << " " << r.from << "-" << r.to << ", ";
    // }

    // first density filter
    if (restricted_tree == NULL || restricted_tree->count < minPts) {
        // cout << " less than minPts, " << endl;
        // delete_scy_tree(restricted_tree);
        delete_scy_tree(restricted_tree);
        /*
        if (restricted_tree != NULL) {
           #pragma omp critical
            {
                trees.push_back(restricted_tree);
            }
        }
        */
        return;
    }
    // cout << endl;


    restricted_dims.push_back(restr);
    int first_superspace_cl = clusters.size();
    eDusc_dim_cpu(restricted_tree, minPts, restricted_dims, lvl+1, dims, db, 
          eps, delta, clusters);
    int last_superspace_cl = clusters.size();

    // bool redundant = redundancy_scan(clusters, restricted_dims, delta, 
                                     // restricted_tree->count);
    bool redundant = false;
    if (!redundant) {
        vector<point> restr_db = get_points(db, restricted_dims);
        dbscan_inscy_cpu(restr_db, eps, minPts, restricted_dims, clusters);
    }

    // delete_scy_tree(restricted_tree);
   #pragma omp critical
    {
        trees.push_back(restricted_tree);
    }
    restricted_dims.pop_back();
}
