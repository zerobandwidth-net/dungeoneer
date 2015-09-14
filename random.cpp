/*
 * random.cpp
 * You may play with this source however you like, as long as you don't remove
 * this comment, claim undue credit, or make a profit on it.  Enjoy.
 * CHANGE HISTORY
 * 2011-11-17
 *  Created file.
 */

#ifndef _DUNGEONEER_H_
#include "dungeoneer.h"
#endif

#include <cstdlib>
#include <ctime>
#include "random.h"
using namespace std ;

/* *** EXTERNAL REFERENCES ************************************** (none) *** */

/* *** LOCAL DATA *********************************************** (none) *** */

/* *** LOCAL PROCEDURE PROTOTYPES ******************************* (none) *** */

/* *** PROCEDURE DEFINITIONS *********************************************** */

void set_random_seed( void )
{
	srand( time(NULL) ) ;
	return ;
}

unsigned short roll( unsigned short rolls, unsigned short die, short bonus )
{
	short total = 0 ;
	short r ;

	for( int i = 0 ; i < (int)rolls ; i++ )
	{
		r = ( (short)( rand() % (short)die ) ) + 1 ;
		if( r < 0 ) r *= -1 ;
		total += r ;
	}

	if( (total+bonus) < 0 ) return 0 ;
	else return ( total + bonus ) ;
}

unsigned short randShort( unsigned short min, unsigned short max )
{
	return ( (unsigned short)( rand() % (max-min+1) ) + min ) ;
}

unsigned int randInt( unsigned int min, unsigned int max )
{
	return ( (unsigned int)( rand() % (max-min+1) ) + min ) ;
}

bool randPercent( unsigned short chance )
{
	return ( (rand()%100) < chance ) ;
}

bool coin_toss( void )
{
	if( (rand() % 2) == 0 ) return false ;
	else return true ;
}

/* ************************************************************************* */
