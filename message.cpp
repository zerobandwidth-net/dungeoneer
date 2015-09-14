/*
 * message.cpp
 * You may play with this source however you like, as long as you don't remove
 * this comment, claim undue credit, or make a profit on it.  Enjoy.
 * CHANGE HISTORY
 * 2012-01-06 (0.00)
 *  Created file.
 */

#ifndef _DUNGEONEER_H_
#include "dungeoneer.h"
#endif

#include <deque>
#include <sstream>
#include <string>
#include "actor.h"
#include "ally.h"
#include "debug.h"
#include "message.h"
#include "party.h"
#include "ui.h"
using namespace std ;

/* *** EXTERNAL REFERENCES ************************************************* */

extern Logger *log ; // debug.cpp
extern string statusEffectNames[STATUS_EFFECTS] ; // ui.cpp

/* *** LOCAL DATA *********************************************** (none) *** */

/* *** LOCAL PROCEDURE PROTOTYPES ******************************* (none) *** */

/* *** PROCEDURE DEFINITIONS ************************************ (none) *** */

/* *** CLASS DEFINITION (MessageEngine) ************************************ */

// /// STATIC FUNCTIONS (MessageEngine) ///////////////////////// (none) /// //

// /// CONSTRUCTORS (MessageEngine) //////////////////////////////////////// //

/*
 * Default constructor.
 */
MessageEngine::MessageEngine()
{
	this->lines = new deque<string*>() ;
	this->height = MSG_AREA_SIZE ;
}

/*
 * Constructor allowing explicit height specification.
 */
MessageEngine::MessageEngine( unsigned short height )
{
	this->lines = new deque<string*>() ;
	this->height = height ;
}

// /// ACCESSORS (MessageEngine) /////////////////////////////////////////// //

// /// MUTATORS (MessageEngine) //////////////////////////////////////////// //

/*
 * Flushes the message buffer.
 */
void MessageEngine::flush(void)
{
	while( !lines->empty() ) lines->pop_front() ;
	return ;
}

/*
 * Pops the oldest message off the buffer and returns it.
 */
string MessageEngine::pop(void)
{
	string *msg = lines->front() ;
	lines->pop_front() ;
	return *msg ;
}

/*
 * Pushes a new message onto the buffer.
 */
void MessageEngine::push( string msg )
{
	lines->push_back( new string(msg) ) ;
	return ;
}

/*
 * Pushes the contents of the message buffer onto the queue.
 */
void MessageEngine::pushBuffer(void)
{
	lines->push_back( new string( this->buf.str() ) ) ;
	this->buf.str("") ;
	return ;
}

// /// OTHER PUBLIC FUNCTIONS (MessageEngine) ////////////////////////////// //

/*
 * Visually clears the message area.
 */
void MessageEngine::clear(void)
{
	for( unsigned short r = 0 ; r < this->height ; r++ )
	for( unsigned short c = 0 ; c < DCOLS ; c++ )
		mvaddch( r, c, ' ' ) ;
	return ;
}

/*
 * Draws the current message buffer into the message area.  If the buffer has
 * more lines than the defined height of the area, this function will start
 * intercepting user input, allowing the user to page through the messages
 * until they're all displayed.
 */
void MessageEngine::draw(void)
{
	// COME BACK TO THIS: Insert handling of a buffer that's too big.

	for( unsigned int r = 0 ; r < this->lines->size() ; r++ )
	{
		const char *c = (this->lines->at(r))->c_str() ;
		mvaddstr( r, 0, c ) ;
	}

	return ;
}

/*
 * STATUS LINE FORMAT

0....:....1....:....2....:....3....:....4....:....5....:....6....:....7....:....
 * nnnnnnnnnn Lv.xx (xx%) HP hhh/hhh ACac starving hsacpbhpptfct

 *      Largo Lv.95 (14%) HP 345/432 AC 6          h    b    f
       Margot Lv.92 (86%) HP 782/986 AC10  hungry  h          c
      Khargoh Lv.25 ( 7%) HP  62/ 62 AC 3              p       t
   Burdenturd Lv. 1 ( 0%) HP   6/  6 AC 1 starving  sac bhppt

 */

/*
 * Draws the status of the player's party on the bottom of the screen.
 */
void MessageEngine::drawStatus( Party *p )
{
	stringstream sline ( stringstream::out ) ;

	for( unsigned short m = 0 ; m < p->size() ; m++ )
	{
		sline << "   " // Future: Indicates party leader.
			<< pad( p[m]->getName(), 10 )
			<< ( p[m]->getActualLevel() > p[m]->getLevel() ? " ELv" : " Lv " )
			<< pad( p[m]->getLevel(), 2 )
			<< " (" << pad( p[m]->getXPPercent(), 2 ) << "%) "
			<< "HP " << pad( p[m]->getHP(), 3 )
			<< "/" << pad( p[m]->getMaxHP(), 3 )
			<< " AC" << pad( p[m]->getAC(), 2 )
			;
		switch( p[m]->getHungerState() )
		{
			case (Actor::HUNGRY):   sline << "  hungry  " ; break ;
			case (Actor::WEAK):     sline << "   weak   " ; break ;
			case (Actor::FAINT):    sline << "  faint   " ; break ;
			case (Actor::STARVING): sline << " starving " ; break ;
			default:                sline << "          " ;
		}
		for( unsigned short e = 0 ; e < STATUS_EFFECTS ; e++ )
		{
			if( p[m]->getStatus(StatusEffect(e)) )
				sline << statusEffectNames[e].substr(0,1) ;
			else
				sline << " " ;
		}

		mvaddstr( (DROWS-MAX_PARTY_SIZE+m), 0, (sline.str()).c_str() ) ;
		sline.str("") ;
	}

	return ;
}

// /// PRIVATE FUNCTIONS (MessageEngine) //////////////////////// (none) /// //

/* ************************************************************************* */
