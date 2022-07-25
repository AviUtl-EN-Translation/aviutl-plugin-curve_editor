//----------------------------------------------------------------------------------
//		Flow for AviUtl
//		Source File (Functions for IDCurve class)
//		(Visual C++ 2019)
//----------------------------------------------------------------------------------

#include "ce_header.hpp"


//---------------------------------------------------------------------
//		����_�Q��ǉ�
//---------------------------------------------------------------------
void ce::Curve_ID::addPoint(POINT gr_pt)
{
	int index = 0;
	Point_Address tmp;
	if (ctpts.size() >= CE_POINT_MAX) return;
	for (int i = 0; i < ctpts.size(); i++) {
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
	ctpt_add.pt_left = { gr_pt.x - CE_HANDLE_DEF_L, gr_pt.y };
	ctpt_add.pt_right = { gr_pt.x + CE_HANDLE_DEF_L, gr_pt.y };
	ctpts.insert(ctpts.begin() + index, ctpt_add);

	//���E�̓_�����ׂ̒����̓_��荶/�E�ɏo�Ă�����C��
	if (ctpts[index].pt_left.x < ctpts[index - 1].pt_center.x)
		ctpts[index].pt_left.x = ctpts[index - 1].pt_center.x;
	if (ctpts[index].pt_right.x > ctpts[index + 1].pt_center.x)
		ctpts[index].pt_right.x = ctpts[index + 1].pt_center.x;

	//���ׂ̍��E�̓_�������̓_���E/���ɏo�Ă�����C��
	
	tmp = { index - 1, 3 };
	CorrectHandle(tmp, GetHandleAngle(tmp));
	
	tmp = { index + 1, 2 };
	CorrectHandle(tmp, GetHandleAngle(tmp));
	
}


//---------------------------------------------------------------------
//		����_�Q���폜
//---------------------------------------------------------------------
void ce::Curve_ID::DeletePoint(POINT cl_pt)
{
	Point_Address address = PtInCtpts(cl_pt);
	if (!address.CLR) return;
	for (int i = 1; i < ctpts.size() - 1; i++) {
		if (address.index == i) {
			ctpts.erase(ctpts.begin() + i);
			break;
		}
	}
}


POINT ce::Curve_ID::GetPoint(Point_Address address)
{
	POINT result = { -1, -1 };
		switch (address.CLR) {
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
//		����_(�Q)���ړ�
//---------------------------------------------------------------------
void ce::Curve_ID::MovePoint(Point_Address address, POINT gr_pt, BOOL bReset)
{
	constexpr double MATH_PI = 3.14159265;
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
		tmp = { address.index - 1, 3 };
		agl_prev = GetHandleAngle(tmp);
		tmp = { address.index + 1, 2 };
		agl_next = GetHandleAngle(tmp);
		tmp = { address.index, 2 };
		agl_left = GetHandleAngle(tmp);
		tmp = { address.index, 3 };
		agl_right = GetHandleAngle(tmp);

		len_left = DISTANCE(ctpts[address.index].pt_center, ctpts[address.index].pt_left);
		len_right = DISTANCE(ctpts[address.index].pt_center, ctpts[address.index].pt_right);
	}
	switch (address.CLR) {
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
			tmp = { address.index, 2 };//��  O-----[]
			CorrectHandle(tmp, agl_left);
			tmp = { address.index, 3 };//�E []-----O
			CorrectHandle(tmp, agl_right);

			ctpts[address.index - 1].pt_right = prevright;
			ctpts[address.index + 1].pt_left = nextleft;

			tmp = { address.index - 1, 3 };//�E []-----O (�O�̐���_�Q)
			CorrectHandle(tmp, agl_prev);
			tmp = { address.index + 1, 2 };//��  O-----[] (���̐���_�Q)
			CorrectHandle(tmp, agl_next);
		}
		break;

	case 2://Left   O-----o
		if (ctpts[address.index].type == 0) return;

		ctpts[address.index].pt_left.x = MINMAXLIM(gr_pt.x,
			ctpts[address.index - 1].pt_center.x,
			ctpts[address.index].pt_center.x);
		ctpts[address.index].pt_left.y = gr_pt.y;

		//����(�p�x)
		if (g_cfg.align_mode == 1) {
			tmp = { address.index, 2 };
			agl_tmp = GetHandleAngle(tmp);
			tmp = { address.index, 3 };
			SetHandleAngle(tmp, agl_tmp + MATH_PI, TRUE, len_right);
		}
		break;

	case 3://Right  o-----O
		if (ctpts[address.index].type == 1) return;

		ctpts[address.index].pt_right.x = MINMAXLIM(gr_pt.x,
			ctpts[address.index].pt_center.x,
			ctpts[address.index + 1].pt_center.x);
		ctpts[address.index].pt_right.y = gr_pt.y;

		//����(�p�x)
		if (g_cfg.align_mode == 1) {
			tmp = { address.index, 3 };
			agl_tmp = GetHandleAngle(tmp);
			tmp = { address.index, 2 };
			SetHandleAngle(tmp, agl_tmp + MATH_PI, TRUE, len_left);
		}
		break;
	}
}


//---------------------------------------------------------------------
//		�w�肵�����W������_�Q�̓����ɑ��݂��Ă��邩
//---------------------------------------------------------------------
ce::Point_Address ce::Curve_ID::PtInCtpts(POINT cl_pt)
{
	RECT rcCenter, rcLeft, rcRight;
	for (int i = 0; i < ctpts.size(); i++) {
		rcCenter = {
			(LONG)ToClient(ctpts[i].pt_center).x - CE_POINT_RANGE,
			(LONG)ToClient(ctpts[i].pt_center).y - CE_POINT_RANGE,
			(LONG)ToClient(ctpts[i].pt_center).x + CE_POINT_RANGE,
			(LONG)ToClient(ctpts[i].pt_center).y + CE_POINT_RANGE
		};
		rcLeft = {
			(LONG)ToClient(ctpts[i].pt_left).x - CE_POINT_RANGE,
			(LONG)ToClient(ctpts[i].pt_left).y - CE_POINT_RANGE,
			(LONG)ToClient(ctpts[i].pt_left).x + CE_POINT_RANGE,
			(LONG)ToClient(ctpts[i].pt_left).y + CE_POINT_RANGE
		};
		rcRight = {
			(LONG)ToClient(ctpts[i].pt_right).x - CE_POINT_RANGE,
			(LONG)ToClient(ctpts[i].pt_right).y - CE_POINT_RANGE,
			(LONG)ToClient(ctpts[i].pt_right).x + CE_POINT_RANGE,
			(LONG)ToClient(ctpts[i].pt_right).y + CE_POINT_RANGE
		};

		if (PtInRect(&rcLeft, cl_pt) && ctpts[i].type != 0)
			return { i, 2 };
		else if (PtInRect(&rcRight, cl_pt) && ctpts[i].type != 1)
			return { i, 3 };
		else if (PtInRect(&rcCenter, cl_pt) && ctpts[i].type > 1)
			return { i, 1 };
	}
	return { 0, 0 };
}


//---------------------------------------------------------------------
//		�n���h���̊p�x���擾
//---------------------------------------------------------------------
double ce::Curve_ID::GetHandleAngle(Point_Address address)
{
	double angle;
	int dstx, dsty;
	switch (address.CLR) {
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
	if (dstx == 0 && dsty == 0) return 0;
	angle = std::atan2(dsty, dstx);
	return angle;
}


//---------------------------------------------------------------------
//		�n���h���̊p�x��ݒ�
//---------------------------------------------------------------------
void ce::Curve_ID::SetHandleAngle(Point_Address address, double angle, BOOL bLength, double lgth)
{
	double length;
	if (address.CLR == 2 && ctpts[address.index].type != 0) { //��-left
		length = bLength ? lgth : DISTANCE(
			ctpts[address.index].pt_center,
			ctpts[address.index].pt_left);

		ctpts[address.index].pt_left.x = ctpts[address.index].pt_center.x + std::cos(angle) * length;
		ctpts[address.index].pt_left.y = ctpts[address.index].pt_center.y + std::sin(angle) * length;
		CorrectHandle(address, angle);
	}
	else if (address.CLR == 3 && ctpts[address.index].type != 1) { //�E-right
		length = bLength ? lgth : DISTANCE(
			ctpts[address.index].pt_center,
			ctpts[address.index].pt_right);

		ctpts[address.index].pt_right.x = ctpts[address.index].pt_center.x + std::cos(angle) * length;
		ctpts[address.index].pt_right.y = ctpts[address.index].pt_center.y + std::sin(angle) * length;
		CorrectHandle(address, angle);
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
void ce::Curve_ID::CorrectHandle(Point_Address address, double angle)
{
	switch (address.CLR) {
	case 2://��   O-----[]
		//���̐���_���O�̐���_�Q�̒����̓_��荶���ɂ������Ƃ�
		if (GetPoint(address).x < ctpts[address.index - 1].pt_center.x) {
			ctpts[address.index].pt_left.x = ctpts[address.index - 1].pt_center.x;
			//�p�x��ۂ�
			ctpts[address.index].pt_left.y =
				ctpts[address.index].pt_center.y + std::tan(angle) *
				(ctpts[address.index].pt_left.x - ctpts[address.index].pt_center.x);
		}
		break;
	case 3://�E  []-----O
		if (GetPoint(address).x > ctpts[address.index + 1].pt_center.x) {
			ctpts[address.index].pt_right.x = ctpts[address.index + 1].pt_center.x;
			//�p�x��ۂ�
			ctpts[address.index].pt_right.y =
				ctpts[address.index].pt_center.y + std::tan(angle) *
				(ctpts[address.index].pt_right.x - ctpts[address.index].pt_center.x);
		}
		break;
	default:
		return;
	}
}


//---------------------------------------------------------------------
//		�O���t�𔽓]
//---------------------------------------------------------------------
void ce::Curve_ID::ReversePoints()
{
	std::vector<Points_ID> ctpts_old;
	for (int i = 0; i < ctpts.size() / 2.0; i++) {
		ctpts_old.emplace_back(ctpts[i]);
		ctpts[i].pt_center.x = CE_GR_RES - ctpts[ctpts.size() - i - 1].pt_center.x;
		ctpts[i].pt_center.y = CE_GR_RES - ctpts[ctpts.size() - i - 1].pt_center.y;
		ctpts[i].pt_left.x = CE_GR_RES - ctpts[ctpts.size() - i - 1].pt_right.x;
		ctpts[i].pt_left.y = CE_GR_RES - ctpts[ctpts.size() - i - 1].pt_right.y;
		ctpts[i].pt_right.x = CE_GR_RES - ctpts[ctpts.size() - i - 1].pt_left.x;
		ctpts[i].pt_right.y = CE_GR_RES - ctpts[ctpts.size() - i - 1].pt_left.y;

		ctpts[ctpts.size() - i - 1].pt_center.x = CE_GR_RES - ctpts_old[i].pt_center.x;
		ctpts[ctpts.size() - i - 1].pt_center.y = CE_GR_RES - ctpts_old[i].pt_center.y;
		ctpts[ctpts.size() - i - 1].pt_left.x = CE_GR_RES - ctpts_old[i].pt_right.x;
		ctpts[ctpts.size() - i - 1].pt_left.y = CE_GR_RES - ctpts_old[i].pt_right.y;
		ctpts[ctpts.size() - i - 1].pt_right.x = CE_GR_RES - ctpts_old[i].pt_left.x;
		ctpts[ctpts.size() - i - 1].pt_right.y = CE_GR_RES - ctpts_old[i].pt_left.y;
	}

}


//---------------------------------------------------------------------
//		Get Value
//---------------------------------------------------------------------
double ce::Curve_ID::GetValue(double ratio, double st, double ed)
{
	if (!ISINRANGE(ratio, 0, 1)) return 0;
	for (int i = 0; i < ctpts.size() - 1; i++) {
		if (ISINRANGE(ratio, ctpts[i].pt_center.x / (double)CE_GR_RES, ctpts[i + 1].pt_center.x / (double)CE_GR_RES)) {
			double range = (ctpts[i + 1].pt_center.x - ctpts[i].pt_center.x) / (double)CE_GR_RES;
			double x1 = (ctpts[i].pt_right.x - ctpts[i].pt_center.x) / (ctpts[i + 1].pt_center.x - ctpts[i].pt_center.x);
			double y1 = (ctpts[i].pt_right.y - ctpts[i].pt_center.y) / (ctpts[i + 1].pt_center.y - ctpts[i].pt_center.y);
			double x2 = (ctpts[i + 1].pt_center.x - ctpts[i + 1].pt_left.x) / (ctpts[i + 1].pt_center.x - ctpts[i].pt_center.x);
			double y2 = (ctpts[i + 1].pt_center.y - ctpts[i + 1].pt_left.y) / (ctpts[i + 1].pt_center.y - ctpts[i].pt_center.y);
			double st2 = st + ctpts[i].pt_center.y * (ed - st) / (double)CE_GR_RES;
			double ed2 = st + ctpts[i + 1].pt_center.y * (ed - st) / (double)CE_GR_RES;
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