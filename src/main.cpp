#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>

#ifdef __APPLE__
#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>
#else
#include <SDL.h>
#include <SDL_ttf.h>
#endif

#include "SDL_FontCache.h"
#include "VARIABLES.h"
#include "FUNCTIONS.h"

  //
 //   MAIN LOOP   ///////////////////////////////////////////////// ///////  //////   /////    ////     ///      //       /
//

int fps_rate = 0;

int main(int, char*[])
{
#if __APPLE__
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
#endif
    
	// MAIN INIT
	INIT_SDL();
	auto WINDOW = INIT_WINDOW();
	auto RENDERER = INIT_RENDERER(WINDOW);

	while (!QUIT) // MAIN LOOP
	{
        const char *error = SDL_GetError();
        if (*error) {
            //QUIT = 1;
            std::cout << "SDL Error: " << error << std::endl;
            SDL_ClearError();
            //break;
        }
        
        
		const Uint64 fps_start = SDL_GetPerformanceCounter(); // fps counter

		BRUSH_UPDATE = 0; // reset brush update

		float t_win_w = (float)WINDOW_W, t_win_h = (float)WINDOW_H; // temporary window size

		// SET WINDOW X, Y, W, H
		// CLEAR RENDER TARGET
		SDL_GetWindowSize(WINDOW, &WINDOW_W, &WINDOW_H);
		SDL_GetWindowPosition(WINDOW, &WINDOW_X, &WINDOW_Y);
		SDL_SetRenderTarget(RENDERER, nullptr);
		SDL_SetRenderDrawColor(RENDERER, 0, 0, 0, 255);
		SDL_RenderClear(RENDERER);

		// recenter the canvas if the window changes size
		if ((WINDOW_W != t_win_w) || (WINDOW_H != t_win_h))
		{
			CANVAS_X = (((float)WINDOW_W * .5f) - ((t_win_w * .5f) - CANVAS_X));
			CANVAS_Y = (((float)WINDOW_H * .5f) - ((t_win_h * .5f) - CANVAS_Y));
		}

		///////////////////////////////////////////////// ///////  //////   /////    ////     ///      //       /
		EVENT_LOOP();
		///////////////////////////////////////////////// ///////  //////   /////    ////     ///      //       /

		// UPDATE THE BRUSH TEXTURE PER-CHANGE
		// this is because a complex shape might be drawn in one tick; like floodfill
		if (BRUSH_UPDATE) {
			// make sure the brush_update isn't beyond the canvas
			BRUSH_UPDATE_X1 = (clamp(BRUSH_UPDATE_X1, 0, CANVAS_W));
			BRUSH_UPDATE_Y1 = (clamp(BRUSH_UPDATE_Y1, 0, CANVAS_H));
			BRUSH_UPDATE_X2 = (clamp(BRUSH_UPDATE_X2, 0, CANVAS_W));
			BRUSH_UPDATE_Y2 = (clamp(BRUSH_UPDATE_Y2, 0, CANVAS_H));

			SDL_Rect const brush_dirty_rect {
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

		// LAYER UPDATE
		int t_layer_update_w = std::max((int)LAYER_UPDATE_X2 - (int)LAYER_UPDATE_X1, 0), t_layer_update_h = std::max((int)LAYER_UPDATE_Y2 - (int)LAYER_UPDATE_Y1, 0); // probably don't need these max()

		if ((LAYER_UPDATE == 1) && (t_layer_update_w > 0) && (t_layer_update_h > 0))
		{
			UNDO_DATA _u {(uint16_t)t_layer_update_w, (uint16_t)t_layer_update_h};
			_u.x = (uint16_t)LAYER_UPDATE_X1;
			_u.y = (uint16_t)LAYER_UPDATE_Y1;
			_u.layer = CURRENT_LAYER;

			COLOR* layer_data = (LAYERS[CURRENT_LAYER].pixels.get());
			for (int16_t _y = LAYER_UPDATE_Y1; _y < LAYER_UPDATE_Y2; ++_y) {
				for (int16_t _x = LAYER_UPDATE_X1; _x < LAYER_UPDATE_X2; ++_x) {
					const int _pos = (_y * CANVAS_W + _x);
					const COLOR brush_color = BRUSH_PIXELS[_pos];
					const COLOR dest_color = layer_data[_pos];

					const COLOR empty{0, 0, 0, 0};

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
			const SDL_Rect dirty_rect { LAYER_UPDATE_X1, LAYER_UPDATE_Y1, t_layer_update_w, t_layer_update_h };

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
				for (const auto& layer: LAYERS) {
					SDL_SetTextureBlendMode(layer.texture, SDL_BLENDMODE_NONE);
					SDL_UpdateTexture(layer.texture, nullptr, &layer.pixels[0], CANVAS_PITCH);
				}
			}

			SDL_UpdateTexture(BRUSH_TEXTURE, nullptr, BRUSH_PIXELS.get(), CANVAS_PITCH);
			CANVAS_UPDATE = 0;
			UNDO_UPDATE = 0;
			UNDO_UPDATE_LAYER = 0;
		}


		SDL_SetRenderDrawColor(RENDERER, 255, 255, 255, 255);

		// RENDER
		// smooth lerping animation to make things SLIGHTLY smooth when panning and zooming
		// the '4.0' can be any integer, and will be a changeable option in Settings
		CANVAS_X_ANIM = (reach_tween(CANVAS_X_ANIM, floor(CANVAS_X), 40.0));
		CANVAS_Y_ANIM = (reach_tween(CANVAS_Y_ANIM, floor(CANVAS_Y), 40.0));
		CANVAS_W_ANIM = (reach_tween(CANVAS_W_ANIM, floor((float)CANVAS_W * CANVAS_ZOOM), 40.0));
		CANVAS_H_ANIM = (reach_tween(CANVAS_H_ANIM, floor((float)CANVAS_H * CANVAS_ZOOM), 40.0));
		CELL_W_ANIM = (reach_tween(CELL_W_ANIM, floor((float)CELL_W * CANVAS_ZOOM), 40.0));
		CELL_H_ANIM = (reach_tween(CELL_H_ANIM, floor((float)CELL_H * CANVAS_ZOOM), 40.0));
		
		SDL_FRect F_RECT {};

		// transparent background grid
		float bg_w = (ceil(CANVAS_W_ANIM / CELL_W_ANIM) * CELL_W_ANIM);
		float bg_h = (ceil(CANVAS_H_ANIM / CELL_H_ANIM) * CELL_H_ANIM);
		F_RECT = SDL_FRect {CANVAS_X_ANIM, CANVAS_Y_ANIM, bg_w, bg_h};
		SDL_SetTextureBlendMode(BG_GRID_TEXTURE, SDL_BLENDMODE_BLEND);
		SDL_RenderCopyF(RENDERER, BG_GRID_TEXTURE, nullptr, &F_RECT);

		// these 2 rects cover the overhang the background grid has beyond the canvas
		SDL_SetRenderDrawColor(RENDERER, 0, 0, 0, 255);
		F_RECT = SDL_FRect {
			std::max(0.0f, CANVAS_X_ANIM), std::max(0.0f,CANVAS_Y_ANIM + (CANVAS_H_ANIM)),
			std::min(float(WINDOW_W),bg_w), CELL_H * CANVAS_ZOOM
		};
		SDL_RenderFillRectF(RENDERER, &F_RECT);
		F_RECT = SDL_FRect {
			std::max(0.0f, CANVAS_X_ANIM + (CANVAS_W_ANIM)), std::max(0.0f, CANVAS_Y_ANIM),
			CELL_W * CANVAS_ZOOM, std::min(float(WINDOW_H),bg_h)
		};
		SDL_RenderFillRectF(RENDERER, &F_RECT);
		
		F_RECT = {CANVAS_X_ANIM, CANVAS_Y_ANIM, CANVAS_W_ANIM, CANVAS_H_ANIM};

		// RENDER THE LAYERS
		for (uint16_t i = 0; i < LAYERS.size(); i++)
		{
			const LAYER_INFO& layer = LAYERS[i];
			SDL_SetTextureBlendMode(layer.texture, layer.blendmode);
			SDL_RenderCopyF(RENDERER, layer.texture, nullptr, &F_RECT);
			if (i == CURRENT_LAYER)
			{
				if (BRUSH_UPDATE)
				{
					SDL_SetTextureBlendMode(BRUSH_TEXTURE, SDL_BLENDMODE_BLEND);
					SDL_RenderCopyF(RENDERER, BRUSH_TEXTURE, nullptr, &F_RECT);
				}
			}
		}

		// the grey box around the canvas
		SDL_SetRenderDrawColor(RENDERER, 64, 64, 64, 255);
		F_RECT = { CANVAS_X_ANIM - 2.0f, CANVAS_Y_ANIM - 2.0f, CANVAS_W_ANIM + 4.0f, CANVAS_H_ANIM + 4.0f };
		SDL_RenderDrawRectF(RENDERER, &F_RECT);

		// RENDER THE UI BOXES
		int16_t t_UIBOX_IN = UIBOX_IN;
		UIBOX_IN = -1;
		//int16_t _uiborder_over = -1;
		const int16_t _uiboxes_size = UIBOXES.size() - 1;
		//if (!MOUSEBUTTON_LEFT) UIBOX_CLICKED_IN = -1;
		SDL_Rect RECT{};
		int16_t _uibox_id = 0;
		for (int16_t i = 0; i <= _uiboxes_size; i++)
		{
			_uibox_id = _uiboxes_size - i;
			UIBOX_INFO& uibox = UIBOXES[_uibox_id];

			uibox.x = clamp(uibox.x, 0, WINDOW_W - uibox.w);
			uibox.y = clamp(uibox.y, 0, WINDOW_H - uibox.h);

			RECT = { uibox.x, uibox.y, uibox.w, uibox.h };
			SDL_SetRenderDrawColor(RENDERER, 16, 16, 16, 255);
			SDL_RenderFillRect(RENDERER, &RECT);
			SDL_SetRenderDrawColor(RENDERER, 64, 64, 64, 255);
			SDL_RenderDrawRect(RENDERER, &RECT);
			if ((UIBOX_CLICKED_IN==-1 || UIBOX_CLICKED_IN==t_UIBOX_IN) && (point_in_rect(MOUSE_X, MOUSE_Y, uibox.x, uibox.y, uibox.w, uibox.h)
				|| point_in_rect(MOUSE_PREVX, MOUSE_PREVY, uibox.x, uibox.y, uibox.w, uibox.h)))
			{
				UIBOX_IN = _uibox_id;
				//
				//  THIS WAS AN ATTEMPT AT DOING A BORDER SELECT, BUT THE LAYERING ISN'T RIGHT
				//
				/*if (_uiborder_over > UIBOX_IN) _uiborder_over = -1;
				if (_uiborder_over==-1 && UIBOX_CLICKED_IN==-1 && !point_in_rect(MOUSE_X, MOUSE_Y, uibox.x + 5, uibox.y + 5, uibox.w - 10, uibox.h - 10))
				{
					_uiborder_over = UIBOX_IN;
					SDL_SetRenderDrawColor(RENDERER, 128, 128, 128, 255);
					RECT = { uibox.x + 1, uibox.y + 1, uibox.w - 2, uibox.h - 2 };
					SDL_RenderDrawRect(RENDERER, &RECT);
				}*/
			}
		}

		SDL_SetRenderDrawColor(RENDERER, 0, 0, 0, 255);

		// TEST HUE BAR
		//const SDL_Rect temp_rect{10,10,16,360};
		//SDL_RenderCopy(RENDERER, UI_TEXTURE_HUEBAR, nullptr, &temp_rect);

		FC_Draw(font, RENDERER, 36, 10, "%i", UNDO_LIST.size());
		FC_Draw(font, RENDERER, 36, 30, "%i\n%i", UIBOX_IN, UIBOX_CLICKED_IN);

		SDL_SetRenderDrawColor(RENDERER, 0, 0, 0, 0);
		SDL_RenderPresent(RENDERER);
        
#if __APPLE__
        SDL_Delay(1);
#endif
		//

		// SET FPS
        const Uint64 fps_end = SDL_GetPerformanceCounter();
        const static Uint64 fps_freq = SDL_GetPerformanceFrequency();
        const double fps_seconds = (fps_end - fps_start) / static_cast<double>(fps_freq);
        FPS = reach_tween(FPS, 1 / (float)fps_seconds, 100.0);
        if (fps_rate <= 0) {
            std::cout << " FPS: " << (int)floor(FPS) << '\n';
            fps_rate = 60 * 10;
        } else fps_rate--;
	}

	SDL_Delay(10);

	FC_FreeFont(font);
	FC_FreeFont(font_bold);
	SDL_DestroyRenderer(RENDERER);
	SDL_DestroyWindow(WINDOW);
	SDL_Quit();

	return 0;
}

  //
 //   END   ///////////////////////////////////////////////// ///////  //////   /////    ////     ///      //       /
//
