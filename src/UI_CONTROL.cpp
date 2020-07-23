#include "UI_CONTROL.h"

// manpat: really not a fan of this - this should be cmakes job :(
#ifdef __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

#include "SDL_FontCache.h"
#include "VARIABLES.h"
#include "FUNCTIONS.h"

void SYSTEM_UIBOX_CONTROL()
{
	// RENDER THE UI BOXES
	int16_t t_UIBOX_IN = UIBOX_IN;
	UIBOX_IN = -1;
	int16_t t_ELEMENT_IN = ELEMENT_IN;
	ELEMENT_IN = -1;

	const int16_t _uiboxes_size = UIBOXES.size() - 1;
	SDL_Rect RECT{};
	int16_t _uibox_id = 0;
	for (int16_t i = 0; i <= _uiboxes_size; i++)
	{
		_uibox_id = _uiboxes_size - i;
		UIBOX_INFO* uibox = &UIBOXES[_uibox_id];

		uibox->x = clamp(uibox->x, 0, WINDOW_W - uibox->w);
		uibox->y = clamp(uibox->y, 0, WINDOW_H - uibox->h);

		RECT = { uibox->x, uibox->y, uibox->w, uibox->h };

		SDL_Rect rect{};
		UIBOX_CHAR* _charinfo;

		int _uibox_w = uibox->chr_w;
		int _uibox_h = uibox->chr_h;

		static int _mpx, _mpy;

		// MOUSE IS IN WINDOW
		if (uibox->element_update || ((UIBOX_CLICKED_IN == -1 || UIBOX_CLICKED_IN == t_UIBOX_IN) && (point_in_rect(MOUSE_X, MOUSE_Y, uibox->x, uibox->y, uibox->w, uibox->h)
			|| point_in_rect(MOUSE_PREVX, MOUSE_PREVY, uibox->x, uibox->y, uibox->w, uibox->h))))
		{
			UIBOX_IN = _uibox_id;
			bool _in_grab = (uibox->can_grab && point_in_rect(MOUSE_X, MOUSE_Y, uibox->x, uibox->y, uibox->w - (FONT_CHRW * 4), FONT_CHRH));
			bool _in_shrink = (uibox->can_shrink && point_in_rect(MOUSE_X, MOUSE_Y, uibox->x + (uibox->w - (FONT_CHRW * 4)), uibox->y, FONT_CHRW * 3, FONT_CHRH));

			bool _check_grab = (_in_grab && !uibox->in_grab);
			bool _check_shrink = (_in_shrink && !uibox->in_shrink);

			if (_check_grab || _check_shrink) // IF YOU'RE IN EITHER TOPBAR BUTTON
			{
				for (uint16_t j = (_check_shrink ? (_uibox_w - 4) : 0); j < (_check_shrink ? (_uibox_w - 1) : (_uibox_w - 4)); j++)
				{
					_charinfo = &uibox->charinfo[j];
					uibox_setchar(_charinfo, uibox, j, _charinfo->chr, COL_BLACK, COL_ACCENT, 1);
				}
				if (_in_grab) uibox->in_grab = true;
				if (_in_shrink) uibox->in_shrink = true;
				uibox->update = true;
			}

			_check_grab = (!_in_grab && uibox->in_grab);
			_check_shrink = (!_in_shrink && uibox->in_shrink);

			if (_check_grab || _check_shrink) // IF YOU'RE NOT IN EITHER TOPBAR BUTTON
			{
				for (uint16_t j = (_check_shrink ? (_uibox_w - 4) : 0); j < (_check_shrink ? (_uibox_w - 1) : (_uibox_w - 4)); j++)
				{
					_charinfo = &uibox->charinfo[j];
					uibox_setchar(_charinfo, uibox, j, _charinfo->chr, COL_WHITE, COL_BGUPDATE, 1);
				}
				uibox->in_grab = false;
				uibox->in_shrink = false;
				uibox->update = true;
			}


			if ((uibox->element_update || UIBOX_IN == UIBOX_PREVIN) && !uibox->shrink)
			{
				UIBOX_ELEMENT* _element;
				for (int e = 0; e < uibox->element.size(); e++)
				{
					_element = &uibox->element[e];
					if ((uibox->element_update && ((_element->input_int == nullptr && (bool)(*_element->input_bool)) || (_element->input_int != nullptr && *_element->input_int == _element->input_int_var))) ||
						((!_element->is_pos && point_in_rect(MOUSE_X, MOUSE_Y, uibox->x + (FONT_CHRW * 2), uibox->y + ((e + 2) * FONT_CHRH), uibox->w - (FONT_CHRW * 4), FONT_CHRH)) ||
							_element->is_pos && point_in_rect(MOUSE_X, MOUSE_Y, uibox->x + (FONT_CHRW * _element->px), uibox->y + (FONT_CHRH * _element->py), FONT_CHRW * _element->text.size(), FONT_CHRH)))
					{
						if (!uibox->element_update) ELEMENT_IN = e;
						if (_element->over && !uibox->element_update) continue; // if mouse is already over, don't update

						for (uint16_t ej = 0; ej < _uibox_w; ej++)
						{
							if (((!_element->is_pos) && ej >= (_uibox_w - 4)) || ((_element->is_pos) && ej >= _element->text.size())) break;
							uint16_t _tj = (!_element->is_pos) ? (ej + 2 + ((e + 2) * _uibox_w)) : (_element->px + (_element->py * _uibox_w) + ej);
							_charinfo = &uibox->charinfo[_tj];
							_charinfo->col = COL_WHITE;// COL_BLACK;// (!_element->is_pos) ? COL_BLACK : (!(bool)(*_element->input_bool) ? COL_BLACK : COLOR{ 255,0,64,255 });
							_charinfo->bg_col = COLOR{ 255,0,64,255 };
							if (((_element->input_int == nullptr && (bool)(*_element->input_bool)) || (_element->input_int != nullptr && *_element->input_int == _element->input_int_var)) || uibox->element_update)
							{
								if (ej < _element->over_text.size()) _charinfo->chr = (_element->over_text.c_str())[ej]; else _charinfo->chr = 32;
							}
							uibox->update_stack.insert(uibox->update_stack.begin() + (rand() % (uibox->update_stack.size() + 1)), _tj);
						}

						uibox->update = true;
						_element->over = true;
					}
					else
						if (_element->over || uibox->element_update)
						{
							if (((_element->input_int == nullptr && !(bool)(*_element->input_bool)) || (_element->input_int != nullptr && *_element->input_int != _element->input_int_var)) || uibox->element_update)
							{
								for (uint16_t ej = 0; ej < _uibox_w; ej++)
								{
									if (((!_element->is_pos) && ej >= (_uibox_w - 4)) || ((_element->is_pos) && ej >= _element->text.size())) break;
									uint16_t _tj = (!_element->is_pos) ? (ej + 2 + ((e + 2) * _uibox_w)) : (_element->px + (_element->py * _uibox_w) + ej);
									_charinfo = &uibox->charinfo[_tj];
									_charinfo->col = COL_WHITE;
									_charinfo->bg_col = COLOR{ 0,0,0,1 };
									if (((_element->input_int == nullptr && !(bool)(*_element->input_bool)) || (_element->input_int != nullptr && *_element->input_int != _element->input_int_var)) || uibox->element_update)
									{
										if (ej < _element->text.size()) _charinfo->chr = (_element->text.c_str())[ej]; else _charinfo->chr = 32;
									}
									uibox->update_stack.insert(uibox->update_stack.begin() + (rand() % (uibox->update_stack.size() + 1)), _tj);
								}
								uibox->update = true;
							}
							_element->over = false;
						}
				}
				uibox->element_update = false;
			}
		}

		if (uibox->update)
		{
			if (uibox->texture == nullptr)
			{
				uibox->texture = SDL_CreateTexture(RENDERER, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, uibox->w, uibox->h);
			}

			SDL_SetTextureBlendMode(uibox->texture, SDL_BLENDMODE_BLEND);
			SDL_SetTextureBlendMode(FONTMAP, SDL_BLENDMODE_BLEND);
			SDL_SetRenderTarget(RENDERER, uibox->texture);
			SDL_SetRenderDrawColor(RENDERER, 255, 255, 255, 255);

			SDL_Rect chr_rect{};

			uint16_t j;
			do { // do the whole loop so we don't have to wait for the windows to appear upon opening the app
				if (uibox->update_stack.size())
				{
					if (uibox->update_tick <= 0)
					{
						do
						{
							j = uibox->update_stack.front();
							uibox->update_stack.pop_front();
							_charinfo = &uibox->charinfo[j];
						} while (uibox->update_stack.size() > 1 && _charinfo->chr == 32 && !_charinfo->bg_col.a); // skip Space chars, but not if it has to draw a bg

						rect = { (j % _uibox_w) * FONT_CHRW, (j / _uibox_w) * FONT_CHRH, FONT_CHRW, FONT_CHRH };

						if (_charinfo->bg_col.a) // if the bg_col isn't 0
						{
							chr_rect = { 0xdb * FONT_CHRW, 0, FONT_CHRW, FONT_CHRH }; // the SOLID_BLOCK char is the bg block
							SDL_SetTextureColorMod(FONTMAP, _charinfo->bg_col.r, _charinfo->bg_col.g, _charinfo->bg_col.b);
							SDL_RenderCopy(RENDERER, FONTMAP, &chr_rect, &rect);
							if (_charinfo->bg_col.a == 1) _charinfo->bg_col.a = 0; // if it's a quick "return to black", only do it once
						}

						chr_rect = { _charinfo->chr * FONT_CHRW, 0, FONT_CHRW, FONT_CHRH };
						SDL_SetTextureColorMod(FONTMAP, _charinfo->col.r, _charinfo->col.g, _charinfo->col.b);
						SDL_RenderCopy(RENDERER, FONTMAP, &chr_rect, &rect);
						uibox->update_tick = 4; // Can be any number. Bigger number = slower manifest animation
					}
					else uibox->update_tick--;
				}
				else
				{
					uibox->update = false;
					uibox->creation_update = false;
				}
			} while (uibox->creation_update); // only happens the moment a window is created

			SDL_SetRenderTarget(RENDERER, NULL);
		}

		if (uibox->shrink)
		{
			SDL_Rect shrink_rect = { 0,0,uibox->w,FONT_CHRH };
			rect = { uibox->x, uibox->y, uibox->w, FONT_CHRH };
			SDL_RenderCopy(RENDERER, uibox->texture, &shrink_rect, &rect);
		}
		else
		{
			rect = { uibox->x, uibox->y, uibox->w, uibox->h };
			SDL_RenderCopy(RENDERER, uibox->texture, NULL, &rect);
		}
	}

	UIBOX_PREVIN = UIBOX_IN;
}