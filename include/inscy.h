#ifndef __INSCY_H_INCLUDED
#define __INSCY_H_INCLUDED

#include <vector>
#include <deque>

#define bins 3

// define structs used in inscy tree
struct descriptor {
    short dim;
    short interval;
    bool border;
};

struct restriction {
    short dim;
    short from;
    short to;
};

struct inscy_node {
    descriptor descr;
    int count;
    inscy_node *children[2*bins];
};

struct point {
    int index;
    std::vector<float> values;
};

struct cluster {
    std::vector<restriction> subspace;
    std::vector<int> points; // index of points in the cluster
};


inscy_node *init_scy_tree(std::vector<std::vector<float> > &db, double eps);

void delete_scy_tree(inscy_node * head);

bool check_inscy_tree(inscy_node *root, int dim, int numPoints);

void inscy_algorithm(std::vector<std::vector<float> > &db, double eps, double delta, int minPts);

void eDusc(inscy_node *root,
           int minPts,
           std::vector<restriction> &restricted_dims,
           int lvl,
           int dims,
           std::vector<std::vector<float> > &db,
           double eps,
           double delta,
           std::vector<cluster> &clusters);

bool weak_density_scan(std::vector<point> &db, double eps, int minPts,
                  std::vector<restriction> restricted_dimensions);

inscy_node *merge_trees(inscy_node *t1, inscy_node*t2);

inscy_node *restrict_tree(inscy_node *root, descriptor descr);

inscy_node *copy_tree(inscy_node *head);

std::vector<point> get_points(std::vector<std::vector<float> > &db, 
        std::vector<restriction> &restrictions);

std::vector<point> get_points_all_dim(std::vector<point> &db, restriction &new_restr);

bool point_in_restriction(std::vector<point> point, std::vector<restriction> &restrictions);

bool descriptor_in_tree(inscy_node *head, descriptor descr);

std::vector<bool> occupied_connectivity_borders(inscy_node *head, int dim);

// restricts to multiple regions at a time
inscy_node *restrict_tree(inscy_node *head, restriction restr);

bool redundancy_scan(std::vector<cluster> clusters, 
        std::vector<restriction> restricted_dims, double delta, int numPoints);

void print_scy_tree(inscy_node *head, int level = 0);

int find_dim_lvl(inscy_node *head, descriptor descr, int lvl = 0);

/* ----------------------------------------
 * cpu implementation
 */
void inscy_algorithm_cpu(std::vector<std::vector<float> > &db, 
        double eps, double delta, int minPts, int num_threads);


void eDusc_dim_cpu(inscy_node *root,
                   int minPts,
                   std::vector<restriction> &restricted_dims,
                   int lvl,
                   int dims,
                   std::vector<std::vector<float> > &db,
                   double eps,
                   double delta,
                   std::deque<cluster> &clusters);

void eDusc_cpu(inscy_node *root,
           int minPts,
           std::vector<restriction> &restricted_dims,
           int lvl,
           int dims,
           short curr_dim,
           std::vector<std::vector<float> > &db,
           double eps,
           double delta,
           std::deque<cluster> &clusters,
           short first_bin, short last_bin);

#endif
