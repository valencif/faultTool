/*
 * Synthesizer.cpp
 *
 *  Created on: Mar 28, 2019
 *      Author: valencif
 */

#include "Synthesizer.h"

Synthesizer::Synthesizer() {
	area = 0;
	gates = 0;
	criticalPath = 0;
	timeSyn = 0;
	slack=0;
	areaNAND=0;

}

Synthesizer::~Synthesizer() {

}

void Synthesizer::setTime(float timeSyn){
	this->timeSyn=timeSyn;
}
