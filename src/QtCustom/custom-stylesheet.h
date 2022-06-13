#ifndef CUSTOM_STYLESHEET_H
#define CUSTOM_STYLESHEET_H
#include <QString>

// 主题颜色
const QString THEME_COLOR = QString("background-color:rgb(24, 17, 64);");

// 分割线
const QString DIVIDING_LINE_BLACK = QString("QWidget {background-color:rgb(0, 0, 0);}");
const QString DIVIDING_LINE_GRAY = QString("QWidget {background-color:rgb(14, 9, 42);}");
const QString DIVIDING_LINE_WHITE = QString("QWidget {background-color:rgb(255,255,255);}");
const QString DIVIDING_LINE_LIST = QString("QWidget {background-color:rgb(238, 236, 243);}");

// 按钮
const QString CLOSE_BTN = QString("QPushButton{border-image:url(:/res/images/close.png); background-color:transparent;}"
			"QPushButton:hover{border-image:url(:/res/images/close_hover_2.png);}"
			"QPushButton:pressed{border-image:url(:/res/images/close_hover_2.png);}");

// LoginDialog
const QString SMALL_CLOSE_BTN = QString("QPushButton{"
	"border-image:url(:/res/images/login/close_c.png);"
	"background-color:transparent;}"
	"QPushButton::hover{border-image:url(:/res/images/login/close_c.png);}");
const QString LOGIN_APP_ICON = QString("QLabel{"
			"border-image:url(:/res/images/login/applogo.png);"
			"background-color:transparent;}");
const QString LOGIN_APP_NAME = QString("QLabel{color:#444444;font-size:20px;background-color:transparent;font-family:Microsoft YaHei;}");
const QString LOGIN_SHOP_CODE = QString(
	"QLineEdit{color:#444444;font-size:14px;background-color:transparent;border:none;line-height:20px;font-family:Microsoft YaHei;}");
const QString EYE_STYLE = QString("QPushButton{background-color:transparent;border:none;"
		"border-image:url(:/res/images/login/eyeon.png);}"
		"QPushButton::checked{border-image:url(:/res/images/login/eyeoff.png);}") ;
const QString LOGIN_USER_NAME_ICON = QString("QLabel{"
			"border-image:url(:/res/images/login/username.png);"
			"background-color:transparent;}");
const QString LOGIN_SHOP_CODE_ICON = QString(
	"QLabel{"
	"border-image:url(:/res/images/login/shopcode.png);"
	"background-color:transparent;border:none;}"); 
const QString LOGIN_USER_NAME = QString("color:#FFFFFF;font-size:14px;background:transparent;border-width:0;border-style:outset;font-family:Microsoft YaHei;");
const QString LOGIN_LINE = QString("QLabel{background-color:#EEEEEE;}");
const QString LOGIN_PASSWORD_ICON = QString("QLabel{border-image:url(:/res/images/login/shouji.png);background-color:transparent;}");
const QString LOGIN_PASSWORD = QString("color:#FFFFFF; font-size:14px; background:transparent;border-width:0;border-style:outset;font-family:Microsoft YaHei;");
const QString LOGIN_REMEMBER_PASSWORD = QString("QCheckBox{spacing:5px;"
			"color:#BBB3CB;font-size:12px;font-family:Microsoft YaHei;}"
			"QCheckBox:hover{color:white;}"
			"QCheckBox:checked{color:white;}"
			"QCheckBox::indicator:disabled{color:#555555;}"
			"QCheckBox::indicator{width:16px;height:16px;}"
			"QCheckBox::indicator:unchecked {"
			"border-image:url(:/res/images/checkbox-unselect.png) 0 0 0 0 repeat repeat;"
			"border-width :0px;"
			"border-radius :0px;}"
			"QCheckBox::indicator:unchecked:hover"
			"{border-image:url(:/res/images/checkbox-unselect-hover.png) 0 0 0 0 repeat repeat;}"
			"QCheckBox::indicator:unchecked:pressed"
			"{border-image:url(:/res/images/checkbox-unselect-hover.png) 0 0 0 0 repeat repeat;}"
			"QCheckBox::indicator:checked"
			"{border-image : url(:/res/images/checkbox-selected.png) 0 0 0 0 repeat repeat;"
			"border-width:0px;border-radius:0px;}"
			"QCheckBox::indicator:checked:hover"
			"{border-image : url(:/res/images/checkbox-selected.png) 0 0 0 0 repeat repeat;}"
			"QCheckBox::indicator:checked:pressed{"
			"border-image : url(:/res/images/checkbox-selected.png) 0 0 0 0 repeat repeat;}");
const QString LOGIN_BTN = QString("QPushButton{background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop : 0 #FFDB00, stop : 1 #FFEB00);border-radius:22px; color:#222222;}"
	"QPushButton::hover{background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop : 0 #FFC900, stop : 1 #FFDC00);}");


const QString MSG_BTN_STYLE = QString("QPushButton{background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop : 0 #FFDB00, stop : 1 #FFEB00);border-radius:15px; color:#222222;}"
	"QPushButton::hover{background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop : 0 #FFC900, stop : 1 #FFDC00);}");

const QString LOGIN_TIP = QString("font-size:12px;font-weight: 400;color: #BBB3CB;line-height: 16px;font-family:Microsoft YaHei;");
const QString LOGIN_QRCODE = QString("background-color:transparent;border-image:url(:/res/images/qrcode.png);");
const QString LOGIN_DOWN_TXT = QString("font-size:12px;font-weight: 400;color: #BBB3CB;line-height: 14px;font-family:Microsoft YaHei;");

// LeftWidget
const QString LEFT_WIDGET_BG = QString("QWidget#left_widget_bg {")
			+ QString("border:0px; border-radius:6px;")
			+ THEME_COLOR
			+ QString("}");
const QString LEFT_WIDGET_SCENE = QString("QLabel{padding-left:10px;color:rgb(225, 225, 225);}");
const QString LEFT_WIDGET_LIST = QString("QListWidget {background-color:rgb(24, 17, 64); border: 0px;}"
			"QListWidget:focus {outline: none;} "
			"QListWidget:item {height:46px; background-color:rgb(24, 17, 64);}"
			"QListWidget:item:hover {border:0px; background-color:rgb(14, 9, 42);}"
			"QListWidget:item::selected:active {border:0px; background-color:rgb(14, 9, 42);}"
			"QListWidget:item:selected {border:0px; background-color:rgb(14, 9, 42);}");
const QString LEFT_WIDGET_SCROLL = QString("QScrollBar:vertical {border:none;margin:0px,0px,0px,0px;width:4px; background-color:#26195A;}"
			"QScrollBar:vertical:hover {margin: 0px,0px,0px,0px; background-color:#26195A;}"
			"QScrollBar::handle:vertical {max-width:4px; background-color:#55498D;  height: 40px;}"
			"QScrollBar::add-line:vertical {height:0px;}"
			"QScrollBar::sub-line:vertical {height:0px;}"
			"QScrollBar::add-page:vertical {background:transparent;}"
			"QScrollBar::sub-page:vertical {background:transparent;}");
const QString LEFT_WIDGET_ADD_SOURCE_BTN = QString("QPushButton{border-image:url(:/res/images/add.png)}"
			"QPushButton:hover{border-image:url(:/res/images/add_hover.png)}"
			"QPushButton:pressed{border-image:url(:/res/images/add_hover.png)}");
const QString LEFT_WIDGET_MOVE_DOWN_BTN = QString("QPushButton{border-image:url(:/res/images/addtop.png)}"
			"QPushButton:hover{border-image:url(:/res/images/addtop_hover.png)}"
			"QPushButton:pressed{border-image:url(:/res/images/addtop_hover.png)}");
const QString LEFT_WIDGET_MOVE_UP_BTN = QString("QPushButton{border-image:url(:/res/images/addbottom.png)}"
			"QPushButton:hover{border-image:url(:/res/images/addbottom_hover.png)}"
			"QPushButton:pressed{border-image:url(:/res/images/addbottom_hover.png)}");

// SourceItem
const QString SOURCE_ITEM_TEXT = QString("QLabel{color:rgb(225, 225, 225);font-size:12px;font-family:Microsoft YaHei;}"
			"QLabel:disabled{color:rgb(126,126,126);}");
const QString SOURCE_ITEM_EDIT_BTN = QString("QPushButton{border-image:url(:/res/images/edit.png);}"
			"QPushButton:hover{border-image:url(:/res/images/edit_hover.png);}"
			"QPushButton:pressed{border-image:url(:/res/images/edit_hover.png);}");
const QString SOURCE_ITEM_DELETE_BTN = QString("QPushButton{border-image:url(:/res/images/delete.png);}"
			"QPushButton:hover{border-image:url(:/res/images/delete_hover.png);}"
			"QPushButton:pressed{border-image:url(:/res/images/delete_hover.png);}");
const QString SOURCE_ITEM_CHECK_BOX = QString("QCheckBox::indicator{width:16px;height:16px;}"
			"QCheckBox::indicator:checked{border-image:url(:/res/images/enable.png);}"
			"QCheckBox::indicator:checked:hover{border-image:url(:/res/images/enable_hover.png);}"
			"QCheckBox::indicator:checked:hover{border-image:url(:/res/images/enable_hover.png);}"
			"QCheckBox::indicator:unchecked{border-image:url(:/res/images/disable.png);}"
			"QCheckBox::indicator:unchecked:hover{border-image:url(:/res/images/disable_hover.png);}"
			"QCheckBox::indicator:unchecked:hover{border-image:url(:/res/images/disable_hover.png);}");

// TopWidget
const QString TOP_WIDGET_BG = QString("QWidget#top_widget_bg {background-color:rgb(255, 255, 255);}");
const QString TOP_WIDGET_NAME_LABEL = QString("QLabel{color:rgb(34, 34, 34);font-size:14px;background-color:transparent;font-family:Microsoft YaHei;}");
const QString TOP_WIDGET_COMPANY_LABEL = QString("QLabel{color:rgb(153, 153, 153);font-size:10px;background-color:transparent;font-family:Microsoft YaHei;}");
const QString TOP_WIDGET_INFO_LABEL = QString("QLabel{color:rgb(85, 85, 85);font-size:12px;background-color:transparent;font-family:Microsoft YaHei;}");
const QString TOP_WIDGET_WAIT_LABEL = QString("QLabel{color:rgb(255, 78, 68);font-size:12px;background-color:transparent;font-family:Microsoft YaHei;}");
const QString TOP_WIDGET_CURRENT_LABEL = QString("QLabel{color:rgb(34, 34, 34);font-size:12px;background-color:transparent;font-family:Microsoft YaHei;}");
const QString TOP_WIDGET_MESSAGE_BTN = QString("QPushButton{border-image:url(:/res/images/message_normal.png);background-color:transparent;}"
			"QPushButton:hover{border-image:url(:/res/images/message_normal.png);}"
			"QPushButton:pressed{border-image:url(:/res/images/message_normal.png);}");
const QString TOP_WIDGET_MIN_BTN = QString("QPushButton{border-image:url(:/res/images/min.png);background-color:transparent;}"
			"QPushButton:hover{border-image:url(:/res/images/min_hover.png);}"
			"QPushButton:pressed{border-image:url(:/res/images/min_hover.png);}");
const QString TOP_WIDGET_MAX_BTN = QString("QPushButton{border-image:url(:/res/images/max.png);background-color:transparent;}"
			"QPushButton:hover{border-image:url(:/res/images/max_hover.png);}"
			"QPushButton:pressed{border-image:url(:/res/images/max_hover.png);}");
const QString TOP_WIDGET_RESTORE_BTN = QString("QPushButton{border-image:url(:/res/images/restore.png);background-color:transparent;}"
			"QPushButton:hover{border-image:url(:/res/images/restore_hover.png);}"
			"QPushButton:pressed{border-image:url(:/res/images/restore_hover.png);}");
const QString TOP_WIDGET_CLOSE_BTN = QString("QPushButton{border-image:url(:/res/images/close.png);background-color:transparent;}"
			"QPushButton:hover{border-image:url(:/res/images/close_hover_2.png);}"
			"QPushButton:pressed{border-image:url(:/res/images/close_hover_2.png);}");

// BottomCtrls
const QString BOTTOM_CTRL_BG = QString("QWidget#bottom_ctrl_bg {border-bottom-left-radius:6px;border-bottom-right-radius:6px;")
			+ THEME_COLOR
			+ QString("}");
const QString BOTTOM_CTRL_START_LIVE_BTN = QString("QPushButton{background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop : 0 #5D00FF, stop : 1 #793BFF);border-radius:22px;color:rgb(225, 225, 225);}"
			"QPushButton::hover{background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop : 0 #4F01D5, stop : 1 #6E2AFF);}");

// NamedBtn
const QString NAME_BTN_STYLE = QString("QPushButton {border:0px; background-color:transparent;}");
const QString NAME_BTN_TEXT = QString("QLabel{color:rgb(102, 102, 102);font-size:12px; background-color:transparent;font-family:Microsoft YaHei;}");
const QString NAME_BTN_MASKBTN = QString("QPushButton{border-image: url(:/res/images/tiezhi.png);background-color:transparent;}"
			"QPushButton::hover{border-image: url(:/res/images/tiezhi_hover.png);background-color:transparent;}"
			"QPushButton::pressed{border-image: url(:/res/images/tiezhi_hover.png);background-color:transparent;}");
const QString NAME_BTN_MASK3DBTN = QString("QPushButton{border-image: url(:/res/images/mianju.png);background-color:transparent;}"
			"QPushButton::hover{border-image: url(:/res/images/mianju_hover.png);background-color:transparent;}"
			"QPushButton::pressed{border-image: url(:/res/images/mianju_hover.png);background-color:transparent;}");
const QString NAME_BTN_BKGROUNDBTN = QString("QPushButton{border-image: url(:/res/images/beijing.png);background-color:transparent;}"
			"QPushButton::hover{border-image: url(:/res/images/beijing_hover.png);background-color:transparent;}"
			"QPushButton::pressed{border-image: url(:/res/images/beijing_hover.png);background-color:transparent;}");
const QString NAME_BTN_DEVICEBTN = QString("QPushButton{border-image: url(:/res/images/shebei.png);background-color:transparent;}"
			"QPushButton::hover{border-image: url(:/res/images/shebei_hover.png);background-color:transparent;}"
			"QPushButton::pressed{border-image: url(:/res/images/shebei_hover.png);background-color:transparent;}");
const QString NAME_BTN_QUAILTYBTN = QString(
	"QPushButton{border-image: url(:/res/images/huazhi.png);background-color:transparent;}"
	"QPushButton::hover{border-image: url(:/res/images/huazhi_hover.png);background-color:transparent;}"
	"QPushButton::pressed{border-image: url(:/res/images/huazhi_hover.png);background-color:transparent;}");
const QString NAME_BTN_MKFBTN = QString("QPushButton{border-image: url(:/res/images/maikef.png);background-color:transparent;}"
			"QPushButton::hover{border-image: url(:/res/images/maikef_hover.png);background-color:transparent;}"
			"QPushButton::pressed{border-image: url(:/res/images/maikef_hover.png);background-color:transparent;}");
//UserInfoWidget
const QString USER_INFO_BG = QString("QWidget#user_info_bg {border-top-left-radius:6px;border-top-right-radius:6px;")
			+ THEME_COLOR
			+ QString("}");
const QString USER_INFO_TITLE = QString("QLabel{color:rgb(34, 34, 34);font-size:18px;background-color:transparent;font-family:Microsoft YaHei;}");
const QString USER_INFO_ID = QString("QLabel{color:rgb(68, 68, 68);font-size:14px;background-color:transparent;font-family:Microsoft YaHei;}");
const QString USER_INFO_GENERAL = QString("QLabel{color:rgb(153, 153, 153);font-size:13px;background-color:transparent;font-family:Microsoft YaHei;}");
const QString USER_INFO_LIVE_STATE = QString("QLabel{color:rgb(255, 76, 61);font-size:13px;background-color:transparent;font-family:Microsoft YaHei;}");
const QString USER_INFO_DEPTH = QString("QLabel{color:rgb(68, 68, 68);font-size:13px;background-color:transparent;font-family:Microsoft YaHei;}");
const QString USER_INFO_FINISH_BTN = QString("QPushButton{background: rgb(255,255,255);border-radius:6px;}"
			"QPushButton::hover{background: rgb(250, 250, 250);}");
const QString USER_INFO_START_BTN = QString("QPushButton{background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop : 0 #FFDB00, stop : 1 #FFEB00);border-radius:6px;}"
			"QPushButton::hover{background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop : 0 #FFC900, stop : 1 #FFDC00);}");

// IMWidget
const QString IM_WIDGET_BG = QString("QWidget#imwidget_bg {border-radius:6px;")
			+ THEME_COLOR
			+ QString("}");
const QString IM_WIDGET_NOTICE_ICON = QString("QLabel {background-color:transparent;border-image:url(:/res/images/guanbo.png);}");
const QString IM_WIDGET_NOTICE_TEXT = QString("QLineEdit {font-size: 12px;line-height:17px;border:none;background-color:transparent;color:rgb(225, 225, 225);font-family:Microsoft YaHei;}");
const QString IM_WIDGET_NOTICE_BTN = QString("QPushButton {background-color:transparent;border-image:url(:/res/images/edit_title.png);}");
const QString IM_WIDGET_SYSTEM_TIP = QString("QTextEdit {margin:0px;font-size: 12px;font-weight: 600;color: #C3BEE0;line-height: 20px;border:none;background-color:transparent;font-family:Microsoft YaHei;}");
const QString IM_WIDGET_RECONECT = QString("QLabel {font-size:12px;font-weight:400;color:#FFC62A;font-family:Microsoft YaHei;}");
const QString IM_WIDGET_LIST = QString(
			"QListWidget{border:none;padding:0px;padding-top:0px; "
			"outline:0px;"
			"background:transparent;"
			"border-radius:2px;}"
			"QListWidget::Item{padding:0 0px;background:transparent;}"
			"QListWidget::Item:hover{background:transparent;padding:0 0px;border:none;}"
			"QListWidget::item:selected{background:transparent;padding:0 0px;border:none;}"
			"QListWidget::item:selected:!active{background:transparent;padding:0 0px;}");
const QString IM_WIDGET_SHIELD = QString("QCheckBox::indicator{width:25px;height:25px;}"
			"QCheckBox::indicator:checked{border-image:url(:/res/images/texiao_on.png);}"
			"QCheckBox::indicator:checked:hover{border-image:url(:/res/images/texiao_on_hover.png);}"
			"QCheckBox::indicator:checked:hover{border-image:url(:/res/images/texiao_on_hover.png);}"
			"QCheckBox::indicator:unchecked{border-image:url(:/res/images/texiao_off.png);}"
			"QCheckBox::indicator:unchecked:hover{border-image:url(:/res/images/texiao_off_hover.png);}"
			"QCheckBox::indicator:unchecked:hover{border-image:url(:/res/images/texiao_off_hover.png);}");
const QString IM_WIDGET_RED_BTN = QString("QPushButton {border-image:url(:/res/images/big_red_envelopes.png)}");

// SacrougeDialog
const QString SACROUGE_BG = QString("QWidget#SacrougeBg{background-color:rgb(252, 251, 253); border-radius:6px; border:0px;}");
const QString SACROUGE_BIG_LABEL = QString("QLabel{color:rgb(68, 68, 68); font-size:14px;font-family:Microsoft YaHei;}");
const QString SACROUGE_EDIT = QString("QLineEdit{border-radius: 4px; border:1px solid rgb(211, 204, 223); font-size:12px; padding-left:10;font-family:Microsoft YaHei;}");
const QString SACROUGE_SMALL_LABEL = QString("QLabel {color:rgb(95, 91, 115); font-size:12px; max-height:17px;font-family:Microsoft YaHei;}");
const QString SACROUGE_TICKET_COUNT = QString("QLabel {color:rgb(106, 90, 255); font-size:12px; max-height:17px;font-family:Microsoft YaHei;}");
const QString SACROUGE_CONFIRM_BTN = QString("QPushButton{background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop : 0 #5D00FF, stop : 1 #793BFF);border-radius:22px; color:rgb(225,224,225); font-size:14px;}"
			"QPushButton::hover{background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop : 0 #4F01D5, stop : 1 #6E2AFF);}");

// RedDetailsItem
const QString RED_DETAILS_ITEM_NAME = QString("QLabel {color:rgb(85, 85, 85); font-size:12px;font-family:Microsoft YaHei;}");
const QString RED_DETAILS_ITEM_AMOUNT = QString("QLabel {color:rgb(255, 44, 108); font-size:12px;font-family:Microsoft YaHei;}");
const QString RED_DETAILS_ITEM_TIME = QString("QLabel {color:rgb(187, 179, 203); font-size:12px;font-family:Microsoft YaHei;}");

// RedDetailsDialog
const QString RED_DETAILS_IMAGE = QString("QLabel {border-image:url(:/res/images/big_red_envelopes.png);}");
const QString RED_DETAILS_SHALLOW = QString("QLabel {color:rgb(95, 91, 115); font-size:14px; background:transparent;font-family:Microsoft YaHei;}");
const QString RED_DETAILS_DEEP = QString("QLabel {color:rgb(27, 20, 67); font-size:14px; background:transparent;font-family:Microsoft YaHei;}");
const QString RED_DETAILS_LABEL = QString("QLabel {color:rgb(95, 91, 115); font-size:12px; background:transparent;font-family:Microsoft YaHei;}");

// SettingDialog
const QString SETTING_SET = QString("QLabel {color:rgb(85, 85, 85); padding-left:20px; font-size:12px; background-image:url(:/res/images/set.png); background-repeat:no-repeat; background-position:top left; background-color:transparent;font-family:Microsoft YaHei;}");
const QString SETTING_CHECK_BOX = QString("QCheckBox{spacing:5px; color:rgb(85, 85, 85); font-size:12px;font-family:Microsoft YaHei;}"
			"QCheckBox::indicator:disabled{color:#555555;}"
			"QCheckBox::indicator{width:16px;height:16px;}"
			"QCheckBox::indicator:unchecked {border-image:url(:/res/images/checkbox-unselect.png) 0 0 0 0 repeat repeat;border-width :0px;border-radius :0px;}"
			"QCheckBox::indicator:unchecked:hover{border-image:url(:/res/images/checkbox_unselect_hover_2.png) 0 0 0 0 repeat repeat;}"
			"QCheckBox::indicator:unchecked:pressed{border-image:url(:/res/images/checkbox_unselect_hover_2.png) 0 0 0 0 repeat repeat;}"
			"QCheckBox::indicator:checked{border-image : url(:/res/images/checkbox_selected_2.png) 0 0 0 0 repeat repeat;border-width:0px;border-radius:0px;}"
			"QCheckBox::indicator:checked:hover{border-image : url(:/res/images/checkbox_selected_2.png) 0 0 0 0 repeat repeat;}"
			"QCheckBox::indicator:checked:pressed{border-image : url(:/res/images/checkbox_selected_2.png) 0 0 0 0 repeat repeat;}");
const QString SETTING_MIN = QString("QLabel {color:rgb(85, 85, 85); font-size:12px;font-family:Microsoft YaHei;}");
const QString SETTING_RADIO_BUTTON = QString("QRadioButton {color:rgb(68, 68, 68); background-color:transparent; font-size:14px;font-family:Microsoft YaHei;}"
			"QRadioButton::indicator {width:19px; height:19px}"
			"QRadioButton::indicator::unchecked {image:url(:/res/images/radio_normal.png);}"
			"QRadioButton::indicator::unchecked:hover {image:url(:/res/images/radio_hover.png);}"
			"QRadioButton::indicator::unchecked:pressed {image:url(:/res/images/radio_hover.png);}"
			"QRadioButton::indicator::checked {image:url(:/res/images/radio_check_normal.png);}"
			"QRadioButton::indicator::checked:hover {image:url(:/res/images/radio_check_normal.png);}"
			"QRadioButton::indicator::checked:pressed {image:url(:/res/images/radio_check_normal.png);}");
const QString SETTING_CLOSE = QString("QLabel {color:rgb(85, 85, 85); font-size:12px;font-family:Microsoft YaHei;}");
const QString SETTING_BUTTON = QString("QPushButton {background-color:transparent; border:0px; outline:0px;}"
			"QPushButton:hover {background-color:rgb(239, 239, 250)}"
			"QPushButton:pressed {background-color:rgb(239, 239, 250)}"
			"QPushButton:disabled {background-color:rgb(225, 225, 225)}");

// LiveStatics
const QString LIVE_END_TIP = QString("font-size:18px;font-weight:600;color:#1B1443;line-height:25px;font-family:Microsoft YaHei;");
const QString LIVE_TIME = QString("font-size:14px;font-weight:400;color:#5F5B73;line-height: 20px;font-family:Microsoft YaHei;");
const QString STATIC_NUM = QString("font-size:20px;color:#1B1443;font-family:Microsoft YaHei;");
const QString STATIC_TXT = QString("font-size:12px;color:#8F8DA5;font-family:Microsoft YaHei;");

// MsgItem
const QString MSG_BROWSER = QString("border:none;background:transparent;");
const QString MSG_LINEAR_GRADIENT = QString("QLabel{background:#634DD0;border-radius:18px;}");
const QString MSG_USER_NAME = QString("font-size:12px;font-weight: 400;color: #C3BEE0;background-color: transparent;line-height: 30px;font-family:Microsoft YaHei;");
const QString MSG_GIFT_TEXT = QString("background-color:transparent;font-size:12px;font-weight:400;color:#FFFFFF;line-height:30px;font-family:Microsoft YaHei;");
const QString MSG_GIFT_ICON = QString("background-color:transparent;");
const QString MSG_GIFT_NUM = QString("background-color:transparent;font-size:18px;font-weight: 500;color:#FFFFFF;line-height: 30px;font-family:Microsoft YaHei;");
const QString MSG_GIFT_RED_PACKET_ICON = QString("background-color:transparent;border-image:url(:/res/images/big_red_envelopes.png);");

// CustomSlider
const QString CUSTOM_SLIDER = QString("QSlide {background-color:red;min-width:5px;max-width:5px;border-radius:3px;margin:-14-7px;border:15px solid red;}"
			"QSlider::add-page:vertical{background-color:#FFAB13;width:4px;}"
			"QSlider::sub-page:vertical{background-color: #F0EFF2;width:4px;}"
			"QSlider::groove:vertical{background:yellow;width:7px;}"
			"QSlider::handle:vertical{background-color: #FFD669;width:14px;height:14px;margin: -0-4px;border-radius:7px;}");

// LeftRightButton
const QString LEFT_RIGHT_LABEL = QString("QLabel {color:rgb(34, 34, 34); font-size:14px; text-align:left; background-color:transparent;font-family:Microsoft YaHei;}");

// RecordSaveTip
const QString RECORD_SAVE_TIP = QString("font-size:12px;color:#383257;background:transparent;font-family:Microsoft YaHei;");
const QString RECORD_VIEW_BTN = QString("QPushButton{background:qlineargradient("
			"spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 #5D00FF,"
			"stop:1 #793BFF);border-radius:12px;font-size:12px;font-family:Microsoft YaHei;"
			"font-weight:400;color:white;}");
const QString RECORD_SET_BTN = QString("QPushButton{background:#F3F3F8;border-radius:12px;font-size:12px;font-family:Microsoft YaHei;"
			"font-weight:400;color:rgb(18,49,255);}");

// WindowCaptureItem
const QString WINDOW_CAPTURE_ITEM_LABEL = QString("QLabel {color:rgb(56, 50, 87); font-size:12px; background-color:transparent;font-family:Microsoft YaHei;}");
const QString WINDOW_CAPTURE_ITEM_SELECT = QString("QPushButton{background-color:transparent;border-image:url(:/res/images/big_check.png);}");

// SourceBaseDialog
const QString SOURCE_BASE_CONFIRM = QString("QPushButton{background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop : 0 #5D00FF, stop : 1 #793BFF);border-radius:18px; color:rgb(225,224,225);}"
			"QPushButton::hover{background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop : 0 #4F01D5, stop : 1 #6E2AFF);}");
const QString SOURCE_BASE_CANCEL = QString("QPushButton {background-color:rgb(243, 243, 248); font-size:14px; color:rgb(93, 0, 255); border-radius:18px;font-family:Microsoft YaHei;}"
			"QPushButton:hover {background-color:rgb(232, 229, 237);}");

// WindowCaptureDialog
const QString WINDOW_CAPTURE_LABEL = QString("QLabel {color:rgb(34, 34, 34); font-size:16px;font-family:Microsoft YaHei;}");
const QString WINDOW_CAPTURE_INFO = QString("QLabel {color:rgb(95, 91, 115); font-size:12px;font-family:Microsoft YaHei;}");
const QString WINDOW_CAPTURE_LIST = QString("QListWidget {border:0px; border-radius:6px; background-color:rgb(232, 229, 237); padding:8px 8px 8px 8px;}"
			"QListWidget:focus {outline: none;} "
			"QListWidget:item {background-color:transparent;}"
			"QListWidget:item:hover {border:0px; background-color:rgb(252, 251, 253);}"
			"QListWidget:item::selected:active {border:0px; background-color:rgb(252, 251, 253);}"
			"QListWidget:item:selected {border:0px; background-color:rgb(252, 251, 253);}");
const QString WINDOW_CAPTURE_SCROLL = QString("QScrollBar:vertical {border:none;margin:0px,0px,0px,0px;width:8px; border-radius:4px;background-color:#E8E5ED;}"
			"QScrollBar:vertical:hover {margin: 0px,0px,0px,0px; background-color:#E8E5ED;}"
			"QScrollBar::handle:vertical {max-width:8px; background-color:#5F5B73;  height: 40px; border-radius: 4px;}"
			"QScrollBar::add-line:vertical {height:0px;}"
			"QScrollBar::sub-line:vertical {height:0px;}"
			"QScrollBar::add-page:vertical {background:transparent;}"
			"QScrollBar::sub-page:vertical {background:transparent;}");

// CameraDialog
const QString CAMERA_COMBOBOX = QString("QComboBox {padding-left:8px; border:1px solid #D3CCDF; border-radius:4px; background:transparent;}"
			"QComboBox::drop-down {border:0px; width:32px;}"
			"QComboBox::down-arrow {border-image:url(:/res/images/arr_bottom.png); width:14px; height:14px;}"
			"QComboBox::down-arrow:hover {border-image:url(:/res/images/arr_bottom_hover.png);}"
			"QComboBox QAbstractItemView {border:1px solid #D3CCDF; outline:none;}"
			"QComboBox QAbstractItemView:item {background-color:rgb(255,255,255); color:rgb(85, 85, 85); height:36px;}"
			"QComboBox QAbstractItemView:item:hover {background-color:rgb(249,248,251); color:rgb(85, 85, 85);}");
const QString CAMERA_REFRESH_BTN = QString("QPushButton {border-image:url(:/res/images/refresh.png);}"
			"QPushButton:hover {border-image:url(:/res/images/refresh_hover.png);}");

// AddSourceButton
const QString SOURCE_BUTTON = QString("QPushButton{background-color:rgb(255,255,255); border: 0px;}"
			"QPushButton:hover{background-color:rgb(239,239,250);}"
			"QPushButton:pressed{background-color:rgb(239,239,250);}"
			"QPushButton:disabled{background-color:rgb(225,225,225);}");
const QString SOURCE_BUTTON_TEXT = QString("QLabel{color:rgb(85,85,85); font-size:12px;font-family:Microsoft YaHei;}");

// AddSourceWidget
const QString SOURCE_WIDGET_BG = QString("QWidget#SourceWidget{border-image:url(:/res/images/add_bg.png)}");

//tab item
const QString TAB_URL_STYLE =QString("QLabel{background-color:transparent; border: 0px;border-image:url(%1);}"
			"QLabel:hover{background-color:transparent;border-image:url(%2);}"
			"QLabel:checked{background-color:transparent;border-image:url(%2);}"
			"QLabel:enabled{background-color:transparent; border: 0px;border-image:url(%2);}"
			"QLabel:disabled{background-color:transparent; border: 0px;border-image:url(%1);}");

// CustomerServiceBaseDialog
const QString CS_BASE_DIALOG_TOP_BG = QString("QWidget#cs_top{border-image:url(:res/images/cs_top_bg.png) 0 24 0 242 strech strech; border-top:0; border-right:24; border-bottom:0; border-left:242;}");
const QString CS_BASE_DIALOG_TITLE = QString("QLabel{color:rgb(34, 34, 34);font-size:14px;background-color:transparent;font-weight:bold; font-family:Microsoft YaHei;}");
const QString CS_BASE_DIALOG_CANCEL = QString("QPushButton {background-color:rgb(255,255,255); border:1px solid #EBEBEF; font-size:14px; color:#222222; border-radius:6px; font-weight: 400; font-family:Microsoft YaHei;}"
			"QPushButton:hover {background-color:rgb(232, 229, 237);}");
const QString CS_BASE_DIALOG_CONFIRM = QString("QPushButton{background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop : 0 #FFDB00, stop : 1 #FFEB00);border-radius:6px; color:rgb(34,34,34); font-size:14px;font-family:Microsoft YaHei;}"
			"QPushButton::hover{background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop : 0 #FFC900, stop : 1 #FFDC00);}");
const QString CS_BASE_DIALOG_CLOSE = QString("QPushButton{border-image:url(:/res/images/close.png); background-color:transparent;}"
			"QPushButton:hover{border-image:url(:/res/images/close_hover_2.png);}"
			"QPushButton:pressed{border-image:url(:/res/images/close_hover_2.png);}");
// EffectDialog
const QString MASK_DIALOG_LIST = QString("QListWidget {border: 0px; background-color:rgb(255,255,255); padding: 15px; margin:0px;}"
			"QListWidget:focus {outline: none;}");
const QString MASK_DIALOG_SCROLL = QString("QScrollBar:vertical {border:none;margin:0px,0px,0px,0px;width:8px; border-radius:4px;background-color:#E8E5ED;}"
			"QScrollBar:vertical:hover {margin: 0px,0px,0px,0px; background-color:#E8E5ED;}"
			"QScrollBar::handle:vertical {max-width:8px; background-color:#5F5B73;  height: 40px; border-radius: 4px;}"
			"QScrollBar::add-line:vertical {height:0px;}"
			"QScrollBar::sub-line:vertical {height:0px;}"
			"QScrollBar::add-page:vertical {background:transparent;}"
			"QScrollBar::sub-page:vertical {background:transparent;}");

// FinishLiveDialog
const QString FINISH_DIALOG_LABEL = QString("QLabel{color:rgb(68, 68, 68);font-size:14px;background-color:transparent;font-family:Microsoft YaHei;}");

// DeviceDialog
const QString DEVICE_LABEL = QString("QLabel{color:rgb(68, 68, 68);font-size:14px; background-color:transparent;font-family:Microsoft YaHei;}");

//alter password
const QString LABEL_TITLE_TEXT = QString("QLabel{color:#222222;font-size:14px;background:transparent;font-family:Microsoft YaHei;}");
const QString LABEL_TEXT = QString("QLabel{color:#444444;font-size:14px;background:transparent;font-family:Microsoft YaHei;}");

const QString LABEL_INPUT_EDIT = QString("QLineEdit{border: 1px solid #E9E9E9;border-radius:4px;font-size:14px;color:black;background:transparent; padding-left:10px;font-family:Microsoft YaHei;}");

//font-set
const QString FONT_SET_COMBOBOX = QString("QComboBox {padding-left:7px; border: 1px solid #E1DEE6; border-radius:4px;}"
			"QComboBox::hover {padding-left:7px; border: 1px solid #A59CB2; border-radius:4px;}"
			"QComboBox::pressed {padding-left:7px; border: 1px solid #A59CB2; border-radius:4px;}"
			"QComboBox::drop-down {border:0px; width:32px;}"
			"QComboBox::down-arrow {border-image:url(:/res/images/arr-right.png); width:9px; height:6px;}"
			"QComboBox::down-arrow:hover {border-image:url(:/res/images/arr-right-hover.png);}"
			"QComboBox QAbstractItemView {background:#FFFFFF;border:1px solid #DAD6E2; outline:none;}"
			"QComboBox QAbstractItemView:item {background-color:rgb(255,255,255); color:#383257; height:36px;}"
			"QComboBox QAbstractItemView:item:hover {background-color:#F0EFF2; color:rgb(85, 85, 85);}");

const QString FONT_SET_COMBO = QString("\
	QComboBox{border-radius: 4px;padding: 0px 0px 0px 0px;background: #FFFFFF;border: 1px solid #E1DEE6;font-weight:400;font-size:12px;color:#383257;line-height:16px;margin-right:16px;font-family:Microsoft YaHei;}\
	QComboBox:hover{border-radius: 4px;border: 1px solid #E1DEE6;}\
	QComboBox:focus{border-radius: 4px;border: 1px solid #E1DEE6;}\
	QComboBox::down-arrow {image: url(:/res/images/arr-right.png);}\
	QComboBox::down-arrow:hover {image: url(:/res/images/arr-right-hover.png);}\
	QComboBox::down-arrow:pressed {image: url(:/res/images/arr-right-hover.png);}\
	QComboBox::drop-down{background-color:transparent;width: 9px;height: 6px;border: 0px;margin-top:11px;margin-right:6px;margin-bottom:11px;}\
	QComboBox::drop-down:hover{background-color:transparent;width: 9px;height: 6px;}\
	QComboBox::drop-down:pressed{background-color:transparent;width: 9px;height: 6px;}\
	QComboBox QAbstractItemView{outline:0px;background-color:white;border:1px solid #DAD6E2;}\
	QComboBox QAbstractItemView::item{height:36px;border:0px;background-color:white;color:#383257;font-size:12px;font-weight:400;line-height:26px;font-family:Microsoft YaHei;}\
	QComboBox QAbstractItemView::item:hover{height:30px;background-color:#EOEFF2;color:#383257;font-size:12px;font-weight:400;line-height:26px;font-family:Microsoft YaHei;}\
	QComboBox QAbstractItemView::item:selected{height:30px;background-color:#EOEFF2;color:#383257;font-size:12px;font-weight:400;line-height:26px;font-family:Microsoft YaHei;}");

const QString FONT_SET_TXT = QString(
	"margin-left:5px;margin-top:5px;margin-right:5px;margin-bottom:5px;"
	"background: white;"
	"border-radius: 4px;border: 1px solid #D9D4E3;"
	"font-size: 14px;font-family:Microsoft YaHei;"
	"font-weight: 400;color: #5c567b;line-height: 22px;");
const QString FONT_SET_COUNT = QString(
	"background-color:transparent;margin-left:5px;border-radius:6px;"
	"font-size: 12px;font-family:Microsoft YaHei;"
	"font-weight: 400;color: #9693A9;line-height: 16px;");
const QString FONT_ADD_BTN = QString("QPushButton{background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop : 0 #FFDB00, stop : 1 #FFEB00);border-radius:6px; color:#222222;}"
	"QPushButton::hover{background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop : 0 #FFC900, stop : 1 #FFDC00);}");

const QString FONT_SET_CANCLE = QString("QPushButton {background-color:rgb(255,255,255); border:1px solid #EBEBEF; font-size:14px; color:#222222; border-radius:6px; font-weight: 400; font-family:Microsoft YaHei;}"
			"QPushButton:hover {background-color:rgb(232, 229, 237);}");

const QString FontScroll = QString("QScrollBar:vertical\
    {\
        width: 3px;\
        min-height: 19px;\
        margin: 0px;\
        padding: 0px;\
	border: none;\
        background-color: transparent;\
    }\
    QScrollBar::handle:vertical\
    {\
        width: 3px;\
        min-height: 19px;\
        border: none;\
        border-radius: 1px;\
        background-color: #c4c0cb;\
    }\
    QScrollBar::sub-line:vertical\
    {\
        height: 0px;\
        border-width: 0px;\
        subcontrol-position: top;\
	background-color: transparent;\
    }\
    QScrollBar::add-line:vertical\
    {\
        height: 0px;\
        border-width: 0px;\
        subcontrol-position: bottom;\
	background-color: transparent;\
    }\
    QScrollBar::down-arrow:vertical, QScrollBar::up-arrow:vertical\
    {\
        background: none;\
	background-color: transparent;\
    } \
    QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical\
    {\
        margin: 0px 0px 0px 0px;\
        background-color: transparent;\
    }");
const QString FONT_COUNT_TIPS = QString("QLabel{background:transparent;"
	"font-family:Microsoft YaHei;font-weight:400;color:#FF4D4F;font-size:12px;"
	"}");

#endif // !CUSTOM_STYLESHEET_H
