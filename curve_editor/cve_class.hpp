//----------------------------------------------------------------------------------
//		Curve Editor
//		�w�b�_�t�@�C�� (�N���X)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#pragma once

#include "cve_struct.hpp"



namespace cve {
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
	//		�r�b�g�}�b�v�o�b�t�@
	//---------------------------------------------------------------------
	class Bitmap_Buffer {
	private:
		HBITMAP bitmap;
		HWND hwnd;
		RECT rect;

		void draw_grid();

	public:
		HDC hdc_memory;
		ID2D1SolidColorBrush* brush = nullptr;

		void init(HWND hw);
		void exit() const;
		void transfer() const;
		bool d2d_setup(COLORREF cr);
		void set_size(const RECT& rect_wnd);

		void draw_panel();
		void draw_panel_main(const RECT& rect_sepr);
		void draw_panel_editor();

		void draw_rounded_edge(int flag, float radius);
	};



	//---------------------------------------------------------------------
	//		�J�[�u
	//---------------------------------------------------------------------
	class Curve {
	protected:
		double				get_bezier_value(double ratio, Static_Array<Curve_Points, CVE_POINT_MAX>& points);
		void				set_handle_position(const Point_Address& pt_address, const POINT& pt_graph, double length, bool use_angle, double angle);
		void				correct_handle(const Point_Address& pt_address, double angle);
		double				get_handle_angle(const Point_Address& pt_address);
		void				set_handle_angle(const Point_Address& pt_address, double angle, bool use_length, double lgth);
		void				draw_dash_line(Bitmap_Buffer* bitmap_buffer, const RECT& rect_wnd, int pt_idx);
		void				draw_bezier(
								Bitmap_Buffer* bitmap_buffer,
								const Float_Point& stpt,
								const Float_Point& ctpt1,
								const Float_Point& ctpt2,
								const Float_Point& edpt,
								float thickness
							);
		void				draw_handle(
								Bitmap_Buffer* bitmap_buffer,
								const Float_Point& st,
								const Float_Point& ed,
								int drawing_mode,
								bool draw_point_only
							);

	public:
		// �T�C�Y�Œ�
		Static_Array<Curve_Points, CVE_POINT_MAX> ctpts;

		Curve() { initialize(); }

		// ����
		virtual void		initialize();
		void				clear();
		virtual void		pt_in_ctpt(const POINT& pt_client, Point_Address* pt_address);
		void				reverse_curve();
		virtual void		draw_curve(Bitmap_Buffer* bitmap_buffer, const RECT& rect_wnd, int drawing_mode);
		bool				is_data_valid();
		virtual void		read_number(int number, Static_Array<Curve_Points, CVE_POINT_MAX>& points);
		virtual void		read_number(int number);

		// �}���`�x�W�F���[�h�p
		virtual void		move_handle(const Point_Address& pt_address, const POINT& pt_graph, bool init);
	};



	//---------------------------------------------------------------------
	//		�J�[�u(���l�^�C�v)
	//---------------------------------------------------------------------
	class Curve_Type_Numeric : public Curve {
	public:
		int result_number;
	};



	//---------------------------------------------------------------------
	//		�J�[�u(ID�^�C�v)
	//---------------------------------------------------------------------
	class Curve_Type_ID : public Curve {
	public:
		void				add_point(const POINT& pt_client);
		void				delete_point(const POINT& pt_client);
		void				move_point(int index, const POINT& pt_graph, bool init);
	};



	//---------------------------------------------------------------------
	//		�J�[�u(�W��)
	//---------------------------------------------------------------------
	class Curve_Normal : public Curve_Type_Numeric {
	public:
		int					create_number();
		double				create_result(int number, double ratio, double st, double ed);
		std::string			create_parameters();
	};



	//---------------------------------------------------------------------
	//		�J�[�u(�}���`�x�W�F)
	//---------------------------------------------------------------------
	class Curve_Multibezier : public Curve_Type_ID {
	public:
		double				create_result(double ratio, double st, double ed);
	};



	//---------------------------------------------------------------------
	//		�J�[�u(�U��)
	//---------------------------------------------------------------------
	class Curve_Elastic : public Curve_Type_Numeric {
	private:
		double				func_elastic(double st, double ed, double f, double t);
		void				elastic_maxpoint(double st, double ed, double f, Double_Point* pt);

	public:
		double				freq;

		Curve_Elastic() { initialize(); }

		void				initialize();
		void				pt_in_ctpt(const POINT& pt_client, Point_Address* pt_address);
		void				move_handle(int pt_graph_y);
		void				draw_curve(Bitmap_Buffer* bitmap_buffer, const RECT& rect_wnd, int drawing_mode);
		int					create_number();
		void				read_number(int number);
		double				create_result(int number, double ratio, double st, double ed);
	};



	//---------------------------------------------------------------------
	//		�J�[�u(�e��)
	//---------------------------------------------------------------------
	class Curve_Bounce : public Curve_Type_Numeric {
	public:

		void				initialize();

		/*int				create_number();
		void				read_number();*/
		double				create_result(int number, double ratio, double st, double ed) { return 0; }
	};



	//---------------------------------------------------------------------
	//		�J�[�u(���l�w��)
	//---------------------------------------------------------------------
	class Curve_Value : public Curve_Type_ID {
	public:
		double				create_result(double ratio, double st, double ed) { return 0; }
	};



	//---------------------------------------------------------------------
	//		�E�B���h�E
	//---------------------------------------------------------------------
	class Window {
	protected:
		static LRESULT CALLBACK wndproc_static(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
		virtual LRESULT		wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	public:
		HWND hwnd;

		BOOL				create(
			HWND		hwnd_parent,
			LPCTSTR		class_name,
			WNDPROC		wndproc,
			LONG		window_style,
			LONG		class_style,
			const RECT& rect,
			LPVOID		lp_param
		);

		virtual void		move(const RECT& rect) const;
		void				redraw() const;
		BOOL				close() const;
		BOOL				show() const;
		BOOL				hide() const;
	};



	//---------------------------------------------------------------------
	//		�{�^��
	//---------------------------------------------------------------------
	class Button : public Window {
	public:
		enum Content_Type {
			Null,
			Icon,
			String
		};

		int					id;

		virtual BOOL		initialize(
			HWND hwnd_p,
			LPCTSTR name,
			LPCTSTR desc,
			Content_Type cont_type,
			LPCTSTR ico_res_dark,
			LPCTSTR ico_res_light,
			LPCTSTR lb,
			int ct_id,
			const RECT& rect,
			int flag
		);
		
	protected:
		LPTSTR				icon_res_dark;
		LPTSTR				icon_res_light;
		Content_Type		content_type;
		char				label[CVE_CT_LABEL_MAX];
		Bitmap_Buffer		bitmap_buffer;
		HWND				hwnd_parent;
		HWND				hwnd_tooltip;
		TOOLINFO			tool_info;
		bool				hovered, clicked;
		TRACKMOUSEEVENT		tme;
		HICON				icon_dark;
		HICON				icon_light;
		HFONT				font;
		LPTSTR				description;
		int					edge_flag;

		void				draw_content(COLORREF bg, RECT* rect_content, LPCTSTR content, bool change_color);
		void				draw_edge();
		void				set_font(int font_height, LPTSTR font_name);
		virtual LRESULT		wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	};



	//---------------------------------------------------------------------
	//		�{�^��(�X�C�b�`)
	//---------------------------------------------------------------------
	class Button_Switch : public Button {
	private:
		bool				is_selected;

		LRESULT				wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	public:
		void				set_status(BOOL bl);
	};



	//---------------------------------------------------------------------
	//		�{�^��(Value)
	//---------------------------------------------------------------------
	class Button_Param : public Button {
	private:
		LRESULT				wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	};



	//---------------------------------------------------------------------
	//		�{�^��(ID)
	//---------------------------------------------------------------------
	class Button_ID : public Button {
	private:
		int					id_buffer;
		POINT				pt_lock;
		bool				is_scrolling = false;
		int					coef_move;

		LRESULT				wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	};



	//---------------------------------------------------------------------
	//		�{�^��(�J�e�S���[)
	//---------------------------------------------------------------------
	class Button_Category : public Button {
	private:
		bool is_expanded;

		LRESULT				wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	};



	//---------------------------------------------------------------------
	//		�{�^��(�F�I��)
	//---------------------------------------------------------------------
	class Button_Color : public Button {
	private:
		COLORREF bg_color;

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
			origin.x = CVE_GRAPH_PADDING;
			scale.x = ((double)rect.right - (int)(2 * CVE_GRAPH_PADDING)) / (double)CVE_GRAPH_RESOLUTION;

			if (rect.right > rect.bottom && rect.bottom > CVE_GRAPH_PADDING * 2 + CVE_GRAPH_RESOLUTION * CVE_GRAPH_SCALE_MIN) {
				origin.y = (float)(rect.bottom - CVE_GRAPH_PADDING);
				scale.y = ((double)rect.bottom - (int)(2 * CVE_GRAPH_PADDING)) / (double)CVE_GRAPH_RESOLUTION;
			}
			else {
				origin.y = (rect.bottom + rect.right) * 0.5f - CVE_GRAPH_PADDING;
				scale.y = scale.x;
			}
		}
	};
}
