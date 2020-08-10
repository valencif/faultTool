/*
 * MTLBplot.cpp
 *
 *  Created on: Sep 30, 2019
 *      Author: valencif
 */

#include "MTLBplot.h"

MTLBplot::MTLBplot() {


}

MTLBplot::~MTLBplot() {

}

void MTLBplot::generateScript(string scriptFSAFN, string scriptTemplFN){

	string templatePath, plotterPath;
	string tmp;
	int flag;
	FFManager &folderfiles = FFManager::instance();

	templatePath = folderfiles.getTemplateFolder();
	plotterPath = folderfiles.getPaths(10)+"/";

	tmp = templatePath  + scriptFSAFN;
	flag = folderfiles.copyfile(plotterPath, scriptFSAFN, tmp );
	tmp = templatePath + scriptTemplFN;
	flag = folderfiles.copyfile(plotterPath, scriptTemplFN, tmp );

	scriptResults = scriptFSAFN;
	scriptTemplate = scriptTemplFN;
}
void MTLBplot::plotFSAresults(string resultsPath, string variablePath, int corrKey){


	string cmd;
	string scriptFolder;

	FFManager &folderfiles = FFManager::instance();

	scriptFolder = folderfiles.getPaths(10);


	// Need two pipes because they are unidirectional
	int fd1[2];                      // an array that will hold two file descriptors
	int fd2[2];                      // an array that will hold two file descriptors
	if (pipe(fd1)==-1){               // populates fds with two file descriptors
		cout<<" Error opening pipe for plotting"<<endl;;
		return;
	}
	if (pipe(fd2)==-1){               // populates fds with two file descriptors
		cout<<" Error opening pipe for plotting"<<endl;;
		return;
	}
	pid_t process = fork();          // create child process that is a clone of the parent
	if (process < 0){
		cout<<" Error forking for simulation"<<endl;;
		return;
	}else{
		if (process == 0) {              // Child process (process==0)


			int flag=chdir(scriptFolder.c_str());   // Move current path


			// Input data
			close(fd1[1]);                 // file descriptor unused in child
			dup2(fd1[0], STDIN_FILENO);    // fds[0] (the read end of pipe) donates its data to file descriptor 0
			close(fd1[0]);                 // file descriptor no longer needed in child since stdin is a copy

			// Output data
			close(fd2[0]);                 // file descriptor unused in child
			dup2(fd2[1], STDOUT_FILENO);   // fds[0] (the write end of pipe) donates its data to file descriptor 1
			close(fd2[1]);                 // file descriptor no longer needed in child since stdin is a copy



			char *argv[] = {(char *)"csh", NULL, NULL};   // create argument vector
			if (execvp(argv[0], argv) < 0) exit(0);  // run sort command (exit if something went wrong)

		}else{    // Parent process (process>0)

			volatile size_t tmp;
			char buffer[256];
			int readChar=1;
			int flag=0;
			string out="";
			size_t pos;
			ostringstream oss;

			close(fd1[0]);                 // file descriptor unused in parent
			close(fd2[1]);                 // file descriptor unused in parent

			fcntl(fd2[0], F_SETFL, O_NONBLOCK); // Avoid the pipe to block the program

			cmd = "/opt/matlab/R2016a/bin/matlab  -nodisplay -nosplash -nodesktop  \n";
			dprintf(fd1[1], cmd.c_str() );

			// Wait initialization
			while(flag == 0){
				readChar=read(fd2[0],buffer,256);
				if(readChar > 0 ){
					out.append(buffer,readChar);
					pos = out.find(">>");
					if( pos != std::string::npos){
						flag = 1;
					}
				}
				display.print(VIEW_DEBUG) <<  out;
				out = "";
				sleep(1);
			}

			oss << corrKey;
			cmd = scriptResults.substr(0,scriptResults.size()-2)+"( '"+variablePath+"', '" +resultsPath+ "',"+ oss.str()+") \n";
			dprintf(fd1[1], cmd.c_str() );
			oss.str("");
			display.print(VIEW_NORMAL)  << cmd<<endl;

			// Wait script finish
			flag = 0;
			while(flag == 0){
				readChar=read(fd2[0],buffer,256);
				if(readChar > 0 ){
					out.append(buffer,readChar);
					pos = out.find(">>");
					if( pos != std::string::npos){
						flag = 1;
					}
				}
				display.print(VIEW_NORMAL) <<  out;
				out = "";
				sleep(1);
			}

			dprintf(fd1[1], "exit\n");
			dprintf(fd1[1], "exit\n");

			close(fd1[1]);

			int status;
			pid_t wpid = waitpid(process, &status, 0); // wait for child to finish before exiting
			if(flag == 2){
				return;
			}
			if(wpid != process && WIFEXITED(status)){
				return;
			}

		}
	}
}

void MTLBplot::plotTemplate(string resultsPath, string variablePath){
	int flag;
	string cmd;
	string scriptFolder;

	FFManager &folderfiles = FFManager::instance();

	scriptFolder = folderfiles.getPaths(10);


	// Need two pipes because they are unidirectional
	int fd1[2];                      // an array that will hold two file descriptors
	int fd2[2];                      // an array that will hold two file descriptors
	if (pipe(fd1)==-1){               // populates fds with two file descriptors
		cout<<" Error opening pipe for plotting"<<endl;;
		return;
	}
	if (pipe(fd2)==-1){               // populates fds with two file descriptors
		cout<<" Error opening pipe for plotting"<<endl;;
		return;
	}
	pid_t process = fork();          // create child process that is a clone of the parent
	if (process < 0){
		cout<<" Error forking for simulation"<<endl;;
		return;
	}else{
		if (process == 0) {              // Child process (process==0)

			flag=chdir(scriptFolder.c_str());   // Move current path


			// Input data
			close(fd1[1]);                 // file descriptor unused in child
			dup2(fd1[0], STDIN_FILENO);    // fds[0] (the read end of pipe) donates its data to file descriptor 0
			close(fd1[0]);                 // file descriptor no longer needed in child since stdin is a copy

			// Output data
			close(fd2[0]);                 // file descriptor unused in child
			dup2(fd2[1], STDOUT_FILENO);   // fds[0] (the write end of pipe) donates its data to file descriptor 1
			close(fd2[1]);                 // file descriptor no longer needed in child since stdin is a copy



			char *argv[] = {(char *)"csh", NULL, NULL};   // create argument vector
			if (execvp(argv[0], argv) < 0) exit(0);  // run sort command (exit if something went wrong)

		}else{    // Parent process (process>0)

			volatile size_t tmp;
			char buffer[256];
			int readChar=1;
			int flag=0;
			string out="";
			size_t pos;

			close(fd1[0]);                 // file descriptor unused in parent
			close(fd2[1]);                 // file descriptor unused in parent

			fcntl(fd2[0], F_SETFL, O_NONBLOCK); // Avoid the pipe to block the program

			cmd = "/opt/matlab/R2016a/bin/matlab  -nodisplay -nosplash -nodesktop  \n";
			dprintf(fd1[1], cmd.c_str() );

			// Wait initialization
			while(flag == 0){
				readChar=read(fd2[0],buffer,256);
				if(readChar > 0 ){
					out.append(buffer,readChar);
					pos = out.find(">>");
					if( pos != std::string::npos){
						flag = 1;
					}
				}
				display.print(VIEW_DEBUG) <<  out;
				out = "";
				sleep(1);
			}


			cmd = scriptTemplate.substr(0,scriptTemplate.size()-2)+"( '"+variablePath+"', '" +resultsPath+ "' ) \n";
			dprintf(fd1[1], cmd.c_str() );
			display.print(VIEW_NORMAL)  << cmd<<endl;

			// Wait script finish
			flag = 0;
			while(flag == 0){
				readChar=read(fd2[0],buffer,256);
				if(readChar > 0 ){
					out.append(buffer,readChar);
					pos = out.find(">>");
					if( pos != std::string::npos){
						flag = 1;
					}
				}
				display.print(VIEW_NORMAL) <<  out;
				out = "";
				sleep(1);
			}

			dprintf(fd1[1], "exit\n");
			dprintf(fd1[1], "exit\n");

			close(fd1[1]);

			int status;
			pid_t wpid = waitpid(process, &status, 0); // wait for child to finish before exiting
			if(flag == 2){
				return;
			}
			if(wpid != process && WIFEXITED(status)){
				return;
			}

		}
	}
}

