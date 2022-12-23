//----------------------------------------------------------------------------------
//		Curve Editor
//		�\�[�X�t�@�C��(�G�f�B�b�g�{�b�N�X)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"



//---------------------------------------------------------------------
//		�R���g���[�����쐬
//---------------------------------------------------------------------
BOOL cve::Edit_Box::initialize(
	HWND			hwnd_p,
	LPCTSTR			name,
	int				l_height,
	COLORREF		bg_cl,
	COLORREF		tx_cl,
	int				ct_id,
	const RECT&		rect,
	int				flag
)
{
	id = ct_id;
	edge_flag = flag;
	hwnd_parent = hwnd_p;
	bg_color = bg_cl;
	tx_color = tx_cl;
	line_height = l_height;

	return create(
		hwnd_p,
		name,
		wndproc_static,
		NULL,
		NULL,
		rect,
		this
	);
}



//---------------------------------------------------------------------
//		�E�B���h�E�v���V�[�W��
//---------------------------------------------------------------------
LRESULT cve::Edit_Box::wndproc(HWND hw, UINT msg, WPARAM wparam, LPARAM lparam)
{
	RECT rect_wnd;

	::GetClientRect(hw, &rect_wnd);

	switch (msg) {
	case WM_CREATE:
		bitmap_buffer.init(hw);
		bitmap_buffer.set_size(rect_wnd);

		editbox = ::CreateWindowEx(
			NULL,
			"EDIT",
			NULL,
			WS_CHILD | WS_VISIBLE,
			CVE_MARGIN,
			(rect_wnd.right + line_height) / 2,
			rect_wnd.right - CVE_MARGIN * 2,
			line_height,
			hwnd,
			NULL,
			g_fp->dll_hinst,
			NULL
		);

		return 0;

	case WM_CLOSE:
		bitmap_buffer.exit();
		return 0;

	case WM_SIZE:
		bitmap_buffer.set_size(rect_wnd);
		return 0;

		// �`��
	case WM_PAINT:
	{
		COLORREF bg = g_theme[g_config.theme].bg_editbox;

		bitmap_buffer.d2d_setup(TO_BGR(bg));

		draw_edge();
		bitmap_buffer.transfer();

		return 0;
	}

	// �}�E�X���������Ƃ�
	case WM_MOUSEMOVE:
		if (!disabled) {
			hovered = true;
			::InvalidateRect(hw, NULL, FALSE);
			::TrackMouseEvent(&tme);
		}
		return 0;

	case WM_SETCURSOR:
		::SetCursor(::LoadCursor(NULL, IDC_HAND));
		return 0;

		// ���N���b�N�����ꂽ�Ƃ�
	case WM_LBUTTONDOWN:
		if (!disabled) {
			::SetCursor(::LoadCursor(NULL, IDC_HAND));
			clicked = true;
			::InvalidateRect(hw, NULL, FALSE);
			::TrackMouseEvent(&tme);
		}
		return 0;

		// ���N���b�N���I������Ƃ�
	case WM_LBUTTONUP:
		if (!disabled && clicked) {
			::SetCursor(::LoadCursor(NULL, IDC_HAND));
			clicked = false;
			::SendMessage(hwnd_parent, WM_COMMAND, id, 0);
			::InvalidateRect(hw, NULL, FALSE);
		}
		return 0;

		// �}�E�X���E�B���h�E���痣�ꂽ�Ƃ�
	case WM_MOUSELEAVE:
		clicked = false;
		hovered = false;
		::InvalidateRect(hw, NULL, FALSE);
		return 0;

		// �R�}���h
	case WM_COMMAND:
		switch (wparam) {
		case CVE_CM_REDRAW:
			::InvalidateRect(hw, NULL, FALSE);
			return 0;

		case CVE_CM_CHANGE_LABEL:
		{
			strcpy_s(label, CVE_CT_LABEL_MAX, (LPTSTR)lparam);
			::InvalidateRect(hw, NULL, FALSE);
			return 0;
		}

		case CVE_CM_SET_STATUS:
			set_status(lparam);
			return 0;
		}
		return 0;

	default:
		return ::DefWindowProc(hw, msg, wparam, lparam);
	}
}