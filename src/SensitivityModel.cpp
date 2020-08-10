/*
 * ModelsFSA.cpp
 *
 *  Created on: Sep 13, 2019
 *      Author: valencif
 */

#include "SensitivityModel.h"

SensitivityModel::SensitivityModel() {
	templateModel=0; // By default the models are not templates

}

SensitivityModel::~SensitivityModel() {

}

void SensitivityModel::printModelType(){
	cout<<"Sensitivity model: "<< _modelType << endl;

}

string SensitivityModel::getModelType(){
	return _modelType;
}

int SensitivityModel::isTemplate(){
	return templateModel;
}

