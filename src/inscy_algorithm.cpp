#include <iostream>
#include <vector>

#include "inscy.h"

using std::vector;
using std::cout;
using std::endl;

void print_scy_tree(inscy_node *head, int level = 0);
int find_dim_lvl(inscy_node *head, descriptor descr, int lvl = 0);

void inscy_algorithm(vector<vector<float> > &db, double eps, int minPts) {
    inscy_node *t1 = init_scy_tree(db, eps);
    // inscy_node *t2 = copy_tree(t1);
    // inscy_node *new_tree = merge_trees(t1, t2);

    print_scy_tree(t1);

    vector<restriction> restricted_dimensions;
    eDusc(t1, minPts, restricted_dimensions, 0, db[0].size(), db);

    /*
    vector<restriction> restricted_dimensions;
    restriction r1 = {1, 2, 3};
    restriction r2 = {2, 2, 3};
    restriction r3 = {3, 0, 1};
    restricted_dimensions.push_back(r1);
    restricted_dimensions.push_back(r2);
    restricted_dimensions.push_back(r3);
    vector<vector<float> > points = get_points(db, restricted_dimensions);
    cout << points.size() << endl;
    */

    /*
    for (int i = 0; i < db.size(); i++) {
        for (int j = 0; j < db[0].size(); j++) {
            cout << db[i][j] << " ";
        }
        cout << endl;
        for (int j = 0; j < db[0].size(); j++) {
            cout << points[i][j] << " ";
        }
        cout << endl << endl;
    }
    */


    /*
    inscy_node *t2 = restrict_tree(t1, descriptor({1, 0, 0}));
    print_scy_tree(t2);

    inscy_node *t3 = restrict_tree(t2, descriptor({3, 2, 0}));
    print_scy_tree(t3);

    cout << "printing restricted tree" << endl;
    if (t2 == NULL) {
        cout << "t2 is null" << endl;
    }
    print_scy_tree(t2);
    // check_inscy_tree(t2, 2, t2->count);
    cout << "restricting again" << endl;
    inscy_node *t3 = restrict_tree(t2, descriptor({2, 2, 0}));

    cout << "printing restricted tree" << endl;
    print_scy_tree(t3);
    */

    // for each descriptor restrict tree {
    // restrict tree
}

void eDusc(inscy_node *root, int minPts, vector<restriction> &restricted_dims
        , int lvl, int dims, vector<vector<float> > &db) {
    int next_dim;
    if (restricted_dims.size() == 0) {
        next_dim = 1;
    } else {
        next_dim = restricted_dims.back().dim+1;
    }

    for (short dim = next_dim; dim <= dims; dim++) {
        for (short interv = 0; interv < bins; interv++) {
            for (int ind = 0; ind < lvl; ind++) {
                cout << "    ";
            }
            inscy_node *restricted_tree = restrict_tree(root, {dim, interv, 0});
            cout << "looking at interval " << interv << " in dimension " << dim << " with " << 
                ((restricted_tree == NULL) ? 0 : restricted_tree->count)
                << " points";

            if (restricted_tree == NULL) {
                cout << " continuing as restricted tree is null" << endl;
                continue;
            }
            cout << endl;

            inscy_node *tree_border = restrict_tree(root, {dim, interv, 1});
            
            short interv_to = interv + 1;
            restricted_dims.push_back({dim, interv, interv_to});
            eDusc(restricted_tree, minPts, restricted_dims, lvl+1, dims, db);
            restricted_dims.pop_back();
        }
    }
}

/*
 * Helper function to find the level that the dimension occur in the tree
 */
int find_dim_lvl(inscy_node *head, descriptor descr, int lvl) {
    if (head == NULL) {
        cout << "dimension does not exist in tree" << endl;
        return 0;
    }

    if (head->descr.dim == descr.dim) {
        return lvl;
    }
    for (int i = 0; i < 2*bins; i++) {
        if (head->children[i] != NULL) {
            return find_dim_lvl(head->children[i], descr, lvl+1);
        }
    }
}

inscy_node *restrict_tree_rec(inscy_node *head, descriptor descr, int lvl) {
    if (head == NULL) { return NULL; }

    if (lvl == 1) {
        int child_index = 2*descr.interval + descr.border;
        if (head->children[child_index] == NULL) {
            return NULL;
        }
        inscy_node *child = head->children[child_index];
        inscy_node *copy = new inscy_node({head->descr, child->count});
        for (int i = 0; i < 2*bins; i++) {
            if (child->children[i] != NULL) {
                copy->children[i] = copy_tree(child->children[i]);
            }
        }
        return copy;
    }

    inscy_node *copy = new inscy_node({head->descr});
    bool all_null = true;
    for (int i = 0; i < 2*bins; i++) {
        if (head->children[i] != NULL) {
            copy->children[i] = restrict_tree_rec(head->children[i], descr, lvl-1);
            if (copy->children[i] != NULL) {
                copy->count += copy->children[i]->count;
                all_null = false;
            }
        }
    }
    if (all_null) {
        return NULL;
    }

    return copy;
}

inscy_node *restrict_tree(inscy_node *head, descriptor descr) {
    int lvl = find_dim_lvl(head, descr);
    
    return restrict_tree_rec(head, descr, lvl);
}


// returns a new tree which is t1 and t2 merged
inscy_node *merge_trees(inscy_node *t1, inscy_node*t2) {
    if (t1 == NULL) {
        return copy_tree(t2);
    }
    if (t2 == NULL) {
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


void eDusc_old(inscy_node *root, int minPts, short currDim, int lvl) {
    if (currDim == 0) {
        return;
    }
    inscy_node *prev_tree = NULL;
    for (short i = 0; i < bins; i++) {
        for (int ind = 0; ind < lvl; ind++) {
            cout << "    ";
        }
        inscy_node *restricted_tree = restrict_tree(root, {currDim, i, 0});
        cout << "looking at interval " << i << " in dimension " << currDim << " with " << 
            ((restricted_tree == NULL) ? 0 : restricted_tree->count)
            << " points";

        if (restricted_tree == NULL && prev_tree == NULL) {
            cout << " continuing as restricted tree is null" << endl;
            continue;
        }

        inscy_node *tree_border = restrict_tree(root, {currDim, i, 1});

        if (prev_tree != NULL) {
            cout << " merging with prev tree ";
            restricted_tree = merge_trees(restricted_tree, prev_tree);
        }
        prev_tree = NULL;
        if (tree_border != NULL && i < bins-1) {
            cout << " there is a border point so we don't recurse " << endl;
            prev_tree = restricted_tree;
            // cout << "not recursing as there is a border in " << currDim << " interval " << i << endl;
            continue;
        }

        // induce

        // merge

        // prune based on minPts, no subspace can conatin minPts
        /*
        if (restricted_tree->count < minPts) {
            continue;
        }
        */
        cout << " - recursing on " << 
            ((restricted_tree == NULL) ? 0 : restricted_tree->count) 
            << " points" << endl;
        
        eDusc_old(restricted_tree, minPts, currDim-1, lvl+1);
    }
}


// returns an inscy_tree restricted to the descriptors
// Assumes descriptors is in same order as the tree
inscy_node *restrict_tree_leaf(inscy_node *head, descriptor descr) {
    if (head == NULL) {
        return NULL;
    }
    if (head->descr.dim == descr.dim-1) {
        int child_index = 2*descr.interval + descr.border;
        if (head->children[child_index] == NULL) {
            return NULL;
        }
        int child_count = head->children[child_index]->count;
        inscy_node *copy = new inscy_node({head->descr, child_count});
        return copy;
    }
    inscy_node *copy = new inscy_node({head->descr});
    bool all_null = true;
    for (int i = 0; i < 2*bins; i++) {
        if (head->children[i] != NULL) {
            copy->children[i] = restrict_tree(head->children[i], descr);
            if (copy->children[i] != NULL) {
                copy->count += copy->children[i]->count;
                all_null = false;
            }
        }
    }
    if (all_null) {
        return NULL;
    }

    return copy;
}
