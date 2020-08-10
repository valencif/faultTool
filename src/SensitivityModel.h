
/**
 * \class SensitivityModel
 *
 *
 * \brief Interface to implement sensitivity models
 *
 * This interface sets the necessary function to work with a sensitivity model.
 * It may contain non-pure virtual functions
 *
 * \author Andrés Felipe Valencia
 * \version 0.1
 * \copyright GNU Public License.
 * \date : 2019/08/26 16:00:00
 * Contact: valena@usi.ch
 * Created on: Sep 13, 2019
 *
 */


#ifndef SENSITIVITY_MODLE_H_
#define SENSITIVITY_MODLE_H_

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include "FFManager.h"

#include "View.h"

// Visualization class
extern View &display;

using namespace std;

class SensitivityModel {
protected:
	string _modelType;
	int templateModel;

public:
	SensitivityModel();
	virtual ~SensitivityModel();
	void printModelType();
	string getModelType();
	int isTemplate();
	virtual int getSensitivity(int inter)=0;
};

#endif
