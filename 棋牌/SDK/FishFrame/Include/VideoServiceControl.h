#ifndef VIDEO_SERVICE_CONTROL_HEAD_FILE
#define VIDEO_SERVICE_CONTROL_HEAD_FILE

#pragma once

#include "VideoServiceModule.h"
#include "DlgVideoClone.h"


//////////////////////////////////////////////////////////////////////////

//视频窗口
class VIDEO_SERVICE_CLASS CVideoServiceControl : public CWnd
{
	//友元定义
	friend class CVideoServiceManager;

	//内核变量
private:
	bool							m_bInitVideo;						//配置标志
	bool							m_bShowVideo;						//显示视频

	//用户信息
private:
	DWORD							m_dwUserID;							//用户标识
	TCHAR							m_szAccounts[LEN_NICKNAME];				//用户帐号

	//状态变量
private:
	float							m_fVolumePercent;					//语音比例
	CRect							m_rcPaintVolume;					//音波区域

	//送花变量
private:
	DWORD							m_dwTargerUserID;					//目标玩家

	//菜单变量
private:
	tagUserInfo						*m_pUserData;						//玩家信息

	//控制变量
private:
	bool							m_bSmallMode;						//小型模式
	bool							m_bLocalMode;						//本地标志
	bool							m_bEnableVideo;						//是否视频
	bool							m_bEnableAudio;						//是否音频

	//控钮控件
private:
	CSkinButton						m_btMore;							//更多按钮
	CSkinButton						m_btVideo;							//视频按钮
	CSkinButton						m_btAudio;							//音频按钮
	CSkinButton						m_btClone;							//克隆按钮
	CSkinButton						m_btFlower;							//送花按钮
	CDlgVideoClone					m_DlgVideoClone;					//克隆对话框

	//控件变量
protected:
	CToolTipCtrl					m_ToolTipCtrl;						//提示控件

	//图片资源
private:
	CBitImage						m_ImageSmallPhoto;					//缩小头像
	CBitImage						m_ImageLargePhoto;					//放大头像
	CBitImage						m_ImageSmallFrame;					//缩小框架
	CBitImage						m_ImageLargeFrame;					//放大框架
	CBitImage						m_ImageLargeVolume;					//放大框架
	CBitImage						m_ImageLargeVolumeEx;				//放大框架
	CBitImage						m_ImageSmallVolumeEx;				//放大框架
	CBitImage						m_ImageSmallVolume;					//放大框架

	//函数定义
public:
	//构造函数
	CVideoServiceControl();
	//析构函数
	virtual ~CVideoServiceControl();

	//重载函数
protected:
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//大小模式
public:
	//大小模式
	bool SetSmallMode(bool bSmallMode);
	//大小模式
	bool IsSamllMode() { return m_bSmallMode; }
	//获取大小
	void GetVideoSize(CRect &rcVideo);

	//远程用户
public:
	//远程用户
	DWORD GetVideoUserID() { return m_dwUserID; }
	//远程用户
	bool SetVideoUserInfo(DWORD dwUserID, LPCTSTR pszAccounts);

	//控制函数
public:
	//配置视频
	bool InitVideoService(bool bLocalMode, bool bSmallMode);
	//设置音波
	void SetVolume(DWORD dwCurrentVolume);

	//视频函数
public:		
	//绘制视频
	void DrawUserVideo(DWORD dwUserid, LPVOID lpBuf, DWORD dwLen, BITMAPINFOHEADER bmiHeader);
	//音量改变
	void OnWaveInVolumeChange(DWORD dwVolume);
	//音量改变
	void OnWaveOutVolumeChange(DWORD dwVolume);

	//内部函数
protected:
	//调整窗口
	VOID RectifyControl();	

	//消息映射
protected:
	//重画函数
	VOID OnPaint();
	//创建函数
	INT OnCreate(LPCREATESTRUCT lpCreateStruct);

	//重载函数
protected:
	//菜单命令
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	//事件函数
protected:
	//进入事件
	bool OnEventUserEnter();
	//离开事件
	bool OnEventUserLeave();
	//重置事件
	bool OnEventReset();
	//摄像头事件
	void OnEventCameraState(BYTE cbState);

	//按钮消息
protected:
	//更多按钮
	VOID OnBnClickedMore();
	//视频按钮
	VOID OnBnClickedVideo();
	//音频按钮
	VOID OnBnClickedAudio();
	//克隆按钮
	VOID OnBnClickedClone();
	//送花按钮
	VOID OnBnClickedFlower();
    
	//消息宏
	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
interface IClientKernel;
//视频管理
class VIDEO_SERVICE_CLASS CVideoServiceManager : public CWnd
{
	//友元定义
	friend class CVideoServiceControl;

	//视频信息
protected:
	WORD							m_wVideoPort;						//视频端口
	DWORD							m_dwVideoServer;					//视频地址
	DWORD							m_dwLocalUserID;					//本地用户
	DWORD							m_dwUserID[MAX_CHAIR];				//用户标识
	DWORD							m_dwVideoRoomID;					//房间标识

	//资源变量
protected:
	static HINSTANCE				m_hResInstance;						//资源句柄
	//static tagVideoFrameImage		m_VideoFrameImage;					//视频资源
	//static tagPropertyMenuImage		m_PropertyMenuImage;				//菜单资源

	//组件变量
protected:
	IClientKernel *					m_pIClientKernel;					//内核接口
	IStringMessage *				m_pIMessageProxy;					//消息代理
	CVideoServiceControl *			m_VideoServiceControl[MAX_CHAIR];	//视频数组

	//静态变量
protected:
	static CVideoServiceManager *	m_pVideoServiceManager;				//管理对象

	//函数定义
public:
	//构造函数
	CVideoServiceManager();
	//析构函数
	virtual ~CVideoServiceManager();

	//配置函数
public:
	//设置接口
	bool SetClientKernel(IUnknownEx * pIUnknownEx);
	//获取接口
	IClientKernel *GetClientKernel() {return m_pIClientKernel;}
	//设置接口
	bool SetMessageProxy(IUnknownEx * pIUnknownEx);
	//服务信息
	bool SetVideoServiceInfo(DWORD dwLoaclUserID, DWORD dwVideoServer, WORD wVideoPort);
	//设置对象
	bool SetVideoServiceControl(WORD wVideoIndex, CVideoServiceControl * pVideoServiceControl);
	//获取对象
	CVideoServiceControl *GetVideoServiceControl(WORD wVideoIndex);
	//登陆视频
	void LoginVideoSystem();
	//登出系统
	void LogoutVideoSystem();
	//设置资源
	static void SetSkinResource(HINSTANCE hResInstance/*,tagVideoFrameImage *pVideoFrameImage,tagPropertyMenuImage *pPropertyMenuImage*/);

	//事件接口
public:
	//重置视频
	bool OnEventGameReset();

	//辅助函数
public:
	//显示日志
	void AppendLogString(CString strLog);
	//获取索引
	WORD GetVideoIndex(DWORD dwUserID);
	//获取索引
	WORD GetVideoIndex(tagUserInfo const*pagUserData);

	//视频事件
public:
	//连接事件
	afx_msg LRESULT OnGVClientConnect(WPARAM wParam, LPARAM lParam);
	//登陆事件
	afx_msg LRESULT OnGVClientLogin(WPARAM wParam, LPARAM lParam);
	//进入事件
	afx_msg LRESULT OnGVClientEnterRoom(WPARAM wParam, LPARAM lParam);
	//列表事件
	afx_msg LRESULT OnGVClientOnlineUser(WPARAM wParam, LPARAM lParam);
	//进入事件
	afx_msg LRESULT OnGVClientUserAtRoom(WPARAM wParam, LPARAM lParam);
	//断网事件
	afx_msg LRESULT OnGVClientLinkClose(WPARAM wParam, LPARAM lParam);
	//摄像头事件
	afx_msg LRESULT OnGVClientCameraState(WPARAM wParam, LPARAM lParam);

	//静态函数
public:
	//获取实例
	static CVideoServiceManager * GetInstance() { return m_pVideoServiceManager; }

	//消息宏
	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif
