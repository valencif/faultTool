
/**
 * \class Synthesizer
 *
 *
 * \brief Interface for hardware synthesizer
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


#ifndef SYNTHESIZER_H_
#define SYNTHESIZER_H_


#include <iostream>   // std::cout
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <string>     // std::string, std::stof

#include "FFManager.h"
#include "ModelHW.h"
#include "View.h"
#include "pipeBridge.h"

// Visualization class
extern View &display;

using namespace std;

class Synthesizer {

protected:
	float area; // Area of gate level circuit
	float areaNAND;
	float gates; // Gate equivalent
	float criticalPath; // Arrival time
	float timeSyn; // Period for synthesis
	float slack; // Already expressed in the synthesys units

	string designName; ///< Baseline for the output files of synthesis

public:
	Synthesizer();
	virtual ~Synthesizer();

	virtual int generateScript(string &templatefile, ModelHW &model)=0;
	virtual int createGateLevelModule()=0;
	virtual float checkErrorsTime(string &entityName)=0;
	virtual int checkErrors()=0;
	virtual void obtainedInfo(string &entityName)=0;
	virtual void printInfo()=0;
	virtual void findMinTime()=0;
	virtual void cleanFiles()=0;
	virtual void setAreaMinimumGate(float nandarea)=0;
	virtual void setDesignName(string &templatefile,ModelHW &model)=0;
	//virtual void setSYNscript(string &script)=0;
	virtual string getDesignName()=0;

	void setTime(float timeSyn);


};

#endif /* SYNTHESIZER_H_ */
