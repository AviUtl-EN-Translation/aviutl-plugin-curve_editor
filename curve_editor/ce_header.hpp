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
			bg_wnd,
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

	typedef struct tagPoint_Address {
		int index,
			CLR;							//0:NULL; 1:����; 2:��; 3:�E;
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
			sepr,
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
		POINT ctpt[2];

		Curve_Value()
		{
			ctpt[0] = { 400, 400 };
			ctpt[1] = { 600, 600 };
		}

		int PtInCtpt(POINT cl_pt);
		void MovePoint(int index, POINT gr_pt);
	};

	class Curve_ID {
	public:
		int size = sizeof(Points_ID) * CE_POINT_MAX;
		std::vector<Points_ID> ctpts;

		Curve_ID()
		{
			Points_ID pt_add[2];
			pt_add[0].type = 0;
			pt_add[0].pt_center = { 0, 0 };
			pt_add[0].pt_right = { (int)(CE_GR_RES * 0.4), (int)(CE_GR_RES * 0.4) };
			pt_add[0].pt_left = { 0, 0 };
			ctpts.emplace_back(pt_add[0]);

			pt_add[1].type = 1;
			pt_add[1].pt_center = { CE_GR_RES, CE_GR_RES };
			pt_add[1].pt_left = { (int)(CE_GR_RES * 0.6), (int)(CE_GR_RES * 0.6) };
			pt_add[1].pt_right = { CE_GR_RES, CE_GR_RES };
			ctpts.emplace_back(pt_add[1]);
		}

		void				addPoint(POINT cl_pt);
		void				DeletePoint(POINT cl_pt);
		POINT				GetPoint(Point_Address address);
		void				MovePoint(Point_Address address, POINT gr_pt, BOOL bReset);
		Point_Address		PtInCtpts(POINT cl_pt);
		double				GetHandleAngle(Point_Address address);
		void				CorrectHandle(Point_Address address, double angle);
		void				SetHandleAngle(Point_Address address, double angle, BOOL bLength, double lgth);
		void				ReversePoints();
		double				GetValue(double ratio, double st, double ed);
	};

	class Control {
		int flag;
		LPCTSTR icon;
		COLORREF& color;
		static LRESULT CALLBACK MessageRouter(HWND, UINT, WPARAM, LPARAM);
	public:
		HWND hwnd;
		int id;

		Control(COLORREF& col) : color{ col } {}

		BOOL				Create(HWND hwnd_p, WNDPROC exWndProc, LPSTR name, LPCTSTR icon, int ct_id, int x, int y, int w, int h, int fl);
		void				Move(int x, int y, int w, int h);
		LRESULT CALLBACK	wndProc(HWND, UINT, WPARAM, LPARAM);
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
extern ce::Curve_Value						g_cv_vl;
extern ce::Curve_ID							g_cv_id[CE_CURVE_MAX];
extern std::vector<ce::Preset_Value>		g_pres_vl;
extern std::vector<ce::Preset_ID>			g_pres_id;
extern const ce::Theme						g_th_dark, g_th_light;
extern ce::Theme							g_theme[2];
extern ce::Config							g_cfg;
extern ce::Window							g_wnd;
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
BOOL				Init(FILTER*);

//�I��
BOOL				Exit(FILTER*);

//1�����J�[�uID���󂯎�萧��_��ύX
void				ReadValue(int);
//������̕���
std::vector<std::string> split(const std::string& s, TCHAR c);
//���݂̐���_�̍��W����4�����J�[�uID(������)�𐶐�
std::string			Create4DValue();
//���݂̐���_�̍��W����1�����J�[�uID(���l)�𐶐�
int					Create1DValue();
//�N���b�v�{�[�h�Ƀe�L�X�g���R�s�[
BOOL				CopyToClipboard(HWND, LPCTSTR);
//�q�E�B���h�E���쐬
HWND				CreateChild(HWND hwnd, WNDPROC wndProc, LPSTR name, LONG style, int x, int y, int width, int height);


//�`��
void				D2D1_Init(HDC hdc, LPRECT rect_wnd, COLORREF cr);
void				D2D1_DrawBezier(ID2D1SolidColorBrush* pBrush,
					DoublePoint stpt, DoublePoint ctpt1, DoublePoint ctpt2, DoublePoint edpt, float thickness);
void				D2D1_DrawHandle(ID2D1SolidColorBrush* pBrush, DoublePoint stpt, DoublePoint edpt);
void				D2D1_DrawSquare(ID2D1SolidColorBrush* pBrush, DoublePoint cl_pt);
void				DrawMain(HWND hwnd, HDC hdc_mem, LPRECT rect_wnd);
void				DrawSide(HWND hwnd, HDC hdc_mem, LPRECT rect_wnd);
void				DrawLibrary(HWND hwnd, HDC hdc_mem, LPRECT rect_wnd);
void				DrawEditor(HWND hwnd, HDC hdc_mem, LPRECT rect_wnd);
void				DrawGraph(HWND hwnd, HDC hdc_mem, POINT* pt_trace, LPRECT rect_wnd);


//�_�C�A���O�v���V�[�W��
BOOL CALLBACK		DlgProc_About(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK		DlgProc_Custom(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK		DlgProc_Pref(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK		DlgProc_Value(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK		DlgProc_Read(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK		DlgProc_Save(HWND, UINT, WPARAM, LPARAM);


//�E�B���h�E�v���V�[�W��
BOOL				WndProc_Base(HWND, UINT, WPARAM, LPARAM, void*, FILTER*);
LRESULT CALLBACK	WndProc_Main(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	WndProc_Editor(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	WndProc_Side(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	WndProc_Graph(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	WndProc_Library(HWND, UINT, WPARAM, LPARAM);

//�R���g���[���v���V�[�W��
LRESULT CALLBACK	CtrlProc_Apply(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	CtrlProc_Value(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	CtrlProc_Save(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	CtrlProc_Read(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	CtrlProc_Mode_Value(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	CtrlProc_Mode_ID(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	CtrlProc_ID_ID(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	CtrlProc_Align(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	CtrlProc_ID_Delete(HWND, UINT, WPARAM, LPARAM);


//�o�O����̃e�X�g�p
LRESULT CALLBACK	WndProc_Test(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);


//----------------------------------------------------------------------------------
//		�C�����C���֐�
//----------------------------------------------------------------------------------

//�O���t -> �N���C�A���g
inline DoublePoint ToClient(int gr_x, int gr_y)
{
	return {
		g_disp_info.o.x + gr_x * g_disp_info.scale.x,
		g_disp_info.o.y - gr_y * g_disp_info.scale.y
	};
}

inline DoublePoint ToClient(POINT gr_pt)
{
	return {
		ToClient(gr_pt.x, gr_pt.y).x,
		ToClient(gr_pt.x, gr_pt.y).y
	};
}

//�N���C�A���g -> �O���t
inline POINT ToGraph(double cl_x, double cl_y)
{
	return {
		(int)((cl_x - g_disp_info.o.x) / g_disp_info.scale.x),
		(int)((-cl_y + g_disp_info.o.y) / g_disp_info.scale.y)
	};
}

inline POINT ToGraph(POINT cl_pt)
{
	return {
		ToGraph(cl_pt.x, cl_pt.y).x,
		ToGraph(cl_pt.x, cl_pt.y).y
	};
}
