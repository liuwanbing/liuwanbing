// LoveSendClassInExe.cpp : 实现文件
//

#include "stdafx.h"
#include "LoveSendClassInExe.h"
#include "Platform/bzsound.h"
#include "../common/gameroommessage.h"
#include "../common/gameplacemessage.h"
#include "Resource.h"
#include "Platform/confirmdlg.h"
#include "IMSetUser.h"
#include "IMMessage.h"

#include "detours.h"
#include "skinsb.h"

#include "PBank4GameWnd.h"		///< 第二版游戏银行
#include "RightFrame.h"			///游戏右边框
#include "GameTrottingWnd.h"
#include "UseBoardCastWnd.h"    ///喇叭道具窗口

#include "BzCrashRpt.h"

#include "NewGameFrame/XLogic.h"

#define  _USE_PROP_PAGE_SW_

//控件 ID
#define ID_GAME_VIEW						10							//游戏视图 ID

//用户菜单消息定义
#define IDM_COPY_USER_NAME					WM_USER+101					//拷贝用户名
#define IDM_SET_TALK_OBJECT					WM_USER+102					//设为谈话对象 
#define IDM_CANCEL_TALK						WM_USER+103					//取消聊天对象 
#define IDM_EMABLE_WATCH					WM_USER+104					//允许旁观 
#define IDM_UNEMABLE_WATCH					WM_USER+105					//禁止旁观
#define IDM_SHIELD_USER						WM_USER+106					//屏蔽用户
#define IDM_UNSHIELD_USER					WM_USER+107					//不蔽用户
#define IDM_KICK_USER						WM_USER + 108				//请他离开(踢人)
#define IDM_SET_FRIEND						WM_USER+227                 //加为好友
#define IDM_SET_EMENY                       WM_USER+228                 //加入黑名单
//#define IDM_GIVE_PROP						WM_USER+109

// PengJiLin, 2010-9-10, 踢人卡的菜单消息
#define IDM_KICK_USER_PROP                  WM_USER+110

// duanxiaohui 2011-11-11 用户详细信息
#define IDM_USER_INFO                       WM_USER + 111

/// 用来确保读取到文件的宏，如果在游戏目录下读取不到，则在大厅目录下读取文件
/// 调用前须事先准备好skinHall，因为文件有时在skin0\下，有时在skin0\game\下

#ifndef BZ_ZLIB
#ifndef GET_FILENAME
	#define GET_FILENAME(path, FormatStr,skinfolder) \
		wsprintf(path,FormatStr,skinfolder);\
		if (!CBcfFile::IsFileExist(path))\
		{\
		wsprintf(path,FormatStr,skinHall);\
		}
	#endif// GET_FILENAME
#else
	#define GET_FILENAME(path, FormatStr,skinfolder) \
		wsprintf(path,FormatStr,skinfolder);
#endif//GET_FILENAME


// 此为静态成员，须由使用者在进程结束时显式调用CBcfFile::ClearMap()函数来释放，否则MFC会报内存泄漏
EXT_CLASS std::map<CString, CBcfFileNameItem*> CBcfFile::mapFile;
EXT_CLASS std::map<CString, FileItem*> CUnZipRes::mapFile;
unsigned char CUnZipRes::m_bPassWord[MAX_PASSWORD_LENGTH];

extern UINT AFCMessageBox(CString strInfo,CString strTitel,UINT style);
static CGlobal g_global;
CGlobal& Glb(){return g_global;}

//静态变量
CArray<LONG,LONG>				CLoveSendClassInExe::m_ShieldArray;			//屏蔽用户

//等级判断函数
TCHAR * GetGameOrderNameInExe(__int64 dwPoint)
{
	static __int64 uBasePoint[15]=
	{
		-300,0,100,200,
		400,700,1100,1600,
		2200,2900,3700,4500,
		5500,6700,8000
	};
	static TCHAR * szOrderName[15]=
	{
		TEXT("庶民"),TEXT("秀才"),TEXT("举人"),TEXT("九品芝麻官"),
		TEXT("八品县丞"),TEXT("七品知县"),TEXT("六品通判"),TEXT("五品知州"),
		TEXT("四品知府"),TEXT("三品按察使"),TEXT("二品巡抚"),TEXT("一品大学士"),
		TEXT("宰相"),TEXT("王爷"),TEXT("皇帝")
	};
	for (int i=0;i<15;i++)	if (dwPoint<uBasePoint[i]) return szOrderName[i];
	return TEXT("皇帝");
}
/// 获取指定数值的字符串
/// 本函数同时支持UNICODE和ANSI
/// @param[out] str 字符串数组，须由调用者分配空间
/// @param[in] nNum 须显示的整数数值
/// @param[in] nPower 以10的几次幂显示，如123显示成123000，则此参数为3
/// @param[in] bHasSpace 字符串是否分节显示，默认值false表示数字不分节，true表示用空格每三位分一节
/// @return 返回str所在的位置
///
// PengJiLin, 2010-8-3, 增加可以输入分隔符
// PengJiLin, 2011-7-8, 64位金币显示
TCHAR *GlbGetNumString(TCHAR str[], __int64 nNum, int nPower, bool bHasSpace, TCHAR* chSpace)
{
	/// 暂存结果
	TCHAR strTmp[32];
	/// 如果是0，就直接返回一个0 
	if (0==nNum)
	{
		_tcscpy(str, TEXT("0"));
		return str;
	}

	wsprintf( strTmp, TEXT("%I64d"), nNum);

	//Add By YangJie 2011-02-21
	if (nPower >= 0) {
		for (int i=0; i<nPower; ++i)
		{
			_tcscat( strTmp, TEXT("0"));
		}
	} else {
		bool bFlag = false;
		if (nNum < 0) {
			::memset(strTmp, 0, sizeof(strTmp));
			wsprintf(strTmp, TEXT("%I64d"), -nNum);
			bFlag = true;
		}

		int iLen = _tcslen(strTmp);
		int tmpPower = -nPower;
		if (iLen > tmpPower) {
			for (int i = iLen; i > (iLen - tmpPower); i --) {
				strTmp[i] = strTmp[i-1];
			}
			strTmp[iLen-tmpPower] = TEXT('.');
			strTmp[iLen+1] = TEXT('\0');
		} else {
			TCHAR strTmp1[32];
			int i = 0;
			int j = 0;
			int k = 0;
			::memset(strTmp1, 0, sizeof(strTmp1));
			for (i = 0; i < (tmpPower - iLen + 1); i ++) {
				strTmp1[i] = TEXT('0');
			}
			for (j = i, k = 0; k < iLen;j ++, k ++) {
				strTmp1[j] = strTmp[k];
			}
			strTmp1[j++] = TEXT('\0');
			::memset(strTmp, 0, sizeof(strTmp));
			_tcscpy(strTmp, strTmp1);
			for (i = j; i > 1; i --) {
				strTmp[i] = strTmp[i-1];
			}
			strTmp[1] = TEXT('.');
			strTmp[j+1] = TEXT('\0');
		}

		if (bFlag) {
			iLen = _tcslen(strTmp);
			for (int i = iLen; i > 0; i --) {
				strTmp[i] = strTmp[i-1];
			}
			strTmp[0] = TEXT('-');
			strTmp[iLen+1] = TEXT('\0');
		}
	}


	// 如果没有空格则直接返回整个字符串
	if (!bHasSpace)
	{
		_tcscpy(str, strTmp);
		return str;
	}

	if (nPower >= 0) {

		// PengJiLin, 2010-9-1, 可以配置多少位分为一节, 最大不超过8位   
		CString s = CBcfFile::GetAppPath();
		CBcfFile f( s + "Order.bcf");
		BYTE byBits = f.GetKeyVal("VirtualMoney", "SpaceBits", 3);

		if(byBits < 1) byBits = 1;
		if(byBits > 8) byBits = 8;

		/// 按位数每三位空一格显示
		int nLen = _tcslen(strTmp);	// 字符串长度
		int nTimes = nLen % byBits/*3*/;		// 三位一节时，多余的位数

		int nSrc = 0;
		int nDes = 0;

		// PengJiLin, 2010-8-3, 增加可以输入分隔符
		TCHAR chUseSpace = 0;
		if(NULL == chSpace)
		{
			chUseSpace = ' ';
		}
		else
		{
			chUseSpace = chSpace[0];
		}

		for (int i=0; i<nLen; ++i)
		{
			/// 负数时，负号与数值间不加空格
			if ((i>0) && (i%byBits==nTimes) && (!(i==1 && nNum<0)))
			{
				str[nDes++] = chUseSpace;
			}
			str[nDes++] = strTmp[nSrc++];
		}
		str[nDes] = TEXT('\0');
	} else {

		_tcscpy(str, strTmp);
	}

	return str;
}
void DebugPrintf(const char *p, ...)
{
	char szFilename[256];
	sprintf( szFilename, "E:\\hn\\DebugInfo\\%s.txt", Glb().m_szProcessName);
	FILE *fp = fopen( szFilename, "a" );
	if (NULL == fp)
	{
		return;
	}
	va_list arg;
	va_start( arg, p );
	fprintf(fp,"[%s]--", CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S"));
	vfprintf(fp, p, arg );
	fprintf(fp,"\n");
	fclose(fp);
}

void DebugPrintH(const void *buf, int nSize)
{
	FILE *fp = fopen( "E:\\hn\\DebugInfo\\Hex.txt", "a" );
	if (NULL == fp)
	{
		return;
	}
	BYTE *p = (BYTE *)buf;
	for (int i=0; i<nSize; ++i)
	{
		if (i%16 == 0)
		{
			fprintf(fp, "\n");
		}
		fprintf(fp, "%02X ", *p++);
	}
	fprintf(fp, "\n");
	fclose(fp);
}

// CLoveSendClassInExe 对话框

IMPLEMENT_DYNAMIC(CLoveSendClassInExe, CLoadFileBmp)

BEGIN_MESSAGE_MAP(CLoveSendClassInExe, CLoadFileBmp)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_MESSAGE(WM_HIT_EXPMESSTION,OnExpression)
	ON_MESSAGE(WM_RICH_EDIT_HIT,OnRichEditHit)
	ON_MESSAGE(WM_CLOSE_GAME,OnCloseGame)
	ON_MESSAGE(WM_SPLIT_WND,OnSplitMessage)
	ON_MESSAGE(WM_RELOAD_TALKINI,OnReLoadTalkINI)
	ON_MESSAGE(WM_USE_PROP,OnPannelUseProp)		// 从道具面板中传回使用道具消息
	ON_MESSAGE(WM_PROP_BUY,OnPannelBuyProp)		// 从道具面板中传回购买道具消息
	ON_MESSAGE(WM_GIVE_PROP,OnPannelGiveProp)	// 从道具面板中传回赠送道具消息
	ON_MESSAGE(WM_BUY_VIP,OnPannelBuyVIP)		// 从道具面板中传回购买VIP消息   
	ON_MESSAGE(WM_USE_BOARDCAST,OnUseBoardcast)
	ON_MESSAGE(WM_BANK_CHECK_OUT,OnBankCheckOut)	// 从银行面板中取钱
	ON_MESSAGE(WM_GETFRIENDLIST,OnSendGetFriendList)		// 从道具面板中传回购买VIP消息  
	ON_MESSAGE(WM_CHECK_MONEY,OnGameBankMoneyCheck) //游戏银行存取钱
	ON_MESSAGE(WM_CHECK_MONEY2,OnGameBankMoneyCheck2) //游戏银行存取钱
	ON_MESSAGE(WM_BANK2_CLOSE,OnGameBankClose) //关闭游戏银行
	//	ON_MESSAGE(IDM_GETPOINT,OnGetPoint)

	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_EXIT, OnBnClickedExit)
	ON_BN_CLICKED(IDC_REST, OnBnClickedRest)
	ON_BN_CLICKED(IDC_QUIT_GAME, &CLoveSendClassInExe::OnBnClickedQuitGame)
	ON_BN_CLICKED(IDC_MAX, &CLoveSendClassInExe::OnBnClickedMax)
	ON_BN_CLICKED(IDC_HIDE, &CLoveSendClassInExe::OnBnClickedHide)
	ON_BN_CLICKED(IDC_MIN, &CLoveSendClassInExe::OnBnClickedMin)
	ON_WM_MOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_GAME_BTN_01, &CLoveSendClassInExe::OnBnClickedGameBtn01)
	ON_BN_CLICKED(IDC_GAME_BTN_02, &CLoveSendClassInExe::OnBnClickedGameBtn02)
	ON_BN_CLICKED(IDC_GAME_BTN_03, &CLoveSendClassInExe::OnBnClickedGameBtn03)
	ON_BN_CLICKED(IDC_GAME_BTN_04, &CLoveSendClassInExe::OnBnClickedGameBtn04)

	ON_BN_CLICKED(IDC_BUTTON_PROP1, &CLoveSendClassInExe::OnBnClickedButtonProp1)
	ON_BN_CLICKED(IDC_BUTTON_PROP2, &CLoveSendClassInExe::OnBnClickedButtonProp2)
	ON_BN_CLICKED(IDC_BUTTON_PROP3, &CLoveSendClassInExe::OnBnClickedButtonProp3)
	ON_BN_CLICKED(IDC_BUTTON_PROP4, &CLoveSendClassInExe::OnBnClickedButtonProp4)
	ON_BN_CLICKED(IDC_BUTTON_PROP5, &CLoveSendClassInExe::OnBnClickedButtonProp5)
	ON_BN_CLICKED(IDC_BUTTON_PROP6, &CLoveSendClassInExe::OnBnClickedButtonProp6)  
	ON_BN_CLICKED(IDC_BUTTON_PROP7, &CLoveSendClassInExe::OnBnClickedButtonProp7)
	ON_BN_CLICKED(IDC_BUTTON_PROP8, &CLoveSendClassInExe::OnBnClickedButtonProp8)
	ON_BN_CLICKED(IDC_BUTTON_PROP9, &CLoveSendClassInExe::OnBnClickedButtonProp9)
	ON_BN_CLICKED(IDC_BUTTON_PROP10, &CLoveSendClassInExe::OnBnClickedButtonProp10)

	ON_NOTIFY_EX( TTN_NEEDTEXT, 0, OnToolTipNotify)

	//ON_BN_CLICKED(IDC_SNDPLAY,OnSndPlay)
	//ON_BN_CLICKED(IDC_SNDPAUSE,OnSndPause)
	//ON_BN_CLICKED(IDC_SNDVOLDOWN,OnSndVolDown)
	//ON_BN_CLICKED(IDC_SNDVOLUP,OnSndVolUp)

	ON_CBN_SELCHANGE(IDC_INPUT, &CLoveSendClassInExe::OnCbnSelchangeInput)
	ON_BN_CLICKED(IDC_SEND, &CLoveSendClassInExe::OnBnClickedSend)
	ON_BN_CLICKED(IDC_SETCOLOR, &CLoveSendClassInExe::OnBnClickedSetcolor)
	ON_BN_CLICKED(IDC_EXPRESSION, &CLoveSendClassInExe::OnBnClickedExpression)
	ON_BN_CLICKED(IDC_GO_AHEAD, &CLoveSendClassInExe::OnBnClickedGoAhead)
	ON_BN_CLICKED(IDC_GO_NEXT, &CLoveSendClassInExe::OnBnClickedGoNext)
    ON_BN_CLICKED(IDC_GAME_BTN_05, OnBnClickedBlueDiamond) // PengJiLin, 2010-9-30, 蓝钻贵族
	ON_WM_CLOSE()
	ON_WM_NCLBUTTONDBLCLK()

    // PengJiLin, 2010-10-12, 商场道具功能
    ON_BN_CLICKED(IDC_GAME_BTN_06,OnBnClickedGoAheadS)
    ON_BN_CLICKED(IDC_GAME_BTN_07,OnBnClickedGoNextS)

    ON_WM_ACTIVATE()                // PengJiLin, 2011-6-23, 激活消息

END_MESSAGE_MAP()

__int64                  g_PointArray[20];                  ///等级积分数组
map<int,string>          g_LevelNameArray;                  ///等级名字数组

CLoveSendClassInExe::CLoveSendClassInExe(CUsingSkinClass * pGameView)
: CLoadFileBmp(IDD_EXE_DIALOG), m_pGameView(pGameView), m_MessageHandle(&m_TalkMessage)
{
	m_iVideoOffsetNum = 0;
	memset(m_pVideoXOffset, 0, sizeof(m_pVideoXOffset));
	memset(m_pVideoYOffset, 0, sizeof(m_pVideoYOffset));

	m_pVideoInterface = NULL;
	m_bInit = false;
	m_CurpUserInfoClicked=NULL;
	m_pGameRoom=NULL;
	m_pGameInfo=NULL;
	m_dlgBroadcast = NULL;
	m_uTimeID=0;
	m_uTimeCount=0;
	m_bTimeStation=255;
	m_bSaveView=false;
	m_bWatchMode=false;
	m_bWatchOther=false;
	m_dwLastTalkTime=0L;
	m_dwCurrentUserID=0L;
	memset(m_pUserInfo,0,sizeof(m_pUserInfo));

	CString strBcfFilename = m_skinmgr.GetGameSkinBcfFileName();
	if (CBcfFile::IsFileExist(CBcfFile::GetAppPath() + "config\\" + strBcfFilename))
		m_iniFile= CBcfFile::GetAppPath() + "config\\" + strBcfFilename;
	else
		m_iniFile= CBcfFile::GetAppPath()+"..\\config\\" + strBcfFilename;


	m_BtPropArray[0] = IDC_BUTTON_PROP2;
	m_BtPropArray[1] = IDC_BUTTON_PROP3;
	m_BtPropArray[2] = IDC_BUTTON_PROP4;
	m_BtPropArray[3] = IDC_BUTTON_PROP5;
	m_BtPropArray[4] = IDC_BUTTON_PROP6;
	m_BtPropArray[5] = IDC_BUTTON_PROP7;
	m_BtPropArray[6] = IDC_BUTTON_PROP8;
	m_BtPropArray[7] = IDC_BUTTON_PROP9;
	m_BtPropArray[8] = IDC_BUTTON_PROP10;
	//m_BtPropArray[7] = IDC_BUTTON_PROP8;
	//m_BtPropArray[3] = IDC_BUTTON_PROP4;  // PengJiLin, 2010-10-12, 注释，只显示3个

	m_curPropPage=0;		//当前道具页=0，每页4个
	m_pPropSelUser=NULL;//2008-04-18
	m_MessageHandle.SetColor(m_colorresource);

	szAdvUrl="";
	m_pPropPannel = NULL;

	m_sndListID=0;
	m_sndBtnCx=m_sndBtnCy=16;

	pSingnalimg=NULL;
	m_NetSignalLevel=0;
	m_SavedSingalIndex=0;
	m_pPersonBank = NULL;

	//第二版游戏银行对话框
	m_pBank2Wnd = NULL;	

	m_pUserDetailsDlg = NULL;

	m_pRightFrame = NULL;

    // PengJiLin, 2010-9-9
    m_dlgKickProp = NULL;

    m_curPropPage_S = 0;

	//跑马灯控件
	m_pPmdWnd = NULL;

    memset(&m_SendMoneyData, 0, sizeof(m_SendMoneyData));

	m_bIsQueueDisMiss = false;

	// 初始BzCrashRpt
// 	BzCrash_Initiation();
// 	BzCrash_DisableSetUnhandledExceptionFilter();
// 	BzCrash_SetProjectName("D平台大厅");
// 	BzCrash_SetEmailSender("bzbugrev@sina.com");
// 	BzCrash_SetEmailReceiver("bzbugrev@sina.com");
// 	BzCrash_SetSmtpServer("smtp.sina.com");
// 	BzCrash_SetSmtpUser("bzbugrev");
// 	BzCrash_SetSmtpPassword("1122334455");
// 	BzCrash_DeleteSended(false);

}

CLoveSendClassInExe::~CLoveSendClassInExe()
{
	if(m_pPersonBank != NULL)
	{
		delete m_pPersonBank;
		m_pPersonBank = NULL;
	}

	if (m_pUserDetailsDlg != NULL)
	{
		delete m_pUserDetailsDlg;
		m_pUserDetailsDlg = NULL;
	}

	if (m_pVideoInterface != NULL)
	{
		m_pVideoInterface->exit();
		if (m_pVideoInterface != NULL)
		{
			m_pVideoInterface = NULL;
		}
	}
	delete m_pPropSelUser;
	delete m_dlgBroadcast;
	delete m_pPropPannel;
	m_dlgBroadcast = NULL;
	m_TalkMessage.DestroyWindow();
	UserItemStruct * pFindUserItem=NULL;
	INT_PTR uCount=m_ThisDeskPtrArray.GetCount();
	for (INT_PTR i=0;i<uCount;++i)
	{
		pFindUserItem=(UserItemStruct *)m_ThisDeskPtrArray.GetAt(i);
		delete pFindUserItem;
		pFindUserItem = NULL;
	}
	m_ThisDeskPtrArray.RemoveAll();
}

////写日志 add by wlr 20090806
//void CLoveSendClassInExe::WriteInfo(TCHAR * pch)
//{
//	SYSTEMTIME st;
//	GetLocalTime(&st);
//	CString cStr;
//	cStr.Format(_T("%d-%d-%d %02d:%02d:%02d 游戏玩家id=%d,%s"),
//		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute,st.wSecond,
//		GetMeUserInfo()->dwUserID, pch);
//	WriteStr(cStr.GetBuffer());
//	return;
//}
////end of 写日志 add by wlr 20090806

void CLoveSendClassInExe::DoDataExchange(CDataExchange* pDX)
{
	CLoadFileBmp::DoDataExchange(pDX);
	//	DDX_Control(pDX, IDC_HOR_SPLIT, m_HorSplitBar);

	DDX_Control(pDX, IDC_INPUT, m_InputMessage);
	DDX_Control(pDX, IDC_SEND, m_BtSend);
	DDX_Control(pDX, IDC_EXPRESSION, m_BtExpression);
	DDX_Control(pDX, IDC_TALK_MESSAGE, m_TalkMessage);
	DDX_Control(pDX, IDC_QUIT_GAME, m_BtGameQuit);
	DDX_Control(pDX, IDC_SETCOLOR, m_BtSetColor);

	DDX_Control(pDX, IDC_HIDE, m_BtHide);
	DDX_Control(pDX, IDC_EXIT, m_BtExit);
	DDX_Control(pDX, IDC_MIN, m_BtMin);
	DDX_Control(pDX, IDC_MAX, m_btMax);
	DDX_Control(pDX, IDC_REST, m_btRest);

	DDX_Control(pDX, IDC_GAME_BTN_01, m_Btbt1);
	DDX_Control(pDX, IDC_GAME_BTN_02, m_Btbt2);
	DDX_Control(pDX, IDC_GAME_BTN_03, m_Btbt3);
	DDX_Control(pDX, IDC_GAME_BTN_04, m_Btbt4);
	DDX_Control(pDX, IDC_GAME_BTN_05, m_Btbt5);
	DDX_Control(pDX, IDC_GAME_BTN_06, m_Btbt6);//道具面板第一排向前
	DDX_Control(pDX, IDC_GAME_BTN_07, m_Btbt7);//              向后
	DDX_Control(pDX, IDC_GAME_BTN_10, m_Btbt10);

	//{add by zxj 2008/04/07
	DDX_Control(pDX, IDC_GO_AHEAD, m_BtPropGoAhead);//道具面板第一排向前
	DDX_Control(pDX, IDC_GO_NEXT, m_BtPropGoNext);
	DDX_Control(pDX, IDC_BUTTON_PROP1, m_BtProp1);
	DDX_Control(pDX, IDC_BUTTON_PROP2, m_BtProp2);
	DDX_Control(pDX, IDC_BUTTON_PROP3, m_BtProp3);
	DDX_Control(pDX, IDC_BUTTON_PROP4, m_BtProp4);
	DDX_Control(pDX, IDC_BUTTON_PROP5, m_BtProp5);
	DDX_Control(pDX, IDC_BUTTON_PROP6, m_BtProp6);
	DDX_Control(pDX, IDC_BUTTON_PROP7, m_BtProp7);
	DDX_Control(pDX, IDC_BUTTON_PROP8, m_BtProp8);
	DDX_Control(pDX, IDC_BUTTON_PROP9, m_BtProp9);
	DDX_Control(pDX, IDC_BUTTON_PROP10, m_BtProp10);
	//end add}

	DDX_Control(pDX, IDC_EXPLORER1, m_ie);

	//DDX_Control(pDX, IDC_SNDPLAY,m_btSndPlay);
	//DDX_Control(pDX, IDC_SNDPAUSE,m_btSndPause);
	//DDX_Control(pDX, IDC_SNDVOLDOWN,m_btVolDown);
	//DDX_Control(pDX, IDC_SNDVOLUP,m_btVolUp);
}



//建立窗口
bool CLoveSendClassInExe::CreateFrame()
{
	if (GetSafeHwnd()==NULL) 
	{
		AfxSetResourceHandle(GetModuleHandle(FACE_DLL_NAME));		
		Create(IDD_EXE_DIALOG, GetDesktopWindow());
		AfxSetResourceHandle(GetModuleHandle(NULL));
	}
	m_SavedScreen.x=1024;//r.Width();
	m_SavedScreen.y=736;//r.Height();

	CBcfFile f(CBcfFile::GetAppPath() + _T("GameOption.bcf"));
	UINT uMaxType = f.GetKeyVal("FrameControl","max_type",1);

	if (uMaxType == 1) //普通最大化
	{
		m_bShowMax=true;
	}
	else if (uMaxType == 2) //最大采用改变屏幕分辨率的方式
	{
		m_bShowMax=false;
	}

	//获取视频窗口偏移位置
	CString strKey = _T("VideoOffset");
	m_iVideoOffsetNum = f.GetKeyVal(strKey, _T("DeskPeople"), 0);
	if (0 < m_iVideoOffsetNum && m_iVideoOffsetNum <= VIDEO_MAX_PEOPLE)
	{
		CString strX, strY;
		for (int i=0; i!=m_iVideoOffsetNum; ++i)
		{
			strX.Format(_T("People%dx"), i);
			strY.Format(_T("People%dy"), i);
			m_pVideoXOffset[i] = f.GetKeyVal(strKey, strX, 0);
			m_pVideoYOffset[i] = f.GetKeyVal(strKey, strY, 0);
		}
	}
	
	m_CurpUserInfoClicked=GetMeUserInfo();
	ResetGameStation();
	// 背景音乐
	//m_PlaySound.PlayMusic(NULL,m_BackMusicIni,m_BMusicKey,m_pGameInfo->bEnableBackSound);
	if(this->m_hWnd)
	{
		//		OnBnClickedMax();
		ShowWindow(SW_SHOW);
		SetForegroundWindow();
		this->m_pGameRoom->ShowWindow(SW_HIDE);
	}
	if(!m_bSndPause)
		BZSoundContinue(m_sndListID);
	//	AfxGetApp()->m_pMainWnd->PostMessage(WM_USER+100,2,0);

	//
	//m_iPaintCount=0;
	//	SetTimer(IDT_SHOWWIN,500,NULL);

	//  增加详细资料框
	m_pUserDetailsDlg = new CUserDetailsDlg();
	m_pUserDetailsDlg->m_uGameNameID = m_pGameInfo->uNameID;
	m_pUserDetailsDlg->m_uComType = m_pGameInfo->uComType;
	AfxSetResourceHandle(GetModuleHandle(FACE_DLL_NAME));
	m_pUserDetailsDlg->Create(IDD_USER_DETAILS, this);
	AfxSetResourceHandle(GetModuleHandle(NULL));
	// end 

	m_pPmdWnd = new CGameTrottingWnd(this);
	m_pPmdWnd->init(this->GetSafeHwnd());
	m_pPmdWnd->SetLabelTextFont("宋体", 16);
	m_pPmdWnd->ShowWindow(SW_HIDE);

	return true;
}
//删除对象
void CLoveSendClassInExe::DeleteFrame()
{
	if (GetSafeHwnd() != NULL)
		OnCancel();
	delete this;
}

//视频设置改变
LRESULT CLoveSendClassInExe::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	//跑马灯位置匹配父窗口
	if (message == WM_MOVE)
	{
		if (m_pPmdWnd)
		{
			if (this->GetSafeHwnd())
			{
				m_pPmdWnd->AdjustPos(this->GetSafeHwnd());
			}
		}
	}
	return CDialog::WindowProc(message, wParam, lParam);
}
// 用从平台传来的消息初始化
bool CLoveSendClassInExe::InitFrame(GameInfoStructInExe * pGameInfo, CWnd *pGameRoom)
{
	m_pGameInfo=pGameInfo;
	m_pGameRoom=pGameRoom;
	_tcscpy(m_szGameRoomName, pGameInfo->szGameRoomName); //显示房间名字 
	Glb().m_szProcessName = pGameInfo->szProcessName;
	if (m_pGameView != NULL) m_pGameView->SetGameInfo(pGameInfo);

	return true;
}
//关闭窗口
bool CLoveSendClassInExe::AFCCloseFrame(bool bNotifyServer)
{
	//OutputDebugString("[aa]SDK CLoveSendClassInExe::AFCCloseFrame");
	//保存设置参数
	WriteProfileInt(TEXT("CanLooking"),m_pGameInfo->bEnableWatch?1:0);
	WriteProfileInt(TEXT("PlaySound"),m_pGameInfo->bEnableSound?1:0);
	WriteProfileInt(TEXT("BackSound"),m_pGameInfo->bEnableBackSound?1:0);

	WriteProfileInt(TEXT("ShowUserInfo"),m_pGameInfo->bShowUserInfo?1:0);
	//	m_PlaySound.StopMusic();
	//m_DlgGetPoint.ShowWindow(false);
	//设置参数
	SetStationParameter(0);
	//去掉显示
	if(m_DlgShowLeft.Showing)
	{
		m_DlgShowLeft.KillTimer(1);
		m_DlgShowLeft.ShowWindow(0);
		m_DlgShowLeft.Showing=0;
		m_DlgShowLeft.DestroyWindow();
	}
	m_TalkMessage.DestroyWindow();

	// 清除同桌玩家清单
	UserItemStruct * pFindUserItem=NULL;
	INT_PTR uCount=m_ThisDeskPtrArray.GetCount();
	for (INT_PTR i=0;i<uCount;++i)
	{
		pFindUserItem=(UserItemStruct *)m_ThisDeskPtrArray.GetAt(i);
		delete pFindUserItem;
		pFindUserItem = NULL;
	}
	m_ThisDeskPtrArray.RemoveAll();

	//添加视频代码，用户退出游戏时，要清空视频信息
	if (m_pVideoInterface)
	{
		m_pVideoInterface->exit();
		if (m_pVideoInterface)
			m_pVideoInterface = NULL;
	}
	//发送关闭消息到父窗口
	//::SendMessage(m_pGameRoom->m_hWnd,IDM_CLOSE_GAME,0,0);

	if (GetSafeHwnd()!=NULL) 
	{
		//		m_pGameView->ResetGameView();
		ResetGameFrame();
		this->ShowWindow (SW_HIDE);
		//this->DestroyWindow();
	}
	BZSoundPause(m_sndListID);
	CWnd * winApp=AfxGetApp()->m_pMainWnd;
	if(winApp && winApp->m_hWnd){
		winApp->PostMessage(WM_GAME_QUIT,bNotifyServer?0:1,0);	// 短参不为0，表示不通知大厅
		//OutputDebugString("[aa]SDK CLoveSendClassInExe::AFCCloseFrame end");
	}
	if (NULL != m_pPersonBank)
	{
		if (NULL != m_pPersonBank->GetSafeHwnd())
		{
			Sleep(10);
		}
		delete m_pPersonBank;
		m_pPersonBank = NULL;
	}

	return true;
}

// CLoveSendClassInExe 消息处理程序

int CLoveSendClassInExe::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	//创建右框架
	m_pRightFrame = new CRightFrame(this);
	m_pRightFrame->Create(m_hWnd,NULL,UI_WNDSTYLE_DIALOG,0,0,0,0,0,NULL);
	m_UserListDlg.m_pUserListDu = m_pRightFrame->m_pUsersList;
	m_MessageHandle.SwitchToDuiVersion(m_pRightFrame);

	//m_InputMessage.Create(CBS_DROPDOWN|CBS_AUTOHSCROLL|WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IDC_INPUT);//聊天输入框 下拉列表

	return 0;
}
//006位置变化
void CLoveSendClassInExe::OnSize(UINT nType, int cx, int cy)
{
	if (cx<800)
	{
		cx = 800;
	}
	if (cy<600)
	{
		cy = 600;
	}
	FixControlStation(cx,cy);
	__super::OnSize(nType, cx, cy);
	////调整拆分条
	//if (m_HorSplitBar.GetSafeHwnd()) 
	//{
	//	SafeMoveWindow(&m_HorSplitBar,cx-209,414,204,SPLIT_WIDE);
	//	m_HorSplitBar.InitSplitBar(310,cy-100,true);
	//}
	return;
}

// PengJiLin, 2011-6-23, 激活
void CLoveSendClassInExe::OnActivate(UINT uState, CWnd* pOldWnd, BOOL bMinisted)
{
	this->m_pGameRoom->PostMessage(WM_ACTIVE_TO_ROOM, uState, 1);
   /* if(WA_INACTIVE != uState)
    {
        this->m_pGameRoom->PostMessage(WM_ACTIVE_TO_ROOM, 0, 1);
	}
	else
	{
		this->m_pGameRoom->PostMessage(WM_ACTIVE_TO_ROOM, 1, 1);
    }*/
}

//设置状态参数
void CLoveSendClassInExe::SetStationParameter(BYTE bGameStation)
{
	m_pGameInfo->bGameStation=bGameStation;
	this->m_pGameRoom->PostMessage(WM_SET_STATION, 0, bGameStation);
}

//重新设置界面
void CLoveSendClassInExe::ResetGameFrame()
{
	//清理界面元素
	m_UserListDlg.DeleteAllUser();
	//for (int i=1;i<m_TargetPeople.GetCount();i++) 
	//	m_TargetPeople.DeleteString(i);
	//m_TargetPeople.SetCurSel(0);
	UpdateGameTitle();
	//清理用户资料
	m_bSaveView=false;
	m_bWatchMode=true;
	m_bWatchOther=false;
	m_pGameInfo->bGameStation=0;
	//m_pGameInfo->bDeskOnwer=false;
	memset(m_pUserInfo,0,sizeof(m_pUserInfo));
	for (INT_PTR i=0;i<m_WatchPtrArray.GetCount();i++) 
		m_WatchPtrArray.SetAt(i,NULL);
	m_BtGameQuit.m_bIsMouseMove = false;
	m_BtExit.m_bIsMouseMove = false;
	m_Btbt3.m_bIsMouseMove = false;
	return;
}

//表情消息
LRESULT CLoveSendClassInExe::OnExpression(WPARAM wparam, LPARAM lparam)
{
	CTrueItem * pExpItem=m_ExpressWnd.GetExpressItem((UINT)wparam);
	if (pExpItem!=NULL)
	{
		CString strBuffer;
		m_InputMessage.GetWindowText(strBuffer);
		strBuffer+=pExpItem->m_szTrancelate;
		m_InputMessage.SetWindowText(strBuffer);
		m_InputMessage.SetFocus();
		m_InputMessage.SetEditSel(strBuffer.GetLength(),strBuffer.GetLength());
		if(m_pRightFrame)
		{
			m_pRightFrame->AddExpressionToTalkInput(pExpItem->m_szTrancelate);
		}
	}
	return 0;
}

//名字消息
LRESULT CLoveSendClassInExe::OnRichEditHit(WPARAM wparam, LPARAM lparam)
{
	if ((wparam!=NULL)&&((lparam==WM_LBUTTONDBLCLK)||(lparam==WM_LBUTTONDOWN)))
	{
		CString * pStrBuffer=(CString *)wparam;
		//int iSelect=m_TargetPeople.FindString(0,*pStrBuffer);
		//if (iSelect!=LB_ERR) m_TargetPeople.SetCurSel(iSelect);
		m_InputMessage.SetFocus();
	}
	return 0;
}
//关闭游戏
LRESULT CLoveSendClassInExe::OnCloseGame(WPARAM wparam, LPARAM lparam)
{
	//去掉显示
	if(m_DlgShowLeft.Showing)
	{
		m_DlgShowLeft.KillTimer(1);
		m_DlgShowLeft.ShowWindow(0);
		m_DlgShowLeft.Showing=0;
		m_DlgShowLeft.DestroyWindow();
	}
	//关闭
	AFCCloseFrame();
	return 0;
}
//拆分条消息
LRESULT CLoveSendClassInExe::OnSplitMessage(WPARAM wparam, LPARAM lparam)
{
	//if (wparam==IDC_HOR_SPLIT)	//拆分条消息
	//{
	//	//移动拆分条
	//	CRect ClientRect;
	//	GetClientRect(&ClientRect);
	//	SafeMoveWindow(&m_HorSplitBar,ClientRect.right-209,HIWORD(lparam),204,SPLIT_WIDE);

	//	//移动其他控件
	//	FixControlStation(ClientRect.Width(),ClientRect.Height());
	//}

	return 0;
}
// 从道具面板中传回使用道具消息
LRESULT CLoveSendClassInExe::OnPannelUseProp(WPARAM wparam, LPARAM lparam)
{
	UsePropItem(lparam);
	return 0;
}
// 从道具面板中传回购买道具消息
LRESULT CLoveSendClassInExe::OnPannelBuyProp(WPARAM wparam, LPARAM lparam)
{
	this->m_pGameRoom->PostMessage(WM_PROP_BUY, wparam, lparam);
	return 0;
}
// 从道具面板中传回赠送道具消息
LRESULT CLoveSendClassInExe::OnPannelGiveProp(WPARAM wparam, LPARAM lparam)
{
	this->m_pGameRoom->PostMessage(WM_GIVE_PROP, wparam, lparam);
	return 0;
}

LRESULT CLoveSendClassInExe::OnPannelBuyVIP(WPARAM wparam, LPARAM lparam)
{
	this->m_pGameRoom->PostMessage(WM_BUY_VIP, wparam, lparam);
	return 0;
}

LRESULT CLoveSendClassInExe::OnSendGetFriendList(WPARAM wparam, LPARAM lparam)
{
	this->m_pGameRoom->PostMessage(WM_GETFRIENDLIST, wparam, lparam);
	return 0;
}

//游戏银行存取钱
LRESULT CLoveSendClassInExe::OnGameBankMoneyCheck(WPARAM wParam, LPARAM lParam)
{
	OnCheckMoney( (CheckMoney_t *)(lParam) );
	return 0;
}

LRESULT CLoveSendClassInExe::OnGameBankMoneyCheck2(WPARAM wParam, LPARAM lParam)
{
	OnCheckMoney2((bank_op_normal*)(lParam));
	return 0;
}

//关闭游戏银行窗口消息
LRESULT CLoveSendClassInExe::OnGameBankClose(WPARAM wParam, LPARAM lParam)
{
	OnCloseBank2();
	return 0;
}

//用户使用广播，对话框里确定后，在这里给用户ID和用户名赋值
LRESULT CLoveSendClassInExe::OnUseBoardcast(WPARAM wparam, LPARAM lparam)
{
	_TAG_BOARDCAST *pMsg = (_TAG_BOARDCAST *)lparam;
	pMsg->dwUserID = this->GetMeUserInfo()->dwUserID;
	//	int nType = *((int*)((BYTE*)lparam + sizeof(_TAG_BOARDCAST)));
	_tcscpy_s(pMsg->szUserName, 32, this->GetMeUserInfo()->nickName);

	//减少道具数量
	int propCount=Glb().userPropLibrary.GetCount();
	_TAG_USERPROP *userProp=NULL;
	for(int i=0;i<propCount;i++)
	{
		userProp=Glb().userPropLibrary.GetAt(i);
		if(userProp->nPropID==pMsg->nPropID)
		{
			userProp->nHoldCount-=1;
            if(userProp->nHoldCount < 0)userProp->nHoldCount = 0;
			//if(userProp->nHoldCount<1)    // PengJiLin, 2010-10-14, 新的道具系统数量可以为0，屏蔽
			//{
			//	Glb().userPropLibrary.RemoveAt(i);
			//	delete userProp;
			//	userProp=NULL;
			//}
			//需要修改本地金币
			break;
		}
	}

	this->m_pGameRoom->PostMessage(WM_USE_BOARDCAST, wparam, lparam);
	return 0;
}


BOOL CLoveSendClassInExe::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此添加专用代码和/或调用基类
	cs.style &= ~WS_VISIBLE;
	cs.style |= WS_CLIPSIBLINGS;
	cs.style |= WS_CLIPCHILDREN;

	return __super::PreCreateWindow(cs);
}

///< 读取等级信息，GetGameOrderName使用
void CLoveSendClassInExe::ReadLevelData()
{
	CString strKey;
	CString strConfig = CBcfFile::GetAppPath();
	strConfig += _T("GameOption.bcf");
	CBcfFile fLevel(strConfig);

	for(int i=1; i<21; i++)
	{
		strKey.Format(_T("Order%d"),i);
		g_PointArray[i] = fLevel.GetKeyVal(_T("ScoreOrderName"),strKey,0);
		strKey = fLevel.GetKeyMemo(_T("ScoreOrderName"),strKey,_T("")); /// 这里对strKey赋值只是借用此变量
		g_LevelNameArray.insert(pair<int,string>(i,strKey.GetBuffer()));
	}
}

///< 获取游戏里面的积分级别
///< @dwPoint玩家积分
///< 玩家积分名称
TCHAR * CLoveSendClassInExe::GetGameOrderName(__int64 dwPoint)
{
	int low = 1;
	int high = 19;
	int mid;
	UINT point;
	TCHAR OrderName[MAX_PATH] = {0};

	dwPoint = dwPoint<0?0:dwPoint; ///积分小于零也在最低级别

	while(low <= high)
	{
		mid = low + (high-low)/2;

		if (dwPoint<g_PointArray[mid])
		{
			high = mid-1;
		}
		else
		{
			low = mid+1;
		}
	}

    // PengJiLin, 2010-8-2, 避免越界处理
    int iFirst = 1;         // Key 从 1 开始
    int iLast = g_LevelNameArray.size();
    int iIndex = low-1;
    if(iIndex < iFirst)
    {
        iIndex = iFirst;
    }
    else if(iIndex > iLast)
    {
        iIndex = iLast;
    }

	map<int,string>::iterator ite = g_LevelNameArray.find(iIndex);

	return (TCHAR*)(*ite).second.c_str();
}

BOOL CLoveSendClassInExe::OnInitDialog()
{
	GetDlgItem(IDC_SNDPLAY)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_SNDPAUSE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_SNDVOLDOWN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_SNDVOLUP)->ShowWindow(SW_HIDE);

	CBcfFile forder( CBcfFile::GetAppPath() + "..\\Order.bcf");
	Glb().m_nPowerOfGold = forder.GetKeyVal("VirtualMoney", "PowerOfGold", 0);

    // 金币显示分隔符配置
    Glb().m_bUseSpace = forder.GetKeyVal("VirtualMoney", "UseSpace", 0);
    CString strSpace = forder.GetKeyVal("VirtualMoney", "SpaceChar", " ");
    memcpy(Glb().m_strSpaceChar, strSpace, sizeof(Glb().m_strSpaceChar)*sizeof(TCHAR));

	int _bit = forder.GetKeyVal("VirtualMoney", "SpaceBits", 3);
	if (_bit == 0) Glb().m_iMaxBit = 18;
	else Glb().m_iMaxBit = 18 + 18 / _bit - 1;

	CLoadFileBmp::OnInitDialog();
	
	ModifyStyle( WS_CAPTION, WS_MINIMIZEBOX|WS_MAXIMIZEBOX, SWP_DRAWFRAME );

	AfxSetResourceHandle(GetModuleHandle(FACE_DLL_NAME));
	m_PlaySound.CreateWnd(this,IDD_NULL);
	AfxSetResourceHandle(GetModuleHandle(NULL));
	m_pGameView->m_showcoredlg.m_ucomtype=GetComType();
	m_pGameView->m_showcoredlg.m_count=m_pGameInfo->uDeskPeople;
	//读取设置参数
	m_pGameInfo->bEnableWatch=(GetProfileInt(TEXT("CanLooking"),FALSE)!=FALSE);
	m_pGameInfo->bEnableSound=(GetProfileInt(TEXT("PlaySound"),TRUE)!=FALSE);
	m_pGameInfo->bEnableBackSound=(GetProfileInt(TEXT("BackSound"),TRUE)!=FALSE);
	m_pGameInfo->bShowUserInfo=(GetProfileInt(TEXT("ShowUserInfo"),FALSE)!=FALSE);  //缺省不显示玩家进出信息

	m_CurpUserInfoClicked=GetMeUserInfo();
	//if(m_Btbt5.m_hWnd)m_Btbt5.ShowWindow(SW_HIDE);
	//if(m_Btbt6.m_hWnd)m_Btbt6.ShowWindow(SW_HIDE);
	//if(m_Btbt7.m_hWnd)m_Btbt7.ShowWindow(SW_HIDE);
	//if(m_Btbt8.m_hWnd)m_Btbt8.ShowWindow(SW_HIDE);
	//if(m_Btbt9.m_hWnd)m_Btbt9.ShowWindow(SW_HIDE);
	if(m_Btbt10.m_hWnd)m_Btbt10.ShowWindow(SW_HIDE);
	CBcfFile bcffile(CBcfFile::GetAppPath() + _T("GameOption.bcf"));
	UINT uMaxType = bcffile.GetKeyVal("FrameControl","max_type",1);
	if (uMaxType != 0)
	{
	m_btRest.ShowWindow(SW_HIDE);
	m_btMax.ShowWindow(SW_SHOW);
	}
	//显示游戏的右窗口
	if (m_pRightFrame)
	{
		m_pRightFrame->ShowWindow(SW_SHOW);
	}

	LoadSkin(false);

	//建立游戏视图
	//HINSTANCE hInstance=AfxGetInstanceHandle();
	//AfxSetResourceHandle(hInstance);
	//m_ADDialog.Create(IDD_FLASH_DLG,this);
	//m_HorSplitBar.InitSplitBar(310,500,true);

	////OutputDebugString("CLoveSendClassInExe::OnInitDialog 3");

	//设置控件
	AfxSetResourceHandle(GetModuleHandle(FACE_DLL_NAME));
	m_UserListDlg.Create(IDD_USER_LIST, this);
	AfxSetResourceHandle(GetModuleHandle(NULL));

	ReadLevelData(); ///读取积分等级配置
	m_pGameInfo->pOrderName = GetGameOrderName;///< 获取游戏里面的积分级别,取消以前在游戏DLL里面读积分等级

	//修改信息框等级与玩家列表中的等级不同
	//m_UserListDlg.InitListCtrl(GetGameOrderNameInExe,m_pGameInfo->uComType);
	if( m_pGameInfo->pOrderName == NULL )
	{
		MessageBox("m_pGameInfo->pOrderName == NULL");
		return FALSE;
	}
	m_UserListDlg.InitListCtrl(GetPointOrderName,m_pGameInfo->uComType);
	
	m_pGameView->Create(NULL,NULL,WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_VISIBLE,CRect(0,0,0,0),this,ID_GAME_VIEW,NULL);//这行语句超慢

	if (m_pGameInfo->dwRoomRule & GRR_NOTCHEAT)
	{
		m_pGameView->m_bShowRight = false;
	}
	UINT showright = (m_pGameView->m_bShowRight) ? SW_SHOW : SW_HIDE;
	showright = SW_HIDE;

	m_TalkMessage.Init();

	m_TalkMessage.SetTextMode(TRANSPARENT);
	m_TalkMessage.IsGameMessage();						//房间的聊天消息，用来保存不同的ini文件，有别于大厅

	PostMessageA(WM_RELOAD_TALKINI);

	////////////////////
	if (CUserPower::CanLookIP(m_pGameInfo->dwMasterPower)) 
		m_UserListDlg.EnableIPView(true);
	m_InputMessage.LimitText(CUserPower::CanSendMuch(m_pGameInfo->dwGamePower)?MAX_TALK_LEN:NORMAL_TALK_LEN);
	//设置广告

	//////////////////////////////////////////////////////////////////////////
	//读取窗口广告地址
#ifdef _USE_IE_AD_
	CString s=CBcfFile::GetAppPath () + "..\\";//////本地路径
	CBcfFile f( s + "bzgame.bcf");
	szAdvUrl=f.GetKeyVal (Glb().m_key,"gameadvertise","");
	if(szAdvUrl=="")
		m_ie.MoveWindow(-10,-10,5,5);
	else
	{
		CString surl;
		if(szAdvUrl.Find("?")!=-1)
			surl.Format("%s&roomid=%d",szAdvUrl,m_pGameInfo->uRoomID);
		else
			surl.Format("%s?roomid=%d",szAdvUrl,m_pGameInfo->uRoomID);
		if (m_pGameView->m_bShowRight)
		{
			m_ie.Navigate(surl,0,0,0,0);
		}
	}
#else
	m_ie.ShowWindow(SW_HIDE);
#endif

	//////////////////////////////////////////////////////////////////////////	
	if(GetComType()==TY_MATCH_GAME)
	{
		m_InputMessage.EnableWindow(false);
		m_BtExpression.EnableWindow(false);
		m_BtSend.EnableWindow(false);
	}	
	//聊天框
	if (m_pGameView->m_bShowRight)
	{
		m_TalkMessage.SetFocus();
	}

	//设置控件
	m_bInit = true;
	m_ToolTip.Create(this);
	m_ToolTip.Activate(TRUE);
	m_ToolTip.AddTool(GetDlgItem(IDC_GAME_BTN_01), TEXT("设置游戏相关选项"));
	m_ToolTip.AddTool(GetDlgItem(IDC_GAME_BTN_02), TEXT("购买道具"));
	m_ToolTip.AddTool(GetDlgItem(IDC_GAME_BTN_03), TEXT("充值"));
	m_ToolTip.AddTool(GetDlgItem(IDC_GAME_BTN_04), TEXT("打开主页"));
	m_ToolTip.AddTool(GetDlgItem(IDC_GO_AHEAD),TEXT("向前查看道具"));
	//m_ToolTip.AddTool(GetDlgItem(IDC_GO_NEXT),TEXT("向后查看道具"));
	m_ToolTip.AddTool(GetDlgItem(IDC_EXPRESSION),TEXT("选择表情头像"));
	m_ToolTip.AddTool(GetDlgItem(IDC_SEND),TEXT("发送聊天信息"));
	m_ToolTip.AddTool(GetDlgItem(IDC_BUTTON_PROP1), "VIP道具");
	m_ToolTip.AddTool(GetDlgItem(IDC_BUTTON_PROP2), LPSTR_TEXTCALLBACK);
	m_ToolTip.AddTool(GetDlgItem(IDC_BUTTON_PROP3), LPSTR_TEXTCALLBACK);
    m_ToolTip.AddTool(GetDlgItem(IDC_BUTTON_PROP4), LPSTR_TEXTCALLBACK);
    m_ToolTip.AddTool(GetDlgItem(IDC_BUTTON_PROP5), LPSTR_TEXTCALLBACK);
	m_ToolTip.AddTool(GetDlgItem(IDC_BUTTON_PROP6), LPSTR_TEXTCALLBACK);
	m_ToolTip.AddTool(GetDlgItem(IDC_BUTTON_PROP7), LPSTR_TEXTCALLBACK);
	m_ToolTip.AddTool(GetDlgItem(IDC_BUTTON_PROP8), LPSTR_TEXTCALLBACK);
	m_ToolTip.AddTool(GetDlgItem(IDC_BUTTON_PROP9), LPSTR_TEXTCALLBACK);
	m_ToolTip.AddTool(GetDlgItem(IDC_BUTTON_PROP10), LPSTR_TEXTCALLBACK);

	//wushuqun 音乐播放按钮的提示

	//m_ToolTip.AddTool(GetDlgItem(IDC_SNDPLAY),TEXT("播放音乐")); 
	//m_ToolTip.AddTool(GetDlgItem(IDC_SNDPAUSE),TEXT("停止播放音乐")); 
	//m_ToolTip.AddTool(GetDlgItem(IDC_SNDVOLDOWN),TEXT("调小声音")); 
	//m_ToolTip.AddTool(GetDlgItem(IDC_SNDVOLUP),TEXT("调大声音"));

    // PengJiLin, 2010-10-12, 商场道具功能
    m_ToolTip.AddTool(GetDlgItem(IDC_GAME_BTN_06), TEXT("向前查看道具"));
    m_ToolTip.AddTool(GetDlgItem(IDC_GAME_BTN_07), TEXT("向后查看道具"));


    // PengJilin, 2010-9-30
    m_ToolTip.AddTool(GetDlgItem(IDC_GAME_BTN_05),TEXT("申请VIP"));

	LoadSoundList();

	OnInitPropList();


	if (uMaxType == 1) //普通最大化
	{
		OnBnClickedMax();
	}
	else if (uMaxType == 2) //最大采用改变屏幕分辨率的方式
	{
		OnBnClickedRest();
	}

	
	m_UserListDlg.ShowWindow(showright);
	m_TalkMessage.ShowWindow(showright);
	m_Btbt1.ShowWindow(SW_SHOW);
	m_Btbt2.ShowWindow(SW_HIDE);						//modify by wyl   11-5-12
	m_Btbt3.ShowWindow(SW_HIDE);
	m_Btbt4.ShowWindow(SW_HIDE);
	m_Btbt5.ShowWindow(SW_HIDE);						//end
			
	m_BtPropGoAhead.ShowWindow(SW_HIDE);
	m_BtPropGoNext.ShowWindow(SW_HIDE);
	m_BtProp1.ShowWindow(showright);
	m_BtProp2.ShowWindow(showright);
	m_BtProp3.ShowWindow(showright);
	m_BtProp4.ShowWindow(showright);
	m_BtProp5.ShowWindow(showright);
	m_BtProp6.ShowWindow(showright);
	m_BtProp7.ShowWindow(showright);
	m_BtProp8.ShowWindow(showright);
	m_BtProp9.ShowWindow(showright);
	m_BtProp10.ShowWindow(showright);
	m_BtExpression.ShowWindow(showright);
	m_InputMessage.ShowWindow(showright);
	m_BtSend.ShowWindow(showright);

	//向平台请求获取同桌用户资料
	BYTE bMeDeskStation=m_pGameInfo->uisMeUserInfo.GameUserInfo.bDeskNO;
	if (bMeDeskStation != 255)
	{
		GetOnLineUser( bMeDeskStation );
	}
	m_Btbt1.SetFocus();

	//为排队机添加
	if ((m_pGameInfo->dwRoomRule & GRR_NOTCHEAT) > 0)//(IsQueueGameRoom())
	{
		m_pGameView->m_bShowRight = false;
// 		m_BtHide.EnableWindow(FALSE);
// 		m_BtHide.ShowWindow(SW_HIDE);
		
		OnBnClickedHide();
	}
	//end of 为排队机添加
	UpdateGameTitle();

	
    // PengJiLin, 2010-6-11, 因为鸥朴项目将设置按钮隐藏了，
    // 所以固定设置为TRUE。必须放在最后面设置，避免成员没有初始化
    BYTE byType = m_UserListDlg.m_UserList.GetUserListDefineType();
    switch(byType)
    {
    case 1:
        m_pGameInfo->bEnableWatch = true;
        m_pGameInfo->bEnableSound = true;
        m_pGameInfo->bEnableBackSound = true;
        m_pGameInfo->bShowUserInfo = false;     //PengJiLin, 2010-10-15, 缺省不显示玩家进出信息
        break;
    default:
        break;
    }

    // PengJiLin, 2010-9-27, 道具显示数字
    m_BtProp1.SetTextColor(RGB(255, 255, 255), true);
    m_BtProp2.SetTextColor(RGB(255, 255, 255), true);
    m_BtProp3.SetTextColor(RGB(255, 255, 255), true);
    m_BtProp4.SetTextColor(RGB(255, 255, 255), true);
    m_BtProp5.SetTextColor(RGB(255, 255, 255), true);
    m_BtProp6.SetTextColor(RGB(255, 255, 255), true);
    m_BtProp7.SetTextColor(RGB(255, 255, 255), true);
    m_BtProp8.SetTextColor(RGB(255, 255, 255), true);
	m_BtProp9.SetTextColor(RGB(255, 255, 255), true);
	m_BtProp10.SetTextColor(RGB(255, 255, 255), true);
    
    m_Btbt5.ShowWindow(SW_HIDE);
    m_Btbt6.ShowWindow(showright);
    m_Btbt7.ShowWindow(showright);


	//获取原窗口尺寸
	m_nWiondwMetricsX = ::GetSystemMetrics(SM_CXSCREEN); 
	m_nWiondwMetricsY = ::GetSystemMetrics(SM_CYSCREEN);

	CRect rect;
	GetClientRect(&rect);
	FixControlStation(rect.Width(),rect.Height());
	m_pGameView->UpdateViewFace(NULL);

	return false;
}
///切换最大化与还原
void CLoveSendClassInExe::SwitchSize()
{
}
//获取数值
UINT CLoveSendClassInExe::GetProfileInt(TCHAR * szEntry, int iDefault)
{
	return AfxGetApp()->GetProfileInt(m_pGameInfo->szGameName,szEntry,iDefault);
}

///011获取字符串
CString CLoveSendClassInExe::GetProfileString(TCHAR * szEntry, TCHAR * szDefault)
{
	return AfxGetApp()->GetProfileString(m_pGameInfo->szGameName,szEntry,szDefault);
}

//写入数值
BOOL CLoveSendClassInExe::WriteProfileInt(TCHAR * szEntry, int nValue)
{
	return AfxGetApp()->WriteProfileInt(m_pGameInfo->szGameName,szEntry,nValue);
}

//获取字符串
BOOL CLoveSendClassInExe::WriteProfileString(TCHAR * szEntry, TCHAR * szValue)
{
	return AfxGetApp()->WriteProfileString(m_pGameInfo->szGameName,szEntry,szValue);
}

/// 银行面板中取款
LRESULT CLoveSendClassInExe::OnBankCheckOut(WPARAM wparam,LPARAM lparam)
{
	if (lparam != sizeof(MSG_GR_S_CheckMoney))
	{
		return E_FAIL;
	}
	MSG_GR_S_CheckMoney *pCheckMoney = (MSG_GR_S_CheckMoney *)wparam;

	//m_TCPSocket->SendData(&checkMoney,sizeof(checkMoney),MDM_GR_MONEY,ASS_GR_CHECK_OUT,0);
	SendGameData(pCheckMoney, sizeof(MSG_GR_S_CheckMoney), MDM_GR_MONEY,ASS_GR_CHECK_OUT_INGAME,0);
	delete pCheckMoney;
	pCheckMoney = NULL;
	return 0;
}

//发送积分
LRESULT CLoveSendClassInExe::OnGetPoint(WPARAM wparam,LPARAM lparam)
{
	TCHAR url[300];
	wsprintf(url,"%sapp/huodong.asp",Glb().m_CenterServerPara.m_strWebRootADDR);
	//wsprintf(url,"%schongzhi.%s?roomid=%d&un=%s",Glb().m_CenterServerPara.m_strHomeADDR,Glb().m_urlpostfix,m_pGameInfo->uRoomID,m_pGameInfo->uisMeUserInfo.GameUserInfo.szName);
	ShellExecute(NULL,"open",url,NULL,NULL,SW_MAXIMIZE|SW_SHOW);
	return 0;
	//	if((int)lparam != 0)
	//		return 0;
	MSG_GR_S_GetPoint getpoint;
	::memset(&getpoint,0,sizeof(getpoint));
	getpoint.dwPoint = (int)wparam;
	SendGameData(&getpoint, sizeof(getpoint), MDM_GR_POINT, ASS_GR_GET_POINT, 0);
	return 0;
}

// 载入皮肤 
void CLoveSendClassInExe::LoadSkin(bool bHide)
{
	int r,g,b;
	CString s=CBcfFile::GetAppPath ()+"..\\";//;/////本地路径
	TCHAR path[MAX_PATH];
	//s += m_iniFile;
	//百家乐 add by wlr 20090716
	CFileFind	fileFind;			//查找文件
	bool		bFileExist=false;	//文件是否存在，缺省为不存在
	wsprintf(path,".\\config\\" + m_skinmgr.GetGameSkinBcfFileName()); /// add by wxx 

	bFileExist=fileFind.FindFile(path);
	fileFind.Close();
	if (bFileExist)
	{
		m_iniFile = path;   ///add by wxx
		s = "";
	}

	CString strBcfFilename = m_skinmgr.GetGameSkinBcfFileName();
	if (CBcfFile::IsFileExist(CBcfFile::GetAppPath() + "config\\" + strBcfFilename))
		m_iniFile= CBcfFile::GetAppPath() + "config\\" + strBcfFilename;
	else
		m_iniFile= CBcfFile::GetAppPath()+"..\\config\\" + strBcfFilename;

	//百家乐 end of add by wlr 20090716
	CBcfFile f( m_iniFile);

	CString key=TEXT("config");
	
	CString skinfolder;
	skinfolder=f.GetKeyVal(key,"skinfolder","image\\client\\skin1\\game\\");
	skinfolder=s+skinfolder;
	DebugPrintf("s=%s",s);
	DebugPrintf("skinfolder=%s",skinfolder);
	skinfolder.Replace("Image","image");
	CString skinHall = CBcfFile::GetAppPath()+"..\\image\\client\\skin1\\";

	CString ph;
	//背景图
	ph=f.GetKeyVal(key,"backimages","image\\client\\skin1\\game\\backimages.bmp");

	//m_pGameView->m_bkimgs.SetLoadInfo(ph.GetBuffer(),true);
	m_backImage.SetLoadInfo(ph.GetBuffer(),true);
	//加载四个框
	ZeroMemory(path,sizeof(path));
	//wsprintf(path,"%s\\game_tc.bmp",skinfolder);
	GET_FILENAME(path,"%s\\game_tc.bmp",skinfolder);
	m_tc.SetLoadInfo(path,true);//CGameImageLink::m_bAutoLock);
	GET_FILENAME(path,"%s\\game_bc.bmp",skinfolder);
	m_bc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	GET_FILENAME(path,"%s\\game_rc.bmp",skinfolder);
	m_rc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	GET_FILENAME(path,"%s\\GameWnd.png",skinfolder);
	m_lc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	//四角
	GET_FILENAME(path,"%s\\game_lt.bmp",skinfolder);
	m_lt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	GET_FILENAME(path,"%s\game_lb.bmp",skinfolder);
	m_lb.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	GET_FILENAME(path,"%s\\game_rt.bmp",skinfolder);
	m_rt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	GET_FILENAME(path,"%s\\game_rb.bmp",skinfolder);
	m_rb.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	//加载右边
	GET_FILENAME(path,"%s\\game_right_tc.bmp",skinfolder);
	m_right_tc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	GET_FILENAME(path,"%s\\game_right_bc.bmp",skinfolder);
	m_right_bc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	GET_FILENAME(path,"%s\\game_right_lc.bmp",skinfolder);
	m_right_lc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	GET_FILENAME(path,"%s\\game_right_rc.bmp",skinfolder);
	m_right_rc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	//四个角
	GET_FILENAME(path,"%s\\game_right_lt.bmp",skinfolder);
	m_right_lt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	GET_FILENAME(path,"%s\\game_right_lb.bmp",skinfolder);
	m_right_lb.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	GET_FILENAME(path,"%s\\game_right_rt.bmp",skinfolder);
	m_right_rt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	GET_FILENAME(path,"%s\\game_right_rb.bmp",skinfolder);
	m_right_rb.SetLoadInfo(path,CGameImageLink::m_bAutoLock);

	GET_FILENAME(path,"%s\\UserInfo_bk.bmp",skinfolder);
	m_UserInfo_bk.SetLoadInfo(path,CGameImageLink::m_bAutoLock);

	//中间
	//	wsprintf(path,"%s\\mid_c.bmp",skinfolder);
	//	m_iMid.SetLoadInfo(path,CGameImageLink::m_bAutoLock);

    // PengJiLin, 2010-9-28, 按钮背景图
    GET_FILENAME(path,"%s\\probBack.bmp",skinfolder);
    m_propBackImage.SetLoadInfo(path,CGameImageLink::m_bAutoLock);

    GET_FILENAME(path,"%s\\probBack2.bmp",skinfolder);
    m_propBackImage2.SetLoadInfo(path,CGameImageLink::m_bAutoLock);

	//
	ph=f.GetKeyVal(key,"backimageh",m_skinmgr.GetGameSkinPath() + "backimageh.bmp");


	m_pGameView->m_bkimgh.SetLoadInfo(ph.GetBuffer(),true);
	m_pGameView->m_ShowType=f.GetKeyVal(key,"showtype",0);
    if (!bHide)
	    m_pGameView->m_bShowRight=f.GetKeyVal(key,"showright",1);

	m_titlex=f.GetKeyVal(key,"titlex",12);
	m_titley=f.GetKeyVal(key,"titley",7);
	r=f.GetKeyVal(key,"titler",255);
	g=f.GetKeyVal(key,"titleg",234);
	b=f.GetKeyVal(key,"titleb",0);
	m_titlecr=RGB(r,g,b);
	m_logx=f.GetKeyVal(key,"flogx",198);
	m_logy=f.GetKeyVal(key,"flogy",40);
	m_u1x=f.GetKeyVal(key,"u1x",110);
	m_u1y=f.GetKeyVal(key,"u1y",64);
	m_u1w=f.GetKeyVal(key,"u1w",75);
	m_u1h=f.GetKeyVal(key,"u1h",18);
	m_u2x=f.GetKeyVal(key,"u2x",189);
	m_u2y=f.GetKeyVal(key,"u2y",158);
	m_u2w=f.GetKeyVal(key,"u2w",86);
	m_u2h=f.GetKeyVal(key,"u2h",18);
// 	m_vlistx=f.GetKeyVal(key,"vlistx",0);
// 	m_pGameView->m_vlistx=m_vlistx;

	int iWindowsWidth = GetSystemMetrics(SM_CXSCREEN);
	int iWindowsHeight = GetSystemMetrics(SM_CYSCREEN);
	if (m_pRightFrame)
	{
		if (iWindowsWidth >= 1280)
		{
			m_vlistx = m_pRightFrame->m_uMaxWidth;
			m_pGameView->m_vlistx = m_vlistx;
		}
		else
		{
			m_vlistx = m_pRightFrame->m_uMinWidth;
			m_pGameView->m_vlistx = m_vlistx;
		}
	}

	m_vlisty=f.GetKeyVal(key,"vlisty",0);

	r=f.GetKeyVal(key,"userr",0);
	g=f.GetKeyVal(key,"userg",0);
	//wushuqun 2009.6.10
	b=f.GetKeyVal(key,"userb",0);
	m_usercr=RGB(r,g,b);
	/////////////////////////////
	//Kylin 20090205 增加用户信息背景
	r=f.GetKeyVal(key,"userbkr",0);
	g=f.GetKeyVal(key,"userbkg",0);
	b=f.GetKeyVal(key,"userbkb",0);
	m_userbkcr=RGB(r,g,b);
	/////////////////////////////

	r=f.GetKeyVal(key,"bkr",45);
	g=f.GetKeyVal(key,"bkg",80);
	b=f.GetKeyVal(key,"bkb",133);
	m_colorresource.m_crTalkBkColor=RGB(r,g,b);
	r=f.GetKeyVal(key,"namer",255);
	g=f.GetKeyVal(key,"nameg",255);
	b=f.GetKeyVal(key,"nameb",255);
	m_colorresource.m_crTalkNameColor=RGB(r,g,b);
	r=f.GetKeyVal(key,"systemr",255);
	g=f.GetKeyVal(key,"systemg",255);
	b=f.GetKeyVal(key,"systemb",255);
	m_colorresource.m_crSystemTxColor=RGB(r,g,b);
	r=f.GetKeyVal(key,"systembdr",255);
	g=f.GetKeyVal(key,"systembdg",0);
	b=f.GetKeyVal(key,"systembdb",0);
	m_colorresource.m_crSystemBdColor=RGB(r,g,b);
	r=f.GetKeyVal(key,"normalr",255);
	g=f.GetKeyVal(key,"normalg",255);
	b=f.GetKeyVal(key,"normalb",255);
	m_colorresource.m_crNormalTxColor=RGB(r,g,b);
	////////////////////////////////////
	////Kylin 20090117 聊天信息颜色设置
	r=f.GetKeyVal(key,"talkr",255);
	g=f.GetKeyVal(key,"talkg",255);
	b=f.GetKeyVal(key,"talkb",255);
	m_colorresource.m_crTalkTxColor=RGB(r,g,b);	
	////////////////////////////////////
	//框架颜色
	r=f.GetKeyVal(key,"framecolorr",150);
	g=f.GetKeyVal(key,"framecolorg",191);
	b=f.GetKeyVal(key,"framecolorb",239);
	m_FrameColor = RGB(r,g,b);		
	//新闻
	r=f.GetKeyVal(key,"newscolorr",255);
	g=f.GetKeyVal(key,"newscolorg",255);
	b=f.GetKeyVal(key,"newscolorb",255);
	m_colorresource.m_crNewsTxColor=RGB(r,g,b);		
	r=f.GetKeyVal(key,"senttextr",0);
	g=f.GetKeyVal(key,"senttextg",0);
	b=f.GetKeyVal(key,"senttextb",255);
	m_colorresource.m_crSentTxColor=RGB(r,g,b);	
	r=f.GetKeyVal(key,"receivedtextr",255);
	g=f.GetKeyVal(key,"receivedtextg",0);
	b=f.GetKeyVal(key,"receivedtextb",0);
	m_colorresource.m_crReceivedTxColor=RGB(r,g,b);	

	//m_colorresource.m_crNormalTxColor=RGB(r,g,b);
	SetFillColor(m_colorresource.m_crTalkBkColor);	
	m_MessageHandle.SetColor(m_colorresource);

	CGameImage img;
	HRGN hRgn;
	int w = 18;
	int h = 18;
	GET_FILENAME(path,"%sgame_left_face.bmp",skinfolder);
	m_BtExpression.LoadButtonBitmap(path,false);
	img.SetLoadInfo(path,true);
	hRgn=AFCBmpToRgn(img,RGB(255,0,255),RGB(1,1,1));
	if(m_BtExpression)
	{
		if(hRgn)
			m_BtExpression.SetWindowRgn(hRgn,true);
		m_BtExpression.SetWindowPos(NULL,0,0,img.GetWidth()/4,img.GetHeight(),SWP_NOZORDER|SWP_NOMOVE);
	}

	//设置颜色
	GET_FILENAME(path,"%sgame_setcolor.bmp",skinfolder);
	m_BtSetColor.LoadButtonBitmap(path,false);
	img.SetLoadInfo(path,true);
	hRgn=AFCBmpToRgn(img,RGB(255,0,255),RGB(1,1,1));
	if(m_BtExpression)
	{
		if(hRgn)
			m_BtSetColor.SetWindowRgn(hRgn,true);
		m_BtSetColor.SetWindowPos(NULL,0,0,img.GetWidth()/4,img.GetHeight(),SWP_NOZORDER|SWP_NOMOVE);
	}

	GET_FILENAME(path,"%sgame_left_send.bmp",skinfolder);
	m_BtSend.LoadButtonBitmap(path,false);
	img.SetLoadInfo(path,true);
	if(m_BtSend.m_hWnd)
	{
		m_BtSend.SetWindowPos(NULL,0,0,img.GetWidth()/4,img.GetHeight(),SWP_NOZORDER|SWP_NOMOVE);
		hRgn=AFCBmpToRgn(img,RGB(255,0,255),RGB(1,1,1));
		if(hRgn)
			m_BtSend.SetWindowRgn(hRgn,true);
	}

    if (!bHide)
    {
	    GET_FILENAME(path,"%shide_bt.bmp",skinfolder);
    }
    else
    {
        GET_FILENAME(path,"%sshow_bt.bmp",skinfolder);
    }

	m_BtHide.LoadButtonBitmap(path,false);
	img.SetLoadInfo(path,true);
	if(m_BtHide.m_hWnd)
	{
		m_BtHide.SetWindowPos(NULL,0,0,img.GetWidth()/4,img.GetHeight(),SWP_NOZORDER|SWP_NOMOVE);
		hRgn=AFCBmpToRgn(img,RGB(255,0,255),RGB(1,1,1));
		if(hRgn)
			m_BtHide.SetWindowRgn(hRgn,true);
	}

    if (!bHide)
    {
        m_HidebtPath = path;
        GET_FILENAME(path,"%sshow_bt.bmp",skinfolder);
        m_ShowbtPath = path;
    }
    else
    {
        m_ShowbtPath = path;
        GET_FILENAME(path,"%shide_bt.bmp",skinfolder);
        m_HidebtPath = path;
    }

	w = 20;
	h = 20;
	CBcfFile bcffile(CBcfFile::GetAppPath() + _T("GameOption.bcf"));
	UINT uMaxType = bcffile.GetKeyVal("FrameControl","max_type",1);
	GET_FILENAME(path,"%sgame_min_bt.bmp",skinfolder);
	m_BtMin.LoadButtonBitmap(path,false);	
	if (0 != uMaxType)
	{
	GET_FILENAME(path,"%sgame_max_bt.bmp",skinfolder);
	m_btMax.LoadButtonBitmap(path,false);	
	GET_FILENAME(path,"%sgame_rest_bt.bmp",skinfolder);
	m_btRest.LoadButtonBitmap(path,false);	

	m_btRest.ShowWindow(SW_SHOW);
	m_btMax.ShowWindow(SW_HIDE);
	}
	else
	{
		m_btRest.ShowWindow(SW_HIDE);
		m_btMax.ShowWindow(SW_HIDE);
	}

	GET_FILENAME(path,"%sgame_close_bt.bmp",skinfolder);
	m_BtGameQuit.LoadButtonBitmap(path,false);

	GET_FILENAME(path,"%sgame_bt01.bmp",skinfolder);
	m_Btbt1.LoadButtonBitmap(path,false);
	img.SetLoadInfo(path,true);

	CGameImage sndImg;
	sndImg.SetLoadInfo(path,true);
	m_sndBtnCx=sndImg.GetWidth()/4;
	m_sndBtnCy=sndImg.GetHeight();


	if(m_Btbt1.m_hWnd)
	{
		hRgn=AFCBmpToRgn(img,RGB(255,0,255),RGB(1,1,1));
		if(hRgn)
			m_Btbt1.SetWindowRgn(hRgn,true);
	}

	GET_FILENAME(path,"%sgame_bt02.bmp",skinfolder);
	m_Btbt2.LoadButtonBitmap(path,false);
	img.SetLoadInfo(path,true);
	if(m_Btbt2.m_hWnd)
	{
		hRgn=AFCBmpToRgn(img,RGB(255,0,255),RGB(1,1,1));
		if(hRgn)
			m_Btbt2.SetWindowRgn(hRgn,true);
	}

	GET_FILENAME(path,"%sgame_bt03.bmp",skinfolder);
	m_Btbt3.LoadButtonBitmap(path,false);
	img.SetLoadInfo(path,true);
	if(m_Btbt3.m_hWnd)
	{
		hRgn=AFCBmpToRgn(img,RGB(255,0,255),RGB(1,1,1));
		if(hRgn)
			m_Btbt3.SetWindowRgn(hRgn,true);
	}

	GET_FILENAME(path,"%sgame_bt04.bmp",skinfolder);
	m_Btbt4.LoadButtonBitmap(path,false);
	img.SetLoadInfo(path,true);
	if(m_Btbt4.m_hWnd)
	{
		hRgn=AFCBmpToRgn(img,RGB(255,0,255),RGB(1,1,1));
		if(hRgn)
			m_Btbt4.SetWindowRgn(hRgn,true);
	}

    // PengJiLin, 2010-9-30, 蓝钻贵族
    GET_FILENAME(path,"%sgame_btbluediamond.bmp",skinfolder);
    m_Btbt5.LoadButtonBitmap(path,false);
    img.SetLoadInfo(path,true);
    if(m_Btbt5.m_hWnd)
    {
        hRgn=AFCBmpToRgn(img,RGB(255,0,255),RGB(1,1,1));
        if(hRgn)
            m_Btbt5.SetWindowRgn(hRgn,true);
    }

	sndPlayX=f.GetKeyVal(key,"SndPlayX",100);
	sndPlayY=f.GetKeyVal(key,"SndPlayY",-200);

	sndPauseX=f.GetKeyVal(key,"SndPauseX",100);
	sndPauseY=f.GetKeyVal(key,"SndPauseY",-200);

	sndVolDownX=f.GetKeyVal(key,"SndVolDownX",100);
	sndVolDownY=f.GetKeyVal(key,"SndVolDownY",-200);

	sndVolUpX=f.GetKeyVal(key,"SndVolUpX",100);
	sndVolUpY=f.GetKeyVal(key,"SndVolUpY",-200);

	if(m_Btbt10.m_hWnd)m_Btbt10.ShowWindow(SW_HIDE);

	m_MessageHandle.SetColor(m_colorresource);

    char wpath[MAX_PATH] = {0};
	GET_FILENAME(wpath,"%s\\game_signal.bmp",skinfolder);

	//AfxMessageBox(CW2A(wpath));
#ifndef BZ_ZLIB
	pSingnalimg = Image::FromFile(CA2W(wpath));
#else
	CUnZipRes unZipRes;
	IStream * pStream=NULL;
	//char * pchar=s.GetBuffer(s.GetLength()+1);
	pStream=unZipRes.LoadFileResFromZip(wpath);
	//DebugPrintf("Load zip %s",wpath);
	pSingnalimg=new Image(pStream);
	if(pStream!=NULL)
	{
		pStream->Release();
		pStream=NULL;
	}
	
#endif
	
	//等待区底图 yjj 090227

	// ----- 调用指定游戏的配置文件 -------- dxh
	char GameCode[20];
	::memset(GameCode,0,sizeof(GameCode));
	GetGameCode(GameCode);
	//if (str == "11901800"
	//	|| str == "10901800"
	//	|| str == "30501800")
	TCHAR szKey[10];
	CString sPath=CBcfFile::GetAppPath();
	CBcfFile fsr( sPath + "..\\SpecialRule.bcf");
	wsprintf(szKey, "%d", m_pGameInfo->uNameID);
	int iResult = fsr.GetKeyVal (_T("BJLType"), szKey, 0);

	m_UserListDlg.m_UserList.SetIniFile(m_iniFile, TRUE);
	if (iResult>0)
	{
		m_btMax.ShowWindow(SW_HIDE);
	}
	// ----- 调用指定游戏的配置文件 --------

    // PengJiLin, 2010-10-13, 道具向前向后按钮图
    {
        CGameImage	img;
        HRGN	hRgn;
        CString s=CINIFile::GetAppPath ();/////本地路径

        CString skinHall = CBcfFile::GetAppPath()+"..\\" + m_skinmgr.GetSkinPath();
        CBcfFile f( m_iniFile);

        CString key=TEXT("config");
        TCHAR path[MAX_PATH];
        CString skinfolder;
        skinfolder=f.GetKeyVal(key,"skinfolder","image\\game\\");
        TCHAR	szPropImagePath[MAX_PATH];
        wsprintf(szPropImagePath,TEXT("%s\\prop\\"),skinfolder);
		CString strGameSkinPath = CBcfFile::GetAppPath() + "config\\" + m_skinmgr.GetGameSkinBcfFileName();
       

		if (!CBcfFile::IsFileExist(strGameSkinPath))
        {
            wsprintf(szPropImagePath, TEXT("%s\\game\\prop\\"), skinHall );
        }

		CRect rect;
		GetClientRect(&rect);
		FixControlStation(rect.Width(),rect.Height());
		m_pGameView->UpdateViewFace(NULL);
        //////////////////////////////////////////////////////////////////////////
        //2008-08-11
        //设置向前，向后按钮的图片
        m_curPropPage=0;
        TCHAR szTemp[MAX_PATH];

		m_Btbt6.LoadButtonBitmap(szTemp, false);
        img.SetLoadInfo(szTemp, true);
        hRgn =AFCBmpToRgn(img, RGB(255, 0, 255), RGB(1,1,1));
        if( hRgn )
            m_Btbt6.SetWindowRgn(hRgn, true);
        m_Btbt6.EnableWindow(FALSE);	//初始化时前翻页按钮无效


        m_Btbt7.LoadButtonBitmap(szTemp, false);
        img.SetLoadInfo(szTemp, true);
        hRgn =AFCBmpToRgn(img, RGB(255, 0, 255), RGB(1,1,1));
        if( hRgn )
            m_Btbt7.SetWindowRgn(hRgn, true);
        m_Btbt7.EnableWindow(FALSE);	//初始化时前翻页按钮无效

#ifdef _USE_PROP_PAGE_SW_
        wsprintf(szTemp, TEXT("%sPropPrev.bmp"), szPropImagePath);
        m_BtPropGoAhead.LoadButtonBitmap(szTemp, false);
        img.SetLoadInfo(szTemp, true);
        hRgn =AFCBmpToRgn(img, RGB(255, 0, 255), RGB(1,1,1));
        if( hRgn )
            m_BtPropGoAhead.SetWindowRgn(hRgn, true);
        m_BtPropGoAhead.EnableWindow(FALSE);	//初始化时前翻页按钮无效

        wsprintf(szTemp, TEXT("%sPropNext.bmp"), szPropImagePath);
        m_BtPropGoNext.LoadButtonBitmap(szTemp, false);
        img.SetLoadInfo(szTemp, true);
        hRgn =AFCBmpToRgn(img, RGB(255, 0, 255), RGB(1,1,1));
        if( hRgn )
            m_BtPropGoNext.SetWindowRgn(hRgn, true);
        wsprintf(m_szPropEmptyImage, TEXT("%sPropEmpty.bmp"), szPropImagePath);
#endif
	}
}

LRESULT CLoveSendClassInExe::OnLoadSkin(WPARAM wparam, LPARAM lparam)
{
	//此函数暂时不被调用
	LoadSkin(false);

	//m_UserListDlg.m_UserList.LoadSkin();

	CRect ClientRect;
	GetClientRect(&ClientRect);
	int cx,cy;
	cx=ClientRect.Width();
	cy=ClientRect.Height();
	OnSize(0,cx,cy);
	return true;
}
LRESULT CLoveSendClassInExe::OnReLoadTalkINI(WPARAM wparam, LPARAM lparam)
{
	//清除原来内容
	m_InputMessage.ResetContent();
	//从talkInGame.ini文件中读取聊天条
	//modify by wlr 20090716
	//CString s="..\\"; CINIFile::GetAppPath ();/////本地路径
	//CBcfFile f( s + "talkInGame.bcf");
	CBcfFile f("talkInGame.bcf");
	//end of modify by wlr 20090716

	CString key="BZW_TALK";
	//	TCHAR path[MAX_PATH];
	CString szTalk;
	CString szNumber;
	m_nTalkDefaultCount=0;
	do
	{
		szNumber.Format("%d",++m_nTalkDefaultCount);
		szTalk=f.GetKeyVal(key,"T"+szNumber,"");
		if(!szTalk.IsEmpty())
			m_InputMessage.AddString(szTalk);
	}while(!szTalk.IsEmpty());
	m_nTalkDefaultCount--;
	return 0;
}
//更新标题
void CLoveSendClassInExe::UpdateGameTitle()
{
	if (m_pGameInfo!=NULL)
	{
		CString strBuffer;

		//strBuffer.Format(TEXT("%s %d.%02d"),m_pGameInfo->szGameName,HIWORD(m_pGameInfo->dwGameMSVer),LOWORD(m_pGameInfo->dwGameLSVer));
		strBuffer.Format(TEXT("%s %d.%02d"),m_szGameRoomName,HIWORD(m_pGameInfo->dwGameMSVer),LOWORD(m_pGameInfo->dwGameLSVer));
		
		if (m_pGameInfo->uisMeUserInfo.GameUserInfo.bDeskNO!=255)
		{
			CString strDeskStation;
			// PengJiLin, 2010-5-19, 比赛和排队机场屏蔽桌子号
			if(IsQueueGameRoom() || m_pGameInfo->uComType ==TY_MATCH_GAME ) 
				strDeskStation = TEXT(" - 某游戏桌");
			else
				strDeskStation.Format(TEXT(" - [%ld]号游戏桌"),m_pGameInfo->uisMeUserInfo.GameUserInfo.bDeskNO+1);
			strBuffer+=strDeskStation;
		}
		SetWindowTitle(strBuffer);
		m_pGameView->SetViewTitle(strBuffer);
	}
	return;
}
/// 根据全局变量中的加密方式，取得加密后的密码字符串可能是sha可能是MD5
/// 函数里不对指针的合法性作判断
/// @param szMD5Pass 加密后的字符串
/// @param szSrcPass 源字符串
/// @return 加密后的字符串指针
TCHAR *CLoveSendClassInExe::GetCryptedPasswd(TCHAR *szMD5Pass, TCHAR *szSrcPass)
{
	CenterServerMsg& par=Glb().m_CenterServerPara;
	//md5方式
	if( par.m_nEncryptType == 1)
	{
		unsigned char szMDTemp[16];
		MD5_CTX Md5;
		Md5.MD5Update((unsigned char *)szSrcPass,lstrlen(szSrcPass));
		Md5.MD5Final(szMDTemp);
		for (int i = 0; i < 16; i ++) 
			wsprintf(&szMD5Pass[i * 2], "%02x", szMDTemp[i] );
	}
	//sha加密方式
	if(par.m_nEncryptType == 2)
	{
		unsigned szMDTemp[5]={0};
		CString strSha=szSrcPass;
		SHA1 sha;
		sha.Reset();
		sha<<strSha;
		sha.Result(szMDTemp);

		CString strSingle;
		strSha="";
		for (int i=0;i<5;i++) 
		{
			strSingle.Format("%.8x",szMDTemp[i]);
			strSha+=strSingle;
		}
		wsprintf(szMD5Pass,"%s",strSha);
	}
	return szMD5Pass;
}
/// 银行界面发送指令，请求转帐记录
/// @return 无
void CLoveSendClassInExe::OnTransferRecord()
{
	MSG_GR_S_TransferRecord_t transferRecord;
	transferRecord.dwUserID = GetMeUserInfo()->dwUserID;
	/// 只带用户ID的带参数的消息
	SendGameData(&transferRecord, sizeof(transferRecord), MDM_GR_MONEY, ASS_GR_TRANSFER_RECORD_INGAME, 0);
}
/// 银行界面发送指令，修改密码
/// @param pChangePasswd 修改密码结构指针，在函数执行结束后要删除该指针
/// @return 无
void CLoveSendClassInExe::OnChangePasswd( ChangePasswd_t *pChangePasswd )
{
	MSG_GR_S_ChangePasswd_t changePasswd;
	changePasswd.UserID = GetMeUserInfo()->dwUserID;
	GetCryptedPasswd(changePasswd.szMD5PassOld, pChangePasswd->szPwdOld);
	GetCryptedPasswd(changePasswd.szMD5PassNew, pChangePasswd->szPwdNew);

	SendGameData(&changePasswd, sizeof(changePasswd), MDM_GR_MONEY, ASS_GR_CHANGE_PASSWD_INGAME, 0);
	delete pChangePasswd;
	pChangePasswd = NULL;
}
/// 银行界面发送指令，转帐
/// @param pTransferMoney 存取钱结构指针，在函数执行结束后要删除该指针
/// @return 无
void CLoveSendClassInExe::OnTransferMoney( TransferMoney_t *pTransferMoney )
{
	CBcfFile fMsg(CBcfFile::GetAppPath()+"..\\ClientMessage.bcf");
	MSG_GR_S_TransferMoney transferMoney;
	transferMoney.i64Money = pTransferMoney->i64Money;
	transferMoney.destUserID = pTransferMoney->uDestUserID;
	transferMoney.UserID = GetMeUserInfo()->dwUserID;
	transferMoney.bUseDestID = pTransferMoney->bUseDestID;
	strcpy(transferMoney.szNickName,GetMeUserInfo()->nickName);		//add by lxl 转账提示信息显示昵称
	strcpy(transferMoney.szDestNickName, pTransferMoney->szDestNickName);
	if (transferMoney.UserID == transferMoney.destUserID)
	{
		delete pTransferMoney;
		pTransferMoney = NULL;
		CString str = fMsg.GetKeyVal("BankDlg","TransferToSelf","您打算转帐给自己？对不起，系统暂不提供此项服务。");
		TCHAR szMsg[MAX_PATH];
		wsprintf(szMsg, str.GetBuffer());
		AFCMessageBox(szMsg);
		return;
	}
	GetCryptedPasswd(transferMoney.szMD5Pass, pTransferMoney->szPWD);

	SendGameData(&transferMoney,sizeof(transferMoney),MDM_GR_MONEY,ASS_GR_TRANSFER_MONEY_INGAME,0);
	delete pTransferMoney;
	pTransferMoney = NULL;
}
/// @param pCheckMoney 存取钱结构指针，在函数执行结束后要删除该指针
/// @return 无
void CLoveSendClassInExe::OnCheckMoney( CheckMoney_t *pCheckMoney )
{
	CBcfFile fMsg(CBcfFile::GetAppPath()+"..\\ClientMessage.bcf");
	/// 判断是存还是取
	if (pCheckMoney->uType==0) /// 存钱，不需要密码
	{
		__int64 i64Left = GetMeUserInfo()->i64Money - pCheckMoney->i64Money;
		if (i64Left < 0 || i64Left < m_pGameInfo->uLessPoint)
		{
			CString strBuffer;
			TCHAR szNum[128];
			GlbGetNumString(szNum, m_pGameInfo->uLessPoint, Glb().m_nPowerOfGold, Glb().m_bUseSpace, Glb().m_strSpaceChar);
			CString str = fMsg.GetKeyVal("GameRoom","NeedCoins","此房间需要您至少有 %s 的金币");
			strBuffer.Format(str, szNum);
			AFCMessageBox(strBuffer,m_pGameInfo->szGameName);
			/// 记得删除
			delete pCheckMoney;
			pCheckMoney = NULL;
			return;
		}
		MSG_GR_S_CheckMoney cm;
		cm.i64Count = pCheckMoney->i64Money;
		cm.UserID = GetMeUserInfo()->dwUserID;
		SendGameData(&cm,sizeof(cm),MDM_GR_MONEY,ASS_GR_CHECK_IN_INGAME,0);
	}
	else ///< 取钱，需要密码
	{
		MSG_GR_S_CheckMoneyWithPwd_t cm;
		cm.i64Count = pCheckMoney->i64Money;
		cm.UserID = GetMeUserInfo()->dwUserID;
		GetCryptedPasswd(cm.szMD5Pass, pCheckMoney->szPWD);
		SendGameData(&cm,sizeof(cm),MDM_GR_MONEY,ASS_GR_CHECK_OUT_INGAME,0);
	}
	/// 记得删除
	delete pCheckMoney;
	pCheckMoney = NULL;
}

/// 第2版银行的存取款
/// @param pCheckMoney 存取钱结构指
/// @return 无
void CLoveSendClassInExe::OnCheckMoney2(bank_op_normal* pCheckMoney )
{
	SendGameData(pCheckMoney,sizeof(bank_op_normal),MDM_BANK,ASS_BANK_NORMAL,0);
	return;
}

// lxl, 2010-11-17, 根据用户ID获取昵称
void CLoveSendClassInExe::OnGetNickNameOnID(GetNickNameOnID_t* pGetNickNameOnID)
{
	//OutputDebugString("发送消息给ExeForGame lxl");
    MSG_GR_S_GetNickNameOnID_t stGetNickNameOnID;
    stGetNickNameOnID.iUserID = pGetNickNameOnID->uUserID;

    SendGameData(&stGetNickNameOnID, sizeof(stGetNickNameOnID), MDM_GR_ROOM, ASS_GR_GET_NICKNAME_ONID_INGAME, 0);
    delete pGetNickNameOnID;
    pGetNickNameOnID = NULL;
}

//消息解释函数
BOOL CLoveSendClassInExe::PreTranslateMessage(MSG * pMsg)
{
	/// 银行界面发来的修改密码
	if (pMsg->message == WM_CHANGE_PASSWD)
	{
		OnChangePasswd( (ChangePasswd_t *)(pMsg->lParam) );
		return true;
	}

	/// 银行界面发来的转帐记录请求
	if (pMsg->message == WM_TRANSFER_RECORD)
	{
		OnTransferRecord();
		return true;
	}	/// 银行界面发来的转帐消息
	if (pMsg->message == WM_TRANSFER_MONEY)
	{
		OnTransferMoney( (TransferMoney_t *)(pMsg->lParam) );
		return true;
	}

	/// 银行界面发来的存取钱消息
// 	if (pMsg->message == WM_CHECK_MONEY)
// 	{
// 		OnCheckMoney( (CheckMoney_t *)(pMsg->lParam) );
// 		return true;
// 	}

	//新版银行存取款
// 	if (pMsg->message == WM_CHECK_MONEY2)
// 	{
// 		OnCheckMoney2((bank_op_normal*)(pMsg->lParam));
// 		return true;
// 	}

	//JianGuankun 2012-1-2,关闭银行窗口时使用
// 	if (pMsg->message == WM_BANK2_CLOSE)
// 	{
// 		OnCloseBank2();
// 		return true;
// 	}
    
	// lxl, 2010-11-17, 银行界面发来的获取用户昵称的消息
    if(WM_BANK_GET_NICKNAME_ONID == pMsg->message)
    {
        OnGetNickNameOnID((GetNickNameOnID_t*)(pMsg->lParam));
        return true;
    }
	
	if(pMsg->message==WM_SYSCOMMAND && pMsg->wParam==SC_RESTORE)
	{
		if(pMsg->wParam==SC_MAXIMIZE)
		{
			//百家乐 modify by wlr 20090716
			/*m_bMax=true;
			CRect rect;
			::SystemParametersInfo(SPI_GETWORKAREA,NULL,&rect,NULL);
			MoveWindow(0,0,rect.Width(),rect.Height());*/
			OnBnClickedMax();//add by wlr 20090716
			//百家乐 end of modify by wlr 20090716

			return TRUE;
		}
		if(pMsg->wParam==SC_RESTORE)
		{
			ShowWindow(SW_RESTORE);
			if(m_bShowMax)
			{
				//百家乐 modify by wlr 20090716
				/*m_bMax=true;
				CRect rect;
				::SystemParametersInfo(SPI_GETWORKAREA,NULL,&rect,NULL);
				MoveWindow(0,0,rect.Width(),rect.Height());*/
				//百家乐 end of modify by wlr 20090716
				OnBnClickedMax();//add by wlr 20090716
			}
			else
			{
				m_bMax=false;

				MoveWindow(0,0,m_SavedScreen.x,m_SavedScreen.y);
				CenterWindow();
			}
			SetForegroundWindow();
			return TRUE;
		}
	}
	//过滤取消消息和回车消息
	if(pMsg->message==UM_USEITEM_USERSELECTED)
	{
		for(int i=0; i<MAX_PEOPLE; i++)
		{
			if( m_pUserInfo[i] && m_pUserInfo[i]->GameUserInfo.dwUserID == pMsg->lParam )
			{
				int nRet = OnUseProp((int)pMsg->wParam, m_pUserInfo[i]->GameUserInfo.dwUserID);
				break;
			}
		}
		return TRUE;
	}
	if (pMsg->message==WM_KEYDOWN)
	{
		if (pMsg->wParam==VK_RETURN)
		{	
			SendTalkMessage();
			return TRUE;
		}
		else if (pMsg->wParam == VK_F1)
		{
			return TRUE;
		}
		else 
			if (pMsg->wParam==VK_ESCAPE) 
				return TRUE;
	}
#ifdef _USE_PROP_PAGE_SW_
	//{add by HuangYin 2008/04/07
	if(pMsg->message==WM_MOUSEMOVE)
	{
		if(this->GetFocus()==(CWnd*)&m_BtPropGoAhead || 
			this->GetFocus()==(CWnd*)&m_BtPropGoNext)
			this->SetFocus();
	}
#endif
	//wushuqun 2009.6.9
	if (pMsg->message == WM_LBUTTONDOWN)
	{
		if (m_pGameInfo)
		{
			if (m_pGameInfo->bIsInRecord)
			{
				return true;
			}
		}
	}
	////百家乐 最大化不能拖动 del by wlr 20090717
	////wushuqun 2009.6.10
	////最大化后不能拖动
	//if((WM_NCLBUTTONDOWN == pMsg->message)
	//&& (HTCAPTION == pMsg->wParam)
	//&& (m_hWnd == pMsg->hwnd) && m_bMax)  // 是自己窗口消息
	//{
	//	//百家乐 add by wlr 20090717
	//	char GameCode[20];
	//	::memset(GameCode,0,sizeof(GameCode));
	//	GetGameCode(GameCode);
	//	CString str = GameCode;
	//	if (str != "10301800" &&
	//		str != "30501800" &&
	//		str != "10901800" &&
	//		str != "11901800" )
	//	{
	//		return TRUE;
	//	}
	//	//end of 百家乐 add by wlr 20090717
	//}
	////end of 百家乐 最大化不能拖动 del by wlr 20090717

	if (m_pGameView->m_bShowRight)
	{
		m_ToolTip.RelayEvent(pMsg);
	}
	return __super::PreTranslateMessage(pMsg);
}

void CLoveSendClassInExe::SendTalkMessage(int nDefaultIndex)
{
	//判断状态
	if ((GetMeUserInfo()->bDeskNO==255)
		||(GetMeUserInfo()->bDeskStation==255)) 
		return;

	//判断信息
	if (m_pRightFrame)
	{
		m_pRightFrame->SetFocusToTalkInput();
		if (m_pRightFrame->GetTalkInputMessage(NULL) == 0) 
			return;
	}
	else
	{
		m_InputMessage.SetFocus();
		if (m_InputMessage.GetWindowTextLength()==0) 
			return;
	}
	

	try
	{
		//禁止游戏聊天
		if (m_pGameInfo->dwRoomRule & GRR_FORBID_GAME_TALK)
			throw TEXT("抱歉,此游戏禁止聊天!");
		//判断是否可以聊天
		if (CUserPower::CanGameTalk(m_pGameInfo->dwGamePower)==false) 
			throw TEXT("抱歉，你暂时没有聊天的权利!");

		//获取消息
		CString strBuffer;
		if (m_pRightFrame)
		{
			char szMsgBuf[MAX_TALK_LEN];
 			m_pRightFrame->GetTalkInputMessage(szMsgBuf);
			m_pRightFrame->ClearTalkInput();
			m_pRightFrame->SetFocusToTalkInput();
			strBuffer = szMsgBuf;
		}
		else
		{
			m_InputMessage.GetWindowText(strBuffer);
		}
		
		////判断目标对象
		long int dwTargerID=0;

		if(m_CurpUserInfoClicked)
		{
			CString nick=m_CurpUserInfoClicked->nickName;
			nick+=":";
			int l=nick.GetLength();
			if(strBuffer.Left(l)==nick)
			{
				dwTargerID=m_CurpUserInfoClicked->dwUserID;
				strBuffer=strBuffer.Mid(l);
			}
		}

		if(strBuffer=="")
			return;

		//判断信息长度
		int iMaxSendLen=CUserPower::CanSendMuch(m_pGameInfo->dwGamePower)?MAX_TALK_LEN:NORMAL_TALK_LEN;
		if (strBuffer.GetLength()>iMaxSendLen)
			throw TEXT("由于发送的信息太长，发送失败！");

		//过滤消息
		if (strBuffer.Find(TEXT("{\\rtf"))!=-1) 
			throw TEXT("不能发送非法字符，发送失败！");

		for (int i=strBuffer.GetLength();i>0;i--)
			if ((strBuffer[i-1]>=-1)&&(strBuffer[i-1]<30))
				throw TEXT("不能发送非法字符，发送失败！");

		//判断速度
		if (CUserPower::CanTalkFast(m_pGameInfo->dwGamePower)==false)
		{
			long int dwNowTime=(long int)time(NULL);
			if ((CUserPower::CanTalkFast(m_pGameInfo->dwGamePower)==false)&&((m_dwLastTalkTime+1L)>dwNowTime))
				throw TEXT("请爱护聊天环境，请不要频繁发送信息！");
			if (((m_dwLastTalkTime+3L)>dwNowTime)&&(m_strLastTalk.Compare(strBuffer)==0)) 
				throw TEXT("请爱护聊天环境，请不要重复发言！");
		}

		//发送聊天信息
		MSG_GR_RS_NormalTalk Talk;
		ZeroMemory(&Talk,sizeof(MSG_GR_RS_NormalTalk));
		Talk.iLength=strBuffer.GetLength();
		Talk.dwSendID=GetMeUserInfo()->dwUserID;
		Talk.dwTargetID=dwTargerID;
		Talk.nDefaultIndex=nDefaultIndex;
		lstrcpy(Talk.szMessage,strBuffer);
		Talk.crColor=m_colorresource.m_crNormalTxColor;

		SendGameData(&Talk,sizeof(Talk)-sizeof(Talk.szMessage)+Talk.iLength+1,MDM_GM_GAME_FRAME,ASS_GM_NORMAL_TALK,0);

		//保存信息
		m_strLastTalk=Talk.szMessage;
		if (m_InputMessage.FindString(0,m_strLastTalk)==-1) 
			m_InputMessage.InsertString(0,m_strLastTalk);

		if (m_InputMessage.GetCount()>40) 
			m_InputMessage.DeleteString(40-m_nTalkDefaultCount);

		//设置参数
		m_dwLastTalkTime=(ULONG)time(NULL);
		m_InputMessage.SetWindowText(TEXT(""));
	}
	catch (TCHAR * szErrorMessage) 
	{
		m_MessageHandle.InsertSystemMessage(szErrorMessage);
	}
	catch (...) 
	{ 
		m_MessageHandle.InsertSystemMessage(TEXT("消息发送失败！"));
	}
	::SendMessage(m_TalkMessage.m_hWnd,WM_VSCROLL,SB_BOTTOM,0);

	return;
}
BOOL CLoveSendClassInExe::OnToolTipNotify(UINT id, NMHDR * pTTTStruct, LRESULT * pResult )
{
	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pTTTStruct;    
	UINT nID =pTTTStruct->idFrom;			//得到相应窗口ID，有可能是HWND 
	CString stip="点击购买道具";
	int curPropIndex=m_curPropPage*PEER_PAGE;
    int curPropIndex_S=m_curPropPage_S*PEER_PAGE;   // PengJiLin, 2010-10-12
    BOOL bIsNormalProp = TRUE;     // 普通道具
	_TAG_USERPROP *userProp=NULL;
	int propCount=Glb().userPropLibrary.GetCount();
	if (pTTT->uFlags & TTF_IDISHWND)		//表明nID是否为HWND 
	{ 
		nID = ::GetDlgCtrlID((HWND)nID);	//从HWND得到ID值，当然你也可以通过HWND值来判断 
		switch(nID) 
		{	
		case IDC_BUTTON_PROP1:
			break;
		case IDC_BUTTON_PROP2:
			curPropIndex+=0;
			break;
		case IDC_BUTTON_PROP3:
			curPropIndex+=1;
			break;
		case IDC_BUTTON_PROP4:
			curPropIndex+=2;
			break;
		case IDC_BUTTON_PROP5:
			curPropIndex+=3;
			break;
		case IDC_BUTTON_PROP6:
			curPropIndex+=4;
			break;
		case IDC_BUTTON_PROP7:
			curPropIndex+=5;
			break;
		case IDC_BUTTON_PROP8:
			curPropIndex+=6;
			break;
		case IDC_BUTTON_PROP9:
			curPropIndex+=7;
			break;
		case IDC_BUTTON_PROP10:
			curPropIndex+=8;
			break;
		/*case IDC_BUTTON_PROP6:
            bIsNormalProp = FALSE;
			curPropIndex_S+=2;
			break;
        case IDC_BUTTON_PROP4:
            bIsNormalProp = FALSE;
            curPropIndex_S+=0;
            break;
        case IDC_BUTTON_PROP5:
            bIsNormalProp = FALSE;
            curPropIndex_S+=1;*/
            break;
		default:
			break;
		}
		if(curPropIndex<propCount && TRUE == bIsNormalProp)
		{
			userProp=Glb().userPropLibrary.GetAt(curPropIndex);
			stip.Format("%s [%d]",userProp->szPropName,userProp->nHoldCount);
		}
        else if(curPropIndex_S < propCount && FALSE == bIsNormalProp)
        {
            userProp=Glb().userPropLibrary.GetAt(curPropIndex_S);
            stip.Format("%s",userProp->szPropName);
        }

		stip.Trim();
		lstrcpy(pTTT->lpszText, stip);	//设置相应的显示字串 
	} 
	return FALSE; 
}
BOOL CLoveSendClassInExe::OnUseProp(int nPropIndex, int nDeskUserID)
{
	//////////////////////////////////////////////////////////////////////////
	//2008-08-11
	if(nPropIndex<0 || nPropIndex>=Glb().userPropLibrary.GetCount())
		return FALSE;

	_TAG_USERPROP * userProp=Glb().userPropLibrary.GetAt(nPropIndex);
	if(userProp->nHoldCount<=0)
		return FALSE;

	_TAG_USINGPROP  usingProp;
	ZeroMemory(&usingProp,sizeof(_TAG_USINGPROP));
	usingProp.dwUserID=GetMeUserInfo()->dwUserID;
	usingProp.dwTargetUserID=nDeskUserID;
	usingProp.nPropID=userProp->nPropID;
	usingProp.nPropActionAttrib=userProp->attribAction;
	usingProp.nPropValueAttrib=userProp->attribValue;

	memcpy(usingProp.szPropName,userProp->szPropName,sizeof(userProp->szPropName));
	SendGameData(&usingProp,sizeof(_TAG_USINGPROP),MDM_GR_PROP,ASS_PROP_USEPROP,0);

	// 本地扣除相应道具数量
	//if(userProp->nHoldCount <=0)
	//{
	//	Glb().userPropLibrary.RemoveAt(nPropIndex);
	//	delete userProp;
	//	userProp=NULL;
	//}
	return TRUE;
}
// 载入声音列表
void CLoveSendClassInExe::LoadSoundList(void)
{
	TCHAR sz[61],szPath[MAX_PATH];
	lstrcpy(sz,m_pGameInfo->szProcessName);
	sz[lstrlen(sz)-lstrlen(".ico")]='\0';
	wsprintf(szPath,"GameOption.bcf");
	//AfxMessageBox(szPath);
	//DebugPrintf("szPath = [%s]", szPath);
	CBcfFile f(szPath);
	int		soundCount=f.GetKeyVal("BGSOUND","SoundCount",0);
	//DebugPrintf("SoundCount = [%d]", soundCount);
	if(soundCount==0)
		return;

	m_sndListID=BZSoundCreateList(this,0);
	if(m_sndListID==0)
		return;
	CString soundFile;
	for(int i=0;i<soundCount;i++)
	{
		wsprintf(sz,"Sound%d",i);
		soundFile=f.GetKeyVal("BGSOUND",sz,"");
		wsprintf(szPath,"%s",soundFile);
		//DebugPrintf("szPath = [%s]", szPath);
		BZSoundAddList(m_sndListID,szPath);
	}
	int nVol=f.GetKeyVal("BGSOUND","SoundVolume",300);
	BZSoundSetVolume(m_sndListID,nVol);
	m_bSndPause=f.GetKeyVal("BGSOUND","PauseSound",0);
	if(m_bSndPause)
	{
		BZSoundPause(m_sndListID);
		//m_btSndPlay.ShowWindow(SW_SHOW);
		//m_btSndPause.ShowWindow(SW_HIDE);
	}
	else
	{
		BZSoundPlayList(m_sndListID);
		//m_btSndPlay.ShowWindow(SW_HIDE);
		//m_btSndPause.ShowWindow(SW_SHOW);
	}
	if(AfxGetApp()->m_pMainWnd)
		AfxGetApp()->m_pMainWnd->PostMessage(WM_USER+100,2,0);
}

//调整位置
void CLoveSendClassInExe::FixControlStation(int iWidth, int iHeight)
{
	if(!m_bInit)
		return ;
	if(!m_bMax)
	{
		CRect rcRect;
		GetWindowRect(rcRect);
		if(rcRect.Width()>320 && rcRect.Height()>200)
		{
			m_SavedScreen.x=rcRect.Width();
			m_SavedScreen.y=rcRect.Height();
		}
	}

	int iWindowsWidth = GetSystemMetrics(SM_CXSCREEN); 
	int iWindowsHeight = GetSystemMetrics(SM_CYSCREEN); 

	if (m_pRightFrame)
	{
		if (m_bShowMax)
		{
			if (iWindowsWidth >= 1280)
			{
				m_vlistx = m_pRightFrame->m_uMaxWidth;
			}		
			else
			{
				m_vlistx = m_pRightFrame->m_uMinWidth;
			}
		}
		else
		{
			m_vlistx = m_pRightFrame->m_uMinWidth;
		}
		m_pGameView->m_vlistx = m_vlistx;
	}

	CString strBcfFilename = m_skinmgr.GetGameSkinBcfFileName();
	if (CBcfFile::IsFileExist(CBcfFile::GetAppPath() + "config\\" + strBcfFilename))
		m_iniFile= CBcfFile::GetAppPath() + "config\\" + strBcfFilename;
	else
		m_iniFile= CBcfFile::GetAppPath()+"..\\config\\" + strBcfFilename;

	CString s=CINIFile::GetAppPath ();
	CBcfFile f( m_iniFile);
	//AfxMessageBox(m_iniFile);
	CString key=TEXT("config");
	bool showright=m_pGameView->m_bShowRight;
	//广告位
	int m_AdvX=-100;
	int	m_AdvY=-100;
	int m_AdvWidth=10;
	int m_AdvHeight=f.GetKeyVal(key,"advh",0);

    // PengJiLin, 2010-9-28, 刷新控件
#ifdef _USE_IE_AD_
    m_ie.Invalidate(FALSE);

	if(m_ie.m_hWnd && szAdvUrl!="" && showright)//只有在IE有效和地址有效的情况下才有效
	{
		if(m_AdvHeight>0)
		{
			m_AdvX=f.GetKeyVal(key,"advx",-100);
			m_AdvY=f.GetKeyVal(key,"advy",-100);
			m_AdvWidth=f.GetKeyVal(key,"advw",10);
			m_ie.MoveWindow(iWidth-m_AdvX,m_AdvY,m_AdvWidth,m_AdvHeight);
		}
		else
		{
			m_ie.MoveWindow(-100,-100,0,0);
			//m_AdvHeight=0;
		}
	}
	else
	{
		m_ie.MoveWindow(-100,-100,0,0);
		//m_AdvHeight=0;
	}
#endif

	int x,y,w,h;
	CRect rect;
	// 最大化的情况下才做的事
	if (showright)
	{

        x=f.GetKeyVal(key,"bluediamondx",190);
        y=f.GetKeyVal(key,"bluediamondy",368);
        m_Btbt5.SetWindowPos(NULL, iWidth-x, y , 0, 0, SWP_NOSIZE|SWP_NOZORDER);

		x=f.GetKeyVal(key,"ulistx",0);
		y=f.GetKeyVal(key,"ulisty",0);
		w=f.GetKeyVal(key,"ulistw",0);
		h=f.GetKeyVal(key,"ulisth",0);

		CGameImageHelper helprui(&m_UserInfo_bk);
		CGameImageHelper helprbc(&m_right_bc);
		CGameImageHelper helprrt(&m_right_rt);
		CGameImageHelper helprtc(&m_right_tc);
		CGameImageHelper helprt(&m_rt);
		CGameImageHelper helprb(&m_rb);

		//用户列表
		y = helprt.GetHeight()+helprtc.GetHeight()+helprui.GetHeight();
		m_UserListRect.top = y;
		m_UserListRect.left = iWidth-x;
		m_UserListRect.right = m_UserListRect.left + w;
		m_UserListRect.bottom = m_UserListRect.top + h;
		SafeMoveWindow(	&m_UserListDlg, iWidth-x,y,w,h);
		m_userlistW = w;
		m_userlistH = h;


		// PengJiLin, 2010-9-28, 道具背景图位置
		x=f.GetKeyVal(key,"propbackx",0);
		y=f.GetKeyVal(key,"propbacky",0);
		w=f.GetKeyVal(key,"propbackw",0);
		h=f.GetKeyVal(key,"propbackh",0);
		CRect rc;
		m_UserListDlg.GetWindowRect(&rc);
		y = helprt.GetHeight()+helprtc.GetHeight()+helprui.GetHeight()+rc.Height();
		m_propBackRect.left = iWidth - x;
		m_propBackRect.top = y;
		m_propBackRect.right = m_propBackRect.left + w;
		m_propBackRect.bottom = m_propBackRect.top + h;
		x=f.GetKeyVal(key,"propbackx2",0);
		y=f.GetKeyVal(key,"propbacky2",0);
		w=f.GetKeyVal(key,"propbackw2",0);
		h=f.GetKeyVal(key,"propbackh2",0);
		m_propBackRect2.left = iWidth - x;
		m_propBackRect2.top = y;
		m_propBackRect2.right = m_propBackRect2.left + w;
		m_propBackRect2.bottom = m_propBackRect2.top + h;


		x=f.GetKeyVal(key,"chatx",0);
		y=f.GetKeyVal(key,"chaty",0);
		w=f.GetKeyVal(key,"chatw",0);
		//h=f.GetKeyVal(key,"chath",0);
		//注意：h是整个高度-聊天框的高度，即，要通过iHeight-h来取得聊天框实际的高度
		//另，用户列表框也是配置来的，所以要减去用户列表框的高度
		//Fred Huang 2008-05-29
		int chath=f.GetKeyVal(key,"chath",480);


		y = helprt.GetHeight()+helprtc.GetHeight()+helprui.GetHeight()+rc.Height()+m_propBackRect.Height();
		chath=iHeight - y - helprbc.GetHeight() - helprb.GetHeight();
		//聊天框
		SafeMoveWindow(	&m_TalkMessage, iWidth-x,y,w,chath);//223);	
		m_talkY = y;
		m_talkW = w;
		m_talkH = chath;
		//设置
		//{modify by zxj 2008/04/08
		//x=f.GetKeyVal(key,"gamebt1x",188);
		x=f.GetKeyVal(key,"gamebt1x",190);
		//end modify}
		y=f.GetKeyVal(key,"gamebt1y",368);

		//{modify by zxj 2008/04/03
		//m_Btbt1.SetWindowPos(NULL,iWidth-x,y,0,0,SWP_NOSIZE|SWP_NOZORDER);
		//m_Btbt1.SetWindowPos(NULL,iWidth-x,y-10,0,0,SWP_NOSIZE|SWP_NOZORDER);
		m_Btbt1.MoveWindow(iWidth-x,y-10,18,18);
		//end modify}

		m_Btbt1.GetClientRect(&rect);
		m_iStatic1X = iWidth-x+10;
		m_iStatic1Y = y+rect.Height()+2;	

		//{add by zxj 2008/04/07
		//道具面板布局 modify by lxl 2010-12-27
		int itemWith = f.GetKeyVal(key,"itemwith",35); //道具的间隔
		//第一排道具
#ifdef _USE_PROP_PAGE_SW_
		x=f.GetKeyVal(key,"itemprev1x",197);
		y=f.GetKeyVal(key,"itemprev1y",379);
		m_BtPropGoAhead.SetWindowPos(NULL, iWidth -x, y , 0, 0, SWP_NOSIZE|SWP_NOZORDER);	//向前按钮
		x=f.GetKeyVal(key,"itemnext1x",30);
		m_BtPropGoNext.SetWindowPos(NULL, iWidth-x, y, 0, 0, SWP_NOSIZE|SWP_NOZORDER);		//向后按钮
#endif

// 		x=f.GetKeyVal(key,"itemrow1x",180);
// 		y=f.GetKeyVal(key,"itemrow1y",360);
		x = m_propBackRect.Width() - (m_propBackRect.Width()-itemWith*5)/2;
		y = m_propBackRect.top + (m_propBackRect.Height()-itemWith*2)/2;
		m_BtProp1.SetWindowPos(NULL, iWidth-x, y , 0, 0, SWP_NOSIZE|SWP_NOZORDER);
		x -= itemWith;
		m_BtProp2.SetWindowPos(NULL, iWidth-x, y , 0, 0, SWP_NOSIZE|SWP_NOZORDER);
		x -= itemWith;
		m_BtProp3.SetWindowPos(NULL, iWidth-x, y,  0, 0, SWP_NOSIZE|SWP_NOZORDER);
		x -= itemWith;
        m_BtProp4.SetWindowPos(NULL, iWidth-x, y , 0, 0, SWP_NOSIZE|SWP_NOZORDER);   
		x -= itemWith;
		m_BtProp5.SetWindowPos(NULL, iWidth-x, y , 0, 0, SWP_NOSIZE|SWP_NOZORDER);   
		
		//第二排道具
#ifdef _USE_PROP_PAGE_SW_
		x=f.GetKeyVal(key,"itemprev2x",197);
        y=f.GetKeyVal(key,"itemprev2y",383);
        m_Btbt6.SetWindowPos(NULL, iWidth -x, y , 0, 0, SWP_NOSIZE|SWP_NOZORDER);   //向前按钮
        x=f.GetKeyVal(key,"itemnext2x",36);
        m_Btbt7.SetWindowPos(NULL, iWidth -x, y , 0, 0, SWP_NOSIZE|SWP_NOZORDER);   //向后按钮
#endif

// 		x=f.GetKeyVal(key,"itemrow2x",180);
//      y=f.GetKeyVal(key,"itemrow2y",395);
		x = m_propBackRect.Width() - (m_propBackRect.Width()-itemWith*5)/2;
		y = m_propBackRect.top + (m_propBackRect.Height()-itemWith*2)/2 + itemWith;
		m_BtProp6.SetWindowPos(NULL, iWidth-x, y, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
		x -= itemWith;
		m_BtProp7.SetWindowPos(NULL, iWidth-x, y, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
		x -= itemWith;
		m_BtProp8.SetWindowPos(NULL, iWidth-x, y, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
		x -= itemWith;
		m_BtProp9.SetWindowPos(NULL, iWidth-x, y, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
		x -= itemWith;
		m_BtProp10.SetWindowPos(NULL, iWidth-x, y, 0, 0, SWP_NOSIZE|SWP_NOZORDER);

		//end add}

		//{modify by zxj 2008/04/07
		//x=f.GetKeyVal(key,"gamebt2x",148);
		x=f.GetKeyVal(key,"gamebt2x",150);
		//end modify}
		y=f.GetKeyVal(key,"gamebt2y",368);

		//{modify by zxj 2008/04/03
		//m_Btbt2.SetWindowPos(NULL,iWidth-x,y,0,0,SWP_NOSIZE|SWP_NOZORDER);
		m_Btbt2.SetWindowPos(NULL,iWidth-x,y-10,0,0,SWP_NOSIZE|SWP_NOZORDER);
		//end modify}

		//m_Btbt2.GetClientRect(&rect);

		//{modify by zxj 2008/04/07
		//x=f.GetKeyVal(key,"gamebt3x",108);
		x=f.GetKeyVal(key,"gamebt3x",110);
		//end modify}
		y=f.GetKeyVal(key,"gamebt3y",368);

		//{modify by zxj 2008/04/03
		//m_Btbt3.SetWindowPos(NULL,iWidth-x,y,0,0,SWP_NOSIZE|SWP_NOZORDER);
		m_Btbt3.SetWindowPos(NULL,iWidth-x,y-10,0,0,SWP_NOSIZE|SWP_NOZORDER);
		//end modify}
		//m_Btbt3.GetClientRect(&rect);

		//{modify by zxj 2008/04/07 
		//x=f.GetKeyVal(key,"gamebt4x",68);
		x=f.GetKeyVal(key,"gamebt4x",70);
		//end modify}
		y=f.GetKeyVal(key,"gamebt4y",368);

		//{modify by zxj 2008/04/03
		//m_Btbt4.SetWindowPos(NULL,iWidth-x,y,0,0,SWP_NOSIZE|SWP_NOZORDER);
		m_Btbt4.SetWindowPos(NULL,iWidth-x,y-10,0,0,SWP_NOSIZE|SWP_NOZORDER);
		//end modify}


		//聊天控件
		x=f.GetKeyVal(key,"expx",0);
		y=f.GetKeyVal(key,"expy",0);
		//SafeMoveWindow(&m_TargetPeople,x,iHeight-BUTTOM_HEIGHT+5,iWidth-x-50,200);	//目标
		//m_BtExpression.SetWindowPos(NULL,iWidth-x,iHeight-y,0,0,SWP_NOSIZE|SWP_NOZORDER);
		m_BtExpression.GetClientRect(&rect);
		m_BtExpression.MoveWindow(iWidth-x,iHeight-y,rect.Width(),rect.Height());

		//设置颜色按钮
		x=f.GetKeyVal(key,"setcolorx",0);
		y=f.GetKeyVal(key,"setcolory",0);
		m_BtSetColor.MoveWindow(iWidth-x,iHeight-y,18,18);

		x=f.GetKeyVal(key,"inputx",0);
		y=f.GetKeyVal(key,"inputy",0);
		w=f.GetKeyVal(key,"inputw",0);
		h=f.GetKeyVal(key,"inputh",0);
		SafeMoveWindow(&m_InputMessage,iWidth-x,iHeight-y,w,20);	//输入
		x=f.GetKeyVal(key,"sendx",0);
		y=f.GetKeyVal(key,"sendy",0);
		//m_BtSend.SetWindowPos(NULL,iWidth-x,iHeight-y,0,0,SWP_NOSIZE|SWP_NOZORDER);
		m_BtSend.GetClientRect(&rect);
		m_BtSend.MoveWindow(iWidth-x,iHeight-y,rect.Width(),rect.Height());
	}

	///////////////////////////////////////
	////Kylin 20090209 自动适应图片大小
	//右上按钮
	CBcfFile bcffile(CBcfFile::GetAppPath() + _T("GameOption.bcf"));
	UINT uMaxType = bcffile.GetKeyVal("FrameControl","max_type",1);
	//最小化
	x=f.GetKeyVal(key,"minx",0);
	y=f.GetKeyVal(key,"miny",0);
	CString strPos;
	if (uMaxType != 0)
	{
	//m_BtMin.SetWindowPos(NULL,iWidth-x,y,0,0,SWP_NOSIZE|SWP_NOZORDER);
	m_BtMin.GetClientRect(&rect);
	m_BtMin.MoveWindow(iWidth-x,y,rect.Width(),rect.Height());
		strPos.Format("rende116 X: %d , Y: %d",iWidth-x,y);
		OutputDebugString(strPos);
	}
	else
	{
		x = x + (f.GetKeyVal(key,"restx",0) - x);
		y = f.GetKeyVal(key,"resty",0);

		strPos.Format("rende116 X: %d , Y: %d",iWidth-x,y);
		OutputDebugString(strPos);
		m_BtMin.GetClientRect(&rect);
		m_BtMin.MoveWindow(iWidth-x,y,rect.Width(),rect.Height());
	}
	//退出
	x=f.GetKeyVal(key,"quitx",0);
	y=f.GetKeyVal(key,"quity",0);
	//m_BtGameQuit.SetWindowPos(NULL,iWidth-x,y,0,0,SWP_NOSIZE|SWP_NOZORDER);

	m_BtGameQuit.GetClientRect(&rect);
	m_BtGameQuit.MoveWindow(iWidth-x,y,rect.Width(),rect.Height());
	//m_BtExit.GetClientRect(&rect);
	//m_BtExit.MoveWindow(iWidth-x,y,rect.Width(),rect.Height());

	//最大化和还原
	if (uMaxType != 0)
	{
	x=f.GetKeyVal(key,"restx",0);
	y=f.GetKeyVal(key,"resty",0);
	m_btMax.GetClientRect(&rect);
	m_btMax.MoveWindow(iWidth-x,y,rect.Width(),rect.Height());
	m_btRest.GetClientRect(&rect);
	m_btRest.MoveWindow(iWidth-x,y,rect.Width(),rect.Height());
	}

	y=f.GetKeyVal(key,"hideshowbt",350);
	if(showright)
	{//隐藏按钮
		m_BtHide.SetWindowPos(NULL,iWidth-m_vlistx,y,0,0,SWP_NOSIZE|SWP_NOZORDER);
	}
	else
	{
		CRect rect;
		CGameImageHelper helprc(&m_rc);
		m_BtHide.GetClientRect(&rect);
		m_BtHide.SetWindowPos(NULL,iWidth-rect.Width()-helprc.GetWidth(),y,0,0,SWP_NOSIZE|SWP_NOZORDER);
	}
	//游戏界面
	x=f.GetKeyVal(key,"viewx",0);
	y=f.GetKeyVal(key,"viewy",0);
	w=f.GetKeyVal(key,"vieww",0);
	h=f.GetKeyVal(key,"viewh",0);

	if(m_pGameView->m_ShowType == 0)
	{
		if(showright)
			SafeMoveWindow(m_pGameView,x,y,iWidth-x-m_vlistx,iHeight-h-y);
		else
			SafeMoveWindow(m_pGameView,x,y,iWidth-w-x,iHeight-h-y);
	}
	else if(m_pGameView->m_ShowType == 1)
	{
		SafeMoveWindow(m_pGameView,x,y,iWidth-w-x,iHeight-h-y);
		//CRect rect;
		//m_pGameView->GetClientRect(&rect);
		//m_pGameView->OnSize(SIZE_RESTORED,rect.Width(),rect.Height());
		//m_pGameView->UpdateViewFace(NULL);
	}

	//{add by zxj 2008/04/09
	for( int i=0; i<MAX_PEOPLE; i++)
	{
		m_UserFrameRectArray[i].SetRect(m_pGameView->m_PtLogo[i].x, m_pGameView->m_PtLogo[i].y,\
			m_pGameView->m_PtLogo[i].x + m_pGameView->m_iUserFrame.GetWidth(), \
			m_pGameView->m_PtLogo[i].y + m_pGameView->m_iUserFrame.GetHeight());
	}
	//end add}

	SafeMoveWindow(&m_Btbt1,iWidth-215,3,m_sndBtnCx,m_sndBtnCy);			//modify by wyl 11-5-12  
	/////////////////////////////////////
	///////Kylin 20090122 添加积分榜文字颜色设置
	int r=f.GetKeyVal(key,"showscorer",0);
	int g=f.GetKeyVal(key,"showscoreg",0);
	int b=f.GetKeyVal(key,"showscoreb",0);

	m_pGameView->m_showcoredlg.SetTextColor(RGB(r,g,b));
	/////////////////////////////////////

	if (m_bShowMax)
	{
		if (m_pRightFrame)
		{
			CRect rcRight;
			CRect rcMain;

			char szRcFile[64] = {0};
			_stprintf(szRcFile,"..\\config\\%s",m_skinmgr.GetGameSkinBcfFileName().GetBuffer());
			CBcfFile f(szRcFile);

			CRect rc[3];
			enum emSoduku
			{
				TC = 0, //中上截图
				RC = 1, //右中截图
				BC = 2, //中下截图
			};

			char* szKey[3] = {"2_TC","5_RC","7_BC"};

			for(int i = 0; i < 3; ++i)
			{
				CString strPos;
				strPos = f.GetKeyVal(_T("Sudoku"),szKey[i],"0,0,0,0");
				rc[i].left	  = ::atoi(::strtok(strPos.GetBuffer(),","));
				rc[i].top	  = ::atoi(::strtok(NULL,","));
				rc[i].right  = rc[i].left + ::atoi(::strtok(NULL,","));
				rc[i].bottom = rc[i].top +  ::atoi(::strtok(NULL,","));
			}

			::GetWindowRect(m_pRightFrame->GetHWND(),&rcRight);
			::GetWindowRect(m_hWnd,rcMain);
			if (iWindowsWidth >= 1280)
			{
				::MoveWindow(m_pRightFrame->GetHWND(),rcMain.left + iWidth - m_pRightFrame->m_uMaxWidth - rc[RC].Width(),rc[TC].Height(),
					m_pRightFrame->m_uMaxWidth,iHeight- rc[TC].Height() - rc[BC].Height(),true);
			}
			else
			{
				::MoveWindow(m_pRightFrame->GetHWND(),rcMain.left + iWidth - m_pRightFrame->m_uMinWidth - rc[RC].Width(),rc[TC].Height(),
					m_pRightFrame->m_uMinWidth,iHeight- rc[TC].Height() - rc[BC].Height(),true);
			}
		}
	}
	set_videos_pos();
}

void CLoveSendClassInExe::OnInitPropList(int nRefleshType)
{
	// 只更新数量，不更新画面
	if (nRefleshType>0)
	{
		//更新道具面板
		if (NULL != this->m_pPropPannel)
		{
			m_pPropPannel->ResetData();
		}
		return;
	}
	
    // PengJiLin, 2010-10-13, 按钮资源加载全部移到LoadSkin

	OnSetPropImages();
    //OnSetPropImagesS();     // PengJiLin, 2010-10-12
}
//====设置填充色
void CLoveSendClassInExe::SetFillColor(COLORREF rgb)
{
	if(!m_TalkMessage.m_hWnd)
		return;

	m_TalkMessage.SetBackgroundColor(false,rgb);
	m_TalkMessage.m_bkColor=rgb;
}
//重画函数
void CLoveSendClassInExe::OnPaint()
{
	/// 用双缓存的办法重写OnPaint函数
	CPaintDC dc(this);

	/// 设置裁剪区域 zxd 20100330
	/// 取消三个子窗口区域
	CRgn rgn1, rgnWnd;
	rgn1.CreateRectRgnIndirect(&m_UserListRect);

	//获取位置
	CRect ClientRect;
	GetClientRect(&ClientRect);
	CFont font,*pOldFont;
	int iWidth = ClientRect.Width();
	int iHeight = ClientRect.Height();

	rgnWnd.SetRectRgn(&ClientRect);
	rgn1.CombineRgn(&rgn1, &rgnWnd, RGN_AND);
	rgnWnd.CombineRgn(&rgnWnd, &rgn1, RGN_XOR);
	//dc.SelectClipRgn(&rgnWnd, RGN_AND);

	//双缓存
	CDC tempCDC;
	tempCDC.CreateCompatibleDC(&dc);
	CBitmap	BufferBmp;
	int width = ClientRect.Width();
	int height = ClientRect.Height();
	BufferBmp.CreateCompatibleBitmap(&dc,width,height);
	CBitmap* pOldBitmap = tempCDC.SelectObject(&BufferBmp);
	tempCDC.BitBlt(0,0,width,height,&dc,0,0,SRCCOPY);
	//end 双缓存

	tempCDC.SetBkMode(TRANSPARENT);	
	font.CreateFont(-12,0,0,0,400,NULL,NULL,NULL,134,3,2,1,2,TEXT("宋体"));
	pOldFont=tempCDC.SelectObject(&font);	

	DrawViewFrame(&tempCDC,iWidth,iHeight);
	//标题
	TCHAR title[100];
	GetWindowText(title,99);

	// 标题居中显示
	CSize size;
	size = dc.GetTextExtent(title,strlen(title));
	int iStrWidth = size.cx; // 得到显示title的总宽度
	//m_titlex 可根据最大化，还原，动态调整标题显示位置
	if((iWidth - iStrWidth) / 2 >= 0)
		m_titlex = (iWidth - iStrWidth) / 2;
	
	tempCDC.SetTextColor(m_titlecr);
	tempCDC.TextOut(m_titlex,m_titley,title,lstrlen(title));

	if(m_pGameView->m_bShowRight)
	{
		DrawCurUserInfo(&tempCDC);
	}

	tempCDC.SetTextColor(RGB(0,0,0));
	tempCDC.SelectObject(pOldFont);

	font.DeleteObject();

	drawSignal(&tempCDC);
	dc.BitBlt(0,0,width,height,&tempCDC,0,0,SRCCOPY);

	tempCDC.SelectObject(pOldBitmap);
	BufferBmp.DeleteObject();
	tempCDC.DeleteDC();


	//if(m_ie.m_hWnd && szAdvUrl!="" && m_pGameView->m_bShowRight && !m_ie.get_Busy())
	//	m_ie.Refresh();
	return;
}
//绘制框架
void CLoveSendClassInExe::DrawViewFrame(CDC * pDC, int iWidth, int iHeight)
{
	//上下
	CGameImageHelper helptc(&m_tc);
	CGameImageHelper helpbc(&m_bc);
	//左右
	CGameImageHelper helplc(&m_lc);
	CGameImageHelper helprc(&m_rc);

	//边角
	//上
	CGameImageHelper helplt(&m_lt);
	CGameImageHelper helprt(&m_rt);
	//边界down
	CGameImageHelper helplb(&m_lb);
	CGameImageHelper helprb(&m_rb);
//	CGameImageHelper helpGWnd(&m_gameWnd);



	if (false)
	{
		if(helptc.GetWidth() > 0)
		{
			//////////////////////////////////////
			int i = 0;
			for(; i < iWidth - helptc.GetWidth(); i+=helptc.GetWidth())
				helptc.BitBlt(pDC->GetSafeHdc(),i,0);

			///Kylin 20090104 防止由于图片过宽画不全而刷新不到的问题
			helptc.BitBlt(pDC->GetSafeHdc(),i,0,iWidth-i,helptc.GetHeight(),0,0);
			//////////////////////////////////////
		}	
		if(helpbc.GetWidth() > 0)
		{
			for(int i = 0; i < iWidth -helpbc.GetWidth() ; i += helpbc.GetWidth())
				helpbc.BitBlt(pDC->GetSafeHdc(),i,iHeight - helpbc.GetHeight());
		}
		if(helplc.GetWidth() > 0)
		{
			for(int i = 0; i < iHeight; i+=helplc.GetHeight())
			{
				helplc.BitBlt(pDC->GetSafeHdc(),0,i);
				helprc.BitBlt(pDC->GetSafeHdc(),iWidth-helprc.GetWidth(),i);
			}
		}
		/// 绘制边框
		helplt.BitBlt(pDC->GetSafeHdc(),0,0);	
		helprt.BitBlt(pDC->GetSafeHdc(),iWidth - helprt.GetWidth(),0);	
		helplb.BitBlt(pDC->GetSafeHdc(),0,iHeight - helplb.GetHeight());	
		helprb.BitBlt(pDC->GetSafeHdc(),iWidth - helprb.GetWidth(),iHeight - helprb.GetHeight());	

	}

	char szRcFile[64] = {0};
	_stprintf(szRcFile,"..\\config\\%s",m_skinmgr.GetGameSkinBcfFileName().GetBuffer());
	CBcfFile f(szRcFile);

	CRect rc[9];

	char* szKey[9] = {"1_TL","2_TC","3_TR","4_LC","5_RC","6_BL","7_BC","8_BR"};
	enum emSoduku
	{
		TL = 0,	//左上截图
		TC = 1, //中上截图
		TR = 2, //右上截图
		LC = 3, //左中截图
		RC = 4, //右中截图
		BL = 5, //左下截图
		BC = 6, //中下截图
		BR = 7, //右下截图
	};

	for(int i = 0; i < 9; ++i)
	{
		CString strPos;
		strPos = f.GetKeyVal(_T("Sudoku"),szKey[i],"0,0,0,0");
		rc[i].left	  = ::atoi(::strtok(strPos.GetBuffer(),","));
		rc[i].top	  = ::atoi(::strtok(NULL,","));
		rc[i].right  = rc[i].left + ::atoi(::strtok(NULL,","));
		rc[i].bottom = rc[i].top +  ::atoi(::strtok(NULL,","));
	}
	
	//TL = 0,左上截图
	helplc.BitBlt(pDC->GetSafeHdc(),
		rc[TL].left,
		rc[TL].top,
		rc[TL].Width(),
		rc[TL].Height(),
		rc[TL].left,
		rc[TL].top);

	//TC = 1,中上截图
	helplc.StretchBlt(pDC->GetSafeHdc(),
		rc[TC].left,
		rc[TC].top,
		iWidth - rc[TL].Width() - rc[TR].Width(),
		rc[TC].Height(),
		rc[TC].left,rc[TC].top,
		rc[TC].Width(),
		rc[TC].Height());

	//TR = 2,右上截图
	helplc.BitBlt(pDC->GetSafeHdc(),
		iWidth - rc[TR].Width(),
		rc[TR].top,
		rc[TR].Width(),
		rc[TR].Height(),
		rc[TR].left,
		rc[TR].top);

	// LC = 3,左中截图
	helplc.StretchBlt(pDC->GetSafeHdc(),
		rc[LC].left,
		rc[TL].Height(),
		rc[LC].Width(),
		iHeight - rc[TL].Height() - rc[BL].Height(),
		rc[LC].left,
		rc[LC].top,
		rc[LC].Width(),
		rc[LC].Height());

	// RC = 4,右中截图
	helplc.StretchBlt(pDC->GetSafeHdc(),
		iWidth - rc[RC].Width(),
		rc[TR].Height(),
		rc[RC].Width(),
		iHeight - rc[TR].Height() - rc[BR].Height(),
		rc[RC].left,
		rc[RC].top,
		rc[RC].Width(),
		rc[RC].Height());

	// BL = 5,左下截图
	helplc.BitBlt(pDC->GetSafeHdc(),
		rc[BL].left,
		iHeight - rc[BL].Height(),
		rc[BL].Width(),
		rc[BL].Height(),
		rc[BL].left,
		rc[BL].top);

	// BC = 6,中下截图
	helplc.StretchBlt(pDC->GetSafeHdc(),
		rc[BL].Width(),
		iHeight - rc[BC].Height(),
		iWidth - rc[BL].Width() - rc[BR].Width(),
		rc[BC].Height(),
		rc[BC].left,
		rc[BC].top,
		rc[BC].Width(),
		rc[BC].Height());

	// BR = 7,右下截图
	helplc.BitBlt(pDC->GetSafeHdc(),
		iWidth - rc[BR].Width(),
		iHeight - rc[BR].Height(),
		rc[BR].Width(),
		rc[BR].Height(),
		rc[BR].left,
		rc[BR].top);


    // 按钮背景图
    CGameImageHelper helpPropBack(&m_propBackImage);
    CGameImageHelper helpPropBack2(&m_propBackImage2);
    CDC destdc;
    destdc.CreateCompatibleDC(pDC);
    destdc.SelectObject(helpPropBack);
    ::TransparentBlt(pDC->GetSafeHdc(),m_propBackRect.left, m_propBackRect.top, 
                     m_propBackRect.Width(), m_propBackRect.Height(),
                     destdc.GetSafeHdc(),0,0,helpPropBack.GetWidth(),helpPropBack.GetHeight(),RGB(255,0,255));
    destdc.SelectObject(helpPropBack2);
    ::TransparentBlt(pDC->GetSafeHdc(),m_propBackRect2.left, m_propBackRect2.top, 
                    m_propBackRect2.Width(), m_propBackRect2.Height(),
                    destdc.GetSafeHdc(),0,0,helpPropBack2.GetWidth(),helpPropBack2.GetHeight(),RGB(255,0,255));
    destdc.DeleteDC();

	DrawCurUserInfo(pDC);

}

void CLoveSendClassInExe::DrawCurUserInfo(CDC *pDC)
{
	//显示用户详细信息，可配置显示项目，modify by lxl 2010-10-15
	//读取显示用户信息配置文件
	CBcfFile bcfFile( CBcfFile::GetAppPath () + "..\\GameUserInfor.bcf");

	//初始化默认显示项目的标识
	bool bUserName = bcfFile.GetKeyVal(_T("InforDefaul"), _T("IsHaveUserName"), 0); //用户名
	bool bUserID = bcfFile.GetKeyVal(_T("InforDefaul"), _T("IsHaveUserID"), 0);		//ID
	bool bNickName = bcfFile.GetKeyVal(_T("InforDefaul"), _T("IsHaveNickName"), 0);	//昵称
	bool bScore = bcfFile.GetKeyVal(_T("InforDefaul"), _T("IsHaveScore"), 0);	//积分
	bool bCoin = bcfFile.GetKeyVal(_T("InforDefaul"), _T("IsHaveCoin"), 0);			//金币	
	bool bCharm = bcfFile.GetKeyVal(_T("InforDefaul"), _T("IsHaveCharm"), 0);		//魅力
	bool bWealthLevel = bcfFile.GetKeyVal(_T("InforDefaul"), _T("IsHaveWealthLevel"), 0);	//财富等级

	CString szCharm = bcfFile.GetKeyVal(_T("InfoName"), _T("Charm"),_T("魅  力："));
	CString szCoin = bcfFile.GetKeyVal(_T("InfoName"), _T("Coin"),_T("金  币："));
	CString szNickName = bcfFile.GetKeyVal(_T("InfoName"), _T("NickName"),_T("昵  称："));
	CString szScore = bcfFile.GetKeyVal(_T("InfoName"), _T("Score"),_T("积  分："));
	CString szUserID = bcfFile.GetKeyVal(_T("InfoName"), _T("UserID"),_T("Ｉ  Ｄ："));
	CString szUserName = bcfFile.GetKeyVal(_T("InfoName"), _T("UserName"),_T("用户名："));
	CString szWealthLevel = bcfFile.GetKeyVal(_T("InfoName"), _T("WealthLevel"),_T("等  级："));


	/// 如果不显示右边窗口，则直接返回
	if (!m_pGameView->m_bShowRight)
	{
		return;
	}
	if(m_CurpUserInfoClicked==NULL)
		return ;
	CRect ClientRect;
	//if(m_pGameView->m_ShowType == 1)
	//	m_pGameView->GetClientRect(&ClientRect);
	//else
	GetClientRect(&ClientRect);
	int x,y,h;
	TCHAR sz[100];
	y=0;
	//头像
	//百家乐 modify by wlr 20090716
	{
		char GameCode[20];
		::memset(GameCode,0,sizeof(GameCode));
		GetGameCode(GameCode);
		CString str = GameCode;
		//if (str != "10301800" && 
		//	str != "30501800" &&
		//	str != "10901800" &&
		//	str != "11901800" )
		//{
		//	m_pGameView->DrawLogo(pDC,m_CurpUserInfoClicked->bLogoID,ClientRect.right-m_logx,m_logy+8,false,m_CurpUserInfoClicked->dwUserID,false,true);
		//}
	}
	//m_pGameView->DrawLogo(pDC,m_CurpUserInfoClicked->bLogoID,ClientRect.right-m_logx,m_logy+8,false,m_CurpUserInfoClicked->dwUserID,false,true);
	//end of modify by wlr 20090716

	//TCHAR szBug[256]={0};
	//wsprintf(szBug,"CLoveSendClass::DrawCurUserInfo():%d %d",m_iPaintCount++,GetTickCount());
	////OutputDebugString(szBug);

	//信息
	x=ClientRect.right-m_u1x;
	//	SetRect(&rect,x,
	y=m_u1y;
	h=m_u1h;
	pDC->SetTextColor(m_usercr);
	char ch[30];
	::memset(ch,0,sizeof(ch));
	if (m_CurpUserInfoClicked->dwUserID == GetMeUserInfo()->dwUserID)
	{
		if ( lstrlen(m_CurpUserInfoClicked->szName)>28)
		{
			GetChar(m_CurpUserInfoClicked->szName,ch,28);
			ch[25]='.';
			ch[26]='.';
			ch[27]='.';
		}
		else
		{
			GetChar(m_CurpUserInfoClicked->szName,ch,28);
		}
	}
	else
	{
		if ( lstrlen(m_CurpUserInfoClicked->nickName)>28 )
		{
			GetChar(m_CurpUserInfoClicked->nickName,ch,28);
			ch[25]='.';
			ch[26]='.';
			ch[27]='.';
		}
		else
		{
			GetChar(m_CurpUserInfoClicked->nickName,ch,28);
		}
	}

	//屏蔽百家乐相关功能 add by wlr 20090716
	char GameCode[20];
	::memset(GameCode,0,sizeof(GameCode));
	GetGameCode(GameCode);
	CString str = GameCode;

	int i = 0;
	if(bUserName)
	{
		if (m_CurpUserInfoClicked->dwUserID == GetMeUserInfo()->dwUserID)
		{
			pDC->TextOut(x,y+h*i++,sz,wsprintf(sz,"%s%s",szUserName, ch));
		}
		else
		{
			pDC->TextOut(x,y+h*i++,sz,wsprintf(sz,""));
		}
	}

	char ch1[30];
	::memset(ch1,0,sizeof(ch1));
	if ( lstrlen(m_CurpUserInfoClicked->nickName)>28 )
	{
		GetChar(m_CurpUserInfoClicked->nickName,ch1,28);
		ch[25]='.';
		ch[26]='.';
		ch[27]='.';
	}
	else
	{
		GetChar(m_CurpUserInfoClicked->nickName,ch1,28);
	}


	//OutputDebugMsg("ClientDLL>>绘制玩家[%d]信息栏 x[%d] y[%d] ",m_CurpUserInfoClicked->dwUserID,x,y);

	
	if(NULL != m_CurpUserInfoClicked)
	{
		////pDC->TextOut(x,y+h*i++,sz,wsprintf(sz,"等级:%s",m_pGameInfo->pOrderName(m_CurpUserInfoClicked->dwPoint)));
		//if(m_pGameInfo->uComType!=TY_MONEY_GAME)
		//{
		//	//pDC->TextOut(x,y+h*i++,sz,wsprintf(sz,"积分:%d",m_CurpUserInfoClicked->dwPoint-GetNormalOrderBasePoint(m_CurpUserInfoClicked->dwPoint)));
		//}	
		if (bNickName)
		{
			int tempy = y+h*i++;
			CRect rectName(x, tempy, x+400, tempy+20);
			wsprintf(sz,"%s%s",szNickName, ch1);
			pDC->DrawText(sz, rectName, DT_TOP|DT_LEFT|DT_SINGLELINE|DT_END_ELLIPSIS);//20090915昵称太长出来，改成DrawText方式显示
		}
		
		if(bUserID)
			pDC->TextOut(x,y+h*i++,sz,wsprintf(sz,"%s%d",szUserID, m_CurpUserInfoClicked->dwUserID));
		
		

  //      // PengJiLin, 2010-6-12, 鸥朴样式显示的内容不一样
  //      BYTE byType = m_UserListDlg.m_UserList.GetUserListDefineType();
  //      switch(byType)
  //      {
  //      case 1:         // 鸥朴样式
		//	if(bScore)
		//		pDC->TextOut(x,y+h*i++,sz,wsprintf(sz,"积分  :%d",m_CurpUserInfoClicked->dwPoint));
  //          
		//	if(bWealthLevel)
		//		pDC->TextOut(x,y+h*i++,sz,wsprintf(sz,"等级  :%s",m_pGameInfo->pOrderName(m_CurpUserInfoClicked->dwPoint)));
  //          break;
  //      default:
  //          {
  //              //pDC->TextOut(x,y+h*i++,sz,wsprintf(sz,"昵称  :%s",ch1));
  //              TCHAR szNum[32]; GlbGetNumString(szNum, m_CurpUserInfoClicked->dwMoney, Glb().m_nPowerOfGold, Glb().m_bUseSpace, Glb().m_strSpaceChar);
		//		
		//		if(bCoin)
		//			pDC->TextOut(x,y+h*i++,sz,wsprintf(sz,"金币  :%s",szNum));

		//		if(bCharm)
		//			pDC->TextOut(x,y+h*i++,sz,wsprintf(sz,"魅力  :%d",m_CurpUserInfoClicked->dwFascination));

		//		if(bWealthLevel)
		//			pDC->TextOut(x,y+h*i++,sz,wsprintf(sz,"等级  :%s",GetMoneyOrderName(m_CurpUserInfoClicked->dwMoney)) );

  //          }
  //          break;
		//}

		//modify by lxl 2010-10-15 根据配置文件显示信息项目
		if(bScore)
			pDC->TextOut(x,y+h*i++,sz,wsprintf(sz,"%s%d",szScore, m_CurpUserInfoClicked->dwPoint));

		if(bCoin)
		{
			TCHAR szNum[128]; 
			GlbGetNumString(szNum, m_CurpUserInfoClicked->i64Money, Glb().m_nPowerOfGold, Glb().m_bUseSpace, Glb().m_strSpaceChar);
			pDC->TextOut(x,y+h*i++,sz,wsprintf(sz,"%s%s",szCoin, szNum));
		}

		if(bCharm)
			pDC->TextOut(x,y+h*i++,sz,wsprintf(sz,"%s%d",szCharm, m_CurpUserInfoClicked->dwFascination));

		if(bWealthLevel)
			pDC->TextOut(x,y+h*i++,sz,wsprintf(sz,"%s%s",szWealthLevel, GetMoneyOrderName(m_CurpUserInfoClicked->i64Money)) );
	}
	else
	{
   //     // PengJiLin, 2010-6-12, 鸥朴样式显示的内容不一样
   //     BYTE byType = m_UserListDlg.m_UserList.GetUserListDefineType();
   //     switch(byType)
   //     {
   //     case 1:
			//pDC->TextOut(x,y+h*i++,sz,wsprintf(sz,"ID    :"));
			//pDC->TextOut(x,y+h*i++,sz,wsprintf(sz,"昵称  :"));
			//pDC->TextOut(x,y+h*i++,sz,wsprintf(sz,"积分  :"));
			//pDC->TextOut(x,y+h*i++,sz,wsprintf(sz,"等级  :"));
   //         break;
   //     default:
			//pDC->TextOut(x,y+h*i++,sz,wsprintf(sz,"ID    :"));
			//pDC->TextOut(x,y+h*i++,sz,wsprintf(sz,"昵称  :"));
			//pDC->TextOut(x,y+h*i++,sz,wsprintf(sz,"金币  :"));
			//pDC->TextOut(x,y+h*i++,sz,wsprintf(sz,"魅力  :"));
			//pDC->TextOut(x,y+h*i++,sz,wsprintf(sz,"等级  :"));
   //         break;
   //     }

		//modify by lxl, 2010-10-15, 根据配置文件显示信息项目
		if(bUserID)
			pDC->TextOut(x,y+h*i++,sz,wsprintf(sz,szUserID));
		if(bNickName)
			pDC->TextOut(x,y+h*i++,sz,wsprintf(sz,szNickName));
		if(bScore)
			pDC->TextOut(x,y+h*i++,sz,wsprintf(sz,szScore));
		if(bCoin)
			pDC->TextOut(x,y+h*i++,sz,wsprintf(sz,szCoin));
		if(bCharm)
			pDC->TextOut(x,y+h*i++,sz,wsprintf(sz,szCharm));
		if(bWealthLevel)
			pDC->TextOut(x,y+h*i++,sz,wsprintf(sz,szWealthLevel));
	}
	/*pDC->TextOut(x,y+(2+k)*h,sz,wsprintf(sz,"赢:%d",m_CurpUserInfoClicked->uWinCount));
	pDC->TextOut(x,y+(3+k)*h,sz,wsprintf(sz,"和:%d",m_CurpUserInfoClicked->uMidCount));
	pDC->TextOut(x,y+(4+k)*h,sz,wsprintf(sz,"输:%d",m_CurpUserInfoClicked->uLostCount));
	unsigned int count=m_CurpUserInfoClicked->uLostCount+m_CurpUserInfoClicked->uMidCount+m_CurpUserInfoClicked->uWinCount+m_CurpUserInfoClicked->uCutCount;
	pDC->TextOut(x,y+(5+k)*h,sz,wsprintf(sz,"总局数:%d",count));
	UINT uCutPercent=(m_CurpUserInfoClicked->uCutCount>0)?(UINT)((double)(m_CurpUserInfoClicked->uCutCount*100)/(double)count):0;
	pDC->TextOut(x,y+(6+k)*h,sz,wsprintf(sz,"逃跑率:%d%%",uCutPercent));
	*/


	//if(m_iPaintCount<1)
	//	SetTimer(1,250,NULL);
}

//016用户进入
bool CLoveSendClassInExe::GameUserCome(BYTE bDeskStation, UserItemStruct * pUserItem, bool bWatchUser)
{
	//====用户加入
	if (bDeskStation<MAX_PEOPLE)
	{
		//设置变量
		if (bWatchUser) 
		{
			INT_PTR i=0;
			for (i=0;i<m_WatchPtrArray.GetCount();i++)
			{
				if (m_WatchPtrArray.GetAt(i)==NULL) break;
			}
			m_WatchPtrArray.SetAtGrow(i,pUserItem);
		}
		else
		{
			m_pUserInfo[bDeskStation]=pUserItem;
			//m_bDeskStation[bDeskStation]=1;	
			//this->InvalidateRect(m_UserInfoRect[bDeskStation]);
		}

		if(pUserItem->GameUserInfo.dwUserID==GetMeUserInfo()->dwUserID)
		{
			m_UserListDlg.AddGameUser(pUserItem, true);
			if (m_pRightFrame)
			{
				m_pRightFrame->ShowUserInfo(pUserItem);
			}
		}
		//设置界面
		if(GetComType()!=TY_MATCH_GAME)
		{//比赛时,只显示自己
			//if(pUserItem->GameUserInfo.dwUserID==GetMeUserInfo()->dwUserID)
			m_UserListDlg.AddGameUser(pUserItem);
		}

		if (bWatchUser) 
		{
			BYTE bDeskStation=pUserItem->GameUserInfo.bDeskStation;
			if (m_pUserInfo[bDeskStation]!=NULL)
				m_UserListDlg.SetWatchUser(pUserItem,m_pUserInfo[bDeskStation]->GameUserInfo.nickName);
		}
		else 
		{
			m_pGameView->SetUserInfo(ViewStation(bDeskStation),pUserItem);
		}

		return true;
	}
	return false;
}
//015用户离开
bool CLoveSendClassInExe::GameUserLeft(BYTE bDeskStation, UserItemStruct * pUserItem, bool bWatchUser)
{
	if (bDeskStation<MAX_PEOPLE)
	{
		//设置变量
		if (bWatchUser==true) 
		{
			UserItemStruct * pFindUserItem=NULL;
			for (INT_PTR i=0;i<m_WatchPtrArray.GetCount();i++)
			{
				pFindUserItem=(UserItemStruct *)m_WatchPtrArray.GetAt(i);
				if (NULL == pFindUserItem)
				{
					continue;
				}
				if (pUserItem->GameUserInfo.dwUserID==pFindUserItem->GameUserInfo.dwUserID)
				{
					m_WatchPtrArray.SetAt(i,NULL);
					break;
				}
			}
		}
		else
		{
			m_pUserInfo[bDeskStation]=NULL;
		}
		// 从本桌玩家列表中清除数据
		UserItemStruct * pFindUserItem=NULL;
		INT_PTR uCount=m_ThisDeskPtrArray.GetCount();
		for (INT_PTR i=0;i<uCount;++i)
		{
			pFindUserItem=(UserItemStruct *)m_ThisDeskPtrArray.GetAt(i);
			if (pUserItem->GameUserInfo.dwUserID
				==pFindUserItem->GameUserInfo.dwUserID)
			{
				m_UserListDlg.DeleteGameUser(pFindUserItem);
				delete pFindUserItem;
				pFindUserItem = NULL;
				m_ThisDeskPtrArray.RemoveAt(i);
				if (bWatchUser==false)
				{
					if (m_pGameView) m_pGameView->SetUserInfo(ViewStation(bDeskStation),NULL);
				}
				break;
			}
		}

		//设置界面
		if(pUserItem->GameUserInfo.dwUserID==m_CurpUserInfoClicked->dwUserID)
		{
			m_CurpUserInfoClicked=GetMeUserInfo();
			NavigateVirFace();
		}

		return true;
	}
	return false;
}

//用户同意
bool CLoveSendClassInExe::UserAgreeGame(BYTE bDeskStation)
{
	if ((bDeskStation<MAX_PEOPLE)&&(m_pUserInfo[bDeskStation]!=NULL))
	{
		//设置界面
		bool bRet = m_UserListDlg.UpdateGameUser(m_pUserInfo[bDeskStation]);
		//DebugPrintf("UserAgreeGame() UpdateGameUser-%s--%d", bRet?"成功":"失败",
		//	m_pUserInfo[bDeskStation]->GameUserInfo.bUserState);
		m_pGameView->UpdateUserInfo(ViewStation(bDeskStation));
		return true;
	}
	return false;
}
void CLoveSendClassInExe::NavigateVirFace()
{
	if(!m_CurpUserInfoClicked)
		return;
	//CRect rect;
	//GetClientRect(&rect);
	//rect.top+=24;
	//rect.left=rect.right-3-207;
	//rect.right=rect.left+207;
	//rect.bottom=rect.top+193;
	/// 只有在需要显示右边栏的时候才更新界面
	if(m_pGameView->m_bShowRight)
	{
#if 0
		InvalidateRect(&m_UserInfoRect,false);
#else
		InvalidateRect(&m_UserInfoRect);
#endif
	}
}
void CLoveSendClassInExe::OnSetPropImages()
{
	//OutputDebugString("点了向下翻页按钮");
	int propCout = (int)Glb().userPropLibrary.GetCount();

//#ifdef _USE_PROP_PAGE_SW_
	//设置翻页
	if(m_curPropPage*PEER_PAGE>=propCout)
		m_curPropPage--;
	if(m_curPropPage<0)
		m_curPropPage=0;
	if(m_curPropPage>0)
	{
		//wushuqun 2009.5.19
		//设置焦点
		m_BtPropGoAhead.EnableWindow(TRUE);
		m_Btbt6.EnableWindow(TRUE);
		m_BtPropGoAhead.SetFocus();
	}
	else
	{
		m_BtPropGoAhead.EnableWindow(FALSE);
		m_Btbt6.EnableWindow(FALSE);
	}
	if((m_curPropPage+1)*PEER_PAGE<propCout)
	{
		//wushuqun 2009.5.19
		//设置焦点
		m_BtPropGoNext.EnableWindow(TRUE);
		m_Btbt7.EnableWindow(TRUE);
		m_BtPropGoNext.SetFocus();
	}
	else
	{
		m_BtPropGoNext.EnableWindow(FALSE);
		m_Btbt7.EnableWindow(FALSE);
	}
//#else
//	m_curPropPage = 0;
//#endif

	_TAG_USERPROP *userProp=NULL;
	CString key=TEXT("config");
	//TCHAR path[MAX_PATH];
	CString skinfolder;

	CString s=CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f( m_iniFile);
	CString skinHall = CBcfFile::GetAppPath()+"..\\" + m_skinmgr.GetSkinPath();

	TCHAR	szPropImagePath[MAX_PATH];



	if (CBcfFile::IsFileExist(s + "config\\" + m_skinmgr.GetGameSkinBcfFileName()))  /// add by wxx 2010.7.1
	{ 
		wsprintf(szPropImagePath, TEXT("%s\\prop\\"), s + "image\\game");
	} 
	else 
	{ 
		wsprintf(szPropImagePath, TEXT("%sprop\\"), s + "..\\" + m_skinmgr.GetGameSkinPath());
	} 


	TCHAR szPropImage[MAX_PATH]={0};
	for(int i=0;i<PEER_PAGE;i++)
	{
		int curPropIndex=m_curPropPage*PEER_PAGE+i;
		CGameImage img;
		HRGN hrgn;
		CNormalBitmapButtonEx *pButton=(CNormalBitmapButtonEx*)GetDlgItem(m_BtPropArray[i]);//取4个道具BUTTON

//         // PengJiLin, 2010-9-27
//         pButton->EnableToShowText(0, false);

		if(curPropIndex<propCout)
		{
            //如果有道具
            pButton->EnableWindow(TRUE);
			userProp=Glb().userPropLibrary.GetAt(curPropIndex);
			//wsprintf(szPropImage, TEXT("%sProp%d.bmp"), szPropImagePath, (userProp->attribAction&0x7F000000)>>24);
			wsprintf(szPropImage, TEXT("%sProp%d.bmp"), szPropImagePath, curPropIndex+1);
			img.SetLoadInfo(szPropImage,true);
			pButton->LoadButtonBitmap(szPropImage,false);
			m_ToolTip.UpdateTipText(userProp->szPropName,this,m_BtPropArray[i]);

            // PengJiLin, 2010-9-27
            pButton->EnableToShowText(userProp->nHoldCount, true);
		}
		else
		{
            //如果没有道具
            pButton->EnableWindow(FALSE);
			img.SetLoadInfo(m_szPropEmptyImage,true);
			pButton->LoadButtonBitmap(m_szPropEmptyImage,false);
			m_ToolTip.UpdateTipText("点击购买道具",this,m_BtPropArray[i]);
		}

		hrgn=AFCBmpToRgn(img,RGB(255,0,255),RGB(1,1,1));
		if(hrgn)
			pButton->SetWindowRgn(hrgn,true);
	}
	//m_BtProp1 固定为VIP功能

	if (m_pRightFrame)
	{
		m_pRightFrame->OnSetPropImages();
	}

	TCHAR	szPropPath[MAX_PATH];
	wsprintf(szPropPath,"..\\%sProp\\Prop9999.bmp",m_skinmgr.GetGameSkinPath());

	m_BtProp1.LoadButtonBitmap(szPropPath,false);
	//更新道具面板
	if (NULL != this->m_pPropPannel)
	{
		m_pPropPannel->ResetData();
	}

}

// PengJiLin, 2010-10-12, 商场道具功能
void CLoveSendClassInExe::OnSetPropImagesS()
{
    int propCout = (int)Glb().userPropLibrary.GetCount();

    //设置翻页
    if(m_curPropPage_S*PEER_PAGE>=propCout)
        m_curPropPage_S--;
    if(m_curPropPage_S<0)
        m_curPropPage_S=0;
    if(m_curPropPage_S>0)
    {
        m_Btbt6.EnableWindow(TRUE);
        m_Btbt6.SetFocus();
    }
    else
    {
        m_Btbt6.EnableWindow(FALSE);
    }

    if((m_curPropPage_S+1)*PEER_PAGE<propCout)
    {
        m_Btbt7.EnableWindow(TRUE);
        m_Btbt7.SetFocus();
    }
    else
    {
        m_Btbt7.EnableWindow(FALSE);
    }


    _TAG_USERPROP *userProp=NULL;
    CString key=TEXT("config");
    CString skinfolder;

    CString s=CBcfFile::GetAppPath ();
    CBcfFile f( m_iniFile);
    CString skinHall = CBcfFile::GetAppPath()+"..\\" + m_skinmgr.GetSkinPath();
	OutputDebugString((LPCTSTR)skinHall);
    TCHAR	szPropImagePath[MAX_PATH];
	OutputDebugString(s + "config\\" + m_skinmgr.GetGameSkinBcfFileName());
    if (CBcfFile::IsFileExist(s + "config\\" + m_skinmgr.GetGameSkinBcfFileName()))
    { 
        wsprintf(szPropImagePath, TEXT("%s\\prop\\"), s + "image\\game");
    } 
    else 
    { 
        wsprintf(szPropImagePath, TEXT("%sprop\\"), s + "..\\" + m_skinmgr.GetGameSkinPath());
    } 


    TCHAR szPropImage[MAX_PATH]={0};
    for(int i=0;i<PEER_PAGE;i++)
    {
        int curPropIndex=m_curPropPage_S*PEER_PAGE+i;
        CGameImage img;
        HRGN hrgn;
        CNormalBitmapButtonEx *pButton = NULL;
        if(0 == i)pButton = (CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON_PROP4);
        else if(1 == i)pButton = (CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON_PROP5);
        else pButton = (CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON_PROP6);

        if(curPropIndex<propCout)
        {
            //如果有道具
            pButton->EnableWindow(TRUE);
            userProp=Glb().userPropLibrary.GetAt(curPropIndex);
            wsprintf(szPropImage, TEXT("%sProp%d.bmp"), szPropImagePath, (userProp->attribAction&0x7F000000)>>24);
            img.SetLoadInfo(szPropImage,true);
            pButton->LoadButtonBitmap(szPropImage,false);
            m_ToolTip.UpdateTipText(userProp->szPropName,this,m_BtPropArray[i]);

        }
        else
        {
            //如果没有道具
            pButton->EnableWindow(FALSE);
            img.SetLoadInfo(m_szPropEmptyImage,true);
            pButton->LoadButtonBitmap(m_szPropEmptyImage,false);
        }

        hrgn=AFCBmpToRgn(img,RGB(255,0,255),RGB(1,1,1));
        if(hrgn)
            pButton->SetWindowRgn(hrgn,true);
    }

}

void CLoveSendClassInExe::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	OnBnClickedExit();
	//AfxGetApp()->m_pMainWnd->PostMessage(WM_GAME_QUIT,0,0);
}

void CLoveSendClassInExe::OnBnClickedExit()
{
	// 需要判断是否正在游戏中
	if ((m_bWatchMode==false)&&GetStationParameter() >= 20 && GetStationParameter() < 23)//!CanLeftDesk())
	{
		//if (IDOK!=AFCMessageBox("你正在游戏中，现在退出将会被扣分，确实要退出吗？",m_pGameInfo->szGameName,AFC_YESNO))//,MB_YESNO|MB_DEFBUTTON2|MB_ICONINFORMATION,this))
		//	return;
		//SendGameData(MDM_GM_GAME_FRAME,ASS_GM_FORCE_QUIT,0);
	}
	OnCancel();
}

void CLoveSendClassInExe::OnBnClickedRest()
{
	/*
	//百家乐 add by wlr 20090717
	char GameCode[20];
	::memset(GameCode,0,sizeof(GameCode));
	GetGameCode(GameCode);
	//if (str == "11901800"
	//	|| str == "10901800"
	//	|| str == "30501800")
	TCHAR szKey[10];
	CString sPath=CBcfFile::GetAppPath();
	CBcfFile fsr( sPath + "..\\SpecialRule.bcf");
	wsprintf(szKey, "%d", m_pGameInfo->uNameID);
	int iResult = fsr.GetKeyVal (_T("BJLType"), szKey, 0);

	if (iResult>0)
	{
		return;
	}
	//end of 百家乐 add by wlr 20090717
	*/

	// TODO: 在此添加控件通知处理程序代码
	m_bMax=false;
	m_bShowMax=false;

	CBcfFile f(CBcfFile::GetAppPath() + _T("GameOption.bcf"));
	UINT uMaxType = f.GetKeyVal("FrameControl","max_type",1);

	if (uMaxType == 2) //修改分辨率
	{
		DEVMODE lpDevMode;
		lpDevMode.dmBitsPerPel=32;
		lpDevMode.dmPelsWidth = m_nWiondwMetricsX;
		lpDevMode.dmPelsHeight = m_nWiondwMetricsY;
		lpDevMode.dmSize = sizeof(lpDevMode);
		lpDevMode.dmFields = DM_PELSWIDTH|DM_PELSHEIGHT|DM_BITSPERPEL;
		LONG result;
		result = ChangeDisplaySettings(&lpDevMode,0);
		if (result == DISP_CHANGE_SUCCESSFUL)
		{
			ChangeDisplaySettings(&lpDevMode,CDS_UPDATEREGISTRY);
			// 　　//使用CDS_UPDATEREGISTRY表示次修改是持久的，
			// 　　//并在注册表中写入了相关的数据 
		}
		else
		{
			//AfxMessageBox(L"修改失败，恢复原有设置!");
			ChangeDisplaySettings(NULL,0);
		}
	}


	MoveWindow(0,0,1024, 738);
	//MoveWindow(0,0,1000,700);
	CenterWindow();
	m_btMax.ShowWindow(SW_SHOW);
	m_btRest.ShowWindow(SW_HIDE);

	this->m_pGameRoom->PostMessage(WM_WINDOW_MOVE,0,1);

	if (uMaxType == 2) //修改分辨率
	{
		//通知大厅UI作主相应调整
		this->m_pGameRoom->PostMessage(WM_SCREEN_SIZE_CHANGE,0,2);
	}

}

void CLoveSendClassInExe::OnBnClickedQuitGame()
{
    // PengJiLin, 2011-4-18, 在线时长、局数送金币 询问
    CString strInfo = "";
    GetSendMoneyFormatInfo(strInfo);
    strInfo += "您确定要离开吗?";

    if(m_SendMoneyData.dwMoneyOnTimes > 0 || m_SendMoneyData.dwMoneyOnCounts > 0)
    {
        if(IDOK != AFCMessageBox(strInfo, "提示", AFC_YESNO, this))
        {
            return;
        }
    }

	// TODO: 在此添加控件通知处理程序代码
	OnBnClickedExit();
}

void CLoveSendClassInExe::OnBnClickedMax()
{
	/*
	//百家乐 add by wlr 20090717
	char GameCode[20];
	::memset(GameCode,0,sizeof(GameCode));
	GetGameCode(GameCode);
	//if (str == "11901800"
	//	|| str == "10901800"
	//	|| str == "30501800")
	TCHAR szKey[10];
	CString sPath=CBcfFile::GetAppPath();
	CBcfFile fsr( sPath + "..\\SpecialRule.bcf");
	wsprintf(szKey, "%d", m_pGameInfo->uNameID);
	int iResult = fsr.GetKeyVal (_T("BJLType"), szKey, 0);

	if (iResult>0)
	{
		return;
	}
	//end of 百家乐 add by wlr 20090717
	*/
	
	// TODO: 在此添加控件通知处理程序代码
	m_bMax=true;
	m_bShowMax=true;

	//修改分辨率

	CBcfFile f(CBcfFile::GetAppPath() + _T("GameOption.bcf"));
	UINT uMaxType = f.GetKeyVal("FrameControl","max_type",1);

	if (uMaxType == 1) //普通最大化
	{
		ShowWindow(SW_RESTORE);
	}
	else if (uMaxType == 2) //修改分辨率
	{

		DEVMODE lpDevMode;
		lpDevMode.dmBitsPerPel=32;
		lpDevMode.dmPelsWidth=1024;
		lpDevMode.dmPelsHeight=768;
		lpDevMode.dmSize = sizeof(lpDevMode);
		lpDevMode.dmFields = DM_PELSWIDTH|DM_PELSHEIGHT|DM_BITSPERPEL;
		LONG result;
		result = ChangeDisplaySettings(&lpDevMode,0);
		if (result == DISP_CHANGE_SUCCESSFUL)
		{
			ChangeDisplaySettings(&lpDevMode,CDS_UPDATEREGISTRY);
			// 　　//使用CDS_UPDATEREGISTRY表示次修改是持久的，
			// 　　//并在注册表中写入了相关的数据 
		}
		else
		{
			//AfxMessageBox(L"修改失败，恢复原有设置!");
			ChangeDisplaySettings(NULL,0);
		}
	}

	CRect rect;
	::SystemParametersInfo(SPI_GETWORKAREA,NULL,&rect,NULL);
	if (uMaxType == 1) //普通最大化
	{
		MoveWindow(0,0,rect.Width(),rect.Height());
	}
	m_btMax.ShowWindow(SW_HIDE);
	m_btRest.ShowWindow(SW_SHOW);

	if (uMaxType == 2) //修改分辨率
	{
		//通知大厅UI作主相应调整
		this->m_pGameRoom->PostMessage(WM_SCREEN_SIZE_CHANGE,0,1);
	}

	SetForegroundWindow();
}

void CLoveSendClassInExe::OnBnClickedHide()
{
	//屏蔽百家乐相关功能 add by wlr 20090716
	TCHAR szKey[10];
	CString sPath=CBcfFile::GetAppPath();
	CBcfFile fsr( sPath + "..\\SpecialRule.bcf");
	wsprintf(szKey, "%d", m_pGameInfo->uNameID);
	int iResult = fsr.GetKeyVal (_T("BJLType"), szKey, 0);

	if (iResult>0)
	{
		return;
	}
	//end of add by wlr 20090716

	CGameImage img;
	HRGN hRgn;
	if(m_pGameView->m_bShowRight)
	{
		m_pGameView->m_bShowRight = false;
		m_BtHide.LoadButtonBitmap(m_ShowbtPath.GetBuffer(),false);
		img.SetLoadInfo(m_ShowbtPath.GetBuffer(),true);
		m_BtHide.SetWindowPos(NULL,0,0,img.GetWidth()/4,img.GetHeight(),SWP_NOZORDER|SWP_NOMOVE);
		hRgn=AFCBmpToRgn(img,RGB(255,0,255),RGB(1,1,1));
		if(hRgn)
			m_BtHide.SetWindowRgn(hRgn,true);

		if (m_pRightFrame)
		{
			m_pRightFrame->ShowWindow(SW_HIDE);
		}
	}
	else
	{
		m_pGameView->m_bShowRight = true;
		m_BtHide.LoadButtonBitmap(m_HidebtPath.GetBuffer(),false);
		img.SetLoadInfo(m_HidebtPath.GetBuffer(),true);
		m_BtHide.SetWindowPos(NULL,0,0,img.GetWidth()/4,img.GetHeight(),SWP_NOZORDER|SWP_NOMOVE);
		hRgn=AFCBmpToRgn(img,RGB(255,0,255),RGB(1,1,1));
		if(hRgn)
			m_BtHide.SetWindowRgn(hRgn,true);

		if (m_pRightFrame)
		{
			m_pRightFrame->ShowWindow(SW_SHOW);
		}

	}
	UINT showright = (m_pGameView->m_bShowRight) ? SW_SHOW : SW_HIDE;
	showright = SW_HIDE;
	m_UserListDlg.ShowWindow(showright);
	m_TalkMessage.ShowWindow(showright);
	//	m_BtGameSet.ShowWindow(showright);
	m_Btbt1.ShowWindow(SW_SHOW);
//	m_Btbt2.ShowWindow(showright);
//	m_Btbt3.ShowWindow(showright);
//	m_Btbt4.ShowWindow(showright);

	//{add by zxj 2008/04/07
	m_BtPropGoAhead.ShowWindow(SW_HIDE);
	m_BtPropGoNext.ShowWindow(SW_HIDE);
	m_BtProp1.ShowWindow(showright);
	m_BtProp2.ShowWindow(showright);
	m_BtProp3.ShowWindow(showright);
    m_BtProp4.ShowWindow(showright);
    m_BtProp5.ShowWindow(showright);
	m_BtProp6.ShowWindow(showright);
	m_BtProp7.ShowWindow(showright);
	m_BtProp8.ShowWindow(showright);
	m_BtProp9.ShowWindow(showright);
	m_BtProp10.ShowWindow(showright);
	//end add}

	//m_Btbt5.ShowWindow(showright);
	//m_Btbt6.ShowWindow(showright);
	//m_Btbt7.ShowWindow(showright);
	m_BtExpression.ShowWindow(showright);
	m_InputMessage.ShowWindow(showright);
	m_BtSend.ShowWindow(showright);

    // PengJiLin, 2010-10-8
//    m_Btbt5.ShowWindow(showright);
//    m_Btbt6.ShowWindow(showright);
//    m_Btbt7.ShowWindow(showright);


	CRect rect;
	GetClientRect(&rect);
	FixControlStation(rect.Width(),rect.Height());
	m_pGameView->UpdateViewFace(NULL);
	return;
}

void CLoveSendClassInExe::OnBnClickedMin()
{
	// TODO: 在此添加控件通知处理程序代码
	this->ShowWindow(SW_MINIMIZE);

	//PostMessage(WM_BANK2_TEST,0,0);

	//AFCMessageBox("WM_BANK2_TEST0");
	
}
//014发送函数
int CLoveSendClassInExe::SendGameData(void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode)
{
	if (m_pGameInfo->uisMeUserInfo.GameUserInfo.bDeskNO==255) 
		return 0;

	UINT nBufferLen = uBufLen + sizeof(SendGameDataStructWithoutParam);
	BYTE *buffer = new BYTE[nBufferLen];
	SendGameDataStructWithoutParam *pHead = (SendGameDataStructWithoutParam *)buffer;
	pHead->bMainID		= bMainID;
	pHead->bAssistantID = bAssistantID;
	pHead->bHandleCode	= bHandleCode;
	memcpy(buffer+sizeof(SendGameDataStructWithoutParam), pData, uBufLen);

	m_pGameRoom->PostMessage(WM_SEND_GAME_DATA, (WPARAM)nBufferLen, (LPARAM)buffer);
	return 0;
}
//发送函数
int CLoveSendClassInExe::SendGameData(BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode)
{
	if (m_pGameInfo->uisMeUserInfo.GameUserInfo.bDeskNO==255)
		return 0;
	SendGameDataStructWithoutParam *pHead = new SendGameDataStructWithoutParam;
	pHead->bMainID		= bMainID;
	pHead->bAssistantID = bAssistantID;
	pHead->bHandleCode	= bHandleCode;
	WORD nLen = sizeof(SendGameDataStructWithoutParam);

	if (bAssistantID == ASS_GM_AGREE_GAME && m_pGameInfo->dwRoomRule & GRR_QUEUE_GAME)
	{
		CBcfFile f(CBcfFile::GetAppPath() + _T("GameOption.bcf"));
		UINT _bExit = f.GetKeyVal("QueueRoom", "ContinueExit",1);
		//pHead->bHandleCode = 10;
		if (_bExit == 1 || m_bIsQueueDisMiss || m_pGameInfo->dwRoomRule & GRR_CONTEST)
		{
			pHead->bHandleCode = 1;
			m_pGameRoom->SendMessage(WM_SEND_GAME_DATA, (WPARAM)nLen, (LPARAM)pHead);
			m_pGameRoom->PostMessage(WM_GAME_QUIT,0,0);
			return 0;
		}
	}

	m_pGameRoom->PostMessage(WM_SEND_GAME_DATA, (WPARAM)nLen, (LPARAM)pHead);
	return 0;
}

void CLoveSendClassInExe::OnMove(int x, int y)
{
	int iWindowsWidth = GetSystemMetrics(SM_CXSCREEN);
	int iWindowsHeight = GetSystemMetrics(SM_CYSCREEN);
	if (m_pRightFrame)
	{
		if (m_bShowMax)
		{
			if (iWindowsWidth >= 1280)
			{
				m_vlistx = m_pRightFrame->m_uMaxWidth;
			}
			else
			{
				m_vlistx = m_pRightFrame->m_uMinWidth;
			}
		}
		else
		{
			m_vlistx = m_pRightFrame->m_uMinWidth;
		}
		m_pGameView->m_vlistx = m_vlistx;
	}
	
	if (x != -32000 && y != -32000)
	{
		this->m_pGameRoom->PostMessage(WM_WINDOW_MOVE, 0, 1);
	}
	else
	{
		this->m_pGameRoom->PostMessage(WM_ACTIVE_TO_ROOM, 4, 1);
	}
	// TODO: 在此处添加消息处理程序代码
	CLoadFileBmp::OnMove(x, y);

	//{{Deleted by ZXD
	CRect rect;
	GetClientRect(&rect);
	FixControlStation(rect.right,rect.bottom);
	//Deleted by ZXD}}

	CRect rc;
	::GetWindowRect(m_hWnd,rc);
	int iWRight = rc.Width();
	int iHRight = rc.Height();
// 	CString str;
// 	str.Format("DClient: 1 x = %d, y = %d, Main(%d,%d) %d",x,y,iWRight,iHRight,m_bShowMax);
// 	OutputDebugString(str);

	if (m_pRightFrame)
	{

		CRect rcRight;
		CRect rcMain;
		::GetWindowRect(m_pRightFrame->GetHWND(),&rcRight);
		::GetWindowRect(m_hWnd,rcMain);

		char szRcFile[64] = {0};
		_stprintf(szRcFile,"..\\config\\%s",m_skinmgr.GetGameSkinBcfFileName().GetBuffer());
		CBcfFile f(szRcFile);

		CRect rc[3];
		enum emSoduku
		{
			TC = 0, //中上截图
			RC = 1, //右中截图
			BC = 2, //中下截图
		};

		char* szKey[3] = {"2_TC","5_RC","7_BC"};

		for(int i = 0; i < 3; ++i)
		{
			CString strPos;
			strPos = f.GetKeyVal(_T("Sudoku"),szKey[i],"0,0,0,0");
			rc[i].left	  = ::atoi(::strtok(strPos.GetBuffer(),","));
			rc[i].top	  = ::atoi(::strtok(NULL,","));
			rc[i].right  = rc[i].left + ::atoi(::strtok(NULL,","));
			rc[i].bottom = rc[i].top +  ::atoi(::strtok(NULL,","));
		}


		if (m_bShowMax)
		{
			if (iWindowsWidth >= 1280)
			{			
				::MoveWindow(m_pRightFrame->GetHWND(),x + rcMain.Width() - m_pRightFrame->m_uMaxWidth - rc[RC].Width(),
					y + rc[TC].Height(),m_pRightFrame->m_uMaxWidth,rcMain.Height() - rc[TC].Height() - rc[BC].Height(),
					true);
			}
			else
			{
				::MoveWindow(m_pRightFrame->GetHWND(),x + rcMain.Width() - m_pRightFrame->m_uMinWidth - rc[RC].Width(),
					y + rc[TC].Height(),m_pRightFrame->m_uMinWidth,rcMain.Height() - rc[TC].Height() - rc[BC].Height(),
					true);
			}
		}
		else
		{
			::MoveWindow(m_pRightFrame->GetHWND(),x + rcMain.Width() - m_pRightFrame->m_uMinWidth - rc[RC].Width(),
				y + rc[TC].Height(),m_pRightFrame->m_uMinWidth,rcMain.Height() - rc[TC].Height() - rc[BC].Height(),
				true);
		}
		
	}
}

BOOL CLoveSendClassInExe::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//wushuqun 2009.6.9
	return true;
	return CLoadFileBmp::OnEraseBkgnd(pDC);
}

void CLoveSendClassInExe::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (!m_bMax)
	{
		if (point.y < 28)
		{
			PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
		}
		CDialog::OnLButtonDown(nFlags, point);
	}
}

BOOL CLoveSendClassInExe::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	switch (LOWORD(wParam))
	{
	case IDM_SAVE_MESSAGE:				//保存聊天信息
		{
			m_TalkMessage.OnSave();
			return TRUE;
		}
	case IDM_CLEAR_SCREEN:				//清除聊天屏幕
		{
			m_TalkMessage.OnClearAll();
			return TRUE;
		}
	case IDM_AUTO_SCROLL:				//自动滚动屏幕
		{
			m_TalkMessage.OnAutoScroll();
			return TRUE;
		}
	case IDM_SET_FRIEND:
		{
			m_pGameRoom->PostMessage(WM_FRIEND, 0, m_dwCurrentUserID);
			return TRUE;
		}
	case IDM_SET_EMENY:
		{
			m_pGameRoom->PostMessage(WM_EMENY, 0, m_dwCurrentUserID);
			return TRUE;
		}
	case IDM_GAME_RULE:					//游戏帮助
		{
			//CString strBuffer;
			//ShellExecute(NULL,TEXT("open"),TEXT(Glb().m_CenterServerPara.m_strHelpADDR),NULL,NULL,SW_MAXIMIZE);
			return TRUE;
		}
	case IDM_TALK_COLOR:				//设置聊天颜色
		{
			CColorDialog Color(CGameImageLink::m_ColorResource.m_crTalkTxColor,CC_FULLOPEN,this);
			if (Color.DoModal()==IDOK) 
				CGameImageLink::m_ColorResource.m_crTalkTxColor=Color.GetColor();
			m_InputMessage.SetFocus();
			return TRUE;
		}
	case IDM_USER_INFO:                 //查看用户详细资料
		{
			return OnCmdShowUserInfo();
		}
	case IDM_COPY_USER_NAME:			//复制用户名字
		{
			return OnCmdCopyUserInfo();
		}
	case IDM_GIVE_PROP:
		{
			::PostMessage(m_pGameRoom->GetSafeHwnd(),WM_USER+157,m_dwCurrentUserID,0);
			return TRUE;
		}
	case IDM_KICK_USER://踢人
		{
			return OnCmdKickUser();
		}
    case IDM_KICK_USER_PROP:        // PengJiLin, 2010-9-10, 踢人卡的踢人功能
        {
            return OnCmdKickUserProp();
        }
	case IDM_SET_TALK_OBJECT:			//设置谈话对象
		{
			if (m_dwCurrentUserID!=0L)
			{
				//处理数据
				UserItemStruct * pUserItem=FindOnLineUser(m_dwCurrentUserID);
				if (pUserItem==NULL) return TRUE;

				//设置聊天对象
				//if (pUserItem!=NULL)
				//{
				//	int iIndex=m_TargetPeople.FindString(1,pUserItem->GameUserInfo.nickName);
				//	if (iIndex!=CB_ERR) 
				//		m_TargetPeople.SetCurSel(iIndex);
				//}
			}
			m_InputMessage.SetFocus();
			return TRUE;
		}
	case IDM_CANCEL_TALK:				//取消谈话对象
		{
			//m_TargetPeople.SetCurSel(0);
			m_InputMessage.SetFocus();
			return TRUE;
		}
	case IDM_JOIN_GAME:					//加入游戏
		{
			//效验数据
			if ((m_bWatchMode==false)||(m_pGameInfo->uisMeUserInfo.GameUserInfo.bDeskNO==255)) 
				return TRUE;

			//处理数据

			return TRUE;
		}
	case IDM_EMABLE_WATCH:				//允许旁观
	case IDM_UNEMABLE_WATCH:			//不允许旁观
		{
			if ((m_dwCurrentUserID!=0)&&(m_bWatchMode==false))
			{
				MSG_GM_WatchSet WatchSet;
				WatchSet.dwUserID=m_dwCurrentUserID;
				SendGameData(&WatchSet,sizeof(WatchSet),MDM_GM_GAME_FRAME,ASS_GM_WATCH_SET,LOWORD(wParam)==IDM_EMABLE_WATCH);
			}
			return TRUE;
		}
	case IDM_SHIELD_USER:				//屏蔽用户
		{
			//效验数据
			if ((m_dwCurrentUserID==0L)||(m_dwCurrentUserID==GetMeUserInfo()->dwUserID)) 
				return TRUE;

			//设置数据
			INT_PTR i=0;
			for (i=0;i<m_ShieldArray.GetCount();i++)
			{
				if ((m_ShieldArray.GetAt(i)==0L)||(m_ShieldArray.GetAt(i)==m_dwCurrentUserID))
					break;
			}
			if (i==m_ShieldArray.GetCount()) 
				m_ShieldArray.SetAtGrow(i,m_dwCurrentUserID);

			//插入消息
			UserItemStruct * pUserItem=FindOnLineUser(m_dwCurrentUserID);
			if (pUserItem!=NULL)
			{
				CString strBuffer;
				strBuffer.Format(TEXT("屏蔽玩家 [ %s ] 成功"),pUserItem->GameUserInfo.nickName);
				m_MessageHandle.InsertSystemMessage(strBuffer.GetBuffer());
				strBuffer.ReleaseBuffer();
			}

			return TRUE;
		}
	case IDM_UNSHIELD_USER:				//解除屏蔽用户
		{
			//效验数据
			if ((m_dwCurrentUserID==0L)||(m_dwCurrentUserID==GetMeUserInfo()->dwUserID)) 
				return TRUE;

			//设置数据
			for (INT_PTR i=0;i<m_ShieldArray.GetCount();i++)
			{
				if (m_ShieldArray.GetAt(i)==m_dwCurrentUserID) 
				{
					m_ShieldArray.RemoveAt(i);
					break;
				}
			}

			//插入消息
			UserItemStruct * pUserItem=FindOnLineUser(m_dwCurrentUserID);
			if (pUserItem!=NULL)
			{
				CString strBuffer;
				strBuffer.Format(TEXT("解除屏蔽玩家[%s]成功"),pUserItem->GameUserInfo.nickName);
				m_MessageHandle.InsertSystemMessage(strBuffer.GetBuffer());
				strBuffer.ReleaseBuffer();
			}

			return TRUE;
		}
	case IDM_RED:						
	case IDM_BULE:
	case IDM_GRAY:
	case IDM_GREEN:
	case IDM_WHITE:
	case IDM_YELLOW:
	case IDM_POWER_RED:					//设置聊天颜色
		{
			UINT uColorMenuID[7]={IDM_RED,IDM_YELLOW,IDM_BULE,IDM_GREEN,IDM_WHITE,IDM_POWER_RED,IDM_GRAY};
			COLORREF rcColor[8]={RGB(255,0,0),RGB(255,255,0),RGB(0,255,255),RGB(0,255,0),RGB(255,255,255),RGB(255,0,255),RGB(192,192,192),RGB(255,255,255)};
			UINT i=0;
			for (i=0;i<sizeof(uColorMenuID)/sizeof(uColorMenuID[0]);i++)
			{
				if (LOWORD(wParam)==uColorMenuID[i]) break;
			}
			CGameImageLink::m_ColorResource.m_crTalkTxColor=rcColor[i];
			m_InputMessage.SetFocus();
			return TRUE;
		}
	}
	return __super::OnCommand(wParam, lParam);
}
// 左击用户列表
void CLoveSendClassInExe::OnLBClickUserList(UserItemStruct * pUserItem)
{
	if(m_pPropPannel != NULL)
	{
		m_pPropPannel->m_ctlGiveUser.SetWindowText(pUserItem->GameUserInfo.nickName);
		//m_pPropPannel->UpdateData(true);
	}
	if(m_CurpUserInfoClicked==&pUserItem->GameUserInfo)
		return ;
	m_CurpUserInfoClicked=&pUserItem->GameUserInfo;
	NavigateVirFace();
}
// 双击用户列表
void CLoveSendClassInExe::OnLBDBClickUserList(UserItemStruct * pUserItem)
{
	if(GetMeUserInfo()->dwUserID==pUserItem->GameUserInfo.dwUserID)
		return;
	m_CurpUserInfoClicked=&pUserItem->GameUserInfo;
	CString nick=pUserItem->GameUserInfo.nickName;
	m_InputMessage.SetWindowText(nick+":");
	m_InputMessage.SetFocus();
	int l=nick.GetLength()+1;
	m_InputMessage.SetEditSel(l,l);

}
//打开右键菜单
void CLoveSendClassInExe::OpenUserMenu(UserItemStruct * pUserItem)
{
	// add duanxiaohui 20111111 下面代码杂乱过多(不再使用, 紧作参考作用), 在此扩充一个新的函数
	return OpenUserMenuEx(pUserItem);
	// end duanxiaohui

	//效验数据
	if (pUserItem==NULL) 
	{
		m_dwCurrentUserID=0L;
		return;
	}
	//设置数据
	UserInfoStruct * pUserInfo=&pUserItem->GameUserInfo;
	if (!m_pRightFrame)
	{
		m_dwCurrentUserID=pUserItem->GameUserInfo.dwUserID;
	}

	//建立菜单
	CString strBuffer;
	CMenu UserInfoMenu;
	UserInfoMenu.CreatePopupMenu();
	MENUINFO mi;
	UserInfoMenu.GetMenuInfo(&mi);
	mi.cbSize=sizeof(MENUINFO);
	mi.fMask=MIM_BACKGROUND; 
	mi.hbrBack = CreateSolidBrush(RGB(0xD1,0xF1,0xFF));
	UserInfoMenu.SetMenuInfo(&mi);

	//功能菜单
	UserInfoMenu.AppendMenu(0,IDM_COPY_USER_NAME,TEXT("拷贝用户名"));

	//////////////////////////////////////////////////////////////////////////
	//暂时屏蔽 by fred huang for 曾宪冬 20090403
	if(m_dwCurrentUserID != GetMeUserInfo()->dwUserID)
	{
		UserInfoMenu.AppendMenu(0,IDM_SET_FRIEND,TEXT("添加好友"));
		//UserInfoMenu.AppendMenu(0,IDM_GIVE_PROP,TEXT("赠送道具..."));
	}
	//////////////////////////////////////////////////////////////////////////
	//加入踢人功能
	/// 百家乐类型的游戏无踢人功能
	CString stime;
	stime.Format("%d",CTime::GetCurrentTime());        //当前的时间，String格式
	int curtime=atoi(stime);                        //转换为int
	TCHAR szKey[10];
	CString sPath=CBcfFile::GetAppPath();
	CBcfFile fsr( sPath + "..\\SpecialRule.bcf");
	wsprintf(szKey, "%d", m_pGameInfo->uNameID);
	int iResult = fsr.GetKeyVal (_T("BJLType"), szKey, 0);

	if (iResult==0)
	{
		if(GetMeUserInfo()->iVipTime>curtime)//会员才有此功能
        {
			UserInfoMenu.AppendMenu(0,IDM_KICK_USER,TEXT("请他离开"));
        }
        else        // PengJiLin, 2010-9-10, 可配置是否有踢人卡的踢人功能
        {
            CBcfFile bcfFile( sPath + "..\\Order.bcf");
            int iValue = bcfFile.GetKeyVal(_T("ClassInExe"), _T("UseKickUserProp"), 0);
            if(iValue > 0)  // 显示踢人菜单
            {
                UserInfoMenu.AppendMenu(0,IDM_KICK_USER_PROP,TEXT("请他离开"));
            }
        }
	}

	//if (m_dwCurrentUserID!=GetMeUserInfo()->dwUserID)
	//{
	//	strBuffer.Format(TEXT("与 %s 聊天"),pUserInfo->nickName);
	//	//UserInfoMenu.AppendMenu(0,IDM_SET_TALK_OBJECT,strBuffer);
	//}
	//int iStation=m_TargetPeople.GetCurSel();
	//if ((iStation!=CB_ERR)&&(iStation!=0))
	//{
	//	CString strName;
	//	m_TargetPeople.GetLBText(iStation,strName);
	//	strBuffer.Format(TEXT("取消与 %s 的聊天"),strName);
	//	//UserInfoMenu.AppendMenu(0,IDM_CANCEL_TALK,strBuffer);
	//}
	//加入屏蔽功能
	INT_PTR i;
	for (i=0;i<m_ShieldArray.GetCount();i++)
	{
		if (m_ShieldArray.GetAt(i)==m_dwCurrentUserID) break;
	}
	if (i==m_ShieldArray.GetCount()) 
	{
		if (m_dwCurrentUserID!=GetMeUserInfo()->dwUserID)
		{
			strBuffer.Format(TEXT("屏蔽 %s 的消息"),pUserInfo->nickName);
			UserInfoMenu.AppendMenu(0,IDM_SHIELD_USER,strBuffer);
		}
	}
	else
	{
		strBuffer.Format(TEXT("解除 %s 的消息过滤"),pUserInfo->nickName);
		UserInfoMenu.AppendMenu(0,IDM_UNSHIELD_USER,strBuffer);
	}

	//加入设置旁观
	if ((m_bWatchMode==false)&&(pUserInfo->bDeskStation==GetMeUserInfo()->bDeskStation)
		&&(m_dwCurrentUserID!=GetMeUserInfo()->dwUserID))
	{
		UserInfoMenu.AppendMenu(0,MF_SEPARATOR);
		strBuffer.Format(TEXT("允许 %s 旁观游戏"),pUserInfo->nickName);
		UserInfoMenu.AppendMenu(0,IDM_EMABLE_WATCH,strBuffer);
		strBuffer.Format(TEXT("不允许 %s 旁观游戏"),pUserInfo->nickName);
		UserInfoMenu.AppendMenu(0,IDM_UNEMABLE_WATCH,strBuffer);
	}


	//用户信息菜单
	//UINT uAllCount=pUserInfo->uWinCount+pUserInfo->uLostCount+pUserInfo->uCutCount+pUserInfo->uMidCount;		//由于断线、强退，在数据库里均算输局，所有，在总局数里不再把断线、强退纳入计算范围 zxj 2009-9-29
	UINT uAllCount=pUserInfo->uWinCount+pUserInfo->uLostCount+pUserInfo->uMidCount;

	UINT uCutPercent=(pUserInfo->uCutCount>0)?(UINT)((double)(pUserInfo->uCutCount*100)/(double)uAllCount):0;
	UserInfoMenu.AppendMenu(0,MF_SEPARATOR);


	if(pUserInfo->iVipTime>curtime)
		strBuffer.Format("用户名：%s ( VIP 会员 )",pUserInfo->nickName);
	else
		strBuffer.Format("用户名：%s",pUserInfo->nickName);
	/*
	if (pUserInfo->bMember==0) strBuffer.Format(TEXT("用户名：%s"),pUserInfo->nickName);
	else 
	if (pUserInfo->bMember==1) strBuffer.Format(TEXT("用户名：%s ( 普通会员 )"),pUserInfo->nickName);
	else 
	if (pUserInfo->bMember==2) strBuffer.Format(TEXT("用户名：%s ( 高级会员 )"),pUserInfo->nickName);
	else
	if (pUserInfo->bMember==3) strBuffer.Format(TEXT("用户名：%s ( 特邀嘉宾 )"),pUserInfo->nickName);
	else 
	if (pUserInfo->bMember==4) strBuffer.Format(TEXT("用户名：%s ( 永久会员 )"),pUserInfo->nickName);
	else 
	strBuffer.Format(TEXT("用户名：%s ( 元老玩家 )"),pUserInfo->nickName);*/
	UserInfoMenu.AppendMenu(0,0,strBuffer);



	strBuffer.Format(TEXT("积分：%d"),pUserInfo->dwPoint-GetNormalOrderBasePoint(pUserInfo->dwPoint));
	UserInfoMenu.AppendMenu(0,0,strBuffer);
	strBuffer.Format(TEXT("等级：%s"),m_pGameInfo->pOrderName(pUserInfo->dwPoint));
	UserInfoMenu.AppendMenu(0,0,strBuffer);

    // PengJiLin, 2010-6-11, 如果是鸥朴样式，有些菜单不显示
    BYTE byType = m_UserListDlg.m_UserList.GetUserListDefineType();
    TCHAR szNum[128]; 
    switch(byType)
    {
    case 1:
        break;
    default:
        GlbGetNumString(szNum, pUserInfo->i64Money, Glb().m_nPowerOfGold, Glb().m_bUseSpace, Glb().m_strSpaceChar);
        strBuffer.Format(TEXT("金币：%s"),szNum);
        UserInfoMenu.AppendMenu(0,0,strBuffer);
        strBuffer.Format(TEXT("财富级别：%s"),GetMoneyOrderName(pUserInfo->i64Money));
        UserInfoMenu.AppendMenu(0,0,strBuffer);
        strBuffer.Format(TEXT("总局数：%d"),uAllCount);
        UserInfoMenu.AppendMenu(0,0,strBuffer);
        strBuffer.Format(TEXT("逃跑率：%d"),uCutPercent);
        UserInfoMenu.AppendMenu(0,0,strBuffer);
        break;
    }
	
	/*	if (pUserInfo->szClassName[0]!=0)
	{
	strBuffer.Format(TEXT("社团：%s"),pUserInfo->szClassName);
	UserInfoMenu.AppendMenu(0,0,strBuffer);
	}*/
	if (CUserPower::CanLookIP(m_pGameInfo->dwMasterPower)==true)
	{
		ULONG dwUserIP=pUserInfo->dwUserIP;
		strBuffer.Format(TEXT("用户 IP：%d.%d.%d.%d"),FOURTH_IPADDRESS(dwUserIP),THIRD_IPADDRESS(dwUserIP),
			SECOND_IPADDRESS(dwUserIP),FIRST_IPADDRESS(dwUserIP));
		UserInfoMenu.AppendMenu(0,0,strBuffer);
	}
	if (pUserInfo->bGameMaster!=0)
	{
		if (pUserInfo->bGameMaster==1) UserInfoMenu.AppendMenu(0,0,TEXT("管理职称：内部管理员"));
		else if (pUserInfo->bGameMaster==2) UserInfoMenu.AppendMenu(0,0,TEXT("管理职称：外部管理员"));
		else if (pUserInfo->bGameMaster==3) UserInfoMenu.AppendMenu(0,0,TEXT("管理职称：超级管理员"));
		else if (pUserInfo->bGameMaster==4) UserInfoMenu.AppendMenu(0,0,TEXT("管理职称：超人"));
		else if (pUserInfo->bGameMaster==5) UserInfoMenu.AppendMenu(0,0,TEXT("管理职称：外太空生物"));
		else if (pUserInfo->bGameMaster==6) UserInfoMenu.AppendMenu(0,0,TEXT("管理职称：上帝"));
	}

	//显示菜单
	POINT MousePoint;
	GetCursorPos(&MousePoint);
	UserInfoMenu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON,MousePoint.x,MousePoint.y,this);
	DeleteObject(mi.hbrBack);
	UserInfoMenu.DestroyMenu();
	return;
}

// 打开右键菜单
void CLoveSendClassInExe::OpenUserMenuEx(UserItemStruct * pUserItem)
{
	// 效验数据
	if (pUserItem==NULL) 
	{
		m_dwCurrentUserID = 0L;
		return;
	}

	// 设置数据
	UserInfoStruct * pUserInfo = &pUserItem->GameUserInfo;
	UserInfoStruct * pMeUserInfo = GetMeUserInfo();
	if (!m_pRightFrame)
	{
		m_dwCurrentUserID = pUserItem->GameUserInfo.dwUserID;
	}

	CBcfFile fMsg(CBcfFile::GetAppPath() + "ClientMessage.bcf");

	// 建立菜单
	CGameMenu * pMenu = new CGameMenu;
	pMenu->CreatePopupMenu();
	pMenu->SetSelectedBarColor(RGB(0x66, 0x99, 0xff));
	pMenu->SetBackColor(RGB(227, 243, 251));
	pMenu->SetSideBarStartColor(RGB(40, 119, 183));
	pMenu->SetSideBarEndColor(RGB(40, 119, 183));
	pMenu->SetTextSpace(-10);
	pMenu->AddSideBar(new CGameMenuSideBar(14, ""));

	// PengJiLin, 2010-8-17, 是否禁止私聊
	int iForbitSay = 8 & Glb().m_CenterServerPara.m_nFunction;

	CString str, strBuffer;

	/// 百家乐类型的游戏无踢人功能
	CString stime;
	stime.Format("%d",CTime::GetCurrentTime());        // 当前的时间，String格式
	int curtime = atoi(stime);                         // 转换为int
	TCHAR szKey[10];
	CString sPath = CBcfFile::GetAppPath();
	CBcfFile fsr(sPath + "..\\SpecialRule.bcf");
	wsprintf(szKey, "%d", m_pGameInfo->uNameID);
	int iResult = fsr.GetKeyVal (_T("BJLType"), szKey, 0);

	if (/*iResult == 0 && */pMeUserInfo->dwUserID != m_dwCurrentUserID)
	{
		if(GetMeUserInfo()->iVipTime > curtime) // 会员才有此功能
		{
			pMenu->AppendODMenu(IDM_KICK_USER, TEXT("请他离开"));
			pMenu->AppendSeparator();
		}
		else  // PengJiLin, 2010-9-10, 可配置是否有踢人卡的踢人功能
		{
			CBcfFile bcfFile(sPath + "..\\Order.bcf");
			int iValue = bcfFile.GetKeyVal(_T("ClassInExe"), _T("UseKickUserProp"), 0);
			if(iValue > 0)  // 显示踢人菜单
			{
				pMenu->AppendODMenu(IDM_KICK_USER_PROP, TEXT("请他离开"));
				pMenu->AppendSeparator();
			}
		}
	}

	pMenu->AppendODMenu(IDM_USER_INFO, "详细信息");
	pMenu->AppendODMenu(IDM_COPY_USER_NAME, "复制用户名");

	//功能菜单
	if (!iForbitSay && pMeUserInfo->dwUserID != m_dwCurrentUserID  && !(m_pGameInfo->dwRoomRule & GRR_AUTO_SIT_DESK))
	{
		pMenu->AppendSeparator();

		str = fMsg.GetKeyVal("GameRoom", "AddFriend", TEXT("加为好友"));
		pMenu->AppendODMenu(IDM_SET_FRIEND, str);

		str = fMsg.GetKeyVal("GameRoom", "AddEmeny", TEXT("加入黑名单"));
		pMenu->AppendODMenu(IDM_SET_EMENY, str);
	}

	// 显示菜单
	POINT point;
	::GetCursorPos(&point);
	pMenu->TrackPopupMenu(TPM_LEFTBUTTON, point.x, point.y, this);
	delete pMenu;
	pMenu = NULL;
}

void CLoveSendClassInExe::OnGetBuyItemData(int iPropID)
{
	this->m_pPropSelUser;
	if(!m_pPropPannel)
	{
		m_pPropPannel=new CPropPannel();
		m_pPropPannel->m_pGameInfo = this->m_pGameInfo;
		m_pPropPannel->m_pParent = this;
		m_pPropPannel->m_bUsingNewUI = false;
		AfxSetResourceHandle(GetModuleHandle(FACE_DLL_NAME));
		m_pPropPannel->Create(IDD_PROPPANNEL,this);
		AfxSetResourceHandle(GetModuleHandle(NULL));
		m_pPropPannel->CenterWindow();
	}
	m_pPropPannel->openPannel(0,"", iPropID);
	m_pPropPannel->BringWindowToTop();
	m_pPropPannel->Invalidate();
}

void CLoveSendClassInExe::OnBnClickedGameBtn01()
{
	// TODO: 在此添加控件通知处理程序代码
	OnGameSetting();
	return ;
}
// 购买道具，打开道具面板
void CLoveSendClassInExe::OnBnClickedGameBtn02()
{
	// TODO: 在此添加控件通知处理程序代码
	//屏蔽百家乐相关功能 add by wlr 20090716
	char GameCode[20];
	::memset(GameCode,0,sizeof(GameCode));
	GetGameCode(GameCode);
	//if (str == "11901800"
	//	|| str == "10901800"
	//	|| str == "30501800")
	TCHAR szKey[10];
	CString sPath=CBcfFile::GetAppPath();
	CBcfFile fsr( sPath + "..\\SpecialRule.bcf");
	wsprintf(szKey, "%d", m_pGameInfo->uNameID);
	int iResult = fsr.GetKeyVal (_T("BJLType"), szKey, 0);

	if (iResult>0)
		return ;
	//end of add by wlr 20090716

    // TODO: 在此添加控件通知处理程序代码
    CString s=CBcfFile::GetAppPath () + "..\\";
    CBcfFile f( s + "bzgame.bcf");
    CString url;
    url=f.GetKeyVal("BZW","gamepropurl","http://www.kfgame.com");

    //CString surl;
    //surl.Format("%s/app/userlogin.asp?userid=%d&token=%s&url=%s",
    //	Glb().m_CenterServerPara.m_strWebRootADDR,m_pGameInfo->uisMeUserInfo.GameUserInfo.dwUserID,Glb().m_TML_SN,url);
    //surl.Format("%s/app/userlogin.asp?userid=%d&token=",
    //            url,m_pGameInfo->uisMeUserInfo.GameUserInfo.dwUserID);
    //ShellExecute(NULL,"open",surl,NULL,NULL,SW_MAXIMIZE|SW_SHOW);
    ShellExecute(NULL,"open",url,NULL,NULL,SW_MAXIMIZE|SW_SHOW);
	return ;
}


void CLoveSendClassInExe::OnBnClickedGameBtn03()
{
	//屏蔽百家乐相关功能 add by wlr 20090716
	char GameCode[20];
	::memset(GameCode,0,sizeof(GameCode));
	GetGameCode(GameCode);
	CString str = GameCode;
	//if (str == "11901800"
	//	|| str == "10901800"
	//	|| str == "30501800")
	TCHAR szKey[10];
	CString sPath=CBcfFile::GetAppPath();
	CBcfFile fsr( sPath + "..\\SpecialRule.bcf");
	wsprintf(szKey, "%d", m_pGameInfo->uNameID);
	int iResult = fsr.GetKeyVal (_T("BJLType"), szKey, 0);

	if (iResult>0)
		return ;
	//end of add by wlr 20090716

	// TODO: 在此添加控件通知处理程序代码
	CString s=CBcfFile::GetAppPath () + "..\\";// CINIFile::GetAppPath ();/////本地路径
	CBcfFile f( s + "bzgame.bcf");
	CString url;
	url=f.GetKeyVal("BZW","gameurl1",Glb().m_CenterServerPara.m_strHomeADDR);

	CString surl;
	//surl.Format("%s/app/userlogin.asp?userid=%d&token=%s&url=%s",
	//	Glb().m_CenterServerPara.m_strWebRootADDR,m_pGameInfo->uisMeUserInfo.GameUserInfo.dwUserID,Glb().m_TML_SN,url);
	surl.Format("%s/app/userlogin.asp?userid=%d&token=",
		url,m_pGameInfo->uisMeUserInfo.GameUserInfo.dwUserID);
	DebugPrintf(surl);
	//ShellExecute(NULL,"open",surl,NULL,NULL,SW_MAXIMIZE|SW_SHOW);
	ShellExecute(NULL,"open",url,NULL,NULL,SW_MAXIMIZE|SW_SHOW);
	return ;
}

void CLoveSendClassInExe::OnBnClickedGameBtn04()
{
	//屏蔽百家乐相关功能 add by wlr 20090716
	char GameCode[20];
	::memset(GameCode,0,sizeof(GameCode));
	GetGameCode(GameCode);
	//if (str == "11901800"
	//	|| str == "10901800"
	//	|| str == "30501800")
	TCHAR szKey[10];
	CString sPath=CBcfFile::GetAppPath();
	CBcfFile fsr( sPath + "..\\SpecialRule.bcf");
	wsprintf(szKey, "%d", m_pGameInfo->uNameID);
	int iResult = fsr.GetKeyVal (_T("BJLType"), szKey, 0);

	if (iResult>0)
		return ;
	//end of add by wlr 20090716

	CString s= CBcfFile::GetAppPath () + "..\\";//INIFile::GetAppPath ();/////本地路径
	CBcfFile f( s + "bzgame.bcf");
	CString url;
	url=f.GetKeyVal("BZW","gameurl2",Glb().m_CenterServerPara.m_strHomeADDR);

	CString surl;
	//surl.Format("%s/app/userlogin.asp?userid=%d&token=%s&url=%s",
	//	Glb().m_CenterServerPara.m_strWebRootADDR,m_pGameInfo->uisMeUserInfo.GameUserInfo.dwUserID,Glb().m_TML_SN,url);

	DebugPrintf(surl);
	//ShellExecute(NULL,"open",surl,NULL,NULL,SW_MAXIMIZE|SW_SHOW);
	ShellExecute(NULL,"open",url,NULL,NULL,SW_MAXIMIZE|SW_SHOW);
}
// 使用道具     // PengJiLin, 2010-10-14, 增加一参数
void CLoveSendClassInExe::UsePropItem(int propIndex, BOOL bCheckNum)
{
	//////////////////////////////////////////////////////////////////////////
	//Modify Fred Huang,2008-04-23
	if(GetMeUserInfo()->bUserState==USER_WATCH_GAME)
	{
		AFCMessageBox("对不起，旁观时不能使用道具！","提示");
		return;
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//2008-08-11
	int propCount=(int)Glb().userPropLibrary.GetCount();
	if(propIndex<0 || propIndex>=propCount)
	{
		//没有道具，购买道具
		OnGetBuyItemData(1);
		return;
	}
	_TAG_USERPROP * userProp=Glb().userPropLibrary.GetAt(propIndex);
	if(userProp==NULL) return;

    // PengJiLin, 2010-10-14, 道具的数量可以为0，须处理
    if(userProp->nHoldCount <= 0 && TRUE == bCheckNum)
    {
        TCHAR chInfo[256] = {0};
        wsprintf(chInfo, "抱歉，[%s]已经使用完毕，是否现在购买并使用?", userProp->szPropName);
        if(IDOK == AFCMessageBox(chInfo, TEXT("提示"), AFC_YESNO, this))  // 确定购买
        {
            BuyAndUseProp(propIndex);
            return;
        }
        else        // 取消购买
        {
            return;
        }
    }

	if(userProp->attribAction & 0x80 || userProp->attribAction &0x100)
	{//大喇叭和小喇叭
		this->UseBoardCast( userProp );
		return;
	}

    // PengJiLin, 2010-6-1, 记牌器 功能
    if(userProp->attribAction & 0x200)
    {
        this->UseLookCardProp(userProp);
        return;
    }

    // PengJiLin, 2010-9-9, 踢人卡功能
    if(userProp->attribAction & 0x400)
    {
        this->UseKickUserProp(userProp);
        return;
    }

    // PengJiLin, 2010-9-9, 防踢卡功能
    if(userProp->attribAction & 0x800)
    {
        this->UseAntiKickProp(userProp);
        return;
    }

	//区分对自己使用，还是对其它人使用
	if(userProp->attribAction & 0x01)
	{//对其它人使用
		if(m_pPropSelUser==NULL)
		{
			m_pPropSelUser=new CPropSelUser;
			AfxSetResourceHandle(GetModuleHandle(FACE_DLL_NAME));
			m_pPropSelUser->Create(IDD_PROP_SEL_USER,this);
			AfxSetResourceHandle(GetModuleHandle(NULL));
		}
		for(int i=0;i<MAX_PEOPLE;i++)
			m_pPropSelUser->m_pUserItem[i]=m_pUserInfo[i];
		m_pPropSelUser->dwUsedPropID=propIndex;
		m_pPropSelUser->dwLocalUserID=GetMeUserInfo()->dwUserID;
		m_pPropSelUser->propItemName=userProp->szPropName;
		m_pPropSelUser->resetInformation();
		m_pPropSelUser->CenterWindow();
		m_pPropSelUser->ShowWindow(SW_SHOW);		
	}
	else
	{//对自己使用
		/*
		CString stip;
		stip.Format("您确定要使用道具 %s 吗?",userProp->szPropName);
		if(userProp->attribAction & 0x2)
		stip+="\n会将您的负积分置零!";

		*/
		////////////////////////////////////////////////////
		///2008 1216 Kylin 正分时使用负分清零没有提示
		CString stip;
		stip.Format("您确定要使用道具 %s 吗?",userProp->szPropName);
		if(userProp->attribAction & 0x2)
		{
			if(GetMeUserInfo()->dwPoint>=0)
			{
				stip.Format("您的积分为正，使用道具 %s ，会消耗您一个道具而没有效果，确定使用吗?",userProp->szPropName);
			}
			else stip+="\n会将您的负积分置零!";
		}

		UINT uRet = AFCMessageBox(stip,"提示",AFC_YESNO,this);
		if(uRet==IDOK)
		{
			OnUseProp(propIndex,GetMeUserInfo()->dwUserID);
		}
		////////////////////////////////////////////////////
	}
	return;
}
void CLoveSendClassInExe::UseBoardCast(_TAG_USERPROP * userProp)
{
	//先建立对象
	AfxSetResourceHandle(GetModuleHandle(FACE_DLL_NAME));
	if(!m_dlgBroadcast)
	{
		m_dlgBroadcast=new CBoardCast();;
		m_dlgBroadcast->Create(IDD_BOARDCAST,GetDesktopWindow());
		m_dlgBroadcast->pParentWnd=(CWnd*)this;
	}

	m_dlgBroadcast->m_nPropID=userProp->nPropID;

	if(userProp->attribAction & 0x80)
		m_dlgBroadcast->SetValue(BRD_MSG_BIG);
	else
		m_dlgBroadcast->SetValue(BRD_MSG_SMALL);
	AfxSetResourceHandle(GetModuleHandle(NULL));
	m_dlgBroadcast->ShowWindow(SW_SHOW);
}
void CLoveSendClassInExe::OnBnClickedButtonProp1()
{
	// duanxiaohui 20111103 体验场(免费场) 不能使用道具
	if (m_pGameInfo->dwRoomRule & GRR_EXPERCISE_ROOM)
	{
		AFCMessageBox("对不起, 体验场(免费场)不能使用游戏道具", "温馨提示");
		return ;
	}
	// end duanxiaohui

	//屏蔽百家乐相关功能 add by wlr 20090716
	char GameCode[20];
	::memset(GameCode,0,sizeof(GameCode));
	GetGameCode(GameCode);
	//if (str == "11901800"
	//	|| str == "10901800"
	//	|| str == "30501800")
	TCHAR szKey[10];
	CString sPath=CBcfFile::GetAppPath();
	CBcfFile fsr( sPath + "..\\SpecialRule.bcf");
	wsprintf(szKey, "%d", m_pGameInfo->uNameID);
	int iResult = fsr.GetKeyVal (_T("BJLType"), szKey, 0);

	if (iResult>0)
	{
		AFCMessageBox("对不起,无法在百人类游戏使用道具","提示");
		return ;
	}
	//end of add by wlr 20090716

	//UsePropItem(m_curPropPage*PEER_PAGE+0);
	OnGetBuyItemData(9999);
	return;
}

void CLoveSendClassInExe::OnBnClickedButtonProp2()
{
	// duanxiaohui 20111103 体验场(免费场) 不能使用道具
	if (m_pGameInfo->dwRoomRule & GRR_EXPERCISE_ROOM)
	{
		AFCMessageBox("对不起, 体验场(免费场)不能使用游戏道具", "温馨提示");
		return ;
	}
	// end duanxiaohui

	//屏蔽百家乐相关功能 add by wlr 20090716
	char GameCode[20];
	::memset(GameCode,0,sizeof(GameCode));
	GetGameCode(GameCode);
	//if (str == "11901800"
	//	|| str == "10901800"
	//	|| str == "30501800")
	TCHAR szKey[10];
	CString sPath=CBcfFile::GetAppPath();
	CBcfFile fsr( sPath + "..\\SpecialRule.bcf");
	wsprintf(szKey, "%d", m_pGameInfo->uNameID);
	int iResult = fsr.GetKeyVal (_T("BJLType"), szKey, 0);

	if (iResult>0)
	{
		AFCMessageBox("对不起,无法在百人类游戏使用道具","提示");
		return ;
	}
	//end of add by wlr 20090716

	//UsePropItem(m_curPropPage*PEER_PAGE+1);
	OnGetBuyItemData(1);
	return;
}

void CLoveSendClassInExe::OnBnClickedButtonProp3()
{
	// duanxiaohui 20111103 体验场(免费场) 不能使用道具
	if (m_pGameInfo->dwRoomRule & GRR_EXPERCISE_ROOM)
	{
		AFCMessageBox("对不起, 体验场(免费场)不能使用游戏道具", "温馨提示");
		return ;
	}
	// end duanxiaohui


	//屏蔽百家乐相关功能 add by wlr 20090716
	char GameCode[20];
	::memset(GameCode,0,sizeof(GameCode));
	GetGameCode(GameCode);
	//if (str == "11901800"
	//	|| str == "10901800"
	//	|| str == "30501800")
	TCHAR szKey[10];
	CString sPath=CBcfFile::GetAppPath();
	CBcfFile fsr( sPath + "..\\SpecialRule.bcf");
	wsprintf(szKey, "%d", m_pGameInfo->uNameID);
	int iResult = fsr.GetKeyVal (_T("BJLType"), szKey, 0);

	if (iResult>0)
	{
		AFCMessageBox("对不起,无法在百人类游戏使用道具","提示");
		return ;
	}
	//end of add by wlr 20090716

	//UsePropItem(m_curPropPage*PEER_PAGE+2);
	OnGetBuyItemData(2);
	return;
}

// PengJiLin, 2010-10-12, 商场道具功能
void CLoveSendClassInExe::OnBnClickedButtonProp4()
{
	// duanxiaohui 20111103 体验场(免费场) 不能使用道具
	if (m_pGameInfo->dwRoomRule & GRR_EXPERCISE_ROOM)
	{
		AFCMessageBox("对不起, 体验场(免费场)不能使用游戏道具", "温馨提示");
		return ;
	}
	// end duanxiaohui


    //屏蔽百家乐相关功能 add by wlr 20090716
    char GameCode[20];
    ::memset(GameCode,0,sizeof(GameCode));
    GetGameCode(GameCode);
    //if (str == "11901800"
    //	|| str == "10901800"
    //	|| str == "30501800")
    TCHAR szKey[10];
    CString sPath=CBcfFile::GetAppPath();
    CBcfFile fsr( sPath + "..\\SpecialRule.bcf");
    wsprintf(szKey, "%d", m_pGameInfo->uNameID);
    int iResult = fsr.GetKeyVal (_T("BJLType"), szKey, 0);

	if (iResult>0)
	{
		AFCMessageBox("对不起,无法在百人类游戏使用道具","提示");
		return ;
	}
    //end of add by wlr 20090716

    //BuyAndUseProp(m_curPropPage_S*PEER_PAGE+0);
	OnGetBuyItemData(3);
}
void CLoveSendClassInExe::OnBnClickedButtonProp5()
{
	// duanxiaohui 20111103 体验场(免费场) 不能使用道具
	if (m_pGameInfo->dwRoomRule & GRR_EXPERCISE_ROOM)
	{
		AFCMessageBox("对不起, 体验场(免费场)不能使用游戏道具", "温馨提示");
		return ;
	}
	// end duanxiaohui


    //屏蔽百家乐相关功能 add by wlr 20090716
    char GameCode[20];
    ::memset(GameCode,0,sizeof(GameCode));
    GetGameCode(GameCode);
    //if (str == "11901800"
    //	|| str == "10901800"
    //	|| str == "30501800")
    TCHAR szKey[10];
    CString sPath=CBcfFile::GetAppPath();
    CBcfFile fsr( sPath + "..\\SpecialRule.bcf");
    wsprintf(szKey, "%d", m_pGameInfo->uNameID);
    int iResult = fsr.GetKeyVal (_T("BJLType"), szKey, 0);

	if (iResult>0)
	{
		AFCMessageBox("对不起,无法在百人类游戏使用道具","提示");
		return ;
	}
    //end of add by wlr 20090716

    //BuyAndUseProp(m_curPropPage_S*PEER_PAGE+1);
	OnGetBuyItemData(4);
}
void CLoveSendClassInExe::OnBnClickedButtonProp6()
{
	// duanxiaohui 20111103 体验场(免费场) 不能使用道具
	if (m_pGameInfo->dwRoomRule & GRR_EXPERCISE_ROOM)
	{
		AFCMessageBox("对不起, 体验场(免费场)不能使用游戏道具", "温馨提示");
		return ;
	}
	// end duanxiaohui


    //屏蔽百家乐相关功能 add by wlr 20090716
    char GameCode[20];
    ::memset(GameCode,0,sizeof(GameCode));
    GetGameCode(GameCode);
    //if (str == "11901800"
    //	|| str == "10901800"
    //	|| str == "30501800")
    TCHAR szKey[10];
    CString sPath=CBcfFile::GetAppPath();
    CBcfFile fsr( sPath + "..\\SpecialRule.bcf");
    wsprintf(szKey, "%d", m_pGameInfo->uNameID);
    int iResult = fsr.GetKeyVal (_T("BJLType"), szKey, 0);

	if (iResult>0)
	{
		AFCMessageBox("对不起,无法在百人类游戏使用道具","提示");
		return ;
	}
    //end of add by wlr 20090716

    //BuyAndUseProp(m_curPropPage_S*PEER_PAGE+2);
	OnGetBuyItemData(5);
}

void CLoveSendClassInExe::OnBnClickedButtonProp7()
{
	// duanxiaohui 20111103 体验场(免费场) 不能使用道具
	if (m_pGameInfo->dwRoomRule & GRR_EXPERCISE_ROOM)
	{
		AFCMessageBox("对不起, 体验场(免费场)不能使用游戏道具", "温馨提示");
		return ;
	}
	// end duanxiaohui


	//屏蔽百家乐相关功能 add by wlr 20090716
	char GameCode[20];
	::memset(GameCode,0,sizeof(GameCode));
	GetGameCode(GameCode);

	TCHAR szKey[10];
	CString sPath=CBcfFile::GetAppPath();
	CBcfFile fsr( sPath + "..\\SpecialRule.bcf");
	wsprintf(szKey, "%d", m_pGameInfo->uNameID);
	int iResult = fsr.GetKeyVal (_T("BJLType"), szKey, 0);

	if (iResult>0)
	{
		AFCMessageBox("对不起，无法在百人类游戏使用道具","提示");
		return ;
	}
	//end of add by wlr 20090716

	OnGetBuyItemData(6);
	return;
}

void CLoveSendClassInExe::OnBnClickedButtonProp8()
{
	// duanxiaohui 20111103 体验场(免费场) 不能使用道具
	if (m_pGameInfo->dwRoomRule & GRR_EXPERCISE_ROOM)
	{
		AFCMessageBox("对不起, 体验场(免费场)不能使用游戏道具", "温馨提示");
		return ;
	}
	// end duanxiaohui


	//屏蔽百家乐相关功能 add by wlr 20090716
	char GameCode[20];
	::memset(GameCode,0,sizeof(GameCode));
	GetGameCode(GameCode);

	TCHAR szKey[10];
	CString sPath=CBcfFile::GetAppPath();
	CBcfFile fsr( sPath + "..\\SpecialRule.bcf");
	wsprintf(szKey, "%d", m_pGameInfo->uNameID);
	int iResult = fsr.GetKeyVal (_T("BJLType"), szKey, 0);

	if (iResult>0)
	{
		AFCMessageBox("对不起,无法在百人类游戏使用道具","提示");
		return ;
	}
	//end of add by wlr 20090716

	OnGetBuyItemData(7);
	return;
}

void CLoveSendClassInExe::set_videos_pos()
{
	if (0 >= m_iVideoOffsetNum || m_iVideoOffsetNum > VIDEO_MAX_PEOPLE || m_iVideoOffsetNum > m_pGameInfo->uDeskPeople)
	{
		//只支持最多显示16个人的游戏视频
		return;
	}
	if (!m_pVideoInterface)
	{
		return;
	}
	
	for(int i = 0; i < m_pGameInfo->uDeskPeople && m_pVideoInterface; ++i)
	{
	//	int nViewSite = ViewStation(i);
		if (i >= VIDEO_MAX_PEOPLE)
			continue;
		int iXOffset = m_pVideoXOffset[i];
		int iYOffset = m_pVideoYOffset[i];
		if (m_pGameView->m_PtName[i].x != 0 && m_pGameView->m_PtName[i].y != 0)
			m_pVideoInterface->set_video_pos(i, m_pGameView->m_PtName[i].x+iXOffset, m_pGameView->m_PtName[i].y+iYOffset);
		else
			m_pVideoInterface->set_video_pos(i, m_pGameView->m_PtLogo[i].x+iXOffset, m_pGameView->m_PtLogo[i].y+iYOffset);
	}
}

void CLoveSendClassInExe::set_videos_userid()
{
	UserInfoForGame UserInfo;
	for(int i = 0; i < m_pGameInfo->uDeskPeople && m_pVideoInterface; ++i)
	{
		if(!GetUserInfo(i,UserInfo))
		{
			continue;
		}
		if (i==GetMyDeskStation())
		{
			m_pVideoInterface->set_self_user_id( m_pGameInfo->uRoomID * 1000 + UserInfo.bDeskNO, UserInfo.dwUserID); 
		}
		else
		{
			m_pVideoInterface->set_user_id( m_pGameInfo->uRoomID * 1000 + UserInfo.bDeskNO, UserInfo.dwUserID, ViewStation(i)); 
		}
	}
	set_videos_pos();
}

///获取用户信息
bool CLoveSendClassInExe::GetUserInfo(int nDeskStation, UserInfoForGame &Info)
{
	INT_PTR uActiveCount=m_ThisDeskPtrArray.GetCount();
	UserItemStruct * pFindUserItem = NULL;
	UserItemStruct * pTmp = NULL;
	if (nDeskStation == m_pGameInfo->uisMeUserInfo.GameUserInfo.bDeskStation)
	{
		pFindUserItem =  &m_pGameInfo->uisMeUserInfo;
	}

	for (int i=0;i<uActiveCount;i++)
	{
		UserItemStruct * pTmp=(UserItemStruct *)m_ThisDeskPtrArray.GetAt(i);
		if ((pTmp!=NULL)&&(pTmp->GameUserInfo.bDeskStation==nDeskStation)) 
		{
			pFindUserItem = pTmp;
			break; 
		}
	}
	if (pFindUserItem!=NULL)
	{
		Info.i64Money = pFindUserItem->GameUserInfo.i64Money;
		_tcscpy(Info.szNickName, pFindUserItem->GameUserInfo.nickName);

		Info.bBoy = pFindUserItem->GameUserInfo.bBoy;
		Info.bDeskNO = pFindUserItem->GameUserInfo.bDeskNO;
		Info.bDeskStation = pFindUserItem->GameUserInfo.bDeskStation;            
		Info.bGameMaster = pFindUserItem->GameUserInfo.bGameMaster;
		Info.bLogoID = pFindUserItem->GameUserInfo.bLogoID;
		Info.dwUserID = pFindUserItem->GameUserInfo.dwUserID;
		Info.dwUserIP = pFindUserItem->GameUserInfo.dwUserIP;
		Info.bIsRobot = pFindUserItem->GameUserInfo.isVirtual;
		_tcscpy(Info.szCity, pFindUserItem->GameUserInfo.szCity);     

		if (m_pGameInfo->dwRoomRule & GRR_CONTEST)
		{
			Info.i64Money = pFindUserItem->GameUserInfo.i64ContestScore;
		}
		return true;
	}
	return false;
}

int CLoveSendClassInExe::GetMyDeskStation()
{
	return GetMeUserInfo()->bDeskStation;
}

void CLoveSendClassInExe::OnBnClickedButtonProp9()
{
	// duanxiaohui 20111103 体验场(免费场) 不能使用道具
	if (m_pGameInfo->dwRoomRule & GRR_EXPERCISE_ROOM)
	{
		AFCMessageBox("对不起, 体验场(免费场)不能使用游戏道具", "温馨提示");
		return ;
	}
	// end duanxiaohui


	//屏蔽百家乐相关功能 add by wlr 20090716
	char GameCode[20];
	::memset(GameCode,0,sizeof(GameCode));
	GetGameCode(GameCode);

	TCHAR szKey[10];
	CString sPath=CBcfFile::GetAppPath();
	CBcfFile fsr( sPath + "..\\SpecialRule.bcf");
	wsprintf(szKey, "%d", m_pGameInfo->uNameID);
	int iResult = fsr.GetKeyVal (_T("BJLType"), szKey, 0);

	if (iResult>0)
	{
		AFCMessageBox("对不起,无法在百人类游戏使用道具","提示");
		return ;
	}
	//end of add by wlr 20090716

	OnGetBuyItemData(8);
	return;
}

void CLoveSendClassInExe::OnBnClickedButtonProp10()
{
	// duanxiaohui 20111103 体验场(免费场) 不能使用道具
	if (m_pGameInfo->dwRoomRule & GRR_EXPERCISE_ROOM)
	{
		AFCMessageBox("对不起, 体验场(免费场)不能使用游戏道具", "温馨提示");
		return ;
	}
	// end duanxiaohui


	//屏蔽百家乐相关功能 add by wlr 20090716
	char GameCode[20];
	::memset(GameCode,0,sizeof(GameCode));
	GetGameCode(GameCode);

	TCHAR szKey[10];
	CString sPath=CBcfFile::GetAppPath();
	CBcfFile fsr( sPath + "..\\SpecialRule.bcf");
	wsprintf(szKey, "%d", m_pGameInfo->uNameID);
	int iResult = fsr.GetKeyVal (_T("BJLType"), szKey, 0);

	if (iResult>0)
	{
		AFCMessageBox("对不起,无法在百人类游戏使用道具","提示");
		return ;
	}
	//end of add by wlr 20090716

	OnGetBuyItemData(9);
	return;
}
//MFC获取是否为旁光者模式
bool CLoveSendClassInExe::isWatchingMode()
{
	return ((GetMeUserInfo() != NULL) && (GetMeUserInfo()->bUserState == USER_WATCH_GAME));
}

void CLoveSendClassInExe::BuyAndUseProp(int propIndex)
{
	// duanxiaohui 20111103 体验场(免费场) 不能使用道具
	if (m_pGameInfo->dwRoomRule & GRR_EXPERCISE_ROOM)
	{
		AFCMessageBox("对不起, 体验场(免费场)不能使用游戏道具", "温馨提示");
		return ;
	}
	// end duanxiaohui


    if(GetMeUserInfo()->bUserState==USER_WATCH_GAME)
    {
        AFCMessageBox("对不起，旁观时不能使用道具！","提示");
        return;
    }

    _TAG_USERPROP * userProp=Glb().userPropLibrary.GetAt(propIndex);
    if(userProp==NULL) return;

    if(userProp->nHoldCount > 0)    // 道具数量为0才使用即买即用功能
    {
        TCHAR chInfo[256] = {0};
        wsprintf(chInfo, "[%s]的数量不为0，不能使用商店的即买即用功能", userProp->szPropName);
        m_MessageHandle.InsertSystemMessage(chInfo);  
        return;
    }

    _TAG_PROP_BUY *pPropBuy = new _TAG_PROP_BUY;
    pPropBuy->dwUserID = GetMeUserInfo()->dwUserID;
    pPropBuy->nPropID = userProp->nPropID;
    pPropBuy->iPropPayMoney = 0;
    pPropBuy->nPropBuyCount = 1;

    this->m_pGameRoom->PostMessage(WM_PROP_BUY_NEW, sizeof(_TAG_PROP_BUY), (LPARAM)pPropBuy);
}


//信息函数（不需要重载）
bool CLoveSendClassInExe::OnSystemMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize)
{
	ASSERT(pNetHead->bMainID==MDM_GM_MESSAGE);
	switch (pNetHead->bAssistantID)
	{
	case ASS_GM_SYSTEM_MESSAGE:		//系统消息
		{
			//效验数据
			MSG_GA_S_Message * pMessage=(MSG_GA_S_Message *)pNetData;
			AFCPlaySound(AfxGetInstanceHandle(),TEXT("SYSTEM_MSG"));

			//处理数据
			if ((pMessage->bShowStation&SHS_TALK_MESSAGE)!=0)
			{
				m_MessageHandle.InsertSystemMessage(pMessage->szMessage,pMessage->bFontSize);
			}
			if ((pMessage->bShowStation&SHS_MESSAGE_BOX)!=0)
			{
				m_bSaveView=true;
				SetStationParameter(20);
				AFCMessageBox("好象是这里吧",m_pGameInfo->szGameName);//,MB_ICONQUESTION,this);
				//AFCMessageBox(pMessage->szMessage,m_pGameInfo->szGameName);//,MB_ICONQUESTION,this);
				m_bSaveView=false;
				if (pMessage->bCloseFace==TRUE)
					AFCCloseFrame(false);
			}
			::SendMessage(m_TalkMessage.m_hWnd,WM_VSCROLL,SB_BOTTOM,0);

			//OnBnClickedEnd();
			return true;
		}
	}
	return true;
}

//框架消息处理函数（不需要重载）
bool CLoveSendClassInExe::OnFrameMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize)
{
	ASSERT(pNetHead->bMainID==MDM_GM_GAME_FRAME);
	switch (pNetHead->bAssistantID)
	{
	case ASS_GM_GAME_INFO:		//游戏信息
		{
			//效验数据
			MSG_GM_S_GameInfo * pGameInfo=(MSG_GM_S_GameInfo *)pNetData;

			//处理数据
			m_bWatchOther=(pGameInfo->bWatchOther==TRUE);
			m_bWaitTime=pGameInfo->bWaitTime;
			m_bWatchMode=(m_pGameInfo->uisMeUserInfo.GameUserInfo.bUserState==USER_WATCH_GAME);
			SetStationParameter(pGameInfo->bGameStation);
			//显示信息
			if (uDataSize>(sizeof(MSG_GM_S_GameInfo)-sizeof(pGameInfo->szMessage)))
			{
				m_MessageHandle.InsertSystemMessage(pGameInfo->szMessage);
			}
			::SendMessage(m_TalkMessage.m_hWnd,WM_VSCROLL,SB_BOTTOM,0);

			return true;
		}
	case ASS_GM_GAME_STATION:	//游戏状态
		{	////	AfxMessageBox("111111111");
			return SetGameStation(pNetData,uDataSize);//调用DLL中的函数，，麻将中uDataSize为51
		}
	case ASS_GM_SET_VIDEOADDR:
		{
			if (m_pVideoInterface != NULL)
				return true;

			SetVideoSrvStruct *pmsg = (SetVideoSrvStruct*)pNetData;

			m_pVideoInterface = create_video_interface(m_pGameInfo->uDeskPeople,(long long)this->GetSafeHwnd(), pmsg->szIP, pmsg->videoport, pmsg->audioport);			
			set_videos_userid();			
			set_videos_pos();
			return true;
		}
	case ASS_GM_NORMAL_TALK:	//普通聊天
		{
			//效验数据
			MSG_GR_RS_NormalTalk * pNormalTalk=(MSG_GR_RS_NormalTalk *)pNetData;

			//处理数据
			UserItemStruct * pSendUserItem=FindOnLineUser(pNormalTalk->dwSendID);
			if (pSendUserItem==NULL) 
				return true;

			//过滤不受欢迎用户
			if (pSendUserItem->uConnection==CM_ENEMY) 
				return true;

			//过滤屏蔽用户
			for (INT_PTR i=0;i<m_ShieldArray.GetCount();i++)
			{
				if (pSendUserItem->GameUserInfo.dwUserID==m_ShieldArray.GetAt(i)) 
					return true;
			}
			TCHAR msg[502];//保存消息
			lstrcpy(msg,pNormalTalk->szMessage);
			//MessageBox(msg);
			//处理消息
			if (pNormalTalk->dwTargetID!=0L)
			{
				UserItemStruct * pTargerUserItem=FindOnLineUser(pNormalTalk->dwTargetID);
				if (pTargerUserItem!=NULL) 
				{
					m_MessageHandle.InsertUserTalk(GetComType()!=TY_MATCH_GAME?pSendUserItem->GameUserInfo.nickName:"玩家",GetComType()!=TY_MATCH_GAME?pTargerUserItem->GameUserInfo.nickName:"玩家",
						pNormalTalk->szMessage,pNormalTalk->crColor,false);
					lstrcpy(pNormalTalk->szMessage,msg);
					ShowUserTalk(pNormalTalk);
					return true;
				}
			}
			m_MessageHandle.InsertUserTalk(GetComType()!=TY_MATCH_GAME?pSendUserItem->GameUserInfo.nickName:"玩家",NULL,pNormalTalk->szMessage,pNormalTalk->crColor,false);
			lstrcpy(pNormalTalk->szMessage,msg);
			if(GetComType()!=TY_MATCH_GAME)
				ShowUserTalk(pNormalTalk);
			if(pNormalTalk->nDefaultIndex>=0)
			{
// 				CString soundFile;
// 				soundFile.Format("..\\Music\\Talk\\%d.wav",pNormalTalk->nDefaultIndex);
// 				BZSoundPlay(this,soundFile.GetBuffer(),0,1);

				CBcfFile fMsg(CBcfFile::GetAppPath() + "TalkInGame.bcf");
				CString str = fMsg.GetKeyVal("BZW_TALK","path","..\\Music\\Talk");

				//播放聊天语音
				TCHAR szSpeakPath[MAX_PATH];
				if (pSendUserItem->GameUserInfo.bBoy)
				{
					_stprintf(szSpeakPath,"%s\\male\\%s.wav",str,pNormalTalk->szMessage);	
				}
				else
				{
					_stprintf(szSpeakPath,"%s\\female\\%s.wav",str,pNormalTalk->szMessage);
				}
				BZSoundPlay(this,szSpeakPath,0);
			}
			return true;
		}
	case ASS_GM_WATCH_SET:		//旁观设置
		{

			ASSERT(m_bWatchMode==true);
			bool bNotifyClient=(m_bWatchOther!=(pNetHead->bHandleCode==TRUE));
			if (bNotifyClient==true)
			{
				m_bWatchOther=(pNetHead->bHandleCode==TRUE);
				if (m_bWatchOther == true)
				{
					m_MessageHandle.InsertSystemMessage(TEXT("你被允许旁观"));
				}
				else 
				{
					m_MessageHandle.InsertSystemMessage(TEXT("你被禁止旁观"));
				}
				OnWatchSetChange();
			}
			return true;
		}
    case ASS_GM_USE_KICK_PROP:      // PengJiLin, 2010-9-10, 使用踢人卡
        {
            return OnUseKickPropResult(pNetHead, pNetData, uDataSize, TRUE);
        }
    case ASS_GM_USE_ANTI_KICK_PROP: // PengJiLin, 2010-9-10, 使用防踢卡
        {
            return OnUseKickPropResult(pNetHead, pNetData, uDataSize, FALSE);;
        }
	}
	return false;
}
//控制消息 
UINT CLoveSendClassInExe::OnControlMessage(UINT uControlCode, void * pControlData, UINT uDataSize)
{
	//判断窗口是否有效
	if (GetSafeHwnd()==NULL)
	{
		DebugPrintf("窗口还没启动成功，返回");
		return 0;
	}

	//DebugPrintf("uControlCode = [%d]",uControlCode);
	//处理数据
	switch (uControlCode)
	{
	case IPC_USER_PROP_RESULT:
		{
			NetMessageHead * pNetHead = new NetMessageHead;
			pNetHead->bAssistantID = ASS_PROP_RESULT;
			m_pPropPannel->OnNetMessage(pNetHead,(void *)pControlData,uDataSize);
		}
	case CM_USER_SEND_TIMES_MONEY:
		{
			if (uDataSize!=sizeof(CM_UserState_Send_Times_Money_For_Exe)) return 1;
			CM_UserState_Send_Times_Money_For_Exe *pMoneyChange = (CM_UserState_Send_Times_Money_For_Exe*)pControlData;

			if (pMoneyChange->dwUserID != m_pGameInfo->uisMeUserInfo.GameUserInfo.dwUserID)
				return 0;

            // PengJiLin, 2011-4-18, 在线时长、局数赠送金币功能
            m_SendMoneyData.dwUserID = pMoneyChange->dwUserID;
            m_SendMoneyData.dwGetMoney = pMoneyChange->dwGetMoney;
            m_SendMoneyData.dwMoneyOnTimes = pMoneyChange->dwMoneyOnTimes;
            m_SendMoneyData.dwMoneyOnCounts = pMoneyChange->dwMoneyOnCounts;
            m_SendMoneyData.dwTimesNeed = pMoneyChange->dwTimesNeed;
            m_SendMoneyData.dwCountsNeed = pMoneyChange->dwCountsNeed;

            if(pMoneyChange->dwGetMoney > 0)
            {
                CString str;
                TCHAR szNum[128]; 
                GlbGetNumString(szNum, pMoneyChange->dwGetMoney, Glb().m_nPowerOfGold, Glb().m_bUseSpace, Glb().m_strSpaceChar);

				CBcfFile fMsg(CBcfFile::GetAppPath()+"..\\ClientMessage.bcf");
				str = fMsg.GetKeyVal("GameFrame","SendMoney_Time","/:54获得系统赠送%s金币/:54");
				str.Format(str,szNum);

                m_MessageHandle.InsertNormalMessage(str.GetBuffer());
            }

			CString str;
			str.Format("zhtlog:pMoneyChange->dwGetMoney=%d pMoneyChange->dwTimesNeed=%d pMoneyChange->dwCountsNeed =%d",pMoneyChange->dwGetMoney,pMoneyChange->dwTimesNeed,pMoneyChange->dwCountsNeed );
			OutputDebugString(str);
           /* if(pMoneyChange->dwGetMoney > 0 || pMoneyChange->dwTimesNeed > 0 || pMoneyChange->dwCountsNeed > 0)
            {
               
            }*/

			return 0;

		}
		break;
	case CM_USER_STATE:		//用户状态改变
		{

			//效验数据
			if (uDataSize!=sizeof(CM_UserState_Change_ForExe))
			{
				DebugPrintf("数据大小不对uDataSize =[%d], 应当为[%d]",sizeof(CM_UserState_Change_ForExe));
				return 1;
			}
			
			CM_UserState_Change_ForExe* pStateChange=(CM_UserState_Change_ForExe *)pControlData;

			if (pStateChange->bActionCode == ACT_USER_BANKMONEY)
			{
				INT_PTR nUserCountInThisDesk = m_ThisDeskPtrArray.GetCount();
				for (INT_PTR i=0; i<nUserCountInThisDesk; ++i)
				{
					__int64 i64BankMoney = pStateChange->uisUserItem.GameUserInfo.i64Bank;
					UserItemStruct *pFindItem = (UserItemStruct *)m_ThisDeskPtrArray.GetAt(i);
					if (pFindItem->GameUserInfo.dwUserID == pStateChange->uisUserItem.GameUserInfo.dwUserID)
					{
						pFindItem->GameUserInfo.i64Bank = i64BankMoney;
						if (m_pBank2Wnd)
						{
							m_pBank2Wnd->SetWealthInfor(i64BankMoney,GetMeUserInfo()->i64Money);
						}
						break;
					}
				}
				return 1;
			}

			INT_PTR nUserCountInThisDesk = m_ThisDeskPtrArray.GetCount();
			for (INT_PTR i=0; i<nUserCountInThisDesk; ++i)
			{
				UserItemStruct *pFindItem = (UserItemStruct *)m_ThisDeskPtrArray.GetAt(i);
				if (pFindItem->GameUserInfo.dwUserID == pStateChange->uisUserItem.GameUserInfo.dwUserID)
				{
					memcpy(pFindItem, &pStateChange->uisUserItem, sizeof(UserItemStruct));
				}
			}

			//处理数据CS
			switch (pStateChange->bActionCode)
			{
			case ACT_USER_UP:		//用户起来
			case ACT_WATCH_UP:		//旁观起来
				{
					//处理数据
					if (pStateChange->uisUserItem.GameUserInfo.dwUserID
						!= m_pGameInfo->uisMeUserInfo.GameUserInfo.dwUserID)//同桌的其他人离开
					{
						GameUserLeft(pStateChange->bDeskStation, &pStateChange->uisUserItem, pStateChange->bActionCode==ACT_WATCH_UP);

						if (pStateChange->bActionCode != ACT_WATCH_UP)
						{
							//用户起身时，重置该位置的视频用户ID
							if (m_pVideoInterface)
							{
								m_pVideoInterface->set_user_id(-1, -1, ViewStation(pStateChange->bDeskStation));
							}
						}

						if ((GetComType()!=TY_MATCH_GAME) &&m_pGameInfo->bShowUserInfo==true)
						{	
							m_MessageHandle.InsertUserName(pStateChange->uisUserItem.GameUserInfo.nickName);

							char szTalk[255];
							sprintf(szTalk, "%s 离开了", pStateChange->uisUserItem.GameUserInfo.nickName);
							m_MessageHandle.InsertNormalMessage(szTalk);

							if (pStateChange->bActionCode == ACT_WATCH_UP)
							{
								;
							}
							else 
							{
								//提示不存在，显示提示（在，什么都不做）
								if(!m_DlgShowLeft.Showing && GetSafeHwnd()!=NULL
									&&(m_pGameInfo->dwRoomRule & GRR_ALL_ON_DESK)== 0L)//需要全部人离开
								{
									char GameCode[20];
									::memset(GameCode,0,sizeof(GameCode));
									GetGameCode(GameCode);
									//CString str = GameCode;
									//if (str != "10301800" && 
									//	str != "30501800" &&
									//	str != "10901800" &&
									//	str != "11901800" )
									TCHAR szKey[10];
									CString sPath=CBcfFile::GetAppPath();
									CBcfFile fsr( sPath + "..\\SpecialRule.bcf");
									wsprintf(szKey, "%d", m_pGameInfo->uNameID);
									int iResult = fsr.GetKeyVal (_T("BJLType"), szKey, 0);

									if (iResult==0)

									{
										m_DlgShowLeft.GetNickName(pStateChange->uisUserItem.GameUserInfo.nickName);
										AfxSetResourceHandle(GetModuleHandle(FACE_DLL_NAME));
										m_DlgShowLeft.Create(IDD_SHOW_LEFT,this);
										AfxSetResourceHandle(GetModuleHandle(NULL));
										RECT rc;
										GetClientRect(&rc);
										ScreenToClient(&rc);
										m_DlgShowLeft.ShowWindow(1);
										m_DlgShowLeft.Showing=1;
										m_DlgShowLeft.BeginTimer();
										EnableWindow(FALSE);
									}
								}
							}
						}
					}
					else//自己离开
					{
						//if (m_uTimeID!=0L) 
						//	KillGameTimer(m_uTimeID);
						//if (m_bSaveView==false)
						//{
						//	this->ResetGameStation();
						//	m_pGameView->ResetGameView();
						//	ResetGameFrame();
						//}
						////主动离开
						//if(!m_DlgShowLeft.Showing)
						//	AFCCloseFrame();
						DebugPrintf("自己离开了");
					}

					::SendMessage(m_TalkMessage.m_hWnd,WM_VSCROLL,SB_BOTTOM,0);

					return 0;
				}
			case ACT_USER_SIT:		//用户坐下
			case ACT_WATCH_SIT:		//旁观坐下
				{
					//处理数据
					// 若是别人坐下，则直接通知
					if (pStateChange->uisUserItem.GameUserInfo.dwUserID
						!= m_pGameInfo->uisMeUserInfo.GameUserInfo.dwUserID)//同桌的其他人
					{
						OnUserListAtDesk(&pStateChange->uisUserItem);
						if (m_pGameInfo->bShowUserInfo==true&&(GetComType()!=TY_MATCH_GAME))
						{
							char szTalk[255];
							sprintf(szTalk, "%s 进来了", pStateChange->uisUserItem.GameUserInfo.nickName);
							m_MessageHandle.InsertNormalMessage(szTalk);
						}
					}
					else	// 若是自己坐到某个桌子，则需要找到同桌已有玩家的信息，并更新信息，用于换桌
					{
						//OnBnClickedMax();
						//效验状态
						//m_bWatchMode = pStateChange->uisUserItem.GameUserInfo.bDeskStation == USER_WATCH_GAME;
						BYTE bMeDeskStation=m_pGameInfo->uisMeUserInfo.GameUserInfo.bDeskNO;
						//DebugPrintf("自己所在桌[%d]，消息中坐到[%d]桌",
						//	m_pGameInfo->uisMeUserInfo.GameUserInfo.bDeskNO,
						//	pStateChange->uisUserItem.GameUserInfo.bDeskNO
						//	);

						//发送获取信息
						MSG_GM_S_ClientInfo ClientInfo;
						ClientInfo.bEnableWatch=m_pGameInfo->bEnableWatch;
						SendGameData(&ClientInfo,sizeof(ClientInfo),MDM_GM_GAME_FRAME,ASS_GM_GAME_INFO,0);
						UpdateGameTitle();
					}

					::SendMessage(m_TalkMessage.m_hWnd,WM_VSCROLL,SB_BOTTOM,0);

					return 0;
				}
			case ACT_USER_CUT:		//玩家断线or强退了
				{

					//处理数据
					m_UserListDlg.UpdateGameUser(&pStateChange->uisUserItem);
					m_pGameView->UpdateUserInfo(pStateChange->bDeskStation);
					//机器人断线不用提示信息201103-16
					if(0 == pStateChange->uisUserItem.GameUserInfo.isVirtual)
					{
						//显示信息
						char szTalk[255];
						if(GetComType() != TY_MATCH_GAME)
						{
							sprintf(szTalk, "%s 断线了", pStateChange->uisUserItem.GameUserInfo.nickName);
						}
						else
						{
							sprintf(szTalk, "断线了", pStateChange->uisUserItem.GameUserInfo.nickName);
						}
						
						m_MessageHandle.InsertNormalMessage(szTalk);

					}
					//m_MessageHandle.InsertNormalMessage(TEXT("托管了"));
					//OutputDebugString("[aa]SDK 强退了");

					::SendMessage(m_TalkMessage.m_hWnd,WM_VSCROLL,SB_BOTTOM,0);

					return 0;
				}
			case ACT_USER_AGREE:	//用户同意
				{
					UserAgreeGame(pStateChange->bDeskStation);
					return 0;
				}
			case ACT_GAME_BEGIN:	//游戏开始
			case ACT_GAME_END:		//游戏结束
				{
					//m_bWatchMode = pStateChange->uisUserItem.GameUserInfo.bDeskStation == USER_WATCH_GAME;
					for (BYTE i=0;i<m_pGameInfo->uDeskPeople;i++)
					{
						
						if (m_pUserInfo[i]!=NULL) 
						{
							//DebugPrintf("m_pUserInfo[%d].GameUserInfo.nickName=%s",i,m_pUserInfo[i]->GameUserInfo.nickName);
							m_pUserInfo[i]->GameUserInfo.bUserState = USER_PLAY_GAME;
							m_UserListDlg.UpdateGameUser(m_pUserInfo[i]);
							//DebugPrintf("游戏开始时－m_pUserInfo[%d].state = %d——%s", i,
							//	m_pUserInfo[i]->GameUserInfo.bUserState,
							//	m_pGameInfo->uisMeUserInfo.GameUserInfo.nickName);
						}
					}
					m_pGameView->UpdateUserInfo(ViewStation(m_pGameInfo->uDeskPeople));
					if (m_pGameView->m_bShowRight)	///< 防止排队机和防作弊房间闪烁
					{
						::SendMessage(m_TalkMessage.m_hWnd,WM_VSCROLL,SB_BOTTOM,0);

					}
					return 0;
				}
			case ACT_USER_POINT:	//用户经验值
				{
					UserItemStruct * pUserItem=FindOnLineUser(pStateChange->uisUserItem.GameUserInfo.dwUserID);
					if (pUserItem == NULL)
					{
						return 0;
					}
					memcpy( pUserItem, &pStateChange->uisUserItem, sizeof(UserItemStruct) );
					m_UserListDlg.UpdateGameUser(pUserItem);
					if (GetMeUserInfo()->dwUserID == pStateChange->uisUserItem.GameUserInfo.dwUserID)// add by ramon 09.05.31 修复个人信息栏不刷新
					{
						memcpy(&m_pGameInfo->uisMeUserInfo, &pStateChange->uisUserItem, sizeof(UserItemStruct));
					}
					if(pUserItem->GameUserInfo.dwUserID==m_CurpUserInfoClicked->dwUserID)
					{
						//wushuqun 2009.5.20
						//修改积分不刷新BUG
						m_CurpUserInfoClicked = &pUserItem->GameUserInfo;
						NavigateVirFace();
						m_pRightFrame->ShowUserInfo();
					}

					CString str;
					//str.Format("玩家 %s,分数 %d,金币 %d",pUserItem->GameUserInfo.nickName,pUserItem->GameUserInfo.dwPoint,
					//	pUserItem->GameUserInfo.dwMoney);
					//OutputDebugString(str);

					if (pUserItem->GameUserInfo.bUserState!=USER_WATCH_GAME)
					{
						m_pGameView->UpdateUserInfo(pStateChange->bDeskStation);
					}

					//如果已打开游戏银行，更新银行的数值
					//Add by JianGuankun 2012.1.2
					if (GetMeUserInfo()->dwUserID == pStateChange->uisUserItem.GameUserInfo.dwUserID) //自己分数更改
					{
						if (m_pBank2Wnd)
						{
							m_pBank2Wnd->SetWealthInfor(GetMeUserInfo()->i64Bank,GetMeUserInfo()->i64Money);
						}
					}
					//End Add

					//本人积分更改判断是否需要离开
					if(GetMeUserInfo()->dwUserID == pUserItem->GameUserInfo.dwUserID)//是否需要离开游戏桌
					{
						if(GetMeUserInfo()->bUserState!= USER_PLAY_GAME && GetMeUserInfo()->i64Money < m_pGameInfo->uLessPoint)
						{
							//OnBnClickedQuitGame();    // PengJiLin, 2011-4-18
                            OnBnClickedExit();          // PengJiLin, 2011-4-18
						}	
					}
					if (m_pGameView->m_bShowRight)
					{
#if 0	//2010-7-16 yangyan change
						InvalidateRect(m_UserInfoRect,false);
#else
						InvalidateRect(&m_UserInfoRect);
#endif
					}
					::SendMessage(m_TalkMessage.m_hWnd,WM_VSCROLL,SB_BOTTOM,0);

					return 0;
				}
			case ACT_USER_CONTESTPOINT:
				{
					UserItemStruct * pUserItem=FindOnLineUser(pStateChange->uisUserItem.GameUserInfo.dwUserID);
					if (pUserItem == NULL)
					{
						return 0;
					}
					memcpy( pUserItem, &pStateChange->uisUserItem, sizeof(UserItemStruct) );
					m_UserListDlg.UpdateGameUser(pUserItem);
					if (GetMeUserInfo()->dwUserID == pStateChange->uisUserItem.GameUserInfo.dwUserID)// add by ramon 09.05.31 修复个人信息栏不刷新
					{
						memcpy(&m_pGameInfo->uisMeUserInfo, &pStateChange->uisUserItem, sizeof(UserItemStruct));
					}
					if(pUserItem->GameUserInfo.dwUserID==m_CurpUserInfoClicked->dwUserID)
					{
						//wushuqun 2009.5.20
						//修改积分不刷新BUG
						m_CurpUserInfoClicked = &pUserItem->GameUserInfo;
						NavigateVirFace();
					}

					CString str;
					//str.Format("玩家 %s,分数 %d,金币 %d",pUserItem->GameUserInfo.nickName,pUserItem->GameUserInfo.dwPoint,
					//	pUserItem->GameUserInfo.dwMoney);
					//OutputDebugString(str);

					if (pUserItem->GameUserInfo.bUserState!=USER_WATCH_GAME)
					{
						m_pGameView->UpdateUserInfo(pStateChange->bDeskStation);
					}
					//本人积分更改判断是否需要离开
					if(GetMeUserInfo()->dwUserID == pUserItem->GameUserInfo.dwUserID)//是否需要离开游戏桌
					{
						if(GetMeUserInfo()->bUserState!= USER_PLAY_GAME && GetMeUserInfo()->i64ContestScore < m_pGameInfo->i64LowChip)
						{
							//OnBnClickedQuitGame();    // PengJiLin, 2011-4-18
							OnBnClickedExit();          // PengJiLin, 2011-4-18
						}	
					}
					if (m_pGameView->m_bShowRight)
					{
#if 0	//2010-7-16 yangyan change
						InvalidateRect(m_UserInfoRect,false);
#else
						InvalidateRect(&m_UserInfoRect);
#endif
					}
					::SendMessage(m_TalkMessage.m_hWnd,WM_VSCROLL,SB_BOTTOM,0);
					return 0;
				}


			case ACT_USER_QUEUE_DISMISS:
				{
					CString debug;
					debug.Format("xxxxxx 用户状态改变为ACT_USER_QUEUE_DISMISSACT_USER_QUEUE_DISMISSACT_USER_QUEUE_DISMISS");
					OutputDebugString(debug);
					m_bIsQueueDisMiss = true;
					return 0;
				}

			case ACT_USER_CHARM://更新魅力值
				{
					UserItemStruct * pUserItem=FindOnLineUser(pStateChange->uisUserItem.GameUserInfo.dwUserID);
					if (pUserItem == NULL)
					{
						return 0;
					}
					memcpy( pUserItem, &pStateChange->uisUserItem, sizeof(UserItemStruct) );
					m_UserListDlg.UpdateGameUser(pUserItem);
					if (GetMeUserInfo()->dwUserID == pStateChange->uisUserItem.GameUserInfo.dwUserID)// add by ramon 09.05.31 修复个人信息栏不刷新
					{
						memcpy(&m_pGameInfo->uisMeUserInfo, &pStateChange->uisUserItem, sizeof(UserItemStruct));
					}
					if(pUserItem->GameUserInfo.dwUserID==m_CurpUserInfoClicked->dwUserID)
					{
						NavigateVirFace();
					}
					if (pUserItem->GameUserInfo.bUserState!=USER_WATCH_GAME)
					{
						m_pGameView->UpdateUserInfo(pStateChange->bDeskStation);
					}
					return 0;
				}

			case ACT_USER_AVATAR_CHANGE: //玩家形象更新
				{
					UserItemStruct * pUserItem = FindOnLineUser(pStateChange->uisUserItem.GameUserInfo.dwUserID);
					if (pUserItem == NULL)
					{
						return 0;
					}

					//更新游戏右边框的头像
					if (m_pRightFrame)
					{
						m_pRightFrame->ShowUserInfo();
					}

					m_pGameView->UpdateViewFace(NULL);

					return 0;
				}
			}
		}
		break;
	case IPC_TROTTING:
		{
			TMailItem* pMailMsg = (TMailItem*)pControlData;
			m_pPmdWnd->AdjustPos(this->GetSafeHwnd());
			m_pPmdWnd->StartPmdLabel(120000,pMailMsg->szMsg,20,true);
			ShowWindow(SW_SHOWNORMAL);
			m_pPmdWnd->ShowCtrl();
			return 0;
		}
		break;

	case IPC_EX_SKIN:
		{
			//换肤通知
			CString strUserName = (static_cast<MSG_GP_R_LogonResult*>(pControlData))->szName;
			m_skinmgr.SetLogOnUserName(strUserName);
			m_skinmgr.Notify();
			
            if (m_pGameView->m_bShowRight)
			    LoadSkin(false);
            else
                LoadSkin(true);

            Invalidate();
			if (m_TalkMessage.IsWindowVisible())
			{
				m_TalkMessage.LoadScrollSkin();
				m_TalkMessage.Invalidate();
			}

			if (m_TalkMessage.m_DiyChat.IsWindowVisible())
			{
				m_TalkMessage.m_DiyChat.LoadSkin();
				m_TalkMessage.m_DiyChat.Invalidate();
			}

			if (m_DlgShowLeft.IsWindowVisible())
			{
				m_DlgShowLeft.LoadSkin();
				m_DlgShowLeft.Invalidate();
			}

			if (m_pUserDetailsDlg != NULL)
			{
				m_pUserDetailsDlg->LoadSkin();
				m_pUserDetailsDlg->Invalidate();
			}

			if (m_UserListDlg.m_UserList.IsWindowVisible())
			{
				m_UserListDlg.m_UserList.InitIniFile();
				m_UserListDlg.m_UserList.LoadSkin();
				m_UserListDlg.m_UserList.Invalidate();
			}

			if (m_dlgBroadcast != NULL)
			{
				m_dlgBroadcast->LoadSkin();
				m_dlgBroadcast->Invalidate();
			}

			if (m_dlgKickProp != NULL)
			{
				m_dlgKickProp->LoadSkin();
				m_dlgKickProp->Invalidate();
			}

			if (m_pPropPannel != NULL)
			{
				m_pPropPannel->LoadSkin();
				m_pPropPannel->Invalidate();
				if (m_pPropPannel->m_pLogic != NULL)
				{
					if (m_pPropPannel->m_pLogic->m_dlgBroadcast != NULL)
					{
						m_pPropPannel->m_pLogic->m_dlgBroadcast->LoadSkin();
						m_pPropPannel->m_pLogic->m_dlgBroadcast->Invalidate();
					}
					if (m_pPropPannel->m_pLogic->m_pPropSelUser != NULL)
					{
						m_pPropPannel->m_pLogic->m_pPropSelUser->LoadSkin();
						m_pPropPannel->m_pLogic->m_pPropSelUser->Invalidate();
					}
					if (m_pPropPannel->m_pLogic->m_pPersonBank != NULL)
					{
						m_pPropPannel->m_pLogic->m_pPersonBank->LoadSkin();
						m_pPropPannel->m_pLogic->m_pPersonBank->Invalidate();
					}
				}
			}
		}

		break;
	}

	return 0;
}
//游戏消息处理函数
bool CLoveSendClassInExe::HandleGameMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize)
{
	//OutputDebugString(_T("SDK的 HandleGameMessage 函数"));
	//CString cStr1;
	//cStr1.Format(_T("MDM_GM_GAME_NOTIFY_bMainID=%d,bAssistantID=%d"),pNetHead->bMainID,pNetHead->bAssistantID);
	//OutputDebugString(cStr1.GetBuffer());

	if (pNetHead->bMainID == MDM_GM_GAME_NOTIFY)
	{
		switch (pNetHead->bAssistantID)
		{	
		case ASS_PROP:				//道具
			{
				if (uDataSize!=sizeof(_TAG_PROP_MOIVE_USE)) {
					//WriteInfo(_T("CLoveSendClassInExe::HandleGameMessage 00"));
					return FALSE;
				}
				_TAG_PROP_MOIVE_USE *propMoive=(_TAG_PROP_MOIVE_USE*)pNetData;

				UserProp(propMoive);// pstProp->sendDeskStation,pstProp->recvDeskStation,pstProp->iValue);
				return true;
			}
		}
	}
	CString cStr;
	cStr.Format(_T("CLoveSendClassInExe::HandleGameMessage 11 Main_id=%d,AssID=%d"),pNetHead->bMainID,pNetHead->bAssistantID);
	//WriteInfo(cStr.GetBuffer());
	return false;
}

//经验盒消息处理函数
bool CLoveSendClassInExe::HandlePointMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize)
{
	return false;
}

bool CLoveSendClassInExe::UserProp(_TAG_PROP_MOIVE_USE *propMoive)
{
	int attribAction=propMoive->nPropActionAttrib;
	int attribValue=propMoive->nPropValueAttrib;


	if(attribAction & 0x40)
	{
		int iValue=(attribValue & 0x7F000000)>>24;
		m_stProp.iValue=iValue;
		m_stProp.sendDeskStation=propMoive->nUserStation;
		m_stProp.recvDeskStation=propMoive->nTargetUserStation;
		m_pGameView->SetGif(ViewStation(propMoive->nUserStation),iValue,2000);
		if (m_pPropPannel != NULL)
			SetTimer(ID_GIF_TIMER,m_pPropPannel->GetPropDisplayTime(),NULL);
		else
			SetTimer(ID_GIF_TIMER,500,NULL);
		//{添加动画道具声音 zxj 2009-9-17
		//CString soundFile;
		//soundFile.Format("..\\image\\prop\\PropGif%d%s.mp3",m_stProp.iValue%1000, m_stProp.iValue<1000?"A":"B");
		//BZSoundPlay(this,soundFile.GetBuffer(),0,1);
		//添加结束}
	}
	return true;
}
//桌号换算到视图位置
BYTE CLoveSendClassInExe::ViewStation(BYTE bDeskStation)
{
	BYTE bViewStation=(bDeskStation+m_pGameInfo->uDeskPeople-m_pGameInfo->uisMeUserInfo.GameUserInfo.bDeskStation);
	switch (m_pGameInfo->uDeskPeople)
	{
	case 1: { bViewStation+=1; break; }
	case 2: { bViewStation+=1; break; }
	case 3: { bViewStation+=1; break; }
	case 4: { bViewStation+=2; break; }
	case 5: { bViewStation+=2; break; }
	case 6: { bViewStation+=3; break; }
	case 7: { bViewStation+=3; break; }
	case 8: { bViewStation+=4; break; }
	}
	return bViewStation%m_pGameInfo->uDeskPeople;
}
//设置定时器
void CLoveSendClassInExe::SetGameTimer(BYTE bDeskStation, UINT uMaxTime, UINT uTimeID)
{
	if (m_uTimeID!=0L) KillGameTimer(m_uTimeID);
	if ((bDeskStation<m_pGameInfo->uDeskPeople)&&(uMaxTime>0L))
	{
		m_uTimeID=uTimeID;
		m_uTimeCount=uMaxTime;
		m_bTimeStation=bDeskStation;
		SetTimer(uTimeID,1000,NULL);
		m_pGameView->SetUserTimer(ViewStation(bDeskStation),m_uTimeCount);
	}
	return;
}
//删除定时器
void CLoveSendClassInExe::KillGameTimer(UINT uTimeID)
{
	//调整定时器
	if (m_uTimeID==0L) return;
	if (uTimeID==0L) uTimeID=m_uTimeID;
	if (m_uTimeID==uTimeID)
	{
		//清理界面
		KillTimer(m_uTimeID);
		if (m_bTimeStation!=255)
			m_pGameView->SetUserTimer(ViewStation(m_bTimeStation),0);

		//设置数据
		m_uTimeID=0L;
		m_uTimeCount=0L;
		m_bTimeStation=255;
	}
	return;
}
//定时器消息
void CLoveSendClassInExe::OnTimer(UINT nIDEvent)
{
	//if(nIDEvent==1)
	//{
	//	KillTimer(1);
	//	Invalidate(FALSE);
	//}
	switch(nIDEvent)//动画
	{
	case 1:
		{
			KillTimer(1);
			int nDefaultIndex=m_InputMessage.GetCurSel();
			int nTalkCount=m_InputMessage.GetCount();
			if(nTalkCount>m_nTalkDefaultCount)
				nDefaultIndex-=nTalkCount-m_nTalkDefaultCount;
			if(nDefaultIndex>=0)
				SendTalkMessage(nDefaultIndex+1);
		}
		break;
	case ID_GIF_TIMER:
		if(m_pGameView->m_pGif == NULL)
		{
			OutputDebugString("pro::----------m_pgif=NULL");
			KillTimer(ID_GIF_TIMER);
			m_pGameView->UpdateViewFace(NULL);
			PlayNextGif();
		}
		else
		{
			m_pGameView->UpdateViewFace(NULL);
			m_pGameView->CountFrame();
		}
		break;
	case ID_GIF_TIMER2:
		if(m_pGameView->m_pGif == NULL)
		{
			KillTimer(ID_GIF_TIMER2);
			m_pGameView->UpdateViewFace(NULL);
		}
		else
		{
			m_pGameView->UpdateViewFace(NULL);
			m_pGameView->CountFrame();
		}
		break;
	}

	//删除定时器
	if (nIDEvent==m_uTimeID)
	{
		if ((m_uTimeCount>0L)&&(m_bTimeStation!=255))
		{
			m_uTimeCount--;
			bool bHandle=OnGameTimer(m_bTimeStation,m_uTimeID,m_uTimeCount);
			if ((bHandle==false)||(m_uTimeCount==0))
				KillGameTimer(m_uTimeID);
			else m_pGameView->SetUserTimer(ViewStation(m_bTimeStation),m_uTimeCount);
		}
		else KillGameTimer(m_uTimeID);
	}
	__super::OnTimer(nIDEvent);
	return;
}
//播放下一张gif动画
bool CLoveSendClassInExe::PlayNextGif()
{
	m_pGameView->SetGif(ViewStation(m_stProp.recvDeskStation),m_stProp.iValue + 1000,2000);
	if (m_pPropPannel != NULL)
		SetTimer(ID_GIF_TIMER2,m_pPropPannel->GetPropDisplayTime(),NULL);
	else
		SetTimer(ID_GIF_TIMER2,500,NULL);
	return true;
}
// 请求平台传来同桌玩家的数据
void CLoveSendClassInExe::GetOnLineUser( BYTE bMeDeskStation )
{
	CWnd * winApp=AfxGetApp()->m_pMainWnd;
	if(winApp && winApp->m_hWnd)
		winApp->PostMessage(WM_REQUEST_PLAYERLIST, (WPARAM)bMeDeskStation, 0);
}
// 根据用户ID找本桌玩家中是否有
UserItemStruct *CLoveSendClassInExe::FindOnLineUser(long int dwUserID)
{
	INT_PTR uActiveCount=m_ThisDeskPtrArray.GetCount();
	for (int i=0;i<uActiveCount;i++)
	{
		UserItemStruct * pFindUserItem=(UserItemStruct *)m_ThisDeskPtrArray.GetAt(i);
		if ((pFindUserItem!=NULL)&&(pFindUserItem->GameUserInfo.dwUserID==dwUserID)) 
			return pFindUserItem;
	}
	return NULL;
}
// 加入一个玩家到本桌中
void CLoveSendClassInExe::OnUserListAtDesk(void *pUserItemVoid)
{
	UserItemStruct *pUserItemToAdd = (UserItemStruct *)pUserItemVoid;
	//查找在线用户
	bool bNewItem=false;
	if (pUserItemToAdd->GameUserInfo.dwUserID == m_pGameInfo->uisMeUserInfo.GameUserInfo.dwUserID)
	{
		memcpy(&m_pGameInfo->uisMeUserInfo, pUserItemToAdd, sizeof(UserItemStruct));
	}
	UserItemStruct * pUserItem=FindOnLineUser(pUserItemToAdd->GameUserInfo.dwUserID);
	if (pUserItem==NULL)
	{
		//申请内存
		bNewItem = true;
		pUserItem =new UserItemStruct;
		ASSERT(pUserItem!=NULL);
	}
	// 如果已经有了的，也要复制数据，因为有可能玩家在同一桌内换到不同位置
	memcpy( pUserItem, pUserItemToAdd, sizeof(UserItemStruct) );
	
	//加入队列
	if (bNewItem==true) m_ThisDeskPtrArray.Add(pUserItem);

	//每当有用户坐桌，就重新排一下视频窗口位置
	set_videos_userid();

	GameUserCome(pUserItem->GameUserInfo.bDeskStation,
		pUserItem,
		pUserItem->GameUserInfo.bUserState==USER_WATCH_GAME);
}

//网络读取消息
bool CLoveSendClassInExe::OnSocketReadEvent(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize)
{
	//过滤处理
	if (m_pGameInfo->uisMeUserInfo.GameUserInfo.bDeskNO==255) 
		return true;
	//处理消息
	switch (pNetHead->bMainID)
	{
	case MDM_GR_NETSIGNAL:
		{
			//wushuqun 2009.6.18
			//此处的MDM_GR_NETSIGNAL消息为IPC模拟消息
			//为解决播放器中会出现有旁观模式BUG
			if (pNetHead->bAssistantID == MDM_GR_NETSIGNAL && pNetHead->bHandleCode == MDM_GR_NETSIGNAL)
			{
				m_bWatchMode = false;				
				m_bWatchOther = true;
				return true;
			}
			//////////////////////////////////////
			getNetSignal(pNetData);
			return true;
		}
	case MDM_GM_GAME_FRAME:		//框架消息
		{
			if (GetSafeHwnd()==NULL)
				return true;
			return OnFrameMessage(pNetHead,pNetData,uDataSize);
		}
	case MDM_GM_MESSAGE:		//信息消息
		{
			if (GetSafeHwnd()==NULL) 
				return true;
			return OnSystemMessage(pNetHead,pNetData,uDataSize);
		}
	case MDM_GM_GAME_NOTIFY:	//游戏消息
		{
			if (this == NULL)
			{
				AFCMessageBox("o");
				return true;
			}
			if (!this->m_hWnd) 
				return true;
			//wushuqun 2009.6.16
			//OutputDebugString("SDK 调用HandleGameMessage之前");
			bool bRet = HandleGameMessage(pNetHead,pNetData,uDataSize);
			if (m_pGameInfo->bIsInRecord)
			{
				SetRecordView();
			}

			return bRet;
		}
	case MDM_GR_POINT:		//经验盒
		{
			if(NULL == GetSafeHwnd())
				return true;
			return HandlePointMessage(pNetHead,pNetData,uDataSize);
		}
	case MDM_GR_PROP:		//使用道具
		{
			if(!GetSafeHwnd())
				return true;
			return OnPropMessage(pNetHead,pNetData,uDataSize);
		}
	case MDM_GR_MONEY:		// 来自银行的消息
		{
			if(NULL == GetSafeHwnd())
				return true;
			return OnAboutMoney(pNetHead,pNetData,uDataSize);
		}
	case MDM_BANK:
		{
			if (m_pBank2Wnd)
			{
				m_pBank2Wnd->OnNetMessage(pNetHead,pNetData,uDataSize);
			}
		}
		break;
    case MDM_GR_ROOM2GAME:  // PengJiLin, 2010-10-14, 由房间发过来的消息
        {
            switch (pNetHead->bAssistantID)
            {
            case ASS_GR_R2G_USEPROP:    // PengJiLin, 2010-10-14, 游戏端道具的即买即用功能
                {
                    if(uDataSize != sizeof(_TAG_USERPROP)) return false;

                    _TAG_USERPROP* pSendProp = (_TAG_USERPROP*)pNetData;

                    int propCount=Glb().userPropLibrary.GetCount();
                    _TAG_USERPROP *userProp=NULL;
                    bool bPropExist=false;
                    int iPropIndex = 0;
                    for(int i=0;i<propCount;i++)
                    {
                        userProp=Glb().userPropLibrary.GetAt(i);
                        if(userProp->nPropID==pSendProp->nPropID)
                        {
                            bPropExist=true;
                            iPropIndex = i;
                            break;
                        }
                    }

                    // 找到
                    if(true == bPropExist)
                    {
						if (pNetHead->bHandleCode == DTK_GP_PROP_BUYANDUSE_SUCCEED)
						{
							if (m_pRightFrame && m_pRightFrame->m_pBoardCastWnd)
								m_pRightFrame->m_pBoardCastWnd->UseProp_Buy(iPropIndex);
						}
                        else
							UsePropItem(iPropIndex, FALSE);
                    }

                    return true;
                }
                break;
			case ASS_GR_GET_NICKNAME_ONID_INGAME:      // lxl, 2010-11-17, 根据用户ID获取昵称
			{
				OutputDebugString("游戏中成功收到服务器发来的昵称");
				if(ASS_GR_GETNICKNAME_SUCCESS == pNetHead->bHandleCode) // 操作成功
				{
                // 效验数据
					if (uDataSize != sizeof(MSG_GR_S_GetNickNameOnID_t)) return false;
					MSG_GR_S_GetNickNameOnID_t* pGetNickNameOnID = (MSG_GR_S_GetNickNameOnID_t *)pNetData;

					m_pPersonBank->SetNickName(pGetNickNameOnID->szNickName); 
					return true;
				}
				return true;
			}
			break;
            default:
                break;
            }
        }
	}
	return false;
}
/// 处理银行的消息
/// 与客户端的GameRoomEx中大同小异，只操作本地金币数和银行面板数据的变化
/// @param[in] pNetHead 协议头
/// @param[in] pNetData 数据包
/// @param[in] uDataSize 数据包大小
/// @return 如果成功处理，则返回true，返回false表示处理该条消息失败，其中一种可能是因为协议不匹配，例如修改了头文件却没有都重新编译
/// 
bool CLoveSendClassInExe::OnAboutMoney(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize)
{
	CBcfFile fMsg(CBcfFile::GetAppPath()+"..\\ClientMessage.bcf");

	/// 根据子协议ID的不同，数据长度也有所不同，因此在每个case里自行判断
	/// 需要处理的消息有：来自于服务器的所有与银行和钱包金额变化的消息
	switch (pNetHead->bAssistantID)
	{
	case ASS_GR_TRANSFER_RECORD_INGAME:	///< 转帐记录
		{
			//效验数据
			if(uDataSize!=sizeof(MSG_GR_R_TransferRecord_t))
			{
				return false;
			}
			MSG_GR_R_TransferRecord_t *pRecord = (MSG_GR_R_TransferRecord_t *)pNetData;
			m_pPersonBank->OnReceivedTransferRecord( pRecord );
			break;
		}
	case ASS_GR_CHANGE_PASSWD_INGAME:	///< 修改密码
		{
			/// 只需要bHandelCode就可以了
			CString strBuffer;
			CString str;
			if (pNetHead->bHandleCode == ASS_GR_OPEN_WALLET_SUC)
			{
				str = fMsg.GetKeyVal("GameRoom","ChangePasswdOK","修改银行密码成功！请妥善保管新密码。");
			}
			else
			{
				str = fMsg.GetKeyVal("GameRoom","ChangePasswdFailed","修改银行密码失败！");
			}
			strBuffer.Format(str);
			AFCMessageBox(strBuffer,m_pGameInfo->szGameName);//,MB_OK,this);
			break;
		}
	case ASS_GR_PAY_MONEY:
		{
			switch(pNetHead->bHandleCode)
			{
			case 1://成功
				{
					break; // 已经由大厅处理过，并有另一条协议传达了改变内容，因此这里不再重复处理
				}
			case 0: //不成功
				{
					CString strBuffer;
					strBuffer.Format(TEXT("您的金币额不够此次支付!"));
					AFCMessageBox(strBuffer,m_pGameInfo->szGameName);//,MB_OK,this);
					break;
				}
			}
			break;
		}
	case ASS_GR_OPEN_WALLET_INGAME:
	case ASS_GR_OPEN_WALLET:
		{
			switch (pNetHead->bHandleCode)
			{
			case ASS_GR_OPEN_WALLET_ERROR:
				{
					CString strBuffer;
					strBuffer.Format(TEXT("打开钱柜失败,请稍后再试!"));
					AFCMessageBox(strBuffer,m_pGameInfo->szGameName);//,MB_OK,this);
					break;
				}
			case ASS_GR_OPEN_WALLET_ERROR_PSW:
				{
					CString strBuffer;
					strBuffer.Format(TEXT("二级密码错误,请重试!"));
					AFCMessageBox(strBuffer,m_pGameInfo->szGameName);//,MB_OK,this);
					break;
				}
			case ASS_GR_OPEN_WALLET_SUC:
				{
					DebugPrintf("打开银行");
					//效验数据
					MSG_GR_UserBank * pBank=(MSG_GR_UserBank *)pNetData;
					CString strBuffer;
					if(uDataSize!=sizeof(MSG_GR_UserBank)) 
					{
						CString str;
						str = fMsg.GetKeyVal("GameRoom","ErrorInGettingMoney","打开钱柜失败,请稍后再试!");
						strBuffer.Format(str);
						AFCMessageBox(strBuffer,m_pGameInfo->szGameName);//,MB_OK,this);
					}

					/*if (pBank->nVer == 1) //老版银行
					{
						AfxSetResourceHandle(GetModuleHandle(FACE_DLL_NAME));
						//建立窗口
						if (m_pPersonBank == NULL) 
						{
							try
							{
								m_pPersonBank=new CPBankDlg(this);
								m_pPersonBank->Create(IDD_BANK,this);
								if (m_pPersonBank==NULL)
									return false;
							}
							catch (...) { return false; }
						}
						if (m_pPersonBank!=NULL)
						{
							DebugPrintf("游戏中给银行成员赋值dwBank=[%d]dwMoney=[%d]---%d行", pBank->i64Bank, m_pGameInfo->uisMeUserInfo.GameUserInfo.i64Money
								, __LINE__);
							/// 先赋值
							m_pPersonBank->SetWealthInfor(pBank->i64Bank, m_pGameInfo->uisMeUserInfo.GameUserInfo.i64Money);
							if (m_pPersonBank->GetSafeHwnd()!=NULL)
							{
								AfxSetResourceHandle(GetModuleHandle(FACE_DLL_NAME));
								/// 给银行的成员赋值
								DebugPrintf("游戏中准备打开银行界面[%d]---%d行", m_pPersonBank->GetSafeHwnd()
									, __LINE__);
								m_pPersonBank->CenterWindow();
								m_pPersonBank->ShowWindow(SW_SHOW);
								DebugPrintf("游戏中银行界面退出[%d]---%d行", m_pPersonBank->GetSafeHwnd()
									, __LINE__);
							}
						}
						AfxSetResourceHandle(GetModuleHandle(NULL));
					}
					else if (pBank->nVer == 2) //新版银行*/
					{	
						if (!m_pBank2Wnd)
						{
							//非百家乐游戏不让打开游戏银行
// 							TCHAR szKey[10];
// 							CString sPath=CBcfFile::GetAppPath();
// 							CBcfFile fsr( sPath + "..\\SpecialRule.bcf");
// 							wsprintf(szKey, "%d", m_pGameInfo->uNameID);
// 							int iResult = fsr.GetKeyVal (_T("BJLType"), szKey, 0);
// 
// 							if (iResult == 0)
// 							{
// 								return true;
// 							}

							m_pBank2Wnd = new CPBank4GameWnd(this,m_pGameInfo);
							m_pBank2Wnd->Create(this->m_hWnd,NULL,UI_WNDSTYLE_DIALOG,0,0,0,0,0,NULL);
							m_pBank2Wnd->SetWealthInfor(pBank->i64Bank,GetMeUserInfo()->i64Money);
							m_pBank2Wnd->SetBankVersion(pBank->nVer);
							m_pBank2Wnd->CenterWindow();
							strcpy(m_pBank2Wnd->m_szTwoPassword,pBank->szTwoPassword);
						}

						if(m_pBank2Wnd)
						{
							m_pBank2Wnd->ShowWindow(SW_SHOW);
						}

					}

					break;
				}
			}
			break;
		}
	case ASS_GR_CHECK_OUT://取钱
	case ASS_GR_CHECK_OUT_INGAME://游戏中取钱
		{
			if(pNetHead->bHandleCode==ASS_GR_CHECKOUT_ERROR)
			{
				if (ASS_GR_CHECK_OUT_INGAME == pNetHead->bAssistantID)
				{
					CString strBuffer;
					CString str;
					str = fMsg.GetKeyVal("GameRoom","NetWorkBusy","取出失败，可能原因是密码不正确或余额不足！");
					strBuffer.Format(str);
					DUIMessageBox(NULL,MB_OK|MB_ICONERROR,m_pGameInfo->szGameName,strBuffer.GetBuffer());
				}
			}

			if(pNetHead->bHandleCode==ASS_GR_CHECKOUT_ERROR_PLAYING)
			{
				if (ASS_GR_CHECK_OUT_INGAME == pNetHead->bAssistantID)
				{
					CString strBuffer;
					CString str;
					str = fMsg.GetKeyVal("GameRoom","NoDrawForPlaying","您正在游戏中，不能取款。");
					strBuffer.Format(str);
					DUIMessageBox(NULL,MB_OK|MB_ICONINFORMATION,m_pGameInfo->szGameName,strBuffer.GetBuffer());
				}
			}

			if(pNetHead->bHandleCode==ASS_GR_CHECKOUT_SUC)
			{
				//效验数据
				if (uDataSize!=sizeof(MSG_GR_CheckMoney)) return false;
				MSG_GR_CheckMoney * pCheckOut=(MSG_GR_CheckMoney *)pNetData;
				if(pCheckOut->dwUserID==m_pGameInfo->uisMeUserInfo.GameUserInfo.dwUserID)//自己
				{
					//更新对话框
					if (m_pPersonBank!=NULL) 
					{
						if (m_pPersonBank->GetSafeHwnd()!=NULL) 
						{
							m_pPersonBank->UpdateUserMoney(0, pCheckOut->i64CheckOut);
						}
					}
				}
				//更新金币信息
				FlushUserMoney(pCheckOut->dwUserID,pCheckOut->i64CheckOut);
			}
			break;
		}
	case ASS_GR_CHECK_IN://存钱
	case ASS_GR_CHECK_IN_INGAME://游戏中存钱
		{
			if(pNetHead->bHandleCode==ASS_GR_CHECKIN_ERROR)
			{
				if (ASS_GR_CHECK_IN_INGAME == pNetHead->bAssistantID)
				{
					CString strBuffer;
					CString str;
					str = fMsg.GetKeyVal("GameRoom","NetWorkBusySave","网络拥塞,存入失败!请稍后再试!");
					strBuffer.Format(str);
					 DUIMessageBox(NULL,MB_OK|MB_ICONINFORMATION,m_pGameInfo->szGameName,strBuffer.GetBuffer());
				}
			}
			if(pNetHead->bHandleCode==ASS_GR_CHECKIN_ERROR_PLAYING)
			{
				if (ASS_GR_CHECK_IN_INGAME == pNetHead->bAssistantID)
				{
					CString strBuffer;
					CString str;
					str = fMsg.GetKeyVal("GameRoom","NoSaveForPlaying","您正在游戏中，不能存款。");
					strBuffer.Format(str);
					 DUIMessageBox(NULL,MB_OK|MB_ICONINFORMATION,m_pGameInfo->szGameName,strBuffer.GetBuffer());
				}
			}

			if(pNetHead->bHandleCode==ASS_GR_CHECKIN_ZHUANGJIA)  //百家乐类 上庄中不能存款 add by lxl 2010-11-5
			{
				if (ASS_GR_CHECK_IN_INGAME == pNetHead->bAssistantID)
				{
					CString strBuffer;
					CString str;
					str = fMsg.GetKeyVal("GameRoom","NoSaveForZhuangjia","您正在上庄中，不能存款。");
					strBuffer.Format(str);
					 DUIMessageBox(NULL,MB_OK|MB_ICONINFORMATION,m_pGameInfo->szGameName,strBuffer.GetBuffer());
				}
			}

			if(pNetHead->bHandleCode==ASS_GR_CHECKIN_SUC)
			{
				//效验数据
				if (uDataSize!=sizeof(MSG_GR_CheckMoney)) return false;
				MSG_GR_CheckMoney * pCheckIn=(MSG_GR_CheckMoney *)pNetData;

				if(pCheckIn->dwUserID==m_pGameInfo->uisMeUserInfo.GameUserInfo.dwUserID)//自己
				{
					//更新对话框
					if (m_pPersonBank!=NULL) 
					{
						m_pPersonBank->UpdateUserMoney(1, pCheckIn->i64CheckOut);
					}
				}
				FlushUserMoney(pCheckIn->dwUserID,0-pCheckIn->i64CheckOut);
			}

			break;
		}
	case ASS_GR_TRANSFER_MONEY:		   //转帐
	case ASS_GR_TRANSFER_MONEY_INGAME: //游戏中转帐
		{
			CString strBuffer;
			CString str;
			TCHAR szNum[128], szBank[128];
			//效验数据
			if (uDataSize!=sizeof(MSG_GR_S_TransferMoney)) return false;
			MSG_GR_S_TransferMoney * pTransfer=(MSG_GR_S_TransferMoney *)pNetData;
			/// 如果转帐不成功，且是在平台发出的转帐指令，则弹出对话框，若是游戏中发的指令，则平台只响应对话框事件
			if (ASS_GR_TRANSFER_MONEY_INGAME == pNetHead->bAssistantID)
			{
				switch(pNetHead->bHandleCode)
				{
				case ASS_GR_TRANSFER_SUC:	///< 转帐成功					
					{
						/////Added by xqm 20101109
						/////给Z服务器发送转帐成功的消息,然后由Z服务器给转帐目标客户端发送一条转帐过来的消息
						//OutputDebugString("游戏发给Z服务器");
						//SendGameData(pNetData, uDataSize, MDM_GR_MONEY, ASS_GR_TRANSFER_MONEY,ASS_GR_TRANSFER_SUC);	
						//SendGameData(pNetData, uDataSize, MDM_GR_MONEY, ASS_GR_TRANSFER_MONEY,ASS_GR_TRANSFER_SUC);	
						///SendGameData((void *)pNetData, uDataSize, pNetHead->bMainID, ASS_GR_TRANSFER_SUC, 0);
						//SendGameData(pNetData, uDataSize, MDM_GR_MONEY, ASS_GR_TRANSFER_MONEY,ASS_GR_TRANSFER_SUC);
						break;
					}
				case ASS_GR_PASSWORD_ERROR:												///< 转帐密码错误
					{
						str = fMsg.GetKeyVal("GameRoom","TransferPassWdError","您输入的密码没有通过验证，请重新输入密码！");
						strBuffer.Format(str, szNum);
						AFCMessageBox(strBuffer,m_pGameInfo->szGameName);//,MB_OK,this);
						return true;
					}
				case ASS_GR_TRANSFER_TOTAL_LESS:										///< 银行总额太小，不够资格
					{
						GlbGetNumString(szNum, pTransfer->i64ActualTransfer, Glb().m_nPowerOfGold, Glb().m_bUseSpace, Glb().m_strSpaceChar);
						str = fMsg.GetKeyVal("GameRoom","TransferTotalLess","您暂时还不可以转帐，银行存款达到或超过%s金币才可以转帐！");
						strBuffer.Format(str, szNum);
						AFCMessageBox(strBuffer,m_pGameInfo->szGameName);//,MB_OK,this);
						return true;
					}
					break;
				case ASS_GR_TRANSFER_TOO_LESS:											///< 单笔转帐数目太少
					{
						GlbGetNumString(szNum, pTransfer->i64ActualTransfer, Glb().m_nPowerOfGold, Glb().m_bUseSpace, Glb().m_strSpaceChar);
						str = fMsg.GetKeyVal("GameRoom","TransferTooLess","转帐不成功，单次转帐金额必须大于%s金币。");
						strBuffer.Format(str, szNum);
						AFCMessageBox(strBuffer,m_pGameInfo->szGameName);//,MB_OK,this);
						return true;
					}
					break;
				case ASS_GR_TRANSFER_MULTIPLE:											///< 单笔转帐数目必须是某数的倍数
					{
						GlbGetNumString(szNum, pTransfer->i64ActualTransfer, Glb().m_nPowerOfGold, Glb().m_bUseSpace, Glb().m_strSpaceChar);
						str = fMsg.GetKeyVal("GameRoom","TransferMultiple","转帐不成功！转帐金额必须是%s的整数倍。");
						strBuffer.Format(str, szNum);
						AFCMessageBox(strBuffer,m_pGameInfo->szGameName);//,MB_OK,this);
						return true;
					}
				case ASS_GR_TRANSFER_NOT_ENOUGH:										///< 银行金额不够本次转帐
					{
						GlbGetNumString(szNum, pTransfer->i64Money, Glb().m_nPowerOfGold, Glb().m_bUseSpace, Glb().m_strSpaceChar);
						GlbGetNumString(szBank, pTransfer->i64ActualTransfer, Glb().m_nPowerOfGold, Glb().m_bUseSpace, Glb().m_strSpaceChar);
						str = fMsg.GetKeyVal("GameRoom","TransferNotEnoughMoney","转帐不成功！您银行中只有%s金币，不够支付本次转帐%s金币。");
						strBuffer.Format(str, szBank, szNum);
						AFCMessageBox(strBuffer,m_pGameInfo->szGameName);//,MB_OK,this);
						return true;
					}
				case ASS_GR_TRANSFER_NO_DEST:											///< 转帐目标不存在
					{
						str = fMsg.GetKeyVal("GameRoom","AccountNotExist","转帐目标不存在,请查实帐号再试!");
						strBuffer.Format(str);
						AFCMessageBox(strBuffer,m_pGameInfo->szGameName);//,MB_OK,this);
						return true;
					}
					break;
				default:
					{
						CString strBuffer;
						CString str;
						str = fMsg.GetKeyVal("GameRoom","NetWorkBusySave","网络拥塞,存入失败!请稍后再试!");
						strBuffer.Format(str);
						AFCMessageBox(strBuffer,m_pGameInfo->szGameName);//,MB_OK,this);
						return true;
					}
				}
			}
			/// 转帐成功，设置大厅金币和银行界面金币
			if(pNetHead->bHandleCode==ASS_GR_TRANSFER_SUC)
			{
				TCHAR szAct[128];
				CString strBuffer;
				CString str;
				GlbGetNumString(szNum, pTransfer->i64Money, Glb().m_nPowerOfGold/*, false*/, Glb().m_bUseSpace, Glb().m_strSpaceChar);
				GlbGetNumString(szAct, pTransfer->i64ActualTransfer, Glb().m_nPowerOfGold/*, false*/, Glb().m_bUseSpace, Glb().m_strSpaceChar);
				/// 是自己转给别人
				if(pTransfer->UserID == m_pGameInfo->uisMeUserInfo.GameUserInfo.dwUserID)//自己
				{
					if (ASS_GR_TRANSFER_MONEY_INGAME == pNetHead->bAssistantID)
					{
						//提示用户
						if(pTransfer->bUseDestID)
						{
							str = fMsg.GetKeyVal("GameRoom","TransferCoinsId","您转给ID[%d]的玩家 %s 金币，扣除手续费后实际到帐 %s 金币!");
							strBuffer.Format(str, pTransfer->destUserID, szNum, szAct);
						}
						else
						{
							str = fMsg.GetKeyVal("GameRoom","TransferCoinsNn","您转给昵称[%s]的玩家 %s 金币，扣除手续费后实际到帐 %s 金币!");
							strBuffer.Format(str, pTransfer->szDestNickName, szNum, szAct);
						}
						AFCMessageBox(strBuffer,m_pGameInfo->szGameName);//,MB_OK,this);
					}
					//更新对话框
					if (m_pPersonBank!=NULL) 
					{
						m_pPersonBank->UpdateUserMoneyByTransfer(0, pTransfer->i64Money);
					}
				}
				/// 转帐目标是自己
				else if (pTransfer->destUserID==m_pGameInfo->uisMeUserInfo.GameUserInfo.dwUserID)
				{
					if (ASS_GR_TRANSFER_MONEY_INGAME == pNetHead->bAssistantID)
					{
						str = fMsg.GetKeyVal("GameRoom","BeTransfered","玩家[%d]转给您 %s 金币，扣除手续费后实际到帐 %s 金币!");
						strBuffer.Format(str, pTransfer->UserID, szNum, szAct);
						AFCMessageBox(strBuffer,m_pGameInfo->szGameName);//,MB_OK,this);
					}
					//更新对话框
					if (m_pPersonBank!=NULL) 
					{
						m_pPersonBank->UpdateUserMoneyByTransfer(1, pTransfer->i64ActualTransfer);
					}
				}
			}
			break;
		}
	default:
		break;
	}
	return true;
}
/// 刷新用户列表中的金币
/// @param dwUserID 用户ID号
/// @param uCount 可为正或负数的值，意思为加或减
bool CLoveSendClassInExe::FlushUserMoney(long dwUserID,__int64 i64Count)
{
	UserItemStruct * pUserItem=FindOnLineUser(dwUserID);
	if(pUserItem==NULL)
		return false;

	//设置数据
	pUserItem->GameUserInfo.i64Money += i64Count;
	pUserItem->GameUserInfo.i64Bank -= i64Count;//银行金币

	//更新座位区用户
	m_UserListDlg.UpdateGameUser(pUserItem);
	if(pUserItem->GameUserInfo.dwUserID==m_CurpUserInfoClicked->dwUserID)
	{
		NavigateVirFace();
	}
	return true;
}

/// 从游戏界面中打开银行界面
void CLoveSendClassInExe::OnVisitBank()
{
	//AfxSetResourceHandle(GetModuleHandle(FACE_DLL_NAME));
	////建立窗口
	//if (m_pPersonBank == NULL) 
	//{
	//	try
	//	{
	//		m_pPersonBank=new CPBankDlg(this);
	//		if (m_pPersonBank==NULL)
	//			return;
	//	}
	//	catch (...) { return; }
	//}
	//if (m_pPersonBank!=NULL)
	//{
	//	/// 先赋值
	//	m_pPersonBank->SetWealthInfor(100000, 500000);
	//	if (m_pPersonBank->GetSafeHwnd()==NULL)
	//	{
	//		AfxSetResourceHandle(GetModuleHandle(FACE_DLL_NAME));
	//		m_pPersonBank->DoModal();//Create(IDD_DIALOGBANK,this);
	//	}
	//}
	//AfxSetResourceHandle(GetModuleHandle(NULL));

	// duanxiaohui 20111103 体验场(免费场) 不能使用银行功能
	if (m_pGameInfo->dwRoomRule & GRR_EXPERCISE_ROOM)
	{
		AFCMessageBox("对不起, 体验场(免费场)不能使用银行功能", "温馨提示");
		return ;
	}
	// end duanxiaohui

	if((m_pPersonBank->GetSafeHwnd() != NULL) && (::IsWindowVisible(m_pPersonBank->GetSafeHwnd())))
	{
		/// 如果银行已经打开，再次点击时关闭银行
		::PostMessage(m_pPersonBank->GetSafeHwnd(), WM_COMMAND, MAKELONG(IDCANCEL,BN_CLICKED), 0);
	}
	else
	{
		SendGameData(MDM_GR_MONEY,ASS_GR_OPEN_WALLET_INGAME,0);
	}
}

//道具消息
bool CLoveSendClassInExe::OnPropMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize)
{
	switch(pNetHead->bAssistantID)
	{
		//wushuqun 2008.9.28
		//修改小喇叭使用后没有更新客户端道具面板显示 
	case ASS_PROP_SMALL_BOARDCASE:
		{
            // PengJiLin, 2010-10-13, 在房间使用小喇叭，游戏端的数量没有改变，须增加以下代码
            // boardCast->iLength 变量为房间传递过来的数量，直接使用
            _TAG_BOARDCAST *boardCast=(_TAG_BOARDCAST*)pNetData;
            int propCount=Glb().userPropLibrary.GetCount();
            _TAG_USERPROP *userProp=NULL;
            for(int i=0;i<propCount;i++)
            {
                userProp=Glb().userPropLibrary.GetAt(i);
                if(userProp->nPropID==boardCast->nPropID)
                {
                    userProp->nHoldCount = boardCast->iLength;                        
                }
            }
			
			OnSetPropImages();
			break;
		}
	case ASS_PROP_USEPROP:
		return OnUsePropResult(pNetHead,pNetData,uDataSize);
		break;
	case ASS_PROP_VIP_CHECKTIME:
		return OnPropVipCheckTime(pNetHead,pNetData,uDataSize);
		break;
    case ASS_PROP_NEW_KICKUSER:    // PengJiLin, 2010-9-14, 使用踢人卡踢人结果消息
        return OnNewKickUserResult(pNetHead,pNetData,uDataSize);
        break;
	}
	return true;
}
//更新同桌玩家数据
void CLoveSendClassInExe::ResetUserInfo(void *pUserInfoVoid)
{
	UserItemStruct *pUserInfo =(UserItemStruct *)pUserInfoVoid;
	INT_PTR nPlayerCount = m_ThisDeskPtrArray.GetCount();
	if (GetMeUserInfo()->dwUserID == pUserInfo->GameUserInfo.dwUserID)
	{
		memcpy(&m_pGameInfo->uisMeUserInfo, pUserInfo, sizeof(UserItemStruct));
		if (NULL != m_pPropPannel)
		{
			m_pPropPannel->ResetData();
		}
	}
	for (INT_PTR i=0; i<nPlayerCount; ++i)
	{
		UserItemStruct *pFindUserItem = (UserItemStruct *)m_ThisDeskPtrArray.GetAt(i);
		if (pFindUserItem->GameUserInfo.dwUserID == pUserInfo->GameUserInfo.dwUserID)
		{
			memcpy(pFindUserItem, pUserInfo, sizeof(UserItemStruct));
			bool bRet = m_UserListDlg.UpdateGameUser(pFindUserItem);
			//DebugPrintf("ResetUserInfo %s bUserState = %d", bRet?"成功":"失败", pFindUserItem->GameUserInfo.bUserState);
			break;
		}
	}
	if( pUserInfo->GameUserInfo.dwUserID== this->m_CurpUserInfoClicked->dwUserID)
	{
		NavigateVirFace();
	}
}

void CLoveSendClassInExe::OnAddFriend(bool bIsHave)
{
	CString s;
	s.Format("xxx CLoveSendClassInExe OnAddFriend %d, %d", bIsHave, m_dwCurrentUserID);
	OutputDebugString(s);
	if (bIsHave)
	{
		AFCMessageBox("对不起，该用户已经是您的好友，不需要再添加！", "提示");
	}
	else
	{
		//处理数据
		UserItemStruct * pUserItem=FindOnLineUser(m_dwCurrentUserID);
		if ((pUserItem==NULL))
			return;

		//Ctest _test;
		//添加请求留言
		CIMSetUser imsu;
		imsu.dwUser = m_dwCurrentUserID;
		imsu.szName = pUserItem->GameUserInfo.nickName;
		AfxSetResourceHandle(GetModuleHandle(FACE_DLL_NAME));
		if(imsu.DoModal()==IDCANCEL)
			return;
		AfxSetResourceHandle(GetModuleHandle(NULL));

		MSG_IM_C_ADDREQUEST* msg = new MSG_IM_C_ADDREQUEST;
		ZeroMemory(msg,sizeof(MSG_IM_C_ADDREQUEST));

		//CMainRoomEx *pWnd=(CMainRoomEx*)pParentWnd;
		msg->dwUserID= GetMeUserInfo()->dwUserID;
		msg->dwRequestedUserID= m_dwCurrentUserID;
		memcpy(&(msg->sUserName), GetMeUserInfo()->nickName,sizeof(GetMeUserInfo()->nickName));
		memcpy(&(msg->sRequestNotes),imsu.imWords,imsu.imWords.GetLength());
		msg->nMsgLength=imsu.imWords.GetLength();
		msg->cbSize=sizeof(MSG_IM_C_ADDREQUEST)-sizeof(msg->sRequestNotes)+msg->nMsgLength+1;//实际长度

		//MSG_IM_C_ADDREQUEST_Game t;
		//t.dwUserID1 = GetMeUserInfo()->dwUserID;
		//t.dwUserID = m_dwCurrentUserID;
		m_pGameRoom->PostMessage(WM_FRIEND_R, sizeof(MSG_IM_C_ADDREQUEST), LPARAM(msg));
		return;
	}
}

void CLoveSendClassInExe::OnGetFriendList(void* pBuff, int nSize)
{
	MSG_IM_C_GETFRIENDLIST *msg = (MSG_IM_C_GETFRIENDLIST*)pBuff;

	if(nSize == 0)
	{
		m_FriendList.erase(m_FriendList.begin(), m_FriendList.end());
	}
	else if(nSize == sizeof(MSG_IM_C_GETFRIENDLIST))
	{
		m_FriendList.push_back(*msg);
	}
	return;
}

// duanxiaohui 2011-11-11 加入黑名单
void CLoveSendClassInExe::OnAddEmeny(bool bIsHave)
{
	//处理数据
	UserItemStruct * pUserItem = FindOnLineUser(m_dwCurrentUserID);
	if (pUserItem == NULL)
	{	
		return;
	}

	MSG_IM_C_SETGROUP * msg = new MSG_IM_C_SETGROUP;
	ZeroMemory(msg, sizeof(MSG_IM_C_SETGROUP));

	msg->dwUserID = GetMeUserInfo()->dwUserID;
	msg->dwRemoteUserID = m_dwCurrentUserID;
	msg->groupID = 3;   // 组ID，1-好友，3-黑名单,0-删除

	m_pGameRoom->PostMessage(WM_EMENY_R, sizeof(MSG_IM_C_SETGROUP), LPARAM(msg));
	return;
}

//查看用户信息
BOOL CLoveSendClassInExe::OnCmdShowUserInfo()
{
	if (m_dwCurrentUserID != 0L)
	{
		UserItemStruct* pUserInfo = FindOnLineUser(m_dwCurrentUserID);

		m_pUserDetailsDlg->SetUserItem(pUserInfo);

		POINT point;
		::GetCursorPos(&point);

		CRect rect;
		m_pUserDetailsDlg->GetClientRect(&rect);

		if (m_pRightFrame)
		{
			RECT rcRightWnd;
			::GetWindowRect(m_pRightFrame->GetHWND(),&rcRightWnd);
			m_pUserDetailsDlg->MoveWindow(rcRightWnd.left - rect.Width(), point.y - 20, rect.Width(), rect.Height());
		}
		else
		{
			CRect listRect;
			m_UserListDlg.GetWindowRect(&listRect);
			m_pUserDetailsDlg->MoveWindow(listRect.left - rect.Width() + 15, point.y - 10, rect.Width(), rect.Height());
		}

		m_pUserDetailsDlg->ShowWindow(SW_SHOW);
	}

	return TRUE;
}

//复制用户信息
BOOL CLoveSendClassInExe::OnCmdCopyUserInfo()
{
	if (m_dwCurrentUserID != 0)
	{
		UserItemStruct* pUserItem = FindOnLineUser(m_dwCurrentUserID);

		//处理数据
		if ((pUserItem==NULL)||(OpenClipboard()==FALSE))
			return TRUE;
		if (EmptyClipboard()==FALSE) 
		{
			CloseClipboard();
			return TRUE;
		}

		//复制数据
		HANDLE hData=GlobalAlloc(GMEM_MOVEABLE,sizeof(pUserItem->GameUserInfo.nickName));
		if (hData==NULL) 
		{
			CloseClipboard();
			return TRUE;
		}

		LPTSTR szMemName=(LPTSTR)GlobalLock(hData);

		// duanxiaohui 2011-11-21 复制到粘贴板中需要转换成Unicode
		//lstrcpy(szMemName,pUserItem->GameUserInfo.nickName);

		CString strText;
		strText.Format("%s", pUserItem->GameUserInfo.nickName);
		if (0 == MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)strText, strText.GetLength() + 1, (WCHAR*)szMemName,  1024)) 
		{ 
			GlobalUnlock(hData); 
			CloseClipboard(); 
			GlobalFree(hData); 
			return TRUE; 
		} 
		// end duanxiaohui

		SetClipboardData(CF_UNICODETEXT,hData);
		GlobalUnlock(hData); 
		GlobalFree(hData);
		CloseClipboard();

		//显示名字
		CString strBuffer;
		m_InputMessage.GetWindowText(strBuffer);
		strBuffer+=pUserItem->GameUserInfo.nickName;
		m_InputMessage.SetWindowText(strBuffer);
		m_InputMessage.SetFocus();
	}

	return TRUE; 
}

//踢人
BOOL CLoveSendClassInExe::OnCmdKickUser()
{
	if (m_dwCurrentUserID!=0L)
	{
		//wushuqun 2009.5.19
		//旁观时不能T人
		if (GetMeUserInfo()->bUserState == USER_WATCH_GAME)
		{
			AFCMessageBox("对不起，您正在旁观游戏，不能进行此操作", "提示");
			return true;
		}

		if(GetMeUserInfo()->dwUserID == m_dwCurrentUserID)
		{
			AFCMessageBox("对不起,无法对自己进行此项操作","提示");
			return true;
		}

		CString stime;
		stime.Format("%d",CTime::GetCurrentTime());        //当前的时间，String格式
		int curtime=atoi(stime);                        //转换为int
		if(GetMeUserInfo()->iVipTime < curtime)
		{
			AFCMessageBox("对不起,只有vip才有此项功能","提示");
			return true;
		}
		///检查是否为旁观 add by rmaon 09.04.27
		if( m_bWatchMode )
		{
			AFCMessageBox("对不起,旁观状态不能踢人","提示");
			return true;
		}
		//处理数据
		UserItemStruct * pUserItem=FindOnLineUser(m_dwCurrentUserID);
		if (pUserItem==NULL) 
			return TRUE;
		if(pUserItem->GameUserInfo.iVipTime>curtime)
		{
			AFCMessageBox("对不起,对方是Vip,踢人失败!","提示");
			return TRUE;
		}

		// PengJiLin, 2010-9-10, 可配置是否有踢人卡的踢人功能
		CString sPath=CBcfFile::GetAppPath();
		CBcfFile bcfFile( sPath + "..\\Order.bcf");
		int iValue = bcfFile.GetKeyVal(_T("ClassInExe"), _T("UseKickUserProp"), 0);
		if(iValue > 0)      // 如果有踢人卡、防踢卡功能，使用新的踢人功能
		{
			_TAG_NEW_KICKUSER stKick;
			stKick.dwUserID = GetMeUserInfo()->dwUserID;
			stKick.dwDestID = m_dwCurrentUserID;
			SendGameData(&stKick,sizeof(stKick),MDM_GR_PROP,ASS_PROP_NEW_KICKUSER,0);
		}
		else
		{
			_TAG_KICKUSER stKick;
			stKick.dwUserID = GetMeUserInfo()->dwUserID;
			stKick.dwDestID = m_dwCurrentUserID;
			SendGameData(&stKick,sizeof(stKick),MDM_GR_PROP,ASS_PROP_KICKUSER,0);
		}
	}

	return TRUE;
}

//踢人
BOOL CLoveSendClassInExe::OnCmdKickUserProp()
{
	if (m_dwCurrentUserID!=0L)
	{
		// 旁观时不能T人
		if (GetMeUserInfo()->bUserState == USER_WATCH_GAME)
		{
			AFCMessageBox("对不起，您正在旁观游戏，不能进行此操作", "提示");
			return true;
		}
		// 检查是否为旁观
		if( m_bWatchMode )
		{
			AFCMessageBox("对不起,旁观状态不能踢人","提示");
			return true;
		}
		if(GetMeUserInfo()->dwUserID == m_dwCurrentUserID)
		{
			AFCMessageBox("对不起,无法对自己进行此项操作","提示");
			return true;
		}

		CString stime;
		stime.Format("%d",CTime::GetCurrentTime());         // 当前的时间，String格式
		int curtime=atoi(stime);                            // 转换为int
		UserItemStruct * pUserItem=FindOnLineUser(m_dwCurrentUserID);
		if (pUserItem==NULL) 
		{
			AFCMessageBox("对不起,对方状态异常,踢人失败!","提示");
			return TRUE;
		}
		if(pUserItem->GameUserInfo.iVipTime>curtime)
		{
			AFCMessageBox("对不起,对方也是Vip,踢人失败!","提示");
			return TRUE;
		}

		// 道具数据分析、比较放到存储过程中实现
		_TAG_NEW_KICKUSER stKick;
		stKick.dwUserID = GetMeUserInfo()->dwUserID;
		stKick.dwDestID = m_dwCurrentUserID;
		SendGameData(&stKick,sizeof(stKick),MDM_GR_PROP,ASS_PROP_NEW_KICKUSER,0);
	}

	return TRUE;
}

//拍砖头
BOOL CLoveSendClassInExe::OnCmdBlickProp()
{
	
	if (m_dwCurrentUserID!=0L)
	{
		// 旁观时不能T人
		if (GetMeUserInfo()->bUserState == USER_WATCH_GAME)
		{
			::DUIMessageBox(NULL,MB_OK|MB_ICONINFORMATION,m_pGameInfo->szGameName,"对不起，您正在旁观游戏，不能进行此操作");
			return true;
		}
		// 检查是否为旁观
		if( m_bWatchMode )
		{
			::DUIMessageBox(NULL,MB_OK|MB_ICONINFORMATION,m_pGameInfo->szGameName,"对不起，您正在旁观游戏，不能进行此操作");
			return true;
		}
		if(GetMeUserInfo()->dwUserID == m_dwCurrentUserID)
		{
			::DUIMessageBox(NULL,MB_OK|MB_ICONINFORMATION,m_pGameInfo->szGameName,"对不起,无法对自己进行此项操作");
			return true;
		}

		int nPropIndex = 0;

		_TAG_USERPROP * userProp = NULL;

		int propCount = Glb().userPropLibrary.GetCount();
		for(int i = 0; i < propCount; ++i)
		{
			userProp = Glb().userPropLibrary.GetAt(i);
			if(strcmp(userProp->szPropName,"拍砖头") == 0)
			{
				nPropIndex = i;
				break;
			}
		}

		userProp = Glb().userPropLibrary.GetAt(nPropIndex);

		if (NULL == userProp)
		{
			return FALSE;
		}

		if(userProp->nHoldCount<=0)
		{
			::DUIMessageBox(NULL,MB_OK|MB_ICONINFORMATION,m_pGameInfo->szGameName,"对不起,您已没有拍砖头道具，请购买。");
			return true;
		}

		_TAG_USINGPROP  usingProp;
		ZeroMemory(&usingProp,sizeof(_TAG_USINGPROP));
		usingProp.dwUserID = m_pGameInfo->uisMeUserInfo.GameUserInfo.dwUserID;
		usingProp.dwTargetUserID = m_dwCurrentUserID;
		usingProp.nPropID = userProp->nPropID;
		usingProp.nPropActionAttrib=userProp->attribAction;
		usingProp.nPropValueAttrib=userProp->attribValue;

		memcpy(usingProp.szPropName,userProp->szPropName,sizeof(userProp->szPropName));
		SendGameData(&usingProp,sizeof(_TAG_USINGPROP),MDM_GR_PROP,ASS_PROP_USEPROP,0);
	}

	return TRUE;
	
}
//赠送鲜花
BOOL CLoveSendClassInExe::OnCmdFlowerProp()
{
	if (m_dwCurrentUserID!=0L)
	{
		// 旁观时不能T人
		if (GetMeUserInfo()->bUserState == USER_WATCH_GAME)
		{
			::DUIMessageBox(NULL,MB_OK|MB_ICONINFORMATION,m_pGameInfo->szGameName,"对不起，您正在旁观游戏，不能进行此操作");
			return true;
		}
		// 检查是否为旁观
		if( m_bWatchMode )
		{
			::DUIMessageBox(NULL,MB_OK|MB_ICONINFORMATION,m_pGameInfo->szGameName,"对不起，您正在旁观游戏，不能进行此操作");
			return true;
		}
		if(GetMeUserInfo()->dwUserID == m_dwCurrentUserID)
		{
			::DUIMessageBox(NULL,MB_OK|MB_ICONINFORMATION,m_pGameInfo->szGameName,"对不起,无法对自己进行此项操作");
			return true;
		}

		int nPropIndex = 0;

		_TAG_USERPROP * userProp = NULL;
		
		int propCount = Glb().userPropLibrary.GetCount();
		for(int i = 0; i < propCount; ++i)
		{
			userProp = Glb().userPropLibrary.GetAt(i);
			if(strcmp(userProp->szPropName,"送鲜花") == 0)
			{
				nPropIndex = i;
				break;
			}
		}

		userProp = Glb().userPropLibrary.GetAt(nPropIndex);

		if (NULL == userProp)
		{
			return FALSE;
		}

		if(userProp->nHoldCount<=0)
		{
			::DUIMessageBox(NULL,MB_OK|MB_ICONINFORMATION,m_pGameInfo->szGameName,"对不起,您已没有送鲜花道具，请购买。");
			return true;
		}

		_TAG_USINGPROP  usingProp;
		ZeroMemory(&usingProp,sizeof(_TAG_USINGPROP));
		usingProp.dwUserID = m_pGameInfo->uisMeUserInfo.GameUserInfo.dwUserID;
		usingProp.dwTargetUserID = m_dwCurrentUserID;
		usingProp.nPropID = userProp->nPropID;
		usingProp.nPropActionAttrib=userProp->attribAction;
		usingProp.nPropValueAttrib=userProp->attribValue;

		memcpy(usingProp.szPropName,userProp->szPropName,sizeof(userProp->szPropName));
		SendGameData(&usingProp,sizeof(_TAG_USINGPROP),MDM_GR_PROP,ASS_PROP_USEPROP,0);
	}

	return TRUE;
}

//处理道具消息
bool CLoveSendClassInExe::OnUsePropResult(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize)
{
	if(pNetHead->bHandleCode!=DTK_GR_PROP_USE_SUCCEED)
	{
		switch(pNetHead->bHandleCode)
		{
		case DTK_GP_PROP_GIVE_NOTARGETUSER:
			ShowMsgInGame("目标玩家不处于在线状态！");
			break;
		default:
			break;
		}
		return true;
	}
	if(sizeof(_TAG_USINGPROP)!= uDataSize)
		return false;
	_TAG_USINGPROP * usingProp = (_TAG_USINGPROP *)pNetData;
	UserItemStruct *pUserInfo =FindOnLineUser(usingProp->dwTargetUserID);
	UserItemStruct *pUserInfoUsed =FindOnLineUser(usingProp->dwUserID);
	if(pUserInfo == NULL || pUserInfoUsed == NULL)
		return true;
	// 如果是自己使用的道具，要扣除道具数量
	if (usingProp->dwUserID == GetMeUserInfo()->dwUserID)
	{
		_TAG_USERPROP *userProp = NULL;
		for (int i=0; i<Glb().userPropLibrary.GetCount(); ++i)
		{
			userProp=Glb().userPropLibrary.GetAt(i);
			if ((userProp!=NULL) && (userProp->nPropID == usingProp->nPropID))
			{
				break;
			}
		}
		if((userProp!=NULL) && (userProp->nHoldCount<=0))
		{
			userProp->nHoldCount-=1;
            if(userProp->nHoldCount < 0)userProp->nHoldCount = 0;
		}
	}
	
	//使用者的昵称
	CString name1=pUserInfoUsed->GameUserInfo.nickName;// rewrite by ramon
	//被使用者的昵称
	CString name2=pUserInfo->GameUserInfo.nickName;	// rewrite by ramon

	if(usingProp->dwUserID==m_pGameInfo->uisMeUserInfo.GameUserInfo.dwUserID)
		name1="您";
	if(usingProp->dwTargetUserID==m_pGameInfo->uisMeUserInfo.GameUserInfo.dwUserID)
		name2="您";
	CString name3=name2;
	if(usingProp->dwUserID==usingProp->dwTargetUserID)
		name2="自己";

	int attribAction=usingProp->nPropActionAttrib;
	int attribValue=usingProp->nPropValueAttrib;

	CString stip;
	stip.Format("%s 对 %s 使用了 %s 道具",name1,name2,usingProp->szPropName);
	m_MessageHandle.InsertSystemMessageM(stip.GetBuffer());

	CString _s = "B";
	if (usingProp->dwTargetUserID==m_pGameInfo->uisMeUserInfo.GameUserInfo.dwUserID)
	{
		_s = "B";
	}
	if (usingProp->dwUserID==m_pGameInfo->uisMeUserInfo.GameUserInfo.dwUserID)
	{
		_s = "A";
	}
	if (_s != "" && pUserInfo->GameUserInfo.bDeskNO == m_pGameInfo->uisMeUserInfo.GameUserInfo.bDeskNO)
	{
		CString soundFile;
		soundFile.Format("..\\image\\prop\\Prop%d%s.mp3", usingProp->nPropID, _s);
		BZSoundPlay(this,soundFile.GetBuffer(),0,1);
	}
	
	//if(m_hWnd)
	//	Invalidate(FALSE);

	//双倍积分
	if(attribAction & 0x4)
	{
		pUserInfo->GameUserInfo.iDoublePointTime=usingProp->iDoubleTime;
		if(usingProp->dwTargetUserID==m_pGameInfo->uisMeUserInfo.GameUserInfo.dwUserID)
		{
			time_t t=(time_t)usingProp->iDoubleTime;
			CTime expiresTime(t);
			stip.Format(TEXT("您的双倍积分功能可以使用到 %s！"),expiresTime.Format("%Y-%m-%d %H:%M:%S"));
			m_MessageHandle.InsertSystemMessageM(stip.GetBuffer());
		}
	}
	if(attribAction & 0x8)
	{
		pUserInfo->GameUserInfo.iProtectTime=usingProp->iProtectTime;
		if(usingProp->dwUserID == m_pGameInfo->uisMeUserInfo.GameUserInfo.dwUserID)
		{
			time_t t=(time_t)usingProp->iProtectTime;
			CTime expiresTime(t);
			stip.Format(TEXT("您的护身符功能(输了不扣积分)可以使用到 %s！"),expiresTime.Format("%Y-%m-%d %H:%M:%S"));
			m_MessageHandle.InsertSystemMessageM(stip.GetBuffer());
		}
	}

	if(attribAction & 0x10)
	{
		int fasciChange=(attribValue & 0xFF0000)>>16;
		stip.Format("%s 的魅力值增加 %d 点",name3,fasciChange);
		if(attribAction & 0x20)
		{
			stip.Format("%s 的魅力值减少 %d 点",name3,fasciChange);
			fasciChange=-fasciChange;
		}
		//	pUserInfo->GameUserInfo.dwFascination+=fasciChange;
		m_MessageHandle.InsertSystemMessageM(stip.GetBuffer());
		m_UserListDlg.Invalidate(FALSE);
	}
	if(attribAction & 0x2)
	{
		CString str;
		str.Format("负分清0道具结果:%d 负分清0",usingProp->dwTargetUserID);
		//OutputDebugString(str);
		if(usingProp->dwTargetUserID==m_pGameInfo->uisMeUserInfo.GameUserInfo.dwUserID)
		{
			if(m_pGameInfo->uisMeUserInfo.GameUserInfo.dwPoint<0)
				m_pGameInfo->uisMeUserInfo.GameUserInfo.dwPoint=0;
		}
		if (pUserInfoUsed->GameUserInfo.dwPoint<0)
		{
			pUserInfoUsed->GameUserInfo.dwPoint = 0;
		}
		if (pUserInfoUsed->GameUserInfo.dwUserID == m_CurpUserInfoClicked->dwUserID)
		{
			NavigateVirFace();
		}
	}

	this->m_pGameRoom->PostMessage(WM_ACTIVE_TO_ROOM, 1, 1);
	//更新玩家列表
	bool bRet = m_UserListDlg.UpdateGameUser(pUserInfo);
	//OnSetPropImages();
	//m_BtPropGoAhead.Invalidate(FALSE);
	//m_BtPropGoNext.Invalidate(FALSE);
	//m_BtProp1.Invalidate(FALSE);
	//m_BtProp2.Invalidate(FALSE);
	//m_BtProp3.Invalidate(FALSE);
	//m_BtProp4.Invalidate(FALSE);
//	this->Invalidate(FALSE);
	return true;
}
//vip时间检测
bool CLoveSendClassInExe::OnPropVipCheckTime(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize)
{
	if(uDataSize!=sizeof(_TAG_PROP_TIME))
		return false;
	_TAG_PROP_TIME * pData = (_TAG_PROP_TIME *)pNetData;
	CString stime;
	stime.Format("%d",CTime::GetCurrentTime());        //当前的时间，String格式
	int curtime=atoi(stime);                        //转换为int

	if(GetMeUserInfo()->iVipTime-curtime < 10*60 && GetMeUserInfo()->iVipTime > curtime)
	{
		//处理数据
		CString strMessage;
		strMessage.Format(TEXT("您的Vip身份将于 %d 分钟后结束!"),GetMeUserInfo()->iVipTime/60);
		m_MessageHandle.InsertSystemMessage(strMessage.GetBuffer());
	}
	else
	{
		if(GetMeUserInfo()->iVipTime <=curtime)
		{
			//处理数据
			CString strMessage;
			strMessage.Format(TEXT("您的Vip身份已经结束，请您重新购买！"));
			m_MessageHandle.InsertSystemMessage(strMessage.GetBuffer());
		}
	}
	//OutputDebugString("OnPropVipCheckTime");
	OnSetPropImages();
	return true;
}

// PengJiLin, 2010-9-14, 使用踢人卡踢人结果消息
bool CLoveSendClassInExe::OnNewKickUserResult(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize)
{
    if(uDataSize!=sizeof(_TAG_NEW_KICKUSER)) return false;

    _TAG_NEW_KICKUSER* pKickResult = (_TAG_NEW_KICKUSER*)pNetData;
    if(pKickResult != NULL && pKickResult->dwUserID == m_pGameInfo->uisMeUserInfo.GameUserInfo.dwUserID)   // 自己踢别人
    {
        switch(pNetHead->bHandleCode)
        {
        case ERR_GR_NEW_KICKUSER_SUCCESS:       // 成功
            {
                UserItemStruct* pUserInfo = NULL;
                for (int i = 0; i < MAX_PEOPLE; ++i)
                {
                    pUserInfo = m_pUserInfo[i];
                    if(pUserInfo != NULL && pUserInfo->GameUserInfo.dwUserID == pKickResult->dwDestID)
                    {
                        break;
                    }
                    pUserInfo = NULL;
                }
                CString strInfo = "";
                if(NULL != pUserInfo)
				{
					strInfo.Format("玩家%s已被踢出游戏桌", pUserInfo->GameUserInfo.nickName);
				}
                else
				{
					strInfo.Format("玩家%d已被踢出游戏桌", pKickResult->dwDestID);
				}
                
                //ShowNewKickPropDlg(TRUE, FALSE, TRUE, strInfo);
				m_MessageHandle.InsertSystemMessage(strInfo.GetBuffer());
            }
            break;
        case ERR_GR_NEW_KICKUSER_NOTIME:        // 自己的道具已过期，或者没有道具
            {
                CString strInfo = "";
                strInfo.Format("您没有\"踢人卡\"道具，或者已经过期");
                //ShowNewKickPropDlg(TRUE, TRUE, TRUE, strInfo);
				m_MessageHandle.InsertSystemMessage(strInfo.GetBuffer());
            }
            break;
        case ERR_GR_NEW_KICKUSER_HAD_ANTI:      // 对方有防踢卡时间
            {
                CString strInfo = "";
                strInfo.Format("该玩家使用了\"防踢卡\"，您不能将他踢出游戏桌");
                //ShowNewKickPropDlg(TRUE, FALSE, TRUE, strInfo);
				m_MessageHandle.InsertSystemMessage(strInfo.GetBuffer());
            }
            break;
        case ERR_GR_NEW_KICKUSER_HAD_VIP:       // 对方有VIP时间
            {
                CString strInfo = "";
                strInfo.Format("该玩家是VIP会员，您不能将他踢出游戏桌");
                //ShowNewKickPropDlg(TRUE, FALSE, TRUE, strInfo);
				m_MessageHandle.InsertSystemMessage(strInfo.GetBuffer());
            }
            break;
        case ERR_GR_NEW_KICKUSER_PLAYING:       // 游戏中不踢人
            m_MessageHandle.InsertSystemMessage("操作失败，对方正在游戏");
            break;
        default:
            break;
        }
    }
	return true;
}

//关闭银行
void CLoveSendClassInExe::OnCloseBank2()
{
	if (m_pBank2Wnd)
	{	
		delete m_pBank2Wnd ;
		m_pBank2Wnd = NULL;
		//不关闭，只隐藏
		//m_pBank2Wnd->ShowWindow(SW_HIDE);
	}

	return;
}

//发送游戏房间消息
void CLoveSendClassInExe::ShowMsgInGame(TCHAR * szMsg, BYTE bNewOrSys,UINT uSize, TCHAR * szFontName,TCHAR *Title)
{
	if (NULL == szMsg)
	{
		SetForegroundWindow();
		//OnBnClickedMax();
	}
	else
	{
		m_MessageHandle.InsertTitleMessage(szMsg,bNewOrSys,uSize,szFontName,Title);
	}
}

//desc:获取游戏编号
//remark:百家乐 add by wlr 20090716
void CLoveSendClassInExe::GetGameCode(TCHAR * szGameCode)
{
	::strcpy(szGameCode,"00000000");
}
//end of  add by wlr 20090716

///判断是否是排队机房间
///@return true:是排队机房间；false：不是排队机房间
bool CLoveSendClassInExe::IsQueueGameRoom() const
{
	return(((m_pGameInfo->dwRoomRule & GRR_QUEUE_GAME) > 0) ? true : false);
}

//发送嘟嘟消息
void CLoveSendClassInExe::ShowMsgDudu(TCHAR * szSendName, TCHAR * szTargetName,TCHAR * szCharString, COLORREF crTextColor, bool bShowTime, UINT uSize, TCHAR * szFontName,int iDuduType)
{
	//wushuqun 2009.5.27
	//处理大喇叭消息
// 	if (strcmp(szSendName ,this->GetMeUserInfo()->nickName) == 0)
// 	{
// 		//OutputDebugString("ShowMsgDudu");
// 		this->OnSetPropImages();
// 	}
	m_MessageHandle.InsertDudu(szSendName,szTargetName,szCharString,crTextColor,false,0,0,iDuduType);
}

void CLoveSendClassInExe::OnCbnSelchangeInput()
{
	SetTimer(1,10,NULL);
}

void CLoveSendClassInExe::OnBnClickedSend()
{
	SendTalkMessage();
	return;
}

void CLoveSendClassInExe::OnBnClickedSetcolor()
{
	// TODO: 在此添加控件通知处理程序代码
}

void CLoveSendClassInExe::OnBnClickedExpression()
{
	//建立表情窗口
	if (m_ExpressWnd.GetSafeHwnd()==NULL) m_ExpressWnd.CreateExpressionWnd(this);
	
	//移动窗口
	CRect ButtonRect;
	CSize ExpressSize;
	CRect ClientRect;
	GetClientRect(&ClientRect);
	m_ExpressWnd.GetFixSize(ExpressSize);
	m_BtExpression.GetWindowRect(&ButtonRect);
	SafeMoveWindow(&m_ExpressWnd,ButtonRect.right-ExpressSize.cx+176,ButtonRect.top-ExpressSize.cy-3,ExpressSize.cx,ExpressSize.cy);
	//SafeMoveWindow(&m_ExpressWnd,min(ButtonRect.right,max(0,ClientRect.right-ExpressSize.cx)) + 320,ButtonRect.top-ExpressSize.cy,ExpressSize.cx,ExpressSize.cy);
	m_ExpressWnd.ShowWindow(SW_SHOW);
	m_ExpressWnd.SetFocus();

	return;
}

void CLoveSendClassInExe::OnBnClickedGoAhead()
{
	//OutputDebugString("OnBnClickedGoAhead");
	m_curPropPage--;
	OnSetPropImages();
	// TODO: 在此添加控件通知处理程序代码
}

// PengJiLin, 2010-10-12, 商场道具功能
void CLoveSendClassInExe::OnBnClickedGoAheadS()
{
	m_curPropPage--;
	OnSetPropImages();
	if (m_pPropPannel != NULL)
	{
		m_pPropPannel->SetFocus();
	}
}

void CLoveSendClassInExe::OnBnClickedGoNext()
{
	//OutputDebugString("点了道具向下按钮");
	m_curPropPage++;
	OnSetPropImages();

	//OutputDebugString("OnBnClickedGoNext");
	// TODO: 在此添加控件通知处理程序代码
}

void CLoveSendClassInExe::OnBnClickedGoNextS()
{
	m_curPropPage++;
	OnSetPropImages();
	if (m_pPropPannel != NULL)
	{
		m_pPropPannel->SetFocus();
	}
}

// PengJiLin, 2010-9-30, 蓝钻贵族
void CLoveSendClassInExe::OnBnClickedBlueDiamond()
{
    //屏蔽百家乐相关功能 add by wlr 20090716
    char GameCode[20];
    ::memset(GameCode,0,sizeof(GameCode));
    GetGameCode(GameCode);
    CString str = GameCode;
    //if (str == "11901800"
    //	|| str == "10901800"
    //	|| str == "30501800")
    TCHAR szKey[10];
    CString sPath=CBcfFile::GetAppPath();
    CBcfFile fsr( sPath + "..\\SpecialRule.bcf");
    wsprintf(szKey, "%d", m_pGameInfo->uNameID);
    int iResult = fsr.GetKeyVal (_T("BJLType"), szKey, 0);

    if (iResult>0)
        return ;
    //end of add by wlr 20090716

    // TODO: 在此添加控件通知处理程序代码
    CString s=CBcfFile::GetAppPath () + "..\\";
    CBcfFile f( s + "bzgame.bcf");
    CString url;
    url=f.GetKeyVal("BZW","gameurl5","http://www.kfgame.com");

    //CString surl;
    //surl.Format("%s/app/userlogin.asp?userid=%d&token=%s&url=%s",
    //	Glb().m_CenterServerPara.m_strWebRootADDR,m_pGameInfo->uisMeUserInfo.GameUserInfo.dwUserID,Glb().m_TML_SN,url);
    //surl.Format("%s/app/userlogin.asp?userid=%d&token=",
    //            url,m_pGameInfo->uisMeUserInfo.GameUserInfo.dwUserID);
    //ShellExecute(NULL,"open",surl,NULL,NULL,SW_MAXIMIZE|SW_SHOW);
    ShellExecute(NULL,"open",url,NULL,NULL,SW_MAXIMIZE|SW_SHOW);
    return ;
}



void CLoveSendClassInExe::OnClose()
{
	// 通知平台，已经退出
	//OnBnClickedExit();
	CLoadFileBmp::OnClose();
}

void CLoveSendClassInExe::OnCancel()
{
	if(m_pPersonBank != NULL)
	{
		if(m_pPersonBank->GetSafeHwnd()!=NULL)
			m_pPersonBank->EndDialog(0);
		delete m_pPersonBank;
		m_pPersonBank = NULL;
	}
	//OutputDebugString("[aa]SDK CLoveSendClassInExe::OnCancel() begin 0");
	// TODO: 在此添加专用代码和/或调用基类
	m_TalkMessage.DestroyWindow();
	CWnd * winApp=AfxGetApp()->m_pMainWnd;
	if(winApp && winApp->m_hWnd){
		winApp->PostMessage(WM_GAME_QUIT,0,0);
		//OutputDebugString("[aa]SDK CLoveSendClassInExe::OnCancel() end 0");
	}

	//还原窗口的分辨率
	if (m_bShowMax)
	{

		CBcfFile f(CBcfFile::GetAppPath() + _T("GameOption.bcf"));
		UINT uMaxType = f.GetKeyVal("FrameControl","max_type",1);

		if (uMaxType == 2) //修改分辨率
		{
			DEVMODE lpDevMode;
			lpDevMode.dmBitsPerPel=32;
			lpDevMode.dmPelsWidth = m_nWiondwMetricsX;
			lpDevMode.dmPelsHeight = m_nWiondwMetricsY;
			lpDevMode.dmSize=sizeof(lpDevMode);
			lpDevMode.dmFields = DM_PELSWIDTH|DM_PELSHEIGHT|DM_BITSPERPEL;
			LONG result;
			result=ChangeDisplaySettings(&lpDevMode,0);
			if (result == DISP_CHANGE_SUCCESSFUL)
			{
				ChangeDisplaySettings(&lpDevMode,CDS_UPDATEREGISTRY);
				// 　　//使用CDS_UPDATEREGISTRY表示次修改是持久的，
				// 　　//并在注册表中写入了相关的数据 
			}
			else
			{
				ChangeDisplaySettings(NULL,0);
			}

			//通知大厅UI作主相应调整
			this->m_pGameRoom->PostMessage(WM_SCREEN_SIZE_CHANGE,0,2);
		}
		
	}
}
/************************************************************************/
//Function:checkNetSignal()
//Parameter:none
//Author:Fred Huang 2008-06-27
//Note:向服务器发送网络检测消息
/************************************************************************/
void CLoveSendClassInExe::checkNetSignal(void)
{
	if(m_pGameInfo->uisMeUserInfo.GameUserInfo.dwUserID==0)        //没有ID号，不发
		return;
	//如果上次的消息都没有收到，则网格极差，但消息还是要发
	if(m_NetSignalIndex>m_SavedSingalIndex)
	{
		m_NetSignalLevel=0;

	}
	drawSignal(&CWindowDC(this));
	//这个是不需要发消息的

}

/************************************************************************/
//Function:checkNetSignal()
//Parameter:
//                pNetData        :服务器反回的消息体
//Author:Fred Huang 2008-06-27
//Note:解析服务器返回的消息
/************************************************************************/
void CLoveSendClassInExe::getNetSignal(void * pNetData)
{
	MSG_GP_Game_NETSignal *netSignal=(MSG_GP_Game_NETSignal*)pNetData;
	//如果不是发给自己的，丢弃
	if(m_pGameInfo->uisMeUserInfo.GameUserInfo.dwUserID!=netSignal->dwUserID)
		return;
	//先将级别置0
	m_NetSignalLevel=0;
	//计算时差
	UINT uTimeDiff=GetTickCount()-netSignal->dwTimeFlag;

	m_NetSignalIndex = netSignal->dwSignalIndex;
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
	drawSignal(&CWindowDC(this));
	//Invalidate(true);
}

/************************************************************************/
//Function:drawSignal()
//Parameter:无
//Author:Fred Huang 2008-06-27
//Note:画图
/************************************************************************/
void CLoveSendClassInExe::drawSignal(CDC * pDc)
{
	if (NULL == pDc || NULL == pDc->m_hDC )
		return;


	if(!pSingnalimg || pSingnalimg->GetFlags()==0)
		return;
	CString s="..\\";// CINIFile::GetAppPath();

	// ----- 调用指定游戏的配置文件 -------- dxh
	char GameCode[20];
	::memset(GameCode,0,sizeof(GameCode));
	GetGameCode(GameCode);
	//if (str == "11901800"
	//	|| str == "10901800"
	//	|| str == "30501800")
	TCHAR szKey[10];
	CString sPath=CBcfFile::GetAppPath();
	CBcfFile fsr( sPath + "..\\SpecialRule.bcf");
	wsprintf(szKey, "%d", m_pGameInfo->uNameID);
	int iResult = fsr.GetKeyVal (_T("BJLType"), szKey, 0);

	if (iResult>0)
	{
		s = ".\\config\\";  ///<add by wxx 2010.7.1
	}
	//else
	//{
	//	s="..//config//"; ///<add by wxx 2010.07.14
	//	//return;
	//}
	// ----- 调用指定游戏的配置文件 --------

	CBcfFile f( s + "GameOption.bcf");

	CString key= m_skinmgr.GetKeyVal(m_skinmgr.GetSkinBcfFileName());
	CRect rc;
	GetClientRect(rc);
	int x = rc.Width() - f.GetKeyVal(key, _T("game_signalx"), 230) - pSingnalimg->GetWidth();

	Graphics g(this->m_hWnd);
	g.DrawImage(pSingnalimg,Rect(x,f.GetKeyVal(key, _T("game_signaly"),3),pSingnalimg->GetWidth(),pSingnalimg->GetHeight()/6),0,m_NetSignalLevel*pSingnalimg->GetHeight()/6,pSingnalimg->GetWidth(),pSingnalimg->GetHeight()/6,UnitPixel);
}
void CLoveSendClassInExe::OnSndPlay()
{
	m_bSndPause=false;
	BZSoundContinue(m_sndListID);
	//m_btSndPlay.ShowWindow(SW_HIDE);
	//m_btSndPause.ShowWindow(SW_SHOW);

	TCHAR sz[MAX_PATH] = {0};
	TCHAR szPath[MAX_PATH] = {0};
	lstrcpy(sz,m_pGameInfo->szProcessName);
	sz[lstrlen(sz)-lstrlen(".ico")]='\0';
	wsprintf(szPath,"Music\\%s.bcf",sz,sz);
	CBcfFile f(szPath);
	f.SetKeyValString("BGSOUND","PauseSound","0");

}
void CLoveSendClassInExe::OnSndPause()
{
	m_bSndPause=true;
	BZSoundPause(m_sndListID);
	//m_btSndPlay.ShowWindow(SW_SHOW);
	//m_btSndPause.ShowWindow(SW_HIDE);

	TCHAR sz[MAX_PATH] = {0};
	TCHAR szPath[MAX_PATH] = {0};
	lstrcpy(sz,m_pGameInfo->szProcessName);
	sz[lstrlen(sz)-lstrlen(".ico")]='\0';
	wsprintf(szPath,"Music\\%s.bcf",sz,sz);
	CBcfFile f(szPath);
	f.SetKeyValString("BGSOUND","PauseSound","1");
}
void CLoveSendClassInExe::OnSndVolDown()
{
	int nVol=BZSoundGetVolume(m_sndListID);
	BZSoundSetVolume(m_sndListID,nVol-50);

	TCHAR sz[MAX_PATH] = {0};
	TCHAR szPath[MAX_PATH] = {0};
	lstrcpy(sz,m_pGameInfo->szProcessName);
	sz[lstrlen(sz)-lstrlen(".ico")]='\0';
	wsprintf(szPath,"Music\\%s.bcf",sz,sz);
	CBcfFile f(szPath);
	CString sini;
	sini.Format("%d",(nVol-50)>1?nVol-50:1);
	f.SetKeyValString("BGSOUND","SoundVolume",sini);

}
void CLoveSendClassInExe::OnSndVolUp()
{
	int nVol=BZSoundGetVolume(m_sndListID);
	BZSoundSetVolume(m_sndListID,nVol+50);

	TCHAR sz[MAX_PATH] = {0};
	TCHAR szPath[MAX_PATH] = {0};
	lstrcpy(sz,m_pGameInfo->szProcessName);
	sz[lstrlen(sz)-lstrlen(".ico")]='\0';
	wsprintf(szPath,"Music\\%s.bcf",sz,sz);
	CBcfFile f(szPath);
	CString sini;
	sini.Format("%d",(nVol+50)<1000?nVol+50:1000);
	f.SetKeyValString("BGSOUND","SoundVolume",sini);
}

// PengJiLin, 2010-6-1, 增加 记牌器 接口
void CLoveSendClassInExe::UseLookCardProp(_TAG_USERPROP* userProp)
{
    return;
}

// PengJiLin, 2010-9-9, 踢人卡与防踢卡功能接口
void CLoveSendClassInExe::UseKickUserProp(_TAG_USERPROP* userProp)
{
    m_MessageHandle.InsertSystemMessage(TEXT("您正在使用“踢人卡”道具。"));    // test

    MSG_GR_RS_KickProp KickPro;
    ZeroMemory(&KickPro,sizeof(MSG_GR_RS_KickProp));
    KickPro.dwUserID = GetMeUserInfo()->dwUserID;
    KickPro.iPropID = userProp->nPropID;

    SendGameData(&KickPro, sizeof(KickPro), MDM_GM_GAME_FRAME, ASS_GM_USE_KICK_PROP, 0);

    return;
}
void CLoveSendClassInExe::UseAntiKickProp(_TAG_USERPROP* userProp)
{
    m_MessageHandle.InsertSystemMessage(TEXT("您正在使用“防踢卡”道具。"));    // test

    MSG_GR_RS_KickProp KickPro;
    ZeroMemory(&KickPro,sizeof(MSG_GR_RS_KickProp));
    KickPro.dwUserID = GetMeUserInfo()->dwUserID;
    KickPro.iPropID = userProp->nPropID;

    SendGameData(&KickPro, sizeof(KickPro), MDM_GM_GAME_FRAME, ASS_GM_USE_ANTI_KICK_PROP, 0);

    return;
}

// PengJiLin, 2010-9-10, 将秒数格式化为时分秒的格式
void CLoveSendClassInExe::SecondFormat(int iSecond, CString& strOutput)
{
    if(iSecond < 0) iSecond = 0;

    int iSec = 0;
    int iMin = 0;
    int iHour = 0;

    iSec = iSecond%60;
    iMin = iSecond/60;

    iHour = iMin/60;
    iMin = iMin%60;

    if(iHour > 0)
    {
        strOutput.Format("%d小时%d分%d秒", iHour, iMin, iSec);
    }
    else if(iMin > 0)
    {
        strOutput.Format("%d分%d秒", iMin, iSec);
    }
    else
    {
        strOutput.Format("%d秒", iSec);
    }
}

// PengJiLin, 2010-9-10, 踢人卡、防踢卡使用结果
bool CLoveSendClassInExe::OnUseKickPropResult(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, BOOL bIsKick)
{
    MSG_GR_RS_KickProp* pKickProp = (MSG_GR_RS_KickProp*)pNetData;

    // 查找道具，同时减少道具数量
    int propCount=Glb().userPropLibrary.GetCount();
    _TAG_USERPROP * userProp = NULL;
    int iPropValue = 0;
    for(int i = 0; i < propCount; ++i)
    {
        userProp = Glb().userPropLibrary.GetAt(i);
        if(userProp->nPropID == pKickProp->iPropID)
        {
            iPropValue = userProp->attribValue & 0xFFFFFF;
            userProp->nHoldCount -= 1;
            if(userProp->nHoldCount < 0)userProp->nHoldCount = 0;
            //if(userProp->nHoldCount < 1)  // PengJiLin, 2010-10-14, 新的道具系统数量可以为0，屏蔽
            //{
            //    Glb().userPropLibrary.RemoveAt(i);
            //    delete userProp;
            //    userProp=NULL;
            //}
            break;
        }
        userProp = NULL;
    }

    if(TRUE == bIsKick)
    {
        CString strValue = "";
        SecondFormat(iPropValue, strValue);

        CString strTotalValue = "";
        SecondFormat(pKickProp->iTotalTime, strTotalValue);

        CString strInfo = ""; 
        strInfo.Format("您已成功使用“踢人卡”道具，您可以在游戏右侧列表中右键点击目标玩家将其踢出游戏，道具单次有效时间%s，总时间%s", 
                        strValue, strTotalValue);

		m_MessageHandle.InsertSystemMessage(strInfo.GetBuffer());

        //ShowNewKickPropDlg(TRUE, FALSE, TRUE, strInfo);
    }
    else
    {
        CString strValue = "";
        SecondFormat(iPropValue, strValue);

        CString strTotalValue = "";
        SecondFormat(pKickProp->iTotalTime, strTotalValue);

        CString strInfo = ""; 
        strInfo.Format("您已成功使用“防踢卡”道具，其他玩家无法将您踢出游戏，道具单次有效时间%s，总时间%s", 
                        strValue, strTotalValue);

		m_MessageHandle.InsertSystemMessage(strInfo.GetBuffer());

        //ShowNewKickPropDlg(TRUE, FALSE, FALSE, strInfo);
    }

    // 刷新
    OnSetPropImages();

    return true;
}

// PengJiLin, 2010-9-14, 踢人卡、防踢卡提示框显示
void CLoveSendClassInExe::ShowNewKickPropDlg(BOOL bUseForOther, BOOL bUseLink, 
                                             BOOL bIsTOut, CString& strInfo)
{
    //先建立对象
    AfxSetResourceHandle(GetModuleHandle(FACE_DLL_NAME));
    
    if(!m_dlgKickProp)
    {
        m_dlgKickProp=new CBoardCast();
        m_dlgKickProp->FuncForProp(bUseForOther, bUseLink, bIsTOut);
        m_dlgKickProp->SetPropInfo(strInfo);
        m_dlgKickProp->Create(IDD_BOARDCAST,GetDesktopWindow());
        m_dlgKickProp->pParentWnd=(CWnd*)this;
    }

    m_dlgKickProp->FuncForProp(bUseForOther, bUseLink, bIsTOut);
    m_dlgKickProp->SetPropInfo(strInfo);

    AfxSetResourceHandle(GetModuleHandle(NULL));
    m_dlgKickProp->ShowWindow(SW_SHOW);
    return;
}


// PengJiLin, 2011-3-31, 在线时长、局数送金币功能：获取格式化提示消息
void CLoveSendClassInExe::GetSendMoneyFormatInfo(CString& strInfo)
{
    CString strTimeInfo = "";
    CString strCountInfo = "";
    strInfo = "";

	CBcfFile fMsg(CBcfFile::GetAppPath()+"..\\ClientMessage.bcf");

    // 倍数支持    
    char tempStr0[100] = {0};
    GlbGetNumString(tempStr0, m_SendMoneyData.dwMoneyOnTimes, Glb().m_nPowerOfGold, Glb().m_bUseSpace, Glb().m_strSpaceChar);
    char tempStr1[100] = {0};
    GlbGetNumString(tempStr1, m_SendMoneyData.dwMoneyOnCounts, Glb().m_nPowerOfGold, Glb().m_bUseSpace, Glb().m_strSpaceChar);

    if(m_SendMoneyData.dwMoneyOnTimes > 0 && m_SendMoneyData.dwTimesNeed > 0)
    {
        UINT uSec = 0;
        UINT uMin = 0;
        uMin = m_SendMoneyData.dwTimesNeed / 60;
        uSec = m_SendMoneyData.dwTimesNeed % 60;
        
		strTimeInfo = fMsg.GetKeyVal("GameFrame","SendMoney_TimeInfo","再玩%d分钟%d秒, 就可获赠%s金币");
		strTimeInfo.Format(strTimeInfo, uMin, uSec, tempStr0);

    }

    if(m_SendMoneyData.dwMoneyOnCounts > 0 && m_SendMoneyData.dwCountsNeed > 0)
    {
		strCountInfo = fMsg.GetKeyVal("GameFrame","SendMoney_CountInfo","多玩%d局, 就可获赠%s金币");
		strCountInfo.Format(strTimeInfo, m_SendMoneyData.dwCountsNeed, tempStr1);
    }

    strInfo += strTimeInfo;
    strInfo += strCountInfo;
}

// PengJiLin, 2011-4-19, 在线时长、局数送金币图像提示
void CLoveSendClassInExe::ShowGetMoneyImage()
{
    return;
}
// PengJiLin, 2011-5-10, 在线时长、局数送金币功能：获取格式化提示信息
void CLoveSendClassInExe::GetSendMoneyFormatInfo(CString& strTime, CString& strCount)
{
    CString strTimeInfo = "";
    CString strCountInfo = "";
    strTime = "";
    strCount = "";

	CBcfFile fMsg(CBcfFile::GetAppPath()+"..\\ClientMessage.bcf");

    // 小数点支持
    char tempStr0[100] = {0};
    GlbGetNumString(tempStr0, m_SendMoneyData.dwMoneyOnTimes, Glb().m_nPowerOfGold, Glb().m_bUseSpace, Glb().m_strSpaceChar);
    char tempStr1[100] = {0};
    GlbGetNumString(tempStr1, m_SendMoneyData.dwMoneyOnCounts, Glb().m_nPowerOfGold, Glb().m_bUseSpace, Glb().m_strSpaceChar);

    if(m_SendMoneyData.dwMoneyOnTimes > 0 && m_SendMoneyData.dwTimesNeed > 0)
    {
        UINT uSec = 0;
        UINT uMin = 0;
        uMin = m_SendMoneyData.dwTimesNeed / 60;
        uSec = m_SendMoneyData.dwTimesNeed % 60;

		strTimeInfo = fMsg.GetKeyVal("GameFrame","SendMoney_TimeInfo","再玩%d分钟%d秒, 就可获赠%s金币");
		strTimeInfo.Format(strTimeInfo, uMin, uSec, tempStr0);
    }

    if(m_SendMoneyData.dwMoneyOnCounts > 0 && m_SendMoneyData.dwCountsNeed > 0)
    {
		strCountInfo = fMsg.GetKeyVal("GameFrame","SendMoney_CountInfo","多玩%d局, 就可获赠%s金币");
		strCountInfo.Format(strTimeInfo, m_SendMoneyData.dwCountsNeed, tempStr1);
    }

    if(strTimeInfo.GetLength() < 1 && strCountInfo.GetLength() > 1)
    {
        strCountInfo += ", 继续努力吧!";
    }
    if(strCountInfo.GetLength() < 1 && strTimeInfo.GetLength() > 1)
    {
        strTimeInfo += ", 继续努力吧!";
    }
    if(strTimeInfo.GetLength() > 1 && strCountInfo.GetLength() > 1)
    {
        strCountInfo += ", 继续努力吧!";
    }

    strTime = strTimeInfo;
    strCount = strCountInfo;
}

// PengJiLin, 2011-5-10, 在线时长、局数送金币功能：结算框使用的格式化信息
void CLoveSendClassInExe::GetSendMoneyFormatInfo(CString& strGet, CString& strTime, CString& strCount)
{
    // 小数点支持
    TCHAR tempStr[100] = {0};
    GlbGetNumString(tempStr, m_SendMoneyData.dwGetMoney, Glb().m_nPowerOfGold, Glb().m_bUseSpace, Glb().m_strSpaceChar);

	CString strMessage;
	CBcfFile fMsg(CBcfFile::GetAppPath()+"..\\ClientMessage.bcf");
	strMessage = fMsg.GetKeyVal("GameFrame","SendMoney"," 您获得系统赠送 %s 金币");
	strMessage.Format(strMessage,tempStr);

    strGet = "";
    if(m_SendMoneyData.dwGetMoney > 0) 
    {
        strGet = strMessage;
    }

    GetSendMoneyFormatInfo(strTime, strCount);
}

UserItemStruct* CLoveSendClassInExe::GetUserItemInfo(int iIdx)
{
	return m_pUserInfo[iIdx];
}
