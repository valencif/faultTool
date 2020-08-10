/**
 * \class MaximumSMT
 *
 *
 * \brief Sensitivity model implemented using template with maximum as agregation function
 *
 * \author Andrés Felipe Valencia
 * \version 0.1
 * \copyright GNU Public License.
 * \date : 2019/08/26 16:00:00
 * Contact: valena@usi.ch
 * Created on: Sep 13, 2019
 */

#ifndef MAXIMUMSMT_H_
#define MAXIMUMSMT_H_

#include "../../src/SensitivityModelTemplate.h"

class MaximumSMT: public SensitivityModelTemplate {
public:
	MaximumSMT();
	virtual ~MaximumSMT();

	int aggregationFnc(vector<int> vec) ;
	int getSensitivity(int inter);


};


extern "C" {
	SensitivityModel *createSensitivityModel() {
		return new MaximumSMT();
	}
	void destroyModel(SensitivityModel* m) {
		delete m;
	}
}


#endif /* MAXIMUMSMT_H_ */
