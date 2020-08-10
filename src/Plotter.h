/**
 * \class Plotter
 *
 *
 * \brief Interface for plotting the information of the FSA results
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

#ifndef PLOTTER_H_
#define PLOTTER_H_

#include <iostream>
#include <string>
#include <sstream>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>


#include "FFManager.h"

using namespace std;

class Plotter {
protected:
	string scriptResults;
	string scriptTemplate;
public:
	Plotter();
	virtual ~Plotter();

	virtual void generateScript(string scriptFSAFN, string scriptTemplFN)=0;
	virtual void plotFSAresults(string resultsPath, string variablePath, int corrKey)=0;
	virtual void plotTemplate(string resultsPath, string variablePath)=0;
};

#endif // PLOTTER_H_
