#pragma once

#include "Platform/AFCDevHead.h"
#include "Platform/explorer1.h"  //IE 浏览器，Fred Huang 2008-06-02
#include "Platform/UserListDlg.h"
#include "Platform/DlgLeftShow.h"
#include "Platform/PlaySound.h"
#include "Platform/PropSelUser.h"
#include "Platform/AFCDialog.h"
#include "../common/centerserverhead.h"
#include "Platform/PlayView.h"
#include "BoardCast.h"			///< 大喇叭小喇叭对话框
#include "PropPannel.h"			///< 购买道具对话框
#include "BankDlg.h"			///< 银行对话框
#include "checkPSW.h"			///< 输入密码对话框
#include "PBankDlg.h"
#include "UserDetailsDlg.h"   ///< 用户详细资料面板
#include "SkinMgr.h"
#include "..\common\IGameFrame.h"
#include "..\common\ivideointerface.h"

class CPBank4GameWnd;
class CRightFrame;				///游戏右边框
class CGameTrottingWnd;

/// CLoveSendClassInExe 对话框

/// 游戏框架接口
interface IFrameInterface
{
	//函数定义
public:
	//初始化（首次生成使用）
	virtual bool InitFrame(GameInfoStructInExe * pGameInfo, CWnd * pGameRoom)=0;
	//建立窗口
	virtual bool CreateFrame()=0;
	//关闭窗口
	virtual bool AFCCloseFrame(bool bNotifyServer=true)=0;
	//删除对象
	virtual void DeleteFrame()=0;
	////显示窗口
	//virtual void AFCShowFrame(int nShow)=0;
	////发送windows消息
	//virtual LRESULT SendWindowMessage(UINT msg,WPARAM wParam,LPARAM lParam)=0;
	//控制消息 
	virtual UINT OnControlMessage(UINT uControlCode, void * pControlData, UINT uDataSize)=0;
	//网络读取消息
	virtual bool OnSocketReadEvent(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize)=0;
	//发送游戏房间消息
	virtual void ShowMsgInGame(TCHAR * szMsg, BYTE bNewOrSys=0,UINT uSize=0, TCHAR * szFontName=NULL,TCHAR *Title=NULL)=0;
	//发送嘟嘟消息
	virtual void ShowMsgDudu(TCHAR * szSendName, TCHAR * szTargetName,TCHAR * szCharString, COLORREF crTextColor, bool bShowTime, UINT uSize=0, TCHAR * szFontName=NULL,int iDuduType=0)=0;
	//同桌用户清单
	virtual void OnUserListAtDesk(void *pIPCBuffer)=0;
	//重新设置用户数据
	virtual void ResetUserInfo(void *pIPCBuffer)=0;
	//初始化道具数据
	virtual void OnInitPropList(int nRefleshType = 0)=0;
	//录影接口函数lc090226
	virtual void SetRecordView()=0;
	//加为好友
	virtual void OnAddFriend(bool bIsHave) = 0;
	
	// duanxiaohui 2011-11-11 加入黑名单
	virtual void OnAddEmeny(bool bIsHave) = 0;
	// end duanxiaohui

	virtual void OnGetFriendList(void* pBuff, int nSize) = 0;
	////发送聊天消息
	//virtual void SendShortMessage(MSG_GR_RS_NormalTalk *pShortMessage)=0;
	//MFC获取是否为旁光者模式
	//virtual bool isWatchingMode() = 0;
};



/// CLoveSendClassInExe
/// 用来给游戏窗户端继承的基类
/// 提供消息接收和发送接口
/// 提供游戏中银行界面
/// 提供道具操作界面
/// 提供确认或提示对话框界面
/// 
class EXT_CLASS CLoveSendClassInExe : public CLoadFileBmp, public IFrameInterface
{
	friend class CRightFrame;

	enum{PEER_PAGE=9/*4*/}; // PengJiLin, 2010-10-12, 只显示3个
	DECLARE_DYNAMIC(CLoveSendClassInExe)


	//-------------------------------------------------------------------------
	//原CLoveSendClass的成员
	CWnd								* m_pGameRoom;					//房间窗口
	CUsingSkinClass						* const m_pGameView;			//游戏视图

	// 游戏中加入信号灯功能
	void checkNetSignal(void);
	void getNetSignal(void * pNetData);
	void drawSignal(CDC * pDc);
	int m_NetSignalIndex;                //本次网络信息检测序号
	int m_SavedSingalIndex;     //上一次发送的网络信息检测序号，收到消息后与m_NetSingalIndex同步
	int m_NetSignalLevel;                //网速信号级别
	Image								*pSingnalimg;
	GameInfoStructInExe					* m_pGameInfo;					//游戏信息指针
	vector<MSG_IM_C_GETFRIENDLIST>		m_FriendList;

	UserItemStruct						* m_pUserInfo[MAX_PEOPLE];		//游戏玩家信息

protected:

//	CBankDlg						* m_pBankDlg;						///< 银行界面对话框
//	CCheckPSW						* m_pCheckPwd;						///< 输入密码对话框
	CPBankDlg						* m_pPersonBank;					///< 个人银行界面对话框
	CPBank4GameWnd					* m_pBank2Wnd;						///< 第二版游戏银行对话框
	CUserDetailsDlg					* m_pUserDetailsDlg;				///< 用户详细资料面板
	CGameTrottingWnd				* m_pPmdWnd;						///< 跑马灯

	bool								m_bSaveView;					//保存界面
	POINT								m_SavedScreen;
	bool								m_bMax;							//是否最大化
	bool								m_bShowMax;						//是否显示最大化

	bool								m_bWatchMode;					//旁观模式
	bool								m_bWatchOther;					//可以旁观
	BYTE								m_bWaitTime;					//等待时间
	CPtrArray							m_WatchPtrArray;				//正在旁观玩家指针数组
	CPtrArray							m_ThisDeskPtrArray;				//正在本桌的玩家指针数组

	CString								m_iniFile;						//配置文件路径
	CString								m_HidebtPath;
	CString								m_ShowbtPath;

	bool								m_bInit;						// 指示成员是否初始化
	UserInfoStruct						* m_CurpUserInfoClicked;		// 当前选中的玩家信息指针

	TCHAR								m_BackMusicIni[MAX_PATH+1];		//背景音乐配置文件
	TCHAR								m_BMusicKey[100];				//背景音乐key

	CString								szAdvUrl;						//广告地址链接

	//运行辅助信息
	long int							m_dwCurrentUserID;				//当前用户
	CPropSelUser						*m_pPropSelUser;				//当前正在使用道具的玩家
	int									m_curPropPage;					//当前道具页
	TCHAR								m_szPropEmptyImage[MAX_PATH];	//空道具或道具计数为0时的图片路径
	CRect								m_UserFrameRectArray[MAX_PEOPLE];						//记录用户位置的框数组

	//聊天辅助信息
	CString								m_strLastTalk;					//发送语句
	long int							m_dwLastTalkTime;				//说话时间
	static CArray<LONG,LONG>			m_ShieldArray;					//屏蔽用户
	int									m_nTalkDefaultCount;			//默认聊天次数

	//-----------------------------------------------------------------\
	//有的与显示界面相关
	UINT								m_uTimeID;						//定时器 ID
	UINT								m_uTimeCount;					//定时器计数
	BYTE								m_bTimeStation;					//定期器位置
	COLORREF							m_FrameColor;					//最外框颜色

	int									m_titlex,m_titley;				//标题位置
	int									m_u1x,m_u1y,m_u1h, m_u1w;
	int									m_u2x,m_u2y,m_u2h,m_u2w;
	int									m_vlistx,m_vlisty;
	int									m_logx,m_logy;					//用户头像位置
	COLORREF							m_titlecr,m_usercr;				//标题和用户信息颜色
	CGameImage							m_backImage;					//背景图片

	int									m_iStatic1X,m_iStatic1Y;
	CRect								m_UserInfoRect,m_UserListRect,m_TalkRect;	// 用户信息框，玩家列表框，聊天框
	//背景图
	CGameImage							m_rc,m_lc,m_tc,m_bc;							//四个框
	CGameImage							m_lt,m_rt,m_lb,m_rb;							//四个角
	//右边
	CGameImage							m_right_rc,m_right_lc,m_right_tc,m_right_bc;	//四个框
	CGameImage							m_right_lt,m_right_rt,m_right_lb,m_right_rb;	//四个角
	CGameImage							m_UserInfo_bk;
	int									m_userlistW,m_userlistH,m_talkW,m_talkH;
	int									m_talkX,m_talkY;										//聊天框高XY

	TCHAR								m_szGameRoomName[61];
	CAFCColorResource					m_colorresource;				//聊天,系统颜色结构
	PropStruct							m_stProp ;						//当前道具播放动画情况;

	/////////////////////////////
	//Kylin 20090205 增加用户信息背景
	COLORREF	m_userbkcr;
	/////////////////////////////

	CRightFrame*						m_pRightFrame;					//游戏右边框\
																		//  JianGuankun 2012.4.11

	//原窗口尺寸
	int									m_nWiondwMetricsX;
	int									m_nWiondwMetricsY;

	//-----------------------------------------------------------------\
	//控件

	CBoardCast							*m_dlgBroadcast;
	CExplorer2							m_ie;							//IE浏览器
	CDlgLeftShow						m_DlgShowLeft;					//有人离开提示框
	CToolTipCtrl						m_ToolTip;						//工具提示
	//CControlMessage						m_HorSplitBar;					//横拆分条
	CNewMiniDlg							m_TalkMessage;					//聊天信息
	CPlaySound							m_PlaySound;					//背景音乐
	CGetPoint							m_DlgGetPoint;					//经验盒
	CComboBox							m_InputMessage;					//输入信息

	//CPtrArray							m_ShortMessage;					//信息窗口
	CPictureHelper						m_UserListDlg;					//用户列表
	CGameTalkText						m_ExpressWnd;					//表情窗口
	CLookingClass						m_MessageHandle;				//消息处理
	// 音乐相关
	int									m_sndListID;
	//CNormalBitmapButtonEx				m_btSndPlay;					//播放
	//CNormalBitmapButtonEx				m_btSndPause;					//暂停
	//CNormalBitmapButtonEx				m_btVolDown;					//音量调低
	//CNormalBitmapButtonEx				m_btVolUp;						//音量调高
	int									m_sndBtnCx,m_sndBtnCy;			//按钮大小
	int									sndPlayX,sndPlayY;				//播放按钮坐标
	int									sndPauseX,sndPauseY;			//暂停按钮坐标
	int									sndVolUpX,sndVolUpY;			//音量减小按钮坐标
	int									sndVolDownX,sndVolDownY;		//音量增大按钮坐标
	bool								m_bSndPause;					//是否暂停

	// 玩家间交互
	CNormalBitmapButtonEx				m_BtSend;						//发送按钮
	CNormalBitmapButtonEx				m_BtExpression;					//表情按钮
	CNormalBitmapButtonEx				m_BtGameQuit;					//退出按钮
	CNormalBitmapButtonEx				m_BtSetColor;					//设置文字聊天颜色

	CNormalBitmapButtonEx				m_Btbt1;						//
	CNormalBitmapButtonEx				m_Btbt2;						//
	CNormalBitmapButtonEx				m_Btbt3;						//
	CNormalBitmapButtonEx				m_Btbt4;						//
	CNormalBitmapButtonEx				m_Btbt5;						//
	CNormalBitmapButtonEx				m_Btbt6;						//
	CNormalBitmapButtonEx				m_Btbt7;						//
	//CNormalBitmapButtonEx				m_Btbt8;						//
	//CNormalBitmapButtonEx				m_Btbt9;						//
	CNormalBitmapButtonEx				m_Btbt10;						//

	// 道具
	CNormalBitmapButtonEx				m_BtPropGoAhead;	
	CNormalBitmapButtonEx				m_BtPropGoNext;	
	CNormalBitmapButtonEx				m_BtProp1;
	CNormalBitmapButtonEx				m_BtProp2;
	CNormalBitmapButtonEx				m_BtProp3;
	CNormalBitmapButtonEx				m_BtProp4;
	CNormalBitmapButtonEx				m_BtProp5;
	CNormalBitmapButtonEx				m_BtProp6;
	CNormalBitmapButtonEx				m_BtProp7;
	CNormalBitmapButtonEx				m_BtProp8;
	CNormalBitmapButtonEx				m_BtProp9;
	CNormalBitmapButtonEx				m_BtProp10;
	UINT								m_BtPropArray[PEER_PAGE];			//为了方便,所以将四个按钮放到数组里
	CPropPannel							*m_pPropPannel;				//道具面板对象

	CNormalBitmapButtonEx				m_BtHide;						//隐藏
	CNormalBitmapButtonEx				m_BtExit;						//按钮(大)
	CNormalBitmapButtonEx				m_BtMin;						//按钮
	CNormalBitmapButtonEx				m_btMax;						//最大化
	CNormalBitmapButtonEx				m_btRest;						//还原
	int									nNothing;						//无用成员，
	//------------------------------------------------------------------------//

    // PengJiLin, 2010-9-9, 踢人卡、防踢卡对话框
    CBoardCast*                         m_dlgKickProp;

    // PengJiLin, 2010-9-28
    CGameImage  m_propBackImage;
    CRect       m_propBackRect;
    CGameImage  m_propBackImage2;
    CRect       m_propBackRect2;

    int         m_curPropPage_S;

	bool		m_bIsQueueDisMiss;

private:
	CSkinMgr				m_skinmgr;	

	//保存各个视图位置玩家的视频对话框偏移量
	int					m_iVideoOffsetNum;//玩家数量
	int					m_pVideoXOffset[VIDEO_MAX_PEOPLE]; //X偏移量
	int					m_pVideoYOffset[VIDEO_MAX_PEOPLE]; //Y偏移量

	ivideointerface			*m_pVideoInterface;
	int						GetMyDeskStation();
	///获取用户信息
	virtual bool GetUserInfo(int nDeskStation, UserInfoForGame &Info);
	//设置视频窗口位置
	void set_videos_pos();
	//重新设置视频对应的玩家ID
	void set_videos_userid();
public:
    // PengJiLin, 2011-4-18, 在线时长、局数送金币功能
    CM_UserState_Send_Times_Money_For_Exe m_SendMoneyData;

    // PengJiLin, 2011-4-18, 在线时长、局数送金币功能：获取格式化提示消息
    void GetSendMoneyFormatInfo(CString& strInfo);

    // PengJiLin, 2011-5-10, 在线时长、局数送金币功能：获取格式化提示信息
    void GetSendMoneyFormatInfo(CString& strTime, CString& strCount);

    // PengJiLin, 2011-5-10, 在线时长、局数送金币功能：结算框使用的格式化信息
    void GetSendMoneyFormatInfo(CString& strGet, CString& strTime, CString& strCount);

    // PengJiLin, 2011-4-19, 在线时长、局数送金币图像提示
    virtual void ShowGetMoneyImage();

	UserItemStruct* GetUserItemInfo(int iIdx);

public:
	CLoveSendClassInExe(CUsingSkinClass * pGameView);   // 标准构造函数
	virtual ~CLoveSendClassInExe();
	//处理视频设置改变
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//初始化
	bool InitFrame(GameInfoStructInExe * pGameInfo, CWnd *pGameRoom);
	bool AFCCloseFrame(bool bNotifyServer=true);

	bool CreateFrame();
	void DeleteFrame();
	//初始化函数
	virtual BOOL OnInitDialog();

	// 对话框数据
	//enum { IDD = IDD_EXE_DIALOG };

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	void DoDataExchangeWebVirFace(CDataExchange * pDX){}
	//消息解释函数
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//重画函数
	afx_msg void OnPaint();
	//位置变化
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//定时器消息
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	public:
	///< 读取等级信息，GetGameOrderName使用
	static void ReadLevelData();

    // PengJiLin, 2011-6-23, 激活
    afx_msg void OnActivate(UINT uState, CWnd* pOldWnd, BOOL bMinisted);

	///< 获取游戏里面的积分级别
	///< @dwPoint玩家积分
	///< 玩家积分名称
	static TCHAR * GetGameOrderName(__int64 dwPoint);
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedExit();
	virtual afx_msg void OnBnClickedRest();//百家乐 改成虚函数 by wlr 20090716
	//表情消息
	LRESULT OnExpression(WPARAM wparam, LPARAM lparam);
	//名字消息
	LRESULT OnRichEditHit(WPARAM wparam, LPARAM lparam);
	//关闭游戏
	LRESULT OnCloseGame(WPARAM wparam, LPARAM lparam);
	//拆分条消息
	LRESULT OnSplitMessage(WPARAM wparam, LPARAM lparam);
	//用户使用广播确定后
	LRESULT OnUseBoardcast(WPARAM wparam, LPARAM lparam);
	// 从道具面板中传回使用道具消息
	LRESULT OnPannelUseProp(WPARAM wparam, LPARAM lparam);
	// 从道具面板中传回购买道具消息
	LRESULT OnPannelBuyProp(WPARAM wparam, LPARAM lparam);
	// 从道具面板中传回赠送道具消息
	LRESULT OnPannelGiveProp(WPARAM wparam, LPARAM lparam);
	//载入皮肤
	LRESULT OnLoadSkin(WPARAM wparam, LPARAM lparam);
	//重读talkIngame.ini
	LRESULT OnReLoadTalkINI(WPARAM wparam, LPARAM lparam);
	//发送积分
	LRESULT OnGetPoint(WPARAM wparam,LPARAM lparam);
	//银行取钱操作
	LRESULT OnBankCheckOut(WPARAM wparam,LPARAM lparam);
	//道具传回的购买VIP消息     add by wyl   11-5-21
	LRESULT OnPannelBuyVIP(WPARAM wparam, LPARAM lparam);
	//添加好友
	LRESULT OnSendGetFriendList(WPARAM wParam, LPARAM lParam);   
	//游戏银行存取钱
	LRESULT OnGameBankMoneyCheck(WPARAM wParam, LPARAM lParam);   
	LRESULT OnGameBankMoneyCheck2(WPARAM wParam, LPARAM lParam);   
	//关闭游戏银行窗口消息
	LRESULT OnGameBankClose(WPARAM wParam, LPARAM lParam);   

	//购买使用
	LRESULT OnBuyAndUse(WPARAM wParam, LPARAM lParam);  


protected:
	DECLARE_MESSAGE_MAP()

	//绘制框架
	void DrawViewFrame(CDC * pDC, int iWidth, int iHeight);
	void DrawCurUserInfo(CDC *pDC);

	// ----------------------\\
	//注册表函数
	//注册表函数
public:
	///写入数值
	///@param szEntry 入口字符串
	///@param iDefault 默认值
	BOOL WriteProfileInt(TCHAR * szEntry, int iDefault);
	//获取字符串
	BOOL WriteProfileString(TCHAR * szEntry, TCHAR * szValue);
	//获取数值
	UINT GetProfileInt(TCHAR * szEntry, int iDefault);
	//获取字符串
	CString GetProfileString(TCHAR * szEntry, TCHAR * szDefault);

    // PengJiLin, 2010-6-1, 增加 记牌器 接口, 游戏端重载
    virtual void UseLookCardProp(_TAG_USERPROP* userProp);

    // PengJiLin, 2010-9-9, 踢人卡与防踢卡功能接口
    virtual void UseKickUserProp(_TAG_USERPROP* userProp);
    virtual void UseAntiKickProp(_TAG_USERPROP* userProp);

    // PengJiLin, 2010-9-10, 将秒数格式化为时分秒的格式
    void SecondFormat(int iSecond, CString& strOutput);

    // PengJiLin, 2010-9-10, 踢人卡、防踢卡使用结果
    bool OnUseKickPropResult(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, BOOL bIsKick);

    // PengJiLin, 2010-9-14, 踢人卡、防踢卡提示框显示
    void ShowNewKickPropDlg(BOOL bUseForOther, BOOL bUseLink, BOOL bIsTOut, CString& strInfo);

public:
	//-------------------------------------------------------------------------\
	//供子窗口调用的接口
	int GetComType(void){return m_pGameInfo->uComType;}

	void OpenUserMenu(UserItemStruct * pUserItem);			// 右击用户列表时，打开右键菜单
	void OpenUserMenuEx(UserItemStruct * pUserItem);		// 右击用户列表时，打开右键菜单
	void OnLBClickUserList(UserItemStruct * pUserItem);		// 左击用户列表
	void OnLBDBClickUserList(UserItemStruct * pUserItem);	// 双击用户列表

	bool IsEnableSound(){ return m_pGameInfo->bEnableSound; }		// 是否播放声音
	//获取自己信息
	UserInfoStruct * GetMeUserInfo() { if(NULL == m_pGameInfo)return NULL; return &m_pGameInfo->uisMeUserInfo.GameUserInfo; };
	//控制消息 
	virtual UINT OnControlMessage(UINT uControlCode, void * pControlData, UINT uDataSize);
	// 收到服务器端传来的同桌玩家清单
	void OnUserListAtDesk(void *pUserItem);
	//网络读取消息
	virtual bool OnSocketReadEvent(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize);
	//发送游戏房间消息
	virtual void ShowMsgInGame(TCHAR * szMsg,BYTE bNewOrSys=0, UINT uSize=0, TCHAR * szFontName=NULL, TCHAR *Title=NULL);
	//发送嘟嘟消息
	virtual void ShowMsgDudu(TCHAR * szSendName, TCHAR * szTargetName,TCHAR * szCharString, COLORREF crTextColor, bool bShowTime, UINT uSize=0, TCHAR * szFontName=NULL,int iDuduType = 0);
	void OnInitPropList(int nRefleshType = 0);											//当收到道具列表信息时初始化道具
	void ResetUserInfo(void *pUserInfo);					//更新同桌玩家数据

	void OnAddFriend(bool bIsHave);
	void OnGetFriendList(void* pBuff, int nSize);

	// duanxiaohui 2011-11-11 加入黑名单
	void OnAddEmeny(bool bIsHave);

	//菜单功能///////////////////////////////////////////////

	//查看用户信息
	BOOL OnCmdShowUserInfo();

	//复制用户信息
	BOOL OnCmdCopyUserInfo();

	//踢人
	BOOL OnCmdKickUser();

	//踢人
	BOOL OnCmdKickUserProp();

	//赠送鲜花
	BOOL OnCmdFlowerProp();

	//拍砖头
	BOOL OnCmdBlickProp();

	/////////////////////////////////////////////////////////

	//录影接口函数lc090226
	virtual void SetRecordView()
	{
	};
	//-------------------------------------------------------------------------\
	//功能函数
	//发送网络消息函数
	int SendGameData(BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode);
	//发送网络消息函数
	int SendGameData(void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode);
	//桌号换算到视图位置
	virtual BYTE ViewStation(BYTE bDeskStation);
	//获取状态参数
	BYTE GetStationParameter() { return m_pGameInfo->bGameStation; };
	//设置定时器
	void SetGameTimer(BYTE bDeskStation, UINT uMaxTime, UINT uTimeID);
	//获取游戏编号 add by wlr 20090716
	virtual void GetGameCode(char * szGameCode);
	///判断是否是排队机房间
	///@return true:是排队机房间；false：不是排队机房间
	inline bool IsQueueGameRoom() const;
	//MFC获取是否为旁光者模式
	bool isWatchingMode();
	///切换最大化与还原
	void SwitchSize();
protected:
	void LoadSkin(bool bHide);///< 载入皮肤
	void LoadSoundList(void);

	void SendTalkMessage(int nDefaultIndex=0);
	void OnSetPropImages();											//设置道具按钮图片
	BOOL OnToolTipNotify(UINT id, NMHDR * pTTTStruct, LRESULT * pResult );	//响应ToolTip消息
	BOOL OnUseProp(int nPropIndex, int nDeskUserID);				//使用道具
	//设置状态参数
	void SetStationParameter(BYTE bGameStation);
	//重新设置界面
	virtual void ResetGameFrame();
	virtual void ShowUserTalk(MSG_GR_RS_NormalTalk * pNormalTalk){return ;}
	//播放声音
	void PlayGameSound(TCHAR * szFileName);
	//播放声音
	void PlayGameSound(HINSTANCE hInstance, TCHAR * szResourceName);
	//删除定时器
	void KillGameTimer(UINT uTimeID);

	/// 从游戏界面中打开银行界面
	void OnVisitBank();
	/// 刷新用户列表中的金币
	/// @param dwUserID 用户ID号
	/// @param uCount 可为正或负数的值，意思为加或减
	bool FlushUserMoney(long dwUserID,__int64 i64Count);

    // PengJiLin, 2010-10-12, 商场道具功能
    void OnSetPropImagesS();

	//道具响应消息函数
	afx_msg void OnBnClickedGoAhead();//第一排
	afx_msg void OnBnClickedGoNext();
    afx_msg void OnBnClickedGoAheadS();//第二排
    afx_msg void OnBnClickedGoNextS();

	afx_msg void OnBnClickedButtonProp1();
	afx_msg void OnBnClickedButtonProp2();
	afx_msg void OnBnClickedButtonProp3();
    afx_msg void OnBnClickedButtonProp4();
    afx_msg void OnBnClickedButtonProp5();
    afx_msg void OnBnClickedButtonProp6();
	afx_msg void OnBnClickedButtonProp7();
	afx_msg void OnBnClickedButtonProp8();
	afx_msg void OnBnClickedButtonProp9();
	afx_msg void OnBnClickedButtonProp10();

    void BuyAndUseProp(int propIndex);

	//内部虚拟函数（不需要重载）
protected:
	//更新标题
	virtual void UpdateGameTitle();
	//用户进入
	virtual bool GameUserCome(BYTE bDeskStation, UserItemStruct * pUserItem, bool bWatchUser);
	//用户离开
	virtual bool GameUserLeft(BYTE bDeskStation, UserItemStruct * pUserItem, bool bWatchUser);
	//用户同意
	virtual bool UserAgreeGame(BYTE bDeskStation);
	//调整位置
	virtual void FixControlStation(int iWidth, int iHeight);
	////框架函数
	virtual bool OnFrameMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize);
	////信息函数
	virtual bool OnSystemMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize);

	// 需要重载的虚拟函数
	virtual bool HandleGameMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize);
	//定时器消息
	virtual bool OnGameTimer(BYTE bDeskStation, UINT uTimeID, UINT uTimeCount)=0;
protected:
	//常规重载函数
	//游戏消息处理函数
	//经验盒消息处理函数
	virtual bool HandlePointMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize);
	/// 处理银行的消息
	/// 与客户端的GameRoomEx中大同小异，只操作本地金币数和银行面板数据的变化
	/// @param[in] pNetHead 协议头
	/// @param[in] pNetData 数据包
	/// @param[in] uDataSize 数据包大小
	/// @return 如果成功处理，则返回true，返回false表示处理该条消息失败，其中一种可能是因为协议不匹配，例如修改了头文件却没有都重新编译
	/// 
	virtual bool OnAboutMoney(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize);
	//道具消息
	virtual bool OnPropMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize);
	//道具消息
	virtual bool OnPropVipCheckTime(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize);
	//使用道具
	virtual bool OnUsePropResult(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize);
	//重新设置游戏数据
	virtual void ResetGameStation(int iGameStation=1){};
	//设置函数
	virtual void OnGameSetting() {};
	//设置函数
	virtual void OnGetBuyItemData(int iPropID);
	//设置游戏状态
	virtual bool SetGameStation(void * pStationData, UINT uDataSize){return false;};
	//旁观改变
	virtual void OnWatchSetChange(){};
	//道具
	virtual bool UserProp(_TAG_PROP_MOIVE_USE *propMoive);
	//道具动画
	virtual bool PlayNextGif();

    // PengJiLin, 2010-9-14, 使用踢人卡踢人结果消息
    virtual bool OnNewKickUserResult(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize);

	//关闭银行
	virtual void OnCloseBank2();

private:
	void SetFillColor(COLORREF rgb);		//设置填充色
	void NavigateVirFace();
	afx_msg void OnBnClickedQuitGame();
	virtual afx_msg void OnBnClickedMax();//百家乐 改成虚函数 by wlr 20090716
	afx_msg void OnBnClickedHide();
	afx_msg void OnBnClickedMin();
	virtual afx_msg void OnMove(int x, int y);//百家乐 改成虚函数 by wlr 20090716
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	void OnSndPlay();
	void OnSndPause();
	void OnSndVolDown();
	void OnSndVolUp();

	/// 根据全局变量中的加密方式，取得加密后的密码字符串可能是sha可能是MD5
	/// 函数里不对指针的合法性作判断
	/// @param szMD5Pass 加密后的字符串
	/// @param szSrcPass 源字符串
	/// @return 加密后的字符串指针
	TCHAR * GetCryptedPasswd(TCHAR *szMD5Pass, TCHAR *szSrcPass);
	/// 银行界面发送指令，存钱或取钱
	/// @param pCheckMoney 存取钱结构指针，在函数执行结束后要删除该指针
	/// @return 无
	void OnCheckMoney( CheckMoney_t *pCheckMoney );
	/// 第2版银行的存取款
	/// @param pCheckMoney 存取钱结构指针
	/// @return 无
	void OnCheckMoney2( bank_op_normal *pCheckMoney );
	/// 银行界面发送指令，转帐
	/// @param pTransferMoney 存取钱结构指针，在函数执行结束后要删除该指针
	/// @return 无
	void OnTransferMoney( TransferMoney_t *pTransferMoney );
	/// 银行界面发送指令，修改密码
	/// @param pChangePasswd 修改密码结构指针，在函数执行结束后要删除该指针
	/// @return 无
	void OnChangePasswd( ChangePasswd_t *pChangePasswd );
	/// 银行界面发送指令，转帐记录
	/// @return 无
	void OnTransferRecord();

	void OnGetNickNameOnID(GetNickNameOnID_t* pGetNickNameOnID);
protected:
	//在本桌查找单个用户，根据用户ID查找
	UserItemStruct * FindOnLineUser(long int dwUserID);
	//请平台传来所有同桌玩家名单，用于自己加入桌子时
	void GetOnLineUser( BYTE bMeDeskStation );
	// 使用道具
	void UsePropItem(int propIndex, BOOL bCheckNum = TRUE);     // PengJiLin, 2010-10-14, 增加一参数
	void UseBoardCast(_TAG_USERPROP * userProp);

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedGameBtn01();
public:
	afx_msg void OnBnClickedGameBtn02();  //游戏商城
public:
	afx_msg void OnBnClickedGameBtn03();
public:
	afx_msg void OnBnClickedGameBtn04();
public:
	afx_msg void OnCbnSelchangeInput();
public:
	afx_msg void OnBnClickedSend();
public:
	afx_msg void OnBnClickedSetcolor();
public:
	afx_msg void OnBnClickedExpression();
public:
    afx_msg void OnBnClickedBlueDiamond();  // PengJiLin, 2010-9-30, 蓝钻贵族 购买vip

public:
	afx_msg void OnClose();
protected:
	virtual void OnCancel();
};

class EXT_CLASS CGlobal//全局类
{
public :
	CGlobal()
	{
		CString s=CBcfFile::GetAppPath();/////本地路径


		CINIFile f( s + "bzgame.ini");
		m_key="BZW";
		_tcscpy(m_CenterServerPara.m_strHomeADDR, TEXT("http://bbs"));
		//		m_CenterServerIPAddr=f.GetKeyVal (m_key,"LoginIP","127.0.0.1");//服务器IP		

		m_skin=f.GetKeyVal(m_key,"skin","skin1");
		//m_pastimeurl=f.GetKeyVal(m_key,"pastime","");
		m_urlpostfix=f.GetKeyVal(m_key,"urlpostfix","asp");
		m_NewUrl=f.GetKeyVal(m_key,"newurl","");

		m_szSPToken="sn";
		m_szProcessName = "ddz";
		userPropLibrary.RemoveAll();
	}
	virtual ~CGlobal()
	{
		for (int i=0; i<userPropLibrary.GetCount(); ++i)
		{
			_TAG_USERPROP *userProp = userPropLibrary.GetAt(i);
			delete userProp;
		}
		userPropLibrary.RemoveAll();
	}
	CenterServerMsg m_CenterServerPara;
	CString m_key;
	CString m_Btn[10];//最多20个
	CString	m_szSPToken;//CookieSPToken,用于验证用户登录
	CString	m_skin;
	CString m_sktext[5];
	CString m_urlpostfix;
	CString m_NewUrl;
	CString m_TML_SN;
	CArray<_TAG_USERPROP*,_TAG_USERPROP*> userPropLibrary;							//用户拥有的道具

	// 只用于游戏部分的成员，平台部分的无下列数据
	CString m_szProcessName;
	/// 显示玩家金币，银行金币等表示金额的数值时，后面加上几个零
	/// 123金币须显示为123000，则此值为3
	int			m_nPowerOfGold;	

    // PengJiLin, 2010-8-3, 金币显示可以配置是否有分隔符，以及什么样的分隔符
    bool        m_bUseSpace;        // true = 有分隔符
    TCHAR       m_strSpaceChar[10]; // 单个字母的分隔符

	int			m_iMaxBit;
};



extern EXT_CLASS CGlobal& Glb();
