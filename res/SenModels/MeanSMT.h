/**
 * \class MeanSMT
 *
 *
 * \brief Sensitivity model implemented using template with mean as agregation function
 *
 * \author Andrés Felipe Valencia
 * \version 0.1
 * \copyright GNU Public License.
 * \date : 2019/08/26 16:00:00
 * Contact: valena@usi.ch
 * Created on: Sep 13, 2019
 */

#ifndef MEANSMT_H_
#define MEANSMT_H_

#include "../../src/SensitivityModelTemplate.h"

class MeanSMT: public SensitivityModelTemplate {
public:
	MeanSMT();
	virtual ~MeanSMT();

	int aggregationFnc(vector<int> vec) ;
	int getSensitivity(int inter);


};


extern "C" {
	SensitivityModel *createSensitivityModel() {
		return new MeanSMT();
	}
	void destroyModel(SensitivityModel* m) {
		delete m;
	}
}


#endif /* MEANSMT_H_ */
