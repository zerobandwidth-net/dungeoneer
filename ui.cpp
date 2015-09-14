/*
 * ui.cpp
 * You may play with this source however you like, as long as you don't remove
 * this comment, claim undue credit, or make a profit on it.  Enjoy.
 * CHANGE HISTORY
 * 2011-11-18
 *  Created file.
 */

#include <cstring>
#include <ncurses.h>
#include <sstream>
#include <string>
#include "debug.h"
#include "ui.h"
#include "window.h"
using namespace std ;

/* *** EXTERNAL REFERENCES ************************************************* */

extern Logger *log ;

/* *** LOCAL DATA ********************************************************** */

string statusEffectNames[STATUS_EFFECTS] =
    {
        "hastened",          // HASTED
        "slowed",            // SLOWED
        "asleep",            // ASLEEP
        "confused",          // CONFUSED
        "poisoned",          // POISONED
        "blinded",           // BLINDED
        "hallucinating",     // HALLUCINATING
        "paralyzed",         // PARALYZED
        "petrified",         // STONED
        "terrified",         // SCARED
        "floating",          // FLOATING
        "seeing creatures",  // SEES_CREATURES
        "seeing treasure",   // SEES_STUFF
    };

/* *** LOCAL PROCEDURE PROTOTYPES ******************************* (none) *** */

/* *** PROCEDURE DEFINITIONS *********************************************** */

/*
 * Returns alphabetic position of letter.  The input is case-insensitive; the
 * letters 'a' and 'A' would both return 1.  If the character is not a letter,
 * the procedure returns 0.
 */
unsigned short ltoi( char c )
{
	if( c >= 'a' && c <= 'z' )
		return ( c - 'a' + 1 ) ;
	else if( c >= 'A' && c <= 'Z' )
		return ( c - 'A' + 1 ) ;
	else
		return 0 ;
}

/*
 * (English-only) Determines the appropriate article "a" or "an" for a supplied
 * noun string.
 */
string article( string noun, bool capitalize )
{
	char c = noun.at(0) ;

	if( cins( c, "AEIOUaeiou" ) )
	{
		if( capitalize ) return "An" ;
		else return "an" ;
	}
	else
	{
		if( capitalize ) return "A" ;
		else return "a" ;
	}
}

/*
 * If the initial character is a lower-case letter, this function replaces it
 * with an uppercase letter.
 */
string capitalizeString( string s )
{
	char c = s[0] ;

	if( ( c >= 'a' ) && ( c <= 'z' ) )
		return s.replace(0,1,1,(c-32)) ;
	else
		return s ;
}

/*
 * Returns true if char c is found anywhere in string s.
 */
bool cins( char c, char *s )
{
	int l = strlen(s) ;

	for( int i = 0 ; i < l ; i++ )
		if( s[i] == c ) return true ;

	return false ;
}

/*
 * Initializes the curses screen.  The colorize argument specifies whether the
 * procedure should attempt to set colorized mode.
 */
void initializeCurses( bool colorize )
{
	log->write( Logger::INFO, __FILE__, __LINE__, "initializeCurses(bool)",
	            "Initializing curses mode..." ) ;
	initscr() ;					// Initializes curses mode.
	raw() ;						// Sets input mode to raw.
	keypad( stdscr, FALSE ) ;	// Sets keypad parsing.
	noecho() ;					// Disables echo of inputs.
	curs_set(0) ;				// Makes the cursor invisible.

	if( colorize && has_colors() )
	{
		log->write( Logger::INFO, __FILE__, __LINE__, "initializeCurses(bool)",
		            "Initializing colorized mode..." ) ;
		start_color() ;			// Starts colorized mode.
		for( int i = 0 ; i < 8 ; i++ )
			init_pair( i, i, 0 ) ;
	}

	refresh() ;
}

/*
 * Returns true if the input character is printable.  Uses a very simplistic
 * standard for this test.
 */
bool isPrintable( char c ) { return ( c >= 32 && c <= 126 ) ; }

/*
 * Converts number to string.  Probably the worst way to do this, but I don't
 * need any more time sinks right now.  This should be optimized later.
 */
string numToString( long num )
{
	stringstream ss (stringstream::out) ;
	ss << num ;
	return ss.str() ;
}

/*
 * Padded display of values.
 */
string pad( short x, unsigned short width )
{
	stringstream ss (stringstream::out) ;
	ss << x ;
	return pad( ss.str(), width ) ;
}

string pad( unsigned short x, unsigned short width )
{
	stringstream ss (stringstream::out) ;
	ss << x ;
	return pad( ss.str(), width ) ;
}

string pad( int x, unsigned short width )
{
	stringstream ss (stringstream::out) ;
	ss << x ;
	return pad( ss.str(), width ) ;
}

string pad( unsigned int x, unsigned short width )
{
	stringstream ss (stringstream::out) ;
	ss << x ;
	return pad( ss.str(), width ) ;
}

string pad( string s, unsigned short width )
{
	if( s.length() > width )
		return s.substr( 0, width ) ;

	string ps = s ;

	for( unsigned short i = 0 ; i < (width-s.length()) ; i++ )
		ps.insert( 0, 1, ' ' ) ;

	return ps ;
}

/*
 * Waits for any user input.
 */
char waitForInput(void)
{
	return getch() ;
}

/*
 * Waits for a specific user input.
 */
char waitForInput( char *s )
{
	char c ;
	do { c = getch() ; } while( !(cins(c,s)) ) ;
	return c ;
}

/* ************************************************************************* */
