#include "stdafx.h"
#include "XLogic.h"
#include "IPC\ChannelService.h"
#include "UI_ForLogic/IUserInterface.h"
#include "UI_ForLogic/GameIDDefines.h"
#include "Common/UIMessageDefines.h"
#include "IGameImpl.h"
#include "igameengine.h"
#include "LoveSendClassInExe.h"
#include "gameplacemessage.h"
#include "IMMessage.h"
#include "IMSetUser.h"
#include "Platform/AFCDevHead.h"
#include "bzsound.h"
#include "XRightFrame.h"			///游戏右边框
#include "../DllForGame/UseBoardCastWnd.h"

extern void DebugPrintfA(const char *p, ...);

#define GETCTRL(CTRL_TYPE,PRE,pUI,CTRL_ID)\
    PRE = dynamic_cast<CTRL_TYPE *>(pUI->GetIControlbyID(CTRL_ID));\

XLogic *g_pXLogic = NULL;

//Avatar系统相关
#define BzGetAvatarIndex(obj) \
	GetAvatarIndex((obj)->GameUserInfo.bLogoID,(obj)->GameUserInfo.i64Bank,(obj)->GameUserInfo.i64Money,\
	(obj)->GameUserInfo.dwPoint,m_GameInfo.uNameID,m_GameInfo.uComType)

void char2wchar(char *pchar,wchar_t * tchar) 
{
	int   nwLen  =MultiByteToWideChar(CP_ACP,0,pchar,-1,NULL,0); 
	MultiByteToWideChar(CP_ACP,0,pchar,-1,tchar,nwLen);
}
//默认列名和列宽度
const TCHAR COLOMN_NAME[][14] = 
{
	"昵称,100", "金币,60","财富级别,90", "积分,60", "积分级别,90", "魅力,60", "魅力级别,90", "性别,40",
	"总局数,50", "胜局数,50", "输局数,50", "和局数,50", "逃跑率,50","旁观对象,100"
};

//判断一个文件是否存在
bool FileExists(LPCTSTR lpszFileName,bool bIsDirCheck)
{
	DWORD dwAttributes = GetFileAttributes(lpszFileName);
	if(dwAttributes == 0xFFFFFFFF)
	{
		return false;
	}
	if((dwAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
	{
		return bIsDirCheck;
	}
	else
	{
		return !bIsDirCheck;
	}
}

XGameLogicBase* CreateGameLogic()
{
    if (NULL == g_pXLogic)
    {
        g_pXLogic = new XLogic();
    }    
    return g_pXLogic;
}
void DestroyGameLogic(XGameLogicBase **ppLogic)
{
    if (*ppLogic != NULL)
    {
        delete *ppLogic;
        *ppLogic = NULL;
    }
}

XLogic::XLogic()
{
    m_pIPC = NULL;
    m_hGameInstance = NULL;

	m_bUserContestOver = false;
	m_iVideoOffsetNum = 0;
	memset(m_pVideoXOffset, 0, sizeof(m_pVideoXOffset));
	memset(m_pVideoYOffset, 0, sizeof(m_pVideoYOffset));
	m_pVideoInterface = NULL;

    m_pGameImpl = NULL;
    m_pIUI = NULL;
    m_hWnd = NULL;
    m_dwIPCHeartBeatTick = 0;
    m_pEngine = NULL;
    m_nPropPage = 0;

    memset(m_pPropContain, 0, sizeof(m_pPropContain));

    m_pPropPannel = NULL;
    m_pPropSelUser = NULL;
    m_dlgBroadcast = NULL;
    m_pPersonBank = NULL;

    m_hAudioMod = NULL;
    m_pBackAudio = NULL;

    pCreateAudioPlay = NULL;
    pDeleteAudioPlay = NULL;

    char psz[255]={0};
    sprintf(psz, "Audio.dll");
    m_hAudioMod = ::LoadLibrary(psz);
    if (m_hAudioMod)
    {
        pCreateAudioPlay = (CREATEAUDIOPLAYC)::GetProcAddress(m_hAudioMod, "CreateAudioPlayC");
        pDeleteAudioPlay = (DELETEAUDIOPLAYC)::GetProcAddress(m_hAudioMod, "DeleteAudioPlayC");
    }
    if (pCreateAudioPlay)
    {
        m_pBackAudio = pCreateAudioPlay();
    }

    m_NetSignalLevel=0;
    m_SavedSingalIndex=0;     //上一次发送的网络信息检测序号，收到消息后与m_NetSingalIndex同步
    m_NetSignalIndex=0;

	m_bIsAutoScroll=true;
	m_bIsAcceptChatMsg=true;

	//第二版游戏银行对话框
	m_pBank2Wnd = NULL;	

	//获取原窗口尺寸
	m_nWiondwMetricsX = ::GetSystemMetrics(SM_CXSCREEN); 
	m_nWiondwMetricsY = ::GetSystemMetrics(SM_CYSCREEN);

	m_pPmdWnd = NULL;

	m_bIsQueueDisMiss = false;

	m_pRightFrame = NULL;

	//用户列表
	m_pUserListDu = NULL;	

	//显示右边框的按钮
	m_pShowRightFrameBtn = NULL; 

    m_strBcfFileName = "";

	//百宝湾UI带的聊天容器
	m_pTalkCtn = NULL;

	m_bWatchMode = false;
}

XLogic::~XLogic()
{
    if (NULL!=m_hGameInstance && m_pGameImpl!=NULL)
    {
        DESTROYGAMEIMPLEMENT DestroyGame = (DESTROYGAMEIMPLEMENT)GetProcAddress(m_hGameInstance, ("DestroyNewGameImpl"));
        if (DestroyGame!=NULL)
        {
            DestroyGame(&m_pGameImpl);
        }
    }
	if (m_pVideoInterface)
	{
		m_pVideoInterface->exit();
		if (m_pVideoInterface)
		{
			m_pVideoInterface = NULL;
		}
	}
    delete m_pPropPannel;
    m_pPropPannel = NULL;
    delete m_pPropSelUser;
    m_pPropSelUser = NULL;
    delete m_dlgBroadcast;
    m_dlgBroadcast = NULL;
    delete m_pPersonBank;
    m_pPersonBank = NULL;

    //if (m_pBackAudio)
    //{
    //    if (pDeleteAudioPlay)
    //        pDeleteAudioPlay(&m_pBackAudio);
    //}
    //if (m_hAudioMod)
    //{
    //    ::FreeLibrary(m_hAudioMod);
    //    m_hAudioMod = NULL;
    //}
}


/// 初始化
int XLogic::Initial(LogicInitParam_t &lip)
{
    DebugPrintfA("%s", __FUNCTION__);


    for (int i=0; i<m_GameInfo.uDeskPeople; ++i)
    {
        m_IsUserSmall[i] = false;
    }


    ////////////////////////////////////////////////////////////////////////////////
    CBcfFile forder( CBcfFile::GetAppPath() + "..\\Order.bcf");
    Glb().m_nPowerOfGold = forder.GetKeyVal("VirtualMoney", "PowerOfGold", 0);
    //金币显示分隔符配置
    Glb().m_bUseSpace = forder.GetKeyVal("VirtualMoney", "UseSpace", 0);
    CString strSpace = forder.GetKeyVal("VirtualMoney", "SpaceChar", " ");
    memcpy(Glb().m_strSpaceChar, strSpace, sizeof(Glb().m_strSpaceChar)*sizeof(TCHAR));
    ////////////////////////////////////////////////////////////////////////////////

	int _bit = forder.GetKeyVal("VirtualMoney", "SpaceBits", 3);
	if (_bit == 0) 
		Glb().m_iMaxBit = 18;
	else 
		Glb().m_iMaxBit = 18 + 18 / _bit - 1;

    m_pEngine = (IGameEngine *)lip.pEngine;
    memcpy(&m_GameInfo, lip.pGameInfo, sizeof(m_GameInfo));
    CLoveSendClassInExe::ReadLevelData();
    m_GameInfo.pOrderName = GetPointOrderName;
	
    m_dwIPCHeartBeatTick = GetTickCount();
    m_hWnd = lip.hwndHall;
    m_pIPC = (CChannelService *)lip.pIpc;
    m_pIUI = (IUserInterface *)lip.pUI;
    m_hGameInstance = lip.hInstance;
    if (NULL!=m_hGameInstance)
    {
        /// 获取函数指针
        CREATEGAMEIMPLEMENT CreateGame= (CREATEGAMEIMPLEMENT)GetProcAddress(lip.hInstance, ("CreateNewGameImpl"));
        if (NULL!=CreateGame)
        {
            m_pGameImpl = CreateGame(dynamic_cast<IGameFrame *>(this), m_pIUI);
		}
	}	
	RECT rcShrink = {0,0,0,0};
	m_pIUI->SetControlsShrinkArea(rcShrink);
    m_pGameImpl->Initial();
    m_pGameImpl->InitUI();
	//创建右框架
	m_pRightFrame = new XRightFrame(this);
	m_pRightFrame->Create(m_hWnd,NULL,UI_WNDSTYLE_DIALOG,0,0,0,0,0,NULL);
	m_pUserListDu = m_pRightFrame->m_pUsersList;

	//显示游戏的右窗口
	if (m_pRightFrame)
	{
		m_pRightFrame->HideWindow();
	}
	
	WINDOWPLACEMENT place;
	m_pEngine->GetCWnd()->GetWindowPlacement(&place);

	IButton *pBtn = NULL ; 

	CBcfFile bcffile(CBcfFile::GetAppPath() + _T("GameOption.bcf"));
	UINT uMaxType = bcffile.GetKeyVal("FrameControl","max_type",1);
	if (0 == uMaxType)
	{
		pBtn = dynamic_cast<IButton *>(m_pIUI->GetIControlbyID(BN_SET_FRAME)) ; 
		if(NULL != pBtn)
		{
			pBtn->SetControlVisible(false); 
		}
		pBtn = dynamic_cast<IButton *>(m_pIUI->GetIControlbyID(BN_RESET_FRAME)) ; 
		if(NULL != pBtn)
		{
			pBtn->SetControlVisible(false); 
		}
	}
	if(place.showCmd == SW_SHOWMAXIMIZED)
	{
		pBtn = dynamic_cast<IButton *>(m_pIUI->GetIControlbyID(BN_SET_FRAME)) ; 
		if(NULL != pBtn)
		{
			pBtn->SetControlVisible(false) ; 
		}
		pBtn = dynamic_cast<IButton *>(m_pIUI->GetIControlbyID(BN_RESET_FRAME)) ; 
		if(NULL != pBtn)
		{
			pBtn->SetControlVisible(true) ; 
		}
	}
	else
	{
		pBtn = dynamic_cast<IButton *>(m_pIUI->GetIControlbyID(BN_RESET_FRAME)) ; 
		if(NULL != pBtn)
		{
			pBtn->SetControlVisible(false) ; 
		}
		pBtn = dynamic_cast<IButton *>(m_pIUI->GetIControlbyID(BN_SET_FRAME)) ; 
		if(NULL != pBtn)
		{
			pBtn->SetControlVisible(true) ; 
		}
	}

	//获取视频窗口偏移位置
	CString strKey = _T("VideoOffset");
	m_iVideoOffsetNum = bcffile.GetKeyVal(strKey, _T("DeskPeople"), 0);
	if (0 < m_iVideoOffsetNum && m_iVideoOffsetNum <= VIDEO_MAX_PEOPLE)
	{
		CString strX, strY;
		for (int i=0; i!=m_iVideoOffsetNum; ++i)
		{
			strX.Format(_T("People%dx"), i);
			strY.Format(_T("People%dy"), i);
			m_pVideoXOffset[i] = bcffile.GetKeyVal(strKey, strX, 0);
			m_pVideoYOffset[i] = bcffile.GetKeyVal(strKey, strY, 0);
		}
	}

    BYTE bMeDeskStation=m_GameInfo.uisMeUserInfo.GameUserInfo.bDeskNO;
    GetOnLineUser(bMeDeskStation);

    UIInitBiaoQingKuang();
    InitPlayerListCtrl();
    InitPropPannel();
    // 初始化银行界面
    m_pPersonBank = new CPBankDlg(m_pEngine->GetCWnd());
	m_pPersonBank->m_pLogic = this;

    SetTimer(OLEDRAW_TIMER_ID, OLEDRAW_INTERVAL);

    LoadBkSoundList();

    IImage *pNetSignal = dynamic_cast<IImage*>(m_pIUI->GetIControlbyID(IMAGE_NET_SIGNAL));
    if (pNetSignal)
    {
        pNetSignal->SetSrcY(0);
        pNetSignal->SetControlVisible(true);
    }

    IEditorText* _P=NULL;
    GETCTRL(IEditorText,_P,m_pIUI, ETEXT_TALK_INPUT);

    if (_P)
        _P->SetMaxTypeIn(30);

    SetFocus(m_pIUI->GetWindowsHwnd());

	//m_pEngine->GetCWnd()->ShowWindow(SW_MINIMIZE);
	// duanxiaohui 2011-11-14 增加详细资料框
	m_pUserDetailsDlg = new CUserDetailsDlg();
	m_pUserDetailsDlg->m_uGameNameID = m_GameInfo.uNameID;
	m_pUserDetailsDlg->m_uComType = m_GameInfo.uComType;
	AfxSetResourceHandle(GetModuleHandle(FACE_DLL_NAME));
	m_pUserDetailsDlg->Create(IDD_USER_DETAILS, NULL);
	AfxSetResourceHandle(GetModuleHandle(NULL));
	// end duanxiaohui

	m_pPmdWnd = new CGameTrottingWnd(m_pEngine->GetCWnd());
	m_pPmdWnd->init(m_pEngine->GetCWnd()->GetSafeHwnd());
	m_pPmdWnd->AdjustPos((HWND)m_pIUI->GetWindowsHwnd());
	m_pPmdWnd->SetLabelTextFont("宋体", 16);
	m_pPmdWnd->ShowWindow(SW_HIDE);
	m_GameInfo.bEnableWatch=AfxGetApp()->GetProfileInt(m_GameInfo.szGameName,TEXT("CanLooking"),FALSE);

	m_GameInfo.bEnableWatch=AfxGetApp()->GetProfileInt(m_GameInfo.szGameName,TEXT("CanLooking"),FALSE);//(GetProfileInt(TEXT("CanLooking"),FALSE)!=FALSE);
	m_GameInfo.bEnableSound=AfxGetApp()->GetProfileInt(m_GameInfo.szGameName,TEXT("PlaySound"),FALSE);//(GetProfileInt(TEXT("PlaySound"),TRUE)!=FALSE);
	m_GameInfo.bEnableBackSound=AfxGetApp()->GetProfileInt(m_GameInfo.szGameName,TEXT("BackSound"),FALSE);//(GetProfileInt(TEXT("BackSound"),TRUE)!=FALSE);
	m_GameInfo.bShowUserInfo=AfxGetApp()->GetProfileInt(m_GameInfo.szGameName,TEXT("ShowUserInfo"),FALSE);//(GetProfileInt(TEXT("ShowUserInfo"),FALSE)!=FALSE);  //PengJiLin, 2010-10-15, 缺省不显示玩家进出信息
	

	OnMessageProc(UM_FIXRIGHTFRAME,0,0);

	//初始化聊天用的Combo控件，如果Combo控件在在，则原来的文件输入框不用
	ICombox* pCobBox = dynamic_cast<ICombox*>(m_pIUI->GetIControlbyID(ETEXT_TALK_COMBO));
	if (pCobBox)
	{
		IEditorText* pEdtCtrl = dynamic_cast<IEditorText*>(m_pIUI->GetIControlbyID(ETEXT_TALK_INPUT));
		if(pEdtCtrl) 
		{
			pEdtCtrl->SetControlVisible(false);
		}
		pCobBox->SetControlMoveDirect(1);
		pCobBox->SetAddItemWidth(30);
		//加载聊天默认句子
		OnReLoadTalkINI();
	}

	IContain* pRightCtn = dynamic_cast<IContain*>(m_pIUI->GetIControlbyID(800));
	if (pRightCtn)
	{
		pRightCtn->SetControlVisible(false);
	}
	//设置一些固有控件，收缩范围与窗口同步，不与右边框隐藏显示所影响/////////////////////////////////
	ISudoku* pSoduku = dynamic_cast<ISudoku*>(m_pIUI->GetIControlbyID(1));
	if (pSoduku)
	{
		pSoduku->SetHaveShrinkArea(false);

		RECT rc = {0,0,0,0};

		Image* pImage = Image::FromFile(L"resources\\image\\GameBg.png");
		if(pImage)
		{
			rc.right = pImage->GetWidth();
			rc.bottom = pImage->GetHeight();
			delete pImage;
			pImage = NULL;
		}
		pSoduku->SetPicByIndex(8,L"resources\\image\\GameBg.png",rc);
	}

	pBtn = dynamic_cast<IButton*>(m_pIUI->GetIControlbyID(1003500));
	if (pBtn)
	{
		pBtn->SetHaveShrinkArea(false);
	}

	pBtn = dynamic_cast<IButton*>(m_pIUI->GetIControlbyID(BN_MIN_FRAME));
	if (pBtn)
	{
		pBtn->SetHaveShrinkArea(false);
	}

	pBtn = dynamic_cast<IButton*>(m_pIUI->GetIControlbyID(BN_RESET_FRAME));
	if (pBtn)
	{
		pBtn->SetHaveShrinkArea(false);
	}


	pBtn = dynamic_cast<IButton*>(m_pIUI->GetIControlbyID(BN_CLOSE_FRAME));
	if (pBtn)
	{
		pBtn->SetHaveShrinkArea(false);
	}

	pBtn = dynamic_cast<IButton*>(m_pIUI->GetIControlbyID(BN_SET_FRAME));
	if (pBtn)
	{
		pBtn->SetHaveShrinkArea(false);
	}

	pBtn = dynamic_cast<IButton*>(m_pIUI->GetIControlbyID(BN_VOLUM_UP));
	if (pBtn)
	{
		pBtn->SetHaveShrinkArea(false);
	}

	pBtn = dynamic_cast<IButton*>(m_pIUI->GetIControlbyID(BN_VOLUM_DOWN));
	if (pBtn)
	{
		pBtn->SetHaveShrinkArea(false);
	}

	pBtn = dynamic_cast<IButton*>(m_pIUI->GetIControlbyID(BN_VOLUM_OFF));
	if (pBtn)
	{
		pBtn->SetHaveShrinkArea(false);
	}

	pBtn = dynamic_cast<IButton*>(m_pIUI->GetIControlbyID(BN_VOLUM_ON));
	if (pBtn)
	{
		pBtn->SetHaveShrinkArea(false);
	}

	IImage* pImage = NULL;
	pImage = dynamic_cast<IImage*>(m_pIUI->GetIControlbyID(IMAGE_NET_SIGNAL));
	if (pImage)
	{
		pImage->SetHaveShrinkArea(false);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	//生成隐藏可边框的按钮
	IButton* pCloseBtn = dynamic_cast<IButton*>(m_pIUI->GetIControlbyID(BN_CLOSE_FRAME));
	IContain* pCtn = dynamic_cast<IContain*>(m_pIUI->GetIControlbyID(m_pIUI->GetControlBlongedId(BN_CLOSE_FRAME)));

	if (pCloseBtn)
	{
		m_pShowRightFrameBtn = dynamic_cast<IButton*>(pCloseBtn->Clone(BN_SHOW_RIGHTFRAME));
		pCtn->IInsertControl(m_pShowRightFrameBtn);
		char szBuff[256] = {0};
		sprintf(szBuff,"..\\%sshow_bt.png",m_skinmgr.GetGameSkinPath());
		m_pShowRightFrameBtn->LoadPic(CA2W(szBuff));
		m_pShowRightFrameBtn->SetGameW(10);
		m_pShowRightFrameBtn->SetGameH(98);
		m_pShowRightFrameBtn->SetGameXY(1008,350);
		m_pShowRightFrameBtn->SetHaveShrinkArea(false);
		m_pShowRightFrameBtn->SetControlVisible(false);
	}

	pBtn = dynamic_cast<IButton*>(m_pIUI->GetIControlbyID(BN_VOLUM_UP));
	if (pBtn)
	{
		pBtn->SetControlVisible(false);
	}
	pBtn = dynamic_cast<IButton*>(m_pIUI->GetIControlbyID(BN_VOLUM_DOWN));
	if (pBtn)
	{
		pBtn->SetControlVisible(false);
	}
	pBtn = dynamic_cast<IButton*>(m_pIUI->GetIControlbyID(BN_VOLUM_ON));
	if (pBtn)
	{
		pBtn->SetControlVisible(false);
	}
	pBtn = dynamic_cast<IButton*>(m_pIUI->GetIControlbyID(BN_VOLUM_OFF));
	if (pBtn)
	{
		pBtn->SetControlVisible(false);
	}

	CString strWndInitShow = bcffile.GetKeyVal("FrameControl","WndInitShow","Restore");
	if (strWndInitShow == "Max")
	{
		m_pEngine->SwitchSize();
	}

	//百宝湾UI带的聊天容器
	m_pTalkCtn = dynamic_cast<IContain*>(m_pIUI->GetIControlbyID(TALK_CONATINER));
    return 0;
}
/// IPC消息
bool XLogic::OnIPCMessage(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)
{
    //默认处理
    switch (pHead->wMainCmdID)
    {
    case IPC_MAIN_SOCKET:	//网络数据，来自于与游戏服务器端关于游戏本身的消息
        {   
            return OnIPCSocket(pHead,pIPCBuffer,wDataSize,hWndSend);
        }
    case IPC_MAIN_CONCTROL:	//控制消息
        {            
            return OnIPCControl(pHead,pIPCBuffer,wDataSize,hWndSend);
        }
    case IPC_MAIN_CONFIG:	//设置消息
        {   
            return OnIPCConfig(pHead,pIPCBuffer,wDataSize,hWndSend);
        }
    case IPC_MAIN_USER:		//原OnControlMessage内容
        {            
            return OnIPCUserAct(pHead,pIPCBuffer,wDataSize,hWndSend);
        }
    case IPC_MAIN_PROP:		//玩家道具初始化信息
        {            
            return OnIPCProp(pHead,pIPCBuffer,wDataSize,hWndSend);
        }
	case IPC_USER_PROP_RESULT:   //玩家道具操作结果
		{
			return OnIPCUserPropResult(pHead,pIPCBuffer,wDataSize,hWndSend);
		}
	case IPC_FRIEND:
		{
			if (pHead->wSubCmdID == IPC_ASS_CHECKFRIEND)
			{

				UserItemStruct *pUser = (UserItemStruct *)m_ThisDeskPtrArray.GetAt(m_iSelectIndex);
				if (pUser == NULL) return 0;
				CString s;
				bool bIsHave = *((int*)pIPCBuffer) == 1;
				s.Format("xxx CLoveSendClassInExe OnAddFriend %d, %d", bIsHave, pUser->GameUserInfo.dwUserID);
				OutputDebugString(s);
				if (bIsHave)
				{
					AFCMessageBox("对不起，该用户已经是您的好友，不需要再添加！", "提示");
				}
				else
				{

					//Ctest _test;
					//添加请求留言
					CIMSetUser imsu;
					imsu.dwUser = pUser->GameUserInfo.dwUserID;
					imsu.szName = pUser->GameUserInfo.nickName;
					AfxSetResourceHandle(GetModuleHandle(FACE_DLL_NAME));
					if(imsu.DoModal()==IDCANCEL)
						return true;
					AfxSetResourceHandle(GetModuleHandle(NULL));

					MSG_IM_C_ADDREQUEST* msg = new MSG_IM_C_ADDREQUEST;
					ZeroMemory(msg,sizeof(MSG_IM_C_ADDREQUEST));

					//CMainRoomEx *pWnd=(CMainRoomEx*)pParentWnd;
					msg->dwUserID= GetMeUserInfo()->dwUserID;
					msg->dwRequestedUserID= pUser->GameUserInfo.dwUserID;
					memcpy(&(msg->sUserName), GetMeUserInfo()->nickName,sizeof(GetMeUserInfo()->nickName));
					memcpy(&(msg->sRequestNotes),imsu.imWords,imsu.imWords.GetLength());
					msg->nMsgLength=imsu.imWords.GetLength();
					msg->cbSize=sizeof(MSG_IM_C_ADDREQUEST)-sizeof(msg->sRequestNotes)+msg->nMsgLength+1;//实际长度

					//MSG_IM_C_ADDREQUEST_Game t;
					//t.dwUserID1 = GetMeUserInfo()->dwUserID;
					//t.dwUserID = m_dwCurrentUserID;
					//m_pGameRoom->PostMessage(WM_FRIEND_R, sizeof(MSG_IM_C_ADDREQUEST), LPARAM(msg));
					m_pIPC->SendData(IPC_FRIEND, IPC_ASS_ADDFRIEND, msg, msg->cbSize);
					return true;
				}
			}
			if (pHead->wSubCmdID == IPC_ASS_GETFRIENDLIST)
			{
				MSG_IM_C_GETFRIENDLIST *msg = (MSG_IM_C_GETFRIENDLIST*)pIPCBuffer;

				if(wDataSize == 0)
				{
					m_FriendList.erase(m_FriendList.begin(), m_FriendList.end());
				}
				else if(wDataSize == sizeof(MSG_IM_C_GETFRIENDLIST))
				{
					m_FriendList.push_back(*msg);
				}
			}
		}
		break;

	case IPC_TROTTING:
		{
			OnIPCTrottingMsg(pHead,pIPCBuffer,wDataSize,hWndSend);
		}
		break;

	case IPC_EX_SKIN:
		{			
			OnExchageSkin(pHead,pIPCBuffer,wDataSize,hWndSend);
		}
		break;
    }

    return false;
}


//读talkIngame.ini
void XLogic::OnReLoadTalkINI()
{
	//清除原来内容
	ICombox* pComb = dynamic_cast<ICombox*>(m_pIUI->GetIControlbyID(ETEXT_TALK_COMBO));
	if (!pComb)
	{
		return;
	}

	pComb->Clear();
	CBcfFile f("talkInGame.bcf");
	CString key="BZW_TALK";
	CString szTalk;
	CString szNumber;
	int nIdx = 0;
	do
	{
		szNumber.Format("%d",++nIdx);
		szTalk = f.GetKeyVal(key,"T"+szNumber,"");
		if(!szTalk.IsEmpty())
			pComb->AddString(CA2W(szTalk.GetBuffer()));
	}
	while(!szTalk.IsEmpty());

	pComb->SetText(L"");

	return;
}

bool XLogic::OnIPCControl(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)
{
    ASSERT(pHead->wMainCmdID==IPC_MAIN_CONCTROL);
    switch (pHead->wSubCmdID)
    {
    case IPC_SUB_START_FINISH:		//启动完成
        {
            return true;
        }
    case IPC_SUB_CLOSE_FRAME:		//关闭框架
        {
            m_pIPC->SendData(IPC_MAIN_IPC_KERNEL, IPC_SUB_CLOSE_FRAME, NULL, 0);
            QuitGame(false);

            DebugPrintfA("收到IPC_SUB_CLOSE_FRAME，退出游戏");
            return true;
        }
    case IPC_SUB_SHOW_MSG_IN_GAME:
        {
            MsgInGameStruct *pMsg = (MsgInGameStruct *)pIPCBuffer;
            // 根据传过来的内容，判断传入参数是否应为空
            /*this->m_IGameFrame->ShowMsgInGame(pMsg->szMsg,
            pMsg->bNewOrSys,
            pMsg->uSize,
            pMsg->bIsFontValid?pMsg->szFontName:NULL,
            pMsg->bIsTitleValid?pMsg->szTitle:NULL);*/

			//玩家道具操作结果显示
			InsertSystemMessage(pMsg->szMsg);
            return true;
        }
    case IPC_SUB_SHOW_DUDU:
        {
            MsgDuduStruct *pMsg = (MsgDuduStruct *)pIPCBuffer;
            // 喇叭消息
            InsertDuduMsg(pMsg->szSendName,
            pMsg->szTargetName,
            pMsg->szCharString,
            pMsg->crTextColor,
            pMsg->bShowTime,
            pMsg->uSize,
            pMsg->bIsFontValid?pMsg->szFontName:NULL,
            pMsg->iDuduType);
            return true;
        }
    case IPC_SUB_JOIN_IN_GAME:
        {
            //if (NULL != m_IGameFrame)
            //{            //    
            //    m_IGameFrame->ShowMsgInGame(NULL);
            //}
            return true;
        }
    }

    return false;
}
//网络数据，来自于与游戏服务器端关于游戏本身的消息
bool XLogic::OnIPCSocket(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)
{
    ASSERT(pHead->wMainCmdID==IPC_MAIN_SOCKET);

    // 心跳消息处理，记录时刻
    m_dwIPCHeartBeatTick = GetTickCount();
    m_pIPC->SendData(IPC_MAIN_SOCKET, IPC_SUB_IPC_HEART_BEAT, NULL, 0);

    switch (pHead->wSubCmdID)
    {
    case IPC_SUB_SOCKET_SEND:	//发包消息
        {
            //效验数据
            ASSERT(wDataSize>=sizeof(CMD_Command));
            if (wDataSize<sizeof(CMD_Command)) 
            {
                return false;
            }

            //处理数据
            NetMessageHead * pNetHead=(NetMessageHead *)pIPCBuffer;
            WORD wSendSize=wDataSize-sizeof(NetMessageHead);
            void *pNetData = NULL;
            if (wSendSize >= 0)
            {
                pNetData = (BYTE*)pIPCBuffer + sizeof(NetMessageHead);
            }
            //if (NULL != m_IGameFrame)
            //{
            //    m_IGameFrame->OnSocketReadEvent(pNetHead, pNetData, wSendSize);
            //}
            OnSocketReadEvent(pNetHead, pNetData, wSendSize);
            return true;
        }
    case IPC_SUB_IPC_HEART_BEAT:
        {
            // 心跳消息处理，记录时刻
            m_dwIPCHeartBeatTick = GetTickCount();
            m_pIPC->SendData(IPC_MAIN_SOCKET, IPC_SUB_IPC_HEART_BEAT, NULL, 0);
            return true;
        }
    }

    return false;
}
/// IPC心跳检测
bool XLogic::OnIPCHeartBeat()
{
    DWORD dwThisTick = GetTickCount();
    if (NULL==m_pIPC)
    {
        return false;
    }
    if (dwThisTick - m_dwIPCHeartBeatTick > (HEART_BEAT_DELAY+HEART_BEAT_SECOND)*1000) // 加上延时
    {
        DebugPrintfA("IPC 心跳检测失败，退出游戏");
        //表示大厅端已经停止了
        QuitGame();
        return false;
    }
    else
    {
        m_pIPC->SendData(IPC_MAIN_SOCKET, IPC_SUB_IPC_HEART_BEAT, NULL, 0);
    }
    return true;
}

/************************************************************************/
//Function:checkNetSignal()
//Parameter:
//                pNetData        :服务器反回的消息体
//Author:Fred Huang 2008-06-27
//Note:解析服务器返回的消息
/************************************************************************/
void XLogic::getNetSignal(void * pNetData)
{
    MSG_GP_Game_NETSignal *netSignal=(MSG_GP_Game_NETSignal*)pNetData;
    //如果不是发给自己的，丢弃
    if(m_GameInfo.uisMeUserInfo.GameUserInfo.dwUserID!=netSignal->dwUserID)
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
    drawSignal();
    //Invalidate(true);
}

/************************************************************************/
//Function:drawSignal()
//Parameter:无
//Author:Fred Huang 2008-06-27
//Note:画图
/************************************************************************/
void XLogic::drawSignal()
{
    IImage *pNetSignal = dynamic_cast<IImage*>(m_pIUI->GetIControlbyID(IMAGE_NET_SIGNAL));
    if (pNetSignal)
    {
        pNetSignal->SetSrcY(0);
        pNetSignal->SetControlVisible(true);
        pNetSignal->SetSrcY(m_NetSignalLevel*pNetSignal->GetPicHeight()/6);
    }

    //if (NULL == pDc || NULL == pDc->m_hDC )
    //    return;

    //if(!pSingnalimg || pSingnalimg->GetFlags()==0)
    //    return;
    //CString s="..\\";// CINIFile::GetAppPath();

    //// ----- 调用指定游戏的配置文件 -------- dxh
    //char GameCode[20];
    //::memset(GameCode,0,sizeof(GameCode));
    //GetGameCode(GameCode);
    ////if (str == "11901800"
    ////	|| str == "10901800"
    ////	|| str == "30501800")
    //TCHAR szKey[10];
    //CString sPath=CBcfFile::GetAppPath();
    //CBcfFile fsr( sPath + "..\\SpecialRule.bcf");
    //wsprintf(szKey, "%d", m_pGameInfo->uNameID);
    //int iResult = fsr.GetKeyVal (_T("BJLType"), szKey, 0);

    //if (iResult>0)
    //{
    //    s = ".\\config\\";  ///<add by wxx 2010.7.1
    //}
    ////else
    ////{
    ////	s="..//config//"; ///<add by wxx 2010.07.14
    ////	//return;
    ////}
    //// ----- 调用指定游戏的配置文件 --------

    //CBcfFile f( s + "GameOption.bcf");

    //CString key= Glb().m_skin;

    //CRect rc;
    //GetClientRect(rc);
    //int x = rc.Width() - f.GetKeyVal(key, _T("game_signalx"), 184) - pSingnalimg->GetWidth();

    //Graphics g(this->m_hWnd);
    //g.DrawImage(pSingnalimg,Rect(x,f.GetKeyVal(key, _T("game_signaly"),2),pSingnalimg->GetWidth(),pSingnalimg->GetHeight()/6),0,m_NetSignalLevel*pSingnalimg->GetHeight()/6,pSingnalimg->GetWidth(),pSingnalimg->GetHeight()/6,UnitPixel);
}

//套接字消息
bool XLogic::OnSocketReadEvent(NetMessageHead * pNetHead, void *pNetData, WORD wSendSize)
{
    //过滤处理
    if (m_GameInfo.uisMeUserInfo.GameUserInfo.bDeskNO==255) 
        return true;
    //处理消息
    switch (pNetHead->bMainID)
    {
    case MDM_GR_NETSIGNAL:      //网络状况
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
        break;
    case MDM_GM_GAME_NOTIFY:	//游戏消息
        {
            if (ASS_PROP==pNetHead->bAssistantID)
            {

				
				if (wSendSize!=sizeof(_TAG_PROP_MOIVE_USE)) {
					//WriteInfo(_T("CLoveSendClassInExe::HandleGameMessage 00"));
					return FALSE;
				}
				_TAG_PROP_MOIVE_USE *propMoive=(_TAG_PROP_MOIVE_USE*)pNetData;

				UserProp(propMoive);

				//AfxMessageBox("送花");
                // 不必要的消息，占用了2这个消息号
                return 0;
            }

			CString s;
			s.Format("xxxdxy %d %d %I64d",(WORD)pNetHead->bAssistantID, m_GameInfo.uisMeUserInfo.GameUserInfo.dwUserID, m_GameInfo.uisMeUserInfo.GameUserInfo.i64Money);
			OutputDebugString(s);

            int nRet = m_pGameImpl->HandleGameMessage(MDM_GM_GAME_NOTIFY,(WORD)pNetHead->bAssistantID,(BYTE*)pNetData, wSendSize);
            if (m_GameInfo.bIsInRecord)
            {
                //SetRecordView();
            }
            return 0==nRet;
        }
        break;
    case MDM_GM_GAME_FRAME:		//框架消息
        {
            return OnFrameMessage(pNetHead,pNetData,wSendSize);
        }
    case MDM_GM_MESSAGE:		//信息消息
        {
            return OnSystemMessage(pNetHead,pNetData,wSendSize);
        }
    case MDM_GR_PROP:		//使用道具
        {
            return OnPropMessage(pNetHead,pNetData,wSendSize);
        }
    case MDM_GR_ROOM2GAME:
        {
            switch (pNetHead->bAssistantID)
            {
			case ASS_GR_R2G_USEPROP:    // PengJiLin, 2010-10-14, 游戏端道具的即买即用功能
				{
					if(wSendSize != sizeof(_TAG_USERPROP)) return false;

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
                            if (wSendSize != sizeof(MSG_GR_S_GetNickNameOnID_t)) return false;
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
    case MDM_GR_MONEY:
        {   
            return OnAboutMoney(pNetHead,pNetData,wSendSize);
        }
        break;
	case MDM_BANK:
		{
			if (m_pBank2Wnd)
			{
				m_pBank2Wnd->OnNetMessage(pNetHead,pNetData,wSendSize);
			}
		}
		break;
    default:
        break;
    }
    return true;
}

// 从平台传来的信息，数据wDataSize = pIPCBuffer数据的长度
bool XLogic::OnIPCConfig(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)
{
    ASSERT(pHead->wMainCmdID==IPC_MAIN_CONFIG);
    switch (pHead->wSubCmdID)
    {
    case IPC_SUB_SERVER_INFO:
        {
            // 直接就是m_GameInfo的内容，没有别的命令
            //处理数据

            IPC_SocketPackage * pSocketPackage=(IPC_SocketPackage *)pIPCBuffer;
            CMD_Command *pCommand= &pSocketPackage->Command;
            WORD wSendSize=wDataSize;
            if (wSendSize != sizeof(GameInfoStructInExe)) 
            {
                AfxMessageBox("平台和GameLancher中的GameInfoStructInExe不一致");
                return false;
            }
            else 
            {
                memcpy( &m_GameInfo, pIPCBuffer, sizeof(m_GameInfo));
                //if (m_GameInfo.pOrderName = NULL)
                //{
                //    //m_GameInfo.pOrderName = GetGameOrderNameInExe;
                //}
                m_GameInfo.pOrderName = GetPointOrderName;

				CString str;
				str.Format("DGameL: 1.1 m_GameInfo.pOrderName = 0x%08x",GetPointOrderName);
				OutputDebugString(str);

                /*ShowWindow(SW_HIDE);                
                OnBnClickedButton1();*/
                m_pIPC->SendData(IPC_MAIN_CONFIG,IPC_SUB_SERVER_INFO,NULL,0);
            }
            return true;
        }
    case IPC_SUB_ENCRYPT_TYPE:
        {
            int *pEncrypt = (int*)pIPCBuffer;
            Glb().m_CenterServerPara.m_nEncryptType = *pEncrypt;
            return true;
        }
    }

    return false;
}
// 从平台传来的信息，数据pIPCBuffer中第一个UINT，是控制码UINT uControlCode, void * pControlData, UINT uDataSize
bool XLogic::OnIPCUserAct(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)
{
    ASSERT(pHead->wMainCmdID==IPC_MAIN_USER);
    if (NULL == m_pGameImpl)
    {
        //DebugPrintf("由于窗口没启动，丢了信息");
        return false;
    }
    switch(pHead->wSubCmdID)
    {
    case IPC_SUB_REPLY_USER_LIST:
        {
            if(wDataSize<sizeof(UserItemStruct)) return false;
            OnUserListAtDesk((UserItemStruct *)pIPCBuffer);
            return true;
        }
    case IPC_SUB_USER_STATUS:
    case IPC_SUB_USER_SCORE:		//用户积分
        {
            UINT *pControlCode = (UINT*)pIPCBuffer;
            void *pControlData = (BYTE*)pIPCBuffer + sizeof(UINT);
            OnControlMessage( *pControlCode, pControlData, wDataSize-sizeof(UINT) );
            break;
        }
    case IPC_SUB_USER_COME:
        {
            UserItemStruct *pUserInfo = (UserItemStruct *)pIPCBuffer;

			if (GetMeUserInfo()->dwUserID == pUserInfo->GameUserInfo.dwUserID)
			{
				GetMeUserInfo()->i64Bank = pUserInfo->GameUserInfo.i64Bank;
			}
            //this->m_IGameFrame->ResetUserInfo(pUserInfo);
            break;
        }
    default:
        break;
    }
    return true;
}
// 从平台传来的与道具有关的信息
bool XLogic::OnIPCProp(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)
{
    ASSERT(pHead->wMainCmdID==IPC_MAIN_PROP);
    switch(pHead->wSubCmdID)
    {
    case IPC_SUB_INIT_PROP_FINISH:
        {
            MsgGlbData *pGD = (MsgGlbData*)pIPCBuffer;
            Glb().m_CenterServerPara.m_nEncryptType = pGD->m_nEncryType;
            Glb().m_NewUrl = pGD->m_NewUrl;
            Glb().m_TML_SN = pGD->m_szToken;            
            // 如果传来的数量与原有数量不同，则刷新之
            int nPropCountNew = (int)m_userPropLibrary.GetCount();
            int nPropCountOld = (int)Glb().userPropLibrary.GetCount();
            // 无论是否有更新，都需要进行复制
            for(int i=0; i<nPropCountOld; ++i)
            {
                _TAG_USERPROP *userProp=Glb().userPropLibrary.GetAt(i);
                delete userProp;
                userProp=NULL;
            }
            Glb().userPropLibrary.RemoveAll();
            for (int i=0; i<nPropCountNew; ++i)
            {
                _TAG_USERPROP *userProp=m_userPropLibrary.GetAt(i);
                Glb().userPropLibrary.Add(userProp);
            }
            m_userPropLibrary.RemoveAll();
            // 如果数量不一致，则需要刷新界面
            // PengJiLin, 2010-10-13, 每次都更新界面，所以注释以下部分代码
            //if (nPropCountOld != nPropCountNew)
//{
//    if (m_IGameFrame)
//    {
//        m_IGameFrame->OnInitPropList();
//        //DebugPrintf("刷新道具");
//    }
//}
            //else
            //{
            //	m_IGameFrame->OnInitPropList(1);	// 只更新数量，不更新画面
            //}
            //m_bNeedRefleshProp = false;
            //m_nPropCount = 0;


            m_nPropPage = 0;
            RefreshPropPannel();
            break;
        }
    case IPC_SUB_INIT_PROP:
        {
            //正在传输中
            MSG_PROP_S_GETPROP * msg=(MSG_PROP_S_GETPROP*)pIPCBuffer;
            _TAG_USERPROP *userProp=new _TAG_USERPROP;
            memcpy(userProp,msg,sizeof(_TAG_USERPROP));
            m_userPropLibrary.Add(userProp);
            return true;
        }
    default:
        break;
    }

    return true;
}


//平台传来的道具操作结果
bool XLogic::OnIPCUserPropResult(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)
{
	NetMessageHead * pNetHead = new NetMessageHead;
	pNetHead->bAssistantID = ASS_PROP_RESULT;
	m_pPropPannel->OnNetMessage(pNetHead,(void *)pIPCBuffer,wDataSize);
	return true;
}

//跑马灯消息操作
bool XLogic::OnIPCTrottingMsg(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)
{
	TMailItem* pMailMsg = (TMailItem*)pIPCBuffer;
	m_pPmdWnd->StartPmdLabel(120000,pMailMsg->szMsg,20,true);
	m_pPmdWnd->ShowCtrl();
	return true;
}

//换肤消息 Add by JianGuankun 2012.10.19
bool XLogic::OnExchageSkin(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)
{
	if (FileExists("GameComm",true))
	{
		return false;
	}

	//换肤通知
	MSG_GP_R_LogonResult* _p = (MSG_GP_R_LogonResult*)pIPCBuffer;
    if (_p != NULL)
    {
        CString strUserName("");
	    strUserName.Format("%s", _p->szName);
	    m_skinmgr.SetLogOnUserName(strUserName);
        m_strBcfFileName = m_skinmgr.GetSkinBcfFileName();
    }
	m_skinmgr.Notify();

	wchar_t wszPath[MAX_PATH] = {0};
	wchar_t wszName[MAX_PATH] = {0};
	swprintf(wszPath,L"..\\%sgame",CA2W(m_skinmgr.GetSkinPath()));

	if (m_pShowRightFrameBtn)
	{
		swprintf(wszName,L"%s\\show_bt.png",wszPath);
		m_pShowRightFrameBtn->LoadPic(wszName);
	}

	ISudoku* pSoduku = dynamic_cast<ISudoku*>(m_pIUI->GetIControlbyID(1));
	if(pSoduku)
	{
		swprintf(wszName,L"%s\\GameWnd.png",wszPath);

		char szRcFile[64] = {0};
		_stprintf(szRcFile,"..\\config\\%s",m_skinmgr.GetGameSkinBcfFileName().GetBuffer());
		CBcfFile f(szRcFile);

		RECT rc;

		auto ldGetRect = [&f,&rc](char* key)
		{
			CString strPos;
			strPos = f.GetKeyVal(_T("Sudoku"),key,"0,0,0,0");
			rc.left	  = ::atoi(::strtok(strPos.GetBuffer(),","));
			rc.top	  = ::atoi(::strtok(NULL,","));
			rc.right  = rc.left + ::atoi(::strtok(NULL,","));
			rc.bottom = rc.top +  ::atoi(::strtok(NULL,","));
			return;
		};

		ldGetRect(_T("4_LC"));
		pSoduku->SetPicByIndex(0,wszName,rc);

		ldGetRect(_T("5_RC"));
		pSoduku->SetPicByIndex(1,wszName,rc);

		ldGetRect(_T("2_TC"));
		pSoduku->SetPicByIndex(2,wszName,rc);
 
		ldGetRect(_T("1_TL"));
 		pSoduku->SetPicByIndex(3,wszName,rc);

		ldGetRect(_T("3_TR"));
 		pSoduku->SetPicByIndex(4,wszName,rc);

		ldGetRect(_T("6_BL"));
		pSoduku->SetPicByIndex(5,wszName,rc);

		ldGetRect(_T("8_BR"));
 		pSoduku->SetPicByIndex(6,wszName,rc);

		ldGetRect(_T("7_BC"));
		pSoduku->SetPicByIndex(7,wszName,rc);

	}

	IButton* pBtn = NULL;
	IImage* pImg = NULL;

	RECT rc = {0,0,0,0};
	::GetWindowRect(m_hWnd,&rc);

	int iX = rc.right - rc.left - 350;
	int iTopH = pSoduku ? (pSoduku->GetSrcRect(2).bottom - pSoduku->GetSrcRect(2).top) : 2;

	pImg = dynamic_cast<IImage*>(m_pIUI->GetIControlbyID(IMAGE_NET_SIGNAL));
	if (pImg != NULL)
	{
		swprintf(wszName,L"%s\\game_signal.png",wszPath);

		CString s;
		s.Format("DGameL:%d,%d",iX,(iTopH - pImg->GetPicHeight()/6 - 2)/2);
		OutputDebugString(s);
		pImg->SetGameXY(iX,(iTopH - pImg->GetPicHeight()/6 - 2)/2);
		pImg->LoadPic(wszName);
		pImg->SetGameW(pImg->GetPicWidth());
		pImg->SetGameH(pImg->GetPicHeight()/6);
		pImg->SetSrcW(pImg->GetPicWidth());
		pImg->SetSrcH(pImg->GetPicHeight()/6);
		iX += pImg->GetPicWidth() + 15;
	}

	pBtn = dynamic_cast<IButton*>(m_pIUI->GetIControlbyID(1003500));
	if (pBtn != NULL)
	{
		swprintf(wszName,L"%s\\game_bt01.bmp",wszPath);
		pBtn->SetGameW(pBtn->GetPicWidth()/4);
		pBtn->SetGameH(pBtn->GetPicHeight());
		pBtn->SetGameXY(iX,(iTopH - pBtn->GetPicHeight() - 2)/2-1);
		iX += pBtn->GetPicWidth()/4 + 15;
		pBtn->LoadPic(wszName);
	}

	pBtn = dynamic_cast<IButton*>(m_pIUI->GetIControlbyID(BN_VOLUM_UP));
	if (pBtn != NULL)
	{
		swprintf(wszName,L"%s\\SndVolUp.bmp",wszPath);
		pBtn->SetGameH(pBtn->GetPicHeight());
		pBtn->SetGameXY(iX,(iTopH - pBtn->GetPicHeight() - 2)/2-1);
		pBtn->SetGameW(pBtn->GetPicWidth()/4);
		iX += pBtn->GetPicWidth()/4 + 2;
		pBtn->LoadPic(wszName);
		pBtn->SetControlVisible(true);
		pBtn->SetEnable(true);
	}

	pBtn = dynamic_cast<IButton*>(m_pIUI->GetIControlbyID(BN_VOLUM_ON));
	if (pBtn != NULL)
	{
		swprintf(wszName,L"%s\\SndPlay.bmp",wszPath);
		pBtn->SetGameW(pBtn->GetPicWidth()/4);
		pBtn->SetGameH(pBtn->GetPicHeight());
		pBtn->SetGameXY(iX,(iTopH - pBtn->GetPicHeight() - 2)/2-1);
		pBtn->LoadPic(wszName);
		pBtn->SetControlVisible(false);
		pBtn->SetEnable(true);
	}

	pBtn = dynamic_cast<IButton*>(m_pIUI->GetIControlbyID(BN_VOLUM_OFF));
	if (pBtn != NULL)
	{
		swprintf(wszName,L"%s\\SndPause.bmp",wszPath);
		pBtn->SetGameW(pBtn->GetPicWidth()/4);
		pBtn->SetGameH(pBtn->GetPicHeight());
		pBtn->SetGameXY(iX,(iTopH - pBtn->GetPicHeight() - 2)/2-1);
		iX += pBtn->GetPicWidth()/4 + 2;
		pBtn->LoadPic(wszName);
		pBtn->SetControlVisible(true);
		pBtn->SetEnable(true);
	}

	pBtn = dynamic_cast<IButton*>(m_pIUI->GetIControlbyID(BN_VOLUM_DOWN));
	if (pBtn != NULL)
	{
		swprintf(wszName,L"%s\\SndVolDown.bmp",wszPath);
		pBtn->SetGameW(pBtn->GetPicWidth()/4);
		pBtn->SetGameH(pBtn->GetPicHeight());
		pBtn->SetGameXY(iX,(iTopH - pBtn->GetPicHeight() - 2)/2-1);
		pBtn->LoadPic(wszName);
		pBtn->SetControlVisible(true);
		pBtn->SetEnable(true);
	}

	iX = rc.right - rc.left;

	CBcfFile f(CBcfFile::GetAppPath() + _T("GameOption.bcf"));
	UINT uMaxType = f.GetKeyVal("FrameControl","max_type",1);

	pBtn = dynamic_cast<IButton*>(m_pIUI->GetIControlbyID(BN_CLOSE_FRAME));
	if (pBtn != NULL)
	{
		swprintf(wszName,L"%s\\game_close_bt.bmp",wszPath);
		pBtn->SetGameW(pBtn->GetPicWidth()/4);
		pBtn->SetGameH(pBtn->GetPicHeight());
		pBtn->SetGameXY(iX - pBtn->GetPicWidth()/4,0);
		iX -= pBtn->GetPicWidth()/4;
		pBtn->LoadPic(wszName);
	}

	pBtn = dynamic_cast<IButton*>(m_pIUI->GetIControlbyID(BN_RESET_FRAME));
	if (pBtn != NULL && 0 != uMaxType)
	{
		swprintf(wszName,L"%s\\game_rest_bt.bmp",wszPath);
		pBtn->SetGameW(pBtn->GetPicWidth()/4);
		pBtn->SetGameH(pBtn->GetPicHeight());
		pBtn->SetGameXY(iX - pBtn->GetPicWidth()/4,0);
		pBtn->LoadPic(wszName);
	}

	pBtn = dynamic_cast<IButton*>(m_pIUI->GetIControlbyID(BN_SET_FRAME));
	if (pBtn != NULL && uMaxType != 0)
	{
		swprintf(wszName,L"%s\\game_max_bt.bmp",wszPath);
		pBtn->SetGameW(pBtn->GetPicWidth()/4);
		pBtn->SetGameH(pBtn->GetPicHeight());
		pBtn->SetGameXY(iX - pBtn->GetPicWidth()/4,0);
		iX -= pBtn->GetPicWidth()/4;
		pBtn->LoadPic(wszName);
	}

	if(0 == uMaxType) //没有最大化与还原按钮
	{
		pBtn = dynamic_cast<IButton*>(m_pIUI->GetIControlbyID(BN_RESET_FRAME));
		if (pBtn)
		{
			pBtn->SetControlVisible(false);
		}
		pBtn = dynamic_cast<IButton*>(m_pIUI->GetIControlbyID(BN_SET_FRAME));
		if (pBtn)
		{
			pBtn->SetControlVisible(false);
			iX += pBtn->GetPicWidth()/4;
		}
	}

	pBtn = dynamic_cast<IButton*>(m_pIUI->GetIControlbyID(BN_MIN_FRAME));
	if (pBtn != NULL)
	{
		swprintf(wszName,L"%s\\game_min_bt.bmp",wszPath);
		pBtn->SetGameW(pBtn->GetPicWidth()/4);
		pBtn->SetGameH(pBtn->GetPicHeight());
		pBtn->SetGameXY(iX - pBtn->GetPicWidth()/4,0);
		iX -= pBtn->GetPicWidth()/4;
		pBtn->LoadPic(wszName);
	}

	

	WINDOWPLACEMENT place;
	::GetWindowPlacement(m_pEngine->GetCWnd()->GetSafeHwnd(),&place);
	OnMessageProc(UM_FIXRIGHTFRAME,place.showCmd,0);

	return true;
}

/// UI消息
int XLogic::OnUIMessage(TUIMessage *pMessage)
{
	if (pMessage->_uMessageType==UI_WINDOW_MODE_CHANGE)
	{
		set_videos_pos();
		return 0;
	}
	// 聊天消息
	if (((pMessage->_uControlID == ETEXT_TALK_INPUT && pMessage->_ch == 13 && UI_KEYUP==pMessage->_uMessageType)
		|| (pMessage->_uControlID == ETEXT_TALK_COMBO && pMessage->_ch == 13 && UI_KEYUP==pMessage->_uMessageType)
		|| (pMessage->_uControlID == BN_TALK_SEND && UI_LBUTTONUP==pMessage->_uMessageType))
		/*&& CanTalk()*/)
	{
		ICombox* _pCob = NULL;
		GETCTRL(ICombox,_pCob,m_pIUI,ETEXT_TALK_COMBO);
		if (_pCob != NULL)
		{
			wchar_t *p = _pCob->GetText();
			TCHAR* _Msg = CW2T(p);
			{
				SendTalkMsg(_Msg,0);
				_pCob->TextClear();
			}
		}
		else
		{
			IEditorText* _P=NULL;
			GETCTRL(IEditorText,_P,m_pIUI, ETEXT_TALK_INPUT);

			if (_P != NULL)
			{
				wchar_t *p = _P->GetText();
				TCHAR* _Msg = CW2T(p);
				if (_Msg != NULL)
				{
					SendTalkMsg(_Msg, 0);
					_P->Clear();
				}
			}
		}

		return 0;
	}

	if (pMessage->_uControlID == ETEXT_TALK_COMBO && UI_COMBOBOX_SELITEM == pMessage->_uMessageType)
	{
		ICombox* _pCob = NULL;
		GETCTRL(ICombox,_pCob,m_pIUI,ETEXT_TALK_COMBO);
		if (_pCob != NULL)
		{
			wchar_t *p = _pCob->GetCurSelValue();
			TCHAR* _Msg = CW2T(p);
			{
				SendTalkMsg(_Msg,0);
				_pCob->TextClear();
			}
		}
	}

    if (pMessage->_uControlID>=801 && pMessage->_uControlID<=850 && UI_LBUTTONUP==pMessage->_uMessageType)      // 道具购买容器
    {
		// duanxiaohui 20111103 体验场(免费场) 不能使用道具
		if (m_GameInfo.dwRoomRule & GRR_CONTEST)
		{
			AFCMessageBox("对不起, 比赛场不能使用游戏道具", "温馨提示");
			return 0;
		}
		if (m_GameInfo.dwRoomRule & GRR_EXPERCISE_ROOM)
		{
			AFCMessageBox("对不起, 体验场(免费场)不能使用游戏道具", "温馨提示");
			return 0;
		}
		// end duanxiaohui

        if (pMessage->_uControlID==BN_BUY_VIP)
        {
            if (UI_LBUTTONUP==pMessage->_uMessageType)
            {
                m_pPropPannel->setSkinIndex(0);
                m_pPropPannel->openPannel(0,"", 9999);
            }            
        }
        else
        {
            OnBuyPropUIMessage(pMessage);
        }
        return 0;
    }

    if (BTN_SET==pMessage->_uControlID ||
        (pMessage->_uControlID>=SET_ID_BEGIN && pMessage->_uControlID<=SET_ID_END) )
    {
        OnSetUIMessage(pMessage);
        return 0;
    }
	if (UI_LBUTTONDOWN == pMessage->_uMessageType && RIGHTBOARD_USERLIST==pMessage->_uControlID)
	{
		// 看选中的用户
		IListControl *pList = dynamic_cast<IListControl *>(m_pIUI->GetIControlbyID(RIGHTBOARD_USERLIST));
		if (NULL!=pList)
		{
			m_iSelectIndex = pList->GetSelectRowIndex();
			if (m_iSelectIndex>=0 && m_iSelectIndex<m_ThisDeskPtrArray.GetCount())
			{
				UserItemStruct *pUser = (UserItemStruct *)m_ThisDeskPtrArray.GetAt(m_iSelectIndex);
				if (pUser != NULL/* && pUser->GameUserInfo.dwUserID != GetMeUserInfo()->dwUserID*/)
				{
					ShowCurrentUserInfo(pUser , true) ; 
				}					
			}
		}

	}

    if (UI_RBUTTONDOWN==pMessage->_uMessageType && RIGHTBOARD_USERLIST==pMessage->_uControlID)
	{
		// 看选中的用户
		IListControl *pList = dynamic_cast<IListControl *>(m_pIUI->GetIControlbyID(RIGHTBOARD_USERLIST));
		if (pList == NULL)
		{
			return 0;
		}

		m_iSelectIndex = pList->GetSelectRowIndex();
		if (m_iSelectIndex < 0 || m_iSelectIndex >= m_ThisDeskPtrArray.GetCount())
		{
			return 0;
		}


		if (m_GameInfo.dwRoomRule & GRR_NOTCHEAT) return 0;

		UserItemStruct *pUser = (UserItemStruct *)m_ThisDeskPtrArray.GetAt(m_iSelectIndex);
		IMenu *pMenu = dynamic_cast<IMenu *>(m_pIUI->GetIControlbyID(RIGHTBOARD_USERMENU));

		if (pUser != NULL && pMenu != NULL)
		{
			pMenu->Clear();

			// duanxiaohui 2011-11-14 右键菜单更改
			pMenu->AddString(L"  详细资料");
			pMenu->AddString(L"  复制用户名");

			if (pUser->GameUserInfo.dwUserID != GetMeUserInfo()->dwUserID)
			{
				pMenu->AddString(L"  加为好友");
				pMenu->AddString(L"  加入黑名单");
				pMenu->AddString(L"  请他离开");
			}
			// end duanxiaohui

			//////////////////////////////////////////////////////////////////////////////////
			//UserItemStruct *pUser = (UserItemStruct *)m_ThisDeskPtrArray.GetAt(m_iSelectIndex);
			//char szFor[200]={0};
			//// 用户名                    
			//sprintf(szFor, "  用户名:%s", pUser->GameUserInfo.nickName);
			//pMenu->AddString(CA2W(szFor));
			//// 积分
			//sprintf(szFor, "  积分:%d", pUser->GameUserInfo.dwPoint-GetNormalOrderBasePoint(pUser->GameUserInfo.dwPoint));
			//pMenu->AddString(CA2W(szFor));
			//// 等级                    
			//sprintf(szFor, "  等级:%s", m_GameInfo.pOrderName(pUser->GameUserInfo.dwPoint));
			//pMenu->AddString(CA2W(szFor));
			//// 金币
			//char szMoney[200]={0};
			//GlbGetNumString(szMoney, pUser->GameUserInfo.i64Money, Glb().m_nPowerOfGold, Glb().m_bUseSpace, Glb().m_strSpaceChar);
			//sprintf(szFor, "  金币:%s", szMoney);
			//pMenu->AddString(CA2W(szFor));
			//// 财富级别
			//sprintf(szFor, "  财富级别:%s", GetMoneyOrderName(pUser->GameUserInfo.i64Money));
			//pMenu->AddString(CA2W(szFor));
			//// 总局数
			//int nTotalCount = pUser->GameUserInfo.uMidCount+pUser->GameUserInfo.uWinCount+pUser->GameUserInfo.uLostCount;
			//sprintf(szFor, "  总局数:%d", nTotalCount);
			//pMenu->AddString(CA2W(szFor));
			//// 逃跑率                    
			//sprintf(szFor, "  逃跑率:%f", pUser->GameUserInfo.uCutCount*1.0f/nTotalCount);
			//pMenu->AddString(CA2W(szFor));
			////////////////////////////////////////////////////////////////////////////////////

			pMenu->SetGameXY(pMessage->_mouseX, pMessage->_mouseY);
			pMenu->SetControlVisible(true);
		}
	}
//聊天右键菜单
	if (UI_RBUTTONDOWN==pMessage->_uMessageType && TALK_SHOW==pMessage->_uControlID)
	{
		IMenu *pMenu = NULL;
		GETCTRL(IMenu,pMenu,m_pIUI,TALK_MENU);
		if (pMenu)
		{	
			pMenu->Clear();

			pMenu->AddString(L"  清除屏幕");
			pMenu->AddString(L"  复制");
			if (m_bIsAutoScroll)
			{
				pMenu->AddString(L"√ 允许滚屏");
			} 
			else
			{
				pMenu->AddString(L"  允许滚屏");
			}			

			if (m_bIsAcceptChatMsg)
			{
				pMenu->AddString(L"√ 接收聊天消息");	
			} 
			else
			{
				pMenu->AddString(L"  接收聊天消息");
			}

			pMenu->SetControlVisible(true);

			RECT r;
			GetClientRect(m_pIUI->GetWindowsHwnd(), &r);
			int x, y;
			x = pMessage->_mouseX;
			y = pMessage->_mouseY;
			if (pMessage->_mouseX - pMenu->GetGameW() < 0)
				x = pMenu->GetGameW();
			else if (pMessage->_mouseX + pMenu->GetGameW() > r.right)
				x = r.right - pMenu->GetGameW();
			if (pMessage->_mouseY - pMenu->GetGameH() < 0)
				y = pMenu->GetGameH();
			else if (pMessage->_mouseY + pMenu->GetGameH() > r.bottom)
				y = r.bottom - pMenu->GetGameH();

			pMenu->SetGameXY(x, y);
		}
	}
	if (UI_LBUTTONDOWN==pMessage->_uMessageType && TALK_MENU==pMessage->_uControlID)
	{
		OnTalkMenuMessage(pMessage);
	}

	if (pMessage->_uControlID!=TALK_MENU && UI_LBUTTONDOWN==pMessage->_uMessageType)
	{
		IMenu *pMenu = dynamic_cast<IMenu *>(m_pIUI->GetIControlbyID(TALK_MENU));
		if (NULL!=pMenu)
		{
			pMenu->SetControlVisible(false);
		}
	}

    if (pMessage->_uControlID!=RIGHTBOARD_USERMENU && UI_LBUTTONDOWN==pMessage->_uMessageType)
    {
        IMenu *pMenu = dynamic_cast<IMenu *>(m_pIUI->GetIControlbyID(RIGHTBOARD_USERMENU));
        if (NULL!=pMenu)
        {
            pMenu->SetControlVisible(false);
        }
    }

	if (pMessage->_uControlID == BN_SHOW_RIGHTFRAME && UI_LBUTTONDOWN == pMessage->_uMessageType)
	{
		if (m_pRightFrame->m_uShowState == SW_HIDE)
		{
			m_pRightFrame->m_uShowState = SW_SHOW;
			OnBnClickedRightFrameShow();
			m_pShowRightFrameBtn->SetControlVisible(false);
		}
	}

    if (UI_LBUTTONUP==pMessage->_uMessageType)
    {
        if (BN_CLOSE_FRAME==pMessage->_uControlID)
        {
            if (!m_pGameImpl->OnGameQuiting())
            {
                //if (IDNO==::MessageBox(m_pEngine->GetCWnd()->GetSafeHwnd(), "游戏正在进行，您确定要退出？", "提示", MB_YESNO))
                {
                    return 0;
                }
            }
            QuitGame();
            return 0;
        }  
        else if (BN_MIN_FRAME==pMessage->_uControlID)
        {
            m_pEngine->GetCWnd()->ShowWindow(SW_MINIMIZE);                        
        }
        else if (BN_SET_FRAME==pMessage->_uControlID ||BN_RESET_FRAME == pMessage->_uControlID)
        {
			CBcfFile f(CBcfFile::GetAppPath() + _T("GameOption.bcf"));
			UINT uMaxType = f.GetKeyVal("FrameControl","max_type",1);

			if (uMaxType == 1) //普通最大化
			{
				m_pEngine->SwitchSize();
			}
			else if(uMaxType == 2) 
			{
				DEVMODE lpDevMode;
				lpDevMode.dmBitsPerPel=32;

				if (BN_SET_FRAME==pMessage->_uControlID)//改变分辨率方式的最大化
				{

					lpDevMode.dmPelsWidth = 1024;
					lpDevMode.dmPelsHeight = 768;

				}
				else if (BN_RESET_FRAME == pMessage->_uControlID)
				{
					lpDevMode.dmPelsWidth = m_nWiondwMetricsX;
					lpDevMode.dmPelsHeight = m_nWiondwMetricsY;
				}

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

				SendIpcData(IPC_MAIN_USER, IPC_SUB_SCREEN_SIZE_CHANGE,0,0);
			}
            

			IButton *pBtn = NULL ; 
			if(BN_SET_FRAME==pMessage->_uControlID)
			{
				pBtn = dynamic_cast<IButton *>(m_pIUI->GetIControlbyID(BN_SET_FRAME)) ; 
				if(NULL != pBtn)
				{
					pBtn->SetControlVisible(false) ; 
				}
				pBtn = dynamic_cast<IButton *>(m_pIUI->GetIControlbyID(BN_RESET_FRAME)) ; 
				if(NULL != pBtn)
				{
					pBtn->SetControlVisible(true) ; 
				}
			}
			else
			{
				pBtn = dynamic_cast<IButton *>(m_pIUI->GetIControlbyID(BN_RESET_FRAME)) ; 
				if(NULL != pBtn)
				{
					pBtn->SetControlVisible(false) ; 
				}
				pBtn = dynamic_cast<IButton *>(m_pIUI->GetIControlbyID(BN_SET_FRAME)) ; 
				if(NULL != pBtn)
				{
					pBtn->SetControlVisible(true) ; 
				}
			}

			///最大化和最小化是隐藏菜单中的玩家信息
			IMenu *pMenu = dynamic_cast<IMenu *>(m_pIUI->GetIControlbyID(RIGHTBOARD_USERMENU));
			if (NULL!=pMenu)
			{
				pMenu->SetControlVisible(false);
			}
        }
        else if (203==pMessage->_uControlID)
        {
            // 弹出表情框
            UIShowBiaoQingKuang(pMessage);
            return 0;
        }
        else if (pMessage->_uControlID>=IMG_BIAOQING_0 && pMessage->_uControlID<=IMG_BIAOQING_49 )
        {
            // 单击表情
            UIHitBiaoQing(pMessage);
            return 0;
        }        
        else if (BN_START_GAME==pMessage->_uControlID)
        {
            SendGameData(ASS_GM_AGREE_GAME,NULL, 0);
            return 0;
        }
        else if (RIGHTBOARD_PROP_BTN_NEXT1==pMessage->_uControlID || RIGHTBOARD_PROP_BTN_NEXT2==pMessage->_uControlID) 
        {
            m_nPropPage++;
            RefreshPropPannel();
            return 0;
        }
        else if (RIGHTBOARD_PROP_BTN_PRE1==pMessage->_uControlID || RIGHTBOARD_PROP_BTN_PRE2==pMessage->_uControlID)
        {
            m_nPropPage--;
            RefreshPropPannel();
            return 0;
        }
        else if (pMessage->_uControlID>=PROP_BTN_BEGIN && pMessage->_uControlID<PROP_BTN_END)
        {
			if (m_GameInfo.dwRoomRule & GRR_CONTEST)
			{
				AFCMessageBox("对不起,  比赛场不能使用游戏道具", "温馨提示");
				return 0;
			}
            // 算出单击了哪一个
            int nIndex = (pMessage->_uControlID-PROP_BTN_BEGIN)/PROP_IDS;

            OnHitPropBtn(nIndex);
            return 0;
        }
        else if (BN_DLALOG_OK==pMessage->_uControlID
            || BN_DLALOG_CANCEL==pMessage->_uControlID
            || BN_DLALOG_CLOSE==pMessage->_uControlID)
        {
            IContain *pContain = dynamic_cast<IContain *>(m_pIUI->GetIControlbyID(CTN_DIALOG));
            if (NULL!=pContain)
            {
                pContain->SetControlVisible(false);
            }
        }        
        else if (BN_VOLUM_UP==pMessage->_uControlID)
        {
            if (m_pBackAudio)
            {
                int nVol=m_pBackAudio->GetVolume();
                nVol+=5;
                if (nVol>100)
                {
                    nVol = 100;
                }
                m_pBackAudio->SetVolume(nVol);
            }
        }        
        else if (BN_VOLUM_DOWN==pMessage->_uControlID)
        {
            if (m_pBackAudio)
            {
                int nVol=m_pBackAudio->GetVolume();
                nVol-=5;
                if (nVol<0)
                {
                    nVol = 0;
                }
                m_pBackAudio->SetVolume(nVol);
            }
            
        }        
        else if (BN_VOLUM_OFF==pMessage->_uControlID)
        {
            if (m_pBackAudio)
            {
                m_pBackAudio->Pause();
                IButton *pSoundOn = dynamic_cast<IButton *>(m_pIUI->GetIControlbyID(BN_VOLUM_ON));
                IButton *pSoundOFF = dynamic_cast<IButton *>(m_pIUI->GetIControlbyID(BN_VOLUM_OFF));
                if (pSoundOn && pSoundOFF)
                {
                    pSoundOn->SetControlVisible(true);
                    pSoundOFF->SetControlVisible(false);
                }
            }
        }        
        else if (BN_VOLUM_ON==pMessage->_uControlID)
        {
            if (m_pBackAudio)
            {   
                m_pBackAudio->Continue();
                IButton *pSoundOn = dynamic_cast<IButton *>(m_pIUI->GetIControlbyID(BN_VOLUM_ON));
                IButton *pSoundOFF = dynamic_cast<IButton *>(m_pIUI->GetIControlbyID(BN_VOLUM_OFF));
                if (pSoundOn && pSoundOFF)
                {
                    pSoundOn->SetControlVisible(false);
                    pSoundOFF->SetControlVisible(true);
                }
            }
        }
		else if (pMessage->_uControlID == RIGHTBOARD_USERMENU)
		{
			IMenu *pMenu;
			GETCTRL(IMenu, pMenu, m_pIUI, RIGHTBOARD_USERMENU);
			int iSel = pMenu->GetSelectIndex();
			if (iSel >= 0)
			{
				UserItemStruct *pUser = (UserItemStruct *)m_ThisDeskPtrArray.GetAt(m_iSelectIndex);
				if (pUser == NULL)
				{
					return 0;
				}

				// duanxiaohui 2011-11-14 右键菜单更改
				if (!wcscmp(pMenu->GetItemText(iSel), L"  详细资料"))
				{	
					if (m_GameInfo.dwRoomRule & GRR_NOTCHEAT) return 0;
					m_pUserDetailsDlg->SetUserItem(pUser);
				
					IListControl *pList = dynamic_cast<IListControl *>(m_pIUI->GetIControlbyID(RIGHTBOARD_USERLIST));
					if (pList != NULL)
					{
						POINT point;
						::GetCursorPos(&point);

						CRect rect;
						m_pUserDetailsDlg->GetClientRect(&rect);
						m_pUserDetailsDlg->MoveWindow(point.x - rect.Width(), point.y, rect.Width(), rect.Height());
						m_pUserDetailsDlg->ShowWindow(SW_SHOW);
					}
				}
				else if (!wcscmp(pMenu->GetItemText(iSel), L"  复制用户名"))
				{
					if (OpenClipboard(NULL) == FALSE)
					{	
						return TRUE;
					}

					if (EmptyClipboard() == FALSE) 
					{
						CloseClipboard();
						return TRUE;
					}

					// 复制数据
					HANDLE hData = GlobalAlloc(GMEM_MOVEABLE, sizeof(pUser->GameUserInfo.nickName));
					if (hData == NULL) 
					{
						CloseClipboard();
						return TRUE;
					}

					LPTSTR szMemName = (LPTSTR)GlobalLock(hData);

					// duanxiaohui 2011-11-21 复制到粘贴板中需要转换成Unicode
					//lstrcpy(szMemName, pUser->GameUserInfo.nickName);

					CString strText;
					strText.Format("%s", pUser->GameUserInfo.nickName);
					if (0 == MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)strText, strText.GetLength() + 1, (WCHAR*)szMemName,  1024)) 
					{ 
						GlobalUnlock(hData); 
						CloseClipboard(); 
						GlobalFree(hData); 
						return TRUE; 
					} 
					// end duanxiaohui


					SetClipboardData(CF_UNICODETEXT, hData);
					GlobalUnlock(hData); 
					GlobalFree(hData);
					CloseClipboard();

					// 显示名字
					IEditorText* _P = NULL;
					GETCTRL(IEditorText, _P, m_pIUI, ETEXT_TALK_INPUT);
					if (_P != NULL)
					{
						_P->SetText(CA2W(pUser->GameUserInfo.nickName));
					}
				}
				else if (!wcscmp(pMenu->GetItemText(iSel), L"  加为好友"))
				{
					m_pIPC->SendData(IPC_FRIEND, IPC_ASS_CHECKFRIEND, &(pUser->GameUserInfo.dwUserID), sizeof(int));
				}
				else if (!wcscmp(pMenu->GetItemText(iSel), L"  加入黑名单"))
				{
					m_pIPC->SendData(IPC_EMENY, IPC_ASS_CHECKEMENY, &(pUser->GameUserInfo.dwUserID), sizeof(int));
				}
				else if (!wcscmp(pMenu->GetItemText(iSel), L"  请他离开"))
				{
					long dwCurrentUserID = pUser->GameUserInfo.dwUserID ; 
					if (dwCurrentUserID!=0L)
					{
						//wushuqun 2009.5.19
						//旁观时不能T人
						if (GetMeUserInfo()->bUserState == USER_WATCH_GAME)
						{
							AFCMessageBox("对不起，您正在旁观游戏，不能进行此操作", "提示");
							return true;
						}

						if(GetMeUserInfo()->dwUserID == dwCurrentUserID)
						{
							AFCMessageBox("对不起,无法对自己进行此项操作","提示");
							return true;
						}

						CString stime;
						stime.Format("%d",CTime::GetCurrentTime());        //当前的时间，String格式
						int curtime=atoi(stime);                        //转换为int
						//if(GetMeUserInfo()->iVipTime < curtime)
						//{
						//	AFCMessageBox("对不起,只有vip才有此项功能","提示");
						//	return true;
						//}
						///检查是否为旁观 add by rmaon 09.04.27
						if( m_bWatchMode )
						{
							AFCMessageBox("对不起,旁观状态不能踢人","提示");
							return true;
						}
						//处理数据
						UserItemStruct * pUserItem=FindOnLineUser(dwCurrentUserID);
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
							stKick.dwDestID = dwCurrentUserID;
							SendGameData(&stKick,sizeof(stKick),MDM_GR_PROP,ASS_PROP_NEW_KICKUSER,0);
						}
						else
						{
							_TAG_KICKUSER stKick;
							stKick.dwUserID = GetMeUserInfo()->dwUserID;
							stKick.dwDestID = dwCurrentUserID;
							SendGameData(&stKick,sizeof(stKick),MDM_GR_PROP,ASS_PROP_KICKUSER,0);
						}
					}
					
				}
			}
		}
    }

	//动画消息FF
	if(pMessage->_uMessageType == UI_CATOONFINISH)
	{
		if (pMessage->_uControlID >= USERPROP_ANIMAL_0 && pMessage->_uControlID <= USERPROP_ANIMAL_0+10)
		{
			CString strFilePathName; 
			strFilePathName.Format("..\\image\\prop\\PropGif%dB.bani",m_stProp.iValue%1000); //modify by lxl  图片格式修改gif->spr

			//送道俱位置结束
			if (DeskStation2View(m_stProp.sendDeskStation) == pMessage->_uControlID - USERPROP_ANIMAL_0)
			{
				PlayProp(false,strFilePathName);
			}
		}
	}
    m_pGameImpl->OnUIMessage(pMessage);
    return 0;
}

void XLogic::PlayProp(const bool& isSend,const CString& strFilePathName)
{
	IExAnimate* pExAnimate=NULL;
	GETCTRL(IExAnimate,pExAnimate,m_pIUI,
		USERPROP_ANIMAL_0+DeskStation2View((isSend) ? m_stProp.sendDeskStation : m_stProp.recvDeskStation));
	
	if (NULL != pExAnimate)
	{
		pExAnimate->SetExtFilePath(CA2W(strFilePathName));
		pExAnimate->LoadFromFile();
		pExAnimate->SetPlayState(1);
		pExAnimate->SetIsSymmetrical(false);
		pExAnimate->SetIsLoop(false) ; 
		pExAnimate->SetControlVisible(true);
	}
}

/// 时钟
void XLogic::OnTimer(int nEventID)
{
    if (OLEDRAW_TIMER_ID==nEventID)
    {
        IWeb *pweb = dynamic_cast<IWeb *>(m_pIUI->GetIControlbyID(BUYPROP_WEB_PROPINFO));
        if (pweb!=NULL)
        {
            pweb->Draw();
        }
        
        return;
    }
    m_pGameImpl->OnTimer(nEventID);
}




// 初始化表情框
void XLogic::UIInitBiaoQingKuang()
{
    // 表情框
    IContain *pContain = NULL;
    GETCTRL(IContain, pContain, m_pIUI, CTN_BIAOQING_KUANG);
    if (NULL==pContain)
    {
        return;
    }
    pContain->SetControlVisible(false);
    // 表情背景
    IImage *pImgBK = NULL;
    GETCTRL(IImage, pImgBK, m_pIUI, IMG_BIAOQING_BJ);
    if (NULL==pImgBK)
    {
        return;
    }
    // 表情ID
    int nBiaoQingID[50];
    // 表情图片 
    IImage *pBiaoQingImg[50];
    // 获取表情图片
    for (int i=0; i<50; ++i)
    {
        nBiaoQingID[i] = IMG_BIAOQING_0+i;
        pBiaoQingImg[i] = NULL;
        GETCTRL(IImage, pBiaoQingImg[i], m_pIUI, nBiaoQingID[i]);
        if (NULL==pBiaoQingImg[i])
        {
            return;
        }
    }    
    // 第一个表情的位置（左上角）
    int nStartX = pBiaoQingImg[0]->GetGameX();
    int nStartY = pBiaoQingImg[0]->GetGameY();

    int nOffsetX = pImgBK->GetPicWidth()/10;
    int nOffsetY = nOffsetX;//pImgBK->GetPicHeight()/5;

    int nSrcOffsetX = pBiaoQingImg[0]->GetPicWidth()/10;
    int nSrcOffsetY = nSrcOffsetX;//pBiaoQingImg[0]->GetPicHeight()/5;

    // 初始化各表情图片位置
    for (int i=1; i<50; ++i)
    {
        pBiaoQingImg[i]->SetGameXY(nStartX+(i%10)*nOffsetX, nStartY+(i/10)*nOffsetY);
        pBiaoQingImg[i]->SetSrcX(nSrcOffsetX*(i%10));
        pBiaoQingImg[i]->SetSrcY(nSrcOffsetY*(i/10));
    }

}
// 弹出表情框
void XLogic::UIShowBiaoQingKuang(TUIMessage *pMessage)
{
    // 表情框
    static IContain *pContain = NULL;
    if (NULL==pContain)
    {
        GETCTRL(IContain, pContain, m_pIUI, CTN_BIAOQING_KUANG);
        if (NULL==pContain)
        {
            return;
        }
    }

    pContain->SetControlVisible(!pContain->GetVisible());
}

// 单击表情
void XLogic::UIHitBiaoQing(TUIMessage *pMessage)
{
    // 表情Index
    int nIndex = pMessage->_uControlID-IMG_BIAOQING_0;

    IEditorText* _P=NULL;
    GETCTRL(IEditorText,_P,m_pIUI, ETEXT_TALK_INPUT);
    TCHAR szText[255];
    TCHAR* _Msg = NULL;
    if (_P != NULL)
    {
        _Msg = CW2T(_P->GetText()); 
    }

    if (NULL==_Msg)
    {
        _stprintf(szText, TEXT("/:%3d"), nIndex);
        SendTalkMsg(szText, 0);
        //_P->SetText(CA2W(szText));
    }
    else
    {
        _stprintf(szText, TEXT("%s/:%3d"), _Msg, nIndex);
        SendTalkMsg(szText, 0);
        _P->Clear();
        //_P->SetText(CA2W(szText));        
    }

    // 表情框
    UIShowBiaoQingKuang(NULL);
}

void XLogic::SendTalkMsg(TCHAR* pMsg, int iUserID,int nTalkIdx/* = 0*/)
{
	if (!pMsg)
	{
		return;
	}

	if (strlen(pMsg) == 0)
	{
		return;
	}

	m_pRightFrame->SetFocusToTalkInput();
	if (m_pRightFrame->GetTalkInputMessage(NULL) == 0)
	{
		if (NULL == m_pTalkCtn)
		{
			return;
		}
	}

	//禁止聊天的房间不发送禁止游戏聊天
	if (m_GameInfo.dwRoomRule & GRR_FORBID_GAME_TALK)
	{
		InsertSystemMessage(TEXT("抱歉，本房间禁止聊天"));
		return;
	}

	//判断是否可以聊天
	if (CUserPower::CanGameTalk(m_GameInfo.dwGamePower)==false) 
	{
		InsertSystemMessage( TEXT("抱歉，你暂时没有聊天的权利!"));
		return;
	}
    //发送聊天信息
    MSG_GR_RS_NormalTalk Talk;
    ZeroMemory(&Talk,sizeof(MSG_GR_RS_NormalTalk));
    Talk.iLength=(short)strlen(pMsg);
    Talk.dwSendID=GetMeUserInfo()->dwUserID;
    Talk.dwTargetID=iUserID;
    Talk.nDefaultIndex=nTalkIdx;
    strcpy(Talk.szMessage,pMsg);
    //Talk.crColor=m_colorresource.m_crNormalTxColor;
    SendGameData(&Talk,sizeof(Talk)-sizeof(Talk.szMessage)+Talk.iLength+1,MDM_GM_GAME_FRAME,ASS_GM_NORMAL_TALK,0);
}

int XLogic::ShowOneUserInfo(int nControlIndex,UserItemStruct * pUserInfo,bool bVisible)
{
	UserInfoStruct *pMeInfo = GetMeUserInfo();
	if (pUserInfo == NULL || pMeInfo == NULL) return 0;
	if (pUserInfo->GameUserInfo.dwUserID == pMeInfo->dwUserID)//(nControlIndex==m_GameInfo.uDeskPeople/2)        // 是自己
    {
        UserInfoStruct *pMeInfo = GetMeUserInfo();

        IText *pTextName = NULL;
        IText *pTextID = NULL;
        IText *pTextJifen = NULL;
        IText *pTextJingbi = NULL;
        IText *pTextMeili = NULL;
        IText *pTextDenji = NULL;
        GETCTRL(IText, pTextName, m_pIUI, RIGHTBOARD_TEXT_NICKNAME);
        GETCTRL(IText, pTextID, m_pIUI,    RIGHTBOARD_TEXT_ID    ); 
        GETCTRL(IText, pTextJifen, m_pIUI, RIGHTBOARD_TEXT_JIFEN );
        GETCTRL(IText, pTextJingbi, m_pIUI,RIGHTBOARD_TEXT_JINGBI);
        GETCTRL(IText, pTextMeili, m_pIUI, RIGHTBOARD_TEXT_MEILI );
        GETCTRL(IText, pTextDenji, m_pIUI, RIGHTBOARD_TEXT_DENJI );

		//读取显示用户信息配置文件zht2011-11-07 zht修改为可配置
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

        
        char path[MAX_PATH]={0};
		int y = 0;
		int x = 0;
        if (pTextName!=NULL)
        {
			if (bNickName)
			{
				sprintf(path,"%s%s",szNickName,pMeInfo->nickName);
				pTextName->SetText(CA2W(path));
				y = pTextName->GetGameY();
				x = pTextName->GetGameX();
				y += 18;
			}
			else if(bUserName)
			{
				sprintf(path,"%s%s",szUserName,pMeInfo->szName);
				pTextName->SetText(CA2W(path));
				y = pTextName->GetGameY();
				x = pTextName->GetGameX();
				y += 18;
			}
			else
			{
				pTextName->SetControlVisible(false);
			}
		
        }
        if (pTextID!=NULL)
		{
			if (bUserID)
			{
				sprintf(path,"%s%d",szUserID,pMeInfo->dwUserID);
				pTextID->SetText(CA2W(path));
				if (y>0)
				{
					pTextID->SetGameXY(x,y);
					y += 18;
				}
				else
				{
					y = pTextID->GetGameY();
					x = pTextID->GetGameX();
					pTextID->SetGameXY(x,y);
					y += 18;
				}
			}
			else
			{
				pTextID->SetControlVisible(false);
			}
        }
		if (pTextJifen!=NULL)
		{
			if (bScore)
			{
				if (((m_GameInfo.dwRoomRule & GRR_NOTCHEAT) && pUserInfo->GameUserInfo.dwUserID != GetMeUserInfo()->dwUserID) || m_GameInfo.dwRoomRule & GRR_CONTEST)
				{
					sprintf(path,"%s???",szScore);
				}
				else
				{
					sprintf(path,"%s%d",szScore,pMeInfo->dwPoint);
				}
				
				pTextJifen->SetText(CA2W(path));
				if (y>0)
				{
					pTextJifen->SetGameXY(x,y);
					y += 18;
				}
				else
				{
					y = pTextJifen->GetGameY();
					x = pTextJifen->GetGameX();
					pTextJifen->SetGameXY(x,y);
					y += 18;
				}
			}
			else
			{
				pTextJifen->SetControlVisible(false);
			}
        }
        if (pTextJingbi!=NULL)
        {
            /*CString strMoneyName;
            static bool bConfigReaded = false;
            if (!bConfigReaded)
            {
                CString szAppPath = CINIFile::GetAppPath();
                szAppPath += "/GameOption.bcf";
                CBcfFile f(szAppPath.GetBuffer());
                strMoneyName=f.GetKeyVal("NewGameFrame","MoneyName","金币");
				}*/
			if (bCoin)
			{
				GlbGetNumString(path, pMeInfo->i64Money, Glb().m_nPowerOfGold, Glb().m_bUseSpace, TEXT(","));
				char sz[300]={0};
				sprintf(sz,"%s%s",szCoin.GetBuffer(),path);
				if (((m_GameInfo.dwRoomRule & GRR_NOTCHEAT) && pUserInfo->GameUserInfo.dwUserID != GetMeUserInfo()->dwUserID) || m_GameInfo.dwRoomRule & GRR_CONTEST)
				{
					sprintf(sz,"%s%s",szCoin.GetBuffer(),"???");
				}
				pTextJingbi->SetText(CA2W(sz));
				if (y>0)
				{
					pTextJingbi->SetGameXY(x,y);
					y += 18;
				}
				else
				{
					y = pTextJingbi->GetGameY();
					x = pTextJingbi->GetGameX();
					pTextJingbi->SetGameXY(x,y);
					y += 18;
				}
			}
			else
			{
				pTextJingbi->SetControlVisible(false);
			}
        }
        if (pTextMeili!=NULL)
        {
			if (bCharm)
			{
				if (((m_GameInfo.dwRoomRule & GRR_NOTCHEAT) && pUserInfo->GameUserInfo.dwUserID != GetMeUserInfo()->dwUserID) || m_GameInfo.dwRoomRule & GRR_CONTEST)
				{
					sprintf(path,"%s???",szCharm);
				}
				else
				{
					sprintf(path,"%s%d",szCharm,pMeInfo->dwFascination);
				}
				
				pTextMeili->SetText(CA2W(path));
				if (y>0)
				{
					pTextMeili->SetGameXY(x,y);
					y += 18;
				}
				else
				{
					y = pTextMeili->GetGameY();
					x = pTextMeili->GetGameX();
					pTextMeili->SetGameXY(x,y);
					y += 18;
				}
			}
			else
			{
				pTextMeili->SetControlVisible(false);
			}
		}
        if (pTextDenji!=NULL)
        {
			if (bWealthLevel)
			{
				if (((m_GameInfo.dwRoomRule & GRR_NOTCHEAT) && pUserInfo->GameUserInfo.dwUserID != GetMeUserInfo()->dwUserID) || m_GameInfo.dwRoomRule & GRR_CONTEST)
				{
					sprintf(path,"%s???",szWealthLevel);
				}
				else
				{
					sprintf(path,"%s%s",szWealthLevel,GetMoneyOrderName(pMeInfo->i64Money));
				}
				
				pTextDenji->SetText(CA2W(path));
				if (y>0)
				{
					pTextDenji->SetGameXY(x,y);
					y += 18;
				}
				else
				{
					y = pTextDenji->GetGameY();
					x = pTextDenji->GetGameX();
					pTextDenji->SetGameXY(x,y);
					y += 18;
				}
			}
			else
			{
				pTextDenji->SetControlVisible(false);
			}
        }
    }


    char path[MAX_PATH]={0};
    ///等级
    IText *pText;
    GETCTRL(IText,pText,m_pIUI,TEXT_LEV+nControlIndex);
    if(pText!=NULL)
    {
        sprintf(path,"%d",0);
        pText->SetText(CA2W(path));
        pText->SetFontShowStyle(AlignmentCenter);
        char tip[512];
        memset(tip, 0, 512 * sizeof(TCHAR));
       /* sprintf(tip, "<font><color=0,size=16,style=楷书>玩家等级</font>");
        pText->SetTooltip(CA2W(tip));*/
        pText->SetControlVisible(bVisible);
    }

    ///帐号
    GETCTRL(IText,pText,m_pIUI,TEXT_ACCOUNT+nControlIndex);
    if(pText!=NULL)
    {
		if (((m_GameInfo.dwRoomRule & GRR_NOTCHEAT) && pUserInfo->GameUserInfo.dwUserID != GetMeUserInfo()->dwUserID))
		{
			 sprintf(path,"玩家",pUserInfo->GameUserInfo.dwUserID);
		}
		else
		{
			 sprintf(path,"%d",pUserInfo->GameUserInfo.dwUserID);
		}
       
        pText->SetText(CA2W(path));
        pText->SetFontShowStyle(AlignmentCenter);
        wchar_t tip[512];
        memset(tip, 0, 512 * sizeof(TCHAR));
        /*wsprintfW(tip, L"<font><color=0,size=16,style=楷书>玩家帐号</font>");
        pText->SetTooltip(tip);*/
        pText->SetControlVisible(bVisible);
    }

	if (m_GameInfo.uDeskPeople > 10)
	{
		return 0; //大于10人的不更新
	}
    ///昵称
    GETCTRL(IText,pText,m_pIUI,TEXT_NICKNAME+nControlIndex);
    if(pText!=NULL)
    {
		if (((m_GameInfo.dwRoomRule & GRR_NOTCHEAT) && pUserInfo->GameUserInfo.dwUserID != GetMeUserInfo()->dwUserID))
		{
			 sprintf(path,"玩家");
		}
		else
		{
			 sprintf(path,"%s",pUserInfo->GameUserInfo.nickName);
		}
      
        pText->SetText(CA2W(path));
        pText->SetFontShowStyle(AlignmentCenter);
        pText->SetControlVisible(bVisible);
    }
    ///昵称底图
    IImage * pImage=NULL;
    GETCTRL(IImage,pImage,m_pIUI,IMAGE_NICKNAME_BK+nControlIndex);
    if(pImage!=NULL)
    {
        pImage->SetControlVisible(bVisible);
    }

    //const TCHAR C_LogoSex[2][8] = {TEXT("boy"), TEXT("girl")};
    //const TCHAR C_LogoName[2][8] = {TEXT("left"), TEXT("right")};

    
    //人物头像
    CString szAppPath = CINIFile::GetAppPath();
    szAppPath.SetAt(szAppPath.GetLength()-1, TCHAR('\0'));
    int nPosOfSlash = szAppPath.ReverseFind(TCHAR('\\'));
    CString szAppPathParent = szAppPath.Left(nPosOfSlash);
    TCHAR szBuffer[200]={0};

	int iSitGrade = 0;
	iSitGrade = BzGetAvatarIndex(pUserInfo);

	wsprintf(szBuffer,"..\\image\\log\\UserLogos\\Logo_%d.png",pUserInfo->GameUserInfo.bLogoID);
	
    //人物头像
    IImage * pImageLogo=NULL;
    GETCTRL(IImage,pImageLogo,m_pIUI,IMAGE_LOGO+nControlIndex);
    if(pImageLogo!=NULL)
    {
        pImageLogo->LoadPic(CT2W(szBuffer));
        pImageLogo->SetControlVisible(bVisible);
    }
    IImage *pImageLogoFrame = NULL;
    GETCTRL(IImage, pImageLogoFrame, m_pIUI, IMAGE_LOGO_FRAME+nControlIndex);
    if (pImageLogoFrame!=NULL)
    {
        pImageLogoFrame->SetControlVisible(bVisible);
    }

    //int nWhichSide = 0;
    //if (nControlIndex<(m_GameInfo.uDeskPeople/2))
    //{
    //    nWhichSide = 1;
    //}
    //IExAnimate *pExAmimate=NULL;
    //GETCTRL(IExAnimate,pExAmimate,m_pIUI,IMAGE_LOGO+nControlIndex);
    //if(pExAmimate)
    //{
    //    pExAmimate->SetPlayState(bVisible);
    //    if (bVisible)
    //    {
    //        int index = pUserInfo->bBoy? 0: 1;
    //        int nWhichSide = (nControlIndex<(m_GameInfo.uDeskPeople/2))? 1: 0;
    //        wsprintf(path, _T("client\\anim\\%s_%d%s.bani"), C_LogoSex[index], pUserInfo->iImageNO % GAME_LOG_COUNT, C_LogoName[nWhichSide]);
    //        pExAmimate->SetExtFilePath(path);
    //        pExAmimate->LoadFromFile();
    //        pExAmimate->SetCurveInstance(CT_NONE);//不动
    //        pExAmimate->SetIsLoop(true);//循环播放
    //    }

    //}


    ///金币1
    GETCTRL(IText,pText,m_pIUI,TEXT_MONEY1+nControlIndex);
    if(pText!=NULL)
    {
		GlbGetNumString(path, pUserInfo->GameUserInfo.i64Money, Glb().m_nPowerOfGold, Glb().m_bUseSpace, TEXT(","));

		if(pUserInfo->GameUserInfo.dwUserID != GetMeUserInfo()->dwUserID)
		{
			if (m_GameInfo.dwRoomRule & GRR_NOTCHEAT || m_GameInfo.dwRoomRule & GRR_CONTEST)
			{
				sprintf(path,"???");
			}
		}
		else
		{
			if (m_GameInfo.dwRoomRule & GRR_CONTEST)
			{
				GlbGetNumString(path, pUserInfo->GameUserInfo.i64ContestScore, Glb().m_nPowerOfGold, Glb().m_bUseSpace, TEXT(","));
			}
		}

		CString str;
		str.Format("DClient:path = %s",path);
		OutputDebugString(str);

        pText->SetText(CA2W(path));
        pText->SetFontShowStyle(AlignmentCenter);
        char tip[512];
        memset(tip, 0, 512 * sizeof(TCHAR));
       /* sprintf(tip, "<font><color=0,size=16,style=楷书>玩家金币</font>");
        pText->SetTooltip(CA2W(tip));*/
        pText->SetControlVisible(bVisible);
    }

    ///金币2
    GETCTRL(IText,pText,m_pIUI,TEXT_MONEY2+nControlIndex);
    if(pText!=NULL)
    {
		GlbGetNumString(path, pUserInfo->GameUserInfo.i64Money, Glb().m_nPowerOfGold, Glb().m_bUseSpace, TEXT(","));

		if(pUserInfo->GameUserInfo.dwUserID != GetMeUserInfo()->dwUserID)
		{
			if (m_GameInfo.dwRoomRule & GRR_NOTCHEAT || m_GameInfo.dwRoomRule & GRR_CONTEST)
			{
				sprintf(path,"???");
			}
		}
		else
		{
			if (m_GameInfo.dwRoomRule & GRR_CONTEST)
			{
				GlbGetNumString(path, pUserInfo->GameUserInfo.i64ContestScore, Glb().m_nPowerOfGold, Glb().m_bUseSpace, TEXT(","));
			}
		}

		CString str;
		str.Format("DClient:path = %s",path);
		OutputDebugString(str);
// 		if (m_GameInfo.dwRoomRule & GRR_CONTEST)
// 		{
// 			
// 		}
        
        pText->SetText(CA2W(path));
        pText->SetFontShowStyle(AlignmentCenter);
        char tip[512];
        memset(tip, 0, 512 * sizeof(TCHAR));
      /*  sprintf(tip, "<font><color=0,size=16,style=楷书>玩家元宝</font>");
        pText->SetTooltip(CA2W(tip));*/
        pText->SetControlVisible(bVisible);
    }

   // RefreshPlayList();

    return 0;
}
int XLogic::ShowCurrentUserInfo(UserItemStruct * pUserInfo,bool bVisible)
{
	UserInfoStruct *pMeInfo = &pUserInfo->GameUserInfo;

	if (pUserInfo == NULL || pMeInfo == NULL) return 0;
	if (pUserInfo->GameUserInfo.dwUserID == pMeInfo->dwUserID)//(nControlIndex==m_GameInfo.uDeskPeople/2)        // 是自己
	{
		IText *pTextName = NULL;
		IText *pTextID = NULL;
		IText *pTextJifen = NULL;
		IText *pTextJingbi = NULL;
		IText *pTextMeili = NULL;
		IText *pTextDenji = NULL;
		GETCTRL(IText, pTextName, m_pIUI, RIGHTBOARD_TEXT_NICKNAME);
		GETCTRL(IText, pTextID, m_pIUI,    RIGHTBOARD_TEXT_ID    ); 
		GETCTRL(IText, pTextJifen, m_pIUI, RIGHTBOARD_TEXT_JIFEN );
		GETCTRL(IText, pTextJingbi, m_pIUI,RIGHTBOARD_TEXT_JINGBI);
		GETCTRL(IText, pTextMeili, m_pIUI, RIGHTBOARD_TEXT_MEILI );
		GETCTRL(IText, pTextDenji, m_pIUI, RIGHTBOARD_TEXT_DENJI );

		//读取显示用户信息配置文件zht2011-11-07 zht修改为可配置
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


		char path[MAX_PATH]={0};
		int y = 0;
		int x = 0;
		if (pTextName!=NULL)
		{
			if (bNickName)
			{
				sprintf(path,"%s%s",szNickName,pMeInfo->nickName);
				if ((m_GameInfo.dwRoomRule & GRR_NOTCHEAT) && pUserInfo->GameUserInfo.dwUserID != GetMeUserInfo()->dwUserID)//防作弊场:不是自己不显示积分 lym 2011- 12-1
				{			
					sprintf(path,"%s玩家",szNickName);
				}
				pTextName->SetText(CA2W(path));
				y = pTextName->GetGameY();
				x = pTextName->GetGameX();
				y += 18;
			}
			else if(bUserName)
			{
				if ((m_GameInfo.dwRoomRule & GRR_NOTCHEAT) && pUserInfo->GameUserInfo.dwUserID != GetMeUserInfo()->dwUserID)//防作弊场:不是自己不显示积分 lym 2011- 12-1
				{			
					sprintf(path,"%s玩家",szNickName);
				}
				sprintf(path,"%s%s",szUserName,pMeInfo->szName);
				pTextName->SetText(CA2W(path));
				y = pTextName->GetGameY();
				x = pTextName->GetGameX();
				y += 18;
			}
			else
			{
				pTextName->SetControlVisible(false);
			}

		}
		if (pTextID!=NULL)
		{
			if (bUserID)
			{
				sprintf(path,"%s%d",szUserID,pMeInfo->dwUserID);
				if (((m_GameInfo.dwRoomRule & GRR_NOTCHEAT) && pUserInfo->GameUserInfo.dwUserID != GetMeUserInfo()->dwUserID))//防作弊场:不是自己不显示积分 lym 2011- 12-1
				{			
					sprintf(path,"%s???",szUserID);
				}
				pTextID->SetText(CA2W(path));
				if (y>0)
				{
					pTextID->SetGameXY(x,y);
					y += 18;
				}
				else
				{
					y = pTextID->GetGameY();
					x = pTextID->GetGameX();
					pTextID->SetGameXY(x,y);
					y += 18;
				}
			}
			else
			{
				pTextID->SetControlVisible(false);
			}
		}
		if (pTextJifen!=NULL)
		{
			if (bScore)
			{
				sprintf(path,"%s%d",szScore,pMeInfo->dwPoint);
				if (((m_GameInfo.dwRoomRule & GRR_NOTCHEAT) && pUserInfo->GameUserInfo.dwUserID != GetMeUserInfo()->dwUserID) || m_GameInfo.dwRoomRule & GRR_CONTEST)
				{	
					sprintf(path,"%s???",szScore);
				}
				if (m_GameInfo.dwRoomRule & GRR_CONTEST)
				{
					sprintf(path,"%s???",szScore);
				}
				pTextJifen->SetText(CA2W(path));
				if (y>0)
				{
					pTextJifen->SetGameXY(x,y);
					y += 18;
				}
				else
				{
					y = pTextJifen->GetGameY();
					x = pTextJifen->GetGameX();
					pTextJifen->SetGameXY(x,y);
					y += 18;
				}
			}
			else
			{
				pTextJifen->SetControlVisible(false);
			}
		}
		if (pTextJingbi!=NULL)
		{
			/*CString strMoneyName;
			static bool bConfigReaded = false;
			if (!bConfigReaded)
			{
			CString szAppPath = CINIFile::GetAppPath();
			szAppPath += "/GameOption.bcf";
			CBcfFile f(szAppPath.GetBuffer());
			strMoneyName=f.GetKeyVal("NewGameFrame","MoneyName","金币");
			}*/
			if (bCoin)
			{
				GlbGetNumString(path, pMeInfo->i64Money, Glb().m_nPowerOfGold, Glb().m_bUseSpace, TEXT(","));
				char sz[300]={0};
				sprintf(sz,"%s%s",szCoin.GetBuffer(),path);
				if (((m_GameInfo.dwRoomRule & GRR_NOTCHEAT) && pUserInfo->GameUserInfo.dwUserID != GetMeUserInfo()->dwUserID) || m_GameInfo.dwRoomRule & GRR_CONTEST)
				{			
					sprintf(sz,"%s???",szCoin.GetBuffer());
				}
				if (m_GameInfo.dwRoomRule & GRR_CONTEST)
				{
					sprintf(sz,"%s???",szCoin.GetBuffer());
				}
				pTextJingbi->SetText(CA2W(sz));
				if (y>0)
				{
					pTextJingbi->SetGameXY(x,y);
					y += 18;
				}
				else
				{
					y = pTextJingbi->GetGameY();
					x = pTextJingbi->GetGameX();
					pTextJingbi->SetGameXY(x,y);
					y += 18;
				}
			}
			else
			{
				pTextJingbi->SetControlVisible(false);
			}
		}
		if (pTextMeili!=NULL)
		{
			if (bCharm)
			{
				sprintf(path,"%s%d",szCharm,pMeInfo->dwFascination);
				if (((m_GameInfo.dwRoomRule & GRR_NOTCHEAT) && pUserInfo->GameUserInfo.dwUserID != GetMeUserInfo()->dwUserID) || m_GameInfo.dwRoomRule & GRR_CONTEST)
				{			
					sprintf(path,"%s???",szCharm);
				}
				pTextMeili->SetText(CA2W(path));
				if (y>0)
				{
					pTextMeili->SetGameXY(x,y);
					y += 18;
				}
				else
				{
					y = pTextMeili->GetGameY();
					x = pTextMeili->GetGameX();
					pTextMeili->SetGameXY(x,y);
					y += 18;
				}
			}
			else
			{
				pTextMeili->SetControlVisible(false);
			}
		}
		if (pTextDenji!=NULL)
		{
			if (bWealthLevel)
			{
				sprintf(path,"%s%s",szWealthLevel,GetMoneyOrderName(pMeInfo->i64Money));
				if (((m_GameInfo.dwRoomRule & GRR_NOTCHEAT) && pUserInfo->GameUserInfo.dwUserID != GetMeUserInfo()->dwUserID) || m_GameInfo.dwRoomRule & GRR_CONTEST)
				{			
					sprintf(path,"%s???",szWealthLevel);
				}
				if (m_GameInfo.dwRoomRule & GRR_CONTEST)
				{
					sprintf(path,"%s???",szWealthLevel);
				}
				pTextDenji->SetText(CA2W(path));
				if (y>0)
				{
					pTextDenji->SetGameXY(x,y);
					y += 18;
				}
				else
				{
					y = pTextDenji->GetGameY();
					x = pTextDenji->GetGameX();
					pTextDenji->SetGameXY(x,y);
					y += 18;
				}
			}
			else
			{
				pTextDenji->SetControlVisible(false);
			}
		}

		//ShowOneUserInfo(pUserInfo,bVisible);
		/*IText *pTextName = NULL;
		IText *pTextID = NULL;
		IText *pTextJifen = NULL;
		IText *pTextJingbi = NULL;
		IText *pTextMeili = NULL;
		IText *pTextDenji = NULL;
		GETCTRL(IText, pTextName, m_pIUI, RIGHTBOARD_TEXT_NICKNAME);
		GETCTRL(IText, pTextID, m_pIUI,    RIGHTBOARD_TEXT_ID    ); 
		GETCTRL(IText, pTextJifen, m_pIUI, RIGHTBOARD_TEXT_JIFEN );
		GETCTRL(IText, pTextJingbi, m_pIUI,RIGHTBOARD_TEXT_JINGBI);
		GETCTRL(IText, pTextMeili, m_pIUI, RIGHTBOARD_TEXT_MEILI );
		GETCTRL(IText, pTextDenji, m_pIUI, RIGHTBOARD_TEXT_DENJI );

		char path[MAX_PATH]={0};
		if (pTextName!=NULL)
		{
			sprintf(path,"昵称:%s",pMeInfo->nickName);
			pTextName->SetText(CA2W(path));
		}
		if (pTextID!=NULL)
		{
			sprintf(path,"I  D:%d",pMeInfo->dwUserID);
			pTextID->SetText(CA2W(path));
		}
		if (pTextJifen!=NULL)
		{
			sprintf(path,"积分:%d",pMeInfo->dwPoint);
			pTextJifen->SetText(CA2W(path));
		}
		if (pTextJingbi!=NULL)
		{
			CString strMoneyName;
			static bool bConfigReaded = false;
			if (!bConfigReaded)
			{
				CString szAppPath = CINIFile::GetAppPath();
				szAppPath += "/GameOption.bcf";
				CBcfFile f(szAppPath.GetBuffer());
				strMoneyName=f.GetKeyVal("NewGameFrame","MoneyName","金币");
			}
			GlbGetNumString(path, pMeInfo->i64Money, Glb().m_nPowerOfGold, false, TEXT(","));
			char sz[300]={0};
			sprintf(sz,"%s:%s",strMoneyName.GetBuffer(),path);
			pTextJingbi->SetText(CA2W(sz));
		}
		if (pTextMeili!=NULL)
		{
			sprintf(path,"魅力:%d",pMeInfo->dwFascination);
			pTextMeili->SetText(CA2W(path));
		}
		if (pTextDenji!=NULL)
		{
			sprintf(path,"等级:%s",m_GameInfo.pOrderName(pMeInfo->dwPoint));
			pTextDenji->SetText(CA2W(path));
		}*/
	}
	return 0;
}


/// 发消息
bool XLogic::SendGameData(void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode)
{
    if (m_GameInfo.uisMeUserInfo.GameUserInfo.bDeskNO==255) 
        return 0;

    UINT nBufferLen = uBufLen + sizeof(SendGameDataStructWithoutParam);
    BYTE *buffer = new BYTE[nBufferLen];
    SendGameDataStructWithoutParam *pHead = (SendGameDataStructWithoutParam *)buffer;
    pHead->bMainID		= bMainID;
    pHead->bAssistantID = bAssistantID;
    pHead->bHandleCode	= bHandleCode;
    memcpy(buffer+sizeof(SendGameDataStructWithoutParam), pData, uBufLen);


    bool bRet = false;
    if (NULL != m_pIPC)
    {
        bRet = m_pIPC->SendData(IPC_MAIN_SOCKET, IPC_SUB_SOCKET_SEND, buffer, nBufferLen);
    }
    delete []buffer;
    buffer = NULL;
    return bRet;

    return true;
}

//发送函数
int XLogic::SendGameData(BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode)
{
    if (m_GameInfo.uisMeUserInfo.GameUserInfo.bDeskNO==255)
        return 0;
    SendGameDataStructWithoutParam *pHead = new SendGameDataStructWithoutParam;
    pHead->bMainID		= bMainID;
    pHead->bAssistantID = bAssistantID;
    pHead->bHandleCode	= bHandleCode;
    WORD nLen = sizeof(SendGameDataStructWithoutParam);

    bool bRet = false;
    if (NULL != m_pIPC)
    {
        bRet = m_pIPC->SendData(IPC_MAIN_SOCKET, IPC_SUB_SOCKET_SEND, pHead, nLen);
    }
    delete []pHead;
    pHead = NULL;
    return bRet;

    //m_pGameRoom->PostMessage(WM_SEND_GAME_DATA, (WPARAM)nLen, (LPARAM)pHead);
    return 0;
}

//离开游戏
void XLogic::QuitGame(bool bNotify,bool bNeedCheck)
{
	if (m_GameInfo.dwRoomRule & GRR_CONTEST)
	{
		if (bNeedCheck && !m_bUserContestOver)
		{
			if (!m_pGameImpl->OnGameQuiting())
			{
				return;
			}
		}
	}
	else if (bNeedCheck)
	{
		if (!m_pGameImpl->OnGameQuiting())
		{
			return;
		}
	}

	//添加视频代码，用户退出游戏时，要清空视频信息
	if (m_pVideoInterface)
	{
		m_pVideoInterface->exit();
		if (m_pVideoInterface)
			m_pVideoInterface = NULL;
	}  

	CBcfFile f(CBcfFile::GetAppPath() + _T("GameOption.bcf"));
	UINT uMaxType = f.GetKeyVal("FrameControl","max_type",1);

	if(uMaxType == 2) 
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

		SendIpcData(IPC_MAIN_USER, IPC_SUB_SCREEN_SIZE_CHANGE,0,0);
	}

    SetStationParameter(0);
    CloseGameEngine(&m_pEngine);
    if(m_pIPC)
    {
        // 参数用来开关，是否要通知大厅发一个离开消息
        m_pIPC->CloseChannel(bNotify,true);
        delete m_pIPC;
        m_pIPC = NULL;
    }
	//保存设置参数
	AfxGetApp()->WriteProfileInt(m_GameInfo.szGameName,TEXT("CanLooking"),m_GameInfo.bEnableWatch?1:0);
	AfxGetApp()->WriteProfileInt(m_GameInfo.szGameName,TEXT("PlaySound"),m_GameInfo.bEnableSound?1:0);
	AfxGetApp()->WriteProfileInt(m_GameInfo.szGameName,TEXT("BackSound"),m_GameInfo.bEnableBackSound?1:0);
	AfxGetApp()->WriteProfileInt(m_GameInfo.szGameName,TEXT("ShowUserInfo"),m_GameInfo.bShowUserInfo?1:0);
	/*WriteProfileInt(TEXT("CanLooking"),m_GameInfo.bEnableWatch?1:0);
	WriteProfileInt(TEXT("PlaySound"),m_GameInfo.bEnableSound?1:0);
	WriteProfileInt(TEXT("BackSound"),m_GameInfo.bEnableBackSound?1:0);

	WriteProfileInt(TEXT("ShowUserInfo"),m_GameInfo.bShowUserInfo?1:0);*/

    exit(0);
}
/// 窗口大小改变
void XLogic::OnSize(RECT &rect)
{
    if (NULL==m_pIUI)
    {
        return;
    }
    UIInitBiaoQingKuang();

    ITalk *pTalk = NULL;
    GETCTRL(ITalk, pTalk, m_pIUI, TALK_SHOW);
    if (NULL!=pTalk)
    {
        pTalk->SetGameH(rect.bottom-pTalk->GetGameY()-50);
    }

    if (m_strBcfFileName != m_skinmgr.GetSkinBcfFileName())
    {		
		OnExchageSkin(NULL,NULL,0,NULL);
	}

	if (NULL != m_pEngine)
	{
		WINDOWPLACEMENT place;
		::GetWindowPlacement(m_pEngine->GetCWnd()->GetSafeHwnd(),&place);
		TUIMessage tagUIMsg;
		tagUIMsg._uMessageType = UI_WINDOW_MODE_CHANGE;
		tagUIMsg._ch = (char)place.showCmd;
		OnUIMessage(&tagUIMsg);
	}
	
}

/// 使用道具
void XLogic::OnPannelUseProp(WPARAM wParam, LPARAM lParam)
{
    UsePropItem(lParam);
}
/// 购买道具
void XLogic::OnPannelBuyProp(WPARAM wParam, LPARAM lParam)
{
    // 直接加上消息头部，把子窗口发来的消息发送到平台
    BYTE *buffer = (BYTE *)lParam;
    if (NULL != m_pIPC)
    {
        m_pIPC->SendData(IPC_MAIN_PROP, IPC_SUB_BUY_PROP, buffer, wParam);
        _TAG_PROP_BUY *pPropBuy = (_TAG_PROP_BUY *)lParam;
    }
    // 子窗口发来的是new出来的指针，此时删除
    delete buffer;
    buffer = NULL;
}
/// 赠送道具
void XLogic::OnPannelGiveProp(WPARAM wParam, LPARAM lParam)
{
    // 直接加上消息头部，把子窗口发来的消息发送到平台
    BYTE *buffer = (BYTE *)lParam;
    if (NULL != m_pIPC)
    {
        m_pIPC->SendData(IPC_MAIN_PROP, IPC_SUB_PRESENT_PROP, buffer, wParam);
        _TAG_PROP_GIVE *pProp = (_TAG_PROP_GIVE *)lParam;
        ////DebugPrintf("[%s]送[%s][%d]个[%d]，值[%d]",
        //	pProp->szUser, pProp->szTargetUser, pProp->nGiveCount, pProp->nPropID, pProp->iPropValue);
    }
    // 子窗口发来的是new出来的指针，此时删除
    delete buffer;
    buffer = NULL;
}
/// 对他人使用道具
void XLogic::OnSelOtherUseProp(WPARAM wParam, LPARAM lParam)
{
    UserItemStruct *pUser = FindOnLineUser(lParam);
    if (NULL!=pUser)
    {
        int nRet = OnUseProp((int)wParam, pUser->GameUserInfo.dwUserID);
    }    
}

//状态改变
void XLogic::SetStationParameter(BYTE bGameStation)
{

	BYTE Temp = m_GameInfo.bGameStation ;
	
	m_GameInfo.bGameStation = bGameStation;
    
    if (NULL != m_pIPC)
    {
        m_pIPC->SendData(IPC_MAIN_USER, IPC_SUB_GAME_START, &bGameStation, sizeof(BYTE));
    }

	if (bGameStation != Temp)
	{
		m_pGameImpl->SetStationParameter(bGameStation);
	}

    /// 置状态 由 GameLauncher 调用
    //m_pGameImpl->SetStationParameter(bGameStation);
}

////////////////////////////////////////////////////////////////////////////////
// 框架
///游戏从框架中获取的信息
int XLogic::GetMyDeskStation()
{
    return GetMeUserInfo()->bDeskStation;
}
///获得自己的桌子号
int XLogic::GetMyDeskNum()
{
    return true;
}
///这里传的是视图索引
int XLogic::GetUserStation(int nIndex)
{
    return ViewStation2DeskStation(nIndex);
}  
///将消息内容发送到游戏服务器
int XLogic::SendGameData(WORD wSubId,void *pData, int nLen)
{
    if (m_GameInfo.uisMeUserInfo.GameUserInfo.bDeskNO==255) 
        return 0;

    UINT nBufferLen = nLen + sizeof(SendGameDataStructWithoutParam);
    BYTE *newbuffer = new BYTE[nBufferLen];
    SendGameDataStructWithoutParam *pHead = (SendGameDataStructWithoutParam *)newbuffer;
    pHead->bMainID		= MDM_GM_GAME_NOTIFY;
    pHead->bAssistantID = (BYTE)wSubId;
    pHead->bHandleCode	= 0;
    memcpy(newbuffer+sizeof(SendGameDataStructWithoutParam), (BYTE*)pData, nLen);


    bool bRet = false;
    if (NULL != m_pIPC)
    {
        bRet = m_pIPC->SendData(IPC_MAIN_SOCKET, IPC_SUB_SOCKET_SEND, newbuffer, nBufferLen);
    }
    delete []newbuffer;
    newbuffer = NULL;
    return bRet;
}
///只将消息发送到客户端大厅
int XLogic::SendIpcData(WORD wMainCmdID, WORD wSubCmdID, void * pBuffer, WORD wDataSize)
{
	if (NULL != m_pIPC)
	{
		if (wDataSize == 0)
		{
			m_pIPC->SendData(wMainCmdID,wSubCmdID);
		}
		else
		{
			m_pIPC->SendData(wMainCmdID,wSubCmdID,pBuffer,wDataSize);
		}
	}
    return true;
}

///视图转服务器坐位 因为所有游戏都要用到，所以在框架中处理
int XLogic::ViewStation2DeskStation(int nViewIndex)
{
    if(GetMeUserInfo()==NULL || m_GameInfo.uDeskPeople==0)
    {
        return 0;
    }
    BYTE bMyDeskStation = GetMeUserInfo()->bDeskStation ;//m_GameInfo.pMeUserInfo->GameUserInfo.bDeskStation;
    BYTE bDeskStation =  ((nViewIndex -m_GameInfo.uDeskPeople/2)+bMyDeskStation+m_GameInfo.uDeskPeople)%m_GameInfo.uDeskPeople ;	// 加上PLAY_COUNT是为了防止出现负数

    return bDeskStation;
}
///坐位号转视图号
int XLogic::DeskStation2View(int nDeskStation)
{
    if(GetMeUserInfo()==NULL || m_GameInfo.uDeskPeople==0 || m_GameInfo.uDeskPeople > PLAYNUM)
    {
        return 0;
    }
    BYTE bMyDeskStation = GetMeUserInfo()->bDeskStation ;//m_GameInfo.pMeUserInfo->GameUserInfo.bDeskStation;
    BYTE bViewStation=((nDeskStation-bMyDeskStation)+m_GameInfo.uDeskPeople/2+m_GameInfo.uDeskPeople)%m_GameInfo.uDeskPeople;

    return bViewStation;
}
///获取玩家昵称
TCHAR * XLogic::GetUserNickName(int nDeskStation)
{
    INT_PTR uActiveCount=m_ThisDeskPtrArray.GetCount();
    for (int i=0;i<uActiveCount;i++)
    {
        UserItemStruct * pFindUserItem=(UserItemStruct *)m_ThisDeskPtrArray.GetAt(i);
        if ((pFindUserItem!=NULL)&&(pFindUserItem->GameUserInfo.bDeskStation==nDeskStation)) 
        {
            return pFindUserItem->GameUserInfo.szName;
        }
    }
    return NULL;
}
///关闭游戏客户端
void XLogic::CloseClient()
{
    QuitGame();
}
///点击继续
int XLogic::ClickContinue()
{
	CBcfFile f(CBcfFile::GetAppPath() + _T("GameOption.bcf"));
	UINT _bExit = f.GetKeyVal("QueueRoom", "ContinueExit",1);
	if (m_GameInfo.dwRoomRule & GRR_QUEUE_GAME && _bExit == 1 || m_bIsQueueDisMiss || m_GameInfo.dwRoomRule & GRR_CONTEST)
	{
		SendGameData(MDM_GM_GAME_NOTIFY, ASS_GM_AGREE_GAME, 1);
		QuitGame();
	}
	else
		SendGameData(MDM_GM_GAME_NOTIFY, ASS_GM_AGREE_GAME, 0);
    return true;
}
///强制关闭游戏
void XLogic::CloseClientforce()
{
    QuitGame();
}
///设置时钟
void XLogic::SetTimer(int nEventID, DWORD dwTime)
{
    m_pEngine->SetTimer(nEventID, dwTime);
}
///关闭时钟
void XLogic::KillTimer(int nEventID)
{
    m_pEngine->KillTimer(nEventID);
}
///获取游戏音量
int XLogic::GetVolume()
{
    return true;
} 
///获取游戏方言
int XLogic::GetTalkKind()
{
    return true;
}
///获取声音路径
/// param _path[in, out] 调用者准备的内存空间
/// param nSize[in] _path所在地址拥有的空间大小，单位为sizeof(TCHAR)
/// return 将_path所在空间赋值后，返回_path
char* XLogic::GetAudioPath(int nDeskStation,char *_path, int nSize)
{
    return NULL;
}
///获取奖励类型
bool XLogic::GetAwardType(AwardTypeForGame &Type)
{
    return true;
}	
///播放奖励动画
bool XLogic::PlayAwardAnimation(const AwardTypeForGame &type)
{
    return true;
}
///获取倍率
bool XLogic::GetGameBaseInfo(GameInfoForGame &Info)
{
    Info.iPower = 0;

    return true;
}
///获取用户信息
bool XLogic::GetUserInfo(int nDeskStation, UserInfoForGame &Info)
{
	INT_PTR uActiveCount=m_ThisDeskPtrArray.GetCount();
	UserItemStruct * pFindUserItem = NULL;
	UserItemStruct * pTmp = NULL;
	if (nDeskStation == m_GameInfo.uisMeUserInfo.GameUserInfo.bDeskStation)
	{
		pFindUserItem =  &m_GameInfo.uisMeUserInfo;
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

		if (m_GameInfo.dwRoomRule & GRR_CONTEST)
		{
			Info.i64Money = pFindUserItem->GameUserInfo.i64ContestScore;
		}
		return true;
	}
	return false;
}
/** @brief 播放大头娃娃表情
*	@param[in]  nViewStation    视图位置
*	@param[in]  nIndex          表情类序号，目前为1~7，具体参见《动画表情.xls》
*	@return 布尔型
*/
bool XLogic::PlayBigPortrait(int nViewStation, int nIndex)
{
    return true;
}
/** @brief 使用小头像
 */
bool XLogic::SetUseSmallLogo(int nViewStation, bool bUseSmall)
{
    DebugPrintfA("%d 使用小头像 bUseSmall %d", nViewStation, bUseSmall);

    m_IsUserSmall[nViewStation] = bUseSmall;
    UserItemStruct *puser = FindOnLineUser((BYTE)ViewStation2DeskStation(nViewStation));
    if (puser)
    {
        ShowOneUserInfo(nViewStation, puser, true);
    }
    return true;
}
/// 播放声音
bool XLogic::PlaySound(const char *szFileName)
{
	if (m_pBackAudio)
	{
		m_pBackAudio->Play(szFileName);
	}

    return true;
}
/// 某方位是否使用视频
bool XLogic::SetUseVideo(int nViewStation, bool bUse)
{
    return true;
}
/// UI获取是否为旁观状态
bool XLogic::GetIsWatching()
{
    return (GetMeUserInfo() != NULL && GetMeUserInfo()->bUserState == USER_WATCH_GAME);
}
/// 获取是否允许旁观
bool XLogic::GetIsEnableWatch()
{
    return m_bWatchOther;
}
/// 从游戏界面中打开银行界面
void XLogic::OnVisitBank()
{
	// duanxiaohui 20111103 体验场(免费场) 不能使用银行功能
	if (m_GameInfo.dwRoomRule & GRR_EXPERCISE_ROOM)
	{
		AFCMessageBox("对不起, 体验场(免费场)不能使用银行功能", "温馨提示");
		return ;
	}
	// end duanxiaohui

    SendGameData(MDM_GR_MONEY,ASS_GR_OPEN_WALLET_INGAME,0);
}
/// 获取全局信息
CGlobal* XLogic::GetGlb()
{
    return &Glb();
}
/** @brief 获取窗口大小
 */
RECT XLogic::GetGameWindowRect()
{
    RECT rt={0,0,0,0};
    m_pEngine->GetCWnd()->GetClientRect(&rt);
    return rt;
}

void XLogic::set_videos_pos()
{
	if (0 >= m_iVideoOffsetNum || m_iVideoOffsetNum > VIDEO_MAX_PEOPLE || m_iVideoOffsetNum > m_GameInfo.uDeskPeople)
	{
		//只支持最多显示16个人的游戏视频
		return;
	}
	if (!m_pVideoInterface)
	{
		return;
	}
	IBCInterFace *p = NULL;
	for(int i = 0; i < m_GameInfo.uDeskPeople && m_pVideoInterface; ++i)
	{
		int nViewSite = DeskStation2View(i);
		if (nViewSite >= VIDEO_MAX_PEOPLE)
			continue;

		int iXOffset = m_pVideoXOffset[nViewSite];
		int iYOffset = m_pVideoYOffset[nViewSite];
/*
		if (m_GameInfo.uNameID == 20034400)
		{//温州四人麻将ID特殊处理头像ID从2002110开始
			p = m_pIUI->GetIControlbyID(2002110+nViewSite);			
		}
		else if (m_GameInfo.uNameID == 20600200 && 1 == nViewSite)
		{
			p = m_pIUI->GetIControlbyID(70+nViewSite+1);
		}
		else*/
		{
			p = m_pIUI->GetIControlbyID(70+nViewSite);
			if (!p)
			{
				p = m_pIUI->GetIControlbyID(90+nViewSite);				
			}
		}
		if (!p)
		{
			continue;
		}
		m_pVideoInterface->set_video_pos(nViewSite, p->GetGameX()+iXOffset, p->GetGameY()+iYOffset);
	}
}

void XLogic::set_videos_userid()
{
	UserInfoForGame UserInfo;
	for(int i = 0; i < m_GameInfo.uDeskPeople && m_pVideoInterface; ++i)
	{   
		if(!GetUserInfo(i,UserInfo)) continue;

		if (i==GetMyDeskStation())
		{
			m_pVideoInterface->set_self_user_id( GetGameInfo()->uRoomID * 1000 + UserInfo.bDeskNO, UserInfo.dwUserID); 
		}
		else
		{
			m_pVideoInterface->set_user_id( GetGameInfo()->uRoomID * 1000 + UserInfo.bDeskNO, UserInfo.dwUserID, DeskStation2View(i) ); 
		}
	}
	set_videos_pos();
}

////////////////////////////////////////////////////////////////////////////////
// 消息处理
bool XLogic::OnSystemMessage(NetMessageHead * pNetHead, void *pNetData, WORD wSendSize)
{
    if (pNetHead->bMainID!=MDM_GM_MESSAGE)
    {
        return false;
    }
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
                //m_MessageHandle.InsertSystemMessage(pMessage->szMessage,pMessage->bFontSize);
                InsertSystemMessage(pMessage->szMessage);
            }
            if ((pMessage->bShowStation&SHS_MESSAGE_BOX)!=0)
            {
                //m_bSaveView=true;
                SetStationParameter(20);
                //AFCMessageBox("好象是这里吧",m_pGameInfo.szGameName);//,MB_ICONQUESTION,this);
                ////AFCMessageBox(pMessage->szMessage,m_GameInfo.szGameName);//,MB_ICONQUESTION,this);
                //m_bSaveView=false;
                if (pMessage->bCloseFace==TRUE)
                {
                    DebugPrintfA("ASS_GM_SYSTEM_MESSAGE 系统消息，退出游戏");

                    QuitGame();
                }
            }            
            return true;
        }
    }
    return false;
}
bool XLogic::OnFrameMessage(NetMessageHead * pNetHead, void *pNetData, WORD wSendSize)
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
            m_bWatchMode=(m_GameInfo.uisMeUserInfo.GameUserInfo.bUserState==USER_WATCH_GAME);
            SetStationParameter(pGameInfo->bGameStation);
            //显示信息
            if (wSendSize>(sizeof(MSG_GM_S_GameInfo)-sizeof(pGameInfo->szMessage)))
            {
                InsertSystemMessage(pGameInfo->szMessage);
            }

            return true;
        }
    case ASS_GM_GAME_STATION:	//游戏状态
        {	
            int nRet = m_pGameImpl->HandleGameMessage(MDM_GM_GAME_NOTIFY,2,(BYTE*)pNetData, wSendSize);            
            return 0==nRet;
        }
	case ASS_GM_SET_VIDEOADDR:
		{
			if (m_pVideoInterface != NULL)
				return true;

			SetVideoSrvStruct *pmsg = (SetVideoSrvStruct*)pNetData;

			m_pVideoInterface = create_video_interface(m_GameInfo.uDeskPeople,(long long)m_pIUI->GetWindowsHwnd(), pmsg->szIP, pmsg->videoport, pmsg->audioport);
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
                    //m_MessageHandle.InsertUserTalk(GetComType()!=TY_MATCH_GAME?pSendUserItem->GameUserInfo.nickName:"玩家",GetComType()!=TY_MATCH_GAME?pTargerUserItem->GameUserInfo.nickName:"玩家",
                    //    pNormalTalk->szMessage,pNormalTalk->crColor,false);
                    //lstrcpy(pNormalTalk->szMessage,msg);
                    //ShowUserTalk(pNormalTalk);

					char szGBuffer[512] = {0};
					char szTempBuffer[256] = {0};

					::strcat(szGBuffer,"<p 4>");

					TCHAR* _pUser = (m_GameInfo.dwRoomRule & GRR_NOTCHEAT) ? TEXT("玩家") : pSendUserItem->GameUserInfo.nickName;
					::strcat(szGBuffer,_pUser);

					::strcat(szGBuffer,"说：");

					szTempBuffer[0] = '\0';
					_stprintf(szTempBuffer,"<c %08x>%s</c>",0x000000FF,pNormalTalk->szMessage);
					::strcat(szGBuffer,szTempBuffer);

					::strcat(szGBuffer,"</p>");

					if (NULL != m_pRightFrame)
					{
						m_pRightFrame->InsertTalkMessage(szGBuffer);
					}

					if (NULL != m_pTalkCtn)
					{
						TalkMessage_t msg;
						localtime(&msg.sTime);
						TCHAR* _pUser = TY_MATCH_GAME?pSendUserItem->GameUserInfo.nickName:TEXT("玩家");
						_tcscpy(msg.szSourceNickName, _pUser);
						msg.iSourceID = pNormalTalk->dwSendID;
						msg.iTargetID = pNormalTalk->dwTargetID;
						_tcscpy(msg.szMsgString, CA2T(pNormalTalk->szMessage));
						InsertTalk(msg);
						RefreshTalk();
						//char2Tchar(_P->szMessage, msg.szMsgString);
					}

					//播放聊天语音
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
					BZSoundPlay(m_pEngine->GetCWnd(),szSpeakPath,0);


                    return true;
                }
            }
            //m_MessageHandle.InsertUserTalk(GetComType()!=TY_MATCH_GAME?pSendUserItem->GameUserInfo.nickName:"玩家",NULL,pNormalTalk->szMessage,pNormalTalk->crColor,false);
            //lstrcpy(pNormalTalk->szMessage,msg);
            //if(GetComType()!=TY_MATCH_GAME)
            //    ShowUserTalk(pNormalTalk);
            //if(pNormalTalk->nDefaultIndex>0)
            //{
            //    CString soundFile;
            //    soundFile.Format("..\\Music\\Talk\\%d.wav",pNormalTalk->nDefaultIndex);
            //    BZSoundPlay(this,soundFile.GetBuffer(),0,1);
            //}            


			if (NULL != m_pTalkCtn)
			{
				TalkMessage_t msgt;            
				localtime(&msgt.sTime);
				TCHAR* _pUser = TY_MATCH_GAME?pSendUserItem->GameUserInfo.nickName:TEXT("玩家");
				_tcscpy(msgt.szSourceNickName, _pUser);
				msgt.iSourceID = pNormalTalk->dwSendID;
				msgt.iTargetID = pNormalTalk->dwTargetID;
				_tcscpy(msgt.szMsgString, CA2T(pNormalTalk->szMessage));
				InsertTalk(msgt);
				RefreshTalk();
			}
           
			//播放聊天语音
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
			BZSoundPlay(m_pEngine->GetCWnd(),szSpeakPath,0);
			

            return true;
        }
    case ASS_GM_WATCH_SET:		//旁观设置
        {

            ASSERT(m_bWatchMode==true);
            bool bNotifyClient=(m_bWatchOther!=(pNetHead->bHandleCode==TRUE));
            if (bNotifyClient==true)
            {
                m_bWatchOther=(pNetHead->bHandleCode==TRUE);
                if (m_bWatchOther==true) 
                    InsertSystemMessage(TEXT("你被允许旁观"));
                else InsertSystemMessage(TEXT("你被禁止旁观"));
                //OnWatchSetChange();
            }
            return true;
        }
    case ASS_GM_USE_KICK_PROP:      // PengJiLin, 2010-9-10, 使用踢人卡
        {
            return OnUseKickPropResult(pNetHead, pNetData, wSendSize, TRUE);
        }
    case ASS_GM_USE_ANTI_KICK_PROP: // PengJiLin, 2010-9-10, 使用防踢卡
        {
            return OnUseKickPropResult(pNetHead, pNetData, wSendSize, FALSE);;
        }
    }
    return false;
}


////////////////////////////////////////////////////////////////////////////////
// 用户信息
void XLogic::GetOnLineUser( BYTE bMeDeskStation )
{
    CWnd * winApp=AfxGetApp()->m_pMainWnd;
    if(winApp && winApp->m_hWnd)
        winApp->PostMessage(WM_REQUEST_PLAYERLIST, (WPARAM)bMeDeskStation, 0);
}
UserInfoStruct * XLogic::GetMeUserInfo()
{
    return &m_GameInfo.uisMeUserInfo.GameUserInfo;
}
// 根据用户ID找本桌玩家中是否有
UserItemStruct *XLogic::FindOnLineUser(long int dwUserID)
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

INT_PTR XLogic::GetOnlineUserCount()
{
	return m_ThisDeskPtrArray.GetCount();
}

UserItemStruct *XLogic::FindOnLineUserByIdx(long int dwIdx)
{
	INT_PTR uActiveCount=m_ThisDeskPtrArray.GetCount();
	if (dwIdx<uActiveCount)
	{
		return (UserItemStruct *)m_ThisDeskPtrArray.GetAt(dwIdx);
	}
	return NULL;
}

UserItemStruct *XLogic::FindWatchUser(long int dwUserID)
{
    INT_PTR uActiveCount=m_WatchPtrArray.GetCount();
    for (int i=0;i<uActiveCount;i++)
    {
        UserItemStruct * pFindUserItem=(UserItemStruct *)m_WatchPtrArray.GetAt(i);
        if ((pFindUserItem!=NULL)&&(pFindUserItem->GameUserInfo.dwUserID==dwUserID)) 
            return pFindUserItem;
    }
    return NULL;
}

UserItemStruct *XLogic::FindOnLineUser(BYTE bDeskStation)
{
    INT_PTR uActiveCount=m_ThisDeskPtrArray.GetCount();
    for (int i=0;i<uActiveCount;i++)
    {
        UserItemStruct * pFindUserItem=(UserItemStruct *)m_ThisDeskPtrArray.GetAt(i);
        if ((pFindUserItem!=NULL)&&(pFindUserItem->GameUserInfo.bDeskStation==bDeskStation)) 
            return pFindUserItem;
    }
    return NULL;
}

// 用户离开
bool XLogic::GameUserLeft(BYTE bDeskStation, UserItemStruct * pUserItem, bool bWatchUser)
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
					if (m_pUserListDu)
					{
						m_pUserListDu->DeleteGameUser(pFindUserItem);
					}
                    delete pFindUserItem;
                    pFindUserItem = NULL;
                    m_WatchPtrArray.RemoveAt(i);
                    break;
                }
            }
        }
        else
        {
            
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
				if (m_pUserListDu)
				{
					m_pUserListDu->DeleteGameUser(pFindUserItem);
				}
                delete pFindUserItem;
                pFindUserItem = NULL;
                m_ThisDeskPtrArray.RemoveAt(i);
                if (bWatchUser==false)
                {
                    
                }
                break;
            }
        }
        RefreshPlayList();

        return true;
    }
    return false;
}

void XLogic::OnUserListAtDesk(UserItemStruct *pUserItemVoid)
{
    UserItemStruct *pUserItemToAdd = (UserItemStruct *)pUserItemVoid;

	bool bIsMySelf = false;

    if (pUserItemToAdd->GameUserInfo.dwUserID == m_GameInfo.uisMeUserInfo.GameUserInfo.dwUserID)
    {
        memcpy(&m_GameInfo.uisMeUserInfo, pUserItemToAdd, sizeof(UserItemStruct));
		bIsMySelf = true;
    }

    if (pUserItemToAdd->GameUserInfo.bUserState!=USER_WATCH_GAME)
    {
	/*	if (m_GameInfo.dwRoomRule & GRR_CONTEST)
		{
			UserItemStruct * pFindUserItem = NULL;
			INT_PTR uCount=m_ThisDeskPtrArray.GetCount();
			for (INT_PTR i=0;i<uCount;++i)
			{
				pFindUserItem=(UserItemStruct *)m_ThisDeskPtrArray.GetAt(i);
				delete pFindUserItem;
				pFindUserItem = NULL;
			}
			m_ThisDeskPtrArray.RemoveAll();
			//比赛场则需要清空用户列表
			if (m_pUserListDu)
			{
				m_pUserListDu->DeleteAllItems();
			}
		}*/
        //查找在线用户
        bool bNewItem=false;        
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

		if (m_pUserListDu)
		{
			m_pUserListDu->AddGameUser(pUserItem,bIsMySelf);
		}

		if (m_pRightFrame)
		{
			if (bIsMySelf)
			{
				m_pRightFrame->ShowUserInfo(pUserItem);
			}
		}
    }
	//每当有用户坐桌，就重新排一下视频窗口位置
	set_videos_userid();
    m_pGameImpl->GameUserCome();
    if (pUserItemToAdd->GameUserInfo.bUserState!=USER_WATCH_GAME)
    {
        ShowOneUserInfo(DeskStation2View(pUserItemVoid->GameUserInfo.bDeskStation), pUserItemVoid, true);
    }    
    else
    {
        //查找在线用户
        bool bNewItem=false;        
        UserItemStruct * pUserItem=FindWatchUser(pUserItemToAdd->GameUserInfo.dwUserID);
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
        if (bNewItem==true) m_WatchPtrArray.Add(pUserItem);//这里应该加入旁观队列

		if (m_pUserListDu)
		{
			m_pUserListDu->AddGameUser(pUserItem,bIsMySelf);
		}

		if (m_pRightFrame)
		{
			if (bIsMySelf)
			{
				m_pRightFrame->ShowUserInfo(pUserItem);
			}
		}

		UserItemStruct* pBeWatchUser = FindOnLineUser(pUserItemVoid->GameUserInfo.bDeskStation);
		if (pBeWatchUser)
		{
			m_pUserListDu->SetWatchUser(pUserItem, pBeWatchUser->GameUserInfo.nickName);
		}

    }
    RefreshPlayList();
}

void XLogic::OnShowContestResult(UINT bActionCode, int iRank, int iAward)
{
	if (bActionCode == ACT_CONTEST_GAMEOVER && iAward > 0)
	{
		IContain *pCtnDesk = dynamic_cast<IContain*>(m_pIUI->GetIControlbyID(CTN_CONTEST_BK));
		if (pCtnDesk)
		{
			pCtnDesk->SetControlVisible(false);
		}
		IContain *pCtnAward = dynamic_cast<IContain*>(m_pIUI->GetIControlbyID(CTN_CONTEST_AWARDS));
		if (pCtnAward)
		{
			IText *pRankTxt = dynamic_cast<IText*>(m_pIUI->GetIControlbyID(LABEL_RANK));
			if (pRankTxt)
			{
				wchar_t szRank[10];
				wsprintfW(szRank, L"%d", iRank);
				pRankTxt->SetText(szRank);
			}
			IText *pAwardTxt = dynamic_cast<IText*>(m_pIUI->GetIControlbyID(LABEL_AWARDS));
			if (pAwardTxt)
			{
				wchar_t szAward[20];
				wsprintfW(szAward, L"%d金币", iAward);
				pAwardTxt->SetText(szAward);
			}
			pCtnAward->SetControlVisible(true);
		}
	}
	else
	{
		//显示或隐藏配桌画面
		IContain *pCtnDesk = dynamic_cast<IContain*>(m_pIUI->GetIControlbyID(CTN_CONTEST_BK));
		if (pCtnDesk)
		{
			if (ACT_GAME_BEGIN == bActionCode)
			{//比赛开始不显示
				pCtnDesk->SetControlVisible(false);
			}
			else
			{
				IImage *pDeskImage = NULL, *pKickImage=NULL, *pGameOverImage=NULL, *pWaitImage=NULL;
				pDeskImage = dynamic_cast<IImage *>(m_pIUI->GetIControlbyID(IMAGE_AUTO_DESK));
				pKickImage = dynamic_cast<IImage *>(m_pIUI->GetIControlbyID(IMAGE_CONTEST_KICK));
				pGameOverImage = dynamic_cast<IImage *>(m_pIUI->GetIControlbyID(IMAGE_CONTEST_NOAWARD));
				pWaitImage = dynamic_cast<IImage *>(m_pIUI->GetIControlbyID(IMAGE_CONTEST_WAIT));
				if (pDeskImage)
				{
					pDeskImage->SetControlVisible(bActionCode==ACT_GAME_END?true:false);
					IContain *pCtnAward = dynamic_cast<IContain*>(m_pIUI->GetIControlbyID(CTN_CONTEST_AWARDS));
					if (pCtnAward)
					{
						pCtnAward->SetControlVisible(false);
					}
				}
				if (pKickImage)
				{
					pKickImage->SetControlVisible(bActionCode==ACT_CONTEST_KICK?true:false);
				}
				if (pGameOverImage)
				{
					pGameOverImage->SetControlVisible(bActionCode==ACT_CONTEST_GAMEOVER?true:false);
				}
				if (pWaitImage)
				{
					pWaitImage->SetControlVisible(bActionCode==ACT_CONTEST_WAIT_GAMEOVER?true:false);
				}
				pCtnDesk->SetControlVisible(true);
			}			
		}
		return;
	}
}

////////////////////////////////////////////////////////////////////////////////
// 框架
//控制消息 
UINT XLogic::OnControlMessage(UINT uControlCode, void * pControlData, UINT uDataSize)
{
    //处理数据
    switch (uControlCode)
    {
    case CM_USER_SEND_TIMES_MONEY:
        {
            if (uDataSize!=sizeof(CM_UserState_Send_Times_Money_For_Exe)) return 1;
            CM_UserState_Send_Times_Money_For_Exe *pMoneyChange = (CM_UserState_Send_Times_Money_For_Exe*)pControlData;

            if (pMoneyChange->dwUserID != m_GameInfo.uisMeUserInfo.GameUserInfo.dwUserID)
                return 0;

            TCHAR str[100]={0};

			if(pMoneyChange->dwGetMoney > 0)
			{
				CString str;
				TCHAR szNum[128]; 
				GlbGetNumString(szNum, pMoneyChange->dwGetMoney, Glb().m_nPowerOfGold, Glb().m_bUseSpace, Glb().m_strSpaceChar);

				CBcfFile fMsg(CBcfFile::GetAppPath()+"..\\ClientMessage.bcf");
				str = fMsg.GetKeyVal("GameFrame","SendMoney_Time","/:54获得系统赠送%s金币/:54");
				str.Format(str,szNum);

				InsertNormalMessage(str.GetBuffer());
			}

            return 0;

        }
        break;
    case CM_USER_STATE:		//用户状态改变
        {
            //效验数据
            if (uDataSize!=sizeof(CM_UserState_Change_ForExe))
            {
                DebugPrintfA("数据大小不对uDataSize =[%d], 应当为[%d]",sizeof(CM_UserState_Change_ForExe));
                return 1;
            }
            CM_UserState_Change_ForExe * pStateChange=(CM_UserState_Change_ForExe *)pControlData;

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
            //处理数据
            switch (pStateChange->bActionCode)
            {
            case ACT_USER_UP:		//用户起来
            case ACT_WATCH_UP:		//旁观起来
                {
                    //处理数据
                    if (pStateChange->uisUserItem.GameUserInfo.dwUserID
                        != m_GameInfo.uisMeUserInfo.GameUserInfo.dwUserID)//同桌的其他人离开
                    {
                        GameUserLeft(pStateChange->bDeskStation, &pStateChange->uisUserItem, pStateChange->bActionCode==ACT_WATCH_UP);

						if (pStateChange->bActionCode != ACT_WATCH_UP)
						{
							//用户起身时，重置该位置的视频用户ID
							if (m_pVideoInterface)
							{
								m_pVideoInterface->set_user_id(-1, -1, DeskStation2View(pStateChange->bDeskStation));
							}
							m_pGameImpl->GameUserLeft(pStateChange->bDeskStation);
						}

						CString s;
						s.Format("xxxxxx uComType=%d, bShowUserInfo=%d", m_GameInfo.uComType, m_GameInfo.bShowUserInfo);
						OutputDebugString(s);


                        if ((m_GameInfo.uComType!=TY_MATCH_GAME)
                            &&m_GameInfo.bShowUserInfo==true
                            /*&&0 == pStateChange->uisUserItem.GameUserInfo.isVirtual*/)//.机器人也应该显示zht2011-10-25
                        {	

                                char szTalk[255];
                                sprintf(szTalk, "%s 离开了", pStateChange->uisUserItem.GameUserInfo.nickName);
                                InsertNormalMessage(szTalk);

                        }

						if (m_GameInfo.uComType != TY_MATCH_GAME)
						{
							if (pStateChange->bActionCode != ACT_WATCH_UP)
							{
								ShowOneUserInfo(DeskStation2View(pStateChange->bDeskStation), &pStateChange->uisUserItem, false);
							}
						}
						
                    }
                    else//自己离开
                    {
                        DebugPrintfA("自己离开了");
                    }
                    return 0;
                }
            case ACT_WATCH_SIT:		//用户坐下
            case ACT_USER_SIT:		//旁观坐下
                {
					if (m_GameInfo.dwRoomRule & GRR_CONTEST)
					{
						m_bUserContestOver = false;
						UserItemStruct * pFindUserItem = NULL;
						INT_PTR uCount=m_ThisDeskPtrArray.GetCount();
						for (INT_PTR i=0;i<uCount;++i)
						{
							pFindUserItem=(UserItemStruct *)m_ThisDeskPtrArray.GetAt(i);
							delete pFindUserItem;
							pFindUserItem = NULL;
						}
						m_ThisDeskPtrArray.RemoveAll();
						//比赛场则需要清空用户列表
						if (m_pUserListDu)
						{
							m_pUserListDu->DeleteAllItems();
						}
					}
                    //处理数据
                    // 若是别人坐下，则直接通知
                    if (pStateChange->uisUserItem.GameUserInfo.dwUserID
                        != m_GameInfo.uisMeUserInfo.GameUserInfo.dwUserID)//同桌的其他人
                    {
                        OnUserListAtDesk(&pStateChange->uisUserItem);
                        if (m_GameInfo.bShowUserInfo==true&&(m_GameInfo.uComType!=TY_MATCH_GAME))
                        {
                            char szTalk[255];
                            sprintf(szTalk, "%s 进来了", pStateChange->uisUserItem.GameUserInfo.nickName);
                            InsertNormalMessage(szTalk);
                        }
                    }
                    else	// 若是自己坐到某个桌子，则需要找到同桌已有玩家的信息，并更新信息，用于换桌
                    {
						if ((m_GameInfo.dwRoomRule & GRR_CONTEST) &&
							(GetMeUserInfo()->dwUserID == pStateChange->uisUserItem.GameUserInfo.dwUserID))// add by ramon 09.05.31 修复个人信息栏不刷新
						{
							memcpy(&m_GameInfo.uisMeUserInfo, &pStateChange->uisUserItem, sizeof(UserItemStruct));
						}
                        //发送获取信息
                        MSG_GM_S_ClientInfo ClientInfo;
                        ClientInfo.bEnableWatch=m_GameInfo.bEnableWatch;
                        SendGameData(&ClientInfo,sizeof(ClientInfo),MDM_GM_GAME_FRAME,ASS_GM_GAME_INFO,0);
                        //UpdateGameTitle();
                    }

                    return 0;
                }
            case ACT_USER_CUT:		//玩家断线or强退了
                {
                    //机器人断线不用提示信息
					if(0 == pStateChange->uisUserItem.GameUserInfo.isVirtual)
					{
						if (m_GameInfo.bShowUserInfo==true)
						{
							//显示信息
							char szTalk[255];

							sprintf(szTalk, "%s 断线了", pStateChange->uisUserItem.GameUserInfo.nickName);
							InsertNormalMessage(szTalk);
						}

						UserItemStruct* pFindUserItem = FindOnLineUser(pStateChange->uisUserItem.GameUserInfo.dwUserID);

						if (m_pUserListDu)
						{
							m_pUserListDu->UpdateGameUser(pFindUserItem);
						}

						if (m_pRightFrame)
						{
							m_pRightFrame->ShowUserInfo();
						}
                    }
                    return 0;
                }
            case ACT_USER_AGREE:	//用户同意
                {
                    MSG_GR_R_UserAgree agree;
                    agree.bDeskStation = pStateChange->bDeskStation;
                    agree.bAgreeGame = true;
                    m_pGameImpl->HandleGameMessage(MDM_GM_GAME_NOTIFY, ASS_GM_AGREE_GAME, (BYTE*)&agree, sizeof(agree));

					UserItemStruct* pFindUserItem = FindOnLineUser(pStateChange->uisUserItem.GameUserInfo.dwUserID);

					if (m_pUserListDu)
					{
						m_pUserListDu->UpdateGameUser(pFindUserItem);
					}

					if (m_pRightFrame)
					{
						m_pRightFrame->ShowUserInfo();
					}

                    return 0;
                }
            case ACT_GAME_BEGIN:	//游戏开始
            case ACT_GAME_END:		//游戏结束
                {
					if (m_pGameImpl && (m_GameInfo.dwRoomRule & GRR_CONTEST))
					{
						//显示或隐藏配桌画面
						OnShowContestResult(pStateChange->bActionCode);
					}
                    INT_PTR uActiveCount=m_ThisDeskPtrArray.GetCount();
					if (0 == uActiveCount && pStateChange->bActionCode)
					{
						if (m_pRightFrame)
						{
							m_pRightFrame->ShowUserInfo(&m_GameInfo.uisMeUserInfo);							
						}
						OnExchageSkin(NULL,NULL,0,NULL);
						return 0;
					}
                    for (int i=0;i<uActiveCount;i++)
                    {
                        UserItemStruct * pFindUserItem=(UserItemStruct *)m_ThisDeskPtrArray.GetAt(i);

						if (pStateChange->bActionCode == ACT_GAME_BEGIN)
						{
							pFindUserItem->GameUserInfo.bUserState = USER_PLAY_GAME;
							//更新信息
							ShowOneUserInfo(DeskStation2View(pFindUserItem->GameUserInfo.bDeskStation), pFindUserItem, true);

							if (m_pUserListDu)
							{
								m_pUserListDu->UpdateGameUser(pFindUserItem);
								CString str;
								str.Format("cq::-------------更新用户%d的游戏状态为开始比赛", pFindUserItem->GameUserInfo.dwUserID);
								OutputDebugString(str);
							}
						}
						else if (pStateChange->bActionCode == ACT_GAME_END)
						{
							pFindUserItem->GameUserInfo.bUserState = USER_SITTING;
							if (m_GameInfo.dwRoomRule & GRR_CONTEST)
							{
								pFindUserItem->GameUserInfo.bUserState = USER_LOOK_STATE;
								if (m_pUserListDu)
								{
									m_pUserListDu->DeleteAllItems();
								}
								if (pFindUserItem->GameUserInfo.dwUserID == m_GameInfo.uisMeUserInfo.GameUserInfo.dwUserID)
								{
									//更新信息
									//		ShowOneUserInfo(DeskStation2View(pFindUserItem->GameUserInfo.bDeskStation), pFindUserItem, true);
								}
							}
							else
							{
								//更新信息
								ShowOneUserInfo(DeskStation2View(pFindUserItem->GameUserInfo.bDeskStation), pFindUserItem, true);

								if (m_pUserListDu)
								{
									m_pUserListDu->UpdateGameUser(pFindUserItem);
								}
							}
						}
					}
                    return 0;
                }
			case ACT_CONTEST_GAMEOVER:
			case ACT_CONTEST_KICK:
			case ACT_CONTEST_WAIT_GAMEOVER:
				{
					if (m_pGameImpl)
					{
						m_bUserContestOver = pStateChange->bActionCode == ACT_CONTEST_WAIT_GAMEOVER? false : true;
						//显示淘汰画面或比赛结束
						if (ACT_CONTEST_GAMEOVER == pStateChange->bActionCode)
						{
							UserItemStruct * pUserItem=FindOnLineUser(pStateChange->uisUserItem.GameUserInfo.dwUserID);
							if (pUserItem == NULL)
							{
								return 0;
							}
							memcpy( pUserItem, &pStateChange->uisUserItem, sizeof(UserItemStruct));
							CString str;
							str.Format("cq::----------GL-------玩家%d的奖励%d", pStateChange->uisUserItem.GameUserInfo.dwUserID, pStateChange->uisUserItem.GameUserInfo.iAward);
							OutputDebugString(str);
							OnShowContestResult(pStateChange->bActionCode, pUserItem->GameUserInfo.iRankNum,pUserItem->GameUserInfo.iAward);							
						}
						else 
						{
							OnShowContestResult(pStateChange->bActionCode);
						}
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
                    
                    //更新信息
                    ShowOneUserInfo(DeskStation2View(pUserItem->GameUserInfo.bDeskStation), pUserItem, true);
                    //更新座位区用户
                    RefreshPlayList();

					if (m_pUserListDu)
					{
						m_pUserListDu->UpdateGameUser(pUserItem);
					}

                    if (GetMeUserInfo()->dwUserID == pStateChange->uisUserItem.GameUserInfo.dwUserID)// add by ramon 09.05.31 修复个人信息栏不刷新
                    {
                        memcpy(&m_GameInfo.uisMeUserInfo, &pStateChange->uisUserItem, sizeof(UserItemStruct));
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

					if (m_pRightFrame)
					{
						m_pRightFrame->ShowUserInfo();
					}
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

					UserInfoStruct *pMeInfo = GetMeUserInfo();					
					//更新信息
					ShowOneUserInfo(1, &pStateChange->uisUserItem, true);
					//更新座位区用户
					RefreshPlayList();

					if (m_pUserListDu)
					{
						m_pUserListDu->UpdateGameUser(pUserItem);
					}

					if (m_pRightFrame)
					{
						m_pRightFrame->ShowUserInfo(pUserItem);
					}					
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
                    
                    //更新信息
                    ShowOneUserInfo(DeskStation2View(pUserItem->GameUserInfo.bDeskStation), pUserItem, true);
                    //更新座位区用户
                    RefreshPlayList();

					if (m_pUserListDu)
					{
						m_pUserListDu->UpdateGameUser(pUserItem);
					}

                    if (GetMeUserInfo()->dwUserID == pStateChange->uisUserItem.GameUserInfo.dwUserID)// add by ramon 09.05.31 修复个人信息栏不刷新
                    {
                        memcpy(&m_GameInfo.uisMeUserInfo, &pStateChange->uisUserItem, sizeof(UserItemStruct));
                    }

					if (m_pRightFrame)
					{
						m_pRightFrame->ShowUserInfo();
					}
                    //if(pUserItem->GameUserInfo.dwUserID==m_CurpUserInfoClicked->dwUserID)
                    //{
                    //    NavigateVirFace();
                    //}
                    //if (pUserItem->GameUserInfo.bUserState!=USER_WATCH_GAME)
                    //{
                    //    m_pGameView->UpdateUserInfo(pStateChange->bDeskStation);
                    //}
                    return 0;
                }
            }
        }
    }

    return 0;
}



////////////////////////////////////////////////////////////////////////////////
// 聊天
int XLogic::InsertSystemMessage(TCHAR* szMessage)
{

	char szGBuffer[512] = {0};
	char szTempBuffer[256] = {0};

	::strcat(szGBuffer,"<p 6>");

	szTempBuffer[0] = '\0';
	_stprintf(szTempBuffer,"<i dialog\\xtxx.png 0> <c %08x>%s</c>",0x00FF0000,szMessage);
	::strcat(szGBuffer,szTempBuffer);

	::strcat(szGBuffer,"</p>");

	if (m_pRightFrame)
	{
		m_pRightFrame->InsertTalkMessage(szGBuffer);
	}

	if (NULL != m_pTalkCtn)
	{
		TalkMessage_t _Msg;
		_Msg.eChatType = CT_SYSTEM;
		_tcscpy(_Msg.szMsgString, szMessage);
		InsertTalk(_Msg);
		RefreshTalk();
	}

    return 0;
}


int XLogic::InsertNormalMessage(TCHAR* szMessage)
{

	char szGBuffer[512] = {0};
	char szTempBuffer[256] = {0};

	::strcat(szGBuffer,"<p 4>");

	szTempBuffer[0] = '\0';
	_stprintf(szTempBuffer,"<c %08x>%s</c>",0x00000000,szMessage);
	::strcat(szGBuffer,szTempBuffer);

	::strcat(szGBuffer,"</p>");

	if (m_pRightFrame)
	{
		m_pRightFrame->InsertTalkMessage(szGBuffer);
	}

	if (NULL != m_pTalkCtn)
	{
		TalkMessage_t _Msg;
		_Msg.eChatType = CT_NORMAL;
		_tcscpy(_Msg.szMsgString, szMessage);
		InsertTalk(_Msg);
		RefreshTalk();
	}

    return 0;    
}
int XLogic::InsertDuduMsg(TCHAR * szSendName, TCHAR * szTargetName,TCHAR * szCharString, 
                          COLORREF crTextColor, bool bShowTime, UINT uSize, 
                          TCHAR * szFontName,int iDuduType)
{
	CString cstrGBuffer("<p 4>");
	CString cstrTmp;
	if (bShowTime)
	{
		SYSTEMTIME SystemTime;
		GetLocalTime(&SystemTime);
		
		cstrTmp.Format("<c #%06f>(%02d:%02d:%02d)\n</c>",0x00000000,
			SystemTime.wHour,SystemTime.wMinute,SystemTime.wSecond);

		cstrGBuffer.Append(cstrTmp);
	}
	cstrGBuffer.Append(szSendName);

	if (szTargetName!=NULL)
	{
		if(iDuduType==BRD_MSG_BIG)
			cstrTmp.Format("<c %08x>拿着大喇叭喊</c>",0x000000FF);	
		else
			cstrTmp.Format("<c %08x>拿着小喇叭喊</c>",0x000000FF);
		cstrGBuffer.Append(cstrTmp);
		cstrGBuffer.Append(szTargetName);
	}
	cstrTmp.Format("<c %08x>%s</c>",0x00FF0000,szCharString);
	cstrGBuffer.Format("%s%s</p>",cstrGBuffer,cstrTmp);
	if (m_pRightFrame)
	{
		m_pRightFrame->InsertTalkMessage(cstrGBuffer.GetBuffer(sizeof(cstrGBuffer)));
	}
    return 0;
}

int XLogic::InsertTalk(const TalkMessage_t& sMsg)
{
	if (NULL == m_pTalkCtn)
	{
		return 0;
	}
	
	if (!m_bIsAcceptChatMsg)	//不接收聊天消息
	{
		return 0;
	}

    m_TalkMessages.push_back(sMsg);
    int iCount(0);
    for (list<TalkMessage_t>::iterator it = m_TalkMessages.begin(); it != m_TalkMessages.end(); it++)
    {
        if (sMsg.eChatType == it->eChatType) iCount++; 
    }
    if (iCount > 50)
    {
        for (list<TalkMessage_t>::iterator it = m_TalkMessages.begin(); it != m_TalkMessages.end(); it++)
        {
            if (sMsg.eChatType == it->eChatType) 
            {
                m_TalkMessages.erase(it);
                break;
            }
        }
    }
    RefreshTalk();
    return 0;
}

int XLogic::RefreshTalk()
{
    ITalk* pTalk;
    GETCTRL(ITalk, pTalk, m_pIUI, TALK_SHOW);
    if (pTalk != NULL)
    {
        pTalk->ClearText();
        TCHAR* pTalkString;
        for (list<TalkMessage_t>::iterator it = m_TalkMessages.begin(); it != m_TalkMessages.end(); it++)
        {
            pTalkString = FormatTalk(*it);
            pTalk->SetText(CT2W(pTalkString));
            delete pTalkString;
        }
    }
    return 0;
}

TCHAR* XLogic::FormatTalk(const TalkMessage_t& sMsg)
{
    TCHAR* pResult = new TCHAR[600];
    memset(pResult, 0, 600 * sizeof(TCHAR));

    CHATTYPE eChatTarget = sMsg.eChatType;
    if (eChatTarget == CT_PRIVATE)
    {
		static CString clr1;//zht2011-11-9 修改系统消息等字体颜色也可配
		static bool bGetColor1 = false;
		if (!bGetColor1)
		{        
			TCHAR szPath[MAX_PATH];
			wsprintf(szPath,"GameOption.bcf");
			CBcfFile f(szPath);
			clr1=f.GetKeyVal("Chat","PrivateColor","0xffffff00");
			bGetColor1 = true;
		}

		//wsprintf(pResult, TEXT("<font><color=%s,size=14,style=楷书>%s</font>"), clr.GetBuffer(), sMsg.szMsgString);
        wsprintf(pResult, TEXT("<font><color=%s,size=12,style=楷书>%s：</font><font><color=%s,size=12,style=楷书>%s</font>"), clr1.GetBuffer(),sMsg.szSourceNickName, clr1.GetBuffer(),sMsg.szMsgString);
    }
    else if (eChatTarget == CT_SYSTEM)
    {
		static CString clr2;
		static bool bGetColor2 = false;
		if (!bGetColor2)
		{        
			TCHAR szPath[MAX_PATH];
			wsprintf(szPath,"GameOption.bcf");
			CBcfFile f(szPath);
			clr2=f.GetKeyVal("Chat","SystemColor","0xffffff00");
			bGetColor2 = true;
		}
		

        wsprintf(pResult, TEXT("<font><color=0xffff0000,size=12,style=楷书>系统消息：</font><font><color=%s,size=12,style=楷书>%s</font>"),clr2.GetBuffer(), sMsg.szMsgString);
    }
    else if (CT_PUBLIC == eChatTarget)
    {
		static CString clr3;
		static bool bGetColor3 = false;
		if (!bGetColor3)
		{        
			TCHAR szPath[MAX_PATH];
			wsprintf(szPath,"GameOption.bcf");
			CBcfFile f(szPath);
			clr3=f.GetKeyVal("Chat","PublicColor","0xffffff00");
			bGetColor3 = true;
		}
        wsprintf(pResult, TEXT("<font><color=0xffff0000,size=12,style=楷书>%s：</font><font><color=%s,size=12,style=楷书>%s</font>"),clr3.GetBuffer(), sMsg.szSourceNickName, sMsg.szMsgString);
    }
    else if (CT_NORMAL == eChatTarget)
    {
        static CString clr;
        static bool bGetColor = false;
        if (!bGetColor)
        {        
            TCHAR szPath[MAX_PATH];
            wsprintf(szPath,"GameOption.bcf");
            CBcfFile f(szPath);
            clr=f.GetKeyVal("Chat","NotifyColor","0xffffff00");
            bGetColor = true;
        }

        wsprintf(pResult, TEXT("<font><color=%s,size=12,style=楷书>%s</font>"), clr.GetBuffer(), sMsg.szMsgString);
    }
    else if (CT_DALABA == eChatTarget)
    {
		static CString clr4;
		static bool bGetColor4 = false;
		if (!bGetColor4)
		{        
			TCHAR szPath[MAX_PATH];
			wsprintf(szPath,"GameOption.bcf");
			CBcfFile f(szPath);
			clr4=f.GetKeyVal("Chat","DaLaBaColor","0xFFFF0000");
			bGetColor4 = true;
			
		}

		///传过来的颜色值好像是反的，在这里帮他转正
		int rColor = GetBValue(sMsg.crTextColor);
		int GColor = GetGValue(sMsg.crTextColor);
		int BColor = GetRValue(sMsg.crTextColor);
		COLORREF temp = RGB(rColor,GColor,BColor);

        wsprintf(pResult, TEXT("<font><color=%s,size=12,style=楷书>%s</font><font><color=0xFF0000FF,size=12,style=楷书>拿着大喇叭喊:</font>\
                               <font><color=0xFF%06X,size=12,style=楷书>%s</font>"),clr4.GetBuffer(),sMsg.szSourceNickName, temp /*sMsg.crTextColor*/,sMsg.szMsgString);
		
    }
    else if (CT_XIAOLABA == eChatTarget)
    {
		static CString clr5;
		static bool bGetColor5 = false;
		if (!bGetColor5)
		{        
			TCHAR szPath[MAX_PATH];
			wsprintf(szPath,"GameOption.bcf");
			CBcfFile f(szPath);
			clr5=f.GetKeyVal("Chat","XiaoLaBaColor","0xFFFF0000");
			bGetColor5 = true;
			//GetRValue();
		}
		///传过来的颜色值好像是反的，在这里帮他转正
		int rColor = GetBValue(sMsg.crTextColor);
		int GColor = GetGValue(sMsg.crTextColor);
		int BColor = GetRValue(sMsg.crTextColor);
		COLORREF temp = RGB(rColor,GColor,BColor);

        wsprintf(pResult, TEXT("<font><color=%s,size=12,style=楷书>%s</font><font><color=0xFF0000FF,size=12,style=楷书>拿着小喇叭喊:</font>\
                               <font><color=0xFF%06X,size=12,style=楷书>%s</font>"),clr5.GetBuffer(), sMsg.szSourceNickName, temp/*sMsg.crTextColor*/,sMsg.szMsgString);
	
    }
    return pResult;
}

int XLogic::InitPlayerListCtrl()
{
    IListControl *pList = NULL;
    GETCTRL(IListControl, pList, m_pIUI, RIGHTBOARD_USERLIST);
    if (NULL==pList)
    {
        return -1;
    }


	CString strColumnName;
	CString strColumnIndex;
	CString strColumnIndexAll;
	CString strConfig;
	BOOL    bConfigExist;

	CString strDefaultColumnIndexAll = "Nickname,Coin,WealthLevel,Score,ScoreLevel,Charm,CharmLevel,Sex,GameCount,WinGame,LoseGame,Peace,Escape,WatchUser,";
	bConfigExist = TRUE;

	CBcfFile fUserList(CBcfFile::GetAppPath () + "..\\GameUserInforList.bcf");
	strColumnIndexAll = fUserList.GetKeyVal("InforList","_ColumnOrder","");
	if (strColumnIndexAll.IsEmpty())
	{
		bConfigExist = FALSE;
		strColumnIndexAll = strDefaultColumnIndexAll;
	}

	int iIndex = 0;
	int iWidth = 0;
	int nColCount = 0;
	int nHeadPos = 0;
	int nTailPos = strColumnIndexAll.Find(',', nHeadPos);
	while (nTailPos > nHeadPos)
	{
		strColumnIndex = strColumnIndexAll.Mid(nHeadPos, nTailPos-nHeadPos);
		if (bConfigExist)
			strConfig = fUserList.GetKeyVal("InforList",strColumnIndex,"");
		else
			strConfig = COLOMN_NAME[iIndex];

		if (!strConfig.IsEmpty())
		{
			DecodeString(strConfig,strColumnName,iWidth);

			wchar_t szStr[10];
			char2wchar(strColumnName.GetBuffer(strColumnName.GetLength()), szStr);
			pList->InsertColumn(iIndex,szStr,iWidth,20);
			m_MapColumn.insert(pair<string,int>(strColumnIndex.GetBuffer(),iIndex));
			iIndex++;
		}

		nHeadPos = nTailPos + 1;
		nTailPos = strColumnIndexAll.Find(',', nHeadPos);
	}

	CBcfFile f("GameOption.bcf");
	
	int r=f.GetKeyVal("GameList","GameListSelectTextR",255);
	int g=f.GetKeyVal("GameList","GameListSelectTextG",0);
	int b=f.GetKeyVal("GameList","GameListSelectTextB",0);
		

    int nCount = pList->GetColNum();
    for (int i=0; i<nCount; ++i)
    {
        pList->SetTextShowStyle(i, AlignmentCenter);
    }
	
    pList->SetSelectRectColor(RGB(r,g,b)+0xff000000);   
    	
	if (m_pUserListDu)
	{
		m_pUserListDu->InitListCtrl(m_GameInfo.pOrderName,m_GameInfo.uComType);
	}

    return 0;
}

int XLogic::RefreshPlayList()
{

    IListControl *pList = NULL;
    GETCTRL(IListControl, pList, m_pIUI, RIGHTBOARD_USERLIST);
    if (NULL==pList)
    {
        return -1;
    }
   /* if (pList->GetColNum()<12)
    {
        return -2;
    }*/

    pList->Clear();
    int nUserCountInThisDesk = m_ThisDeskPtrArray.GetCount();
    for (int i=0; i<nUserCountInThisDesk; ++i)
    {
		
        UserItemStruct *pFindItem = (UserItemStruct *)m_ThisDeskPtrArray.GetAt(i);

		UserItemStruct *pUserInfo = pFindItem;

		if (!pUserInfo)
		{
			continue;
		}

		char szContent[300]={0};
		map<string, int>::iterator ite; 
		
		// 昵称
		ite = m_MapColumn.find("Nickname"); 
		if (m_MapColumn.end() != ite)
		{
			sprintf(szContent,"%s",pFindItem->GameUserInfo.nickName);
			if ((m_GameInfo.dwRoomRule & GRR_NOTCHEAT) && pUserInfo->GameUserInfo.dwUserID != GetMeUserInfo()->dwUserID)
			{			
				sprintf(szContent,"玩家");
			}
			 pList->InsertInItem(i, ite->second, CA2W(szContent));
		}

		// 乐豆
		ite = m_MapColumn.find("Coin"); 
		if (m_MapColumn.end() != ite)
		{
			// 乐豆
			GlbGetNumString(szContent, pFindItem->GameUserInfo.i64Money, Glb().m_nPowerOfGold, Glb().m_bUseSpace, TEXT(","));
			if (((m_GameInfo.dwRoomRule & GRR_NOTCHEAT) && pUserInfo->GameUserInfo.dwUserID != GetMeUserInfo()->dwUserID) || m_GameInfo.dwRoomRule & GRR_CONTEST)
			{			
				sprintf(szContent,"???");
			}
			if (m_GameInfo.dwRoomRule & GRR_CONTEST)
			{
				sprintf(szContent,"???");
			}
			pList->InsertInItem(i, ite->second, CA2W(szContent));
		}
           // 财富级别
		ite = m_MapColumn.find("WealthLevel"); 
		if (m_MapColumn.end() != ite)
		{
			sprintf(szContent,"%s",GetMoneyOrderName(pFindItem->GameUserInfo.i64Money));
			if (((m_GameInfo.dwRoomRule & GRR_NOTCHEAT) && pUserInfo->GameUserInfo.dwUserID != GetMeUserInfo()->dwUserID) || m_GameInfo.dwRoomRule & GRR_CONTEST)
			{			
				sprintf(szContent,"???");
			}
			if (m_GameInfo.dwRoomRule & GRR_CONTEST)
			{
				sprintf(szContent,"???");
			}
			pList->InsertInItem(i, ite->second, CA2W(szContent));
		}
  
        // 积分
		ite = m_MapColumn.find("Score"); 
		if (m_MapColumn.end() != ite)
		{
			sprintf(szContent,"%d",pFindItem->GameUserInfo.dwPoint-GetNormalOrderBasePoint(pFindItem->GameUserInfo.dwPoint));
			if (((m_GameInfo.dwRoomRule & GRR_NOTCHEAT) && pUserInfo->GameUserInfo.dwUserID != GetMeUserInfo()->dwUserID) || m_GameInfo.dwRoomRule & GRR_CONTEST)
			{			
				sprintf(szContent,"???");
			}
			pList->InsertInItem(i, ite->second, CA2W(szContent));
		}

        // 积分级别
		ite = m_MapColumn.find("ScoreLevel"); 
		if (m_MapColumn.end() != ite)
		{
			sprintf(szContent,"%s", m_GameInfo.pOrderName(pFindItem->GameUserInfo.dwPoint));
			if (((m_GameInfo.dwRoomRule & GRR_NOTCHEAT) && pUserInfo->GameUserInfo.dwUserID != GetMeUserInfo()->dwUserID) || m_GameInfo.dwRoomRule & GRR_CONTEST)
			{			
				sprintf(szContent,"???");
			}
			pList->InsertInItem(i, ite->second, CA2W(szContent));
		}

    	// 总局数
		ite = m_MapColumn.find("GameCount"); 
		if (m_MapColumn.end() != ite)
		{
			UINT uAllCount1=pFindItem->GameUserInfo.uWinCount+pFindItem->GameUserInfo.uLostCount+pFindItem->GameUserInfo.uMidCount;
			sprintf(szContent,"%d",uAllCount1);
			if (((m_GameInfo.dwRoomRule & GRR_NOTCHEAT) && pUserInfo->GameUserInfo.dwUserID != GetMeUserInfo()->dwUserID) || m_GameInfo.dwRoomRule & GRR_CONTEST)
			{			
				sprintf(szContent,"???");
			}
			pList->InsertInItem(i, ite->second, CA2W(szContent));
		}
         
		// 胜局数
		ite = m_MapColumn.find("WinGame"); 
		if (m_MapColumn.end() != ite)
		{
			sprintf(szContent,"%d",pFindItem->GameUserInfo.uWinCount);
			if (((m_GameInfo.dwRoomRule & GRR_NOTCHEAT) && pUserInfo->GameUserInfo.dwUserID != GetMeUserInfo()->dwUserID) || m_GameInfo.dwRoomRule & GRR_CONTEST)
			{			
				sprintf(szContent,"???");
			}
			pList->InsertInItem(i, ite->second, CA2W(szContent));
		}
        // 输局数
		ite = m_MapColumn.find("LoseGame"); 
		if (m_MapColumn.end() != ite)
		{
			sprintf(szContent,"%d",pFindItem->GameUserInfo.uLostCount);
			if (((m_GameInfo.dwRoomRule & GRR_NOTCHEAT) && pUserInfo->GameUserInfo.dwUserID != GetMeUserInfo()->dwUserID) || m_GameInfo.dwRoomRule & GRR_CONTEST)
			{			
				sprintf(szContent,"???");
			}
			pList->InsertInItem(i, ite->second, CA2W(szContent));
		}
        // 和局数
		ite = m_MapColumn.find("Peace"); 
		if (m_MapColumn.end() != ite)
		{
			sprintf(szContent,"%d",pFindItem->GameUserInfo.uMidCount);
			if (((m_GameInfo.dwRoomRule & GRR_NOTCHEAT) && pUserInfo->GameUserInfo.dwUserID != GetMeUserInfo()->dwUserID) || m_GameInfo.dwRoomRule & GRR_CONTEST)
			{			
				sprintf(szContent,"???");
			}
			pList->InsertInItem(i, ite->second, CA2W(szContent));
		}

        //// 旁观对象
		ite = m_MapColumn.find("WatchUser"); 
		if (m_MapColumn.end() != ite)
		{
			sprintf(szContent,"");
			pList->InsertInItem(i, ite->second, CA2W(szContent));
		}
        // 魅力
		ite = m_MapColumn.find("Charm"); 
		if (m_MapColumn.end() != ite)
		{
			sprintf(szContent,"%d", pFindItem->GameUserInfo.dwFascination);
			if (((m_GameInfo.dwRoomRule & GRR_NOTCHEAT) && pUserInfo->GameUserInfo.dwUserID != GetMeUserInfo()->dwUserID) || m_GameInfo.dwRoomRule & GRR_CONTEST)
			{			
				sprintf(szContent,"???");
			}
			pList->InsertInItem(i, ite->second, CA2W(szContent));
		}
        // 魅力级别
		ite = m_MapColumn.find("CharmLevel"); 
		if (m_MapColumn.end() != ite)
		{
			sprintf(szContent,"%s", GetCharmGrade(pFindItem->GameUserInfo.dwFascination));
			if (((m_GameInfo.dwRoomRule & GRR_NOTCHEAT) && pUserInfo->GameUserInfo.dwUserID != GetMeUserInfo()->dwUserID) || m_GameInfo.dwRoomRule & GRR_CONTEST)
			{			
				sprintf(szContent,"???");
			}
			pList->InsertInItem(i, ite->second, CA2W(szContent));
		}
		CString stime;
		stime.Format("%d",CTime::GetCurrentTime());        //当前的时间，String格式
		int curtime=atoi(stime);                        //转换为int

		///vip玩家用红色，其余的用白色
		if(pFindItem->GameUserInfo.iVipTime>=curtime)
		{
			pList->SetRowFontColor(i, 0xff0000);
		}
		else
		{
			TCHAR szPath[MAX_PATH];
			wsprintf(szPath,"GameOption.bcf");
			CBcfFile f(szPath);
			int r,g,b;
			r=f.GetKeyVal("GameList","GameListTextR",0);
			g=f.GetKeyVal("GameList","GameListTextG",255);
			b=f.GetKeyVal("GameList","GameListTextB",255);
			COLORREF GameListTextColor=RGB(r,g,b);

			pList->SetRowFontColor(i, GameListTextColor);
		}
    }

    int nWatchCount = m_WatchPtrArray.GetCount();
    for (int i=0; i<nWatchCount; ++i)
    {
        UserItemStruct *pFindItem = (UserItemStruct *)m_WatchPtrArray.GetAt(i);
        if (NULL==pFindItem)
        {
            continue;
        }
        // 昵称
        //char szContent[300]={0};
        //sprintf(szContent,"%s",pFindItem->GameUserInfo.szName);
        //pList->InsertInItem(nUserCountInThisDesk+i, 0, CA2W(szContent));
        //// 乐豆
        //GlbGetNumString(szContent, pFindItem->GameUserInfo.i64Money, Glb().m_nPowerOfGold, false, TEXT(","));
        //pList->InsertInItem(nUserCountInThisDesk+i, 1, CA2W(szContent));
        //// 财富级别
        //sprintf(szContent,"%s",GetMoneyOrderName(pFindItem->GameUserInfo.i64Money));
        //pList->InsertInItem(nUserCountInThisDesk+i, 2, CA2W(szContent));
        //// 积分
        //sprintf(szContent,"%d",pFindItem->GameUserInfo.dwPoint-GetNormalOrderBasePoint(pFindItem->GameUserInfo.dwPoint));
        //pList->InsertInItem(nUserCountInThisDesk+i, 3, CA2W(szContent));
        //// 积分级别
        //sprintf(szContent,"%s", m_GameInfo.pOrderName(pFindItem->GameUserInfo.dwPoint));
        //pList->InsertInItem(nUserCountInThisDesk+i, 4, CA2W(szContent));
        //// 总局数
        //UINT uAllCount1=pFindItem->GameUserInfo.uWinCount+pFindItem->GameUserInfo.uLostCount+pFindItem->GameUserInfo.uMidCount;
        //sprintf(szContent,"%d",uAllCount1);
        //pList->InsertInItem(nUserCountInThisDesk+i, 5, CA2W(szContent));
        //// 胜局数
        //sprintf(szContent,"%d",pFindItem->GameUserInfo.uWinCount);
        //pList->InsertInItem(nUserCountInThisDesk+i, 6, CA2W(szContent));
        //// 输局数
        //sprintf(szContent,"%d",pFindItem->GameUserInfo.uLostCount);
        //pList->InsertInItem(nUserCountInThisDesk+i, 7, CA2W(szContent));
        //// 和局数
        //sprintf(szContent,"%d",pFindItem->GameUserInfo.uMidCount);
        //pList->InsertInItem(nUserCountInThisDesk+i, 8, CA2W(szContent));
        ////// 旁观对象
        //UserItemStruct *pWatchUser = FindOnLineUser(pFindItem->GameUserInfo.bDeskStation);
        //if (pWatchUser)
        //{
        //    sprintf(szContent,"%s", pWatchUser->GameUserInfo.nickName);
        //}
        //pList->InsertInItem(nUserCountInThisDesk+i, 9, CA2W(szContent));
        //// 魅力
        //sprintf(szContent,"%d", pFindItem->GameUserInfo.dwFascination);
        //pList->InsertInItem(nUserCountInThisDesk+i, 10, CA2W(szContent));
        //// 魅力级别
        //sprintf(szContent,"%s", GetCharmGrade(pFindItem->GameUserInfo.dwFascination));
        //pList->InsertInItem(nUserCountInThisDesk+i, 11, CA2W(szContent));


		char szContent[300]={0};
		map<string, int>::iterator ite; 

		// 昵称
		ite = m_MapColumn.find("Nickname"); 
		if (m_MapColumn.end() != ite)
		{
			sprintf(szContent,"%s",pFindItem->GameUserInfo.nickName);
			pList->InsertInItem(nUserCountInThisDesk+i, ite->second, CA2W(szContent));
		}

		// 乐豆
		ite = m_MapColumn.find("Coin"); 
		if (m_MapColumn.end() != ite)
		{
			// 乐豆
			GlbGetNumString(szContent, pFindItem->GameUserInfo.i64Money, Glb().m_nPowerOfGold, Glb().m_bUseSpace, TEXT(","));
			pList->InsertInItem(nUserCountInThisDesk+i, ite->second, CA2W(szContent));
		}
		// 财富级别
		ite = m_MapColumn.find("WealthLevel"); 
		if (m_MapColumn.end() != ite)
		{
			sprintf(szContent,"%s",GetMoneyOrderName(pFindItem->GameUserInfo.i64Money));
			pList->InsertInItem(nUserCountInThisDesk+i, ite->second, CA2W(szContent));
		}

		// 积分
		ite = m_MapColumn.find("Score"); 
		if (m_MapColumn.end() != ite)
		{
			sprintf(szContent,"%d",pFindItem->GameUserInfo.dwPoint-GetNormalOrderBasePoint(pFindItem->GameUserInfo.dwPoint));
			pList->InsertInItem(nUserCountInThisDesk+i, ite->second, CA2W(szContent));
		}

		// 积分级别
		ite = m_MapColumn.find("ScoreLevel"); 
		if (m_MapColumn.end() != ite)
		{
			sprintf(szContent,"%s", m_GameInfo.pOrderName(pFindItem->GameUserInfo.dwPoint));
			pList->InsertInItem(nUserCountInThisDesk+i, ite->second, CA2W(szContent));
		}

		// 总局数
		ite = m_MapColumn.find("GameCount"); 
		if (m_MapColumn.end() != ite)
		{
			UINT uAllCount1=pFindItem->GameUserInfo.uWinCount+pFindItem->GameUserInfo.uLostCount+pFindItem->GameUserInfo.uMidCount;
			sprintf(szContent,"%d",uAllCount1);
			pList->InsertInItem(nUserCountInThisDesk+i, ite->second, CA2W(szContent));
		}

		// 胜局数
		ite = m_MapColumn.find("WinGame"); 
		if (m_MapColumn.end() != ite)
		{
			sprintf(szContent,"%d",pFindItem->GameUserInfo.uWinCount);
			pList->InsertInItem(nUserCountInThisDesk+i, ite->second, CA2W(szContent));
		}
		// 输局数
		ite = m_MapColumn.find("LoseGame"); 
		if (m_MapColumn.end() != ite)
		{
			sprintf(szContent,"%d",pFindItem->GameUserInfo.uLostCount);
			pList->InsertInItem(nUserCountInThisDesk+i, ite->second, CA2W(szContent));
		}
		// 和局数
		ite = m_MapColumn.find("Peace"); 
		if (m_MapColumn.end() != ite)
		{
			sprintf(szContent,"%d",pFindItem->GameUserInfo.uMidCount);
			pList->InsertInItem(nUserCountInThisDesk+i, ite->second, CA2W(szContent));
		}

		//// 旁观对象
		ite = m_MapColumn.find("WatchUser"); 
		
		if (m_MapColumn.end() != ite)
		{
			UserItemStruct *pWatchUser = FindOnLineUser(pFindItem->GameUserInfo.bDeskStation);
			
			if (pWatchUser)
			{
				sprintf(szContent,"%s", pWatchUser->GameUserInfo.nickName);
				
			}
			//sprintf(szContent,"");
			pList->InsertInItem(nUserCountInThisDesk+i, ite->second, CA2W(szContent));
		}
		// 魅力
		ite = m_MapColumn.find("Charm"); 
		if (m_MapColumn.end() != ite)
		{
			sprintf(szContent,"%d", pFindItem->GameUserInfo.dwFascination);
			pList->InsertInItem(nUserCountInThisDesk+i, ite->second, CA2W(szContent));
		}
		// 魅力级别
		ite = m_MapColumn.find("CharmLevel"); 
		if (m_MapColumn.end() != ite)
		{
			sprintf(szContent,"%s", GetCharmGrade(pFindItem->GameUserInfo.dwFascination));
			pList->InsertInItem(nUserCountInThisDesk+i, ite->second, CA2W(szContent));
		}

		CString stime;
		stime.Format("%d",CTime::GetCurrentTime());        //当前的时间，String格式
		int curtime=atoi(stime);                        //转换为int

		
		if(pFindItem->GameUserInfo.iVipTime>=curtime)
		{
			pList->SetRowFontColor(i, 0xff0000);
		}
		else
		{
			TCHAR szPath[MAX_PATH];
			wsprintf(szPath,"GameOption.bcf");
			CBcfFile f(szPath);
			int r,g,b;
			r=f.GetKeyVal("GameList","GameListTextR",0);
			g=f.GetKeyVal("GameList","GameListTextG",255);
			b=f.GetKeyVal("GameList","GameListTextB",255);
			COLORREF GameListTextColor=RGB(r,g,b);
			pList->SetRowFontColor(i, GameListTextColor);
		}
       

    }

	
    return 0;
}



/// 初始化道具列表
int XLogic::InitPropPannel()
{
    IContain *pSingleProp = NULL;
    GETCTRL(IContain, pSingleProp, m_pIUI, RIGHTBOARD_PROP_PROP_1);
    if (NULL==pSingleProp)
    {
        return -1;
    }
    pSingleProp->SetControlVisible(false);
    IContain *pPropBoard = NULL;
    GETCTRL(IContain, pPropBoard, m_pIUI, RIGHTBOARD_PROP_CTN);
    if (NULL==pPropBoard)
    {
        return -2;
    }
    pPropBoard->GetContainPaginator()->SetNumPerPage(PROP_PEER_PAGE+1);
    pPropBoard->GetContainPaginator()->SetSpaces(15, 5, 15, 5, 2, 5);

    IBCInterFace *pBtnBuyVip = m_pIUI->GetIControlbyID(BN_BUY_VIP);
    if (NULL!=pBtnBuyVip)
    {
        pBtnBuyVip->SetControlVisible(false);
        pPropBoard->GetContainPaginator()->InsertPagecontent(pBtnBuyVip);
    }    

	/// 调整道具数量控件的宽度
	IText* pPropNumText = dynamic_cast<IText*>(m_pIUI->GetIControlbyID(723));
	IButton* pPropBtn = dynamic_cast<IButton*>(m_pIUI->GetIControlbyID(722));
	if (pPropNumText && pPropBtn)
	{
		pPropNumText->SetGameW(pPropBtn->GetGameW());
		pPropNumText->SetGameXY(pPropBtn->GetGameX(),pPropNumText->GetGameY());
	}

    /// 克隆道具按钮
    for (int i=0; i<PROP_PEER_PAGE; ++i)
    {
        m_pPropContain[i] = dynamic_cast<IContain *>(pSingleProp->Clone(PROP_BTN_BEGIN+PROP_IDS*i));
        if (NULL!=m_pPropContain[i])
        {
            pPropBoard->GetContainPaginator()->InsertPagecontent(m_pPropContain[i]);
        }
    }
    CString s=CBcfFile::GetAppPath();
    CBcfFile fini(s+"..\\bzgame.bcf");
    m_strPropUrl=fini.GetKeyVal("BZW","PropPannelUrl","http://game.bzw.cn/app/Prop/PropInfo.asp");

    CBcfFile f( s + "..\\skin.bcf");
    CString key=TEXT("PropPannel");
    int r,g,b;
    m_bkcolor_R = f.GetKeyVal(key,"PropPannelR",255);
    m_bkcolor_G = f.GetKeyVal(key,"PropPannelG",255);
    m_bkcolor_B = f.GetKeyVal(key,"PropPannelB",255);

    if(NULL==m_pPropPannel)
    {
        m_pPropPannel=new CPropPannel();
        m_pPropPannel->m_pGameInfo = &m_GameInfo;
        m_pPropPannel->m_pParent = m_pEngine->GetCWnd();
		m_pPropPannel->m_bUsingNewUI = true;
		m_pPropPannel->m_pLogic = this;
        AfxSetResourceHandle(GetModuleHandle(FACE_DLL_NAME));
        m_pPropPannel->Create(IDD_PROPPANNEL,m_pPropPannel->m_pParent);
        AfxSetResourceHandle(GetModuleHandle(NULL));
        m_pPropPannel->CenterWindow();
        m_pPropPannel->ResetData();
    }
    if(m_pPropSelUser==NULL)
    {
        m_pPropSelUser=new CPropSelUser();
        AfxSetResourceHandle(GetModuleHandle(FACE_DLL_NAME));
        m_pPropSelUser->Create(IDD_PROP_SEL_USER,m_pEngine->GetCWnd());
        AfxSetResourceHandle(GetModuleHandle(NULL));
    }
    if(m_dlgBroadcast==NULL)
    {
        m_dlgBroadcast=new CBoardCast();
        AfxSetResourceHandle(GetModuleHandle(FACE_DLL_NAME));
        m_dlgBroadcast->Create(IDD_BOARDCAST,m_pEngine->GetCWnd());
        AfxSetResourceHandle(GetModuleHandle(NULL));
        m_dlgBroadcast->pParentWnd=m_pEngine->GetCWnd();
    }
    return 0;
}

int XLogic::RefreshPropPannel()
{
    const int nMaxPage = Glb().userPropLibrary.GetCount()/PROP_PEER_PAGE; 
    if (m_nPropPage>nMaxPage)
    {
        m_nPropPage = nMaxPage;
    }
    else if (m_nPropPage<0)
    {
        m_nPropPage = 0;

    }

    if (nMaxPage==m_nPropPage)
    {
        UIEnableButton(RIGHTBOARD_PROP_BTN_NEXT1, false);
        UIEnableButton(RIGHTBOARD_PROP_BTN_NEXT2, false);
        UIEnableButton(RIGHTBOARD_PROP_BTN_PRE1, true);
        UIEnableButton(RIGHTBOARD_PROP_BTN_PRE2, true);
    }   
    else if (0==m_nPropPage)
    {
        UIEnableButton(RIGHTBOARD_PROP_BTN_NEXT1, true);
        UIEnableButton(RIGHTBOARD_PROP_BTN_NEXT2, true);
        UIEnableButton(RIGHTBOARD_PROP_BTN_PRE1, false);
        UIEnableButton(RIGHTBOARD_PROP_BTN_PRE2, false);
    }
    else
    {
        UIEnableButton(RIGHTBOARD_PROP_BTN_NEXT1, true);
        UIEnableButton(RIGHTBOARD_PROP_BTN_NEXT2, true);
        UIEnableButton(RIGHTBOARD_PROP_BTN_PRE1, true);
        UIEnableButton(RIGHTBOARD_PROP_BTN_PRE2, true);
    }
    


    wchar_t wszPropImagePath[MAX_PATH];
    wsprintfW(wszPropImagePath, L"gamecomm\\right\\prop\\");
    wchar_t wszPropImage[MAX_PATH]={0};

    /// 显示道具
    for (int i=m_nPropPage*PROP_PEER_PAGE; i<m_nPropPage*PROP_PEER_PAGE+PROP_PEER_PAGE; ++i)
    {
        IButton *pButton = NULL;
        IText   *pTextNum = NULL;
        GETCTRL(IButton, pButton, m_pIUI, PROP_BTN_BEGIN+PROP_IDS*(i%PROP_PEER_PAGE)+1);
        GETCTRL(IText, pTextNum, m_pIUI, PROP_BTN_BEGIN+PROP_IDS*(i%PROP_PEER_PAGE)+2);

		if (!pButton || !pTextNum)
		{
			continue;
		}

        if (i<Glb().userPropLibrary.GetCount())
        {
            _TAG_USERPROP *userProp=Glb().userPropLibrary.GetAt(i);
            if (NULL!=pButton && NULL!=pTextNum && userProp!=NULL)
            {
                wsprintfW(wszPropImage, L"%sProp%d.bmp", wszPropImagePath, i+1);
                pButton->LoadPic(wszPropImage);
                wchar_t wszNum[100]={0};            
                wsprintfW(wszNum, L"%d", userProp->nHoldCount);
                pTextNum->SetText(wszNum);
            }
        }
        else
        {
            wsprintfW(wszPropImage, L"%sPropEmpty.bmp", wszPropImagePath);
            pButton->LoadPic(wszPropImage);
            pTextNum->SetText(L"0");      
        }

    }    


    if (m_pPropPannel!=NULL)
    {
        m_pPropPannel->ResetData();
    }

	if (m_pRightFrame)
	{
		m_pRightFrame->OnSetPropImages();
	}

    return 0;
}

bool XLogic::UIEnableButton(int nID, bool bEnable)
{
    IButton *pbtn=NULL;
    GETCTRL(IButton, pbtn, m_pIUI, nID);
    if (NULL!=pbtn)
    {
        pbtn->SetEnable(bEnable);
        return bEnable;
    }

    return false;
}

void XLogic::OnHitPropBtn(int nIndex)
{
    m_pPropPannel->setSkinIndex(0);
    m_pPropPannel->openPannel(0,"", nIndex);

    //int nIndexInList = nIndex+m_nPropPage*PROP_PEER_PAGE;
    //if (nIndexInList>=Glb().userPropLibrary.GetCount())
    //{
    //    return;
    //}
    //_TAG_USERPROP *userProp=Glb().userPropLibrary.GetAt(nIndexInList);

    ///// 打开道具页面
    //IContain *pContain = dynamic_cast<IContain *>(m_pIUI->GetIControlbyID(BUYPROP_CTN));
    //IWeb *pWeb = dynamic_cast<IWeb *>(m_pIUI->GetIControlbyID(BUYPROP_WEB_PROPINFO));
    //IEditorText *pOwnNum = dynamic_cast<IEditorText *>(m_pIUI->GetIControlbyID(BUYPROP_TEXT_OWNNUM));
    //IEditorText *pBuyNum = dynamic_cast<IEditorText *>(m_pIUI->GetIControlbyID(BUYPROP_TEXT_BUYNUM));
    //IButton *pBtnUse = dynamic_cast<IButton *>(m_pIUI->GetIControlbyID(BUYPROP_BTN_USE));
    //IButton *pBtnBuy = dynamic_cast<IButton *>(m_pIUI->GetIControlbyID(BUYPROP_BTN_BUY));
    //IButton *pBtnClose = dynamic_cast<IButton *>(m_pIUI->GetIControlbyID(BUYPROP_BTN_CLOSE));
    //IText *pNeedGold = dynamic_cast<IText *>(m_pIUI->GetIControlbyID(BUYPROP_TEXT_NEEDGOLD));
    //IText *pHaveGold = dynamic_cast<IText *>(m_pIUI->GetIControlbyID(BUYPROP_TEXT_HAVEGOLD));

    //if (pContain!=NULL)
    //{
    //    pContain->SetControlVisible(true);
    //}

    //// 道具页面
    //if (pWeb!=NULL)
    //{
    //    pWeb->SetUseWindow(true);
    //    CString surl;
    //    surl.Format("%s?propid=%d&bgcolor=%X%X%X",m_strPropUrl,userProp->nPropID,m_bkcolor_R,m_bkcolor_G,m_bkcolor_B);
    //    pWeb->SetShowWebAddress(CA2W(surl.GetBuffer()));
    //}    
    //// 拥有数量
    //if (pOwnNum!=NULL)
    //{
    //    wchar_t wszOwnNum[200]={0};
    //    wsprintfW(wszOwnNum, L"%d", userProp->nHoldCount);
    //    pOwnNum->SetText(wszOwnNum);
    //    pOwnNum->SetEnable(false);
    //}  
    //// 购买数量
    //if (pBuyNum!=NULL)
    //{
    //    pBuyNum->SetText(L"1");
    //    pBuyNum->SetEnableASCII(1, '0', '9');
    //    pBuyNum->SetMaxTypeIn(3);
    //}
    //// 所需金币
    //if (pNeedGold!=NULL)
    //{
    //    pNeedGold->SetText(L"");
    //}   
    //// 银行金币
    //if (pHaveGold!=NULL)
    //{
    //    char szNum[32]; 
    //    GlbGetNumString(szNum, m_GameInfo.uisMeUserInfo.GameUserInfo.dwBank, Glb().m_nPowerOfGold, Glb().m_bUseSpace, Glb().m_strSpaceChar);
    //    wchar_t wszMoneyText[300];
    //    wsprintfW(wszMoneyText, L"购买道具使用的是您银行中的金币，您现在银行中有 %s 金币。", CA2W(szNum));
    //    pHaveGold->SetText(wszMoneyText);
    //}    
}

// 设置框
void XLogic::OnSetUIMessage(TUIMessage *pMessage)
{
    if (pMessage->_uMessageType==UI_LBUTTONUP)
    {
        IContain *pCtn = dynamic_cast<IContain *>(m_pIUI->GetIControlbyID(CTN_SET_DLG));
        ISlider *pSliderVol = dynamic_cast<ISlider *>(m_pIUI->GetIControlbyID(SLIDER_SET_VOL));
        IRadioButton *pEnableWatch = dynamic_cast<IRadioButton *>(m_pIUI->GetIControlbyID(CHECK_ENABLEWATCH));

        if (NULL==pCtn || NULL==pSliderVol || NULL==pEnableWatch)
        {
            return;
        }

        switch (pMessage->_uControlID)
        {
        case BTN_SET:
            {
                // 显示设置框
                pCtn->SetControlVisible(true);
            }
            break;
        case BTN_SET_CLOSE:
            {
                // 显示设置框
                pCtn->SetControlVisible(false);
            }
            break;
        case BTN_SET_OK:
            {
                // 显示设置框
                pCtn->SetControlVisible(false);
            }
            break;
        case BTN_SET_CANCEL:
            {
                // 显示设置框
                pCtn->SetControlVisible(false);
            }
            break;
        case CHECK_ENABLEWATCH:
            {
                // 允许旁观
            }
            break;
        default:
            break;
        }
    }
}
void XLogic::OnBuyPropUIMessage(TUIMessage *pMessage)
{
    //switch (pMessage->_uControlID)
    //{    
    //case BUYPROP_TEXT_BUYNUM:   // 购买数量 
    //    {

    //    }
    //    break;
    //case BUYPROP_BTN_USE:       // 使用
    //    {

    //    }
    //    break;
    //case BUYPROP_BTN_BUY:       // 购买
    //    {

    //    }
    //    break;
    //case BUYPROP_BTN_CLOSE:     // 关闭
    //    {
    //        if (pMessage->_uMessageType==UI_LBUTTONUP)
    //        {
    //            IContain *pContain = dynamic_cast<IContain *>(m_pIUI->GetIControlbyID(BUYPROP_CTN));
    //            if (NULL!=pContain)
    //            {
    //                pContain->SetControlVisible(false);
    //            }
    //        }            
    //    }
    //    break;
    //default:
    //    break;
    //}
}


// 使用道具     // PengJiLin, 2010-10-14, 增加一参数
void XLogic::UsePropItem(int propIndex, BOOL bCheckNum)
{
    //////////////////////////////////////////////////////////////////////////
    //Modify Fred Huang,2008-04-23
    if(GetMeUserInfo()->bUserState==USER_WATCH_GAME)
    {
        AFCMessageBox("对不起，旁观时不能使用道具！","提示");
        return;
    }
	if (m_GameInfo.dwRoomRule & GRR_CONTEST)
	{
		AFCMessageBox("比赛场不能使用道具!");
		return;
	}
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    //2008-08-11
    int propCount=(int)Glb().userPropLibrary.GetCount();
    if(propIndex<0 || propIndex>=propCount)
    {
        //没有道具，购买道具
        //OnGetBuyItemData(1);
        return;
    }
    _TAG_USERPROP * userProp=Glb().userPropLibrary.GetAt(propIndex);
    if(userProp==NULL) return;

    // PengJiLin, 2010-10-14, 道具的数量可以为0，须处理
    if(userProp->nHoldCount <= 0 && TRUE == bCheckNum)
    {
		TCHAR chInfo[256] = {0};
		wsprintf(chInfo, "抱歉，[%s]已经使用完毕，请先购买再使用！", userProp->szPropName);
		if(IDOK == AFCMessageBox(chInfo))
		{
			return ;
		}
		return ; 
        //TCHAR chInfo[256] = {0};
        //wsprintf(chInfo, "抱歉，[%s]已经使用完毕，是否现在购买并使用?", userProp->szPropName);
        //if(IDOK == AFCMessageBox(chInfo, TEXT("提示"), AFC_YESNO, this))  // 确定购买
        //{
        //    BuyAndUseProp(propIndex);
        //    return;
        //}
        //else        // 取消购买
        //{
        //    return;
        //}
    }

    if(userProp->attribAction & 0x80 || userProp->attribAction &0x100)
    {
        //大喇叭和小喇叭
        UseBoardCast( userProp );
        return;
    }

    // PengJiLin, 2010-6-1, 记牌器 功能
    if(userProp->attribAction & 0x200)
    {
        //this->UseLookCardProp(userProp);
        return;
    }

    // PengJiLin, 2010-9-9, 踢人卡功能
    if(userProp->attribAction & 0x400)
    {
        UseKickUserProp(userProp);
        return;
    }

    // PengJiLin, 2010-9-9, 防踢卡功能
    if(userProp->attribAction & 0x800)
    {
        UseAntiKickProp(userProp);
        return;
    }

    //区分对自己使用，还是对其它人使用
    if(userProp->attribAction & 0x01)
    {
        //对其它人使用
        if(m_pPropSelUser==NULL)
        {
            m_pPropSelUser=new CPropSelUser;
            AfxSetResourceHandle(GetModuleHandle(FACE_DLL_NAME));
            m_pPropSelUser->Create(IDD_PROP_SEL_USER,m_pEngine->GetCWnd());
            AfxSetResourceHandle(GetModuleHandle(NULL));
        }
        INT_PTR uActiveCount=m_ThisDeskPtrArray.GetCount();
        for (int i=0;i<uActiveCount;i++)
        {
            UserItemStruct * pFindUserItem=(UserItemStruct *)m_ThisDeskPtrArray.GetAt(i);
            m_pPropSelUser->m_pUserItem[i] = pFindUserItem;
            /*if ((pFindUserItem!=NULL)&&(pFindUserItem->GameUserInfo.dwUserID==dwUserID)) 
                return pFindUserItem;*/
        }
        //for(int i=0;i<MAX_PEOPLE;i++)
        //{
        //    m_pPropSelUser->m_pUserItem[i]=m_pUserInfo[i];
        //}
        m_pPropSelUser->dwUsedPropID=propIndex;
        m_pPropSelUser->dwLocalUserID=GetMeUserInfo()->dwUserID;
        m_pPropSelUser->propItemName=userProp->szPropName;
        m_pPropSelUser->resetInformation();
        m_pPropSelUser->CenterWindow();
        m_pPropSelUser->ShowWindow(SW_SHOW);		
    }
    else
    {
        //对自己使用
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

        UINT uRet = AFCMessageBox(stip,"提示",AFC_YESNO,m_pEngine->GetCWnd());
        if(uRet==IDOK)
        {
            OnUseProp(propIndex,GetMeUserInfo()->dwUserID);
        }
        ////////////////////////////////////////////////////
    }

    return;
}

BOOL XLogic::OnUseProp(int nPropIndex, int nDeskUserID)
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

//用户使用广播，对话框里确定后，在这里给用户ID和用户名赋值
void XLogic::OnUseBoardcast(WPARAM wparam, LPARAM lparam)
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

    // 这里的lParam是一个_TAG_BOARDCAST后面附加一个int
    BYTE *buffer = (BYTE *)lparam;
    if (NULL != m_pIPC)
    {
        m_pIPC->SendData(IPC_MAIN_PROP, IPC_SUB_BROADCAST_BIG, buffer, wparam);
    }
    delete buffer;
    buffer = NULL;

    /*this->m_pGameRoom->PostMessage(WM_USE_BOARDCAST, wparam, lparam);*/
}

//银行界面发来的消息
void XLogic::OnBankMessage(UINT message,WPARAM wParam,LPARAM lParam)
{
    /// 银行界面发来的修改密码
    if (message == WM_CHANGE_PASSWD)
    {
        OnChangePasswd( (ChangePasswd_t *)(lParam) );
        return;
    }

    /// 银行界面发来的转帐记录请求
    if (message == WM_TRANSFER_RECORD)
    {
        OnTransferRecord();
        return;
    }	/// 银行界面发来的转帐消息
    if (message == WM_TRANSFER_MONEY)
    {
        OnTransferMoney( (TransferMoney_t *)(lParam) );
        return;
    }

    /// 银行界面发来的存取钱消息
    if (message == WM_CHECK_MONEY)
    {
        OnCheckMoney( (CheckMoney_t *)(lParam) );
        return;
    }

    // lxl, 2010-11-17, 银行界面发来的获取用户昵称的消息
    if(WM_BANK_GET_NICKNAME_ONID == message)
    {
        OnGetNickNameOnID((GetNickNameOnID_t*)(lParam));
        return;
    }

	//新版银行存取款
	if (WM_CHECK_MONEY2 == message)
	{
		OnCheckMoney2((bank_op_normal*)(lParam));
		return;
	}

	

	//JianGuankun 2012-1-2,关闭银行窗口时使用
	if (WM_BANK2_CLOSE == message)
	{
		OnCloseBank2();
		return;
	}

	//JianGuankun 2012.9.28 收到银行存取成功的消息通知游戏
	if (WM_BANK2_CHECK_SUC == message)
	{
		TUIMessage tagUIMsg;
		tagUIMsg._uMessageType = UI_BANK4GAME_CHECK;
		tagUIMsg._ch = (char)lParam;
		OnUIMessage(&tagUIMsg);
	}

}

//关闭银行
void XLogic::OnCloseBank2()
{
	if (m_pBank2Wnd)
	{	
		delete m_pBank2Wnd;
		m_pBank2Wnd = NULL;
	}

	return;
}


/// 根据全局变量中的加密方式，取得加密后的密码字符串可能是sha可能是MD5
/// 函数里不对指针的合法性作判断
/// @param szMD5Pass 加密后的字符串
/// @param szSrcPass 源字符串
/// @return 加密后的字符串指针
TCHAR *XLogic::GetCryptedPasswd(TCHAR *szMD5Pass, TCHAR *szSrcPass)
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
void XLogic::OnTransferRecord()
{
    MSG_GR_S_TransferRecord_t transferRecord;
    transferRecord.dwUserID = GetMeUserInfo()->dwUserID;
    /// 只带用户ID的带参数的消息
    SendGameData(&transferRecord, sizeof(transferRecord), MDM_GR_MONEY, ASS_GR_TRANSFER_RECORD_INGAME, 0);
}
/// 银行界面发送指令，修改密码
/// @param pChangePasswd 修改密码结构指针，在函数执行结束后要删除该指针
/// @return 无
void XLogic::OnChangePasswd( ChangePasswd_t *pChangePasswd )
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
void XLogic::OnTransferMoney( TransferMoney_t *pTransferMoney )
{
    CBcfFile fMsg(CBcfFile::GetAppPath()+"..\\ClientMessage.bcf");
    MSG_GR_S_TransferMoney transferMoney;
    transferMoney.i64Money = pTransferMoney->i64Money;
    transferMoney.destUserID = pTransferMoney->uDestUserID;
    transferMoney.UserID = GetMeUserInfo()->dwUserID;
    strcpy(transferMoney.szNickName,GetMeUserInfo()->nickName);		//add by lxl 转账提示信息显示昵称
    strcpy(transferMoney.szDestNickName, pTransferMoney->szDestNickName);
	transferMoney.bUseDestID = pTransferMoney->bUseDestID;
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
void XLogic::OnCheckMoney( CheckMoney_t *pCheckMoney )
{
    CBcfFile fMsg(CBcfFile::GetAppPath()+"..\\ClientMessage.bcf");
    /// 判断是存还是取
    if (pCheckMoney->uType==0) /// 存钱，不需要密码
    {
        __int64 i64Left = GetMeUserInfo()->i64Money - pCheckMoney->i64Money;
        if (i64Left < 0 || i64Left < m_GameInfo.uLessPoint)
        {
            CString strBuffer;
            TCHAR szNum[128];
            GlbGetNumString(szNum, m_GameInfo.uLessPoint, Glb().m_nPowerOfGold, Glb().m_bUseSpace, Glb().m_strSpaceChar);
            CString str = fMsg.GetKeyVal("GameRoom","NeedCoins","此房间需要您至少有 %s 的金币");
            strBuffer.Format(str, szNum);
            AFCMessageBox(strBuffer,m_GameInfo.szGameName);
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
void XLogic::OnCheckMoney2(bank_op_normal* pCheckMoney )
{
	SendGameData(pCheckMoney,sizeof(bank_op_normal),MDM_BANK,ASS_BANK_NORMAL,0);
	delete pCheckMoney;
	pCheckMoney = NULL;
	return;
}

// lxl, 2010-11-17, 根据用户ID获取昵称
void XLogic::OnGetNickNameOnID(GetNickNameOnID_t* pGetNickNameOnID)
{
    //OutputDebugString("发送消息给ExeForGame lxl");
    MSG_GR_S_GetNickNameOnID_t stGetNickNameOnID;
    stGetNickNameOnID.iUserID = pGetNickNameOnID->uUserID;

    SendGameData(&stGetNickNameOnID, sizeof(stGetNickNameOnID), MDM_GR_ROOM, ASS_GR_GET_NICKNAME_ONID_INGAME, 0);
    delete pGetNickNameOnID;
    pGetNickNameOnID = NULL;
}

void XLogic::OnBnClickedRightFrameHide()
{
	if(m_pRightFrame)
	{
		m_pRightFrame->ShowWindow(SW_HIDE);
		RECT rcShrink = {0,0,-m_pRightFrame->m_uOldWidth,0};
		m_pIUI->SetControlsShrinkArea(rcShrink);
	}
	return;
}

void XLogic::OnBnClickedRightFrameShow()
{
	if(m_pRightFrame)
	{
		int iWindowsWidth = GetSystemMetrics(SM_CXSCREEN);
		int iWindowsHeight = GetSystemMetrics(SM_CYSCREEN);
		int iRightFrameWidth = 0;

		WINDOWPLACEMENT place;
		GetWindowPlacement(m_pEngine->GetCWnd()->GetSafeHwnd(),&place);

		if (SW_SHOWMAXIMIZED == place.showCmd)
		{
			if (iWindowsWidth >= 1280)
			{
				iRightFrameWidth = m_pRightFrame->m_uMaxWidth - m_pRightFrame->m_uOldWidth;
			}
			else
			{
				iRightFrameWidth = m_pRightFrame->m_uMinWidth - m_pRightFrame->m_uOldWidth;
			}			
		}
		else
		{
			iRightFrameWidth = m_pRightFrame->m_uMinWidth - m_pRightFrame->m_uOldWidth;
		}

		RECT rcShrink = {0,0,iRightFrameWidth,0};
		m_pIUI->SetControlsShrinkArea(rcShrink);
		m_pRightFrame->ShowWindow(SW_SHOW);

	}
	return;
}

//道具消息
bool XLogic::OnPropMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize)
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

            RefreshPropPannel();
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


//处理道具消息
bool XLogic::OnUsePropResult(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize)
{
    if(pNetHead->bHandleCode!=DTK_GR_PROP_USE_SUCCEED)
    {
        switch(pNetHead->bHandleCode)
        {
        case DTK_GP_PROP_GIVE_NOTARGETUSER:
            //ShowMsgInGame("目标玩家不处于在线状态！");
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

    if(usingProp->dwUserID==m_GameInfo.uisMeUserInfo.GameUserInfo.dwUserID)
        name1="您";
    if(usingProp->dwTargetUserID==m_GameInfo.uisMeUserInfo.GameUserInfo.dwUserID)
        name2="您";
    CString name3=name2;
    if(usingProp->dwUserID==usingProp->dwTargetUserID)
        name2="自己";

    int attribAction=usingProp->nPropActionAttrib;
    int attribValue=usingProp->nPropValueAttrib;

    CString stip;
    stip.Format("%s 对 %s 使用了 %s 道具",name1,name2,usingProp->szPropName);
    InsertSystemMessage(stip.GetBuffer());


	CString _s = "B";
	if (usingProp->dwTargetUserID==m_GameInfo.uisMeUserInfo.GameUserInfo.dwUserID)
	{
		_s = "B";
	}
	if (usingProp->dwUserID==m_GameInfo.uisMeUserInfo.GameUserInfo.dwUserID)
	{
		_s = "A";
	}
	if (_s != "" && pUserInfo->GameUserInfo.bDeskNO == m_GameInfo.uisMeUserInfo.GameUserInfo.bDeskNO)
	{
		CString soundFile;
		soundFile.Format("..\\image\\prop\\Prop%d%s.mp3", usingProp->nPropID, _s);
		PlaySound(soundFile.GetBuffer());
	}


    //双倍积分
    if(attribAction & 0x4)
    {
        pUserInfo->GameUserInfo.iDoublePointTime=usingProp->iDoubleTime;
        if(usingProp->dwTargetUserID==m_GameInfo.uisMeUserInfo.GameUserInfo.dwUserID)
        {
            time_t t=(time_t)usingProp->iDoubleTime;
            CTime expiresTime(t);
            stip.Format(TEXT("您的双倍积分功能可以使用到 %s！"),expiresTime.Format("%Y-%m-%d %H:%M:%S"));
            InsertSystemMessage(stip.GetBuffer());
        }
    }
    if(attribAction & 0x8)
    {
        pUserInfo->GameUserInfo.iProtectTime=usingProp->iProtectTime;
        if(usingProp->dwUserID == m_GameInfo.uisMeUserInfo.GameUserInfo.dwUserID)
        {
            time_t t=(time_t)usingProp->iProtectTime;
            CTime expiresTime(t);
            stip.Format(TEXT("您的护身符功能(输了不扣积分)可以使用到 %s！"),expiresTime.Format("%Y-%m-%d %H:%M:%S"));
            InsertSystemMessage(stip.GetBuffer());
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
        InsertSystemMessage(stip.GetBuffer());
    }
    if(attribAction & 0x2)
    {
        CString str;
        str.Format("负分清0道具结果:%d 负分清0",usingProp->dwTargetUserID);
        //OutputDebugString(str);
        if(usingProp->dwTargetUserID==m_GameInfo.uisMeUserInfo.GameUserInfo.dwUserID)
        {
            if(m_GameInfo.uisMeUserInfo.GameUserInfo.dwPoint<0)
                m_GameInfo.uisMeUserInfo.GameUserInfo.dwPoint=0;
        }
        if (pUserInfoUsed->GameUserInfo.dwPoint<0)
        {
            pUserInfoUsed->GameUserInfo.dwPoint = 0;
        }
    }

	if (m_pUserListDu)
	{
		m_pUserListDu->UpdateGameUser(pUserInfoUsed);
	}

	if (m_pRightFrame)
	{
		m_pRightFrame->ShowUserInfo();
	}

    //更新玩家列表
    RefreshPlayList();

    return true;
}

//vip时间检测
bool XLogic::OnPropVipCheckTime(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize)
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
        InsertSystemMessage(strMessage.GetBuffer());
    }
    else
    {
        if(GetMeUserInfo()->iVipTime <=curtime)
        {
            //处理数据
            CString strMessage;
            strMessage.Format(TEXT("您的Vip身份已经结束，请您重新购买！"));
            InsertSystemMessage(strMessage.GetBuffer());
        }
    }
    //OutputDebugString("OnPropVipCheckTime");
    RefreshPropPannel();
    return true;
}

// PengJiLin, 2010-9-14, 使用踢人卡踢人结果消息
bool XLogic::OnNewKickUserResult(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize)
{
	if(uDataSize!=sizeof(_TAG_NEW_KICKUSER)) return false;

	_TAG_NEW_KICKUSER* pKickResult = (_TAG_NEW_KICKUSER*)pNetData;
     if(pKickResult != NULL && pKickResult->dwUserID == m_GameInfo.uisMeUserInfo.GameUserInfo.dwUserID)   // 自己踢别人
    {
        switch(pNetHead->bHandleCode)
        {
        case ERR_GR_NEW_KICKUSER_SUCCESS:       // 成功
            {
                UserItemStruct* pUserInfo = FindOnLineUser(pKickResult->dwDestID);                
                CString strInfo = "";
                if(NULL != pUserInfo) strInfo.Format("玩家%s已被踢出游戏桌", pUserInfo->GameUserInfo.nickName);
                else strInfo.Format("玩家%d已被踢出游戏桌", pKickResult->dwDestID);

				InsertSystemMessage(strInfo.GetBuffer());
                //ShowMessageDialog(strInfo.GetBuffer(), 0);
				//if(IDOK ==AFCMessageBox(strInfo.GetBuffer()))
				//{
				//	return true;
				//}
                //ShowNewKickPropDlg(TRUE, FALSE, TRUE, strInfo);
            }
            break;
        case ERR_GR_NEW_KICKUSER_NOTIME:        // 自己的道具已过期，或者没有道具
            {
                CString strInfo = "";
                strInfo.Format("您没有\"踢人卡\"道具，或者已经过期");

				InsertSystemMessage(strInfo.GetBuffer());
                //ShowMessageDialog(strInfo.GetBuffer(), 0);
				//if(IDOK ==AFCMessageBox(strInfo.GetBuffer()))
				//{
				//	return true;
				//}
                //ShowNewKickPropDlg(TRUE, TRUE, TRUE, strInfo);
            }
            break;
        case ERR_GR_NEW_KICKUSER_HAD_ANTI:      // 对方有防踢卡时间
            {
                CString strInfo = "该玩家使用了\"防踢卡\"，您不能将他踢出游戏桌";

				InsertSystemMessage(strInfo.GetBuffer());
                //strInfo.Format();
                //ShowMessageDialog(strInfo.GetBuffer(), 0);
				//if(IDOK ==AFCMessageBox(strInfo.GetBuffer()))
				//{
				//	return true;
				//}
                //ShowNewKickPropDlg(TRUE, FALSE, TRUE, strInfo);
            }
            break;
        case ERR_GR_NEW_KICKUSER_HAD_VIP:       // 对方有VIP时间
            {
                CString strInfo = "";
                strInfo.Format("该玩家是VIP会员，您不能将他踢出游戏桌");

				InsertSystemMessage(strInfo.GetBuffer());
                //ShowMessageDialog(strInfo.GetBuffer(), 0);
				//if(IDOK ==AFCMessageBox(strInfo.GetBuffer()))
				//{
				//	return true;
				//}
                //ShowNewKickPropDlg(TRUE, FALSE, TRUE, strInfo);
            }
            break;
        case ERR_GR_NEW_KICKUSER_PLAYING:       // 游戏中不踢人
            InsertSystemMessage("操作失败，对方正在游戏");
            break;
        default:
            break;
        }
    }

    return true;
}

void XLogic::UseAntiKickProp(_TAG_USERPROP* userProp)
{
    InsertSystemMessage(TEXT("您正在使用“防踢卡”道具。"));    // test

    MSG_GR_RS_KickProp KickPro;
    ZeroMemory(&KickPro,sizeof(MSG_GR_RS_KickProp));
    KickPro.dwUserID = GetMeUserInfo()->dwUserID;
    KickPro.iPropID = userProp->nPropID;

    SendGameData(&KickPro, sizeof(KickPro), MDM_GM_GAME_FRAME, ASS_GM_USE_ANTI_KICK_PROP, 0);

    return;
}

void XLogic::UseKickUserProp(_TAG_USERPROP* userProp)
{
    InsertSystemMessage(TEXT("您正在使用“踢人卡”道具。"));    // test

    MSG_GR_RS_KickProp KickPro;
    ZeroMemory(&KickPro,sizeof(MSG_GR_RS_KickProp));
    KickPro.dwUserID = GetMeUserInfo()->dwUserID;
    KickPro.iPropID = userProp->nPropID;

    SendGameData(&KickPro, sizeof(KickPro), MDM_GM_GAME_FRAME, ASS_GM_USE_KICK_PROP, 0);

    return;
}

//用户使用广播，对话框里确定后，在这里给用户ID和用户名赋值
void XLogic::UseBoardCast(_TAG_USERPROP * userProp)
{
    //if(userProp->attribAction & 0x80)   // 大喇叭
    //    ShowInputDlg(1);
    //else                                // 小喇叭
    //    ShowInputDlg(2);

    m_dlgBroadcast->m_nPropID=userProp->nPropID;

    if(userProp->attribAction & 0x80)
        m_dlgBroadcast->SetValue(BRD_MSG_BIG);
    else
        m_dlgBroadcast->SetValue(BRD_MSG_SMALL);
    AfxSetResourceHandle(GetModuleHandle(NULL));
    m_dlgBroadcast->ShowWindow(SW_SHOW);
}

///弹出大小喇叭输入框
int XLogic::ShowInputDlg(int type)
{
    IContain* _InputDlg;
    GETCTRL(IContain, _InputDlg, m_pIUI, CTN_MsgInput);
    if (NULL==_InputDlg)
    {
        return 0;
    }

    IText* _Tip;
    GETCTRL(IText, _Tip, m_pIUI, CTN_MsgInput_Tip);
    if (NULL==_Tip)
    {
        return 0;
    }

    IEditorText* _Edit;
    GETCTRL(IEditorText, _Edit, m_pIUI, CTN_MsgInput_Edit);
    if (NULL==_Edit)
    {
        return 0;
    }

    if (1==type)
    {
        _Tip->SetText(L"大喇叭限制发送127个汉字,254个英文");
        _Edit->SetMaxTypeIn(254);
    }
    else if (2==type)
    {
        _Tip->SetText(L"小喇叭限制发送60个汉字,120个英文");
        _Edit->SetMaxTypeIn(120);
    }
    else if (0==type)
    {
        _InputDlg->SetControlVisible(false);
        return 0;
    }
    _InputDlg->SetControlVisible(true);

    return 0;
}


///弹出对话框， fun为回调函数参数为一个bool值点确实和取消的值
void XLogic::ShowMessageDialog(TCHAR* szMsg, int itype)
{
    IContain* _Contain = dynamic_cast<IContain*>(m_pIUI->GetIControlbyID(CTN_DIALOG));
    if (_Contain != NULL)
    {
        IText *_Text;
        _Text = dynamic_cast<IText*>(m_pIUI->GetIControlbyID(TX_DLALOG_MSG));
        if (_Text != NULL)
        {
            _Text->SetText(CT2W(szMsg));
        }

        IButton* _b1 = dynamic_cast<IButton*>(m_pIUI->GetIControlbyID(BN_DLALOG_OK));
        IButton* _b2 = dynamic_cast<IButton*>(m_pIUI->GetIControlbyID(BN_DLALOG_CANCEL));

        if (_b1 != NULL && _b2 != NULL)
        {
            if (itype == 0)             // OK
            {
                _b1->SetControlVisible(true);
                _b2->SetControlVisible(false);
                _b1->SetGameXY(_Contain->GetGameX() + (_Contain->GetGameW() - _b1->GetGameW()) / 2, _b1->GetGameY());
            }
            else if (itype == 1)        // 确定 取消
            {
                _b1->SetControlVisible(true);
                _b2->SetControlVisible(true);
                int _w = (_Contain->GetGameW() - _b1->GetGameW() - _b2->GetGameW()) / 3;
                _b1->SetGameXY(_Contain->GetGameX() + _w, _b1->GetGameY());
                _b2->SetGameXY(_Contain->GetGameX() + 2 * _w + _b1->GetGameW(), _b2->GetGameY());
            }
            else if (itype == 2)        // 无按钮
            {
                _b1->SetControlVisible(false);
                _b2->SetControlVisible(false);
            }
            else if (itype == 3)        // 不显示
            {
                _Contain->SetControlVisible(false);
                return;
            }
        }

        _Contain->SetControlVisible(true);
    }
}


// PengJiLin, 2010-9-10, 踢人卡、防踢卡使用结果
bool XLogic::OnUseKickPropResult(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, BOOL bIsKick)
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

        ShowMessageDialog(strInfo.GetBuffer(), 0);
        //ShowNewKickPropDlg(TRUE, FALSE, TRUE, strInfo);
    }
    else
    {
        CString strValue = "";
        SecondFormat(iPropValue, strValue);

        CString strTotalValue = "";
        SecondFormat(pKickProp->iTotalTime, strTotalValue);

        CString strInfo = ""; 
        strInfo.Format("您已成功使用“防踢卡”道具，其他玩家\n无法将您踢出游戏，道具单次有效\n时间%s，总时间%s", 
            strValue, strTotalValue);

        ShowMessageDialog(strInfo.GetBuffer(), 0);
        //ShowNewKickPropDlg(TRUE, FALSE, FALSE, strInfo);
    }

    // 刷新
    RefreshPropPannel();

    return true;
}

// PengJiLin, 2010-9-10, 将秒数格式化为时分秒的格式
void XLogic::SecondFormat(int iSecond, CString& strOutput)
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


/// 处理银行的消息
/// 与客户端的GameRoomEx中大同小异，只操作本地金币数和银行面板数据的变化
/// @param[in] pNetHead 协议头
/// @param[in] pNetData 数据包
/// @param[in] uDataSize 数据包大小
/// @return 如果成功处理，则返回true，返回false表示处理该条消息失败，其中一种可能是因为协议不匹配，例如修改了头文件却没有都重新编译
/// 
bool XLogic::OnAboutMoney(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize)
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
            AFCMessageBox(strBuffer,m_GameInfo.szGameName);//,MB_OK,this);
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
                    AFCMessageBox(strBuffer,m_GameInfo.szGameName);//,MB_OK,this);
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
                    AFCMessageBox(strBuffer,m_GameInfo.szGameName);//,MB_OK,this);
                    break;
                }
            case ASS_GR_OPEN_WALLET_ERROR_PSW:
                {
                    CString strBuffer;
                    strBuffer.Format(TEXT("二级密码错误,请重试!"));
                    AFCMessageBox(strBuffer,m_GameInfo.szGameName);//,MB_OK,this);
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
                        AFCMessageBox(strBuffer,m_GameInfo.szGameName);//,MB_OK,this);
                    }

					/*if (pBank->nVer == 1) //老版银行
					{
						AfxSetResourceHandle(GetModuleHandle(FACE_DLL_NAME));
						//建立窗口
						if (m_pPersonBank == NULL) 
						{
							try
							{
								m_pPersonBank=new CPBankDlg(m_pEngine->GetCWnd());
								if (m_pPersonBank==NULL)
									return false;
								m_pPersonBank->m_pLogic = this;
							}
							catch (...) { return false; }
						}
						if (m_pPersonBank!=NULL)
						{
							DebugPrintf("游戏中给银行成员赋值dwBank=[%d]dwMoney=[%d]---%d行", pBank->i64Bank, m_GameInfo.uisMeUserInfo.GameUserInfo.i64Money
								, __LINE__);
							/// 先赋值
							m_pPersonBank->SetWealthInfor(pBank->i64Bank, m_GameInfo.uisMeUserInfo.GameUserInfo.i64Money);
							if (m_pPersonBank->GetSafeHwnd()==NULL)
							{
								AfxSetResourceHandle(GetModuleHandle(FACE_DLL_NAME));
								/// 给银行的成员赋值
								DebugPrintf("游戏中准备打开银行界面[%d]---%d行", m_pPersonBank->GetSafeHwnd()
									, __LINE__);
								m_pPersonBank->DoModal();//Create(IDD_DIALOGBANK,this);
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
// 							wsprintf(szKey, "%d", m_GameInfo.uNameID);
// 							int iResult = fsr.GetKeyVal (_T("BJLType"), szKey, 0);
// 
// 							if (iResult == 0)
// 							{
// 								return false;
// 							}

							m_pBank2Wnd = new CPBank4GameWnd(m_pEngine->GetCWnd(),&m_GameInfo);
							m_pBank2Wnd->Create(m_pEngine->GetCWnd()->m_hWnd,NULL,UI_WNDSTYLE_DIALOG,0,0,0,0,0,NULL);
							m_pBank2Wnd->SetBankVersion(pBank->nVer);
							m_pBank2Wnd->SetWealthInfor(pBank->i64Bank,GetMeUserInfo()->i64Money);
							m_pBank2Wnd->CenterWindow();
							strcpy(m_pBank2Wnd->m_szTwoPassword,pBank->szTwoPassword);
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
					DUIMessageBox(NULL,MB_OK|MB_ICONERROR,m_GameInfo.szGameName,strBuffer.GetBuffer());
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
                   DUIMessageBox(NULL,MB_OK|MB_ICONINFORMATION,m_GameInfo.szGameName,strBuffer.GetBuffer());
                }
            }

            if(pNetHead->bHandleCode==ASS_GR_CHECKOUT_SUC)
            {
                //效验数据
                if (uDataSize!=sizeof(MSG_GR_CheckMoney)) return false;
                MSG_GR_CheckMoney * pCheckOut=(MSG_GR_CheckMoney *)pNetData;
                if(pCheckOut->dwUserID==m_GameInfo.uisMeUserInfo.GameUserInfo.dwUserID)//自己
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

                m_pEngine->GetCWnd()->PostMessage(WM_BANK2_CHECK_SUC,0,1);
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
                    DUIMessageBox(NULL,MB_OK|MB_ICONINFORMATION,m_GameInfo.szGameName,strBuffer.GetBuffer());
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
					DUIMessageBox(NULL,MB_OK|MB_ICONINFORMATION,m_GameInfo.szGameName,strBuffer.GetBuffer());
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
                    DUIMessageBox(NULL,MB_OK|MB_ICONINFORMATION,m_GameInfo.szGameName,strBuffer.GetBuffer());
                }
            }

            if(pNetHead->bHandleCode==ASS_GR_CHECKIN_SUC)
            {
                //效验数据
                if (uDataSize!=sizeof(MSG_GR_CheckMoney)) return false;
                MSG_GR_CheckMoney * pCheckIn=(MSG_GR_CheckMoney *)pNetData;

                if(pCheckIn->dwUserID==m_GameInfo.uisMeUserInfo.GameUserInfo.dwUserID)//自己
                {
                    //更新对话框
                    if (m_pPersonBank!=NULL) 
                    {
                        m_pPersonBank->UpdateUserMoney(1, pCheckIn->i64CheckOut);
                    }
                }
                FlushUserMoney(pCheckIn->dwUserID,0-pCheckIn->i64CheckOut);

                m_pEngine->GetCWnd()->PostMessage(WM_BANK2_CHECK_SUC,0,0);
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
                        AFCMessageBox(strBuffer,m_GameInfo.szGameName);//,MB_OK,this);
                        return true;
                    }
                case ASS_GR_TRANSFER_TOTAL_LESS:										///< 银行总额太小，不够资格
                    {
                        GlbGetNumString(szNum, pTransfer->i64ActualTransfer, Glb().m_nPowerOfGold, Glb().m_bUseSpace, Glb().m_strSpaceChar);
                        str = fMsg.GetKeyVal("GameRoom","TransferTotalLess","您暂时还不可以转帐，银行存款达到或超过%s金币才可以转帐！");
                        strBuffer.Format(str, szNum);
                        AFCMessageBox(strBuffer,m_GameInfo.szGameName);//,MB_OK,this);
                        return true;
                    }
                    break;
                case ASS_GR_TRANSFER_TOO_LESS:											///< 单笔转帐数目太少
                    {
                        GlbGetNumString(szNum, pTransfer->i64ActualTransfer, Glb().m_nPowerOfGold, Glb().m_bUseSpace, Glb().m_strSpaceChar);
                        str = fMsg.GetKeyVal("GameRoom","TransferTooLess","转帐不成功，单次转帐金额必须大于%s金币。");
                        strBuffer.Format(str, szNum);
                        AFCMessageBox(strBuffer,m_GameInfo.szGameName);//,MB_OK,this);
                        return true;
                    }
                    break;
                case ASS_GR_TRANSFER_MULTIPLE:											///< 单笔转帐数目必须是某数的倍数
                    {
                        GlbGetNumString(szNum, pTransfer->i64ActualTransfer, Glb().m_nPowerOfGold, Glb().m_bUseSpace, Glb().m_strSpaceChar);
                        str = fMsg.GetKeyVal("GameRoom","TransferMultiple","转帐不成功！转帐金额必须是%s的整数倍。");
                        strBuffer.Format(str, szNum);
                        AFCMessageBox(strBuffer,m_GameInfo.szGameName);//,MB_OK,this);
                        return true;
                    }
                case ASS_GR_TRANSFER_NOT_ENOUGH:										///< 银行金额不够本次转帐
                    {
                        GlbGetNumString(szNum, pTransfer->i64Money, Glb().m_nPowerOfGold, Glb().m_bUseSpace, Glb().m_strSpaceChar);
                        GlbGetNumString(szBank, pTransfer->i64ActualTransfer, Glb().m_nPowerOfGold, Glb().m_bUseSpace, Glb().m_strSpaceChar);
                        str = fMsg.GetKeyVal("GameRoom","TransferNotEnoughMoney","转帐不成功！您银行中只有%s金币，不够支付本次转帐%s金币。");
                        strBuffer.Format(str, szBank, szNum);
                        AFCMessageBox(strBuffer,m_GameInfo.szGameName);//,MB_OK,this);
                        return true;
                    }
                case ASS_GR_TRANSFER_NO_DEST:											///< 转帐目标不存在
                    {
                        str = fMsg.GetKeyVal("GameRoom","AccountNotExist","转帐目标不存在,请查实帐号再试!");
                        strBuffer.Format(str);
                        AFCMessageBox(strBuffer,m_GameInfo.szGameName);//,MB_OK,this);
                        return true;
                    }
                    break;
                default:
                    {
                        CString strBuffer;
                        CString str;
                        str = fMsg.GetKeyVal("GameRoom","NetWorkBusySave","网络拥塞,存入失败!请稍后再试!");
                        strBuffer.Format(str);
                        AFCMessageBox(strBuffer,m_GameInfo.szGameName);//,MB_OK,this);
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
                if(pTransfer->UserID == m_GameInfo.uisMeUserInfo.GameUserInfo.dwUserID)//自己
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
                        AFCMessageBox(strBuffer,m_GameInfo.szGameName);//,MB_OK,this);
                    }
                    //更新对话框
                    if (m_pPersonBank!=NULL) 
                    {
                        m_pPersonBank->UpdateUserMoneyByTransfer(0, pTransfer->i64Money);
                    }
                }
                /// 转帐目标是自己
                else if (pTransfer->destUserID==m_GameInfo.uisMeUserInfo.GameUserInfo.dwUserID)
                {
                    if (ASS_GR_TRANSFER_MONEY_INGAME == pNetHead->bAssistantID)
                    {
                        str = fMsg.GetKeyVal("GameRoom","BeTransfered","玩家[%d]转给您 %s 金币，扣除手续费后实际到帐 %s 金币!");
                        strBuffer.Format(str, pTransfer->UserID, szNum, szAct);
                        AFCMessageBox(strBuffer,m_GameInfo.szGameName);//,MB_OK,this);
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
bool XLogic::FlushUserMoney(long dwUserID,__int64 i64Count)
{
    UserItemStruct * pUserItem=FindOnLineUser(dwUserID);
    if(pUserItem==NULL)
        return false;

    //设置数据
    pUserItem->GameUserInfo.i64Money += i64Count;
    pUserItem->GameUserInfo.i64Bank -= i64Count;//银行金币

    //更新信息
    ShowOneUserInfo(DeskStation2View(pUserItem->GameUserInfo.bDeskStation), pUserItem, true);

	if (m_pUserListDu)
	{
		m_pUserListDu->UpdateGameUser(pUserItem);
	}

	if (m_pRightFrame)
	{
		m_pRightFrame->ShowUserInfo();
	}

    //更新座位区用户
    RefreshPlayList();
    return true;
}



// 载入声音列表
void XLogic::LoadBkSoundList(void)
{
    if (!m_pBackAudio)
    {
        return;
    }
    TCHAR sz[61],szPath[MAX_PATH];
    lstrcpy(sz,m_GameInfo.szProcessName);
    sz[lstrlen(sz)-lstrlen(".ico")]='\0';
    wsprintf(szPath,"GameOption.bcf");
    //AfxMessageBox(szPath);
    //DebugPrintf("szPath = [%s]", szPath);
    CBcfFile f(szPath);
    int		soundCount=f.GetKeyVal("BGSOUND","SoundCount",0);
    //DebugPrintf("SoundCount = [%d]", soundCount);
    if(soundCount==0)
        return;

    CString soundFile;
    for(int i=0;i<soundCount;i++)
    {
        wsprintf(sz,"Sound%d",i);
        soundFile=f.GetKeyVal("BGSOUND",sz,"");
        wsprintf(szPath,"%s",soundFile);
        //DebugPrintf("szPath = [%s]", szPath);
        if (m_pBackAudio)
        m_pBackAudio->Add(szPath);
    }
    int nVol=f.GetKeyVal("BGSOUND","SoundVolume",300);
    if (m_pBackAudio)
          m_pBackAudio->SetVolume(nVol*1.0f/1000.0f*100.0f);    
    m_bSndPause=f.GetKeyVal("BGSOUND","PauseSound",0);
    if (m_pBackAudio)
    {
        m_pBackAudio->Play();
        m_pBackAudio->SetLoopPlay(true);
    }

    if(m_bSndPause)
    {
        if (m_pBackAudio)
            m_pBackAudio->Pause();
        IButton *pSoundOn = dynamic_cast<IButton *>(m_pIUI->GetIControlbyID(BN_VOLUM_ON));
        IButton *pSoundOFF = dynamic_cast<IButton *>(m_pIUI->GetIControlbyID(BN_VOLUM_OFF));
        if (pSoundOn && pSoundOFF)
        {
            pSoundOn->SetControlVisible(true);
            pSoundOFF->SetControlVisible(false);
        }
    }
    else
    {
        if (m_pBackAudio)
        m_pBackAudio->Continue();
        IButton *pSoundOn = dynamic_cast<IButton *>(m_pIUI->GetIControlbyID(BN_VOLUM_ON));
        IButton *pSoundOFF = dynamic_cast<IButton *>(m_pIUI->GetIControlbyID(BN_VOLUM_OFF));
        if (pSoundOn && pSoundOFF)
        {
            pSoundOn->SetControlVisible(false);
            pSoundOFF->SetControlVisible(true);
        }
    }
}


void XLogic::OnPropBuyVIP(UINT message,WPARAM wParam,LPARAM lParam)
{
	BYTE *buffer = (BYTE *)lParam;
	if (NULL != m_pIPC)
	{
		m_pIPC->SendData(IPC_PROP_BUY_VIP, IPC_ASS_BUY_VIP, buffer, wParam);
		_TAG_PROP_BUY_VIP *pPropBuy = (_TAG_PROP_BUY_VIP *)lParam;
	}
	// 子窗口发来的是new出来的指针，此时删除
	delete buffer;
	buffer = NULL;
}
bool XLogic::UserProp(_TAG_PROP_MOIVE_USE *propMoive)
{
	int attribAction=propMoive->nPropActionAttrib;
	int attribValue=propMoive->nPropValueAttrib;


	if(attribAction & 0x40)
	{
		int iValue=(attribValue & 0x7F000000)>>24;
		m_stProp.iValue=iValue;
		m_stProp.sendDeskStation=propMoive->nUserStation;
		m_stProp.recvDeskStation=propMoive->nTargetUserStation;

		CString strFilePathName; 
		strFilePathName.Format("..\\image\\prop\\PropGif%dA.bani",iValue%1000); //modify by lxl  图片格式修改gif->spr

		PlayProp(true,strFilePathName);
		}
		//
		//SetTimer(ID_GIF_TIMER,150,NULL);	
		////{添加动画道具声音 zxj 2009-9-17
		//CString soundFile;
		//soundFile.Format("..\\image\\prop\\PropGif%dA.wav",m_stProp.iValue%1000);
		//PlaySound(soundFile.GetBuffer());
		//添加结束}
	
	return true;
}

void XLogic::DecodeString(CString str, CString &strColumn, int &width)
{
	int pos;
	CString strTemp;

	strTemp = str;
	pos = strTemp.Find(',');
	if (pos >= 0)
	{
		strColumn = strTemp.Left(pos);

		strTemp = strTemp.Right(strTemp.GetLength()-pos-1);
		if (!strTemp.IsEmpty())
		{
			width = atoi(strTemp);
		}
	}	
}
int XLogic::GetMoneyGrade(__int64 dwPoint)
{
	CString s = CBcfFile::GetAppPath();
	CBcfFile f("..\\Order.bcf");
	CString m_key;
	m_key.Format("PlatMoneyOrderName%d", m_GameInfo.uNameID);
	static int szOrderNum[50];
	CString strKey,strTmp;
	int tmpNum = 0;
	int nCount = 0;
	for (int i = 1;i <=50;i++)
	{
		strKey.Format("platOrder_%d",i);
		tmpNum=f.GetKeyVal(m_key,strKey,0);
		szOrderNum[i-1] = tmpNum;
		strTmp = f.GetKeyMemo(m_key,strKey,"");
		if (strTmp == "")
		{
			nCount = i-1;
			break;
		}
	}
	for (int i=0;i<nCount;i++)	
		if (dwPoint<szOrderNum[i]) 
			return i;
	return nCount;
}

void XLogic::OnMessageProc(UINT message,WPARAM wParam,LPARAM lParam)
{
	switch (message) {
		case WM_MOVE:
			{
				if (m_pPmdWnd)
				{
					m_pPmdWnd->AdjustPos((HWND)m_pIUI->GetWindowsHwnd());
					//m_pPmdWnd->ShowCtrl();
				}
			}
			break;
		case UM_FIXRIGHTFRAME:
			{
				if (m_pRightFrame)
				{
					CRect rcRight;
					CRect rcMain;
					::GetWindowRect(m_pRightFrame->GetHWND(),&rcRight);
					::GetWindowRect(m_pEngine->GetCWnd()->GetSafeHwnd(),rcMain);

					ISudoku* pSoduku = dynamic_cast<ISudoku*>(m_pIUI->GetIControlbyID(1));
					if(pSoduku)
					{
						RECT rcTop = pSoduku->GetSrcRect(2);
						RECT rcRight = pSoduku->GetSrcRect(1);
						RECT rcBottom = pSoduku->GetSrcRect(7);
						
						int iWindowsWidth = GetSystemMetrics(SM_CXSCREEN);
						int iWindowsHeight = GetSystemMetrics(SM_CYSCREEN);
						if (wParam == SW_SHOWMAXIMIZED)
						{
							if (iWindowsWidth >= 1280)
							{
								::MoveWindow(m_pRightFrame->GetHWND(),rcMain.right - m_pRightFrame->m_uMaxWidth - (rcRight.right - rcRight.left),
									rcMain.top + (rcTop.bottom - rcTop.top),m_pRightFrame->m_uMaxWidth,
									rcMain.Height() - (rcTop.bottom - rcTop.top) - (rcBottom.bottom - rcBottom.top),
									true);
							}
							else
							{
								::MoveWindow(m_pRightFrame->GetHWND(),rcMain.right - m_pRightFrame->m_uMinWidth - (rcRight.right - rcRight.left),
									rcMain.top + (rcTop.bottom - rcTop.top),m_pRightFrame->m_uMinWidth,
									rcMain.Height() - (rcTop.bottom - rcTop.top) - (rcBottom.bottom - rcBottom.top),
									true);
							}
						}
						else if (wParam == SW_HIDE || wParam == SW_SHOWNORMAL)
						{
							::MoveWindow(m_pRightFrame->GetHWND(),rcMain.right - m_pRightFrame->m_uMinWidth - (rcRight.right - rcRight.left),
								rcMain.top + (rcTop.bottom - rcTop.top),m_pRightFrame->m_uMinWidth,
								rcMain.Height() - (rcTop.bottom - rcTop.top) - (rcBottom.bottom - rcBottom.top),
								true);
						}
					}
				}
			}
			break;
	}
}

/// 聊天右键菜单响应
int XLogic::OnTalkMenuMessage(TUIMessage *pMessage)
{	
	IMenu *pMenu;
	GETCTRL(IMenu,pMenu,m_pIUI,TALK_MENU);	

	int iSel = pMenu->GetSelectIndex();
	if (iSel >= 0)
	{
		if (!wcscmp(pMenu->GetItemText(iSel),L"  清除屏幕"))
		{		
			ITalk* pTalk;
			GETCTRL(ITalk, pTalk, m_pIUI, TALK_SHOW);
			pTalk->ClearText();
			m_TalkMessages.clear();

		}
		else if (!wcscmp(pMenu->GetItemText(iSel),L"  复制"))
		{
			ITalk* pTalk;
			GETCTRL(ITalk, pTalk, m_pIUI, TALK_SHOW);
			pTalk->CopyAllText();
		}
		else if (!wcscmp(pMenu->GetItemText(iSel),L"  允许滚屏"))
		{
			ITalk* pTalk;
			GETCTRL(ITalk, pTalk, m_pIUI, TALK_SHOW);
			pTalk->SetIsAutoScroll(true);
			m_bIsAutoScroll = true;
		}
		else if (!wcscmp(pMenu->GetItemText(iSel),L"  接收聊天消息"))
		{
			m_bIsAcceptChatMsg = true;
		}
		else if (!wcscmp(pMenu->GetItemText(iSel),L"√ 允许滚屏"))
		{
			ITalk* pTalk;
			GETCTRL(ITalk, pTalk, m_pIUI, TALK_SHOW);
			pTalk->SetIsAutoScroll(false);
			m_bIsAutoScroll = false;
		}
		else if (!wcscmp(pMenu->GetItemText(iSel),L"√ 接收聊天消息"))
		{
			m_bIsAcceptChatMsg = false;
		}
	}

	return 0;
}


//查看用户信息
BOOL XLogic::OnCmdShowUserInfo()
{
	if (m_dwCurrentUserID != 0L)
	{
		UserItemStruct* pUserInfo = FindOnLineUser(m_dwCurrentUserID);

		if (NULL == pUserInfo)
		{
			pUserInfo = FindWatchUser(m_dwCurrentUserID);
		}

		if (NULL == pUserInfo)
		{
			return false;
		}

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

			::SetWindowPos(m_pUserDetailsDlg->GetSafeHwnd(),CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
			
		}
// 		else
// 		{
// 			CRect listRect;
// 			m_UserListDlg.GetWindowRect(&listRect);
// 			m_pUserDetailsDlg->MoveWindow(listRect.left - rect.Width() + 15, point.y - 10, rect.Width(), rect.Height());
// 		}

		m_pUserDetailsDlg->ShowWindow(SW_SHOW);
		
	}

	return TRUE;
}

//复制用户信息
BOOL XLogic::OnCmdCopyUserInfo()
{
	if (m_dwCurrentUserID != 0)
	{
		UserItemStruct* pUserItem = FindOnLineUser(m_dwCurrentUserID);

		HWND hwnd = m_pEngine->GetCWnd()->GetSafeHwnd();

		//处理数据
		if ((pUserItem==NULL)||(::OpenClipboard(hwnd)==FALSE))
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
	}

	return TRUE; 
}

//踢人
BOOL XLogic::OnCmdKickUser()
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
BOOL XLogic::OnCmdKickUserProp()
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


//添加好友
BOOL XLogic::OnAddFriend()
{
	return m_pIPC->SendData(IPC_FRIEND, IPC_ASS_CHECKFRIEND, &m_dwCurrentUserID, sizeof(int));
}

//添加黑名单
BOOL XLogic::OnAddEmeny()
{
	return m_pIPC->SendData(IPC_EMENY, IPC_ASS_CHECKEMENY, &m_dwCurrentUserID, sizeof(int));
}

//拍砖头
BOOL XLogic::OnCmdBlickProp()
{

	if (m_dwCurrentUserID!=0L)
	{
		// 旁观时不能T人
		if (GetMeUserInfo()->bUserState == USER_WATCH_GAME)
		{
			::DUIMessageBox(NULL,MB_OK|MB_ICONINFORMATION,m_GameInfo.szGameName,"对不起，您正在旁观游戏，不能进行此操作");
			return true;
		}
		// 检查是否为旁观
		if( m_bWatchMode )
		{
			::DUIMessageBox(NULL,MB_OK|MB_ICONINFORMATION,m_GameInfo.szGameName,"对不起，您正在旁观游戏，不能进行此操作");
			return true;
		}
		if(GetMeUserInfo()->dwUserID == m_dwCurrentUserID)
		{
			::DUIMessageBox(NULL,MB_OK|MB_ICONINFORMATION,m_GameInfo.szGameName,"对不起,无法对自己进行此项操作");
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
			::DUIMessageBox(NULL,MB_OK|MB_ICONINFORMATION,m_GameInfo.szGameName,"对不起,您已没有拍砖头道具，请购买。");
			return true;
		}

		_TAG_USINGPROP  usingProp;
		ZeroMemory(&usingProp,sizeof(_TAG_USINGPROP));
		usingProp.dwUserID = GetMeUserInfo()->dwUserID;
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
BOOL XLogic::OnCmdFlowerProp()
{
	if (m_dwCurrentUserID!=0L)
	{
		// 旁观时不能T人
		if (GetMeUserInfo()->bUserState == USER_WATCH_GAME)
		{
			::DUIMessageBox(NULL,MB_OK|MB_ICONINFORMATION,m_GameInfo.szGameName,"对不起，您正在旁观游戏，不能进行此操作");
			return true;
		}
		// 检查是否为旁观
		if( m_bWatchMode )
		{
			::DUIMessageBox(NULL,MB_OK|MB_ICONINFORMATION,m_GameInfo.szGameName,"对不起，您正在旁观游戏，不能进行此操作");
			return true;
		}
		if(GetMeUserInfo()->dwUserID == m_dwCurrentUserID)
		{
			::DUIMessageBox(NULL,MB_OK|MB_ICONINFORMATION,m_GameInfo.szGameName,"对不起,无法对自己进行此项操作");
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
			::DUIMessageBox(NULL,MB_OK|MB_ICONINFORMATION,m_GameInfo.szGameName,"对不起,您已没有送鲜花道具，请购买。");
			return true;
		}

		_TAG_USINGPROP  usingProp;
		ZeroMemory(&usingProp,sizeof(_TAG_USINGPROP));
		usingProp.dwUserID = GetMeUserInfo()->dwUserID;
		usingProp.dwTargetUserID = m_dwCurrentUserID;
		usingProp.nPropID = userProp->nPropID;
		usingProp.nPropActionAttrib=userProp->attribAction;
		usingProp.nPropValueAttrib=userProp->attribValue;

		memcpy(usingProp.szPropName,userProp->szPropName,sizeof(userProp->szPropName));
		SendGameData(&usingProp,sizeof(_TAG_USINGPROP),MDM_GR_PROP,ASS_PROP_USEPROP,0);
	}

	return TRUE;
}
