/*
 * inventory.h
 * You may play with this source however you like, as long as you don't remove
 * this comment, claim undue credit, or make a profit on it.  Enjoy.
 * CHANGE HISTORY
 * 2011-12-22 (0.00)
 *  Split from pack.h.
 */

#ifndef _DUNGEONEER_H_
#include "dungeoneer.h"
#endif

#ifndef _INVENTORY_H_
#define _INVENTORY_H_

#include <string>
#include "item.h"
#include "window.h"

/* *** TWEAKS *************************************************** (none) *** */

/* *** COMPLEX TYPE DEFINITIONS ******************************************** */ 

typedef struct
{
	char letter ;
	Item* item ;
} InventoryNode ;

/* *** GLOBAL PROCEDURE PROTOTYPES ****************************** (none) *** */

/* *** CLASS PROTOTYPES (needed for friendship) ***************** (none) *** */

/* *** CLASS DEFINITIONS *************************************************** */

/*
 * The Inventory class is a subset of the Pack class, providing inventory
 * display and item selection functions.
 */
class Inventory
{
	public:

        // Static fields...
        // Static functions...
        // Constructors...

		Inventory( unsigned short size ) ; // For items outside a pack.

		// Destructors...

		~Inventory() ;

        // Accessors...

		unsigned short getSize() ;

		Item* selectItem() ; // Selects an item from the window, if any.

        // Mutators...

		char setItem( unsigned short index, Item *item ) ; // Adds an item.

		Window* setWindow() ; // Generic display window.

		Window* setWindow(
				unsigned short iwy, unsigned short iwx,
				unsigned short iwh, unsigned short iww
			) ; // Explicitly-shaped window.

		void setWindowHeader( string h ) ; // Set window header.
		bool setWindowPage( unsigned short ipage ) ; // Move to page.

        // Other public functions...

		void prepare() ; // Prepares display.
		void refresh() ; // Refreshes display.

	private:

        // Fields...
		InventoryNode *items ; // Static array of items, instead of a list.
		unsigned short size ; // Size of the array.
		Window *iwin ; // Window for displaying this inventory.

        // Private functions...
		string describeNode( unsigned short i ) ; // Describes node at index.
		bool hasWindowPage( unsigned short p ) ; // Tests display pg existence.
};

/* ************************************************************************* */

#endif
