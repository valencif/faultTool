/*
 * MaximumSMT.cpp
 *
 *  Created on: Sep 16, 2019
 *      Author: valencif
 */

#include "MaximumSMT.h"

MaximumSMT::MaximumSMT() {
	_modelType ="MaximumTemplate";

}

MaximumSMT::~MaximumSMT() {

}

int MaximumSMT::aggregationFnc(vector<int> vec){
	int max = 0;

	if (vec.size() == 0){
		return 0;
	}
	for(unsigned int i =0; i < vec.size(); i++){
		if(vec.at(i) > max){
			max = vec.at(i);
		}
	}
	return max;
}

int MaximumSMT::getSensitivity(int inter){


	if(loaded == 0){
		loadTemplate(templateFile );
	}

	inter = inter % 256;

	return templateM[inter];

}
