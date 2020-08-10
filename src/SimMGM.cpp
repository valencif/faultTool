/*
 * SimMGM.cpp
 *
 *  Created on: Feb 27, 2019
 *      Author: valencif
 */

#include "SimMGM.h"

SimMGM::SimMGM() {

	modelHWPtr=NULL;

}

SimMGM::~SimMGM() {

}

void SimMGM::setModelHWRef(ModelHW *modelPtr){
	this->modelHWPtr=modelPtr;
}

void SimMGM::setModelSWRef(ModelSW *modelPtr){
	this->modelSWPtr=modelPtr;
}

int parseCMD(string &line){

	volatile size_t tmp=0;
	line=line.substr( tmp+5 );


	tmp=line.find(string("*.vhd "));
	if (tmp != std::string::npos) {
		line=line.substr( tmp+6 );
		return VHD_SRC;
	}

	tmp=line.find(string("*.v "));
	if (tmp != std::string::npos) {
		line=line.substr( tmp+4 );
		return V_SRC;
	}

	tmp=line.find(string("SIM_CR ")); // Need to be before SIM
	if (tmp != std::string::npos) {
		line=line.substr( tmp+7 );
		return SIM_CR;
	}

	tmp=line.find(string("SIM_CS ")); // Need to be before SIM
	if (tmp != std::string::npos) {
		line=line.substr( tmp+7 );
		return SIM_CS;
	}



	tmp=line.find(string("SIM "));
	if (tmp != std::string::npos) {
		line=line.substr( tmp+4 );
		return SIM;
	}


	return 0;


}
int parseDATA(string &line){

	volatile size_t tmp=0;

	line=line.substr( tmp+6 );

	tmp=line.find(string("LIBS "));
	if (tmp != std::string::npos) {
		line=line.substr( tmp+5 );
		return DATA_LIB;
	}

	tmp=line.find(string("FLAGS "));
	if (tmp != std::string::npos) {
		line=line.substr( tmp+6 );
		return DATA_FLAGS;
	}

	tmp=line.find(string("SDFMIN "));
	if (tmp != std::string::npos) {
		line=line.substr( tmp+7 );
		return DATA_SDF_MIN;
	}
	tmp=line.find(string("SDFTYP "));
	if (tmp != std::string::npos) {
		line=line.substr( tmp+7 );
		return DATA_SDF_TYP;
	}
	tmp=line.find(string("SDFMAX "));
	if (tmp != std::string::npos) {
		line=line.substr( tmp+7 );
		return DATA_SDF_MAX;
	}

	return 0;

}

void SimMGM::setCMDDATA(int index, string &cmd){

	switch(index){
	case VHD_SRC:
		cmdVHDL = cmd;
		break;
	case V_SRC:
		cmdVER = cmd;
		break;
	case SIM:
		cmdSIM = cmd;
		break;
	case SIM_CR:
		cmdSIM_C_RTL = cmd;
		break;
	case SIM_CS:
		cmdSIM_C_SYN = cmd;
		break;
	case DATA_LIB:
		libs.push_back( cmd );
		break;
	case DATA_FLAGS:
		flags = cmd;
		break;
	case DATA_SDF_MIN:
		sdfMinPath = cmd;
		break;
	case DATA_SDF_TYP:
		sdfTypPath = cmd;
		break;
	case DATA_SDF_MAX:
		sdfMaxPath = cmd;
		break;
	}

}

void SimMGM::updateInfoFromScript( string &filename  ){

	ifstream filesrc;
	string line;
	size_t tmp;
	int cmd;
	filesrc.open(filename.c_str(), ios::in );

	while (getline(filesrc, line)){
		tmp=line.find(string("#CMD "));
		if (tmp!= std::string::npos) {
			cmd= parseCMD(line);
			setCMDDATA(cmd, line);
			continue;
		}
		tmp=line.find(string("#DATA "));
		if (tmp!= std::string::npos) {
			cmd= parseDATA(line);
			setCMDDATA(cmd, line);
			continue;
		}

	}

	filesrc.close();
}

void SimMGM::translateVSIM(string &cmd){
	volatile size_t tmp;
	ostringstream num;

	tmp=cmd.find(string("#SIM"));
	if (tmp != std::string::npos){
		cmd.replace(tmp,4,cmdSIM);
	}

	tmp=cmd.find(string("#TB"));
	if (tmp != std::string::npos){
		cmd.replace(tmp,3,modelHWPtr->getEntityTb());
	}

	tmp=cmd.find(string("#FLAGS"));
	if (tmp != std::string::npos){
		cmd.replace(tmp,6,flags);
	}

	tmp=cmd.find(string("#LIB"));
	if (tmp != std::string::npos){
		string lib="";
		for(unsigned int i=0; i<libs.size(); i++){
			lib = lib+"-L ./DLIB/"+libs.at(i)+" ";
		}
		cmd.replace(tmp,4,lib);
	}

	tmp=cmd.find(string("#SDFTYP"));
	if (tmp != std::string::npos){
		string sdf="-sdftyp "+sdfTypPath+designName+"_mapped_vlog.sdf";
		cmd.replace(tmp,7,sdf);
	}

	tmp=cmd.find(string("#SDFMIN"));
	if (tmp != std::string::npos){
		string sdf="-sdfmin "+sdfMinPath+designName+"_pared_v2.1_prec4.sdf";
		cmd.replace(tmp,7,sdf);
	}

	tmp=cmd.find(string("#SDFMAX"));
	if (tmp != std::string::npos){
		string sdf="-sdfmax "+sdfMaxPath+designName+"_pared_v2.1_prec4.sdf";
		cmd.replace(tmp,7,sdf);
	}


	tmp=cmd.find(string("#TIMEMAX"));
	if (tmp != std::string::npos){
		num << maxTimeGate*1000;
		cmd.replace(tmp,8,num.str());
		num.str("");
	}
	tmp=cmd.find(string("#TIMEMIN"));
	if (tmp != std::string::npos){
		num << minTimeGate*1000;
		cmd.replace(tmp,8,num.str());
		num.str("");
	}
	tmp=cmd.find(string("#TIMESTEP"));
	if (tmp != std::string::npos){
		num << stepTimeGate*1000;
		cmd.replace(tmp,9,num.str());
		num.str("");
	}


}

int SimMGM::generateRTLScript( string &filename  ){

	FFManager &folderfiles = FFManager::instance();
	string templ=folderfiles.getTemplateFolder();
	string mgm=folderfiles.getPaths(6,1)+"/";
	string line;
	string srcfile=templ+filename;
	string nameTb;
	vector<string> srcs;

	ifstream filesrc;
	ofstream filedst;

	size_t tmp;

	updateInfoFromScript(srcfile );

	cout<<"source "<<srcfile<<endl;

	filesrc.open(srcfile.c_str(), ios::in );
	filedst.open((mgm+filename).c_str(), ios::out );


	// Write header. Header is written literally
	while (getline(filesrc, line)){
		tmp=line.find(string("#HEADER_END"));
		if (tmp!= std::string::npos) {
			break;
		}
		filedst<<line<<endl;

	}

	// Compile all sources
	srcs=modelHWPtr->getSources();
	for (std::vector<string>::const_iterator i = srcs.begin(); i != srcs.end(); i++){
		tmp=(*i).find(string(".vhd"));
		if (tmp!= std::string::npos) {
			filedst<<cmdVHDL<<" HDL/RTL/"<< (*i) <<endl;
			continue;
		}
		tmp=(*i).find(string(".v"));
		if (tmp!= std::string::npos) {
			filedst<<cmdVER<<" HDL/RTL/"<< (*i) <<endl;
			continue;
		}

	}

	// Compile testbench
	nameTb=modelHWPtr->getTestbenchName();
	tmp=nameTb.find(string(".vhd"));
	if (tmp!= std::string::npos) {
		filedst<<cmdVHDL<<" HDL/TBENCH/"<< nameTb <<endl;
	}else{
		tmp=nameTb.find(string(".v"));
		if (tmp!= std::string::npos) {
			filedst<<cmdVER<<" HDL/TBENCH/"<< nameTb <<endl;
		}
	}

	// Place simulation command
	line=cmdSIM_C_RTL;
	translateVSIM(line);
	filedst<<line << endl;


	filesrc.close();
	filedst.close();

	scriptRTL=filename;

	return 0;


}
int SimMGM::generateSYNScript(string &filename, string &source){


	FFManager &folderfiles = FFManager::instance();
	string templ=folderfiles.getTemplateFolder();
	string mgm=folderfiles.getPaths(6,1)+"/";
	string line;
	string srcfile=templ+filename;
	string nameTb;
	vector<string> srcs;

	ifstream filesrc;
	ofstream filedst;

	size_t tmp;

	updateInfoFromScript(srcfile );

	cout<<"source "<<srcfile<<endl;

	filesrc.open(srcfile.c_str(), ios::in );
	filedst.open((mgm+filename).c_str(), ios::out );


	// Write header. Header is written literally
	while (getline(filesrc, line)){
		tmp=line.find(string("#HEADER_END"));
		if (tmp!= std::string::npos) {
			break;
		}
		filedst<<line<<endl;

	}

	// Compile synthesize source
	designName=source;
	filedst<<cmdVHDL<<" HDL/GATE/"<< source <<"_mapped.vhd"<<endl;


	// Compile testbench
	nameTb=modelHWPtr->getGateTB();
	tmp=nameTb.find(string(".vhd"));
	if (tmp!= std::string::npos) {
		filedst<<cmdVHDL<<" HDL/TBENCH/"<< nameTb <<endl;
	}else{
		tmp=nameTb.find(string(".v"));
		if (tmp!= std::string::npos) {
			filedst<<cmdVER<<" HDL/TBENCH/"<< nameTb <<endl;
		}
	}

	// Place simulation command
	line=cmdSIM_C_SYN;
	translateVSIM(line);
	filedst<<line << endl;


	filesrc.close();
	filedst.close();

	scriptGATE=filename;

	return 0;


}

void SimMGM::setSYNscript(string &script){
	scriptGATE=script;
}

int SimMGM::generatePARScript(string &filename, string &source){


	FFManager &folderfiles = FFManager::instance();
	string templ=folderfiles.getTemplateFolder();
	string mgm=folderfiles.getPaths(6,1)+"/";
	string line;
	string srcfile=templ+filename;
	string nameTb;
	vector<string> srcs;

	ifstream filesrc;
	ofstream filedst;

	size_t tmp;

	updateInfoFromScript(srcfile );

	cout<<"source "<<srcfile<<endl;

	filesrc.open(srcfile.c_str(), ios::in );
	filedst.open((mgm+filename).c_str(), ios::out );


	// Write header. Header is written literally
	while (getline(filesrc, line)){
		tmp=line.find(string("#HEADER_END"));
		if (tmp!= std::string::npos) {
			break;
		}
		filedst<<line<<endl;

	}

	// Compile synthesize source
	designName=source;
	filedst<<cmdVER<<" HDL/GATE/"<< source <<"_pared.v"<<endl;


	// Compile testbench
	nameTb=modelHWPtr->getGateTB();
	tmp=nameTb.find(string(".vhd"));
	if (tmp!= std::string::npos) {
		filedst<<cmdVHDL<<" HDL/TBENCH/"<< nameTb <<endl;
	}else{
		tmp=nameTb.find(string(".v"));
		if (tmp!= std::string::npos) {
			filedst<<cmdVER<<" HDL/TBENCH/"<< nameTb <<endl;
		}
	}

	// Place simulation command
	line=cmdSIM_C_SYN;
	translateVSIM(line);
	filedst<<line << endl;


	filesrc.close();
	filedst.close();

	scriptGATEPR=filename;

	return 0;


}

int SimMGM::generatePARScript3(string &filename, string &source){

	FFManager &folderfiles = FFManager::instance();
	string templ=folderfiles.getTemplateFolder();
	string mgm=folderfiles.getPaths(6,1)+"/";
	string line;
	string srcfile=templ+filename;
	string nameTb;
	vector<string> srcs;

	ifstream filesrc;
	ofstream filedst;

	size_t tmp;

	updateInfoFromScript(srcfile );

	cout<<"source "<<srcfile<<endl;

	filesrc.open(srcfile.c_str(), ios::in );
	filedst.open((mgm+filename).c_str(), ios::out );


	// Write header. Header is written literally
	while (getline(filesrc, line)){
		tmp=line.find(string("#HEADER_END"));
		if (tmp!= std::string::npos) {
			break;
		}
		filedst<<line<<endl;

	}

	// Compile synthesize source
	designName=source;
	filedst<<cmdVER<<" HDL/GATE/"<< source <<"_pared.v"<<endl;


	// Compile testbench
	nameTb=modelHWPtr->getGateTB();
	tmp=nameTb.find(string(".vhd"));
	if (tmp!= std::string::npos) {
		filedst<<cmdVHDL<<" HDL/TBENCH/"<< nameTb <<endl;
	}else{
		tmp=nameTb.find(string(".v"));
		if (tmp!= std::string::npos) {
			filedst<<cmdVER<<" HDL/TBENCH/"<< nameTb <<endl;
		}
	}

	/*line=cmdSIM_C_SYN;
	translateVSIM(line);
	filedst<<line << endl;*/

	// Search for Footer start

	while (getline(filesrc, line)){
		tmp=line.find(string("#FOOTER_START"));
		if (tmp!= std::string::npos) {
			break;
		}

	}
	while (getline(filesrc, line)){
		translateVSIM(line);
		filedst<<line<<endl;

	}




	filesrc.close();
	filedst.close();

	scriptGATEPR=filename;

	return 0;

}

int detectError(string &line){ // Detect if there is an error in the modelsim compilation

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



// https://stackoverflow.com/questions/16073136/how-to-open-new-terminal-through-c-program-in-linux
// https://www.geeksforgeeks.org/c-program-demonstrate-fork-and-pipe/
// https://stackoverflow.com/questions/6171552/popen-simultaneous-read-and-write
// http://www.rozmichelle.com/pipes-forks-dups/
int SimMGM::runSimulationRTL(int child_proc){

	FFManager &folderfiles = FFManager::instance();

	string scriptPath=folderfiles.getPaths(6)+"/";
	string script="BIN/"+scriptRTL;

	int flag;

	if(child_proc<1){
		cout<<"It is needed at least one child process"<<endl;
		return 1;
	}


	// Need two pipes because they are unidirectional
	int fd1[2];                      // an array that will hold two file descriptors
	int fd2[2];                      // an array that will hold two file descriptors
	if (pipe(fd1)==-1){               // populates fds with two file descriptors
		display.print(VIEW_ERROR)<<" Error opening pipe for simulation"<<endl;;
		return 1;
	}
	if (pipe(fd2)==-1){               // populates fds with two file descriptors
		display.print(VIEW_ERROR)<<" Error opening pipe for simulation"<<endl;;
		return 1;
	}
	pid_t process = fork();          // create child process that is a clone of the parent
	if (process < 0){
		display.print(VIEW_ERROR)<<" Error forking for simulation"<<endl;;
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


			char *argv[] = {(char *)"csh", (char *)script.c_str(), NULL};   // create argument vector
			if (execvp(argv[0], argv) < 0) exit(0);  // run sort command (exit if something went wrong)

		}else{    // Parent process (process>0)

			pipeBridge bridge;
			int lineSize;
			int found;
			int status;
			pid_t wpid;
			string line;

			close(fd1[0]);                 // file descriptor unused in parent
			close(fd2[1]);                 // file descriptor unused in parent

			fcntl(fd2[0], F_SETFL, O_NONBLOCK); // Avoid the pipe to block the program

			bridge.initialize(256,fd2[0],fd1[1],0);

			flag=0;
			while(flag==0){
				found = bridge.getLineFind(line, string("VSIM 1> "));
				lineSize = line.size();
				if(lineSize >0){
					display.print(VIEW_DEBUG)<<line<<endl;
					if( detectError(line) > 0){
						display.print(VIEW_ERROR)<<"Error in RTL simulation"<<endl;
					}
					if(found == 1){
						bridge.dump();
						flag = 1;
					}
				}else{
					if( bridge.findInInput(string("VSIM 1> ")) == 1){
						flag=1;
					}else{
						sleep(1);
					}
				}
			}
			bridge.sendData("run -all\n");
			flag=0;
			while(flag==0){
				if( bridge.getLine(line) ){
					display.print(VIEW_DEBUG)<<line<<endl;
					if( line.find(string("Testbench successful")) != std::string::npos){
						flag = 1;
					}
					if( line.find(string("Testbench failed")) != std::string::npos){
						flag = 2;
					}
				}else{
					sleep(1);
				}
			}
			bridge.sendData("exit\n");

			close(fd1[1]);

			wpid = waitpid(process, &status, 0); // wait for child to finish before exiting
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


int SimMGM::runSimulationSYN(int child_proc, int FSASTI){

	int *processes;
	int **fd1, **fd2;
	FFManager &folderfiles = FFManager::instance();

	if(child_proc<1){
		cout<<"It is needed at least one child process"<<endl;
		return 1;
	}

	// Create processes identifier
	processes = new int[child_proc];

	// Create 2*threads number of pipes, 2 file descriptor for every pipe
	fd1 = new int*[child_proc]; // pipes from parent to child
	fd2 = new int*[child_proc]; // pipes from child to parent
	for (int i = 0; i < child_proc; ++i){
		fd1[i] = new int[2];
		fd2[i] = new int[2];
		if (pipe(fd1[i])==-1){
			cout<<" Error opening pipe for simulation"<<endl;;
			return 1;
		}
		if (pipe(fd2[i])==-1){
			cout<<" Error opening pipe for simulation"<<endl;;
			return 1;
		}
	}

	// Create child processes
	for(int i=0;i<child_proc;i++){
		processes[i] = fork();
		if(processes[i] == 0){ // Child processes stay in this block

			string scriptPath=folderfiles.getPaths(6)+"/";
			string script="BIN/"+scriptGATE;
			int flag=chdir(scriptPath.c_str());   // Move current path

			// Input data
			close(fd1[i][1]);              // file descriptor unused in child
			dup2(fd1[i][0], STDIN_FILENO);    // fds[0] (the read end of pipe) donates its data to file descriptor 0
			close(fd1[i][0]);                 // file descriptor no longer needed in child since stdin is a copy

			// Output data
			close(fd2[i][0]);                 // file descriptor unused in child
			dup2(fd2[i][1], STDOUT_FILENO);   // fds[0] (the write end of pipe) donates its data to file descriptor 1
			close(fd2[i][1]);                 // file descriptor no longer needed in child since stdin is a copy

			char *argv[] = {(char *)"csh", (char *)script.c_str(), NULL};   // create argument vector
			if (execvp(argv[0], argv) < 0) exit(0);  // run sort command (exit if something went wrong)*/
		}
	}
	// Parent process

	display.print(VIEW_NORMAL) <<"Controller for process measuring sensitivity has started"<<endl;
	if ( FSASTI == 0 ){
		display.print(VIEW_NORMAL) <<"Sensitivity for FSA analysis"<<endl;
	}else{
		display.print(VIEW_NORMAL) <<"Sensitivity for building templates"<<endl;
	}

	int chunck_size=senSimChunk; ///< Variable to specify the length of input data processes every time.
	int numTraces;
	int progress;
	int dimInputs;
	int setProc;
	int inputsPtr; // Pointer save as an integer
	int outputPtr; // Pointer save as an integer
	int exitFlag;
	SensitivityCtrl *controllers;
	string stimulifilename, reportfilename;
	ifstream  stimulifile;
	ofstream reportfile;
	int status;
	pid_t wpid;
	int counter;
	string tbname;


	for(int i=0;i<child_proc;i++){
		close(fd1[i][0]);   // file descriptor unused in parent
		close(fd2[i][1]);   // file descriptor unused in parent
		fcntl(fd2[i][0], F_SETFL, O_NONBLOCK); // Avoid the pipe to block the program
	}

	this->setAnalyzedIn(0, 0);
	dimInputs = modelSWPtr->getInput()+1;


	if ( FSASTI == 0 ){
		numTraces= modelSWPtr->getFSAN();
		progress = analyzedInputFSA;
		stimulifilename = folderfiles.getStimuliFileFSA();
		reportfilename = folderfiles.getReportFileFSA(string("SYN"));
	}else{
		numTraces= modelSWPtr->getFSAT();
		progress = analyzedInputFSAT;
		stimulifilename = folderfiles.getStimuliFileFSAT();
		reportfilename = folderfiles.getReportFileFSAT(string("SYN"));
	}


	controllers = new SensitivityCtrl[child_proc];
	tbname = modelHWPtr->getEntityTb();
	for(int i=0;i<child_proc;i++){
		controllers[i].setPipes(fd2[i][0], fd1[i][1]);
		controllers[i].setLimits(maxTimeGate, minTimeGate, stepTimeGate, chunck_size, dimInputs);
		controllers[i].initialize();
		controllers[i].setTestBenchName(tbname);
	}


	stimulifile.open(stimulifilename.c_str(), ios::in);
	reportfile.open(reportfilename.c_str(), ios::out);

	exitFlag = 0;
	counter =0;
	while( (progress < numTraces) ){
		setProc = SensitivityCtrl::getFreeController(controllers,child_proc);


		if(setProc == -1){ // There is not free thread
			//display.print(VIEW_WARNING)<<"Waiting for free process"<<endl;
			sleep(1); // Waiting to analyzed
		}else{
			if(counter % 10 ==0){
				cout<<"."<<flush;
			}
			counter++;
			modelSWPtr->getStimuliData(stimulifile, &inputsPtr, &outputPtr, chunck_size);
			controllers[setProc].setInput(&inputsPtr, (int *)outputPtr, progress);
			//cout<< progress << " traces processed " << endl;
			progress += chunck_size;

		}

		for(int i=0;i<child_proc;i++){
			//cout<<"Update controllers"<<endl;
			controllers[i].update();
		}

		/*exitFlag=SensitivityCtrl::allControllerFree(controllers,child_proc);
		//cout<<"Exit flag "<< exitFlag << endl;

		if(progress >= numTraces){
			exitFlag=SensitivityCtrl::allControllerFree(controllers,child_proc);
			cout<<"Exit flag "<< exitFlag << endl;
		}*/
	}
	while( exitFlag == 0 ){
		exitFlag=SensitivityCtrl::allControllerFree(controllers,child_proc);
		for(int i=0;i<child_proc;i++){
			//cout<<"Update controllers"<<endl;
			controllers[i].update();
			//cout<<" controller " << i << " has status "<<controllers[i].getStatus()<<endl;
		}
	}

	for(int i=0;i<child_proc;i++){
		controllers[i].exitMGM();
	}

	// Combine files generated sensitivity files
	string aux;
	string path="./";
	string filePattern="Sensitivity";
	string resultPath, resultFile;
	aux = folderfiles.concFileGroup( path , filePattern);
	resultPath = folderfiles.getPaths(10,2)+"/SYN/";
	folderfiles.createFolder( resultPath );
	if ( FSASTI == 0 ){
		resultFile = filePattern+"FSA.txt";
	}else{
		resultFile = filePattern+"FSAT.txt";
	}
	path = path + filePattern + ".txt";
	folderfiles.copyfile(resultPath, resultFile, path);
	folderfiles.deletefile(path);

	cout<<"Process finished"<<endl;


	stimulifile.close();
	reportfile.close();



	for(int i=0;i<child_proc;i++){
		wpid= waitpid(processes[i], &status, 0); // wait for child to finish before exiting
	}
	cout<<"Parent finishes"<<endl;

	// Destroy pipes
	for (int i = 0; i < child_proc; ++i){
	    delete [] fd1[i];
	    delete [] fd2[i];
	}
	delete [] fd1;
	delete [] fd2;

	// Destroy process ids
	delete [] processes;

	delete [] controllers;

	return 0;

}
int SimMGM::runSimulationPAR(int child_proc, int FSASTI){
	int *processes;
	int **fd1, **fd2;
	FFManager &folderfiles = FFManager::instance();

	if(child_proc<1){
		cout<<"It is needed at least one child process"<<endl;
		return 1;
	}

	// Create processes identifier
	processes = new int[child_proc];

	// Create 2*threads number of pipes, 2 file descriptor for every pipe
	fd1 = new int*[child_proc]; // pipes from parent to child
	fd2 = new int*[child_proc]; // pipes from child to parent
	for (int i = 0; i < child_proc; ++i){
		fd1[i] = new int[2];
		fd2[i] = new int[2];
		if (pipe(fd1[i])==-1){
			cout<<" Error opening pipe for simulation"<<endl;;
			return 1;
		}
		if (pipe(fd2[i])==-1){
			cout<<" Error opening pipe for simulation"<<endl;;
			return 1;
		}
	}

	// Create child processes
	for(int i=0;i<child_proc;i++){
		processes[i] = fork();
		if(processes[i] == 0){ // Child processes stay in this block

			string scriptPath=folderfiles.getPaths(6)+"/";
			string script="BIN/"+scriptGATEPR;
			int flag=chdir(scriptPath.c_str());   // Move current path

			// Input data
			close(fd1[i][1]);              // file descriptor unused in child
			dup2(fd1[i][0], STDIN_FILENO);    // fds[0] (the read end of pipe) donates its data to file descriptor 0
			close(fd1[i][0]);                 // file descriptor no longer needed in child since stdin is a copy

			// Output data
			close(fd2[i][0]);                 // file descriptor unused in child
			dup2(fd2[i][1], STDOUT_FILENO);   // fds[0] (the write end of pipe) donates its data to file descriptor 1
			close(fd2[i][1]);                 // file descriptor no longer needed in child since stdin is a copy

			char *argv[] = {(char *)"csh", (char *)script.c_str(), NULL};   // create argument vector
			if (execvp(argv[0], argv) < 0) exit(0);  // run sort command (exit if something went wrong)*/
		}
	}
	// Parent process

	display.print(VIEW_NORMAL) <<"Controller for process measuring sensitivity has started"<<endl;
	if ( FSASTI == 0 ){
		display.print(VIEW_NORMAL) <<"Sensitivity for FSA analysis"<<endl;
	}else{
		display.print(VIEW_NORMAL) <<"Sensitivity for building templates"<<endl;
	}

	int chunck_size=senSimChunk; ///< Variable to specify the length of input data processes every time.
	int numTraces;
	int progress;
	int dimInputs;
	int setProc;
	int inputsPtr; // Pointer save as an integer
	int outputPtr; // Pointer save as an integer
	int exitFlag;
	SensitivityCtrl *controllers;
	string stimulifilename, reportfilename;
	ifstream  stimulifile;
	ofstream reportfile;
	int status;
	pid_t wpid;
	int counter;
	string tbname;


	for(int i=0;i<child_proc;i++){
		close(fd1[i][0]);   // file descriptor unused in parent
		close(fd2[i][1]);   // file descriptor unused in parent
		fcntl(fd2[i][0], F_SETFL, O_NONBLOCK); // Avoid the pipe to block the program
	}

	this->setAnalyzedIn(0, 0);
	dimInputs = modelSWPtr->getInput()+1;


	if ( FSASTI == 0 ){
		numTraces= modelSWPtr->getFSAN();
		progress = analyzedInputFSA;
		stimulifilename = folderfiles.getStimuliFileFSA();
		reportfilename = folderfiles.getReportFileFSA(string("PAR"));
	}else{
		numTraces= modelSWPtr->getFSAT();
		progress = analyzedInputFSAT;
		stimulifilename = folderfiles.getStimuliFileFSAT();
		reportfilename = folderfiles.getReportFileFSAT(string("PAR"));
	}

	tbname = modelHWPtr->getEntityTb();
	controllers = new SensitivityCtrl[child_proc];
	for(int i=0;i<child_proc;i++){
		controllers[i].setPipes(fd2[i][0], fd1[i][1]);
		controllers[i].setLimits(maxTimeGate, minTimeGate, stepTimeGate, chunck_size, dimInputs);
		controllers[i].initialize();
		controllers[i].setTestBenchName(tbname);
	}


	stimulifile.open(stimulifilename.c_str(), ios::in);
	reportfile.open(reportfilename.c_str(), ios::out);

	if(reportfile.is_open() == 0){
		cout << "File is not open"<<endl;
	}

	exitFlag = 0;
	counter =0;
	while( (progress < numTraces)  ){
		setProc = SensitivityCtrl::getFreeController(controllers,child_proc);

		if(setProc == -1){ // There is not free thread
			//display.print(VIEW_WARNING)<<"Waiting for free process"<<endl;
			sleep(1); // Waiting to analyzed
		}else{
			if(counter % 10 ==0){
				cout<<"."<<flush;
			}

			counter++;
			modelSWPtr->getStimuliData(stimulifile, &inputsPtr, &outputPtr, chunck_size);
			//cout << "Control 1: "<< chunck_size<<endl;
			controllers[setProc].setInput(&inputsPtr, (int *)outputPtr, progress,1);
			cout<< progress << " traces processed " << endl;
			progress += chunck_size;

		}

		for(int i=0;i<child_proc;i++){
			//cout<<"Update controllers"<<endl;
			controllers[i].updatePar();
			//cout<<" controller " << i << " has status "<<controllers[i].getStatus()<<endl;
		}

		//exitFlag=SensitivityCtrl::allControllerFree(controllers,child_proc);

	}

	while( exitFlag == 0 ){
		exitFlag=SensitivityCtrl::allControllerFree(controllers,child_proc);
		for(int i=0;i<child_proc;i++){
			//cout<<"Update controllers"<<endl;
			controllers[i].update();
			//cout<<" controller " << i << " has status "<<controllers[i].getStatus()<<endl;
		}
	}

	for(int i=0;i<child_proc;i++){
		controllers[i].exitMGM();
	}

	// Combine files generated sensitivity files
	string aux;
	string path="./";
	string filePattern="Sensitivity";
	string resultPath, resultFile;
	aux = folderfiles.concFileGroup( path , filePattern);
	resultPath = folderfiles.getPaths(10,2)+"/PAR/";
	folderfiles.createFolder( resultPath );
	if ( FSASTI == 0 ){
		resultFile = filePattern+"FSA.txt";
	}else{
		resultFile = filePattern+"FSAT.txt";
	}
	path = path + filePattern + ".txt";
	folderfiles.copyfile(resultPath, resultFile, path);
	folderfiles.deletefile(path);

	cout<<"Process finished"<<endl;


	stimulifile.close();
	reportfile.close();



	for(int i=0;i<child_proc;i++){
		wpid= waitpid(processes[i], &status, 0); // wait for child to finish before exiting
	}
	cout<<"Parent finishes"<<endl;

	// Destroy pipes
	for (int i = 0; i < child_proc; ++i){
		delete [] fd1[i];
		delete [] fd2[i];
	}
	delete [] fd1;
	delete [] fd2;

	// Destroy process ids
	delete [] processes;

	delete [] controllers;

	return 0;

}

int SimMGM::runSimulationPAR2(int child_proc, int FSASTI){
	int *processes;
	int **fd1, **fd2;
	FFManager &folderfiles = FFManager::instance();
	ostringstream sstr;

	if(child_proc<1){
		cout<<"It is needed at least one child process"<<endl;
		return 1;
	}

	// Create processes identifier
	processes = new int[child_proc];

	// Create 2*threads number of pipes, 2 file descriptor for every pipe
	fd1 = new int*[child_proc]; // pipes from parent to child
	fd2 = new int*[child_proc]; // pipes from child to parent
	for (int i = 0; i < child_proc; ++i){
		fd1[i] = new int[2];
		fd2[i] = new int[2];
		if (pipe(fd1[i])==-1){
			cout<<" Error opening pipe for simulation"<<endl;;
			return 1;
		}
		if (pipe(fd2[i])==-1){
			cout<<" Error opening pipe for simulation"<<endl;;
			return 1;
		}
	}

	// Create child processes
	for(int i=0;i<child_proc;i++){
		processes[i] = fork();
		if(processes[i] == 0){ // Child processes stay in this block


			string scriptPath=folderfiles.getPaths(6)+"/";
			string scriptPath2;
			string pathBase;
			string script="BIN/"+scriptGATEPR;

			if(child_proc > 1){
				sstr << i;
				scriptPath = folderfiles.getPaths(6);
				scriptPath2 = scriptPath+sstr.str();
				//scriptPath += "/";
				folderfiles.copyFolder2(scriptPath,scriptPath2);
				scriptPath = scriptPath2;
				sstr.str("");
			}
			int flag=chdir(scriptPath.c_str());   // Move current path

			// Input data
			close(fd1[i][1]);              // file descriptor unused in child
			dup2(fd1[i][0], STDIN_FILENO);    // fds[0] (the read end of pipe) donates its data to file descriptor 0
			close(fd1[i][0]);                 // file descriptor no longer needed in child since stdin is a copy

			// Output data
			close(fd2[i][0]);                 // file descriptor unused in child
			dup2(fd2[i][1], STDOUT_FILENO);   // fds[0] (the write end of pipe) donates its data to file descriptor 1
			close(fd2[i][1]);                 // file descriptor no longer needed in child since stdin is a copy

			char *argv[] = {(char *)"csh", (char *)script.c_str(), NULL};   // create argument vector
			if (execvp(argv[0], argv) < 0) exit(0);  // run sort command (exit if something went wrong)*/
		}
	}
	// Parent process

	display.print(VIEW_NORMAL) <<"Controller for process measuring sensitivity has started"<<endl;
	if ( FSASTI == 0 ){
		display.print(VIEW_NORMAL) <<"Sensitivity for FSA analysis"<<endl;
	}else{
		display.print(VIEW_NORMAL) <<"Sensitivity for building templates"<<endl;
	}

	int chunck_size=senSimChunk; ///< Variable to specify the length of input data processes every time.
	int numTraces;
	int progress;
	int dimInputs;
	int setProc;
	int inputsPtr; // Pointer save as an integer
	int outputPtr; // Pointer save as an integer
	int exitFlag;
	SensitivityCtrl *controllers;
	string stimulifilename, reportfilename;
	ifstream  stimulifile;
	ofstream reportfile;
	int status;
	pid_t wpid;
	int counter;
	string tbname;


	for(int i=0;i<child_proc;i++){
		close(fd1[i][0]);   // file descriptor unused in parent
		close(fd2[i][1]);   // file descriptor unused in parent
		fcntl(fd2[i][0], F_SETFL, O_NONBLOCK); // Avoid the pipe to block the program
	}

	this->setAnalyzedIn(0, 0);
	dimInputs = modelSWPtr->getInput()+1;


	if ( FSASTI == 0 ){
		numTraces= modelSWPtr->getFSAN();
		progress = analyzedInputFSA;
		stimulifilename = folderfiles.getStimuliFileFSA();
		reportfilename = folderfiles.getReportFileFSA(string("PAR"));
	}else{
		numTraces= modelSWPtr->getFSAT();
		progress = analyzedInputFSAT;
		stimulifilename = folderfiles.getStimuliFileFSAT();
		reportfilename = folderfiles.getReportFileFSAT(string("PAR"));
	}

	tbname = modelHWPtr->getEntityTb();
	controllers = new SensitivityCtrl[child_proc];

	for(int i=0;i<child_proc;i++){
		controllers[i].setPipes(fd2[i][0], fd1[i][1]);
		controllers[i].setLimits(maxTimeGate, minTimeGate, stepTimeGate, chunck_size, dimInputs);
		controllers[i].initialize();
		controllers[i].setTestBenchName(tbname);
	}


	stimulifile.open(stimulifilename.c_str(), ios::in);
	reportfile.open(reportfilename.c_str(), ios::out);

	if(reportfile.is_open() == 0){
		cout << "File is not open"<<endl;
	}

	cout<<"Start 1"<<endl;

	exitFlag = 0;
	counter =0;
	progress = 0;
	while( (progress < numTraces)  ){


		setProc = SensitivityCtrl::getFreeController(controllers,child_proc);

		if(setProc == -1){ // There is not free thread
			//display.print(VIEW_WARNING)<<"Waiting for free process"<<endl;
			sleep(1); // Waiting to analyzed
		}else{
			if(counter % 10 ==0){
				cout<<"."<<flush;
			}

			counter++;
			modelSWPtr->getStimuliData(stimulifile, &inputsPtr, &outputPtr, chunck_size);
			controllers[setProc].setInput(&inputsPtr, (int *)outputPtr, progress,1);
			cout<< progress <<" - " << progress+chunck_size-1 << " traces processing in process " << setProc << endl;
			progress += chunck_size;
			if(progress < numTraces){
				continue;
			}

		}

		for(int i=0;i<child_proc;i++){
			//cout<<"Update controllers"<<endl;
			controllers[i].updatePar2();
			//cout<<" controller " << i << " has status "<<controllers[i].getStatus()<<endl;
		}



		//exitFlag=SensitivityCtrl::allControllerFree(controllers,child_proc);

	}

	while( exitFlag == 0 ){
		exitFlag=SensitivityCtrl::allControllerFree(controllers,child_proc);
		//cout<<"Waiting last processes"<<endl;
		for(int i=0;i<child_proc;i++){
			//cout<<"Update controllers"<<endl;
			controllers[i].updatePar2();
			//cout<<" controller " << i << " has status "<<controllers[i].getStatus()<<endl;
		}
	}

	for(int i=0;i<child_proc;i++){
		controllers[i].exitMGM();
	}

	// Combine files generated sensitivity files
	string aux;
	string path="./";
	string filePattern="Sensitivity";
	string resultPath, resultFile;
	aux = folderfiles.concFileGroup( path , filePattern);
	resultPath = folderfiles.getPaths(10,2)+"/PAR/";
	folderfiles.createFolder( resultPath );
	if ( FSASTI == 0 ){
		resultFile = filePattern+"FSA.txt";
	}else{
		resultFile = filePattern+"FSAT.txt";
	}

	path = path + filePattern + ".txt";
	/*if(folderfiles.fileexists(resultPath+resultFile)){
		folderfiles
	}*/
	folderfiles.copyfile(resultPath, resultFile, path);
	folderfiles.deletefile(path);

	cout<<"Process finished"<<endl;


	stimulifile.close();
	reportfile.close();



	for(int i=0;i<child_proc;i++){
		wpid= waitpid(processes[i], &status, 0); // wait for child to finish before exiting
	}
	cout<<"Parent finishes"<<endl;

	//ToDo -1 (Urgent delete other modelsim folders)

	// Destroy pipes
	for (int i = 0; i < child_proc; ++i){
		delete [] fd1[i];
		delete [] fd2[i];
	}
	delete [] fd1;
	delete [] fd2;

	// Destroy process ids
	delete [] processes;

	delete [] controllers;

	return 0;

}


int SimMGM::runSimulationPAR3(int child_proc, int FSASTI){
	int *processes;
	int **fd1, **fd2;
	FFManager &folderfiles = FFManager::instance();
	ostringstream sstr;

	if(child_proc<1){
		cout<<"It is needed at least one child process"<<endl;
		return 1;
	}

	// Create processes identifier
	processes = new int[child_proc];

	// Create 2*threads number of pipes, 2 file descriptor for every pipe
	fd1 = new int*[child_proc]; // pipes from parent to child
	fd2 = new int*[child_proc]; // pipes from child to parent
	for (int i = 0; i < child_proc; ++i){
		fd1[i] = new int[2];
		fd2[i] = new int[2];
		if (pipe(fd1[i])==-1){
			cout<<" Error opening pipe for simulation"<<endl;;
			return 1;
		}
		if (pipe(fd2[i])==-1){
			cout<<" Error opening pipe for simulation"<<endl;;
			return 1;
		}
	}

	// Create child processes
	for(int i=0;i<child_proc;i++){
		processes[i] = fork();
		if(processes[i] == 0){ // Child processes stay in this block


			string scriptPath=folderfiles.getPaths(6)+"/";
			string scriptPath2;
			string pathBase;
			string script="BIN/"+scriptGATEPR;

			sstr << i;
			scriptPath = folderfiles.getPaths(6);
			scriptPath2 = scriptPath+sstr.str();
			folderfiles.copyFolder2(scriptPath,scriptPath2);
			scriptPath = scriptPath2;
			sstr.str("");

			chdir(scriptPath.c_str());   // Move current path

			// Input data
			close(fd1[i][1]);              // file descriptor unused in child
			dup2(fd1[i][0], STDIN_FILENO);    // fds[0] (the read end of pipe) donates its data to file descriptor 0
			close(fd1[i][0]);                 // file descriptor no longer needed in child since stdin is a copy

			// Output data
			close(fd2[i][0]);                 // file descriptor unused in child
			dup2(fd2[i][1], STDOUT_FILENO);   // fds[0] (the write end of pipe) donates its data to file descriptor 1
			close(fd2[i][1]);                 // file descriptor no longer needed in child since stdin is a copy

			char *argv[] = {(char *)"csh", NULL};   // create argument vector
			if (execvp(argv[0], argv) < 0) exit(0);  // run sort command (exit if something went wrong)*/
		}
	}
	// Parent process

	display.print(VIEW_NORMAL) <<"Controller for process measuring sensitivity has started"<<endl;
	if ( FSASTI == 0 ){
		display.print(VIEW_NORMAL) <<"Sensitivity for FSA analysis"<<endl;
	}else{
		display.print(VIEW_NORMAL) <<"Sensitivity for building templates"<<endl;
	}

	int chunck_size=senSimChunk; ///< Variable to specify the length of input data processes every time.
	int numTraces;
	int progress;
	int dimInputs;
	int setProc;
	int inputsPtr; // Pointer save as an integer
	int outputPtr; // Pointer save as an integer
	int exitFlag;
	SensitivityCtrl *controllers;
	string stimulifilename, reportfilename;
	ifstream  stimulifile;
	ofstream reportfile;
	int status;
	pid_t wpid;
	int counter;
	string tbname;


	for(int i=0;i<child_proc;i++){
		close(fd1[i][0]);   // file descriptor unused in parent
		close(fd2[i][1]);   // file descriptor unused in parent
		fcntl(fd2[i][0], F_SETFL, O_NONBLOCK); // Avoid the pipe to block the program
	}

	this->setAnalyzedIn(0, 0);
	dimInputs = modelSWPtr->getInput()+1;


	if ( FSASTI == 0 ){
		numTraces= modelSWPtr->getFSAN();
		progress = analyzedInputFSA;
		stimulifilename = folderfiles.getStimuliFileFSA();
		reportfilename = folderfiles.getReportFileFSA(string("PAR"));
	}else{
		numTraces= modelSWPtr->getFSAT();
		progress = analyzedInputFSAT;
		stimulifilename = folderfiles.getStimuliFileFSAT();
		reportfilename = folderfiles.getReportFileFSAT(string("PAR"));
	}

	tbname = modelHWPtr->getEntityTb();
	controllers = new SensitivityCtrl[child_proc];

	for(int i=0;i<child_proc;i++){
		controllers[i].setPipes(fd2[i][0], fd1[i][1]);
		controllers[i].setLimits(maxTimeGate, minTimeGate, stepTimeGate, chunck_size, dimInputs);
		controllers[i].initialize3();
		controllers[i].setTestBenchName(tbname);
		controllers[i].setScript("BIN/"+scriptGATEPR);
	}


	stimulifile.open(stimulifilename.c_str(), ios::in);
	reportfile.open(reportfilename.c_str(), ios::out);

	if(reportfile.is_open() == 0){
		cout << "File is not open"<<endl;
	}


	exitFlag = 0;
	counter =0;
	progress = 0;
	while( (progress < numTraces)  ){

		setProc = SensitivityCtrl::getFreeController(controllers,child_proc);

		if(setProc == -1){ // There is not free thread
			//display.print(VIEW_WARNING)<<"Waiting for free process"<<endl;
			sleep(1); // Waiting to analyzed
		}else{
			if(counter % 10 ==0){
				cout<<"."<<flush;
			}

			counter++;
			modelSWPtr->getStimuliData(stimulifile, &inputsPtr, &outputPtr, chunck_size);
			controllers[setProc].setInput3(&inputsPtr, (int *)outputPtr, progress,1);
			cout<< progress <<" - " << progress+chunck_size-1 << " traces processing in process " << setProc << endl;
			progress += chunck_size;
			if(progress < numTraces){
				continue;
			}

		}

		for(int i=0;i<child_proc;i++){
			//cout<<"Update controllers"<<endl;
			controllers[i].updatePar3();
			//cout<<" controller " << i << " has status "<<controllers[i].getStatus()<<endl;
		}



		//exitFlag=SensitivityCtrl::allControllerFree(controllers,child_proc);

	}

	while( exitFlag == 0 ){
		exitFlag=SensitivityCtrl::allControllerFree(controllers,child_proc);
		//cout<<"Waiting last processes"<<endl;
		for(int i=0;i<child_proc;i++){
			//cout<<"Update controllers"<<endl;
			controllers[i].updatePar3();
			//cout<<" controller " << i << " has status "<<controllers[i].getStatus()<<endl;
		}
	}

	for(int i=0;i<child_proc;i++){
		controllers[i].exitMGM();
	}

	// Combine files generated sensitivity files
	string aux;
	string path="./";
	string filePattern="Sensitivity";
	string resultPath, resultFile;
	aux = folderfiles.concFileGroup( path , filePattern);
	resultPath = folderfiles.getPaths(10,2)+"/PAR/";
	folderfiles.createFolder( resultPath );
	if ( FSASTI == 0 ){
		resultFile = filePattern+"FSA.txt";
	}else{
		resultFile = filePattern+"FSAT.txt";
	}

	path = path + filePattern + ".txt";
	/*if(folderfiles.fileexists(resultPath+resultFile)){
		folderfiles
	}*/
	folderfiles.copyfile(resultPath, resultFile, path);
	folderfiles.deletefile(path);

	cout<<"Process finished"<<endl;


	stimulifile.close();
	reportfile.close();



	for(int i=0;i<child_proc;i++){
		wpid= waitpid(processes[i], &status, 0); // wait for child to finish before exiting
	}
	cout<<"Parent finishes"<<endl;

	//ToDo -1 (Urgent delete other modelsim folders)

	// Destroy pipes
	for (int i = 0; i < child_proc; ++i){
		delete [] fd1[i];
		delete [] fd2[i];
	}
	delete [] fd1;
	delete [] fd2;

	// Destroy process ids
	delete [] processes;

	delete [] controllers;

	return 0;

}

int SimMGM::checkErrorRTL(){
	return 0;

}
int SimMGM::checkErrorSYN(){
	return 0;

}
int SimMGM::checkErrorPAR(){
	return 0;

}

// ToDo: Take the information from simulation file
void SimMGM::copyCompiledLib(string &folderSrc){

	FFManager &folderfiles = FFManager::instance();
	string templ=folderfiles.getTemplateFolder();
	string mgmlib=folderfiles.getPaths(6,2)+"/";

	folderfiles.copyFolder(folderSrc, templ, mgmlib);

}
