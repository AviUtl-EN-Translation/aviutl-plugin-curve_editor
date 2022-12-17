//----------------------------------------------------------------------------------
//		Curve Editor
//		�w�b�_�t�@�C���i�}�N���j
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#pragma once



//---------------------------------------------------------------------
//		���C�A�E�g�֘A
//---------------------------------------------------------------------
// �E�B���h�E
#define CVE_MARGIN						6
#define CVE_MARGIN_CONTROL				3
#define CVE_MARGIN_PRESET				8
#define CVE_MAX_W						1920
#define CVE_MAX_H						1080
#define CVE_DEF_W						360
#define CVE_DEF_H						540
#define CVE_DEF_PRESET_SIZE				50
#define CVE_ROUND_RADIUS				4.0f
#define CVE_NON_CLIENT_H				36

// �Z�p���[�^
#define CVE_SEPARATOR_WIDTH				8
#define CVE_SEPARATOR_LINE_WIDTH		3.6f
#define CVE_SEPARATOR_LINE_LENGTH		32

// �O���t
#define CVE_GRAPH_PADDING				20
#define CVE_GRAPH_WHEEL_COEF_SCALE		0.1
#define CVE_GRAPH_WHEEL_COEF_POS		0.2
#define CVE_GRAPH_GRID_NUM				2
#define CVE_GRAPH_RESOLUTION			10000				// �ύX�s��
#define CVE_GRAPH_SCALE_BASE			1.01
#define CVE_GRAPH_SCALE_MAX				512
#define CVE_GRAPH_SCALE_MIN				0.001

// �n���h���E�J�[�u�E����_
#define CVE_HANDLE_LENGTH_DEFAULT		50
#define CVE_HANDLE_THICKNESS			2.0f
#define CVE_HANDLE_THICKNESS_PRESET		0.8f
#define CVE_HANDLE_SIZE					5.0f
#define CVE_HANDLE_SIZE_PRESET			0.7f
#define CVE_HANDLE_CIRCLE_LINE			2.2f
#define CVE_HANDLE_CIRCLE_LINE_PRESET	0.5f
#define CVE_POINT_RANGE					10
#define CVE_POINT_SIZE					4.4f
#define CVE_PONINT_SIZE_PRESET			2.0f
#define CVE_CURVE_THICKNESS				1.2f
#define CVE_SUBTRACT_LENGTH				10.0f
#define CVE_SUBTRACT_LENGTH_2			8.0f
#define CVE_CURVE_VALUE_MIN_Y			-2.73f
#define CVE_CURVE_VALUE_MAX_Y			3.73f
#define CVE_POINT_DEFAULT_1				0.3f
#define CVE_POINT_DEFAULT_2				0.7f

// �J�[�u�`��
#define CVE_DRAW_CURVE_REGULAR			0
#define CVE_DRAW_CURVE_TRACE			1
#define CVE_DRAW_CURVE_PRESET			2
#define CVE_DRAW_POINT_ONLY				1
#define CVE_DRAW_HANDLE_ONLY			2

#define CVE_DRAW_GRAPH_INCREASEMENT		1.0f

#define CVE_CURVE_COLOR_DEFAULT			RGB(148, 158, 197)


//�t���O
#define BIT(num)						((UINT)1 << num)

#define CVE_EDGE_LT						BIT(0)
#define CVE_EDGE_LB						BIT(1)
#define CVE_EDGE_RT						BIT(2)
#define CVE_EDGE_RB						BIT(3)
#define CVE_EDGE_ALL					CVE_EDGE_LT | CVE_EDGE_LB | CVE_EDGE_RT | CVE_EDGE_RB

#define CVE_BUTTON_ENABLED				BIT(0)
#define CVE_BUTTON_DISABLED				BIT(1)



//---------------------------------------------------------------------
//		�R���g���[��
//---------------------------------------------------------------------
#define CVE_CT_EDIT_MODE_NORMAL			0x0400
#define CVE_CT_EDIT_MODE_MULTIBEZIER	0x0401
#define CVE_CT_EDIT_MODE_VALUE			0x0402
#define CVE_CT_EDIT_MODE_ELASTIC		0x0403
#define CVE_CT_EDIT_MODE_BOUNCE			0x0404

#define CVE_CT_ALIGN					0x0410
#define CVE_CT_PREV						0x0411
#define CVE_CT_PREV_DUR					0x0412
#define CVE_CT_ID_BACK					0x0413
#define CVE_CT_ID_NEXT					0x0414
#define CVE_CT_ID						0x0415
#define CVE_CT_SETSTATUS				0x0416
#define CVE_CT_COLOR_CURVE				0x0417
#define CVE_CT_EDIT_MODE				0x0418

#define CVE_CT_UPPER_H					30
#define CVE_CT_LOWER_H					40
#define CVE_CT_FONT_H					20
#define CVE_HEADER_H					(CVE_CT_UPPER_H + CVE_CT_LOWER_H + CVE_MARGIN * 2 + CVE_MARGIN_CONTROL)

#define CVE_COEF_MOVE_DEFAULT			9
#define CVE_COEF_MOVE_FAST				1

#define CVE_CT_LABEL_MAX				64



//---------------------------------------------------------------------
//		�R�}���h
//---------------------------------------------------------------------
#define CVE_CM_REDRAW					0x0800
#define CVE_CM_COPY						0x0801
#define CVE_CM_READ						0x0802
#define CVE_CM_SAVE						0x0803
#define CVE_CM_CLEAR					0x0804
#define CVE_CM_FIT						0x0805
#define CVE_CM_REVERSE					0x0806
#define CVE_CM_SHOW_HANDLE				0x0807
#define CVE_CM_CHANGE_ID				0x0808
#define CVE_CM_SELECTED					0x0809
#define CVE_CM_ID_NEXT					0x080a
#define CVE_CM_ID_BACK					0x080b
#define CVE_CM_REDRAW_AVIUTL			0x080c
#define CVE_CM_PARAM					0x080d
#define CVE_CM_PARAM_REDRAW				0x080e
#define CVE_CM_PRESET_MOVE				0x080f
#define CVE_CM_CHANGE_COLOR				0x0810
#define CVE_CM_CHANGE_LABEL				0x0811
#define CVE_CM_SET_STATUS				0x0812



//---------------------------------------------------------------------
//		�e�[�}�֘A
//---------------------------------------------------------------------
// ���邳
#define CVE_BR_GRID						25
#define CVE_BR_GR_INVALID				-10
#define CVE_CT_BR_HOVERED				13
#define CVE_CT_BR_CLICKED				-10
#define CVE_CT_BR_SWITCH				-20					//�I�����Ă��Ȃ����
// �A�C�R��
#define CVE_ICON_SIZE					32

// �t�H���g
#define CVE_FONT_REGULAR				"Yu Gothic UI Regular"
#define CVE_FONT_SEMIBOLD				"Yu Gothic UI Semibold"



//---------------------------------------------------------------------
//		���C�u�����E�v���Z�b�g
//---------------------------------------------------------------------
#define CVE_PRESET_NUM_DEFAULT			40
#define CVE_PRESET_NUM_CUSTOM			1024
#define CVE_PRESET_NAME_MAX				128
#define CVE_LIBRARY_SEARCHBAR_HEIGHT	36
#define CVE_PRESET_TITLE_HEIGHT			16



//---------------------------------------------------------------------
//		���̑�
//---------------------------------------------------------------------
#define CVE_POINT_MAX					64					// �ύX�s��
#define CVE_CURVE_MAX					1024				// �ύX�s��
#define CVE_VALUE_RESOLUTION			1000

#define CVE_MATH_PI						3.14159265



//---------------------------------------------------------------------
//		�v���O�C��(�t�B���^)�̏��
//---------------------------------------------------------------------
#define CVE_PLUGIN_NAME						"Curve Editor"
#define CVE_PLUGIN_VERSION					"v1.0"
#define CVE_VERSION_MAJOR					1
#define CVE_VERSION_MINOR					0
#define CVE_VERSION_REVISION				0
#define CVE_PLUGIN_DEVELOPER				"mimaraka"
#define CVE_PLUGIN_YEAR						"2022"
#define CVE_PLUGIN_INFO						CVE_PLUGIN_NAME " " CVE_PLUGIN_VERSION " by " CVE_PLUGIN_DEVELOPER
#define CVE_PLUGIN_LINK						"https://github.com/mimaraka/aviutl-plugin-curve_editor"
#define CVE_PLUGIN_LINK_HELP				CVE_PLUGIN_LINK "/wiki"



//---------------------------------------------------------------------
//		������
//---------------------------------------------------------------------
#define CVE_STR_MODE_BEZIER					"�x�W�F"
#define CVE_STR_MODE_MULTIBEZIER			"�x�W�F(����)"
#define CVE_STR_MODE_VALUE					"�x�W�F(���l�w��)"
#define CVE_STR_MODE_ELASTIC				"�U��"
#define CVE_STR_MODE_BOUNCE					"�o�E���X"
#define CVE_STR_WARNING_DELETE				"�ҏW���̃J�[�u�����������܂��B��낵���ł����H"
#define CVE_STR_WARNING_DELETE_ALL			"���ׂẴJ�[�u�����������܂��B��낵���ł����H"
#define CVE_STR_WARNING_DATA_INVALID		"�݊����̂Ȃ��o�[�W�����ŃJ�[�u���ǂݍ��܂ꂽ���A�f�[�^���j�����Ă��܂��B\n���ׂẴJ�[�u�����������܂����H"
#define CVE_STR_ABOUT						CVE_PLUGIN_NAME " " CVE_PLUGIN_VERSION "\n(" CVE_PLUGIN_YEAR " " CVE_PLUGIN_DEVELOPER ")"
#define CVE_STR_ERROR_OUTOFRANGE			"�l���͈͊O�ł��B"
#define CVE_STR_ERROR_INPUT_INVALID			"�����ȓ��͒l�ł��B"
#define CVE_STR_ERROR_EXEDIT_NOT_FOUND		"�g���ҏW�v���O�C��(exedit.auf)��������܂���B"
#define CVE_STR_INFO_LATEST_VERSION			"���g���̃o�[�W�����͍ŐV�ł��B"
#define CVE_STR_ERROR_CONNECTION_FAILED		"�ڑ��Ɏ��s���܂����B"



//---------------------------------------------------------------------
//		�}�N���֐�
//---------------------------------------------------------------------
#define TO_BGR(ref)							(RGB(GetBValue(ref), GetGValue(ref), GetRValue(ref)))
#define DISTANCE(pt1,pt2)					(std::sqrt(std::pow(pt2.x - pt1.x, 2) + std::pow(pt2.y - pt1.y, 2)))
#define MAX_LIMIT(value,maxv)				(((value) > (maxv))? (maxv): (value))
#define MIN_LIMIT(value,minv)				(((value) < (minv))? (minv): (value))
#define MINMAX_LIMIT(value,minv,maxv)		(((value) < (minv))? (minv): (((value) > (maxv))? (maxv): (value)))
#define ISINRANGE(value,minv,maxv)			(((value) > (minv))? (((value) < (maxv))? 1 : 0) : 0)
#define ISINRANGEEQ(value,minv,maxv)		(((value) >= (minv))? (((value) <= (maxv))? 1 : 0) : 0)
#define CHANGE_BRIGHTNESS(ref,num)			(RGB(MINMAX_LIMIT(GetRValue(ref) + num, 0, 255), MINMAX_LIMIT(GetGValue(ref) + num, 0, 255), MINMAX_LIMIT(GetBValue(ref) + num, 0, 255)))
#define INVERT(ref)							(RGB(255 - GetRValue(ref), 255 - GetGValue(ref), 255 - GetBValue(ref)))
#define CONTRAST(ref,val)					(RGB(MINMAX_LIMIT(127 + (GetRValue(ref) - 127) * val, 0, 255), MINMAX_LIMIT(127 + (GetGValue(ref) - 127) * val, 0, 255), MINMAX_LIMIT(127 + (GetBValue(ref) - 127) * val, 0, 255)))
#define LARGER(val1, val2)					(((val1) > (val2))? (val1) : (val2))
#define SMALLER(val1, val2)					(((val1) < (val2))? (val1) : (val2))