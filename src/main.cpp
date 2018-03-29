#include "main.h"

#include <chrono>
#include <bitset>
#include <fstream>

#include <stdio.h>
#include <memory.h>

using namespace std;

int main() {
	// vector<vector<float> > data = read_csv("x.csv");
	// vector<vector<float> > data = read_csv("../../datasets/glass/glass.data");
	vector<vector<float> > data = read_csv("10pts_3dim.csv");
    inscy_algorithm(data, 0.05, 5);

    /*
    int a = 17;

    char buffer[sizeof(int)];
    memcopy(buffer, p, sizeof(int));
    */

    /*
    auto start = chrono::high_resolution_clock::now();
    subspace_clustering(data, data[0].size(), 0.1, 5, 4);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    cout << "This took " << elapsed.count() << endl;
    */


	/*
	vector<int> clustering = dbscan(data, 0.5, 5);

	for (auto x : clustering) {
		cout << x << endl;
	}
	cout << endl;
	*/
	/*
	vector<float> x(34);
	vector<float> y(34);

	for (int i = 0; i < x.size(); i++) {
		x[i] = 1;
		y[i] = 0;
	}
	unsigned int blah = 0 - 1;
	cout << blah << endl;

	unsigned int subspace[2] = {blah, 0};

	cout << euclid_dist_subspace(subspace, x, y) << endl;
	*/

	// black magic to create a new dist function with subspaces embedded
	/*
	unsigned int subspace = 3;
	auto dist_with_subspace = [&] (vector<float> p1, vector<float> p2) { return euclid_dist_subspace(&subspace, p1, p2); };

	// auto dist_with_subspace = bind(euclid_dist_subspace, placeholders::_1, subspace);
	vector<int> clustering = dbscan(data, 0.5, 5, dist_with_subspace);
	for (auto x : clustering) {
		cout << x << endl;
	}
	*/
}

// TODO
// change data arguments to const references
//
// change ints and longs to unsigned longs in subspace clustering
//
// Find a way to calculate the number of ones in a number's binary representation, and only
// create a subspace task if you have clustered all the spaces of one less dimension
//
// find out how to allocate vectors on the heap
//
// Find a mapping from the number of subspaces to a unique integer in the same range such that subspaces of 1 dimension are indexed from 1..32 instead of 2^0..2^32 which would improve cache performance greatly

void meassure_subclue(vector<vector<float> > &db, string filename) {
    int repeats = 5;
    double time = 0;
    int thread_num[3] = {1, 2, 4};

    ofstream file;
    // file.open("results.csv");
    file.open(filename);

    file << "eps, minPts, threads, subsets, time" << endl;

    int subspaces = 0;
    for (double eps = 0.01; eps < 0.31 ; eps += 0.01) {
        cout << eps << endl;
        for (int minPts = 4; minPts <= 8; minPts++) {
            for (int threads = 0; threads < 3; threads++) {
                time = 0;
                for (int i = 0; i < repeats; i++) {
                    auto start = chrono::high_resolution_clock::now();
                    subspaces = subspace_clustering(db, db[0].size(), eps, minPts, 
                            thread_num[threads]);
                    auto end = chrono::high_resolution_clock::now();

                    chrono::duration<double> elapsed = end - start;

                    time += elapsed.count() / repeats;
                }
                file << eps << "," << minPts << "," << thread_num[threads] << "," << 
                    subspaces << "," << time << endl;
            }
        }
    }
    file.close();
}
