// manpat: really not a fan of this - this should be cmakes job :(
#ifdef __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

#include "SDL_FontCache.h"
#include "VARIABLES.h"
#include "FUNCTIONS.h"

  //
 //   SYSTEM FUNCTIONS   ///////////////////////////////////////////////// ///////  //////   /////    ////     ///      //       /
//

void INIT_SDL()
{
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
	SDL_SetHint(SDL_HINT_WINDOWS_DISABLE_THREAD_NAMING, "1");
	SDL_SetHint(SDL_HINT_FRAMEBUFFER_ACCELERATION, "opengl");
	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");

	SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO);
}

SDL_Window* INIT_WINDOW()
{
	auto WINDOW = SDL_CreateWindow("EDGITOR", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 2, 2, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
	SDL_SetWindowFullscreen(WINDOW, SDL_WINDOW_FULLSCREEN_DESKTOP);
	SDL_GetWindowSize(WINDOW, &DISPLAY_W, &DISPLAY_H);
	SDL_SetWindowFullscreen(WINDOW, 0);

	SDL_SetWindowSize(WINDOW, (int)((double)DISPLAY_W * .7777777), (int)((double)DISPLAY_H * .7777777)); // make WINDOW 77.77777% of the display
	SDL_GetWindowSize(WINDOW, &WINDOW_W, &WINDOW_H);
	WINDOW_X = ((DISPLAY_W / 2) - (WINDOW_W / 2));
	WINDOW_Y = ((DISPLAY_H / 2) - (WINDOW_H / 2));
	SDL_SetWindowPosition(WINDOW, WINDOW_X, WINDOW_Y);
	return WINDOW;
}

SDL_Renderer* INIT_RENDERER(SDL_Window* WINDOW)
{
	auto RENDERER = SDL_CreateRenderer(WINDOW, -1, SDL_RENDERER_ACCELERATED);

	// BRUSH SURFACE
	BRUSH_PIXELS = std::make_unique<COLOR[]>(CANVAS_W * CANVAS_H);
	BRUSH_TEXTURE = SDL_CreateTexture(RENDERER, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, CANVAS_W, CANVAS_H);
	//SDL_UpdateTexture(CANVAS_TEXTURE, nullptr, &CANVAS_PIXELS[0], (sizeof(uint32_t) * CANVAS_W));
	
	// CENTER CANVAS
	CANVAS_X = (WINDOW_W * 0.5f) - (CANVAS_W * 0.5f);
	CANVAS_Y = (WINDOW_H * 0.5f) - (CANVAS_H * 0.5f);
	CANVAS_X_ANIM = CANVAS_X;
	CANVAS_Y_ANIM = CANVAS_Y;
	CANVAS_W_ANIM = CANVAS_W;
	CANVAS_H_ANIM = CANVAS_H;
	CELL_W_ANIM = CELL_W;
	CELL_H_ANIM = CELL_H;

	// DEFAULT LAYER
	layer_new(RENDERER, 0, 0, 255, SDL_BLENDMODE_BLEND);
	
	// SET DEFAULT CANVAS UNDO
	UNDO_DATA _u1 {0, 0};
	_u1.x = 0;
	_u1.y = 0;
	_u1.w = 0;
	_u1.h = 0;
	UNDO_LIST.push_back(std::move(_u1));

	// LOAD FONTS
	font = FC_CreateFont();
	font_under = FC_CreateFont();
	font_bold = FC_CreateFont();

	std::string font_path		= std::string(RESOURCES_PATH) + "/IBMPlexMono-Regular.ttf";
	std::string font_bold_path	= std::string(RESOURCES_PATH) + "/IBMPlexMono-Bold.ttf";
	std::string FONT_path	= std::string(RESOURCES_PATH) + "/FONT.ttf";
	FC_LoadFont(font, RENDERER, font_path.c_str(), 16, FC_MakeColor(192, 192, 192, 255), TTF_STYLE_NORMAL);
	FC_LoadFont(font_under, RENDERER, font_path.c_str(), 16, FC_MakeColor(192, 192, 192, 255), TTF_STYLE_UNDERLINE);
	FC_LoadFont(font_bold, RENDERER, font_bold_path.c_str(), 16, FC_MakeColor(255, 0, 64, 255), TTF_STYLE_BOLD);

	// TERMINAL FONT
	FONT = TTF_OpenFont(FONT_path.c_str(), 16);
	int _tfw, _tfh;
	TTF_SizeText(FONT, "A", &_tfw, &_tfh);
	FONT_CHRW = (uint16_t)_tfw;
	FONT_CHRH = (uint16_t)_tfh;

	// BACKGROUND GRID TEXTURE
	BG_GRID_W = ((int16_t)ceil((double)CANVAS_W / (double)CELL_W));
	BG_GRID_H = ((int16_t)ceil((double)CANVAS_H / (double)CELL_H));
	auto BG_GRID_PIXELS = std::make_unique<COLOR[]>(BG_GRID_W * BG_GRID_H);
	BG_GRID_TEXTURE = SDL_CreateTexture(RENDERER, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, BG_GRID_W, BG_GRID_H);
	for (int i = 0; i < BG_GRID_H; i++)
	{
		for (int j = 0; j < BG_GRID_W; j++)
		{
			const COLOR cell_colors[] {
				COLOR {0x0c, 0x0c, 0x0c, 0xff},
				COLOR {0x10, 0x10, 0x10, 0xff},
			};

			BG_GRID_PIXELS[i * BG_GRID_W + j] = cell_colors[(i+j) % 2];
		}
	}
	SDL_SetTextureBlendMode(BG_GRID_TEXTURE, SDL_BLENDMODE_NONE);
	SDL_UpdateTexture(BG_GRID_TEXTURE, nullptr, BG_GRID_PIXELS.get(), BG_GRID_W * sizeof(COLOR));

	// BRUSH

	// ADD DEFAULT BRUSH
	const int _t_bs = 13;
	BRUSH_W = _t_bs;
	static uint8_t BRUSH_POINTGRID[_t_bs * _t_bs] = {};
	brush_new(BRUSH_POINTGRID, _t_bs);
	brush_set(BRUSH_LIST_POS);

	BRUSH_CURSOR_PIXELS = new COLOR[_t_bs * _t_bs];
	BRUSH_CURSOR_PIXELS_CLEAR = new COLOR[_t_bs * _t_bs];
	BRUSH_CURSOR_PIXELS_CLEAR_RECT = { 0, 0, 1, 1 };

	for (int i = 0; i < (_t_bs * _t_bs); i++)
	{
		if (BRUSH_POINTGRID[i])
		{
			BRUSH_CURSOR_PIXELS[i] = BRUSH_COLOR;
		}
		else
		{
			BRUSH_CURSOR_PIXELS[i] = COLOR{ 0,0,0,0 };
		}
		BRUSH_CURSOR_PIXELS_CLEAR[i] = COLOR{ 0,0,0,0 };
	}

	// UI
	// HUEBAR
	UI_PIXELS_HUEBAR = new COLOR[16 * 360];
	UI_TEXTURE_HUEBAR = SDL_CreateTexture(RENDERER, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, 16, 360);

	for (int16_t H = 0; H < 360; H++)
	{
		float _tr, _tg, _tb;
		lab2rgb(100.0f - (float)H / 360.0f * 100.0f, 74, -104, _tr, _tg, _tb);

		const uint8_t Rs = (uint8_t)floor(_tr);
		const uint8_t Gs = (uint8_t)floor(_tg);
		const uint8_t Bs = (uint8_t)floor(_tb);

		const COLOR color {Rs, Gs, Bs, 0xff};
		std::fill(&UI_PIXELS_HUEBAR[H * 16], &UI_PIXELS_HUEBAR[(H+1) * 16], color);
	}
	SDL_UpdateTexture(UI_TEXTURE_HUEBAR, nullptr, &UI_PIXELS_HUEBAR[0], sizeof(COLOR) * 16);

	// BOXES
	UIBOX_COLOR = uibox_new(0, 9999, 256, 256, 1, "COLOUR");
	UIBOX_BRUSH = uibox_new(9999, 9999, 256, 256, 1, "BRUSH");

	for (int i = 0; i < BRUSH_W * BRUSH_W; i++) uibox_addinteract(UIBOX_BRUSH, "::", STR_NBSP STR_NBSP, 0, (bool*)&(BRUSH_LIST[BRUSH_LIST_POS]->alpha[i]), nullptr, 0, true, 3 + ((i % BRUSH_W) * 2), 2 + (i / BRUSH_W));

	UIBOX_TOOLS = uibox_new(0, 0, 128, 512, 0, "TOOLS");
	uibox_addinteract(UIBOX_TOOLS, "BRUSH", "> BRUSH", 0, nullptr, &CURRENT_TOOL, 0, false, 0, 0);
	uibox_addinteract(UIBOX_TOOLS, "ERASER", "> ERASER", 0, nullptr, &CURRENT_TOOL, 1, false, 0, 0);
	uibox_addinteract(UIBOX_TOOLS, "FILL", "> FILL", 0, nullptr, &CURRENT_TOOL, 2, false, 0, 0);


	SDL_SetCursor(create_system_cursor());

	return RENDERER;
}

SDL_Texture* INIT_FONT(SDL_Renderer* renderer)
{
	//static const std::vector<const char*> FONT_CHARS;

	/*for (int i = 0; i < 256; i++)
	{
		SDL_Surface* sur = TTF_RenderUTF8_Solid(FONT, CHAR_BOXTL, SDL_Color{ 255, 255, 255, 255 });
	}*/

	const auto char_map =
	" ☺☻♥♦♣♠•◘○◙♂♀♪♫☼"
	"►◄↕‼¶§▬↨↑↓→←∟↔▲▼"
	" !\"#$%&'()*+,-./"
	"0123456789:;<=>?"
	"@ABCDEFGHIJKLMNO"
	"PQRSTUVWXYZ[\\]^_"
	"`abcdefghijklmno"
	"pqrstuvwxyz{|}~ "
	"ÇüéâäàåçêëèïîìÄÅ"
	"ÉæÆôöòûùÿÖÜ¢£¥₧ƒ"
	"áíóúñÑªº¿⌐¬½¼¡«»"
	"░▒▓│┤╡╢╖╕╣║╗╝╜╛┐"
	"└┴┬├─┼╞╟╚╔╩╦╠═╬╧"
	"╨╤╥╙╘╒╓╫╪┘┌█▄▌▐▀"
	"αßΓπΣσµτΦΘΩδ∞φε∩"
	"≡±≥≤⌠⌡÷≈°∙·√ⁿ²■ ";

	SDL_Surface* sur = TTF_RenderUTF8_Solid(FONT, char_map, SDL_Color{ 255, 255, 255, 255 });
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, sur);
	SDL_FreeSurface(sur);

	return texture;
}

void EVENT_LOOP() {
	MOUSEBUTTON_PRESSED_LEFT = false;
	MOUSEBUTTON_PRESSED_MIDDLE = false;
	MOUSEBUTTON_PRESSED_RIGHT = false;

	MOUSE_PREVX = MOUSE_X;
	MOUSE_PREVY = MOUSE_Y;

	int DISPLAY_MOUSE_X {}, DISPLAY_MOUSE_Y {};
	SDL_GetGlobalMouseState(&DISPLAY_MOUSE_X, &DISPLAY_MOUSE_Y);
	MOUSE_X = (DISPLAY_MOUSE_X - WINDOW_X);
	MOUSE_Y = (DISPLAY_MOUSE_Y - WINDOW_Y);

	CANVAS_MOUSE_PREVX = CANVAS_MOUSE_X;
	CANVAS_MOUSE_PREVY = CANVAS_MOUSE_Y;
	double t_CANVAS_MOUSE_X = ((double)(((double)MOUSE_X / (double)CANVAS_ZOOM) - ((double)CANVAS_X / (double)CANVAS_ZOOM)));
	double t_CANVAS_MOUSE_Y = ((double)(((double)MOUSE_Y / (double)CANVAS_ZOOM) - ((double)CANVAS_Y / (double)CANVAS_ZOOM)));

	CANVAS_MOUSE_X = (int16_t)floor(t_CANVAS_MOUSE_X);
	CANVAS_MOUSE_Y = (int16_t)floor(t_CANVAS_MOUSE_Y);

	CANVAS_MOUSE_CELL_X = (int16_t)floor(t_CANVAS_MOUSE_X / (double)CELL_W);
	CANVAS_MOUSE_CELL_Y = (int16_t)floor(t_CANVAS_MOUSE_Y / (double)CELL_H);

	SDL_Event event {};

	while (SDL_PollEvent(&event))
	{
		if ((SDL_QUIT == event.type) || (SDL_KEYDOWN == event.type && SDL_SCANCODE_ESCAPE == event.key.keysym.scancode))
		{
			QUIT = 1;
			break;
		}

		switch (event.type)
		{
		case SDL_TEXTINPUT:
			if (SDL_strlen(event.text.text) == 0 || event.text.text[0] == '\n') break;
			// add input to text
			if (SDL_strlen(KEY_TEXT) + SDL_strlen(event.text.text) < sizeof(KEY_TEXT))
				SDL_strlcat(KEY_TEXT, event.text.text, sizeof(KEY_TEXT));
			KEY_TEXT_INT = atoi(KEY_TEXT);
			break;

		case SDL_MOUSEBUTTONUP:
			if (event.button.button == SDL_BUTTON_LEFT)
			{
				MOUSEBUTTON_LEFT = false;
				MOUSEBUTTON_PRESSED_LEFT = false;
			}

			if (event.button.button == SDL_BUTTON_MIDDLE)
			{
				MOUSEBUTTON_MIDDLE = false;
				MOUSEBUTTON_PRESSED_MIDDLE = false;
			}

			if (event.button.button == SDL_BUTTON_RIGHT)
			{
				MOUSEBUTTON_RIGHT = false;
				MOUSEBUTTON_PRESSED_RIGHT = false;
			}
			break;

		case SDL_MOUSEBUTTONDOWN:
			if (event.button.button == SDL_BUTTON_LEFT)
			{
				MOUSEBUTTON_LEFT = true;
				MOUSEBUTTON_PRESSED_LEFT = true;
			}

			if (event.button.button == SDL_BUTTON_MIDDLE)
			{
				MOUSEBUTTON_MIDDLE = true;
				MOUSEBUTTON_PRESSED_MIDDLE = true;
			}

			if (event.button.button == SDL_BUTTON_RIGHT)
			{
				MOUSEBUTTON_RIGHT = true;
				MOUSEBUTTON_PRESSED_RIGHT = true;
			}
			break;

		case SDL_MOUSEWHEEL: {
			float t_CANVAS_ZOOM = CANVAS_ZOOM;
			CANVAS_ZOOM = clamp(CANVAS_ZOOM + ((float)event.wheel.y * (CANVAS_ZOOM * 0.5f) * 0.5f), 1.0f, 50.0f);
			CANVAS_ZOOM = clamp(CANVAS_ZOOM + (float)event.wheel.y, 1.0f, 100.0f);
			CANVAS_ZOOM = floor(CANVAS_ZOOM);
			if (t_CANVAS_ZOOM != CANVAS_ZOOM)
			{
				float _mx = (((float)MOUSE_X - (float)CANVAS_X) / (float)t_CANVAS_ZOOM), _my = (((float)MOUSE_Y - (float)CANVAS_Y) / (float)t_CANVAS_ZOOM);

				float _nmx = (((float)MOUSE_X - (float)CANVAS_X) / (float)CANVAS_ZOOM), _nmy = (((float)MOUSE_Y - (float)CANVAS_Y) / (float)CANVAS_ZOOM);
				CANVAS_X += (float)((_nmx - _mx) * CANVAS_ZOOM);
				CANVAS_Y += (float)((_nmy - _my) * CANVAS_ZOOM);
			}
			break;
		}

		case SDL_KEYDOWN: {
			const auto keysym = event.key.keysym;
			if (keysym.mod & KMOD_CTRL) {
				switch (keysym.sym) {
				case SDLK_z: {
					if (keysym.mod & KMOD_SHIFT) {
						// because it's the superior 'redo' shortcut :)
						function_undo(-1);
					} else {
						function_undo(1);
					}
					break;
				}
				case SDLK_y: function_undo(-1);  break;
				default: break;
				}
			}
			else
			if (keysym.sym == SDLK_BACKSPACE)
			{
				int textlen = SDL_strlen(KEY_TEXT);
				do {
					if (!textlen)
					{
						break;
					}
					if ((KEY_TEXT[textlen - 1] & 0x80) == 0x00)
					{
						/* One byte */
						KEY_TEXT[textlen - 1] = 0x00;
						break;
					}
					if ((KEY_TEXT[textlen - 1] & 0xC0) == 0x80)
					{
						/* Byte from the multibyte sequence */
						KEY_TEXT[textlen - 1] = 0x00;
						textlen--;
					}
					if ((KEY_TEXT[textlen - 1] & 0xC0) == 0xC0)
					{
						/* First byte of multibyte sequence */
						KEY_TEXT[textlen - 1] = 0x00;
						break;
					}
				} while (1);
				KEY_TEXT_INT = atoi(KEY_TEXT);
			}
			break;
		}
		}
	}

	if (MOUSEBUTTON_MIDDLE)
	{
		if (MOUSEBUTTON_PRESSED_MIDDLE)
		{
			CANVAS_PANX = (float)((float)MOUSE_X - CANVAS_X);
			CANVAS_PANY = (float)((float)MOUSE_Y - CANVAS_Y);
		}
		CANVAS_X = (float)((float)MOUSE_X - CANVAS_PANX);
		CANVAS_Y = (float)((float)MOUSE_Y - CANVAS_PANY);
	}

	if (MOUSEBUTTON_LEFT)
	{
		if (UIBOX_IN >= 0)
		{
			if (MOUSEBUTTON_PRESSED_LEFT)
			{
				if (ELEMENT_IN >= 0)
				{
					UIBOX_INFO* uibox = &UIBOXES[UIBOX_IN];
					UIBOX_ELEMENT* uibox_element = &uibox->element[ELEMENT_IN];
					switch (uibox_element->type)
					{
					case 0:
						if (uibox_element->input_bool != nullptr)
						{
							*uibox_element->input_bool = (!(bool)(*uibox_element->input_bool));
						}
						if (uibox_element->input_int != nullptr)
						{
							*uibox_element->input_int = uibox_element->input_int_var;
						}
						break;
					}
					uibox->element_update = true;
				}

				move_to_end(UIBOXES, UIBOX_IN); // move window to end
				std::rotate(UIBOXES.rbegin(), UIBOXES.rbegin() + 1, UIBOXES.rend()); // then rotate once more to move to start
				// There's probably a better way of doing this
				UIBOX_IN = 0;// UIBOXES.size() - 1;
				// This was originally putting the window at the end of the list,
				// but I made it so it moves it to the start

				UIBOX_INFO* uibox_click = &UIBOXES[UIBOX_IN];

				// grab/pan variables
				UIBOX_CLICKED_IN = UIBOX_IN;
				uibox_click->grab = (uibox_click->in_grab);
				if (uibox_click->can_grab && uibox_click->grab && uibox_click->in_grab)
				{
					UIBOX_PANX = (int16_t)(MOUSE_X - uibox_click->x);
					UIBOX_PANY = (int16_t)(MOUSE_Y - uibox_click->y);
				}

				// shrink
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
			UIBOX_INFO* uibox = &UIBOXES[UIBOX_IN];
			if (uibox->can_grab && uibox->grab && UIBOX_CLICKED_IN == UIBOX_IN)
			{
				// grabbing & moving window
				uibox->x = (MOUSE_X - UIBOX_PANX);
				uibox->y = (MOUSE_Y - UIBOX_PANY);
			}
		}
		
		if (ELEMENT_CLICKED_IN == -1 && UIBOX_CLICKED_IN == -1)
		{
			set_pixel_line(CANVAS_MOUSE_PREVX, CANVAS_MOUSE_PREVY, CANVAS_MOUSE_X, CANVAS_MOUSE_Y, CURRENT_TOOL ? UNDO_COLOR : BRUSH_COLOR);
		}
	}
	else
	{
		UIBOX_CLICKED_IN = -1;
		ELEMENT_CLICKED_IN = -1;
	}

	if (MOUSEBUTTON_PRESSED_RIGHT)
	{
		// NON-CONTIGUOUS / COLOUR REPLACE
		/*uint32_t _tcol;
		for (int i = 0; i < CANVAS_H; i++)
		{
			for (int j = 0; j < CANVAS_W; j++)
			{
				_tcol = CURRENT_LAYER_PTR[i * CANVAS_W + j];
				if (!_tcol) continue;
				//BRUSH_PIXELS[i * CANVAS_W + j] = CURRENT_LAYER_PTR[i * CANVAS_W + j];
				//BRUSH_PIXELS[i * CANVAS_W + j] = 0xff0000ff;
				set_pixel(j, i, 0xff0000ff);
				BRUSH_UPDATE = 1;
				LAYER_UPDATE = 2;
			}
		}*/
		// floodfill((uint16_t)CANVAS_MOUSE_X, (uint16_t)CANVAS_MOUSE_Y, CANVAS_W, CANVAS_H, 0x00000000, 0xff0000ff);
		/*std::thread t1(floodfill, (uint16_t)CANVAS_MOUSE_X-1, (uint16_t)CANVAS_MOUSE_Y, CANVAS_W, CANVAS_H, 0x00000000, 0xff0000ff);
		//std::thread t2(floodfill, (uint16_t)CANVAS_MOUSE_X+1, (uint16_t)CANVAS_MOUSE_Y, CANVAS_W, CANVAS_H, 0x00000000, 0xff0000ff);
		//std::thread t3(floodfill, (uint16_t)CANVAS_MOUSE_X, (uint16_t)CANVAS_MOUSE_Y-1, CANVAS_W, CANVAS_H, 0x00000000, 0xff0000ff);
		//std::thread t4(floodfill, (uint16_t)CANVAS_MOUSE_X, (uint16_t)CANVAS_MOUSE_Y+1, CANVAS_W, CANVAS_H, 0x00000000, 0xff0000ff);
		//t1.join();
		std::cout << t1.joinable() << std::endl;
		std::cout << t1.hardware_concurrency << std::endl;
		t1.join();*/
		//t2.join();
		//t3.join();
		//t4.join();

		//function_undo(1);

	}
}
