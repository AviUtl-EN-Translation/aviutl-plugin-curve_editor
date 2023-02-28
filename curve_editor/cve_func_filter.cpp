//----------------------------------------------------------------------------------
//		Curve Editor
//		�\�[�X�t�@�C�� (�t�B���^�֐�)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"



void ini_load_configs(FILTER*);
void ini_write_configs(FILTER*);



//---------------------------------------------------------------------
//		FILTER�\���̂̃|�C���^���擾
//---------------------------------------------------------------------
EXTERN_C FILTER_DLL __declspec(dllexport)* __stdcall GetFilterTable(void)
{
	return &g_filter;
}



//---------------------------------------------------------------------
//		���������Ɏ��s�����֐�
//---------------------------------------------------------------------
BOOL filter_init(FILTER* fp)
{
	g_fp = fp;

	// aviutl.ini����ݒ��ǂݍ���
	ini_load_configs(fp);

	// Direct2D�̏�����
	if (!cve::dx_init())
		::MessageBox(NULL, CVE_STR_ERROR_D2D_INIT_FAILED, CVE_FILTER_NAME, MB_OK);

	return TRUE;
}



//---------------------------------------------------------------------
//		�I�����Ɏ��s�����֐�
//---------------------------------------------------------------------
BOOL filter_exit(FILTER* fp)
{
	// aviutl.ini�ɐݒ����������
	ini_write_configs(fp);

	// DirectX�I�u�W�F�N�g�J��
	cve::dx_exit();

	return TRUE;
}



//---------------------------------------------------------------------
//		�v���W�F�N�g�t�@�C���ǂݍ��ݎ��Ɏ��s�����֐�
//---------------------------------------------------------------------
BOOL on_project_load(FILTER* fp, void* editp, void* data, int size)
{
	static cve::Static_Array<cve::Curve_Points, CVE_POINT_MAX> point_data[CVE_CURVE_MAX];

	if (data) {
		memcpy(point_data, data, size);

		for (int i = 0; i < CVE_CURVE_MAX; i++)
			g_curve_bezier_multi[i].ctpts = point_data[i];

		g_curve_bezier_multi_trace = g_curve_bezier_multi[g_config.current_id.multi - 1];

		if (g_window_editor.hwnd) {
			g_window_editor.redraw();

			for (int i = 0; i < CVE_CURVE_MAX; i++) {
				if (!g_curve_bezier_multi[i].is_data_valid()) {
					int response = IDOK;

					response = ::MessageBox(
						fp->hwnd,
						CVE_STR_WARNING_DATA_INVALID,
						CVE_FILTER_NAME,
						MB_OKCANCEL | MB_ICONEXCLAMATION
					);

					if (response == IDOK)
						::PostMessage(g_window_editor.hwnd, WM_COMMAND, ID_MENU_DELETE_ALL, TRUE);

					break;
				}
			}
		}
	}

	return TRUE;
}



//---------------------------------------------------------------------
//		�v���W�F�N�g�t�@�C���ۑ����Ɏ��s�����֐�
//---------------------------------------------------------------------
BOOL on_project_save(FILTER* fp, void* editp, void* data, int* size)
{
	static cve::Static_Array<cve::Curve_Points, CVE_POINT_MAX> point_data[CVE_CURVE_MAX] = {};

	for (int i = 0; i < CVE_CURVE_MAX; i++)
		point_data[i] = g_curve_bezier_multi[i].ctpts;

	if (!data) {
		*size = sizeof(point_data);
	}
	else {
		memcpy(data, point_data, sizeof(point_data));
	}
	return TRUE;
}



//---------------------------------------------------------------------
//		aviutl.ini����ݒ��ǂݍ���
//---------------------------------------------------------------------
void ini_load_configs(FILTER* fp)
{
	// �ҏW���[�h
	g_config.edit_mode = (cve::Edit_Mode)fp->exfunc->ini_load_int(fp,
		CVE_INI_KEY_MODE_EDIT,
		cve::Mode_Bezier
	);
	// �K�p���[�h
	g_config.apply_mode = (cve::Config::Apply_Mode)fp->exfunc->ini_load_int(fp, 
		CVE_INI_KEY_MODE_APPLY,
		cve::Config::Normal
	);
	// ���C�A�E�g���[�h
	g_config.layout_mode = (cve::Config::Layout_Mode)fp->exfunc->ini_load_int(fp,
		CVE_INI_KEY_MODE_LAYOUT,
		cve::Config::Vertical
	);

	// �e�[�}
	g_config.theme = fp->exfunc->ini_load_int(fp,
		CVE_INI_KEY_THEME,
		0
	);
	// 1�X�e�b�v�O�̃J�[�u��\��
	g_config.trace = fp->exfunc->ini_load_int(fp,
		CVE_INI_KEY_SHOW_TRACE,
		true
	);
	// �����x�W�F��\��
	g_config.show_bezier_deriv = fp->exfunc->ini_load_int(fp,
		CVE_INI_KEY_SHOW_BEZIER_DERIV,
		false
	);
	// �|�b�v�A�b�v��\��
	g_config.show_popup = fp->exfunc->ini_load_int(fp,
		CVE_INI_KEY_SHOW_POPUP,
		true
	);
	// �z�C�[���𔽓]
	g_config.reverse_wheel = fp->exfunc->ini_load_int(fp,
		CVE_INI_KEY_REVERSE_WHEEL,
		false
	);
	// �����R�s�[
	g_config.auto_copy = fp->exfunc->ini_load_int(fp,
		CVE_INI_KEY_AUTO_COPY,
		false
	);
	// �����K�p
	g_config.auto_apply = fp->exfunc->ini_load_int(fp,
		CVE_INI_KEY_AUTO_APPLY,
		true
	);
	// �x�W�F�𒼐��ɂ���
	g_config.linearize = fp->exfunc->ini_load_int(fp,
		CVE_INI_KEY_LINEARIZE,
		false
	);
	// �Z�p���[�^
	g_config.separator = fp->exfunc->ini_load_int(fp,
		CVE_INI_KEY_SEPARATOR,
		CVE_SEPARATOR_WIDTH
	);
	// �n���h���𐮗�
	g_config.align_handle = fp->exfunc->ini_load_int(fp,
		CVE_INI_KEY_ALIGN_HANDLE,
		true
	);
	// �n���h����\��
	g_config.show_handle = fp->exfunc->ini_load_int(fp,
		CVE_INI_KEY_SHOW_HANDLE,
		true
	);
	// �v���Z�b�g�̃T�C�Y
	g_config.preset_size = fp->exfunc->ini_load_int(fp,
		CVE_INI_KEY_PRESET_SIZE,
		CVE_DEF_PRESET_SIZE
	);
	// �J�[�u�̐F
	g_config.curve_color = (COLORREF)fp->exfunc->ini_load_int(fp,
		CVE_INI_KEY_CURVE_COLOR,
		cve::Config::CURVE_COLOR_DEFAULT
	);
	// �A�b�v�f�[�g��ʒm
	g_config.notify_update = fp->exfunc->ini_load_int(fp,
		CVE_INI_KEY_NOTIFY_UPDATE,
		false
	);

	// �J�[�u�̐��l(�x�W�F)
	g_curve_bezier.read_number(
		fp->exfunc->ini_load_int(fp,
			CVE_INI_KEY_NUM_BEZIER,
			CVE_NUM_BEZIER_LINER
		)
	);
	// �J�[�u�̐��l(�U��)
	g_curve_elastic.read_number(
		fp->exfunc->ini_load_int(fp,
			CVE_INI_KEY_NUM_ELASTIC,
			CVE_NUM_ELASTIC_DEFAULT
		)
	);
	// �J�[�u�̐��l(�o�E���X)
	g_curve_bounce.read_number(
		fp->exfunc->ini_load_int(fp,
			CVE_INI_KEY_NUM_BOUNCE,
			CVE_NUM_BOUNCE_DEFAULT
		)
	);
}



//---------------------------------------------------------------------
//		aviutl.ini�ɐݒ����������
//---------------------------------------------------------------------
void ini_write_configs(FILTER* fp)
{
	// �ҏW���[�h
	fp->exfunc->ini_save_int(fp,
		CVE_INI_KEY_MODE_EDIT,
		g_config.edit_mode
	);
	// �K�p���[�h
	fp->exfunc->ini_save_int(fp,
		CVE_INI_KEY_MODE_APPLY,
		g_config.apply_mode
	);
	// ���C�A�E�g���[�h
	fp->exfunc->ini_save_int(fp,
		CVE_INI_KEY_MODE_LAYOUT,
		g_config.layout_mode
	);

	// �n���h���𐮗�
	fp->exfunc->ini_save_int(fp,
		CVE_INI_KEY_ALIGN_HANDLE,
		g_config.align_handle
	);
	// �n���h����\��
	fp->exfunc->ini_save_int(fp,
		CVE_INI_KEY_SHOW_HANDLE,
		g_config.show_handle
	);
	// �����x�W�F��\��
	fp->exfunc->ini_save_int(fp,
		CVE_INI_KEY_SHOW_BEZIER_DERIV,
		g_config.show_bezier_deriv
	);
	// �Z�p���[�^
	fp->exfunc->ini_save_int(fp,
		CVE_INI_KEY_SEPARATOR,
		g_config.separator
	);

	// �J�[�u�̐��l(�x�W�F)
	fp->exfunc->ini_save_int(fp,
		CVE_INI_KEY_NUM_BEZIER,
		g_curve_bezier.create_number()
	);
	// �J�[�u�̐��l(�U��)
	fp->exfunc->ini_save_int(fp,
		CVE_INI_KEY_NUM_ELASTIC,
		g_curve_elastic.create_number()
	);
	// �J�[�u�̐��l(�o�E���X)
	fp->exfunc->ini_save_int(fp,
		CVE_INI_KEY_NUM_BOUNCE,
		g_curve_bounce.create_number()
	);
}



//---------------------------------------------------------------------
//		�E�B���h�E�v���V�[�W��
//---------------------------------------------------------------------
BOOL filter_wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, void* editp, FILTER* fp)
{
	RECT rect_wnd;
	::GetClientRect(hwnd, &rect_wnd);
	static bool init = true;


	switch (msg) {
		// �E�B���h�E�쐬��
	case WM_FILTER_INIT:
		g_editp = editp;

		// WS_CLIPCHILDREN��ǉ�
		::SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) | WS_CLIPCHILDREN);

		g_window_main.create(
			g_fp->dll_hinst,
			hwnd,
			"WINDOW_MAIN",
			wndproc_main,
			NULL,
			NULL,
			rect_wnd,
			NULL
		);
		return 0;

	case WM_FILTER_CHANGE_WINDOW:
		if (init) {
			init = false;
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_GET_EXEDIT, 0);
		}
		return 0;

	case WM_SIZE:
	{
		RECT rect_wnd_main = rect_wnd;
		rect_wnd_main.right = std::max((int)rect_wnd_main.right, CVE_MIN_W);
		rect_wnd_main.bottom = std::max((int)rect_wnd_main.bottom, CVE_SEPARATOR_WIDTH * 2 + CVE_MENU_H);
		g_window_main.move(rect_wnd_main);
		return 0;
	}

	case WM_KEYDOWN:
		return cve::on_keydown(wparam);

	case WM_COMMAND:
		switch (wparam) {
		case CVE_CM_REDRAW_AVIUTL:
			return TRUE;
		}
	}
	return 0;
}