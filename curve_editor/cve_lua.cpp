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

	/*switch (mode) {
	case cve::Mode_Normal:
	{

		break;
	}
		
	}*/


	if (num < 0 || num > CVE_CURVE_MAX) lua_pushnumber(L, st + (ed - st) * ratio);
	else lua_pushnumber(L, g_curve_mb[num].create_result(ratio, st, ed));
	return 1;
}



//---------------------------------------------------------------------
//		�֐��e�[�u��
//---------------------------------------------------------------------
static luaL_Reg functions[] = {
	{ "get_result", get_result },
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