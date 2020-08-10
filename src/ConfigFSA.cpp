/*
 * ConfigFSA.cpp
 *
 *  Created on: Sep 6, 2019
 *      Author: valencif
 */

#include "ConfigFSA.h"


void ConfigFSA::trimLine(string &line){

	line.erase(0, line.find_first_not_of("\t\n\v\f\r "));
	line.erase(line.find_last_not_of("\t\n\v\f\r ") + 1);

}

int ConfigFSA::getPar(string &line, string parName){

	string init,end;
	size_t pos;

	init="<"+parName+">";
	end="</"+parName+">";

	pos = line.find(init);
	if( pos == std::string::npos){
		return 0;
	}else{
		line.erase(pos, init.length());
		pos = line.find(end);
		line.erase(pos, end.length());
		trimLine(line);
		return 1;
	}

}

void ConfigFSA::parseLine(string &line){

	stringstream num;
	int tmpInt;
	float tmpFloat;

	trimLine(line);

	// Ignore comments and empty lines
	if(line.length() != 0){
		if(line.at(0) == '#'){ // Ignore line
			line="";
			return;
		}
	}else{
		return;
	}

	// Obtained parameters

	// String parameters
	for(unsigned int i=0; i< parStrNames.size(); i++){
		if( getPar(line, parStrNames.at(i)) ){
			parStr.at(i) = line;
			return;
		}
	}

	// Integer parameters
	for(unsigned int i=0; i< parIntNames.size(); i++){
		if( getPar(line, parIntNames.at(i)) ){
			num << line;
			num >> tmpInt;
			parInt.at(i) = tmpInt;
			return;
		}
	}

	// Float parameters
	for(unsigned int i=0; i< parFloatNames.size(); i++){
		if( getPar(line, parFloatNames.at(i)) ){
			num << line;
			num >> tmpFloat;
			parFloat.at(i) = tmpFloat;
			return;
		}
	}

	// Parse executed phases
	for(int i=0; i<10; i++){
		if( getPar(line, phasesNames.at(i)) ){
			num << line;
			num >> phasesExec[i];
			num.str("");
			return;
		}

	}
}


ConfigFSA::ConfigFSA() {


	// Analysis phases
	for(int i=0; i< 10; i++){
		phasesExec[i] = 0;
	}
	phasesNames.push_back(string("folderStructure"));
	phasesNames.push_back(string("stimuliGen"));
	phasesNames.push_back(string("simulationRTL"));
	phasesNames.push_back(string("synthesis"));
	phasesNames.push_back(string("placeRoute"));
	phasesNames.push_back(string("getSensitivity"));
	phasesNames.push_back(string("FSAmodels"));
	phasesNames.push_back(string("phase8"));
	phasesNames.push_back(string("phase9"));
	phasesNames.push_back(string("phase10"));


	// Parameter names

	parStrNames.push_back("projectName");
	parStrNames.push_back("templateFolder");
	parStrNames.push_back("HWpath");
	parStrNames.push_back("pathTB");
	parStrNames.push_back("fileTB");
	parStrNames.push_back("fileGateTB");
	parStrNames.push_back("scriptSYN");
	parStrNames.push_back("scriptPAR");
	parStrNames.push_back("scriptGATE");
	parStrNames.push_back("scriptGATEPR");
	parStrNames.push_back("scriptGATEPR_multi");
	parStrNames.push_back("scriptPlot");
	parStrNames.push_back("scriptPlotT");
	parStrNames.push_back("compiledLibfolder");
	parStrNames.push_back("modelsPath");

	for(unsigned int i=0; i< parStrNames.size(); i++){
		parStr.push_back("");
	}

	parIntNames.push_back("numTracesFSA");
	parIntNames.push_back("numTracesFSAT");
	parIntNames.push_back("debug");
	parIntNames.push_back("phaseFSA");
	parIntNames.push_back("numProc");
	parIntNames.push_back("senSimChunk");
	parIntNames.push_back("pointEvoCorrelation");

	for(unsigned int i=0; i< parIntNames.size(); i++){
		parInt.push_back(0);
	}

	parFloatNames.push_back("unit");
	parFloatNames.push_back("timeSyn");
	parFloatNames.push_back("slack");
	parFloatNames.push_back("precIncr");
	parFloatNames.push_back("Nand");
	parFloatNames.push_back("minTimeGate");
	parFloatNames.push_back("stepTimeGate");

	for(unsigned int i=0; i< parFloatNames.size(); i++){
		parFloat.push_back(0);
	}


}

ConfigFSA::~ConfigFSA() {

}

void ConfigFSA::setConfigFile(string filename){
	this->filename = filename;
}

void ConfigFSA::load(){

	ifstream file;
	string line;
	int i;

	file.open(filename.c_str(), ios::in);
	while (getline(file, line)){
		parseLine(line);
	}
	file.close();


	// Keep phases consistency
	i=0;
	while(phasesExec[i] == 1){
		i++;
	}
	while(i<5){ // Until phase 4 the phases are dependent
		phasesExec[i] = 0;
		i++;
	}

	string temp;
	int exectemp;

	for(i =0; i <10; i++){
		temp = phasesNames.at(i);
		exectemp = phasesExec[i];
		setParInt( temp, exectemp );
	}

}



string ConfigFSA::getParStr(string parName){

	for(unsigned int i=0; i< parStrNames.size(); i++){
		if( parName == parStrNames.at(i)){
			return parStr.at(i);
		}
	}

	return string("");
}


int ConfigFSA::getParInt(string parName){

	for(unsigned int i=0; i< parIntNames.size(); i++){
		if( parName == parIntNames.at(i)){
			return parInt.at(i);
		}
	}
	return 0;

}

float ConfigFSA::getParFloat(string parName){

	for(unsigned int i=0; i< parFloatNames.size(); i++){
		if( parName == parFloatNames.at(i)){
			return parFloat.at(i);
		}
	}
	return 0;

}


void ConfigFSA::setParStr(string parName, string par){

	ifstream file;
	string line;
	string init,end;
	size_t pos;
	int found = 0;

	ofstream tmpFile;

	init="<"+parName+">";
	end="</"+parName+">";


	string tmp;

	file.open(filename.c_str(), ios::in );
	tmpFile.open("Tmp.txt", ios::out);
	while (getline(file, line)){
		pos = line.find(init);
		if( pos != std::string::npos){
			found = 1;
			tmp = init+par+end;
			tmpFile<< tmp <<endl;
		}else{
			tmpFile<<line<<endl;
		}
	}
	if(found == 0){
		tmpFile<<init<<par<<end<<endl;
	}

	file.close();
	tmpFile.close();

	remove(filename.c_str());
	rename( "Tmp.txt" , filename.c_str() );

}
void ConfigFSA::setParInt(string parName, int par){

	stringstream ss;
	string parstr;

	ss << par;

	parstr=ss.str();
	setParStr(parName, parstr);
	ss.str("");

}


void ConfigFSA::setParFloat(string parName, float par){
	stringstream ss;
	string parstr;

	ss << par;

	parstr=ss.str();
	setParStr(parName, parstr);
	ss.str("");

}


void ConfigFSA::setPhase(int par){
	phasesExec[par] = 1;
	setParInt( phasesNames.at(par), 1);
}

int ConfigFSA::isPhaseExec(int i){
	return phasesExec[i];
}


