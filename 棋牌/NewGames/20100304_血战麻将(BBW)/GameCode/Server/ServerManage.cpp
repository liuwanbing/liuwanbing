#include "StdAfx.h"
#include "ServerManage.h"
#include <shlobj.h>

/*
* 打印日志文件
*/
void DebugPrintf(int iRoomID, const char *p, ...)
{
	return ;
	CTime time = CTime::GetCurrentTime();
	char szFilename[256];
	sprintf(szFilename, "%d_%d-%d%d%d-log.txt", NAME_ID, iRoomID, time.GetYear(), time.GetMonth(), time.GetDay());
	FILE *fp = fopen(szFilename, "a");
	if (NULL == fp)
	{
		return;
	}
	va_list arg;
	va_start(arg, p);
	vfprintf(fp, p, arg);
	fprintf(fp,"\n");
	fclose(fp);
}
/**
* 构造函数
*/
CServerGameDesk::CServerGameDesk(void):CGameDesk(FULL_BEGIN)
{	
	m_uSocketID = 0;
	//
	m_iEnableWatch=0;
	m_iLeaveArgee=0;
	m_iLessCount=15;					//多处用作 出牌定时器 控制时间；
	m_iBeenPlayGame=0;
	m_bGameStation=GS_WAIT_ARGEE;
	m_byAgreeLeaveNum = 0;//同意结束游戏的玩家数量
	//m_SrvLogic.SetDeskPoint(this);//事件处理(获取父类指针)

    sCPGAction.SetDeskPoint(this);//吃碰杠检测方法(获取父类指针)
    sCheckHuPai.SetDeskPoint(this);//糊牌检测方法(获取父类指针)
    sCountFen.SetDeskPoint(this);//游戏算分(获取父类指针)
    sThingHandle.SetDeskPoint(this);//事件处理(获取父类指针)
}

/**
* 析构函数
*/
CServerGameDesk::~CServerGameDesk(void)
{
}

/**
* 定时器消息
* @param [in] uTimerID 定时器ID
* @return 返回布尔类型
*/
bool CServerGameDesk::OnTimer(UINT uTimerID)
{
	//m_SrvLogic.OnGameTimer(uTimerID);
	if (m_bGameStation>=20 && m_bGameStation<23)
	{
		sThingHandle.OnGameTimer(uTimerID);
	}
	return __super::OnTimer(uTimerID);
}

/**
* 获取游戏状态信息
* @param [in] bDeskStation 玩家位置
* @param [in] uSoketID Socket ID
* @param [in] bWatchUser 旁观
* @return 返回布尔类型
*/
bool CServerGameDesk::OnGetGameStation(BYTE bDeskStation, UINT uSocketID, bool bWatchUser)
{
	m_uSocketID = uSocketID;//保存断线重连时的socket
	//m_SrvLogic.GetGameStaton(bDeskStation, bWatchUser,m_bGameStation);
	//房间id
	m_RoomId = m_pDataManage->m_InitData.uRoomID;
	CString s = CINIFile::GetAppPath ();/////本地路径
	CINIFile pz(s + SKIN_FOLDER  + _T("_s.ini"));
	char key[500];
	sprintf(key,"game");
	sGameData.m_bShowTax	= pz.GetKeyVal(key,TEXT("bShowTax"),1);		//是否显示台费（默认有）
	sprintf(key,"%d_%d",NAME_ID,m_RoomId);
	sGameData.m_bShowTax	= pz.GetKeyVal(key,TEXT("bShowTax"),sGameData.m_bShowTax);		//是否显示台费（默认有）

	sGameData.m_bChengBao = pz.GetKeyVal(key,TEXT("bChengBao"),sGameData.m_bChengBao);		//分承包制和家家有

	sThingHandle.GetGameStaton(bDeskStation, uSocketID, bWatchUser, m_bGameStation);	


	return true;
}

/**
* 框架游戏数据包处理函数
* @param [in] bDeskStation 玩家位置
* @param [in] pNetHead 消息头
* @param [in] pData 消息体
* @param [in] uSize 消息包的大小
* @param [in] uSoketID Socket ID
* @param [in] bWatchUser 旁观
* @return 返回布尔类型
*/
bool CServerGameDesk::HandleFrameMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser)
{
	switch(pNetHead->bAssistantID)
	{
	case ASS_GM_FORCE_QUIT:		//强行退出//安全退出
		{
			OutputDebugString("碰牌测试：ASS_GM_FORCE_QUIT 00 ");
			//UserFourceQuit(bDeskStation);//强退处理，在这里实现托管等
			char cKey[10];
			CString sPath=CBcfFile::GetAppPath();
			int iResult = 1;
			if (CBcfFile::IsFileExist(sPath + "SpecialRule.bcf"))
			{
				CBcfFile fsr( sPath + "SpecialRule.bcf");
				sprintf(cKey, "%d", NAME_ID);
				iResult = fsr.GetKeyVal (_T("ForceQuitAsAuto"), cKey, 1);
			}
			OutputDebugString("碰牌测试：ASS_GM_FORCE_QUIT 00 ");
			if (iResult == 0)//强退结束
			{
				GameFinish(bDeskStation,GFF_FORCE_FINISH);//强退结束游戏
				return __super::HandleFrameMessage( bDeskStation,  pNetHead,  pData,  uSize,  uSocketID,  bWatchUser); 
			}
			return true;
		}
	}
	//如果是强退，并且是强退结束版本
	return __super::HandleFrameMessage( bDeskStation,  pNetHead,  pData,  uSize,  uSocketID,  bWatchUser);
}

/**
* 游戏数据包处理函数
* @param [in] bDeskStation 玩家位置
* @param [in] pNetHead 消息头
* @param [in] pData 消息体
* @param [in] uSize 消息包的大小
* @param [in] uSoketID Socket ID
* @param [in] bWatchUser 旁观
* @return 返回布尔类型
*/
bool CServerGameDesk::HandleNotifyMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser)
{
	KillTimer(TIME_CHECK_GAME_MSG);
	SetTimer(TIME_CHECK_GAME_MSG,TIME_DEFAULT_HANDLE);//重新设置超时计时器

	switch (pNetHead->bAssistantID)
	{
	case ASS_GM_AGREE_GAME:		//用户同意游戏
		{
			if (bWatchUser)
			{
				return FALSE;
			}
			if(NULL != m_pUserInfo[bDeskStation])
			{
				m_pUserInfo[bDeskStation]->m_UserData.bUserState = USER_ARGEE;  //玩家置为同意状态
			}
			return __super::HandleNotifyMessage(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);
		}
	}
	if(pNetHead->bMainID == MDM_GM_GAME_NOTIFY)//接收游戏消息
	{
		sThingHandle.HandleNotifyMessage(bDeskStation,pNetHead->bAssistantID,(BYTE*)pData,uSize,bWatchUser);
	}
	if(pNetHead->bMainID == 180)
		return true;
	return __super::HandleNotifyMessage(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);
}

/**
* 重置游戏状态
* @param [in] bLastStation 游戏结束标志
* @return 返回布尔类型
*/
bool CServerGameDesk::ReSetGameState(BYTE bLastStation)
{
	return TRUE;
}

/**
* 初始化游戏
* @return 返回布尔类型
*/
bool CServerGameDesk::InitDeskGameStation()
{
	return true;
}

/**
* 游戏开始
* @param [in] bBeginFlasg 开始模式标志
*/
bool CServerGameDesk::GameBegin(BYTE bBeginFlag)
{
	if (__super::GameBegin(bBeginFlag) == false) 
	{
		GameFinish(0,GF_SALE);
		return false;
	}

	//设置状态
	m_byAgreeLeaveNum = 0;//同意结束游戏的玩家数量
	m_iBeenPlayGame++;
	m_bGameStation = GS_SEND_CARD;
    sGameData.ApplyThingID = THING_BEGIN;
    //请求事件dwj1-1
    sThingHandle.ApplyAction(THING_BEGIN,200);
	return true;
}

/**
* 游戏结束
* @param [in] bDeskStation 玩家位置
* @param [in] bCloseFlag 游戏结束标志
* @return 返回布尔类型
*/
bool CServerGameDesk::GameFinish(BYTE bDeskStation, BYTE bCloseFlag)
{
	switch (bCloseFlag)
	{
	case GF_NORMAL:		//游戏正常结束
		{
			m_bGameStation = GS_WAIT_ARGEE;			
    		//计算胜负分数
			for (int i = 0;i < PLAY_COUNT; i ++)
			{
				if (m_pUserInfo[i] != NULL) 
				{
					m_pUserInfo[i]->m_UserData.bUserState = USER_SITTING;
				}
			}

			__int64   i_ChangePoint[PLAY_COUNT];     //积分
			__int64   i_ChangeMoney[PLAY_COUNT];     //金币
			__int64   i_ReturnMoney[PLAY_COUNT];     //返回金币			

			memset(i_ChangePoint, 0, sizeof(i_ChangePoint));
			memset(i_ChangeMoney, 0, sizeof(i_ChangeMoney));
			memset(i_ReturnMoney, 0, sizeof(i_ReturnMoney));

			// 服务器写入数据库
			bool temp_cut[PLAY_COUNT];
			memset(temp_cut, 0, sizeof(temp_cut));
			/////2010-9-27 修改台费扣费问题，没下注的人当庄时所有人都不下注不扣台费
			//CGameUserInfo	* pUserInfoTemp[MAX_PEOPLE];
			//memcpy(pUserInfoTemp,m_pUserInfo, sizeof(m_pUserInfo));
			//乘以基础分
			for(int i=0;i<4;i++)
			{
				sGameData.T_CountFen.m_iZhuanYi[i]	*= sGameData.m_iBaseFen;//呼叫转移分
				sGameData.T_CountFen.m_iHuaJiao[i]	*= sGameData.m_iBaseFen;//花叫分
				sGameData.T_CountFen.iGangFen[i]	*= sGameData.m_iBaseFen;//杠分
				sGameData.T_CountFen.iHuFen[i]		*= sGameData.m_iBaseFen;//糊分
				sGameData.T_CountFen.iZongFen[i]	*= sGameData.m_iBaseFen;//总分
			}
			for(int i=0;i<4;++i)
			{
				i_ChangePoint[i] = sGameData.T_CountFen.iZongFen[i];
			}
			// 扣所有赢家税
			ChangeUserPointint64(i_ChangePoint, temp_cut);
			__super::RecoderGameInfo(i_ChangeMoney);

			for(int i=0;i<4;++i)
			{
				sGameData.T_CountFen.iMoney[i] = i_ChangeMoney[i];//524288+640
			}

			sGameData.T_CountFen.bIsShowTax = sGameData.m_bShowTax;//是否显示服务费
			sGameData.T_CountFen.iBasePoint = m_pDataManage->m_InitData.uBasePoint;//倍数
			//m_SrvLogic.OnKillAllTimer();//删除所有计时器，游戏结束了
			KillTimer(TIME_WAIT_MSG);
			KillTimer(10);//
				
			//拷贝手牌数据
			sUserData.CopyHandPai(sGameData.T_CountFen.m_byArHandPai,0,true);
			sGameData.T_CountFen.byCloseFlag = bCloseFlag;
			//发送数据
			for (int i=0; i<PLAY_COUNT; i++)     
			{

				SendGameData(i, &sGameData.T_CountFen, sizeof(sGameData.T_CountFen), MDM_GM_GAME_NOTIFY, THING_ENG_HANDLE, 0);   
				SendWatchDataEx(i, &sGameData.T_CountFen, sizeof(sGameData.T_CountFen), MDM_GM_GAME_NOTIFY, THING_ENG_HANDLE, 0); 
			}

			bCloseFlag = GF_NORMAL;
			ReSetGameState(bCloseFlag);
			OutputDebugString("事件：游戏正常结束");
			__super::GameFinish(bDeskStation,bCloseFlag);
			return true;
		}
	case GFF_SAFE_FINISH:
		{
			if(bDeskStation==255)
				bDeskStation=0;
			KillTimer(TIME_WAIT_MSG);
			KillTimer(10);//

			KillTimer(TIME_CHECK_GAME_MSG);

			int iBasePoint = m_pDataManage->m_InitData.uBasePoint;//倍数
			for (int i=0; i<m_bMaxPeople; i++)
			{
				if (m_pUserInfo[i] != NULL) 
					m_pUserInfo[i]->m_UserData.bUserState = USER_SITTING;
			}				
			bCloseFlag	= GFF_FORCE_FINISH;

			//写入数据库				
			__int64 i_ChangePoint[PLAY_COUNT];
			__int64 iWinMoney[8] ={0};
			bool temp_cut[PLAY_COUNT];
			memset(i_ChangePoint,0,sizeof(i_ChangePoint));
			memset(temp_cut,0,sizeof(temp_cut));
			//乘以基础分
			//for (int i=0;i<PLAY_COUNT;i++)
			//{
			//	temp_point[i] = sGameData.T_CountFen.iZongFen[i];
			//}
			ChangeUserPointint64(i_ChangePoint, temp_cut);
			__super::RecoderGameInfo(iWinMoney);
			for (int i=0;i<PLAY_COUNT;i++)
			{
				sGameData.T_CountFen.iMoney[i] = iWinMoney[i]; 
			}
			sGameData.T_CountFen.bIsShowTax = sGameData.m_bShowTax;//是否显示服务费
			
			///拷贝手牌数据
			sUserData.CopyHandPai(sGameData.T_CountFen.m_byArHandPai,0,true);
			sGameData.T_CountFen.byCloseFlag = bCloseFlag;
			///发送数据
			for (int i=0; i<PLAY_COUNT; i++)     
			{
				SendGameData(i, &sGameData.T_CountFen, sizeof(sGameData.T_CountFen), MDM_GM_GAME_NOTIFY, THING_ENG_UN_NORMAL, 0);   
				SendWatchDataEx(i, &sGameData.T_CountFen, sizeof(sGameData.T_CountFen), MDM_GM_GAME_NOTIFY, THING_ENG_UN_NORMAL, 0);
			}
		}break;
	case GFF_FORCE_FINISH:
		{//uRunPublish
			// 为方便地让用户离开而新添加的代码
			// 判断该玩家是否压了注，是否庄家
			if(bDeskStation==255 || m_bGameStation == GS_WAIT_ARGEE || m_bGameStation == GS_WAIT_NEXT)
			{
				return true;
			}
			m_bGameStation = GS_WAIT_ARGEE;//GS_WAIT_SETGAME;
			for (int i = 0; i < PLAY_COUNT; i ++)
			{
				if (m_pUserInfo[i] != NULL) 
					m_pUserInfo[i]->m_UserData.bUserState = USER_SITTING;
			}

			KillTimer(TIME_WAIT_MSG);
			KillTimer(10);//
			KillTimer(TIME_CHECK_GAME_MSG);
			bCloseFlag	= GFF_FORCE_FINISH;
			//int iBasePoint = m_pDataManage->m_InitData.uBasePoint;//倍数

			//写入数据库				
			__int64 i_ChangePoint[PLAY_COUNT]={0};
			__int64 iWinMoney[8] ={0};
			bool temp_cut[PLAY_COUNT] = {0};
			//乘以基础分
			i_ChangePoint[bDeskStation] = -(__int64)m_pDataManage->m_InitData.uRunPublish;
			temp_cut[bDeskStation] = true;


			char buff[500];
			sprintf(buff,"事件：碰牌测试 服务器 GFF_FORCE_FINISH 预计扣分%I64d ",i_ChangePoint[bDeskStation] );
			OutputDebugString(buff);

			for (int i=0;i<PLAY_COUNT;i++)
			{
				sprintf(buff,"事件：碰牌测试 服务器 GFF_FORCE_FINISH 玩家%d 初始金币%I64d ",i,iWinMoney[i] );
				OutputDebugString(buff);
			}

			ChangeUserPointint64(i_ChangePoint, temp_cut);
			__super::RecoderGameInfo(iWinMoney);
			for (int i=0;i<PLAY_COUNT;i++)
			{
				sGameData.T_CountFen.iMoney[i] = iWinMoney[i]; 

				sprintf(buff,"事件：碰牌测试 服务器 GFF_FORCE_FINISH 玩家%d 得金币%I64d ",i,iWinMoney[i] );
				OutputDebugString(buff);

			}
			sGameData.T_CountFen.bIsShowTax = sGameData.m_bShowTax;//是否显示服务费

			sprintf(buff,"事件：碰牌测试 服务器 GFF_FORCE_FINISH 实际扣分%I64d ",i_ChangePoint[bDeskStation] );
			OutputDebugString(buff);

			///拷贝手牌数据
			sUserData.CopyHandPai(sGameData.T_CountFen.m_byArHandPai,0,true);
			sGameData.T_CountFen.byCloseFlag = bCloseFlag;

			///发送数据
			for (int i=0; i<PLAY_COUNT; i++)     
			{
				SendGameData(i, &sGameData.T_CountFen, sizeof(sGameData.T_CountFen), MDM_GM_GAME_NOTIFY, THING_ENG_UN_NORMAL, 0);   
				SendWatchDataEx(i, &sGameData.T_CountFen, sizeof(sGameData.T_CountFen), MDM_GM_GAME_NOTIFY, THING_ENG_UN_NORMAL, 0);
			}
			ReSetGameState(bCloseFlag);
			OutputDebugString("事件：游戏强制结束");
			__super::GameFinish(bDeskStation,bCloseFlag);
			return true;
		}
	default:
		{
			break;
		}
	}
	// 重置数据
	ReSetGameState(bCloseFlag);
	return __super::GameFinish(bDeskStation,bCloseFlag);
}

/**
* 判断是否正在游戏
* @param [in] bDeskStation 玩家位置
* @return 返回当前游戏状态
*/
bool CServerGameDesk::IsPlayGame(BYTE bDeskStation)
{
	return 	m_bGameStation >= 20 && m_bGameStation < 23;
}

/**
* 用户离开游戏桌
* @return 返回用户离开状态
*/
BYTE CServerGameDesk::UserLeftDesk(BYTE bDeskStation, CGameUserInfo * pUserInfo)
{
	OutputDebugString("服务器 事件：UserLeftDesk");
	sThingHandle.UserLeftDesk(bDeskStation);
	if((m_bGameStation >= 20 || m_bGameStation < 23) )//不在游戏中桌主离开
	{
		//return false;
	}
	return __super::UserLeftDesk(bDeskStation,pUserInfo);
}

//用户断线离开
bool CServerGameDesk::UserNetCut(BYTE bDeskStation, CGameUserInfo * pLostUserInfo)
{
	OutputDebugString("事件：断线测试 UserNetCut 000");
	if (m_bGameStation>=20 && m_bGameStation<23 )  //在游戏中，有断线托管就进行托管
	{
		sThingHandle.UserNetCut(bDeskStation);

		OutputDebugString("事件：断线测试 UserNetCut 111");
		sUserData.m_byFoceLeavCount = 0;
		for(int i=0;i<4;++i)
		{
			if(sUserData.m_bFoceLeave[i])//强退
			{
				sUserData.m_byFoceLeavCount++;
			}
		}
        if (sUserData.m_byFoceLeavCount >= PLAY_COUNT && sUserData.m_byFoceLeavCount != 255) //四家都强行离开了
        {
            GameFinish(0,GF_NORMAL); //用户强行离开
			OutputDebugString(TEXT("游戏结束：断线测试 UserNetCut --- "));
            return true;
        }
	}
	//记录断线玩家（在重连前部给该玩家发送消息）
	return __super::UserNetCut(bDeskStation, pLostUserInfo);
}



/**
* 玩家坐下
* @return 返回用户坐下状态
*/
BYTE CServerGameDesk::UserSitDesk(MSG_GR_S_UserSit * pUserSit, CGameUserInfo * pUserInfo)
{
	BYTE result = __super::UserSitDesk(pUserSit, pUserInfo);;
	return result;
}

///设置游戏定时器
void CServerGameDesk::SetGameTimer(UINT iId,int iTime)
{
	SetTimer(iId,iTime);
}
//删除指定计时器
void CServerGameDesk::KillGameTimer(UINT iId)
{
	KillTimer(iId);
}


///发送游戏状态
void CServerGameDesk::SendGameStationEx(BYTE nDeskStation, bool bWatchUser, void * pStationData, UINT uSize)
{
	// 发送数据
	SendGameStation(nDeskStation,m_uSocketID,bWatchUser,pStationData,uSize);
}
///发送数据函数 （发送消息给游戏者）
void CServerGameDesk::SendGameDataEx(BYTE nDeskStation, BYTE bMainID, BYTE bAssID, BYTE bHandleCode)
{
	SendGameData(nDeskStation,MDM_GM_GAME_NOTIFY,bAssID,0) ; 
}
///发送旁观数据 （发送消息给旁观者）
void CServerGameDesk::SendWatchDataEx(BYTE nDeskStation, BYTE bMainID, BYTE bAssID, BYTE bHandleCode)
{
	SendWatchData(nDeskStation,MDM_GM_GAME_NOTIFY,bAssID,0) ; 
}
///发送数据函数 （发送消息给游戏者）
void CServerGameDesk::SendGameDataEx(BYTE nDeskStation, void * pData, UINT uSize, BYTE bMainID, BYTE bAssID, BYTE bHandleCode)
{
	SendGameData(nDeskStation, pData, uSize, MDM_GM_GAME_NOTIFY, bAssID, 0);
}
///发送旁观数据 （发送消息给旁观者）
void CServerGameDesk::SendWatchDataEx(BYTE nDeskStation, void * pData, UINT uSize, BYTE bMainID, BYTE bAssID, BYTE bHandleCode)
{
	SendWatchData(nDeskStation, pData, uSize, MDM_GM_GAME_NOTIFY, bAssID, 0);
}




//获取玩家同意状态
void CServerGameDesk::GetUserAgree(BYTE &agree)
{
    for (int i=0; i<PLAY_COUNT; ++i)
    {
		if(m_pUserInfo[i] == NULL)
			continue;
        if (m_pUserInfo[i]->m_UserData.bUserState == USER_ARGEE)  // 同意
        {
            agree |= (1<<i);
        }
        else                            // 未同意
        {
            agree &= (~(1<<i));
        }
    }
}
///获取房间规则
UINT CServerGameDesk::GetRoomRule()
{
	return m_pDataManage->m_InitData.dwRoomRule;//房间规则
}
///获取房间id
UINT CServerGameDesk::GetRoomID()
{
	return m_pDataManage->m_InitData.uRoomID;//房间id
}

//获取房间倍率
int CServerGameDesk::GetRoomPoint()
{
	return GetTableBasePoint();
}
//获取玩家的昵称

void CServerGameDesk::GetUserNickName(BYTE station,char name[])

{

    if (station<0 || station>=PLAY_COUNT|| m_pUserInfo[station]==NULL)  // 

		return;

	memcpy(name,m_pUserInfo[station]->m_UserData.nickName,sizeof(m_pUserInfo[station]->m_UserData.nickName));

}

