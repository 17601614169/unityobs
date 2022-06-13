#include "json-parser.h"
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include "characters-convert.h"
#include <json/json.h>

void GetIntValue(const Json::Value &value,const char*key, int &ref) {
	if (value.isMember(key)) {
		if (value[key].isInt()) {
			ref = value[key].asInt();
		}
	}
}

void GetLongValue(const Json::Value &value, const char *key, double &ref)
{
	if (value.isMember(key)) {
		if (value[key].isDouble()) {
			ref = (double)value[key].asDouble();
		}
	}
}

void GetStringValue(const Json::Value &value, const char *key, std::string & ref)
{
	if (value.isMember(key)) {
		if (value[key].isString()) {
			ref = value[key].asString();
		}
	}
}

void GetBoolValue(const Json::Value &value, const char *key, bool & ref)
{
	if (value.isMember(key)) {
		if (value[key].isBool()) {
			ref = value[key].asBool();
		}
	}
}

void GetObjValue(const Json::Value &value, const char *key, Json::Value &obj) {
	if (value.isMember(key)) {
		if (value[key].isObject()) {
			obj = value[key];
		}
	}
}

void GetArrayValue(const Json::Value &value, const char *key, Json::Value &array) {
	if (value.isMember(key)) {
		if (value[key].isArray()) {
			array = value[key];
		}
	}
}

void ParseBase(const QByteArray& json,STBase & base) {
	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(json.data(), root)) {
		qDebug() << "parse error";
	} else {
		qDebug() << "parse success";
		if (root.isMember("appMsg")) {
			if (root["appMsg"].isString()) {
				base.appMsg = root["appMsg"].asString();
			}
		}
		if (root.isMember("appCode")) {
			if (root["appCode"].isString()) {
				base.appCode = root["appCode"].asString();
			}
		}
		if (root.isMember("success")) {
			if (root["success"].isBool()) {
				base.success = root["success"].asBool();
			}
		}
	}
}

STLoginRsp ParseLogin(const QByteArray &json)
{
	STLoginRsp rsp;
	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(json.data(), root)) {
		qDebug() << "parse error";
	} else {
		qDebug() << "parse success";
		ParseBase(json, (STBase &)rsp);
		if (root.isMember("result")) {
			if (root["result"].isObject()) {
				Json::Value result = root["result"];
				GetStringValue(result, "token",rsp.stService.token);
				if (result.isMember("user") && result["user"].isObject()) {
					Json::Value user = result["user"];
					GetIntValue(user, "id", rsp.stService.stUser.id);
					GetStringValue(user, "userName", rsp.stService.stUser.userName);
					GetStringValue(user, "realName", rsp.stService.stUser.realName);
					GetStringValue(user, "realNameEn", rsp.stService.stUser.realNameEn);
					GetStringValue(user, "avatarUrl", rsp.stService.stUser.avatarUrl);
					GetStringValue(user, "email", rsp.stService.stUser.email);
					GetStringValue(user, "mobile", rsp.stService.stUser.mobile);
					GetIntValue(user, "shopId", rsp.stService.stUser.shopId);
					GetStringValue(user, "shopName", rsp.stService.stUser.shopName);
					GetStringValue(user, "companyName", rsp.stService.stUser.companyName);
					GetIntValue(user, "type", rsp.stService.stUser.type);
					GetStringValue(user, "remark", rsp.stService.stUser.remark);
					GetIntValue(user, "status", rsp.stService.stUser.status);
					GetIntValue(user, "dflag", rsp.stService.stUser.dflag);
					GetIntValue(user, "createBy", rsp.stService.stUser.createBy);
					GetStringValue(user, "createRealName", rsp.stService.stUser.createRealName);
					GetLongValue(user, "createTime", rsp.stService.stUser.createTime);
					GetStringValue(user, "updateBy", rsp.stService.stUser.updateBy);
					GetStringValue(user, "updateRealName", rsp.stService.stUser.updateRealName);
					GetLongValue(user, "updateTime", rsp.stService.stUser.updateTime);
					GetIntValue(user, "roleId", rsp.stService.stUser.roleId);
					GetIntValue(user, "roleCode", rsp.stService.stUser.roleCode);
					GetStringValue(user, "roleName", rsp.stService.stUser.roleName);
					GetIntValue(user, "groupId", rsp.stService.stUser.groupId);
					GetStringValue(user, "groupName", rsp.stService.stUser.groupName);
					GetBoolValue(user, "isShopAdmin", rsp.stService.stUser.isShopAdmin);
					GetIntValue(user, "waitNo", rsp.stService.stUser.waitNo);
					if (user.isMember("csUser") && user["csUser"].isObject()) {
						Json::Value csuser = user["csUser"];
						GetIntValue(csuser, "accountType", rsp.stService.stUser.stCsUser.accountType);
						GetStringValue(csuser, "appGroupId", rsp.stService.stUser.stCsUser.appGroupId);
						GetStringValue(csuser, "avatarUrl", rsp.stService.stUser.stCsUser.avatarUrl);
						GetStringValue(csuser, "companyName", rsp.stService.stUser.stCsUser.companyName);
						GetIntValue(csuser, "conversationType", rsp.stService.stUser.stCsUser.conversationType);
						GetIntValue(csuser, "curSessionNum", rsp.stService.stUser.stCsUser.curSessionNum);
						GetIntValue(csuser, "daySessionNum", rsp.stService.stUser.stCsUser.daySessionNum);
						GetStringValue(csuser, "goodbyeInfo", rsp.stService.stUser.stCsUser.goodbyeInfo);
						GetIntValue(csuser, "groupId", rsp.stService.stUser.stCsUser.groupId);
						GetIntValue(csuser, "id", rsp.stService.stUser.stCsUser.id);
						GetStringValue(csuser, "name", rsp.stService.stUser.stCsUser.name);
						GetStringValue(csuser, "nameEn", rsp.stService.stUser.stCsUser.nameEn);
						GetIntValue(csuser, "noticeUnreadNum", rsp.stService.stUser.stCsUser.noticeUnreadNum);
						GetStringValue(csuser, "onlineDuration", rsp.stService.stUser.stCsUser.onlineDuration);
						GetIntValue(csuser, "receptionAmount", rsp.stService.stUser.stCsUser.receptionAmount);
						GetStringValue(csuser, "remark", rsp.stService.stUser.stCsUser.remark);
						GetIntValue(csuser, "shopId", rsp.stService.stUser.stCsUser.shopId);
						GetStringValue(csuser, "shopName", rsp.stService.stUser.stCsUser.shopName);
						GetIntValue(csuser, "sysUserId", rsp.stService.stUser.stCsUser.sysUserId);
						GetStringValue(csuser, "sysWelcomeInfo", rsp.stService.stUser.stCsUser.sysWelcomeInfo);
						GetIntValue(csuser, "waitJoinNum", rsp.stService.stUser.stCsUser.waitJoinNum);
						GetStringValue(csuser, "welcomeInfo", rsp.stService.stUser.stCsUser.welcomeInfo);
						GetStringValue(csuser, "welcomeSwitch", rsp.stService.stUser.stCsUser.welcomeSwitch);
						GetIntValue(csuser, "workStatus", rsp.stService.stUser.stCsUser.workStatus);
					}
					Json::Value menus = result["menus"];
					if (menus.isArray()) {
						STMenuItem menuItem;
						int num = menus.size();
						for (int i = 0; i < num; i++) {
							Json::Value item = menus[i];
							GetIntValue(item,"id",menuItem.id);
							GetIntValue(item,"parentId",menuItem.parentId);
							GetIntValue(item,"level",menuItem.level);
							GetStringValue(item,"code",menuItem.code);
							GetStringValue(item,"name",menuItem.name);
							GetStringValue(item,"path",menuItem.path);
							GetStringValue(item,"href",menuItem.href);
							GetStringValue(item,"icon",menuItem.icon);
							GetStringValue(item,"description",menuItem.description);

							GetIntValue(item,"sort",menuItem.sort);
							GetIntValue(item,"platform",menuItem.platform);
							GetIntValue(item,"withDataRule",menuItem.withDataRule);
							GetIntValue(item,"status",menuItem.status);
							GetIntValue(item,"createBy",menuItem.createBy);
							rsp.vecMenu.emplace_back(menuItem);
						}
					}
				}
			}
		}
	}
	return rsp;
}

STLiveList ParserLiveList(const QByteArray &json)
{
	STLiveList stLiveList;
	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(json.data(), root)) {
		qDebug() << "parse error";
	} else {
		qDebug() << "parse success";
		ParseBase(json, (STBase &)stLiveList);
		if (root.isMember("result")) {
			Json::Value result = root["result"];
			if (result.isArray()) {
				int size = result.size();
				for (int i = 0; i < size; i++) {
					STLiveItem stLiveItem;
					if (result[i].isMember("id")) {
						if (result[i]["id"].isInt()) {
							stLiveItem.id = result[i]["id"].asInt();
						}
					}
					if (result[i].isMember("labelName")) {
						if (result[i]["labelName"].isString()) {
							stLiveItem.name =
								result[i]["labelName"].asString();
						}
					}
					stLiveList.itemList.push_back(stLiveItem);
				}
			}
		}
	}
	return stLiveList;
}


STBeginRsp ParseBegin(const QByteArray &json)
{
	STBeginRsp stRsp;
	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(json.data(), root)) {
		qDebug() << "parse error";
	} else {
		qDebug() << "parse success";
		ParseBase(json, (STBase&)stRsp);
		if (root.isMember("result")) {
			Json::Value result = root["result"];
			if (result.isObject()) {
				if (result.isMember("liveId")) {
					if (result["liveId"].isInt()) {
						stRsp.liveId = result["liveId"].asInt();
					}
				}
				if (result.isMember("popularValue")) {
					if (result["popularValue"].isInt()) {
						stRsp.popularValue = result["popularValue"].asInt();
					}
				}
				if (result.isMember("roomId")) {
					if (result["roomId"].isInt()) {
						stRsp.roomId = result["roomId"].asInt();
					}
				}
				if (result.isMember("userId")) {
					if (result["userId"].isInt()) {
						stRsp.userId =
							result["userId"].asInt();
					}
				}
				if (result.isMember("avatar")) {
					if (result["avatar"].isString()) {
						stRsp.avatar = result["avatar"].asString();
					}
				}
				if (result.isMember("cover")) {
					if (result["cover"].isString()) {
						stRsp.cover = result["cover"].asString();
					}
				}
				if (result.isMember("labelName")) {
					if (result["labelName"].isString()) {
						stRsp.labelName = result["labelName"].asString();
					}
				}
				if (result.isMember("nickName")) {
					if (result["nickName"].isString()) {
						stRsp.labelName = result["nickName"].asString();
					}
				}
				if (result.isMember("notice")) {
					if (result["notice"].isString()) {
						stRsp.notice = result["notice"].asString();
					}
				}
				if (result.isMember("prompt")) {
					if (result["prompt"].isString()) {
						stRsp.prompt = result["prompt"].asString();
					}
				}
				if (result.isMember("startTime")) {
					if (result["startTime"].isString()) {
						stRsp.startTime = result["startTime"].asString();
					}
				}
				if (result.isMember("title")) {
					if (result["title"].isString()) {
						stRsp.title =  result["title"].asString();
					}
				}
				if (result.isMember("uid")) {
					if (result["uid"].isString()) {
						stRsp.uid = result["uid"].asString();
					}
				}
			}
		}
	}
	return stRsp;
}

STCahtTokenRsp ParseGetToken(const QByteArray &json)
{
	STCahtTokenRsp stRsp;
	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(json.data(), root)) {
		qDebug() << "parse error";
	} else {
		qDebug() << "parse success";
		ParseBase(json, (STBase &)stRsp);
		if (root.isMember("result")) {
			Json::Value result = root["result"];
			if (result.isMember("token")) {
				if (result["token"].isString()) {
					stRsp.token = result["token"].asString();
				}
			}
			if (result.isMember("userInfoVo")) {
				if (result["userInfoVo"].isObject()) {
					if (result["userInfoVo"].isMember(
						    "id")) {
						if (result["userInfoVo"]["id"].isString()) {
							stRsp.stUserInfo.id = result["userInfoVo"]["id"].asString();
						}
					}
					if (result["userInfoVo"].isMember("nickName")) {
						if (result["userInfoVo"]["nickName"].isString()) {
							stRsp.stUserInfo.nickName =result["userInfoVo"]["nickName"].asString();
						}
					}
					if (result["userInfoVo"].isMember("headPortrait")) {
						if (result["userInfoVo"]["headPortrait"].isString()) {
							stRsp.stUserInfo.headPortrait = result["userInfoVo"]["headPortrait"].asString();
						}
					}
					if (result["userInfoVo"].isMember("sourceUserId")) {
						if (result["userInfoVo"]["sourceUserId"].isString()) {
							stRsp.stUserInfo.sourceUserId = result["userInfoVo"]["sourceUserId"].asString();
						}
					}
					if (result["userInfoVo"].isMember("signature")) {
						if (result["userInfoVo"]["signature"].isString()) {
							stRsp.stUserInfo.signature = result["userInfoVo"]["signature"].asString();
						}
					}
					if (result["userInfoVo"].isMember("appKey")) {
						if (result["userInfoVo"]["appKey"].isString()) {
							stRsp.stUserInfo.appKey = result["userInfoVo"]["appKey"].asString();
						}
					}
					if (result["userInfoVo"].isMember("remark")) {
						if (result["userInfoVo"]["remark"].isString()) {
							stRsp.stUserInfo.remark = result["userInfoVo"]["remark"].asString();
						}
					}
					if (result["userInfoVo"].isMember("phone")) {
						if (result["userInfoVo"]["phone"].isString()) {
							stRsp.stUserInfo.phone = result["userInfoVo"]["phone"].asString();
						}
					}
					if (result["userInfoVo"].isMember("roomId")) {
						if (result["userInfoVo"]["roomId"].isInt()) {
							stRsp.stUserInfo.roomId = result["userInfoVo"]["roomId"].asInt();
						}
					}
					if (result["userInfoVo"].isMember("type")) {
						if (result["userInfoVo"]["type"].isInt()) {
							stRsp.stUserInfo.type = result["userInfoVo"]["type"].asInt();
						}
					}
					if (result["userInfoVo"].isMember("status")) {
						if (result["userInfoVo"]["status"].isInt()) {
							stRsp.stUserInfo.status = result["userInfoVo"]["status"].asInt();
						}
					}
					if (result["userInfoVo"].isMember("device")) {
						if (result["userInfoVo"]["device"].isInt()) {
							stRsp.stUserInfo.device = result["userInfoVo"]["device"].asInt();
						}
					}
					if (result["userInfoVo"].isMember("liveId")) {
						if (result["userInfoVo"]["liveId"].isInt()) {
							stRsp.stUserInfo.liveId = result["userInfoVo"]["liveId"].asInt();
						}
					}
					if (result["userInfoVo"].isMember("groupId")) {
						if (result["userInfoVo"]["groupId"].isInt()) {
							stRsp.stUserInfo.groupId = result["userInfoVo"]["groupId"].asInt();
						}
					}
				}
			}
		}
	}
	return stRsp;
}

int ParseFansList(const QByteArray &json)
{
	Json::Reader reader;
	Json::Value root;
	STBase base;
	int count = 0;
	if (!reader.parse(json.data(), root)) {
		qDebug() << "parse error";
	} else {
		qDebug() << "parse success";
		ParseBase(json, base);
		if (base.success) {
			if (root.isMember("result")) {
				Json::Value result = root["result"];
				if (result.isMember("total")) {
					if (result["total"].isInt())
						count = result["total"].asInt();
				}
			}
		}
		
	}
	return count;
}

STCompteInfo ParseAccount(const QByteArray &json)
{
	STCompteInfo stInfo;
	Json::Reader reader;
	Json::Value root;
	int count = 0;
	if (!reader.parse(json.data(), root)) {
		qDebug() << "parse error";
	} else {
		qDebug() << "parse success";
		ParseBase(json, (STBase &)stInfo);
		if (root.isMember("result")) {
			Json::Value result = root["result"];
			if (result.isMember("tnTotalAmount")) {
				if (result["tnTotalAmount"].isDouble())
					stInfo.tnTotalAmount = result["tnTotalAmount"].asDouble();
			}
			if (result.isMember("pnTotalAmount")) {
				if (result["pnTotalAmount"].isDouble())
					stInfo.pnTotalAmount = result["pnTotalAmount"].asDouble();
			}
			if (result.isMember("pnWithdrawAbleAmount")) {
				if (result["pnWithdrawAbleAmount"].isDouble())
					stInfo.pnWithdrawAbleAmount = result["pnWithdrawAbleAmount"].asDouble();
			}
			if (result.isMember("type")) {
				if (result["type"].isString())
					stInfo.type = result["type"].asString();
			}
			if (result.isMember("name")) {
				if (result["name"].isString())
					stInfo.name = result["name"].asString();
			}
			if (result.isMember("account")) {
				if (result["account"].isString())
					stInfo.name = result["account"].asString();
			}
			if (result.isMember("bankName")) {
				if (result["bankName"].isString())
					stInfo.bankName = result["bankName"].asString();
			}
		}
	}
	return stInfo;
}

STFinishRsp ParseFinish(const QByteArray &json)
{
	STFinishRsp stRsp;
	Json::Reader reader;
	Json::Value root;
	int count = 0;
	if (!reader.parse(json.data(), root)) {
		qDebug() << "parse error";
	} else {
		qDebug() << "parse success";
		ParseBase(json, (STBase &)stRsp);
		if (root.isMember("result")) {
			if (root["result"].isObject()) {
				Json::Value result = root["result"];
				if (result.isMember("visitorNum")) {
					if (result["visitorNum"].isInt())
						stRsp.visitorNum = result["visitorNum"].asInt();
				}
				if (result.isMember("income")) {
					if (result["income"].isInt())
						stRsp.visitorNum = result["income"].asInt();
				}
				if (result.isMember("newAttention")) {
					if (result["newAttention"].isInt())
						stRsp.visitorNum = result["newAttention"].asInt();
				}
				if (result.isMember("liveTime")) {
					if (result["liveTime"].isString())
						stRsp.liveTime = result["liveTime"].asString();
				}
			} else {
				GetBoolValue(root,"result",stRsp.bResult);
			}
		}
	}
	return stRsp;
}

STDoBacklistRsp ParseDoBacklist(const QByteArray &json)
{
	STDoBacklistRsp stRsp;
	Json::Reader reader;
	Json::Value root;
	int count = 0;
	if (!reader.parse(json.data(), root)) {
		qDebug() << "parse error";
	} else {
		qDebug() << "parse success";
		ParseBase(json, (STBase &)stRsp);
	}
	return stRsp;
}

STGiftList ParseGiftList(const QByteArray& json) {
	STGiftList stRsp;
	Json::Reader reader;
	Json::Value root;
	int count = 0;
	if (!reader.parse(json.data(), root)) {
		qDebug() << "parse error";
	} else {
		qDebug() << "parse success";
		ParseBase(json, (STBase &)stRsp);
		if (root.isMember("result")) {
			Json::Value result = root["result"];

			if (result.isMember("page")) {
				if (result["page"].isInt())
					stRsp.page = result["page"].asInt();
			}
			if (result.isMember("pageNum")) {
				if (result["pageNum"].isInt())
					stRsp.pageNum = result["pageNum"].asInt();
			}
			if (result.isMember("size")) {
				if (result["size"].isInt())
					stRsp.size = result["size"].asInt();
			}
			if (result.isMember("total")) {
				if (result["total"].isInt())
					stRsp.total = result["total"].asInt();
			}
			if (result.isMember("datas")) {
				if (result["datas"].isArray()) {
					Json::Value datas = result["datas"];
					int size = datas.size();
					for (int i = 0; i < size; i++) {
						STGift stGift;
						if (datas[i].isMember( "giftPrice")) {
							if (datas[i]["giftPrice"].isInt()) {
								stGift.giftPrice = datas[i]["giftPrice"].asInt();
							}
						}
						if (datas[i].isMember("giftId")) {
							if (datas[i]["giftId"].isInt()) {
								stGift.giftId = datas[i]["giftId"].asInt();
							}
						}
						if (datas[i].isMember("giftEffects")) {
							if (datas[i]["giftEffects"].isString()) {
								stGift.giftEffects = datas[i]["giftEffects"].asString();
							}
						}
						if (datas[i].isMember("giftName")) {
							if (datas[i]["giftName"].isString()) {
								stGift.giftName = datas[i]["giftName"].asString();
							}
						}
						if (datas[i].isMember("giftPic")) {
							if (datas[i]["giftPic"].isString()) {
								stGift.giftPic = datas[i]["giftPic"].asString();
							}
						}
						if (datas[i].isMember("giftZip")) {
							if (datas[i]["giftZip"].isString()) {
								stGift.giftZip = datas[i]["giftZip"].asString();
							}
						}
						if (datas[i].isMember("version")) {
							if (datas[i]["version"].isString()) {
								stGift.version = datas[i]["version"].asString();
							}
						}
						stRsp.lists.push_back(stGift);
					}
				}
			}
		}
	}
	return stRsp;
}


STMessageBody ParseMessageBody(const QByteArray &json)
{
	STMessageBody stBody;
	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(json.data(), root)) {
		qDebug() << "parse error";
	} else {
		qDebug() << "parse success";
		if (root.isMember("type")) {
			if (root["type"].isInt())
				stBody.type = root["type"].asInt();
		}
		if (root.isMember("contentType")) {
			if (root["contentType"].isInt())
				stBody.contentType = root["contentType"].asInt();
		}
		if (stBody.contentType == TYPE_CHAT ||
		    stBody.contentType == TYPE_FORCE_LOG_OUT) {
			if (root.isMember("content")) {
				if (root["content"].isString())
					stBody.content = root["content"].asString();
			}
		} else if (stBody.contentType == TYPE_GIFT) {
			if (root.isMember("content")) {
				if (root["content"].isString()) {
					Json::Value gift = root["content"];
					std::string content = gift.asString();
					STSendGift stGift = ParseSendGift(content.data());
					stBody.stSendGift = stGift;
				}
			}
		} else if (stBody.contentType == TYPE_RED_PACKET) {
			if (root.isMember("content")) {
				if (root["content"].isString()) {
					Json::Value gift = root["content"];
					std::string content = gift.asString();
					STRedPacket stRedPacket =
						ParseRedPacket(content.data());
					stBody.stRedPacket = stRedPacket;
				}
			}
		}
		if (root.isMember("fromUserType")) {
			if (root["fromUserType"].isInt())
				stBody.fromUserType =
					root["fromUserType"].asInt();
		}
		if (root.isMember("createTime")) {
			if (root["createTime"].isDouble())
				stBody.createTime = root["createTime"].asDouble();
		}
		
		if (root.isMember("fromUserId")) {
			if (root["fromUserId"].isString())
				stBody.fromUserId = root["fromUserId"].asString();
		}
		if (root.isMember("fromUserName")) {
			if (root["fromUserName"].isString())
				stBody.fromUserName = root["fromUserName"].asString();
		}
		if (root.isMember("groupId")) {
			if (root["groupId"].isString())
				stBody.groupId = root["groupId"].asString();
		}
		if (root.isMember("realUserNo")) {
			if (root["realUserNo"].isString())
				stBody.realUserNo = root["realUserNo"].asString();
		}
		if (root.isMember("fromUserAvatar")) {
			if (root["fromUserAvatar"].isString())
				stBody.fromUserAvatar = root["fromUserAvatar"].asString();
		}
	}
	return stBody;
}


STSendGift ParseSendGift(const QByteArray& json) {
	STSendGift stGift;
	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(json.data(), root)) {
		qDebug() << "parse error";
	} else {
		qDebug() << "parse success";
		if (root.isMember("giftId")) {
			if (root["giftId"].isInt())
				stGift.giftId = root["giftId"].asInt();
		}
		if (root.isMember("giftNum")) {
			if (root["giftNum"].isInt())
				stGift.giftNum = root["giftNum"].asInt();
		}
		if (root.isMember("giftName")) {
			if (root["giftName"].isString())
				stGift.giftName = root["giftName"].asString();
		}
		if (root.isMember("giftEffects")) {
			if (root["giftEffects"].isString())
				stGift.giftEffects = root["giftEffects"].asString();
		}
		if (root.isMember("giftEffects")) {
			if (root["giftEffects"].isString())
				stGift.giftEffects = root["giftEffects"].asString();
		}
		if (root.isMember("userName")) {
			if (root["userName"].isString())
				stGift.userName = root["userName"].asString();
		}
		if (root.isMember("giftUrl")) {
			if (root["giftUrl"].isString())
				stGift.giftUrl = root["giftUrl"].asString();
		}
	}
	return stGift;
}

STRedPacket ParseRedPacket(const QByteArray &json)
{
	STRedPacket stRedPacket;
	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(json.data(), root)) {
		qDebug() << "parse error";
	} else {
		qDebug() << "parse success";
		if (root.isMember("disType")) {
			if (root["disType"].isInt())
				stRedPacket.disType = root["disType"].asInt();
		}
		if (root.isMember("giftNum")) {
			if (root["giftNum"].isInt())
				stRedPacket.totalAmount = root["totalAmount"].asInt();
		}
		if (root.isMember("totalNumber")) {
			if (root["totalNumber"].isInt())
				stRedPacket.totalNumber = root["totalNumber"].asInt();
		}
		if (root.isMember("redType")) {
			if (root["redType"].isInt())
				stRedPacket.redType = root["redType"].asInt();
		}
		if (root.isMember("id")) {
			if (root["id"].isInt())
				stRedPacket.id = root["id"].asInt();
		}
		if (root.isMember("userId")) {
			if (root["userId"].isInt())
				stRedPacket.userId = root["userId"].asInt();
		}
	}
	return stRedPacket;
}

STNoticeRsp ParseNotice(const QByteArray& json) {
	STNoticeRsp stRsp;
	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(json.data(), root)) {
		qDebug() << "parse error";
	} else {
		qDebug() << "parse success";
		ParseBase(json, (STBase &)stRsp);
		if (root.isMember("result")) {
			Json::Value result = root["result"];
			if (result.isMember("id")) {
				if (result["id"].isInt())
					stRsp.id = root["id"].asInt();
			}
			if (result.isMember("status")) {
				if (result["status"].isInt())
					stRsp.status = result["status"].asInt();
			}
			if (result.isMember("anchorId")) {
				if (result["anchorId"].isInt())
					stRsp.anchorId = result["anchorId"].asInt();
			}
			if (result.isMember("shopId")) {
				if (result["shopId"].isInt())
					stRsp.shopId = result["shopId"].asInt();
			}
			if (result.isMember("remind")) {
				if (result["remind"].isString())
					stRsp.remind = result["remind"].asString();
			}
			if (result.isMember("code")) {
				if (result["code"].isString())
					stRsp.code = result["code"].asString();
			}
			if (result.isMember("title")) {
				if (result["title"].isString())
					stRsp.title = result["title"].asString();
			}
			if (result.isMember("notice")) {
				if (result["notice"].isString())
					stRsp.notice = result["notice"].asString();
			}
			if (result.isMember("cover")) {
				if (result["cover"].isString())
					stRsp.cover = result["cover"].asString();
			}
		}
	}
	return stRsp;
}

STUserBasicInfo ParseUserBasicInfo(const QByteArray &json)
{
	STUserBasicInfo stUserBasicInfo;
	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(json.data(), root)) {
		qDebug() << "parse error";
	} else {
		qDebug() << "parse success";
		if (root.isMember("result")) {
			Json::Value result = root["result"];
			if (result.isMember("id")) {
				if (result["id"].isInt()) {
					stUserBasicInfo.id = result["id"].asInt();
				}
			}
			if (result.isMember("phone")) {
				if (result["phone"].isString()) {
					stUserBasicInfo.phone = result["phone"].asString();
				}
			}
			if (result.isMember("userNo")) {
				if (result["userNo"].isString()) {
					stUserBasicInfo.userNo = result["userNo"].asString();
				}
			}
			if (result.isMember("avatar")) {
				if (result["avatar"].isString()) {
					stUserBasicInfo.avatar = result["avatar"].asString();
				}
			}
			if (result.isMember("nickname")) {
				if (result["nickname"].isString()) {
					stUserBasicInfo.nickname = result["nickname"].asString();
				}
			}
			if (result.isMember("userLevel")) {
				if (result["userLevel"].isString()) {
					stUserBasicInfo.userLevel = result["userLevel"].asString();
				}
			}
			if (result.isMember("userIntro")) {
				if (result["userIntro"].isString()) {
					stUserBasicInfo.userIntro = result["userIntro"].asString();
				}
			}
			if (result.isMember("nicknameModifyLimit")) {
				if (result["nicknameModifyLimit"].isInt()) {
					stUserBasicInfo.nicknameModifyLimit = result["nicknameModifyLimit"].asInt();
				}
			}
			if (result.isMember("amount")) {
				if (result["amount"].isInt()) {
					stUserBasicInfo.amount = result["amount"].asInt();
				}
			}
			if (result.isMember("memberStartTime")) {
				if (result["memberStartTime"].isString()) {
					stUserBasicInfo.memberStartTime = result["memberStartTime"].asString();
				}
			}
			if (result.isMember("memberEndTime")) {
				if (result["memberEndTime"].isString()) {
					stUserBasicInfo.memberEndTime = result["memberEndTime"].asString();
				}
			}
			if (result.isMember("isMember")) {
				if (result["isMember"].isBool()) {
					stUserBasicInfo.isMember = result["isMember"].asBool();
				}
			}
		}
	}

	return stUserBasicInfo;
}

std::string ParseLiveTitle(const QByteArray &json)
{
	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(json.data(), root)) {
		qDebug() << "parse error";
	} else {
		qDebug() << "parse success";
		if (root.isMember("result")) {
			if (root["result"].isString())
				return root["result"].asString();
		}
	}

	return std::string("");
}

void ParseScreenShot(const QByteArray &json, QRect &rect)
{
	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(json.data(), root)) {
		qDebug() << "ParseCameraSceen parse error";
	} else {
		qDebug() << "ParseCameraSceen parse success";
		if (root.isMember("x1")) {
			rect.setLeft(root["x1"].asInt());
		}
		if (root.isMember("y1")) {
			rect.setTop(root["y1"].asInt());
		}
		if (root.isMember("x2")) {
			rect.setRight(root["x2"].asInt());
		}
		if (root.isMember("y2")) {
			rect.setBottom(root["y2"].asInt());
		}
	}
}

STAnchorPermission ParseAnchorPermission(const QByteArray &json)
{
	STAnchorPermission permission;
	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(json.data(), root)) {
		qDebug() << "parse error";
	} else {
		qDebug() << "parse success";
		ParseBase(json, (STBase &)permission);
		if (root.isMember("result")) {
			Json::Value result = root["result"];
			if (result.isMember("applyTime")) {
				if (result["applyTime"].isString())
					permission.applyTime = root["applyTime"].asString();
			}
			if (result.isMember("cover")) {
				if (result["cover"].isString())
					permission.cover = root["cover"].asString();
			}
			if (result.isMember("notice")) {
				if (result["notice"].isString())
					permission.notice = root["notice"].asString();
			}
			if (result.isMember("status")) {
				if (result["status"].isString())
					permission.status = root["status"].asString();
			}
		}
	}
	return permission;
}

std::vector<STWareHouseData> ParseWarehouseList(const QByteArray &json)
{
	std::vector<STWareHouseData> vcWareHouse;
	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(json.data(), root)) {
		qDebug() << "parse error";
	} else {
		qDebug() << "parse success";
		if (root.isMember("success")) {
			bool success = false;
			GetBoolValue(root, "success", success);
			if (success) {
				Json::Value result;
				GetObjValue(root, "result", result);
				Json::Value datas;
				GetArrayValue(result, "datas", datas);
				int size = datas.size();
				for (int i = 0; i < size; i++) {
					STWareHouseData stData;
					GetIntValue(datas[i], "createBy", stData.createBy);
					GetIntValue(datas[i], "delFlag", stData.delFlag);
					GetStringValue(datas[i], "fileAddress", stData.fileAddress);
					GetIntValue(datas[i], "fileType", stData.fileType);
					GetIntValue(datas[i], "id", stData.id);
					GetIntValue(datas[i], "shopId", stData.shopId);
					GetIntValue(datas[i], "site", stData.site);
					GetIntValue(datas[i], "warehouseNo", stData.warehouseNo);

					vcWareHouse.push_back(stData);
				}
			}
		}
	}

	return vcWareHouse;
}
STPause ParsePause(const QByteArray& json) {
	STPause stPause;
	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(json.data(), root)) {
		qDebug() << "parse error";
	} else {
		qDebug() << "parse success";
		ParseBase(json, (STBase &)stPause);
		GetBoolValue(root,"result",stPause.bResult);
	}
	return stPause;
}

STRePause ParseRePause(const QByteArray &json){
	STRePause stPause;
	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(json.data(), root)) {
		qDebug() << "parse error";
	} else {
		qDebug() << "parse success";
		ParseBase(json, (STBase &)stPause);
		GetBoolValue(root,"result",stPause.bResult);
	}
	return stPause;
}

STUpdatePassword ParseUpdatePassword(const QByteArray& json) {
	STUpdatePassword stUpdatePassword;
	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(json.data(), root)) {
		qDebug() << "parse error";
	} else {
		qDebug() << "parse success";
		ParseBase(json, (STBase &)stUpdatePassword);
	}
	return stUpdatePassword;
}

STRoomList ParseRoomList(const QByteArray& json) {
	STRoomList stRoomList;
	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(json.data(), root)) {
		qDebug() << "parse error";
	} else {
		qDebug() << "parse success";
		ParseBase(json, (STBase &)stRoomList);
		if (root.isMember("result") && root["result"].isObject()) {
			Json::Value result=root["result"];
			GetIntValue(result,"page",stRoomList.page);
			GetIntValue(result,"size",stRoomList.size);
			GetIntValue(result,"total",stRoomList.total);
			GetIntValue(result,"pageNum",stRoomList.pageNum);
			if (result.isMember("datas") && result["datas"].isArray()) {
				Json::Value datas = result["datas"];
				int size = datas.size();
				for (int i = 0; i < size; i++) {
					STRoomItem item;
					Json::Value vItem = datas[i];
					GetIntValue(vItem,"id",item.id);
					GetIntValue(vItem,"liveNo",item.liveNo);
					GetIntValue(vItem,"roomId",item.roomId);
					GetIntValue(vItem,"anchorId",item.anchorId);
					GetIntValue(vItem,"closeUser",item.closeUser);
					GetIntValue(vItem,"closeUser",item.closeUserType);
					GetIntValue(vItem,"closeUser",item.watchPeople);
					GetIntValue(vItem,"closeUser",item.watchPeopleMax);
					GetIntValue(vItem,"closeUser",item.toGift);

					GetIntValue(vItem,"closeUser",item.liveStatus);
					GetIntValue(vItem,"closeUser",item.delFlag);
					GetIntValue(vItem,"closeUser",item.createBy);
					GetIntValue(vItem,"closeUser",item.inLiveId);

					GetStringValue(vItem,"title",item.title);
					GetStringValue(vItem,"liveCover",item.liveCover);
					GetStringValue(vItem,"anchorName",item.anchorName);
					GetStringValue(vItem,"closeUserName",item.closeUserName);

					GetLongValue(vItem,"openTime",item.openTime);
					GetLongValue(vItem,"closeTime",item.closeTime);
					GetLongValue(vItem,"createTime",item.createTime);

					stRoomList.roomList[item.id]=item;
				}
			}
			
		}
	}
	return stRoomList;
}

QTCUSTOM_EXPORT STInfoClassList ParseInfoClassList(const QByteArray &json)
{
	STInfoClassList stClassList;
	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(json.data(), root)) {
		qDebug() << "parse error";
	} else {
		qDebug() << "parse success";
		ParseBase(json, (STBase &)stClassList);
		if (root.isMember("result") && root["result"].isObject()) {
			Json::Value result=root["result"];
			if (result.isMember("classItemVoList") && result["classItemVoList"].isArray()) {
				Json::Value classItemVoList = result["classItemVoList"];
				int size = classItemVoList.size();
				for (int i = 0; i < size; i++) {
					STInfoClass item;
					Json::Value vItem = classItemVoList[i];
					GetStringValue(vItem, "classCode", item.classCode);
					GetIntValue(vItem, "classId", item.classId);
					GetStringValue(vItem, "className", item.className);
					GetIntValue(vItem, "unreadNum", item.unreadNum);

					stClassList.classlist.push_back(item);
				}
			}
		}
	}
	return stClassList;
}

QTCUSTOM_EXPORT STPageList ParsePageList(const QByteArray &json)
{
	STPageList stPageList;
	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(json.data(), root)) {
		qDebug() << "parse error";
	} else {
		qDebug() << "parse success";
		ParseBase(json, (STBase &)stPageList);
		if (root.isMember("result") && root["result"].isObject()) {
			Json::Value result=root["result"];
			GetIntValue(result, "page", stPageList.page);
			GetIntValue(result, "pageNum", stPageList.pageNum);
			GetIntValue(result, "size", stPageList.size);
			GetIntValue(result, "total", stPageList.total);
			if (result.isMember("datas") && result["datas"].isArray()) {
				Json::Value datas = result["datas"];
				int size = datas.size();
				for (int i = 0; i < size; i++) {
					STPageData item;
					Json::Value vItem = datas[i];
					GetIntValue(vItem, "classId", item.classId);
					GetStringValue(vItem, "content", item.content);
					GetIntValue(vItem, "id", item.id);
					GetIntValue(vItem, "readStatus", item.readStatus);
					GetLongValue(vItem, "readTime", item.readTime);
					GetLongValue(vItem, "sendTime", item.sendTime);
					GetStringValue(vItem, "senderName", item.senderName);
					GetStringValue(vItem, "title", item.title);

					stPageList.pagelist.push_back(item);
				}
			}
		}
	}
	return stPageList;
}

STIMToken ParseIMToken(const QByteArray& json)
{
	STIMToken stImToken;
	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(json.data(), root)) {
		qDebug() << "parse error";
	} else {
		qDebug() << "parse success";
		ParseBase(json, (STBase &)stImToken);
		if (root.isMember("result")) {
			Json::Value result = root["result"];
			GetStringValue(result,"token",stImToken.token);
			if (result.isMember("userInfoVo") && result["userInfoVo"].isObject()) {
				Json::Value userInfoVo = result["userInfoVo"];
				GetStringValue(userInfoVo,"id",stImToken.stUserInfo.id);
				GetStringValue(userInfoVo,"nickName",stImToken.stUserInfo.nickName);
				GetStringValue(result,"headPortrait",stImToken.stUserInfo.headPortrait);
				GetStringValue(userInfoVo,"sourceUserId",stImToken.stUserInfo.sourceUserId);
				GetIntValue(userInfoVo,"type",stImToken.stUserInfo.type);
				GetStringValue(userInfoVo,"signature",stImToken.stUserInfo.signature);
				GetIntValue(userInfoVo,"status",stImToken.stUserInfo.status);
				GetStringValue(userInfoVo,"appKey",stImToken.stUserInfo.appKey);
				GetStringValue(userInfoVo,"remark",stImToken.stUserInfo.remark);
				GetStringValue(userInfoVo,"phone",stImToken.stUserInfo.phone);
				GetStringValue(userInfoVo,"loginIp",stImToken.stUserInfo.loginIp);
				GetIntValue(userInfoVo,"device",stImToken.stUserInfo.device);

				GetIntValue(userInfoVo,"roomId",stImToken.stUserInfo.roomId);
				GetIntValue(userInfoVo,"loginIp",stImToken.stUserInfo.liveId);
				GetIntValue(userInfoVo,"loginIp",stImToken.stUserInfo.groupId);
			}
		}
	}
	return stImToken;
}

QTCUSTOM_EXPORT STCsUser ParseCsUserInfo(const QByteArray &json)
{
	STCsUser stCsUser;
	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(json.data(), root)) {
		qDebug() << "parse error";
	} else {
		qDebug() << "parse success";
		if (root.isMember("result")) {
			Json::Value result = root["result"];
			GetIntValue(result, "accountType", stCsUser.accountType);
			GetStringValue(result, "appGroupId", stCsUser.appGroupId);
			GetStringValue(result, "avatarUrl", stCsUser.avatarUrl);
			GetStringValue(result, "companyName", stCsUser.companyName);
			GetIntValue(result, "conversationType", stCsUser.conversationType);
			GetIntValue(result, "curSessionNum", stCsUser.curSessionNum);
			GetIntValue(result, "daySessionNum", stCsUser.daySessionNum);
			GetStringValue(result, "goodbyeInfo", stCsUser.goodbyeInfo);
			GetIntValue(result, "groupId", stCsUser.groupId);
			GetIntValue(result, "id", stCsUser.id);
			GetStringValue(result, "name", stCsUser.name);
			GetStringValue(result, "nameEn", stCsUser.nameEn);
			GetIntValue(result, "noticeUnreadNum", stCsUser.noticeUnreadNum);
			GetStringValue(result, "onlineDuration", stCsUser.onlineDuration);
			GetIntValue(result, "receptionAmount", stCsUser.receptionAmount);
			GetStringValue(result, "remark", stCsUser.remark);
			GetIntValue(result, "shopId", stCsUser.shopId);
			GetStringValue(result, "shopName", stCsUser.shopName);
			GetIntValue(result, "sysUserId", stCsUser.sysUserId);
			GetStringValue(result, "sysWelcomeInfo", stCsUser.sysWelcomeInfo);
			GetIntValue(result, "waitJoinNum", stCsUser.waitJoinNum);
			GetStringValue(result, "welcomeInfo", stCsUser.welcomeInfo);
			GetStringValue(result, "welcomeSwitch", stCsUser.welcomeSwitch);
			GetIntValue(result, "workStatus", stCsUser.workStatus);
		}
	}
	return stCsUser;
}


QTCUSTOM_EXPORT STHeartBeat ParseHeartBeat(const QByteArray& json) {
	STHeartBeat stHeartBeat;
	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(json.data(), root)) {
		qDebug() << "parse error";
	} else {
		qDebug() << "parse success";
		ParseBase(json, (STBase &)stHeartBeat);
	}
	return stHeartBeat;
}

STDeviceDetail ParseDeviceDetail(const QByteArray& json) {
	STDeviceDetail stDeviceDetail;
	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(json.data(), root)) {
		qDebug() << "parse error";
	} else {
		qDebug() << "parse success";
		ParseBase(json, (STBase &)stDeviceDetail);
		if (root.isMember("result")) {
			Json::Value result=root["result"];
			if (result.isNull()) {
				stDeviceDetail.applyStatus = -1;
			} else {
				GetIntValue(result, "id", stDeviceDetail.id);
				GetIntValue(result, "shopId", stDeviceDetail.shopId);
				GetIntValue(result, "type", stDeviceDetail.type);
				GetStringValue(result, "str", stDeviceDetail.str);

				GetIntValue(result, "gmtCreated", stDeviceDetail.gmtCreated);
				GetIntValue(result, "userCreated", stDeviceDetail.userCreated);
				GetIntValue(result, "applyStatus", stDeviceDetail.applyStatus);
				GetStringValue(result, "checkCode", stDeviceDetail.checkCode);
			}
		}
	}
	return stDeviceDetail;
}
