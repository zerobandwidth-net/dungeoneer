/*
 * command.cpp
 * You may play with this source however you like, as long as you don't remove
 * this comment, claim undue credit, or make a profit on it.  Enjoy.
 * CHANGE HISTORY
 * 2011-12-22
 *  Split from ui.cpp.
 */

//#include <cstring>
#include <ncurses.h>
#include <string>
#include "command.h"
#include "debug.h"
using namespace std ;

/* *** EXTERNAL REFERENCES ************************************************* */

extern Logger *log ;

/* *** LOCAL DATA *********************************************** (none) *** */

/* *** LOCAL PROCEDURE PROTOTYPES ******************************* (none) *** */

/* *** PROCEDURE DEFINITIONS ************************************ (none) *** */

/* *** CLASS DEFINITION (Command) ****************************************** */

// /// STATIC FUNCTIONS (Command) /////////////////////////////// (none) /// //

// /// CONSTRUCTORS (Command) ////////////////////////////////////////////// //
// All constructors call the private function init(bool).

/*
 * Default constructor.
 */
Command::Command(void) { init(true) ; }

/*
 * The Boolean parameter to this constructor indicates whether the input should
 * be "interpreted" as a gameplay command (true) or simply passed back as-is
 * (false).
 */
Command::Command( bool interpreted ) { init(interpreted) ; }


// /// ACCESSORS (Command) ///////////////////////////////////////////////// //

/*
 * Returns the first entered character, or the null character (0) if the
 * command holds a special code.
 */
char Command::getCharCode(void)
{
	string func = "Command::getCharCode()" ; // debug

	if( code > 255 )
	{
		log->write( Logger::TRACE, __FILE__, __LINE__, func,
		            "Returning null character." ) ;
		return 0 ;
	}
	else
	{
		log->lbuf << "Returning character: " << received[0] ;
		log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
		return received[0] ;
	}
}

unsigned int Command::getCode(void) { return code ; }

unsigned short Command::getCount(void) { return count ; }

string Command::str(void) { return received ; }

// /// MUTATORS (Command) ////////////////////////////////////////////////// //

// /// OTHER PUBLIC FUNCTIONS (Command) ///////////////////////// (none) /// //

// /// PRIVATE FUNCTIONS (Command) ///////////////////////////////////////// //

/*
 * Initializes the Command object.  The Boolean parameter indicates whether the
 * input should be "interpreted" as a gameplay command (true) or simply passed
 * back as-is (false).
 */
void Command::init( bool interpreted )
{
	string func = "Command::init(bool)" ;
	log->write( Logger::INFO, __FILE__, __LINE__, func,
	            "Fetching next user command." ) ;

	char c[5] = "\0\0\0\0" ; // Buffer for interpreted characters.
	bool complete = false ; // Signals control loop to stop processing input.
	count = 0 ;

	do
	{
		c[0] = getch() ;
		log->lbuf << "c[0] = " << c[0] ;
		log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;

		switch(c[0])
		{
			case 27: // [Esc]: First character might begin an escape sequence.
				log->write( Logger::TRACE, __FILE__, __LINE__, func,
				            "Evaluating escape sequence." ) ;
				halfdelay(1) ; // If it's just an escape, we'll return K_CANCEL.
				c[1] = getch() ;
				log->lbuf << "c[1] = " << c[1] ;
				log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
				switch(c[1])
				{
					case '[': // Continuing keypad escape sequence.
						log->write( Logger::TRACE, __FILE__, __LINE__, func,
						            "Continuing keypad escape sequence." ) ;
						raw() ;
						c[2] = getch() ;
						log->lbuf << "c[2] = " << c[2] ;
						log->writeBuffer(Logger::TRACE,__FILE__,__LINE__,func) ;
						switch(c[2])
						{
							case '1': // [NP-Home] but not [Home] in Linux
								code = K_NORTHWEST ;
								if( count == 0 ) count = 1 ;
								interruptible = true ;
								c[3] = getch() ; // catch trailing ~
								complete = true ;
								break ;
							case '2': // [Ins] or [NP-Ins]
								code = K_INVENTORY ;
								count = 1 ;
								interruptible = false ;
								c[3] = getch() ; // catch trailing ~
								complete = true ;
								break ;
							case '3': // [Del] or [NP-Del]
								code = K_REST ;
								if( count == 0 ) count = 1 ;
								interruptible = true ;
								c[3] = getch() ; // catch trailing ~
								complete = true ;
								break ;
							case '4': // [End] but not [NP-End] in Linux
								code = K_SOUTHWEST ;
								if( count == 0 ) count = 1 ;
								interruptible = true ;
								c[3] = getch() ; // catch trailing ~
								complete = true ;
								break ;
							case '5': // [PgUp] or [NP-PgUp]
								code = K_NORTHEAST ;
								if( count == 0 ) count = 1 ;
								interruptible = true ;
								c[3] = getch() ; // catch trailing ~
								complete = true ;
								break ;
							case '6': // [PgDn] or [NP-PgDn]
								code = K_SOUTHEAST ;
								if( count == 0 ) count = 1 ;
								interruptible = true ;
								c[3] = getch() ; // catch trailing ~
								complete = true ;
								break ;
							case 'A': // [UpArrow] or [NP-UpArrow]
								code = K_NORTH ;
								if( count == 0 ) count = 1 ;
								interruptible = true ;
								complete = true ;
								break ;
							case 'B': // [DownArrow] or [NP-DownArrow]
								code = K_SOUTH ;
								if( count == 0 ) count = 1 ;
								interruptible = true ;
								complete = true ;
								break ;
							case 'C': // [RightArrow] or [NP-RightArrow]
								code = K_EAST ;
								if( count == 0 ) count = 1 ;
								interruptible = true ;
								complete = true ;
								break ;
							case 'D': // [LeftArrow] or [NP-LeftArrow]
								code = K_WEST ;
								if( count == 0 ) count = 1 ;
								interruptible = true ;
								complete = true ;
								break ;
							case 'G': // [NP-5], Windows/puTTY only
								code = K_REST ;
								if( count == 0 ) count = 1 ;
								interruptible = true ;
								complete = true ;
								break ;
							default:
								code = K_UNKNOWN ;
								count = 0 ;
								interruptible = false ;
								complete = true ;
						}
						break ;
					case 'O': // Continuing navigation keypad escape sequence.
						log->write( Logger::TRACE, __FILE__, __LINE__, func,
						            "Continuing keypad escape sequence." ) ;
						raw() ;
						c[2] = getch() ;
						log->lbuf << "c[2] = " << c[2] ;
						log->writeBuffer(Logger::TRACE,__FILE__,__LINE__,func) ;
						switch(c[2])
						{
							case 'F': // [End] on Linux terms only.
								code = K_SOUTHWEST ;
								if( count == 0 ) count = 1 ;
								interruptible = true ;
								complete = true ;
								break ;
							case 'H': // [Home] on Linux terms only.
								code = K_NORTHWEST ;
								if( count == 0 ) count = 1 ;
								interruptible = true ;
								complete = true ;
								break ;
							default:
								code = K_UNKNOWN ;
								count = 0 ;
								interruptible = false ;
								complete = true ;
						}
						break ;
					case ERR: // halfdelay input was not received.
						c[1] = '\0' ;
						// Don't break; leak into default case.
					default: // Throw away the extra character and render cancel.
						log->write( Logger::TRACE, __FILE__, __LINE__, func,
						            "Discarding escape sequence; reporting K_CANCEL." ) ;
						raw() ;
						code = K_CANCEL ;
						count = 1 ;
						interruptible = false ;
						complete = true ;
				}
				break ;
			case 48: // '0'
			case 49: // '1'
			case 50: // '2'
			case 51: // '3'
			case 52: // '4'
			case 53: // '5'
			case 54: // '6'
			case 55: // '7'
			case 56: // '8'
			case 57: // '9'
				if( interpreted )
				{
					count = ( count * 10 ) + ( c[0] - 48 ) ; // Add digit to count.
					log->lbuf << "Increased action count: " << count ;
					log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
					complete = false ;
				}
				else
				{
					log->lbuf << "Reporting input: " << c[0] ;
					log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
					interruptible = false ;
					complete = true ;
				}
				break ;
			default:
				log->lbuf << "Reporting input " << c[0] ;
				log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
				code = c[0] ;
				if( count == 0 ) count = 1 ;
				interruptible = false ;
				complete = true ;
		}
	} while( !complete ) ;

	received = string(c) ;
	log->lbuf << "Received input: " << received ;
	log->writeBuffer( Logger::INFO, __FILE__, __LINE__, func ) ;
	log->lbuf << "Command code: " << code ;
	log->writeBuffer( Logger::INFO, __FILE__, __LINE__, func ) ;

	return ;
}

/* ************************************************************************* */
