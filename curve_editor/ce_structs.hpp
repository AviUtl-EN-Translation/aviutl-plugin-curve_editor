//----------------------------------------------------------------------------------
//		Curve Editor
//		�w�b�_�t�@�C�� (�\����)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#pragma once

#include "ce_framework.hpp"



namespace ce {
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
	//		�e�[�}
	//---------------------------------------------------------------------
	typedef struct tagTheme {
		COLORREF
			bg,
			bg_graph,
			sepr,
			curve,
			curve_trace,
			handle,
			bt_selected,
			bt_unselected,
			bt_tx,
			bt_tx_selected;
	} Theme;



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
	//		�|�C���g(ID���[�h)
	//---------------------------------------------------------------------
	typedef struct tagPoints_ID {
		POINT
			pt_center,
			pt_left,
			pt_right;
		int type;							//[�^�C�v]0:��������_�̍�; 1:��������_�̉E; 2:�g������_;
	} Points_ID;



	//---------------------------------------------------------------------
	//		�ݒ�
	//---------------------------------------------------------------------
	typedef struct tagConfig {
		enum Mode {
			Value,
			ID
		};

		BOOL
			alert,
			trace,
			auto_copy,
			show_handle,
			align_handle,
			is_hooked;
		int
			theme,
			separator,
			current_id,
			preset_size;
		Mode mode;
	} Config;
}
