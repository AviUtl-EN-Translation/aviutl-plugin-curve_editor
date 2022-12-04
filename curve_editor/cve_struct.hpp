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
	struct Float_Point {
		float x, y;
	};



	//---------------------------------------------------------------------
	//		Double Point (double�̓_)
	//---------------------------------------------------------------------
	struct Double_Point {
		double x, y;
	};



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
	struct Theme {
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
	};



	//---------------------------------------------------------------------
	//		�|�C���g(�}���`�x�W�F���[�h)
	//---------------------------------------------------------------------
	struct Curve_Points {
		enum Type {
			Default_Left,		// ��������_�̍�
			Default_Right,		// ��������_�̉E
			Extended			// �g������_
		};

		POINT	pt_center,
				pt_left,
				pt_right;

		Type type;
	};



	//---------------------------------------------------------------------
	//		�|�C���g�̏ꏊ
	//---------------------------------------------------------------------
	struct Point_Address {
		// �|�C���g�̈ʒu
		enum Point_Position {
			Null,
			Center,
			Left,
			Right
		};

		int index;
		Point_Position position;
	};



	//---------------------------------------------------------------------
	//		�ݒ�
	//---------------------------------------------------------------------
	struct Config {
		enum Layout_Mode {
			Vertical,
			Horizontal
		};

		struct Curve_ID {
			int multibezier = 0,
				value = 0;
		};

		bool	alert,
				trace,
				auto_copy,
				show_handle,
				align_handle,
				is_hooked_popup,
				is_hooked_dialog,
				notify_latest_version;

		int		theme,
				separator,
				preset_size;

		Curve_ID	current_id;
		Edit_Mode	edit_mode;
		Layout_Mode	layout_mode;
		COLORREF	curve_color;
	};
}
