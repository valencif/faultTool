
/**
 * \class SynSNPS
 *
 *
 * \brief Implementation for synthesizer for design compiler
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


#ifndef SYNSNPS_H_
#define SYNSNPS_H_

#include <stdio.h>
#include <sstream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <cstdlib>

#include "Synthesizer.h"




class SynSNPS: public Synthesizer {
private:
	string scriptName; // Script name
public:
	SynSNPS();
	virtual ~SynSNPS();

    //! A pure virtual member.
    /*!
      \sa testMe()
      \param c1 the first argument.
      \param c2 the second argument.
    */
	int generateScript(string &templatefile,ModelHW &model);
	void setDesignName(string &templatefile,ModelHW &model);
	void translateTags(string &line, ModelHW &model);

	int createGateLevelModule();
	float checkErrorsTime(string &entityName); // return slack in ns
	int checkErrors();
	void obtainedInfo(string &entityName);
	void printInfo();
	void findMinTime();
	void cleanFiles();
	float getSlack();
	void setAreaMinimumGate(float nandarea);

	string getDesignName();

};

#endif /* SYNSNPS_H_ */
