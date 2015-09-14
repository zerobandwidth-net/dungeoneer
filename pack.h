/*
 * pack.h
 * You may play with this source however you like, as long as you don't remove
 * this comment, claim undue credit, or make a profit on it.  Enjoy.
 * CHANGE HISTORY
 * 2011-11-22 (0.00)
 *  Created file.
 */

#ifndef _DUNGEONEER_H_
#include "dungeoneer.h"
#endif

#ifndef _PACK_H_
#define _PACK_H_

#include <string>
#include "inventory.h"
#include "item.h"

/* *** TWEAKS ************************************************************** */

#define EMPTY_ITEM "Nothing."

/* *** COMPLEX TYPE DEFINITIONS ********************************* (none) *** */ 

/* *** GLOBAL PROCEDURE PROTOTYPES ****************************** (none) *** */

/* *** CLASS PROTOTYPES (needed for friendship) **************************** */

class Pack ;
class PackNode ;

/* *** CLASS DEFINITIONS *************************************************** */

/*
 * The Pack object represents a backpack that carries items.  Like the original
 * "rogue" code, this can be used to represent the player character's pack, or
 * as a structure to hold a dungeon's treasure, or whatever.  It is designed to
 * be used like the Ring data structure (net.zerobandwidth.util.Ring), in that
 * nodes reference each other bi-directionally, and items may be added to
 * either the front or back of the node list, based on relative position to the
 * empty "top" node.  It also includes an internal definition for its own
 * iterator.
 */
class Pack
{
	public:

		// Static constants...
		static const unsigned short INITIAL_SIZE = 30 ;

		// Constructors...
		Pack() ; // Default constructor.
		Pack( Pack *orig ) ; // Copy constructor.

		// Accessors...
		Item * operator [] ( unsigned short x ) ;
		string describeItem() ; // Returns description of current item.
		unsigned short getCount() ; // Returns count of items in pack.
		Item * getItem() ; // Gets the item at the iterator.
		Item * getItem( unsigned short x ) ; // Gets item at 0-based index.
		Item * getNextItem() ; // Get next item, move iterator pointer forward.
		bool hasItem( Item *item ) ; // Verifies presence of item in pack.
		bool isFull() ;

		// Mutators...
		bool addItem( Item *pi, bool condense ) ; // Adds item to pack.
		void clearFilter() ; // Unsets traversal filter.
		bool insertItem( Item *pi, bool sel ) ; // Inserts an item at current.
		Item * popItem() ; // Pops item at iterator.
		Item * popItem( Item *item ) ; // Pops a specific item if it's in the pack.
		void reset() ; // Resets the iterator to the top.
		void setFilter( unsigned short f ) ; // Set traversal filter.

		// Other public methods...
		unsigned short countItems( unsigned short filter ) ; // How many of a type?
		unsigned short countItems( unsigned short fcat,
		                           unsigned short fkind1,
		                           unsigned short fkind2 ) ;

		Inventory * toInventory() ; // Converts pack contents to inventory.
		Inventory * toInventory( unsigned short filter ) ; // Filtered.
		Inventory * toInventory( unsigned short fcat,
		                         unsigned short fkind1,
		                         unsigned short fkind2 ) ; // Very filtered.

	private:

		// Fields...
		unsigned short maxitems ; // Current maximum pack size (vs. ucount)
		unsigned short t_count ; // Current total item count.

		PackNode *top ; // The empty top node of the ring (not a pointer).
		PackNode *current ; // The current item.
		PackNode *temp ; // A temporary pointer used for internal traversals.
		unsigned short traversalFilter ; // == (PackNode->item)->category

		// Private methods...

};

/*
 * The PackNode class replaces the packnode primitive type, and acts as a node
 * in the Pack's linked list of items.
 */
class PackNode
{
	public: 

		friend class Pack ;

		// Constructors...
		PackNode() ; // Default constructor.
		PackNode( Item *i, PackNode *p, PackNode *n ) ; // Explicit.

	private: 

		Item *item ;
		PackNode *prev, *next ;
};

/* ************************************************************************* */

#endif
