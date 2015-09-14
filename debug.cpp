/*
 * debug.cpp
 * You may play with this source however you like, as long as you don't remove
 * this comment, claim undue credit, or make a profit on it.  Enjoy.
 * CHANGE HISTORY
 * 2011-11-23 (0.00)
 *  Created file.
 */

#ifndef _DUNGEONEER_H_
#include "dungeoneer.h"
#endif

#include <fstream>
#include <iostream>
#include <sstream>
#include "debug.h"
using namespace std ;

/* *** EXTERNAL REFERENCES ************************************** (none) *** */

/* *** LOCAL DATA ********************************************************** */

Logger *log ; // Used globally; should be added as extern in other .cpp files.

/* *** LOCAL PROCEDURE PROTOTYPES ******************************* (none) *** */

/* *** PROCEDURE DEFINITIONS ************************************ (none) *** */

/* *** CLASS DEFINITION (Logger) ******************************************* */

// /// STATIC FUNCTIONS (classname) ///////////////////////////// (none) /// //

// /// CONSTRUCTORS (Logger) /////////////////////////////////////////////// //

/*
 * Default constructor.
 */
Logger::Logger(void)
{
	logindex = 0 ;
	lfout.open( LOGFILE, fstream::out|fstream::app ) ;
	maxlevel = Logger::MAXLEVEL ;
	silenced = false ;
}

/*
 * Constructor which sets log level.
 */
Logger::Logger( unsigned short l )
{
	logindex = 0 ;
	lfout.open( LOGFILE, fstream::out|fstream::app ) ;

	if( l > Logger::MAXLEVEL ) maxlevel = Logger::MAXLEVEL ;
	else maxlevel = l ;

	silenced = false ;
}

// /// ACCESSORS (Logger) /////////////////////////////////////// (none) /// //

// /// MUTATORS (Logger) /////////////////////////////////////////////////// //

/*
 * Clears the current contents of the entry buffer.
 */
void Logger::clearBuffer(void)
{
	lbuf.str("") ;
	return ;
}

/*
 * Close the log file output stream.
 */
bool Logger::close(void)
{
	lfout.close() ;
	return true ;
}

/*
 * Temporarily silences logging output.
 */
void Logger::silence(void) { this->silenced = true ; return ; }

/*
 * Wakens the logger after having been silenced.
 */
void Logger::wake(void) { this->silenced = false ; return ; }

// /// OTHER PUBLIC FUNCTIONS (Logger) ///////////////////////////////////// //

/*
 * Writes a log entry.
 */
void Logger::write( unsigned short level, string file, unsigned int line,
                    string func, string message )
{
	if( level > maxlevel ) return ;
	if( silenced ) return ;

	stringstream entry ( stringstream::out ) ;

	entry << logindex++
	      << "|"
	      << getLevelLabel(level)
	      << "|"
	      << file
	      << "|"
	      << line
	      << "|"
	      << func
	      << "|"
	      << message
		  << "|\n"
	      ;

	lfout << entry.str() ;
	lfout.flush() ;
}

/*
 * Writes and flushes the contents of the logger object's buffer.
 */
void Logger::writeBuffer( unsigned short level, string file,
                          unsigned int line, string func )
{
	write( level, file, line, func, lbuf.str() ) ;
	lbuf.str("") ;
}

// /// PRIVATE FUNCTIONS (Logger) ////////////////////////////////////////// //

/*
 * Returns a string representation of a logging level constant.
 */
string Logger::getLevelLabel( unsigned short level )
{
	switch(level)
	{
		case (Logger::ERROR):   return "Error" ;
		case (Logger::WARNING): return "Warn " ;
		case (Logger::INFO):    return "Info " ;
		case (Logger::TRACE):   return "Trace" ;
		default:                return "     " ;
	}
}

/*************************************************************************** */
