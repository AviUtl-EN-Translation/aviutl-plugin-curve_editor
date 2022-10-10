//----------------------------------------------------------------------------------
//		Curve Editor
//		�\�[�X�t�@�C��(�R���g���[��)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "ce_header.hpp"



//---------------------------------------------------------------------
//		�R���g���[�����쐬
//---------------------------------------------------------------------
BOOL ce::Control::create(HWND hwnd_p, LPTSTR name, LPTSTR desc, LPTSTR ico_res_dark, LPTSTR ico_res_light, int ct_id, LPRECT rect)
{
	WNDCLASSEX tmp;
	id = ct_id;
	icon_res_dark = ico_res_dark;
	icon_res_light = ico_res_light;
	description = desc;

	tmp.cbSize			= sizeof(tmp);
	tmp.style			= CS_HREDRAW | CS_VREDRAW;
	tmp.lpfnWndProc		= wndproc_static;
	tmp.cbClsExtra		= 0;
	tmp.cbWndExtra		= 0;
	tmp.hInstance		= g_fp->dll_hinst;
	tmp.hIcon			= NULL;
	tmp.hIconSm			= NULL;
	tmp.hCursor			= LoadCursor(NULL, IDC_ARROW);
	tmp.hbrBackground	= NULL;
	tmp.lpszMenuName	= NULL;
	tmp.lpszClassName	= name;

	if (RegisterClassEx(&tmp)) {
		hwnd = ::CreateWindowEx(
			NULL,
			name,
			NULL,
			WS_CHILD | WS_VISIBLE,
			rect->left, rect->top,
			rect->right - rect->left,
			rect->bottom - rect->top,
			hwnd_p,
			NULL,
			g_fp->dll_hinst,
			this
		);
		if (hwnd != nullptr)
			return 1;
	}
	return 0;
}



//---------------------------------------------------------------------
//		�R���g���[�����ړ�
//---------------------------------------------------------------------
void ce::Control::move(LPRECT rect)
{
	MoveWindow(
		hwnd,
		rect->left, rect->top,
		rect->right - rect->left,
		rect->bottom - rect->top,
		TRUE
	);
}



//---------------------------------------------------------------------
//		�R���g���[�����ĕ`��
//---------------------------------------------------------------------
void ce::Control::redraw()
{
	::SendMessage(hwnd, WM_COMMAND, CE_CM_REDRAW, 0);
}



//---------------------------------------------------------------------
//		�ÓI�E�B���h�E�v���V�[�W��
//---------------------------------------------------------------------
LRESULT CALLBACK ce::Control::wndproc_static(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	Control* app = (Control*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	if (!app) {//�擾�ł��Ȃ�����(�E�B���h�E������)�ꍇ
		if (msg == WM_CREATE) {
			app = (Control*)((LPCREATESTRUCT)lparam)->lpCreateParams;
			if (app) {
				SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)app);
				return app->wndproc(hwnd, msg, wparam, lparam);
			}
		}
	}
	else {//�擾�ł����ꍇ(�E�B���h�E������)
		return app->wndproc(hwnd, msg, wparam, lparam);
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}



//---------------------------------------------------------------------
//		�E�B���h�E�v���V�[�W��(static�ϐ��g������_���I)
//---------------------------------------------------------------------
LRESULT ce::Control::wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	RECT rect_wnd;

	::GetClientRect(hwnd, &rect_wnd);

	switch (msg) {
	case WM_CREATE:
		canvas.init(hwnd);

		hwnd_parent = ::GetParent(hwnd);

		icon_dark = ::LoadIcon(g_fp->dll_hinst, icon_res_dark);
		icon_light = ::LoadIcon(g_fp->dll_hinst, icon_res_light);

		hwnd_tooltip = ::CreateWindowEx(
			0, TOOLTIPS_CLASS,
			NULL, TTS_ALWAYSTIP,
			CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, CW_USEDEFAULT,
			hwnd, NULL, g_fp->dll_hinst,
			NULL
		);

		tool_info.cbSize = sizeof(TOOLINFO);
		tool_info.uFlags = TTF_SUBCLASS;
		tool_info.hwnd = hwnd;
		tool_info.uId = id;
		tool_info.lpszText = description;
		SendMessage(hwnd_tooltip, TTM_ADDTOOL, 0, (LPARAM)&tool_info);

		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = hwnd;
		return 0;

	case WM_CLOSE:
		canvas.exit();
		return 0;

	case WM_SIZE:
		tool_info.rect = rect_wnd;
		SendMessage(hwnd_tooltip, TTM_NEWTOOLRECT, 0, (LPARAM)&tool_info);
		return 0;

	// �`��
	case WM_PAINT:
	{
		COLORREF bg;
		ID2D1SolidColorBrush* pBrush = NULL;

		if (clicked)
			bg = BRIGHTEN(g_theme[g_config.theme].bg, CE_CT_BR_CLICKED);
		else if (hovered)
			bg = BRIGHTEN(g_theme[g_config.theme].bg, CE_CT_BR_HOVERED);
		else
			bg = g_theme[g_config.theme].bg;
		d2d_setup(&canvas, &rect_wnd, bg);

		::DrawIcon(
			canvas.hdc_memory,
			(int)(rect_wnd.right * 0.5f - CE_ICON_SIZE * 0.5f),
			(int)(rect_wnd.bottom * 0.5f - CE_ICON_SIZE * 0.5f),
			g_config.theme ? icon_light : icon_dark
		);

		canvas.transfer(&rect_wnd);
		return 0;
	}

	// �}�E�X���������Ƃ�
	case WM_MOUSEMOVE:
		::SetCursor(LoadCursor(NULL, IDC_HAND));
		hovered = TRUE;
		::InvalidateRect(hwnd, NULL, FALSE);
		::TrackMouseEvent(&tme);
		return 0;

	// ���N���b�N�����ꂽ�Ƃ�
	case WM_LBUTTONDOWN:
		::SetCursor(LoadCursor(NULL, IDC_HAND));
		clicked = TRUE;
		::InvalidateRect(hwnd, NULL, FALSE);
		::TrackMouseEvent(&tme);
		return 0;

	// ���N���b�N���I������Ƃ�
	case WM_LBUTTONUP:
		::SetCursor(LoadCursor(NULL, IDC_HAND));
		clicked = FALSE;
		::InvalidateRect(hwnd, NULL, FALSE);
		SendMessage(hwnd_parent, WM_COMMAND, id, 0);
		return 0;

	// �}�E�X���E�B���h�E���痣�ꂽ�Ƃ�
	case WM_MOUSELEAVE:
		clicked = FALSE;
		hovered = FALSE;
		::InvalidateRect(hwnd, NULL, FALSE);
		return 0;

	// �R�}���h
	case WM_COMMAND:
		switch (wparam) {
		case CE_CM_REDRAW:
			::InvalidateRect(hwnd, NULL, FALSE);
			return 0;
		}
		return 0;

	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
}