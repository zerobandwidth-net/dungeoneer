/*
 * rogue.cpp
 * You may play with this source however you like, as long as you don't remove
 * this comment, claim undue credit, or make a profit on it.  Enjoy.
 * CHANGE HISTORY
 * 2011-11-17
 *  Created file.
 */

#ifndef _DUNGEONEER_H_
#include "dungeoneer.h"
#endif

#include <sstream>
#include <string>
#include "command.h"
#include "debug.h"
#include "pack.h"
#include "rogue.h"
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
	"Ungawa", "Victor", "Wollongong", "Xander", "Yohaa", "Zerogue"
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

/* *** CLASS DEFINITION (Rogue) ******************************************** */

// /// STATIC FUNCTIONS (classname) ///////////////////////////// (none) /// //

// /// CONSTRUCTORS (Rogue) //////////////////////////////////////////////// //

Rogue::Rogue(void)
{
	log->write( Logger::INFO, __FILE__, __LINE__, "Rogue()",
	            "Constructing Rogue object..." ) ;

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

	displaychar = V_PLAYER ;
	name = "" ;
	hasY = true ;
}

// /// DESTRUCTORS (Rogue) ///////////////////////////////////////////////// //

Rogue::~Rogue()
{
	delete[] status_effects ;
	delete[] equipment ;
	delete pack ;
}

// /// ACCESSORS (Rogue) /////////////////////////////////////////////////// //

string Rogue::getName(void) { return this->name ; }

bool Rogue::getGender(void) { return this->hasY ; }

char Rogue::getGenderChar(void) { return ( this->hasY ? 'M' : 'F' ) ; }

short Rogue::getMaxHP( void )
{
	if( maxhp_d >= ( maxhp + ( xplevel * this->getMVIT() ) ) )
		return 1 ;
	else
		return( maxhp + ( xplevel * this->getMVIT() ) - maxhp_d ) ;
}

short Rogue::getHP(void) { return hp ; }

unsigned short Rogue::getSTR( void )
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

unsigned short Rogue::getDEX( void )
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

unsigned short Rogue::getVIT( void )
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

unsigned short Rogue::getWIL( void )
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

short Rogue::getMSTR( void )
{
	return( ( short(getSTR()) / 2 ) - 5 ) ;
//	short base = short(getSTR()) - 10 ;
//	if( base >= 0 ) return( base / 2 ) ;
//	else return( (base/2) - 1 ) ;
}

short Rogue::getMDEX( void )
{
	return( ( short(getDEX()) / 2 ) - 5 ) ;
//	short base = short(getDEX()) - 10 ;
//	if( base >= 0 ) return( base / 2 ) ;
//	else return( (base/2) - 1 ) ;
}

short Rogue::getMVIT( void )
{
	return( ( short(getVIT()) / 2 ) - 5 ) ;
//	short base = short(getVIT()) - 10 ;
//	if( base >= 0 ) return( base / 2 ) ;
//	else return( (base/2) - 1 ) ;
}

short Rogue::getMWIL( void )
{
	return( ( short(getWIL()) / 2 ) - 5 ) ;
//	short base = short(getWIL()) - 10 ;
//	if( base >= 0 ) return( base / 2 ) ;
//	else return( (base/2) - 1 ) ;
}

bool Rogue::getStatus( StatusEffect stype )
{
	if( status_effects[stype] > 0 ) return true ;
	else return false ;
}

unsigned short Rogue::getLevel( void )
{
	if( xplevel_d >= xplevel ) return 1 ;
	else return( xplevel - xplevel_d ) ;
}

unsigned short Rogue::getActualLevel(void)
{
	return xplevel ;
}

unsigned int Rogue::getXP(void) { return xp ; }

unsigned short Rogue::getXPPercent(void)
{
	return (unsigned short)
		( ( this->xp - xpLevels[(this->xplevel)-1 ] ) * 100 )
		  / ( xpLevels[this->xplevel] - xpLevels[(this->xplevel)-1] ) ;
}

unsigned short Rogue::getHunger(void) { return hunger ; }

unsigned short Rogue::getHungerState( void )
{
	if( hunger <= Rogue::STARVED )       return Rogue::STARVING ;
	else if( hunger <= Rogue::FAINTING ) return Rogue::FAINT ;
	else if( hunger <= Rogue::WEAKNESS ) return Rogue::WEAK ;
	else if( hunger <= Rogue::HUNGER )   return Rogue::HUNGRY ;
	else return 0 ;
} 

unsigned int Rogue::getGold(void) { return gold ; }

/*
 * Returns a count of items equipped by the rogue, by counting the number of
 * equipped item pointers that aren't NULL.
 */
unsigned short Rogue::getEquippedItemCount(void)
{
	unsigned short c = 0 ;

	for( int i = 0 ; i < EQUIPMENT_SLOTS ; i++ )
		if( equipment[i] != NULL ) ++c ;

	return c ;
}

/*
 * Gets the equipped item at the specified equipment slot.
 */
Item * Rogue::getEquipment( EquipmentSlot slot )
{
	if( slot < EQUIPMENT_SLOTS )
		return equipment[slot] ;
	else
	{
		log->write( Logger::ERROR, __FILE__, __LINE__, "Rogue::getEquipment()",
		            "Invalid slot index passed to function." ) ;
		return NULL ;
	}
}

/*
 * Returns an inventory object for the rogue's pack.  Since an Inventory object
 * can't make any changes to the pack itself, it's safe to use this as a public
 * function for UI purposes.
 */
Inventory * Rogue::getInventory(void)
{
	return pack->toInventory() ;
}

/*
 * Returns a filtered inventory for the rogue's pack.
 */
Inventory * Rogue::getInventory( unsigned short filter )
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
Pack * Rogue::getPack(void) { return pack ; }

/*
 * Get rogue's location on the current dungeon level.
 */
unsigned short Rogue::getRow(void) { return this->row ; }
unsigned short Rogue::getCol(void) { return this->col ; }

/*
 * Indicates whether a character may act in a given turn.
 */
bool Rogue::canAct(void)
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
bool Rogue::hasRing( unsigned short rkind )
{
	return( ( equipment[EQ_LRING] != NULL
	       && equipment[EQ_LRING]->getKind() == rkind )
	     || ( equipment[EQ_RRING] != NULL
	       && equipment[EQ_RRING]->getKind() == rkind ) ) ;
}

/*
 * Indicates whether the character is wearing the specified type of armor.
 */
bool Rogue::hasNecklace( unsigned short nkind )
{
	return( equipment[EQ_NECK] != NULL
	     && equipment[EQ_NECK]->getKind() == nkind ) ;
}

// /// MUTATORS (Rogue) //////////////////////////////////////////////////// //

bool Rogue::addItemToPack( Item *item )
{
	string func = "Rogue::addItemToPack(Item*)" ; // debug

	log->write( Logger::INFO, __FILE__, __LINE__, func,
	            "Adding item to pack..." ) ;
	(void)item->getDiag() ;

	if( item->getCategory() == GOLD )
	{
		this->gold += item->getValue() ;
		log->write( Logger::TRACE, __FILE__, __LINE__, func,
		            "Added gold to rogue's total." ) ;
		return true ;
	}
	else return pack->addItem( item, true ) ;
}

/*
 * Handles the ace of eating a piece of food.  Returns the number of steps/units
 * by which the rogue's hunger was sated.
 */
unsigned short Rogue::eat( Item *food )
{
	string func = "Rogue::eat(Item*)" ; // debug

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
			( STARTING_HUNGER - (Rogue::HUNGER) )
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
 * armor, if any, to the rogue's pack.
 */
Item * Rogue::equipArmor( Item *na )
{
	string func = "Rogue::equipArmor(Item*)" ; // debug

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
 * wielded weapon, if any, to the rogue's pack.
 */
Item * Rogue::equipMeleeWeapon( Item *nmw )
{
	string func = "Rogue::equipMeleeWeapon(Item*)" ; // debug

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
 * used ranged weapon, if any, to the rogue's pack.
 *
 * Throws integer exceptions:
 *  -1 = Couldn't drop incompatible ammo into rogue's pack.
 */
Item * Rogue::equipRangedWeapon( Item *nrw )
{
	string func = "Rogue::equipRangedWeapon(Item*)" ; // debug

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
	// be returned to the rogue's pack.  If these conditions fail, then the
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
 * the previously-equipped ammunition, if any, to the rogue's pack.
 *
 * Throws integer exceptions:
 *  -1 = Couldn't put old ammo into pack.
 *  -2 = Can't equip ammo without equipped ranged weapon.
 *  -3 = Can't equip incompatible ammo.
 */
Item * Rogue::equipAmmunition( Item *na )
{
	string func = "Rogue::equipAmmunition(Item*)" ; // debug

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
			            "Old ammo returned to rogue's pack." ) ;
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
Item * Rogue::equipRing( Item *newring, bool rh )
{
	string func = "Rogue::equipRing(Item*,bool)" ; // debug

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
 * necklace, if any, to the rogue's pack.
 */
Item * Rogue::equipNecklace( Item *nn )
{
	string func = "Rogue::equipNecklace(Item*)" ; // debug

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
 * Handles the situation where a rogue gains experience points.  This function
 * DOES NOT level-up the player's statistics; that will be done in the series of
 * UI elements presented by levelUp(), as there are user choices involved.
 */
bool Rogue::gainXP( unsigned int xpgain )
{
	string func = "Rogue::gainXP(uint)" ; // debug
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
bool Rogue::heal( unsigned short x )
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
bool Rogue::inflict( StatusEffect effect,
                     unsigned short duration,
                     unsigned short save )
{
	string func = "Rogue::inflict(StatusEffect,duration)" ; // debug

	log->lbuf << "Inflicting status " << statusEffectNames[effect]
		<< " on " << this->name
		<< " for " << duration << " turns; save target " << save << "."
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
bool Rogue::quaff( Item *potion )
{
	string func = "Rogue::quaff(Item*)" ; // debug

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
 * Removes an item from the rogue's pack.
 */
Item * Rogue::removeItemFromPack( Item *item )
{
	string func = "Rogue::removeItemFromPack(Item*)" ; // debug
	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Removing item from pack:" ) ;
	item->getDiag() ;

	return pack->popItem(item) ;
}

/*
 * Rerolls the rogue's vital statistics.
 */
void Rogue::reroll( void )
{
	log->write( Logger::INFO, __FILE__, __LINE__, "Rogue::reroll()",
	            "Rerolling character statistics..." ) ;

	xplevel = 1 ; xplevel_d = 0 ; xp = 0 ;

	str = Rogue::rerollStat() ;
	dex = Rogue::rerollStat() ;
	vit = Rogue::rerollStat() ;
	wil = Rogue::rerollStat() ;

	str_d = dex_d = vit_d = wil_d = 0 ;

	for( int i = 0 ; i < STATUS_EFFECTS ; i++ )
		status_effects[i] = 0 ;

	hunger = STARTING_HUNGER ;
	this->resetStepsToHeal() ;

	maxhp = ROGUE_HIT_DIE ;
	maxhp_d = 0 ;
	hp = this->getMaxHP() ;
}

/* Sets gender. */
bool Rogue::setGender( bool y ) { this->hasY = y ; return y ; }

/*
 * Sets gender based on character input.  'M' or 'Y' will set to male; 'F' or
 * 'X' will set to female.
 */
bool Rogue::setGender( char g )
{
	if( cins( g, "fFxX" ) ) this->hasY = false ;
	else if( cins( g, "mMyY" ) ) this->hasY = true ;
	else
	{ // Invalid input.
		log->lbuf << "Received invalid input '" << g << "'." ;
		log->writeBuffer( Logger::WARNING, __FILE__, __LINE__,
		                  "Rogue::setGender(char)" ) ;
		return true ;
	}
}

/* Sets the character's name. */
void Rogue::setName( string n ) { this->name = n ; return ; }

/*
 * Set the rogue's location in the current level.  It's up to the caller to
 * ensure that the given location is valid.
 */
void Rogue::setRow( unsigned short r ) { this->row = r ; return ; }
void Rogue::setCol( unsigned short c ) { this->col = c ; return ; }

void Rogue::setLocation( unsigned short r, unsigned short c )
{
	string func = "Rogue::setLocation(ushort,ushort)" ; // debug

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
short Rogue::tick(void)
{
	string func = "Rogue::tick()" ; // debug

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

// /// OTHER PUBLIC FUNCTIONS (Rogue) ////////////////////////////////////// //

/*
 * Initiates display of total inventory.  The UI begins with a menu allowing
 * the player to choose an item filter.  Once a filter is selected, the
 * filtered version of the function is called.
 */
Item * Rogue::displayInventory(void)
{
	return this->displayInventory( "Inventory" ) ;
}

Item * Rogue::displayInventory( string h )
{
	string func = "Rogue::displayInventory()" ; // debug

	log->write( Logger::INFO, __FILE__, __LINE__, func,
	            "Invoking inventory display for pack..." ) ;

	stringstream buf (stringstream::out) ;

	if( isNaked() && ( pack->getCount() == 0 ) ) // Nothing to display.
	{
		log->write( Logger::TRACE, __FILE__, __LINE__, func,
		            "Not carrying anything." ) ;

		buf << this->name << " isn't carrying anything." ;
		displayDialog( buf.str() ) ;
		return NULL ;
	}
	else // Display inventory.
	{
		Item *chosen = NULL ;

		do
		{
			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Initializing filter selection screen..." ) ;

			Window *wTypeSelect = new Window( 13, 25 ) ;
			wTypeSelect->setHeader( "Select category:" ) ;
			buf << gold << " " << COIN << ( gold > 1 ? "s" : "" ) ;
			wTypeSelect->mvwstr( 1,  1, buf.str() ) ; buf.str("") ;
			wTypeSelect->mvwstr( 2,  1, " *  all items" ) ;
			wTypeSelect->mvwstr( 3,  1, " E  equipped items" ) ;
			buf << " " << (char)K_FOOD << "  food" ;
			wTypeSelect->mvwstr( 4,  1, buf.str() ) ; buf.str("") ;
			buf << " " << (char)K_ARMOR << "  armor" ;
			wTypeSelect->mvwstr( 5,  1, buf.str() ) ; buf.str("") ;
			buf << " " << (char)K_WEAPON << "  weapons" ;
			wTypeSelect->mvwstr( 6,  1, buf.str() ) ; buf.str("") ;
			buf << " " << (char)K_POTION << "  potions" ;
			wTypeSelect->mvwstr( 7,  1, buf.str() ) ; buf.str("") ;
			buf << " " << (char)K_SCROLL << "  scrolls" ;
			wTypeSelect->mvwstr( 8,  1, buf.str() ) ; buf.str("") ;
			buf << " " << (char)K_WAND << "  wands" ;
			wTypeSelect->mvwstr( 9,  1, buf.str() ) ; buf.str("") ;
			buf << " " << (char)K_RING << "  rings" ;
			wTypeSelect->mvwstr( 10, 1, buf.str() ) ; buf.str("") ;
			buf << " " << (char)K_NECKLACE << "  necklaces" ;
			wTypeSelect->mvwstr( 11, 1, buf.str() ) ; buf.str("") ;
			wTypeSelect->setFooter( "[Esc] Cancel" ) ;
			wTypeSelect->refresh() ;

			char c = getch() ;

			delete wTypeSelect ;
			refresh() ;

			if( c == K_CANCEL )
			{
				log->write( Logger::TRACE, __FILE__, __LINE__, func,
				            "Display cancelled." ) ;
				return NULL ;
			}
			else
			{
				log->lbuf << "Received character: " << c ;
				log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
			}

			chosen = displayInventory( c, h ) ;
		} while( chosen == NULL ) ;

		return chosen ;
	}
}

/*
 * Displays a filtered inventory of the rogue's pack.  If an item is selected,
 * it is returned.
 */
Item * Rogue::displayInventory( char filter )
{
	return this->displayInventory( filter, "Inventory" ) ;
}

Item * Rogue::displayInventory( char filter, string h )
{
	string func = "Rogue::displayInventory(char)" ;

	log->write( Logger::INFO, __FILE__, __LINE__, func,
	            "Displaying inventory..." ) ;

	unsigned short category = Item::charToCat(filter) ;

	if( category == GOLD ) category = K_ALLITEMS ; // The pain of sharing keys.

	unsigned short count =
		( filter == K_EQUIPMENT ?
		  this->countEquippedItems() :
		  pack->countItems(category) )
		;
	log->lbuf << "Filter " << filter << " translated to category " << category
	          << " with " << count << ( count == 1 ? " item" : " items" )
	          ;
	log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;

	// Catch case where rogue isn't carrying any items of this type.
	if( count == 0 )
	{
		log->write( Logger::TRACE, __FILE__, __LINE__, func,
		            "Nothing appropriate." ) ;

		displayDialog( "Nothing appropriate." ) ;
		return NULL ;
	}

	Inventory *inv ;

	if( category == K_EQUIPMENT )
	{
		log->write( Logger::TRACE, __FILE__, __LINE__, func,
		            "Constructing inventory of equipment..." ) ;

		inv = new Inventory(count) ;
		unsigned short is = 0 ;
		for( unsigned short i = 0 ; i < EQUIPMENT_SLOTS ; i++ )
			if( equipment[i] != NULL )
				inv->setItem( is++, equipment[i] ) ;
	}
	else if( category == K_ALLITEMS )
	{
		log->write( Logger::TRACE, __FILE__, __LINE__, func,
		            "Constructing inventory of entire pack..." ) ;

		inv = pack->toInventory() ;
	}
	else
	{
		log->write( Logger::TRACE, __FILE__, __LINE__, func,
		            "Constructing filtered inventory..." ) ;

		inv = pack->toInventory(category) ;
	}

	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Selecting item to be returned..." ) ;

	inv->setWindow() ;
	inv->setWindowHeader(h) ;

	Item *isel = inv->selectItem() ;

	delete inv ;
	refresh() ;

	return isel ;
}

/*
 * Displays a status screen.
 */
void Rogue::displayStatus(void)
{
	log->write( Logger::TRACE, __FILE__, __LINE__, "Rogue::displayStatus()",
	            "Displaying character status screen." ) ;

	refresh() ;
	
	Window *wStatus =
		new Window( ((DROWS/2)-(EQUIPMENT_SLOTS/2)-2),
	                0,
	                (EQUIPMENT_SLOTS+2), 
	                (DCOLS/2) )
		;
	Inventory *inv = this->getInventory(K_EQUIPMENT) ;
	stringstream buf ( stringstream::out ) ;

	buf << this->name << " (" << this->getGenderChar() << ")" ;
	wStatus->setHeader( buf.str() ) ; buf.str("") ;
	buf << "Level: " << xplevel ;
	if( xplevel_d != 0 )
		buf << " (" << this->getLevel() << ")" ;
	wStatus->mvwstr( 1, 2, buf.str() ) ; buf.str("") ;
	buf << "Experience: " << xp << " / " << xpLevels[this->xplevel]
	    << " (" << this->getXPPercent() << "%)"
	    ;
	wStatus->mvwstr( 2, 2, buf.str() ) ; buf.str("") ;
	buf << "STR " << pad( this->getSTR(), 2 ) << "/" << pad(str,2)
	    << " ["
	    << ( this->getMSTR() >= 0 ? "+" : "" )
	    << this->getMSTR() << "]"
	    ;
	wStatus->mvwstr( 4, 2, buf.str() ) ; buf.str("") ;
	buf << "DEX " << pad( this->getDEX(), 2 ) << "/" << pad(dex,2)
	    << " ["
	    << ( this->getMDEX() >= 0 ? "+" : "" )
	    << this->getMDEX() << "]"
	    ;
	wStatus->mvwstr( 5, 2, buf.str() ) ; buf.str("") ;
	buf << "VIT " << pad( this->getVIT(), 2 ) << "/" << pad(vit,2)
	    << " ["
	    << ( this->getMVIT() >= 0 ? "+" : "" )
	    << this->getMVIT() << "]"
	    ;
	wStatus->mvwstr( 6, 2, buf.str() ) ; buf.str("") ;
	buf << "WIL " << pad( this->getWIL(), 2 ) << "/" << pad(wil,2)
	    << " ["
	    << ( this->getMWIL() >= 0 ? "+" : "" )
	    << this->getMWIL() << "]"
	    ;
	wStatus->mvwstr( 7, 2, buf.str() ) ; buf.str("") ;

	if( inv != NULL ) { delete inv ; refresh() ; }
	inv = this->getInventory(K_EQUIPMENT) ;
	inv->setWindow( ((DROWS/2)-(EQUIPMENT_SLOTS/2)-2), (DCOLS/2),
	                (EQUIPMENT_SLOTS+2), (DCOLS/2) ) ;
	inv->setWindowHeader( "Currently equipped:" ) ;
	inv->setWindowPage(0) ;

	wStatus->redraw() ;
	wStatus->prepare() ;
	inv->prepare() ;

	doupdate() ;
	
	waitForInput() ;

	delete wStatus ;
	delete inv ;
	refresh() ;
	return ;
}

/*
 * Displays the level-up UI for a player character.
 */
void Rogue::levelUp(void)
{
	string func = "Rogue::levelUp()" ; // debug

	if( this->xp < xpLevels[xplevel] )
	{ // Bogus request; don't raise the character's level.
		log->lbuf << name << "'s XP total " << this->xp
		          << " does not exceed threshhold of " << xpLevels[xplevel]
		          << " for level " << xplevel << "."
		          ;
		log->writeBuffer( Logger::WARNING, __FILE__, __LINE__, func ) ;
		return ;
	}

	stringstream buf ( stringstream::out ) ; // throwaway output buffer
	unsigned short x = 0 ; // throwaway numeric value

	Window *wLevelUp = new Window( ((DROWS/2)-3), 5, 5, (DCOLS-10) ) ;
	
	buf << this->name << " has gained a level! (" << ++(this->xplevel) << ")" ;
	log->write( Logger::INFO, __FILE__, __LINE__, func, buf.str() ) ;
	wLevelUp->setHeader( buf.str() ) ; buf.str("") ;
	if( (this->xplevel) % 4 == 0 ) wLevelUp->setFooter( "..." ) ;

	// Add to max HP and "ding!" current HP to match if it was lower than max.
	x = roll( 1, ROGUE_HIT_DIE, 0 ) ;

	if( this->hp <= this->getMaxHP() )
	{
		this->maxhp += x ;
		this->hp = this->getMaxHP() ;
	}
	else
	{
		this->hp += x ;
		this->maxhp += x ;
	}

	buf << "Max HP rose by " << x + getMVIT()
		<< " points. (" << this->getMaxHP() << ")"
		;
	wLevelUp->mvwstr( 2, 2, buf.str() ) ; buf.str("") ;

	wLevelUp->refresh() ;

	waitForInput() ;

	delete wLevelUp ;
	refresh() ;

	if( xplevel % 4 == 0 ) manageBasics(1) ;

	return ;
}

/*
 * Displays UI for managing basic statistics: name, gender, and stat raises.
 * As this is assumed to be used during character creation, this function begins
 * by randomly assigning a gender, name, and basic statistics.  The player may
 * then change any or all of these attributes within the UI.
 */
void Rogue::manageBasics(void)
{
	string func = "Rogue::manageBasics()" ; // debug

	log->write( Logger::INFO, __FILE__, __LINE__, func,
	            "Managing character's basic statistics." ) ;

	stringstream buf ( stringstream::out ) ;

	// Assign initial random values for basic attributes.
	this->hasY = coin_toss() ;
	if( hasY ) this->name = randomMaleNames[randShort(0,25)] ;
	else this->name = randomFemaleNames[randShort(0,25)] ;
	this->reroll() ;

	// Construct initial window.
	Window *win = new Window( 13, 60 ) ;
	win->setHeader( "Create character:" ) ;
	win->setFooter( "[N]ame [G]ender [R]eroll | [Enter] to Accept" ) ;

	// Main command loop; execute until changes are committed.
	bool done = false ;
	do
	{
		win->clear() ;

		buf << "Name: " << this->name ;
		win->mvwstr( 2, 2, buf.str() ) ; buf.str("") ;

		buf << "Gender: " << this->getGenderChar() ;
		win->mvwstr( 3, 2, buf.str() ) ; buf.str("") ;

		buf << "Strength:  " << pad( this->str, 2 ) << " ["
		    << ( this->getMSTR() >= 0 ? "+" : "" )
		    << this->getMSTR() << "]"
			;
		win->mvwstr( 5, 2, buf.str() ) ; buf.str("") ;
		buf << "Dexterity: " << pad( this->dex, 2 ) << " ["
		    << ( this->getMDEX() >= 0 ? "+" : "" )
		    << this->getMDEX() << "]"
		    ;
		win->mvwstr( 6, 2, buf.str() ) ; buf.str("") ;
		buf << "Vitality:  " << pad( this->vit, 2 ) << " ["
		    << ( this->getMVIT() >= 0 ? "+" : "" )
		    << this->getMVIT() << "]"
		    ;
		win->mvwstr( 7, 2, buf.str() ) ; buf.str("") ;
		buf << "Willpower: " << pad( this->wil, 2 ) << " ["
		    << ( this->getMWIL() >= 0 ? "+" : "" )
		    << this->getMWIL() << "]"
		    ;
		win->mvwstr( 8, 2, buf.str() ) ; buf.str("") ;

		buf << "Hit Points: " << this->getMaxHP() ;
		win->mvwstr( 10, 2, buf.str() ) ; buf.str("") ;

		win->refresh() ;

		Command *c = new Command(false) ;
		char cc = c->getCharCode() ;
		delete c ;

		switch(cc)
		{
			case 10: // [Enter]
			case 27: // [Esc]
			case 'x': case 'X':
				done = true ;
				break ;
			case 'n': case 'N':
			{
				string nn = win->getTextEntry( 2, 8, 24 ) ;
				if( nn.compare("") ) // got a new entry
					this->name = nn ;
			}	break ;
			case 'g': case 'G':
				if( this->hasY ) this->hasY = false ;
				else this->hasY = true ;
				break ;
			case 'r': case 'R':
				this->reroll() ;
				break ;
			default: ;
		}
	} while( !done ) ;

	delete win ;
	refresh() ;
	return ;
}

/*
 * Displays a UI allowing the player to apply bonus points to basic attributes.
 */
void Rogue::manageBasics( unsigned short points )
{
	string func = "Rogue::manageBasics(ushort)" ; // debug

	if( points == 0 ) return ;

	log->lbuf << "Raising " << this->name << "'s statistics by "
	          << points << ( points == 1 ? " point." : " points." )
	          ;
	log->writeBuffer( Logger::INFO, __FILE__, __LINE__, func ) ;

	stringstream buf ( stringstream::out ) ;

	Window *win = new Window( 10, 40 ) ;
	win->setHeader( "Boost Attributes" ) ;

	bool done = false ;
	unsigned short p = points ; // Points to spend.
	unsigned short cr = 4 ; // Cursor row for selecting stat.
	unsigned short adds[4] = { 0, 0, 0, 0 } ;

	win->mvwstr( 2, 2, "Points remaining: " ) ;


	do
	{
		// Update stats in real time.
		buf << "STR  " << pad( this->getSTR() + adds[0], 2 ) << "/"
		    << pad( this->str + adds[0], 2 ) << " ["
		    << ( (this->getSTR()+adds[0]-10)/2 >= 0 ? "+" : "" )
		    << (this->getSTR()+adds[0]-10)/2 << "]"
		    ;
		win->mvwstr( 4, 5, buf.str() ) ; buf.str("") ;
		buf << "DEX  " << pad( ( this->getDEX() + adds[1] ), 2 ) << "/"
		    << pad( ( this->dex + adds[1] ), 2 ) << " ["
		    << ( (this->getDEX()+adds[1]-10)/2 >= 0 ? "+" : "" )
		    << (this->getDEX()+adds[1]-10)/2 << "]"
		    ;
		win->mvwstr( 5, 5, buf.str() ) ; buf.str("") ;
		buf << "VIT  " << pad( ( this->getVIT() + adds[2] ), 2 ) << "/"
		    << pad( ( this->vit + adds[2] ), 2 ) << " ["
		    << ( (this->getVIT()+adds[2]-10)/2 >= 0 ? "+" : "" )
		    << (this->getVIT()+adds[2]-10)/2 << "]"
		    ;
		win->mvwstr( 6, 5, buf.str() ) ; buf.str("") ;
		buf << "WIL  " << pad( ( this->getWIL() + adds[3] ), 2 ) << "/"
		    << pad( ( this->wil + adds[3] ), 2 ) << " ["
		    << ( (this->getWIL()+adds[3]-10)/2 >= 0 ? "+" : "" )
		    << (this->getWIL()+adds[3]-10)/2 << "]"
		    ;
		win->mvwstr( 7, 5, buf.str() ) ; buf.str("") ;

		// Construct footer.
		buf << " " ;
		if( p > 0 ) buf << "[+] Raise " ;
		if( p < points && adds[cr-4] > 0 ) buf << "[-] Lower " ;
		if( p == 0 ) buf << "[Enter] Commit " ;
		win->setFooter( buf.str() ) ; buf.str("") ;

		// Redraw dynamic window contents.
		buf << p << "  " ;
		win->mvwstr( 2, 20, buf.str() ) ; buf.str("") ;
		win->mvwstr( cr, 2, ">>" ) ; // stat cursor
		for( unsigned int r = 4 ; r < 8 ; r++ )
		{ // update display of points added to each stat
			buf << "(add " << adds[r-4] << ")" ;
			win->mvwstr( r, 21, buf.str() ) ; buf.str("") ;
		}

		win->refresh() ;

		Command *c = new Command(false) ;
		unsigned int cc = c->getCode() ;
		delete c ;

		switch(cc)
		{
			case K_UARROW:
				if( cr > 4 ) win->mvwstr( cr--, 2, "  " ) ;
				break ;
			case K_DARROW:
				if( cr < 7 ) win->mvwstr( cr++, 2, "  " ) ;
				break ;
			case 43: // '+'
				if( p > 0 )
				{
					++(adds[cr-4]) ;
					--p ;
				}
				break ;
			case 45: // '-'
				if( adds[cr-4] > 0 )
				{
					--(adds[cr-4]) ;
					++p ;
				}
				break ;
			case 10: // [Enter]
			case 27: // [Esc]
				if( p == 0 ) done = true ;
			default: ;
		}
	} while( !done ) ;

	delete win ;
	refresh() ;

	this->str += adds[0] ;
	this->dex += adds[1] ;
	this->vit += adds[2] ;
	this->wil += adds[3] ;
	
	return ;
}

/*
 * Displays food-eating UI.  The function returns the number of steps/units by
 * which the rogue's hunger was sated; this may then trigger various displays
 * or other actions in the main game UI.
 */
unsigned short Rogue::manageFood(void)
{
	string func = "Rogue::manageFood()" ; // debug

	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Selecting food to eat..." ) ;

	Item *food = this->displayInventory( Item::catToChar(FOOD), "Eat what?" ) ;

	if( food != NULL ) return this->eat(food) ;
	else return 0 ;
}

/*
 * Displays general equipment management UI.
 */
void Rogue::manageEquipment(void)
{
	string func = "Rogue::manageEquipment()" ; // debug

	log->write( Logger::INFO, __FILE__, __LINE__, func,
	            "Presenting equipment management UI..." ) ;

	Window *wChoices = NULL ;
	Inventory *inv = NULL ;

	bool done = false ;

	do
	{
		refresh() ;

		if( wChoices != NULL ) { delete wChoices ; refresh() ; }
		wChoices = new Window( ((DROWS/2)-(EQUIPMENT_SLOTS/2)-2),
		                       0,
		                       (EQUIPMENT_SLOTS+2), 
		                       27 ) ;
		wChoices->setHeader( "Select equipment type:" ) ;
		wChoices->mvwstr( 2, 2, " [A]rmor " ) ;
		wChoices->mvwstr( 3, 2, " [W]eapons " ) ;
		wChoices->mvwstr( 4, 2, " [R]ings " ) ;
		wChoices->mvwstr( 5, 2, " [N]ecklaces " ) ;
		wChoices->setFooter( "[Esc] Cancel" ) ;

		if( inv != NULL ) { delete inv ; refresh() ; }
		inv = this->getInventory(K_EQUIPMENT) ;
		inv->setWindow( ((DROWS/2)-(EQUIPMENT_SLOTS/2)-2),
		                27,
		                (EQUIPMENT_SLOTS+2),
		                (DCOLS-27) ) ;
		inv->setWindowHeader( "Currently equipped:" ) ;
		inv->setWindowPage(0) ;

		wChoices->redraw() ;
		wChoices->prepare() ;
		inv->prepare() ;

		doupdate() ;

        Command *c = new Command(false) ;
        char cc = c->getCharCode() ;

        switch( cc )
        {
            case 27: // [Esc]
            case ' ':
            case 'x':
            case 'X':
                done = true ;
                break ;
			case 'A':
            case 'a':
				this->manageArmor() ;
				break ;
			case 'W':
			case 'w':
				this->manageWeapons() ;
				break ;
			case 'R':
			case 'r':
				this->manageRings() ;
				break ;
			case 'N':
			case 'n':
				this->manageNecklaces() ;
				break ;
			default: ;
		}

		delete c ;
	} while( !done ) ;

	delete wChoices ;
	delete inv ;
	refresh() ;

	return ;
}

/*
 * Displays an armor management UI for the rogue.
 */
void Rogue::manageArmor(void)
{
    string func = "Rogue::manageArmor()" ; // debug

    log->write( Logger::INFO, __FILE__, __LINE__, func,
                "Managing character's armor..." ) ;

    // Initialize three panes and inventory.
	Window *wEquipped = NULL ;
	Window *wStatus = NULL ;
    Inventory *invArmors = NULL ;

    bool done = false ;
	bool changed = true ;

    do
    {
		if( changed )
		{
			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Updating screen..." ) ;

			refresh() ;

			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Updating equipped item pane." ) ;

			if( wEquipped != NULL )
			{
				delete wEquipped ;
				refresh() ;
			}
		    wEquipped = new Window( 0, 0, 3, (DCOLS/2) ) ;
			wEquipped->setHeader( "Equipped" ) ;

			if( equipment[EQ_ARMOR] != NULL )
		        wEquipped->mvwstr( 1, 1, (equipment[EQ_ARMOR])->getDesc() ) ;
			else
        		wEquipped->mvwstr( 1, 1, EMPTY_ITEM ) ;

			if( equipment[EQ_ARMOR] == NULL )
				wEquipped->setFooter( "[S]elect | E[x]it" ) ;
		    else if( pack->isFull() )
				wEquipped->setFooter( "[S]elect | E[x]it" ) ;
			else if( (equipment[EQ_ARMOR])->isCursed() )
				wEquipped->setFooter( "E[x]it" ) ;
			else
			    wEquipped->setFooter( "[S]elect | [U]nequip | E[x]it" ) ;

			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Updating item status pane." ) ;

			if( wStatus != NULL )
			{
				delete wStatus ;
				refresh() ;
			}
			wStatus = new Window( 3, 0, (DROWS-3), (DCOLS/2) ) ;

   			if( equipment[EQ_ARMOR] != NULL )
				(equipment[EQ_ARMOR])->displayStatistics(wStatus) ;
	
			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Replacing inventory." ) ;

			if( invArmors != NULL )
			{
				delete invArmors ;
				log->write( Logger::TRACE, __FILE__, __LINE__, func,
				            "Deleted old item inventory." ) ;
				refresh() ;
			}
			invArmors = pack->toInventory(ARMOR) ;
			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Constructed new inventory." ) ;
		    invArmors->setWindow( 0, (DCOLS/2), DROWS, (DCOLS/2) ) ;
			invArmors->setWindowHeader( "Available armor sets:" ) ;
		    invArmors->setWindowPage(0) ;

			changed = false ;

			wEquipped->redraw() ;
			wEquipped->prepare() ;
			wStatus->redraw() ;
			wStatus->prepare() ;
		    invArmors->prepare() ;
			doupdate() ;
		}

        Command *c = new Command(false) ;
        char cc = c->getCharCode() ;

        switch( cc )
        {
            case 27: // [Esc]
            case ' ':
            case 'x':
            case 'X':
                done = true ;
                break ;
            case 's':
            case 'S':
				if( equipment[EQ_ARMOR] == NULL
				 || ! (equipment[EQ_ARMOR])->isCursed() )
				{
					Item *na = invArmors->selectItem() ;
					if( na != NULL ) equipArmor(na) ;
					changed = true ;
				}
				break ;
            case 'u':
            case 'U':
				if( !(pack->isFull()) ) equipArmor(NULL) ;
				changed = true ;
                break ;
            default: ;
        }

        delete c ;

    } while( !done ) ;

    delete wEquipped ;
    delete wStatus ;
    delete invArmors ;
    refresh() ;

    return ;
}

/*
 * Displays melee weapon management UI.
 */
void Rogue::manageMeleeWeapons(void)
{
    string func = "Rogue::manageMeleeWeapons()" ; // debug

    log->write( Logger::INFO, __FILE__, __LINE__, func,
                "Managing character's melee weapons..." ) ;

    // Initialize three panes and inventory.
	Window *wEquipped = NULL ;
	Window *wStatus = NULL ;
    Inventory *invMWeapons = NULL ;

    bool done = false ;
	bool changed = true ;

    do
    {
		if( changed )
		{
			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Updating screen..." ) ;

			refresh() ;

			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Updating equipped item pane." ) ;

			if( wEquipped != NULL )
			{
				delete wEquipped ;
				refresh() ;
			}
		    wEquipped = new Window( 0, 0, 3, (DCOLS/2) ) ;
			wEquipped->setHeader( "Equipped" ) ;

			if( equipment[EQ_MWEAPON] != NULL )
		        wEquipped->mvwstr( 1, 1, (equipment[EQ_MWEAPON])->getDesc() ) ;
			else
        		wEquipped->mvwstr( 1, 1, EMPTY_ITEM ) ;

			if( equipment[EQ_MWEAPON] == NULL )
				wEquipped->setFooter( "[S]elect | E[x]it" ) ;
		    else if( pack->isFull() )
				wEquipped->setFooter( "[S]elect | E[x]it" ) ;
			else if( (equipment[EQ_MWEAPON])->isCursed() )
				wEquipped->setFooter( "E[x]it" ) ;
			else
			    wEquipped->setFooter( "[S]elect | [U]nequip | E[x]it" ) ;

			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Updating item status pane." ) ;

			if( wStatus != NULL )
			{
				delete wStatus ;
				refresh() ;
			}
			wStatus = new Window( 3, 0, (DROWS-3), (DCOLS/2) ) ;

   			if( equipment[EQ_MWEAPON] != NULL )
				(equipment[EQ_MWEAPON])->displayStatistics(wStatus) ;
	
			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Replacing inventory." ) ;

			if( invMWeapons!= NULL )
			{
				delete invMWeapons ;
				log->write( Logger::TRACE, __FILE__, __LINE__, func,
				            "Deleted old item inventory." ) ;
				refresh() ;
			}
			invMWeapons = pack->toInventory( WEAPON,
			                                 SIMPLE_WEAPON,
			                                 MARTIAL_WEAPON + MARTIAL_WEAPONS ) ;
			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Constructed new inventory." ) ;
		    invMWeapons->setWindow( 0, (DCOLS/2), DROWS, (DCOLS/2) ) ;
			invMWeapons->setWindowHeader( "Available weapons:" ) ;
		    invMWeapons->setWindowPage(0) ;

			changed = false ;

			wEquipped->redraw() ;
			wEquipped->prepare() ;
			wStatus->redraw() ;
			wStatus->prepare() ;
		    invMWeapons->prepare() ;
			doupdate() ;
		}

        Command *c = new Command(false) ;
        char cc = c->getCharCode() ;

        switch( cc )
        {
            case 27: // [Esc]
            case ' ':
            case 'x':
            case 'X':
                done = true ;
                break ;
            case 's':
            case 'S':
				if( equipment[EQ_MWEAPON] == NULL
				 || ! (equipment[EQ_MWEAPON])->isCursed() )
				{
					Item *na = invMWeapons->selectItem() ;
					if( na != NULL ) equipMeleeWeapon(na) ;
					changed = true ;
				}
				break ;
            case 'u':
            case 'U':
				if( !(pack->isFull()) ) equipMeleeWeapon(NULL) ;
				changed = true ;
                break ;
            default: ;
        }

        delete c ;

    } while( !done ) ;

    delete wEquipped ;
    delete wStatus ;
    delete invMWeapons ;
    refresh() ;
	return ;
}

/*
 * Displays ranged weapon management UI.
 */
void Rogue::manageRangedWeapons(void)
{
    string func = "Rogue::manageRangedWeapons()" ; // debug

    log->write( Logger::INFO, __FILE__, __LINE__, func,
                "Managing character's ranged weapons..." ) ;

    // Initialize three panes and inventory.
	Window *wEquipped = NULL ;
	Window *wWStatus = NULL ;
	Window *wAStatus = NULL ;
    Inventory *invRWeapons = NULL ;
	Inventory *invAmmo = NULL ;

    bool done = false ;
	bool changed = true ;

    do
    {
		if( changed )
		{
			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Updating screen..." ) ;

			refresh() ;

			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Updating equipped item pane." ) ;

			if( wEquipped != NULL ) { delete wEquipped ; refresh() ; }

		    wEquipped = new Window( 0, 0, 4, (DCOLS/2) ) ;
			wEquipped->setHeader( "Equipped" ) ;

			if( equipment[EQ_RWEAPON] != NULL )
		        wEquipped->mvwstr( 1, 1, (equipment[EQ_RWEAPON])->getDesc() ) ;
			else
        		wEquipped->mvwstr( 1, 1, EMPTY_ITEM ) ;

			if( equipment[EQ_AMMO] != NULL )
				wEquipped->mvwstr( 2, 1, (equipment[EQ_AMMO])->getDesc() ) ;
			else
				wEquipped->mvwstr( 2, 1, EMPTY_ITEM ) ;

			stringstream eqft ( stringstream::out ) ;

			if( equipment[EQ_RWEAPON] == NULL
			 || ! (equipment[EQ_RWEAPON])->isCursed() )
				eqft << "[W]eapon|" ;
			if( equipment[EQ_RWEAPON] != NULL
			 && ( equipment[EQ_AMMO] == NULL
 			   || ! (equipment[EQ_AMMO])->isCursed() ) )
				eqft << "[A]mmo|" ;
			if( ( equipment[EQ_RWEAPON] != NULL
			   && ! (equipment[EQ_RWEAPON])->isCursed() )
			 || ( equipment[EQ_AMMO] != NULL
			   && ! (equipment[EQ_AMMO])->isCursed() )
			  )
				eqft << "[U]nequip|" ;
			eqft << "E[x]it" ;
			wEquipped->setFooter( eqft.str() ) ;

			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Updating item status panes." ) ;

			if( wWStatus != NULL ) { delete wWStatus ; refresh() ; }

			wWStatus = new Window( 4, 0, ((DROWS-4)/2), (DCOLS/2) ) ;

   			if( equipment[EQ_RWEAPON] != NULL )
				(equipment[EQ_RWEAPON])->displayStatistics(wWStatus) ;

			if( wAStatus != NULL ) { delete wAStatus ; refresh() ; }
	
			wAStatus = new Window( (4+((DROWS-4)/2)), 0,
			                       ((DROWS-4)/2), (DCOLS/2) ) ;

			if( equipment[EQ_AMMO] != NULL )
				(equipment[EQ_AMMO])->displayStatistics(wAStatus) ;

			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Replacing inventories." ) ;

			if( invRWeapons != NULL ) { delete invRWeapons ; refresh() ; }
			invRWeapons = pack->toInventory( WEAPON, RANGED_WEAPON,
			                                 RANGED_WEAPON + RANGED_WEAPONS ) ;
			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Constructed new ranged weapon inventory." ) ;
		    invRWeapons->setWindow( 4, (DCOLS/2), ((DROWS-4)/2), (DCOLS/2) ) ;
			invRWeapons->setWindowHeader( "Ranged weapons:" ) ;
		    invRWeapons->setWindowPage(0) ;

			if( invAmmo != NULL ) { delete invAmmo ; refresh() ; }
			invAmmo = pack->toInventory( WEAPON, AMMUNITION,
			                             AMMUNITION + AMMUNITIONS ) ;
			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Constructed new ammunition inventory." ) ;
			invAmmo->setWindow( (4+((DROWS-4)/2)), (DCOLS/2),
			                    ((DROWS-4)/2), (DCOLS/2) ) ;
			invAmmo->setWindowHeader( "Available ammunition:" ) ;
			invAmmo->setWindowPage(0) ;

			changed = false ;

			wEquipped->redraw() ;
			wEquipped->prepare() ;
			wWStatus->redraw() ;
			wWStatus->prepare() ;
			wAStatus->redraw() ;
			wAStatus->prepare() ;
			invRWeapons->prepare() ;
			invAmmo->prepare() ;
			doupdate() ;
		}

        Command *c = new Command(false) ;
        char cc = c->getCharCode() ;

        switch( cc )
        {
            case 27: // [Esc]
            case ' ':
            case 'x':
            case 'X':
                done = true ;
                break ;
            case 'w':
            case 'W':
				if( equipment[EQ_RWEAPON] == NULL
				 || ! (equipment[EQ_RWEAPON])->isCursed() )
				{
					Item *nrw = invRWeapons->selectItem() ;
					if( nrw != NULL )
					{
						try { equipRangedWeapon(nrw) ; }
						catch( int e )
						{
							displayDialog( "Couldn't put away old ammo." ) ;
							refresh() ;
						}
					}
					changed = true ;
				}
				break ;
			case 'a':
			case 'A':
				if( equipment[EQ_RWEAPON] != NULL
				 && ( equipment[EQ_AMMO] == NULL
				   || ! (equipment[EQ_AMMO])->isCursed() ) )
				{
					Item *na = invAmmo->selectItem() ;
					if( na != NULL )
					{
						try { equipAmmunition(na) ; }
						catch( int e )
						{
							switch(e)
							{
								case -1:
									displayDialog( "Couldn't put away old ammo." ) ;
									break ;
								case -2:
									displayDialog( "Can't equip ammo without weapon." ) ;
									break ;
								case -3:
									displayDialog( "Can't equip incompatible ammo." ) ;
									break ;
								default: ;
							}
							refresh() ;
						}
					}
					changed = true ;
				}
				break ;
            case 'u':
            case 'U':
				if( !(pack->isFull()) ) equipRangedWeapon(NULL) ;
				changed = true ;
                break ;
            default: ;
        }

        delete c ;

    } while( !done ) ;

    delete wEquipped ;
    delete wWStatus ;
	delete wAStatus ;
    delete invRWeapons ;
	delete invAmmo ;
    refresh() ;
	return ;
}

/*
 * Displays general weapon management UI.  This function merely presents an
 * initial menu allowing the player to select melee or ranged weapons; once the
 * choice is made, the appropriate subfunction manageMeleeWeapons() or
 * manageRangedWeapons() is called.
 */
void Rogue::manageWeapons(void)
{
	string func = "Rogue::manageWeapons()" ; // debug

    log->write( Logger::INFO, __FILE__, __LINE__, func,
                "Managing character's weapons..." ) ;

	// Start here with a "[M]elee or [R]anged" dialog, then do it just like
	// the armor management UI.
	Window *wChoice = new Window( 3, 31 ) ;
	wChoice->setHeader( "Select Weapon Type" ) ;
	wChoice->mvwstr( 1, 2, "[M]elee | [R]anged | E[x]it" ) ;
	wChoice->refresh() ;

	bool done = false ;

	do
	{
		Command *c = new Command(false) ;
		char cc = c->getCharCode() ;

		switch(cc)
		{
			case 'm':
			case 'M':
				this->manageMeleeWeapons() ;
				done = true ;
				break ;
			case 'r':
			case 'R':
				this->manageRangedWeapons() ;
				done = true ;
				break ;
			case 27: // [Esc]
			case ' ':
			case 'x':
			case 'X':
				done = true ;
				break ;
			default: ;
		}
	} while(!done) ;

	delete wChoice ;
	refresh() ;

	return ;
}

/*
 * Displays potion-quaffing UI.  The function returns a pointer to the selected
 * potion.
 */
Item * Rogue::managePotions(void)
{
	string func = "Rogue::managePotions()" ; // debug

	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Selecting potion to quaff..." ) ;

	return this->displayInventory( Item::catToChar(POTION), "Quaff what?" ) ;
}

/*
 * Displays scroll-reading UI.  The function returns a pointer to the selected
 * scroll.
 */
Item * Rogue::manageScrolls(void)
{
	string func = "Rogue::manageScrolls()" ; // debug

	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Selecting scroll to read..." ) ;

	return this->displayInventory( Item::catToChar(SCROLL), "Read what?" ) ;
}

/*
 * Displays ring management UI.
 */
void Rogue::manageRings(void)
{
    string func = "Rogue::manageRings()" ; // debug

    log->write( Logger::INFO, __FILE__, __LINE__, func,
                "Managing character's rings..." ) ;

    // Initialize three panes and inventory.
	Window *wEquipped = NULL ;
	Window *wLRing = NULL ;
	Window *wRRing = NULL ;
    Inventory *invRings = NULL ;

    bool done = false ;
	bool changed = true ;

    do
    {
		if( changed )
		{
			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Updating screen..." ) ;

			refresh() ;

			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Updating equipped item pane." ) ;

			if( wEquipped != NULL ) { delete wEquipped ; refresh() ; }

		    wEquipped = new Window( 0, 0, 4, (DCOLS/2) ) ;
			wEquipped->setHeader( "Equipped" ) ;

			if( equipment[EQ_LRING] != NULL )
		        wEquipped->mvwstr( 1, 1, (equipment[EQ_LRING])->getDesc() ) ;
			else
        		wEquipped->mvwstr( 1, 1, EMPTY_ITEM ) ;

			if( equipment[EQ_RRING] != NULL )
				wEquipped->mvwstr( 2, 1, (equipment[EQ_RRING])->getDesc() ) ;
			else
				wEquipped->mvwstr( 2, 1, EMPTY_ITEM ) ;

			stringstream eqft ( stringstream::out ) ;

			if( equipment[EQ_LRING] == NULL
			 || ! (equipment[EQ_LRING])->isCursed() )
				eqft << "[L]eft|" ;
			if( equipment[EQ_RRING] == NULL
			 || ! (equipment[EQ_RRING])->isCursed() )
				eqft << "[R]ight|" ;
			if( ( equipment[EQ_LRING] != NULL
			   && ! (equipment[EQ_LRING])->isCursed() )
			 || ( equipment[EQ_RRING] != NULL
			   && ! (equipment[EQ_RRING])->isCursed() )
			  )
				eqft << "[U]nequip|" ;
			eqft << "E[x]it" ;
			wEquipped->setFooter( eqft.str() ) ;

			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Updating ring status panes." ) ;

			if( wLRing != NULL ) { delete wLRing ; refresh() ; }

			wLRing = new Window( 4, 0, ((DROWS-4)/2), (DCOLS/2) ) ;

   			if( equipment[EQ_LRING] != NULL )
				(equipment[EQ_LRING])->displayStatistics(wLRing) ;

			if( wRRing != NULL ) { delete wRRing ; refresh() ; }
	
			wRRing = new Window( (4+((DROWS-4)/2)), 0, 
			                     ((DROWS-4)/2), (DCOLS/2) ) ;

			if( equipment[EQ_RRING] != NULL )
				(equipment[EQ_RRING])->displayStatistics(wRRing) ;

			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Replacing inventory." ) ;

			if( invRings != NULL ) { delete invRings ; refresh() ; }
			invRings = pack->toInventory( RING ) ;
			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Constructed new rings inventory." ) ;
		    invRings->setWindow( 0, (DCOLS/2), DROWS, (DCOLS/2) ) ;
			invRings->setWindowHeader( "Select ring:" ) ;
		    invRings->setWindowPage(0) ;

			changed = false ;

			wEquipped->redraw() ;
			wEquipped->prepare() ;
			wLRing->redraw() ;
			wLRing->prepare() ;
			wRRing->redraw() ;
			wRRing->prepare() ;
			invRings->prepare() ;
			doupdate() ;
		}

        Command *c = new Command(false) ;
        char cc = c->getCharCode() ;

        switch( cc )
        {
            case 27: // [Esc]
            case ' ':
            case 'x':
            case 'X':
                done = true ;
                break ;
            case 'l':
            case 'L':
				if( equipment[EQ_LRING] == NULL
				 || ! (equipment[EQ_LRING])->isCursed() )
				{
					Item *nlr = invRings->selectItem() ;
					if( nlr != NULL ) equipRing( nlr, false ) ;
					changed = true ;
				}
				break ;
			case 'r':
			case 'R':
				if( equipment[EQ_RRING] == NULL
				 || ! (equipment[EQ_RRING])->isCursed() )
				{
					Item *nrr = invRings->selectItem() ;
					if( nrr != NULL ) equipRing( nrr, true ) ;
					changed = true ;
				}
				break ;
            case 'u':
            case 'U':
				if( !(pack->isFull()) ) equipRing(NULL,false) ;
				if( !(pack->isFull()) ) equipRing(NULL,true) ;
				changed = true ;
                break ;
            default: ;
        }

        delete c ;

    } while( !done ) ;

    delete wEquipped ;
    delete wLRing ;
	delete wRRing ;
    delete invRings ;
    refresh() ;
}

/*
 * Displays a necklace management UI for the rogue.
 */
void Rogue::manageNecklaces(void)
{
    string func = "Rogue::manageNecklaces()" ; // debug

    log->write( Logger::INFO, __FILE__, __LINE__, func,
                "Managing character's necklaces..." ) ;

    // Initialize three panes and inventory.
	Window *wEquipped = NULL ;
	Window *wStatus = NULL ;
    Inventory *invNecklaces = NULL ;

    bool done = false ;
	bool changed = true ;

    do
    {
		if( changed )
		{
			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Updating screen..." ) ;

			refresh() ;

			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Updating equipped item pane." ) ;

			if( wEquipped != NULL )
			{
				delete wEquipped ;
				refresh() ;
			}
		    wEquipped = new Window( 0, 0, 3, (DCOLS/2) ) ;
			wEquipped->setHeader( "Equipped" ) ;

			if( equipment[EQ_NECK] != NULL )
		        wEquipped->mvwstr( 1, 1, (equipment[EQ_NECK])->getDesc() ) ;
			else
        		wEquipped->mvwstr( 1, 1, EMPTY_ITEM ) ;

			if( equipment[EQ_NECK] == NULL )
				wEquipped->setFooter( "[S]elect | E[x]it" ) ;
		    else if( pack->isFull() )
				wEquipped->setFooter( "[S]elect | E[x]it" ) ;
			else if( (equipment[EQ_NECK])->isCursed() )
				wEquipped->setFooter( "E[x]it" ) ;
			else
			    wEquipped->setFooter( "[S]elect | [U]nequip | E[x]it" ) ;

			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Updating item status pane." ) ;

			if( wStatus != NULL )
			{
				delete wStatus ;
				refresh() ;
			}
			wStatus = new Window( 3, 0, (DROWS-3), (DCOLS/2) ) ;

   			if( equipment[EQ_NECK] != NULL )
				(equipment[EQ_NECK])->displayStatistics(wStatus) ;
	
			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Replacing inventory." ) ;

			if( invNecklaces != NULL )
			{
				delete invNecklaces ;
				log->write( Logger::TRACE, __FILE__, __LINE__, func,
				            "Deleted old item inventory." ) ;
				refresh() ;
			}
			invNecklaces = pack->toInventory(NECKLACE) ;
			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Constructed new inventory." ) ;
		    invNecklaces->setWindow( 0, (DCOLS/2), DROWS, (DCOLS/2) ) ;
			invNecklaces->setWindowHeader( "Available necklaces:" ) ;
		    invNecklaces->setWindowPage(0) ;

			changed = false ;

			wEquipped->redraw() ;
			wEquipped->prepare() ;
			wStatus->redraw() ;
			wStatus->prepare() ;
		    invNecklaces->prepare() ;
			doupdate() ;
		}

        Command *c = new Command(false) ;
        char cc = c->getCharCode() ;

        switch( cc )
        {
            case 27: // [Esc]
            case ' ':
            case 'x':
            case 'X':
                done = true ;
                break ;
            case 's':
            case 'S':
				if( equipment[EQ_NECK] == NULL
				 || ! (equipment[EQ_NECK])->isCursed() )
				{
					Item *nn = invNecklaces->selectItem() ;
					if( nn != NULL ) equipNecklace(nn) ;
					changed = true ;
				}
				break ;
            case 'u':
            case 'U':
				if( !(pack->isFull()) ) equipNecklace(NULL) ;
				changed = true ;
                break ;
            default: ;
        }

        delete c ;

    } while( !done ) ;

    delete wEquipped ;
    delete wStatus ;
    delete invNecklaces ;
    refresh() ;

    return ;
}


// /// PRIVATE FUNCTIONS (Rogue) /////////////////////////////////////////// //

/* Counts number of items equipped. */
unsigned short Rogue::countEquippedItems()
{
	string func = "Rogue::countEquippedItems()" ; // debug

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
bool Rogue::isNaked(void)
{
	for( unsigned short i = 0 ; i < EQUIPMENT_SLOTS ; i++ )
		if( equipment[i] != NULL ) return false ;

	return true ;

//	return ( m_weapon == NULL && r_weapon == NULL && ammunition == NULL
//	      && armor == NULL && lring == NULL && rring == NULL
//	      && neck == NULL )
//	       ;
}

unsigned short Rogue::rerollStat( void )
{
	// Roll 4d3, keep the highest 3 rolls.
	unsigned short rolls[3] = { 0, 0, 0 } ;
	unsigned short r, t ;

	for( unsigned short i = 0 ; i < 4 ; i++ )
	{
		r = roll(1,6,0) ;
		for( unsigned short j = 0 ; j < 3 ; j++ )
		{
			if( r > rolls[j] )
			{
				t = rolls[j] ;
				rolls[j] = r ;
				r = t ;
			}
		}
	}

	r = 0 ;
	for( unsigned short i = 0 ; i < 3 ; i++ ) r += rolls[i] ;

	return r ;
}

/*
 * Resets the number of steps to go until the character spontaneously heals 1
 * HP.
 */
unsigned short Rogue::resetStepsToHeal(void)
{
	string func = "Rogue::resetStepsToHeal()" ; // debug

	// Basic algorithm: The rogue should heal 1% of his HP every 5 steps,
	// thus healing 100% HP after 500 steps.

	stepsToHeal = 500 / getMaxHP() ;

	log->lbuf << name << " will heal another 1 hp in "
		 << stepsToHeal << ( stepsToHeal == 1 ? " step." : " steps." )
		;
	log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;

	return stepsToHeal ;
}

/* ************************************************************************* */
