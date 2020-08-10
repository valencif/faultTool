/*
 * SynSNPS.cpp
 *
 *  Created on: Mar 28, 2019
 *      Author: valencif
 */

#include "SynSNPS.h"

SynSNPS::SynSNPS() {


}

SynSNPS::~SynSNPS() {

}


void SynSNPS::translateTags(string &line, ModelHW &model){

	volatile size_t tmp;
	string prop;



	ostringstream ss;


	tmp=line.find(string("#M_ENT"));
	if (tmp != std::string::npos){
		prop=model.getEntityName();
		line.replace(tmp,6,prop);
	}

	tmp=line.find(string("#M_ARCH"));
	if (tmp != std::string::npos){
		prop=model.getArchitectureName();
		line.replace(tmp,7,prop);
	}
	tmp=line.find(string("#M_CLOCK"));
	if (tmp != std::string::npos){
		prop=model.getClockName();
		line.replace(tmp,8,prop);
	}
	tmp=line.find(string("#M_PER"));
	if (tmp != std::string::npos){
		ss << timeSyn/1e-9; // Place in nanosecond
		prop=ss.str();
		line.replace(tmp,6,prop);
	}
	tmp=line.find(string("#M_SRCS"));
	if (tmp != std::string::npos){
		vector<string> sources=model.getSources();
		string folder="./HDL/RTL/";


		for( vector<string>::iterator src = sources.begin(); src != sources.end(); ++src) {
			tmp=src->find(string(".vhd"));
			if (tmp != std::string::npos){
				ss<<"analyze -library work -format vhdl {"<<folder<<*src<<"}"<<endl;
				continue;
			}
			tmp=src->find(string(".v"));
			if (tmp != std::string::npos){
				ss<<"analyze -library work -format verilog {"<<folder<<*src<<"}"<<endl;
				continue;
			}

		}
		prop=ss.str();
		line=prop;
	}
}

int SynSNPS::generateScript(string &templatefile, ModelHW &model){


	FFManager &folderfiles = FFManager::instance();
	string templ=folderfiles.getTemplateFolder()+"/"+templatefile;
	string synScript=folderfiles.getPaths(7,1)+"/"+templatefile;
	string line;
	ostringstream ss;


	scriptName=templatefile;

	ifstream filesrc;
	ofstream filedst;

	filesrc.open(templ.c_str(), ios::in );
	filedst.open(synScript.c_str(), ios::out );

	while (getline(filesrc, line)){
		translateTags(line,model);
		filedst<<line<<endl;
	}


	filesrc.close();
	filedst.close();

	designName = model.getEntityName();
	ss << timeSyn/1e-9; // Place in nanosecond
	designName= designName+"_"+ss.str()+"ns";

	return 0;
}
void SynSNPS::setDesignName(string &templatefile,ModelHW &model){

	ostringstream ss;
	scriptName=templatefile;
	designName = model.getEntityName();
	ss << timeSyn/1e-9; // Place in nanosecond
	designName= designName+"_"+ss.str()+"ns";

}

int SynSNPS::createGateLevelModule(){

	FFManager &folderfiles = FFManager::instance();

	string scriptPath=folderfiles.getPaths(7)+"/";
	string script="BIN/"+scriptName;

	int flag;


	// Need two pipes because they are unidirectional
	int fd1[2];                      // an array that will hold two file descriptors
	int fd2[2];                      // an array that will hold two file descriptors
	if (pipe(fd1)==-1){               // populates fds with two file descriptors
		cout<<" Error opening pipe for simulation"<<endl;;
		return 1;
	}
	if (pipe(fd2)==-1){               // populates fds with two file descriptors
		cout<<" Error opening pipe for simulation"<<endl;;
		return 1;
	}
	pid_t process = fork();          // create child process that is a clone of the parent
	if (process < 0){
		cout<<" Error forking for simulation"<<endl;;
		return 1;
	}else{
		if (process == 0) {              // Child process (process==0)

			flag=chdir(scriptPath.c_str());   // Move current path

			// Input data
			close(fd1[1]);                 // file descriptor unused in child
			dup2(fd1[0], STDIN_FILENO);    // fds[0] (the read end of pipe) donates its data to file descriptor 0
			close(fd1[0]);                 // file descriptor no longer needed in child since stdin is a copy

			// Output data
			close(fd2[0]);                 // file descriptor unused in child
			dup2(fd2[1], STDOUT_FILENO);   // fds[0] (the write end of pipe) donates its data to file descriptor 1
			close(fd2[1]);                 // file descriptor no longer needed in child since stdin is a copy


			char *argv[] = {(char *)"csh", NULL};   // create argument vector
			//if (execlp("csh", NULL)) exit(0);  // run sort command (exit if something went wrong)
			if (execvp(argv[0], argv) < 0) exit(0);  // run sort command (exit if something went wrong)

		}else{    // Parent process (process>0)
			pipeBridge bridge;

			int state=0;
			string line;
			flag=0;

			close(fd1[0]);                 // file descriptor unused in parent
			close(fd2[1]);                 // file descriptor unused in parent

			fcntl(fd2[0], F_SETFL, O_NONBLOCK); // Avoid the pipe to block the program

			bridge.initialize(256,fd2[0],fd1[1],0);

			bridge.sendData("echo START\n");

			while(flag==0){
				switch(state){
				case 0:// wait for start
					bridge.waitFindInLine(string("START"));
					bridge.sendData("source /opt/designconfig/startup/cshrc; echo ENVS\n");
					state=1;
					break;
				case 1:
					bridge.waitFindInLine(string("ENVS"));
					script="dc_shell -f "+script+ " |& tee RPT/output.txt \n";
					bridge.sendData(script);
					state=2;
					break;
				case 2:
					bridge.waitFindInLine(string("Thank you..."),1);
					state=3;
					flag=1;
					break;
				case 3:
					cout<<endl;
					break;
				}
			}
			cout<<endl;

			bridge.sendData("exit\n");

			close(fd1[1]);

			int status;
			pid_t wpid = waitpid(process, &status, 0); // wait for child to finish before exiting
			if(flag == 2){
				return 1;
			}
			if(wpid != process && WIFEXITED(status)){
				return 1;
			}

		}
	}
	return 0;


}

int SynSNPS::checkErrors(){

	FFManager &folderfiles = FFManager::instance();
	string reports=folderfiles.getPaths(7,4)+"/";
	string line,file;
	ifstream filesrc;
	volatile size_t tmp;
	char ch;

	// Check result of linking
	file=reports+"check_link.txt";

	filesrc.open(file.c_str(), ios::in );
	filesrc.seekg(-2,ios_base::end);
	filesrc.get(ch);
	if(ch != '1'){
		cout<<"Link was not correct in synthesis"<<endl;
		return 1;
	}
	filesrc.close();

	// Check result of "check design"
	file=reports+"check_design.txt";

	filesrc.open(file.c_str(), ios::in );
	filesrc.seekg(-2,ios_base::end);
	filesrc.get(ch);
	if(ch != '1'){
		cout<<"Design was not correct in synthesis"<<endl;
		return 1;
	}
	filesrc.close();

	// Check for latches
	file=reports+"output.txt";

	filesrc.open(file.c_str(), ios::in );

	while (getline(filesrc, line)){
		tmp=line.find(string("Latch"));
		if (tmp != std::string::npos){
			cout<<"Design has latches"<<endl;
			return 1;
		}
	}
	filesrc.close();



	return 0;
}
float SynSNPS::checkErrorsTime(string &entityName){

	// Check time violation

	FFManager &folderfiles = FFManager::instance();
	string reports=folderfiles.getPaths(7,4);
	string line;
	volatile size_t tmp;
	ifstream filesrc;
	stringstream ss;

	ss<< reports << "/" << entityName << "_" << timeSyn/1e-9 << "ns_timing.rpt";

	filesrc.open(ss.str().c_str(), ios::in );


	string prop;

	while (getline(filesrc, line)){

		tmp=line.find(string("slack (VIOLATED)"));
		if (tmp != std::string::npos){
			line=line.substr(tmp+17);
			//slack=std::stof(line); // It works even if in some systems an error is shown
			slack=std::atof(line.c_str());;
			break;
		}
		tmp=line.find(string("slack (MET)"));
		if (tmp != std::string::npos){
			line=line.substr(tmp+12);
			//slack=std::stof(line); // It works even if in some systems an error is shown
			slack=std::atof(line.c_str());;
			break;

		}
	}
	filesrc.close();

	return slack;
}
void SynSNPS::obtainedInfo(string &entityName){

	FFManager &folderfiles = FFManager::instance();
	string reports=folderfiles.getPaths(7,4);
	string line,file;
	ifstream filesrc;
	volatile size_t tmp;
	stringstream ss;


	// Read area
	ss<< reports << "/" << entityName << "_" << timeSyn/1e-9 << "ns_area.rpt";
	file=ss.str();

	filesrc.open(file.c_str(), ios::in );

	while (getline(filesrc, line)){
		tmp=line.find(string("Total cell area:"));
		if (tmp != std::string::npos){
			line=line.substr(tmp+17);
			//area=std::stof(line); // It works even if in some systems an error is shown
			area=std::atof(line.c_str());
			gates=area/areaNAND;
		}
	}
	filesrc.close();
	ss.str("");


	// Read area
	ss<< reports << "/" << entityName << "_" << timeSyn/1e-9 << "ns_timing.rpt";
	file=ss.str();

	filesrc.open(file.c_str(), ios::in );

	while (getline(filesrc, line)){
		tmp=line.find(string("data required time"));
		if (tmp != std::string::npos){
			line=line.substr(tmp+19);
			//criticalPath=std::stof(line); // It works even if in some systems an error is shown
			criticalPath=std::atof(line.c_str());

		}
	}
	filesrc.close();


}
void SynSNPS::printInfo(){

	display.print(VIEW_NORMAL)<<"Area module [um2] "<< area << endl;
	display.print(VIEW_NORMAL)<<"Area in [GE] "<< gates << endl;
	display.print(VIEW_NORMAL)<<"Critical path [ns] "<< criticalPath << endl;
	display.print(VIEW_NORMAL)<<"Simulation time [ns] "<< timeSyn << endl;
	display.print(VIEW_NORMAL)<<"Slack [ns] "<< slack << endl;

}

void SynSNPS::findMinTime(){

}

void SynSNPS::cleanFiles(){

	FFManager &folderfiles = FFManager::instance();
	string folders;

	for (int i=2;i <= 6; i++){

		folders=folderfiles.getPaths(7,i)+"/";
		folderfiles.cleanFolder(folders);
	}
	folders=folderfiles.getPaths(4,2)+"/";
	folderfiles.cleanFolder(folders);


}
float SynSNPS::getSlack(){
	return this->slack;
}

void SynSNPS::setAreaMinimumGate(float nandarea){
	this->areaNAND=nandarea;
}

string SynSNPS::getDesignName(){
	return designName;
}



