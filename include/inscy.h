#ifndef __INSCY_H_INCLUDED
#define __INSCY_H_INCLUDUD

#include <vector>

#define bins 3

// define structs used in inscy tree
struct descriptor;
struct inscy_node;

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


inscy_node *init_scy_tree(std::vector<std::vector<float> > &db, double eps);

void delete_scy_tree(inscy_node * head);

bool check_inscy_tree(inscy_node *root, int dim, int numPoints);

void inscy_algorithm(std::vector<std::vector<float> > &db, double eps, int minPts);

void eDusc(inscy_node *root, int minPts, std::vector<restriction> &restricted_dim
        , int lvl, int dims, std::vector<std::vector<float> > &db);

inscy_node *merge_trees(inscy_node *t1, inscy_node*t2);

inscy_node *restrict_tree(inscy_node *root, descriptor descr);

inscy_node *copy_tree(inscy_node *head);

std::vector<std::vector<float> > get_points(std::vector<std::vector<float> > &db, 
        std::vector<restriction> &restrictions);


#endif
