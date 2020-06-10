/*
 * FatTreeRoutingAlgorithm.cc
 *
 *  Created on: Apr 25, 2020
 *      Author: hongt
 */

#include "FatTreeRoutingAlgorithm.h"

FatTreeRoutingAlgorithm::FatTreeRoutingAlgorithm(FatTreeGraph G, bool precomputed) {
    this->G = G;
    buildTables();
    if (precomputed){
        vector<int> hosts = G.hosts();
        for (int i = 0; i < hosts.size() - 1; i++){
            for (int j = i + 1; j < hosts.size(); j++){
                int source = hosts[i];
                int destination = hosts[j];
                path(source, destination);
            }
        }
    }
}

void FatTreeRoutingAlgorithm::buildTables(){
    // TODO - build prefix - suffix routing table
    int k = G.getK();
    int numEachPod = k * k / 4 + k;

    //edge switches
    for (int p = 0; p < k; p++){
        int offset = numEachPod * p;;
        for (int e = 0; e < k / 2; e++){
            int edgeSwitch = offset + k * k / 4 + e;
            //create suffix table
            map<int, int> suffixTable;
            for(int suffix = 2; suffix <= k/2 + 1; suffix++){
                int agg = offset + k * k / 4 + (e + suffix -2) % (k / 2) + (k / 2);
                suffixTable.insert(pair<int, int>(suffix, agg));
            }
            suffixTables.insert(pair<int,map<int, int>>(edgeSwitch, suffixTable));
        }

    }

    //agg switches
    for (int p = 0; p < k; p++){
        int offset = numEachPod * p;
        for (int a = 0; a < k / 2; a++){
            int aggSwitch = offset + k * k / 4 + k / 2 + a;

            //create suffix table
            map<int, int> suffixTable;
            for (int suffix = 2; suffix <= k / 2 + 1; suffix++){
               int core = a * k / 2 + (suffix + a - 2) % (k /  2) + numEachPod * k;
               suffixTable.insert(pair<int, int>(suffix, core));
            }
            //inject to the behavior
            suffixTables.insert(pair<int, map<int, int>>(aggSwitch, suffixTable));

            //create prefix table
            map<int, int> prefixTable;
            for (int e = 0; e < k / 2; e++){
                int edgeSwitch = offset + k * k / 4 + e;
                int temp = (10 << 16) | (p << 8) | e;
                prefixTable.insert(pair<int, int>(temp, edgeSwitch));
            }
            prefixTables.insert(pair<int, map<int, int>>(aggSwitch, prefixTable));
        }
    }

    //core switches
    for (int c = 0; c < k * k / 4; c++){
        int core = k * k * k / 4 + k * k + c;

        // build core prefix
        map<int, int> corePrefixTable;
        for (int p = 0; p < k; p++){
            int offset = numEachPod * p;
            int agg = (c / (k / 2)) + k / 2 + k * k / 4 + offset;
            int temp = (10 << 8) | p;
            corePrefixTable.insert(pair<int, int>(temp, agg));
        }
        corePrefixTables.insert(pair<int, map<int, int>>(core, corePrefixTable));
    }
}

void FatTreeRoutingAlgorithm::PrintTables()
{
    // print tables
    cout << "suffixTables" << endl;
    for (map<int, map<int, int>>::iterator it0 = suffixTables.begin(); it0 != suffixTables.end(); ++it0) {
        cout << "suffixTable: " << G.getAddress(it0->first).toString() << ": " << endl;
        for (map<int, int>::iterator it1 = it0->second.begin(); it1 != it0->second.end(); ++it1) {
           cout << "Suffix : " << "0.0.0."<< it1->first << "/8 - " << " Output Port: " << it1->first << " - Address Out: " << G.getAddress(it1->second).toString() << endl;
        }
    }
    cout << endl;;

    cout << "prefixTables" << endl;
    for (map<int, map<int, int>>::iterator it0 = prefixTables.begin(); it0 != prefixTables.end(); ++it0) {
        cout << "prefixTable: " << G.getAddress(it0->first).toString() << ": " << endl;
        for (map<int, int>::iterator it1 = it0->second.begin(); it1 != it0->second.end(); ++it1) {
            int ip = it1->first;
            int out = it1->second;
            int ip_0 = (ip >> 16);
            int ip_1 = ((ip << 16) >> 24);
            int ip_2 = ((ip << 24) >> 24);
            cout << "Prefix : " << ip_0 << "." << ip_1 << "." << ip_2 << ".0/24 - " << " Output Port: " << ip_2 <<" - Address Out: "<< G.getAddress(out).toString() << endl;
        }
        cout << endl;
    }

    cout << "corePrefixTables" << endl;
    for (map<int, map<int, int>>::iterator it0 = corePrefixTables.begin(); it0 != corePrefixTables.end(); ++it0) {
        cout << "corePrefixTable: " << G.getAddress(it0->first).toString() << ": " << endl;
        for (map<int, int>::iterator it1 = it0->second.begin(); it1 != it0->second.end(); ++it1) {
            int ip = it1->first;
            int out = it1->second;
            int ip_0 = (ip >> 8);
            int ip_1 = ((ip << 24) >> 24);
            cout << ip_0 << "." << ip_1 << ".0.0/16 - " << " Output Port: " << ip_1 << " - Address Out: " << G.getAddress(out).toString() << endl;
        }
        cout << endl;
    }
    cout << endl;;
}

int FatTreeRoutingAlgorithm::next(int source, int current, int destination) {
    if (G.isHostVertex(current)) {
        return G.adj[current][0];
    }
    else if (std::find(G.adj[current].begin(), G.adj[current].end(), destination) != G.adj[current].end()) {
        return destination;
    }
    else {
        int type = G.switchType(current);
        if (type == FatTreeGraph::CORE) {

            Address address = G.getAddress(destination);
            int prefix = (address._1 << 8) | address._2;
            map<int, int> corePrefixTable = corePrefixTables[current];
            return corePrefixTable[prefix];
        }
        else if (type == FatTreeGraph::AGG) {

            Address address = G.getAddress(destination);

            int prefix = (address._1 << 16) | (address._2 << 8) | address._3;
            int suffix = address._4;

            map<int, int> prefixTable = prefixTables[current];
            map<int, int> suffixTable = suffixTables[current];

            if (prefixTable.count(prefix)) {
                return prefixTable[prefix];
            }
            else {
                return suffixTable[suffix];
            }
        }
        else { // Edge switch

            Address address = G.getAddress(destination);
            int suffix = address._4;

            map<int, int> suffixTable = suffixTables[current];
            return suffixTable[suffix];
        }
    }
}

RoutingPath FatTreeRoutingAlgorithm::path(int source, int destination) {
    if (precomputedPaths.count(pair<int, int>(source, destination))) {
        return precomputedPaths[pair<int, int>(source, destination)];
    }
    else {
        RoutingPath rp;
        int current = source;
        while (current != destination) {
            if (current != source) {
                rp.path.push_back(current);
            }
            current = next(source, current, destination);
        }
        precomputedPaths.insert(pair<pair<int, int>, RoutingPath>(pair<int, int>(source, destination), rp));
        return rp;
    }
}
