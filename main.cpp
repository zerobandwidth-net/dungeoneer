/*
 * main.cpp
 * You may play with this source however you like, as long as you don't remove
 * this comment, claim undue credit, or make a profit on it.  Enjoy.
 * CHANGE HISTORY
 * 2011-11-17
 *  Created file.
 */

#include <cstdio>
#include <iostream>
#include <ncurses.h>
#include <string>
#include "actor.h"
#include "ally.h"
#include "command.h"
#include "debug.h"
#include "dungeon.h"
#include "item.h"
#include "party.h"
#include "party.cpp"
#include "random.h"
#include "ui.h"
#include "window.h"
using namespace std ;

/* *** EXTERNAL REFERENCES ************************************************* */

extern item_template armor_templates[] ; // item.cpp
extern item_template weapon_templates[] ; // item.cpp
extern item_template potion_templates[] ; // item.cpp
extern item_template scroll_templates[] ; // item.cpp
extern item_template wand_templates[] ; // item.cpp
extern item_template ring_templates[] ; // item.cpp
extern item_template necklace_templates[] ; // item.cpp

extern Logger *log ; // debug.cpp

/* *** LOCAL DATA ********************************************************** */

/* *** LOCAL PROCEDURE PROTOTYPES ****************************************** */

void initializePlayerItems(Ally*) ; // Sets default player items/equipment

/* *** PROCEDURE DEFINITIONS *********************************************** */

int main( int argc, char *argv[] )
{
	string func = "main()" ;

	log = new Logger(Logger::TRACE) ;
	log->write( Logger::INFO, __FILE__, __LINE__, func, "Starting main()." ) ;

	set_random_seed() ;
	initializeItemTemplates() ;
	initializeCurses(true) ; // sets all curses init values; see ui.cpp

	// Initialize player party.
	Party<Ally> *party = new Party<Ally>( MAX_PARTY_SIZE ) ;
	Ally *a = new Ally() ;
	party->addMember(a) ;
	a->manageBasics() ;
	initializePlayerItems(a) ;

	Dungeon testDungeon( DT_TEST, a, "Test Dungeon" ) ;
	testDungeon.play() ;

	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Exited test dungeon." ) ;

/* OLD UNIT TEST CODE
	mvprintw( y++, x, "(begin)" ) ;
	refresh() ;

	Command *c ;
	Window *w ;
	
	for(;;)
	{
		msg.str("") ; // reset buffer

		if( y > 22 )
		{
			y = 0 ;
			x += 8 ;
			if( x >= 72 )
			{
				x = 0 ;
			}
		}

		c = new Command() ;

		msg << c->getCode() << " " << c->str() ;

		switch(c->getCode())
		{
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
				attron(COLOR_PAIR(c->getCode())) ;
				quit = false ;
				break ;
			case K_QUIT: // 17 / Ctrl+Q
				endwin() ;
				return K_QUIT ;
			case K_REDRAW: // 18 / Ctrl+R
				refresh() ;
				quit = false ;
				break ;
			case 23: // Ctrl+W
			    w = new Window( 2, 2, 3, 17 ) ;
			    w->aset( COLOR_PAIR(7) ) ;
			    w->mvwstr( 1, 1, "This is a test." ) ;
				w->setFooter( "TEST" ) ;
				w->setHeader( "farglebargledarglebarg" ) ;
			    w->refresh() ;
			    (void)getch() ;
			    delete w ;
				break ;
			case K_CANCEL: // 27 / Esc
				if( quit ) { endwin() ; return 27 ; }
				else quit = true ;
				break ;
			case K_RING: // 61 / =
				r->manageRings() ;
				quit = false ;
				break ;
			case K_EQUIPMENT: // 69 / E
				r->manageEquipment() ;
				quit = false ;
				break ;
			case K_WEAR_ARMOR: // 87 / 'W'
				r->manageArmor() ;
				quit = false ;
				break ;
			case K_INVENTORY: // 105 / 'i'
				r->displayInventory() ;
				quit = false ;
				break ;
			case K_WIELD_WEAPON: // 118 / 'w'
				r->manageWeapons() ;
				quit = false ;
				break ;
			default:
				quit = false ;
		}

		mvprintw( y++, x, "%s", (msg.str()).c_str() ) ;
		refresh() ;
	}
*/

	endwin() ;

	log->write( Logger::TRACE, __FILE__, __LINE__, func, "Exiting." ) ;

	log->close() ;

	return 0 ;
}

/*
 * Set initial possessions and equipment for the player.
 */
void initializePlayerItems( Ally *a )
{
	string func = "initializePlayerItems(Ally*)" ; // debug
	log->write( Logger::INFO, __FILE__, __LINE__, func,
	            "Initializing player items..." ) ;

	Item *i ;

	// Create food.
	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Adding food item." ) ;
	i = new Item() ;
	i->makeRandom( FOOD, 1 ) ;
	a->addItemToPack(i) ;

	// Create armor.
	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Adding and equipping armor." ) ;
	i = new Item( armor_templates, ARMOR, LEATHER_ARMOR ) ;
	a->addItemToPack(i) ;
	a->equipArmor(i) ;

	// Create melee weapon.
	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Adding and equipping melee weapon." ) ;
	i = new Item( weapon_templates, WEAPON, (SIMPLE_WEAPON+MACE) ) ;
	a->addItemToPack(i) ;
	a->equipMeleeWeapon(i) ;

	// Create ranged weapons.
	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Adding and equipping ranged weapons." ) ;
	i = new Item( weapon_templates, WEAPON, (RANGED_WEAPON+SHORTBOW) ) ;
	a->addItemToPack(i) ;
	a->equipRangedWeapon(i) ;
	i = new Item( weapon_templates, WEAPON, (AMMUNITION+ARROW) ) ;
	i->setQuantity(30) ;
	a->addItemToPack(i) ;
	a->equipAmmunition(i) ;

	// TEST ONLY: Create a bunch of other stuff and add it to the pack.
	for( unsigned short c = 1 ; c < ITEM_TYPES ; c++ )
	{
		i = new Item() ;
		log->write( Logger::TRACE, __FILE__, __LINE__, func,
		            "Creating extra item..." ) ;
		i->makeRandom( c, 1 ) ;
		i->getDiag() ;
		a->addItemToPack(i) ;
	}

	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Player items initialized.  Exiting..." ) ;
	return ;
}
