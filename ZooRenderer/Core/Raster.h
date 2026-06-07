#pragma once
#include <cmath>
#include <algorithm>
#include "../Math/Vector.h"
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

//边函数
//计算结果等于2维叉积，即ab x ap = |ab|*|ap|*sinθ,绝对值等于三角形面积两倍
//但注意结果有正负，和顺序相关。可用于计算权重
//规定绕序下面积应小于0，大于0为背面
inline float EdgeFunc(const Vector2& a, const Vector2& b, const Vector2& p) {
	return (b.x - a.x) * (p.y - a.y) - (b.y - a.y) * (p.x - a.x);
}

//把0-1的RGB打包成0xRRGGBB
inline uint32_t ColorToU32(const Vector3& c) {
	auto clamp01 = [](float v) {
		return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v);
	};
	int r = int(clamp01(c.x) * 255.0f + 0.5f);
	int g = int(clamp01(c.y) * 255.0f + 0.5f);
	int b = int(clamp01(c.z) * 255.0f + 0.5f);
	return Framebuffer::MakeColor(r, g, b);
}

//用重心坐标填充一个三角形，在内部插值顶点颜色，并做深度测试
inline void DrawTriangle(Framebuffer& fb,
	const Vector3& v0, const Vector3& v1, const Vector3& v2,
	const Vector3& c0, const Vector3& c1, const Vector3& c2) {
	Vector2 p0{ v0.x,v0.y }, p1{ v1.x,v1.y }, p2{ v2.x,v2.y };

	//1.包围盒
	int minX = (int)std::floor(std::min({ p0.x,p1.x,p2.x }));
	int maxX = (int)std::ceil(std::max({ p0.x,p1.x,p2.x }));
	int minY = (int)std::floor(std::min({ p0.y, p1.y, p2.y }));
	int maxY = (int)std::ceil(std::max({ p0.y, p1.y, p2.y }));
	minX = std::max(minX, 0);
	maxX = std::min(maxX, fb.Width() - 1);
	minY = std::max(minY, 0);              
	maxY = std::min(maxY, fb.Height() - 1);

	//2.计算面积，检查，做好倒数便于后续计算
	float area = EdgeFunc(p0, p1, p2);//用二维叉积计算面积2倍
	if (area == 0.0f) return;
	float invArea = 1.0f / area;//后续要除大三角形面积，算好倒数，乘比除快

	//3.逐像素：找到中心点p,算p在▲abc的重心权重，进而计算颜色
	for (int y = minY; y <= maxY; ++y) {
		for (int x = minX; x <= maxX; ++x) {
			Vector2 p{ x + 0.5f,y + 0.5f };//像素中心
			float w0 = EdgeFunc(p1, p2, p) * invArea;
			float w1 = EdgeFunc(p2, p0, p) * invArea;
			float w2 = EdgeFunc(p0, p1, p) * invArea;
			if (w0 >= 0.0f && w1 >= 0.0f && w2 >= 0.0f) {
				Vector3 color = c0 * w0 + c1 * w1 + c2 * w2;
				float z = v0.z * w0 + v1.z * w1 + v2.z * w2;
				fb.SetPixel(x, y, z,ColorToU32(color));
			}
		}
	}

}
