#include "CANVAS.h"
#include "COLOR.h"
#include "VARIABLES.h"
#include "BRUSH.h"


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
std::vector<LAYER_INFO> LAYERS;



void layer_new(SDL_Renderer* _renderer, int16_t _x, int16_t _y, int16_t _a, /*SDL_BlendMode*/ int _b)
{
	LAYER_INFO new_layer;
	new_layer.texture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, CANVAS_W, CANVAS_H);

	new_layer.pixels = std::make_unique<COLOR[]>(CANVAS_W * CANVAS_H);
	new_layer.x = _x;
	new_layer.y = _y;
	new_layer.alpha = _a;
	new_layer.blendmode = _b;
	LAYERS.push_back(std::move(new_layer));
	CANVAS_UPDATE = 1;
	CURRENT_LAYER = 0;
	CURRENT_LAYER_PTR = LAYERS[CURRENT_LAYER].pixels.get();
}

bool in_canvas(const uint16_t x, const uint16_t y)
{
	return (x < CANVAS_W && y < CANVAS_H);
}

bool out_canvas(const uint16_t x, const uint16_t y)
{
	return (x >= CANVAS_W || y >= CANVAS_H);
}

void set_pixel(const int16_t x, const int16_t y, const COLOR c)
{
	if (out_canvas(x, y)) return;

	BRUSH_PIXELS[y * CANVAS_W + x] = c;
	BRUSH_UPDATE_X1 = std::min(BRUSH_UPDATE_X1, int16_t(x - 1));
	BRUSH_UPDATE_Y1 = std::min(BRUSH_UPDATE_Y1, int16_t(y - 1));
	BRUSH_UPDATE_X2 = std::max(BRUSH_UPDATE_X2, int16_t(x + 1));
	BRUSH_UPDATE_Y2 = std::max(BRUSH_UPDATE_Y2, int16_t(y + 1));
}

void set_pixel_brush(int x, int y, COLOR c)
{
	float _a;
	int _tx, _ty;
	for (int i = 0; i < BRUSH_W; i++)
		for (int j = 0; j < BRUSH_W; j++)
		{
			_tx = ((x + BRUSH_X) + i);
			_ty = ((y + BRUSH_Y) + j);
			//if (!in_canvas(_tx, _ty)) continue;
			_a = (BRUSH_LIST[BRUSH_LIST_POS]->alpha[j * BRUSH_W + i]);
			if (!_a) continue;
			//if (BRUSH_PIXELS[_tx, _ty] != 0x00000000) continue;
			if (_a) set_pixel(_tx, _ty, c);
		}
}

void set_pixel_layer(const int16_t x, const int16_t y, const COLOR c, uint16_t l)
{
	if (out_canvas(x, y)) return;

	LAYERS[l].pixels[y * CANVAS_W + x] = c;
	BRUSH_UPDATE_X1 = std::min(BRUSH_UPDATE_X1, int16_t(x - 1));
	BRUSH_UPDATE_Y1 = std::min(BRUSH_UPDATE_Y1, int16_t(y - 1));
	BRUSH_UPDATE_X2 = std::max(BRUSH_UPDATE_X2, int16_t(x + 1));
	BRUSH_UPDATE_Y2 = std::max(BRUSH_UPDATE_Y2, int16_t(y + 1));
}

void set_pixel_line(int16_t x0, int16_t y0, const int16_t x1, const int16_t y1, COLOR c)
{
	int16_t dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int16_t dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int16_t err = dx + dy, e2;
	for (;;) {
		set_pixel_brush(x0, y0, c); // set_pixel_brush
		if (x0 == x1 && y0 == y1) break;
		e2 = err << 1;
		if (e2 >= dy) { err += dy; x0 += sx; }
		if (e2 <= dx) { err += dx; y0 += sy; }
	}
	BRUSH_UPDATE = 1;
	LAYER_UPDATE = 2;
}

bool floodfill_check(const uint16_t x, const uint16_t y, const COLOR col)
{
	return ((CURRENT_LAYER_PTR[y * CANVAS_W + x] != col) || (BRUSH_PIXELS[y * CANVAS_W + x] != col));
}

bool floodfill_check_not(const uint16_t x, const uint16_t y, const COLOR col)
{
	return ((CURRENT_LAYER_PTR[y * CANVAS_W + x] == col) && (BRUSH_PIXELS[y * CANVAS_W + x] == col));
}


void floodfill(uint16_t x, uint16_t y, const uint16_t width, const uint16_t height, const COLOR col_old, const COLOR col_new)
{
	while (true)
	{
		int ox = x, oy = y;
		while (y != 0 && floodfill_check_not(x, y - 1, col_old)) y--;
		while (x != 0 && floodfill_check_not(x - 1, y, col_old)) x--;
		if (x == ox && y == oy) break;
	}
	floodfill_core(x, y, width, height, col_old, col_new);
	BRUSH_UPDATE = 1;
	LAYER_UPDATE = 2;
}

void floodfill_core(uint16_t x, uint16_t y, const uint16_t width, const uint16_t height, const COLOR col_old, const COLOR col_new)
{
	int lastRowLength = 0;
	do
	{
		int rowLength = 0, sx = x;
		if (lastRowLength != 0 && floodfill_check(x, y, col_old))
		{
			do
			{
				if (--lastRowLength == 0) return;
				++x;
			} while (floodfill_check(x, y, col_old));
			sx = x;
		}
		else
		{
			for (; x != 0 && floodfill_check_not(x - 1, y, col_old); rowLength++, lastRowLength++)
			{
				set_pixel(--x, y, col_new);
				if (y != 0 && floodfill_check_not(x, y - 1, col_old)) floodfill(x, y - 1, width, height, col_old, col_new);
			}
		}

		for (; sx < width && floodfill_check_not(sx, y, col_old); rowLength++, sx++) set_pixel(sx, y, col_new);
		if (rowLength < lastRowLength)
		{
			for (int end = x + lastRowLength; ++sx < end; )
			{
				if (floodfill_check_not(sx, y, col_old)) floodfill_core(sx, y, width, height, col_old, col_new);
			}
		}
		else if (rowLength > lastRowLength && y != 0)
		{
			for (int ux = x + lastRowLength; ++ux < sx; )
			{
				if (floodfill_check_not(ux, y - 1, col_old)) floodfill(ux, y - 1, width, height, col_old, col_new);
			}
		}
		lastRowLength = rowLength;
	} while (lastRowLength != 0 && ++y < height);
}

