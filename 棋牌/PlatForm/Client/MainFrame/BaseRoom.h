#pragma once
#ifndef BASEROOM_HEAD_H
#define BASEROOM_HEAD_H

#include "GamePlace.h"
#include "NewControl\\TStatic.h"
#include "SkinMgr.h"
/*******************************************************************************************************/

//连接对话框类
class CConnectDlg : public CGameFaceGo
{
	//变量定义
protected:
	CNormalBitmapButtonEx				m_Quit;								//退出按钮
	CTransparentStatic				m_Static_1;
	COLORREF						m_bkcolor;

    CGameImage  m_CbackPic;     //背景图    PengJiLin, 2010-5-14

	//函数定义
public:
	//构造函数
	CConnectDlg(); 
	//析构函数
	virtual ~CConnectDlg();
	//DDX/DDV 支持
	virtual void DoDataExchange(CDataExchange * pDX);    
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定
	virtual void OnOK() { return; }
	//取消
	virtual void OnCancel() { return; }
	//设置消息显示
	bool SetMessageText(const TCHAR * szMessage);

	//重写CenterWindow使其屏幕居中，代替父窗口剧中
	//JianGuankun,2012.7.26
	void CenterWindow(CWnd* pAlternateOwner = NULL);

private:
	CSkinMgr m_skinmgr;

private:
	LRESULT OnExchangeSkin(WPARAM wpara,LPARAM lpara);

private:
	void Init();

	void LoadSkin();
public:
	//定时器消息
	afx_msg void OnTimer(UINT nIDEvent);
	//退出按钮消息
	afx_msg void OnQuit();
	afx_msg void OnPaint();
	//控件颜色消息
	LRESULT OnControlColor(WPARAM wparam, LPARAM lparam);

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	DECLARE_MESSAGE_MAP()
};

/*******************************************************************************************************/

//房间基础类接口
interface IBaseGameRoom
{
	//函数定义
public:
	//询问关闭
	virtual bool CanCloseNow()=0;
	//关闭窗口
	virtual bool CloseGameRoom()=0;
	//取消连接消息
	virtual bool OnCancelConnectEvent()=0;
	//位置变化
	virtual bool OnSizeEvent(UINT nSizeType, int cx, int cy)=0;
	//拆分条消息
	virtual bool OnSplitEvent(UINT uSplitID, UINT uXPos, UINT uYPos)=0;
	//调整位置
	virtual void FixControlStation(int iWidth, int iHeight)=0;
};

/*******************************************************************************************************/

//房间基础类 
class CBaseRoom  : public CGameFaceGo , public IClientSocketService, public ISocketSendInterface, public IBaseGameRoom
{
	//变量定义
public:
	CString										m_strTitle;						//标题信息
	CConnectDlg									m_Connect;						//连接信息框
	CTCPClientSocket*							m_TCPSocket;					//网络 SOCKET

	//函数定义
public:
	//构造函数
	CBaseRoom(UINT uDialogTemplata); 
	//析构函数
	virtual ~CBaseRoom();

	//重载函数
public:
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual void OnOK() { return; }
	//取消函数
	virtual void OnCancel() { return; }

	//消息函数
public:
	//位置变化
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//拆分条消息
	LRESULT OnSplitMessage(WPARAM wparam, LPARAM lparam);

	//接口函数
protected:
	//发送网络消息函数
	inline virtual int SendGameData(BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode);
	//发送网络消息函数
	inline virtual int SendGameData(void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode);

	DECLARE_MESSAGE_MAP()

public:
	//发送函数
	virtual int SendData(void * pData, UINT uSize, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode);
	//简单命令发送函数
	virtual int SendData(BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode);

	virtual int CloseSocket(bool bNotify=true);
};

#endif
/*******************************************************************************************************/
