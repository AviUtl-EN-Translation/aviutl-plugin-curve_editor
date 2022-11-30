//----------------------------------------------------------------------------------
//		Curve Editor
//		�w�b�_�t�@�C��
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#pragma once

#include "cve_class.hpp"



//---------------------------------------------------------------------
//		�v���Z�b�g
//---------------------------------------------------------------------
namespace cve {
	template <typename Curve_Class>
	class Preset : public Button {
	protected:
		virtual LRESULT		wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	public:
		char				name[CVE_PRESET_NAME_MAX];
		Curve_Class			curve;
		time_t				unix_time;

		Preset();

		BOOL				initialize(HWND hwnd_p, const Curve_Class& cv, LPTSTR nm);

		void				move(int panel_width, int index);
	};
}



//----------------------------------------------------------------------------------
//		extern�錾
//----------------------------------------------------------------------------------
extern cve::Curve_Normal					g_curve_normal,
											g_curve_normal_previous;
extern cve::Curve_Multibezier				g_curve_mb[CVE_CURVE_MAX],
											g_curve_mb_previous;
extern cve::Curve_Elastic					g_curve_elastic,
											g_curve_elastic_previous;
extern cve::Curve_Bounce					g_curve_bounce,
											g_curve_bounce_previous;
extern cve::Curve_Value						g_curve_value[CVE_CURVE_MAX],
											g_curve_value_previous;

extern cve::Static_Array<cve::Preset<cve::Curve_Normal>, CVE_PRESET_NUM_DEFAULT> g_presets_normal_default;
extern cve::Static_Array<cve::Preset<cve::Curve_Normal>, CVE_PRESET_NUM_CUSTOM> g_presets_normal_custom;

extern cve::Static_Array<cve::Preset<cve::Curve_Multibezier>, CVE_PRESET_NUM_DEFAULT> g_presets_mb_default;
extern cve::Static_Array<cve::Preset<cve::Curve_Multibezier>, CVE_PRESET_NUM_CUSTOM> g_presets_mb_custom;

extern cve::Static_Array<cve::Preset<cve::Curve_Elastic>, CVE_PRESET_NUM_CUSTOM> g_presets_elastic_custom;

extern cve::Static_Array<cve::Preset<cve::Curve_Bounce>, CVE_PRESET_NUM_CUSTOM> g_presets_bounce_custom;

extern const cve::Theme						g_theme[2];

extern cve::Config							g_config;

extern cve::Window							g_window_main,
											g_window_header,
											g_window_editor,
											g_window_library,
											g_window_preset_list;

extern cve::Graph_View_Info					g_view_info;

extern FILTER*								g_fp;
extern FILTER_DLL							g_filter;
//Direct2D
extern ID2D1Factory*						g_d2d1_factory;
extern ID2D1DCRenderTarget*					g_render_target;

//�t�b�N
extern BOOL									(WINAPI* TrackPopupMenu_original)(HMENU, UINT, int, int, int, HWND, const RECT*);
extern INT_PTR								(WINAPI* DialogBox_original)(HINSTANCE, LPCSTR, HWND, DLGPROC, LPARAM);
extern INT_PTR								(WINAPI* dialogproc_original)(HWND, UINT, WPARAM, LPARAM);



//---------------------------------------------------------------------
//		�R���X�g���N�^
//---------------------------------------------------------------------
template <typename Curve_Class>
cve::Preset<Curve_Class>::Preset()
{
	::time(&unix_time);

	bool is_id_available;

	int count = 0;

	// ID���J�E���g�A�b�v���Ă������p�\��ID��T��
	while (true) {
		is_id_available = true;

		// �����̃v���Z�b�g��ID������
		for (int i = 0; i < (int)g_presets_normal_custom.size; i++) {
			if (count == g_presets_normal_custom[i].id) {
				is_id_available = false;
				break;
			}
		}

		if (is_id_available)
			break;

		count++;
	}

	id = count;
}



//---------------------------------------------------------------------
//		�v���Z�b�g(�E�B���h�E)���쐬
//---------------------------------------------------------------------
template <typename Curve_Class>
BOOL cve::Preset<Curve_Class>::initialize(HWND hwnd_p, const Curve_Class& cv, LPTSTR nm)
{
	strcpy_s(name, CVE_PRESET_NAME_MAX, nm);
	std::string class_name = "cve_preset_" + std::to_string(id);
	hwnd_parent = hwnd_p;
	content_type = Button::String;

	RECT rect = {
		0, 0,
		g_config.preset_size,
		g_config.preset_size
	};

	edge_flag = CVE_EDGE_ALL;

	curve = cv;

	return create(
		hwnd_p,
		(LPTSTR)class_name.c_str(),
		wndproc_static,
		NULL,
		NULL,
		rect,
		this
	);
}



//---------------------------------------------------------------------
//		�v���Z�b�g���ړ�
//---------------------------------------------------------------------
template <typename Curve_Class>
void cve::Preset<Curve_Class>::move(int panel_width, int index)
{
	const POINT width = {
		CVE_MARGIN + g_config.preset_size,
		CVE_MARGIN + g_config.preset_size + CVE_PRESET_TITLE_HEIGHT
	};

	int x_count = index % (panel_width / width.x);
	int y_count = index / (panel_width / width.x);

	::MoveWindow(
		hwnd,
		width.x * x_count + CVE_MARGIN / 2,
		width.y * y_count + CVE_MARGIN / 2,
		g_config.preset_size,
		g_config.preset_size + CVE_PRESET_TITLE_HEIGHT,
		TRUE
	);
}



//---------------------------------------------------------------------
//		�E�B���h�E�v���V�[�W��(static�ϐ��g�p�s��)
//---------------------------------------------------------------------
template <typename Curve_Class>
LRESULT cve::Preset<Curve_Class>::wndproc(HWND hw, UINT msg, WPARAM wparam, LPARAM lparam)
{
	RECT rect_wnd;

	::GetClientRect(hwnd, &rect_wnd);

	RECT rect_title = {
		0,
		g_config.preset_size,
		rect_wnd.right,
		rect_wnd.bottom
	};


	switch (msg) {
	case WM_CREATE:
		bitmap_buffer.init(hw);
		bitmap_buffer.set_size(rect_wnd);

		font = ::CreateFont(
			14, 0,
			0, 0,
			FW_REGULAR,
			FALSE, FALSE, FALSE,
			SHIFTJIS_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			NULL,
			CVE_FONT_REGULAR
		);

		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = hw;
		return 0;

	case WM_PAINT:
	{
		COLORREF bg = g_theme[g_config.theme].bg;

		::SetTextColor(bitmap_buffer.hdc_memory, g_theme[g_config.theme].preset_tx);

		draw_content(bg, &rect_title, strcmp(name, "") == 0 ? "(noname)" : name, true);

		if (g_render_target != nullptr && g_d2d1_factory != nullptr) {
			g_render_target->BeginDraw();

			curve.draw_curve(&bitmap_buffer, rect_wnd, CVE_DRAW_CURVE_PRESET);

			g_render_target->EndDraw();
		}

		draw_edge();
		bitmap_buffer.transfer();

		return 0;
	}

	default:
		return Button::wndproc(hw, msg, wparam, lparam);
	}
}



//----------------------------------------------------------------------------------
//		�t�B���^�֐�
//----------------------------------------------------------------------------------

//�t�B���^�\���̂̃|�C���^��n��
EXTERN_C FILTER_DLL __declspec(dllexport)* __stdcall GetFilterTable(void);

//�������E�I��
BOOL				filter_initialize(FILTER*);
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

	// �L�[�������̏���
	LRESULT						on_keydown(WPARAM wparam);
}



//----------------------------------------------------------------------------------
//		�E�B���h�E�v���V�[�W��
//----------------------------------------------------------------------------------
LRESULT CALLBACK	wndproc_main(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	wndproc_header(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	wndproc_editor(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	wndproc_library(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	wndproc_preset_list(HWND, UINT, WPARAM, LPARAM);



//----------------------------------------------------------------------------------
//		�_�C�A���O�v���V�[�W��
//----------------------------------------------------------------------------------
BOOL CALLBACK		dialogproc_config(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK		dialogproc_param_normal(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK		dialogproc_read(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK		dialogproc_save(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK		dialogproc_id(HWND, UINT, WPARAM, LPARAM);



//----------------------------------------------------------------------------------
//		�t�b�N
//----------------------------------------------------------------------------------
BOOL WINAPI			TrackPopupMenu_hooked(HMENU menu, UINT flags, int x, int y, int reserved, HWND hwnd, const RECT* rect);
INT_PTR WINAPI		DialogBox_hooked(HINSTANCE hinstance, LPCSTR template_name, HWND hwnd_parent, DLGPROC dlgproc, LPARAM init_param);
BOOL CALLBACK		dialogproc_hooked(HWND, UINT, WPARAM, LPARAM);



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

inline cve::Float_Point to_client(const POINT& pt_graph)
{
	return {
		to_client(pt_graph.x, pt_graph.y).x,
		to_client(pt_graph.x, pt_graph.y).y
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