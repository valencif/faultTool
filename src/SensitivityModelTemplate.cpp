/*
 * SensitivityModelTemplate.cpp
 *
 *  Created on: Sep 16, 2019
 *      Author: valencif
 */

#include "SensitivityModelTemplate.h"

SensitivityModelTemplate::SensitivityModelTemplate() {
	templateModel=1;  // Set type of the model as template
	templateM = NULL;
	maxIntermediate = 0;
	loaded = 0;

}

SensitivityModelTemplate::~SensitivityModelTemplate() {

}


void SensitivityModelTemplate::setMaxIntermediate(int value){
	maxIntermediate = value;
}


void SensitivityModelTemplate::generateTemplate(string stimuliTFN, int posIntermeadiate, string sensitivityFN, int phaseFSA){

	// If file exist load
	string templatefileFN;
	ofstream templateF;

	FFManager &folderfiles = FFManager::instance();
	ifstream stimuli, sensitivity;
	int sen, inter;
	vector<vector<int> > senMatrix(maxIntermediate);

	if(phaseFSA == 0){
		templatefileFN = folderfiles.getPaths(10,2)+"/SYN/"+_modelType+".txt";
	}else{
		templatefileFN = folderfiles.getPaths(10,2)+"/PAR/"+_modelType+".txt";
	}
	templateFile = templatefileFN;

	// ToDo: Make this update dynamix
	//if(folderfiles.fileexists(templatefileFN) == 1){
	//	display.print(VIEW_WARNING)<<"File for "<< _modelType <<" already exist and will not be updated"<<endl;
	//	return;
	//}

	stimuli.open(stimuliTFN.c_str(), ios::in);

	sensitivity.open(sensitivityFN.c_str(), ios::in);

	while( !stimuli.eof() &&  !sensitivity.eof() ){
		for (int i=1;i < posIntermeadiate;i++){ // Read until finding the necessary input
			stimuli>>inter;
		}
		stimuli >> inter;
		sensitivity >> sen;

		if (stimuli.good()  && sensitivity.good()){
			senMatrix.at(inter).push_back(sen);
			//cout<< "out: " << inter << " sen: "<< sen << " "<< stimuli.good() << " "<< sensitivity.good()<<endl;
		}

	}

	stimuli.close();
	sensitivity.close();

	templateM = new int[maxIntermediate];

	templateF.open(templatefileFN.c_str(), ios::out);
	for ( int i=0; i < maxIntermediate ; i++){
		templateM[i]= aggregationFnc(senMatrix.at(i));
		templateF << templateM[i] <<endl;
		//cout <<  tempate[i] <<endl;
	}

	templateF.close(); // ToDo: Allow to choose to keep template loaded or not



	unloadTemplate(); // Liberate space while doing other things

}

void SensitivityModelTemplate::loadTemplate(string filename){

	int x;
	ifstream templateF;

	unloadTemplate();
	templateF.open(filename.c_str(), ios::in);

	templateM = new int[maxIntermediate];

	for ( int i=0; i < maxIntermediate ; i++){
		templateF >> x;
		templateM[i] = x;
	}


	templateF.close();
	loaded = 1;

	templateFile = filename;



}

void SensitivityModelTemplate::unloadTemplate(){

	if(templateM != NULL){
		delete[] templateM;
		templateM = NULL;
	}
	loaded=0;
}
