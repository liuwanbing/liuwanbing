#ifndef GAME_FRAME_HEAD_HEAD_FILE
#define GAME_FRAME_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//包含文件

//MFC 文件
#include <Afxmt.h>
#include <AtlBase.h>

//平台文件
#include "Platform.h"

//网络命令
#include "CMD_Commom.h"
#include "CMD_GameServer.h"

//进程命令
#include "IPC_GameFrame.h"

//组件文件
#include "ServiceCoreHead.h"
#include "ProcessControlHead.h"

//组件文件
#include "WHImageHead.h"
#include "GameEngineHead.h"
#include "UserServiceHead.h"
#include "SkinControlHead.h"
#include "ShareControlHead.h"
#include "PlatformDataHead.h"
#include "GamePropertyHead.h"
#include "AvatarControlHead.h"
#include "VideoServiceModule.h"

//////////////////////////////////////////////////////////////////////////////////
//公共定义

//导出定义
#ifndef GAME_FRAME_CLASS
	#ifdef  GAME_FRAME_DLL
		#define GAME_FRAME_CLASS _declspec(dllexport)
	#else
		#define GAME_FRAME_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifndef _DEBUG
	#define GAME_FRAME_DLL_NAME		TEXT("GameFrame.dll")				//组件名字
#else
	#define GAME_FRAME_DLL_NAME		TEXT("GameFrameD.dll")				//组件名字
#endif

//////////////////////////////////////////////////////////////////////////////////
//宏定义

//元素数目
#define MAX_CLOCK_ITEM				3									//时间数目

//名字大小
#define ACCOUNTS_CX					96									//帐号宽度
#define ACCOUNTS_CY					12									//帐号高度

//////////////////////////////////////////////////////////////////////////////////
//枚举定义

//绘画模式
#define DRAW_MODE_SPREAD			0									//平铺模式
#define DRAW_MODE_CENTENT			1									//居中模式
#define DRAW_MODE_ELONGGATE			2									//拉伸模式

//////////////////////////////////////////////////////////////////////////////////
//结构定义

//用户属性
struct tagUserAttribute
{
	//用户属性
	DWORD							dwUserID;							//用户标识
	WORD							wTableID;							//桌子号码
	WORD							wChairID;							//椅子号码

	//权限属性
	DWORD							dwUserRight;						//用户权限
	DWORD							dwMasterRight;						//管理权限
};

//游戏属性
struct tagGameAttribute
{
	WORD							wKindID;							//类型标识
	WORD							wChairCount;						//椅子数目
	DWORD							dwClientVersion;					//游戏版本
	TCHAR							szGameName[LEN_KIND];				//游戏名字
};

//房间属性
struct tagServerAttribute
{
	WORD							wKindID;							//类型标识
	WORD							wServerID;							//房间标识
	WORD							wServerType;						//游戏类型
	DWORD							dwServerRule;						//房间规则
	TCHAR							szServerName[LEN_SERVER];			//房间名称
	WORD							wAVServerPort;						//视频端口
	DWORD							dwAVServerAddr;						//视频地址
};

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IClientKernel INTERFACE_VERSION(1,1)
	static const GUID IID_IClientKernel={0xa6da55ae,0x784b,0x44fa,0x00be,0xad,0xd8,0xc4,0xab,0x2b,0x9c,0x27};
#else
	#define VER_IClientKernel INTERFACE_VERSION(1,1)
	static const GUID IID_IClientKernel={0xb6f72cb2,0x62a4,0x42cc,0x00b6,0x43,0x26,0x2f,0xda,0x74,0x11,0x5c};
#endif

//客户端内核
interface IClientKernel : public IUnknownEx
{
	//配置接口
public:
	//内核配置
	virtual bool InitClientKernel(LPCTSTR lpszComLine)=NULL;
	//设置接口
	virtual bool SetUserEventSink(IUnknownEx * pIUnknownEx)=NULL;
	//设置接口
	virtual bool SetStringMessage(IUnknownEx * pIUnknownEx)=NULL;
	//设置接口
	virtual bool SetClientKernelSink(IUnknownEx * pIUnknownEx)=NULL;
	//设置接口
	virtual bool SetClientPropertySink(IUnknownEx * pIUnknownEx)=NULL;
	//游戏属性
	virtual bool SetGameAttribute(WORD wKindID, WORD wPlayerCount, DWORD dwClientVersion, HICON hGameIcon, LPCTSTR pszGameName)=NULL;

	//连接接口
public:
	//连接大厅
	virtual bool CreateConnect()=NULL;
	//中断连接
	virtual bool IntermitConnect()=NULL;

	//属性接口
public:
	//用户属性
	virtual tagUserAttribute * GetUserAttribute()=NULL;
	//游戏属性
	virtual tagGameAttribute * GetGameAttribute()=NULL;
	//房间属性
	virtual tagServerAttribute * GetServerAttribute()=NULL;

	//模式接口
public:
	//单机模式
	virtual bool IsSingleMode()=NULL;
	//旁观状态
	virtual bool IsLookonMode()=NULL;
	//允许状态
	virtual bool IsAllowLookon()=NULL;
	//服务状态
	virtual bool IsServiceStatus()=NULL;

	//状态接口
public:
	//获取状态
	virtual BYTE GetGameStatus()=NULL;
	//设置状态
	virtual VOID SetGameStatus(BYTE cbGameStatus)=NULL;

	//用户接口
public:
	//自己位置
	virtual WORD GetMeChairID()=NULL;
	//自己位置
	virtual IClientUserItem * GetMeUserItem()=NULL;
	//游戏用户
	virtual IClientUserItem * GetTableUserItem(WORD wChariID)=NULL;
	//查找用户
	virtual IClientUserItem * SearchUserByUserID(DWORD dwUserID)=NULL;
	//查找用户
	virtual IClientUserItem * SearchUserByGameID(DWORD dwGameID)=NULL;
	//查找用户
	virtual IClientUserItem * SearchUserByNickName(LPCTSTR szNickName)=NULL;
	//旁观用户
	virtual IClientUserItem * EnumLookonUserItem(WORD wEnumIndex)=NULL;

	//网络接口
public:
	//发送函数
	virtual bool SendSocketData(WORD wMainCmdID, WORD wSubCmdID)=NULL;
	//发送函数
	virtual bool SendSocketData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;

	//进程接口
public:
	//发送数据
	virtual bool SendProcessData(WORD wMainCmdID, WORD wSubCmdID)=NULL;
	//发送数据
	virtual bool SendProcessData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
	//执行命令
	virtual bool SendProcessCommand(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;

	//功能接口
public:
	//发送准备
	virtual bool SendUserReady(VOID * pData, WORD wDataSize)=NULL;
	//旁观控制
	virtual bool SendUserLookon(DWORD dwUserID, bool bAllowLookon)=NULL;
	//发送表情
	virtual bool SendUserExpression(DWORD dwTargetUserID, WORD wItemIndex)=NULL;
	//发送聊天
	virtual bool SendUserChatMessage(DWORD dwTargetUserID, LPCTSTR pszChatString, COLORREF crColor)=NULL;

	//辅助接口
public:
	//激活框架
	virtual VOID ActiveGameFrame()=NULL;
	//切换椅子
	virtual WORD SwitchViewChairID(WORD wChairID)=NULL;

	//时钟接口
public:
	//时钟标识
	virtual UINT GetClockID()=NULL;
	//时钟位置
	virtual WORD GetClockChairID()=NULL;
	//删除时钟
	virtual VOID KillGameClock(WORD wClockID)=NULL;
	//设置时钟
	virtual VOID SetGameClock(WORD wChairID, WORD wClockID, UINT nElapse)=NULL;

	//操作接口
public:
	//信息接口
	virtual IStringMessage * GetIStringMessage()=NULL;
	//设置事件
	virtual bool OnGameOptionChange()=NULL;
};

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IClientKernelSink INTERFACE_VERSION(1,1)
	static const GUID IID_IClientKernelSink={0xfec3bd90,0x2ba4,0x4dc4,0x00ae,0x2c,0xd7,0x5b,0x0c,0xe1,0x59,0xed};
#else
	#define VER_IClientKernelSink INTERFACE_VERSION(1,1)
	static const GUID IID_IClientKernelSink={0xae401d54,0xaf7f,0x4b46,0x00ab,0x5c,0x59,0x8b,0x8c,0x4a,0x31,0x7e};
#endif

//内核回调
interface IClientKernelSink : public IUnknownEx
{
	//控制接口
public:
	//重置游戏
	virtual VOID ResetGameClient()=NULL;
	//关闭游戏
	virtual VOID CloseGameClient()=NULL;

	//配置接口
public:
	//配置完成
	virtual bool OnSchemeFinish()=NULL;
	//列表配置
	virtual bool SchemeColumnItem(tagColumnItem ColumnItem[], BYTE cbColumnCount)=NULL;
	//道具配置
	virtual bool SchemePropertyInfo(tagPropertyInfo PropertyInfo[], BYTE cbPropertyCount)=NULL;

	//内核事件
public:
	//游戏消息
	virtual bool OnEventSocket(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
	//进程消息
	virtual bool OnEventProcess(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;

	//游戏事件
public:
	//旁观消息
	virtual bool OnEventLookonMode(VOID * pData, WORD wDataSize)=NULL;
	//场景消息
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonUser, VOID * pData, WORD wDataSize)=NULL;

	//时钟事件
public:
	//时钟删除
	virtual bool OnEventGameClockKill(WORD wChairID)=NULL;
	//时钟信息
	virtual bool OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD wClockID)=NULL;

	//设置事件
public:
	//设置事件
	virtual bool OnGameOptionChange()=NULL;
};

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IUserEventSink INTERFACE_VERSION(1,1)
	static const GUID IID_IUserEventSink={0x6cfedee7,0x4acb,0x4ae1,0x84,0x92,0xe8,0x49,0x87,0x60,0xff,0x6f};
#else
	#define VER_IUserEventSink INTERFACE_VERSION(1,1)
	static const GUID IID_IUserEventSink={0x3719eae7,0xd137,0x4036,0x97,0xa0,0x72,0x67,0x26,0x25,0x43,0x01};
#endif

//用户事件
interface IUserEventSink : public IUnknownEx
{
	//用户事件
public:
	//用户进入
	virtual VOID OnEventUserEnter(IClientUserItem * pIClientUserItem, bool bLookonUser)=NULL;
	//用户离开
	virtual VOID OnEventUserLeave(IClientUserItem * pIClientUserItem, bool bLookonUser)=NULL;
	//用户积分
	virtual VOID OnEventUserScore(IClientUserItem * pIClientUserItem, bool bLookonUser)=NULL;
	//用户状态
	virtual VOID OnEventUserStatus(IClientUserItem * pIClientUserItem, bool bLookonUser)=NULL;
	//用户属性
	virtual VOID OnEventUserAttrib(IClientUserItem * pIClientUserItem, bool bLookonUser)=NULL;
	//用户头像
	virtual VOID OnEventCustomFace(IClientUserItem * pIClientUserItem, bool bLookonUser)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IGameFrameWnd INTERFACE_VERSION(1,1)
	static const GUID IID_IGameFrameWnd={0xb6ac5ec5,0x26f9,0x47dd,0xb5,0x0f,0x4f,0x73,0xa1,0x3e,0xbc,0xc9};
#else
	#define VER_IGameFrameWnd INTERFACE_VERSION(1,1)
	static const GUID IID_IGameFrameWnd={0x4224b444,0x7ed3,0x44d3,0xa9,0x77,0x0b,0xdc,0xae,0xe2,0x56,0x08};
#endif

//游戏框架
interface IGameFrameWnd : public IUnknownEx
{
	//窗口控制
public:
	//游戏规则
	virtual bool ShowGameRule()=NULL;
	//最大窗口
	virtual bool MaxSizeWindow()=NULL;
	//还原窗口
	virtual bool RestoreWindow()=NULL;

	//控制接口
public:
	//声音控制
	virtual bool AllowGameSound(bool bAllowSound)=NULL;
	//旁观控制
	virtual bool AllowGameLookon(DWORD dwUserID, bool bAllowLookon)=NULL;
	//控制接口
	virtual bool OnGameOptionChange()=NULL;
};

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IGameFrameView INTERFACE_VERSION(1,1)
	static const GUID IID_IGameFrameView={0x8b8011d9,0x2e83,0x419c,0xa5,0x90,0x2f,0xe5,0xcf,0xb6,0xdb,0x58};
#else
	#define VER_IGameFrameView INTERFACE_VERSION(1,1)
	static const GUID IID_IGameFrameView={0xf940a4c3,0x926d,0x4b01,0x82,0xc9,0xaf,0x1b,0xab,0x3f,0x1e,0x1f};
#endif

//视图接口
interface IGameFrameView : public IUnknownEx
{
	//窗口接口
public:
	//窗口句柄
	virtual HWND GetGameViewHwnd()=NULL;
	//创建窗口
	virtual bool CreateGameViewWnd(CWnd * pParentWnd, UINT nWndID)=NULL;

	//控制接口
public:
	//切换状态
	virtual VOID SwitchToReadyStatus()=NULL;

	//时钟管理
public:
	//获取时钟
	virtual WORD GetUserClock(WORD wChairID)=NULL;
	//设置时钟
	virtual VOID SetUserClock(WORD wChairID, WORD wUserClock)=NULL;

	//用户管理
public:
	//获取用户
	virtual IClientUserItem * GetClientUserItem(WORD wChairID)=NULL;
	//设置用户
	virtual VOID SetClientUserItem(WORD wChairID, IClientUserItem * pIClientUserItem)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IGameFrameControl INTERFACE_VERSION(1,1)
	static const GUID IID_IGameFrameControl={0xf908a318,0x3729,0x4fbc,0x97,0x09,0xc1,0x7b,0x3c,0x04,0x66,0xcc};
#else
	#define VER_IGameFrameControl INTERFACE_VERSION(1,1)
	static const GUID IID_IGameFrameControl={0xab9c1739,0x510e,0x4ef3,0x84,0xe3,0xb7,0xe0,0x82,0xf1,0x2b,0x84};
#endif

//控制框架
interface IGameFrameControl : public IUnknownEx
{
	//控制接口
public:
	//重置控件
	virtual VOID ResetControl()=NULL;
	//设置广告
	virtual VOID SetGameBillInfo(WORD wKindID, WORD wServerID)=NULL;
	//设置用户
	virtual VOID SetUserViewInfo(IClientUserItem * pIClientUserItem)=NULL;
	//列表配置
	virtual VOID SetColumnDescribe(tagColumnItem ColumnItem[], BYTE cbColumnCount)=NULL;
	//道具配置
	virtual VOID SetPropertyDescribe(tagPropertyInfo PropertyInfo[], BYTE cbPropertyCount)=NULL;

	//菜单接口
public:
	//显示菜单
	virtual VOID TrackUserItemMenu(IClientUserItem * pIClientUserItem)=NULL;
	//显示菜单
	virtual VOID TrackUserItemMenu(IClientUserItem * pIClientUserItem[], WORD wItemCount)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IGameFrameToolBar INTERFACE_VERSION(1,1)
	static const GUID IID_IGameFrameToolBar={0x42f11860,0x4f2b,0x4d09,0xa1,0x4a,0xbd,0x44,0x5a,0xd7,0xbd,0xa5};
#else
	#define VER_IGameFrameToolBar INTERFACE_VERSION(1,1)
	static const GUID IID_IGameFrameToolBar={0xa51ecb9c,0x2a15,0x4243,0xa7,0x6d,0x5d,0x45,0x73,0x62,0xd1,0x35};
#endif

//工具框架
interface IGameFrameToolBar : public IUnknownEx
{
};

/////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IClientPropertySink INTERFACE_VERSION(1,1)
	static const GUID IID_IClientPropertySink={0x208765ba, 0x482c, 0x494b, 0xa7, 0x63, 0x6, 0xe3, 0x40, 0x9b, 0xcd, 0xca};
#else
	#define VER_IClientPropertySink INTERFACE_VERSION(1,1)
	static const GUID IID_IClientPropertySink={0xa41e3947, 0x6192, 0x4a32, 0xb1, 0x54, 0xe0, 0x19, 0xde, 0x62, 0x23, 0x1};
#endif

//道具接口
interface IClientPropertySink : public IUnknownEx
{
	//道具成功
	virtual VOID OnPropertySuccess(LPCTSTR szSourceNickName,DWORD dwSourceUserID,DWORD dwTargerUserID,WORD wPropertyIndex,WORD wPropertyCount)=NULL;
	//道具失败
	virtual VOID OnPropertyFailure( LONG lErrorCode,LPCTSTR szDescribeString)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IGameFrameEngine INTERFACE_VERSION(1,1)
	static const GUID IID_IGameFrameEngine={0xaea43c61,0x98e5,0x429c,0x88,0x7b,0x07,0x94,0xec,0x19,0x4e,0x1a};
#else
	#define VER_IGameFrameEngine INTERFACE_VERSION(1,1)
	static const GUID IID_IGameFrameEngine={0x917a4fe1,0x9754,0x4dad,0x86,0x82,0xdb,0x30,0x55,0x6f,0xb3,0x8e};
#endif

//框架引擎
interface IGameFrameEngine : public  IUnknownEx
{
	//系统事件
public:
	//创建函数
	virtual bool OnInitGameEngine()=NULL;
	//重置函数
	virtual bool OnResetGameEngine()=NULL;

	//时钟事件
public:
	//时钟删除
	virtual bool OnEventGameClockKill(WORD wChairID)=NULL;
	//时钟信息
	virtual bool OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD wClockID)=NULL;

	//游戏事件
public:
	//旁观消息
	virtual bool OnEventLookonMode(VOID * pData, WORD wDataSize)=NULL;
	//游戏消息
	virtual bool OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
	//场景消息
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonUser, VOID * pData, WORD wDataSize)=NULL;
	//银行消息
	virtual bool OnEventInsureMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;

	//用户事件
public:
	//用户进入
	virtual VOID OnEventUserEnter(IClientUserItem * pIClientUserItem, bool bLookonUser)=NULL;
	//用户离开
	virtual VOID OnEventUserLeave(IClientUserItem * pIClientUserItem, bool bLookonUser)=NULL;
	//用户积分
	virtual VOID OnEventUserScore(IClientUserItem * pIClientUserItem, bool bLookonUser)=NULL;
	//用户状态
	virtual VOID OnEventUserStatus(IClientUserItem * pIClientUserItem, bool bLookonUser)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IGameFrameService INTERFACE_VERSION(1,1)
	static const GUID IID_IGameFrameService={0x75f319b5,0xcfa5,0x49b0,0x9a,0x30,0x92,0x6a,0xa7,0x23,0x5e,0x8c};
#else
	#define VER_IGameFrameService INTERFACE_VERSION(1,1)
	static const GUID IID_IGameFrameService={0x06d5798d,0x6cc1,0x45c2,0xb6,0xe3,0x4d,0x4a,0xfa,0xa0,0x5a,0x83};
#endif

//框架服务
interface IGameFrameService : public IUnknownEx
{
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IGameOptionItem INTERFACE_VERSION(1,1)
static const GUID IID_IGameOptionItem={0x6b0ced33,0xac9e,0x4a02,0x0088,0x53,0x13,0x84,0x45,0xe2,0xd1,0xfd};
#else
#define VER_IGameOptionItem INTERFACE_VERSION(1,1)
static const GUID IID_IGameOptionItem={0x415c531b,0xc84e,0x4f35,0x0084,0xdc,0x23,0x75,0x45,0xf1,0x10,0x78};
#endif

//配置接口
interface IGameOptionItem : public IUnknownEx
{
	//窗口接口
public:
	//获取句柄
	virtual HWND __cdecl GetWindowHwnd()=NULL;
	//创建控件
	virtual bool __cdecl CreateOptionItem(CWnd * pParentWnd)=NULL;

	//扩展接口
public:
	//调整控件
	virtual VOID __cdecl RectifyControl(INT nWidth, INT nHeight)=NULL;
	//绘画控件
	virtual VOID __cdecl DrawControlFace(CDC * pDC, INT nWidth, INT nHeight)=NULL;

	//参数接口
public:
	//保存配置
	virtual bool __cdecl SaveParameter()=NULL;
	//默认参数
	virtual bool __cdecl DefaultParameter()=NULL;

	//信息接口
public:
	//获取标题
	virtual LPCTSTR __cdecl GetItemTitle()=NULL;
	//获取位置
	virtual VOID __cdecl GetControlMinSize(CSize & ControlSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////
//组件模块

DECLARE_MODULE_HELPER(ClientKernel,GAME_FRAME_DLL_NAME,"CreateClientKernel")

//////////////////////////////////////////////////////////////////////////////////
//导出文件

#ifndef GAME_FRAME_DLL
	#include "GlobalUnits.h"
	#include "GameFrameApp.h"
	#include "GameFrameWnd.h"
	#include "GameFrameView.h"
	#include "GameFrameEngine.h"
	#include "GameFrameAvatar.h"
	#include "GameFrameService.h"
	#include "GameFrameControl.h"
	#include "GameFrameViewD3D.h"
	#include "GameFrameViewGDI.h"
	#include "GameFrameUserView.h"
	#include "GameFrameMedalView.h"
	#include "GameFramePublicize.h"
#endif

//////////////////////////////////////////////////////////////////////////////////

#endif