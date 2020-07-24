#pragma once

#include <vector>
#include <cstdint>

#include "COLOR.h"

struct SDL_Rect;

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


void function_undo(int n);



extern std::vector<UNDO_DATA> UNDO_LIST;
extern uint16_t UNDO_POS;
extern uint16_t UNDO_UPDATE;
extern uint16_t UNDO_UPDATE_LAYER;
extern SDL_Rect UNDO_UPDATE_RECT;
