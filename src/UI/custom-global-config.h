#ifndef CUSTOMGLOBALCONFIG_H
#define CUSTOMGLOBALCONFIG_H
#include "base-define.h"
#include <string>
#include <QString>
class DataConfig {
public:
	~DataConfig();
	SINGLETON_DEFINE(DataConfig);

	void SetMicSoundValue(int value);
	void SetAudioSoundValue(int value);
	std::string ReadRecordPath();
	void SetRecordPath(const std::string &path);
	void SetRecord(bool record);
	bool GetRecord();
	const char *GetSecretName();
	void SetScretName(const char* name);
	const char *GetScreenShotName();
	void SetScreenShotName(const char *name);
	void CheckNameChanged(const char *prev, const char *last);

	// 最大化
	bool GetMaximum();
	void SetMaximum(bool bmax);
	void SetLiveList(const STLiveList& lists);
	STLiveList GetLiveList();
	void SetLoginInfo(const STLoginRsp& stRsp);
	void SetCsuserInfo(const STCsUser &stCsuser);
	STLoginRsp GetLoginInfo();
	std::string GetLoginToken();
	int GetSysUserId();
	std::string GetImId();
	void SetBeginRsp(const STBeginRsp &);
	STBeginRsp GetBeginRsp();
	void SetCahtTokenRsp(const STCahtTokenRsp &);
	STCahtTokenRsp GetCahtTokenRsp();
	std::string GetChatToken();

	// 开机自启动
	void SetAutoStart(bool bAuto);
	bool GetAutoStart();

	// 关闭是退出还是到托盘
	void SetCloseType(bool bTray);
	bool GetCloseType();
	
	const char *GetCameraName();
	void SetCameraName(const char* name);
	void SetRoomList(const STRoomList& list);
	void SetRoomId(int id);
	STRoomItem GetRoomItem();
	void SetIMTokenInfo(const STIMToken & imToken);
	std::string GetImToken();
	const char *GetCloseSourceName();
	void SetCloseSourceName(const char* name);
	void SetMenus(std::vector<STMenuItem>& vecMenus);
	std::vector<STMenuItem> GetMenus() ;

	void SetFilterType(int type);
	int GetFilterType();

	void SetMaskPath(const char *path);
	const char *GetMaskPath();

	void SetMaskType(int type);
	int GetMaskType();

	void SetSkinWhiteValue(const char *value);
	const char *GetSkinWhiteValue();
	
	void SetMoultingValue(const char *value);
	const char *GetMoultingValue();
	
	void SetRuddyValue(const char *value);
	const char *GetRuddyValue();
	
	void SetEyesReshapeValue(const char *value);
	const char *GetEyesReshapeValue();
	
	void SetFaceLiftValue(const char *value);
	const char *GetFaceLiftValue();
	
	void SetBlusherValue(const char *value);
	const char *GetBlusherValue();
	
	void SetThinChinValue(const char *value);
	const char *GetThinChinValue();
private:
	DataConfig();
private:
	int m_nMicSoundValue;
	int m_nAudioSoundValue;
	std::string m_strRecordPath;
	bool m_bRecord;
	STLiveList m_listLive;
	STLoginRsp m_stLoginRsp;
	STBeginRsp m_stBeginRsp;
	STCahtTokenRsp m_stCahtTokenRsp;
	int m_nRoomId;
	STRoomList m_mapRoom;
	STIMToken m_stImToken;
	std::vector<STMenuItem> m_vecMenus;
};

#endif // !CUSTOMGLOBALCONFIG_H
