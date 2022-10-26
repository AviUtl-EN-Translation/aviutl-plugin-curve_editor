//----------------------------------------------------------------------------------
//		Curve Editor
//		�\�[�X�t�@�C���i�r�b�g�}�b�v�o�b�t�@�j
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "ce_header.hpp"



//---------------------------------------------------------------------
//		������
//---------------------------------------------------------------------
void ce::Bitmap_Buffer::init(HWND hw)
{
	hwnd = hw;
	HDC hdc = GetDC(hw);
	hdc_memory = CreateCompatibleDC(hdc);
	bitmap = CreateCompatibleBitmap(hdc, CE_MAX_W, CE_MAX_H);
	SelectObject(hdc_memory, bitmap);
	ReleaseDC(hw, hdc);
}



//---------------------------------------------------------------------
//		�I��
//---------------------------------------------------------------------
void ce::Bitmap_Buffer::exit() const
{
	DeleteDC(hdc_memory);
	DeleteObject(bitmap);
}



//---------------------------------------------------------------------
//		�r�b�g�}�b�v���o�b�t�@�����ʂɓ]��
//---------------------------------------------------------------------
void ce::Bitmap_Buffer::transfer(const RECT& rect) const
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);
	BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdc_memory, 0, 0, SRCCOPY);
	EndPaint(hwnd, &ps);
	DeleteDC(hdc);
}