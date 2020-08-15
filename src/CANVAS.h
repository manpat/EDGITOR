#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <algorithm>

struct COLOR;
struct RECT;
struct SDL_Renderer;
struct SDL_Texture;


struct LAYER_INFO {
	SDL_Texture* texture;
	std::unique_ptr<COLOR[]> pixels;
	int16_t x;
	int16_t y;
	uint8_t alpha;
	int blendmode; /* SDL_BlendMode */
};


bool in_canvas(const uint16_t x, const uint16_t y);
bool out_canvas(const uint16_t x, const uint16_t y);


void layer_new(SDL_Renderer* _renderer, int16_t _x, int16_t _y, int16_t _a, /*SDL_BlendMode*/ int _b);


void set_pixel(const int16_t x, const int16_t y, const COLOR c);
void set_pixel_brush(int x, int y, COLOR c);
void set_pixel_layer(const int16_t x, const int16_t y, const COLOR c, uint16_t l);
void set_pixel_line(int16_t x0, int16_t y0, const int16_t x1, const int16_t y1, COLOR c);

COLOR get_pixel(const int16_t x, const int16_t y);
COLOR get_pixel_layer(const int16_t x, const int16_t y, uint16_t l);


//bool floodfill_check(const uint16_t x, const uint16_t y, const COLOR col);
//bool floodfill_check_not(const uint16_t x, const uint16_t y, const COLOR col);

void floodfill(int x, int y, COLOR newColor, COLOR oldColor);
//void floodfill_core(uint16_t x, uint16_t y, const uint16_t width, const uint16_t height, const COLOR col_old, const COLOR col_new);


// CANVAS
extern bool CANVAS_UPDATE;
extern float CANVAS_ZOOM;
extern float CANVAS_X;
extern float CANVAS_Y;
extern uint16_t CANVAS_W;
extern uint16_t CANVAS_H;
extern uint16_t CANVAS_PREVW;
extern uint16_t CANVAS_PREVH;
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
extern RECT LAYER_UPDATE_REGION;


extern std::vector<LAYER_INFO> LAYERS;
