#ifndef __MAIN_H_INCLUDED__
#define __MAIN_H_INCLUDED__

#include <iostream>
#include <vector>

#include "util.h"
#include "clustering.h"
#include "subspace.h"
#include "inscy.h"

int main();

void meassure_subclue(std::vector<std::vector<float> > &db, std::string filename);

#endif
