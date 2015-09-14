/*
 * dungeoneer.h
 * You may play with this source however you like, as long as you don't remove
 * this comment, claim undue credit, or make a profit on it.  Enjoy.
 * CHANGE HISTORY
 * 2011-11-17
 *  Created file.
 */

#ifndef _DUNGEONEER_H_
#define _DUNGEONEER_H_

/* *** TWEAKS ************************************************************** */

#define CURSE_CHANCE 7 
#define DCOLS 80
#define DROWS 24
#define ITEM_BONUS_CHANCE 20
#define ITEM_NAME_LENGTH 32
#define MAX_DUNGEON_DEPTH 32

// /// DISPLAY CHARACTERS (V for visual) /////////////////////////////////// //

/* Conventional non-curses character set...
#define V_FLOOR '.'
#define V_VWALL '|'
#define V_HWALL '-'
#define V_ULCORNER '-'
#define V_URCORNER '-'
#define V_LLCORNER '-'
#define V_LRCORNER '-'
#define V_DOOR '+'
#define V_PASSAGE '#'
*/
// Curses-dependent character set...
#define V_FLOOR ACS_BULLET
#define V_VWALL ACS_VLINE
#define V_HWALL ACS_HLINE
#define V_ULCORNER ACS_ULCORNER
#define V_URCORNER ACS_URCORNER
#define V_LLCORNER ACS_LLCORNER
#define V_LRCORNER ACS_LRCORNER
#define V_DOOR ACS_PLUS
#define V_PASSAGE ACS_CKBOARD

#define V_PLAYER '@'
#define V_GOLD '*'
#define V_FOOD ':'
#define V_ARMOR ']'
#define V_WEAPON ')'
#define V_POTION '!'
#define V_SCROLL '?'
#define V_WAND '/'
#define V_RING '='
#define V_NECKLACE ','

#define V_TRAP '^'

// /// STATUS FLAG NAMETAGS //////////////////////////////////////////////// //
// Whereas "rogue" uses a single long integer to store status flags, this code
// uses an array of integers indicating the remaining duration of each status.
// Is this better?  I don't know, but that's just the way it is.  These
// constants define semantic names for indices in the array of status integers.
// The constant STATUS_EFFECTS is the number of possible status effects, and
// thus defines the size of the array.

enum StatusEffect
{
	HASTED, SLOWED, ASLEEP, CONFUSED, POISONED, BLINDED, HALLUCINATING,
	PARALYZED, STONED, SCARED, FLOATING, SEES_CREATURES, SEES_STUFF
} ;
#define STATUS_EFFECTS 13

/* ************************************************************************* */
#endif
