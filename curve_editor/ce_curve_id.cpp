//----------------------------------------------------------------------------------
//		Curve Editor
//		�\�[�X�t�@�C�� (ID���[�h�ł̃J�[�u�̊֐�)
//		VC++ 2022
//----------------------------------------------------------------------------------

#include "ce_header.hpp"


//---------------------------------------------------------------------
//		����_�Q��ǉ�
//---------------------------------------------------------------------
void ce::Curve_ID::add_point(POINT gr_pt)
{
	int index = 0;
	Point_Address tmp;
	if (control_points.size() >= CE_POINT_MAX) return;
	for (int i = 0; i < control_points.size(); i++) {
		if (control_points[i].pt_center.x == gr_pt.x)
			return;
		else if (control_points[i].pt_center.x > gr_pt.x) {
			index = i;
			break;
		}
	}
	Points_ID control_point_add;
	control_point_add.type = 2;
	control_point_add.pt_center = gr_pt;
	//�r���[�̏k�ڂɍ��킹�ăn���h���̃f�t�H���g�̒�����ύX
	control_point_add.pt_left = { gr_pt.x - (int)(CE_HANDLE_DEF_L / g_disp_info.scale.x), gr_pt.y};
	control_point_add.pt_right = { gr_pt.x + (int)(CE_HANDLE_DEF_L / g_disp_info.scale.x), gr_pt.y};
	control_points.insert(control_points.begin() + index, control_point_add);

	//���E�̓_�����ׂ̒����̓_��荶/�E�ɏo�Ă�����C��
	if (control_points[index].pt_left.x < control_points[index - 1].pt_center.x)
		control_points[index].pt_left.x = control_points[index - 1].pt_center.x;
	if (control_points[index].pt_right.x > control_points[index + 1].pt_center.x)
		control_points[index].pt_right.x = control_points[index + 1].pt_center.x;

	//���ׂ̍��E�̓_�������̓_���E/���ɏo�Ă�����C��
	
	tmp = { index - 1, CTPT_RIGHT };
	correct_handle(tmp, get_handle_angle(tmp));
	
	tmp = { index + 1, CTPT_LEFT };
	correct_handle(tmp, get_handle_angle(tmp));
	
}


//---------------------------------------------------------------------
//		����_�Q���폜
//---------------------------------------------------------------------
void ce::Curve_ID::delete_point(POINT cl_pt)
{
	Point_Address address = pt_in_control_points(cl_pt);
	if (!address.position) return;
	for (int i = 1; i < control_points.size() - 1; i++) {
		if (address.index == i) {
			control_points.erase(control_points.begin() + i);
			break;
		}
	}
}


POINT ce::Curve_ID::get_point(Point_Address address)
{
	POINT result = { -1, -1 };
		switch (address.position) {
		case 1://����
			result = control_points[address.index].pt_center;
			break;

		case 2://��
			result = control_points[address.index].pt_left;
			break;

		case 3://�E
			result = control_points[address.index].pt_right;
			break;

		default:
			break;
	}
		return result;
}


//---------------------------------------------------------------------
//		����_(�Q)���ړ�
//---------------------------------------------------------------------
void ce::Curve_ID::move_point(Point_Address address, POINT gr_pt, BOOL bReset)
{
	static POINT prevright, nextleft, hdleft, hdright;
	Point_Address tmp;
	double agl_tmp;
	static double agl_prev, agl_next,
		agl_left, agl_right,
		len_left, len_right;

	//�n���h���ʒu&�p�x���L��
	if (bReset && control_points[address.index].type > 1) {
		prevright = control_points[address.index - 1].pt_right;
		nextleft = control_points[address.index + 1].pt_left;
		hdleft = {
		control_points[address.index].pt_left.x - control_points[address.index].pt_center.x,
		control_points[address.index].pt_left.y - control_points[address.index].pt_center.y,
		};
		hdright = {
			control_points[address.index].pt_right.x - control_points[address.index].pt_center.x,
			control_points[address.index].pt_right.y - control_points[address.index].pt_center.y,
		};
		tmp = { address.index - 1, CTPT_RIGHT };
		agl_prev = get_handle_angle(tmp);
		tmp = { address.index + 1, CTPT_LEFT };
		agl_next = get_handle_angle(tmp);
		tmp = { address.index, CTPT_LEFT };
		agl_left = get_handle_angle(tmp);
		tmp = { address.index, CTPT_RIGHT };
		agl_right = get_handle_angle(tmp);

		len_left = DISTANCE(control_points[address.index].pt_center, control_points[address.index].pt_left);
		len_right = DISTANCE(control_points[address.index].pt_center, control_points[address.index].pt_right);
	}
	switch (address.position) {
		//���� ---[]---
	case 1:
		if (control_points[address.index].type < 2) return;

		//���� ---[]---
		control_points[address.index].pt_center.x = MINMAXLIM(gr_pt.x,
			control_points[address.index - 1].pt_center.x + 1,
			control_points[address.index + 1].pt_center.x - 1);
		control_points[address.index].pt_center.y = gr_pt.y;

		//��   O-----[]
		control_points[address.index].pt_left.x = control_points[address.index].pt_center.x + hdleft.x;
		control_points[address.index].pt_left.y = gr_pt.y + hdleft.y;

		//�E  []-----O
		control_points[address.index].pt_right.x = control_points[address.index].pt_center.x + hdright.x;
		control_points[address.index].pt_right.y = gr_pt.y + hdright.y;


		//���E���[�̃n���h���␳
		if (control_points[address.index].type > 1) {//�g������_�������ꍇ
			tmp = { address.index, CTPT_LEFT };//��  O-----[]
			correct_handle(tmp, agl_left);
			tmp = { address.index, CTPT_RIGHT };//�E []-----O
			correct_handle(tmp, agl_right);

			control_points[address.index - 1].pt_right = prevright;
			control_points[address.index + 1].pt_left = nextleft;

			tmp = { address.index - 1, CTPT_RIGHT };//�E []-----O (�O�̐���_�Q)
			correct_handle(tmp, agl_prev);
			tmp = { address.index + 1, CTPT_LEFT };//��  O-----[] (���̐���_�Q)
			correct_handle(tmp, agl_next);
		}
		break;

	case 2://�� O-----[]
		if (control_points[address.index].type == 0) return;

		control_points[address.index].pt_left.x = MINMAXLIM(gr_pt.x,
			control_points[address.index - 1].pt_center.x,
			control_points[address.index].pt_center.x);
		control_points[address.index].pt_left.y = gr_pt.y;

		//����(�p�x)
		if (g_config.align_mode == 1) {
			agl_tmp = get_handle_angle(address);
			tmp = { address.index, CTPT_RIGHT };
			set_handle_angle(tmp, agl_tmp + MATH_PI, TRUE, len_right);
		}
		break;

	case 3://Right  o-----O
		if (control_points[address.index].type == 1) return;

		control_points[address.index].pt_right.x = MINMAXLIM(gr_pt.x,
			control_points[address.index].pt_center.x,
			control_points[address.index + 1].pt_center.x);
		control_points[address.index].pt_right.y = gr_pt.y;

		//����(�p�x)
		if (g_config.align_mode == 1) {
			agl_tmp = get_handle_angle(address);
			tmp = { address.index, CTPT_LEFT };
			set_handle_angle(tmp, agl_tmp + MATH_PI, TRUE, len_left);
		}
		break;
	}
}


//---------------------------------------------------------------------
//		�w�肵�����W������_�Q�̓����ɑ��݂��Ă��邩
//---------------------------------------------------------------------
ce::Point_Address ce::Curve_ID::pt_in_control_points(POINT cl_pt)
{
	RECT rcCenter, rcLeft, rcRight;
	for (int i = 0; i < control_points.size(); i++) {
		rcCenter = {
			(LONG)to_client(control_points[i].pt_center).x - CE_POINT_RANGE,
			(LONG)to_client(control_points[i].pt_center).y - CE_POINT_RANGE,
			(LONG)to_client(control_points[i].pt_center).x + CE_POINT_RANGE,
			(LONG)to_client(control_points[i].pt_center).y + CE_POINT_RANGE
		};
		rcLeft = {
			(LONG)to_client(control_points[i].pt_left).x - CE_POINT_RANGE,
			(LONG)to_client(control_points[i].pt_left).y - CE_POINT_RANGE,
			(LONG)to_client(control_points[i].pt_left).x + CE_POINT_RANGE,
			(LONG)to_client(control_points[i].pt_left).y + CE_POINT_RANGE
		};
		rcRight = {
			(LONG)to_client(control_points[i].pt_right).x - CE_POINT_RANGE,
			(LONG)to_client(control_points[i].pt_right).y - CE_POINT_RANGE,
			(LONG)to_client(control_points[i].pt_right).x + CE_POINT_RANGE,
			(LONG)to_client(control_points[i].pt_right).y + CE_POINT_RANGE
		};

		if (PtInRect(&rcLeft, cl_pt) && control_points[i].type != 0)
			return { i, CTPT_LEFT };
		else if (PtInRect(&rcRight, cl_pt) && control_points[i].type != 1)
			return { i, CTPT_RIGHT };
		else if (PtInRect(&rcCenter, cl_pt) && control_points[i].type > 1)
			return { i, CTPT_CENTER };
	}
	ce::Point_Address control_point_null = { 0, CTPT_NULL };
	return control_point_null;
}


//---------------------------------------------------------------------
//		�n���h���̊p�x���擾
//---------------------------------------------------------------------
double ce::Curve_ID::get_handle_angle(Point_Address address)
{
	double angle;
	int dstx, dsty;
	switch (address.position) {
	case 2:
		dstx = control_points[address.index].pt_left.x - control_points[address.index].pt_center.x;
		dsty = control_points[address.index].pt_left.y - control_points[address.index].pt_center.y;
		break;
	case 3:
		dstx = control_points[address.index].pt_right.x - control_points[address.index].pt_center.x;
		dsty = control_points[address.index].pt_right.y - control_points[address.index].pt_center.y;
		break;
	default:
		return 0;
	}
	//x, y���Ƃ���0�̂Ƃ�
	if (dstx == 0 && dsty == 0) {
		//��
		if (address.position == CTPT_LEFT)
			return MATH_PI;
		//�E
		else if (address.position == CTPT_RIGHT)
			return 0;
	}
	angle = std::atan2(dsty, dstx);
	return angle;
}


//---------------------------------------------------------------------
//		�n���h���̊p�x��ݒ�
//---------------------------------------------------------------------
void ce::Curve_ID::set_handle_angle(Point_Address address, double angle, BOOL bLength, double lgth)
{
	double length;
	//��-left
	if (address.position == 2 && control_points[address.index].type != 0) {
		length = bLength ? lgth : DISTANCE(
			control_points[address.index].pt_center,
			control_points[address.index].pt_left);

		control_points[address.index].pt_left.x = control_points[address.index].pt_center.x + std::cos(angle) * length;
		control_points[address.index].pt_left.y = control_points[address.index].pt_center.y + std::sin(angle) * length;
		correct_handle(address, angle);
	}
	//�E-right
	else if (address.position == CTPT_RIGHT &&
			control_points[address.index].type != 1) {
		length = bLength ? lgth : DISTANCE(
			control_points[address.index].pt_center,
			control_points[address.index].pt_right);

		control_points[address.index].pt_right.x = control_points[address.index].pt_center.x + std::cos(angle) * length;
		control_points[address.index].pt_right.y = control_points[address.index].pt_center.y + std::sin(angle) * length;
		correct_handle(address, angle);
	}
	else return;
}


//---------------------------------------------------------------------
//		�w�肵���A�h���X�̃n���h��(�E����)���C�O�܂��͎��̐���_�Q�̒����̓_�𒴂��Ă���ꍇ�ɁC
//		�n���h���̊p�x��ۂ��Ȃ���C������֐�
//		address:	�w�肷�鐧��_�̃A�h���X
//		x:			�w�肷��X���W
//		bAngle:		�p�x��ݒ肷�邩�ǂ���(FALSE�Ȃ�w�肵���A�h���X�̃n���h���̊p�x���擾����)
//		agl:		�ݒ肷��p�x
//---------------------------------------------------------------------
void ce::Curve_ID::correct_handle(Point_Address address, double angle)
{
	switch (address.position) {
	case 2://��   O-----[]
		//���̐���_���O�̐���_�Q�̒����̓_��荶���ɂ������Ƃ�
		if (get_point(address).x < control_points[address.index - 1].pt_center.x) {
			control_points[address.index].pt_left.x = control_points[address.index - 1].pt_center.x;
			//�p�x��ۂ�
			control_points[address.index].pt_left.y =
				control_points[address.index].pt_center.y + std::tan(angle) *
				(control_points[address.index].pt_left.x - control_points[address.index].pt_center.x);
		}
		break;
	case 3://�E  []-----O
		if (get_point(address).x > control_points[address.index + 1].pt_center.x) {
			control_points[address.index].pt_right.x = control_points[address.index + 1].pt_center.x;
			//�p�x��ۂ�
			control_points[address.index].pt_right.y =
				control_points[address.index].pt_center.y + std::tan(angle) *
				(control_points[address.index].pt_right.x - control_points[address.index].pt_center.x);
		}
		break;
	default:
		return;
	}
}


//---------------------------------------------------------------------
//		�O���t�𔽓]
//---------------------------------------------------------------------
void ce::Curve_ID::reverse_points()
{
	std::vector<Points_ID> control_points_old;
	for (int i = 0; i < control_points.size() / 2.0; i++) {
		control_points_old.emplace_back(control_points[i]);
		control_points[i].pt_center.x = CE_GR_RES - control_points[control_points.size() - i - 1].pt_center.x;
		control_points[i].pt_center.y = CE_GR_RES - control_points[control_points.size() - i - 1].pt_center.y;
		control_points[i].pt_left.x = CE_GR_RES - control_points[control_points.size() - i - 1].pt_right.x;
		control_points[i].pt_left.y = CE_GR_RES - control_points[control_points.size() - i - 1].pt_right.y;
		control_points[i].pt_right.x = CE_GR_RES - control_points[control_points.size() - i - 1].pt_left.x;
		control_points[i].pt_right.y = CE_GR_RES - control_points[control_points.size() - i - 1].pt_left.y;

		control_points[control_points.size() - i - 1].pt_center.x = CE_GR_RES - control_points_old[i].pt_center.x;
		control_points[control_points.size() - i - 1].pt_center.y = CE_GR_RES - control_points_old[i].pt_center.y;
		control_points[control_points.size() - i - 1].pt_left.x = CE_GR_RES - control_points_old[i].pt_right.x;
		control_points[control_points.size() - i - 1].pt_left.y = CE_GR_RES - control_points_old[i].pt_right.y;
		control_points[control_points.size() - i - 1].pt_right.x = CE_GR_RES - control_points_old[i].pt_left.x;
		control_points[control_points.size() - i - 1].pt_right.y = CE_GR_RES - control_points_old[i].pt_left.y;
	}

}


//---------------------------------------------------------------------
//		Get Value
//---------------------------------------------------------------------
double ce::Curve_ID::get_value(double ratio, double st, double ed)
{
	if (!ISINRANGE(ratio, 0, 1)) return 0;
	for (int i = 0; i < control_points.size() - 1; i++) {
		if (ISINRANGE(ratio, control_points[i].pt_center.x / (double)CE_GR_RES, control_points[i + 1].pt_center.x / (double)CE_GR_RES)) {
			double range = (control_points[i + 1].pt_center.x - control_points[i].pt_center.x) / (double)CE_GR_RES;
			double x1 = (control_points[i].pt_right.x - control_points[i].pt_center.x) / (control_points[i + 1].pt_center.x - control_points[i].pt_center.x);
			double y1 = (control_points[i].pt_right.y - control_points[i].pt_center.y) / (control_points[i + 1].pt_center.y - control_points[i].pt_center.y);
			double x2 = (control_points[i + 1].pt_center.x - control_points[i + 1].pt_left.x) / (control_points[i + 1].pt_center.x - control_points[i].pt_center.x);
			double y2 = (control_points[i + 1].pt_center.y - control_points[i + 1].pt_left.y) / (control_points[i + 1].pt_center.y - control_points[i].pt_center.y);
			double st2 = st + control_points[i].pt_center.y * (ed - st) / (double)CE_GR_RES;
			double ed2 = st + control_points[i + 1].pt_center.y * (ed - st) / (double)CE_GR_RES;
			double tl = 0;
			double tr = 1;
			double ta = 0.5 * (tl + tr);
			double xta;
			for (int j = 1; j < 10; j++) {
				xta = (1 - 3 * x2 + 3 * x1) * std::pow(ta, 3) + (x2 - 2 * x1) * 3 * std::pow(ta, 2) + 3 * x1 * ta;
				if (ratio < xta) tr = ta;
				else tl = ta;
				ta = 0.5 * (tl + tr);
			}
			return std::pow(ta, 3) * (ed2 - st2 - 3 * y2 + 3 * y1) + 3 * std::pow(ta, 2) * (y2 - 2 * y1 + st2) + 3 * ta * (y1 - st2) + st2;
		}
	}
}