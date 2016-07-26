#include "stdafx.h"
#include "NewGameClient.h"
#include "UI_ControlsDefine.h"
#include "UI_ForLogic/IUserInterface.h"
#include "UI_ForLogic/GameIDDefines.h"
#include "Common/UIMessageDefines.h"
#include "LoveSendClassInExe.h"


IGameImpl *CreateNewGameImpl(IGameFrame* pIBase, IUserInterface *pIUI)
{
	return dynamic_cast<IGameImpl*>(new CNewGameClient(pIBase, pIUI));
}
void DestroyNewGameImpl(IGameImpl **ppImp)
{
	delete *ppImp;
	*ppImp = NULL;
}

/**
* 依当前金币倍率得到字符串
* @param [int] int nNum 要显示的数字
* @param [out] CString &Str 返回的字符串
* @param [int] int nPower 金币倍率
* @param [in]  BOOL bIsSpace TRUE: 用空格分隔 FALSE: 不分隔
* @return void
*/
void GlbGetFloatString( char  Str[256], __int64 nNum, int nPower=0, BOOL bIsSpace=TRUE)
{
	if (0 == nNum)
	{
		wsprintf(Str,"0");
		return;
	}

	// 暂存结果
	TCHAR strTmp[32];
	TCHAR str[50];

	// 目前只处理大于0的情况，在正常字符串后面要加至少一个0
	wsprintf(strTmp, TEXT("%I64d"), nNum);

	for (int i = 0; i < nPower; ++i)
	{
		_tcscat(strTmp, TEXT("0"));
	}

	// 按位数每4位用逗号格开显示
	int nLen = _tcslen(strTmp);	// 字符串长度
	int nTimes = nLen % 4;	    // 这个字符串可以分成的节数，至少是1节

	int nSrc = 0;
	int nDes = 0;

	for (int i = 0; i < nLen; ++i)
	{
		if ((i > 0) && (i % 4 == nTimes) && bIsSpace && (str[i-1] != '-'))
		{
			str[nDes++] = TEXT(',');
		}

		str[nDes++] = strTmp[nSrc++];
	}

	str[nDes] = TEXT('\0');
	wsprintf(Str,"%s",str);
}

CNewGameClient::CNewGameClient(IGameFrame *pGameFrame, IUserInterface *pUI)
{
	//设置UI
	m_pIUI = pUI;
	m_MyUI.SetpIUI(pUI,NAME_ID);
	m_bIsWatching = false;
	m_pGameFrame = pGameFrame;
	m_bGameStation = GS_WAIT_SETGAME;           //游戏状态
	m_iBeginTime = 0;			//游戏开始时间
	m_iThinkTime = 0;			//游戏思考时间
	m_bTimeOutCount = 0;
	m_bSound = TRUE;
	m_SoundEngine = NULL;
	m_bCanMove = false;

	::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));			//桌面扑克的数目
	m_SoundEngine = createIrrKlangDevice();
	m_bAutoStart=false;
	m_i64HalfLessMoney=0;
	InitGameData();
	m_bNewTurn=false;
	memset(m_iTempBt,0,sizeof(m_iTempBt));
}

CNewGameClient::~CNewGameClient()
{

}

//加载配制文件
void CNewGameClient::LoadData(void)
{
	CString s = CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f(s + GET_SKIN_FOLDER(szTempStr) + TEXT(".bcf"));
	CString key = TEXT("Operate");
	m_bSound = f.GetKeyVal(key,TEXT("Sound"),TRUE);
	m_MyUI.SetIsSelect(CTN_1000000_CTN_1003510_RButton_1003520, m_bSound);
}
/// 初始化
int CNewGameClient::Initial()
{
	// 载入UIsdp
	LoadData();

	TCHAR szDataFileName[MAX_PATH]={0};
	
	sprintf(szDataFileName, "UI_%d.dat",NAME_ID);
	m_MyUI.Initial(szDataFileName);

	m_crMoveRect.CreateRectRgn(685,640,760,640+27);

	return 0;
}
/// 初始化UI
int CNewGameClient::InitUI()
{
	m_MyUI.ShowControl(CTN_1000000_BTN_10001, false);//准备按钮隐藏
	//设置鼠标响应454
	//SetButtonOnMoseMove(10121002,true);
	SetImageOnMoseMove(CTN_1000000_CTN_1012000_CTN_1012100_IMG_10121001,true);
	SetImageOnMoseMove(CTN_1000000_IMG_120120088,true);
	SetImageOnMoseMove(CTN_1000000_CTN_1012000_IMG_10150027,true);
	SetImageOnMoseMove(CTN_1000000_CTN_1012000_CTN_1012100_BTN_10121002,true);
	SetImageOnMoseMove(CTN_1000000_IMG_454,true);
	SetImageMoseThough(CTN_1000000_CTN_1012000_CTN_1012100_IMG_10121003,true);
	for(int i=0;i<6;i++)
	{
		SetImageOnMoseMove(CTN_1000000_CTN_1012000_BTN_10120041+i,true);
	}
	for(int i=0;i<PLAY_COUNT;i++)
	{
		SetImageOnMoseMove(CTN_1000000_CTN_1012000_BTN_120120040+i,true);
	}
	//SetImageOnMoseMove(10121003,true);
	SetImageOnMoseMove(454,true);
	//初始化成绩容器
	SetControlVisible(CTN_1000000_BTN_2004043,false);
	SetControlVisible(CTN_1000000_CTN_2004042,false);
	SetControlVisible(CTN_1000000_BTN_2004044,true);
	for(int i=0;i<15;i++)
	{
		SetControlVisible(CTN_1000000_CTN_2004042_TEXT_10150028+i,false);
	}
	SetRButtonSelected(CTN_1000000_RButton_10150047,false);
	SetRButtonSelected(CTN_1000000_RButton_10150048,false);
	for(int i=0;i<PLAY_COUNT;i++)
	{
		SetControlVisible(CTN_1000000_CTN_120120073+i,false);
	}
	InitGameUI();
	return 0;
}

//游戏退出
bool CNewGameClient::OnGameQuiting()
{

	//查询状态
	if (GetStationParameter() >= GS_SEND_CARD&&GetStationParameter() < GS_WAIT_NEXT && m_bPlayer[m_pGameFrame->GetMyDeskStation()]
	&& !m_pGameFrame->GetIsWatching() && m_bUserState[m_bMyDesk])
	{
		CString strInfo = "";
		strInfo += "您正在游戏中，是否要退出游戏?\n";

		char cKey[10];
		CString sPath=CBcfFile::GetAppPath();
		int iResult = 1;
		if (CBcfFile::IsFileExist(sPath + "..\\SpecialRule.bcff"))
		{
			CBcfFile fsr( sPath + "..\\SpecialRule.bcf");
			sprintf(cKey, "%d", NAME_ID);
			iResult = fsr.GetKeyVal (_T("ForceQuitAsAuto"), cKey, 0);
		}


		if (iResult)
		{
			strInfo.Format("现在退出将会被扣所有下注，您真的要退出吗?");
		}
		else
		{
			strInfo.Format("下注后退出将会被托管,您真的要退出吗");
		}

		if(IDOK != AFCMessageBox(strInfo, "提示",AFC_YESNO))
		{
			return false;
		}	
		m_pGameFrame->SendGameData(NULL,0,MDM_GM_GAME_FRAME,ASS_GM_FORCE_QUIT,0);
		memset(m_bPlayer,0,sizeof(m_bPlayer));
	}

	
	return true;
}

//用户强行离开
void CNewGameClient::OnHandleASS_CUT_END(BYTE * buffer,int nLen)
{
	if (nLen!=sizeof(GameCutStruct)) 
		return;

	BYTE bStationPa=GetStationParameter();
	if (GS_WAIT_NEXT==bStationPa || GS_WAIT_SETGAME==bStationPa || GS_WAIT_ARGEE==bStationPa)
		return;

	GameCutStruct * pCmd=(GameCutStruct *)buffer;
	KillAllTimer();

	TCHAR skin[MAX_NAME_INFO];
	CString s = CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f( s + GET_SKIN_FOLDER(skin) + ".bcf");
	CString key = TEXT("config");
	CString sec = _T("");
	//处理非空
	GameFinishNotify CutNotify;

	if(pCmd->bDeskStation != m_bMyDesk && IsPlayer(pCmd->bDeskStation))
	{
		//系统消息
		CutNotify.iBasePoint = m_iBaseNote;
		CutNotify.iStyle = 0;
		//显示信息
		TCHAR szMessage[100] = {0};
		UserInfoForGame UserInfo;
		if (m_pGameFrame->GetUserInfo(pCmd->bDeskStation,UserInfo))
		{
			// 由于〖 %s 〗强行退出,他将被扣分(游戏币)，游戏结束。"
			sec.Format("ResultTipCutEnd%d", m_pGameFrame->GetGameInfo()->uComType);
			wsprintf(szMessage,f.GetKeyVal(key, sec, ""),UserInfo.szNickName);
		}
		m_pGameFrame->InsertNormalMessage(szMessage);
		CutNotify.iBasePoint = m_iBaseNote;
		CutNotify.iStyle = 0;
		//声音
		
	}

	//筹码收集
	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		ShowPlayerNote(i,false);
	}

	SetStationParameter(GS_WAIT_ARGEE);
	ShowResult(CutNotify,true);
	
	return;
}

//玩家准备消息
void CNewGameClient::OnHandleGM_AGREE_GAME(BYTE * buffer,int nLen)
{
	MSG_GR_R_UserAgree * pCmd = (MSG_GR_R_UserAgree *)buffer;
	if (nLen != sizeof(MSG_GR_R_UserAgree))
	{
		return;
	}

	if (1 == pCmd->bAgreeGame)
	{
		ShowAgree(pCmd->bDeskStation);
	}

	return;
}

//52游戏开始
void CNewGameClient::OnHandleGM_ASS_GAME_BEGIN(BYTE * buffer,int nLen)
{

	if (nLen != sizeof(GameBeginStruct))
	{
		return;
	}

	InitGameData();
	InitGameUI();
	ShowBegin();
	m_bNewTurn=false;
	memset(m_iTempBt,0,sizeof(m_iTempBt));
	KillAllTimer();
	
	memset((void *)&m_szName, 0,sizeof(m_szName));

	//设置游戏状态
	SetStationParameter(GS_SEND_CARD);
	m_bTimeOutCount = 0;												// 超时次数
	GameBeginStruct * pCmd=(GameBeginStruct *)buffer;
	m_bIsSuperUser = pCmd->bIsSuperUser;//自己是否超级客户端//lym 2011-11-28

	m_iLimitNote = pCmd->iLimitNote;									// 最大注(一局)
	m_iBaseNote = pCmd->iBaseNote;									// 底注
	m_iSelAddMoney = m_iBaseNote;
	m_i64HalfLessMoney=pCmd->i64HalfLessMoney;
	m_byCurStyle = 0x00;
	::CopyMemory(m_bUserState,pCmd->bUserState,sizeof(pCmd->bUserState));	//用户状态

	UserInfoForGame UserInfo;
	//下底注
	for(int i = 0; i < PLAY_COUNT; i ++)
	{
		if(!IsPlayer(i) || false == m_bUserState[i])
			continue;
		//modified by yjp.游戏开始的底注根据底注，而不是上限
		AddTotalNote(m_iBaseNote);
		m_TempNote[i] = m_iBaseNote;
		m_iPerJuTotalNote[i] += m_iBaseNote;//(DOUBLE)m_iLimitNote*0.015;
		m_iUserStation[i] = -1;
		if (m_pGameFrame->GetUserInfo(i,UserInfo))
			lstrcpy(m_szName[i],UserInfo.szNickName);
	}

	//播放玩家下注动画
	m_Time_BASENOTE_Count = 0;
	m_pGameFrame->SetTimer(ID_TIME_BASENOTE,10);

	ShowRoomInfo();
	PlaySound(SOUND_START_DEF);

	return;
}
//53发牌信息
void CNewGameClient::OnHandleGM_ASS_SEND_CARD(BYTE * buffer,int nLen)
{
	if(nLen != sizeof(SendOneTurnCardStruct))
	{
		return;
	}
	OutputDebugString("dwjsh::OnHandleGM_ASS_SEND_CARD（0）");
	SendOneTurnCardStruct  *pSendData = (SendOneTurnCardStruct *)buffer;

	for(int i = 0 ; i<PLAY_COUNT ; i++)
	{
		if(!IsPlayer(i) || false == m_bUserState[i])
		{
			m_iDeskCardCount[i] = 0 ;
		}
		else
		{

			if(m_bIsSuperUser)//超级客户端显示所有的牌
			{
				if (m_iUserStation[i] != TYPE_GIVE_UP)
				{
					m_iDeskCardCount[i] = pSendData->iCardCount[i]; 
					memcpy(&m_UserCard[i],&pSendData->iCardList[i],sizeof(BYTE)*5);
					memcpy(&m_DeskCard[i],&pSendData->iCardList[i],sizeof(BYTE)*5);
				}
			}
			else
			{
				m_iDeskCardCount[i] = pSendData->iCardCount[i];
				memcpy(&m_UserCard[i],&pSendData->iCardList[i],sizeof(BYTE)*5);
				memcpy(&m_DeskCard[i],&pSendData->iCardList[i],sizeof(BYTE)*5);
			}
		}
		if(!m_bIsSuperUser)//lym 2011-11-28
		{
			m_DeskCard[i][0] = 0x00;	
		}
	}


	for(int i = 0 ;i<PLAY_COUNT; i++)
	{
		if(!IsPlayer(i) || false == m_bUserState[i])
		{
			continue;
		}
		else
		{
			m_iNeedSendCardTotle += pSendData->iCardCount[i] ;
		}
	}			

	m_iFirstSendCardCount = pSendData->iFirstSendCardCount;
	m_iSendCardTotle = 0;
	m_iSendAnimaCardPos = pSendData->iStartPos;
	ZeroMemory(m_iSendCardNum,sizeof(m_iSendCardNum));
	m_bSendCard = true;
	m_pGameFrame->SetTimer(ID_TIME_SEND_CARD,20);

	return;
}
//54发牌完成
void CNewGameClient::OnHandleGM_ASS_SEND_FINISH(BYTE * buffer,int nLen)
{
	//停止发牌，显示手牌
	m_pGameFrame->KillTimer(ID_TIME_SEND_CARD);
	m_bSendCard = false;
	m_iSendAnimaCardPos = 0;//下面用不着了，初始化就可以了
	m_iSendCardTotle = m_iNeedSendCardTotle;

	m_MyUI.ShowControl(CTN_1000000_CTN_1015000_NoMoveCard_10150011, false);
	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		if (IsPlayer(i))
		{
			m_iSendCardNum[i] = m_iFirstSendCardCount;//各自只发了两张
			BYTE icard[5] = {0};
			CopyMemory(icard,&m_DeskCard[i],sizeof(BYTE)*5);
			ShowHandCard(i, icard, m_iSendCardNum[i], true);
		}
	}

	return;
}
//55开始游戏
void CNewGameClient::OnHandleGM_ASS_GAME_PLAY(BYTE * buffer,int nLen)
{
	if (nLen != sizeof(BeginPlayStruct)) 
		return;
	//设置游戏状态
	SetStationParameter(GS_PLAY_GAME);

	BeginPlayStruct * pCmd=(BeginPlayStruct *)buffer;

	m_bTimeOutCount = 0;					//超时次数设置
	m_iNowOutPeople = m_iFirstOutPeople = pCmd->bDeskStaton ;
	m_iNowBigNote = 0;							//当前最大押注数
	m_iThisTurnLimit = m_iLimitNote/4; //NOTE_LIMIT+m_iBaseNote; 第一轮下注 第二轮为 是底与全甩的1/4之间
	//第一次下注
	m_iThisNoteTimes = 1;
	//设置时间
	if(m_iNowOutPeople != -1)
	{
		SetGameTimer(m_iNowOutPeople);
	}
	//显示最大牌玩家位置
	ShowCardMaxMark(true);

	if (true == m_bUserState[m_bMyDesk])
	{
		SetNoteButText(m_iBaseNote);
	}

	bool bShow = true;
	// 小于2不能梭哈
	if (m_iThisNoteTimes < m_bShowHa)
	{
		bShow = false;
	}

	// 自己是第一个下注的人
	if (m_bMyDesk == m_iNowOutPeople)
	{
		if (m_bPK)
		{
			ShowNoteBtnEnable(0,bShow,0,1);
		}
		else
		{
			ShowNoteBtnEnable(1,bShow,1,1);
		}
		SetImageOnMoseMove(CTN_1000000_IMG_120120088,true);
		m_MyUI.EnableControl(CTN_1000000_CTN_1012000_CTN_1012100_BTN_10121002,true);
	}
	else
	{
		SetImageOnMoseMove(CTN_1000000_IMG_120120088,false);
		m_MyUI.EnableControl(CTN_1000000_CTN_1012000_CTN_1012100_BTN_10121002,false);
	}
	bool bSelected=false;
	GetRButtonSelected(CTN_1000000_RButton_10150048,&bSelected);
	if(bSelected)
	{
		m_pGameFrame->KillTimer(ID_TIME_SEE_CARD);
		BYTE icard[5] = {0};
		CopyMemory(icard,&m_UserCard[m_bMyDesk],sizeof(BYTE)*5);
		ShowHandCard(m_bMyDesk, icard, m_iDeskCardCount[m_bMyDesk], true);
		m_pGameFrame->SetTimer(ID_TIME_SEE_CARD, 2000);
	}
	return;
}
//60下注
void CNewGameClient::OnHandleGM_ASS_NOTE(BYTE * buffer,int nLen)
{
	if (nLen != sizeof(tagUserResult)) 
		return;
	tagUserResult *pCmd = (tagUserResult *) buffer;
	//用户下注用来显示下注动画,暂不赋值

	KillGameTimer(m_iNowOutPeople);

	m_iNowOutPeople = pCmd->iOutPeople;		//当前加注者

	CString sdp;
	sdp.Format("sdp_ 发一张牌 开始下注消息 玩家ID[%d]",m_iNowOutPeople);
	OutputDebugString(sdp);

	if (pCmd->bNewTurn)
	{
		m_iFirstOutPeople = pCmd->iOutPeople;
		ShowCardMaxMark(true);//绘制牌最大标志
	}

	//设置时间
	if (m_iNowOutPeople != -1) 
	{
		SetGameTimer(m_iNowOutPeople);
	}
		//计时器
//1、	第1轮系统为所有人共同下的底（可设置不通房间不同的最少下底额）
//2、	第2轮下注额，是底与全甩的1/4之间，
//3、	第3轮下注额，是底到全甩的1/2之间，
//4、	第4轮下注额是底和棱哈之间
//5、 第5 轮下注额是底和棱哈之间

	// qy //设置押注上限
	if (1 == pCmd->bNoteTimes)  // 第一轮
	{
		m_iThisTurnLimit = m_iLimitNote/4;
	}
	else if (2 == pCmd->bNoteTimes)
	{
		m_iThisTurnLimit = m_iLimitNote/2;
	}
	else
	{
		m_iThisTurnLimit = (m_iLimitNote - m_iLimitNote/2 - m_iLimitNote/4-m_iBaseNote);
	}

	//系统给他下注次数
	m_iThisNoteTimes = pCmd->bNoteTimes;
	//当前最大押注
	m_iNowBigNote = pCmd->iNowBigNote;

	if (pCmd->iOutPeople == m_bMyDesk)
	{					
		BYTE style = pCmd->iNowBigNoteStyle;			//上一玩家最大下注类型
		m_byCurStyle = style;                           // 保存下注类型
		BOOL show = m_iThisNoteTimes >= m_bShowHa;					//第四轮才可以梭
					
		BOOL add;

		add = (style != TYPE_SHUTTLE && m_iNowBigNote >= 0);

		BOOL note = (m_iNowBigNote == 0);					//下注,只有在第一个下注者,或者本轮没有人下过注
		BOOL follow = (style != TYPE_SHUTTLE);	//没有人梭且下注额非0
		BOOL giveup = 1;
		if (m_bPK)
		{
			giveup= ((m_iThisNoteTimes > m_byUserFollow) && (m_byCurStyle != TYPE_SHUTTLE));
		}
		else
		{
			giveup= ((m_iThisNoteTimes > m_byUserFollow) || (m_byCurStyle == TYPE_SHUTTLE));
		}
					
		// 大于本论限注，不能加注
		if (m_iNowBigNote >= m_iThisTurnLimit)
		{
			if(m_bNewTurn)
			{
				add = FALSE;
			}
		}
		//超过押注次数了 lym 2011-12-13
		if(m_byAddNum>0 && m_byAddNum!=255)
		{
			if (pCmd->m_byUserAddNum[m_bMyDesk] >= m_byAddNum)
			{
				if(m_bNewTurn)
				{
					add = FALSE;
				}
			}
			//第一个不能加
			if (m_iFirstOutPeople == m_bMyDesk && !pCmd->bNewTurn)
			{
				if(m_bNewTurn)
				{
					add = FALSE;
				}
				
				if (pCmd->m_byUserAddNum[m_bMyDesk]  == 0)
					show = FALSE;
			}
		}

		// 小于2不能梭哈
		if (m_iThisNoteTimes < m_bShowHa)
		{
			show = FALSE;
		}

		if (m_bPK)
		{
			ShowNoteBtnEnable(0,1,0,1);
		}

		// 其他人梭哈时，是可以梭哈的
		if (TYPE_SHUTTLE == style)
		{
			show = FALSE;
			follow = TRUE;
		}
					
		ShowNoteBtnEnable(follow,show,add,giveup);
	}
	return;
}
//63下注结果
void CNewGameClient::OnHandleGM_ASS_NOTE_RESULT(BYTE * buffer,int nLen)
{
	if (nLen != sizeof(NoteResult)) 
		return;
	KillGameTimer(m_iNowOutPeople);
	UserInfoForGame UserInfo;
	NoteResult *pCmd = (NoteResult*)buffer;
	::CopyMemory(m_iPerJuTotalNote,pCmd->iPlayerTotalNote,sizeof(pCmd->iPlayerTotalNote));
	int iViewStation=m_pGameFrame->DeskStation2View(pCmd->iOutPeople);
	//声音
	//如果是放弃则不再显示牌面
	if(pCmd->bNoteStyle == TYPE_GIVE_UP)
	{
		PlaySound(SOUND_GIVE_UP);
		//在左框顯示
		TCHAR szMessage[100];
		if(IsPlayer(pCmd->iOutPeople))
		{
			if (m_pGameFrame->GetUserInfo(pCmd->iOutPeople,UserInfo))
			{
				wsprintf(szMessage,TEXT("[提示] %s 放弃本局游戏"),UserInfo.szNickName);
			}
		}

		m_pGameFrame->InsertNormalMessage(szMessage);
		m_iUserStation[pCmd->iOutPeople] = TYPE_GIVE_UP;
		ShowGiveUp(pCmd->iOutPeople);

		memset(m_DeskCard[pCmd->iOutPeople],0,m_iDeskCardCount[pCmd->iOutPeople]);
		memset(m_UserCard[pCmd->iOutPeople],0,sizeof(m_UserCard[pCmd->iOutPeople]));
		//显示桌面牌情形
		BYTE icard[5] = {0};
		CopyMemory(icard,&m_DeskCard[pCmd->iOutPeople],sizeof(BYTE)*5);
		ShowHandCard(pCmd->iOutPeople, icard, 0, false);
									
		if(m_bMyDesk==pCmd->iOutPeople)
		{
			ShowNoteBtnEnable();
		}
	}
	else			//更新下注
	{
		if (pCmd->bNoteStyle != TYPE_SHUTTLE)
		{
			PlaySound(SOUND_NOTE);
		}
		else
		{
			PlaySound(SOUND_SHUTTLE);
		}
			
		//此为本轮用户下注数,可能是加注者,也可以是跟注,补注
		AddTotalNote(pCmd->iCurNote);
				
		//colin090420记录
		m_TempNote[pCmd->iOutPeople] = pCmd->iCurNote;

		//设置某一个人下注总额
		if(m_iThisNoteTimes < m_bShowHa)
		m_iThisGameNote[pCmd->iOutPeople] += pCmd->iCurNote;
		//本轮下注
		m_iThisTurnNote[pCmd->iOutPeople] += pCmd->iCurNote;
		//下注动画
		if (pCmd->iCurNote > 0)
		{
			ShowNoteAnima(pCmd->iOutPeople, pCmd->iCurNote, 500);
		}
		__int64 i64AllBet=0;
		//更新总注
		for(int i=0;i<PLAY_COUNT;i++)
		{
			i64AllBet+=m_iPerJuTotalNote[i];
		}
		SetControlVisible(CTN_1000000_IMG_120120045,true);
		SetTextinfo(CTN_1000000_TEXT_120120046,i64AllBet, true);
		SetTextinfo(CTN_1000000_CTN_1011000_TEXT_10110002,i64AllBet, true);
		//更新已下注
		SetControlVisible(CTN_1000000_IMG_120120047+iViewStation,true);
		SetTextinfo(CTN_1000000_TEXT_120120052+iViewStation,m_iThisTurnNote[pCmd->iOutPeople], true);		
	}
	//更新提示
	if (TYPE_GIVE_UP==pCmd->bNoteStyle)//放弃
	{
		//提示容器
		for(int i=0;i<5;i++)
		{
			SetControlVisible(CTN_1000000_CTN_120120062+i,false);
		}
		m_pGameFrame->KillTimer(ID_TIME_TIP);
		SetControlVisible(CTN_1000000_CTN_120120062+iViewStation,true);
		SetPicSize(CTN_1000000_CTN_120120062_IMG_120120067+iViewStation,"./resources/05.png",true,188,0,47,24);
		m_pGameFrame->SetTimer(ID_TIME_TIP,2*1000);
	}
	else if (TYPE_NOTE==pCmd->bNoteStyle)//下注
	{
		//提示容器
		for(int i=0;i<5;i++)
		{
			SetControlVisible(CTN_1000000_CTN_120120062+i,false);
		}
		m_pGameFrame->KillTimer(ID_TIME_TIP);
		SetControlVisible(CTN_1000000_CTN_120120062+iViewStation,true);
		SetPicSize(CTN_1000000_CTN_120120062_IMG_120120067+iViewStation,"./resources/05.png",true,47,0,47,24);
		m_pGameFrame->KillTimer(ID_TIME_TIP);
		m_pGameFrame->SetTimer(ID_TIME_TIP,2*1000);
	}
	else if (TYPE_ADD==pCmd->bNoteStyle)//加注
	{
		//提示容器
		for(int i=0;i<5;i++)
		{
			SetControlVisible(CTN_1000000_CTN_120120062+i,false);
		}
		m_pGameFrame->KillTimer(ID_TIME_TIP);
		SetControlVisible(CTN_1000000_CTN_120120062+iViewStation,true);
		SetPicSize(CTN_1000000_CTN_120120062_IMG_120120067+iViewStation,"./resources/05.png",true,0,0,47,24);
		m_pGameFrame->KillTimer(ID_TIME_TIP);
		m_pGameFrame->SetTimer(ID_TIME_TIP,2*1000);
	}
	else if (TYPE_FOLLOW==pCmd->bNoteStyle)//跟注
	{
		//提示容器
		for(int i=0;i<5;i++)
		{
			SetControlVisible(CTN_1000000_CTN_120120062+i,false);
		}
		m_pGameFrame->KillTimer(ID_TIME_TIP);
		SetControlVisible(CTN_1000000_CTN_120120062+iViewStation,true);
		SetPicSize(CTN_1000000_CTN_120120062_IMG_120120067+iViewStation,"./resources/05.png",true,141,0,47,24);
		m_pGameFrame->KillTimer(ID_TIME_TIP);
		m_pGameFrame->SetTimer(ID_TIME_TIP,2*1000);
	}
	else if (TYPE_SHUTTLE==pCmd->bNoteStyle)//梭哈
	{
		//提示容器
		for(int i=0;i<5;i++)
		{
			SetControlVisible(CTN_1000000_CTN_120120062+i,false);
		}
		m_pGameFrame->KillTimer(ID_TIME_TIP);
		SetControlVisible(CTN_1000000_CTN_120120062+iViewStation,true);
		SetPicSize(CTN_1000000_CTN_120120062_IMG_120120067+iViewStation,"./resources/05.png",true,235,0,47,24);
		m_pGameFrame->KillTimer(ID_TIME_TIP);
		m_pGameFrame->SetTimer(ID_TIME_TIP,2*1000);
	}
	return;
}
//70收集筹码
void CNewGameClient::OnHandleGM_ASS_COLLECT_JETON(BYTE * buffer,int nLen)
{
	if (nLen != sizeof(CollectJetonStruct)) 
		return;	

	ZeroMemory(m_iTempThisTurnNote, 0, sizeof(m_iTempThisTurnNote));
	//colin090418筹码的下一轮发牌时更新
	for (int i = 0; i< PLAY_COUNT;i++)
	{
		m_iTempThisTurnNote[i] = m_iThisTurnNote[i];
	}

	//播放收集筹码动画
	return;
}
//71机器人托管
void CNewGameClient::OnHandleGM_ASS_AI_STATION(BYTE * buffer,int nLen)
{
	if(nLen != sizeof(UseAIStation))
		return;
	UseAIStation *pCmd = (UseAIStation*)buffer;
	//在左框顯示
	TCHAR szMessage[100];
	UserInfoForGame UserInfo;

	if (!IsPlayer(pCmd->bDeskStation))
	{
		return;
	}

	if(IsPlayer(pCmd->bDeskStation) && true == m_bUserState[pCmd->bDeskStation])
	{
		if (m_pGameFrame->GetUserInfo(pCmd->bDeskStation,UserInfo))
		{
			m_iUserStation[pCmd->bDeskStation] = TYPE_GIVE_UP;
			wsprintf(szMessage,TEXT("[提示] %s 弃牌走了"),UserInfo.szNickName);
			m_pGameFrame->InsertNormalMessage(szMessage);
		}
	}

	memset(m_DeskCard[pCmd->bDeskStation],0,m_iDeskCardCount[pCmd->bDeskStation]);
	m_bNetCutPlayer[pCmd->bDeskStation] = TRUE;

	if( m_iNowOutPeople == pCmd->bDeskStation)
		ReplaceUserNote(pCmd->bDeskStation);

	//隐藏手牌和筹码
	BYTE icard[5] = {0};
	CopyMemory(icard,&m_DeskCard[pCmd->bDeskStation],sizeof(BYTE)*5);
	ShowHandCard(pCmd->bDeskStation, icard, 0, false);

	return;
}
//81新发一张牌
void CNewGameClient::OnHandleGM_ASS_SEND_A_CARD(BYTE * buffer,int nLen)
{
	OutputDebugString("dwjlog::C_收到发一张牌消息");
	if(nLen != sizeof(SendOneTurnCard))
	{
		return;
	}
	ShowCardMaxMark(false);
	::memset(m_iThisTurnNote,0,sizeof(m_iThisTurnNote));

	///可能会断线重连，所以重新计算发牌的总数和要发的张数
	m_iSendCardTotle = 0;
	m_iNeedSendCardTotle = 0;
	SendOneTurnCard *pCmd = (SendOneTurnCard *)buffer ;
	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		if(0 !=pCmd->iTurnCard[i] && IsPlayer(i) &&	true == m_bUserState[i] && TYPE_GIVE_UP!=m_iUserStation[i])
		{
			m_iSendCardNum[i] = m_iDeskCardCount[i];
			m_iSendCardTotle += m_iDeskCardCount[i];
			m_DeskCard[i][m_iDeskCardCount[i]] = pCmd->iTurnCard[i];			//用户桌面上的牌
			m_UserCard[i][m_iDeskCardCount[i]++] = pCmd->iTurnCard[i];		//用户手中牌
			m_iNeedSendCardTotle += m_iDeskCardCount[i];
		}
	}
	//发牌起始位置
	m_iSendAnimaCardPos = pCmd->bStartPos;
	m_bSendCard = true;
	m_pGameFrame->SetTimer(ID_TIME_SEND_CARD,20);

	CString sdp;
	sdp.Format("sdp_ 发一张牌 玩家ID[%d]牌张数=%d,%d",pCmd->bStartPos,m_iDeskCardCount[pCmd->bStartPos],m_iSendCardNum[pCmd->bStartPos]);
	OutputDebugString(sdp);

	return;
}
//90游戏结束
void CNewGameClient::OnHandleGM_ASS_CONTINUE_END(BYTE * buffer,int nLen)
{
	GameEndStruct * pCmd = (GameEndStruct *)buffer;
	if (nLen != sizeof(GameEndStruct))
	{
		return;
	}

	BYTE bStationPa=GetStationParameter();
	if (GS_WAIT_NEXT==bStationPa || GS_WAIT_SETGAME==bStationPa || GS_WAIT_ARGEE==bStationPa)
		return;

	CString stime;
	stime.Format("%d",CTime::GetCurrentTime());
	int curtime=atoi(stime);

	__int64 nMoneyTemp = 1;
	for(int i=0; i<Glb().m_nPowerOfGold; i++)
	{
		nMoneyTemp*=10;
	}

	TCHAR skin[MAX_NAME_INFO];
	CString s = CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f( s + GET_SKIN_FOLDER(skin) + ".bcf");
	CString key = TEXT("config");
	CString sec = _T("");

	GameFinishNotify finishnotify;
	GameEndStruct * pGameEnd = (GameEndStruct *)buffer;
	KillAllTimer();
	m_pGameFrame->InsertNormalMessage(TEXT("本局游戏结果："));
	m_pGameFrame->InsertNormalMessage(TEXT("－－－－－－－－－－－－"));
				
	CString sdp;


	CString str;
	int count = 0;
	for (int i = 0; i < PLAY_COUNT; i ++)
	{
		if (pGameEnd->bShowCardShape[i])
		{
			count++;
		}
	}
	UserInfoForGame UserInfo;
	//设置分数 
	for (int i = 0; i < PLAY_COUNT; i ++)
	{
		if(IsPlayer(i) && true == m_bUserState[i])
		{
			__int64 nChangeMoneyTmp = pGameEnd->iChangeMoney[i]*nMoneyTemp;

			if (count <= 1 && m_bMyDesk != i)
			{
				m_UserCard[i][0] = 0;
			}

			//刷新
			BYTE icard[5] = {0};
			CopyMemory(icard,&m_UserCard[i],sizeof(BYTE)*5);
			ShowHandCard(i, icard, m_iDeskCardCount[i], true);
			sdp.Format("sdp_ 发一张牌 游戏结束 玩家ID[%d]牌张数=%d,%d",i, m_iDeskCardCount[i]);
			OutputDebugString(sdp);

			// %s : 得分 %d 游戏币 %d
			sec.Format("ResultMsg%d", m_pGameFrame->GetGameInfo()->uComType);
			if (m_pGameFrame->GetUserInfo(i,UserInfo))
			{
				if (m_pGameFrame->GetGameInfo()->uComType ==1)
				{
					str.Format(f.GetKeyVal(key, sec, ""),UserInfo.szNickName,
						pGameEnd->iTurePoint[ i ]);

				}
				else
				{
					str.Format(f.GetKeyVal(key, sec, ""),UserInfo.szNickName,
						nChangeMoneyTmp);
				}
			}
			//玩家结算信息
			m_pGameFrame->InsertNormalMessage(str.GetBuffer(MAX_PATH));
		}
		else
		{
			__int64 nChangeMoneyTmp = pGameEnd->iChangeMoney[i]*nMoneyTemp;
			sec.Format("ResultMsg%d", m_pGameFrame->GetGameInfo()->uComType);
			str.Format(f.GetKeyVal(key, sec, ""), m_szName[i], nChangeMoneyTmp);
		}
		if (i == m_bMyDesk)
		{
			//声音
			if (pGameEnd->iChangeMoney[i] > 0)
			{
				PlaySound(SOUND_WIN);
			}
			else
			{
				PlaySound(SOUND_LOSE);
			}
		}

		finishnotify.iWardPoint[i] = pGameEnd->iTurePoint[i];
		finishnotify.iMoney[i] = pGameEnd->iChangeMoney[i];
		if (count > 1 || i ==  m_bMyDesk)
			finishnotify.iCardShape[i] = pGameEnd->iCardShape[i];
		else
			finishnotify.iCardShape[i] = 0;
	}

	// 放弃玩家是否显示牌型（配置文件配置）
	CBcfFile fCardShape( s + GET_SKIN_FOLDER(skin) + ".bcf");
	int iShowCardType = fCardShape.GetKeyVal(_T("config"), _T("GiveUpShowCardType"), 1);
	if (0==iShowCardType && !pGameEnd->bShowCardShape[m_bMyDesk])
		memset(finishnotify.iCardShape, 0, sizeof(finishnotify.iCardShape));

	m_pGameFrame->InsertNormalMessage(TEXT("－－－－－－－－－－－－"));
	SetStationParameter(GS_WAIT_NEXT);
	finishnotify.iBasePoint = m_iBaseNote;
	finishnotify.iStyle = 1;
        
	////////////////////////////////////colin收筹码结束动画 
	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		ShowPlayerNote(i,false);
	}
	
	//结算框
	//ShowResult(finishnotify,true);
	for(int i=0; i<PLAY_COUNT; i++)
	{
		SetControlVisible(CTN_1000000_CTN_120120073+i,false);
	}
	//刷新累积成绩和本局成绩
	for(int i=0; i<PLAY_COUNT; i++)
	{
		if (!IsPlayer(i))
			continue;
		//正常状态的玩家
		if (m_pGameFrame->GetUserInfo(i,UserInfo))
		{
			SetTextShowText(CTN_1000000_CTN_2004042_TEXT_10150028+i,UserInfo.szNickName, true);
			SetTextinfo(CTN_1000000_CTN_2004042_TEXT_10150033+i,pCmd->i64UpGrade[i], true);
			SetTextinfo(CTN_1000000_CTN_2004042_TEXT_10150038+i,pCmd->i64AcculateGrade[i], true);
			//刷新本局成绩
			int iViewStation=m_pGameFrame->DeskStation2View(i);
			SetControlVisible(CTN_1000000_CTN_120120073+iViewStation,true);
			if(pGameEnd->iChangeMoney[i]>0)
			{
				wchar_t szImagePath[256]=L"./resources/10.png";
				SetInumInfo(CTN_1000000_CTN_120120073_Num_120120078+iViewStation,szImagePath,pGameEnd->iChangeMoney[i],true);
				//根据图片控件设置显示图片的尺寸
				SetPicSize(CTN_1000000_CTN_120120073_IMG_120120083+iViewStation,"./resources/11.png", true,0,0,35,39);
			}
			else if(0==pGameEnd->iChangeMoney[i])
			{
				wchar_t szImagePath[256]=L"./resources/09.png";
				SetInumInfo(CTN_1000000_CTN_120120073_Num_120120078+iViewStation,szImagePath,pGameEnd->iChangeMoney[i],true);
			}
			else
			{
				wchar_t szImagePath[256]=L"./resources/09.png";
				SetInumInfo(CTN_1000000_CTN_120120073_Num_120120078+iViewStation,szImagePath,pGameEnd->iChangeMoney[i],true);
				//根据图片控件设置显示图片的尺寸
				SetPicSize(CTN_1000000_CTN_120120073_IMG_120120083+iViewStation,"./resources/11.png", true,35,0,35,39);
			}
		}
		else
		{
			SetControlVisible(CTN_1000000_CTN_2004042_TEXT_10150028+i,false);
			SetControlVisible(CTN_1000000_CTN_2004042_TEXT_10150033+i,false);
			SetControlVisible(CTN_1000000_CTN_2004042_TEXT_10150038+i,false);
		}
	}
	//显示继续按钮
	SetControlVisible(CTN_1000000_CTN_1001000_BTN_1001100,true);
	bool bSelected=false;
	GetRButtonSelected(CTN_1000000_RButton_10150047,&bSelected);
	if(bSelected)
	{
		m_pGameFrame->SetTimer(ID_TIME_AUTOSTART,5*1000);
	}
	m_bAutoStart=true;
	//启动结束定时器
	m_pGameFrame->SetTimer(ID_END_TIME,m_iBeginTime*1000);
	m_MyUI.ShowControl(CTN_1000000_CTN_1010000_CTN_1010001 + ViewStation(m_bMyDesk), true);
	m_MyUI.SetClock(CTN_1000000_CTN_1010000_CTN_1010001_Time_10100001 + ViewStation(m_bMyDesk), m_iBeginTime, true);
	return;
}
//93游戏安全结束
void CNewGameClient::OnHandleGM_ASS_SALE_END(BYTE * buffer,int nLen)
{
	if (nLen!=sizeof(GameCutStruct)) 
		return;

	BYTE bStationPa=GetStationParameter();
	if (GS_WAIT_NEXT==bStationPa || GS_WAIT_SETGAME==bStationPa || GS_WAIT_ARGEE==bStationPa)
		return;

	GameCutStruct * pCmd=(GameCutStruct *)buffer;
	//KillGameTimer(0);

	//声音
	SetStationParameter(GS_WAIT_ARGEE);	
	KillAllTimer();

	return;
}
//94游戏结束无胜出者
void CNewGameClient::OnHandleGM_ASS_NO_WINER(BYTE * buffer,int nLen)
{
	if(nLen != sizeof(GameAheadStruct))
		return;

	BYTE bStationPa=GetStationParameter();
	if (GS_WAIT_NEXT==bStationPa || GS_WAIT_SETGAME==bStationPa || GS_WAIT_ARGEE==bStationPa)
		return;

	KillAllTimer();
	GameAheadStruct * pCmd = (GameAheadStruct*)buffer;

	GameFinishNotify finishnotify;
	::memset(&finishnotify,0,sizeof(finishnotify));
	m_pGameFrame->InsertNormalMessage(TEXT("本局游戏结果："));
	m_pGameFrame->InsertNormalMessage(TEXT("--------------------"));

	__int64 nMoneyTemp = 1;
	for(int i=0; i<Glb().m_nPowerOfGold; i++)
	{
		nMoneyTemp*=10;
	}

	TCHAR skin[MAX_NAME_INFO];
	CString s = CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f( s + GET_SKIN_FOLDER(skin) + ".bcf");
	CString key = TEXT("config");
	CString sec = _T("");

	CString str;
	UserInfoForGame UserInfo;
	for(int i = 0; i < PLAY_COUNT; i++)
	{
		if(!IsPlayer(i) || false == m_bUserState[i])
			continue;

		__int64 nChangeMoneyTmp = pCmd->iChangeMoney[i]*nMoneyTemp;
		
		finishnotify.iWardPoint[i] = pCmd->iTurePoint[i];
		finishnotify.iMoney[i] = pCmd->iChangeMoney[i];
		if (m_pGameFrame->GetUserInfo(pCmd->bDeskStation,UserInfo))
		{
			// %s 得分 %d 金币 %d
			sec.Format("ResultTip%d", m_pGameFrame->GetGameInfo()->uComType);
			if (m_pGameFrame->GetGameInfo()->uComType ==1)
			{
				str.Format(f.GetKeyVal(key, sec, ""),UserInfo.szNickName,
					pCmd->iTurePoint[ i ]);

			}
			else
			{
				str.Format(f.GetKeyVal(key, sec, ""),UserInfo.szNickName,
					nChangeMoneyTmp);
			}	
			m_pGameFrame->InsertNormalMessage(str.GetBuffer(MAX_PATH));
		}
	}

	////////////////////////////////////colin收筹码结束动画 
	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		ShowPlayerNote(i,false);
	}

	//声音
	//////////////////////////////// 

	m_pGameFrame->InsertNormalMessage(TEXT("－－－－－－－－－－－－"));
	SetStationParameter(GS_WAIT_ARGEE);
	ShowResult(finishnotify,true);

	return;
}

void CNewGameClient::SetGameStat_GameStation_Base(void * pBuffer,int nLen)
{
	GameStation_Base *pCmd = (GameStation_Base *) pBuffer;
	m_bIsSuperUser   = pCmd->bIsSuperUser;    //自己是否超级客户端//lym 2011-11-28
	m_bIsWatching = m_pGameFrame->GetIsWatching();
	CString sdp;
	sdp.Format("sdp_ 自己是旁观=%d",m_bIsWatching);
	OutputDebugString(sdp);
	//设置数据
	m_iThinkTime     = pCmd->iThinkTime;
	m_iBeginTime     = pCmd->iBeginTime;

	m_iRoomBasePoint = pCmd->iRoomBasePoint ;//房间倍数
	m_iRunPublish    = pCmd->iRunPublish ;	 //逃跑扣分
	m_iLimitNote     = pCmd->iLimitNote;	 //最大注
	m_iBaseNote      = pCmd->iBaseNote;		 //底注
	m_iSelAddMoney	 = m_iBaseNote;
	//下注选项				
	m_byUserFollow   = pCmd->byUserFollow ;
	m_bShowHa        = pCmd->bShowHa;
	m_byAddNum	     = pCmd->m_byAddNum;	 /**< 每轮加注次数  0时不做限制 */   //lym 2011-12-13

	m_iThisTurnLimit = 0;
	m_iThisNoteTimes = 0;
	m_bPK            = pCmd->bPK;
	::CopyMemory(m_bUserState,pCmd->bUserState,sizeof(pCmd->bUserState));	//用户状态
	
	
	SetStationParameter(pCmd->bGameStation);

	ShowRoomInfo();

	return;
}
void CNewGameClient::SetGameStat_GameStation_1(void * pBuffer,int nLen)
{
	GameStation_1 *pCmd = (GameStation_1 *) pBuffer;
	return;
}
void CNewGameClient::SetGameStat_GameStation_2(void * pBuffer,int nLen)
{
	GameStation_2 *pCmd = (GameStation_2 *) pBuffer;

	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		if (pCmd->bAgree[i])
		{
			ShowAgree(i);
		}
	}

	memcpy(m_bUserState,pCmd->bUserState, sizeof(m_bUserState));



	//显示开始按钮 
	if (m_pGameFrame->GetGameInfo()->dwRoomRule & GRR_QUEUE_GAME)
	{
		m_MyUI.ShowControl(CTN_1000000_BTN_10001, false);//准备按钮隐藏
	}
	else
	{
		m_pGameFrame->SetTimer(ID_BEGIN_TIME,m_iBeginTime * 1000);
		m_MyUI.ShowControl(CTN_1000000_BTN_10001, true);//准备按钮显示
		m_MyUI.ShowControl(CTN_1000000_CTN_1010000_CTN_1010001 + ViewStation(m_bMyDesk), true);
		m_MyUI.SetClock(CTN_1000000_CTN_1010000_CTN_1010001_Time_10100001 + ViewStation(m_bMyDesk), m_iBeginTime, true);
	}
	return;
}

void CNewGameClient::SetGameStat_GameStation_3(void * pBuffer,int nLen)
{
	GameStation_3 *pCmd = (GameStation_3 *) pBuffer;
	if (NULL == pCmd)
	{
		return;
	}


	memcpy(m_bUserState,pCmd->bUserState, sizeof(m_bUserState));

	for (int i = 0; i < PLAY_COUNT;i++)
	{	
		//设置各家手中牌
		//桌面牌数
		m_iDeskCardCount[i] = pCmd->iUserCardCount[i];
		if(m_bIsSuperUser && m_iDeskCardCount[i]>0)//不是超级客户端不能看牌//lym 2011-11-28
		{
			m_iDeskCardCount[i] = 5;
		}
		::CopyMemory(m_UserCard[i],pCmd->iUserCard[i],sizeof(BYTE)*m_iDeskCardCount[i]);
		::CopyMemory(m_DeskCard[i],pCmd->iUserCard[i],sizeof(BYTE)*m_iDeskCardCount[i]);
		if(!m_bIsSuperUser)//不是超级客户端不能看牌//lym 2011-11-28
		{
			m_DeskCard[i][0] = 0;
		}
		m_iUserStation[i] = pCmd->iUserStation[i];
		//m_PlayView.m_UserCard[desk].SetCard(m_DeskCard[desk],NULL,m_iDeskCardCount[desk]);
		//设置各家手中押注情况
		//下底注
		UserInfoForGame UserInfo;
		for(int i = 0; i < PLAY_COUNT; i ++)
		{
			if(!IsPlayer(i) || false == m_bUserState[i])
				continue;
			//modified by yjp.游戏开始的底注根据底注，而不是上限
			AddTotalNote(m_iBaseNote);
			m_TempNote[i] = m_iBaseNote;
			m_iPerJuTotalNote[i] += m_iBaseNote;//(DOUBLE)m_iLimitNote*0.015;
			m_iUserStation[i] = -1;
			if (m_pGameFrame->GetUserInfo(i,UserInfo))
				lstrcpy(m_szName[i],UserInfo.szNickName);
			//下注
			ShowPlayerNote(i,true);
			//手牌
			BYTE icard[5] = {0};
			CopyMemory(icard,&m_DeskCard[i],sizeof(BYTE)*5);
			ShowHandCard(i, icard, m_iDeskCardCount[i], true);
		}
	}

	return;
}

void CNewGameClient::SetGameStat_GameStation_4(void * pBuffer,int nLen)
{
	GameStation_4 *pCmd = (GameStation_4 *) pBuffer;

	//设置数据
	m_iNowOutPeople = pCmd->tag.iOutPeople;
	m_iFirstOutPeople = pCmd->iFirstOutPeople;
	//保存当前用户下注总额
	::CopyMemory(m_iThisGameNote,pCmd->iTotalNote,sizeof(pCmd->iTotalNote));
	//memcpy(m_bUserState,pCmd->bUserState, sizeof(m_bUserState));
	::CopyMemory(m_bUserState,pCmd->bUserState,sizeof(m_bUserState));	//用户状态

	for (int i = 0;i < PLAY_COUNT; i ++)
	{	
		if (!IsPlayer(i))
		{
			continue;
		}

		//设置各家手中牌
		//桌面牌数
		m_iDeskCardCount[i] = pCmd->iUserCardCount[i];
		if(m_bIsSuperUser && m_iDeskCardCount[i]>0)//不是超级客户端不能看牌//lym 2011-11-28
		{
			m_iDeskCardCount[i] = 5;
		}
		::CopyMemory(m_UserCard[i],pCmd->iUserCard[i],sizeof(BYTE)*m_iDeskCardCount[i]);
		::CopyMemory(m_DeskCard[i],pCmd->iUserCard[i],sizeof(BYTE)*m_iDeskCardCount[i]);
		if(!m_bIsSuperUser)//不是超级客户端不能看牌//lym 2011-11-28
		{
			m_DeskCard[i][0] = 0;
		}
		if (m_iUserStation[i] == TYPE_GIVE_UP)
		{
			memset(m_DeskCard[i],0,sizeof(m_DeskCard[i]));
			ShowGiveUp(i);
		}

		//设置各家手中押注情况
		m_iThisTurnNote[i] = pCmd->iThisTurnNote[i];
		m_iPerJuTotalNote[i] = pCmd->iTotalNote[i];
		AddTotalNote(m_iPerJuTotalNote[i]);
		if (pCmd->iTotalNote[i] > 0)
		{
			ShowPlayerNote(i,true);
		}

		m_bNetCutPlayer[i] = pCmd->ibNetCutPlayer[i];
		if (m_bNetCutPlayer[i] == TRUE)
		{
			m_iDeskCardCount[i]=0;
			memset(m_DeskCard[i],0,sizeof(BYTE)*5);
		}

		BYTE icard[5] = {0};
		CopyMemory(icard,&m_DeskCard[i],sizeof(BYTE)*5);
		ShowHandCard(i, icard, m_iDeskCardCount[i], true);

		m_iUserStation[i] = pCmd->iUserStation[i];
		m_iSendCardTotle+=m_iDeskCardCount[i];
	}
          

	m_iNeedSendCardTotle=m_iSendCardTotle ;

	m_iThisNoteTimes = pCmd->tag.bNoteTimes;

	m_iNowBigNote = pCmd->tag.iNowBigNote;

	//显示最大牌玩家位置
	ShowCardMaxMark(true);

	SetNoteButText(m_iBaseNote);
	//设置总押注情况
	ShowTotalNote(true);

	//设置时间
	if (m_iNowOutPeople != -1) 
		SetGameTimer(m_iNowOutPeople);

//////////////////////

	// qy //设置押注上限
	if (1 == m_iThisNoteTimes)  // 第一轮
	{
		m_iThisTurnLimit = m_iLimitNote/4;
	}
	else if (2 == m_iThisNoteTimes)
	{
		m_iThisTurnLimit = m_iLimitNote/2;
	}
	else
	{
		m_iThisTurnLimit = (m_iLimitNote - m_iLimitNote/2 - m_iLimitNote/4-m_iBaseNote);
	}

	if (m_iNowOutPeople == m_bMyDesk)
	{					
		BYTE style = pCmd->tag.iNowBigNoteStyle;			//上一玩家最大下注类型
		m_byCurStyle = style;                           // 保存下注类型
		BOOL show = m_iThisNoteTimes >= m_bShowHa;					//第四轮才可以梭

		BOOL add;

		add = (style != TYPE_SHUTTLE && m_iNowBigNote >= 0);

		BOOL note = (m_iNowBigNote == 0);					//下注,只有在第一个下注者,或者本轮没有人下过注
		BOOL follow = (style != TYPE_SHUTTLE);	//没有人梭且下注额非0
		BOOL giveup = 1;
		if (m_bPK)
		{
			giveup= ((m_iThisNoteTimes > m_byUserFollow) && (m_byCurStyle != TYPE_SHUTTLE));
		}
		else
		{
			giveup= ((m_iThisNoteTimes > m_byUserFollow) || (m_byCurStyle == TYPE_SHUTTLE));
		}

		// 大于本论限注，不能加注
		if (m_iNowBigNote >= m_iThisTurnLimit)
		{
			add = FALSE;
		}
		//超过押注次数了 lym 2011-12-13
		if(m_byAddNum>0 && m_byAddNum!=255)
		{
			if (pCmd->m_byUserAddNum[m_bMyDesk] >= m_byAddNum)
			{
				add = FALSE;


			}
			//第一个不能加
			if (m_iFirstOutPeople == m_bMyDesk && !pCmd->tag.bNewTurn)
			{
				add = FALSE;
				if (pCmd->m_byUserAddNum[m_bMyDesk]  == 0)
					show = FALSE;
			}
		}

		// 小于2不能梭哈
		if (m_iThisNoteTimes < m_bShowHa)
		{
			show = FALSE;
		}

		if (m_bPK)
		{
			ShowNoteBtnEnable(0,1,0,1);
		}

		// 其他人梭哈时，是可以梭哈的
		if (TYPE_SHUTTLE == style)
		{
			show = FALSE;
			follow = TRUE;
		}

		ShowNoteBtnEnable(follow,show,add,giveup);
	}


	////////////////////////////////
	return;
}

void CNewGameClient::SetGameStat_GameStation_5(void * pBuffer,int nLen)
{
	GameStation_5 *pCmd = (GameStation_5 *) pBuffer;


	if (NULL == pCmd)
	{
		return;
	}

	memcpy(m_bUserState,pCmd->bUserState, sizeof(m_bUserState));
	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		if (pCmd->bAgree[i])
		{
			ShowAgree(i);
		}
	}

	//显示开始按钮 
	if (m_pGameFrame->GetGameInfo()->dwRoomRule & GRR_QUEUE_GAME)
	{
		m_MyUI.ShowControl(CTN_1000000_BTN_10001, false);//准备按钮隐藏
	}
	else
	{
		m_pGameFrame->SetTimer(ID_BEGIN_TIME,m_iBeginTime * 1000);
		m_MyUI.ShowControl(CTN_1000000_BTN_10001, true);//准备按钮显示
		m_MyUI.ShowControl(CTN_1000000_CTN_1010000_CTN_1010001 + ViewStation(m_bMyDesk), true);
		m_MyUI.SetClock(CTN_1000000_CTN_1010000_CTN_1010001_Time_10100001 + ViewStation(m_bMyDesk), m_iBeginTime, true);
	}

	return;
}



///设置游戏不同状态时的数据内容，主要用于断线重连
void CNewGameClient::SetGameStation(void * pBuffer,int nLen)
{
	InitGameUI();
	InitGameData();
	
	m_bMyDesk = m_pGameFrame->GetMyDeskStation();

	if(m_SoundEngine)
	{
		m_SoundEngine->setAllSoundsPaused(true);
		m_SoundEngine->stopAllSounds();
	}
	if (m_bSound)
	{
		PlaySceneSound();
	}
	GameStation_Base *pGameStation = (GameStation_Base *)pBuffer;
	if (NULL == pGameStation)
	{
		return;
	}
	//刷新累积成绩
	UserInfoForGame tUserInfo;
	memset(&tUserInfo,0,sizeof(tUserInfo));
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(m_pGameFrame->GetUserInfo(i,tUserInfo))
		{
			SetTextShowText(CTN_1000000_CTN_2004042_TEXT_10150028+i,tUserInfo.szNickName, true);
			SetTextinfo(CTN_1000000_CTN_2004042_TEXT_10150033+i,pGameStation->i64UpGrade[i], true);
			SetTextinfo(CTN_1000000_CTN_2004042_TEXT_10150038+i,pGameStation->i64AcculateGrade[i], true);
		}
	}
	SetGameStat_GameStation_Base(pBuffer, nLen);
	CString sdp;
	sdp.Format("sdp_ 客户端 SetGameStation %d", GetStationParameter());
	OutputDebugString(sdp);

	switch(GetStationParameter())
	{
	case GS_WAIT_SETGAME:
	case GS_WAIT_ARGEE:
		{
			SetGameStat_GameStation_2(pBuffer, nLen);

			break;
		}
	case GS_SEND_CARD:
		{
			SetGameStat_GameStation_3(pBuffer, nLen);
			break;
		}
	case GS_PLAY_GAME:
		{
			SetGameStat_GameStation_4(pBuffer, nLen);
			break;
		}
	case GS_WAIT_NEXT:
		{
			SetGameStat_GameStation_5(pBuffer, nLen);
			break;
		}
	}
}

/// 游戏消息
int CNewGameClient::HandleGameMessage(WORD nMainID,WORD nSubId,BYTE * buffer,int nLen)
{

	/*
	存在客户端主动发给服务端的消息ID
	case ASS_UG_USER_SET://50用户设置游戏
	case ASS_VREB_CHECK://61用户处理
	case ASS_REPLACE_VREB_CHECK://62代替(断线)用户处理
	case ASS_HAVE_THING://100
	case ASS_LEFT_RESULT://101
	case ASS_STOP_THING://103
	case ASS_AGREE_STOP://104
	case ASS_BEGIN_FLASH://106 
	*/
	switch(nSubId)
	{
	case ASS_CUT_END://用户强行离开
		{
			OnHandleASS_CUT_END(buffer, nLen);
			break;
		}
	case ASS_GM_AGREE_GAME:
		{
			OnHandleGM_AGREE_GAME(buffer, nLen);
			break;
		}
	case ASS_GM_GAME_STATION:           ///<设置状态
		{			    
			SetGameStation(buffer,nLen);
			break;
		}
	case ASS_GAME_BEGIN://52游戏开始
		{
			OnHandleGM_ASS_GAME_BEGIN(buffer, nLen);
		}
		break;
	case ASS_SEND_CARD://53发牌信息
		{
			OnHandleGM_ASS_SEND_CARD(buffer, nLen);
		}
		break;
	case ASS_SEND_FINISH://54发牌完成
		{
			OnHandleGM_ASS_SEND_FINISH(buffer, nLen);
		}
		break;
	case ASS_GAME_PLAY://55开始游戏
		{
			OnHandleGM_ASS_GAME_PLAY(buffer, nLen);
		}
		break;
	case ASS_NOTE://60下注
		{
			OnHandleGM_ASS_NOTE(buffer, nLen);
		}
		break;
	case ASS_NOTE_RESULT://63下注结果
		{
			OnHandleGM_ASS_NOTE_RESULT(buffer, nLen);
		}
		break;
	case ASS_COLLECT_JETON://70收集筹码
		{
			OnHandleGM_ASS_COLLECT_JETON(buffer, nLen);
		}
		break;
	case ASS_AI_STATION://71弃牌
		{
			OnHandleGM_ASS_AI_STATION(buffer, nLen);
		}
		break;
	case ASS_SEND_A_CARD://81新发一张牌
		{
			OnHandleGM_ASS_SEND_A_CARD(buffer, nLen);
		}
		break;
	case ASS_CONTINUE_END://90游戏结束
	case ASS_NO_CONTINUE_END:
		{
			OnHandleGM_ASS_CONTINUE_END(buffer, nLen);
		}
		break;
	case ASS_SALE_END://93游戏安全结束
		{
			OnHandleGM_ASS_SALE_END(buffer, nLen);
		}
		break;
	case ASS_NO_WINER://94游戏结束无胜出者
	case ASS_AHEAD_END://95提前结束
		{
			OnHandleGM_ASS_NO_WINER(buffer, nLen);
		}
		break;
	case ASS_COME_DESK:
		{
			if(nLen!=sizeof(NotifyComeMes))
			{
				return 0;
			}	
			NotifyComeMes * pComeDesk = (NotifyComeMes *)buffer;
			if (NULL == pComeDesk)
			{
				return 0;
			}
			if(255!=pComeDesk->byDeskStation)
			{
				UserInfoForGame tUserInfo;
				memset(&tUserInfo,0,sizeof(tUserInfo));
				if(m_pGameFrame->GetUserInfo(pComeDesk->byDeskStation,tUserInfo))
				{
					SetTextShowText(CTN_1000000_CTN_2004042_TEXT_10150028+pComeDesk->byDeskStation,tUserInfo.szNickName, true);
					SetTextinfo(CTN_1000000_CTN_2004042_TEXT_10150033+pComeDesk->byDeskStation,pComeDesk->i64UpGrade, true);
					SetTextinfo(CTN_1000000_CTN_2004042_TEXT_10150038+pComeDesk->byDeskStation,pComeDesk->i64AcculateGrade, true);
				}
			}
			break;
		}
	case ASS_LEAVE_DESK://用户离开游戏桌他妈的
		{
			
			if(nLen!=sizeof(LeaveDeskMes))
			{
				return 0;
			}	
			LeaveDeskMes * pLeaveDesk = (LeaveDeskMes *)buffer;
			if (NULL == pLeaveDesk)
			{
				return 0;
			}
			
			if(255!=pLeaveDesk->byDeskStation)
			{
				SetControlVisible(CTN_1000000_CTN_2004042_TEXT_10150028+pLeaveDesk->byDeskStation,false);
				SetControlVisible(CTN_1000000_CTN_2004042_TEXT_10150033+pLeaveDesk->byDeskStation,false);
				SetControlVisible(CTN_1000000_CTN_2004042_TEXT_10150038+pLeaveDesk->byDeskStation,false);
			}
			break;
		}
	case ASS_NEW_TURN://新一轮游戏开始
		{
			if(nLen!=sizeof(NewTurn))
			{
				return 0;
			}	
			NewTurn * pNewTurn = (NewTurn *)buffer;
			if (NULL == pNewTurn)
			{
				return 0;
			}
			m_bNewTurn=true;
			break;
		}
	default:
		{
			break;
		}
	}

	return TRUE;
}
/// 界面消息
int CNewGameClient::OnUIMessage(TUIMessage* pMessage)
{
	CString cs;
	cs.Format("lwlog::pMessage->_uMessageType=%d",pMessage->_uMessageType);
	//OutputDebugString(cs);
	if(pMessage->_uMessageType == UI_LBUTTONUP)
	{
		switch(pMessage->_uControlID)
		{
		case CTN_1000000_CTN_1015000_NoMoveCard_10150023://看牌
			{
				if (GetStationParameter() == GS_WAIT_BACK || GetStationParameter() == GS_PLAY_GAME)
				{
					int x=0,y=0,iMinx=0,iMax=0;
					m_MyUI.GetControlGameXY(CTN_1000000_CTN_1015000_NoMoveCard_10150023,x,y);//获得手牌控件位置
					iMinx=x+((5-m_iDeskCardCount[m_pGameFrame->GetMyDeskStation()])*8);
					iMax=iMinx+15;
					
					if(pMessage->_mouseX>=iMinx && pMessage->_mouseX<=iMax)
					{
						
						m_pGameFrame->KillTimer(ID_TIME_SEE_CARD);
						BYTE icard[5] = {0};
						CopyMemory(icard,&m_UserCard[m_pGameFrame->GetMyDeskStation()],sizeof(BYTE)*5);
						ShowHandCard(m_pGameFrame->GetMyDeskStation(), icard, m_iDeskCardCount[m_pGameFrame->GetMyDeskStation()], true);
						m_pGameFrame->SetTimer(ID_TIME_SEE_CARD, 2*1000);
					}
				}
				break;
			}
		case CTN_1000000_RButton_10150047:///<自动开始  单选框 
			{
				if(m_bAutoStart)
				{
					bool bSelected=false;
					GetRButtonSelected(CTN_1000000_RButton_10150047,&bSelected);
					if(bSelected)
					{
						m_pGameFrame->SetTimer(ID_TIME_AUTOSTART,5*1000);
					}
				}
				break;
			}
		case CTN_1000000_RButton_10150048:///<自动看牌  单选框 
			{
				if (GetStationParameter() == GS_WAIT_BACK || GetStationParameter() == GS_PLAY_GAME)
				{
					
					bool bSelected=false;
					GetRButtonSelected(CTN_1000000_RButton_10150048,&bSelected);
					if(bSelected)
					{
						m_pGameFrame->KillTimer(ID_TIME_SEE_CARD);
						BYTE icard[5] = {0};
						CopyMemory(icard,&m_UserCard[m_bMyDesk],sizeof(BYTE)*5);
						ShowHandCard(m_bMyDesk, icard, m_iDeskCardCount[m_bMyDesk], true);
						m_pGameFrame->SetTimer(ID_TIME_SEE_CARD, 2000);
					}
					
				}
				break;
			}
		case CTN_1000000_BTN_2004043://关闭成绩
			{
				SetControlVisible(CTN_1000000_CTN_2004042,false);
				SetControlVisible(CTN_1000000_BTN_2004043,false);
				SetControlVisible(CTN_1000000_BTN_2004044,true);
				break;
			}
		case CTN_1000000_BTN_2004044://打开成绩
			{
				SetControlVisible(CTN_1000000_CTN_2004042,true);
				SetControlVisible(CTN_1000000_BTN_2004043,true);
				SetControlVisible(CTN_1000000_BTN_2004044,false);
				break;
			}
		}
	}
	if (pMessage->_uControlID == CTN_1000000_CTN_100210735_CTN_1002202 && pMessage->_uMessageType == UI_LBUTTONUP)
	{
		if (m_bSendCard)
		{
			return 0;
		}
		if (GetStationParameter() == GS_WAIT_BACK || GetStationParameter() == GS_PLAY_GAME)
		{
			m_pGameFrame->KillTimer(ID_TIME_SEE_CARD);
			BYTE icard[5] = {0};
			CopyMemory(icard,&m_UserCard[m_bMyDesk],sizeof(BYTE)*5);
			ShowHandCard(m_bMyDesk, icard, m_iDeskCardCount[m_bMyDesk], true);
			m_pGameFrame->SetTimer(ID_TIME_SEE_CARD, 2000);
		}
	}

	switch(pMessage->_uMessageType)
	{
	case UI_CATOONFINISH://<动画播放完成
		{
			OnUIMess_CATOONFINISH(pMessage->_uControlID);
		}
		break;
	case UI_LBUTTONDOWN:	//鼠标按下
		{
			
			POINT pPT;
			pPT.x = pMessage->_mouseX;
			pPT.y = pMessage->_mouseY;
			
			if (m_crMoveRect.PtInRegion(pPT))
			{
				m_ptMouse.x=pMessage->_mouseX;
				m_ptMouse.y=pMessage->_mouseY;
				m_bCanMove = true;
				SetImageSrcX(CTN_1000000_CTN_1012000_CTN_1012100_IMG_10121003,17);
			}
			else
			{
				if (pMessage->_uControlID == CTN_1000000_CTN_1012000_CTN_1012100_IMG_10121001)
				{
					OutputDebugString("dwjlogsh::鼠标点击了");
				}
			}
			break;
		}
	case UI_LBUTTONUP://鼠标左键弹起
		{
			if (m_bIsWatching)
			{
				return 0;
			}
			
			//if(m_MyUI.GetControlShow(CTN_1000000_CTN_1012000_CTN_1012100_BTN_10121002))
			//{
				m_bCanMove = false;
			//}
			//SetImageSrcX(CTN_1000000_CTN_1012000_CTN_1012100_IMG_10121003,0);
			OnUIMess_LBUTTONUP(pMessage->_uControlID);
			break;
		}
	case UI_MOUSEENTER://鼠标移入/移出控件
		{
			OnUIMess_MOUSEENTER(pMessage->_uControlID);
			
			break;
		}
	case UI_MOUSEMOVE:	//鼠标移动
		{
			//if (pMessage->_uControlID == 10121001 || pMessage->_uControlID == CTN_1000000_CTN_1012000_CTN_1012100_IMG_10121003)
			if(CTN_1000000_IMG_120120088==pMessage->_uControlID)
			{
				
				if (m_bCanMove)
				{
					int iMoveX = pMessage->_mouseX-m_ptMouse.x;
					int iMoveY = pMessage->_mouseY-m_ptMouse.y;

					POINT ptImage = GetImagePosition(CTN_1000000_CTN_1012000_CTN_1012100_IMG_10121003);
					if (ptImage.x+iMoveX <= 685)
					{
						SetImagePosition(CTN_1000000_CTN_1012000_CTN_1012100_IMG_10121003,685,640);
						m_crMoveRect.SetRectRgn(685,640,760,640+27);
						m_MyUI.EnableControl(CTN_1000000_CTN_1012000_CTN_1012100_BTN_10121002,true);
						if(m_bNewTurn)
						{
							m_iSelAddMoney = m_iBaseNote;
						}
						else
						{
							m_iSelAddMoney=0;
						}
						
						m_MyUI.SetTextForBut( CTN_1000000_CTN_1012000_CTN_1012100_BTN_10121002, m_iSelAddMoney);

					}
					else if (ptImage.x+iMoveX >= 760)
					{
						SetImagePosition(CTN_1000000_CTN_1012000_CTN_1012100_IMG_10121003,760,640);
						m_crMoveRect.SetRectRgn(685,640,773,640+27);
						m_MyUI.EnableControl(CTN_1000000_CTN_1012000_CTN_1012100_BTN_10121002,true);
						if(m_bNewTurn)
						{
							m_iSelAddMoney = m_iLimitNote;
						}
						else
						{
							m_iSelAddMoney=m_iTempBt[3];
						}
						
						m_MyUI.SetTextForBut( CTN_1000000_CTN_1012000_CTN_1012100_BTN_10121002, m_iSelAddMoney);
					}
					else
					{
						SetImagePosition(CTN_1000000_CTN_1012000_CTN_1012100_IMG_10121003,ptImage.x+iMoveX,640);
						m_crMoveRect.SetRectRgn(ptImage.x+iMoveX,640,ptImage.x+iMoveX+26,640+27);
						m_MyUI.EnableControl(CTN_1000000_CTN_1012000_CTN_1012100_BTN_10121002,true);

						//获取移动的位置
						int iX = (ptImage.x+iMoveX)-685;
						if (iX == 0)
						{
							if(m_bNewTurn)
							{
								m_iSelAddMoney = m_iBaseNote;
							}
							else
							{
								m_iSelAddMoney = 0;
							}
							
						}
						else if (iX == 200)
						{
							if(m_bNewTurn)
							{
								m_iSelAddMoney = m_iLimitNote;
							}
							else
							{
								m_iSelAddMoney = m_iTempBt[3];
							}
							
						}
						else
						{
							if(m_bNewTurn)
							{
								m_iSelAddMoney = m_iBaseNote + (((m_iLimitNote-m_iBaseNote)/200)*iX);
							}
							else
							{
								m_iSelAddMoney = 0 + (((m_iTempBt[3]-0)/200)*iX);
							}
							
						}
						
						m_MyUI.SetTextForBut( CTN_1000000_CTN_1012000_CTN_1012100_BTN_10121002, m_iSelAddMoney);
					}

					//重新记录鼠标的位置
					m_ptMouse.x=pMessage->_mouseX;
					m_ptMouse.y=pMessage->_mouseY;
				}
			}
			else
			{
				
				m_bCanMove = false;
				//SetImageSrcX(CTN_1000000_CTN_1012000_CTN_1012100_IMG_10121003,0);
			}
			
			break;
		}
	default:
		break;
	}

	return 0;
}
//动画播放完成消息
void CNewGameClient::OnUIMess_CATOONFINISH(UINT32 uControlID)
{
	switch(uControlID)
	{
	case CTN_1000000_CTN_1015000_MOVE_10150001:
		{
			m_MyUI.ShowControl(CTN_1000000_CTN_1015000_NoMoveCard_10150011, false);
			int ix = 0;
			int iy = 0;
			m_MyUI.GetControlGameXY(CTN_1000000_CTN_1015000_NoMoveCard_10150011, ix, iy);

			for (int i = 0; i < PLAY_COUNT; ++i)
			{
				int _ix = 0;
				int _iy = 0;

				m_MyUI.GetControlGameXY(CTN_1000000_CTN_1015000_NoMoveCard_10150021 + i, _ix, _iy);
				int iw = 0;
				int ih = 0;
				m_MyUI.GetControlGameWH(CTN_1000000_CTN_1015000_NoMoveCard_10150021 + i, iw, ih);

				CString sdp;
				sdp.Format("sdp_ 发一张牌 动画 [%d,%d] [%d,%d] [iw=%d]",ix,iy,_ix,_iy,iw / 2 - 20);
				OutputDebugString(sdp);

				if ((_ix + iw / 2 - 20) == ix && _iy == iy)
				{
					BYTE icard[5] = {0};
					int	iDesk = m_pGameFrame->ViewStation2DeskStation(i);
					CopyMemory(icard,&m_DeskCard[iDesk],sizeof(BYTE)*5);
					ShowHandCard(iDesk, icard, m_iSendCardNum[iDesk], true);

					sdp.Format("sdp_ 发一张牌 动画 玩家ID[%d]牌张数=%d,%d",i,m_iDeskCardCount[i],m_iSendCardNum[i]);
					OutputDebugString(sdp);


					break;
				}
			}
		}
		break;
	case CTN_1000000_CTN_1012000_MOVE_10120001://用于移动图片
		{
			m_MyUI.ShowControl(CTN_1000000_CTN_1012000_IMG_10120012, false);
			int ix = 0;
			int iy = 0;
			m_MyUI.GetControlGameXY(CTN_1000000_CTN_1012000_IMG_10120012, ix, iy);

			for (int i = 0; i < PLAY_COUNT; ++i)
			{
				int _ix = 0;
				int _iy = 0;

				m_MyUI.GetControlGameXY(CTN_1000000_CTN_1012000_IMG_10120021 + i, _ix, _iy);

				if (_ix == ix && _iy == iy)
				{
					//显示玩家下的注
					ShowPlayerNote(m_pGameFrame->ViewStation2DeskStation(i),true);
					break;
				}
			}
		}
		break;
	case CTN_1000000_CTN_1012000_MOVE_10120002://用于移动文字
		{
			m_MyUI.ShowControl(CTN_1000000_CTN_1012000_TEXT_10120011, false);
			int ix = 0;
			int iy = 0;
			m_MyUI.GetControlGameXY(CTN_1000000_CTN_1012000_TEXT_10120011, ix, iy);

			for (int i = 0; i < PLAY_COUNT; ++i)
			{
				int _ix = 0;
				int _iy = 0;

				m_MyUI.GetControlGameXY(CTN_1000000_CTN_1012000_TEXT_10120031 + i, _ix, _iy);

				if (_ix == ix && _iy == iy)
				{
					//显示玩家下的注
					ShowPlayerNote(m_pGameFrame->ViewStation2DeskStation(i),true);
					break;
				}
			}
		}
		break;
	default:
		break;
	}
	return;
}
//鼠标左键弹起消息
void CNewGameClient::OnUIMess_LBUTTONUP(UINT32 uControlID)
{
	switch(uControlID)
	{
	case CTN_1000000_CTN_1001000_BTN_1001100:
		{
			m_pGameFrame->KillTimer(ID_TIME_AUTOSTART);
			m_bAutoStart=false;
			KillAllTimer();
			m_pGameFrame->ClickContinue();
			InitGameData();
			InitGameUI();
		}
		break;
	case CTN_1000000_CTN_1001000_BTN_1001200:
		{
			m_pGameFrame->CloseClient();
		}
		break;
	case CTN_1000000_CTN_1012000_BTN_10120041:///<按扭1跟
		{
			OnHitActiveResult(TYPE_FOLLOW, 0);
		}
		break;
	case CTN_1000000_CTN_1012000_BTN_10120042:///<按扭2全押
		{
			OnHitActiveResult(TYPE_SHUTTLE, 0);
		}
		break;
	case CTN_1000000_CTN_1012000_BTN_120120040:///<按扭3下注
	case CTN_1000000_CTN_1012000_BTN_120120041:///<按扭4下注
	case CTN_1000000_CTN_1012000_BTN_120120042:///<按扭5下注
	case CTN_1000000_CTN_1012000_BTN_120120043:
		{
			int itag = uControlID - CTN_1000000_CTN_1012000_BTN_120120040;
			if(m_bNewTurn)
			{
				OnSelectNote(m_iAddButtonNum[itag]);
			}
			else
			{
				OnSelectNote(m_iTempBt[itag]);
			}
			
		}
		break;
	case CTN_1000000_CTN_1012000_CTN_1012100_BTN_10121002:	//拖动下注
		{
			if (m_iSelAddMoney == m_iLimitNote)
			{
				OnHitActiveResult(TYPE_SHUTTLE, 0);
			}
			else
			{
				if(m_iSelAddMoney>0)
				{
					OnSelectNote(m_iSelAddMoney);
				}
				
			}
			break;
		}
	case CTN_1000000_CTN_1012000_BTN_10120046:///<按扭6放弃
		{
			OnHitActiveResult(TYPE_GIVE_UP, 0);
		}
		break;
	case CTN_2_BTN_1003500:
		{
			OutputDebugString("sdp_ 设置面板 点击设置按钮 显示设置面板");
			ShowInitSet(true);
			break;
		}
	case CTN_1000000_CTN_1003510_BTN_1003540://ok
		{
			bool bOldBSound = m_bSound;
			m_bSound = m_MyUI.GetIsSelect(CTN_1000000_CTN_1003510_RButton_1003520);
			OutputDebugString("sdp_ 设置面板 点击OK 隐藏设置面板");
			ShowInitSet(false);

			CString s = CBcfFile::GetAppPath ();/////本地路径

			DWORD dwCfgFile = ::cfgOpenFile(s + GET_SKIN_FOLDER(szTempStr) +".bcf");

			if(dwCfgFile >=0x10) //文件打开成功
			{
				//声音开关配置
				::cfgSetValue(dwCfgFile,"Operate","Sound",m_bSound);

				::cfgClose(dwCfgFile);						
			}

			if (m_bSound)//开启声音
			{
				if (bOldBSound != m_bSound)//设置存在改变
				{
					if(m_SoundEngine)
						m_SoundEngine->setAllSoundsPaused(false);
					PlaySceneSound();
				}
			} 
			else//关闭声音
			{
				if (bOldBSound != m_bSound)//设置存在改变
				{
					if(m_SoundEngine)
					{
						m_SoundEngine->setAllSoundsPaused(true);
						m_SoundEngine->stopAllSounds();
					}
				}
			}


			break;
		}
	case CTN_1000000_CTN_1003510_BTN_1003550://取消
		{
			m_MyUI.SetIsSelect(CTN_1000000_CTN_1003510_RButton_1003520, m_bSound);
			OutputDebugString("sdp_ 设置面板 点击取消 隐藏设置面板");
			ShowInitSet(false);
		}
		break;
	default:
		{
			break;
		}
	}
	return;
}

//鼠标移入/移出控件消息
void CNewGameClient::OnUIMess_MOUSEENTER(UINT32 uControlID)
{
	if (m_MyUI.IsBut(uControlID))
	{
		PlaySound(SOUND_MOVEBUTTON);
	}
	return;
}

//显示设置框
void CNewGameClient::ShowInitSet(bool bFlag)
{
	m_MyUI.ShowControl(CTN_1000000_CTN_1003510, bFlag);
	m_MyUI.ShowControl(CTN_1000000_CTN_1003510_BTN_1003540, bFlag);
	m_MyUI.ShowControl(CTN_1000000_CTN_1003510_BTN_1003550, bFlag);

	m_MyUI.SetIsSelect(CTN_1000000_CTN_1003510_RButton_1003520, m_bSound);
	m_MyUI.ShowControl(CTN_1000000_CTN_1003510_RButton_1003520, bFlag);

	m_MyUI.ShowControl(CTN_1000000_CTN_1003510_TEXT_1003521, bFlag);

	return;
}

/// 
int CNewGameClient::ResetGameFrame(void)
{
	InitGameUI();
	return 0;
}
/// 玩家进入
int CNewGameClient::GameUserCome()
{
	return 0;
}
/// 玩家离开
int CNewGameClient::GameUserLeft(int nIndex)
{
	int iView = ViewStation(nIndex);
	m_MyUI.ShowControl(CTN_1000000_CTN_1014000_IMG_10140001 + iView, false);//准备标志
	SetControlVisible(CTN_1000000_CTN_120120073+iView,false);
	SetControlVisible(CTN_1000000_IMG_120120047+iView,false);
	SetControlVisible(CTN_1000000_TEXT_120120052+iView,false);
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
	return true;
}
/// 定时器消息
void CNewGameClient::OnTimer(int nTimerID)
{
	CString sdp;

	switch(nTimerID)
	{
	case ID_TIME_AUTOSTART://自动开始消息
		{
			m_pGameFrame->KillTimer(ID_TIME_AUTOSTART);
			KillAllTimer();
			m_pGameFrame->ClickContinue();
			InitGameData();
			InitGameUI();
			break;
		}
	case ID_TIME_TIP://提示
		{
			m_pGameFrame->KillTimer(ID_TIME_TIP);
			//提示容器
			for(int i=0;i<5;i++)
			{
				SetControlVisible(CTN_1000000_CTN_120120062+i,false);
			}
			break;
		}
	case ID_TIME_SEE_CARD:
		{
			m_pGameFrame->KillTimer(ID_TIME_SEE_CARD);
			BYTE icard[5] = {0};
			CopyMemory(icard,&m_DeskCard[m_bMyDesk],sizeof(BYTE)*5);
			ShowHandCard(m_bMyDesk, icard, m_iDeskCardCount[m_bMyDesk], true);	
		}
		break;
	case ID_BEGIN_TIME:
		{
			m_pGameFrame->KillTimer(ID_BEGIN_TIME);
			if (!m_bIsWatching)
			{
				m_pGameFrame->ClickContinue();
			}
			InitGameData();
			InitGameUI();
			break;
		}
	case ID_END_TIME:
		{
			m_pGameFrame->KillTimer(ID_END_TIME);
			InitGameData();
			InitGameUI();
			m_pGameFrame->CloseClient();
			break;
		}
		break;
	case ID_TIME_THINK:
		{
			KillGameTimer(m_iNowOutPeople);
		}
		break;
	case ID_TIME_BASENOTE:
		{
			m_pGameFrame->KillTimer(ID_TIME_BASENOTE);
			if (IsPlayer(m_Time_BASENOTE_Count))
			{
				//玩家下注动画
				ShowNoteAnima(m_Time_BASENOTE_Count, m_iBaseNote, 100);
			}
			if (m_Time_BASENOTE_Count >= PLAY_COUNT)//已经超过玩家总人数
			{
				return;
			}

			m_Time_BASENOTE_Count++;
			if (IsPlayer(m_Time_BASENOTE_Count))
			{
				m_pGameFrame->SetTimer(ID_TIME_BASENOTE,800);
			}
			else
			{
				m_pGameFrame->SetTimer(ID_TIME_BASENOTE,10);
			}

			break;
		}
	case ID_TIME_SEND_CARD://发牌动画
		{
			m_pGameFrame->KillTimer(ID_TIME_SEND_CARD);
			OutputDebugString("dwjsh::ID_TIME_SEND_CARD（0）");
			if (m_iSendCardTotle >= m_iNeedSendCardTotle)//发完了
			{
				m_bSendCard = false;
				break;
			}
			OutputDebugString("dwjsh::ID_TIME_SEND_CARD（1）");
			if (!IsPlayer(m_iSendAnimaCardPos) || m_iUserStation[m_iSendAnimaCardPos] == TYPE_GIVE_UP)//该位置上玩家不存在 或 已经弃牌，则迅速跳到下家发牌
			{
				OutputDebugString("dwjsh::ID_TIME_SEND_CARD（2）");
				m_iSendAnimaCardPos = (m_iSendAnimaCardPos - 1 + PLAY_COUNT) % PLAY_COUNT;//逆时针发牌
				m_pGameFrame->SetTimer(ID_TIME_SEND_CARD, 20);
				break;
			}
			OutputDebugString("dwjsh::ID_TIME_SEND_CARD（3）");
			PlaySound(SOUND_SEND_CARD);
			int iView = ViewStation(m_iSendAnimaCardPos);
			BYTE icard[1] = {0};
			icard[0] = m_DeskCard[m_iSendAnimaCardPos][m_iSendCardNum[m_iSendAnimaCardPos]];
			m_iSendCardNum[m_iSendAnimaCardPos]++;
			m_MyUI.PlayCardInAnima(CTN_1000000_CTN_1015000_MOVE_10150001, 
				CTN_1000000_CTN_1015000_MOVE_10150001, 
				CTN_1000000_CTN_1015000_NoMoveCard_10150021, 
				CTN_1000000_CTN_1015000_NoMoveCard_10150011, 
				iView, 
				icard, 
				1, 
				200, 
				true);
			OutputDebugString("dwjsh::ID_TIME_SEND_CARD（4）");
			m_iSendCardTotle++;
			m_iSendAnimaCardPos = (m_iSendAnimaCardPos - 1 + PLAY_COUNT) % PLAY_COUNT;//逆时针发牌

			m_pGameFrame->SetTimer(ID_TIME_SEND_CARD, 400);
		}
	default:
		{
			break;
		}
	}
}

/// 设置游戏置状态
void CNewGameClient::SetStationParameter(BYTE bGameStation)
{
	m_bGameStation = bGameStation;
	if (m_pGameFrame)
		m_pGameFrame->SetStationParameter(bGameStation);
}

/// 获取游戏状态
BYTE CNewGameClient::GetStationParameter(void)
{
	return m_bGameStation;
}

int CNewGameClient::ViewStation(int desk)
{
	if (NULL != m_pGameFrame)
	{
		return m_pGameFrame->DeskStation2View(desk);
	}

	return 0;


}

//是否存在该玩家
bool CNewGameClient::IsPlayer(BYTE bDeskStation)
{
	UserInfoForGame UserInfo;
	if (NULL != m_pGameFrame)
	{
		if (m_pGameFrame->GetUserInfo(bDeskStation, UserInfo) && m_bUserState[bDeskStation])
		{
			return true;
		}
	}

	return false;
}


///同意游戏
bool CNewGameClient::OnControlHitBegin()
{
	//if (m_bInLisureRoom)//该游戏是否在悠闲房
	//{
	//	MSG_GM_S_ClientInfo ClientInfo;
	//	ClientInfo.bEnableWatch=false;
	//	m_pGameFrame->SendGameData(&ClientInfo,sizeof(ClientInfo),MDM_GM_GAME_FRAME,ASS_GM_GAME_INFO,0);
	//}
	//else
	{
		m_pGameFrame->SendGameData(ASS_GM_AGREE_GAME,NULL,0);
	}
				
	return true;
}

//游戏逻辑数据重配
void CNewGameClient::InitGameData()
{
	m_bTimeOutCount = 0;
	m_iNowBigNote = 0;
	m_bSendCard = false;
	m_iThisNoteTimes = 0;			//押注轮数
	//房间信息
	m_iLimitNote     = 0;				//最大限额注
	 m_iBaseNote     = 0;				//底注
	m_iThisTurnLimit = 0;			//本轮限注
	::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));			//桌面扑克的数目
	memset(m_DeskCard,0,sizeof(m_DeskCard));
	memset(m_UserCard,0,sizeof(m_UserCard));
	memset(m_iThisGameNote,0,sizeof(m_iThisGameNote));
	m_iNoteTotalPoint = 0;
	m_iSendAnimaCardPos = 0;     ///发牌动画位置
	m_iSendCardTotle = 0;         ///发牌的张数
	m_iNeedSendCardTotle = 0;	 ///当前需要发的牌数目
	ZeroMemory(m_iSendCardNum,sizeof(m_iSendCardNum));
	SetNoteButText(m_iBaseNote);
	::memset(m_iThisTurnNote,0,sizeof(m_iThisTurnNote));
	::memset(m_iPerJuTotalNote,0,sizeof(m_iPerJuTotalNote));
	m_iNowOutPeople = -1;
	m_iFirstOutPeople=-1;
	memset(m_iThisTurnNote,0,sizeof(m_iThisTurnNote));
	m_bTimeOutCount=0;
	m_bShowAnimalStation=0;
	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		m_iUserStation[i] = -1;
	}
	memset(m_szName,0,sizeof(m_szName));
	memset(m_iDoublePointTime,0,sizeof(m_iDoublePointTime));
	memset(m_iiProtectTime,0,sizeof(m_iiProtectTime));
	memset(m_TempNote,0,sizeof(m_TempNote));
	memset(m_iTempThisTurnNote,0,sizeof(m_iTempThisTurnNote));
	memset(m_bUserState,0,sizeof(m_bUserState));
	memset(m_iAddButtonNum,0,sizeof(m_iAddButtonNum));
	m_iSelAddMoney = 0;
	m_Time_BASENOTE_Count=0;
	m_byCurStyle=0;
	m_iFirstSendCardCount = 1;
	memset(m_bNetCutPlayer,0,sizeof(m_bNetCutPlayer));
}
//游戏UI重配
void CNewGameClient::InitGameUI()
{
	for (int i = 0; i< PLAY_COUNT; ++i)
	{
		m_MyUI.ShowControl(CTN_1000000_CTN_1010000_CTN_1010001 + i, false);
		m_MyUI.ShowControl(CTN_1000000_CTN_1010000_CTN_1010001_Time_10100001 + i, false);//计时器

		m_MyUI.ShowControl(CTN_1000000_CTN_1014000_IMG_10140001 + i, false);//准备标志
	}





	ShowNoteBtnEnable();
	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		ShowPlayerNote(i,false);
		BYTE iCard[5] = {0};
		ShowHandCard(i, iCard, 0, false);
	}

	ShowCardMaxMark(false);
	ShowTotalNote(false);

	GameFinishNotify szResultInfo;
	ShowResult(szResultInfo, false);


	for (int i = 0; i< 6; ++i)
	{
		m_MyUI.ShowControl(CTN_1000000_CTN_1012000_BTN_10120041 + i, true);
	}


	m_ptMouse.x=685;
	m_ptMouse.y=640;
	m_bCanMove = false;
	SetImagePosition(CTN_1000000_CTN_1012000_CTN_1012100_IMG_10121003,685,640);
	m_crMoveRect.SetRectRgn(685,640,760,640+27);
	m_MyUI.EnableControl(CTN_1000000_CTN_1012000_CTN_1012100_BTN_10121002,false);

	for(int i=0;i<10;i++)
	{
		SetControlVisible(CTN_1000000_IMG_120120047+i,false);
	}
	SetControlVisible(CTN_1000000_IMG_120120045,false);
	SetControlVisible(CTN_1000000_TEXT_120120046,false);
	SetControlVisible(CTN_1000000_CTN_1001000_BTN_1001100,false);
	//提示容器
	for(int i=0;i<PLAY_COUNT;i++)
	{
		SetControlVisible(CTN_1000000_CTN_120120062+i,false);
	}
	for(int i=0; i<PLAY_COUNT; i++)
	{
		SetControlVisible(CTN_1000000_CTN_120120073+i,false);
	}
	m_MyUI.EnableControl(CTN_1000000_CTN_1012000_CTN_1012100_BTN_10121002,false);
	SetImageOnMoseMove(CTN_1000000_IMG_120120088,false);
}

/// FlashWindowEx封闭）
void CNewGameClient::FlashWindow()
{
	HWND hwnd = (m_MyUI.GetpIUI()->GetWindowsHwnd());
	FLASHWINFO info;
	info.cbSize = sizeof(FLASHWINFO);
	info.dwFlags = FLASHW_TRAY|FLASHW_TIMERNOFG;
	info.hwnd = hwnd;
	info.uCount = 100;
	info.dwTimeout = 500;
	FlashWindowEx(&info);
}
/// 设定游戏定时器
/// @param BYTE bDesk 当前显示计时器的位置
void CNewGameClient::SetGameTimer(BYTE bDesk)
{
	m_pGameFrame->SetTimer(ID_TIME_THINK, m_iThinkTime * 1000);
	int iView = ViewStation(bDesk);
	m_MyUI.ShowControl(CTN_1000000_CTN_1010000_CTN_1010001 + iView, true);
	m_MyUI.SetClock(CTN_1000000_CTN_1010000_CTN_1010001_Time_10100001 + iView, m_iThinkTime, true);

	return;

}

/// 消除定时器
void CNewGameClient::KillGameTimer(BYTE bDesk)
{
	m_pGameFrame->KillTimer(ID_TIME_THINK);
	int iView = ViewStation(bDesk);
	m_MyUI.ShowControl(CTN_1000000_CTN_1010000_CTN_1010001 + iView, false);
	m_MyUI.SetClock(CTN_1000000_CTN_1010000_CTN_1010001_Time_10100001 + iView, 0, false);

	if (m_iNowOutPeople == m_bMyDesk)
	{
		ShowNoteBtnEnable();
	}
	return;
}

/// 显示 结算框
void CNewGameClient::ShowResult(GameFinishNotify szResultInfo, bool bFlag)
{
	//TCHAR				name[PLAY_COUNT][61];//昵称
	//BYTE				iCardShape[PLAY_COUNT];		//牌型	
	//__int64				iWardPoint[PLAY_COUNT];//得分
	//__int64				iMoney[PLAY_COUNT];//金币
	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		m_MyUI.ShowControl(CTN_1000000_CTN_1001000_TEXT_1001001 + i, false);
		m_MyUI.ShowControl(CTN_1000000_CTN_1001000_TEXT_1001011 + i, false);
		m_MyUI.ShowControl(CTN_1000000_CTN_1001000_IMG_1001021 + i, false);
		m_MyUI.ShowControl(CTN_1000000_CTN_1001000_TEXT_1001031 + i, false);
		m_MyUI.ShowControl(CTN_1000000_CTN_1001000_TEXT_1001041 + i, false);
	}
	m_MyUI.ShowControl(CTN_1000000_CTN_1001000_BTN_1001100, bFlag);
	m_MyUI.ShowControl(CTN_1000000_CTN_1001000_BTN_1001200, bFlag);

	if (!bFlag)
	{
		m_MyUI.ShowControl(CTN_1000000_CTN_1001000, false);
		return;
	}

	UserInfoForGame UserInfo;
	int iDesk = 0;
	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		if (!IsPlayer(i))
			continue;

		//昵称
		if (m_pGameFrame->GetUserInfo(i,UserInfo))
			m_MyUI.SetTextInfo(CTN_1000000_CTN_1001000_TEXT_1001001 + iDesk, UserInfo.szNickName, true);

		//牌型
		TCHAR _TcNickName[MAX_PATH] = {0};
		switch(szResultInfo.iCardShape[i])
		{
		case UG_ERROR_KIND:/**< 错误 */
			{
				sprintf(_TcNickName, "无");
			}
			break;
		case SH_OTHER:/**< 散牌 */
			{
				sprintf(_TcNickName, "散牌");
			}
			break;
		case SH_DOUBLE:/**< 对子 */
			{
				sprintf(_TcNickName, "对子");
			}
			break;
		case SH_TWO_DOUBLE:/**< 两对 */
			{
				sprintf(_TcNickName, "两对");
			}
			break;
		case SH_THREE:/**< 三条 */
			{
				sprintf(_TcNickName, "三条");
			}
			break;
		case SH_SMALL_CONTINUE:/**< 最小顺子 */
			{
				sprintf(_TcNickName, "最小顺子");
			}
			break;
		case SH_CONTINUE:/**< 顺子 */
			{
				sprintf(_TcNickName, "顺子");
			}
			break;
		case SH_SAME_HUA:/**< 同花 */
			{
				sprintf(_TcNickName, "同花");
			}
			break;
		case SH_HU_LU:/**< 葫芦 */
			{
				sprintf(_TcNickName, "葫芦");
			}
			break;
		case SH_TIE_ZHI:/**< 铁支 */
			{
				sprintf(_TcNickName, "铁支");
			}
			break;
		case SH_SMALL_SAME_HUA_CONTINUE:/**< 同花最小顺子 */
			{
				sprintf(_TcNickName, "同花最小顺子");
			}
			break;
		case SH_SAME_HUA_CONTINUE:/**< 同花顺 */
			{
				sprintf(_TcNickName, "同花顺");
			}
			break;
		default:
			{
				sprintf(_TcNickName, "无");
				break;
			}
		}

		m_MyUI.SetTextInfo(CTN_1000000_CTN_1001000_TEXT_1001011 + iDesk, _TcNickName, true);
		//胜负
		TCHAR TcNickName[MAX_PATH] = {0};
#ifdef UI2_5D
		if (szResultInfo.iMoney[i] > 0)
		{
			sprintf(TcNickName, "%s/resources/result/win.png", CBcfFile::GetAppPath());
		}
		else if(szResultInfo.iMoney[i] < 0)
		{
			sprintf(TcNickName, "%s/resources/result/lost.png", CBcfFile::GetAppPath());
		}
#else
		if (szResultInfo.iMoney[i] > 0)
		{
			sprintf(TcNickName, "%s/resources/result/win.png", CBcfFile::GetAppPath());
		}
		else if(szResultInfo.iMoney[i] < 0)
		{
			sprintf(TcNickName, "%s/resources/result/lost.png", CBcfFile::GetAppPath());
		}
#endif

		m_MyUI.SetImagesLoadPic(CTN_1000000_CTN_1001000_IMG_1001021 + iDesk, TcNickName, true);

		//积分
		m_MyUI.SetTextInfo(CTN_1000000_CTN_1001000_TEXT_1001031 + iDesk, szResultInfo.iWardPoint[i], true,true);
		//金币
		m_MyUI.SetTextInfo(CTN_1000000_CTN_1001000_TEXT_1001041 + iDesk, szResultInfo.iMoney[i], true,true);

		iDesk++;
	}

	if (m_pGameFrame->GetIsWatching())
	{
		m_MyUI.EnableControl(CTN_1000000_CTN_1001000_BTN_1001200,false);
	}
	else
	{
		m_MyUI.EnableControl(CTN_1000000_CTN_1001000_BTN_1001200,true);
	}

	m_MyUI.ShowControl(CTN_1000000_CTN_1001000, false);

	m_pGameFrame->SetTimer(ID_END_TIME,m_iBeginTime*1000);
	m_MyUI.ShowControl(CTN_1000000_CTN_1010000_CTN_1010001 + ViewStation(m_bMyDesk), true);
	m_MyUI.SetClock(CTN_1000000_CTN_1010000_CTN_1010001_Time_10100001 + ViewStation(m_bMyDesk), m_iBeginTime, true);
  
}

//设置房间信息
void CNewGameClient::ShowRoomInfo()
{
	m_MyUI.ShowControl(CTN_1000000_CTN_1011000, true);

	m_MyUI.SetTextInfo(CTN_1000000_CTN_1011000_TEXT_10110001, m_iBaseNote, true);
	//m_MyUI.SetTextInfo(CTN_1000000_CTN_1011000_TEXT_10110002, m_iLimitNote, true);
	return;
}

//设置按钮可用状态
void CNewGameClient::ShowNoteBtnEnable(bool bFollow,bool bAllNote,bool bNote,bool bGiveUp)
{
	for (int i = 0; i < 6; ++i)
	{
		m_MyUI.ShowControl(CTN_1000000_CTN_1012000_BTN_10120041 + i, true);
	}
	
	m_MyUI.EnableControl(CTN_1000000_CTN_1012000_BTN_10120041,bFollow);
	m_MyUI.EnableControl(CTN_1000000_CTN_1012000_BTN_10120042,bAllNote);

	ShowNoteButText(bNote);
	m_MyUI.EnableControl(CTN_1000000_CTN_1012000_BTN_120120040,bNote);
	m_MyUI.EnableControl(CTN_1000000_CTN_1012000_BTN_120120041,bNote);
	m_MyUI.EnableControl(CTN_1000000_CTN_1012000_BTN_120120042,bNote);
	m_MyUI.EnableControl(CTN_1000000_CTN_1012000_BTN_120120043,bNote);
	m_MyUI.EnableControl(CTN_1000000_CTN_1012000_CTN_1012100_BTN_10121002,bNote);
	//SetImageOnMoseMove(CTN_1000000_CTN_1012000_CTN_1012100_IMG_10121001,bNote);
	//SetImageMoseThough(CTN_1000000_CTN_1012000_CTN_1012100_IMG_10121003,bNote);
	//m_bCanMove=bNote;
	
	if(bNote)
	{
		SetImagePosition(CTN_1000000_CTN_1012000_CTN_1012100_IMG_10121003,685,640);
	}
	SetImageOnMoseMove(CTN_1000000_IMG_120120088,bNote);
	m_MyUI.EnableControl(CTN_1000000_CTN_1012000_CTN_1012100_BTN_10121002,bNote);

	m_MyUI.EnableControl(CTN_1000000_CTN_1012000_BTN_10120046,bGiveUp);
	
	return ;
}

//---------------游戏操作----------------------------------
//用户处理结果
void CNewGameClient::OnHitActiveResult(BYTE btag, __int64 iNote)
{
	if (GetStationParameter() == GS_PLAY_GAME)
	{
		if (m_iNowOutPeople != m_bMyDesk)
		{
			KillGameTimer(m_bMyDesk);
			ShowNoteBtnEnable();
			return;
		}
		
		tagUserProcess process;
		process.iVrebType = btag;	// 下注类型
		process.iNote =iNote;		// 下注数(只有下注和加注才设置此值)
		m_pGameFrame->SendGameData(&process,sizeof(process),MDM_GM_GAME_NOTIFY,ASS_VREB_CHECK,0);
	}
	return;
}

//用户选择下注数
void CNewGameClient::OnSelectNote(__int64 i64Num)
{
	if(m_bNewTurn)
	{
		if (i64Num<m_iBaseNote || i64Num > m_iLimitNote)
		{
			return;
		}
	}
	
	tagUserProcess process;
	memset(&process,NULL,sizeof(process));

	//下注数(只有下注和加注才设置此值) 因为改为__int64后lparam无法容纳而修改  xyh:20110729
	/*switch(itag)
	{
	case 2:     //第三个下注按钮
		{
			process.iNote = m_iAddButtonNum[2];//4*m_iBaseNote + GetMaxNote(m_iThisTurnNote);
			break;
		}
	case 1:     //第二个下注按钮
		{
			process.iNote =  m_iAddButtonNum[1];//2*m_iBaseNote + GetMaxNote(m_iThisTurnNote);
			break;
		}
	case 0:     //第一个下注按钮
		{	
			process.iNote =  m_iAddButtonNum[0];//1*m_iBaseNote + GetMaxNote(m_iThisTurnNote);
			break;
		}
	default:
		break;
	}*/
	process.iNote = i64Num;

	process.iVrebType = (m_iNowBigNote == 0)?TYPE_NOTE:TYPE_ADD;//下注类型
	m_pGameFrame->SendGameData(&process,sizeof(process),MDM_GM_GAME_NOTIFY,ASS_VREB_CHECK,0);
	return;
}

//显示准备的玩家
void CNewGameClient::ShowAgree(BYTE bDeskStation)
{
	UserInfoForGame UserInfo;
	if (NULL != m_pGameFrame)
	{
		if (!m_pGameFrame->GetUserInfo(bDeskStation, UserInfo))
		{
			return;
		}
	}
	
	int iView = ViewStation(bDeskStation);
	TCHAR pchar[MAX_PATH] = {0};
#ifdef UI2_5D
	sprintf(pchar, "%s/resources/ready.png", CBcfFile::GetAppPath());
#else
	sprintf(pchar, "%s/resources/ready.png", CBcfFile::GetAppPath());
#endif
	
	m_MyUI.SetImagesLoadPic(CTN_1000000_CTN_1014000_IMG_10140001 + iView, pchar, true);

	if (bDeskStation == m_bMyDesk)
	{	
		SetControlVisible(CTN_1000000_CTN_1001000_BTN_1001100,false);
		m_MyUI.ShowControl(CTN_1000000_CTN_1010000_CTN_1010001 + ViewStation(m_bMyDesk), false);
		m_MyUI.SetClock(CTN_1000000_CTN_1010000_CTN_1010001_Time_10100001 + ViewStation(m_bMyDesk), 0, false);
		m_MyUI.ShowControl(CTN_1000000_BTN_10001, false);
		m_pGameFrame->KillTimer(ID_BEGIN_TIME);
		m_pGameFrame->KillTimer(ID_END_TIME);
	}
	
	return;
}

//显示弃牌
void CNewGameClient::ShowGiveUp(BYTE bDeskStation)
{
	if (!IsPlayer(bDeskStation))
	{
		return;
	}

	int iView = ViewStation(bDeskStation);
	TCHAR pchar[MAX_PATH] = {0};
#ifdef UI2_5D
	sprintf(pchar, "%s/resources/pass.png", CBcfFile::GetAppPath());
#else
	sprintf(pchar, "%s/resources/pass.png", CBcfFile::GetAppPath());
#endif
	
	m_MyUI.SetImagesLoadPic(CTN_1000000_CTN_1014000_IMG_10140001 + iView, pchar, true);

	return;
}

//游戏开始时初始化界面
void CNewGameClient::ShowBegin()
{
	for (int i = 0; i< PLAY_COUNT; ++i)
	{
		m_MyUI.ShowControl(CTN_1000000_CTN_1014000_IMG_10140001 + i, false);//准备标志
		m_MyUI.ShowControl(CTN_1000000_CTN_1010000_CTN_1010001 + i, false);
		m_MyUI.ShowControl(CTN_1000000_CTN_1010000_CTN_1010001_Time_10100001 + i, false);//计时器
	}
	m_MyUI.ShowControl(CTN_1000000_BTN_10001, false);//准备按钮隐藏
	m_pGameFrame->KillTimer(ID_BEGIN_TIME);

	return;
}

__int64 CNewGameClient::GetMaxNote(__int64 a[4])
{
	__int64 p = -1;
	for (int i = 0; i < 4; i++)
	{
		if (a[i] > p)
		{
			p = a[i];
		}
	}
	return p;
}

//代替断线玩家下注
BOOL CNewGameClient::ReplaceUserNote(BYTE bDeskStation)
{
	KillAllTimer();
	if (GetStationParameter() == GS_PLAY_GAME)
	{
		ReplaceUserNoteStruct supNote;
		supNote.bDeskStation = bDeskStation;	//代替下注位置
		supNote.iVrebType = 0;					//下注类型
		supNote.iNote = 0;						//下注数(只有下注和加注才设置此值)
		m_pGameFrame->SendGameData(&supNote,sizeof(supNote),MDM_GM_GAME_NOTIFY,ASS_REPLACE_VREB_CHECK,0);
	}
	return TRUE;
}

//非循环播放声音函数
void CNewGameClient::PlaySound(BYTE bDeskStation, BYTE bStyle)
{
	if (!m_bSound || m_SoundEngine==NULL)
	{
		return;
	}

	if (!IsPlayer(bDeskStation))
	{
		return;
	}

	bool bIsBoy = false;
	UserInfoForGame UserInfo;
	if (m_pGameFrame->GetUserInfo(bDeskStation,UserInfo))
	bIsBoy = UserInfo.bBoy;
	char szFileName[MAX_PATH];

	switch(bStyle)
	{
	case SOUND_SHUTTLE:
		wsprintf(szFileName, "Music\\%s\\suoha.wav",bIsBoy?"MAN":"WOM");
		break;
	case SOUND_FOLLOW:
		wsprintf(szFileName, "Music\\%s\\xiazhu.ogg",bIsBoy?"MAN":"WOM");
		break;
	case SOUND_ADD:
		wsprintf(szFileName, "Music\\%s\\xiazhu.ogg",bIsBoy?"MAN":"WOM");
		break;
	case SOUND_NOTE:
		wsprintf(szFileName, "Music\\%s\\xiazhu.ogg",bIsBoy?"MAN":"WOM");
		break;
	case SOUND_GIVE_UP:
		wsprintf(szFileName, "Music\\%s\\pass.wav",bIsBoy?"MAN":"WOM");
		break;
	case SOUND_MOVEBUTTON:
		wsprintf(szFileName, "Music\\%s\\movebutton.ogg",bIsBoy?"MAN":"WOM");
		break;
	case SOUND_WARNING:
		wsprintf(szFileName, "Music\\%s\\Warning.wav",bIsBoy?"MAN":"WOM");
		break;
	case SOUND_SEND_CARD:
		wsprintf(szFileName, "Music\\%s\\fapai.wav",bIsBoy?"MAN":"WOM");
		break;
	case SOUND_START_DEF:
		wsprintf(szFileName, "Music\\%s\\start.wav",bIsBoy?"MAN":"WOM");
		break;
	case SOUND_WIN:
		wsprintf(szFileName, "Music\\%s\\ying.wav",bIsBoy?"MAN":"WOM");
		break;
	case SOUND_LOSE:
		wsprintf(szFileName, "Music\\%s\\shu.wav",bIsBoy?"MAN":"WOM");
		break;
	default:
		return;
		break;
	}

	m_SoundEngine->play2D(szFileName,false);
	return;
}

//非循环播放声音函数(无须判断玩家是男女)
void CNewGameClient::PlaySound(BYTE bStyle)
{
	if (!m_bSound || m_SoundEngine==NULL)
	{
		return;
	}

	char szFileName[MAX_PATH];

	switch(bStyle)
	{
	case SOUND_SHUTTLE:
		wsprintf(szFileName, "Music\\suoha.wav");
		break;
	case SOUND_FOLLOW:
		wsprintf(szFileName, "Music\\xiazhu.ogg");
		break;
	case SOUND_ADD:
		wsprintf(szFileName, "Music\\xiazhu.ogg");
		break;
	case SOUND_NOTE:
		wsprintf(szFileName, "Music\\xiazhu.ogg");
		break;
	case SOUND_GIVE_UP:
		wsprintf(szFileName, "Music\\pass.wav");
		break;
	case SOUND_MOVEBUTTON:
		wsprintf(szFileName, "Music\\movebutton.ogg");
		break;
	case SOUND_WARNING:
		wsprintf(szFileName, "Music\\Warning.wav");
		break;
	case SOUND_SEND_CARD:
		wsprintf(szFileName, "Music\\fapai.wav");
		break;
	case SOUND_START_DEF:
		wsprintf(szFileName, "Music\\start.wav");
		break;
	case SOUND_WIN:
		wsprintf(szFileName, "Music\\ying.wav");
		break;
	case SOUND_LOSE:
		wsprintf(szFileName, "Music\\shu.wav");
		break;
	default:
		return;
		break;
	}

	m_SoundEngine->play2D(szFileName,false);
	return;
}

//随机背景音乐
void CNewGameClient::PlaySceneSound()
{
	char szFileName[MAX_PATH];
	strcpy(szFileName, "Music\\gamemusic\\1.ogg");
	m_SoundEngine->play2D(szFileName,true);

	return;
}

//加注
void CNewGameClient::AddTotalNote(__int64 iNoteTotal)
{
	m_iNoteTotalPoint += iNoteTotal;
	ShowTotalNote(true);
	return ;
}

//初始化总注
void CNewGameClient::SetTotalNote(__int64 iNoteTotal)
{
	m_iNoteTotalPoint=iNoteTotal;
	ShowTotalNote(true);
	return ;
}

//显示总注
void CNewGameClient::ShowTotalNote(bool bFlag)
{
	//m_MyUI.SetTextInfo(CTN_1000000_CTN_1012000_TEXT_10120050, m_iNoteTotalPoint, bFlag,false, AlignmentCenter);
	return;
}

//玩家下注动画
void CNewGameClient::ShowNoteAnima(BYTE bDeskStation, __int64 iNote, int iTime)
{
	int bView = ViewStation(bDeskStation);
	int iMoveActionID[2] = {CTN_1000000_CTN_1012000_MOVE_10120001,CTN_1000000_CTN_1012000_MOVE_10120002};
	int iEndPointID[2] = {CTN_1000000_CTN_1012000_IMG_10120021,CTN_1000000_CTN_1012000_TEXT_10120031};
	int iBeMoveID[2] = {CTN_1000000_CTN_1012000_IMG_10120012,CTN_1000000_CTN_1012000_TEXT_10120011};

	m_MyUI.SetTextInfo(CTN_1000000_CTN_1012000_TEXT_10120011, iNote, false, false,AlignmentCenter);

	m_MyUI.PlayChipInAnima_Image(iMoveActionID, 
		CTN_1000000_CTN_1015000_NoMoveCard_10150021, 
		iEndPointID, 
		iBeMoveID, 
		bView, 
		(int)iNote, 
		iTime, 
		true);

	return;
}

//杀死所有计时器
void CNewGameClient::KillAllTimer()
{
	m_pGameFrame->KillTimer(ID_BEGIN_TIME);
	m_pGameFrame->KillTimer(ID_END_TIME);
	m_pGameFrame->KillTimer(ID_TIME_SEND_CARD);
	m_pGameFrame->KillTimer(ID_TIME_THINK);
	m_pGameFrame->KillTimer(ID_TIME_BASENOTE);
	
	return;
}

//显示玩家下的注
void CNewGameClient::ShowPlayerNote(BYTE bDeskStation, bool bFlag)
{
	int iView = ViewStation(bDeskStation);
	m_MyUI.SetTextInfo(CTN_1000000_CTN_1012000_TEXT_10120031 + iView, m_iPerJuTotalNote[bDeskStation], bFlag, false,AlignmentCenter);
	m_MyUI.ShowControl(CTN_1000000_CTN_1012000_IMG_10120021 + iView, bFlag);
	
	return;
}

//显示玩家手牌
void CNewGameClient::ShowHandCard(BYTE bDeskStation, BYTE iCard[], int iCount, bool bFlag)
{
	int iView = ViewStation(bDeskStation);

	m_MyUI.SetCardInfo(CTN_1000000_CTN_1015000_NoMoveCard_10150021 + iView, iCard, iCount, bFlag);
	return;
}

//显示最大牌玩家标记
void CNewGameClient::ShowCardMaxMark(bool bFlag)
{
	if (bFlag)
	{
		for (int i = 0; i < PLAY_COUNT; ++i)
		{
			int iView = ViewStation(i);
			if (IsPlayer(i) && i == m_iNowOutPeople)
			{
				m_MyUI.ShowControl(CTN_1000000_CTN_1014000_IMG_10140011 + iView, true);
			}
			else
			{
				m_MyUI.ShowControl(CTN_1000000_CTN_1014000_IMG_10140011 + iView, false);
			}
		}
	} 
	else
	{
		for (int i = 0; i < PLAY_COUNT; ++i)
		{
			m_MyUI.ShowControl(CTN_1000000_CTN_1014000_IMG_10140011 + i, false);
		}
	}

	return;
}

//显示下注按钮上的数目
void CNewGameClient::ShowNoteButText(bool bFlag)
{
	if (bFlag)
	{
		if(m_bNewTurn)
		{
			for (int i = 0; i < 4; ++i)
			{
				m_MyUI.SetTextForBut( CTN_1000000_CTN_1012000_BTN_120120040 + i, m_iAddButtonNum[i]);
				m_MyUI.EnableControl(CTN_1000000_CTN_1012000_BTN_120120040 + i,true);
			}
			m_MyUI.SetTextForBut( CTN_1000000_CTN_1012000_CTN_1012100_BTN_10121002, m_iSelAddMoney);
			m_MyUI.EnableControl(CTN_1000000_CTN_1012000_CTN_1012100_BTN_10121002,true);
		}
		else
		{
			__int64 i64Temp=m_i64HalfLessMoney-m_iPerJuTotalNote[m_pGameFrame->GetMyDeskStation()];
			m_iTempBt[3] = (__int64)(i64Temp/1.0);
			m_iTempBt[2] = (__int64)(i64Temp/2.0);
			m_iTempBt[1] = (__int64)(i64Temp/3.0);
			m_iTempBt[0] = (__int64)(i64Temp/4.0);
			CString cs;
			cs.Format("lwlog::i64Temp=%d",i64Temp);
			OutputDebugString(cs);
			for (int i = 0; i < 4; ++i)
			{
				m_MyUI.SetTextForBut( CTN_1000000_CTN_1012000_BTN_120120040 + i, (m_iTempBt[i]>0)?m_iTempBt[i]:0);
				m_MyUI.EnableControl(CTN_1000000_CTN_1012000_BTN_120120040 + i,(m_iTempBt[i]>0)?true:false);
			}
			m_MyUI.SetTextForBut( CTN_1000000_CTN_1012000_CTN_1012100_BTN_10121002, 0);
		}
		
	} 
	else
	{
		for (int i = 0; i < 4; ++i)
		{
			m_MyUI.SetTextForBut( CTN_1000000_CTN_1012000_BTN_120120040 + i, 0);
		}
		m_MyUI.SetTextForBut( CTN_1000000_CTN_1012000_CTN_1012100_BTN_10121002, 0);
	}

	return;
}

//设置下注按钮上的数目
void CNewGameClient::SetNoteButText(__int64 iBaseNote)
{
	//modified by yjp.下注按钮根据底注，而不是上限
	__int64 ibt[4] = {0};
	ibt[0] = (__int64)(iBaseNote*1.0);//(DOUBLE)m_iLimitNote*1.5/100;
	ibt[1] = (__int64)(iBaseNote*2.0);//(DOUBLE)m_iLimitNote*3/100;
	ibt[2] = (__int64)(iBaseNote*3.0);//(DOUBLE)m_iLimitNote*3/100;
	ibt[3] = (__int64)(iBaseNote*4.0);//(DOUBLE)m_iLimitNote*6/100;
	if (ibt[0]<= 0)
	{
		ibt[0] = 1;
		ibt[1] = 2;
		ibt[2] = 3;
		ibt[2] = 4;
	}

	CopyMemory(m_iAddButtonNum, ibt, sizeof(ibt));

	return;
}



//设置鼠标响应
void	CNewGameClient::SetButtonOnMoseMove(const int iID, bool bFlag)
{
	IButton *pButton = NULL;
	pButton = dynamic_cast<IButton *>(m_pIUI->GetIControlbyID(iID));
	if (pButton != NULL)
	{
		pButton->SetMouseMove(bFlag);
	}
}


//设置鼠标响应
void	CNewGameClient::SetImageOnMoseMove(const int iID,bool bFlag)
{
	IImage *pImage = NULL;
	pImage = dynamic_cast<IImage *>(m_pIUI->GetIControlbyID(iID));
	if (pImage != NULL)
	{
		pImage->SetMouseMove(bFlag);
	}
}

//获取图片的位置
POINT	CNewGameClient::GetImagePosition(const int iID)
{
	POINT ptImage;
	memset(&ptImage,0, sizeof(ptImage));
	IImage *pImage = NULL;
	pImage = dynamic_cast<IImage *>(m_pIUI->GetIControlbyID(iID));
	if (pImage != NULL)
	{
		ptImage.x = pImage->GetGameX();
		ptImage.y = pImage->GetGameY();
	}
	return ptImage;
}

//设置图片位置
void	CNewGameClient::SetImagePosition(const int iID, int iX, int iY)
{
	IImage *pImage = NULL;
	pImage = dynamic_cast<IImage *>(m_pIUI->GetIControlbyID(iID));
	if (pImage != NULL)
	{
		pImage->SetGameXY(iX, iY);
	}
}

//设置鼠标穿透
void	CNewGameClient::SetImageMoseThough(const int iID,bool bFlag)
{
	IImage *pImage = NULL;
	pImage = dynamic_cast<IImage *>(m_pIUI->GetIControlbyID(iID));
	if (pImage != NULL)
	{
		pImage->SetMouseThough(bFlag);
	}
}

//设置图片加载宽度
void	CNewGameClient::SetImageSrcX(const int iID,int isrcX)
{
	IImage *pImage = NULL;
	pImage = dynamic_cast<IImage *>(m_pIUI->GetIControlbyID(iID));
	if (pImage != NULL)
	{
		pImage->SetSrcX(isrcX);
	}
}
//设置控件是否可见
void CNewGameClient::SetControlVisible(const int iControlID,bool bFlag)
{
	IBCInterFace  *pControl = NULL ; 
	pControl = dynamic_cast<IBCInterFace *>(m_pIUI->GetIControlbyID(iControlID)); 
	if(NULL != pControl)
	{
		pControl->SetControlVisible(bFlag) ; 
	}
}
//设置文本控件显示内容(显示数字)
void	CNewGameClient::SetTextinfo(const int iTextID,__int64 iNum, bool bFlag)
{
	IText *pText = NULL;
	pText = dynamic_cast<IText *>(m_pIUI->GetIControlbyID(iTextID));
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
	pText = dynamic_cast<IText *>(m_pIUI->GetIControlbyID(iTextID));
	if (NULL != pText)
	{
		CString sText;
		sText.Format("%s",ch);
		pText->SetText(CA2W(sText));
		pText->SetControlVisible(bFlag);
	}
}
//根据图片控件设置显示图片的尺寸
void CNewGameClient::SetPicSize(const int iButtonID,CString sImagSrc, bool bFlag,int x,int y,int w,int h)
{
	IImage *pImage = NULL;
	pImage = dynamic_cast<IImage *>(m_pIUI->GetIControlbyID(iButtonID));
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
//设置数字空间显示内容
void	CNewGameClient::SetInumInfo(const int iNumID, wchar_t * pchar,__int64 i64Num,bool bFlag)
{
	INum *pNum = NULL;
	pNum = dynamic_cast<INum *>(m_pIUI->GetIControlbyID(iNumID));
	if (NULL != pNum)
	{
		pNum->SetImagePath(pchar);
		pNum->SetNum(i64Num);
		pNum->SetControlVisible(bFlag);
	}
}
//--------------------------------------------------------------------------------------------------------
//设置单选框控件是否被选中
void	CNewGameClient::SetRButtonSelected(const int iRButtonID,bool bSelected)
{
	IRadioButton *pRadioButton = NULL;
	pRadioButton = dynamic_cast<IRadioButton *>(m_pIUI->GetIControlbyID(iRButtonID));
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
	pRadioButton = dynamic_cast<IRadioButton *>(m_pIUI->GetIControlbyID(iRButtonID));
	if (NULL != pRadioButton)
	{
		*bSelected=pRadioButton->GetIsSelect();
		return (*bSelected);
	}
	return TRUE;
}