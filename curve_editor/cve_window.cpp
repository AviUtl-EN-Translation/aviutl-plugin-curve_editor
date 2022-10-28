//----------------------------------------------------------------------------------
//		Curve Editor
//		�\�[�X�t�@�C�� (�E�B���h�E)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"



//---------------------------------------------------------------------
//		�E�B���h�E�쐬
//---------------------------------------------------------------------
BOOL cve::Window::create(HWND hwnd_parent, LPTSTR class_name, WNDPROC wndproc, LONG style, const RECT& rect)
{
	WNDCLASSEX tmp;
	tmp.cbSize = sizeof(tmp);
	tmp.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	tmp.lpfnWndProc = wndproc;
	tmp.cbClsExtra = 0;
	tmp.cbWndExtra = 0;
	tmp.hInstance = g_fp->dll_hinst;
	tmp.hIcon = NULL;
	tmp.hCursor = LoadCursor(NULL, IDC_ARROW);
	tmp.hbrBackground = NULL;
	tmp.lpszMenuName = NULL;
	tmp.lpszClassName = class_name;
	tmp.hIconSm = NULL;

	if (RegisterClassEx(&tmp)) {
		hwnd = CreateWindowEx(
			NULL,
			class_name,
			NULL,
			WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | style,
			rect.left,
			rect.top,
			rect.right - rect.left,
			rect.bottom - rect.top,
			hwnd_parent,
			NULL,
			g_fp->dll_hinst,
			NULL
		);
		if (hwnd != nullptr)
			return TRUE;
	}
	return FALSE;
}



//---------------------------------------------------------------------
//		�E�B���h�E�ړ�
//---------------------------------------------------------------------
void cve::Window::move(const RECT& rect) const
{
	::MoveWindow(
		hwnd,
		SMALLER(rect.left, rect.right),
		SMALLER(rect.top, rect.bottom),
		std::abs(rect.right - rect.left),
		std::abs(rect.bottom - rect.top),
		TRUE
	);
}



//---------------------------------------------------------------------
//		�ĕ`��
//---------------------------------------------------------------------
void cve::Window::redraw() const
{
	::SendMessage(hwnd, WM_COMMAND, CVE_CM_REDRAW, 0);
}



//---------------------------------------------------------------------
//		�\��
//---------------------------------------------------------------------
BOOL cve::Window::show() const
{
	return ::ShowWindow(hwnd, SW_SHOW);
}



//---------------------------------------------------------------------
//		��\��
//---------------------------------------------------------------------
BOOL cve::Window::hide() const
{
	return ::ShowWindow(hwnd, SW_HIDE);
}



//---------------------------------------------------------------------
//		�E�B���h�E�����
//---------------------------------------------------------------------
BOOL cve::Window::close() const
{
	return ::DestroyWindow(hwnd);
}



//---------------------------------------------------------------------
//		�ÓI�E�B���h�E�v���V�[�W��
//---------------------------------------------------------------------
LRESULT CALLBACK cve::Window::wndproc_static(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	Window* app = (Window*)::GetWindowLongPtr(hwnd, GWLP_USERDATA);

	// �擾�ł��Ȃ�����(�E�B���h�E������)�ꍇ
	if (!app) {
		if (msg == WM_CREATE) {
			app = (Window*)((LPCREATESTRUCT)lparam)->lpCreateParams;

			if (app) {
				::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)app);
				return app->wndproc(hwnd, msg, wparam, lparam);
			}
		}
		return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}
	// �擾�ł����ꍇ(�E�B���h�E������)
	else {
		return app->wndproc(hwnd, msg, wparam, lparam);
	}
}



//---------------------------------------------------------------------
//		�E�B���h�E�v���V�[�W��
//---------------------------------------------------------------------
LRESULT cve::Window::wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	return ::DefWindowProc(hwnd, msg, wparam, lparam);
}