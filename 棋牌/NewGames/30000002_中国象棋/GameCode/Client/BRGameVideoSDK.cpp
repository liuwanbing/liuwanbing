// BRGameVideoSDK.cpp : implementation file
//
#include "StdAfx.h"
#include <afxwin.h>
#include "BRGameVideoSDK.h"



/////////////////////////////////////////////////////////////////////////////
// CBRGameVideoSDK

CBRGameVideoSDK::CBRGameVideoSDK()
{
	m_pBRGameVideoClient = NULL;
}

CBRGameVideoSDK::~CBRGameVideoSDK()
{
}

/**
 *	初始化SDK，该方法必须在其它方法被调用之前调用
 *	@param pParent 父窗体指针，不能为空
 *	@param hMsgWnd 消息接口窗口句柄，默认为空，表示消息发送到父窗体
 *	@return 初始化成功返回TRUE，否则返回FALSE，可能的原因是SDK插件没有注册
 */
BOOL CBRGameVideoSDK::InitGameVideoSDK(CWnd* pParent, HWND hMsgWnd)
{
	if(!pParent)
		return FALSE;
	if(m_pBRGameVideoClient)		///< 已经初始化
		return TRUE;

	BOOL rc = FALSE;

	m_pBRGameVideoClient = new CBRGameVideoClient;
	if(m_pBRGameVideoClient->Create(NULL,0,CRect(0,0,0,0),pParent,0))
	{
		m_pBRGameVideoClient->ShowWindow(SW_HIDE);
		// 打开日志记录 
		m_pBRGameVideoClient->ActiveCallLog(TRUE);
		// 设置消息通知接收句柄
		long* phWnd = hMsgWnd ? (long*)&hMsgWnd : (long*)&pParent->m_hWnd;
		m_pBRGameVideoClient->SetMessageNotifyHandle(phWnd);
		// 设置财富图标文件
		m_pBRGameVideoClient->SetFortuneImage(NULL);
		
		// 设置录像临时文件保存路径
		CHAR szRecordDirectory[MAX_PATH] = {0};
		::GetModuleFileName(NULL,szRecordDirectory,MAX_PATH);
		(strrchr(szRecordDirectory,'\\'))[1] = 0; 
		strcat(szRecordDirectory,"Record");
		m_pBRGameVideoClient->SetRecordFileSavePath(szRecordDirectory);
		
		// 设置快照临时文件保存路径
		CHAR szSnapShotDirectory[MAX_PATH] = {0};
		::GetModuleFileName(NULL,szSnapShotDirectory,MAX_PATH);
		(strrchr(szSnapShotDirectory,'\\'))[1] = 0; 
		strcat(szSnapShotDirectory,"SnapShot");
		m_pBRGameVideoClient->SetSnapShotSavePath(szSnapShotDirectory);

		SetVideoFrameStyle();
		rc = TRUE;
	}
	else
	{
		delete m_pBRGameVideoClient;
		m_pBRGameVideoClient = NULL;
		rc = FALSE;
	}
	return rc;
}

/**
 *	设置视频窗体风格
 */
void CBRGameVideoSDK::SetVideoFrameStyle(void)
{
	GV_VW_STYLE					m_VideoWindowStyle;			///< 视频窗体风格

	memset(&m_VideoWindowStyle,0,sizeof(GV_VW_STYLE));
	m_VideoWindowStyle.cbSize = sizeof(GV_VW_STYLE);
	m_VideoWindowStyle.bkColor = RGB(4,111,193);			///< 设置窗体背景色
	m_VideoWindowStyle.selfTitleColor = RGB(254,114,141);	///< 自己的标题颜色
	m_VideoWindowStyle.otherTitleColor = RGB(255,255,255);	///< 别人的标题颜色
	m_VideoWindowStyle.vfStyle = GV_VF_STYLE_SMALLFRAME;	///< 设置窗体样式
	m_VideoWindowStyle.bMoveWindow = FALSE;					///< 不支持窗体移动
	m_VideoWindowStyle.bShowTitle = TRUE;					///< 显示标题
	m_VideoWindowStyle.bShowVolume = TRUE;					///< 显示音量
	m_VideoWindowStyle.vfSkin = GV_VF_SKIN_DEFAULT;			///< 设置默认窗体皮肤风格

	GV_VFT_STYLE				m_VideoToolbarStyle;		///< 视频工具条风格
	memset(&m_VideoToolbarStyle,0,sizeof(GV_VFT_STYLE));
	m_VideoToolbarStyle.cbSize = sizeof(GV_VFT_STYLE);
	m_VideoToolbarStyle.bkColor = RGB(241,254,150);			///< 设置工具条背景色
	m_VideoToolbarStyle.maxTransPercent = 70;				///< 设置工具条最大透明度
	m_VideoToolbarStyle.animateDuration = 500;				///< 设置工具条变化周期(毫秒)
	m_VideoToolbarStyle.dwBtnMark = 0xFFFFFFFFL;			///< 显示所有按钮
	m_VideoToolbarStyle.dwBtnSpacePix = 5;					///< 按钮之间的间隔
	m_VideoToolbarStyle.vftDispMode = GV_VFTDM_AUTOSHOW;	///< 工具条显示模式：自动
	m_VideoToolbarStyle.vftBtnAlign = GV_VFTBA_LEFT;		///< 按钮排列模式
	strcpy((char*)m_VideoToolbarStyle.szExtendBtnTooltip,"更多...");	///< 工具条扩展按钮提示信息
	m_VideoToolbarStyle.dwEBtnTooltipSize =					///< 工具条扩展按钮提示信息字符串长度
		strlen((char*)m_VideoToolbarStyle.szExtendBtnTooltip);

	if(m_pBRGameVideoClient)
	{
		m_pBRGameVideoClient->SetVideoFrameStyle((long*)&m_VideoWindowStyle);
		m_pBRGameVideoClient->SetVideoToolBarStyle((long*)&m_VideoToolbarStyle);
	}
}
/**
 *	关闭SDK，退出系统之前调用，主动释放资源，避免内存泄露
 */
void CBRGameVideoSDK::CloseGameVideoSDK(void)
{
	if(m_pBRGameVideoClient)
	{
		m_pBRGameVideoClient->Release();
		m_pBRGameVideoClient->DestroyWindow();
		delete m_pBRGameVideoClient;
		m_pBRGameVideoClient = NULL;
	}
}
/**
 *	连接到服务器
 *	该方法立即返回，连接服务是否成功，可通过相关的消息获得
 *	@param ipaddr 服务器IP地址（必须为IP地址，不能是域名）
 *	@param port 服务器端口，默认为8902
 */
void CBRGameVideoSDK::ConnectToServer(LPCSTR ipaddr, DWORD port)
{
	if(m_pBRGameVideoClient)
		m_pBRGameVideoClient->StartVideoClient(ipaddr,(SHORT)port);
}
/**
 *	登录服务器
 *	该方法立即返回，登录是否成功，可通过相关的消息获得
 *	@param username 登录用户名，默认为空，表示以游客方式登录系统
 *	@param password 登录密码，默认空，表示以游客方式登录系统
 */
void CBRGameVideoSDK::LoginSystem(LPCSTR username, LPCSTR password)
{
	if(m_pBRGameVideoClient)
		m_pBRGameVideoClient->RegisterUser(0,0,username,password,0);
}
/**
 *	从服务器注销
 */
void CBRGameVideoSDK::LogoutSystem(void)
{
	if(m_pBRGameVideoClient)
		m_pBRGameVideoClient->UnRegisterUser(-1);
}
/**
 *	进入房间
 *	@param roomid 房间编号
 *	@param roompass 房间密码，默认为空，表示该房间没有密码
 */
void CBRGameVideoSDK::EnterRoom(INT roomid, LPCSTR roompass)
{
	if(m_pBRGameVideoClient)
		m_pBRGameVideoClient->EnterRoom(roomid,roompass,0);	
}
/**
 *	离开房间
 *	@param roomid 房间编号，默认为-1，表示退出当前房间
 */
void CBRGameVideoSDK::LeaveRoom(INT roomid)
{
	if(m_pBRGameVideoClient)
		m_pBRGameVideoClient->LeaveRoom(roomid);	
}
/**
 *	打开本地Mic
 */
void CBRGameVideoSDK::OpenLocalMic()
{
	if(m_pBRGameVideoClient)
		m_pBRGameVideoClient->UserSpeakControl(-1,TRUE);
}
/**
 *	关闭本地Mic
 */
void CBRGameVideoSDK::CloseLocalMic()
{
	if(m_pBRGameVideoClient)
		m_pBRGameVideoClient->UserSpeakControl(-1,FALSE);
}
/**
 *	打开本地摄像头
 */
void CBRGameVideoSDK::OpenLocalCamera()
{
	if(m_pBRGameVideoClient)
		m_pBRGameVideoClient->UserCameraControl(-1,TRUE,0,0,0,0,0);
}
/**
 *	关闭本地摄像头
 */
void CBRGameVideoSDK::CloseLocalCamera()
{
	if(m_pBRGameVideoClient)
		m_pBRGameVideoClient->UserCameraControl(-1,FALSE,0,0,0,0,0);
}
/**
 *	获取本地摄像头状态
 *	根据返回参数的不同，可以判别用户当前摄像头的状态
 *	@return 没有摄像头返回0，有摄像头但没有打开返回1，摄像头已打开返回2
 */
INT CBRGameVideoSDK::GetLocalCameraState()
{
	INT rc = 0;
	if(m_pBRGameVideoClient)
		rc = m_pBRGameVideoClient->GetCameraState(-1);
	return rc;
}

/**
 *	显示语音视频设置面板
 */
void CBRGameVideoSDK::ShowSettingPanel(void)
{
	if(m_pBRGameVideoClient)
		m_pBRGameVideoClient->ShowSettingPanel();
}

/**
 *	显示（隐藏）用户视频窗体
 *	@param userid		用户编号，-1表示显示自己的面板
 *	@param bShow		是否显示，BOOL型，TRUE显示，否则表示隐藏
 *	@param rc			显示位置
 *	@param style		显示样式：0 弹出窗口；1 子窗口
 *	@param pParentWnd	父窗口对象指针
 */
void CBRGameVideoSDK::ShowUserVideo(INT userid, short bShow, CRect rc, CWnd* pParentWnd)
{
	if(m_pBRGameVideoClient)
	{
//		m_pBRGameVideoClient->ShowUserPanelEx(userid,bShow,
//		(short)rc.left,(short)rc.top,(short)rc.right,(short)rc.bottom,1,(long*)pParentWnd);

/*
 注：用下面的方法也可以，ShowUserPanelEx2方法是在V4.1版本中新增的，区别在于：
	ShowUserPanelEx(...,pParentWnd)，最后的参数是传递 CWnd*地址
	ShowUserPanelEx2(...,parentHandle)，最后的参数是传递 CWnd对象的句柄（m_hWnd）地址，该方法适用于C#等语言调用	
*/

		HWND parentHandle = pParentWnd->GetSafeHwnd();
		m_pBRGameVideoClient->ShowUserPanelEx2(userid,bShow,
		(short)rc.left,(short)rc.top,(short)rc.right,(short)rc.bottom,1,(long*)&parentHandle);
	}
}
/**
 *	设置用户面板标题内容
 *	通过该方法可以设置用户的nickname和其它信息等，默认情况下，不调用该方法将显示用户的登录用户名
 *	该方法需在用户面板显示之后调用，即在ShowUserVideo方法之后调用
 *	@param userid		用户编号，-1表示设置自己的面板标题
 *	@param lpTitleStr	标题条内容
 */
void CBRGameVideoSDK::SetUserTitle(INT userid, LPCTSTR lpTitleStr) 
{
	if(m_pBRGameVideoClient)
		m_pBRGameVideoClient->SetUserPanelTitle(userid,lpTitleStr);
}
/**
 *	设置用户的个性图片，显示在背景中
 *	@param userid 用户编号，-1表示自己
 *	@param pathname 图片路径
 */
void CBRGameVideoSDK::SetUserPicture(INT userid, LPCSTR pathname)
{
	if(m_pBRGameVideoClient)
		m_pBRGameVideoClient->SetUserPicture(userid,pathname);
}
/**
 *	获取当前房间在线用户列表
 *	@param 保存用户列表信息的list
 *	@return 返回在线用户数
 */
INT CBRGameVideoSDK::GetRoomUserList(std::list<INT>& userlist)
{
	INT num = 0;
	if(m_pBRGameVideoClient)
	{
		m_pBRGameVideoClient->PrepaGetUserList();
		INT userid = -1;
		while( (userid = m_pBRGameVideoClient->FetchNextUserID()) != -1)
		{
			userlist.push_back(userid);
			num ++;
		}
	}
	return num;
}

/**
 * 得到指定ID的用户名
 *
 * @param id 要得到用户的ID
 *
 * @return 返回这个用户的名称
 */
CString CBRGameVideoSDK::GetUserNameById(int id)
{
	if(m_pBRGameVideoClient) 
		return m_pBRGameVideoClient->GetUserNameByUserID(id);

	return "";
}

/**
 * 刷新指定的面板
 *
 * @param id 要刷新的面板
 */
void CBRGameVideoSDK::RefreshUserPanel(int id)
{
	if(m_pBRGameVideoClient) 
		m_pBRGameVideoClient->RefreshUserPanel(id);
}

/**
 *	设置财富菜单项图标
 *	图标文件中的每个图标宽16个像素，高15个像素，RGB24位色，背景色为RGB(255,0,255)时背景透明
 *	@param lpImageFile 图标列表文件，如果为空，则使用系统默认的图标
 *	@return 成功返回TRUE，否则返回FALSE表示加载失败
 */
BOOL CBRGameVideoSDK::SetFortuneImage(LPCTSTR lpImageFile) 
{
	BOOL rc = FALSE;
	if(m_pBRGameVideoClient)
		rc = m_pBRGameVideoClient->SetFortuneImage(lpImageFile)==0 ? TRUE : FALSE;
	return rc;
}
/**
 *	重置（清空）指定用户的财富菜单
 *	@param userid 用户编号，-1表示重置自己的财富菜单
 */
void CBRGameVideoSDK::ResetFortuneMenu(long userid) 
{
	if(m_pBRGameVideoClient)
		m_pBRGameVideoClient->ResetFortuneMenu(userid);
}
/**
 *	给指定用户添加财富菜单项，该方法通常在显示用户面板后调用，或是用户的财富信息发生改变时调用
 *	@param userid 用户编号，-1表示自己
 *	@param menuText 财富菜单显示文字
 *	@param toolTip  财富菜单项提示信息
 *	@param menuTag  菜单标记，值要求大于0，当用户选择一个菜单后，会通过消息将该项值发送给调用程序，便于调用程序响应菜单事件
 *	@param imageIndex 财富菜单图标索引，从0开始，表示该菜单显示imageIndex所指的图标，图标为SetFortuneImage方法设置
 */
void CBRGameVideoSDK::AppendFortuneMenuItem(long userid, LPCTSTR menuText, LPCTSTR toolTip, short menuTag, short imageIndex) 
{
	if(m_pBRGameVideoClient)
		m_pBRGameVideoClient->AppendFortuneMenuItem(userid,menuText,toolTip,menuTag,imageIndex);
}

/**
 *	更改当前聊天模式
 *	@param chatmode 聊天模式：0 公聊模式， 1 私聊模式
 */
void CBRGameVideoSDK::ChangeChatMode(DWORD chatmode)
{
	if(m_pBRGameVideoClient)
		m_pBRGameVideoClient->ChangeChatMode(chatmode);
}

/**
 *	获取指定用户当前的聊天模式
 *	@param userid 需要获取聊天模式的用户ID号，-1表示获取自己的聊天模式
 *	@return 返回该用户的聊天模式
 */
long CBRGameVideoSDK::GetUserChatMode(long userid) 
{
	long rc = 0;
	if(m_pBRGameVideoClient)
		rc = m_pBRGameVideoClient->GetUserChatMode(userid);
	return 0;
}
/**
 *	请求与对方私聊，向对方发起私聊请求
 *	@param userid 对方的用户ID号
 */
long CBRGameVideoSDK::PrivateChatRequest(long userid) 
{
	long rc = 0;
	if(m_pBRGameVideoClient)
		rc = m_pBRGameVideoClient->PrivateChatRequest(userid);
	return rc;
}
/**
 *	回复对方的私聊请求
 *	@param userid 对方的用户ID号
 *	@param requestid 请求ID号，在请求消息中传过来，标示是哪一个请求
 *	@param bAccept 是否同意对方的私聊请求
 */
long CBRGameVideoSDK::PrivateChatEcho(long userid, long requestid, long bAccept) 
{
	long rc = 0;
	if(m_pBRGameVideoClient)
		rc = m_pBRGameVideoClient->PrivateChatEcho(userid,requestid,bAccept);
	return rc;
}
/**
 *	退出与某用户的私聊，或者将某用户从自己的私聊列表中清除
 *	断开与该用户的私聊连接后，不影响与其它用户的私聊
 *	@param userid 对方的用户ID号
 */
long CBRGameVideoSDK::PrivateChatExit(long userid) 
{
	long rc = 0;
	if(m_pBRGameVideoClient)
		rc = m_pBRGameVideoClient->PrivateChatExit(userid);
	return rc;
}