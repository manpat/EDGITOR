#include "UI_CONTROL.h"

// manpat: really not a fan of this - this should be cmakes job :(
#ifdef __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

#include "VARIABLES.h"
#include "FUNCTIONS.h"
#include "SYSTEM.h"
#include "SUPERSTACK.h"
#include <iostream>
#include <algorithm>

SDL_Texture* UI_TEXTURE_HUEBAR;
COLOR* UI_PIXELS_HUEBAR;

/*namespace
{
	int16_t UIBOX_IN = -1;
	int16_t UIBOX_PREVIN = -1;
	int16_t UIBOX_CLICKED_IN = -1;
	int16_t UIBOX_PANX = 0;
	int16_t UIBOX_PANY = 0;

	int16_t ELEMENT_IN = -1;
	int16_t ELEMENT_CLICKED_IN = -1;

	std::vector<std::unique_ptr<UIBOX_INFO>> UIBOXES;
}*/

int16_t UIBOX_IN = -1;
int16_t UIBOX_PREVIN = -1;
int16_t UIBOX_CLICKED_IN = -1;
int16_t UIBOX_PANX = 0;
int16_t UIBOX_PANY = 0;

int16_t ELEMENT_IN = -1;
int16_t ELEMENT_CLICKED_IN = -1;
bool ELEMENT_TOGGLE_BOOL = false;

std::vector<std::unique_ptr<UIBOX_INFO>> UIBOXES;
std::vector<std::pair<std::string, bool>> PATH_FILES;


// NOTE(manpat): This is not strictly correct but is the best I can do for now.
// The correct solution would be to use std::filesystem::path everywhere instead of doing path stuff
// manually, but that's a much larger job than I have time for atm
const auto PATH_SEP = static_cast<char>(std::filesystem::path::preferred_separator);


void UPDATE_PATH_FILES()
{
	int _folders = 0;
	PATH_FILES.clear();
	for (auto& p : std::filesystem::directory_iterator(std::filesystem::current_path()))
	{
		std::string _path = p.path().string();
		std::string _name;
		for (auto _htap = _path.crbegin(); _htap != _path.crend(); ++_htap)
		{
			if (*_htap == PATH_SEP) break;
			_name.insert(_name.begin(), *_htap);
		}

		if (_name[0] != '.')
		{
			if (p.is_directory())
			{
				PATH_FILES.insert(PATH_FILES.begin() + _folders, std::pair{ _name,1 });
				_folders += 1;
			}
			else
				PATH_FILES.push_back(std::pair{ _name,0 });
		}
	}
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

		uint16_t _uibox_w = uibox->chr_w;

		// MOUSE IS IN WINDOW
		bool _in_uibox = (point_in_rect(MOUSE_X, MOUSE_Y, uibox->x, uibox->y, uibox->w, uibox->h) || point_in_rect(MOUSE_PREVX, MOUSE_PREVY, uibox->x, uibox->y, uibox->w, uibox->h));

		// SCROLLING ELEMENTS
		if (_in_uibox && uibox->can_scroll && uibox->scroll_element_list.size() > 0 && (MOUSEWHEEL_X != 0 || MOUSEWHEEL_Y != 0))
		{
			/*int16_t _tscrollx = uibox->scroll_x;
			uibox->scroll_x = clamp(uibox->scroll_x + MOUSEWHEEL_X, 0, uibox->scoll_maxx);
			if (_tscrollx != uibox->scroll_x)
			{
				// UPDATE X-scroll
			}*/

			int16_t _tscrolly = uibox->scroll_y;
			uibox->scroll_y = clamp(uibox->scroll_y - MOUSEWHEEL_Y, 0, uibox->scroll_element_list_size - 1);

			if (_tscrolly != uibox->scroll_y)
			{
				int _tx, _ty;
				std::string _tchar;
				for (int si = 0; si < uibox->scroll_element_list.size(); si++)
				{
					_tx = uibox->element_list[uibox->scroll_element_list[si]]->x;
					_ty = uibox->element_list[uibox->scroll_element_list[si]]->y;
					_tchar = uibox->element_list[uibox->scroll_element_list[si]]->text;
					for (uint16_t sj = 0; sj < _tchar.size(); sj++)
					{
						if (((sj + (_ty * uibox->chr_w + _tx)) % uibox->chr_w) > (uibox->chr_w - 4)) break;
						uibox_set_char(UIBOX_FILES, sj + (_ty * UIBOX_FILES->chr_w + _tx), 32, COL_EMPTY, COL_BGUPDATE, 1);
					}
				}

				UIBOX_FILES->element_update = true;
				UIBOX_FILES->creation_update = true;
				UIBOX_FILES->update = true;

				uibox_update_files();
			}
		}

		if (UIBOX_CLICKED_IN == -1 || UIBOX_CLICKED_IN == t_UIBOX_IN)
		{
			if (_in_uibox)
			{
				UIBOX_IN = _uibox_id;
				bool _in_grab = (uibox->can_grab && UIBOX_IN == t_UIBOX_IN && point_in_rect(MOUSE_X, MOUSE_Y, uibox->x, uibox->y, uibox->w - (FONT_CHRW * 4), FONT_CHRH));
				bool _in_shrink = (uibox->can_shrink && UIBOX_IN == t_UIBOX_IN && point_in_rect(MOUSE_X, MOUSE_Y, uibox->x + (uibox->w - (FONT_CHRW * 4)), uibox->y, FONT_CHRW * 3, FONT_CHRH));

				bool _check_grab = (_in_grab && !uibox->in_grab);
				bool _check_shrink = (_in_shrink && !uibox->in_shrink);

				if (_check_grab || _check_shrink) // IF YOU'RE IN EITHER TOPBAR BUTTON
				{
					for (uint16_t j = (_check_shrink ? (_uibox_w - 4) : 0); j < (_check_shrink ? (_uibox_w - 1) : (_uibox_w - 4)); j++)
					{
						//_charinfo = &uibox->charinfo[j];
						uibox_set_char(uibox, j, 0, COL_EMPTY, COL_ACCENT, 1);
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
						//_charinfo = &uibox->charinfo[j];
						uibox_set_char(uibox, j, 0, COL_EMPTY, COL_BGUPDATE, 1);
					}
					uibox->in_grab = false;
					uibox->in_shrink = false;
					uibox->update = true;
				}
			}

			// ELEMENT UPDATE
			if ((uibox->element_update || UIBOX_IN == t_UIBOX_IN) && !uibox->shrink && !uibox->element_list.empty())
			{
				UIBOX_ELEMENT_MAIN* _element;
				bool _t_over;
				bool _t_is_sel;
				bool _t_update = 0;

				for (uint16_t e = 0; e < uibox->element_list.size(); e++)
				{
					_t_update = 0; // temp update variable

					_element = uibox->element_list[e].get();
					if (_element->const_update)
					{
						_element->update(uibox);
						continue;
					}

					_t_over = (point_in_rect(MOUSE_X, MOUSE_Y,
						uibox->x + (FONT_CHRW * _element->x), uibox->y + (FONT_CHRH * _element->y),
						FONT_CHRW * _element->w, FONT_CHRH * _element->h));

					if (_t_over)
					{
						ELEMENT_IN = e;
					}
					if (_element->prev_over != _t_over)
					{
						_element->prev_over = _t_over;
						_t_update = 1;
					}

					_t_is_sel = _element->is_sel();

					if (_element->prev_sel != _t_is_sel)
					{
						_element->prev_sel = _t_is_sel;
						_t_update = 1;
					}

					if (uibox->element_update) _t_update = 1;

					if (_t_update)
					{
						_element->update(uibox);
					}
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
							j = uibox->update_stack.top();
							uibox->update_stack.pop();
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
						uibox->update_tick = UIBOX_UPDATE_TICK;
						_charinfo->update = false;
					}
					else uibox->update_tick--;
				}
				else
				{
					uibox->update = false;
					uibox->creation_update = false;
				}
			} while (uibox->creation_update); // only happens the moment a window is created

			SDL_SetRenderTarget(RENDERER, nullptr);
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
			SDL_RenderCopy(RENDERER, uibox->texture, nullptr, &rect);
		}
	}

	//UIBOX_PREVIN = UIBOX_IN;
}



bool SYSTEM_UIBOX_HANDLE_MOUSE_DOWN(bool is_click, int mouse_x, int mouse_y)
{
	if (UIBOX_IN < 0)
	{
		return false;
	}

	if (is_click && ELEMENT_IN >= 0)
	{
		uibox_update_element(UIBOX_IN, ELEMENT_IN);
	}

	if (MOUSEBUTTON_PRESSED_LEFT)
	{
		/*if (ELEMENT_IN >= 0)
		{
			uibox_update_element(UIBOX_IN, ELEMENT_IN);
		}*/

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
			uibox_click->shrink = !uibox_click->shrink;
			uibox_click->h = (uibox_click->shrink) ? FONT_CHRH : (uibox_click->chr_h * FONT_CHRH);

			if (uibox_click->shrink)
			{
				uibox_set_char(uibox_click, 0, CHAR_BOXH, COL_EMPTY, COL_BGUPDATE, 1);
				uibox_set_char(uibox_click, uibox_click->chr_w - 1, CHAR_BOXH, COL_EMPTY, COL_BGUPDATE, 1);
				uibox_set_char(uibox_click, uibox_click->chr_w - 3, CHAR_ARWL, COL_EMPTY, COL_EMPTY, 1);
			}
			else
			{
				uibox_set_char(uibox_click, 0, CHAR_BOXTL, COL_EMPTY, COL_BGUPDATE, 1);
				uibox_set_char(uibox_click, uibox_click->chr_w - 1, CHAR_BOXTR, COL_EMPTY, COL_BGUPDATE, 1);
				uibox_set_char(uibox_click, uibox_click->chr_w - 3, CHAR_ARWD, COL_EMPTY, COL_EMPTY, 1);
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

void uibox_update_files()
{
	if (!UIBOX_FILES->scroll_element_list.empty() && !UIBOX_FILES->element_list.empty())
	{
		int _offset = 0;
		for (int i = 0; i < UIBOX_FILES->scroll_element_list.size(); i++)
		{
			UIBOX_FILES->element_list.erase(UIBOX_FILES->element_list.begin() + UIBOX_FILES->scroll_element_list[i - _offset]);
			++_offset;
		}
	}

	UIBOX_FILES->can_scroll = true;
	UIBOX_FILES->scroll_element_list.clear();
	if (UIBOX_FILES->scroll_element_list_create)
	{
		UIBOX_FILES->element_list.clear();
		if (CURRENT_PATH.back() != PATH_SEP)
		{
			CURRENT_PATH += PATH_SEP;
			std::filesystem::current_path(CURRENT_PATH);
		}
		std::string _currentpath = CURRENT_PATH;
		std::string _name;
		int _tt = 0;
		for (auto _htap = _currentpath.crbegin(); _htap != _currentpath.crend(); ++_htap)
		{
			if (*_htap == PATH_SEP && _tt)
			{
				uibox_add_element_button_files_goto(UIBOX_FILES, (_currentpath.size() - _tt) + 2, 2, 0, 1, _name, &CURRENT_PATH, CURRENT_PATH.substr(0, (_currentpath.size() - _tt) + _name.size()));
				_name.clear();
			}
			_tt++;
			_name.insert(_name.begin(), *_htap);
		}

		uibox_add_element_button_files_goto(UIBOX_FILES, (_currentpath.size() - _tt) + 2, 2, 0, 1, _name, &CURRENT_PATH, CURRENT_PATH.substr(0, (_currentpath.size() - _tt) + _name.size()));
		uibox_add_element_textbox(UIBOX_FILES, 2, 3, STR_LINEV);
		UPDATE_PATH_FILES();
	}

	for (int _file = 0; _file < PATH_FILES.size(); ++_file)
	{
		if (UIBOX_FILES->scroll_element_list_create) UIBOX_FILES->scroll_element_list_size += 1;
		if (!((_file - UIBOX_FILES->scroll_y) >= 0 && ((4 + _file) - UIBOX_FILES->scroll_y) < (UIBOX_FILES->chr_h - 1))) continue;

		if (PATH_FILES[_file].first.size() > (UIBOX_FILES->chr_w - 6)) // shorten name if it's wider than uibox
		{
			PATH_FILES[_file].first.erase(UIBOX_FILES->chr_w - 10);
			PATH_FILES[_file].first += "...";
		}

		if (PATH_FILES[_file].second)
		{
			uibox_add_element_button_files_goto(UIBOX_FILES, 2, (4 + _file) - UIBOX_FILES->scroll_y, 0, 1, (_file < (PATH_FILES.size() - 1) ? STR_LINEBL STR_ARWR " " : STR_LINEBL STR_ARWR " ") + PATH_FILES[_file].first + PATH_SEP, &CURRENT_PATH, CURRENT_PATH + PATH_FILES[_file].first);
		}
		else
		{
			std::string _tstr = PATH_FILES[_file].first.substr(PATH_FILES[_file].first.size() - 3, 3);
			if (_tstr == "png" || _tstr == "PNG")
			{
				uibox_add_element_button_files_load(UIBOX_FILES, 2, (4 + _file) - UIBOX_FILES->scroll_y, 0, 1, (_file < (PATH_FILES.size() - 1) ? "\xc5 " : "\xc1 ") + PATH_FILES[_file].first, CURRENT_PATH + PATH_FILES[_file].first, PATH_FILES[_file].first);
			}
			else
				uibox_add_element_textbox(UIBOX_FILES, 2, (4 + _file) - UIBOX_FILES->scroll_y, "\xb3 " + PATH_FILES[_file].first);
		}
		UIBOX_FILES->scroll_element_list.push_back(UIBOX_FILES->element_list.size() - 1);
	}
	UIBOX_FILES->scroll_element_list_create = 0;
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
	new_uibox->update_stack.reserve(new_uibox->chr_w * new_uibox->chr_h);

	// MAKE WINDOW THE NEW ROUNDED CHAR SIZE
	new_uibox->w = (new_uibox->chr_w * FONT_CHRW);
	new_uibox->h = (new_uibox->chr_h * FONT_CHRH);

	new_uibox->construct();

	new_uibox->texture = nullptr;

	UIBOXES.push_back(std::move(new_uibox));

	return UIBOXES.back().get();
}

void uibox_set_char(UIBOX_INFO* ui, uint16_t char_pos, uint8_t _chr, COLOR _col, COLOR _bg_col, bool update)
{
	UIBOX_CHAR* ci = &ui->charinfo[char_pos];
	if (_chr) ci->chr = _chr; // set _chr to 0 to not update it
	if (_col!=COL_EMPTY) ci->col = _col;
	if (_bg_col != COL_EMPTY) ci->bg_col = _bg_col;
	if (!update && !ci->update) return;
	ci->update = true;
	ui->update_stack.push(char_pos);// .insert(ui->update_stack.begin() + (rand() % (ui->update_stack.size() + 1)), char_pos);
	ui->update = true;
}

void uibox_set_string(UIBOX_INFO* uibox, std::string _charlist, uint16_t x, uint16_t y, COLOR col, bool update)
{
	const char* _chars = _charlist.c_str();
	for (uint16_t j = 0; j < _charlist.size(); j++)
	{
		/*if (((j + (y * uibox->chr_w + x)) % uibox->chr_w) > (uibox->chr_w - 4))
		{
			uibox_set_char(uibox, j + (y * uibox->chr_w + x)-3, '.', COL_EMPTY, COL_EMPTY, 0);
			uibox_set_char(uibox, j + (y * uibox->chr_w + x)-2, '.', COL_EMPTY, COL_EMPTY, 0);
			uibox_set_char(uibox, j + (y * uibox->chr_w + x)-1, '.', COL_EMPTY, COL_EMPTY, 0);
			break;
		}*/
		//if ((j + x) > (uibox->chr_w - 4)) break;
		uibox_set_char(uibox, j + (y * uibox->chr_w + x), _chars[j], col, COL_EMPTY, update);
	}
}

inline void uibox_element_setxywh(UIBOX_INFO* uibox, std::shared_ptr<UIBOX_ELEMENT_MAIN> _element, uint16_t x, uint16_t y, int16_t w, int16_t h, std::string text, std::string sel_text)
{
	_element->x = (w == -1) ? 2 : x;
	_element->y = y;
	_element->w = (w == -1) ? (uibox->chr_w - 4) : ((w == 0) ? std::max(sel_text.size(), text.size()) : w);
	_element->h = (h == -1) ? (uibox->chr_h - 4) : h;
}

void uibox_add_element_textbox(UIBOX_INFO* uibox, uint16_t x, uint16_t y, std::string text)
{
	std::shared_ptr<UIBOX_ELEMENT_TEXTBOX> _element = std::make_shared<UIBOX_ELEMENT_TEXTBOX>();
	_element->x = x;
	_element->y = y;
	_element->text = text;
	_element->create(uibox);
	uibox->element_list.push_back(std::move(_element));
}

void uibox_add_element_varbox(UIBOX_INFO* uibox, uint16_t x, uint16_t y, std::string text, uint16_t* input_var, uint16_t var)
{
	std::shared_ptr<UIBOX_ELEMENT_VARBOX> _element = std::make_shared<UIBOX_ELEMENT_VARBOX>();
	_element->x = x;
	_element->y = y;
	_element->text = text;
	_element->input_var = input_var;
	_element->var = var;
	_element->create(uibox);
	uibox->element_list.push_back(std::move(_element));
}

void uibox_add_element_varbox_u8(UIBOX_INFO* uibox, uint16_t x, uint16_t y, std::string text, uint8_t* input_var, uint8_t var)
{
	std::shared_ptr<UIBOX_ELEMENT_VARBOX_U8> _element = std::make_shared<UIBOX_ELEMENT_VARBOX_U8>();
	_element->x = x;
	_element->y = y;
	_element->text = text;
	_element->input_var = input_var;
	_element->var = var;
	_element->create(uibox);
	uibox->element_list.push_back(std::move(_element));
}

void uibox_add_element_varbox_f(UIBOX_INFO* uibox, uint16_t x, uint16_t y, std::string text, float* input_var, float var)
{
	std::shared_ptr<UIBOX_ELEMENT_VARBOX_F> _element = std::make_shared<UIBOX_ELEMENT_VARBOX_F>();
	_element->x = x;
	_element->y = y;
	_element->text = text;
	_element->input_var = input_var;
	_element->var = var;
	_element->create(uibox);
	uibox->element_list.push_back(std::move(_element));
}

void uibox_add_element_button(UIBOX_INFO* uibox, uint16_t x, uint16_t y, int16_t w, int16_t h, std::string text, std::string sel_text, uint16_t* input_var, uint16_t button_var)
{
	std::shared_ptr<UIBOX_ELEMENT_BUTTON> _element = std::make_shared<UIBOX_ELEMENT_BUTTON>();
	uibox_element_setxywh(uibox, _element, x, y, w, h, text, sel_text);

	_element->text = text;
	_element->sel_text = sel_text;
	_element->input_var = input_var;
	_element->button_var = button_var;
	_element->create(uibox);
	uibox->element_list.push_back(std::move(_element));
}

void uibox_add_element_button_u8(UIBOX_INFO* uibox, uint16_t x, uint16_t y, int16_t w, int16_t h, std::string text, std::string sel_text, uint8_t* input_var, uint8_t button_var)
{
	std::shared_ptr<UIBOX_ELEMENT_BUTTON_U8> _element = std::make_shared<UIBOX_ELEMENT_BUTTON_U8>();
	uibox_element_setxywh(uibox, _element, x, y, w, h, text, sel_text);

	_element->text = text;
	_element->sel_text = sel_text;
	_element->input_var = input_var;
	_element->button_var = button_var;
	_element->create(uibox);
	uibox->element_list.push_back(std::move(_element));
}

void uibox_add_element_button_string(UIBOX_INFO* uibox, uint16_t x, uint16_t y, int16_t w, int16_t h, std::string text, std::string sel_text, std::string* input_var, std::string button_var)
{
	std::shared_ptr<UIBOX_ELEMENT_BUTTON_STRING> _element = std::make_shared<UIBOX_ELEMENT_BUTTON_STRING>();
	uibox_element_setxywh(uibox, _element, x, y, w, h, text, sel_text);

	_element->text = text;
	_element->sel_text = sel_text;
	_element->input_var = input_var;
	_element->button_var = button_var;
	_element->create(uibox);
	uibox->element_list.push_back(std::move(_element));
}

void uibox_add_element_button_files_goto(UIBOX_INFO* uibox, uint16_t x, uint16_t y, int16_t w, int16_t h, std::string text, std::string* input_var, std::string button_var)
{
	if (x + text.size() > (uibox->chr_w - 1)) return;
	std::shared_ptr<UIBOX_ELEMENT_BUTTON_FILES_GOTO> _element = std::make_shared<UIBOX_ELEMENT_BUTTON_FILES_GOTO>();
	uibox_element_setxywh(uibox, _element, x, y, w, h, text, "");

	_element->text = text;
	_element->sel_text = text;
	_element->input_var = input_var;
	_element->button_var = button_var;
	_element->create(uibox);
	uibox->element_list.push_back(std::move(_element));
}

void uibox_add_element_button_files_load(UIBOX_INFO* uibox, uint16_t x, uint16_t y, int16_t w, int16_t h, std::string text, std::string path, std::string file)
{
	if (x + text.size() > (uibox->chr_w - 1)) return;
	std::shared_ptr<UIBOX_ELEMENT_BUTTON_FILES_LOAD> _element = std::make_shared<UIBOX_ELEMENT_BUTTON_FILES_LOAD>();
	uibox_element_setxywh(uibox, _element, x, y, w, h, text, "");

	_element->text = text;
	_element->sel_text = text;
	_element->path = path;
	_element->file = file;
	_element->create(uibox);
	uibox->element_list.push_back(std::move(_element));
}

void uibox_add_element_toggle(UIBOX_INFO* uibox, uint16_t x, uint16_t y, int16_t w, int16_t h, std::string text, std::string sel_text, bool* input_var)
{
	std::shared_ptr<UIBOX_ELEMENT_TOGGLE> _element = std::make_shared<UIBOX_ELEMENT_TOGGLE>();
	uibox_element_setxywh(uibox, _element, x, y, w, h, text, sel_text);

	_element->text = text;
	_element->sel_text = sel_text;
	_element->input_var = input_var;
	_element->create(uibox);
	uibox->element_list.push_back(std::move(_element));
}

void uibox_add_element_slider(UIBOX_INFO* uibox, uint16_t x, uint16_t y, std::string text, uint16_t* input_var)
{
	std::shared_ptr<UIBOX_ELEMENT_SLIDER> _element = std::make_shared<UIBOX_ELEMENT_SLIDER>();
	_element->x = x;
	_element->y = y;
	_element->text = text;
	_element->create(uibox);
	uibox->element_list.push_back(std::move(_element));
}

void uibox_add_element_textinput(UIBOX_INFO* uibox, uint16_t x, uint16_t y, std::string text)
{
	std::shared_ptr<UIBOX_ELEMENT_TEXTINPUT> _element = std::make_shared<UIBOX_ELEMENT_TEXTINPUT>();
	_element->x = x;
	_element->y = y;
	_element->text = text;
	_element->create(uibox);
	uibox->element_list.push_back(std::move(_element));
}

void uibox_add_element_numinput(UIBOX_INFO* uibox, uint16_t x, uint16_t y, std::string text)
{
	std::shared_ptr<UIBOX_ELEMENT_NUMINPUT> _element = std::make_shared<UIBOX_ELEMENT_NUMINPUT>();
	_element->x = x;
	_element->y = y;
	_element->text = text;
	_element->create(uibox);
	uibox->element_list.push_back(std::move(_element));
}

void uibox_update_element(int16_t uibox_in, int16_t element_in)
{
	UIBOX_INFO* uibox = UIBOXES[uibox_in].get();

	uibox->element_list[element_in]->set();
	//uibox->element_update = true;
	//uibox->creation_update = true; // the whole window updates every time you click
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