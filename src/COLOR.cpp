#include "COLOR.h"

COLOR blend_colors(COLOR src_color, COLOR dst_color) {
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
