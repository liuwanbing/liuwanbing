#include "XGmMessage.h"
#include <stdio.h>
#include <tchar.h>
#include <atltime.h>

#include "log.h"

/// 创建GM消息处理对象
CGMMessageBase *CreateGMModule(IGMFunction *pFunction)
{
	XGmMessageModule *p = new XGmMessageModule(pFunction);
	if (NULL != p)
	{
		return p;
	}
	return NULL;
}
/// 销毁GM消息处理对象
void DestroyGMModule(CGMMessageBase *pBase)
{
	if (NULL != pBase)
	{
		delete pBase;
		pBase = NULL;
	}
}

/// 打印调试信息
static void DebugPrintf(const char *p, ...)
{
	char szFilename[256];
	sprintf( szFilename, "E:\\bzw\\DebugInfo\\GM_Module.txt");
	FILE *fp = fopen( szFilename, "a" );
	if (NULL == fp)
	{
		return;
	}
	va_list arg;
	va_start( arg, p );
	fprintf(fp,"[%s]--", CTime::GetCurrentTime().Format("%H:%M:%S"));
	vfprintf(fp, p, arg );
	fprintf(fp,"\n");
	fclose(fp);
}

/// 构造函数
XGmMessageModule::XGmMessageModule(IGMFunction *pFunction) : m_pFunction(pFunction)
{
	//创建日志
	XAFCLog::CreateLog("Z_Socket_GM",1,LOG_DEBUG);
}
/// 析构函数
XGmMessageModule::~XGmMessageModule(void)
{
}
/// 数值变 IP 字符
TCHAR * XGmMessageModule::ULongToIP(ULONG dwIP, TCHAR * szIP)
{
	wsprintf(szIP,TEXT("%d.%d.%d.%d"),FOURTH_IPADDRESS(dwIP),THIRD_IPADDRESS(dwIP),SECOND_IPADDRESS(dwIP),FIRST_IPADDRESS(dwIP));
	return szIP;
}
/// 分钟转化为友好字符串
TCHAR * XGmMessageModule::GetTimeString(TCHAR *szTime, int iMinutes)
{
	int iDays = iMinutes / 60 / 24;
	int iHours = iMinutes / 60 - iDays * 24;
	int iMin = iMinutes % 60;
	
	TCHAR szDay[20], szHour[20],szMin[20];

	//modify by lxl 2010-12-6 
	if(iDays != 0)
	{
		wsprintf(szDay, TEXT("%d天"),iDays);
		_tcscat(szTime,szDay);
	}
	
	if(iHours != 0)
	{
		wsprintf(szHour, TEXT("%d小时"),iHours);
		_tcscat(szTime,szHour);
	}
	if(iMin != 0)
	{
		wsprintf(szMin, TEXT("%d分钟"),iMin);
		_tcscat(szTime,szMin);
	}
	return szTime;
}

/// 道具ID与名称列表
bool XGmMessageModule::OnGetPropInfo(NetMessageHead * pNetHead, void * pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID)
{
	/// 无参数
	DataBaseLineHead dbHead;
	m_pFunction->PushDataLine(&dbHead, sizeof(dbHead), DTK_GET_PROP_INFO_GM, uIndex, dwHandleID);
	return true;
}
/// 奖励方案
bool XGmMessageModule::OnGetAwardType(NetMessageHead * pNetHead, void * pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID)
{
	/// 无参数
	DataBaseLineHead dbHead;
	m_pFunction->PushDataLine(&dbHead, sizeof(dbHead), DTK_GET_AWARD_TYPE_GM, uIndex, dwHandleID);
	return true;
}
/// GM取得指定玩家的被处罚记录
bool XGmMessageModule::OnGetPunishedUser(NetMessageHead * pNetHead, void * pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID)
{
	//效验数据
	if (uSize!=sizeof(GM_GetOperationByPlayer_t)) return false;///验证结构是否一致
	if (NULL == pData)
	{
		return false;
	}

	//处理
	GM_GetOperationByPlayer_t * pPlayInfo=(GM_GetOperationByPlayer_t *)pData;
	pPlayInfo->playerInfo.szName[sizeof(pPlayInfo->playerInfo.szName)/sizeof(pPlayInfo->playerInfo.szName[0])-1]=0;

	//加入队列
	GM_DT_I_GetOperatedUser_t dl_GetHistory;
	dl_GetHistory.requestPlayer = *pPlayInfo;

	m_pFunction->PushDataLine(&dl_GetHistory.DataBaseHead,sizeof(dl_GetHistory), DTK_GET_OPEATED_USER_GM,uIndex,dwHandleID);//提交数据库
	return true;
}
/// GM处理玩家，可能是奖励可能是惩罚
bool XGmMessageModule::OnOperateUserByID(NetMessageHead * pNetHead, void * pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID)
{
	//效验数据
	if (uSize!=sizeof(GM_OperateByID_t)) return false;///验证结构是否一致
	if (NULL == pData)
	{
		return false;
	}

	//处理
	GM_OperateByID_t * pOperation=(GM_OperateByID_t *)pData;


	if (!m_pFunction->IsUserAlreadyLogon(pOperation->iUserID))
	{
		m_pFunction->SendDataByUser(pOperation->iUserID, pData, uSize, pNetHead->bMainID, pNetHead->bAssistantID, pNetHead->bHandleCode, 0);
		return true;
	}

	//加入队列
	GM_DT_I_OperateUser_t dl_OperateUser;
	dl_OperateUser.operation = *pOperation;

	m_pFunction->PushDataLine(&dl_OperateUser.DataBaseHead,sizeof(dl_OperateUser), DTK_PUT_OPEATION_USER_GM,uIndex,dwHandleID);//提交数据库
	int iTargetIndex = m_pFunction->GetIndexByID(pOperation->iUserID);

	TCHAR szTime[128];
	ZeroMemory(szTime,128);
	GetTimeString(szTime, pOperation->iWorkMinutes);
	GM_Broadcast_t msg;

	//modify by lxl 2010-12-6 提示信息显示玩家昵称
	char lpBuffer[256];
	GetCurrentDirectory(255,lpBuffer);
	CString ss=lpBuffer;
	ss += "\\Function.ini";
	
	BOOL bShowGmMsg = GetPrivateProfileInt("ShowGmMsg","ShowGmMsg",1, ss);

	//CBcfFile f(CBcfFile::GetAppPath() + "Function.bcf");
	//BOOL bShowGmMsg = f.GetKeyVal("ShowGmMsg","ShowGmMsg",1);
	switch(pOperation->iOperation)
	{
	case GM_ACT_AWORD://		1	///< 奖励，奖励金额保存到银行中，不实时写入用户钱包里，本条消息发送到Z服务器
		{
			wsprintf(msg.szMsg, TEXT("恭喜您因[%s]获得游戏管理员[%d]奖励：金币已存入银行，道具请重新登录后查收。"), pOperation->szReason, pOperation->iGMID);
			m_pFunction->SendData(iTargetIndex, &msg, sizeof(GM_Broadcast_t), MDM_GP_PLAYER_2_GM, ASS_GR_MSG_BE_AWARD, 0, 0);
			wsprintf(msg.szMsg, TEXT("恭喜玩家[%s]因[%s]被游戏管理员[%d]奖励金币道具若干。"), pOperation->szNickName, pOperation->szReason, pOperation->iGMID);
			break;
		}
	case GM_ACT_NO_TALK://		2	///< 禁言，被操作的玩若当前在某房间中，GM管理工具客户端须同时向该玩家所在的W服务器发本条消息
		{
			wsprintf(msg.szMsg, TEXT("玩家[%s]因[%s]被游戏管理员[%d]禁言%s。"), pOperation->szNickName, pOperation->szReason, pOperation->iGMID, szTime);
			if (pOperation->iWorkMinutes == 0)
			{
				wsprintf(msg.szMsg, TEXT("玩家[%s]因[%s]被游戏管理员[%d]解除禁言。"), pOperation->szNickName, pOperation->szReason, pOperation->iGMID);
			}
			break;
		}
	case GM_ACT_KICK_OUT://		3	///< 踢出房间，被操作的玩若当前在某房间中，GM管理工具客户端须同时向该玩家所在的W服务器发本条消息
		{
			wsprintf(msg.szMsg, TEXT("玩家[%s]因[%s]被游戏管理员[%d]踢出房间%s。"), pOperation->szNickName, pOperation->szReason, pOperation->iGMID, szTime);
			if (pOperation->iWorkMinutes == 0)
			{
				wsprintf(msg.szMsg, TEXT("玩家[%s]因[%s]被游戏管理员[%d]恢复进入房间权限。"), pOperation->szNickName, pOperation->szReason, pOperation->iGMID);
			}
			else
			{
				m_pFunction->SendData(iTargetIndex, &msg, sizeof(GM_Broadcast_t), MDM_GP_PLAYER_2_GM, ASS_GR_MSG_BE_KICKED, 0, 0);
			}
			if (!bShowGmMsg)
				return true;
			break;
		}
	case GM_ACT_BANDON_ID://	4	///< 禁止登录，即封号，被操作的玩若当前在某房间中，GM管理工具客户端同时向该玩家所在的W服务器发本条消息
		{
			wsprintf(msg.szMsg, TEXT("玩家[%s]因[%s]被游戏管理员[%d]封号%s。"), pOperation->szNickName, pOperation->szReason, pOperation->iGMID, szTime);
			if (pOperation->iWorkMinutes == 0)
			{
				wsprintf(msg.szMsg, TEXT("玩家[%s]因[%s]被游戏管理员[%d]取消封号。"), pOperation->szNickName, pOperation->szReason, pOperation->iGMID);
			}
			else
			{
				m_pFunction->SendData(iTargetIndex, &msg, sizeof(GM_Broadcast_t), MDM_GP_PLAYER_2_GM, ASS_GR_MSG_BE_BANDID, 0, 0);
			}
			if (!bShowGmMsg)
				return true;
			break;
		}
	}
	// 玩家XXX因XXXX被游戏管理员踢出房间%d分钟。
	// 玩家XXX因XXXX被游戏管理员禁言%d分钟。
	// 玩家XXX因XXXX被游戏管理员奖励金币道具若干。
	// 玩家XXX因XXXX被游戏管理员封号%d分钟。

	
	m_pFunction->SendBatchData(&msg, sizeof(GM_Broadcast_t), MDM_GP_PLAYER_2_GM, ASS_GP_MSG_FROM_GM, 0);

	return true;
}
/// GM广播或向某特定玩家发消息
bool XGmMessageModule::OnGMSendMessage(NetMessageHead * pNetHead, void * pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID)
{
	//效验数据
	if (uSize!=sizeof(GM_Broadcast_t)) return false;///验证结构是否一致

	GM_Broadcast_t *pMessage = (GM_Broadcast_t *)pData;


	XAFCLog::LOG("Z_Socket_GM","");
	XAFCLog::LOG("Z_Socket_GM","+Get a ASS_GP_MSG_FROM_GM message user id = %d",pMessage->iUserID);
	XAFCLog::LOG("Z_Socket_GM","+---- to userid = %d,msg = %s",pMessage->iUserID,pMessage->szMsg);
	
	/// 广播消息
	if (pMessage->iUserID == 0)
	{
		int ret = m_pFunction->SendBatchData(pMessage, sizeof(GM_Broadcast_t), MDM_GP_PLAYER_2_GM, ASS_GP_MSG_FROM_GM, 0);
		XAFCLog::LOG("Z_Socket_GM","+--------[广播消息] SendBatchData ret = %d",ret);
	}
	else
	{
		pMessage->szMsg[sizeof(pMessage->szMsg)/sizeof(TCHAR)-1] = TEXT('\0');
		int iDstIndex = m_pFunction->GetIndexByID(pMessage->iUserID);
		//DebugPrintf("玩家[%d]Socket序号为[%d]", pMessage->iUserID, iDstIndex);
		if (iDstIndex < 0)
		{
			GM_Broadcast_t msgTmp;
			msgTmp.iUserID = pMessage->iUserID;
			msgTmp.iGMID = pMessage->iGMID;
			TCHAR szTmp[32];
			memcpy(szTmp, pMessage->szMsg, sizeof(TCHAR)*32);
			szTmp[31] = TEXT('\0');
			if (_tcslen(pMessage->szMsg)>31)
			{
				szTmp[28] = TEXT('.');
				szTmp[29] = TEXT('.');
				szTmp[30] = TEXT('.');
			}
			wsprintf(msgTmp.szMsg, TEXT("玩家[%d]已经离线，消息[%s]发送不成功！"), pMessage->iUserID, szTmp);
			int ret = m_pFunction->SendData(uIndex, &msgTmp, sizeof(GM_Broadcast_t), MDM_GP_PLAYER_2_GM, ASS_GP_MSG_FROM_GM, 1, dwHandleID);
			XAFCLog::LOG("Z_Socket_GM","+--------[已经离线] SendData ret = %d",pMessage->iUserID,ret);
			return true;
		}
		int ret = m_pFunction->SendData(iDstIndex, pMessage, sizeof(GM_Broadcast_t), MDM_GP_PLAYER_2_GM, ASS_GP_MSG_FROM_GM, 0, 0);
		XAFCLog::LOG("Z_Socket_GM","+--------[发送消息] SendData ret = %d",pMessage->iUserID,ret);
	}
	return true;
}

/// GM登录
bool XGmMessageModule::OnGMLogonOn(NetMessageHead * pNetHead, void * pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID)
{
	//效验数据
	if (uSize!=sizeof(GM_MSG_S_LogonByName_t)) return false;///验证结构是否一致

	//处理登陆
	GM_MSG_S_LogonByName_t * pLogonInfo=(GM_MSG_S_LogonByName_t *)pData;
	pLogonInfo->szName[sizeof(pLogonInfo->szName)/sizeof(pLogonInfo->szName[0])-1]=0;
	pLogonInfo->szMD5Pass[sizeof(pLogonInfo->szMD5Pass)/sizeof(pLogonInfo->szMD5Pass[0])-1]=0;
	//加入队列
	GM_DT_I_LogonByName_t DL_Logon;
	DL_Logon.logonInfo=*pLogonInfo;//////赋值，把登陆用户密码都传过去

	ULongToIP(uAccessIP,DL_Logon.szAccessIP);//把IP弄过来

	m_pFunction->PushDataLine(&DL_Logon.DataBaseHead,sizeof(DL_Logon),DTK_LOGON_BY_NAME_GM,uIndex,dwHandleID);//提交数据库
	return true;
}

/// 消息处理函数
bool XGmMessageModule::OnSocketMessage(NetMessageHead * pNetHead, void * pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID)
{
	if (pNetHead->bMainID!=MDM_GAMEMASTER_MESSAGE)
	{
		return false;
	}
	switch (pNetHead->bAssistantID)
	{
	case ASS_GM_MESSAGE_AWARD_TYPE:			/// 1、下发奖励方案
		OnGetAwardType(pNetHead,pData,uSize,uAccessIP,uIndex,dwHandleID);
		break;
	case ASS_GM_MESSAGE_OPERATE_HISTORY:	/// 2、下发某玩家的处罚历史记录
		OnGetPunishedUser(pNetHead,pData,uSize,uAccessIP,uIndex,dwHandleID);
		break;
	case ASS_GM_MESSAGE_OPERATE_ACT:		/// 3、上传对玩家的操作
		OnOperateUserByID(pNetHead,pData,uSize,uAccessIP,uIndex,dwHandleID);
		break;
	case ASS_GM_MESSAGE_BROADCAST:			/// 4、与某特定玩家发言，及广播消息，公告
		OnGMSendMessage(pNetHead,pData,uSize,uAccessIP,uIndex,dwHandleID);
		break;
	case ASS_GM_MESSAGE_PROP_INFO:		/// 6、用于Z服务器，取得道具ID与名称对应关系
		OnGetPropInfo(pNetHead,pData,uSize,uAccessIP,uIndex,dwHandleID);
		break;
	case ASS_GM_MESSAGE_GM_LOGON:			/// 7、GM登录协议，与普通玩家登录采用不同协议
		OnGMLogonOn(pNetHead,pData,uSize,uAccessIP,uIndex,dwHandleID);
		break;
	default:
		break;
	}
	
	return true;
}
///数据库结果处理
bool XGmMessageModule::OnDataBaseResultEvent(DataBaseResultLine * pResultData)
{
	if ((pResultData->uHandleKind<GM_DT_MESSAGE_START) || (pResultData->uHandleKind>GM_DT_MESSAGE_END))
	{
		return false;
	}
	switch (pResultData->uHandleKind)
	{
	case DTK_GET_AWARD_TYPE_GM:		///< 1、获取奖励方案
		OnGMGetAwardTypeListResult(pResultData);
		break;
	case DTK_GET_OPEATED_USER_GM:	///< 2、获取指定玩家的历史被操作记录，当前状态
		OnGMGetPunishedUserResult(pResultData);
		break;
	case DTK_PUT_OPEATION_USER_GM:	///< 3、写入对玩家的操作到记录表中
		OnGMPutOperationUserResult(pResultData);
		break;
	case DTK_GET_PROP_INFO_GM:		///< 4、获取道具ID与名称对应关系
		OnGMGetPropInfoResult(pResultData);
	case DTK_LOGON_BY_NAME_GM:		///< 5、GM帐号登录信息
		OnGMLogonOnResult(pResultData);
		break;
	default:
		break;
	}
	
	return true;
}
/// GM奖惩玩家结果
bool XGmMessageModule::OnGMPutOperationUserResult(DataBaseResultLine *pResultData)
{
	/// 效验数据
	if (pResultData->LineHead.uSize != sizeof(DataBaseResultLine)) return false;
	BYTE bHandleCode= (pResultData->uHandleRusult == DTR_GM_OPERATE_SUCCESS)? ERR_GM_OPERATOR_SUCCESS : ERR_GM_OPERATOR_FAILED;
	m_pFunction->SendData(pResultData->uIndex,MDM_GAMEMASTER_MESSAGE, ASS_GM_MESSAGE_OPERATE_ACT, bHandleCode,pResultData->dwHandleID);
	return true;
}
/// GM获取道具ID与名称对应关系结果
bool XGmMessageModule::OnGMGetPropInfoResult(DataBaseResultLine *pResultData)
{
	/// 效验数据
	if (pResultData->LineHead.uSize != sizeof(GM_DT_O_GetPropInfoResult_t)) return false;
	/// 处理数据
	GM_DT_O_GetPropInfoResult_t *pPropInfoResult = (GM_DT_O_GetPropInfoResult_t*)pResultData;

	/// 两种返回结果
	/// 1、本条数据是有效数据，但返回未结束
	if (pResultData->uHandleRusult==DTR_GM_GET_RECORD_OK)
	{
		BYTE bHandleCode = HC_GM_GET_LIST_OK;
		m_pFunction->SendData(pPropInfoResult->ResultHead.uIndex, 
			&pPropInfoResult->propInfo,
			sizeof(pPropInfoResult->propInfo),
			MDM_GAMEMASTER_MESSAGE, ASS_GM_MESSAGE_PROP_INFO,
			bHandleCode, pResultData->dwHandleID);

	}
	/// 2、返回数据结束，本条数据为无效数据，只用作指示
	else if (pResultData->uHandleRusult==DTR_GM_GET_RECORD_END)
	{
		BYTE bHandleCode = HC_GM_GET_LIST_END;
		m_pFunction->SendData(pPropInfoResult->ResultHead.uIndex, 
			&pPropInfoResult->propInfo,
			sizeof(pPropInfoResult->propInfo),
			MDM_GAMEMASTER_MESSAGE, ASS_GM_MESSAGE_PROP_INFO,
			bHandleCode, pResultData->dwHandleID);

	}	return true;
}
/// GM获取指定玩家的历史被操作记录
bool XGmMessageModule::OnGMGetPunishedUserResult(DataBaseResultLine *pResultData)
{
	/// 效验数据
	if (pResultData->LineHead.uSize != sizeof(GM_DT_O_OperatedUser_t)) return false;
	/// 处理数据
	GM_DT_O_OperatedUser_t *pOperatedPlayerResult = (GM_DT_O_OperatedUser_t*)pResultData;

	/// 本条数据是有效数据
	BYTE bHandleCode = pResultData->uHandleRusult == DTR_GM_GET_RECORD_OK ? HC_GM_GET_LIST_OK : HC_GM_GET_LIST_END;
	m_pFunction->SendData(pOperatedPlayerResult->ResultHead.uIndex, 
		&pOperatedPlayerResult->operatedPlayer,
		sizeof(pOperatedPlayerResult->operatedPlayer),
		MDM_GAMEMASTER_MESSAGE, ASS_GM_MESSAGE_OPERATE_HISTORY,
		bHandleCode, pResultData->dwHandleID);
	return true;
}

/// 取奖励方案列表结果
bool XGmMessageModule::OnGMGetAwardTypeListResult(DataBaseResultLine *pResultData)
{
	/// 效验数据
	if (pResultData->LineHead.uSize != sizeof(GM_DT_O_AwardType_t)) return false;
	/// 处理数据
	GM_DT_O_AwardType_t *pAwardTypeResult = (GM_DT_O_AwardType_t*)pResultData;
	/// 两种返回结果
	/// 1、本条数据是有效数据，但返回未结束
	if (pResultData->uHandleRusult==DTR_GM_GET_RECORD_OK)
	{
		BYTE bHandleCode = HC_GM_GET_LIST_OK;
		m_pFunction->SendData(pAwardTypeResult->ResultHead.uIndex, 
			&pAwardTypeResult->awardType,
			sizeof(pAwardTypeResult->awardType),
			MDM_GAMEMASTER_MESSAGE, ASS_GM_MESSAGE_AWARD_TYPE,
			bHandleCode, pResultData->dwHandleID);
	}
	/// 2、返回数据结束，本条数据为无效数据，只用作指示
	else if (pResultData->uHandleRusult==DTR_GM_GET_RECORD_END)
	{
		BYTE bHandleCode = HC_GM_GET_LIST_END;
		m_pFunction->SendData(pAwardTypeResult->ResultHead.uIndex,
			MDM_GAMEMASTER_MESSAGE, ASS_GM_MESSAGE_AWARD_TYPE,
			bHandleCode, pResultData->dwHandleID);
	}
	return true;
}
/// GM登录返回结果
bool XGmMessageModule::OnGMLogonOnResult(DataBaseResultLine *pResultData)
{
	//效验数据
	if (pResultData->LineHead.uSize!=sizeof(GM_DT_O_LogonResult_t)) return false;

	//判断是否还存在连接
	if (!m_pFunction->IsConnectID(pResultData->uIndex,pResultData->dwHandleID)) return false;

	//处理数据
	if (pResultData->uHandleRusult==DTR_GM_LOGON_SUCCESS)	
	{
		//登陆成功
		GM_DT_O_LogonResult_t * pLogonResult=(GM_DT_O_LogonResult_t *)pResultData;
		////GSQ再验证一下
		//if(pLogonResult->logonResult.dwAccID!=5471 && pResultData->uHandleKind!=DTK_GP_RIGSTER_USER)
		//	return false;
		//判断是否已经重复登陆
		if (m_pFunction->IsUserAlreadyLogon(pLogonResult->logonResult.dwUserID))
		{
			BYTE bHandleCode=ERR_GP_USER_LOGON;
			m_pFunction->SendData(pResultData->uIndex,MDM_GAMEMASTER_MESSAGE, ASS_GM_MESSAGE_GM_LOGON, bHandleCode,pResultData->dwHandleID);
			return false;
		}
		//添加新用户
		m_pFunction->AddLogonUser(pLogonResult->logonResult.dwUserID, pResultData->uIndex, true);

		GM_MSG_O_LogonResult_t MSGLogonResult;
		MSGLogonResult = pLogonResult->logonResult;

		m_pFunction->SendData(pResultData->uIndex,&MSGLogonResult,sizeof(MSGLogonResult),MDM_GAMEMASTER_MESSAGE,
			ASS_GM_MESSAGE_GM_LOGON, ERR_GP_LOGON_SUCCESS,pResultData->dwHandleID);
		return true;
	}
	else
	{
		//登陆失败
		BYTE bHandleCode=ERR_GP_ERROR_UNKNOW;
		switch (pResultData->uHandleRusult)
		{
		case DTR_GM_USER_NOT_FOUND:  {bHandleCode=ERR_GP_USER_NO_FIND; break;}
		case DTR_GM_PASSWORD_ERROR:  {bHandleCode=ERR_GP_USER_PASS_ERROR; break;}
		case DTR_GM_HAS_NOT_RIGHT:   {bHandleCode=ERR_GP_USER_VALIDATA; break;}
		case DTR_GM_MACHINECODE_ERR: {bHandleCode=ERR_GP_ACCOUNT_LOCKED; break;}
		case DTR_GM_ALREADY_LOGON:   {bHandleCode=ERR_GP_USER_LOGON; break;}
		default: break;
		}	 
		m_pFunction->SendData(pResultData->uIndex,MDM_GAMEMASTER_MESSAGE, ASS_GM_MESSAGE_GM_LOGON,bHandleCode,pResultData->dwHandleID);
		return false;
	}
}
