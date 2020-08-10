/*
 * SensitivityCtrl.cpp
 *
 *  Created on: Aug 26, 2019
 *      Author: valencif
 */

#include "SensitivityCtrl.h"

int SensitivityCtrl::counter=0;

int extractLineCtrl(string &line, string &input ){

	volatile size_t ind;

	if(input.size() == 0){
		return 0;
	}

	ind = input.find('\n');
	if (ind != std::string::npos) {
		line=input.substr(0,ind);
		input=input.substr(ind+1);
		return 1;
	}
	return 0;
}

int detectErrorCtrl(string &line){ // Detect if there is an error in the modelsim compilation

	volatile size_t tmp;
	string errors;
	int err=0;
	errors=line;

	tmp=errors.find(string("Errors: "));
	if (tmp == std::string::npos){
		return 0;
	}

	errors=errors.substr( tmp+8 );

	tmp=errors.find(string("Warnings: "));
	if (tmp == std::string::npos){
		return 0;
	}
	errors=errors.substr( 0,tmp-2 );
	err=atoi (errors.c_str());

	return err;

	//Errors: 0, Warnings: 0

}


SensitivityCtrl::SensitivityCtrl() {
	pipeIn=0;
	pipeOut=0;
	status= SEN_CTRL_STATUS_IDLE ;
	inputsNum = 0;
	dimInputs = 0;
	maxTime = 0;
	minTime = 0;
	stepTime = 0;

	MaxCycle = NULL;
	MinCycle = NULL;
	vectorSensitvity = NULL;

	simTime=0;

	indexIn = 0;
	inputMatrix = NULL;
	outputVector = NULL;

	inBuffer = "";

	procId = counter;
	counter ++;

	bridge = NULL;

	foundSen = 0;
	foundIndex = 0;
	currPer = 0;


}

SensitivityCtrl::~SensitivityCtrl() {


	if(MaxCycle != NULL){
		delete [] MaxCycle;
	}
	if(MinCycle != NULL){
		delete [] MinCycle;
	}
	if(vectorSensitvity != NULL){
		delete [] vectorSensitvity;
	}
}

void SensitivityCtrl::setPipes(int pipesIn, int pipeOut){
	this->pipeIn=pipesIn;
	this->pipeOut=pipeOut;

	bridge = new pipeBridge();
	bridge->initialize(1024,pipesIn,pipeOut);
}

void SensitivityCtrl::setLimits(float critPath, float minTime, float timeStep, int NumInputs, int dimInputs){

	this->maxTime = 1000*critPath; // Convert to ps
	this->minTime = 1000*minTime; // Convert to ps
	this->stepTime = 1000*timeStep; // Convert to ps
	this->inputsNum = NumInputs;
	this->dimInputs = dimInputs;

}

void SensitivityCtrl::initVectors(){

	for(int i=0; i < inputsNum; i++){
		MaxCycle[i] = (2*maxTime-minTime); // To allow the program start a
		MinCycle[i] = minTime;
		vectorSensitvity[i] = 0;
	}
}


void SensitivityCtrl::initialize(){

	int init=0;
	int isLine;

	string line;

	if(MaxCycle == NULL){
		MaxCycle = new int[inputsNum];
	}
	if(MinCycle == NULL){
		MinCycle = new int[inputsNum];
	}
	if(vectorSensitvity == NULL){
		vectorSensitvity = new int[inputsNum];
	}

	initVectors();



	while (init == 0){
		isLine = bridge->getLineFind(line, string("VSIM 1> ") );
		if( line.size() >0 ){
			cout<<"line: "<<line<<endl;
			if( detectErrorCtrl(line) > 0){
				cout<<"Error in GATE simulation"<<endl;
			}
			if(isLine){
				bridge->dump();
				status = SEN_CTRL_STATUS_WAITING;
				init = 1;
			}
		}else{
			if ( bridge->findInInput("VSIM 1> ") ){ // When programs starts new line is not sent
				status = SEN_CTRL_STATUS_WAITING;
				init = 1;
			}else{
				sleep(1);
			}
		}
	}

	//bridge->setDisplay(1);
}

void SensitivityCtrl::initialize3(){

	string line;

	if(MaxCycle == NULL){
		MaxCycle = new int[inputsNum];
	}
	if(MinCycle == NULL){
		MinCycle = new int[inputsNum];
	}
	if(vectorSensitvity == NULL){
		vectorSensitvity = new int[inputsNum];
	}

	initVectors();

	status = SEN_CTRL_STATUS_WAITING;


	bridge->setDisplay(1);
}

int SensitivityCtrl::getStatus(){
	return status;
}

static void sendMsgPipe(int pipe, stringstream &cmd){

	dprintf(pipe, cmd.str().c_str());
	//cout<< cmd.str().c_str() << endl;
	cmd.str("");
}

void SensitivityCtrl::simCycle(int addWave, int phase ){

	int totalTime = 0; //simTime;
	int timeCycle=0;
	stringstream cmd;

	cmd<<"force -freeze sim:/"<< tbname <<"/uut/ClkxCI 1 "<< totalTime <<"ps \n";
	sendMsgPipe(pipeOut, cmd);


	for(int i=0; i < inputsNum; i++){
		if(vectorSensitvity[i] == 0){

			cmd<<"force -freeze sim:/"<< tbname <<"/uut/Operand1xDI  10#"<<inputMatrix[0][i]<<" "<< totalTime <<"ps \n";
			sendMsgPipe(pipeOut, cmd);
			cmd<<"force -freeze sim:/"<< tbname <<"/uut/Operand2xDI  10#"<<inputMatrix[1][i]<<" "<< totalTime <<"ps \n";
			sendMsgPipe(pipeOut, cmd);

			timeCycle= (MaxCycle[i] + MinCycle[i]) / 2;
			cmd<<"force -freeze sim:/"<< tbname <<"/uut/ClkxCI "<<0<<" "<< totalTime + (timeCycle/2) <<"ps \n";
			sendMsgPipe(pipeOut, cmd);
			cmd<<"force -freeze sim:/"<< tbname <<"/uut/ClkxCI "<<1<<" "<< totalTime + timeCycle <<"ps \n";
			sendMsgPipe(pipeOut, cmd);
			totalTime += timeCycle;

		}
	}

	if (phase == 1 ){
		cmd<<"force -freeze sim:/"<< tbname <<"/uut/ClkxCI "<<0<<" "<< totalTime + (timeCycle/2) <<"ps \n";
		sendMsgPipe(pipeOut, cmd);
		cmd<<"force -freeze sim:/"<< tbname <<"/uut/ClkxCI "<<1<<" "<< totalTime + timeCycle <<"ps \n";
		sendMsgPipe(pipeOut, cmd);
		totalTime += timeCycle;
	}

	if ( addWave == 1){
		cmd<<"add wave -position insertpoint  sim:/"<< tbname <<"/uut/OutxDO \n";
		sendMsgPipe(pipeOut, cmd);
	}

	//cmd<<"run "<< totalTime-simTime<<"ps; echo SimDone \n";
	cmd<<"run "<< totalTime<<"ps; echo SimDone \n";
	sendMsgPipe(pipeOut, cmd);

	status = SEN_CTRL_STATUS_PROC;

}

void SensitivityCtrl::simIteration(int addWave, int phase, int time ){

	int totalTime = 0; //simTime;
	int timeCycle= time;
	stringstream cmd;

	if ( addWave == 1){
		cmd<<"add wave -position insertpoint  sim:/"<< tbname <<"/uut/OutxDO \n";
		bridge->sendData(cmd);
	}

	cmd<<"force -freeze sim:/"<< tbname <<"/uut/ClkxCI 1 0, 0 {"<< (timeCycle/2) <<"ps} -r "<<timeCycle<<"ps \n";
	bridge->sendData(cmd);

	for(int i=foundIndex; i < inputsNum; i++){
			cmd<<"force -freeze sim:/"<< tbname <<"/uut/Operand1xDI  10#"<<inputMatrix[0][i]<<" "<< totalTime <<"ps \n";
			bridge->sendData(cmd);
			cmd<<"force -freeze sim:/"<< tbname <<"/uut/Operand2xDI  10#"<<inputMatrix[1][i]<<" "<< totalTime <<"ps \n";
			bridge->sendData(cmd);
			totalTime += timeCycle;
	}

	if (phase == 1 ){
		totalTime += timeCycle;
	}

	cmd<<"run "<< totalTime<<"ps; echo SimDone \n";
	bridge->sendData(cmd);

	status = SEN_CTRL_STATUS_PROC;

}

void SensitivityCtrl::setTestBenchName(string tb){
	tbname = tb;
}

int SensitivityCtrl::readInLine(string &line){
	char buffer[256];
	int readChar;

	readChar=read(pipeIn,buffer,256);

	if(readChar > 0 ){
		inBuffer.append(buffer,readChar);
	}
	return  extractLineCtrl(line, inBuffer);

}

int SensitivityCtrl::lineContains(string &line, string substr){
	int tmp;
	tmp=line.find(substr);
	if (tmp != std::string::npos){
		return 1;
	}else{
		return 0;
	}


}

void SensitivityCtrl::setScript(string script){
	this->script=script;
}

int SensitivityCtrl::allSensitivityFound(){

	int found =1;
	for(int i=0; i < inputsNum; i++){ // Read all outputs
		if(vectorSensitvity[i] == 0){
			found = 0;
		}
	}
	return found;
}

void SensitivityCtrl::setInput(int *input, int *output, int indexIn, int phase){

	inputMatrix = (int **) *input;;
	outputVector = output;
	this->indexIn = indexIn;

	foundIndex = 0;
	currPer = maxTime;

	simIteration(1,phase, maxTime);


}

void SensitivityCtrl::setInput3(int *input, int *output, int indexIn, int phase){

	FFManager &folderfiles = FFManager::instance();
	string stimuliFN;
	string cmd;
	ofstream stimuliFID;
	stringstream ss;


	inputMatrix = (int **) *input;;
	outputVector = output;
	this->indexIn = indexIn;

	ss << procId;
	stimuliFN = folderfiles.getPaths(6)+ ss.str();
	stimuliFN += "/OUT/";

	cout<<"Folder "<<stimuliFN<<endl;

	while(!folderfiles.fileexists(stimuliFN)){
		sleep(1);
	}

	stimuliFN += "stimuli.txt";

	cout<<"Proc "<< procId <<"=> Stimuli vector in: " << stimuliFN <<endl;
	ss.str();

	stimuliFID.open(stimuliFN.c_str(), ios::out);

	for(int i=0; i < inputsNum; i++){
		stimuliFID <<inputMatrix[0][i]<<" "<< inputMatrix[1][i] << " " <<outputVector[i] << endl;
	}


	stimuliFID.close();


	cmd = "source "+script+";echo \"# SimDone\"\n";
	bridge->sendData(cmd);

	status = SEN_CTRL_STATUS_PROC;



}


int SensitivityCtrl::getFreeController(SensitivityCtrl *ctrls, int len){
	int index=-1;
	for(int i=0;i<len;i++){
		if( ctrls[i].getStatus() == SEN_CTRL_STATUS_WAITING ){
			index=i;
			break;
		}
	}
	return index;
}

int SensitivityCtrl::allControllerFree(SensitivityCtrl *ctrls, int len){
	int index=1;
	for(int i=0;i<len;i++){
		if( ctrls[i].getStatus() != SEN_CTRL_STATUS_WAITING ){
			index=0;
			break;
		}
	}
	return index;
}



void SensitivityCtrl::update( ){

	int timeCycle=0;
	int isline,simDone;
	int tries=0;

	int totalTime = 0; //simTime;
	stringstream cmd, num;
	string line;


	if(status == SEN_CTRL_STATUS_PROC){
		simDone = 0;
		tries = 10;
		while(simDone == 0){
			isline=readInLine(line);
			if(isline == 1){
				if( detectErrorCtrl(line) > 0){
					cout<<"Error in GATE simulation"<<endl;
				}
				if ( lineContains(line, string("# SimDone")) ){
					status=SEN_CTRL_STATUS_PROC_END_CYCLE;
					line = "";
					simDone = 1;
				}
			}else{ // Release the process if the simulation has not finished
				if(tries ==0){
					simDone = 1;
				}
				tries --;
			}
		}
	}

	if (status == SEN_CTRL_STATUS_PROC_END_CYCLE){
		totalTime = simTime;
		for(int i=0; i < inputsNum; i++){ // Read all outputs
			if(vectorSensitvity[i] == 0){
				timeCycle= (MaxCycle[i] + MinCycle[i]) / 2;
				totalTime += timeCycle;
				cmd << "examine -radix unsigned -time {"<<totalTime<<" ps} sim:/"<< tbname <<"/uut/OutxDO \n";
				sendMsgPipe(pipeOut, cmd);
				//cout << "Proc:"<< procId<<" Cmd: "<<"examine -radix unsigned -time {"<<totalTime<<" ps} sim:/"<< tbname <<"/uut/OutxDO"<<endl;

				simDone = 0;
				while (simDone == 0){
					isline=readInLine(line);
					if(isline == 1 && lineContains(line, string("# ")) ){
						line = line.substr(2);
						num<<outputVector[i];
						if(num.str()== line){
							//cout<<"Correct result "; // Modify vector
							MaxCycle[i] = timeCycle;
							if( (timeCycle - stepTime) < MinCycle[i] ){
								vectorSensitvity[i]=1;
							}
						}else{
							MinCycle[i] = timeCycle;
							if( (timeCycle + stepTime) > MaxCycle[i] ){
								vectorSensitvity[i]=1;
							}

						}
						//cout<<"|| "<<line<<" || "<<num.str().c_str()<<" ||"<<endl;
						num.str("");
						simDone=1;
					}
				}
			}
		}

		simTime = totalTime;
		simDone = allSensitivityFound();

		if(simDone==1){
			/*cout << "Max vector :";
			for(int i=0; i < inputsNum; i++){
				cout<< MaxCycle[i] << "  ";
			}
			cout << "|| Min vector :";
			for(int i=0; i < inputsNum; i++){
				cout<< MinCycle[i] << "  ";
			}
			cout << endl;*/
			status = SEN_CTRL_STATUS_END;

		}else{
			simCycle();
		}
	}

	if(status == SEN_CTRL_STATUS_END){
		string filename;
		ofstream file;
		num << indexIn;
		//cout << "Saving information"<<endl;
		filename="Sensitivity"+num.str()+".txt";
		file.open(filename.c_str(), ios::out);
		if(file.is_open()){
			for(int i=0; i < inputsNum; i++){
				file<< MaxCycle[i] << "\n";
			}
			initVectors();
			status = SEN_CTRL_STATUS_WAITING;
		}else{
			cout<<"Error opening file temporal file"<<endl;
		}
		file.close();


	}

}

void SensitivityCtrl::updatePar2(){


	int timeCycle=0;
	int isline,simDone;
	int tries=0;
	int i;

	int totalTime = 0;
	stringstream cmd, num;
	string number;
	string line;


	if(status == SEN_CTRL_STATUS_PROC){

		simDone = 0;
		tries = 10;
		while(simDone == 0){
			//isline=readInLine(line);
			isline = bridge->getLineFind(line, string("# SimDone"));

			if(line.size() >0 ){
				//cout <<"Proc: "<<procId<<"--"<< line <<endl;
				if( detectErrorCtrl(line) > 0){
					cout<<"Error in GATE simulation"<<endl;
				}
				if(isline == 1){
					status=SEN_CTRL_STATUS_PROC_END_CYCLE;
					//cout<<"Proc: "<<procId<<"*********** Cycle finished"<<endl;
					simDone = 1;
					bridge->dump();
				}
			}else{ // Release the process if the simulation has not finished
				if(tries ==0){
					simDone = 1;
				}
				tries --;
			}
		}
	}

	if (status == SEN_CTRL_STATUS_PROC_END_CYCLE){

		totalTime = currPer;
		simDone = 0;


		for(i=foundIndex; i < inputsNum; i++){ // Read all outputs
			totalTime += currPer;
			cmd << "examine -radix unsigned -time {"<<totalTime<<" ps} sim:/"<< tbname <<"/uut/OutxDO \n";
			bridge->sendData(cmd);


			isline = 0;
			while (isline == 0){
				isline=bridge->getLineFind(line, string("# "));
				if(isline ){
					line = line.substr(2);
					num<<outputVector[i];
					number = num.str();
					num.str("");
					if( number == line){
						//cout<<"Correct result "; // Modify vector
						MaxCycle[i] = currPer;
						break;
					}else{
						MinCycle[i] = currPer;
						break;

					}
				}
			}
		}

		bridge->sendData("restart -f \n");
		bridge->waitFindInInput("VSIM ");


		i = foundIndex;
		while(MaxCycle[i] - MinCycle[i] <= stepTime){
			foundSen=1;
			i++;
			currPer = maxTime;
			if(i == inputsNum){
				simDone = 1;
				break;
			}else{
				foundIndex = i;
				foundSen = 0;
			}
		}

		if(simDone==1){
			/*cout << "Max vector :";
			for(int i=0; i < inputsNum; i++){
				cout<< MaxCycle[i] << "  ";
			}
			cout << "|| Min vector :";
			for(int i=0; i < inputsNum; i++){
				cout<< MinCycle[i] << "  ";
			}
			cout << endl;*/
			status = SEN_CTRL_STATUS_END;

		}else{
			currPer = (MaxCycle[foundIndex] + MinCycle[foundIndex])/2;
			simIteration(0,1, currPer);
			return; // Liberate process
		}
	}

	if(status == SEN_CTRL_STATUS_END){
		string filename;
		ofstream file;
		num << indexIn;
		//cout << "Saving information"<<endl;
		filename="Sensitivity"+num.str()+".txt";
		file.open(filename.c_str(), ios::out);
		if(file.is_open()){
			for(int i=0; i < inputsNum; i++){
				file<< MaxCycle[i] << "\n";
			}
			initVectors();
			status = SEN_CTRL_STATUS_WAITING;
		}else{
			cout<<"Error opening file temporal file"<<endl;
		}
		file.close();

		//cout <<"Proc: "<<procId<<"--"<< "Chunck finished" <<endl;


	}

}


void SensitivityCtrl::updatePar3(){


	int timeCycle=0;
	int isline,simDone;
	int tries=0;
	int i;

	int totalTime = 0;
	stringstream cmd, num;
	string number;
	string line;

	FFManager &folderfiles = FFManager::instance();
	vector<string> outputFiles;

	if(status == SEN_CTRL_STATUS_PROC){

		simDone = 0;
		tries = 10;
		while(simDone == 0){
			//isline=readInLine(line);
			isline = bridge->getLineFind(line, string("# SimDone"));

			if(line.size() >0 ){
				//cout <<"Proc "<<procId<<">> "<< line <<endl;
				if( detectErrorCtrl(line) > 0){
					cout<<"Error in GATE simulation"<<endl;
				}
				if(isline == 1){
					status=SEN_CTRL_STATUS_PROC_END_CYCLE;
					//cout <<"Proc "<<procId<<">> "<<"Cycle finished"<<endl;
					simDone = 1;
				}
			}else{ // Release the process if the simulation has not finished
				if(tries ==0){
					simDone = 1;
				}
				tries --;
			}
		}
	}

	if (status == SEN_CTRL_STATUS_PROC_END_CYCLE){

		string folderName;
		string out1, out2;
		ifstream outfile;
		int time;

		num <<procId;
		folderName = folderfiles.getPaths(6)+num.str()+"/OUT/";
		num.str("");



		outputFiles = folderfiles.getfilesPrefix(folderName,"file");

		for (auto filename = outputFiles.begin(); filename != outputFiles.end(); filename++) {

			time = folderfiles.getNumber(*filename, "file", ".rpt");

			//cout<<"Testing time "<< time << endl;

			outfile.open( (folderName+(*filename)).c_str(), ios::in);
			for(i=0; i < inputsNum; i++){ // Read all outputs
				getline(outfile,line);
				out1 = line.substr(0,line.find(" "));
				out2 = line.substr(line.find(" ")+1);
				if( out1 == out2 && time < MaxCycle[i] ){
					MaxCycle[i] = time;
				}
				if( out1 != out2 && time > MinCycle[i] ){
					MinCycle[i] = time;
				}

			}
			outfile.close();
		}

		status = SEN_CTRL_STATUS_END;
		return;

	}

	if(status == SEN_CTRL_STATUS_END){
		string filename;
		ofstream file;
		num << indexIn;
		//cout << "Saving information"<<endl;
		filename="Sensitivity"+num.str()+".txt";
		file.open(filename.c_str(), ios::out);
		if(file.is_open()){
			for(int i=0; i < inputsNum; i++){
				file<< MinCycle[i]+ stepTime << "\n";
			}
			initVectors();
			status = SEN_CTRL_STATUS_WAITING;
		}else{
			cout<<"Error opening file temporal file"<<endl;
		}
		file.close();

	}

}

void SensitivityCtrl::updatePar(){

	int timeCycle=0;
	int isline,simDone;
	int tries=0;

	int totalTime = 0; //simTime;
	stringstream cmd, num;
	string line;


	if(status == SEN_CTRL_STATUS_PROC){

		simDone = 0;
		tries = 10;
		while(simDone == 0){
			isline=readInLine(line);

			if(isline == 1){
				if( detectErrorCtrl(line) > 0){
					cout<<"Error in GATE simulation"<<endl;
				}
				if ( lineContains(line, string("# SimDone")) ){
					status=SEN_CTRL_STATUS_PROC_END_CYCLE;
					line = "";
					//cout<<"*********** Cycle finished"<<endl;
					simDone = 1;
				}
			}else{ // Release the process if the simulation has not finished
				if(tries ==0){
					simDone = 1;
				}
				tries --;
			}
		}
	}

	if (status == SEN_CTRL_STATUS_PROC_END_CYCLE){
		totalTime = simTime;

		timeCycle= (MaxCycle[0] + MinCycle[0]) / 2;
		totalTime += timeCycle;
		//cmd << "examine -radix unsigned -time {"<<totalTime<<" ps} sim:/"<< tbname <<"/uut/OutxDO \n";
		//sendMsgPipe(pipeOut, cmd);

		for(int i=0; i < inputsNum; i++){ // Read all outputs
			if(vectorSensitvity[i] == 0){
				if( i < inputsNum - 1){
					timeCycle= (MaxCycle[i+1] + MinCycle[i+1]) / 2;
				}else{
					//Not updating timeCycle
				}
				totalTime += timeCycle;
				cmd << "examine -radix unsigned -time {"<<totalTime<<" ps} sim:/"<< tbname <<"/uut/OutxDO \n";
				sendMsgPipe(pipeOut, cmd);
				//cout << "Proc:"<< procId<<" Cmd: "<<"examine -radix unsigned -time {"<<totalTime<<" ps} sim:/"<< tbname <<"/uut/OutxDO"<<endl;

				simDone = 0;
				while (simDone == 0){
					isline=readInLine(line);
					if(isline == 1 && lineContains(line, string("# ")) ){
						line = line.substr(2);
						num<<outputVector[i];
						if(num.str().c_str() == line){
							//cout<<"Correct result "; // Modify vector
							MaxCycle[i] = timeCycle;
							if( (timeCycle - stepTime) < MinCycle[i] ){
								vectorSensitvity[i]=1;
							}
						}else{
							MinCycle[i] = timeCycle;
							if( (timeCycle + stepTime) > MaxCycle[i] ){
								vectorSensitvity[i]=1;
							}

						}
						//cout<<"|| "<<line<<" || "<<num.str().c_str()<<" ||"<<endl;
						num.str("");
						simDone=1;
					}
				}
			}
		}

		simTime = totalTime;
		simDone = allSensitivityFound();

		if(simDone==1){
			/*cout << "Max vector :";
			for(int i=0; i < inputsNum; i++){
				cout<< MaxCycle[i] << "  ";
			}
			cout << "|| Min vector :";
			for(int i=0; i < inputsNum; i++){
				cout<< MinCycle[i] << "  ";
			}
			cout << endl;*/
			status = SEN_CTRL_STATUS_END;

		}else{
			simCycle(0,1);
		}
	}

	if(status == SEN_CTRL_STATUS_END){
		string filename;
		ofstream file;
		num << indexIn;
		//cout << "Saving information"<<endl;
		filename="Sensitivity"+num.str()+".txt";
		file.open(filename.c_str(), ios::out);
		if(file.is_open()){
			for(int i=0; i < inputsNum; i++){
				file<< MaxCycle[i] << "\n";
			}
			initVectors();
			status = SEN_CTRL_STATUS_WAITING;
		}else{
			cout<<"Error opening file temporal file"<<endl;
		}
		file.close();


	}

}


void SensitivityCtrl::exitMGM(){

	stringstream cmd;
	int isline;
	string line;

	cmd<<"exit \n";
	sendMsgPipe(pipeOut, cmd);

	isline =1 ;
	while(isline){
		isline=readInLine(line);
		//cout << line << endl;
	}

	freeSpace();




}

void SensitivityCtrl::freeSpace(){

	if(inputMatrix != NULL){
		for(int i=0; i < dimInputs; i++){
			if (inputMatrix[i] != NULL){
				delete [] inputMatrix[i];
			}
		}
		delete [] inputMatrix;
	}

	if(outputVector != NULL){
		delete [] outputVector;
	}

}
