/*
 * party.h
 * You may play with this source however you like, as long as you don't remove
 * this comment, claim undue credit, or make a profit on it.  Enjoy.
 * CHANGE HISTORY
 * 2012-01-25 (0.00)
 *  Created file.
 */

#ifndef _DUNGEONEER_H_
#include "dungeoneer.h"
#endif

#ifndef _PARTY_H_
#define _PARTY_H_

#include "actor.h"
using namespace std ;

/* *** TWEAKS ************************************************************** */

enum PartyException
{ // Yes, it was a personal struggle not to name this enum PartyFoul.
	X_PARTY_OK = 0,
	X_PARTY_NO_MEMBERS = -1,
	X_PARTY_INVALID_INDEX = -2,
	X_PARTY_FULL = -3,
	X_PARTY_NULL_MEMBER = -4,
	X_PARTY_INVALID_ACTOR = -5,
	X_PARTY_INVALID_SIZE = -6
} ;

#define MAX_PARTY_SIZE 3 // Default maximum number of party members.

/* *** COMPLEX TYPE DEFINITIONS ********************************* (none) *** */

/* *** GLOBAL PROCEDURE PROTOTYPES ****************************** (none) *** */

/* *** CLASS PROTOTYPES (needed for friendship) ***************** (none) *** */

template <class T> class Party ;
template <class T> class PartyMember ;

/* *** CLASS DEFINITIONS *************************************************** */

// Prototypes for templatized classes Party and PartyMember appear in
// party.cpp, as they must be colocated with the implementation.

/* ************************************************************************* */
#endif
