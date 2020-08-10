/**
 * \class ConfigFSA
 *
 *
 * \brief Class that manages the phases and common information for performing the FSA analysis.
 *

 *
 * \author Andrés Felipe Valencia
 * \version 0.1
 * \copyright copyright
 * \date : 2019/08/26 16:00:00
 * Contact: valena@usi.ch
 * Created on: Sep 6, 2019
 * \warning
 * \copyright GNU Public License.
 *
 */


#ifndef CONFIGFSA_H_
#define CONFIGFSA_H_


#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;


class ConfigFSA {
private:
	string filename; ///< File with the configuration for performing the FSA analysis
	ifstream file; ///< Configuration file

	// Project parameters
	vector<string> parStrNames;
	vector<string> parStr;

	vector<string> parIntNames;
	vector<int> parInt;

	vector<string> parFloatNames;
	vector<float> parFloat;


	int phasesExec[10]; ///< Executed processes
	vector<string> phasesNames;


	// Auxiliary methods
	void trimLine(string &line);
	int getPar(string &line, string parName);
	void parseLine(string &line);

public:

	ConfigFSA();
	virtual ~ConfigFSA();

	void setConfigFile(string filename);
	void load();

	string getParStr(string parName);
	int getParInt(string parName);
	float getParFloat(string parName);

	void setParStr(string parName, string par);
	void setParInt(string parName, int par);
	void setParFloat(string parName, float par);

	void setPhase(int par);
	int isPhaseExec(int i);


};

#endif // CONFIGFSA_H_
