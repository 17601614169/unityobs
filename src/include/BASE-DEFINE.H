#ifndef BASEDEFINE_H
#define BASEDEFINE_H
#include <list>
#include <vector>
#include <string>
#include <map>
#define SINGLETON_DEFINE(TypeName)             \
	static TypeName *GetInstance()         \
	{                                      \
		static TypeName type_instance; \
		return &type_instance;         \
	}                                      \
                                               \
	TypeName(const TypeName &) = delete;   \
	TypeName &operator=(const TypeName &) = delete;

#define SOURCEWIDGET_WIDTH	130
#define SOURCEWIDGET_HEIGHT	255
#define SCREENSHOT_TOPHEIGHT	30
#define SCREENSHOT_PADDING	3
#define OVERSEAS_TEST 0
#define OVERSEAS 1
#if OVERSEAS
/*
* 海外环境https://ics.aicspro.com/apis,https://ics.aicspro.com,wss://video.aicspro.com
* 海外测试环境https://icstest.aicspro.com/apis,https://icstest.aicspro.com,wss://videotest.aicspro.com
*/
#if OVERSEAS_TEST
const std::string BASE_URL_KEEPFUN = std::string("https://icstest.aicspro.com/apis");
const std::string BASE_URL = BASE_URL_KEEPFUN;
const std::string BASE_ROUTER_URL = std::string("https://icstest.aicspro.com");
const std::string MEDIASOUP_URL = std::string("wss://videotest.aicspro.com");
const std::string ANCHOR_ROUTER = std::string("/anchor");
#else
const std::string BASE_URL_KEEPFUN = std::string("https://ics.aicspro.com/apis");
const std::string BASE_URL = BASE_URL_KEEPFUN;
const std::string BASE_ROUTER_URL = std::string("https://ics.aicspro.com");
const std::string MEDIASOUP_URL = std::string("wss://video.aicspro.com");
const std::string ANCHOR_ROUTER = std::string("/anchor");
#endif
#else

const std::string BASE_URL_KEEPFUN = std::string("https://www.keepfun.cn:29006");
const std::string BASE_URL = BASE_URL_KEEPFUN;
const std::string BASE_ROUTER_URL = std::string("https://www.keepfun.cn");
const std::string MEDIASOUP_URL = std::string("wss://101.133.230.207:4443");
const std::string ANCHOR_ROUTER = std::string("/ics-anchor");
#endif


const std::string USER_EXCHANGE_HTML	= BASE_ROUTER_URL + ANCHOR_ROUTER + std::string("/transactionRecord.html?token=%s");
const std::string REDPACKET_EDU_HTML	= BASE_ROUTER_URL + ANCHOR_ROUTER + std::string("/redPackRecord.html?token=%s");
const std::string ROOMLIST_HTML		= BASE_ROUTER_URL + ANCHOR_ROUTER + std::string("/myLiveroom.html?token=%s&imToken=%s");

const std::string IMCHAT_HTML		= BASE_ROUTER_URL + ANCHOR_ROUTER + std::string("/Liveroom.html?token=%s&imToken=%s&sysUserId=%d&imId=%s");

const std::string DATA_STATICS_HTML	= BASE_ROUTER_URL + ANCHOR_ROUTER + std::string("/liveData.html?token=%s");
const std::string LIVE_DETAIL_HTML	= BASE_ROUTER_URL + ANCHOR_ROUTER + std::string("/liveDetail.html?token=%s");
const std::string KNOWLEDGE_HTML	= BASE_ROUTER_URL + ANCHOR_ROUTER + std::string("/knowledge.html?token=%s");
const std::string QUICK_REPLY_HTML	= BASE_ROUTER_URL + ANCHOR_ROUTER + std::string("/quick-reply.html?token=%s");
const std::string LIVE_EVALYATION_HTML  = BASE_ROUTER_URL + ANCHOR_ROUTER + std::string("/evaluate.html?token=%s");
#define USE_LOCAL_URL 0
#define CUSTOM_POP_ENABLED 1
enum STRINDEX {
	STR_START_LIVE = 1,
	STR_STOP_LIVE = 2,
	STR_SECRET = 3,
	STR_OPEN_EXIST_FILE_NAME = 4,
	STR_START_LIVE_TIP =5,
	STR_PLEASE_START_LIVE_TIP = 6,
	STR_LOGIN_TITLE = 7,
	STR_LOGIN_ERROR = 8,
	STR_SCREEN_SHOT = 9,
	STR_STREAM = 10,
	STR_PROCESS = 11,
	STR_PROCESS_ALREADY_EXIST = 12,
	STR_PROCESS_INVALID_ARGV=13,
	STR_HANDLE_SUCCESS = 14,
	STR_HANDLE_FAIL =15,
	STR_HANDLE_INPUT_NOEMPTY=16,
	STR_CAMERA = 17,
	STR_CAMERA_USED = 18,
	STR_FINISH_LIVE = 19,
	STR_CLOSE_SOURCE = 20,
	STR_PROCESS_LOGIN_EXPIRED = 21,
	STR_UNIQUECODE=22,
	STR_UNIQUECODE_AUTHENTICATED=23,
	STR_UNIQUECODE_REJECT=24,
	STR_UNIQUECODE_REQ=25,
	STR_WHITE_ARGS_NOT_NULL = 26,
	STR_WHITE_APPLY = 27,
	STR_UNIQUECODE_AUTHING=28,
	STR_PASSWORD_LIMIT=29,
	STR_CONFIRM_PASSWORD_AGAIN=30,
	STR_FONT_SET_NOEMPTY=31,
	STR_IMAGE_NOEMPTY=32
};

enum StreamState {
	STREAM_START = 0,
	STREAM_PAUSE = 1,
	STREAM_STOP=2,
	STREAM_REPAUSE = 3
};

struct STBase {
	bool success;
	std::string appCode;
	std::string appMsg;
	STBase() {
		success = true;
	}
};

struct STCsUser {
	int accountType;
	std::string appGroupId;
	std::string avatarUrl;
	std::string companyName;
	int conversationType;
	int curSessionNum;
	int daySessionNum;
	std::string goodbyeInfo;
	int groupId;
	int id;
	std::string name;
	std::string nameEn;
	int noticeUnreadNum;
	std::string onlineDuration;
	int receptionAmount;
	std::string remark;
	int shopId;
	std::string shopName;
	int sysUserId;
	std::string sysWelcomeInfo;
	int waitJoinNum;
	std::string welcomeInfo;
	std::string welcomeSwitch;
	int workStatus;
};

struct STUser {
	int id;
	std::string userName;
	std::string realName;
	std::string realNameEn;
	std::string avatarUrl;
	std::string email;
	std::string mobile;
	int shopId;
	std::string shopName;
	std::string companyName;
	int type;
	std::string remark;
	int status;
	int dflag;
	int createBy;
	std::string createRealName;
	double createTime;
	std::string updateBy;
	std::string updateRealName;
	double updateTime;
	int roleId;
	int roleCode;
	std::string roleName;
	int groupId ;
	std::string groupName;
	bool isShopAdmin;
	int waitNo;
	STCsUser stCsUser;
	STUser() {
		id = 0;
		shopId = 0;
		type =0;
		status = 0;
		dflag = 0;
		createBy = 0;
		createTime = 0;
		updateTime = 0;
		roleId = 0;
		roleCode = 0;
		groupId = 0;
		waitNo = 0;
	}
};

struct STService {
	std::string token;
	STUser stUser;
};

struct STMenuItem {
	int id;
	int parentId;
	int level;
	std::string code;
	std::string name;
	std::string path;
	std::string href;
	std::string icon;
	int sort;
	std::string description;
	int platform;
        int withDataRule;
	int status;
	int createBy;
};

struct STLoginRsp : public STBase {
	std::string result;
	STService stService;
	std::vector<STMenuItem> vecMenu;
	STLoginRsp() {
		success = false;
	}
};

struct STLiveItem {
	int id;
	std::string name;
};

typedef std::list<STLiveItem> LiveList;

struct STLiveList : public STBase {
	LiveList itemList;
	STLiveList() {
		success = false;
	}
};

struct STBeginRsp : public STBase {
	//result(beginLiveVo, optional)
	int liveId;            //直播id,
	int popularValue;      //人气值
	int roomId;            //房间id,
	int userId;            //主播id
	std::string avatar;    //头像,
	std::string cover;     //直播封面,
	std::string labelName; //类别,
	std::string nickName; //昵称,
	std::string notice;   //公告,
	std::string prompt; //提示信息
	std::string startTime; //直播开始时间;
	std::string title;     //房间标题
	std::string uid;       //UID
	STBeginRsp() { success = false; }
};

// 账户信息
struct STCompteInfo : public STBase {
	// result
	int tnTotalAmount;		// 饭团
	int pnTotalAmount;		// 饭票
	int pnWithdrawAbleAmount;	//
	std::string type;		//
	std::string name;		//
	std::string account;		//
	std::string bankName;		//
	STCompteInfo()
	{
		tnTotalAmount = 0;
		pnTotalAmount = 0;
		pnWithdrawAbleAmount = 0;
	}
};

//结束直播
struct STFinishRsp : public STBase  {
	int visitorNum;
	int income;
	int newAttention;
	bool bResult;
	std::string liveTime;
	STFinishRsp()
	{
		visitorNum = 0;
		income = 0;
		newAttention = 0;
		bResult = false;
	}
};
//拉入黑名单
struct STDoBacklistRsp :public STBase{

};

struct STNoticeRsp : public STBase {
	int id;
	int status;
	int anchorId;
	int shopId;
	std::string remind;
	std::string code;
	std::string title;
	std::string notice;
	std::string cover;
};

struct STPause : public STBase {
	bool bResult;
};

struct STRePause : public STBase {
	bool bResult;
};

struct STRoomItem {
	int id;
	int liveNo;
	int roomId;
	std::string title;
	std::string liveCover;
	int anchorId;
	std::string anchorName;
	double openTime;
	double closeTime;
	int closeUser;
	std::string closeUserName;
	int closeUserType;
	int watchPeople;
	int watchPeopleMax;
	int toGift;
	int liveStatus;
	int delFlag;
	int createBy;
	double createTime;
	int inLiveId;
	STRoomItem() {
		id = 0; liveNo=0; roomId=0; anchorId=0; openTime=0;
		closeTime=0; closeUser=0;closeUserType=0;watchPeople=0;
		watchPeopleMax=0;toGift=0;liveStatus=0;delFlag=0;createBy=0;
		inLiveId=0;createTime=0;
	}
};

struct STRoomList : public STBase{
	int page;
	int size;
	int total;
        int pageNum;
	std::map<int,STRoomItem> roomList;
};

struct STUpdatePassword : public STBase {

};

struct STAnchorPermission : public STBase {
	std::string applyTime;
	std::string cover;
	std::string notice;
	std::string status;
};

struct STRoomInfo : public STNoticeRsp {
};

struct STUserInfoVo {
	std::string id;
	std::string nickName;
	std::string headPortrait;
	std::string sourceUserId;
	int type;
	std::string signature;
	int status;
	std::string appKey;
	std::string remark;
	std::string phone;
	std::string loginIp;
	int device;
	int roomId;
	int liveId;
	int groupId;
};

struct STCahtTokenRsp {
	bool success; //应用返回结果
	std::string appCode;
	std::string appMsg;
	std::string token;
	STUserInfoVo stUserInfo;
};

struct STHeartBeat :public STBase {

};

struct STIMToken :public STBase{
	std::string token;
	STUserInfoVo stUserInfo;
};

// 红包领取详情人员数据
struct STPersonRedDetails {
	std::string avatar;
	int drawAmount;
	std::string drawTime;
	std::string isBestLuck;
	std::string nickname;
	int userId;
};

// 用户基本信息
struct STUserBasicInfo {
	int amount;
	std::string avatar;
	int id;
	bool isMember;
	std::string memberEndTime;
	std::string memberStartTime;
	std::string nickname;
	int nicknameModifyLimit;
	std::string phone;
	std::string userIntro;
	std::string userLevel;
	std::string userNo;

	STUserBasicInfo() {
		amount = 0;
		id = 0;
		nicknameModifyLimit = 0;
	}
};

struct STGift {
	int giftId;             //礼物id
	int giftPrice;           //礼物价格,
	std::string giftEffects;//礼物特效
	std::string giftName;//礼物名称
	std::string giftPic;//静态图片
	std::string giftZip;//礼物特效压缩 为空则代表没有特效图片,
	std::string version;//礼物版本 为空则代表没有特效图片
	STGift() {
		giftId = 0;
		giftPrice = 0;
	}
};
struct STGiftList :public STBase{
	int page;//当前页数
	int pageNum;//总页数
	int size;//每页条数
	int total;//总数
	std::vector<STGift> lists;
};

enum ContentType {
	TYPE_CHAT = 1,//聊天消息
	TYPE_GIFT = 2,//礼物消息
	TYPE_RED_PACKET = 3,//红包
	TYPE_ENTER_ROOM = 4, //进入房间
	TYPE_BAN_POST = 5,//禁言
	TYPE_EXIT_ROOM = 6,//退出房间
	TYPE_ROOM_MANAGE = 7,//设为房管
	TYPE_FOCUS_HOST  = 8,//关注主播
	TYPE_DO_BACKLIST = 9,//拉黑
	TYPE_FORCE_LOG_OUT = 11,//强制下播
};

struct STRedPacket {
	int disType;
	int totalAmount;
	int totalNumber;
	int redType;
	int id;
	int userId;
	STRedPacket()
	{
		disType = 0;
		totalAmount = 0;
		totalNumber = 0;
		redType = 0;
		id = 0;
		userId = 0;
	}
};

struct STSendGift {
	int giftId;
	int giftNum;
	std::string giftName;
	std::string giftEffects;
	std::string userName;
	std::string giftUrl;
	STSendGift()
	{
		giftId = 0;
		giftNum = 1;
	}
};

struct STMessageBody {
	int type;
	int contentType;
	int fromUserType;
	double createTime;
	std::string content;
	std::string fromUserId;
	std::string fromUserName;
	std::string groupId;
	std::string realUserNo;
	std::string fromUserAvatar;
	STSendGift stSendGift;
	STRedPacket stRedPacket;
	STMessageBody()
	{
		type = 0;
		contentType = 0;
		fromUserType = 0;
		createTime = 0;
	}
};

struct STInfoClass {
	std::string classCode;
        int classId;
        std::string className;
        int unreadNum;
	STInfoClass() {
		classId = 0;
		unreadNum = 0;
	}
};

struct STInfoClassList : public STBase {
	int unreadTotalNum;
	std::vector<STInfoClass> classlist;
 };

struct STPageData {
	int classId;
        std::string content;
        int id;
        int readStatus;
        double readTime;
	double sendTime;
        std::string senderName;
        std::string title;

	STPageData() {
		readTime = 0.00;
		sendTime = 0.00;
	}
 };

struct STPageList : public STBase {
	int page;
	int pageNum;
	int size;
	int total;
	std::vector<STPageData> pagelist;
 };

struct STWareHouseData {
	int createBy;
	int delFlag;		//是否删除 0 未删除 1 删除 ,
	std::string fileAddress; 
	int fileType;		//文件类型 10 面具库 20 背景库 ,
	int id;
	int shopId;
	int site;		//位置 11 脸 12 双眼 13 鼻子 14 眼睛 15 嘴巴 16 组合 
	int warehouseNo;
};

struct STDeviceDetail: public STBase {
	int id;
	int shopId;
	int type;
	std::string str;
	int gmtCreated;
	int userCreated;
	/**
	* -1:result为null
	* 0 :等待审核
	* 1 已验证
	* 2 已拒绝
	*/
	int applyStatus;
	std::string checkCode;
	STDeviceDetail() {
		id = 0;
		shopId = 0;
		applyStatus = -1;
	}
};

enum EMothod {
	GET = 0,
	POST = 1,
	DOWNLOAD=2
};

enum BeautyFace_Type {
	BeautyFace_SkinWhite = 0,
	BeautyFace_Moulting,
	BeautyFace_Ruddy,
	BeautyFace_EyesReshape,
	BeautyFace_FaceLift,
	BeautyFace_Blusher,
	BeautyFace_ThinChin
};

enum Filter_Type {
	Filter_Normal = 0,
	Filter_Baixi,
	Filter_Chaobai,
	Filter_Qingyang,
	Filter_Baixue,
	Filter_Chulian,
	Filter_Naiyou,
	Filter_Qingchun,
	Filter_Rouhe,
	Filter_Weixun,
	Filter_Chuxin,
	Filter_Feifan,
	Filter_Huopo,
	Filter_Musi,
	Filter_Qiangwei,
	Filter_Dongren,
	Filter_Landiao,
	Filter_Naihui,
	Filter_Quqi,
	Filter_Rixi,
	Filter_Shensui,
	Filter_Fu,
	Filter_Bai,
	Filter_Fen,
};

enum Mask_Type {
	Mask_Mosaic = 0,

	Mask_Toushi_0,
	Mask_Toushi_1,
	Mask_Toushi_2,

	Mask_Tousha_0,

	Mask_Patch_0,
	Mask_Patch_1,
	Mask_Patch_2,
	Mask_Patch_3,
	Mask_Patch_4,
	Mask_Patch_5,
	Mask_Patch_6,
	Mask_Patch_7,
	Mask_Patch_8,
	Mask_Patch_9,
	
	Mask_Glasses_0,
	Mask_Glasses_1,

	Mask_Nose_0,

	Mask_Combination_0,

	Mask_Mask_0,
	Mask_Mask_1,
	Mask_Mask_2,
	Mask_Mask_3,
	Mask_Mask_4,

	Mask_Animation_0,
	Mask_Animation_1,
	Mask_Animation_2,
	Mask_Animation_3,
	Mask_Animation_4,
	Mask_Animation_5,
	Mask_Animation_6,
	Mask_Animation_7,
};

enum Mask3d_Type {
	Mask3d_Kzyj = 0,
	Mask3d_Ssds,
	Mask3d_Hdj,
	Mask3d_Hlmj,
	Mask3d_Ms,
	Mask3d_Shtmz,
	Mask3d_Tzej,
	Mask3d_Mymz,
	Mask3d_Gdej,
	Mask3d_Hrx,
	Mask3d_Hrt,
	Mask3d_Qqx,
	Mask3d_Hx,
	Mask3d_Lx,
	// 2d
	Mask3d_Mosaic,

	Mask3d_Toushi_0,
	Mask3d_Toushi_1,
	Mask3d_Toushi_2,

	Mask3d_Tousha_0,

	Mask3d_Patch_0,
	Mask3d_Patch_1,
	Mask3d_Patch_2,
	Mask3d_Patch_3,
	Mask3d_Patch_4,
	Mask3d_Patch_5,
	Mask3d_Patch_6,
	Mask3d_Patch_7,
	Mask3d_Patch_8,
	Mask3d_Patch_9,
	
	Mask3d_Glasses_0,
	Mask3d_Glasses_1,

	Mask3d_Nose_0,

	Mask3d_Combination_0,

	Mask3d_Mask3d_0,
	Mask3d_Mask3d_1,
	Mask3d_Mask3d_2,
	Mask3d_Mask3d_3,
	Mask3d_Mask3d_4,

	Mask3d_Animation_0,
	Mask3d_Animation_1,
	Mask3d_Animation_2,
	Mask3d_Animation_3,
	Mask3d_Animation_4,
	Mask3d_Animation_5,
	Mask3d_Animation_6,
	Mask3d_Animation_7,
};
#endif
