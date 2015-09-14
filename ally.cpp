/*
 * ally.cpp
 * You may play with this source however you like, as long as you don't remove
 * this comment, claim undue credit, or make a profit on it.  Enjoy.
 * CHANGE HISTORY
 * 2011-11-17
 *  Created file.
 * 2012-01-25
 *  Split from rogue.cpp.
 */

#ifndef _DUNGEONEER_H_
#include "dungeoneer.h"
#endif

#include <sstream>
#include <string>
#include "actor.h"
#include "ally.h"
#include "command.h"
#include "debug.h"
#include "pack.h"
#include "random.h"
#include "ui.h"
#include "window.h"
using namespace std ;

/* *** EXTERNAL REFERENCES ************************************************* */

extern Logger *log ; // debug.cpp
extern string randomMaleNames[26] ; // actor.cpp
extern string randomFemaleNames[26] ; // actor.cpp
extern string statusEffectNames[STATUS_EFFECTS] ; // ui.cpp
extern unsigned int xpLevels[20] ; // actor.cpp

/* *** LOCAL DATA *********************************************** (none) *** */

/* *** LOCAL PROCEDURE PROTOTYPES ******************************* (none) *** */

/* *** PROCEDURE DEFINITIONS ************************************ (none) *** */

/* *** CLASS DEFINITION (Ally) ******************************************** */

// /// STATIC FUNCTIONS (classname) ///////////////////////////// (none) /// //

// /// CONSTRUCTORS (Ally) ///////////////////////////////////////////////// //

Ally::Ally(void)
{
	log->write( Logger::INFO, __FILE__, __LINE__, "Ally()",
	            "Constructing Ally object..." ) ;

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

// /// DESTRUCTORS (Ally) ////////////////////////////////////////////////// //

Ally::~Ally()
{
	delete[] status_effects ;
	delete[] equipment ;
	delete pack ;
}

// /// ACCESSORS (Ally) //////////////////////////////////////////////////// //

// /// MUTATORS (Ally) ///////////////////////////////////////////////////// //

/*
 * Rerolls the ally's vital statistics.
 */
void Ally::reroll( void )
{
    log->write( Logger::INFO, __FILE__, __LINE__, "Ally::reroll()",
                "Rerolling ally's statistics..." ) ;

    xplevel = 1 ; xplevel_d = 0 ; xp = 0 ;

    str = Ally::rerollStat() ;
    dex = Ally::rerollStat() ;
    vit = Ally::rerollStat() ;
    wil = Ally::rerollStat() ;

    str_d = dex_d = vit_d = wil_d = 0 ;

    for( int i = 0 ; i < STATUS_EFFECTS ; i++ )
        status_effects[i] = 0 ;

    hunger = STARTING_HUNGER ;
    this->resetStepsToHeal() ;

    maxhp = ROGUE_HIT_DIE ;
    maxhp_d = 0 ;
    hp = this->getMaxHP() ;
}

// /// OTHER PUBLIC FUNCTIONS (Ally) /////////////////////////////////////// //

/*
 * Initiates display of total inventory.  The UI begins with a menu allowing
 * the player to choose an item filter.  Once a filter is selected, the
 * filtered version of the function is called.
 */
Item * Ally::displayInventory(void)
{
	return this->displayInventory( "Inventory" ) ;
}

Item * Ally::displayInventory( string h )
{
	string func = "Ally::displayInventory()" ; // debug

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
Item * Ally::displayInventory( char filter )
{
	return this->displayInventory( filter, "Inventory" ) ;
}

Item * Ally::displayInventory( char filter, string h )
{
	string func = "Ally::displayInventory(char)" ;

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
void Ally::displayStatus(void)
{
	log->write( Logger::TRACE, __FILE__, __LINE__, "Ally::displayStatus()",
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
void Ally::levelUp(void)
{
	string func = "Ally::levelUp()" ; // debug

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
void Ally::manageBasics(void)
{
	string func = "Ally::manageBasics()" ; // debug

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
void Ally::manageBasics( unsigned short points )
{
	string func = "Ally::manageBasics(ushort)" ; // debug

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
unsigned short Ally::manageFood(void)
{
	string func = "Ally::manageFood()" ; // debug

	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Selecting food to eat..." ) ;

	Item *food = this->displayInventory( Item::catToChar(FOOD), "Eat what?" ) ;

	if( food != NULL ) return this->eat(food) ;
	else return 0 ;
}

/*
 * Displays general equipment management UI.  Return value indicates whether
 * any changes to equipment were made.
 */
void Ally::manageEquipment(void)
{
	string func = "Ally::manageEquipment()" ; // debug

	log->write( Logger::INFO, __FILE__, __LINE__, func,
	            "Presenting equipment management UI..." ) ;

	bool changed = false ;
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
				if( this->manageArmor() ) changed = true ;
				break ;
			case 'W':
			case 'w':
				if( this->manageWeapons() ) changed = true ;
				break ;
			case 'R':
			case 'r':
				if( this->manageRings() ) changed = true ;
				break ;
			case 'N':
			case 'n':
				if( this->manageNecklaces() ) changed = true ;
				break ;
			default: ;
		}

		delete c ;
	} while( !done ) ;

	delete wChoices ;
	delete inv ;
	refresh() ;

	return changed ;
}

/*
 * Displays an armor management UI for the rogue.  Return value indicates that
 * a change was made.
 */
bool Ally::manageArmor(void)
{
    string func = "Ally::manageArmor()" ; // debug

    log->write( Logger::INFO, __FILE__, __LINE__, func,
                "Managing character's armor..." ) ;

    // Initialize three panes and inventory.
	Window *wEquipped = NULL ;
	Window *wStatus = NULL ;
    Inventory *invArmors = NULL ;

    bool done = false ;
	bool redraw = true ;
	bool changed = false ;

    do
    {
		if( redraw )
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

			redraw = false ;

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
					if( na != NULL )
					{
						equipArmor(na) ;
						changed = true ;
					}
					redraw = true ;
				}
				break ;
            case 'u':
            case 'U':
				if( !(pack->isFull()) )
				{
					equipArmor(NULL) ;
					changed = true ;
				}
				redraw = true ;
                break ;
            default: ;
        }

        delete c ;

    } while( !done ) ;

    delete wEquipped ;
    delete wStatus ;
    delete invArmors ;
    refresh() ;

    return changed ;
}

/*
 * Displays melee weapon management UI.  Return value indicates whether a change
 * was made.
 */
bool Ally::manageMeleeWeapons(void)
{
    string func = "Ally::manageMeleeWeapons()" ; // debug

    log->write( Logger::INFO, __FILE__, __LINE__, func,
                "Managing character's melee weapons..." ) ;

    // Initialize three panes and inventory.
	Window *wEquipped = NULL ;
	Window *wStatus = NULL ;
    Inventory *invMWeapons = NULL ;

    bool done = false ;
	bool redraw = true ;
	bool changed = false ;

    do
    {
		if( redraw )
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

			redraw = false ;

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
					if( na != NULL )
					{
						equipMeleeWeapon(na) ;
						changed = true ;
					}
					redraw = true ;
				}
				break ;
            case 'u':
            case 'U':
				if( !(pack->isFull()) )
				{
					equipMeleeWeapon(NULL) ;
					changed = true ;
				}
				redraw = true ;
                break ;
            default: ;
        }

        delete c ;

    } while( !done ) ;

    delete wEquipped ;
    delete wStatus ;
    delete invMWeapons ;
    refresh() ;
	return changed ;
}

/*
 * Displays ranged weapon management UI.
 */
bool Ally::manageRangedWeapons(void)
{
    string func = "Ally::manageRangedWeapons()" ; // debug

    log->write( Logger::INFO, __FILE__, __LINE__, func,
                "Managing character's ranged weapons..." ) ;

    // Initialize three panes and inventory.
	Window *wEquipped = NULL ;
	Window *wWStatus = NULL ;
	Window *wAStatus = NULL ;
    Inventory *invRWeapons = NULL ;
	Inventory *invAmmo = NULL ;

    bool done = false ;
	bool redraw = true ;
	bool changed = false ;

    do
    {
		if( redraw )
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

			redraw = false ;

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
						try
						{
							equipRangedWeapon(nrw) ;
							changed = true ;
						}
						catch( int e )
						{
							displayDialog( "Couldn't put away old ammo." ) ;
							refresh() ;
						}
					}
					redraw = true ;
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
						try
						{
							equipAmmunition(na) ;
							changed = true ;
						}
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
					redraw = true ;
				}
				break ;
            case 'u':
            case 'U':
				if( !(pack->isFull()) )
				{
					equipRangedWeapon(NULL) ;
					changed = true ;
				}
				redraw = true ;
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
	return changed ;
}

/*
 * Displays general weapon management UI.  This function merely presents an
 * initial menu allowing the player to select melee or ranged weapons; once the
 * choice is made, the appropriate subfunction manageMeleeWeapons() or
 * manageRangedWeapons() is called.  Return value indicates that any changes to
 * weapons were made.
 */
bool Ally::manageWeapons(void)
{
	string func = "Ally::manageWeapons()" ; // debug

    log->write( Logger::INFO, __FILE__, __LINE__, func,
                "Managing character's weapons..." ) ;

	// Start here with a "[M]elee or [R]anged" dialog, then do it just like
	// the armor management UI.
	Window *wChoice = new Window( 3, 31 ) ;
	wChoice->setHeader( "Select Weapon Type" ) ;
	wChoice->mvwstr( 1, 2, "[M]elee | [R]anged | E[x]it" ) ;
	wChoice->refresh() ;

	bool done = false ;
	bool changed = false ;

	do
	{
		Command *c = new Command(false) ;
		char cc = c->getCharCode() ;

		switch(cc)
		{
			case 'm':
			case 'M':
				changed = this->manageMeleeWeapons() ;
				done = true ;
				break ;
			case 'r':
			case 'R':
				changed = this->manageRangedWeapons() ;
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

	return changed ;
}

/*
 * Displays potion-quaffing UI.  The function returns a pointer to the selected
 * potion.
 */
Item * Ally::managePotions(void)
{
	string func = "Ally::managePotions()" ; // debug

	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Selecting potion to quaff..." ) ;

	return this->displayInventory( Item::catToChar(POTION), "Quaff what?" ) ;
}

/*
 * Displays scroll-reading UI.  The function returns a pointer to the selected
 * scroll.
 */
Item * Ally::manageScrolls(void)
{
	string func = "Ally::manageScrolls()" ; // debug

	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Selecting scroll to read..." ) ;

	return this->displayInventory( Item::catToChar(SCROLL), "Read what?" ) ;
}

/*
 * Displays ring management UI.  Return value indicates whether a change was
 * made to equipped rings.
 */
bool Ally::manageRings(void)
{
    string func = "Ally::manageRings()" ; // debug

    log->write( Logger::INFO, __FILE__, __LINE__, func,
                "Managing character's rings..." ) ;

    // Initialize three panes and inventory.
	Window *wEquipped = NULL ;
	Window *wLRing = NULL ;
	Window *wRRing = NULL ;
    Inventory *invRings = NULL ;

    bool done = false ;
	bool redraw = true ;
	bool changed = false ;

    do
    {
		if( redraw )
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

			redraw = false ;

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
					if( nlr != NULL )
					{
						equipRing( nlr, false ) ;
						changed = true ;
					}
					redraw = true ;
				}
				break ;
			case 'r':
			case 'R':
				if( equipment[EQ_RRING] == NULL
				 || ! (equipment[EQ_RRING])->isCursed() )
				{
					Item *nrr = invRings->selectItem() ;
					if( nrr != NULL )
					{
						equipRing( nrr, true ) ;
						changed = true ;
					}
					redraw = true ;
				}
				break ;
            case 'u':
            case 'U':
				if( !(pack->isFull()) )
				{
					equipRing(NULL,false) ;
					changed = true ;
				}
				if( !(pack->isFull()) )
				{
					equipRing(NULL,true) ;
					changed = true ;
				}
				redraw = true ;
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
	return changed ;
}

/*
 * Displays a necklace management UI for the rogue.  Return value indicates
 * whether a change was made to equipped necklace.
 */
bool Ally::manageNecklaces(void)
{
    string func = "Ally::manageNecklaces()" ; // debug

    log->write( Logger::INFO, __FILE__, __LINE__, func,
                "Managing character's necklaces..." ) ;

    // Initialize three panes and inventory.
	Window *wEquipped = NULL ;
	Window *wStatus = NULL ;
    Inventory *invNecklaces = NULL ;

    bool done = false ;
	bool redraw = true ;
	bool changed = false ;

    do
    {
		if( redraw )
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

			redraw = false ;

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
					if( nn != NULL )
					{
						equipNecklace(nn) ;
						changed = true ;
					}
					redraw = true ;
				}
				break ;
            case 'u':
            case 'U':
				if( !(pack->isFull()) )
				{
					equipNecklace(NULL) ;
					changed = true ;
				}
				redraw = true ;
                break ;
            default: ;
        }

        delete c ;

    } while( !done ) ;

    delete wEquipped ;
    delete wStatus ;
    delete invNecklaces ;
    refresh() ;

    return changed ;
}

// /// PROTECTED FUNCTIONS (Ally) ///////////////////////////////////////// //

unsigned short Ally::rerollStat( void )
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

/* ************************************************************************* */
