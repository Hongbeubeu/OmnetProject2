/*
 * txc1.cc
 *
 *  Created on: Mar 28, 2020
 *      Author: hongt
 */



#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

/**
 * Derive the Txc1 class from cSimpleModule. In the Tictoc1 network,
 * both the `tic' and `toc' modules are Txc1 objects, created by OMNeT++
 * at the beginning of the simulation.
 */
class Txc1 : public cSimpleModule
{

};

// The module class needs to be registered with OMNeT++
Define_Module(Txc1);
