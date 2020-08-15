#pragma once

#include <cstdint>

struct COLOR {
	uint8_t r, g, b, a;

	friend bool operator==(COLOR a, COLOR b) {
		return a.r == b.r && a.g == b.g
			&& a.b == b.b && a.a == b.a;
	}

	friend bool operator!=(COLOR a, COLOR b) {
		return !(a == b);
	}
};


// PRESET COLORS
constexpr COLOR COL_ACCENT	{ 0xff, 0x00, 0x40, 0xc0 };
constexpr COLOR COL_BGUPDATE{ 0x00, 0x00, 0x00, 0x01 };
constexpr COLOR COL_EMPTY	{ 0x00, 0x00, 0x00, 0x00 };

constexpr COLOR COL_WHITE	{ 0xff, 0xff, 0xff, 0xff };
constexpr COLOR COL_LTGRAY	{ 0xc0, 0xc0, 0xc0, 0xff };
constexpr COLOR COL_GRAY	{ 0x80, 0x80, 0x80, 0xff };
constexpr COLOR COL_DKGRAY	{ 0x40, 0x40, 0x40, 0xff };
constexpr COLOR COL_BLACK	{ 0x00, 0x00, 0x00, 0xff };



COLOR blend_colors(COLOR src_color, COLOR dst_color);

