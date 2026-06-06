#pragma once
#include <cmath>
#include <algorithm>
#include "Framebuffer.h"

//DDA 直线光栅化
inline void DrawLine(Framebuffer& fb, int x0, int y0,
	int x1, int y1, uint32_t color) {
	int dx = x1 - x0;
	int dy = y1 - y0;
	
	int steps = std::max(std::abs(dx), std::abs(dy));

	if(steps==0) {
		fb.SetPixel(x0, y0, color);
		return;
	}

	float xInc = float(dx) / steps;
	float yInc = float(dy) / steps;

	float x = float(x0);
	float y = float(y0);
	for (int i = 0; i <= steps; i++) {
		fb.SetPixel(int(std::lround(x)), int(std::lround(y)), color);
		//Q lround是干什么的？
		//A lround是C++标准库中的一个函数，用于将一个浮点数四舍五入到最近的整数。它返回一个长整数类型（long int）。在这个代码中，std::lround(x)和std::lround(y)用于将计算得到的浮点坐标x和y四舍五入为整数，以便正确地设置像素位置。这是因为像素位置必须是整数，而DDA算法计算得到的坐标可能是浮点数，因此需要使用lround进行四舍五入处理。
		x += xInc;
		y += yInc;
	}
}