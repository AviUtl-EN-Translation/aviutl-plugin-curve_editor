//----------------------------------------------------------------------------------
//		Curve Editor
//		�w�b�_�t�@�C��
//		(Visual C++ 2022)
//----------------------------------------------------------------------------------


#include <windows.h>
#include <windowsx.h>
#include <wininet.h>
#include <d2d1.h>
#include <lua.hpp>
#include <commctrl.h>
#include <fstream>
#include <regex>
#include <filter.h>
#include "ce_macro.hpp"
#include "resource.h"


//----------------------------------------------------------------------------------
//		typedef
//----------------------------------------------------------------------------------
typedef struct tagDoublePoint {
	double x, y;
} DoublePoint;

namespace ce {
	typedef struct tagTheme {
		COLORREF
			bg,
			bg_window,
			gr_bg,
			curve,
			handle,
			bt_apply,
			bt_tx,
			bt_other;
		COLORREF reserved[10];
	} Theme;

	typedef struct tagWindow {
		HWND
			base,
			main,
			side,
			editor,
			graph,
			prev;
	} Window;

	typedef enum tagPoint_Position {
		CONTROLPOINT_NULL,
		CONTROLPOINT_CENTER,
		CONTROLPOINT_LEFT,
		CONTROLPOINT_RIGHT
	} Point_Position;

	typedef struct tagPoint_Address {
		int index;
		Point_Position position;
	} Point_Address;

	typedef struct tagPoints_ID {
		POINT
			pt_center,
			pt_left,
			pt_right;
		int type;							//[�^�C�v]0:��������_�̍�; 1:��������_�̉E; 2:�g������_;
	} Points_ID;

	typedef struct tagConfig {
		BOOL
			alert,
			trace,
			auto_copy,
			mode,
			show_handle;
		int
			theme,
			separator,
			id_current,
			align_mode;
	} Config;

	typedef struct tagGr_Disp_Info {
		DoublePoint o;
		DoublePoint scale;
	} Gr_Disp_Info;
}


//----------------------------------------------------------------------------------
//		�N���X
//----------------------------------------------------------------------------------
namespace ce {
	class Curve_Value {
	public:
		POINT control_point[2];

		Curve_Value()
		{
			control_point[0] = { 400, 400 };
			control_point[1] = { 600, 600 };
		}

		int point_in_control_points(POINT cl_pt);
		void move_point(int index, POINT gr_pt);
	};

	class Curve_ID {
	public:
		int size = sizeof(Points_ID) * CE_POINT_MAX;
		std::vector<Points_ID> control_points;

		Curve_ID()
		{
			Points_ID pt_add[2];
			pt_add[0].type = 0;
			pt_add[0].pt_center = { 0, 0 };
			pt_add[0].pt_right = { (int)(CE_GR_RES * 0.4), (int)(CE_GR_RES * 0.4) };
			pt_add[0].pt_left = { 0, 0 };
			control_points.emplace_back(pt_add[0]);

			pt_add[1].type = 1;
			pt_add[1].pt_center = { CE_GR_RES, CE_GR_RES };
			pt_add[1].pt_left = { (int)(CE_GR_RES * 0.6), (int)(CE_GR_RES * 0.6) };
			pt_add[1].pt_right = { CE_GR_RES, CE_GR_RES };
			control_points.emplace_back(pt_add[1]);
		}

		void				add_point(POINT cl_pt);
		void				delete_point(POINT cl_pt);
		POINT				get_point(Point_Address address);
		void				move_point(Point_Address address, POINT gr_pt, BOOL bReset);
		Point_Address		pt_in_control_points(POINT cl_pt);
		double				get_handle_angle(Point_Address address);
		void				correct_handle(Point_Address address, double angle);
		void				set_handle_angle(Point_Address address, double angle, BOOL bLength, double lgth);
		void				reverse_points();
		double				get_value(double ratio, double st, double ed);
	};

	class Control {
		int flag;
		LPCTSTR icon;
		COLORREF& color;
		static LRESULT CALLBACK message_router(HWND, UINT, WPARAM, LPARAM);
	public:
		HWND hwnd;
		int id;

		Control(COLORREF& col) : color{ col } {}

		BOOL				create(HWND hwnd_p, WNDPROC exWndProc, LPSTR name, LPCTSTR icon, int ct_id, int x, int y, int w, int h, int fl);
		void				move(int x, int y, int w, int h);
		LRESULT CALLBACK	wndproc(HWND, UINT, WPARAM, LPARAM);
	};

	//�v���Z�b�g
	typedef struct tagPreset_Value {
		LPCTSTR name;
		POINT ct1, ct2;
	} Preset_Value;

	typedef struct Preset_ID {
		LPCTSTR name;
		Curve_ID curve;
	} Preset_ID;
}


//----------------------------------------------------------------------------------
//		extern�錾
//----------------------------------------------------------------------------------
extern ce::Curve_Value						g_curve_value;
extern ce::Curve_ID							g_curve_id[CE_CURVE_MAX];
extern std::vector<ce::Preset_Value>		g_presets_value;
extern std::vector<ce::Preset_ID>			g_presets_id;
extern const ce::Theme						g_theme_dark, g_theme_light;
extern ce::Theme							g_theme[2];
extern ce::Config							g_config;
extern ce::Window							g_window;
extern ce::Gr_Disp_Info						g_disp_info;
extern FILTER*								g_fp;
extern FILTER_DLL							g_filter;
//Direct2D
extern ID2D1Factory*						g_d2d1_factory;
extern ID2D1DCRenderTarget*					g_render_target;


//----------------------------------------------------------------------------------
//		�֐�
//----------------------------------------------------------------------------------

//�t�B���^�\���̂̃|�C���^��n��
EXTERN_C FILTER_DLL __declspec(dllexport)* __stdcall GetFilterTable(void);

//������
BOOL				initialize(FILTER*);

//�I��
BOOL				exit(FILTER*);

void				ini_load_configs(FILTER* fp);

//1�����J�[�uID���󂯎�萧��_��ύX
void				read_value(int);
//������̕���
std::vector<std::string> split(const std::string& s, TCHAR c);
//���݂̐���_�̍��W����4�����J�[�uID(������)�𐶐�
std::string			create_value_4d();
//���݂̐���_�̍��W����1�����J�[�uID(���l)�𐶐�
int					create_value_1d();
//�N���b�v�{�[�h�Ƀe�L�X�g���R�s�[
BOOL				copy_to_clipboard(HWND, LPCTSTR);
//�q�E�B���h�E���쐬
HWND				create_child(HWND hwnd, WNDPROC wndproc, LPSTR name, LONG style, int x, int y, int width, int height);


//�`��
void				d2d_setup(HDC hdc, LPRECT rect_wnd, COLORREF cr);
void				d2d_draw_bezier(ID2D1SolidColorBrush* pBrush,
					DoublePoint stpt, DoublePoint ctpt1, DoublePoint ctpt2, DoublePoint edpt, float thickness);
void				d2d_draw_handle(ID2D1SolidColorBrush* pBrush, DoublePoint stpt, DoublePoint edpt);
void				draw_main(HWND hwnd, HDC hdc_mem, LPRECT rect_wnd);
void				draw_side(HWND hwnd, HDC hdc_mem, LPRECT rect_wnd);
void				draw_panel_library(HWND hwnd, HDC hdc_mem, LPRECT rect_wnd);
void				draw_panel_editor(HWND hwnd, HDC hdc_mem, LPRECT rect_wnd);
void				draw_panel_graph(HWND hwnd, HDC hdc_mem, POINT* pt_trace, LPRECT rect_wnd);


//�_�C�A���O�v���V�[�W��
BOOL CALLBACK		wndproc_daialog_settings(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK		wndproc_daialog_value(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK		wndproc_daialog_read(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK		wndproc_daialog_save(HWND, UINT, WPARAM, LPARAM);


//�E�B���h�E�v���V�[�W��
BOOL				wndproc_base(HWND, UINT, WPARAM, LPARAM, void*, FILTER*);
LRESULT CALLBACK	wndproc_main(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	wndproc_editor(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	wndproc_side(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	wndproc_graph(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	wndproc_library(HWND, UINT, WPARAM, LPARAM);

//�R���g���[���v���V�[�W��
LRESULT CALLBACK	wndproc_control_apply(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	wndproc_control_value(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	wndproc_control_save(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	wndproc_control_read(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	wndproc_control_mode_value(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	wndproc_control_mode_id(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	wndproc_control_id(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	wndproc_control_align(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	wndproc_control_id_delete(HWND, UINT, WPARAM, LPARAM);


//�o�O����̃e�X�g�p
LRESULT CALLBACK	WndProc_Test(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);


//----------------------------------------------------------------------------------
//		�C�����C���֐�
//----------------------------------------------------------------------------------

//�O���t -> �N���C�A���g
inline DoublePoint to_client(int gr_x, int gr_y)
{
	return {
		g_disp_info.o.x + gr_x * g_disp_info.scale.x,
		g_disp_info.o.y - gr_y * g_disp_info.scale.y
	};
}

inline DoublePoint to_client(POINT gr_pt)
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
		(int)((cl_x - g_disp_info.o.x) / g_disp_info.scale.x),
		(int)((-cl_y + g_disp_info.o.y) / g_disp_info.scale.y)
	};
}

inline POINT to_graph(POINT cl_pt)
{
	return {
		to_graph(cl_pt.x, cl_pt.y).x,
		to_graph(cl_pt.x, cl_pt.y).y
	};
}
