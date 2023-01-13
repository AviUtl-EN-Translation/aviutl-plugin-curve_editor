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

		void draw_grid();

	public:
		HDC hdc_memory;
		ID2D1SolidColorBrush* brush;
		RECT rect;

		Bitmap_Buffer();

		void init(HWND hw);
		void exit();
		void transfer() const;
		bool d2d_setup(COLORREF cr);
		void resize();

		void draw_panel();
		void draw_panel_main(const RECT& rect_sepr);

		void draw_rounded_edge(int flag, float radius);

		template <class Interface>
		void release(Interface** pp_resource)
		{
			if (*pp_resource != nullptr) {
				(*pp_resource)->Release();
				(*pp_resource) = nullptr;
			}
		}
	};



	//---------------------------------------------------------------------
	//		Direct2D�`��I�u�W�F�N�g(�p��)
	//---------------------------------------------------------------------
	class My_Direct2d_Paint_Object : public aului::Direct2d_Paint_Object {
	private:
		void draw_grid();

	public:
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
		static constexpr float HANDLE_THICKNESS					= 2.f;
		static constexpr float HANDLE_THICKNESS_PRESET			= 0.8f;
		static constexpr float HANDLE_SIZE						= 5.f;
		static constexpr float HANDLE_SIZE_PRESET				= 0.7f;
		static constexpr float HANDLE_BORDER_THICKNESS			= 2.2f;
		static constexpr float HANDLE_BORDER_THICKNESS_PRESET	= 0.5f;
		static constexpr int POINT_BOX_WIDTH					= 10;
		static constexpr float POINT_SIZE						= 4.4f;
		static constexpr float POINT_SIZE_PRESET				= 2.f;
		static constexpr float CURVE_THICKNESS					= 1.2f;

		static constexpr float DRAW_GRAPH_STEP					= 1.f;

		double				get_bezier_value(double ratio, Static_Array<Curve_Points, CVE_POINT_MAX>& points);
		void				set_handle_position(const Point_Address& pt_address, const POINT& pt_graph, double length, bool use_angle, double angle);
		void				correct_handle(const Point_Address& pt_address, double angle);

		// �n���h���̊p�x���擾&�ݒ�
		double				get_handle_angle(const Point_Address& pt_address);
		void				set_handle_angle(const Point_Address& pt_address, double angle, bool use_length, double lgth);

		// �`��n
		void				draw_dash_line(aului::Direct2d_Paint_Object* paint_object, const RECT& rect_wnd, int pt_idx);
		void				draw_bezier(
								aului::Direct2d_Paint_Object* paint_object,
								const Float_Point& stpt,
								const Float_Point& ctpt1,
								const Float_Point& ctpt2,
								const Float_Point& edpt,
								float thickness
							);
		void				draw_handle(
								aului::Direct2d_Paint_Object* paint_object,
								const Float_Point& st,
								const Float_Point& ed,
								int drawing_mode,
								int draw_option
							);

	public:
		static constexpr float HANDLE_DEFAULT_1 = 0.3f;
		static constexpr float HANDLE_DEFAULT_2 = 0.7f;
		static constexpr int DRAW_CURVE_NORMAL	= 0;
		static constexpr int DRAW_CURVE_TRACE	= 1;
		static constexpr int DRAW_CURVE_PRESET	= 2;
		static constexpr int DRAW_POINT_ONLY	= 1;
		static constexpr int DRAW_HANDLE_ONLY	= 2;

		// �T�C�Y�Œ�
		Static_Array<Curve_Points, CVE_POINT_MAX> ctpts;

		Curve() { init(); }

		// ������
		virtual void		init(Static_Array<Curve_Points, CVE_POINT_MAX>& points);
		virtual void		init() { init(ctpts); }

		// �|�C���g���폜(&������)
		void				clear(Static_Array<Curve_Points, CVE_POINT_MAX>& points);
		void				clear() { clear(ctpts); }

		virtual void		pt_on_ctpt(const POINT& pt_client, Point_Address* pt_address, bool prioritize_point=false);
		virtual void		reverse_curve();
		virtual void		draw_curve(aului::Direct2d_Paint_Object* paint_object, const RECT& rect_wnd, int drawing_mode);
		bool				is_data_valid();

		// ���l��ǂݎ��
		virtual bool		read_number(int number, Static_Array<Curve_Points, CVE_POINT_MAX>& points);
		virtual bool		read_number(int number) { return read_number(number, ctpts); }

		virtual void		move_handle(const Point_Address& pt_address, const POINT& pt_graph, bool init);
	};



	//---------------------------------------------------------------------
	//		�J�[�u(���l�^�C�v)
	//---------------------------------------------------------------------
	class Curve_Type_Numeric : public Curve {};



	//---------------------------------------------------------------------
	//		�J�[�u(ID�^�C�v)
	//---------------------------------------------------------------------
	class Curve_Type_ID : public Curve {
	protected:
		static constexpr int HANDLE_LENGTH_DEFAULT = 50;

	public:
		void				add_point(const POINT& pt_client);
		void				delete_point(const POINT& pt_client);
		void				move_point(int index, const POINT& pt_graph, bool init);
	};



	//---------------------------------------------------------------------
	//		�J�[�u(�x�W�F)
	//---------------------------------------------------------------------
	class Curve_Bezier : public Curve_Type_Numeric {
	public:
		static constexpr float MIN_Y = -2.73f;
		static constexpr float MAX_Y = 3.73f;

		int					create_number();
		double				create_result(int number, double ratio, double st, double ed);
		std::string			create_parameters();
		bool				read_parameters(LPCTSTR param, Static_Array<Curve_Points, CVE_POINT_MAX>& points);
		bool				read_parameters(LPCTSTR param) { return read_parameters(param, ctpts); }
	};



	//---------------------------------------------------------------------
	//		�J�[�u(�x�W�F(����))
	//---------------------------------------------------------------------
	class Curve_Bezier_Multi : public Curve_Type_ID {
	public:
		double				create_result(double ratio, double st, double ed);
	};



	//---------------------------------------------------------------------
	//		�J�[�u(�x�W�F(�l�w��)))
	//---------------------------------------------------------------------
	class Curve_Bezier_Value : public Curve_Type_ID {
	public:
		double				create_result(double ratio, double st, double ed) { return 0; }
	};



	//---------------------------------------------------------------------
	//		�J�[�u(�U��)
	//---------------------------------------------------------------------
	class Curve_Elastic : public Curve_Type_Numeric {
	private:
		static constexpr double DEF_FREQ		= 8.0;
		static constexpr double DEF_DECAY		= 6.0;
		static constexpr double DEF_AMP			= 1.0;

		double				func_elastic(double ratio, double f, double k, double a, double st, double ed);
		double				pt_to_param(int pt_graph_val, int idx_param);
		void				param_to_pt(POINT* pt_graph, int idx_pt);

	public:
		double				ampl;
		double				freq;
		double				dec;
		bool				reverse;

		Curve_Elastic() { init(); }

		void				init();
		void				pt_on_ctpt(const POINT& pt_client, Point_Address* pt_address);
		void				move_handle(const Point_Address pt_address, const POINT& pt_graph);
		void				draw_curve(aului::Direct2d_Paint_Object* paint_object, const RECT& rect_wnd, int drawing_mode);
		void				reverse_curve();
		int					create_number();
		bool				read_number(int number, double* f, double* k, double* a, bool* rev);
		bool				read_number(int number) { return read_number(number, &freq, &dec, &ampl, &reverse); }
		double				create_result(int number, double ratio, double st, double ed);
	};



	//---------------------------------------------------------------------
	//		�J�[�u(�o�E���X)
	//---------------------------------------------------------------------
	class Curve_Bounce : public Curve_Type_Numeric {
	private:
		static constexpr double DEF_COEF_BOUNCE	= 0.6;
		static constexpr double DEF_COEF_TIME	= 0.5;

		double				func_bounce(double ratio, double e, double t, double st, double ed);
		double				pt_to_param(const POINT& pt_graph, int idx_param);
		void				param_to_pt(POINT* pt_graph);

	public:
		double				coef_bounce;
		double				coef_time;
		bool				reverse;

		Curve_Bounce() { init(); }

		void				init();
		void				pt_on_ctpt(const POINT& pt_client, Point_Address* pt_address);
		void				move_handle(const POINT& pt_graph);
		void				draw_curve(aului::Direct2d_Paint_Object* paint_object, const RECT& rect_wnd, int drawing_mode);
		void				reverse_curve();
		int					create_number();
		bool				read_number(int number, double* e, double* t, bool* rev);
		bool				read_number(int number) { return read_number(number, &coef_bounce, &coef_time, &reverse); }
		double				create_result(int number, double ratio, double st, double ed);
	};



	//---------------------------------------------------------------------
	//		�{�^��
	//---------------------------------------------------------------------
	class Button : public aului::Window {
	public:
		static constexpr int FLAG_DISABLED = 1 << 0;
		static constexpr int FLAG_USE_ICON = 1 << 1;
		static constexpr int FLAG_EDGE_LT = 1 << 0;
		static constexpr int FLAG_EDGE_LB = 1 << 1;
		static constexpr int FLAG_EDGE_RT = 1 << 2;
		static constexpr int FLAG_EDGE_RB = 1 << 3;
		static constexpr int FLAG_EDGE_ALL = FLAG_EDGE_LT | FLAG_EDGE_LB | FLAG_EDGE_RT | FLAG_EDGE_RB;
		enum Content_Type {
			Null,
			Icon,
			String
		};

		int					id;
		int					status;

		virtual BOOL		init(
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
		void				set_status(int flags);
		
	protected:
		LPTSTR				icon_res_dark;
		LPTSTR				icon_res_light;
		Content_Type		content_type;
		char				label[CVE_CT_LABEL_MAX];
		Bitmap_Buffer		bitmap_buffer;
		HWND				hwnd_parent;
		HWND				hwnd_tooltip;
		TOOLINFO			tool_info;
		bool				hovered, clicked, disabled;
		TRACKMOUSEEVENT		tme;
		HICON				icon_dark;
		HICON				icon_light;
		HFONT				font;
		LPTSTR				text_tooltip;
		int					flag_edge;

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
	//		�G�f�B�b�g�{�b�N�X
	//---------------------------------------------------------------------
	class Edit_Box : public Button {
	private:
		int					line_height;

		LRESULT				wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	public:
		HWND				editbox;

		BOOL				init(
			HWND			hwnd_p,
			LPCTSTR			name,
			int				l_height,
			int				ct_id,
			const RECT&		rect,
			int				flag
		);
	};



	//---------------------------------------------------------------------
	//		�I�u�W�F�N�g�ݒ�_�C�A���O�̃{�^��
	//---------------------------------------------------------------------
	class Obj_Dialog_Buttons {
	private:
		HWND hwnd_obj;
		HWND hwnd_button;

		BOOL id_to_rect(int id_, aului::Window_Rectangle* rect);

	public:
		int id = -1;
		// �I�u�W�F�N�g�ݒ�_�C�A���O�̃N���C�A���g���W
		aului::Window_Rectangle rect_button;

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
		Double_Point origin;
		Double_Point scale;

		void fit(const RECT& rect)
		{
			origin.x = (double)CVE_GRAPH_PADDING;
			scale.x = ((double)rect.right - (int)(2 * CVE_GRAPH_PADDING)) / (double)CVE_GRAPH_RESOLUTION;

			if (rect.right > rect.bottom && rect.bottom > CVE_GRAPH_PADDING * 2 + CVE_GRAPH_RESOLUTION * CVE_GRAPH_SCALE_MIN) {
				origin.y = (double)(rect.bottom - CVE_GRAPH_PADDING);
				scale.y = ((double)rect.bottom - (int)(2 * CVE_GRAPH_PADDING)) / (double)CVE_GRAPH_RESOLUTION;
			}
			else {
				origin.y = (rect.bottom + rect.right) * 0.5 - CVE_GRAPH_PADDING;
				scale.y = scale.x;
			}
		}
	};
}
