#include "custom-global-config.h"
#include "obs-app.hpp"
#include <util/config-file.h>

#include <QSettings>

DataConfig::DataConfig()
{
	m_nMicSoundValue	= 0;
	m_nAudioSoundValue	= 0;
	m_nRoomId = 0;
}

DataConfig::~DataConfig()
{
}

void DataConfig::SetMicSoundValue(int value)
{
	m_nMicSoundValue = value;
}

void DataConfig::SetAudioSoundValue(int value)
{
	m_nAudioSoundValue = value;
}

std::string DataConfig::ReadRecordPath() {
	const char* path = config_get_string(App()->GlobalConfig(),
		"RecordSet", "RecordPath");
	if (path == nullptr) {
		return std::string("");
	}
	m_strRecordPath.assign(path);
	return m_strRecordPath;
}

void DataConfig::SetRecordPath(const std::string &path) {
	m_strRecordPath = path;
}

void DataConfig::SetRecord(bool record)
{
	m_bRecord = record;
}

bool DataConfig::GetRecord() {
	return m_bRecord;
}


const char* DataConfig::GetSecretName() {
	return config_get_string(App()->GlobalConfig(), "SecretSet",
					     "name");
}

void DataConfig::SetScretName(const char* name) {
	config_set_string(App()->GlobalConfig(), "SecretSet", "name", name);
}

const char *DataConfig::GetScreenShotName()
{
	return config_get_string(App()->GlobalConfig(), "ScreenShot", "name");
}

void DataConfig::SetScreenShotName(const char *name) {
	config_set_string(App()->GlobalConfig(), "ScreenShot", "name", name);
}

void DataConfig::CheckNameChanged(const char *prev, const char *last) {
	const char* cur = config_get_string(App()->GlobalConfig(), "SecretSet", "name");
	if (strcmp(prev, last) != 0 && strcmp(prev,cur)==0 ) {
		config_set_string(App()->GlobalConfig(),
			"SecretSet", "name",last);
	}
}

bool DataConfig::GetMaximum() {
	return config_get_bool(App()->GlobalConfig(), "BasicWindow", "Maximum");
}

void DataConfig::SetMaximum(bool bmax) {
	config_set_bool(App()->GlobalConfig(), "BasicWindow", "Maximum", bmax);
}

void DataConfig::SetLiveList(const STLiveList &lists)
{
	m_listLive = lists;
}

STLiveList DataConfig::GetLiveList()
{
	return m_listLive;
}

void DataConfig::SetLoginInfo(const STLoginRsp &stRsp)
{
	m_stLoginRsp = stRsp;

}

void DataConfig::SetCsuserInfo(const STCsUser &stCsuser) {
	m_stLoginRsp.stService.stUser.stCsUser = stCsuser;
}

STLoginRsp DataConfig::GetLoginInfo()
{
	return m_stLoginRsp;
}

std::string DataConfig::GetLoginToken() {
	return m_stLoginRsp.stService.token;
}

int DataConfig::GetSysUserId() {
	return m_stLoginRsp.stService.stUser.stCsUser.sysUserId;
}

std::string DataConfig::GetImId() {
	return m_stImToken.stUserInfo.id;
}

void DataConfig::SetBeginRsp(const STBeginRsp &rsp)
{
	m_stBeginRsp = rsp;
}

STBeginRsp DataConfig::GetBeginRsp() {
	return m_stBeginRsp;
}

void DataConfig::SetCahtTokenRsp(const STCahtTokenRsp &rsp)
{
	m_stCahtTokenRsp = rsp;
}

STCahtTokenRsp DataConfig::GetCahtTokenRsp() {
	return m_stCahtTokenRsp;
}

void DataConfig::SetAutoStart(bool bAuto) {
#if _WINDOWS
	QString appName = QApplication::applicationName(); //程序名称
	QString appPath = QApplication::applicationFilePath(); // 程序路径
	appPath = appPath.replace("/", "\\");

	QSettings reg("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
	QString val = reg.value(appName).toString();
	if (bAuto) {
		if (val != appPath)
			reg.setValue(appName, appPath);
	} else {
		reg.remove(appName);
	}
#endif // _WINDOWS	
}

bool DataConfig::GetAutoStart() {
#if _WINDOWS
	QString appName = QApplication::applicationName();     //程序名称
	QString appPath = QApplication::applicationFilePath(); // 程序路径
	appPath = appPath.replace("/", "\\");

	QSettings reg("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
	QString val = reg.value(appName).toString();
	if (val == appPath)
		return true;
	else
		return false;
#else
	return false;
#endif // _WINDOWS
}

void DataConfig::SetCloseType(bool bTray) {
	config_set_bool(App()->GlobalConfig(), "BasicWindow", "SysCloseToTray", bTray);
}

bool DataConfig::GetCloseType()
{
	return config_get_bool(App()->GlobalConfig(), "BasicWindow", "SysCloseToTray");
}

const char *DataConfig::GetCameraName()
{
	return config_get_string(App()->GlobalConfig(), "CameraSet", "name");
}

void DataConfig::SetCameraName(const char *name)
{
	config_set_string(App()->GlobalConfig(), "CameraSet", "name", name);
}

std::string DataConfig::GetChatToken() 
{
	return m_stCahtTokenRsp.token;
}

void  DataConfig::SetRoomList(const STRoomList& list) {
	m_mapRoom.roomList.clear();
	m_mapRoom = list;
}

void DataConfig::SetRoomId(int id) {
	m_nRoomId = id;
}

STRoomItem DataConfig::GetRoomItem() {
	STRoomItem stRoomItem;
	if (m_mapRoom.roomList.find(m_nRoomId) != m_mapRoom.roomList.end()) {
		stRoomItem = m_mapRoom.roomList[m_nRoomId];
	}
	return stRoomItem;
}

void DataConfig::SetIMTokenInfo(const STIMToken& imToken) {
	m_stImToken = imToken;
}

std::string DataConfig::GetImToken() {
	return m_stImToken.token;
}

const char *DataConfig::GetCloseSourceName()
{
	return config_get_string(App()->GlobalConfig(), "CloseSource", "name");
}

void DataConfig::SetCloseSourceName(const char *name) {
	config_set_string(App()->GlobalConfig(), "CloseSource", "name", name);
}

void DataConfig::SetMenus(std::vector<STMenuItem>& vecMenus) {
	m_vecMenus = vecMenus;
}

std::vector<STMenuItem> DataConfig::GetMenus() {
	return m_vecMenus;
}

void DataConfig::SetFilterType(int type) {
	config_set_int(App()->GlobalConfig(), "Filter", "Type", type);
}

int DataConfig::GetFilterType()
{
	return config_get_int(App()->GlobalConfig(), "Filter", "Type");
}

void DataConfig::SetMaskPath(const char *path) {
	config_set_string(App()->GlobalConfig(), "Mask", "path", path);
}

const char *DataConfig::GetMaskPath()
{
	return config_get_string(App()->GlobalConfig(), "Mask", "path");
}

void DataConfig::SetMaskType(int type) {
	config_set_int(App()->GlobalConfig(), "Mask", "Type", type);
}

int DataConfig::GetMaskType()
{
	return config_get_int(App()->GlobalConfig(), "Mask", "Type");
}

void DataConfig::SetSkinWhiteValue(const char *value) {
	config_set_string(App()->GlobalConfig(), "Filter", "SkinWhite", value);
}

const char *DataConfig::GetSkinWhiteValue()
{
	return config_get_string(App()->GlobalConfig(), "Filter", "SkinWhite");
}

void DataConfig::SetMoultingValue(const char *value) {
	config_set_string(App()->GlobalConfig(), "Filter", "Moulting", value);
}

const char *DataConfig::GetMoultingValue()
{
	return config_get_string(App()->GlobalConfig(), "Filter", "Moulting");
}

void DataConfig::SetRuddyValue(const char *value) {
	config_set_string(App()->GlobalConfig(), "Filter", "Ruddy", value);
}

const char *DataConfig::GetRuddyValue()
{
	return config_get_string(App()->GlobalConfig(), "Filter", "Ruddy");
}

void DataConfig::SetEyesReshapeValue(const char *value) {
	config_set_string(App()->GlobalConfig(), "Filter", "EyesReshape", value);
}

const char *DataConfig::GetEyesReshapeValue()
{
	return config_get_string(App()->GlobalConfig(), "Filter", "EyesReshape");
}

void DataConfig::SetFaceLiftValue(const char *value) {
	config_set_string(App()->GlobalConfig(), "Filter", "FaceLift", value);
}

const char *DataConfig::GetFaceLiftValue()
{
	return config_get_string(App()->GlobalConfig(), "Filter", "FaceLift");
}

void DataConfig::SetBlusherValue(const char *value) {
	config_set_string(App()->GlobalConfig(), "Filter", "Blusher", value);
}

const char *DataConfig::GetBlusherValue()
{
	return config_get_string(App()->GlobalConfig(), "Filter", "Blusher");
}

void DataConfig::SetThinChinValue(const char *value) {
	config_set_string(App()->GlobalConfig(), "Filter", "ThinChin", value);
}

const char *DataConfig::GetThinChinValue()
{
	return config_get_string(App()->GlobalConfig(), "Filter", "ThinChin");
}
