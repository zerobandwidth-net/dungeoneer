/*
 * rogue.h
 * You may play with this source however you like, as long as you don't remove
 * this comment, claim undue credit, or make a profit on it.  Enjoy.
 * CHANGE HISTORY
 * 2011-11-17
 *  Created file.
 */

#ifndef _ROGUE_H_
#define _ROGUE_H_

#ifndef _DUNGEONEER_H_
#include "dungeoneer.h"
#endif

#include "item.h"
#include "pack.h"

/* *** TWEAKS ************************************************************** */

#define STARTING_HUNGER 1250
#define ROGUE_HIT_DIE 6

enum EquipmentSlot {
	EQ_MWEAPON, EQ_RWEAPON, EQ_AMMO, EQ_ARMOR, EQ_LRING, EQ_RRING, EQ_NECK
};
#define EQUIPMENT_SLOTS 7

/* *** COMPLEX TYPE DEFINITIONS ********************************* (none) *** */

/* *** GLOBAL PROCEDURE PROTOTYPES ****************************** (none) *** */

/* *** CLASS PROTOTYPES (needed for friendship) ***************** (none) *** */

/* *** CLASS DEFINITIONS *************************************************** */

class Rogue
{
	public:

		// Constructors...

		Rogue() ; // Default: Sets everything to zero/blank.

		// Destructors...

		~Rogue() ;

		// Accessors...

		string getName() ; // Character's name.
		bool getGender() ; // Indicates whether character is male.
		char getGenderChar() ; // Returns 'M' or 'F'.
		short getMaxHP() ; // Maximum HP.
		short getHP() ; // Current HP.
		unsigned short getSTR() ; // Strength stat.
		unsigned short getDEX() ; // Dexterity stat.
		unsigned short getVIT() ; // Vitality stat.
		unsigned short getWIL() ; // Willpower stat.
		short getMSTR() ; // Strength modifier.
		short getMDEX() ; // Dexterity modifier.
		short getMVIT() ; // Vitality modifier.
		short getMWIL() ; // Willpower modifier.
		bool getStatus( StatusEffect stype ) ;
		unsigned short getLevel() ; // Effective level.
		unsigned short getActualLevel() ; // actual level
		unsigned int getXP() ; // XP stat.
		unsigned short getXPPercent() ; // Percentage complete.
		unsigned short getHunger() ; // Returns number of remaining steps.
			static const unsigned short HUNGER = 300 ;
			static const unsigned short WEAKNESS = 150 ;
			static const unsigned short FAINTING = 20 ;
			static const unsigned short STARVED = 0 ;
		unsigned short getHungerState() ; // Returns one of the statuses below.
			static const unsigned short HUNGRY = 1 ;
			static const unsigned short WEAK = 2 ;
			static const unsigned short FAINT = 3 ;
			static const unsigned short STARVING = 4 ;
		unsigned int getGold() ;

		unsigned short getEquippedItemCount() ;
		Item* getEquipment( EquipmentSlot slot ) ;

		Inventory* getInventory() ;
		Inventory* getInventory( unsigned short filter ) ;

		Pack *getPack() ; // DEBUG - Shouldn't be public in final version.

		unsigned short getRow() ;
		unsigned short getCol() ;

		bool canAct() ; // Indicates whether the player may act in a given turn.
		bool hasRing( unsigned short rkind ) ;
		bool hasNecklace( unsigned short nkind ) ;

		// Mutators...

		bool addItemToPack( Item *item ) ; // Adds item or returns false if not
		unsigned short eat( Item *food ) ; // Eat food; returns steps added.
		Item* equipArmor( Item *na ) ; // Returns old armor or NULL.
		Item* equipMeleeWeapon( Item *nmw ) ; // Returns old weapon or NULL.
		Item* equipRangedWeapon( Item *nrw ) ; // Returns old weapon or NULL.
		Item* equipAmmunition( Item *na ) ; // Returns old ammo or NULL.
		Item* equipRing( Item *newring, bool rh ) ; // Returns old ring or NULL.
		Item* equipNecklace( Item *nn ) ; // Returns old necklace or NULL.
		bool gainXP( unsigned int xpgain ) ; // Rogue gains experience.
		bool heal( unsigned short x ) ; // Heal specified HP if necessary.
		bool inflict( StatusEffect effect,
		              unsigned short duration,
		              unsigned short save ) ;
		bool quaff( Item *potion ) ; // Quaff a potion.
		Item* removeItemFromPack( Item *item ) ;
		void reroll() ; // Initialize a Rogue instance with statistics.
		bool setGender( bool y ) ; // Sets character's gender.
		bool setGender( char g ) ; // Sets character's gender.
		void setName( string n ) ; // Sets character's name.
		void setRow( unsigned short r ) ;
		void setCol( unsigned short c ) ;
		void setLocation( unsigned short r, unsigned short c ) ;
		short tick() ; // Advance time by 1 turn.

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
		void manageEquipment() ; // Displays general equipment management UI.
		void manageArmor() ; // Displays armor management UI.
		void manageMeleeWeapons() ; // Displays melee weapon management UI.
		void manageRangedWeapons() ; // Displays ranged weapon management UI.
		void manageWeapons() ; // Displays weapon management UI.
		Item* managePotions() ; // Displays potion drinking UI.
		Item* manageScrolls() ; // Displays scroll reading UI.
		void manageRings() ; // Displays ring management UI.
		void manageNecklaces() ; // Displays necklace management UI.

	private:

		// Status and statistics...
		short maxhp, hp, maxhp_d ; // Max and current HP, and damage to max HP.
		unsigned short str, dex, vit, wil ; // Vital statistics.
		unsigned short str_d, dex_d, vit_d, wil_d ; // Damage to vital stats.
		unsigned short *status_effects ; // Remaining duration of status effects
		unsigned short xplevel, xplevel_d ; // Actual level and drained levels.
		unsigned int xp ; // Experience points.
		unsigned short hunger ; // Hunger level, counted in remaining actions.
		unsigned short stepsToHeal ; // Number of steps until you heal 1 HP.

		// Equipment...
		Item ** equipment ;
		unsigned int gold ;
		Pack *pack ; // Pack of unequipped items being carried.

		// Other...
		unsigned short row, col ; // Location in the current dungeon level.
		unsigned char displaychar ; // Used to display the rogue onscreen.
		string name ; // The character's name.
		bool hasY ; // Indicates that the character is male.

		// Functions...
		unsigned short countEquippedItems() ;
		bool isNaked() ; // True if no items are equipped.
		static unsigned short rerollStat() ;
		unsigned short resetStepsToHeal() ;
};

/* ************************************************************************* */
#endif
