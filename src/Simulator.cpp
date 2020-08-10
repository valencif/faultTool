/*
 * Simulator.cpp
 *
 *  Created on: Feb 27, 2019
 *      Author: valencif
 */

#include "Simulator.h"

/*Simulator::Simulator() {


}*/

Simulator::~Simulator() {

}

void Simulator::setTimeGate(float maxTimeGate, float minTimeGate,float stepTimeGate){

	this->maxTimeGate = maxTimeGate;
	this->minTimeGate = minTimeGate;
	this->stepTimeGate = stepTimeGate;
}

void Simulator::setAnalyzedIn(	int analyzedInputFSA, int analyzedInputFSAT){

	this->analyzedInputFSA = analyzedInputFSA;
	this->analyzedInputFSAT = analyzedInputFSAT;
}

void Simulator::setSenSimChunk(int shunkSize){
	senSimChunk = shunkSize;
}
