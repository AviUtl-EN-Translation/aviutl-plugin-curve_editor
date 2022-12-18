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
	ratio = MINMAX_LIMIT(ratio, 0.0, 1.0);

	return st + get_bezier_value(ratio, ctpts) * (ed - st) / (double)CVE_GRAPH_RESOLUTION;
}