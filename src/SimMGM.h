
/**
 * \class SimMGM
 *
 *
 * \brief Implementation of simulation for Modelsim program
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


#ifndef SIMMGM_H_
#define SIMMGM_H_

#include "Simulator.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "FFManager.h"
#include "SensitivityCtrl.h"
#include "pipeBridge.h"


//#define _XOPEN_SOURCE 600
/*#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>

#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>

#include<unistd.h>
#include<sys/wait.h>
#include<sys/prctl.h>
#include<signal.h>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>*/

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

class SimMGM: public Simulator {

public:
	SimMGM();
	virtual ~SimMGM();

	int generateRTLScript( string &filename  );
	int generateSYNScript(string &filename, string &source);
	int generatePARScript(string &filename, string &source);




	int runSimulationRTL(int child_proc=1);
	int runSimulationSYN(int child_proc=1, int FSASTI=0);
	int runSimulationPAR(int child_proc=1, int FSASTI=0);

	int runSimulationPAR2(int child_proc=1, int FSASTI=0); //


	int generatePARScript3(string &filename, string &source);
	int runSimulationPAR3(int child_proc=1, int FSASTI=0); //

	int checkErrorRTL();
	int checkErrorSYN();
	int checkErrorPAR();

	void setCMDDATA(int index, string &cmd);
	void updateInfoFromScript( string &filename  );
	void setModelHWRef(ModelHW *modelPtr);
	void setModelSWRef(ModelSW *modelPtr);
	void translateVSIM(string &cmd);
	void copyCompiledLib(string &folderSrc);
	void setSYNscript(string &script);
};

#endif /* SIMMGM_H_ */
