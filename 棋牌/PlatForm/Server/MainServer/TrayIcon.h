#pragma once

//#ifndef CTRAYICON_H_2003
//#define CTRAYICON_H_2003
const UINT WM_CREATETRAYBAR =  RegisterWindowMessage(_T("TaskbarCreated"));
//任务栏图标类
class CTrayIcon
{
	//变量定义
protected:
	NOTIFYICONDATA				m_NotifyData;			//通知消息

	//函数定义
public:
	//构造函数
	explicit CTrayIcon(UINT uID);
	//析构函数
	~CTrayIcon();
	//设置消息处理窗口
	void SetNotifyWnd(CWnd * pNotifyWnd, UINT uMessage);

	//功能函数
public:
	//设置图标
	BOOL SetIcon(HICON hIcon, LPCTSTR szTip,BOOL bForce=FALSE);
	//显示提示
	BOOL ShowBalloonTip(LPCTSTR szMesssage, LPCTSTR szTitle, UINT uTimeout=5, DWORD dwInfoFlags=NIIF_INFO);
};

//#endif 