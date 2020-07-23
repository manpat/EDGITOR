#pragma once

inline void SYSTEM_BRUSH_UPDATE()
{
	// UPDATE THE BRUSH TEXTURE PER-CHANGE
			// this is because a complex shape might be drawn in one tick; like floodfill
	if (BRUSH_UPDATE) {
		// make sure the brush_update isn't beyond the canvas
		BRUSH_UPDATE_X1 = (clamp(BRUSH_UPDATE_X1, 0, CANVAS_W));
		BRUSH_UPDATE_Y1 = (clamp(BRUSH_UPDATE_Y1, 0, CANVAS_H));
		BRUSH_UPDATE_X2 = (clamp(BRUSH_UPDATE_X2, 0, CANVAS_W));
		BRUSH_UPDATE_Y2 = (clamp(BRUSH_UPDATE_Y2, 0, CANVAS_H));

		SDL_Rect const brush_dirty_rect{
			BRUSH_UPDATE_X1, BRUSH_UPDATE_Y1,
			(BRUSH_UPDATE_X2 - BRUSH_UPDATE_X1), (BRUSH_UPDATE_Y2 - BRUSH_UPDATE_Y1),
		};

		// update the brush texture
		SDL_SetTextureBlendMode(BRUSH_TEXTURE, SDL_BLENDMODE_NONE);
		SDL_UpdateTexture(BRUSH_TEXTURE, &brush_dirty_rect, &BRUSH_PIXELS[BRUSH_UPDATE_Y1 * CANVAS_W + BRUSH_UPDATE_X1], CANVAS_PITCH);

		// the layer updates only when we stop drawing - for performance.
		// so we constantly update the min and max bounds
		LAYER_UPDATE_X1 = std::min(LAYER_UPDATE_X1, BRUSH_UPDATE_X1);
		LAYER_UPDATE_Y1 = std::min(LAYER_UPDATE_Y1, BRUSH_UPDATE_Y1);
		LAYER_UPDATE_X2 = std::max(LAYER_UPDATE_X2, BRUSH_UPDATE_X2);
		LAYER_UPDATE_Y2 = std::max(LAYER_UPDATE_Y2, BRUSH_UPDATE_Y2);

		// reset the brush bounds with every tick
		BRUSH_UPDATE_X1 = INT16_MAX;
		BRUSH_UPDATE_Y1 = INT16_MAX;
		BRUSH_UPDATE_X2 = INT16_MIN;
		BRUSH_UPDATE_Y2 = INT16_MIN;
	}
}

inline void SYSTEM_LAYER_UPDATE()
{
	// LAYER UPDATE
	int t_layer_update_w = std::max((int)LAYER_UPDATE_X2 - (int)LAYER_UPDATE_X1, 0), t_layer_update_h = std::max((int)LAYER_UPDATE_Y2 - (int)LAYER_UPDATE_Y1, 0); // probably don't need these max()

	if ((LAYER_UPDATE == 1) && (t_layer_update_w > 0) && (t_layer_update_h > 0))
	{
		UNDO_DATA _u{ (uint16_t)t_layer_update_w, (uint16_t)t_layer_update_h };
		_u.x = (uint16_t)LAYER_UPDATE_X1;
		_u.y = (uint16_t)LAYER_UPDATE_Y1;
		_u.layer = CURRENT_LAYER;

		COLOR* layer_data = (LAYERS[CURRENT_LAYER].pixels.get());
		for (int16_t _y = LAYER_UPDATE_Y1; _y < LAYER_UPDATE_Y2; ++_y) {
			for (int16_t _x = LAYER_UPDATE_X1; _x < LAYER_UPDATE_X2; ++_x) {
				const int _pos = (_y * CANVAS_W + _x);
				const COLOR brush_color = BRUSH_PIXELS[_pos];
				const COLOR dest_color = layer_data[_pos];

				const COLOR empty{ 0, 0, 0, 0 };

				BRUSH_PIXELS[_pos] = empty; // clear the brush pixel

				if (brush_color == empty) // if there's an empty pixel in the brush texture
				{
					_u.set(_x - LAYER_UPDATE_X1, _y - LAYER_UPDATE_Y1, dest_color, dest_color);
					continue;
				}

				if (CURRENT_TOOL == 1) // if it's the erase tool
				{
					layer_data[_pos] = empty; // erase the destination pixel
					_u.set(_x - LAYER_UPDATE_X1, _y - LAYER_UPDATE_Y1, dest_color, empty);
					continue;
				}

				if (dest_color == empty) // if destination pixel is empty
				{
					layer_data[_pos] = brush_color; // make destination the saved brush pixel
					_u.set(_x - LAYER_UPDATE_X1, _y - LAYER_UPDATE_Y1, dest_color, brush_color);
					continue;
				}

				// if it isn't any of those edge cases, we properly mix the colours
				const COLOR new_col = blend_colors(brush_color, dest_color);
				layer_data[_pos] = new_col;
				_u.set(_x - LAYER_UPDATE_X1, _y - LAYER_UPDATE_Y1, dest_color, new_col);
			}
		}

		// clear the brush texture (since we made all pixels 0x00000000)
		const SDL_Rect dirty_rect{ LAYER_UPDATE_X1, LAYER_UPDATE_Y1, t_layer_update_w, t_layer_update_h };

		SDL_SetTextureBlendMode(BRUSH_TEXTURE, SDL_BLENDMODE_NONE);
		SDL_UpdateTexture(BRUSH_TEXTURE, &dirty_rect, &BRUSH_PIXELS[LAYER_UPDATE_Y1 * CANVAS_W + LAYER_UPDATE_X1], CANVAS_PITCH);

		// if we're back a few steps in the undo reel, we clear all the above undo steps.
		while (UNDO_POS > 0) {
			UNDO_LIST.pop_back();
			UNDO_POS--;
		};

		// add the new undo
		UNDO_LIST.push_back(std::move(_u));

		// update the layer we drew to
		SDL_UpdateTexture(LAYERS[CURRENT_LAYER].texture, &dirty_rect, &layer_data[LAYER_UPDATE_Y1 * CANVAS_W + LAYER_UPDATE_X1], CANVAS_PITCH);

		LAYER_UPDATE = 0;
	}
	else
	{
		if (LAYER_UPDATE > 0) LAYER_UPDATE--;

		if (LAYER_UPDATE == 0)
		{
			LAYER_UPDATE_X1 = INT16_MAX;
			LAYER_UPDATE_Y1 = INT16_MAX;
			LAYER_UPDATE_X2 = INT16_MIN;
			LAYER_UPDATE_Y2 = INT16_MIN;

			LAYER_UPDATE = -1;
		}
	}
}

inline void SYSTEM_CANVAS_UPDATE()
{
	// CANVAS UPDATE
	if (CANVAS_UPDATE) {
		if (UNDO_UPDATE)
		{
			SDL_SetTextureBlendMode(LAYERS[UNDO_UPDATE_LAYER].texture, SDL_BLENDMODE_NONE);
			UNDO_UPDATE_RECT.x = (clamp(UNDO_UPDATE_RECT.x, 0, CANVAS_W - 1));
			UNDO_UPDATE_RECT.y = (clamp(UNDO_UPDATE_RECT.y, 0, CANVAS_H - 1));
			UNDO_UPDATE_RECT.w = (clamp(UNDO_UPDATE_RECT.w, 1, CANVAS_W));
			UNDO_UPDATE_RECT.h = (clamp(UNDO_UPDATE_RECT.h, 1, CANVAS_H));
			SDL_UpdateTexture(LAYERS[UNDO_UPDATE_LAYER].texture, &UNDO_UPDATE_RECT, &LAYERS[UNDO_UPDATE_LAYER].pixels[UNDO_UPDATE_RECT.y * CANVAS_W + UNDO_UPDATE_RECT.x], CANVAS_PITCH);
		}
		else
		{
			for (const auto& layer : LAYERS) {
				SDL_SetTextureBlendMode(layer.texture, SDL_BLENDMODE_NONE);
				SDL_UpdateTexture(layer.texture, nullptr, &layer.pixels[0], CANVAS_PITCH);
			}
		}

		SDL_UpdateTexture(BRUSH_TEXTURE, nullptr, BRUSH_PIXELS.get(), CANVAS_PITCH);
		CANVAS_UPDATE = 0;
		UNDO_UPDATE = 0;
		UNDO_UPDATE_LAYER = 0;
	}
}