/*
 * ModelHW.cpp
 *
 *  Created on: Feb 27, 2019
 *      Author: valencif
 */

#include "ModelHW.h"

ModelHW::ModelHW() {
	numberSrc=0;
	// ToDo: Get this information automatically
	clockName="ClkxCI";
	rstName="";

}

ModelHW::~ModelHW() {

}


void ModelHW::setSources(string &sourceFolder, string &fileWithSrc){
	this->sourceFolder=sourceFolder;

	if( FFManager::fileexists(fileWithSrc) ){
		this->fileWithSrc=fileWithSrc;
	}else{
		display.print(VIEW_ERROR)<<"File with HW sources doesn't exist, possible error may occur"<<endl;
	}

}

int ModelHW::captureInfo(){

	ifstream srcs;
	string line;
	string pathHDL;
	string fileHDL;
	int srcCnt=0;
	FFManager &ffm = FFManager::instance();


	// Read the source names
	srcs.open(fileWithSrc.c_str(), ios::in );
	if(srcs.fail()){
		display.print(VIEW_ERROR)<<"Not possible to open source file"<<endl;
		return 0;
	}

	while (getline(srcs, line))
	{
		sources.push_back(line);
		srcCnt++;
	}

	// Check if all source files exist and then copy to the corresponding the project folder
	pathHDL=ffm.getPaths(4,1)+"/";
	for (int i=0; i<srcCnt ; i++){
		line=sourceFolder+sources.at(i);
		if( ffm.fileexists(line) == 0){
			display.print(VIEW_ERROR)<<"Specified source do not exist"<< endl;
			return 0;
		}else{
			fileHDL=pathHDL+sources.at(i);
			if( ffm.fileexists(fileHDL) == 0){
				ffm.copyfile(pathHDL,sources.at(i),line);
			}else{
				display.print(VIEW_WARNING)<<"File "<< sources.at(i) << " is already in the project folder and will not be updated"<< endl;
			}

		}
	}

	srcs.close();
	numberSrc=srcCnt;

	// Information from main file
	line= getArchitectureName();

	return 1;

}


string ModelHW::getArchitectureName(){

	string mainSrc;
	string fileExt;
	FFManager &ffm = FFManager::instance();

	if(!architectureName.empty()){
		return architectureName;
	}

	mainSrc=ffm.getPaths(4,1)+"/"+sources.at(numberSrc-1);
	fileExt= ffm.getExtensionFile(mainSrc);

	if(fileExt== "vhd"){
		return getArchitectureNameVHDL(mainSrc);
	}else{
		display.print(VIEW_ERROR)<<"HW language not accepted"<<endl;
		return string("");
	}

}

#if HAVE_WORKING_REGEX==1
int ModelHW::obtaineArcEntVHDL(string &filename){
	ifstream file;
	string line;
	std::smatch base_match;


	file.open(filename.c_str(), ios::in);

	string reg_str="(?i)architecture (.*) of (.*) is";
	std::regex base_regex(reg_str);
	while (getline(file, line)){
		if (regex_match(line, base_match, base_regex)) {
			if (base_match.size() == 3) {
				architectureName = base_match[1].str();
				entiryName =base_match[2].str();
				break;
			}

		}

	}
	file.close();

	if(!architectureName.empty()){
		cout<<cout<< "\033[1;31m "<<"Not possible to find architecture"<<" \033[0m\n";
		return 0;
	}

	return 1;




}
#else
int ModelHW::obtaineArcEntVHDL(string &filename,string &entity, string &architecture){

	ifstream file;
	string line;
	string arch;
	string lineCapital;

	size_t posEA,posBO; // Position "end architecture"  and "beginning of"
	size_t posEO,posBI; // Position "end of" and "beginning is"


	file.open(filename.c_str(), ios::in);



	string reg_str="(?i)architecture (.*) of (.*) is";
	while (getline(file, line)){
		lineCapital=line;
		transform(lineCapital.begin(), lineCapital.end(),lineCapital.begin(), ::toupper);


		posEA=lineCapital.find(string("ARCHITECTURE"));
		if (posEA== std::string::npos) {
			continue;
		}
		posEA+=13;

		posBO=lineCapital.find(string(" OF "));
		if (posBO == std::string::npos) {
			continue;
		}

		posEO=posBO+4;
		posBO-=1;

		posBI=lineCapital.find(string(" IS"));
		if (posBI == std::string::npos) {
			continue;
		}

		posBI-=1;

		architecture = line.substr(posEA,posBO-posEA+1);
		architecture.erase(remove(architectureName.begin(), architectureName.end(), ' '), architectureName.end());
		entity = line.substr(posEO,posBI-posEO+1);
		entity.erase(remove(entityName.begin(), entityName.end(), ' '), entityName.end());



		break;

	}
	file.close();

	return 1;
}

#endif


string ModelHW::getArchitectureNameVHDL(string &filename){

	if(obtaineArcEntVHDL(filename,entityName,architectureName)==0){
		return string("");
	}
	return architectureName;

}

string ModelHW::getEntityName(){
	string mainSrc;
	string fileExt;
	FFManager &ffm = FFManager::instance();

	if(!entityName.empty()){
		return entityName;
	}

	mainSrc=ffm.getPaths(4,1)+"/"+sources.at(numberSrc-1);
	fileExt= ffm.getExtensionFile(mainSrc);

	if(fileExt== "vhd"){
		return getEntityNameVHDL(mainSrc);
	}else{
		cout<< "\033[1;31m "<<"HW language not accepted"<<" \033[0m\n";
		return string("");
	}
}

string ModelHW::getEntityNameVHDL(string &filename){

	if(obtaineArcEntVHDL(filename,entityName,architectureName)==0){
		return string("");
	}
	return entityName;
}


// Assumptions about module interface:
// Interface done in VHDL
// The interface starts with 1 line "entity <entityName> is"
// Then generic or port sections are written with the same format differing only in the keywords: generic and port
// The keywords generic and port are in the same line with the symbol '(', in this line no signal is specified
// Every signal is specified in a single line
// Closing the sections generic and port is done with the symbol ');' alone in one line
int ModelHW::getModuleInfo(){

	/* 1. Open file
	 * 2. Search for string entity <entity> is
	 * 3. Search generic or port
	 *
	 */

	getEntityName();

	return 0;
}

// Copy predefined testbech into de project folder
void ModelHW::getPreDefinedTB(string & pathSrc, string &filename){

	string pathTB;
	string src;

	FFManager &ffm = FFManager::instance();

	pathTB=ffm.getPaths(4, 3)+"/"+filename;
	src=pathSrc+filename;

	ffm.copyfile(pathTB, src);

	testbenchName=filename;

	obtaineArcEntVHDL(pathTB,entityNameTb,architectureNameTb);




}
void ModelHW::setGateTB(string &pathSrc,string &fileGateTB){

	string pathTB;
	string src;

	FFManager &ffm = FFManager::instance();

	pathTB=ffm.getPaths(4, 3)+"/"+fileGateTB;
	src=pathSrc+fileGateTB;

	ffm.copyfile(pathTB, src);
	testbenchNameGate=fileGateTB;
}
string ModelHW::getGateTB(){

	return testbenchNameGate;

}

string ModelHW::getEntityTb(){
	return entityNameTb;

}

vector<string> & ModelHW::getSources(){

	return sources;

}
string ModelHW::getTestbenchName(){
	return testbenchName;

}

string ModelHW::getClockName(){
	return clockName;
}
