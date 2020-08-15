#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <deque>
#include <filesystem>

struct SDL_Renderer;
struct SDL_Texture;
struct SDL_Cursor;
struct SDL_Rect;

// manpat: really not a fan of this - this should be cmakes job :(
#ifdef __APPLE__
#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>
#else
#include <SDL.h>
#include <SDL_ttf.h>
#endif

#include "COLOR.h"

  //
 //   VARIABLES   ///////////////////////////////////////////////// ///////  //////   /////    ///     //      /
//

extern SDL_Renderer* RENDERER;
extern SDL_Texture* FONTMAP;

extern TTF_Font* FONT;
extern uint16_t FONT_CHRW;
extern uint16_t FONT_CHRH;

extern bool QUIT;
extern float FPS;

extern std::string CURRENT_PATH;
extern std::string CURRENT_FILE;

extern char KEY_TEXT[256];
extern int* KEY_TEXT_HOLD;
extern int KEY_TEXT_INT;

extern int16_t MOUSE_X;
extern int16_t MOUSE_Y;
extern int16_t MOUSE_PREVX;
extern int16_t MOUSE_PREVY;

extern int WINDOW_X;
extern int WINDOW_Y;
extern int WINDOW_W;
extern int WINDOW_H;

extern int DISPLAY_W;
extern int DISPLAY_H;

extern bool MOUSEBUTTON_LEFT;
extern bool MOUSEBUTTON_PRESSED_LEFT;
extern bool MOUSEBUTTON_MIDDLE;
extern bool MOUSEBUTTON_PRESSED_MIDDLE;
extern bool MOUSEBUTTON_RIGHT;
extern bool MOUSEBUTTON_PRESSED_RIGHT;
extern int16_t MOUSEWHEEL_X;
extern int16_t MOUSEWHEEL_Y;

extern bool KEYBOARD_CTRL;
extern bool KEYBOARD_PRESSED_CTRL;
extern bool KEYBOARD_SHIFT;
extern bool KEYBOARD_PRESSED_SHIFT;
extern bool KEYBOARD_ALT;
extern bool KEYBOARD_PRESSED_ALT;

extern bool KEYBOARD_SPACE;
extern bool KEYBOARD_PRESSED_SPACE;
extern bool KEYBOARD_ESC;
extern bool KEYBOARD_PRESSED_ESC;

// TOOL
enum TOOL {
	BRUSH,
	ERASER,
	PICKER,
	FILL,
	CANVAS,
};

extern uint16_t CURRENT_TOOL;
extern uint16_t TEMP_CURRENT_TOOL;


SDL_Cursor* create_system_cursor();

const uint8_t CHAR_NBSP = 0xffu; // no-break space
#define STR_NBSP "\xff"

const uint8_t CHAR_BLOCK = 0xdb; // █
#define STR_BLOCK "\xdb"

const uint8_t CHAR_BOXTL = 0xc9;// u8"╔";
#define STR_BOXTL "\xc9"
const uint8_t CHAR_BOXTR = 0xbb;// u8"╗";
#define STR_BOXTR "\xbb"
const uint8_t CHAR_BOXBL = 0xc8;//u8"╚";
#define STR_BOXBL "\xc8"
const uint8_t CHAR_BOXBR = 0xbc;//u8"╝";
#define STR_BOXBR "\xbc"
const uint8_t CHAR_BOXH = 0xcd;//u8"═";
#define STR_BOXH "\xcd"
const uint8_t CHAR_BOXV = 0xba;//u8"║";
#define STR_BOXV "\xba"

const uint8_t CHAR_LINEV = 0xb3;// │
#define STR_LINEV "\xb3"
const uint8_t CHAR_LINEVR = 0xc3;// ├
#define STR_LINEVR "\xc3"
const uint8_t CHAR_LINEBL = 0xc0;// └
#define STR_LINEBL "\xc0"

const uint8_t CHAR_ARWU = 0x18;
#define STR_ARWU "\x18"
const uint8_t CHAR_ARWD = 0x19;
#define STR_ARWD "\x19"
const uint8_t CHAR_ARWL = 0x1b;
#define STR_ARWL "\x1b"
const uint8_t CHAR_ARWR = 0x1a;
#define STR_ARWR "\x1a"

/*

	*	_0	_1	_2	_3	_4	_5	_6	_7	_8	_9	_a	_b	_c	_d	_e	_f
	0		☺	☻	♥	♦	♣	♠	•	◘	○	◙	♂	♀	♪	♫	☼
	1	►	◄	↕	‼	¶	§	▬	↨	↑	↓	→	←	∟	↔	▲	▼
	2	sp	!	"	#	$	%	&	'	(	)	*	+	,	-	.	/
	3	0	1	2	3	4	5	6	7	8	9	:	;	<	=	>	?
	4	@	A	B	C	D	E	F	G	H	I	J	K	L	M	N	O
	5	P	Q	R	S	T	U	V	W	X	Y	Z	[	\	]	^	_
	6	`	a	b	c	d	e	f	g	h	i	j	k	l	m	n	o
	7	p	q	r	s	t	u	v	w	x	y	z	{	|	}	~	del
	8	Ç	ü	é	â	ä	à	å	ç	ê	ë	è	ï	î	ì	Ä	Å
	9	É	æ	Æ	ô	ö	ò	û	ù	ÿ	Ö	Ü	¢	£	¥	₧	ƒ
	a	á	í	ó	ú	ñ	Ñ	ª	º	¿	⌐	¬	½	¼	¡	«	»
	b	░	▒	▓	│	┤	╡	╢	╖	╕	╣	║	╗	╝	╜	╛	┐
	c	└	┴	┬	├	─	┼	╞	╟	╚	╔	╩	╦	╠	═	╬	╧
	d	╨	╤	╥	╙	╘	╒	╓	╫	╪	┘	┌	█	▄	▌	▐	▀
	e	α	ß	Γ	π	Σ	σ	µ	τ	Φ	Θ	Ω	δ	∞	φ	ε	∩
	f	≡	±	≥	≤	⌠	⌡	÷	≈	°	∙	·	√	ⁿ	²	■	nbsp

	*/