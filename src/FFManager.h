
/**
 * \class FFManager
 *
 *
 * \brief Files and folder manager
 *
 * Class that contains all the files related to a project,
 * additionally it contains function related to managing files and folders
 *
 * \author Andrés Felipe Valencia
 * \version 0.1
 * \copyright GNU Public License.
 * \date : 2019/08/26 16:00:00
 * Contact: valena@usi.ch
 * Created on: 2019/08/26 16:00:00
 *
 */


#ifndef FFMANAGER_H_
#define FFMANAGER_H_

#include <iostream>
#include <stdlib.h>
#include <vector>
#include <string>

#include <sys/stat.h> // mkdir
#include <zconf.h> // rmdir
#include <dirent.h> // DIR
#include <unistd.h> // symlink
#include <memory> // shared_ptr
#include <fstream>
#include <sstream>
#include <cerrno>
#include <bits/stdc++.h>  // Sort

#include "View.h"


#define ERRORFOLDERCREATION 0
#define FOLDERCREATED 1
#define FOLDEREXISTED 2

using namespace std;

// Visualization class
extern View &display;

// ToDo: evaluate if every can be made abstract to avoid the necessity of object creation
class FFManager {
private:
	string MainPath; ///< Path with the complete project (One level up relative to the binary)
	string TemplatesFolder; ///< Folder with the templates used to create the project
	// Constructor private because this class is a Singleton
	FFManager(){}

public:

	// Implementing singleton pattern
	static FFManager &instance()
	{
		static FFManager obj;
		string path="./Templates/";
		obj.setTemplateFolder(path);
		return obj;
	}
	FFManager(FFManager &);              // Don't Implement
	void operator=(FFManager const&);          // Don't implement


	virtual ~FFManager();

	void setTemplateFolder(string &path);
	int createFolder(string &path); // Creates generic folder
	int createProjectFolder(string &projectname); // Creates main folder
	void setProjectPath(string path);
	int createFolderStructure(); // Creates all subfolders and files for the project
	string getPaths(int level1, int level2=0, int level3=0, int level4=0, int level5=0); // Allows to get folder path based on a fixed structure
	int copyfile(string &pathDst, string &filenameDst, string &fileSrc );// Copy a file to a path
	int copyfile(string &fileDst, string &fileSrc );
	void deletefile(string &filenameDst);
	string getStimuliFileFSA();
	string getStimuliFileFSAT();
	string getReportFileFSA(string suffix = "" );
	string getReportFileFSAT(string suffix = "" );
	string getExtensionFile(string &filename);
	string getTemplateFolder();
	int copyFolder(string &foldertocopy, string &folderSrc, string folderDst);
	int copyFolder2( string folderSrc, string folderDst);



	string concFileGroup(string path,string prefix);

	void cleanFolder(string folderPath);
	void deleteFolder(string folderPath);

	vector<string> getfiles(string folderPath);
	vector<string> getfilesPrefix(string folderPath, string prefix);

	int getNumber(string fileName, string prefix, string suffix); ///< Get the number from a file name after removing preffix and suffix

	static int fileexists(string &filename);


};

#endif /* FFMANAGER_H_ */

// Todo: create function check if file exist
// Todo: create function check if folder exist
