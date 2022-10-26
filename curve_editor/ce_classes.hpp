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
	//		�J�[�u
	//---------------------------------------------------------------------
	class Curve {
	private:
		void				set_handle_position(const Point_Address& pt_address, const POINT& pt_graph, double length, bool use_angle, double angle);
		void				correct_handle(const Point_Address& pt_address, double angle);
		double				get_handle_angle(const Point_Address& pt_address);
		void				set_handle_angle(const Point_Address& pt_address, double angle, bool use_length, double lgth);

	public:
		ce::Static_Array<Curve_Points, CE_POINT_MAX> ctpts;
		Mode mode;

		// ����
		void				initialize();
		void				initialize(Mode md);
		void				clear();
		void				pt_in_ctpt(const POINT& pt_client, Point_Address* pt_address);
		void				reverse_curve();

		// Value���[�h�p
		int					create_value_1d();
		std::string			create_value_4d();
		void				read_value_1d(int value);

		// ID���[�h�p
		void				add_point(const POINT& pt_client);
		void				delete_point(const POINT& pt_client);
		void				move_point(int index, const POINT& pt_graph, bool init);
		void				move_handle(const Point_Address& pt_address, const POINT& pt_graph, bool init);
		double				id_create_result(double ratio, double st, double ed);
	};



	//---------------------------------------------------------------------
	//		�r�b�g�}�b�v�L�����o�X
	//---------------------------------------------------------------------
	class Bitmap_Buffer {
	private:
		HBITMAP bitmap;
		HWND hwnd;
	public:
		HDC hdc_memory;

		void init(HWND hw);
		void exit() const;
		void transfer(const RECT& rect) const;
	};



	//---------------------------------------------------------------------
	//		�E�B���h�E
	//---------------------------------------------------------------------
	class Window {
	protected:
		static LRESULT CALLBACK wndproc_static(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	public:
		HWND hwnd;

		virtual BOOL		create(HWND hwnd_parent, LPTSTR class_name, WNDPROC wndproc, LONG style, const RECT& rect);
		virtual void		move(const RECT& rect) const;
		void				redraw() const;
		BOOL				close() const;
		BOOL				show() const;
		BOOL				hide() const;
		virtual LRESULT		wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	};



	//---------------------------------------------------------------------
	//		�v���Z�b�g
	//---------------------------------------------------------------------
	class Preset : public Window {
	public:
		HWND				hwnd;
		LPTSTR				name;
		Curve				curve;
		time_t				unix_time;
		const int			val_or_id;

		Preset(int v_i, Curve cv, LPTSTR n) : name(n), curve(cv), val_or_id(v_i)
		{
			::time(&unix_time);
		}

		BOOL				create(HWND hwnd_parent);
		void				move(int panel_width, int index);
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
		Bitmap_Buffer		bitmap_buffer;
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

		void				draw(COLORREF bg, RECT& rect_wnd, LPTSTR content);
		void				set_font(const RECT& rect_wnd, LPTSTR font_name);

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
								const RECT& rect,
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
	//		�{�^��(Value)
	//---------------------------------------------------------------------
	class Button_Value : public Button {
	public:
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

		void set(const RECT& rc);
		void set(int left, int top, int right, int bottom);
		void set_margin(int left, int top, int right, int bottom);
		void divide(LPRECT rects_child[], float weights[], int n) const;
		void client_to_screen(HWND hwnd);
		void screen_to_client(HWND hwnd);
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

		void	init(HWND hwnd) { hwnd_obj = hwnd; }
		BOOL	is_hovered() const { return id >= 0; }

		int		update(LPPOINT pt_sc, LPRECT old_rect);
		void	click();
		void	highlight() const;
		void	invalidate(const LPRECT rect) const;
	};



	//---------------------------------------------------------------------
	//		�O���t�\��
	//---------------------------------------------------------------------
	class Graph_View_Info {
	public:
		Float_Point origin;
		Double_Point scale;

		void fit(const RECT& rect)
		{
			origin.x = CE_GR_PADDING;
			scale.x = ((double)rect.right - (int)(2 * CE_GR_PADDING)) / (double)CE_GR_RESOLUTION;

			if (rect.right <= rect.bottom) {
				origin.y = (rect.bottom + rect.right) * 0.5f - CE_GR_PADDING;
				scale.y = scale.x;
			}
			else if (rect.bottom > CE_GR_PADDING * 2 + CE_GR_RESOLUTION * CE_GR_SCALE_MIN) {
				origin.y = (float)(rect.bottom - CE_GR_PADDING);
				scale.y = ((double)rect.bottom - (int)(2 * CE_GR_PADDING)) / (double)CE_GR_RESOLUTION;
			}
		}
	};
}
