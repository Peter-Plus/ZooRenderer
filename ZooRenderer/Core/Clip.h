#pragma once
#include "../Math/Vector.h"

//裁剪空间中的顶点：位置+待插值的属性（目前只有颜色）
//V4 clip以及V3 color
struct ClipVertex
{
	Vector4 clip;
	Vector3 color;
};

// 把一个三角形对近平面裁剪，判断在近平面方法：w+z>=0
//输入裁剪顶点数组，输入3个裁剪顶点，输出最多4个（0/3/4）
inline int ClipTriangleNear(const ClipVertex in[3], ClipVertex out[4]) {
	auto dist = [](const Vector4& v) {return v.z + v.w; };
	int n = 0;
	for (int i = 0; i < 3; i++) {
		const ClipVertex& cur = in[i];
		const ClipVertex& nxt = in[(i + 1) % 3];
		float dC = dist(cur.clip), dN = dist(nxt.clip);
		bool inC = dC >= 0.0f, inN = dN >= 0.0f;
		
		if (inC) out[n++] = cur;
		if (inC != inN) {
			float t = dC / (dC - dN);
			ClipVertex e;
			e.clip = cur.clip + (nxt.clip - cur.clip) * t;
			e.color = cur.color + (nxt.color - cur.color) * t;
			out[n++] = e;

		}
		
	}
	return n;//0/3/4分别对应全后，全前/两点后和一点后
}

//背面剔除模式
enum class CullMode{None,Back,Front};

//float screenArea，大于0背面,mode CullMode::Back
inline bool ShouldCull(float screenArea, CullMode mode) {
	if (mode == CullMode::None) return false;
	if (mode == CullMode::Back) return screenArea >= 0.0f;//认为Area>=0背面
	return screenArea <= 0.0f;//剔除正面
}