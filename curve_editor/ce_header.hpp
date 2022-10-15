//----------------------------------------------------------------------------------
//		Curve Editor
//		�w�b�_�t�@�C��
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#pragma once

#include "ce_classes.hpp"



//----------------------------------------------------------------------------------
//		extern�錾
//----------------------------------------------------------------------------------
extern ce::Curve_Value						g_curve_value;
extern ce::Curve_Value						g_curve_value_previous;
extern ce::Curve_ID							g_curve_id[CE_CURVE_MAX];
extern ce::Curve_ID							g_curve_id_previous;
extern std::vector<ce::Preset>				g_presets;
extern const ce::Theme						g_theme_dark,
											g_theme_light;
extern const ce::Theme						g_theme[2];
extern ce::Config							g_config;
extern ce::Window							g_window_main,
											g_window_header,
											g_window_editor,
											g_window_preset;
extern ce::Graph_View_Info					g_view_info;
extern FILTER*								g_fp;
extern FILTER_DLL							g_filter;
//Direct2D
extern ID2D1Factory*						g_d2d1_factory;
extern ID2D1DCRenderTarget*					g_render_target;



//----------------------------------------------------------------------------------
//		�t�B���^�֐�
//----------------------------------------------------------------------------------

//�t�B���^�\���̂̃|�C���^��n��
EXTERN_C FILTER_DLL __declspec(dllexport)* __stdcall GetFilterTable(void);

//�������E�I��
BOOL				filter_init(FILTER*);
BOOL				filter_exit(FILTER*);

// aviutl.ini��p���Đݒ��ǂݏ���
void				ini_load_configs(FILTER* fp);
void				ini_write_configs(FILTER* fp);

// �v���W�F�N�g�ǂݍ��݁E�����o�����ɌĂяo�����֐�
BOOL				on_project_load(FILTER* fp, void* editp, void* data, int size);
BOOL				on_project_save(FILTER* fp, void* editp, void* data, int* size);

// �E�B���h�E�v���V�[�W��
BOOL				filter_wndproc(HWND, UINT, WPARAM, LPARAM, void*, FILTER*);



//----------------------------------------------------------------------------------
//		���̑��̊֐�
//----------------------------------------------------------------------------------

//������̕���
std::vector<std::string> split(const std::string& s, TCHAR c);

//�N���b�v�{�[�h�Ƀe�L�X�g���R�s�[
BOOL				copy_to_clipboard(HWND, LPCTSTR);

// ���������Z
ce::Float_Point		subtract_length(ce::Float_Point st, ce::Float_Point ed, float length);

// �ݒ�����j���[�ɔ��f
void				apply_config_to_menu(HMENU menu, MENUITEMINFO minfo);

// RECT�𕪊�
void				rect_divide(LPRECT rect_parent, LPRECT* rects_child, float* weights, int n);

// RECT�ɗ]����ݒ�
void				rect_set_margin(LPRECT rect, int left, int top, int right, int bottom);

// �L�[�������̏���
LRESULT				on_keydown(WPARAM wparam);



//----------------------------------------------------------------------------------
//		�`��֐�
//----------------------------------------------------------------------------------
void				d2d_init();
void				d2d_setup(ce::Bitmap_Canvas* canvas, LPRECT rect_wnd, COLORREF cr);
void				d2d_draw_rounded_edge(ID2D1SolidColorBrush* brush, LPRECT rect_wnd, int flag, float radius);
void				draw_panel_main(ce::Bitmap_Canvas* canvas, LPRECT rect_wnd, LPRECT rect_sepr);
void				draw_panel_header(ce::Bitmap_Canvas* canvas, LPRECT rect_wnd);
void				draw_panel_preset(ce::Bitmap_Canvas* canvas, LPRECT rect_wnd);
void				draw_panel_editor(ce::Bitmap_Canvas* canvas, LPRECT rect_wnd);



//----------------------------------------------------------------------------------
//		�E�B���h�E�v���V�[�W��
//----------------------------------------------------------------------------------
LRESULT CALLBACK	wndproc_main(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	wndproc_header(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	wndproc_editor(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	wndproc_preset(HWND, UINT, WPARAM, LPARAM);



//----------------------------------------------------------------------------------
//		�_�C�A���O�v���V�[�W��
//----------------------------------------------------------------------------------
BOOL CALLBACK		dialogproc_settings(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK		dialogproc_value(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK		dialogproc_read(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK		dialogproc_save(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK		dialogproc_id(HWND, UINT, WPARAM, LPARAM);



//----------------------------------------------------------------------------------
//		�R���g���[���v���V�[�W��
//----------------------------------------------------------------------------------
LRESULT CALLBACK	wndproc_control_value(HWND, UINT, WPARAM, LPARAM);



//----------------------------------------------------------------------------------
//		�C�����C���֐�
//----------------------------------------------------------------------------------

//�O���t -> �N���C�A���g
inline ce::Float_Point to_client(int gr_x, int gr_y)
{
	return {
		g_view_info.origin.x + (float)(gr_x * g_view_info.scale.x),
		g_view_info.origin.y - (float)(gr_y * g_view_info.scale.y)
	};
}

inline ce::Float_Point to_client(POINT gr_pt)
{
	return {
		to_client(gr_pt.x, gr_pt.y).x,
		to_client(gr_pt.x, gr_pt.y).y
	};
}

//�N���C�A���g -> �O���t
inline POINT to_graph(double cl_x, double cl_y)
{
	return {
		(int)((cl_x - g_view_info.origin.x) / g_view_info.scale.x),
		(int)((-cl_y + g_view_info.origin.y) / g_view_info.scale.y)
	};
}

inline POINT to_graph(POINT cl_pt)
{
	return {
		to_graph(cl_pt.x, cl_pt.y).x,
		to_graph(cl_pt.x, cl_pt.y).y
	};
}
