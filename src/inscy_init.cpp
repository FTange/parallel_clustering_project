#include <iostream>
#include <limits>
#include <cmath>
#include <algorithm>

#include "inscy.h"

using std::vector;
using std::cout;
using std::endl;


void insert_point(inscy_node *head, vector<float> &point, double eps,
        vector<float> &mins, vector<float> &maxs, vector<double> &interval_size,
        int currLvl = 0, bool borderPath = false);
void find_min_max_intervals( std::vector<std::vector<float> > &db,
        vector<float> &maxs, vector<float> &mins, vector<double> &intervals);

inline inscy_node *find_child(inscy_node *head, int bin, bool border);
inline inscy_node *insert_node(inscy_node *head, short dim, short interval, 
        bool isBorderPoint, bool isBorderDim);
inline int find_bin(float min, double interval, float value);

vector<float> maxs;
vector<float> mins;
vector<double> interval_size;

/*
 * insert a row number when we get to the end, 
 * get the vector and treat it like ints using memcpy
 *
 * check that &last_element - &current_element - sizeof(int) is positive
 * if not resize the vector, otherwise memcopy to first_element + count * sizeof(int)
 *
 * inscy_child *p = node.children.data();
 *
 * memcpy(p, &a, sizeof(int));
 *
 * int point;
 * memcpy(&point, p, sizeof(int));
 */
inscy_node *init_scy_tree(std::vector<std::vector<float> > &db, double eps) {

    int dim = db[0].size();
    maxs = db[0];
    mins = db[0];
    interval_size.resize(db[0].size());

    find_min_max_intervals(db, maxs, mins, interval_size);

    inscy_node *root = new inscy_node();

    /*
    cout << "maxs" << endl;
    for (float i : maxs) {
        cout << i << " ";
    }
    cout << endl << "mins" << endl;
    for (float i : mins) {
        cout << i << " ";
    }
    cout << endl << "interval sizes" << endl;
    for (float i : interval_size) {
        cout << i << " ";
    }
    cout << endl;
    */

    int bin;
    for (int i = 0, j; i < db.size(); i++) {
        /*
        if (i == 6) {
            int b = find_bin(mins[0], interval_size[0], db[i][0]);
            cout << "bin is " << b << endl;
        }
        */
        insert_point(root, db[i], eps, mins, maxs, interval_size);
    }

    // set count for root
    for (int i = 0; i < bins * 2; i += 2) {
        inscy_node *ptr = root->children[i];
        if (ptr != NULL) {
            root->count += ptr->count;
        }
    }

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
    for ( ; j <= point.size(); j++) {
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

        head = insert_node(head, j+1, bin, borderPath, isBorderDim);
    }
}

inline inscy_node *insert_node(inscy_node *head, short dim, short interval, 
        bool isBorderPoint, bool isBorderDim) {

    bool border = isBorderPoint & isBorderDim;
    descriptor descr = {dim, interval, border};
    inscy_node *child = new inscy_node({descr, !isBorderPoint? 1 : 0});

    head->children[2*interval + border] = child;
    return child;
}

inline inscy_node *find_child(inscy_node *head, int bin, bool border) {
    return head->children[2*bin+border];
}

// calculates a value between 0 and interval for each value `value`
// (value - min) / ((max - min - min) / intervals)
inline int find_bin(float min, double interval, float value) {
    /*
    if (fabs(value - 0.4881189) < 0.1) {
        cout << value << " " << fabs(value - 0.4881189) << endl;
    }
    */
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

void delete_scy_tree(inscy_node *head) {
    if (head == NULL) { return; }
    for (int i = 0; i < 2*bins; i++) {
        delete_scy_tree(head->children[i]);
    }
    delete head;
}

bool isclose(double a, double b, double eps = 0.000001) {
    return fabs(a - b) < eps;
}

/*
 * Assumes at least one point in db and that all have same number of dimensions
 */
vector<vector<float> > get_points(vector<vector<float> > &db, 
        vector<restriction> &restrictions) {
    vector<vector<float> > points;
    int d = db[0].size();
    for (int i = 0; i < db.size(); i++) {
        int j = 0, k = 0;
        while (k < restrictions.size()) {
            restriction next_res = restrictions[k];
            // find net restricted dimension
            while (j < next_res.dim-1) { // restrictions are 1-indexed
                j++;
            }
            // is i in the restricted section?
            double lowerbound = mins[j] + next_res.from * interval_size[j];
            double upperbound = mins[j] + next_res.to * interval_size[j];
            // if upperbound is close to max[j] set it to the max of the two, due to
            // float imprecision
            upperbound = (isclose(upperbound, maxs[j])) ? fmax(upperbound, maxs[j])
                                                        : upperbound;
            if (db[i][j] < lowerbound || upperbound < db[i][j]) {
                break;
            }
            k++;
        }
        // i satisfies all the k restrictions
        if (k == restrictions.size()) {
            // add db[i] to points
            points.push_back(db[i]);
        }
    }

    return points;
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
    for (int i = 0; i < 2*bins; i += 2) {
        inscy_node *child = head->children[i];
        if (child != NULL) {
            points += pointsOnLevelK(child, currLvl+1, k);
        }
    }
    return points;
}

bool borderCountIsZero(inscy_node *head, int lvl = 0) {
    bool borderZero = true;
    if (head->descr.border && head->count != 0) {
        cout << "border point at lvl " << lvl << " was " << head->count << endl;
        return false;
    }
    for (int i = 0; i < 2*bins; i++) {
        inscy_node *child = head->children[i];
        if (child != NULL) {
            borderZero = borderZero & borderCountIsZero(child, lvl+1);
        }
    }
    return borderZero;
}

bool dimPathLength(inscy_node *head, int dim, int lvl = 0) {
    bool noChildren = true;
    for (int i = 0; i < 2*bins; i+=2) {
        if (head->children[i] != NULL) {
            noChildren = false;
            break;
        }
    }

    if (lvl == dim || noChildren) {
        if ( !(lvl == dim & noChildren) ) {
            cout << "we are on lvl " << lvl << " with all children null " <<
               " dim: " << head->descr.dim << 
               " interval: " << head->descr.interval <<
              " border: " << head->descr.border << endl;
        }
        return lvl == dim & noChildren;
    }
    bool dPathLength = true;
    for (int i = 0; i < 2*bins; i+=2) {
        inscy_node *child = head->children[i];
        if (child != NULL) {
            dPathLength = dPathLength & dimPathLength(child, dim, lvl+1);
        }
    }
    return dPathLength;
}

bool dimSameAsLevel(inscy_node *head, int lvl = 0) {
    bool lvlCorrect = true;
    if (lvl != 0) {
        lvlCorrect = head->descr.dim == lvl;
        if (lvlCorrect == false) {
            cout << "on level " << lvl << " encountered dimension " << 
                head->descr.dim << " border is " << head->descr.border << endl;
        }
    }
    for (int i = 0; i < 2*bins; i++) {
        inscy_node *child = head->children[i];
        if (child != NULL) {
            // lvlCorrect = lvlCorrect & (child->descr.dim == lvl);
            lvlCorrect = lvlCorrect & dimSameAsLevel(child, lvl+1);
        }
    }
    return lvlCorrect;
}

// doesn't check whether there is a point that is a border point but
// no non-border sibling exists - then we also need to know if we are
// in a border path.
bool properChildren(inscy_node *head) {
    for (int i = 0; i < 2*bins; i += 2) {
        if (head->children[i] == NULL &&
            head->children[i+1] != NULL) {
            cout << "border exists without non-border sibling in dim " <<
                head->descr.dim << " and interval " << head->descr.dim << endl;
            return false;
        }
    }
    bool childrenCorrect = true;
    for (int i = 0; i < 2*bins; i += 2) {
        if (head->children[i] != NULL) {
            childrenCorrect &= properChildren(head->children[i]);
        }
    }
    return true;
}


// check that each level contains the correct count
// check that all counts in subtrees of border points are 0
// all paths has the same length d
bool check_inscy_tree(inscy_node *root, int dim, int numPoints) {
    cout << "number of points is " << numPoints << endl;
    bool valid_tree = true;
    for (int i = 1; i <= dim; i++) {
        int points = pointsOnLevelK(root, 0, i);
        cout << "points on level " << i << " is " << points << endl;
        if (points != numPoints) { valid_tree = false; }
    }
    cout << "is tree valid " << valid_tree << endl;

    valid_tree = valid_tree & borderCountIsZero(root);
    cout << "is tree valid " << valid_tree << endl;
    valid_tree = valid_tree & dimPathLength(root, dim);
    cout << "is tree valid " << valid_tree << endl;
    valid_tree = valid_tree & dimSameAsLevel(root);
    cout << "is tree valid " << valid_tree << endl;

    // check that at most 2d nodes at any level and every combination only occurs once
    valid_tree = valid_tree & properChildren(root);
    cout << "is tree valid " << valid_tree << endl;

    return valid_tree;
}
