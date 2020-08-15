#include "UNDO.h"
#include "CANVAS.h"
#include "COLOR.h"
#include "FUNCTIONS.h"
#include "VARIABLES.h"
#include "BRUSH.h"
#include "RECT.h"

uint16_t UNDO_UPDATE_LAYER = 0;
RECT UNDO_UPDATE_REGION = RECT::empty();

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
	auto const region = undo_entry->affected_region;
	auto const& data = is_undo? undo_entry->undo_pixels : undo_entry->redo_pixels;

	for (auto [x, y] : region)
	{
		const int index = (x - region.left) + (y - region.top) * region.width();
		set_pixel_layer(x, y, data[index], undo_entry->affected_layer);
	}

	CANVAS_UPDATE = true;
	CURRENT_LAYER = undo_entry->affected_layer;
	UNDO_UPDATE_LAYER = undo_entry->affected_layer;
	UNDO_UPDATE_REGION = region;
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
