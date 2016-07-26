#if !defined(BRGAMEVIDEOSDK_H__INCLUDED_)
#define BRGAMEVIDEOSDK_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BRGameVideoSDK.h : header file
//
#include "brgamevideoclient.h"
#include "SDK/GVSDK.h"
#include "SDK/GVClientStyleDefine.h"
#include <list>

/**
 *	AnyChat语音视频互动平台SDK套件包装类
 *	该类封装了SDK的相关操作，如果某些方法SDK套件有，而该类没有，请自行加上
 *	外部可以根据需要适当的修改该类的部分代码，实现与上层系统的融合
 *	详细内容可参考：《BRGameVideoSDK类使用指南》
 */

class CBRGameVideoSDK
{
// Construction
public:
	CBRGameVideoSDK();
	virtual ~CBRGameVideoSDK();

private:
	CBRGameVideoClient*		m_pBRGameVideoClient;		///< 游戏视频客户端对象指针



private:
	// 设置视频窗体风格
	void SetVideoFrameStyle(void);


public:
	// 退出与某用户的私聊，或者将某用户从自己的私聊列表中清除
	long PrivateChatExit(long userid);
	// 回复对方的私聊请求
	long PrivateChatEcho(long userid, long requestid, long bAccept);
	// 请求与对方私聊，向对方发起私聊请求
	long PrivateChatRequest(long userid);
	// 获取指定用户当前的聊天模式
	long GetUserChatMode(long userid);
	// 更改当前聊天模式
	void ChangeChatMode(DWORD chatmode);

	// 给指定用户添加财富菜单项
	void AppendFortuneMenuItem(long userid, LPCTSTR menuText, LPCTSTR toolTip, short menuTag, short imageIndex);
	//重置（清空）指定用户的财富菜单
	void ResetFortuneMenu(long userid);
	// 设置财富菜单项图标
	BOOL SetFortuneImage(LPCTSTR lpImageFile);

	// 得到指定ID的用户名
	CString GetUserNameById(int id);
	// 获取当前房间在线用户列表
	INT GetRoomUserList(std::list<INT>& userlist);
	/// 刷新指定的面板
	void RefreshUserPanel(int id);
	// 设置用户的个性图片
	void SetUserPicture(INT userid, LPCSTR pathname);
	// 设置用户面板标题内容
	void SetUserTitle(INT userid, LPCTSTR lpTitleStr);
	// 显示（隐藏）用户视频窗体
	void ShowUserVideo(INT userid, short bShow, CRect rc, CWnd* pParentWnd);
	// 显示语音视频设置面板
	void ShowSettingPanel(void);
	// 获取本地摄像头状态
	INT GetLocalCameraState(void);
	// 关闭本地摄像头
	void CloseLocalCamera(void);
	// 打开本地摄像头
	void OpenLocalCamera(void);
	// 关闭本地Mic
	void CloseLocalMic(void);
	// 打开本地Mic
	void OpenLocalMic(void);
	// 离开房间
	void LeaveRoom(INT roomid=-1);
	// 进入房间
	void EnterRoom(INT roomid, LPCSTR roompass=NULL);
	// 从服务器注销
	void LogoutSystem(void);
	// 登录服务器
	void LoginSystem(LPCSTR username=NULL, LPCSTR password=NULL);
	// 连接到服务器
	void ConnectToServer(LPCSTR ipaddr, DWORD port=8902);
	// 关闭SDK
	void CloseGameVideoSDK(void);
	// 初始化SDK
	BOOL InitGameVideoSDK(CWnd* pParent, HWND hMsgWnd=NULL);
};



#endif // !defined(BRGAMEVIDEOSDK_H__INCLUDED_)
