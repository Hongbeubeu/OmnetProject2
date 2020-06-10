/*
 * Graph.cc
 *
 *  Created on: Apr 25, 2020
 *      Author: hongt
 */

#include "Graph.h"

using namespace std;

void Graph::validateVertex(int v){
    if(v < 0 || v >= V) throw ("vertext is invalided");
}

void Graph::addEdge(int v, int w){
    validateVertex(v);
    validateVertex(w);
    E++;
    adj[v].push_back(w);
    adj[w].push_back(v);
}

int Graph::degree(int u){
    return adj[u].size();
}

vector<int> Graph::hosts(){
    return vector<int>();
}

vector<int> Graph::switches(){
    return vector<int>();
}
