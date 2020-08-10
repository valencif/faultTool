//============================================================================
// Name        : FSA.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//============================================================================

/*! \mainpage FSA analyzer
 *
 * \section intro_sec Introduction
 *
 * This program is intended to analyzed a hardware module agaisnt fault sensitivity analysis.
 * It calls CAD tools used in the circuit desing process
 */

#include <iostream>
#include <stdlib.h>
#include <memory>
#include <math.h>       // ceil
#include <string>
#include <ctime>


#include "FFManager.h"
#include "ModelSW.h"
#include "ModelHW.h"
#include "SimMGM.h"
#include "SynSNPS.h"
#include "ParCDS.h"
#include "ConfigFSA.h"
#include "View.h"
#include "FSAanalyzer.h"
#include "MTLBplot.h"


using namespace std;

// ToDo: Create VHDLmodule and VerilogModule, class that take parse the code of module, without checking functionality

View &display = View::instance();

int main(void) {


	ConfigFSA *configuration;

	int numTracesFSA, numTracesFSAT;
	ModelSW *swm;
	int aux;
	string stimulifileFSA="";
	string stimulifileFSAT="";
	string projectName;
	string projectFolder;
	int timeInit, timeEnd;


	// ************************************************************
	// Initialize common modules
	FFManager &folderfiles = FFManager::instance();
	string configfilename="FSAConfig.txt";


	// Read configuration parameters
	configuration = new ConfigFSA();
	configuration->setConfigFile(configfilename);
	configuration->load();


	// Manage debug messages
	display.setDebug( configuration->getParInt(string("debug")) );


	// ************************************************************
	// Generate folders
	projectName = configuration->getParStr(string("projectName"));
	projectFolder = string("../")+projectName+"/";
	folderfiles.setProjectPath(projectFolder);

	timeInit = time(0);
	if(configuration->isPhaseExec(0) == 0){

		folderfiles.deleteFolder(projectFolder);
		if( folderfiles.createProjectFolder(projectName) == ERRORFOLDERCREATION){
			display.print(VIEW_ERROR)<<"Error main folder creation"<<endl;
		}
		if( folderfiles.createFolderStructure() == ERRORFOLDERCREATION){
			display.print(VIEW_ERROR)<<"Error folder structure creation"<<endl;
		}
		configuration->setPhase(0);
		display.print(VIEW_COMMENT)<<"File structure was created"<<endl;
	}else{
		display.print(VIEW_WARNING)<<"File structure was not updated"<<endl;
	}

	timeEnd = time(0);
	display.print(VIEW_COMMENT) <<"Time for creating folder structure "<< difftime( timeEnd, timeInit)  << " seconds"<<endl;

	// ************************************************************
	// Load SW model dynamically
	string templateFolder;
	string ccpSrc;
	string libFile;

	templateFolder = configuration->getParStr(string("templateFolder"));
	ccpSrc = templateFolder+projectName+".cpp";
	libFile = templateFolder +projectName+".so";
	swm=ModelSW::loadModelDinamically(ccpSrc, libFile);



	// ************************************************************
	// Create stimuli file
	numTracesFSA = configuration->getParInt(string("numTracesFSA"));
	numTracesFSAT = configuration->getParInt(string("numTracesFSAT"));


	swm->setNumTraces(numTracesFSA,numTracesFSAT);
	stimulifileFSA = folderfiles.getStimuliFileFSA();
	stimulifileFSAT = folderfiles.getStimuliFileFSAT();
	display.print(VIEW_NORMAL)<<"Stimuli file for testing the attack: "<<stimulifileFSA<<endl;
	display.print(VIEW_NORMAL)<<"Stimuli file for generating the template: "<<stimulifileFSAT<<endl;


	timeInit = time(0);
	if(configuration->isPhaseExec(1) == 0){
		aux=swm->createStimuliFSA(stimulifileFSA);
		if(!aux){
			display.print(VIEW_ERROR)<<"Error creating testing stimuli"<<endl;
			return -1;
		}
		aux=swm->createStimuliFSAT(stimulifileFSAT);
		if(!aux){
			display.print(VIEW_ERROR)<<"Error creating template stimuli"<<endl;
			return -1;
		}

		configuration->setPhase(1);
		display.print(VIEW_COMMENT)<<"Stimuli files were  created"<<endl;
	}else{
		swm->loadModelInfo(stimulifileFSA, stimulifileFSAT);
		display.print(VIEW_WARNING) << "Stimuli files were not updated"<<endl;

	}
	timeEnd = time(0);
	display.print(VIEW_COMMENT) <<"Time for creating stimuli "<< difftime( timeEnd, timeInit)  << " seconds"<<endl;

	// ************************************************************
	// Load HW model and generate testbench

	ModelHW hwm= ModelHW();
	string HWpath;
	string HWsources;



	HWpath = configuration->getParStr(string("HWpath"));  // HWpath="./HDL/";
	HWsources = HWpath+"src.txt";
	hwm.setSources(HWpath, HWsources);
	if(hwm.captureInfo() == 0){
		display.print(VIEW_ERROR) << "Error reading HW files"<<endl;
		return -1;
	}

	string pathTB = configuration->getParStr(string("pathTB"));  //  "./TBENCH/";
	string fileTB = configuration->getParStr(string("fileTB"));  // "addMod_tb.vhd";
	hwm.getPreDefinedTB(pathTB,fileTB);



	// ************************************************************
	// Generate simulation script

	//Simulator *sim= new SimMGM();
	SimMGM *sim= new SimMGM();
	string scriptRTL="compileRTL.csh";

	// Generate RTL script and simulate
	timeInit = time(0);
	sim->setModelHWRef(&hwm);
	sim->generateRTLScript(scriptRTL);
	if(configuration->isPhaseExec(2) == 0){
		sim->runSimulationRTL();
		configuration->setPhase(2);
		display.print(VIEW_COMMENT)<<"RTL simulation finished"<<endl;
	}else{
		display.print(VIEW_WARNING) << "RTL simulation was not updated"<<endl;
	}
	timeEnd = time(0);
	display.print(VIEW_COMMENT) <<"Time for testing functionality "<< difftime( timeEnd, timeInit)  << " seconds"<<endl;


	// ************************************************************
	// Synthesize for gating gate design
	Synthesizer *syn=new SynSNPS();
	//SynSNPS *syn=new SynSNPS();
	float unit; // ns
	float timeSyn; // Place desired minimum gate
	float slack;
	float precIncr;
	float Nand;
	string scriptSYN;
	string entityName;

	entityName = hwm.getEntityName();
	unit = configuration->getParFloat(string("unit"));
	timeSyn = configuration->getParFloat(string("timeSyn"));
	slack = configuration->getParFloat(string("slack"));
	precIncr = configuration->getParFloat(string("precIncr"));
	Nand = configuration->getParFloat(string("Nand"));
	scriptSYN = configuration->getParStr(string("scriptSYN"));


	syn->setAreaMinimumGate(Nand);
	syn->setTime(timeSyn*unit); // Set simulation time
	timeInit = time(0);
	if(configuration->isPhaseExec(3) == 0){
		slack = -1; // Force at least one synthesis when configuration file as for updating this file
		while(slack < 0){
			syn->setTime(timeSyn*unit); // Set simulation time
			syn->generateScript(scriptSYN,hwm);
			syn->cleanFiles();
			syn->createGateLevelModule();
			slack=syn->checkErrorsTime(entityName);
			if(slack<0){
				syn->cleanFiles();
				timeSyn += ( ceil(-slack/precIncr) )*precIncr;

			}else{
				if( syn->checkErrors() == 1){
					display.print(VIEW_ERROR)<<"Error: Errors in synthesis"<<endl;
				}else{
					break;
				}
			}
		}
		display.print(VIEW_COMMENT)<<"Synthesis successful with time = "<<timeSyn*unit<<" s"<<endl;
		configuration->setParFloat(string("timeSyn"), timeSyn);
		configuration->setParFloat(string("slack"), slack);
		configuration->setPhase(3);
		display.print(VIEW_COMMENT)<<"Synthesis finished"<<endl;
	}else{
		syn->setDesignName(scriptSYN,hwm);
		display.print(VIEW_WARNING) << "Synthesis results were not updated"<<endl;

	}
	timeEnd = time(0);
	display.print(VIEW_COMMENT) <<"Time for synthesizing "<< difftime( timeEnd, timeInit)  << " seconds"<<endl;

	syn->obtainedInfo(entityName);
	syn->printInfo();



	// *********************************************************
	// Create scripts for P&R

	ParCDS *par=new ParCDS();
	string scriptPAR;

	scriptPAR = configuration->getParStr(string("scriptPAR")); //"SoC_Encounter.tcl";

	par->setModelRef(&hwm);
	par->setTimeSyn(timeSyn);
	timeInit = time(0);
	if(configuration->isPhaseExec(4) == 0){
		par->cleanFiles();
		if( par->generateScript(scriptPAR) != 0){
			display.print(VIEW_ERROR)<<"Error generating PAR script"<<endl;
		}
		if( par->placeRouteModule() != 0){
			display.print(VIEW_ERROR)<<"Error running PAR script"<<endl;
		}
		par->cleanTmpFile();

		if( par->checkErrors() != 0){
			display.print(VIEW_ERROR)<<"Error in place and route process"<<endl;
			return 1;
		}

		configuration->setPhase(4);
		display.print(VIEW_COMMENT)<<"Place and route finished"<<endl;
	}else{
		par->setDesignName(scriptPAR);
		display.print(VIEW_WARNING) << "Place and route results were not updated"<<endl;
	}
	timeEnd = time(0);
	display.print(VIEW_COMMENT) <<"Time for place and route "<< difftime( timeEnd, timeInit)  << " seconds"<<endl;



	// *********************************************************
	// Measure sensitivity for synthesis or Par
	string fileGateTB, compiledLibfolder, scriptGATE,scriptGATEPR, designName;
	int phaseFSA; // 0 = make FSA analysis after
	int numProc; // Number of processes
	int chunkSize;

	float minTimeGate, stepTimeGate;

	numProc =  configuration->getParInt(string("numProc"));
	chunkSize = configuration->getParInt(string("senSimChunk"));
	phaseFSA = configuration->getParInt(string("phaseFSA"));
	fileGateTB = configuration->getParStr(string("fileGateTB")); // "addModGATE_tb.vhd";
	compiledLibfolder = configuration->getParStr(string("compiledLibfolder")); // "FreePDK45";
	scriptGATE = configuration->getParStr(string("scriptGATE")); // "compileGATE.csh";
	scriptGATEPR = configuration->getParStr(string("scriptGATEPR")); // "compileGATE_PR.csh";
	minTimeGate = configuration->getParFloat(string("minTimeGate"));

	stepTimeGate = configuration->getParFloat(string("stepTimeGate"));


	designName = syn->getDesignName();
	hwm.setGateTB(pathTB,fileGateTB);

	/*
	//timeInit = time(0);
	if(configuration->isPhaseExec(5) == 0){
		sim->copyCompiledLib(compiledLibfolder); // Should be executed before generateSynScript
		if (phaseFSA == 0){
			// Sensitivity simulation using timing after synthesis

			sim->generateSYNScript(scriptGATE, designName);
			sim->setTimeGate(timeSyn, minTimeGate, stepTimeGate); // Note: SteptimeGate smaller than 0.001 does not converge
			sim->setSenSimChunk(chunkSize);
			sim->setModelSWRef(swm);
			timeInit = time(0);
			sim->runSimulationSYN(numProc);
			timeEnd = time(0);
			display.print(VIEW_COMMENT) <<"Time for measuring sensitivity FSA Stimuli "<< difftime( timeEnd, timeInit)  << " seconds"<<endl;
			timeInit = time(0);
			sim->runSimulationSYN(numProc,1);
			timeEnd = time(0);
			display.print(VIEW_COMMENT) <<"Time for measuring sensitivity Template Stimuli "<< difftime( timeEnd, timeInit)  << " seconds"<<endl;
		}else{
			cout<< scriptGATEPR <<endl;
			sim->generatePARScript(scriptGATEPR, designName);
			sim->setTimeGate(timeSyn, minTimeGate, stepTimeGate); // Note: SteptimeGate smaller than 0.001 does not converge
			sim->setSenSimChunk(chunkSize);
			sim->setModelSWRef(swm);
			timeInit = time(0);
			sim->runSimulationPAR2(numProc); //(numProc);
			timeEnd = time(0);
			display.print(VIEW_COMMENT) <<"Time for measuring sensitivity FSA Stimuli "<< difftime( timeEnd, timeInit)  << " seconds"<<endl;
			//return 1;
			timeInit = time(0);
			sim->runSimulationPAR2(numProc,1);
			display.print(VIEW_COMMENT) <<"Time for measuring sensitivity Template Stimuli "<< difftime( timeEnd, timeInit)  << " seconds"<<endl;
		}
		//configuration->setPhase(5);
		display.print(VIEW_COMMENT)<<"Sensitivity vector are generated"<<endl;
	}else{
		sim->setSYNscript(scriptGATE);
		display.print(VIEW_COMMENT) <<"Time for measuring sensitivity FSA Stimuli 0 seconds"<< endl;
		display.print(VIEW_COMMENT) <<"Time for measuring sensitivity Template Stimuli 0 seconds"<<endl;
		display.print(VIEW_WARNING) << "Sensitivity information was not updated"<<endl;


	}
	*/

	scriptGATEPR = configuration->getParStr(string("scriptGATEPR_multi"));

	//timeInit = time(0);
	if(configuration->isPhaseExec(5) == 0){
		sim->copyCompiledLib(compiledLibfolder); // Should be executed before generateSynScript
		if (phaseFSA == 0){

		}else{
			cout<< scriptGATEPR <<endl;
			sim->setTimeGate(timeSyn, minTimeGate, stepTimeGate); // Note: SteptimeGate smaller than 0.001 does not converge
			sim->generatePARScript3(scriptGATEPR, designName);
			sim->setSenSimChunk(chunkSize);
			sim->setModelSWRef(swm);
			timeInit = time(0);
			//sim->runSimulationPAR3(numProc); //(numProc);
			//return 1;
			timeEnd = time(0);
			display.print(VIEW_COMMENT) <<"Time for measuring sensitivity FSA Stimuli "<< difftime( timeEnd, timeInit)  << " seconds"<<endl;
			timeInit = time(0);
			sim->runSimulationPAR3(numProc,1);
			timeEnd = time(0);
			display.print(VIEW_COMMENT) <<"Time for measuring sensitivity Template Stimuli "<< difftime( timeEnd, timeInit)  << " seconds"<<endl;
		}
		//configuration->setPhase(5);
		display.print(VIEW_COMMENT)<<"Sensitivity vector are generated"<<endl;
	}else{
		sim->setSYNscript(scriptGATE);
		display.print(VIEW_COMMENT) <<"Time for measuring sensitivity FSA Stimuli 0 seconds"<< endl;
		display.print(VIEW_COMMENT) <<"Time for measuring sensitivity Template Stimuli 0 seconds"<<endl;
		display.print(VIEW_WARNING) << "Sensitivity information was not updated"<<endl;


	}


	// *********************************************************
	// Perform FSA analysis
	string modelsPath;
	int pointEvoCorrelation; ///< Points in the correlation evolution graph


	FSAanalyzer *FSA = new FSAanalyzer();



	pointEvoCorrelation = configuration->getParInt(string("pointEvoCorrelation"));
	modelsPath = configuration->getParStr(string("modelsPath")); // "./res/SenModels/";


	cout<< "Update models "<<configuration->isPhaseExec(6)<<endl;

	if(configuration->isPhaseExec(6) == 0){ // Update all models
		FSA->updateAllModels();
		//configuration->setPhase(6); // Not update all models next time
	}



	timeInit = time(0);
	FSA->setModelsPath(modelsPath);
	FSA->loadModels();
	FSA->setSWModel(swm);
	FSA->setAnalysis(phaseFSA);



	FSA->genTemplateModels();
	FSA->genIntermediates();
	FSA->estimatedSenForAllModels();
	FSA->getCorrelationForAllModels(pointEvoCorrelation);

	timeEnd = time(0);
	display.print(VIEW_COMMENT) <<"Time for performing FSA "<< difftime( timeEnd, timeInit)  << " seconds"<<endl;



	// *********************************************************
	// Get images results
	//Plotter plot = Plotter();
	MTLBplot *plot;

	plot = new MTLBplot();




	string scriptPlot = "";
	string scriptPlotT = "";
	string folderImages = "";
	string variablePath = "";
	int corrKey;



	corrKey = swm->getFixedkey();

	scriptPlot = configuration->getParStr(string("scriptPlot"));
	scriptPlotT = configuration->getParStr(string("scriptPlotT"));




	timeInit = time(0);
	if (phaseFSA == 0){
		folderImages = "./FIG/SYN/";
		variablePath = "./VAR/SYN/";
	}else{
		folderImages = "./FIG/PAR/";
		variablePath = "./VAR/PAR/";
	}

	plot->generateScript(scriptPlot, scriptPlotT);
	plot->plotFSAresults(folderImages,variablePath,corrKey);
	plot->plotTemplate(folderImages, variablePath);



	timeEnd = time(0);
	display.print(VIEW_COMMENT) <<"Time for plotting results "<< difftime( timeEnd, timeInit)  << " seconds"<<endl;


	// *********************************************************
	// Free memory space

	delete plot;
	delete FSA;
	delete par;
	delete syn;
	delete sim;
	delete configuration;



	cout<<"End!!!"<<endl;
	return EXIT_SUCCESS;
}


// ToDo: Simulation needs flag 'novopt' for multitherdiagn. Avoid this crating a different library and wlf file every process
// ToDo: Allow to reestart sensitivity simulation
// ToDo: In sensitivity simulation avoid restarting process
// ToDo: Class for reading and writing information of the complete simulation
// ToDo: Include simulation for multi-input, multicycle, multioutput
// ToDo: Enforce codign style from http://www.yolinux.com/TUTORIALS/LinuxTutorialC++CodingStyle.htmlo
// ToDo: Unify tag translation in modelHW
// ToDo: Create class FSAparameter and put this class in modelSW
// ToDo: Unify error codes



// https://blog.noctua-software.com/object-factory-c++.html
// https://www.linuxjournal.com/article/3687


// Number of iterations for running from obtaining sensitivity form 1000 ps with a precision of 2ps is:
// ceil(log2(1000/2)) + 1= 11
