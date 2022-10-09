//----------------------------------------------------------------------------------
//		Curve Editor
//		�\�[�X�t�@�C��(�E�B���h�E�v���V�[�W��)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "ce_header.hpp"



//---------------------------------------------------------------------
//		�E�B���h�E�v���V�[�W���i�x�[�X�j
//---------------------------------------------------------------------
BOOL wndproc_base(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, void* editp, FILTER* fp)
{
	static HWND hwnd_main;
	RECT rect_wnd;
	GetClientRect(hwnd, &rect_wnd);

	switch (msg) {
	// �E�B���h�E�쐬��
	case WM_FILTER_INIT:
		SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) | WS_CLIPCHILDREN);
		hwnd_main = create_child(
			hwnd, wndproc_main, "Wnd_Main", WS_CLIPCHILDREN,
			0, 0, rect_wnd.right, rect_wnd.bottom
		);
		g_window.base = hwnd;
		return 0;

	case WM_SIZE:
		MoveWindow(hwnd_main, 0, 0, rect_wnd.right, rect_wnd.bottom, TRUE);
		return 0;

	case WM_GETMINMAXINFO:
		MINMAXINFO* mmi;
		mmi = (MINMAXINFO*)lparam;
		mmi->ptMaxTrackSize.x = CE_MAX_W;
		mmi->ptMaxTrackSize.y = CE_MAX_H;
		return 0;

	case WM_KEYDOWN:
		switch (wparam) {
		case 82: //[R]
			if (g_window.graph) SendMessage(g_window.graph, WM_COMMAND, CE_CM_REVERSE, 0);
			return 0;

		case 67: //[C]
			if (GetAsyncKeyState(VK_CONTROL) < 0)
				SendMessage(g_window.graph, WM_COMMAND, CE_CT_COPY, 0);
			return 0;

		case 83: //[S]
			if (GetAsyncKeyState(VK_CONTROL) < 0)
				SendMessage(g_window.graph, WM_COMMAND, CE_CT_SAVE, 0);
			else
				SendMessage(g_window.graph, WM_COMMAND, CE_CM_SHOWHANDLE, 0);
			return 0;

		case VK_LEFT: //[<]	
			if (g_config.mode) {
				g_config.current_id--;
				g_config.current_id = MINMAXLIM(g_config.current_id, 0, CE_CURVE_MAX - 1);
				g_curve_id_previous = g_curve_id[g_config.current_id];
				if (g_window.graph) SendMessage(g_window.graph, WM_COMMAND, CE_CM_REDRAW, 0);
			}
			return 0;

		case VK_RIGHT: //[>]
			if (g_config.mode) {
				g_config.current_id++;
				g_config.current_id = MINMAXLIM(g_config.current_id, 0, CE_CURVE_MAX - 1);
				g_curve_id_previous = g_curve_id[g_config.current_id];
				if (g_window.graph) SendMessage(g_window.graph, WM_COMMAND, CE_CM_REDRAW, 0);
			}
			return 0;

		case 70: //[F]
			if (g_window.graph) SendMessage(g_window.graph, WM_COMMAND, CE_CT_FIT, 0);
			return 0;

		case 65: //[A]
			if (g_window.graph) SendMessage(g_window.graph, WM_COMMAND, CE_CT_ALIGN, 0);
			return 0;

		case VK_DELETE:
			if (g_window.graph) SendMessage(g_window.graph, WM_COMMAND, CE_CM_DELETE, 0);
			return 0;
		}
		return 0;
	}
	return 0;
}



//---------------------------------------------------------------------
//		�E�B���h�E�v���V�[�W���i���C���j
//---------------------------------------------------------------------
LRESULT CALLBACK wndproc_main(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	static BOOL bSepr = FALSE;
	POINT cl_pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
	RECT rect_wnd, rect_sepr;
	//HANDLE
	static HWND hwnd_editor, hwnd_footer;

	static ce::Bitmap_Canvas canvas;

	GetClientRect(hwnd, &rect_wnd);

	rect_sepr = {
		0,
		g_config.separator - CE_SEPR_W,
		rect_wnd.right,
		g_config.separator + CE_SEPR_W
	};

	switch (msg) {
	case WM_CLOSE:
		canvas.exit();
		return 0;

	case WM_CREATE:
		canvas.init(hwnd);

		//�G�f�B�^�p�l��
		hwnd_editor = create_child(
			hwnd, wndproc_editor, "Wnd_Editor", WS_CLIPCHILDREN,
			0, 0, rect_wnd.right, g_config.separator - CE_SEPR_W
		);

		//�t�b�^�p�l��
		hwnd_footer = create_child(
			hwnd, wndproc_footer, "Wnd_Side", WS_CLIPCHILDREN,
			0, g_config.separator + CE_SEPR_W, rect_wnd.right, rect_wnd.bottom
		);

		g_window.main = hwnd;
		return 0;

	case WM_PAINT:
		draw_main(&canvas, &rect_wnd, &rect_sepr);
		return 0;

	case WM_SIZE:
		MoveWindow(hwnd_editor, 0, 0, rect_wnd.right, g_config.separator - CE_SEPR_W, TRUE);
		MoveWindow(hwnd_footer, 0, g_config.separator + CE_SEPR_W, rect_wnd.right, rect_wnd.bottom, TRUE);
		return 0;

	case WM_LBUTTONDOWN:
		if (PtInRect(&rect_sepr, cl_pt)) {
			bSepr = TRUE;
			SetCursor(LoadCursor(NULL, IDC_SIZENS));
			SetCapture(hwnd);
		}
		return 0;

	case WM_LBUTTONUP:
		bSepr = FALSE;
		ReleaseCapture();
		return 0;

	case WM_MOUSEMOVE:
		if (PtInRect(&rect_sepr, cl_pt)) SetCursor(LoadCursor(NULL, IDC_SIZENS));
		if (bSepr) {
			g_config.separator = MINMAXLIM(cl_pt.y, CE_SEPR_W, rect_wnd.bottom - CE_SEPR_W);
			MoveWindow(hwnd_editor, 0, 0, rect_wnd.right, g_config.separator - CE_SEPR_W, TRUE);
			MoveWindow(hwnd_footer, 0, g_config.separator + CE_SEPR_W, rect_wnd.right, rect_wnd.bottom, TRUE);
			InvalidateRect(hwnd, NULL, FALSE);
		}
		return 0;

	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
}



//---------------------------------------------------------------------
//		�E�B���h�E�v���V�[�W���i�G�f�B�^�p�l���j
//---------------------------------------------------------------------
LRESULT CALLBACK wndproc_editor(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	RECT rect_wnd;
	static HWND hwnd_parent, hwnd_graph;
	static ce::Bitmap_Canvas canvas;


	//�N���C�A���g�̈�̋�`���擾
	GetClientRect(hwnd, &rect_wnd);

	switch (msg) {
	case WM_CLOSE:
		canvas.exit();
		return 0;

		///////////////////////////////////////////
		//		WM_CREATE
		//		(�E�B���h�E���쐬���ꂽ�Ƃ�)
		///////////////////////////////////////////
	case WM_CREATE:
		canvas.init(hwnd);

		//�O���t�p�l��
		hwnd_graph = create_child(
			hwnd, wndproc_graph, "Wnd_Graph", WS_CLIPCHILDREN,
			CE_MARGIN, CE_MARGIN, rect_wnd.right - CE_MARGIN * 2, rect_wnd.bottom - CE_MARGIN * 2
		);
		hwnd_parent = GetParent(hwnd);
		g_window.editor = hwnd;
		return 0;

		///////////////////////////////////////////
		//		WM_SIZE
		//		(�T�C�Y���ύX���ꂽ�Ƃ�)
		///////////////////////////////////////////
	case WM_SIZE:
		MoveWindow(hwnd_graph, CE_MARGIN, CE_MARGIN, rect_wnd.right - CE_MARGIN * 2, rect_wnd.bottom - CE_MARGIN * 2, TRUE);
		return 0;

		///////////////////////////////////////////
		//		WM_PAINT
		//		(�E�B���h�E���`�悳�ꂽ�Ƃ�)
		///////////////////////////////////////////
	case WM_PAINT:
		draw_panel_editor(&canvas, &rect_wnd);
		return 0;

		///////////////////////////////////////////
		//		WM_COMMAND
		//		(�R�}���h���󂯎�����Ƃ�)
		///////////////////////////////////////////
	case WM_COMMAND:
		//switch (LOWORD(wparam)) {
		//	//�ۑ��{�^��
		//case ID_RCLICKMENU_SAVEPRESET:
		//case CT_SAVE:
		//	//�v�Z
		//	if (g_curve_value.ctpt[0].y < -2735 || 3735 < g_curve_value.ctpt[0].y || g_curve_value.ctpt[1].y < -2735 || 3735 < g_curve_value.ctpt[1].y) {
		//		if (g_config.bAlerts) MessageBox(hwnd, g_config.lang ? FLSTR_JA_OUTOFRANGE : FLSTR_OUTOFRANGE, "Flow", MB_OK | MB_ICONINFORMATION);
		//		return 0;
		//	}
		//	DialogBox(g_fp->dll_hinst, g_config.lang ? MAKEINTRESOURCE(IDD_SAVE_JA) : MAKEINTRESOURCE(IDD_SAVE), hwnd, dialogproc_save);
		//	return 0;

		//	//Value�p�l��
		//case CM_EDITOR_VALUE:
		//	if (!g_config.lang) DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_VALUE), hwnd, dialogproc_value);
		//	else DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_VALUE_JA), hwnd, dialogproc_value);
		//	InvalidateRect(hwnd, NULL, FALSE);
		//	RedrawChildren();
		//	return 0;

		//	//�R���g���[���ĕ`��
		//case CE_CM_REDRAW:
		//	InvalidateRect(hwnd, NULL, FALSE);
		//	RedrawChildren();
		//	return 0;
		//}
		return 0;

	default:
		return(DefWindowProc(hwnd, msg, wparam, lparam));
	}
	return 0;
}



//---------------------------------------------------------------------
//		�E�B���h�E�v���V�[�W���i�O���t�p�l���j
//---------------------------------------------------------------------
LRESULT CALLBACK wndproc_graph(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//int
	static int		move_pt			= 0;			//0:None, 1:Ctpt1, 2:Ctpt2
	static BOOL		move_view		= 0;
	static int		move_or_scale	= NULL;
	static int		is_dragging		= FALSE;

	//double
	static double	prev_scale_x, prev_scale_y;
	static float	prev_o_x, prev_o_y;

	//POINT
	static POINT	pt_view;
	POINT			cl_pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
	POINT			gr_pt;

	//RECT
	RECT			rect_wnd;

	//�n���h��
	static HMENU	menu;
	static HWND		hwnd_parent;

	//���̑�
	static ce::Point_Address address;
	static MENUITEMINFO minfo = { 0 };
	static ce::Bitmap_Canvas canvas;


	//�N���C�A���g�̈�̋�`���擾
	GetClientRect(hwnd, &rect_wnd);
	//�O���t���W
	if (g_disp_info.origin.x != NULL)
		gr_pt = to_graph(cl_pt);

	switch (msg) {
	case WM_CLOSE:
		canvas.exit();
		return 0;

		///////////////////////////////////////////
		//		WM_CREATE
		//		(�E�B���h�E���쐬���ꂽ�Ƃ�)
		///////////////////////////////////////////
	case WM_CREATE:
		canvas.init(hwnd);

		// ���j���[
		menu = GetSubMenu(LoadMenu(g_fp->dll_hinst, MAKEINTRESOURCE(IDR_MENU1)), 0);

		minfo.cbSize = sizeof(MENUITEMINFO);

		g_curve_value_previous = g_curve_value;
		g_curve_id_previous = g_curve_id[g_config.current_id];

		g_disp_info.origin.x = CE_GR_PADDING;
		g_disp_info.origin.y = (float)(rect_wnd.bottom - CE_GR_PADDING);
		g_disp_info.scale.x = ((double)rect_wnd.right - (int)(2 * CE_GR_PADDING)) / (double)CE_GR_RESOLUTION;
		g_disp_info.scale.y = ((double)rect_wnd.bottom - (int)(2 * CE_GR_PADDING)) / (double)CE_GR_RESOLUTION;

		hwnd_parent = GetParent(hwnd);
		g_window.graph = hwnd;

		return 0;

		///////////////////////////////////////////
		//		WM_SIZE
		//		(�E�B���h�E�̃T�C�Y���ύX���ꂽ�Ƃ�)
		///////////////////////////////////////////
	case WM_SIZE:

		return 0;

		///////////////////////////////////////////
		//		WM_PAINT
		//		(�E�B���h�E���`�悳�ꂽ�Ƃ�)
		///////////////////////////////////////////
	case WM_PAINT:
		draw_panel_graph(&canvas, &rect_wnd);
		return 0;

		///////////////////////////////////////////
		//		WM_LBUTTONDOWN
		//		(���N���b�N�����ꂽ�Ƃ�)
		///////////////////////////////////////////
	case WM_LBUTTONDOWN:
		// �l���[�h&����_���N���b�N���ꂽ�Ƃ�
		if (g_config.mode == 0 && g_curve_value.point_in_ctpts(cl_pt) >= 1) {
			move_pt = g_curve_value.point_in_ctpts(cl_pt);
			g_curve_value_previous = g_curve_value;
			SetCursor(LoadCursor(NULL, IDC_HAND));
			SetCapture(hwnd);
		}
		// �l���[�h�łȂ��Ƃ�
		else {
			// �J�[�\��������_��ɂ��邩�ǂ���
			address = g_curve_id[g_config.current_id].pt_in_ctpt(cl_pt);
			// �J�[�\��������_��ɂ���Ƃ��C�n���h���̍��W���L��
			if (address.position != 0) {
				g_curve_id[g_config.current_id].move_point(address, gr_pt, TRUE);
				g_curve_id_previous = g_curve_id[g_config.current_id];
				SetCursor(LoadCursor(NULL, IDC_HAND));
				SetCapture(hwnd);
			}
			// �J�[�u��D&D����
			else {
				is_dragging = TRUE;
				SetCapture(hwnd);
			}
		}
		return 0;

		///////////////////////////////////////////
		//		WM_LBUTTONUP
		//		(�}�E�X�̍��{�^���������ꂽ�Ƃ�)
		///////////////////////////////////////////
	case WM_LBUTTONUP:
		// Value���[�h
		if (!g_config.mode) {
			// �߂��ɂ��鐧��_���J�[�\���Ɉړ�
			if (!move_pt) {
				g_curve_value_previous = g_curve_value;
				int d1 = (int)DISTANCE(to_client(g_curve_value.ctpt[0]), cl_pt);
				int d2 = (int)DISTANCE(to_client(g_curve_value.ctpt[1]), cl_pt);
				if (d1 < d2) {
					g_curve_value.ctpt[0].x = to_graph(cl_pt).x;
					g_curve_value.ctpt[0].y = to_graph(cl_pt).y;
				}
				else if (d1 >= d2) {
					g_curve_value.ctpt[1].x = to_graph(cl_pt).x;
					g_curve_value.ctpt[1].y = to_graph(cl_pt).y;
				}
				InvalidateRect(hwnd, NULL, FALSE);
				// �I�[�g�R�s�[
				if (g_config.auto_copy) SendMessage(hwnd, WM_COMMAND, CE_CT_COPY, 0);
			}
			// �ړ����[�h����
			if (move_pt) {
				move_pt = 0;
				if (g_config.auto_copy) SendMessage(hwnd, WM_COMMAND, CE_CT_COPY, 0);
			}
		}
		// ID���[�h�E�J�[�u��D&D����
		else {
			address.position = ce::CTPT_NULL;
			is_dragging = FALSE;
		}
		ReleaseCapture();
		return 0;

		///////////////////////////////////////////
		//		WM_MBUTTONDOWN
		//		(�}�E�X�̍��{�^�����_�u���N���b�N���ꂽ�Ƃ�)
		///////////////////////////////////////////
	case WM_LBUTTONDBLCLK:
		// ID���[�h�̂Ƃ�
		if (g_config.mode == 1) {
			//�J�[�\��������_��ɂ��邩�ǂ���
			address = g_curve_id[g_config.current_id].pt_in_ctpt(cl_pt);

			if (address.position == ce::CTPT_CENTER)
				g_curve_id[g_config.current_id].delete_point(cl_pt);
			else
				g_curve_id[g_config.current_id].add_point(gr_pt);

			InvalidateRect(hwnd, NULL, FALSE);
		}
		return 0;

		///////////////////////////////////////////
		//		WM_RBUTTONUP
		//		(�}�E�X�̉E�{�^���������ꂽ�Ƃ�)
		///////////////////////////////////////////
		//�E�N���b�N���j���[
	case WM_RBUTTONUP:
		apply_config_to_menu(menu, minfo);
		ClientToScreen(hwnd, &cl_pt);
		TrackPopupMenu(menu, TPM_LEFTALIGN | TPM_TOPALIGN, cl_pt.x, cl_pt.y, 0, hwnd, NULL);
		return 0;

		///////////////////////////////////////////
		//		WM_MBUTTONDOWN
		//		(�}�E�X�̒����{�^���������ꂽ�Ƃ�)
		///////////////////////////////////////////
	case WM_MBUTTONDOWN:
		move_view	= TRUE;
		pt_view		= cl_pt;
		prev_scale_x		= g_disp_info.scale.x;
		prev_scale_y		= g_disp_info.scale.y;
		prev_o_x			= g_disp_info.origin.x;
		prev_o_y			= g_disp_info.origin.y;
		SetCursor(LoadCursor(NULL, IDC_SIZEALL));
		SetCapture(hwnd);
		return 0;

		///////////////////////////////////////////
		//		WM_MBUTTONUP
		//		(�}�E�X�̒����{�^���������ꂽ�Ƃ�)
		///////////////////////////////////////////
	case WM_MBUTTONUP:
		move_view = FALSE;
		move_or_scale = NULL;
		ReleaseCapture();
		return 0;

		///////////////////////////////////////////
		//		WM_MOUSEMOVE
		//		(�E�B���h�E��ŃJ�[�\�����������Ƃ�)
		///////////////////////////////////////////
	case WM_MOUSEMOVE:
		//����_�ړ�
		//Value���[�h
		if (move_pt) {
			g_curve_value.move_point(move_pt - 1, gr_pt);
			InvalidateRect(hwnd, NULL, FALSE);
		}
		//ID���[�h
		if (g_config.mode) {
			if (address.position) {
				g_curve_id[g_config.current_id].move_point(address, gr_pt, FALSE);
				InvalidateRect(hwnd, NULL, FALSE);
			}
		}

		//����_�z�o�[���ɃJ�[�\����ύX
		//Value���[�h
		if (!g_config.mode) {
			if (g_curve_value.point_in_ctpts(cl_pt) > 0)
				SetCursor(LoadCursor(NULL, IDC_HAND));
		}
		//ID���[�h
		else {
			if (g_curve_id[g_config.current_id].pt_in_ctpt(cl_pt).position > 0)
				SetCursor(LoadCursor(NULL, IDC_HAND));
		}

		// �r���[�ړ��E�g��k��
		if (move_view && !move_pt) {
			// �g��k��
			if ((GetAsyncKeyState(VK_CONTROL) < 0 && move_or_scale == 0) || move_or_scale == 2) {
				double coef_x, coef_y;
				coef_x = MINMAXLIM(std::pow(CE_GR_SCALE_INC, cl_pt.x - pt_view.x),
					CE_GR_SCALE_MIN / prev_scale_x, CE_GR_SCALE_MAX / prev_scale_x);
				coef_y = MINMAXLIM(std::pow(CE_GR_SCALE_INC, pt_view.y - cl_pt.y),
					CE_GR_SCALE_MIN / prev_scale_y, CE_GR_SCALE_MAX / prev_scale_y);
				if (GetAsyncKeyState(VK_SHIFT) < 0) {
					coef_x = coef_y = max(coef_x, coef_y);
					prev_scale_x = prev_scale_y = max(prev_scale_x, prev_scale_y);
				}
				g_disp_info.scale.x = prev_scale_x * coef_x;
				g_disp_info.scale.y = prev_scale_y * coef_y;
				g_disp_info.origin.x = rect_wnd.right * 0.5f + (float)(coef_x * (prev_o_x - rect_wnd.right * 0.5f));
				g_disp_info.origin.y = rect_wnd.bottom * 0.5f + (float)(coef_y * (prev_o_y - rect_wnd.bottom * 0.5f));
				if (move_or_scale == 0)
					move_or_scale = 2;
			}
			// �ړ�
			else {
				g_disp_info.origin.x = prev_o_x + cl_pt.x - pt_view.x;
				g_disp_info.origin.y = prev_o_y + cl_pt.y - pt_view.y;
				if (move_or_scale == 0)
					move_or_scale = 1;
			}
			SetCursor(LoadCursor(NULL, IDC_SIZEALL));
			// �ĕ`��
			InvalidateRect(hwnd, NULL, FALSE);
		}

		// �J�[�u��D&D����
		/*if (is_dragging) {
			SetCursor(LoadCursor(g_fp->dll_hinst, MAKEINTRESOURCE(IDC_DRAGGING)));
		}*/

		return 0;

		///////////////////////////////////////////
		//		WM_MOUSEWHEEL
		//		(�}�E�X�z�C�[������]�����Ƃ�)
		///////////////////////////////////////////
	case WM_MOUSEWHEEL:
	{
		// Ctrl�L�[��������Ă���Ƃ�(���Ɉړ�)
		if (GetAsyncKeyState(VK_CONTROL) < 0 && GetAsyncKeyState(VK_SHIFT) >= 0)
			g_disp_info.origin.x += (float)(GET_Y_LPARAM(wparam) * CE_GR_WHEEL_COEF_POS);

		// Shift�L�[��������Ă���Ƃ�(�c�Ɉړ�)
		else if (GetAsyncKeyState(VK_SHIFT) < 0 && GetAsyncKeyState(VK_CONTROL) >= 0)
			g_disp_info.origin.y += (float)(GET_Y_LPARAM(wparam) * CE_GR_WHEEL_COEF_POS);

		// �k�ڂ̏��������ݒ�
		else {
			double coef = std::pow(CE_GR_SCALE_INC, GET_Y_LPARAM(wparam) * CE_GR_WHEEL_COEF_SCALE);
			double scale_after_x, scale_after_y;

			if (max(g_disp_info.scale.x, g_disp_info.scale.y) > CE_GR_SCALE_MAX / coef) {
				coef = CE_GR_SCALE_MAX / max(g_disp_info.scale.x, g_disp_info.scale.y);
			}
			else if (min(g_disp_info.scale.x, g_disp_info.scale.y) < CE_GR_SCALE_MIN / coef) {
				coef = CE_GR_SCALE_MIN / min(g_disp_info.scale.x, g_disp_info.scale.y);
			}

			scale_after_x = g_disp_info.scale.x * coef;
			scale_after_y = g_disp_info.scale.y * coef;

			g_disp_info.origin.x = (g_disp_info.origin.x - rect_wnd.right * 0.5f) * (float)(scale_after_x / g_disp_info.scale.x) + rect_wnd.right * 0.5f;
			g_disp_info.origin.y = (g_disp_info.origin.y - rect_wnd.bottom * 0.5f) * (float)(scale_after_y / g_disp_info.scale.y) + rect_wnd.bottom * 0.5f;

			g_disp_info.scale.x = scale_after_x;
			g_disp_info.scale.y = scale_after_y;
		}
		// �ĕ`��
		InvalidateRect(hwnd, NULL, FALSE);
		return 0;
	}

		///////////////////////////////////////////
		//		WM_COMMAND
		//		(�R�}���h)
		///////////////////////////////////////////
	case WM_COMMAND:
		switch (wparam) {
		// �ĕ`��
		case CE_CM_REDRAW:
			InvalidateRect(hwnd, NULL, FALSE);
			return 0;

		// Value���[�h�ɕύX
		case ID_MENU_MODE_VALUE:
			g_config.mode = 0;
			InvalidateRect(hwnd, NULL, FALSE);
			return 0;

		// ID���[�h�ɕύX
		case ID_MENU_MODE_ID:
			g_config.mode = 1;
			InvalidateRect(hwnd, NULL, FALSE);
			return 0;

		// �O���t���t�B�b�g
		case ID_MENU_FIT:
		case CE_CT_FIT:
			g_disp_info.origin.x = CE_GR_PADDING;
			g_disp_info.origin.y = (float)(rect_wnd.bottom - CE_GR_PADDING);
			g_disp_info.scale.x = ((double)rect_wnd.right - (int)(2 * CE_GR_PADDING)) / (double)CE_GR_RESOLUTION;
			g_disp_info.scale.y = ((double)rect_wnd.bottom - (int)(2 * CE_GR_PADDING)) / (double)CE_GR_RESOLUTION;
			InvalidateRect(hwnd, NULL, FALSE);
			return 0;

		// �n���h���𐮗�
		case ID_MENU_ALIGNHANDLE:
		case CE_CT_ALIGN:
			g_config.align_handle = g_config.align_handle ? 0 : 1;
			return 0;

		// �J�[�u�𔽓]
		case ID_MENU_REVERSE:
		case CE_CM_REVERSE:
			// ID���[�h
			if (g_config.mode == 1)
				g_curve_id[g_config.current_id].reverse_curve();
			// Value���[�h
			else {
				POINT pt_tmp = g_curve_value.ctpt[0];
				g_curve_value.ctpt[0].x = CE_GR_RESOLUTION - g_curve_value.ctpt[1].x;
				g_curve_value.ctpt[0].y = CE_GR_RESOLUTION - g_curve_value.ctpt[1].y;
				g_curve_value.ctpt[1].x = CE_GR_RESOLUTION - pt_tmp.x;
				g_curve_value.ctpt[1].y = CE_GR_RESOLUTION - pt_tmp.y;
			}
			InvalidateRect(hwnd, NULL, FALSE);
			return 0;

		// �n���h����\��
		case ID_MENU_SHOWHANDLE:
		case CE_CM_SHOWHANDLE:
			g_config.show_handle = g_config.show_handle ? 0 : 1;
			InvalidateRect(hwnd, NULL, FALSE);
			return 0;

		// �J�[�u���폜
		case ID_MENU_DELETE:
		case CE_CM_DELETE:
		{
			int response = IDOK;
			if (g_config.alert)
				response = MessageBox(hwnd, CE_STR_DELETE, CE_PLUGIN_NAME, MB_OKCANCEL | MB_ICONEXCLAMATION);
			if (response == IDOK) {
				if (g_config.mode)
					g_curve_id[g_config.current_id].clear();
				else
					g_curve_value.init();

				InvalidateRect(hwnd, NULL, FALSE);
			}
			return 0;
		}

		// �J�[�u�̃v���p�e�B
		case ID_MENU_PROPERTY:
		{
			std::string info;
			info = "ID : " + std::to_string(g_config.current_id) + "\n"
				+ "�|�C���g�� : " + std::to_string(g_curve_id[g_config.current_id].ctpts.size);
			MessageBox(hwnd, info.c_str(), CE_PLUGIN_NAME, MB_OK | MB_ICONINFORMATION);
			return 0;
		}

		//�ݒ�
		case ID_MENU_CONFIG:
			DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_CONFIG), hwnd, dialogproc_settings);
			InvalidateRect(hwnd, NULL, FALSE);
			SendMessage(hwnd_parent, WM_COMMAND, CE_CM_REDRAW, 0);
			return 0;

		// �{�v���O�C���ɂ���
		case ID_MENU_ABOUT:
			MessageBox(hwnd, CE_STR_ABOUT, CE_PLUGIN_NAME, MB_OK);
			return 0;

		// �l���R�s�[
		case ID_MENU_COPY:
		case CE_CT_COPY:
		{
			TCHAR result_str[12];
			int result = g_curve_value.create_value_1d();
			//������֕ϊ�
			_itoa_s(result, result_str, 12, 10);
			//�R�s�[
			copy_to_clipboard(hwnd, result_str);
			return 0;
		}

		// �l���R�s�[(4����)
		case ID_MENU_COPY4D:
		{
			std::string buffer = g_curve_value.create_value_4d();
			LPCTSTR result = buffer.c_str();
			copy_to_clipboard(hwnd, result);
			return 0;
		}

		// �l��ǂݎ��
		case ID_MENU_READ:
			DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_READ), hwnd, dialogproc_read);
			InvalidateRect(hwnd, NULL, FALSE);
			return 0;
		}
		return 0;

	default:
		return(DefWindowProc(hwnd, msg, wparam, lparam));
	}
	return 0;
}



//---------------------------------------------------------------------
//		�E�B���h�E�v���V�[�W���i�t�b�^�p�l���j
//---------------------------------------------------------------------
LRESULT CALLBACK wndproc_footer(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	RECT rect_wnd;
	static HWND hwnd_test;
	static ce::Bitmap_Canvas canvas;
	static ce::Control copy, read, save, clear, fit;
	RECT rect_buttons_parent, rect_copy, rect_read, rect_save, rect_clear, rect_fit;

	LPRECT lprect_buttons[] = {
		&rect_copy,
		&rect_read,
		&rect_save,
		&rect_clear,
		&rect_fit
	};

	GetClientRect(hwnd, &rect_wnd);

	rect_buttons_parent = {
		CE_MARGIN,
		CE_MARGIN * 2 + CE_CT_H,
		rect_wnd.right - CE_MARGIN,
		CE_MARGIN * 2 + CE_CT_H * 2
	};
	divide_rect(&rect_buttons_parent, lprect_buttons, 5);

	switch (msg) {
	case WM_CLOSE:
		canvas.exit();
		return 0;

	case WM_CREATE:
		canvas.init(hwnd);

		copy.create(hwnd, "CTRL_COPY", NULL, CE_CT_COPY, &rect_copy);
		read.create(hwnd, "CTRL_READ", NULL, CE_CT_READ, &rect_read);
		save.create(hwnd, "CTRL_SAVE", NULL, CE_CT_SAVE, &rect_save);
		clear.create(hwnd, "CTRL_CLEAR", NULL, CE_CT_DELETE, &rect_clear);
		fit.create(hwnd, "CTRL_FIT", NULL, CE_CT_FIT, &rect_fit);
		return 0;

	case WM_SIZE:
		copy.move(&rect_copy);
		read.move(&rect_read);
		save.move(&rect_save);
		clear.move(&rect_clear);
		fit.move(&rect_fit);
		return 0;

	case WM_PAINT:
		draw_footer(&canvas, &rect_wnd);
		return 0;

	case WM_COMMAND:
		switch (wparam) {
		case CE_CT_COPY:
			SendMessage(g_window.graph, WM_COMMAND, CE_CT_COPY, 0);
			return 0;


		}
		return 0;

	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}



//---------------------------------------------------------------------
//		�E�B���h�E�v���V�[�W���i���C�u�����j
//---------------------------------------------------------------------
LRESULT CALLBACK wndproc_library(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	POINT			cl_pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
	RECT			rect_wnd;
	static ce::Bitmap_Canvas canvas;

	GetClientRect(hwnd, &rect_wnd);


	switch (msg) {
	case WM_CLOSE:
		canvas.exit();
		return 0;

	case WM_CREATE:
		canvas.init(hwnd);
		return 0;

	case WM_PAINT:
		draw_panel_library(&canvas, &rect_wnd);
		return 0;

	case WM_COMMAND:
		/*switch (wparam) {
		case CE_CM_REDRAW:
			InvalidateRect(hwnd, NULL, FALSE);
			SendMessage(buttons.hDefault, WM_COMMAND, CE_CM_REDRAW, 0);
			SendMessage(buttons.hUser, WM_COMMAND, CE_CM_REDRAW, 0);
			return 0;
		}*/
		return 0;

	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
}