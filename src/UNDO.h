#pragma once

#include <vector>
#include <cstdint>

#include "COLOR.h"
#include "RECT.h"

struct UNDO_ENTRY
{
	RECT affected_region;
	uint16_t affected_layer = 0;

	std::vector<COLOR> undo_pixels;
	std::vector<COLOR> redo_pixels;

	UNDO_ENTRY(RECT _rect, uint16_t _layer)
		: affected_region {_rect}
		, affected_layer {_layer}
		, undo_pixels(_rect.width()*_rect.height())
		, redo_pixels(_rect.width()*_rect.height())
	{
	}

	void set(uint16_t xx, uint16_t yy, COLOR prev_col, COLOR new_col)
	{
		int index = xx + yy * this->affected_region.width();
		this->undo_pixels[index] = prev_col;
		this->redo_pixels[index] = new_col;
	}
};


void push_undo_entry(UNDO_ENTRY);
void clear_undo_stack();

// NOTE: these names are way too short but I couldn't think of anything better
void undo();
void redo();


extern uint16_t UNDO_UPDATE_LAYER;
extern RECT UNDO_UPDATE_REGION;
