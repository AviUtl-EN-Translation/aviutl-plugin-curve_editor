//----------------------------------------------------------------------------------
//		Curve Editor
//		�\�[�X�t�@�C�� (���C��)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "ce_header.hpp"



//---------------------------------------------------------------------
//		�O���[�o���ϐ�
//---------------------------------------------------------------------
ce::Curve						g_curve_value;
ce::Curve						g_curve_value_previous;
ce::Curve						g_curve_id[CE_CURVE_MAX];
ce::Curve						g_curve_id_previous;
ce::Config						g_config;
ce::Window						g_window_main,
								g_window_header,
								g_window_editor,
								g_window_preset;
ce::Graph_View_Info				g_view_info;
FILTER*							g_fp;
std::vector<ce::Preset>			g_presets;

//�_�[�N�e�[�}
const ce::Theme g_theme_dark = {
	RGB(27, 27, 27),
	RGB(27, 27, 27),
	RGB(240, 240, 240),
	RGB(103, 103, 241),
	RGB(57, 57, 57),
	RGB(243, 243, 243),
	RGB(176, 198, 203),
	RGB(68, 68, 68),
	RGB(240, 240, 240),
	RGB(35, 35, 35),
};
//���C�g�e�[�}
const ce::Theme g_theme_light = {
	RGB(244, 244, 244),
	RGB(230, 230, 230),
	RGB(70, 70, 70),
	RGB(103, 103, 241),
	RGB(205, 205, 205),
	RGB(70, 70, 70),
	RGB(176, 198, 203),
	RGB(215, 215, 215),
	RGB(70, 70, 70),
	RGB(70, 70, 70)
};

const ce::Theme g_theme[2] = {
	g_theme_dark,
	g_theme_light,
};

//Direct2D
ID2D1Factory*					g_d2d1_factory;
ID2D1DCRenderTarget*			g_render_target;



//---------------------------------------------------------------------
//		�t�B���^�\���̒�`
//---------------------------------------------------------------------
FILTER_DLL g_filter = {
	FILTER_FLAG_ALWAYS_ACTIVE |
	FILTER_FLAG_WINDOW_SIZE |
	FILTER_FLAG_WINDOW_THICKFRAME |
	FILTER_FLAG_DISP_FILTER |
	FILTER_FLAG_EX_INFORMATION,
	CE_DEF_W,
	CE_DEF_H,
	CE_PLUGIN_NAME,
	NULL,NULL,NULL,
	NULL,NULL,
	NULL,NULL,NULL,
	NULL,
	filter_initialize,
	filter_exit,
	NULL,
	filter_wndproc,
	NULL,NULL,
	NULL,
	NULL,
	CE_PLUGIN_INFO,
	NULL,NULL,
	NULL,NULL,NULL,NULL,
	NULL,
	on_project_load,
	on_project_save,
};