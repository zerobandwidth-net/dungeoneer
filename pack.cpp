/*
 * pack.cpp
 * You may play with this source however you like, as long as you don't remove
 * this comment, claim undue credit, or make a profit on it.  Enjoy.
 * CHANGE HISTORY
 * 2011-11-22 (0.00)
 *  Created file.
 */

#ifndef _DUNGEONEER_H_
#include "dungeoneer.h"
#endif

#include <string>
#include "command.h"
#include "debug.h"
#include "inventory.h"
#include "item.h"
#include "pack.h"
using namespace std ;

/* *** EXTERNAL REFERENCES ************************************************* */

extern Logger *log ; // debug.cpp

/* *** LOCAL DATA *********************************************** (none) *** */

/* *** LOCAL PROCEDURE PROTOTYPES ******************************* (none) *** */

/* *** PROCEDURE DEFINITIONS ************************************ (none) *** */

/* *** CLASS DEFINITION (Pack) ********************************************* */

// /// STATIC FUNCTIONS (Pack) ////////////////////////////////// (none) /// //

// /// CONSTRUCTORS (Pack) ///////////////////////////////////////////////// //

// Default constructor.
Pack::Pack()
{
	string func = "Pack::Pack()" ;

	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Constructing default pack..." ) ;

	top = new PackNode() ;
	maxitems = Pack::INITIAL_SIZE ;
	t_count = 0 ;
	current = top ;
	traversalFilter = 0 ;

	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Pack constructed." ) ;
}

Pack::Pack( Pack *orig )
{
	string func = "Pack::Pack(Pack*)" ;

	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Constructing duplicate pack..." ) ;

	top = new PackNode() ;

	maxitems = orig->maxitems ;

	log->lbuf << "Max Items: " << maxitems ;
	log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;

	t_count = 0 ;
	current = top ;

	Item *i  ;

	// Make a copy of each item in the original pack, and add it to this one.
	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Initiating item ring copy..." ) ;
	while( ( i = orig->getNextItem() ) != NULL )
	{
		Item *j = new Item(i) ;
		log->write( Logger::TRACE, __FILE__, __LINE__, func,
		            "Adding next item from original pack..." ) ;
		(void)addItem(j,false) ;
		log->write( Logger::TRACE, __FILE__, __LINE__, func,
		            "Item added." ) ;
	}

	traversalFilter = 0 ;
}

// /// ACCESSORS (Pack) //////////////////////////////////////////////////// //

/*
 * Element-at operator; calls Pack::getItem(ushort).
 */
Item * Pack::operator[] ( unsigned short x )
{
	return this->getItem(x) ;
}

/*
 * Returns a description of the item at the current iterator position.
 */
string Pack::describeItem( void )
{
	string func = "Pack::describeItem()" ;

	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Describing item..." ) ;

	if( current == top )
	{
		log->write( Logger::TRACE, __FILE__, __LINE__, func,
		            "Pack is empty." ) ;
		return EMPTY_ITEM ;
	}

	if( current->item == NULL )
	{
		log->write( Logger::TRACE, __FILE__, __LINE__, func,
		            "Bad current pointer." ) ;
		return EMPTY_ITEM ;
	}

	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Fetching item description for return..." ) ;

	return (current->item)->getDesc(false) ;
}

/*
 * Return the total count of items in the pack.
 */
unsigned short Pack::getCount(void)
{
	string func = "Pack::getCount()" ;

	log->lbuf << "Get total pack count: " << t_count ;
	log->writeBuffer( Logger::INFO, __FILE__, __LINE__, func ) ;

	return t_count ;
}

/*
 * Returns a pointer to the current item in the pack.
 */
Item * Pack::getItem( void )
{
	string func = "Pack::getItem()" ;

	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Fetching item: " ) ;
	(void)((current->item)->getDiag()) ;

	return current->item ;
}

/*
 * Gets the item at the specified 0-based index.  If the traversal wraps back
 * around to the top node before the index is reached, a NULL pointer is
 * returned.  A better design might return some sort of exception, and if this
 * were Java instead of C++, that's probably what I'd do.
 */
Item * Pack::getItem( unsigned short x )
{
	string func = "Pack::getItem(ushort)" ; // debug

	if( x > t_count )
	{
		log->lbuf << "Index " << x << " out of bounds." ;
		log->writeBuffer( Logger::ERROR, __FILE__, __LINE__, func ) ;
		return NULL ;
	}

	temp = top->next ;

	for( unsigned short i = 0 ; i < x ; i++ )
	{
		if( temp == top )
		{
			log->lbuf << "Index " << i << " wrapped to top." ;
			log->writeBuffer( Logger::ERROR, __FILE__, __LINE__, func ) ;
			return NULL ;
		}
		temp = temp->next ;
	}

	return temp->item ;
}

/*
 * Move the iterator to the next item in the pack, and return it.  Note that,
 * if the next node is the top of the ring, this function will return NULL.
 */
Item * Pack::getNextItem( void )
{
	string func = "Pack::getNextItem()" ;

	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Iterating to next item..." ) ;

	if( traversalFilter != 0 )
	{
		log->lbuf << "Traversal filter: " << traversalFilter ;
		log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
	}

	do { current = current->next ; }
	while( ( traversalFilter != 0 )
	    && ( current->item != NULL )
	    && ( (current->item)->getCategory() != traversalFilter )
	     ) ;

	if( current->item == NULL )
	{
		if( traversalFilter != 0 )
		{
			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "No matching item found." ) ;
		}
		else
		{
			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "This item is undefined." ) ;
		}
	}
	else
	{
		log->write( Logger::TRACE, __FILE__, __LINE__, func,
		            "Found item: " ) ;
		(void)((current->item)->getDiag()) ;
	}

	return current->item ;
}

/*
 * Verifies the presence of an existing item in the pack.  Unlike the logic
 * that condenses items, this function is actually verifying that the specified
 * object in memory is in fact the target of a PackNode->item reference.
 */
bool Pack::hasItem( Item *item )
{
	string func = "Pack::hasItem(Item*)" ; // debug
	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Searching for item..." ) ;

	for( temp = top->next ; temp != top ; temp = temp->next )
		if( temp->item == item ) return true ;

	return false ;
}

/*
 * Indicates whether the pack is full and cannot take more items.
 */
bool Pack::isFull(void) { return ( t_count >= maxitems ) ; } 

// /// MUTATORS (Pack) ///////////////////////////////////////////////////// //

/*
 * Adds an item to the pack, and returns true if the add was successful.  This
 * method does not move the pack's iterator; it simply adds the item at the
 * "end" of the ring, just prior to the top node.
 */
bool Pack::addItem( Item *pi, bool condense )
{
	string func = "Pack::addItem(Item*,bool)" ;

	log->lbuf << "Adding item"
	          << ( condense ? " and condensing" : "" )
	          ;
	log->writeBuffer( Logger::INFO, __FILE__, __LINE__, func ) ;

	if( pi == NULL )
	{
		log->write( Logger::TRACE, __FILE__, __LINE__, func,
		            "Null reference passed in; nothing added." ) ;
		return false ;
	}

	if( t_count >= maxitems )
	{
		log->write( Logger::INFO, __FILE__, __LINE__, func,
		            "Can't add; pack is full." ) ;
		return false ;
	}

	if( condense && pi->isStackable() && (top->next != NULL) )
	{ // Merge the new item into a matching item stack already in the pack.
		log->write( Logger::TRACE, __FILE__, __LINE__, func,
		            "Checking stackability..." ) ;

		temp = top->next ;
		while( ( temp->item != NULL ) && ( temp != top ) )
		{
			if( (temp->item)->couldStackWith(pi) )
			{
				if( (temp->item)->absorb(pi) )
				{
					log->write( Logger::INFO, __FILE__, __LINE__, func,
					            "Item was absorbed into an existing item." ) ;
					return true ;
				}
			}
			temp = temp->next ;
		}
	}
	// If the above has not already returned, then attempt to add the item
	// assuming that it was not stackable.

	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Creating pack node..." ) ;

	temp = 
		new PackNode( pi,
		              ( top->prev == NULL ? top : top->prev ),
		              top
		            ) ;

	top->prev = temp ;
	temp->prev->next = temp ;

	++t_count ;

	log->lbuf << "New count: " << t_count ;
	log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;

	return true ;
}

/*
 * Clears the traversal filter and resets traversal.
 */
void Pack::clearFilter(void)
{
	traversalFilter = 0 ;
	reset() ;
	return ;
}

/*
 * Inserts an item into the pack at the iterator's current position.  The
 * additional boolean sel indicates whether the iterator should then select
 * the new node, or leave the old node selected.
 */
bool Pack::insertItem( Item *pi, bool sel )
{
	string func = "Pack::insertItem(Item*,bool)" ;

	log->write( Logger::INFO, __FILE__, __LINE__, func,
	            "Inserting item into pack." ) ;

	if( t_count >= maxitems )
	{
		log->write( Logger::INFO, __FILE__, __LINE__, func,
		            "Can't add; pack is full." ) ;
		return false ;
	}

	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Creating pack node..." ) ;
	temp =
		new PackNode( pi,
		              current,
		              ( current->next == NULL ? top : current->next )
		            ) ;

	current->next = temp ;
	(temp->next)->prev = temp ;

	if( sel )
	{
		log->write( Logger::TRACE, __FILE__, __LINE__, func,
		            "Selecting new item node." ) ;
		current = temp ;
	}

	++t_count ;

	log->lbuf << "New count: " << t_count ;
	log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;

	return true ;
}

/*
 * Pops the current node out of the pack, and returns the corresponding item.
 */
Item * Pack::popItem( void )
{
	string func = "Pack::popItem()" ; // debug

	temp = current ;

	(temp->prev)->next = temp->next ;
	(temp->next)->prev = temp->prev ;
	current = temp->next ;

	--t_count ;

	log->lbuf << "Removed item; new count: " << t_count << "." ;
	log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;

	Item *i = temp->item ;
	delete temp ;
	return i ;
}

/*
 * Pops a specific item if it's in the pack; otherwise, returns NULL.
 */
Item * Pack::popItem( Item *item )
{
	string func = "Pack::popItem(Item*)" ; // debug
	log->write( Logger::INFO, __FILE__, __LINE__, func,
	            "Removing item..." ) ;

	for( temp = top->next ; temp != top ; temp = temp->next )
	{
		if( temp->item == item )
		{
			(temp->prev)->next = temp->next ;
			(temp->next)->prev = temp->prev ;
			if( current == temp ) current = temp->next ;
			delete temp ;
			--t_count ;

			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Item found.  Returning pointer." ) ;

			return item ;
		}
	}

	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Item not found.  Returning NULL." ) ;

	return NULL ;
}

/*
 * Resets the iterator to the top of the pack ring.
 */
void Pack::reset(void) { current = top ; }

/*
 * Sets a traversal filter for the pack.  During traversal, the value of this
 * filter is compared to the "category" field in the Item object in each node.
 */
void Pack::setFilter( unsigned short f )
{
	if( f >= ITEM_TYPES ) return ; // If invalid, return without changing.

	traversalFilter = f ;
	this->reset() ;
	return ;
}

// /// OTHER PUBLIC FUNCTIONS (Pack) //////////////////////////// (none) /// //

/*
 * Counts how many items of a given type are stored in the pack.  The function
 * uses its own counter, thus it does not mess up the "current" pointer for the
 * pack.
 */
unsigned short Pack::countItems( unsigned short filter )
{
	string func = "Pack::countItems(ushort)" ; // debug

	if( filter == K_ALLITEMS )
	{
		log->write( Logger::TRACE, __FILE__, __LINE__, func,
		            "Counting all items." ) ;
		return t_count ;
	}

	if( filter == K_EQUIPMENT )
	{
		log->write( Logger::TRACE, __FILE__, __LINE__, func,
		            "Counting equipment." ) ;
		// Equipment is held by the rogue, not the pack.
	    return 0 ;
	}

	log->lbuf << "Counting items with filter: " << filter ;
	log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;

	unsigned short count = 0 ;
	PackNode *c ;

	for( c = top->next ; c != top ; c = c->next )
		if( ((c->item)->getCategory()) == filter ) count++ ;

	log->lbuf << "Filtered count: " << count ;
	log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;

	return count ;
}

/*
 * Counts how many items of a given type are stored in the pack.  The function
 * uses its own counter, thus it does not mess up the "current" pointer for the
 * pack.  The limits on the item kind range are INCLUSIVE; the number of item
 * kinds included in the filter will be fkind2-fkind1+1.
 */
unsigned short Pack::countItems( unsigned short fcat,
                                 unsigned short fkind1,
                                 unsigned short fkind2 )
{
	string func = "Pack::countItems(ushort,ushort,ushort)" ; // debug

	if( fkind2 < fkind1 )
	{
		log->write( Logger::WARNING, __FILE__, __LINE__, func,
		            "Invalid item kind filter; returning 0." ) ;
		return 0 ;
	}

	log->lbuf << "Counting items with category " << fcat
	          << " and kind between " << fkind1 << " and " << fkind2 << "."
	          ;
	log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;

	unsigned short count = 0 ;
	PackNode *c ;

	for( c = top->next ; c != top ; c = c->next )
	{
		if( (c->item)->getCategory() == fcat
		 && (c->item)->getKind() >= fkind1
		 && (c->item)->getKind() <= fkind2 )
		{
			++count ;
		}
	}

	log->lbuf << "Filtered count: " << count ;
	log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;

	return count ;
}

/*
 * Constructs a new Inventory object containing all items in the pack.
 */
Inventory * Pack::toInventory(void)
{
	if( this->t_count == 0 ) return NULL ;

	Inventory *inv = new Inventory(this->t_count) ;
	int i = 0 ;

	for( temp = top->next ; temp != top ; temp = temp->next )
		inv->setItem( i++, temp->item ) ;

	return inv ;
}

/*
 * Constructs a new Inventory object containing all items in the pack that
 * belong to the specified item category filter.
 */
Inventory * Pack::toInventory( unsigned short filter )
{
	string func = "Pack::toInventory(ushort)" ; // debug

	unsigned short size = this->countItems( filter ) ;

	if( size == 0 )
	{
		log->write( Logger::TRACE, __FILE__, __LINE__, func,
		            "Returning empty inventory." ) ;
		return new Inventory(0) ;
	}

	Inventory *inv = new Inventory( size ) ;
	unsigned short i = 0 ;

	for( temp = top->next ; temp != top ; temp = temp->next )
	{
		if( (temp->item)->getCategory() == filter )
			inv->setItem( i++, temp->item ) ;
	}

	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Returning filtered inventory of items." ) ;

	return inv ;
}

/*
 * Constructs a new Inventory object containing all items in the pack that
 * belong to the specified item category filter and fall between the given
 * minimum and maximum filter constants.  Both range limits are INCLUSIVE;
 * the number of kinds included in the filter will be fkind2-fkind1+1.
 */
Inventory * Pack::toInventory( unsigned short fcat,
                               unsigned short fkind1,
                               unsigned short fkind2 )
{
	string func = "Pack::toInventory(ushort,ushort,ushort)" ; // debug

	if( fkind2 < fkind1 )
	{
		log->write( Logger::WARNING, __FILE__, __LINE__, func,
		            "Invalid item kind filter; returning 0." ) ;
		return 0 ;
	}

	unsigned short size = this->countItems( fcat, fkind1, fkind2 ) ;

	if( size == 0 )
	{
		log->write( Logger::TRACE, __FILE__, __LINE__, func,
		            "Returning empty inventory." ) ;
		return new Inventory(0) ;
	}

	Inventory *inv = new Inventory( size ) ;
	unsigned short i = 0 ;

	for( temp = top->next ; temp != top ; temp = temp->next )
	{
		if( (temp->item)->getCategory() == fcat
		 && (temp->item)->getKind() >= fkind1
		 && (temp->item)->getKind() <= fkind2 )
		{
			inv->setItem( i++, temp->item ) ;
		}
	}

	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Returning filtered inventory of items." ) ;

	return inv ;
}

// /// PRIVATE CLASS METHODS (Pack) //////////////////////////////////////// //

/* *** CLASS DEFINITION (PackNode) ***************************************** */

// /// STATIC FUNCTIONS (PackNode) ////////////////////////////// (none) /// //

// /// CONSTRUCTORS (PackNode) ///////////////////////////////////////////// //

// Default constructor.
PackNode::PackNode(void)
{
	item = NULL ;
	prev = NULL ;
	next = NULL ;
}

// Explicit constructor.
PackNode::PackNode( Item *i, PackNode *p, PackNode *n )
{
	item = i ;
	prev = p ;
	next = n ;
}

// /// ACCESSORS (PackNode) ///////////////////////////////////// (none) /// //

// /// MUTATORS (PackNode) ////////////////////////////////////// (none) /// //

// /// OTHER PUBLIC FUNCTIONS (PackNode) //////////////////////// (none) /// //

// /// PRIVATE FUNCTIONS (PackNode) ///////////////////////////// (none) /// //

/* ************************************************************************* */
