//----------------------------------------------------------------------------------
//		Curve Editor
//		�\�[�X�t�@�C�� (���C��)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"



//---------------------------------------------------------------------
//		�O���[�o���ϐ�
//---------------------------------------------------------------------
cve::Curve						g_curve_normal;
cve::Curve						g_curve_normal_previous;
cve::Curve						g_curve_mb[CVE_CURVE_MAX];
cve::Curve						g_curve_mb_previous;
cve::Curve						g_curve_elastic;
cve::Curve						g_curve_elastic_previous;
cve::Curve						g_curve_bounce;
cve::Curve						g_curve_bounce_previous;
cve::Curve						g_curve_value[CVE_CURVE_MAX];
cve::Curve						g_curve_value_previous;

cve::Config						g_config;

cve::Window						g_window_main,
								g_window_header,
								g_window_editor,
								g_window_library,
								g_window_preset_list;

cve::Graph_View_Info			g_view_info;

FILTER*							g_fp;

cve::Static_Array<cve::Preset, CVE_PRESET_NUM_DEFAULT> g_presets_default;
cve::Static_Array<cve::Preset, CVE_PRESET_NUM_CUSTOM> g_presets_custom;


//�_�[�N�e�[�}
const cve::Theme g_theme_dark = {
	RGB(27, 27, 27),
	RGB(27, 27, 27),
	RGB(240, 240, 240),
	RGB(57, 57, 57),
	RGB(240, 240, 240),
	RGB(243, 243, 243),
	RGB(180, 180, 180),
	RGB(230, 230, 230),
	RGB(68, 68, 68),
	RGB(240, 240, 240),
	RGB(35, 35, 35),
	RGB(180, 180, 180)
};

//���C�g�e�[�}
const cve::Theme g_theme_light = {
	RGB(244, 244, 244),
	RGB(230, 230, 230),
	RGB(70, 70, 70),
	RGB(205, 205, 205),
	RGB(100, 100, 100),
	RGB(70, 70, 70),
	RGB(160, 160, 160),
	RGB(230, 230, 230),
	RGB(188, 188, 188),
	RGB(70, 70, 70),
	RGB(70, 70, 70),
	RGB(70, 70, 70)
};

const cve::Theme g_theme[2] = {
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
	CVE_DEF_W,
	CVE_DEF_H,
	CVE_PLUGIN_NAME,
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
	CVE_PLUGIN_INFO,
	NULL,NULL,
	NULL,NULL,NULL,NULL,
	NULL,
	on_project_load,
	on_project_save,
};