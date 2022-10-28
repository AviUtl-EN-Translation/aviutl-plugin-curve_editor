//----------------------------------------------------------------------------------
//		Curve Editor
//		�\�[�X�t�@�C���i��`�j
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"



//---------------------------------------------------------------------
//		������(1)
//---------------------------------------------------------------------
void cve::Rectangle::set(const RECT& rc)
{
	rect = rc;
}



//---------------------------------------------------------------------
//		������(2)
//---------------------------------------------------------------------
void cve::Rectangle::set(int left, int top, int right, int bottom)
{
	rect = {
		left, top,
		right, bottom
	};
}



//---------------------------------------------------------------------
//		��n�ɕ�������
//---------------------------------------------------------------------
void cve::Rectangle::divide(LPRECT rects_child[], float weights[], int n) const
{
	// �e�v�f�̉���
	int width_parent = rect.right - rect.left;
	// ��������ꍇ�̎q�v�f�̉���
	int width_child = width_parent / n;
	float weights_sum = 0;
	// 0�Ԗڂ���k-1�Ԗڂ܂ł̏d�݂̑��a��n�i�[���ꂽ�z��
	std::vector<float> list_weights_sum;
	int left, right;

	// �d�݂�����ꍇ
	if (weights) {
		// �d�݂̑��a���v�Z����ƂƂ��ɁA0�Ԗڂ���k-1�Ԗڂ܂ł̏d�݂̑��a��list_weights_sum[k - 1]�ɑ������B
		for (int i = 0; i < n; i++) {
			weights_sum += weights[i];
			list_weights_sum.emplace_back(weights_sum);
		}
	}

	// ���ꂼ���RECT�̈ʒu��ݒ�
	for (int i = 0; i < n; i++) {
		// �d�݂�����ꍇ
		if (weights) {
			left = (int)(width_parent * ((i == 0) ? 0 : list_weights_sum[i - 1]) / weights_sum);
			right = (int)(width_parent * list_weights_sum[i] / weights_sum);
		}
		// �d�݂��Ȃ�(��������)�ꍇ
		else {
			left = i * width_child;
			right = (i + 1) * width_child;
		}
		rects_child[i]->left = rect.left + left;
		rects_child[i]->right = rect.left + right;
		rects_child[i]->top = rect.top;
		rects_child[i]->bottom = rect.bottom;
	}
}



//---------------------------------------------------------------------
//		�]����ݒ肷��
//---------------------------------------------------------------------
void cve::Rectangle::set_margin(int left, int top, int right, int bottom)
{
	rect.left += left;
	rect.top += top;
	rect.right -= right;
	rect.bottom -= bottom;

	if (rect.left > rect.right)
		rect.left = rect.right = (rect.left + rect.right) / 2;
	if (rect.top > rect.bottom)
		rect.top = rect.bottom = (rect.top + rect.bottom) / 2;
}



//---------------------------------------------------------------------
//		�N���C�A���g����X�N���[��
//---------------------------------------------------------------------
void cve::Rectangle::client_to_screen(HWND hwnd)
{
	POINT pt[2];
	pt[0] = {
		rect.left,
		rect.top
	};
	pt[1] = {
		rect.right,
		rect.bottom
	};
	::ClientToScreen(hwnd, &pt[0]);
	::ClientToScreen(hwnd, &pt[1]);
	rect = {
		pt[0].x, pt[0].y,
		pt[1].x, pt[1].y,
	};
}



//---------------------------------------------------------------------
//		�X�N���[������N���C�A���g
//---------------------------------------------------------------------
void cve::Rectangle::screen_to_client(HWND hwnd)
{
	POINT pt[2];
	pt[0] = {
		rect.left,
		rect.top
	};
	pt[1] = {
		rect.right,
		rect.bottom
	};
	::ScreenToClient(hwnd, &pt[0]);
	::ScreenToClient(hwnd, &pt[1]);
	rect = {
		pt[0].x, pt[0].y,
		pt[1].x, pt[1].y,
	};
}