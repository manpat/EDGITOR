#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <deque>

struct SDL_Renderer;
struct SDL_Texture;
struct SDL_Cursor;
struct FC_Font;
struct SDL_Rect;

// manpat: really not a fan of this - this should be cmakes job :(
#ifdef __APPLE__
#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>
#else
#include <SDL.h>
#include <SDL_ttf.h>
#endif

  //
 //   VARIABLES   ///////////////////////////////////////////////// ///////  //////   /////    ///     //      /
//


extern SDL_Renderer* RENDERER;
extern SDL_Texture* FONTMAP;

extern FC_Font* font;
extern FC_Font* font_under;
extern FC_Font* font_bold;

extern TTF_Font* FONT;
extern uint16_t FONT_CHRW;
extern uint16_t FONT_CHRH;

extern bool QUIT;
extern float FPS;

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

struct COLOR {
	uint8_t r, g, b, a;

	friend bool operator==(COLOR a, COLOR b) {
		return a.r == b.r && a.g == b.g
			&& a.b == b.b && a.a == b.a;
	}

	friend bool operator!=(COLOR a, COLOR b) {
		return !(a == b);
	}
};

// CANVAS
extern bool CANVAS_UPDATE;
extern float CANVAS_ZOOM;
extern float CANVAS_X;
extern float CANVAS_Y;
extern uint16_t CANVAS_W;
extern uint16_t CANVAS_H;
extern int16_t CANVAS_MOUSE_X;
extern int16_t CANVAS_MOUSE_Y;
extern int16_t CANVAS_MOUSE_CLICKX;
extern int16_t CANVAS_MOUSE_CLICKY;
extern int16_t CANVAS_MOUSE_CLICKPREVX;
extern int16_t CANVAS_MOUSE_CLICKPREVY;
extern int16_t CANVAS_MOUSE_PREVX;
extern int16_t CANVAS_MOUSE_PREVY;
extern int16_t CANVAS_MOUSE_CELL_X;
extern int16_t CANVAS_MOUSE_CELL_Y;
extern int16_t CANVAS_PITCH;

extern SDL_Texture* BG_GRID_TEXTURE;
extern int16_t CELL_W;
extern int16_t CELL_H;
extern int16_t BG_GRID_W;
extern int16_t BG_GRID_H;

extern float CANVAS_PANX;
extern float CANVAS_PANY;

extern float CANVAS_X_ANIM;
extern float CANVAS_Y_ANIM;
extern float CANVAS_W_ANIM;
extern float CANVAS_H_ANIM;
extern float CELL_W_ANIM;
extern float CELL_H_ANIM;

// LAYER
extern uint16_t CURRENT_LAYER;
extern COLOR* CURRENT_LAYER_PTR;
extern int16_t LAYER_UPDATE;
extern int16_t LAYER_UPDATE_X1;
extern int16_t LAYER_UPDATE_Y1;
extern int16_t LAYER_UPDATE_X2;
extern int16_t LAYER_UPDATE_Y2;

struct LAYER_INFO {
	SDL_Texture* texture;
	std::unique_ptr<COLOR[]> pixels;
	int16_t x;
	int16_t y;
	uint8_t alpha;
	SDL_BlendMode blendmode;
};

extern std::vector<LAYER_INFO> LAYERS;

// BRUSH
extern SDL_Texture* BRUSH_CURSOR_TEXTURE;
extern SDL_Texture* BRUSH_TEXTURE;
extern bool BRUSH_UPDATE;

extern int16_t BRUSH_X;
extern int16_t BRUSH_Y;
extern uint16_t BRUSH_W;
extern int16_t BRUSH_UPDATE_X1;
extern int16_t BRUSH_UPDATE_Y1;
extern int16_t BRUSH_UPDATE_X2;
extern int16_t BRUSH_UPDATE_Y2;
extern std::unique_ptr<COLOR[]> BRUSH_PIXELS;
extern COLOR BRUSH_COLOR;
extern COLOR UNDO_COLOR;
extern COLOR* BRUSH_CURSOR_PIXELS;
extern COLOR* BRUSH_CURSOR_PIXELS_CLEAR;
extern SDL_Rect BRUSH_CURSOR_PIXELS_CLEAR_RECT;
extern uint16_t BRUSH_CURSOR_PIXELS_CLEAR_POS;

struct BRUSH_DATA
{
	int x = 0;
	int y = 0;
	int w = 0;
	std::vector<uint8_t> alpha;

	BRUSH_DATA(int _w) : alpha(_w* _w)
	{
		this->w = _w;
		this->x = (-_w / 2);
		this->y = (-_w / 2);
	}

	void set(int p, uint8_t a)
	{
		alpha[p] = a;
	}
};

extern std::vector<std::unique_ptr<BRUSH_DATA>> BRUSH_LIST;
extern uint16_t BRUSH_LIST_POS;

//SDL_Texture* CANVAS_TEXTURE;// = SDL_CreateTexture(RENDERER, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, CANVAS_W, CANVAS_H);
//uint32_t* CANVAS_PIXELS;// = new uint32_t[CANVAS_W * CANVAS_H];

extern bool MOUSEBUTTON_LEFT;
extern bool MOUSEBUTTON_PRESSED_LEFT;
extern bool MOUSEBUTTON_MIDDLE;
extern bool MOUSEBUTTON_PRESSED_MIDDLE;
extern bool MOUSEBUTTON_RIGHT;
extern bool MOUSEBUTTON_PRESSED_RIGHT;

// TOOL
extern uint16_t CURRENT_TOOL;

// UI
extern int16_t UIBOX_IN;
extern int16_t UIBOX_PREVIN;
extern int16_t UIBOX_CLICKED_IN;
extern int16_t UIBOX_PANX;
extern int16_t UIBOX_PANY;
extern SDL_Texture* UI_TEXTURE_HUEBAR;
extern COLOR* UI_PIXELS_HUEBAR;

extern int16_t ELEMENT_IN;
extern int16_t ELEMENT_CLICKED_IN;

extern bool TEST_BOOL;

struct UIBOX_CHAR {
	uint8_t chr;
	COLOR col;
	COLOR bg_col = { 0,0,0,1 };
};

struct UIBOX_ELEMENT {
	std::string text;
	std::string over_text;
	bool over = false;
	bool sel = false;
	uint8_t type = 0; // 0 = button, >0 anything else
	bool* input_bool = nullptr;
	uint16_t* input_int = nullptr;
	uint16_t input_int_var = 0;
	bool is_pos = false;
	uint16_t px = 0;
	uint16_t py = 0;
};

struct UIBOX_INFO {
	bool creation_update = true;
	bool update = true;
	bool element_update = true;

	bool can_grab = true;
	bool in_grab = false;
	bool grab = false;

	bool can_shrink = true;
	bool in_shrink = false;
	bool shrink = false;

	std::vector<UIBOX_CHAR> charinfo;
	std::vector<UIBOX_ELEMENT> element;
	std::deque<uint16_t> update_stack;
	SDL_Texture* texture;
	uint16_t tex_w;
	uint16_t tex_h;
	uint16_t chr_w;
	uint16_t chr_h;
	uint16_t x;
	uint16_t y;
	uint16_t w;
	uint16_t h;
	uint8_t alpha;
	uint16_t update_tick = 0;
};

extern std::vector<UIBOX_INFO> UIBOXES;

extern UIBOX_INFO* UIBOX_TOOLS;
extern UIBOX_INFO* UIBOX_COLOR;
extern UIBOX_INFO* UIBOX_BRUSH;
extern UIBOX_INFO* UIBOX_CANVAS;

// UNDO
struct UNDO_DATA
{
	uint16_t layer = 0;
	uint16_t x = 0;
	uint16_t y = 0;
	uint16_t w;
	uint16_t h;
	std::vector<COLOR> undo_pixels;
	std::vector<COLOR> redo_pixels;

	UNDO_DATA(uint16_t _w, uint16_t _h)
		: w {_w}
		, h {_h}
		, undo_pixels(_w*_h)
		, redo_pixels(_w*_h)
	{
	}

	void set(uint16_t xx, uint16_t yy, COLOR prev_col, COLOR new_col)
	{
		int index = xx + yy * this->w;
		undo_pixels[index] = prev_col;
		redo_pixels[index] = new_col;
	}
};

extern COLOR COL_ACCENT;
extern COLOR COL_BGUPDATE;

extern std::vector<UNDO_DATA> UNDO_LIST;
extern uint16_t UNDO_POS;
extern uint16_t UNDO_UPDATE;
extern uint16_t UNDO_UPDATE_LAYER;
extern SDL_Rect UNDO_UPDATE_RECT;
extern COLOR UNDO_COL;

// PRESET COLORS
extern COLOR COL_WHITE;
extern COLOR COL_LTGRAY;
extern COLOR COL_GRAY;
extern COLOR COL_DKGRAY;
extern COLOR COL_BLACK;

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

const uint8_t CHAR_ARWU = 0x18;
#define STR_ARWU "\x18"
const uint8_t CHAR_ARWD = 0x19;
#define STR_ARWD "\x19"
const uint8_t CHAR_ARWL = 0x1b;
#define STR_ARWL "\x1b"
const uint8_t CHAR_ARWR = 0x1a;
#define STR_ARWR "\x1a"

extern bool DEBUG_BOOL;

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