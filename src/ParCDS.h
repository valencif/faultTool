/*
 * ParCDS.h
 *
 *  Created on: Jul 19, 2019
 *      Author: andres
 */


/**
 * \class ParCDS
 *
 *
 * \brief PAR cadence
 *
 * Implementation of PARouter for Cadence program
 *
 * \author Andrés Felipe Valencia
 * \version 0.1
 * \copyright GNU Public License.
 * \date : 2019/08/26 16:00:00
 * Contact: valena@usi.ch
 * Created on: Feb 27, 2019
 * \copyright GNU Public License.
 *
 */

#ifndef PARCDS_H_
#define PARCDS_H_

#include "PARouter.h"
#include <fstream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>     // atoi
#include "pipeBridge.h"

#include <sstream>



class ParCDS: public PARouter {
private:
	string scriptName; // Script name
	float timeSynCpy;
public:
	ParCDS();
	virtual ~ParCDS();

	int generateScript(string &templatefile );
	void setDesignName(string &templatefile);
	int generateViewScript();
	int placeRouteModule();
	void translateTags(string &line);
	void setTimeSyn(float time);
	int checkErrors();
	void obtainedInfo(string &entityName);
	void printInfo();
	void cleanFiles();
	void cleanTmpFile();

	void setModelRef(ModelHW *modelPtr);

	string getDesignName();
};

#endif /* PARCDS_H_ */
