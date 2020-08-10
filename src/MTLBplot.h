/**
 * \class MTLBplot
 *
 *
 * \brief Implementation of Plotter that uses Matlab script to obtain the information
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


#ifndef MTLBPLOT_H_
#define MTLBPLOT_H_

#include "Plotter.h"

class MTLBplot: public Plotter {
public:
	MTLBplot();
	virtual ~MTLBplot();

	void generateScript(string scriptFSAFN, string scriptTemplFN);
	void plotFSAresults(string resultsPath, string variablePath, int corrKey);
	void plotTemplate(string resultsPath, string variablePath);
};

#endif /* MTLBPLOT_H_ */
