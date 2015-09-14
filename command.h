/*
 * command.h
 * You may play with this source however you like, as long as you don't remove
 * this comment, claim undue credit, or make a profit on it.  Enjoy.
 * CHANGE HISTORY
 * 2011-12-22
 *  Split from ui.h.
 */

#ifndef _DUNGEONEER_H_
#include "dungeoneer.h"
#endif

#ifndef _COMMAND_H_
#define _COMMAND_H_

#include <string>
#include <ncurses.h>
using namespace std ;

/* *** TWEAKS ************************************************************** */

// /// COMMAND CODES /////////////////////////////////////////////////////// //
// Command codes here are not necessarily display characters.  Where possible,
// the codes do correspond to display characters matching the key that was
// pressed by the player.  Where the character is not displayable, or the code
// is derived by a multi-key input, an abstracted code is assigned.  Commands
// are assembled by the Command class constructor.

// Ctrl+2 // 0
// Ctrl+A // 1
// Ctrl+B // 2
// Ctrl+C // 3
// Ctrl+D // 4
// Ctrl+E // 5
// Ctrl+F // 6
// Ctrl+G // 7
#define K_HELP 8 // Ctrl+H; also [Backspace]
// Ctrl+I // 9; also [Tab]
#define K_ENTER 10 // Ctrl+J; also [Enter] and Ctrl+M
// Ctrl+K // 11
// Ctrl+L // 12
// Ctrl+M is reported as 10 instead of 13; see above.
// Ctrl+N // 14
// Ctrl+O // 15
// Ctrl+P // 16
#define K_QUIT 17 // Ctrl+Q.  Brings up the "Quit?" dialog.
#define K_REDRAW 18 // Ctrl+R.  Redraws the screen.
#define K_SAVE 19 // Ctrl+S.  Brings up the save game dialog.
// Ctrl+T // 20
// Ctrl+U // 21
// Ctrl+V // 22
// Ctrl+W // 23
// Ctrl+X // 24
// Ctrl+Y // 25
// Ctrl+Z // 26
#define K_CANCEL 27 // [Esc]
// Ctrl+1 is reported as 1 in Linux and not at all in Windows/puTTY.
// Ctrl+2 is reported as null and equals 0; see above.
// Ctrl+3 is reported as escape and equals 27; see above.
// Ctrl+4 // 28; also Ctrl+\
// Ctrl+5 // 29; also Ctrl+]
// Ctrl+6 // 30; also Ctrl+^
// Ctrl+7 // 31; also Ctrl+_
// Ctrl+8 is reported as Ctrl+? and equals 127; see below.
// Ctrl+9 is reported as 9 in Linux and not at all in Windows/puTTY.
// Ctrl+0 is not reported.
// Ctrl+- shrinks terminal windows in Linux and is reported as Ctrl+_ (31) in Windows/puTTY.
// Ctrl+= is reported as = in Linux and not at all in Windows/puTTY.
#define K_SPACE 32 // ' ' [Space]
#define K_POTION 33 // '!'
// " // 34; @ is 64
// # // 35
// $ // 36
// % // 37
// & // 38
// ' // 39; * is 42
// ( // 40
#define K_WEAPON 41 // ')'
#define K_GOLD 42 // '*'
#define K_ALLITEMS 42 // '*' again
// + // 43
#define K_NECKLACE 44 // ','
// - // 45
// '.' (46) is remapped to special code K_REST (273)
#define K_WAND 47 // '/'
// 0 // 48
// 1 // 49
// 2 // 50
// 3 // 51
// 4 // 52
// 5 // 53
// 6 // 54
// 7 // 55
// 8 // 56
// 9 // 57
#define K_FOOD 58 // ':'
#define K_PICK_UP 59 // ';'
#define K_UPSTAIRS 60 // '<'
#define K_RING 61 // '='
#define K_DOWNSTAIRS 62 // '>'
#define K_SCROLL 63 // '?'
// @ // 64
// A // 65
#define K_RUN_SOUTHWEST 66 // 'B'
// C // 67
#define K_DISARM_TRAP 68 // 'D'
#define K_EQUIPMENT 69 // 'E'
// F // 70
// G // 71
#define K_RUN_WEST 72 // 'H'
// I // 73
#define K_RUN_SOUTH 74 // 'J'
#define K_RUN_NORTH 75 // 'K'
#define K_RUN_EAST 76 // 'L'
// M // 77
#define K_RUN_SOUTHEAST 78 // 'N'
// O // 79
// P // 80
// Q // 81
// R // 82
#define K_STATUS 83 // 'S'
// T // 84
#define K_RUN_NORTHEAST 85 // 'U'
// V // 86
#define K_WEAR_ARMOR 87 // 'W'
// X // 88
#define K_RUN_NORTHWEST 89 // 'Y'
// Z // 90
// [ // 91
// \ // 92
#define K_ARMOR 93 // ']'
// ^ // 94
// _ // 95
// ` // 96
#define K_APPRAISE 97 // 'a'
#define K_SOUTHWEST_VI 98 // 'b'
// c // 99
#define K_DROP_ITEM 100 // 'd'
#define K_EAT_FOOD 101 // 'e'
#define K_FIRE_WEAPON 102 // 'f'
// g // 103
#define K_WEST_VI 104 // 'h'
#define K_INVENTORY 105 // 'i'
#define K_SOUTH_VI 106 // 'j'
#define K_NORTH_VI 107 // 'k'
#define K_EAST_VI 108 // 'l'
// m // 109
#define K_SOUTHEAST_VI 110 // 'n'
// o // 111
// p // 112
#define K_QUAFF_POTION 113 // 'q'
#define K_READ_SCROLL 114 // 'r'
#define K_SEARCH 115 // 's'
#define K_THROW_ITEM 116 // 't'
#define K_NORTHEAST_VI 117 // 'u'
// v // 118
#define K_WIELD_WEAPON 119 // 'w'
// x // 119
#define K_NORTHWEST_VI 121 // 'y'
#define K_ZAP 122 // 'z'
// { // 123
// | // 124
// } // 125
// ~ // 126
// Ctrl+8 // 127; also Ctrl+?

#define K_NORTHWEST 271 // ^[[1~ [NP-Home] Indicate direction: northwest.
// ^[[2~ [Ins] and [NP-Ins] Intercepted and re-interpreted as K_INVENTORY (105).
#define K_REST 273 // ^[[3~ [Del] and [NP-Del] Rest for one turn.
#define K_SOUTHWEST 274 // ^[[4~ [NP-End] Indicate direction: southwest.
#define K_NORTHEAST 275 // ^[[5~ [PgUp] and [NP-PgUp] Indicate direction: northeast.
#define K_SOUTHEAST 276 // ^[[6~ [PgDn] and [NP-PgDn] Indicate direction: southeast.
#define K_NORTH 280 // ^[[A [UArr] and [NP-UArr] Indicate direction: north.
#define K_SOUTH 281 // ^[[B [DArr] and [NP-DArr] Indicate direction: south.
#define K_EAST 282 // ^[[C [RArr] and [NP-RArr] Indicate direction: east.
#define K_WEST 283 // ^[[D [LArr] and [NP-LArr] Indicate direction: west.
// ^[[G is the Windows/puTTY-only code for [NP-5]; it will be a synonym of [Del].

#define K_HOMEKEY 271 // Literal alias for "^[[1~" [NP-Home]
#define K_UARROW 280 // Literal alias for "^[[A" [UArr] and [NP-UArr]
#define K_PAGEUP 275 // Literal alias for "^[[5~" [PgUp] and [NP-PgUp]
#define K_LARROW 283 // Literal alias for "^[[D" [LArr] and [NP-LArr]
#define K_RARROW 282 // Literal alias for "^[[C" [RArr] and [NP-RArr]
#define K_ENDKEY 274 // Literal alias for "^[[4~" [NP-End]
#define K_DARROW 281 // Literal alias for "^[[B" [DArr] and [NP-DArr]
#define K_PAGEDN 276 // Literal alias for "^[[6~" [PgDn] and [NP-PgDn]
#define K_INSERT 272 // Literal alias for "^[[2~" [Ins] and [NP-Ins]
#define K_DELETE 273 // Literal alias for "^[[3~" [Del] and [NP-Del]

#define K_UNKNOWN 999 // Indicates an error.

/* *** COMPLEX TYPE DEFINITIONS ********************************* (none) *** */

/* *** GLOBAL PROCEDURE PROTOTYPES ****************************** (none) *** */

/* *** CLASS PROTOTYPES (needed for friendship) ***************** (none) *** */

/* *** CLASS DEFINITIONS *************************************************** */

/*
 * The Command class is a data structure that allows the game to process user
 * inputs based on expected escape sequences from non-printing keys, and a list
 * of pre-defined result codes (see COMMAND CODES above).
 */
class Command
{
	public:
		// Static fields...
		// Static functions...

		// Constructors...
		Command() ;
		Command( bool interpreted ) ;

		// Accessors...
		char getCharCode() ; // Returns first command character or 0.
		unsigned int getCode() ;
		unsigned short getCount() ;
		string str() ; // Returns the received string.

		// Mutators...
		// Other public functions...

	private:
		// Fields...
		unsigned short code ;
		unsigned short count ;
		unsigned short interruptible ;
		string received ;

		// Private functions...
		void init(bool) ;
};

/* ************************************************************************* */
#endif
