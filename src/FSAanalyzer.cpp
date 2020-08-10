/*
 * FSAanalyzer.cpp
 *
 *  Created on: Sep 16, 2019
 *      Author: valencif
 */

#include "FSAanalyzer.h"

FSAanalyzer::FSAanalyzer() {
	modelFac = NULL;
	phase = 0;
	swm = NULL;
	updateModels = 0;
}

FSAanalyzer::~FSAanalyzer() {
	if(modelFac != NULL){
		delete modelFac;
	}
}


void FSAanalyzer::setModelsPath(string path){
	pathModels = path;
}

void FSAanalyzer::loadModels(){

	if(modelFac == NULL){
		modelFac = new SMFactory();
	}

	//if( models->loadModels(string("./res/SenModels/")) == 0 ){
	if( modelFac->loadModels( pathModels ) == 0 ){
		display.print(VIEW_ERROR)<<"Error loading sensitivity models"<<endl;
	}

	models = modelFac->getAllModelsPtr();

}

void FSAanalyzer::setSWModel(ModelSW *swm){
	this->swm = swm;
}

void FSAanalyzer::setFilenames(){
	/*senFilename = sensitivityFN;
	senTempFilename = templateFN;*/

	FFManager &folderfiles = FFManager::instance();

	senFilename = folderfiles.getPaths(10,2);

	if(phase == 0){
		senFilename += "SYN/";
	}else{
		senFilename += "PR/";
	}
	senTempFilename = senFilename;

}

void FSAanalyzer::setAnalysis(int phase){
	this->phase = phase;
	setFilenames();
}

void FSAanalyzer::genTemplateModels(){

	// Get stimuli file
	// Get sensitivity

	string stimuliTFN, sensitivityTFN;
	SensitivityModelTemplate *modelPtr;
	int posIntermediate;

	FFManager &folderfiles = FFManager::instance();

	stimuliTFN = folderfiles.getPaths(10,2)+"/STI/stimuliTemplate.txt";



	if(phase==0){
		sensitivityTFN = folderfiles.getPaths(10,2)+"/SYN/SensitivityFSAT.txt";
	}else{
		sensitivityTFN = folderfiles.getPaths(10,2)+"/PAR/SensitivityFSAT.txt";
	}

	//cout<< "Stimuli file: "<< stimuliTFN<< endl;
	//cout<< "Sentivity file: "<< sensitivityTFN << endl;

	posIntermediate = swm->getOuputPos();

	for (unsigned int i=0;i < models.size(); i++){
		//cout << " ---> " << models[i]->isTemplate() <<endl;
		if ( models[i]->isTemplate() == 1){
			modelPtr = (SensitivityModelTemplate*) models[i];
			modelPtr->setMaxIntermediate(swm->getMaxIntermediate());
			modelPtr->generateTemplate(stimuliTFN,posIntermediate,sensitivityTFN,phase);
		}
	}


}


////< The intermediates values in the files is organize as a Matrix where every row are the intermediates
////< assuming a specific key, and every column is a set of inputs of the stimuli.
void FSAanalyzer::genIntermediates(){

	string intermediatesFN, stimuliFN;
	ofstream interF;
	ifstream stimuli;
	int maxKey;
	int input, numInputs, inter, numTest;
	FFManager &folderfiles = FFManager::instance();

	intermediatesFN = folderfiles.getPaths(10,2)+"/STI/intermediates.txt";
	stimuliFN = folderfiles.getPaths(10,2)+"/STI/stimuliTest.txt";


	//cout<<" Files "<< intermediatesFN << endl;

	maxKey = swm->getMaxKey();
	numInputs = swm->getInput();
	numTest = swm->getFSAN();

	if(folderfiles.fileexists(intermediatesFN) == 1 && updateModels==0){
		display.print(VIEW_WARNING)<<"File for intermediates already exit and will not be updated"<<endl;
		return;
	}

	interF.open(intermediatesFN.c_str(), ios::out);
	stimuli.open(stimuliFN.c_str(), ios::in);

	for (int k=0; k< maxKey; k++){
		stimuli.seekg(0, ios::beg);
		swm->setKey( k);
		for(int t=0; t < numTest; t++ ){
			for(int i=0; i< numInputs; i++){
				stimuli >> input;
				swm->setInput(i, input);
				inter = swm->getOuput();
				interF << inter << " ";

			}
			stimuli>>input;// skipping supposedly unknown key
			stimuli>>input;// skipping output
		}
		interF << endl;

	}



	interF.close();
	stimuli.close();

}

void FSAanalyzer::estimatedSenForAllModels(){

	string intermediatesFN, sensitiviFNmodel;
	int numTest, maxKey, intermediate, sensitivity;
	ofstream senF;
	ifstream interF;
	FFManager &folderfiles = FFManager::instance();

	intermediatesFN = folderfiles.getPaths(10,2)+"/STI/intermediates.txt";

	interF.open(intermediatesFN.c_str(), ios::in);

	numTest = swm->getFSAN();
	maxKey = swm->getMaxKey();

	for(unsigned int i=0; i < models.size(); i++){
		interF.seekg(0, ios::beg);
		if(phase==0){
			sensitiviFNmodel = folderfiles.getPaths(10,2)+"/SYN/Sen"+models.at(i)->getModelType()+".txt";
		}else{
			sensitiviFNmodel = folderfiles.getPaths(10,2)+"/PAR/Sen"+models.at(i)->getModelType()+".txt";
		}
		if( folderfiles.fileexists(sensitiviFNmodel) == 1 && updateModels==0){
			display.print(VIEW_WARNING)<<"Sensitivity with model "<< models.at(i)->getModelType() << " already exists and it will not be updated"<<endl;
		}else{
			senF.open(sensitiviFNmodel.c_str(), ios::out);

			for (int k=0; k< maxKey; k++){
				for(int t=0; t < numTest; t++){
					interF >>intermediate;
					sensitivity = models.at(i)->getSensitivity(intermediate);
					senF << sensitivity << " ";
				}
				senF << endl;
			}

			senF.close();
		}

		display.print(VIEW_NORMAL)<<"Sensitivity files at: "<<sensitiviFNmodel<<endl;
	}

	updateModels = 0; // Do not update all models next time

	interF.close();

}

void FSAanalyzer::getCorrelationForAllModels(int precisionEvol){

	FFManager &folderfiles = FFManager::instance();

	string estimatedSenFN, measuredSenFN;
	string correlationFN;



	if(phase==0){
		measuredSenFN = folderfiles.getPaths(10,2)+"/SYN/SensitivityFSA.txt";
	}else{
		measuredSenFN = folderfiles.getPaths(10,2)+"/PAR/SensitivityFSA.txt";
	}

	for(unsigned int i=0; i < models.size(); i++){
			if(phase==0){
				estimatedSenFN = folderfiles.getPaths(10,2)+"/SYN/Sen"+models.at(i)->getModelType()+".txt";
				correlationFN = folderfiles.getPaths(10,2)+"/SYN/Corr"+models.at(i)->getModelType()+".txt";
			}else{
				estimatedSenFN = folderfiles.getPaths(10,2)+"/PAR/Sen"+models.at(i)->getModelType()+".txt";
				correlationFN = folderfiles.getPaths(10,2)+"/PAR/Corr"+models.at(i)->getModelType()+".txt";
			}

			getCorrelation( precisionEvol,  estimatedSenFN,  measuredSenFN, correlationFN);
	}


}

void FSAanalyzer::getCorrelation(int precisionEvol, string estimatedSenFN, string measuredSenFN, string corrEvoFN){

	vector<float> corrEvo(precisionEvol);
	vector<int> measuredSenVec, estimatedSenVec;

	ifstream estimatedSen,measuredSen;
	ofstream corrEvoF;
	int FSAtraces,maxKey, sensitivity;

	FSAtraces = swm->getFSAN();
	maxKey = swm->getMaxKey();

	FFManager &folderfiles = FFManager::instance();

	if(folderfiles.fileexists(corrEvoFN) == 1 && updateModels==0){
		display.print(VIEW_NORMAL)<<"Sensitivity file "<<corrEvoFN<<" will not be updated"<<endl;
		return;
	}
	corrEvoF.open(corrEvoFN.c_str(), ios::out);

	// Read vector of measured sensitivity
	measuredSen.open(measuredSenFN.c_str(), ios::in);
	for(int i=0; i < FSAtraces; i++){
		measuredSen >> sensitivity;
		measuredSenVec.push_back(sensitivity);
	}
	measuredSen.close();


	estimatedSen.open(estimatedSenFN.c_str(), ios::in);
	for(int k=0; k  < maxKey; k++){
		estimatedSenVec.clear();
		for(int i=0; i < FSAtraces; i++){
			estimatedSen >> sensitivity;
			//cout << sensitivity << " ";
			estimatedSenVec.push_back(sensitivity);
		}
		corrEvo = correlationEvo( measuredSenVec, estimatedSenVec, precisionEvol);

		for(unsigned int i=0; i < corrEvo.size(); i++){
			corrEvoF << corrEvo.at(i)<<" ";
			//cout << corrEvo.at(i)<<" ";
		}
		corrEvoF << endl;
		//cout << endl;
	}

	estimatedSen.close();
	corrEvoF.close();

}

vector<float> FSAanalyzer::correlationEvo(vector<int> A, vector<int> B, int precisionEvol){

	int index, N, step;
	float a_acc, b_acc, aa_acc, bb_acc, ab_acc; ///< Accumulators for calculation the correlation
	float pearson, denA, denB, num;
	vector<float> corrEvo(precisionEvol);

	a_acc = 0;
	b_acc = 0;
	aa_acc = 0;
	bb_acc = 0;
	ab_acc = 0;
	N = A.size();


	if(A.size() != B.size()){
		display.print(VIEW_ERROR) << "Estimated and measured sensitivities have different size"<<endl;
		return corrEvo;
	}

	index = N/precisionEvol;
	step = 1;
	for(int i = 0; i < N; i++){
		a_acc += A.at(i);
		b_acc += B.at(i);
		aa_acc += (A.at(i) * A.at(i));
		bb_acc += (B.at(i) * B.at(i));
		ab_acc += (A.at(i) * B.at(i));

		if(i == index-1){
			num = (index*ab_acc- a_acc*b_acc);
			if(num ==0){
				pearson = 0;
			}else{
				denA = sqrt(index*aa_acc - a_acc*a_acc);
				denB = sqrt(index*bb_acc - b_acc*b_acc);
				pearson = num/( denA * denB );
			}
			//cout<<"Saving correlation when index = "<< index <<endl;
			corrEvo.at(step-1) = pearson;
			step = step + 1;
			index = (N*step)/precisionEvol;
		}

		//cout << a_acc << " "<< b_acc << " "<< aa_acc << " "<< bb_acc << " "<< ab_acc << " "<< num << " "<< denA << " "<< denB << " "<<pearson<<endl;
	}

	return corrEvo;
}

/*
 *
 * switch(level2){ // MATLAB
		case 1:
			return MainPath+"/MATLAB/FIG";
		case 2:
			return MainPath+"/MATLAB/VAR";
		case 3:
			return MainPath+"/MATLAB/RPT";
		default:
			return MainPath+"/MATLAB";
		}
		break;
 */


void FSAanalyzer::cleanResults(){
	/*
	FFManager &folderfiles = FFManager::instance();

	string resultFolder;

	resultFolder = folderfiles.getPaths(10,2)+"/SYN/";
	display.print(VIEW_DEBUG) <<" Cleaning "<< resultFolder <<endl;
	folderfiles.cleanFolder(resultFolder);
	//folderfiles.deleteFolder(resultFolder);

	resultFolder = folderfiles.getPaths(10,2)+"/PAR/";
	display.print(VIEW_DEBUG) <<" Cleaning "<< resultFolder <<endl;
	folderfiles.cleanFolder(resultFolder);
	//folderfiles.deleteFolder(resultFolder);

	resultFolder = folderfiles.getPaths(10,2)+"/STI/intermediates.txt";
	display.print(VIEW_DEBUG) <<" Cleaning "<< resultFolder <<endl;
	folderfiles.deletefile(resultFolder);

	resultFolder = folderfiles.getPaths(10,1)+"/";
	display.print(VIEW_DEBUG) <<" Cleaning "<< resultFolder <<endl;
	folderfiles.cleanFolder(resultFolder);
	*/
}

void  FSAanalyzer::updateAllModels(){

	updateModels = 1;

}

