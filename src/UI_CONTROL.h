#pragma once

#include <string>
#include <vector>
#include <memory>
#include <deque>
#include <iostream>

#include "COLOR.h"
#include "CANVAS.h"
#include "BRUSH.h"

struct SDL_Texture;
constexpr uint16_t UIBOX_UPDATE_TICK = 10;

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

struct UIBOX_ELEMENT_MAIN;

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

	std::vector<std::shared_ptr<UIBOX_ELEMENT_MAIN>> element_list;

	std::deque<uint16_t> update_stack;
	std::string title;
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


void SYSTEM_UIBOX_UPDATE();

// returns if the mouse input was claimed by a uibox
bool SYSTEM_UIBOX_HANDLE_MOUSE_DOWN(bool is_click, int mouse_x, int mouse_y);
void SYSTEM_UIBOX_HANDLE_MOUSE_UP();

UIBOX_INFO* uibox_new(uint16_t _x, uint16_t _y, uint16_t _w, uint16_t _h, bool can_grab, std::string title);
void uibox_set_char(UIBOX_INFO* ui, uint16_t char_pos, uint8_t _CHR, COLOR _COL, COLOR _BG_COL, bool update);
void uibox_set_string(UIBOX_INFO* uibox, std::string _charlist, uint16_t x, uint16_t y, COLOR col, bool update);
//void uibox_add_element(UIBOX_INFO* uibox, std::string text, std::string over_text, uint8_t type, bool* bool_ptr, uint16_t* int_ptr, uint16_t int_var, bool is_pos, uint16_t px, uint16_t py);

void uibox_add_element_textbox(UIBOX_INFO* uibox, uint16_t x, uint16_t y, std::string text);
void uibox_add_element_varbox(UIBOX_INFO* uibox, uint16_t x, uint16_t y, std::string text, uint16_t* input_var, uint16_t var);
void uibox_add_element_varbox_u8(UIBOX_INFO* uibox, uint16_t x, uint16_t y, std::string text, uint8_t* input_var, uint8_t var);
void uibox_add_element_button(UIBOX_INFO* uibox, uint16_t x, uint16_t y, int16_t w, int16_t h, std::string text, std::string sel_text, uint16_t* input_var, uint16_t button_var);
void uibox_add_element_button_u8(UIBOX_INFO* uibox, uint16_t x, uint16_t y, int16_t w, int16_t h, std::string text, std::string sel_text, uint8_t* input_var, uint16_t button_var);
void uibox_add_element_toggle(UIBOX_INFO* uibox, uint16_t x, uint16_t y, int16_t w, int16_t h, std::string text, std::string sel_text, bool* input_var);
void uibox_add_element_slider(UIBOX_INFO* uibox, uint16_t x, uint16_t y, std::string text, uint16_t* input_var);
void uibox_add_element_textinput(UIBOX_INFO* uibox, uint16_t x, uint16_t y, std::string text);
void uibox_add_element_numinput(UIBOX_INFO* uibox, uint16_t x, uint16_t y, std::string text);

void uibox_update_element(int16_t uibox_in, int16_t element_in);

/*
	Finds UIBOX though the title
	RETURN: ID of that UIBOX or -1 if it does not exist
*/
int16_t uibox_get_uibox_by_title(std::string title);

/*
	Finds ELEMENT though the text
	RETURN: ID of that UIBOX or -1 if it does not exist
*/
int16_t uibox_get_element_by_text(int16_t uibox_in, std::string text);

extern SDL_Texture* UI_TEXTURE_HUEBAR;
extern COLOR* UI_PIXELS_HUEBAR;

  //
 //   ELEMENTS   ///////////////////////////////////////////////// ///////  //////   /////    ////     ///      //       /
//

// UIBOX_ELEMENT_MAIN
struct UIBOX_ELEMENT_MAIN {
	bool over = 0;
	bool highlight = 1;
	bool prev_sel = 0;
	bool prev_over = 0;
	
	std::string text = "";
	std::string sel_text = "";
	uint16_t x = 0;
	uint16_t y = 0;
	// for W & H: 0=text size, -1=window size
	int16_t w = 0;
	int16_t h = 0;
	virtual void create(UIBOX_INFO*) = 0;
	virtual void set(void)
	{
		//
	}
	virtual void update(UIBOX_INFO* uibox)
	{
		for (uint16_t iy = 0; iy < h; iy++)
		for (uint16_t ix = 0; ix < w; ix++)
		{
			uibox_set_char(uibox, x + ix + ((y + iy) * uibox->chr_w),
				prev_sel ? (ix < sel_text.size() ? (sel_text.c_str())[ix] : 32) : (ix < text.size() ? (text.c_str())[ix] : 32),
				COL_EMPTY,
				(prev_over || prev_sel) ? COL_ACCENT : COL_BGUPDATE,
				1);
		}
	}
	virtual bool is_sel()
	{
		return 0;
	}
};

// TEXTBOX
struct UIBOX_ELEMENT_TEXTBOX : public UIBOX_ELEMENT_MAIN {
	void create(UIBOX_INFO* uibox)
	{
		uibox_set_string(uibox, text, x, y, COL_WHITE, false);
	}
	void update(UIBOX_INFO* uibox)
	{
		//
	}
};

// VARBOX
struct UIBOX_ELEMENT_VARBOX : public UIBOX_ELEMENT_MAIN {
	uint16_t* input_var = nullptr;
	uint16_t var = 0;
	void create(UIBOX_INFO* uibox)
	{
		uibox_set_string(uibox, text, x, y, COL_WHITE, false);
	}
	void update(UIBOX_INFO* uibox)
	{
		char buffer[16];
		sprintf(buffer, "%i", *input_var);
		std::string var_text(buffer);

		for (uint16_t ix = 0; ix < 3; ix++)
		{
			uibox_set_char(uibox, x + ix + (y * uibox->chr_w),
				ix < var_text.size() ? (var_text.c_str())[ix] : 32,
				COL_EMPTY,
				COL_BGUPDATE,
				1);
		}
	}
	bool is_sel()
	{
		bool _c = *input_var != var;
		if (_c) var = *input_var;
		return _c;
	}
};

struct UIBOX_ELEMENT_VARBOX_U8 : public UIBOX_ELEMENT_MAIN {
	uint8_t* input_var = nullptr;
	uint8_t var = 0;
	void create(UIBOX_INFO* uibox)
	{
		uibox_set_string(uibox, text, x, y, COL_WHITE, false);
	}
	void update(UIBOX_INFO* uibox)
	{
		char buffer[4];
		sprintf(buffer, "%i", *input_var);
		std::string var_text(buffer);
		
		for (uint16_t ix = 0; ix < 3; ix++)
		{
			uibox_set_char(uibox, x + ix + (y * uibox->chr_w),
				ix < var_text.size() ? (var_text.c_str())[ix] : 32,
				COL_EMPTY,
				COL_BGUPDATE,
				1);
		}
	}
	bool is_sel()
	{
		bool _c = *input_var != var;
		if (_c) var = *input_var;
		return _c;
	}
};

// BUTTON
struct UIBOX_ELEMENT_BUTTON : public UIBOX_ELEMENT_MAIN {
	uint16_t* input_var = nullptr;
	uint16_t button_var = 0;
	
	void create(UIBOX_INFO* uibox)
	{
		uibox_set_string(uibox, text, x, y, COL_WHITE, false);
	};
	void set()
	{
		*input_var = button_var;
	}
	bool is_sel()
	{
		return *input_var == button_var;
	}
};

struct UIBOX_ELEMENT_BUTTON_U8 : public UIBOX_ELEMENT_MAIN {
	uint8_t* input_var = nullptr;
	uint8_t button_var = 0;

	void create(UIBOX_INFO* uibox)
	{
		uibox_set_string(uibox, text, x, y, COL_WHITE, false);
	};
	void set()
	{
		*input_var = button_var;
	}
	bool is_sel()
	{
		return *input_var == button_var;
	}
};

// TOGGLE
struct UIBOX_ELEMENT_TOGGLE : public UIBOX_ELEMENT_MAIN {
	bool* input_var = nullptr;
	void create(UIBOX_INFO* uibox)
	{
		uibox_set_string(uibox, text, x, y, COL_WHITE, false);
	};
	void set()
	{
		*input_var = !(*input_var);
	}
	bool is_sel()
	{
		return *input_var == true;
	}
};

// SLIDER
struct UIBOX_ELEMENT_SLIDER : public UIBOX_ELEMENT_MAIN {
	void create(UIBOX_INFO* uibox) {};
};

// TEXTINPUT
struct UIBOX_ELEMENT_TEXTINPUT : public UIBOX_ELEMENT_MAIN {
	void create(UIBOX_INFO* uibox) {};
};

// NUM_INPUT
struct UIBOX_ELEMENT_NUMINPUT : public UIBOX_ELEMENT_MAIN {
	void create(UIBOX_INFO* uibox) {};
};