#include "graphic_mpu6050.h"
#include "mpu6050.h"
#include "oled.h"
#include "math.h"

static float cy, sy, cp, sp, cr, sr;
static int8_t test_x, test_y;

void ypr(void) {
	cy = cosf(0.01745 * yaw),   sy = sinf(0.01745 * yaw);
	cp = cosf(pitch), sp = sinf(pitch);
	cr = cosf(roll),  sr = sinf(roll);
}

void euler(float a, float b, float c) {
	float x_rot = a * (cp * cy) + b * (-cp * sy) + c * (sp);
	float y_rot = a * (sr * sp * cy + cr * sy) +
	              b * (-sr * sp * sy + cr * cy) +
	              c * (-sr * cp);

	const float scale = 23.0f;

	test_x = (int8_t)(x_rot * scale);
	test_y = (int8_t)(-y_rot * scale);
}

void cle(uint8_t x, uint8_t y) {
	OLED_DrawPoint(x - 1, y - 1, 1);
	OLED_DrawPoint(x - 1, y, 1);
	OLED_DrawPoint(x - 1, y + 1, 1);
	OLED_DrawPoint(x, y - 1, 1);
	OLED_DrawPoint(x, y + 1, 1);
	OLED_DrawPoint(x + 1, y - 1, 1);
	OLED_DrawPoint(x + 1, y, 1);
	OLED_DrawPoint(x + 1, y + 1, 1);

	//OLED_DrawPoint(x + 2, y, 1);
	//OLED_DrawPoint(x - 2, y, 1);
	//OLED_DrawPoint(x, y + 2, 1);
	//OLED_DrawPoint(x, y - 2, 1);

	OLED_DrawPoint(x, y, 0);
}

void line_oi(int8_t x, int8_t y) {
	x += 63;
	//y = -y;
	y += 31;
	OLED_DrawLine(126 - x, 62 - y, x, y, 1);
	cle(x, y);
}

void line_o(int8_t x, int8_t y) {
	x += 63;
	y = -y;
	y += 31;
	OLED_DrawLine(63, 31, x, y, 1);
	cle(x, y);
}

void graphic_cube_show(void) {
	ypr();
	euler(0, 0, 1);
	line_oi(test_x, test_y);
	euler(0, 1, 0);
	line_oi(test_x, test_y);
	euler(1, 0, 0);
	line_oi(test_x, test_y);
}

