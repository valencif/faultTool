/**
 * \class SensitivityModelTemplate
 *
 *
 * \brief Interface to implement sensitivity models that are created as a template
 *
 * This interface extends SentivityModel class to include models that have been created with template
 *
 * \author Andrés Felipe Valencia
 * \version 0.1
 * \copyright GNU Public License.
 * \date : 2019/08/26 16:00:00
 * Contact: valena@usi.ch
 * Created on: Sep 13, 2019
 *
 */

#ifndef SENSITIVITYMODELTEMPLATE_H_
#define SENSITIVITYMODELTEMPLATE_H_

#include "SensitivityModel.h"

class SensitivityModelTemplate: public SensitivityModel {
protected:
	string templateFile;
	int *templateM;
	int maxIntermediate;
	int loaded;
public:
	SensitivityModelTemplate();
	virtual ~SensitivityModelTemplate();

	virtual int aggregationFnc(vector<int> vec) = 0;

	void generateTemplate(string stimuliTFN, int posIntermeadia, string sensitivity, int phaseFSA) ;
	void loadTemplate(string filename);
	void unloadTemplate();
	void setMaxIntermediate(int value);
};

#endif // SENSITIVITYMODELTEMPLATE_H_
