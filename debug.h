/*
 * debug.h
 * You may play with this source however you like, as long as you don't remove
 * this comment, claim undue credit, or make a profit on it.  Enjoy.
 * CHANGE HISTORY
 * 2011-11-23 (0.00)
 *  Created file.
 */

#ifndef _DUNGEONEER_H_
#include "dungeoneer.h"
#endif

#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <iostream>
#include <fstream>
#include <sstream>
using namespace std ;

/* *** TWEAKS ************************************************************** */

#define LOGFILE "log"

/* *** COMPLEX TYPE DEFINITIONS ********************************* (none) *** */

/* *** GLOBAL PROCEDURE PROTOTYPES ****************************** (none) *** */

/* *** CLASS PROTOTYPES (needed for friendship) ***************** (none) *** */

/* *** CLASS DEFINITIONS *************************************************** */

/*
 * The Logger class provides standardized logging.
 */
class Logger
{
	public:

		// Static constants...

		static const unsigned short ERROR = 0 ;
		static const unsigned short WARNING = 1 ;
		static const unsigned short INFO = 2 ;
		static const unsigned short TRACE = 3 ;
		static const unsigned short MAXLEVEL = 3 ;

		// Static functions...

		// Public fields...
		stringstream lbuf ; // Offered as buffer for numeric data.

		// Constructors...

		Logger() ; // Default constructor.
		Logger( unsigned short l ) ; // Sets maximum level.

		// Accessors...

		// Mutators...

		void clearBuffer() ;
		bool close() ; // Close the log file output stream.
		void silence() ;
		void wake() ;

		// Other functions...

		void write( unsigned short level, string file, unsigned int line,
		            string func, string message ) ; // Writes a log entry.

		void writeBuffer( unsigned short level, string file, unsigned int line,
		                  string func ) ;

	private:

		// Fields...
		unsigned int logindex ; // Line number in log file.
		fstream lfout ; // Output stream for log file.
		unsigned short maxlevel ; // Set maximum logging level as field.
		bool silenced ; // Allows for a logger to be silenced temporarily.

		// Functions...
		string getLevelLabel( unsigned short level ) ;
};

/* ************************************************************************* */ 
#endif
