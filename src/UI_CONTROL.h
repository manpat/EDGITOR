#pragma once

#include <string>
#include <vector>
#include <memory>
#include <deque>
#include <iostream>
#include <filesystem>
#include <algorithm>

#include "COLOR.h"
#include "CANVAS.h"
#include "BRUSH.h"
#include "SYSTEM.h"
#include "VARIABLES.h"
#include "SDL_image.h"
#include "SUPERSTACK.h"

struct SDL_Texture;

constexpr uint16_t UIBOX_UPDATE_TICK = 0; // 0=go as fast as possible, >0=frame delay between character updates

extern int16_t UIBOX_IN;
extern int16_t UIBOX_PREVIN;
extern int16_t UIBOX_CLICKED_IN;
extern int16_t UIBOX_PANX;
extern int16_t UIBOX_PANY;

extern int16_t ELEMENT_IN;
extern int16_t ELEMENT_CLICKED_IN;
extern bool ELEMENT_TOGGLE_BOOL;

extern std::vector<std::unique_ptr<UIBOX_INFO>> UIBOXES;
extern std::vector<std::pair<std::string, bool>> PATH_FILES;

struct UIBOX_CHAR {
	bool update = 0;
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
struct UIBOX_INFO;

void uibox_set_string(UIBOX_INFO* uibox, std::string _charlist, uint16_t x, uint16_t y, COLOR col, bool update);
void uibox_set_char(UIBOX_INFO* ui, uint16_t char_pos, uint8_t _CHR, COLOR _COL, COLOR _BG_COL, bool update);

struct UIBOX_INFO {
	bool creation_update = true;
	bool update = true;
	bool element_update = true;
	bool can_scroll = false;

	bool can_grab = true;
	bool in_grab = false;
	bool grab = false;

	bool can_shrink = true;
	bool in_shrink = false;
	bool shrink = false;

	std::vector<UIBOX_CHAR> charinfo;
	std::vector<UIBOX_ELEMENT> element;

	std::vector<std::shared_ptr<UIBOX_ELEMENT_MAIN>> element_list;

	faststack<uint16_t> update_stack;
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
	uint16_t scroll_x = 0;
	uint16_t scroll_y = 0;
	std::vector<uint16_t> scroll_element_list;
	bool scroll_element_list_create = true;
	uint16_t scroll_element_list_size = 0;

	void construct()
	{
		if (this->texture != nullptr)
		{
			SDL_DestroyTexture(this->texture);
			this->texture = nullptr;
		}

		// FILL WINDOW WITH EMPTY CHARS
		for (int j = 0; j < this->chr_w * this->chr_h; j++)
		{
			UIBOX_CHAR _chr;
			_chr.col = COL_WHITE;
			_chr.chr = ' ';
			//_chr.bg_col = COL_BGUPDATE;
			this->charinfo.push_back(std::move(_chr));
			this->update_stack.push(j);// .insert(this->update_stack.begin() + (rand() % (this->update_stack.size() + 1)), j);
		}

		// ADD BORDER
		for (int j = 0; j < this->chr_h; j++)
		{
			for (int i = 0; i < this->chr_w; i++)
			{
				if ((j > 0 && j < this->chr_h - 1) && (i > 0 && i < this->chr_w - 1))
				{
					continue;
				}
				this->charinfo[j * this->chr_w + i].chr = (j == 0) ? ((i == 0) ? CHAR_BOXTL : ((i == this->chr_w - 1) ? CHAR_BOXTR : CHAR_BOXH)) :
					((j == this->chr_h - 1) ? ((i == 0) ? CHAR_BOXBL : ((i == this->chr_w - 1) ? CHAR_BOXBR : CHAR_BOXH)) : CHAR_BOXV);
			}
		}

		// TITLE
		uibox_set_string(this, STR_NBSP + title + STR_NBSP, 1, 0, COL_WHITE, false);

		// SHRINK BUTTON
		uibox_set_string(this, STR_NBSP STR_ARWD STR_NBSP, this->chr_w - 4, 0, COL_WHITE, false);
	}
};


void UPDATE_PATH_FILES();

void SYSTEM_UIBOX_UPDATE();

// returns if the mouse input was claimed by a uibox
bool SYSTEM_UIBOX_HANDLE_MOUSE_DOWN(bool is_click, int mouse_x, int mouse_y);
void SYSTEM_UIBOX_HANDLE_MOUSE_UP();
void uibox_update_files();

UIBOX_INFO* uibox_new(uint16_t _x, uint16_t _y, uint16_t _w, uint16_t _h, bool can_grab, std::string title);
//void uibox_add_element(UIBOX_INFO* uibox, std::string text, std::string over_text, uint8_t type, bool* bool_ptr, uint16_t* int_ptr, uint16_t int_var, bool is_pos, uint16_t px, uint16_t py);

void uibox_add_element_textbox(UIBOX_INFO* uibox, uint16_t x, uint16_t y, std::string text);
void uibox_add_element_varbox(UIBOX_INFO* uibox, uint16_t x, uint16_t y, std::string text, uint16_t* input_var, uint16_t var);
void uibox_add_element_varbox_u8(UIBOX_INFO* uibox, uint16_t x, uint16_t y, std::string text, uint8_t* input_var, uint8_t var);
void uibox_add_element_varbox_f(UIBOX_INFO* uibox, uint16_t x, uint16_t y, std::string text, float* input_var, float var);
void uibox_add_element_button(UIBOX_INFO* uibox, uint16_t x, uint16_t y, int16_t w, int16_t h, std::string text, std::string sel_text, uint16_t* input_var, uint16_t button_var);
void uibox_add_element_button_u8(UIBOX_INFO* uibox, uint16_t x, uint16_t y, int16_t w, int16_t h, std::string text, std::string sel_text, uint8_t* input_var, uint8_t button_var);
void uibox_add_element_button_string(UIBOX_INFO* uibox, uint16_t x, uint16_t y, int16_t w, int16_t h, std::string text, std::string sel_text, std::string* input_var, std::string button_var);
void uibox_add_element_button_files_goto(UIBOX_INFO* uibox, uint16_t x, uint16_t y, int16_t w, int16_t h, std::string text, std::string* input_var, std::string button_var);
void uibox_add_element_button_files_load(UIBOX_INFO* uibox, uint16_t x, uint16_t y, int16_t w, int16_t h, std::string text, std::string path, std::string file);
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
	bool over = false;
	bool highlight = true;
	bool prev_sel = false;
	bool prev_over = false;
	bool const_update = false;
	
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
		{
			for (uint16_t ix = 0; ix < w; ix++)
			{
				uibox_set_char(uibox, x + ix + ((y + iy) * uibox->chr_w),
					prev_sel ? (ix < sel_text.size() ? (sel_text.c_str())[ix] : 32) : (ix < text.size() ? (text.c_str())[ix] : 32),
					COL_EMPTY,
					(prev_over || prev_sel) ? COL_ACCENT : COL_BGUPDATE,
					1);
			}
		}
	}
	virtual bool is_sel()
	{
		return false;
	}
};

// TEXTBOX
struct UIBOX_ELEMENT_TEXTBOX : public UIBOX_ELEMENT_MAIN {
	void create(UIBOX_INFO* uibox) override
	{
		uibox_set_string(uibox, text, x, y, COL_WHITE, uibox->element_update);
	}
};

// VARBOX
struct UIBOX_ELEMENT_VARBOX : public UIBOX_ELEMENT_MAIN {
	uint16_t* input_var = nullptr;
	uint16_t var = 0;
	void create(UIBOX_INFO* uibox) override
	{
		uibox_set_string(uibox, text, x, y, COL_WHITE, uibox->element_update);
	}
	void update(UIBOX_INFO* uibox) override
	{
		char buffer[16];
		sprintf(buffer, "%i", *input_var);
		std::string var_text(buffer);

		for (uint16_t ix = 0; ix < 5; ix++)
		{
			uibox_set_char(uibox, x + ix + (y * uibox->chr_w),
				ix < var_text.size() ? (var_text.c_str())[ix] : 32,
				COL_EMPTY,
				COL_BGUPDATE,
				1);
		}
	}
	bool is_sel() override
	{
		bool _c = *input_var != var;
		if (_c) var = *input_var;
		return _c;
	}
};

struct UIBOX_ELEMENT_VARBOX_U8 : public UIBOX_ELEMENT_MAIN {
	uint8_t* input_var = nullptr;
	uint8_t var = 0;
	void create(UIBOX_INFO* uibox) override
	{
		uibox_set_string(uibox, text, x, y, COL_WHITE, uibox->element_update);
	}
	void update(UIBOX_INFO* uibox) override
	{
		char buffer[4];
		sprintf(buffer, "%i", *input_var);
		std::string var_text(buffer);
		
		for (uint16_t ix = 0; ix < 5; ix++)
		{
			uibox_set_char(uibox, x + ix + (y * uibox->chr_w),
				ix < var_text.size() ? (var_text.c_str())[ix] : 32,
				COL_EMPTY,
				COL_BGUPDATE,
				1);
		}
	}
	bool is_sel() override
	{
		bool _c = *input_var != var;
		if (_c) var = *input_var;
		return _c;
	}
};

struct UIBOX_ELEMENT_VARBOX_F : public UIBOX_ELEMENT_MAIN {
	float* input_var = nullptr;
	float var = 0;
	void create(UIBOX_INFO* uibox) override
	{
		uibox_set_string(uibox, text, x, y, COL_WHITE, uibox->element_update);
	}
	void update(UIBOX_INFO* uibox) override
	{
		char buffer[256];
		sprintf(buffer, "%f", *input_var);
		std::string var_text(buffer);

		for (uint16_t ix = 0; ix < 5; ix++)
		{
			uibox_set_char(uibox, x + ix + (y * uibox->chr_w),
				ix < var_text.size() ? (var_text.c_str())[ix] : 32,
				COL_EMPTY,
				COL_BGUPDATE,
				1);
		}
	}
	bool is_sel() override
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
	
	void create(UIBOX_INFO* uibox) override
	{
		uibox_set_string(uibox, text, x, y, COL_WHITE, uibox->element_update);
	};
	void set() override
	{
		*input_var = button_var;
	}
	bool is_sel() override
	{
		return *input_var == button_var;
	}
};

struct UIBOX_ELEMENT_BUTTON_U8 : public UIBOX_ELEMENT_MAIN {
	uint8_t* input_var = nullptr;
	uint8_t button_var = 0;

	void create(UIBOX_INFO* uibox) override
	{
		uibox_set_string(uibox, text, x, y, COL_WHITE, uibox->element_update);
	};
	void set() override
	{
		*input_var = button_var;
	}
	bool is_sel() override
	{
		return *input_var == button_var;
	}
};

struct UIBOX_ELEMENT_BUTTON_STRING : public UIBOX_ELEMENT_MAIN {
	std::string* input_var = nullptr;
	std::string button_var = "";

	void create(UIBOX_INFO* uibox) override
	{
		uibox_set_string(uibox, text, x, y, COL_WHITE, uibox->element_update);
	};
	void set() override
	{
		*input_var = button_var;
	}
	bool is_sel() override
	{
		return *input_var == button_var;
	}
};

struct UIBOX_ELEMENT_BUTTON_FILES_GOTO : public UIBOX_ELEMENT_MAIN {
	std::string* input_var = nullptr;
	std::string button_var = "";

	void create(UIBOX_INFO* uibox) override
	{
		uibox_set_string(uibox, text, x, y, COL_WHITE, uibox->element_update);
	};
	void set() override
	{
		MOUSEBUTTON_LEFT = 0;
		MOUSEBUTTON_PRESSED_LEFT = 0;

		if (*input_var != button_var)
		{
			std::cout << *input_var << std::endl;
			*input_var = button_var;
			std::filesystem::current_path(button_var);
			std::cout << *input_var << std::endl;

			int _tx, _ty;
			std::string _tchar;
			for (int i = 0; i < UIBOX_FILES->element_list.size(); i++)
			{
				_tx = UIBOX_FILES->element_list[i]->x;
				_ty = UIBOX_FILES->element_list[i]->y;
				_tchar = UIBOX_FILES->element_list[i]->text;
				for (uint16_t j = 0; j < _tchar.size(); j++)
				{
					// clearing the previous elements
					uibox_set_char(UIBOX_FILES, j + (_ty * UIBOX_FILES->chr_w + _tx), 32, COL_EMPTY, COL_BGUPDATE, 1);
				}
			}

			UIBOX_FILES->element_update = true;
			UIBOX_FILES->update = true;

			UIBOX_FILES->scroll_x = 0;
			UIBOX_FILES->scroll_y = 0;
			UIBOX_FILES->scroll_element_list.clear();
			UIBOX_FILES->scroll_element_list_create = 1;
			UIBOX_FILES->scroll_element_list_size = 0;

			uibox_update_files();
		}
		UPDATE_PATH_FILES();
	}
	bool is_sel() override
	{
		return *input_var == button_var;
	}
};

struct UIBOX_ELEMENT_BUTTON_FILES_LOAD : public UIBOX_ELEMENT_MAIN {
	std::string file;
	std::string path;

	void create(UIBOX_INFO* uibox) override
	{
		uibox_set_string(uibox, text, x, y, COL_WHITE, uibox->element_update);
	}

	void set() override
	{
		MOUSEBUTTON_LEFT = 0;
		MOUSEBUTTON_PRESSED_LEFT = 0;

		SDL_Surface* _surfload = IMG_Load(path.c_str());
		SDL_Surface* _surf = SDL_ConvertSurfaceFormat(_surfload, SDL_PIXELFORMAT_RGBA32, 0);

		CANVAS_W = _surf->w;
		CANVAS_H = _surf->h;
		SDL_FreeSurface(_surfload);
		while (LAYERS.size() > 1)
		{
			LAYERS.erase(LAYERS.end());
		}
		CURRENT_LAYER = 0;
		CURRENT_LAYER_PTR = LAYERS[CURRENT_LAYER].pixels.get();

		LAYERS[CURRENT_LAYER].texture = SDL_CreateTextureFromSurface(RENDERER, _surf);
		LAYERS[CURRENT_LAYER].pixels = nullptr;
		LAYERS[CURRENT_LAYER].pixels = std::make_unique<COLOR[]>(CANVAS_W * CANVAS_H);

		COLOR* layer_data = (LAYERS[CURRENT_LAYER].pixels.get());
		for (int i = 0; i < CANVAS_W * CANVAS_H; i++)
		{

			int bpp = _surf->format->BytesPerPixel;
			uint8_t* p = (uint8_t*)_surf->pixels + i * bpp;

			layer_data[i] = COLOR{ p[0], p[1], p[2], p[3] };
		}

		// CENTER CANVAS
		CANVAS_X = (WINDOW_W * 0.5f) - (CANVAS_W * 0.5f);
		CANVAS_Y = (WINDOW_H * 0.5f) - (CANVAS_H * 0.5f);
		CANVAS_X_ANIM = CANVAS_X;
		CANVAS_Y_ANIM = CANVAS_Y;
		CANVAS_W_ANIM = CANVAS_W;
		CANVAS_H_ANIM = CANVAS_H;
		CELL_W_ANIM = CELL_W;
		CELL_H_ANIM = CELL_H;
		CANVAS_ZOOM = 1;

		CURRENT_FILE = file;
	}
};

// TOGGLE
struct UIBOX_ELEMENT_TOGGLE : public UIBOX_ELEMENT_MAIN {
	bool* input_var = nullptr;
	void create(UIBOX_INFO* uibox) override
	{
		uibox_set_string(uibox, text, x, y, COL_WHITE, uibox->element_update);
	};
	void set() override
	{
		if (MOUSEBUTTON_LEFT && MOUSEBUTTON_PRESSED_LEFT) ELEMENT_TOGGLE_BOOL = !(*input_var);
		*input_var = ELEMENT_TOGGLE_BOOL;// !(*input_var);
	}
	bool is_sel() override
	{
		return *input_var == true;
	}
};

// SLIDER
struct UIBOX_ELEMENT_SLIDER : public UIBOX_ELEMENT_MAIN {
	void create(UIBOX_INFO*) override {}
};

// TEXTINPUT
struct UIBOX_ELEMENT_TEXTINPUT : public UIBOX_ELEMENT_MAIN {
	void create(UIBOX_INFO*) override {}
};

// NUM_INPUT
struct UIBOX_ELEMENT_NUMINPUT : public UIBOX_ELEMENT_MAIN {
	void create(UIBOX_INFO*) override {}
};