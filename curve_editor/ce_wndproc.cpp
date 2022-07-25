//----------------------------------------------------------------------------------
//		Curve Editor
//		�\�[�X�t�@�C��(�E�B���h�E�v���V�[����)
//		(Visual C++ 2022)
//----------------------------------------------------------------------------------

#include "ce_wndproc.hpp"

//�f�o�b�O
#define MSGBOX(str) MessageBox(hwnd, str, "Debug", MB_ICONINFORMATION)

//---------------------------------------------------------------------
//		�E�B���h�E�v���V�[�W���i�x�[�X�j
//---------------------------------------------------------------------
BOOL WndProc_Base(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, void* editp, FILTER* fp)
{
	static HWND hwnd_main;
	RECT rect_wnd;
	GetClientRect(hwnd, &rect_wnd);

	switch (msg) {
	case WM_FILTER_INIT:
		SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) | WS_CLIPCHILDREN);
		hwnd_main = CreateChild(
			hwnd, WndProc_Main, "Wnd_Main", WS_CLIPCHILDREN,
			0, 0, rect_wnd.right, rect_wnd.bottom
		);
		g_wnd.base = hwnd;
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
			if (g_wnd.graph) SendMessage(g_wnd.graph, WM_COMMAND, CE_WM_REVERSE, 0);
			return 0;

		case 67: //[C]
			if (GetAsyncKeyState(VK_CONTROL) < 0)
				SendMessage(g_wnd.graph, WM_COMMAND, CE_CT_APPLY, 0);
			return 0;

		case 83: //[S]
			if (GetAsyncKeyState(VK_CONTROL) < 0)
				SendMessage(g_wnd.graph, WM_COMMAND, CE_CT_SAVE, 0);
			else
				SendMessage(g_wnd.graph, WM_COMMAND, CE_WM_SHOWHANDLE, 0);
			return 0;

		case 37: //[<]	
			g_cfg.id_current--;
			g_cfg.id_current = MINMAXLIM(g_cfg.id_current, 0, CE_CURVE_MAX - 1);
			if (g_wnd.graph) SendMessage(g_wnd.graph, WM_COMMAND, CE_WM_REDRAW, 0);
			return 0;

		case 39: //[>]
			g_cfg.id_current++;
			g_cfg.id_current = MINMAXLIM(g_cfg.id_current, 0, CE_CURVE_MAX - 1);
			if (g_wnd.graph) SendMessage(g_wnd.graph, WM_COMMAND, CE_WM_REDRAW, 0);
			return 0;

		case 70: //[F]
			if (g_wnd.graph) SendMessage(g_wnd.graph, WM_COMMAND, CE_CT_FIT, 0);
			return 0;

		case 65: //[A]
			if (g_wnd.graph) SendMessage(g_wnd.graph, WM_COMMAND, CE_CT_ALIGN, 0);
			return 0;
		}
		return 0;
	}
	return 0;
}



//---------------------------------------------------------------------
//		�E�B���h�E�v���V�[�W���i���C���j
//---------------------------------------------------------------------
LRESULT CALLBACK WndProc_Main(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	static BOOL bSepr = FALSE;
	POINT cl_pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
	RECT rect_wnd, rect_sepr;
	//HANDLE
	static HWND hwnd_editor, hwnd_side;
	static HDC hdc, hdc_mem;
	static HBITMAP bitmap;

	GetClientRect(hwnd, &rect_wnd);

	rect_sepr = {
		g_cfg.sepr - CE_SEPR_W,
		0,
		g_cfg.sepr + CE_SEPR_W,
		rect_wnd.bottom
	};

	switch (msg) {
	case WM_CLOSE:
		DeleteDC(hdc_mem);
		DeleteObject(bitmap);
		return 0;

	case WM_CREATE:
		hdc = GetDC(hwnd);
		hdc_mem = CreateCompatibleDC(hdc);
		bitmap = CreateCompatibleBitmap(hdc, CE_MAX_W, CE_MAX_H);
		SelectObject(hdc_mem, bitmap);
		ReleaseDC(hwnd, hdc);

		//�T�C�h�p�l��
		hwnd_side = CreateChild(
			hwnd, WndProc_Side, "Wnd_Side", WS_CLIPCHILDREN,
			CE_WD_POS_SIDE(rect_wnd)
		);

		//�G�f�B�^
		hwnd_editor = CreateChild(
			hwnd, WndProc_Editor, "Wnd_Editor", WS_CLIPCHILDREN,
			CE_WD_POS_EDT(rect_wnd)
		);

		g_wnd.main = hwnd;
		return 0;

	case WM_PAINT:
		DrawMain(hwnd, hdc_mem, &rect_wnd);
		return 0;

	case WM_SIZE:
		MoveWindow(hwnd_side, CE_WD_POS_SIDE(rect_wnd), TRUE);
		MoveWindow(hwnd_editor, CE_WD_POS_EDT(rect_wnd), TRUE);
		return 0;

	case WM_LBUTTONDOWN:
		if (PtInRect(&rect_sepr, cl_pt)) {
			bSepr = TRUE;
			SetCursor(LoadCursor(NULL, IDC_SIZEWE));
			SetCapture(hwnd);
		}
		return 0;

	case WM_LBUTTONUP:
		bSepr = FALSE;
		ReleaseCapture();
		return 0;

	case WM_MOUSEMOVE:
		if (PtInRect(&rect_sepr, cl_pt)) SetCursor(LoadCursor(NULL, IDC_SIZEWE));
		if (bSepr) {
			g_cfg.sepr = MINMAXLIM(cl_pt.x, CE_WD_SIDE_MINW + CE_SEPR_W, rect_wnd.right - CE_SEPR_W);
			MoveWindow(hwnd_side, CE_WD_POS_SIDE(rect_wnd), TRUE);
			MoveWindow(hwnd_editor, CE_WD_POS_EDT(rect_wnd), TRUE);
			InvalidateRect(hwnd, NULL, FALSE);
		}
		return 0;

	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
}



//---------------------------------------------------------------------
//		�E�B���h�E�v���V�[�W���i�T�C�h�p�l���j
//---------------------------------------------------------------------
LRESULT CALLBACK WndProc_Side(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	static HDC hdc, hdc_mem;
	static HBITMAP bitmap;
	RECT rect_wnd;
	static HWND hwnd_test;
	ce::Control
		mode_value{ g_theme[g_cfg.theme].bg_wnd },
		mode_id{ g_theme[g_cfg.theme].bg_wnd },
		apply{ g_theme[g_cfg.theme].bt_apply },
		save{ g_theme[g_cfg.theme].bg_wnd },
		read{ g_theme[g_cfg.theme].bg_wnd },
		align{ g_theme[g_cfg.theme].bg_wnd },
		pref{ g_theme[g_cfg.theme].bg_wnd };

	GetClientRect(hwnd, &rect_wnd);

	switch (msg) {
	case WM_CLOSE:
		DeleteDC(hdc_mem);
		DeleteObject(bitmap);
		return 0;

	case WM_CREATE:
		hdc = GetDC(hwnd);
		hdc_mem = CreateCompatibleDC(hdc);
		bitmap = CreateCompatibleBitmap(hdc, CE_MAX_W, CE_MAX_H);
		SelectObject(hdc_mem, bitmap);
		ReleaseDC(hwnd, hdc);

		hwnd_test = CreateChild(hwnd, WndProc_Test, "test", NULL, CE_CT_POS_APPLY(rect_wnd));

		//��������o���ƃo�O��

		/*mode_value.Create(
			hwnd, NULL,
			"Ct_Mode_Value",
			"Value",
			CE_CT_MODE_VALUE,
			CE_CT_POS_MODE_VALUE(rect_wnd),
			CE_EDGE_RT
		);

		mode_id.Create(
			hwnd, NULL,
			"Ct_Mode_ID",
			"ID",
			CE_CT_MODE_ID,
			CE_CT_POS_MODE_ID(rect_wnd),
			CE_EDGE_LT
		);

		apply.Create(
			hwnd, NULL,
			"Ct_Apply",
			"Apply",
			CE_CT_APPLY,
			CE_CT_POS_APPLY(rect_wnd),
			NULL
		);

		save.Create(
			hwnd, NULL,
			"Ct_Save",
			CE_ICON_SAVE,
			CE_CT_SAVE,
			CE_CT_POS_SAVE(rect_wnd),
			CE_EDGE_LB
		);

		read.Create(
			hwnd, NULL,
			"Ct_Read",
			CE_ICON_READ,
			CE_CT_READ,
			CE_CT_POS_READ(rect_wnd),
			NULL
		);

		align.Create(
			hwnd, NULL,
			"Ct_Align",
			CE_ICON_ALIGN,
			CE_CT_ALIGN,
			CE_CT_POS_ALIGN(rect_wnd),
			NULL
		);

		pref.Create(
			hwnd, NULL,
			"Ct_Pref",
			CE_ICON_PREF,
			CE_CT_PREF,
			CE_CT_POS_PREF(rect_wnd),
			CE_EDGE_RB
		);*/

		return 0;

	case WM_PAINT:
		DrawSide(hwnd, hdc_mem, &rect_wnd);
		return 0;

	case WM_SIZE:
		MoveWindow(hwnd_test, CE_CT_POS_APPLY(rect_wnd), TRUE);
		return 0;

	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}


//---------------------------------------------------------------------
//		�E�B���h�E�v���V�[�W���i�G�f�B�^�p�l���j
//---------------------------------------------------------------------
LRESULT CALLBACK WndProc_Editor(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//int
	int result;
	//LPTSTR, std::string
	LPCTSTR lpcsResult;
	std::string strBuffer;

	//POINT
	POINT ctpt_tmp;
	POINT cl_pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
	POINT gr_pt;

	//RECT
	RECT rect_wnd;

	//HANDLE
	static HDC hdc, hdc_mem;
	static HBITMAP bitmap;
	static HMENU menu;
	static HWND hwnd_parent, hwnd_graph;

	//���̑�
	static ce::Point_Address address;
	static ce::Control
		prev_play{ g_theme[g_cfg.theme].bg_wnd },
		prev_dur{ g_theme[g_cfg.theme].bg_wnd },
		fit{ g_theme[g_cfg.theme].bg_wnd },
		id_back{ g_theme[g_cfg.theme].bg_wnd },
		id_next{ g_theme[g_cfg.theme].bg_wnd },
		id{ g_theme[g_cfg.theme].bg_wnd },
		id_delete{ g_theme[g_cfg.theme].bg_wnd };


	//�N���C�A���g�̈�̋�`���擾
	GetClientRect(hwnd, &rect_wnd);

	switch (msg) {
	case WM_CLOSE:
		DeleteDC(hdc_mem);
		DeleteObject(bitmap);
		return 0;

		///////////////////////////////////////////
		//		WM_CREATE
		//		(�E�B���h�E���쐬���ꂽ�Ƃ�)
		///////////////////////////////////////////
	case WM_CREATE:
		//Load CompatibleItems
		hdc = GetDC(hwnd);
		hdc_mem = CreateCompatibleDC(hdc);
		bitmap = CreateCompatibleBitmap(hdc, CE_MAX_W, CE_MAX_H);
		SelectObject(hdc_mem, bitmap);
		ReleaseDC(hwnd, hdc);

		////���j���[
		//menu = GetSubMenu(LoadMenu(g_fp->dll_hinst, MAKEINTRESOURCE(g_cfg.lang ? IDR_MENU2 : IDR_MENU1)), 0);

		/*prev_play.Create(hwnd, NULL, "prev_play", CE_ICON_PREV, &g_th_dark.bt_other, CT_PREV,
			rect_wnd.left + CE_MRG, rect_wnd.bottom - CE_MRG - CT_EDITOR_HEIGHT,
			CT_EDITOR_HEIGHT, CT_EDITOR_HEIGHT
			);*/

		//�O���t
		hwnd_graph = CreateChild(
			hwnd, WndProc_Graph, "Wnd_Graph", WS_CLIPCHILDREN,
			CE_WD_POS_GRAPH(rect_wnd)
		);
		hwnd_parent = GetParent(hwnd);
		g_wnd.editor = hwnd;
		return 0;

		///////////////////////////////////////////
		//		WM_SIZE
		//		(�T�C�Y���ύX���ꂽ�Ƃ�)
		///////////////////////////////////////////
	case WM_SIZE:
		MoveWindow(hwnd_graph, CE_WD_POS_GRAPH(rect_wnd), TRUE);
		return 0;

		///////////////////////////////////////////
		//		WM_PAINT
		//		(�E�B���h�E���`�悳�ꂽ�Ƃ�)
		///////////////////////////////////////////
	case WM_PAINT:
		DrawEditor(hwnd, hdc_mem, &rect_wnd);
		return 0;

		///////////////////////////////////////////
		//		WM_RBUTTONUP
		//		(�}�E�X�̉E�{�^���������ꂽ�Ƃ�)
		///////////////////////////////////////////
		//�E�N���b�N���j���[
	case WM_RBUTTONUP:
		ClientToScreen(hwnd, &cl_pt);
		TrackPopupMenu(menu, TPM_LEFTALIGN | TPM_TOPALIGN, cl_pt.x, cl_pt.y, 0, hwnd, NULL);
		return 0;

		///////////////////////////////////////////
		//		WM_COMMAND
		//		(�R�}���h���󂯎�����Ƃ�)
		///////////////////////////////////////////
	case WM_COMMAND:
		//switch (LOWORD(wparam)) {
		//	//COPY�{�^��
		//case ID_RCLICKMENU_COPYVALUES:
		//case CT_APPLY:
		//	TCHAR chResult[12];
		//	result = Create1DValue();
		//	if (result == OUTOFRANGE) {
		//		if (g_cfg.bAlerts)
		//			MessageBox(hwnd, g_cfg.lang ? FLSTR_JA_OUTOFRANGE : FLSTR_OUTOFRANGE, TEXT("Flow"), MB_OK | MB_ICONINFORMATION);
		//		return 0;
		//	}
		//	//������֕ϊ�
		//	_itoa_s(result, chResult, 12, 10);
		//	//�R�s�[
		//	CopyToClipboard(hwnd, chResult);
		//	return 0;

		//	//�ۑ��{�^��
		//case ID_RCLICKMENU_SAVEPRESET:
		//case CT_SAVE:
		//	//�v�Z
		//	if (g_cv_vl.ctpt[0].y < -2735 || 3735 < g_cv_vl.ctpt[0].y || g_cv_vl.ctpt[1].y < -2735 || 3735 < g_cv_vl.ctpt[1].y) {
		//		if (g_cfg.bAlerts) MessageBox(hwnd, g_cfg.lang ? FLSTR_JA_OUTOFRANGE : FLSTR_OUTOFRANGE, "Flow", MB_OK | MB_ICONINFORMATION);
		//		return 0;
		//	}
		//	DialogBox(g_fp->dll_hinst, g_cfg.lang ? MAKEINTRESOURCE(IDD_SAVE_JA) : MAKEINTRESOURCE(IDD_SAVE), hwnd, DlgProc_Save);
		//	return 0;

		//	//����_�𔽓]
		//case CM_REVERSE:
		//	pt_tmp = g_cv_vl.ctpt[0];
		//	g_cv_vl.ctpt[0].x = CE_GR_RES - g_cv_vl.ctpt[1].x;
		//	g_cv_vl.ctpt[0].y = CE_GR_RES - g_cv_vl.ctpt[1].y;
		//	g_cv_vl.ctpt[1].x = CE_GR_RES - pt_tmp.x;
		//	g_cv_vl.ctpt[1].y = CE_GR_RES - pt_tmp.y;
		//	InvalidateRect(hwnd, NULL, FALSE);
		//	return 0;

		//	//Value�p�l��
		//case CM_EDITOR_VALUE:
		//	if (!g_cfg.lang) DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_VALUE), hwnd, DlgProc_Value);
		//	else DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_VALUE_JA), hwnd, DlgProc_Value);
		//	InvalidateRect(hwnd, NULL, FALSE);
		//	RedrawChildren();
		//	return 0;

		//	//�ǂݎ��{�^��
		//case CM_EDITOR_READ:
		//	if (!g_cfg.lang) DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_READ), hwnd, DlgProc_Read);
		//	else DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_READ_JA), hwnd, DlgProc_Read);
		//	InvalidateRect(hwnd, NULL, FALSE);
		//	return 0;

		//	//About
		//case ID_RCLICKMENU_ABOUT:
		//	if (!g_cfg.lang) DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_ABOUT), hwnd, DlgProc_About);
		//	else DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_ABOUT_JA), hwnd, DlgProc_About);
		//	return 0;

		//	//�ݒ�
		//case ID_RCLICKMENU_PREFERENCES:
		//	if (!g_cfg.lang) DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_PREF), hwnd, DlgProc_Pref);
		//	else DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_PREF_JA), hwnd, DlgProc_Pref);
		//	InvalidateRect(hwnd, NULL, FALSE);
		//	SendMessage(hwnd_parent, WM_COMMAND, CE_WM_REDRAW, 0);
		//	RedrawChildren();
		//	return 0;

		//	//�l���R�s�[
		//case ID_RCLICKMENU_COPYVALUES4:
		//	strBuffer = Create4DValue();
		//	lpcsResult = strBuffer.c_str();
		//	CopyToClipboard(hwnd, lpcsResult);
		//	return 0;

		//	//�R���g���[���ĕ`��
		//case CE_WM_REDRAW:
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
LRESULT CALLBACK WndProc_Graph(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//int
	static int		move_pt		= 0;			//0:None, 1:Ctpt1, 2:Ctpt2
	static BOOL		move_view	= 0;
	static int		o_x, o_y;

	//double
	static double	scale_x, scale_y;

	//POINT
	static POINT	pt_trace[] = { {0, 0}, {0, 0} };
	static POINT	pt_view;
	POINT			pt_tmp;							//�l���]�p
	POINT			cl_pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
	POINT			gr_pt;

	//RECT
	RECT			rect_wnd;

	//�n���h��
	static HDC		hdc, hdc_mem;
	static HBITMAP	bitmap;
	static HMENU	menu;
	static HWND		hwnd_parent;

	//���̑�
	static ce::Point_Address address;


	//�N���C�A���g�̈�̋�`���擾
	GetClientRect(hwnd, &rect_wnd);
	//�O���t���W
	if (g_disp_info.o.x != NULL)
		gr_pt = ToGraph(cl_pt);

	switch (msg) {
	case WM_CLOSE:
		DeleteDC(hdc_mem);
		DeleteObject(bitmap);
		return 0;

		///////////////////////////////////////////
		//		WM_CREATE
		//		(�E�B���h�E���쐬���ꂽ�Ƃ�)
		///////////////////////////////////////////
	case WM_CREATE:
		//Load CompatibleItems
		hdc		= GetDC(hwnd);
		hdc_mem = CreateCompatibleDC(hdc);
		bitmap	= CreateCompatibleBitmap(hdc, CE_MAX_W, CE_MAX_H);
		SelectObject(hdc_mem, bitmap);
		ReleaseDC(hwnd, hdc);

		//���j���[
		//menu = GetSubMenu(LoadMenu(g_fp->dll_hinst, MAKEINTRESOURCE(g_cfg.lang ? IDR_MENU2 : IDR_MENU1)), 0);

		pt_trace[0] = g_cv_vl.ctpt[0];
		pt_trace[1] = g_cv_vl.ctpt[1];

		g_disp_info.o.x = CE_GR_PADDING;
		g_disp_info.o.y = rect_wnd.bottom - CE_GR_PADDING;
		g_disp_info.scale.x = ((double)rect_wnd.right - (int)(2 * CE_GR_PADDING)) / (double)CE_GR_RES;
		g_disp_info.scale.y = ((double)rect_wnd.bottom - (int)(2 * CE_GR_PADDING)) / (double)CE_GR_RES;

		hwnd_parent = GetParent(hwnd);
		g_wnd.graph = hwnd;

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
		DrawGraph(hwnd, hdc_mem, pt_trace, &rect_wnd);
		return 0;

		///////////////////////////////////////////
		//		WM_LBUTTONDOWN
		//		(���N���b�N�����ꂽ�Ƃ�)
		///////////////////////////////////////////
	case WM_LBUTTONDOWN:
		//�l���[�h
		if (!g_cfg.mode) {
			//����_2���N���b�N���ꂽ�Ƃ�
			if (g_cv_vl.PtInCtpt(cl_pt) == 2) {
				move_pt = 2;
				pt_trace[0] = g_cv_vl.ctpt[0];
				pt_trace[1] = g_cv_vl.ctpt[1];
				SetCursor(LoadCursor(NULL, IDC_HAND));
				SetCapture(hwnd);
			}
			//����_1���N���b�N���ꂽ�Ƃ�
			else if (g_cv_vl.PtInCtpt(cl_pt) == 1) {
				move_pt = 1;
				pt_trace[0] = g_cv_vl.ctpt[0];
				pt_trace[1] = g_cv_vl.ctpt[1];
				SetCursor(LoadCursor(NULL, IDC_HAND));
				SetCapture(hwnd);
			}
		}
		//ID���[�h
		else {
			//�J�[�\��������_��ɂ��邩�ǂ���
			address = g_cv_id[g_cfg.id_current].PtInCtpts(cl_pt);
			//�J�[�\��������_��ɂ���Ƃ��C�n���h���̍��W���L��
			if (address.CLR != 0) {
				g_cv_id[g_cfg.id_current].MovePoint(address, gr_pt, TRUE);
				SetCursor(LoadCursor(NULL, IDC_HAND));
				SetCapture(hwnd);
			}
		}
		return 0;

		///////////////////////////////////////////
		//		WM_LBUTTONUP
		//		(�}�E�X�̍��{�^���������ꂽ�Ƃ�)
		///////////////////////////////////////////
	case WM_LBUTTONUP:
		//Value���[�h
		if (!g_cfg.mode) {
			//�߂��ɂ��鐧��_���J�[�\���Ɉړ�
			if (!move_pt) {
				pt_trace[0] = g_cv_vl.ctpt[0]; pt_trace[1] = g_cv_vl.ctpt[1];
				int d1 = (int)DISTANCE(ToClient(g_cv_vl.ctpt[0]), cl_pt);
				int d2 = (int)DISTANCE(ToClient(g_cv_vl.ctpt[1]), cl_pt);
				if (d1 < d2) {
					g_cv_vl.ctpt[0].x = ToGraph(cl_pt).x;
					g_cv_vl.ctpt[0].y = ToGraph(cl_pt).y;
				}
				else if (d1 >= d2) {
					g_cv_vl.ctpt[1].x = ToGraph(cl_pt).x;
					g_cv_vl.ctpt[1].y = ToGraph(cl_pt).y;
				}
				InvalidateRect(hwnd, NULL, FALSE);
				if (g_cfg.auto_copy) SendMessage(hwnd, WM_COMMAND, CE_CT_APPLY, 0);
			}
			//�ړ����[�h����
			if (move_pt) {
				move_pt = 0;
				if (g_cfg.auto_copy) SendMessage(hwnd, WM_COMMAND, CE_CT_APPLY, 0);
			}
		}
		//ID���[�h
		else address.CLR = 0;
		ReleaseCapture();
		return 0;

		///////////////////////////////////////////
		//		WM_MBUTTONDOWN
		//		(�}�E�X�̍��{�^�����_�u���N���b�N���ꂽ�Ƃ�)
		///////////////////////////////////////////
	case WM_LBUTTONDBLCLK:
		//�J�[�\��������_��ɂ��邩�ǂ���
		address = g_cv_id[g_cfg.id_current].PtInCtpts(cl_pt);

		if (address.CLR == 1)
			g_cv_id[g_cfg.id_current].DeletePoint(cl_pt);
		else
			g_cv_id[g_cfg.id_current].addPoint(gr_pt);

		InvalidateRect(hwnd, NULL, FALSE);
		return 0;

		///////////////////////////////////////////
		//		WM_RBUTTONUP
		//		(�}�E�X�̉E�{�^���������ꂽ�Ƃ�)
		///////////////////////////////////////////
		//�E�N���b�N���j���[
	case WM_RBUTTONUP:
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
		scale_x		= g_disp_info.scale.x;
		scale_y		= g_disp_info.scale.y;
		o_x			= g_disp_info.o.x;
		o_y			= g_disp_info.o.y;
		SetCapture(hwnd);
		return 0;

		///////////////////////////////////////////
		//		WM_MBUTTONUP
		//		(�}�E�X�̒����{�^���������ꂽ�Ƃ�)
		///////////////////////////////////////////
	case WM_MBUTTONUP:
		move_view = FALSE;
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
			g_cv_vl.MovePoint(move_pt - 1, gr_pt);
			InvalidateRect(hwnd, NULL, FALSE);
		}
		//ID���[�h
		if (g_cfg.mode) {
			if (address.CLR) {
				g_cv_id[g_cfg.id_current].MovePoint(address, gr_pt, FALSE);
				InvalidateRect(hwnd, NULL, FALSE);
			}
		}

		//����_�z�o�[���ɃJ�[�\����ύX
		//Value���[�h
		if (!g_cfg.mode) {
			if (g_cv_vl.PtInCtpt(cl_pt) > 0)
				SetCursor(LoadCursor(NULL, IDC_HAND));
		}
		//ID���[�h
		else {
			if (g_cv_id[g_cfg.id_current].PtInCtpts(cl_pt).CLR > 0)
				SetCursor(LoadCursor(NULL, IDC_HAND));
		}

		//�r���[�ړ��E�g��k��
		if (move_view && !move_pt) {
			if (GetAsyncKeyState(VK_CONTROL) < 0) {
				double ratio_x, ratio_y;
				ratio_x = MINMAXLIM(std::pow(CE_GR_SCALE_INC, cl_pt.x - pt_view.x),
					CE_GR_SCALE_MIN / scale_x, CE_GR_SCALE_MAX / scale_x);
				ratio_y = MINMAXLIM(std::pow(CE_GR_SCALE_INC, pt_view.y - cl_pt.y),
					CE_GR_SCALE_MIN / scale_y, CE_GR_SCALE_MAX / scale_y);
				g_disp_info.scale.x = scale_x * ratio_x;
				g_disp_info.scale.y = scale_y * ratio_y;
				g_disp_info.o.x = rect_wnd.right * 0.5 + ratio_x * (o_x - rect_wnd.right * 0.5);
				g_disp_info.o.y = rect_wnd.bottom * 0.5 + ratio_y * (o_y - rect_wnd.bottom * 0.5);
			}
			else {
				g_disp_info.o.x = o_x + cl_pt.x - pt_view.x;
				g_disp_info.o.y = o_y + cl_pt.y - pt_view.y;
			}
			InvalidateRect(hwnd, NULL, FALSE);
		}

		return 0;

		///////////////////////////////////////////
		//		WM_MOUSEWHEEL
		//		(�}�E�X�z�C�[������]�����Ƃ�)
		///////////////////////////////////////////
	case WM_MOUSEWHEEL:
		g_disp_info.scale.x = MINMAXLIM(g_disp_info.scale.x * std::pow(CE_GR_SCALE_INC, GET_Y_LPARAM(wparam) * CE_GR_WHEEL_RATIO), CE_GR_SCALE_MIN, CE_GR_SCALE_MAX);
		g_disp_info.scale.y = MINMAXLIM(g_disp_info.scale.y * std::pow(CE_GR_SCALE_INC, GET_Y_LPARAM(wparam) * CE_GR_WHEEL_RATIO), CE_GR_SCALE_MIN, CE_GR_SCALE_MAX);
		g_disp_info.o.x = (g_disp_info.o.x - rect_wnd.right * 0.5) * std::pow(CE_GR_SCALE_INC, GET_Y_LPARAM(wparam) * CE_GR_WHEEL_RATIO) + rect_wnd.right * 0.5;
		g_disp_info.o.y = (g_disp_info.o.y - rect_wnd.bottom * 0.5) * std::pow(CE_GR_SCALE_INC, GET_Y_LPARAM(wparam) * CE_GR_WHEEL_RATIO) + rect_wnd.bottom * 0.5;
		
		InvalidateRect(hwnd, NULL, FALSE);
		return 0;

		///////////////////////////////////////////
		//		WM_COMMAND
		//		(�R�}���h)
		///////////////////////////////////////////
	case WM_COMMAND:
		switch (wparam) {
		//�ĕ`��
		case CE_WM_REDRAW:
			InvalidateRect(hwnd, NULL, FALSE);
			return 0;

		case CE_CT_FIT:
			g_disp_info.o.x = CE_GR_PADDING;
			g_disp_info.o.y = rect_wnd.bottom - CE_GR_PADDING;
			g_disp_info.scale.x = ((double)rect_wnd.right - (int)(2 * CE_GR_PADDING)) / (double)CE_GR_RES;
			g_disp_info.scale.y = ((double)rect_wnd.bottom - (int)(2 * CE_GR_PADDING)) / (double)CE_GR_RES;
			InvalidateRect(hwnd, NULL, FALSE);
			return 0;

		case CE_CT_ALIGN:
			g_cfg.align_mode = g_cfg.align_mode ? 0 : 1;
			return 0;

		case CE_WM_REVERSE:
			g_cv_id[g_cfg.id_current].ReversePoints();
			InvalidateRect(hwnd, NULL, FALSE);
			return 0;

		case CE_WM_SHOWHANDLE:
			g_cfg.show_handle = g_cfg.show_handle ? 0 : 1;
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
//		�E�B���h�E�v���V�[�W���i���C�u�����j
//---------------------------------------------------------------------
LRESULT CALLBACK WndProc_Library(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	POINT			cl_pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
	HDC				hdc;
	static HDC		hdc_mem;
	static HBITMAP	bitmap;
	RECT			rect_wnd;

	GetClientRect(hwnd, &rect_wnd);


	switch (msg) {
	case WM_CLOSE:
		DeleteDC(hdc_mem);
		DeleteObject(bitmap);
		return 0;

	case WM_CREATE:
		hdc		= GetDC(hwnd);
		hdc_mem = CreateCompatibleDC(hdc);
		bitmap	= CreateCompatibleBitmap(hdc, CE_MAX_W, CE_MAX_H);
		SelectObject(hdc_mem, bitmap);
		ReleaseDC(hwnd, hdc);

		return 0;

	case WM_PAINT:
		DrawLibrary(hwnd, hdc_mem, &rect_wnd);
		return 0;

	case WM_SIZE:

		return 0;

	case WM_COMMAND:
		/*switch (wparam) {
		case CE_WM_REDRAW:
			InvalidateRect(hwnd, NULL, FALSE);
			SendMessage(buttons.hDefault, WM_COMMAND, CE_WM_REDRAW, 0);
			SendMessage(buttons.hUser, WM_COMMAND, CE_WM_REDRAW, 0);
			return 0;
		}*/
		return 0;

	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
}