/*
 * Model.cpp
 *
 *  Created on: Feb 22, 2019
 *      Author: valencif
 */

#include "ModelSW.h"




ModelSW::ModelSW() {
	numInputs = 0;
	numTracesFSA = 0;
	numTracesFSAT = 0;

}

ModelSW::~ModelSW() {

}



void ModelSW::setNumTraces(int FSA, int FSAT){
	numTracesFSA=FSA;
	numTracesFSAT=FSAT;
}
int ModelSW::getOuput(){
	return 0;
}

int ModelSW::createStimuliFSA(string &filename){

	cout<<"Implement function createStimuliFSA in model"<<endl;
	return -1;
}
int ModelSW::createStimuliFSAT(string &filename){

	cout<<"Implement function createStimuliFSAT in model"<<endl;
	return -1;
}

int ModelSW::setInput(int index, int value){
	cout<<"Check input type"<<endl;
	return -1;
}
int  ModelSW::setKey( int value){
	cout<<"Check key type"<<endl;
	return -1;
}
int  ModelSW::getFixedkey(){
	return 0;
}


// Load the c++ model into de program
// Useful: https://www.linuxjournal.com/article/3687
// ToDo: Allow to load more that 1 model. Need to use map
ModelSW* ModelSW::loadModelDinamically(string source, string libFile)
{
	string cmd;
	void * handleLib;


	// Create library
	// -Wl,--no-undefined
	cmd="g++ -O2  -std=c++0x -fPIC -rdynamic -shared  "+source+" -o " +libFile+"  ";
	//system("pwd");
	system (cmd.c_str());
	display.print(VIEW_COMMENT)<<"Compilation command: "<<cmd<<endl;


	// Load library
	handleLib = dlopen ( libFile.c_str(), RTLD_NOW |RTLD_GLOBAL );
	if ( !handleLib ) {
		display.print(VIEW_ERROR) << "Error loading library model "<<endl;
		cout<<dlerror()<<endl;
		return NULL;
	}
	display.print(VIEW_COMMENT)<<"Library generated and loaded: "<<libFile.c_str()<<endl;

	ModelSW* (*create)(void);
	create = (ModelSW* (*)(void)) dlsym(handleLib, "createModel");
	if(!create){
		display.print(VIEW_ERROR)<<"Error loading model"<<endl;
		return NULL;
	}

	ModelSW *x= create();

	return x;


}

int ModelSW::fileExists(string &filename){

	return FFManager::fileexists(filename);
}

int ModelSW::getInput(){
	return numInputs;
}
int ModelSW::getFSAN(){
	return  numTracesFSA;
}
int ModelSW::getFSAT(){
	return numTracesFSAT;
}


// It assumes open file
// startIndex describe the number of ignore lines
// Space for inputs and output is allocated but it is not deleted
void ModelSW::getStimuliData(ifstream &file,  	int *input, int *output, int len, int startIndex){

	int aux;
	int **inputs;
	int *out;

	int aux2;


	inputs = new int*[numInputs+1];
	for(int i=0; i < numInputs+1; i++){
		inputs[i] = new int[len];
		for(int j=0; j<len; j++){
			inputs[i][j] = 0; // ToDo: Take form object using this object, from file
		}
	}


	out = new int[len];


	for (int i=0; i < startIndex; i++){
		for(int j=0; j< numInputs+ 1; j++){
			file >> aux; // Reading not necessary inputs
		}
		file >> aux; // Reading not necessary output
	}
	for(int j=0; j< len; j++){
		for (int i=0; i < numInputs+ 1 ; i++){

			file >> inputs[i][j]; // Reading not necessary inputs
			//cout<< inputs[i][j]<<" ";
		}
		file >> out[j]; // Reading not necessary output
		//cout<< " --> " << out[j]<< endl;
	}

	*input= inputs;
	*output = out;

}

void ModelSW::freeMemory(int **inputs, int *output, int len ){

	if(inputs != NULL){
		for(int i=0; i < numInputs+1; i++){
			if (inputs[i] != NULL){
				delete [] inputs[i];
			}
		}
		delete [] inputs;
	}

	if(output != NULL){
		delete [] output;
	}
}


