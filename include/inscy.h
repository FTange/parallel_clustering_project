#ifndef __INSCY_H_INCLUDED
#define __INSCY_H_INCLUDUD

#include <vector>

// define structs used in inscy tree
struct descriptor;
struct inscy_node;
struct inscy_child;

struct descriptor {
    short dim;
    short interval;
    bool border;
};

struct inscy_node {
    descriptor descr;
    int count;
    std::vector<inscy_child> children;
};

struct inscy_child {
    descriptor descr;
    inscy_node *child;
};

inscy_node *init_scy_tree(std::vector<std::vector<float> > &db, double eps);

#endif
