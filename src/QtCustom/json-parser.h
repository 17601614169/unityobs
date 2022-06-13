#ifndef JSONPARSER_H
#define JSONPARSER_H
#include <QString>
#include <QRect>
#include "qtcustom_global.h"
#include "base-define.h"
/*
*积累STBase解析
*/
QTCUSTOM_EXPORT void ParseBase(const QByteArray &json,STBase & base);

/**
@brief 解析json,登录
@param QByteArray json串
@return STLoginRsp 
*/
QTCUSTOM_EXPORT STLoginRsp ParseLogin(const QByteArray &json);

/**
@brief 直播列表
@param QByteArray json串
@return STLiveList 
*/
QTCUSTOM_EXPORT STLiveList ParserLiveList(const QByteArray &json);

/**
@brief 开始直播
@param QByteArray json串
@return STBeginRsp 
*/
QTCUSTOM_EXPORT STBeginRsp ParseBegin(const QByteArray &json);

/**
@brief 获取聊天室token
@param QByteArray json串
@return STCahtTokenRsp 
*/
QTCUSTOM_EXPORT STCahtTokenRsp ParseGetToken(const QByteArray &json);

/**
@brief 获取粉丝列表
@param QByteArray json串
@return int 
*/
QTCUSTOM_EXPORT int ParseFansList(const QByteArray &json);

/**
@brief 获取用户账户信息
@param QByteArray json串
@return STCompteInfo 
*/
QTCUSTOM_EXPORT STCompteInfo ParseAccount(const QByteArray &json);

/**
@brief 结束直播
@param QByteArray json串
@return STFinishRsp 
*/
QTCUSTOM_EXPORT STFinishRsp ParseFinish(const QByteArray &json);

/**
@brief 拉入黑名单
@param QByteArray json串
@return STDoBacklistRsp 
*/
QTCUSTOM_EXPORT STDoBacklistRsp ParseDoBacklist(const QByteArray &json);

/**
@brief 拉入黑名单
@param QByteArray json串
@return STGiftList 
*/
QTCUSTOM_EXPORT STGiftList ParseGiftList(const QByteArray &json);

/**
@brief 拉入黑名单
@param QByteArray json串
@return STGiftList 
*/
QTCUSTOM_EXPORT STMessageBody ParseMessageBody(const QByteArray &json);

/**
@brief 送礼消息content
@param QByteArray json串
@return STSendGift 
*/
QTCUSTOM_EXPORT STSendGift ParseSendGift(const QByteArray &json);

/**
@brief 红包消息content
@param QByteArray json串
@return STSendGift 
*/
QTCUSTOM_EXPORT STRedPacket ParseRedPacket(const QByteArray &json);

/**
@brief 公告
@param QByteArray json串
@return STNoticeRsp 
*/
QTCUSTOM_EXPORT STNoticeRsp ParseNotice(const QByteArray &json);

/**
@brief 用户基本信息
@param QByteArray json串
@return STUserBasicInfo 
*/
QTCUSTOM_EXPORT STUserBasicInfo ParseUserBasicInfo(const QByteArray &json);

/**
@brief 直播间标题
@param QByteArray json串
@return string 
*/
QTCUSTOM_EXPORT std::string ParseLiveTitle(const QByteArray &json);

/**
@brief 直播间标题
@param QByteArray json串
@return string 
*/
QTCUSTOM_EXPORT void ParseScreenShot(const QByteArray &json, QRect &rect);

/**
@brief 直播权限
@param QByteArray json串
@return string 
*/
QTCUSTOM_EXPORT STAnchorPermission ParseAnchorPermission(const QByteArray &json);
/**
@brief 面具库&背景库
@param QByteArray json串
@return std::vector<STWareHouseData> 
*/
QTCUSTOM_EXPORT std::vector<STWareHouseData> ParseWarehouseList(const QByteArray &json);
/**
@brief 暂停直播
@param QByteArray json串
@return string 
*/
QTCUSTOM_EXPORT STPause ParsePause(const QByteArray &json);

/**
@brief 恢复暂停直播
@param QByteArray json串
@return string 
*/
QTCUSTOM_EXPORT STRePause ParseRePause(const QByteArray &json);

/**
@brief 更新密码
@param QByteArray json串
@return string 
*/
QTCUSTOM_EXPORT STUpdatePassword ParseUpdatePassword(const QByteArray &json);

/**
@brief 房间列表
@param QByteArray json串
@return string 
*/
QTCUSTOM_EXPORT STRoomList ParseRoomList(const QByteArray &json);
/**
@brief 消息通知分类信息(带未读消息数)
@param QByteArray json串
@return STInfoClassList 
*/
QTCUSTOM_EXPORT STInfoClassList ParseInfoClassList(const QByteArray &json);
/**
@brief 消息通知列表
@param QByteArray json串
@return STPageList 
*/
QTCUSTOM_EXPORT STPageList ParsePageList(const QByteArray &json);
/**
@brief 获取IM登录token 
@param QByteArray json串
@return string 
*/
QTCUSTOM_EXPORT STIMToken ParseIMToken(const QByteArray &json);
/**
@brief 修改用户信息
@param QByteArray json串
@return STCsUser 
*/
QTCUSTOM_EXPORT STCsUser ParseCsUserInfo(const QByteArray &json);

QTCUSTOM_EXPORT STHeartBeat ParseHeartBeat(const QByteArray& json);

QTCUSTOM_EXPORT STDeviceDetail ParseDeviceDetail(const QByteArray& json);

#endif // !JSONPARSER_H
