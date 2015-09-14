/*
 * window.h
 * You may play with this source however you like, as long as you don't remove
 * this comment, claim undue credit, or make a profit on it.  Enjoy.
 * CHANGE HISTORY
 * 2011-12-22
 *  Split from ui.h.
 */

#ifndef _DUNGEONEER_H_
#include "dungeoneer.h"
#endif

#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <string>
#include <ncurses.h>
using namespace std ;

/* *** TWEAKS *************************************************** (none) *** */

/* *** COMPLEX TYPE DEFINITIONS ********************************* (none) *** */

/* *** GLOBAL PROCEDURE PROTOTYPES ***************************************** */

char displayDialog( string message ) ; // Displays a one-line dialog box.

/* *** CLASS PROTOTYPES (needed for friendship) ***************** (none) *** */

/* *** CLASS DEFINITIONS *************************************************** */

/*
 * The Window class is a wrapper for the curses WINDOW object.  It defines a
 * default border style using the ncurses ACS macros, and provides member
 * functions that mirror the general-display procedure names rather than the
 * w-appended window procedure names.
 */
class Window
{
    public:
        // Static fields...
        // Static functions...
        // Constructors...

		// There is no default constructor.

		// Explicit...
		Window( unsigned short y, unsigned short x,
		        unsigned short h, unsigned short w ) ;

		// Dialog constructor: Centers itself based on width, height.
		Window( unsigned short h, unsigned short w ) ;

		// Destructor...
		~Window() ;

        // Accessors...
		unsigned short getHeight() ;
		unsigned short getIHeight() ;
		unsigned short getIWidth() ;
		unsigned short getWidth() ;

        // Mutators...
		void aoff( attr_t a ) ; // wattroff()
		void aon( attr_t a ) ; // wattron()
		void aset( attr_t a ) ; // wattrset()
		void chg( short count, attr_t a, short v ) ; // wchgat()
		void clear() ; // wclear()
		void clearFooter() ; // Clears the window footer.
		void clearHeader() ; // Clears the window header.
		void mvchg( unsigned short py, unsigned short px, short count, attr_t a, short v ) ; // mvwchgat()
		void mvwch( unsigned short py, unsigned short px, char c ) ; // wmvaddch()
		void mvwstr( unsigned short py, unsigned short px, string text ) ; // wmvaddstr()
		void setFooter( string nf ) ; // Set the footer.
		void setHeader( string nh ) ; // Set the header.
		void wch( char c ) ; // waddch()
		void wstr( string text ) ; // waddstr()

        // Other public functions...
		string getTextEntry( unsigned short r, unsigned short c,
		                     unsigned short length ) ;
		void prepare() ; // wnoutrefresh()
		void prepare( bool touch ) ; // wnoutrefresh() with option to touchwin()
		void redraw() ; // redrawwin()
		void refresh() ; // wrefresh()
		void refresh( bool touch ) ; // wrefresh() with option to touchwin()
		void touch() ; // touchwin()

    private:
        // Fields..
		WINDOW *win ; // An ncurses window object.
		unsigned short y, x, h, w ; // Upper left row, col, height, width.
		unsigned short cy, cx ; // Cursor position within window.
		string header, footer ;

        // Private functions...
		void drawBorder() ; // Redraws the window's border.
		void drawFooter() ; // Redraws the window's footer.
		void drawHeader() ; // Redraws the window's header.
};

/* ************************************************************************* */
#endif
