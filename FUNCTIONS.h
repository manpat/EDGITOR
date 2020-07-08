#pragma once

  //
 //   FUNCTIONS   ///////////////////////////////////////////////// ///////  //////   /////    ///     //      /
//

inline float reach_tween(float a, float b, float t)
{
	return (a != b) ? (a + ((1 / t) * (b - a))) : (a);
}

inline float clampf(float x, float lo, float hi)
{
	return (x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x);
}

inline int16_t clamp(int16_t x, int16_t lo, int16_t hi)
{
	return (x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x);
}

int16_t sign(int16_t x) {
	return (x > 0) - (x < 0);
}

void HSVtoRGB(int16_t H, double S, double V, int16_t output[3]) {
	double C = S * V;
	double X = C * (1 - abs(fmod(H / 60.0, 2) - 1));
	double m = V - C;
	double Rs, Gs, Bs;

	if (H >= 0 && H < 60) {
		Rs = C;
		Gs = X;
		Bs = 0;
	}
	else if (H >= 60 && H < 120) {
		Rs = X;
		Gs = C;
		Bs = 0;
	}
	else if (H >= 120 && H < 180) {
		Rs = 0;
		Gs = C;
		Bs = X;
	}
	else if (H >= 180 && H < 240) {
		Rs = 0;
		Gs = X;
		Bs = C;
	}
	else if (H >= 240 && H < 300) {
		Rs = X;
		Gs = 0;
		Bs = C;
	}
	else {
		Rs = C;
		Gs = 0;
		Bs = X;
	}

	output[0] = (Rs + m) * 255;
	output[1] = (Gs + m) * 255;
	output[2] = (Bs + m) * 255;
}

void lab2rgb(float l_s, float a_s, float b_s, float& R, float& G, float& B)
{
	float var_Y = (l_s + 16.) / 116.;
	float var_X = a_s / 500. + var_Y;
	float var_Z = var_Y - b_s / 200.;

	if (pow(var_Y, 3) > 0.008856) var_Y = pow(var_Y, 3);
	else                      var_Y = (var_Y - 16. / 116.) / 7.787;
	if (pow(var_X, 3) > 0.008856) var_X = pow(var_X, 3);
	else                      var_X = (var_X - 16. / 116.) / 7.787;
	if (pow(var_Z, 3) > 0.008856) var_Z = pow(var_Z, 3);
	else                      var_Z = (var_Z - 16. / 116.) / 7.787;

	float X = 95.047 * var_X;    //ref_X =  95.047     Observer= 2°, Illuminant= D65
	float Y = 100.000 * var_Y;   //ref_Y = 100.000
	float Z = 108.883 * var_Z;    //ref_Z = 108.883


	var_X = X / 100.;       //X from 0 to  95.047      (Observer = 2°, Illuminant = D65)
	var_Y = Y / 100.;       //Y from 0 to 100.000
	var_Z = Z / 100.;      //Z from 0 to 108.883

	float var_R = var_X * 3.2406 + var_Y * -1.5372 + var_Z * -0.4986;
	float var_G = var_X * -0.9689 + var_Y * 1.8758 + var_Z * 0.0415;
	float var_B = var_X * 0.0557 + var_Y * -0.2040 + var_Z * 1.0570;

	if (var_R > 0.0031308) var_R = 1.055 * pow(var_R, (1 / 2.4)) - 0.055;
	else                     var_R = 12.92 * var_R;
	if (var_G > 0.0031308) var_G = 1.055 * pow(var_G, (1 / 2.4)) - 0.055;
	else                     var_G = 12.92 * var_G;
	if (var_B > 0.0031308) var_B = 1.055 * pow(var_B, (1 / 2.4)) - 0.055;
	else                     var_B = 12.92 * var_B;

	R = clamp(var_R * 255., 0.0f, 255.0f);
	G = clamp(var_G * 255., 0.0f, 255.0f);
	B = clamp(var_B * 255., 0.0f, 255.0f);
}


inline uint32_t blend_colors(uint32_t src_color, uint32_t dst_color) {
	const float src_cola = (src_color & 0x000000ff) / 255.0f;
	const float dest_cola = (dst_color & 0x000000ff) / 255.0f * (1. - src_cola);
	const float new_cola = (src_cola + dest_cola);
	const float tdest_cola = (dest_cola / 255.0f);
	const float tsrc_cola = (src_cola / 255.0f);
	return (uint32_t)(
		((uint8_t)((((((src_color & 0xff000000) >> 24) * tsrc_cola) + (((dst_color & 0xff000000) >> 24) * tdest_cola))) / new_cola * 255) << 24) |
		((uint8_t)((((((src_color & 0x00ff0000) >> 16) * tsrc_cola) + (((dst_color & 0x00ff0000) >> 16) * tdest_cola))) / new_cola * 255) << 16) |
		((uint8_t)((((((src_color & 0x0000ff00) >> 8) * tsrc_cola) + (((dst_color & 0x0000ff00) >> 8) * tdest_cola))) / new_cola * 255) << 8) |
		(uint8_t)(new_cola * 255.0f));
}

  //
 //   CANVAS FUNCTIONS   ///////////////////////////////////////////////// ///////  //////   /////    ///     //      /
//

inline void layer_new(SDL_Renderer* _renderer, int16_t _x, int16_t _y, int16_t _a, SDL_BlendMode _b)
{
	LAYER_INFO new_layer;
	new_layer.texture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, CANVAS_W, CANVAS_H);

	new_layer.pixels = std::make_unique<uint32_t[]>(CANVAS_W * CANVAS_H);
	new_layer.x = _x;
	new_layer.y = _y;
	new_layer.alpha = _a;
	new_layer.blendmode = _b;
	LAYERS.push_back(std::move(new_layer));
	CANVAS_UPDATE = 1;
	CURRENT_LAYER = 0;
	CURRENT_LAYER_PTR = LAYERS[CURRENT_LAYER].pixels.get();
}

inline bool in_canvas(const uint16_t x, const uint16_t y)
{
	return (x < CANVAS_W && y < CANVAS_H);
}

inline bool out_canvas(const uint16_t x, const uint16_t y)
{
	return (x >= CANVAS_W || y >= CANVAS_H);
}

inline void set_pixel(const int16_t x, const int16_t y, const uint32_t c)
{
	if (out_canvas(x, y)) return;

	BRUSH_PIXELS[y * CANVAS_W + x] = c;
	BRUSH_UPDATE_X1 = std::min(BRUSH_UPDATE_X1, int16_t(x - 1));
	BRUSH_UPDATE_Y1 = std::min(BRUSH_UPDATE_Y1, int16_t(y - 1));
	BRUSH_UPDATE_X2 = std::max(BRUSH_UPDATE_X2, int16_t(x + 1));
	BRUSH_UPDATE_Y2 = std::max(BRUSH_UPDATE_Y2, int16_t(y + 1));
}

inline void set_pixel_layer(const int16_t x, const int16_t y, const uint32_t c, uint16_t l)
{
	if (out_canvas(x, y)) return;

	LAYERS[l].pixels[y * CANVAS_W + x] = c;
	BRUSH_UPDATE_X1 = std::min(BRUSH_UPDATE_X1, int16_t(x - 1));
	BRUSH_UPDATE_Y1 = std::min(BRUSH_UPDATE_Y1, int16_t(y - 1));
	BRUSH_UPDATE_X2 = std::max(BRUSH_UPDATE_X2, int16_t(x + 1));
	BRUSH_UPDATE_Y2 = std::max(BRUSH_UPDATE_Y2, int16_t(y + 1));
}

void set_pixel_line(int16_t x0, int16_t y0, const int16_t x1, const int16_t y1, uint32_t c)
{
	int16_t dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int16_t dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int16_t err = dx + dy, e2;
	for (;;) {
		set_pixel(x0, y0, c); // set_pixel_brush
		if (x0 == x1 && y0 == y1) break;
		e2 = err << 1;
		if (e2 >= dy) { err += dy; x0 += sx; }
		if (e2 <= dx) { err += dx; y0 += sy; }
	}
	BRUSH_UPDATE = 1;
	LAYER_UPDATE = 2;
}

inline bool floodfill_check(const uint16_t x, const uint16_t y, const uint32_t col)
{
	return ((CURRENT_LAYER_PTR[y * CANVAS_W + x] != col) || (BRUSH_PIXELS[y * CANVAS_W + x] != col));
}

inline bool floodfill_check_not(const uint16_t x, const uint16_t y, const uint32_t col)
{
	return ((CURRENT_LAYER_PTR[y * CANVAS_W + x] == col) && (BRUSH_PIXELS[y * CANVAS_W + x] == col));
}

inline void floodfill_core(uint16_t x, uint16_t y, const uint16_t width, const uint16_t height, const uint32_t col_old, const uint32_t col_new);

inline void floodfill(uint16_t x, uint16_t y, const uint16_t width, const uint16_t height, const uint32_t col_old, const uint32_t col_new)
{
	while (true)
	{
		int ox = x, oy = y;
		while (y != 0 && floodfill_check_not(x, y - 1, col_old)) y--;
		while (x != 0 && floodfill_check_not(x - 1, y, col_old)) x--;
		if (x == ox && y == oy) break;
	}
	floodfill_core(x, y, width, height, col_old, col_new);
	BRUSH_UPDATE = 1;
	LAYER_UPDATE = 2;
}

inline void floodfill_core(uint16_t x, uint16_t y, const uint16_t width, const uint16_t height, const uint32_t col_old, const uint32_t col_new)
{
	int lastRowLength = 0;
	do
	{
		int rowLength = 0, sx = x;
		if (lastRowLength != 0 && floodfill_check(x, y, col_old))
		{
			do
			{
				if (--lastRowLength == 0) return;
				++x;
			} while (floodfill_check(x, y, col_old));
			sx = x;
		}
		else
		{
			for (; x != 0 && floodfill_check_not(x - 1, y, col_old); rowLength++, lastRowLength++)
			{
				set_pixel(--x, y, col_new);
				if (y != 0 && floodfill_check_not(x, y - 1, col_old)) floodfill(x, y - 1, width, height, col_old, col_new);
			}
		}

		for (; sx < width && floodfill_check_not(sx, y, col_old); rowLength++, sx++) set_pixel(sx, y, col_new);
		if (rowLength < lastRowLength)
		{
			for (int end = x + lastRowLength; ++sx < end; )
			{
				if (floodfill_check_not(sx, y, col_old)) floodfill_core(sx, y, width, height, col_old, col_new);
			}
		}
		else if (rowLength > lastRowLength && y != 0)
		{
			for (int ux = x + lastRowLength; ++ux < sx; )
			{
				if (floodfill_check_not(ux, y - 1, col_old)) floodfill(ux, y - 1, width, height, col_old, col_new);
			}
		}
		lastRowLength = rowLength;
	} while (lastRowLength != 0 && ++y < height);
}

void function_undo(int n)
{
	const bool _is_undo = (n >= 0);
	UNDO_POS = (clamp(UNDO_POS + n, 0, UNDO_LIST.size() - 1));

	const int _tpos = (UNDO_LIST.size() - (UNDO_POS + (!_is_undo)));
	const auto undo_entry = UNDO_LIST[_tpos].get();
	const int start_x = (undo_entry->x);
	const int start_y = (undo_entry->y);
	const uint16_t _l = (undo_entry->layer);
	const std::vector<uint32_t>& _p = (_is_undo ? undo_entry->undo_pixels : undo_entry->redo_pixels);

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
	UNDO_UPDATE_RECT = { BRUSH_UPDATE_X1, BRUSH_UPDATE_Y1, (BRUSH_UPDATE_X2 - BRUSH_UPDATE_X1), (BRUSH_UPDATE_Y2 - BRUSH_UPDATE_Y1) };
	CURRENT_LAYER = _l;
	CANVAS_UPDATE = true;
}

  //
 //   SYSTEM FUNCTIONS   ///////////////////////////////////////////////// ///////  //////   /////    ///     //      /
//

inline void INIT_SDL()
{
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
	SDL_SetHint(SDL_HINT_WINDOWS_DISABLE_THREAD_NAMING, "1");
	SDL_SetHint(SDL_HINT_FRAMEBUFFER_ACCELERATION, "opengl");
	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");

	SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO);
}

inline SDL_Window* INIT_WINDOW()
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

inline SDL_Renderer* INIT_RENDERER(SDL_Window* WINDOW)
{
	auto RENDERER = SDL_CreateRenderer(WINDOW, -1, SDL_RENDERER_ACCELERATED);

	// BRUSH SURFACE
	BRUSH_PIXELS = new uint32_t[CANVAS_W * CANVAS_H] {};
	BRUSH_TEXTURE = SDL_CreateTexture(RENDERER, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, CANVAS_W, CANVAS_H);
	//SDL_UpdateTexture(CANVAS_TEXTURE, nullptr, &CANVAS_PIXELS[0], (sizeof(uint32_t) * CANVAS_W));
	
	// DEFAULT LAYER
	layer_new(RENDERER, 0, 0, 255, SDL_BLENDMODE_BLEND);
	
	// SET DEFAULT CANVAS UNDO
	auto _u1 = std::make_shared<UNDO_DATA>(0, 0);
	_u1->x = 0;
	_u1->y = 0;
	_u1->w = 0;
	_u1->h = 0;
	UNDO_LIST.push_back(_u1);

	// LOAD FONTS
	font = FC_CreateFont();
	font_under = FC_CreateFont();
	font_bold = FC_CreateFont();
	FC_LoadFont(font, RENDERER, "IBMPlexMono-Regular.ttf", 16, FC_MakeColor(192, 192, 192, 255), TTF_STYLE_NORMAL);
	FC_LoadFont(font_under, RENDERER, "IBMPlexMono-Regular.ttf", 16, FC_MakeColor(192, 192, 192, 255), TTF_STYLE_UNDERLINE);
	FC_LoadFont(font_bold, RENDERER, "IBMPlexMono-Bold.ttf", 16, FC_MakeColor(255, 0, 64, 255), TTF_STYLE_BOLD);

	// BACKGROUND GRID TEXTURE
	BG_GRID_W = ((int16_t)ceil((double)CANVAS_W / (double)CELL_W));
	BG_GRID_H = ((int16_t)ceil((double)CANVAS_H / (double)CELL_H));
	auto BG_GRID_PIXELS = std::make_unique<uint32_t[]>(BG_GRID_W * BG_GRID_H);
	BG_GRID_TEXTURE = SDL_CreateTexture(RENDERER, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, BG_GRID_W, BG_GRID_H);
	for (int i = 0; i < BG_GRID_H; i++)
	{
		for (int j = 0; j < BG_GRID_W; j++)
		{
			BG_GRID_PIXELS[i * BG_GRID_W + j] = (((j + (i % 2)) % 2) ? (0x1a1a1aff) : (0x212121ff));// (((int)(i + floor((double)i / (double)bg_grid_w)) % 2) ? (0x1a1a1aff) : (0x212121ff));
		}
	}
	SDL_SetTextureBlendMode(BG_GRID_TEXTURE, SDL_BLENDMODE_NONE);
	SDL_UpdateTexture(BG_GRID_TEXTURE, nullptr, BG_GRID_PIXELS.get(), BG_GRID_W * sizeof(uint32_t));

	// UI
	// HUEBAR
	UI_PIXELS_HUEBAR = new uint32_t[16 * 360];
	UI_TEXTURE_HUEBAR = SDL_CreateTexture(RENDERER, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 16, 360);
	uint32_t _tcol = 0x00000000;
	int16_t H;
	uint8_t Rs, Gs, Bs;
	for (int16_t i = 0; i < 360; i++)
	{
		H = i;// (int16_t)floor(((double)i / 256.0) * 360.0);
		uint8_t X = (uint8_t)floor((1 - abs(fmod(H / 60.0, 2) - 1)) * 255);

		if (H >= 0 && H < 60) {
			Rs = 0xff;
			Gs = X;
			Bs = 0;
		}
		else if (H >= 60 && H < 120) {
			Rs = X;
			Gs = 0xff;
			Bs = 0;
		}
		else if (H >= 120 && H < 180) {
			Rs = 0;
			Gs = 0xff;
			Bs = X;
		}
		else if (H >= 180 && H < 240) {
			Rs = 0;
			Gs = X;
			Bs = 0xff;
		}
		else if (H >= 240 && H < 300) {
			Rs = X;
			Gs = 0;
			Bs = 0xff;
		}
		else {
			Rs = 0xff;
			Gs = 0;
			Bs = X;
		}

		float _tr, _tg, _tb;
		lab2rgb(100.0f - (float)(((float)i / 360.0f) * 100.0f), 74, -104, _tr, _tg, _tb);

		Rs = (uint8_t)floor(_tr);
		Gs = (uint8_t)floor(_tg);
		Bs = (uint8_t)floor(_tb);

		_tcol = (uint32_t)((Rs<<24) | (Gs << 16) | (Bs << 8) | 0xff);
		for (int16_t j = 0; j < 16; j++)
		{
			UI_PIXELS_HUEBAR[i * 16 + j] = _tcol;
		}
	}
	SDL_UpdateTexture(UI_TEXTURE_HUEBAR, nullptr, &UI_PIXELS_HUEBAR[0], (sizeof(uint32_t) * 16));
	return RENDERER;
}

inline void EVENT_LOOP() {
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

	SDL_Event EVENT {};

	while (SDL_PollEvent(&EVENT))
	{
		if ((SDL_QUIT == EVENT.type) || (SDL_KEYDOWN == EVENT.type && SDL_SCANCODE_ESCAPE == EVENT.key.keysym.scancode))
		{
			QUIT = 1;
			break;
		}

		switch (EVENT.type)
		{
		case SDL_MOUSEBUTTONUP:
			if (EVENT.button.button == SDL_BUTTON_LEFT)
			{
				MOUSEBUTTON_LEFT = false;
				MOUSEBUTTON_PRESSED_LEFT = false;
			}

			if (EVENT.button.button == SDL_BUTTON_MIDDLE)
			{
				MOUSEBUTTON_MIDDLE = false;
				MOUSEBUTTON_PRESSED_MIDDLE = false;
			}

			if (EVENT.button.button == SDL_BUTTON_RIGHT)
			{
				MOUSEBUTTON_RIGHT = false;
				MOUSEBUTTON_PRESSED_RIGHT = false;
			}
			break;

		case SDL_MOUSEBUTTONDOWN:
			if (EVENT.button.button == SDL_BUTTON_LEFT)
			{
				MOUSEBUTTON_LEFT = true;
				MOUSEBUTTON_PRESSED_LEFT = true;
			}

			if (EVENT.button.button == SDL_BUTTON_MIDDLE)
			{
				MOUSEBUTTON_MIDDLE = true;
				MOUSEBUTTON_PRESSED_MIDDLE = true;
			}

			if (EVENT.button.button == SDL_BUTTON_RIGHT)
			{
				MOUSEBUTTON_RIGHT = true;
				MOUSEBUTTON_PRESSED_RIGHT = true;
			}
			break;

		case SDL_MOUSEWHEEL:
			float t_CANVAS_ZOOM = CANVAS_ZOOM;
			CANVAS_ZOOM = clamp(CANVAS_ZOOM + ((float)EVENT.wheel.y * (CANVAS_ZOOM * 0.5f) * 0.5f), 1.0f, 50.0f);
			CANVAS_ZOOM = clamp(CANVAS_ZOOM + (float)EVENT.wheel.y, 1.0f, 100.0f);
			CANVAS_ZOOM = floorf(CANVAS_ZOOM);
			if (t_CANVAS_ZOOM != CANVAS_ZOOM)
			{
				float _mx = (((float)MOUSE_X - (float)CANVAS_X) / (float)t_CANVAS_ZOOM), _my = (((float)MOUSE_Y - (float)CANVAS_Y) / (float)t_CANVAS_ZOOM);

				float _nmx = (((float)MOUSE_X - (float)CANVAS_X) / (float)CANVAS_ZOOM), _nmy = (((float)MOUSE_Y - (float)CANVAS_Y) / (float)CANVAS_ZOOM);
				CANVAS_X += (float)((_nmx - _mx) * CANVAS_ZOOM);
				CANVAS_Y += (float)((_nmy - _my) * CANVAS_ZOOM);
			}
			break;
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
		set_pixel_line(CANVAS_MOUSE_PREVX, CANVAS_MOUSE_PREVY, CANVAS_MOUSE_X, CANVAS_MOUSE_Y, 0xffffffff);
		//std::cout << "TICK" << std::endl;
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
		//floodfill((uint16_t)CANVAS_MOUSE_X, (uint16_t)CANVAS_MOUSE_Y, CANVAS_W, CANVAS_H, 0x00000000, 0xff0000ff);
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

		function_undo(1);

	}
}
