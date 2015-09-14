/*
 * inventory.cpp
 * You may play with this source however you like, as long as you don't remove
 * this comment, claim undue credit, or make a profit on it.  Enjoy.
 * CHANGE HISTORY
 * 2011-12-22 (0.00)
 *  Split from pack.cpp.
 */

#ifndef _DUNGEONEER_H_
#include "dungeoneer.h"
#endif

#include <sstream>
#include <string>
#include "command.h"
#include "debug.h"
#include "inventory.h"
#include "item.h"
#include "ui.h"
#include "window.h"
using namespace std ;

/* *** EXTERNAL REFERENCES ************************************************* */

extern Logger *log ; // debug.cpp

/* *** LOCAL DATA ********************************************************** */

const char letters[] =
	"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ" ;

/* *** LOCAL PROCEDURE PROTOTYPES ******************************* (none) *** */

/* *** PROCEDURE DEFINITIONS ************************************ (none) *** */

/* *** CLASS DEFINITION (Inventory) **************************************** */

// /// STATIC FUNCTIONS (Inventory) ///////////////////////////// (none) /// //

// /// CONSTRUCTORS (Inventory) //////////////////////////////////////////// //

Inventory::Inventory( unsigned short size )
{
	items = new InventoryNode[size] ;
	this->size = size ;
	iwin = NULL ;
}

// /// DESTRUCTOR (Inventory) ////////////////////////////////////////////// //

Inventory::~Inventory()
{
	delete[] items ;
	delete iwin ;
}

// /// ACCESSORS (Inventory) /////////////////////////////////////////////// //

/*
 * Allows the player to select an item from the currently-constructed inventory
 * window.  A pointer to the selected item is returned.
 */
Item* Inventory::selectItem(void)
{
	string func = "Inventory::selectItem()" ;

	log->write( Logger::INFO, __FILE__, __LINE__, func,
	            "Selecting item from inventory..." ) ;

	unsigned short ipage = 0 ; // Pages of items.
	unsigned short isel = 0 ; // Item highlighted in window.

	setWindowPage(0) ;
	iwin->refresh() ;
	
	for(;;) // User input loop broken from within.
	{
		Command *c = new Command() ; // Constructor fetches input.
		unsigned int cc = c->getCode() ;
		delete c ;

		// This isn't a switch() because some conditions apply to ranges of
		// values, which is kind of a pain when you have to define each
		// value explicitly.
		if( cc == K_CANCEL || cc == K_SPACE )
		{ // Cancel item selection and return a null item.
			log->write( Logger::INFO, __FILE__, __LINE__, func,
			            "Item selection cancelled." ) ;

			return NULL ;                                      // EXIT CONDITION
			// NOTE: This is obviously dangerous, as the calling function
			// will have to be capable of coping with null pointers coming
			// back from this function.
		}
		else if( cc == K_PAGEUP )
		{ // Scroll back to previous page of items, if any.
			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Selected previous page." ) ;

			if( ipage > 0 )
			{
				setWindowPage(--ipage) ;
				log->lbuf << "Back to page " << ipage << "..." ;
				log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
				iwin->refresh() ;
			}
		}
		else if( cc == K_PAGEDN )
		{ // Scroll forward to next page of items, if any.
			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Selected next page." ) ;

			if( size > ((ipage+1)*(iwin->getIHeight())) )
			{
				setWindowPage( ++ipage ) ;
				log->lbuf << "Forward to page " << ipage << "..." ;
				log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
				iwin->refresh() ;
			}
		}
		else if( cc == K_UARROW )
		{
			if( isel > 1 )
			{ // Highlight/select the previous item in the window.
				iwin->mvwstr( isel, 1, "  " ) ;
				--isel ;
				iwin->mvwstr( isel, 1, ">>" ) ;
				iwin->refresh() ;
			}
			else if( isel == 1 )
			{ // Deselect the first item.
				iwin->mvwstr( isel, 1, "  " ) ;
				isel = 0 ;
				iwin->refresh() ;
			}
		}
		else if( cc == K_DARROW )
		{
			if( isel == 0 )
			{ // Highlight/select the first item.
				isel = 1 ;
				iwin->mvwstr( isel, 1, ">>" ) ;
				iwin->refresh() ;
			}
			else if( ( isel < ( iwin->getHeight() - 2 ) )
			      && ( ( ipage * iwin->getIHeight() ) + isel < size )
			       )
			{ // Highlight/select the next item in the window.
				iwin->mvwstr( isel, 1, "  " ) ;
				++isel;
				iwin->mvwstr( isel, 1, ">>" ) ;
				iwin->refresh() ;
			}
		}
		else if( cc == K_ENTER )
		{
			if( isel > 0 )
			{
				log->write( Logger::TRACE, __FILE__, __LINE__, func,
				            "Attempting to select highlighted item..." ) ;

				unsigned int ii = // Item index is...
					( ipage * ( iwin->getIHeight() ) ) // ...the current page...
					+ ( isel - 1 ) // ...plus the index of the highlight.
					;

				if( ii < size )
				{
					log->write( Logger::INFO, __FILE__, __LINE__, func,
					            "Selected item:" ) ;
					(items[ii].item)->getDiag() ;

					return items[ii].item ;                   // EXIT CONDITION
				}
			}
			else return NULL ;                                // EXIT CONDITION
		}
		else if( cins( (char)cc, (char*)letters ) )
		{
			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Attempting to select item..." ) ;

			unsigned int ii = // Item index is...
				( ipage * ( iwin->getIHeight() ) ) // ...the current page...
				+ ( ltoi( (char)cc ) - 1 ) // ...plus index on current page.
				;

			if( ii < size )
			{
				log->write( Logger::INFO, __FILE__, __LINE__, func,
							"Selected item:" ) ;
				(items[ii].item)->getDiag() ;

				return items[ii].item ;                       // EXIT CONDITION
			}
		}
		else
		{
			log->lbuf << "Ignoring invalid input: " << cc ;
			log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;
		}
	}
}

// /// MUTATORS (Inventory) //////////////////////////////////////////////// //

/*
 * Sets the item addressed by an inventory node, and returns its letter.
 */
char Inventory::setItem( unsigned short index, Item *item )
{
	items[index] = (InventoryNode){ letters[ index % (DROWS-2) ], item } ;
	return letters[ index % (DROWS-2) ] ;
}

/*
 * Sets up a generic inventory display window for this inventory object.  The
 * window will be centered on the main screen.
 */
Window* Inventory::setWindow(void)
{
	string func = "Inventory::setWindow()" ; // debug

	unsigned short iww = 0 ;
	unsigned short iwh = 0 ;

	// Set width based on longest item descriptor.
	for( unsigned short i = 0 ; i < size ; i++ )
	{
		string s = describeNode(i) ;
		if( ( s.length() + 2 ) > iww )
			iww = s.length() + 2 ;
	}

	// If window is wider than screen, limit window size to screen size.
    if( iww > (DCOLS-2) ) iww = DCOLS - 2 ;

    // If inventory is longer than screen height, limit window to screen height
    if( size > (DROWS-2) ) iwh = DROWS ;
    else iwh = size + 2 ;

    log->lbuf << "Window dimensions: ( " << iww << ", " << iwh << " )" ;
    log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;

	if( iwin != NULL ) delete iwin ;
	iwin = new Window( iwh, iww ) ;
    iwin->aset( COLOR_PAIR(7) ) ;
    iwin->setHeader( "Inventory" ) ;

	return this->iwin ;
}

/*
 * Sets up an inventory display window for this inventory object, given
 * explicit coordinates and size.
 */
Window* Inventory::setWindow( unsigned short iwy, unsigned short iwx,
                              unsigned short iwh, unsigned short iww )
{
	string func = "Inventory::setWindow([dims])" ; // debug

	// We assume that the Window::mvwstr() function will automatically detect
	// and truncate descriptors that are too long for the explicit window size.

	if( iwin != NULL ) delete iwin ;
	iwin = new Window( iwy, iwx, iwh, iww ) ;
    iwin->aset( COLOR_PAIR(7) ) ;
    iwin->setHeader( "Inventory" ) ;

    return this->iwin ;
}

/*
 * Sets window header.
 */
void Inventory::setWindowHeader( string h )
{
	if( iwin != NULL ) iwin->setHeader(h) ;
	return ;
}

/*
 * Updates the contents of an inventory menu by scrolling to the specified page
 * of items.  If no such page exists, no update is performed.  Returns true if
 * the window is updated, and false otherwise.  Note that inventory pages are
 * zero-indexed; think of the page number as an offset from the first page.
 */
bool Inventory::setWindowPage( unsigned short ipage )
{
	string func = "Inventory::setWindowPage(ushort)" ; // debug

	stringstream buf (stringstream::out) ;

	if( this->hasWindowPage(ipage) )
	{
		log->lbuf << "Set to page " << ipage ;
		log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, func ) ;

		iwin->clear() ;
//		iwin->prepare() ;

		for( unsigned short r = 0 ; r < iwin->getIHeight() ; r++ )
		{
			// If the window is sized artificially large, this will stop us
			// from overrunning the end of the item array.
			if( ( ( ipage * iwin->getIHeight() ) + r ) >= size ) break ;

			iwin->mvwstr( r+1, 1,
			              describeNode( ( ipage * iwin->getIHeight() ) + r ) ) ;
		}

		if( ipage > 0 ) buf << "[PgUp]" ;
		if( this->hasWindowPage(ipage+1) ) buf << "[PgDn]" ;
		iwin->setFooter(buf.str()) ;
//		iwin->prepare() ;
	}
}

// /// OTHER PUBLIC FUNCTIONS (Inventory) /////////////////////// (none) /// //

void Inventory::prepare(void)
{
	iwin->prepare(true) ;
	return ;
}

void Inventory::refresh(void)
{
	iwin->refresh() ;
	return ;
}

// /// PRIVATE FUNCTIONS (Inventory) /////////////////////////////////////// //

/*
 * Describes an InventoryNode object suitably for display within an Inventory
 * display window.  In theory, the need for this function should mean that the
 * InventoryNode data object should be a class instead of a struct, and would
 * include this function as a member.  However, there are other reasons to keep
 * the InventoryNode object as a simple struct, mostly having to do with
 * declaring arrays of them, with which many people might disagree, but frankly
 * I don't care.
 */
string Inventory::describeNode( unsigned short i )
{
	stringstream buf ( stringstream::out ) ;

	buf << "  (" << items[i].letter << ") "
	    << (items[i].item)->getDesc()
		<< "  "
		;

	log->write( Logger::TRACE, __FILE__, __LINE__, "Inventory:describeNode()",
	            buf.str() ) ;

	return buf.str() ;
}

/*
 * Returns true if the specified page of inventory items exists.
 */
bool Inventory::hasWindowPage( unsigned short p )
{
	if( iwin == NULL ) return false ;
	else return( size > ( p * iwin->getIHeight() ) ) ;
}

/* ************************************************************************* */
