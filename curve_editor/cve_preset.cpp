//----------------------------------------------------------------------------------
//		Curve Editor
//		�\�[�X�t�@�C��(�v���Z�b�g)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"



//---------------------------------------------------------------------
//		�R���X�g���N�^
//---------------------------------------------------------------------
cve::Preset::Preset()
{
	::time(&unix_time);

	bool is_id_available;

	int count = 0;

	// ID���J�E���g�A�b�v���Ă������p�\��ID��T��
	while (true) {
		is_id_available = true;

		// �����̃v���Z�b�g��ID������
		for (int i = 0; i < (int)g_presets.size; i++) {
			if (count == g_presets[i].id) {
				is_id_available = false;
				break;
			}
		}

		if (is_id_available)
			break;

		count++;
	}

	id = count;
}



//---------------------------------------------------------------------
//		�v���Z�b�g(�E�B���h�E)���쐬
//---------------------------------------------------------------------
BOOL cve::Preset::initialize(HWND hwnd_p, const Curve* cv, LPTSTR nm)
{
	strcpy_s(name, CVE_PRESET_NAME_MAX, nm);
	std::string class_name = "cve_preset_" + std::to_string(id);
	curve = *cv;
	hwnd_parent = hwnd_p;
	content_type = Button::Null;

	RECT rect = {
		g_config.preset_size * id,
		0,
		g_config.preset_size * (id + 1),
		g_config.preset_size
	};

	edge_flag = CVE_EDGE_ALL;


	return create(hwnd_p, (LPTSTR)class_name.c_str(), wndproc_static, NULL, rect, this);
}



//---------------------------------------------------------------------
//		�v���Z�b�g���ړ�
//---------------------------------------------------------------------
void cve::Preset::move(int panel_width, int index)
{
	const int w = 2 * CVE_MARGIN_PRESET + g_config.preset_size;
	int x_count = index % (panel_width / w);
	int y_count = index / (panel_width / w);

	::MoveWindow(
		hwnd,
		w * x_count,
		w * y_count,
		g_config.preset_size,
		g_config.preset_size + CVE_MARGIN_PRESET,
		TRUE
	);
}



//---------------------------------------------------------------------
//		�E�B���h�E�v���V�[�W��(static�ϐ��g�p�s��)
//---------------------------------------------------------------------
LRESULT cve::Preset::wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{

	switch (msg) {

	default:
		return Button::wndproc(hwnd, msg, wparam, lparam);
	}
}