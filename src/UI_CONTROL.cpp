#include "UI_CONTROL.h"

// manpat: really not a fan of this - this should be cmakes job :(
#ifdef __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

#include "VARIABLES.h"
#include "FUNCTIONS.h"
#include <iostream>

SDL_Texture* UI_TEXTURE_HUEBAR;
COLOR* UI_PIXELS_HUEBAR;

namespace
{
	int16_t UIBOX_IN = -1;
	int16_t UIBOX_PREVIN = -1;
	int16_t UIBOX_CLICKED_IN = -1;
	int16_t UIBOX_PANX = 0;
	int16_t UIBOX_PANY = 0;

	int16_t ELEMENT_IN = -1;
	int16_t ELEMENT_CLICKED_IN = -1;

	std::vector<std::unique_ptr<UIBOX_INFO>> UIBOXES;
}


void SYSTEM_UIBOX_UPDATE()
{
	// RENDER THE UI BOXES
	int16_t t_UIBOX_IN = UIBOX_IN;
	UIBOX_IN = -1;
	ELEMENT_IN = -1;

	for (int16_t i = 0; i < (int)UIBOXES.size(); i++)
	{
		int _uibox_id = UIBOXES.size() - i - 1;
		UIBOX_INFO* uibox = UIBOXES[_uibox_id].get();

		uibox->x = clamp(uibox->x, 0, WINDOW_W - uibox->w);
		uibox->y = clamp(uibox->y, 0, WINDOW_H - uibox->h);

		UIBOX_CHAR* _charinfo;

		int _uibox_w = uibox->chr_w;

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
					uibox_set_char(_charinfo, uibox, j, _charinfo->chr, COL_BLACK, COL_ACCENT, 1);
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
					uibox_set_char(_charinfo, uibox, j, _charinfo->chr, COL_WHITE, COL_BGUPDATE, 1);
				}
				uibox->in_grab = false;
				uibox->in_shrink = false;
				uibox->update = true;
			}

			if ((uibox->element_update || UIBOX_IN == UIBOX_PREVIN) && !uibox->shrink && !uibox->element_list.empty())
			{
				UIBOX_ELEMENT_MAIN* _element;
				//for (int e = 0; e < (int)uibox->element.size(); e++)
				for (int e = 0; e < uibox->element_list.size(); e++)
				{
					_element = uibox->element_list[e].get();
					//std::cout << _element->y << std::endl;
					if (point_in_rect(MOUSE_X, MOUSE_Y,
						uibox->x + (FONT_CHRW * _element->x), uibox->y + (FONT_CHRH * _element->y),
						FONT_CHRW * _element->text.size(), FONT_CHRH))
					{
						ELEMENT_IN = e;
						//std::cout << _element->text.size() << std::endl;
					}

					/*_element = &uibox->element[e];
					if ((uibox->element_update && ((_element->input_int == nullptr && (bool)(*_element->input_bool)) || (_element->input_int != nullptr && *_element->input_int == _element->input_int_var))) ||
						((!_element->is_pos && point_in_rect(MOUSE_X, MOUSE_Y, uibox->x + (FONT_CHRW * 2), uibox->y + ((e + 2) * FONT_CHRH), uibox->w - (FONT_CHRW * 4), FONT_CHRH)) ||
							(_element->is_pos && point_in_rect(MOUSE_X, MOUSE_Y, uibox->x + (FONT_CHRW * _element->px), uibox->y + (FONT_CHRH * _element->py), FONT_CHRW * _element->text.size(), FONT_CHRH))))
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
					{
						if (_element->over || uibox->element_update)
						{
							if (((_element->input_int == nullptr && !(*_element->input_bool)) || (_element->input_int != nullptr && *_element->input_int != _element->input_int_var)) || uibox->element_update)
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
					}*/
				}
			}
			uibox->element_update = false;
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

						SDL_Rect rect = { (j % _uibox_w) * FONT_CHRW, (j / _uibox_w) * FONT_CHRH, FONT_CHRW, FONT_CHRH };

						if (_charinfo->bg_col.a) // if the bg_col isn't 0
						{
							SDL_Rect chr_rect = { 0xdb * FONT_CHRW, 0, FONT_CHRW, FONT_CHRH }; // the SOLID_BLOCK char is the bg block
							SDL_SetTextureColorMod(FONTMAP, _charinfo->bg_col.r, _charinfo->bg_col.g, _charinfo->bg_col.b);
							SDL_RenderCopy(RENDERER, FONTMAP, &chr_rect, &rect);
							if (_charinfo->bg_col.a == 1) _charinfo->bg_col.a = 0; // if it's a quick "return to black", only do it once
						}

						SDL_Rect chr_rect = { _charinfo->chr * FONT_CHRW, 0, FONT_CHRW, FONT_CHRH };
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
			SDL_Rect rect = { uibox->x, uibox->y, uibox->w, FONT_CHRH };
			SDL_RenderCopy(RENDERER, uibox->texture, &shrink_rect, &rect);
		}
		else
		{
			SDL_Rect rect = { uibox->x, uibox->y, uibox->w, uibox->h };
			SDL_RenderCopy(RENDERER, uibox->texture, NULL, &rect);
		}
	}

	UIBOX_PREVIN = UIBOX_IN;
}



bool SYSTEM_UIBOX_HANDLE_MOUSE_DOWN(bool is_click, int mouse_x, int mouse_y)
{
	if (UIBOX_IN < 0)
	{
		return false;
	}

	if (is_click)
	{
		if (ELEMENT_IN >= 0)
		{
			uibox_update_element(UIBOX_IN, ELEMENT_IN);
		}

		move_to_end(UIBOXES, UIBOX_IN); // move window to end
		std::rotate(UIBOXES.rbegin(), UIBOXES.rbegin() + 1, UIBOXES.rend()); // then rotate once more to move to start
		// There's probably a better way of doing this
		UIBOX_IN = 0;// UIBOXES.size() - 1;
		// This was originally putting the window at the end of the list,
		// but I made it so it moves it to the start

		UIBOX_INFO* uibox_click = UIBOXES[UIBOX_IN].get();

		// grab/pan variables
		UIBOX_CLICKED_IN = UIBOX_IN;
		uibox_click->grab = (uibox_click->in_grab);
		if (uibox_click->can_grab && uibox_click->grab && uibox_click->in_grab)
		{
			UIBOX_PANX = (int16_t)(mouse_x - uibox_click->x);
			UIBOX_PANY = (int16_t)(mouse_y - uibox_click->y);
		}

		// shrink [↓]
		if (!uibox_click->in_grab && !uibox_click->grab && uibox_click->in_shrink)
		{
			UIBOX_CHAR* _charinfo;
			uibox_click->shrink = !uibox_click->shrink;
			uibox_click->h = (uibox_click->shrink) ? FONT_CHRH : (uibox_click->chr_h * FONT_CHRH);

			if (uibox_click->shrink)
			{
				/*float deltaX = (WINDOW_W / 2) - (uibox_click->x + (uibox_click->w / 2));
				float deltaY = (WINDOW_H / 2) - uibox_click->y;
				float angle = atan2(deltaY, deltaX);
				float _win_hyp = sqrtf(((WINDOW_W / 2) * (WINDOW_W / 2)) + ((WINDOW_H / 2) * (WINDOW_H / 2)));

				uibox_click->x -= (uint16_t)((9999 * cos(angle)) + (uibox_click->w / 2));
				uibox_click->y -= (uint16_t)(9999 * sin(angle));*/

				uibox_click->h = (uibox_click->shrink) ? FONT_CHRH : (uibox_click->chr_h * FONT_CHRH);
				_charinfo = &uibox_click->charinfo[0];
				_charinfo->chr = CHAR_BOXH;
				_charinfo->bg_col = COLOR{ 0,0,0,1 };
				uibox_click->update_stack.insert(uibox_click->update_stack.begin() + (rand() % (uibox_click->update_stack.size() + 1)), 0);

				_charinfo = &uibox_click->charinfo[uibox_click->chr_w-1];
				_charinfo->chr = CHAR_BOXH;
				_charinfo->bg_col = COLOR{ 0,0,0,1 };
				uibox_click->update_stack.insert(uibox_click->update_stack.begin() + (rand() % (uibox_click->update_stack.size() + 1)), uibox_click->chr_w - 1);

				_charinfo = &uibox_click->charinfo[uibox_click->chr_w - 3];
				_charinfo->chr = CHAR_ARWL;
				_charinfo->bg_col = COLOR{ 255,0,64,1 };
				uibox_click->update_stack.insert(uibox_click->update_stack.begin() + (rand() % (uibox_click->update_stack.size() + 1)), uibox_click->chr_w - 3);
			}
			else
			{
				uibox_click->h = (uibox_click->shrink) ? FONT_CHRH : (uibox_click->chr_h * FONT_CHRH);
				_charinfo = &uibox_click->charinfo[0];
				_charinfo->chr = CHAR_BOXTL;
				_charinfo->bg_col = COLOR{ 0,0,0,1 };
				uibox_click->update_stack.insert(uibox_click->update_stack.begin() + (rand() % (uibox_click->update_stack.size() + 1)), 0);

				_charinfo = &uibox_click->charinfo[uibox_click->chr_w - 1];
				_charinfo->chr = CHAR_BOXTR;
				_charinfo->bg_col = COLOR{ 0,0,0,1 };
				uibox_click->update_stack.insert(uibox_click->update_stack.begin() + (rand() % (uibox_click->update_stack.size() + 1)), uibox_click->chr_w - 1);

				_charinfo = &uibox_click->charinfo[uibox_click->chr_w - 3];
				_charinfo->chr = CHAR_ARWD;
				_charinfo->bg_col = COLOR{ 255,0,64,1 };
				uibox_click->update_stack.insert(uibox_click->update_stack.begin() + (rand() % (uibox_click->update_stack.size() + 1)), uibox_click->chr_w - 3);
			}
			uibox_click->update = true;
			if (!uibox_click->shrink) uibox_click->element_update = true;
			uibox_click->creation_update = true;
		}
	}
	else // !is_click
	{
		UIBOX_INFO* uibox = UIBOXES[UIBOX_IN].get();
		if (uibox->can_grab && uibox->grab && UIBOX_CLICKED_IN == UIBOX_IN)
		{
			// grabbing & moving window
			uibox->x = (MOUSE_X - UIBOX_PANX);
			uibox->y = (MOUSE_Y - UIBOX_PANY);
		}
	}

	return ELEMENT_CLICKED_IN != -1 || UIBOX_CLICKED_IN != -1;
}


void SYSTEM_UIBOX_HANDLE_MOUSE_UP()
{
	UIBOX_CLICKED_IN = -1;
	ELEMENT_CLICKED_IN = -1;
}



UIBOX_INFO* uibox_new(uint16_t _x, uint16_t _y, uint16_t _w, uint16_t _h, bool can_grab, std::string title)
{
	auto new_uibox = std::make_unique<UIBOX_INFO>();

	new_uibox->title = title;
	new_uibox->can_grab = can_grab;
	new_uibox->update = true;
	new_uibox->x = _x;
	new_uibox->y = _y;

	// GET CHARS THAT FIT WINDOW
	new_uibox->chr_w = (int)floor(((float)_w / (float)FONT_CHRW) + 0.5f);
	new_uibox->chr_h = (int)floor(((float)_h / (float)FONT_CHRH) + 0.5f);

	// MAKE WINDOW THE NEW ROUNDED CHAR SIZE
	new_uibox->w = (new_uibox->chr_w * FONT_CHRW);
	new_uibox->h = (new_uibox->chr_h * FONT_CHRH);

	// FILL WINDOW WITH EMPTY CHARS
	for (int j = 0; j < new_uibox->chr_w * new_uibox->chr_h; j++)
	{
		UIBOX_CHAR _chr;
		COLOR _tcol{ 255, 255, 255, 255 };
		_chr.col = _tcol;
		_chr.chr = ' ';
		new_uibox->charinfo.push_back(std::move(_chr));
		new_uibox->update_stack.insert(new_uibox->update_stack.begin() + (rand() % (new_uibox->update_stack.size() + 1)), j);
	}

	// ADD BORDER
	for (int j = 0; j < new_uibox->chr_h; j++)
	{
		for (int i = 0; i < new_uibox->chr_w; i++)
		{
			if ((j > 0 && j < new_uibox->chr_h -1) && (i > 0 && i < new_uibox->chr_w -1))
			{
				continue;
			}
			new_uibox->charinfo[j * new_uibox->chr_w + i].chr = (j == 0) ? ((i == 0) ? CHAR_BOXTL : ((i == new_uibox->chr_w -1) ? CHAR_BOXTR : CHAR_BOXH)) :
				((j == new_uibox->chr_h -1) ? ((i == 0) ? CHAR_BOXBL : ((i == new_uibox->chr_w -1) ? CHAR_BOXBR : CHAR_BOXH)) : CHAR_BOXV);
		}
	}

	// TITLE
	uibox_set_string(new_uibox.get(), STR_NBSP + title + STR_NBSP, 1, 0, COL_WHITE, false);

	// SHRINK BUTTON
	uibox_set_string(new_uibox.get(), STR_NBSP STR_ARWD STR_NBSP, new_uibox->chr_w - 4, 0, COL_WHITE, false);

	new_uibox->texture = nullptr;

	UIBOXES.push_back(std::move(new_uibox));

	return UIBOXES.back().get();
}

void uibox_set_char(UIBOX_CHAR* ci, UIBOX_INFO* ui, uint16_t char_pos, uint8_t _chr, COLOR _col, COLOR _bg_col, bool update)
{
	ci->chr = _chr;
	ci->col = _col;
	ci->bg_col = _bg_col;
	if (!update) return;
	ui->update_stack.insert(ui->update_stack.begin() + (rand() % (ui->update_stack.size() + 1)), char_pos);
	ui->update = true;
}

void uibox_set_string(UIBOX_INFO* uibox, std::string _charlist, uint16_t x, uint16_t y, COLOR col, bool update)
{
	const char* _chars = _charlist.c_str();
	for (uint16_t j = 0; j < _charlist.size(); j++)
	{
		const auto pos = j + (y * uibox->chr_w + x);
		auto _charinfo = &uibox->charinfo[pos];
		uibox_set_char(_charinfo, uibox, pos, _chars[j], col, _charinfo->bg_col, update);
	}
}

/*void uibox_add_element(UIBOX_INFO* uibox, std::string text, std::string over_text, uint8_t type, bool* bool_ptr, uint16_t* int_ptr, uint16_t int_var, bool is_pos, uint16_t px, uint16_t py)
{
	UIBOX_ELEMENT newuibox_interact;
	newuibox_interact.text = text;
	newuibox_interact.over_text = over_text;
	newuibox_interact.type = type;
	newuibox_interact.input_bool = bool_ptr;
	newuibox_interact.input_int = int_ptr;
	newuibox_interact.input_int_var = int_var;
	newuibox_interact.is_pos = is_pos;
	newuibox_interact.px = px;
	newuibox_interact.py = py;
	uibox->element.push_back(std::move(newuibox_interact));

	UIBOX_ELEMENT_BUTTON _checkbox;
	_checkbox.text = "HELLO";

	uibox->element_list.push_back(&_checkbox);

	std::cout << (uibox->element_list[0]->text) << std::endl;
	uibox->element_list[0]->update();

	if (!is_pos)
	{
		uibox_set_string(uibox, text, 2, 2 + (uibox->element.size() - 1), COL_WHITE, false);
	}
	else
	{
		uibox_set_string(uibox, text, px, py, COL_WHITE, false);
	}
}*/

void uibox_add_element_textbox(UIBOX_INFO* uibox, uint16_t x, uint16_t y, std::string text)
{
	std::unique_ptr<UIBOX_ELEMENT_TEXTBOX> _element = std::make_unique<UIBOX_ELEMENT_TEXTBOX>();
	_element->x = x;
	_element->y = y;
	_element->text = text;
	_element->create(uibox);
	uibox->element_list.push_back(std::move(_element));
}

void uibox_add_element_button(UIBOX_INFO* uibox, uint16_t x, uint16_t y, std::string text, uint16_t* input_var, uint16_t button_var)
{
	std::unique_ptr<UIBOX_ELEMENT_BUTTON> _element = std::make_unique<UIBOX_ELEMENT_BUTTON>();
	_element->x = x;
	_element->y = y;
	_element->text = text;
	_element->input_var = input_var;
	_element->button_var = button_var;
	_element->create(uibox);
	uibox->element_list.push_back(std::move(_element));
}

void uibox_add_element_toggle(UIBOX_INFO* uibox, uint16_t x, uint16_t y, std::string text, bool* input_var)
{
	std::unique_ptr<UIBOX_ELEMENT_TOGGLE> _element = std::make_unique<UIBOX_ELEMENT_TOGGLE>();
	_element->x = x;
	_element->y = y;
	_element->text = text;
	_element->create(uibox);
	uibox->element_list.push_back(std::move(_element));
}

void uibox_add_element_slider(UIBOX_INFO* uibox, uint16_t x, uint16_t y, std::string text, uint16_t* input_var)
{
	std::unique_ptr<UIBOX_ELEMENT_SLIDER> _element = std::make_unique<UIBOX_ELEMENT_SLIDER>();
	_element->x = x;
	_element->y = y;
	_element->text = text;
	_element->create(uibox);
	uibox->element_list.push_back(std::move(_element));
}

void uibox_add_element_textinput(UIBOX_INFO* uibox, uint16_t x, uint16_t y, std::string text)
{
	std::unique_ptr<UIBOX_ELEMENT_TEXTINPUT> _element = std::make_unique<UIBOX_ELEMENT_TEXTINPUT>();
	_element->x = x;
	_element->y = y;
	_element->text = text;
	_element->create(uibox);
	uibox->element_list.push_back(std::move(_element));
}

void uibox_add_element_numinput(UIBOX_INFO* uibox, uint16_t x, uint16_t y, std::string text)
{
	std::unique_ptr<UIBOX_ELEMENT_NUMINPUT> _element = std::make_unique<UIBOX_ELEMENT_NUMINPUT>();
	_element->x = x;
	_element->y = y;
	_element->text = text;
	_element->create(uibox);
	uibox->element_list.push_back(std::move(_element));
}

void uibox_update_element(int16_t uibox_in, int16_t element_in)
{
	UIBOX_INFO* uibox = UIBOXES[uibox_in].get();

	uibox->element_list[element_in]->update();
	uibox->element_update = true;
	uibox->creation_update = true; // the whole window updates every time you click
}

int16_t uibox_get_uibox_by_title(std::string title)
{
	for (int16_t i = 0; i < (int)UIBOXES.size(); i++)
	{
		UIBOX_INFO* uibox = UIBOXES[i].get();
		if (uibox->title == title) return i;
	}

	return -1;
}

int16_t uibox_get_element_by_text(int16_t uibox_in, std::string text) {
	UIBOX_INFO* uibox = UIBOXES[uibox_in].get();

	for (int16_t i = 0; i < (int)uibox->element.size(); i++) {
		UIBOX_ELEMENT* uibox_element = &uibox->element[i];
		if (uibox_element->text == text) return i;
	}

	return -1;
}