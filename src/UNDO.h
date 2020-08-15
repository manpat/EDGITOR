#pragma once

#include <vector>
#include <cstdint>

#include "COLOR.h"

struct SDL_Rect;

struct UNDO_ENTRY
{
	uint16_t layer = 0;
	uint16_t x = 0;
	uint16_t y = 0;
	uint16_t w;
	uint16_t h;
	std::vector<COLOR> undo_pixels;
	std::vector<COLOR> redo_pixels;

	UNDO_ENTRY(uint16_t _w, uint16_t _h)
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


void push_undo_entry(UNDO_ENTRY);
void clear_undo_stack();

// NOTE: these names are way too short but I couldn't think of anything better
void undo();
void redo();


extern uint16_t UNDO_UPDATE;
extern uint16_t UNDO_UPDATE_LAYER;
extern SDL_Rect UNDO_UPDATE_RECT;
