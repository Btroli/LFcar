#include "mmgj.h"

void cal_valueP(uint8_t *p, float *v) {
	static const float pow10[10] = {
		1e-9f, 1e-8f, 1e-7f, 1e-6f, 1e-5f, 1e-4f, 1e-3f, 1e-2f, 1e-1f, 1e0f
	};
	*v = 0;
	for (uint8_t i = 0; i < 9; ++i)
		*v = *v * 10 + *(p++);
	*v *= pow10[*p];
}
float cal_value(uint8_t *p) {
	static const float pow10[10] = {
		1e-9f, 1e-8f, 1e-7f, 1e-6f, 1e-5f, 1e-4f, 1e-3f, 1e-2f, 1e-1f, 1e0f
	};
	float v = 0;
	for (uint8_t i = 0; i < 9; ++i)
		v = v * 10 + *(p++);
	v *= pow10[*p];
	return v;
}
void FuXuan(uint8_t n) {
	if (n < 3) {
		OLED_DrawBoxXuLine(0, 8 + 18 * n, 127, 26 + 18 * n, 3, 1);
		OLED_DrawBoxXuLine(2, 10 + 18 * n, 125, 24 + 18 * n, 3, 1);
	} else {
		n -= 3;
		OLED_DrawBoxLine(0, 8 + 18 * n, 127, 26 + 18 * n, 1);
		OLED_DrawBoxLine(2, 10 + 18 * n, 125, 24 + 18 * n, 1);
		OLED_DrawBoxXuLine(0, 8 + 18 * n, 127, 26 + 18 * n, 3, 0);
		OLED_DrawBoxXuLine(2, 10 + 18 * n, 125, 24 + 18 * n, 3, 0);
	}
}

