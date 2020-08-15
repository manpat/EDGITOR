#include "UNDO.h"
#include "CANVAS.h"
#include "COLOR.h"
#include "FUNCTIONS.h"
#include "VARIABLES.h"
#include "BRUSH.h"
#include "RECT.h"

// manpat: really not a fan of this - this should be cmakes job :(
#ifdef __APPLE__
#include <SDL2/SDL_rect.h>
#else
#include <SDL_rect.h>
#endif

uint16_t UNDO_UPDATE = 0;
uint16_t UNDO_UPDATE_LAYER = 0;
SDL_Rect UNDO_UPDATE_RECT = { 0, 0, 1, 1 };

namespace
{
	std::vector<UNDO_ENTRY> UNDO_STACK;
	uint16_t UNDO_POS = 0;
}


void push_undo_entry(UNDO_ENTRY undo_entry)
{
	// if we're back a few steps in the undo reel, we clear all the above undo steps.
	if (UNDO_POS > 0) {
		auto const old_end = UNDO_STACK.end();
		auto const new_end = std::prev(old_end, UNDO_POS);
		UNDO_STACK.erase(new_end, old_end);
	}

	// add the new undo
	UNDO_STACK.push_back(std::move(undo_entry));
	UNDO_POS = 0;
}


void clear_undo_stack()
{
	UNDO_STACK.clear();
	UNDO_POS = 0;
}


static void apply_undo_data(UNDO_ENTRY const* undo_entry, bool is_undo)
{
	int const start_x = undo_entry->x;
	int const start_y = undo_entry->y;
	auto const& data = is_undo? undo_entry->undo_pixels : undo_entry->redo_pixels;

	for (int y = 0; y < undo_entry->h; y++)
	{
		for (int x = 0; x < undo_entry->w; x++)
		{
			const int index = x + y * undo_entry->w;
			set_pixel_layer(x + start_x, y + start_y, data[index], undo_entry->layer);
		}
	}

	UNDO_UPDATE = 1;
	UNDO_UPDATE_LAYER = undo_entry->layer;
	UNDO_UPDATE_RECT = BRUSH_UPDATE_REGION.to_sdl();
	CURRENT_LAYER = undo_entry->layer;
	CANVAS_UPDATE = true;
}


void undo()
{
	if (UNDO_POS >= UNDO_STACK.size()) {
		return;
	}

	auto const undo_entry = &UNDO_STACK[UNDO_STACK.size() - UNDO_POS - 1];
	apply_undo_data(undo_entry, true);

	UNDO_POS++;
}


void redo()
{
	if (UNDO_POS == 0) {
		return;
	}

	UNDO_POS--;

	auto const undo_entry = &UNDO_STACK[UNDO_STACK.size() - UNDO_POS - 1];
	apply_undo_data(undo_entry, false);
}
