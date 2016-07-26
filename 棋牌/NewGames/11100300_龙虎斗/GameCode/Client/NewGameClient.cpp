#include "stdafx.h"
#include "NewGameClient.h"
#include "../common/IGameFrame.h"
#include "UI_ForLogic/IUserInterface.h"
#include "UI_ForLogic/GameIDDefines.h"
#include "GameControlsIDDefine.h"
#include "Common/UIMessageDefines.h"
#include "LoveSendClassInExe.h"

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
	return 0;
}
//-----------------------------------------------------------------------------------
/// 初始化UI
int CNewGameClient::InitUI()
{
	
	//下注按钮不可用
	SetBetBtEnable(false);
	//显示空闲时间
	SetImageVisible(BTN_FREE_TIME,false);

	m_dbyLuzi.clear();
	//下庄按钮不可用
	SetBtEnable(BTN_APPLY_XIAZHUANG,false);
	//上庄按钮可用
	SetBtEnable(BTN_APPLY_MING,true);
	//上庄列表上下拉动按钮
	SetBtEnable(BTN_BUTTON_LEFT,true);
	SetBtEnable(BTN_BUTTON_RIGHT,true);
	ResetGameData();
	
	return 0;
}
//---------------------------------------------------------------------------------------
//重置UI界面 
void	CNewGameClient::ReSetUiDlag()
{
	//下注按钮不可用
	SetBetBtEnable(false);
	
}
/// 玩家进入
int CNewGameClient::GameUserCome(void)
{
	m_iMyDeskStation = m_pGameFrame->GetMyDeskStation(); 
	ShowMyInfo(true);
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
//--------------------------------------------------------------------------------------------------------
//设置按钮是否可见
void	CNewGameClient::SetBtVisible(const int iButtonID, bool bFlag)
{
	IButton *pButton = NULL;
	pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(iButtonID));
	if (NULL != pButton)
	{
		pButton->SetControlVisible(bFlag);
	}
}


//-------------------------------------------------------------------------------
/// 置状态
void CNewGameClient::SetStationParameter(BYTE bGameStation)
{
	m_byGameStation = bGameStation ; 
}
//-------------------------------------------------------------------------------
///获取游戏状态
BYTE CNewGameClient::GetStationParameter()
{
	return m_byGameStation;
}
//---------------------------------------------------------------------------------------
///初始化游戏中的数据
void	CNewGameClient::InitialGameData()
{
	
	m_iMyDeskStation = 255;		///玩家自己的游戏位子
	m_byNtStation	 = 255;		//庄家位置
	m_i64NtHaveWin	 = 0;		//庄家输赢情况
	m_iNtPlayCount	 = 0;		//庄家坐庄次数
	m_i64MeHaveWin	 = 0;		//我的输赢情况
	m_bIsSuperUser	 = false;	//是否超级用户
	
	m_bIsReady		 = false;	//是否准备了
	m_bIsPlaying	 = false;	//游戏是否已经开始
	m_byGameStation	 = GS_WAIT_ARGEE;         ///当前的游戏状态
	
	m_iPlayBaShu=0;//自己的把数
	m_iPlayJuShu=0;//自己的局数
	m_iPlayGrade=0;//自己的成绩
	m_byCardSort=0;
	m_byNtWaitListCounter=0;//上庄列表标签
	SetImageVisible(CTN_1000000_IMG_10001330,false);
	SetImageVisible(CTN_1000000_IMG_10001331,false);
	m_bySelChouMaType = 255;		//当前选择的筹码类型
	
	memset(m_byLuziList,-1,sizeof(m_byLuziList));			//路子
	memset(m_i64AreaAllSum,0x00,sizeof(m_i64AreaAllSum));					//清除区域总值
	memset(m_i64RealityAreaAllSum,0x00,sizeof(m_i64RealityAreaAllSum));		//清除真人下注总值

	//设置容器响应鼠标
	//SetContainEnable(BTN_APPLY_MING,true);
	for (int i=0; i<PLAY_COUNT; i++)
	{
		m_byWaitNtList[i] = 255;	//上庄列表
	}
	m_bSoundIsClose=TRUE;										//是否关闭声音
	m_bUserExitIsClose=FALSE;										//是否关闭用户进出提示信息
	
}

//------------------------------------------------------------------------------
/// 界面消息
int CNewGameClient::OnUIMessage(TUIMessage* pMessage)
{
	///按钮弹起的消息 
	if(pMessage->_uMessageType == UI_LBUTTONUP)
	{
		switch(pMessage->_uControlID)
		{
		case  BTN_MONEY_YIBAI:	//100筹码
		case  BTN_MONEY_YIQIAN:	//1000筹码
		case  BTN_MONEY_YIWAN:	//10000筹码
		case  BTN_MONEY_SHIWAN:	//100000筹码
		case  BTN_XIAZHU_YIBAIWAN:	//1000000筹码
		case  BTN_MONEY_YIQIANWAN:	//10000000筹码
			{
				if (m_byGameStation != GS_BET_STATUS)
				{
					return 0;
				}
				
				//当前选择的筹码类型值
				m_bySelChouMaType = pMessage->_uControlID - BTN_MONEY_YIBAI;
				if (m_bySelChouMaType >= 0 && m_bySelChouMaType<=5 )
				{
					CString sImageSrc;
					if(4==m_bySelChouMaType)
					{
						sImageSrc.Format(".\\resources\\image\\chouma_%d.png",m_bySelChouMaType);
					}
					else
					{
						sImageSrc.Format(".\\resources\\image\\chouma_%d.bmp",m_bySelChouMaType);
					}
					m_pUI->GetICursor()->SetCursorImage(CA2W(sImageSrc));
					m_pUI->GetICursor()->SetSrcW(m_pUI->GetICursor()->GetCursorImageWidth()/4);
					m_pUI->GetICursor()->SetPointMode(1);
					m_pUI->GetICursor()->SetControlVisible(true);
				}
				break;
			}
			//筹码控件ID
		case   BTN_10001321://下注龙
			{
				if (m_byGameStation != GS_BET_STATUS)
				{
					return 0;
				}
				if(255==m_bySelChouMaType)
				{
					return 0;
				}
				if(m_iMyDeskStation==m_byNtStation)
				{
					return 0;
				}
				OutputDebugString("liuwen-下注龙");
				UserBetMsg betMsg;
				betMsg.byDeskStation=m_iMyDeskStation;
				betMsg.byBetArea=0;
				betMsg.byChouMaType=m_bySelChouMaType;
				m_pGameFrame->SendGameData(&betMsg, sizeof(betMsg) ,MDM_GM_GAME_NOTIFY,ASS_USER_BET,0);
				break;
			}
		case   BTN_10001322://下注虎
			{
				if (m_byGameStation != GS_BET_STATUS)
				{
					return 0;
				}
				if(255==m_bySelChouMaType)
				{
					return 0;
				}
				if(m_iMyDeskStation==m_byNtStation)
				{
					return 0;
				}
				
				UserBetMsg betMsg;
				betMsg.byDeskStation=m_iMyDeskStation;
				betMsg.byBetArea=1;
				betMsg.byChouMaType=m_bySelChouMaType;
				m_pGameFrame->SendGameData(&betMsg, sizeof(betMsg) ,MDM_GM_GAME_NOTIFY,ASS_USER_BET,0);
				break;
			}
		case   BTN_10001325://下注和
			{
				if (m_byGameStation != GS_BET_STATUS)
				{
					return 0;
				}
				if(255==m_bySelChouMaType)
				{
					return 0;
				}
				if(m_iMyDeskStation==m_byNtStation)
				{
					return 0;
				}
				
				UserBetMsg betMsg;
				betMsg.byDeskStation=m_iMyDeskStation;
				betMsg.byBetArea=2;
				betMsg.byChouMaType=m_bySelChouMaType;
				m_pGameFrame->SendGameData(&betMsg, sizeof(betMsg) ,MDM_GM_GAME_NOTIFY,ASS_USER_BET,0);
				break;
			}
		case BTN_APPLY_MING:	//申请上庄
			{
				
				//申请上庄
				UserApplyNtStation();
				break;
			}
		case BTN_APPLY_XIAZHUANG:	//申请下庄
			{
				//申请下庄
				UserApplyLiveNt();
				break;
			}
		case BTN_BUTTON_RIGHT://右拉
		case BTN_BUTTON_LEFT://左拉
			{
				BYTE NtListBt=-1;
				NtListBt = pMessage->_uControlID - BTN_BUTTON_LEFT;
				NtListMove(NtListBt);//上庄列表移动
				break;
			}
		case BTN_BANK:
			{
				m_pGameFrame->OnVisitBank(); 
				break;
			}
		case CTN_2_BTN_13://音量加减
		case CTN_2_BTN_14:
			{
				BYTE bButton=0;
				bButton = pMessage->_uControlID - CTN_2_BTN_13;
				GameSet(bButton);
				break;
			}
		case CTN_2_BTN_1003500://游戏设置
			{
				SetContainVisible(CTN_1000000_CTN_10001420,TRUE);
				for(int i=0;i<2;i++)
				{
					if(0==i)
					{
						SetRButtonSelected(CTN_1000000_CTN_10001420_RButton_10001421+i,m_bSoundIsClose);
					}
					else if(1==i)
					{
						SetRButtonSelected(CTN_1000000_CTN_10001420_RButton_10001421+i,m_bUserExitIsClose);
					}
				}
				break;
			}
		case CTN_1000000_CTN_10001420_BTN_10001423:
		case CTN_1000000_CTN_10001420_BTN_10001424://游戏设置确定/取消
			{
				BYTE bButton=0;
				bButton = pMessage->_uControlID - CTN_1000000_CTN_10001420_BTN_10001423;
				GameSet(bButton);
				break;
			}
		case CTN_1000000_CTN_10001411_BTN_10001418://超端用户确定
			{
				BYTE byTempSuperResult=255;
				for(int i=0;i<3;i++)
				{
					bool bSelected=FALSE;
					bSelected=GetRButtonSelected(CTN_1000000_CTN_10001411_RButton_10001415+i,&bSelected);
					if(bSelected)
					{
						byTempSuperResult=i;
						break;
					}
				}
				if((byTempSuperResult>=0) && (byTempSuperResult<=2))
				{
					SuperUserControlReslut SuperResult;
					SuperResult.byDeskStation=m_iMyDeskStation;
					SuperResult.bySuperReslut=byTempSuperResult;
					//发送申请上庄消息
					m_pGameFrame->SendGameData(&SuperResult, sizeof(SuperResult) ,MDM_GM_GAME_NOTIFY,ASS_SUPER_RESULT,0);
				}
				ClearShowSuperResult();
				break;
			}
		case CTN_1000000_CTN_10001411_BTN_10001419://超端用户取消
			{
				ClearShowSuperResult();
				break;
			}
		case CTN_1000000_CTN_10001411_BTN_10001426:
		case CTN_1000000_CTN_10001411_BTN_10001427:
			{
				m_bShowOrHideRobot =! m_bShowOrHideRobot;
				SetShowOrHideRobotBnt();
				SetShowRobotNote();
				break;
			}
		default:
			{

				break;
			}
		}
	}
	//右击
	if(pMessage->_uMessageType == UI_RBUTTONDOWN)
	{
		if (m_pUI->GetICursor()->GetControlVisible())
		{
			m_pUI->GetICursor()->SetControlVisible(false);
		}
		m_bySelChouMaType = 255;
		return 0;
	}
	//鼠标移出移入
	if(pMessage->_uMessageType == UI_MOUSEENTER)
	{
		switch(pMessage->_uControlID)
		{
		case BTN_APPLY_MING:	//申请上庄
		case BTN_APPLY_XIAZHUANG:	//申请下庄
		case BTN_MONEY_YIBAI:	//筹码
		case BTN_MONEY_YIQIAN:	//筹码
		case BTN_MONEY_YIWAN:	//筹码
		case BTN_MONEY_SHIWAN:	//筹码
		case BTN_XIAZHU_YIBAIWAN:	//筹码
		case BTN_MONEY_YIQIANWAN:	//筹码
			{
				
				break;
			}
		
		default:
			{
				break;
			}
		}

		return 0;
	}
	//键盘按下
	if (pMessage->_uMessageType == UI_KEYDOWN)
	{
		//按下F12 弹出超端
		if (pMessage->_ch == VK_F12 && m_bIsSuperUser && m_byGameStation == GS_BET_STATUS)
		{
			//清除超端用户界面
			ClearShowSuperResult();
			//显示超端结果界面
			ShowSuperResult(true);
			
			return 0;
		}
	}
	return 0;
	
}

//游戏设置确定/取消
void CNewGameClient::GameSet(BYTE bButton)
{
	if(0==bButton)//确定
	{
		for(int i=0;i<2;i++)
		{
			bool bSelected=FALSE;
			bSelected=GetRButtonSelected(CTN_1000000_CTN_10001420_RButton_10001421+i,&bSelected);
			{
				if(0==i)//关闭声音
				{
					if(bSelected)
					{
						m_bSoundIsClose=TRUE;
						
					}
					else
					{
						m_SoundEngineGame->stopAllSounds(); 
						m_bSoundIsClose=FALSE;
					}
				}
				else if(1==i)//关闭用户进出提示消息
				{
					if(bSelected)
					{
						
						m_bUserExitIsClose=TRUE;
					}
					else
					{
						
						m_bUserExitIsClose=FALSE;
					}
					
				}
				
			}
		}
	}
	else if(1==bButton)//取消
	{
		;
	}
	SetContainVisible(CTN_1000000_CTN_10001420,FALSE);
	for(int i=0;i<2;i++)
	{
		if(0==i)
		{
			SetRButtonSelected(CTN_1000000_CTN_10001420_RButton_10001421+i,m_bSoundIsClose);
		}
		else if(1==i)
		{
			SetRButtonSelected(CTN_1000000_CTN_10001420_RButton_10001421+i,m_bUserExitIsClose);
		}
		
	}
}

//-------------------------------------------------------------------------------------------------------
//设置显示机器人下注数
void	CNewGameClient::SetShowRobotNote()
{
	//区域总下注
	if(m_i64AreaAllSum[0]>0 && m_bShowOrHideRobot)
	{
		SetInumPro(CTN_1000000_CTN_10001369_Num_10001392,m_i64AreaAllSum[0],2,true);
	}
	else if(m_i64RealityAreaAllSum[0] > 0x00)
	{
		SetInumPro(CTN_1000000_CTN_10001369_Num_10001392,m_i64RealityAreaAllSum[0],2,true);
	}
	else
	{
		SetInumPro(CTN_1000000_CTN_10001369_Num_10001392,0,2,FALSE);
	}
	//区域总下注
	if(m_i64AreaAllSum[1]>0 && m_bShowOrHideRobot)
	{
		SetInumPro(CTN_1000000_CTN_10001370_Num_10001394,m_i64AreaAllSum[1],2,true);
	}
	else if(m_i64RealityAreaAllSum[1] > 0x00)
	{
		SetInumPro(CTN_1000000_CTN_10001370_Num_10001394,m_i64RealityAreaAllSum[1],2,true);
	}
	else
	{
		SetInumPro(CTN_1000000_CTN_10001370_Num_10001394,0,2,FALSE);
	}
	//区域总下注
	if(m_i64AreaAllSum[2]>0 && m_bShowOrHideRobot)
	{
		SetInumPro(CTN_1000000_CTN_10001371_Num_10001396,m_i64AreaAllSum[2],2,true);
	}
	else if(m_i64RealityAreaAllSum[2] > 0x00)
	{
		SetInumPro(CTN_1000000_CTN_10001371_Num_10001396,m_i64RealityAreaAllSum[2],2,true);
	}
	else
	{
		SetInumPro(CTN_1000000_CTN_10001371_Num_10001396,0,2,FALSE);
	}

}
//-------------------------------------------------------------------------------------------------------
//显示超端结果界面
void	CNewGameClient::ShowSuperResult(bool bFlag)
{
	if (m_bIsSuperUser)
	{
		SetContainVisible(CTN_1000000_CTN_10001411,bFlag);		///<超端界面  容器
		//SetBtVisible(,bFlag);//<关闭 
		//SetBtVisible(,bFlag);///<确定
	}
}
//清空超端用户界面
void	CNewGameClient::ClearShowSuperResult()
{
	SetContainVisible(CTN_1000000_CTN_10001411,FALSE);		///<超端界面  容器
	for(int i=0;i<3;i++)
	{
		SetRButtonSelected(CTN_1000000_CTN_10001411_RButton_10001415+i,FALSE);
	}
	SetShowOrHideRobotBnt();
}
//--------------------------------------------------------------------------------------------------------
//设置显示或隐藏机器人显示按钮
void CNewGameClient::SetShowOrHideRobotBnt()
{
	IButton *pButtonShow = NULL ;
	IButton *pButtonNoShow = NULL ;
	pButtonShow = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(CTN_1000000_CTN_10001411_BTN_10001426));
	pButtonNoShow = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(CTN_1000000_CTN_10001411_BTN_10001427));
	CString lhx;
	if(pButtonShow!=NULL && pButtonNoShow !=NULL)
	{
		pButtonShow->SetControlVisible(m_bShowOrHideRobot);
		pButtonNoShow->SetControlVisible(!m_bShowOrHideRobot);

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
		//pRadioButton->SetControlVisible(bFlag);
		
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
		//pRadioButton->SetIsSelect(bSelected);
		//pRadioButton->SetControlVisible(bFlag);
		*bSelected=pRadioButton->GetIsSelect();
		return (*bSelected);
	}
}
/*
功能：上庄列表移动
参数：0，上拉；1，下拉
*/
void CNewGameClient::NtListMove(BYTE button)
{
	BYTE byNtListCount=NtListCount();
	if(byNtListCount<=5)
	{
		return;
	}
	if(0==button)
	{
		if(m_byNtWaitListCounter>=5)
		{
			m_byNtWaitListCounter=m_byNtWaitListCounter-5;
			UserInfoForGame UserInfo ;
			if(m_pGameFrame->GetUserInfo(m_byWaitNtList[m_byNtWaitListCounter], UserInfo))
			{
				ShowNtWaitList(TRUE);
			}
			else
			{
				m_byNtWaitListCounter=m_byNtWaitListCounter+5;
			}
		}

	}
	if(1==button)
	{
		if(m_byNtWaitListCounter>=0)
		{
			m_byNtWaitListCounter=m_byNtWaitListCounter+5;
			UserInfoForGame UserInfo ;
			if(m_pGameFrame->GetUserInfo(m_byWaitNtList[m_byNtWaitListCounter], UserInfo))
			{
				ShowNtWaitList(TRUE);
			}
			else
			{
				m_byNtWaitListCounter=m_byNtWaitListCounter-5;
			}
		}
	}
}
//统计上庄列表人数
BYTE CNewGameClient::NtListCount()
{
	BYTE byNtListCount=0;
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(255!=m_byWaitNtList[i])
		{
			byNtListCount++;
		}
	}
	return byNtListCount;
}

//申请下庄
void CNewGameClient::UserApplyLiveNt()
{
	ApplyLiveNtMsg liveNtMsg;
	liveNtMsg.byDeskStation=m_iMyDeskStation;
	//发送申请上庄消息
	m_pGameFrame->SendGameData(&liveNtMsg, sizeof(liveNtMsg) ,MDM_GM_GAME_NOTIFY,ASS_APPLY_LEAVE,0);

}
//申请上庄
void CNewGameClient::UserApplyNtStation()
{
	ApplyNtMsg applyMsg;
	applyMsg.byDeskStation = m_iMyDeskStation;
	//发送申请上庄消息
	m_pGameFrame->SendGameData(&applyMsg, sizeof(applyMsg) ,MDM_GM_GAME_NOTIFY,ASS_APPLY_NT,0);
	
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
//---------------------------------------------------------------------------------------
//播放背景音效
void	CNewGameClient::PlayBgSound(bool bISLoop)
{
	if(!m_SoundEnginBg)
	{
		return ;
	}
	/*if(!m_bSoundBgPlay)
	{
	return ;
	}*/

	char szFileName[MAX_PATH];
	memset(szFileName,0,sizeof(szFileName)) ; 
	wsprintf(szFileName,"music\\sound\\BeiJing.ogg");
	m_SoundEnginBg->setSoundVolume(0.5);
	m_SoundEnginBg->play2D(szFileName,bISLoop);
}

//-------------------------------------------------------------------------
//设置哪些下注按钮可用
void	CNewGameClient::SetBetBtEnable(bool bFalg)
{
	UserInfoForGame UserInfo ; 
	if (m_pGameFrame->GetUserInfo(m_iMyDeskStation , UserInfo))
	{
		__int64 i64Money = UserInfo.i64Money;
		for(int i=0;i<6;i++)
		{

			if (i64Money > G_iChouMaValue[i])
			{
				SetBtEnable(BTN_MONEY_YIBAI+i,bFalg);
			}
			else
			{
				SetBtEnable(BTN_MONEY_YIBAI+i,false);
			}
		}
		
	}
}
//-------------------------------------------------------------------------
//更新显示路子
void CNewGameClient::ShowLuziList(bool bFlag)
{
		CString sFilePath;
		for (int i=0; i<13; i++)
		{
			if (0 != m_byLuziList[i])
			{
				sFilePath.Format(".\\resources\\image\\LuZi\\LuZi_%d.jpg",m_byLuziList[i]);
				SetImageLoadSrc(BTN_BUTTON_LEFT+i,sFilePath,true);
			}
			else
			{
				//SetImageVisible(m_byLuziList+i,false);
			}
		}
	}

//--------------------------------------------------------------------------------------------------------
//设置图片控件加载显示指定的文件
void	CNewGameClient::SetImageLoadSrc(const int iImageID,CString sImagSrc,bool bFlag)
{
	//virtual int LoadPic(wchar_t *szFileName)=0;
	IImage *pImage = NULL;
	pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(iImageID));
	if (NULL != pImage)
	{
		pImage->LoadPic(CA2W(sImagSrc));
		pImage->SetControlVisible(bFlag);
	}
}
//---------------------------------------------------------------------------------
//获取指定玩家的剩余金币
__int64		CNewGameClient::GetUserRemaindMoney(BYTE byDestStation)
{
	__int64 i64Money = 0;
	UserInfoForGame UserInfo ; 
	if (m_pGameFrame->GetUserInfo(byDestStation , UserInfo))
	{
		i64Money = UserInfo.i64Money - GetUserBetSum(byDestStation);
	}
	return i64Money;
}
//---------------------------------------------------------------------------------
//获取某位玩家的总下注情况
__int64		CNewGameClient::GetUserBetSum(BYTE byDestStation)
{
	if (byDestStation > PLAY_COUNT)
	{
		return 0;
	}
	__int64 i64Money = 0;
	for(int i=0; i< BET_ARES; i++)
	{
		//i64Money += m_i64UserBetCount[byDestStation][i];
	}
	return i64Money;
}
//--------------------------------------------------------------------------------------------------------
//设置图片控件是否可见
void	CNewGameClient::SetImageVisible(const int iImageID, bool bFlag)
{
	IImage *pImage = NULL;
	pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(iImageID));
	if (NULL != pImage)
	{
		pImage->SetControlVisible(bFlag);
	}
}

/// 玩家强退结束游戏
bool CNewGameClient::UserExit(WORD nSubId,BYTE * buffer)
{
	tagGameForceQuit *quit=(tagGameForceQuit *)buffer;
	if (NULL==quit)
	{
		return true;
	}
	switch(nSubId)
	{
	case ASS_CUT_END:
		{
			TCHAR szMessage[100]={0};
			UserInfoForGame userInfo;
			CString cs;
			cs.Format("退出玩家=%d",quit->byDeskStation);
			OutputDebugString(cs);
			m_pGameFrame->GetUserInfo(quit->byDeskStation,userInfo);
			_stprintf_s(szMessage, sizeof(szMessage), TEXT("〖 %s 〗强退了"),userInfo.szNickName);
			m_pGameFrame->InsertNormalMessage(szMessage);
			break;
		}
	case ASS_SAFE_END:
		{
			TCHAR szMessage[100]={0};
			UserInfoForGame userInfo;
			m_pGameFrame->GetUserInfo(quit->byDeskStation,userInfo);
			_stprintf_s(szMessage, sizeof(szMessage), TEXT("〖 %s 〗退出了"),userInfo.szNickName);
			m_pGameFrame->InsertNormalMessage(szMessage);
			break;
		}
	}
	return TRUE;
}
//用户准备阶段退出消息
void	CNewGameClient::HandleGamePrepareQuit(void * pBuffer)
{
	GamePrepareStageQuit *prepareQuit = (GamePrepareStageQuit *)pBuffer;
	if(NULL == prepareQuit)
	{
		return ;
	}
	m_byNtStation = prepareQuit->byNtStation;
	m_iNtPlayCount= prepareQuit->byNtcount;
	m_i64NtScore=prepareQuit->i64NtScore;

	memset(m_byWaitNtList,255,sizeof(m_byWaitNtList));

	//复制上庄列表
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(255==prepareQuit->byNtWaitList[i])
		{
			continue;
		}

		m_byWaitNtList[i]=prepareQuit->byNtWaitList[i];
	}
	//刷新庄家信息
	ShowNtInfo(TRUE);
	//刷新庄家列表
	ShowNtWaitList(TRUE);
}
//-----------------------------------------------------------------------------------------
/// 游戏消息
int CNewGameClient::HandleGameMessage(WORD nMainID,WORD nSubId,BYTE * buffer,int nLen)
{
	switch(nSubId)
	{
	case ASS_GM_GAME_STATION:   ///<设置状态(平台调用的)
		{
			SetGameStation(buffer, nLen);    
			break;        
		}
	case ASS_CUT_END:	///用户强行离开
	case ASS_SAFE_END:
		{
			UserExit(nSubId, buffer);
			break;
		}
	case ASS_PREPARE_END://用户准备阶段退出
		{
			if(nLen!=sizeof(GamePrepareStageQuit))
			{
				return 0;
			}
			HandleGamePrepareQuit(buffer);
			break;
		}
	case ASS_GM_AGREE_GAME:
		{		
			if(nLen!=sizeof(MSG_GR_R_UserAgree) || m_bIsPlaying)
			{
				return 0;
			}				

			MSG_GR_R_UserAgree * pUserAgree=(MSG_GR_R_UserAgree *)buffer;
			if (NULL == pUserAgree)
			{
				return 0;
			}
			if (pUserAgree->bDeskStation == m_iMyDeskStation)
			{
				m_bIsReady = true;	//已经准备了
				//重置游戏界面
				ReSetUiDlag();
				//隐藏倒计时
				//SetTimePlay(CTN_1000000_CTN_1000100_Time_1000199,false,0);
			}
			break;
		}
	case ASS_SUPER_USER:	//超端客户端
		{
			
			if(nLen!=sizeof(SUPERSTATE))
			{
				return 0;
			}	
			HandleSuperMessage(buffer);
			break;
		}
	case ASS_GAME_COME:
		{
			if(nLen!=sizeof(UserComePlayerMsg))
			{
				return 0;
			}
			PlayCome(buffer);
			break;
		}

	case ASS_GAME_FREE:	//空闲
		{
			if(nLen!=sizeof(BeginFreeMsg))
			{
				return 0;
			}
			
			HandleGameFree(buffer);
			break;
		}
	case ASS_BEGIN_BET:	//开始下注(下注阶段)
		{
			if(nLen!=sizeof(BeginBetMsg))
			{
				return 0;
			}	
			HandleBeginBet(buffer);
			break;
		}
	
	case ASS_GAME_SEND://开始开牌（开牌阶段）
		{
			if(nLen!=sizeof(BeginSendCardMsg))
			{
				return 0;
			}	
			//开牌
			HandleKaiPai(buffer);
			break;
		}
	case ASS_GAME_DATA://结算的数据
		{
			if(nLen!=sizeof(BeginShowResultData))
			{
				return 0;
			}
			HandleReceiveData(buffer);
			break;
		}
	case ASS_GAME_ACCOUNT://显示游戏结算框
		{
			if(nLen!=sizeof(BeginShowResult))
			{
				return 0;
			}	
			ShowGameResult(buffer) ; 
			break;
		}
	case ASS_APPLY_RESULT://申请上庄结果消息
		{
			if(nLen!=sizeof(ApplyNtResultMsg))
			{
				return 0;
			}
			
			HandleApplyResult(buffer);
			break;
		}
	case ASS_LEAVE_RESULT:	//申请下庄结果
		{
			
			if(nLen!=sizeof(ApplyLiveNtResultMsg))
			{
				return 0;
			}
			
			HandleLeaveNtResult(buffer);
			break;
		}
	case ASS_BET_RESULT:	//下注结果
		{
			
			if(nLen!=sizeof(UserBetResultMsg))
			{
				return 0;
			}
			HandleUserBetResult(buffer);
			break;
		}
	
	default:
		{
			break;
		}

	}
	return 0;
}
//处理超端用户信息
void CNewGameClient::HandleSuperMessage(void *buffer)
{
	SUPERSTATE *pSuperUser = (SUPERSTATE *) buffer;
	if (NULL == pSuperUser)
	{
		return ;
	}
	m_iMyDeskStation = m_pGameFrame->GetMyDeskStation(); 
	/*CString cs;
	cs.Format("test  pSuperUser->byDeskStation=%d",pSuperUser->byDeskStation);
	OutputDebugString(cs);
	cs.Format("test  m_iMyDeskStation=%d",m_iMyDeskStation);
	OutputDebugString(cs);*/
	if (pSuperUser->byDeskStation == m_iMyDeskStation)
	{
				
		m_bIsSuperUser	 = TRUE;	//是否超级用户
		CString cs;
		cs.Format("test  超端客户端=%d",m_iMyDeskStation);
		OutputDebugString(cs);
	}
}
//接受结算数据
void CNewGameClient::HandleReceiveData(void *buffer)
{
	BeginShowResultData *receiveData = (BeginShowResultData *)buffer;
	if(NULL == receiveData)
	{
		return ;
	}
	i64PlayerGrade[receiveData->bDeskStation]=receiveData->i64PlayerGrade;//玩家成绩
	i64ChangeMoney[receiveData->bDeskStation]=receiveData->i64ChangeMoney;//玩家得分
	i64ReturnMoney[receiveData->bDeskStation]=receiveData->i64ReturnMoney;//返还的金币数 
	m_i64NtScore=i64PlayerGrade[m_byNtStation];

}
//玩家进入游戏
void CNewGameClient::PlayCome(void *buffer)
{
	UserComePlayerMsg *playerComeMsg = (UserComePlayerMsg *)buffer;
	if(NULL == playerComeMsg)
	{
		return ;
	}
	UserInfoForGame UserInfo ; 
	if(m_pGameFrame->GetUserInfo(playerComeMsg->byPlayStation, UserInfo))
	{
		TCHAR cs[100];
		_stprintf_s(cs, sizeof(cs), TEXT("%s玩家进入了游戏"), UserInfo.szNickName);
		m_pGameFrame->InsertNormalMessage(cs);
	}
	
}
void CNewGameClient::ChaoGuo(void *buffer)
{
	ChaoguoMsg *startMsg = (ChaoguoMsg *)buffer;
	if(NULL == startMsg)
	{
		return ;
	}
	for(int i=0;i<6;i++)
	{
		SetBtEnable(BTN_MONEY_YIBAI+i,false);
	}
}

void CNewGameClient::HandleStart(void *buffer)
{
	StartMsg *startMsg = (StartMsg *)buffer;
	if(NULL == startMsg)
	{
		return ;
	}
	    
	//设置时钟倒计时
	SetTimePlay(BTN_CLOCK,true,startMsg->startTime);  

}
//处理申请下庄结果消息
void CNewGameClient::HandleLeaveNtResult(void *buffer)
{
	
	ApplyLiveNtResultMsg *ShowResultMsg = (ApplyLiveNtResultMsg *)buffer;
	if(NULL == ShowResultMsg)
	{
		return ;
	}
	m_byNtStation=ShowResultMsg->byNtStation;
	m_iNtPlayCount=ShowResultMsg->byNtcount;//庄家坐庄次数
	//庄家的成绩
	m_i64NtScore=ShowResultMsg->i64NtScore;

	ShowNtInfo(TRUE);
	//复制上庄列表
	memset(m_byWaitNtList,255,sizeof(m_byWaitNtList));
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(255==ShowResultMsg->byWaitNtList[i])
		{
			continue;
		}

		m_byWaitNtList[i]=ShowResultMsg->byWaitNtList[i];
	}

	BOOL flag=false;//是否在上庄列表中的标志位
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(m_iMyDeskStation==m_byWaitNtList[i])//自己在庄家列表中
		{
			flag=true;

			break;
		}
	}
	if(flag)//在庄家列表里能下注
	{
		//控制上庄下庄按钮的使用和显示
		BtNtDisp(1);
	}
	else
	{
		if((m_byNtStation!=255)&&(m_byNtStation==m_iMyDeskStation))//自己是庄家在空閒階段才能下莊
		{
			if(GS_FREE_STATUS==m_byGameStation)
			{
				//控制上庄下庄按钮的使用和显示
				BtNtDisp(1);
			}
			else
			{
				//控制上庄下庄按钮的使用和显示
				BtNtDisp(2);
			}

		}
		else
		{
			//检查是否达到上庄金币
			UserInfoForGame UserInfo ; 
			memset(&UserInfo,0,sizeof(UserInfo));
			//自己金币
			if (m_pGameFrame->GetUserInfo(m_iMyDeskStation , UserInfo))
			{
				if(UserInfo.i64Money>=m_i64ApplyNtLimite)
				{
					//控制上庄下庄按钮的使用和显示
					BtNtDisp(0);
				}
				else
				{
					//控制上庄下庄按钮的使用和显示
					BtNtDisp(2);
				}
			}
			m_byNtWaitListCounter=0;
		}
	}
	//显示庄家列表
	ShowNtWaitList(true);
	
}
//空闲状态
void CNewGameClient::HandleFreeStation(void *buffer)
{
	GameStation_Base *TGameStation = (GameStation_Base *)buffer;
	if(NULL == TGameStation)
	{
		return ;
	}
	
	
	//玩家把数
	SetTextinfo(BTN_PLAY_BASHU,m_iPlayBaShu,true);

	m_byGameStation = TGameStation->bGameStation;	//游戏状态
	m_byFreeTime=TGameStation->byFreeTime;			//空闲时间
	m_byBetTime=TGameStation->byBetTime;			//下注时间
	m_byOpenCardTime=TGameStation->bySendTime;	    //开牌时间
	byAccountTime=TGameStation->byAccountTime;//结算时间
	m_byShowResult=TGameStation->byShowResult;//展示结果时间
	m_bySendSpaceTime=TGameStation->bySendCardSpaceTime;	//发牌间隔时间
	m_bySpeedTime=TGameStation->bySpeedTime;               //遮挡牌移动速度
	m_byShowWinTime=TGameStation->byShowWinTime;
	m_iClock=TGameStation->byRemaindTime;                     //剩余时间

	m_i64ApplyNtLimite=TGameStation->iApplyNtLimite ; //庄家上庄条件

	//庄家信息（把数和成绩）
	m_byNtStation=TGameStation->byNtStation;		//庄家位置

	m_i64NtScore=TGameStation->i64NtGrade;	//庄家成绩
	m_iNtPlayCount=TGameStation->iNtPlayCount;		//庄家坐庄次数
	m_byMaxNtPeople=TGameStation->byMaxNtPeople;		//最大上庄人数
	m_iPlayGrade=TGameStation->i64PlayerGrade;//玩家成績
	m_iPlayJuShu=TGameStation->byEvent;//游戏局数
	if(255!=m_byNtStation)
	{
		m_iPlayBaShu++;
	}

	//显示庄家信息
	ShowNtInfo(true);

	//显示自己的信息
	ShowMyInfo(true);
	//显示阶段图片
	ShowSection(0);
	//总下注数清零
	//SetInumPro(CTN_1000000_CTN_10001421_Num_10001429,0,2,true);
	m_dbyLuzi.clear();
	//复制路子
	for(int i=0;i<72;i++)
	{
		if(255==TGameStation->byLuziList[i])
		{
			continue;
		}
		m_dbyLuzi.push_back(TGameStation->byLuziList[i]);
	}

	ShowLuzi();
	int iNtCount=0;
	memset(m_byWaitNtList,255,sizeof(m_byWaitNtList));
	//复制上庄列表信息
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(255==TGameStation->byWaitNtList[i])
		{
			continue;
		}
		m_byWaitNtList[i]=TGameStation->byWaitNtList[i];
		iNtCount++;
	}
	
	//显示庄家列表
	ShowNtWaitList(true);

	BOOL flag=false;//是否在上庄列表中的标志位
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(m_iMyDeskStation==m_byWaitNtList[i])//自己在庄家列表中
		{
			flag=true;

			break;
		}
	}

	if(flag)//在庄家列表里能下注
	{
		//控制上庄下庄按钮的使用和显示
		BtNtDisp(1);
	}
	else
	{
		if((m_byNtStation!=255)&&(m_byNtStation==m_iMyDeskStation))//自己是庄家不能下注
		{
			if(GS_FREE_STATUS==m_byGameStation)
			{
				//控制上庄下庄按钮的使用和显示
				BtNtDisp(1);
			}
			else
			{
				//控制上庄下庄按钮的使用和显示
				BtNtDisp(2);
			}
		}
		else
		{
			if(iNtCount>=m_byMaxNtPeople)
			{
				//控制上庄下庄按钮的使用和显示
				BtNtDisp(2);
			}
			else
			{
				//检查是否达到上庄金币
				UserInfoForGame UserInfo ; 
				memset(&UserInfo,0,sizeof(UserInfo));
				//自己金币
				if (m_pGameFrame->GetUserInfo(m_iMyDeskStation , UserInfo))
				{
					if(UserInfo.i64Money>=m_i64ApplyNtLimite)
					{
						//控制上庄下庄按钮的使用和显示
						BtNtDisp(0);
					}
					else
					{
						//控制上庄下庄按钮的使用和显示
						BtNtDisp(2);
					}
				}

			}

		}

	}
	//控制能否下注
	BtEnable(false);
	//显示下注按钮
	SetBetBtEnable(false,1);

	//启动倒计时
	//设置时间框显示倒计时
	//SetTimePlay(BTN_CLOCK,true, m_iClock);

	m_iClockLable=m_iClock;                     //时钟标签
	SetInumPro(CTN_1000000_CTN_10001408_Num_10001409,m_iClock,2,true); 
	//设置时钟倒计时
	m_pGameFrame->SetTimer(TIME_ClOCK,1000);

	TCHAR cs[100];
	_stprintf_s(cs, sizeof(cs), TEXT("上庄金币:%I64d"), m_i64ApplyNtLimite);
	m_pGameFrame->InsertNormalMessage(cs);
	PlayGameSound(SOUND_GAME_E,FALSE);
}
//下注状态
void CNewGameClient::HandleXiaZhuStation(void *buffer)
{
	GameStation_Base *TGameStation = (GameStation_Base *)buffer;
	if(NULL == TGameStation)
	{
		return ;
	}
	
	//玩家把数
	SetTextinfo(BTN_PLAY_BASHU,m_iPlayBaShu,true); 

	m_byGameStation = TGameStation->bGameStation;	//游戏状态
	m_byFreeTime=TGameStation->byFreeTime;			//空闲时间
	m_byBetTime=TGameStation->byBetTime;			//下注时间
	m_byOpenCardTime=TGameStation->bySendTime;	    //开牌时间
	byAccountTime=TGameStation->byAccountTime;//结算时间
	m_byShowResult=TGameStation->byShowResult;//展示结果时间
	m_bySendSpaceTime=TGameStation->bySendCardSpaceTime;	//发牌间隔时间
	m_bySpeedTime=TGameStation->bySpeedTime;               //遮挡牌移动速度
	m_byShowWinTime=TGameStation->byShowWinTime;
	m_iClock=TGameStation->byRemaindTime;                     //剩余时间

	m_i64ApplyNtLimite=TGameStation->iApplyNtLimite ; //庄家上庄条件

	//庄家信息（把数和成绩）
	m_byNtStation=TGameStation->byNtStation;		//庄家位置

	m_i64NtScore=TGameStation->i64NtGrade;	//庄家成绩
	m_iNtPlayCount=TGameStation->iNtPlayCount;		//庄家坐庄次数
	m_byMaxNtPeople=TGameStation->byMaxNtPeople;		//最大上庄人数
	m_iPlayGrade=TGameStation->i64PlayerGrade;//玩家成績
	m_iPlayJuShu=TGameStation->byEvent;//游戏局数
	if(255!=m_byNtStation)
	{
		m_iPlayBaShu++;
	}
	//显示庄家信息
	ShowNtInfo(true);

	//显示自己的信息
	ShowMyInfo(true);

	//显示阶段图片
	ShowSection(1);
	m_dbyLuzi.clear();
	//复制路子
	for(int i=0;i<72;i++)
	{
		if(255==TGameStation->byLuziList[i])
		{
			continue;
		}
		m_dbyLuzi.push_back(TGameStation->byLuziList[i]);
	}         
	ShowLuzi();
	int iNtCount=0;
	memset(m_byWaitNtList,255,sizeof(m_byWaitNtList));
	//复制上庄列表信息
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(255==TGameStation->byWaitNtList[i])
		{
			continue;
		}

		m_byWaitNtList[i]=TGameStation->byWaitNtList[i];
		iNtCount++;
	}

	//显示庄家列表
	ShowNtWaitList(true);

	BOOL flag=false;//是否在上庄列表中的标志位
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(m_iMyDeskStation==m_byWaitNtList[i])//自己在庄家列表中
		{
			flag=true;

			break;
		}
	}

	if(flag)//在庄家列表里能下注
	{
		//控制上庄下庄按钮的使用和显示
		BtNtDisp(1);
		//控制能否下注
		BtEnable(true);
		//显示下注按钮
		SetBetBtEnable(true,0);
	}
	else
	{
		if((m_byNtStation!=255)&&(m_byNtStation==m_iMyDeskStation))//自己是庄家不能下注
		{
			if(GS_FREE_STATUS==m_byGameStation)
			{
				//控制上庄下庄按钮的使用和显示
				BtNtDisp(1);
			}
			else
			{
				//控制上庄下庄按钮的使用和显示
				BtNtDisp(2);
			}
			//控制能否下注
			BtEnable(false);
			//显示下注按钮
			SetBetBtEnable(false,1);
		}
		else
		{
			if(iNtCount>=m_byMaxNtPeople)
			{
				//控制上庄下庄按钮的使用和显示
				BtNtDisp(2);
			}
			else
			{
				//检查是否达到上庄金币
				UserInfoForGame UserInfo ; 
				memset(&UserInfo,0,sizeof(UserInfo));
				//自己金币
				if (m_pGameFrame->GetUserInfo(m_iMyDeskStation , UserInfo))
				{
					if(UserInfo.i64Money>=m_i64ApplyNtLimite)
					{
						//控制上庄下庄按钮的使用和显示
						BtNtDisp(0);
					}
					else
					{
						//控制上庄下庄按钮的使用和显示
						BtNtDisp(2);
					}
				}
			}
			//控制能否下注
			BtEnable(true);
			//显示下注按钮
			SetBetBtEnable(true,0);
		}

	}
	if(255==m_byNtStation)
	{
		//控制能否下注
		BtEnable(false);
		//显示下注按钮
		SetBetBtEnable(false,1);
	}

	//设置时钟倒计时
	//SetTimePlay(BTN_CLOCK,true,m_iClock);

	m_iClockLable=m_iClock;                     //时钟标签
	SetInumPro(CTN_1000000_CTN_10001408_Num_10001409,m_iClock,2,true); 
	//设置时钟倒计时
	m_pGameFrame->SetTimer(TIME_ClOCK,1000);
	TCHAR cs[100];
	_stprintf_s(cs, sizeof(cs), TEXT("上庄金币:%I64d"), m_i64ApplyNtLimite);
	m_pGameFrame->InsertNormalMessage(cs);
	PlayGameSound(SOUND_GAME_E, FALSE); 
}
//开牌状态
void CNewGameClient::HandleKaiPaiStation(void *buffer)
{
	GameStation_Base *TGameStation = (GameStation_Base *)buffer;
	if(NULL == TGameStation)
	{
		return ;
	}
	  
	//玩家把数
	SetTextinfo(BTN_PLAY_BASHU,m_iPlayBaShu,true); 

	m_byGameStation = TGameStation->bGameStation;	//游戏状态
	m_byFreeTime=TGameStation->byFreeTime;			//空闲时间
	m_byBetTime=TGameStation->byBetTime;			//下注时间
	m_byOpenCardTime=TGameStation->bySendTime;	    //开牌时间
	byAccountTime=TGameStation->byAccountTime;//结算时间
	m_byShowResult=TGameStation->byShowResult;//展示结果时间
	m_bySendSpaceTime=TGameStation->bySendCardSpaceTime;	//发牌间隔时间
	m_bySpeedTime=TGameStation->bySpeedTime;               //遮挡牌移动速度
	m_byShowWinTime=TGameStation->byShowWinTime;
	m_iClock=TGameStation->byRemaindTime;                     //剩余时间

	m_i64ApplyNtLimite=TGameStation->iApplyNtLimite ; //庄家上庄条件

	//庄家信息（把数和成绩）
	m_byNtStation=TGameStation->byNtStation;		//庄家位置

	m_i64NtScore=TGameStation->i64NtGrade;	//庄家成绩
	m_iNtPlayCount=TGameStation->iNtPlayCount;		//庄家坐庄次数
	m_byMaxNtPeople=TGameStation->byMaxNtPeople;		//最大上庄人数
	m_iPlayGrade=TGameStation->i64PlayerGrade;//玩家成績
	m_iPlayJuShu=TGameStation->byEvent;//游戏局数
	
	if(255!=m_byNtStation)
	{
		m_iPlayBaShu++;
	}
	//显示庄家信息
	ShowNtInfo(true);

	//显示自己的信息
	ShowMyInfo(true);

	//显示阶段图片
	ShowSection(2);
	//显示总下注

	//显示各区域总下注
	//SetInumPro(CTN_1000000_CTN_10001421_Num_10001429,0,2,true);
	m_dbyLuzi.clear();
	//复制路子
	for(int i=0;i<72;i++)
	{
		if(255==TGameStation->byLuziList[i])
		{
			continue;
		}
		m_dbyLuzi.push_back(TGameStation->byLuziList[i]);
	}         
	ShowLuzi();
	int iNtCount=0;
	memset(m_byWaitNtList,255,sizeof(m_byWaitNtList));
	//复制上庄列表信息
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(255==TGameStation->byWaitNtList[i])
		{
			continue;
		}
		m_byWaitNtList[i]=TGameStation->byWaitNtList[i];
		iNtCount++;
	}

	//显示庄家列表
	ShowNtWaitList(true);

	BOOL flag=false;//是否在上庄列表中的标志位
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(m_iMyDeskStation==m_byWaitNtList[i])//自己在庄家列表中
		{
			flag=true;

			break;
		}
	}

	if(flag)//在庄家列表里能下注
	{
		//控制上庄下庄按钮的使用和显示
		BtNtDisp(1);

	}
	else
	{
		if((m_byNtStation!=255)&&(m_byNtStation==m_iMyDeskStation))//自己是庄家不能下注
		{
			//控制上庄下庄按钮的使用和显示
			BtNtDisp(2);

		}
		else
		{
			if(iNtCount>=m_byMaxNtPeople)
			{
				if(GS_FREE_STATUS==m_byGameStation)
				{
					//控制上庄下庄按钮的使用和显示
					BtNtDisp(1);
				}
				else
				{
					//控制上庄下庄按钮的使用和显示
					BtNtDisp(2);
				}
			}
			else
			{
				//检查是否达到上庄金币
				UserInfoForGame UserInfo ; 
				memset(&UserInfo,0,sizeof(UserInfo));
				//自己金币
				if (m_pGameFrame->GetUserInfo(m_iMyDeskStation , UserInfo))
				{
					if(UserInfo.i64Money>=m_i64ApplyNtLimite)
					{
						//控制上庄下庄按钮的使用和显示
						BtNtDisp(0);
					}
					else
					{
						//控制上庄下庄按钮的使用和显示
						BtNtDisp(2);
					}
				}
			}
		}

	}
	//控制能否下注
	BtEnable(false);
	//显示下注按钮
	SetBetBtEnable(false,1);
	//设置时钟倒计时
	//SetTimePlay(BTN_CLOCK,true,m_iClock);
	m_iClockLable=m_iClock;                     //时钟标签
	SetInumPro(CTN_1000000_CTN_10001408_Num_10001409,m_iClock,2,true); 
	//设置时钟倒计时
	m_pGameFrame->SetTimer(TIME_ClOCK,1000);
	TCHAR cs[100];
	_stprintf_s(cs, sizeof(cs), TEXT("上庄金币:%I64d"), m_i64ApplyNtLimite);
	m_pGameFrame->InsertNormalMessage(cs);
	PlayGameSound(SOUND_GAME_E,FALSE);
}
//显示路子
void CNewGameClient::ShowLuzi()
{
	//显示路子
	for(int i=0;i<m_dbyLuzi.size();i++)
	{
		if(0==m_dbyLuzi.at(i))
		{
			CString src;
			src.Format(".\\resources\\image\\daqi.bmp");
			SetPicSize(BTN_LUZI_1+i,src, true,0,0,27,28);
		}
		else if(1==m_dbyLuzi.at(i))
		{
			CString src;
			src.Format(".\\resources\\image\\daqi.bmp");
			SetPicSize(BTN_LUZI_1+i,src,true,27,0,27,28);
		}
		else if(2==m_dbyLuzi.at(i))
		{
			CString src;
			src.Format(".\\resources\\image\\daqi.bmp");
			SetPicSize(BTN_LUZI_1+i,src, true,54,0,27,28);
		}
	}
}
//隐藏路子
void CNewGameClient::HideLuzi()
{
	//显示路子
	for(int i=0;i<72;i++)
	{
		SetImageVisible(BTN_LUZI_1+i,FALSE);
	}
}
//显示结算框
void CNewGameClient::HandleShowResult()
{
	;
}

//--------------------------------------------------------------------------------------------------------
//设置容器控件可见
void	CNewGameClient::SetContainVisible(const int iContainID,bool bFlag)
{
	IContain *pContain = NULL;
	pContain = dynamic_cast<IContain *>(m_pUI->GetIControlbyID(iContainID));
	if (NULL != pContain)
	{
		pContain->SetControlVisible(bFlag);
	}
}
//处理玩家下注结果
void CNewGameClient::HandleUserBetResult(void *buffer)
{
	
	UserBetResultMsg *BetResultMsg = (UserBetResultMsg *)buffer;
	if(NULL == BetResultMsg)
	{
		return ;
	}
	PlayGameSound(SOUND_GAME_A, FALSE);
	//显示总下注
	//SetInumPro(CTN_1000000_CTN_10001421_Num_10001429,BetResultMsg->i64SumAllBet,2,true);
	//

	if(0x01==BetResultMsg->byBetNumFlag)
	{
		PlayGameSound(SOUND_GAME_K, FALSE);
	}
	else if(0x02==BetResultMsg->byBetNumFlag)
	{
		PlayGameSound(SOUND_GAME_L, FALSE);
	}
	else if(0x03==BetResultMsg->byBetNumFlag)
	{
		PlayGameSound(SOUND_GAME_D, FALSE);
	}
	//对应区域显示筹码值
	SetShCtrlInfo(BTN_BET_LEFT+BetResultMsg->byBetArea,G_iChouMaValue[BetResultMsg->byChouMaType], true);

	for(int i = 0x00;i < BET_ARES;i++)
	{
		m_i64AreaAllSum[i] = BetResultMsg->i64AreaBetAllSum[i];
		m_i64RealityAreaAllSum[i] = BetResultMsg->i64AreaBetAllSum[i] - BetResultMsg->i64AreaRobot[i];
	}
	//显示区域自己下注和总下注
	//区域自己下注
	if(BetResultMsg->i64UserBetCount[0]>0)
	{
		SetInumPro(CTN_1000000_CTN_10001369_Num_10001393,BetResultMsg->i64UserBetCount[0],2,true);
	}
	if(BetResultMsg->i64UserBetCount[1]>0)
	{
		SetInumPro(CTN_1000000_CTN_10001370_Num_10001395,BetResultMsg->i64UserBetCount[1],2,true);
	}
	if(BetResultMsg->i64UserBetCount[2]>0)
	{
		SetInumPro(CTN_1000000_CTN_10001371_Num_10001397,BetResultMsg->i64UserBetCount[2],2,true);
	}
	//区域总下注
	SetShowRobotNote();
	UserInfoForGame UserInfo ;
	memset(&UserInfo,0,sizeof(UserInfo));
	if(m_pGameFrame->GetUserInfo(m_iMyDeskStation, UserInfo))
	{
		m_i64PlayerCurrentMoney=UserInfo.i64Money;
	}
	__int64 betMoney=0;
	for(int i=0;i<BET_ARES;i++)
	{
		betMoney+=BetResultMsg->i64UserBetCount[i]; //闲家在每个区域的下注情况
	}
	m_i64PlayerCurrentMoney=m_i64PlayerCurrentMoney-betMoney;
	//玩家金币
	SetTextinfo(BTN_PLAY_MONEY,m_i64PlayerCurrentMoney,true);
	//根据剩余的钱显示筹码按钮
	if(m_iMyDeskStation!=m_byNtStation)
	{
		for(int i=0;i<6;i++)
		{
			if (m_i64PlayerCurrentMoney > G_iChouMaValue[i])
			{
				SetBtEnable(CTN_1000000_CTN_10001332_BTN_10001182+i,true);
			}
			else
			{
				SetBtEnable(CTN_1000000_CTN_10001332_BTN_10001182+i,FALSE);
			}
		}
	}
	if(255 != m_bySelChouMaType)
	{
		if(G_iChouMaValue[m_bySelChouMaType]>m_i64PlayerCurrentMoney)
		{
			if (m_pUI->GetICursor()->GetControlVisible())
			{
				m_pUI->GetICursor()->SetControlVisible(false);
			}
			m_bySelChouMaType = 255;
		}
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

//开牌
void CNewGameClient::HandleKaiPai(void *buffer)
{
	BeginSendCardMsg *pBeginKaiPai = (BeginSendCardMsg *)buffer;
	
	if(NULL == pBeginKaiPai)
	{
		return ;
	}
	m_byGameStation =pBeginKaiPai->byGameStation;         ///当前的游戏状态
	m_byArea=pBeginKaiPai->byWinArea;
	//隐藏移动筹码
	if (m_pUI->GetICursor()->GetControlVisible())
	{
		m_pUI->GetICursor()->SetControlVisible(false);
	}
	m_bySelChouMaType = 255;
	m_SoundEngineGame->stopAllSounds();
	PlayGameSound(SOUND_GAME_C,FALSE); 
	//显示阶段图片
	ShowSection(2);
	//删除定时器
	m_pGameFrame->KillTimer(TIME_ClOCK);
	//开奖阶段不能下注
	//控制能否下注
	BtEnable(false);
	//显示下注按钮
	SetBetBtEnable(false,1);

	//清空上一把牌型
	memset(m_byCardStyle,-1,sizeof(m_byCardStyle));
	for(int i=0;i<pBeginKaiPai->byCardCount;i++)
	{
		m_byCardStyle[i]=pBeginKaiPai->byCardStyle[i];
	}
	BYTE iNtCount=0;
	memset(m_byWaitNtList,255,sizeof(m_byWaitNtList));
	//复制上庄列表
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(255==pBeginKaiPai->byNtWaitList[i])
		{
			continue;
		}

		m_byWaitNtList[i]=pBeginKaiPai->byNtWaitList[i];
		iNtCount++;
	}

	//显示庄家列表
	ShowNtWaitList(true);

	BOOL flag=false;//是否在上庄列表中的标志位
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(m_iMyDeskStation==m_byWaitNtList[i])//自己在庄家列表中
		{
			flag=true;

			break;
		}
	}


	if(flag)//在庄家列表里能下注
	{
		//控制上庄下庄按钮的使用和显示
		BtNtDisp(1);
	}
	else
	{
		if((m_byNtStation!=255)&&(m_byNtStation==m_iMyDeskStation))//自己是庄家不能下庄
		{
			if(GS_FREE_STATUS==m_byGameStation)
			{
				//控制上庄下庄按钮的使用和显示
				BtNtDisp(1);
			}
			else
			{
				//控制上庄下庄按钮的使用和显示
				BtNtDisp(2);
			}

		}
		else
		{
			//上庄要检查上庄人数和上庄金币
			if(iNtCount>=m_byMaxNtPeople)
			{
				//控制上庄下庄按钮的使用和显示
				BtNtDisp(2);
			}
			else
			{
				//检查是否达到上庄金币
				UserInfoForGame UserInfo ; 
				memset(&UserInfo,0,sizeof(UserInfo));
				//自己金币
				if (m_pGameFrame->GetUserInfo(m_iMyDeskStation , UserInfo))
				{
					if(UserInfo.i64Money>=m_i64ApplyNtLimite)
					{
						//控制上庄下庄按钮的使用和显示
						BtNtDisp(0);
					}
					else
					{
						//控制上庄下庄按钮的使用和显示
						BtNtDisp(2);
					}
				}
			}

		}

	}
	//清空路子
	m_dbyLuzi.clear();
	//复制路子
	for(int i=0;i<72;i++)
	{
		if(255==pBeginKaiPai->byLuZi[i])
		{
			continue;
		}
		m_dbyLuzi.push_back(pBeginKaiPai->byLuZi[i]);
	}

	//设置发牌时钟倒计时
	//SetTimePlay(BTN_CLOCK,true,m_byOpenCardTime);
	m_iClockLable=m_byOpenCardTime;                     //时钟标签
	SetInumPro(CTN_1000000_CTN_10001408_Num_10001409,m_byOpenCardTime,2,true); 
	//设置时钟倒计时
	m_pGameFrame->SetTimer(TIME_ClOCK,1000);


	//启动发龙牌定时器
	m_pGameFrame->SetTimer(TIME_SENDLONG_CARD,m_bySendSpaceTime*1000);
}
///显示玩家的发牌动画
void CNewGameClient::ShowUserSendCard(const int ID,BYTE cardStyle,BYTE cardCount,bool bShow) 
{
	
	INoMoveCard  *pNoMoveCard = NULL ; 
		pNoMoveCard = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(ID))  ; 
		
		if(NULL != pNoMoveCard)
		{
			BYTE iCardList[45] ; 
			memset(iCardList , 0x40, sizeof(iCardList)) ; 
			

			switch(cardStyle)
			{
				//方块2-A
			case 0x01:iCardList[0]=49;break;
			case 0x02:iCardList[0]=50;break;
			case 0x03:iCardList[0]=51;break;
			case 0x04:iCardList[0]=52;break;
			case 0x05:iCardList[0]=53;break;
			case 0x06:iCardList[0]=54;break;
			case 0x07:iCardList[0]=55;break;
			case 0x08:iCardList[0]=56;break;
			case 0x09:iCardList[0]=57;break;
			case 0x0A:iCardList[0]=58;break;
			case 0x0B:iCardList[0]=59;break;
			case 0x0C:iCardList[0]=60;break;
			case 0x0D:iCardList[0]=61;break;
				////梅花 2 - A
			case 0x11:iCardList[0]=33;break;
			case 0x12:iCardList[0]=34;break;
			case 0x13:iCardList[0]=35;break;
			case 0x14:iCardList[0]=36;break;
			case 0x15:iCardList[0]=37;break;
			case 0x16:iCardList[0]=38;break;
			case 0x17:iCardList[0]=39;break;
			case 0x18:iCardList[0]=40;break;
			case 0x19:iCardList[0]=41;break;
			case 0x1A:iCardList[0]=42;break;
			case 0x1B:iCardList[0]=43;break;
			case 0x1C:iCardList[0]=44;break;
			case 0x1D:iCardList[0]=45;break;
				////红桃 2 - A
			case 0x21:iCardList[0]=17;break;
			case 0x22:iCardList[0]=18;break;
			case 0x23:iCardList[0]=19;break;
			case 0x24:iCardList[0]=20;break;
			case 0x25:iCardList[0]=21;break;
			case 0x26:iCardList[0]=22;break;
			case 0x27:iCardList[0]=23;break;
			case 0x28:iCardList[0]=24;break;
			case 0x29:iCardList[0]=25;break;
			case 0x2A:iCardList[0]=26;break;
			case 0x2B:iCardList[0]=27;break;
			case 0x2C:iCardList[0]=28;break;
			case 0x2D:iCardList[0]=29;break;
				////黑桃 2 - A
			case 0x31:iCardList[0]=1;break;
			case 0x32:iCardList[0]=2;break;
			case 0x33:iCardList[0]=3;break;
			case 0x34:iCardList[0]=4;break;
			case 0x35:iCardList[0]=5;break;
			case 0x36:iCardList[0]=6;break;
			case 0x37:iCardList[0]=7;break;
			case 0x38:iCardList[0]=8;break;
			case 0x39:iCardList[0]=9;break;
			case 0x3A:iCardList[0]=10;break;
			case 0x3B:iCardList[0]=11;break;
			case 0x3C:iCardList[0]=12;break;
			case 0x3D:iCardList[0]=13;break;
			default:break;
			}
			pNoMoveCard->SetControlVisible(bShow) ; 
			pNoMoveCard->SetCardList(iCardList , cardCount) ; 
		}
		else
		{
			;
		}
		
	
	return ; 
}
//申请上庄结果处理
void CNewGameClient::HandleApplyResult(void *buffer)
{
	ApplyNtResultMsg *pApply = (ApplyNtResultMsg *)buffer;
	if(pApply->bApplay)
	{
		//显示上庄金币不足的提醒图片

		//m_pGameFrame->SetTimer(TIME_SHOW_MONEY,1.5*1000);
		return;
	}
	m_byGameStation=pApply->bGameStation;
	m_byNtStation=pApply->byNtStation;		//庄家位置
	m_iNtPlayCount=pApply->byNtcount;//庄家坐庄次数
	m_iPlayJuShu=pApply->bEvent;
	//庄家的成绩
	m_i64NtScore=pApply->i64NtScore;
	ShowNtInfo(true);

	int iNtCount=0;//统计上庄列表的人数
	memset(m_byWaitNtList,255,sizeof(m_byWaitNtList));
	//复制上庄列表
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(255==pApply->byWaitNtList[i])
		{
			continue;
		}

		m_byWaitNtList[i]=pApply->byWaitNtList[i];
		iNtCount++;
	}



	BOOL flag=false;//是否在上庄列表中的标志位
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(m_iMyDeskStation==m_byWaitNtList[i])//自己在庄家列表中
		{
			flag=true;

			break;
		}
	}
	if(flag)//在庄家列表里能下注
	{
		//控制上庄下庄按钮的使用和显示
		BtNtDisp(1);
		//控制能否下注
		BtEnable(true);
		//哪些下注按钮可用
		SetBetBtEnable(true,0);
		m_byNtWaitListCounter=0;
	}
	else
	{
		if((m_byNtStation!=255)&&(m_byNtStation==m_iMyDeskStation))//自己是庄家不能下注
		{
			if(GS_FREE_STATUS==m_byGameStation)
			{
				//控制上庄下庄按钮的使用和显示
				BtNtDisp(1);
			}
			else
			{
				//控制上庄下庄按钮的使用和显示
				BtNtDisp(2);
			}

			//控制能否下注
			BtEnable(false);
			//哪些下注按钮可用
			SetBetBtEnable(false,1);
			m_byNtWaitListCounter=0;
		}
		else
		{
			if(iNtCount>=m_byMaxNtPeople)
			{
				//控制上庄下庄按钮的使用和显示
				BtNtDisp(2);
			}
			else
			{
				//检查是否达到上庄金币
				UserInfoForGame UserInfo ; 
				memset(&UserInfo,0,sizeof(UserInfo));
				//自己金币
				if (m_pGameFrame->GetUserInfo(m_iMyDeskStation , UserInfo))
				{
					if(UserInfo.i64Money>=m_i64ApplyNtLimite)
					{
						//控制上庄下庄按钮的使用和显示
						BtNtDisp(0);
					}
					else
					{
						//控制上庄下庄按钮的使用和显示
						BtNtDisp(2);
					}
				}
			}

			//控制能否下注
			BtEnable(true);
			//哪些下注按钮可用
			SetBetBtEnable(true,0);
		}

	}

	if((GS_FREE_STATUS==m_byGameStation) || (GS_SEND_STATUS==m_byGameStation))//不能下注
	{
		//控制能否下注
		BtEnable(false);
		//哪些下注按钮可用
		SetBetBtEnable(false,1);
	}
	if((GS_BET_STATUS==m_byGameStation) || (GS_SEND_STATUS==m_byGameStation))
	{
		if(pApply->bNtFlag)//杀掉所有定时器
		{
			KillAllGameTime();
			//m_pGameFrame->KillTimer(TIME_AWARDSHOW);//删除胜利框显示定时器
			////胜利框不显示
			//for(int i=0;i<32;i++)
			//{
			//	SetImageVisible(CTN_1000000_CTN_10001478_IMG_10001479+i,false);
			//}
			////胜利框不显示
			//SetImageLoadSrc(CTN_1000000_CTN_10001432_IMG_10001465,"",FALSE,1);
		}

	}
	//显示庄家列表
	ShowNtWaitList(true);
	//自己的局数
	SetTextinfo(BTN_PLAY_ROUND,m_iPlayJuShu,TRUE);
}
//显示上庄列表
void CNewGameClient::ShowNtWaitList(bool bFalg)
{

	BYTE byTemp=0;
	for(int i=m_byNtWaitListCounter;i<m_byNtWaitListCounter+5;i++)
	{
		if(255!=m_byWaitNtList[i])
		{
			UserInfoForGame UserInfo ;
			if (m_pGameFrame->GetUserInfo(m_byWaitNtList[i], UserInfo))
			{
				///昵称
				SetChinfo(BTN_SHANGZHUANG_LIST_1+byTemp,UserInfo.szNickName,bFalg);

			}
		}
		else
		{
			//昵称
			SetChinfo(BTN_SHANGZHUANG_LIST_1+byTemp,"",false);
			
		}
		byTemp++;

	}


}
//-------------------------------------------------------------------------
//设置哪些下注筹码可用
void	CNewGameClient::SetBetBtEnable(bool bFalg,BYTE jud)
{
	if(0==jud)
	{
		UserInfoForGame UserInfo ; 
		if (m_pGameFrame->GetUserInfo(m_iMyDeskStation , UserInfo))
		{
			__int64 i64Money = UserInfo.i64Money;
			for(int i=0;i<6;i++)
			{

				if (i64Money > G_iChouMaValue[i])
				{
					SetBtEnable(CTN_1000000_CTN_10001332_BTN_10001182+i,bFalg);
				}
				else
				{
					SetBtEnable(CTN_1000000_CTN_10001332_BTN_10001182+i,!bFalg);
				}
			}

		}
	}
	if(1==jud)
	{
		for(int i=0;i<6;i++)
		{
			SetBtEnable(CTN_1000000_CTN_10001332_BTN_10001182+i,bFalg);
		}
	}
}
//控制下注区域点击是否生效
void CNewGameClient::BtEnable(BOOL bflag)
{
	SetBtEnable(CTN_1000000_CTN_10001369_BTN_10001321, bflag);
	SetBtEnable(CTN_1000000_CTN_10001370_BTN_10001322, bflag);
	SetBtEnable(CTN_1000000_CTN_10001371_BTN_10001325, bflag);
	
}
//控制上庄下庄按钮的使用和显示
void CNewGameClient::BtNtDisp(BYTE flag)
{
	if(0==flag)//上庄按钮可用
	{
		SetBtEnable(CTN_1000000_BTN_10001365,true);
		SetBtVisible(CTN_1000000_BTN_10001365, true);
		SetBtEnable(CTN_1000000_BTN_10001171,false);
		SetBtVisible(CTN_1000000_BTN_10001171, false);
	}
	else if(1==flag)//下庄按钮可用
	{
		SetBtEnable(CTN_1000000_BTN_10001365,false);
		SetBtVisible(CTN_1000000_BTN_10001365, false);
		SetBtEnable(CTN_1000000_BTN_10001171,true);
		SetBtVisible(CTN_1000000_BTN_10001171, true);
	}
	else if(2==flag)//都不可用
	{
		SetBtEnable(CTN_1000000_BTN_10001365,false);
		SetBtVisible(CTN_1000000_BTN_10001365,TRUE);
		SetBtEnable(CTN_1000000_BTN_10001171,false);
		SetBtVisible(CTN_1000000_BTN_10001171, false);
	}
}
//-------------------------------------------------------------------------
//空闲消息
void	CNewGameClient::HandleGameFree(void * pBuffer)
{
	
	BeginFreeMsg *pGameFree = (BeginFreeMsg *)pBuffer;
	if(NULL == pGameFree)
	{
		return ;
	}
	//清空游戏临时数据
	ResetGameData();
	
	//显示阶段图片
	ShowSection(0);
	//游戏重新开始了删除所有定时器
	KillAllGameTime();
	//控制能否下注
	BtEnable(false);
	//空闲状态下注按钮不可用
	SetBetBtEnable(FALSE,1);
	
	if(m_byNtStation!=pGameFree->byNtStation)//换庄了
	{
		m_byNtWaitListCounter=0;
		//庄家的成绩清零
		m_i64NtScore=0;
		//m_pGameFrame->SetTimer(TIME_SHOW_HUANZHUANG,3*1000);
	}
	 m_byGameStation =pGameFree->byGameStation;         ///当前的游戏状态
	 m_byNtStation = pGameFree->byNtStation;
	 m_iNtPlayCount= pGameFree->byNtcount;
	 m_i64NtScore= pGameFree->i64NtScore;

	 
	 /*CString cs;
	 cs.Format("test  坐庄次数=%d",m_iNtPlayCount);
	 OutputDebugString(cs);*/
	//显示庄家信息
	ShowNtInfo(true);

	int iNtCount=0;
	memset(m_byWaitNtList,255,sizeof(m_byWaitNtList));
	 //复制上庄列表
	 for(int i=0;i<PLAY_COUNT;i++)
	 {
		 if(255==pGameFree->byWaitNtList[i])
		 {
			 continue;
		 }
		
		 m_byWaitNtList[i]=pGameFree->byWaitNtList[i];
		 iNtCount++;
	 }
	 //显示庄家列表
	 ShowNtWaitList(true);
	 
	 BOOL flag=false;//是否在上庄列表中的标志位
	 for(int i=0;i<PLAY_COUNT;i++)
	 {
		 if(m_iMyDeskStation==m_byWaitNtList[i])//自己在庄家列表中
		 {
			 flag=true;

			 break;
		 }
	 }
	 if(flag)
	 {
		BtNtDisp(1);
	 }
	 else
	 {
		 if(m_byNtStation==m_iMyDeskStation)//自己是庄家
		 {
			 
			 if(GS_FREE_STATUS==m_byGameStation)
			 {
				 
				 //控制上庄下庄按钮的使用和显示
				 BtNtDisp(1);
			 }
			 else
			 {
				 //控制上庄下庄按钮的使用和显示
				 BtNtDisp(2);
			 }
		 }
		 else
		 {
			 if(iNtCount>=m_byMaxNtPeople)
			 {
				 //控制上庄下庄按钮的使用和显示
				 BtNtDisp(2);
			 }
			 else
			 {
				 //检查是否达到上庄金币
				 UserInfoForGame UserInfo ; 
				 memset(&UserInfo,0,sizeof(UserInfo));
				 //自己金币
				 if (m_pGameFrame->GetUserInfo(m_iMyDeskStation , UserInfo))
				 {
					 if(UserInfo.i64Money>=m_i64ApplyNtLimite)
					 {
						 //控制上庄下庄按钮的使用和显示
						 BtNtDisp(0);
					 }
					 else
					 {
						 //控制上庄下庄按钮的使用和显示
						 BtNtDisp(2);
					 }
				 }
			 }
			 
		 }
		 
	 }
	m_iClockLable=m_byFreeTime;                     //时钟标签
	//设置时钟倒计时
	//SetTimePlay(CTN_1000000_Time_10001180,true,m_byFreeTime);
	SetInumPro(CTN_1000000_CTN_10001408_Num_10001409,m_byFreeTime,2,true);
	//设置时钟倒计时
	m_pGameFrame->SetTimer(TIME_ClOCK,1000);
}
//显示阶段图片
void CNewGameClient::ShowSection(BYTE bSection)
{
	switch(bSection)
	{
	case 0:
		{
			SetImageVisible(CTN_1000000_IMG_10001174,true);
			SetImageVisible(CTN_1000000_IMG_10001175,false);
			SetImageVisible(CTN_1000000_IMG_10001176,false);
			break;
		}
	case 1:
		{
			SetImageVisible(CTN_1000000_IMG_10001175,true);
			SetImageVisible(CTN_1000000_IMG_10001174,false);
			SetImageVisible(CTN_1000000_IMG_10001176,false);
			break;
		}
	case 2:
		{
			SetImageVisible(CTN_1000000_IMG_10001176,true);
			SetImageVisible(CTN_1000000_IMG_10001175,false);
			SetImageVisible(CTN_1000000_IMG_10001174,false);
			break;
		}
	}
}
//---------------------------------------------------------------------------------------
///删除所有定时器
void CNewGameClient:: KillAllGameTime() 
{
	m_pGameFrame->KillTimer(TIME_SENDLONG_CARD) ; 

	m_pGameFrame->KillTimer(TIME_SENDHU_CARD) ; 

	m_pGameFrame->KillTimer(TIME_SHOW_LONG) ; 

	m_pGameFrame->KillTimer(TIME_SHOW_HU);

	m_pGameFrame->KillTimer(TIME_SHOW_HUANZHUANG);

	m_pGameFrame->KillTimer(TIME_SHOW_WIN_TIME);
	m_pGameFrame->KillTimer(TIME_SHOW_MONEY);
	
	m_pGameFrame->KillTimer(TIME_SHOW_JIEPAI);
	//关掉时钟
	//SetTimePlay(BTN_CLOCK,true,0);
	
	m_pGameFrame->KillTimer(TIME_ClOCK);
}

//-------------------------------------------------------------------------
//收到开始下注消息
void	CNewGameClient::HandleBeginBet(void * pBuffer)
{
	
	BeginBetMsg *pBeginBet = (BeginBetMsg *)pBuffer;
	if(NULL == pBeginBet)
	{
		return ;
	}
	//显示阶段图片
	ShowSection(1);
	//删除定时器
	m_pGameFrame->KillTimer(TIME_ClOCK);
	//m_pGameFrame->KillTimer(TIME_AWARDSHOW);//删除胜利框显示定时器
	//PlayGameSound(SOUND_GAME_J, FALSE);
	m_SoundEngineGame->stopAllSounds();
	PlayGameSound(SOUND_GAME_M,FALSE); 
	PlayGameSound(SOUND_GAME_J,FALSE); 
	//胜利框不显示
	//SetImageLoadSrc(CTN_1000000_CTN_10001432_IMG_10001465,"",FALSE,1);
	//胜利框不显示
	
		//SetImageVisible(CTN_1000000_CTN_10001478_IMG_10001479+i,false);
	
	//m_byWinPrize=-1;//开奖结果下注阶段清除
	m_byGameStation=pBeginBet->byGameStation;//游戏状态
	m_byNtStation	= pBeginBet->byNtStation;		//庄家位置
	m_iPlayJuShu    =pBeginBet->byEvent;//游戏局数
	m_iNtPlayCount= pBeginBet->byNtcount;
	m_i64NtScore= pBeginBet->i64NtScore;
	//显示庄家信息
	ShowNtInfo(true);
	if(pBeginBet->bLuziClearFlag)
	{
		HideLuzi(); 
		ClearAccountResult();
	}
	//有庄家才算一把（更新把数和局数）
	if(255!=m_byNtStation)
	{
		m_iPlayBaShu++;
		//自己的把数
		SetTextinfo(BTN_PLAY_BASHU,m_iPlayBaShu,TRUE);
		//局数
		SetTextinfo(BTN_PLAY_ROUND,m_iPlayJuShu,TRUE);
	}
	//记住开奖起始位置
	//m_byWinPrizeStart= pBeginBet->byWinPrizeStart;
	////复制牌数据
	//for(int i=0;i<SEND_COUNT;i++)
	//{
	//	m_byCardStyle[i]=pBeginBet->byCardStyle[i];
	//}
	int iNtCount=0;
	memset(m_byWaitNtList,255,sizeof(m_byWaitNtList));
	//复制上庄列表
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(255==pBeginBet->byNtWaitList[i])
		{
			continue;
		}

		m_byWaitNtList[i]=pBeginBet->byNtWaitList[i];
		iNtCount++;
	}
	//显示庄家列表
	ShowNtWaitList(true);

	BOOL flag=false;//是否在上庄列表中的标志位
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(m_iMyDeskStation==m_byWaitNtList[i])//自己在庄家列表中
		{
			flag=true;

			break;
		}
	}

	if(flag)//在庄家列表里能下注
	{
		//控制上庄下庄按钮的使用和显示
		BtNtDisp(1);
		//控制能否下注
		BtEnable(true);
		//显示下注按钮
		SetBetBtEnable(true,0);
	}
	else
	{
		if((m_byNtStation!=255)&&(m_byNtStation==m_iMyDeskStation))//自己是庄家不能下注
		{
			if(GS_FREE_STATUS==m_byGameStation)
			{
				//控制上庄下庄按钮的使用和显示
				BtNtDisp(1);
			}
			else
			{
				//控制上庄下庄按钮的使用和显示
				BtNtDisp(2);
			}
			//控制能否下注
			BtEnable(false);
			//显示下注按钮
			SetBetBtEnable(false,1);
		}
		else
		{
			if(iNtCount>=m_byMaxNtPeople)
			{
				//控制上庄下庄按钮的使用和显示
				BtNtDisp(2);
			}
			else
			{
				//检查是否达到上庄金币
				UserInfoForGame UserInfo ; 
				memset(&UserInfo,0,sizeof(UserInfo));
				//自己金币
				if (m_pGameFrame->GetUserInfo(m_iMyDeskStation , UserInfo))
				{
					if(UserInfo.i64Money>=m_i64ApplyNtLimite)
					{
						//控制上庄下庄按钮的使用和显示
						BtNtDisp(0);
					}
					else
					{
						//控制上庄下庄按钮的使用和显示
						BtNtDisp(2);
					}
				}
			}

			//控制能否下注
			BtEnable(true);
			//显示下注按钮
			SetBetBtEnable(true,0);
		}

	}
	if(255==m_byNtStation)//没有庄家不能下注
	{
		//控制能否下注
		BtEnable(false);
		//显示下注按钮
		SetBetBtEnable(false,1);
	}

	//设置时钟倒计时
	//SetTimePlay(CTN_1000000_Time_10001180,true,m_byBetTime);
	m_iClockLable=m_byBetTime;                     //时钟标签
	SetInumPro(CTN_1000000_CTN_10001408_Num_10001409,m_byBetTime,2,true); 
	//设置时钟倒计时
	m_pGameFrame->SetTimer(TIME_ClOCK,1000);


}


//-------------------------------------------------------------------------
///显示结算框
void	CNewGameClient::ShowGameResult(void *buffer) 
{
	BeginShowResult *BeginJieSuan= (BeginShowResult *)buffer;

	if(NULL == BeginJieSuan)
	{
		return ;
	}

	//
	//显示结算框
	SetContainVisible(CTN_1000000_CTN_10001280,true);
	if(m_iMyDeskStation==m_byNtStation)//自己是庄家
	{
		
		//得分
		SetInumPro(CTN_1000000_CTN_10001280_Num_10001386,0,0,true);
		SetInumPro(CTN_1000000_CTN_10001280_Num_10001387,0,0,true);
		SetInumPro(CTN_1000000_CTN_10001280_Num_10001388,i64ChangeMoney[m_byNtStation],0,true);

		//成绩
		if(255!=m_byNtStation)
		{
			SetTextinfo(CTN_1000000_CTN_10001360_TEXT_10001212,i64PlayerGrade[m_byNtStation],true);
		}
		SetTextinfo(CTN_1000000_CTN_10001362_TEXT_10001296,i64PlayerGrade[m_iMyDeskStation],true);

		if(i64ChangeMoney[m_iMyDeskStation]>0)
		{
			PlayGameSound(SOUND_GAME_O, FALSE);
		}
		else
		{
			PlayGameSound(SOUND_GAME_G, FALSE);
		}
	}
	else
	{
		
		SetInumPro(CTN_1000000_CTN_10001280_Num_10001386,i64ChangeMoney[m_iMyDeskStation],0,true);
		SetInumPro(CTN_1000000_CTN_10001280_Num_10001387,i64ReturnMoney[m_iMyDeskStation],0,true);
		SetInumPro(CTN_1000000_CTN_10001280_Num_10001388,i64ChangeMoney[m_byNtStation],0,true);

		
		//成绩
		if(255!=m_byNtStation)
		{
			SetTextinfo(CTN_1000000_CTN_10001360_TEXT_10001212,i64PlayerGrade[m_byNtStation],true);
		}
		SetTextinfo(CTN_1000000_CTN_10001362_TEXT_10001296,i64PlayerGrade[m_iMyDeskStation],true);
		if(i64ChangeMoney[m_iMyDeskStation]>0)
		{
			PlayGameSound(SOUND_GAME_O, FALSE);
		}
		else
		{
			PlayGameSound(SOUND_GAME_G, FALSE);
		}
		
	}

	//更新金币

	//闲家金币
	UserInfoForGame UserInfo ; 
	memset(&UserInfo,0,sizeof(UserInfo));
	if (m_pGameFrame->GetUserInfo(m_iMyDeskStation , UserInfo))
	{
		/*CString cs;
		cs.Format("test 玩家金币=%d",UserInfo.i64Money);
		OutputDebugString(cs);*/
		SetTextinfo(CTN_1000000_CTN_10001362_TEXT_10001293,UserInfo.i64Money,true);
	}

	//庄家金币
	memset(&UserInfo,0,sizeof(UserInfo));
	if (m_pGameFrame->GetUserInfo(m_byNtStation , UserInfo))
	{
		/*CString cs;
		cs.Format("test 庄家金币=%d",UserInfo.i64Money);
		OutputDebugString(cs);*/
		SetTextinfo(CTN_1000000_CTN_10001360_TEXT_10001210,UserInfo.i64Money,true);
	}

	if(m_bUserExitIsClose)
	{
		TCHAR cs[100];
		_stprintf_s(cs, sizeof(cs), TEXT("本局结果:----------"));
		m_pGameFrame->InsertNormalMessage(cs);
		for(int i=0;i<PLAY_COUNT;i++)
		{
			if(i==m_byNtStation)
			{
				if(m_pGameFrame->GetUserInfo(m_byNtStation, UserInfo))
				{

					TCHAR cs[100];
					_stprintf_s(cs, sizeof(cs), TEXT("%s(庄家)得分:%I64d"),UserInfo.szNickName, i64ChangeMoney[m_byNtStation]);
					m_pGameFrame->InsertNormalMessage(cs);

				}

			}
			else
			{
				if(m_pGameFrame->GetUserInfo(i, UserInfo))
				{

					TCHAR cs[100];
					_stprintf_s(cs, sizeof(cs), TEXT("%s(闲家)得分:%I64d"),UserInfo.szNickName, i64ChangeMoney[i]);
					m_pGameFrame->InsertNormalMessage(cs);

				}

			}
		}
		_stprintf_s(cs, sizeof(cs), TEXT("------------------"));
		m_pGameFrame->InsertNormalMessage(cs);
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
	/*if(!m_bSoundPlay)
	{
		return ;
	}*/
	if(!m_bSoundIsClose)
	{
		return;
	}
	CString  folder ;
	CString strPath = CINIFile::GetAppPath() ;
	char szFileName[MAX_PATH];
	memset(szFileName,0,sizeof(szFileName)) ; 

	switch(SoundID)
	{
	case SOUND_GAME_A:	//下注的声音
		{
			folder.Format("music/addchip.ogg");
			break;
		}
	case SOUND_GAME_B:	//图片跳动的声音
		{
			//folder.Format("Music/Sound/Animal1.wav");
			break;
		}
	case SOUND_GAME_C:	//开始开牌
		{
			folder.Format("music/ReadyStart.ogg");
			break;
		}
	case SOUND_GAME_D:	//超500w喝彩声
		{
			folder.Format("music/chao500w.wav");
			break;
		}
	case SOUND_GAME_E:	//玩家进入的时候播放的声音
		{
			folder.Format("music/begingame.wav");
			break;
		}
	case SOUND_GAME_F:	//游戏开始的声音
		{
			folder.Format("music/gamebegin.wav");
			break;
		}
	case SOUND_GAME_G:	//输的声音
		{
			folder.Format("music/lost.wav");
			break;
		}
	case SOUND_GAME_H:	//游戏结束
		{
			//folder.Format("Music/Sound/Over.ogg");
			break;
		}
	case SOUND_GAME_I:	//发牌
		{
			folder.Format("music/sendcard.wav");
			break;
		}
	case SOUND_GAME_J:		//开始下注
		{
			folder.Format("music/ReadyXiaZhu.ogg");
			break;
		}
	case SOUND_GAME_K:	//100w的声音
		{
			folder.Format("music/sound100w.wav");
			break;
		}
	case SOUND_GAME_L://500w的声音
		{
			folder.Format("music/sound500w.wav");
			break;
		}
	case SOUND_GAME_M:	//舞动的声音
		{
			folder.Format("music/Start.ogg");
			break;
		}
	case SOUND_GAME_N:	//即将到时的声音
		{
			folder.Format("music/timeout.ogg");
			break;
		}
	case SOUND_GAME_O:	//赢的声音
		{
			folder.Format("music/win.wav");
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
//--------------------------------------------------------------------------------------------------------
//设置数字空间显示内容
void	CNewGameClient::SetInumPro(const int iNumID, __int64 i64Num,BYTE bPro,bool bFlag)
{
	INum *pNum = NULL;
	pNum = dynamic_cast<INum *>(m_pUI->GetIControlbyID(iNumID));
	if (NULL != pNum)
	{
		switch(bPro)
		{
		case 0:
			{
				NEW_NUM_STRUCT NumStruct;

				NumStruct.bShowPlusSignOrMinusSign = true;
				NumStruct.bShowComma = true;
				NumStruct.iShowCommaIndex = 3;
				NumStruct.iShowZeroType = 0;
				pNum->SetNewNumStruct(NumStruct);
				pNum->SetNewNum(i64Num);
				pNum->SetControlVisible(bFlag);
				break;
			}
		case 1:
			{
				NEW_NUM_STRUCT NumStruct;

				NumStruct.bShowPlusSignOrMinusSign = false;
				NumStruct.bShowComma = true;
				NumStruct.iShowCommaIndex = 3;
				NumStruct.iShowZeroType = 0;
				pNum->SetNewNumStruct(NumStruct);
				pNum->SetNewNum(i64Num);
				pNum->SetControlVisible(bFlag);
				break;
			}
		case 2:
			{
				pNum->SetNum(i64Num);
				pNum->SetControlVisible(bFlag);
				break;
			}
		}

	}
}
//--------------------------------------------------------------------------------------------------------
//设置数字空间显示内容
void	CNewGameClient::SetInumInfo(const int iNumID, __int64 i64Num, bool bFlag)
{
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
//设置数字空间显示内容(不带加号和减号及逗号)
void	CNewGameClient::SetInumInfo_1(const int iNumID, __int64 i64Num, bool bFlag)
{
	INum *pNum = NULL;
	pNum = dynamic_cast<INum *>(m_pUI->GetIControlbyID(iNumID));
	if (NULL != pNum)
	{
		pNum->SetNum(i64Num);
		pNum->SetControlVisible(bFlag);
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
//设置文本控件显示内容（显示字符）
void	CNewGameClient::SetChinfo(const int iTextID,TCHAR ch[61], bool bFlag)
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

//-----------------------------------------------------------------------------------------------
/// 设置游戏的当前状态
void CNewGameClient::SetGameStation(void * pBuffer,int nLen)
{
	m_iMyDeskStation = m_pGameFrame->GetMyDeskStation() ; 

	//m_bWatchMode = m_pGameFrame->GetIsWatching() ;

	switch(GetStationParameter())
	{
	case GS_WAIT_SETGAME:		//游戏没有开始状态
	case GS_WAIT_ARGEE:			//等待玩家开始状态
		{
			if (nLen != sizeof(GameStation_Base))
			{
				return;
			}

			GameStation_Base * TGameStation = (GameStation_Base *)pBuffer;
			if(NULL == TGameStation)
			{
				return;
			}
			m_byGameStation = TGameStation->bGameStation;	//游戏状态
			m_byGameStation=GS_FREE_STATUS;//置为空闲状态，可以申请上庄
			m_byFreeTime=TGameStation->byFreeTime;			//空闲时间
			m_byBetTime=TGameStation->byBetTime;			//下注时间
			m_byOpenCardTime=TGameStation->bySendTime;	    //开牌时间
			byAccountTime=TGameStation->byAccountTime;//结算时间
			m_byShowResult=TGameStation->byShowResult;//展示结果时间
			m_bySendSpaceTime=TGameStation->bySendCardSpaceTime;	//发牌间隔时间
			m_bySpeedTime=TGameStation->bySpeedTime;               //遮挡牌移动速度
			m_byShowWinTime=TGameStation->byShowWinTime;
			m_iClock=TGameStation->byRemaindTime;                     //剩余时间

			m_i64ApplyNtLimite=TGameStation->iApplyNtLimite ; //庄家上庄条件

			//庄家信息（把数和成绩）
			m_byNtStation=TGameStation->byNtStation;		//庄家位置

			m_i64NtScore=TGameStation->i64NtGrade;	//庄家成绩
			m_iNtPlayCount=TGameStation->iNtPlayCount;		//庄家坐庄次数
			m_byMaxNtPeople=TGameStation->byMaxNtPeople;		//最大上庄人数
			m_iPlayGrade=TGameStation->i64PlayerGrade;//玩家成績
			m_iPlayJuShu=TGameStation->byEvent;//游戏局数
			if(255!=m_byNtStation)
			{
				m_iPlayBaShu++;
			}
			//庄家信息清零
			ShowNtInfo(true);
			//上庄列表清零
			ShowNtWaitList(true);
			//显示自己的信息
			ShowMyInfo(true);
			//控制能否下注
			BtEnable(false);
			//下注按钮不可用
			SetBetBtEnable(FALSE,1);

			HideLuzi();
			//控制上庄下庄按钮的使用和显示
			UserInfoForGame UserInfo ; 
			memset(&UserInfo,0,sizeof(UserInfo));
			//自己金币
			if (m_pGameFrame->GetUserInfo(m_iMyDeskStation , UserInfo))
			{
				if(UserInfo.i64Money>=m_i64ApplyNtLimite)
				{
					BtNtDisp(0);
				}
				else
				{
					BtNtDisp(2);
				}
			}      

			//显示阶段图片
			ShowSection(0);
			//总下注数清零
			//SetInumPro(CTN_1000000_CTN_10001421_Num_10001429,0,2,true);
			TCHAR cs[100];
			_stprintf_s(cs, sizeof(cs), TEXT("上庄金币:%I64d"), m_i64ApplyNtLimite);
			m_pGameFrame->InsertNormalMessage(cs);
			PlayGameSound(SOUND_GAME_E,FALSE);
			//ViewLuzi();
			break; 
		}

	case GS_FREE_STATUS://空闲阶段
		{
			if (nLen != sizeof(GameStation_Base))
			{
				return;
			}
			HandleFreeStation(pBuffer);
			break;
		}
	case GS_BET_STATUS:	//游戏下注阶段
		{
			if (nLen != sizeof(GameStation_Base))
			{
				return;
			}
			HandleXiaZhuStation(pBuffer);
			break ;
		}
	case GS_SEND_STATUS://开牌阶段
		{
			if (nLen != sizeof(GameStation_Base))
			{
				return;
			}
			//游戏状态消息处理
			HandleKaiPaiStation(pBuffer);
			break;
		}
	}
}
////查看路子
//void CNewGameClient::ViewLuzi(const int iContainID)
//{
//	IContain *pContain = NULL;
//	pContain = dynamic_cast<IContain *>(m_pUI->GetIControlbyID(iContainID));
//	if (NULL != pContain)
//	{
//		pContain->SetControlVisible(TRUE);
//		IImage *image;
//		pContain->IInsertControl(image);
//
//		SetPicSize(const int iButtonID, CString sImagSrc,bool bFlag,int x,int y,int w,int h);
//	}
//}
//显示自己的信息
void CNewGameClient::ShowMyInfo(bool bFalg)
{
	//获取用户信息
	UserInfoForGame UserInfo ; 
	if (NULL==m_pGameFrame->GetUserInfo(m_iMyDeskStation , UserInfo))
	{
		return ;
	}
	
	//自己的昵称
	SetChinfo(BTN_PLAY_NAME,UserInfo.szNickName,bFalg);
	//自己的金币
	SetTextinfo(BTN_PLAY_MONEY,UserInfo.i64Money,bFalg);
	//自己的把数
	SetTextinfo(BTN_PLAY_BASHU,m_iPlayBaShu,bFalg);
	//自己的局数
	SetTextinfo(BTN_PLAY_ROUND,m_iPlayJuShu,bFalg);
	//自己的成绩
	SetTextinfo(BTN_PLAY_GRADGE,m_iPlayGrade,bFalg);
}
//-------------------------------------------------------------------------
//显示庄家信息
void	CNewGameClient::ShowNtInfo(bool bFalg)
{
	UserInfoForGame UserInfo ; 
	//判断庄家是否存在，存在就显示
	if (m_pGameFrame->GetUserInfo(m_byNtStation , UserInfo) && m_byNtStation != 255)
	{
		
		//庄家昵称
		SetChinfo(BTN_KING_NAME,UserInfo.szNickName,bFalg);
		//庄家的金币
		SetTextinfo(BTN_KING_MONEY,UserInfo.i64Money,bFalg);
		//庄家的局数
		SetTextinfo(BTN_KING_ROUND,m_iNtPlayCount,bFalg);
		//庄家的成绩
		SetTextinfo(BTN_KING_GRADGE,m_i64NtScore,bFalg);
	}
	else
	{
		//庄家昵称
		SetChinfo(BTN_KING_NAME,UserInfo.szNickName,!bFalg);
		//庄家的金币
		SetTextinfo(BTN_KING_MONEY,UserInfo.i64Money,!bFalg);
		//庄家的局数
		SetTextinfo(BTN_KING_ROUND,m_iNtPlayCount,!bFalg);
		//庄家的成绩
		SetTextinfo(BTN_KING_GRADGE,m_i64NtScore,!bFalg);
	}
}
//--------------------------------------------------------------------------------------------------------
//设置图片控件的位置
void	CNewGameClient::SetImagePosition(const int iImageID, int iPositonX, int iPositonY, bool bFlag)
{
	IImage *pImage = NULL;
	pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(iImageID));
	if (NULL != pImage)
	{
		pImage->SetGameXY(iPositonX,iPositonY);
		pImage->SetControlVisible(bFlag);
	}
}
/// 重新设置状态
int	CNewGameClient::ResetGameFrame(void)
{
	CString debg;
	debg.Format("debug-ResetGameFrame(client)\n");
	OutputDebugString(debg);
	return 0;
}
/// 玩家离开
int CNewGameClient::GameUserLeft(int nIndex)
{
	CString debg;
	debg.Format("exit-GameUserLeft(client)\n");
	OutputDebugString(debg);
	OutputDebugString("dwjlog::C_玩家离开...");
	return 0;
}
/// 玩家断线
int CNewGameClient::GameUserOffLine(void)
{
	CString debg;
	debg.Format("exit-GameUserOffLine(client)\n");
	OutputDebugString(debg);
	OutputDebugString("dwjlog::C_玩家断线...");
	return 0;
}
// 是否正在游戏，由游戏实现
// 旁观玩家返回false，游戏玩家正在游戏中返回true，其它状态返回false
bool CNewGameClient::IsPlayingGame()
{
	if (m_byGameStation > GS_FREE_STATUS && m_byGameStation < GS_WAIT_NEXT)
	{
		OutputDebugString("dwjlog::C_正在游戏中...");
		return true;
	}
	
	return false;
}

///玩家退出游戏
bool CNewGameClient::OnGameQuiting()
{

	CString debg ;
	debg.Format("exit-OnGameQuiting(Client)(debug)") ; 
	OutputDebugString(debg);
	if(m_pGameFrame->GetIsWatching())
	{
		OutputDebugString("exit-OnGameQuiting(Client)====0");
		return true ; 
	}

	CString szAppPath = CINIFile::GetAppPath();
	szAppPath.SetAt(szAppPath.GetLength()-1, TCHAR('\0'));
	int nPosOfSlash = szAppPath.ReverseFind(TCHAR('\\'));
	CString szAppPathParent = szAppPath.Left(nPosOfSlash);


	CString strKeyName  ; 
	strKeyName.Format("%d" ,NAME_ID) ; 
	CBcfFile File(szAppPathParent + "\\SpecialRule.bcf") ; 

	bool   bForceQuitAsAuto = File.GetKeyVal("ForceQuitAsAuto",strKeyName ,1) ; 

	if(m_byGameStation > GS_FREE_STATUS && m_byGameStation < GS_WAIT_NEXT)
	{
		OutputDebugString("exit-OnGameQuiting========1(Client)");
		if(bForceQuitAsAuto)
		{
			OutputDebugString("exit-OnGameQuiting========2(Client)");
			if(IDOK != AFCMessageBox("您正在游戏中, 现在离开游戏将会托管处理 ，是否要离开？", "提示",AFC_YESNO)) 
			{
				return false ; 
			}	
		}
		else
		{
			OutputDebugString("exit-OnGameQuiting========3(Client)");
			if(IDOK != AFCMessageBox("您正在游戏中, 现在离开将会被扣罚，是否真的要离开？", "提示",AFC_YESNO)) 
			{
				return false ; 
			}
			m_pGameFrame->SendGameData(NULL , 0 ,MDM_GM_GAME_FRAME,ASS_GM_FORCE_QUIT,0);
		}
	}
	
	
	return true;
}
/// 定时器消息
void CNewGameClient::OnTimer(int nTimerID)
{
	switch(nTimerID)
	{
		case TIME_ClOCK:
		{
			m_iClockLable--;
			SetInumPro(CTN_1000000_CTN_10001408_Num_10001409,m_iClockLable,2,true);
			if(m_iClockLable<=5)
			{
				PlayGameSound(SOUND_GAME_N,FALSE);  
			}
			if(m_iClockLable<=0)
			{
				m_pGameFrame->KillTimer(TIME_ClOCK);
			}
			break;
		}
		case TIME_SENDLONG_CARD://开始发龙牌
		{
			int x=0,y=0;
			//删除定时器
			m_pGameFrame->KillTimer(TIME_SENDLONG_CARD);
			//龙位置发牌
			ShowUserSendCard(CTN_1000000_NoMoveCard_101317,m_byCardStyle[0],1,true);

			IImage *pImage = NULL;
			pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(CTN_1000000_IMG_10001379));
			if (NULL != pImage)
			{
						
				x=pImage->GetGameX();
				y=pImage->GetGameY();
				SetImagePosition(CTN_1000000_IMG_10001330,x,y, true);
			}
					
			//显示龙牌遮挡牌
			SetImageVisible(CTN_1000000_IMG_10001330,true);
			//启动发虎牌定时器
			m_pGameFrame->SetTimer(TIME_SENDHU_CARD,m_bySendSpaceTime*1000);
			break;
		}
		case TIME_SENDHU_CARD://开始发虎牌
			{
				int x=0,y=0;
				//删除定时器
				m_pGameFrame->KillTimer(TIME_SENDHU_CARD);
				//虎位置发牌
				ShowUserSendCard(BTN_CHUPAI_HU,m_byCardStyle[1],1,true); 
				IImage *pImage = NULL;
				pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(CTN_1000000_IMG_10001378));
				if (NULL != pImage)
				{
					x=pImage->GetGameX();
					y=pImage->GetGameY();
					SetImagePosition(CTN_1000000_IMG_10001331,x,y, true);
				}
				SetImageVisible(CTN_1000000_IMG_10001331,true);
				//开始揭牌
				m_pGameFrame->SetTimer(TIME_SHOW_JIEPAI,2*1000);
					
				break;
			}
		case TIME_SHOW_JIEPAI:
			{
				m_pGameFrame->KillTimer(TIME_SHOW_JIEPAI);
				PlayGameSound(SOUND_GAME_F, FALSE);
				//启动龙牌遮挡牌移动定时器
				m_pGameFrame->SetTimer(TIME_SHOW_LONG,m_bySpeedTime);
				break;
			}
		case TIME_SHOW_LONG://龙发牌动画
			{
				int x=0,y=0,end=0;
				IImage *pImage = NULL;
				pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(CTN_1000000_IMG_10001379));
				if (NULL != pImage)
				{
					end=pImage->GetGameX();
				}
				pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(CTN_1000000_IMG_10001330));
				if (NULL != pImage)
				{
					x=pImage->GetGameX();
					y=pImage->GetGameY();
					x++;
					if(x>(end+20))
					{
						m_pGameFrame->KillTimer(TIME_SHOW_LONG);
						PlayGameSound(SOUND_GAME_F, FALSE);
						//启动虎牌遮挡牌移动定时器
						m_pGameFrame->SetTimer(TIME_SHOW_HU,m_bySpeedTime);
						return;
					}
					else
					{
						SetImagePosition(CTN_1000000_IMG_10001330,x,y, true);

					}
				}
					
				break;
					
			}
		case  TIME_SHOW_HU://虎发牌动画
			{
				int x=0,y=0,end=0;
				IImage *pImage = NULL;
				pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(CTN_1000000_IMG_10001378));
				if (NULL != pImage)
				{
					end=pImage->GetGameX();
				}
				pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(CTN_1000000_IMG_10001331));
				if (NULL != pImage)
				{
					x=pImage->GetGameX();
					y=pImage->GetGameY();
					x++;
						
					if(x>(end+20))
					{
						//启动判断输赢画面
						m_pGameFrame->SetTimer(TIME_SHOW_WIN_TIME,m_byShowWinTime*1000);
						m_pGameFrame->KillTimer(TIME_SHOW_HU);
						return;
					}
					else
					{
						SetImagePosition(CTN_1000000_IMG_10001331,x,y, true);

					}
				}
				break;

			}
		case TIME_SHOW_WIN_TIME:
			{
				m_pGameFrame->KillTimer(TIME_SHOW_WIN_TIME);
				//m_pGameFrame->SetTimer(TIME_AWARDSHOW,500);
				if(0==m_byArea)
				{
					SetImageVisible(BTN_DRAGON_WINNER,true);
				}
				else if(1==m_byArea)
				{
					SetImageVisible(BTN_TIGER_WINNER,true);
				}
				else if(2==m_byArea)
				{
					SetImageVisible(BTN_AND_WINNER,true);
				}

				AccountResult();
				for(int i=0;i<m_dbyLuzi.size();i++)
				{

					if(0==m_dbyLuzi.at(i))
					{

						CString src(".\\resources\\image\\game_state.bmp");
						SetPicSize(BTN_LUZI_1+i,src, true,0,0,27,28);
					}
					else if(1==m_dbyLuzi.at(i))
					{
						CString src;
						src.Format(".\\resources\\image\\game_state.bmp");
						SetPicSize(BTN_LUZI_1+i,src,true,27,0,27,28);
					}
					else if(2==m_dbyLuzi.at(i))
					{

						CString src;
						src.Format(".\\resources\\image\\game_state.bmp");
						SetPicSize(BTN_LUZI_1+i,src, true,54,0,27,28);
					}
				}
				break;
			}
		case TIME_SHOW_HUANZHUANG:
			{
				m_pGameFrame->KillTimer(TIME_SHOW_HUANZHUANG);
				SetImageVisible(CTN_1000000_IMG_10001398,false);
				break;
			}
		case TIME_SHOW_MONEY:
			{
				SetImageVisible(CTN_1000000_CTN_10001361_IMG_10001399,false);
				m_pGameFrame->KillTimer(TIME_SHOW_MONEY);
				break;
			}
		case TIME_AWARDSHOW:
			{
				ShowAwardFrame();
				break;
			}
	}
}
//清除龙虎和的个数
void CNewGameClient::ClearAccountResult()
{
	SetTextinfo(CTN_1000000_TEXT_10001405,0, true);
	SetTextinfo(CTN_1000000_TEXT_10001406,0, true);
	SetTextinfo(CTN_1000000_TEXT_10001407,0, true);
}
//计算龙虎和的个数
void CNewGameClient::AccountResult()
{
	int _long=0,_hu=0,_he=0;
	//计算72路子中龙、虎、和的个数
	for(int i=0;i<m_dbyLuzi.size();i++)
	{
		if(0==m_dbyLuzi.at(i))
		{
			_long++;
		}
		else if(1==m_dbyLuzi.at(i))
		{
			_hu++;
		}
		else if(2==m_dbyLuzi.at(i))
		{
			_he++;
		}
	}
	SetTextinfo(CTN_1000000_TEXT_10001405,_long, true);
	SetTextinfo(CTN_1000000_TEXT_10001406,_hu, true);
	SetTextinfo(CTN_1000000_TEXT_10001407,_he, true);
}
//显示中奖框
void CNewGameClient::ShowAwardFrame()
{

	BYTE flag=-1;
	//选择显示区域
	switch(m_byArea)
	{
	case 0:
		{
			if(m_bAwardDisplayFlag)
			{
				m_bAwardDisplayFlag=false;
				CString src;
				src.Format("./image/ractangleBigEnd.png");
				//SetImageVisible(CTN_1000000_CTN_10001478_IMG_10001479+m_byWinPrize,true);
				//SetImageLoadSrc(CTN_1000000_CTN_10001432_IMG_10001465,src,TRUE,0);
			}
			else
			{

				m_bAwardDisplayFlag=true;
				CString src;
				src.Format("./image/ractangleBig.png");
				//SetImageVisible(CTN_1000000_CTN_10001478_IMG_10001479+m_byWinPrize,FALSE);
				//SetImageLoadSrc(CTN_1000000_CTN_10001432_IMG_10001465,src,TRUE,0);
			}
			break;
		}
	
	case 1:
		{
			if(m_bAwardDisplayFlag)
			{
				m_bAwardDisplayFlag=false;
				CString src;
				src.Format("./image/ractangleBigEnd.png");
				//SetImageVisible(CTN_1000000_CTN_10001478_IMG_10001479+m_byWinPrize,true);
				//SetImageLoadSrc(CTN_1000000_CTN_10001432_IMG_10001469,src,TRUE,0);
			}
			else
			{
				m_bAwardDisplayFlag=true;
				CString src;
				src.Format("./image/ractangleBig.png");
				//SetImageVisible(CTN_1000000_CTN_10001478_IMG_10001479+m_byWinPrize,FALSE);
				//SetImageLoadSrc(CTN_1000000_CTN_10001432_IMG_10001469,src,TRUE,0);
			}
			break;
		}
	
	case 2:
		{
			if(m_bAwardDisplayFlag)
			{
				m_bAwardDisplayFlag=false;
				CString src;
				src.Format("./image/ractangleBigEnd.png");
				//SetImageVisible(CTN_1000000_CTN_10001478_IMG_10001479+m_byWinPrize,true);
				//SetImageLoadSrc(CTN_1000000_CTN_10001432_IMG_10001466,src,TRUE,0);
			}
			else
			{
				m_bAwardDisplayFlag=true;
				CString src;
				src.Format("./image/ractangleBig.png");
				//SetImageVisible(CTN_1000000_CTN_10001478_IMG_10001479+m_byWinPrize,FALSE);
				//SetImageLoadSrc(CTN_1000000_CTN_10001432_IMG_10001466,src,TRUE,0);
			}
			break;
		}
	
	}
}
///初始化部分数据
void CNewGameClient::ResetGameData()
{
	//清空结算数据
	memset(i64PlayerGrade,0,sizeof(i64PlayerGrade));
	memset(i64ChangeMoney,0,sizeof(i64ChangeMoney));
	memset(i64ReturnMoney,0,sizeof(i64ReturnMoney));

	memset(m_i64AreaAllSum,0x00,sizeof(m_i64AreaAllSum));					//清除区域总值
	memset(m_i64RealityAreaAllSum,0x00,sizeof(m_i64RealityAreaAllSum));		//清除真人下注总值

	m_bAwardDisplayFlag=TRUE;//开奖位置显示标签

	//隐藏结算框
	SetContainVisible(CTN_1000000_CTN_10001280,FALSE);

	INoMoveCard  *pNoMoveCard = NULL ; 
	pNoMoveCard = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(CTN_1000000_NoMoveCard_101317))  ; 
	if(NULL != pNoMoveCard)
	{
		//清牌
		pNoMoveCard->SetControlVisible(FALSE) ; 
	}
	pNoMoveCard = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(BTN_CHUPAI_HU))  ; 

	if(NULL != pNoMoveCard)
	{
		//清牌
		pNoMoveCard->SetControlVisible(FALSE) ;
	}
	//遮挡牌不可见
	SetImageVisible(CTN_1000000_IMG_10001330,FALSE);
	SetImageVisible(CTN_1000000_IMG_10001331,FALSE);

	//龙赢...隐藏
	SetImageVisible(BTN_DRAGON_WINNER,FALSE);
	SetImageVisible(BTN_TIGER_WINNER,FALSE);
	SetImageVisible(BTN_AND_WINNER,FALSE);


	m_i64PlayerCurrentMoney=0;//玩家当前金币

	//清除桌子上的筹码和下注数的显示
	//对应区域显示筹码值
	for(int i=0;i<3;i++)
	{
		SetShCtrlInfo(BTN_BET_LEFT+i,0, FALSE);
	}
	//区域自己下注
	SetInumPro(CTN_1000000_CTN_10001369_Num_10001393,0,2,FALSE);
	SetInumPro(CTN_1000000_CTN_10001370_Num_10001395,0,2,FALSE);
	SetInumPro(CTN_1000000_CTN_10001371_Num_10001397,0,2,FALSE);
	//区域总下注
	SetInumPro(CTN_1000000_CTN_10001369_Num_10001392,0,2,FALSE);
	SetInumPro(CTN_1000000_CTN_10001370_Num_10001394,0,2,FALSE);
	SetInumPro(CTN_1000000_CTN_10001371_Num_10001396,0,2,FALSE);
	
	m_SoundEngineGame->stopAllSounds();


}
