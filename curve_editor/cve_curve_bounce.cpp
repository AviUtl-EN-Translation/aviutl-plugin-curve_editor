//----------------------------------------------------------------------------------
//		Curve Editor
//		�\�[�X�t�@�C�� (�e�����[�h�̃J�[�u)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"



//---------------------------------------------------------------------
//		������
//---------------------------------------------------------------------
void cve::Curve_Bounce::initialize()
{
	coef_bounce = 0.6;
	coef_time = 0.5;
}



//---------------------------------------------------------------------
//		�o�E���X�̊֐�
//---------------------------------------------------------------------
double func1(double ratio, double e)
{
	return std::floor(std::log((e - 1) * ratio + 1) / std::log(e));
}

double func2(double ratio, double e)
{
	return ratio + 0.5 + 1.0 / (e - 1.0) - (e + 1.0) * std::pow(e, func1(ratio + 0.5, e)) / (2.0 * e - 2.0);
}

double cve::Curve_Bounce::func_bounce(double ratio, double e, double t, double st, double ed)
{
	ratio = MINMAX_LIMIT(ratio, 0, 1);
	const double lim_val = t * (1 / (1.0 - e) - 0.5);
	double result;
	e = MAX_LIMIT(e, 0.999);

	if (lim_val > 1.0) {
		int n = (int)(std::log(1 + (e - 1.0) * (1 / t + 0.5)) / std::log(e));
		if (ratio < t * ((std::pow(e, n) - 1.0) / (e - 1.0) - 0.5))
			result = (ed - st) * (4 * std::pow(func2(ratio / t, e), 2) - std::pow(e, 2 * func1(ratio / t + 0.5, e))) + ed;
		else
			result = ed;
	}
	else {
		if (ratio < lim_val)
			result = (ed - st) * (4 * std::pow(func2(ratio / t, e), 2) - std::pow(e, 2 * func1(ratio / t + 0.5, e))) + ed;
		else
			result = ed;
	}
	return result;
}



//---------------------------------------------------------------------
//		�w�肵�����W���|�C���g�E�n���h���̓����ɑ��݂��Ă��邩
//---------------------------------------------------------------------
void cve::Curve_Bounce::pt_in_ctpt(const POINT& pt_client, Point_Address* pt_address)
{
	POINT pt;

	param_to_pt(&pt);
	// �U���𒲐�����n���h��(��)
	const RECT rect_point = {
		(LONG)to_client(pt).x - CVE_POINT_RANGE,
		(LONG)to_client(pt).y - CVE_POINT_RANGE,
		(LONG)to_client(pt).x + CVE_POINT_RANGE,
		(LONG)to_client(pt).y + CVE_POINT_RANGE
	};

	pt_address->index = NULL;

	if (::PtInRect(&rect_point, pt_client))
		pt_address->position = Point_Address::Center;
	else
		pt_address->position = Point_Address::Null;
}



//---------------------------------------------------------------------
//		�J�[�u�𔽓]
//---------------------------------------------------------------------
void cve::Curve_Bounce::reverse_curve()
{
	reverse = !reverse;
}



//---------------------------------------------------------------------
//		�|�C���g���ړ�
//---------------------------------------------------------------------
void cve::Curve_Bounce::move_handle(const POINT& pt_graph)
{
	coef_bounce = pt_to_param(pt_graph, 0);
	coef_time = pt_to_param(pt_graph, 1);
}



//---------------------------------------------------------------------
//		�p�����[�^ -> �|�C���g
//---------------------------------------------------------------------
void cve::Curve_Bounce::param_to_pt(POINT* pt_graph)
{
	pt_graph->x = (int)((coef_time * (coef_bounce + 1) * 0.5) * CVE_GRAPH_RESOLUTION);
	pt_graph->y = (int)((1.0 - std::pow(coef_bounce, 2)) * CVE_GRAPH_RESOLUTION);
	if (reverse) {
		pt_graph->x = CVE_GRAPH_RESOLUTION - pt_graph->x;
		pt_graph->y = CVE_GRAPH_RESOLUTION - pt_graph->y;
	}
}



//---------------------------------------------------------------------
//		�|�C���g -> �p�����[�^
//		idx_param : �p�����[�^�̃C���f�b�N�X(0: coef_bounce, 1: coef_time)
//---------------------------------------------------------------------
double cve::Curve_Bounce::pt_to_param(const POINT& pt_graph, int idx_param)
{
	POINT pt;
	if (reverse) {
		pt.x = CVE_GRAPH_RESOLUTION - pt_graph.x;
		pt.y = CVE_GRAPH_RESOLUTION - pt_graph.y;
	}
	else
		pt = pt_graph;

	double e = std::sqrt(1.0 - MINMAX_LIMIT(pt.y, 1, CVE_GRAPH_RESOLUTION) / (double)CVE_GRAPH_RESOLUTION);
	double t = 2.0 * MINMAX_LIMIT(pt.x, 1, CVE_GRAPH_RESOLUTION) / ((e + 1.0) * CVE_GRAPH_RESOLUTION);
	if (idx_param == 0)
		return e;
	else
		return t;
}



//---------------------------------------------------------------------
//		�J�[�u��`��
//---------------------------------------------------------------------
void cve::Curve_Bounce::draw_curve(Bitmap_Buffer* bitmap_buffer, const RECT& rect_wnd, int drawing_mode)
{
	COLORREF handle_color;
	COLORREF curve_color;
	POINT pt_graph;

	if (drawing_mode == CVE_DRAW_CURVE_REGULAR) {
		handle_color = g_theme[g_config.theme].handle;
		curve_color = g_config.curve_color;
	}
	else if (drawing_mode == CVE_DRAW_CURVE_TRACE) {
		handle_color = g_theme[g_config.theme].handle;
		curve_color = g_theme[g_config.theme].curve_trace;
	}
	else {
		handle_color = g_theme[g_config.theme].handle_preset;
		curve_color = g_theme[g_config.theme].curve_preset;
	}

	float left_side = to_client(0, 0).x;
	float right_side = to_client(CVE_GRAPH_RESOLUTION, 0).x;

	// �J�[�u��`��
	if (right_side < 0 || left_side > rect_wnd.right) {
		return;
	}
	if (left_side < 0) {
		left_side = 0;
	}
	if (right_side > rect_wnd.right) {
		right_side = (float)rect_wnd.right;
	}

	bitmap_buffer->brush->SetColor(D2D1::ColorF(TO_BGR(curve_color)));

	double y1, y2;

	for (float x = left_side; x < right_side; x += CVE_DRAW_GRAPH_INCREASEMENT) {
		if (reverse) {
			y1 = func_bounce(1.0 - to_graphf(x, 0).x / (double)CVE_GRAPH_RESOLUTION, coef_bounce, coef_time, 1.0, 0.0);
			y2 = func_bounce(1.0 - to_graphf(x + CVE_DRAW_GRAPH_INCREASEMENT, 0).x / (double)CVE_GRAPH_RESOLUTION, coef_bounce, coef_time, 1.0, 0.0);
		}
		else {
			y1 = func_bounce(to_graphf(x, 0).x / (double)CVE_GRAPH_RESOLUTION, coef_bounce, coef_time, 0.0, 1.0);
			y2 = func_bounce(to_graphf(x + CVE_DRAW_GRAPH_INCREASEMENT, 0).x / (double)CVE_GRAPH_RESOLUTION, coef_bounce, coef_time, 0.0, 1.0);
		}
		g_render_target->DrawLine(
			D2D1::Point2F(
				x,
				(to_clientf(0.0f, (float)(y1 * CVE_GRAPH_RESOLUTION)).y)),
			D2D1::Point2F(
				x + CVE_DRAW_GRAPH_INCREASEMENT,
				(to_clientf(0.0f, (float)(y2 * CVE_GRAPH_RESOLUTION)).y)),
			bitmap_buffer->brush, CVE_CURVE_THICKNESS
		);
	}

	// �n���h���E�|�C���g��`��
	if (g_config.show_handle) {
		bitmap_buffer->brush->SetColor(D2D1::ColorF(TO_BGR(handle_color)));

		param_to_pt(&pt_graph);

		// �U�����E�����𒲐�����n���h��
		draw_handle(
			bitmap_buffer,
			to_client(pt_graph),
			to_client(pt_graph),
			drawing_mode, CVE_DRAW_HANDLE_ONLY
		);

		// �n�_
		draw_handle(
			bitmap_buffer,
			to_client(0, 0),
			to_client(0, 0),
			drawing_mode, CVE_DRAW_POINT_ONLY
		);
		// �I�_
		draw_handle(
			bitmap_buffer,
			to_client(CVE_GRAPH_RESOLUTION, CVE_GRAPH_RESOLUTION),
			to_client(CVE_GRAPH_RESOLUTION, CVE_GRAPH_RESOLUTION),
			drawing_mode, CVE_DRAW_POINT_ONLY
		);
	}
}



//---------------------------------------------------------------------
//		���l�𐶐�
//---------------------------------------------------------------------
int cve::Curve_Bounce::create_number()
{
	int result;
	int x = (int)((coef_time * (coef_bounce + 1) * 0.5) * 1000);
	int y = (int)((1.0 - std::pow(coef_bounce, 2)) * 1000);
	result = y + 1001 * x + 10211202;
	if (reverse)
		result *= -1;
	return result;
}



//---------------------------------------------------------------------
//		���l��ǂݎ��
//---------------------------------------------------------------------
bool cve::Curve_Bounce::read_number(int number, double* e, double* t, bool* rev)
{
	// -2147483647 ~  -11213203�F�s�g�p
	//   -11213202 ~  -10211202�F�o�E���X���g�p
	//   -10211201 ~         -1�F�U�����g�p
	//           0             �F�s�g�p
	//           1 ~   10211201�F�U�����g�p
	//    10211202 ~   11213202�F�o�E���X���g�p
	//    11213203 ~ 2147483647�F�s�g�p
	int num;

	if (ISINRANGEEQ(number, -11213202, -10211202)) {
		*rev = true;
		num = -number - 10211202;
	}
	else if (ISINRANGEEQ(number, 10211202, 11213202)) {
		*rev = false;
		num = number - 10211202;
	}
	else return false;

	int x, y;

	x = num / 1001;
	y = num - x * 1001;
	*e = std::sqrt(1.0 - y * 0.001);
	*t = 2.0 * 0.001 * x / (*e + 1.0);

	return true;
}



//---------------------------------------------------------------------
//		Lua�ɓn�����
//---------------------------------------------------------------------
double cve::Curve_Bounce::create_result(int number, double ratio, double st, double ed)
{
	double e, t;
	bool rev;

	ratio = MINMAX_LIMIT(ratio, 0.0, 1.0);

	if (!read_number(number, &e, &t, &rev))
		return st + (ed - st) * ratio;

	if (rev)
		return func_bounce(1.0 - ratio, e, t, ed, st);
	else
		return func_bounce(ratio, e, t, st, ed);
}