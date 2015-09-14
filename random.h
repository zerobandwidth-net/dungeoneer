/*
 * random.h
 * You may play with this source however you like, as long as you don't remove
 * this comment, claim undue credit, or make a profit on it.  Enjoy.
 * CHANGE HISTORY
 * 2011-11-17
 *  Created file.
 */

#ifndef _DUNGEONEER_H_
#include "dungeoneer.h" 
#endif

#ifndef _RANDOM_H_
#define _RANDOM_H_

/* *** TWEAKS *************************************************** (none) *** */

/* *** COMPLEX TYPE DEFINITIONS ********************************* (none) *** */

/* *** GLOBAL PROCEDURE PROTOTYPES ***************************************** */

void set_random_seed() ;

unsigned short roll( unsigned short rolls, unsigned short die, short bonus ) ; // rolls a die

unsigned short randShort( unsigned short min, unsigned short max ) ;

unsigned int randInt( unsigned int min, unsigned int max ) ;

bool randPercent( unsigned short chance ) ;

bool coin_toss( void ) ;

/* *** CLASS PROTOTYPES (needed for friendship) ***************** (none) *** */

/* *** CLASS DEFINITIONS **************************************** (none) *** */

/* ************************************************************************* */
#endif
