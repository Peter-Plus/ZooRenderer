#include "Window.h"

//显示链接这两个库
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")

//Q上面两句代码是什么意思？
// #pragma comment(lib,"user32.lib")和#pragma comment(lib,"gdi32.lib")是用来告诉链接器在链接阶段需要链接这两个库文件。user32.lib是Windows用户界面相关的库，提供了创建窗口、处理消息等功能；gdi32.lib是Windows图形设备接口相关的库，提供了绘图、显示图像等功能。通过这两行代码，我们确保在编译和链接过程中正确地包含了这些库，以便使用它们提供的功能。
//Q 相当于include系统的文件吗？
//A 不完全相同。#include是用来包含头文件的，它会将头文件的内容直接插入到当前文件中。而#pragma comment(lib, "library")是用来告诉链接器在链接阶段需要链接指定的库文件。虽然两者都是为了使用外部资源，但它们的作用和使用方式不同。#include是编译时的指令，而#pragma comment(lib, "library")是链接时的指令。

//全程用W结尾的宽字符版本，无论项目字符集设成什么都不受影响
static const wchar_t* kClassName = L"ZooRendererWindowClass";

Window::Window(int width, int height, const wchar_t* title)
	:m_width(width), m_height(height) {
	HINSTANCE hInst = GetModuleHandleW(nullptr);// 获取当前模块的实例句柄

	// 1 注册窗口类
	WNDCLASSW wc{};
	wc.lpfnWndProc = WndProc;// 指定窗口过程函数
	wc.hInstance = hInst;// 设置实例句柄
	wc.lpszClassName = kClassName;// 设置窗口类名
	wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);// 加载默认箭头光标，Q这句代码是啥意思
	//A 这句代码是用来加载默认的箭头光标的。LoadCursorW函数用于加载一个光标资源，第一个参数为nullptr表示从系统预定义的光标中加载，第二个参数IDC_ARROW是一个预定义的常量，表示要加载的光标类型，这里是默认的箭头光标。通过设置wc.hCursor，我们指定了窗口在显示时使用这个光标。
	RegisterClassW(&wc);// 注册窗口类

	// 2 创建窗口
	DWORD style = (WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX);// 创建一个不可调整大小、没有最大化按钮的窗口
	RECT rc{ 0,0,width,height };// 定义窗口的客户区大小
	AdjustWindowRect(&rc, style, FALSE);// 调整窗口大小以适应客户区

	m_hwnd = CreateWindowW(kClassName,title,style,
		CW_USEDEFAULT, CW_USEDEFAULT,rc.right-rc.left,rc.bottom - rc.top,
		nullptr, nullptr, hInst, nullptr);// 创建窗口
	ShowWindow(m_hwnd, SW_SHOW);// 显示窗口

	// 3 填好BITMAPINFO，告诉GDI我们要显示的帧缓冲格式
	m_bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);// 设置BITMAPINFOHEADER的大小
	m_bmi.bmiHeader.biWidth = width;// 设置图像宽度
	m_bmi.bmiHeader.biHeight = -height;// 设置图像高度，负值表示顶向下存储
	m_bmi.bmiHeader.biPlanes = 1;// 设置颜色平面数，必须为1
	m_bmi.bmiHeader.biBitCount = 32;// 设置每像素位数，32位表示0xRRGGBB格式
	m_bmi.bmiHeader.biCompression = BI_RGB;// 设置压缩类型，BI_RGB表示不压缩
}

Window::~Window() {
	if (m_hwnd) DestroyWindow(m_hwnd);// 销毁窗口
	UnregisterClassW(kClassName, GetModuleHandleW(nullptr));// 注销窗口类
}

bool Window::ProcessMessages() {
	MSG msg{};
	while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT) return false;// 收到退出消息
		TranslateMessage(&msg);// 翻译消息
		DispatchMessageW(&msg);// 分发消息
	}
	return true;
}

void Window::Present(const Framebuffer& fb) {
	HDC hdc = GetDC(m_hwnd);
	StretchDIBits(hdc,
		0, 0, m_width, m_height,// 目标矩形
		0, 0, m_width, m_height,// 源矩形
		fb.Data(), &m_bmi, DIB_RGB_COLORS, SRCCOPY);
	ReleaseDC(m_hwnd, hdc);
}

LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);// 发送退出消息
		return 0;
	case WM_PAINT: {
		PAINTSTRUCT ps;
		BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
		return 0;
	}
	}
	return DefWindowProcW(hwnd, msg, wp, lp);
}
//Q 这里的WndProc函数是干什么用的？做了什么事情？
//A WndProc函数是窗口过程函数，用于处理窗口消息的回调函数。
// 当窗口接收到消息时，Windows会调用这个函数来处理相应的事件。
// 在这个函数中，我们通过switch语句来判断消息类型，并根据不同的消息执行相应的操作。
// 例如，当收到WM_DESTROY消息时，我们调用PostQuitMessage(0)来发送退出消息；
// 当收到WM_PAINT消息时，我们调用BeginPaint和EndPaint来处理窗口的绘制。
// 对于其他未处理的消息，我们调用DefWindowProcW来进行默认处理。
// 总之，WndProc函数是窗口消息处理的核心，用于响应用户输入、窗口事件等各种消息。
//Q 什么情况下会受到WM_PAINT消息？
//A 当窗口需要重绘时，Windows会发送WM_PAINT消息。这可能发生在以下情况下：
//A 1. 当窗口首次显示时，系统会发送WM_PAINT消息来请求窗口绘制其内容。
//A 2. 当窗口被覆盖后再次暴露时，系统会发送WM_PAINT消息来请求窗口重绘其内容。
//A 3. 当窗口大小发生变化时，系统会发送WM_PAINT消息来请求窗口重新绘制以适应新的大小。
//A 4. 当调用InvalidateRect或InvalidateRgn等函数来标记窗口的某个区域为无效时，系统会发送WM_PAINT消息来请求窗口重绘该区域。
//A 5. 当窗口接收到其他需要重绘的事件时，例如主题更改、系统颜色更改等，系统也会发送WM_PAINT消息来请求窗口重绘其内容。
//Q 这里Present会触发WM_PAINT消息吗？为什么？
//A 在这个实现中，Present函数直接使用StretchDIBits将帧缓冲的内容绘制到窗口上，
// 而不是通过InvalidateRect等函数来标记窗口为无效。
// 因此，Present函数本身不会触发WM_PAINT消息。
// WM_PAINT消息通常是在窗口需要重绘时由系统自动发送的，
// 而在这个实现中，我们直接在Present函数中进行绘制，所以不会触发WM_PAINT消息。