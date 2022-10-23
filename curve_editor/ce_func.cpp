//----------------------------------------------------------------------------------
//		Curve Editor
//		�\�[�X�t�@�C�� (���̑��̊֐�)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "ce_header.hpp"



//---------------------------------------------------------------------
//		aviutl.ini����ݒ��ǂݍ���
//---------------------------------------------------------------------
void ini_load_configs(FILTER* fp)
{
	g_config.theme = fp->exfunc->ini_load_int(fp, "theme", 0);
	g_config.trace = fp->exfunc->ini_load_int(fp, "show_previous_curve", 1);
	g_config.alert = fp->exfunc->ini_load_int(fp, "show_alerts", 1);
	g_config.auto_copy = fp->exfunc->ini_load_int(fp, "auto_copy", 0);
	g_config.current_id = fp->exfunc->ini_load_int(fp, "id", 0);
	g_curve_value.ctpt[0].x = fp->exfunc->ini_load_int(fp, "x1", (int)(CE_GR_RESOLUTION * 0.4));
	g_curve_value.ctpt[0].y = fp->exfunc->ini_load_int(fp, "y1", (int)(CE_GR_RESOLUTION * 0.4));
	g_curve_value.ctpt[1].x = fp->exfunc->ini_load_int(fp, "x2", (int)(CE_GR_RESOLUTION * 0.6));
	g_curve_value.ctpt[1].y = fp->exfunc->ini_load_int(fp, "y2", (int)(CE_GR_RESOLUTION * 0.6));
	g_config.separator = fp->exfunc->ini_load_int(fp, "separator", CE_SEPR_W);
	g_config.mode = (ce::Config::Mode)fp->exfunc->ini_load_int(fp, "mode", 0);
	g_config.align_handle = fp->exfunc->ini_load_int(fp, "align_handle", 1);
	g_config.show_handle = fp->exfunc->ini_load_int(fp, "show_handle", 1);
	g_config.preset_size = fp->exfunc->ini_load_int(fp, "preset_size", CE_DEF_PRESET_SIZE);
}



//---------------------------------------------------------------------
//		aviutl.ini����ݒ����������
//---------------------------------------------------------------------
void ini_write_configs(FILTER* fp)
{
	fp->exfunc->ini_save_int(fp, "x1", g_curve_value.ctpt[0].x);
	fp->exfunc->ini_save_int(fp, "y1", g_curve_value.ctpt[0].y);
	fp->exfunc->ini_save_int(fp, "x2", g_curve_value.ctpt[1].x);
	fp->exfunc->ini_save_int(fp, "y2", g_curve_value.ctpt[1].y);
	fp->exfunc->ini_save_int(fp, "separator", g_config.separator);
	fp->exfunc->ini_save_int(fp, "mode", g_config.mode);
	fp->exfunc->ini_save_int(fp, "align_handle", g_config.align_handle);
	fp->exfunc->ini_save_int(fp, "show_handle", g_config.show_handle);
}



//---------------------------------------------------------------------
//		split�֐�
//---------------------------------------------------------------------
std::vector<std::string> split(const std::string& s, TCHAR c)
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
BOOL copy_to_clipboard(HWND hwnd, LPCTSTR text)
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
//		for UI
//---------------------------------------------------------------------
ce::Float_Point subtract_length(ce::Float_Point st, ce::Float_Point ed, float length)
{
	ce::Float_Point result;
	float old_length = (float)DISTANCE(st, ed);
	if (old_length == 0)
		return ed;
	if (length > old_length)
		return st;
	double length_ratio = (old_length - length) / old_length;
	float after_x = (float)(st.x + (ed.x - st.x) * length_ratio);
	float after_y = (float)(st.y + (ed.y - st.y) * length_ratio);
	result = {after_x, after_y};

	return result;
}



//---------------------------------------------------------------------
//		g_config�̓��e���|�b�v�A�b�v���j���[�ɔ��f������
//---------------------------------------------------------------------
void apply_config_to_menu(HMENU menu, MENUITEMINFO* mi) {
	mi->fMask = MIIM_STATE;

	// Value/ID
	mi->fState = g_config.mode == ce::Config::ID ? MFS_CHECKED : MFS_UNCHECKED;
	SetMenuItemInfo(menu, ID_MENU_MODE_ID, FALSE, mi);
	mi->fState = g_config.mode == ce::Config::ID ? MFS_UNCHECKED : MFS_CHECKED;
	SetMenuItemInfo(menu, ID_MENU_MODE_VALUE, FALSE, mi);

	//���̑�
	mi->fState = g_config.show_handle ? MFS_CHECKED : MFS_UNCHECKED;
	SetMenuItemInfo(menu, ID_MENU_SHOWHANDLE, FALSE, mi);
	mi->fState = g_config.align_handle ? MFS_CHECKED : MFS_UNCHECKED;
	SetMenuItemInfo(menu, ID_MENU_ALIGNHANDLE, FALSE, mi);

	// �{�^���𖳌���/�L����
	// ID���[�h�ŗL����
	// �`�F�b�N�{�b�N�X�����݂���ꍇ
	mi->fState |= g_config.mode == ce::Config::ID ? MFS_ENABLED : MFS_DISABLED;
	SetMenuItemInfo(menu, ID_MENU_ALIGNHANDLE, FALSE, mi);
	// �`�F�b�N�{�b�N�X�����݂��Ȃ��ꍇ
	mi->fState = g_config.mode == ce::Config::ID ? MFS_ENABLED : MFS_DISABLED;
	SetMenuItemInfo(menu, ID_MENU_PROPERTY, FALSE, mi);

	// Value���[�h�ŗL����
	mi->fState = g_config.mode == ce::Config::ID ? MFS_DISABLED : MFS_ENABLED;
	SetMenuItemInfo(menu, ID_MENU_COPY, FALSE, mi);
	SetMenuItemInfo(menu, ID_MENU_COPY4D, FALSE, mi);
	SetMenuItemInfo(menu, ID_MENU_READ, FALSE, mi);

	//�v���O�C�����̔��f
	mi->fMask = MIIM_TYPE;
	mi->dwTypeData = CE_PLUGIN_NAME "�ɂ���";
	SetMenuItemInfo(menu, ID_MENU_ABOUT, FALSE, mi);
}



//---------------------------------------------------------------------
//		RECT�ɗ]����ݒ�
//---------------------------------------------------------------------
void rect_set_margin(LPRECT rect, int left, int top, int right, int bottom)
{
	rect->left += left;
	rect->top += top;
	rect->right -= right;
	rect->bottom -= bottom;

	if (rect->left > rect->right)
		rect->left = rect->right = (rect->left + rect->right) / 2;
	if (rect->top > rect->bottom)
		rect->top = rect->bottom = (rect->top + rect->bottom) / 2;
}



//---------------------------------------------------------------------
//		�L�[�������̏���
//---------------------------------------------------------------------
LRESULT on_keydown(WPARAM wparam)
{
	switch (wparam) {
	case 82: //[R]
		if (g_window_editor.hwnd)
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CE_CM_REVERSE, 0);
		return 0;

	case 67: //[C]
		if (::GetAsyncKeyState(VK_CONTROL) < 0 && g_config.mode == ce::Config::Value)
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CE_CM_COPY, 0);
		return 0;

	case 83: //[S]
		if (::GetAsyncKeyState(VK_CONTROL) < 0)
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CE_CM_SAVE, 0);
		else
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CE_CM_SHOWHANDLE, 0);
		return 0;

	case VK_LEFT: //[<]	
		if (g_config.mode == ce::Config::ID && g_window_header.hwnd)
			::SendMessage(g_window_header.hwnd, WM_COMMAND, CE_CM_ID_BACK, 0);
		return 0;

	case VK_RIGHT: //[>]
		if (g_config.mode == ce::Config::ID && g_window_header.hwnd)
			::SendMessage(g_window_header.hwnd, WM_COMMAND, CE_CM_ID_NEXT, 0);
		return 0;

	case 70: //[F]
		if (g_window_editor.hwnd)
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CE_CM_FIT, 0);
		return 0;

	case 65: //[A]
		if (g_window_editor.hwnd)
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CE_CT_ALIGN, 0);
		return 0;

	case VK_DELETE:
		if (g_window_editor.hwnd)
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CE_CM_CLEAR, 0);
		return 0;
	}
	return 0;
}