#include <omnetpp.h>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>
#include <iostream>

#include "FatTreeGraph.h"
#include "FatTreeRoutingAlgorithm.h"

using namespace omnetpp;
using namespace std;

class Tree : public cSimpleModule{
protected:
    virtual void initialize();
};

Define_Module(Tree);

void Tree::initialize() {

    if (strcmp(getName(), "fat") == 0){
        //Lab05A
        int k = 4;
        FatTreeGraph fatTreeGraph(k);
        FatTreeRoutingAlgorithm* ftra = new FatTreeRoutingAlgorithm(fatTreeGraph, true);

        //Print to Console
        ftra->PrintTables();

        //Lab05B
        vector<int> hosts;
        int numEachPod = k * k / 4 + k;
        for (int p = 0; p < k; p++) {
            int offset = numEachPod * p;
            for (int e = 0; e < k / 2; e++) {
                for (int s = 0; s < k / 2; s++) {
                    int server = offset + e * k / 2 + s;
                    hosts.push_back(server);
                }
            }
        }

        //generate random list of host
        random_shuffle (hosts.begin(), hosts.end());

        //generate random pair host
        vector<pair<int,int>> genRandomPairs;
        int counter = 0;
        int prehost;
        for (auto nexthost : hosts) {
            if (++counter == 2) {
                counter = 0;
                genRandomPairs.push_back(pair<int,int>(prehost, nexthost));
            }
            prehost = nexthost;
        }

        //Print to console
        cout << "List hosts:" << endl;
        for (auto i : hosts) {
            cout << "Host[" << i << "] :" << fatTreeGraph.getAddress(i).toString() << endl;
        }
        cout << endl;
        cout << "Random pair: " << endl;
        for (pair<int, int> p : genRandomPairs) {
            cout << "Host[" << p.first << "] : " << fatTreeGraph.getAddress(p.first).toString();
            cout << " - " << "Host[" << p.second << "] : " << fatTreeGraph.getAddress(p.second).toString() << endl;
        }
        cout << endl;

        //Lab5C
        for (pair<int, int> p : genRandomPairs) {
            cout << "Routing from " <<
                fatTreeGraph.getAddress(p.first).toString()
                << " to "
                << fatTreeGraph.getAddress(p.second).toString()
                << " : ";

            cout << fatTreeGraph.getAddress(p.first).toString() << " -> ";
            for (int node : ftra->path(p.first, p.second).path) {
                cout << fatTreeGraph.getAddress(node).toString() << " -> ";
            }
            cout << fatTreeGraph.getAddress(p.second).toString() << endl;

            cout << endl;
        }
    }

}
