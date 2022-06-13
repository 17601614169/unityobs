#include "window-basic-main.hpp"
#include <QDebug>
#include <QTextCodec>
#include <QJsonObject>
#include <QJsonDocument>
#include "asyn-request.h"
#include "json-parser.h"
#include "characters-convert.h"
#include "custom-global-config.h"
#include "bottom-ctrls.h"
#include "record-save-tip.h"
#include "set-dialog.h"
#include "top-widget.h"
#include "user-info.h"
#include "im-widget.h"
#include "live_statics.h"
#include "sacrouge-dialog.h"
#include "source-base-dialog.h"
#include "qt-wrappers.hpp"
#include "left-widget.h"
#include "string-convert.h"
#include "webengine-widget.h"
#include "customer-service-base-dialog.h"
#include "custom-config-ini.h"
#include "window-basic-main-outputs.hpp"
#include "msgbox.h"
#include <QDir>

#ifdef _WIN32
#include <Windows.h>
#endif

extern void AddSecret();
extern void DeleteSecret();
struct AddSourceData {
	obs_source_t *source;
	bool visible;
};
static void AddSource2(void *_data, obs_scene_t *scene)
{
	AddSourceData *data = (AddSourceData *)_data;
	obs_sceneitem_t *sceneitem;

	sceneitem = obs_scene_add(scene, data->source);
	obs_sceneitem_set_visible(sceneitem, data->visible);
}



static void AddSource(void *_data, obs_scene_t *scene)
{
	AddSourceData *data = (AddSourceData *)_data;
	obs_sceneitem_t *sceneitem;

	sceneitem = obs_scene_add(scene, data->source);
	obs_sceneitem_set_visible(sceneitem, data->visible);
}

static void AddCamera()
{
	const char *camera_name = DataConfig::GetInstance()->GetCameraName();
	std::string strName;
	if (!camera_name || !*camera_name){
		qDebug() << "none value";
		strName = StringConvert::GetInstance()->GetString(STR_CAMERA).toStdString();
		DataConfig::GetInstance()->SetCameraName(strName.c_str());
	}
	else {
		strName = camera_name;
	}
	const char *id = "dshow_input";
	const char *name = strName.c_str();
	bool visible = true;
	OBSBasic *main = reinterpret_cast<OBSBasic *>(App()->GetMainWindow());
	OBSScene scene = main->GetCurrentScene();
	bool success = false;
	if (!scene)
		return ;
	obs_source_t *source = obs_get_source_by_name(name);
	obs_sceneitem_t *item = obs_scene_find_source(scene, name);
	if (!source || !item) {
		const char *v_id = obs_get_latest_input_type_id(id);
		source = obs_source_create(v_id, name, NULL, nullptr);
		obs_source_set_image_path(source, "");
		obs_source_set_take_effect(source, false);
		obs_source_set_change_path(source, false);
		obs_source_set_mosaic(source, false);
		if (source) {
			AddSourceData data;
			data.source = source;
			data.visible = visible;

			obs_enter_graphics();
			obs_scene_atomic_update(scene, AddSource, &data);
			obs_leave_graphics();

			/* set monitoring if source monitors by default */
			uint32_t flags = obs_source_get_output_flags(source);
			if ((flags & OBS_SOURCE_MONITOR_BY_DEFAULT) != 0) {
				obs_source_set_monitoring_type(
					source,
					OBS_MONITORING_TYPE_MONITOR_ONLY);
			}

			success = true;
		}
	}
	if (success && source) {
		main->CreateCameraWindow(source);
		

		obs_data_t *setting = obs_source_get_settings(source);
		obs_data_set_int(setting, "res_type", 1);
		obs_data_set_string(setting, "resolution", "1280x720");
		obs_source_update(source, setting);
		obs_data_release(setting);

		std::string face_mask_name = obs_source_get_display_name("new_mask_face");
		obs_source_t *face_mask_existing_filter = obs_source_get_filter_by_name(source, face_mask_name.c_str());
		if (!face_mask_existing_filter) {
			obs_source_t *filter = obs_source_create("new_mask_face", face_mask_name.c_str(), nullptr, nullptr);
			if (filter) {
				obs_source_filter_add(source, filter);
				obs_source_release(filter);
			}
		}
		obs_source_release(face_mask_existing_filter);

		std::string clut_filter_name = obs_source_get_display_name("clut_filter");
		obs_source_t *clut_filter_source = obs_source_get_filter_by_name(source, clut_filter_name.c_str());
		if (!clut_filter_source) {
			clut_filter_source =obs_source_create("clut_filter",clut_filter_name.c_str(),nullptr,nullptr);
			obs_source_filter_add(source, clut_filter_source);

			int type = DataConfig::GetInstance()->GetFilterType();
			QString filterPath = common::GetFilterPath((Filter_Type)type);
			if (filterPath.isEmpty()) {
				QString path = QCoreApplication::instance()->applicationDirPath();
				path += "/../../data/obs-plugins/obs-filters/LUTs/original.png";
				filterPath = path;
				DataConfig::GetInstance()->SetFilterType(0);
			} 
			obs_data_t *setting = obs_source_get_settings(clut_filter_source);
			obs_data_set_string(setting, "image_path", QT_TO_UTF8(filterPath));
			obs_source_update(clut_filter_source, setting);
			obs_data_release(setting);
		}
		obs_source_release(clut_filter_source);

		std::string skin_white_filter_name = obs_source_get_display_name("skin_white_filter");
		obs_source_t *skin_white_filter_source = obs_source_get_filter_by_name(source, skin_white_filter_name.c_str());
		if (!skin_white_filter_source) {
			skin_white_filter_source =obs_source_create("skin_white_filter",skin_white_filter_name.c_str(),nullptr,nullptr);
			obs_source_filter_add(source, skin_white_filter_source);

			const char *value = DataConfig::GetInstance()->GetSkinWhiteValue();
			if (!value) {
				DataConfig::GetInstance()->SetSkinWhiteValue("50");
				value = "50";
			} 
			obs_data_t *setting = obs_source_get_settings(skin_white_filter_source);
			QString path = QCoreApplication::instance()->applicationDirPath();
			path += "/../../data/obs-plugins/obs-filters/LUTs/skin_lookup.png";
			obs_data_set_string(setting, "image_path", QT_TO_UTF8(path));
			obs_data_set_double(setting, "clut_amount", QT_UTF8(value).toDouble() / 100.0);
			obs_source_update(skin_white_filter_source, setting);
			obs_data_release(setting);
		}
		obs_source_release(skin_white_filter_source);

		std::string moulting_filter_name = obs_source_get_display_name("moulting_filter");
		obs_source_t *moulting_filter_source = obs_source_get_filter_by_name(source, moulting_filter_name.c_str());
		if (!moulting_filter_source) {
			moulting_filter_source =obs_source_create("moulting_filter",moulting_filter_name.c_str(),nullptr,nullptr);
			obs_source_filter_add(source, moulting_filter_source);

			const char *value = DataConfig::GetInstance()->GetMoultingValue();
			if (!value) {
				DataConfig::GetInstance()->SetMoultingValue("8");
				value = "8";
			} 
			obs_data_t *setting = obs_source_get_settings(moulting_filter_source);
			obs_data_set_double(setting, "strongly", QT_UTF8(value).toDouble() / 100.0);
			obs_source_update(moulting_filter_source, setting);
			obs_data_release(setting);
		}
		obs_source_release(moulting_filter_source);

		std::string ruddy_filter_name = obs_source_get_display_name("ruddy_filter");
		obs_source_t *ruddy_filter_source = obs_source_get_filter_by_name(source, ruddy_filter_name.c_str());
		if (!ruddy_filter_source) {
			ruddy_filter_source =obs_source_create("ruddy_filter",ruddy_filter_name.c_str(),nullptr,nullptr);
			obs_source_filter_add(source, ruddy_filter_source);

			const char *value = DataConfig::GetInstance()->GetRuddyValue();
			if (!value) {
				DataConfig::GetInstance()->SetRuddyValue("50");
				value = "50";
			} 
			obs_data_t *setting = obs_source_get_settings(ruddy_filter_source);
			obs_data_set_double(setting, "strongly", QT_UTF8(value).toDouble() / 200.0 + 1);
			obs_source_update(ruddy_filter_source, setting);
			obs_data_release(setting);
		}
		obs_source_release(ruddy_filter_source);

		std::string face_lift_filter_name = obs_source_get_display_name("face_lift_filter");
		obs_source_t *face_lift_filter_source = obs_source_get_filter_by_name(source, face_lift_filter_name.c_str());
		if (!face_lift_filter_source) {
			face_lift_filter_source =obs_source_create("face_lift_filter",face_lift_filter_name.c_str(),nullptr,nullptr);
			obs_source_filter_add(source, face_lift_filter_source);

			const char *face_value = DataConfig::GetInstance()->GetFaceLiftValue();
			if (!face_value) {
				DataConfig::GetInstance()->SetFaceLiftValue("8");
				face_value = "8";
			} 
			obs_data_t *setting = obs_source_get_settings(face_lift_filter_source);
			obs_data_set_double(setting, "face_strongly", QT_UTF8(face_value).toDouble() / 100.0);

			const char *eye_value = DataConfig::GetInstance()->GetEyesReshapeValue();
			if (!eye_value) {
				DataConfig::GetInstance()->SetEyesReshapeValue("8");
				eye_value = "8";
			} 
			obs_data_set_double(setting, "eye_strongly", QT_UTF8(eye_value).toDouble() / 100.0);

			const char *thin_value = DataConfig::GetInstance()->GetThinChinValue();
			if (!thin_value) {
				DataConfig::GetInstance()->SetThinChinValue("8");
				thin_value = "8";
			} 
			obs_data_set_double(setting, "thin_strongly", QT_UTF8(thin_value).toDouble() / 100.0);

			obs_source_update(face_lift_filter_source, setting);
			obs_data_release(setting);
		}
		obs_source_release(face_lift_filter_source);

		std::string blusher_name = obs_source_get_display_name("blusher");
		obs_source_t *blusher_source = obs_source_get_filter_by_name(source, blusher_name.c_str());
		if (!blusher_source) {
			blusher_source =obs_source_create("blusher",blusher_name.c_str(),nullptr,nullptr);
			obs_source_filter_add(source, blusher_source);

			const char *value = DataConfig::GetInstance()->GetBlusherValue();
			if (!value) {
				DataConfig::GetInstance()->SetBlusherValue("16");
				value = "16";
			} 
			obs_data_t *setting = obs_source_get_settings(blusher_source);
			obs_data_set_double(setting, "strongly", QT_UTF8(value).toDouble() / 200.0);
			obs_source_update(blusher_source, setting);
			obs_data_release(setting);
		}
		obs_source_release(blusher_source);


		//初始化默认添加的滤镜
		
		std::string daoju_name = obs_source_get_display_name("old_face_mask_filter");
		obs_source_t *daoju_existing_filter = obs_source_get_filter_by_name(source, daoju_name.c_str());
		if (!daoju_existing_filter) {
			obs_source_t *filter = obs_source_create("old_face_mask_filter", daoju_name.c_str(), nullptr, nullptr);
			if (filter) {
				obs_source_filter_add(source, filter);
				obs_source_release(filter);
			}
		}
		obs_source_release(daoju_existing_filter);
	}
	if(source){
		item = obs_scene_find_source(scene, name);
		if(item){
			obs_sceneitem_set_locked(item,false);
			obs_sceneitem_set_order(item,OBS_ORDER_MOVE_BOTTOM);
			main->StretchToScreen(item);
			obs_sceneitem_set_locked(item,true);
		}
		
	}
	obs_source_release(source);
}

static void AddCloseSource() {
	OBSBasic *main = reinterpret_cast<OBSBasic *>(App()->GetMainWindow());
	OBSScene scene = main->GetCurrentScene();
	if (!scene)
		return;

	const char *close_source_name = DataConfig::GetInstance()->GetCloseSourceName();
	std::string strName;
	if (!close_source_name || !*close_source_name){
		qDebug() << "none value";
		strName = StringConvert::GetInstance()->GetString(STR_CLOSE_SOURCE).toStdString();
		DataConfig::GetInstance()->SetCloseSourceName(strName.c_str());
	}
	else {
		strName = close_source_name;
	}
	main->m_strCloseName = strName;

	if (!strName.empty()) {
		obs_source_t *source = obs_get_source_by_name(strName.c_str());
		obs_sceneitem_t *close_item = obs_scene_find_source(scene, strName.c_str());
		if (!source || !close_item) {
			source = obs_source_create("image_source", strName.c_str(), NULL, nullptr);

			if (source) {
				AddSourceData data;
				data.source = source;
				data.visible = true;

				obs_enter_graphics();
				obs_scene_atomic_update(scene, AddSource, &data);
				obs_leave_graphics();

				/* set monitoring if source monitors by default */
				uint32_t flags = obs_source_get_output_flags(source);
				if ((flags & OBS_SOURCE_MONITOR_BY_DEFAULT) != 0) {
					obs_source_set_monitoring_type(
						source,
						OBS_MONITORING_TYPE_MONITOR_ONLY);
				}

				obs_properties_t *properties = obs_source_properties(source);
				obs_property_t *property = obs_properties_first(properties);
				while (property) {
					const char *setting = obs_property_name(property);
					if (strcmp(setting, "file") == 0) {
						obs_data_t *settings = obs_source_get_settings(source);
						QString strFile = QApplication::applicationDirPath() + "\\images\\close.png";
						obs_data_set_string(settings, setting, strFile.toUtf8());
						obs_source_update(source, settings);
						obs_data_release(settings);
						break;
					}
					obs_property_next(&property);
				}
				obs_properties_destroy(properties);
			}

		}
		if (!close_item)
			close_item = obs_scene_find_source(scene, strName.c_str());
		if(close_item){
			obs_sceneitem_set_order(close_item,OBS_ORDER_MOVE_TOP);
			obs_sceneitem_set_visible(close_item,false);
			obs_sceneitem_select(close_item, false);
		}
		obs_source_release(source);
	}
}

void OBSBasic::InitStompClient()
{
	return;
#if 0
	if (m_pStompClient == nullptr) {
		m_pStompClient = QSharedPointer<QStompClient>::create();
		qRegisterMetaType<QStompResponseFrame>("QStompResponseFrame");
		if (m_pStompClient.get()) {
			connect(m_pStompClient.get(),
				&QStompClient::websocketConnected, this,
				&OBSBasic::OnSocketConnected,
				Qt::QueuedConnection);
			connect(m_pStompClient.get(),
				&QStompClient::websocketDisConnected, this,
				&OBSBasic::OnSocketDisConnected,
				Qt::QueuedConnection);
			connect(m_pStompClient.get(),
				&QStompClient::messageFrame, this,
				&OBSBasic::OnMessageFrame,Qt::QueuedConnection);
		}
	}
#endif
}

void OBSBasic::GetLiveList() {
	STReqData req;
	req.func = FUNC_LIVE_LIST;
	req.method = GET;
	req.list.append(qMakePair(QByteArray("Content-Type"),
				  QByteArray ("application/json")));
	req.list.append(qMakePair(QByteArray("appCode"), QByteArray("keepfun_sportlive")));
	req.list.append(qMakePair(
		QByteArray("token"),
		QByteArray(DataConfig::GetInstance()->GetLoginToken().c_str())));
	sprintf(req.url, "%s%s", BASE_URL.c_str(), "/app/sys/label/v1/list/0");
	blog(LOG_INFO, "request %s", req.url);
	AsynReqManager::GetInstance()->InsertReq(req);
}

void OBSBasic::StartStreamTimer(){
	m_bStreamOperate = true;
	if (m_pStreamTimer) {
		m_pStreamTimer->start();
	}
}

void OBSBasic::BeginLive() {
	STReqData req;
	req.func = FUNC_BEGIN_LIVE;
	req.method = GET;
	req.list.append(qMakePair(QByteArray("Accept"),
				  QByteArray("application/json")));
	req.list.append(qMakePair(QByteArray("platform"),
				  QByteArray("3")));
	req.list.append(qMakePair(
		QByteArray("token"),
		QByteArray(DataConfig::GetInstance()->GetLoginToken().c_str())));
	QString strTitle;
	if (strTitle.isEmpty()) {
		strTitle = QString("Hello");
	}
	std::string str1 = strTitle.toStdString();
	std::string str2 = CharatersConvert::UTF8ToGBK(str1);

	//房间id 5398
#if 0
	std::string api = std::string("/shop/live/info/open/") + std::string("5432");
#else
	std::string api = std::string("/shop/live/info/open/") + m_strRoomId.toStdString();
#endif
	std::string url = BASE_URL + api;
	sprintf(req.url, "%s", url.c_str());
	blog(LOG_INFO, "request %s", url.c_str());
	AsynReqManager::GetInstance()->InsertReq(req);
	m_bStartStream = true;
}

void OBSBasic::CheckHearBeat()
{
	if (m_bStartStream) {
		STReqData req;
		req.func = FUNC_HEART_BEAT;
		req.method = GET;
		req.list.append(qMakePair(QByteArray("Accept"),
					  QByteArray("application/json")));
		req.list.append(qMakePair(QByteArray("platform"),
				  QByteArray("5")));
		std::string token = DataConfig::GetInstance()->GetLoginToken();
		STBeginRsp stBeginRsp;
		stBeginRsp = DataConfig::GetInstance()->GetBeginRsp();
		req.list.append(qMakePair(QByteArray("token"),
					  QByteArray(token.c_str())));
		sprintf(req.url,
			"%s/shop/live/info/heart/beat?liveId=%d",
			BASE_URL.c_str(),stBeginRsp.liveId);
		AsynReqManager::GetInstance()->InsertReq(req);
	}
}

void OBSBasic::StartSignInHearBeat(){
	if (m_pCheckInHeartbeat!=nullptr)
		m_pCheckInHeartbeat->start();
}

void OBSBasic::CheckSignInHearBeat() {
	STReqData req;
	req.func = FUNC_SIGNIN_HEART_BEAT;
	req.method = POST;
	req.list.append(qMakePair(QByteArray("Content-Type"),
				  QByteArray("application/json")));
	std::string token = DataConfig::GetInstance()->GetLoginToken();
	req.list.append(qMakePair(QByteArray("token"),
				  QByteArray(token.c_str())));
	sprintf(req.url,
		"%s/shop/service/heart",
		BASE_URL.c_str());
	sprintf_s(req.body,"{\"shopId\":0,\"status\":0,\"sysUserId\":0}");
	AsynReqManager::GetInstance()->InsertReq(req);
}

void OBSBasic::InitConnections() {
	connect(AsynReqManager::GetInstance(),&AsynReqManager::readyData,
		this,&OBSBasic::OnRecvNetSlot);
	connect(ui->statusbar, &OBSBasicStatusBar::recordSignal, this,
		&OBSBasic::OnRecordSlot);
}


void OBSBasic::OnRecordSlot(bool start, int total)
{
	qDebug() << "start:" << start << ",total:" << total;
	//if (DataConfig::GetInstance()->GetRecord()) {
	//	if (m_pBottomCtrl != nullptr) {
	//		m_pBottomCtrl->SetRecord(start, total);
	//		if (!start) {
	//			QPoint pos;
	//			int w = 0, h = 0;
	//			m_pBottomCtrl->GetRecordBtnGlobalPos(pos, w, h);
	//			m_pRecordSaveTip->ResetPos(pos, w, h);
	//			m_pRecordSaveTip->ShowWithLimitTime();
	//			//add by huchenzhi
	//			DataConfig::GetInstance()->SetRecord(false);
	//		}
	//	}
	//}
}

void OBSBasic::OnAddSource(QString str)
{
	std::string string = str.toStdString();
	const char *cstr = string.c_str();
	AddSource(cstr);
}

void OBSBasic::OnMoveSourceItem(void *scene, int move)
{
	OBSSceneItem *item = static_cast<OBSSceneItem *>(scene);
	if (move == 1) {
		obs_sceneitem_set_order(*item, OBS_ORDER_MOVE_DOWN);
	} else if (move == -1) {
		obs_sceneitem_set_order(*item, OBS_ORDER_MOVE_UP);
	}
}

void OBSBasic::OnSelectSourceItem(void *currentScene, void *previousScene)
{
	if (currentScene) {
		OBSSceneItem *item = static_cast<OBSSceneItem *>(currentScene);
		ui->sources->SelectItem(item->Get(), true);

		if (previousScene) {
			OBSSceneItem *preitem =
				static_cast<OBSSceneItem *>(previousScene);
			ui->sources->SelectItem(preitem->Get(), false);
		}

		UpdateContextBar();
	}
}

void OBSBasic::OnEditClicked(void *scene)
{
	if (scene) {
		OBSSceneItem *item = static_cast<OBSSceneItem *>(scene);
		obs_source_t *source = obs_sceneitem_get_source(*item);
		const char *id = obs_source_get_id(source);
		if (source) {
			if (stricmp(id, "window_capture") == 0) {
				WindowCaptureDialog dialog(GetCurrentScene(), source, this);
				dialog.exec();
			} else if (stricmp(id, "dshow_input") == 0) {
				CameraDialog dialog(GetCurrentScene(), source, this);
				dialog.exec();
			}else if(stricmp(id, "text_gdiplus") == 0 || stricmp(id, "text_gdiplus_v2") == 0){
				FontSet* set = new FontSet(source,this);
				connect(set,&FontSet::addTxtSignal,this,&OBSBasic::OnAddFontTxt);
				set->setAttribute(Qt::WA_DeleteOnClose);
				set->raise();
				set->show();
			} else {
				CreatePropertiesWindow(source);
			}
		}
	}
}

void OBSBasic::OnSettingButtonClicked(int id)
{
	if (id == SET_TYPE::TYPE_MIN_TASKBAR) {
		config_set_bool(GetGlobalConfig(), "BasicWindow",
				"SysTrayMinimizeToTray", false);
	} else if (id == SET_TYPE::TYPE_MIN_TRAY) {
		config_set_bool(GetGlobalConfig(), "BasicWindow",
				"SysTrayMinimizeToTray", true);
	} else if (id == SET_TYPE::TYPE_CLOSE_EXIT) {
		DataConfig::GetInstance()->SetCloseType(false);
	} else if (id == SET_TYPE::TYPE_CLOSE_TRAY) {
		DataConfig::GetInstance()->SetCloseType(true);
	} else if (id == SET_TYPE::TYPE_UPDATE) {

	} else if (id == SET_TYPE::TYPE_CLOSE_WINDOW) {
		OnCloseWindow();
	}
}

void OBSBasic::OnSysButtonClicked(int id) {
	Qt::WindowStates state = windowState();
	if (id == TitleButtonType::TYPE_MESSAGE) {
		NotificationDialog dialog(DataConfig::GetInstance()->GetLoginToken(), this);
		if (m_pTopWidget)
			connect(&dialog, &NotificationDialog::OperationMessage, m_pTopWidget, &TopWidget::OnTimeout, Qt::UniqueConnection);
		dialog.exec();
	} else if (id == TitleButtonType::TYPE_MIN) {
		blog(LOG_INFO,"%s","WindowMinimized");
		setWindowState(windowState() | Qt::WindowMinimized);
		ui->chatwidget->setFixedWidth(282);
	} else if (id == TitleButtonType::TYPE_MAX) {
		blog(LOG_INFO,"%s","WindowMaximized");
		setWindowState(windowState() | Qt::WindowMaximized);
		ui->chatwidget->setFixedWidth(282);
	} else if (id == TitleButtonType::TYPE_RESTORE) {
		blog(LOG_INFO,"%s","WindowMaximized");
		setWindowState(windowState() & ~Qt::WindowMaximized);
		Qt::WindowStates state = windowState();
		if ((state&Qt::WindowNoState)==Qt::WindowNoState){
			blog(LOG_INFO,"restore %s","WindowNoState");
			ui->chatwidget->setFixedWidth(282);
		}else if((state&Qt::WindowMinimized)==Qt::WindowMinimized){
			blog(LOG_INFO,"restore %s","WindowMaximized");
			ui->chatwidget->setFixedWidth(282);
		}else if((state&Qt::WindowMaximized)==Qt::WindowMaximized){
			blog(LOG_INFO,"restore %s","WindowMaximized");
			ui->chatwidget->setFixedWidth(282);
		}
	}
}

void OBSBasic::OnCloseWindow() {
	if (DataConfig::GetInstance()->GetCloseType()) {
		ToggleShowHide();
	} else {
		close();
	}
}

void OBSBasic::OnExitWindow(){
	restart = true;
	close();
}

void OBSBasic::FinishLive()
{
	STReqData req;
	req.func = FUNC_FINISH_LIVE;
	req.method = GET;
	req.list.append(qMakePair(QByteArray("Accept"),
				  QByteArray("application/json")));
	req.list.append(qMakePair(QByteArray("platform"),
				  QByteArray("3")));
	req.list.append(qMakePair(
		QByteArray("token"),
		QByteArray(DataConfig::GetInstance()->GetLoginToken().c_str())));

	STBeginRsp stBeginRsp;
	stBeginRsp = DataConfig::GetInstance()->GetBeginRsp();
	sprintf(req.url, "%s/shop/live/info/close/%d", BASE_URL.c_str(),
		stBeginRsp.liveId);
	blog(LOG_INFO, "request %s", req.url);
	AsynReqManager::GetInstance()->InsertReq(req);
}

void OBSBasic::GetRoomInfo() {
	STReqData req;
	req.func = FUN_ROOM_INFO;
	req.method = GET;
	req.list.append(qMakePair(QByteArray("Accept"),
				  QByteArray("application/json")));
	req.list.append(qMakePair(QByteArray("appCode"),
				  QByteArray("keepfun_sportlive")));
	req.list.append(qMakePair(QByteArray("terminalType"), QByteArray("1")));
	req.list.append(qMakePair(QByteArray("token"), QByteArray(DataConfig::GetInstance()->GetLoginToken().c_str())));
	STBeginRsp stBeginRsp;
	stBeginRsp = DataConfig::GetInstance()->GetBeginRsp();
	sprintf(req.url, "%s/app/room/v1/info?roomId=%d", BASE_URL.c_str(),
		stBeginRsp.roomId);
	blog(LOG_INFO, "request %s", req.url);
	AsynReqManager::GetInstance()->InsertReq(req);
}

void OBSBasic::GetGiftList() {
	STReqData req;
	req.func = FUNC_GIFT_LIST;
	req.method = GET;
	req.list.append(qMakePair(QByteArray("Accept"),
				  QByteArray("application/json")));
	req.list.append(qMakePair(QByteArray("appCode"),
				  QByteArray("keepfun_sportlive")));
	req.list.append(qMakePair(QByteArray("terminalType"), QByteArray("1")));
	int pageIndex = 1;
	int pageSize = 100;
	sprintf(req.url,
		"%s/app/gift/bag/v1/list?page=%d&size=%d&appCode=keepfun_sportlive",
		BASE_URL.c_str(), pageIndex, pageSize);
	blog(LOG_INFO, "request %s", req.url);
	AsynReqManager::GetInstance()->InsertReq(req);
}

void OBSBasic::OnShowRedEnvelops() {
	if (m_pSacrougeDialog) {
		delete m_pSacrougeDialog;
		m_pSacrougeDialog = nullptr;
	}
	m_pSacrougeDialog =
		new SacrougeDialog(DataConfig::GetInstance()->GetBeginRsp(),
				   DataConfig::GetInstance()->GetLoginToken(), this);
	m_pSacrougeDialog->show();
}

void OBSBasic::OnSocketConnected()
{
#if 0
	if (m_pStompClient.get()) {
		STCahtTokenRsp rsp;
		rsp = DataConfig::GetInstance()->GetCahtTokenRsp();
		if (rsp.success) {
			m_pStompClient->ConnectFrame(rsp);
		}
	}
#endif
}

void OBSBasic::OnSocketDisConnected() {
	if (m_pIMWidget != nullptr) {
		m_pIMWidget->SetConnected(false);
	}
}

void OBSBasic::ConnectWebSocket()
{
#if 0
	STCahtTokenRsp stCahtTokenRsp =
		DataConfig::GetInstance()->GetCahtTokenRsp();
	if (m_pStompClient.get() && stCahtTokenRsp.success) {
#if 0
		m_pStompClient->ConnectWebSocket(
			QString("ws://47.103.97.102:8081/weim/websocket"));
#else
		m_pStompClient->ConnectWebSocket(
			QString("wss://im.keepfun.cn/apis/message/weim/websocket"));
#endif
		
	}
#endif
}

void OBSBasic::SubscribeChat(bool sub)
{
#if 0
	if (m_pStompClient.get()) {
		if (sub)
			m_pStompClient->Subscribe();
		else
			m_pStompClient->Unsubscribe();
	}
#endif
}

void OBSBasic::SendChat() {
#if 0
	if (m_pStompClient.get()) {
		m_pStompClient->TestSend();
	}
#endif
}

void OBSBasic::ShowEndLiveInfo()
{
	return;
	STReqData req;
	req.func = FUNC_FINISH_LIVE;
	req.method = GET;
	req.list.append(qMakePair(QByteArray("Accept"),
				  QByteArray("application/json")));
	req.list.append(qMakePair(QByteArray("appCode"),
				  QByteArray("keepfun_sportlive")));
	STBeginRsp stBeginRsp;
	stBeginRsp = DataConfig::GetInstance()->GetBeginRsp();
	sprintf(req.url, "%s/shop/live/info/close/%d", BASE_URL.c_str(),
		stBeginRsp.liveId);
	blog(LOG_INFO, "request %s", req.url);
	LiveStatics *pSta = new LiveStatics(req);
	pSta->setModal(true);
	pSta->setAttribute(Qt::WA_DeleteOnClose);
	pSta->show();
}

void OBSBasic::OnRecvNetSlot(const STRspData &stRsp) {
	switch (stRsp.func) {
	case FUNC_LIVE_LIST: {
		STLiveList lists;
		lists = ParserLiveList(stRsp.recvBytes);
		DataConfig::GetInstance()->SetLiveList(lists);
	} break;
	case FUNC_BEGIN_LIVE: {
		STBeginRsp stBeginRsp;
		stBeginRsp = ParseBegin(stRsp.recvBytes);
		stBeginRsp.liveId = atoi(m_strRoomId.toStdString().c_str());
		stBeginRsp.roomId = atoi(m_strRoomId.toStdString().c_str());
		stBeginRsp.userId = DataConfig::GetInstance()->GetLoginInfo().stService.stUser.id;
		if (stBeginRsp.success) {
			DataConfig::GetInstance()->SetBeginRsp(stBeginRsp);

			char data[2048] = {0};
			sprintf(data,IMCHAT_HTML.c_str(),DataConfig::GetInstance()->GetLoginToken().c_str(),
			DataConfig::GetInstance()->GetImToken().c_str(),
			DataConfig::GetInstance()->GetSysUserId(),
			DataConfig::GetInstance()->GetImId().c_str());
			blog(LOG_INFO,"IM_CHAT_URL:%s",data);
			QString strData = QString(data);
			m_pChatWebWidget->Load(data);

			blog(LOG_INFO, "live_id:%d", stBeginRsp.liveId);
			PermissionStream();
			QString strScript = QString("liveStart('%1','%2');");
			strScript = strScript.arg(m_strRoomId.toStdString().c_str())
					     .arg(m_strLiveId.toStdString().c_str());
			blog(LOG_INFO,"IM_CHAT:%s",strScript.toStdString().c_str());
			if (m_pChatWebWidget)
				m_pChatWebWidget->CallJavaScript(strScript);
			
		} else {
			blog(LOG_INFO, "begin live failed,login token:%s",
			     DataConfig::GetInstance()->GetLoginToken().c_str());
		}
	} break;
	case FUNC_GET_CHAT_TOKEN: {
		STCahtTokenRsp stCahtTokenRsp;
		stCahtTokenRsp = ParseGetToken(stRsp.recvBytes);
		STBeginRsp stBegin = DataConfig::GetInstance()->GetBeginRsp();
		stCahtTokenRsp.stUserInfo.groupId = stBegin.roomId;
		if (stCahtTokenRsp.success) {
			blog(LOG_INFO, " chat token:%s",
			     stCahtTokenRsp.token.c_str());
			DataConfig::GetInstance()->SetCahtTokenRsp(
				stCahtTokenRsp);
			QTimer::singleShot(100, [&]() { ConnectWebSocket(); });
		} else {
			blog(LOG_INFO, "get chat token  failed,login token:%s",
			     DataConfig::GetInstance()->GetLoginToken().c_str());
		}

	} break;
	case FUNC_FINISH_LIVE: {
		STFinishRsp info = ParseFinish(stRsp.recvBytes);
		if (info.success) {

			bool confirm = config_get_bool(GetGlobalConfig(), "BasicWindow",
					       "WarnBeforeStoppingStream");

			if (confirm && isVisible()) {
				QMessageBox::StandardButton button =
					OBSMessageBox::question(
						this, QTStr("ConfirmStop.Title"),
						QTStr("ConfirmStop.Text"),
						QMessageBox::Yes | QMessageBox::No,
						QMessageBox::No);

				if (button == QMessageBox::No) {
					ui->streamButton->setChecked(true);
					return;
				}
			}
			StopStreaming();

			CleanSource();

			QString strScript = QString("liveFinish('%1','%2');");
			strScript = strScript.arg(m_strRoomId.toStdString().c_str())
					     .arg(m_strLiveId.toStdString().c_str());
			blog(LOG_INFO,"IM_CHAT:%s",strScript.toStdString().c_str());
			if (m_pChatWebWidget)
				m_pChatWebWidget->CallJavaScript(strScript);
		}else{
			MsgBox::information(nullptr,
				StringConvert::GetInstance()->GetString(STR_PROCESS),
				QString::fromStdString(info.appMsg));
		}
	} break;
	case FUNC_GIFT_LIST: {
		STGiftList giftList;
		giftList = ParseGiftList(stRsp.recvBytes);
	} break;
	case FUN_ROOM_INFO: {
		STRoomInfo roomInfo;
		(STNoticeRsp&)roomInfo = ParseNotice(stRsp.recvBytes);
		if (m_pIMWidget != nullptr) {
			m_pIMWidget->SetRoomInfo(roomInfo);
		}
	} break;
	case FUNC_ANCHOR_PERMISSION: {
		STAnchorPermission permission;
		permission = ParseAnchorPermission(stRsp.recvBytes);
		if (permission.success) {
			BeginLive();
		} else {
			MsgBox::information(
				nullptr,
				QString(StringConvert::GetInstance()->GetString(
					STR_STREAM)),
				QString::fromStdString(permission.appMsg));
		}
	} break;
	case FUNC_LIVE_PAUSE:{
		STPause stPause;
		stPause = ParsePause(stRsp.recvBytes);
		if (stPause.success) {
			StreamStateSignal(STREAM_PAUSE);
			AddSecret();
			outputHandler->PauseStream(true);
		} else {
			MsgBox::information(
				nullptr,
				QString(StringConvert::GetInstance()->GetString(
					STR_STREAM)),
				QString::fromStdString(stPause.appMsg));
		}
	}break;
	case FUNC_LIVE_REPAUSE:{
		STRePause stRePause;
		stRePause = ParseRePause(stRsp.recvBytes);
		if (stRePause.success) {
			emit StreamStateSignal(STREAM_REPAUSE);
			outputHandler->PauseStream(false);
			DeleteSecret();
		} else {
			MsgBox::information(
				nullptr,
				QString(StringConvert::GetInstance()->GetString(
					STR_STREAM)),
				QString::fromStdString(stRePause.appMsg));
		}
	}break;
	case FUNC_ROOM_LIST:{
		STRoomList stRoomList;
		stRoomList = ParseRoomList(stRsp.recvBytes);
		if (stRoomList.success) {
			if (m_pUserInfoWidget != nullptr) {
				int id = atoi(m_strRoomId.toStdString().c_str());
				STRoomItem item;
				if (stRoomList.roomList.find(id) != stRoomList.roomList.end()) {
					item = stRoomList.roomList[id];
				}
				if (item.id>0){
					m_pUserInfoWidget->SetTitle(QString::fromStdString(item.title));
					m_pUserInfoWidget->SetId(QString("ID:")+QString::number(item.id));
				}
			}
		} else {
			MsgBox::information(
				nullptr,
				QString(StringConvert::GetInstance()->GetString(
					STR_STREAM)),
				QString::fromStdString(stRoomList.appMsg));
		}
	}break;
	case FUNC_GET_IMTOKEN :{
		STIMToken stImToken;
		stImToken = ParseIMToken(stRsp.recvBytes);
		if(stImToken.success){
			DataConfig::GetInstance()->SetIMTokenInfo(stImToken);
			LoadWebUrl();

			/**
			* 开启迁入心跳
			*/
			StartSignInHearBeat();
		}
	}break;
	case FUNC_HEART_BEAT:{
		STHeartBeat stBase = ParseHeartBeat(stRsp.recvBytes);
		if (stBase.success){
		}
	}break;
	case FUNC_SIGNIN_HEART_BEAT:{
		STHeartBeat stBase = ParseHeartBeat(stRsp.recvBytes);
		if (stBase.success){
		}
	}break;
	case FUNC_DROPOUT:{
		blog(LOG_INFO,"drop out");
	}break;
		default:
			break;
	}
	STBase stBase ;
	ParseBase(stRsp.recvBytes,stBase);
	if (!stBase.success) {
		if (stBase.appCode.compare("S0001")==0){
			ShowInvalidDialog();
		}
	}
}
#if 0
void OBSBasic::OnMessageFrame(const QStompResponseFrame& frame) {
	switch (frame.type()) {
		case QStompResponseFrame::ResponseMessage: {
			QByteArray body = frame.rawBody();
			QByteArray decode = QByteArray::fromBase64(body);
			STMessageBody stBody = ParseMessageBody(decode);
			STCahtTokenRsp stCahtTokenRsp;
			stCahtTokenRsp =
				DataConfig::GetInstance()->GetCahtTokenRsp();
			if (stBody.fromUserId.compare(stCahtTokenRsp.stUserInfo.id)!=0)
				NotifyMessageBody(stBody);
		}break;
		case QStompResponseFrame::ResponseConnected: {
			QByteArray body = frame.rawBody();
			QByteArray decode = QByteArray::fromBase64(body);
			if (m_pIMWidget != nullptr) {
				m_pIMWidget->SetConnected(true);
			}
		}
		default:
			break;
	}
	
	char data[4096] = {0};
	strncpy_s(
		data,
		"eyJjb250ZW50Ijoie1wiZ2lmdElkXCI6NjUyOCxcImdpZnROYW1lXCI6XCLoh63puKHom4tcIixcImdpZnROdW1cIjoxLFwiZ2lmdEVmZmVjdHNcIjpcImh0dHA6Ly9waWMua2VlcGZ1bi5jbi91cGxvYWQvL2ZpbGUvMjAyMC8xMS8xMS8xNjA1MDYxMTA2MTEzNDAxLnN2Z2FcIixcInVzZXJOYW1lXCI6XCLpmLTlpKnmnInlsI/pm6hcIixcImdpZnRVcmxcIjpcImh0dHA6Ly9waWMua2VlcGZ1bi5jbi91cGxvYWQvL2ltYWdlcy8yMDIwLzExLzExLzE2MDUwNjEwOTY3OTk4MjEucG5nXCJ9IiwiY29udGVudFR5cGUiOjIsImNyZWF0ZVRpbWUiOjE2MDUwNzg0MjI2ODYsImZyb21Vc2VyQXZhdGFyIjoiaHR0cDovL3BpYy5rZWVwZnVuLmNuL3VwbG9hZC8vaW1hZ2UvYXZhdGFyLzIwMjAvMDgvMDMvMTU5NjQ0MTI1ODYxMjU4My5qcGVnIiwiZnJvbVVzZXJJZCI6Ijc2IiwiZnJvbVVzZXJOYW1lIjoi6Zi05aSp5pyJ5bCP6ZuoIiwiZnJvbVVzZXJUeXBlIjoxLCJncm91cElkIjoiNTYzMiIsInR5cGUiOjN9",
		4096);
	QByteArray byteData;
	byteData.resize(strlen(data));
	byteData.setRawData(data, strlen(data));
	QByteArray decode2 = QByteArray::fromBase64(byteData);
	STMessageBody stBody = ParseMessageBody(decode2);
	NotifyMessageBody(stBody);
	stBody.contentType = 3;
	stBody.stRedPacket.disType = stBody.stRedPacket.id =
		stBody.stRedPacket.redType = stBody.stRedPacket.totalAmount =
			stBody.stRedPacket.userId = 1;
	NotifyMessageBody(stBody);
	
}
#endif
void OBSBasic::NotifyMessageBody(const STMessageBody &stbody) {
	if (m_pIMWidget != nullptr) {
		m_pIMWidget->InsertMessageBody(stbody);
	}
}

void OBSBasic::SelectLeftWidgetItem(OBSSceneItem item, bool select) {
	if (m_pLeftWidget)
		m_pLeftWidget->SelectItem(item, select);
}

void OBSBasic::CheckAnchorPermission() {
	STReqData req;
	req.func = FUNC_ANCHOR_PERMISSION;
	req.method = GET;
	req.list.append(qMakePair(QByteArray("Content-Type"),
				  QByteArray("application/json")));
	req.list.append(qMakePair(QByteArray("appCode"),
				  QByteArray("keepfun_sportlive")));
	req.list.append(qMakePair(
		QByteArray("token"),
		QByteArray(DataConfig::GetInstance()->GetLoginToken().c_str())));
	sprintf(req.url, "%s%s", BASE_URL.c_str(), "/app/signal/v1/anchorPermission");
	blog(LOG_INFO, "request %s", req.url);
	AsynReqManager::GetInstance()->InsertReq(req);
}


void OBSBasic::InitUrlTab() {
	m_pUrlTabWidget = new UrlTabWidget(ui->leftWidget);
	ui->verticalLayout_10->addWidget(m_pUrlTabWidget);
}

void OBSBasic::InitUrlWebView() {
	if (m_pStackedLayout==nullptr){
		m_pStackedLayout = new QStackedLayout(this);
		ui->bottomLayout->removeWidget(ui->rightwidget);
		m_pStackedLayout->insertWidget(TYPE_TAB_LIVE, ui->rightwidget);
		/**
		* 房间列表
		*/
		m_pRoomListWebWidget = new WebBrowserWidget(this);
		connect(m_pRoomListWebWidget,&WebBrowserWidget::enterRoomSignal,this,&OBSBasic::OnEnterRoolSlot);
		m_pStackedLayout->insertWidget(TYPE_TAB_ROOM_LIST,m_pRoomListWebWidget);
		/**
		* 交易记录
		*/
		m_pExchangeWebWidget=new WebBrowserWidget(this);
		char data2[2048] = {0};
		sprintf(data2,USER_EXCHANGE_HTML.c_str(),DataConfig::GetInstance()->GetLoginToken().c_str());
		QString strData2 = QString(data2);
		m_pExchangeWebWidget->Load(data2);
		blog(LOG_INFO,"%s",data2);
		m_pStackedLayout->insertWidget(TYPE_TAB_EXCHANGE,m_pExchangeWebWidget);
		
		/**
		* 红包额度记录
		*/
		m_pRedPacketWebWidget=new WebBrowserWidget(this);
		m_pStackedLayout->insertWidget(TYPE_TAB_RED_PACKET,m_pRedPacketWebWidget);
		char data3[2048] = {0};
		sprintf(data3,REDPACKET_EDU_HTML.c_str(),DataConfig::GetInstance()->GetLoginToken().c_str());
		QString strData3 = QString(data3);
		m_pRedPacketWebWidget->Load(data3);
		blog(LOG_INFO,"%s",data3);
		/**
		* 直播数据统计
		*/
		m_pWebData=new WebBrowserWidget(this);
		connect(m_pWebData,&WebBrowserWidget::liveDetailSignal,this,&OBSBasic::OnLiveDetailSlot);
		m_pStackedLayout->insertWidget(TYPE_TAB_DATA_STATIC,m_pWebData);
		char data33[2048] = {0};
#if USE_LOCAL_URL
		std::string strUrl = std::string("file:///") + QDir::currentPath().toStdString() + "/anchor/liveData.html?token=%s";
		sprintf(data33,strUrl.c_str(),DataConfig::GetInstance()->GetLoginToken().c_str());
#else
		sprintf(data33,DATA_STATICS_HTML.c_str(),DataConfig::GetInstance()->GetLoginToken().c_str());
#endif
		QString strData33 = QString(data33);
		m_pWebData->Load(strData33);
		blog(LOG_INFO,"%s",data33);
		/**
		* 直播明细
		*/
		m_pWebDetail=new WebBrowserWidget(this);
		connect(m_pWebData, &WebBrowserWidget::queryAnchor, this,
			&OBSBasic::OnQueryAnchorSlot);
		
		m_pStackedLayout->insertWidget(TYPE_TAB_LIVE_DETAIL,m_pWebDetail);
		char data_know2[2048] = {0};
#if USE_LOCAL_URL
		std::string strUrl2 = std::string("file:///") + QDir::currentPath().toStdString() + "/anchor/liveDetail.html?token=%s";
		sprintf(data_know2,strUrl2.c_str(),DataConfig::GetInstance()->GetLoginToken().c_str());
#else
		sprintf(data_know2,LIVE_DETAIL_HTML.c_str(),DataConfig::GetInstance()->GetLoginToken().c_str());
#endif
		blog(LOG_INFO,"%s",data_know2);
		m_pWebDetail->Load(QString(data_know2));
		/**
		* 直播评价
		*/
		m_pWebEvaluate = new WebBrowserWidget(this);
		m_pStackedLayout->insertWidget(TYPE_EVALUTE, m_pWebEvaluate);
		char evaluate_data[2048] = {0};
		sprintf(evaluate_data, LIVE_EVALYATION_HTML.c_str(),
			DataConfig::GetInstance()->GetLoginToken().c_str());
		m_pWebEvaluate->Load(QString(evaluate_data));
		blog(LOG_INFO, "%s", evaluate_data);
		/**
		* 知识库
		*/
		m_pKnowledgeWebWidget=new WebBrowserWidget(this);
		m_pStackedLayout->insertWidget(TYPE_TAB_KNOWLEDGE,m_pKnowledgeWebWidget);
		char data_know[2048] = {0};
		sprintf(data_know,KNOWLEDGE_HTML.c_str(),DataConfig::GetInstance()->GetLoginToken().c_str());
		m_pKnowledgeWebWidget->Load(QString(data_know));
		blog(LOG_INFO,"%s",data_know);
		/**
		* 快捷回复
		*/
		m_pQuickReplyWebWidget=new WebBrowserWidget(this);
		char data_quick[2048] = {0};
		sprintf(data_quick,QUICK_REPLY_HTML.c_str(),DataConfig::GetInstance()->GetLoginToken().c_str());
		m_pStackedLayout->insertWidget(TYPE_TAB_QUICK_REPLY,m_pQuickReplyWebWidget);
		m_pQuickReplyWebWidget->Load(QString(data_quick));
		blog(LOG_INFO,"%s",data_quick);

		ui->bottomLayout->addLayout(m_pStackedLayout);
#if 0
		m_pStackedLayout->setCurrentIndex(TYPE_TAB_LIVE);
#else
		m_pStackedLayout->setCurrentIndex(TYPE_TAB_ROOM_LIST);
#endif
		if (m_pUrlTabWidget != nullptr) {
			connect(m_pUrlTabWidget,SIGNAL(UrlTabSignal(const TabType&)),this,SLOT(UrlTabSlot(const TabType& )));
			connect(m_pUrlTabWidget,SIGNAL(forBack(const TabType&)),this,SLOT(ForBackSlot(const TabType& )));
		}
		m_pUrlTabWidget->SetMenuList(DataConfig::GetInstance()->GetMenus());
	}

	if (m_pChatWebWidget == nullptr) {
		ui->chatwidget->setFixedWidth(282);
		m_pChatWebWidget = new WebBrowserWidget(ui->chatwidget);
#if TEST_IM_ENABLED
		m_pStackedLayout->insertWidget(8,m_pChatWebWidget);
#else
		ui->verticalLayout_11->addWidget(m_pChatWebWidget);
#endif
		
		connect(m_pChatWebWidget,&WebBrowserWidget::closeRoom,this,&OBSBasic::OnCloseRoom);
	}
}

void OBSBasic::ForBackSlot(const TabType& type) {
	switch (type) {
		case TYPE_TAB_KNOWLEDGE: {
			m_pKnowledgeWebWidget->ForBack();
		}break;
	}
}

void OBSBasic::LoadWebUrl() {
	if (m_pRoomListWebWidget != nullptr) {
		char data[4096] = {0};
#if USE_LOCAL_URL
		std::string strUrl = std::string("file:///") + QDir::currentPath().toStdString() + "/anchor/myLiveroom.html?token=%s&imToken=%s";
		sprintf(data,strUrl.c_str(),DataConfig::GetInstance()->GetLoginToken().c_str(),
			DataConfig::GetInstance()->GetImToken().c_str());
#else
		sprintf(data,ROOMLIST_HTML.c_str(),
			DataConfig::GetInstance()->GetLoginToken().c_str(),
			DataConfig::GetInstance()->GetImToken().c_str());
#endif
		blog(LOG_INFO,"%s",data);
		m_pRoomListWebWidget->Load(QString(data));
	}
	if (m_pChatWebWidget!=nullptr) {
		char data[2048] = {0};
		sprintf(data,IMCHAT_HTML.c_str(),DataConfig::GetInstance()->GetLoginToken().c_str(),
			DataConfig::GetInstance()->GetImToken().c_str(),
			DataConfig::GetInstance()->GetSysUserId(),
			DataConfig::GetInstance()->GetImId().c_str());
		QString strData = QString(data);
		m_pChatWebWidget->Load(data);
		blog(LOG_INFO,"%s",data);
	}
}


void OBSBasic::PauseLive() {
	STReqData req;
	req.func = FUNC_LIVE_PAUSE;
	req.method = GET;
	req.list.append(qMakePair(QByteArray("Accept"),
				  QByteArray("application/json")));
	req.list.append(qMakePair(QByteArray("platform"),
				  QByteArray("3")));
	req.list.append(qMakePair(
		QByteArray("token"),
		QByteArray(DataConfig::GetInstance()->GetLoginToken().c_str())));

	STBeginRsp stBeginRsp;
	stBeginRsp = DataConfig::GetInstance()->GetBeginRsp();
	sprintf(req.url, "%s/shop/live/info/pause/%d", BASE_URL.c_str(),
		stBeginRsp.liveId);
	blog(LOG_INFO, "request %s", req.url);
	AsynReqManager::GetInstance()->InsertReq(req);

	
}

void OBSBasic::RePauseLive() {
	STReqData req;
	req.func = FUNC_LIVE_REPAUSE;
	req.method = GET;
	req.list.append(qMakePair(QByteArray("Accept"),
				  QByteArray("application/json")));
	req.list.append(qMakePair(QByteArray("platform"),
				  QByteArray("3")));
	req.list.append(qMakePair(
		QByteArray("token"),
		QByteArray(DataConfig::GetInstance()->GetLoginToken().c_str())));

	STBeginRsp stBeginRsp;
	stBeginRsp = DataConfig::GetInstance()->GetBeginRsp();
	sprintf(req.url, "%s/shop/live/info/rePause/%d", BASE_URL.c_str(),
		stBeginRsp.liveId);
	blog(LOG_INFO, "request %s", req.url);
	AsynReqManager::GetInstance()->InsertReq(req);
}

void OBSBasic::ShowInvalidDialog() {
	if (m_pInvalidDlg == nullptr) {
		m_pInvalidDlg = new MsgBox(nullptr,StringConvert::GetInstance()->GetString(STR_PROCESS)
		,StringConvert::GetInstance()->GetString(STR_PROCESS_LOGIN_EXPIRED),EOK_CANCLE);
		auto CloseSlot = [&](){
			int btn = m_pInvalidDlg->GetMsgValue();
			if (btn==EOK){
				emit m_pTopWidget->ExitWindow();
				restart=true;
			}
			m_pInvalidDlg->deleteLater();
			m_pInvalidDlg = nullptr;
		};
		connect(m_pInvalidDlg,&MsgBox::sigClose,CloseSlot);
	}
	if (m_pInvalidDlg != nullptr){
		m_pInvalidDlg->setModal(true);
		m_pInvalidDlg->raise();
		m_pInvalidDlg->show();
	}
}

void OBSBasic::UrlTabSlot(const TabType& type) {
	if (type >= TYPE_TAB_LIVE && type <= TYPE_EVALUTE) {
		if (type == TYPE_TAB_LIVE) {
			if (!m_bEnterRoom) {
				m_pStackedLayout->setCurrentIndex(TYPE_TAB_ROOM_LIST);
			}
			else {
				m_pStackedLayout->setCurrentIndex(TYPE_TAB_LIVE);
				ui->preview->move(0,0);
			}
		} else {
			m_pStackedLayout->setCurrentIndex(type);
		}
#if 1
		WebBrowserWidget* widget = dynamic_cast<WebBrowserWidget*>(m_pStackedLayout->currentWidget());
		if (widget != nullptr) {
			//widget->Reload();
		}
#endif
		/*if (type == TYPE_TAB_LIVE) {
			SetCameraEnabled(true);
		} else {
			SetCameraEnabled(false);
		}*/
	}
}

void OBSBasic::OnEnterRoolSlot(const QString& roomid,const QString & liveId) {
	try {
		
	} catch (std::exception e) {
		OutputDebugStringA(e.what());
	}
	
	if (!roomid.isEmpty()) {
		SetRoomState(true);
		m_strRoomId = roomid;
		m_strLiveId = liveId;
		DataConfig::GetInstance()->SetRoomId(atoi(m_strRoomId.toStdString().c_str()));
		if(m_pUserInfoWidget)
			m_pUserInfoWidget->ResetLiveState();

		GetRoomList();
	}
}

void OBSBasic::OnLiveDetailSlot(const QString &anchorName, const QString &openTimeStart,
		      const QString &openTimeEnd)
{
	if (m_pStackedLayout != nullptr && m_pWebDetail!=nullptr && m_pUrlTabWidget!=nullptr ) {
		QString strScript = QString("initQueryPage('%1','%2','%3');");
		strScript = strScript.arg(anchorName).arg(openTimeStart).arg(openTimeEnd);
		m_pWebDetail->CallJavaScript(strScript);
		m_pUrlTabWidget->SetItemWidgetSelected(TYPE_TAB_LIVE_DETAIL);
		m_pStackedLayout->setCurrentWidget(m_pWebDetail);
	}
}

void OBSBasic::OnQueryAnchorSlot(const QString &strQueryId) {
	if (m_pStackedLayout != nullptr && m_pWebEvaluate != nullptr &&
	    m_pUrlTabWidget != nullptr) {
		QString strScript = QString("QueryAnchor('%1');");
		strScript = strScript.arg(strQueryId);
		m_pWebEvaluate->CallJavaScript(strScript);
		m_pUrlTabWidget->SetItemWidgetSelected(TYPE_EVALUTE);
		m_pStackedLayout->setCurrentWidget(m_pWebEvaluate);
	}
}

void OBSBasic::SetRoomState(bool enter) {
	m_bEnterRoom=enter;
	CleanSource();
	if (m_bEnterRoom) {
		m_pStackedLayout->setCurrentIndex(TYPE_TAB_LIVE);
		AddCamera();
		AddCloseSource();
		DeleteSecret();
	}
	else {
		DataConfig::GetInstance()->SetRoomId(0);
		m_pStackedLayout->setCurrentIndex(TYPE_TAB_ROOM_LIST);
		QString script = QString("refreshPage();");
		m_pRoomListWebWidget->CallJavaScript(script);
	}
}

void OBSBasic::GetRoomList() {
	STReqData req;
	req.func = FUNC_ROOM_LIST;
	req.method = GET;
	req.list.append(qMakePair(QByteArray("Accept"),
				  QByteArray("application/json")));
	std::string token = DataConfig::GetInstance()->GetLoginToken();
	req.list.append(qMakePair(QByteArray("token"),
				  QByteArray(token.c_str())));
	int pageIndex = 1;
	int pageSize = 50;
	sprintf(req.url,
		"%s/shop/live/info/list?page=%d&size=%d",
		BASE_URL.c_str(), pageIndex, pageSize);
	blog(LOG_INFO, "request %s", req.url);
	AsynReqManager::GetInstance()->InsertReq(req);
}

static void  remove_leftwidget_item(obs_sceneitem_t *item) {
	OBSBasic *main = reinterpret_cast<OBSBasic *>(App()->GetMainWindow());
	if (main) {
		main->RemoveLeftWidgetItem(item);
	}
}

void OBSBasic::CleanSource() {
	QVector<OBSSceneItem> items;
	items.clear();
	OBSScene scene = GetCurrentScene();
	obs_scene_enum_items(scene, EnumItem, &items);

	for (auto item : items) {
		if (scene) {
			obs_source_t *source = obs_sceneitem_get_source(item);
			if (source) {
				obs_sceneitem_remove_left_widget(item, remove_leftwidget_item);
			}
		}
	}
	CustomConfig::SetMaskPath("");
	CustomConfig::SetBackgroundPath("");
}

void OBSBasic::GetIMToken() {
	STReqData req;
	req.func = FUNC_GET_IMTOKEN;
	req.method = POST;
	req.list.append(qMakePair(QByteArray("Accept"),
				  QByteArray("application/json")));
	std::string token = DataConfig::GetInstance()->GetLoginToken();
	req.list.append(qMakePair(QByteArray("platform"), QByteArray("5")));
	req.list.append(qMakePair(QByteArray("token"),
				  QByteArray(token.c_str())));
	sprintf(req.url,
		"%s/shop/customer/online/service/im/login?device=1&workStatus=1",
		BASE_URL.c_str());
	blog(LOG_INFO, "request %s", req.url);
	AsynReqManager::GetInstance()->InsertReq(req);
}

void OBSBasic::DropOut() {
	STReqData req;
	req.func = FUNC_DROPOUT;
	req.method = POST;
	req.list.append(qMakePair(QByteArray("Accept"),
				  QByteArray("application/json")));
	
	std::string token = DataConfig::GetInstance()->GetLoginToken();
	req.list.append(qMakePair(QByteArray("platform"), QByteArray("5")));
	req.list.append(qMakePair(QByteArray("token"),
				  QByteArray(token.c_str())));
	std::string strImToken = DataConfig::GetInstance()->GetImToken();
	req.list.append(qMakePair(QByteArray("imToken"),
				  QByteArray(strImToken.c_str())));
	sprintf(req.url,
		"%s/shop/customer/online/service/im/dropOut?device=1&workStatus=0",
		BASE_URL.c_str(),strImToken.c_str());
	blog(LOG_INFO, "request %s", req.url);
	AsynReqManager::GetInstance()->InsertReq(req);
}

void OBSBasic::OnCloseRoom() {
	/*if (outputHandler->StreamingActive()&&m_bStartStream) {
		FinishLive();
	}*/
	/**
	* 仅在当前情况下不调用finishlive直接停止推流
	*/
	StopStreaming();
	CleanSource();
	QString strScript = QString("liveFinish('%1','%2');");
	strScript = strScript.arg(m_strRoomId.toStdString().c_str())
				.arg(m_strLiveId.toStdString().c_str());
	blog(LOG_INFO,"IM_CHAT:%s",strScript.toStdString().c_str());
	if (m_pChatWebWidget)
		m_pChatWebWidget->CallJavaScript(strScript);
}

void OBSBasic::OnUpdateLoginInfo(STCsUser csuser) {
	DataConfig::GetInstance()->SetCsuserInfo(csuser);
}

void OBSBasic::RemoveNonDShowSources() {
	char source_id[32]={0};
	sprintf(source_id, "%s","dshow_input");
	obs_sceneitem_t * item = nullptr;
	item = obs_scene_select_find_source_not_match(GetCurrentScene(),source_id);
	while (item) {
		obs_sceneitem_remove_left_widget(item,remove_leftwidget_item);
		item = obs_scene_select_find_source_not_match(GetCurrentScene(),source_id);
	}
}

void OBSBasic::OnAddFontTxt(const STTxtSet& set) {
	if (set.text.isEmpty())
		return;
	const char* name;
	obs_source_t *source = nullptr;
	const char *id = "text_gdiplus";
	OBSBasic *main = reinterpret_cast<OBSBasic *>(App()->GetMainWindow());
	OBSScene scene = main->GetCurrentScene();
	bool success = false;
	if (!scene)
		return ;
	if (strlen(set.name)==0) {
		QString placeHolderText{QT_UTF8(common::GetSourceDisplayNameFromId(id))};
		QString text{placeHolderText};
		int i = 2;
		while ((source = obs_get_source_by_name(QT_TO_UTF8(text))))
		{
			obs_source_release(source);
			text = QString("%1 %2").arg(placeHolderText).arg(i++);
		}
		;
		const char *v_id = obs_get_latest_input_type_id(id);
		source = obs_source_create(v_id, QT_TO_UTF8(text), NULL, nullptr);
		AddSourceData data;
		data.source = source;
		data.visible = true;

		obs_enter_graphics();
		obs_scene_atomic_update(scene, AddSource2, &data);
		obs_leave_graphics();

		/* set monitoring if source monitors by default */
		uint32_t flags = obs_source_get_output_flags(source);
		if ((flags & OBS_SOURCE_MONITOR_BY_DEFAULT) != 0) {
			obs_source_set_monitoring_type(
				source,
				OBS_MONITORING_TYPE_MONITOR_ONLY);
		}
	} else {
		name = set.name;
		source = obs_get_source_by_name(set.name);
	}
	if (source) {
		OBSData settings = obs_source_get_settings(source);
		obs_data_set_string(settings,"text",set.text.toStdString().c_str());
			
		if (!set.bg_color.isEmpty()) {
			long long color = common::color_to_int(QColor(set.bg_color));
			obs_data_set_int(settings, "bk_color", color);
			obs_data_set_int(settings, "bk_opacity", 50);
		}
		if (!set.txt_color.isEmpty()) {
			long long color = common::color_to_int(QColor(set.txt_color));
			obs_data_set_int(settings, "color", color);
		}
		OBSData font_setting = obs_data_create();
		if (!set.family.isEmpty()) {
			obs_data_set_string(font_setting, "face", set.family.toStdString().c_str());
		}
		if (set.bold && set.italic) {
			obs_data_set_string(font_setting, "style", "Bold Italic");
		} else if (set.bold) {
			obs_data_set_string(font_setting, "style", "Bold");
		} else if (set.italic) {
			obs_data_set_string(font_setting, "style", "Italic");
		} else {
			obs_data_set_string(font_setting, "style", "Regular");
		}
		int fz = atoi(set.font_size.toStdString().c_str());
		obs_data_set_int(font_setting, "size", fz);
		int font_flags = set.bold ? OBS_FONT_BOLD : 0;
		font_flags |= set.italic ? OBS_FONT_ITALIC : 0;
		font_flags |= set.underline ? OBS_FONT_UNDERLINE : 0;
		obs_data_set_int(font_setting, "flags", font_flags);
		obs_data_set_obj(settings,"font",font_setting);
		obs_source_update(source,settings);
		obs_data_release(font_setting);
		obs_data_release(settings);	
	}
	obs_source_release(source);

	if (strlen(set.name) !=0 ) {
		Sleep(100);
		obs_sceneitem_t *item = obs_scene_find_source(scene, set.name);
		MoveItem(item);
	}
}

static inline const char *GetSourceDisplayName(const char *id)
{
	if (strcmp(id, "scene") == 0)
		return Str("Basic.Scene");
	const char *v_id = obs_get_latest_input_type_id(id);
	return obs_source_get_display_name(v_id);
}

void OBSBasic::OnFilterChange(int type) {
	blog(LOG_INFO,"filter type:%d",type);
	QString path = common::GetFilterPath((Filter_Type)type);
	DataConfig::GetInstance()->SetFilterType(type);

	QString placeHolderText{QT_UTF8(GetSourceDisplayName("dshow_input"))};
	QString text{placeHolderText};
	int i = 2;
	obs_source_t *source = nullptr;
	while ((source = obs_get_source_by_name(QT_TO_UTF8(text)))) {
		//clut_filter
		std::string clut_filter_name = obs_source_get_display_name("clut_filter");
		obs_source_t *clut_filter_source = obs_source_get_filter_by_name(source, clut_filter_name.c_str());
		if (clut_filter_source) {
			obs_data_t *setting = obs_source_get_settings(clut_filter_source);
			obs_data_set_string(setting, "image_path", QT_TO_UTF8(path));
			obs_source_update(clut_filter_source, setting);
			obs_data_release(setting);
		}
		obs_source_release(clut_filter_source);

		//
		
		obs_source_release(source);
		text = QString("%1 %2").arg(placeHolderText).arg(i++);
	}

	const char* camera_name = config_get_string(App()->GlobalConfig(), "CameraSet", "name");
	source = obs_get_source_by_name(camera_name);
	if (source) {
		std::string clut_filter_name = obs_source_get_display_name("clut_filter");
		obs_source_t *clut_filter_source = obs_source_get_filter_by_name(source, clut_filter_name.c_str());
		if (clut_filter_source) {
			obs_data_t *setting = obs_source_get_settings(clut_filter_source);
			obs_data_set_string(setting, "image_path", QT_TO_UTF8(path));
			obs_source_update(clut_filter_source, setting);
			obs_data_release(setting);
		}
		obs_source_release(clut_filter_source);

		//
		
		obs_source_release(source);
	}
}

void OBSBasic::OnBeautyFaceChange(int type, int value) {
	blog(LOG_INFO,"beauty face:type:%d,value:%d",type,value);
	BeautyFace_Type bType = (BeautyFace_Type)type;
	char *id = "";
	switch (bType) {
		//all shape change functioned in face_lift_filter
	case BeautyFace_SkinWhite:
		id = "skin_white_filter";
		DataConfig::GetInstance()->SetSkinWhiteValue(QT_TO_UTF8(QString::number(value)));
		break;
	case BeautyFace_Moulting:
		id = "moulting_filter";
		DataConfig::GetInstance()->SetMoultingValue(QT_TO_UTF8(QString::number(value)));
		break;
	case BeautyFace_Ruddy:
		id = "ruddy_filter";
		DataConfig::GetInstance()->SetRuddyValue(QT_TO_UTF8(QString::number(value)));
		break;
	case BeautyFace_EyesReshape:
		id = "face_lift_filter";
		DataConfig::GetInstance()->SetEyesReshapeValue(QT_TO_UTF8(QString::number(value)));
		break;
	case BeautyFace_FaceLift:
		id = "face_lift_filter";
		DataConfig::GetInstance()->SetFaceLiftValue(QT_TO_UTF8(QString::number(value)));
		break;
	case BeautyFace_Blusher:
		id = "blusher";
		DataConfig::GetInstance()->SetBlusherValue(QT_TO_UTF8(QString::number(value)));
		break;
	case BeautyFace_ThinChin:
		id = "face_lift_filter";
		DataConfig::GetInstance()->SetThinChinValue(QT_TO_UTF8(QString::number(value)));
		break;
	default:
		break;
	}
	if (strlen(id) > 0) {
		blog(LOG_INFO, "face filter id:%s",id);
	}
	QString placeHolderText{QT_UTF8(GetSourceDisplayName("dshow_input"))};
	QString text{placeHolderText};
	int i = 2;
	obs_source_t *source = nullptr;
	while ((source = obs_get_source_by_name(QT_TO_UTF8(text)))) {
		std::string filter_name = obs_source_get_display_name(id);
		blog(LOG_INFO,"filter name:%s",filter_name.c_str());
		obs_source_t *filter_source = obs_source_get_filter_by_name(source, filter_name.c_str());
		if (filter_source) {
			obs_data_t *setting = obs_source_get_settings(filter_source);
			if (bType == BeautyFace_SkinWhite) {
				obs_data_set_double(setting, "clut_amount", (double)value / 100.0);
			} else if (bType == BeautyFace_Ruddy) {
				obs_data_set_double(setting, "strongly", (double)value / 200.0 + 1);
			} else if (bType == BeautyFace_Blusher) {
				obs_data_set_double(setting, "strongly", (double)value / 200.0);
			} else if (bType == BeautyFace_EyesReshape) {
				obs_data_set_double(setting, "eye_strongly", (double)value / 100.0);
			} else if (bType == BeautyFace_FaceLift) {
				obs_data_set_double(setting, "face_strongly", (double)value / 100.0);
			} else if (bType == BeautyFace_ThinChin) {
				obs_data_set_double(setting, "thin_strongly", (double)value / 100.0);
			} else if (bType == BeautyFace_Moulting) {
				obs_data_set_double(setting, "strongly", (double)value / 100.0);
			}
			obs_source_update(filter_source, setting);
			obs_data_release(setting);
		}
		obs_source_release(filter_source);

		//
		
		obs_source_release(source);
		text = QString("%1 %2").arg(placeHolderText).arg(i++);
	}

	const char* camera_name = config_get_string(App()->GlobalConfig(), "CameraSet", "name");
	source = obs_get_source_by_name(camera_name);
	if (source) {
		std::string filter_name = obs_source_get_display_name(id);
		obs_source_t *filter_source = obs_source_get_filter_by_name(source, filter_name.c_str());
		if (filter_source) {
			blog(LOG_INFO,"camera name:%s,filter name:%s",camera_name,filter_name.c_str());
			obs_data_t *setting = obs_source_get_settings(filter_source);
			if (bType == BeautyFace_SkinWhite) {
				obs_data_set_double(setting, "clut_amount", (double)value / 100.0);
			} else if (bType == BeautyFace_Ruddy) {
				obs_data_set_double(setting, "strongly", (double)value / 200.0 + 1);
			} else if (bType == BeautyFace_Blusher) {
				obs_data_set_double(setting, "strongly", (double)value / 200.0);
			} else if (bType == BeautyFace_EyesReshape) {
				obs_data_set_double(setting, "eye_strongly", (double)value / 100.0);
			} else if (bType == BeautyFace_FaceLift) {
				obs_data_set_double(setting, "face_strongly", (double)value / 100.0);
			} else if (bType == BeautyFace_ThinChin) {
				obs_data_set_double(setting, "thin_strongly", (double)value / 100.0);
			} else if (bType == BeautyFace_Moulting) {
				obs_data_set_double(setting, "strongly", (double)value / 100.0);
			}
			obs_source_update(filter_source, setting);
			obs_data_release(setting);
		}
		obs_source_release(filter_source);

		obs_source_release(source);
	}
}

void OBSBasic::SetCameraEnabled(bool enabled) {
	const char* camera_name = config_get_string(App()->GlobalConfig(), "CameraSet", "name");
	obs_source *source = obs_get_source_by_name(camera_name);
	if (source) {
		bool bEnabled = obs_source_enabled(source);
		if (bEnabled != enabled) {
			obs_source_set_enabled(source, enabled);
			obs_source_release(source);
		}
	}
}
