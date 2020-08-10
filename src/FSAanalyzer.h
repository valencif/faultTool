/**
 * \class FSAanalyzer
 *
 *
 * \brief Class to perform FSA on the obtained data
 *
 * This class load all models and perform FSA using the software model and the generated data
 *
 * \author Andrés Felipe Valencia
 * \version 0.1
 * \copyright GNU Public License.
 * \date : 2019/08/26 16:00:00
 * Contact: valena@usi.ch
 * Created on: Sep 13, 2019
 *
 */

#ifndef FSAANALYZER_H_
#define FSAANALYZER_H_

#include <iostream>
#include <string>
#include <vector>
#include <cmath>

#include "View.h"
#include "SMFactory.h"
#include "ModelSW.h"
#include "SensitivityModel.h"
#include "SensitivityModelTemplate.h"


using namespace std;


// Visualization class
extern View &display;

class FSAanalyzer {
	string pathModels;
	SMFactory *modelFac;
	ModelSW *swm;
	string senFilename;
	string senTempFilename;
	int phase;
	vector<SensitivityModel*> models;
	int updateModels;
public:
	FSAanalyzer();
	virtual ~FSAanalyzer();
	void setModelsPath(string path);
	void loadModels();
	void setSWModel(ModelSW *swm);
	void setFilenames();
	void setAnalysis(int phase);
	void genTemplateModels();
	void genIntermediates(); // Generate general intermediates.
	void estimatedSenForAllModels(); ///< Converts the intermediate file into a file with the estimated sensitivity
	void getCorrelationForAllModels(int precisionEvol); ///< Obtained the correlation for all present models
	void getCorrelation(int precisionEvol, string estimatedSenFN, string measuredSenFN, string corrEvoFN ); ///< Obtained the correlation
	vector<float> correlationEvo(vector<int> A, vector<int> B, int precisionEvol);
	void cleanResults();
	void updateAllModels();
};

#endif /* FSAANALYZER_H_ */
