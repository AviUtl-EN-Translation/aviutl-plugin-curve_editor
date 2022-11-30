//----------------------------------------------------------------------------------
//		Curve Editor
//		�\�[�X�t�@�C�� (�}���`�x�W�F���[�h�̃J�[�u)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"



//---------------------------------------------------------------------
//		�X�N���v�g�ɓn���l�𐶐�
//---------------------------------------------------------------------
double cve::Curve_Multibezier::create_result(double ratio, double st, double ed)
{
	// �i����0~1�͈̔͊O�ł������ꍇ
	if (!ISINRANGEEQ(ratio, 0, 1))
		return 0;
	else
		return st + get_bezier_value(ratio, ctpts) * (ed - st) / (double)CVE_GRAPH_RESOLUTION;
}