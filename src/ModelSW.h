/**
 * \class ModelSW
 *
 *
 * \brief Software model
 *
 * Class that contains the software model providing functions for generating
 * stimuli.
 *
 * \author Andrés Felipe Valencia
 * \version 0.1
 * \copyright GNU Public License.
 * \date : 2019/08/26 16:00:00
 * Contact: valena@usi.ch
 * Created on: 2019/08/26 16:00:00
 *
 */


#ifndef MODEL_H_
#define MODEL_H_

#include <iostream>
#include <stdlib.h>
#include <dlfcn.h>

#include "FFManager.h"
#include "View.h"

#define STIMULI_NOT_CREATED 0
#define STIMULI_CREATED 1
#define STIMULI_EXISTS 2

// Visualization class
extern View &display;

using namespace std;

// ToDo: Capture more responsibilities from the specific model
class ModelSW {
protected:
	int numInputs; ///< Numbers of inputs of the systems, without counting the key
	int numTracesFSA; ///< Number of the traces generated for the FSA analysis
	int numTracesFSAT; ///< Number of the traces generated for the FSA Template analysis
	string __ModleType;

public:
	ModelSW();
	virtual ~ModelSW();

	virtual void setNumTraces(int FSA, int FSAT);
	virtual int setInput(int index, int value);
	virtual int setKey( int value);
	virtual int getOuput();
	virtual int getMaxIntermediate()=0; ///< Indicates the range of possible intermediates from 0 to return value-1
	virtual int getMaxKey()=0; ///< Indicates the range of possible key from 0 to return value-1
	virtual int getOuputPos()=0; ///< Get position of the output in the stimuli file starting from 1
	virtual void loadModelInfo(string stimuliFSA, string stimuliFSAT)=0;
	int getInput(); ///< Get number of inputs without counting the key
	int getFSAN(); ///< Get the number of traces for FSA
	int getFSAT(); ///< Get the number of traces for FSA template

	void getStimuliData(ifstream &file, int *inputs, int *output, int len, int startIndex=0 );
	void freeMemory(int **inputs, int *output, int len );

	// The stimuli for combinational models are created for the format
	// input1 ... input2 key output
	virtual int createStimuliFSA(string &filename);
	virtual int createStimuliFSAT(string &filename);

	int fileExists(string &filename);

	virtual int getFixedkey();

	//Load a type of model dynamically and create a pointer
	// ToDo: Create function that accept more sources of immediately the compiling code
	static ModelSW* loadModelDinamically(string source, string libFile);



};

// the types of the class factories
//using Base_creator_t = ModelSW *(*)();

#endif /* MODEL_H_ */
