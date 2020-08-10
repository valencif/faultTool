/**
 * \class MinimumSMT
 *
 *
 * \brief Sensitivity model implemented using template with minimum as agregation function
 *
 * \author Andrés Felipe Valencia
 * \version 0.1
 * \copyright GNU Public License.
 * \date : 2019/08/26 16:00:00
 * Contact: valena@usi.ch
 * Created on: Sep 13, 2019
 */

#ifndef MINIMUMSMT_H_
#define MINIMUMSMT_H_

#include "../../src/SensitivityModelTemplate.h"

class MinimumSMT: public SensitivityModelTemplate {
public:
	MinimumSMT();
	virtual ~MinimumSMT();

	int aggregationFnc(vector<int> vec) ;
	int getSensitivity(int inter);


};


extern "C" {
	SensitivityModel *createSensitivityModel() {
		return new MinimumSMT();
	}
	void destroyModel(SensitivityModel* m) {
		delete m;
	}
}


#endif // MINIMUMSMT_H_ 
