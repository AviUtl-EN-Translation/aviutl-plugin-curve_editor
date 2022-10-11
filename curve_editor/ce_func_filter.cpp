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