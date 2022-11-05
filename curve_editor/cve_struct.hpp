//----------------------------------------------------------------------------------
//		Curve Editor
//		�w�b�_�t�@�C�� (�\����)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#pragma once

#include "cve_framework.hpp"



namespace cve {
	//---------------------------------------------------------------------
	//		Float Point (float�̓_)
	//---------------------------------------------------------------------
	typedef struct tagFloat_Point {
		float x, y;
	} Float_Point;



	//---------------------------------------------------------------------
	//		Double Point (double�̓_)
	//---------------------------------------------------------------------
	typedef struct tagDouble_Point {
		double x, y;
	} Double_Point;



	//---------------------------------------------------------------------
	//		�G�f�B�b�g���[�h
	//---------------------------------------------------------------------
	enum Edit_Mode {
		Mode_Normal,
		Mode_Multibezier,
		Mode_Elastic,
		Mode_Bounce,
		Mode_Value
	};



	//---------------------------------------------------------------------
	//		�e�[�}
	//---------------------------------------------------------------------
	typedef struct tagTheme {
		COLORREF	bg,
					bg_graph,
					sepr,
					curve_trace,
					curve_prset,
					handle,
					handle_preset,
					bt_selected,
					bt_unselected,
					bt_tx,
					bt_tx_selected,
					preset_tx;
	} Theme;



	//---------------------------------------------------------------------
	//		�|�C���g(�}���`�x�W�F���[�h)
	//---------------------------------------------------------------------
	typedef struct tagCurve_Points {
		enum Type {
			Default_Left,		// ��������_�̍�
			Default_Right,		// ��������_�̉E
			Extended			// �g������_
		};

		POINT	pt_center,
				pt_left,
				pt_right;

		Type type;
	} Curve_Points;



	//---------------------------------------------------------------------
	//		�|�C���g�̏ꏊ
	//---------------------------------------------------------------------
	typedef struct tagPoint_Address {
		// �|�C���g�̈ʒu
		enum Point_Position {
			Null,
			Center,
			Left,
			Right
		};

		int index;
		Point_Position position;
	} Point_Address;



	//---------------------------------------------------------------------
	//		�ݒ�
	//---------------------------------------------------------------------
	typedef struct tagConfig {
		enum Layout_Mode {
			Vertical,
			Horizontal
		};

		bool	alert,
				trace,
				auto_copy,
				show_handle,
				align_handle,
				is_hooked_popup,
				is_hooked_dialog;

		int		theme,
				separator,
				current_id,
				preset_size;

		Edit_Mode	edit_mode;
		Layout_Mode	layout_mode;
		COLORREF	curve_color;
	} Config;
}
