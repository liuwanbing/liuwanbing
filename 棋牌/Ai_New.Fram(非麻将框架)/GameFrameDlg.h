#pragma once

#ifndef CGAMEFRAMEDLG_HEAD_FILE
#define CGAMEFRAMEDLG_HEAD_FILE


/*******************************************************************************************************/

//控制消息定义
#define CM_USER_STATE					1						//状态改变
#define IDM_GETPOINT						WM_USER + 430				//积分
#define IDM_GET_ROOMNAME					WM_USER+435					//得到房间名称

//动作代码
#define ACT_USER_UP						1						//用户起来
#define ACT_WATCH_UP					2						//旁观起来
#define ACT_USER_SIT					3						//用户坐下
#define ACT_WATCH_SIT					4						//旁观坐下
#define ACT_USER_CUT					5						//用户断线
#define ACT_USER_AGREE					6						//用户同意
#define ACT_GAME_BEGIN					7						//游戏开始
#define ACT_GAME_END					8						//游戏结束
#define ACT_USER_POINT					9						//用户经验值

//用户状态改变
struct CM_UserState_Change
{
	BYTE								bActionCode;			//动作代码
	BYTE								bDeskStation;			//桌子位置
	UserItemStruct						* pUserItem;			//用户指针
};

/*******************************************************************************************************/

//游戏框架接口
interface IFrameInterface
{
	//函数定义
public:
	//初始化（首次生成使用）
	virtual bool InitFrame(GameInfoStruct * pGameInfo, CWnd * pGameRoom)=0;
	virtual ~IFrameInterface() {}
	//建立窗口
	virtual bool AFCCreateFrame()=0;
	//关闭窗口
	virtual bool AFCCloseFrame()=0;
	//控制消息 
	virtual UINT OnControlMessage(UINT uControlCode, void * pControlData, UINT uDataSize)=0;
	//网络读取消息
	virtual bool OnSocketReadEvent(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)=0;
	//游戏计时器
	virtual bool OnGameTimer(BYTE bDeskStation, UINT uTimeID, UINT uTimeCount) = 0;
};

/*******************************************************************************************************/



/*******************************************************************************************************/

//查找用户接口
interface IFindUserInterface
{
	//函数定义
public:
	//查找用户
	virtual UserItemStruct * FindOnLineUser(long int dwUserID)=0;
	//查找用户
	virtual UserItemStruct * FindOnLineUser(int dwAccID)=0;
	//查找用户
	virtual INT_PTR FindOnLineUser(const TCHAR * szUserName, CPtrArray & ResultPtrArray)=0;
	//查找用户
	virtual INT_PTR FindOnLineUser(BYTE bDeskIndex, CPtrArray & PlayPtrArray, CPtrArray & WatchPtrArray)=0;
};

/*******************************************************************************************************/

//游戏基础框架 
class EXT_CLASS CLoveSendClass/*C-GameFrameDlg*/ : public CDialog, public IFrameInterface
{
	//变量定义
protected:
	CWnd								* m_pGameRoom;					//房间窗口

	GameInfoStruct						* m_pGameInfo;					//游戏信息指针
	UserItemStruct						* m_pUserInfo[MAX_PEOPLE];		//游戏玩家信息
	UINT								m_uTimeID;						//定时器 ID
	UINT								m_uTimeCount;					//定时器计数
	BYTE								m_bTimeStation;					//定期器位置
	TCHAR								m_ErrorMsg[MAX_PATH];			//错误日志
	int                                 m_nPowerOfGold;                ///金币倍率
	//函数定义
protected:
	//构造函数
	CLoveSendClass(UINT uTemplate);
	//析构函数
	virtual ~CLoveSendClass();
	enum			{ IDD = IDD_GAME_FRAME };		     	//ID 号码
	//重载函数
protected:
	//初始化函数
	virtual BOOL OnInitDialog();
	//倥件绑定函数
	virtual void DoDataExchange(CDataExchange * pDX);
protected:
	//游戏消息处理函数
	virtual bool HandleGameMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//设置游戏状态
	virtual bool SetGameStation(void * pStationData, UINT uDataSize)=0;
	//重新设置界面
	virtual void ResetGameFrame();
	//重新设置游戏数据
	virtual void ResetGameStation(int iGameStation=1){};

	//游戏控制函数
protected:
	//同意开始游戏 
	virtual bool OnControlHitBegin()=0;

	//功能函数
public:
	void SetInifile(LPCTSTR path);
	//获取自己信息
	UserInfoStruct * GetMeUserInfo() { return &m_pGameInfo->pMeUserInfo->GameUserInfo; };
	//设置状态参数
	void SetStationParameter(BYTE bGameStation) { m_pGameInfo->bGameStation=bGameStation; };
	//获取状态参数
	BYTE GetStationParameter() { return m_pGameInfo->bGameStation; };
	//发送网络消息函数
	int SendGameData(BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode);
	//发送网络消息函数
	int SendGameData(void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode);

	//框架函数
	virtual bool OnFrameMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);

	//接口函数（不需要重载）
private:
	//初始化
	virtual bool InitFrame(GameInfoStruct * pGameInfo, CWnd * pGameRoom);
	//建立窗口
	virtual bool AFCCreateFrame();
public:
	//关闭窗口
	virtual bool AFCCloseFrame();
private:
	//控制消息 
	virtual UINT OnControlMessage(UINT uControlCode, void * pControlData, UINT uDataSize);
	//网络读取消息
	virtual bool OnSocketReadEvent(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
public:
	//计时器
	virtual bool SetGameTimer(BYTE bDeskStation,UINT uTimeCount,UINT uTimeID);
	//杀计时器
	virtual bool KillGameTimer(UINT uTimeID);
	//桌号换算到视图位置
	virtual BYTE ViewStation(BYTE bDeskStation);
public:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);

};
/*******************************************************************************************************/

#endif
