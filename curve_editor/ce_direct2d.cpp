//----------------------------------------------------------------------------------
//		Curve Editor
//		�\�[�X�t�@�C�� (Direct2D)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "ce_header.hpp"



//---------------------------------------------------------------------
//		������
//---------------------------------------------------------------------
void ce::Direct2d_Window::init(HWND hwnd, LPRECT rect)
{
	D2D1_SIZE_U size = {
		rect->right - rect->left,
		rect->bottom - rect->top
	};
	g_d2d1_factory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(hwnd, size),
		&render_target
	);
}


//---------------------------------------------------------------------
//		����
//---------------------------------------------------------------------
void ce::Direct2d_Window::setup(COLORREF cr)
{
	render_target->BeginDraw();
	g_render_target->SetTransform(D2D1::Matrix3x2F::Identity());
	g_render_target->Clear(D2D1::ColorF(TO_BGR(cr)));
	g_render_target->EndDraw();
}



//---------------------------------------------------------------------
//		���T�C�Y
//---------------------------------------------------------------------
void ce::Direct2d_Window::resize(int width, int height)
{
	D2D1_SIZE_U size = {
		width,
		height
	};
	render_target->Resize(&size);
}



//---------------------------------------------------------------------
//		�I��
//---------------------------------------------------------------------
void ce::Direct2d_Window::exit()
{
	if (render_target != NULL)
		render_target->Release();
}