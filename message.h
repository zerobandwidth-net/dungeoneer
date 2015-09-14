/*
 * message.h
 * You may play with this source however you like, as long as you don't remove
 * this comment, claim undue credit, or make a profit on it.  Enjoy.
 * CHANGE HISTORY
 * 2012-01-06 (0.00)
 *  Created file.
 */

#ifndef _DUNGEONEER_H_
#include "dungeoneer.h"
#endif

#ifndef _STATUS_H_
#define _STATUS_H_

#include <deque>
#include <sstream>
#include <string>
#include "actor.h"
#include "ally.h"
using namespace std ;

/* *** TWEAKS ************************************************************** */

#define MSG_AREA_SIZE 2 // Used as default; may be overridden by constructor.

/* *** COMPLEX TYPE DEFINITIONS ********************************* (none) *** */

/* *** GLOBAL PROCEDURE PROTOTYPES ****************************** (none) *** */

/* *** CLASS PROTOTYPES (needed for friendship) ***************** (none) *** */

/* *** CLASS DEFINITIONS *************************************************** */

/*
 * The MessageEngine class provides an API for managing two parts of the main
 * gameplay screen: the message area at the top, and the status bar at the
 * bottom.  The message area is not a window, and is not managed as a separate
 * ncurses WINDOW object; it is instead a reserved area of the main ncurses
 * screen that is managed by this object.
 */
class MessageEngine
{
	public:
		// Static fields...

		// Static functions...

		// Constructors...
		MessageEngine() ; // default constructor
		MessageEngine( unsigned short height ) ; // Explicit message area size.

		// Public fields...
		stringstream buf ; // Buffer for building up messages.

		// Accessors...

		// Mutators...
		void flush() ; // Clears the message queue.
		string pop() ; // Pops a message off the queue.
		void push( string msg ) ; // Pushes a message onto the queue.
		void pushBuffer() ; // Push the buffer's contents onto the queue.

		// Other public functions...
		void clear() ; // Blanks the message area.
		void draw() ; // Refreshes the message area.
		void drawStatus( Party *p ) ;

	private:
		// Fields..
		deque<string*> *lines ; // Buffer of strings to be shown.
		unsigned short height ; // Size of the message area.

		// Private functions...
};

/* ************************************************************************* */
#endif
