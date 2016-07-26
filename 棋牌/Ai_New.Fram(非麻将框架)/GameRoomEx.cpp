#include "StdAfx.h"
#include "GameRoomEx.h"
#include "auto_aidlg.h"
#include "gameroommessage.h"


static bool g_bUserAI = true;

///rqffffffffffffffffffffff
static double randf()
{
	return rand()*1.0/RAND_MAX;
}

int CGameRoomEx::m_nOnLinePeople = 0;

BEGIN_MESSAGE_MAP(CGameRoomEx, CBaseRoom)
	ON_WM_TIMER()
	ON_MESSAGE(IDM_SET_GAME_TIME,SetGameTime)
	ON_MESSAGE(IDM_KILL_GAME_TIME,KillGameTime)
	ON_MESSAGE(IDM_CLOSE,OnCloseGame)//20090211
END_MESSAGE_MAP()

//构造函数
CGameRoomEx::CGameRoomEx(TCHAR * szGameName, RoomInfoStruct * pRoomInfo, MSG_GP_R_LogonResult * pLogonInfo,BYTE bEnterStyle
						 , bool bMachineAndPlay, int iMachineUserId, BYTE bMachineDeskCount, bool bAutoDesk) 
: CBaseRoom(IDD_GAME_ROOM)
{
	m_IGameFrame = NULL; 	//游戏接口

	m_bEnterStyle = bEnterStyle;
   m_iContestState = 0;
	//20081126新加机器人配置项
	m_bMachineAndPlay = bMachineAndPlay;//机器人与玩家是否同桌
	m_iMachineCheckTime  = iMachineUserId;//机器人UserID
	m_bMachineDeskCount = bMachineDeskCount;//每桌机器人个数
	m_bAutoDesk       = bAutoDesk;
	m_bNowDesk        = 255;                         //当前要坐的桌号
	m_bErrorDeskCount = 0;                  //不可坐桌号个数。超过20个时，重置
	memset(&m_bErrorDesk,255,sizeof(m_bErrorDesk)); //不可坐桌号

	memset(&m_bErrorDeskFlag,0,sizeof(m_bErrorDeskFlag)); //不可坐桌号
	
	memset(&m_pLogonInfo,0,sizeof(m_pLogonInfo));
	::memset(&m_ComRoomInfo,0,sizeof(m_ComRoomInfo));

	m_pRoomInfo = pRoomInfo;
	////初始化共享数据
	memset(&m_GameInfo,0,sizeof(m_GameInfo));
	lstrcpy(m_GameInfo.szGameName,szGameName);
	m_GameInfo.pISocketSend=this;
	m_GameInfo.pIFindUser=&m_UserManage;
	m_GameInfo.bShowUserInfo=true;
	m_GameInfo.bEnableSound=true;
	m_GameInfo.bEnableWatch=false;
	m_GameInfo.uNameID=pRoomInfo->stComRoomInfo.uNameID;
	m_GameInfo.uRoomID=pRoomInfo->stComRoomInfo.uRoomID;
	//m_GameInfo.dwGameLSVer=pRoomInfo->dwGameLSVer;
	m_GameInfo.uComType=pRoomInfo->stComRoomInfo.uComType;
	m_GameInfo.uDeskPeople=pRoomInfo->stComRoomInfo.uDeskPeople;
	m_GameInfo.dwRoomRule = pRoomInfo->stComRoomInfo.dwRoomRule ; 


	m_i64MaxAiMoney = 10000000;                    //AI身上最多的钱数，达到这个数就存钱
	m_i64CheckMoneyMax = 100000;                 //取钱最大数
	m_i64CheckMoneyMix = 10000;                 //取钱最小数
	m_i64SaveAndOutMoneyBase = 100;				//存取钱基数（整存整取基数）
	m_bAiBank = FALSE;
	m_bOpenBankBase = FALSE;					//是否开启机器人整存整取
	m_UseBank = FALSE;
	m_CheckCount = 0;

	CopyMemory(&m_ComRoomInfo,&(pRoomInfo->stComRoomInfo),sizeof(ComRoomInfo));
	CopyMemory(&m_pLogonInfo,(MSG_GP_R_LogonResult*)pLogonInfo,sizeof(m_pLogonInfo));

	

	return;
}

//析构函数
CGameRoomEx::~CGameRoomEx()
{
	if (m_IGameFrame != NULL)
	{
		delete m_IGameFrame;
		m_IGameFrame = NULL;
	}
	//OutputDebugString("~CGameRoomEx()");
}

//数据绑定函数
void CGameRoomEx::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

}

//初始化函数
BOOL CGameRoomEx::OnInitDialog()
{
	__super::OnInitDialog();

	ConnectToGameServer();

	CString ad=CINIFile::GetAppPath ();
	CINIFile f( ad + "bzgame.ini");
	SetTimer(IDT_GAMEEXIT, TIME_CHECK_GAMEEXIT, NULL);
	CString m_key = "BZW";
	m_CheckCountMax = f.GetKeyVal(m_key,"CheckCountMax",10);//
	//CString ad=CINIFile::GetAppPath ();
	//CINIFile f( ad + "bzgame.ini");
	//CString key = "BZW";
	//m_i64MaxAiMoney = f.GetKeyVal(key,"MaxAiMoney",m_i64MaxAiMoney);                    //AI身上最多的钱数，达到这个数就存钱
	//m_i64CheckMoneyMax = f.GetKeyVal(key,"CheckMoneyMax",m_i64CheckMoneyMax);                 //取钱最大数
	//m_i64CheckMoneyMix = f.GetKeyVal(key,"CheckMoneyMix",m_i64CheckMoneyMix);                   //取钱最小数
	//m_bAiBank = f.GetKeyVal(key,"AiBank",m_bAiBank);
	/*CString str;
	str.Format("zhtlog:m_i64MaxAiMoney=%I64d",m_i64MaxAiMoney);
	OutputDebugString(str);*/

	return TRUE;
}
//登陆游戏房间
void  CGameRoomEx::ConnectToGameServer()
{
	int static iConnect = 0;

	if(m_TCPSocket->Connect(m_ComRoomInfo.szServiceIP,m_ComRoomInfo.uServicePort)==false && iConnect ++ < 5)
	{
		
		this->ConnectToGameServer();
	}
	

	return ;
}
//网络连接消息
bool CGameRoomEx::OnSocketConnectEvent(UINT uErrorCode, CTCPClientSocket * pClientSocket)
{
	//WriteStr("OnSocketConnectEvent");
	if (uErrorCode!=0)	//连接出现错误
	{
		//WriteStr("OnSocketConnectEvent Error");
		return true;
	}
	return true;
}

//网络读取消息
bool CGameRoomEx::OnSocketReadEvent(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	/*wsprintf(m_ErrorMsg,"OnSocketReadEvent bMainID = %d,bAssistantID = %d",pNetHead->bMainID,pNetHead->bAssistantID );
	WriteStr(m_ErrorMsg,1);*/
	switch (pNetHead->bMainID)
	{
	case MDM_CONNECT:			//连接消息,准备进入房间 
		{
			return OnConnectMessage(pNetHead,pNetData,uDataSize,pClientSocket);
		}
	case MDM_GR_LOGON:			//房间登陆
		{
			return OnLogonMessage(pNetHead,pNetData,uDataSize,pClientSocket);
		}
	case MDM_GR_USER_LIST:		//用户列表
		{
			return OnUserListMessage(pNetHead,pNetData,uDataSize,pClientSocket);
		}
	case MDM_GR_USER_ACTION:	//用户动作
		{
			return OnUserActionMessage(pNetHead,pNetData,uDataSize,pClientSocket);
		}
	case MDM_GR_ROOM:			//房间信息
		{
			return OnRoomMessage(pNetHead,pNetData,uDataSize,pClientSocket);
		}
	case MDM_GR_MATCH_INFO:		//比赛信息
		{
			return true;//return OnMatchMessage(pNetHead,pNetData,uDataSize,pClientSocket);
		}
	case MDM_GR_MANAGE:			//管理消息
		{
			return true;//return OnManageMessage(pNetHead,pNetData,uDataSize,pClientSocket);
		}
	case MDM_GR_MESSAGE:		//系统消息
		{
			return true;//return OnSystemMessage(pNetHead,pNetData,uDataSize,pClientSocket);
		}
	//case MDM_GR_POINT:			//经验盒
	//	{
	//		break;
	//	}
	case MDM_GR_MONEY:			//钱柜划账,扣钱
		{
			//return true;//
			return OnAboutMoney(pNetHead,pNetData,uDataSize,pClientSocket);
		}
	//case MDM_GR_ITEM_DATA_MSG://道俱
	//	{
	//		return true;
			//return OnItemDataMsg(pNetHead,pNetData,uDataSize,pClientSocket);
	//	}	
		//case MDM_GR_BROADCAST_MESSAGE://广播消息
		//	{
		//		return OnBroadCastManage(pNetHead,pNetData,uDataSize,pClientSocket);
		//	}
#ifdef MY_MDM
	case MDM_GR_FRIEND_MANAGE://好友消息
		{
			return true;//OnFriendManage(pNetHead,pNetData,uDataSize,pClientSocket);
		}
	case MDM_GR_QUEUE_MSG://排队等待消息
		{
			return true;//OnQueueMsg(pNetHead,pNetData,uDataSize,pClientSocket);
		}
	case MDM_GR_VIP_MSG:	//vip消息
		{
			return true;//OnVipOperationMsg(pNetHead,pNetData,uDataSize,pClientSocket);
		}
#endif
	}

	////游戏信息
	if (m_IGameFrame!=NULL)
		return m_IGameFrame->OnSocketReadEvent(pNetHead,pNetData,uDataSize,pClientSocket);

	return true;
}
#ifdef MY_MDM
//更新玩家道俱
bool CGameRoomEx::OnItemDataMsg(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	switch(pNetHead->bAssistantID)
	{
	case ASS_S_GET_ITEM_DATA:	//获取
		return true;
		break;
	case ASS_S_USE_ITEM:		//使用道俱
		return OnUserUseItemResult(pNetHead,pNetData,uDataSize,pClientSocket);
		break;
	case ASS_S_GET_BUY_ITEM_DATA://购买
		return true;
		break;
	case ASS_S_BUY_ITEM_DATA:	//购买
		return true;
		break;
	}
	return true;
}

//使用道俱
bool CGameRoomEx::OnUserUseItemResult(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	if(sizeof(tagUserUseItem)!= uDataSize)
		return false;
	tagUserUseItem * pUseItem = (tagUserUseItem *)pNetData;
	UserItemStruct *pUserInfo = m_UserManage.FindOnLineUser(pUseItem->dwDestUserID);
	CString stime;
	stime.Format("%d",CTime::GetCurrentTime());        //当前的时间，String格式
	int curtime=atoi(stime);                        //转换为int
	if(pUserInfo == NULL)
		return true;
	switch(pUseItem->iItemID)
	{
	case ITEM_FLOWER://鲜花魅力+1
		{
			pUserInfo->GameUserInfo.dwFascination += 1;
			break;
		}
	case ITEM_BLOCK://砖头魅力-1
		{
			pUserInfo->GameUserInfo.dwFascination -= 1;
			break;
		}
	case ITEM_ERASER://负分清零
		{
			if(pUserInfo->GameUserInfo.dwPoint < 0)
				pUserInfo->GameUserInfo.dwPoint = 0;
			break;
		}
	case ITEM_SMALL_BROADCAST:
	case ITEM_BIG_BROADCAST:
		{
			break;
		}
	case DOUBLE_SCORE_CARD_1_HOUR:
		{
			if(pUseItem->dwDestUserID ==  m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)
			{
				if(pUserInfo->GameUserInfo.iDoublePointTime>curtime)
					pUserInfo->GameUserInfo.iDoublePointTime+=60*60;
				else
					pUserInfo->GameUserInfo.iDoublePointTime=curtime+60*60;
				time_t t=(time_t)pUserInfo->GameUserInfo.iDoublePointTime;
				CTime expiresTime(t);

			}
			break;
		}
	case DOUBLE_SCORE_CARD_3_HOUR:
		if(pUseItem->dwDestUserID ==  m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)
		{
			if(pUserInfo->GameUserInfo.iDoublePointTime>curtime)
				pUserInfo->GameUserInfo.iDoublePointTime+=3*60*60;
			else
				pUserInfo->GameUserInfo.iDoublePointTime=curtime+3*60*60;
			time_t t=(time_t)pUserInfo->GameUserInfo.iDoublePointTime;
			CTime expiresTime(t);

		}
		break;
	case DOUBLE_SCORE_CARD_1_DAY:
		if(pUseItem->dwDestUserID ==  m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)
		{
			if(pUserInfo->GameUserInfo.iDoublePointTime>curtime)
				pUserInfo->GameUserInfo.iDoublePointTime+=24*60*60;
			else
				pUserInfo->GameUserInfo.iDoublePointTime=curtime+24*60*60;
			time_t t=(time_t)pUserInfo->GameUserInfo.iDoublePointTime;
			CTime expiresTime(t);
		}
		break;
	case DOUBLE_SCORE_CARD_7_DAY:
		{
			if(pUseItem->dwDestUserID ==  m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)
			{
				if(pUserInfo->GameUserInfo.iDoublePointTime>curtime)
					pUserInfo->GameUserInfo.iDoublePointTime+=7*24*60*60;
				else
					pUserInfo->GameUserInfo.iDoublePointTime=curtime+7*24*60*60;
				time_t t=(time_t)pUserInfo->GameUserInfo.iDoublePointTime;
				CTime expiresTime(t);
			}
			break;
		}
	}
	//更新玩家列表
//	m_pUserList->UpdateGameUser(pUserInfo);
	//与自己同桌
	if(pUserInfo->GameUserInfo.bDeskNO == m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO
		&& m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO != 255)
	{
		if(NULL != m_IGameFrame )
			m_IGameFrame->OnSocketReadEvent(pNetHead,pNetData,uDataSize,pClientSocket);
	}
	return true;
}
#endif
//网络关闭消息
bool CGameRoomEx::OnSocketCloseEvent()
{
	//OutputDebugString("zhtlog:网络关闭消息");
	try
	{
		if(m_IGameFrame)
		{
			m_IGameFrame->AFCCloseFrame();
			//起身
		}
	}catch(...)
	{
		if (m_IGameFrame != NULL)
		{
			delete m_IGameFrame;
		}
		m_IGameFrame = NULL;
	}
	::PostMessage(GetParent()->m_hWnd,IDM_CLOSE_GAME_ROOM,(WPARAM)m_pRoomInfo, m_pLogonInfo.dwUserID);
	return true;
}
//关闭房间
void CGameRoomEx:: CloseGameRoom()
{
	m_TCPSocket->CloseSocket(false);
	try
	{
		if(m_IGameFrame)
		{
			m_IGameFrame->AFCCloseFrame();
			//起身
		}
	}catch(...)
	{
		if (m_IGameFrame != NULL)
		{
			delete m_IGameFrame;
		}
		m_IGameFrame = NULL;
	}
	::PostMessage(GetParent()->m_hWnd,IDM_CLOSE_GAME_ROOM,(WPARAM)m_pRoomInfo, m_pLogonInfo.dwUserID);
	return ;
}
//连接消息处理
bool CGameRoomEx::OnConnectMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	if (pNetHead->bAssistantID==ASS_NET_TEST)	//网络测试信息
	{
		pClientSocket->SendData(MDM_CONNECT,ASS_NET_TEST,0);
		return true;
	}
	else if ((pNetHead->bAssistantID==ASS_CONNECT_SUCCESS)||(pNetHead->bAssistantID==3))	//连接成功
	{
		MSG_S_ConnectSuccess * _p = (MSG_S_ConnectSuccess *)pNetData;
		if (_p != NULL)
		{
			pClientSocket->SetCheckCode(_p->i64CheckCode, SECRET_KEY);
		}

		//检测成功
		SendUserInfo();
		return true;
	}
	return true;
}

//001,房间登陆处理,一进入房间  
bool CGameRoomEx::OnLogonMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	switch (pNetHead->bAssistantID)
	{
	case ASS_GR_LOGON_SUCCESS:		//登陆成功
		{
			//效验数据
			if (uDataSize!=sizeof(MSG_GR_R_LogonResult)) return false;

			MSG_GR_R_LogonResult * pLogonInfo=(MSG_GR_R_LogonResult *)pNetData;

			//CopyMemory(&m_pLogonInfo,(MSG_GR_R_LogonResult * )pLogonInfo,sizeof(MSG_GR_R_LogonResult));
			m_GameInfo.uLessPoint=pLogonInfo->uLessPoint;
			m_GameInfo.pMeUserInfo = m_UserManage.AddGameUser(&pLogonInfo->pUserInfoStruct,1,0);
						
			if(m_GameInfo.pMeUserInfo == NULL)
			{
				WriteStr("ASS_GR_LOGON_SUCCESS玩家离开了");
				
				CloseGameRoom();
				return false;
			}
			
			m_iMyMoney = (m_GameInfo.pMeUserInfo->GameUserInfo.i64Money+m_GameInfo.pMeUserInfo->GameUserInfo.i64Bank);
						
			return true;
		}
	case ASS_GR_LOGON_ERROR:		//登陆失败
		{
			//wsprintf(m_ErrorMsg,"ASS_GR_LOGON_ERROR//pNetHead->bHandleCode = %d",pNetHead->bHandleCode);
			WriteStr("Login Failed");
			OutputDebugString("ASS_GR_LOGON_ERROR");
			//m_bNowRoomIndex--;
			::PostMessage(GetParent()->m_hWnd,IDM_LOGON_FAIL,0,0);//20081128
			return false;
		}
	case ASS_GR_SEND_FINISH:		//登陆完成
		{
			if(m_bEnterStyle == 1)
				return true;
			//设置总状态跟踪
			SetCheckStationTime();
			return true;
		}
	}
	return true;
}


//设置状态检测计时器
bool CGameRoomEx::SetCheckStationTime()
{
	//SetTimer(ID_CHECK_STATE,TIME_CHECK_STATE,NULL);
	SetTimer(ID_CHECK_STATE,m_iMachineCheckTime,NULL);
	SetTimer(ID_CLEAR_ERRORDESK,1800*1000,NULL);
	AiControlBank();
	return true;
}

//起立
bool CGameRoomEx::StandingDesk()
{
	//下面发送坐下的信息
	if (m_IGameFrame!=NULL)
	{
		CClientGameDlg *dlg = NULL;
		dlg = (CClientGameDlg*)m_IGameFrame;
	}
	m_TCPSocket->SendData(MDM_GR_USER_ACTION,ASS_GR_USER_UP,0);
	return true;
}

//坐下
bool CGameRoomEx::OnSitDesk()
{
	if(m_GameInfo.dwRoomRule &GRR_QUEUE_GAME)
	{
		m_TCPSocket->SendData(MDM_GR_USER_ACTION,ASS_GR_JOIN_QUEUE,0);
		return  true ; 
	}
	
	BYTE bDeskIndex = 255;
	BYTE bDeskStation = 255;
	//查找空桌子
	//OnFindNullDesk(bDeskIndex,bDeskStation);
	//随机查找一个空桌子
#ifdef MY_GGW
	OnFindRandDesk(bDeskIndex,bDeskStation);
#else
	//新余面对面要求机器人不跟真人玩游戏
	if (!m_bMachineAndPlay)
	{
		OnFindMachineDesk(bDeskIndex,bDeskStation);
	}
	else
	{
		//查找真人最多空桌子(不能机器人同机器人玩,斗地主最多2个机器人)

		//rqffffffffffffffffffffffffffffffff
		//OnFindTureDesk(bDeskIndex,bDeskStation);
		OnFindNullDesk(bDeskIndex,bDeskStation);
	}
#endif

	if(bDeskIndex == 255||bDeskStation == 255)
	{
		return true;
	}
	MSG_GR_S_UserSit UserSit;
	UserSit.bDeskIndex = bDeskIndex;
	UserSit.bDeskStation = bDeskStation;
	m_bNowDesk        = bDeskIndex;
	//下面发送坐下的信息
	m_TCPSocket->SendData(&UserSit,sizeof(UserSit),MDM_GR_USER_ACTION,ASS_GR_USER_SIT,0);
	return true;
}
//随机坐一个空桌子
bool CGameRoomEx::OnFindRandDesk(BYTE &bDeskIndex,BYTE &bDeskStation)
{
	srand(GetTickCount());
	int time = 0;
	UINT bDesk = rand() % m_ComRoomInfo.uDeskCount;

	while(time++ < 5)
	{
		//坐位
		for(UINT station = 0; station < m_GameInfo.uDeskPeople; station++)
		{
			if (IsErrorDesk(station))//20090227判断是否为不可用桌子
			{
				continue;
			}
			bool havepeople = false;
			for(int i = 0 ; i < m_UserManage.GetOnLineUserCount(); i ++)
			{
				UserItemStruct * pFindUserItem =  m_UserManage.FindOnLineUserByIndex(i);
				if(pFindUserItem == NULL)
					continue;
				if(pFindUserItem->GameUserInfo.bDeskNO == bDesk && pFindUserItem->GameUserInfo.bUserState == USER_PLAY_GAME)//桌上游戏已经开始
				{
					havepeople = true;
					break;
				}
				if(pFindUserItem->GameUserInfo.bDeskNO == bDesk &&pFindUserItem->GameUserInfo.bDeskStation == station)//此坐位已经有人
				{
					havepeople = true;
					break;
				}
			}
			if(havepeople != true)
			{
				bDeskIndex = bDesk;
				bDeskStation = station;
				return true;
			}
		}
	}
	return true;
}

//查找空桌子
bool CGameRoomEx::OnFindNullDesk(BYTE &bDeskIndex,BYTE &bDeskStation)
{
	//从每一个桌子开始查找
	for(UINT desk = 0; desk < m_ComRoomInfo.uDeskCount; desk ++)
	{
		if (IsErrorDesk(desk))//20090227判断是否为不可用桌子
		{
			continue;
		}
		//坐位
		for(UINT station = 0; station < m_GameInfo.uDeskPeople; station++)
		{
			bool havepeople = false;
			int iPCount = m_UserManage.GetOnLineUserCount();
			for(int i = 0 ; i < iPCount; i ++)
			{
				UserItemStruct * pFindUserItem =  m_UserManage.FindOnLineUserByIndex(i);
				if(pFindUserItem == NULL)
					continue;
				if(pFindUserItem->GameUserInfo.bDeskNO == desk&&pFindUserItem->GameUserInfo.bUserState == USER_PLAY_GAME)//桌上游戏已经开始
				{
					//考虑捕鱼玩家是在游戏进行中进来的sdp2014-05-07
					CString ctext;
					ctext.Format("%d",NAME_ID);
					if (ctext.Left(3) == "330" && ctext.GetAt(4) == '0')
					{
						havepeople = false;
					}
					else
					{
						havepeople = true;
						break;
					}
					//////////////////////////////////
				}
				if(pFindUserItem->GameUserInfo.bDeskNO == desk &&pFindUserItem->GameUserInfo.bDeskStation == station)//此桌有人
				{
					havepeople = true;
					break;
				}
			}
			if(havepeople != true)
			{

				if(CountMachine(desk) >= m_bMachineDeskCount)//机器人不能够超过设定的数量
				{
					continue;
				}
	
				bDeskIndex = desk;
				bDeskStation = station;
				return true;
			}
		}
	}
	return true;
}

//查找指定桌子空位置(处理被邀请的时候处理)
BYTE CGameRoomEx::GetNullStation(BYTE bDeskIndex)
{
	BYTE bDeskStation = 255;
	//坐位
	for(UINT station = 0; station < m_GameInfo.uDeskPeople; station++)
	{
		if (IsErrorDesk(station))//20090227判断是否为不可用桌子
		{
			continue;
		}
		bool havepeople = false;
		for(int i = 0 ; i < m_UserManage.GetOnLineUserCount(); i ++)
		{
			UserItemStruct * pFindUserItem =  m_UserManage.FindOnLineUserByIndex(i);
			if(pFindUserItem == NULL)
				continue;
			if(pFindUserItem->GameUserInfo.bDeskNO == bDeskIndex && pFindUserItem->GameUserInfo.bUserState == USER_PLAY_GAME)//桌上游戏已经开始
			{
				havepeople = true;
				break;
			}
			if(pFindUserItem->GameUserInfo.bDeskNO == bDeskIndex &&pFindUserItem->GameUserInfo.bDeskStation == station)//此桌有人
			{
				havepeople = true;
				break;
			}
		}
		if(havepeople != true)
		{
			bDeskStation = station;
			break;;
		}
	}
	return bDeskStation;
}
//查找真人最多空桌子
bool CGameRoomEx::OnFindTureDesk(BYTE &bDeskIndex,BYTE &bDeskStation)
{
	bDeskIndex = 255;
	bDeskStation = 255;
	//int nPeopleCount = 0;
	//从每一个桌子开始查找
	for(UINT desk = 0; desk < m_ComRoomInfo.uDeskCount; desk ++)
	{
		if (IsErrorDesk(desk))//20090227判断是否为不可用桌子
		{
			continue;
		}
		//坐位
		for(UINT station = 0; station < m_GameInfo.uDeskPeople; station++)
		{
			bool havepeople = false;
			bool bVirtual = false;
			for(int i = 0 ; i < m_UserManage.GetOnLineUserCount(); i ++)
			{
				UserItemStruct * pFindUserItem =  m_UserManage.FindOnLineUserByIndex(i);
				if(pFindUserItem == NULL)
					continue;
				if (pFindUserItem->GameUserInfo.dwUserID < 0 && pFindUserItem->GameUserInfo.bDeskNO == desk)
				{
					bVirtual = true;
					break;
				}
				if(pFindUserItem->GameUserInfo.bDeskNO == desk&&pFindUserItem->GameUserInfo.bUserState == USER_PLAY_GAME)//桌上游戏已经开始
				{
					havepeople = true;
					break;
				}
				if(pFindUserItem->GameUserInfo.bDeskNO == desk &&pFindUserItem->GameUserInfo.bDeskStation == station)//此桌有人
				{
					havepeople = true;
					break;
				}
			}
			if (bVirtual)  //被封的座位，查找下一桌
			{
				continue;
			}
			if(havepeople != true)
			{

				if(CountMachine(desk) >= m_bMachineDeskCount)//机器人不能够超过2
				{

					continue;

				}
				if (CountTruePlayer(desk) > 0)
				{
					//nPeopleCount = CountTruePlayer(desk);
					bDeskIndex = desk;
					bDeskStation = station;
					return true;
				}
				if (bDeskIndex == 255)
				{
					bDeskIndex = desk;
					bDeskStation = station;
				}
				
				//return true;
			}
		}
	}
	return true;
}

//查找机器人最多空桌子
bool CGameRoomEx:: OnFindMachineDesk(BYTE &bDeskIndex,BYTE &bDeskStation)
{
	//从每一个桌子开始查找
	for(UINT desk = 0; desk < m_ComRoomInfo.uDeskCount; desk ++)
	{
		if (IsErrorDesk(desk))//20090227判断是否为不可用桌子
		{
			continue;
		}
		//坐位
		for(UINT station = 0; station < m_GameInfo.uDeskPeople; station++)
		{
			bool havepeople = false;
			bool bVirtual = false;
			for(int i = 0 ; i < m_UserManage.GetOnLineUserCount(); i ++)
			{
				UserItemStruct * pFindUserItem =  m_UserManage.FindOnLineUserByIndex(i);
				if(pFindUserItem == NULL)
					continue;
				if (pFindUserItem->GameUserInfo.dwUserID < 0 && pFindUserItem->GameUserInfo.bDeskNO == desk)
				{
					bVirtual = true;
					break;
				}
				if(pFindUserItem->GameUserInfo.bDeskNO == desk&&pFindUserItem->GameUserInfo.bUserState == USER_PLAY_GAME)//桌上游戏已经开始
				{
					havepeople = true;
					break;
				}
				if(pFindUserItem->GameUserInfo.bDeskNO == desk &&pFindUserItem->GameUserInfo.bDeskStation == station)//此坐号已经有人
				{
					havepeople = true;
					break;
				}
			}
			if (bVirtual)  //被封的座位，查找下一桌
			{
				continue;
			}
			if(havepeople != true)//非空
			{
				if(CountMachine(desk) == m_bMachineDeskCount)//机器人不能够超过2
				{
					continue;
				}
				if(CountTruePlayer(desk) >= 1 
					&& m_GameInfo.pMeUserInfo->GameUserInfo.isVirtual/*IsMachineID(m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)*/)//机器人不能够与人坐
					continue;
				bDeskIndex = desk;
				bDeskStation = station;
				return true;
			}
		}
	}
	return true;
}



//统计某桌机器人数量
BYTE CGameRoomEx::CountMachine(BYTE bDeskIndex)
{
	int iCount = 0;
	for(int i = 0 ; i < m_UserManage.GetOnLineUserCount(); i ++)
	{
		UserItemStruct * pFindUserItem =  m_UserManage.FindOnLineUserByIndex(i);
		if(pFindUserItem == NULL)
			continue;
		if(pFindUserItem->GameUserInfo.bDeskNO == bDeskIndex && pFindUserItem->GameUserInfo.isVirtual/*IsMachineID(pFindUserItem->GameUserInfo.dwUserID)*/)
		{
			iCount ++ ;
		}
	}
	return iCount;
}
//统计某桌真人数量
BYTE CGameRoomEx::CountTruePlayer(BYTE bDeskIndex)
{
	int iCount = 0;
	for(int i = 0 ; i < m_UserManage.GetOnLineUserCount(); i ++)
	{
		UserItemStruct * pFindUserItem =  m_UserManage.FindOnLineUserByIndex(i);
		if(pFindUserItem == NULL)
			continue;
		if(pFindUserItem->GameUserInfo.bDeskNO == bDeskIndex && !pFindUserItem->GameUserInfo.isVirtual/*IsPlayerID(pFindUserItem->GameUserInfo.dwUserID)*/)
		{
			iCount ++ ;
		}
	}
	return iCount;
}

//是否需要换桌子
bool CGameRoomEx::NeedChange(BYTE bDeskIndex)
{
	int count = 0;	
	for(int i = 0 ; i < m_UserManage.GetOnLineUserCount(); i ++)
	{
		UserItemStruct * pFindUserItem =  m_UserManage.FindOnLineUserByIndex(i);
		if(pFindUserItem == NULL)
			continue;
		if(pFindUserItem->GameUserInfo.bDeskNO == bDeskIndex)//桌上游戏已经开始
		{
			count ++;
		}
	}
	if(count >= 2 || m_GameInfo.pMeUserInfo->GameUserInfo.bUserState == USER_PLAY_GAME)
		return false;

	return true;
}

//查找另外一张桌子
bool CGameRoomEx::FindOtherDesk()
{
	OnSitDesk();
	return true;
}

//发送用户信息
bool CGameRoomEx::SendUserInfo()
{
	//发送用户登录信息
	MSG_GR_S_RoomLogon RoomLogon;
	RoomLogon.uGameVer = 0;//m_GameInfo.dwGameLSVer;
	RoomLogon.uRoomVer = GAME_PLACE_MAX_VER;
	RoomLogon.uNameID = m_ComRoomInfo.uNameID;
	RoomLogon.dwUserID = m_pLogonInfo.dwUserID;
	lstrcpy(RoomLogon.szMD5Pass,m_pLogonInfo.szMD5Pass);

	//发送登陆
	m_TCPSocket->SendData(&RoomLogon,sizeof(RoomLogon),MDM_GR_LOGON,ASS_GR_LOGON_BY_ID,0);

	return true;
}

//用户列表处理
bool CGameRoomEx::OnUserListMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	switch (pNetHead->bAssistantID)
	{
	case ASS_GR_ONLINE_USER:
	case ASS_GR_NETCUT_USER:	//用户列表
		{
			UINT uUserCount = uDataSize/sizeof(UserInfoStruct);

			if (uUserCount>0L)
			{
				//定义变量
				UserItemStruct * pUserItem = NULL;
				UserInfoStruct * pUserInfo = (UserInfoStruct *)pNetData;

				for (UINT i = 0; i < uUserCount; i ++)
				{
					pUserItem = m_UserManage.AddGameUser(pUserInfo+i,1,0);
				}
			}
			CGameRoomEx::m_nOnLinePeople = uUserCount;
			return true;
		}
	case ASS_GR_DESK_STATION:	//桌子状态 
		{
			if (uDataSize!=sizeof(MSG_GR_DeskStation)) return false;
			MSG_GR_DeskStation * pDeskStation=(MSG_GR_DeskStation *)pNetData;
			for (UINT i=0;i<pNetHead->bHandleCode;i++)
			{
				if ((pDeskStation->bVirtualDesk[i/8]&(1<<(i%8)))!=0)    //change by yjj 090325
				{
					UserInfoStruct pUserInfo;      //玩家信息
					int c=0;

					for (int k=0; k<m_GameInfo.uDeskPeople;k++)
					{
						
						pUserInfo.bUserState=USER_PLAY_GAME;
						pUserInfo.bDeskNO=i;
						pUserInfo.bDeskStation=k;
						pUserInfo.dwUserID = -1;//虚拟用户

						m_UserManage.AddGameUser(&pUserInfo,1,0);
					}
				}
			}
			return true;
		}
	}
	return true;
}


//房间信息处理
bool CGameRoomEx::OnRoomMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	ASSERT(pNetHead->bMainID==MDM_GR_ROOM);

	switch (pNetHead->bAssistantID)
	{
	case ASS_GR_NORMAL_TALK:	//普通聊天
		{
			
			return true;
		}
	case ASS_GR_SHORT_MSG:		//短信息聊天
		{
			return true;
		}
	case ASS_GR_USER_AGREE:		//用户同意
		{
			return true;
		}
	case ASS_GR_CONTEST_APPLY:
		{
			if (uDataSize != sizeof(MSG_GR_ContestApply))	return false;

			MSG_GR_ContestApply *pMsgContestApply = (MSG_GR_ContestApply*)pNetData;
			CString str;
			str.Format("robot-----------switch(%d)", pMsgContestApply->iApplyResult);
			OutputDebugString(str);

			//根据服务端反馈的报名状况做处理
			switch (pMsgContestApply->iApplyResult)
			{
			case 1://报名成功
			case 3://已经报名
			{
				m_iContestState = 1;
				if (pMsgContestApply->iContestBegin == 1)
				{
					m_iContestState = 2;
					OutputDebugString("robot---------比赛开始，发送入列消息");
					//此用户报名后达到开赛人数要求，则发送入列消息
					m_TCPSocket->SendData(MDM_GR_USER_ACTION,ASS_GR_JOIN_QUEUE,0);
				}
				break;
			}
			case 7://房间报名人数达到上限，需切换房间
				{
					OutputDebugString("robot---------房间报名人数达到上限，需切换房间");
					CloseGameRoom();
					break;
				}
			case 8://用户身上金币不够报名费，需要先取钱
				{
					OutputDebugString("robot-----------用户身上金币不够报名费，需要先取钱");
					AiControlBank();
					CloseGameRoom();
					break;
				}
			default:
				{
					CloseGameRoom();
					break;
				}
			}
			return true;
		}	
	case ASS_GR_INIT_CONTEST:	
	case ASS_GR_CONTEST_WAIT_GAMEOVER:
		{
			return true;
		}
	case ASS_GR_CAHNGE_ROOM:
		{
			if (0 == m_iContestState)
			{
				CloseGameRoom();//关闭房间				
			}
			return true;
		}
	case ASS_GR_CONTEST_GAMEOVER:
	case ASS_GR_CONTEST_KICK:
		{
			CloseGameRoom();//关闭房间
			return true;
		}
	case ASS_GR_USER_CONTEST:
		{
			//处理数据
			if (uDataSize != sizeof(MSG_GR_ContestChange)) return false;
			MSG_GR_ContestChange* _p = (MSG_GR_ContestChange*)pNetData;

			UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(_p->dwUserID);
			if (pUserItem != NULL)
			{
				//被淘汰玩家此时的排名比在线人数的值要大，比如用户被淘汰此时排名为7，则在线人数为6，此处修改一下显示
				_p->iRemainPeople = _p->iRemainPeople>_p->iRankNum ? _p->iRemainPeople : _p->iRankNum;
				pUserItem->GameUserInfo.iContestCount = _p->iContestCount;
				pUserItem->GameUserInfo.i64ContestScore = _p->i64ContestScore;
				pUserItem->GameUserInfo.iRankNum = _p->iRankNum;
				pUserItem->GameUserInfo.iRemainPeople = _p->iRemainPeople;

				if( pUserItem->GameUserInfo.dwUserID  == m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)
				{
					CString str;
					str.Format("robot----大厅程序------------id = %d rank = %d RemainPeople = %d DeskNo = %d DeskStation = %d",
						m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID,
						m_GameInfo.pMeUserInfo->GameUserInfo.iRankNum,
						m_GameInfo.pMeUserInfo->GameUserInfo.iRemainPeople,
						m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO,
						m_GameInfo.pMeUserInfo->GameUserInfo.bDeskStation);
					OutputDebugString(str);
				}
			}
			return true;
		}
	case ASS_GR_GAME_BEGIN:		//游戏开始
		{
			//效验数据
			if (uDataSize!=0) return false;
			OutputDebugString("robot--------------接收ASS_GR_GAME_BEGIN");
			//处理数据
			BYTE bDeskIndex = pNetHead->bHandleCode;
			for(UINT i = 0 ; i < m_UserManage.GetOnLineUserCount(); i ++)
			{
				UserItemStruct * pFindUserItem =  m_UserManage.FindOnLineUserByIndex(i);
				if(pFindUserItem == NULL)
					continue;
				if(pFindUserItem->GameUserInfo.bDeskNO == bDeskIndex)
				{
					pFindUserItem->GameUserInfo.bUserState = USER_PLAY_GAME;					
				}
			}
			////////////加入自动发言zht2008-7-18
			if(m_GameInfo.pMeUserInfo!=NULL&&bDeskIndex == m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO)
			{
				//发送聊天信息
				CString strBuffer;
				CString s = CINIFile::GetAppPath ();/////本地路径
				CINIFile f( s +"automsg.ini");
				CString key = TEXT("AUTOMSG");
				int AutoMsg=f.GetKeyVal(key,"IsAuto",1);
				if (AutoMsg == 0)
					return true;
				TCHAR szName[21]={0};
				srand((unsigned)GetTickCount()+m_pLogonInfo.dwUserID);
				int i = rand()%500;
				if (i > 100)
					return true;
				wsprintf(szName,"MSG%d",i);

				strBuffer = f.GetKeyVal(key,szName,"");
				if (strBuffer.IsEmpty())
				{
					return true;
				}
				int r = f.GetKeyVal(key,"r",0);
				int g = f.GetKeyVal(key,"g",255);
				int b = f.GetKeyVal(key,"b",255);
				MSG_GR_RS_NormalTalk Talk;
				Talk.iLength=strBuffer.GetLength();
				Talk.dwSendID=m_pLogonInfo.dwUserID;
				Talk.dwTargetID=0L;
				lstrcpy(Talk.szMessage,strBuffer);
				Talk.crColor=RGB(r,g,b);
				if (i > 50)
				{
					m_TCPSocket->SendData(&Talk,sizeof(Talk)-sizeof(Talk.szMessage)+Talk.iLength+1,MDM_GM_GAME_FRAME,ASS_GM_NORMAL_TALK,FALSE);
				}
				else
				{
					m_TCPSocket->SendData(&Talk,sizeof(Talk)-sizeof(Talk.szMessage)+Talk.iLength+1,MDM_GR_ROOM,ASS_GR_NORMAL_TALK,0);
				}
			}
           /////////////////自动发言END
			return true;
		}
	case ASS_GR_GAME_END:		//游戏结束
		{
			//效验数据
			if (uDataSize!=0) 
			{
				return false;
			}
			if (m_GameInfo.dwRoomRule & GRR_CONTEST)
			{
				//比赛场时，结束一盘后房间不需采取任何措施，直接等待服务端配桌
				return true;
			}
		    bool bHaveplayer=false; //是否有真实玩家
			//处理数据
			BYTE bDeskIndex = pNetHead->bHandleCode;
			for(int i = 0 ; i < m_UserManage.GetOnLineUserCount(); i ++)
			{
				UserItemStruct * pFindUserItem =  m_UserManage.FindOnLineUserByIndex(i);
				if(pFindUserItem == NULL)
				{
					continue;
				}
				if(pFindUserItem->GameUserInfo.bDeskNO == bDeskIndex)
				{
					pFindUserItem->GameUserInfo.bUserState = USER_DESK_AGREE;
					if (pFindUserItem->GameUserInfo.isVirtual == false)
					{
						bHaveplayer = true;
					}
				}
			}
			//本桌游戏结束有1/2机率离开
			if(m_GameInfo.pMeUserInfo!=NULL&&bDeskIndex == m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO)
			{
				CTime t = CTime::GetCurrentTime();
				TTime_day t_day(t.GetHour(), t.GetMinute());
				if (CmpTime_dat(m_ExitTime, t_day) <= -1)
				{
					OnSocketCloseEvent();
					return true;
				}
			}


			if (m_nOnLinePeople > CAuto_AIDlg::m_nRoomPeopleSize)
			{
				m_nOnLinePeople--;
                StandingDesk();
				CloseGameRoom();
			}

			if(m_GameInfo.pMeUserInfo!=NULL&&bDeskIndex == m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO)
			{
				if(rand()%5 == 1)
				{
					if (m_bAutoDesk)
					{
						StandingDesk();
					}
				}

				if (bHaveplayer && m_bMachineAndPlay == false)//不与真人同桌
				{
					StandingDesk();
				}

				if(m_GameInfo.pMeUserInfo!=NULL)
				{
					__int64 Temp = (m_GameInfo.pMeUserInfo->GameUserInfo.i64Money+m_GameInfo.pMeUserInfo->GameUserInfo.i64Bank);
					
					m_iMyMoney = Temp;
					
					AiControlBank();
				}

				if (g_bUserAI)
				{
					CString ad=CINIFile::GetAppPath ();
					CINIFile f( ad + "AIConfig.ini");
					int static iConnect=1;
					CString m_key = "Action";
					CString sp1 = f.GetKeyVal(m_key,"GameFinishStand", "0.3");
					double fp1 = atof(sp1);
					if (randf() < fp1)
					{
						StandingDesk();		
					}

					CString sp2 = f.GetKeyVal(m_key,"GameFinishLeave", "0.1");
					double fp2 = atof(sp2);
					if (randf() < fp2)
					{
						OnSocketCloseEvent();
					}
				}
			}				
			return true;
		}
	case ASS_GR_INSTANT_UPDATE:		//即时更新分数金币
		{
			return TRUE;
		}
	case ASS_GR_USER_POINT:		//用户金币积分
		{
			//TCHAR sz[200];
			//wsprintf(sz,"pNetHead->bHandleCode = %d,uDataSize  = %d,MSG_GR_S_RefalshMoney = %d",
			//	pNetHead->bHandleCode,uDataSize,sizeof(MSG_GR_S_RefalshMoney));
			//WriteStr(sz);
			if(pNetHead->bHandleCode==10)//同步金币
			{
				//效验数据
				if (uDataSize!=sizeof(MSG_GR_S_RefalshMoney)) 
				{
					return false;
				}
				MSG_GR_S_RefalshMoney * pReflashMoney=(MSG_GR_S_RefalshMoney*)pNetData;

				//处理数据
				UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(pReflashMoney->dwUserID);
				if (pUserItem==NULL) 
				{
					return false;
				}

				pUserItem->GameUserInfo.i64Money=pReflashMoney->i64Money;

				if(m_GameInfo.pMeUserInfo!=NULL && pReflashMoney->dwUserID == m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)
				{
					__int64 Temp = (m_GameInfo.pMeUserInfo->GameUserInfo.i64Money+m_GameInfo.pMeUserInfo->GameUserInfo.i64Bank);

	
					m_iMyMoney = Temp;

	
					//AiControlBank();
				}

				//更新用户
				//m_UserManage.UpdateGameUser(pUserItem);

				return true;
			}else if(pNetHead->bHandleCode==11)//同步经验值
			{
				//效验数据
				if (uDataSize!=sizeof(MSG_GR_S_RefalshMoney)) 
				{
					return false;
				}
				MSG_GR_S_RefalshMoney * pReflashMoney=(MSG_GR_S_RefalshMoney*)pNetData;

				//处理数据
				UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(pReflashMoney->dwUserID);
				if (pUserItem==NULL) 
				{
					return false;
				}
				pUserItem->GameUserInfo.dwPoint=pReflashMoney->i64Money;

				if(m_GameInfo.pMeUserInfo!=NULL && pReflashMoney->dwUserID == m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)
				{
					__int64 Temp = (m_GameInfo.pMeUserInfo->GameUserInfo.i64Money+m_GameInfo.pMeUserInfo->GameUserInfo.i64Bank);

//					m_i64NowMoney+=(Temp-m_iMyMoney);

					m_iMyMoney = Temp;

	//				::PostMessage(GetParent()->m_hWnd,IDM_UPDATE_USER,0,0);

					//AiControlBank();
				}
				//更新用户
				//m_UserManage.UpdateGameUser(pUserItem);
				return true;
			}

			//效验数据
			if (uDataSize!=sizeof(MSG_GR_R_UserPoint)) return false;
			MSG_GR_R_UserPoint * pUserPoint=(MSG_GR_R_UserPoint *)pNetData;

			//处理数据
			UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(pUserPoint->dwUserID);
			if (pUserItem!=NULL)
			{
				//设置数据

				pUserItem->GameUserInfo.dwPoint+=pUserPoint->dwPoint;
				pUserItem->GameUserInfo.i64Money+=pUserPoint->dwMoney;
				pUserItem->GameUserInfo.uWinCount+=pUserPoint->bWinCount;
				pUserItem->GameUserInfo.uLostCount+=pUserPoint->bLostCount;
				pUserItem->GameUserInfo.uMidCount+=pUserPoint->bMidCount;
				pUserItem->GameUserInfo.uCutCount+=pUserPoint->bCutCount;

				if(m_GameInfo.pMeUserInfo!=NULL && pUserPoint->dwUserID == m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)
				{
					__int64 Temp = (m_GameInfo.pMeUserInfo->GameUserInfo.i64Money+m_GameInfo.pMeUserInfo->GameUserInfo.i64Bank);

//					m_i64NowMoney+=(Temp-m_iMyMoney);

					m_iMyMoney = Temp;

//					::PostMessage(GetParent()->m_hWnd,IDM_UPDATE_USER,0,0);

					//AiControlBank();
				}
			}
			return true;
		}
	case ASS_GR_INVITEUSER:		//邀请用户
		{
			//效验数据
			if (uDataSize!=sizeof(MSG_GR_SR_InviteUser)) return false;
			MSG_GR_SR_InviteUser * pInviteUser=(MSG_GR_SR_InviteUser *)pNetData;
			if(m_bEnterStyle < 7)
			{
				return true;//不接受邀请
			}
			if (!m_bMachineAndPlay)
			{
				return true;
			}

			BYTE bDeskIndex = pNetHead->bHandleCode;
			BYTE bDeskStation = GetNullStation(bDeskIndex);

			if(bDeskIndex == 255||bDeskStation == 255 ||m_GameInfo.pMeUserInfo->GameUserInfo.bUserState == USER_PLAY_GAME)
			{
				return true;
			}
			MSG_GR_S_UserSit UserSit;
			UserSit.bDeskIndex = bDeskIndex;
			UserSit.bDeskStation = bDeskStation;

			//下面发送坐下的信息
			m_TCPSocket->SendData(&UserSit,sizeof(UserSit),MDM_GR_USER_ACTION,ASS_GR_USER_SIT,0);
			return true;
		}
	default:
		return true;
	}
	return true;
}

//用户动作处理
bool CGameRoomEx::OnUserActionMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	switch (pNetHead->bAssistantID)
	{
	case ASS_GR_USER_HIT_BEGIN:
		{
			//效验数据
			if(uDataSize!=sizeof(MSG_GM_WatchSet))
				return false;
			MSG_GM_WatchSet* pUserID = (MSG_GM_WatchSet*) pNetData;

			//处理数据
			UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(pUserID->dwUserID);
			pUserItem->GameUserInfo.bUserState = USER_DESK_AGREE;
			return true;
		}
	case ASS_GR_USER_UP:		//用户起来
		//case ASS_GR_WATCH_UP:		//旁观起来
		{
			//效验数据
			if (uDataSize!=sizeof(MSG_GR_R_UserSit)) return false;
			MSG_GR_R_UserSit * pUserUp=(MSG_GR_R_UserSit *)pNetData;

			//处理数据
			UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(pUserUp->dwUserID);

			if (pUserItem!=NULL)
			{
				//桌号
				BYTE bTemp = pUserItem->GameUserInfo.bDeskNO;
				//设置数据
				pUserItem->GameUserInfo.bDeskNO = 255;
				pUserItem->GameUserInfo.bDeskStation = 255;
				pUserItem->GameUserInfo.bUserState = pUserUp->bUserState;

				if(pUserUp->dwUserID == m_pLogonInfo.dwUserID)//自己起来
				{
					if(m_IGameFrame != NULL)
					{
						m_IGameFrame->AFCCloseFrame();
						delete m_IGameFrame;
					}
					m_IGameFrame= NULL;
					return true;
				}
				//本桌
				if(bTemp == m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO)
				{
					//if(NeedChange(bTemp))
					//{
					//	//重新找坐
					//	//FindOtherDesk();
					//}
				}

			}
			return true;
		}
	case ASS_GR_USER_SIT:		//用户坐下
		//case ASS_GR_WATCH_SIT:		//旁观坐下
		{
	//效验数据

			if (uDataSize!=sizeof(MSG_GR_R_UserSit)) 
			{
				return false;
			}
			MSG_GR_R_UserSit * pUserSit=(MSG_GR_R_UserSit *)pNetData;

			//处理数据
			UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(pUserSit->dwUserID);
			if (pUserItem!=NULL)
			{
				if(pUserItem->GameUserInfo.dwUserID == m_pLogonInfo.dwUserID )//本人坐下
				memset(m_bErrorDeskFlag, 0, sizeof(m_bErrorDeskFlag));
			}
			if (pUserItem!=NULL)
			{
				/////////////////////////////////////////
				//设置数据
				pUserItem->GameUserInfo.bDeskNO=pUserSit->bDeskIndex;
				pUserItem->GameUserInfo.bDeskStation=pUserSit->bDeskStation;

				pUserItem->GameUserInfo.bUserState=pUserSit->bUserState;
				if(pUserItem->GameUserInfo.dwUserID == m_pLogonInfo.dwUserID )//本人坐下
				{OutputDebugString("cqro::----本人坐下");
					//3以下级别不处理坐下
					if( m_bEnterStyle < 3)
					{
						return true;
					}
					StartGameClient();
					//控制客户端
					CM_UserState_Change ControlMessage;
					ControlMessage.bActionCode=ACT_USER_SIT;
					ControlMessage.bDeskStation = m_GameInfo.pMeUserInfo->GameUserInfo.bDeskStation;
					ControlMessage.pUserItem = m_GameInfo.pMeUserInfo;
					ControlGameClient(CM_USER_STATE,&ControlMessage,sizeof(ControlMessage));
				}else
				{
					if (m_bMachineAndPlay == false)//不与机真人同桌
					{
					
						if(pUserItem->GameUserInfo.bDeskNO == m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO)
						{
							//如果是真人,机器人起身
							if(/*IsPlayerID(pUserItem->GameUserInfo.dwUserID)*/!pUserItem->GameUserInfo.isVirtual
								&&m_GameInfo.pMeUserInfo->GameUserInfo.isVirtual/*IsMachineID(m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)*/)
								StandingDesk();

						}
					}
			
				}
			}
			return true;
		}
	case ASS_GR_QUEUE_USER_SIT:
		{
			int* pData = (int*)pNetData;
			if (pNetHead->uMessageSize - sizeof(NetMessageHead) != sizeof(int) * (m_GameInfo.uDeskPeople*3 + 1)) return false;
			int _DeskIndex = *pData;
			pData++;

			bool bIsSelf = false;
			for (int i = 1; i < m_GameInfo.uDeskPeople + 1; i++)
			{
				UserItemStruct * pUserItem = m_UserManage.FindOnLineUser((long)*pData++);
				if (pUserItem != NULL)
				{
					// PengJiLin, 2010-5-19，排队机不在桌子上显示玩家信息
					if(pUserItem->GameUserInfo.dwUserID !=m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID && m_GameInfo.dwRoomRule & GRR_NOTCHEAT)
						strcpy(pUserItem->GameUserInfo.nickName, "玩家");

					pUserItem->GameUserInfo.bDeskNO= _DeskIndex;
					pUserItem->GameUserInfo.bDeskStation = i - 1;
					pUserItem->GameUserInfo.bUserState = USER_SITTING;
					pUserItem->GameUserInfo.iRankNum = (long)(*(pData++));
					pUserItem->GameUserInfo.iRemainPeople = (long)(*(pData++));
					if ((pUserItem->GameUserInfo.dwUserID == m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID))
					{				
						bIsSelf = true;
						m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO = _DeskIndex;
						m_GameInfo.pMeUserInfo->GameUserInfo.bDeskStation = i - 1;
					}
				}
			}

			if (bIsSelf)
			{
				pData = (int*)pNetData;
				pData++;

				//关闭游戏端
				if(m_IGameFrame!=NULL && !(m_GameInfo.dwRoomRule & GRR_CONTEST))
				{
					m_IGameFrame->AFCCloseFrame();
				}
				StartGameClient();

				CM_UserState_Change ControlMessage;
				ControlMessage.bActionCode= ACT_USER_SIT;
				ControlMessage.bDeskStation= m_GameInfo.pMeUserInfo->GameUserInfo.bDeskStation;
				ControlMessage.pUserItem=m_GameInfo.pMeUserInfo;
				ControlGameClient(CM_USER_STATE,&ControlMessage,sizeof(ControlMessage));
			}
			return true ;
		}
	case ASS_GR_USER_COME:		//用户进入
		{
			//效验数据
			if (uDataSize!=sizeof(MSG_GR_R_UserCome)) return false;
			MSG_GR_R_UserCome * pUserCome=(MSG_GR_R_UserCome *)pNetData;

			//处理数据
			UserItemStruct * pUserItem=m_UserManage.AddGameUser(&pUserCome->pUserInfoStruct,1,0);
			CGameRoomEx::m_nOnLinePeople = m_UserManage.GetOnLineUserCount();
			return true;
		}
	case ASS_GR_USER_LEFT:		//用户离开
		{
			//效验数据
			if (uDataSize!=sizeof(MSG_GR_R_UserLeft)) return false;
			MSG_GR_R_UserLeft * pUserLeft=(MSG_GR_R_UserLeft *)pNetData;


			UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(pUserLeft->dwUserID);
			if (pUserItem!=NULL)
			{
				m_UserManage.DeleteGameUser(pUserItem);
			}
			CGameRoomEx::m_nOnLinePeople = m_UserManage.GetOnLineUserCount();

			if (m_nOnLinePeople < CAuto_AIDlg::m_nRoomPeopleSize)
			{
				::PostMessage(GetParent()->m_hWnd,IDM_UPDATE_USER,0,0);
			}
			return true;
		}
	case ASS_GR_USER_CUT:		//用户断线
		{
			//效验数据
			if (uDataSize!=sizeof(MSG_GR_R_UserCut)) return false;
			MSG_GR_R_UserCut * pUserCut=(MSG_GR_R_UserCut *)pNetData;
			//处理数据
			UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(pUserCut->dwUserID);
			if (pUserItem!=NULL)
			{
				//设置数据
				pUserItem->GameUserInfo.bUserState=USER_CUT_GAME;
			}

			return true;
		}
	case ASS_GR_SET_TABLE_BASEPOINT:	//设置用户桌子倍数
		{	
			return true;
		}
	case ASS_GR_SIT_ERROR:		//坐下错误
		{
			//20090227机器人坐下错误处理
			if (m_bErrorDeskCount > 20)
			{
				m_bErrorDeskCount = 0;
				memset(m_bErrorDesk, 255, sizeof(m_bErrorDesk));
			}
			m_bErrorDesk[m_bErrorDeskCount] = m_bNowDesk;
			m_bErrorDeskCount++;

			m_bErrorDeskFlag[m_bNowDesk] = true;

			OnSitDesk();
			return true;
		}
	}
	return true;
}

//启动游戏客户端
bool CGameRoomEx::StartGameClient()
{
	try
	{
		//获取对象 
		if (m_IGameFrame != NULL)
		{
			delete m_IGameFrame;
			m_IGameFrame = NULL;
		}
		//if (m_IGameFrame==NULL)
		{
			m_IGameFrame = new CClientGameDlg();

			m_IGameFrame->InitFrame(&m_GameInfo,this);
		}
		return true;
	}
	catch (...) {}

	m_IGameFrame->AFCCreateFrame();

	return false;
}

//发送控制消息 
UINT CGameRoomEx::ControlGameClient(UINT uControlCode, void * pControlData, UINT uDataSize)
{
	try
	{
		if (m_IGameFrame!=NULL)
			m_IGameFrame->OnControlMessage(uControlCode,pControlData,uDataSize);
	}
	catch (...) {}
	return 0;
}

//计时器消息
LRESULT CGameRoomEx::SetGameTime(WPARAM wparam,LPARAM lparam)
{
	SetTimer(wparam,lparam,NULL);
	return TRUE;
}

//关闭游戏,在游戏中的机器人发强退消息，
//add 20090211
//zht
LRESULT CGameRoomEx::OnCloseGame(WPARAM wparam,LPARAM lparam)
{
	if (NULL != m_IGameFrame)
	{
		CClientGameDlg* FrameGame = (CClientGameDlg*)m_IGameFrame;
		//sdp20140624 写死判断条件，不用宏，防止每次编译可能不通过
		if (FrameGame->GetStationParameter() >= 2 &&FrameGame->GetStationParameter() <= GS_WAIT_NEXT)
		{
			SendGameData(MDM_GM_GAME_FRAME,ASS_GM_FORCE_QUIT,0);
		}

		m_IGameFrame->AFCCloseFrame();
	}
	return TRUE;
}

//计时器消息
LRESULT CGameRoomEx::KillGameTime(WPARAM wparam,LPARAM lparam)
{
	if((UINT)wparam == 0)
	{
		for(int i = 100;i < 110;i++)
		{
			KillTimer(i);
		}
		return 0;

	}
	KillTimer((UINT)wparam);
	return TRUE;
}

//计时器
void CGameRoomEx::OnTimer(UINT_PTR uIDEvent)
{
	if(uIDEvent >= 100 && uIDEvent <= 200 )//100-110计时器为游戏逻辑使用
	{
		KillTimer(uIDEvent);
		if (m_IGameFrame!=NULL)
		{
			m_IGameFrame->OnGameTimer(0,uIDEvent,0);
		}
		return ;
	}
	//1-100为房间使用
	switch(uIDEvent)
	{
	case ID_CHECK_STATE://当前状态处理
		{
			//游戏中或者使用银行中，不游戏
			if(m_GameInfo.pMeUserInfo == NULL || m_GameInfo.pMeUserInfo->GameUserInfo.bUserState == USER_PLAY_GAME || m_UseBank)
			{
				///rqffffffffffffffffffffff
			/*	if (g_bUserAI)
				{
					CString ad=CINIFile::GetAppPath ();
					CINIFile f( ad + "AIConfig.ini");
					int static iConnect=1;
					CString m_key = "Action";


					CString sp2 = f.GetKeyVal(m_key,"GameForceLeave", "0.1");//自动登入时间
					double fp2 = atof(sp2);
					if (randf() < fp2)
					{OutputDebugString("cqro::------------------关闭4");
						OnSocketCloseEvent();
					}
				}*/
				break;
			}
			
			if (m_GameInfo.dwRoomRule & GRR_CONTEST)
			{
				KillTimer(ID_CHECK_STATE);
				//比赛场机器人开始报名
				m_TCPSocket->SendData(MDM_GR_USER_ACTION, ASS_GR_CONTEST_APPLY, 0);				
				return;
			}
			//金币不够者(仅适用于金币场)
			if(m_GameInfo.pMeUserInfo->GameUserInfo.i64Money < m_GameInfo.uLessPoint && m_GameInfo.uComType == TY_MONEY_GAME)
			{
				KillTimer(ID_CHECK_STATE);
				KillTimer(ID_CLEAR_ERRORDESK);
				CloseGameRoom();
				//g_ErrorUser.insert(std::pair<std::string,int>(m_GameInfo.pMeUserInfo->GameUserInfo.szName,m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID));
				
				return ;
			}
			if(m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO == 255)//未坐下启动坐下计时器
			{
				OnSitDesk();
			}
			else
			{
				///rqffffffffffffffffffffff
			/*	if (g_bUserAI)
				{
					CString ad=CINIFile::GetAppPath ();
					CINIFile f( ad + "AIConfig.ini");
					int static iConnect=1;
					CString m_key = "Action";
					CString sp1 = f.GetKeyVal(m_key,"GameSitStand", "0.33");//自动登入时间
					double fp1 = atof(sp1);
					if (randf() < fp1)
						StandingDesk();

					CString sp2 = f.GetKeyVal(m_key,"GameSitLeave", "0.1");//自动登入时间
					double fp2 = atof(sp2);
					if (randf() < fp2)
					{
						OutputDebugString("cqro::------------------关闭5");
						OnSocketCloseEvent();
					}
				}*/

				if(rand()%20 > 5 )//邀请玩家
				{
					//不处理大家乐不需要机器人邀请人
					//OnInvitePlay();
					break;
				}
				else
				{
					if (m_bAutoDesk)
					{
						StandingDesk();//起立
						break;
					}
				}
				m_CheckCount++;
				if (m_CheckCount > m_CheckCountMax)
				{
					StandingDesk();//起立
					break;
				}
			}
			break;
		}
	case ID_CLEAR_ERRORDESK:
		{
			//20090227机器人坐下错误处理
			m_bErrorDeskCount = 0;
			memset(m_bErrorDesk, 255, sizeof(m_bErrorDesk));

			memset(m_bErrorDeskFlag, 0, sizeof(m_bErrorDeskFlag));
			

			break;
		}
	case ID_CHECK_AIBANK:
		{
			KillTimer(ID_CHECK_AIBANK);
			m_UseBank = FALSE;
			//AiControlBank();
			break;
		}
	case IDT_GAMEEXIT:
		{
			CTime t = CTime::GetCurrentTime();
			TTime_day t_day(t.GetHour(), t.GetMinute());
			if (CmpTime_dat(m_ExitTime, t_day) <= -1)
			{
				if (m_GameInfo.pMeUserInfo != NULL && 
					m_GameInfo.pMeUserInfo->GameUserInfo.bUserState != USER_PLAY_GAME)
				{
					OutputDebugString("cqro::------------------关闭6");
					OnSocketCloseEvent();
				}
			}
			break;
		}
	}
	return __super::OnTimer(uIDEvent);
}

//邀请加入
void CGameRoomEx::OnInvitePlay()
{
	long dwInvertID = GetInvertID();
	if(dwInvertID == 0)
		return ;
	//发送数据
	MSG_GR_SR_InviteUser InviteUser;
	InviteUser.dwUserID = m_pLogonInfo.dwUserID;
	InviteUser.dwTargetID = dwInvertID;
	m_TCPSocket->SendData(&InviteUser,sizeof(InviteUser),MDM_GR_ROOM,ASS_GR_INVITEUSER,m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO);
	return;
}

//获取邀请对象
LONG CGameRoomEx::GetInvertID()
{
	//在所有在线用户中查找
	for(int i = 0 ; i < m_UserManage.GetOnLineUserCount(); i ++)
	{
		UserItemStruct * pFindUserItem =  m_UserManage.FindOnLineUserByIndex(i);
		if(pFindUserItem == NULL)
		{
			continue;
		}
		if(pFindUserItem->GameUserInfo.bDeskNO == m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO //同桌
			|| pFindUserItem->GameUserInfo.bUserState == USER_PLAY_GAME)					   //桌上游戏已经开始
		{
			continue;
		}
		//玩家是否金币足够
		if(pFindUserItem->GameUserInfo.i64Money < m_GameInfo.uLessPoint)
		{
			continue;
		}
		//处理是否邀请机器人
		if(pFindUserItem->GameUserInfo.isVirtual/*IsMachineID(pFindUserItem->GameUserInfo.dwUserID)*/)
		{
			continue;
		}
		return pFindUserItem->GameUserInfo.dwUserID;
	}
	return 0;
}
//使用isVirtual来判定是否机器人
//是否为机器人ID
//bool CGameRoomEx::IsMachineID(LONG dwUserID)
//{
//	if(dwUserID <= m_iMachineCheckTime)
//	{
//		return true;
//	}
//	return false;
//}
////是否为机器人ID
//bool CGameRoomEx::IsPlayerID(LONG dwUserID)
//{
//	if(dwUserID > m_iMachineCheckTime)
//	{
//		return true;
//	}
//	return false;
//}
//是否为不可用桌子
bool CGameRoomEx::IsErrorDesk(BYTE desk)
{
	return m_bErrorDeskFlag[desk];

	for (int i=0; i<m_bErrorDeskCount; i++)
	{
		if (m_bErrorDesk[i] == desk)
		{
			return true;
		}
	}
	return false;
}


///钱柜划账付钱
bool CGameRoomEx::OnAboutMoney(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	if (NULL == m_GameInfo.pMeUserInfo)
	{
		return true;
	}
	/*CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");

	TCHAR szNum[32];
	/// 如果游戏启动了，则发送到游戏客户端
	if (NULL != m_IGameFrame)
	{
	m_IGameFrame->OnSocketReadEvent(pNetHead, pNetData, uDataSize, pClientSocket);
	}*/
	switch (pNetHead->bAssistantID)
	{
	case ASS_GR_CHECK_OUT://取钱
	case ASS_GR_CHECK_OUT_INGAME:	// 游戏中取钱也需要更新，但不提示对话框
		{

			if(pNetHead->bHandleCode==ASS_GR_CHECKOUT_ERROR)
			{
				if (ASS_GR_CHECK_OUT == pNetHead->bAssistantID)
				{
					//CString strBuffer;
					//CString str;
					//str = fMsg.GetKeyVal("GameRoom","NetWorkBusy","取出失败，可能原因是密码不正确或余额不足！");
					//strBuffer.Format(str);
					//AFCMessageBox(strBuffer,m_GameInfo.szGameName);//,this);
					WriteStr("取出失败，可能原因是密码不正确或余额不足！");
				}
			}

			if(pNetHead->bHandleCode==ASS_GR_CHECKOUT_ERROR_PLAYING) //游戏中不能取钱 add by lxl 2010-12-6
			{
				if (ASS_GR_CHECK_OUT == pNetHead->bAssistantID)
				{
					//CString strBuffer;
					//CString str;
					//str = fMsg.GetKeyVal("GameRoom","NoDrawForPlaying","您正在游戏中，不能取款。");
					//strBuffer.Format(str);
					//AFCMessageBox(strBuffer,m_GameInfo.szGameName);//,MB_OK,this);
					WriteStr("正在游戏中，不能取款！");
				}
			}

			if(pNetHead->bHandleCode==ASS_GR_CHECKOUT_SUC)
			{
				//效验数据
				if (uDataSize!=sizeof(MSG_GR_CheckMoney)) return false;
				MSG_GR_CheckMoney * pCheckOut=(MSG_GR_CheckMoney *)pNetData;
				
				if(pCheckOut->dwUserID==m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)//自己
				{

					if (uDataSize!=sizeof(MSG_GR_CheckMoney)) return false;
					MSG_GR_CheckMoney * pCheckIn=(MSG_GR_CheckMoney *)pNetData;

					if(pCheckIn->dwUserID==m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)//自己
					{
						UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(pCheckIn->dwUserID);
						if(pUserItem==NULL)
							return false;

						//设置数据
						pUserItem->GameUserInfo.i64Money += pCheckIn->i64CheckOut;
						pUserItem->GameUserInfo.i64Bank -= pCheckIn->i64MoneyInBank;//银行金币
						//m_UseBank = FALSE;
					}
				}
				//更新金币信息取钱成功

			}
			
			break;
		}
	case ASS_GR_CHECK_IN://存钱
	case ASS_GR_CHECK_IN_INGAME://游戏中存钱
		{
			
			if(pNetHead->bHandleCode==ASS_GR_CHECKIN_ERROR)
			{
				if (ASS_GR_CHECK_IN == pNetHead->bAssistantID)
				{
					//CString strBuffer;
					//CString str;
					//str = fMsg.GetKeyVal("GameRoom","NetWorkBusySave","网络拥塞,存入失败!请稍后再试!");
					//strBuffer.Format(str);
					//AFCMessageBox(strBuffer,m_GameInfo.szGameName);//,MB_OK,this);
					WriteStr("网络拥塞,存入失败!请稍后再试!！");
					
				}
			}
			if(pNetHead->bHandleCode==ASS_GR_CHECKIN_ERROR_PLAYING)
			{
				if (ASS_GR_CHECK_IN == pNetHead->bAssistantID)
				{
					//CString strBuffer;
					//CString str;
					//str = fMsg.GetKeyVal("GameRoom","NoSaveForPlaying","您正在游戏中，不能存款。");
					//strBuffer.Format(str);
					//AFCMessageBox(strBuffer,m_GameInfo.szGameName);//,MB_OK,this);
					WriteStr("您正在游戏中，不能存款！");
				}
			}

			if(pNetHead->bHandleCode==ASS_GR_CHECKIN_ZHUANGJIA)  //百家乐类游戏 上庄中不能存款 add by lxl 2010-11-5
			{
				if (ASS_GR_CHECK_IN == pNetHead->bAssistantID)
				{
					//CString strBuffer;
					//CString str;
					//str = fMsg.GetKeyVal("GameRoom","NoSaveForZhuangjia","您正在上庄中，不能存款。");
					//strBuffer.Format(str);
					//AFCMessageBox(strBuffer,m_GameInfo.szGameName);//,MB_OK,this);
					WriteStr("您正在上庄中，不能存款。");
				}
			}

			if(pNetHead->bHandleCode==ASS_GR_CHECKIN_SUC)
			{
				//效验数据
				if (uDataSize!=sizeof(MSG_GR_CheckMoney)) return false;
				MSG_GR_CheckMoney * pCheckIn=(MSG_GR_CheckMoney *)pNetData;

				if(pCheckIn->dwUserID==m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)//自己
				{
					UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(pCheckIn->dwUserID);
					if(pUserItem==NULL)
						return false;

					//设置数据
					pUserItem->GameUserInfo.i64Money -= pCheckIn->i64CheckOut;
					pUserItem->GameUserInfo.i64Bank += pCheckIn->i64MoneyInBank;//银行金币
					
				}
				//存钱成功
			}
			//m_UseBank = FALSE;
			break;
		}
		break;
	}
	return true;
}

//机器人根据金币操作银行
void CGameRoomEx::AiControlBank()
{
	//比赛场不使用银行功能
	if (m_bAiBank && !m_UseBank)
	{
		srand(m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID*(int)GetCurrentTime());//取随机种子
		if (m_GameInfo.pMeUserInfo->GameUserInfo.i64Money > m_i64MaxAiMoney && !(m_GameInfo.dwRoomRule & GRR_CONTEST))
		{
			///取mix - max 之间的随机数
			__int64 i64RandMoney = m_i64CheckMoneyMax - m_i64CheckMoneyMix;
			//防止取余差值为0
			if(m_i64CheckMoneyMax==m_i64CheckMoneyMix)
			{
				m_i64CheckMoneyMax=_abs64(m_i64CheckMoneyMax*2);
			}
			UINT nrand = 0;
			if (m_bAiBank && m_bOpenBankBase)
			{
				nrand = rand()*m_i64SaveAndOutMoneyBase;
			}
			else
			{
				nrand = rand()*100000;
			}
			
			i64RandMoney = (nrand%i64RandMoney) + m_i64CheckMoneyMix + m_i64SaveAndOutMoneyBase;
			__int64 Temp = m_GameInfo.pMeUserInfo->GameUserInfo.i64Money - i64RandMoney;
						
			m_UseBank = true;
			StandingDesk();
			SetTimer(ID_CHECK_AIBANK,10000,NULL);
			CheckMoney(Temp);			
		}
		else if(m_GameInfo.pMeUserInfo->GameUserInfo.i64Money < m_i64CheckMoneyMix || m_GameInfo.pMeUserInfo->GameUserInfo.i64Money < m_i64SaveAndOutMoneyBase)
		{
			///取mix - max 之间的随机数
			__int64 i64RandMoney = m_i64CheckMoneyMax - m_i64CheckMoneyMix;
			if(m_i64CheckMoneyMax==m_i64CheckMoneyMix)
			{
				m_i64CheckMoneyMax=_abs64(m_i64CheckMoneyMax*2);
			}

			UINT nrand = 0;
			if (m_bAiBank && m_bOpenBankBase)
			{
				nrand = rand()*m_i64SaveAndOutMoneyBase;
			}
			else
			{
				nrand = rand()*100000;
			}

			i64RandMoney = (nrand%i64RandMoney) + m_i64CheckMoneyMix + m_i64SaveAndOutMoneyBase;
			
			m_UseBank = true;
		//	if (!(m_GameInfo.dwRoomRule & GRR_CONTEST))
			{
				StandingDesk();
			}
			SetTimer(ID_CHECK_AIBANK,10000,NULL);
			CheckMoneyWithPwd(i64RandMoney);			
		}
	}
}
///存钱
void CGameRoomEx::CheckMoney(__int64 iMoney)
{

	//if (m_GameInfo.uNameID == 10900400
	//	|| m_GameInfo.uNameID == 10900200)//牛牛游戏存取钱先离开房间
	//{
	//为防金币异常存钱要离开桌子。
	/*	if (m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO != 255)
		{
			StandingDesk();
		}*/
	//}
	OutputDebugString("dwjloglog::CheckMoney");
	if (m_bAiBank && m_bOpenBankBase)
	{
		OutputDebugString("dwjloglog::CheckMoney1");
		iMoney = iMoney/m_i64SaveAndOutMoneyBase;
		iMoney *= m_i64SaveAndOutMoneyBase;
	}
	CString dwj;
	dwj.Format("dwjloglog::存钱=%d",iMoney);
	OutputDebugString(dwj);

	/// 取得字符串并发出
	MSG_GR_S_CheckMoney checkMoney;
	memset(&checkMoney,0,sizeof(checkMoney));
	checkMoney.i64Count = iMoney;
	checkMoney.UserID = m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID;//m_pLogonInfo.dwUserID;
	
	/*CString str;
	str.Format("zhtlog:存入：%I64d checkMoney.UserID=%d",iMoney,checkMoney.UserID);*/
	m_TCPSocket->SendData(&checkMoney,sizeof(checkMoney),MDM_GR_MONEY,ASS_GR_CHECK_IN,0);
}

void CGameRoomEx::CheckEditMoney(__int64 iMoney, int UserID)
{
	//	if (m_GameInfo.uNameID == 10900400
	//	|| m_GameInfo.uNameID == 10900200)//牛牛游戏存取钱先离开房间
	//{
	/*	if (m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO != 255)
		{
			StandingDesk();
		}*/
	//}
	/// 取得字符串并发出
	MSG_GR_S_CheckMoney checkMoney;
	memset(&checkMoney,0,sizeof(checkMoney));
	checkMoney.i64Count = iMoney;
	checkMoney.UserID = UserID;//m_pLogonInfo.dwUserID;
	
	/*CString str;
	str.Format("zhtlog:存入：%I64d checkMoney.UserID=%d",iMoney,checkMoney.UserID);*/
	m_TCPSocket->SendData(&checkMoney,sizeof(checkMoney),MDM_GR_MONEY,ASS_GR_CHECK_IN,0);
}

///取钱
void CGameRoomEx::CheckMoneyWithPwd(__int64 iMoney)
{

	//if (m_GameInfo.uNameID == 10900400
	//	|| m_GameInfo.uNameID == 10900200)//牛牛游戏存取钱先离开房间
	//{
		if (m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO != 255)
		{
			StandingDesk();
		}
	OutputDebugString("dwjloglog::CheckMoneyWithPwd");
	// 开启了机器人操作银行 且整存整取 那就除以基数再乘以基数
	if (m_bAiBank && m_bOpenBankBase)
	{
		OutputDebugString("dwjloglog::CheckMoneyWithPwd1");
		iMoney = iMoney/m_i64SaveAndOutMoneyBase;
		iMoney *= m_i64SaveAndOutMoneyBase;
	}
	CString dwj;
	dwj.Format("dwjloglog::取钱=%d",iMoney);
	OutputDebugString(dwj);
	//dwj.FORMAT_EX_PARAMETERS(
	//}
	//取钱
	MSG_GR_S_CheckMoneyWithPwd_t cm;
	cm.i64Count = iMoney;
	cm.UserID = m_pLogonInfo.dwUserID;
	//GetCryptedPasswd(cm.szMD5Pass, pCheckMoney->szPWD);
	memcpy(cm.szMD5Pass,m_pLogonInfo.szMD5Pass,sizeof(m_pLogonInfo.szMD5Pass));
	//cm.szMD5Pass = m_pLogonInfo->szMD5Pass;
	m_TCPSocket->SendData(&cm,sizeof(cm),MDM_GR_MONEY,ASS_GR_CHECK_OUT,0);
}

void CGameRoomEx::SetBankInfo(BOOL bAiBand,BOOL bOpenBankBase, __int64 i64MaxAiMoney, __int64 i64CheckMoneyMax, __int64 i64CheckMoneyMix, __int64 i64SaveAndOutMoneyBase)
{
	m_i64MaxAiMoney     = i64MaxAiMoney;                    //AI身上最多的钱数，达到这个数就存钱
	m_i64CheckMoneyMax  = i64CheckMoneyMax;                 //取钱最大数
	m_i64CheckMoneyMix  = i64CheckMoneyMix;                 //取钱最小数
	m_bAiBank           = bAiBand;                          //是否机器人操作银行
	m_bOpenBankBase		= bOpenBankBase;						//是否开启
	m_i64SaveAndOutMoneyBase =  i64SaveAndOutMoneyBase;		//整存整取基数
}

void CGameRoomEx::SetExitTime(TTime_day time)
{
	m_ExitTime = time;
}