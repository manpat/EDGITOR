#pragma once

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

