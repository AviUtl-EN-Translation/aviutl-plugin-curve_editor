//----------------------------------------------------------------------------------
//		Curve Editor
//		�\�[�X�t�@�C��(�E�B���h�E�v���V�[�W��)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"



//---------------------------------------------------------------------
//		�E�B���h�E�v���V�[�W���i�w�b�_�p�l���j
//---------------------------------------------------------------------
LRESULT CALLBACK wndproc_header(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	RECT rect_wnd;

	static HMENU menu;
	static MENUITEMINFO minfo;

	static cve::Bitmap_Buffer bitmap_buffer;
	static cve::Button	copy,
		read,
		save,
		clear,
		fit,
		mode,
		id_back,
		id_next;

	static cve::Button_Value value;
	static cve::Button_ID id_id;

	cve::Rectangle		rect_lower_buttons,
		rect_id_buttons;

	RECT				rect_copy,
		rect_read,
		rect_save,
		rect_clear,
		rect_fit,
		rect_mode,
		rect_id_back,
		rect_id_id,
		rect_id_next;

	POINT			pt_client = {
		GET_X_LPARAM(lparam),
		GET_Y_LPARAM(lparam)
	};

	LPCTSTR str_mode[] = {
		CVE_STR_MODE_NORMAL,
		CVE_STR_MODE_MULTIBEZIER,
		CVE_STR_MODE_ELASTIC,
		CVE_STR_MODE_BOUNCE,
		CVE_STR_MODE_VALUE
	};


	::GetClientRect(hwnd, &rect_wnd);

	// �����̃{�^��������RECT
	rect_lower_buttons.set(
		CVE_MARGIN,
		CVE_MARGIN + CVE_MARGIN_CONTROL + CVE_CT_UPPER_H,
		rect_wnd.right - CVE_MARGIN,
		CVE_MARGIN + CVE_MARGIN_CONTROL + CVE_CT_LOWER_H + CVE_CT_UPPER_H
	);

	// Value/ID�X�C�b�`������RECT
	rect_mode = {
		CVE_MARGIN,
		CVE_MARGIN,
		(rect_wnd.right - CVE_MARGIN) / 2,
		CVE_MARGIN + CVE_CT_UPPER_H
	};

	// ID�̃{�^��������RECT
	rect_id_buttons.set(
		(rect_wnd.right + CVE_MARGIN) / 2,
		CVE_MARGIN,
		rect_wnd.right - CVE_MARGIN,
		CVE_MARGIN + CVE_CT_UPPER_H
	);

	LPRECT lower_buttons[] = {
		&rect_copy,
		&rect_read,
		&rect_save,
		&rect_clear,
		&rect_fit
	};

	LPRECT id_buttons[] = {
		&rect_id_back,
		&rect_id_id,
		&rect_id_next
	};

	rect_lower_buttons.divide(lower_buttons, NULL, 5);
	rect_id_buttons.divide(id_buttons, NULL, 3);

	switch (msg) {
	case WM_CLOSE:
		bitmap_buffer.exit();
		return 0;

	case WM_CREATE:
		bitmap_buffer.init(hwnd);
		bitmap_buffer.set_size(rect_wnd);

		// ���j���[
		menu = ::GetSubMenu(LoadMenu(g_fp->dll_hinst, MAKEINTRESOURCE(IDR_MENU_MODE)), 0);
		minfo.cbSize = sizeof(MENUITEMINFO);

		// ���[�h�I���{�^��
		mode.initialize(
			hwnd,
			"CTRL_MODE",
			NULL,
			cve::Button::String,
			NULL, NULL,
			str_mode[g_config.edit_mode],
			CVE_CT_EDIT_MODE,
			rect_mode,
			CVE_EDGE_ALL
		);

		// �l
		value.initialize(
			hwnd,
			"CTRL_VALUE",
			NULL,
			cve::Button::String,
			NULL, NULL,
			NULL,
			CVE_CM_VALUE,
			rect_id_buttons.rect,
			CVE_EDGE_ALL
		);

		// �O��ID�̃J�[�u�Ɉړ�
		id_back.initialize(
			hwnd,
			"CTRL_ID_BACK",
			"�O��ID�̃J�[�u�Ɉړ�",
			cve::Button::Icon,
			MAKEINTRESOURCE(IDI_BACK),
			MAKEINTRESOURCE(IDI_BACK_LIGHT),
			NULL,
			CVE_CM_ID_BACK,
			rect_id_back,
			CVE_EDGE_ALL
		);

		// ID�\���{�^��
		id_id.initialize(
			hwnd,
			"CTRL_ID_ID",
			NULL,
			cve::Button::String,
			NULL, NULL,
			NULL,
			NULL,
			rect_id_id,
			CVE_EDGE_ALL
		);

		// ����ID�̃J�[�u�Ɉړ�
		id_next.initialize(
			hwnd,
			"CTRL_ID_NEXT",
			"����ID�̃J�[�u�Ɉړ�",
			cve::Button::Icon,
			MAKEINTRESOURCE(IDI_NEXT),
			MAKEINTRESOURCE(IDI_NEXT_LIGHT),
			NULL,
			CVE_CM_ID_NEXT,
			rect_id_next,
			CVE_EDGE_ALL
		);

		// �R�s�[
		copy.initialize(
			hwnd,
			"CTRL_COPY",
			"�J�[�u�̒l���R�s�[",
			cve::Button::Icon,
			MAKEINTRESOURCE(IDI_COPY),
			MAKEINTRESOURCE(IDI_COPY_LIGHT),
			NULL,
			CVE_CM_COPY,
			rect_copy,
			CVE_EDGE_ALL
		);

		// �ǂݎ��
		read.initialize(
			hwnd,
			"CTRL_READ",
			"�J�[�u�̒l��ǂݎ��",
			cve::Button::Icon,
			MAKEINTRESOURCE(IDI_READ),
			MAKEINTRESOURCE(IDI_READ_LIGHT),
			NULL,
			CVE_CM_READ,
			rect_read,
			CVE_EDGE_ALL
		);

		// �ۑ�
		save.initialize(
			hwnd,
			"CTRL_SAVE",
			"�J�[�u���v���Z�b�g�Ƃ��ĕۑ�",
			cve::Button::Icon,
			MAKEINTRESOURCE(IDI_SAVE),
			MAKEINTRESOURCE(IDI_SAVE_LIGHT),
			NULL,
			CVE_CM_SAVE,
			rect_save,
			CVE_EDGE_ALL
		);

		// ������
		clear.initialize(
			hwnd,
			"CTRL_CLEAR",
			"�J�[�u��������",
			cve::Button::Icon,
			MAKEINTRESOURCE(IDI_CLEAR),
			MAKEINTRESOURCE(IDI_CLEAR_LIGHT),
			NULL,
			CVE_CM_CLEAR,
			rect_clear,
			CVE_EDGE_ALL
		);

		// �t�B�b�g
		fit.initialize(
			hwnd,
			"CTRL_FIT",
			"�O���t���t�B�b�g",
			cve::Button::Icon,
			MAKEINTRESOURCE(IDI_FIT),
			MAKEINTRESOURCE(IDI_FIT_LIGHT),
			NULL,
			CVE_CM_FIT,
			rect_fit,
			CVE_EDGE_ALL
		);

		if (g_config.edit_mode == cve::Mode_Multibezier) {
			value.hide();
		}
		else {
			id_id.hide();
			id_back.hide();
			id_next.hide();
		}
		return 0;

	case WM_SIZE:
		bitmap_buffer.set_size(rect_wnd);

		mode.move(rect_mode);

		value.move(rect_id_buttons.rect);

		id_back.move(rect_id_back);
		id_id.move(rect_id_id);
		id_next.move(rect_id_next);

		copy.move(rect_copy);
		read.move(rect_read);
		save.move(rect_save);
		clear.move(rect_clear);
		fit.move(rect_fit);
		return 0;

	case WM_PAINT:
		bitmap_buffer.draw_panel();
		return 0;

	case WM_COMMAND:
		switch (wparam) {
		case CVE_CM_REDRAW:
			::InvalidateRect(hwnd, NULL, FALSE);
			mode.redraw();

			value.redraw();

			id_back.redraw();
			id_id.redraw();
			id_next.redraw();

			copy.redraw();
			read.redraw();
			save.redraw();
			clear.redraw();
			fit.redraw();
			return 0;

			// �ҏW���[�h�I���{�^��
		case CVE_CT_EDIT_MODE:
		{
			minfo.fMask = MIIM_STATE;

			int edit_mode_menu_id[] = {
				ID_MODE_NORMAL,
				ID_MODE_MULTIBEZIER
			};

			// ���[�h�̃`�F�b�N
			for (int i = 0; i < sizeof(edit_mode_menu_id) / sizeof(int); i++) {
				minfo.fState = g_config.edit_mode == i ? MFS_CHECKED : MFS_UNCHECKED;
				SetMenuItemInfo(menu, edit_mode_menu_id[i], FALSE, &minfo);
			}

			::ClientToScreen(hwnd, &pt_client);
			::TrackPopupMenu(menu, TPM_LEFTALIGN | TPM_TOPALIGN, pt_client.x, pt_client.y, 0, hwnd, NULL);

			return 0;
		}

		// �W�����[�h
		case CVE_CT_EDIT_MODE_NORMAL:
		case ID_MODE_NORMAL:
			g_config.edit_mode = cve::Mode_Normal;
			::SendMessage(mode.hwnd, WM_COMMAND, CVE_CM_CHANGE_LABEL, (LPARAM)str_mode[g_config.edit_mode]);
			value.show();
			id_id.hide();
			id_back.hide();
			id_next.hide();
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_REDRAW, 0);

			return 0;

			// �}���`�x�W�F���[�h
		case CVE_CT_EDIT_MODE_MULTIBEZIER:
		case ID_MODE_MULTIBEZIER:
			g_config.edit_mode = cve::Mode_Multibezier;
			::SendMessage(mode.hwnd, WM_COMMAND, CVE_CM_CHANGE_LABEL, (LPARAM)str_mode[g_config.edit_mode]);
			value.hide();
			id_id.show();
			id_back.show();
			id_next.show();
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_REDRAW, 0);

			return 0;

			// �O��ID�̃J�[�u�Ɉړ�
		case CVE_CM_ID_BACK:
			if (g_config.edit_mode == cve::Mode_Multibezier) {
				g_config.current_id--;
				g_config.current_id = MINMAX_LIMIT(g_config.current_id, 0, CVE_CURVE_MAX - 1);
				g_curve_mb_previous = g_curve_mb[g_config.current_id];
				id_id.redraw();
				g_window_editor.redraw();
			}
			return 0;

			// ����ID�̃J�[�u�Ɉړ�
		case CVE_CM_ID_NEXT:
			if (g_config.edit_mode == cve::Mode_Multibezier) {
				g_config.current_id++;
				g_config.current_id = MINMAX_LIMIT(g_config.current_id, 0, CVE_CURVE_MAX - 1);
				g_curve_mb_previous = g_curve_mb[g_config.current_id];
				id_id.redraw();
				g_window_editor.redraw();
			}
			return 0;

		case CVE_CM_CHANGE_ID:
			if (g_config.edit_mode == cve::Mode_Multibezier) {
				g_config.current_id = MINMAX_LIMIT(lparam, 0, CVE_CURVE_MAX - 1);
				g_curve_mb_previous = g_curve_mb[g_config.current_id];
				id_id.redraw();
				g_window_editor.redraw();
			}
			return 0;

		case CVE_CM_VALUE_REDRAW:
			value.redraw();
			return 0;

		case CVE_CM_VALUE:
			::DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_PARAM), hwnd, dialogproc_param_normal);
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_REDRAW, 0);
			return 0;

		case CVE_CM_COPY:
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_COPY, 0);
			return 0;

		case CVE_CM_READ:
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_READ, 0);
			return 0;

		case CVE_CM_SAVE:
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_SAVE, 0);
			return 0;

		case CVE_CM_CLEAR:
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_CLEAR, 0);
			return 0;

		case CVE_CM_FIT:
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_FIT, 0);
			return 0;
		}
		return 0;

	default:
		return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}