#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <algorithm>
#include <cmath>

  //
 //   FUNCTIONS   ///////////////////////////////////////////////// ///////  //////   /////    ////     ///      //       /
//

inline const float reach_tween(const float a, const float b, const float t)
{
	return (a != b) ? (a + ((1 / t) * (b - a))) : (a);
}

inline const float clampf(const float x, const float lo, const float hi)
{
	return (x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x);
}

inline const int16_t clamp(const int16_t x, const int16_t lo, const int16_t hi)
{
	return (x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x);
}

inline const int16_t sign(const int16_t x) {
	return (x > 0) - (x < 0);
}

inline const bool point_in_rect(const int16_t px, const int16_t py, const int16_t rx, const int16_t ry, const int16_t rw, const int16_t rh)
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

inline const COLOR blend_colors(const COLOR src_color, const COLOR dst_color) {
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


#endif // FUNCTIONS_H
