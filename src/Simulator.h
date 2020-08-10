
/**
 * \class Simulator
 *
 *
 * \brief Interface for hardware simulator
 *

 *
 * \author Andrés Felipe Valencia
 * \version 0.1
 * \copyright copyright
 * \date : 2019/08/26 16:00:00
 * Contact: valena@usi.ch
 * Created on: Feb 27, 2019
 * \warning
 * \copyright GNU Public License.
 *
 */



#ifndef SIMULATOR_H_
#define SIMULATOR_H_

#include <iostream>
#include <string>
#include <vector>
#include "ModelHW.h"
#include "ModelSW.h"
#include "FFManager.h"

#include <unistd.h>
#include <sys/wait.h>
#include <thread>

// Visualization class
extern View &display;


#define VHD_SRC 1
#define V_SRC 2
#define SIM 3
#define SIM_CR 4
#define SIM_CS 5
#define SIM_CP 6

#define DATA_LIB  100
#define DATA_FLAGS 101
#define DATA_SDF_MIN 102
#define DATA_SDF_TYP 103
#define DATA_SDF_MAX 104


using namespace std;


//ToDo: Implement factory for extending when other simulator rules are needed

// Interface for Simulator
class Simulator {
protected:
	string cmdVHDL; ///<  command for compiling VHDL files
	string cmdVER; ///<  Command for compiling verilog files
	string cmdSIM; ///<  Command for calling simulator
	string cmdSIM_C_RTL; ///<  Complete command for simulation
	string cmdSIM_C_SYN; ///<  Complete command for simulation
	string cmdSIM_C_PAR; ///<  Complete command for simulation
	vector<string> libs; ///<  Paths to the libraries used in simulation
	string flags; ///<  string containing all simulation flags
	string sdfMinPath; ///<  Path with the sdf information (Min)
	string sdfTypPath; ///<  Path with the sdf information (Typ)
	string sdfMaxPath; ///<  Path with the sdf information (Max)
	ModelHW *modelHWPtr; ///<  Pointer to the hardware model
	ModelSW *modelSWPtr; ///<  Pointer to the software model

	string designName; ///< Base for building the gate and timing files

	string scriptRTL; ///<  Name of the rtl script
	string scriptGATE; ///<  Name of the rtl script
	string scriptGATEPR; ///<  Name of the rtl script

	float maxTimeGate; ///< Maximum time for sensitivity range in ns
	float minTimeGate; ///< Maximum time for sensitivity range in ns
	float stepTimeGate; ///< Maximum time for sensitivity range in ns

	int senSimChunk; ///< Size of the chunks for sensitivity simulations

	int analyzedInputFSA;  ///< Progress of inputs when analysing sensitivity for FSA
	int analyzedInputFSAT;  ///< Progress of inputs when analysing sensitivity for FSA template


public:
	// Simulator(); //
	virtual ~Simulator();

	virtual int generateRTLScript(string &filename) = 0; // Source and destination paths are fixed,
	virtual int generateSYNScript(string &filename, string &source) = 0;
	virtual int generatePARScript(string &filename, string &source) = 0;

	virtual int runSimulationRTL(int child_proc=1) = 0;
	virtual int runSimulationSYN(int child_proc=1, int FSASTI=0) = 0;
	virtual int runSimulationPAR(int child_proc=1, int FSASTI=0) = 0;

	virtual int checkErrorRTL() = 0;
	virtual int checkErrorSYN() = 0;
	virtual int checkErrorPAR() = 0;

	virtual void setModelHWRef(ModelHW *modelPtr)=0;
	virtual void setModelSWRef(ModelSW *modelPtr)=0;

	void setTimeGate(float maxTimeGate, float minTimeGate,float stepTimeGate);
	void setAnalyzedIn(	int analyzedInputFSA, int analyzedInputFSAT);
	void setSenSimChunk(int shunkSize);

	virtual void setSYNscript(string &script)=0;

};

#endif /* SIMULATOR_H_ */
