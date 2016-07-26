#include "stdafx.h"
#include "NewGameClient.h"
//#include "IGameFrame.h"
#include "../common/IGameFrame.h"
#include "UI_ForLogic/GameIDDefines.h"
#include "UI_ForLogic/IUserInterface.h"
#include "GameControlsIDDefine.h"
#include "Common/UIMessageDefines.h"

static  bool  AIChooseCard = false ;   

void MsgPrint(const char *lpszFormat, ...)
{
	va_list pArg;
	va_start(pArg, lpszFormat);
	char szMessage[1024] = { 0 };
#ifdef WIN32
	_vsnprintf(szMessage, 1023, lpszFormat, pArg);
#else
	vsnprintf(szMessage, 1023, lpszFormat, pArg);
#endif
	va_end(pArg);
	OutputDebugString(szMessage);
}

IGameImpl *CreateNewGameImpl(IGameFrame* pIBase, IUserInterface *pIUI)
{
	return dynamic_cast<IGameImpl*>(new CNewGameClient(pIBase, pIUI));
}
void DestroyNewGameImpl(IGameImpl **ppImp)
{
	delete *ppImp;
	*ppImp = NULL;
}
//构造函数
CNewGameClient::CNewGameClient(IGameFrame *pGameFrame, IUserInterface *pUI)
{
	m_pGameFrame = pGameFrame;
	m_pUI = pUI;
	m_SoundEngineGame = createIrrKlangDevice();
	m_SoundEnginBg = createIrrKlangDevice() ; 
	
	InitialGameData() ; 
}
//析构函数
CNewGameClient::~CNewGameClient()
{
	if(m_SoundEngineGame)
	{
		m_SoundEngineGame->drop();
	}

	if(m_SoundEnginBg)
	{
		m_SoundEnginBg->drop() ; 
	} 
}
//-----------------------------------------------------------------------------------
/// 初始化
int CNewGameClient::Initial()
{
	//载入UI
	wchar_t wszUIFile[MAX_PATH];
	::swprintf_s(wszUIFile,L"%d_UI.dat",NAME_ID);
	m_pUI->LoadData(wszUIFile);

	m_isSuperDialogShow = false;
	m_isWinOrLose = FALSE;
	return 0;
}
//-----------------------------------------------------------------------------------
/// 初始化UI
int CNewGameClient::InitUI()
{
	SetControlVisible(CTN_1014089_CTN_3000459,false);
	SetControlVisible(CTN_1014089_CTN_3000469,false);
	SetControlVisible(CTN_1014089_CTN_3000478,false);
	SetControlVisible(CTN_1014089_BTN_10001,false);
	SetControlVisible(CTN_1014089_OperateCard_3000489,false);
	SetControlVisible(CTN_1014089_BTN_3000493,false);
	SetControlVisible(CTN_1014089_CTN_3000509,false);

	SetControlVisible(CTN_1014089_BTN_3000492,false);

	CString appPath = CINIFile::GetAppPath();
	char szfileName[MAX_PATH] = {0};
	CString strFile;
	strFile.Format("./resources/sounds/bg.wav");
	wsprintf(szfileName,"%s%s",appPath,strFile);
	m_SoundEnginBg->setSoundVolume(0.4);
	m_SoundEnginBg->play2D(szfileName,true);
	return 0;
}
/// 玩家进入
int CNewGameClient::GameUserCome(void)
{
	m_iMyDeskStation = m_pGameFrame->GetMyDeskStation(); 
	return 0;
}
//--------------------------------------------------------------------------------------------------------
//设置按钮是否可用
void	CNewGameClient::SetBtEnable(const int iButtonID, bool bFlag)
{

	IButton *pButton = NULL;
	pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(iButtonID));
	if (NULL != pButton)
	{
		pButton->SetEnable(bFlag);
	}
}

//--------------------------------------------------------------------------------------------------------
//设置时间框显示倒计时
void	CNewGameClient::SetTimePlay(const int iTimeID,bool bFlag, BYTE byMaxTime, BYTE byMinTime)
{
	ITime *pTime = NULL;
	pTime = dynamic_cast<ITime *>(m_pUI->GetIControlbyID(iTimeID));
	if (NULL != pTime)
	{
		pTime->SetShowMaxNum(byMaxTime);
		pTime->SetShowMinNum(byMinTime);
		pTime->SetPLay(bFlag);
		pTime->SetControlVisible(bFlag);
	}

}
//设置图片控件加载显示指定的文件
void	CNewGameClient::SetImageLoadSrc(const int iImageID,CString sImagSrc,bool bFlag)
{
	IImage *pImage = NULL;
	pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(iImageID));
	if (NULL != pImage)
	{
		pImage->LoadPic(CA2W(sImagSrc));
		pImage->SetControlVisible(bFlag);
	}
}
//制定控件是否显示
bool CNewGameClient::GetControlShow(const int iID)
{
	IBCInterFace *pControl = NULL;
	pControl = dynamic_cast<IBCInterFace *>(m_pUI->GetIControlbyID(iID));
	if (NULL != iID)
	{
		return pControl->GetVisible();
	}
}

//设定控件的x,y,w,h
void CNewGameClient::SetControlPosition(const int iID,int x,int y,int w,int h)
{
	IBCInterFace *pControl = NULL;
	pControl = dynamic_cast<IBCInterFace *>(m_pUI->GetIControlbyID(iID));
	if (NULL != iID)
	{
		pControl->SetGameXY(x,y);
		pControl->SetGameW(w);
		pControl->SetGameH(h);
	}
}
/*
功能：设置容器控件不响应任何鼠标时间
参数：iContainID:控件ID；bFlag：是否响应
return:void
*/
void  CNewGameClient::SetContainEnable(const int iContainID,bool bFlag)
{
	IContain *pContain = NULL;
	pContain = dynamic_cast<IContain *>(m_pUI->GetIControlbyID(iContainID));
	if (NULL != pContain)
	{
		pContain->SetEnable(bFlag);
	}
}

//--------------------------------------------------------------------------------------------------------
//设置文本控件显示内容(显示数字)
void	CNewGameClient::SetTextinfo(const int iTextID,__int64 iNum, bool bFlag)
{
	IText *pText = NULL;
	pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(iTextID));
	if (NULL != pText)
	{
		CString sText;
		sText.Format("%I64d",iNum);
		pText->SetText(CA2W(sText));
		pText->SetControlVisible(bFlag);
	}
}

//--------------------------------------------------------------------------------------------------------
//设置文本控件显示内容(显示字符)
void	CNewGameClient::SetTextShowText(const int iTextID,TCHAR ch[], bool bFlag)
{
	IText *pText = NULL;
	pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(iTextID));
	if (NULL != pText)
	{
		CString sText;
		sText.Format("%s",ch);
		pText->SetText(CA2W(sText));
		pText->SetControlVisible(bFlag);
	}
}

//设置数字空间显示内容
void	CNewGameClient::SetInumInfo(const int iNumID, __int64 i64Num,bool bFlag)
{
	INum *pNum = NULL;
	pNum = dynamic_cast<INum *>(m_pUI->GetIControlbyID(iNumID));
	if (NULL != pNum)
	{
		pNum->SetNum(i64Num);
		pNum->SetControlVisible(bFlag);
	}
}
//设置新形式数字控件显示内容（带加号、减号、逗号）
void CNewGameClient::SetNewInumInfo(const int iNumID, __int64 i64Num,bool bFlag)
{
	/*CString cs;
	cs.Format("lwlog::SetNewInumInfo=%d",iNumID);
	OutputDebugString(cs);*/
	INum *pNum = NULL;
	pNum = dynamic_cast<INum *>(m_pUI->GetIControlbyID(iNumID));
	if (NULL != pNum)
	{
		NEW_NUM_STRUCT NumStruct;
		NumStruct.bShowPlusSignOrMinusSign = true;
		NumStruct.bShowComma = true;
		NumStruct.iShowCommaIndex = 3;
		NumStruct.iShowZeroType = 0;
		pNum->SetNewNumStruct(NumStruct);
		pNum->SetNewNum(i64Num);
		pNum->SetControlVisible(bFlag);
	}
}
//--------------------------------------------------------------------------------------------------------
//设置筹码控件显示值
void	CNewGameClient::SetShCtrlInfo(const int iShCtrlID, int iNum, bool bFlag)
{
	IShCtrl *pShCtrl = NULL;
	pShCtrl = dynamic_cast<IShCtrl *>(m_pUI->GetIControlbyID(iShCtrlID));
	if (NULL != pShCtrl)
	{
		pShCtrl->SetShNote(iNum);
		pShCtrl->SetControlVisible(bFlag);
	}

}
//设置控件位置
void CNewGameClient::SetControlPos(const int iControlID,int x,int y)
{
	IBCInterFace  *pControl = NULL ; 
	pControl = dynamic_cast<IBCInterFace *>(m_pUI->GetIControlbyID(iControlID)); 
	if(NULL != pControl)
	{
		pControl->SetGameXY(x,y) ; 
	}
}
//根据图片控件设置显示图片的尺寸
void CNewGameClient::SetPicSize(const int iButtonID,CString sImagSrc, bool bFlag,int x,int y,int w,int h)
{
	IImage *pImage = NULL;
	pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(iButtonID));
	if (NULL != pImage)
	{
		pImage->LoadPic(CA2W(sImagSrc));
		pImage->SetSrcX(x); 
		pImage->SetSrcY(y); 
		pImage->SetSrcW(w); 
		pImage->SetSrcH(h); 
		pImage->SetControlVisible(bFlag);
	}
}

//--------------------------------------------------------------------------------------------------------
//设置单选框控件是否被选中
void	CNewGameClient::SetRButtonSelected(const int iRButtonID,bool bSelected)
{
	IRadioButton *pRadioButton = NULL;
	pRadioButton = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(iRButtonID));
	if (NULL != pRadioButton)
	{
		pRadioButton->SetIsSelect(bSelected);
	}
}

//--------------------------------------------------------------------------------------------------------
//获取单选框控件是否被选中
BOOL	CNewGameClient::GetRButtonSelected(const int iRButtonID,bool *bSelected)
{
	IRadioButton *pRadioButton = NULL;
	pRadioButton = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(iRButtonID));
	if (NULL != pRadioButton)
	{
		*bSelected=pRadioButton->GetIsSelect();
		return (*bSelected);
	}
	return TRUE;
}
//设置指定动画显示指定的帧数
void CNewGameClient::PlayIExAnimate(const int iID,int x,int y, bool bFlag)
{
	IExAnimate *pControl = NULL;
	pControl = dynamic_cast<IExAnimate *>(m_pUI->GetIControlbyID(iID));
	if (NULL != pControl)
	{
		pControl->SetIsSymmetrical(false);
		pControl->SetGameXY(x,y);
		pControl->SetIsLoop(false);
		pControl->SetCurveInstance(CT_NONE);
		pControl->SetPlayState(bFlag);
		//第几帧
		//pControl->SetShowFrame(iFramNum);
		pControl->SetControlVisible(bFlag);
	}
}
//设置指定动画显示指定的帧数
void CNewGameClient::CloseAnimate(const int iID)
{
	IExAnimate *pControl = NULL;
	pControl = dynamic_cast<IExAnimate *>(m_pUI->GetIControlbyID(iID));
	if (NULL != pControl)
	{
		pControl->SetPlayState(false);
		pControl->SetControlVisible(false);
	}
}
//CString转TCHAR
TCHAR* CNewGameClient::CStringToTCHAR(CString &str) 
{ 
	int iLen = str.GetLength(); 
	TCHAR* szRs = new TCHAR[iLen]; 
	lstrcpy(szRs, str.GetBuffer(iLen)); 
	str.ReleaseBuffer(); 
	return szRs; 
} 
/*
功能：可操作牌发牌
形参：bCardList[]牌值;cardCount要发几张牌
返回值：void
*/
void CNewGameClient::ShowOperateCard(const int iControlID,BYTE bCardList[],BYTE cardCount)
{
	IOperateCard  *pOperateCard = NULL ; 
	pOperateCard = dynamic_cast<IOperateCard *>(m_pUI->GetIControlbyID(iControlID))  ; 
	if(NULL != pOperateCard)
	{
		pOperateCard->SetControlVisible(true) ; 
		pOperateCard->SetCardList(bCardList , cardCount) ; 
	}
}
/*
功能：不可操作牌发牌
形参：bCardList[]牌值;cardCount要发几张牌
返回值：void
*/
void CNewGameClient::ShowNoMoveCard(const int iControlID,BYTE bCardList[],BYTE cardCount)
{

	INoMoveCard  *pOperate = NULL ;
	pOperate = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(iControlID))  ; 
	if(NULL != pOperate)
	{
		pOperate->SetControlVisible(true) ; 
		pOperate->SetCardList(bCardList , cardCount) ; 
	}
}
//获得图片控件是否显示
BOOL CNewGameClient::GetNoMoveIsVisible(const int iControlID)
{
	INoMoveCard  *pOperate = NULL ;
	pOperate = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(iControlID))  ; 
	if(NULL != pOperate)
	{
		return pOperate->GetVisible();
	}
}
/*
功能：根据提示升起牌
形参：iControlID,可操作牌ID,bCardList[]牌值;cardCount要发几张牌
返回值：void
*/
void CNewGameClient::ShowUpCard(const int iControlID,BYTE bUpCardList[],BYTE bUpCardCount)
{
	IOperateCard  *pOperateCard1 = NULL ; 
	pOperateCard1 = dynamic_cast<IOperateCard *>(m_pUI->GetIControlbyID(iControlID))  ; 
	if(NULL != pOperateCard1)
	{
		pOperateCard1->SetControlVisible(true) ; 
		pOperateCard1->SetUpCardList(bUpCardList,bUpCardCount);
	}
}
/*
功能：获取升起的牌
形参：iControlID,可操作牌ID,bCardList升起的牌值
返回值：void
*/
void CNewGameClient::GetUpCard(const int iControlID,BYTE bCardList[])
{
	IOperateCard  *pOperateCard = NULL ; 
	pOperateCard = dynamic_cast<IOperateCard *>(m_pUI->GetIControlbyID(iControlID)); 
	memset(bCardList,255,sizeof(bCardList));
	if(NULL != pOperateCard)
	{
		pOperateCard->SetControlVisible(true) ; 
		pOperateCard->GetUpCardList(bCardList) ; 
	}
}
//设置控件是否可见
void CNewGameClient::SetControlVisible(const int iControlID,bool bFlag)
{
	IBCInterFace  *pControl = NULL ; 
	pControl = dynamic_cast<IBCInterFace *>(m_pUI->GetIControlbyID(iControlID)); 
	if(NULL != pControl)
	{
		pControl->SetControlVisible(bFlag) ; 
	}
}
//-------------------------------------------------------------------------------
/// 置状态
void CNewGameClient::SetStationParameter(BYTE bGameStation)
{
	m_byGameStation = (ENUM_GameStation)bGameStation; 
}
//-------------------------------------------------------------------------------
///获取游戏状态
BYTE CNewGameClient::GetStationParameter()
{
	//OutputDebugString("hs [Client] 2: CNewGameClient::GetStationParameter");
	return m_byGameStation;
}

//---------------------------------------------------------------------------------------
///初始化游戏中的数据
void	CNewGameClient::InitialGameData()
{
	m_iThinkTime=0;//游戏思考时间
	m_iBeginTime=0;//游戏开始时间		
	m_iMyDeskStation = 255;	///玩家自己的游戏位子

	m_ClockTickCount = TellGameModeTime;	//时钟计时数

	//准备按钮可见
	//SetControlVisible(CTN_1014089_BTN_3000492,true);

	SetStationParameter(/*GAMESTATION_NOPLAYER*/GS_NOMODE);
	//重置游戏数据
	ResetGameData();
}

//恢复游戏状态
void CNewGameClient::GameOverReset()
{
	//TODO:初始化牌数据
	memset(m_byFirstHeap,0,sizeof(m_byFirstHeap));
	memset(m_bySecondHeap,0,sizeof(m_bySecondHeap));
	memset(m_byOwnCards,0,sizeof(m_byOwnCards));

	//准备按钮可见
	SetImageLoadSrc(CTN_1014089_BTN_3000492,CString("./resources/image/准备.png"),true);
	SetControlVisible(CTN_1014089_BTN_3000492,true);
	SetBtEnable(CTN_1014089_BTN_3000492,true);

	int ibaseControlID = 3000700;
	IText *pText = NULL;
	for (int i = 0; i<PLAY_COUNT;++i)
	{
		int ibaseControlIDnew = ibaseControlID + 4*i;

		pText = dynamic_cast<IText*>(m_pUI->GetIControlbyID(ibaseControlIDnew));
		if (pText == NULL)
			return;
		pText->SetText(L"");	
		// 			CString cs;
		// 			cs.Format(_T("hs %s"),wbuffer);
		// 			OutputDebugString(cs);

		//昵称
		pText = dynamic_cast<IText*>(m_pUI->GetIControlbyID(ibaseControlIDnew+1));
		if (pText == NULL)
			return;
		pText->SetText(L"");	

		//得分
		pText = dynamic_cast<IText*>(m_pUI->GetIControlbyID(ibaseControlIDnew +2));
		if (pText == NULL)
			return;
		pText->SetText(L"");

		//牌型
		pText = dynamic_cast<IText*>(m_pUI->GetIControlbyID(ibaseControlIDnew +3));
		if (pText == NULL)
			return;
		pText->SetText(L"");
	}
	//////隐藏 牌、结算控件、以及其他控件
	//牌
	SetControlVisible(CTN_1014089_NoMoveCard_3000487,false);		// 0-all
	SetControlVisible(CTN_1014089_NoMoveCard_3000488,false);		// 1-all
	SetControlVisible(CTN_1014089_OperateCard_3000489,false);		// 2-all
	SetControlVisible(CTN_1014089_NoMoveCard_3000490,false);		// 3-all

	SetControlVisible(CTN_1014089_NoMoveCard_3000499,false);		// 0-1st
	SetControlVisible(CTN_1014089_NoMoveCard_3000505,false);		// 1-1st
	SetControlVisible(CTN_1014089_NoMoveCard_3000502,false);		// 2-1st
	SetControlVisible(CTN_1014089_NoMoveCard_3000503,false);		// 3-1st

	SetControlVisible(CTN_1014089_NoMoveCard_3000504,false);		// 0-2th
	SetControlVisible(CTN_1014089_NoMoveCard_3000501,false);		// 1-2th
	SetControlVisible(CTN_1014089_NoMoveCard_3000506,false);		// 2-2th
	SetControlVisible(CTN_1014089_NoMoveCard_3000507,false);		// 3-2th
	
	//牌型
	SetControlVisible(CTN_1014089_IMG_3000518,false);				//0
	SetControlVisible(CTN_1014089_IMG_3000519,false);				//1
	SetControlVisible(CTN_1014089_IMG_3000508,false);				//2
	SetControlVisible(CTN_1014089_IMG_3000520,false);				//3

	//结算容器
	SetControlVisible(CTN_1014089_CTN_3000522,false);

	//分牌按钮
	SetControlVisible(CTN_1014089_BTN_3000493,false);

}

//重置游戏数据
void CNewGameClient::ResetGameData()
{																			
	memset(m_bUserPrepare,0,sizeof(m_bUserPrepare));
	
	SetControlVisible(CTN_1014089_BTN_3000493,false);
	
}
//重置UI界面 
void	CNewGameClient::ResetGameDialog()
{

}
//------------------------------------------------------------------------------
/// 界面消息
int CNewGameClient::OnUIMessage(TUIMessage* pMessage)
{
	CString appPath = CINIFile::GetAppPath();
	CString soundPath = CString("/resources/sounds/button.wav");
	char szFileName[MAX_PATH] = {0};

	m_SoundEngineGame->setSoundVolume(1.0);
	m_SoundEngineGame->play2D(szFileName,false);
	
	//按钮弹起的消息
	bool RBselect;
	if(pMessage->_uMessageType == UI_LBUTTONUP)
		{
			if(m_pGameFrame->GetIsWatching())
			{
				return 0;
			}
			switch(pMessage->_uControlID)
			{
			case CTN_1014089_CTN_3000459_RButton_3000465:	 ///<单选框-轮庄
				{
					m_SoundEngineGame->play2D(szFileName,false);
					if (GetRButtonSelected(CTN_1014089_CTN_3000459_RButton_3000466,&RBselect))
						SetRButtonSelected(CTN_1014089_CTN_3000459_RButton_3000466,false);
					if (GetRButtonSelected(CTN_1014089_CTN_3000459_RButton_3000467,&RBselect))
						SetRButtonSelected(CTN_1014089_CTN_3000459_RButton_3000467,false);
				}
				break;
			case CTN_1014089_CTN_3000459_RButton_3000466:	 ///<单选框-定庄
				{
					m_SoundEngineGame->play2D(szFileName,false);
					if (GetRButtonSelected(CTN_1014089_CTN_3000459_RButton_3000465,&RBselect))
						SetRButtonSelected(CTN_1014089_CTN_3000459_RButton_3000465,false);
					if (GetRButtonSelected(CTN_1014089_CTN_3000459_RButton_3000467,&RBselect))
						SetRButtonSelected(CTN_1014089_CTN_3000459_RButton_3000467,false);
				}
				
				break;
			case CTN_1014089_CTN_3000459_RButton_3000467:	 ///<单选框-抢庄
				{
					m_SoundEngineGame->play2D(szFileName,false);
					if (GetRButtonSelected(CTN_1014089_CTN_3000459_RButton_3000465,&RBselect))
						SetRButtonSelected(CTN_1014089_CTN_3000459_RButton_3000465,false);
					if (GetRButtonSelected(CTN_1014089_CTN_3000459_RButton_3000466,&RBselect))
						SetRButtonSelected(CTN_1014089_CTN_3000459_RButton_3000466,false);
				}
				
				break;

			case CTN_1014089_CTN_3000459_BTN_3000463:	//确定选择游戏模式
				{
					m_SoundEngineGame->play2D(szFileName,false);
					OnBtnSelectGameModeOK();
				}
				
				break;
			case CTN_1014089_CTN_3000459_BTN_3000464:	//退出游戏模式选择： 即退出进入的游戏桌
				{
					m_SoundEngineGame->play2D(szFileName,false);
					// TODO: 告诉服务器玩家退出
					ExitWithoutSelectMode();
				}
				
				break;
			case CTN_1014089_CTN_3000478_BTN_3000479:	//模式通知容器的确定按钮
				{
					m_SoundEngineGame->play2D(szFileName,false);
					m_pGameFrame->KillTimer(TIMER_SHOWGAMEMODEINFO);

					SetTimePlay(CTN_1014089_CTN_3000478_BTN_3000479,false,m_ClockTickCount);//确定按钮
					SetControlVisible(CTN_1014089_CTN_3000478,false);		//隐藏游戏模式通知按钮

					//SetControlVisible(CTN_1014089_BTN_3000492,true);	//设置准备按钮可见
				}
				break;
			case CTN_1014089_BTN_3000492:		//准备按钮
				{
					m_SoundEngineGame->play2D(szFileName,false);
					m_pGameFrame->KillTimer(TIME_BEGIN);

					SetImageLoadSrc(CTN_1014089_BTN_3000492,CString("./resources/image/准备好了.png"),true);
					SetBtEnable(CTN_1014089_BTN_3000492,false);	//准备按钮不可用状态
					SetControlVisible(CTN_1014089_Time_3000719,false);
					
					TGameStation tGameStation ;
					tGameStation.byGameDeskNO = m_pGameFrame->GetMyDeskStation();					
					//m_pGameFrame->SendGameData(&tGameStation,sizeof(tGameStation),MDM_GM_GAME_NOTIFY,GM_GETREADY,0);
					m_pGameFrame->ClickContinue();
				}
				break;
			case CTN_1014089_OperateCard_3000489:	//可操作牌
				{
					m_SoundEngineGame->play2D(szFileName,false);
					//设置分牌按钮是否可用
					if(GetUpNums(CTN_1014089_OperateCard_3000489) == 3)
						SetBtEnable(CTN_1014089_BTN_3000493,true);
					else
						SetBtEnable(CTN_1014089_BTN_3000493,false);						
				}
				break;
			case CTN_1014089_BTN_3000493:		//<分牌  按扭
				{
					m_SoundEngineGame->play2D(szFileName,false);
					//将分出的牌发送给服务器
				
					m_pGameFrame->KillTimer(TIMER_THINK);
					m_ClockTickCount = 15;
					SetControlVisible(CTN_1014089_Time_3000719,false);

					SendCardToServer();
					//发送完成后，将分牌按钮设置为不可见
					SetControlVisible(CTN_1014089_BTN_3000493,false);
					//将可操作牌设置为不可见
					SetControlVisible(CTN_1014089_OperateCard_3000489,false);
				}
				break;
			case CTN_1014089_CTN_3000509_BTN_3000510:	//注码1 按钮, 
				{
					m_SoundEngineGame->play2D(szFileName,false);
					m_pGameFrame->KillTimer(TIMER_BET);
					
					SetControlVisible(CTN_1014089_Time_3000719,false);	//隐藏定时器

					Betting(CTN_1014089_CTN_3000509_BTN_3000510);	//下注
					SetControlVisible(CTN_1014089_CTN_3000509,false);	//隐藏投注界面

				}
				break;
			case CTN_1014089_CTN_3000509_BTN_3000511:	//注码2 按钮
				{
					m_SoundEngineGame->play2D(szFileName,false);
					m_pGameFrame->KillTimer(TIMER_BET);
					
					SetControlVisible(CTN_1014089_Time_3000719,false);	//隐藏定时器
					Betting(CTN_1014089_CTN_3000509_BTN_3000511);	//下注
					SetControlVisible(CTN_1014089_CTN_3000509,false);	//隐藏投注界面

				}
				break;
			case CTN_1014089_CTN_3000509_BTN_3000512:	//注码3 按钮
				{
					m_SoundEngineGame->play2D(szFileName,false);
					m_pGameFrame->KillTimer(TIMER_BET);
					
					SetControlVisible(CTN_1014089_Time_3000719,false);	//隐藏定时器
					Betting(CTN_1014089_CTN_3000509_BTN_3000512);	//下注
					SetControlVisible(CTN_1014089_CTN_3000509,false);	//隐藏投注界面
					
				}
				break;
			case CTN_1014089_CTN_3000509_BTN_3000513:	//注码4 按钮
				{
					m_SoundEngineGame->play2D(szFileName,false);
					m_pGameFrame->KillTimer(TIMER_BET);
					m_ClockTickCount = 15;
					SetControlVisible(CTN_1014089_Time_3000719,false);	//隐藏定时器
					Betting(CTN_1014089_CTN_3000509_BTN_3000513);	//下注
					SetControlVisible(CTN_1014089_CTN_3000509,false);	//隐藏投注界面

				}
				break;
			case CTN_1014089_CTN_3000522_BTN_3000543:	//结算容器- 继续游戏
				{
					m_SoundEngineGame->play2D(szFileName,false);
					//重置客户端状态和数据、 发送消息给服务器使其重置游戏状态和数据
					m_pGameFrame->KillTimer(TIMER_SHOWSETTLE);	
					GameOverReset();				
				}
				break;
			case CTN_1014089_CTN_3000522_BTN_3000544:	//结算容器- 退出游戏
				{
					m_SoundEngineGame->play2D(szFileName,false);
					m_pGameFrame->KillTimer(TIMER_SHOWSETTLE);
					m_pGameFrame->CloseClient();
				}
				break;
			case CTN_1014089_CTN_3000727_BTN_3000728:	//抢庄
				{
					m_pGameFrame->KillTimer(TIMER_BETBANKER);
					TBetBanker tBetBanker;
					tBetBanker.isBet = true;
					tBetBanker.byDeskStation = m_pGameFrame->GetMyDeskStation();
					m_pGameFrame->SendGameData(&tBetBanker,sizeof(tBetBanker),MDM_GM_GAME_NOTIFY,GM_BETBANKER,0);

					SetControlVisible(CTN_1014089_CTN_3000727,false);
				}
				break;
			case CTN_1014089_CTN_3000727_BTN_3000729:	//不抢
				{
					m_pGameFrame->KillTimer(TIMER_BETBANKER);
					TBetBanker tBetBanker;
					tBetBanker.isBet = false;
					tBetBanker.byDeskStation = m_pGameFrame->GetMyDeskStation();
					m_pGameFrame->SendGameData(&tBetBanker,sizeof(tBetBanker),MDM_GM_GAME_NOTIFY,GM_BETBANKER,0);

					SetControlVisible(CTN_1014089_CTN_3000727,false);
				}
				break;
			#pragma region 超端
			case CTN_1014089_BTN_3000755:	//超端按钮
				{
					if(!m_isSuperDialogShow)
					{
						SetControlVisible(CTN_1014089_CTN_3000736,true);
					}
				}
				break;
			case CTN_1014089_CTN_3000736_BTN_3000754:	//超端-确定按钮
				{
					TSuper tsuper;
					bool bSelectWin;
					bool bSelectLose;
					GetRButtonSelected(CTN_1014089_CTN_3000736_RButton_3000737,&bSelectWin);
					GetRButtonSelected(CTN_1014089_CTN_3000736_RButton_3000743,&bSelectLose);
					if(!bSelectLose && !bSelectWin)
					{
						tsuper.IsSetSuper = false;
					}
					else
					{
						tsuper.IsSetSuper = true;
						if(bSelectWin)
							tsuper.isWinOrLose = TRUE;
						else
							tsuper.isWinOrLose = FALSE;
						
					}
					m_pGameFrame->SendGameData(&tsuper,sizeof(TSuper),MDM_GM_GAME_NOTIFY,ASS_WINORLOSE,0);
					SetControlVisible(CTN_1014089_CTN_3000736,false);
					m_isSuperDialogShow = false;
				}
				break;
			case CTN_1014089_CTN_3000736_BTN_3000753:	//超端-取消按钮
				{
					SetControlVisible(CTN_1014089_CTN_3000736,false);
					m_isSuperDialogShow = false;
				}
				break;
			case CTN_1014089_CTN_3000736_RButton_3000737:	//庄赢
				{
					SetWinOrLose(CTN_1014089_CTN_3000736_RButton_3000737,TRUE);
					SetRButtonSelected(CTN_1014089_CTN_3000736_RButton_3000743,false);
				}
				break;
			case CTN_1014089_CTN_3000736_RButton_3000743:	//庄输
				{
					SetWinOrLose(CTN_1014089_CTN_3000736_RButton_3000743,FALSE);
					SetRButtonSelected(CTN_1014089_CTN_3000736_RButton_3000737,false);
				}
				break;
			#pragma region comment

			//case CTN_1014089_CTN_3000736_RButton_3000739:	//闲1赢
			//	{
			//		SetWinOrLose(CTN_1014089_CTN_3000736_RButton_3000739,TRUE);
			//		SetRButtonSelected(CTN_1014089_CTN_3000736_RButton_3000744,false);
			//	}
			//	break;
			//case CTN_1014089_CTN_3000736_RButton_3000744://	闲1输
			//	{
			//		SetWinOrLose(CTN_1014089_CTN_3000736_RButton_3000744,FALSE);
			//		SetRButtonSelected(CTN_1014089_CTN_3000736_RButton_3000739,false);
			//	}
			//	break;
			//case CTN_1014089_CTN_3000736_RButton_3000741:	//闲2赢
			//	{
			//		SetWinOrLose(CTN_1014089_CTN_3000736_RButton_3000741,TRUE);
			//		SetRButtonSelected(CTN_1014089_CTN_3000736_RButton_3000745,false);
			//	}
			//	break;
			//case CTN_1014089_CTN_3000736_RButton_3000745:	//闲2输
			//	{
			//		SetWinOrLose(CTN_1014089_CTN_3000736_RButton_3000745,FALSE);
			//		SetRButtonSelected(CTN_1014089_CTN_3000736_RButton_3000741,false);
			//	}
			//	break;
			//case CTN_1014089_CTN_3000736_RButton_3000742:	//闲3赢
			//	{
			//		SetWinOrLose(CTN_1014089_CTN_3000736_RButton_3000742,TRUE);
			//		SetRButtonSelected(CTN_1014089_CTN_3000736_RButton_3000746,false);
			//	}
			//	break;
			//case CTN_1014089_CTN_3000736_RButton_3000746:	//闲3输
			//	{
			//		SetWinOrLose(CTN_1014089_CTN_3000736_RButton_3000746,FALSE);
			//		SetRButtonSelected(CTN_1014089_CTN_3000736_RButton_3000742,false);
			//	}
			//	break;
			#pragma endregion comment
			#pragma endregion 超端
			case CTN_2_BTN_14:	//音量 -
				{
					irrklang::ik_f32 ivolume = m_SoundEnginBg->getSoundVolume();
					ivolume = (ivolume - 0.2) <=0 ? 0 :(ivolume - 0.2);
					m_SoundEnginBg->setSoundVolume(ivolume);
				}
				break;
			case CTN_2_BTN_13:	//音量+
				{
					irrklang::ik_f32 ivolume = m_SoundEnginBg->getSoundVolume();
					ivolume = (ivolume + 0.2) >=1 ? 1 :(ivolume + 0.2);
					m_SoundEnginBg->setSoundVolume(ivolume);
				}
				break;
			case CTN_2_BTN_15:	//音量关
				{
					m_SoundEnginBg->setSoundVolume(0.0);
				}
				break;
			case CTN_2_BTN_16:	//音量开
				{
					m_SoundEnginBg->setSoundVolume(0.4);
				}
				break;
			case CTN_2_BTN_11:	//关闭按钮
				{
					NetMessageHead tNetMessageHead;
					tNetMessageHead.bAssistantID = ASS_GM_FORCE_QUIT;
					m_pGameFrame->SendGameData(ASS_GM_FORCE_QUIT,&tNetMessageHead,sizeof(tNetMessageHead));
					m_pGameFrame->CloseClient();
				}
				break;
			}
		}
	//播放动画完成消息
	if(pMessage->_uMessageType == UI_CATOONFINISH)
	{
		switch(pMessage->_uControlID)
		{
		}
	}
	return 0;
}

//获得升起牌的数量
int CNewGameClient::GetUpNums(const int iControlID)
{
	BYTE bCardList[13];
	int iNums=0;
	memset(bCardList,0,sizeof(BYTE)*13);
	IOperateCard  *pOperateCard = NULL ; 
	pOperateCard = dynamic_cast<IOperateCard *>(m_pUI->GetIControlbyID(iControlID)); 
	if(NULL != pOperateCard)
	{
		pOperateCard->GetUpCardList(bCardList);
	}
	for(int i=0;i<13;i++)
	{
		if(m_Logic.TestData(bCardList[i],1))
		{
			iNums++;
		}
	}
	return iNums;
}

//游戏模式选择失败
void CNewGameClient::ExitWithoutSelectMode()
{
 	TGameStation tGameStation;
	m_pGameFrame->KillTimer(TIMER_SELECTGAMEMODE);	//关闭游戏模式选择时间定时器
 	SetTimePlay(CTN_1014089_CTN_3000459_Time_3000483,false,m_ClockTickCount);	//停止播放倒计时
	m_ClockTickCount = 15;
 
 	tGameStation.enGameStation = GS_NOMODE;//GAMESTATION_EXITSELECTMODE;
 	tGameStation.enGameMode = GAMEMODE_INVALIDE;
 	tGameStation.byGameDeskNO = m_pGameFrame->GetMyDeskStation();
 	m_pGameFrame->SendGameData(&tGameStation,sizeof(tGameStation),MDM_GM_GAME_NOTIFY,GM_GAMESTATIONCHANGED,0);
 
 	m_pGameFrame->CloseClient();						//退出游戏	
}

//播放背景音效
void	CNewGameClient::PlayBgSound(bool bISLoop)
{
	CString  folder ;
	CString strPath = CINIFile::GetAppPath() ;
	char szFileName[MAX_PATH];
	memset(szFileName,0,sizeof(szFileName)) ; 
	folder.Format("./resources/music/背景音乐.ogg");
	wsprintf(szFileName,"%s%s",strPath ,folder);
	m_SoundEngineGame->setSoundVolume(1.0);
	m_SoundEngineGame->play2D(szFileName,bISLoop);
}
/// 设置游戏的当前状态
void CNewGameClient::SetGameStation(void * pBuffer,int nLen)
{
	SetControlVisible(CTN_1014089_CTN_3000459,false);	//模式设置
	SetControlVisible(CTN_1014089_CTN_3000478,false);	//模式通知

	TGameStation *pGameStation = (TGameStation*)pBuffer;
	m_iMyDeskStation = m_pGameFrame->GetMyDeskStation();//自己的座位号
	m_byGameStation = pGameStation->enGameStation;
	ENUM_GameMode gameMode ;	//游戏模式
	TCHAR buffer[100] = {0};	//提示信息缓冲区
	

	switch(m_byGameStation)
	{
	case GS_NOMODE:	//没有玩家, 显示模式设置对话框
		{
			TGameStation tGameStation;
			tGameStation.byGameDeskNO = m_iMyDeskStation;
			tGameStation.enGameStation = GS_WITHMODE;
			m_pGameFrame->SendGameData(&tGameStation,sizeof(tGameStation),MDM_GM_GAME_NOTIFY,GM_GAMESTATIONCHANGED,0);		//改变游戏状态
			//SetControlVisible(CTN_1014089_CTN_3000459,true);				//显示模式设置
			//SetControlVisible(CTN_1014089_CTN_3000459_Time_3000483,true);	//显示定时器
			//SetTimePlay(CTN_1014089_CTN_3000459_Time_3000483,true,m_ClockTickCount);
			//
			//m_pGameFrame->SetTimer(TIMER_SELECTGAMEMODE,15*1000);
			//启动定时器
			m_ClockTickCount = 15;
			m_pGameFrame->SetTimer(TIME_BEGIN,1*1000);
			SetControlVisible(CTN_1014089_BTN_3000492,true);	//准备按钮可用
		}
		break;
	case GS_MODESETTING:	//模式设置 
		{
			//SetControlVisible(CTN_1014089_CTN_3000478,true);	//显示模式通知
			//SetControlVisible(CTN_1014089_CTN_3000478_Time_3000484,true);	//时间框
			//SetTimePlay(CTN_1014089_CTN_3000478_Time_3000484,true,m_ClockTickCount);
			//SetTextShowText(CTN_1014089_CTN_3000478_TEXT_3000481,__T("玩家正在设置游戏模式，请稍等!"),true);

			//m_pGameFrame->SetTimer(TIMER_SHOWGAMEMODEINFO,15*1000);
			//SetTimePlay(CTN_1014089_CTN_3000478_Time_3000484,true,m_ClockTickCount);
			//SetInumInfo(CTN_1014089_CTN_3000478_Time_3000484,m_ClockTickCount,true);
			SetControlVisible(CTN_1014089_BTN_3000492,true);	//准备按钮可用
		}
		break;

	case GS_WITHMODE:		//有一个玩家，等下下一个玩家准备即可开始
		{	
			//gameMode = pGameStation->enGameMode;	
			//SetControlVisible(CTN_1014089_CTN_3000478,true);	//显示模式通知
			/*	if (gameMode == GAMEMODE_INVALIDE)
			{
			SetTextShowText(CTN_1014089_CTN_3000478_TEXT_3000481,__T("玩家正在设置游戏模式，请稍等!"),true);
			}
			else if(gameMode == GAMEMODE_BANKER_BET)
			{
			SetTextShowText(CTN_1014089_CTN_3000478_TEXT_3000481,__T("本桌游戏模式为： 抢庄 !"),true);
			SetTextShowText(CTN_1014089_TEXT_3000477,__TEXT("抢庄"),true);
			}
			else if (gameMode == GAMEMODE_BANKER_CONST)
			{
			SetTextShowText(CTN_1014089_CTN_3000478_TEXT_3000481,__T("本桌游戏模式为： 定庄 !"),true);
			SetTextShowText(CTN_1014089_TEXT_3000477,__TEXT("定庄"),true);
			}
			else if (gameMode == GAMEMODE_BANKER_INTURN)
			{
			SetTextShowText(CTN_1014089_CTN_3000478_TEXT_3000481,__T("本桌游戏模式为： 轮庄 !"),true);
			SetTextShowText(CTN_1014089_TEXT_3000477,__TEXT("轮庄"),true);
			}*/
			//设置准备按钮可见
			SetControlVisible(CTN_1014089_BTN_3000492,true);	//准备按钮可用	
		}		
		break;
	case GS_BETTING:
	case GS_READY:			//准备4
		//{
		//	SetControlVisible(CTN_1014089_BTN_3000492,true);	//准备按钮可见
		//}
		//break;
	case GS_GAMEBEGIN:			//游戏中5
	case GS_WASHCARDS:			//洗牌中6
	case GS_SENDCARDS:			//发牌中7
	//case GS_BETTING:
	case GS_FINDBANKER:
	case GS_PK:			//比牌中
		{
			SetControlVisible(CTN_1014089_BTN_3000492,false);	//准备按钮不可见
		}
		break;
	case GS_SETTLE:				//结算8
	case GS_GAMEOVER:			//游戏结束9
		{
			SetControlVisible(CTN_1014089_BTN_3000492,true);	//准备按钮可用

			//gameMode = pGameStation->enGameMode;	
			//SetControlVisible(CTN_1014089_CTN_3000478,true);	//显示模式通知
			//if (gameMode == GAMEMODE_INVALIDE)
			//{
			//	SetTextShowText(CTN_1014089_CTN_3000478_TEXT_3000481,__T("玩家正在设置游戏模式，请稍等!"),true);
			//}
			//else if(gameMode == GAMEMODE_BANKER_BET)
			//{
			//	SetTextShowText(CTN_1014089_CTN_3000478_TEXT_3000481,__T("本桌游戏模式为： 抢庄 !"),true);
			//	SetTextShowText(CTN_1014089_TEXT_3000477,__TEXT("抢庄"),true);
			//}
			//else if (gameMode == GAMEMODE_BANKER_CONST)
			//{
			//	SetTextShowText(CTN_1014089_CTN_3000478_TEXT_3000481,__T("本桌游戏模式为： 定庄 !"),true);
			//	SetTextShowText(CTN_1014089_TEXT_3000477,__TEXT("定庄"),true);
			//}
			//else if (gameMode == GAMEMODE_BANKER_INTURN)
			//{
			//	SetTextShowText(CTN_1014089_CTN_3000478_TEXT_3000481,__T("本桌游戏模式为： 轮庄 !"),true);
			//	SetTextShowText(CTN_1014089_TEXT_3000477,__TEXT("轮庄"),true);
			//}
		}
		break;
	}
}

//-----------------------------------------------------------------------------------------
/// 游戏消息
int CNewGameClient::HandleGameMessage(WORD nMainID,WORD nSubId,BYTE * buffer,int nLen)
{
	switch(nSubId)
	{
	case ASS_GM_AGREE_GAME:
		{		
			if(nLen!=sizeof(MSG_GR_R_UserAgree))
			{
				return 0;
			}	
		
			MSG_GR_R_UserAgree * pUser=(MSG_GR_R_UserAgree *)buffer;

			if(pUser->bDeskStation == m_iMyDeskStation)
			{
				SetControlVisible(CTN_1014089_BTN_3000492,false);	//准备按钮可见
			}	
		}
		break;

	case ASS_GM_GAME_STATION:   ///<设置状态(平台调用的)  -- 当玩家进入游戏桌坐下时，平台向客户端发送的消息，让客户端设置游戏状态
		{
			SetGameStation(buffer,nLen);	
			break;  
		}
	case ASS_SUPER:
		{
			SetControlVisible(CTN_1014089_BTN_3000755,true);
		}
		break;
	case GS_NOMODE:
		{
			SetGameStation(buffer,nLen);
			break;
		}
	case GS_WITHMODE:
		{
			SetGameStation(buffer,nLen);
		}
		break;
	case GM_GAMEMODESELECTED:
		GameModeSelectedMessage(buffer);
		break;
	case GM_BETBANKER:
		{
			//显示抢庄画面
			SetControlVisible(CTN_1014089_CTN_3000727,true);
			//启动定时器，若玩家没有抢庄则自动发送不抢庄消息
			m_pGameFrame->SetTimer(TIMER_BETBANKER,5000);
		}
		break;
	//下注
	case GM_BET:
		{
			T_SC_WhoIsBanker *ptBanker = (T_SC_WhoIsBanker *)buffer;
			if(ptBanker == NULL)
				break;
			int iView = m_pGameFrame->DeskStation2View(ptBanker->byDeskStation);
			int ipicbase = CTN_1014089_IMG_3000732;
			SetControlVisible(ipicbase + iView,true);	//显示庄家图片

			if((int)ptBanker->byDeskStation == m_pGameFrame->GetMyDeskStation())
			{
				//m_pGameFrame->ClickContinue();
				break;
			}
			else
			{
				m_pGameFrame->SetTimer(TIMER_BET,15*1000);
				SetControlVisible(CTN_1014089_Time_3000719,true);
				SetTimePlay(CTN_1014089_Time_3000719,true,m_ClockTickCount);
				ShowBet();
			}
		}
		break;

	case ASS_HANDSUP:
		{
			BYTE *pbyDeskStation = (BYTE*)buffer;
			if(pbyDeskStation == nullptr)
			{
				break;
			}
			/*if(*pbyDeskStation == m_pGameFrame->GetMyDeskStation())
			{*/
			m_pGameFrame->ClickContinue();
			//}
		}
		break;
	//洗牌
	case GM_WASHCARD:
		{
			//1. 隐藏准备按钮
			SetControlVisible(CTN_1014089_BTN_3000492,false);
			//播放洗牌动画
		}
		break;

	//发牌
	case GM_SENDCARD:
		{
			SetControlVisible(CTN_1014089_BTN_3000492,false);
			T_SC_Cards *tCards = (T_SC_Cards*)buffer;
			if(tCards == NULL)
				break;
			else
			{
				SetControlVisible(CTN_1014089_OperateCard_3000489,true);
				ShowOperateCard(CTN_1014089_OperateCard_3000489,tCards->m_cards,5);	//显示有效牌
				
			}
			memset(m_byOwnCards,0,sizeof(m_byOwnCards));	//清理内存
			memcpy(m_byOwnCards,tCards->m_cards,5);			//保存玩家的牌

			for (int i = 0; i <PLAY_COUNT; ++i)
			{
				UserInfoForGame tUserinfo;
				if(m_pGameFrame->GetUserInfo(i,tUserinfo))
				{
					int iView = m_pGameFrame->DeskStation2View(tUserinfo.bDeskStation);
					BYTE byCardList[5] = {80};
					ShowNoMoveCard(CTN_1014089_NoMoveCard_3000487 +iView,byCardList,5);

				}
			}
			//分牌按钮可见，但不可用
			SetControlVisible(CTN_1014089_BTN_3000493,true);
			SetBtEnable(CTN_1014089_BTN_3000493,false);

			//启动定时器
			m_pGameFrame->SetTimer(TIMER_THINK,15*1000);
			SetControlVisible(CTN_1014089_Time_3000719,true);
			SetTimePlay(CTN_1014089_Time_3000719,true,m_ClockTickCount);
			//SetInumInfo(CTN_1014089_Time_3000719,m_ClockTickCount,true);
		}
		break;

	case GM_SHOWTOALL:	//亮牌
		{
			ShowCards(buffer);
		}
		break;
	//比牌
	case GM_RESULT:
		{
			ShowCardsResult(buffer);
		}
		break;
	//结算
	case GM_SETTLE:
		{
			SetControlVisible(CTN_1014089_CTN_3000522,true);
			m_pGameFrame->SetTimer(TIMER_SHOWSETTLE,3000);
			OnSettle(buffer);

			//隐藏庄家标识
			SetControlVisible(CTN_1014089_IMG_3000732,false);
			SetControlVisible(CTN_1014089_IMG_3000733,false);
			SetControlVisible(CTN_1014089_IMG_3000734,false);
			SetControlVisible(CTN_1014089_IMG_3000735,false);
		}
		break;
	case GM_RESET_STATION:
		{
			//GameOverReset();
			//m_pGameFrame->KillTimer(TIMER_BET);
			//m_pGameFrame->KillTimer(TIMER_THINK);
			//SetControlVisible(CTN_1014089_Time_3000719,false);	//隐藏定时器
			//SetControlVisible(CTN_1014089_CTN_3000509,false);

			//隐藏庄列表
			SetControlVisible(CTN_1014089_IMG_3000732,false);
			SetControlVisible(CTN_1014089_IMG_3000733,false);
			SetControlVisible(CTN_1014089_IMG_3000734,false);
			SetControlVisible(CTN_1014089_IMG_3000735,false);
			if(buffer == NULL)
				return 0;
			if(nLen != sizeof(TGameStation))
				return 0;

			TGameStation* gamestation = (TGameStation*)buffer;
			int iView = m_pGameFrame->DeskStation2View(gamestation->byGameDeskNO);
			SetControlVisible(CTN_1014089_IMG_3000732 + iView, true);
			SetControlVisible(CTN_1014089_BTN_3000492,true);
			/*SetImageLoadSrc(CTN_1014089_BTN_3000492,CString("./resources/image/准备.png"),true);
			SetControlVisible(CTN_1014089_BTN_3000492,true);
			SetBtEnable(CTN_1014089_BTN_3000492,true);

			SetGameStation(buffer,nLen);*/
		}
		break;
	default:
		break;
	}
	return 0;
}


void CNewGameClient::OnSettle(void *pData)
{
	T_SC_Settle *ptSettle = (T_SC_Settle*)pData;
	if(ptSettle == NULL)
		return;
	int ibaseControlID = 3000700;

	IText *pText = NULL;
	for (int i = 0; i<PLAY_COUNT;++i)
	{
		char buffer[50] = {0};
		int ibaseControlIDnew = ibaseControlID + 4*i;
		WCHAR wbuffer[50] = {0};
		if(ptSettle->iScore[i] != 0)
		{
			//庄、闲
			memset(wbuffer,0,sizeof(wbuffer));
			GetPlayerRole(ptSettle->bIsBanker[i],wbuffer);
			pText = dynamic_cast<IText*>(m_pUI->GetIControlbyID(ibaseControlIDnew));
			if (pText == NULL)
				return;
			pText->SetText(wbuffer);	
// 			CString cs;
// 			cs.Format(_T("hs %s"),wbuffer);
// 			OutputDebugString(cs);

			//昵称
			pText = dynamic_cast<IText*>(m_pUI->GetIControlbyID(ibaseControlIDnew+1));
			if (pText == NULL)
				return;
			pText->SetText(CA2W(ptSettle->cbNickName[i]));	

			//得分
			pText = dynamic_cast<IText*>(m_pUI->GetIControlbyID(ibaseControlIDnew +2));
			if (pText == NULL)
				return;
			sprintf(buffer,"%I64d",ptSettle->iScore[i]);
			pText->SetText(CA2W(buffer));

			//牌型
			memset(wbuffer,0,sizeof(wbuffer));
			pText = dynamic_cast<IText*>(m_pUI->GetIControlbyID(ibaseControlIDnew +3));
			if (pText == NULL)
				return;
			GetCardShapeStr(ptSettle->enCardShape[i],wbuffer);
			pText->SetText(wbuffer);
		}
	}


}

void CNewGameClient::GetPlayerRole(bool bBanker,WCHAR* wbuffer)
{
	if(bBanker)
		memcpy(wbuffer,L"庄家",sizeof(L"庄家"));
	else
		memcpy(wbuffer,L"闲家",sizeof(L"闲家"));
}

void CNewGameClient::GetCardShapeStr(EnCardShape enCardShape,WCHAR *buffer)
{
	switch(enCardShape)
	{
	case CS_NOTEN:
		memcpy(buffer,L"无凑",sizeof(L"无凑"));
		break;
	case CS_OT_ONE:
		memcpy(buffer,L"有凑一点",sizeof(L"有凑一点"));
		break;
	case CS_OT_TWO:
		memcpy(buffer,L"有凑二点",sizeof(L"有凑二点"));
		break;
	case CS_OT_THREE:
		memcpy(buffer,L"有凑三点",sizeof(L"有凑三点"));
		break;
	case CS_OT_FOUR:
		memcpy(buffer,L"有凑四点",sizeof(L"有凑四点"));
		break;
	case CS_OT_FIVE:
		memcpy(buffer,L"有凑五点",sizeof(L"有凑五点"));
		break;
	case CS_OT_SIX:
		memcpy(buffer,L"有凑六点",sizeof(L"有凑六点"));
		break;
	case CS_OT_SEVEN:
		memcpy(buffer,L"有凑七点",sizeof(L"有凑七点"));
		break;
	case CS_OT_EIGHT:
		memcpy(buffer,L"有凑八点",sizeof(L"有凑八点"));
		break;
	case CS_OT_NIGHT:
		memcpy(buffer,L"有凑九点",sizeof(L"有凑九点"));
		break;
	case CS_DOUBLETEN:
		memcpy(buffer,L"马股",sizeof(L"马股"));
		break;
	case CS_DTENANDDF:
		memcpy(buffer,L"铜锤",sizeof(L"铜锤"));
		break;
	case CS_SMALLHORSE:
		memcpy(buffer,L"小顺马",sizeof(L"小顺马"));
		break;
	case CS_BIGHORSE:
		memcpy(buffer,L"大顺马",sizeof(L"大顺马"));
		break;
	case CS_TENHORSE:
		memcpy(buffer,L"十马",sizeof(L"十马"));
		break;
	case CS_BOOM:
		memcpy(buffer,L"四条",sizeof(L"四条"));
		break;
	}
}

//显示出牌结果
void CNewGameClient::ShowCardsResult(void *pBuffer)
{
	T_SC_CardResult *ptResult = (T_SC_CardResult*)pBuffer;
	if(ptResult == NULL)
		return;

	int iControlID = 0;
	int iView = m_pGameFrame->DeskStation2View(ptResult->byDeskStation);
	switch(iView)
	{
	case 0:
		iControlID = CTN_1014089_IMG_3000518;
		break;
	case 1:
		iControlID = CTN_1014089_IMG_3000519;
		break;
	case 2:
		iControlID = CTN_1014089_IMG_3000508;
		break;
	case 3:
		iControlID = CTN_1014089_IMG_3000520;
		break;
	}

	IImage *pImage = NULL;
	pImage = dynamic_cast<IImage *> (m_pUI->GetIControlbyID(iControlID));
	if(pImage == NULL)
		return;

	SetControlVisible(iControlID,true);
	switch (ptResult->enCardShape)
	{
	case CS_NOTEN:	//无凑
		pImage->LoadPic(CA2W(CString("./resources/image/cardshape/wucou.png")));
		break;
	case CS_OT_ONE:	//有凑1点
		pImage->LoadPic(CA2W(CString("./resources/image/cardshape/1.png")));
		break;
	case CS_OT_TWO:	//有凑2点
		pImage->LoadPic(CA2W(CString("./resources/image/cardshape/2.png")));
		break;
	case CS_OT_THREE:	//有凑3点
		pImage->LoadPic(CA2W(CString("./resources/image/cardshape/3.png")));
		break;
	case CS_OT_FOUR:	//有凑4点
		pImage->LoadPic(CA2W(CString("./resources/image/cardshape/4.png")));
		break;
	case CS_OT_FIVE:	//有凑5点
		pImage->LoadPic(CA2W(CString("./resources/image/cardshape/5.png")));
		break;
	case CS_OT_SIX:		//有凑6点
		pImage->LoadPic(CA2W(CString("./resources/image/cardshape/6.png")));
		break;
	case CS_OT_SEVEN:	//有凑7点
		pImage->LoadPic(CA2W(CString("./resources/image/cardshape/7.png")));
		break;
	case CS_OT_EIGHT:	//有凑八点
		pImage->LoadPic(CA2W(CString("./resources/image/cardshape/8.png")));
		break;
	case CS_OT_NIGHT:	//有凑9点
		pImage->LoadPic(CA2W(CString("./resources/image/cardshape/9.png")));
		break;
	case CS_DOUBLETEN:	//马股
		pImage->LoadPic(CA2W(CString("./resources/image/cardshape/magu.png")));
		break;
	case CS_DTENANDDF:	//铜锤
		pImage->LoadPic(CA2W(CString("./resources/image/cardshape/tongchui.png")));
		break;
	case CS_SMALLHORSE:	//小顺马
		pImage->LoadPic(CA2W(CString("./resources/image/cardshape/xiaoshunma.png")));
		break;
	case CS_BIGHORSE:	//大顺马
		pImage->LoadPic(CA2W(CString("./resources/image/cardshape/dashunma.png")));
		break;
	case CS_TENHORSE:	//十马
		pImage->LoadPic(CA2W(CString("./resources/image/cardshape/shima.png")));
		break;
	case CS_BOOM:	//四条
		pImage->LoadPic(CA2W(CString("./resources/image/cardshape/sitiao.png")));
		break;
	case CS_INVALID:
		break;
	}
}

// 游戏模式选择成功
void CNewGameClient::GameModeSelectedMessage(BYTE *pData)
{
	
	TGameStation *pGameStation = (TGameStation*)pData;
	if (pGameStation == NULL)
	{
		OutputDebugString("hs Client recivce Empty Message");
		return ;
	}
	if (pGameStation == NULL)
		return;
// 	if(m_pGameFrame->GetIsWatching())
// 	{
// 
// 	}
	ENUM_GameMode gameMode = pGameStation->enGameMode;
		
	if (GetControlShow(CTN_1014089_CTN_3000478))
		SetControlVisible(CTN_1014089_CTN_3000478,false);
	SetControlVisible(CTN_1014089_CTN_3000478,true);	//显示模式通知
	SetControlVisible(CTN_1014089_CTN_3000478_Time_3000484,true);	//时间框
	SetTimePlay(CTN_1014089_CTN_3000478_Time_3000484,true,m_ClockTickCount);
	if (gameMode == GAMEMODE_INVALIDE)
	{
		SetTextShowText(CTN_1014089_CTN_3000478_TEXT_3000481,__T("玩家正在设置游戏模式，请稍等!"),true);
		SetTextShowText(CTN_1014089_TEXT_3000477,__T("未设置"),true);
	}
	else if(gameMode == GAMEMODE_BANKER_BET)
	{
		SetTextShowText(CTN_1014089_CTN_3000478_TEXT_3000481,__T("本桌游戏模式为： 抢庄!"),true);
		SetTextShowText(CTN_1014089_TEXT_3000477,__T("抢庄"),true);
	}
	else if (gameMode == GAMEMODE_BANKER_CONST)
	{
		SetTextShowText(CTN_1014089_CTN_3000478_TEXT_3000481,__T("本桌游戏模式为： 定庄!"),true);
		SetTextShowText(CTN_1014089_TEXT_3000477,__T("定庄"),true);
	}
	else if (gameMode == GAMEMODE_BANKER_INTURN)
	{
		SetTextShowText(CTN_1014089_CTN_3000478_TEXT_3000481,__T("本桌游戏模式为： 轮庄!"),true);
		SetTextShowText(CTN_1014089_TEXT_3000477,__T("轮庄"),true);
	}
	m_pGameFrame->SetTimer(TIMER_SHOWGAMEMODEINFO,15*1000);
	//SetInumInfo(CTN_1014089_CTN_3000478_Time_3000484,m_ClockTickCount,true);
	SetTimePlay(CTN_1014089_CTN_3000478_Time_3000484,true,m_ClockTickCount);


	
}

//重新设置状态
int	CNewGameClient::ResetGameFrame(void)
{

	return 0;
}
/// 玩家离开
int CNewGameClient::GameUserLeft(int nIndex)
{
	int iView = m_pGameFrame->DeskStation2View(nIndex);
	SetControlVisible(CTN_1014089_IMG_3000732 + iView, false);

	if(m_bUserPrepare[nIndex])
	{
		int viewStation=m_pGameFrame->DeskStation2View(nIndex);
		//显示准备图片
		//SetControlVisible(CTN_1014089_IMG_2000288+viewStation,false);
	}
	if(m_pGameFrame->GetIsWatching())
	{
		m_pGameFrame->CloseClient();
	}
	return 0;
}
/// 玩家断线
int CNewGameClient::GameUserOffLine(void)
{
	return 0;
}
// 是否正在游戏，由游戏实现
// 旁观玩家返回false，游戏玩家正在游戏中返回true，其它状态返回false
bool CNewGameClient::IsPlayingGame()
{	
	return false;
}

///玩家退出游戏
bool CNewGameClient::OnGameQuiting()
{
	if(m_pGameFrame->GetIsWatching() || !m_bUserPrepare[m_iMyDeskStation])
	{
		return true ; 
	}
	CString szAppPath = CINIFile::GetAppPath();
	szAppPath.SetAt(szAppPath.GetLength()-1, TCHAR('\0'));
	int nPosOfSlash = szAppPath.ReverseFind(TCHAR('\\'));
	CString szAppPathParent = szAppPath.Left(nPosOfSlash);

	CString strKeyName  ; 
	strKeyName.Format("%d" ,NAME_ID) ; 
	CBcfFile File(szAppPathParent + "\\SpecialRule.bcf") ; 
	m_pGameFrame->SendGameData(NULL , 0 ,MDM_GM_GAME_FRAME,ASS_GM_FORCE_QUIT,0);

	bool   bForceQuitAsAuto = File.GetKeyVal("ForceQuitAsAuto",strKeyName ,1) ; 
	
	//if(GS_READY<m_byGameStation && m_byGameStation<GS_SETTLE)
	if(m_byGameStation > GS_WITHMODE && m_byGameStation < GS_GAMEOVER)
	{
		if(IDOK != AFCMessageBox("您正在游戏中, 现在离开将会由笨笨的机器人代打哦？", "提示",AFC_YESNO)) 
		{
			return false ; 
		}
		m_pGameFrame->SendGameData(NULL , 0 ,MDM_GM_GAME_FRAME,ASS_GM_FORCE_QUIT,0);
	}
	return true;
}
/// 定时器消息
void CNewGameClient::OnTimer(int nTimerID)
{
	switch(nTimerID)
	{
	case TIMER_SELECTGAMEMODE:	//游戏模式选择定时器
		{
			m_pGameFrame->KillTimer(TIMER_SELECTGAMEMODE);
			TGameStation tGameStation;
			tGameStation.byGameDeskNO = m_pGameFrame->GetMyDeskStation();
			tGameStation.enGameMode = GAMEMODE_INVALIDE;
			tGameStation.enGameStation = GS_NOMODE;
			m_pGameFrame->SendGameData(&tGameStation,sizeof(tGameStation),MDM_GM_GAME_NOTIFY,GM_GAMESTATIONCHANGED,0);
			m_pGameFrame->SendGameData(NULL,0,MDM_GM_GAME_FRAME,ASS_GM_FORCE_QUIT,0);		//退出游戏
			m_pGameFrame->CloseClient();
		}
		break;
	case TIMER_SHOWGAMEMODEINFO:	// 游戏模式通知定时器
		{
			m_pGameFrame->KillTimer(TIMER_SHOWGAMEMODEINFO);
			SetControlVisible(CTN_1014089_CTN_3000478,false);
		}
		break;
	case TIMER_BET:		//若玩家没有点击下注，则自动下注
		{
			m_pGameFrame->KillTimer(TIMER_BET);
			//发送下注消息
			UserInfoForGame tUserinfo;
			m_pGameFrame->GetUserInfo(m_pGameFrame->GetMyDeskStation(),tUserinfo);
			__int64 iUserMoney = tUserinfo.i64Money;
			__int64 iBetMoney  = iUserMoney / 100;

			T_CS_Bet tBet;
			tBet.iBetMony = iBetMoney;
			tBet.byDeskStation = m_pGameFrame->GetMyDeskStation();
			m_pGameFrame->SendGameData(&tBet,sizeof(tBet),MDM_GM_GAME_NOTIFY,GM_BET,0);

			int iButtonIDs = CTN_1014089_CTN_3000509_BTN_3000510;
			for (int i=0;i<4;++i)
			{
				IButton *pIButton = dynamic_cast<IButton*>(m_pUI->GetIControlbyID(iButtonIDs+i));
				if(pIButton == NULL)
					return;
				SetControlVisible(iButtonIDs+i,true);
				WCHAR buffer[30] = {0};
				pIButton->SetText(buffer);
			}
			SetControlVisible(CTN_1014089_CTN_3000509,false);		//隐藏下注容器
			SetControlVisible(CTN_1014089_Time_3000719,false);		//隐藏计时器
			
			//m_pGameFrame->ClickContinue();	//举手
		}
		break;
	case TIMER_THINK:
		{
			m_pGameFrame->KillTimer(TIMER_THINK);
			SetControlVisible(CTN_1014089_Time_3000719,false);	// view or deskstation?
			//选几张牌，发送出去
			T_CS_OutCard tOutCard;
			tOutCard.byDeskStation = m_pGameFrame->GetMyDeskStation();
			memcpy(tOutCard.firstHeap,m_byOwnCards,sizeof(BYTE)*3);
			memcpy(tOutCard.SecondHeap,&m_byOwnCards[3],sizeof(BYTE)*2);
			memcpy(tOutCard.allCards,m_byOwnCards,sizeof(BYTE)*5);
			tOutCard.cardCount = 5;
			m_pGameFrame->SendGameData(&tOutCard,sizeof(tOutCard),MDM_GM_GAME_NOTIFY,GM_PK,0);

			SetControlVisible(CTN_1014089_BTN_3000493,false);
		}
		break;
	case TIMER_NOTIFYMODE:
		{
			m_pGameFrame->KillTimer(TIMER_NOTIFYMODE);
		
			SetControlVisible(CTN_1014089_CTN_3000478,false);	//设置通知容器不可见
			SetControlVisible(CTN_1014089_BTN_3000492,true);	//设置准备按钮可见
		}
		break;
	case TIMER_BETBANKER:	//抢庄定时器，当玩家没有抢庄时，自动发送不抢庄消息
		{
			m_pGameFrame->KillTimer(TIMER_BETBANKER);

			TBetBanker tBetBanker;
			tBetBanker.isBet = false;
			tBetBanker.byDeskStation = m_pGameFrame->GetMyDeskStation();
			m_pGameFrame->SendGameData(&tBetBanker,sizeof(tBetBanker),MDM_GM_GAME_NOTIFY,GM_BETBANKER,0);
		//	m_pGameFrame->ClickContinue();

			SetControlVisible(CTN_1014089_CTN_3000727,false);
		}
		break;
	case TIMER_SHOWSETTLE:
		{
			m_pGameFrame->KillTimer(TIMER_SHOWSETTLE);
			SetControlVisible(CTN_1014089_BTN_3000492,true);	//准备按钮可见
			m_ClockTickCount = 15;
			m_pGameFrame->SetTimer(TIME_BEGIN,m_ClockTickCount * 1000);
		}
		break;
	case TIME_BEGIN:
		{
			SetControlVisible(CTN_1014089_Time_3000719,true);
			SetTimePlay(CTN_1014089_Time_3000719,true,m_ClockTickCount);
			m_ClockTickCount--;
			if(m_ClockTickCount <= 0)
				m_pGameFrame->CloseClient();
		}
		break;
	default:
		break;
	}
}

//---------------------------------------------------------------------------------------
//播放游戏音效
void	CNewGameClient::PlayGameSound(int SoundID, bool bISLoop )
{
	if(!m_SoundEngineGame)
	{
		return ;
	}
	CString  folder ;
	CString strPath = CINIFile::GetAppPath() ;
	char szFileName[MAX_PATH];
	memset(szFileName,0,sizeof(szFileName)) ; 
	UserInfoForGame Info;
	m_pGameFrame->GetUserInfo(m_iMyDeskStation,Info);
	if(Info.bBoy)
	{
		folder.Format("./resources/music/boy/");
	}
	else
	{
		folder.Format("./resources/music/girl/");
	}
	switch(SoundID)
	{
	case SOUND_BUTTON://按钮
		{
			folder.Empty();
			folder.Format("./resources/music/button.wav");
			break;
		}
	default:
		{
			break;
		}
	}
	wsprintf(szFileName,"%s%s",strPath ,folder);
	m_SoundEngineGame->setSoundVolume(1.0);
	m_SoundEngineGame->play2D(szFileName,bISLoop);
}

void CNewGameClient::CloneImage(const int iID, int iGoalID)
{
	IImage *_pControl = NULL;
	_pControl=dynamic_cast<IImage *>(m_pUI->GetIControlbyID(iID));
	IImage *pControl = NULL;
	if (NULL != _pControl)
	{
		pControl = dynamic_cast<IImage *>(_pControl->Clone(iGoalID));//克隆
		if (NULL != pControl)
		{
			if (NULL == m_pUI->GetIControlbyID(pControl->GetGameControlID()))
			{
				m_pUI->IInsertControl(pControl);
			}
		}
	}
	return;
}

/*
@brief:播放动画
@param:ACTION_ID,动画控件ID，CONTROL_ID 被控制的控件,startXY 起点坐标,endXY 终点坐标
@return:void
*/
void CNewGameClient::PlayIMoveAction(const int ACTION_ID,const int CONTROL_ID,POINT startXY,POINT endXY,int second)
{
	//获得动画控件ID
	IMoveAction* pMoveAction = NULL;
	GETCTRL(IMoveAction,pMoveAction,m_pUI,ACTION_ID);
	//获得被控制的ID
	INoMoveCard *pNoMoveCard = NULL;
	pNoMoveCard = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(CONTROL_ID));
	if(NULL!=pMoveAction && NULL!=pNoMoveCard)
	{
		pMoveAction->SetControlingCtrlID(CONTROL_ID);
		pMoveAction->SetStartP(startXY);
		pMoveAction->SetEndP(endXY);
		pMoveAction->SetTotalMoveTime(second);
		pMoveAction->SetControlVisible(true);
		pMoveAction->SetPlayState(true);
	}
}

// 选择游戏模式 确定按钮
void CNewGameClient::OnBtnSelectGameModeOK()
{
	bool RBselect;
	TGameStation tGameStation;	//模式，状态，座位号
	tGameStation.byGameDeskNO = m_pGameFrame->GetMyDeskStation();//座位号
	tGameStation.enGameStation = GS_WITHMODE;//GAMESTATION_GAMEMODESELECTED;

	if ((GetRButtonSelected(CTN_1014089_CTN_3000459_RButton_3000465,&RBselect) == FALSE) &&
		(GetRButtonSelected(CTN_1014089_CTN_3000459_RButton_3000466,&RBselect) == FALSE) &&
		(GetRButtonSelected(CTN_1014089_CTN_3000459_RButton_3000467,&RBselect) == FALSE)
		)		//当未选择任何游戏模式时，点击确定按钮无效
		return;

	m_pGameFrame->KillTimer(TIMER_SELECTGAMEMODE);	//关闭游戏模式选择时间定时器
	SetTimePlay(CTN_1014089_CTN_3000459_Time_3000483,false,m_ClockTickCount); //停止播放倒计时
	m_ClockTickCount = 15;

	//TODO: 通知服务器游戏模式已设定
	//if(GetRButtonSelected(CTN_1014089_CTN_3000459_RButton_3000465,&RBselect))		//选择轮庄模式
	//{
	//	tGameStation.enGameMode = GAMEMODE_BANKER_INTURN;
	//}
	//else if (GetRButtonSelected(CTN_1014089_CTN_3000459_RButton_3000466,&RBselect))	//选择定庄模式
	//{
	//	tGameStation.enGameMode = GAMEMODE_BANKER_CONST;
	//}
	//else if (GetRButtonSelected(CTN_1014089_CTN_3000459_RButton_3000467,&RBselect))	//选择抢庄模式
	//{
	//	tGameStation.enGameMode = GAMEMODE_BANKER_BET;
	//}
	m_pGameFrame->SendGameData(&tGameStation,sizeof(tGameStation),MDM_GM_GAME_NOTIFY,GM_GAMESTATIONCHANGED,0);	//向服务器通知游戏模式设置成功，并将服务器的游戏状态设置为 GAMESTATION_WATINGBEGIN。
	SetControlVisible(CTN_1014089_CTN_3000459,false);
	m_pGameFrame->SetTimer(TIMER_NOTIFYMODE,15*1000);
	SetTimePlay(CTN_1014089_CTN_3000478_Time_3000484,true,m_ClockTickCount); 
	
	//m_pGameFrame->ClickContinue();
	SetControlVisible(CTN_1014089_BTN_3000492,false);	//准备按钮可见

}
//接受发牌消息服务函数
void CNewGameClient::OnReveiveSendCard(void *pBuffer)
{
// 	T_SC_SendCard *pSendCard=(T_SC_SendCardd *)pBuffer;
// 	if(NULL==pSendCard)
// 	{
// 		return;
// 	}
// 	int iView=m_pGameFrame->DeskStation2View(0);
}


void CNewGameClient::SendCardToServer()
{
	//获取第一堆牌
	GetUpCard(CTN_1014089_OperateCard_3000489,m_byFirstHeap);
	int counter = 0;
	for (int i = 0 ;i<3;++i)
	{
		if(m_byFirstHeap[i] != 0)
			++counter;
	}

	//获取第二堆牌
	vector<BYTE> car2;
	car2.clear();
	for (int i =0 ;i<5;++i)
	{
		int Count = 0;
		for (int j = 0;j<3;++j)
		{	
			if (m_byOwnCards[i] != m_byFirstHeap[j])
			{
				++Count;
				if(Count == 3)
					car2.push_back(m_byOwnCards[i]);	
			}
		}
	}
	if(car2.size() != 2)
	{
		char buffer[100] = {0};
		sprintf(buffer,"hs SendCard erorr, sizeof secondheap: %d",car2.size());
		OutputDebugString(buffer);
		return;
	}

	for (int i = 0;i< car2.size();++i)
		m_bySecondHeap[i] = car2[i];

	//将牌数据发送给服务器
	T_CS_OutCard tOutCard;
	tOutCard.cardCount = counter +2;
	tOutCard.byDeskStation = m_pGameFrame->GetMyDeskStation();
	memcpy(tOutCard.firstHeap,m_byFirstHeap,sizeof(BYTE)*3);		//拷贝第一堆牌
	memcpy(tOutCard.SecondHeap,m_bySecondHeap,sizeof(BYTE)*2);		//拷贝第二堆牌
	memcpy(tOutCard.allCards,m_byOwnCards,sizeof(BYTE)*5);			//拷贝所有牌

	m_pGameFrame->SendGameData(&tOutCard,sizeof(tOutCard),MDM_GM_GAME_NOTIFY,GM_PK,0);
}

void CNewGameClient::ShowBet()
{
	UserInfoForGame tUserinfo;
	m_pGameFrame->GetUserInfo(m_pGameFrame->GetMyDeskStation(),tUserinfo);

	__int64 iUserMoney = tUserinfo.i64Money;
	
	m_iBetMoney[0] = iUserMoney / 1000;
	m_iBetMoney[1] = iUserMoney / 500;
	m_iBetMoney[2] = iUserMoney / 200;
	m_iBetMoney[3] = iUserMoney / 100;

	SetControlVisible(CTN_1014089_CTN_3000509,true);

	IImage *pImage = dynamic_cast<IImage*>(m_pUI->GetIControlbyID(CTN_1014089_CTN_3000509_IMG_3000517));
	if(pImage == NULL)
	{
		OutputDebugString("hs can't get the image contorl");
		return ;
	}
	pImage->LoadPic(CA2W(CString("./resources/image/bettext.png")));
	
	int iButtonID = CTN_1014089_CTN_3000509_BTN_3000510;
	for (int i = 0;i<4;++i)
	{
		IButton *pIButton = dynamic_cast<IButton*>(m_pUI->GetIControlbyID(iButtonID+i));
		if(pIButton == NULL)
			return;

		SetControlVisible(iButtonID+i,true);
		WCHAR buffer[30] = {0};
		swprintf_s(buffer,sizeof(buffer),L"%I64d",m_iBetMoney[i]);	//显示下注金额

		pIButton->SetText(buffer);
		
	}
}

void CNewGameClient::Betting(int iButtonID)
{
	__int64 iBetMoney = 0;

	switch(iButtonID)
	{
	case CTN_1014089_CTN_3000509_BTN_3000510:
		iBetMoney = m_iBetMoney[0];
		break;
	case CTN_1014089_CTN_3000509_BTN_3000511:
		iBetMoney = m_iBetMoney[1];
		break;
	case CTN_1014089_CTN_3000509_BTN_3000512:
		iBetMoney = m_iBetMoney[2];
		break;
	case CTN_1014089_CTN_3000509_BTN_3000513:
		iBetMoney = m_iBetMoney[3];
		break;
	}
	if(iBetMoney == 0)	//投注失败
	{
		OutputDebugString("hs 投注失败");
		return ;
	}

	T_CS_Bet tBet;
	tBet.iBetMony = iBetMoney;
	tBet.byDeskStation = m_pGameFrame->GetMyDeskStation();

	m_pGameFrame->SendGameData(&tBet,sizeof(tBet),MDM_GM_GAME_NOTIFY,GM_BET,0);
	
	//m_pGameFrame->ClickContinue();	//举手

	SetControlVisible(CTN_1014089_CTN_3000509,true);	//设置下注界面可见

	int iButtonIDs = CTN_1014089_CTN_3000509_BTN_3000510;
	for (int i=0;i<4;++i)
	{
		IButton *pIButton = dynamic_cast<IButton*>(m_pUI->GetIControlbyID(iButtonIDs+i));
		if(pIButton == NULL)
			return;
		SetControlVisible(iButtonIDs+i,true);
		WCHAR buffer[30] = {0};
		pIButton->SetText(buffer);
	}
}

void CNewGameClient::ShowCards(BYTE* pData)
{
	T_SC_OutCard *ptCards = (T_SC_OutCard*)pData;
	if(ptCards == NULL)
		return;
	int iView = m_pGameFrame->DeskStation2View(ptCards->byDeskStation);


	switch (iView)
	{
	case 0:
		{
			SetControlVisible(CTN_1014089_NoMoveCard_3000487,false);
			//显示不可操作牌
			ShowNoMoveCard(CTN_1014089_NoMoveCard_3000499,ptCards->firstHeap,3);
			ShowNoMoveCard(CTN_1014089_NoMoveCard_3000504,ptCards->secondHeap,2);
			//显示牌型字体
			ShowCardShapeInfo(ptCards->enCardShape,CTN_1014089_IMG_3000518);
		}
		break;
	case 1:
		{
			SetControlVisible(CTN_1014089_NoMoveCard_3000488,false);
			//显示不可操作牌
			ShowNoMoveCard(CTN_1014089_NoMoveCard_3000505,ptCards->firstHeap,3);
			ShowNoMoveCard(CTN_1014089_NoMoveCard_3000501,ptCards->secondHeap,2);

			//显示牌型字体
			ShowCardShapeInfo(ptCards->enCardShape,CTN_1014089_IMG_3000519);
		}
		break;
	case 2:
		{
			SetControlVisible(CTN_1014089_OperateCard_3000489,false);
			//显示不可操作牌
			ShowNoMoveCard(CTN_1014089_NoMoveCard_3000502,ptCards->firstHeap,3);
			ShowNoMoveCard(CTN_1014089_NoMoveCard_3000506,ptCards->secondHeap,2);

			//显示牌型字体
			ShowCardShapeInfo(ptCards->enCardShape,CTN_1014089_IMG_3000508);
		}
		break;
	case 3:
		{
			SetControlVisible(CTN_1014089_NoMoveCard_3000490,false);
			//显示不可操作牌
			ShowNoMoveCard(CTN_1014089_NoMoveCard_3000503,ptCards->firstHeap,3);
			ShowNoMoveCard(CTN_1014089_NoMoveCard_3000507,ptCards->secondHeap,2);

			//显示牌型字体
			ShowCardShapeInfo(ptCards->enCardShape,CTN_1014089_IMG_3000520);
		}
		break;
	}
}

void CNewGameClient::ShowCardShapeInfo(EnCardShape enCardShape,int ControlID)
{
	CString appPath = CINIFile::GetAppPath();
	CString sndPaht = CString("/resources/sounds/");
	char szFilePath[MAX_PATH] = {0};
	m_SoundEngineGame->setSoundVolume(0.5);

	IImage *pImage = NULL;
	SetControlVisible(ControlID,true);
	switch (enCardShape)
	{
	case CS_NOTEN: 	//无凑
		{
			pImage= dynamic_cast<IImage*>(m_pUI->GetIControlbyID(ControlID));
			if(pImage == NULL)
				return;
			pImage->LoadPic(CA2W(CString("./resources/image/cardshape/wucou.png")));

			CString strone = CString("noc.wav");
			wsprintf(szFilePath,"%s%s%s",appPath,sndPaht,strone);
			m_SoundEngineGame->play2D(szFilePath,false);
		}
		break;
	case CS_OT_ONE:			//有凑一点
		{
			pImage= dynamic_cast<IImage*>(m_pUI->GetIControlbyID(ControlID));
			if(pImage == NULL)
				return;
			pImage->LoadPic(CA2W(CString("./resources/image/cardshape/1.png")));

			CString strone = CString("yc1-ptnv1.wav");
			wsprintf(szFilePath,"%s%s%s",appPath,sndPaht,strone);
			m_SoundEngineGame->play2D(szFilePath,false);
			
		}
		break;
	case CS_OT_TWO:			//有凑二点
		{
			pImage= dynamic_cast<IImage*>(m_pUI->GetIControlbyID(ControlID));
			if(pImage == NULL)
				return;
			pImage->LoadPic(CA2W(CString("./resources/image/cardshape/2.png")));

			CString strone = CString("yc2-ptnv1.wav");
			wsprintf(szFilePath,"%s%s%s",appPath,sndPaht,strone);
			m_SoundEngineGame->play2D(szFilePath,false);
			
		}
		break;
	case CS_OT_THREE:		//有凑三点
		{
			pImage= dynamic_cast<IImage*>(m_pUI->GetIControlbyID(ControlID));
			if(pImage == NULL)
				return;
			pImage->LoadPic(CA2W(CString("./resources/image/cardshape/3.png")));

			CString strone = CString("yc3-ptnv1.wav");
			wsprintf(szFilePath,"%s%s%s",appPath,sndPaht,strone);
			m_SoundEngineGame->play2D(szFilePath,false);
		
		}
		break;
	case CS_OT_FOUR:			//有凑四点
		{
			pImage= dynamic_cast<IImage*>(m_pUI->GetIControlbyID(ControlID));
			if(pImage == NULL)
				return;
			pImage->LoadPic(CA2W(CString("./resources/image/cardshape/4.png")));

			CString strone = CString("yc4-ptnv1.wav");
			wsprintf(szFilePath,"%s%s%s",appPath,sndPaht,strone);
			m_SoundEngineGame->play2D(szFilePath,false);
		
		}
		break;
	case CS_OT_FIVE:			//有凑五点
		{
			pImage= dynamic_cast<IImage*>(m_pUI->GetIControlbyID(ControlID));
			if(pImage == NULL)
				return;
			pImage->LoadPic(CA2W(CString("./resources/image/cardshape/5.png")));

			CString strone = CString("yc5-ptnv1.wav");
			wsprintf(szFilePath,"%s%s%s",appPath,sndPaht,strone);
			m_SoundEngineGame->play2D(szFilePath,false);
			
		}
		break;
	case CS_OT_SIX:			//有凑六点
		{
			pImage= dynamic_cast<IImage*>(m_pUI->GetIControlbyID(ControlID));
			if(pImage == NULL)
				return;
			pImage->LoadPic(CA2W(CString("./resources/image/cardshape/6.png")));

			CString strone = CString("yc6-ptnv1.wav");
			wsprintf(szFilePath,"%s%s%s",appPath,sndPaht,strone);
			m_SoundEngineGame->play2D(szFilePath,false);
			
		}
		break;
	case CS_OT_SEVEN:		//有凑七点
		{
			pImage= dynamic_cast<IImage*>(m_pUI->GetIControlbyID(ControlID));
			if(pImage == NULL)
				return;
			pImage->LoadPic(CA2W(CString("./resources/image/cardshape/7.png")));

			CString strone = CString("yc7-ptnv1.wav");
			wsprintf(szFilePath,"%s%s%s",appPath,sndPaht,strone);
			m_SoundEngineGame->play2D(szFilePath,false);
			
		}
		break;
	case CS_OT_EIGHT:		//有凑八点
		{
			pImage= dynamic_cast<IImage*>(m_pUI->GetIControlbyID(ControlID));
			if(pImage == NULL)
				return;
			pImage->LoadPic(CA2W(CString("./resources/image/cardshape/8.png")));
			
			CString strone = CString("yc8-ptnv1.wav");
			wsprintf(szFilePath,"%s%s%s",appPath,sndPaht,strone);
			m_SoundEngineGame->play2D(szFilePath,false);
		
		}
		break;
	case CS_OT_NIGHT:		//有凑九点
		{
			pImage= dynamic_cast<IImage*>(m_pUI->GetIControlbyID(ControlID));
			if(pImage == NULL)
				return;
			pImage->LoadPic(CA2W(CString("./resources/image/cardshape/9.png")));

			CString strone = CString("yc9-ptnv1.wav");
			wsprintf(szFilePath,"%s%s%s",appPath,sndPaht,strone);
			m_SoundEngineGame->play2D(szFilePath,false);
		
		}
		break;
	case CS_DOUBLETEN:		//马股
		{
			pImage= dynamic_cast<IImage*>(m_pUI->GetIControlbyID(ControlID));
			if(pImage == NULL)
				return;
			pImage->LoadPic(CA2W(CString("./resources/image/cardshape/magu.png")));

			CString strone = CString("magu-ptnv1.wav");
			wsprintf(szFilePath,"%s%s%s",appPath,sndPaht,strone);
			m_SoundEngineGame->play2D(szFilePath,false);
		
		}
		break;
	case CS_DTENANDDF:		//铜锤
		{
			pImage= dynamic_cast<IImage*>(m_pUI->GetIControlbyID(ControlID));
			if(pImage == NULL)
				return;
			pImage->LoadPic(CA2W(CString("./resources/image/cardshape/tongchui.png")));

			CString strone = CString("tongc-ptnv1.wav");
			wsprintf(szFilePath,"%s%s%s",appPath,sndPaht,strone);
			m_SoundEngineGame->play2D(szFilePath,false);
		
		}
		break;
	case CS_SMALLHORSE:		//小顺马
		{
			pImage= dynamic_cast<IImage*>(m_pUI->GetIControlbyID(ControlID));
			if(pImage == NULL)
				return;
			pImage->LoadPic(CA2W(CString("./resources/image/cardshape/xiaoshunma.png")));

			CString strone = CString("xiaosm-ptnv1.wav");
			wsprintf(szFilePath,"%s%s%s",appPath,sndPaht,strone);
			m_SoundEngineGame->play2D(szFilePath,false);
		
		}
		break;
	case CS_BIGHORSE:		//大顺马
		{
			pImage= dynamic_cast<IImage*>(m_pUI->GetIControlbyID(ControlID));
			if(pImage == NULL)
				return;
			pImage->LoadPic(CA2W(CString("./resources/image/cardshape/dashunma.png")));

			CString strone = CString("dasm-ptnv1.wav");
			wsprintf(szFilePath,"%s%s%s",appPath,sndPaht,strone);
			m_SoundEngineGame->play2D(szFilePath,false);
		
		}
		break;
	case CS_TENHORSE:			//十马
		{
			pImage= dynamic_cast<IImage*>(m_pUI->GetIControlbyID(ControlID));
			if(pImage == NULL)
				return;
			pImage->LoadPic(CA2W(CString("./resources/image/cardshape/shima.png")));

			CString strone = CString("shim-ptnv1.wav");
			wsprintf(szFilePath,"%s%s%s",appPath,sndPaht,strone);
			m_SoundEngineGame->play2D(szFilePath,false);
			
		}
		break;
	case CS_BOOM:			//四条
		{
			pImage= dynamic_cast<IImage*>(m_pUI->GetIControlbyID(ControlID));
			if(pImage == NULL)
				return;
			pImage->LoadPic(CA2W(CString("./resources/image/cardshape/sitiao.png")));

			CString strone = CString("sit-ptnv1.wav");
			wsprintf(szFilePath,"%s%s%s",appPath,sndPaht,strone);
			m_SoundEngineGame->play2D(szFilePath,false);
		
		}
		break;
	case CS_INVALID:			//无效
		OutputDebugString("hs 【show cardshape inform】CS_INVALID");
		break;
	default:
		OutputDebugString("hs 【show cardshape inform】default:");
		break;
	}
}

//设置输赢
void CNewGameClient::SetWinOrLose(int iControlID,BOOL IsWin)
{
	bool bSelect;

	GetRButtonSelected(iControlID,&bSelect);
	if(bSelect)
	{
		m_isWinOrLose = IsWin;
	}
	else
	{
		m_isWinOrLose = (!IsWin);
	}
}