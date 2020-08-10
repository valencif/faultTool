

/**
 * \class ModelHW
 *
 *
 * \brief Hardware model
 *
 * This class contains all information about the hardware implementation of the module.
 * It also contains functions to get information from HDL sources
 *
 * \author Andrés Felipe Valencia
 * \version 0.1
 * \copyright copyright
 * \date : 2019/08/26 16:00:00
 * Contact: valena@usi.ch
 * Created on: Feb 27, 2019
 * \warning   It is tested only using vhdl code. It assumes very strict rules about the vhdl code
 * \copyright GNU Public License.
 *
 */


#ifndef MODELHW_H_
#define MODELHW_H_

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>




#define HAVE_WORKING_REGEX 0

#if HAVE_WORKING_REGEX==1
#include <regex>
using namespace std::tr1;
#endif

#include "FFManager.h"

// Visualization class
extern View &display;

using namespace std;

// Assumptions about module interface:
// Interface done in VHDL
// The interface starts with 1 line "entity <entityName> is"
// Then generic or port sections are written with the same format differing only in the keywords: generic and port
// The keywords generic and port are in the same line with the symbol '(', in this line no signal is specified
// Every signal is specified in a single line
// Closing the sections generic and port is done with the symbol ');' alone in one line
// Except clock and reset the rest of the inputs are organized input1, .., inputn, key

class ModelHW {
private:
	string sourceFolder; ///< Folder with all sources
	// File containing the name of all source files, the order dictates the hierarchy
	// last file is the highest in the hierarchy
	string fileWithSrc;
	int numberSrc; ///< Number of sources
	vector<string> sources; ///< Name of every source file
	string architectureName; ///< Name of the first architecture
	string entityName; ///< Name of the entity
	string testbenchName; ///< Name of the testbench
	string testbenchNameGate; ///< Name of the testbench for gate level design
	string architectureNameTb; ///< Name of the first architecture in the tb
	string entityNameTb; ///< Name of the testbench entity

	// ToDo: Get clock and reset from file, so far it is fixed to 'ClkxCI' and ''
	string clockName; ///< Name of the clock signal (First signal with the string clk)
	string rstName; ///< Name of the reset signal (First signal with the string rst)
	vector<string> inputs;
	vector<string> outputs;
	vector<string> generic;

public:
	ModelHW();
	virtual ~ModelHW();

	void setSources(string &sourceFolder, string &fileWithSrc);
	int captureInfo();
	string getArchitectureName();// Returns the first architecture
	string getArchitectureNameVHDL(string &filename); // Returns the first architecture
	//ToDo: string getArchitectureNameV();
	//ToDo: Specialize getEntity for  verilog
	string getEntityName();
	string getEntityNameVHDL(string &filename);
	int obtaineArcEntVHDL(string &filename,string &entity, string &architecture); // Obtains architecture and entity from a VHDL file
	int getModuleInfo(); // Obtains signal names from the module interface
	void getPreDefinedTB(string & pathSrc, string &filename);
	void genTestBench(string &filename);
	// ToDo: Implement testBench reading the meta info of the ports

	void setGateTB(string &pathSrc,string &fileGateTB);
	string getGateTB();

	vector<string> &getSources();
	string getTestbenchName();
	string getEntityTb();
	string getClockName();
};

#endif

// ToDo: Document this class
