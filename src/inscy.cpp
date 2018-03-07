#include <iostream>
#include <limits>
#include <cmath>

#include "inscy.h"

#define bins 10

using std::vector;
using std::cout;
using std::endl;


void insert_point(inscy_node *head, vector<float> &point, double eps,
        vector<float> &mins, vector<float> &maxs, vector<double> &interval_size,
        int currLvl = 0, bool borderPath = false);
void find_min_max_intervals( std::vector<std::vector<float> > &db,
        vector<float> &maxs, vector<float> &mins, vector<double> &intervals);
bool check_inscy_tree(inscy_node *root, int dim, int numPoints);

inline inscy_node *find_child(inscy_node *head, int bin, bool border);
inline inscy_node *insert_node(inscy_node *head, short dim, short interval, 
        bool isBorderPoint, bool isBorderDim);
inline int find_bin(float min, double interval, float value);


inscy_node *init_scy_tree(std::vector<std::vector<float> > &db, double eps) {

    int dim = db[0].size();
    vector<float> maxs = db[0];
    vector<float> mins = db[0];
    vector<double> interval_size(db[0].size());

    find_min_max_intervals(db, maxs, mins, interval_size);

    inscy_node *root = new inscy_node();
    inscy_node *head;
    // root->children = new vector<inscy_child>();

    int bin;
    for (int i = 0, j; i < db.size(); i++) {
        // head = root;
        insert_point(root, db[i], eps, mins, maxs, interval_size);
    }
    /*
    head = root->children[0].child;
    while (head != NULL) {
        cout << "head points to node in dim " << head->descr.dim <<
           " in interval " << head->descr.interval <<
           " border " << head->descr.border << " and count " << head->count << endl;
        head = head->children[0].child;

    }
    */
    int child_count = 0;
    for (inscy_child child : root->children) {
        inscy_node *ptr = child.child;
        cout << "count of interval " << ptr->descr.interval << " in dim " << 
            ptr->descr.dim << " and is border " << ptr->descr.border << " is " << ptr->count << endl;
        child_count += ptr->count;
    }
    cout << "total count is " << child_count << endl;
    cout << "database size is " << db.size() << endl;

    check_inscy_tree(root, dim, db.size());

    return root;
}


// change such that the first time we encounter a dimension where the point is a border 
// we insert both a new node with 0 and 1 as border, but next time we do it in the border case
// we just continue our border point
// Also in the border path we should insert the current dimension only as being a border if we 
// are also a border point in the current dimension
void insert_point(inscy_node *head, vector<float> &point, double eps,
        vector<float> &mins, vector<float> &maxs, vector<double> &interval_size,
        int currLvl, bool borderPath) {
    int j = currLvl, bin;

    // find how much already exists in the tree
    for ( ; j < point.size(); j++) {
        bin = find_bin(mins[j], interval_size[j], point[j]);
        bool isBorderDim = std::fmod((point[j] - mins[j]), interval_size[j]) < eps;
        // even though this is a border dimension we are only looking for border node if 
        // we are on a border path
        inscy_node *child = find_child(head, bin, borderPath & isBorderDim);

        // if point is a border point in this dimension but not the previous, 
        // insert it as border below
        if (!borderPath && isBorderDim) {
            // insert border path if it does not already exist
            insert_point(head, point, eps, mins, maxs, interval_size, j, true);
        }

        // no child exists, create rest of path as new nodes
        if (child == NULL) { break; }

        // only keep count in non-border points
        if (!borderPath) { child->count++; }
        head = child;
    }
    // create nodes from j to d
    for ( ; j < point.size(); j++) {
        bin = find_bin(mins[j], interval_size[j], point[j]);
        bool isBorderDim = std::fmod((point[j] - mins[j]), interval_size[j]) < eps;

        head = insert_node(head, j, bin, borderPath, isBorderDim);
    }
}

inline inscy_node *insert_node(inscy_node *head, short dim, short interval, 
        bool isBorderPoint, bool isBorderDim) {
    descriptor descr = {dim, interval, isBorderPoint & isBorderDim};
    inscy_node *child = new inscy_node({descr, !isBorderPoint? 1 : 0});
    /*
    child->count = !isBorderPoint? 1 : 0; // don't count borders
    child->descr.dim = dim;
    child->descr.interval = interval;
    child->descr.border = isBorderPoint;
    */

    inscy_child child_ptr = {descr, child};

    head->children.push_back(child_ptr);

    return child;
}

inline inscy_node *find_child(inscy_node *head, int bin, bool border) {
    for (inscy_child child : head->children) {
        if (child.descr.interval == bin && child.descr.border == border) {
            return child.child;
        }
    }
    return NULL;
}

// calculates a value between 0 and interval for each value `value`
// (value - min) / ((max - min - min) / intervals)
inline int find_bin(float min, double interval, float value) {
    int bin = (int) ((value - min) / interval);
    if (bin == bins) bin--;
    return bin;
}

/*
 * calculates min max and the size of the intervals for each dimension
 * when using `bins` intervals
 */
void find_min_max_intervals(std::vector<std::vector<float> > &db,
        vector<float> &maxs, vector<float> &mins, vector<double> &intervals) {
    for (int i = 1; i < db.size(); i++) {
        for (int j = 0; j < db[0].size(); j++) {
            maxs[j] = maxs[j] < db[i][j] ? db[i][j] : maxs[j];
            mins[j] = mins[j] > db[i][j] ? db[i][j] : mins[j];
        }
    }
    for (int i = 0; i < db[0].size(); i++) {
        intervals[i] = ((maxs[i] - mins[i]) / bins);
    }
}



/*
// doesn't work
std::vector<std::vector<float> > normalize_data(
        std::vector<std::vector<float> > db) {

    for (int i = 0, j; i < db[0].size(); i++) {
        float min = FLT_MAX, max = FLT_MIN;
        for (int j = 0; j < db.size(); j++) {
            min = db[j][i] < min ? db[j][i] : min;
            max = db[j][i] > max ? db[j][i] : max;
        }
        for (int j = 0; j < db.size(); j++) {
            float newValue = (db[j][i] - min) / (max - min);
            db[j][i] = (db[j][i] - min) / (max - min);
        }
    }

    return db;
}
*/

/*
 * Functions to validate properties of a correct scy-tree
 */

int pointsOnLevelK(inscy_node *head, int currLvl, int k) {
    if (currLvl == k) {
        return head->count;
    }
    int points = 0;
    for (inscy_child child : head->children) {
        points += pointsOnLevelK(child.child, currLvl+1, k);
    }
    return points;
}

bool borderCountIsZero(inscy_node *head, int lvl = 0) {
    bool borderZero = true;
    if (head->descr.border && head->count != 0) {
        cout << "border point at lvl " << lvl << " was " << head->count << endl;
        return false;
    }
    for (inscy_child child : head->children) {
        if (child.descr.border) {
            borderZero = borderZero & borderCountIsZero(child.child, lvl+1);
        }
    }
    return borderZero;
}

bool dimPathLength(inscy_node *head, int dim, int lvl = 0) {
    if (lvl == dim || head->children.size() == 0) {
        if ( !(lvl == dim & head->children.size()) ) {
            cout << "we are on lvl " << lvl << " children is " << head->children.size() << 
               " dim: " << head->descr.dim << 
               " interval: " << head->descr.interval <<
              " border: " << head->descr.border << endl;
        }
        return lvl == dim & head->children.size() == 0;
    }
    bool dPathLength = true;
    for (inscy_child child : head->children) {
        if (child.descr.border) {
            dPathLength = dPathLength & dimPathLength(child.child, lvl+1, dim);
        }
    }
    return dPathLength;
}

bool dimSameAsLevel(inscy_node *head, int lvl = 0) {
    bool lvlCorrect = true;
    if (lvl != 0) {
        lvlCorrect = head->descr.dim == lvl-1;
        if (lvlCorrect == false) {
            cout << "on level " << lvl << " encountered dimension " << 
                head->descr.dim << " border is " << head->descr.border << endl;
        }
    }
    for (inscy_child child : head->children) {
        lvlCorrect = lvlCorrect & (child.descr.dim == lvl);
        dimSameAsLevel(child.child, lvl+1);
    }
    return lvlCorrect;
}

// doesn't check whether there is a point that is a border point but
// no non-border sibling exists - then we also need to know if we are
// in a border path.
bool properChildren(inscy_node *head, int interval, int dim) {
    if (head->children.size() > interval * 2) {
        cout << "Too many children at node in dim " << head->descr.dim << 
            " interval " << head->descr.interval << " and border " <<
            head->descr.border << endl;
        return false;
    }
    for (int i = 0; i < head->children.size(); i++) {
        inscy_child child = head->children[i];
        for (int j = 0; j < head->children.size(); j++) {
            if (i == j) { continue; }
            inscy_child otherChild = head->children[j];
            if (child.descr.interval == otherChild.descr.interval &&
                child.descr.border   == otherChild.descr.border) {
                cout << "found duplicates in dimension" << child.descr.dim
                    << " interval " << child.descr.interval << " border: "
                    << child.descr.border << endl;
                return false;
            }
        }
        properChildren(head->children[i].child, interval, dim);
    }
    return true;
}

// check that each level contains the correct count
// check that all counts in subtrees of border points are 0
// all paths has the same length d
bool check_inscy_tree(inscy_node *root, int dim, int numPoints) {
    bool valid_tree = true;
    for (int i = 1; i <= dim; i++) {
        int points = pointsOnLevelK(root, 0, i);
        cout << "points on level " << i << " is " << points << endl;
        if (points != numPoints) { valid_tree = false; }
    }
    cout << "is tree valid " << valid_tree << endl;

    valid_tree = valid_tree & borderCountIsZero(root);
    cout << "is tree valid " << valid_tree << endl;
    // valid_tree = valid_tree & dimPathLength(root, dim);
    cout << "is tree valid " << valid_tree << endl;
    valid_tree = valid_tree & dimSameAsLevel(root);
    cout << "is tree valid " << valid_tree << endl;

    // check that at most 2d nodes at any level and every combination only occurs once
    valid_tree = valid_tree & properChildren(root, bins, dim);
    cout << "is tree valid " << valid_tree << endl;

    return valid_tree;
}
