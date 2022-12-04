//----------------------------------------------------------------------------------
//		Curve Editor
//		�\�[�X�t�@�C��(�E�B���h�E�v���V�[�W��)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"



//---------------------------------------------------------------------
//		�E�B���h�E�v���V�[�W���i�G�f�B�^�p�l���j
//---------------------------------------------------------------------
LRESULT CALLBACK wndproc_editor(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//int
	static bool		move_view		= false;
	static bool		is_dragging		= false;
	// 0: NULL, 1: �ړ�, 2: �g��k��
	static int		move_or_scale = NULL;

	//double
	static double	prev_scale_x, prev_scale_y;
	static float	prev_o_x, prev_o_y;

	//POINT
	static POINT	pt_view;

	POINT			pt_client = {
		GET_X_LPARAM(lparam),
		GET_Y_LPARAM(lparam)
	};

	POINT			pt_graph;

	//RECT
	RECT			rect_wnd;

	//�n���h��
	static HMENU	menu;
	static HWND		hwnd_obj;
	static HWND		hwnd_exedit;
	static DWORD	thread_id;

	//���̑�
	static cve::Point_Address		pt_address;
	static cve::Bitmap_Buffer		bitmap_buffer;
	static cve::Obj_Dialog_Buttons	obj_buttons;
	static MENUITEMINFO				minfo;


	//�N���C�A���g�̈�̋�`���擾
	::GetClientRect(hwnd, &rect_wnd);

	//�O���t���W
	if (g_view_info.origin.x != NULL)
		pt_graph = to_graph(pt_client);


	switch (msg) {
	case WM_CLOSE:
		bitmap_buffer.exit();
		return 0;

		///////////////////////////////////////////
		//		WM_CREATE
		//		(�E�B���h�E���쐬���ꂽ�Ƃ�)
		///////////////////////////////////////////
	case WM_CREATE:
	{
		bitmap_buffer.init(hwnd);
		bitmap_buffer.set_size(rect_wnd);

		// ���j���[
		menu = ::GetSubMenu(LoadMenu(g_fp->dll_hinst, MAKEINTRESOURCE(IDR_MENU_EDITOR)), 0);

		minfo.cbSize = sizeof(MENUITEMINFO);

		// �g���ҏW�ƃI�u�W�F�N�g�ݒ�_�C�A���O�̃E�B���h�E�n���h���̎擾
		hwnd_exedit = auls::Exedit_GetWindow(g_fp);
		hwnd_obj = auls::ObjDlg_GetWindow(hwnd_exedit);

		obj_buttons.init(hwnd_obj);

		if (g_config.notify_latest_version)
			::CreateThread(NULL, 0, cve::check_version, NULL, 0, &thread_id);

		g_curve_normal_previous = g_curve_normal;
		g_curve_mb_previous = g_curve_mb[g_config.current_id.multibezier];
		g_curve_elastic_previous = g_curve_elastic;
		g_curve_bounce_previous = g_curve_bounce;
		g_curve_value_previous = g_curve_value[g_config.current_id.value];

		g_view_info.fit(rect_wnd);

		return 0;
	}

		///////////////////////////////////////////
		//		WM_SIZE
		//		(�E�B���h�E�̃T�C�Y���ύX���ꂽ�Ƃ�)
		///////////////////////////////////////////
	case WM_SIZE:
		bitmap_buffer.set_size(rect_wnd);
		return 0;

		///////////////////////////////////////////
		//		WM_PAINT
		//		(�E�B���h�E���`�悳�ꂽ�Ƃ�)
		///////////////////////////////////////////
	case WM_PAINT:
		bitmap_buffer.draw_panel_editor();
		return 0;

		///////////////////////////////////////////
		//		WM_LBUTTONDOWN
		//		(���N���b�N�����ꂽ�Ƃ�)
		///////////////////////////////////////////
	case WM_LBUTTONDOWN:

		// �ҏW���[�h�ŐU�蕪��
		switch (g_config.edit_mode) {
			// �W�����[�h�̂Ƃ�
		case cve::Mode_Normal:
			g_curve_normal.pt_in_ctpt(pt_client, &pt_address);

			// ���炩�̐���_���N���b�N���Ă����ꍇ
			if (pt_address.position != cve::Point_Address::Null) {
				g_curve_normal_previous = g_curve_normal;
				::SetCursor(LoadCursor(NULL, IDC_HAND));
				::SetCapture(hwnd);

				return 0;
			}
			break;

			// �}���`�x�W�F���[�h�̂Ƃ�
		case cve::Mode_Multibezier:
			g_curve_mb[g_config.current_id.multibezier].pt_in_ctpt(pt_client, &pt_address);

			// ���炩�̐���_���N���b�N���Ă����ꍇ
			if (pt_address.position != cve::Point_Address::Null) {
				// �n���h���̍��W���L��
				if (pt_address.position == cve::Point_Address::Center)
					g_curve_mb[g_config.current_id.multibezier].move_point(pt_address.index, pt_graph, true);
				else
					g_curve_mb[g_config.current_id.multibezier].move_handle(pt_address, pt_graph, true);

				g_curve_mb_previous = g_curve_mb[g_config.current_id.multibezier];
				::SetCursor(LoadCursor(NULL, IDC_HAND));
				::SetCapture(hwnd);

				return 0;
			}
			break;

			// �U�����[�h�̂Ƃ�
		case cve::Mode_Elastic:
			g_curve_elastic.pt_in_ctpt(pt_client, &pt_address);

			if (pt_address.position != cve::Point_Address::Null) {
				g_curve_elastic_previous = g_curve_elastic;

				::SetCursor(LoadCursor(NULL, IDC_HAND));
				::SetCapture(hwnd);

				return 0;
			}
			break;

			// �e�����[�h�̂Ƃ�
		case cve::Mode_Bounce:
			break;

			// ���l�w�胂�[�h�̂Ƃ�
		case cve::Mode_Value:
			break;
		}


		// �J�[�u��D&D����
		if (!move_view)
			is_dragging = true;
		
		::SetCapture(hwnd);
		return 0;

		///////////////////////////////////////////
		//		WM_LBUTTONUP
		//		(�}�E�X�̍��{�^���������ꂽ�Ƃ�)
		///////////////////////////////////////////
	case WM_LBUTTONUP:
		// �I�[�g�R�s�[
		if (g_config.edit_mode == cve::Mode_Normal) {
			if (g_config.auto_copy && pt_address.position != cve::Point_Address::Null)
				::SendMessage(hwnd, WM_COMMAND, CVE_CM_COPY, 0);
		}

		// �J�[�u��D&D����
		if (is_dragging) {
			// Alt�L�[�������Ă��Ȃ��ꍇ
			if (obj_buttons.is_hovered() && ::GetAsyncKeyState(VK_MENU) >= 0) {
				g_config.is_hooked_popup = TRUE;
				g_config.is_hooked_dialog = TRUE;
				
				obj_buttons.click();
				obj_buttons.invalidate(NULL);
			}
		}

		pt_address.position = cve::Point_Address::Null;
		is_dragging = false;

		// Aviutl���ĕ`��
		::SendMessage(g_fp->hwnd, WM_COMMAND, CVE_CM_REDRAW_AVIUTL, 0);
		::ReleaseCapture();
		return 0;

		///////////////////////////////////////////
		//		WM_MBUTTONDOWN
		//		(�}�E�X�̍��{�^�����_�u���N���b�N���ꂽ�Ƃ�)
		///////////////////////////////////////////
	case WM_LBUTTONDBLCLK:
		// ���[�h�U�蕪��
		switch (g_config.edit_mode) {
			// �}���`�x�W�F���[�h�̂Ƃ�
		case cve::Mode_Multibezier:
			//�J�[�\��������_��ɂ��邩�ǂ���
			g_curve_mb[g_config.current_id.multibezier].pt_in_ctpt(pt_client, &pt_address);

			if (pt_address.position == cve::Point_Address::Center)
				g_curve_mb[g_config.current_id.multibezier].delete_point(pt_client);
			else if (ISINRANGEEQ(pt_graph.x, 0, CVE_GRAPH_RESOLUTION)) {
				g_curve_mb_previous = g_curve_mb[g_config.current_id.multibezier];
				g_curve_mb[g_config.current_id.multibezier].add_point(pt_graph);
			}

			::InvalidateRect(hwnd, NULL, FALSE);
			break;

			// ���l�w�胂�[�h�̂Ƃ�
		case cve::Mode_Value:
			//�J�[�\��������_��ɂ��邩�ǂ���
			g_curve_value[g_config.current_id.value].pt_in_ctpt(pt_client, &pt_address);

			if (pt_address.position == cve::Point_Address::Center)
				g_curve_value[g_config.current_id.value].delete_point(pt_client);
			else if (ISINRANGEEQ(pt_graph.x, 0, CVE_GRAPH_RESOLUTION)) {
				g_curve_value_previous = g_curve_value[g_config.current_id.value];
				g_curve_value[g_config.current_id.value].add_point(pt_graph);
			}

			::InvalidateRect(hwnd, NULL, FALSE);
			break;
		}
		return 0;

		///////////////////////////////////////////
		//		WM_RBUTTONUP
		//		(�}�E�X�̉E�{�^���������ꂽ�Ƃ�)
		///////////////////////////////////////////
		//�E�N���b�N���j���[
	case WM_RBUTTONUP:
		cve::apply_config_to_menu(menu, &minfo);
		::ClientToScreen(hwnd, &pt_client);
		::TrackPopupMenu(menu, TPM_LEFTALIGN | TPM_TOPALIGN, pt_client.x, pt_client.y, 0, hwnd, NULL);
		return 0;
	

		///////////////////////////////////////////
		//		WM_MBUTTONDOWN
		//		(�}�E�X�̒����{�^���������ꂽ�Ƃ�)
		///////////////////////////////////////////
	case WM_MBUTTONDOWN:
		if (pt_address.position == cve::Point_Address::Null && !is_dragging) {
			move_view = true;
			pt_view = pt_client;
			prev_scale_x = g_view_info.scale.x;
			prev_scale_y = g_view_info.scale.y;
			prev_o_x = g_view_info.origin.x;
			prev_o_y = g_view_info.origin.y;

			::SetCursor(::LoadCursor(NULL, IDC_SIZEALL));
			::SetCapture(hwnd);
		}
		return 0;

		///////////////////////////////////////////
		//		WM_MBUTTONUP
		//		(�}�E�X�̒����{�^���������ꂽ�Ƃ�)
		///////////////////////////////////////////
	case WM_MBUTTONUP:
		move_view = false;
		move_or_scale = NULL;

		::ReleaseCapture();
		return 0;

		///////////////////////////////////////////
		//		WM_MOUSEMOVE
		//		(�E�B���h�E��ŃJ�[�\�����������Ƃ�)
		///////////////////////////////////////////
	case WM_MOUSEMOVE:

		//����_�ړ�
		if (pt_address.position != cve::Point_Address::Null) {
			// ���[�h�U�蕪��
			switch (g_config.edit_mode) {
				// �W�����[�h�̂Ƃ�
			case cve::Mode_Normal:
				g_curve_normal.move_handle(pt_address, pt_graph, false);

				::InvalidateRect(hwnd, NULL, FALSE);
				::PostMessage(g_window_header.hwnd, WM_COMMAND, CVE_CM_PARAM_REDRAW, 0);
				break;

				// �}���`�x�W�F���[�h�̂Ƃ�
			case cve::Mode_Multibezier:
				if (pt_address.position == cve::Point_Address::Center)
					g_curve_mb[g_config.current_id.multibezier].move_point(pt_address.index, pt_graph, false);
				else
					g_curve_mb[g_config.current_id.multibezier].move_handle(pt_address, pt_graph, false);

				::InvalidateRect(hwnd, NULL, FALSE);
				break;

				// �U�����[�h�̂Ƃ�
			case cve::Mode_Elastic:
				g_curve_elastic.move_handle(pt_graph.y);

				::InvalidateRect(hwnd, NULL, FALSE);
				::PostMessage(g_window_header.hwnd, WM_COMMAND, CVE_CM_PARAM_REDRAW, 0);
				break;

				// �e�����[�h�̂Ƃ�
			case cve::Mode_Bounce:

				break;

				// ���l�w�胂�[�h�̂Ƃ�
			case cve::Mode_Value:

				break;
			}
		}

		//����_�z�o�[���ɃJ�[�\����ύX
		if (!is_dragging) {
			cve::Point_Address tmp;

			// ���[�h�U�蕪��
			switch (g_config.edit_mode) {
				// �W�����[�h�̂Ƃ�
			case cve::Mode_Normal:
				g_curve_normal.pt_in_ctpt(pt_client, &tmp);
				break;

				// �}���`�x�W�F���[�h�̂Ƃ�
			case cve::Mode_Multibezier:
				g_curve_mb[g_config.current_id.multibezier].pt_in_ctpt(pt_client, &tmp);
				break;

				// �U�����[�h�̂Ƃ�
			case cve::Mode_Elastic:
				g_curve_elastic.pt_in_ctpt(pt_client, &tmp);
				break;

				// �e�����[�h�̂Ƃ�
			case cve::Mode_Bounce:
				//g_curve_bounce.pt_in_ctpt(pt_client, &tmp);
				break;

				// ���l�w�胂�[�h�̂Ƃ�
			case cve::Mode_Value:

				break;
			}

			if (tmp.position != cve::Point_Address::Null)
				::SetCursor(::LoadCursor(NULL, IDC_HAND));


			// �r���[�ړ��E�g��k��
			if (move_view) {
				// �g��k��
				if ((::GetAsyncKeyState(VK_CONTROL) < 0 && move_or_scale == 0) || move_or_scale == 2) {
					double coef_x, coef_y;
					coef_x = MINMAX_LIMIT(std::pow(CVE_GRAPH_SCALE_BASE, pt_client.x - pt_view.x),
						CVE_GRAPH_SCALE_MIN / prev_scale_x, CVE_GRAPH_SCALE_MAX / prev_scale_x);
					coef_y = MINMAX_LIMIT(std::pow(CVE_GRAPH_SCALE_BASE, pt_view.y - pt_client.y),
						CVE_GRAPH_SCALE_MIN / prev_scale_y, CVE_GRAPH_SCALE_MAX / prev_scale_y);
					if (::GetAsyncKeyState(VK_SHIFT) < 0) {
						coef_x = coef_y = LARGER(coef_x, coef_y);
						prev_scale_x = prev_scale_y = LARGER(prev_scale_x, prev_scale_y);
					}
					g_view_info.scale.x = prev_scale_x * coef_x;
					g_view_info.scale.y = prev_scale_y * coef_y;
					g_view_info.origin.x = rect_wnd.right * 0.5f + (float)(coef_x * (prev_o_x - rect_wnd.right * 0.5f));
					g_view_info.origin.y = rect_wnd.bottom * 0.5f + (float)(coef_y * (prev_o_y - rect_wnd.bottom * 0.5f));
					if (move_or_scale == 0)
						move_or_scale = 2;
				}
				// �ړ�
				else {
					g_view_info.origin.x = prev_o_x + pt_client.x - pt_view.x;
					g_view_info.origin.y = prev_o_y + pt_client.y - pt_view.y;
					if (move_or_scale == 0)
						move_or_scale = 1;
				}
				::SetCursor(::LoadCursor(NULL, IDC_SIZEALL));
				// �ĕ`��
				::InvalidateRect(hwnd, NULL, FALSE);
			}
		}
		// �J�[�u��D&D����
		else {
			// �J�[�\��
			if (!::PtInRect(&rect_wnd, pt_client))
				::SetCursor(::LoadCursor(g_fp->dll_hinst, MAKEINTRESOURCE(IDC_DRAGGING)));

			// �g��`��
			POINT sc_pt = pt_client;
			::ClientToScreen(hwnd, &sc_pt);
			cve::Rectangle old_rect;

			// �X�V
			int old_id = obj_buttons.update(&sc_pt, &old_rect.rect);
			
			// ��قǂ܂Ńn�C���C�g���Ă���RECT�ƌ��݃z�o�[���Ă���RECT���قȂ�ꍇ
			if (old_id != obj_buttons.id) {
				// ��قǂ܂ŉ��炩��RECT���n�C���C�g���Ă����ꍇ
				if (old_id >= 0)
					obj_buttons.invalidate(&old_rect.rect);

				// ���݉��炩��RECT�Ƀz�o�[���Ă���ꍇ
				if (obj_buttons.is_hovered() && ::GetAsyncKeyState(VK_MENU) >= 0) {
					if (::GetAsyncKeyState(VK_CONTROL) < 0)
						obj_buttons.highlight(1);
					else
						obj_buttons.highlight(0);
				}
			}
		}

		return 0;

		///////////////////////////////////////////
		//		WM_MOUSEWHEEL
		//		(�}�E�X�z�C�[������]�����Ƃ�)
		///////////////////////////////////////////
	case WM_MOUSEWHEEL:
	{
		// Ctrl�L�[��������Ă���Ƃ�(���Ɉړ�)
		if (::GetAsyncKeyState(VK_CONTROL) < 0 && GetAsyncKeyState(VK_SHIFT) >= 0)
			g_view_info.origin.x += (float)(GET_Y_LPARAM(wparam) * CVE_GRAPH_WHEEL_COEF_POS);

		// Shift�L�[��������Ă���Ƃ�(�c�Ɉړ�)
		else if (::GetAsyncKeyState(VK_SHIFT) < 0 && GetAsyncKeyState(VK_CONTROL) >= 0)
			g_view_info.origin.y += (float)(GET_Y_LPARAM(wparam) * CVE_GRAPH_WHEEL_COEF_POS);

		// �k�ڂ̏��������ݒ�
		else {
			double coef = std::pow(CVE_GRAPH_SCALE_BASE, GET_Y_LPARAM(wparam) * CVE_GRAPH_WHEEL_COEF_SCALE);
			double scale_after_x, scale_after_y;

			if (LARGER(g_view_info.scale.x, g_view_info.scale.y) > CVE_GRAPH_SCALE_MAX / coef) {
				coef = CVE_GRAPH_SCALE_MAX / LARGER(g_view_info.scale.x, g_view_info.scale.y);
			}
			else if (SMALLER(g_view_info.scale.x, g_view_info.scale.y) < CVE_GRAPH_SCALE_MIN / coef) {
				coef = CVE_GRAPH_SCALE_MIN / SMALLER(g_view_info.scale.x, g_view_info.scale.y);
			}

			scale_after_x = g_view_info.scale.x * coef;
			scale_after_y = g_view_info.scale.y * coef;

			g_view_info.origin.x =
				(g_view_info.origin.x - rect_wnd.right * 0.5f) *
				(float)(scale_after_x / g_view_info.scale.x) + rect_wnd.right * 0.5f;

			g_view_info.origin.y =
				(g_view_info.origin.y - rect_wnd.bottom * 0.5f) *
				(float)(scale_after_y / g_view_info.scale.y) + rect_wnd.bottom * 0.5f;

			g_view_info.scale.x = scale_after_x;
			g_view_info.scale.y = scale_after_y;
		}
		// �ĕ`��
		::InvalidateRect(hwnd, NULL, FALSE);
		return 0;
	}

		///////////////////////////////////////////
		//		WM_COMMAND
		//		(�R�}���h)
		///////////////////////////////////////////
	case WM_COMMAND:
	{
		const int edit_mode_menu_id[] = {
			ID_MENU_MODE_NORMAL,
			ID_MENU_MODE_MULTIBEZIER,
			ID_MENU_MODE_ELASTIC,
			ID_MENU_MODE_BOUNCE,
			ID_MENU_MODE_VALUE
		};

		// �ҏW���[�h��ύX
		for (const int& el : edit_mode_menu_id) {
			if (wparam == el) {
				g_config.edit_mode = (cve::Edit_Mode)(wparam - ID_MENU_MODE_NORMAL);
				::InvalidateRect(hwnd, NULL, FALSE);
				::SendMessage(g_window_header.hwnd, WM_COMMAND, CVE_CT_EDIT_MODE_NORMAL + wparam - ID_MENU_MODE_NORMAL, 0);
				return 0;
			}
		}
		

		switch (wparam) {
			// �ĕ`��
		case CVE_CM_REDRAW:
			::InvalidateRect(hwnd, NULL, FALSE);
			return 0;

			// �O���t���t�B�b�g
		case ID_MENU_FIT:
		case CVE_CM_FIT:
			g_view_info.fit(rect_wnd);
			::InvalidateRect(hwnd, NULL, FALSE);
			return 0;

			// �n���h���𐮗�
		case ID_MENU_ALIGNHANDLE:
		case CVE_CT_ALIGN:
			g_config.align_handle = g_config.align_handle ? 0 : 1;
			return 0;

			// �J�[�u�𔽓]
		case ID_MENU_REVERSE:
		case CVE_CM_REVERSE:
			// �W�����[�h
			if (g_config.edit_mode == cve::Mode_Normal)
				g_curve_normal.reverse_curve();
			// �}���`�x�W�F���[�h
			else
				g_curve_mb[g_config.current_id.multibezier].reverse_curve();

			::InvalidateRect(hwnd, NULL, FALSE);
			return 0;

			// �n���h����\��
		case ID_MENU_SHOWHANDLE:
		case CVE_CM_SHOW_HANDLE:
			g_config.show_handle = g_config.show_handle ? 0 : 1;

			::InvalidateRect(hwnd, NULL, FALSE);
			return 0;

			// �J�[�u���폜
		case CVE_CM_CLEAR:
		case ID_MENU_DELETE:
		{
			int response = IDOK;
			if (g_config.alert)
				response = ::MessageBox(
					hwnd,
					CVE_STR_WARNING_DELETE,
					CVE_PLUGIN_NAME,
					MB_OKCANCEL | MB_ICONEXCLAMATION
				);

			if (response == IDOK) {
				if (g_config.edit_mode == cve::Mode_Normal) {
					g_curve_normal.clear();

					::PostMessage(g_window_header.hwnd, WM_COMMAND, CVE_CM_PARAM_REDRAW, 0);
				}
				else
					g_curve_mb[g_config.current_id.multibezier].clear();

				::InvalidateRect(hwnd, NULL, FALSE);
				// Aviutl���ĕ`��
				::SendMessage(g_fp->hwnd, WM_COMMAND, CVE_CM_REDRAW_AVIUTL, 0);
			}
			return 0;
		}

		// ���ׂẴJ�[�u���폜
		case ID_MENU_DELETE_ALL:
			if (g_config.edit_mode == cve::Mode_Multibezier) {
				int response = IDOK;
				if (g_config.alert && !lparam)
					response = ::MessageBox(
						hwnd,
						CVE_STR_WARNING_DELETE_ALL,
						CVE_PLUGIN_NAME,
						MB_OKCANCEL | MB_ICONEXCLAMATION
					);

				if (response == IDOK || lparam) {
					for (int i = 0; i < CVE_CURVE_MAX; i++) {
						g_curve_mb[i].clear();
					}

					::InvalidateRect(hwnd, NULL, FALSE);
					// Aviutl���ĕ`��
					::SendMessage(g_fp->hwnd, WM_COMMAND, CVE_CM_REDRAW_AVIUTL, 0);
				}
			}
			return 0;

			// �J�[�u�̃v���p�e�B
		case ID_MENU_PROPERTY:
		{
			std::string info;
			info = "ID : " + std::to_string(g_config.current_id.multibezier) + "\n"
				+ "�|�C���g�� : " + std::to_string(g_curve_mb[g_config.current_id.multibezier].ctpts.size);
			::MessageBox(
				hwnd,
				info.c_str(),
				CVE_PLUGIN_NAME,
				MB_OK | MB_ICONINFORMATION
			);

			return 0;
		}

		//�ݒ�
		case ID_MENU_CONFIG:
			::DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_CONFIG), hwnd, dialogproc_config);
			g_window_main.redraw();
			return 0;

			// �{�v���O�C���ɂ���
		case ID_MENU_ABOUT:
			::MessageBox(hwnd, CVE_STR_ABOUT, CVE_PLUGIN_NAME, MB_OK);
			return 0;

			// �l���R�s�[
		case ID_MENU_COPY:
		case CVE_CM_COPY:
			if (g_config.edit_mode == cve::Mode_Normal) {
				TCHAR result_str[12];
				int result = g_curve_normal.create_number();
				//������֕ϊ�
				::_itoa_s(result, result_str, 12, 10);
				//�R�s�[
				cve::copy_to_clipboard(hwnd, result_str);
			}
			return 0;

			// �l���R�s�[(4����)
		case ID_MENU_COPY4D:
			if (g_config.edit_mode == cve::Mode_Normal) {
				std::string str_result = g_curve_normal.create_parameters();
				LPCTSTR result = str_result.c_str();

				cve::copy_to_clipboard(hwnd, result);
			}
			return 0;

			// �l��ǂݎ��
		case CVE_CM_READ:
		case ID_MENU_READ:
			::DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_READ), hwnd, dialogproc_read);
			::InvalidateRect(hwnd, NULL, FALSE);

			return 0;

			// �ۑ��{�^��
		case ID_MENU_SAVE:
		case CVE_CM_SAVE:
			::DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_SAVE), hwnd, dialogproc_save);

			return 0;

			// �w���v
		case ID_MENU_HELP:
			::ShellExecute(0, "open", CVE_PLUGIN_LINK_HELP, NULL, NULL, SW_SHOWNORMAL);
			return 0;

			// �Z�p���[�^�̈ʒu��������
		case ID_MENU_RESET_SEPARATOR:
			g_config.separator = CVE_SEPARATOR_WIDTH;
			::SendMessage(g_window_main.hwnd, WM_COMMAND, CVE_CM_REDRAW, 0);
			return 0;

			// �O��ID�Ɉړ�
		case ID_MENU_ID_BACK:
			::SendMessage(g_window_header.hwnd, WM_COMMAND, CVE_CM_ID_BACK, 0);
			return 0;

			// ����ID�Ɉړ�
		case ID_MENU_ID_NEXT:
			::SendMessage(g_window_header.hwnd, WM_COMMAND, CVE_CM_ID_NEXT, 0);
			return 0;

			// ���C�A�E�g���c�����ɂ���
		case ID_MENU_VERTICAL:
			g_config.layout_mode = cve::Config::Vertical;

			::SendMessage(g_window_main.hwnd, WM_COMMAND, CVE_CM_REDRAW, 0);
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_FIT, 0);
			return 0;

			// ���C�A�E�g���������ɂ���
		case ID_MENU_HORIZONTAL:
			g_config.layout_mode = cve::Config::Horizontal;

			::SendMessage(g_window_main.hwnd, WM_COMMAND, CVE_CM_REDRAW, 0);
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_FIT, 0);
			return 0;

		case ID_MENU_LATEST_VERSION:
			::DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_LATEST_VERSION), hwnd, dialogproc_latest_version);
			return 0;
		}

		return 0;
	}

	default:
		return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}