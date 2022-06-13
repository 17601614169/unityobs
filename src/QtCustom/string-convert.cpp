#include "string-convert.h"


StringConvert ::StringConvert() {

}

StringConvert ::~StringConvert() {

}

QString StringConvert::GetString(STRINDEX index) {
	switch (index) {
	case STR_START_LIVE:
		return QString(tr("STR_START_LIVE"));
		break;
	case STR_STOP_LIVE:
		return QString(tr("STR_STOP_LIVE"));
		break;
	case STR_SECRET:
		return QString(tr("STR_SECRET"));
		break;
	case STR_OPEN_EXIST_FILE_NAME:
		return QString(tr("STR_OPEN_EXIST_FILE_NAME"));
		break;
	case STR_START_LIVE_TIP:
		return QString(tr("STR_START_LIVE_TIP"));
		break;
	case STR_PLEASE_START_LIVE_TIP:
		return QString(tr("STR_PLEASE_START_LIVE_TIP"));
		break;
	case STR_LOGIN_TITLE:
		return QString(tr("STR_LOGIN_TITLE"));
		break;
	case STR_LOGIN_ERROR:
		return QString(tr("STR_LOGIN_ERROR"));
		break;
	case STR_SCREEN_SHOT:
		return QString(tr("STR_SCREEN_SHOT"));
		break;
	case STR_STREAM:
		return QString(tr("STR_STREAM"));
		break;
	case STR_PROCESS:
		return QString(tr("STR_PROCESS"));
		break;
	case STR_PROCESS_ALREADY_EXIST:
		return QString(tr("STR_PROCESS_ALREADY_EXIST"));
		break;
	case STR_PROCESS_INVALID_ARGV:
		return QString(tr("STR_PROCESS_INVALID_ARGV"));
		break;
	case STR_CAMERA:
		return QString(tr("STR_CAMERA"));
		break;
	case STR_HANDLE_SUCCESS:
		return QString(tr("STR_HANDLE_SUCCESS"));
		break;
	case STR_HANDLE_FAIL:
		return QString(tr("STR_HANDLE_FAIL"));
		break;
	case STR_HANDLE_INPUT_NOEMPTY:
		return QString(tr("STR_HANDLE_INPUT_NOEMPTY"));
		break;
	case STR_CAMERA_USED:
		return QString(tr("STR_CAMERA_USED"));
		break;
	case STR_FINISH_LIVE:
		return QString(tr("STR_FINISH_LIVE"));
		break;
	case STR_CLOSE_SOURCE:
		return QString(tr("STR_CLOSE_SOURCE"));
		break;
	case STR_PROCESS_LOGIN_EXPIRED:
		return QString(tr("STR_PROCESS_LOGIN_EXPIRED"));
		break;
	case STR_UNIQUECODE:
		return QString(tr("STR_UNIQUECODE"));
		break;
	case STR_UNIQUECODE_AUTHENTICATED:
		return QString(tr("STR_UNIQUECODE_AUTHENTICATED"));
		break;
	case STR_UNIQUECODE_REJECT:
		return QString(tr("STR_UNIQUECODE_REJECT"));
		break;
	case STR_UNIQUECODE_REQ:
		return QString(tr("STR_UNIQUECODE_REQ"));
		break;
	case STR_WHITE_ARGS_NOT_NULL:
		return QString(tr("STR_WHITE_ARGS_NOT_NULL"));
		break;
	case STR_WHITE_APPLY:
		return QString(tr("STR_WHITE_APPLY"));
		break;
	case STR_UNIQUECODE_AUTHING:
		return QString(tr("STR_UNIQUECODE_AUTHING"));
		break;
	case STR_PASSWORD_LIMIT:
		return QString(tr("STR_PASSWORD_LIMIT"));
		break;
	case STR_CONFIRM_PASSWORD_AGAIN:
		return QString(tr("STR_CONFIRM_PASSWORD_AGAIN"));
		break;
	case STR_FONT_SET_NOEMPTY:
		return QString(tr("STR_FONT_SET_NOEMPTY"));
		break;
	case STR_IMAGE_NOEMPTY:
		return QString(tr("STR_IMAGE_NOEMPTY"));
		break;
	default:
		break;
	}
	return QString("");
}
