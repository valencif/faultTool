/*
 * MaximumSMT.cpp
 *
 *  Created on: Sep 16, 2019
 *      Author: valencif
 */

#include "MeanSMT.h"

MeanSMT::MeanSMT() {
	_modelType ="MeanTemplate";

}

MeanSMT::~MeanSMT() {

}

int MeanSMT::aggregationFnc(vector<int> vec){
	int acc = 0;

	if (vec.size() == 0){
		return 0;
	}
	for(unsigned int i =0; i < vec.size(); i++){
	    acc += vec.at(i);
		
	}
	acc = acc/vec.size();
	return acc;
}

int MeanSMT::getSensitivity(int inter){


	if(loaded == 0){
		loadTemplate(templateFile );
	}

	inter = inter % 256;

	return templateM[inter];

}
