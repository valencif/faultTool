/*
 * NonZeroSM.cpp
 *
 *  Created on: Sep 13, 2019
 *      Author: valencif
 */

#include "NonZeroSM.h"

NonZeroSM::NonZeroSM() {
	_modelType ="NonZero";

}

NonZeroSM::~NonZeroSM() {

}

int NonZeroSM::getSensitivity(int inter){

	if(inter == 0){
		return 0;
	}else{
		return 1;
	}
}

