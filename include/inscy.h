#ifndef __INSCY_H_INCLUDED
#define __INSCY_H_INCLUDED

#include <vector>

#define bins 9

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
           std::vector<point> &db,
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

#endif
