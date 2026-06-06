#pragma once
#include <vector>
#include <cstdint>
//Q 这行代码是什么意思？#include <cstdint>是干什么的？有什么作用？
//#include <cstdint>是C++标准库中的一个头文件，提供了固定宽度整数类型的定义，例如int32_t、uint32_t等，用于确保整数类型的大小在不同平台上的一致性。

// 我们自己的后台缓冲back buffer
// 颜色统一用0xRRGGBB表示
class Framebuffer {
public:
	Framebuffer(int width,int height)
		:m_width(width), m_height(height), m_pixels(size_t(width)* height, 0) {
	}
	//Q 这里为什么要初始化m_pixels为size_t(width)* height, 0？这是什么东西看不懂
	//A 这里初始化m_pixels为size_t(width)* height, 0是为了创建一个大小
	// 为width*height的像素数据向量，并将所有像素初始化为0（即黑色）。
	// size_t(width)是将width转换为无符号整数类型，以确保在计算像素数量时不会发生整数溢出。
	// 这个初始化确保了帧缓冲有足够的空间来存储每个像素的数据，并且初始状态下所有像素都是黑色。
	//Q size_t(width)* height是声明容器的数量，0是值对吗？但我还是看不懂
	//size_t(width)* height是什么东西，size_t是什么API吗，为什么不直接width*height?
	//A size_t是C++标准库中定义的一个无符号整数类型，通常用于表示对象的大小或容器的大小。这里使用size_t(width)是为了确保在计算像素数量时不会发生整数溢出，因为width和height可能是较大的值。直接使用width*height可能会导致整数溢出，特别是在32位系统上，而使用size_t可以避免这个问题。因此，size_t(width)* height是为了安全地计算像素数量，并且0是用来初始化每个像素的值，表示初始状态下所有像素都是黑色。
	int Width() const { return m_width; }
	int Height() const { return m_height; }
	const uint32_t* Data() const { return m_pixels.data(); }
	//Q 这里的Data()函数是干什么用的？为什么返回const uint32_t*？有什么作用？
	//A Data()函数是用来获取帧缓冲中像素数据的指针。返回const uint32_t*是为了保证调用者不能修改帧缓冲中的像素数据，这样可以保护数据的完整性和安全性。通过这个函数，外部代码可以访问帧缓冲中的像素数据，但不能直接修改它们，从而避免了潜在的错误和不一致性。

	//整块填充API
	void Clear(uint32_t color) {
		for (uint32_t& p : m_pixels) p = color;
	}

	//单像素写入API
	void SetPixel(int x, int y, uint32_t color) {
		if (x < 0 || x >= m_width || y < 0 || y >= m_height) return;// 越界检查
		m_pixels[size_t(y) * m_width + x] = color;
	}

	//把0-255的RGB颜色分量转换成0xRRGGBB格式的颜色值
	static uint32_t MakeColor(int r, int g, int b) {
		return (uint32_t(r) << 16) | (uint32_t(g) << 8) | uint32_t(b);
	}

private:
	int m_width = 0;// 帧缓冲的宽度
	int m_height = 0;// 帧缓冲的高度
	std::vector<uint32_t> m_pixels;// 存储像素数据的向量，每个像素用一个32位无符号整数表示，格式为0xRRGGBB
};

