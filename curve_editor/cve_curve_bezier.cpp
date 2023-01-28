//----------------------------------------------------------------------------------
//		Curve Editor
//		�\�[�X�t�@�C�� (�x�W�F���[�h�̃J�[�u)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"



//---------------------------------------------------------------------
//		���l�𐶐�
//---------------------------------------------------------------------
int cve::Curve_Bezier::create_number()
{
	int result;
	aului::Point<float> ptf[2];
	POINT pt[2];

	ptf[0].x = std::clamp(
		ctpts[0].pt_right.x / (float)CVE_GRAPH_RESOLUTION,
		0.f, 1.f
	);

	ptf[0].y = std::clamp(
		ctpts[0].pt_right.y / (float)CVE_GRAPH_RESOLUTION,
		MIN_Y, MAX_Y
	);

	ptf[1].x = std::clamp(
		ctpts[1].pt_left.x / (float)CVE_GRAPH_RESOLUTION,
		0.f, 1.f
	);

	ptf[1].y = std::clamp(
		ctpts[1].pt_left.y / (float)CVE_GRAPH_RESOLUTION,
		MIN_Y, MAX_Y
	);

	for (int i = 0; i < 2; i++) {
		pt[i].x = (int)std::round(ptf[i].x * 100);
		pt[i].y = (int)std::round(ptf[i].y * 100);
	}
	// �v�Z
	result = 6600047 * (pt[1].y + 273) + 65347 * pt[1].x + 101 * (pt[0].y + 273) + pt[0].x - 2147483647;
	if (result >= -12368442)
		result += 24736885;
	return result;
}



//---------------------------------------------------------------------
//		�p�����[�^�𐶐�(","�ŋ�؂�)
//---------------------------------------------------------------------
std::string cve::Curve_Bezier::create_parameters()
{
	aului::Point<float> pt;
	std::string strx, stry, result;

	pt.x = std::clamp(
		(float)(std::round(ctpts[0].pt_right.x * 100 / (double)CVE_GRAPH_RESOLUTION) * 0.01f),
		0.f, 1.f
	);
	pt.y = std::clamp(
		(float)(std::round(ctpts[0].pt_right.y * 100 / (double)CVE_GRAPH_RESOLUTION) * 0.01f),
		MIN_Y, MAX_Y
	);

	strx = std::to_string(pt.x);
	stry = std::to_string(pt.y);
	strx.erase(4);

	if (ctpts[0].pt_right.y < 0)
		stry.erase(5);
	else
		stry.erase(4);
	result += strx + ", " + stry + ", ";

	pt.x = std::clamp(
		(float)(std::round(ctpts[1].pt_left.x * 100 / (double)CVE_GRAPH_RESOLUTION) * 0.01f),
		0.f, 1.f
	);
	pt.y = std::clamp(
		(float)(std::round(ctpts[1].pt_left.y * 100 / (double)CVE_GRAPH_RESOLUTION) * 0.01f),
		MIN_Y, MAX_Y
	);

	strx = std::to_string(pt.x);
	stry = std::to_string(pt.y);
	strx.erase(4);

	if (ctpts[1].pt_left.y < 0)
		stry.erase(5);
	else
		stry.erase(4);
	result += strx + ", " + stry;

	return result;
}



//---------------------------------------------------------------------
//		�p�����[�^��ǂݎ��
//---------------------------------------------------------------------
bool cve::Curve_Bezier::read_parameters(LPCTSTR param, Static_Array<Curve_Points, CVE_POINT_MAX>& points)
{
	const std::regex regex_param(R"(^((\d+ *, *)|(\d*\.\d* *, *))((-?\d+ *, *)|(-?\d*\.\d* *, *))((\d+ *, *)|(\d*\.\d* *, *))((-?\d+ *)|(-?\d*\.\d* *))$)");

	if (std::regex_match(param, regex_param)) {
		std::vector<std::string> vec = cve::split(param, ',');

		float values[4];

		values[0] = std::clamp(std::stof(vec[0]), 0.f, 1.f);
		values[1] = std::clamp(
			std::stof(vec[1]),
			MIN_Y, MAX_Y
		);
		values[2] = std::clamp(std::stof(vec[2]), 0.f, 1.f);
		values[3] = std::clamp(
			std::stof(vec[3]),
			MIN_Y, MAX_Y
		);

		points[0].pt_right.x = (int)(values[0] * CVE_GRAPH_RESOLUTION);
		points[0].pt_right.y = (int)(values[1] * CVE_GRAPH_RESOLUTION);
		points[1].pt_left.x = (int)(values[2] * CVE_GRAPH_RESOLUTION);
		points[1].pt_left.y = (int)(values[3] * CVE_GRAPH_RESOLUTION);

		return true;
	}
	else
		return false;
}



//---------------------------------------------------------------------
//		�X�N���v�g�ɓn���l�𐶐�
//---------------------------------------------------------------------
double cve::Curve_Bezier::create_result(int number, double ratio, double st, double ed)
{
	// -2147483647 ~  -12368443�F�x�W�F���g�p
	//   -12368442 ~         -1�FID���g�p
	//           0             �F�s�g�p
	//           1 ~   12368442�FID���g�p
	//    12368443 ~ 2147483646�F�x�W�F���g�p
	//  2147483647             �F�s�g�p
	
	Static_Array<Curve_Points, CVE_POINT_MAX> ctpts_buffer;

	ratio = std::clamp(ratio, 0., 1.);

	if (!read_number(number, ctpts_buffer))
		return st + (ed - st) * ratio;

	return st + get_bezier_value(ratio, ctpts_buffer) * (ed - st) / (double)CVE_GRAPH_RESOLUTION;
}