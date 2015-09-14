/*
 * ally.h
 * You may play with this source however you like, as long as you don't remove
 * this comment, claim undue credit, or make a profit on it.  Enjoy.
 * CHANGE HISTORY
 * 2011-11-17
 *  Created file.
 * 2012-01-25
 *  Split from rogue.h.
 */

#ifndef _ALLY_H_
#define _ALLY_H_

#ifndef _DUNGEONEER_H_
#include "dungeoneer.h"
#endif

#include "item.h"
#include "pack.h"

/* *** TWEAKS *************************************************** (none) *** */

/* *** COMPLEX TYPE DEFINITIONS ********************************* (none) *** */

/* *** GLOBAL PROCEDURE PROTOTYPES ****************************** (none) *** */

/* *** CLASS PROTOTYPES (needed for friendship) ***************** (none) *** */

/* *** CLASS DEFINITIONS *************************************************** */

class Ally : public Actor
{
	public:

		// Constructors...

		Ally() ; // Default: Sets everything to zero/blank.

		// Destructors...

		~Ally() ;

		// Accessors...

		// Mutators...
        void reroll() ; // Initialize a Rogue instance with statistics.

		// Other public functions...

        Item* displayInventory() ; // Displays an inventory window.
        Item* displayInventory( char filter ) ; // Filtered inventory.
        Item* displayInventory( string h ) ; // Display inv w/ custom header.
        Item* displayInventory( char filter, string h ) ;
        void displayStatus() ; // Displays a status screen.
        void levelUp() ; // Raise the rogue's actual exp. level.
        void manageBasics() ; // Allows user to set name, gender, initial stats.
        void manageBasics( unsigned short points ) ; // Levelup UI.
        unsigned short manageFood() ; // Eat a food item.
        bool manageEquipment() ; // Displays general equipment management UI.
        bool manageArmor() ; // Displays armor management UI.
        bool manageMeleeWeapons() ; // Displays melee weapon management UI.
        bool manageRangedWeapons() ; // Displays ranged weapon management UI.
        bool manageWeapons() ; // Displays weapon management UI.
        Item* managePotions() ; // Displays potion drinking UI.
        Item* manageScrolls() ; // Displays scroll reading UI.
        bool manageRings() ; // Displays ring management UI.
        bool manageNecklaces() ; // Displays necklace management UI.

	protected:

		// Functions...
		static unsigned short rerollStat() ;
};

/* ************************************************************************* */
#endif
