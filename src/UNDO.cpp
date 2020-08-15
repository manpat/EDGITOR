#include "UNDO.h"
#include "CANVAS.h"
#include "COLOR.h"
#include "FUNCTIONS.h"
#include "VARIABLES.h"
#include "BRUSH.h"
#include "RECT.h"

// manpat: really not a fan of this - this should be cmakes job :(
#ifdef __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

std::vector<UNDO_DATA> UNDO_LIST;
uint16_t UNDO_POS = 0;
uint16_t UNDO_UPDATE = 0;
uint16_t UNDO_UPDATE_LAYER = 0;
SDL_Rect UNDO_UPDATE_RECT = { 0, 0, 1, 1 };


void function_undo(int n)
{
	const bool _is_undo = (n >= 0);
	UNDO_POS = (clamp(UNDO_POS + n, 0, UNDO_LIST.size() - 1));

	const int _tpos = (UNDO_LIST.size() - (UNDO_POS + (!_is_undo)));
	const auto undo_entry = &UNDO_LIST[_tpos];
	const int start_x = (undo_entry->x);
	const int start_y = (undo_entry->y);
	const uint16_t _l = (undo_entry->layer);
	const std::vector<COLOR>& _p = (_is_undo ? undo_entry->undo_pixels : undo_entry->redo_pixels);

	for (int y = 0; y < undo_entry->h; y++)
	{
		for (int x = 0; x < undo_entry->w; x++)
		{
			const int index = x + y * undo_entry->w;
			set_pixel_layer(x + start_x, y + start_y, _p[index], _l);
		}
	}

	UNDO_UPDATE = 1;
	UNDO_UPDATE_LAYER = _l;
	UNDO_UPDATE_RECT = BRUSH_UPDATE_REGION.to_sdl();
	CURRENT_LAYER = _l;
	CANVAS_UPDATE = true;
}