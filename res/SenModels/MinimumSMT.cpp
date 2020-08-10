/*
 * MinimumSMT.cpp
 *
 *  Created on: Sep 16, 2019
 *      Author: valencif
 */

#include "MinimumSMT.h"

MinimumSMT::MinimumSMT() {
	_modelType ="MinimumTemplate";

}

MinimumSMT::~MinimumSMT() {

}

int MinimumSMT::aggregationFnc(vector<int> vec){
	int min = 100000;

	if (vec.size() == 0){
		return 0;
	}
	for(unsigned int i =0; i < vec.size(); i++){
		if(vec.at(i) < min){
			min = vec.at(i);
		}
	}
	return min;
}

int MinimumSMT::getSensitivity(int inter){


	if(loaded == 0){
		loadTemplate(templateFile );
	}

	inter = inter % 256;

	return templateM[inter];

}
