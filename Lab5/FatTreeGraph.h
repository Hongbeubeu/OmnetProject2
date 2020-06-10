/*
 * FatTreeGraph.h
 *
 *  Created on: Apr 25, 2020
 *      Author: hongt
 */

#ifndef FATTREEGRAPH_H_
#define FATTREEGRAPH_H_


#include <stdlib.h>
#include <vector>
#include <list>
#include "Address.h"
#include "Graph.h"
#include <iostream>

using namespace std;

class FatTreeGraph : public Graph {
    public:
        static const int CORE = 1;
        static const int AGG = 2;
        static const int EDGE =3;

        vector<int> switches();
        vector<int> hosts();

        Address* address;
        FatTreeGraph();
        FatTreeGraph(int k);

        int getK();

        Address getAddress(int u);
        void printAddress();
        bool isHostVertex(int u);
        bool isSwitchVertex(int u);
        int switchType(int u);

    private:
        int numServers;
        int numPodSwitches;
        int numCores;
        int k;

        void buildAddress();
};

#endif /* FATTREEGRAPH_H_ */
