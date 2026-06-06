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

private:
	//窗口过程函数，处理窗口消息的回调函数，必须是静态成员函数或者全局函数，因为Windows API要求回调函数必须具有特定的签名
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

	HWND m_hwnd = nullptr;// 窗口句柄
	int m_width, m_height;// 窗口尺寸
	BITMAPINFO m_bmi{};// 用于描述帧缓冲格式的结构体

};