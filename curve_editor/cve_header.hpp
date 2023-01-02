//----------------------------------------------------------------------------------
//		Curve Editor
//		�w�b�_�t�@�C��
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#pragma once

#include "cve_preset.hpp"



//----------------------------------------------------------------------------------
//		�t�B���^�֐�
//----------------------------------------------------------------------------------

//�t�B���^�\���̂̃|�C���^��n��
EXTERN_C FILTER_DLL __declspec(dllexport)* __stdcall GetFilterTable(void);

//�������E�I��
BOOL							filter_init(FILTER*);
BOOL							filter_exit(FILTER*);

// �v���W�F�N�g�ǂݍ��݁E�����o�����ɌĂяo�����֐�
BOOL							on_project_load(FILTER* fp, void* editp, void* data, int size);
BOOL							on_project_save(FILTER* fp, void* editp, void* data, int* size);

// �E�B���h�E�v���V�[�W��
BOOL							filter_wndproc(HWND, UINT, WPARAM, LPARAM, void*, FILTER*);



//----------------------------------------------------------------------------------
//		���̑��̊֐�
//----------------------------------------------------------------------------------

namespace cve {
	// Direct2D������
	void						d2d_init();

	//������̕���
	std::vector<std::string>	split(const std::string& s, TCHAR c);

	//�N���b�v�{�[�h�Ƀe�L�X�g���R�s�[
	BOOL						copy_to_clipboard(HWND, LPCTSTR);

	// ���������Z
	void						subtract_length(cve::Float_Point* pt, const cve::Float_Point& st, const cve::Float_Point& ed, float length);

	// �ݒ�����j���[�ɔ��f
	void						apply_config_to_menu(HMENU menu, MENUITEMINFO* mi);

	// �J�[�u���g���[�X
	void						trace_curve();

	// �L�[�������̏���
	LRESULT						on_keydown(WPARAM wparam);

	// �o�[�W�������`�F�b�N
	DWORD WINAPI				check_version(LPVOID param);
}



//----------------------------------------------------------------------------------
//		�E�B���h�E�v���V�[�W��
//----------------------------------------------------------------------------------
LRESULT CALLBACK				wndproc_main(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK				wndproc_menu(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK				wndproc_editor(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK				wndproc_library(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK				wndproc_preset_list(HWND, UINT, WPARAM, LPARAM);



//----------------------------------------------------------------------------------
//		�_�C�A���O�v���V�[�W��
//----------------------------------------------------------------------------------
BOOL CALLBACK					dialogproc_config(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK					dialogproc_bezier_param(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK					dialogproc_read(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK					dialogproc_save(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK					dialogproc_id(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK					dialogproc_check_update(HWND, UINT, WPARAM, LPARAM);



//----------------------------------------------------------------------------------
//		�t�b�N
//----------------------------------------------------------------------------------
BOOL WINAPI						TrackPopupMenu_hooked(HMENU menu, UINT flags, int x, int y, int reserved, HWND hwnd, const RECT* rect);
INT_PTR WINAPI					DialogBox_hooked(HINSTANCE hinstance, LPCSTR template_name, HWND hwnd_parent, DLGPROC dlgproc, LPARAM init_param);
BOOL CALLBACK					dialogproc_hooked(HWND, UINT, WPARAM, LPARAM);



//----------------------------------------------------------------------------------
//		�C�����C���֐�
//----------------------------------------------------------------------------------

//�O���t -> �N���C�A���g
inline cve::Float_Point to_client(int gr_x, int gr_y)
{
	return {
		g_view_info.origin.x + (float)(gr_x * g_view_info.scale.x),
		g_view_info.origin.y - (float)(gr_y * g_view_info.scale.y)
	};
}

inline cve::Float_Point to_clientf(float gr_x, float gr_y)
{
	return {
		g_view_info.origin.x + (float)(gr_x * g_view_info.scale.x),
		g_view_info.origin.y - (float)(gr_y * g_view_info.scale.y)
	};
}

inline cve::Float_Point to_client(const POINT& pt_graph)
{
	return {
		to_client(pt_graph.x, pt_graph.y).x,
		to_client(pt_graph.x, pt_graph.y).y
	};
}

inline cve::Float_Point to_client(const cve::Float_Point& pt_graph)
{
	return {
		to_clientf(pt_graph.x, pt_graph.y).x,
		to_clientf(pt_graph.x, pt_graph.y).y
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

inline cve::Float_Point to_graphf(double cl_x, double cl_y)
{
	return {
		(float)((cl_x - g_view_info.origin.x) / g_view_info.scale.x),
		(float)((-cl_y + g_view_info.origin.y) / g_view_info.scale.y)
	};
}

inline POINT to_graph(const POINT& pt_client)
{
	return {
		to_graph(pt_client.x, pt_client.y).x,
		to_graph(pt_client.x, pt_client.y).y
	};
}

//�O���t -> �v���Z�b�g
inline cve::Float_Point to_preset(int gr_x, int gr_y)
{
	return {
		(float)CVE_MARGIN_PRESET + (gr_x / (float)CVE_GRAPH_RESOLUTION) * (g_config.preset_size - CVE_MARGIN_PRESET * 2),
		g_config.preset_size - (float)CVE_MARGIN_PRESET - (gr_y / (float)CVE_GRAPH_RESOLUTION) * (g_config.preset_size - CVE_MARGIN_PRESET * 2)
	};
}

inline cve::Float_Point to_preset(const POINT& pt_graph)
{
	return {
		to_preset(pt_graph.x, pt_graph.y).x,
		to_preset(pt_graph.x, pt_graph.y).y
	};
}