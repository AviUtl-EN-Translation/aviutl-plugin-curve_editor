//----------------------------------------------------------------------------------
//		Curve Editor
//		�\�[�X�t�@�C�� (Lua�p)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"



//---------------------------------------------------------------------
//		�����֐�
//---------------------------------------------------------------------
int get_result(lua_State* L)
{
	// mode:	�ҏW���[�h
	// index:	�J�[�u�̐��l�܂���ID
	// ratio:	�i��(0~1)
	// st:		�g���b�N�o�[�ł̊J�n���̒l
	// ed:		�g���b�N�o�[�ł̏I�����̒l
	int		mode = lua_tointeger(L, 1);
	int		num = lua_tointeger(L, 2);
	double	ratio = lua_tonumber(L, 3);
	double	st = lua_tonumber(L, 4);
	double	ed = lua_tonumber(L, 5);
	double	result;

	switch ((cve::Edit_Mode)mode) {
	case cve::Mode_Bezier:
		result = g_curve_bezier.create_result(num, ratio, st, ed);
		break;

	case cve::Mode_Bezier_Multi:
		result = g_curve_bezier_multi[std::clamp(num, 1, CVE_CURVE_MAX) - 1].create_result(ratio, st, ed);
		break;

	case cve::Mode_Bezier_Value:
		//result = g_curve_bezier_value[std::clamp(num, 1, CVE_CURVE_MAX) - 1].create_result(ratio, st, ed);
		result = (ed - st) * ratio + st;
		break;

	case cve::Mode_Elastic:
		result = g_curve_elastic.create_result(num, ratio, st, ed);
		break;

	case cve::Mode_Bounce:
		result = g_curve_bounce.create_result(num, ratio, st, ed);
		break;

	default:
		result = 0;
		break;
	}

	lua_pushnumber(L, result);
	return 1;
}



//---------------------------------------------------------------------
//		�֐��e�[�u��
//---------------------------------------------------------------------
static luaL_Reg functions[] = {
	{ "getcurve", get_result },
	{ nullptr, nullptr }
};



//---------------------------------------------------------------------
//		�o�^
//---------------------------------------------------------------------
extern "C" __declspec(dllexport) int luaopen_curve_editor(lua_State * L)
{
	luaL_register(L, "curve_editor", functions);
	return 1;
}