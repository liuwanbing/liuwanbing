#ifndef USERINFO_H_2003_7
#define USERINFO_H_2003_7

#include "AFCPtrArray.h"
#include "../common/ComStruct.h"
///#include "itemdatemessage.h"
#include "../common/GameRoomMessage.h"
///宏定义
#define ERROR_SOCKET_INDEX					9999L					///无效索引

/*******************************************************************************************************/

///绑定分数结构
struct UserChangePoint
{
	UINT						uWinCount;							///胜利
	UINT						uLostCount;							///输局
	UINT						uCutCount;							///强退
	UINT						uMidCount;							///和局
	int							dwPoint;							///分数
	int							dwMoney;							///金币
	int							dwTaxCom;							///税收
};

/*******************************************************************************************************/

///用户资料类
class AFX_EXT_CLASS CGameUserInfo
{
	///变量定义
public:
	bool						m_bAccess;							///可以访问
	char						m_szMD5Pass[50];					///MD5 密码
	int							m_dwScrPoint;						///用户原分
	long int					m_dwAccessLogID;					///记录标识
	long int					m_dwLogonTime;						///登陆时间
	long int					m_dwGamePower;						///用户权限
	long int					m_dwMasterPower;					///管理权限
	long int					m_dwPlayCount;						///游戏时间
	UINT						m_uSocketIndex;						///网络索引
	DWORD						m_dwHandleID;						///处理标识
	UserRuleStruct				m_Rule;								///规则设置
	UserInfoStruct				m_UserData;							///用户资料
	UserChangePoint				m_ChangePoint;						///分数变化


	int							m_SelectWeight; 
///	tagItemData					m_HoldItemData;						/// 持有平台物品数据；

	///函数定义 
public:
	///构造函数 
	CGameUserInfo(void);
	///析构函数
	~CGameUserInfo(void);

	///功能函数
public:
	///判断是否存在用户信息
	bool IsAccess() { return m_bAccess; }
	///设置用户密码
	bool SetMD5Pass(char szMD5Pass[50]);
	///设置用户原分数
	bool SetUserScrPoint(int dwScrPoint);
	///设置用户被送的金币
	bool SetUserSendedMoeny(int dwSendMoney);
	///设置信息
	bool SetUserData(UserInfoStruct & UserInfo, UINT uSocketIndex, DWORD dwHandleID, long int dwGamePower, long int dwMasterPower, long int dwAccessLogID=0);
	///设置状态
	bool SetUserState(BYTE bUserState, BYTE bDeskNO, BYTE bDeskStation, bool bSetDeskStation);
	///更改用户分数
	bool ChangePoint(__int64 dwPoint, __int64 dwTaxCom, bool bWin, bool bLost, bool bMid, bool bCut, long int dwPlayCount, __int64 &dwMoney);
	///设置规则
	bool SetRule(UserRuleStruct * pRule);
	///对比规则是否适合
	bool IsFixRule(UserRuleStruct * pRule, UserInfoStruct * pCheckUserInfo, BYTE & bErrorResult);
	///清理用户信息
	bool CleanData();
	///获取用户总局数
	UINT GetUserAllCount();
	///获取用户信息
	UserInfoStruct * GetUserData(UserInfoStruct * pUserInfoStruct);
	///获取分数更改
	UserChangePoint * GetChangePointInfo(UserChangePoint * pChangePoint);

	//增加被选中的权值
	int GetWeigth();


	///辅助函数
public:
	///获取处理标识
	DWORD GetHandleID() { return m_dwHandleID; }
	///获取网络索引
	UINT GetSocketIndex() { return m_uSocketIndex; }
	///获取登陆时间
	long int GetLogonTime() { return (long int)time(NULL)-m_dwLogonTime; }
};

/*******************************************************************************************************/

///用户管理类
class AFX_EXT_CLASS CGameUserInfoManage
{
	///变量定义
public:
	UINT										m_uOnLineCount;					///在线用户数量
	CAFCPtrArray<CGameUserInfo>					m_OnLineUserInfo;				///在线用户信息
	CAFCPtrArray<CGameUserInfo>					m_NetCutUserInfo;				///断线用户信息

	///函数定义
public:
	///构造函数
	CGameUserInfoManage(void);
	///析构函数
	~CGameUserInfoManage(void);

	///功能函数
public:
	///初始化函数
	bool Init(UINT uMaxOnLineCount, UINT uMaxNetCutCount,UINT uVipReseverCount);
	///取消初始化
	bool UnInit();
	///激活用户
	CGameUserInfo * ActiveUser(UserInfoStruct * pUserInfo, UINT uSocketIndex, DWORD dwHandleID, long int dwGamePower, long int dwMasterPower, long int lAccessLogID);
	///释放用户
	bool FreeUser(CGameUserInfo * pGameUserInfo, bool bOnLinePeople);
	///注册用户断线
	CGameUserInfo * RegNetCutUser(CGameUserInfo * pNetCutUserInfo);
	///获取用户
	CGameUserInfo * GetOnLineUserInfo(UINT uIndex);
	///获取在线用户 
	UINT FillOnLineUserInfo(char * pOnLineBuffer, UINT uBufferSize, UINT & uIndex, UINT & uFillCount, bool & bFinish);
	///获取断线用户 
	UINT FillNetCutUserInfo(char * pNetCutBuffer, UINT uBufferSize, UINT & uBeginPos, bool & bFinish);
	///查找用户
	CGameUserInfo * FindOnLineUser(long int dwUserID);
	///查找用户
	CGameUserInfo * FindOnLineUser(char * szName);
	///查找用户
	CGameUserInfo * FindNetCutUser(long int dwUserID);
	///查找用户
	CGameUserInfo * FindNetCutUser(char * szName);
	///获取在线人数
	UINT GetOnLineUserCount() { return m_uOnLineCount; }
	///获取断线人数
	UINT GetNetCutUserCount() { return m_NetCutUserInfo.GetActiveCount(); }
	///获取全部人数
	UINT GetGameRoomCount() { return m_uOnLineCount+m_NetCutUserInfo.GetActiveCount(); }
};

/*******************************************************************************************************/

#endif