#include <iostream>
#include <vector>
#include <chrono>

#include "inscy.h"
#include "clustering.h"
#include "math_unsigned.cpp"

using std::vector;
using std::cout;
using std::endl;


void inscy_algorithm(vector<vector<float> > &db, double eps, double delta, int minPts) {
    inscy_node *t1 = init_scy_tree(db, eps);
    // inscy_node *t2 = copy_tree(t1);
    // inscy_node *new_tree = merge_trees(t1, t2);

    // print_scy_tree(t1);
    /*
    vector<bool> occupied = occupied_connectivity_borders(t1, 1);
    for (bool b : occupied) {
        cout << b << endl;
    }
    */

    vector<restriction> restricted_dimensions;
    vector<cluster> clusters;
    /*
    vector<point> db_points(db.size());
    for (int i = 0; i < db.size(); i++) {
        db_points[i] = {i, db[i]};
    }
    */

    // represent subspaces as arbitrary bitvectors
    /*
    math::Unsigned blah(0);
    math::Unsigned shift(1);
    for (int i = 0; i < 64; i++) {
        blah |= (shift << i);
    }
    cout << blah << endl;
    */

    auto start = std::chrono::system_clock::now();
    eDusc(t1, minPts, restricted_dimensions, 0, db[0].size(), db, eps, delta, clusters);
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    cout << "found " << clusters.size() << " clusters in " << elapsed.count() << endl;
    /*
    for (cluster c : clusters) {
        cout << c.points.size() << endl;
        for (restriction r : c.subspace) {
            cout << "  " << r.dim << " " << r.from << " " << r.to << endl;
        }
        cout << endl;
    }
    */

    /*
    vector<restriction> restricted_dimensions;
    restriction r1 = {1, 0, 3};
    restriction r2 = {2, 0, 3};
    restriction r3 = {3, 0, 3};
    restricted_dimensions.push_back(r1);
    restricted_dimensions.push_back(r2);
    restricted_dimensions.push_back(r3);
    vector<point> points = get_points(db, restricted_dimensions);
    cout << points.size() << endl;
    for (point p : points) {
        cout << p.index << " " << p.values.size() << endl;
    }
    vector<cluster> blah;
    dbscan_inscy(points, eps, minPts, restricted_dimensions, blah);


    vector<int> clusters = dbscan(db, eps, minPts, euclid_dist);
    cout << "clusters found regular" << endl;
    for (int c : clusters) {
        cout << c << endl;
    }
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

// lvl is only for printing purposes
void eDusc(inscy_node *root, 
           int minPts, 
           vector<restriction> &restricted_dims,
           int lvl, 
           int dims, 
           vector<vector<float> > &db, 
           double eps, 
           double delta,
           vector<cluster> &clusters) {

    int next_dim;
    if (restricted_dims.size() == 0) {
        next_dim = 1;
    } else {
        next_dim = restricted_dims.back().dim+1;
    }

    for (short dim = next_dim; dim <= dims; dim++) {
        inscy_node *prev_tree = NULL;
        restriction prev_restr;
        for (short interv = 0; interv < bins; interv++) {
            // for (int ind = 0; ind < lvl; ind++) {
                // cout << "    ";
            // }
            inscy_node *restricted_tree = restrict_tree(root, descriptor({dim, interv, 0}));
            // cout << "looking at interval " << interv << " in dimension " << dim << " with " << 
                // ((restricted_tree == NULL) ? 0 : restricted_tree->count) << " points";

            // inscy_node *tree_border = restrict_tree(root, {dim, interv, 1});

            restriction restr;
            if (prev_tree != NULL) {
                restricted_tree = merge_trees(restricted_tree, prev_tree);
                restr = prev_restr;
                restr.to = interv + 1;
                delete_scy_tree(prev_tree);
                prev_tree = NULL;
                // cout << " merging with previous tree, now containing " << 
                    // restricted_tree->count << " points, ";
            } else {
                short interv_to = interv + 1;
                restr = {dim, interv, interv_to};
            }

            // if there is a point inside the border and we aren't in the last interval
            // if (tree_border != NULL && interv < (bins-1)) {
            if (descriptor_in_tree(root, {dim, interv, 1}) && interv < (bins-1)) {
                // cout << " point in border so continuing " << endl;
                prev_tree = restricted_tree;
                prev_restr = restr;
                continue;
            }

            // first density filter
            if (restricted_tree == NULL || restricted_tree->count < minPts) {
                // cout << " less than minPts, " << endl;
                delete_scy_tree(restricted_tree);
                continue;
            }
            // cout << endl;

            // get the subset of points that are also in current region
            // vector<point> restr_db = get_points_all_dim(db, restr);

            // debugging
            /*
            if (restr_db.size() != restricted_tree->count) {
                // cout << "tree has size " << restricted_tree->count << " database contains " << 
                    // restr_db.size() << endl;
                if (restricted_tree->count == 2 && restr_db.size() == 4) {
                    restricted_dims.push_back(restr);
                    point_in_restriction(restr_db, restricted_dims);
                    exit(1);
                }
            }
            */
            // cout << endl;

            // second density filter
            /*
            if ( !weak_density_scan(restr_db, eps, minPts, restricted_dims) ) {
                continue;
            }
            */


            restricted_dims.push_back(restr);
            int first_superspace_cl = clusters.size();
            eDusc(restricted_tree, minPts, restricted_dims, lvl+1, dims, db, 
                  eps, delta, clusters);
            int last_superspace_cl = clusters.size();

            bool redundant = redundancy_scan(clusters, restricted_dims, delta, 
                                             restricted_tree->count);
            // bool redundant = false;
            if (!redundant) {
                vector<point> restr_db = get_points(db, restricted_dims);
                dbscan_inscy(restr_db, eps, minPts, restricted_dims, clusters);
            }

            delete_scy_tree(restricted_tree);
            restricted_dims.pop_back();
        }
    }
}

// assumes restrictions are in sorted order of dimension
bool redundancy_scan(vector<cluster> clusters, 
        vector<restriction> restricted_dims, double delta, int numPoints) {

    int numRestrictions = restricted_dims.size();
    int j, k;
    for (int i = 0; i < clusters.size(); i++) {
        // if cluster[i] contains more restrictions
        if (clusters[i].subspace.size() > numRestrictions) {
            // if cluster[i] contains enough points for this cluster to be redundant
            if (clusters[i].points.size() >= numPoints * delta) {
                vector<restriction> &cluster_restrictions = clusters[i].subspace;
                for (j = 0, k = 0; j < numRestrictions; j++) {
                    while (cluster_restrictions[k].dim < restricted_dims[j].dim) { k++; }
                    if (restricted_dims[j].dim != cluster_restrictions[k].dim) {
                        break; // current cluster is not restricted in restricted_dims[j]
                    }
                    // current cluster is restricted in another region in current dim
                    if (restricted_dims[j].from < cluster_restrictions[k].from
                          || cluster_restrictions[k].to < restricted_dims[j].to) {
                        break;
                    }
                }
                /*we didn't break out of loop and clusters[i].points.size() 
                      is less than redundancy parameter allows for, return true;*/
                if (j == numRestrictions) {
                    /*
                    cout << "cluster of " << numPoints << " points in space ";
                    for (restriction r : restricted_dims) {
                        cout << r.dim << ":" << r.from << "-" << r.to << " ";
                    }
                    cout << "due to cluster of " << clusters[i].points.size() 
                        << " points in space ";
                    for (restriction r : cluster_restrictions) {
                        cout << r.dim << ":" << r.from << "-" << r.to << " ";
                    }
                    cout << endl;
                    */
                    return true;
                }
            }
        }
    }
    return false;
}


bool weak_density_scan(vector<point> &db, double eps, int minPts,
                  vector<restriction> restricted_dimensions) {
    auto restricted_dist = [&] (vector<float> &a, vector<float> &b) {
        return euclid_dist_inscy(restricted_dimensions, a, b); 
    };
    for (int i = 0; i < db.size(); i++) {
        int in_neighborhood = 0;
        for (int j = 0; j < db.size(); j++) {
            if (i == j) { continue; }
            if (restricted_dist(db[i].values, db[j].values) <= eps) {
                in_neighborhood++;
            }
        }
        if (in_neighborhood >= minPts) {
            return true;
        }
    }
    // cout << "no weak dense points, db size " << db.size() << " restricted in " 
        // << restricted_dimensions.size() << endl;
    return false;
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

bool descriptor_in_tree_rec(inscy_node *head, descriptor descr, int lvl) {
    if (head == NULL) { return false; }

    if (lvl == 0) {
        if (head->descr.interval    == descr.interval
             && head->descr.dim    == descr.dim
             && head->descr.border == descr.border) {
            return true;
        }
        return false;
    } else {
        bool in_subtree = false;
        for (int i = 0; i < 2*bins; i++) {
            if (head->children[i] != NULL) {
                in_subtree |= descriptor_in_tree_rec(head->children[i], descr, lvl-1);
            }
        }
        return in_subtree;
    }
}

bool descriptor_in_tree(inscy_node *head, descriptor descr) {
    int lvl = find_dim_lvl(head, descr);
    
    return descriptor_in_tree_rec(head, descr, lvl);
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
        delete copy;
        return NULL;
    }

    return copy;
}

inscy_node *restrict_tree(inscy_node *head, descriptor descr) {
    int lvl = find_dim_lvl(head, descr);
    
    return restrict_tree_rec(head, descr, lvl);
}

vector<bool> occupied_connectivity_borders(inscy_node *head, int dim) {
    vector<bool> occupied(bins);
    if (dim == 1) { // look for points in connectivity border
        for (int i = 0; i < bins; i++) {
            if (head->children[i*2+1] != NULL) {
                occupied[i] = true;
            }
        }
    } else { // recursively look through tree
        for (int i = 0; i < bins; i++) {
            inscy_node *child = head->children[2*i];
            if (child != NULL) {
                vector<bool> occupied_child = occupied_connectivity_borders(child, dim-1);
                for (int j = 0; j < bins; j++) {
                    occupied[j] = occupied[j] | occupied_child[j];
                }
            }
        }
    }

    return occupied;
}

// copies all border and connectivity-borders in restr
// needs an entire different approach where you merge different regions
/*
inscy_node *restrict_tree_rec(inscy_node *head, restriction restr, int lvl) {
    if (head == NULL) { return NULL; }

    inscy_node *copy = new inscy_node({head->descr});
    if (lvl == 1) {
        for (int child_index = 2*restr.from; child_index < 2*restr.to; child_index++) {
            if (head->children[child_index] == NULL) { continue; }
            inscy_node *child = head->children[child_index];
            for (int i = 0; i < 2*bins; i++) {
                if (child->children[i] != NULL) {
                    copy->children[i] = copy_tree(child->children[i]);
                    
                }
            }


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

inscy_node *restrict_tree(inscy_node *head, restriction restr) {
    int lvl = find_dim_lvl(head, {restr.dim, 0, 0});
    
    return restrict_tree_rec(head, restr, lvl);
}
*/


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


/*
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

        // if (restricted_tree->count < minPts) {
            // continue;
        // }

        cout << " - recursing on " << 
            ((restricted_tree == NULL) ? 0 : restricted_tree->count) 
            << " points" << endl;
        
        eDusc_old(restricted_tree, minPts, currDim-1, lvl+1);
    }
}
*/


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
