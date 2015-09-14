/*
 * ui.h
 * You may play with this source however you like, as long as you don't remove
 * this comment, claim undue credit, or make a profit on it.  Enjoy.
 * CHANGE HISTORY
 * 2011-11-18
 *  Created file.
 */

#ifndef _DUNGEONEER_H_
#include "dungeoneer.h"
#endif

#ifndef _UI_H_
#define _UI_H_

#include <string>
#include <ncurses.h>
#include "command.h"
#include "window.h"
using namespace std ;

/* *** TWEAKS *************************************************** (none) *** */

/* *** COMPLEX TYPE DEFINITIONS ********************************* (none) *** */

/* *** GLOBAL PROCEDURE PROTOTYPES ***************************************** */

unsigned short ltoi( char c ) ; // Returns alphabetic position of letter.

string article( string noun, bool capitalize ) ; // Returns "a" or "an"

string capitalizeString( string s ) ; // Capitalizes first letter if necessary.

bool cins( char c, char *s ) ; // True if c is in s.

void initializeCurses( bool colorize ) ; // Initializes curses mode.

bool isPrintable( char c ) ; // True if character is printable onscreen.

string numToString( long num ) ; // Converts number to string.

string pad( short x, unsigned short width ) ; // Padded display of short.
string pad( unsigned short x, unsigned short width ) ;
string pad( int x, unsigned short width ) ;
string pad( unsigned int x, unsigned short width ) ;
string pad( string s, unsigned short width ) ;

char waitForInput() ; // Waits for user input.

char waitForInput( char *s ) ; // Waits for specific input.

/* *** CLASS PROTOTYPES (needed for friendship) ***************** (none) *** */

/* *** CLASS DEFINITIONS **************************************** (none) *** */

/* ************************************************************************* */
#endif
