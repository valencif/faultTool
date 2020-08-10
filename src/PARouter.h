
/**
 * \class PARouter
 *
 *
 * \brief Interface for hardware place and router
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

#ifndef PAROUTER_H_
#define PAROUTER_H_

#include <string>     // std::string, std::stof
#include <sstream>
#include "ModelHW.h"
#include "FFManager.h"

using namespace std;

class PARouter {
protected:
	ModelHW *modelPtr; // Pointer to the hardware model
	string designName;
public:
	PARouter();
	virtual ~PARouter();
	virtual int generateScript(string &templatefile)=0;
	virtual void setDesignName(string &templatefile)=0;
	virtual int placeRouteModule()=0;
	virtual int checkErrors()=0;
	virtual void obtainedInfo(string &entityName)=0;
	virtual void printInfo()=0;
	virtual void cleanFiles()=0;
	virtual void setModelRef(ModelHW *modelPtr)=0;

	virtual string getDesignName()=0;

};

#endif /* PAROUTER_H_ */
