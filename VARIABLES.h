#pragma once

  //
 //   VARIABLES   ///////////////////////////////////////////////// ///////  //////   /////    ///     //      /
//

FC_Font* font;
FC_Font* font_under;
FC_Font* font_bold;

bool QUIT = false;
float FPS = 0;

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

// CANVAS
int16_t CANVAS_UPDATE = 0;
float CANVAS_ZOOM = 1.0;
float CANVAS_X = 32.0;
float CANVAS_Y = 32.0;
uint16_t CANVAS_W = 8000;
uint16_t CANVAS_H = 8000;
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
int16_t CANVAS_PITCH = (sizeof(uint32_t) * CANVAS_W);

SDL_Texture* BG_GRID_TEXTURE;
uint32_t* BG_GRID_PIXELS;
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

float SRC_COLA, DEST_COLA, NEW_COLA;

// LAYER
uint16_t CURRENT_LAYER = 0;
uint32_t* CURRENT_LAYER_PTR = nullptr;
int16_t LAYER_UPDATE = 0;
int16_t LAYER_UPDATE_X1 = INT16_MAX;
int16_t LAYER_UPDATE_Y1 = INT16_MAX;
int16_t LAYER_UPDATE_X2 = INT16_MIN;
int16_t LAYER_UPDATE_Y2 = INT16_MIN;
struct LAYER_INFO {
	SDL_Texture* texture;
	std::unique_ptr<uint32_t[]> pixels;
	int16_t x;
	int16_t y;
	int16_t alpha;
	SDL_BlendMode blendmode;
};
std::vector<LAYER_INFO> LAYERS;

// BRUSH
SDL_Texture* BRUSH_CURSOR_TEXTURE;
SDL_Texture* BRUSH_TEXTURE;
bool BRUSH_UPDATE = 0;

int16_t BRUSH_UPDATE_X1 = INT16_MAX;
int16_t BRUSH_UPDATE_Y1 = INT16_MAX;
int16_t BRUSH_UPDATE_X2 = INT16_MIN;
int16_t BRUSH_UPDATE_Y2 = INT16_MIN;
uint32_t* BRUSH_PIXELS;
uint32_t* BRUSH_CURSOR_PIXELS;
uint32_t* BRUSH_CURSOR_PIXELS_CLEAR;
SDL_Rect BRUSH_CURSOR_PIXELS_CLEAR_RECT;
uint16_t BRUSH_CURSOR_PIXELS_CLEAR_POS;

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
SDL_Texture* UI_TEXTURE_HUEBAR;
uint32_t* UI_PIXELS_HUEBAR;

struct UIBOX_INFO {
	SDL_Texture* texture;
	uint32_t* pixels;
	int16_t x;
	int16_t y;
	int16_t alpha;
	SDL_BlendMode blendmode;
};
std::vector<UIBOX_INFO> UI_BOX;

// UNDO
struct UNDO_DATA
{
	uint16_t type = 0;
	uint16_t layer = 0;
	uint16_t x = 0;
	uint16_t y = 0;
	uint16_t w;
	uint16_t h;
	std::vector<uint32_t> undo_pixels;
	std::vector<uint32_t> redo_pixels;

	UNDO_DATA(uint16_t _w, uint16_t _h) : undo_pixels(_w* _h), redo_pixels(_w* _h)
	{
		this->w = _w;
		this->h = _h;
	}

	void _set_pixel(uint16_t p, uint32_t uc, uint32_t rc)
	{
		undo_pixels[p] = uc;
		redo_pixels[p] = rc;
	}
};

std::vector<std::shared_ptr<UNDO_DATA>> UNDO_LIST;
int16_t UNDO_POS = 0;
int16_t UNDO_UPDATE = 0;
int16_t UNDO_UPDATE_LAYER = 0;
SDL_Rect UNDO_UPDATE_RECT = { 0, 0, 1, 1 };
uint32_t UNDO_COL = 0xff0040c0;