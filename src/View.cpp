/*
 * View.cpp
 *
 *  Created on: Sep 6, 2019
 *      Author: valencif
 */

#include "View.h"

View::View(){

	debug = 0;
	format = 0;
	output = 0;

	logfile.open("Log.txt", ios::out );
}

View::~View() {
	if(logfile.is_open()){
		logfile.close();
	}
}


const View& View::operator<<( std::ostream& (* msg)(std::ostream&) ) const {
	if(debug == 0 && format==VIEW_DEBUG){ // Avoid printing endl of debug tasks.
		return *this;
	}
	if( output == 1){
		logfile << msg;
	}else{
		cout<< msg;
	}
	return *this;
}
const View& View::operator<<( std::basic_ios<char>& (* msg)(std::basic_ios<char>&) )  const  {
	if( output == 1){
		logfile << msg;
	}else{
		cout<< msg;
	}
	return *this;
}

const View& View::operator<<( std::ios_base& (* msg)(std::ios_base&) )  const  {
	if( output == 1){
		logfile << msg;
	}else{
		cout<< msg;
	}
	return *this;
}

View& View::print(int fmt){
	format= fmt;
	output = 0;
	return instance();
}
View& View::log(int fmt){
	format= fmt;
	output = 1;
	return instance();

}
void View::setDebug(int flag){
	debug=flag;
}




/*

void View::printError(T data, int newline){

}
void View::printComment(T data, int newline){

}
void View::printWarning(T data, int newline){

}
void View::printText(T data, int newline){

}
void View::saveLog(T data, int newline){

}
*/
