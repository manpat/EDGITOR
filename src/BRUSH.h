#pragma once

#include <cstdint>
#include <vector>
#include <memory>

struct SDL_Texture;
struct SDL_Rect;
struct COLOR;
struct RECT;

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

void brush_new(uint8_t* a, uint16_t w);
void brush_set(uint16_t sel);

extern SDL_Texture* BRUSH_CURSOR_TEXTURE;
extern SDL_Texture* BRUSH_TEXTURE;
extern bool BRUSH_UPDATE;

extern int16_t BRUSH_X;
extern int16_t BRUSH_Y;
extern uint16_t BRUSH_W;
extern RECT BRUSH_UPDATE_REGION;
extern std::unique_ptr<COLOR[]> BRUSH_PIXELS;
extern COLOR BRUSH_COLOR;
extern COLOR UNDO_COLOR;
extern COLOR* BRUSH_CURSOR_PIXELS;
extern COLOR* BRUSH_CURSOR_PIXELS_CLEAR;


extern std::vector<std::unique_ptr<BRUSH_DATA>> BRUSH_LIST;
extern uint16_t BRUSH_LIST_POS;