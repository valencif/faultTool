/*
 * ParCDS.cpp
 *
 *  Created on: Jul 19, 2019
 *      Author: andres
 */

#include "ParCDS.h"

ParCDS::ParCDS() {
	timeSynCpy=0;

}

ParCDS::~ParCDS() {

}


void ParCDS::translateTags(string &line){

	volatile size_t tmp;
	string prop;



	ostringstream ss;


	tmp=line.find(string("#M_ENT"));
	if (tmp != std::string::npos){
		prop=modelPtr->getEntityName();
		line.replace(tmp,6,prop);
	}

	tmp=line.find(string("#M_ARCH"));
	if (tmp != std::string::npos){
		prop=modelPtr->getArchitectureName();
		line.replace(tmp,7,prop);
	}
	tmp=line.find(string("#M_CLOCK"));
	if (tmp != std::string::npos){
		prop=modelPtr->getClockName();
		line.replace(tmp,8,prop);
	}
	tmp=line.find(string("#M_PER"));
	if (tmp != std::string::npos){
		ss << timeSynCpy; // Place in nanosecond
		prop=ss.str();
		line.replace(tmp,6,prop);
	}
	tmp=line.find(string("#M_SRCS"));
	if (tmp != std::string::npos){
		vector<string> sources=modelPtr->getSources();
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

int ParCDS::generateScript(string &templatefile){

	FFManager &folderfiles = FFManager::instance();
	string templ=folderfiles.getTemplateFolder()+"/"+templatefile;

	string synScript=folderfiles.getPaths(2,1)+"/"+templatefile;
	string line;

	ifstream filesrc;
	ofstream filedst;

	ostringstream ss;

	scriptName=templatefile;

	filesrc.open(templ.c_str(), ios::in );
	filedst.open(synScript.c_str(), ios::out );

	while (getline(filesrc, line)){
		//cout<<"PAR line: "<<line<<endl;
		translateTags(line);
		filedst<<line<<endl;
	}


	filesrc.close();
	filedst.close();

	designName = modelPtr->getEntityName();
	ss << timeSynCpy/1e-9; // Place in nanosecond
	designName= designName+"_"+ss.str()+"ns";

	return generateViewScript();
}

void ParCDS::setDesignName(string &templatefile){

	ostringstream ss;
	scriptName=templatefile;

	designName = modelPtr->getEntityName();
	ss << timeSynCpy/1e-9; // Place in nanosecond
	designName= designName+"_"+ss.str()+"ns";

}


int ParCDS::generateViewScript(){

	FFManager &folderfiles = FFManager::instance();
	string templ=folderfiles.getTemplateFolder()+"moduleView.view";

	string entityname=modelPtr->getEntityName()+".view";
	string synScript=folderfiles.getPaths(2,2)+"/"+entityname;

	string line;

	ifstream filesrc;
	ofstream filedst;

	filesrc.open(templ.c_str(), ios::in );
	filedst.open(synScript.c_str(), ios::out );

	while (getline(filesrc, line)){
		translateTags(line);
		filedst<<line<<endl;
	}


	filesrc.close();
	filedst.close();


	return 0;
}


int ParCDS::placeRouteModule(){

	FFManager &folderfiles = FFManager::instance();

	string scriptPath=folderfiles.getPaths(2)+"/";
	string script="BIN/"+scriptName;

	int flag;

	// Need two pipes because they are unidirectional
	int fd1[2];                      // an array that will hold two file descriptors
	int fd2[2];                      // an array that will hold two file descriptors
	if (pipe(fd1)==-1){               // populates fds with two file descriptors
		cout<<" Error opening pipe for par"<<endl;;
		return 1;
	}
	if (pipe(fd2)==-1){               // populates fds with two file descriptors
		cout<<" Error opening pipe for par"<<endl;;
		return 1;
	}

	pid_t process = fork();          // create child process that is a clone of the parent
	if (process < 0){
		cout<<" Error forking for par"<<endl;;
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

			//volatile size_t tmp;
			pipeBridge bridge;

			ifstream flog;
			string  linef;
			string logfname;

			int state=0;
			string line;
			int xtermPID = 0;
			int readC;
			int status;
			pid_t wpid;

			flag=0;

			close(fd1[0]);                 // file descriptor unused in parent
			close(fd2[1]);                 // file descriptor unused in parent

			fcntl(fd2[0], F_SETFL, O_NONBLOCK); // Avoid the pipe to block the program

			bridge.initialize(256,fd2[0],fd1[1],0);

			script="encounter -no_gui -init "+script+"  -cmd LOG/encounter.cmd -log LOG/encounter.log  -overwrite \n";
			bridge.sendData("source /opt/designconfig/startup/cshrc \n");
			bridge.sendData("setenv OA_UNSUPPORTED_PLAT \"linux_rhel50_gcc44x\" \n");
			bridge.sendData(script);


			logfname=scriptPath+"LOG/encounter.log";


			while(flag==0){
				switch(state){
				case 0:
					readC = bridge.getLineFind(line, string("] "));
					if(readC == 1){
						line = line.substr(3);
						xtermPID = std::atoi(line.c_str());
						state = 1;
					}
					break;
				case 1:
					sleep(1);
					flog.open(logfname.c_str(), ios::in );
					flog.seekg(-100,flog.end); // go 100 character before the end of file
					while (! flog.eof() ){
						getline( flog, linef);
						if (linef.find(string("--- Ending ") )!= std::string::npos){
							state = 2;
						}
					}
					flog.close();
					break;
				case 2:
					//cout << "Killing "<<xtermPID<<endl;
					if (xtermPID != 0){
						kill(xtermPID, SIGTERM);
						flag=1;
					}
					break;

				}
			}
			close(fd1[1]);

			wpid = waitpid(process, &status, 0); // wait for child to finish before exiting
			if(wpid != process && WIFEXITED(status)){
				return 1;
			}

		}
	}
	return 0;
}

int ParCDS::checkErrors(){

	FFManager &folderfiles = FFManager::instance();
	ifstream flog;
	string logfname;
	string line;
	volatile size_t ind;

	int errors = 0;

/*
		ind = input.find('\n');
		if (ind != std::string::npos) {
			line=input.substr(0,ind);
			input=input.substr(ind+1);
			return 1;*/


	logfname=folderfiles.getPaths(2)+"/LOG/encounter.log";

	flog.open(logfname.c_str(), ios::in );
	while (! flog.eof() ){
		getline( flog, line);
		ind = line.find(string("*** Message Summary:") );
		if (ind != std::string::npos){
			line= line.substr(21);
			ind = line.find(string("warning(s),") );
			line= line.substr(ind+12);
			ind = line.find(string("error(s)") );
			line= line.substr(0,ind);

			errors = std::atoi(line.c_str());

			if(errors >0){
				cout<<"Errors in PAR process"<<endl;
				return 1;
			}

		}
	}
	flog.close();



	return 0;
}
void ParCDS::obtainedInfo(string &entityName){


}
void ParCDS::printInfo(){

}
void ParCDS::cleanTmpFile(){

	FFManager &folderfiles = FFManager::instance();
	string folder;
	string file;
	vector<string> filesExt;

	folder = folderfiles.getPaths(2)+"/";

	filesExt = folderfiles.getfiles(folder);

	for(unsigned int i=0; i<filesExt.size(); i++){

		if(filesExt[i] != string("edadk.conf")){
			file=folder+filesExt[i];
			folderfiles.deletefile(file);
		}

	}


}
void ParCDS::cleanFiles(){

	FFManager &folderfiles = FFManager::instance();
	string folders;
	vector<string> files;

	// Clean folders
	for (int i=1;i <= 10; i++){

		folders=folderfiles.getPaths(2,i)+"/";
		folderfiles.cleanFolder(folders);
	}
	folders=folderfiles.getPaths(4,2)+"/";

	// Clean circuit description generated for PAR
	files = folderfiles.getfiles(folders);
	for(unsigned int i=0; i<files.size(); i++){
		if (files[i].find(string("pared") ) != std::string::npos){
			files[i]=folders+files[i];
			folderfiles.deletefile(files[i]);

		}

	}


	// Clean temporal files
	cleanTmpFile();

}
void ParCDS::setModelRef(ModelHW *modelPtr){
	this->modelPtr=modelPtr;
}

void ParCDS::setTimeSyn(float time){
	timeSynCpy=time;
}

string ParCDS::getDesignName(){
	return string("");
}


