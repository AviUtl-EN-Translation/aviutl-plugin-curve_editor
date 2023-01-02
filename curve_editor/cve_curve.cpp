//----------------------------------------------------------------------------------
//		Curve Editor
//		�\�[�X�t�@�C�� (�J�[�u)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"

#define CVE_GR_POINT_LINE_THICKNESS		0.2f
#define CVE_GRAPH_POINT_CONTRAST		3
#define CVE_GRAPH_POINT_DASH			42
#define CVE_GRAPH_POINT_DASH_BLANK		24



//---------------------------------------------------------------------
//		������
//---------------------------------------------------------------------
void cve::Curve::init(Static_Array<Curve_Points, CVE_POINT_MAX>& points)
{
	// �ǉ����鐧��_
	Curve_Points pt_add[2];
	
	pt_add[0].type = Curve_Points::Default_Left;
	pt_add[0].pt_center = { 0, 0 };
	pt_add[0].pt_right = {
		(int)(CVE_GRAPH_RESOLUTION * CVE_POINT_DEFAULT_1),
		(int)(CVE_GRAPH_RESOLUTION * CVE_POINT_DEFAULT_1)
	};
	pt_add[0].pt_left = { 0, 0 };

	points.PushBack(pt_add[0]);


	pt_add[1].type = Curve_Points::Default_Right;

	pt_add[1].pt_center = {
		CVE_GRAPH_RESOLUTION,
		CVE_GRAPH_RESOLUTION
	};

	pt_add[1].pt_left = {
		(int)(CVE_GRAPH_RESOLUTION * CVE_POINT_DEFAULT_2),
		(int)(CVE_GRAPH_RESOLUTION * CVE_POINT_DEFAULT_2)
	};

	pt_add[1].pt_right = {
		CVE_GRAPH_RESOLUTION,
		CVE_GRAPH_RESOLUTION
	};

	points.PushBack(pt_add[1]);
}



//---------------------------------------------------------------------
//		�|�C���g���ړ�
//---------------------------------------------------------------------
void cve::Curve_Type_ID::move_point(int index, const POINT& pt_graph, bool init)
{
	static POINT		handle_prev_right,
						handle_next_left,
						handle_left,
						handle_right;

	Point_Address		tmp;

	static double		agl_prev, agl_next,
						agl_left, agl_right;


	// �g������_�łȂ��ꍇ�e��
	if (ctpts[index].type != Curve_Points::Extended)
		return;
						

	// �n���h���ʒu&�p�x���L��
	if (init) {
		handle_prev_right = ctpts[index - 1].pt_right;
		handle_next_left = ctpts[index + 1].pt_left;

		// ���E�̃n���h���̑��΍��W
		handle_left = {
			ctpts[index].pt_left.x - ctpts[index].pt_center.x,
			ctpts[index].pt_left.y - ctpts[index].pt_center.y,
		};
		handle_right = {
			ctpts[index].pt_right.x - ctpts[index].pt_center.x,
			ctpts[index].pt_right.y - ctpts[index].pt_center.y,
		};

		// ���ׂ̃|�C���g�̉E�n���h���̊p�x���擾
		tmp.index = index - 1;
		tmp.position = Point_Address::Right;

		agl_prev = get_handle_angle(tmp);

		// �E�ׂ̃|�C���g�̍��n���h���̊p�x���擾
		tmp.index = index + 1;
		tmp.position = Point_Address::Left;

		agl_next = get_handle_angle(tmp);

		// ���̃n���h���̊p�x���擾
		tmp.index = index;
		tmp.position = Point_Address::Left;

		agl_left = get_handle_angle(tmp);

		// �E�̃n���h���̊p�x���擾
		tmp.index = index;
		tmp.position = Point_Address::Right;

		agl_right = get_handle_angle(tmp);
	}


	// ����    ---o---
	ctpts[index].pt_center.x = MINMAX_LIMIT(
		pt_graph.x,
		ctpts[index - 1].pt_center.x + 1,
		ctpts[index + 1].pt_center.x - 1
	);

	ctpts[index].pt_center.y = pt_graph.y;

	// ��   O-----o--
	ctpts[index].pt_left.x = ctpts[index].pt_center.x + handle_left.x;
	ctpts[index].pt_left.y = pt_graph.y + handle_left.y;

	// �E       --o-----O
	ctpts[index].pt_right.x = ctpts[index].pt_center.x + handle_right.x;
	ctpts[index].pt_right.y = pt_graph.y + handle_right.y;


	// ���E���[�̃n���h���␳
	//��   O-----o--
	tmp.index = index;
	tmp.position = Point_Address::Left;
	correct_handle(tmp, agl_left);

	// �E       --o-----O
	tmp.index = index;
	tmp.position = Point_Address::Right;
	correct_handle(tmp, agl_right);

	ctpts[index - 1].pt_right = handle_prev_right;
	ctpts[index + 1].pt_left = handle_next_left;

	// �E       --o-----O
	// (�O�̐���_�Q)
	tmp.index = index - 1;
	tmp.position = Point_Address::Right;

	correct_handle(tmp, agl_prev);

	// ��   O-----o--
	// (���̐���_�Q)
	tmp.index = index + 1;
	tmp.position = Point_Address::Left;

	correct_handle(tmp, agl_next);
}



//---------------------------------------------------------------------
//		�n���h�����ړ�
//---------------------------------------------------------------------
void cve::Curve::move_handle(const Point_Address& pt_address, const POINT& pt_graph, bool init)
{
	static bool shift_key, alt_key, ctrl_key, ctrl_shift_key;
	static POINT pt_lock;
	static double length, length_opposite;
	POINT pt_result;
	Point_Address tmp;
	int left_side, right_side, top, bottom;

	// ���̎��_�̃L�[�̉�����Ԃ𒲂ׂ邱�Ƃ��ł���
	const bool move_symmetrically = ::GetAsyncKeyState(VK_SHIFT) < 0 && ::GetAsyncKeyState(VK_CONTROL) < 0;
	const bool is_angle_locked = ::GetAsyncKeyState(VK_MENU) < 0;
	const bool snapped = ::GetAsyncKeyState(VK_SHIFT) < 0 && ::GetAsyncKeyState(VK_CONTROL) >= 0;
	const bool is_length_locked = ::GetAsyncKeyState(VK_SHIFT) >= 0 && ::GetAsyncKeyState(VK_CONTROL) < 0;


	// ���E�̓_�ȊO�Ȃ�e��
	if (pt_address.position != Point_Address::Left && pt_address.position != Point_Address::Right)
		return;


	// �L�^
	if (init) {
		if (pt_address.position == Point_Address::Left)
			length = DISTANCE(
				ctpts[pt_address.index].pt_center,
				ctpts[pt_address.index].pt_right
			);
		else if (pt_address.position == Point_Address::Right)
			length = DISTANCE(
				ctpts[pt_address.index].pt_center,
				ctpts[pt_address.index].pt_left
			);
	}

	if (pt_address.position == Point_Address::Left) {
		left_side = ctpts[pt_address.index - 1].pt_center.x;
		right_side = ctpts[pt_address.index].pt_center.x;
		top = ctpts[pt_address.index].pt_center.y;
		bottom = ctpts[pt_address.index - 1].pt_center.y;
	}
	else if (pt_address.position == Point_Address::Right) {
		left_side = ctpts[pt_address.index].pt_center.x;
		right_side = ctpts[pt_address.index + 1].pt_center.x;
		top = ctpts[pt_address.index + 1].pt_center.y;
		bottom = ctpts[pt_address.index].pt_center.y;
	}


	// Alt�L�[
	if (is_angle_locked) {
		// �L�^���̏���
		if (!alt_key) {
			alt_key = true;
			ctrl_key = false;
			shift_key = false;

			pt_lock = pt_graph;
		}

		pt_result.x = MINMAX_LIMIT(
			pt_graph.x,
			left_side,
			right_side
		);

		if (pt_address.position == Point_Address::Left) {
			if (pt_lock.x <= left_side)
				pt_result.y = pt_graph.y;
			else
				pt_result.y = top - (int)((right_side - pt_result.x) *
					(top - pt_lock.y) / (double)(right_side - pt_lock.x));
		}
		else if (pt_address.position == Point_Address::Right) {
			if (pt_lock.x >= right_side)
				pt_result.y = pt_graph.y;
			else
				pt_result.y = bottom + (int)((pt_result.x - left_side) * (pt_lock.y - bottom) / (double)(pt_lock.x - left_side));
		}
	}
	// Ctrl�L�[
	else if (is_length_locked) {
		if (!ctrl_key) {
			ctrl_key = true;
			alt_key = false;
			shift_key = false;

			pt_lock = pt_graph;
		}

		float angle;
		int result_y;
		const double handle_len = DISTANCE(pt_lock, ctpts[pt_address.index].pt_center);

		if (pt_address.position == Point_Address::Left) {
			angle = (float)std::atan2(top - pt_graph.y, right_side - pt_graph.x);

			pt_result.x = right_side - (int)(handle_len * std::cos(angle));

			result_y = top - (int)(handle_len * std::sin(angle));

			if (angle > CVE_MATH_PI * 0.5)
				pt_result.y = top - (int)(handle_len);

			else if (angle < -CVE_MATH_PI * 0.5)
				pt_result.y = top + (int)(handle_len);

			else
				pt_result.y = result_y;
		}
		else if (pt_address.position == Point_Address::Right) {
			angle = (float)std::atan2(pt_graph.y - bottom, pt_graph.x - left_side);

			pt_result.x = left_side + (int)(handle_len * std::cos(angle));

			result_y = bottom + (int)(handle_len * std::sin(angle));

			if (angle > CVE_MATH_PI * 0.5)
				pt_result.y = bottom + (int)(handle_len);

			else if (angle < -CVE_MATH_PI * 0.5)
				pt_result.y = bottom - (int)(handle_len);

			else
				pt_result.y = result_y;
		}
	}
	// Shift�L�[
	else if (snapped) {
		if (!shift_key) {
			shift_key = true;
			alt_key = false;
			ctrl_key = false;

			pt_lock = pt_graph;
		}

		pt_result.x = pt_graph.x;

		if (pt_lock.y >= (top + bottom) / 2)
			pt_result.y = top;
		else
			pt_result.y = bottom;
	}
	else {
		alt_key = false;
		ctrl_key = false;
		shift_key = false;

		pt_result = pt_graph;
	}


	// �n���h���̈ʒu��ݒ�
	set_handle_position(pt_address, pt_result, length, false, NULL);


	// ���������̃n���h���Ɠ����ɓ����Ƃ�
	if (move_symmetrically) {
		// ���������̃n���h���̍��W
		POINT pt_opposite;

		// ��   O-----o--
		if (pt_address.position == Point_Address::Left) {
			pt_opposite = {
				ctpts[pt_address.index - 1].pt_center.x + ctpts[pt_address.index].pt_center.x - pt_result.x,
				ctpts[pt_address.index - 1].pt_center.y + ctpts[pt_address.index].pt_center.y - pt_result.y,
			};

			// Ctrl�L�[��Shift�L�[���������u��
			if (!ctrl_shift_key) {
				ctrl_shift_key = true;

				// �������̃n���h���̃y�A�̃n���h���̒������L�^
				length_opposite = DISTANCE(
					ctpts[pt_address.index - 1].pt_center,
					ctpts[pt_address.index - 1].pt_left
				);
			};

			tmp.index = pt_address.index - 1;
			tmp.position = Point_Address::Right;
		}
		// �E       --o-----O
		else if (pt_address.position == Point_Address::Right) {
			pt_opposite = {
				ctpts[pt_address.index + 1].pt_center.x - (pt_result.x - ctpts[pt_address.index].pt_center.x),
				ctpts[pt_address.index + 1].pt_center.y - (pt_result.y - ctpts[pt_address.index].pt_center.y),
			};

			// Ctrl�L�[��Shift�L�[���������u��
			if (!ctrl_shift_key) {
				ctrl_shift_key = true;

				// �������̃n���h���̃y�A�̃n���h���̒������L�^
				length_opposite = DISTANCE(
					ctpts[pt_address.index + 1].pt_center,
					ctpts[pt_address.index + 1].pt_right
				);
			};

			tmp.index = pt_address.index + 1;
			tmp.position = Point_Address::Left;
		}

		// ���������̃n���h���̈ʒu��ݒ�
		set_handle_position(tmp, pt_opposite, length_opposite, false, NULL);
	}
	else
		ctrl_shift_key = false;
}



//---------------------------------------------------------------------
//		�n���h�������ۂɓ������Ă���Ƃ���
//---------------------------------------------------------------------
void cve::Curve::set_handle_position(const Point_Address& pt_address, const POINT& pt_graph, double length, bool use_angle, double angle)
{
	Point_Address		tmp;
	double				agl_tmp = use_angle ? angle : get_handle_angle(pt_address);

	// �����̓_�Ȃ�e��
	if (pt_address.position == Point_Address::Center)
		return;


	switch (pt_address.position) {
		//��   O-----o--
	case Point_Address::Left:
		if (ctpts[pt_address.index].type == Curve_Points::Default_Left)
			return;

		// �n���h���𓮂���
		ctpts[pt_address.index].pt_left.x = MINMAX_LIMIT(
			pt_graph.x,
			ctpts[pt_address.index - 1].pt_center.x,
			ctpts[pt_address.index].pt_center.x
		);
		ctpts[pt_address.index].pt_left.y = pt_graph.y;

		//����(�p�x)
		if (g_config.align_handle && ctpts[pt_address.index].type == Curve_Points::Extended) {
			tmp.index = pt_address.index;
			tmp.position = Point_Address::Right;

			set_handle_angle(tmp, agl_tmp + CVE_MATH_PI, TRUE, length);
		}
		break;

		//�E       --o-----O
	case Point_Address::Right:
		if (ctpts[pt_address.index].type == Curve_Points::Default_Right)
			return;

		// �n���h���𓮂���
		ctpts[pt_address.index].pt_right.x = MINMAX_LIMIT(
			pt_graph.x,
			ctpts[pt_address.index].pt_center.x,
			ctpts[pt_address.index + 1].pt_center.x
		);
		ctpts[pt_address.index].pt_right.y = pt_graph.y;

		//����(�p�x)
		if (g_config.align_handle && ctpts[pt_address.index].type == Curve_Points::Extended) {
			tmp.index = pt_address.index;
			tmp.position = Point_Address::Left;

			set_handle_angle(tmp, agl_tmp + CVE_MATH_PI, TRUE, length);
		}
		break;
	}
}



//---------------------------------------------------------------------
//		�|�C���g�E�n���h����ǉ�
//---------------------------------------------------------------------
void cve::Curve_Type_ID::add_point(const POINT& pt_graph)
{
	int index = 0;

	// �E�|�C���g�̌����ő�
	// �E�ǉ�����|�C���g��X���W���͈͊O
	// �̂����ꂩ�̏ꍇ
	if (ctpts.size >= CVE_POINT_MAX ||
		!ISINRANGE(pt_graph.x, 0, CVE_GRAPH_RESOLUTION))
		return;

	// �ǉ�����|�C���g�������牽�ԖڂɂȂ邩
	for (int i = 0; i < (int)ctpts.size; i++) {
		// �����̃|�C���g��X���W������Ă����ꍇ
		if (ctpts[i].pt_center.x == pt_graph.x)
			return;

		else if (ctpts[i].pt_center.x > pt_graph.x) {
			index = i;
			break;
		}
	}

	Curve_Points pt_add;
	pt_add.type = Curve_Points::Extended;
	pt_add.pt_center = pt_graph;

	//�r���[�̏k�ڂɍ��킹�ăn���h���̃f�t�H���g�̒�����ύX
	pt_add.pt_left = {
		pt_graph.x - (int)(CVE_HANDLE_LENGTH_DEFAULT / g_view_info.scale.x),
		pt_graph.y
	};
	pt_add.pt_right = {
		pt_graph.x + (int)(CVE_HANDLE_LENGTH_DEFAULT / g_view_info.scale.x),
		pt_graph.y
	};

	// ����_�}��
	ctpts.Insert(pt_add, index);

	//���E�̓_�����ׂ̒����̓_��荶/�E�ɏo�Ă�����C��
	if (ctpts[index].pt_left.x < ctpts[index - 1].pt_center.x)
		ctpts[index].pt_left.x = ctpts[index - 1].pt_center.x;

	if (ctpts[index].pt_right.x > ctpts[index + 1].pt_center.x)
		ctpts[index].pt_right.x = ctpts[index + 1].pt_center.x;

	Point_Address tmp;
	//���ׂ̍��E�̓_�������̓_���E/���ɏo�Ă�����C��
	tmp.index = index - 1;
	tmp.position = Point_Address::Right;

	correct_handle(tmp, get_handle_angle(tmp));

	tmp.index = index + 1;
	tmp.position = Point_Address::Left;

	correct_handle(tmp, get_handle_angle(tmp));

}



//---------------------------------------------------------------------
//		�|�C���g���폜
//---------------------------------------------------------------------
void cve::Curve_Type_ID::delete_point(const POINT& pt_client)
{
	Point_Address pt_address;
	pt_in_ctpt(pt_client, &pt_address);
	if (pt_address.position == Point_Address::Null)
		return;
	for (int i = 1; i < (int)ctpts.size - 1; i++) {
		if (pt_address.index == i) {
			ctpts.Erase(i);
			break;
		}
	}
}



//---------------------------------------------------------------------
//		�|�C���g���N���A
//---------------------------------------------------------------------
void cve::Curve::clear(Static_Array<Curve_Points, CVE_POINT_MAX>& points)
{
	points.Clear();
	init(points);
}



//---------------------------------------------------------------------
//		�w�肵�����W���|�C���g�E�n���h���̓����ɑ��݂��Ă��邩
//---------------------------------------------------------------------
void cve::Curve::pt_in_ctpt(const POINT& pt_client, Point_Address* pt_address)
{
	RECT center, left, right;

	for (int i = 0; i < (int)ctpts.size; i++) {
		center = {
			(LONG)to_client(ctpts[i].pt_center).x - CVE_POINT_BOX_WIDTH,
			(LONG)to_client(ctpts[i].pt_center).y - CVE_POINT_BOX_WIDTH,
			(LONG)to_client(ctpts[i].pt_center).x + CVE_POINT_BOX_WIDTH,
			(LONG)to_client(ctpts[i].pt_center).y + CVE_POINT_BOX_WIDTH
		};
		left = {
			(LONG)to_client(ctpts[i].pt_left).x - CVE_POINT_BOX_WIDTH,
			(LONG)to_client(ctpts[i].pt_left).y - CVE_POINT_BOX_WIDTH,
			(LONG)to_client(ctpts[i].pt_left).x + CVE_POINT_BOX_WIDTH,
			(LONG)to_client(ctpts[i].pt_left).y + CVE_POINT_BOX_WIDTH
		};
		right = {
			(LONG)to_client(ctpts[i].pt_right).x - CVE_POINT_BOX_WIDTH,
			(LONG)to_client(ctpts[i].pt_right).y - CVE_POINT_BOX_WIDTH,
			(LONG)to_client(ctpts[i].pt_right).x + CVE_POINT_BOX_WIDTH,
			(LONG)to_client(ctpts[i].pt_right).y + CVE_POINT_BOX_WIDTH
		};

		// �|�C���g�ォ�n���h����̂Ƃ�
		if (::PtInRect(&center, pt_client) && ctpts[i].type == Curve_Points::Extended) {
			if (::PtInRect(&left, pt_client) && ctpts[i].type != Curve_Points::Default_Left) {
				if (ctpts[i].pt_center.x == ctpts[i].pt_left.x && ctpts[i].pt_center.y == ctpts[i].pt_left.y) {
					if (::GetAsyncKeyState(VK_CONTROL) < 0) {
						pt_address->index = i;
						pt_address->position = Point_Address::Center;
						return;
					}
				}
			}
			else if (::PtInRect(&right, pt_client) && ctpts[i].type != Curve_Points::Default_Right) {
				if (ctpts[i].pt_center.x == ctpts[i].pt_right.x && ctpts[i].pt_center.y == ctpts[i].pt_right.y) {
					if (::GetAsyncKeyState(VK_CONTROL) < 0) {
						pt_address->index = i;
						pt_address->position = Point_Address::Center;
						return;
					}
				}
			}
			else {
				pt_address->index = i;
				pt_address->position = Point_Address::Center;
				return;
			}
		}
		// �n���h����̂Ƃ�
		if (::PtInRect(&left, pt_client) && ctpts[i].type != Curve_Points::Default_Left) {
			pt_address->index = i;
			pt_address->position = Point_Address::Left;
			return;
		}
		else if (::PtInRect(&right, pt_client) && ctpts[i].type != Curve_Points::Default_Right) {
			pt_address->index = i;
			pt_address->position = Point_Address::Right;
			return;
		}
	}
	pt_address->index = NULL;
	pt_address->position = Point_Address::Null;
}



//---------------------------------------------------------------------
//		�n���h���̊p�x���擾
//---------------------------------------------------------------------
double cve::Curve::get_handle_angle(const Point_Address& pt_address)
{
	double angle;
	int dstx, dsty;

	switch (pt_address.position) {
		// ��
	case Point_Address::Left:
		dstx = ctpts[pt_address.index].pt_left.x - ctpts[pt_address.index].pt_center.x;
		dsty = ctpts[pt_address.index].pt_left.y - ctpts[pt_address.index].pt_center.y;
		break;

		// �E
	case Point_Address::Right:
		dstx = ctpts[pt_address.index].pt_right.x - ctpts[pt_address.index].pt_center.x;
		dsty = ctpts[pt_address.index].pt_right.y - ctpts[pt_address.index].pt_center.y;
		break;

	default:
		return 0;
	}

	// x, y���Ƃ���0�̂Ƃ�
	if (dstx == 0 && dsty == 0) {
		// ��
		if (pt_address.position == Point_Address::Left)
			return CVE_MATH_PI;
		// �E
		else if (pt_address.position == Point_Address::Right)
			return 0;
	}

	angle = std::atan2(dsty, dstx);
	return angle;
}



//---------------------------------------------------------------------
//		�n���h���̊p�x��ݒ�
//---------------------------------------------------------------------
void cve::Curve::set_handle_angle(const Point_Address& pt_address, double angle, bool use_length, double lgth)
{
	double length;

	//��
	if (pt_address.position == Point_Address::Left &&
		ctpts[pt_address.index].type != 0) {
		length = use_length ? lgth : DISTANCE(
			ctpts[pt_address.index].pt_center,
			ctpts[pt_address.index].pt_left
		);

		ctpts[pt_address.index].pt_left.x =
			(LONG)(ctpts[pt_address.index].pt_center.x + std::cos(angle) * length);
		ctpts[pt_address.index].pt_left.y =
			(LONG)(ctpts[pt_address.index].pt_center.y + std::sin(angle) * length);

		correct_handle(pt_address, angle);
	}
	//�E
	else if (pt_address.position == Point_Address::Right &&
		ctpts[pt_address.index].type != 1) {
		length = use_length ? lgth : DISTANCE(
			ctpts[pt_address.index].pt_center,
			ctpts[pt_address.index].pt_right
		);

		ctpts[pt_address.index].pt_right.x =
			(LONG)(ctpts[pt_address.index].pt_center.x + std::cos(angle) * length);
		ctpts[pt_address.index].pt_right.y =
			(LONG)(ctpts[pt_address.index].pt_center.y + std::sin(angle) * length);

		correct_handle(pt_address, angle);
	}
	else return;
}



//---------------------------------------------------------------------
//		�w�肵���A�h���X�̃n���h��(�E����)���C�O�܂��͎��̃|�C���g�𒴂��Ă���ꍇ�ɁC
//		�n���h���̊p�x��ۂ��Ȃ���C������֐�
//		pt_address:	�w�肷�鐧��_�̃A�h���X
//		angle:		�ݒ肷��p�x
//---------------------------------------------------------------------
void cve::Curve::correct_handle(const Point_Address& pt_address, double angle)
{

	if (pt_address.position == Point_Address::Left) {
		//���̐���_���O�̐���_�Q�̒����̓_��荶���ɂ������Ƃ�
		if (ctpts[pt_address.index].pt_left.x < ctpts[pt_address.index - 1].pt_center.x) {
			ctpts[pt_address.index].pt_left.x = ctpts[pt_address.index - 1].pt_center.x;
			

			//�p�x��ۂ�
			ctpts[pt_address.index].pt_left.y = (LONG)
				(ctpts[pt_address.index].pt_center.y + std::tan(angle) *
					(ctpts[pt_address.index].pt_left.x - ctpts[pt_address.index].pt_center.x));
		}
	}
	else if (pt_address.position == Point_Address::Right) {
		if (ctpts[pt_address.index].pt_right.x > ctpts[pt_address.index + 1].pt_center.x) {
			ctpts[pt_address.index].pt_right.x = ctpts[pt_address.index + 1].pt_center.x;

			//�p�x��ۂ�
			ctpts[pt_address.index].pt_right.y = (LONG)
				(ctpts[pt_address.index].pt_center.y + std::tan(angle) *
					(ctpts[pt_address.index].pt_right.x - ctpts[pt_address.index].pt_center.x));
		}
	}
}



//---------------------------------------------------------------------
//		�J�[�u�𔽓]
//---------------------------------------------------------------------
void cve::Curve::reverse_curve()
{
	Static_Array<Curve_Points, CVE_POINT_MAX> ctpts_old;
	for (int i = 0; i < ctpts.size / 2.0f; i++) {
		ctpts_old.PushBack(ctpts[i]);

		ctpts[i].pt_center = {
			CVE_GRAPH_RESOLUTION - ctpts[ctpts.size - i - 1].pt_center.x,
			CVE_GRAPH_RESOLUTION - ctpts[ctpts.size - i - 1].pt_center.y
		};

		ctpts[i].pt_left = {
			CVE_GRAPH_RESOLUTION - ctpts[ctpts.size - i - 1].pt_right.x,
			CVE_GRAPH_RESOLUTION - ctpts[ctpts.size - i - 1].pt_right.y
		};

		ctpts[i].pt_right = {
			CVE_GRAPH_RESOLUTION - ctpts[ctpts.size - i - 1].pt_left.x,
			CVE_GRAPH_RESOLUTION - ctpts[ctpts.size - i - 1].pt_left.y
		};


		ctpts[ctpts.size - i - 1].pt_center = {
			CVE_GRAPH_RESOLUTION - ctpts_old[i].pt_center.x,
			CVE_GRAPH_RESOLUTION - ctpts_old[i].pt_center.y
		};

		ctpts[ctpts.size - i - 1].pt_left = {
			CVE_GRAPH_RESOLUTION - ctpts_old[i].pt_right.x,
			CVE_GRAPH_RESOLUTION - ctpts_old[i].pt_right.y
		};

		ctpts[ctpts.size - i - 1].pt_right = {
			CVE_GRAPH_RESOLUTION - ctpts_old[i].pt_left.x,
			CVE_GRAPH_RESOLUTION - ctpts_old[i].pt_left.y
		};
	}
}



//---------------------------------------------------------------------
//		�x�W�F�Ȑ���`��
//---------------------------------------------------------------------
void cve::Curve::draw_bezier(
	Bitmap_Buffer* bitmap_buffer,
	const Float_Point& stpt,
	const Float_Point& ctpt1,
	const Float_Point& ctpt2,
	const Float_Point& edpt,
	float thickness
)
{
	ID2D1GeometrySink* sink;
	ID2D1PathGeometry* bezier;
	static ID2D1StrokeStyle* style = nullptr;

	if (style == nullptr) {
		g_d2d1_factory->CreateStrokeStyle(
			D2D1::StrokeStyleProperties(
				D2D1_CAP_STYLE_ROUND,
				D2D1_CAP_STYLE_ROUND,
				D2D1_CAP_STYLE_ROUND,
				D2D1_LINE_JOIN_MITER,
				10.0f,
				D2D1_DASH_STYLE_SOLID,
				0.0f),
			NULL, NULL,
			&style
		);
	}

	g_d2d1_factory->CreatePathGeometry(&bezier);
	bezier->Open(&sink);
	sink->BeginFigure(D2D1::Point2F(stpt.x, stpt.y), D2D1_FIGURE_BEGIN_HOLLOW);
	sink->AddBezier(D2D1::BezierSegment(
		D2D1::Point2F(ctpt1.x, ctpt1.y),
		D2D1::Point2F(ctpt2.x, ctpt2.y),
		D2D1::Point2F(edpt.x, edpt.y)
	));
	sink->EndFigure(D2D1_FIGURE_END_OPEN);
	sink->Close();

	if (bezier)
		g_render_target->DrawGeometry(bezier, bitmap_buffer->brush, thickness, style);
}



//---------------------------------------------------------------------
//		�n���h����`��
//---------------------------------------------------------------------
void cve::Curve::draw_handle(
	Bitmap_Buffer* bitmap_buffer,
	const Float_Point& st,
	const Float_Point& ed,
	int drawing_mode,
	int draw_option)
{
	static ID2D1StrokeStyle* style = nullptr;

	Float_Point st_new = st;
	Float_Point ed_new = ed;

	const float handle_thickness = (drawing_mode == CVE_DRAW_CURVE_REGULAR) ? CVE_HANDLE_THICKNESS : CVE_HANDLE_THICKNESS_PRESET;
	const float point_size = (drawing_mode == CVE_DRAW_CURVE_REGULAR) ? CVE_POINT_SIZE : CVE_PONINT_SIZE_PRESET;
	const float handle_size = (drawing_mode == CVE_DRAW_CURVE_REGULAR) ? CVE_HANDLE_SIZE : CVE_HANDLE_SIZE_PRESET;
	const float handle_circle_line = (drawing_mode == CVE_DRAW_CURVE_REGULAR) ? CVE_HANDLE_CIRCLE_LINE : CVE_HANDLE_CIRCLE_LINE_PRESET;

	if (drawing_mode == CVE_DRAW_CURVE_REGULAR) {
		subtract_length(&st_new, ed, st, draw_option == CVE_DRAW_HANDLE_ONLY ? CVE_SUBTRACT_LENGTH : CVE_SUBTRACT_LENGTH_2);
		subtract_length(&ed_new, st_new, ed, CVE_SUBTRACT_LENGTH);
	}

	g_d2d1_factory->CreateStrokeStyle(
		D2D1::StrokeStyleProperties(
			D2D1_CAP_STYLE_ROUND,
			D2D1_CAP_STYLE_ROUND,
			D2D1_CAP_STYLE_ROUND,
			D2D1_LINE_JOIN_MITER,
			10.0f,
			D2D1_DASH_STYLE_SOLID,
			0.0f),
		NULL, NULL,
		&style
	);

	if (drawing_mode != CVE_DRAW_CURVE_TRACE) {
		if (draw_option != CVE_DRAW_POINT_ONLY) {
			// �n���h���̒���
			g_render_target->DrawLine(
				D2D1::Point2F(st_new.x, st_new.y),
				D2D1::Point2F(ed_new.x, ed_new.y),
				bitmap_buffer->brush, handle_thickness
			);

			// �n���h���̐�[
			g_render_target->DrawEllipse(
				D2D1::Ellipse(
					D2D1::Point2F(ed.x, ed.y),
					handle_size, handle_size),
				bitmap_buffer->brush, CVE_HANDLE_CIRCLE_LINE
			);
		}

		// �n���h���̍���
		if (draw_option == CVE_DRAW_HANDLE_ONLY) {
			g_render_target->DrawEllipse(
				D2D1::Ellipse(
					D2D1::Point2F(st.x, st.y),
					handle_size, handle_size),
				bitmap_buffer->brush, CVE_HANDLE_CIRCLE_LINE
			);
		}
		else {
			g_render_target->FillEllipse(
				D2D1::Ellipse(
					D2D1::Point2F(st.x, st.y),
					point_size, point_size),
				bitmap_buffer->brush
			);
		}
	}
}



//---------------------------------------------------------------------
//		�|�C���g�Ɉ������_����`��
//---------------------------------------------------------------------
void cve::Curve::draw_dash_line(Bitmap_Buffer* bitmap_buffer, const RECT& rect_wnd, int pt_idx)
{
	static ID2D1StrokeStyle* style_dash;
	const float dashes[] = { CVE_GRAPH_POINT_DASH, CVE_GRAPH_POINT_DASH_BLANK };

	if (style_dash == nullptr) {
		g_d2d1_factory->CreateStrokeStyle(
			D2D1::StrokeStyleProperties(
				D2D1_CAP_STYLE_FLAT,
				D2D1_CAP_STYLE_FLAT,
				D2D1_CAP_STYLE_ROUND,
				D2D1_LINE_JOIN_MITER,
				10.0f,
				D2D1_DASH_STYLE_CUSTOM,
				0.0f),
			dashes,
			ARRAYSIZE(dashes),
			&style_dash
		);
	}

	// �[�_�ȊO�̐���_�Ɉ������_��
	bitmap_buffer->brush->SetColor(D2D1::ColorF(TO_BGR(CONTRAST(INVERT(g_theme[g_config.theme].bg_graph), CVE_GRAPH_POINT_CONTRAST))));

	if (pt_idx > 0) {
		g_render_target->DrawLine(
			D2D1::Point2F(to_client(ctpts[pt_idx].pt_center).x, 0),
			D2D1::Point2F(to_client(ctpts[pt_idx].pt_center).x, (float)rect_wnd.bottom),
			bitmap_buffer->brush, CVE_GR_POINT_LINE_THICKNESS, style_dash
		);
	}
}



//---------------------------------------------------------------------
//		�J�[�u��`��
//---------------------------------------------------------------------
void cve::Curve::draw_curve(Bitmap_Buffer* bitmap_buffer, const RECT& rect_wnd, int drawing_mode)
{
	COLORREF handle_color;
	COLORREF curve_color;

	const bool is_preset = drawing_mode == CVE_DRAW_CURVE_PRESET;

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


	for (int i = 0; i < (int)ctpts.size - 1; i++)
	{
		// �[�_�ȊO�̐���_�Ɉ������_��
		if (drawing_mode == CVE_DRAW_CURVE_REGULAR) {
			draw_dash_line(bitmap_buffer, rect_wnd, i);
		}

		// �x�W�F�Ȑ���`��
		bitmap_buffer->brush->SetColor(D2D1::ColorF(TO_BGR(curve_color)));
		draw_bezier(bitmap_buffer,
			is_preset ? to_preset(ctpts[i].pt_center) : to_client(ctpts[i].pt_center),
			is_preset ? to_preset(ctpts[i].pt_right) : to_client(ctpts[i].pt_right),
			is_preset ? to_preset(ctpts[i + 1].pt_left) : to_client(ctpts[i + 1].pt_left),
			is_preset ? to_preset(ctpts[i + 1].pt_center) : to_client(ctpts[i + 1].pt_center),
			CVE_CURVE_THICKNESS
		);

		//�n���h���̕`��
		if (g_config.show_handle) {
			bitmap_buffer->brush->SetColor(D2D1::ColorF(TO_BGR(handle_color)));
			draw_handle(bitmap_buffer,
				is_preset ? to_preset(ctpts[i].pt_center) : to_client(ctpts[i].pt_center),
				is_preset ? to_preset(ctpts[i].pt_right) : to_client(ctpts[i].pt_right),
				drawing_mode, NULL
			);
			draw_handle(bitmap_buffer,
				is_preset ? to_preset(ctpts[i + 1].pt_center) : to_client(ctpts[i + 1].pt_center),
				is_preset ? to_preset(ctpts[i + 1].pt_left) : to_client(ctpts[i + 1].pt_left),
				drawing_mode, NULL
			);
		}
	}
}



//---------------------------------------------------------------------
//		�f�[�^���L�����ǂ���
//---------------------------------------------------------------------
bool cve::Curve::is_data_valid()
{
	return ISINRANGEEQ(ctpts.size, 2, CVE_POINT_MAX);
}



//---------------------------------------------------------------------
//		1�����l��ǂݎ��
//---------------------------------------------------------------------
bool cve::Curve::read_number(int number, Static_Array<Curve_Points, CVE_POINT_MAX>& points)
{
	// -2147483647 ~  -12368443�F�x�W�F���g�p
	//   -12368442 ~         -1�FID���g�p
	//           0             �F�s�g�p
	//           1 ~   12368442�FID���g�p
	//    12368443 ~ 2147483646�F�x�W�F���g�p
	//  2147483647             �F�s�g�p

	int64_t num;
	if (number <= -12368443) {
		num = (int64_t)number + (int64_t)2147483647;
	}
	else if (12368443 <= number && number <= 2147483646) {
		num = (int64_t)number + (int64_t)2122746762;
	}
	else return false;

	clear(points);

	points[1].pt_left.y = (int32_t)(num / 6600047);
	points[1].pt_left.x = (int32_t)((num - (int64_t)points[1].pt_left.y * 6600047) / 65347);
	points[0].pt_right.y = (int32_t)((num - ((int64_t)points[1].pt_left.y * 6600047 + (int64_t)points[1].pt_left.x * 65347)) / 101);
	points[0].pt_right.x = (int32_t)((num - ((int64_t)points[1].pt_left.y * 6600047 + (int64_t)points[1].pt_left.x * 65347)) % 101);
	points[0].pt_right.x *= CVE_GRAPH_RESOLUTION / 100;
	points[0].pt_right.y *= CVE_GRAPH_RESOLUTION / 100;
	points[1].pt_left.x *= CVE_GRAPH_RESOLUTION / 100;
	points[1].pt_left.y *= CVE_GRAPH_RESOLUTION / 100;
	points[0].pt_right.y -= (int32_t)(2.73 * CVE_GRAPH_RESOLUTION);
	points[1].pt_left.y -= (int32_t)(2.73 * CVE_GRAPH_RESOLUTION);

	return true;
}



//---------------------------------------------------------------------
//		�x�W�F�̒l���v�Z
//---------------------------------------------------------------------
double cve::Curve::get_bezier_value(double ratio, Static_Array<Curve_Points, CVE_POINT_MAX>& points)
{
	// �i����0~1�͈̔͊O�ł������ꍇ
	if (!ISINRANGEEQ(ratio, 0, 1))
		return 0;

	// �i���ɑ��������Ԃ𒲂ׂ�
	for (int i = 0; i < (int)points.size - 1; i++) {
		if (ISINRANGEEQ(ratio, points[i].pt_center.x / (double)CVE_GRAPH_RESOLUTION, points[i + 1].pt_center.x / (double)CVE_GRAPH_RESOLUTION)) {
			// ��Ԃ̒���(ratio�Ɠ����X�P�[��)
			double range = (points[i + 1].pt_center.x - points[i].pt_center.x) / (double)CVE_GRAPH_RESOLUTION;
			// ��Ԃ��Ƃ̐i���̑��Βl(0~1)
			double ratio2 = (ratio - points[i].pt_center.x / (double)CVE_GRAPH_RESOLUTION) / range;
			// ��Ԃ��Ƃ̐���_1��X���W(���Βl�A0~1)
			double x1 = (points[i].pt_right.x - points[i].pt_center.x) / (double)(points[i + 1].pt_center.x - points[i].pt_center.x);
			// ��Ԃ��Ƃ̐���_1��Y���W(���ۂ̒l)
			double y1 = points[i].pt_right.y - points[i].pt_center.y / (double)(points[i + 1].pt_center.y - points[i].pt_center.y);
			// ��Ԃ��Ƃ̐���_2��X���W(���Βl�A0~1)
			double x2 = (points[i + 1].pt_left.x - points[i].pt_center.x) / (double)(points[i + 1].pt_center.x - points[i].pt_center.x);
			// ��Ԃ��Ƃ̐���_2��Y���W(���Βl)
			double y2 = points[i + 1].pt_left.y - points[i].pt_center.y / (double)(points[i + 1].pt_center.y - points[i].pt_center.y);

			// ��Ԃ��Ƃ̎n�l�A�I�l(���Βl�ł͂Ȃ��A�O���t�̒l)
			double st2 = points[i].pt_center.y;
			double ed2 = points[i + 1].pt_center.y;

			// �x�W�F�̌v�Z
			double tl = 0;
			double tr = 1;
			double ta = 0.5 * (tl + tr);
			double xta;
			for (int j = 0; j < 10; j++) {
				xta = (1 - 3 * x2 + 3 * x1) * std::pow(ta, 3) + (x2 - 2 * x1) * 3 * std::pow(ta, 2) + 3 * x1 * ta;
				if (ratio2 < xta) tr = ta;
				else tl = ta;
				ta = 0.5 * (tl + tr);
			}
			return std::pow(ta, 3) * (ed2 - st2 - 3 * y2 + 3 * y1) + 3 * std::pow(ta, 2) * (y2 - 2 * y1 + st2) + 3 * ta * (y1 - st2) + st2;
		}
	}
	return 0;
}