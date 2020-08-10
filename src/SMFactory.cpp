/*
 * SMFactory.cpp
 *
 *  Created on: Sep 13, 2019
 *      Author: valencif
 */

#include "SMFactory.h"

SMFactory::SMFactory() {


}

SMFactory::~SMFactory() {
	for(int i=0; i < models.size(); i++){
		delete models.at(i);
	}

}

int SMFactory::loadModels(string path){

	vector<string> libs;
	string ext;
	FFManager &folderfiles = FFManager::instance();
	int counter = 0;
	void * handleLib;
	createSM createfnc;

	SensitivityModel *modelCurr;



	libs = folderfiles.getfiles(path);

	for(unsigned int i=0; i < libs.size(); i++){
		ext = libs.at(i).substr (libs.at(i).length() - 3 );
		if(ext == string(".so")){

			// Load library
			handleLib = dlopen( (path+libs.at(i)).c_str(), RTLD_NOW |RTLD_GLOBAL );
			if ( !handleLib ) {
				display.print(VIEW_ERROR) << "Error loading library sensitivity model "<<endl;
				cout<<dlerror()<<endl;
				return 0;
			}
			createfnc = (createSM) dlsym(handleLib, "createSensitivityModel");
			if(!createfnc){
				display.print(VIEW_ERROR)<<"Error loading model"<<endl;
				return 0;
			}
			modelCurr = createfnc();

			modelCurr->printModelType();

			models.push_back(modelCurr);
			factory[modelCurr->getModelType().c_str()]=createfnc;
			counter++;

		}
	}

	// Check if it is needed to close the libraries

	//

	return counter;
}


SensitivityModel* SMFactory::createModel(string name){
	return factory[name.c_str()]();
}
vector<SensitivityModel*> SMFactory::getAllModelsPtr(){
	return models;
}
