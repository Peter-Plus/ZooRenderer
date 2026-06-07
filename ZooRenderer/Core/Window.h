#pragma once
#define NOMINMAX//防止windows.h里定义的宏和std::min、std::max冲突
#include <windows.h>
#include "Framebuffer.h"

//封装一个win32窗口：负责创建窗口、处理消息、显示Framebuffer
class Window {
public:
	Window(int width, int height, const wchar_t* title);
	~Window();

	//处理本帧累积的所有Windows消息，返回是否收到退出消息
	bool ProcessMessages();

	//把帧缓冲的内容显示到窗口上
	void Present(const Framebuffer& fb);

	const bool* Keys() const { return m_keys; }
	bool IsKeyDown(int vk) const { return vk >= 0 && vk < 256 && m_keys[vk]; }

	//取出上次调用累积的鼠标移动量，并清零
	void ConsumeMouseDelta(float& dx, float& dy) {
		dx = m_mouseDX;
		dy = m_mouseDY;
		m_mouseDX = m_mouseDY = 0;
	}

private:
	//窗口过程函数，处理窗口消息的回调函数，必须是静态成员函数或者全局函数，因为Windows API要求回调函数必须具有特定的签名
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

	void CaptureMouse();
	void ReleaseMouse();
	void ResetCursorToCenter();

	HWND m_hwnd = nullptr;// 窗口句柄
	int m_width, m_height;// 窗口尺寸
	BITMAPINFO m_bmi{};// 用于描述帧缓冲格式的结构体

	bool m_keys[256] = {};// 当前帧所有按键的状态，Q这个数组是怎么更新的呢？在哪里设置按键状态的代码呢？为什么要放在Window类里呢？A 这个数组的状态是在WndProc函数中更新的，当收到WM_KEYDOWN或WM_KEYUP消息时，会根据消息参数更新对应按键的状态。放在Window类里是因为它与窗口的输入处理相关，方便在其他地方访问当前按键状态，例如在Camera类中根据按键状态来控制相机移动。
	//bool m_rmbDown = false;// 右键是否按下，用于控制视角旋转
	bool m_mouseCaptured = false;// 是否进入鼠标控制视角模式
	//bool m_ignoreNextMouseMove = false;// 是否忽略下一次鼠标移动，用于避免在进入鼠标控制视角模式时产生一次大的跳变
	int m_centerX = 0, m_centerY = 0;// 窗口中心位置，用于鼠标控制视角模式下的鼠标位置重置

	//int m_lastX = 0, m_lastY = 0;// 上一帧鼠标位置，用于计算移动距离
	float m_mouseDX = 0, m_mouseDY = 0;// 鼠标累积移动量
	//Q 鼠标的位置总是int，为什么鼠标累积移动量是float呢？
	// A 鼠标的位置是以像素为单位的整数，而鼠标累积移动量是以像素为单位的浮点数。使用float可以更精确地表示鼠标的移动，尤其是在高分辨率显示器上，鼠标移动可能会非常细微，使用float可以避免整数除法带来的精度损失。
	// 此外，鼠标累积移动量通常会乘以一个灵敏度系数，这个系数可能是一个小于1的浮点数，因此使用float可以更方便地进行计算和调整。

};