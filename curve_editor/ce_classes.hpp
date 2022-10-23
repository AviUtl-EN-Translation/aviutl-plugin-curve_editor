//----------------------------------------------------------------------------------
//		Curve Editor
//		�w�b�_�t�@�C�� (�N���X)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#pragma once

#include "ce_structs.hpp"



namespace ce {
	//---------------------------------------------------------------------
	//		�z��
	//---------------------------------------------------------------------
	template <typename T, size_t N>
	class Static_Array : public yulib::CStaticArray<T, N> {
	public:
		void PushBack(const T& v) {
			if (this->size < this->max_size) {
				this->size++;
				this->buf[this->size - 1] = v;
			}
		}
	};



	//---------------------------------------------------------------------
	//		�J�[�u(Value���[�h)
	//---------------------------------------------------------------------
	class Curve_Value {
	public:
		POINT ctpt[2];

		Curve_Value() { init(); }

		void				init();
		int					point_in_ctpts(POINT cl_pt);
		void				move_point(int index, POINT gr_pt);
		int					create_value_1d();
		std::string			create_value_4d();
		void				read_value_1d(int value);
	};



	//---------------------------------------------------------------------
	//		�J�[�u(ID���[�h)
	//---------------------------------------------------------------------
	class Curve_ID {
	public:
		ce::Static_Array<Points_ID, CE_POINT_MAX> ctpts;

		Curve_ID() { init(); }

		void				init();
		void				add_point(POINT cl_pt);
		void				delete_point(POINT cl_pt);
		void				clear();
		POINT				get_point(Point_Address address);
		void				move_point(Point_Address address, POINT gr_pt, BOOL reset);
		Point_Address		pt_in_ctpt(POINT cl_pt);
		double				get_handle_angle(Point_Address address);
		void				correct_handle(Point_Address address, double angle);
		void				set_handle_angle(Point_Address address, double angle, BOOL bLength, double lgth);
		void				reverse_curve();
		double				get_value(double ratio, double st, double ed);
	};



	//---------------------------------------------------------------------
	//		�r�b�g�}�b�v�L�����o�X
	//---------------------------------------------------------------------
	class Bitmap_Canvas {
	private:
		HBITMAP bitmap;
		HWND hwnd;
	public:
		HDC hdc_memory;

		void init(HWND hw);
		void exit();
		void transfer(LPRECT rect);
	};



	//---------------------------------------------------------------------
	//		�E�B���h�E
	//---------------------------------------------------------------------
	class Window {
	protected:
		static LRESULT CALLBACK wndproc_static(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	public:
		HWND hwnd;

		virtual BOOL		create(HWND hwnd_parent, LPTSTR class_name, WNDPROC wndproc, LONG style, LPRECT rect);
		virtual void		move(LPRECT rect);
		void				redraw();
		BOOL				close();
		virtual LRESULT		wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	};



	//---------------------------------------------------------------------
	//		�v���Z�b�g
	//---------------------------------------------------------------------
	class Preset : public Window {
	public:
		HWND				hwnd;
		LPTSTR				name;
		Curve_Value			curve_value;
		Curve_ID			curve_id;
		time_t				unix_time;
		const int			val_or_id, def_or_user;
		int					index;

		Preset(int v_i, Curve_Value c_value, Curve_ID c_id, LPTSTR n, int d_u) : name(n), val_or_id(v_i), def_or_user(d_u)
		{
			if (v_i == 1)
				curve_id = c_id;
			else
				curve_value = c_value;
			time(&unix_time);
		}

		BOOL				create(HWND hwnd_parent);
		void				move(int width);
		LRESULT				wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	};



	//---------------------------------------------------------------------
	//		�{�^��
	//---------------------------------------------------------------------
	class Button : public Window {
	protected:
		LPTSTR				icon_res_dark;
		LPTSTR				icon_res_light;
		int					icon_or_str;			//0: �A�C�R��, 1: ������
		LPTSTR				label;
		Bitmap_Canvas		canvas;
		HWND				hwnd_parent;
		HWND				hwnd_tooltip;
		TOOLINFO			tool_info;
		BOOL				hovered, clicked;
		TRACKMOUSEEVENT		tme;
		HICON				icon_dark;
		HICON				icon_light;
		HFONT				font;
		LPTSTR				description;
		int					edge_flag;
		ID2D1SolidColorBrush* brush;

		void				draw(COLORREF bg, LPRECT rect_wnd, LPTSTR content);

	public:
		int					id;

		BOOL				create(
								HWND hwnd_p,
								LPTSTR name,
								LPTSTR desc,
								int ic_or_str,
								LPTSTR ico_res_dark,
								LPTSTR ico_res_light,
								LPTSTR lb,
								int ct_id,
								LPRECT rect,
								int flag
							);
		virtual LRESULT		wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	};



	//---------------------------------------------------------------------
	//		�{�^��(�X�C�b�`)
	//---------------------------------------------------------------------
	class Button_Switch : public Button {
	private:
		BOOL				is_selected;

	public:
		void				set_status(BOOL bl);
		LRESULT				wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	};



	//---------------------------------------------------------------------
	//		�{�^��(ID)
	//---------------------------------------------------------------------
	class Button_ID : public Button {
	private:
		int					id_buffer;
		POINT				pt_lock;
		BOOL				is_scrolling = FALSE;
		int					coef_move;

	public:
		LRESULT				wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	};



	//---------------------------------------------------------------------
	//		��`
	//---------------------------------------------------------------------
	class Rectangle {
	public:
		RECT rect;

		void				set(RECT rc);
		void				set(int left, int top, int right, int bottom);
		void				set_margin(int left, int top, int right, int bottom);
		void				divide(LPRECT* rects_child, float* weights, int n);
		void				client_to_screen(HWND hwnd);
		void				screen_to_client(HWND hwnd);
	};



	//---------------------------------------------------------------------
	//		�I�u�W�F�N�g�ݒ�_�C�A���O�̃{�^��
	//---------------------------------------------------------------------
	class Obj_Dialog_Buttons {
	private:
		HWND hwnd_obj;
		HWND hwnd_button;

		BOOL id_to_rect(int id_, Rectangle* rect);

	public:
		int id = -1;
		// �I�u�W�F�N�g�ݒ�_�C�A���O�̃N���C�A���g���W
		Rectangle rect_button;

		void				init(HWND hwnd) { hwnd_obj = hwnd; }
		BOOL				is_hovered() { return id >= 0; }

		int					update(POINT pt_sc, LPRECT old_rect);
		void				click();
		void				highlight();
		void				invalidate(LPRECT rect);
	};



	//---------------------------------------------------------------------
	//		�O���t�\��
	//---------------------------------------------------------------------
	class Graph_View_Info {
	public:
		Float_Point origin;
		Double_Point scale;

		void fit(LPRECT rect)
		{
			origin.x = CE_GR_PADDING;
			scale.x = ((double)rect->right - (int)(2 * CE_GR_PADDING)) / (double)CE_GR_RESOLUTION;
			if (rect->right <= rect->bottom) {
				origin.y = (rect->bottom + rect->right) * 0.5f - CE_GR_PADDING;
				scale.y = scale.x;
			}
			else {
				origin.y = (float)(rect->bottom - CE_GR_PADDING);
				scale.y = ((double)rect->bottom - (int)(2 * CE_GR_PADDING)) / (double)CE_GR_RESOLUTION;
			}
		}
	};
}
