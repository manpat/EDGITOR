#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <algorithm>
#include <cmath>

  //
 //   FUNCTIONS   ///////////////////////////////////////////////// ///////  //////   /////    ////     ///      //       /
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

inline int16_t sign(int16_t x) {
	return (x > 0) - (x < 0);
}

inline bool point_in_rect(int16_t px, int16_t py, int16_t rx, int16_t ry, int16_t rw, int16_t rh)
{
	return (px >= rx && py >= ry && px < (rx + rw) && py < (ry + rh));
}

inline void HSVtoRGB(int16_t H, double S, double V, int16_t output[3]) {
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

inline void lab2rgb(float l_s, float a_s, float b_s, float& R, float& G, float& B)
{
	float var_Y = (l_s + 16.) / 116.;
	float var_X = a_s / 500. + var_Y;
	float var_Z = var_Y - b_s / 200.;

	if (pow(var_Y, 3) > 0.008856f)	var_Y = pow(var_Y, 3);
	else							var_Y = (var_Y - 16.0f / 116.0f) / 7.787f;
	if (pow(var_X, 3) > 0.008856f)	var_X = pow(var_X, 3);
	else							var_X = (var_X - 16.0f / 116.0f) / 7.787f;
	if (pow(var_Z, 3) > 0.008856f)	var_Z = pow(var_Z, 3);
	else							var_Z = (var_Z - 16.0f / 116.0f) / 7.787f;

	float X = 95.047 * var_X;    //ref_X =  95.047     Observer= 2�, Illuminant= D65
	float Y = 100.000 * var_Y;   //ref_Y = 100.000
	float Z = 108.883 * var_Z;    //ref_Z = 108.883


	var_X = X / 100.;       //X from 0 to  95.047      (Observer = 2�, Illuminant = D65)
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

inline COLOR blend_colors(COLOR src_color, COLOR dst_color) {
	const float src_cola = src_color.a / 255.0f;
	const float dest_cola = dst_color.a / 255.0f * (1. - src_cola);
	const float new_cola = (src_cola + dest_cola);

	return COLOR {
		uint8_t((src_color.r/255.0f * src_cola + dst_color.r/255.0f * dest_cola) / new_cola * 255.0f),
		uint8_t((src_color.g/255.0f * src_cola + dst_color.g/255.0f * dest_cola) / new_cola * 255.0f),
		uint8_t((src_color.b/255.0f * src_cola + dst_color.b/255.0f * dest_cola) / new_cola * 255.0f),
		uint8_t(new_cola * 255.0f),
	};
}

template <typename T>
inline void move_to_end(std::vector<T>& v, size_t index)
{
	auto it = v.begin() + index;
	std::rotate(it, it + 1, v.end());
}

#if 0
<<<<<<< HEAD
=======
  //
 //   CANVAS FUNCTIONS   ///////////////////////////////////////////////// ///////  //////   /////    ////     ///      //       /
//

inline void layer_new(SDL_Renderer* _renderer, int16_t _x, int16_t _y, int16_t _a, SDL_BlendMode _b)
{
	LAYER_INFO new_layer;
	new_layer.texture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, CANVAS_W, CANVAS_H);

	new_layer.pixels = std::make_unique<COLOR[]>(CANVAS_W * CANVAS_H);
	new_layer.x = _x;
	new_layer.y = _y;
	new_layer.alpha = _a;
	new_layer.blendmode = _b;
	LAYERS.push_back(std::move(new_layer));
	CANVAS_UPDATE = 1;
	CURRENT_LAYER = 0;
	CURRENT_LAYER_PTR = LAYERS[CURRENT_LAYER].pixels.get();
}

inline void uibox_setchar(UIBOX_CHAR* ci, UIBOX_INFO* ui, uint16_t char_pos, uint8_t _CHR, COLOR _COL, COLOR _BG_COL, bool update)
{
	ci->chr = _CHR;
	ci->col = _COL;
	ci->bg_col = _BG_COL;
	if (!update) return;
	ui->update_stack.insert(ui->update_stack.begin() + (rand() % (ui->update_stack.size() + 1)), char_pos);
	ui->update = true;
}

inline void uibox_setstring(UIBOX_INFO* uibox, std::string _charlist, uint16_t x, uint16_t y, COLOR col, bool update)
{
	UIBOX_CHAR* _charinfo;
	uint16_t pos;
	const char* _CHARS = _charlist.c_str();
	for (uint16_t j = 0; j < _charlist.size(); j++)
	{
		pos = j + (y * uibox->chr_w + x);
		_charinfo = &uibox->charinfo[pos];
		uibox_setchar(_charinfo, uibox, pos, _CHARS[j], col, _charinfo->bg_col, update);
	}
}

inline void uibox_addinteract(UIBOX_INFO* uibox, std::string text, std::string over_text, uint8_t type, bool* bool_ptr, uint16_t* int_ptr, uint16_t int_var, bool is_pos, uint16_t px, uint16_t py)
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
	if (!is_pos) uibox_setstring(uibox, text, 2, 2 + (uibox->element.size() - 1), COL_WHITE, 0); else uibox_setstring(uibox, text, px, py, COL_WHITE, 0);
}

inline UIBOX_INFO* uibox_new(uint16_t _x, uint16_t _y, uint16_t _w, uint16_t _h, bool can_grab, std::string title)
{
	UIBOX_INFO new_uibox;
	UIBOX_INFO* uibox_ptr = &new_uibox;

	uibox_ptr->title = title;
	uibox_ptr->can_grab = can_grab;
	uibox_ptr->update = true;
	uibox_ptr->x = _x;
	uibox_ptr->y = _y;

	// GET CHARS THAT FIT WINDOW
	uibox_ptr->chr_w = (int)floor(((float)_w / (float)FONT_CHRW) + 0.5f);
	uibox_ptr->chr_h = (int)floor(((float)_h / (float)FONT_CHRH) + 0.5f);

	// MAKE WINDOW THE NEW ROUNDED CHAR SIZE
	uibox_ptr->w = (uibox_ptr->chr_w * FONT_CHRW);
	uibox_ptr->h = (uibox_ptr->chr_h * FONT_CHRH);

	// FILL WINDOW WITH EMPTY CHARS
	for (int j = 0; j < uibox_ptr->chr_w * uibox_ptr->chr_h; j++)
	{
		UIBOX_CHAR _chr;
		COLOR _tcol{ 255, 255, 255, 255 };
		_chr.col = _tcol;
		_chr.chr = ' ';
		uibox_ptr->charinfo.push_back(_chr);
		uibox_ptr->update_stack.insert(uibox_ptr->update_stack.begin() + (rand() % (uibox_ptr->update_stack.size() + 1)), j);
	}

	// ADD BORDER
	for (int j = 0; j < uibox_ptr->chr_h; j++)
	{
		for (int i = 0; i < uibox_ptr->chr_w; i++)
		{
			if ((j > 0 && j < uibox_ptr->chr_h -1) && (i > 0 && i < uibox_ptr->chr_w -1))
			{
				continue;
			}
			uibox_ptr->charinfo[j * uibox_ptr->chr_w + i].chr = (j == 0) ? ((i == 0) ? CHAR_BOXTL : ((i == uibox_ptr->chr_w -1) ? CHAR_BOXTR : CHAR_BOXH)) :
				((j == uibox_ptr->chr_h -1) ? ((i == 0) ? CHAR_BOXBL : ((i == uibox_ptr->chr_w -1) ? CHAR_BOXBR : CHAR_BOXH)) : CHAR_BOXV);
		}
	}

	/*uibox_ptr.charinfo[1].chr = '[';
	uibox_ptr.charinfo[2].chr = ' ';
	uibox_ptr.charinfo[3].chr = 'T';
	uibox_ptr.charinfo[4].chr = 'E';
	uibox_ptr.charinfo[5].chr = 'S';
	uibox_ptr.charinfo[6].chr = 'T';
	uibox_ptr.charinfo[7].chr = ' ';
	uibox_ptr.charinfo[8].chr = ']';*/

	// TITLE
	uibox_setstring(uibox_ptr, STR_NBSP + title + STR_NBSP, 1, 0, COL_WHITE, 0);

	// SHRINK BUTTON
	uibox_setstring(uibox_ptr, STR_NBSP STR_ARWD STR_NBSP, uibox_ptr->chr_w - 4, 0, COL_WHITE, 0);

	uibox_ptr->texture = nullptr;

	UIBOXES.push_back(std::move(*uibox_ptr));

	return &UIBOXES.back();
}

inline bool in_canvas(const uint16_t x, const uint16_t y)
{
	return (x < CANVAS_W && y < CANVAS_H);
}

inline bool out_canvas(const uint16_t x, const uint16_t y)
{
	return (x >= CANVAS_W || y >= CANVAS_H);
}

inline void set_pixel(const int16_t x, const int16_t y, const COLOR c)
{
	if (out_canvas(x, y)) return;

	BRUSH_PIXELS[y * CANVAS_W + x] = c;
	BRUSH_UPDATE_X1 = std::min(BRUSH_UPDATE_X1, int16_t(x - 1));
	BRUSH_UPDATE_Y1 = std::min(BRUSH_UPDATE_Y1, int16_t(y - 1));
	BRUSH_UPDATE_X2 = std::max(BRUSH_UPDATE_X2, int16_t(x + 1));
	BRUSH_UPDATE_Y2 = std::max(BRUSH_UPDATE_Y2, int16_t(y + 1));
}

void set_pixel_brush(int x, int y, COLOR c)
{
	uint8_t _a;
	int16_t _tx, _ty;
	for (int i = 0; i < BRUSH_W; i++)
		for (int j = 0; j < BRUSH_W; j++)
		{
			_tx = ((x + BRUSH_X) + i);
			_ty = ((y + BRUSH_Y) + j);
			//if (!in_canvas(_tx, _ty)) continue;
			_a = (BRUSH_LIST[BRUSH_LIST_POS]->alpha[j * BRUSH_W + i]);
			if (!_a) continue;
			//if (BRUSH_PIXELS[_tx, _ty] != 0x00000000) continue;
			if (_a) set_pixel(_tx, _ty, c);
		}
}

inline void set_pixel_layer(const int16_t x, const int16_t y, const COLOR c, uint16_t l)
{
	if (out_canvas(x, y)) return;

	LAYERS[l].pixels[y * CANVAS_W + x] = c;
	BRUSH_UPDATE_X1 = std::min(BRUSH_UPDATE_X1, int16_t(x - 1));
	BRUSH_UPDATE_Y1 = std::min(BRUSH_UPDATE_Y1, int16_t(y - 1));
	BRUSH_UPDATE_X2 = std::max(BRUSH_UPDATE_X2, int16_t(x + 1));
	BRUSH_UPDATE_Y2 = std::max(BRUSH_UPDATE_Y2, int16_t(y + 1));
}

void set_pixel_line(int16_t x0, int16_t y0, const int16_t x1, const int16_t y1, COLOR c)
{
	int16_t dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int16_t dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int16_t err = dx + dy, e2;
	for (;;) {
		set_pixel_brush(x0, y0, c); // set_pixel_brush
		if (x0 == x1 && y0 == y1) break;
		e2 = err << 1;
		if (e2 >= dy) { err += dy; x0 += sx; }
		if (e2 <= dx) { err += dx; y0 += sy; }
	}
	BRUSH_UPDATE = 1;
	LAYER_UPDATE = 2;
}

inline bool floodfill_check(const uint16_t x, const uint16_t y, const COLOR col)
{
	return ((CURRENT_LAYER_PTR[y * CANVAS_W + x] != col) || (BRUSH_PIXELS[y * CANVAS_W + x] != col));
}

inline bool floodfill_check_not(const uint16_t x, const uint16_t y, const COLOR col)
{
	return ((CURRENT_LAYER_PTR[y * CANVAS_W + x] == col) && (BRUSH_PIXELS[y * CANVAS_W + x] == col));
}

inline void floodfill_core(uint16_t x, uint16_t y, const uint16_t width, const uint16_t height, const COLOR col_old, const COLOR col_new);

inline void floodfill(uint16_t x, uint16_t y, const uint16_t width, const uint16_t height, const COLOR col_old, const COLOR col_new)
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

inline void floodfill_core(uint16_t x, uint16_t y, const uint16_t width, const uint16_t height, const COLOR col_old, const COLOR col_new)
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
	UNDO_UPDATE_RECT = { BRUSH_UPDATE_X1, BRUSH_UPDATE_Y1, (BRUSH_UPDATE_X2 - BRUSH_UPDATE_X1), (BRUSH_UPDATE_Y2 - BRUSH_UPDATE_Y1) };
	CURRENT_LAYER = _l;
	CANVAS_UPDATE = true;
}

inline void brush_new(uint8_t* a, uint16_t w)
{
	std::unique_ptr<BRUSH_DATA> _u(new BRUSH_DATA(w));
	
	for (int _i = 0; _i < 49; _i++)
	{
		_u->set(_i, a[_i]);
	}
	BRUSH_LIST.push_back(std::move(_u));
}

inline void brush_set(uint16_t sel)
{
	uint16_t w = BRUSH_LIST[sel]->w;
	BRUSH_X = -w / 2;
	BRUSH_Y = -w / 2;
	BRUSH_W = w;
}


  //
 //   SYSTEM FUNCTIONS   ///////////////////////////////////////////////// ///////  //////   /////    ////     ///      //       /
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

	for (int i = 0; i < BRUSH_W * BRUSH_W; i++)
	{
		uibox_addinteract(UIBOX_BRUSH, "::", STR_NBSP STR_NBSP, 0, (bool*)&(BRUSH_LIST[BRUSH_LIST_POS]->alpha[i]), nullptr, 0, true, 3 + ((i % BRUSH_W) * 2), 2 + (i / BRUSH_W));
	}

	UIBOX_TOOLS = uibox_new(0, 0, 128, 512, 0, "TOOLS");
	uibox_addinteract(UIBOX_TOOLS, "BRUSH",  "> BRUSH",  0, nullptr, &CURRENT_TOOL, 0, false, 0, 0);
	uibox_addinteract(UIBOX_TOOLS, "ERASER", "> ERASER", 0, nullptr, &CURRENT_TOOL, 1, false, 0, 0);
	uibox_addinteract(UIBOX_TOOLS, "FILL",   "> FILL",   0, nullptr, &CURRENT_TOOL, 2, false, 0, 0);


	SDL_SetCursor(init_system_cursor(arrow));

	return RENDERER;
}

inline SDL_Texture* INIT_FONT(SDL_Renderer* renderer)
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
				else if (keysym.sym == SDLK_BACKSPACE)
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
				else if (keysym.sym == SDLK_b)
				{
					CURRENT_TOOL = TOOL::BRUSH;
				}
				else if (keysym.sym == SDLK_e)
				{
					CURRENT_TOOL = TOOL::ERASER;
				}
				else if (keysym.sym == SDLK_f)
				{
					CURRENT_TOOL = TOOL::FILL;
				}

				int16_t tools = uibox_get_uibox_by_title("TOOLS");
				if (tools != -1) {
					uint16_t last;
					switch (CURRENT_TOOL) {
						case TOOL::BRUSH: last = uibox_get_element_by_text(tools, "BRUSH"); break;
						case TOOL::ERASER: last = uibox_get_element_by_text(tools, "ERASER"); break;
						case TOOL::FILL: last = uibox_get_element_by_text(tools, "FILL"); break;
						default: last = -1;
					}
					if (last != -1) uibox_update_element(tools, last);
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
					uibox_update_element(UIBOX_IN, ELEMENT_IN);
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

				// shrink [↓]
				if (!uibox_click->in_grab && !uibox_click->grab && uibox_click->in_shrink)
				{
					UIBOX_CHAR* _charinfo;
					uibox_click->shrink = !uibox_click->shrink;
					uibox_click->h = (uibox_click->shrink) ? FONT_CHRH : (uibox_click->chr_h * FONT_CHRH);

					int diff_x = (uibox_click->x - (WINDOW_W / 2)), diff_y = (uibox_click->y - (WINDOW_H / 2));

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

>>>>>>> alepcho/master
#endif

#endif // FUNCTIONS_H
