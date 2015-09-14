/*
 * dungeon.h
 * You may play with this source however you like, as long as you don't remove
 * this comment, claim undue credit, or make a profit on it.  Enjoy.
 * CHANGE HISTORY
 * 2011-11-29 (0.00)
 *  Created file.
 */

#ifndef _DUNGEONEER_H_
#include "dungeoneer.h"
#endif

#ifndef _TEMPLATE_H_
#define _TEMPLATE_H_

#include <string>
#include <vector>
#include "actor.h"
#include "ally.h"
//#include "creature.h"
#include "item.h"
#include "message.h"
#include "pack.h"
#include "party.h"
#include "party.cpp"
using namespace std ;

/* *** TWEAKS ************************************************************** */

// Dungeon types.  Used to determine which algorithm should be used for level
// creation.
enum DungeonType
{
	DT_OVERWORLD = 0,   // An island continent on which the player lives.
	DT_TOWN,            // A friendly place with shops and inns and such.
	DT_LABYRINTH,       // A classical Ally-style dungeon/maze.
	DT_RUINS,           // A dungeon where some rooms are adjacent.
	DT_FOREST,          // A forested valley.
	DT_CAVE,            // A natural cave system.
	DT_TEST = 255       // Magic value for a large "test arena" dungeon.
};
#define DUNGEON_TYPES 6

// Maximum dimensions of a dungeon level in screenspace.
#define MINDGCOL 0 // Minimum dungeon column.
#define MAXDGCOL DCOLS // Maximum dungeon column.
#define MINDGROW 2 // Minimum dungeon row.
#define MAXDGROW (DROWS-MAX_PARTY_SIZE) // Maximum dungeon row.

// Dungeon level sector types, for labyrinths, ruins, and caves.
enum DungeonSectorType
{
	SEC_EMPTY = 0,      // Empty; there is nothing here.
	SEC_ROOM,           // A room with some number of exits.
	SEC_MAZE,           // A room full of twisty passages.
	SEC_PASSAGE         // A sector with passages between other sectors.
};
#define SECTOR_TYPES 4

// Dungeon space types.
enum DungeonSpaceType
{
	DS_HYPERSPACE = 0,  // An impassable space outside the passable space.
	DS_WALL,            // A wall of a room or building.
	DS_DOOR,            // A door into a room or building.
	DS_FLOOR,           // The floor of a room, or a space in an open field.
	DS_PASSAGE          // A dark passageway in a dungeon.
};

// Dungeon level return codes.  These codes indicate what to do when a player
// has exited a dungeon level.
enum DungeonExitCode
{
	DX_DIED = -2,       // All characters have died.
	DX_QUIT,            // The player has elected to quit the game.
	DX_LEAVE,           // The characters have left the dungeon.
	DX_UPWARD,          // Proceed upward through the dungeon. (decrease depth)
	DX_DOWNWARD         // Proceed downward. (increase depth)
};

// Item creation randomization factors...
#define DG_ITEMS_MAX 5 // Maximum guaranteed number of items per dungeon level.
#define DG_ITEMS_MIN 3 // Minimum guaranteed number of items per dungeon level.
#define DG_ITEMS_PCT 33 // Chance that there might be even more items.
#define EXTRA_GOLD_CHANCE 50 // Chance of extra gold per room.
#define PARTY_ROOM_CHANCE 8 // Chance of a "party room" on a level with rooms.

/* *** COMPLEX TYPE DEFINITIONS ******************************************** */

typedef struct // DungeonSector
{
	DungeonSectorType type ; // == one of the SEC_* constants above.
	unsigned short row, col ; // ...of the top-left corner.
	unsigned short height, width ; // ...from top-left row,col
	short exitN, exitS, exitE, exitW ; // row or column
} DungeonSector ;

typedef struct // DungeonSpace
{
	DungeonSpaceType type ; // == one of the DS_* constants above.
	bool visible ; // The space is visible without searching for it.
	bool mapped ; // The space has been revealed to the player.
	bool lit ; // The space, if visible, should be lit.
	Pack *items ; // There is at least one item on the floor here.
//	Feature *feature ; // A dungeon feature stands here.
//	Trap *trap ; // There is a trap on the floor here.
} DungeonSpace ;

/* *** GLOBAL PROCEDURE PROTOTYPES ****************************** (none) *** */

/* *** CLASS PROTOTYPES (needed for friendship) ***************** (none) *** */

class Dungeon ;
class DungeonLevel ;
class DungeonRoom ;

/* *** CLASS DEFINITIONS *************************************************** */

class Dungeon
{
	public:
		// Static fields...
		// Static functions...
		// Constructors...
		Dungeon( DungeonType type, Party *players, string name ) ;

		// Accessors...
		string getName() ;

		// Mutators...
		// Other public functions...
		DungeonExitCode play() ;

	private:
		// Fields...
		vector<DungeonLevel> levels ;
		string name ; // The dungeon's name.
		Party *players ; // The player's data.
		DungeonType type ; // == one of the DT_* constants above.

		// Private functions...
};

class DungeonLevel
{
	public:
		friend class Dungeon ;

		// Static fields...
		// Static functions...

		// Constructors...
		DungeonLevel() ; // Default constructor.
		DungeonLevel( unsigned short depth, Party *players ) ;

		// Destructors...
//		~DungeonLevel() ;

		// Accessors...

		// Mutators...
		void init( DungeonType type, unsigned short depth ) ;
		void linkTo( Dungeon *d ) ;
		void setDName( string *s ) ;

		// Other public functions...
		DungeonExitCode play( bool backward ) ; // The main game environment.

	private:
		// Fields...
		unsigned short depth ; // The depth of this level within the dungeon.
		string *dname ; // Reference to the name of the dungeon.
		MessageEngine *msg ; // Message display engine.
		Party *players ; // The player's party of characters.
//		Party *foes ; // The hostile foes wandering the dungeon.
		vector<DungeonRoom*> rooms ; // If the level uses the rooms concept.
		DungeonSpace **spaces ; // Two-dimensional array of dungeon spaces.
		unsigned short nRow, nCol, xRow, xCol ; // Entry and exit points.

		// Private functions...
		bool bamfPlayer( Ally *a, unsigned short row, unsigned short col ) ;
		bool canAct( Ally *pc ) ; // Checks whether a character can act.
		bool catchDeadPC( Ally *pc ) ; // Handle character death.
		bool catchStateChange( Ally *pc, short sec ) ;
		void darkenSpace( unsigned short r, unsigned short c ) ;
		void darkenRoom( DungeonRoom *room ) ;
		void darkenPassage( unsigned short row, unsigned short col ) ;
		void drawSpace( unsigned short row, unsigned short col ) ;
		bool dropItem() ; // ...at current position
		DungeonRoom * getRoom( unsigned short row, unsigned short col ) ;
		void initItems( unsigned short depth ) ; // Randomly place items.
		void initOverworldLevel( unsigned short depth ) ;
		void initTown( unsigned short depth ) ;
		void initLabyrinthLevel( unsigned short depth ) ;
		void initRuinsLevel( unsigned short depth ) ;
		void initForestLevel( unsigned short depth ) ;
		void initCaveLevel( unsigned short depth ) ;
		void initTestLevel( unsigned short depth ) ;
		bool isPassable( unsigned short row, unsigned short col ) ;
		void lightSpace( unsigned short r, unsigned short c ) ;
		void lightPassage( unsigned short row, unsigned short col ) ;
		void lightRoom( DungeonRoom *room ) ;
		bool movePlayer( short v, short h, unsigned short count ) ;
		bool pickUpItems( Actor *a ) ; // ...at ally's current position
		void placeItem( Item *item ) ; // Places an item in the dungeon.
		void placeItem( Item *item, unsigned short row, unsigned short col ) ;
		void processScroll( Item *scroll ) ; // Scope issues put this here.
		void redraw() ; // Redraws main screen based on space data.
		bool runPlayer( Ally *a, short v, short h ) ;
		bool spideySense( unsigned short row, unsigned short col ) ;
		bool stepPlayer( Ally *a, short v, short h ) ;
		void updateSpaces( DungeonRoom *room ) ;
};

class DungeonRoom
{ 
    public:
		friend class DungeonLevel ;
        // Static fields...
        // Static functions...
        // Constructors...
		DungeonRoom( unsigned short trow, unsigned short brow,
		             unsigned short lcol, unsigned short rcol ) ;
        // Accessors...
		bool isInRoom( unsigned short row, unsigned short col ) ;

        // Mutators...
		void setExitN( short col ) ;
		void setExitS( short col ) ;
		void setExitE( short row ) ;
		void setExitW( short row ) ;

        // Other public functions...

    private:
        // Fields..
			unsigned short trow, brow, lcol, rcol ; // defines box
			short exitN, exitS, exitE, exitW ; // abs. row/col
			bool lit ; // Indicates whether room has already been lit.
        // Private functions...
};

/* ************************************************************************* */
#endif
