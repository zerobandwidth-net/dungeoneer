/*
 * feature.h
 * You may play with this source however you like, as long as you don't remove
 * this comment, claim undue credit, or make a profit on it.  Enjoy.
 * CHANGE HISTORY
 * 2012-01-08 (0.00)
 *  Created file.
 */

#ifndef _DUNGEONEER_H_
#include "dungeoneer.h"
#endif

#ifndef _FEATURE_H_
#define _FEATURE_H_

#include <string>
#include "rogue.h"
using namespace std ;

/* *** TWEAKS ************************************************************** */

// Dungeon feature type constants...
enum FeatureType
{
	BROKEN_FEATURE = 0,
	UPSTAIRS,
	DOWNSTAIRS,
	TRANSMUTATOR,                 // Transforms items into gold and vice versa.
	ANALYTICATOR,        // Analyzes items, but might break them or steal gold.
	FOUNTAIN,                               // Dispenses free doses of potions.
	RECOMBINATOR,        // Magically melts items together to share properties.
	HEALING_ALTAR                 // Heals wounds but might demand an offering.
} ;
#define FEATURE_TYPES 8

/* *** COMPLEX TYPE DEFINITIONS ********************************* (none) *** */

/* *** GLOBAL PROCEDURE PROTOTYPES ****************************** (none) *** */

/* *** CLASS PROTOTYPES (needed for friendship) ***************** (none) *** */

/* *** CLASS DEFINITIONS *************************************************** */

/*
 * A Feature is a feature of a dungeon level.  It is generally some ancient
 * piece of derelict arcana with which the player may choose to interact.
 */
class Feature
{
	public:
		// Static fields...

		// Static functions...
		static Feature * makeRandom( unsigned short depth ) ;

		// Constructors...
		virtual Feature( unsigned short depth ) ;

		// Accessors...
		virtual string getDesc() ;
		virtual string getDesc( bool capitalize ) ;
		virtual char getDisplayChar() ;
		virtual bool isPassable() ;

		// Mutators...
		virtual void init() ; // Initializes.

		// Other public functions...
		virtual void interact( Rogue* player ) ; // Interacts with character.

	protected:
		// Fields...
		char displayChar ; // On-screen representation of feature.
		bool isIdentified ; // Player has already interacted with this object.
		FeatureType type ; // Tracks the type of feature.

		unsigned short row, col ; // Location in the dungeon level.

		// Private functions...
};

/*
 * An Analyzer will accept an item from the player's inventory, and identify it.
 * At random, the item might be broken, or some amount of money might be stolen
 * from the player's pocket by a special "erzatz alchemist" creature.
 */
class Analyzer : public Feature
{
	public:
		// Static fields...

		// Static functions...

		// Constructors...
		Analyzer( unsigned short depth ) ;

		// Accessors...
		string getDesc() ;
		string getDesc( bool capitalize ) ;
		char getDisplayChar() ;
		bool isPassable() ;

		// Mutators...
		void init() ; // Initializes.

		// Other public functions...
		void interact( Rogue* player ) ; // Interacts with character.

	protected:
		// Fields...

		// Private functions...
};

/* ************************************************************************* */
#endif
