#pragma once

#ifndef CPLAYVIEW_HEAD_FILE
#define CPLAYVIEW_HEAD_FILE

#include "PublicDefine.h"
#include "AFCResource.h"
#include "ClientComStruct.h"
#include "ShowScore.h"
#include "objectfactory.h"
#include "ContestInfoWnd.h"
#include "CommonDefine.h"
/*******************************************************************************************************/
 
//框架大小 
#define VIEW_FRAME_TOP					0								//框架上高
#define VIEW_FRAME_LEFT					18								//框架左宽
#define VIEW_FRAME_RIGHT				18								//框架右宽
#define VIEW_FRAME_BOTTOM				18								//框架下高

//定时器大小
#define TIME_WIDTH						30								//时间宽度
#define TIME_HEIGHT						21								//时间高度								 

//同意标志大小
#define AGREE_WIDTH						13								//同样宽度
#define AGREE_HEIGHT					13								//同样高度

//定时器消息
#define THREAD_TIME_ID					1								//线程定时器 ID

#define GIF_COUNT						4								//gif动画总数

//#define PLAYVIEW_LBUTTON_DOWN			WM_USER + 3421					//add by zxj 2008/04/09
//#define PROP_SELECT_CURSOR				WM_USER + 3562					//add by zxj 2008/04/09	
/*******************************************************************************************************/

//绘画信息类
class EXT_CLASS CNewCDC//C-ThreadDraw
{
	//变量定义
private:
	bool								m_bQuit;						//是否退出
	int									m_iSpeed;						//显示帧数
	int									m_iTimeSpace;					//睡眠时间
	int									m_iGDISleepTime;				//睡眠时间
	HANDLE								m_hGDIThreadHandle;				//线程句柄
	HANDLE								m_hTimeThreadHandle;			//线程句柄

	//位置信息（主线程设置）
protected:
	int									m_iXPos;						//X 点
	int									m_iYPos;						//Y 点
	int									m_iDrawWidth;					//视图宽度
	int									m_iDrawHeight;					//视图高度
	CWnd								* const m_pGameView;			//游戏视图

	//函数定义
public:
	//构造函数
	CNewCDC(CWnd * pGameView);
	//析构函数
	virtual ~CNewCDC();

	//重载函数
public:
	//初始化绘图数据
	virtual bool InitDraw(CDC * pDrawDC);
	//取消初始化数据
	virtual bool UnInitDraw();
	//绘画界面函数
	virtual void ThreadDrawView(CDC * pDC, int iDrawWith, int iDrawHeight)=0;

	//功能函数
public:
	//获取帧数
	int GetDisplaySpeed() { return m_iSpeed; }
	//设置定时器间隔
	void SetTimeSpace(int iTimeSpace) { m_iTimeSpace=iTimeSpace; };
	//设置 GDI 线程间隔
	void SetGDISleepTime(int iGDISleepTime) { m_iGDISleepTime=iGDISleepTime; };
	//设置范围
	void SetDrawRange(int iXPos, int iYPos, int iDrawWidth, int iDrawHeight);

	//线程函数
public:
	//开始线程
	bool CreateDrawThread();
	//结束线程
	bool EndDrawThread();
	//停止线程
	void SuspendThread();
	//启动线程
	void ResumeThread();
	//定时器线程
	static unsigned __stdcall TimerThread(LPVOID pThreadData);
	//画图线程
	static unsigned __stdcall GDIModeThread(LPVOID pThreadData);
};

/*******************************************************************************************************/

//视图基础类
class EXT_CLASS CUsingSkinClass/*C-GDIPlayView*/ : public CWnd
{
	//友元定义
	friend class CNewCDC;
	HBRUSH m_bkBrush;
	//绘画信息
private:
	bool							m_bReDraw;							//是否重画
	BITMAP							m_PicInfo;							//位图信息
	CBitmap							m_PicBuffer;						//缓冲位图
	const bool						m_bDoubleBuf;						//缓冲标志
	const int						m_iBufferColor;						//缓冲色深
	CNewCDC						* const m_pDrawInfoPtr;				//绘图信息
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	COLORREF						m_bkColor;
protected:
	CString							m_strTitle;							//窗口标题
	CGameImage						m_PicGameView;						//游戏底图
	CGameImage						m_PicGameViewLogo;					//游戏底图LOGO

	

	//绘画参数（拉伸，居中，平铺）
	enum DrawMode { DM_ELONGATE, DM_CENTENT, DM_SPREAD };

	//界面图
private:
	static CGameImage				m_PicAgree;							//同意图案
	static CGameImage				m_PicTimer;							//时间数字
	static CGameImage               m_PicTimerFrame;                    //时间框
	static CGameImage				m_PicTop;							//框架图片
	static CGameImage				m_PicBottom;						//框架图片
	static CGameImage				m_PicLeft;							//框架图片
	static CGameImage				m_PicRight;							//框架图片
	static CGameImage				m_PicTopRight;						//框架图片
	static CGameImage				m_PicTopLeft;						//框架图片
	static CGameImage				m_PicBottomRight;					//框架图片
	static CGameImage				m_PicBottomLeft;					//框架图片
public:
	CGameImage						m_iUserFrame;						//人物头像图片
	//{add by huangyin
	//int								nLogoPositionInformation[MAX_PEOPLE][5];
	//[][0]:ID
	//[][1]:Left
	//[][2]:Top
	//[][3]:Width
	//[][4]:Height
//	LRESULT	OnPropSelectCursorChange(WPARAM wParam, LPARAM lParam);
//	HCURSOR	m_hCursor;
	//end add}
	//界面信息
protected:	
	UserItemStruct					* m_pUserItem[MAX_PEOPLE];			//用户信息
	GameInfoStructInExe					* m_pGameInfo;

	//元素位置
public:
	CShowScore						m_showcoredlg;
	CContestInfoWnd					m_ContestInfoWnd;

	UINT							m_uTime[MAX_PEOPLE];				//用户时间
	CPoint							m_PtLogo[MAX_PEOPLE];				//图标位置
	CPoint							m_PtName[MAX_PEOPLE];				//名字位置
	CPoint							m_PtTime[MAX_PEOPLE];				//时间位置
	CPoint							m_PtAgree[MAX_PEOPLE];				//同意位置
	int								m_ShowType;							//显示方式
	bool							m_bShowRight;						//是否显示右边控件
	int								m_vlistx;							//隐藏按钮的位置
	CGameImage						m_bkimgs;							//显示右边时的图片
	CGameImage						m_bkimgh;							//隐藏右边时的图片
	//函数定义
protected:
	//构造函数
	CUsingSkinClass(bool bDoubleBuf=true, int iBufferColor=16, CNewCDC * pDrawInfoPtr=NULL);
	//析构函数
	virtual ~CUsingSkinClass();

	//重载函数
public:
	//重新设置界面
	virtual void ResetGameView();
	//绘画界面函数
	virtual void DrawViewFace(CDC * pDC, int iWidth, int iHeight);
	//更新用户资料
	virtual void UpdateUserInfo(BYTE bViewStation)=0;
	//调整位置
	virtual void FixControlSize(int iWidth, int iHeight)=0;

	//绘画函数 
public:
	//绘画背景
	void DrawBackPic(CDC * pDC, DrawMode Mode, int iWidth, int iHeight);
	//绘画同意标志
	void DrawArgeeFlag(CDC * pDC, int iXPos, int iYPos);
	//绘画时间
	void DrawTimer(CDC * pDC, int iXPos, int iYPos, UINT uTime, int iNewXPos = 12, int iNewYPos = 15);
	//绘画头像
	void DrawLogo(CDC * pDC, UINT uLogoID, int iXPos, int iYPos, bool bLost,long dwUserID =0,BOOL bIsSmall = false,bool bRightTop=false);
	//绘制头像框
	virtual void DrawLogoFrame(CDC *pDC,int iXPos,int iYPos, bool bIsDraw = false);
	//绘画框架
	void DrawViewFrame(CDC * pDC, int iWidth, int iHeight);
	//透明绘画
	bool StretchBlt(CDC * pDesDC, int iXDes, int iYDes, int iDesWidth, int iDesHeight, CDC * pScrDC, int iXScr, int iYScr, int iScrWidth, int iScrHeight, COLORREF crColor);
	//绘画透明图
	bool StretchImage(CDC * pDesDC, int iXDes, int iYDes, int iDesWidth, int iDesHeight, HBITMAP hBitBmp, int iXScr, int iYScr, int iScrWidth, int iScrHeight, COLORREF crColor);

	//功能函数 
public:
	//更新界面
	void UpdateViewFace(const RECT * pRect);
	//重画窗口
	void ReDrawViewFace(const RECT * pRect);
	//设置窗口标题
	void SetViewTitle(const TCHAR * szTitle);
	//设置显示时间
	virtual bool SetUserTimer(BYTE bViewStation, UINT uTime);   //yjj 
	//设置用户信息
	bool SetUserInfo(BYTE bViewStation, UserItemStruct * pUserItem);
	bool SetGameInfo(GameInfoStructInExe* pGameInfo);
	//移动窗口
	bool SafeMoveWindow(UINT uID, int x, int y, int iWidth, int iHeight);
	//移动窗口
	bool SafeMoveWindow(CWnd * pControlWnd, int x, int y, int iWidth, int iHeight);

	//消息函数
public:
	//绘画函数
	afx_msg void OnPaint();
	//位置变化
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//建立通知函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//窗口销毁
	afx_msg void OnDestroy();
	//鼠标右键按下
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//设置frame图片
	void SetFrameImage(TCHAR *path) ;
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);	
	int						m_iGifDeskStation;					//当前播放动画位置
	IGif					*m_pLoadGif;						//动画
	IGif					*m_pGif;							//当前动画指针
	int						m_iGifLifeTime;						//动画生存期
	//初始化gif	
	virtual void InitGif();
	//设置gif
	virtual void SetGif(BYTE bDeskView,int iValue,int iTimer);
	//绘制gif
	virtual void DrawGif(CDC *pDC,int ix,int iy);

	//获取玩家昵称颜色
	virtual COLORREF GetNameColor(UserInfoStruct *pUserInfo);
	DECLARE_MESSAGE_MAP()
//public:
//	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
//public:
//	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
public:
	////////////////////////////////
	/////Kylin 20090120 清理动画
	//清理Gif动画播放
	void ClearGif();
	////////////////////////////////

private:
	int						m_iFrameCount;						//动画帧数
public:
	void CountFrame();

	int GetFrameCount() const;
};

/*******************************************************************************************************/

#endif