/*
 * FFManager.cpp
 *
 *  Created on: Feb 21, 2019
 *      Author: valencif
 */

#include "FFManager.h"



/*
FFManager::FFManager() {


	TemplatesFolder="./Templates/";

}
*/

FFManager::~FFManager() {
}

void FFManager::setTemplateFolder(string &path){
	TemplatesFolder=path;
}

string FFManager::getTemplateFolder(){
	return TemplatesFolder;

}


string FFManager::getPaths(int level1, int level2, int level3, int level4, int level5){

	switch(level1){
	case 1:
		switch(level2){ // CDS_VISO
		case 1:
			return MainPath+"/CDS_VISO/DLIB";
		default:
			return MainPath+"/CDS_VISO";
		}
		break;
	case 2:
		switch(level2){ // CDS_SOCE
		case 1:
			return MainPath+"/CDS_SOCE/BIN";
		case 2:
			return MainPath+"/CDS_SOCE/CONF";
		case 3:
			return MainPath+"/CDS_SOCE/CTS";
		case 4:
			return MainPath+"/CDS_SOCE/DB";
		case 5:
			return MainPath+"/CDS_SOCE/DEX";
		case 6:
			return MainPath+"/CDS_SOCE/LOG";
		case 7:
			return MainPath+"/CDS_SOCE/RPT";
		case 8:
			return MainPath+"/CDS_SOCE/SDC";
		case 9:
			return MainPath+"/CDS_SOCE/TEC";
		case 10:
			return MainPath+"/CDS_SOCE/TIM";
		default:
			return MainPath+"/CDS_SOCE";
		}
		break;
	case 3:
		return MainPath+"/DOC";
	case 4:
		switch(level2){ // HDL
		case 1:
			return MainPath+"/HDL/RTL";
		case 2:
			return MainPath+"/HDL/GATE";
		case 3:
			return MainPath+"/HDL/TBENCH";
		default:
			return MainPath+"/HDL";
		}
		break;
	case 5:
		return MainPath+"/IP";
		break;
	case 6:
		switch(level2){ // MGC_MSIM
		case 1:
			return MainPath+"/MGC_MSIM/BIN";
		case 2:
			return MainPath+"/MGC_MSIM/DLIB";
		case 3:
			return MainPath+"/MGC_MSIM/OUT";
		case 4:
			return MainPath+"/MGC_MSIM/PROJ";
		default:
			return MainPath+"/MGC_MSIM";
		}
		break;
	case 7:
		switch(level2){ // SNPS_DC
		case 1:
			return MainPath+"/SNPS_DC/BIN";
		case 2:
			return MainPath+"/SNPS_DC/DB";
		case 3:
			return MainPath+"/SNPS_DC/DLIB";
		case 4:
			return MainPath+"/SNPS_DC/RPT";
		case 5:
			return MainPath+"/SNPS_DC/SDC";
		case 6:
			return MainPath+"/SNPS_DC/TIM";
		default:
			return MainPath+"/SNPS_DC";
		}
		break;
	case 8:
		switch(level2){ // SNPS_TMAX
		case 1:
			return MainPath+"/SNPS_TMAX/BIN";
		case 2:
			return MainPath+"/SNPS_TMAX/RPT";
		case 3:
			return MainPath+"/SNPS_TMAX/TV";
		case 4:
			return MainPath+"/SNPS_DC/TIM";
		default:
			return MainPath+"/SNPS_TMAX";
		}
		break;
	case 9:
		return MainPath+"/SNPS_NSIM";
		break;
	case 10:
		switch(level2){ // MATLAB
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
	default:
		return MainPath;
	}



}


// Takes the name of the project
// Returns: ERRORFOLDERCREATION, FOLDERCREATED, FOLDEREXISTED
int FFManager::createFolder(string &path){ // Creates generic folder

	DIR *dp;
	dp = opendir(path.c_str());

	int previous_mask;
	int mask=0777;


	previous_mask=umask(0); // Allow to give all right

	if(dp==NULL){
		try{
			mkdir(path.c_str(),mask);
			return FOLDERCREATED;
		}catch (...){
			return ERRORFOLDERCREATION;
		}
	}

	closedir(dp);
	return FOLDEREXISTED;
}

// Takes the name of the project
// Returns: ERRORFOLDERCREATION, FOLDERCREATED, FOLDEREXISTED
int FFManager::createProjectFolder(string &projectname){

	string relativePath=string("../")+projectname;
	int result;
	result=createFolder(relativePath);

	if(result != ERRORFOLDERCREATION){
		MainPath=relativePath;
	}


	return result;

}

void FFManager::setProjectPath(string path){
	MainPath=path;
}


int FFManager::createFolderStructure(){

	string pathAux;
	string fileSrc;
	string fileDst;
	int level1Limit=10;
	int level2Limits[10]={1,10,0,3,0,4,6,4,0,3};
	int linkindex[6]={1,2,6,7,8,9};
	int flag;


	for (int i=0;i< level1Limit ;i++){
		for (int j=0;j<=level2Limits[i];j++){
			pathAux=getPaths(i+1,j);
			if( createFolder( pathAux) == ERRORFOLDERCREATION){
				return ERRORFOLDERCREATION;
			}
		}
	}


	fileDst="edadk.conf";
	pathAux=getPaths(0)+"/";
	fileSrc=TemplatesFolder+fileDst;
	copyfile(pathAux, fileDst, fileSrc);

	fileDst=".synopsys_dc.setup";
	pathAux=getPaths(7)+"/";
	fileSrc=TemplatesFolder+fileDst;
	copyfile(pathAux, fileDst, fileSrc);

	pathAux=getPaths(10,2)+"/STI";
	if( createFolder( pathAux) == ERRORFOLDERCREATION){
		return ERRORFOLDERCREATION;
	}


	for (int i=0;i<6;i++){
		pathAux=getPaths(linkindex[i])+"/HDL";
		flag= symlink("../HDL",pathAux.c_str());
	}
	for (int i=0;i<6;i++){
		pathAux=getPaths(linkindex[i])+"/edadk.conf";
		flag= symlink("../edadk.conf",pathAux.c_str());
	}


	return FOLDERCREATED;

}

int FFManager::copyfile(string &pathDst, string &filenameDst, string &fileSrc )
{

    string fileDst=pathDst+filenameDst;
    return copyfile(fileDst, fileSrc );

}

int FFManager::copyfile(string &fileDst, string &fileSrc ){

	ifstream filesrc;
	ofstream filedst;

	filesrc.open(fileSrc.c_str(), ios::in | ios::binary);
	filedst.open(fileDst.c_str(), ios::out | ios::binary);
	filedst<< filesrc.rdbuf();

	filesrc.close();
	filedst.close();


	 return 0;
}

void FFManager::deletefile(string &filenameDst){
	remove(filenameDst.c_str());
}


string FFManager::getStimuliFileFSA(){
	return getPaths(10,2)+"/STI/stimuliTest.txt";
}
string FFManager::getStimuliFileFSAT(){
	return getPaths(10,2)+"/STI/stimuliTemplate.txt";
}

string FFManager::getReportFileFSA(string suffix){
	return getPaths(10,3)+"/RPTTest"+suffix+".txt";
}

string FFManager::getReportFileFSAT(string suffix){
	return getPaths(10,3)+"/RPTTemplate"+suffix+".txt";
}


int FFManager::fileexists(string &filename){

	struct stat buf;
	int flag;

	flag=stat(filename.c_str(), &buf);

	return ( flag == 0);


}

string FFManager::getExtensionFile(string &filename){
	string ext;

	ext=filename.substr(filename.find_last_of(".") + 1);

	return ext;
}

void FFManager::cleanFolder(string folderPath){
	DIR *folderDir;
	struct dirent *next_file;
	string files;
	struct stat structure;
	int status;


	folderDir= opendir(folderPath.c_str());
	if(folderDir!=NULL){
		while ( (next_file = readdir(folderDir)) != NULL ){
			files = string(next_file->d_name);

			if( files == string("..")){
				continue;
			}
			if(files == string(".")){
				continue;
			}
			files=folderPath+ files;

			stat(files.c_str(), &structure);


			if(S_ISDIR(structure.st_mode)){ // Folder
				files=files+"/";
				cleanFolder(files);
				status=rmdir(files.c_str());
				stat(files.c_str(), &structure);
				if( status == 0){
					//display.print(VIEW_DEBUG)<< "Deleting folder: "<< files <<endl;
				}else{
					if( errno== ENOTEMPTY){
						display.print(VIEW_DEBUG)<<files<< " Not empty"<<endl;
					}
					if( errno == ENOTDIR){
						files.erase (files.end()-1, files.end());
						status=remove((files).c_str());
					}
				}
			}
			if(S_ISREG(structure.st_mode)){
				remove(files.c_str());
				//display.print(VIEW_DEBUG)<< "Deleting file: "<< files <<endl;
			}

			if( (! S_ISREG(structure.st_mode)) && (! S_ISDIR(structure.st_mode) ) ){

				if( ! unlink(files.c_str() ) ){
					//display.print(VIEW_DEBUG)<<files<<"Deleted as link"<<endl;
				}else{
					display.print(VIEW_DEBUG)<<"Item "<<files<<endl;
					display.print(VIEW_DEBUG)<<"Item to delete is not file, directory or link "<<endl;
				}
			}

		}
		closedir(folderDir);
	}

}
void FFManager::deleteFolder(string folderPath){
	cleanFolder(folderPath);
	rmdir(folderPath.c_str());


}

vector<string> FFManager::getfiles(string folderPath){
	vector<string> filesVec;

	DIR *folderDir;
	struct dirent *next_file;
	string files;
	struct stat structure;


	folderDir= opendir(folderPath.c_str());
	while ( (next_file = readdir(folderDir)) != NULL ){
		files = string(next_file->d_name);

		if( files == string("..")){
			continue;
		}
		if(files == string(".")){
			continue;
		}
		stat((folderPath+ files).c_str(), &structure);

		if(S_ISREG(structure.st_mode)){
			filesVec.push_back(files);
		}

	}
	closedir(folderDir);

	return filesVec;
}

vector<string> FFManager::getfilesPrefix(string folderPath, string prefix){

	vector<string> filesVec;


	filesVec = getfiles(folderPath);


	for (auto filename = filesVec.begin(); filename != filesVec.end(); filename++) {
		if( (*filename).find(prefix) != 0){
			filesVec.erase(filename--); // Erase keeping validity of iterator
		}
	}

	return filesVec;


}

int FFManager::getNumber(string fileName, string prefix, string suffix){

	stringstream num;
	int time=0;

	fileName = fileName.substr(prefix.size(),fileName.size() - prefix.size()-suffix.size()); // Deleting prefix file, and extension plus units
	num << fileName<<endl;
	num >> time;
	num.str("");

	return time;
}


int FFManager::copyFolder(string &foldertocopy, string &folderSrc, string folderDst){

	DIR *folderDir;
	struct dirent *next_file;
	struct stat structure;
	string tmpStr, tmpStrPath, outStrPath, inputDir_str = folderSrc.c_str();

	string folderOut, folderIn;
	string files, fileIn, fileOut;

	folderIn = folderSrc+foldertocopy;
	folderOut = folderDst+foldertocopy;

	stat(folderIn.c_str(), &structure);

	if( S_ISDIR(structure.st_mode) == false){
		cout<<"Error copying library folder"<<endl;
		return 1;
	}

	if(createFolder(folderOut) == ERRORFOLDERCREATION ){
		cout<<"Error copying library folder"<<endl;
		return 1;
	}

	folderDir = opendir(folderIn.c_str());

	if( folderDir != NULL ) {
		while ( (next_file = readdir(folderDir)) != NULL ){
			files = string(next_file->d_name);

			if( files == string("..") || files == string(".") ){
				continue;
			}
			fileIn = folderIn + "/" + files;
			fileOut = folderOut + "/" + files;
			stat(fileIn.c_str(), &structure);
			if(S_ISREG(structure.st_mode)){
				if( copyfile(fileOut, fileIn) != 0 ){
					return 1;
				}
			}
			if(S_ISDIR(structure.st_mode)){ // Folder
				if( copyFolder(files, folderIn, fileOut) != 0){
					return 1;
				}
			}
		}
		closedir(folderDir);
	}

	return 0;
}

int FFManager::copyFolder2( string folderSrc, string folderDst){
	DIR *folderDir;
	struct dirent *next_file;
	struct stat structure;
	string tmpStr, tmpStrPath, outStrPath, inputDir_str = folderSrc.c_str();

	string folderOut, folderIn;
	string files, fileIn, fileOut;

	folderIn = folderSrc;
	folderOut = folderDst;

	stat(folderIn.c_str(), &structure);

	if( S_ISDIR(structure.st_mode) == false){
		cout<<"Error copying library folder"<<endl;
		return 1;
	}

	if(createFolder(folderOut) == ERRORFOLDERCREATION ){
		cout<<"Error copying library folder"<<endl;
		return 1;
	}

	folderDir = opendir(folderIn.c_str());

	if( folderDir != NULL ) {
		while ( (next_file = readdir(folderDir)) != NULL ){
			files = string(next_file->d_name);

			if( files == string("..") || files == string(".") ){
				continue;
			}
			fileIn = folderIn + "/" + files;
			fileOut = folderOut + "/" + files;
			stat(fileIn.c_str(), &structure);
			if(S_ISREG(structure.st_mode)){
				if( copyfile(fileOut, fileIn) != 0 ){
					return 1;
				}
			}
			if(S_ISDIR(structure.st_mode)){ // Folder
				if( copyFolder2(fileIn, fileOut) != 0){
					return 1;
				}
			}
		}
		closedir(folderDir);
	}

	return 0;
}

string FFManager::concFileGroup(string path,string prefix){

	vector<string> files;
	vector<int> times;
	ifstream filesIn;
	ofstream fileOut;
	stringstream ss;

	string preffix="Sensitivity";
	string suffix=".txt";

	files = getfilesPrefix(path, preffix);

	for(unsigned int i=0;i < files.size(); i++){
		times.push_back( getNumber(files.at(i), preffix, suffix) );
		cout<<"File before ordering: "<< files.at(i) <<endl;
	}
	sort(times.begin(), times.end() );
	for(unsigned int i=0;i < times.size(); i++){
		ss<<times.at(i);
		files.at(i) = preffix+ss.str()+suffix;
		ss.str("");
	}





	fileOut.open((path+prefix+".txt").c_str(),  ios::out | ios::binary);

	for(unsigned int i=0;i < files.size(); i++){
		if ( files.at(i).find(prefix) != std::string::npos){
			cout<<"Copy from: "<<files.at(i)<<endl;
			filesIn.open((path+files.at(i)).c_str(), ios::in | ios::binary);
			fileOut<< filesIn.rdbuf();
			filesIn.close();
			//deletefile(files.at(i));
		}
	}

	fileOut.close();



	return string("");
}



