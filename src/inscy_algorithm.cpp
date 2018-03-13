#include <iostream>

#include "inscy.h"

using std::vector;
using std::cout;
using std::endl;

void print_scy_tree(inscy_node *head, int level = 0);

void inscy_algorithm(vector<vector<float> > &db, double eps) {
    inscy_node *t1 = init_scy_tree(db, eps);
    // inscy_node *t2 = copy_tree(t1);
    // inscy_node *new_tree = merge_trees(t1, t2);

    print_scy_tree(t1);

    inscy_node *t2 = restrict_tree(t1, descriptor({2, 2, 0}));

    cout << "printing restricted tree" << endl;
    print_scy_tree(t2);
    cout << "restricting again" << endl;
    inscy_node *t3 = restrict_tree(t2, descriptor({1, 0, 0}));

    cout << "printing restricted tree" << endl;
    print_scy_tree(t3);
    // check_inscy_tree(t2, 3, t2->count);

    // for each descriptor restrict tree {
    // restrict tree
}

// TODO this uses dimenions for root which is zero

// returns an inscy_tree restricted to the descriptors
// Assumes descriptors is in same order as the tree
inscy_node *restrict_tree(inscy_node *head, descriptor descr) {
    if (head->descr.dim == descr.dim-1) {
        int child_index = 2*descr.interval + descr.border;
        if (head->children[child_index] == NULL ||
            head->descr.border != descr.border) {
            return NULL;
        }
        int in_interval = ((head->children[child_index] == NULL) ? 0 : 
                            head->children[child_index]->count);
        inscy_node *copy = new inscy_node({head->descr, in_interval});
        return copy;
    }
    inscy_node *copy = new inscy_node({head->descr});
    for (int i = descr.border; i < 2*bins; i++) {
        if (head->children[i] != NULL) {
            copy->children[i] = restrict_tree(head->children[i], descr);
            copy->count += ((copy->children[i] == NULL) ? 0 : 
                             copy->children[i]->count);
        }
    }
    if (copy->count == 0) {
        return NULL;
    }

    return copy;
}

// returns a new tree which is t1 and t2 merged
inscy_node *merge_trees(inscy_node *t1, inscy_node*t2) {
    if (t1 == NULL) {
        cout << "got here" << endl;
        return copy_tree(t2);
    }
    if (t2 == NULL) {
        cout << "got here instead" << endl;
        return copy_tree(t1);
    }
    // is this necessary? keep for now to find problems later
    if (t1->descr.dim      != t2->descr.dim ||
        t1->descr.interval != t2->descr.interval ||
        t1->descr.border   != t2->descr.border) {
        cout << "cannot merge trees with different descriptors" << endl;
        return NULL;
    }
    inscy_node *new_node = new inscy_node({t1->descr, t1->count + t2->count});
    for (int i = 0; i < 2*bins; i++) {
        if (t1->children[i] != NULL || t2->children[i] != NULL) {
            new_node->children[i] = merge_trees(t1->children[i], t2->children[i]);
        }
    }

    return new_node;
}

inscy_node *copy_tree(inscy_node *head) {
    if (head == NULL) { return NULL; }

    inscy_node *new_node = new inscy_node({head->descr, head->count});
    for (int i = 0; i < 2*bins; i++) {
        if (head->children[i] != NULL) {
            new_node->children[i] = copy_tree(head->children[i]);
        }
    }

    return new_node;
}

void print_scy_tree(inscy_node *head, int level) {
    if (head == NULL) { return; }
    for (int i = 0; i < level; i++) {
        cout << "    ";
    }
    cout << "(" << head->count << " " << head->descr.dim << " " << 
        head->descr.interval << " " << head->descr.border << ")" << endl;
    for (int i = 0; i < 2*bins; i++) {
        if (head->children[i] != NULL) {
            print_scy_tree(head->children[i], level + 1);
        }
    }
}
