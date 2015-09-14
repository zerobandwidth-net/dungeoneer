/*
 * dungeon.cpp
 * You may play with this source however you like, as long as you don't remove
 * this comment, claim undue credit, or make a profit on it.  Enjoy.
 * CHANGE HISTORY
 * 2011-11-29 (0.00)
 *  Created file.
 */

#ifndef _DUNGEONEER_H_
#include "dungeoneer.h"
#endif

#include <string>
#include <ncurses.h>
#include <vector>
#include "actor.h"
#include "ally.h"
#include "command.h"
#include "dungeon.h"
#include "debug.h"
#include "item.h"
#include "message.h"
#include "pack.h"
#include "random.h"
#include "ui.h"
using namespace std ;

/* *** EXTERNAL REFERENCES ************************************************* */

extern Logger *log ; // debug.cpp
extern string statusEffectNames[STATUS_EFFECTS] ; // ui.cpp

/* *** LOCAL DATA *********************************************** (none) *** */

/* *** LOCAL PROCEDURE PROTOTYPES ****************************************** */

string describeDirection( short v, short h ) ; // String indicates direction.
short signOf( short x ) ; // Reduces number to -1, 0, or 1.

/* *** PROCEDURE DEFINITIONS *********************************************** */

/*
 * Given a vertical velocity v and horizontal velocity h, returns a string
 * indicating the movement's direction.
 */
string describeDirection( short v, short h )
{
	if( v < 0 )
	{ // northward
		if( h < 0 ) return "northwest" ;
		else if( h > 0 ) return "northeast" ;
		else return "north" ;
	}
	else if( v > 0 )
	{ // southward
		if( h < 0 ) return "southwest" ;
		else if( h > 0 ) return "southeast" ;
		else return "south" ;
	}
	else // horizontal
	{
		if( h < 0 ) return "west" ;
		else if( h > 0 ) return "east" ;
		else return "nowhere" ; // Shouldn't happen.
	}
}

/*
 * Reduces a number to -1, 0, or 1, based on its sign.
 */
short signOf( short x )
{
	if( x < 0 ) return -1 ;
	else if( x > 0 ) return 1 ;
	else return 0 ;
}

/* *** CLASS DEFINITION (Dungeon) ****************************************** */

// /// STATIC FUNCTIONS (Dungeon) /////////////////////////////// (none) /// //

// /// CONSTRUCTORS (Dungeon) ////////////////////////////////////////////// //

/*
 * Creates a dungeon and drops the player into it.
 */
Dungeon::Dungeon( DungeonType type, Party *players, string name )
{
	this->name = name ;
	this->type = type ;
	this->players = players ;
}

// /// ACCESSORS (Dungeon) ///////////////////////////////////////////////// //

/*
 * Returns the dungeon's name.
 */
string Dungeon::getName(void) { return this->name ; }

// /// MUTATORS (Dungeon) ////////////////////////////////////////////////// //

// /// OTHER PUBLIC FUNCTIONS (Dungeon) //////////////////////////////////// //

/*
 * Plays through the dungeon.  Returns an exit code from the DX_* constants.
 */
DungeonExitCode Dungeon::play(void)
{
	string func = "Dungeon::play()" ; // debug
	log->lbuf << "Playing through dungeon: " << this->name ;
	log->writeBuffer( Logger::INFO, __FILE__, __LINE__, func ) ;

	DungeonExitCode exitCode ;
	bool direction = false ;
	unsigned short depth = 1 ;

	for(;;) // infinity broken within
	{
		if( direction == false )
		{ // Moving downward; create next level.
			log->lbuf << "Moving downward in " << this->name
			          << " to level " << depth
			          ;
			log->writeBuffer( Logger::INFO, __FILE__, __LINE__, func ) ;
			DungeonLevel l( depth, this->players ) ;
			(this->levels).push_back(l) ;
			l.init( this->type, depth ) ;
			l.setDName( &(this->name) ) ;
			exitCode = l.play(false) ;
		}
		else // Moving upward; reenter previous level.
		{
			log->lbuf << "Moving upward in " << this->name
			          << " to level " << depth
			          ;
			log->writeBuffer( Logger::INFO, __FILE__, __LINE__, func ) ;
			exitCode = levels[depth].play(true) ;
		}

		log->lbuf << "Caught returned exit code: " << exitCode ;
		log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;

		switch(exitCode)
		{
			case DX_DIED:
				log->write( Logger::INFO, __FILE__, __LINE__, func,
				            "Passing \"died\" code upward..." ) ;
				return DX_DIED ;                               // EXIT CONDITION
			case DX_QUIT:
				log->write( Logger::INFO, __FILE__, __LINE__, func,
				            "Passing \"quit\" code upward..." ) ;
				return DX_QUIT ;                               // EXIT CONDITION
			case DX_LEAVE:
				log->write( Logger::INFO, __FILE__, __LINE__, func,
				            "Passing \"leave\" code upward..." ) ;
				return DX_LEAVE ;                              // EXIT CONDITION
			case DX_UPWARD:
				if( depth == 1 ) // We just left the dungeon.
				{
					log->write( Logger::INFO, __FILE__, __LINE__, func,
					            "Moved upward from first level.  Leaving..." ) ;
					return DX_LEAVE ;                          // EXIT CONDITION
				}
				else
				{
					log->write( Logger::TRACE, __FILE__, __LINE__, func,
					            "Received \"upward\" code." ) ;
					--depth ;
					direction = true ; // upward
				}
				break ;
			case DX_DOWNWARD:
				log->write( Logger::TRACE, __FILE__, __LINE__, func,
				            "Received \"downward\" code." ) ;
				++depth ;
				direction = false ; // downward
				break ;
			default:
				log->lbuf << "Invalid exit code returned: " << exitCode ;
				log->writeBuffer( Logger::WARNING, __FILE__, __LINE__, func ) ;
		}
	}
}

// /// PRIVATE FUNCTIONS (Dungeon) ////////////////////////////// (none) /// //

/* *** CLASS DEFINITION (DungeonLevel) ************************************* */

// /// STATIC FUNCTIONS (DungeonLevel) ////////////////////////// (none) /// //

// /// CONSTRUCTORS (DungeonLevel) ///////////////////////////////////////// //

/*
 * Default constructor.
 */
DungeonLevel::DungeonLevel(void)
{
	string func = "DungeonLevel()" ; // debug
	log->write( Logger::INFO, __FILE__, __LINE__, func,
	            "Constructing default dungeon object..." ) ;

	this->depth = 0 ;
	this->players = NULL ;
}

/*
 * Constructor with a depth specified.
 */
DungeonLevel::DungeonLevel( unsigned short depth, Party *players )
{
	string func = "DungeonLevel(ushort,Ally*)" ; // debug
	log->lbuf << "Constructing level at depth " << depth ;
	log->writeBuffer( Logger::INFO, __FILE__, __LINE__, func ) ;

	this->depth = depth ;
	this->players = players ;
}

// /// DESTRUCTORS (DungeonLevel) ////////////////////////////////////////// //
/*
DungeonLevel::~DungeonLevel()
{
	for( unsigned short i = 0 ; i < DROWS ; i++ )
		delete[] this->spaces[i] ;
	delete[] this->spaces ;
}
*/
// /// ACCESSORS (DungeonLevel) //////////////////////////////////////////// //

// /// MUTATORS (DungeonLevel) ///////////////////////////////////////////// //

/*
 * Constructs a level of a dungeon, according to the specified dungeon type
 * and indicated depth within that dungeon.
 */
void DungeonLevel::init( DungeonType type, unsigned short depth )
{
	string func = "DungeonLevel::init(DungeonType,ushort)" ; // debug
	log->lbuf << "Initializing level of type " << type
	          << " at depth " << depth
	          ;
	log->writeBuffer( Logger::INFO, __FILE__, __LINE__, func ) ;
	
	this->depth = depth ;
	this->msg = new MessageEngine(2) ;

	// By default, set all spaces to empty, hidden hyperspace.
	log->lbuf << "Creating two-dimensional array of spaces: "
	          << DROWS << " by " << DCOLS
	          ;
	log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;

	this->spaces = new DungeonSpace*[DROWS] ;
	for( unsigned short i = 0 ; i < DROWS ; i++ )
		spaces[i] = new DungeonSpace[DCOLS] ;

	for( unsigned short c = 0 ; c < DCOLS ; c++ )
	for( unsigned short r = 0 ; r < DROWS ; r++ )
	{
		this->spaces[r][c] = (DungeonSpace)
			{ DS_HYPERSPACE, true, false, false, NULL
			  /*, NULL, NULL, NULL*/
			} ;
	}

	// Eventually, there would be a switch(type) here, but for now we'll start
	// with the classic "ruins" type from the original Ally.
//	switch( type )
//	{
//		case DT_OVERWORLD: this->initOverworldLevel(depth) ; break ;
//		case DT_TOWN:      this->initTown(depth) ; break ;
//		case DT_LABYRINTH: this->initLabyrinthLevel(depth) ; break ;
//		case DT_RUINS:     this->initRuinsLevel(depth) ; break ;
//		case DT_FOREST:    this->initForestLevel(depth) ; break ;
//		case DT_CAVE:      this->initCaveLevel(depth) ; break ;
/*		default: */ this->initTestLevel(depth) ;
//	}

	this->initItems(depth) ;

	return ;
}

/*
 * Sets reference to the name of the dungeon of which this is a level.
 */
void DungeonLevel::setDName( string *n )
{
	this->dname = n ;
	return ;
}

// /// OTHER PUBLIC FUNCTIONS (DungeonLevel) /////////////////////////////// //

/*
 * The main command loop for the game environment.  This is where all the
 * magic happens.  The level will ultimately return an exit code, indicating
 * what to do next.
 */
DungeonExitCode DungeonLevel::play( bool backward )
{
	string func = "DungeonLevel::play()" ; // debug
	log->write( Logger::INFO, __FILE__, __LINE__, func,
	            "Playing level..." ) ;

	if( this->players == NULL ) return DX_QUIT ;

	if( backward )
	{
		log->write( Logger::TRACE, __FILE__, __LINE__, func,
		            "Playing through backward." ) ;
		players->getLeader()->setLocation( this->xRow, this->xCol ) ;
	}
	else
	{
		log->write( Logger::TRACE, __FILE__, __LINE__, func,
		            "Playing through forward." ) ;
		player->getLeader()->setLocation( this->nRow, this->nCol ) ;
	}

	this->lightCurrentSpace() ;

	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Entering command loop..." ) ;
	for(;;) // infinity broken within
	{
		log->write( Logger::TRACE, __FILE__, __LINE__, func,
		            "Next iteration." ) ;

		msg->drawStatus( this->players ) ;

		msg->draw() ; refresh() ;

		// Player now decides what action the party's leader will take.

		Command *cmd = new Command(true) ;
		unsigned short cc = cmd->getCode() ;

		msg->flush() ;
		msg->clear() ;
		refresh() ;

		Ally *ldr = players->getLeader() ; // for the sake of convenience.
		bool ticked = false ; // Indicates whether player command ticks time.

		switch(cc)
		{
			case K_QUIT: // 17 / Ctrl+Q
				delete cmd ;
				log->write( Logger::TRACE, __FILE__, __LINE__, func,
				            "Returing \"quit\" code." ) ;
				return DX_QUIT ;                               // EXIT CONDITION
			case K_REDRAW: // 18 / Ctrl+R
				this->redraw() ;
				refresh() ;
				break ;
			case K_WEAPON: // 41 / ')'
			case K_WIELD_WEAPON: // 118 / 'w'
				if( canAct(ldr) ) ticked = ldr->manageWeapons() ;
				break ;
			case K_PICK_UP: // 59 / ';'
				if( canAct(ldr) ) ticked = this->pickUpItems(ldr) ;
				break ;
			case K_RING: // 61 / '='
				if( canAct(ldr) ) ticked = ldr->manageRings() ;
				break ;
			case K_RUN_SOUTHWEST: // 66 / 'B'
				if( canAct(ldr) ) runPlayer( ldr, 1, -1 ) ; // FLARGLE
				else ticked = true ;
				break ;
			case K_EQUIPMENT: // 69 / 'E'
				if( canAct(player) ) player->manageEquipment() ;
				break ;
			case K_RUN_WEST: // 72 / 'H'
				if( canAct(player) ) runPlayer( 0, -1 ) ;
				else catchStateChange( player, player->tick() ) ;
				ticked = true ;
				break ;
			case K_RUN_SOUTH: // 74 / 'J'
				if( canAct(player) ) runPlayer( 1, 0 ) ;
				else catchStateChange( player, player->tick() ) ;
				ticked = true ;
				break ;
			case K_RUN_NORTH: // 75 / 'K'
				if( canAct(player) ) runPlayer( -1, 0 ) ;
				else catchStateChange( player, player->tick() ) ;
				ticked = true ;
				break ;
			case K_RUN_EAST: // 76 / 'L'
				if( canAct(player) ) runPlayer( 0, 1 ) ;
				else catchStateChange( player, player->tick() ) ;
				ticked = true ;
				break ;
			case K_RUN_SOUTHEAST: // 78 / 'N'
				if( canAct(player) ) runPlayer( 1, 1 ) ;
				else catchStateChange( player, player->tick() ) ;
				ticked = true ;
				break ;
			case K_STATUS: // 83 / 'S'
				player->displayStatus() ;
				break ;
			case K_RUN_NORTHEAST: // 85 / 'U'
				if( canAct(player) ) runPlayer( -1, 1 ) ;
				else catchStateChange( player, player->tick() ) ;
				ticked = true ;
				break ;
			case K_WEAR_ARMOR: // 87 / 'W'
			case K_ARMOR: // 93 / ']'
				player->manageArmor() ;
				break ;
			case K_RUN_NORTHWEST: // 89 / 'Y'
				if( canAct(player) ) runPlayer( -1, -1 ) ;
				else catchStateChange( player, player->tick() ) ;
				ticked = true ;
				break ;
			case K_SOUTHWEST_VI: // 98 / 'b'
			case K_SOUTHWEST: // 274 / magic
				if( canAct(player) ) movePlayer( 1, -1, cmd->getCount() ) ;
				else catchStateChange( player, player->tick() ) ;
				ticked = true ;
				break ;
			case K_DROP_ITEM: // 100 / 'd'
				if( canAct(player) )
				{
					if( this->dropItem() )
					{
						player->tick() ;
						ticked = true ;
					}
				}
				else
				{
					catchStateChange( player, player->tick() ) ;
					ticked = true ;
				}
				break ;
			case K_EAT_FOOD: // 101 / 'e'
				if( canAct(player) ) 
				{
					if( player->getHungerState() > 0 )
					{
						if( player->manageFood() > 0 )
						{
							if( player->getHunger() >= STARTING_HUNGER )
							{
								msg->buf << player->getName()
									<< " feels stuffed."
									;
							}
							else
							{
								msg->buf << player->getName()
								         << " is a bit less hungry." ;
							}
						
							msg->pushBuffer() ;
						}
					}
					else
					{
						msg->buf << player->getName() << " isn't hungry." ;
						msg->pushBuffer() ;
					}
				}
				else catchStateChange( player, player->tick() ) ;
				player->tick() ;
				ticked = true ;
				break ;
			case K_WEST_VI: // 104 / 'h'
			case K_WEST: // 283 / magic
				if( canAct(player) ) movePlayer( 0, -1, cmd->getCount() ) ;
				else catchStateChange( player, player->tick() ) ;
				ticked = true ;
				break ;
			case K_INVENTORY: // 105 / 'i'
				player->displayInventory() ;
				break ;
			case K_SOUTH_VI: // 106 / 'j'
			case K_SOUTH: // 281 / magic
				if( canAct(player) ) movePlayer( 1, 0, cmd->getCount() ) ;
				else catchStateChange( player, player->tick() ) ;
				ticked = true ;
				break ;
			case K_NORTH_VI: // 107 / 'k'
			case K_NORTH: // 280 / magic
				if( canAct(player) ) movePlayer( -1, 0, cmd->getCount() ) ;
				else catchStateChange( player, player->tick() ) ;
				ticked = true ;
				break ;
			case K_EAST_VI: // 108 / 'l'
			case K_EAST: // 282 / magic
				if( canAct(player) ) movePlayer( 0, 1, cmd->getCount() ) ;
				else catchStateChange( player, player->tick() ) ;
				ticked = true ;
				break ;
			case K_SOUTHEAST_VI: // 110 / 'n'
			case K_SOUTHEAST: // 276 / magic
				if( canAct(player) ) movePlayer( 1, 1, cmd->getCount() ) ;
				else catchStateChange( player, player->tick() ) ;
				ticked = true ;
				break ;
			case K_QUAFF_POTION: // 119 / 'q'
			{
				if( canAct(player) ) 
				{
					Item *potion = player->managePotions() ;
					if( potion != NULL )
					{
						unsigned short k = potion->getKind() ;
						if( player->quaff(potion) )
						{
							msg->buf << player->getName()
							         << potion->getUseMessage()
							         ;
							switch(k)
							{
								case P_SEE_MONS:
								case P_SEE_STUFF:
								case P_TRUESIGHT:
									this->redraw() ;
									break ;
								case P_BLIND:
									darkenCurrentSpace() ;
									this->redraw() ;
									break ;
								default: ;
							}
						}
						else
						{
							msg->buf << player->getName()
								<< " shrugs off the potion's effect."
								;
						}
						msg->pushBuffer() ;
						player->tick() ;
					}
				}
				else catchStateChange( player, player->tick() ) ;
				ticked = true ;
			}	break ;
			case K_READ_SCROLL: // 120 / 'r'
				if( canAct(player) )
					this->processScroll( player->manageScrolls() ) ;
				else catchStateChange( player, player->tick() ) ;
				ticked = true ;
				break ;
			case K_NORTHEAST_VI: // 117 / 'u'
			case K_NORTHEAST: // 275 / magic
				if( canAct(player) ) movePlayer( -1, 1, cmd->getCount() ) ;
				else catchStateChange( player, player->tick() ) ;
				ticked = true ;
				break ;
			case K_NORTHWEST_VI: // 121 / 'y'
			case K_NORTHWEST: // 271 / magic
				if( canAct(player) ) movePlayer( -1, -1, cmd->getCount() ) ;
				else catchStateChange( player, player->tick() ) ;
				ticked = true ;
				break ;
			default: ;
		}

		delete cmd ;

		// If time has passed, then perform various checks.
		if( ticked )
		{
			if( player->getHP() <= 0 )
				if( catchDeadPC( player ) )
					 return DX_DIED ;
		}
	}
}

// /// PRIVATE FUNCTIONS (DungeonLevel) //////////////////////////////////// //

/*
 * Tries to teleport the player to the specified location.  Returns true if the
 * move is successful.
 */
bool DungeonLevel::bamfPlayer( Ally *a, unsigned short row, unsigned short col )
{
	if( !isPassable( row, col ) ) return false ;

	unsigned short oldr = a->getRow() ;
	unsigned short oldc = a->getCol() ;

	a->setLocation( row, col ) ;

	darkenSpace( oldr, oldc ) ;
	lightSpace( row, col ) ;

	drawSpace( oldr, oldc ) ;
	drawSpace( row, col ) ;

	return true ;
}

/*
 * Checks whether a character can act this turn.
 */
bool DungeonLevel::canAct( Ally *pc )
{
	string func = "DungeonLevel::canAct(Ally*)" ; // debug

	if( pc->canAct() ) return true ;

	// Otherwise, display a message informing the player why this character
	// can't act this turn.
	if( pc->getStatus(ASLEEP) > 0 )
	{
		log->lbuf << pc->getName() << " is asleep." ;
		log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
		msg->buf << pc->getName() << " is asleep." ;
		msg->pushBuffer() ;
	}
	else if( pc->getStatus(PARALYZED) > 0 )
	{
		log->lbuf << pc->getName() << " is paralyzed." ;
		log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
		msg->buf << pc->getName() << " is paralyzed." ;
		msg->pushBuffer() ;
	}
	else if( pc->getStatus(STONED) > 0 )
	{
		log->lbuf << pc->getName() << " is petrified." ;
		log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
		msg->buf << pc->getName() << " is petrified." ;
		msg->pushBuffer() ;
	}

	return false ;
}

/*
 * Catches and handles a character death event.  Return value indicates whether
 * all members of the party are dead.
 */
bool DungeonLevel::catchDeadPC( Ally *pc )
{
	// For now, while there is still only one character supported, that
	// character's death is enough to signal "true".
	msg->buf << pc->getName() << " has died." ;
	msg->pushBuffer() ;
	msg->draw() ; refresh() ;

	(void)waitForInput() ;

	return true ;
}

/*
 * Catches and handles messaging for a player character state change after a
 * call to Ally::tick().  The short parameter is the return value from
 * Ally::tick().
 */
bool DungeonLevel::catchStateChange( Ally *pc, short sec )
{
	string func = "DungeonLevel::catchStateChange(Ally*,short)" ; // debug

	log->lbuf << "Evaluating state change " << sec
		<< " for " << pc->getName() << "."
		;
	log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;

	bool interrupted = false ;

	// Display hunger message if necessary.
	switch( pc->getHunger() )
	{
		case Actor::HUNGER:
			msg->buf << pc->getName() << " is starting to feel hungry." ;
			msg->pushBuffer() ;
			interrupted = true ;
			break ;
		case Actor::WEAKNESS:
			msg->buf << pc->getName() << " is growing weak from hunger." ;
			msg->pushBuffer() ;
			interrupted = true ;
			break ;
		case Actor::FAINTING:
			msg->buf << pc->getName() << " feels faint from lack of food." ; 
			msg->pushBuffer() ;
			interrupted = true ;
			break ;
		case Actor::STARVED:
			msg->buf << pc->getName() << " has starved to death." ;
			msg->pushBuffer() ;
			msg->draw() ;
			return true ;
		default: ;
	}

	if( interrupted ) // by a hunger status notice...
	{
		log->write( Logger::TRACE, __FILE__, __LINE__, func,
			"Pushed hunger status message." ) ;
	}

	// Display "status cured" message if necessary.
	if( sec >= 0 )
	{
		interrupted = true ;

		StatusEffect s = (StatusEffect)(sec) ;

		msg->buf << pc->getName() << " is no longer "
			<< statusEffectNames[sec] << "."
			;
		log->write( Logger::TRACE, __FILE__, __LINE__, func, msg->buf.str() ) ;
		msg->pushBuffer() ;

		if( sec == BLINDED ) lightCurrentSpace() ;
	}

	log->write( Logger::TRACE, __FILE__, __LINE__, func,
		( interrupted ? "Interrupting operation." : "Evaluation complete." ) ) ;

	return interrupted ;
}

/*
 * Darkens the space around the specified location.  If the location is inside a
 * room, and any players are standing within the room, the action is aborted.
 * If the location is inside a passage, and any players are still standing
 * within sight of the location, the action is aborted.
 */
void DungeonLevel::darkenSpace( unsigned short r, unsigned short c )
{
	string func = "DungeonLevel::darkenSpace(ushort,ushort)" ; // debug

	log->lbuf << "Attempting to darken space at (" << r << "," << c << ")." ;
	log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;

	switch( spaces[r][c].type )
	{
		case DS_HYPERSPACE:
			log->lbuf << "Tried to darken hyperspace at ("
			          << r << "," << c << ")."
			          ;
			log->writeBuffer( Logger::WARNING, __FILE__, __LINE__, func ) ;
			break ;
		case DS_WALL:
			log->lbuf << "Space at (" << r << "," << c << ") is a wall." ;
			log->writeBuffer( Logger::WARNING, __FILE__, __LINE__, func ) ;
			break ;
		case DS_DOOR:
			log->lbuf << "Darkening passage from door at ("
			          << r << "," << c << ")."
			          ;
			log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
			darkenPassage(r,c) ;
			// no break; leak into DS_FLOOR case block.
		case DS_FLOOR:
			log->lbuf << "Darkening room around ("
			          << r << "," << c << ")."
			          ;
			log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
			darkenRoom( getRoom(r,c) ) ;
			break ;
		case DS_PASSAGE:
			log->lbuf << "Darkening passages around ("
			          << r << "," << c << ")."
			          ;
			log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
			darkenPassage(r,c) ;
			break ;
		default:
			log->lbuf << "Tried to light invalid space type: "
			          << spaces[r][c].type
			          ;
			log->writeBuffer( Logger::WARNING, __FILE__, __LINE__, func ) ;
	}

	return ;
}

/*
 * Darkens a room.  This might be done because the player has left it, or
 * because the player has become blinded.  Note that this function does LESS
 * screen updating than lightRoom(), because, after a room has been lit, we
 * still keep the walls and doors visible onscreen.
 */
void DungeonLevel::darkenRoom( DungeonRoom *room )
{
	string func = "DungeonLevel::darkenRoom(DungeonRoom*)" ; // debug

	if( room == NULL )
	{
		log->write( Logger::ERROR, __FILE__, __LINE__, func,
		            "Tried to darken nonexistent room." ) ;
		return ;
	}

	if( !room->lit )
	{
		log->write( Logger::TRACE, __FILE__, __LINE__, func,
		            "Room was already dark." ) ;
		return ;
	}

	for( unsigned short i = 0 ; i < players->size() ; i++ )
	{
		if( room->isInRoom( players[i]->getRow(), players[i]->getCol() )
		 && players[i]->getStatus(BLINDED) <= 0 )
		{ // Someone with sight is still inside; don't darken the room.
			log->write( Logger::TRACE, __FILE__, __LINE__, func,
				"Sighted player still inside room." ) ;
			return ;
		}
	}

	log->silence() ;
	for( unsigned short r = room->trow ; r <= room->brow ; r++ )
	for( unsigned short c = room->lcol ; c <= room->rcol ; c++ )
	{
		spaces[r][c].lit = false ;
		drawSpace( r, c ) ;
	}
	log->wake() ;

	room->lit = false ;

	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Successfully darkened room." ) ;

	return ;
}

/*
 * Darkens a passage.
 */
void DungeonLevel::darkenPassage( unsigned short row, unsigned short col )
{
	string func = "DungeonLevel::darkenPassage(ushort,ushort)" ;

	// Verify that no sighted players are standing in the passage.
	

	log->lbuf << "Darkening passage from (" << row << "," << col << ")." ;
	log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;

	spaces[row][col].lit = false ;

	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Darkening passage northward." ) ;
	for( unsigned short r = row ; r > MINDGROW ; r-- )
	{ // Darken passage to the north.
		if( !(spaces[r-1][col].lit) )
		{
			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Passage is already dark." ) ;
			break ;
		}
		else if( spaces[r-1][col].type == DS_PASSAGE
		      || spaces[r-1][col].type == DS_DOOR )
		{
			spaces[r-1][col].lit = false ;
			log->silence() ;
			drawSpace( r-1, col ) ;
			log->wake() ;
		}
		else
		{
			log->lbuf << "Stopped after " << row-r
			          << ( (row-r) == 1 ? " space." : " spaces." )
			          ;
			log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
			break ;
		}
	}

	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Darkening passage southward." ) ;
	for( unsigned short r = row ; r < MAXDGROW ; r++ )
	{ // Darken passage to the south.
		if( ! (spaces[r+1][col].lit) )
		{
			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Passage is already dark." ) ;
			break ;
		}
		else if( spaces[r+1][col].type == DS_PASSAGE
		      || spaces[r+1][col].type == DS_DOOR )
		{
			spaces[r+1][col].lit = false ;
			log->silence() ;
			drawSpace( r+1, col ) ;
			log->wake() ;
		}
		else 
        {
            log->lbuf << "Stopped after " << r-row
                      << ( (r-row) == 1 ? " space." : " spaces." )
                      ;
			log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
            break ;
        }
	}

	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Darkening passage westward." ) ;
	for( unsigned short c = col ; c > MINDGCOL ; c-- )
	{ // Darken passage to the west.
		if( !(spaces[row][c-1].lit) )
		{
			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Passage is already dark." ) ;
			break ;
		}
		else if( spaces[row][c-1].type == DS_PASSAGE
		      || spaces[row][c-1].type == DS_DOOR )
		{
			spaces[row][c-1].lit = false ;
			log->silence() ;
			drawSpace( row, c-1 ) ;
			log->wake() ;
		}
		else
        {
            log->lbuf << "Stopped after " << col-c
                      << ( (col-c) == 1 ? " space." : " spaces." )
                      ;
			log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
            break ;
        }
	}

	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Darkening passage eastward." ) ;
	for( unsigned short c = col ; c < MAXDGCOL ; c++ )
	{ // Darken passage to the east.
		if( !(spaces[row][c+1].lit) )
		{
			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Passage is already dark." ) ;
			break ;
		}
		else if( spaces[row][c+1].type == DS_PASSAGE
		      || spaces[row][c+1].type == DS_DOOR )
		{
			spaces[row][c+1].lit = false ;
			log->silence() ;
			drawSpace( row, c+1 ) ;
			log->wake() ;
		}
		else
        {
            log->lbuf << "Stopped after " << c-col
                      << ( (c-col) == 1 ? " space." : " spaces." )
                      ;
			log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
            break ;
        }
	}

	return ;
}
/*
 * Redraws a single space of the dungeon level, based on what's there.
 */
void DungeonLevel::drawSpace( unsigned short row, unsigned short col )
{
	string func = "DungeonLevel::drawSpace(ushort,ushort)" ; // debug

	if( !spaces[row][col].visible )
	{
		log->lbuf << "(" << row << "," << col << ") is invisible." ;
		log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
		mvaddch( row, col, ' ' ) ;
		return ;
	}

	if( !spaces[row][col].mapped )
	{
		log->lbuf << "(" << row << "," << col << ") is not mapped." ;
		log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;

		// Insert handling for "revealed" creatures in unmapped spaces.

		// Handle "revealed" stuff in unmapped spaces.
		if( spaces[row][col].items != NULL
		 && (spaces[row][col].items)->getCount() > 0
		 && (players->getLeader())->getStatus(SEES_STUFF) > 0 
		  )
		{ // Display the first item in the pack of items here.
			Item *item = spaces[row][col].items[0] ;
			if( item != NULL )
			{ // Draw the item.
				log->lbuf << "Item from pile ("
					<< item->getDisplayChar() << ")"
					;
				mvaddch( row, col, item->getDisplayChar() ) ;
			}
		}
		else mvaddch( row, col, ' ' ) ;
		return ;
	}

	// All spaces that are mapped should be drawn, whether they are lit or not.

	log->lbuf << "Drawing at (" << row << "," << col << "): " ;

	// Objects take precedence over geography, so test and draw things in
	// descending order of priority.
	if( players->getMemberAt(row,col) != NULL )
	{ // Write the display character for a player.
		(players->getMemberAt(row,col))->draw() ;
	}
//	else if( foes->getMemberAt(row,col) != NULL
//	      && ( spaces[row][col].lit && player->getStatus(SEES_CREATURES) > 0 )
//         )
//	{ // Write the display character for that creature.
//		Foe *f = foes->getMemberAt(row,col) ;
//		log->lbuf << "Creature (" << f->getDisplayChar() << ")" ;
//		f->draw() ;
//	}
//	else if( spaces[row][col].feature != NULL && spaces[row][col].lit )
//	{ // Write the display character for a dungeon feature.
//		log->lbuf << "Feature ("
//		          << (spaces[row][col].feature)->getDisplayChar() << ")"
//		          ;
//		mvaddch( row, col, (spaces[row][col].feature)->getDisplayChar() ) ;
//	}
	else if( spaces[row][col].items != NULL
	      && (spaces[row][col].items)->getCount() > 0
	      && ( spaces[row][col].lit
	        || (players->getLeader())->getStatus(SEES_STUFF) > 0 ) 
	       )
	{ // Display the first item in the pack of items here.
		Item *item = (spaces[row][col].items)->getItem(0) ;
		if( item != NULL )
		{ // Draw the item.
			log->lbuf << "Item from pile (" << item->getDisplayChar() << ")" ;
			mvaddch( row, col, item->getDisplayChar() ) ;
		}
	}
//	else if( spaces[row][col].trap != NULL
//	      && (spaces[row][col].trap)->isVisible() )
//	{ // Write display character for trap.
//		log->lbuf << "Trap (" << V_TRAP << ")" ;
//		mvaddch( row, col, V_TRAP ) ;
//	}
	else if( spaces[row][col].type == DS_HYPERSPACE )
	{ // Blank out the space.
		log->lbuf << "Hyperspace." ;
		mvaddch( row, col, ' ' ) ;
	}
	else if( spaces[row][col].type == DS_WALL )
	{ // Draw the appropriate wall character.
		log->lbuf << "Wall." ;
		DungeonRoom *room = getRoom(row,col) ;
		if( row == room->trow )
		{
			if( col == room->lcol ) mvaddch( row, col, V_ULCORNER ) ;
			else if( col == room->rcol ) mvaddch( row, col, V_URCORNER ) ;
			else mvaddch( row, col, V_HWALL ) ;
		}
		else if( row == room->brow )
		{
			if( col == room->lcol ) mvaddch( row, col, V_LLCORNER ) ;
			else if( col == room->rcol ) mvaddch( row, col, V_LRCORNER ) ;
			else mvaddch( row, col, V_HWALL ) ;
		}
		else mvaddch( row, col, V_VWALL ) ;

		if( spaces[row][col].lit )
			mvchgat( row, col, 1, (A_BOLD|A_ALTCHARSET), 7, NULL ) ;
		else
			mvchgat( row, col, 1, (A_DIM|A_ALTCHARSET), 4, NULL ) ;
	}
	else if( spaces[row][col].type == DS_DOOR )
	{ // Draw a door.
		log->lbuf << "Door." ;
		mvaddch( row, col, V_DOOR ) ;
		if( spaces[row][col].lit )
			mvchgat( row, col, 1, (A_DIM|A_ALTCHARSET), 3, NULL ) ;
		else
			mvchgat( row, col, 1, (A_DIM|A_ALTCHARSET), 4, NULL ) ;
	}
	else if( spaces[row][col].type == DS_FLOOR )
	{ // Draw a floor space.
		if( spaces[row][col].lit )
		{
			log->lbuf << "Floor of a lit room." ;
			mvaddch( row, col, V_FLOOR ) ;
			mvchgat( row, col, 1, (A_DIM|A_ALTCHARSET), 7, NULL ) ;
		}
		else
		{
			log->lbuf << "Floor of an unlit room." ;
			mvaddch( row, col, ' ' ) ;
		}
	}
	else if( spaces[row][col].type == DS_PASSAGE )
	{
		log->lbuf << "Passage." ;
		mvaddch( row, col, V_PASSAGE ) ;
		if( spaces[row][col].lit )
			mvchgat( row, col, 1, (A_DIM|A_ALTCHARSET), 7, NULL ) ;
		else
			mvchgat( row, col, 1, (A_DIM|A_ALTCHARSET), 4, NULL ) ;
	}
	// No "else" clause; do nothing by default.
	
	log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;

	return ;
}

/*
 * Allows player to drop an item at the current position.
 */
bool DungeonLevel::dropItem(void)
{
	string func = "DungeonLevel::dropItem()" ; // debug

	Item *item = player->displayInventory( K_ALLITEMS, "Drop which item?" ) ;
	if( item != NULL )
	{
		this->placeItem( item, player->getRow(), player->getCol() ) ;
		player->removeItemFromPack( item ) ;
		msg->buf << "Dropped " << item->getDesc() << "." ;
		msg->pushBuffer() ;
		msg->draw() ;
		return true ;
	}
	return false ;
}

/*
 * Returns a pointer to the room that contains the current location.
 */
DungeonRoom * DungeonLevel::getRoom( unsigned short row, unsigned short col )
{
	for( unsigned short i = 0 ; i < rooms.size() ; i++ )
		if( rooms[i]->isInRoom(row,col) ) return rooms[i] ;

	return NULL ;
}

/*
 * Randomly scatters items throughout a new level.  The algorithm may be
 * tweaked in the header file, but is basically lifted from rogue.
 */
void DungeonLevel::initItems( unsigned short depth )
{
	string func = "DungeonLevel::initItems(ushort)" ; // debug
	log->write( Logger::INFO, __FILE__, __LINE__, func,
	            "Placing random items..." ) ;

	Item *item ;

	// Always ensure that the level contains at least one ration of food.
	item = new Item() ;
	item->makeRandom( FOOD, depth ) ;
	this->placeItem(item) ;

	// Decide how many random items will be created.
	unsigned short count = randShort( DG_ITEMS_MIN, DG_ITEMS_MAX ) ;
	while( randPercent(DG_ITEMS_PCT) ) ++count ;

	// Create all the random items.
	for( unsigned short i = 0 ; i < count ; i++ )
	{
		item = new Item() ;
		item->makeRandom( depth ) ;
		this->placeItem(item) ;
	}

	// Now place some extra money in rooms, if there are rooms.
	for( unsigned short i = 0 ; i < (this->rooms).size() ; i++ )
	{
		if( randPercent(EXTRA_GOLD_CHANCE) )
		{
			item = new Item() ;
			item->makeRandom( GOLD, depth ) ;
			unsigned short row =
				randShort( (this->rooms[i])->trow+1, (this->rooms[i])->brow-1 ) ;
			unsigned short col =
				randShort( (this->rooms[i])->lcol+1, (this->rooms[i])->rcol-1 ) ;
			this->placeItem( item, row, col ) ;
		}
	}

	return ;
}

/*
 * Initializes a level of a classical "labyrinth"-type dungeon.
 */
void DungeonLevel::initLabyrinthLevel( unsigned short depth )
{
	string func = "DungeonLevel::initLabyrinthLevel(ushort)" ; // debug

	log->write( Logger::INFO, __FILE__, __LINE__, func,
	            "Initializing labyrinth level..." ) ;

	// A labyrinth level is divided into nine sectors, each of which might
	// contain a room, a maze, or a set of connecting passages.
	// -------
	// |0|1|2|
	// -------
	// |3|4|5|
	// -------
	// |6|7|8|
	// -------
	DungeonSector sectors[9] ;
	unsigned short sectorHeight = (unsigned short)((MAXDGROW-MINDGROW)/3) ;
	unsigned short sectorWidth = (unsigned short)((MAXDGCOL-MINDGCOL)/3) ;
	for( unsigned short y = 0 ; y < 3 ; y++ )
	{
		for( unsigned short x = 0 ; x < 3 ; x++ )
		{
			sectors[x+(3*y)] = (DungeonSector)
				{ (DungeonSectorType)( randShort( 0, SECTOR_TYPES-1 ) ),
				  (MINDGROW+(y*sectorHeight)),
				  (MINDGCOL+(x*sectorWidth)),
				  sectorHeight, sectorWidth,
				  0, 0, 0, 0 }
				;
		}
	}

	// First, decide which sectors will be the "starting" and "ending"
	// sectors.  The starting sector will have the upward staircase; the ending
	// sector will have the downward staircase.  Thus, after we choose these
	// sectors, we will overwrite their randomly-chosen types with SEC_ROOM.
	unsigned short startSector = randShort( 0, 8 ) ;
	unsigned short endSector ;
	do { endSector = randShort( 0, 8 ) ; } while( endSector == startSector ) ;
	sectors[startSector].type = SEC_ROOM ;
	sectors[endSector].type = SEC_ROOM ;

	// To be continued...
}

/*
 * Initializes a test level consisting of a huge room surrounded by a ring of
 * passages.
 */
void DungeonLevel::initTestLevel( unsigned short depth )
{
	string func = "DungeonLevel::initTestLevel(ushort)" ; // debug

	log->write( Logger::INFO, __FILE__, __LINE__, func,
	            "Initializing testing level..." ) ;

	// Create a room that takes up most of the screen.
	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Initializing large room..." ) ;

	(this->rooms).push_back( 
		new DungeonRoom( MINDGROW+6, MAXDGROW-6, MINDGCOL+6, MAXDGCOL-6 )
		) ;

	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Setting exits..." ) ;
	rooms.back()->exitN = 3 ;
	rooms.back()->exitS = 3 ;
	rooms.back()->exitE = 3 ;
	rooms.back()->exitW = 3 ;

	this->updateSpaces( rooms.back() ) ;

	// Create a ring passage around the big room.
	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Creating ring passage..." ) ;
	for( unsigned short r = rooms.back()->trow-2 ;
	     r <= rooms.back()->brow+2 ;
	     r++ )
	{
		(this->spaces[r][rooms.back()->lcol-2]).type = DS_PASSAGE ;
		(this->spaces[r][rooms.back()->rcol+2]).type = DS_PASSAGE ;
	}
	for( unsigned short c = rooms.back()->lcol-2 ;
	     c <= rooms.back()->rcol+2 ;
	     c++ )
	{
		(this->spaces[rooms.back()->trow-2][c]).type = DS_PASSAGE ;
		(this->spaces[rooms.back()->brow+2][c]).type = DS_PASSAGE ;
	}

	// Connect the ring passage to the big room.
	(this->spaces[rooms.back()->trow-1][rooms.back()->lcol+rooms.back()->exitN]).type = DS_PASSAGE ;
	(this->spaces[rooms.back()->brow+1][rooms.back()->lcol+rooms.back()->exitS]).type = DS_PASSAGE ;
	(this->spaces[rooms.back()->trow+rooms.back()->exitE][rooms.back()->rcol+1]).type = DS_PASSAGE ;
	(this->spaces[rooms.back()->trow+rooms.back()->exitW][rooms.back()->lcol-1]).type = DS_PASSAGE ;

	// Choose entry and exit points.
	this->nRow = randShort( rooms.back()->trow+1, rooms.back()->brow-1 ) ;
	this->nCol = randShort( rooms.back()->lcol+1, rooms.back()->rcol-1 ) ;
	log->lbuf << "Entry at (" << this->nRow << "," << this->nCol << ")." ;
	log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
	do
	{
		this->xRow = randShort( rooms.back()->trow+1, rooms.back()->brow-1 ) ;
		this->xCol = randShort( rooms.back()->lcol+1, rooms.back()->rcol-1 ) ;
	} while( this->xRow == this->nRow && this->xCol == this->xCol ) ;
	log->lbuf << "Exit at (" << this->xRow << "," << this->xCol << ")." ;
	log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;

	// BEGIN UNIT TEST CODE
	for( unsigned int i = 0 ; i < POTION_TYPES ; i++ )
	{
		this->placeItem( new Item( POTION, i, 3 ),
		                 rooms.back()->trow + 1,
		                 rooms.back()->lcol + 1 + i ) ;
	}
	for( unsigned int i = 0 ; i < SCROLL_TYPES ; i++ )
	{
		this->placeItem( new Item( SCROLL, i, 2 ),
		                 rooms.back()->trow + 2,
		                 rooms.back()->lcol + 1 + i ) ;
	}
	for( unsigned int i = 0 ; i < RING_TYPES ; i++ )
	{
		this->placeItem( new Item( RING, i, 1 ),
		                 rooms.back()->trow + 3,
		                 rooms.back()->lcol + 1 + i ) ;
	}
	// END UNIT TEST CODE

	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Level initialized.  Exiting." ) ;

	return ;
}

/*
 * Tests whether the specified dungeon level space would be passable.  The
 * function works reductively, checking each condition that might render the
 * space impassible, and returning false for each.  If none of these conditions
 * is satisfied, the function returns true.
 */
bool DungeonLevel::isPassable( unsigned short row, unsigned short col )
{
	string func = "DungeonLevel::isPassable(ushort,ushort)" ; // debug

	log->lbuf << "Space (" << row << "," << col << ")" ;

	if( spaces[row][col].type == DS_HYPERSPACE )
	{
		log->lbuf << " is in hyperspace." ;
		log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
		return false ;
	}

	if( spaces[row][col].type == DS_WALL )
	{
		log->lbuf << " is a wall." ;
		log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
		return false ;
	}

	if( !spaces[row][col].visible )
	{
		log->lbuf << " is invisible." ;
		log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
		return false ;
	}

//	if( foes->getMemberAt(row,col) != NULL )
//	{
//		log->lbuf << " is guarded by a creature." ;
//		log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
//		return false ;
//	}

//	if( spaces[row][col].feature != NULL
//	 && !(spaces[row][col].feature)->isPassable() )
//	{
//		log->lbuf << " contains an impassable feature." ;
//		log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
//		return false ;
//	}

	log->clearBuffer() ;
	return true ;
}

/*
 * Lights the area around the specified location.  If the location is inside a
 * room, then the entire room is lit.  If the location is inside a passage, then
 * the entire passage is lit.
 */
void DungeonLevel::lightSpace( unsigned short r, unsigned short c )
{
	string func = "DungeonLevel::lightSpace(ushort,ushort)" ; // debug

	log->lbuf << "Attempting to light space at (" << r << "," << c << ")." ;
	log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;

	switch( spaces[r][c].type )
	{
		case DS_HYPERSPACE:
			log->lbuf << "Tried to brighten hyperspace at ("
			          << r << "," << c << ")."
			          ;
			log->writeBuffer( Logger::WARNING, __FILE__, __LINE__, func ) ;
			break ;
		case DS_WALL:
			log->lbuf << "Space at " << r << "," << c << ") is a wall." ;
			log->writeBuffer( Logger::WARNING, __FILE__, __LINE__, func ) ;
			break ;
		case DS_DOOR:
			log->lbuf << "Lighting passage from door at ("
			          << r << "," << c << ")."
			          ;
			log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
			lightPassage(r,c) ;
			// no break; leak into DS_FLOOR case block.
		case DS_FLOOR:
			log->lbuf << "Lighting room around ("
			          << r << "," << c << ")."
			          ;
			log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
			lightRoom( getRoom(r,c) ) ;
			break ;
		case DS_PASSAGE:
			log->lbuf << "Lighting passages around ("
			          << r << "," << c << ")."
			          ;
			log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
			lightPassage(r,c) ;
			break ;
		default:
			log->lbuf << "Tried to light invalid space type: "
			          << spaces[r][c].type
			          ;
			log->writeBuffer( Logger::WARNING, __FILE__, __LINE__, func ) ;
	}

	return ;
}

/*
 * Lights a passage.
 */
void DungeonLevel::lightPassage( unsigned short row, unsigned short col )
{
	string func = "DungeonLevel::lightPassage(ushort,ushort)" ;

	log->lbuf << "Lighting passage from (" << row << "," << col << ")." ;
	log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;

	spaces[row][col].mapped = true ;
	spaces[row][col].lit = true ;

	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Lighting passage northward." ) ;
	for( unsigned short r = row ; r > MINDGROW ; r-- )
	{ // Light passage to the north.
		if( spaces[r-1][col].lit )
		{
			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Passage is already lit." ) ;
			break ;
		}
		else if( spaces[r-1][col].type == DS_PASSAGE
		      || spaces[r-1][col].type == DS_DOOR )
		{
			spaces[r-1][col].mapped = true ;
			spaces[r-1][col].lit = true ;
			log->silence() ;
			drawSpace( r-1, col ) ;
			log->wake() ;
		}
		else
		{
			log->lbuf << "Stopped after " << row-r
			          << ( (row-r) == 1 ? " space." : " spaces." )
			          ;
			log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
			break ;
		}
	}

	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Lighting passage southward." ) ;
	for( unsigned short r = row ; r < MAXDGROW ; r++ )
	{ // Light passage to the south.
		if( spaces[r+1][col].lit )
		{
			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Passage is already lit." ) ;
			break ;
		}
		else if( spaces[r+1][col].type == DS_PASSAGE
		      || spaces[r+1][col].type == DS_DOOR )
		{
			spaces[r+1][col].mapped = true ;
			spaces[r+1][col].lit = true ;
			log->silence() ;
			drawSpace( r+1, col ) ;
			log->wake() ;
		}
		else 
        {
            log->lbuf << "Stopped after " << r-row
                      << ( (r-row) == 1 ? " space." : " spaces." )
                      ;
			log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
            break ;
        }
	}

	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Lighting passage westward." ) ;
	for( unsigned short c = col ; c > MINDGCOL ; c-- )
	{ // Light passage to the west.
		if( spaces[row][c-1].lit )
		{
			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Passage is already lit." ) ;
			break ;
		}
		else if( spaces[row][c-1].type == DS_PASSAGE
		      || spaces[row][c-1].type == DS_DOOR )
		{
			spaces[row][c-1].mapped = true ;
			spaces[row][c-1].lit = true ;
			log->silence() ;
			drawSpace( row, c-1 ) ;
			log->wake() ;
		}
		else
        {
            log->lbuf << "Stopped after " << col-c
                      << ( (col-c) == 1 ? " space." : " spaces." )
                      ;
			log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
            break ;
        }
	}

	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Lighting passage eastward." ) ;
	for( unsigned short c = col ; c < MAXDGCOL ; c++ )
	{ // Light passage to the east.
		if( spaces[row][c+1].lit )
		{
			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Passage is already lit." ) ;
			break ;
		}
		else if( spaces[row][c+1].type == DS_PASSAGE
		      || spaces[row][c+1].type == DS_DOOR )
		{
			spaces[row][c+1].mapped = true ;
			spaces[row][c+1].lit = true ;
			log->silence() ;
			drawSpace( row, c+1 ) ;
			log->wake() ;
		}
		else
        {
            log->lbuf << "Stopped after " << c-col
                      << ( (c-col) == 1 ? " space." : " spaces." )
                      ;
			log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
            break ;
        }
	}

	return ;
}

/*
 * Lights a room.
 */
void DungeonLevel::lightRoom( DungeonRoom *room )
{
	string func = "DungeonLevel::lightRoom(DungeonRoom*)" ; // debug

	if( room == NULL )
	{
		log->write( Logger::ERROR, __FILE__, __LINE__, func,
		            "Tried to write nonexistent room." ) ;
		return ;
	}

	if( room->lit )
	{
		log->write( Logger::TRACE, __FILE__, __LINE__, func,
		            "Room was already lit." ) ;
		return ;
	}

	for( unsigned int r = room->trow ; r <= room->brow ; r++ )
	{
		for( unsigned int c = room->lcol ; c <= room->rcol ; c++ )
		{
			spaces[r][c].visible = true ;
			spaces[r][c].mapped = true ;
			spaces[r][c].lit = true ;

			log->silence() ;
			drawSpace( r, c ) ;
			log->wake() ;
		}
	}

	room->lit = true ;

	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Successfully lit room." ) ;

	return ;
}

/*
 * Moves the player in the direction indicated by the vertical (v) and
 * horizontal (h) offsets, by a specified number of paces.  The return value
 * indicates whether the movement was, in fact, interrupted.
 */
bool DungeonLevel::movePlayer( short v, short h, unsigned short count )
{
	string func = "DungeonLevel::movePlayer(short,short,ushort)" ; // debug
	log->lbuf << "Attempting to move "
	          << count
	          << ( count == 1 ? " pace." : " paces." )
	          ;
	log->writeBuffer( Logger::INFO, __FILE__, __LINE__, func ) ;

	msg->flush() ;
	msg->clear() ;
	refresh() ;

	bool interrupted = false ;
	unsigned short steps = count ; // NOTE: Counts steps remaining!
	do
	{
		if( !(interrupted = stepPlayer(v,h)) ) --steps ;
	} while( !interrupted && steps > 0 ) ;

	if( interrupted )
	{
		log->lbuf << "Movement interrupted with " << steps
		          << ( steps == 1 ? " step remaining." : " steps remaining." )
		          ;
		log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
	}

	return interrupted ;
}

/*
 * Attempts to have the character pick up the items at the current location.
 * The return value indicates whether the operation succeeded.
 */
bool DungeonLevel::pickUpItems( Actor *a )
{
	string func = "DungeonLevel::pickUpItems(Actor*)" ; // debug

	unsigned short row = a->getRow() ;
	unsigned short col = a->getCol() ;
	bool pickedUp = false ;

	Pack *items = spaces[row][col].items ;

	if( items == NULL )
	{ // If there's no pack, don't try to pick anything up.
		log->lbuf << "No pack at (" << row << "," << col << ")." ;
		log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
		return false ;
	}

	if( items->getCount() == 0 )
	{ // If the pack is already empty, free its memory.
		log->lbuf << "No items in pack at (" << row << "," << col
			<< ").  Deleting pack and returning false."
			;
		log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
		delete items ;
		spaces[row][col].items = NULL ;
		items = NULL ;
		return false ;
	}

	if( items->getCount() > 1 )
	{
		Inventory *inv = NULL ;
		bool done = false ;

		do
		{
			refresh() ;

			if( inv != NULL ) { delete inv ; refresh() ; }
			inv = items->toInventory() ;
			inv->setWindow() ;
			inv->setWindowHeader( "Pick up which item?" ) ;
			Item *item = inv->selectItem() ;
			if( item != NULL )
			{
				if( a->addItemToPack(item) )
				{
					items->popItem(item) ;
					if( items->getCount() == 0 )
						done = true ;
				}
				else
				{
					displayDialog(
						"Can't pick this up.  Check your inventory."
						) ;
				}
			}
			else done = true ;
		} while( !done ) ;

		delete inv ;
		msg->flush() ;
		msg->clear() ;
		refresh() ;
	}
	else
	{
		items->reset() ; // Reset iterator.
		Item *i = items->getNextItem() ;
		if( a->addItemToPack(i) )
		{ // Item added successfully.
			items->popItem() ; // Remove from floor.
			msg->buf << a->getName() << " picked up "
			         << i->getDesc() << "."
			         ;
			msg->pushBuffer() ;
			msg->draw() ;
		}
	}

	if( items->getCount() == 0 )
	{ // If the pack is now empty, free its memory.
		log->lbuf << "No more items in pack at (" << row << "," << col
			<< ").  Deleting pack."
			;
		log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
		delete items ;
		spaces[row][col].items = NULL ;
		items = NULL ;
	}

	return true ;
}

/*
 * Places an item onto the dungeon floor at a random location.
 */
void DungeonLevel::placeItem( Item *item )
{
	string func = "DungeonLevel::placeItem(Item*)" ; // debug

	unsigned short row = 0 ;
	unsigned short col = 0 ;
	bool placed = false ;

	do
	{
		row = randShort( MINDGROW, MAXDGROW ) ;
		col = randShort( MINDGCOL, MAXDGCOL ) ;
		if( isPassable(row,col) )
		{
			log->lbuf << "Random location: (" << row << "," << col << ")." ;
			log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
			this->placeItem( item, row, col ) ;
			placed = true ;
		}
	} while( !placed ) ;

	return ;
}

/*
 * Places an item onto the floor at a specific location.
 */
void DungeonLevel::placeItem( Item *item,
                              unsigned short row, unsigned short col )
{
	string func = "DungeonLevel::placeItem(Item*,ushort,ushort)" ; // debug

	if( spaces[row][col].items == NULL )
		spaces[row][col].items = new Pack() ;

	(spaces[row][col].items)->addItem( item, true ) ;

	log->lbuf << "Placed item at (" << row << "," << col << ")." ;
	log->writeBuffer( Logger::INFO, __FILE__, __LINE__, func ) ;

	return ;
}

/*
 * Processes the effect of a scroll.  Because scrolls can act on rogues, their
 * items, monsters, tiles, and all sorts of other things, the scope limitations
 * imposed by the engine's OO design force us to put this function here, where
 * it can have an effect on everything in the dungeon level.
 */
void DungeonLevel::processScroll( Item *scroll )
{
	string func = "DungeonLevel::processScroll(Item*)" ; // debug

	if( scroll == NULL ) return ;

	log->write( Logger::INFO, __FILE__, __LINE__, func,
	            "Processing effect of scroll:" ) ;
	(void)scroll->getDiag() ;

	scroll->use(1) ;

	if( scroll->getQuantity() == 0 )
		player->removeItemFromPack(scroll) ;

	switch( scroll->getKind() )
	{
		case S_ORUST:
		{
			msg->buf << "A golden aura forms around "
			         << player->getName() << "..."
			         ;
			msg->pushBuffer() ;
			msg->draw() ; refresh() ;
			Item *enh =
				player->displayInventory( K_EQUIPMENT, "Offer which item?" ) ;
			if( enh != NULL )
			{
				enh->setProtected(true) ;
				msg->flush() ; msg->clear() ;
				msg->buf << player->getName() << " now has " 
				         << enh->getDesc() << "."
				         ;
				msg->pushBuffer() ;
			}
			else msg->push( "The aura dissipates." ) ;
		}	break ;
		case S_HARDEN:
		{
			Item *armor = player->getEquipment( EQ_ARMOR ) ;
			if( armor != NULL )
			{
				if( player->hasNecklace(N_WIZARDRY) )
					armor->setBonus( armor->getBonus() + randShort(1,2) ) ;
				else armor->setBonus( armor->getBonus() + 1 ) ;
				msg->buf << player->getName()
					<< "'s armor is bathed in a soft blue glow for a moment."
					;
			}
			else
			{
				msg->buf << player->getName()
					<< "'s body briefly glows blue, but nothing happens."
					;
			}
			msg->pushBuffer() ;
		}	break ;
		case S_SHARPEN:
		{
			Item *weapon = player->getEquipment( EQ_MWEAPON ) ;
			if( weapon != NULL )
			{
				if( player->hasNecklace(N_WIZARDRY) )
					weapon->setBonus( weapon->getBonus() + randShort(1,2) ) ;
				else weapon->setBonus( weapon->getBonus() + 1 ) ;
				msg->buf << player->getName()
					<< "'s weapon is bathed in a soft blue glow for a moment."
					;
			}
			else
			{
				msg->buf << player->getName()
					<< "'s hand glows for a moment, but nothing happens."
					;
			}
			msg->pushBuffer() ;
		}	break ;
		case S_IDENTIFY:
		{
			msg->push( "This is a scroll of identify." ) ;
			msg->draw() ; refresh() ;
			Item *item = player->displayInventory( "Identify which item?" ) ;
			if( item != NULL )
			{
				msg->flush() ; msg->clear() ;
				item->identify() ;
				msg->buf << "Identified: " << item->getDesc() ;
				msg->pushBuffer() ;
			}
			else msg->push( "The scroll crumbles to dust." ) ;
		}	break ;
		case S_UNCURSE:
		{
			msg->buf << "A silver aura forms around "
				<< player->getName() << "..."
				;
			msg->pushBuffer() ;
			msg->draw() ; refresh() ;
			Item *enh =
				player->displayInventory( K_EQUIPMENT, "Offer which item?" ) ;
			if( enh != NULL && enh->isCursed() )
			{
				enh->setCursed(false) ;
				msg->flush() ; msg->clear() ;
				msg->buf << "The curse has been lifted from "
					<< player->getName() << "'s item."
					;
				msg->pushBuffer() ;
			}
			else msg->push( "The aura dissipates." ) ;
		}	break ;
		case S_PARALYZE:
			if( scroll->isRecognized() )
			{ // Paralyzes monsters.  Save for later.
				;
			}
			else
			{ // Paralyze player instead.
				if( player->inflict( PARALYZED, roll(1,6,1), SCROLL_SAVE ) )
					msg->buf << player->getName() << "'s body stiffens." ;
				else
				{
					msg->buf << player->getName()
						<< "'s body stiffens momentarily."
						;
				}
				msg->pushBuffer() ;
			}
			break ;
		case S_SLEEP:
			if( scroll->isRecognized() )
			{ // Puts monsters to sleep.  Save for later.
				;
			}
			else
			{ // Put player to sleep instead.
				if( player->inflict( ASLEEP, roll(1,6,1), SCROLL_SAVE ) )
					msg->buf << player->getName() << " falls asleep." ;
				else
				{
					msg->buf << player->getName()
						<< "'s eyelids droop drowsily for a moment."
						;
				}
				msg->pushBuffer() ;
			}
			break ;
		case S_SCARE: // Scares monsters.  Save for later.
			break ;
		case S_SPAWN: // Spawns a monster.  Save for later.
			break ;
		case S_AGGRAVATE: // Aggravates monsters.  Save for later.
			break ;
		case S_MAPPING:
			log->silence() ;
			for( unsigned short r = MINDGROW ; r <= MAXDGROW ; r++ )
			for( unsigned short c = MINDGCOL ; c <= MAXDGCOL ; c++ )
			{
				this->spaces[r][c].mapped = true ;
				this->drawSpace(r,c) ;
			}
			log->wake() ;
			msg->push( "The ink flows across the page to form a map." ) ;
			break ;
		case S_TELEPORT:
		{
			unsigned short r ;
			unsigned short c ;
			log->write( Logger::TRACE, __FILE__, __LINE__, func,
				"Finding a passable space for a teleport destination." ) ;
			do
			{
				r = randShort( MINDGROW, MAXDGROW ) ;
				c = randShort( MINDGCOL, MAXDGCOL ) ;
			} while( !(this->bamfPlayer(r,c)) ) ;
			if( !(scroll->isRecognized()) )
				player->inflict( CONFUSED, roll(1,6,1), SCROLL_SAVE ) ;
			msg->buf << "The world around " << player->getName()
			         << " changes in a puff of acrid black smoke."
			         ;
			msg->pushBuffer() ;
		}	break ;
		default: ; 
	}

	scroll->identify() ;

	if( scroll->getQuantity() == 0 )
	{
		log->write( Logger::TRACE, __FILE__, __LINE__, func,
			"Deleting spent scroll stack." ) ;
		delete scroll ;
	}

	return ;
}

/*
 * Redraws the main screen based on spaces[][] data.
 */
void DungeonLevel::redraw(void)
{
	string func = "DungeonLevel::redraw()" ; // debug

	log->write( Logger::TRACE, __FILE__, __LINE__, func, "Redrawing screen." ) ;

	log->silence() ;

	for( unsigned short r = MINDGROW ; r <= MAXDGROW ; r++ )
	for( unsigned short c = MINDGCOL ; c <= MAXDGCOL ; c++ )
		drawSpace(r,c) ;

	log->wake() ;

	return ;
}

/*
 * Allows player to "run" a certain number of steps in a direction.  The run is
 * interrupted whenever the player runs against something "interesting".  The
 * function returns true if the run is interrupted.
 */
bool DungeonLevel::runPlayer( Ally *a, short v, short h )
{
	string func = "DungeonLevel::runPlayer(Ally*,short,short)" ; // debug

	log->lbuf << a->getName() " is attempting to run "
		<< describeDirection(v,h)
		<< " from (" << a->getRow() << "," << a->getCol() << ")."
		;
	log->writeBuffer( Logger::INFO, __FILE__, __LINE__, func ) ;

	msg->flush() ;
	msg->clear() ;
	refresh() ;

    bool interrupted = false ;
    unsigned short steps = 0 ; // NOTE: Counts steps traversed!
    do
    {
		if( !(interrupted = stepPlayer(a,v,h)) ) ++steps ; // FLARGLE2
		if( spideySense( a->getRow(), a->getCol() ) )
			interrupted = true ;
	} while( !interrupted ) ;

    if( interrupted )
    {
        log->lbuf << "Movement interrupted after " << steps
                  << ( steps == 1 ? " step." : " steps." )
                  ;
        log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
    }

    return interrupted ;
}

/*
 * Indicates whether anything "interesting" is present at the indicated
 * space on the dungeon level.  Interesting objects can interrupt a running
 * movement or cause other distractions.  For the purposes of movement, we
 * assume that the passability of the space has already been checked, so we
 * won't bother doing that here.
 */
bool DungeonLevel::spideySense( unsigned short row, unsigned short col )
{
	string func = "DungeonLevel::spideySense(ushort,ushort)" ; // func

	// If we're in a passage, see if there's an intersection.
	if( spaces[row][col].type == DS_PASSAGE )
	{
		unsigned short passages = 0 ;
		if( isPassable( row-1, col ) ) ++passages ;
		if( isPassable( row+1, col ) ) ++passages ;
		if( isPassable( row, col-1 ) ) ++passages ;
		if( isPassable( row, col+1 ) ) ++passages ;
		if( passages > 2 )
		{
			log->lbuf << "Found an intersection at "
			          << "(" << row << "," << col << ")."
			          ;
			log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
			return true ;
		}
	}

	// Doors are always interesting.
	if( spaces[row][col].type == DS_DOOR )
	{
		log->lbuf << "Found a door at "
		          << "(" << row << "," << col << ")."
		          ;
		log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
		return true ;
	}

	// Check to see if there are any objects adjacent to this space.
	for( unsigned short r = row-1 ; r <= row+1 ; r++ )
	for( unsigned short c = col-1 ; c <= col+1 ; c++ )
	{
		if( spaces[r][c].type == DS_DOOR
		 || spaces[r][c].items != NULL
//		 || spaces[r][c].creature != NULL
//		 || spaces[r][c].feature != NULL
//		 || ( (spaces[r][c].trap)->isVisible()
//		   && (spaces[r][c].trap)->isArmed() )
		  )
		{
			log->lbuf << "Found an interesting object at "
			          << "(" << r << "," << c << ")."
			          ;
			log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
			return true ;
		}
	}

	// By reaching this point, we've found nothing interesting.
	return false ;
}

/*
 * Player takes a single step in the specified direction.  This function is
 * used in a loop within movePlayer() and runPlayer().
 */
bool DungeonLevel::stepPlayer( Ally *a, short v, short h )
{
	string func = "DungeonLevel::stepPlayer(Ally*,short,short)" ; // debug

	// The parameters could be any crazy things; we care only about their
	// signedness.
	short vs = signOf(v) ;
	short hs = signOf(h) ;

	if( a->getStatus(CONFUSED) > 0 )
	{ // Randomize direction.
		log->lbuf << a->getName() << " is confused.  " ;
		do
		{ // Ensure character doesn't stand still.
			vs = (short)(randShort(1,3)) - 2 ;
			hs = (short)(randShort(1,3)) - 2 ;
		} while( vs == 0 && hs == 0 ) ;
		log->lbuf << "Randomized direction to (" << vs << "," << hs << ")." ;
		log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
	}

	unsigned short row = a->getRow() ;
	unsigned short col = a->getCol() ;
	bool interrupted = false ;

	if( ( vs == -1 && row <= MINDGROW ) // too high to go up
	 || ( vs ==  1 && row >= MAXDGROW ) // too low to go down
	 || ( hs == -1 && row <= MINDGCOL ) // too far to go left
	 || ( hs ==  1 && row >= MAXDGCOL ) // too far to go right
	  )
	{
		log->write( Logger::TRACE, __FILE__, __LINE__, func,
		            "Stopped by edge of dungeon." ) ;
		msg->push( "Your way is blocked by a thick white mist." ) ;
		interrupted = true ;
	}
	else if( !isPassable( row+vs, col+hs ) )
	{
		log->write( Logger::TRACE, __FILE__, __LINE__, func,
		            "Target square is impassible." ) ;
		interrupted = true ;
	}
	else if( vs != 0 && hs != 0
	      && ( !isPassable( row+vs, col ) || !isPassable( row, col+hs ) ) )
	{ // Block diagonal movement if spaces around the diagonal are impassable.
		log->write( Logger::TRACE, __FILE__, __LINE__, func,
			"Target bordering diagonal movement is impassible." ) ;
		interrupted = true ;
	}
	else
	{ // Do the movement.
		if( a->hasRing(R_TELEPORT) && randPercent(10) )
		{ // Randomly teleport player instead of moving.
			unsigned short r ;
			unsigned short c ;
			log->write( Logger::TRACE, __FILE__, __LINE__, func,
				"Random teleportation from ring interrupted normal movement." );
			do
			{
				r = randShort( MINDGROW, MAXDGROW ) ;
				c = randShort( MINDGCOL, MAXDGCOL ) ;
			} while( !(this->bamfPlayer(r,c)) ) ; // FLARGLE3
			player->inflict( CONFUSED, roll(1,4,1), 5 ) ;
			msg->buf << player->getName() << "'s ring flashes and "
				<< ( player->getGender() ? "he " : "she " )
				<< "disappears in a puff of black smoke."
				;
			msg->pushBuffer() ;
			msg->draw() ; refresh() ;
			return true ;
		}

		// Darken the space. (might light it later)
		darkenCurrentSpace() ;

		player->setLocation( row+vs, col+hs ) ;
		log->lbuf << "Moved to (" << row+vs << "," << col+hs << ")." ;
		log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;

		// Relight the space if necessary, or at least map it.
		if( player->getStatus(BLINDED) <= 0 )
			lightCurrentSpace() ;
		else
			spaces[row+vs][col+hs].mapped = true ;

		interrupted = this->catchStateChange( player, player->tick() ) ;

		// If player was poisoned, ensure that he's still alive.
		if( player->getHP() <= 0 )
		{
			log->lbuf << player->getName() << " has died." ;
			log->writeBuffer( Logger::INFO, __FILE__, __LINE__, func ) ;
			interrupted = true ;
		}
		else if( spaces[row+vs][col+hs].items != NULL )
		{
			unsigned short ic = (spaces[row+vs][col+hs].items)->getCount() ;

			log->lbuf << "Pack of " << ic
				<< ( ic == 1 ? " item found here." : " items found here." )
				;
			log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;

			if( player->getStatus(BLINDED) > 0 )
			{
				if( randPercent(25) )
				{
					msg->buf << player->getName()
						<< " tripped over something."
						;
					msg->pushBuffer() ;
				}
			}
			else
			{
				msg->buf << player->getName() << " finds " ;

				if( ic > 1 )
				{
					msg->buf << ic << " items here."
						;
				}
				else
				{
					msg->buf
					  << ((spaces[row+vs][col+hs].items)->getItem(0))->getDesc()
				      << " here."
				      ;
				}

				msg->pushBuffer() ;
			}
		}

		drawSpace( row, col ) ;
		drawSpace( row+vs, col+hs ) ;
	}

	msg->draw() ;
	refresh() ;

	return interrupted ;
}

/*
 * Updates the spaces[][] array to note the presence of a room.
 */
void DungeonLevel::updateSpaces( DungeonRoom *room )
{
	string func = "DungeonLevel::updateSpaces(DungeonRoom)" ; // debug
	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Updating spaces table..." ) ;

	// Note walls.
	log->lbuf << "Scribing walls between rows "
	          << room->trow << " and " << room->brow
	          << " and columns "
	          << room->lcol << " and " << room->rcol
	          ;
	log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
	for( unsigned short c = room->lcol ; c <= room->rcol ; c++ )
	{
		(this->spaces[room->trow][c]).type = DS_WALL ;
		(this->spaces[room->brow][c]).type = DS_WALL ;
	}
	for( unsigned short r = room->trow ; r <= room->brow ; r++ )
	{
		(this->spaces[r][room->lcol]).type = DS_WALL ;
		(this->spaces[r][room->rcol]).type = DS_WALL ;
	}

	// Note flooring.
	log->lbuf << "Filling in floor between ("
	          << room->trow + 1 << "," << room->lcol + 1 << ") and ("
	          << room->brow - 1 << "," << room->rcol - 1 << ")"
	          ;
	log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
	for( unsigned short c = room->lcol+1 ; c < room->rcol ; c++ )
		for( unsigned short r = room->trow+1 ; r < room->brow ; r++ )
			spaces[r][c].type = DS_FLOOR ;

	// Note doors.
	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Noting exits." ) ;
	if( room->exitN > 0 )
		spaces[room->trow][room->lcol+room->exitN].type = DS_DOOR ;
	if( room->exitS > 0 )
		spaces[room->brow][room->lcol+room->exitS].type = DS_DOOR ;
	if( room->exitE > 0 )
		spaces[room->trow+room->exitE][room->rcol].type = DS_DOOR ;
	if( room->exitW > 0 )
		spaces[room->trow+room->exitW][room->lcol].type = DS_DOOR ;

	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Spaces updated." ) ;
	return ;
}

/* *** CLASS DEFINITION (DungeonRoom) ************************************** */

// /// STATIC FUNCTIONS (DungeonRoom) /////////////////////////// (none) /// //

// /// CONSTRUCTORS (DungeonRoom) ////////////////////////////////////////// //

/*
 * Constructs with explicit dimensions but no exits.
 */
DungeonRoom::DungeonRoom( unsigned short trow, unsigned short brow,
                          unsigned short lcol, unsigned short rcol )
{
	log->lbuf << "Constructing room from rows " << trow << " to " << brow
	          << " and columns " << lcol << " to " << rcol
	          ;
	log->writeBuffer( Logger::INFO, __FILE__, __LINE__, "DungeonRoom(...)" ) ;

	this->trow = trow ; this->brow = brow ;
	this->lcol = lcol ; this->rcol = rcol ;
	this->exitN = this->exitS = this->exitE = this->exitW = -1 ;

	this->lit = false ;
}

// /// ACCESSORS (DungeonRoom) ///////////////////////////////////////////// //

/*
 * Returns true if the given location is inside this room.
 */
bool DungeonRoom::isInRoom( unsigned short row, unsigned short col )
{
	return ( row >= this->trow && row <= this->brow
	      && col >= this->lcol && col <= this->rcol ) ;
}

// /// MUTATORS (DungeonRoom) ////////////////////////////////////////////// //

/*
 * Sets the coordinates of various exits.
 */
void DungeonRoom::setExitN( short col ) { exitN = col ; return ; }
void DungeonRoom::setExitS( short col ) { exitS = col ; return ; }
void DungeonRoom::setExitE( short row ) { exitE = row ; return ; }
void DungeonRoom::setExitW( short row ) { exitW = row ; return ; }

// /// OTHER PUBLIC FUNCTIONS (DungeonRoom) ///////////////////// (none) /// //

// /// PRIVATE FUNCTIONS (DungeonRoom) ////////////////////////// (none) /// //

/* ************************************************************************* */
