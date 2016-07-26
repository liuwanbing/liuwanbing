#pragma once

#ifndef GAMEMAINMANAGE_H_FILE
#define GAMEMAINMANAGE_H_FILE

#include "Afxtempl.h"
#include "MainManage.h"
#include "GameDesk.h"
#include "UserInfo.h"
#include "AFCSocket.h"
#include "AFCDataBase.h"
#include "GameRoomLogonDT.h"
///#include "AFCItemData.h"
#include "PropService.h"

///{{ Added by zxd 20100727  GM管理工具所用头文件
#include "GM_MessageDefine.h"
#include "InterfaceForGM.h"
/// Added by zxd 20100727}}


#include "BankService.h"
//#include "GameUserInfoManage.h"

#include "CmdGameLogic.h"

#include "IAutoAllotDesk.h"
#include <map>

using namespace std;

///类说明
class CGameDesk;
class CBaseMainManage;
class CGameUserInfoManage;

///模式数值定义
#define ACCEPT_THREAD_COUNT				1									///应答线程数目
#define SOCKET_THREAD_COUNT				4									///网络线程数目

///结构说明
struct MSG_GR_SR_MatchUser;
struct DL_GR_I_RegisterLogon;
struct DL_GR_O_RegisterLogon;

///宏定义
#define TIME_SPACE						50L									///游戏 ID 间隔
#define TIME_START_ID					100L								///定时器开始 ID
#define ASS_RECEIVE                     1                                   //自动赠送成功
                                 

/*******************************************************************************************************/

///游戏数据库处理
class AFX_EXT_CLASS CGameDataBaseHandle : public CDataBaseHandle
{
public:
	CGameDataBaseHandle(){m_ErrorSQLCount = 0;};
	///辅助函数
private:
	///数据库处理接口
	virtual UINT HandleDataBase(DataBaseLineHead * pSourceData);
	///处理函数
private:
	///大厅登陆处理
	UINT OnRoomLogon(DataBaseLineHead * pSourceData);
	///用户离开处理
	UINT OnUserLeft(DataBaseLineHead * pSourceData);
	///更新服务器信息
	UINT OnUpdateServerInfo(DataBaseLineHead * pSourceData);
	///记录游戏信息
	UINT OnRecordGameInfo(DataBaseLineHead * pSourceData);
	///检测数据库连接
	UINT OnCheckDataConnect(DataBaseLineHead * pSourceData);
	///检测是否服务到期
	UINT OnTimeServiceControl(DataBaseLineHead * pSourceData);
	///呼叫GM处理
	UINT OnCallGM(DataBaseLineHead * pSourceData);
	///管理权限修改
	UINT OnGamePower(DataBaseLineHead * pSourceData);
	///管理员踢人动作纪录
	UINT OnGMKickUserRecord(DataBaseLineHead * pSourceData);
	///管理员法警告消息纪录
	UINT OnGMWarningUserRecord(DataBaseLineHead * pSourceData);
	///打开钱柜
	UINT OnOpenWallet(DataBaseLineHead *pSourceData, UINT uCode = DTK_GR_DEMAND_MONEY);
	///取出金币
	UINT OnCheckOutMoney(DataBaseLineHead *pSourceData, UINT uCode= DTK_GR_CHECKOUT_MONEY);
	///存入金币
	UINT OnCheckInMoney(DataBaseLineHead *pSourceData, UINT uCode= DTK_GR_CHECKIN_MONEY);
	///转帐
	UINT OnTransferMoney(DataBaseLineHead *pSourceData, UINT uCode= DTK_GR_TRANSFER_MONEY);
	///修改密码
	UINT OnChangePasswd(DataBaseLineHead *pSourceData, UINT uCode= DTK_GR_CHANGE_PASSWD);
	///转帐记录
	UINT OnTransferRecord(DataBaseLineHead *pSourceData, UINT uCode= DTK_GR_TRANSFER_RECORD);
	///更新用户信息
	UINT OnUpdateUserResult(DataBaseLineHead * pSourceData);
	///一次性更新同桌所有用户信息
	UINT OnUpdateAllUserResult(DataBaseLineHead * pSourceData);
	//比赛开始，获取比赛ID，并修改报名表中用户状态
	UINT OnContestBegin(DataBaseLineHead * pSourceData);
	//比赛结束，获取奖励
	UINT OnContestGameOver(DataBaseLineHead * pSourceData);
	///更新用户比赛信息
	UINT OnUpdateContestInfo(DataBaseLineHead * pSourceData);
	//获取赠送的金币值 //2013-01-28 wangzhitu
	void GetWalletMoney(RECEIVEMONEY &strRecMoney,DWORD dwUserID);

	///得到用户比赛信息
	UINT OnGetContestInfo(DataBaseLineHead * pSourceData);
	//玩家报名比赛
	UINT OnContestApply(DataBaseLineHead *pSourceData);
	///即时更新分数金币
	UINT OnModifyUserMoney(DataBaseLineHead * pSourceData);
	///即时修改玩家魅力值
	UINT OnUpdateUserCharm(DataBaseLineHead * pSourceData);
	///平台道俱总数
	///UINT OnGetUserHoldPlatformItemCount(DataBaseLineHead *pSourceData);
	///使用道俱
	UINT OnUserUseOneProp(DataBaseLineHead *pSourceData);
	UINT OnUseSmallBoardcast(DataBaseLineHead * pSourceData);
	///通过按照次数和金额给用户送金币
	UINT OnSendUserMoenyByTimes(DataBaseLineHead *pSourceData);
	///获取购买道俱列表

	///UINT OnGetBuyItemData(DataBaseLineHead *pSourceData);
	///购买道俱
	///UINT OnBuyItem(DataBaseLineHead *pSourceData);

	///UINT BufferIMMessage(DataBaseLineHead *pSourceData);

	///UINT OnFriendManage(DataBaseLineHead *pSourceData);
	///
	///20090203 Kylin 防止更新数据库后，用户登陆失败
	///删除登陆信息
	bool UnRegisterUserLogon(long int dwUserID, bool bAllLogonInfo);
	///清理用户断线
	UINT OnUnRegisterOnLine(DataBaseLineHead * pSourceData);
	///
	
	///wushuqun 2009.5.22
	///修改禁止登录问题，修改插入TWLoginRecord 记录时机
	///执行时机：登录房间后发送完游戏房间列表执行
    UINT OnWriteTWLoginRecord(DataBaseLineHead * pSourceData);
	///wushuqun 2009.6.6
	///即时封桌功能
	UINT OnAloneDeskInTime(DataBaseLineHead * pSourceData);

	///辅助函数

	UINT OnClearNetCutUserOnlineFlag(DataBaseLineHead * pSourceData);	///2009-4-17添加 zxj

	UINT OnReWriteWLoginRecord(DataBaseLineHead * pSourceData);		///2009-4-28添加 zxj

	UINT OnContestUserLeft(DataBaseLineHead * pSourceData);		///2009-4-28添加 zxj
	
	UINT OnContestReturnFee(DataBaseLineHead * pSourceData);//退回报名费

	/// 记录修改房间密码
	UINT OnRoomPWChangeRecord(DataBaseLineHead * pSourceData);

    // PengJiLin, 2010-8-23, 根据用户ID获取昵称
    UINT OnGetNickNameOnID(DataBaseLineHead * pSourceData);

    // PengJiLin, 2010-9-10, 使用踢人卡、防踢卡道具
    UINT OnUseKickProp(DataBaseLineHead * pSourceData, BOOL bIsKickUser);

    // PengJiLin, 2010-9-13, 使用踢人卡踢人功能
    UINT OnNewKickUserProp(DataBaseLineHead * pSourceData);


	UINT OnBankOperateNormal(DataBaseLineHead * pSourceData);

	UINT OnBankOperateTransfer(DataBaseLineHead * pSourceData);

	UINT OnBankOperateDeposit(DataBaseLineHead * pSourceData);

	UINT OnBankGetWallet(DataBaseLineHead * pSourceData);

	UINT OnBankQueryDeposits(DataBaseLineHead * pSourceData);

	UINT OnPropChange(DataBaseLineHead * pSourceData);
	HANDLE sqlSPSetNameEx(HANDLE & hHandle,LPCTSTR szSPName,bool bReturnValue = FALSE);
	int                 m_ErrorSQLCount;

};

/*******************************************************************************************************/

///比赛信息结构
struct MatchInfoStruct
{
	long int									dwUserID;							///用户 ID
	TCHAR										szUserNname[61];					///用户名字
	UINT										uPlayCount;							///限制局数
	long int									dwPlayPoint;						///限制分数
};

///限制聊天信息
struct LimitTalkStruct
{
	long int									dwUserID;							///用户 ID
	long int									dwLimitedTime;						///限制时间
};

/*******************************************************************************************************/

///基础数据管理类
class AFX_EXT_CLASS CGameMainManage : public CBaseMainManage
{
	friend class CGameDesk;

	///变量定义
protected:
	UINT										m_uDeskCount;						///游戏桌数目
	CGameDesk									* * m_pDesk;						///游戏桌指针
	CGameDesk									* m_pDeskArray;						///游戏桌指针	
	///状态变量
protected:
	bool										m_bStopLogon;						///停止登陆

	///比赛信息
private:
	UINT										m_uMatchUserCount;					///比赛数量
	MatchInfoStruct								* m_pMatchInfo;						///比赛信息
	MSG_GR_SR_MatchUser							* m_pMatchUser;						///比赛用户

	///限制聊天信息
private:
	CArray<LimitTalkStruct,LimitTalkStruct>		m_RoomLimited;						///房间限制
	CArray<LimitTalkStruct,LimitTalkStruct>		m_GameLimited;						///游戏限制

	///信息变量
protected:
	TCHAR										m_szMsgRoomLogon[500];				///房间消息
	TCHAR										m_szMsgGameLogon[500];				///游戏消息
	TCHAR                                       m_szVIPPassWord[50];                ///< VIP房间密码
	long                                        m_lVIPID;                           ///< 本房间第一个设置密码的VIP ID
	UINT										m_uVIPIndex;                        ///< 本房间第一个设置密码的VIP的套接字索引
	BOOL										m_bIsVIPLogon;                      ///< 是否设置密码的VIP重返密码房间

public:
	CGameUserInfoManage							m_UserManage;						///用户管理
	BYTE                                        m_GameFinishCount;                  ///游戏结束计数
	list<CGameUserInfo*>						m_pGameUserInfoList;				///<排队机list
	int											m_nRate; ///< 税率的分母.100则表示百分几，1000则千分几。分子是 m_pDataManage-m_InitData.uTax
	bool                                        m_bPresentCoin; ///< 是否赠送金币。20把，每天玩家低于若干值时赠送若干金币。
private:	
	///	CAFCItemData								*m_ItemDate;
	CBankService								*m_pBankService;
	CPropService								*m_PropService;
	

    UINT                m_uQueuerMaxPeople;      // PengJiLin, 2010-5-20, 排队机配置项
	int					m_uQueuerPeople;
	int					m_iQueueTime;
	
	IAutoAllotDesk*		m_pIAutoAllotDesk;	///< 自动撮桌处理对象接口指针，视情况由OnInit创建，如为空，表示不需要撮桌机制{{By zxd on 20101129}}
	BYTE				*m_pQueueUserSitBuffer;	///< 自动撮桌玩家坐下时发送的几个玩家一起坐下的缓冲区，避免每次都new出来

	
	///函数定义
public:
	///构造函数
	CGameMainManage(void);
	///析构函数
	virtual ~CGameMainManage(void);
	CGameMainManage(CGameMainManage&);
	CGameMainManage& operator=(CGameMainManage&); 
	//比赛场初始排名随机
	bool RandRank(int *pData, int iLen);
	///判断是否是排队机房间
	///@return true:是排队机房间；false：不是排队机房间
	bool IsQueueGameRoom();


		/// 创建自动撮桌对象
	virtual IAutoAllotDesk *CreateAutoAllotDesk(int iDeskPeopleCount, int iDeskCount);
		   ///服务接口函数 （主线程调用）
	///排队机添加用户
	virtual int AutoAllotDeskAddUser(CGameUserInfo *pInfo);
	///排队机删除用户
	virtual int AutoAllotDeskDeleteUser(CGameUserInfo *pInfo);
	//删除所有排队机用户
	virtual int AutoAllotDeskDeleteAll();
	void NewQueueGame();
private:
	///数据管理模块初始化
	virtual bool OnInit(ManageInfoStruct * pInitData, KernelInfoStruct * pKernelData);
	///数据管理模块卸载
	virtual bool OnUnInit();
	///数据管理模块启动
	virtual bool OnStart();
	///数据管理模块关闭
	virtual bool OnStop();
	
	///服务扩展接口函数 （本处理线程调用）
private:
	///SOCKET 数据读取
	virtual bool OnSocketRead(NetMessageHead * pNetHead, void * pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID);
	///SOCKET 关闭
	virtual bool OnSocketClose(ULONG uAccessIP, UINT uSocketIndex, long int lConnectTime);
	///数据库处理结果
	virtual bool OnDataBaseResult(DataBaseResultLine * pResultData);
	///定时器消息
	virtual bool OnTimerMessage(UINT uTimerID);
	///网络处理扩展

public:
	/// 转账用户ID比较
	/// @param UserID 玩家自己的ID，UserIDDest目标ID,散户ID NomalIDFrom开始，散户ID NomalIDEnd结束
	/// @return  0不能转账，1 可以转账,扣税。2 可以转账,不扣税.3 可以转账都为非特别ID，检查转账金额
	static int TransferIDCompare(long UserID,long UserIDDest,long NomalIDFrom,long NomalIDEnd);
	/// 获取玩家ID信息，是否散户，特别账号，指派账号
	/// @param UserID 玩家ID,NomalIDFrom散户ID开始，NomalIDEnd散户ID结束
	/// @return 0到8是指派账号后面的0的个数，-1为散户，-2为无效账号，-5为5连号，-6为6连号
	static int GetIDInformation(long UserID,long NomalIDFrom,long NomalIDEnd);
	/// 区取某张游戏桌的指针
	const CGameDesk* GetGameDesk(BYTE bDeskNo);

private:
	//{{ Added by zxd 20100806
	/// GM消息处理模块
	bool OnGMSocketMessage(NetMessageHead * pNetHead, void * pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID);
	// Added by zxd 20100806}}

	///用户登陆处理 
	bool OnUserLogonMessage(NetMessageHead * pNetHead, void * pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID);
	///用户动作处理
	bool OnUserActionMessage(NetMessageHead * pNetHead, void * pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID);
	///用户房间处理
	bool OnRoomMessage(NetMessageHead * pNetHead, void * pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID);
	///管理消息处理
	bool OnManageMessage(NetMessageHead * pNetHead, void * pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID);
	///钱柜划账处理
	bool OnAboutMoney(NetMessageHead * pNetHead, void * pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID);

	/// 打印房间所有玩家的消息
	/// Added By JianGuankun 2011.12.5
	void PrintUsersLog();
	static bool Writeline(CFile* p,CString str);

	///经验盒处理
	bool OnPointMessage(NetMessageHead * pNetHead, void * pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID);
	///新增加功能2008.3.5
	///小喇叭
	///bool OnBroadcastMessage(NetMessageHead *pNetHead, void* pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID);
	///好友
	bool OnFriendManageMsg(NetMessageHead *pNetHead, void* pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID);
	///排队处理
	bool OnQueueMsg(NetMessageHead *pNetHead, void* pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID);
	
	///排队机处理
	void queuerGame();

	///道俱消息2008.3.20

	bool IsContestGameOver();


	///好友管理结果
	bool OnFriendManageDBOperateResult(DataBaseResultLine *pResultData);
	///好友聊天结果
	bool OnBufferIMMessageDBOperateResult(DataBaseResultLine *pResultData);
	///数据库处理扩展

public:
	//租赁时间到期
	bool IsLeaseTimeOver();



private:
	///房间登陆处理
	bool OnRoomLogonResult(DataBaseResultLine * pResultData);
	///检测连接处理
	bool OnCheckConnect(DataBaseResultLine * pResultData);
	///呼叫网管结果处理
	bool OnCallGMResult(DataBaseResultLine * pResultData);
	///房间权限设置结果处理
	bool OnGamePowerSetResult(DataBaseResultLine * pResultData);
	///钱柜
	bool OnAboutMoneyResult(DataBaseResultLine * pResultData, UINT uCode=ASS_GR_OPEN_WALLET);
	///房间－＞钱柜
	bool OnCheckInMoney(DataBaseResultLine * pResultData, UINT uCode=DTK_GR_CHECKIN_MONEY);
	///房间＜－钱柜
	bool OnCheckOutMoney(DataBaseResultLine * pResultData, UINT uCode=DTK_GR_CHECKOUT_MONEY);
	///转帐
	bool OnTransferMoney(DataBaseResultLine * pResultData, UINT uCode=DTK_GR_TRANSFER_MONEY);
	///修改密码结果
	bool OnChangePasswd(DataBaseResultLine * pResultData, UINT uCode=DTK_GR_CHANGE_PASSWD);
	///转帐记录结果
	bool OnTransferRecord(DataBaseResultLine * pResultData, UINT uCode=DTK_GR_TRANSFER_RECORD);
	///赠送金币结果，add by yjj 080703
	bool SendUserMoenyByTimesResult(DataBaseResultLine * pResultData);
	
	///wushuqun 2009.6.5
	///混战房间记录结果处理
	bool OnBattleRoomRecord(DataBaseResultLine * pResultData);
	///

    //根据用户ID获取昵称
    bool OnGetNickNameOnIDResult(DataBaseResultLine * pResultData);

    //踢人卡、防踢卡功能
    bool OnUseKickPropResult(DataBaseResultLine * pResultData, BOOL bIsKickUser);

    //踢人卡踢人功能
    bool OnNewKickUserProp(NetMessageHead *pNetHead, void* pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID);

    //使用踢人卡踢人
    bool OnNewKickUserPropResult(DataBaseResultLine * pResultData);

    //在线时长、局数送金币
    bool OnSendMoneyOnTimesAndCounts(DataBaseResultLine * pResultData);

	//获取比赛当前排名，成绩等
	bool OnGetContestRankResult(DataBaseResultLine * pResultData);

	//比赛开始
	bool OnGetContestBeginResult(DataBaseResultLine *pResultData);

	//用户报名/退赛 比赛房间
	bool OnGetContestApplyResult(DataBaseResultLine *pResultData);

	//获取比赛结束的信息
	bool OnGetContestGameOverResult(DataBaseResultLine *pResultData);

	bool OnPropChangeResult(DataBaseResultLine *  pResultData);
	///内部函数
private:
	///离开桌子函数
	bool UserLeftDesk(CGameUserInfo * pUserInfo);
	///用户是否游戏
	bool IsUserPlayGame(CGameUserInfo * pUserInfo);
	///用户是否游戏
	bool IsUserPlayGameByStation(CGameUserInfo * pUserInfo);
	///清理用户资料
	bool CleanUserInfo(CGameUserInfo * pUserInfo);
	///是否限制聊天
	bool IsLimitedTalk(CGameUserInfo * pUserInfo, bool bRoom);
	///发送房间信息

	/// 发送用户信息
	/// @param pUserInfo用户信息,uSocketIndex套接字索引,pLogonResult登陆信息
	/// @return 成功为true
	bool SendRoomInfo(CGameUserInfo * pUserInfo, UINT uSocketIndex, DWORD dwHandleID,DL_GR_O_LogonResult * pLogonResult);

	///发送比赛信息
	bool SendMatchInfo(UINT uSocketIndex, DWORD dwHandleID);
	///发送房间消息
	bool SendRoomMessage(UINT uSocketIndex, DWORD dwHandleID, TCHAR * szMessage, BYTE bShowStation=SHS_TALK_MESSAGE, BYTE bFontSize=0, BYTE bCloseFace=FALSE);
	///发送游戏消息
	bool SendGameMessage(UINT uSocketIndex, DWORD dwHandleID, TCHAR * szMessage, BYTE bShowStation=SHS_TALK_MESSAGE, BYTE bFontSize=0, BYTE bCloseFace=FALSE);

	///内部函数
private:
	///初始化游戏桌
	bool InitGameDesk(UINT uDeskCount, UINT	uDeskType);///百家乐 UINT uDeskType add by wlr 20090714 
	///获取游戏桌子信息
	virtual CGameDesk * CreateDeskObject(UINT uInitDeskCount, UINT & uDeskClassSize)=0;
public:
	virtual CGameDesk * GetDeskObject(BYTE bIndex);

	HANDLE sqlSPSetNameEx(HANDLE & hHandle,LPCTSTR szSPName,bool bReturnValue = FALSE);

};

typedef CGameUserInfo *PGameUserInfo; 

class compare
{       
public:       
    bool operator ()(PGameUserInfo& t1,  PGameUserInfo& t2) const       
    {       
		return t1->GetWeigth() > t2->GetWeigth();  
    }       
};  


/*******************************************************************************************************/

///游戏数据管理类模板
template <class GameDeskClass, UINT uBasePoint, UINT uLessPointTimes> class CGameMainManageTemplate : public CGameMainManage
{
	///函数定义
public:
	///构造函数
	CGameMainManageTemplate(void) {};
	///析构函数
	virtual ~CGameMainManageTemplate(void) {};

	///重载函数
private:
	///获取信息函数 （必须重载）
	virtual bool PreInitParameter(ManageInfoStruct * pInitData, KernelInfoStruct * pKernelData)
	{
		///设置版本信息
		pKernelData->bMaxVer=GAME_MAX_VER;
		pKernelData->bLessVer=GAME_LESS_VER;

		///设置使用网络
		pKernelData->bStartTCPSocket=TRUE;

		///设置数据库信息
		pKernelData->bLogonDataBase=TRUE;
		pKernelData->bNativeDataBase=TRUE;
		pKernelData->bStartSQLDataBase=TRUE;

		///设置线程数目
		pKernelData->uAcceptThreadCount=ACCEPT_THREAD_COUNT;
		pKernelData->uSocketThreadCount=SOCKET_THREAD_COUNT;

		///设置游戏信息
		pKernelData->uNameID=NAME_ID;
		pKernelData->uDeskPeople=PLAY_COUNT;

		CString _NameID;
		_NameID.Format("%d", pKernelData->uNameID);
		CBcfFile fConfig(CBcfFile::GetAppPath()+"SpecialRule.bcf");
		pKernelData->uMinDeskPeople = pKernelData->uDeskPeople;

		///调节人数
		if ((pInitData->uDeskCount*PLAY_COUNT+50)<pInitData->uMaxPeople)
		{
			pInitData->uMaxPeople=pInitData->uDeskCount*PLAY_COUNT+50;
		}

		///调节表名字
		if (lstrcmp(pInitData->szLockTable,TEXT("N/A"))==0) pInitData->szLockTable[0]=0;
		if (lstrcmp(pInitData->szIPRuleTable,TEXT("N/A"))==0) pInitData->szIPRuleTable[0]=0;
		if (lstrcmp(pInitData->szNameRuleTable,TEXT("N/A"))==0) pInitData->szNameRuleTable[0]=0;
		CString msg;
		msg.Format("%d.dll",NAME_ID);

		if (lstrcmp(pInitData->szGameTable,TEXT("N/A"))==0) lstrcpy(pInitData->szGameTable,msg);

		if ((pInitData->uComType==TY_MATCH_GAME)&&(pInitData->szLockTable[0]==0)) 
		{
			AfxMessageBox("比赛房间，但是没有锁定一个表ComLogonMoney");
			return false;
		}

		///金币场必须记录日志
		if (pInitData->uComType==TY_MONEY_GAME)
		{
			pInitData->dwRoomRule|=GRR_RECORD_GAME;
		}

		///调节基础分
		if (pInitData->uBasePoint==0) pInitData->uBasePoint=uBasePoint;///统一为1
		if ((pInitData->uComType==TY_MATCH_GAME)||(pInitData->uComType==TY_MONEY_GAME))
		{
			if (pInitData->uLessPoint<pInitData->uBasePoint)///*uLessPointTimes)///最小入场分为基础分
				pInitData->uLessPoint=pInitData->uBasePoint;///*uLessPointTimes;
			
            // PengJiLin, 2011-7-15, 因为服务费的设置可以很大，不做上限。
            if(pInitData->uTax<0)
				pInitData->uTax=0;

		}

		return true;
	};

	///获取游戏桌子信息
	virtual CGameDesk * CreateDeskObject(UINT uInitDeskCount, UINT & uDeskClassSize)
	{
		uDeskClassSize=sizeof(GameDeskClass);
		return new GameDeskClass [uInitDeskCount];
	};
};

/*******************************************************************************************************/

///游戏模块类模板
template <class GameDeskClass, UINT uBasePoint, UINT uLessPointTimes> class CGameModuleTemplate : public IModuleManageService
{
	///变量定义
public:
	CGameDataBaseHandle														m_DataBaseHandle;	///数据库处理模块
	CGameMainManageTemplate<GameDeskClass,uBasePoint,uLessPointTimes>		m_GameMainManage;	///游戏数据管理

	///函数定义
public:
	///构造函数
	CGameModuleTemplate(void) {};
	///析构函数
	virtual ~CGameModuleTemplate(void) {};

	///接口函数
public:
	///初始化函数 
	virtual bool InitService(ManageInfoStruct * pInitData)
	{
		KernelInfoStruct KernelData;
		memset(&KernelData,0,sizeof(KernelData));
		return m_GameMainManage.Init(pInitData,&m_DataBaseHandle);
	}
	///卸载函数 
	virtual bool UnInitService() { return m_GameMainManage.UnInit(); }
	///开始函数 
	virtual bool StartService(UINT &errCode) { return m_GameMainManage.Start(); }
	///停止函数 
	virtual bool StoptService() { return m_GameMainManage.Stop(); }
	///删除函数
	virtual bool DeleteService() { delete this; return true; }

};

/*******************************************************************************************************/

#endif