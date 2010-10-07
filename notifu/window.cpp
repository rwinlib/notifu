#include "stdafx.h"

//#include <windows.h>
#include <windowsx.h>
//#include <ole2.h>
//#include <commctrl.h>
//#include <shlwapi.h>
//#include <shlobj.h>
#include <shellapi.h>

HINSTANCE g_hinst;                     

#define NOTIFU_TRAY_ICON_MESSAGE WM_APP+1
#define NOTIFU_TRAY_ICON_ID 1234;

class Window
{
public:
	HWND GetHWND() { return m_hwnd; }
protected:
	virtual LRESULT HandleMessage(
		UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void PaintContent(PAINTSTRUCT *pps) { }
	virtual LPCTSTR ClassName() = 0;
	virtual BOOL WinRegisterClass(WNDCLASS *pwc)
	{ return RegisterClass(pwc); }
	virtual ~Window() { }

	HWND WinCreateWindow(DWORD dwExStyle, LPCTSTR pszName,
		DWORD dwStyle, int x, int y, int cx, int cy,
		HWND hwndParent, HMENU hmenu)
	{
		Register();
		return CreateWindowEx(dwExStyle, ClassName(), pszName, dwStyle,
			x, y, cx, cy, hwndParent, hmenu, g_hinst, this);
	}
private:
	void Register();
	void OnPaint();
	void OnPrintClient(HDC hdc);
	static LRESULT CALLBACK s_WndProc(HWND hwnd,
		UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:
	HWND m_hwnd;
};

void Window::Register()
{
	WNDCLASS wc;
	wc.style         = 0;
	wc.lpfnWndProc   = Window::s_WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = g_hinst;
	wc.hIcon         = NULL;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = ClassName();

	WinRegisterClass(&wc);
}

LRESULT CALLBACK Window::s_WndProc(
	HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Window *self;
	if (uMsg == WM_NCCREATE) {
		LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
		self = reinterpret_cast<Window *>(lpcs->lpCreateParams);
		self->m_hwnd = hwnd;
		SetWindowLongPtr(hwnd, GWLP_USERDATA,
			reinterpret_cast<LPARAM>(self));
	} else {
		self = reinterpret_cast<Window *>
			(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}
	if (self) {
		return self->HandleMessage(uMsg, wParam, lParam);
	} else {
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

LRESULT Window::HandleMessage(
	UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lres;

	switch (uMsg) {
	case WM_NCDESTROY:
		lres = DefWindowProc(m_hwnd, uMsg, wParam, lParam);
		SetWindowLongPtr(m_hwnd, GWLP_USERDATA, 0);
		delete this;
		return lres;

	case WM_PAINT:
		OnPaint();
		return 0;

	case WM_PRINTCLIENT:
		OnPrintClient(reinterpret_cast<HDC>(wParam));
		return 0;
	}

	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}

void Window::OnPaint()
{
	PAINTSTRUCT ps;
	BeginPaint(m_hwnd, &ps);
	PaintContent(&ps);
	EndPaint(m_hwnd, &ps);
}

void Window::OnPrintClient(HDC hdc)
{
	PAINTSTRUCT ps;
	ps.hdc = hdc;
	GetClientRect(m_hwnd, &ps.rcPaint);
	PaintContent(&ps);
}

class RootWindow : public Window
{
public:
	typedef Window super;

	virtual LPCTSTR ClassName() { return TEXT("Scratch"); }
	static RootWindow *Create();
protected:
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCreate();
private:
	HWND m_hwndChild;
};

LRESULT RootWindow::OnCreate()
{
	return 0;
}

LRESULT RootWindow::HandleMessage(
	UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		return OnCreate();  

	case WM_NCDESTROY:
		// Death of the root window ends the thread
		PostQuitMessage(0);
		break;

	case WM_SIZE:
		if (m_hwndChild) {
			SetWindowPos(m_hwndChild, NULL, 0, 0,
				GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam),
				SWP_NOZORDER | SWP_NOACTIVATE);
		}
		return 0;

	case WM_SETFOCUS:
		if (m_hwndChild) {
			SetFocus(m_hwndChild);
		}
		return 0;
	}

	return super::HandleMessage(uMsg, wParam, lParam);
}

RootWindow *RootWindow::Create()
{
	RootWindow *self = new RootWindow();
	if (self && self->WinCreateWindow(0,
		TEXT("Scratch"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL)) {
			return self;
	}
	delete self;
	return NULL;
}

int PASCAL StartNotifuWindow(HINSTANCE hinst, HINSTANCE, LPSTR, int nShowCmd)
{
	g_hinst = hinst;

	if (SUCCEEDED(CoInitialize(NULL))) {

		RootWindow *prw = RootWindow::Create();
		if (prw) {
			//ShowWindow(prw->GetHWND(), nShowCmd);

            // zero the structure - note: Some Windows funtions
            // require this but I can't be bothered to remember
            // which ones do and which ones don't.

			NOTIFYICONDATA niData; 
			ZeroMemory(&niData,sizeof(NOTIFYICONDATA));


			// get Shell32 version number and set the size of the
			// structure note: the MSDN documentation about this is
			// a little dubious(see bolow) and I'm not at all sure
			// if the code bellow is correct

			ULONGLONG ullVersion = GetDllVersion(_T("Shell32.dll"));

			if(ullVersion >= MAKEDLLVERULL(6,0,0,0))
				niData.cbSize = sizeof(NOTIFYICONDATA);
			else if(ullVersion >= MAKEDLLVERULL(5,0,0,0))
				niData.cbSize = NOTIFYICONDATA_V2_SIZE;
			else niData.cbSize = NOTIFYICONDATA_V1_SIZE;

			// the ID number can be any UINT you choose and will
			// be used to identify your icon in later calls to
			// Shell_NotifyIcon
			niData.uID = NOTIFU_TRAY_ICON_ID;

			// state which structure members are valid
			// here you can also choose the style of tooltip
			// window if any - specifying a balloon window:
			// NIF_INFO is a little more complicated 
			niData.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP;

			// load the icon note: you should destroy the icon
			// after the call to Shell_NotifyIcon
			niData.hIcon =
				(HICON)LoadImage( hInstance,
				MAKEINTRESOURCE(IDI_NOTIFU),
				IMAGE_ICON,
				GetSystemMetrics(SM_CXSMICON),
				GetSystemMetrics(SM_CYSMICON),
				LR_DEFAULTCOLOR);


			// set the window you want to recieve event messages
			niData.hWnd = hWnd;

			// set the message to send
			// note: the message value should be in the
			// range of WM_APP through 0xBFFF
			niData.uCallbackMessage = NOTIFU_TRAY_ICON_MESSAGE;
			Shell_NotifyIcon(NIM_ADD, &niData);

			MSG msg;
			while (GetMessage(&msg, NULL, 0, 0)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			Shell_NotifyIcon(NIM_DELETE, &niData);
		}
		CoUninitialize();
	}
	return 0;
}
