/*
 * Address.cc
 *
 *  Created on: Apr 25, 2020
 *      Author: hongt
 */

#include "Address.h"
using namespace std;

Address::Address(){

}

Address::Address(int _1, int _2, int _3, int _4){
    this->_1 = _1;
    this->_2 = _2;
    this->_3 = _3;
    this->_4 = _4;
}

string Address::toString(){
    return to_string(_1) + "." + to_string(_2) + "." + to_string(_3) + "." + to_string(_4);
}
