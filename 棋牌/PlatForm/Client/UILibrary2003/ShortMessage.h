#pragma once
#ifndef SHORTMESSAGE_HEAD_H
#define  SHORTMESSAGE_HEAD_H


//#include "StdAfx.h"
#include "Resource.h"
#include "Afxcmn.h"
#include "clientcomstruct.h"
#include "AFCImage.h"
#include "afcbutton.h"
#include "messagehandle.h"
#include "afcdialog.h"
#include "MessageRecordMgr.h"
#include "..\Common\IMMessage.h"
//class CGameRoomEx;
#include "SkinMgr.h"
#define  WM_GM_TALK       WM_USER + 1113 

class SendMsgUserStruct
{
public:
	SendMsgUserStruct()
	{
		ZeroMemory(this, sizeof(SendMsgUserStruct));
	}

public:
	BYTE						bDeskNO;							///游戏桌号
	BYTE						bGameMaster;						///管理等级
	long int					dwUserID;							///ID 号码
	int							dwAccID;							///ACC 号码
	char						nickName[61];						///用户昵称
};

//发送短信息类
class EXT_CLASS CShortMessage : public CGameFaceGo
{
	HBRUSH m_bkBrush;

	//变量定义
public:
	CWnd								* m_pParentWnd;						//处理窗口
		
	//聊天状态信息
	long int							m_dwLastTalkTime;					//说话时间
	CString								m_strLastTalk;						//聊天内容

	//信息变量
	long int							m_dwTargetUserID;					//对话用户
	int									m_dwTargerAccID;					//ACC 号码
	CString								m_strTargerName;					//用户名字
	CString								m_strRoomName;						//房间名字
	SendMsgUserStruct					m_TargetUserItem;				//目标用户
	bool								m_bIsInRoom;						//房间是他父窗口还是游戏是他父窗口
	long int							m_dwGamePower;
	//控件变量
public:
	CEdit								m_SendMessage;						//发送窗口
	CToolTipCtrl						m_ToolTip;							//工具提示
	CNewMiniDlg							m_RecvMessage;						//接受窗口
	CNormalBitmapButtonEx				m_BtSend;							//发送按钮
	CNormalBitmapButtonEx				m_BtExpression;						//表情按钮
    CNormalBitmapButtonEx           m_BtChatRecord;                     //聊天记录
	///CNormalBitmapButtonEx			m_BtHistory;						//历史按钮
	CNormalBitmapButtonEx				m_BtMin;							//退出按钮
	CNormalBitmapButtonEx				m_BtClose;							//退出按钮
	CNormalBitmapButtonEx				m_BtClose2;							//退出按钮
	CLookingClass						m_MessageHandle;					//消息处理
	CGameTalkText						m_ExpressWnd;						//表情窗口
	CGameImage							m_bk;
	CAFCColorResource					m_colorresource;
    CSkinMgr                                m_skinmgr;
     CMessageRecordMgr m_MsgRecordMgr;
	DECLARE_MESSAGE_MAP()
    CString                                     m_strUserAccount;              //玩家登录账户
	//函数定义
public:
	//构造函数
	CShortMessage(CWnd * pParentWnd);
	//析构函数
	virtual ~CShortMessage();
	//初始化函数
	virtual BOOL OnInitDialog();

	//重载函数
protected:
	//控件绑定函数
	virtual void DoDataExchange(CDataExchange * pDX); 
	//确定按钮
	virtual void OnOK();
	//取消按钮
	virtual void OnCancel();
	//消息解释函数
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//功能函数 
public:
	//设置信息
	void SetTalkInfo(SendMsgUserStruct * pTargerUserItem, long int dwGamePower, TCHAR * szRoomName);

	int m_iGmID;
	void SetTalkInfo(int iGmID);
	//调整位置
	void FixControlSize(int iWidth, int iHegiht);
	//收到消息
	void OnRecvMessage(TCHAR * szSendName, TCHAR * szMessage, COLORREF crColor);
    //设置登录账户
    void SetUserAccount(const CString &strUserAccount);

	//按钮函数
public:
	//表情按钮
	void OnBnClickedExpression();

    //聊天记录按钮
    void OnBnClickedRecordChat();

	//消息函数
public:
	//绘画函数
	afx_msg void OnPaint();
	//位置消息
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//发送按钮
	afx_msg void OnBnClickedSendShort();
	//历史记录
	afx_msg void OnBnClickedHistory();
	//退出按钮
	afx_msg void OnBnClickedClose();
	afx_msg void OnBnClickedMin();
	//获取位置
	afx_msg void OnGetMinMaxInfo(MINMAXINFO * lpMMI);
	//鼠标右键按下消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//表情消息
	LRESULT OnExpression(WPARAM wparam, LPARAM lparam);
	//头像消息
	LRESULT OnHitLogo(WPARAM wparam, LPARAM lparam);

	afx_msg void OnDestroy();
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	void TransUserInfo(UserItemStruct *pIn, SendMsgUserStruct *pOut);
    //换肤
    void ExchangeSkin(const CString& strUserID = "");

    //记录聊天内容
    void RecordTalkMsg(const bool bSend,const MSG_IM_S_MSG* pMsg);
};

#endif