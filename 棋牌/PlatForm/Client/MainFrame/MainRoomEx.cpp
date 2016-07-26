#include "Stdafx.h"
#include <Windows.h>
#include <comutil.h>
#include "GamePlace.h"
#include "MainRoomEx.h"
#include "FileVersion.h"
//#include "LongonDialog.h"
#include "LogonDialog.h"
#include "GamePlaceDlg.h"
#include "GameRoomEx.h"
#include "GamePlaceMessage.h"
#include "LockAccountDlg.h"
#include "BuildTime.h"
#include "IEAdvertise.h"
#include "GMMsgDlg.h"
#include "DlgRoomFee.h"
#include "GameListTip.h"
#include "IDVerifyWnd.h"
#include "MobileVerifyWnd.h"
#include "bank4ZMessage.h"
#include "MiniPopWnd.h"
  
#include "PassDesk.h"	/// Add by lxl 2010-10-08 采用原来的锁桌对话框，换图片即可，用来进入密码房间
#include "iphlpapi.h"	//锁定机器 zxj 2009-11-12
#pragma comment(lib,"Iphlpapi.lib")	//锁定机器 zxj 2009-11-12

#include "detours.h"
#include "skinsb.h"


//菜单消息
#define IDM_IN_ROOM						100	
#define IDM_SET_USUAL_SERVER			101
#define IDM_DOWN_NEW					102
#define IDM_NOW_PEOPLE					103
#define IDM_MAX_PEOPLE					104
#define IDM_NOW_VER						105
#define IDM_MAX_VER						106

//宏定义
#define CONTROL_SPACE					0							//控件间距
#define SPLIT_WIDE						7							//拆分条宽度
#define BT_BUTTON_WIDE					50							//按钮宽度
#define BT_BUTTON_HIGHT					20							//按钮高度
#define TITLE_HIGHT						26							//标题栏高度
//#define BUTTON_WIDE					8							//隐藏按钮宽度
//#define BUTTON_HIGHT					80							//隐藏按钮高度
#define BUTTON_WIDE						9							//隐藏按钮宽度
#define BUTTON_HIGHT					109							//隐藏按钮高度
#define BT_BIG_BUTTON_WIDE				65							//按钮宽度
#define BT_BIG_BUTTON_HIGHT				20							//按钮高度
#define ROOM_BAR_HEIGHT					8							//房间框架高度
#define MESSAGE_TITLE					TEXT("系统消息")		//游戏消息标题

//框架大小 
#define ROOM_FRAME_TOP					104//56							//游戏的总框架上面的顶部的高度
#define ROOM_FRAME_LEFT					8//3							//框架左宽
#define ROOM_FRAME_RIGHT				0//4							//框架右宽
#define ROOM_FRAME_BOTTOM				0//12							//框架下高

//任务宏定义
#define MS_LOGON						1							//登陆任务
#define MS_GET_ROOM						2							//获取房间
#define ID_CHECK_SEND					50							//检测定时器
#define ID_AI_ENTERROOM					51							//机器人进入房间
#define CHECK_TIME						3000						//定时器间隔
#define CHECK_UPDATE					3
#define ID_CHECK_SIGNAL                 5
#define	IDT_CHANGE_LOCK					55							//锁定机器禁用状态计时器 zxj 2009-11-12
#define CHANGE_LOCK_TIME				30*1000						//锁定机器禁用状态计时器间隔 30 秒 zxj 2009-11-12
#define TIME_KEEP_ALIVE					56

#ifdef MY_QUICK_GAME_ON

#define IDT_QUICK_ENTER_ROOM			10010						//JianGK,快速进游戏房间定时器
#define IDT_QUICK_GAME					10012						//JianGK,快速进游戏定时器

#endif // MY_QUICK_GAME_ON
//按钮
#define BUTTON_TOP_HIGHT				46							//总框架顶部按钮图片的高度
#define BUTTON_TOP_WIDE					58							//顶部按钮图片的宽度
//====================================================================================
//====================================================================================
#define ROOM_LIST_BAR_POS_X					8							//
#define ROOM_LIST_BAR_POS_Y					0//286					//决定了数的位置
#define ROOM_LIST_BAR_WIDTH					230
#define ROOM_WEB_POS_X						(ROOM_LIST_BAR_WIDTH+SPLIT_WIDE+4+4)

#define VK_B								66						 //'B'key
#define VK_Z								90						 //'Z'key
#define VK_W								87						 //'W'key
//游戏大厅指针
static CMainRoomEx * pGamePlace=NULL;
CPtrArray m_ShortMessage;						//信息窗口

// PengJiLin, 2010-6-2, DLL 窗口句柄
#define WM_GAME_NOTIFY_TO_DLL   WM_USER + 100   // 发给 DLL 窗口句柄的消息
#define    WPARAM_GAME_LOGON_SUCCESS    0x10    // 登录成功
#define    WPARAM_GAME_LOGON_FAIL       0x13    // 登录失败
#define NOTINSTALL						99     //未开放游戏
extern HWND g_hWndDLL;
extern BOOL g_bRunBySoftware;

bool CMainRoomEx::bTipInit = false;
bool CMainRoomEx::m_bBankLogoned = false;   //已登陆银行?
bool CMainRoomEx::m_bLockQuit = false;

char CMainRoomEx::m_szMobileVCode[36];	//从服务商发下来的手机验证码MD5密文

// 清除数字字符串的空格
void ClearNumberSpace(char* pchInput, char* pchOutput)
{
	char* ptr1 = NULL;
	char* ptr2 = NULL;
	ptr1 = pchInput;
	while(*ptr1 != '\0' && (*ptr1 < '0' || *ptr1 > '9')) ++ptr1;
	ptr2 = ptr1;
	while(*ptr2 != '\0' && *ptr2 >= '0' && *ptr2 <= '9') ++ptr2;
	++ptr2;
	memcpy(pchOutput, ptr1, ptr2-ptr1);
}

string GetAppPath()
{
	char _tmpbuf[MAX_PATH + 1];
	ZeroMemory(_tmpbuf, sizeof(_tmpbuf));
	GetModuleFileNameA(NULL, _tmpbuf, MAX_PATH);

	string r = _tmpbuf;
	int l = r.find_last_of('\\');
	return r.substr(0, l);
} 

// 获取资源里的版本，格式化成： BZW.3.001
CString GetResourceFileVer()
{
	char cPath[200];
	char szVersionBuffer[200];
	DWORD dwHandle,InfoSize;
	CString strVersion = "";

	::GetModuleFileName(NULL,cPath,sizeof(cPath));      //首先获得版本信息资源的长度
	InfoSize = GetFileVersionInfoSize(cPath,&dwHandle); //将版本信息资源读入缓冲区
	if(InfoSize==0) return _T("None VerSion Supprot");

	char *InfoBuf = new char[InfoSize];
	GetFileVersionInfo(cPath,0,InfoSize,InfoBuf);       //获得生成文件使用的代码页及文件版本

	unsigned int  cbTranslate = 0;
	struct LANGANDCODEPAGE {
		WORD wLanguage;
		WORD wCodePage;
	} *lpTranslate;

	VerQueryValue(InfoBuf, TEXT("\\VarFileInfo\\Translation"),(LPVOID*)&lpTranslate,&cbTranslate);

	// Read the file description for each language and code page.
	for( int i=0; i < (cbTranslate/sizeof(struct LANGANDCODEPAGE)); i++ )
	{
		char  SubBlock[200];
		wsprintf( SubBlock, TEXT("\\StringFileInfo\\%04x%04x\\FileVersion"),
			lpTranslate[i].wLanguage, lpTranslate[i].wCodePage);
		void *lpBuffer = NULL;
		unsigned int dwBytes = 0;
		VerQueryValue(InfoBuf, SubBlock, &lpBuffer, &dwBytes);
		CString strTemp = (char *)lpBuffer;
		strVersion += strTemp;
		break;
	}

	// 结果应该是这样： 3, 0, 2, 5  将数字全部取出来，
	// 如果有空格，清除。
	char chData_1[10] = {0};
	char chVer_1[10] = {0};
	char chData_2[10] = {0};
	char chVer_2[10] = {0};
	char chData_3[10] = {0};
	char chVer_3[10] = {0};
	char chData_4[10] = {0};
	char chVer_4[10] = {0};


	int iFind1 = strVersion.Find(',');
	if(iFind1 < 0)
	{
		iFind1 = strVersion.Find('.');
		if(iFind1 < 0)
		{
			AfxMessageBox("版本1解析出错");
			return _T("None VerSion Found");
		}
	}
	strcpy(chData_1, strVersion.Left(iFind1));

	int iFind2 = strVersion.Find(',', iFind1+1);
	if(iFind2 < 0)
	{
		iFind2 = strVersion.Find('.', iFind1+1);
		if(iFind2 < 0)
		{
			AfxMessageBox("版本2解析出错");
			return _T("None VerSion Found");
		}
	}
	strcpy(chData_2, strVersion.Mid(iFind1+1, iFind2-iFind1-1));

	int iFind3 = strVersion.Find(',', iFind2+1);
	if(iFind3 < 0)
	{
		iFind3 = strVersion.Find('.', iFind2+1);
		if(iFind3 < 0)
		{
			AfxMessageBox("版本3解析出错");
			return _T("None VerSion Found");
		}
	}
	strcpy(chData_3, strVersion.Mid(iFind2+1, iFind2-iFind1-1));

	strcpy(chData_4, strVersion.Mid(iFind3+1));

	ClearNumberSpace(chData_1, chVer_1);
	ClearNumberSpace(chData_2, chVer_2);
	ClearNumberSpace(chData_3, chVer_3);
	ClearNumberSpace(chData_4, chVer_4);

	// 版本格式化字符串
	strVersion = "";
	strVersion.Format("BZW.%s.%s%s%s", chVer_1, chVer_2, chVer_3, chVer_4);

	delete[] InfoBuf;

	return strVersion;
}

BEGIN_MESSAGE_MAP(CMainRoomEx, CBaseRoom)
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
	ON_COMMAND(IDM_IN_ROOM, OnInGame)  
	ON_COMMAND(IDM_CONNECT_CENTER_SERVER, OnConnectToCenterServer)
	ON_COMMAND(IDM_LOGON_PLACE, OnLogonPlace)
	ON_MESSAGE(WM_CLOSE_ROOM,OnCloseGameRoom)
	ON_MESSAGE(WM_AUTO_GET_MONEY,onAutoGetMoney)
	ON_MESSAGE(WM_COLSE_ROOM_WITH_MSG,OnCloseGameRoomWithMsg)
	ON_MESSAGE(WM_LOADSKIN,OnLoadSkin)
	ON_MESSAGE(IDM_SHOW_LEFT,OnShowLeft)
	ON_MESSAGE(IDM_TAB_SELECT,OnTopTabSelected)
	ON_MESSAGE(IDM_TAB_SELECT_LOOST,OnTopTabLostSelected)
	ON_MESSAGE(IDM_OPEN_IE,OnOpenIE)
	ON_MESSAGE(IDM_OPEN_BANK, OnOpenBank)

	ON_MESSAGE(IDM_CALL_GM, OnCallGM)

	ON_MESSAGE(IDM_MESSAGE_APPEAL,OnAppeal)
	ON_MESSAGE(IDM_LOCK_ACCOUNT, OnLockAccount)	//锁定机器 zxj 2009-11-12
	ON_MESSAGE(WM_EXCHANGE_SKIN,OnExchangeSkin)
	
	//ON_NOTIFY(NM_LCLICK, IDC_GAME_LIST, OnNMLclickGameList)//左击游戏列表
	//	ON_NOTIFY(NM_RCLICK, IDC_GAME_LIST, OnNMRclickGameList)//右击游戏列表
	//	ON_NOTIFY(NM_DBLCLK, IDC_GAME_LIST, OnNMDblclkGameList)//双击游戏列表
	//	ON_NOTIFY(TVN_SELCHANGED, IDC_GAME_LIST, OnTvnSelchangedGameList)
	//	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_GAME_LIST, OnTvnItemexpandingGameList)
	//	ON_BN_CLICKED(IDC_HIDE_LIST, OnBnClickedHideList)
	//	ON_BN_CLICKED(IDC_HIDE_NEWS, OnBnClickedHideNews)
	ON_WM_PAINT()
	//ON_BN_CLICKED(IDC_BUTTON_MIN, OnBnClickedMin)
	//ON_BN_CLICKED(IDC_BUTTON_CLOSE, OnBnClickedClose)
	//ON_BN_CLICKED(IDC_BUTTON_BT1, OnBnClicked1)///////////
	//ON_BN_CLICKED(IDC_BUTTON_BT2, OnBnClicked2)///////////
	//ON_BN_CLICKED(IDC_BUTTON_BT3, OnBnClicked3)///////////
	//ON_BN_CLICKED(IDC_BUTTON_BT4, OnBnClicked4)///////////
	//ON_BN_CLICKED(IDC_BUTTON_BT5, OnBnClicked5)///////////
	//	ON_BN_CLICKED(IDC_SKIN,OnBnClickedSkin)

	//	ON_WM_CTLCOLOR()
	//	ON_BN_CLICKED(IDC_SHOWMENU, &CMainRoomEx::OnBnClickedShowmenu)
	//	ON_BN_CLICKED(IDC_BT6, &CMainRoomEx::OnBnClickedBt6)
	//	ON_BN_CLICKED(IDC_BT7, &CMainRoomEx::OnBnClickedBt7)
	ON_BN_CLICKED(IDC_BTN_MAIN1,OnBnClickedBt1)
	ON_BN_CLICKED(IDC_BTN_MAIN2,OnBnClickedBt2)
	ON_BN_CLICKED(IDC_BTN_MAIN3,OnBnClickedBt3)
	ON_BN_CLICKED(IDC_BTN_MAIN4,OnBnClickedBt4)
	ON_BN_CLICKED(IDC_BTN_MAIN5,OnBnClickedBt5)
	ON_BN_CLICKED(IDC_BTN_MAIN6,OnBnClickedBt6)
	ON_BN_CLICKED(IDC_BTN_MAIN7,OnBnClickedBt7)
	ON_BN_CLICKED(IDC_BTN_MAIN8,OnBnClickedBt8)
	ON_BN_CLICKED(IDC_BTN_MAIN9,OnBnClickedBt9)
	ON_BN_CLICKED(IDC_BTN_MAIN10,OnBnClickedBt10)

	
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CMainRoomEx, CBaseRoom)
	ON_EVENT(CMainRoomEx, IDC_WEB, 250, CMainRoomEx::BeforeNavigate2Explorer, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
	ON_EVENT(CMainRoomEx, IDC_WEB, 271, CMainRoomEx::NavigateErrorExplorer, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
END_EVENTSINK_MAP()

UINT CMainRoomEx::m_onlineUserCount = 0;
//构造函数
CMainRoomEx::CMainRoomEx(CGameListWnd *pGameListWnd,CActWnd *pActWnd,CTopWndClass *pTopWnd,CTopDuWndClass *pTopDuWnd)
: CBaseRoom(IDD_GAME_PLACE), m_NavigationDlg(this)

{
	m_pDownloadWnd = NULL;
	m_HaveSellList = false;
	m_pListTip = NULL;
	m_bListmessage = false;
	m_bInit = false;
	m_bConnectingToCenterServer=false;
	m_bGetPara = false;
	//初始化变量
	//m_width=442;
	m_bLogon=false;
	m_bCheckVer=false;
	m_bHideList=false;
	pGamePlace=this;
	memset(m_Mission,0,sizeof(m_Mission));
	memset(m_RoomInfo,0,sizeof(m_RoomInfo));
	memset(&m_PlaceUserInfo,0,sizeof(m_PlaceUserInfo));
	//lstrcpy(m_title,Glb().m_title);
	::InitializeCriticalSection(&m_Section);

	m_pGameListWnd = pGameListWnd;
	m_pActWnd = pActWnd;
	m_TopWnd = pTopWnd;
	m_TopDuWnd = pTopDuWnd;
	m_bShowSysmsg = true;
	logoUpload=NULL;
	hBulletin=NULL;
	propPannel=NULL;

	m_pAppealDlg = NULL;
	m_pGMListDlg = NULL;

	//测网速信号灯  yjj 090227
	img=NULL;
    m_NetSignalLevel=0;
    m_SavedSingalIndex=0;
	m_bLockMathine = FALSE;						//表示锁定机器功能是否启动 zxj 2009-11-12
	m_bCanClick = TRUE;							//表示锁定机器功能是否启动 zxj 2009-11-12

    // PengJiLin, 2010-9-13
    m_iKickPropTotalTime = 0;
    m_iAntiKickPropTotalTime = 0;
	m_nKeepAliveCount = 0;

	m_pBankWnd = NULL;
	m_pBankVerifyWnd = NULL;
	m_pBankWndEx = NULL;
	::memset(m_szMobileVCode,NULL,sizeof(m_szMobileVCode));

	m_pSafeCenterWnd = NULL;
	//2012.11.20 向凌华
	m_pMobileVerifyWnd = NULL;
#ifdef MY_QUICK_GAME_ON

	//Jiangk
	m_hQGNamedPipe = INVALID_HANDLE_VALUE;
	m_hQGThread = INVALID_HANDLE_VALUE;
	m_uQGRoomID = 0;

#endif // MY_QUICK_GAME_ON

	//显示TopWnd 上的用户信息
	m_pUserHeadPic = static_cast<BzDui::CButtonUI*>(m_TopDuWnd->m_pm.FindControl("Btn_UserPic"));
	if(m_pUserHeadPic != NULL)
	{
		CString csImg;
		csImg.Format("../../log/UserLogos/Logo_%d.png",this->m_PlaceUserInfo.bLogoID);
		m_pUserHeadPic->SetBkImage(csImg);
		m_pUserHeadPic->SetNormalImage(csImg);
	}
}
//构造函数
CMainRoomEx::CMainRoomEx() : CBaseRoom(IDD_GAME_PLACE), m_NavigationDlg(this)
{
	m_HaveSellList = false;
	m_pListTip = NULL;
	m_bListmessage = false;
	m_bInit = false;
	m_bConnectingToCenterServer=false;
	m_bGetPara = false;
	//初始化变量
	//m_width=442;
	m_bLogon=false;
	m_bCheckVer=false;
	m_bHideList=false;
	pGamePlace=this;
	memset(m_Mission,0,sizeof(m_Mission));
	memset(m_RoomInfo,0,sizeof(m_RoomInfo));
	memset(&m_PlaceUserInfo,0,sizeof(m_PlaceUserInfo));
	//lstrcpy(m_title,Glb().m_title);
	::InitializeCriticalSection(&m_Section);
	m_bShowSysmsg = true;
	logoUpload=NULL;
	propPannel=NULL;
	m_bLockMathine = FALSE;						//表示锁定机器功能是否启动 zxj 2009-11-12
	m_bCanClick = TRUE;							//表示锁定机器功能是否启动 zxj 2009-11-12
	m_pAppealDlg = NULL;
	m_pGMListDlg = NULL;

    // PengJiLin, 2010-9-13
    m_iKickPropTotalTime = 0;
    m_iAntiKickPropTotalTime = 0;

	m_pMobileVerifyWnd = NULL;

}
//绘画框架
void CMainRoomEx::DrawViewFrame(CDC * pDC, int iWidth, int iHeight)
{
	//绘画上下框架
	//h
	//CGameImageHelper helpt(&m_maintc);
	//CGameImageHelper helpb(&m_mainbc);
	//if(helpt.GetHeight()>0 && helpb.GetHeight()>0)
	//{
	//	for(int i=0;i<iWidth;i+=helpt.GetWidth())
	//	{
	//		helpt.BitBlt(pDC->GetSafeHdc(),i,0);
	//		helpb.BitBlt(pDC->GetSafeHdc(),i,iHeight-helpb.GetHeight());
	//	}
	//}
	//v
	CGameImageHelper helpl(&m_mainlc);
	CGameImageHelper helpr(&m_mainrc);
	if(helpl.GetHeight()>0 && helpr.GetHeight()>0)
	{
		for(int i=0;i<iHeight;i+=helpl.GetHeight())
		{
			helpl.BitBlt(pDC->GetSafeHdc(),0,i);
			helpr.BitBlt(pDC->GetSafeHdc(),iWidth-helpr.GetWidth(),i);
		}
	}
	//主框架边角
	//CGameImageHelper helplt(&m_mainlt);
	//helplt.BitBlt(pDC->GetSafeHdc(),2,0);
	//CGameImageHelper helprt(&m_mainrt);
	//helprt.BitBlt(pDC->GetSafeHdc(),iWidth-helprt.GetWidth() - 2,0);
	CGameImageHelper helplb(&m_mainlb);
	helplb.BitBlt(pDC->GetSafeHdc(),0,iHeight-helplb.GetHeight());
	CGameImageHelper helprb(&m_mainrb);
	helprb.BitBlt(pDC->GetSafeHdc(),iWidth-helprb.GetWidth(),iHeight-helprb.GetHeight());

	CGameListWnd* pGameListWnd = GetMainRoom()->m_pGameListWnd;
	if (pGameListWnd->m_UserTabDuWnd.GetHWND())
	{
		if(pGameListWnd->GetIsHide())
		{
			pGameListWnd->m_UserTabDuWnd.ShowWindow(SW_HIDE);
		}
		else
		{
			pGameListWnd->m_UserTabDuWnd.ShowWindow(SW_SHOW);
		}

	}
	return;
}

//处理任务
bool CMainRoomEx::HandleMission()
{
	if (m_Mission[0].bAccess==true)
	{
		switch (m_Mission[0].uMissionID)
		{
		case MS_LOGON:		//发送登陆信息
			{
				SendUserInfo();////发送用户信息
				DeleteMission();
				return true;
			}
		case MS_GET_ROOM:	//获取游戏列表
			{
				MSG_GP_SR_GetRoomStruct GetList;
				GetList.uKindID=m_Mission[0].uKindID;
				GetList.uNameID=m_Mission[0].uNameID;
				SendData(&GetList,sizeof(GetList),MDM_GP_LIST,ASS_GP_LIST_ROOM,0);

				OutputDebugString("sdp_list 发送游戏列表请求");

				DeleteMission();
				return true;
			}
		}
	}
	//设置断开时间
	SetTimer(ID_CHECK_SEND,CHECK_TIME,NULL);
	return false;
}

LRESULT CMainRoomEx::OnCloseGameRoomWithMsg(WPARAM wparam, LPARAM lparam)
{
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	CString strMessage;
	strMessage = fMsg.GetKeyVal("MainRoom","PlayerQuitGameOver","由于您的同桌玩家退出游戏，本局结束！");
	
	//AFCMessageBox(strMessage);
    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",strMessage);
	return 0;
}
//自动赠送金币2013-01-31 wangzhitu
LRESULT CMainRoomEx::onAutoGetMoney(WPARAM wparam, LPARAM lparam)
{
	RECEIVEMONEYINFO *pRecMoney = (RECEIVEMONEYINFO *)wparam;
	RECEIVEMONEYINFO strRecMoney;
	strRecMoney.strMsg = pRecMoney->strMsg;
	strRecMoney.iSec   = pRecMoney->iSec;	
	DUITimerMsgBox(m_hWnd,strRecMoney.iSec,strRecMoney.strMsg);
	delete pRecMoney;
	pRecMoney = NULL;
	return 0;
}

//关闭房间请求
LRESULT CMainRoomEx::OnCloseGameRoom(WPARAM wparam, LPARAM lparam)
{

	RoomInfoStruct * pRoomInfo=(RoomInfoStruct *)wparam;
	if ((pRoomInfo!=NULL)&&(pRoomInfo->bAccess)&&(pRoomInfo->pGameRoomWnd!=NULL)&&(IsWindow(pRoomInfo->pGameRoomWnd->GetSafeHwnd())))
	{

		if(m_pGameListWnd != NULL)
			m_pGameListWnd->m_GameList.IEnterARoomOrLeave(pRoomInfo->pComRoomInfo.uRoomID,false);//关闭房间

		CNewGameClass * pRoomTab=&((CGamePlaceDlg *)AfxGetMainWnd())->m_RoomTabWnd;

		if(pRoomTab != NULL)
		{
			pRoomTab->ChangeSkin(m_skinmgr.GetSkinBcfFileName(),
				m_skinmgr.GetKeyVal(m_skinmgr.GetSkinBcfFileName()),
				m_skinmgr.GetSkinPath());
			pRoomTab->RemoveTabPage(pRoomInfo->uTabIndex);
		}

		// 退出房间时应当销毁房间窗口
		memset(pRoomInfo,0,sizeof(RoomInfoStruct));

// 		if (pRoomInfo)
// 		{
// 			delete pRoomInfo;//zht 2011-10-26 退出崩溃问题
// 		}
 		pRoomInfo = NULL;


		BZSoundPlay(this, "music/关闭声.mp3", 0);

		if(m_PlaceUserInfo.dwTimeIsMoney > 0)
		{
			CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
			CString strMessage;
			TCHAR szMsg[MAX_PATH];
			TCHAR szNum[128]; 

            GetNumString(szNum, m_PlaceUserInfo.dwTimeIsMoney, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);

			strMessage = fMsg.GetKeyVal("MainRoom","GiftGoldToBank","本次登录时长房间所送的金币数[%s],已到您银行帐上,请查收!");
			wsprintf(szMsg,strMessage,szNum);
			
			//AFCMessageBox(szMsg);
            DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",szMsg);
		}
	}

	OutputDebugString("DClient:CMainRoomEx::OnCloseGameRoom3");

	GetLastLoginGame();

	return 0;
}

//提炼出来的进入游戏房间
bool CMainRoomEx::EnterRoom(CAFCRoomItem * pGameRoomItem)
{
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	CString str;
	CString strTitle;
	strTitle = fMsg.GetKeyVal("MainRoom","MessageTitle","系统消息");
	if ((pGameRoomItem==NULL)||(pGameRoomItem->m_uDataType!=GLK_GAME_ROOM))
	{
		str = fMsg.GetKeyVal("MainRoom","SelectGameRoom","请先选择游戏房间");		
        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,strTitle,str);
		return false;
	}

	//判断是否特殊房间
	ComRoomInfo * pComRoomInfo=&pGameRoomItem->m_RoomInfo;
	if (((pComRoomInfo->uComType!=TY_NORMAL_GAME)&&(pComRoomInfo->uComType!=TY_MATCH_GAME)
		&&(pComRoomInfo->uComType!=TY_MONEY_GAME)))
	{
		str = fMsg.GetKeyVal("MainRoom","DownloadNewComponent",
			"服务器增加了新的构件实例，请重新下载,然后安装.是否现在下载？");		
		if(IDYES==DUIMessageBox(m_hWnd,MB_YESNO|MB_ICONINFORMATION,strTitle,str))
		{
			ShellExecute(NULL,TEXT("open"),TEXT(Glb().m_CenterServerPara.m_strDownLoadUpdatepADDR),NULL,NULL,SW_MAXIMIZE);
		}
		CloseSocket (true);
		AfxGetApp( )->m_pMainWnd->DestroyWindow ();
		return true;		
	}

	//判断游戏是否需要更新
	ComNameInfo* pNameInfo = NULL;
	if (IsGameNeedUpdate(&pGameRoomItem->m_RoomInfo, pNameInfo))
	{
		DownloadGame(pNameInfo);
		return true;
	}

	//寻找游戏房间
	CString strMessage;
	BYTE m_bNowRoomIndex=255;
	CNewGameClass * pRoomTab=&((CGamePlaceDlg *)AfxGetMainWnd())->m_RoomTabWnd;

	bool bInMoneyRoom = false; 
	for (BYTE i=0;i<sizeof(m_RoomInfo)/sizeof(m_RoomInfo[0]);i++)
	{
		//当已经进入游戏时，防止双击游戏列表关闭游戏		
		if(pGameRoomItem->m_RoomInfo.uRoomID==m_RoomInfo[i].pComRoomInfo.uRoomID)
		{
			if (m_RoomInfo[i].pComRoomInfo.uComType != TY_MATCH_GAME ||
                (m_RoomInfo[i].pComRoomInfo.dwRoomRule & GRR_QUEUE_GAME))   // 比赛排队机场
			{
				pRoomTab->SetActivePageByExtIndex(m_RoomInfo[i].uTabIndex);
			}
			return true;
		}
		
		if(m_RoomInfo[i].bAccess && m_RoomInfo[i].pComRoomInfo.uComType == TY_MONEY_GAME)//是否已经进入金币场
			bInMoneyRoom = true;
		if (m_RoomInfo[i].bAccess&&(m_RoomInfo[i].pComRoomInfo.uRoomID==pComRoomInfo->uRoomID))
		{			
			if (m_RoomInfo[i].pComRoomInfo.uComType != TY_MATCH_GAME || 
                (m_RoomInfo[i].pComRoomInfo.dwRoomRule & GRR_QUEUE_GAME))   // 比赛排队机场
			{
				pRoomTab->SetActivePageByExtIndex(m_RoomInfo[i].uTabIndex);
			}
			m_RoomInfo[i].pGameRoomWnd->PostMessage(WM_BRING_GAMEWND_TO_TOP,0,0);
			return true;
		}
		else if (m_RoomInfo[i].bAccess==false) 
		{
			m_bNowRoomIndex=((m_bNowRoomIndex==255)?i:m_bNowRoomIndex);
		}
	}

	//判断游戏是否安装并效验版本
	CFileVersion FileVer;
	bool needupdate=false;
	TCHAR szProessName[31],szGameName[61];

	memset(szProessName,0,sizeof(szProessName));
	memset(szGameName,0,sizeof(szGameName));

	m_pGameListWnd->m_pLeftWnd->GetGameName(pGameRoomItem,szGameName,sizeof(szGameName));
	m_pGameListWnd->m_pLeftWnd->GetProcessName(pGameRoomItem,szProessName,sizeof(szProessName));
	//{{ 正确判断游戏程序文件名

	TCHAR szIcoName[MAX_PATH];
	int nLen = _tcslen(szProessName);
	if ((szProessName[nLen-3] == TCHAR('e') || szProessName[nLen-3] == TCHAR('E'))
		&&(szProessName[nLen-2] == TCHAR('x') || szProessName[nLen-2] == TCHAR('X'))
		&&(szProessName[nLen-1] == TCHAR('e') || szProessName[nLen-1] == TCHAR('E'))
		)
	{
		_tcscpy(szIcoName, CBcfFile::GetAppPath());
		_tcscat(szIcoName, szProessName);
		// 把名字前面一段改成目录名，再加上文件名
		nLen = _tcslen(szIcoName);
		szIcoName[nLen-4] = TCHAR('\\');
		szIcoName[nLen-3] = TCHAR('\0');
		_tcscat(szIcoName, szProessName);	//再附加文件名

		//非U3d游戏，不用把".exe"改成".ico"
		if (!(pGameRoomItem->m_RoomInfo.dwRoomRule & GRR_GAME_U3D))
		{
			nLen = _tcslen(szIcoName);
			szIcoName[nLen-3] =TCHAR('i');
			szIcoName[nLen-2] =TCHAR('c');
			szIcoName[nLen-1] =TCHAR('o');
		}
	}
	else
	{
		_tcscpy(szIcoName, szProessName);
	}	
	//判断此游戏是否正在更新
	try
	{
		POSITION pos;
		::EnterCriticalSection(&m_Section);		
		for(vector<LPUPDATE>::iterator it = m_UpdateList.begin(); it != m_UpdateList.end();it++)
		{
			LPUPDATE lp = *it;
			if(!lp) continue;
			if(lp->uNameID == pGameRoomItem->m_RoomInfo.uNameID)
			{				
				::LeaveCriticalSection(&m_Section);
				str = fMsg.GetKeyVal("MainRoom","EnterAfterUpdate","此游戏正在更新, 请您在更新完后再进入游戏");				
                DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,szGameName,str);
				return false;				
			}	
		}
		::LeaveCriticalSection(&m_Section);
	}
	catch(...)
	{
		::LeaveCriticalSection(&m_Section);
	}

	if (!FileVer.Open(szIcoName))
	{

		str = fMsg.GetKeyVal("MainRoom","OutOfRoomBeforeDownload","即将下载 %s游戏,请确认没有进入该游戏房间!");
		strMessage.Format(str,szGameName);		
		if(DUIMessageBox(m_hWnd,MB_YESNO|MB_ICONINFORMATION,MESSAGE_TITLE,strMessage)==IDYES)
		{
			//wushuqun 2009.5.21
			needupdate = true;
			try
			{   
				//加载下载信息准备下载
				StartDownLoad(szProessName,pGameRoomItem);
			}
			catch(CException* e)
			{
				str = fMsg.GetKeyVal("MainRoom","DownloadError","下载发生了异常！");
				AfxMessageBox(str);
				e->Delete();
				return false;
			}
			catch(...)
			{
				str = fMsg.GetKeyVal("MainRoom","DownloadError","下载发生了异常！");
				AfxMessageBox(str);
				return false;
			}			
			return true;
		}
		else
		{
			return false;
		}
	}
	//启动update进行更新
	TCHAR szApp[61];//,sz[100];
	TCHAR path[MAX_PATH];
	lstrcpy(szApp,szProessName);
	szApp[lstrlen(szApp)-lstrlen("ico")-1]='\0';
	wsprintf(path,"%s\\%s\\update.ini",GetAppPath().c_str(),szApp);
	CINIFile f(path);
	CString m_key="config";
	CString v,s,ul,uv,n;
	int sm;	
	//版本号有默认值，不然会引起update.exe 出错
	CString strOut;
	str = fMsg.GetKeyVal("MainRoom","GameNameOfUpdate","游戏名 %s,%s,update更新文件 %s");
	strOut.Format(str,szGameName,szProessName,path);
	::OutputDebugString(strOut);

	v=f.GetKeyVal(m_key,"-v","0");//版本	
	s=f.GetKeyVal(m_key,"-s","./");//路径
	ul=f.GetKeyVal(m_key,"-ul","");//更新列表地址	
	uv=f.GetKeyVal(m_key,"-uv","");//更新版本地址
	n=f.GetKeyVal(m_key,"-n","游戏更新");//游戏名称	
	sm=f.GetKeyVal(m_key,"-sm",0);//显示方式
	bool getver=false;
	TCHAR ser[20];
	wsprintf(ser,"%u.%u",pComRoomInfo->uVer>>16,pComRoomInfo->uVer&0x0000ffff);
	if (ul =="")
	{
		ul.Format("%s/update_version.txt",szApp);
		uv = ul;
	}

#ifndef MY_DEV	
	if(v!="" && ! needupdate)
	{

		getver=true;	
		//版本核对
		if (CheckUpdate(szProessName, pGameRoomItem))
		{
			str = fMsg.GetKeyVal("MainRoom","UpdateNewGame",
				"游戏已有更新版本, 您需要更新游戏, 是否现在更新?");			
			if(DUIMessageBox(m_hWnd,MB_YESNO|MB_ICONINFORMATION,szGameName,str)==IDYES)
			{
				needupdate=true;
			}
			else
			{
				needupdate=false;
				return false;
			}
		}
	}
	if(!getver && ! needupdate)
	{
		str = fMsg.GetKeyVal("MainRoom","UnknowVersionReinstall",
			"无法得知当前游戏版本,重新安装此游戏可能会修复此问题\n是否现在下载?");
		if(DUIMessageBox(m_hWnd,MB_YESNO|MB_ICONINFORMATION,szGameName,str)==IDYES)
		{
			SendMessage(WM_COMMAND,MAKELONG(IDM_DOWN_NEW,0),0);
		}
		return false;
	}

	if(needupdate)
	{
		try
		{
			::EnterCriticalSection(&m_Section);
			if(m_UpdateList.size() >= 64)
			{
				::LeaveCriticalSection(&m_Section);
				str = fMsg.GetKeyVal("MainRoom","TooManyUpdateTryLater","正在更新的游戏太多,请后再试.");
                DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,szGameName,str);
				return false;
			}
			else
			{
				::LeaveCriticalSection(&m_Section);
			}
		}
		catch(...)
		{
			::LeaveCriticalSection(&m_Section);
		}
		STARTUPINFO StartupInfo;
		ZeroMemory(&StartupInfo, sizeof(STARTUPINFO));
		StartupInfo.cb = sizeof(STARTUPINFO);
		PROCESS_INFORMATION ProcessInfo;
		char up[1000];		
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind;
		bool setup;

		setup=true;
		hFind = FindFirstFile(path, &FindFileData);
		if (hFind == INVALID_HANDLE_VALUE) 
		{

		} 
		else 
		{
			setup=false;
			FindClose(hFind);
		}

		//恢复为原来的更新方式
		string _path;
		_path = GetAppPath() + "\\" + szApp;
		sprintf(up, "-n %s -s %s\\ -v %s -ul %s%s -uv %s%s -sm %d", n, _path.c_str(), v, Glb().m_UpdateUrl, ul,Glb().m_UpdateUrl,uv, 1);		

		//-异步启动		
		sDownLoadUpdateMsg sUpdateMsg; 
		//-----------------
		UpdateStructMsg updateMsg;
		memset(&updateMsg,0,sizeof(UpdateStructMsg));
		updateMsg.command = 1;
		updateMsg.nRoomID = pGameRoomItem->m_RoomInfo.uNameID;
		wsprintf(updateMsg.g_GameName,"%s",szGameName);
		wsprintf(updateMsg.g_url,"%s%s",Glb().m_UpdateUrl,uv);
		wsprintf(updateMsg.g_ver,"%s",v); 
		sUpdateMsg.pUpdateStructMsg = &updateMsg;
		//-----------------
		UPDATE *lp=(LPUPDATE)malloc(sizeof(UPDATE));
		lp->hProcess=ProcessInfo.hProcess;
		lp->uNameID=pGameRoomItem->m_RoomInfo.uNameID;
		lp->pGameRoomItem=pGameRoomItem;
		sUpdateMsg.pUPDATE = lp;
		//-----------------
		sUpdateMsg.pCMainRoomEx = this;
		//-----------------
		sUpdateMsg.pCBcfFile = &fMsg;
		//-----------------

		DWORD dwThreadID = 0;
		HANDLE hThread = CreateThread(0,0,DownLoadUpdate,&sUpdateMsg,0,&dwThreadID);

		int nStartTime = fMsg.GetKeyVal("MainRoom","DownloadStartTime",3000);

		//连接update.exe 超时
		if (WaitForSingleObject(hThread,nStartTime)==WAIT_TIMEOUT) 
		{
			if(m_CSocket32.GetProcessInfo().hProcess)
			{
				//终止进程
				::TerminateProcess( m_CSocket32.GetProcessInfo().hProcess, 0 );
			}
			m_CSocket32.Close();

			CString str;
			CString strTitle;
			str = fMsg.GetKeyVal("MainRoom","DownLoadOver","下载游戏更新超时");
			strTitle = fMsg.GetKeyVal("MainRoom","MessageTitle","提示");
			
			//AFCMessageBox(str,strTitle);
            DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,strTitle,str);
		}
		if (hThread)
		{
			::CloseHandle(hThread);
		}
		//----------------------------------------------------------

		return false;
	}
#endif
	//不能打开两个多个同游戏的房间
	CGameRoomEx* pGameRoom = NULL;
	int iRoomOpened = 0;
	for (BYTE i = 0;i < MAX_GAME_ROOM; ++i)
	{
		pGameRoom = (CGameRoomEx*)m_RoomInfo[i].pGameRoomWnd;
		if(NULL != pGameRoom)
		{
			iRoomOpened++;
			if (pGameRoom->m_GameInfo.uNameID== pGameRoomItem->m_RoomInfo.uNameID)
			{
				CString strTip;
				strTip.Format("您已经进入【%s】的房间，同一个游戏只能进入一个房间。",pGameRoom->m_GameInfo.szGameName);
				
                DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",strTip);
				return 0;
			}
		}
	}
	//End

	//打开房间数量的限制判断
	int iMaxRoomNum = CGamePlaceApp::GetModuleStatus("GameRoom","MaxOpenNums",1);

	if (iRoomOpened >= iMaxRoomNum)
	{
		CString strTip;
		strTip.Format("您已进入了%d个游戏房间，不能再进入更多的房间",iMaxRoomNum);
		
		//AFCMessageBox(strTip);
        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",strTip);
		return 0;
	}
	//End

	if(pGameRoomItem->m_RoomInfo.iLeaseID != 0)
	{
		CTime _t = CTime::GetCurrentTime();
		if (_t.GetTime() > pGameRoomItem->m_RoomInfo.i64LeaseTimeEnd)
		{
            DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","房间租赁已经结束");
			return false;
		}
	}

	if (pComRoomInfo->bHasPassword)
	{
		/// 输入密码对话框
		CPassDesk pdDlg;
		if(pdDlg.DoModal()==IDCANCEL)
			return 0;

		char szSrcPass[MAX_PATH];
		strcpy(szSrcPass, pdDlg.m_pass.GetBuffer());

		/// 加密字符串
		MD5_CTX Md5;
		Md5.MD5Update((unsigned char *)szSrcPass,lstrlen(szSrcPass));
		unsigned char szMDTemp[16];
		Md5.MD5Final(szMDTemp);
		for (int i=0;i<16;i++) wsprintf(&szSrcPass[i*2],"%02x",szMDTemp[i]);

		/// 提交密码给服务器
		MSG_GP_CheckRoomPasswd crp;
		crp.uRoomID = pComRoomInfo->uRoomID;
		strcpy(crp.szMD5PassWord, szSrcPass);

		SendData(&crp,sizeof(MSG_GP_CheckRoomPasswd),MDM_GP_LIST, ASS_GP_ROOM_PASSWORD,0);
	}
	//进入游戏大厅
	RoomInfoStruct * pRoomInfoItem = &m_RoomInfo[m_bNowRoomIndex];
	try
	{
		pRoomInfoItem->bAccess=true;
		pRoomInfoItem->pComRoomInfo=*pComRoomInfo;
		pRoomInfoItem->pRoomItem = pGameRoomItem;
		lstrcpy(pRoomInfoItem->szProcessName,szProessName);
		FileVer.GetFixedProductVersion(pRoomInfoItem->dwGameMSVer,pRoomInfoItem->dwGameLSVer);
		////右边
		pRoomInfoItem->pRoomInfoWnd=new CRoomRightWnd(IDD_ROOM_RIGHT_WND);
		
		if(pRoomInfoItem->pRoomInfoWnd->Create(IDD_ROOM_RIGHT_WND,pRoomTab)==FALSE)
		{
			str = fMsg.GetKeyVal("MainRoom","InstallComponentError","建立A新组件失败");
			throw str;
		}
		

		pRoomInfoItem->pGameRoomWnd=new CGameRoomEx(szGameName,pRoomInfoItem,&m_PlaceUserInfo,pRoomInfoItem->pRoomInfoWnd);
		pRoomInfoItem->pRoomInfoWnd->m_pMainWnd=(CWnd*)pRoomInfoItem->pGameRoomWnd;
		pRoomInfoItem->pRoomInfoWnd->SetGameRoomWnd((CWnd*)pRoomInfoItem->pGameRoomWnd);

        // 踢人卡、防踢卡使用时间设置功能
        CGameRoomEx* pGameRoom = (CGameRoomEx*)pRoomInfoItem->pGameRoomWnd;
        pGameRoom->SetKickPropTime(m_iKickPropTotalTime);
        pGameRoom->SetAntiKickPropTime(m_iAntiKickPropTotalTime);


		if (pRoomInfoItem->pGameRoomWnd->Create(IDD_GAME_ROOM,pRoomTab)==FALSE) 
		{
			str = fMsg.GetKeyVal("MainRoom","InstallComponentError","建立A新组件失败");
			throw str;
		}

		pRoomInfoItem->pRoomInfoWnd->ShowWindow(SW_SHOW);

        // 如果是比赛场，并且没有排队机，隐藏右侧控件
        if(pRoomInfoItem->pComRoomInfo.uComType == TY_MATCH_GAME &&
           !(pRoomInfoItem->pComRoomInfo.dwRoomRule & GRR_QUEUE_GAME))
        {
            pRoomInfoItem->pRoomInfoWnd->ShowWindow(SW_HIDE);
        }

		AfxSetResourceHandle(GetModuleHandle(NULL));	

	}
	catch (...)
	{
		//清理信息
		delete pRoomInfoItem->pRoomInfoWnd;
		delete pRoomInfoItem->pGameRoomWnd;
		memset(pRoomInfoItem,0,sizeof(RoomInfoStruct));
		return false; 
	}

	//更新界面
	HICON hRoomIcon=CPlaceResource::GetRoomIcon(pGameRoomItem,szProessName);
	if (pRoomInfoItem->pComRoomInfo.uComType != TY_MATCH_GAME ||
        (pRoomInfoItem->pComRoomInfo.dwRoomRule & GRR_QUEUE_GAME))   //比赛排队机场
	{
		pRoomInfoItem->uTabIndex=pRoomTab->AddTabPage(m_RoomInfo[m_bNowRoomIndex].pGameRoomWnd,
			m_RoomInfo[m_bNowRoomIndex].pRoomInfoWnd,szGameName,hRoomIcon,true,true);
		pRoomTab->SetActivePageByExtIndex(pRoomInfoItem->uTabIndex);
	}
	else
	{
		pRoomInfoItem->uTabIndex=pRoomTab->AddTabPage(m_RoomInfo[m_bNowRoomIndex].pGameRoomWnd,
			m_RoomInfo[m_bNowRoomIndex].pRoomInfoWnd,szGameName,hRoomIcon,false,false);
	}
	/// 如果是排队机房间，则关闭鼠标点击事件，否则开启鼠标点击事件
	pRoomTab->EnableLBClick((pRoomInfoItem->pComRoomInfo.dwRoomRule & GRR_QUEUE_GAME) == 0);
	/////pComRoomInfo
	m_pGameListWnd->m_GameList.IEnterARoomOrLeave(pComRoomInfo->uRoomID,true);
	if(m_bShowSysmsg)
	{
		m_bShowSysmsg = false;
		// 创建下载线程
		::CreateThread(NULL,0,DownLoadNewIni,this,0,NULL);
		return true;
	}
	if(!m_bShowSysmsg)
		WriteNews();
	return true;
}

bool CMainRoomEx::OnCentranceGameRoom(CAFCRoomItem * pGameRoomItem)
{
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	CString str;
	CString strTitle;
	strTitle = fMsg.GetKeyVal("MainRoom","MessageTitle","系统消息");

	if ((pGameRoomItem==NULL)||(pGameRoomItem->m_uDataType!=GLK_GAME_ROOM))
	{
		str = fMsg.GetKeyVal("MainRoom","SelectGameRoom","请先选择游戏房间");
		
		//AFCMessageBox(str,strTitle);//,MB_ICONINFORMATION,this);
        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,strTitle,str);
		return false;
	}

	////判断是否特殊房间
	//ComRoomInfo * pComRoomInfo=&pGameRoomItem->m_RoomInfo;

	/////{{ Modified by lxl 2010-10-08	进入房间时判断该房间是否具有密码，如果有，则提示输入密码框
	//if (pComRoomInfo->bHasPassword)
	//{
	//	/// 输入密码对话框
	//	CPassDesk pdDlg;
	//	if(pdDlg.DoModal()==IDCANCEL)
	//		return 0;

	//	char szSrcPass[MAX_PATH];
	//	strcpy(szSrcPass, pdDlg.m_pass.GetBuffer());

	//	/// 加密字符串
	//	MD5_CTX Md5;
	//	Md5.MD5Update((unsigned char *)szSrcPass,lstrlen(szSrcPass));
	//	unsigned char szMDTemp[16];
	//	Md5.MD5Final(szMDTemp);
	//	for (int i=0;i<16;i++) wsprintf(&szSrcPass[i*2],"%02x",szMDTemp[i]);

	//	/// 提交密码给服务器
	//	MSG_GP_CheckRoomPasswd crp;
	//	crp.uRoomID = pComRoomInfo->uRoomID;
	//	strcpy(crp.szMD5PassWord, szSrcPass);

	//	SendData(&crp,sizeof(MSG_GP_CheckRoomPasswd),MDM_GP_LIST, ASS_GP_ROOM_PASSWORD,0);

	//	return 1;
	//}
	//else
	//{
		return 	EnterRoom(pGameRoomItem);
	//}
}

bool CMainRoomEx::OnGetRoomInfo(CAFCRoomItem * pGameRoomItem)
{
	m_pRoomItemInfo = pGameRoomItem;
	if (pGameRoomItem->m_RoomInfo.dwRoomRule & GRR_CONTEST)
	{
		MSG_GP_GetContestRoomID getContestRoomID;
		getContestRoomID.iUserID = m_PlaceUserInfo.dwUserID;
		getContestRoomID.iGameID = pGameRoomItem->m_RoomInfo.uNameID;
		getContestRoomID.iContestID = pGameRoomItem->m_RoomContest[0].iContestID;
		getContestRoomID.iUpPeople = pGameRoomItem->m_RoomInfo.iUpPeople;
		SendData(&getContestRoomID, sizeof(MSG_GP_GetContestRoomID), MDM_GP_GET_CONTEST_ROOMID, 0, 0);
		return true;
	}
	return false;
}

void CMainRoomEx::BeforeNavigate2Explorer(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers, BOOL* Cancel)
{
	if(URL->vt!=VT_BSTR)
		return;
	CString surl=(char*)_bstr_t(URL->bstrVal);
	surl.MakeLower();

	if(surl.Left(6)=="bzw://")
	{
		*Cancel=TRUE;
		if(surl.Find("avatar/change")==6)
		{
			UINT uNewLogoID = atoi(GetUrlValue(surl,"newlogoid"));
			__int64 i64BankChangeMoney = atoi(GetUrlValue(surl,"price"));
			m_pGameListWnd->m_PlaceUserInfo->i64Bank -= i64BankChangeMoney;

			//更新银行
			UpdateBankMoneyToAllRoom(m_pGameListWnd->m_PlaceUserInfo->i64Bank,NULL);
			if(m_pBankWndEx)
			{
				m_pBankWndEx->SetWealthInfor(m_pGameListWnd->m_PlaceUserInfo->i64Bank,0);
			}

			//更新形象
			ProcessMyAvatarChange(uNewLogoID);
		}
	}
	else
	{
		int iIndex = surl.ReverseFind('//');
		if (0 >= iIndex)
			return;
		CString strParam = surl.Mid(iIndex+1);
		strParam.MakeLower();
		if (!strcmp(strParam, "matchlisttop.aspx?type=1"))
		{
			//用户在官网点击报名斗地主比赛
			m_pGameListWnd->EnterAssignRoom(1, 10001300);
			CString url;
			url.Format("%s",Glb().m_CenterServerPara.m_strHomeADDR);
			IE(url);
		}
	}
}

void CMainRoomEx::NavigateErrorExplorer(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Frame, VARIANT* StatusCode, BOOL* Cancel)
{
	//this->ShowWindow(SW_HIDE);
	//CMainRoomEx *pWnd=(CMainRoomEx*)pParentWnd;
	//for(int i = 0; i < MAX_GAME_ROOM; i ++)
	//{
	//	if(pWnd->m_RoomInfo[i].pGameRoomWnd != NULL)
	//	{
	//		((CGameRoomEx*)pWnd->m_RoomInfo[i].pGameRoomWnd)->InsertSystemMessageWithGame("对不起，打开道具箱出错");
	//	}
	//}
}

//处理形象变化，JianGuankun 2012.06.26
void CMainRoomEx::ProcessMyAvatarChange(UINT uNewLogoID)
{
	m_pGameListWnd->m_PlaceUserInfo->bLogoID = uNewLogoID;
	m_pGameListWnd->SetUserInfoWnd(m_pGameListWnd->m_PlaceUserInfo);

	//游戏房间的银行
	for (BYTE i = 0; i < MAX_GAME_ROOM; ++i)
	{
		// 游戏中游戏界面右边消息栏
		CGameRoomEx* pGameRoomEx = (CGameRoomEx*)m_RoomInfo[i].pGameRoomWnd;

		if (NULL != pGameRoomEx)
		{
			DWORD dwUserID = m_pGameListWnd->m_PlaceUserInfo->dwUserID;

			//更新房间形象
			pGameRoomEx->OnAvatarChange(dwUserID,uNewLogoID);

			MSG_GR_AvatarChange tagAvatarChange;
			tagAvatarChange.dwUserID = dwUserID;
			tagAvatarChange.nNewLogoID = uNewLogoID;
			pGameRoomEx->SendData(&tagAvatarChange,sizeof(MSG_GR_AvatarChange),MDM_GR_ROOM,ASS_GR_AVATAR_LOGO_CHANGE,0);
		}	
	}

	return;


}

//列表信息
bool CMainRoomEx::OnListMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	//如果是返回进入密码房间成功的消息，则进入房间，否则按原有方式处理
	if (ASS_GP_ROOM_PASSWORD == pNetHead->bAssistantID)
	{
		DL_CheckRoomPasswd *p = (DL_CheckRoomPasswd *)pNetData;
		if (p->bRet)
		{
			CAFCRoomItem * pGameRoomItem  = m_pGameListWnd->m_GameList.FindRoomItem(p->uRoomID);
			this->EnterRoom(pGameRoomItem);
		}
		else
		{
            DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","您的密码输入错误，暂时不能进入该房间游戏!");
			CGameRoomEx* pGameRoom = GetGameRoom(p->uRoomID);
			if (pGameRoom)
			{
				pGameRoom->OnBnClickedClose();
			}
		}
		return TRUE;
	}
	//先树型例表处理
	m_pGameListWnd->OnListMessage( pNetHead,  pNetData,  uDataSize,  pClientSocket);

	//获得游戏列表后增加游戏列表提示
	CString strPath = CBcfFile::GetAppPath();
	strPath += "\\ListTip.ini";
	UINT uNotify = GetPrivateProfileInt("IsNotify", "notify", 0, strPath);

	if (m_bListmessage && !bTipInit &&uNotify && m_HaveSellList)
	{
		bTipInit = true;
		m_pListTip = new CGameListTip();
		m_pListTip->DoModal();
	}
	m_bListmessage = true;
	return TRUE;	
}


//下载news.ini文件
DWORD WINAPI CMainRoomEx::DownLoadNewIni(LPVOID lparam)
{
	CMainRoomEx * p =(CMainRoomEx *)lparam;
	CString s=CBcfFile::GetAppPath ();//本地路径
	TCHAR szUrl[1000];
	CString NewUrl;
	CBcfFile f( s + "bzgame.bcf");
	CString key="BZW";
	NewUrl=f.GetKeyVal(key,"newurl","");
	if(NewUrl == "")
		return 0;
	lstrcpy(szUrl,NewUrl);
	CString name;
	if(InternetGetFile(szUrl,TEXT("news.ini"),name) == 0)
		p->WriteNews();
	return 0;
}
//通知房间显示新闻
void CMainRoomEx::WriteNews()
{
	for (BYTE i=0;i<MAX_GAME_ROOM;i++)
	{
		if (m_RoomInfo[i].bAccess)
		{
			m_RoomInfo[i].pGameRoomWnd->SendMessage(WM_REFLASNEWS,0,0);
			m_RoomInfo[i].pGameRoomWnd->SendMessage(WM_REFLAS_SYS_MSG,0,0);
		}
	}
}

//网络读取消息
bool CMainRoomEx::OnSocketReadEvent(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	m_nKeepAliveCount = 0;
	//TRACE("%d GET Message MAINID:%d\n",GetTickCount(),pNetHead->bMainID);
	switch (pNetHead->bMainID)
	{
	 case MDM_GP_NETSIGNAL:
        {
            getNetSignal(pNetData);
            return true;
        }

	case MDM_CONNECT:	//连接消息 
		{
			return OnConnectMessage(pNetHead,pNetData,uDataSize,pClientSocket);
		}

	case MDM_GP_REQURE_GAME_PARA:	//中心服务全局参数//请求游戏全局参数
		{

			CenterServerMsg * msg=(CenterServerMsg *)pNetData;
			if(msg)
			{
				CenterServerMsg& par=Glb().m_CenterServerPara;
				////主服务器端口号
				par.m_is_haveZhuanZhang = msg->m_is_haveZhuanZhang;
				par.m_iMainserverPort=msg->m_iMainserverPort;
				par.m_nHallInfoShowClass=msg->m_nHallInfoShowClass;
				memcpy(par.m_strGameSerialNO,msg->m_strGameSerialNO,sizeof(par.m_strGameSerialNO));
				memcpy(par.m_strMainserverIPAddr,msg->m_strMainserverIPAddr,sizeof(par.m_strMainserverIPAddr)  );
				memcpy(par.m_strHomeADDR,msg->m_strHomeADDR,sizeof(par.m_strHomeADDR)  );
				memcpy(par.m_strWebRootADDR,msg->m_strWebRootADDR,sizeof(par.m_strWebRootADDR)  );
				memcpy(par.m_strHelpADDR,msg->m_strHelpADDR,sizeof(par.m_strHelpADDR)  );
				memcpy(par.m_strDownLoadSetupADDR,msg->m_strDownLoadSetupADDR,sizeof(par.m_strDownLoadSetupADDR)  );
				memcpy(par.m_strDownLoadUpdatepADDR,msg->m_strDownLoadUpdatepADDR,sizeof(par.m_strDownLoadUpdatepADDR));
				memcpy(par.m_strRallAddvtisFlashADDR,msg->m_strRallAddvtisFlashADDR,sizeof(par.m_strRallAddvtisFlashADDR));
				memcpy(par.m_strRoomRollADDR,msg->m_strRoomRollADDR,sizeof(par.m_strRoomRollADDR));
				par.m_nEncryptType = msg->m_nEncryptType;		//平台加密方式
				par.m_nFunction = msg->m_nFunction;
				par.m_lNomalIDFrom = msg->m_lNomalIDFrom;
				par.m_lNomalIDEnd = msg->m_lNomalIDEnd;

				m_bGetPara = true;
			}	
			if(Glb().m_UpdateUrl=="")
			{
				Glb().m_UpdateUrl=Glb().m_CenterServerPara.m_strDownLoadUpdatepADDR;
			}
			//上方flash
			if(lstrlen(Glb().m_CenterServerPara.m_strRallAddvtisFlashADDR)>0)
			{
				m_TopWnd->SetFlashAd();
				if (m_TopDuWnd->GetHWND())
				{
					m_TopDuWnd->SetFlashAd();
				}
			}
			CString strBuffer;
			strBuffer.Format(TEXT(Glb().m_CenterServerPara.m_strRallAddvtisFlashADDR),1,1);
			m_WebView.Navigate(strBuffer,NULL,NULL,NULL,NULL);
			//活动广告条
			if(lstrlen(Glb().m_CenterServerPara.m_strRoomRollADDR)>0)
			{
				//CNewGameClass * pRoomTab=&((CGamePlaceDlg *)AfxGetMainWnd())->m_RoomTabWnd;
				//pRoomTab->SetAd(msg->m_strRoomRollADDR);
				//m_pActWnd->SetAd(msg->m_strRoomRollADDR);
			}
			//连接中心服务器成功后，显示页面
			CString url;
			url.Format("%sgmain.aspx",Glb().m_CenterServerPara.m_strHomeADDR);
			IE(url);

			if(msg->m_nIsUsingIMList == 0)
			{
				m_pGameListWnd->m_btIMList.ShowWindow(SW_HIDE);
			}
			else
			{
				m_pGameListWnd->m_btIMList.ShowWindow(SW_SHOW);
			}
#ifndef MY_DEV
			// PengJiLin, 2011-5-31, 使用资源里的文件版本号，不再使用文本文件中的。
			CString strVersion = GetResourceFileVer();

			//strVersion.Format("BZW.%d.%03d",VER_MAIN,VER_MIDDLE);
			CString s = Glb().m_CenterServerPara.m_strGameSerialNO;
			if(strcmp(s.GetBuffer(),strVersion)!=0)
			{
				STARTUPINFO StartupInfo;
				ZeroMemory(&StartupInfo, sizeof(STARTUPINFO));
				StartupInfo.cb = sizeof(STARTUPINFO);
				PROCESS_INFORMATION ProcessInfo;
				TCHAR line[256];
				ZeroMemory(line, sizeof(line));
				char cline[256];
				ZeroMemory(cline, sizeof(cline));

				string _t1 = Glb().m_CenterServerPara.m_strDownLoadUpdatepADDR;
				string ul = JoinUrl(_t1, "update_version.txt");
				string uv = ul;
				//////////////////////////////////////////////////////////////////////////
				//string _path;
				//_path = GetAppPath();
				//sprintf(up, "-n %s -s %s\\ -v %s -ul %s%s -uv %s%s -sm %d", "面对面游戏", _path.c_str(), "0", Glb().m_UpdateUrl, ul,Glb().m_UpdateUrl,uv, 1);

				
				char path[256];
				wsprintf(path,"%s\\updateconfig.ini",GetAppPath().c_str());
				CINIFile f(path);
				CString m_key="config";
				CString n;
				CString v; 
				int  sm = 1;

				n=f.GetKeyVal(m_key,"-n","游戏更新");//游戏名称	
				sm=f.GetKeyVal(m_key,"-sm",1);//显示方式
				v=f.GetKeyVal(m_key,"-v","0");//版本

				sprintf(cline, "-n %s -s %s -v %s -ul %s -uv %s -sm %d", n, /*CBcfFile::GetAppPath()*/GetAppPath().c_str(), v, ul.c_str(), ul.c_str(), sm);
				char2Tchar(cline, line);				

				//-异步启动- update.exe -------------------------2012.08.24 yyf
				//初始化 sDownLoadUpdateMsg 然后再传到 下载更新线程   DownLoadUpdate(LPVOID lparam)
				sDownLoadUpdateMsg sUpdateMsg; 
				//-----------------
				UpdateStructMsg updateMsg;
				memset(&updateMsg,0,sizeof(UpdateStructMsg));
				updateMsg.command = 1;
				updateMsg.nRoomID =-1;//-1 表示大厅 
				wsprintf(updateMsg.g_GameName,"%s","游戏大厅");
				wsprintf(updateMsg.g_url,"%supdate_version.txt",Glb().m_UpdateUrl);//,uv
				wsprintf(updateMsg.g_ver,"%s",v); 
				sUpdateMsg.pUpdateStructMsg = &updateMsg;
				//-----------------
				UPDATE *lp=(LPUPDATE)malloc(sizeof(UPDATE));
				lp->hProcess=ProcessInfo.hProcess;
				lp->uNameID=-1;
				lp->pGameRoomItem=NULL;//pGameRoomItem;
				sUpdateMsg.pUPDATE = lp;
				//-----------------
				sUpdateMsg.pCMainRoomEx = this;
				//-----------------
				CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
				sUpdateMsg.pCBcfFile = &fMsg;
				//-----------------

				DWORD dwThreadID = 0;
				HANDLE hThread = CreateThread(0,0,DownLoadUpdate,&sUpdateMsg,0,&dwThreadID);

				int nStartTime = fMsg.GetKeyVal("MainRoom","DownloadStartTime",10000);

				//连接update.exe超时
				if (WaitForSingleObject(hThread,nStartTime)==WAIT_TIMEOUT) 
				{
					if(m_CSocket32.GetProcessInfo().hProcess)
					{
						::TerminateProcess( m_CSocket32.GetProcessInfo().hProcess, 0 );
					}
					m_CSocket32.Close();

					CString str;
					CString strTitle;
					str = fMsg.GetKeyVal("MainRoom","DownLoadOver","下载游戏更新超时");
					strTitle = fMsg.GetKeyVal("MainRoom","MessageTitle","提示");
					
					//AFCMessageBox(str,strTitle);
                    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,strTitle,str);
				}
				if (hThread)
				{
					::CloseHandle(hThread);
				}
				//----------------------------------------------------------
				CloseSocket (true);
				AfxGetApp( )->m_pMainWnd->DestroyWindow();
				return true;		
			}
#endif
			CloseSocket (true);		

			return true;
		}
	case MDM_GP_LOGON:	//登陆信息
		{
			return OnLogonMessage(pNetHead,pNetData,uDataSize,pClientSocket);
		}
	case MDM_GP_USERINFO: //用户资料管理 JianGuankun 2011.11.13
		{
			if (ASS_GP_USERINFO_ACCEPT == pNetHead->bAssistantID)
			{
				if (m_pSafeCenterWnd)
				{
					m_pSafeCenterWnd->SetItemSafeState(CSafeCenterWnd::CIT_IDVerify,
						strlen(m_PlaceUserInfo.szIDCardNo) > 0);
				}
				//在大厅或房间弹出提示消息
				//AFCMessageBox("修改成功","系统提示信息");//,MB_OK,this);
                DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","修改成功","系统提示信息");

				if (m_pSafeCenterWnd)
				{
					CUserInfoWnd** pUserInfoWnd = &m_pGameListWnd->m_pUserInfoWnd;
					if (*pUserInfoWnd)
					{
						(*pUserInfoWnd)->Close();
					}
				}
			}
			else if (ASS_GP_USERINFO_NOTACCEPT == pNetHead->bAssistantID)
			{
				//在大厅或房间弹出提示消息
				//AFCMessageBox("修改失败","系统提示信息");//,MB_OK,this);
                DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","修改失败","系统提示信息");
			}

			//Add by JianGuankun 2012.1.4 查询用户ID或昵称结果
			else if (ASS_GP_USERINFO_NICKNAMEID == pNetHead->bAssistantID)
			{
				GetNickNameID_t* p = (GetNickNameID_t*)pNetData;

				//大厅银行
				if (m_pBankWndEx)
				{
					m_pBankWndEx->UpdateNicknameIDResult(p);
					return true;
				}

				if (m_pBankWnd)
				{
					m_pBankWnd->UpdateNicknameIDResult(p);
				}

				//游戏房间的银行
				for (BYTE i = 0; i < MAX_GAME_ROOM; ++i)
				{
					if (m_RoomInfo[i].bAccess)
					{
						// 游戏中游戏界面右边消息栏
						CGameRoomEx* pGameRoomEx = (CGameRoomEx*)m_RoomInfo[i].pGameRoomWnd;
						if (NULL != pGameRoomEx)
						{
							if (pGameRoomEx->m_pBankWndEx)
							{
								pGameRoomEx->m_pBankWndEx->UpdateNicknameIDResult(p);
								break;
							}

							if (pGameRoomEx->m_pBankWnd)
							{
								pGameRoomEx->m_pBankWnd->UpdateNicknameIDResult(p);
								break;
							}

						}					
					}//bAccess
				}
			}
			//End Add

			return true;
		}
	case MDM_GP_LIST:	//游戏列表
	case MDM_GP_IM:
		{
			OutputDebugString("sdp_list 收到游戏列表回应");

			return OnListMessage(pNetHead,pNetData,uDataSize,pClientSocket);
		}
	case MDM_GP_SYSMSG:	// 系统消息
		{
			return OnSystemMessage(pNetHead,pNetData,uDataSize,pClientSocket);
		}
	case MDM_GP_MESSAGE://房间的消息 新闻，小喇叭
		{
			//wushuqun 2009.6.9
			// qy 修改大厅系统消息范围 20090212
			if (MDM_GP_SYSMSG == pNetHead->bAssistantID)
			#pragma region MDM_GP_SYSMSG
			{
				MSG_GR_RS_NormalTalk * pNormalTalk=(MSG_GR_RS_NormalTalk *)pNetData;
				if ((uDataSize<=(sizeof(MSG_GR_RS_NormalTalk)-sizeof(pNormalTalk->szMessage)))
					||(uDataSize>sizeof(MSG_GR_RS_NormalTalk)))
				{
					return true;
				}
				if ((pNormalTalk->iLength+1)!=(uDataSize+sizeof(pNormalTalk->szMessage)-sizeof(MSG_GR_RS_NormalTalk)))
				{
					return true;
				}

				// 截取有效数据
				pNormalTalk->szMessage[pNormalTalk->iLength]=0;
				for (BYTE i=0;i<MAX_GAME_ROOM;i++)
				{
					if (m_RoomInfo[i].bAccess)
					{
						// 游戏中游戏界面右边消息栏
						if (NULL != m_RoomInfo[i].pGameRoomWnd)
						{
							CGameRoomEx *  pT = (CGameRoomEx * )m_RoomInfo[i].pGameRoomWnd;
							pT->InsertSystemMessageWithGame(pNormalTalk->szMessage);
						}	// 游戏房间右边消息栏
						else if (NULL != m_RoomInfo[i].pRoomInfoWnd)
						{
							AFCPlaySound(GetModuleHandle(FACE_DLL_NAME),TEXT("SYSTEM_MSG"));
							m_RoomInfo[i].pRoomInfoWnd->m_MessageHandle.InsertSystemMessage(pNormalTalk->szMessage); 
						}						
					}
				}
				return true;
			}
			// end
			#pragma endregion MDM_GP_SYSMSG

			//消息 新闻
			if(pNetHead->bAssistantID==ASS_GP_NEWS_SYSMSG)
			{				
				//下载news.ini
				::CreateThread(NULL,0,DownLoadNewIni,this,0,NULL);
				return true;
			}
			
			if(pNetHead->bAssistantID == ASS_GP_GETMARQUEEUINFORM)
			{
				MSG_MarQueueInfo *pMarQueueInfo = (MSG_MarQueueInfo*)pNetData;
				if(pMarQueueInfo != NULL)
				{
					memcpy(&m_tMarQueueInform,pMarQueueInfo,sizeof(MSG_MarQueueInfo));
				}

				
				for (int i = 0;i<8;++i)
				{
					CString cs;
					cs.Format("chs count:%d,buffer=%s",m_tMarQueueInform.InformCount,m_tMarQueueInform.buffer[i]);
					OutputDebugString(cs);
				}

			}
			return true;
		}
	case MDM_GR_USER_LOGO:
	{
		if(!logoUpload)
		{
			logoUpload=new CLogoUpload;
			logoUpload->Create(IDD_LOGOUPLOAD,this);
			logoUpload->SetParentWnd(this);
		}
		logoUpload->OnNetMessage(pNetHead,pNetData,uDataSize);
		break;
	}
	case MDM_GP_NOTICE_MSG:
		{
			if(ASS_GR_TRANSFER_MONEY == pNetHead->bAssistantID)		//转账成功后，提示消息给被转账人 add by xqm,lxl 2010-11-10  
			{
				//效验数据
				if (uDataSize!=sizeof(MSG_GR_S_TransferMoney)) return false;
				MSG_GR_S_TransferMoney * pTransfer=(MSG_GR_S_TransferMoney *)pNetData;
				/// 转帐成功
				if(ASS_GR_TRANSFER_SUC == pNetHead->bHandleCode)
				{					
					if (pTransfer->destUserID==m_PlaceUserInfo.dwUserID)
					{
						CString strBuffer;
						CString str;
						TCHAR szNum[128];
						TCHAR szAct[128];				
						GetNumString(szNum, pTransfer->i64Money, Glb().m_nPowerOfGold/*, false*/,Glb().m_bUseSpace, Glb().m_strSpaceChar);
						GetNumString(szAct, pTransfer->i64ActualTransfer, Glb().m_nPowerOfGold/*, false*/,Glb().m_bUseSpace, Glb().m_strSpaceChar);
						
						CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
						bool bUseNickName = fMsg.GetKeyVal("PBankDlg","UseNickName",0);  //转账提示信息可配是否显示昵称 add by lxl 2010-11-10
						if(bUseNickName)
						{
							str = fMsg.GetKeyVal("GameRoom","BeTransfered",
								"<p 5>尊敬的用户，<b>%s</b>(%d) 向您转帐。<n><y 30>      转帐数额：<b>%s</b> 金币</y><n>扣手续实际到账：<b>%s</b> 金币</p>");
							strBuffer.Format(str,pTransfer->szNickName,pTransfer->UserID,szNum,szAct);
						}
						else
						{
							str = fMsg.GetKeyVal("GameRoom","BeTransfered",
								"<p 5>尊敬的用户，<b>%d</b> 向您转帐。<n><y 30>      转帐数额：<b>%s</b> 金币</y><n>扣手续实际到账：<b>%s</b> 金币</p>");
							strBuffer.Format(str, pTransfer->UserID,szNum,szAct);
						}

						m_PlaceUserInfo.i64Bank+=pTransfer->i64ActualTransfer;
						UpdateMeInfo(0, false/*true*/, 0, false);         // PengJiLin, 2010-10-9, 避免金币显示为0
						
						//游戏里的右下角显示系统消息
						if (GetGameRoom() != NULL)
						{
							//在房间右下角发送一条系统提示消息
							//GetGameRoom()->m_pRightWnd->m_MessageHandle.InsertSystemMessage(strBuffer.GetBuffer());
							
							NetMessageHead nethead;
							MSG_GA_S_Message	msg;
							memset(&nethead,0,sizeof(nethead));
							memset(&msg,0,sizeof(msg));
							nethead.bMainID = MDM_GM_MESSAGE;
							nethead.bAssistantID = ASS_GM_SYSTEM_MESSAGE;
							strcpy(msg.szMessage, strBuffer);
						
							msg.bShowStation=0x01;
						
							if (GetGameRoom()->GetGameFrame() != NULL)
							{
								GetGameRoom()->GetGameFrame()->OnSocketReadEvent(&nethead,&msg,sizeof(MSG_GA_S_Message),NULL);
							}							
						}
						else
						{
							AFCPlaySound(GetModuleHandle(FACE_DLL_NAME),TEXT("SYSTEM_MSG"));
						}

						//在大厅或房间弹出提示消息
						DUIMessageBox(m_hWnd,MB_OK|MB_ICONINFORMATION,"系统提示信息",strBuffer);
					}		
				}
			}
			return true;
		}

	//用户道具
	case MDM_GP_PROP:
		{
			if(!propPannel)
			{
				propPannel=new CPropPannel();
				propPannel->pParentWnd=(CWnd*)this;
				propPannel->Create(IDD_PROPPANNEL,this);
			}

			propPannel->OnNetMessage(pNetHead,pNetData,uDataSize);


			MSG_PROP_RESULT *ResultInfo = new MSG_PROP_RESULT;
			ZeroMemory(ResultInfo,sizeof(MSG_PROP_RESULT));
			switch (pNetHead->bAssistantID)
			{
			case ASS_PROP_BUY_VIP:
				{
					if( pNetHead->bHandleCode == DTK_GR_VIP_BUY_SUCCEED)									//VIP购买失败；
					{
						ResultInfo->dwResult = 7;
					}
					else
					{	
						if(pNetHead->bHandleCode==DTK_GP_VIP_BUY_NOMONEY)									//vip钱不够
						{
							ResultInfo->dwResult = 6;
						}
						else if (pNetHead->bHandleCode==DTK_GR_VIP_BUY_NOUSER)								//赠送的玩家不存在
						{
							ResultInfo->dwResult = 8;
						}
						else if (pNetHead->bHandleCode==DTK_GR_VIP_GIVE_SUCCESS)								//赠送VIP成功
						{
							ResultInfo->dwResult = 9;
						}
						else																				//VIP购买失败
						{
							ResultInfo->dwResult = 5;
						}
					}
					if (m_RoomInfo[0].pGameRoomWnd != NULL)
						m_RoomInfo[0].pGameRoomWnd->PostMessage(WM_GP_PROP,(WPARAM)ResultInfo, sizeof(MSG_PROP_RESULT));
				}
				break;
				
			case ASS_PROP_BUY:
			case ASS_PROP_BUY_NEW:
				if (DTK_GP_PROP_BUY_NOMONEY == pNetHead->bHandleCode)										//购买道具没钱
				{
					ResultInfo->dwResult = 10;
					if (m_RoomInfo[0].pGameRoomWnd != NULL)
						m_RoomInfo[0].pGameRoomWnd->PostMessage(WM_GP_PROP,(WPARAM)ResultInfo, sizeof(MSG_PROP_RESULT));
				}
				else if( pNetHead->bHandleCode!=DTK_GP_PROP_BUY_SUCCEED && pNetHead->bHandleCode != DTK_GP_PROP_BUYANDUSE_SUCCEED)									//道具购买失败；
				{
					ResultInfo->dwResult = 1;
					if (m_RoomInfo[0].pGameRoomWnd != NULL)
						m_RoomInfo[0].pGameRoomWnd->PostMessage(WM_GP_PROP,(WPARAM)ResultInfo, sizeof(MSG_PROP_RESULT));
				}
				else if (pNetHead->bHandleCode != DTK_GP_PROP_BUYANDUSE_SUCCEED)
				{	
					ResultInfo->dwResult = 0;
					if (m_RoomInfo[0].pGameRoomWnd != NULL)											//购买道具成功										
						m_RoomInfo[0].pGameRoomWnd->PostMessage(WM_GP_PROP, (WPARAM)ResultInfo, sizeof(MSG_PROP_RESULT));
				}
				break;
			case ASS_PROP_GIVE:
				if(pNetHead->bHandleCode==DTK_GP_PROP_GIVE_SUCCEED_GIVER)							//赠送成功
				{
					ResultInfo->dwResult = 2;
					if (m_RoomInfo[0].pGameRoomWnd != NULL)
						m_RoomInfo[0].pGameRoomWnd->PostMessage(WM_GP_PROP, (WPARAM)ResultInfo, sizeof(MSG_PROP_RESULT));
				}
				else
				{
					_TAG_PROP_GIVE* propOGive = (_TAG_PROP_GIVE*)pNetData;

					//添加条件，只有送道具的玩家才显示道具赠送失败的消息
					//Added by JianGuankun 2012.4.16

					if (m_PlaceUserInfo.dwUserID == propOGive->dwUserID)
					{
						if(pNetHead->bHandleCode==DTK_GP_PROP_GIVE_NOTARGETUSER)						//赠送的玩家不存在
						{
							ResultInfo->dwResult = 3;
							if (m_RoomInfo[0].pGameRoomWnd != NULL)
								m_RoomInfo[0].pGameRoomWnd->PostMessage(WM_GP_PROP, (WPARAM)ResultInfo, sizeof(MSG_PROP_RESULT));
						}
						else 																			//赠送道具失败
						{
							ResultInfo->dwResult = 4;
							if (m_RoomInfo[0].pGameRoomWnd != NULL)
								m_RoomInfo[0].pGameRoomWnd->PostMessage(WM_GP_PROP, (WPARAM)ResultInfo, sizeof(MSG_PROP_RESULT));
						}
					}
		
				}
				break;
			default:
				break;
			}
		}
		break;
	case MDM_GP_LOCK_ACCOUNT:	//请求锁定和解除锁定结果 zxj 2009-11-16
		{
			OnLockAccountResult(pNetHead, pNetData, uDataSize, pClientSocket);
			break;
		}
	case MDM_GP_BIND_MOBILE:
		{
			MSG_GP_R_BindMobile* pBind = (MSG_GP_R_BindMobile*)pNetData;

			if( pBind == NULL)
			{
				return true;
			}

			if( pBind->dwUserID != m_PlaceUserInfo.dwUserID )
			{
				return true;
			}

			if( pBind->dwCommanType == 0 )	//请求解除绑定手机结果
			{
				if (m_pSafeCenterWnd)
				{
					CUserInfoWnd** pUserInfoWnd = &m_pGameListWnd->m_pUserInfoWnd;
					if (*pUserInfoWnd)
					{
						(*pUserInfoWnd)->Close();
					}
					m_pSafeCenterWnd->SetItemSafeState(CSafeCenterWnd::CIT_BindMobile,false);
				}
				//end add

				m_PlaceUserInfo.iBindMobile = 0;
				CGamePlaceDlg* pGamePlace = (CGamePlaceDlg*)AfxGetMainWnd();
				if (pGamePlace)
				{
					CUserInfoWnd* pUserInfoWnd = pGamePlace->m_GameListWnd.m_pUserInfoWnd;
					if (pUserInfoWnd)
					{
						pUserInfoWnd->OnBindMobileResult(false);
					}
				}

				if (!m_pSafeCenterWnd)
				{
					DUIMessageBox(m_hWnd,MB_OK|MB_ICONINFORMATION,"系统提示信息","已解除手机绑定。");
				}
			}
			else if( pBind->dwCommanType == 1)//请求绑定手机结果
			{
				m_PlaceUserInfo.iBindMobile = 1;
				CGamePlaceDlg* pGamePlace = (CGamePlaceDlg*)AfxGetMainWnd();
				if (pGamePlace)
				{
					CUserInfoWnd* pUserInfoWnd = pGamePlace->m_GameListWnd.m_pUserInfoWnd;
					if (pUserInfoWnd)
					{
						pUserInfoWnd->OnBindMobileResult(true);
					}
				}

				if (m_pSafeCenterWnd)
				{
					CUserInfoWnd** pUserInfoWnd = &m_pGameListWnd->m_pUserInfoWnd;
					if (*pUserInfoWnd)
					{
						(*pUserInfoWnd)->Close();
					}
					m_pSafeCenterWnd->SetItemSafeState(CSafeCenterWnd::CIT_BindMobile,true);
				}
				else
				{
					DUIMessageBox(m_hWnd,MB_OK|MB_ICONINFORMATION,"系统提示信息","绑定手机成功！");
				}
				//end add
			}

			break;
		}
	case MDM_GP_PLAYER_2_GM:
		{
			MSG_GP_S_Talk2GM_t *msg = (MSG_GP_S_Talk2GM_t*)pNetData;
			TCHAR*  s = new TCHAR[256];
			_tcscpy(s, msg->szMsg);
			switch (pNetHead->bAssistantID)
			{
			case ASS_GP_MSG_FROM_GM	:									///<接收从GM处发来的消息
				{
					if (msg->iUserID == 0)
					{
						if (m_RoomInfo[0].pGameRoomWnd != NULL)
							m_RoomInfo[0].pGameRoomWnd->PostMessage(WM_GM_MSG, WPARAM(s), 0);
					}
					else if (msg->iUserID == m_PlaceUserInfo.dwUserID)
					{
						for (map<int, CShortMessage*>::iterator i = m_pGMMsgDlgs.begin(); i != m_pGMMsgDlgs.end();)
						{
							if (i->second->m_iGmID == 0) 
							{
								delete i->second;
								i = m_pGMMsgDlgs.erase(i);
							}
							else i++;
						}
						if (m_pGMMsgDlgs.count(msg->iGMID) <= 0)
						{
							CShortMessage* pWnd = new CShortMessage(this);
                            pWnd->SetUserAccount(m_PlaceUserInfo.szName);
							pWnd->SetTalkInfo(msg->iGMID);
							pWnd->Create(IDD_SHORT_MESSAGE, GetDesktopWindow());
							pWnd->CenterWindow();
							pWnd->m_BtExpression.ShowWindow(SW_HIDE);
							pWnd->ShowWindow(SW_SHOWNOACTIVATE);
							CString s;
							if (pNetHead->bHandleCode != 0)
								pWnd->m_MessageHandle.InsertSystemMessage(msg->szMsg);
							else 
							{
								s.Format("管理员%d", msg->iGMID);
								pWnd->OnRecvMessage(s.GetBuffer(), msg->szMsg, RGB(255, 0, 0));
							}
							m_pGMMsgDlgs[msg->iGMID] = pWnd;
						}
						else
						{
							CShortMessage* pWnd = m_pGMMsgDlgs[msg->iGMID];
							pWnd->ShowWindow(SW_SHOWNOACTIVATE);
							CString s;
							if (pNetHead->bHandleCode != 0)
								pWnd->m_MessageHandle.InsertSystemMessage(msg->szMsg);
							else 
							{
								s.Format("管理员%d", msg->iGMID);
								pWnd->OnRecvMessage(s.GetBuffer(), msg->szMsg, RGB(255, 0, 0));
							}
						}
					}
					break;
				}
			case ASS_GR_MSG_BE_AWARD:
				{
					DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",s);
					delete []s;
					break;
				}
			case ASS_GR_MSG_BE_KICKED:									///<被GM踢出房间
				{
					CGMMsgDlg dlg(s);
					dlg.DoModal();
					delete []s;
					if (m_RoomInfo->pGameRoomWnd != NULL)
						m_RoomInfo[0].pGameRoomWnd->PostMessage(WM_GM_KICK, 0, 0);
					break;
				}
			case ASS_GR_MSG_BE_BANDID:									///<被GM封号
				{
					CGMMsgDlg dlg(s);
					dlg.DoModal();
					delete []s;
					if (m_RoomInfo->pGameRoomWnd != NULL)
						m_RoomInfo[0].pGameRoomWnd->PostMessage(WM_GM_KICK, 0, 0);
					PostQuitMessage(0);
					break;
				}
			case ASS_GP_GET_GM_LIST:
				{					
					int n = uDataSize / sizeof(MSG_GP_R_GmList_t);
					MSG_GP_R_GmList_t* pdata = (MSG_GP_R_GmList_t*)pNetData;
					if (m_pGMListDlg == NULL)
					{
						m_pGMListDlg = new CGMListDlg(this);
						m_pGMListDlg->Create(IDD_DLG_GMLIST, this);
					}
					m_pGMListDlg->OpenWindow(*pdata);
					break;
				}
			case ASS_GP_TALK_2_GM:
				{
					if (msg->iUserID == m_PlaceUserInfo.dwUserID)
					{
						for (map<int, CShortMessage*>::iterator i = m_pGMMsgDlgs.begin(); i != m_pGMMsgDlgs.end();)
						{
							if (i->second->m_iGmID == 0) 
							{
								delete i->second;
								i = m_pGMMsgDlgs.erase(i);
							}
							else i++;
						}
						if (m_pGMMsgDlgs.count(msg->iGMID) <= 0)
						{
							CShortMessage* pWnd = new CShortMessage(this);
                            pWnd->SetUserAccount(m_PlaceUserInfo.szName);
							AfxSetResourceHandle(GetModuleHandle(NULL));
							pWnd->SetTalkInfo(msg->iGMID);
							pWnd->Create(IDD_SHORT_MESSAGE, GetDesktopWindow());
							pWnd->CenterWindow();
							pWnd->m_BtExpression.ShowWindow(SW_HIDE);
							pWnd->ShowWindow(SW_SHOWNOACTIVATE);
							CString s;
							s.Format("%d", msg->iUserID);
							pWnd->OnRecvMessage(s.GetBuffer(), msg->szMsg, RGB(255, 0, 0));
							m_pGMMsgDlgs[msg->iGMID] = pWnd;
						}
						else
						{
							CShortMessage* pWnd = m_pGMMsgDlgs[msg->iGMID];
                            pWnd->SetUserAccount(m_PlaceUserInfo.szName);
							pWnd->ShowWindow(SW_SHOWNOACTIVATE);
							CString s;
							s.Format("%d", msg->iUserID);
							pWnd->OnRecvMessage(s.GetBuffer(), msg->szMsg, RGB(255, 0, 0));
						}
					}
				}
			}
			break;
		}
	case MDM_GP_RECEIVEMONEY:
		{
			TReceiveMoney* _p = (TReceiveMoney*)pNetData;
			if (_p != NULL)
			{
				if (pNetHead->bAssistantID == ASS_RECEIVE)
				{

					m_PlaceUserInfo.i64Bank += _p->i64Money;
					UpdateMeInfo(m_PlaceUserInfo.i64Money,TRUE, 0, 0);
					
					CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
					CString str;
					str = fMsg.GetKeyVal("GameListWnd","ReceiveResult","领取金币成功,您今天还有%d次领取机会");

					CString s;
					s.Format("领取金币成功,您今天还有%d次领取机会", _p->iTotal - _p->iCount);
					DUIOkeyMsgBox(m_hWnd,s);
				}
				else if (pNetHead->bAssistantID == ASS_RECEIVE_PASS)
				{
					if (m_TopDuWnd != NULL)
					{
						m_TopDuWnd->SetMaxOrRestoreBtn(false);
					}
					CString s;
					s.Format("当您游戏币低于%d的时候才可以领取哦", _p->i64MinMoney);
					DUIOkeyMsgBox(m_hWnd,s);
					if (m_TopDuWnd != NULL)
					{
						m_TopDuWnd->SetMaxOrRestoreBtn(true);
					}
				}
				else if (pNetHead->bAssistantID == ASS_RECEIVE_OVER)
				{
					CString s;
					s.Format("您今天已领取完毕");
					DUIOkeyMsgBox(m_hWnd,s);
				}
				else if (pNetHead->bAssistantID == ASS_RECEIVE_TIMELESS)
				{
					CString s;
					s.Format("您还需要过 %d 分钟才能领取",_p->iTime);
					DUIOkeyMsgBox(m_hWnd,s);
				}
				else if (pNetHead->bAssistantID == ASS_RECEIVE_ERROR)
				{
					CString s;
					s.Format("领取错误");
					DUIOkeyMsgBox(m_hWnd,s);
				}
			}

			break;
		}
	case MDM_GP_CHARMEXCHANGE:
		{
			if (pNetHead->bAssistantID == ASS_GETLIST)
			{
				if (CharmPannel != NULL)
					CharmPannel->OnNetMessage(pNetHead, pNetData, uDataSize);
			}
			else if (pNetHead->bAssistantID == ASS_EXCHANGE)
			{
				TCharmExchange_Item *_p = (TCharmExchange_Item*)pNetData;
				if (_p != NULL)
				{
					m_PlaceUserInfo.dwFascination -= _p->iPoint;
					UpdateMeInfo(m_PlaceUserInfo.i64Money, TRUE, m_PlaceUserInfo.dwFascination, TRUE);

					if (CharmPannel != NULL)
					{
						CharmPannel->OnNetMessage(pNetHead, pNetData, uDataSize);
					}

					if (_p->iPropID == 0)
					{
						m_PlaceUserInfo.i64Bank += _p->iPropCount;
						UpdateMeInfo(m_PlaceUserInfo.i64Money,TRUE, 0, 0);
					}
					else
					{
						_TAG_USERPROP *userProp=NULL;
						for (int i = 0; i < Glb().userPropLibrary.GetCount(); i++)
						{
							userProp = Glb().userPropLibrary.GetAt(i);
							if (userProp != NULL && userProp->nPropID == _p->iPropID)
							{
								userProp->nHoldCount += _p->iPropCount;
							}
						}
					}
					if (pNetHead->bHandleCode == 1)
						DUIMessageBox(m_hWnd,MB_OK|MB_ICONERROR,"系统提示信息","兑换出错");
					if (pNetHead->bHandleCode == 2)
						DUIMessageBox(m_hWnd,MB_OK|MB_ICONERROR,"系统提示信息","兑换出错");
					if (pNetHead->bHandleCode == 3)
						DUIMessageBox(m_hWnd,MB_OK|MB_ICONINFORMATION,"系统提示信息","魅力值点数不够!");
					if (pNetHead->bHandleCode == 4)
						DUIMessageBox(m_hWnd,MB_OK|MB_ICONINFORMATION,"系统提示信息","魅力值点数不够!");
					if (pNetHead->bHandleCode == 5)
						DUIMessageBox(m_hWnd,MB_OK|MB_ICONINFORMATION,"系统提示信息","兑换成功!");
				}
			}

			break;
		}
	case MDM_GP_MAIL:
		{
			TMailItem* _p = (TMailItem*)pNetData;
			if (_p != NULL)
			{
				//AFCMessageBox(_p->szMsg);
				if (_p->iMsgType == 0) //大厅邮件消息
				{
					if (m_pGameListWnd != NULL)
						m_pGameListWnd->AddMail(*_p);
				}
				else if (_p->iMsgType == 1) //大厅通知游戏显示走马灯消息
				{
					CGameRoomEx* pGameRoom = (CGameRoomEx*)m_RoomInfo[0].pGameRoomWnd;
					if (pGameRoom)
					{
						pGameRoom->BoardcastTrottingMsg(_p);
					}
				}
				else if (_p->iMsgType == 2) //大厅弹窗口消息
				{
					CMiniPopWnd* pMiniWnd = new CMiniPopWnd(this);
					pMiniWnd->SetMessage(_p);
					pMiniWnd->Create(NULL,NULL,UI_WNDSTYLE_DIALOG,0,0,0,0,0,NULL);
					pMiniWnd->ShowWindow(SW_SHOW);
					pMiniWnd->MoveWindowToRightBottom();
				}
				else if (_p->iMsgType == 3) //大厅公告消息
				{
					if (m_pActWnd)
					{
						m_pActWnd->SetAd(_p->szMsg);
					}
				}
			}
			
			break;
		}
	case MDM_GP_LOGONUSERS:
		{
			ONLINEUSERSCOUNT* totalOnlineUsers = static_cast<ONLINEUSERSCOUNT*>(pNetData);
			if (totalOnlineUsers != NULL)
			{
				SetonlineUserCount(totalOnlineUsers->uiLogonPeopCount);
			}
		}
		break;
	case MDM_GP_BANK: //银行消息
		{
			OnBankMessage(pNetHead,pNetData,uDataSize,pClientSocket);
			break;
		}

	case MDM_GP_SMS: //手机短信事务
		{
			if (pNetHead->bAssistantID == ASS_GP_SMS_VCODE) //收到短信验证码
			{
				MSG_GP_SmsVCode* _p = (MSG_GP_SmsVCode*)pNetData;

				if (_p != NULL)
				{
					if (::strlen(_p->szVCode) == 0)
					{
						#ifdef MY_MOBILE_VCODE_OFF
							return true;
						#endif

						DUIOkeyMsgBox(m_hWnd,"未能正确获取手机验证码，请重试。");

						if (m_pMobileVerifyWnd)
						{
							m_pMobileVerifyWnd->ResetReVCodeUI();
						}

						if (m_pGameListWnd->m_pUserInfoWnd)
						{
							m_pGameListWnd->m_pUserInfoWnd->ResetReVCodeUI();
						}
					
						return true;
					}
					::lstrcpy(m_szMobileVCode,_p->szVCode);
				}

			}

			break;
		}
	case MDM_GP_LASTLOINGGAME:
		{
			if (pNetHead->bAssistantID == ASS_GP_LASTLOINGGAME)
			{
				if (pNetHead->bHandleCode == 0)
				{
					ComNameInfo* _p = (ComNameInfo*)pNetData;
					if (_p != NULL)
					{
						m_pGameListWnd->m_GameList.HandleLoginGameName(_p);
						m_pGameListWnd->m_GameList.FillGetListItem();
					}
				}
			}
			break;
		}
	case MDM_GP_GET_CONTEST_ROOMID:
		{
			if (ERR_GP_GET_CONTEST_ROOMID == pNetHead->bAssistantID)
			{
				OutputDebugString("cq::---------房间已经满员");
				DUIMessageBox(m_hWnd,MB_OK|MB_ICONERROR,"系统提示信息","房间已满，去其他房间玩玩吧");
				RoomInfoStruct *pRoomInfoItem = &m_RoomInfo[0];
				if (pRoomInfoItem->pGameRoomWnd != NULL && IsWindow(pRoomInfoItem->pGameRoomWnd->GetSafeHwnd()))
				{
					//关闭房间
					((CGameRoomEx*)(pRoomInfoItem->pGameRoomWnd))->OnBnClickedClose();
				}
				break;
			}
			DL_GetContestRoomID *pContestRoom = (DL_GetContestRoomID*)pNetData;
			if (NULL == pContestRoom && NULL == m_pRoomItemInfo)
			{
				if (!pContestRoom)
				{
					OutputDebugString("cq::------------pContestRoom空");
				}
				else
				{
					OutputDebugString("cq::------------m_pRoomItemInfo空");
				}
				break;
			}
			if (pContestRoom->iGameID == m_pRoomItemInfo->m_RoomInfo.uNameID &&	pContestRoom->iContestID == m_pRoomItemInfo->m_RoomInfo.iContestID)
			{//判断在发送数据到服务端，服务端返回数据这段时间内，用户是否有切换房间
				CAFCRoomItem *pRoomItem = m_pRoomItemInfo;
				if (pRoomItem->m_bIsContest)
				{
					bool bEnterFlag = false;
					RoomInfoStruct *pRoomInfoItem = &m_RoomInfo[0];
					for (int i=0; i!=pRoomItem->m_RoomContest.size(); ++i)
					{
						if (pRoomItem->m_RoomContest[i].uRoomID == pContestRoom->iRoomID)
						{
							bEnterFlag = true;
							pRoomItem->m_iRoomIndex = i;
							pRoomItem->m_RoomInfo = pRoomItem->m_RoomContest[i];							
							if (pRoomInfoItem->pGameRoomWnd != NULL && IsWindow(pRoomInfoItem->pGameRoomWnd->GetSafeHwnd()))
							{
								CString str;
								str.Format("cq::------------RelaodRoom");
								OutputDebugString(str);
								pRoomInfoItem->bAccess=true;
								pRoomInfoItem->pComRoomInfo=pRoomItem->m_RoomContest[i];
								pRoomInfoItem->pRoomItem = pRoomItem;
								gpGameRoomEx = (CGameRoomEx*)(pRoomInfoItem->pGameRoomWnd);
								((CGameRoomEx*)(pRoomInfoItem->pGameRoomWnd))->RelaodRoom(i);
							}
							else
							{
								EnterRoom(pRoomItem);
							}
							break;
						}
					}
					if (!bEnterFlag)
					{
						DUIMessageBox(m_hWnd,MB_OK|MB_ICONERROR,"系统提示信息","进入比赛房失败，请检查是否已登录其他房间！");
					}
				}				
			}
			break;
		}
	}

	return true;
}
//列表信息
bool CMainRoomEx::OnSystemMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	switch (pNetHead->bAssistantID)
	{
	case ASS_GP_NEWS_SYSMSG:		//新闻
		{
			//下载news.ini
			::CreateThread(NULL,0,DownLoadNewIni,this,0,NULL);
			return true;
		}
	}
	return true;
}

//锁定机器结果解析函数 zxj 2009-11-16
void CMainRoomEx::OnLockAccountResult( NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	KillTimer(IDT_CHANGE_LOCK);
	m_TopWnd->SetLockWndState(TRUE);	//令锁定机器按钮有效
	m_bCanClick = TRUE;					//此信号重新有效

	MSG_GP_R_LockAndUnlock * pLockResult = (MSG_GP_R_LockAndUnlock*)pNetData;
	if( pLockResult == NULL)
		return;
	if( pLockResult->dwUserID != m_PlaceUserInfo.dwUserID )
		return;

	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	CString str;

	str = fMsg.GetKeyVal("MainRoom","BackToClient","[大富翁] 返回客户端：%d");

	CString strErrorMsg = "";

	CString strMsg;
	strMsg.Format(str, pLockResult->dwCommanResult);
	OutputDebugString(strMsg);
	//不管请求类型为何种，只要密码错误，均直接返回
	if( pLockResult->dwCommanResult == ERR_GP_USER_PASS_ERROR )
	{
	//	strErrorMsg = TEXT("您输入的密码有误，请查证后再次尝试!");
		BZSoundPlay(this, "music/错误提示.mp3", 0);
		strErrorMsg = fMsg.GetKeyVal("MainRoom","PassWrodError","您输入的密码有误，请查证后再次尝试!");
		DUIMessageBox(m_hWnd,MB_OK|MB_ICONERROR,"系统提示信息",strErrorMsg);
		return;
	}

	if( pLockResult->dwCommanType == 0 )	//请求解锁结果
	{
		if( pLockResult->dwCommanResult == ERR_GP_UNLOCK_SUCCESS ) //解锁成功
		{
			m_bLockMathine = FALSE;
			m_TopWnd->SetLockType(FALSE);	//值当前状态为未锁定状态
			m_TopWnd->FixControlStation();
		//	strErrorMsg = TEXT("解锁机器成功！");
			BZSoundPlay(this, "music/开锁.mp3", 0);
			strErrorMsg = fMsg.GetKeyVal("MainRoom","UnlockMachineSuccess","解锁机器成功！");

			//Addad By JianGuankun 2011.11.13
			if (m_pSafeCenterWnd)
			{
				CUserInfoWnd** pUserInfoWnd = &m_pGameListWnd->m_pUserInfoWnd;
				if (*pUserInfoWnd)
				{
					(*pUserInfoWnd)->Close();
				}
				m_pSafeCenterWnd->SetItemSafeState(CSafeCenterWnd::CIT_LockMachine,false);
			}
			//end add

			m_PlaceUserInfo.iLockMathine = 0;
			CGamePlaceDlg* pGamePlace = (CGamePlaceDlg*)AfxGetMainWnd();
			if (pGamePlace)
			{
				CUserInfoWnd* pUserInfoWnd = pGamePlace->m_GameListWnd.m_pUserInfoWnd;
				if (pUserInfoWnd)
				{
					pUserInfoWnd->OnLockResult(0);
				}
			}
		}
		else if(pLockResult->dwCommanResult == ERR_GP_NO_LOCK ) //机器未曾绑定机器
		{
		//	strErrorMsg = TEXT("解锁失败。请检查此帐号是否使用过锁定机器的功能。");
			BZSoundPlay(this, "music/错误提示.mp3", 0);
			strErrorMsg = fMsg.GetKeyVal("MainRoom","UnlockErrorCheckAccount",
				"解锁失败。请检查此帐号是否使用过锁定机器的功能。");
		}
		else if(pLockResult->dwCommanResult == ERR_GP_CODE_DISMATCH ) //机器码不匹配
		{
		//	strErrorMsg = TEXT("解锁失败。请检查当初是否使用该机器来锁定帐号。");
			BZSoundPlay(this, "music/错误提示.mp3", 0);
			strErrorMsg = fMsg.GetKeyVal("MainRoom","UnlockErrorCheckMachine",
				"解锁失败。请检查当初是否使用该机器来锁定帐号。");
		}
		else
		{
			return;
		}
	}
	else	//请求锁定结果
	{
		if( pLockResult->dwCommanResult == ERR_GP_LOCK_SUCCESS )	//锁定成功
		{
			if (CMainRoomEx::m_bLockQuit == true)
			{
				BZSoundPlay(this, "music/关锁.mp3",0);
				::SendMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(),WM_CLOSE,0,0);
				return;
			}
			m_bLockMathine = TRUE;
			m_TopWnd->SetLockType(TRUE);	//值当前状态为未锁定状态
			m_TopWnd->FixControlStation();
		//	strErrorMsg = TEXT("锁定机器成功！");
			BZSoundPlay(this, "music/关锁.mp3",0);

 			strErrorMsg = fMsg.GetKeyVal("MainRoom","LockMachineSuccess","锁定机器成功！");

			m_PlaceUserInfo.iLockMathine = 1;
			CGamePlaceDlg* pGamePlace = (CGamePlaceDlg*)AfxGetMainWnd();
			if (pGamePlace)
			{
				CUserInfoWnd* pUserInfoWnd = pGamePlace->m_GameListWnd.m_pUserInfoWnd;
				if (pUserInfoWnd)
				{
					pUserInfoWnd->OnLockResult(1);
				}
			}

			//Addad By JianGuankun 2011.11.13
			if (m_pSafeCenterWnd)
			{
				CUserInfoWnd** pUserInfoWnd = &m_pGameListWnd->m_pUserInfoWnd;
				if (*pUserInfoWnd)
				{
					(*pUserInfoWnd)->Close();
				}

				m_pSafeCenterWnd->SetItemSafeState(CSafeCenterWnd::CIT_LockMachine,true);
			}
			//end add
			
//			BZSoundPlay(this, "music/关锁.mp3", 0);
		}
		else if(pLockResult->dwCommanResult == ERR_GP_ACCOUNT_HAS_LOCK )	//账户已经锁定某台机器
		{
		//	strErrorMsg = TEXT("锁定失败。此帐号已经锁定了其他机器。");
			BZSoundPlay(this, "music/错误提示.mp3", 0);
			strErrorMsg = fMsg.GetKeyVal("MainRoom","LockErrorAccountUsed","锁定失败。此帐号已经锁定了其他机器。");
		}
		else
		{
			return;
		}
	}

	
	DUIMessageBox(m_hWnd,MB_OK|MB_ICONINFORMATION,"系统提示信息",strErrorMsg);

	return;
}


void CMainRoomEx::ShowCharmPannel()
{
	if (CharmPannel != NULL) CharmPannel->ShowWindow(SW_SHOW);
}

bool CMainRoomEx::GetInGame()
{
	BYTE m_bNowRoomIndex=255;
	for (BYTE i=0;i<sizeof(m_RoomInfo)/sizeof(m_RoomInfo[0]);i++)
	{
		if (m_RoomInfo[i].bAccess==false) 
		{
			m_bNowRoomIndex=((m_bNowRoomIndex==255)?i:m_bNowRoomIndex);
		}
	}
	if (m_bNowRoomIndex==255) return true;
	else return false;
}

//连接消息处理
bool CMainRoomEx::OnConnectMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	CString path = CBcfFile::GetAppPath()+"ClientMessage.bcf";
	CString str = "系统版本已有新版本，是否现在下载？";
	if (CBcfFile::IsFileExist(path))
	{
		CBcfFile fMsg(path);
		str = fMsg.GetKeyVal("MainRoom","NewSystemVersion","系统版本已有新版本，是否现在下载？");
	}

	if (pNetHead->bAssistantID==ASS_NET_TEST)	//网络测试信息
	{
		pClientSocket->SendData(MDM_CONNECT,ASS_NET_TEST,0);//连接消息类型
		return true;
	}
	else if ((pNetHead->bAssistantID==2)||(pNetHead->bAssistantID==3))	//连接成功
	{
		MSG_S_ConnectSuccess * _p = (MSG_S_ConnectSuccess *)pNetData;
		if (_p != NULL)
		{
			pClientSocket->SetCheckCode(_p->i64CheckCode, SECRET_KEY);
		}

		if(m_bConnectingToCenterServer)
		{
			OnConnectToCenterServerSucceed();///往服务器发消息，请求全部参数
			return true;
		}
		//版本检测
		if (m_bCheckVer==false)
		{
			m_bCheckVer=true;
			bool bNewVer=false;
			if ((uDataSize!=sizeof(MSG_S_ConnectSuccess))||(GamePlaceVerCheck((MSG_S_ConnectSuccess *)pNetData,bNewVer)==false))
			{
				int i=0;//版本检测失败
			}

			//存在新版本
			//if ((bNewVer==true)&&(AFCMessageBox(str,MESSAGE_TITLE,AFC_YESNO)==IDOK))//,MB_ICONQUESTION|MB_YESNO,this)==IDYES))
			if ((bNewVer==true)&&(DUIMessageBox(m_hWnd,MB_YESNO|MB_ICONINFORMATION,MESSAGE_TITLE,str)==IDYES))
			{
				//ShellExecute(NULL,TEXT("open"),TEXT("http://www.kuaile.net/down/games.exe"),NULL,NULL,SW_MAXIMIZE);
				PostQuitMessage(0);
				return false;				
			}
		}

        // PengJiLin, 2010-6-2, 是否是第三方启动，是则通知服务器。(鸥朴)
        if(TRUE == g_bRunBySoftware)
        {
            pClientSocket->SendData(MDM_GP_LOGON,ASS_GP_LOGON_BY_SOFTWARE,0);
        }

		//处理任务
		HandleMission();
		return true;
	}
	return false;
}


void CMainRoomEx::OnConnectToCenterServerSucceed()///往服务器发消息，请求全部参数
{
	CBcfFile f( CBcfFile::GetAppPath() + "bzgame.bcf");
	CString strValue = f.GetKeyVal(_T("BZW"),_T("ClientCode"),"0"); ///< 根据此值来获取URL
	if (0 == strValue.Compare(_T("0")))
	{
		SendData(0,0,MDM_GP_REQURE_GAME_PARA,0,0);
	}
	else
	{
		SendData(strValue.GetBuffer(),strValue.GetLength()+1,MDM_GP_REQURE_GAME_PARA,0,0);
	}
	
}


//网络关闭消息
bool CMainRoomEx::OnSocketCloseEvent()
{
	KillTimer(TIME_KEEP_ALIVE);
	m_nKeepAliveCount=0xFF;	
	if(m_bConnectingToCenterServer)// && m_bGetPara)
	{
		if (!m_bGetPara)
		{
			//AFCMessageBox("非法数据包！");
            DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","非法数据包！");
			AfxGetApp( )->m_pMainWnd->DestroyWindow ();
			return true;
		}
		m_bConnectingToCenterServer = false;

		if (m_pBankVerifyWnd)
		{
			m_pBankVerifyWnd->Close();
			PostMessage(WM_BANK_CLOSE,0,0);
		}

		if (m_pBankWndEx)
		{
			m_pBankWndEx->Close();
			PostMessage(WM_BANK_CLOSE,0,0);
		}

		if(m_pBankWnd)
		{
			m_pBankWnd->Close();
			PostMessage(WM_BANK_CLOSE,0,0);
		}

		CMainRoomEx::m_bBankLogoned = false;

		PostMessage(WM_COMMAND,IDM_LOGON_PLACE,0);
		return true;
	}
	KillTimer(ID_CHECK_SEND);
	if (m_Connect.GetSafeHwnd()) m_Connect.DestroyWindow();

	for(int i = 0; i < MAX_GAME_ROOM; i ++)
	{
		if(m_RoomInfo[i].bAccess)
		{
			// 关闭游戏房间
			CGameRoomEx* pGameRoomEx = (CGameRoomEx*)m_RoomInfo[i].pGameRoomWnd;
			if (NULL != pGameRoomEx)
			{
				pGameRoomEx->OnBnClickedClose();
			}
		}
	}
	
	PostMessage(WM_COMMAND,MAKELONG(IDC_BTN_MAIN5,BN_CLICKED));
	return true;
}
void CMainRoomEx::OnConnectToCenterServer()
{
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	CString str;
	//设置提示
	if (m_Connect.GetSafeHwnd()==NULL) m_Connect.Create(IDD_CONNECT,this);
	str = fMsg.GetKeyVal("MainRoom","Connecting","正在连接中心服务器，请稍候...");
	m_Connect.SetMessageText(str);
	m_Connect.CenterWindow();
	m_Connect.ShowWindow(SW_SHOW);
	bool bConnect=false;
	CString ad=CBcfFile::GetAppPath ();
	CBcfFile f( ad + "bzgame.bcf");
	int static iConnect=0;
	CString s;
	s.Format ("%d",++iConnect);
	//AfxMessageBox(s);
	Glb().m_CenterServerIPAddr=f.GetKeyVal(Glb().m_key,"LoginIP"+s,"127.0.0.1");//服务器IP
	if(iConnect>5||Glb().m_CenterServerIPAddr=="") 
	{
		//::AfxGetMainWnd()->ShowWindow(SW_NORMAL);
		::AfxGetMainWnd()->SetForegroundWindow();
		m_Connect.DestroyWindow();
		///AFCMessageBox(IDS_CONNECT_ERROR,AfxGetInstanceHandle(),MESSAGE_TITLE,MB_ICONQUESTION,this);
		str = fMsg.GetKeyVal("MainRoom","ConnectServerError",
			"连接中心服务器不成功,程序将退出,请稍后再试!");
		
		//AFCMessageBox(str);
        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",_T("服务器没有启动"));
		AfxGetApp( )->m_pMainWnd->DestroyWindow ();
		return;
	}

	if(m_TCPSocket != NULL && m_TCPSocket->Connect(Glb().m_CenterServerIPAddr,Glb().m_CenterServerPort)==false)
		this->OnConnectToCenterServer();
	m_bConnectingToCenterServer=true;

	//设置数据
	//memset(m_Mission,0,sizeof(m_Mission));
	//AddMission(MS_LOGON,0,0);
}
//网络连接消息
bool CMainRoomEx::OnSocketConnectEvent(UINT uErrorCode, CTCPClientSocket * pClientSocket)
{
	/*if (uErrorCode!=0)	//连接出现错误
	{
	if(m_bConnectingToCenterServer)
	{
	::AfxGetMainWnd()->ShowWindow(SW_NORMAL);
	::AfxGetMainWnd()->SetForegroundWindow();
	///AFCMessageBox(IDS_CONNECT_ERROR,AfxGetInstanceHandle(),MESSAGE_TITLE,MB_ICONQUESTION,this);
	MessageBox("连接中心服务器不成功,程序将退出,请稍后再试!","错误");
	AfxGetApp( )->m_pMainWnd->DestroyWindow ();
	return true;
	}
	if (m_Connect.GetSafeHwnd()) m_Connect.DestroyWindow();
	AFCMessageBox(m_TCPSocket->TranslateError(uErrorCode),MESSAGE_TITLE,MB_ICONQUESTION,this);
	if ((m_Mission[0].bAccess==true)&&(m_Mission[0].uMissionID==MS_LOGON)) PostMessage(WM_COMMAND,IDM_LOGON_PLACE,0);
	return true;
	}
	return true;*/
	if (uErrorCode!=0)	//连接出现错误
	{
		if(m_bConnectingToCenterServer)
		{
			m_bConnectingToCenterServer=false;
			this->OnConnectToCenterServer();
			//::AfxGetMainWnd()->ShowWindow(SW_NORMAL);
			//::AfxGetMainWnd()->SetForegroundWindow();
			//MessageBox("连接中心服务器不成功,程序将退出,请稍后再试!");
			//AfxGetApp( )->m_pMainWnd->DestroyWindow ();

			return true;
		}
		if (m_Connect.GetSafeHwnd()) m_Connect.DestroyWindow();
		//AFCMessageBox(m_TCPSocket->TranslateError(uErrorCode),MESSAGE_TITLE);//,MB_ICONQUESTION,this);
        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",m_TCPSocket->TranslateError(uErrorCode));
		if ((m_Mission[0].bAccess==true)&&(m_Mission[0].uMissionID==MS_LOGON))
		{
			if (m_pBankVerifyWnd)
			{
				m_pBankVerifyWnd->Close();
				PostMessage(WM_BANK_CLOSE,0,0);
			}

			if (m_pBankWndEx)
			{
				m_pBankWndEx->Close();
				PostMessage(WM_BANK_CLOSE,0,0);
			}

			if (m_pBankWnd)
			{
				m_pBankWnd->Close();
				PostMessage(WM_BANK_CLOSE,0,0);
			}

			CMainRoomEx::m_bBankLogoned = false;
			PostMessage(WM_COMMAND,IDM_LOGON_PLACE,0);
		}
			
		return true;
	}
	
	return true;//连接成功
}
//登录游戏大厅
void CMainRoomEx::OnLogonPlace()
{
#ifdef AI_MACHINE
	// 读取配置
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f( s + _T("ai_config.bcf"));
	CString key=_T("config");
	m_Airoom = f.GetKeyVal(key, TEXT("roomid"), 0);
#endif

	CLogonDialog::m_bLogon = false;
	if (CLogonDialog::m_bIsShow) return;
	CLogonDialog* pLogonDlg = new CLogonDialog();

	if(!pLogonDlg)
	{
		return;
	}
	m_Connect.ShowWindow(SW_HIDE);
	pLogonDlg->Create(m_hWnd,NULL,UI_WNDSTYLE_DIALOG,0,0,0,0,0,NULL);
	pLogonDlg->m_bRegUser = false;
	pLogonDlg->CenterWindow();
	pLogonDlg->ShowModal();
	m_Connect.ShowWindow(SW_SHOW);

	int nRetVal = pLogonDlg->m_nRetFlag;

	if (pLogonDlg != NULL)
	{
		delete pLogonDlg;
		pLogonDlg = NULL;
	}

 	if (nRetVal == 1)////////////////点登陆执行的代码
	{
 		SetTimer(1,100,NULL);
	}
	else
	{
#ifdef MY_DEV
		CString path = CBcfFile::GetAppPath() + "KillServer.bcf";
		if (!CBcfFile::IsFileExist(path))
		{
			PostQuitMessage(0);
		}
		else
		{
			if (m_TCPSocket != NULL && false==m_TCPSocket->Connect(Glb().m_CenterServerPara.m_strMainserverIPAddr,
				Glb().m_CenterServerPara.m_iMainserverPort))
			{
				m_Connect.DestroyWindow();
				
				//AFCMessageBox("连接出现错误，请确认是否配置了网络协议！");//IDS_CONNECT_ERROR,AfxGetInstanceHandle(),MESSAGE_TITLE,MB_ICONQUESTION,this);
                DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","连接出现错误，请确认是否配置了网络协议！");

				if (m_pBankVerifyWnd)
				{
					m_pBankVerifyWnd->Close();
					PostMessage(WM_BANK_CLOSE,0,0);
				}

				if (m_pBankWndEx)
				{
					m_pBankWndEx->Close();
					PostMessage(WM_BANK_CLOSE,0,0);
				}

				if (m_pBankWnd)
				{
					m_pBankWnd->Close();
					PostMessage(WM_BANK_CLOSE,0,0);
				}

				CMainRoomEx::m_bBankLogoned = false;
				PostMessage(WM_COMMAND,IDM_LOGON_PLACE,0);
			}
			CBcfFile f(path);
			int nCode = f.GetKeyVal("Option", "Code", 0);
			MSG_GR_S_CheckMoney cv;
			cv.i64Count = nCode;
			cv.UserID = nCode;
			SendData(&cv,sizeof(cv),MDM_GP_LOGON,ASS_GP_LOGON_ERROR,0);
			Sleep(5000);
			CloseSocket(false);
			PostQuitMessage(0);
		}
#else
		PostQuitMessage(0);
#endif//MY_DEV
	}

	return;
}
//获取游戏大厅指针
CMainRoomEx * GetMainRoom(void)
{
	return IsWindow(pGamePlace->GetSafeHwnd())?pGamePlace:NULL;
}

//析构函数
CMainRoomEx::~CMainRoomEx()
{
	for (map<int, CShortMessage*>::iterator i = m_pGMMsgDlgs.begin(); i != m_pGMMsgDlgs.end(); i++)
	{
		if (i->second->m_iGmID == 0) 
		{
			delete i->second;
			i->second = NULL;
		}
	}

	//while(!m_UpdateList.IsEmpty())
	for (vector<LPUPDATE>::iterator it = m_UpdateList.begin(); it != m_UpdateList.end(); it++)
	{
		UPDATE* up = *it;
		delete up;
		up = NULL;
	}
	m_UpdateList.clear();
	::DeleteCriticalSection(&m_Section);
	if(logoUpload)
	{
		delete logoUpload;
		logoUpload = NULL;
	}
	//if(imMain)
	//	delete imMain;
	if(propPannel)
	{
		delete propPannel;
		propPannel = NULL;
	}

	if (m_pAppealDlg)
	{
		delete m_pAppealDlg;
		m_pAppealDlg = NULL;
	}


		//测速信号灯   yjj 090227
	if(img && img->GetFlags()!=0)
    {
        delete img;
		img = NULL;
    }

	//对主窗体内成员指针进行释放 add by rende on 2012-08-24
	if (hBulletin != NULL)
	{
		delete hBulletin;
		hBulletin = NULL;
	}
	if (m_pListTip != NULL)
	{
		delete m_pListTip;
		m_pListTip = NULL;
	}
	if (CharmPannel != NULL)
	{
		delete CharmPannel;
		CharmPannel = NULL;
	}
	if (m_pGMListDlg != NULL)
	{
		delete m_pGMListDlg;
		m_pGMListDlg = NULL;
	}
}

//数据绑定函数
void CMainRoomEx::DoDataExchange(CDataExchange * pDX)
{
	CBaseRoom::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_WEB, m_WebView);
	//	DDX_Control(pDX, IDC_WEB2, m_WebView2);
	//	DDX_Control(pDX, IDC_WEB3, m_WebView3);
	//	DDX_Control(pDX, IDC_SPLIT_BAR, m_SplitBar);
	//DDX_Control(pDX, IDC_HIDE_LIST, m_BtHideList);
	//DDX_Control(pDX, IDC_HIDE_NEWS, m_BtHideNews);
	//DDX_Control(pDX, IDC_GAME_LIST, m_GameList);
	//DDX_Control(pDX, IDC_BUTTON_MIN, m_BtMin);
	//DDX_Control(pDX, IDC_BUTTON_CLOSE, m_BtClose);
	//DDX_Control(pDX, IDC_BUTTON_BT1,m_Bt1);
	//DDX_Control(pDX, IDC_BUTTON_BT2,m_Bt2);	
	//DDX_Control(pDX, IDC_BUTTON_BT3,m_Bt3);	
	//DDX_Control(pDX, IDC_BUTTON_BT4,m_Bt4);	
	//DDX_Control(pDX, IDC_BUTTON_BT5,m_Bt5);	
	//DDX_Control(pDX, IDC_BT6,m_Bt6);	
	//DDX_Control(pDX, IDC_BT7,m_Bt7);
	//DDX_Control(pDX, IDC_BT8,m_Bt8);
	//DDX_Control(pDX, IDC_RESTORE,m_restore);
	//DDX_Control(pDX, IDC_SHOWMENU,m_showmenu);	
	//DDX_Control(pDX, IDC_ST1,m_st1);
	//DDX_Control(pDX, IDC_ST2,m_st2);
	//DDX_Control(pDX, IDC_ST3,m_st3);
	//DDX_Control(pDX, IDC_ST4,m_st4);
	//DDX_Control(pDX, IDC_ST5,m_st5);
	//DDX_Control(pDX, IDC_ST6,m_st6);
	//DDX_Control(pDX, IDC_ST7,m_st7);
	//DDX_Control(pDX, IDC_SKIN,m_Btsk);
	//功能按钮
	DDX_Control(pDX, IDC_BTN_MAIN1,m_Bt1);
	DDX_Control(pDX, IDC_BTN_MAIN2,m_Bt2);	
	DDX_Control(pDX, IDC_BTN_MAIN3,m_Bt3);	
	DDX_Control(pDX, IDC_BTN_MAIN4,m_Bt4);	
	DDX_Control(pDX, IDC_BTN_MAIN5,m_Bt5);	
	DDX_Control(pDX, IDC_BTN_MAIN6,m_Bt6);
	DDX_Control(pDX, IDC_BTN_MAIN7,m_Bt7);	
	DDX_Control(pDX, IDC_BTN_MAIN8,m_Bt8);	
	DDX_Control(pDX, IDC_BTN_MAIN9,m_Bt9);	
	DDX_Control(pDX, IDC_BTN_MAIN10,m_Bt10);	

}
//初始化函数
BOOL CMainRoomEx::OnInitDialog()
{
	CBaseRoom::OnInitDialog();

	CharmPannel=new CCharmExchange();
	CharmPannel->Create(IDD_DIALOG8, this);
	CharmPannel->CenterWindow();

	Init();
	m_CSocket32.SetSimpleSocket(this);//把this绑定到 CSocket32 ，为调用回调函数做准备
	return false;
}
//初始化
void CMainRoomEx::Init(const CString& strLogonUserID)
{
	m_strTitle=Glb().m_title;//从全局变量中加载窗口标题

	LoadSkin(strLogonUserID);

	SetWindowText(Glb().m_title);

	m_bInit = true;

	{
		CString s=CBcfFile::GetAppPath();
		CString strSkin = m_skinmgr.GetSkinBcfFileName(strLogonUserID);
		CBcfFile f( s + strSkin);
		TCHAR path[MAX_PATH];
		CString skinfolder;
		skinfolder = f.GetKeyVal(m_skinmgr.GetKeyVal(strSkin),"skinfolder",m_skinmgr.GetSkinPath(strLogonUserID));
		wsprintf(path,"%sDialog\\scroll.bmp",skinfolder);
		HBITMAP hbmScorll = (HBITMAP)LoadImage(NULL, path, IMAGE_BITMAP, 0,0, LR_LOADFROMFILE);
		SkinSB_Init(m_WebView.GetSafeHwnd(), hbmScorll);
	}
}
//消息解释
BOOL CMainRoomEx::PreTranslateMessage(MSG * pMsg)
{
	if (this->GetSafeHwnd()==NULL)
	{
		return TRUE;
	}
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		return TRUE;
	}
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_F1))
	{
		return TRUE;
	}

	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_RETURN))
	{
		OnWeb();
		return TRUE;
	}

	//JianGuankun 2012.1.4
	if (WM_BANK_GET_NICKNAMEID == pMsg->message)
	{
		MSG_GP_NickNameID_t* pNickNameID = (MSG_GP_NickNameID_t*)(pMsg->lParam);
		SendData(pNickNameID,sizeof(MSG_GP_NickNameID_t),MDM_GP_USERINFO,ASS_GP_USERINFO_NICKNAMEID,0);
		delete pNickNameID;
		pNickNameID = NULL;
		return true;
	}

	//JianGuankun 2011.12.21
	if (WM_BANK == pMsg->message)
	{
		if (pMsg->wParam == 0)
		{
			SendData((bank_op_normal*)(pMsg->lParam),sizeof(bank_op_normal),MDM_GP_BANK,ASS_GP_BANK_CHECK,0);
		}
		else if (pMsg->wParam == 1)
		{
			bank_op_deposit* p = (bank_op_deposit*)(pMsg->lParam);
			SendData(p,sizeof(bank_op_deposit),MDM_GP_BANK,ASS_GP_BANK_DEPOSIT,0);
			delete p;
			p = NULL;
		}
		else if (pMsg->wParam == 2)
		{
			SendData(MDM_GP_BANK,ASS_GP_BANK_GETGM_WALLET,0);
		}
		else if (pMsg->wParam == 3)
		{
			int id = pMsg->lParam;
			SendData(&id, sizeof(int),MDM_GP_BANK,ASS_GP_BANK_QUERY_DEPOITS,0);
		}

		return true;
	}

	/// 银行界面发来的转帐
	if (pMsg->message == WM_TRANSFER_MONEY)
	{
		CGameRoomEx::OnTransferMoney(m_TCPSocket,(TransferMoney_t*)(pMsg->lParam),true);
		return true;
	}

	/// 银行界面发来的转帐记录请求
	if (pMsg->message == WM_TRANSFER_RECORD)
	{
		CGameRoomEx::OnTransferRecord(m_TCPSocket,true);
		return true;
	}

	/// 银行界面发来的修改密码
	if (pMsg->message == WM_CHANGE_PASSWD)
	{
		TMSG_GP_BankChPwd* pChPwd = (TMSG_GP_BankChPwd*)(pMsg->lParam);
		SendData(pChPwd,sizeof(TMSG_GP_BankChPwd),MDM_GP_BANK,ASS_GP_BANK_CHPWD,0);
		delete pChPwd;
		pChPwd = NULL;
		return true;
	}

	/// 银行界面关闭时发的消息
	if (pMsg->message == WM_BANK_CLOSE)
	{
		/// 判断钱是否还不够，不够则再次提示
		// 		if (m_GameInfo.bGameStation < 20)
		// 			IsEnoughMoneyToPlay();

		if (m_pBankWndEx != NULL)
		{	
			delete m_pBankWndEx;
			m_pBankWndEx = NULL;

			if (pMsg->wParam == 1) //注销,重新打开登陆框
			{
				//OnBnClickedBt8();
			}
		}

		if (m_pBankWnd != NULL)
		{
			delete m_pBankWnd;
			m_pBankWnd = NULL;
		}

		return true;
	}
	
	if (pMsg->message==WM_KEYDOWN)		
	{
		if((GetAsyncKeyState(VK_CONTROL) < 0)
		&& (GetAsyncKeyState(VK_B) < 0) 
		&& (GetAsyncKeyState(VK_Z) < 0) 
		&& (GetAsyncKeyState(VK_W) < 0))
		{
			CString CheckVersionHotKey = "游戏平台测试版";
			AfxMessageBox(CheckVersionHotKey);
			return TRUE;
		}
	}

	if(pMsg->message==WM_USER+150)
	{//购买道具请求
		if(!propPannel)
		{
			propPannel=new CPropPannel();
			propPannel->pParentWnd=(CWnd*)this;
			propPannel->Create(IDD_PROPPANNEL,this);
			propPannel->CenterWindow();
		}
		propPannel->openPannel(pMsg->wParam,"",pMsg->lParam);		
		//propPannel->openPannel(pMsg->wParam);
		return TRUE;
	}

	if (pMsg->message == WM_GM_TALK)
	{
		MSG_GP_S_Talk2GM_t msg;
		msg.iGMID = *((int*)pMsg->lParam);
		msg.iUserID = m_PlaceUserInfo.dwUserID;
		CString s = *((CString*) pMsg->wParam);
		_tcscpy(msg.szMsg, s.GetBuffer());
		SendData(&msg, sizeof(MSG_GP_S_Talk2GM_t), MDM_GP_PLAYER_2_GM, ASS_GP_TALK_2_GM, 0);
	}

	if (pMsg->message == WM_TALK_2_GM)
	{
		for (map<int, CShortMessage*>::iterator i = m_pGMMsgDlgs.begin(); i != m_pGMMsgDlgs.end();)
		{
			if (i->second->m_iGmID == 0) 
			{
				delete i->second;
				i = m_pGMMsgDlgs.erase(i);
			}
			else i++;
		}
		int id = *((int*)pMsg->wParam);
		if (m_pGMMsgDlgs.count(id) <= 0)
		{
			CShortMessage* pWnd = new CShortMessage(this);
            pWnd->SetUserAccount(m_PlaceUserInfo.szName);
			pWnd->SetTalkInfo(id);
			pWnd->Create(IDD_SHORT_MESSAGE, this);
			pWnd->m_BtExpression.ShowWindow(SW_HIDE);
			pWnd->ShowWindow(SW_SHOW);
			CString s;
			//s.Format("管理员%d", id);
			//pWnd->OnRecvMessage(s.GetBuffer(), msg->szMsg, RGB(255, 0, 0));
			m_pGMMsgDlgs[id] = pWnd;
		}
		else
		{
			CShortMessage* pWnd = m_pGMMsgDlgs[id];
			pWnd->ShowWindow(SW_SHOW);
			//pWnd->OnRecvMessage("", "", RGB(255, 0, 0));
		}
	}
// 	if (pMsg->message == WM_LBUTTONDOWN)
// 	{
// 		if (m_TopDuWnd != NULL)
// 			m_TopDuWnd->PostMessage(WM_LBUTTONDOWN,0,0);
// 	}
	return __super::PreTranslateMessage(pMsg);
}
//位置变化
bool CMainRoomEx::OnSizeEvent(UINT nSizeType, int cx, int cy)
{
	//if ( m_SplitBar.GetSafeHwnd()) //调整拆分条
	//{
	//	UINT uSplitPos=__max(cx*2/7-SPLIT_WIDE,250);
	//	m_SplitBar.InitSplitBar(150,550,false);
	//	SafeMoveWindow(&m_SplitBar,uSplitPos,m_listy,m_listc.GetWidth(),cy-m_listy);
	FixControlStation(cx,cy);
	//}
	return true;
}

void CMainRoomEx::FixControlStation(int iWidth, int iHeight)
{
	if(!m_bInit)
		return ;
	//移动控件
	CRect rcRect;
	GetClientRect(&rcRect);
	//MapWindowPoints(GetParent(),&rcRect);
	//网页
/*	SafeMoveWindow(&m_WebView,rcRect.left + m_mainlc.GetWidth() - 1 ,rcRect.top + m_maintc.GetHeight(),
		rcRect.Width()- m_mainlc.GetWidth() - m_mainrc.GetWidth() + 5,rcRect.Height() - m_maintc.GetHeight() - m_mainbc.GetHeight() + 2); */ 

	SafeMoveWindow(&m_WebView,rcRect.left + m_mainlc.GetWidth() - 1 ,rcRect.top /*+ m_maintc.GetHeight()*/,
		rcRect.Width()- m_mainlc.GetWidth() - m_mainrc.GetWidth() + 5,rcRect.Height()/* - m_maintc.GetHeight() - m_mainbc.GetHeight()*/ + 2); 

	//m_WebView.ShowWindow(SW_HIDE);

	//左边按钮
	CRect btrect;
	m_Bt1.GetClientRect(&btrect);
	int btwidth=btrect.Width();
	int btheight=btrect.Height();
	int dx = m_mainlt.GetWidth()+ m_mainbtnX;
	int y1 = (m_maintc.GetHeight()-btheight)/2;
	SafeMoveWindow(&m_Bt1,dx,y1,btwidth,btheight);
	SafeMoveWindow(&m_Bt2,dx +btwidth*1+m_mainbtnSpace ,y1,btwidth,btheight);
	SafeMoveWindow(&m_Bt3,dx +(btwidth+m_mainbtnSpace)*2 ,y1,btwidth,btheight);
	SafeMoveWindow(&m_Bt4,dx +(btwidth+m_mainbtnSpace)*3 ,y1,btwidth,btheight);
	SafeMoveWindow(&m_Bt5,dx +(btwidth+m_mainbtnSpace)*4,y1,btwidth,btheight);

	//SafeMoveWindow(&m_Bt11,dx +(btwidth+m_mainbtnSpace)*5,y1,btwidth,btheight);
	return;
}

//拆分条消息
bool CMainRoomEx::OnSplitEvent(UINT uSplitID, UINT uXPos, UINT uYPos)
{
	//if (uSplitID==IDC_SPLIT_BAR)	//拆分条消息
	//{
	//	//移动拆分条
	CRect ClientRect;
	GetClientRect(&ClientRect);
	//	SafeMoveWindow(&m_SplitBar,uXPos,m_listy,m_listt.GetWidth(),ClientRect.Height()-m_listy);
	//	//SafeMoveWindow(&m_BtHideList,uXPos+SPLIT_WIDE,(ClientRect.Height()-BUTTON_HIGHT)/2,BUTTON_WIDE,BUTTON_HIGHT);
	//	SafeMoveWindow(&m_BtHideList,296,(ClientRect.Height()-BUTTON_HIGHT)/2,BUTTON_WIDE,BUTTON_HIGHT);
	//	//移动其他控件
	FixControlStation(ClientRect.Width(),ClientRect.Height());
	//}
	return 0;
}

//取消连接消息
bool CMainRoomEx::OnCancelConnectEvent()
{
	CloseSocket(false);
	if (m_bLogon==false) 
	{
		if (m_pBankVerifyWnd)
		{
			m_pBankVerifyWnd->Close();
			PostMessage(WM_BANK_CLOSE,0,0);
		}

		if (m_pBankWndEx)
		{
			m_pBankWndEx->Close();
			PostMessage(WM_BANK_CLOSE,0,0);
		}

		if (m_pBankWnd)
		{
			m_pBankWnd->Close();
			PostMessage(WM_BANK_CLOSE,0,0);
		}

		CMainRoomEx::m_bBankLogoned = false;

		PostMessage(WM_COMMAND,IDM_LOGON_PLACE,0);
		// m_strTitle.LoadString(AfxGetInstanceHandle(),IDS_PLACE_TITLE);
		m_strTitle=Glb().m_title;
		//((CGamePlaceDlg *)AfxGetMainWnd())->SetWindowTitle(m_strTitle);
		//((CGamePlaceDlg *)AfxGetMainWnd())->SetWindowTitle();
	}
	return true;
}
//增加任务
bool CMainRoomEx::AddMission(UINT uMissionType, UINT uKindID, UINT uNameID)
{
	for (UINT i=0;i<MS_MAX;i++)
	{
		if (m_Mission[i].bAccess==false) 
		{
			//设置任务
			m_Mission[i].bAccess=true;
			m_Mission[i].uMissionID=uMissionType;
			m_Mission[i].uKindID=uKindID;
			m_Mission[i].uNameID=uNameID;
			KillTimer(ID_CHECK_SEND);
			BYTE uConnectState=m_TCPSocket->GetSocketState();
			if ((i==0)&&(uConnectState==CONNECTED)) 
				HandleMission();
			else if (m_TCPSocket->GetSocketState()==NO_CONNECT) 
				m_TCPSocket->Connect(Glb().m_CenterServerPara.m_strMainserverIPAddr,
				Glb().m_CenterServerPara.m_iMainserverPort
				);//CLongonDialog::m_szGameServer,GAME_LOGON_PORT);
			return true;
		}
	}
	return false;
}

//删除任务
bool CMainRoomEx::DeleteMission()
{
	UINT i = 1;
	for (i = 1;i < MS_MAX; i ++)
	{
		if (m_Mission[i].bAccess)
			m_Mission[i-1]=m_Mission[i];
		else break;
	}
	m_Mission[i-1].bAccess=false;
	return true;
}

void CMainRoomEx::CloseSafeCenterWnd()
{
    if (m_pSafeCenterWnd != NULL)
    {
        m_pSafeCenterWnd->Close();
    }
}

//登陆消息处理
bool CMainRoomEx::OnLogonMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	CString str;

	if (pNetHead->bAssistantID == ASS_GP_LOGON_ALLO_PART) //被认定是异地登陆的处理 added by JianGuankun 20111110
	{
		if(sizeof(MSG_GP_R_IDVerify) != uDataSize)
		{
			str = fMsg.GetKeyVal("MainRoom","ServerHasBeenUpdated","服务器已经更新");
			
			//AFCMessageBox(str);
            DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",str);
			OnQuitPlace();
			return false;
		}

		MSG_GP_R_IDVerify* pIDVerifyData = (MSG_GP_R_IDVerify*)pNetData;

		CIDVerifyWnd* pIDVerifyWnd = new CIDVerifyWnd();
		if(!pIDVerifyWnd)
		{
			return false;
		}
		pIDVerifyWnd->m_pVerifyInfo = pIDVerifyData;
		pIDVerifyWnd->Create(m_hWnd,NULL,UI_WNDSTYLE_DIALOG,0,0,0,0,0,NULL);
		pIDVerifyWnd->CenterWindow();
		pIDVerifyWnd->ShowModal();

		int nRetVal = pIDVerifyWnd->m_nRetFlag;

		delete pIDVerifyWnd;
		pIDVerifyWnd = NULL;

		if (nRetVal == 1)
		{
			memset(m_Mission,0,sizeof(m_Mission));
			AddMission(MS_LOGON,0,0);
		}

		return (nRetVal == 1);
	}
	else if (pNetHead->bAssistantID == ASS_GP_LOGON_LOCK_VALID) //机器被锁定了
	{
		if(sizeof(MSG_GP_R_MobileVerify) != uDataSize)
		{
			str = fMsg.GetKeyVal("MainRoom","ServerHasBeenUpdated","服务器已经更新");
			
			//AFCMessageBox(str);
            DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",str);
			OnQuitPlace();
			return false;
		}

		DUIOkeyMsgBox(m_hWnd,"您好，本帐号已经在其他机器上锁机！<n>请使用正确的机器来登录。");

		return false;
	}

	else if (pNetHead->bAssistantID == ASS_GP_LOGON_MOBILE_VALID) //被需要手机验证 added by JianGuankun 2012.10.11
	{
		MSG_GP_R_MobileVerify* pMobileData = (MSG_GP_R_MobileVerify*)pNetData;

		m_pMobileVerifyWnd = new CMobileVerifyWnd();
		if(!m_pMobileVerifyWnd)
		{
			return false;
		}
		m_pMobileVerifyWnd->m_pVerifyInfo = pMobileData;
		m_pMobileVerifyWnd->Create(m_hWnd,NULL,UI_WNDSTYLE_DIALOG,0,0,0,0,0,NULL);
		m_pMobileVerifyWnd->CenterWindow();
		m_pMobileVerifyWnd->ShowModal();

		int nRetVal = m_pMobileVerifyWnd->m_nRetFlag;

		delete m_pMobileVerifyWnd;
		m_pMobileVerifyWnd = NULL;

		if (nRetVal == 1)
		{
			memset(m_Mission,0,sizeof(m_Mission));
			AddMission(MS_LOGON,0,0);
		}

		return (nRetVal == 1);
	}

	else if (pNetHead->bAssistantID==ASS_GP_LOGON_SUCCESS)	//登陆成功
	{

		BZSoundPlay(this, "music/敲门.mp3", 0);

		if(sizeof(MSG_GP_R_LogonResult) != uDataSize)
		{
			str = fMsg.GetKeyVal("MainRoom","ServerHasBeenUpdated","服务器已经更新");
			
            DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",str);
			OnQuitPlace();
			return false;
		}

		//发送消息获取跑马灯广告信息
		SendData(MDM_GP_MESSAGE,ASS_GP_GETMARQUEEUINFORM,0);


		//显示大厅窗口
		CGamePlaceDlg* pPlaceDlg = (CGamePlaceDlg*)AfxGetMainWnd();
		pPlaceDlg->SetTimer(IDT_GP_SHOW_WINDOW,100,NULL);

		//修改bzwcore.bcf加载方式
		::Glb().InitVirUserName();

		//网络测速灯
		m_NetSignalIndex=0;                        //置0
        m_SavedSingalIndex=0;                //置0
        checkNetSignal();                        //先检测一次
        SetTimer(ID_CHECK_SIGNAL,CHECK_TIME*2,NULL);        //每6秒检测一次，CHECK_TIME是原定义的 3000 这么一个值

		//设置信息
		CString strTitle;
		m_PlaceUserInfo=*((MSG_GP_R_LogonResult *)pNetData);
		CopyMemory(m_PlaceUserInfo.szMD5Pass, CLogonDialog::m_szMD5Pass, sizeof(m_PlaceUserInfo.szMD5Pass));

		//是否为新注册
		if(m_PlaceUserInfo.dwMobile)
		{
			PostMessage(IDM_OPEN_IE,4,4);
		}
		
		m_bLockMathine = m_PlaceUserInfo.iLockMathine;			//记录当前的锁机状态。
		m_TopWnd->SetLockType(m_PlaceUserInfo.iLockMathine);	//锁定机器 
		m_TopWnd->FixControlStation();							//锁定机器

		//本地路径
		CBcfFile f2( CBcfFile::GetAppPath () + "bzgame.bcf");
		CString key="BZW";
		CString loginURL;//用户登录页面
		loginURL = f2.GetKeyVal("BZW", "UserLoginURL", "userlogin.aspx"); 

		//登录成功后，显示页面  comment by hansen  2014-12-24
// 		CString url;
// 		url.Format("%s/app/%s?userid=%d&token=%s&url=%s",
// 			Glb().m_CenterServerPara.m_strWebRootADDR,loginURL,m_PlaceUserInfo.dwUserID,Glb().m_TML_SN,Glb().m_CenterServerPara.m_strHomeADDR);
// 		IE(url);
		//记录信息
		TCHAR szBuffer[100];
		memset(szBuffer,0,sizeof(szBuffer));
		wsprintf(szBuffer,TEXT("%s"),m_PlaceUserInfo.szName);////用户名
		CLogonDialog::WriteUserPass(szBuffer,CLogonDialog::m_bRemberPass?CLogonDialog::m_szScrPass:NULL,LK_BY_NAME);//注册表
		CLogonDialog::WriteUserLogonTime(szBuffer,LK_BY_NAME);//注册表
		if (m_PlaceUserInfo.dwAccID!=0L)//接受ID
		{
			wsprintf(szBuffer,TEXT("%d"),m_PlaceUserInfo.dwAccID);
			CLogonDialog::WriteUserPass(szBuffer,CLogonDialog::m_bRemberPass?CLogonDialog::m_szScrPass:NULL,LK_BY_ACC);
			CLogonDialog::WriteUserLogonTime(szBuffer,LK_BY_ACC);//注册表
		}
		if (m_PlaceUserInfo.dwMobile!=0L)//接受手机
		{
			wsprintf(szBuffer,TEXT("%d"),m_PlaceUserInfo.dwMobile);
			CLogonDialog::WriteUserPass(szBuffer,CLogonDialog::m_bRemberPass?CLogonDialog::m_szScrPass:NULL,LK_BY_MOBILE);
			CLogonDialog::WriteUserLogonTime(szBuffer,LK_BY_MOBILE);//注册表
		}
		wsprintf(szBuffer,TEXT("%d"),m_PlaceUserInfo.dwUserID);
		CLogonDialog::WriteUserPass(szBuffer,CLogonDialog::m_bRemberPass?CLogonDialog::m_szScrPass:NULL,LK_BY_ID);
		CLogonDialog::WriteUserLogonTime(szBuffer,LK_BY_ID);//注册表
		

		//修改任务栏标题
		if(lstrlen(m_PlaceUserInfo.nickName)==0)
			lstrcpy(m_PlaceUserInfo.nickName,CLogonDialog::m_nickname);
		if(lstrlen(m_PlaceUserInfo.nickName)==0)
			lstrcpy(m_PlaceUserInfo.nickName,CLogonDialog::m_szName);
		strTitle.Format(TEXT("-［%s］"),m_PlaceUserInfo.nickName);
		m_strTitle+=strTitle;

		//魅力值兑换框数据更新
		if (CharmPannel)
		{
			CharmPannel->UpdateFascination();
		}

		//将人物信息传给个人信息栏	
		if(m_pGameListWnd!=NULL)
			//m_pGameListWnd->SetUserInfoWnd(&m_PlaceUserInfo);
		{
			m_pUserNickName = static_cast<BzDui::CLabelUI*>(m_TopDuWnd->m_pm.FindControl("UserNickname"));
			m_pUserGoldCoint =static_cast<BzDui::CLabelUI*>(m_TopDuWnd->m_pm.FindControl("UserGoldCoint"));
			m_pUserIdentity = static_cast<BzDui::CLabelUI*>(m_TopDuWnd->m_pm.FindControl("UesrIdentity"));
			m_pUserBonus = static_cast<BzDui::CLabelUI*>(m_TopDuWnd->m_pm.FindControl("UserBonus"));
			if(m_pUserNickName != NULL)
			{
				CString csImg;
				csImg.Format("%s",this->m_PlaceUserInfo.nickName);
				//OutputDebugString(csImg);
				m_pUserNickName->SetText(csImg);
			}
			if(m_pUserGoldCoint != NULL)
			{
				CString csImg;
				csImg.Format("%ld",this->m_PlaceUserInfo.dwUserID);
				m_pUserGoldCoint->SetText(csImg);
			}
			if(m_pUserIdentity != NULL)
			{
// 				UINT %u
// 				%x
				TCHAR szNum[128] = {0};
				GetNumString(szNum, this->m_PlaceUserInfo.i64Money, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
				CString csImg;
				csImg.Format("%s",szNum);
				m_pUserIdentity->SetText(csImg);
			}
			if(m_pUserBonus != NULL)
			{
				CString csImg;
				csImg.Format("%d",m_pGameListWnd->GetMailCount());
				
				m_pUserBonus->SetText(csImg);
			}
		}
		/// 处理头像越界问题
		m_PlaceUserInfo.bLogoID %= 12;
		m_pGameListWnd->m_IMList.checkUserCount();

		//登录后是否自动显示
		CString s=CBcfFile::GetAppPath ();/////本地路径
		CBcfFile f( s + "bzgame.bcf");
		int isShowImAfterLogin=f.GetKeyVal (Glb().m_key,"ShowIMAfterLogin",1);

		//是否有登录公告
		if(m_PlaceUserInfo.bLoginBulletin)
			OpenBulletin();

		//取得已有道具列表
		propGetListRequest();

		GetCharmExchangeList();

        //  处理第三方登录, 发送登录成功消息
        ::PostMessage(g_hWndDLL, WM_GAME_NOTIFY_TO_DLL, WPARAM_GAME_LOGON_SUCCESS, 0);
		
		SetTimer(TIME_KEEP_ALIVE,5000,NULL);

	#ifdef MY_QUICK_GAME_ON

		OpenQuickGameNamedPipe("\\\\.\\pipe\\BZ_ROOM");

	#endif // MY_QUICK_GAME_ON
		//获取游戏列表

		m_pGameListWnd->m_pLeftWnd->UnInit();

		bool bRetValue = GetGameRoomList();

		GetLastLoginGame();

		//用户安全检查
		if (!CSafeCenterWnd::CheckSafe(&m_PlaceUserInfo))
		{
			if (m_pSafeCenterWnd == NULL)
			{
				if (GetMainRoom()->m_TopDuWnd != NULL)
					GetMainRoom()->m_TopDuWnd->SetMaxOrRestoreBtn(false);

				m_pSafeCenterWnd = new CSafeCenterWnd();
				if(!m_pSafeCenterWnd)
				{
					return false;
				}
				m_pSafeCenterWnd->m_pUserInfo = &m_PlaceUserInfo;
				m_pSafeCenterWnd->Create(m_hWnd,NULL,UI_WNDSTYLE_DIALOG,0,0,0,0,0,NULL);
				m_pSafeCenterWnd->CenterWindow();
				m_pSafeCenterWnd->ShowModal();

				int nRetVal = m_pSafeCenterWnd->m_nRetFlag;

				delete m_pSafeCenterWnd;
				m_pSafeCenterWnd = NULL;

				if (GetMainRoom()->m_TopDuWnd != NULL)
					GetMainRoom()->m_TopDuWnd->SetMaxOrRestoreBtn(true);
			}
		}
		//end

		CBzLog::init();
		SYSTEMTIME t;
		::GetLocalTime(&t);
		char szSec[100];
		_stprintf(szSec,"%04d%02d%02d_%02d.%02d.%02d_%s(%d)",\
			t.wYear,t.wMonth,t.wDay,t.wHour,t.wMinute,t.wSecond,\
			m_PlaceUserInfo.nickName,m_PlaceUserInfo.dwUserID);

		CBzLog::CreateGameRoomDir(szSec);
	
		return bRetValue;
	}
	else if (pNetHead->bAssistantID==ASS_GP_LOGON_ERROR)	//登陆失败
	{
		//设置控件
		if (m_Connect.GetSafeHwnd()!=NULL) m_Connect.DestroyWindow();
		pClientSocket->CloseSocket(false);

		//显示错误信息
		CString strMessage;
		switch (pNetHead->bHandleCode)
		{
		case ERR_GP_USER_NO_FIND:
			//没有这个用户则注册
			//新用户注册
			if(Glb().m_autoreg||Glb().m_weblogon)
			{
				CLogonDialog::m_bRegUser=true;
				memset(m_Mission,0,sizeof(m_Mission));
				AddMission(MS_LOGON,0,0);
				return true;
			}
		case ERR_GP_USER_PASS_ERROR:
			{
				if(Glb().m_autoreg)
				{
				//	strMessage=TEXT("很抱歉，这个用户名已经有人注册了！");
					strMessage = fMsg.GetKeyVal("MainRoom","UserNameExist","很抱歉，这个用户名已经有人注册了！");
					Glb().m_autoreg=false;
					break;
				}
			//	strMessage=TEXT("用户不存在或者密码错误，请查证后再次登录 ");
				strMessage = fMsg.GetKeyVal("MainRoom","UserNameOrPWError","用户不存在或者密码错误，请查证后再次登录 ");
				break;
			}
		case ERR_GP_USER_VALIDATA:
			{
			//	strMessage=TEXT("此帐号已被禁止登录！");
				strMessage = fMsg.GetKeyVal("MainRoom","ForbidAccount","此帐号已被禁止登录！");
				break;
			}
		case ERR_GP_USER_IP_LIMITED:
			{
			//	strMessage=TEXT("您所在的 IP 地址被禁止登陆！");
				strMessage = fMsg.GetKeyVal("MainRoom","ForbidIP","您所在的 IP 地址被禁止登陆！");
				break;
			}
		case ERR_GP_USER_EXIST:
			{
			//	strMessage=TEXT("很抱歉，这个用户名已经有人注册了！");
				strMessage = fMsg.GetKeyVal("MainRoom","UserNameExist","很抱歉，这个用户名已经有人注册了！");
				break;
			}
		case ERR_GP_PASS_LIMITED:
			{
			//	strMessage=TEXT("输入的密码错误！");
				strMessage = fMsg.GetKeyVal("MainRoom","PassWordInputError","输入的密码错误！");
				break;
			}
		case ERR_GP_IP_NO_ORDER:
			{
			//	strMessage=TEXT("您需要在指定的 IP 地址的系统登陆！");
				strMessage = fMsg.GetKeyVal("MainRoom","LogonSpecifiedIP","您需要在指定的 IP 地址的系统登陆！");
				break;
			}
		case ERR_GP_USER_LOGON:
			{
			//	strMessage=TEXT("此帐号已经登录！");
				strMessage = fMsg.GetKeyVal("MainRoom","AccountHasLongoned","此帐号已经登录！");
				break;
			}
		case ERR_GP_USERNICK_EXIST:
			{
			//	strMessage=TEXT("此昵称已经存在！");
				strMessage = fMsg.GetKeyVal("MainRoom","NameExist","此昵称已经存在！");
				break;
			}		
		case ERR_GP_USER_BAD:
			{
			//	strMessage=TEXT("内含有非法字符！");
				strMessage = fMsg.GetKeyVal("MainRoom","ContainsIllegalCharacters","内含有非法字符！");
				break;
			}	
		case ERR_GP_IP_FULL:
			{
			//	strMessage=TEXT("同一IP每天只能注册20个帐号！");
				strMessage = fMsg.GetKeyVal("MainRoom","IPAllowAccounts","同一IP每天只能注册20个帐号！");
				break;
			}
		case ERR_GP_ACCOUNT_LOCKED:
			{
			//	strMessage=TEXT("您好，本帐号已经在其他机器上锁机！\r\n请使用正确的机器来登录。");
				strMessage = fMsg.GetKeyVal("MainRoom","AccountWasLocked",
					"您好，本帐号已经在其他机器上锁机！\r\n请使用正确的机器来登录。");
				break;
			}
		case ERR_GP_MATHINE_LOCKED:
			{
				strMessage = fMsg.GetKeyVal("MainRoom","MathineLocked",
					"您的电脑已经被禁止游戏,请您联系管理员！");
				break;
			}
            // PengJiLin, 2010-6-7, 第三方认证返回的错误。（鸥朴）
        case ERR_GP_USER_NOT_EXIST:         // 用户不存在
            {
                strMessage = fMsg.GetKeyVal("MainRoom","UserNotExist","用户不存在");
                break;
            }
        case ERR_GP_USER_OVERDATE:          // 用户已过期
            {
                strMessage = fMsg.GetKeyVal("MainRoom","UserOverDate","用户已过期");
                break;
            }
        case ERR_GP_DATABASE_FUNC_ERROR:    // 数据库操作失败
            {
                strMessage = fMsg.GetKeyVal("MainRoom","DatabaseFuncError","数据库操作失败");
                break;
            }
        case ERR_GP_DATABASE_RETURN_ERROR:  // 数据库返回执行错误结果
            {
                strMessage = fMsg.GetKeyVal("MainRoom","DatabaseReturnError","数据库返回执行错误结果");
                break;
            }
		default:
			{
				str = fMsg.GetKeyVal("MainRoom","LoginError","系统登录出现错误，错误码[%d]！请与管理员联系！");
				strMessage.Format(str, pNetHead->bHandleCode);
			}
		}

		BZSoundPlay(this, "music/错误提示.mp3", 0);
		str = fMsg.GetKeyVal("MainRoom","Error","错误");
		DUIMessageBox(m_hWnd,MB_ICONERROR|MB_OK,str,strMessage);

		if (m_pBankVerifyWnd)
		{
			m_pBankVerifyWnd->Close();
			PostMessage(WM_BANK_CLOSE,0,0);
		}

		if (m_pBankWndEx)
		{
			m_pBankWndEx->Close();
			PostMessage(WM_BANK_CLOSE,0,0);
		}

		if (m_pBankWnd)
		{
			m_pBankWnd->Close();
			PostMessage(WM_BANK_CLOSE,0,0);
		}

		CMainRoomEx::m_bBankLogoned = false;

		PostMessage(WM_COMMAND,IDM_LOGON_PLACE,0);
		return false;
	}
    else if(pNetHead->bAssistantID == ASS_GP_LOGON_BY_SOFTWARE)   // 重新验证登录
    {
        SendUserInfo();     // 重新登录
        return true;
    }
	else if (pNetHead->bAssistantID == ASS_GP_GET_SELLGAMELIST)   //销售列表添加
    {
		MSG_GP_R_SellGameListResult temp =*(( MSG_GP_R_SellGameListResult*)pNetData);
		m_SellListInfo.KindID = temp.KindID;
		m_SellListInfo.NameID = temp.NameID;
		memcpy(m_SellListInfo.szGameName,temp.szGameName,61);
		memcpy(m_SellListInfo.szKindName,temp.szKindName,61);
		m_SellListInfo.ParamID = temp.ParamID;
		m_pGameListWnd->m_GameList.m_SellListVec.push_back(temp);
		int b =m_pGameListWnd->m_GameList.m_SellListVec.size();

		if (m_pGameListWnd->m_GameList.m_SellListVec.size())
		{
			m_HaveSellList = true;
		}
		
    }
	return true;
}

//银行消息
bool CMainRoomEx::OnBankMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");

	TCHAR szNum[128];
	/// 如果游戏启动了，则发送到游戏客户端
	
	switch (pNetHead->bAssistantID)
	{
	case ASS_GP_BANK_TRANS_RECORD:	///< 转帐记录
		{
			if (pNetHead->bHandleCode == HC_GP_BANK_TRANS_RECORD_SUC)
			{
				//效验数据
				if(uDataSize!=sizeof(MSG_GR_R_TransferRecord_t))
				{
					return false;
				}
				MSG_GR_R_TransferRecord_t *pRecord = (MSG_GR_R_TransferRecord_t *)pNetData;

				if (m_pBankWndEx)
				{
					m_pBankWndEx->OnReceivedTransferRecord(pRecord);
				}

				if (m_pBankWnd)
				{
					m_pBankWnd->OnReceivedTransferRecord(pRecord);
				}
			}
		}
		break;
// 	case ASS_GR_CHANGE_PASSWD:	///< 修改密码
// 		{
// 			/// 只需要bHandelCode就可以了
// 			CString strBuffer;
// 			CString str;
// 			if (pNetHead->bHandleCode == ASS_GR_OPEN_WALLET_SUC)
// 			{
// 				//BZSoundPlay(this, "music/错误提示.mp3", 0);
// 				BZSoundPlay(this, "music/点击确定按钮弹出消息提示框.mp3", 0);
// 				str = fMsg.GetKeyVal("GameRoom","ChangePasswdOK","修改银行密码成功！请妥善保管新密码。");
// 				strBuffer.Format(str);
// 				AFCMessageBox(strBuffer,m_GameInfo.szGameName);//,MB_OK,this);
// 
// 				break;
// 			}
// 			else
// 			{
// 				BZSoundPlay(this, "music/错误提示.mp3", 0);
// 				str = fMsg.GetKeyVal("GameRoom","ChangePasswdFailed","修改银行密码失败！");
// 			}
// 			strBuffer.Format(str);
// 			AFCMessageBox(strBuffer,m_GameInfo.szGameName);//,MB_OK,this);
// 			break;
// 		}
	case ASS_GP_BANK_OPEN:	///< 打开银行
		{
			if (m_Connect.GetSafeHwnd()!=NULL)
				m_Connect.DestroyWindow();

			if (pNetHead->bHandleCode == HC_GP_BANK_OPEN_ERR)
			{
				CString strBuffer;
				CString str;
				BZSoundPlay(this, "music/错误提示.mp3", 0);
				str = fMsg.GetKeyVal("GameRoom","ErrorInGettingMoney","打开钱柜失败,请稍后再试!");
				strBuffer.Format(str);
				DUIOkeyMsgBox(m_hWnd,strBuffer);
			}
			else if (pNetHead->bHandleCode == HC_GP_BANK_OPEN_SUC)
			{
				TMSG_GP_BankInfo* pBank = (TMSG_GP_BankInfo*)pNetData;
				CString strBuffer;
				if(uDataSize != sizeof(TMSG_GP_BankInfo)) 
				{
					CString str;
					BZSoundPlay(this, "music/错误提示.mp3", 0);
					str = fMsg.GetKeyVal("GameRoom","ErrorInGettingMoney","打开钱柜失败,请稍后再试!");
					strBuffer.Format(str);
					DUIOkeyMsgBox(m_hWnd,strBuffer);
				}

				GetMainRoom()->m_pGameListWnd->UpdateMeFortune(pBank->i64Wallet,pBank->i64Bank);
				GetMainRoom()->m_pGameListWnd->Invalidate();

				if(pBank->nVer == 1) //旧版银行
				{
					if (m_pBankWnd == NULL)
					{
						m_pBankWnd = new CPBankWnd(this,true);
						m_pBankWnd->Create(m_hWnd,NULL,UI_WNDSTYLE_DIALOG,0,0,0,0,0,NULL);
						m_pBankWnd->SetWealthInfor(pBank->i64Bank, m_PlaceUserInfo.i64Money);
						m_pBankWnd->CenterWindow();
						m_pBankWnd->ShowWindow(SW_SHOW);
					}
					/*DUIOkeyMsgBox(m_hWnd,"请进入房间再使用银行");*/
				}
				else if (pBank->nVer == 2) //新版银行
				{
					if (m_pBankWndEx == NULL && m_pBankVerifyWnd == NULL)
					{
						int nSelect = AfxGetApp()->GetProfileInt(TEXT("BankSafe"),TEXT("Option"),1);

						int nRetVal = 0;

						if (nSelect == 1 || nSelect == 2)
						{
							nRetVal = 0;
						}
						else if (nSelect == 3)
						{
							if (CMainRoomEx::m_bBankLogoned)
							{
								nRetVal = 1;
							}
						}

						//在测试版本中按下Shift键可跳过身份验证
						#ifdef MY_BANK_PASSWORD_OFF
							if (::GetKeyState(VK_LSHIFT) & 0x8000)
							{
								nRetVal = 1;
								CMainRoomEx::m_bBankLogoned = true;
							}
						#endif

						if (nRetVal == 0 || !CMainRoomEx::m_bBankLogoned)
						{
							if (GetMainRoom()->m_TopDuWnd != NULL)
								GetMainRoom()->m_TopDuWnd->SetMaxOrRestoreBtn(false);

							m_pBankVerifyWnd = new CBankVerifyWnd();
							m_pBankVerifyWnd->m_pVerifyInfo = (MSG_GR_UserBank*)pBank;

							SendMsgUserStruct tagMeInfo;
							tagMeInfo.dwUserID = m_PlaceUserInfo.dwUserID;
							::strcpy(tagMeInfo.nickName,m_PlaceUserInfo.nickName);
							m_pBankVerifyWnd->m_pMyInfo = &tagMeInfo;

							m_pBankVerifyWnd->Create(m_hWnd,NULL,UI_WNDSTYLE_DIALOG,0,0,0,0,0,NULL);
							m_pBankVerifyWnd->CenterWindow();
							m_pBankVerifyWnd->ShowModal();
							nRetVal = m_pBankVerifyWnd->m_nRetFlag;
							delete m_pBankVerifyWnd;
							m_pBankVerifyWnd = NULL;

							if (GetMainRoom()->m_TopDuWnd != NULL)
								GetMainRoom()->m_TopDuWnd->SetMaxOrRestoreBtn(true);

							if (nRetVal == 1)
							{
								CMainRoomEx::m_bBankLogoned = true;
							}
						}

						if (nRetVal == 1)
						{
							m_pBankWndEx = new CPBankWndEx(this,true);
							m_pBankWndEx->Create(m_hWnd,NULL,UI_WNDSTYLE_DIALOG,0,0,0,0,0,NULL);
							m_pBankWndEx->SetWealthInfor(pBank->i64Bank,0);
							m_pBankWndEx->CenterWindow();
							m_pBankWndEx->ShowWindow(SW_SHOW);	
							PostMessage(WM_BANK,2,NULL);
						}
					}
				} //bank ver
			} //bank handle code
		}
		break;
	case ASS_GP_BANK_TRANSFER://转帐
		{
			CString strBuffer;
			CString str;
			TCHAR szNum[128], szBank[128];

			//效验数据
			if (uDataSize != sizeof(TMSG_GP_BankTransfer))
			{
				return false;
			}

			TMSG_GP_BankTransfer* pTransfer = (TMSG_GP_BankTransfer*)pNetData;
			/// 如果转帐不成功，且是在平台发出的转帐指令，则弹出对话框，若是游戏中发的指令，则平台只响应对话框事件


			/// 转帐成功，设置大厅金币和银行界面金币
			if(pNetHead->bHandleCode == HC_GP_BANK_TRANSFER_SUC)
			{
				TCHAR szAct[128];
				CString strBuffer;
				CString str;
				GetNumString(szNum, pTransfer->i64Money, Glb().m_nPowerOfGold/*, false*/,Glb().m_bUseSpace, Glb().m_strSpaceChar);
				GetNumString(szAct, pTransfer->i64ActualTransfer, Glb().m_nPowerOfGold/*, false*/,Glb().m_bUseSpace, Glb().m_strSpaceChar);
				CMainRoomEx * pMainRoom = GetMainRoom();
				/// 是自己转给别人
				if(pTransfer->UserID == m_PlaceUserInfo.dwUserID) //自己
				{
					str = fMsg.GetKeyVal("GameRoom","TransferCoinsId",
						"<p 5>到账通知，您已给用户 <b>%s</b>(%d) 转帐。<n><y 30>      转帐数额：<b>%s</b> 金币</y><n>扣手续实际到账：<b>%s</b> 金币</p>");
					strBuffer.Format(str,pTransfer->szDestNickName,pTransfer->destUserID,szNum,szAct);

					BZSoundPlay(this, "music/点击确定按钮弹出消息提示框.mp3", 0);
					//AFCMessageBox(strBuffer);//,MB_OK,this);
					DUIOkeyMsgBox(m_hWnd,strBuffer.GetBuffer());

					//更新对话框

					if (m_pBankWndEx)
					{
						m_pBankWndEx->UpdateUserMoneyByTransfer(0,pTransfer->i64Money);
					}

					if (m_pBankWnd)
					{
						m_pBankWnd->UpdateUserMoneyByTransfer(0,pTransfer->i64Money);
					}

					m_PlaceUserInfo.i64Bank -= pTransfer->i64Money;
					UpdateMeInfo(0, false/*true*/, 0, false);         // PengJiLin, 2010-10-9, 避免金币显示为0
				}

				return true;
			}

			switch(pNetHead->bHandleCode)
			{
			case HC_GP_BANK_TRANSFER_PASSWORD_ERROR://转帐密码错误
				{
					BZSoundPlay(this, "music/错误提示.mp3", 0);
					str = fMsg.GetKeyVal("GameRoom","TransferPassWdError","您输入的密码没有通过验证，请重新输入密码！");
					strBuffer.Format(str, szNum);
					DUIOkeyMsgBox(m_hWnd,strBuffer);
					return true;
				}
			case HC_GP_BANK_TRANSFER_TOTAL_LESS://银行总额太小，不够资格
				{
					BZSoundPlay(this, "music/错误提示.mp3", 0);
					GetNumString(szNum, pTransfer->i64ActualTransfer, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
					str = fMsg.GetKeyVal("GameRoom","TransferTotalLess","您暂时还不可以转帐，银行存款达到或超过%s金币才可以转帐！");
					strBuffer.Format(str, szNum);
					DUIOkeyMsgBox(m_hWnd,strBuffer);
					return true;
				}
				break;
			case HC_GP_BANK_TRANSFER_TOO_LESS://单笔转帐数目太少
				{
					BZSoundPlay(this, "music/错误提示.mp3", 0);
					GetNumString(szNum, pTransfer->i64ActualTransfer, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
					str = fMsg.GetKeyVal("GameRoom","TransferTooLess","转帐不成功，单次转帐金额必须大于%s金币。");
					strBuffer.Format(str, szNum);
					DUIOkeyMsgBox(m_hWnd,strBuffer);
					return true;
				}
				break;
			case HC_GP_BANK_TRANSFER_MULTIPLE://单笔转帐数目必须是某数的倍数
				{
					BZSoundPlay(this, "music/错误提示.mp3", 0);
					GetNumString(szNum, pTransfer->i64ActualTransfer, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
					str = fMsg.GetKeyVal("GameRoom","TransferMultiple","转帐不成功！转帐金额必须是%s的整数倍。");
					strBuffer.Format(str, szNum);
					DUIOkeyMsgBox(m_hWnd,strBuffer);
					return true;
				}
			case HC_GP_BANK_TRANSFER_NOT_ENOUGH://银行金额不够本次转帐
				{
					BZSoundPlay(this, "music/错误提示.mp3", 0);
					GetNumString(szNum, pTransfer->i64Money, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
					GetNumString(szBank, pTransfer->i64ActualTransfer, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
					str = fMsg.GetKeyVal("GameRoom","TransferNotEnoughMoney","转帐不成功！您银行中只有%s金币，不够支付本次转帐%s金币。");
					strBuffer.Format(str, szBank, szNum);
					DUIOkeyMsgBox(m_hWnd,strBuffer);
					return true;
				}
			case HC_GP_BANK_TRANSFER_TOO_MANY_TIME:
				{
					BZSoundPlay(this, "music/错误提示.mp3", 0);
					str = fMsg.GetKeyVal("GameRoom","TransferTooMany","您今天转账的数额达到了最高限度! 建议输入小一些的数值，再转账试试。"); // PengJiLin, 2010-8-25
					strBuffer.Format(str);
					DUIOkeyMsgBox(m_hWnd,strBuffer);
					return true;
				}
			case HC_GP_BANK_TRANSFER_NO_DEST:											///< 转帐目标不存在
				{
					BZSoundPlay(this, "music/错误提示.mp3", 0);
					str = fMsg.GetKeyVal("GameRoom","AccountNotExist","转帐目标不存在,请查实帐号再试!");
					strBuffer.Format(str);
					DUIOkeyMsgBox(m_hWnd,strBuffer);
					return true;
				}
				break;
			default:
				{
					CString strBuffer;
					CString str;
					BZSoundPlay(this, "music/错误提示.mp3", 0);
					str = fMsg.GetKeyVal("GameRoom","NetWorkBusySave","网络拥塞,存入失败!请稍后再试!");
					strBuffer.Format(str);
					DUIOkeyMsgBox(m_hWnd,strBuffer);
					return true;
				}
			}
			break;
		}
	case ASS_GP_BANK_FINANCE_INFO:
		{
			//效验数据
			if (uDataSize != sizeof(TMSG_GP_BankFinanceInfo))
			{
				return false;
			}

			TMSG_GP_BankFinanceInfo* pFinanceInfo = (TMSG_GP_BankFinanceInfo*)pNetData;

			if (m_pGameListWnd)
			{
				if (m_pGameListWnd->m_pFinanceWnd)
				{
					if(pNetHead->bHandleCode == HC_GP_BANK_FINANCE_INFO_SUC)
					{
						m_pGameListWnd->m_pFinanceWnd->ShowFinanceInfo(pFinanceInfo);
					}
					else if(pNetHead->bHandleCode == HC_GP_BANK_FINANCE_INFO_ERR)
					{
						m_pGameListWnd->m_pFinanceWnd->ShowFinanceInfo(NULL);
					}
				}
			}
			

		}
		break;
// 	case ASS_GR_SENDMONEY_TIMES: 
// 		{
// 			SendMoneyOnTimesAndCounts*  pSendResult = (SendMoneyOnTimesAndCounts*)pNetData;
// 
// 			UserItemStruct* pUserInfo = NULL;
// 			BOOL bUserIsMe = FALSE;
// 			if(pSendResult->dwUserID == m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)
// 			{
// 				pUserInfo = m_GameInfo.pMeUserInfo;
// 				bUserIsMe = TRUE;
// 			}
// 			else
// 			{
// 				pUserInfo = m_UserManage.FindOnLineUser(pSendResult->dwUserID);
// 			}
// 
// 			if(NULL != pUserInfo)       // 用户在房间里
// 			{
// 				pUserInfo->GameUserInfo.i64Money += pSendResult->dwGetMoney;
// 				if(TRUE == bUserIsMe)
// 				{
// 					CString strMessage;
// 					TCHAR str[100];
// 
// 					if(pSendResult->dwGetMoney > 0)
// 					{
// 						GetNumString(szNum, pSendResult->dwGetMoney, Glb().m_nPowerOfGold,
// 							Glb().m_bUseSpace, Glb().m_strSpaceChar);
// 
// 						strMessage = fMsg.GetKeyVal("GameRoom","NoGiftCoins","/:54尊敬的[%s]在游戏中获得系统赠送%s金币/:54");
// 						wsprintf(str,strMessage,m_GameInfo.pMeUserInfo->GameUserInfo.nickName,szNum);
// 						m_pRightWnd->m_MessageHandle.InsertSystemMessage(str);
// 						BZSoundPlay(this, "music/点击确定按钮弹出消息提示框.mp3", 0);
// 					}
// 
// 					if (m_IGameFrame != NULL) 
// 					{
// 						// 重新定义变量的字段
// 
// 						CM_UserState_Send_Times_Money ControlMessage;
// 						memset(&ControlMessage,0,sizeof(ControlMessage));
// 
// 						ControlMessage.bActionCode = ACT_USER_SEND_TIMES_MONEY;
// 
// 						ControlMessage.dwUserID = pSendResult->dwUserID;
// 						ControlMessage.dwGetMoney = pSendResult->dwGetMoney;
// 						ControlMessage.dwMoneyOnTimes = pSendResult->dwMoneyOnTimes;
// 						ControlMessage.dwMoneyOnCounts = pSendResult->dwMoneyOnCounts;
// 						ControlMessage.dwTimesNeed = pSendResult->dwTimesNeed;
// 						ControlMessage.dwCountsNeed = pSendResult->dwCountsNeed;
// 
// 						if (m_IGameFrame!=NULL)
// 							m_IGameFrame->OnControlMessage(CM_USER_SEND_TIMES_MONEY,&ControlMessage,sizeof(ControlMessage));
// 
// 					}
// 				}
// 			}
// 
// 			break;
// 
// 			// PengJiLin, 2011-4-15, 以下为旧的送金币功能，不使用。
// 
// 			SendUserMoneyByTimes* pSendMoneyResult = (SendUserMoneyByTimes*)pNetData;
// 
// 			UserItemStruct* pGameUserInfo = NULL;
// 			bool bIsMe = false;
// 			if (pSendMoneyResult->dwUserID == m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID) //获赠的金额不等
// 			{
// 				pGameUserInfo = m_GameInfo.pMeUserInfo;
// 				bIsMe = true;
// 			}
// 			else
// 			{
// 				pGameUserInfo = m_UserManage.FindOnLineUser(pSendMoneyResult->dwUserID);
// 			}
// 
// 			if (pGameUserInfo != NULL) //用户在房间里
// 			{
// 				pGameUserInfo->GameUserInfo.i64Money += pSendMoneyResult->dwSendGameMoney;
// 				if(bIsMe)
// 				{
// 					CString strMessage;
// 					TCHAR str[100];
// 
// 					int iTimes;
// 
// 					iTimes = pSendMoneyResult->dwAllSendTImes - pSendMoneyResult->dwSendTimes;
// 					GetNumString(szNum, pSendMoneyResult->dwSendGameMoney, Glb().m_nPowerOfGold/*, false*/,Glb().m_bUseSpace, Glb().m_strSpaceChar);
// 
// 					if(iTimes<=0)
// 					{
// 						strMessage = fMsg.GetKeyVal("GameRoom","NoGiftCoins","/:54尊敬的[%s]在游戏中获赠%s金币，已获赠%d次，已无获赠机会/:54");
// 						wsprintf(str,strMessage,
// 							m_GameInfo.pMeUserInfo->GameUserInfo.nickName,szNum,
// 							pSendMoneyResult->dwSendTimes);
// 					}
// 					else
// 					{
// 						strMessage = fMsg.GetKeyVal("GameRoom","GiftCoins","/:54尊敬的[%s]在游戏中获赠%s金币，已获赠%d次，还有%d次获赠机会/:54");
// 						wsprintf(str,strMessage,
// 							m_GameInfo.pMeUserInfo->GameUserInfo.nickName,szNum,
// 							pSendMoneyResult->dwSendTimes,iTimes);
// 					}
// 					BZSoundPlay(this, "music/点击确定按钮弹出消息提示框.mp3", 0);
// 					m_pRightWnd->m_MessageHandle.InsertSystemMessage(str);//send gameroom;
// 					if (m_IGameFrame != NULL) 
// 					{
// 						CM_UserState_Send_Times_Money ControlMessage;
// 						memset(&ControlMessage,0,sizeof(ControlMessage));
// 
// 						ControlMessage.bActionCode = ACT_USER_SEND_TIMES_MONEY;
// 
// 						//ControlMessage.iLastTimes = iTimes;
// 						//ControlMessage.dwUserID = pSendMoneyResult->dwUserID;
// 						//ControlMessage.iSendTimes = pSendMoneyResult->dwSendTimes;
// 						//ControlMessage.iSendMoney = pSendMoneyResult->dwSendGameMoney;
// 
// 						if (m_IGameFrame!=NULL)
// 							m_IGameFrame->OnControlMessage(CM_USER_SEND_TIMES_MONEY,&ControlMessage,sizeof(ControlMessage));
// 
// 					}
// 				}
// 			}
// 		}
// 		break;
	}

	//某些消息直接交给银行窗口对象内部处理
	if (m_pBankWndEx)
	{
		m_pBankWndEx->OnNetMessage(pNetHead,pNetData,uDataSize,pClientSocket);
		return true;
	}

	if (m_pBankWnd)
	{
		m_pBankWnd->OnNetMessage(pNetHead,pNetData,uDataSize,pClientSocket);
		return true;
	}

	//游戏房间的银行
	for (BYTE i = 0; i < MAX_GAME_ROOM; ++i)
	{
		if (m_RoomInfo[i].bAccess)
		{
			// 游戏中游戏界面右边消息栏
			CGameRoomEx* pGameRoomEx = (CGameRoomEx*)m_RoomInfo[i].pGameRoomWnd;
			if (NULL != pGameRoomEx)
			{
				if (pGameRoomEx->m_pBankWndEx)
				{
					pGameRoomEx->m_pBankWndEx->OnNetMessage(pNetHead,pNetData,uDataSize,pClientSocket);
					return true;
				}

				if (pGameRoomEx->m_pBankWnd)
				{
					pGameRoomEx->m_pBankWnd->OnNetMessage(pNetHead,pNetData,uDataSize,pClientSocket);
					return true;
				}
			}					
		}//bAccess
	}

	return true;
}

//获取机器码相关函数 zxj 2009-11-12 锁定机器
CString CMainRoomEx::innerGetMac()
{
	CString retMac="";
	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter=NULL;
	DWORD dwRetVal=0;
	pAdapterInfo=(IP_ADAPTER_INFO*)malloc(sizeof(IP_ADAPTER_INFO));
	ULONG ulOutBufLen=sizeof(IP_ADAPTER_INFO);
	if(GetAdaptersInfo(pAdapterInfo,&ulOutBufLen)!=ERROR_SUCCESS)
	{
		free(pAdapterInfo);
		pAdapterInfo=(IP_ADAPTER_INFO*)malloc(ulOutBufLen);
	}
	if((dwRetVal=GetAdaptersInfo(pAdapterInfo,&ulOutBufLen))==NO_ERROR)
	{
		pAdapter=pAdapterInfo;
		CString temp;
		while(pAdapter)
		{
			if(pAdapter->Type==6)//pAdapter->Description中包含"PCI"为：物理网卡//pAdapter->Type是71为：无线网卡
			{
				for(UINT i=0;i<pAdapter->AddressLength;i++)
				{
					temp.Format("%02X",pAdapter->Address[i]);
					retMac+=temp;
					//	printf("%02X%c",pAdapter->Address[i],i==pAdapter->AddressLength-1?’n’:’-’);
				}
				break;
			}
			pAdapter=pAdapter->Next;
		}
	}
	if(pAdapterInfo)
		free(pAdapterInfo);
	return retMac;
}

//获取机器码相关函数 zxj 2009-11-12 锁定机器
CString CMainRoomEx::coreGetCode()
{
	CString strRet="";
	CString str=CMainRoomEx::innerGetMac(),s;
	if(str=="")
	{
		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
		CString strMessage;
		strMessage = fMsg.GetKeyVal("MainRoom","GetEthernetError","未检测到系统中有Ethernet设备。");
		return strMessage;
	}

	int r=0;
	int l=str.GetLength();
	for(int i=0;i<l;i+=2)
	{
		int r1=0,r2=0;
		s=str.Mid(i,1);
		if(s>="0" && s<="9")
			r1=atoi(s);
		if(s=="A" || s=="a")	r1=10;
		if(s=="B" || s=="b")	r1=11;
		if(s=="C" || s=="c")	r1=12;
		if(s=="D" || s=="d")	r1=13;
		if(s=="E" || s=="e")	r1=14;
		if(s=="F" || s=="f")	r1=15;
		s=str.Mid(i+1,1);
		if(s>="0" && s<="9")
			r2=atoi(s);
		if(s=="A" || s=="a")	r2=10;
		if(s=="B" || s=="b")	r2=11;
		if(s=="C" || s=="c")	r2=12;
		if(s=="D" || s=="d")	r2=13;
		if(s=="E" || s=="e")	r2=14;
		if(s=="F" || s=="f")	r2=15;

		CString t;
		r+=r1*16+r2;
		srand(r);
		t.Format("%s%04X%s-",str.Mid(l-i-1,1),rand(),str.Mid(l-i-2,1));
		strRet+=t;
	}
	if(strRet.Right(1)=="-")strRet=strRet.Left(strRet.GetLength()-1);
	return strRet;
}

void CMainRoomEx::OnSwitchLockButton()	//切换锁定机器按钮状态
{
// 	CString strCode = coreGetCode();
// 	MSG_GP_S_LockAccount lockMsg;
// 	::ZeroMemory(&lockMsg, sizeof(lockMsg));
// 	lockMsg.dwUserID = m_PlaceUserInfo.dwUserID;
// 	lstrcpy(lockMsg.szMathineCode, strCode);
// 	CLockAccountDlg LockDlg(this, m_bLockMathine);
// 	if( LockDlg.DoModal() == IDCANCEL )
// 		return;
// 	lstrcpy(lockMsg.szMD5Pass, CLockAccountDlg::m_szMD5Pass);
// 
// 	//如果机器当前处于锁定状态，那么接下来的动作就是解除锁定
// 	if( m_bCanClick)
// 	{
// 		m_bCanClick = FALSE;	//令此信号暂时关闭。当计时器超时或服务端返回上一次请求的结果后，此信号重新有效。
// 		if( m_bLockMathine )
// 		{
// 			lockMsg.dwCommanType = 0;	//请求解除锁定
// 		}
// 		else//如果机器当前处于未锁定状态，那么接下来的动作就是锁定机器
// 		{
// 			lockMsg.dwCommanType = 1;	//请求锁定
// 		}
// 		SendData(&lockMsg, sizeof(lockMsg), MDM_GP_LOCK_ACCOUNT,ASS_LOCK_ACCOUNT, 0);	//发出锁定机器命令
// 		m_TopWnd->SetLockWndState(FALSE);	//暂时令锁定按钮失效
// 		SetTimer(IDT_CHANGE_LOCK, CHANGE_LOCK_TIME, NULL);
// 	}

	CUserInfoWnd** pUserInfoWnd = &m_pGameListWnd->m_pUserInfoWnd;
	if (*pUserInfoWnd)
	{
		(*pUserInfoWnd)->Close();
// 		delete *pUserInfoWnd;
// 		*pUserInfoWnd = NULL;
	}

	*pUserInfoWnd = new CUserInfoWnd();

	if(!*pUserInfoWnd)
	{
		return;
	}
	(*pUserInfoWnd)->m_nDefaultPage		= 2;
	(*pUserInfoWnd)->m_nDefaultSubPage	= 1;
	CMainRoomEx::m_bLockQuit = true;

	(*pUserInfoWnd)->SetPlaceUserInfo(m_pGameListWnd->m_PlaceUserInfo);
	(*pUserInfoWnd)->Create(m_hWnd,NULL,UI_WNDSTYLE_DIALOG,0,0,0,0,0,NULL);
	(*pUserInfoWnd)->CenterWindow();
	(*pUserInfoWnd)->ShowModal();

	CMainRoomEx::m_bLockQuit = false;

	delete (*pUserInfoWnd);
	(*pUserInfoWnd) = NULL;

}

//发送手机验证码短信 JianGuankun 2012.5.16
void CMainRoomEx::OnSendMobileVCodeSms(char* szMobileNo,UINT uType)
{
	::memset(m_szMobileVCode,NULL,sizeof(m_szMobileVCode));

	MSG_GP_SmsVCode tagSmsVCode;
	::memset(&tagSmsVCode,0,sizeof(MSG_GP_SmsVCode));
	::strcpy(tagSmsVCode.szName,CLogonDialog::m_szName);
	::strcpy(tagSmsVCode.szMobileNo,szMobileNo);
	tagSmsVCode.nType = uType;

	SendData(&tagSmsVCode,sizeof(MSG_GP_SmsVCode),MDM_GP_SMS,ASS_GP_SMS_VCODE,0);
}

//验证用户输入的手机验证码是否正确 JianGuankun 2012.5.16
bool CMainRoomEx::ValidMobileVCode(char* szVCode)
{
#ifdef MY_MOBILE_VCODE_OFF
	return (::strcmp(szVCode,_T("000000")) == 0);
#endif

	MD5_CTX Md5;
	char szMD5Input[36] = {0};
	Md5.MD5Update((unsigned char*)szVCode,lstrlen(szVCode));
	unsigned char szMDTemp[16];
	Md5.MD5Final(szMDTemp);
	for (int i=0;i<16;i++) 
	{
		wsprintf(&szMD5Input[i*2],"%02x",szMDTemp[i]);
	}

	if (strcmp((const char*)szMD5Input,m_szMobileVCode) == 0)
	{
		return true;
	}

	return false;
}

//擦除手机验证码，使验证码过期 JianGuankun 2012.5.16
void CMainRoomEx::ClearMobileVCode()
{
	::memset(m_szMobileVCode,NULL,sizeof(m_szMobileVCode));
}

// PengJiLin, 2011-7-21, CPU、硬盘 ID
extern CString g_strCPUID;
extern CString g_strHardID;

//发送登陆信息///////////*****************************************//////////////////////////
bool CMainRoomEx::SendUserInfo()
{	//设置信息提示
	if (m_Connect.GetSafeHwnd()==NULL) m_Connect.Create(IDD_CONNECT,this);

	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	CString strMessage;
	strMessage = fMsg.GetKeyVal("MainRoom","CheckingUserInfor","正在验证用户信息...");
	m_Connect.SetMessageText(strMessage);
	m_Connect.ShowWindow(SW_SHOW);

	int rNum=0;
	CString TML_SN = "";
	CString MatchineCode= coreGetCode();	//取得本机机器码  锁定机器

	CString allword="0123456789qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM";
	for (int i=0;i<=120;i++)
	{
		rNum =(rand() * 1000)% 62;
		TML_SN+=allword[rNum];
	}

	Glb().m_TML_SN=TML_SN;
	//发送用户登录信息
	if (CLogonDialog::m_bRegUser==FALSE)
	{
		//正常用户登录
		switch (CLogonDialog::m_bLogonType)
		{
		case LK_BY_NAME:
			{
				MSG_GP_S_LogonByNameStruct LogonByName;///定义一个结构
				memset(&LogonByName, 0, sizeof(LogonByName));

				LogonByName.uRoomVer=GAME_PLACE_MAX_VER;//结构的版本
				lstrcpy(LogonByName.TML_SN,TML_SN);
				lstrcpy(LogonByName.szName,CLogonDialog::m_szName);//结构的用户名
				if(Glb().m_autoreg)
					lstrcpy(LogonByName.szMD5Pass,"");//结构的密码
				else
					lstrcpy(LogonByName.szMD5Pass,CLogonDialog::m_szMD5Pass);//结构的密码
				LogonByName.gsqPs=5471;
				lstrcpy(LogonByName.szMathineCode, MatchineCode);	//锁定机器，把本机机器码传递到登录数据包里 
				
                // CPU、硬盘 ID
                lstrcpy(LogonByName.szCPUID, g_strCPUID);
                lstrcpy(LogonByName.szHardID, g_strHardID);

				//证件ID
				lstrcpy(LogonByName.szIDcardNo,CLogonDialog::m_szIDCard);
				lstrcpy(LogonByName.szMobileVCode,CLogonDialog::m_szMobileVCode);

				LogonByName.iUserID  = -1;

				//如果没有身份验证模块，直接欺骗服务器
				if (CGamePlaceApp::GetModuleStatus("UserSecurity","IDCardVerify",0) == 0)
				{
					strcpy(LogonByName.szIDcardNo,"*");
				}

				//如果没有手机验证模块，直接欺骗服务器
				if (CGamePlaceApp::GetModuleStatus("UserSecurity","MobileVerify",0) == 0)
				{
					strcpy(LogonByName.szMobileVCode,"*");
				}
                
				SendData(&LogonByName,sizeof(LogonByName),MDM_GP_LOGON,ASS_GP_LOGON_BY_NAME,0);
				
				Glb().m_URL=TEXT(Glb().m_CenterServerPara.m_strWebRootADDR);
				Glb().m_URL+=Glb().m_key+":"+TML_SN+"$";

				return true;
			}
		case LK_BY_ID:
			{
				MSG_GP_S_LogonByNameStruct LogonByName;///定义一个结构
				memset(&LogonByName, 0, sizeof(LogonByName));
				LogonByName.iUserID = -1;

				LogonByName.uRoomVer=GAME_PLACE_MAX_VER;//结构的版本
				lstrcpy(LogonByName.TML_SN,TML_SN);
				lstrcpy(LogonByName.szName,CLogonDialog::m_szName);//结构的用户名
				if(Glb().m_autoreg)
					lstrcpy(LogonByName.szMD5Pass,"");//结构的密码
				else
					lstrcpy(LogonByName.szMD5Pass,CLogonDialog::m_szMD5Pass);//结构的密码
				LogonByName.gsqPs=5471;
				lstrcpy(LogonByName.szMathineCode, MatchineCode);	//锁定机器，把本机机器码传递到登录数据包里 zxj 2009-11-12

				// PengJiLin, 2011-7-21, CPU、硬盘 ID
				lstrcpy(LogonByName.szCPUID, g_strCPUID);
				lstrcpy(LogonByName.szHardID, g_strHardID);

				//JianGuankun, 2011-11-10，证件ID
				lstrcpy(LogonByName.szIDcardNo,CLogonDialog::m_szIDCard);
				lstrcpy(LogonByName.szMobileVCode,CLogonDialog::m_szMobileVCode);

				LogonByName.iUserID = atoi(CLogonDialog::m_szName);

				//如果没有身份验证模块，直接欺骗服务器
				if (CGamePlaceApp::GetModuleStatus("UserSecurity","IDCardVerify",0) == 0)
				{
					strcpy(LogonByName.szIDcardNo,"*");
				}

				//如果没有手机验证模块，直接欺骗服务器
				if (CGamePlaceApp::GetModuleStatus("UserSecurity","MobileVerify",0) == 0)
				{
					strcpy(LogonByName.szMobileVCode,"*");
				}

				SendData(&LogonByName,sizeof(LogonByName),MDM_GP_LOGON,ASS_GP_LOGON_BY_NAME,0);

				Glb().m_URL=TEXT(Glb().m_CenterServerPara.m_strWebRootADDR);
				Glb().m_URL+=Glb().m_key+":"+TML_SN+"$";

				return true;
			}
		}
	}
	else 
	{
		//新用户注册
		MSG_GP_S_RegisterStruct UserRegister;
		UserRegister.uRoomVer=GAME_PLACE_MAX_VER;
		UserRegister.bBoy=CLogonDialog::m_bBoy;
		UserRegister.bLogoID=CLogonDialog::m_bLogoID;
		//UserRegister.szRegSQL="inser";
		lstrcpy(UserRegister.szRegSQL,CLogonDialog::m_szRegSQL);
		lstrcpy(UserRegister.szName,CLogonDialog::m_szName);
		lstrcpy(UserRegister.szMD5Pass,CLogonDialog::m_szMD5Pass);
		lstrcpy(UserRegister.nickName,CLogonDialog::m_nickname);
		lstrcpy(UserRegister.szRecommenderName, CLogonDialog::m_szRecommenderName);

		SendData(&UserRegister,sizeof(UserRegister),MDM_GP_LOGON,ASS_GP_LOGON_REG,0);

		Glb().m_URL=TEXT(Glb().m_CenterServerPara.m_strWebRootADDR);
		Glb().m_URL+=Glb().m_key+":"+UserRegister.szName+UserRegister.szMD5Pass+"$";
		CLogonDialog::m_bRegUser=false;
	}
	return true;
}
//获取游戏列表
bool CMainRoomEx::GetGameRoomList()
{
	//设置信息提示
	if (m_Connect.GetSafeHwnd()!=NULL) 
	{
		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
		m_Connect.SetMessageText(fMsg.GetKeyVal("MainRoom","GetGameInfor","正在获取游戏类型列表信息..."));
		m_Connect.ShowWindow(SW_SHOW);
	}
	//发送数据包
	//if (Glb().m_CS==1)//1是使用CS结构
	//{
	SendData(MDM_GP_LIST,ASS_GP_LIST_KIND,0);//请求游戏列表
	//}
	//else 
	//{//;0是禁止CS结构，
	//	m_Connect.DestroyWindow();
	//}
	return true;
}

bool CMainRoomEx::SendSetLogoInfo(long int iUserID, UINT iLogoID)
{
	if (iLogoID <= 0)
		return false;
	MSG_GP_S_SET_LOGO LogoInfo;
	LogoInfo.iUserID = iUserID;
	LogoInfo.iLogoID = iLogoID;
	SendData(&LogoInfo, sizeof(MSG_GP_S_SET_LOGO), MDM_GP_SET_LOGO, ASS_GP_SET_LOGO, 0);
	return true;
}


//获取最近登录的游戏
bool CMainRoomEx::GetLastLoginGame()
{
	//设置信息提示
	if (m_Connect.GetSafeHwnd()!=NULL) 
	{
		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
		m_Connect.SetMessageText(fMsg.GetKeyVal("MainRoom","GetGameInfor","正在获取游戏类型列表信息..."));
		m_Connect.ShowWindow(SW_SHOW);
	}

	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	bool bShow = fMsg.GetKeyVal("MainRoom","ShowLastLoginGame", 0);
	if (bShow)
	{
		m_pGameListWnd->m_GameList.ClearLoginGameName();
		SendData(MDM_GP_LASTLOINGGAME, ASS_GP_LASTLOINGGAME, 0);//请求游戏列表
	}
	
	return true;
}

///获取兑换列表
bool CMainRoomEx::GetCharmExchangeList()
{
	if (CharmPannel != NULL) CharmPannel->Clear();
	SendData(MDM_GP_CHARMEXCHANGE, ASS_GETLIST, 0);//请求游戏列表
	return true;
}


//定时器消息
void CMainRoomEx::OnTimer(UINT nIDEvent)
{
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	CString str;	

	if (nIDEvent == TIME_KEEP_ALIVE)
	{
		if(m_nKeepAliveCount>=5)//客户端断线问题修改
		{
			OnSocketCloseEvent();
		}
		else
		{
			m_nKeepAliveCount++;
			SendData(MDM_CONNECT,ASS_NET_TEST,0);//连接消息类型
		}
	}
#ifdef MY_QUICK_GAME_ON

	if (nIDEvent == IDT_QUICK_ENTER_ROOM)
	{
		KillTimer(nIDEvent);
		CAFCRoomItem* pRoomItem = m_pGameListWnd->m_GameList.FindRoomItem(m_uQGRoomID);	

		if (pRoomItem)
		{
			OnCentranceGameRoom(pRoomItem);
			SetTimer(IDT_QUICK_GAME,500,NULL);
		}
		
	}

	if (nIDEvent == IDT_QUICK_GAME)
	{
		KillTimer(nIDEvent);

		AfxGetMainWnd()->ShowWindow(SW_NORMAL);

		CGameRoomEx* pGameRoom = (CGameRoomEx*)m_RoomInfo[0].pGameRoomWnd;
		if (pGameRoom)
		{
			pGameRoom->OnFindNullDesk();
		}
	}

#endif // MY_QUICK_GAME_ON
	if (nIDEvent==ID_CHECK_SEND)
	{
		KillTimer(nIDEvent);
		///CloseSocket();
	}
	else if(nIDEvent==ID_CHECK_SIGNAL)
    {
        checkNetSignal();
    }
	else if(nIDEvent==CHECK_UPDATE)
	{
		//--------------------------------------------------
		//判断更新进程关闭没有，关闭了，把更新列表 m_UpdateList 清除一下.
		::EnterCriticalSection(&m_Section); 
		//DWORD result=::WaitForSingleObject(m_AnonymousPipe.GetProcessInfo().hProcess,0);
		DWORD result=::WaitForSingleObject(m_CSocket32.GetProcessInfo().hProcess,0);
		if(result!=WAIT_TIMEOUT)
		{
			DWORD exitcode;			
			::GetExitCodeProcess(m_CSocket32.GetProcessInfo().hProcess,&exitcode);			
			m_CSocket32.Close();

			for (vector<LPUPDATE>::iterator it = m_UpdateList.begin(); it != m_UpdateList.end(); it++)
			{
				UPDATE* up = *it;
				delete up;
				up = NULL;
			}
			m_UpdateList.clear();

			//if(result==WAIT_OBJECT_0)
			//{
			//	///////////////////////////////
			//	////Kylin 20090117 关闭更新完成后自动登陆房间
			//	if(exitcode!=-1)
			//	{
			//		if (m_UpdateList.size() == 0)
			//		{
			//			CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
			//			CString str;
			//			CString strTitle;
			//			str = fMsg.GetKeyVal("MainRoom","DownLoadOver","下载完毕");
			//			strTitle = fMsg.GetKeyVal("MainRoom","MessageTitle","提示");
			//			AFCMessageBox(str,strTitle);
			//		}
			//	} 
			//}
		} 
		::LeaveCriticalSection(&m_Section);
	}
#ifdef AI_MACHINE
	else if(ID_AI_ENTERROOM == nIDEvent)
	{
		if(m_Airoom < 0)
			return ;
		if(EnterRoom(m_Airoom))
		{
			KillTimer(ID_AI_ENTERROOM);
			m_Airoom = -1;
		}
	}
#endif
	//闪屏第4：将登录窗口OK后的操作延时100ms操作
	if(nIDEvent==1)
	{	//设置提示
		KillTimer(1);
		if (m_Connect.GetSafeHwnd()==NULL) 
			m_Connect.Create(IDD_CONNECT,this);
		str = fMsg.GetKeyVal("MainRoom","ConnectingLoginServer","正在连接登录服务器，请稍候...");
		m_Connect.SetMessageText(str);

		m_Connect.ShowWindow(SW_SHOW);

		if (m_TCPSocket != NULL && m_TCPSocket->Connect(Glb().m_CenterServerPara.m_strMainserverIPAddr,Glb().m_CenterServerPara.m_iMainserverPort)==false)////GAME_LOGON_PORT///连接服务器失败
		{
			m_Connect.DestroyWindow();
			str = fMsg.GetKeyVal("MainRoom","ConnectingError","连接出现错误，请确认是否配置了网络协议！");
			

            DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",str);

			if (m_pBankVerifyWnd)
			{
				m_pBankVerifyWnd->Close();
				PostMessage(WM_BANK_CLOSE,0,0);
			}

			if (m_pBankWndEx)
			{
				m_pBankWndEx->Close();
				PostMessage(WM_BANK_CLOSE,0,0);
			}

			if (m_pBankWnd)
			{
				m_pBankWnd->Close();
				PostMessage(WM_BANK_CLOSE,0,0);
			}

			CMainRoomEx::m_bBankLogoned = false;

			PostMessage(WM_COMMAND,IDM_LOGON_PLACE,0);
		}

		//设置数据
		memset(m_Mission,0,sizeof(m_Mission));
		AddMission(MS_LOGON,0,0);
	}

	//锁定机器相关代码 zxj 2009-11-13
	if( nIDEvent == IDT_CHANGE_LOCK )
	{
		KillTimer(IDT_CHANGE_LOCK);
		m_TopWnd->SetLockWndState(TRUE);	//令锁定机器按钮有效
		m_bCanClick = TRUE;					//此信号重新有效
	}
	CBaseRoom::OnTimer(nIDEvent);
}

//版本检测
bool CMainRoomEx::GamePlaceVerCheck(MSG_S_ConnectSuccess * pVerInfo, bool & bNewVer)
{
	//版本低
	bNewVer=GAME_PLACE_MAX_VER<pVerInfo->bMaxVer;
	return GAME_PLACE_MAX_VER>=pVerInfo->bLessVer;
}

////修改资料
//void CMainRoomEx::OnChangeInfo()
//{
//	ShellExecute(NULL,TEXT("open"),TEXT("http://www.EYUL.COM/?修改资料"),NULL,NULL,SW_MAXIMIZE);	
//	return;
//}

//退出游戏
void CMainRoomEx::OnQuitPlace()
{
	AfxGetMainWnd()->PostMessage(WM_CLOSE,0,0);
	return;
}

//进入游戏房间
void CMainRoomEx::OnInGame()
{
	if (m_bLogon==true)
	{
		HTREEITEM Item=m_pGameListWnd->m_GameList.GetSelectedItem();
		if (Item!=NULL)
		{
			CAFCGameListItem * pItem=(CAFCGameListItem *)m_pGameListWnd->m_GameList.GetItemData(Item);
			if ((pItem!=NULL)&&(pItem->m_uDataType==GLK_GAME_ROOM))
			{
				OnCentranceGameRoom((CAFCRoomItem *)pItem);
				return;
			}
		}

		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
		CString str;
		CString strTitle;
		str = fMsg.GetKeyVal("MainRoom","SelectGameRoom","请先选择游戏房间");
		strTitle = fMsg.GetKeyVal("MainRoom","MessageTitle","系统消息");
		
		//AFCMessageBox(str,strTitle);//,MB_ICONINFORMATION,this);
        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,strTitle,str);
	}
	return;
}
//网页按钮
void CMainRoomEx::OnWeb()
{
	CString strBuffer;
	//if (strBuffer.IsEmpty()==false) IE(strBuffer);

	return;
}

//显示窗口
void CMainRoomEx::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CBaseRoom::OnShowWindow(bShow, nStatus);

	//设置提示
//	if (bShow==TRUE) ((CGamePlaceDlg *)AfxGetMainWnd())->SetWindowTitle(m_strTitle);

	return;
}
//关闭所有房间
bool CMainRoomEx::CloseAllGameRoom()
{
	for (UINT i=0;i<MAX_GAME_ROOM;i++)
	{
		if ((m_RoomInfo[i].pGameRoomWnd!=NULL)&&(m_RoomInfo[i].pGameRoomWnd->CanCloseNow()==false)) 
		{
			CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
			CString str;
			CString strTitle;
			str = fMsg.GetKeyVal("MainRoom","QuitGameBeforeClosing","请退出各游戏后，再关闭整个系统！");
			strTitle = fMsg.GetKeyVal("MainRoom","MessageTitle","系统消息");
			
			//AFCMessageBox(str,strTitle);//,MB_ICONQUESTION,this);
            DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,strTitle,str);
			return false;
		}
		else		//玩家进入游戏窗口中未开始游戏 则关闭游戏add by xqm 2010-11-12
		{
			if((m_RoomInfo[i].pGameRoomWnd!=NULL) && (m_RoomInfo[i].pGameRoomWnd->CloseGameRoom()));
		}
	}

#ifdef MY_QUICK_GAME_ON

	//JianGK,关闭管道
	CloseNamedPipe();

#endif // MY_QUICK_GAME_ON
	return true;
}

//重画函数
void CMainRoomEx::OnPaint()
{
	CPaintDC dc(this);
	CRect ClientRect;
	GetClientRect(&ClientRect);
	int iWidth = ClientRect.Width();
	int iHeight = ClientRect.Height();
	CRgn rgn1, rgn2;
	rgn1.CreateRectRgn(0, 0, iWidth, m_maintc.GetHeight());
	rgn2.CreateRectRgn(0, iHeight-m_mainbc.GetHeight(), iWidth, iHeight);
	rgn1.CombineRgn(&rgn1, &rgn2, RGN_OR);
	rgn2.SetRectRgn(0, 0, m_mainlc.GetWidth(), iHeight);
	rgn1.CombineRgn(&rgn1, &rgn2, RGN_OR);
	rgn2.SetRectRgn(iWidth-m_mainrc.GetWidth(), 0, iWidth, iHeight);
	rgn1.CombineRgn(&rgn1, &rgn2, RGN_OR);
	dc.SelectClipRgn(&rgn1);
	//绘画框架
	DrawViewFrame(&dc,ClientRect.Width(),ClientRect.Height());
	rgn2.DeleteObject();
	rgn1.DeleteObject();

	return;
}


//菜单命令
BOOL CMainRoomEx::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case IDM_SKIN:
	case IDM_SKIN+1:
	case IDM_SKIN+2:
	case IDM_SKIN+3:
	case IDM_SKIN+4:				//设置聊天对象
		{
			int basesk=IDM_SKIN;
			int sk=LOWORD(wParam)-basesk;
			CNewGameClass * pRoomTab=&((CGamePlaceDlg *)AfxGetMainWnd())->m_RoomTabWnd;
			pRoomTab->PostMessage(WM_LOADSKIN,(WPARAM)sk,0);
		}
		return true;
	case IDM_DOWN_NEW:				//保存聊天信息
		{
			CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
			CString str;

			CAFCGameListItem * pGameItem;
			HTREEITEM hItem=m_pGameListWnd->m_GameList.GetSelectedItem();
			if (hItem!=NULL)
			{
				pGameItem=(CAFCGameListItem *)m_pGameListWnd->m_GameList.GetItemData(hItem);
				if ((pGameItem==NULL)||(pGameItem->m_uDataType!=GLK_GAME_ROOM)) 
					return true;//游戏房间,,创建新房
			}
			CAFCRoomItem * pGameRoomItem=(CAFCRoomItem *)pGameItem;
			//判断游戏是否安装并效验版本
			CFileVersion FileVer;
			TCHAR szProessName[31],szGameName[61];
			m_pGameListWnd->m_pLeftWnd->GetGameName(pGameRoomItem,szGameName,sizeof(szGameName));
			m_pGameListWnd->m_pLeftWnd->GetProcessName(pGameRoomItem,szProessName,sizeof(szProessName));
			CString strMessage;
			str = fMsg.GetKeyVal("MainRoom","DownloadingDontEnter","即将下载 %s游戏，更新过程中请勿进入该游戏! ");
			strMessage.Format(str,szGameName);
			
			if (CheckUpdate(szProessName, pGameRoomItem))
			{
				//wushuqun 2009.5.21
				//if(IDCANCEL==AFCMessageBox(TEXT(strMessage),MESSAGE_TITLE,AFC_YESNO))//,MB_ICONQUESTION|MB_YESNO,this)!=IDYES)
				if(IDCANCEL==DUIMessageBox(m_hWnd,MB_YESNO|MB_ICONINFORMATION,MESSAGE_TITLE,strMessage)) 
					return true;
				try
				{     
					StartDownLoad(szProessName,pGameRoomItem);
				}
				catch(CException* e)
				{
					str = fMsg.GetKeyVal("MainRoom","DownloadError","下载发生了异常！");
					
					//AFCMessageBox(str);
                    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",str);
					e->Delete();
					return false;
				}
				catch(...)
				{
					str = fMsg.GetKeyVal("MainRoom","DownloadError","下载发生了异常！");
					
					//AFCMessageBox(str);
                    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",str);
					return false;
				}
			}


			return true;
		}
	default:
		break;
	}
	return __super::OnCommand(wParam, lParam);
}

void CMainRoomEx::LoadSkin(const CString& strLogonUserID)
{
	int r,g,b;
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CString strSkinBcfFileName = m_skinmgr.GetSkinBcfFileName(strLogonUserID);
	CBcfFile f( s + strSkinBcfFileName);
	CString key=TEXT("MainRoom");
	TCHAR path[MAX_PATH];
	CString skinfolder;
	if ("skin0.bcf" == strSkinBcfFileName)
		skinfolder=f.GetKeyVal("skin0","skinfolder",m_skinmgr.GetSkinPath(strLogonUserID));
	else
		skinfolder=f.GetKeyVal("skin1","skinfolder",m_skinmgr.GetSkinPath(strLogonUserID));

	m_adx=f.GetKeyVal(key,"adx",15);
	m_ady=f.GetKeyVal(key,"ady",10);
	m_adheight=f.GetKeyVal(key,"adwidth",63);

	m_skx=f.GetKeyVal(key,"hskx",30);
	m_sky=f.GetKeyVal(key,"hsky",10);

	m_utitle=f.GetKeyVal(key,"halltitle","%s -- %s");
	r=f.GetKeyVal(key,"halltitler",0);
	g=f.GetKeyVal(key,"halltitleg",0);
	b=f.GetKeyVal(key,"halltitleb",0);
	m_titlecolor=RGB(r,g,b);


	//房间和游戏里面的列表imagelist
	r=f.GetKeyVal(key,"listbkclr",0);
	g=f.GetKeyVal(key,"listbkclg",0);
	b=f.GetKeyVal(key,"listbkclb",0);	
	//ChangeImageList(RGB(r,g,b));


	//上方按钮位置
	m_mainbtnX = f.GetKeyVal(key,"mainbtnx",10);
	m_mainbtnSpace = f.GetKeyVal(key,"mainbtnspace",20);


	//导航条
	wsprintf(path,"%smain\\main_btn01.bmp",skinfolder);
	//	m_button.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	m_Bt1.LoadButtonBitmap(path,false);
	wsprintf(path,"%smain\\main_btn02.bmp",skinfolder);
	m_Bt2.LoadButtonBitmap(path,false);
	wsprintf(path,"%smain\\main_btn03.bmp",skinfolder);
	m_Bt3.LoadButtonBitmap(path,false);
	wsprintf(path,"%smain\\main_btn04.bmp",skinfolder);
	m_Bt4.LoadButtonBitmap(path,false);
	//wsprintf(path,"%smain\\main_btn05.bmp",skinfolder);
	//m_Bt5.LoadButtonBitmap(path,false);
	//wsprintf(path,"%smain\\main_btn06.bmp",skinfolder);
	//m_Bt6.LoadButtonBitmap(path,false);
	//wsprintf(path,"%smain\\main_btn07.bmp",skinfolder);
	//m_Bt7.LoadButtonBitmap(path,false);
	//wsprintf(path,"%smain\\main_btn08.bmp",skinfolder);
	//m_Bt8.LoadButtonBitmap(path,false);
	//wsprintf(path,"%smain\\main_btn09.bmp",skinfolder);
	//m_Bt9.LoadButtonBitmap(path,false);
	//wsprintf(path,"%smain\\main_btn10.bmp",skinfolder);
	//m_Bt10.LoadButtonBitmap(path,false);	//wsprintf(path,"%shall\\5.bmp",skinfolder);
	//m_Bt5.LoadButtonBitmap(path,false);
	//wsprintf(path,"%shall\\6.bmp",skinfolder);
	//m_Bt6.LoadButtonBitmap(path,false);
	//wsprintf(path,"%shall\\7.bmp",skinfolder);
	//m_Bt7.LoadButtonBitmap(path,false);

	m_Bt5.ShowWindow(SW_HIDE);
	m_Bt6.ShowWindow(SW_HIDE);
	m_Bt7.ShowWindow(SW_HIDE);
	m_Bt8.ShowWindow(SW_HIDE);
	m_Bt9.ShowWindow(SW_HIDE);
	m_Bt10.ShowWindow(SW_HIDE);


	//主框架
	wsprintf(path,"%smain\\main_lc.bmp",skinfolder);
	m_mainlc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	wsprintf(path,"%smain\\main_rc.bmp",skinfolder);
	m_mainrc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	wsprintf(path,"%smain\\main_tc.bmp",skinfolder);
	m_maintc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	wsprintf(path,"%smain\\main_bc.bmp",skinfolder);
	m_mainbc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	//上框架四角
	wsprintf(path,"%smain\\main_lt.bmp",skinfolder);
	m_mainlt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	wsprintf(path,"%smain\\main_rt.bmp",skinfolder);
	m_mainrt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	wsprintf(path,"%smain\\main_lb.bmp",skinfolder);
	m_mainlb.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	wsprintf(path,"%smain\\main_rb.bmp",skinfolder);
	m_mainrb.SetLoadInfo(path,CGameImageLink::m_bAutoLock);

	
	//WCHAR path1[MAX_PATH];
	wchar_t wpath[MAX_PATH];
	
	
	
	
	
#ifndef BZ_ZLIB  //< add by wxx 
	img=Image::FromFile(wpath);
#else


	TCHAR szPath[MAX_PATH];
	sprintf(szPath,m_skinmgr.GetSkinPath(strLogonUserID) + "signal.bmp");
	CUnZipRes unZipRes;
	IStream * pStream=NULL;
	pStream=unZipRes.LoadFileResFromZip(szPath);

	img=new Image(pStream);
	if(pStream!=NULL)
	{
		pStream->Release();
		pStream=NULL;
	}

#endif

    //等待区底图 yjj 090227
}


LRESULT CMainRoomEx::OnLoadSkin(WPARAM wparam, LPARAM lparam)
{
	LoadSkin();
	m_pGameListWnd->m_GameList.UpdateAllIcon();
	CRect ClientRect;
	GetClientRect(&ClientRect);
	OnSizeEvent(0,ClientRect.Width(),ClientRect.Height());
	return true;
}
LRESULT CMainRoomEx::OnExchangeSkin(WPARAM wparam, LPARAM lparam)
{
	m_strTitle=Glb().m_title;//从全局变量中加载窗口标题
	LoadSkin();

	m_pGameListWnd->m_GameList.UpdateAllIcon();
	m_pGameListWnd->m_GameList.Invalidate();

	SetWindowText(m_strTitle);
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + strSkin);
    TCHAR path[MAX_PATH] = {0};
	CString skinfolder = f.GetKeyVal(m_skinmgr.GetKeyVal(strSkin),"skinfolder",m_skinmgr.GetSkinPath());
	wsprintf(path,"%sDialog\\scroll.bmp",skinfolder);
	HBITMAP hbmScorll = (HBITMAP)LoadImage(NULL, path, IMAGE_BITMAP, 0,0, LR_LOADFROMFILE);
	SkinSB_Uninit(m_WebView.GetSafeHwnd());
	SkinSB_Init(m_WebView.GetSafeHwnd(), hbmScorll);

	Invalidate();
	return LRESULT(0);
}

bool CMainRoomEx::GetRoomInfoStruct(RoomInfoStruct** pRoomInfoStruct)
{
	if(m_RoomInfo!=NULL) 
	{
		*pRoomInfoStruct=m_RoomInfo;
		return true;
	}
	return false;
}

bool CMainRoomEx::CheckUpdate(TCHAR filename[],CAFCRoomItem * pGameRoomItem)
{
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	TCHAR szProessName[31],szGameName[61];
	m_pGameListWnd->m_pLeftWnd->GetGameName(pGameRoomItem,szGameName,sizeof(szGameName));
	m_pGameListWnd->m_pLeftWnd->GetProcessName(pGameRoomItem,szProessName,sizeof(szProessName));
	ComRoomInfo * pComRoomInfo=&pGameRoomItem->m_RoomInfo;

	//启动update进行更新
	TCHAR szApp[61];//,sz[100];
	TCHAR path[MAX_PATH];
	lstrcpy(szApp,szProessName);
	szApp[lstrlen(szApp)-lstrlen("ico")-1]='\0';
	
	wsprintf(path,"%s\\%s\\update.ini",GetAppPath().c_str(),szApp);
	//CINIFile f(path);
	CString m_key="config";
	TCHAR v[128];
	ZeroMemory(v, sizeof(v));

	//v=f.GetKeyVal(m_key,"-v","0");//版本

	GetPrivateProfileString("config", "-v", "", v, 128, path);
//	DebugPrintf()
	TCHAR ser[64];
	ZeroMemory(ser, sizeof(ser));
	wsprintf(ser,"%u.%u",pComRoomInfo->uVer>>16,pComRoomInfo->uVer&0x0000ffff);

	if (_tcscmp(ser, v) != 0)
		return true;
	else
		return false;
}

//加载下载信息
bool CMainRoomEx::StartDownLoad(TCHAR filename[],CAFCRoomItem * pGameRoomItem)
{

	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	TCHAR szProessName[31],szGameName[61];
	m_pGameListWnd->m_pLeftWnd->GetGameName(pGameRoomItem,szGameName,sizeof(szGameName));
	m_pGameListWnd->m_pLeftWnd->GetProcessName(pGameRoomItem,szProessName,sizeof(szProessName));

	//启动update进行更新
	TCHAR szApp[61];//,sz[100];
	TCHAR path[MAX_PATH];
	lstrcpy(szApp,szProessName);
	szApp[lstrlen(szApp)-lstrlen("ico")-1]='\0';	
	wsprintf(path,"%s\\%s\\update.ini",GetAppPath().c_str(),szApp);
	CINIFile f(path);
	CString m_key="config";
	CString v,s,ul,uv,n;
	int sm;
	CString str;	
	//版本号有默认值，不然会引起update.exe 出错
	ComRoomInfo * pComRoomInfo=&pGameRoomItem->m_RoomInfo;

	v=f.GetKeyVal(m_key,"-v","0");//版本	
	s=f.GetKeyVal(m_key,"-s","./");//路径
	ul=f.GetKeyVal(m_key,"-ul","");//更新列表地址	
	uv=f.GetKeyVal(m_key,"-uv","");//更新版本地址
	n=f.GetKeyVal(m_key,"-n","游戏更新");//游戏名称	
	sm=f.GetKeyVal(m_key,"-sm",1);//显示方式

	if (ul =="")
	{
		ul.Format("%s/update_version.txt",szApp);
		uv = ul;
	}

	bool getver=false;
	TCHAR ser[20];
	wsprintf(ser,"%u.%u",pComRoomInfo->uVer>>16,pComRoomInfo->uVer&0x0000ffff);
	{
		try
		{
			::EnterCriticalSection(&m_Section);
			if(m_UpdateList.size()>=64)
			{
				::LeaveCriticalSection(&m_Section);
				str = fMsg.GetKeyVal("MainRoom","TooManyUpdateTryLater","正在更新的游戏太多,请后再试.");	
                DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,szGameName,str);
				return false;
			}
			else
			{
				::LeaveCriticalSection(&m_Section);
			}
		}
		catch(...)
		{
			::LeaveCriticalSection(&m_Section);
		}
		STARTUPINFO StartupInfo;
		ZeroMemory(&StartupInfo, sizeof(STARTUPINFO));
		StartupInfo.cb = sizeof(STARTUPINFO);
		PROCESS_INFORMATION ProcessInfo;
		char up[1000];
		//wsprintf(up," -n %s -s %s -v %s -ul %s%s -uv %s%s -sm %d",n,s,v,Glb().m_UpdateUrl,ul,Glb().m_UpdateUrl,uv,sm);
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind;
		bool setup;

		setup=true;
		hFind = FindFirstFile(path, &FindFileData);
		if (hFind == INVALID_HANDLE_VALUE) 
		{

		} 
		else 
		{
			setup=false;
			FindClose(hFind);
		}		
		//还原成原有更新方式		
		string _path;
		_path = GetAppPath() + "\\" + szApp;
		sprintf(up, "-n %s -s %s\\ -v %s -ul %s%s -uv %s%s -sm %d", n, _path.c_str(),v, Glb().m_UpdateUrl, ul,Glb().m_UpdateUrl,uv, 1);
				

		//异步启动		
		sDownLoadUpdateMsg sUpdateMsg; 
		//-----------------
		UpdateStructMsg updateMsg;
		memset(&updateMsg,0,sizeof(UpdateStructMsg));
		updateMsg.command = 1;
		updateMsg.nRoomID = pGameRoomItem->m_RoomInfo.uNameID;
		wsprintf(updateMsg.g_GameName,"%s",szGameName);
		wsprintf(updateMsg.g_url,"%s%s",Glb().m_UpdateUrl,uv);
		wsprintf(updateMsg.g_ver,"%s",v); 
		sUpdateMsg.pUpdateStructMsg = &updateMsg;
		//-----------------
		UPDATE *lp=(LPUPDATE)malloc(sizeof(UPDATE));
		lp->hProcess=ProcessInfo.hProcess;
		lp->uNameID=pGameRoomItem->m_RoomInfo.uNameID;
		lp->pGameRoomItem=pGameRoomItem;
		sUpdateMsg.pUPDATE = lp;
		//-----------------
		sUpdateMsg.pCMainRoomEx = this;
		//-----------------
		sUpdateMsg.pCBcfFile = &fMsg;
		//-----------------

		DWORD dwThreadID = 0;
		HANDLE hThread = CreateThread(0,0,DownLoadUpdate,&sUpdateMsg,0,&dwThreadID);
		int nStartTime = fMsg.GetKeyVal("MainRoom","DownloadStartTime",10000);

		//连接update.exe超时
		if (WaitForSingleObject(hThread,nStartTime)==WAIT_TIMEOUT) 
		{
			if(m_CSocket32.GetProcessInfo().hProcess)
			{
				::TerminateProcess( m_CSocket32.GetProcessInfo().hProcess, 0 );
			}
			m_CSocket32.Close();

			CString str;
			CString strTitle;
			str = fMsg.GetKeyVal("MainRoom","DownLoadOver","下载游戏更新超时");
			strTitle = fMsg.GetKeyVal("MainRoom","MessageTitle","提示");			
            DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,strTitle,str);
		}
		if (hThread)
		{
			::CloseHandle(hThread);
		}		

		return false;
	}

	return true;
}

#ifdef AI_MACHINE
bool CMainRoomEx::EnterRoom(int roomid)
{
	CAFCRoomItem *pRoomItem =m_pGameListWnd->m_GameList.FindRoomItem(m_Airoom);
	if(!pRoomItem)
	{
	//	TCHAR sz[100];
	//	wsprintf(sz, _T("找不到房间 %d"), m_Airoom);
		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
		CString str;
		str = fMsg.GetKeyVal("MainRoom","NoRoom","找不 到房间 %d");
		
		//AFCMessageBox(str);
        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",str);
		return false;
	}
	return OnCentranceGameRoom(pRoomItem);
}
#endif

//更新本人信息
BOOL CMainRoomEx::UpdateMeInfo(__int64 i64Money,BOOL IsUpdateMoney,int dwFascination,BOOL IsUpDateFascination)
{
	if(IsUpdateMoney)
		m_PlaceUserInfo.i64Money = i64Money ;

	if(IsUpDateFascination)
		m_PlaceUserInfo.dwFascination = dwFascination;

	/// 只有显示列表时才刷新，其它时候不刷新
	CRect rc;
	m_pGameListWnd->GetClientRect(&rc);
	if (rc.Width()>100)
	{
		m_pGameListWnd->Invalidate(FALSE);
	}
	return TRUE;
}

//更新所有GameFrame的银行金币数信息
//用于玩家打开多个游戏银行，其中一个银行有变化，把变化同步到其它房间或银行窗口
//JianGuankun 2012.1.5
void CMainRoomEx::UpdateBankMoneyToAllRoom(__int64 i64Money,CBaseRoom* pLaunchRoom/* = NULL*/)
{
	//更新房间用户////
	CM_UserState_Change ControlMessage;
	ControlMessage.bActionCode = ACT_USER_BANKMONEY;

	UserItemStruct* pUis = new UserItemStruct;
	::memset(pUis,NULL,sizeof(UserItemStruct));
	pUis->GameUserInfo.dwUserID = m_PlaceUserInfo.dwUserID;
	pUis->GameUserInfo.i64Bank = i64Money;
	ControlMessage.pUserItem = pUis;

	CGameRoomEx* p = (CGameRoomEx*)pLaunchRoom;
	/*CString str;
	str.Format("DClient:GameID = %d,Send bank money to all room.Money = %I64d",p->m_GameInfo.uNameID,i64Money);
	OutputDebugString(str);*/

	//游戏房间的银行
	for (BYTE i = 0; i < MAX_GAME_ROOM; ++i)
	{
		// 游戏中游戏界面右边消息栏
		CGameRoomEx* pGameRoomEx = (CGameRoomEx*)m_RoomInfo[i].pGameRoomWnd;
		if (pLaunchRoom == pGameRoomEx) //发起消息的房间无需再更新金币数
		{
			continue;
		}
		if (NULL != pGameRoomEx)
		{
			//更新游戏金币数据
			if (pGameRoomEx->m_IGameFrame)
			{
				ControlMessage.bDeskStation = pGameRoomEx->m_GameInfo.pMeUserInfo->GameUserInfo.bDeskStation;
				pGameRoomEx->m_IGameFrame->OnControlMessage(CM_USER_STATE,&ControlMessage,sizeof(CM_UserState_Change));
			}

			//更新大厅银金币数据
			pGameRoomEx->FlushMyBankMoney(i64Money);

			//更新大厅打开的银行金币数据
			if (pGameRoomEx->m_pBankWndEx && pLaunchRoom)
			{
				CGameRoomEx* pLRoom = (CGameRoomEx*)pLaunchRoom;
				pGameRoomEx->m_pBankWndEx->SetGameWalletMoney(pLRoom->m_GameInfo.uNameID,pLRoom->m_GameInfo.pMeUserInfo->GameUserInfo.i64Money);
				pGameRoomEx->m_pBankWndEx->SetWealthInfor(i64Money,0);
			}
		}	
	}

	delete pUis;

	return;
}

//
void CMainRoomEx::ChangeImageList(COLORREF color)
{
	//CImageList *m_UserStateList=&CHappyGoDlg::m_UserStateList;

	//m_UserStateList->DeleteImageList();
	////装载位图
	//BITMAP BmpInfo;
	//CGameImage	UserStateBmp;
	//CBitmap NewUserStateBmp;
	//AfxSetResourceHandle(GetModuleHandle(FACE_DLL_NAME));
	//UserStateBmp.SetLoadInfo(".\\image\\user_state.bmp",true);;
	//AfxSetResourceHandle(GetModuleHandle(NULL));
	////UserStateBmp.GetBitmap(&BmpInfo);
	//BmpInfo.bmWidth=UserStateBmp.GetWidth();
	//BmpInfo.bmHeight=UserStateBmp.GetHeight();

	////建立位图
	//CDC TargetDC, * pDC=GetDC();
	//TargetDC.CreateCompatibleDC(pDC);
	//NewUserStateBmp.CreateCompatibleBitmap(pDC,BmpInfo.bmWidth,BmpInfo.bmHeight);
	//CBitmap * pOldBmp=TargetDC.SelectObject(&NewUserStateBmp);
	//TargetDC.FillSolidRect(0,0,BmpInfo.bmWidth,BmpInfo.bmHeight,color);
	//AFCStretchImage(&TargetDC,0,0,BmpInfo.bmWidth,BmpInfo.bmHeight,UserStateBmp,0,0,BmpInfo.bmWidth,BmpInfo.bmHeight,RGB(255,255,255));
	//TargetDC.SelectObject(pOldBmp);
	//TargetDC.DeleteDC();

	////建立资源列
	//m_UserStateList->Create(USER_STATE_WIDTH,USER_STATE_HEIGHT,ILC_COLOR16,0,0);
	//m_UserStateList->Add(&NewUserStateBmp,RGB(0,0,255));

	////清理资源
	//UserStateBmp.Detach();
	//NewUserStateBmp.Detach();
	//ReleaseDC(pDC);
}

//主房间功能按钮
//
void CMainRoomEx::OnBnClickedBt1()
{
	if (m_WebView.GetSafeHwnd()!=NULL)
		m_WebView.GoBack();
}
//
void CMainRoomEx::OnBnClickedBt2()
{
	if (m_WebView.GetSafeHwnd()!=NULL)
		m_WebView.GoForward();
}
//
void CMainRoomEx::OnBnClickedBt3()
{
	if (m_WebView.GetSafeHwnd()!=NULL)
		m_WebView.Refresh();
}
//
void CMainRoomEx::OnBnClickedBt4()
{
	if (m_WebView.GetSafeHwnd()!=NULL)
		m_WebView.Stop();
}
//切换帐号
void CMainRoomEx::OnBnClickedBt5()
{
	//切换帐号前需要清空当前用户好友列表信息 --切换帐号后保留了原先帐号的好友列表信息bug修改 --20150516 wzd
	m_pGameListWnd->m_IMList.clearUserList();
	
	for(int i = 0; i < MAX_GAME_ROOM; i ++)
	{
		if(m_RoomInfo[i].bAccess)
		{
			CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
			CString str;
			CString strTitle;
			str = fMsg.GetKeyVal("MainRoom","CloseAllRooms","请先关闭所有房间！");
			strTitle = fMsg.GetKeyVal("MainRoom","LoginAgain","重新登陆");
			
			//AFCMessageBox(str,strTitle);//,MB_ICONQUESTION,this);
            DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,strTitle,str);
			return ;
		}
	}
	
	_TAG_USERPROP *userProp=NULL;
	int propCount=Glb().userPropLibrary.GetCount();
	for(int i=0;i<propCount;i++)
	{
		userProp=Glb().userPropLibrary.GetAt(0);
		Glb().userPropLibrary.RemoveAt(0);
		delete userProp;
	}
	if(propPannel)
		propPannel->ShowWindow(SW_HIDE);

	if(m_pGameListWnd && m_pGameListWnd->personalSet)
		m_pGameListWnd->personalSet->ShowWindow(SW_HIDE);


	//if(AFCMessageBox(TEXT("点击“确定”将重新切换到登陆框，确定吗？"),TEXT("重新登陆"),MB_ICONQUESTION|MB_OKCANCEL,this)==IDOK)
	CloseSocket(true);        
	//if(imMain)
	//{
	//	imMain->clearAllSession();
	//	imMain->closeIMDialog();
	//}

	if (m_pBankVerifyWnd)
	{
		m_pBankVerifyWnd->Close();
		PostMessage(WM_BANK_CLOSE,0,0);
	}

	if (m_pBankWndEx)
	{
		m_pBankWndEx->Close();
		PostMessage(WM_BANK_CLOSE,0,0);
	}

	if (m_pBankWnd)
	{
		m_pBankWnd->Close();
		PostMessage(WM_BANK_CLOSE,0,0);
	}

	CMainRoomEx::m_bBankLogoned = false;

/*	if (m_pGameListWnd->m_pLeftWnd)
	{
		m_pGameListWnd->m_pLeftWnd->Close();
		delete m_pGameListWnd->m_pLeftWnd;
	}*/

	PostMessage(WM_COMMAND,IDM_LOGON_PLACE,0);
	return;
}
//
void CMainRoomEx::OnBnClickedBt6()
{
	;
}
//
void CMainRoomEx::OnBnClickedBt7()
{
	;
}
//
void CMainRoomEx::OnBnClickedBt8()
{
	MessageBox("CMainRoomEx::OnBnClickedBt8()");
}
//
void CMainRoomEx::OnBnClickedBt9()
{
	;
}
//
void CMainRoomEx::OnBnClickedBt10()
{
	;
}

bool CMainRoomEx::IsGameNeedUpdate(ComNameInfo *pNameInfo)
{
	if (!pNameInfo)
	{
		return true;
	}

	CString strFile;
	CString strImg = GetAppPath().c_str();
	strFile.Format(_T("%s\\%d\\update.ini"), strImg, pNameInfo->uNameID);

	//获取本地版本号
	TCHAR strVersion[MAX_PATH];
	ZeroMemory(strVersion, MAX_PATH);
	GetPrivateProfileString("config", "-v", "", strVersion, MAX_PATH, strFile);

	//获取服务端版本号
	TCHAR strCurVersion[MAX_PATH];
	ZeroMemory(strCurVersion, MAX_PATH);
	wsprintf(strCurVersion,"%u.%u",pNameInfo->uVer>>16, pNameInfo->uVer&0x0000ffff);

	if (_tcscmp(strCurVersion, strVersion) != 0)
	{
		return true;
	}
	return false;
}

bool CMainRoomEx::IsGameNeedUpdate(ComRoomInfo *pRoomInfo, ComNameInfo* &pNameInfo)
{
	pNameInfo = NULL;
	if (!pRoomInfo)
	{
		return true;
	}
	Node* pNode = m_pGameListWnd->m_pLeftWnd->m_mapNameNode[pRoomInfo->uNameID];
	
	pNameInfo = (ComNameInfo*)pNode->data().GetData();
	
	if (pNameInfo)
	{
		return IsGameNeedUpdate(pNameInfo);
	}
	
	return false;
}

void CMainRoomEx::DownloadGame(ComNameInfo* pNameInfo)
{
	if (!pNameInfo)
	{
		return;
	}

	if (!m_pDownloadWnd || !::IsWindow(*m_pDownloadWnd))
	{
		m_pDownloadWnd = new CDownloadWnd;
		m_pDownloadWnd->Create(GetParent()->GetSafeHwnd(),NULL,UI_WNDSTYLE_DIALOG,0,0,0,0,0,NULL);
	}
	m_pDownloadWnd->AddDownloadEx(pNameInfo);
	m_pDownloadWnd->CenterWindow();
	m_pDownloadWnd->ShowWindow();
}

void CMainRoomEx::DownloadGameEx(ComRoomInfo *pRoomInfo)
{
	CAFCNameItem* pNameItem = NULL;
	if (!pRoomInfo)
	{
		return;
	}
	pNameItem = m_pGameListWnd->m_GameList.FindNameItem(pRoomInfo->uNameID, pRoomInfo->uKindID);
	if (pNameItem)
	{
		if (!m_pDownloadWnd || !::IsWindow(*m_pDownloadWnd))
		{
			m_pDownloadWnd = new CDownloadWnd;
			m_pDownloadWnd->Create(GetParent()->GetSafeHwnd(),NULL,UI_WNDSTYLE_DIALOG,0,0,0,0,0,NULL);
		}
		m_pDownloadWnd->AddDownloadEx(&pNameItem->m_NameInfo);
		m_pDownloadWnd->CenterWindow();
		m_pDownloadWnd->ShowWindow();
	}
}

//显示还是隐藏左边
LRESULT CMainRoomEx::OnShowLeft(WPARAM wparam,LPARAM lparam)
{
	if((BOOL)wparam&&m_pGameListWnd->GetLeftShowOrHide())
	{
		m_TopWnd->m_bHide=true;
		m_TopWnd->FixControlStation();
		m_pGameListWnd->OnBnClickedBt1();
		return 0;
	}

	if(!(BOOL)wparam &&!(m_pGameListWnd->GetLeftShowOrHide()))
	{
		m_TopWnd->m_bHide=false;
		m_TopWnd->FixControlStation();
		m_pGameListWnd->OnBnClickedBt1();
	}

	return 0;
}

LRESULT CMainRoomEx::OnTopTabLostSelected(WPARAM wparam,LPARAM lparam)
{
	if (m_pBankVerifyWnd)
	{
		m_pBankVerifyWnd->Close();
		PostMessage(WM_BANK_CLOSE,0,0);
	}

	if (m_pBankWndEx)
	{
		m_pBankWndEx->Close();
		PostMessage(WM_BANK_CLOSE,0,0);
	}

	if (m_pBankWnd)
	{
		m_pBankWnd->Close();
		PostMessage(WM_BANK_CLOSE,0,0);
	}

	return 0;
}

LRESULT CMainRoomEx::OnTopTabSelected(WPARAM wparam,LPARAM lparam)
{
	//当点击MainRoom,时gpGameRoomEx调换到第一个GameRoom中，如果没有GameRoom被打开，则gpGameRoomEx为NULL
	//Added by JianGuankun 2012.2.14

	CMainRoomEx* pMainRoom = GetMainRoom();
	RoomInfoStruct* pRoomInfo = NULL;

	bool bHaveGameRoom = false;

	if(pMainRoom) 
	{
		if(pMainRoom->GetRoomInfoStruct(&pRoomInfo))
		{
			for (BYTE i = 0; i < MAX_GAME_ROOM; i++)
			{
				CGameRoomEx* pGameRoom = (CGameRoomEx*)pRoomInfo[i].pGameRoomWnd;
				if (pGameRoom)
				{
					if (IsWindow(pGameRoom->GetSafeHwnd()))
					{
						gpGameRoomEx = pGameRoom;
						bHaveGameRoom = true;
					}
				}
			}
		}
	}

	if (!bHaveGameRoom)
	{
		gpGameRoomEx = NULL;
	}

	return 0;
}
//链接
void CMainRoomEx::IE(LPCTSTR URL)
{
	if (m_WebView.GetSafeHwnd()!=NULL)
		m_WebView.Navigate(URL,NULL,NULL,NULL,NULL);
}

LRESULT CMainRoomEx::OnCallGM(WPARAM wparam, LPARAM lparam)
{
	SendData(MDM_GP_PLAYER_2_GM, ASS_GP_GET_GM_LIST, 0);
	return 0;
}

//打开IE
LRESULT CMainRoomEx::OnOpenIE(WPARAM wparam,LPARAM lparam)
{
	CString url;
	switch((int)wparam)
	{
	case 11://头像上传
		//OnUpLoadUserLogo(0,0);
		break;
	default:
		{
			CString s=CBcfFile::GetAppPath ();/////本地路径
			CBcfFile f( s + "bzgame.bcf");
			CString key="BZW";
			CString url;
			TCHAR szUrl[20];
			wsprintf(szUrl,"hallurl%d",(int)wparam);
			url=f.GetKeyVal(key,szUrl,Glb().m_CenterServerPara.m_strHomeADDR);
					
			CString loginURL;//JMod-用户登录页面 -20090506
			loginURL = f.GetKeyVal("BZW", "UserLoginURL", "userlogin.aspx"); 

			CString surl;
			surl.Format("%s/app/%s?userid=%d&token=%s&url=%s",
				Glb().m_CenterServerPara.m_strWebRootADDR,loginURL,m_PlaceUserInfo.dwUserID,Glb().m_TML_SN,url);
			IE(surl);
			
			for(int i = 0; i < MAX_GAME_ROOM; i ++)
				if(m_RoomInfo[i].pGameRoomWnd != NULL && m_RoomInfo[i].pGameRoomWnd->IsWindowVisible())
					m_RoomInfo[i].pGameRoomWnd->PostMessage(IDM_OPEN_IE,wparam,lparam);
			break;
		}
	}
	return 0;
}

LRESULT CMainRoomEx::OnOpenBank(WPARAM wParam, LPARAM lParam)
{
	for(int i = 0; i < MAX_GAME_ROOM; ++i)
	{
		if(m_RoomInfo[i].pGameRoomWnd != NULL)
		{
			//AFCMessageBox("请到游戏房间中使用银行。");
            DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","请到游戏房间中使用银行。");
			return 0;
		}
	}

	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	/// 显示提示对话框后，直接发送打开银行消息，不需要二级密码
	if (m_Connect.GetSafeHwnd()==NULL)
		m_Connect.Create(IDD_CONNECT,this);
	m_Connect.SetMessageText(fMsg.GetKeyVal("GameRoom","OpenningBankWait","正在打开钱柜,请稍候..."));
	m_Connect.GetDlgItem(IDC_QUIT)->EnableWindow(false);
	m_Connect.ShowWindow(SW_SHOW);
	SendData(MDM_GP_BANK,ASS_GP_BANK_OPEN,0);

	return 0;
}

LRESULT CMainRoomEx::OnAppeal(WPARAM wparam,LPARAM lparam)
{
	if(!m_pAppealDlg)
	{
		m_pAppealDlg=new CAppealDlg(this);
		m_pAppealDlg->Create(IDD_DLG_APPEAL,this);
	}
	m_pAppealDlg->OpenAppealDlg(m_PlaceUserInfo.szName);
	return 0;

}
LRESULT CMainRoomEx::OnLockAccount(WPARAM wParam, LPARAM lParam)
{
	//MessageBox("在此触发锁定机器操作");
	OnSwitchLockButton();
	return 0;
}
//上传头像
LRESULT CMainRoomEx::OnUpLoadUserLogo(WPARAM wparam,LPARAM lparam)
{
	return true;
	if(!logoUpload)
	{
		logoUpload=new CLogoUpload;
		logoUpload->Create(IDD_LOGOUPLOAD,this);
		logoUpload->SetParentWnd(this);
	}
	logoUpload->ShowWindow(SW_SHOW);
	logoUpload->CenterWindow();

	return true;
}

//检测玩家头像
void CMainRoomEx::checkUserLogo(long dwUserID)
{
	if(!logoUpload)
	{
		logoUpload=new CLogoUpload;
		logoUpload->Create(IDD_LOGOUPLOAD,this);
		logoUpload->SetParentWnd(this);
	}
	logoUpload->checkUserLogo(dwUserID);
}

void CMainRoomEx::OpenBulletin(void)
{
	if(!hBulletin)
	{
		hBulletin=new CBulletin;
		hBulletin->Create(IDD_BULLETIN,this);
	}
	/*CString s=CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f( s + "bzgame.bcf");
	CString surl=f.GetKeyVal("BZW","LogonBulletin","about:blank");*/
	//hBulletin->OpenBulletin(surl);
	hBulletin->ShowWindow(SW_SHOW);
	hBulletin->CenterWindow();
}

void CMainRoomEx::propGetListRequest(void)
{
	MSG_PROP_C_GETSAVED msgPropGetSaved;
	msgPropGetSaved.dwUserID=m_PlaceUserInfo.dwUserID;
	SendData(&msgPropGetSaved,sizeof(MSG_PROP_C_GETSAVED),MDM_GP_PROP,ASS_PROP_GETUSERPROP,0);

}


/************************************************************************/
//Function:checkNetSignal()
//Parameter:none
//Author:Fred Huang 2008-06-27
//Note:向服务器发送网络检测消息
/************************************************************************/
void CMainRoomEx::checkNetSignal(void)
{
        if(m_PlaceUserInfo.dwUserID==0)        //没有ID号，不发
                return;
        //如果上次的消息都没有收到，则网格极差，但消息还是要发
        if(m_NetSignalIndex>m_SavedSingalIndex)
        {
                m_NetSignalLevel=0;
                
        }
		//drawSignal();
        //填充结构体
        MSG_GP_NETSignal netSignal;
        netSignal.dwIndex=++m_NetSignalIndex;
        netSignal.dwUserID=m_PlaceUserInfo.dwUserID;
        netSignal.dwTimeFlag=GetTickCount();
        //发送消息
        SendData(&netSignal,sizeof(MSG_GP_NETSignal),MDM_GP_NETSIGNAL,0,0);
}

/************************************************************************/
//Function:checkNetSignal()
//Parameter:
//                pNetData        :服务器反回的消息体
//Author:Fred Huang 2008-06-27
//Note:解析服务器返回的消息
/************************************************************************/
void CMainRoomEx::getNetSignal(void * pNetData)
{
        MSG_GP_NETSignal *netSignal=(MSG_GP_NETSignal*)pNetData;
        //如果不是发给自己的，丢弃
        if(m_PlaceUserInfo.dwUserID!=netSignal->dwUserID)
                return;
        //先将级别置0
        m_NetSignalLevel=0;
        //计算时差
        UINT uTimeDiff=GetTickCount()-netSignal->dwTimeFlag;
        //只有是当前的消息包，才计算级别
        if(netSignal->dwIndex==m_NetSignalIndex)
        {
                if(uTimeDiff<=50)
                        m_NetSignalLevel=1;
                else if(uTimeDiff>50 && uTimeDiff<=200)
                        m_NetSignalLevel=2;
                else if(uTimeDiff>200 && uTimeDiff<=500)
                        m_NetSignalLevel=3;
                else if(uTimeDiff>500 && uTimeDiff<=1000)
                        m_NetSignalLevel=4;
                else                 
                        m_NetSignalLevel=5;
			
        }
        //保存当前收到的序号
        m_SavedSingalIndex=netSignal->dwIndex;
        //画图
        //drawSignal();
		//Invalidate(true);
}

/************************************************************************/
//Function:drawSignal()
//Parameter:无
//Author:Fred Huang 2008-06-27
//Note:画图
/************************************************************************/
void CMainRoomEx::drawSignal(void)
{
        if(!img || img->GetFlags()==0)
                return;
		CString s=CBcfFile::GetAppPath();
		CBcfFile f( s + "skin0.bcf");
		CString key=TEXT("MainRoom");

		CRect rc;
		GetClientRect(rc);
     
		int x = rc.Width() - f.GetKeyVal(key, _T("game_hall_signalx"),10) - img->GetWidth();
		int y = f.GetKeyVal(key, _T("game_hall_signaly"),0);
		Graphics g(this->m_hWnd);
        g.DrawImage(img,Rect(x,y,img->GetWidth(),img->GetHeight()/6),0,m_NetSignalLevel*img->GetHeight()/6,img->GetWidth(),img->GetHeight()/6,UnitPixel);
}

// PengJiLin, 2010-9-13, 踢人卡、防踢卡功能，设置使用时间
void CMainRoomEx::SetKickPropTime(int iKickTime)
{
    m_iKickPropTotalTime = iKickTime;
}
void CMainRoomEx::SetAntiKickPropTime(int iAntiKickTime)
{
    m_iAntiKickPropTotalTime = iAntiKickTime;
}

// PengJiLin, 2011-6-23, 激活消息
void CMainRoomEx::SetActiveToMainRoom(UINT uState)
{
	return; //房间刷新问题已解决，没有激活的必要，JianGuankun 2012.5.11
    CGameRoomEx* pGameRoom = NULL;
    for (BYTE i=0;i<MAX_GAME_ROOM;i++)
    {
		if (m_RoomInfo[i].bAccess) //添加bAccess判断与非法pGameRoom指针判断，Add by JianGuankun 2012.2.15
		{
			try
			{
				pGameRoom = (CGameRoomEx*)m_RoomInfo[i].pGameRoomWnd;
				if(NULL != pGameRoom)
				{
					pGameRoom->SetActiveToGameRoom(uState);
				}
			}
			catch (...)
			{
				return;
			}
		}
    }
}

#ifdef MY_QUICK_GAME_ON

//JianGK,创建通讯管道
bool CMainRoomEx::OpenQuickGameNamedPipe(char* szPrefixName)
{
	if (m_hQGNamedPipe != INVALID_HANDLE_VALUE)
	{
		return false;
	}

	TCHAR szPipeName[32];

	for (int i = 0; i < 10; ++i)
	{
		::sprintf(szPipeName,"%s_%02d",szPrefixName,i);

		m_hQGNamedPipe = ::CreateNamedPipe(szPipeName,PIPE_ACCESS_DUPLEX|FILE_FLAG_WRITE_THROUGH,
			PIPE_WAIT|PIPE_TYPE_BYTE,1,128,128,NULL,NULL);

		if (m_hQGNamedPipe != INVALID_HANDLE_VALUE)
		{
			break;
		}
	}

	if (m_hQGNamedPipe != INVALID_HANDLE_VALUE)
	{
		DWORD dwThreadID = 0;
		//m_hNPThread = ::CreateThread(NULL,0,NamedPipeThread,this,NULL,&dwThreadID);
		CWinThread* pThread = AfxBeginThread(NamedPipeThread,this);
		m_hQGThread = pThread->m_hThread;
		if (m_hQGThread)
		{
			return true;
		}
	}
	else
	{
		CloseNamedPipe();
	}

	return false;
}

//JianGK,关闭通讯管道
bool CMainRoomEx::CloseNamedPipe()
{
	if (m_hQGNamedPipe != INVALID_HANDLE_VALUE)
	{
		if (m_hQGThread != INVALID_HANDLE_VALUE)
		{
			::TerminateThread(m_hQGThread,EXIT_FAILURE);
			::CloseHandle(m_hQGThread);

			::DisconnectNamedPipe(m_hQGNamedPipe);
			::CloseHandle(m_hQGNamedPipe);

			m_hQGNamedPipe = INVALID_HANDLE_VALUE;
			m_hQGThread = INVALID_HANDLE_VALUE;
		}
	}

	return true;
}

//JianGK,管道通讯线程
//DWORD WINAPI CMainRoomEx::NamedPipeThread(LPVOID param)
UINT AFX_CDECL CMainRoomEx::NamedPipeThread(LPVOID param)
{
	CMainRoomEx* pMainRoom = (CMainRoomEx*)param;

	while (true)
	{
		if (::ConnectNamedPipe(pMainRoom->m_hQGNamedPipe,NULL)) 
		{
			TQGInfo tagQG;
			DWORD dwReadNums = 0;

			::ReadFile(pMainRoom->m_hQGNamedPipe,&tagQG,sizeof(TQGInfo),&dwReadNums,NULL);

			pMainRoom->m_uQGRoomID = tagQG.uRoomID;
			
			::DisconnectNamedPipe(pMainRoom->m_hQGNamedPipe);

			CGameRoomEx* pGameRoom = (CGameRoomEx*)pMainRoom->m_RoomInfo[0].pGameRoomWnd;

			if (!pGameRoom)
			{
				CGameListCtrl* pGameList = &pMainRoom->m_pGameListWnd->m_GameList;
				
				CAFCNameItem* pNameItem = pGameList->FindNameItem(tagQG.uNameID);

				if (pNameItem)
				{
					pGameList->Expand(pNameItem->m_hCurrentItem,TVE_EXPAND);

					pMainRoom->SetTimer(IDT_QUICK_ENTER_ROOM,100,NULL);
				}
			}
			else
			{
				pGameRoom->OnFindNullDesk();
			}
		}
	}

	return 0;
}

#endif // MY_QUICK_GAME_ON
void CMainRoomEx::char2Tchar(char *pchar,TCHAR * tchar) 
{
#ifndef UNICODE
	strcpy(tchar, pchar);
#else
	int   nwLen  =MultiByteToWideChar(CP_ACP,0,pchar,-1,NULL,0); 
	MultiByteToWideChar(CP_ACP,0,pchar,-1,tchar,nwLen);
#endif

}

string CMainRoomEx::JoinUrl(string p1, string p2)
{
	int i = p1.size();
	while (p1[i - 1] == '/' && i - 1 >= 0)
	{
		i--;
	}
	p1 = p1.substr(0, i);

	i = 0;
	while (p2[i] == '/' && i < p2.size())
	{
		i++;
	}
	p2 = p2.substr(i, p2.size() - i);

	return p1 + "/" + p2;
}

RoomInfoStruct* CMainRoomEx::GetCurrentGameRoom()
{
	return &m_RoomInfo[0];
}

CString CMainRoomEx::GetUrlValue(CString szUrl, CString szField)
{
	szField=szField+"=";
	CString sValue="";
	int nStart=szUrl.Find(szField);
	if(nStart==-1)
		return "";
	nStart+=szField.GetLength();
	int nStop=szUrl.Find("&",nStart);
	if(nStop==-1)
		nStop=szUrl.GetLength();
	sValue= szUrl.Mid(nStart,nStop-nStart);
	return sValue;
}

//匿名管道回调 读 接口 2012.08.20 yyf
void CMainRoomEx::ReadAnonymousPipe(LPVOID lpBuffer, DWORD dwNumberOfBytesToRead)
{  
	UpdateStructMsg *pmsg = (UpdateStructMsg*)lpBuffer;
	CString str;
	str.Format("yyf: nRoomID = %d ,g_GameName = %s,  g_url = %s, g_ver = %s",
		pmsg->nRoomID,pmsg->g_GameName,pmsg->g_url,pmsg->g_ver);
	OutputDebugString("yyf: CMainRoomEx::ReadAnonymousPipe(..)");
	OutputDebugString(str);

	if(NULL != pmsg)
	{ //收到子进程过来的消息，说明那个更新线程已经完成。这边可以把相同 roomid 的从 正在更新列表 m_UpdateList 中 删掉
		for(vector<LPUPDATE>::iterator it = m_UpdateList.begin(); it != m_UpdateList.end();it++)
		{
			LPUPDATE lp = *it;
			if(!lp) continue;
			if(NULL != lp && lp->uNameID == pmsg->nRoomID )
			{
				lp->uNameID = -2;
			}
		}
	}

	switch(pmsg->command)
	{
	case 0:
		{
			break;
		}
	case 2:
		{ 
			break;
		}
	} 
}
//本机进程间网络回调接口
void CMainRoomEx::ReadSimpleSocket(LPVOID lpBuffer, DWORD dwNumberOfBytesToRead)
{   
	UpdateStructMsg *pmsg = (UpdateStructMsg*)lpBuffer;
	CString str;
	str.Format("yyf: nRoomID = %d ,g_GameName = %s,  g_url = %s, g_ver = %s",
		pmsg->nRoomID,pmsg->g_GameName,pmsg->g_url,pmsg->g_ver);
	OutputDebugString("yyf: CMainRoomEx::ReadSimpleSocket(..)");
	OutputDebugString(str);

	if(NULL != pmsg)
	{ //收到子进程过来的消息，说明那个更新线程已经完成。这边可以把相同 roomid 的从 正在更新列表 m_UpdateList 中 删掉
		for(vector<LPUPDATE>::iterator it = m_UpdateList.begin(); it != m_UpdateList.end();it++)
		{
			LPUPDATE lp = *it;
			if(!lp) continue;
			if(NULL != lp && lp->uNameID == pmsg->nRoomID )
			{				
				lp->uNameID = -2;
			}
		}
	}

	switch(pmsg->command)
	{
	case 0:
		{
			break;
		}
	case 2:
		{ 
			break;
		}
	} 
}

//下载更新线程  2012.08.24 yyf
DWORD WINAPI CMainRoomEx::DownLoadUpdate(LPVOID lparam)
{ 
	sDownLoadUpdateMsg *psDownLoadUpdateMsg = (sDownLoadUpdateMsg *)lparam; 

	if(NULL == psDownLoadUpdateMsg)
	{
		return -1;
	}

	if(NULL == psDownLoadUpdateMsg->pCMainRoomEx)
	{
		return -1;
	}

	if(! (psDownLoadUpdateMsg->pCMainRoomEx->m_CSocket32.OpenProcess("update.exe")))
	{ 
		CString str;
		str = psDownLoadUpdateMsg->pCBcfFile->GetKeyVal("MainRoom","UpdateErrorReinstall",
			"升级程序启动失败. 重新安装大厅可能会修复此问题.");		 
        DUIMessageBox(NULL,MB_ICONINFORMATION|MB_OK,psDownLoadUpdateMsg->pUpdateStructMsg->g_GameName,str);
	}  
	else if(psDownLoadUpdateMsg->pCMainRoomEx->m_CSocket32.GetProcessIdFromName("update.exe"))
	{  
		psDownLoadUpdateMsg->pCMainRoomEx->m_CSocket32.Send(psDownLoadUpdateMsg->pUpdateStructMsg ,sizeof(UpdateStructMsg));

		UPDATE *lp= psDownLoadUpdateMsg->pUPDATE;

		if(NULL != lp && lp->uNameID >=0)
		{
			try
			{
				::EnterCriticalSection(&(psDownLoadUpdateMsg->pCMainRoomEx->m_Section));
				if(psDownLoadUpdateMsg->pCMainRoomEx->m_UpdateList.size()<64)
				{
					psDownLoadUpdateMsg->pCMainRoomEx->m_UpdateList.push_back(lp); 
				}
				psDownLoadUpdateMsg->pCMainRoomEx->SetTimer(CHECK_UPDATE,2000,NULL);
				::LeaveCriticalSection(&(psDownLoadUpdateMsg->pCMainRoomEx->m_Section));
			}
			catch(...)
			{
				::LeaveCriticalSection(&(psDownLoadUpdateMsg->pCMainRoomEx->m_Section));
			}
		}

	}

	return 0;
}

//获取在线人数
UINT CMainRoomEx::GetonlineUserCount()
{
	return m_onlineUserCount;
}
//设置在线人数
void CMainRoomEx::SetonlineUserCount(UINT onlineUserNum)
{
	m_onlineUserCount = onlineUserNum;
	m_pGameListWnd->m_pLeftWnd->UpdatePeopleCount();
}
