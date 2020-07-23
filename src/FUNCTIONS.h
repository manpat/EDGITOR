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

	if (pow(var_Y, 3) > 0.008856) var_Y = pow(var_Y, 3);
	else                      var_Y = (var_Y - 16. / 116.) / 7.787;
	if (pow(var_X, 3) > 0.008856) var_X = pow(var_X, 3);
	else                      var_X = (var_X - 16. / 116.) / 7.787;
	if (pow(var_Z, 3) > 0.008856) var_Z = pow(var_Z, 3);
	else                      var_Z = (var_Z - 16. / 116.) / 7.787;

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

inline void set_pixel_brush(int x, int y, COLOR c)
{
	float _a;
	int _tx, _ty;
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

inline void set_pixel_line(int16_t x0, int16_t y0, const int16_t x1, const int16_t y1, COLOR c)
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

inline void function_undo(int n)
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


#endif // FUNCTIONS_H
