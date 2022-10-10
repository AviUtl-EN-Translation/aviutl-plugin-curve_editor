//----------------------------------------------------------------------------------
//		Curve Editor
//		�\�[�X�t�@�C�� (ID���[�h�ł̃J�[�u�̊֐�)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "ce_header.hpp"



//---------------------------------------------------------------------
//		������
//---------------------------------------------------------------------
void ce::Curve_ID::init()
{
	Points_ID pt_add[2];
	pt_add[0].type = 0;
	pt_add[0].pt_center = { 0, 0 };
	pt_add[0].pt_right = { (int)(CE_GR_RESOLUTION * 0.4), (int)(CE_GR_RESOLUTION * 0.4) };
	pt_add[0].pt_left = { 0, 0 };
	ctpts.push_back(pt_add[0]);

	pt_add[1].type = 1;
	pt_add[1].pt_center = { CE_GR_RESOLUTION, CE_GR_RESOLUTION };
	pt_add[1].pt_left = { (int)(CE_GR_RESOLUTION * 0.6), (int)(CE_GR_RESOLUTION * 0.6) };
	pt_add[1].pt_right = { CE_GR_RESOLUTION, CE_GR_RESOLUTION };
	ctpts.push_back(pt_add[1]);
}



//---------------------------------------------------------------------
//		�|�C���g�E�n���h����ǉ�
//---------------------------------------------------------------------
void ce::Curve_ID::add_point(POINT gr_pt)
{
	int index = 0;
	Point_Address tmp;
	if (ctpts.size >= CE_POINT_MAX) return;
	if (!ISINRANGE(gr_pt.x, 0, CE_GR_RESOLUTION)) return;
	for (int i = 0; i < (int)ctpts.size; i++) {
		if (ctpts[i].pt_center.x == gr_pt.x)
			return;
		else if (ctpts[i].pt_center.x > gr_pt.x) {
			index = i;
			break;
		}
	}
	Points_ID ctpt_add;
	ctpt_add.type = 2;
	ctpt_add.pt_center = gr_pt;
	//�r���[�̏k�ڂɍ��킹�ăn���h���̃f�t�H���g�̒�����ύX
	ctpt_add.pt_left = { gr_pt.x - (int)(CE_HANDLE_DEF_L / g_view_info.scale.x), gr_pt.y};
	ctpt_add.pt_right = { gr_pt.x + (int)(CE_HANDLE_DEF_L / g_view_info.scale.x), gr_pt.y};
	ctpts.insert(index, ctpt_add);

	//���E�̓_�����ׂ̒����̓_��荶/�E�ɏo�Ă�����C��
	if (ctpts[index].pt_left.x < ctpts[index - 1].pt_center.x)
		ctpts[index].pt_left.x = ctpts[index - 1].pt_center.x;
	if (ctpts[index].pt_right.x > ctpts[index + 1].pt_center.x)
		ctpts[index].pt_right.x = ctpts[index + 1].pt_center.x;

	//���ׂ̍��E�̓_�������̓_���E/���ɏo�Ă�����C��
	
	tmp = { index - 1, CTPT_RIGHT };
	correct_handle(tmp, get_handle_angle(tmp));
	
	tmp = { index + 1, CTPT_LEFT };
	correct_handle(tmp, get_handle_angle(tmp));
	
}


//---------------------------------------------------------------------
//		�|�C���g���폜
//---------------------------------------------------------------------
void ce::Curve_ID::delete_point(POINT cl_pt)
{
	Point_Address address = pt_in_ctpt(cl_pt);
	if (!address.position) return;
	for (int i = 1; i < (int)ctpts.size - 1; i++) {
		if (address.index == i) {
			ctpts.erase(i);
			break;
		}
	}
}



//---------------------------------------------------------------------
//		�|�C���g���擾
//---------------------------------------------------------------------
POINT ce::Curve_ID::get_point(Point_Address address)
{
	POINT result = { -1, -1 };
		switch (address.position) {
		case 1://����
			result = ctpts[address.index].pt_center;
			break;

		case 2://��
			result = ctpts[address.index].pt_left;
			break;

		case 3://�E
			result = ctpts[address.index].pt_right;
			break;

		default:
			break;
	}
		return result;
}



//---------------------------------------------------------------------
//		�|�C���g���N���A
//---------------------------------------------------------------------
void ce::Curve_ID::clear()
{
	ctpts.clear();
	init();
}



//---------------------------------------------------------------------
//		�|�C���g�E�n���h�����ړ�
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
	if (bReset && ctpts[address.index].type > 1) {
		prevright = ctpts[address.index - 1].pt_right;
		nextleft = ctpts[address.index + 1].pt_left;
		hdleft = {
		ctpts[address.index].pt_left.x - ctpts[address.index].pt_center.x,
		ctpts[address.index].pt_left.y - ctpts[address.index].pt_center.y,
		};
		hdright = {
			ctpts[address.index].pt_right.x - ctpts[address.index].pt_center.x,
			ctpts[address.index].pt_right.y - ctpts[address.index].pt_center.y,
		};
		tmp = { address.index - 1, CTPT_RIGHT };
		agl_prev = get_handle_angle(tmp);
		tmp = { address.index + 1, CTPT_LEFT };
		agl_next = get_handle_angle(tmp);
		tmp = { address.index, CTPT_LEFT };
		agl_left = get_handle_angle(tmp);
		tmp = { address.index, CTPT_RIGHT };
		agl_right = get_handle_angle(tmp);

		len_left = DISTANCE(ctpts[address.index].pt_center, ctpts[address.index].pt_left);
		len_right = DISTANCE(ctpts[address.index].pt_center, ctpts[address.index].pt_right);
	}
	switch (address.position) {
		//���� ---[]---
	case 1:
		if (ctpts[address.index].type < 2) return;

		//���� ---[]---
		ctpts[address.index].pt_center.x = MINMAXLIM(gr_pt.x,
			ctpts[address.index - 1].pt_center.x + 1,
			ctpts[address.index + 1].pt_center.x - 1);
		ctpts[address.index].pt_center.y = gr_pt.y;

		//��   O-----[]
		ctpts[address.index].pt_left.x = ctpts[address.index].pt_center.x + hdleft.x;
		ctpts[address.index].pt_left.y = gr_pt.y + hdleft.y;

		//�E  []-----O
		ctpts[address.index].pt_right.x = ctpts[address.index].pt_center.x + hdright.x;
		ctpts[address.index].pt_right.y = gr_pt.y + hdright.y;


		//���E���[�̃n���h���␳
		if (ctpts[address.index].type > 1) {//�g������_�������ꍇ
			tmp = { address.index, CTPT_LEFT };//��  O-----[]
			correct_handle(tmp, agl_left);
			tmp = { address.index, CTPT_RIGHT };//�E []-----O
			correct_handle(tmp, agl_right);

			ctpts[address.index - 1].pt_right = prevright;
			ctpts[address.index + 1].pt_left = nextleft;

			tmp = { address.index - 1, CTPT_RIGHT };//�E []-----O (�O�̐���_�Q)
			correct_handle(tmp, agl_prev);
			tmp = { address.index + 1, CTPT_LEFT };//��  O-----[] (���̐���_�Q)
			correct_handle(tmp, agl_next);
		}
		break;

	case 2://�� O-----[]
		if (ctpts[address.index].type == 0) return;

		ctpts[address.index].pt_left.x = MINMAXLIM(gr_pt.x,
			ctpts[address.index - 1].pt_center.x,
			ctpts[address.index].pt_center.x);
		ctpts[address.index].pt_left.y = gr_pt.y;

		//����(�p�x)
		if (g_config.align_handle) {
			agl_tmp = get_handle_angle(address);
			tmp = { address.index, CTPT_RIGHT };
			set_handle_angle(tmp, agl_tmp + MATH_PI, TRUE, len_right);
		}
		break;

	case 3://Right  origin-----O
		if (ctpts[address.index].type == 1) return;

		ctpts[address.index].pt_right.x = MINMAXLIM(gr_pt.x,
			ctpts[address.index].pt_center.x,
			ctpts[address.index + 1].pt_center.x);
		ctpts[address.index].pt_right.y = gr_pt.y;

		//����(�p�x)
		if (g_config.align_handle) {
			agl_tmp = get_handle_angle(address);
			tmp = { address.index, CTPT_LEFT };
			set_handle_angle(tmp, agl_tmp + MATH_PI, TRUE, len_left);
		}
		break;
	}
}



//---------------------------------------------------------------------
//		�w�肵�����W���|�C���g�E�n���h���̓����ɑ��݂��Ă��邩
//---------------------------------------------------------------------
ce::Point_Address ce::Curve_ID::pt_in_ctpt(POINT cl_pt)
{
	RECT rcCenter, rcLeft, rcRight;
	for (int i = 0; i < (int)ctpts.size; i++) {
		rcCenter = {
			(LONG)to_client(ctpts[i].pt_center).x - CE_POINT_RANGE,
			(LONG)to_client(ctpts[i].pt_center).y - CE_POINT_RANGE,
			(LONG)to_client(ctpts[i].pt_center).x + CE_POINT_RANGE,
			(LONG)to_client(ctpts[i].pt_center).y + CE_POINT_RANGE
		};
		rcLeft = {
			(LONG)to_client(ctpts[i].pt_left).x - CE_POINT_RANGE,
			(LONG)to_client(ctpts[i].pt_left).y - CE_POINT_RANGE,
			(LONG)to_client(ctpts[i].pt_left).x + CE_POINT_RANGE,
			(LONG)to_client(ctpts[i].pt_left).y + CE_POINT_RANGE
		};
		rcRight = {
			(LONG)to_client(ctpts[i].pt_right).x - CE_POINT_RANGE,
			(LONG)to_client(ctpts[i].pt_right).y - CE_POINT_RANGE,
			(LONG)to_client(ctpts[i].pt_right).x + CE_POINT_RANGE,
			(LONG)to_client(ctpts[i].pt_right).y + CE_POINT_RANGE
		};

		if (PtInRect(&rcLeft, cl_pt) && ctpts[i].type != 0)
			return { i, CTPT_LEFT };
		else if (PtInRect(&rcRight, cl_pt) && ctpts[i].type != 1)
			return { i, CTPT_RIGHT };
		else if (PtInRect(&rcCenter, cl_pt) && ctpts[i].type > 1)
			return { i, CTPT_CENTER };
	}
	ce::Point_Address ctpt_null = { 0, CTPT_NULL };
	return ctpt_null;
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
		dstx = ctpts[address.index].pt_left.x - ctpts[address.index].pt_center.x;
		dsty = ctpts[address.index].pt_left.y - ctpts[address.index].pt_center.y;
		break;
	case 3:
		dstx = ctpts[address.index].pt_right.x - ctpts[address.index].pt_center.x;
		dsty = ctpts[address.index].pt_right.y - ctpts[address.index].pt_center.y;
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
	if (address.position == 2 && ctpts[address.index].type != 0) {
		length = bLength ? lgth : DISTANCE(
			ctpts[address.index].pt_center,
			ctpts[address.index].pt_left);

		ctpts[address.index].pt_left.x = (LONG)(ctpts[address.index].pt_center.x + std::cos(angle) * length);
		ctpts[address.index].pt_left.y = (LONG)(ctpts[address.index].pt_center.y + std::sin(angle) * length);
		correct_handle(address, angle);
	}
	//�E-right
	else if (address.position == CTPT_RIGHT &&
			ctpts[address.index].type != 1) {
		length = bLength ? lgth : DISTANCE(
			ctpts[address.index].pt_center,
			ctpts[address.index].pt_right);

		ctpts[address.index].pt_right.x = (LONG)(ctpts[address.index].pt_center.x + std::cos(angle) * length);
		ctpts[address.index].pt_right.y = (LONG)(ctpts[address.index].pt_center.y + std::sin(angle) * length);
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
		if (get_point(address).x < ctpts[address.index - 1].pt_center.x) {
			ctpts[address.index].pt_left.x = ctpts[address.index - 1].pt_center.x;
			//�p�x��ۂ�
			ctpts[address.index].pt_left.y = (LONG)
				(ctpts[address.index].pt_center.y + std::tan(angle) *
					(ctpts[address.index].pt_left.x - ctpts[address.index].pt_center.x));
		}
		break;
	case 3://�E  []-----O
		if (get_point(address).x > ctpts[address.index + 1].pt_center.x) {
			ctpts[address.index].pt_right.x = ctpts[address.index + 1].pt_center.x;
			//�p�x��ۂ�
			ctpts[address.index].pt_right.y = (LONG)
				(ctpts[address.index].pt_center.y + std::tan(angle) *
					(ctpts[address.index].pt_right.x - ctpts[address.index].pt_center.x));
		}
		break;
	default:
		return;
	}
}



//---------------------------------------------------------------------
//		�J�[�u�𔽓]
//---------------------------------------------------------------------
void ce::Curve_ID::reverse_curve()
{
	std::vector<Points_ID> ctpts_old;
	for (int i = 0; i < ctpts.size / 2.0; i++) {
		ctpts_old.emplace_back(ctpts[i]);
		ctpts[i].pt_center.x = CE_GR_RESOLUTION - ctpts[ctpts.size - i - 1].pt_center.x;
		ctpts[i].pt_center.y = CE_GR_RESOLUTION - ctpts[ctpts.size - i - 1].pt_center.y;
		ctpts[i].pt_left.x = CE_GR_RESOLUTION - ctpts[ctpts.size - i - 1].pt_right.x;
		ctpts[i].pt_left.y = CE_GR_RESOLUTION - ctpts[ctpts.size - i - 1].pt_right.y;
		ctpts[i].pt_right.x = CE_GR_RESOLUTION - ctpts[ctpts.size - i - 1].pt_left.x;
		ctpts[i].pt_right.y = CE_GR_RESOLUTION - ctpts[ctpts.size - i - 1].pt_left.y;

		ctpts[ctpts.size - i - 1].pt_center.x = CE_GR_RESOLUTION - ctpts_old[i].pt_center.x;
		ctpts[ctpts.size - i - 1].pt_center.y = CE_GR_RESOLUTION - ctpts_old[i].pt_center.y;
		ctpts[ctpts.size - i - 1].pt_left.x = CE_GR_RESOLUTION - ctpts_old[i].pt_right.x;
		ctpts[ctpts.size - i - 1].pt_left.y = CE_GR_RESOLUTION - ctpts_old[i].pt_right.y;
		ctpts[ctpts.size - i - 1].pt_right.x = CE_GR_RESOLUTION - ctpts_old[i].pt_left.x;
		ctpts[ctpts.size - i - 1].pt_right.y = CE_GR_RESOLUTION - ctpts_old[i].pt_left.y;
	}

}



//---------------------------------------------------------------------
//		Get Value
//---------------------------------------------------------------------
double ce::Curve_ID::get_value(double ratio, double st, double ed)
{
	// �i����0~1�͈̔͊O�ł������ꍇ
	if (!ISINRANGEEQ(ratio, 0, 1))
		return 0;
	// �i���ɑ��������Ԃ𒲂ׂ�
	for (int i = 0; i < (int)ctpts.size - 1; i++) {
		if (ISINRANGEEQ(ratio, ctpts[i].pt_center.x / (double)CE_GR_RESOLUTION, ctpts[i + 1].pt_center.x / (double)CE_GR_RESOLUTION)) {
			double range = (ctpts[i + 1].pt_center.x - ctpts[i].pt_center.x) / (double)CE_GR_RESOLUTION;
			// ��Ԃ��Ƃ̐i���̑��Βl(0~1)
			double ratio2 = (ratio - ctpts[i].pt_center.x / (double)CE_GR_RESOLUTION) / range;
			// ��Ԃ��Ƃ̐���_1��X���W(���Βl�A0~1)
			double x1 = (ctpts[i].pt_right.x - ctpts[i].pt_center.x) / (double)(ctpts[i + 1].pt_center.x - ctpts[i].pt_center.x);
			// ��Ԃ��Ƃ̐���_1��Y���W(���Βl)
			double y1 = (ctpts[i].pt_right.y - ctpts[i].pt_center.y) / (double)(ctpts[i + 1].pt_center.y - ctpts[i].pt_center.y);
			// ��Ԃ��Ƃ̐���_2��X���W(���Βl�A0~1)
			double x2 = (ctpts[i + 1].pt_left.x - ctpts[i].pt_center.x) / (double)(ctpts[i + 1].pt_center.x - ctpts[i].pt_center.x);
			// ��Ԃ��Ƃ̐���_2��Y���W(���Βl)
			double y2 = (ctpts[i + 1].pt_left.y - ctpts[i].pt_center.y) / (double)(ctpts[i + 1].pt_center.y - ctpts[i].pt_center.y);

			// ��Ԃ��Ƃ̎n�l�A�I�l(���Βl�ł͂Ȃ��A���ۂ̒l)
			double st2 = st + ctpts[i].pt_center.y / (double)CE_GR_RESOLUTION * (ed - st);
			double ed2 = st + ctpts[i + 1].pt_center.y / (double)CE_GR_RESOLUTION * (ed - st);
			// y1,y2�𑊑Βl������ۂ̒l�ɏC��
			y1 = st2 + (ed2 - st2) * y1;
			y2 = st2 + (ed2 - st2) * y2;

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