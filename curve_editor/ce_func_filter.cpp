//----------------------------------------------------------------------------------
//		Curve Editor
//		�\�[�X�t�@�C�� (�t�B���^�֐�)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include <yulib/extra.h>
#include "ce_header.hpp"



//---------------------------------------------------------------------
//		FILTER�\���̂̃|�C���^���擾
//---------------------------------------------------------------------
EXTERN_C FILTER_DLL __declspec(dllexport)* __stdcall GetFilterTable(void)
{
	return &g_filter;
}



//---------------------------------------------------------------------
//		���������Ɏ��s�����֐�
//---------------------------------------------------------------------
BOOL filter_init(FILTER* fp)
{
	g_fp = fp;

	// �J�[�u�̏�����
	g_curve_value.initialize(ce::Mode_Value);
	g_curve_value_previous.initialize(ce::Mode_Value);

	for (int i = 0; i < CE_CURVE_MAX; i++) {
		g_curve_id[i].initialize(ce::Mode_ID);
	}
	g_curve_id_previous.initialize(ce::Mode_ID);


	// aviutl.ini����ݒ��ǂݍ���
	ini_load_configs(fp);
	
	// �t�b�N�̏���
	g_config.is_hooked_popup = FALSE;
	g_config.is_hooked_dialog = FALSE;
	char exedit_path[1024];
	FILTER* fp_exedit = auls::Exedit_GetFilter(fp);
	::GetModuleFileName(fp_exedit->dll_hinst, exedit_path, sizeof(exedit_path));

	// TrackPopupMenu()���t�b�N
	TrackPopupMenu_original = (decltype(TrackPopupMenu_original))yulib::RewriteFunction(
		exedit_path,
		"TrackPopupMenu",
		TrackPopupMenu_hooked
	);

	// DialogBoxParamA()���t�b�N
	DialogBox_original = (decltype(DialogBox_original))yulib::RewriteFunction(
		exedit_path,
		"DialogBoxParamA",
		DialogBox_hooked
	);

	// Direct2D������
	ce::d2d_init();
	return TRUE;
}



//---------------------------------------------------------------------
//		�I�����Ɏ��s�����֐�
//---------------------------------------------------------------------
BOOL filter_exit(FILTER* fp)
{
	ini_write_configs(fp);
	if (NULL != g_render_target) {
		g_render_target->Release();
	}
	if (NULL != g_d2d1_factory) {
		g_d2d1_factory->Release();
	}
	return TRUE;
}



//---------------------------------------------------------------------
//		�v���W�F�N�g�t�@�C���ǂݍ��ݎ��Ɏ��s�����֐�
//---------------------------------------------------------------------
BOOL on_project_load(FILTER* fp, void* editp, void* data, int size)
{
	if (data) {
		memcpy(g_curve_id, data, size);
		g_curve_id_previous = g_curve_id[g_config.current_id];
		if (g_window_editor.hwnd)
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CE_CM_REDRAW, 0);
	}
	return TRUE;
}



//---------------------------------------------------------------------
//		�v���W�F�N�g�t�@�C���ۑ����Ɏ��s�����֐�
//---------------------------------------------------------------------
BOOL on_project_save(FILTER* fp, void* editp, void* data, int* size)
{
	int size_curve_id = sizeof(ce::Curve_Points) * CE_POINT_MAX * CE_CURVE_MAX;
	if (!data) {
		*size = sizeof(g_curve_id);
	}
	else {
		memcpy(data, g_curve_id, sizeof(g_curve_id));
	}
	return TRUE;
}



//---------------------------------------------------------------------
//		aviutl.ini����ݒ��ǂݍ���
//---------------------------------------------------------------------
void ini_load_configs(FILTER* fp)
{
	g_config.theme = fp->exfunc->ini_load_int(fp, "theme", 0);
	g_config.trace = fp->exfunc->ini_load_int(fp, "show_previous_curve", 1);
	g_config.alert = fp->exfunc->ini_load_int(fp, "show_alerts", 1);
	g_config.auto_copy = fp->exfunc->ini_load_int(fp, "auto_copy", 0);
	g_config.current_id = fp->exfunc->ini_load_int(fp, "id", 0);
	g_curve_value.ctpts[0].pt_right.x = MINMAXLIM(fp->exfunc->ini_load_int(fp, "x1", (int)(CE_GR_RESOLUTION * CE_CURVE_DEF_1)), 0, CE_GR_RESOLUTION);
	g_curve_value.ctpts[0].pt_right.y = fp->exfunc->ini_load_int(fp, "y1", (int)(CE_GR_RESOLUTION * CE_CURVE_DEF_1));
	g_curve_value.ctpts[1].pt_left.x = MINMAXLIM(fp->exfunc->ini_load_int(fp, "x2", (int)(CE_GR_RESOLUTION * CE_CURVE_DEF_2)), 0, CE_GR_RESOLUTION);
	g_curve_value.ctpts[1].pt_left.y = fp->exfunc->ini_load_int(fp, "y2", (int)(CE_GR_RESOLUTION * CE_CURVE_DEF_2));
	g_config.separator = fp->exfunc->ini_load_int(fp, "separator", CE_SEPR_W);
	g_config.mode = (ce::Mode)fp->exfunc->ini_load_int(fp, "mode", 0);
	g_config.align_handle = fp->exfunc->ini_load_int(fp, "align_handle", 1);
	g_config.show_handle = fp->exfunc->ini_load_int(fp, "show_handle", 1);
	g_config.preset_size = fp->exfunc->ini_load_int(fp, "preset_size", CE_DEF_PRESET_SIZE);
}



//---------------------------------------------------------------------
//		aviutl.ini����ݒ����������
//---------------------------------------------------------------------
void ini_write_configs(FILTER* fp)
{
	fp->exfunc->ini_save_int(fp, "x1", g_curve_value.ctpts[0].pt_right.x);
	fp->exfunc->ini_save_int(fp, "y1", g_curve_value.ctpts[0].pt_right.y);
	fp->exfunc->ini_save_int(fp, "x2", g_curve_value.ctpts[1].pt_left.x);
	fp->exfunc->ini_save_int(fp, "y2", g_curve_value.ctpts[1].pt_left.y);
	fp->exfunc->ini_save_int(fp, "separator", g_config.separator);
	fp->exfunc->ini_save_int(fp, "mode", g_config.mode);
	fp->exfunc->ini_save_int(fp, "align_handle", g_config.align_handle);
	fp->exfunc->ini_save_int(fp, "show_handle", g_config.show_handle);
}



//---------------------------------------------------------------------
//		�E�B���h�E�v���V�[�W��
//---------------------------------------------------------------------
BOOL filter_wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, void* editp, FILTER* fp)
{
	RECT rect_wnd;
	::GetClientRect(hwnd, &rect_wnd);

	switch (msg) {
		// �E�B���h�E�쐬��
	case WM_FILTER_INIT:
		// WS_CLIPCHILDREN��ǉ�
		::SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) | WS_CLIPCHILDREN);

		g_window_main.create(
			hwnd, "WINDOW_MAIN", wndproc_main, NULL,
			rect_wnd
		);
		return 0;

	case WM_SIZE:
		g_window_main.move(rect_wnd);
		return 0;

	case WM_GETMINMAXINFO:
		MINMAXINFO* mmi;
		mmi = (MINMAXINFO*)lparam;
		mmi->ptMaxTrackSize.x = CE_MAX_W;
		mmi->ptMaxTrackSize.y = CE_MAX_H;
		mmi->ptMinTrackSize.y = g_config.separator + CE_SEPR_W + CE_HEADER_H + CE_NON_CLIENT_H;
		return 0;

	case WM_KEYDOWN:
		return ce::on_keydown(wparam);

	case WM_COMMAND:
		switch (wparam) {
		case CE_CM_REDRAW_AVIUTL:
			return TRUE;
		}
	}
	return 0;
}