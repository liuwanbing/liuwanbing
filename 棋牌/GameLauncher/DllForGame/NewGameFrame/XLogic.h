#pragma once
#include "XGameLogicBase.h"
#include "IGameFrame.h"
#include "IAudioPlay.h"
#include "UserDetailsDlg.h"   ///< 用户详细资料面板
#include <list>
#include <map>
#include "afxWin.h"
#include "afxext.h"
#include "afxdisp.h"
#include "PBank4GameWnd.h"		///< 第二版游戏银行
#include "GameTrottingWnd.h"
#include "SkinMgr.h"
#include "ivideointerface.h"

#include "UI_ForLogic/iuserinterface.h"
using std::list;

class XRightFrame;				///游戏右边框

const int HEART_BEAT_SECOND		= 3;	// 心跳间隔
const int HEART_BEAT_DELAY		= 10;	// 检测心跳延时

const int OLEDRAW_TIMER_ID      = 1;    // 画OLE类型控件
const int OLEDRAW_INTERVAL      = 66;

#define UM_FIXRIGHTFRAME		(WM_USER + 100) //修正右边框位置消息
///聊天类型
enum CHATTYPE
{
    CT_PRIVATE	= 0,	///<私人
    CT_SYSTEM	= 1,	///<系统
    CT_NORMAL   = 2,    ///<一般消息
    CT_PUBLIC	= 3,	///<公共
    CT_SOCIETY  = 4,	///<公会
    CT_DALABA   = 5,    ///<大喇叭消息
    CT_XIAOLABA = 6     ///<小喇叭消息
};
///聊天信息
struct TalkMessage_t
{
    TCHAR           szSourceNickName[32]; /// 发送消息的昵称
    int				iSourceID;
    int				iTargetID;
    CHATTYPE		eChatType;
    time_t			sTime;
    TCHAR			szMsgString[MAX_TALK_LEN + 1];
	COLORREF        crTextColor;
    TalkMessage_t()
    {
        ZeroMemory(this, sizeof(TalkMessage_t));
    }
};

class CChannelService;
class IGameImpl;
interface IUserInterface;
class IGameEngine;
class IContain;
class XLogic : public XGameLogicBase,
    public IGameFrame
{

	friend class XRightFrame;
	friend class CUseBoardCastWnd;

public:
    XLogic();
    virtual ~XLogic();

private:
    XLogic(const XLogic &other);
    XLogic &operator = (const XLogic &other);

public:
    /// 初始化
    virtual int Initial(LogicInitParam_t &lip);
    /// IPC消息
    virtual bool OnIPCMessage(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend);
    /// IPC心跳检测
    virtual bool OnIPCHeartBeat();
    /// UI消息
    virtual int OnUIMessage(TUIMessage *p);
    /// 时钟
    virtual void OnTimer(int nEventID);
    //离开游戏
    virtual void QuitGame(bool bNotify = true,bool bNeedCheck = false/*是否要弹出提示框*/);
    /// 窗口大小改变
    virtual void OnSize(RECT &rect);
    /// 使用道具
    virtual void OnPannelUseProp(WPARAM wParam, LPARAM lParam);
    /// 购买道具
    virtual void OnPannelBuyProp(WPARAM wParam, LPARAM lParam);
    /// 赠送道具
    virtual void OnPannelGiveProp(WPARAM wParam, LPARAM lParam);
    /// 对他人使用道具
    virtual void OnSelOtherUseProp(WPARAM wParam, LPARAM lParam);
    //用户使用广播，对话框里确定后，在这里给用户ID和用户名赋值
    virtual void OnUseBoardcast(WPARAM wparam, LPARAM lparam);   
    //银行界面发来的消息
    virtual void OnBankMessage(UINT message,WPARAM wParam,LPARAM lParam);
	//关闭银行
	virtual void OnCloseBank2();
	//比赛场用于控制显示
	void OnShowContestResult(UINT bActionCode, int iRank=0, int iAward=0);

public:
    int SendGameData(BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode);
    /// 发消息
    virtual bool SendGameData(void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode);
    ///游戏从框架中获取的信息
    virtual int GetMyDeskStation();
    ///获得自己的桌子号
    virtual int GetMyDeskNum();
    ///这里传的是视图索引
    virtual int GetUserStation(int nIndex);  
    ///将消息内容发送到游戏服务器
    virtual int SendGameData(WORD wSubId,void *buffer, int nLen);
    ///只将消息发送到客户端大厅
    virtual int SendIpcData(WORD wMainCmdID, WORD wSubCmdID, void * pBuffer, WORD wDataSize);
    ///视图转服务器坐位 因为所有游戏都要用到，所以在框架中处理
    virtual int ViewStation2DeskStation(int nViewIndex);
    ///坐位号转视图号
    virtual int DeskStation2View(int nDeskStation);
    ///获取玩家昵称
    virtual TCHAR * GetUserNickName(int nDeskStation);
    ///关闭游戏客户端
    virtual void CloseClient();
    ///点击继续
    virtual int ClickContinue();
    ///强制关闭游戏
    virtual void CloseClientforce();
    ///设置时钟
    virtual void SetTimer(int nEventID, DWORD dwTime);
    ///关闭时钟
    virtual void KillTimer(int nEventID);
    ///获取游戏音量
    virtual int GetVolume(); 
    ///获取游戏方言
    virtual int GetTalkKind();
    ///插入一条系统消息
    virtual int InsertSystemMessage(TCHAR* szMessage);
    ///插入一条普通消息
    virtual int InsertNormalMessage(TCHAR* szMessage);

    ///获取声音路径
    /// param _path[in, out] 调用者准备的内存空间
    /// param nSize[in] _path所在地址拥有的空间大小，单位为sizeof(TCHAR)
    /// return 将_path所在空间赋值后，返回_path
    virtual char* GetAudioPath(int nDeskStation,char *_path, int nSize = MAX_PATH);
    ///获取奖励类型
    virtual bool GetAwardType(AwardTypeForGame &Type);	
    ///播放奖励动画
    virtual bool PlayAwardAnimation(const AwardTypeForGame &type);
    ///获取倍率
    virtual bool GetGameBaseInfo(GameInfoForGame &Info);
    ///获取用户信息
    virtual bool GetUserInfo(int nDeskStation, UserInfoForGame &Info);
    /** @brief 播放大头娃娃表情
    *	@param[in]  nViewStation    视图位置
    *	@param[in]  nIndex          表情类序号，目前为1~7，具体参见《动画表情.xls》
    *	@return 布尔型
    */
    virtual bool PlayBigPortrait(int nViewStation, int nIndex);
    /** @brief 使用小头像
     */
    virtual bool SetUseSmallLogo(int nViewStation, bool bUseSmall=true);
    /// 播放声音
    virtual bool PlaySound(const char *szFileName);
    /// 某方位是否使用视频
    virtual bool SetUseVideo(int nViewStation, bool bUse);
    /// 获取是否为旁观状态
    virtual bool GetIsWatching();
    /// 获取是否允许旁观
    virtual bool GetIsEnableWatch();
    /// 从游戏界面中打开银行界面
    virtual void OnVisitBank();
    /// 获取全局信息
    virtual CGlobal* GetGlb();
    virtual RECT GetGameWindowRect();

    /// 获取游戏信息2011-7-7
	virtual const GameInfoStructInExe* GetGameInfo(){return &m_GameInfo;};

	 //状态改变
    virtual void SetStationParameter(BYTE bGameStation);

private:

	//读talkIngame.ini
	void OnReLoadTalkINI();
    //控制通知
    bool OnIPCControl(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend);
    //网络指令
    bool OnIPCSocket(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend);
    //设置指令
    bool OnIPCConfig(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend);
    //用户动作
    bool OnIPCUserAct(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend);
    //道具初始化
    bool OnIPCProp(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend);
    //套接字消息
    bool OnSocketReadEvent(NetMessageHead * pNetHead, void *pNetData, WORD wSendSize);
    //系统消息
    bool OnSystemMessage(NetMessageHead * pNetHead, void *pNetData, WORD wSendSize);
    //道具消息
    bool OnPropMessage(NetMessageHead * pNetHead, void *pNetData, UINT wSendSize);
    bool OnUsePropResult(NetMessageHead * pNetHead, void *pNetData, UINT wSendSize);
    //vip时间检测
    bool OnPropVipCheckTime(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize);
    // PengJiLin, 2010-9-14, 使用踢人卡踢人结果消息
    bool OnNewKickUserResult(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize);
    //框架消息
    bool OnFrameMessage(NetMessageHead * pNetHead, void *pNetData, WORD wSendSize);    
    // PengJiLin, 2010-9-10, 踢人卡、防踢卡使用结果
    bool OnUseKickPropResult(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, BOOL bIsKick);
    // PengJiLin, 2010-9-10, 将秒数格式化为时分秒的格式
    void SecondFormat(int iSecond, CString& strOutput);
   
	//道具操作结果
	bool OnIPCUserPropResult(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend);
	//跑马灯消息操作
	bool OnIPCTrottingMsg(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend);
	//换肤消息
	bool OnExchageSkin(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend);


	bool UserProp(_TAG_PROP_MOIVE_USE *propMoive);

	/// 聊天右键菜单响应
	int OnTalkMenuMessage(TUIMessage *pMessage);

	//设置视频窗口位置
	void set_videos_pos();
	//重新设置视频对应的玩家ID
	void set_videos_userid();
private:
    ////////////////////////////////////////////////////////////////////////////////
    // 用户信息
    // 根据用户ID找本桌玩家中是否有
    // 请求平台传来同桌玩家的数据
    void GetOnLineUser( BYTE bMeDeskStation );
    UserItemStruct *FindOnLineUser(long int dwUserID);
    UserItemStruct *FindWatchUser(long int dwUserID);
    UserItemStruct *FindOnLineUser(BYTE bDeskStation);
	void OnUserListAtDesk(UserItemStruct *puserinfo);
    bool GameUserLeft(BYTE bDeskStation, UserItemStruct * pUserItem, bool bWatchUser);
    UserInfoStruct * GetMeUserInfo();

private:
    ////////////////////////////////////////////////////////////////////////////////
    // 框架
    UINT OnControlMessage(UINT uControlCode, void * pControlData, UINT uDataSize);

    ////////////////////////////////////////////////////////////////////////////////
    // 聊天    
    int InsertTalk(const TalkMessage_t& sMsg);
    int InsertDuduMsg(TCHAR * szSendName, TCHAR * szTargetName,TCHAR * szCharString, 
        COLORREF crTextColor, bool bShowTime, UINT uSize=0, 
        TCHAR * szFontName=NULL,int iDuduType=0);

    int RefreshTalk();
    TCHAR* FormatTalk(const TalkMessage_t& sMsg);

    // 初始化表情框
    void UIInitBiaoQingKuang();
    // 弹出表情框
    void UIShowBiaoQingKuang(TUIMessage *pMessage);
    // 单击表情
    void UIHitBiaoQing(TUIMessage *pMessage);
    void SendTalkMsg(TCHAR* pMsg, int iUserID,int nTalkIdx = 0);
    /// 参数 nControlIndex表示从自己对面数来第几个，对面是0，依次数过来
    int ShowOneUserInfo(int nControlIndex,UserItemStruct * pUserInfo,bool bVisible);

	int ShowCurrentUserInfo(UserItemStruct * pUserInfo,bool bVisible);
    /// 初始化玩家列表控件
    int InitPlayerListCtrl();
    int RefreshPlayList();

    ////////////////////////////////////////////////////////////////////////////////
    // 道具 
    /// 初始化道具列表
    int InitPropPannel();
    int RefreshPropPannel();
    void OnHitPropBtn(int nIndex);
    bool UIEnableButton(int nID, bool bEnable);
    void OnBuyPropUIMessage(TUIMessage *pMessage);
    // 使用道具
    void UsePropItem(int propIndex, BOOL bCheckNum = TRUE);
    BOOL OnUseProp(int nPropIndex, int nDeskUserID);    
    void UseAntiKickProp(_TAG_USERPROP* userProp);  // 使用防踢卡
    void UseKickUserProp(_TAG_USERPROP* userProp);  // 使用踢人卡    
    void UseBoardCast(_TAG_USERPROP * userProp);    // 用户使用广播，对话框里确定后，在这里给用户ID和用户名赋值
    int ShowInputDlg(int type);                   // 弹出大小喇叭输入框    

    void OnSetUIMessage(TUIMessage *pMessage);
    ///弹出对话框， fun为回调函数参数为一个bool值点确实和取消的值
    void ShowMessageDialog(TCHAR* szMsg, int itype);

    void OpenUserMenu(UserItemStruct * pUserItem);			// 右击用户列表时，打开右键菜单

    ////////////////////////////////////////////////////////////////////////////////
    // 银行
    /// 处理银行的消息
    /// 与客户端的GameRoomEx中大同小异，只操作本地金币数和银行面板数据的变化
    /// @param[in] pNetHead 协议头
    /// @param[in] pNetData 数据包
    /// @param[in] uDataSize 数据包大小
    /// @return 如果成功处理，则返回true，返回false表示处理该条消息失败，其中一种可能是因为协议不匹配，例如修改了头文件却没有都重新编译
    /// 
    bool OnAboutMoney(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize);
    /// 刷新用户列表中的金币
    /// @param dwUserID 用户ID号
    /// @param uCount 可为正或负数的值，意思为加或减
    bool FlushUserMoney(long dwUserID,__int64 i64Count);

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

	void OnBnClickedRightFrameHide();
	void OnBnClickedRightFrameShow();

    /************************************************************************/
    //Function:checkNetSignal()
    //Parameter:
    //                pNetData        :服务器反回的消息体
    //Author:Fred Huang 2008-06-27
    //Note:解析服务器返回的消息
    /************************************************************************/
    void getNetSignal(void * pNetData);
    void drawSignal();
	//按金币取等级
	int GetMoneyGrade(__int64 dwPoint);
	void DecodeString(CString str, CString &strColumn, int &width);

public:
	//响应窗口消息
	void OnMessageProc(UINT message,WPARAM wParam,LPARAM lParam);

	//菜单功能///////////////////////////////////////////////

	//查看用户信息
	BOOL OnCmdShowUserInfo();

	//复制用户信息
	BOOL OnCmdCopyUserInfo();

	//踢人
	BOOL OnCmdKickUser();

	//踢人
	BOOL OnCmdKickUserProp();

	//添加好友
	BOOL OnAddFriend();

	//添加黑名单
	BOOL OnAddEmeny();

	//赠送鲜花
	BOOL OnCmdFlowerProp();

	//拍砖头
	BOOL OnCmdBlickProp();


	//播放道具动画
	void PlayProp(const bool& isSend,const CString& strFilePathName);

	//获取窗体句柄
	HWND GetHWND(){return m_hWnd;}
private:
    IGameEngine             *m_pEngine;

	bool					m_bUserContestOver;	//标记该用户是被淘汰或者比赛结束，用于点击关闭按钮时，是否判断游戏状态
	ivideointerface			*m_pVideoInterface;
    IGameImpl               *m_pGameImpl;
    HINSTANCE               m_hGameInstance;
    CChannelService         *m_pIPC;
    IUserInterface          *m_pIUI;                ///< UI对像
    HWND                    m_hWnd;

    DWORD                   m_dwIPCHeartBeatTick;

	CUserDetailsDlg *       m_pUserDetailsDlg;
    GameInfoStructInExe		m_GameInfo;
    list<TalkMessage_t>	    m_TalkMessages;

    bool                    m_bWatchOther;
    bool                    m_bWatchMode;
    BYTE                    m_bWaitTime;
    CArray<LONG,LONG>                       m_ShieldArray;                  //屏蔽用户
    CArray<_TAG_USERPROP*,_TAG_USERPROP*>   m_userPropLibrary;              //用户拥有的道具
    CPtrArray							    m_WatchPtrArray;				//正在旁观玩家指针数组
    CPtrArray							    m_ThisDeskPtrArray;				//正在本桌的玩家指针数组
	map<string, int>                        m_MapColumn;                    //用户列表项存诸

	int						m_iSelectIndex;

	PropStruct							m_stProp ;						//当前道具播放动画情况;


    enum
    {
        PROP_PEER_PAGE=9,   ///<一页多少个道具
        PROP_IDS=5          ///<单个道具占几个ID，按钮+1 个数+2
    };
    IContain                *m_pPropContain[PROP_PEER_PAGE];
    int                     m_nPropPage;

    CString                 m_strPropUrl;
    int			            m_bkcolor_R;
    int			            m_bkcolor_G;
    int			            m_bkcolor_B;

    std::map<int, bool>     m_IsUserSmall;

    int                     m_NetSignalIndex;
    int                     m_NetSignalLevel;                               //网速信号级别
    int                     m_SavedSingalIndex;                             //上一次发送的网络信息检测序号，收到消息后与m_NetSingalIndex同步

	bool m_bIsAutoScroll;	//记录是否滚屏
	bool m_bIsAcceptChatMsg;	//是否接受聊天消息
public:
	CBoardCast              *m_dlgBroadcast;                                //发广播对话框
	CPropPannel             *m_pPropPannel;                                 //道具面板对象
	CPropSelUser            *m_pPropSelUser;                                //当前正在使用道具的玩家
	CPBankDlg				*m_pPersonBank;					                //个人银行界面对话框
	CPBank4GameWnd			*m_pBank2Wnd;									//第二版游戏银行对话框

private:
    HMODULE                 m_hAudioMod;
    CREATEAUDIOPLAYC        pCreateAudioPlay;
    DELETEAUDIOPLAYC        pDeleteAudioPlay;
    IAudioPlay              *m_pBackAudio;
    bool                    m_bSndPause;
    // 载入声音列表
    void LoadBkSoundList(void);
	void OnPropBuyVIP(UINT message,WPARAM wParam,LPARAM lParam);

public:
	vector<MSG_IM_C_GETFRIENDLIST>		m_FriendList;
	INT_PTR GetOnlineUserCount();
	UserItemStruct *FindOnLineUserByIdx(long int dwIdx);
	CChannelService *GetChannelServer() { return m_pIPC;};


	bool		m_bIsQueueDisMiss;
public:
	
	CGameTrottingWnd* m_pPmdWnd; //跑马灯

	XRightFrame* m_pRightFrame;	//游戏右边框\
									JianGuankun 2012.4.11

	CGameUserListCtrlDu* m_pUserListDu;	//用户列表

	IButton* m_pShowRightFrameBtn; //显示右边框的按钮

	IContain* m_pTalkCtn;	/*百宝湾UI带的聊天容器，因有部分游戏没有右边框，但需要聊天功能。
							  所以需要在UI编辑时自定义聊天容器，容器控件ID固定为220，如果游戏不需要自定义聊天容器，则m_pTalkCtn指针将为空，
							  XLogic相关的聊天处理注意m_pTalkCtn的判空
							  JianGuankun 2013.2.26
							  */


	//原窗口尺寸
	int					m_nWiondwMetricsX;
	int					m_nWiondwMetricsY;

	long int			m_dwCurrentUserID;				//当前用户
private:
	CSkinMgr			m_skinmgr;
    CString             m_strBcfFileName;
	//保存各个视图位置玩家的视频对话框偏移量
	int					m_iVideoOffsetNum;//玩家数量
	int					m_pVideoXOffset[VIDEO_MAX_PEOPLE]; //X偏移量
	int					m_pVideoYOffset[VIDEO_MAX_PEOPLE]; //Y偏移量
};