/**
 * \class NonZeroSM
 *
 *
 * \brief Non zero model implementation the interface SensitivityModel
 *
 * Class that contains the software model providing functions for generating
 * stimuli.
 *
 * \author Andrés Felipe Valencia
 * \version 0.1
 * \copyright GNU Public License.
 * \date : 2019/08/26 16:00:00
 * Contact: valena@usi.ch
 * Created on: Sep 13, 2019
 *
 */

#ifndef NONZEROSM_H_
#define NONZEROSM_H_

#include "../../src/SensitivityModel.h"


class NonZeroSM: public SensitivityModel  {
public:
	NonZeroSM();
	virtual ~NonZeroSM();

	int getSensitivity(int inter);
};

extern "C" {
	SensitivityModel *createSensitivityModel() {
		return new NonZeroSM();
	}
	void destroyModel(SensitivityModel* m) {
		delete m;
	}
}


#endif /* NONZEROSM_H_ */
