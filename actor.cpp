/*
 * actor.cpp
 * You may play with this source however you like, as long as you don't remove
 * this comment, claim undue credit, or make a profit on it.  Enjoy.
 * CHANGE HISTORY
 * 2011-11-17
 *  Created file.
 * 2012-01-25
 *  Split from actor.cpp.
 */

#ifndef _DUNGEONEER_H_
#include "dungeoneer.h"
#endif

#include <sstream>
#include <string>
#include "actor.h"
#include "command.h"
#include "debug.h"
#include "pack.h"
#include "random.h"
#include "ui.h"
#include "window.h"
using namespace std ;

/* *** EXTERNAL REFERENCES ************************************************* */

extern Logger *log ; // debug.cpp
extern string statusEffectNames[STATUS_EFFECTS] ; // ui.cpp

/* *** LOCAL DATA ********************************************************** */

/* Random character names to use during character generation. */
string randomMaleNames[26] = {
	"Albion", "Bobbin", "Cantor", "Dorfman", "Edgar", "Flargle", "Gafnald",
	"Hrothgar", "Inigo", "Jasper", "Khaaaaaaaan", "Lederchind", "Milhaus",
	"Nils", "Ogoroth", "Pernick", "Quill", "Rothaus", "Sitheroph", "Tribble",
	"Ungawa", "Victor", "Wollongong", "Xander", "Yohaa", "Zeactor"
};
string randomFemaleNames[26] = {
	"Alice", "Bridget", "Chantrix", "Dagger", "Eve", "Flora", "Ginger",
	"Hannah", "Inara", "Julia", "K'Ehlayr", "Laurana", "Minerva",
	"Ninurta", "Ophelia", "Persephone", "Quinalia", "Rydia", "Shaka",
	"Trinity", "Uma", "Veronica", "Walchia", "Xedria", "Yoli", "Ziera"
};

/*
 * Experience points required to reach a given level.  For any experience level
 * N, the XP required to reach that level is always xpLevels[N-1], and the XP
 * required to advance to the next level is xpLevels[N].
 */
unsigned int xpLevels[20] = {
	0,			// 01
	1000,		// 02
	3000,		// 03
	6000,		// 04
	10000,		// 05
	15000,		// 06
	21000,		// 07
	28000,		// 08
	36000,		// 09
	45000,		// 10
	55000,		// 11
	66000,		// 12
	78000,		// 13
	91000,		// 14
	105000,		// 15
	120000,		// 16
	136000,		// 17
	153000,		// 18
	171000,		// 19
	190000		// 20
} ;

/* *** LOCAL PROCEDURE PROTOTYPES ******************************* (none) *** */

/* *** PROCEDURE DEFINITIONS ************************************ (none) *** */

/* *** CLASS DEFINITION (Actor) ******************************************** */

// /// STATIC FUNCTIONS (classname) ///////////////////////////// (none) /// //

// /// CONSTRUCTORS (Actor) //////////////////////////////////////////////// //

Actor::Actor()
{
	log->write( Logger::INFO, __FILE__, __LINE__, "Actor()",
	            "Constructing Actor object..." ) ;

	maxhp = hp = maxhp_d = 0 ;
	str = dex = vit = wil = 0 ;
	str_d = dex_d = vit_d = wil_d = 0 ;
	status_effects = new unsigned short[STATUS_EFFECTS] ;
	xplevel = 0 ;
	xp = 0 ;
	hunger = 0 ;
	stepsToHeal = 0 ;

	equipment = new Item*[EQUIPMENT_SLOTS] ;
	for( int i = 0 ; i < EQUIPMENT_SLOTS ; i++ )
		equipment[i] = NULL ;

	pack = new Pack() ;

	gold = 0 ;

	row = col = 0 ;

	displaychar = '%' ;
	displaycolor = 7 ;
	name = "" ;
	hasY = true ;
}

// /// DESTRUCTORS (Actor) ///////////////////////////////////////////////// //

Actor::~Actor()
{
	delete[] status_effects ;
	delete[] equipment ;
	delete pack ;
}

// /// ACCESSORS (Actor) /////////////////////////////////////////////////// //

short Actor::getAC(void)
{
	short ac = 10 + getMDEX() ; // Base AC is 10 + mDEX.

	for( unsigned short i = 0 ; i < EQUIPMENT_SLOTS ; i++ )
		if( equipment[i] != NULL )
			ac += equipment[i]->getACBonus() ;

	return ac ;
}

string Actor::getName(void) { return this->name ; }

bool Actor::getGender(void) { return this->hasY ; }

char Actor::getGenderChar(void) { return ( this->hasY ? 'M' : 'F' ) ; }

short Actor::getMaxHP( void )
{
	if( maxhp_d >= ( maxhp + ( xplevel * this->getMVIT() ) ) )
		return 1 ;
	else
		return( maxhp + ( xplevel * this->getMVIT() ) - maxhp_d ) ;
}

short Actor::getHP(void) { return hp ; }

unsigned short Actor::getSTR( void )
{
	short s = str ;

	if( ( equipment[EQ_LRING] != NULL )
	 && ( equipment[EQ_LRING]->getKind() == R_STRBOOST ) )
		s += equipment[EQ_LRING]->getBonus() ;

	if( ( equipment[EQ_RRING] != NULL )
	 && ( equipment[EQ_RRING]->getKind() == R_STRBOOST ) )
		s += equipment[EQ_RRING]->getBonus() ;

	s -= str_d ;

	if( s < 1 ) return 1 ;
	else return ((unsigned short) s) ;
}

unsigned short Actor::getDEX( void )
{
	short d = (short)dex ;

	if( ( equipment[EQ_LRING] != NULL )
	 && ( equipment[EQ_LRING]->getKind() == R_DEXBOOST ) )
		d += equipment[EQ_LRING]->getBonus() ;

	if( ( equipment[EQ_RRING] != NULL )
	 && ( equipment[EQ_RRING]->getKind() == R_DEXBOOST ) )
		d += equipment[EQ_RRING]->getBonus() ;

	d -= dex_d ;

	if( d < 1 ) return 1 ;
	else return ((unsigned short) d) ;
}

unsigned short Actor::getVIT( void )
{
	short v = (short)vit ;

	if( ( equipment[EQ_LRING] != NULL )
	 && ( equipment[EQ_LRING]->getKind() == R_VITBOOST ) )
		v += equipment[EQ_LRING]->getBonus() ;

	if( ( equipment[EQ_RRING] != NULL )
	 && ( equipment[EQ_RRING]->getKind() == R_VITBOOST ) )
		v += equipment[EQ_RRING]->getBonus() ;

	v -= vit_d ;

	if( v < 1 ) return 1 ;
	else return ((unsigned short) v) ;
}

unsigned short Actor::getWIL( void )
{
	short w = (short)wil ;

	if( ( equipment[EQ_LRING] != NULL )
	 && ( equipment[EQ_LRING]->getKind() == R_WILBOOST ) )
		w += equipment[EQ_LRING]->getBonus() ;

	if( ( equipment[EQ_RRING] != NULL )
	 && ( equipment[EQ_RRING]->getKind() == R_WILBOOST ) )
		w += equipment[EQ_RRING]->getBonus() ;

	w -= wil_d ;

	if( w < 1 ) return 1 ;
	else return ((unsigned short) w) ;
}

short Actor::getMSTR( void )
{
	return( ( short(getSTR()) / 2 ) - 5 ) ;
}

short Actor::getMDEX( void )
{
	return( ( short(getDEX()) / 2 ) - 5 ) ;
}

short Actor::getMVIT( void )
{
	return( ( short(getVIT()) / 2 ) - 5 ) ;
}

short Actor::getMWIL( void )
{
	return( ( short(getWIL()) / 2 ) - 5 ) ;
}

bool Actor::getStatus( StatusEffect stype )
{
	if( status_effects[stype] > 0 ) return true ;
	else return false ;
}

unsigned short Actor::getLevel( void )
{
	if( xplevel_d >= xplevel ) return 1 ;
	else return( xplevel - xplevel_d ) ;
}

unsigned short Actor::getActualLevel(void)
{
	return xplevel ;
}

unsigned int Actor::getXP(void) { return xp ; }

unsigned short Actor::getXPPercent(void)
{
	return (unsigned short)
		( ( this->xp - xpLevels[(this->xplevel)-1 ] ) * 100 )
		  / ( xpLevels[this->xplevel] - xpLevels[(this->xplevel)-1] ) ;
}

unsigned short Actor::getHunger(void) { return hunger ; }

unsigned short Actor::getHungerState( void )
{
	if( hunger <= Actor::STARVED )       return Actor::STARVING ;
	else if( hunger <= Actor::FAINTING ) return Actor::FAINT ;
	else if( hunger <= Actor::WEAKNESS ) return Actor::WEAK ;
	else if( hunger <= Actor::HUNGER )   return Actor::HUNGRY ;
	else return 0 ;
} 

unsigned int Actor::getGold(void) { return gold ; }

/*
 * Returns a count of items equipped by the actor, by counting the number of
 * equipped item pointers that aren't NULL.
 */
unsigned short Actor::getEquippedItemCount(void)
{
	unsigned short c = 0 ;

	for( int i = 0 ; i < EQUIPMENT_SLOTS ; i++ )
		if( equipment[i] != NULL ) ++c ;

	return c ;
}

/*
 * Gets the equipped item at the specified equipment slot.
 */
Item * Actor::getEquipment( EquipmentSlot slot )
{
	if( slot < EQUIPMENT_SLOTS )
		return equipment[slot] ;
	else
	{
		log->write( Logger::ERROR, __FILE__, __LINE__, "Actor::getEquipment()",
		            "Invalid slot index passed to function." ) ;
		return NULL ;
	}
}

/*
 * Returns an inventory object for the actor's pack.  Since an Inventory object
 * can't make any changes to the pack itself, it's safe to use this as a public
 * function for UI purposes.
 */
Inventory * Actor::getInventory(void)
{
	return pack->toInventory() ;
}

/*
 * Returns a filtered inventory for the actor's pack.
 */
Inventory * Actor::getInventory( unsigned short filter )
{
	if( filter == K_EQUIPMENT )
	{
		Inventory *inv = new Inventory( this->countEquippedItems() ) ;
		unsigned short is = 0 ;
		for( unsigned short i = 0 ; i < EQUIPMENT_SLOTS ; i++ )
			if( equipment[i] != NULL )
				inv->setItem( is++, equipment[i] ) ;
		return inv ;
	}
	else return pack->toInventory(filter) ;
}

// DEBUG: Pack shouldn't be publicly-accessible in final version.
Pack * Actor::getPack(void) { return pack ; }

/*
 * Get actor's location on the current dungeon level.
 */
unsigned short Actor::getRow(void) { return this->row ; }
unsigned short Actor::getCol(void) { return this->col ; }

/*
 * Indicates whether a character may act in a given turn.
 */
bool Actor::canAct(void)
{
	if( hp <= 0 ) return false ;
	if( status_effects[ASLEEP] > 0 ) return false ;
	if( status_effects[PARALYZED] > 0 ) return false ;
	if( status_effects[STONED] > 0 ) return false ;
	return true ;
}

/*
 * Indicates whether the character is wearing the specified type of ring.
 */
bool Actor::hasRing( unsigned short rkind )
{
	return( ( equipment[EQ_LRING] != NULL
	       && equipment[EQ_LRING]->getKind() == rkind )
	     || ( equipment[EQ_RRING] != NULL
	       && equipment[EQ_RRING]->getKind() == rkind ) ) ;
}

/*
 * Indicates whether the character is wearing the specified type of armor.
 */
bool Actor::hasNecklace( unsigned short nkind )
{
	return( equipment[EQ_NECK] != NULL
	     && equipment[EQ_NECK]->getKind() == nkind ) ;
}

// /// MUTATORS (Actor) //////////////////////////////////////////////////// //

bool Actor::addItemToPack( Item *item )
{
	string func = "Actor::addItemToPack(Item*)" ; // debug

	log->write( Logger::INFO, __FILE__, __LINE__, func,
	            "Adding item to pack..." ) ;
	(void)item->getDiag() ;

	if( item->getCategory() == GOLD )
	{
		this->gold += item->getValue() ;
		log->write( Logger::TRACE, __FILE__, __LINE__, func,
		            "Added gold to actor's total." ) ;
		return true ;
	}
	else return pack->addItem( item, true ) ;
}

/*
 * Handles the act of eating a piece of food.  Returns the number of steps/units
 * by which the actor's hunger was sated.
 */
unsigned short Actor::eat( Item *food )
{
	string func = "Actor::eat(Item*)" ; // debug

	if( food == NULL )
	{
		log->write( Logger::WARNING, __FILE__, __LINE__, func,
		            "Received NULL reference to food item.  Returning." ) ;
		return 0 ;
	}

	log->write( Logger::INFO, __FILE__, __LINE__, func, "Eating food." ) ;

	if( pack->hasItem(food) )
	{ // Use up the pack's item, and if it's spent, remove it.
		if( food->use(1) == 0 ) pack->popItem(food) ;
	}
	else food->use(1) ;

	unsigned short snickers = 0 ;

	if( food->getKind() == FRUIT )
	{
		if( randPercent(FRUIT_XP_CHANCE) )
		{ // Don't restore much hunger, but grant the player experience.
			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Eating fruit of knowledge..." ) ;

			snickers =
				( STARTING_HUNGER / 8 )
				+ randShort( 0, ( STARTING_HUNGER / 16 ) )
				;

			unsigned int xpgain = randInt( 1, 
				( xpLevels[xplevel] - xpLevels[xplevel-1] ) / 8 ) ;

			if( this->gainXP(xpgain) ) this->levelUp() ;
		}
		else
		{
			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Eating normal fruit..." ) ;
			snickers = (STARTING_HUNGER/4) + randShort( 0, STARTING_HUNGER/8 ) ;
		}
	}
	else
	{
		log->write( Logger::TRACE, __FILE__, __LINE__, func,
		            "Eating ration of food." ) ;

		snickers =
			( STARTING_HUNGER - (Actor::HUNGER) )
			+ randShort( 0, STARTING_HUNGER/8 )
			;
	}

	this->hunger += snickers ;

	log->lbuf << "Restored " << snickers << " points of hunger. ("
	          << this->hunger << ")"
	          ;
	log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;

	return snickers ;
}

/*
 * Handles the act of equipping a suit of armor, and adding the previously-worn
 * armor, if any, to the actor's pack.
 */
Item * Actor::equipArmor( Item *na )
{
	string func = "Actor::equipArmor(Item*)" ; // debug

	log->write( Logger::INFO, __FILE__, __LINE__, func,
	            "Equipping armor..." ) ;

	Item *oa = equipment[EQ_ARMOR] ; // Hold old armor in limbo.

	if( na != NULL )
	{
		na->identify() ; // Armor self-identifies when equipped.
		if( pack->hasItem(na) )
			pack->popItem(na) ; // If it's from pack, remove it.
	}

	equipment[EQ_ARMOR] = na ; // Equip new armor.

	pack->addItem(oa,false) ; // Put old armor back in the pack.

	return oa ;
}

/*
 * Handles the act of equipping a melee weapon, and adding the previously-
 * wielded weapon, if any, to the actor's pack.
 */
Item * Actor::equipMeleeWeapon( Item *nmw )
{
	string func = "Actor::equipMeleeWeapon(Item*)" ; // debug

	log->write( Logger::INFO, __FILE__, __LINE__, func,
	            "Equipping melee weapon..." ) ;

	Item *omw = equipment[EQ_MWEAPON] ; // Hold old weapon in limbo.

	if( nmw != NULL )
	{
		nmw->identify() ; // Weapons self-identify when equipped.
		if( pack->hasItem(nmw) )
			pack->popItem(nmw) ; // If it's from pack, remove it.
	}

	equipment[EQ_MWEAPON] = nmw ; // Equip new weapon.

	pack->addItem(omw,false) ; // Put old weapon back in the pack.

	return omw ;
}

/*
 * Handles the act of equipping a ranged weapon, and adding the previously-
 * used ranged weapon, if any, to the actor's pack.
 *
 * Throws integer exceptions:
 *  -1 = Couldn't drop incompatible ammo into actor's pack.
 */
Item * Actor::equipRangedWeapon( Item *nrw )
{
	string func = "Actor::equipRangedWeapon(Item*)" ; // debug

	log->write( Logger::INFO, __FILE__, __LINE__, func,
	            "Equipping ranged weapon..." ) ;

	Item *orw = equipment[EQ_RWEAPON] ; // Hold old weapon in limbo.

	if( nrw != NULL )
	{
		if( pack->hasItem(nrw) )
			pack->popItem(nrw) ; // If it's from pack, remove it.
	}

	pack->addItem(orw,false) ; // Put old weapon in the pack.

	// Before finalizing equipment, the function must ensure that the type of
	// ammunition already equipped, if any, is compatible with the new ranged
	// weapon, or, if not, it must ensure that the incompatible ammunition can
	// be returned to the actor's pack.  If these conditions fail, then the
	// equip action will fail, and the function will throw an error code.
	if( equipment[EQ_AMMO] != NULL )
	{
		if( nrw == NULL 
		 || ( nrw->getKind() == RANGED_WEAPON + CROSSBOW
		   && equipment[EQ_AMMO]->getKind() != AMMUNITION + BOLT )
		 || ( ( nrw->getKind() == RANGED_WEAPON + SHORTBOW
		     || nrw->getKind() == RANGED_WEAPON + LONGBOW )
		   && equipment[EQ_AMMO]->getKind() != AMMUNITION + ARROW )
		  )
		{ // Equipped ammunition must be put back into the pack.
			if( pack->addItem(equipment[EQ_AMMO],true) )
			{ // Item was successfully added to pack, and must be unequipped.
				equipment[EQ_AMMO] = NULL ;
				log->write( Logger::TRACE, __FILE__, __LINE__, func,
				            "Transferred equipped ammo to pack." ) ;
			}
			else
			{ // Adding the ammo back to the pack failed, and we must cancel.
				log->write( Logger::WARNING, __FILE__, __LINE__, func,
				            "Couldn't put away incompatible ammo." ) ;
				pack->popItem(orw) ; // Grab the old weapon back out.
				pack->addItem(nrw,false) ; // Put the new weapon back in.
				throw(-1) ; // Throw an integer exception back to the caller.
			}
		}
	}

	if( nrw != NULL )
		nrw->identify() ; // Weapons self-identify when equipped.

	equipment[EQ_RWEAPON] = nrw ;

	return orw ;
}

/*
 * Handles the act of equipping ammunition for a ranged weapon, and adding
 * the previously-equipped ammunition, if any, to the actor's pack.
 *
 * Throws integer exceptions:
 *  -1 = Couldn't put old ammo into pack.
 *  -2 = Can't equip ammo without equipped ranged weapon.
 *  -3 = Can't equip incompatible ammo.
 */
Item * Actor::equipAmmunition( Item *na )
{
	string func = "Actor::equipAmmunition(Item*)" ; // debug

	log->write( Logger::INFO, __FILE__, __LINE__, func,
	            "Equipping ammunition..." ) ;

	// Before attempting to equip this ammunition, verify that it is compatible
	// with the equipped ranged weapon.  If not, throw an exception.
	if( equipment[EQ_RWEAPON] == NULL )
	{
		log->write( Logger::WARNING, __FILE__, __LINE__, func,
		            "Can't equip ammo without equipped ranged weapon." ) ;
		throw(-2) ; // Throw an integer exception back to the caller.
	}
	else if( na != NULL
	      && ( ( (equipment[EQ_RWEAPON])->getKind() == RANGED_WEAPON + CROSSBOW
	          && na->getKind() != AMMUNITION + BOLT )
	        || ( ( (equipment[EQ_RWEAPON])->getKind() == RANGED_WEAPON + SHORTBOW 
	            || (equipment[EQ_RWEAPON])->getKind() == RANGED_WEAPON + LONGBOW )
	          && na->getKind() != AMMUNITION + ARROW )
	         )
	       )
	{
		log->write( Logger::WARNING, __FILE__, __LINE__, func,
		            "Can't equip incompatible ammo." ) ;
		throw(-3) ; // Throw an integer exception back to the caller.
	}

	Item *oa = equipment[EQ_AMMO] ; // Hold old ammo in limbo.

	if( na != NULL )
	{
		if( pack->hasItem(na) )
			pack->popItem(na) ; // If it's from pack. remove it.
	}

	// Try to add the old ammo, if any, back to the pack.
	if( equipment[EQ_AMMO] != NULL )
	{
		if( pack->addItem( equipment[EQ_AMMO], true ) )
		{
			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Old ammo returned to actor's pack." ) ;
			equipment[EQ_AMMO] = na ;
		}
		else
		{
			log->write( Logger::WARNING, __FILE__, __LINE__, func,
			            "Couldn't put old ammo into pack." ) ;
			throw(-1) ;
		}
	}
	else
	{
		log->write( Logger::TRACE, __FILE__, __LINE__, func,
		            "Equipped new ammo." ) ;
		equipment[EQ_AMMO] = na ;
	}

	return oa ;
}

/*
 * Handles the act of equipping a ring.  Boolean parameter indicates whether
 * the ring is being placed on the right hand; if false, it's on the left hand.
 */
Item * Actor::equipRing( Item *newring, bool rh )
{
	string func = "Actor::equipRing(Item*,bool)" ; // debug

	log->write( Logger::INFO, __FILE__, __LINE__, func,
	            "Equipping ring..." ) ;

	Item *oldring ;
	if( rh ) oldring = equipment[EQ_RRING] ;
	else oldring = equipment[EQ_LRING] ; 

	if( newring != NULL )
	{ // If it's from pack, remove it.
		if( pack->hasItem(newring) ) pack->popItem(newring) ;
	}

	// Equip new ring.
	if( rh ) equipment[EQ_RRING] = newring ;
	else equipment[EQ_LRING] = newring ;

	pack->addItem(oldring,false) ; // Put old ring back in the pack.

	return oldring ;
}

/*
 * Handles the act of equipping a necklace, and adding the previously-worn
 * necklace, if any, to the actor's pack.
 */
Item * Actor::equipNecklace( Item *nn )
{
	string func = "Actor::equipNecklace(Item*)" ; // debug

	log->write( Logger::INFO, __FILE__, __LINE__, func,
	            "Equipping necklace..." ) ;

	Item *on = equipment[EQ_NECK] ; // Hold old necklace in limbo.

	if( nn != NULL )
	{
		if( pack->hasItem(nn) )
			pack->popItem(nn) ; // If it's from pack, remove it.
	}

	equipment[EQ_NECK] = nn ; // Equip new armor.

	pack->addItem(on,false) ; // Put old necklace back in the pack.

	return on ;
}

/*
 * Handles the situation where a actor gains experience points.  This function
 * DOES NOT level-up the player's statistics; that will be done in the series of
 * UI elements presented by levelUp(), as there are user choices involved.
 */
bool Actor::gainXP( unsigned int xpgain )
{
	string func = "Actor::gainXP(uint)" ; // debug
	log->lbuf << "Gaining " << xpgain << " XP." ;
	log->writeBuffer( Logger::INFO, __FILE__, __LINE__, func ) ;

	this->xp += xpgain ;

	if( this->xp > xpLevels[this->xplevel] )
	{
		log->lbuf << "New total " << this->xp
		          << " exceeds threshhold " << xpLevels[xplevel]
		          << " for level " << this->xplevel + 1 << "."
		          ;
		log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
		return true ;
	}
	else return false ;
}

/*
 * Heal specified HP damage if necessary.  If the character's HP is already
 * equal to or greater than his max HP, then nothing happens.
 */
bool Actor::heal( unsigned short x )
{
	if( hp >= getMaxHP() ) return false ;

	hp += x ;
	if( hp > getMaxHP() ) hp = getMaxHP() ;

	return true ;
}

/*
 * Inflicts the specified status effect on the character.  The save parameter
 * is the target number for the saving throw to shake off the effect.  If this
 * parameter is set to 0, no saving throw is allowed.
 */
bool Actor::inflict( StatusEffect effect,
                     unsigned short duration,
                     unsigned short save )
{
	string func = "Actor::inflict(StatusEffect,duration)" ; // debug

	log->lbuf << "Inflicting \"" << statusEffectNames[effect]
		<< "\" status on " << this->name
		<< " for " << duration << " turns (save target " << save << ")."
		;
	log->writeBuffer( Logger::INFO, __FILE__, __LINE__, func ) ;

	// Check for immunities granted by equipped items.
	if( effect == POISONED && this->hasRing(R_OPOISON) )
	{
		log->lbuf << name << "'s ring grants immunity to poison." ;
		log->writeBuffer( Logger::INFO, __FILE__, __LINE__, func ) ;
		return false ;
	}

	if( effect == STONED && this->hasRing(R_OSTONE) )
	{
		log->lbuf << name << "'s ring grants immunity to petrification." ;
		log->writeBuffer( Logger::INFO, __FILE__, __LINE__, func ) ;
		return false ;
	}

	unsigned short sroll = 0, smod = 0 ;

	switch( effect )
	{
		case SLOWED:
		case ASLEEP:
		case POISONED:
		case BLINDED:
		case PARALYZED:
		case STONED:
			// These are physical status effects, resisted by vitality.
			smod = this->getMVIT() ;
			break ;
		case CONFUSED:
		case HALLUCINATING:
		case SCARED:
			// These are mental status effects, resisted by willpower.
			smod = this->getMWIL() ;
			break ;
		case HASTED:
		case FLOATING:
		case SEES_CREATURES:
		case SEES_STUFF:
			// These effects are beneficial; no save will be allowed.
		default: ;
	}
	
	sroll = roll( 1, 20, smod ) ;

	if( sroll > save || ( sroll - smod ) == 20 )
	{ // When no save is allowed, this will always be 0 > 0.
		log->lbuf << this->name << " saves! (" << sroll << ")" ;
		log->writeBuffer( Logger::INFO, __FILE__, __LINE__, func ) ;
		return false ;
	}

	// Having reached this point, the saving throw has failed; add the effect.

	this->status_effects[effect] += duration ;

	return true ;
}

/*
 * Handles the act of quaffing a potion.  Return value indicates whether the
 * potion actually takes effect on the player (i.e. player fails the saving
 * throw, if any.)
 */
bool Actor::quaff( Item *potion )
{
	string func = "Actor::quaff(Item*)" ; // debug

	if( potion == NULL )
	{
		log->write( Logger::WARNING, __FILE__, __LINE__, func,
		            "Received NULL reference to potion.  Returning." ) ;
		return false ;
	}

	log->write( Logger::INFO, __FILE__, __LINE__, func, "Quaffing potion." ) ;

	if( pack->hasItem(potion) )
	{ // Use up the pack's item, and if it's spent, remove it.
		if( potion->use(1) == 0 ) pack->popItem(potion) ;
	}
	else potion->use(1) ;

	bool effect = false ;

	switch(potion->getKind())
	{
		case P_STAT_BOOST:
			if( potion->hasRoll() ) // Add random number of stat points.
				this->manageBasics( potion->dmgroll() ) ;
			else
				this->manageBasics(1) ;
			effect = true ;
			break ;
		case P_LSR_RESTO:
			if( this->xplevel_d != 0 )
				this->xplevel_d = 0 ;
			else if( this->maxhp_d != 0 )
				this->maxhp_d = 0 ;
			else if( this->str_d != 0 )
				this->str_d = 0 ;
			else if( this->dex_d != 0 )
				this->dex_d = 0 ;
			else if( this->vit_d != 0 )
				this->vit_d = 0 ;
			else if( this->wil_d != 0 )
				this->wil_d = 0 ;
			for( unsigned short e = CONFUSED ; e <= SCARED ; e++ )
				this->status_effects[e] = 0 ;
			effect = true ;
			break ;
		case P_GRT_RESTO:
			this->xplevel_d = 0 ;
			this->maxhp_d = 0 ;
			this->str_d = 0 ;
			this->dex_d = 0 ;
			this->vit_d = 0 ;
			this->wil_d = 0 ;
			for( unsigned short e = CONFUSED ; e <= SCARED ; e++ )
				this->status_effects[e] = 0 ;
			effect = true ;
			break ;
		case P_HEALING:
			if( this->hp >= this->getMaxHP() ) effect = false ;
			else
			{
				if( potion->getCritMultiplier() > 0 )
					for( unsigned int r = 0 ; r < this->xplevel ; r++ )
						this->heal( potion->dmgroll() ) ;
				else this->heal( potion->dmgroll() ) ;
				effect = true ;
			}
			break ;
		case P_GRT_HEALING:
			if( potion->getCritMultiplier() > 0 )
				for( unsigned int r = 0 ; r < this->xplevel ; r++ )
					this->hp += potion->dmgroll() ;
			else this->hp += potion->dmgroll() ;
			effect = true ;
			break ;
		case P_LEVELUP:
			this->xp += xpLevels[this->xplevel] - xpLevels[this->xplevel-1] ;
			this->levelUp() ;
			effect = true ;
			break ;
		case P_SEE_MONS:
			this->status_effects[SEES_CREATURES] = 1 ;
			effect = true ;
			break ;
		case P_SEE_STUFF:
			this->status_effects[SEES_STUFF] = 1 ;
			effect = true ;
			break ;
		case P_LEVITATE:
			if( potion->getCritMultiplier() > 0 )
				for( unsigned int r = 0 ; r < this->xplevel ; r++ )
					this->status_effects[FLOATING] += potion->dmgroll() ;
			else this->status_effects[FLOATING] += potion->dmgroll() ;
			effect = true ;
			break ;
		case P_HASTE:
			if( potion->getCritMultiplier() > 0 )
				for( unsigned int r = 0 ; r < this->xplevel ; r++ )
					this->status_effects[HASTED] += potion->dmgroll() ;
			else this->status_effects[HASTED] += potion->dmgroll() ;
			this->status_effects[SLOWED] = 0 ;
			effect = true ;
			break ;
		case P_TRUESIGHT:
			this->status_effects[BLINDED] = 0 ;
			this->status_effects[HALLUCINATING] = 0 ;
			this->status_effects[SCARED] = 0 ;
			this->status_effects[SEES_CREATURES] = 1 ;
			this->status_effects[SEES_STUFF] = 1 ;
			effect = true ;
			break ;
		case P_SLEEP:
			effect = this->inflict( ASLEEP, potion->dmgroll(), POTION_SAVE ) ;
			break ;
		case P_CONFUSE:
			effect = this->inflict( CONFUSED, potion->dmgroll(), POTION_SAVE ) ;
			break ;
		case P_POISON:
			effect = this->inflict( POISONED, potion->dmgroll(), POTION_SAVE ) ;
			break ;
		case P_BLIND:
			effect = this->inflict( BLINDED, potion->dmgroll(), POTION_SAVE ) ;
			break ;
		case P_LSD:
			effect =
				this->inflict( HALLUCINATING, potion->dmgroll(), POTION_SAVE ) ;
			break ;
		default: ;
	}

	potion->identify() ;

	if( potion->getQuantity() == 0 )
	{
		log->write( Logger::TRACE, __FILE__, __LINE__, func,
			"Deleting spent potion stack." ) ;
		delete potion ;
	}

	return effect ;
}

/*
 * Removes an item from the actor's pack.
 */
Item * Actor::removeItemFromPack( Item *item )
{
	string func = "Actor::removeItemFromPack(Item*)" ; // debug
	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Removing item from pack:" ) ;
	item->getDiag() ;

	return pack->popItem(item) ;
}

/* Sets gender. */
bool Actor::setGender( bool y ) { this->hasY = y ; return y ; }

/*
 * Sets gender based on character input.  'M' or 'Y' will set to male; 'F' or
 * 'X' will set to female.
 */
bool Actor::setGender( char g )
{
	if( cins( g, "fFxX" ) ) this->hasY = false ;
	else if( cins( g, "mMyY" ) ) this->hasY = true ;
	else
	{ // Invalid input.
		log->lbuf << "Received invalid input '" << g << "'." ;
		log->writeBuffer( Logger::WARNING, __FILE__, __LINE__,
		                  "Actor::setGender(char)" ) ;
		return true ;
	}
}

/* Sets the character's name. */
void Actor::setName( string n ) { this->name = n ; return ; }

/*
 * Set the actor's location in the current level.  It's up to the caller to
 * ensure that the given location is valid.
 */
void Actor::setRow( unsigned short r ) { this->row = r ; return ; }
void Actor::setCol( unsigned short c ) { this->col = c ; return ; }

void Actor::setLocation( unsigned short r, unsigned short c )
{
	string func = "Actor::setLocation(ushort,ushort)" ; // debug

	this->row = r ;
	this->col = c ;
	log->lbuf << "Moved to (" << r << "," << c << ")" ;
	log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
	return ;
}

/*
 * Advances time by 1 turn.  The return value indicates that any change in the
 * player's status effects has occurred.
 */
short Actor::tick(void)
{
	string func = "Actor::tick()" ; // debug

	// Advance hunger.
	if( this->hasRing(R_SUSTENANCE) )
		log->lbuf << name << " doesn't need food." ;
	else
	{
		--hunger ;
		log->lbuf << "Hunger: " << hunger << " steps remaining." ;
	}
	log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;

	// Heal wounds slightly.
	if( equipment[EQ_LRING] != NULL
	   && equipment[EQ_LRING]->isItem( RING, R_REGENERATION ) )
	{
		heal(1) ;
		log->lbuf << name << " regenerated 1 hp." ;
		log->writeBuffer( Logger::INFO, __FILE__, __LINE__, func ) ;
	}

	if( equipment[EQ_RRING] != NULL
	   && equipment[EQ_RRING]->isItem( RING, R_REGENERATION ) )
	{
		heal(1) ;
		log->lbuf << name << " regenerated 1 hp." ;
		log->writeBuffer( Logger::INFO, __FILE__, __LINE__, func ) ;
	}

	if( --stepsToHeal == 0 )
	{
		resetStepsToHeal() ;
		heal(1) ;
		log->lbuf << name << " naturally healed 1 hp." ;
		log->writeBuffer( Logger::INFO, __FILE__, __LINE__, func ) ;
	}

	// Process poison effect, which decreases HP.
	if( status_effects[POISONED] > 0 ) --hp ;
	
	// Decrement status effects.
	short sec = -1 ;

	for( unsigned int e = 0 ; e < STATUS_EFFECTS ; e++ )
	{
		if( status_effects[e] > 0 )
		{
			if( e == SEES_CREATURES || e == SEES_STUFF )
			{
				log->lbuf << name << " is " << statusEffectNames[e] << "." ;
				log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
				continue ;
			}

			--(status_effects[e]) ;
			log->lbuf << name << " is " << statusEffectNames[e] << " ("
				<< status_effects[e]
				<< ( status_effects[e] == 1 ? " more step)" : " more steps)" )
				;
			log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
			if( status_effects[e] == 0 )
			{
				log->lbuf << name << " is no longer "
					 << statusEffectNames[e] << "."
					 ;
				log->writeBuffer( Logger::INFO, __FILE__, __LINE__, func ) ;
				sec = e ;
			}
		}
	}

	log->write( Logger::TRACE, __FILE__, __LINE__, func, "Tick complete." ) ;
	
	return sec ;
}

// /// OTHER PUBLIC FUNCTIONS (Actor) ////////////////////////////////////// //
// Virtual functions required by subclasses:
// void Actor::levelUp()
// void Actor::manageBasics( unsigned short points ) ;

/*
 * Draws the actor's representative character onscreen.
 */
void Actor::draw(void)
{
	mvaddch( row, col, displaychar ) ;
	if( has_colors() ) mvchgat( row, col, 1, A_BOLD, displaycolor, NULL ) ;
	return ;
}

// /// PROTECTED FUNCTIONS (Actor) ///////////////////////////////////////// //

/* Counts number of items equipped. */
unsigned short Actor::countEquippedItems()
{
	string func = "Actor::countEquippedItems()" ; // debug

	unsigned short c = 0 ;

	for( unsigned short i = 0 ; i < EQUIPMENT_SLOTS ; i++ )
		if( equipment[i] != NULL ) ++c ;

	log->lbuf << "Character has " << c
	          << ( c == 1 ? " item" : " items" )
	          << " equipped."
	          ;
	log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;

	return c ;
}

/* Returns true if no items are equipped. */
bool Actor::isNaked(void)
{
	for( unsigned short i = 0 ; i < EQUIPMENT_SLOTS ; i++ )
		if( equipment[i] != NULL ) return false ;

	return true ;

//	return ( m_weapon == NULL && r_weapon == NULL && ammunition == NULL
//	      && armor == NULL && lring == NULL && rring == NULL
//	      && neck == NULL )
//	       ;
}

/*
 * Resets the number of steps to go until the character spontaneously heals 1
 * HP.
 */
unsigned short Actor::resetStepsToHeal(void)
{
	string func = "Actor::resetStepsToHeal()" ; // debug

	// Basic algorithm: The actor should heal 1% of his HP every 5 steps,
	// thus healing 100% HP after 500 steps.

	stepsToHeal = 500 / getMaxHP() ;

	log->lbuf << name << " will heal another 1 hp in "
		 << stepsToHeal << ( stepsToHeal == 1 ? " step." : " steps." )
		;
	log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;

	return stepsToHeal ;
}

/* ************************************************************************* */
