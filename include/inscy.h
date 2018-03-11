#ifndef __INSCY_H_INCLUDED
#define __INSCY_H_INCLUDUD

#include <vector>

// define structs used in inscy tree
struct descriptor;
struct inscy_node;

struct descriptor {
    short dim;
    short interval;
    bool border;
};

struct inscy_node {
    descriptor descr;
    int count;
    inscy_node *children[20];
};

inscy_node *init_scy_tree(std::vector<std::vector<float> > &db, double eps);

#endif
