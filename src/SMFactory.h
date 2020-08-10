
/**
 * \class SMFactory
 *
 *
 * \brief Factory for creating sensitivity model
 *
 * Class in charge of creating all sensitivity models, it also contains a vector with one
 * instantiation of every model
 *
 * \author Andrés Felipe Valencia
 * \version 0.1
 * \copyright GNU Public License.
 * \date : 2019/08/26 16:00:00
 * Contact: valena@usi.ch
 * Created on: Sep 13, 2019
 *
 */

#ifndef SMFACTORY_H_
#define SMFACTORY_H_

#include <dlfcn.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <stdlib.h>



#include "SensitivityModel.h"
#include "FFManager.h"
#include "View.h"


typedef SensitivityModel* (*createSM)(void);


class SMFactory {
private:
	map <string, createSM> factory;
	vector<SensitivityModel*> models;
public:
	SMFactory();
	virtual ~SMFactory();
	int loadModels(string path);
	SensitivityModel* createModel(string name);
	vector<SensitivityModel*> getAllModelsPtr();

};

#endif /* SMFACTORY_H_ */
