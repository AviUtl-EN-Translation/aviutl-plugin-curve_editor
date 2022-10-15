//----------------------------------------------------------------------------------
//		Curve Editor
//		�\�[�X�t�@�C�� (�t�B���^�֐�)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

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
	ini_load_configs(fp);
	d2d_init();
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
	int size_curve_id = sizeof(ce::Points_ID) * CE_POINT_MAX * CE_CURVE_MAX;
	if (!data) {
		*size = sizeof(g_curve_id);
	}
	else {
		memcpy(data, g_curve_id, sizeof(g_curve_id));
	}
	return TRUE;
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
			&rect_wnd
		);
		return 0;

	case WM_SIZE:
		g_window_main.move(&rect_wnd);
		return 0;

	case WM_GETMINMAXINFO:
		MINMAXINFO* mmi;
		mmi = (MINMAXINFO*)lparam;
		mmi->ptMaxTrackSize.x = CE_MAX_W;
		mmi->ptMaxTrackSize.y = CE_MAX_H;
		mmi->ptMinTrackSize.y = g_config.separator + CE_SEPR_W + CE_HEADER_H + CE_NON_CLIENT_H;
		return 0;

	case WM_KEYDOWN:
		return on_keydown(wparam);

	case WM_COMMAND:
		switch (wparam) {
		case CE_CM_REDRAW_AVIUTL:
			return TRUE;
		}
	}
	return 0;
}