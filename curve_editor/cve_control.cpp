//----------------------------------------------------------------------------------
//		Curve Editor
//		�\�[�X�t�@�C��(�R���g���[��)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"



//---------------------------------------------------------------------
//		�R���g���[�����쐬
//---------------------------------------------------------------------
BOOL cve::Button::initialize(
	HWND			hwnd_p,
	LPCTSTR			name,
	LPCTSTR			desc,
	Content_Type	cont_type,
	LPCTSTR			ico_res_dark,
	LPCTSTR			ico_res_light,
	LPCTSTR			lb,
	int				ct_id,
	const RECT&		rect,
	int				flag
)
{
	id = ct_id;
	description = (LPTSTR)desc;
	edge_flag = flag;
	hwnd_parent = hwnd_p;

	content_type = cont_type;

	if (content_type == Button::String) {
		label = (LPTSTR)lb;
	}
	else if (content_type == Button::Icon) {
		icon_res_dark = (LPTSTR)ico_res_dark;
		icon_res_light = (LPTSTR)ico_res_light;
	}

	return create(hwnd_p, name, wndproc_static, NULL, rect, this);
}



//---------------------------------------------------------------------
//		�R���g���[���`��p�̊֐�
//---------------------------------------------------------------------
void cve::Button::draw_content(COLORREF bg, RECT& rect_wnd, LPCTSTR content)
{
	bitmap_buffer.d2d_setup(TO_BGR(bg));

	::SetBkColor(bitmap_buffer.hdc_memory, bg);

	// �������`��
	if (content_type == Button::String && content != nullptr) {
		::SelectObject(bitmap_buffer.hdc_memory, font);

		::DrawText(
			bitmap_buffer.hdc_memory,
			content,
			strlen(content),
			&rect_wnd,
			DT_CENTER | DT_VCENTER | DT_NOCLIP | DT_SINGLELINE
		);

		::DeleteObject(font);
	}
	// �A�C�R����`��
	else if (content_type == Button::Icon) {
		::DrawIcon(
			bitmap_buffer.hdc_memory,
			(rect_wnd.right - CVE_ICON_SIZE) / 2,
			(rect_wnd.bottom - CVE_ICON_SIZE) / 2,
			g_config.theme ? icon_light : icon_dark
		);
	}

	if (g_render_target != nullptr) {
		g_render_target->BeginDraw();

		bitmap_buffer.draw_rounded_edge(edge_flag, CVE_ROUND_RADIUS);

		g_render_target->EndDraw();
	}

	bitmap_buffer.transfer();
}



//---------------------------------------------------------------------
//		�E�B���h�E�v���V�[�W��(static�ϐ��g�p�s��)
//---------------------------------------------------------------------
LRESULT cve::Button::wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	RECT rect_wnd;

	::GetClientRect(hwnd, &rect_wnd);

	switch (msg) {
	case WM_CREATE:
		bitmap_buffer.init(hwnd);
		bitmap_buffer.set_size(rect_wnd);

		// �A�C�R���������ꍇ
		if (content_type == Button::Icon) {
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
			tool_info.lpszText = (LPTSTR)description;
			::SendMessage(hwnd_tooltip, TTM_ADDTOOL, 0, (LPARAM)&tool_info);
		}

		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = hwnd;

		// �t�H���g
		font = ::CreateFont(
			CVE_CT_FONT_H, 0,
			0, 0,
			FW_REGULAR,
			FALSE, FALSE, FALSE,
			ANSI_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			NULL,
			CVE_FONT_YU_GOTHIC
		);
		return 0;

	case WM_CLOSE:
		bitmap_buffer.exit();
		return 0;

	case WM_SIZE:
		bitmap_buffer.set_size(rect_wnd);

		if (content_type == Button::Icon) {
			tool_info.rect = rect_wnd;
			::SendMessage(hwnd_tooltip, TTM_NEWTOOLRECT, 0, (LPARAM)&tool_info);
		}
		return 0;

	// �`��
	case WM_PAINT:
	{
		COLORREF bg;
		
		if (clicked)
			bg = BRIGHTEN(g_theme[g_config.theme].bg, CVE_CT_BR_CLICKED);
		else if (hovered)
			bg = BRIGHTEN(g_theme[g_config.theme].bg, CVE_CT_BR_HOVERED);
		else
			bg = g_theme[g_config.theme].bg;

		::SetTextColor(bitmap_buffer.hdc_memory, g_theme[g_config.theme].bt_tx);

		draw_content(bg, rect_wnd, label);
		return 0;
	}

	// �}�E�X���������Ƃ�
	case WM_MOUSEMOVE:
		hovered = true;
		::InvalidateRect(hwnd, NULL, FALSE);
		::TrackMouseEvent(&tme);
		return 0;

	case WM_SETCURSOR:
		::SetCursor(::LoadCursor(NULL, IDC_HAND));
		return 0;

	// ���N���b�N�����ꂽ�Ƃ�
	case WM_LBUTTONDOWN:
		::SetCursor(::LoadCursor(NULL, IDC_HAND));
		clicked = true;
		::InvalidateRect(hwnd, NULL, FALSE);
		::TrackMouseEvent(&tme);
		return 0;

	// ���N���b�N���I������Ƃ�
	case WM_LBUTTONUP:
		::SetCursor(::LoadCursor(NULL, IDC_HAND));
		clicked = false;
		::SendMessage(hwnd_parent, WM_COMMAND, id, 0);
		::InvalidateRect(hwnd, NULL, FALSE);
		return 0;

	// �}�E�X���E�B���h�E���痣�ꂽ�Ƃ�
	case WM_MOUSELEAVE:
		clicked = false;
		hovered = false;
		::InvalidateRect(hwnd, NULL, FALSE);
		return 0;

	// �R�}���h
	case WM_COMMAND:
		switch (wparam) {
		case CVE_CM_REDRAW:
			::InvalidateRect(hwnd, NULL, FALSE);
			return 0;
		}
		return 0;

	default:
		return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}
}



//---------------------------------------------------------------------
//		�E�B���h�E�v���V�[�W��(�X�C�b�`)
//---------------------------------------------------------------------
LRESULT cve::Button_Switch::wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	RECT rect_wnd;

	::GetClientRect(hwnd, &rect_wnd);

	switch (msg) {
	// �`��
	case WM_PAINT:
	{
		COLORREF bg;
		// �I����
		if (is_selected) {
			if (clicked)
				bg = BRIGHTEN(g_theme[g_config.theme].bt_selected, CVE_CT_BR_CLICKED);
			else if (hovered)
				bg = BRIGHTEN(g_theme[g_config.theme].bt_selected, CVE_CT_BR_HOVERED);
			else
				bg = g_theme[g_config.theme].bt_selected;

			::SetTextColor(bitmap_buffer.hdc_memory, g_theme[g_config.theme].bt_tx_selected);
		}
		// ��I����
		else {
			if (clicked)
				bg = BRIGHTEN(g_theme[g_config.theme].bt_unselected, CVE_CT_BR_CLICKED);
			else if (hovered)
				bg = BRIGHTEN(g_theme[g_config.theme].bt_unselected, CVE_CT_BR_HOVERED);
			else
				bg = g_theme[g_config.theme].bt_unselected;

			::SetTextColor(bitmap_buffer.hdc_memory, g_theme[g_config.theme].bt_tx);
		}

		draw_content(bg, rect_wnd, label);
		return 0;
	}

	// ���N���b�N���I������Ƃ�
	case WM_LBUTTONUP:
		::SetCursor(LoadCursor(NULL, IDC_HAND));
		clicked = false;
		if (!is_selected) {
			is_selected = true;
			::SendMessage(hwnd_parent, WM_COMMAND, id, CVE_CM_SELECTED);
		}
		::InvalidateRect(hwnd, NULL, FALSE);
		return 0;

	default:
		return Button::wndproc(hwnd, msg, wparam, lparam);
	}
}



//---------------------------------------------------------------------
//		�I����Ԃ�ύX(�X�C�b�`)
//---------------------------------------------------------------------
void cve::Button_Switch::set_status(BOOL bl)
{
	is_selected = bl;
	::InvalidateRect(hwnd, NULL, FALSE);
}



//---------------------------------------------------------------------
//		�E�B���h�E�v���V�[�W��(Value)
//---------------------------------------------------------------------
LRESULT cve::Button_Value::wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	RECT rect_wnd;

	::GetClientRect(hwnd, &rect_wnd);

	switch (msg) {
	case WM_PAINT:
	{
		COLORREF bg;
		std::string str_value_4d = g_curve_value.create_value_4d();
		LPTSTR value_4d = const_cast<LPTSTR>(str_value_4d.c_str());

		if (clicked)
			bg = BRIGHTEN(g_theme[g_config.theme].bg, CVE_CT_BR_CLICKED);
		else if (hovered)
			bg = BRIGHTEN(g_theme[g_config.theme].bg, CVE_CT_BR_HOVERED);
		else
			bg = g_theme[g_config.theme].bg;

		::SetTextColor(bitmap_buffer.hdc_memory, g_theme[g_config.theme].bt_tx);

		draw_content(bg, rect_wnd, value_4d);
		return 0;
	}

	default:
		return Button::wndproc(hwnd, msg, wparam, lparam);
	}
}



//---------------------------------------------------------------------
//		�E�B���h�E�v���V�[�W��(ID)
//---------------------------------------------------------------------
LRESULT cve::Button_ID::wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	RECT rect_wnd;
	POINT pt_client = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };

	::GetClientRect(hwnd, &rect_wnd);

	switch (msg) {
	case WM_PAINT:
	{
		COLORREF bg;
		TCHAR id_text[5];

		::_itoa_s(g_config.current_id, id_text, 5, 10);

		if (clicked)
			bg = BRIGHTEN(g_theme[g_config.theme].bg, CVE_CT_BR_CLICKED);
		else if (hovered)
			bg = BRIGHTEN(g_theme[g_config.theme].bg, CVE_CT_BR_HOVERED);
		else
			bg = g_theme[g_config.theme].bg;

		::SetTextColor(bitmap_buffer.hdc_memory, g_theme[g_config.theme].bt_tx);

		draw_content(bg, rect_wnd, id_text);
		return 0;
	}

	// ���N���b�N�����ꂽ�Ƃ�
	case WM_LBUTTONDOWN:
		pt_lock = pt_client;
		id_buffer = g_config.current_id;

		::SetCursor(LoadCursor(NULL, IDC_HAND));

		if (::GetAsyncKeyState(VK_CONTROL) < 0)
			coef_move = CVE_COEF_MOVE_FAST;
		else
			coef_move = CVE_COEF_MOVE_DEFAULT;

		clicked = true;

		::InvalidateRect(hwnd, NULL, FALSE);
		::SetCapture(hwnd);

		return 0;

	// �J�[�\�����������Ƃ�
	case WM_MOUSEMOVE:
		if (clicked && g_config.mode == Mode_ID) {
			is_scrolling = true;

			::SetCursor(LoadCursor(NULL, IDC_SIZEWE));

			g_config.current_id = MINMAX_LIMIT(id_buffer + (pt_client.x - pt_lock.x) / coef_move, 0, CVE_CURVE_MAX - 1);
			g_curve_id_previous = g_curve_id[g_config.current_id];

			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_REDRAW, 0);
		}
		else ::SetCursor(::LoadCursor(NULL, IDC_HAND));

		hovered = true;

		::InvalidateRect(hwnd, NULL, FALSE);
		::TrackMouseEvent(&tme);

		return 0;

	case WM_LBUTTONUP:
		if (!is_scrolling && clicked && g_config.mode == Mode_ID)
			::DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_ID), hwnd, dialogproc_id);
		
		is_scrolling = false;
		clicked = false;

		::ReleaseCapture();
		::InvalidateRect(hwnd, NULL, FALSE);

		return 0;

	default:
		return Button::wndproc(hwnd, msg, wparam, lparam);
	}
}