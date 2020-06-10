/*
 * Address.h
 *
 *  Created on: Apr 25, 2020
 *      Author: hongt
 */

#ifndef ADDRESS_H_
#define ADDRESS_H_

#include <string>
using namespace std;
class Address{
    public:
        int _1,_2,_3,_4;
        Address();
        Address(int _1, int _2, int _3, int _4);
        string toString();
};

#endif /* ADDRESS_H_ */
