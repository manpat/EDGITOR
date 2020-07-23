#pragma once

  //
 //   VARIABLES   ///////////////////////////////////////////////// ///////  //////   /////    ///     //      /
//

SDL_Renderer* RENDERER;
SDL_Texture* FONTMAP;

FC_Font* font;
FC_Font* font_under;
FC_Font* font_bold;

TTF_Font* FONT;
uint16_t FONT_CHRW;
uint16_t FONT_CHRH;

bool QUIT = false;
float FPS = 0;

static char KEY_TEXT[256];
int* KEY_TEXT_HOLD = nullptr;
int KEY_TEXT_INT;

int16_t MOUSE_X;
int16_t MOUSE_Y;
int16_t MOUSE_PREVX;
int16_t MOUSE_PREVY;

int WINDOW_X;
int WINDOW_Y;
int WINDOW_W;
int WINDOW_H;

int DISPLAY_W;
int DISPLAY_H;

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
bool CANVAS_UPDATE = false;
float CANVAS_ZOOM = 1.0;
float CANVAS_X = 0.0;
float CANVAS_Y = 0.0;
uint16_t CANVAS_W = 512;
uint16_t CANVAS_H = 256;
int16_t CANVAS_MOUSE_X = 0;
int16_t CANVAS_MOUSE_Y = 0;
int16_t CANVAS_MOUSE_CLICKX = 0;
int16_t CANVAS_MOUSE_CLICKY = 0;
int16_t CANVAS_MOUSE_CLICKPREVX = 0;
int16_t CANVAS_MOUSE_CLICKPREVY = 0;
int16_t CANVAS_MOUSE_PREVX = 0;
int16_t CANVAS_MOUSE_PREVY = 0;
int16_t CANVAS_MOUSE_CELL_X = 0;
int16_t CANVAS_MOUSE_CELL_Y = 0;
int16_t CANVAS_PITCH = (sizeof(COLOR) * CANVAS_W);

SDL_Texture* BG_GRID_TEXTURE;
int16_t CELL_W = 16;
int16_t CELL_H = 16;
int16_t BG_GRID_W = 0;
int16_t BG_GRID_H = 0;

float CANVAS_PANX = 0.0;
float CANVAS_PANY = 0.0;

float CANVAS_X_ANIM = 0.0;
float CANVAS_Y_ANIM = 0.0;
float CANVAS_W_ANIM = 0.0;
float CANVAS_H_ANIM = 0.0;
float CELL_W_ANIM = 0.0;
float CELL_H_ANIM = 0.0;

// LAYER
uint16_t CURRENT_LAYER = 0;
COLOR* CURRENT_LAYER_PTR = nullptr;
int16_t LAYER_UPDATE = 0;
int16_t LAYER_UPDATE_X1 = INT16_MAX;
int16_t LAYER_UPDATE_Y1 = INT16_MAX;
int16_t LAYER_UPDATE_X2 = INT16_MIN;
int16_t LAYER_UPDATE_Y2 = INT16_MIN;
struct LAYER_INFO {
	SDL_Texture* texture;
	std::unique_ptr<COLOR[]> pixels;
	int16_t x;
	int16_t y;
	uint8_t alpha;
	SDL_BlendMode blendmode;
};
std::vector<LAYER_INFO> LAYERS;

// BRUSH
SDL_Texture* BRUSH_CURSOR_TEXTURE;
SDL_Texture* BRUSH_TEXTURE;
bool BRUSH_UPDATE = 0;

int16_t BRUSH_X = 0;
int16_t BRUSH_Y = 0;
uint16_t BRUSH_W = 0;
int16_t BRUSH_UPDATE_X1 = INT16_MAX;
int16_t BRUSH_UPDATE_Y1 = INT16_MAX;
int16_t BRUSH_UPDATE_X2 = INT16_MIN;
int16_t BRUSH_UPDATE_Y2 = INT16_MIN;
std::unique_ptr<COLOR[]> BRUSH_PIXELS;
COLOR BRUSH_COLOR {255, 255, 255, 128};
COLOR UNDO_COLOR{255, 0, 64, 192};
COLOR* BRUSH_CURSOR_PIXELS;
COLOR* BRUSH_CURSOR_PIXELS_CLEAR;
SDL_Rect BRUSH_CURSOR_PIXELS_CLEAR_RECT;
uint16_t BRUSH_CURSOR_PIXELS_CLEAR_POS;

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

std::vector<std::unique_ptr<BRUSH_DATA>> BRUSH_LIST;
uint16_t BRUSH_LIST_POS = 0;

//SDL_Texture* CANVAS_TEXTURE;// = SDL_CreateTexture(RENDERER, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, CANVAS_W, CANVAS_H);
//uint32_t* CANVAS_PIXELS;// = new uint32_t[CANVAS_W * CANVAS_H];

bool MOUSEBUTTON_LEFT = false;
bool MOUSEBUTTON_PRESSED_LEFT = false;
bool MOUSEBUTTON_MIDDLE = false;
bool MOUSEBUTTON_PRESSED_MIDDLE = false;
bool MOUSEBUTTON_RIGHT = false;
bool MOUSEBUTTON_PRESSED_RIGHT = false;

// TOOL
uint16_t CURRENT_TOOL = 0;

// UI
int16_t UIBOX_IN = -1;
int16_t UIBOX_PREVIN = -1;
int16_t UIBOX_CLICKED_IN = -1;
int16_t UIBOX_PANX = 0;
int16_t UIBOX_PANY = 0;
SDL_Texture* UI_TEXTURE_HUEBAR;
COLOR* UI_PIXELS_HUEBAR;

int16_t ELEMENT_IN = -1;
int16_t ELEMENT_CLICKED_IN = -1;

bool TEST_BOOL = false;

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
std::vector<UIBOX_INFO> UIBOXES;

UIBOX_INFO* UIBOX_TOOLS;
UIBOX_INFO* UIBOX_COLOR;
UIBOX_INFO* UIBOX_BRUSH;
UIBOX_INFO* UIBOX_CANVAS;

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

COLOR COL_ACCENT{ 0xff, 0x00, 0x40, 0xc0 };
COLOR COL_BGUPDATE{ 0x00, 0x00, 0x00, 0x01 };

std::vector<UNDO_DATA> UNDO_LIST;
uint16_t UNDO_POS = 0;
uint16_t UNDO_UPDATE = 0;
uint16_t UNDO_UPDATE_LAYER = 0;
SDL_Rect UNDO_UPDATE_RECT = { 0, 0, 1, 1 };
COLOR UNDO_COL = COL_ACCENT;

// PRESET COLORS
COLOR COL_WHITE{ 0xff, 0xff, 0xff, 0xff };
COLOR COL_LTGRAY{ 0xc0, 0xc0, 0xc0, 0xff };
COLOR COL_GRAY{ 0x80, 0x80, 0x80, 0xff };
COLOR COL_DKGRAY{ 0x40, 0x40, 0x40, 0xff };
COLOR COL_BLACK{ 0x00, 0x00, 0x00, 0xff };

static const bool arrow[] = {
	1,0,0,0,0,0,1,0,
	0,1,0,0,0,1,0,0,
	0,0,1,0,1,0,0,0,
	0,0,0,1,0,0,0,0,
	0,0,1,0,1,0,0,0,
	0,1,0,0,0,1,0,0,
	1,0,0,0,0,0,1,0,
	0,0,0,0,0,0,0,0
};

static SDL_Cursor* init_system_cursor(const bool image[])
{
	Uint8 data[8*8];
	Uint8 mask[8*8];
	int n = -1;
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (j % 8) {
				data[n] <<= 1;
				mask[n] <<= 1;
			}
			else {
				++n;
				data[n] = mask[i] = 0;
			}
			if (arrow[i * 8 + j])
			{
				data[n] |= 0x00;
				mask[n] |= 0x01;
			}
		}
	}
	return SDL_CreateCursor(data, mask, 8, 8, 3, 3);
}

const uint8_t CHAR_NBSP = 0xff; // no-break space
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

bool DEBUG_BOOL = false;

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