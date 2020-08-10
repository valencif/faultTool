
/**
 * \class SensitivityCtrl
 *
 *
 * \brief Thread control for sensitivity simulation
 *
 * This class controls the thread execution of the simulation for
 * obtaining the sensitivity values.
 *
 * \author valencif
 * \version 0.1
 * \copyright GNU Public License.
 * \date $Date: 2019/08/26 16:00:00 $
 * Contact: valena@usi.ch
 * Created on: 2019/08/26 16:00:00
 *
 */

#ifndef SENSITIVITYCTRL_H_
#define SENSITIVITYCTRL_H_

#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#include "FFManager.h"
#include "pipeBridge.h"


using namespace std;

#define SEN_CTRL_STATUS_IDLE 0
#define SEN_CTRL_STATUS_WAITING 1
#define SEN_CTRL_STATUS_PROC 2
#define SEN_CTRL_STATUS_PROC_END_CYCLE 3
#define SEN_CTRL_STATUS_END 4
#define SEN_CTRL_STATUS_DESTROY 5

class SensitivityCtrl {
private:
	static int counter;
	int procId;
	int pipeIn; ///< Pipe that receives data from simulator
	int pipeOut; ///< Pipe to send information to the simulator
	int status;
	int inputsNum; ///< Number of inputs
	int dimInputs; ///< Dimension of every input
	int maxTime; ///< Maximum limit of the exploration range
	int minTime; ///< Minimum limit of the exploration range
	int stepTime; ///< Time step of the exploration range

	//int **matrixInput; ///< Matrix for saving inputs for the specific set of vector
	int *MaxCycle; ///< Vector for saving the tested cycles. Expressed in ps instead of ns
	int *MinCycle; ///< Vector for saving the tested cycles. Expressed in ps instead of ns
	int *vectorSensitvity; ///< Vector for recording the progress of found sensitivity.

	int simTime;

	string inBuffer;
	string tbname; // Name of the testbench
	string script;

	int indexIn; // Show the position of the input in the complete input set
	int **inputMatrix;
	int *outputVector;

	pipeBridge * bridge;

	int foundSen, foundIndex,currPer;

	void run();
public:
	SensitivityCtrl();
	virtual ~SensitivityCtrl();

	void setPipes(int pipesIn, int pipeOut);
	void setLimits(float critPath, float minTime, float timeStep, int NumInputs, int dimInputs);
	void simCycle(int addWave=0, int phase=0);
	void simIteration(int addWave, int phase, int time );
	int readInLine(string &line);
	int lineContains(string &line, string substr);
	int allSensitivityFound();
	void setInput( int *inputs,  int *output, int indexIn, int phase = 0);
	int getStatus();
	void update();
	void updatePar();
	void updatePar2();
	void exitMGM();
	void freeSpace();
	void setTestBenchName(string tb);
	void setScript(string script);


	void initVectors();
	void initialize();

	void initialize3();
	void setInput3( int *inputs,  int *output, int indexIn, int phase = 0);
	void updatePar3();

	static int getFreeController(SensitivityCtrl *ctrls, int len);
	static int allControllerFree(SensitivityCtrl *ctrls, int len);

};



#endif /* SENSITIVITYCTRL_H_ */
