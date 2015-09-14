/*
 * window.cpp
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
#include "ui.h"
#include "window.h"
using namespace std ;

/* *** EXTERNAL REFERENCES ************************************************* */

extern Logger *log ;

/* *** LOCAL DATA *********************************************** (none) *** */

/* *** LOCAL PROCEDURE PROTOTYPES ******************************* (none) *** */

/* *** PROCEDURE DEFINITIONS *********************************************** */

/*
 * Displays a one-line click-through dialog box.
 */
char displayDialog( string message )
{
	char c ;
	Window *dialog = new Window( 3, (message.length()+2) ) ;
	dialog->mvwstr( 1, 1, message ) ;
	dialog->refresh() ;
	c = getch() ;
	delete dialog ;
	return c ;
}

/* *** CLASS DEFINITION (Window) ****************************************** */

// /// STATIC FUNCTIONS (Window) /////////////////////////////// (none) /// //

// /// CONSTRUCTORS (Window) ////////////////////////////////////////////// //

/* Unlabelled window constructor. */
Window::Window( unsigned short y, unsigned short x,
                unsigned short h, unsigned short w )
{
	this->y = y ; this->x = x ; this->w = w ; this->h = h ;
	this->win = newwin( h, w, y, x ) ;
	wborder( this->win, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE,
             ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER ) ;
	wrefresh( this->win ) ;
}

/* Unlabelled dialog constructor. */
Window::Window( unsigned short h, unsigned short w )
{
	this->y = (unsigned short)( ( DROWS / 2 ) - (unsigned short)( h / 2 ) ) ;
	this->x = (unsigned short)( ( DCOLS / 2 ) - (unsigned short)( w / 2 ) ) ;
	this->w = w ;
	this->h = h ;
	this->win = newwin( this->h, this->w, this->y, this->x ) ;
	wborder( this->win, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE,
             ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER ) ;
	wrefresh( this->win ) ;
}

// /// DESTRUCTOR (Window) //////////////////////////////////////////////// //

Window::~Window(void)
{
	if( win != NULL )
	{
		wborder( win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' ) ; // Erases border.
		werase( win ) ;
		wrefresh( win ) ; // Removes window from view.
		delwin( win ) ; // Frees window from memory.
		touchline( stdscr, y, h ) ;
	}
}

// /// ACCESSORS (Window) ///////////////////////////////////////////////// //

/* Returns the window's absolute height. */
unsigned short Window::getHeight(void) { return this->h ; }

/* Returns the window's interior height. */
unsigned short Window::getIHeight(void) { return ( this->h - 2 ) ; }

/* Returns the window's interior width. */
unsigned short Window::getIWidth(void) { return ( this->h - 2 ) ; }

/* Returns the window's absolute width. */
unsigned short Window::getWidth(void) { return this->w ; }

// /// MUTATORS (Window) ////////////////////////////////////////////////// //

/* Sets an ncurses attribute off. */
void Window::aoff( attr_t a ) { wattroff( this->win, a ) ; return ; }

/* Sets an ncurses attribute on. */
void Window::aon( attr_t a ) { wattron( this->win, a ) ; return ; }

/* Explicitly sets ncurses attributes. */
void Window::aset( attr_t a ) { wattrset( this->win, a ) ; return ; }

/* Changes an ncurses attribute at the current location. */
void Window::chg( short count, attr_t a, short v )
{
	wchgat( this->win, count, a, v, NULL ) ;
	return ;
}

/* Clears the window's contents. */
void Window::clear(void)
{
	log->write( Logger::TRACE, __FILE__, __LINE__, "Window::clear()",
	            "Clearing window.  Redrawing border." ) ;
	werase(this->win) ;
	drawBorder() ;
	drawHeader() ;
	drawFooter() ;
}

/* Clears the window's footer string, and redraws. */
void Window::clearFooter(void)
{
	footer = "" ;
	drawFooter() ;
	return ;
}

/* Changes an ncurses attribute at the given location. */
void Window::mvchg( unsigned short py, unsigned short px, 
                    short count, attr_t a, short v )
{
	mvwchgat( this->win, py, px, count, a, v, NULL ) ;
	cy = py ; cx = px ;
	return ;
}

/* Moves the current relative cursor to (py,px) and prints a character. */
void Window::mvwch( unsigned short py, unsigned short px, char c )
{
	mvwaddch( this->win, py, px, c ) ;
	cy = py ; cx = px ;
	return ;
}

/* Moves the current relative cursor to (py,px) and prints text. */
void Window::mvwstr( unsigned short py, unsigned short px, string text )
{
	string dtext ;

	if( ( text.length() + px ) > this->w ) // Truncate to fit.
		dtext = text.substr( 0, ( this->w - px - 1 ) ) ;
	else dtext = text ;

	mvwaddstr( this->win, py, px, dtext.c_str() ) ;

	cy = py ; cx = px ;
	return ;
}

/* Sets a new footer value and redraws the window. */
void Window::setFooter( string nf )
{
	log->lbuf << "Setting footer: " << nf ;
	log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, "Window::setFooter()" ) ;

	footer = nf ;
	drawFooter() ;
	return ;
}

/* Sets a new header value and redraws the window. */
void Window::setHeader( string nh )
{
	log->lbuf << "Setting header: " << nh ;
	log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, "Window::setHeader()" ) ;

	header = nh ;
	drawHeader() ;
	return ;
}

/* Puts a character at the current relative position. */
void Window::wch( char c )
{
	waddch( this->win, c ) ;
	return ;
}

/* Prints at the current relative position. */
void Window::wstr( string text )
{
	waddstr( win, text.c_str() ) ;
	return ;
}

// /// OTHER PUBLIC FUNCTIONS (Window) //////////////////////////////////// //

/*
 * Collects a textual entry from the user and continually echoes it to the
 * visible window in-place.
 */
string Window::getTextEntry( unsigned short r, unsigned short c,
                             unsigned short length )
{
	string func = "Window::getTextEntry(ushort,ushort,ushort)" ; // debug

	log->lbuf << "Collecting entry of length " << length
	          << " starting at ( " << r << "," << c << ")."
	          ;
	log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;

	string s = "" ;
	string blank = "[]" ;
	for( unsigned short i = 0 ; i < length-2 ; i++ )
		blank.insert( 1, 1, '_' ) ;
	bool done = false ;

	do
	{
		this->mvwstr( r, c, blank.replace( 1, s.length(), s ) ) ;
		this->refresh() ;

		Command *c = new Command(false) ;
		char cc = c->getCharCode() ;
		delete c ;

		if( isPrintable(cc) )
			s = s.append( 1, cc ) ;
		else if( cc == 8 ) // [Backspace]
			s = s.substr( 0, s.length() - 1 ) ;
		else if( cc == 27 ) // [Esc]
		{ // cancel entry
			s = "" ;
			done = true ;
		}
		else if( cc == 10 ) // [Enter]
		{ // accept entry
			done = true ;
		}
	} while( !done ) ;

	for( unsigned short i = c ; i < (c+length) ; i++ )
		this->mvwch( r, i, ' ' ) ;

	this->mvwstr( r, c, s ) ;

	log->lbuf << "Final entry: \"" << s << "\"" ;
	log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;

	return s ;
}

/* Prepares window for a doupdate() call. */
void Window::prepare(void)
{
	wnoutrefresh(this->win) ;
	log->write( Logger::TRACE, __FILE__, __LINE__, "Window::prepare()",
	            "Updated window contents." ) ;
	return ;
}

/*
 * Prepares window for a doupdate() call; offers option to also touch all
 * lines.
 */
void Window::prepare( bool touch )
{
	if( touch ) this->touch() ;
	this->prepare() ;
	return ;
}

/* Forces a redraw of the window. */
void Window::redraw(void)
{
	redrawwin(this->win) ;
	log->write( Logger::TRACE, __FILE__, __LINE__, "Window::redraw()",
	            "Forced redrawing of window." ) ;
	return ;
}
	
/* Refreshes the window display. */
void Window::refresh(void)
{
	wrefresh(this->win) ;
	log->write( Logger::TRACE, __FILE__, __LINE__, "Window::refresh()",
	            "Refreshed window." ) ;
	return ;
}

/* Refreshes the window display; offers option to also touch all lines. */
void Window::refresh( bool touch )
{
	if( touch ) this->touch() ;
	this->refresh() ;
	return ;
}

/* Touches the window to force a redraw. */
void Window::touch(void)
{
	log->write( Logger::TRACE, __FILE__, __LINE__, "Window::touch()",
	            "Touching window to force redraw." ) ;
	touchwin(this->win) ;
	return ;
}

// /// PRIVATE FUNCTIONS (Window) ////////////////////////////// (none) /// //

void Window::drawBorder(void)
{
	wborder( win, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE,
             ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER ) ;
	return ;
}

void Window::drawFooter(void)
{
	string func = "Window::drawFooter()" ;

	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Redrawing footer." ) ;

	mvwaddch( win, h-1, 0, ACS_LLCORNER ) ;
	mvwaddch( win, h-1, w-1, ACS_LRCORNER ) ;
	for( int i = 1 ; i < (w-1) ; i++ )
		mvwaddch( win, h-1, i, ACS_HLINE ) ;

	if( footer.compare("") ) // overwrite footer
	{
		string dfoot = // Displayed version of footer.
			( footer.length() > ( w - 4 ) ?
			  footer.substr( 0, (w-4) ) :
			  footer ) ;

		log->write( Logger::TRACE, __FILE__, __LINE__, func,
		            "Footer has non-blank value." ) ;

		mvwaddch( win, h-1, ( w - dfoot.length() - 3 ), ACS_RTEE ) ;
		mvwaddstr( win, h-1, ( w - dfoot.length() - 2 ), dfoot.c_str() ) ;
		mvwaddch( win, h-1, w-2, ACS_LTEE ) ;
	}

	return ;
}

void Window::drawHeader(void)
{
	string func = "Window::drawHeader()" ;

	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Redrawing header." ) ;

	mvwaddch( win, 0, 0, ACS_ULCORNER ) ;
	mvwaddch( win, 0, w-1, ACS_URCORNER ) ;
	for( int i = 1 ; i < (w-1) ; i++ )
		mvwaddch( win, 0, i, ACS_HLINE ) ;

	if( header.compare("") ) // overwrite header
	{
		string dhead = // Displayed version of footer.
			( header.length() > ( w - 4 ) ?
			  header.substr( 0, (w-4) ) :
			  header ) ;

		log->write( Logger::TRACE, __FILE__, __LINE__, func,
		            "Header has non-blank value." ) ;

		mvwaddch( win, 0, 1, ACS_RTEE ) ;
		mvwaddstr( win, 0, 2, dhead.c_str() ) ;
		mvwaddch( win, 0, ( 2 + dhead.length() ), ACS_LTEE ) ;
	}

	return ;
}

/* ************************************************************************* */
