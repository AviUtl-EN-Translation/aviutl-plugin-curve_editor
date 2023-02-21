//----------------------------------------------------------------------------------
//		Curve Editor
//		�\�[�X�t�@�C���iDirect2D�`��j
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"

#define CVE_GR_GRID_TH_L				.5f
#define CVE_GR_GRID_TH_B				1.f
#define CVE_GR_GRID_MIN					36



//---------------------------------------------------------------------
//		�O���t�̃O���b�h��`��
//---------------------------------------------------------------------
void cve::Cve_Paint_Object::draw_grid()
{
	aului::Color col_grid = g_theme[g_config.theme].bg_graph;
	col_grid.change_brightness(CVE_BR_GRID);
	brush->SetColor(D2D1::ColorF(col_grid.d2dcolor()));
	// 
	int kx = (int)std::floor(std::log(CVE_GRAPH_RESOLUTION * g_view_info.scale.x / (double)CVE_GR_GRID_MIN) / std::log(CVE_GRAPH_GRID_NUM));
	int ky = (int)std::floor(std::log(CVE_GRAPH_RESOLUTION * g_view_info.scale.y / (double)CVE_GR_GRID_MIN) / std::log(CVE_GRAPH_GRID_NUM));
	// �O���t�̘g���ɕ\�������O���b�h�̖{��
	int nx = std::max((int)std::pow(CVE_GRAPH_GRID_NUM, kx), 1);
	int ny = std::max((int)std::pow(CVE_GRAPH_GRID_NUM, ky), 1);
	// 
	float dx = (float)(CVE_GRAPH_RESOLUTION * g_view_info.scale.x) / (float)nx;
	float dy = (float)(CVE_GRAPH_RESOLUTION * g_view_info.scale.y) / (float)ny;
	int lx, ly;

	if (to_graph(0, 0).x >= 0)
		lx = (int)std::floor(to_graph(0, 0).x * nx / (double)CVE_GRAPH_RESOLUTION);
	else
		lx = (int)std::ceil(to_graph(0, 0).x * nx / (double)CVE_GRAPH_RESOLUTION);

	if (to_graph(0, 0).y >= 0)
		ly = (int)std::floor(to_graph(0, 0).y * ny / (double)CVE_GRAPH_RESOLUTION);
	else
		ly = (int)std::ceil(to_graph(0, 0).y * ny / (double)CVE_GRAPH_RESOLUTION);

	float ax = to_client(lx * CVE_GRAPH_RESOLUTION / (float)nx, 0.f).x;
	float ay = to_client(0.f, ly * CVE_GRAPH_RESOLUTION / (float)ny).y;
	float thickness;

	// �c����
	for (int i = 0; ax + dx * i <= rect.right; i++) {
		if ((lx + i) % CVE_GRAPH_GRID_NUM == 0)
			thickness = CVE_GR_GRID_TH_B;
		else
			thickness = CVE_GR_GRID_TH_L;
		p_render_target->DrawLine(
			D2D1::Point2F(ax + dx * i, 0),
			D2D1::Point2F(ax + dx * i, (float)rect.bottom),
			brush, thickness, NULL
		);
	}

	// ������
	for (int i = 0; ay + dy * i <= rect.bottom; i++) {
		if ((ly - i) % CVE_GRAPH_GRID_NUM == 0)
			thickness = CVE_GR_GRID_TH_B;
		else
			thickness = CVE_GR_GRID_TH_L;

		p_render_target->DrawLine(
			D2D1::Point2F(0, ay + dy * i),
			D2D1::Point2F((float)rect.right, ay + dy * i),
			brush, thickness, NULL
		);
	}
}



//---------------------------------------------------------------------
//		���E���h�G�b�W��`��
//---------------------------------------------------------------------
void cve::Cve_Paint_Object::draw_rounded_edge(int flag, float radius) {
	ID2D1GeometrySink* sink = nullptr;
	ID2D1PathGeometry* edge = nullptr;
	D2D1_POINT_2F pt_1, pt_2, pt_3;
	aului::Color col_edge = g_theme[g_config.theme].bg;

	D2D1_POINT_2F pts_1[] = {
		D2D1::Point2F((float)rect.left, (float)rect.top),
		D2D1::Point2F((float)rect.left, (float)rect.bottom),
		D2D1::Point2F((float)rect.right, (float)rect.top),
		D2D1::Point2F((float)rect.right, (float)rect.bottom)
	};

	D2D1_POINT_2F pts_2[] = {
		D2D1::Point2F((float)rect.left, (float)rect.top + radius),
		D2D1::Point2F((float)rect.left + radius, (float)rect.bottom),
		D2D1::Point2F((float)rect.right - radius, (float)rect.top),
		D2D1::Point2F((float)rect.right, (float)rect.bottom - radius)
	};

	D2D1_POINT_2F pts_3[] = {
		D2D1::Point2F((float)rect.left + radius, (float)rect.top),
		D2D1::Point2F((float)rect.left, (float)rect.bottom - radius),
		D2D1::Point2F((float)rect.right, (float)rect.top + radius),
		D2D1::Point2F((float)rect.right - radius, (float)rect.bottom)
	};

	p_factory->CreatePathGeometry(&edge);
	edge->Open(&sink);

	for (int i = 0; i < 4; i++) {
		if (flag & 1 << i) {
			pt_1 = pts_1[i];
			pt_2 = pts_2[i];
			pt_3 = pts_3[i];

			sink->BeginFigure(pt_1, D2D1_FIGURE_BEGIN_FILLED);
			sink->AddLine(pt_2);
			sink->AddArc(
				D2D1::ArcSegment(
					pt_3,
					D2D1::SizeF(radius, radius),
					0.f,
					D2D1_SWEEP_DIRECTION_CLOCKWISE,
					D2D1_ARC_SIZE_SMALL
				)
			);
			sink->EndFigure(D2D1_FIGURE_END_CLOSED);
		}
	}

	sink->Close();
	brush->SetColor(D2D1::ColorF(col_edge.d2dcolor()));
	if (is_safe(&edge))
		p_render_target->FillGeometry(edge, brush, NULL);

	release(&sink);
	release(&edge);
}



//---------------------------------------------------------------------
//		�x�W�F�Ȑ���`��
//---------------------------------------------------------------------
void cve::Cve_Paint_Object::draw_bezier(
	const aului::Point<float>& stpt,
	const aului::Point<float>& ctpt1,
	const aului::Point<float>& ctpt2,
	const aului::Point<float>& edpt,
	float thickness
)
{
	ID2D1GeometrySink* sink = nullptr;
	ID2D1PathGeometry* path_bezier = nullptr;
	ID2D1StrokeStyle* style = nullptr;

	g_p_factory->CreateStrokeStyle(
		D2D1::StrokeStyleProperties(
			D2D1_CAP_STYLE_ROUND,
			D2D1_CAP_STYLE_ROUND,
			D2D1_CAP_STYLE_ROUND,
			D2D1_LINE_JOIN_MITER,
			10.f,
			D2D1_DASH_STYLE_SOLID,
			0.f),
		NULL, NULL,
		&style
	);

	g_p_factory->CreatePathGeometry(&path_bezier);
	path_bezier->Open(&sink);
	sink->BeginFigure(D2D1::Point2F(stpt.x, stpt.y), D2D1_FIGURE_BEGIN_HOLLOW);
	sink->AddBezier(D2D1::BezierSegment(
		D2D1::Point2F(ctpt1.x, ctpt1.y),
		D2D1::Point2F(ctpt2.x, ctpt2.y),
		D2D1::Point2F(edpt.x, edpt.y)
	));
	sink->EndFigure(D2D1_FIGURE_END_OPEN);
	sink->Close();

	if (path_bezier)
		p_render_target->DrawGeometry(path_bezier, brush, thickness, style);

	release(&sink);
	release(&path_bezier);
	release(&style);
}



//---------------------------------------------------------------------
//		�n���h����`��
//---------------------------------------------------------------------
void cve::Cve_Paint_Object::draw_handle(
	const aului::Point<float>& st,
	const aului::Point<float>& ed,
	int drawing_mode,
	int draw_option
)
{
	ID2D1StrokeStyle* style = nullptr;

	aului::Point<float> st_new = st;
	aului::Point<float> ed_new = ed;

	const float handle_thickness = (drawing_mode == Curve::DRAW_CURVE_NORMAL) ? HANDLE_THICKNESS : HANDLE_THICKNESS_PRESET;
	const float point_size = (drawing_mode == Curve::DRAW_CURVE_NORMAL) ? POINT_SIZE : POINT_SIZE_PRESET;
	const float handle_size = (drawing_mode == Curve::DRAW_CURVE_NORMAL) ? HANDLE_SIZE : HANDLE_SIZE_PRESET;
	const float handle_circle_line = (drawing_mode == Curve::DRAW_CURVE_NORMAL) ? HANDLE_BORDER_THICKNESS : HANDLE_BORDER_THICKNESS;

	if (drawing_mode == Curve::DRAW_CURVE_NORMAL) {
		subtract_length(&st_new, ed, st, draw_option == Curve::DRAW_HANDLE_ONLY ? CVE_SUBTRACT_LENGTH : CVE_SUBTRACT_LENGTH_2);
		subtract_length(&ed_new, st_new, ed, CVE_SUBTRACT_LENGTH);
	}

	g_p_factory->CreateStrokeStyle(
		D2D1::StrokeStyleProperties(
			D2D1_CAP_STYLE_ROUND,
			D2D1_CAP_STYLE_ROUND,
			D2D1_CAP_STYLE_ROUND,
			D2D1_LINE_JOIN_MITER,
			10.f,
			D2D1_DASH_STYLE_SOLID,
			0.f),
		NULL, NULL,
		&style
	);

	if (drawing_mode != Curve::DRAW_CURVE_TRACE) {
		if (draw_option != Curve::DRAW_POINT_ONLY) {
			// �n���h���̒���
			p_render_target->DrawLine(
				D2D1::Point2F(st_new.x, st_new.y),
				D2D1::Point2F(ed_new.x, ed_new.y),
				brush, handle_thickness
			);

			// �n���h���̐�[
			p_render_target->DrawEllipse(
				D2D1::Ellipse(
					D2D1::Point2F(ed.x, ed.y),
					handle_size, handle_size),
				brush, HANDLE_BORDER_THICKNESS
			);
		}

		// �n���h���̍���
		if (draw_option == Curve::DRAW_HANDLE_ONLY) {
			p_render_target->DrawEllipse(
				D2D1::Ellipse(
					D2D1::Point2F(st.x, st.y),
					handle_size, handle_size),
				brush, HANDLE_BORDER_THICKNESS
			);
		}
		else {
			p_render_target->FillEllipse(
				D2D1::Ellipse(
					D2D1::Point2F(st.x, st.y),
					point_size, point_size),
				brush
			);
		}
	}
	release(&style);
}



//---------------------------------------------------------------------
//		���C���E�B���h�E��`��
//---------------------------------------------------------------------
void cve::Cve_Paint_Object::draw_panel_main(const RECT& rect_sepr)
{
	ID2D1StrokeStyle* style = nullptr;

	p_factory->CreateStrokeStyle(
		D2D1::StrokeStyleProperties(
			D2D1_CAP_STYLE_ROUND,
			D2D1_CAP_STYLE_ROUND,
			D2D1_CAP_STYLE_ROUND,
			D2D1_LINE_JOIN_MITER,
			10.f,
			D2D1_DASH_STYLE_SOLID,
			0.f),
		NULL, NULL,
		&style
	);

	const D2D1_POINT_2F line_start = {
		g_config.layout_mode == cve::Config::Vertical ?
			(rect_sepr.right + rect_sepr.left) * .5f - SEPARATOR_LINE_LENGTH :
			(float)(rect_sepr.left + CVE_SEPARATOR_WIDTH),
		g_config.layout_mode == cve::Config::Vertical ?
			(float)(rect_sepr.top + CVE_SEPARATOR_WIDTH) :
			(rect_sepr.top + rect_sepr.bottom) * .5f - SEPARATOR_LINE_LENGTH
	};

	const D2D1_POINT_2F line_end = {
		g_config.layout_mode == cve::Config::Vertical ?
			(rect_sepr.right + rect_sepr.left) * .5f + SEPARATOR_LINE_LENGTH :
			(float)(rect_sepr.left + CVE_SEPARATOR_WIDTH),
		g_config.layout_mode == cve::Config::Vertical ?
			(float)(rect_sepr.top + CVE_SEPARATOR_WIDTH) :
			(rect_sepr.top + rect_sepr.bottom) * .5f + SEPARATOR_LINE_LENGTH
	};

	if (p_render_target != nullptr) {
		p_render_target->BeginDraw();
		// �w�i��`��
		p_render_target->Clear(D2D1::ColorF(g_theme[g_config.theme].bg.d2dcolor()));

		brush->SetColor(D2D1::ColorF(g_theme[g_config.theme].sepr.d2dcolor()));

		if (brush)
			p_render_target->DrawLine(
				line_start,
				line_end,
				brush, SEPARATOR_LINE_WIDTH, style
			);

		p_render_target->EndDraw();
	}
	release(&style);
}



//---------------------------------------------------------------------
//		�p�l����`��
//---------------------------------------------------------------------
void cve::Cve_Paint_Object::draw_panel()
{
	if (is_safe(&p_render_target)) {
		p_render_target->BeginDraw();
		// �w�i��`��
		p_render_target->Clear(D2D1::ColorF(g_theme[g_config.theme].bg.d2dcolor()));
		p_render_target->EndDraw();
	}
}



//---------------------------------------------------------------------
//		�G�f�B�^�p�l����`��
//---------------------------------------------------------------------
void cve::Cve_Paint_Object::draw_panel_editor()
{
	aului::Color col_overlay = g_theme[g_config.theme].bg_graph;
	D2D1_RECT_F rect_left = {
		0.f,
		0.f,
		(float)g_view_info.origin.x,
		(float)rect.bottom
	};

	D2D1_RECT_F rect_right = {
		(float)(g_view_info.origin.x + g_view_info.scale.x * CVE_GRAPH_RESOLUTION),
		0.f,
		(float)rect.right,
		(float)rect.bottom,
	};

	D2D1_RECT_F rect_up = {
		(float)g_view_info.origin.x,
		0.f,
		(float)(g_view_info.origin.x + g_view_info.scale.x * CVE_GRAPH_RESOLUTION),
		to_client(0.f, Curve_Bezier::MAX_Y * CVE_GRAPH_RESOLUTION).y
	};

	D2D1_RECT_F rect_down = {
		(float)g_view_info.origin.x,
		to_client(0.f, Curve_Bezier::MIN_Y * CVE_GRAPH_RESOLUTION).y,
		(float)(g_view_info.origin.x + g_view_info.scale.x * CVE_GRAPH_RESOLUTION),
		(float)rect.bottom
	};


	//�`��
	if (is_safe(&p_render_target)) {
		p_render_target->BeginDraw();
		// �w�i��`��
		p_render_target->Clear(D2D1::ColorF(g_theme[g_config.theme].bg_graph.d2dcolor()));

		//�O���b�h
		draw_grid();

		col_overlay.change_brightness(CVE_BR_GR_INVALID);
		brush->SetColor(D2D1::ColorF(col_overlay.d2dcolor()));
		brush->SetOpacity(.5f);
		if (brush) {
			// X��0����1����̕������Â�����
			p_render_target->FillRectangle(&rect_left, brush);
			p_render_target->FillRectangle(&rect_right, brush);
			// �x�W�F���[�h�̂Ƃ�
			if (g_config.edit_mode == cve::Mode_Bezier) {
				p_render_target->FillRectangle(&rect_up, brush);
				p_render_target->FillRectangle(&rect_down, brush);
			}
		}
		brush->SetOpacity(1);

		// �ҏW���[�h�U�蕪��
		switch (g_config.edit_mode) {
			// �x�W�F���[�h�̂Ƃ�
		case Mode_Bezier:
			if (g_config.trace)
				g_curve_bezier_trace.draw_curve(this, rect, Curve::DRAW_CURVE_TRACE);

			g_curve_bezier.draw_curve(this, rect, Curve::DRAW_CURVE_NORMAL);
			break;

			// �x�W�F(����)���[�h�̂Ƃ�
		case Mode_Bezier_Multi:
			if (g_config.trace)
				g_curve_bezier_multi_trace.draw_curve(this, rect, Curve::DRAW_CURVE_TRACE);

			g_curve_bezier_multi[g_config.current_id.multi - 1].draw_curve(this, rect, Curve::DRAW_CURVE_NORMAL);
			break;

			// �l�w�胂�[�h�̂Ƃ�
		case Mode_Bezier_Value:
			if (g_config.trace)
				g_curve_bezier_value_trace.draw_curve(this, rect, Curve::DRAW_CURVE_TRACE);

			g_curve_bezier_value[g_config.current_id.value].draw_curve(this, rect, Curve::DRAW_CURVE_NORMAL);
			break;

			// �U�����[�h�̂Ƃ�
		case Mode_Elastic:
			if (g_config.trace)
				g_curve_elastic_trace.draw_curve(this, rect, Curve::DRAW_CURVE_TRACE);

			g_curve_elastic.draw_curve(this, rect, Curve::DRAW_CURVE_NORMAL);
			break;

			// �o�E���X���[�h�̂Ƃ�
		case Mode_Bounce:
			if (g_config.trace)
				g_curve_bounce_trace.draw_curve(this, rect, Curve::DRAW_CURVE_TRACE);

			g_curve_bounce.draw_curve(this, rect, Curve::DRAW_CURVE_NORMAL);
			break;
		}

		p_render_target->EndDraw();
	}
}