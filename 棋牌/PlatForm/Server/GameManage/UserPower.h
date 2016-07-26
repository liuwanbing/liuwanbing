#ifndef CUSERPOWERH_HEAD_FILE
#define CUSERPOWERH_HEAD_FILE

///普通玩家权限定义
#define PW_NO_ROOM_TALK					0x00000001L				///不允许大厅聊天
#define PW_NO_GAME_TALK					0x00000002L				///不允许游戏聊天
#define PW_NO_SEND_MESSAGE				0x00000004L				///不允许发短信息
#define PW_N0_PLAY_GAME					0x00000008L				///不能进行游戏
#define PW_NO_WATCH_GAME				0x00000010L				///不能旁观游戏
#define PW_CAN_THROW_PASS				0x00000020L				///允许跳过密码
#define PW_CAN_THROW_SET				0x00000040L				///允许跳过限制
#define PW_CAN_SEND_MUCH				0x00000080L				///允许发长文字
#define PW_CAN_SET_FONT					0x00000100L				///允许设置字体
#define PW_CAN_TALK_FAST				0x00000200L				///允许刷屏幕
#define PW_CAN_HIGH_LOOK				0x00000400L				///允许高级观看
#define PW_CAN_FIND_USER				0x00000800L				///允许跨厅查找
#define PW_CAN_WATCH_ANYTIME			0x00001000L				///允许任何旁观

///管理员权限定义
#define PW_CAN_SEND_WARNING				0x00000001L				///允许发警告消息
#define PW_CAN_SEND_SYSTEM				0x00000002L				///允许发系统消息
#define PW_CAN_SEND_GAME				0x00000004L				///允许发游戏信息
#define PW_CAN_LIMIT_OTHER_ROOM_TALK	0x00000008L				///允许禁止其他人大厅聊天
#define PW_CAN_LIMIT_OTHER_GAME_TALK	0x00000010L				///允许禁止其他人游戏聊天
#define PW_CAN_LIMIT_OTHER_SEND			0x00000020L				///允许禁止其他人发短信
#define PW_CAN_LIMIT_OTHER_PLAY			0x00000040L				///允许禁止其他人游戏
#define PW_CAN_LIMIT_OTHER_WATCH		0x00000080L				///允许禁止其他人旁观
#define PW_CAN_SAFE_FINISH				0x00000100L				///允许解除游戏
#define PW_CAN_CUT_USER					0x00000200L				///允许踢玩家下线
#define PW_CAN_LOOK_USER_IP				0x00000400L				///允许查看 IP
#define PW_CAN_LIMIT_IP					0x00000800L				///允许禁止 IP
#define PW_CAN_HIDE_IP					0x00001000L				///允许隐藏 IP
#define PW_CAN_LIMIT_ACCOUN				0x00002000L				///允许禁止帐号
#define PW_CAN_LOOK_POWER				0x00004000L				///允许查询权限
#define PW_CAN_SET_POWER				0x00008000L				///允许设置权限
#define PW_CAN_BIND_GAME				0x00010000L				///允许绑定游戏
#define PW_CAN_BIND_ACCOUN				0x00020000L				///允许绑定帐号
#define PW_CAN_HIDE_ACTION				0x00040000L				///允许隐藏动作
#define PW_CAN_CONTROL_USER				0x00080000L				///允许控制用户
#define PW_CAN_MUCH_CLIENT				0x00100000L				///允许多客户端
#define PW_CAN_SET_SERVER				0x00200000L				///允许设置服务器
#define PW_CAN_LISTEN_MESSAGE			0x00400000L				///允许监听短信息
#define PW_CAN_LISTEN_GAME				0x00800000L				///允许监听游戏聊天
#define PW_CAN_ADD_PIONT				0x0100000L				///允许加减分数
#define PW_CAN_LOGON_LIMIT				0x02000000L				///允许登陆限制房间
#define PW_CAN_ALL_POWER				0x04000000L				///允许所有权限

///用户权限
class  CUserPower
{
	///查询普通玩家权限查询
public:
	CUserPower(){};
	///查询是否允许大厅聊天
	static bool CanRoomTalk(long int dwGamePower) { return ((dwGamePower&PW_NO_ROOM_TALK)==0L); }
	///查询是否允许游戏聊天
	static bool CanGameTalk(long int dwGamePower) { return ((dwGamePower&PW_NO_GAME_TALK)==0L); }
	///查询是否允许发短信息
	static bool CanSendMessage(long int dwGamePower) { return ((dwGamePower&PW_NO_SEND_MESSAGE)==0L); }
	///查询是否允许进行游戏
	static bool CanPlayGame(long int dwGamePower) { return ((dwGamePower&PW_N0_PLAY_GAME)==0L); }
	///查询是否允许旁观游戏
	static bool CanWatchGame(long int dwGamePower) { return ((dwGamePower&PW_NO_WATCH_GAME)==0L); }
	///查询是否允许跳过密码
	static bool CanThrowPass(long int dwGamePower) { return ((dwGamePower&PW_CAN_THROW_PASS)!=0L); }
	///查询是否允许跳过限制
	static bool CanThrowSet(long int dwGamePower) { return ((dwGamePower&PW_CAN_THROW_SET)!=0L); }
	///查询是否允许发长文字
	static bool CanSendMuch(long int dwGamePower) { return ((dwGamePower&PW_CAN_SEND_MUCH)!=0); }
	///查询是否允许设置字体
	static bool CanSetFont(long int dwGamePower) { return ((dwGamePower&PW_CAN_SET_FONT)!=0); }
	///查询是否允许刷屏幕
	static bool CanTalkFast(long int dwGamePower) { return ((dwGamePower&PW_CAN_TALK_FAST)!=0); }
	///查询是否允许高级观看
	static bool CanHighLook(long int dwGamePower) { return ((dwGamePower&PW_CAN_HIGH_LOOK)!=0); }
	///查询允许跨厅查找
	static bool CanFindUser(long int dwGamePower) { return ((dwGamePower&PW_CAN_FIND_USER)!=0); }
	///查询允许任何旁观
	static bool CanWatchAnyTime(long int dwGamePower) { return ((dwGamePower&PW_CAN_WATCH_ANYTIME)!=0); }

	///查询查询管理员权限查询
public:
	///查询是否允许发警告消息
	static bool CanSendWarning(long int dwMasterPower) { return ((dwMasterPower&PW_CAN_SEND_WARNING)!=0L); }
	///查询是否允许发系统消息
	static bool CanSendSystem(long int dwMasterPower) { return ((dwMasterPower&PW_CAN_SEND_SYSTEM)!=0); }
	///查询是否允许发游戏信息
	static bool CanSendGame(long int dwMasterPower) { return ((dwMasterPower&PW_CAN_SEND_GAME)!=0); }
	///查询是否允许禁止其他人大厅聊天
	static bool CanLimiteRoomTalk(long int dwMasterPower) { return ((dwMasterPower&PW_CAN_LIMIT_OTHER_ROOM_TALK)!=0L); }
	///查询是否允许禁止其他人游戏聊天
	static bool CanLimiteGameTalk(long int dwMasterPower) { return ((dwMasterPower&PW_CAN_LIMIT_OTHER_GAME_TALK)!=0L); }
	///查询是否允许禁止其他人发短信
	static bool CanLimiteOtherSend(long int dwMasterPower) { return ((dwMasterPower&PW_CAN_LIMIT_OTHER_SEND)!=0L); }
	///查询是否允许禁止其他人游戏
	static bool CanLimiteOtherPlay(long int dwMasterPower) { return ((dwMasterPower&PW_CAN_LIMIT_OTHER_PLAY)!=0L); }
	///查询是否允许禁止其他人旁观
	static bool CanLimiteOtherWatch(long int dwMasterPower) { return ((dwMasterPower&PW_CAN_LIMIT_OTHER_WATCH)!=0L); }
	///查询是否允许解除游戏
	static bool CanSafeFinish(long int dwMasterPower) { return ((dwMasterPower&PW_CAN_SAFE_FINISH)!=0L); }
	///查询是否允许踢玩家下线
	static bool CanCutUser(long int dwMasterPower) { return ((dwMasterPower&PW_CAN_CUT_USER)!=0L); }
	///查询是否允许查看 IP
	static bool CanLookIP(long int dwMasterPower) { return ((dwMasterPower&PW_CAN_LOOK_USER_IP)!=0); }
	///查询是否允许禁止 IP
	static bool CanLimiteIP(long int dwMasterPower) { return ((dwMasterPower&PW_CAN_LIMIT_IP)!=0L); }
	///查询是否允许允许隐藏 IP
	static bool CanHideIP(long int dwMasterPower) { return ((dwMasterPower&PW_CAN_HIDE_IP)!=0L); }
	///查询是否允许禁止帐号
	static bool CanLimiteAccoun(long int dwMasterPower) { return ((dwMasterPower&PW_CAN_LIMIT_ACCOUN)!=0L); }
	///查询是否允许查询权限
	static bool CanLookPower(long int dwMasterPower) { return ((dwMasterPower&PW_CAN_LOOK_POWER)!=0L); }
	///查询是否允许设置权限
	static bool CanSetPower(long int dwMasterPower) { return ((dwMasterPower&PW_CAN_SET_POWER)!=0L); }
	///查询是否允许绑定游戏
	static bool CanBindGame(long int dwMasterPower) { return ((dwMasterPower&PW_CAN_BIND_GAME)!=0L); }
	///查询是否允许绑定帐号
	static bool CanBindAccoun(long int dwMasterPower) { return ((dwMasterPower&PW_CAN_BIND_ACCOUN)!=0L); }
	///查询是否允许隐藏动作
	static bool CanHideAction(long int dwMasterPower) { return ((dwMasterPower&PW_CAN_HIDE_ACTION)!=0L); }
	///查询是否允许控制用户
	static bool CanControlUser(long int dwMasterPower) { return ((dwMasterPower&PW_CAN_CONTROL_USER)!=0L); }
	///查询是否允许多客户端
	static bool CanMuchClient(long int dwMasterPower) { return ((dwMasterPower&PW_CAN_MUCH_CLIENT)!=0L); }
	///查询是否允许设置服务器
	static bool CanSetServer(long int dwMasterPower) { return ((dwMasterPower&PW_CAN_SET_SERVER)!=0L); }
	///查询允许监听短信息
	static bool CanListenMessage(long int dwMasterPower) { return ((dwMasterPower&PW_CAN_LISTEN_MESSAGE)!=0L); }
	///查询允许监听游戏聊天
	static bool CanListenGame(long int dwMasterPower) { return ((dwMasterPower&PW_CAN_LISTEN_GAME)!=0L); }
	///查询是否允许加减分数
	static bool CanAddPoint(long int dwMasterPower) { return ((dwMasterPower&PW_CAN_ADD_PIONT)!=0L); }
	///查询允许登陆限制房间
	static bool CanLogonLimit(long int dwMasterPower) { return ((dwMasterPower&PW_CAN_LOGON_LIMIT)!=0L); }
	///查询是否允许所有权限
	static bool CanAllPower(long int dwMasterPower) { return ((dwMasterPower&PW_CAN_ALL_POWER)!=0L); }

	///设置普通玩家权限设置
public:
	///设置是否允许大厅聊天
	static void SetCanRoomTalk(long int & dwGamePower, BYTE bEnable) { if (bEnable) dwGamePower&=~PW_NO_ROOM_TALK; else dwGamePower|=PW_NO_ROOM_TALK; }
	///设置是否允许游戏聊天
	static void SetCanGameTalk(long int & dwGamePower, BYTE bEnable) { if (bEnable) dwGamePower&=~PW_NO_GAME_TALK; else dwGamePower|=PW_NO_GAME_TALK; }
	///设置是否允许发短信息
	static void SetCanSendMessage(long int & dwGamePower, BYTE bEnable) { if (bEnable) dwGamePower&=~PW_NO_SEND_MESSAGE; else dwGamePower|=PW_NO_SEND_MESSAGE; }
	///设置是否允许进行游戏
	static void SetCanPlayGame(long int & dwGamePower, BYTE bEnable) { if (bEnable) dwGamePower&=~PW_N0_PLAY_GAME; else dwGamePower|=PW_N0_PLAY_GAME; }
	///设置是否允许旁观游戏
	static void SetCanWatchGame(long int & dwGamePower, BYTE bEnable) { if (bEnable) dwGamePower&=~PW_NO_WATCH_GAME; else dwGamePower|=PW_NO_WATCH_GAME; }
	///设置是否允许跳过密码
	static void SetCanThrowPass(long int & dwGamePower, BYTE bEnable) { if (bEnable) dwGamePower|=PW_CAN_THROW_PASS; else dwGamePower&=~PW_CAN_THROW_PASS; }
	///设置是否允许跳过限制
	static void SetCanThrowSet(long int & dwGamePower, BYTE bEnable) {  if (bEnable) dwGamePower|=PW_CAN_THROW_SET; else dwGamePower&=~PW_CAN_THROW_SET; }
	///设置是否允许发长文字
	static void SetCanSendMuch(long int & dwGamePower, BYTE bEnable) { if (bEnable) dwGamePower|=PW_CAN_SEND_MUCH; else dwGamePower&=~PW_CAN_SEND_MUCH; }
	///设置是否允许设置字体
	static void SetCanSetFont(long int & dwGamePower, BYTE bEnable) { if (bEnable) dwGamePower|=PW_CAN_SET_FONT; else dwGamePower&=~PW_CAN_SET_FONT; }
	///设置是否允许刷屏幕
	static void SetCanTalkFast(long int & dwGamePower, BYTE bEnable) { if (bEnable) dwGamePower|=PW_CAN_TALK_FAST; else dwGamePower&=~PW_CAN_TALK_FAST; }
	///设置是否允许高级观看
	static void SetCanHighLook(long int & dwGamePower, BYTE bEnable) { if (bEnable) dwGamePower|=PW_CAN_HIGH_LOOK; else dwGamePower&=~PW_CAN_HIGH_LOOK; }
	///设置是否允许跨厅查找
	static void SetCanFindUser(long int & dwGamePower, BYTE bEnable) { if (bEnable) dwGamePower|=PW_CAN_FIND_USER; else dwGamePower&=~PW_CAN_FIND_USER; }
	///设置是否允许任何旁观
	static void SetCanWatchAnyTime(long int & dwGamePower, BYTE bEnable) { if (bEnable) dwGamePower|=PW_CAN_WATCH_ANYTIME; else dwGamePower&=~PW_CAN_WATCH_ANYTIME; }

	///设置管理员权限设置
public:
	///设置是否允许发警告消息
	static void SetCanSendWarning(long int & dwMasterPower, BYTE bEnable) { if (bEnable) dwMasterPower|=PW_CAN_SEND_WARNING; else dwMasterPower&=~PW_CAN_SEND_WARNING; }
	///设置是否允许发系统消息
	static void SetCanSendSystem(long int & dwMasterPower, BYTE bEnable) { if (bEnable) dwMasterPower|=PW_CAN_SEND_SYSTEM; else dwMasterPower&=~PW_CAN_SEND_SYSTEM; }
	///设置是否允许发游戏信息
	static void SetCanSendGame(long int & dwMasterPower, BYTE bEnable) { if (bEnable) dwMasterPower|=PW_CAN_SEND_GAME; else dwMasterPower&=~PW_CAN_SEND_GAME; }
	///是否允许禁止其他人大厅聊天
	static void SetCanLimiteRoomTalk(long int & dwMasterPower, BYTE bEnable) { if (bEnable) dwMasterPower|=PW_CAN_LIMIT_OTHER_ROOM_TALK; else dwMasterPower&=~PW_CAN_LIMIT_OTHER_ROOM_TALK; }
	///设置是否允许禁止其他人游戏聊天
	static void SetCanLimiteGameTalk(long int & dwMasterPower, BYTE bEnable) { if (bEnable) dwMasterPower|=PW_CAN_LIMIT_OTHER_GAME_TALK; else dwMasterPower&=~PW_CAN_LIMIT_OTHER_GAME_TALK; }
	///设置是否允许禁止其他人发短信
	static void SetCanLimiteOtherSend(long int & dwMasterPower, BYTE bEnable) { if (bEnable) dwMasterPower|=PW_CAN_LIMIT_OTHER_SEND; else dwMasterPower&=~PW_CAN_LIMIT_OTHER_SEND; }
	///设置是否允许禁止其他人游戏
	static void SetCanLimiteOtherPlay(long int & dwMasterPower, BYTE bEnable) { if (bEnable) dwMasterPower|=PW_CAN_LIMIT_OTHER_PLAY; else dwMasterPower&=~PW_CAN_LIMIT_OTHER_PLAY; }
	///设置是否允许禁止其他人旁观
	static void SetCanLimiteOtherWatch(long int & dwMasterPower, BYTE bEnable) { if (bEnable) dwMasterPower|=PW_CAN_LIMIT_OTHER_WATCH; else dwMasterPower&=~PW_CAN_LIMIT_OTHER_WATCH; }
	///设置是否允许解除游戏
	static void SetCanSafeFinish(long int & dwMasterPower, BYTE bEnable) { if (bEnable) dwMasterPower|=PW_CAN_SAFE_FINISH; else dwMasterPower&=~PW_CAN_SAFE_FINISH; }
	///设置是否允许踢玩家下线
	static void SetCanCutUser(long int & dwMasterPower, BYTE bEnable) { if (bEnable) dwMasterPower|=PW_CAN_CUT_USER; else dwMasterPower&=~PW_CAN_CUT_USER; }
	///设置是否允许查看 IP
	static void SetCanLookIP(long int & dwMasterPower, BYTE bEnable) { if (bEnable) dwMasterPower|=PW_CAN_LOOK_USER_IP; else dwMasterPower&=~PW_CAN_LOOK_USER_IP; }
	///设置是否允许禁止 IP
	static void SetCanLimiteIP(long int & dwMasterPower, BYTE bEnable) { if (bEnable) dwMasterPower|=PW_CAN_LIMIT_IP; else dwMasterPower&=~PW_CAN_LIMIT_IP; }
	///设置是否允许允许隐藏 IP
	static void SetCanHideIP(long int & dwMasterPower, BYTE bEnable) { if (bEnable) dwMasterPower|=PW_CAN_HIDE_IP; else dwMasterPower&=~PW_CAN_HIDE_IP; }
	///设置是否允许禁止帐号
	static void SetCanLimiteAccoun(long int & dwMasterPower, BYTE bEnable) { if (bEnable) dwMasterPower|=PW_CAN_LIMIT_ACCOUN; else dwMasterPower&=~PW_CAN_LIMIT_ACCOUN; }
	///设置是否允许查询权限
	static void SetCanLookPower(long int & dwMasterPower, BYTE bEnable) { if (bEnable) dwMasterPower|=PW_CAN_LOOK_POWER; else dwMasterPower&=~PW_CAN_LOOK_POWER; }
	///设置是否允许设置权限
	static void SetCanSetPower(long int & dwMasterPower, BYTE bEnable) { if (bEnable) dwMasterPower|=PW_CAN_SET_POWER; else dwMasterPower&=~PW_CAN_SET_POWER; }
	///设置是否允许绑定游戏
	static void SetCanBindGame(long int & dwMasterPower, BYTE bEnable) { if (bEnable) dwMasterPower|=PW_CAN_BIND_GAME; else dwMasterPower&=~PW_CAN_BIND_GAME; }
	///设置是否允许绑定帐号
	static void SetCanBindAccoun(long int & dwMasterPower, BYTE bEnable) { if (bEnable) dwMasterPower|=PW_CAN_BIND_ACCOUN; else dwMasterPower&=~PW_CAN_BIND_ACCOUN; }
	///设置是否允许隐藏动作
	static void SetCanHideAction(long int & dwMasterPower, BYTE bEnable) { if (bEnable) dwMasterPower|=PW_CAN_HIDE_ACTION; else dwMasterPower&=~PW_CAN_HIDE_ACTION; }
	///设置是否允许控制用户
	static void SetCanControlUser(long int & dwMasterPower, BYTE bEnable) { if (bEnable) dwMasterPower|=PW_CAN_CONTROL_USER; else dwMasterPower&=~PW_CAN_CONTROL_USER; }
	///设置是否允许多客户端
	static void SetCanMuchClient(long int & dwMasterPower, BYTE bEnable) { if (bEnable) dwMasterPower|=PW_CAN_MUCH_CLIENT; else dwMasterPower&=~PW_CAN_MUCH_CLIENT; }
	///设置是否允许设置服务器
	static void SetCanSetServer(long int & dwMasterPower, BYTE bEnable) { if (bEnable) dwMasterPower|=PW_CAN_SET_SERVER; else dwMasterPower&=~PW_CAN_SET_SERVER; }
	///设置是否允许监听短信息
	static void SetCanListenMessage(long int & dwMasterPower, BYTE bEnable) { if (bEnable) dwMasterPower|=PW_CAN_LISTEN_MESSAGE; else dwMasterPower&=~PW_CAN_LISTEN_MESSAGE; }
	///设置是否允许监听游戏聊天
	static void SetCanListenGame(long int & dwMasterPower, BYTE bEnable) { if (bEnable) dwMasterPower|=PW_CAN_LISTEN_GAME; else dwMasterPower&=~PW_CAN_LISTEN_GAME; }
	///设置是否允许加减分数
	static void SetCanAddPoint(long int & dwMasterPower, BYTE bEnable) { if (bEnable) dwMasterPower|=PW_CAN_ADD_PIONT; else dwMasterPower&=~PW_CAN_ADD_PIONT; }
	///设置是否允许登陆限制房间
	static void SetLogonLimit(long int & dwMasterPower, BYTE bEnable) { if (bEnable) dwMasterPower|=PW_CAN_LOGON_LIMIT; else dwMasterPower&=~PW_CAN_LOGON_LIMIT; }
	///设置是否允许所有权限
	static void SetCanAllPower(long int & dwMasterPower, BYTE bEnable) { if (bEnable) dwMasterPower|=PW_CAN_ALL_POWER; else dwMasterPower&=~PW_CAN_ALL_POWER; }
};

#endif