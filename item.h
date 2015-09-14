/*
 * item.h
 * You may play with this source however you like, as long as you don't remove
 * this comment, claim undue credit, or make a profit on it.  Enjoy.
 * CHANGE HISTORY
 * 2011-11-17 (0.00)
 *  Created file.
 */

#ifndef _DUNGEONEER_H_
#include "dungeoneer.h"
#endif

#ifndef _ITEM_H_
#define _ITEM_H_

#include <string>
#include "window.h"
using namespace std ;

/* *** TWEAKS ************************************************************** */

#define COIN "dinar"
#define FRUIT_NAME "apricot"
#define FRUIT_XP_CHANCE 10

#define PCOLORS 64
#define STSYLLABLES 52
#define WMATERIALS 26
#define JMATERIALS 50

#define POTION_SAVE 15 // absolute
#define SCROLL_SAVE 10 // plus level of caster
#define WAND_SAVE 15 // plus level of user

// /// ITEM CATEGORIES AND KINDS /////////////////////////////////////////// //
// These were originally coded as static constant members of the Item class,
// but then after all the "byte" typedef was removed, they had to be converted
// to unsigned shorts, and that would just take up too much space, so now they
// are back to being pre-processor defines again, just like the original code
// for "rogue".

// Item category constants...
#define GOLD 0
#define FOOD 1
#define ARMOR 2
#define WEAPON 3
#define POTION 4
#define SCROLL 5
#define WAND 6
#define RING 7
#define NECKLACE 8
#define ITEM_TYPES 9

// Food kind constants...
#define RATION 0
#define FRUIT 1
#define FOOD_TYPES 2

// Armor kind constants... (note: magical)
#define CLOTHES 0
#define PADDED_ARMOR 1
#define LEATHER_ARMOR 2
#define STUDDED_LEATHER 3
#define HIDE_ARMOR 4
#define SCALEMAIL 5
#define CHAINMAIL 6
#define BANDED_MAIL 7
#define HALFPLATE 8
#define FULLPLATE 9
#define ARMOR_TYPES 10

// Weapon kind constants... (kind=subtype+name)
#define SIMPLE_WEAPON 0
	#define SPIKEGLOVE 0
	#define DAGGER 1
	#define MACE 2
	#define SHORTSPEAR 3
	#define SPEAR 4
#define SIMPLE_WEAPONS 5

#define MARTIAL_WEAPON 10
	#define HANDAXE 0
	#define KUKRI 1
	#define SHORTSWORD 2
	#define BATTLEAXE 3
	#define LONGSWORD 4
	#define PICK 5
	#define RAPIER 6
	#define WARHAMMER 7
#define MARTIAL_WEAPONS 8

#define THROWING_WEAPON 20
	#define DART 0
	#define JAVELIN 1
	#define THRKNIFE 3
#define THROWING_WEAPONS 3

#define RANGED_WEAPON 30
	#define CROSSBOW 0
	#define SHORTBOW 1
	#define LONGBOW 2
#define RANGED_WEAPONS 3

#define AMMUNITION 40
	#define BOLT 0
	#define ARROW 1
#define AMMUNITIONS 2

#define WEAPON_SUBTYPES 5

// Potion kind constants...
#define P_STAT_BOOST 0
#define P_LSR_RESTO 1 // heals one statistic, one level, or all maxHP damage
#define P_GRT_RESTO 2 // heals everything
#define P_HEALING 3 // heals some HP
#define P_GRT_HEALING 4 // heals a lot of HP
#define P_LEVELUP 5
#define P_SEE_MONS 6
#define P_SEE_STUFF 7
#define P_LEVITATE 8
#define P_HASTE 9
#define P_TRUESIGHT 10
#define P_SLEEP 11
#define P_CONFUSE 12
#define P_POISON 13
#define P_BLIND 14
#define P_LSD 15
#define POTION_TYPES 16

// Scroll kind constants...
#define S_ORUST 0 // prot vs rust
#define S_HARDEN 1 // armor bonus
#define S_SHARPEN 2 // weapon bonus
#define S_IDENTIFY 3
#define S_UNCURSE 4
#define S_PARALYZE 5
#define S_SLEEP 6
#define S_SCARE 7
#define S_SPAWN 8 // creates a monster
#define S_AGGRAVATE 9
#define S_MAPPING 10
#define S_TELEPORT 11
#define SCROLL_TYPES 12

// Wand kind constants...
#define W_HASTE 0
#define W_SLOW 1
#define W_SLEEP 2
#define W_CONFUSE 3
#define W_POISON 4
#define W_PARALYZE 5
#define W_POLYMORPH 6
#define W_MISSILE 7
#define W_NULLIFY 8
#define WAND_TYPES 9
		
// Ring kind constants...
#define R_STEALTH 0
#define R_TELEPORT 1
#define R_REGENERATION 2
#define R_SUSTENANCE 3
#define R_OPOISON 4
#define R_OSTONE 5
#define R_STRBOOST 6
#define R_DEXBOOST 7
#define R_VITBOOST 8
#define R_WILBOOST 9
#define R_TRUESIGHT 10
#define R_ORUST 11
#define R_SEARCHING 12
#define RING_TYPES 13

// Necklace kind constants...
#define N_ESCAPISM 0 // allows dungeon escapes
#define N_DMGREDUCTION 1
#define N_ARMOR 2 // natural armor bonus
#define N_WIZARDRY 3 // boosts item effectiveness
#define NECKLACE_TYPES 4

/* *** COMPLEX TYPE DEFINITIONS ******************************************** */

// Structure for holding templatized values for new random items.
typedef struct // item_template
{
	string cname ; // Name given by the player to an item.
	string nname ; // Name shown before identification.
	string rname ; // Real name of the item after identification.
	unsigned short value ;
	bool is_identified ;
	unsigned short dmgrolls, dmgdie, cthreat, cmult ;
	short bonus ; 
} item_template ;
#define acbonus dmgdie // Alias for AC bonus in armor stats.
#define wcharges cthreat // Alias for charges remaining on a wand.
#define lmult cmult // Alias for indicator of effect multiplier by level.

/* *** GLOBAL PROCEDURE PROTOTYPES ***************************************** */

void initializeItemTemplates(void) ;

/* *** CLASS PROTOTYPES (needed for friendship) ***************** (none) *** */

/* *** CLASS DEFINITIONS *************************************************** */

class Item
{
	public:

		// Static initialization methods...

		static unsigned short charToCat( char tc ) ;
		static char catToChar( unsigned short cat ) ;

		// Constructors...

		Item() ; // Default constructor.

		Item( Item *orig ) ; // Copy constructor.

		// Explicit constructor.
		Item( string x_cname, string x_nname, string x_rname,
		      unsigned short x_category, unsigned short x_kind,
		      unsigned short x_quantity, unsigned short x_value,
		      bool x_is_protected, bool x_is_cursed, bool x_is_identified,
		      bool x_is_carried,
		      unsigned short x_dmgrolls, unsigned short x_dmgdie,
		      unsigned short x_cthreat, unsigned short x_cmult,
		      short x_bonus,
		      unsigned short x_drow, unsigned short x_dcol
		    ) ;

		// Constructor based on item template; can also modify template.
		Item( item_template *itmpl, unsigned short icat, unsigned short ikind );

		// Constructor to make a basic version of an explicit item kind.
		Item( unsigned short icat, unsigned short ikind, unsigned short iquan );

		// Accessor methods...
		short getACBonus() ; // Returns benefit to armor class from item.
		short getBonus() ;
		unsigned short getCategory() ;
		unsigned short getCritMultiplier() ;
		string getDesc() ; // Fully describes item, desc is uncapitalized.
		string getDesc( bool capitalize ) ; // Fully describes the item.
		string getDiag() ; // Returns diagnostic information.
		char getDisplayChar() ; // Returns the display character for this item.
		unsigned short getKind() ;
		string getName( bool capitalize ) ; // Gives the item's name.
		unsigned short getQuantity() ;
		string getUseMessage() ; // Returns string announcing item use.
		unsigned short getValue() ;
		unsigned short getWeaponSubtype() ;
		bool hasRoll() ; // Indicates whether the item has a dieroll effect.
		bool isCursed() ;
		bool isItem( unsigned short c, unsigned short k ) ;
		bool isRecognized() ; // Is identified, or type is identified.
		bool isThrowable() ; // Can be thrown as a weapon to inflict dmg/status
		unsigned short dmgroll() ; // Return result of item's random die roll.

		// Mutator methods...
		bool absorb( Item *that ) ; // Combines two items.
		void identify() ; // Sets is_identified and modifies tables if needed.
		bool makeRandom( unsigned short dlevel ) ; // Creates new random item.
		bool makeRandom( unsigned short icat, unsigned short dlevel ) ;
		void setBonus( short b ) ; // Sets the item's new bonus.
		bool setCursed( bool c ) ;
		void setLocation( unsigned short row, unsigned short col ) ;
		bool setProtected( bool p ) ;
		void setQuantity( unsigned short q ) ;
		unsigned short use( unsigned short q ) ; // Use up some of this item.

		// Other public methods...
		bool couldStackWith( Item *that ) ;
		bool isStackable() ;
		void displayStatistics( Window *win ) ;

	private:

		// Fields...
		string cname ; // Name given by the player to an item.
		string nname ; // Name shown before identification.
		string rname ; // Real name of the item after identification.
		unsigned short category, kind, quantity ;
		unsigned short value ;
		bool is_protected, is_cursed, is_identified, is_carried ;
		unsigned short dmgrolls, dmgdie, cthreat, cmult ;
		short bonus ;
		unsigned short drow, dcol ; // location; used while not picked up

		// Private utility functions...
		void copyFromTemplate( item_template tmpl ) ;
		string getBonusString( bool fpad, bool ppad ) ;
		string getWeaponDamageDesc() ;
		void initFromTemplate( item_template* itmpl,
		                       unsigned short icat,
		                       unsigned short ikind ) ;
};

/* ************************************************************************* */
#endif
