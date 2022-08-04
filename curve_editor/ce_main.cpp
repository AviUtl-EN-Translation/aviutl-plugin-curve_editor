//----------------------------------------------------------------------------------
//		curve_editor
//		source file (main)
//		(Visual C++ 2019)
//----------------------------------------------------------------------------------


#include "ce_header.hpp"


//---------------------------------------------------------------------
//		�O���[�o���ϐ�
//---------------------------------------------------------------------
ce::Curve_Value					g_curve_value;
ce::Curve_ID					g_curve_id[CE_CURVE_MAX];
ce::Config						g_config;
ce::Window						g_window;
ce::Gr_Disp_Info				g_disp_info;
FILTER*							g_fp;
std::vector<ce::Preset_Value>	g_presets_value;
std::vector<ce::Preset_ID>		g_presets_id;

const ce::Theme g_theme_dark = {
	RGB(27,27,27),
	RGB(27,27,27),
	RGB(27,27,27),
	RGB(103,103,241),
	RGB(243,243,243),
	RGB(243,243,243),
	RGB(27,27,27),
	RGB(60,60,60),
	NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL,
};
const ce::Theme g_theme_light = {
	RGB(32,32,32),
	RGB(50,50,50),
	RGB(45,45,45),
	RGB(259,249,249),
	RGB(64,96,222),
	RGB(71,135,228),
	RGB(238,238,238),
	RGB(60,60,60),
	NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL,
};

ce::Theme g_theme[2] = {
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
	CE_FILTER_NAME,
	NULL,NULL,NULL,
	NULL,NULL,
	NULL,NULL,NULL,
	NULL,
	Init,
	Exit,
	NULL,
	WndProc_Base,
	NULL,NULL,
	NULL,
	NULL,
	CE_FILTER_INFO,
	NULL,NULL,
	NULL,NULL,NULL,NULL,
	NULL,
};


//---------------------------------------------------------------------
//		Lua�֐�
//---------------------------------------------------------------------

//�����֐�
int getResult(lua_State* L)
{
	int		index	= lua_tointeger(L, 1);
	double	ratio	= lua_tonumber(L, 2);
	double	st		= lua_tonumber(L, 3);
	double	ed		= lua_tonumber(L, 4);

	if (index < 0 || index > CE_CURVE_MAX) lua_pushnumber(L, st + (ed - st) * ratio);
	else lua_pushnumber(L, g_curve_id[index].GetValue(ratio, st, ed));
	return 1;
}

//�֐��e�[�u��
static luaL_Reg functions[] = {
	{ "getResult", getResult },
	{ nullptr, nullptr }
};

//�o�^
extern "C" __declspec(dllexport) int luaopen_curve_editor(lua_State * L)
{
	luaL_register(L, "curve_editor", functions);
	return 1;
}