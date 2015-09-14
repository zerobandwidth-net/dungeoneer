/*
 * item.cpp
 * You may play with this source however you like, as long as you don't remove
 * this comment, claim undue credit, or make a profit on it.  Enjoy.
 * CHANGE HISTORY
 * 2011-11-17
 *  Created file.
 */

#ifndef _DUNGEONEER_H_
#include "dungeoneer.h"
#endif

#include <iostream>
#include <string>
#include <sstream>
#include "command.h"
#include "debug.h"
#include "item.h"
#include "random.h"
#include "ui.h"
using namespace std ;

/* *** EXTERNAL REFERENCES ************************************************* */

extern Logger *log ; // debug.cpp

/* *** LOCAL DATA ********************************************************** */

// /// ITEM TEMPLATES ////////////////////////////////////////////////////// //

item_template placeholder =
	{ "", "", "", 0, false, 0, 0, 0, 0, 0 } ;

/*
 * Templates for armor types.  Note that the AC bonus of the armor is stored
 * in the dmgdie field (alias acbonus).
 */
item_template armor_templates[ARMOR_TYPES] =
{
	{ "", "", "clothes",          10,    false, 0, 0, 0, 0, 0 }, // 0
	{ "", "", "padded armor",     50,    false, 0, 1, 0, 0, 0 }, // 1
	{ "", "", "leather armor",    100,   false, 0, 2, 0, 0, 0 }, // 2
	{ "", "", "studded leathers", 250,   false, 0, 3, 0, 0, 0 }, // 3
	{ "", "", "hide armor",       325,   false, 0, 4, 0, 0, 0 }, // 4
	{ "", "", "scale mail",       500,   false, 0, 5, 0, 0, 0 }, // 5
	{ "", "", "chain mail",       1500,  false, 0, 6, 0, 0, 0 }, // 6
	{ "", "", "banded mail",      2500,  false, 0, 7, 0, 0, 0 }, // 7
	{ "", "", "half-plate mail",  6000,  false, 0, 8, 0, 0, 0 }, // 8
	{ "", "", "full plate mail",  15000, false, 0, 9, 0, 0, 0 }, // 9
};

/*
 * Each weapon subtype may have up to 10 varieties of weapon.
 */
item_template weapon_templates[(WEAPON_SUBTYPES * 10)] =
{
	{ "", "", "spiked glove",    10,  false, 1, 4, 20, 2, 0 }, // 0
	{ "", "", "dagger",          15,  false, 1, 4, 19, 2, 0 }, // 1
	{ "", "", "mace",            25,  false, 1, 6, 20, 2, 0 }, // 2
	{ "", "", "shortspear",      40,  false, 1, 6, 20, 3, 0 }, // 3
	{ "", "", "spear",           50,  false, 1, 8, 20, 3, 0 }, // 4
	placeholder, // 5
	placeholder, // 6
	placeholder, // 7
	placeholder, // 8
	placeholder, // 9
	{ "", "", "handaxe",         60,  false, 1, 6, 20, 3, 0 }, // 10
	{ "", "", "kukri",           80,  false, 1, 4, 18, 2, 0 }, // 11
	{ "", "", "shortsword",      100, false, 1, 6, 19, 2, 0 }, // 12
	{ "", "", "battleaxe",       100, false, 1, 8, 20, 3, 0 }, // 13
	{ "", "", "longsword",       150, false, 1, 8, 19, 2, 0 }, // 14
	{ "", "", "military pick",   160, false, 1, 6, 20, 4, 0 }, // 15
	{ "", "", "rapier",          200, false, 1, 6, 18, 2, 0 }, // 16
	{ "", "", "warhammer",       225, false, 1, 8, 20, 3, 0 }, // 17
	placeholder, // 18
	placeholder, // 19
	{ "", "", "dart",            5,   false, 1, 4, 20, 2, 0 }, // 20
	{ "", "", "javelin",         10,  false, 1, 6, 20, 2, 0 }, // 21
	{ "", "", "throwing dagger", 15,  false, 1, 4, 19, 2, 0 }, // 22
	placeholder, // 23
	placeholder, // 24
	placeholder, // 25
	placeholder, // 26
	placeholder, // 27
	placeholder, // 28
	placeholder, // 29
	{ "", "", "crossbow",        150, false, 1, 8, 19, 2, 0 }, // 30
	{ "", "", "shortbow",        300, false, 1, 6, 20, 3, 0 }, // 31
	{ "", "", "longbow",         750, false, 1, 8, 20, 3, 0 }, // 32
	placeholder, // 33
	placeholder, // 34
	placeholder, // 35
	placeholder, // 36
	placeholder, // 37
	placeholder, // 38
	placeholder, // 39
	{ "", "", "crossbow bolt",   1,   false, 0, 0, 0, 0, 0 }, // 40
	{ "", "", "arrow",           1,   false, 0, 0, 0, 0, 0 }, // 41
	placeholder, // 42
	placeholder, // 43
	placeholder, // 44
	placeholder, // 45
	placeholder, // 46
	placeholder, // 47
	placeholder, // 48
	placeholder, // 49
};

// The name of a potion should be a noun referring to the condition imparted to
// the target.  The dice specifications for potions define effect duration.
// The cmult field indicates whether the effect should be multiplied by the
// level of the target.
item_template potion_templates[POTION_TYPES] =
{
	{ "", "", "empowerment",         0, false, 0, 0, 0, 0, 0 },
	{ "", "", "restoration",         0, false, 0, 0, 0, 0, 0 },
	{ "", "", "greater restoration", 0, false, 0, 0, 0, 0, 0 },
	{ "", "", "healing",             0, false, 1, 4, 0, 1, 1 },
	{ "", "", "greater healing",     0, false, 1, 8, 0, 1, 1 },
	{ "", "", "experience",          0, false, 0, 0, 0, 0, 0 },
	{ "", "", "creature detection",  0, false, 0, 0, 0, 0, 0 },
	{ "", "", "object detection",    0, false, 0, 0, 0, 0, 0 },
	{ "", "", "levitation",          0, false, 2, 10, 0, 0, 10 },
	{ "", "", "haste",               0, false, 1, 10, 0, 1, 0 },
	{ "", "", "true sight",          0, false, 0, 0, 0, 0, 0 },
	{ "", "", "sleep",               0, false, 1, 10, 0, 1, 0 },
	{ "", "", "confusion",           0, false, 1, 10, 0, 1, 10 },
	{ "", "", "poison",              0, false, 3, 6, 0, 0, 6 },
	{ "", "", "blindness",           0, false, 5, 10, 0, 0, 25 },
	{ "", "", "hallucination",       0, false, 5, 10, 0, 0, 25 }
};

// These messages would be displayed when a character quaffs a potion.  They
// must be formulated such that the first word is the character's name.  When a
// potion is thrown at a monster, and the effect is triggered, this text may
// likewise be appended to a monster's name.
string potionUseMessages[POTION_TYPES] =
{
	" looks more confident than before.",        // P_STAT_BOOST
	" is warmed by the hearty brew.",            // P_LSR_RESTO
	" feels completely cleansed.",               // P_GRT_RESTO
	"'s wounds close with a faint blue glow.",   // P_HEALING
	"'s wounds close with a bright blue glow.",  // P_GRT_HEALING
	" has risen to a new level of experience.",  // P_LEVELUP
	"'s eyes glow red briefly.",                 // P_SEE_MONS
	"'s eyes glow gold briefly.",                // P_SEE_STUFF
	" begins to float above the ground.",        // P_LEVITATE
	" begins to move faster.",                   // P_HASTE
	"'s eyes glow orange briefly.",              // P_TRUESIGHT
	" falls asleep.",                            // P_SLEEP
	" seems disoriented.",                       // P_CONFUSE
	" looks ill.",                               // P_POISON
	" begins flailing about blindly.",           // P_BLIND
	"'s eyes flash briefly in many colors."      // P_LSD
};

// The first sixteen of these are the standard 16-color palette.  The others are
// taken from the XKCD color survey, http://xkcd.com/color/rgb/.
string potion_colors[PCOLORS] =
{
 // 16-color colors
 "black", "burgundy", "green", "brown", "navy", "purple", "teal", "creamy",
 "gray", "red", "lime", "yellow", "blue", "magenta", "cyan", "white",

 // XKCD colors (still 8 to a row)
 "pink", "orange", "violet", "turquoise", "lavender", "tan", "aqua", "mauve",
 "maroon", "olive", "salmon", "beige", "lilac", "peach", "periwinkle", "indigo",
 "mustard", "rose", "plum", "khaki", "taupe", "chartreuse", "mint", "sandy",
 "seafoam", "slate", "rust", "cerulean", "ochre", "crimson", "eggplant", "puce",
 "sage", "sienna", "coral", "grape", "azure", "wine", "cobalt", "scarlet",
 "orchid", "jade", "umber", "aubergine", "avocado", "cerise", "blush", "amber"
};

// The name of a scroll should be a verb related to the action that the scroll's
// spell will perform.  Some are spells taken from other game systems.  The
// dice specifications for scrolls define effect duration, which is generally
// multiplied by the caster's level (unlike potions).  The cmult field is used
// to indicate whether the duration should be multiplied by the caster's level.
item_template scroll_templates[SCROLL_TYPES] =
{
	{ "", "", "sanctify",        0, false, 0, 0, 0, 0, 0 },
	{ "", "", "harden",          0, false, 0, 0, 0, 0, 0 },
	{ "", "", "sharpen",         0, false, 0, 0, 0, 0, 0 },
	{ "", "", "identify",        0, false, 0, 0, 0, 0, 0 },
	{ "", "", "remove curse",    0, false, 0, 0, 0, 0, 0 },
	{ "", "", "paralyze",        0, false, 1, 4, 0, 1, 0 },
	{ "", "", "sleep",           0, false, 1, 10, 0, 1, 0 },
	{ "", "", "frighten",        0, false, 1, 4, 0, 1, 0 },
	{ "", "", "summon creature", 0, false, 0, 0, 0, 0, 0 },
	{ "", "", "aggravate",       0, false, 0, 0, 0, 0, 0 },
	{ "", "", "find the path",   0, false, 0, 0, 0, 0, 0 },
	{ "", "", "teleport",        0, false, 0, 0, 0 ,0, 0 }
};

// Syllables should be fewer than five characters each, and are defined in
// groups of 26, one per letter.
string syllables[STSYLLABLES] =
{
 "aleph","bar","crunk","derp","ecto","foo","gtfac","hurr","icky","jedi",
 "kinko","largo","mizar","nyan","omg","plotz","qidar","ringo","snorf","tarpl",
 "unqyl","vash","wigby","xorn","yalu","zerg",
 "ashwn","balis","calyf","dwhel","escot","fhars","gmilr","hobrq","ingr","jusco",
 "kavis","linus","myrln","nsfw","omegl","pshaa","quind","raesa","shath","thahn",
 "utar","vikap","winkl","xyzzy","ying","zerth"
};

// The name of a wand is a participle.  The dmgrolls, dmgdie, and bonus fields
// indicate the duration of the wand's effect or the damage done by the effect,
// where applicable.  The cthreat field indicates the number of charges the
// wand should start with.  The cmult field indicates whether the wand's effect
// should be multiplied by the user's level.
item_template wand_templates[WAND_TYPES] =
{
	{ "", "", "hastening",    0, false, 1, 6, 6, 1, 6 },
	{ "", "", "retarding",    0, false, 1, 6, 6, 1, 6 },
	{ "", "", "stultifying",  0, false, 1, 10, 6, 1, 10 },
	{ "", "", "befuddling",   0, false, 1, 6, 6, 1, 6 },
	{ "", "", "poisoning",    0, false, 2, 6, 6, 1, 6 },
	{ "", "", "enervating",   0, false, 1, 6, 6, 1, 3 },
	{ "", "", "polymorphing", 0, false, 0, 0, 6, 0, 0 },
	{ "", "", "missiles",     0, false, 1, 4, 30, 1, 1 },
	{ "", "", "cancelling",   0, false, 0, 0, 9, 0, 0 }
};

// Wand materials are either wood or metal.
string w_materials[WMATERIALS] =
{
	"aluminum", "birch", "cedar", "dogwood", "elm", "feldspar", "gold",
	"hickory", "iron", "juniper", "chrome", "laburnum", "mahogany", "nickel",
	"oak", "platinum", "cobalt", "redwood", "silver", "teak", "yew",
	"vallenwood", "walnut", "steel", "hyacinth", "zinc"
};

// The name of a ring is a noun.  If it grants a statistic bonus, that bonus
// will be stored in the bonus field.
item_template ring_templates[RING_TYPES] =
{
	{ "", "", "stealth", 0, false, 0, 0, 0, 0, 0 },
	{ "", "", "teleportation", 0, false, 0, 0, 0, 0, 0 },
	{ "", "", "regeneration", 0, false, 0, 0, 0, 0, 0 },
	{ "", "", "sustenance", 0, false, 0, 0, 0, 0, 0 },
	{ "", "", "resistance to poison", 0, false, 0, 0, 0, 0, 0 },
	{ "", "", "resistance to petrification", 0, false, 0, 0, 0, 0, 0 },
	{ "", "", "strength", 0, false, 0, 0, 0, 0, 0 },
	{ "", "", "dexterity", 0, false, 0, 0, 0, 0, 0 },
	{ "", "", "vitality", 0, false, 0, 0, 0, 0, 0 },
	{ "", "", "wisdom", 0, false, 0, 0, 0, 0, 0 },
	{ "", "", "true sight", 0, false, 0, 0, 0, 0, 0 },
	{ "", "", "protection from corrosion", 0, false, 0, 0, 0, 0, 0 },
	{ "", "", "searching", 0, false, 0, 0, 0, 0, 0 }
};

// Necklaces, like rings, are named by nouns.
item_template necklace_templates[NECKLACE_TYPES] =
{
	{ "", "", "escapism", 0, false, 0, 0, 0, 0, 0 },
	{ "", "", "toughness", 0, false, 0, 0, 0, 0, 0 },
	{ "", "", "deflection", 0, false, 0, 0, 0, 0, 0 },
	{ "", "", "wizardry", 0, false, 0, 0, 0, 0, 0 }
};

// The list of jewelry materials is drawn from the "materials" list in the
// "Jewellery" template on Wikipedia, 21 November 2011, and are listed 5 per
// line.
string j_materials[JMATERIALS] =
{
	"gold", "palladium", "platinum", "rhodium", "silver",
	"electrum", "shakudo", "brass", "bronze", "copper",
	"pewter", "titanium", "aventurine", "agate", "alexandrite",
	"amethyst", "aquamarine", "carnelian", "citrine", "diamond",
	"diopside", "emerald", "garnet", "jade", "jasper",
	"lapis lazuli", "larimar", "malachite", "marcasite", "moonstone",
	"obsidian", "onyx", "opal", "peridot", "quartz",
	"ruby", "sapphire", "sodalite", "sunstone", "tanzanite",
	"tiger's-eye", "topaz", "tourmaline", "turquoise", "amber",
	"copal", "coral", "jet", "pearl", "abalone"
};

/* *** LOCAL PROCEDURE PROTOTYPES ****************************************** */ 

void initializePotionColors(void) ;
void initializeScrollTitles(void) ;
void initializeWandMaterials(void) ;
void initializeRingMaterials(void) ;
void initializeNecklaceMaterials(void) ;

/* *** PROCEDURE DEFINITIONS *********************************************** */

void initializeItemTemplates(void)
{
	log->write( Logger::INFO, __FILE__, __LINE__, "initializeItemTemplates()",
	            "Initializing..." ) ;

	initializePotionColors() ;
	initializeScrollTitles() ;
	initializeWandMaterials() ;
	initializeRingMaterials() ;
	initializeNecklaceMaterials() ;
	return ;
}

void initializePotionColors(void)
{
	log->write( Logger::INFO, __FILE__, __LINE__, "initializePotionColors()",
	            "Initializing..." ) ;

	bool used_color[PCOLORS] ;
	unsigned short n ;

	for( unsigned short i = 0 ; i < PCOLORS ; i++ )
		used_color[PCOLORS] = false ;

	for( unsigned short i = 0 ; i < POTION_TYPES ; i++ )
	{
		do
		{
			n = randShort( 0, PCOLORS-1 ) ;
			if( !used_color[n] )
			{
				potion_templates[i].nname = potion_colors[n] ;
				used_color[n] = true ;
			}
		} while( !(potion_templates[i].nname).compare("") ) ;
	}

	return ;
}

void initializeScrollTitles(void)
{
	log->write( Logger::INFO, __FILE__, __LINE__, "initializeScrollTitles()",
	            "Initializing..." ) ;

	unsigned short i, j, s, n ;

	for( i = 0 ; i < SCROLL_TYPES ; i++ )
	{
		stringstream t ( stringstream::out ) ;

		s = randShort( 2, 3 ) ;
		for( j = 0 ; j < s ; j++ )
		{
			n = randShort( 0, STSYLLABLES-1 ) ;
			t << syllables[n]
			  << ( j < s-1 ? " " : "" )
			  ;
		}

		scroll_templates[i].nname = t.str() ;
	}

	return ;
}

void initializeWandMaterials(void)
{
	log->write( Logger::INFO, __FILE__, __LINE__, "initializeWandMaterials()",
	            "Initializing..." ) ;

	bool used_material[WMATERIALS] ;
	unsigned short i, n ;

	for( i = 0 ; i < WMATERIALS ; i++ ) used_material[i] = false ;

	for( i = 0 ; i < WAND_TYPES ; i++ )
	{
		while( !((wand_templates[i].nname).compare("")) )
		{
			n = randShort( 0, WMATERIALS-1 ) ;
			if( !used_material[n] )
			{
				wand_templates[i].nname = w_materials[n] ;
				used_material[n] = true ;
			}
		}
	}

	return ;
}

void initializeRingMaterials(void)
{
	log->write( Logger::INFO, __FILE__, __LINE__, "initializeRingMaterials()",
	            "Initializing..." ) ;

	bool used_material[JMATERIALS] ;
	unsigned short i, n ;

	for( i = 0 ; i < JMATERIALS ; i++ ) used_material[i] = false ;

	for( i = 0 ; i < RING_TYPES ; i++ )
	{
		while( !((ring_templates[i].nname).compare("")) )
		{
			n = randShort( 0, JMATERIALS-1 ) ;
			if( !used_material[n] )
			{
				ring_templates[i].nname = j_materials[n] ;
				used_material[n] = true ;
			}
		}
	}

	return ;
}

void initializeNecklaceMaterials(void)
{
	log->write( Logger::INFO, __FILE__, __LINE__, "initializeNecklaceMaterials()",
	            "Initializing..." ) ;

	bool used_material[JMATERIALS] ;
	unsigned short i, n ;

	for( i = 0 ; i < JMATERIALS ; i++ ) used_material[i] = false ;

	for( i = 0 ; i < NECKLACE_TYPES ; i++ )
	{
		while( !((necklace_templates[i].nname).compare("")) )
		{
			n = randShort( 0, JMATERIALS-1 ) ;
			if( !used_material[n] )
			{
				necklace_templates[i].nname = j_materials[n] ;
				used_material[n] = true ;
			}
		}
	}

	return ;
}

/* *** CLASS DEFINITION (Item) ********************************************* */

// /// STATIC FUNCTIONS (Item) ///////////////////////////// (none) /// //

unsigned short Item::charToCat( char tc )
{
	switch(tc)
	{
		case K_GOLD:		return GOLD ;			// 0
		case K_FOOD: 		return FOOD ;			// 1
		case K_ARMOR: 		return ARMOR ;			// 2
		case K_WEAPON: 		return WEAPON ;			// 3
		case K_POTION: 		return POTION ;			// 4
		case K_SCROLL:		return SCROLL ;			// 5
		case K_WAND:		return WAND ;			// 6
		case K_RING:		return RING ;			// 7
		case K_NECKLACE:	return NECKLACE ;		// 8
		case K_EQUIPMENT:	return K_EQUIPMENT ;	// 69
		default:			return K_ALLITEMS ;		// 42
	}
}

char Item::catToChar( unsigned short cat )
{
	switch(cat)
	{
		case GOLD:			return (char)(K_GOLD) ;
		case FOOD:			return (char)(K_FOOD) ;
		case ARMOR:			return (char)(K_ARMOR) ;
		case WEAPON:		return (char)(K_WEAPON) ;
		case POTION:		return (char)(K_POTION) ;
		case SCROLL:		return (char)(K_SCROLL) ;
		case WAND:			return (char)(K_WAND) ;
		case RING:			return (char)(K_RING) ;
		case NECKLACE:		return (char)(K_NECKLACE) ;
		case K_EQUIPMENT:	return (char)(K_EQUIPMENT) ;
		default:			return (char)(K_ALLITEMS) ;
	}
}

// /// CONSTRUCTORS (Item) ///////////////////////////////////////////////// //

/*
 * Default constructor.
 */
Item::Item(void)
{
	log->write( Logger::INFO, __FILE__, __LINE__, "Item()",
	            "Creating default item..." ) ;
	cname = "" ;
	nname = "" ;
	rname = "" ;
	category = kind = quantity = 0 ;
	value = 0 ;
	is_protected = is_cursed = is_identified = is_carried = false ;
	dmgrolls = dmgdie = cthreat = cmult = 0 ;
	bonus = 0 ;
	drow = dcol = 0 ;
	(void)getDiag() ;
}

/*
 * Copy constructor.
 */
Item::Item( Item *orig )
{
	log->write( Logger::INFO, __FILE__, __LINE__, "Item(Item*)",
	            "Copying item..." ) ;
	cname = orig->cname ;
	nname = orig->nname ;
	rname = orig->rname ;
	category = orig->category ;
	kind = orig->kind ;
	quantity = orig->quantity ;
	value = orig->value ;
	is_protected = orig->is_protected ;
	is_cursed = orig->is_cursed ;
	is_identified = orig->is_identified ;
	is_carried = orig->is_carried ;
	dmgrolls = orig->dmgrolls ;
	dmgdie = orig->dmgdie ;
	cthreat = orig->cthreat ;
	cmult = orig->cmult ;
	bonus = orig->bonus ;
	drow = orig->drow ;
	dcol = orig->dcol ;
	(void)getDiag() ;
}

/*
 * Explicit constructor.
 */
Item::Item( string x_cname, string x_nname, string x_rname,
            unsigned short x_category, unsigned short x_kind,
            unsigned short x_quantity, unsigned short x_value,
            bool x_is_protected, bool x_is_cursed, bool x_is_identified,
            bool x_is_carried,
            unsigned short x_dmgrolls, unsigned short x_dmgdie,
            unsigned short x_cthreat, unsigned short x_cmult,
            short x_bonus,
            unsigned short x_drow, unsigned short x_dcol )
{
	log->write( Logger::INFO, __FILE__, __LINE__, "Item(explicit)",
	            "Constructing explicit item." ) ;
	cname = x_cname ; // Copy of original.
	nname = x_nname ; // Copy of original.
	rname = x_rname ; // Copy of original.
	category = x_category ;
	kind = x_kind ;
	quantity = x_quantity ;
	value = x_value ;
	is_protected = x_is_protected ;
	is_cursed = x_is_cursed ;
	is_identified = x_is_identified ;
	is_carried = x_is_carried ;
	dmgrolls = x_dmgrolls ;
	dmgdie = x_dmgdie ;
	cthreat = x_cthreat ;
	cmult = x_cmult ;
	bonus = x_bonus ;
	drow = x_drow ;
	dcol = x_dcol ;
	(void)getDiag() ;
}

/*
 * Constructor based on item template; can also modify template data.
 */
Item::Item( item_template *itmpl, unsigned short icat, unsigned short ikind )
{
	this->initFromTemplate( itmpl, icat, ikind ) ;
}

/*
 * Constructor to make a basic version of a specific item.
 */
Item::Item( unsigned short icat, unsigned short ikind, unsigned short iquan )
{
	switch(icat)
	{
		case GOLD:
			cname = "" ;
			nname = "" ;
			rname = "" ;
			category = icat ;
			kind = ikind ;
			quantity = 0 ;
			value = iquan ;
			is_protected = is_cursed = is_identified = is_carried = false ;
			dmgrolls = dmgdie = cthreat = cmult = 0 ;
			bonus = 0 ;
			break ;
		case FOOD:
			cname = "" ;
			nname = "" ;
			rname = "" ;
			category = icat ;
			kind = ikind ;
			value = 0 ;
			is_protected = is_cursed = is_identified = is_carried = false ;
			dmgrolls = dmgdie = cthreat = cmult = 0 ;
			bonus = 0 ;
			break ;
		case ARMOR:
			this->initFromTemplate( armor_templates, icat, ikind ) ;
			break ;
		case WEAPON:
			this->initFromTemplate( weapon_templates, icat, ikind ) ;
			break ;
		case POTION:
			this->initFromTemplate( potion_templates, icat, ikind ) ;
			break ;
		case SCROLL:
			this->initFromTemplate( scroll_templates, icat, ikind ) ;
			break ;
		case WAND:
			this->initFromTemplate( wand_templates, icat, ikind ) ;
			break ;
		case RING:
			this->initFromTemplate( ring_templates, icat, ikind ) ;
			break ;
		case NECKLACE:
			this->initFromTemplate( necklace_templates, icat, ikind ) ;
			break ;
		default: ;
	}

	if( this->isStackable() ) this->quantity = iquan ;
}

// /// ACCESSORS (Item) //////////////////////////////////////////////////// //

short Item::getACBonus(void)
{
	if( category == ARMOR )
		return( short(this->acbonus) + this->bonus ) ;
	else if( category == NECKLACE && kind == N_ARMOR )
		return this->bonus ;
	else return 0 ;
}

short Item::getBonus(void) { return bonus ; }

unsigned short Item::getCategory(void) { return category ; }

unsigned short Item::getCritMultiplier(void) { return cmult ; }

/*
 * Returns a string describing an item.
 */
string Item::getDesc(void)
{
	return this->getDesc(false) ;
}

/*
 * Returns a string describing an item.  The Boolean parameter indicates
 * whether the first letter of the descriptive string (if it begins with a
 * letter) should be capitalized.
 */
string Item::getDesc( bool capitalize )
{
	string func = "Item::getDesc()" ;

	log->write( Logger::TRACE, __FILE__, __LINE__, func,
	            "Describing item..." ) ;
	(void)getDiag() ;

	stringstream desc ( stringstream::out ) ;
	string d ;

	switch( category )
	{
		case (GOLD):
			if( value == 1 )
			{
				desc << article(COIN,false) << COIN ;
			}
			else
				desc << value << " " << COIN << "s" ;
			break ;
		case (FOOD):
			switch( kind )
			{
				case (RATION):
					if( quantity == 1 )
						desc << "a ration of food" ;
					else
						desc << (unsigned short)(quantity) << " rations of food" ;
					break ;
				case (FRUIT):
					if( quantity == 1 )
						desc << article(FRUIT_NAME,false) << " " << FRUIT_NAME ;
					else
					{
						desc << (unsigned short)(quantity) << " "
						     << FRUIT_NAME << "s"
					         ;
					}
					break ;
				default:
					desc << "a pile of rotten food" ;
			}

			break ;
		case (ARMOR):
			if( cname.compare("") ) // Item has a called name.
			{
				desc << rname << " called " << cname 
				     << " [" << (short)(acbonus + bonus) << "]"
				     ;
			}
			else if( is_identified )
			{
				desc << ( is_protected ? "blessed " : "" )
				     << ( is_cursed ? "cursed " : "" )
				     << rname
				     << getBonusString(true,false)
				     << " [" << (short)(acbonus + bonus) << "]"
				     ;
			}
			else desc << rname ;
			break ;
		case (WEAPON):
			if( cname.compare("") ) // Item has a name.
			{
				if( quantity == 1 )
				{
					desc << article(rname,false) << " " << rname
					     << " named " << cname
					     << ( is_identified ? getWeaponDamageDesc() : "" )
					     ;
				}
				else
				{
					desc << (unsigned short)(quantity) << " " << rname
					     << "s named " << cname
					     << ( is_identified ? getWeaponDamageDesc() : "" )
					     ;
				}
			}
			else if( is_identified )
			{
				if( quantity == 1 )
				{
					desc << ( is_protected ? "blessed " : "" )
					     << ( is_cursed ? "cursed " : "" )
					     << rname
					     << getBonusString(true,false)
					     << " " << getWeaponDamageDesc()
					     ;
				}
				else
				{
					desc << (unsigned short)(quantity) << " "
					     << ( is_protected ? "blessed " : "" )
					     << ( is_cursed ? "cursed " : "" )
					     << rname << "s"
					     << getBonusString(true,false)
					     << " " << getWeaponDamageDesc()
					     ;
				}
			}
			else
			{
				if( quantity == 1 )
					desc << article(rname,false) << " " << rname ;
				else
					desc << (unsigned short)(quantity) << " " << rname << "s" ;
			}
			break ;
		case (POTION):
			if( is_identified )
			{
				if( quantity == 1 )
					desc << "a potion of " << rname ;
				else
				{
					desc << (unsigned short)(quantity)
					     << " potions of " << rname
					     ;
				}
			}
			else if( cname.compare("") ) // Item has a name.
			{
				if( quantity == 1 )
				{
					desc << article(nname,false) << " " << nname
					     << " potion called " << cname
					     ;
				}
				else
				{
					desc << (unsigned short)(quantity) << " " << nname
					     << " potions called " << cname
					     ;
				}
			}
			else
			{
				if( quantity == 1 )
					desc << article(nname,false) << " " << nname << " potion" ;
				else
				{
					desc << (unsigned short)(quantity) << " "
					     << nname << " potions"
					     ;
				}
			}
			break ;
		case (SCROLL):
			if( is_identified )
			{
				if( quantity == 1 )
					desc << "a scroll of " << rname ;
				else
				{
					desc << (unsigned short)(quantity)
					     << " scrolls of " << rname
					     ;
				}
			}
			else if( cname.compare("") )
			{
				if( quantity == 1 )
					desc << "a scroll called " << cname ;
				else
				{
					desc << (unsigned short)(quantity)
					     << " scrolls called " << cname
					     ;
				}
			}
			else
			{
				if( quantity == 1 )
					desc << "a scroll titled \"" << nname << "\"" ;
				else
				{
					desc << (unsigned short)(quantity)
					     << " scrolls titled \"" << nname << "\""
					     ;
				}
			}
			break ;
		case (WAND):
			if( is_identified )
			{
				desc << "a wand of " << rname
				     << " [" << (short)(wcharges) << "]"
				     ;
			}
			else if( wand_templates[kind].is_identified )
				desc << "a wand of " << rname ;
			else if( cname.compare("") )
			{
				desc << article(nname,false) << " " << nname
				     << " wand called " << cname ;
			}
			else
				desc << article(nname,false) << " " << nname << " wand" ;
			break ;
		case (RING):
			if( is_identified )
			{
				desc << ( is_cursed ? "a cursed ring of " : "a ring of " )
				     << rname << getBonusString(true,false)
			         ;
			}
			else if( ring_templates[kind].is_identified )
				desc << "a ring of " << rname ;
			else if( cname.compare("") )
			{
				desc << article(nname,false) << " " << nname
				     << " ring called " << cname
				     ;
			}
			else
				desc << article(nname,false) << " " << nname << " ring" ;
			break ;
		case (NECKLACE):
			if( is_identified )
			{
				desc << ( is_cursed ? "a cursed amulet of " : "an amulet of " )
				     << rname << getBonusString(true,false)
				     ;
			}
			else if( necklace_templates[kind].is_identified )
				desc << "an amulet of " << rname ;
			else if( cname.compare("") )
			{
				desc << article(nname,false) << " " << nname
				     << " amulet called " << cname ;
			}
			else
				desc << article(nname,false) << " " << nname << " necklace" ;
			break ;
		default:
			desc << "an unidentifiable pile of garbage" ;
	}

	d = desc.str() ;
	if( capitalize ) d = capitalizeString(d) ;

	log->write( Logger::TRACE, __FILE__, __LINE__, func, d ) ;
	return d ;
}

/*
 * Returns a string of diagnostic information about the item.  This is not
 * generally suitable for an in-game display; it is used only for logging.
 */
string Item::getDiag(void)
{
	string func = "Item::getDiag()" ;
	stringstream diagbuf ( stringstream::out ) ;

	diagbuf << "\"" << cname << "/" << nname << "/" << rname
	        << "\" (" << category
	        << "." << kind
	        << ") x" << quantity
	        << ", value " << value << ", ["
	        << ( is_protected ? "P" : "" )
	        << ( is_cursed ? "C" : "" )
	        << ( is_identified ? "I" : "" )
	        << ( is_carried ? "R" : "" )
	        << "] "
	        << "(" << dmgrolls << "d" << dmgdie << "/"
	        << cthreat << "/" << cmult << ") " << bonus
	        << " at (" << drow << "," << dcol << ")"
	        ;
	log->write( Logger::TRACE, __FILE__, __LINE__, func, diagbuf.str() ) ;
	return diagbuf.str() ;
}

/*
 * Returns the display character for this item.
 */
char Item::getDisplayChar(void)
{
	return Item::catToChar( this->category ) ;
}

/* Returns the object's "kind" value. */
unsigned short Item::getKind(void) { return kind ; }

/*
 * Returns the active name of an item.
 */
string Item::getName( bool capitalize )
{
	string n ;

	if( !(cname.compare("")) )
		n = cname ;
	else if( !is_identified )
	{
		switch( category )
		{
			case (GOLD):
			case (FOOD):
			case (ARMOR):
			case (WEAPON):
				n = this->rname ; break ;
			default:
				n = this->nname ;
		}
	}
	else // identified
		n = this->rname ;

	if( capitalize ) n = capitalizeString(n) ;

	return n ;
}

/*
 * Returns the remaining quantity of the item.
 */
unsigned short Item::getQuantity(void) { return this->quantity ; }
/*
 * Returns a message appropriate for display when an item of this kind is used
 * by a player character.  These messages would also apply to thrown potions
 * which strike creatures.
 */
string Item::getUseMessage(void)
{
	switch(this->category)
	{
		case POTION:	return potionUseMessages[this->kind] ;
		default: return "" ;
	}
}

unsigned short Item::getValue(void)
{
	return this->value ;
}

unsigned short Item::getWeaponSubtype(void)
{
	return ( kind - ( kind % 10 ) ) ;
}

bool Item::hasRoll(void) { return ( dmgrolls > 0 ) ; }

bool Item::isCursed(void) { return is_cursed ; }

/*
 * Returns true of this item is of the specified category and kind.
 */
bool Item::isItem( unsigned short c, unsigned short k )
{
	return( this->category == c && this->kind == k ) ;
}

/*
 * Returns true if the item has been specifically identified, or if this kind of
 * item has been identified based on item template data.
 */
bool Item::isRecognized(void)
{
	if( this->is_identified ) return true ;
	switch( this->category )
	{
		case GOLD: case FOOD: case WEAPON: case ARMOR: return true ;
		case POTION: return potion_templates[this->kind].is_identified ;
		case SCROLL: return scroll_templates[this->kind].is_identified ;
		case WAND: return scroll_templates[this->kind].is_identified ;
		case RING: return scroll_templates[this->kind].is_identified ;
		case NECKLACE: return necklace_templates[this->kind].is_identified ;
		default: return false ;
	}
}

/*
 * Indicates whether an item may be thrown as a weapon.
 */
bool Item::isThrowable(void)
{
	string func = "Item::isThrowable()" ; // debug

	if( category == WEAPON && getWeaponSubtype() == THROWING_WEAPON )
		return true ;
	if( category == POTION )
	{
		switch(kind)
		{
			case P_HEALING:
			case P_GRT_HEALING:
			case P_HASTE:
			case P_SLEEP:
			case P_CONFUSE:
			case P_POISON:
			case P_BLIND:
			case P_LSD:
				return true ;
			default:
				return false ;
		}
	}
	else return false ;
}

/*
 * Returns the result of the item's random die roll attributes, or 0 if the item
 * has no die roll attributes.
 */
unsigned short Item::dmgroll(void)
{
	unsigned short r = roll( this->dmgrolls, this->dmgdie, this->bonus ) ;
	log->lbuf << "Rolled " << this->getWeaponDamageDesc()
	          << ", got result " << r << "." ;
	log->writeBuffer( Logger::TRACE, __FILE__, __LINE__, "Item::roll()" ) ;
	return r ;
}

// /// MUTATORS (Item) ///////////////////////////////////////////////////// //

/*
 * Combines the given item into this one.
 */
bool Item::absorb( Item *that )
{
	if( !( this->couldStackWith(that) ) ) return false ;

	if( category == GOLD )
	{
		this->value += that->value ;
//		that->value = 0 ;
	}
	else
	{
		this->quantity += that->quantity ;
//		that->quantity = 0 ;
	}

	return true ;
}

/*
 * Marks the item as identified.  If appropriate, this will also flip the
 * is_identified bit in the corresponding item template table, so that all
 * future items of the same category and kind will be pre-identified.
 */
void Item::identify(void)
{
	is_identified = true ;

	switch(category)
	{
		case (POTION):
			potion_templates[kind].is_identified = true ; break ;
		case (SCROLL):
			scroll_templates[kind].is_identified = true ; break ;
		case (WAND):
			wand_templates[kind].is_identified = true ; break ;
		case (RING):
			ring_templates[kind].is_identified = true ; break ;
		case (NECKLACE):
			necklace_templates[kind].is_identified = true ; break ;
		default: ;
	}

	return ;
}

bool Item::makeRandom( unsigned short dlevel )
{
	category = randShort( 0, ITEM_TYPES - 1 ) ;

	return ( this->makeRandom( category, dlevel ) ) ;
}

/*
 * This function creates a random item of the specified category.  It will
 * always return true as long as the category specified is valid; otherwise,
 * it will return false, indicating bad input.
 */
bool Item::makeRandom( unsigned short icat, unsigned short dlevel )
{
	string func = "Item::makeRandom(ushort,ushort)" ; // debug

	log->write( Logger::INFO, __FILE__, __LINE__, func,
	            "Creating random item..." ) ;
	/*
	 * Each case within this switch block indicates a valid item creation.
	 * Thus, each case ends with a return, rather than a break.
	 */
	switch( icat )
	{
		case (GOLD):
			category = GOLD ;
			kind = 0 ;
			quantity = 1 ;
			value = randShort( (2*dlevel), (16*dlevel) ) ;
			break ;
		case (FOOD):
			category = FOOD ;
			kind = randShort( 0, FOOD_TYPES-1 ) ;
			quantity = 1 ;
			break ;
		case (ARMOR):
			category = ARMOR ;
			kind = randShort( 0, ARMOR_TYPES-1 ) ;
			cname = "" ;
			nname = rname = armor_templates[kind].rname ;
			quantity = 1 ;
			value = armor_templates[kind].value ;
			is_protected = false ;
			is_cursed = randPercent(CURSE_CHANCE) ;
			is_identified = false ; // Armor suits identify individually.
			acbonus = armor_templates[kind].acbonus ;
			bonus = ( randPercent(ITEM_BONUS_CHANCE) ? randShort(1,3) : 0 ) ;
			if( is_cursed ) bonus *= -1 ;
			break ;
		case (WEAPON):
			category = WEAPON ;
			kind = randShort( 0, WEAPON_SUBTYPES-1 ) * 10 ;
			switch( kind )
			{
				case (SIMPLE_WEAPON):
					kind += randShort( 0, SIMPLE_WEAPONS-1 ) ;
					quantity = 1 ;
					break ;
				case (MARTIAL_WEAPON):
					kind += randShort( 0, MARTIAL_WEAPONS-1 ) ;
					quantity = 1 ;
					break ;
				case (THROWING_WEAPON):
					kind += randShort( 0, THROWING_WEAPONS-1 ) ;
					quantity = randShort( 1, 12 ) ;
					break ;
				case (RANGED_WEAPON):
					kind += randShort( 0, RANGED_WEAPONS-1 ) ;
					quantity = 1 ;
					break ;
				case (AMMUNITION):
					kind += randShort( 0, AMMUNITIONS-1 ) ;
					quantity = randShort( 15, 30 ) ;
					break ;
				default: return false ; // Indicate an error.
			}
			cname = "" ;
			nname = rname = weapon_templates[kind].rname ;
			value = weapon_templates[kind].value ;
			is_protected = false ;
			is_cursed = randPercent(CURSE_CHANCE) ;
			is_identified = false ; // Weapons identify individually.
			dmgrolls = weapon_templates[kind].dmgrolls ;
			dmgdie = weapon_templates[kind].dmgdie ;
			cthreat = weapon_templates[kind].cthreat ;
			cmult = weapon_templates[kind].cmult ;
			bonus = ( randPercent(ITEM_BONUS_CHANCE) ? randShort(1,6) : 0 ) ;
			if( is_cursed ) bonus *= -1 ;
			break ;
		case (POTION):
			category = POTION ;
			kind = randShort( 0, POTION_TYPES-1 ) ;
			quantity = 1 ;
			copyFromTemplate( potion_templates[kind] ) ;
			break ;
		case (SCROLL):
			category = SCROLL ;
			kind = randShort( 0, SCROLL_TYPES-1 ) ;
			quantity = 1 ;
			copyFromTemplate( scroll_templates[kind] ) ;
			break ;
		case (WAND):
			category = WAND ;
			kind = randShort( 0, WAND_TYPES-1 ) ;
			quantity = 1 ;
			copyFromTemplate( wand_templates[kind] ) ;
			is_identified = false ; // Type may be identified, but wand isn't.
			break ;
		case (RING):
			category = RING ;
			kind = randShort( 0, RING_TYPES-1 ) ;
			quantity = 1 ;
			copyFromTemplate( ring_templates[kind] ) ;
			is_identified = false ; // Type may be identified, but ring isn't.
			switch(kind)
			{
				case (R_TELEPORT):
					is_cursed = true ;
					break ;
				case (R_STRBOOST):
				case (R_DEXBOOST):
				case (R_VITBOOST):
				case (R_WILBOOST):
					is_cursed = randPercent(CURSE_CHANCE) ;
					bonus = randShort(1,4) ;
					if( is_cursed ) bonus *= -1 ;
					break ;
				default: is_cursed = false ; bonus = 0 ;
			}
			break ;
		case (NECKLACE):
			category = NECKLACE ;
			kind = randShort( 0, NECKLACE_TYPES-1 ) ;
			quantity = 1 ;
			copyFromTemplate( necklace_templates[kind] ) ;
			is_identified = false ; // Type may be identified, but necklace isn't.
			switch(kind)
			{
				case (N_DMGREDUCTION):
				case (N_ARMOR):
					is_cursed = randPercent(CURSE_CHANCE) ;
					bonus = randShort(1,3) ;
					if( is_cursed ) bonus *= -1 ;
					break ;
				default: is_cursed = false ; bonus = 0 ;
			}
			break ;
		default:
			log->write( Logger::ERROR, __FILE__, __LINE__, func,
				"Invalid item constructed." ) ;
			this->getDiag() ;
			return false ;
	}

	this->getDiag() ; // logging
	return true ;
}

/*
 * Resets the item's bonus value.
 */
void Item::setBonus( short b ) { this->bonus = b ; return ; }

/*
 * Explicitly sets cursed status.
 */
bool Item::setCursed( bool c )
{
	this->is_cursed = c ;
	return c ;
}

/*
 * Sets the location of an item in the current dungeon level.  This might not
 * make sense given the direction that the dungeon design has taken.
 */
void Item::setLocation( unsigned short row, unsigned short col )
{
	drow = row ;
	dcol = col ;
	return ;
}

/*
 * Explicitly sets protected status.
 */
bool Item::setProtected( bool p )
{
	this->is_protected = p ;
	return this->is_protected ;
}

/*
 * Explicitly resets the quantity for an item.  The caller is responsible for
 * ensuring that this change makes sense.
 */
void Item::setQuantity( unsigned short q )
{
	this->quantity = q ;
	return ;
}

/*
 * Uses up a specified quantity of the referenced item, and returns the
 * remaining quantity.
 */
unsigned short Item::use( unsigned short q )
{
	string func = "Item::use(ushort)" ; // debug

	log->lbuf << "Using " << q
	          << ( q == 1 ? " unit" : " units" )
	          << " of an item containing " << this->quantity
	          << ( this->quantity == 1 ? " unit." : " units." )
	          ;
	log->writeBuffer( Logger::INFO, __FILE__, __LINE__, func ) ;

	if( q > this->quantity ) this->quantity = 0 ;
	else this->quantity -= q ;

	return this->quantity ;
}

// /// OTHER PUBLIC FUNCTIONS (Item) /////////////////////////////////////// //

bool Item::couldStackWith( Item *that )
{
	if( !(this->isStackable()) ) return false ;
	if( !(that->isStackable()) ) return false ;

	if( this->category == that->category
	 && this->kind == that->kind
	 && ( this->category != WEAPON
	   || ( this->is_protected == that->is_protected
	     && this->is_cursed == that->is_cursed
	     && this->bonus == that->bonus )
	    )
	  )
		return true ;
	else return false ;
}

bool Item::isStackable(void)
{
	switch(category)
	{
		case WEAPON:
			switch(getWeaponSubtype())
			{
				case THROWING_WEAPON:
				case AMMUNITION:
					return true ;
				default:
					return false ;
			}
		case GOLD:
		case FOOD:
		case POTION:
		case SCROLL:
			return true ;
		default:
			return false ;
	}
}

/*
 * Displays an item's statistics within an ncurses window.
 */
void Item::displayStatistics( Window *win )
{
	string func = "Item::displayStatistics(Window*)" ; // debug

	stringstream buf ( stringstream::out ) ;

	switch( category )
	{
		case GOLD:
			buf << capitalizeString(COIN) << "s" ;
			win->setHeader( buf.str() ) ; buf.str("") ;
			win->mvwstr( 1, 1, this->getDesc() ) ;
			win->mvwstr( 3, 1, "Currency." ) ;
			win->mvwstr( 4, 1, "Used to buy things." ) ;
			break ;
		case FOOD:
			win->setHeader( "Food" ) ;
			win->mvwstr( 1, 1, this->getDesc() ) ;
			win->mvwstr( 2, 1, "Eat when you're hungry." ) ;
			break ;
		case ARMOR:
			if( cname.compare("") ) // Show called name in header.
				win->setHeader( cname ) ;
			else
				win->setHeader( rname ) ;
			buf << "Armor Bonus: " << (acbonus+bonus)
			    << " (" << acbonus
				<< ( bonus >= 0 ? "+" : "" )
			    << bonus << ")"
			    ;
			win->mvwstr( 1, 1, buf.str() ) ; buf.str("") ;
			if( is_protected )
				win->mvwstr( 2, 1, "Blessed against damage." ) ;
			if( is_cursed )
				win->mvwstr( 2, 1, "Cursed.  Cannot remove." ) ;
			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Displayed armor statistics." ) ;
			break ;
		case WEAPON:
			if( cname.compare("") ) // Show called name in header.
				win->setHeader( cname ) ;
			else
				win->setHeader( rname ) ;
			if( getWeaponSubtype() == THROWING_WEAPON
			 || getWeaponSubtype() == AMMUNITION )
			{
				buf << "Quantity held: " << quantity ;
				win->mvwstr( 1, 1, buf.str() ) ; buf.str("") ;
			}
			buf << "Damage Rating: " << getWeaponDamageDesc() ;
			win->mvwstr( 2, 1, buf.str() ) ; buf.str("") ;
			if( is_protected )
				win->mvwstr( 3, 1, "Blessed against damage." ) ;
			if( is_cursed )
				win->mvwstr( 3, 1, "Cursed.  Cannot remove." ) ;
			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Displayed weapon statistics." ) ;
			break ;
		case POTION:
			if( cname.compare("") ) // Show called name in header.
				win->setHeader( cname ) ;
			else if( is_identified || potion_templates[kind].is_identified )
			{
				buf << "potion of " << rname ;
				win->setHeader( buf.str() ) ; buf.str("") ;
			}
			else
			{
				buf << nname << " potion" ;
				win->setHeader( buf.str() ) ; buf.str("") ;
			}
			buf << "Quantity held: " << quantity ;
			win->mvwstr( 1, 1, buf.str() ) ; buf.str("") ;
			if( isThrowable() )
				win->mvwstr( 2, 1, "Throwable." ) ;
			if( is_identified || potion_templates[kind].is_identified )
			{
				switch(kind)
				{
					case P_STAT_BOOST:
						win->mvwstr( 3, 1, "Raises a statistic." ) ;
						break ;
					case P_GRT_RESTO:
						win->mvwstr( 6, 1, "Seems more potent." ) ;
					case P_LSR_RESTO:
						win->mvwstr( 3, 1, "Can heal statistic damage." ) ;
						win->mvwstr( 4, 1, "Can heal experience damage." ) ;
						win->mvwstr( 5, 1, "Can heal max HP damage." ) ;
						break ;
					case P_HEALING:
					case P_GRT_HEALING:
						buf << "Heals " << dmgrolls << "d" << dmgdie
						    << ( bonus >= 0 ? "+" : "" ) << bonus
						    << " wounds"
							<< ( cmult > 0 ? " per level." : "." )
						    ;
						win->mvwstr( 3, 1, buf.str() ) ; buf.str("") ;
						break ;
					case P_LEVELUP:
						win->mvwstr( 3, 1, "Grants ancient knowledge." ) ;
						break ;
					case P_LEVITATE:
						buf << "Carries you aloft for "
						    << dmgrolls << "d" << dmgdie
						    << ( bonus >= 0 ? "+" : "" ) << bonus
						    << " turns"
						    << ( cmult > 0 ? " per level." : "." )
						    ;
						win->mvwstr( 3, 1, buf.str() ) ; buf.str("") ;
						break ;
					case P_HASTE:
						buf << "Speeds up movement for "
						    << dmgrolls << "d" << dmgdie
						    << ( bonus >= 0 ? "+" : "" ) << bonus
						    << " turns"
						    << ( cmult > 0 ? " per level." : "." )
						    ;
						win->mvwstr( 3, 1, buf.str() ) ; buf.str("") ;
						break ;
					case P_TRUESIGHT:
						win->mvwstr( 3, 1, "Enhances perception." ) ;
						break ;
					case P_SLEEP:
						buf << "Puts target to sleep for "
						    << dmgrolls << "d" << dmgdie
						    << ( bonus >= 0 ? "+" : "" ) << bonus
						    << " turns"
						    << ( cmult > 0 ? " per level." : "." )
						    ;
						win->mvwstr( 3, 1, buf.str() ) ; buf.str("") ;
						break ;
					case P_CONFUSE:
						buf << "Confuses target for "
						    << dmgrolls << "d" << dmgdie
						    << ( bonus >= 0 ? "+" : "" ) << bonus
						    << " turns"
						    << ( cmult > 0 ? " per level." : "." )
						    ;
						win->mvwstr( 3, 1, buf.str() ) ; buf.str("") ;
						break ;
					case P_POISON:
						buf << "Poisons target for "
						    << dmgrolls << "d" << dmgdie
						    << ( bonus >= 0 ? "+" : "" ) << bonus
						    << " turns"
						    << ( cmult > 0 ? " per level." : "." )
						    ;
						win->mvwstr( 3, 1, buf.str() ) ; buf.str("") ;
						break ;
					case P_BLIND:
						buf << "Blinds target for "
						    << dmgrolls << "d" << dmgdie
						    << ( bonus >= 0 ? "+" : "" ) << bonus
						    << " turns"
						    << ( cmult > 0 ? " per level." : "." )
						    ;
						win->mvwstr( 3, 1, buf.str() ) ; buf.str("") ;
						break ;
					case P_LSD:
						buf << "Causes hallucinations for "
						    << dmgrolls << "d" << dmgdie
						    << ( bonus >= 0 ? "+" : "" ) << bonus
						    << " turns"
						    << ( cmult > 0 ? " per level." : "." )
						    ;
						win->mvwstr( 3, 1, buf.str() ) ; buf.str("") ;
						break ;
					default: ;
				}
			}
			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Displayed potion statistics." ) ;
			break ;
		case SCROLL:
			if( cname.compare("") ) // Show called name in header.
				win->setHeader( cname ) ;
			else if( is_identified )
			{
				buf << "scroll of " << rname ;
				win->setHeader( buf.str() ) ; buf.str("") ;
			}
			else
			{
				buf << "\"" << nname << "\"" ;
				win->setHeader( buf.str() ) ; buf.str("") ;
			}
			buf << "Quantity held: " << quantity ;
			win->mvwstr( 1, 1, buf.str() ) ; buf.str("") ;
			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Displayed scroll statistics." ) ;
			break ;
		case WAND:
			log->write( Logger::WARNING, __FILE__, __LINE__, func,
			            "Not yet implemented." ) ;
			break ;
		case RING:
			if( cname.compare("") ) // Show called name in header.
				win->setHeader( cname ) ;
			else if( is_identified || ring_templates[kind].is_identified )
			{
				buf << rname << " (" << nname << ")" ;
				win->setHeader( buf.str() ) ; buf.str("") ;
			}
			else
				win->setHeader( nname ) ;
			if( is_protected )
				win->mvwstr( 2, 1, "Blessed against damage." ) ;
			if( is_cursed )
				win->mvwstr( 2, 1, "Cursed.  Cannot remove." ) ;
			if( is_identified || ring_templates[kind].is_identified )
			{
				switch(kind)
				{
					case R_STEALTH:
						win->mvwstr( 4, 1, "Conceals your presence." ) ;
						break ;
					case R_TELEPORT:
						win->mvwstr( 4, 1, "Transports you at random." ) ;
						break ;
					case R_REGENERATION:
						win->mvwstr( 4, 1, "Helps you heal your wounds." ) ;
						break ;
					case R_SUSTENANCE:
						win->mvwstr( 4, 1, "Keeps you from getting hungry." ) ;
						break ;
					case R_OPOISON:
						win->mvwstr( 4, 1, "Protects against poison." ) ;
						break ;
					case R_OSTONE:
						win->mvwstr( 4, 1, "Protects against petrification." ) ;
						break ;
					case R_STRBOOST:
						if( bonus >= 0 )
							buf << "Boosts your strength by " ;
						else
							buf << "Weakens your strength by " ;
						buf << bonus ;
						win->mvwstr( 4, 1, buf.str() ) ; buf.str("") ;
						break ;
					case R_DEXBOOST:
						if( bonus >= 0 )
							buf << "Boosts your dexterity by " ;
						else
							buf << "Weakens your dexterity by " ;
						buf << bonus ;
						win->mvwstr( 4, 1, buf.str() ) ; buf.str("") ;
						break ;
					case R_VITBOOST:
						if( bonus >= 0 )
							buf << "Boosts your vitality by " ;
						else
							buf << "Weakens your vitality by " ;
						buf << bonus ;
						win->mvwstr( 4, 1, buf.str() ) ; buf.str("") ;
						break ;
					case R_WILBOOST:
						if( bonus >= 0 )
							buf << "Boosts your willpower by " ;
						else
							buf << "Weakens your willpower by " ;
						buf << bonus ;
						win->mvwstr( 4, 1, buf.str() ) ; buf.str("") ;
						break ;
					case R_TRUESIGHT:
						win->mvwstr( 4, 1, "Grants you true sight." ) ;
						break ;
					case R_ORUST:
						win->mvwstr( 4, 1, "Protects your possessions." ) ;
						break ;
					case R_SEARCHING:
						win->mvwstr( 4, 1, "Reveals hidden secrets." ) ;
						break ;
					default: ;
				}
			}
			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Displayed ring statistics." ) ;
			break ;
		case NECKLACE:
			if( cname.compare("") ) // Show called name in header.
				win->setHeader( cname ) ;
			else if( is_identified || necklace_templates[kind].is_identified )
			{
				buf << rname << " (" << nname << ")" ;
				win->setHeader( buf.str() ) ; buf.str("") ;
			}
			else
				win->setHeader( nname ) ;
			if( is_protected )
				win->mvwstr( 2, 1, "Blessed against damage." ) ;
			if( is_cursed )
				win->mvwstr( 2, 1, "Cursed.  Cannot remove." ) ;
			if( is_identified || ring_templates[kind].is_identified )
			{
				switch(kind)
				{
					case N_ESCAPISM:
						win->mvwstr( 4, 1,
							"Allows you to escape from dungeons." ) ;
						break ;
					case N_DMGREDUCTION:
						if( bonus >= 0 )
							buf << "Allows you to shirk "
							    << bonus
							    << " damage when hit."
							    ;
						else
							buf << "Makes your wounds more severe by "
							    << abs(bonus) << "."
							    ;
						win->mvwstr( 4, 1, buf.str() ) ; buf.str("") ;
						break ;
					case N_ARMOR:
						if( bonus >= 0 )
							buf << "Increases " ;
						else
							buf << "Decreases " ;
						buf << "the strength of your armor by "
						    << abs(bonus)
							<< "."
						    ;
						win->mvwstr( 4, 1, buf.str() ) ; buf.str("") ;
						break ;
					case N_WIZARDRY:
						win->mvwstr( 4, 1, "Increses magical prowess." ) ;
						break ;
					default: ;
				}
			}
			log->write( Logger::TRACE, __FILE__, __LINE__, func,
			            "Displayed necklace statistics." ) ;
			break ;
		default:
			log->write( Logger::ERROR, __FILE__, __LINE__, func,
			            "Invalid item category." ) ;
	}

	return ;
}

// /// PRIVATE FUNCTIONS (Item) //////////////////////////////////////////// //

/*
 * This method copies an item's statistics directly from an item template.  The
 * function makes no attempt to validate that the template data is from the
 * appropriate template table; it simply copies values across.
 */
void Item::copyFromTemplate( item_template tmpl )
{
	cname = tmpl.cname ;
	nname = tmpl.nname ;
	rname = tmpl.rname ;
	value = tmpl.value ;
	is_identified = tmpl.is_identified ;
	dmgrolls = tmpl.dmgrolls ;
	dmgdie = tmpl.dmgdie ;
	cthreat = tmpl.cthreat ;
	cmult = tmpl.cmult ;
	bonus = tmpl.bonus ;
	return ;
}

string Item::getBonusString( bool fpad, bool ppad )
{
	stringstream bs ( stringstream::out ) ;

	if( bonus == 0 ) return "" ;

	bs << ( fpad ? " " : "" )
	   << ( bonus > 0 ? "+" : "" )
	   << ( bonus < 0 ? "-" : "" )
	   << abs((short)(bonus))
	   << ( ppad ? " " : "" )
	   ;

	return bs.str() ;
}

string Item::getWeaponDamageDesc(void)
{
	stringstream desc ( stringstream::out ) ;

	desc << "(" << (short)(dmgrolls) << "d" << (short)(dmgdie)
	     << getBonusString(false,false)
	     << "/" << (short)(cthreat) << "/" << (short)(cmult) << ")"
	     ;

	return desc.str() ;
}

void Item::initFromTemplate( item_template *itmpl,
                             unsigned short icat, unsigned short ikind )
{
	string func = "Item::initFromTemplate(item_template*,ushort,ushort)" ;
	log->write( Logger::INFO, __FILE__, __LINE__, func,
	            "Constructing item from template..." ) ;
	cname = itmpl[ikind].cname ; // Copy of template value.
	nname = itmpl[ikind].nname ; // Copy of template value.
	rname = itmpl[ikind].rname ; // Copy of template value.
	category = icat ;
	kind = ikind ;
	quantity = 1 ;
	value = itmpl[ikind].value ;
	is_protected = is_cursed = is_carried = false ;
	is_identified = itmpl[ikind].is_identified ;
	dmgrolls = itmpl[ikind].dmgrolls ;
	dmgdie = itmpl[ikind].dmgdie ;
	cthreat = itmpl[ikind].cthreat ;
	cmult = itmpl[ikind].cmult ;
	bonus = itmpl[ikind].bonus ;
	drow = dcol = 0 ;
	(void)getDiag() ;
}


/* ************************************************************************* */
