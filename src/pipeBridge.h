/*
 * pipeBridge.h
 *
 *  Created on: Oct 4, 2019
 *      Author: valencif
 */


/**
 * \class pipeBridge
 *
 *
 * \brief class to facilitate the communication with other program through pipes
 *
 * This class saves output from controlled process and delivers it line by line in order
 * to have better control. It can wait until a string appears. It also sends commands to
 * the controlled process
 *
 * \warning It works with non-blocking pipes
 *
 * \author Andrés Felipe Valencia
 * \version 0.1
 * \copyright GNU Public License.
 * \date : Sep 6, 2019
 * Contact: valena@usi.ch
 * Created on: Sep 6, 2019
 *
 */

#ifndef PIPEBRIDGE_H_
#define PIPEBRIDGE_H_

#include <iostream>
#include <string>
#include <sstream>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

class pipeBridge {
private:
	int pipeIn; ///< Pipe that receives data from controlled process
	int pipeOut; ///< Pipe to send information to controlled process

	char *bufferIn;  ///< Buffer used for reading info from  controlled process
	int bufferInSize; ///< Size of bufferIn
	string  input; ////< Concatenation of all read information in the buffer

	int display; ///< Controls if this class prints information

	void checkNewInput();


public:
	pipeBridge();
	virtual ~pipeBridge();

	void initialize(int bufferInSize, int	pipeIn, int pipeOut, int display = 0 );
	void setDisplay(int display);
	int getLine(string &line); ///< returns a line and delete the information from input. Always check if there is more info in the controlled process
	int  getLineFind(string &line,string text); ///< Get line and find text
	void waitFindInLine(string text, int showProgres=0); ///< Wait until a line with text appears. Lines in the middle will be discarded
	void waitFindInInput(string text); ///< Wait until text appears in the last part of input even but it does not contain '\n'
	int findInInput(string text); ///< Check input contains text but it does not contain '\n'
	//int findInLineInput(string text); ///< Check if text appear in the first line or in the input in case input does not '\n'
	void sendData(string data); ///< Send information to the controlled process
	void sendData(stringstream &cmd);
	void dump();
	void dump(int cnt);
};

#endif /* PIPEBRIDGE_H_ */
