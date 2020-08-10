/*
 * addMod12289.h
 *
 *  Created on: Feb 22, 2019
 *      Author: valencif
 */

#ifndef XOR_SBOX_H_
#define XOR_SBOX_H_ 

#include "../src/ModelSW.h"


using namespace std;

class xorSbox: public ModelSW {
private:
	int *inputContainer; // Array for saving a set of input
	int keyContainer;
	int fixed_key;
	

public:
	xorSbox();
	virtual ~xorSbox();

	void setNumTraces(int FSA, int FSAT) override;
	void loadModelInfo(string stimuliFSA, string stimuliFSAT);

	int setInput(int index, int value);
	int setKey( int value);
	int getOuput();
	int getOuputPos();
	int createStimuliFSA(string &filename);
	int createStimuliFSAT(string &filename);
	int getFixedkey();
	int getMaxIntermediate();
	int getMaxKey();
};


extern "C" {
	ModelSW * createModel() {
		return new xorSbox();
	}
	void destroyModel(ModelSW* m) {
		delete m;
	}
}



#endif // XOR_SBOX_H_ 
