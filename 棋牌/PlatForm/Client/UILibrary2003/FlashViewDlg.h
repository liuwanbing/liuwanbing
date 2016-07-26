#pragma once

#ifndef CFLASHVIEW_HEAD_FILE
#define CFLASHVIEW_HEAD_FILE

#include "PublicDefine.h"
#include "AFCDialog.h"

//using namespace AFC::AFCDialog;

/**************************************************************************************************/

//动画控件
class EXT_CLASS CGoAnimationWnd/*CFlashView*/ : public CWnd
{
	//函数定义
public:
	CLSID const& GetClsid()
	{
		static CLSID const clsid
			= { 0xd27cdb6e, 0xae6d, 0x11cf, { 0x96, 0xb8, 0x44, 0x45, 0x53, 0x54, 0x0, 0x0 } };
		return clsid;
	}
	virtual BOOL Create(LPCTSTR lpszClassName,
		LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT& rect,
		CWnd* pParentWnd, UINT nID,
		CCreateContext* pContext = NULL)
	{ return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID); }

	BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT& rect, CWnd* pParentWnd, UINT nID,
		CFile* pPersist = NULL, BOOL bStorage = FALSE,
		BSTR bstrLicKey = NULL)
	{ return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID,
	pPersist, bStorage, bstrLicKey); }

	//函数定义
public:
	long GetReadyState();
	long GetTotalFrames();
	BOOL GetPlaying();
	void SetPlaying(BOOL bNewValue);
	long GetQuality();
	void SetQuality(long nNewValue);
	long GetScaleMode();
	void SetScaleMode(long nNewValue);
	long GetAlignMode();
	void SetAlignMode(long nNewValue);
	long GetBackgroundColor();
	void SetBackgroundColor(long nNewValue);
	BOOL GetLoop();
	void SetLoop(BOOL bNewValue);
	CString GetMovie();
	void SetMovie(LPCTSTR lpszNewValue);
	long GetFrameNum();
	void SetFrameNum(long nNewValue);
	void SetZoomRect(long left, long top, long right, long bottom);
	void Zoom(long factor);
	void Pan(long x, long y, long mode);
	void Play();
	void Stop();
	void Back();
	void Forward();
	void Rewind();
	void StopPlay();
	void GotoFrame(long FrameNum);
	long CurrentFrame();
	BOOL IsPlaying();
	long PercentLoaded();
	BOOL FrameLoaded(long FrameNum);
	long FlashVersion();
	CString GetWMode();
	void SetWMode(LPCTSTR lpszNewValue);
	CString GetSAlign();
	void SetSAlign(LPCTSTR lpszNewValue);
	BOOL GetMenu();
	void SetMenu(BOOL bNewValue);
	CString GetBase();
	void SetBase(LPCTSTR lpszNewValue);
	CString GetScale();
	void SetScale(LPCTSTR lpszNewValue);
	BOOL GetDeviceFont();
	void SetDeviceFont(BOOL bNewValue);
	BOOL GetEmbedMovie();
	void SetEmbedMovie(BOOL bNewValue);
	CString GetBGColor();
	void SetBGColor(LPCTSTR lpszNewValue);
	CString GetQuality2();
	void SetQuality2(LPCTSTR lpszNewValue);
	void LoadMovie(long layer, LPCTSTR url);
	void TGotoFrame(LPCTSTR target, long FrameNum);
	void TGotoLabel(LPCTSTR target, LPCTSTR label);
	long TCurrentFrame(LPCTSTR target);
	CString TCurrentLabel(LPCTSTR target);
	void TPlay(LPCTSTR target);
	void TStopPlay(LPCTSTR target);
	void SetVariable(LPCTSTR name, LPCTSTR value);
	CString GetVariable(LPCTSTR name);
	void TSetProperty(LPCTSTR target, long property, LPCTSTR value);
	CString TGetProperty(LPCTSTR target, long property);
	void TCallFrame(LPCTSTR target, long FrameNum);
	void TCallLabel(LPCTSTR target, LPCTSTR label);
	void TSetPropertyNum(LPCTSTR target, long property, double value);
	double TGetPropertyNum(LPCTSTR target, long property);
	double TGetPropertyAsNumber(LPCTSTR target, long property);
	CString GetSWRemote();
	void SetSWRemote(LPCTSTR lpszNewValue);
	CString GetFlashVars();
	void SetFlashVars(LPCTSTR lpszNewValue);
	CString GetAllowScriptAccess();
	void SetAllowScriptAccess(LPCTSTR lpszNewValue);
};

/**************************************************************************************************/

//FALSH 对话框
class EXT_CLASS CIEHelper/*CFlashViewDlg*/ : public CGameFaceGo
{
	//变量定义
public:
	CGoAnimationWnd							m_FlashView;						//FLASH 窗口

	//函数定义
public:
	//构造函数
	CIEHelper(CWnd * pParent=NULL);
	//析构函数
	virtual ~CIEHelper();

	DECLARE_MESSAGE_MAP()

	//重载函数
protected:
	//倥件绑定函数
	virtual void DoDataExchange(CDataExchange * pDX);
	//回车信息
	virtual void OnOK() { return; };
	//退出程序
	virtual void OnCancel() { return; };

	//功能函数
public:
	//建立函数
	bool CreateFalsh(CWnd * pParent);
	//获取 FALSH 窗口
	CGoAnimationWnd & GetFlashControl() { return m_FlashView; };

	//消息函数
public:
	//位置函数
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

/**************************************************************************************************/

#endif