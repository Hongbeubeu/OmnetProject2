/*
 * FatTreeRoutingAlgorithm.h
 *
 *  Created on: Apr 25, 2020
 *      Author: hongt
 */

#ifndef FATTREEROUTINGALGORITHM_H_
#define FATTREEROUTINGALGORITHM_H_

#include <map>
#include <list>
#include <algorithm>
#include <iostream>
#include <utility>
#include <random>
#include <chrono>
#include <string>

#include "RoutingPath.h"
#include "FatTreeGraph.h"

#include "RoutingPath.h"

using namespace std;

class FatTreeRoutingAlgorithm
{
private:
    FatTreeGraph G;
    map<pair<int, int>, RoutingPath> precomputedPaths;
    map<int, map<int,int>> suffixTables;
    map<int, map<int,int>> prefixTables;
    map<int, map<int,int>> corePrefixTables;
public:
    FatTreeRoutingAlgorithm(FatTreeGraph G, bool precomputed);
    void buildTables();
    void PrintTables();
    int next(int source, int current, int destination);
    RoutingPath path(int source, int destination);
};

#endif /* FATTREEROUTINGALGORITHM_H_ */
