/*
 * HwSM.cpp
 *
 *  Created on: Sep 13, 2019
 *      Author: valencif
 */

#include "HwSM.h"

HwSM::HwSM() {

	_modelType ="HammingWeight";

}

HwSM::~HwSM() {

}

int HwSM::getSensitivity(int inter){
	int hw, size;

	size = sizeof(inter);
	hw = inter & 1;
	while (inter >>= 1){
		hw += (inter & 1);
	}
	return hw;

}

