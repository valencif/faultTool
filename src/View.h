
/**
 * \class View
 *
 *
 * \brief Class to display information in the console and to log
 *
 * The class print messages in the console with different colors and also send information
 * to a log file. It is implemented as a singleton to guarantee log file consistency.
 *
 * \warning Class does not accept endl, hex. To finish a line place \n
 *
 * \author Andrés Felipe Valencia
 * \version 0.1
 * \copyright GNU Public License.
 * \date : Sep 6, 2019
 * Contact: valena@usi.ch
 * Created on: Sep 6, 2019
 *
 */

#ifndef VIEW_H_
#define VIEW_H_


#define VIEW_NORMAL 0
#define VIEW_ERROR 1
#define VIEW_WARNING 2
#define VIEW_COMMENT 3
#define VIEW_DEBUG 4


#include <iostream>
#include <string>
#include <fstream>

using namespace std;

class View{
private:
	int debug; ///< Decides if debug debug information is display or not
	ofstream logfile;
	int format; ///< Decides the type of message to be transmitted
	int output; ///< 0 = cout, 1 = log file
	View();

public:

	virtual ~View();

	// Implementing singleton pattern
	static View &instance()
	{
		static View obj;
		return obj;
	}
	View (View &);                 // Don't Implement
	void operator=(View const&);   // Don't implement

	template<typename T>
	const View& operator<<(const T& msg)  ;

	const View& operator<<( std::ostream& (* msg)(std::ostream&) ) const ;
	const View& operator<<( std::basic_ios<char>& (* msg)(std::basic_ios<char>&) )  const ;
	const View& operator<<( std::ios_base& (* msg)(std::ios_base&) )  const ;

	View &print(int fmt);
	View &log(int fmt);
	void setDebug(int flag);

};

// Not possible to place in the cpp
template<typename T>
const View& View::operator<<(const T& msg)  {

	if(output == 1){
		switch(format){
			case VIEW_NORMAL:
				logfile<< msg ;
				break;
			case VIEW_ERROR:
				logfile << "*ERROR: ";//<<msg;
				break;
			case VIEW_WARNING:
				logfile << "*WARNING: "<<msg;
				break;
			case VIEW_COMMENT:
				logfile << "INFO: "<<msg;
				break;
			case VIEW_DEBUG:
				if(debug==1){
					logfile<< "DEBUG: "<< msg ;
				}
		}
	}else{
		switch(format){
		case VIEW_NORMAL:
			cout<< msg ;
			break;
		case VIEW_ERROR:
			cout << "\033[1;31m"<<msg<<"\033[0m";
			break;
		case VIEW_WARNING:
			cout << "\033[1;33m"<<msg<<"\033[0m";
			break;
		case VIEW_COMMENT:
			cout << "\033[0;32m"<<msg<<"\033[0m";
			break;
		case VIEW_DEBUG:
			if(debug==1){
				cout<< "DEBUG: "<< msg ;
			}
		}
	}

	return *this;
}



#endif /* VIEW_H_ */
