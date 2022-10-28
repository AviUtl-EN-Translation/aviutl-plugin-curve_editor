//----------------------------------------------------------------------------------
//		Curve Editor
//		�\�[�X�t�@�C�� (���̑��̊֐�)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"



//---------------------------------------------------------------------
//		Direct2D��������
//---------------------------------------------------------------------
void cve::d2d_init()
{
	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_d2d1_factory);
	D2D1_RENDER_TARGET_PROPERTIES prop;
	prop = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(
			DXGI_FORMAT_B8G8R8A8_UNORM,
			D2D1_ALPHA_MODE_IGNORE),
		0, 0,
		D2D1_RENDER_TARGET_USAGE_NONE,
		D2D1_FEATURE_LEVEL_DEFAULT
	);
	g_d2d1_factory->CreateDCRenderTarget(&prop, &g_render_target);
}



//---------------------------------------------------------------------
//		split�֐�
//---------------------------------------------------------------------
std::vector<std::string> cve::split(const std::string& s, TCHAR c)
{
	std::vector<std::string> elems;
	std::string item;
	for (TCHAR ch : s) {
		if (ch == c) {
			if (!item.empty()) {
				elems.emplace_back(item);
				item.clear();
			}
		}
		else item += ch;
	}
	if (!item.empty())
		elems.emplace_back(item);

	return elems;
}



//---------------------------------------------------------------------
//		�N���b�v�{�[�h�Ƀe�L�X�g���R�s�[
//---------------------------------------------------------------------
BOOL cve::copy_to_clipboard(HWND hwnd, LPCTSTR text)
{
	HGLOBAL memory;
	LPTSTR buffer;

	if (!OpenClipboard(hwnd))
		return FALSE;

	::EmptyClipboard();
	memory = GlobalAlloc(GHND | GMEM_SHARE, strlen(text) + 1);
	buffer = (PTSTR)::GlobalLock(memory);
	::lstrcpy(buffer, text);

	::GlobalUnlock(memory);
	::SetClipboardData(CF_TEXT, memory);
	::CloseClipboard();

	return TRUE;
}



//---------------------------------------------------------------------
//		���������Z
//---------------------------------------------------------------------
void cve::subtract_length(cve::Float_Point* pt, const cve::Float_Point& st, const cve::Float_Point& ed, float length)
{
	float old_length = (float)DISTANCE(st, ed);

	if (old_length == 0) {
		pt->x = ed.x;
		pt->y = ed.y;
	}
	if (length > old_length) {
		pt->x = st.x;
		pt->y = st.y;
	}

	double length_ratio = (old_length - length) / old_length;
	float after_x = (float)(st.x + (ed.x - st.x) * length_ratio);
	float after_y = (float)(st.y + (ed.y - st.y) * length_ratio);

	pt->x = after_x;
	pt->y = after_y;
}



//---------------------------------------------------------------------
//		g_config�̓��e���|�b�v�A�b�v���j���[�ɔ��f������
//---------------------------------------------------------------------
void cve::apply_config_to_menu(HMENU menu, MENUITEMINFO* mi) {
	mi->fMask = MIIM_STATE;

	// Value/ID
	mi->fState = g_config.mode == cve::Mode_ID ? MFS_CHECKED : MFS_UNCHECKED;
	SetMenuItemInfo(menu, ID_MENU_MODE_ID, FALSE, mi);
	mi->fState = g_config.mode == cve::Mode_ID ? MFS_UNCHECKED : MFS_CHECKED;
	SetMenuItemInfo(menu, ID_MENU_MODE_VALUE, FALSE, mi);

	//���̑�
	mi->fState = g_config.show_handle ? MFS_CHECKED : MFS_UNCHECKED;
	SetMenuItemInfo(menu, ID_MENU_SHOWHANDLE, FALSE, mi);
	mi->fState = g_config.align_handle ? MFS_CHECKED : MFS_UNCHECKED;
	SetMenuItemInfo(menu, ID_MENU_ALIGNHANDLE, FALSE, mi);

	// �{�^���𖳌���/�L����
	// ID���[�h�ŗL����
	// �`�F�b�N�{�b�N�X�����݂���ꍇ
	mi->fState |= g_config.mode == cve::Mode_ID ? MFS_ENABLED : MFS_DISABLED;
	SetMenuItemInfo(menu, ID_MENU_ALIGNHANDLE, FALSE, mi);
	// �`�F�b�N�{�b�N�X�����݂��Ȃ��ꍇ
	mi->fState = g_config.mode == cve::Mode_ID ? MFS_ENABLED : MFS_DISABLED;
	SetMenuItemInfo(menu, ID_MENU_PROPERTY, FALSE, mi);
	SetMenuItemInfo(menu, ID_MENU_DELETE_ALL, FALSE, mi);

	// Value���[�h�ŗL����
	mi->fState = g_config.mode == cve::Mode_ID ? MFS_DISABLED : MFS_ENABLED;
	SetMenuItemInfo(menu, ID_MENU_COPY, FALSE, mi);
	SetMenuItemInfo(menu, ID_MENU_COPY4D, FALSE, mi);
	SetMenuItemInfo(menu, ID_MENU_READ, FALSE, mi);

	//�v���O�C�����̔��f
	mi->fMask = MIIM_TYPE;
	mi->dwTypeData = CVE_PLUGIN_NAME "�ɂ���";
	SetMenuItemInfo(menu, ID_MENU_ABOUT, FALSE, mi);
}



//---------------------------------------------------------------------
//		�L�[�������̏���
//---------------------------------------------------------------------
LRESULT cve::on_keydown(WPARAM wparam)
{
	switch (wparam) {
		//[R]
	case 82:
		if (g_window_editor.hwnd)
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_REVERSE, 0);
		return 0;

		//[C]
	case 67:
		if (::GetAsyncKeyState(VK_CONTROL) < 0 && g_config.mode == cve::Mode_Value)
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_COPY, 0);
		return 0;

		//[S]
	case 83:
		if (::GetAsyncKeyState(VK_CONTROL) < 0)
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_SAVE, 0);
		else
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_SHOWHANDLE, 0);
		return 0;

		//[<]
	case VK_LEFT:
		if (g_config.mode == cve::Mode_ID && g_window_header.hwnd)
			::SendMessage(g_window_header.hwnd, WM_COMMAND, CVE_CM_ID_BACK, 0);
		return 0;

		//[>]
	case VK_RIGHT:
		if (g_config.mode == cve::Mode_ID && g_window_header.hwnd)
			::SendMessage(g_window_header.hwnd, WM_COMMAND, CVE_CM_ID_NEXT, 0);
		return 0;

		//[Home]
	case VK_HOME:
		if (g_window_editor.hwnd)
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_FIT, 0);
		return 0;

		//[A]
	case 65:
		if (g_window_editor.hwnd)
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CT_ALIGN, 0);
		return 0;

	case VK_DELETE:
		if (g_window_editor.hwnd)
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_CLEAR, 0);
		return 0;
	}
	return 0;
}