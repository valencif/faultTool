/*
 * pipeBridge.cpp
 *
 *  Created on: Oct 4, 2019
 *      Author: valencif
 */

#include "pipeBridge.h"

pipeBridge::pipeBridge() {
	pipeIn = 0;
	pipeOut = 0;
	bufferIn = NULL;
	bufferInSize = 0;
	display = 0;

}

pipeBridge::~pipeBridge() {
	if (bufferIn != NULL){
		delete [] bufferIn;
		bufferIn = NULL;
	}
}

void pipeBridge::initialize(int bufferInSize, int	pipeIn, int pipeOut, int display){

	if( bufferInSize <= 0){
		return;
	}
	if (bufferIn != NULL){
		delete [] bufferIn;
	}
	bufferIn = new char[bufferInSize];
	this->bufferInSize = bufferInSize;
	input = "";

	this->pipeIn = pipeIn;
	this->pipeOut = pipeOut;

	this->display = display;

}
void pipeBridge::setDisplay(int display){
	this->display = display;
}

void pipeBridge::checkNewInput(){
	int readChar;
	int flag=0;
	int counter=10; // Limit how many portions of new data are read every time to deliver the control and allow to process the information

	// Check if there is more information available
	while(flag == 0){

		// Check if possible to save more data in the string
		if(input.size()+bufferInSize > input.max_size() ){
			break;
		}

		readChar=read(pipeIn,bufferIn,bufferInSize);

		if(readChar > 0 ){
			input.append(bufferIn,readChar);
			counter -= 1;
			if(counter == 0){
				flag = 1;
			}
		}else{
			flag = 1;
		}

	}
}

int pipeBridge::getLine(string &line){

	volatile size_t ind;

	// Check if there is more information available
	checkNewInput();

	line = "";
	ind = input.find('\n');
	if (ind != std::string::npos) {
		line=input.substr(0,ind);
		input=input.substr(ind+1);
		return 1;
	}

	return 0;

}

int  pipeBridge::getLineFind(string &line,string text){

	volatile size_t ind;
	int readl;
	readl = getLine(line);

	ind = line.find(text);
	if (ind != std::string::npos) {
		return 1;
	}else{
		return 0;
	}

}

void pipeBridge::waitFindInLine(string text, int showProgres){

	string line;
	int flag;
	volatile size_t ind;
	int readl;
	int cont=0;

	flag = 0;
	while(flag == 0){

		readl = getLine(line);
		if(display == 1 && readl == 1){
			cout << line << endl;
		}

		if(readl == 1){
			cont++;
			if(cont%50==0 && showProgres==1){
				cout<<"."<<flush;
			}
			ind = line.find(text);
			if (ind != std::string::npos) {
				flag = 1;
				if(showProgres==1){
					cout<<endl;
				}
			}
		}else{
		    sleep(1);
		}
	}
}



void pipeBridge::waitFindInInput(string text){

	string line;
	int flag;
	volatile size_t ind;
	int readl;

	flag = 0;
	while(flag == 0){

		int readl = getLine(line);
		if(display == 1 && readl==1 ){
			cout << line << endl;
		}
		if(line.size() == 0 ){
			ind = input.find(text);
			if (ind != std::string::npos) {
				flag = 1;
			}
		}


	}
}

int pipeBridge::findInInput(string text){

	volatile size_t ind;


	ind = input.find('\n');
	if (ind != std::string::npos) {
		return 0;
	}
	ind = input.find(text);
	if (ind != std::string::npos) {
		return 1;
	}else{
		return 0;
	}

}



void pipeBridge::sendData(string data){

	dprintf(pipeOut, data.c_str());
	if(display==1){
		cout<<"Command sent to pipe:" << data << endl;
	}
}

void pipeBridge::sendData(stringstream &cmd){

	dprintf(pipeOut, cmd.str().c_str());
	if(display==1){
		cout<<"Command sent to pipe:" << cmd.str() << flush;
	}
	cmd.str("");
}

void pipeBridge::dump(){
	string line;

	while(getLine(line)){
		if(display==1){
			cout<< line <<endl;
		}
	}

}

void pipeBridge::dump(int cnt){
	string line;
	int flag=1;
	int counter = cnt;


	while(counter > 0 || flag==1){
		flag = getLine(line);
		if(flag){
			if(display==1){
				cout<< line <<endl;
			}
			counter=0;
		}else{
			sleep(1);
			counter --;
		}

	}
}
