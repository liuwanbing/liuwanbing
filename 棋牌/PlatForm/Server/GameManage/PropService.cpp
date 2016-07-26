#include "StdAfx.h"
#include "PropService.h"
#include "GameMainManage.h"

CPropService::CPropService(CWnd *pWnd)
{
	pParentWnd=pWnd;
}

CPropService::~CPropService(void)
{
}

bool CPropService::OnNetMessage(NetMessageHead *pNetHead, void* pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID)
{
	ASSERT(pNetHead->bMainID == MDM_GR_PROP);
	switch(pNetHead->bAssistantID)
	{
	case ASS_PROP_USEPROP:
		OnUseProp(pNetHead,pData,uSize,uAccessIP,uIndex,dwHandleID);
		break;
	case ASS_PROP_SMALL_BOARDCASE:
		OnUserSmallBoardcase(pNetHead,pData,uSize,uAccessIP,uIndex,dwHandleID);
		break;
		//case ASS_C_GET_ITEM_DATA:
		//	{
		//		return OnGetItemDataMsg(pNetHead,pData,uSize,uAccessIP,uIndex,dwHandleID);
		//	}
		//case ASS_C_USE_ITEM:
		//	{
		//		return OnUserUseItemMsg(pNetHead,pData,uSize,uAccessIP,uIndex,dwHandleID);
		//	}
		//case ASS_C_GET_BUY_ITEM_DATA:
		//	{
		//		return OnGetBuyItemDataMsg(pNetHead,pData,uSize,uAccessIP,uIndex,dwHandleID);
		//	}
		//case ASS_C_BUY_ITEM_DATA:
		//	{
		//		return OnBuyItemMsg(pNetHead,pData,uSize,uAccessIP,uIndex,dwHandleID);
		//	}
	case ASS_PROP_KICKUSER:
		{
			if(sizeof(_TAG_KICKUSER)!= uSize)
				return true;
			CGameMainManage *pWnd=(CGameMainManage*)pParentWnd;
			CGameUserInfo * pUserInfo = pWnd->m_UserManage.GetOnLineUserInfo(uIndex);
			if(pUserInfo == NULL)
				return true;
			CString stime;
			stime.Format("%d",CTime::GetCurrentTime());        //当前的时间，String格式
			int curtime=atoi(stime);                        //转换为int
			if(pUserInfo->m_UserData.iVipTime < curtime )
				return true;

			_TAG_KICKUSER * pKick = (_TAG_KICKUSER*)pData;
			CGameUserInfo * pDestUserInfo = pWnd->m_UserManage.FindOnLineUser(pKick->dwDestID);
			if(pDestUserInfo == NULL)
				return true;
			if(pUserInfo->m_UserData.bDeskNO == 255)
				return true;
			CGameDesk *pDesk = pWnd->GetDeskObject(pUserInfo->m_UserData.bDeskNO);//*(m_pDesk + pUserInfo->m_UserData.bDeskNO);
			if(pDesk == NULL)
				return true;
			if(pDesk->IsPlayingByGameStation())//游戏中踢人
			{
				//m_TCPSocket.SendData(pDestUserInfo->m_uSocketIndex,pKick,sizeof(tagKickUserMsg),MDM_GR_VIP_MSG,ASS_S_KICK_USER,0,0);
				return true;
			}
			if (pDestUserInfo->m_UserData.bUserState==USER_WATCH_GAME) 
			{ 
				pDesk->WatchUserLeftDesk(pDestUserInfo->m_UserData.bDeskStation,pDestUserInfo); 
			} 
			else  
			{ 
				pDesk->UserLeftDesk(pDestUserInfo->m_UserData.bDeskStation,pDestUserInfo); 
			} 
			//pDesk->UserLeftDesk(pDestUserInfo->m_UserData.bDeskStation,pDestUserInfo);
			//发送给此人消息
			pWnd->m_TCPSocket.SendData(pDestUserInfo->m_uSocketIndex,pKick,sizeof(_TAG_KICKUSER),MDM_GR_PROP,ASS_PROP_KICKUSER,0,0);
			return true;
		}
	}
	return true;
}

bool CPropService::OnUserSmallBoardcase(NetMessageHead *pNetHead, void* pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID)
{
	if(uSize!=sizeof(_TAG_BOARDCAST))
		return true;
	PROP_GPR_I_BOARDCAST iBoardcast;
	ZeroMemory(&iBoardcast,sizeof(PROP_GPR_I_BOARDCAST));

	memcpy(&iBoardcast.boardCast,pData,uSize);
	CGameMainManage *pWnd=(CGameMainManage*)pParentWnd;
	
	//yjj 090323
	if (!pWnd->m_TalkFilter.IsAllowableTalk(iBoardcast.boardCast.szMessage,strlen(iBoardcast.boardCast.szMessage)))
	{
		const CHAR* pTemTalk = pWnd->m_TalkFilter.GetReplaceContent();
		strcpy(iBoardcast.boardCast.szMessage,pTemTalk);
	}
	pWnd->m_SQLDataManage.PushLine(&iBoardcast.DataBaseHead,sizeof(PROP_GPR_I_BOARDCAST),DTK_GPR_PROP_SMALL_BRD,uIndex,dwHandleID);
	return true;
}

bool CPropService::OnDataBaseResult(DataBaseResultLine * pResultData)
{
	switch(pResultData->uHandleKind)
	{
	case DTK_GR_PROP_USE://持有道俱结果
		return OnUsePropResult(pResultData);
		break;
	case DTK_GPR_PROP_SMALL_BRD:
		return OnUserSmallBoardcaseResult(pResultData);
		break;
	}
	return true;
}

bool CPropService::OnUseProp(NetMessageHead *pNetHead, void* pData, UINT uSize, ULONG uAccessIP, UINT uIndex, DWORD dwHandleID)
{
	ASSERT(pNetHead->bMainID == MDM_GR_PROP);
	ASSERT(pNetHead->bAssistantID == ASS_PROP_USEPROP);

	CGameMainManage *pWnd=(CGameMainManage*)pParentWnd;
	CGameUserInfo * pUserInfo = pWnd->m_UserManage.GetOnLineUserInfo(uIndex);
	if (pUserInfo == NULL) 
		return false;

	if (uSize != sizeof(_TAG_USINGPROP))
		return false;

	//仅鲜花和砖头可以相互使用
	//long dwDestID = pUserInfo->m_UserData.dwUserID;
	_TAG_USINGPROP *usingProp=(_TAG_USINGPROP*)pData;
	if (NULL == usingProp)//20090522zht
	{
		return true;
	}
	CGameUserInfo *pTargetUserInfo=pWnd->m_UserManage.FindOnLineUser(usingProp->dwTargetUserID);
	if (NULL == pTargetUserInfo)//20090522zht
	{
		pWnd->m_TCPSocket.SendData(uIndex,MDM_GR_PROP,ASS_PROP_USEPROP,DTK_GP_PROP_GIVE_NOTARGETUSER,0);
		return true;
	}
	
	//从服务器取用户的信息
	usingProp->iDoubleTime=pTargetUserInfo->m_UserData.iDoublePointTime;
	usingProp->iProtectTime=pTargetUserInfo->m_UserData.iProtectTime;
	

	DL_GR_I_USEPROP propIUse;
	memcpy(&propIUse.usingProp,usingProp,sizeof(_TAG_USINGPROP));

	pWnd->m_SQLDataManage.PushLine(&propIUse.DataBaseHead,sizeof(DL_GR_I_USEPROP),DTK_GR_PROP_USE,0,0);
	return true;
}

bool CPropService::OnUserSmallBoardcaseResult(DataBaseResultLine *pResultData)
{
	if(pResultData->LineHead.uSize!=sizeof(PROP_GPR_O_BOARDCAST))
		return true;
	PROP_GPR_O_BOARDCAST * oBoard=(PROP_GPR_O_BOARDCAST*)pResultData;
	CGameMainManage *pWnd=(CGameMainManage*)pParentWnd;

	//oBoard->
	

	pWnd->m_TCPSocket.SendDataBatch(&(oBoard->boardCast), sizeof(_TAG_BOARDCAST),MDM_GR_PROP,ASS_PROP_SMALL_BOARDCASE,pResultData->uHandleRusult);
	return true;
}

bool CPropService::OnUsePropResult(DataBaseResultLine *pResultData)
{
	if (pResultData->LineHead.uSize != sizeof(DL_GR_O_USEPROP))
		return false;

	DL_GR_O_USEPROP	*useOProp = (DL_GR_O_USEPROP * )pResultData;

	CGameMainManage *pWnd=(CGameMainManage*)pParentWnd;

	//取得使用者的玩家对象
	CGameUserInfo *pPropUseUser = pWnd->m_UserManage.FindOnLineUser(useOProp->usingProp.dwUserID);
	//将道具使用目标对象设为使用对象
	CGameUserInfo *pPropTargetUser=pPropUseUser;

	if (pPropUseUser == NULL)
		return true;
	
	//2个属性，操作属性和取值属性
	int attribAction=useOProp->usingProp.nPropActionAttrib;
	int attribValue=useOProp->usingProp.nPropValueAttrib;
	

	switch(pResultData->uHandleRusult)
	{
	//未知错误，基本上是数据库的
	case DTR_GR_PROP_USE_ERR_UNKNOW:
		return true;
		break;
	//参数无效
	case DTR_GR_PROP_USE_ERR_PARAM_INVALID:
	//没有道具了
	case DTR_GR_PROP_USE_ERR_NOPROP:
		pWnd->m_TCPSocket.SendData(pPropUseUser->m_uSocketIndex,MDM_GR_PROP,ASS_PROP_USEPROP,pResultData->uHandleRusult,0);
		return true;
		break;
	}
	//剩下是使用道具成功
	//大小喇叭
	if(attribAction & 0x80 || attribAction & 0x100)
	{
	//	pWnd->m_TCPSocket.SendData(pPropUseUser->m_uSocketIndex,&(useOProp->usingProp),sizeof(_TAG_USINGPROP),MDM_GR_PROP,ASS_PROP_USEPROP,pResultData->uHandleRusult,0);
		return true;
	}

    // PengJiLin, 2010-6-1, 记牌器返回结果，用于修改显示次数
    if(attribAction & 0x200)
    {
        pWnd->m_TCPSocket.SendData(pPropUseUser->m_uSocketIndex,&(useOProp->usingProp),sizeof(_TAG_USINGPROP),MDM_GR_PROP,ASS_PROP_USEPROP,pResultData->uHandleRusult,0);
        return true;
    }

	//如果是对其它人使用，且其它人的ID号不同于使用者的ID，则取得道具使用目标的对象
	if(attribAction & 0x1 && useOProp->usingProp.dwUserID!=useOProp->usingProp.dwTargetUserID)
		pPropTargetUser=pWnd->m_UserManage.FindOnLineUser(useOProp->usingProp.dwTargetUserID);
	//负分清零功能 yinyi-080910
	if(attribAction & 0x2)
	{
		if(pPropTargetUser->m_UserData.dwPoint<0)
		{
			//wushuqun 2009.6.2
			//修改用户原分
			pPropTargetUser->m_dwScrPoint = 0;
			pPropTargetUser->m_UserData.dwPoint=0;
		}
	}
	//魅力值变化
	int fasciChange=0;
	if(attribAction & 0x10)
	{
		//魅力值变化量
		fasciChange=(attribValue & 0xFF0000)>>16;
		//增加魅力值还是减少魅力值
		if(attribAction & 0x20)
			fasciChange=-fasciChange;
		pPropTargetUser->m_UserData.dwFascination+=fasciChange;

	}
	//影响积分时间变化,双倍积分
	if(attribAction & 0x4)
		pPropTargetUser->m_UserData.iDoublePointTime=useOProp->usingProp.iDoubleTime;
	//影响积分时间变化,护身符
	if(attribAction & 0x8)
		pPropTargetUser->m_UserData.iProtectTime=useOProp->usingProp.iProtectTime;

	//向房间所有人发消息
	pWnd->m_TCPSocket.SendDataBatch(&(useOProp->usingProp), sizeof(_TAG_USINGPROP),MDM_GR_PROP,ASS_PROP_USEPROP,pResultData->uHandleRusult);

	//只有在同一张桌子，才会发动画效果
	if(attribAction & 0x40 && pPropUseUser->m_UserData.bDeskNO!=255 && pPropTargetUser->m_UserData.bDeskNO==pPropUseUser->m_UserData.bDeskNO)
	{
		CGameDesk * pGameDesk=pWnd->GetDeskObject(pPropUseUser->m_UserData.bDeskNO);
		if(pGameDesk)
		{
			//构造结构
			_TAG_PROP_MOIVE_USE propMoive;
			propMoive.dwUserID=pPropUseUser->m_UserData.dwUserID;
			propMoive.dwTargetUserID=pPropTargetUser->m_UserData.dwUserID;
			propMoive.nUserStation=pPropUseUser->m_UserData.bDeskStation;
			propMoive.nTargetUserStation=pPropTargetUser->m_UserData.bDeskStation;
			propMoive.nPropID=useOProp->usingProp.nPropID;
			propMoive.nPropActionAttrib=useOProp->usingProp.nPropActionAttrib;
			propMoive.nPropValueAttrib=useOProp->usingProp.nPropValueAttrib;
			pGameDesk->UserUseProp(pPropUseUser->m_UserData.bDeskStation,pPropTargetUser->m_UserData.bDeskStation,&propMoive);
		}
	}



	return true;
}