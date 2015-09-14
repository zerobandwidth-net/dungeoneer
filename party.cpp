/*
 * party.cpp
 * You may play with this source however you like, as long as you don't remove
 * this comment, claim undue credit, or make a profit on it.  Enjoy.
 * CHANGE HISTORY
 * 2012-01-25 (0.00)
 *  Created file.
 */

#ifndef _DUNGEONEER_H_
#include "dungeoneer.h"
#endif

#include <string>
#include "actor.h"
#include "debug.h"
#include "party.h"
using namespace std ;

/* *** EXTERNAL REFERENCES ************************************************* */

extern Logger *log ; // debug.cpp

/* *** LOCAL DATA *********************************************** (none) *** */

/* *** LOCAL PROCEDURE PROTOTYPES ******************************* (none) *** */

/* *** PROCEDURE DEFINITIONS ************************************ (none) *** */

/* *** CLASS DEFINITION (Party) ******************************************** */

// Class prototype colocated because class is templatized.

template <class T>
class Party
{
    public:
        // Static fields...
        // Static functions...

        // Constructors...
        Party<T>() ; // Default maximum size.
        Party<T>( unsigned short ms ) ; // Arbitrary maximum size.

        // Accessors...
        T * operator [] ( unsigned short ) throw( PartyException ) ;
        bool isLeader( unsigned short x )  throw( PartyException ) ;
        bool isLeader( T *a ) ; // True if actor is leader.
        T * getLeader() ; // Returns leader.
        unsigned short getMaxSize() ; // Returns max size.
        T * getMember( unsigned short x ) throw( PartyException ) ;
		T * getMemberAt( unsigned short row, unsigned short col ) ;
        unsigned short getSize() ; // Returns number of members.

        // Mutators...
        T * addMember( T *a ) throw( PartyException ) ;
        T * dropMember( unsigned short x ) throw( PartyException ) ;
        T * dropMember( T *a ) throw( PartyException ) ;
        T * setLeader( unsigned short x ) throw( PartyException ) ;
        T * setLeader( T *a ) throw( PartyException ) ;
        unsigned short setMaxSize( unsigned short ms ) throw( PartyException) ;

        // Other public functions...

    private:
        // Fields..
        PartyMember<T> *it ; // Iterator reference.
        PartyMember<T> *leader ; // Current party leader.
        unsigned short maxsize ; // Maximum size of party.
        unsigned short size ; // Actual size of party (number of members).
        PartyMember<T> *top ; // Top of member ring.

        // Private functions...
        PartyException iterateTo( unsigned short x ) ; // Sets "it" to index.
        PartyException iterateTo( T *a ) ; // Sets "it" to node of actor.
};

// /// STATIC FUNCTIONS (Party) ///////////////////////////////// (none) /// //

// /// CONSTRUCTORS (Party) //////////////////////////////////////////////// //

/*
 * Default constructor.
 */
template <class T>
Party<T>::Party()
{
	leader = NULL ;
	top = new PartyMember<T>( NULL, NULL, NULL ) ;
	it = NULL ;
	maxsize = MAX_PARTY_SIZE ;
	size = 0 ;

	log->lbuf << "Created party of max size " << maxsize << "." ;
	log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, "Party<>::Party()" ) ;
}

/*
 * Constructor with arbitrary maximum size.
 */
template <class T>
Party<T>::Party( unsigned short ms )
{
	leader = NULL ;
	top = new PartyMember<T>( NULL, NULL, NULL ) ;
	it = NULL ;
	maxsize = ms ;
	size = 0 ;

	log->lbuf << "Created party of max size " << maxsize << "." ;
	log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, "Party<>::Party(us)" );
}

// /// ACCESSORS (Party) /////////////////////////////////////////////////// //

/*
 * Element-at operator; calls Party::member(ushort).
 */
template <class T>
T * Party<T>::operator[] ( unsigned short x ) throw( PartyException )
{
	try { return this->getMember(x) ; }
	catch( PartyException e ) { throw e ; }
}

/*
 * Returns true if member at specified 0-based index is party's leader.
 * Throws PartyException:
 *   X_PARTY_INVALID_INDEX
 */
template <class T>
bool Party<T>::isLeader( unsigned short x ) throw( PartyException )
{
	try { iterateTo(x) ; }
	catch( PartyException e ) { throw e ; }
	if( leader == it ) return true ;
	else return false ;
}

/*
 * Returns true if specified actor is the leader of this party.
 */
template <class T>
bool Party<T>::isLeader( T *a ) { return( leader->member == a ) ; }

/*
 * Returns reference to party leader.
 */
template <class T>
T * Party<T>::getLeader(void) { return leader->member ; }

/*
 * Returns maximum party size.
 */
template <class T>
unsigned short Party<T>::getMaxSize(void) { return maxsize ; }

/*
 * Returns the member at the given 0-based index.
 * Throws PartyException:
 *   X_PARTY_INVALID_INDEX
 */
template <class T>
T * Party<T>::getMember( unsigned short x ) throw( PartyException )
{
	string func = "Party<>::getMember(ushort)" ; // debug

	try { iterateTo(x) ; }
	catch( PartyException e ) { throw e ; }

	log->lbuf << "Returning reference to " << it->member->getName()
		<< " (" << it->member << ")."
		;
	log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;

	return it->member ;
}

/*
 * Returns a reference to any party member standing at the specified (row,col)
 * location, or NULL if no such member exists.
 */
template <class T>
T * Party<T>::getMemberAt( unsigned short row, unsigned short col )
{
	if( top->next == NULL ) return NULL ;

	for( it = top->next ; it != top ; it = it->next )
		if( it->member->getRow() == row && it->member->getCol() == col )
			return it->member ;

	return NULL ;
}

/*
 * Returns current party size -- that is, the current number of members.
 */
template <class T>
unsigned short Party<T>::getSize(void) { return size ; }

// /// MUTATORS (Party) //////////////////////////////////////////////////// //

/*
 * Adds an actor as a member of the party.
 * Throws PartyException:
 *   X_PARTY_FULL
 *   X_PARTY_INVALID_ACTOR
 */
template <class T>
T * Party<T>::addMember( T *a ) throw( PartyException )
{
	string func = "Party<>::addMember(T*)" ; // debug

	if( size >= maxsize )
	{
		log->write( Logger::ERROR, __FILE__, __LINE__, func,
			"Can't add to full party." ) ;
		throw X_PARTY_FULL ;
	}

	if( a == NULL )
	{
		log->write( Logger::ERROR, __FILE__, __LINE__, func,
			"Can't add null reference to actor." ) ;
		throw X_PARTY_INVALID_ACTOR ;
	}

	it = new PartyMember<T>( a, ( top->prev == NULL ? top : top->prev ), top ) ;

	top->prev = it ;
	it->prev->next = it ;
	++size ;

	log->lbuf << "Added " << a->getName() << " (" << a << ") to party."
		<< "  New size: " << size
		;
	log->writeBuffer( Logger::INFO, __FILE__, __LINE__, func ) ;

	return it->member ;
}

/*
 * Drops member at the given 0-based index.
 * Throws PartyException:
 *   X_PARTY_INVALID_INDEX
 */
template <class T>
T * Party<T>::dropMember( unsigned short x ) throw( PartyException )
{
	string func = "Party<>::dropMember(ushort)" ; // debug

	try { iterateTo(x) ; }
	catch( PartyException e ) { throw e ; }

	it->next->prev = it->prev ;
	it->prev->next = it->next ;

	T *a = it->member ;
	delete it ;
	--size ;

	log->lbuf << "Dropped " << a->getName() << " (" << a << ") from party.  "
		<< "New size: " << size
		;
	log->writeBuffer( Logger::INFO, __FILE__, __LINE__, func ) ;

	return a ;
}

/*
 * Drops specified actor from party.
 * Throws PartyException:
 *   X_PARTY_NO_MEMBERS
 *   X_PARTY_INVALID_ACTOR
 */
template <class T>
T * Party<T>::dropMember( T *a ) throw( PartyException )
{
	string func = "Party<>::dropMember(T*)" ; // debug

	try { iterateTo(a) ; }
	catch( PartyException e ) { throw e ; }

	it->prev->next = it->next ;
	it->next->prev = it->prev ;

	delete it ;
	--size ;
	log->lbuf << a->getName() << " removed from party.  "
		<< "New size: " << size
		;
	log->writeBuffer( Logger::INFO, __FILE__, __LINE__, func ) ;
	return a ;
}

/*
 * Resets leadership designation to member at specified index.
 * Throws PartyException:
 *   X_PARTY_INVALID_INDEX
 */
template <class T>
T * Party<T>::setLeader( unsigned short x ) throw( PartyException )
{
	string func = "Party<>::setLeader(ushort)" ; // debug

	try { iterateTo(x) ; }
	catch( PartyException e ) { throw e ; }

	log->lbuf << "Leadership changed from " << leader->member->getName()
		<< " (" << leader->member << ")"
		;
	leader = it ;
	log->lbuf << " to " << leader->member->getName()
		<< " (" << leader->member << ")."
		;
	log->writeBuffer( Logger::INFO, __FILE__, __LINE__, func ) ;

	return leader->member ;
}

/*
 * Resets leadership designation to specified actor.
 * Throws PartyException:
 *   X_PARTY_NO_MEMBERS
 *   X_PARTY_INVALID_ACTOR
 */
template <class T>
T * Party<T>::setLeader( T *a ) throw( PartyException )
{
	string func = "Party<>::setLeader(T*)" ; // debug

	try { iterateTo(a) ; }
	catch( PartyException e ) { throw e ; }

	log->lbuf << "Leadership changed from " << leader->member->getName()
		<< " (" << leader->member << ")"
		;
	leader = it ;
	log->lbuf << " to " << leader->member->getName()
		<< " (" << leader->member << ")."
		;
	log->writeBuffer( Logger::INFO, __FILE__, __LINE__, func ) ;

	return leader->member ;
}

/*
 * Changes max party size.
 * Throws PartyException:
 *   X_PARTY_INVALID_SIZE
 */
template <class T>
unsigned short Party<T>::setMaxSize( unsigned short ms ) throw( PartyException )
{
	string func = "Party<>::setMaxSize(ushort)" ; // debug

	if( ms < size )
	{
		log->lbuf << "Can't shrink party to size " << ms
			<< "; party already has " << size
			<< ( size == 1 ? " member." : " members." )
			;
		log->writeBuffer( Logger::ERROR, __FILE__, __LINE__, func ) ;
		throw X_PARTY_INVALID_SIZE ;
	}

	maxsize = ms ;

	log->lbuf << "Reset max party size to " << maxsize << "." ;
	log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;

	return maxsize ;
}

// /// OTHER PUBLIC FUNCTIONS (Party) /////////////////////////// (none) /// //

// /// PRIVATE FUNCTIONS (Party) /////////////////////////////////////////// //

/*
 * Iterates "it" cursor to specified 0-based index.  Returns exception code to
 * be thrown, or 0.
 */
template <class T>
PartyException Party<T>::iterateTo( unsigned short x )
{
	string func = "Party<>::iterateTo(ushort)" ; // debug

	if( x >= size )
	{
		log->lbuf << "Invalid index " << x << " for size " << size << "." ;
		log->writeBuffer( Logger::ERROR, __FILE__, __LINE__, func ) ;
		return X_PARTY_INVALID_INDEX ;
	}

	it = top->next ;

	for( unsigned short i = 0 ; i < x ; i++ )
	{
		if( it == top )
		{
			log->lbuf << "Index " << x << " wrapped at " << i << "." ;
			log->writeBuffer( Logger::ERROR, __FILE__, __LINE__, func ) ;
			return X_PARTY_INVALID_INDEX ;
		}

		it = it->next ;
	}

	return X_PARTY_OK ;
}

/*
 * Iterates "it" cursor to node containing actor.  Returns exception to be
 * thrown, or 0.
 */
template <class T>
PartyException Party<T>::iterateTo( T *a )
{
	string func = "Party<>::iterateTo(T*)" ; // debug

	if( size <= 0 || top->next == NULL )
	{
		log->write( Logger::ERROR, __FILE__, __LINE__, func,
			"Can't search empty party." ) ;
		return X_PARTY_NO_MEMBERS ;
	}

	if( a == NULL )
	{
		log->write( Logger::ERROR, __FILE__, __LINE__, func,
			"Can't search based on null reference." ) ;
		throw X_PARTY_INVALID_ACTOR ;
	}

	for( it = top->next ; it != top ; it = it->next )
	{
		if( it->member == a )
		{
			log->lbuf << "Actor " << a->getName() << " (" << a
				<< ") found in party."
				;
			log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
			return X_PARTY_OK ;
		}
	}

	log->lbuf << "No match found for " << a->getName() << " (" << a << ")." ;
	log->writeBuffer( Logger::ERROR, __FILE__, __LINE__, func ) ;

	return X_PARTY_INVALID_ACTOR ;
}

/* *** CLASS DEFINITION (PartyMember) ************************************** */

template <class T>
class PartyMember
{
    public:
        friend class Party<T> ;
        // Static fields...
        // Static functions...

        // Constructors...
        PartyMember() ;
        PartyMember( T *m, PartyMember *p, PartyMember *n ) ;

        // Accessors...
        // Mutators...
        // Other public functions...

    private:

        // Fields..
        T *member ;
        PartyMember *prev ;
        PartyMember *next ;

        // Private functions...
};

// /// STATIC FUNCTIONS (PartyMember) /////////////////////////// (none) /// //

// /// CONSTRUCTORS (PartyMember) ////////////////////////////////////////// //

template <class T>
PartyMember<T>::PartyMember()
{
	member = NULL ;
	prev = NULL ;
	next = NULL ;
}

template <class T>
PartyMember<T>::PartyMember( T *m, PartyMember *p, PartyMember *n )
{
	member = m ;
	prev = p ;
	next = n ;
}

// /// ACCESSORS (PartyMember) ///////////////////////////////////////////// //

// /// MUTATORS (PartyMember) ////////////////////////////////////////////// //

// /// OTHER PUBLIC FUNCTIONS (PartyMember) ///////////////////// (none) /// //

// /// PRIVATE FUNCTIONS (PartyMember) ////////////////////////// (none) /// //

/* ************************************************************************* */
