#include "StdAfx.h"
#include "GameListCtrl0.h"
#include "GameLogonDT.h"
#include "CommonUse.h"

/*******************************************************************************************************/

//更新游戏列表
bool CServerGameListManage0::UpdateGameListBuffer(HANDLE hDatabase)
{
	//try
	//{
	//读取游戏类型列表
	HANDLE hRecord=sqlSPSetName(hDatabase,"SP_GetGameKind");
	if(hRecord<(HANDLE)0x1000)
	{
		sqlSPClose(hDatabase,hRecord);
		return false;
	}

	if(!sqlSPExec(hDatabase,hRecord,true))
	{
		sqlSPClose(hDatabase,hRecord);
		return false;
	}

	DWORD dwReadKindCount=(DWORD)sqlGetRecordCount(hDatabase,hRecord);


	if (dwReadKindCount>m_dwKindBufCount)
	{
		delete [] m_pKindPtr;
		m_pKindPtr=new ComKindInfo [dwReadKindCount];
		if (m_pKindPtr==NULL) 
		{
			sqlSPClose(hDatabase,hRecord);
			return false;
		}
		m_dwKindBufCount=dwReadKindCount;
	}

	//读取数据库获取游戏类型
	ComKindInfo KindRead;
	KindRead.Head.uSize=sizeof(KindRead);
	KindRead.Head.bDataType=GLK_GAME_KIND;
	m_dwKindCount=dwReadKindCount;
	int iCount=0;
	while(!sqlEndOfFile(hDatabase,hRecord))
	{
		sqlGetValue(hDatabase,hRecord,TEXT("KindID"),KindRead.uKindID);
		sqlGetValue(hDatabase,hRecord,TEXT("KindName"),KindRead.szKindName,sizeof(KindRead.szKindName));
		sqlGetValue(hDatabase,hRecord,TEXT("ParentKindID"),KindRead.uParentKindID);

		*(m_pKindPtr+(iCount++))=KindRead;

		sqlMoveNext(hDatabase,hRecord);
	}
	sqlSPClose(hDatabase,hRecord);

	//读取名字信息
	hRecord=sqlSPSetName(hDatabase,"SP_GetGameName");
	if(hRecord<(HANDLE)0x1000)
	{
		sqlSPClose(hDatabase,hRecord);
		return false;
	}

	if(!sqlSPExec(hDatabase,hRecord,true))
	{
		sqlSPClose(hDatabase,hRecord);
		return false;
	}

	DWORD dwReadNameCount=(DWORD)sqlGetRecordCount(hDatabase,hRecord);
	if (dwReadNameCount>m_dwNameBufCount)
	{
		delete [] m_pNamePtr;
		m_pNamePtr=new ComNameInfo [dwReadNameCount];
		if (m_pNamePtr==NULL) 
		{
			sqlSPClose(hDatabase,hRecord); 
			return false;
		}
		m_dwNameBufCount=dwReadNameCount;
	}

	//读取数据库
	ComNameInfo NameRead;
	NameRead.Head.uSize=sizeof(NameRead);
	NameRead.Head.bDataType=GLK_GAME_NAME;
	m_dwNameCount=dwReadNameCount;
	iCount=0;

	//读取配置文件（SpecialRule.bcf）
	CBcfFile bcfFile(CBcfFile::GetAppPath() + "SpecialRule.bcf");

	while(!sqlEndOfFile(hDatabase,hRecord))
	{
		sqlGetValue(hDatabase,hRecord,TEXT("KindID"),NameRead.uKindID);
		sqlGetValue(hDatabase,hRecord,TEXT("NameID"),NameRead.uNameID);
		sqlGetValue(hDatabase,hRecord,TEXT("ComName"),NameRead.szGameName,sizeof(NameRead.szGameName));
		sqlGetValue(hDatabase,hRecord,TEXT("ComProcess"),NameRead.szGameProcess,sizeof(NameRead.szGameProcess));
		sqlGetValue(hDatabase,hRecord,TEXT("Version"),NameRead.uVer);

		//获取新和热标识
		CHAR buffer[30] = {0};
		sprintf(buffer,"%ld",NameRead.uNameID);
		CString temp(buffer);
		BYTE RetVal = bcfFile.GetKeyVal(_T("HotOrNew"),temp,8);
		NameRead.m_byHotOrNew = RetVal;
		*(m_pNamePtr+(iCount++))=NameRead;
		sqlMoveNext(hDatabase,hRecord);
		TRACE("SP_GetGameName: %d %d %s %s\n",NameRead.uKindID,NameRead.uNameID,NameRead.szGameName,NameRead.szGameProcess);
	}
	sqlSPClose(hDatabase,hRecord);

	bcfFile.CloseFile();

	//读取房间信息
	hRecord=sqlSPSetName(hDatabase,"SP_GetOnlineRoom");
	if(hRecord<(HANDLE)0x1000)
	{
		sqlSPClose(hDatabase,hRecord);	
		return false;
	}

	if(!sqlSPExec(hDatabase,hRecord,true))
	{
		sqlSPClose(hDatabase,hRecord);
		return false;
	}

	DWORD dwReadRoomCount=(DWORD)sqlGetRecordCount(hDatabase,hRecord);

	bool bNew=false;
	if (dwReadRoomCount>m_dwRoomBufCount)
	{
		delete [] m_pRoomPtr;
		m_pRoomPtr=new ComRoomInfo [dwReadRoomCount];
		if (m_pRoomPtr==NULL) 
		{
			sqlSPClose(hDatabase,hRecord);
			throw DTR_GP_ERROR_UNKNOW;
		}
		m_dwRoomBufCount=dwReadRoomCount;
		bNew=true;
	}

	//读取数据库
	ComRoomInfo RoomRead;
	RoomRead.Head.uSize=sizeof(RoomRead);
	RoomRead.Head.bDataType=GLK_GAME_ROOM;
	m_dwRoomCount=dwReadRoomCount;
	iCount=0;
	bool b=false;
	while(!sqlEndOfFile(hDatabase,hRecord))
	{
		b=sqlGetValue(hDatabase,hRecord,TEXT("GameKindID"),RoomRead.uKindID);
		b=sqlGetValue(hDatabase,hRecord,TEXT("GameNameID"),RoomRead.uNameID);
		b=sqlGetValue(hDatabase,hRecord,TEXT("RoomID"),RoomRead.uRoomID);
		b=sqlGetValue(hDatabase,hRecord,TEXT("GameTypeID"),RoomRead.uComType);
		if(bNew)
			RoomRead.uPeopleCount=0;
		b=sqlGetValue(hDatabase,hRecord,TEXT("VirtualUser"),RoomRead.uVirtualUser);
		RoomRead.uPeopleCount+=RoomRead.uVirtualUser;
		b=sqlGetValue(hDatabase,hRecord,TEXT("VirtualGameTime"),RoomRead.uVirtualGameTime);
		b=sqlGetValue(hDatabase,hRecord,TEXT("RoomRule"),RoomRead.dwRoomRule);

		b=sqlGetValue(hDatabase,hRecord,TEXT("SocketPort"),RoomRead.uServicePort);
		b=sqlGetValue(hDatabase,hRecord,TEXT("DeskPeople"),RoomRead.uDeskPeople);
		b=sqlGetValue(hDatabase,hRecord,TEXT("DeskCount"),RoomRead.uDeskCount);
		b=sqlGetValue(hDatabase,hRecord,TEXT("ServerIP"),RoomRead.szServiceIP,sizeof(RoomRead.szServiceIP));
		
		//混战场房间名称   --begin
		b=sqlGetValue(hDatabase,hRecord,TEXT("BattleRoomID"),RoomRead.uBattleRoomID);
		b=sqlGetValue(hDatabase,hRecord,TEXT("BattleMatchTable"),RoomRead.szBattleGameTable,sizeof(RoomRead.szBattleGameTable));

		RoomRead.i64TimeStart = 0;
		RoomRead.i64TimeEnd = 0;
		if (RoomRead.dwRoomRule & GRR_CONTEST)
		{
			int nYear = 0, nMonth = 0, nDate = 0, nHour = 0, nMin = 0, nSec = 0, nss = 0;  
			CString temp;
		//	sqlGetValue(hDatabase, hRecord, TEXT("StartTime"), temp);
			if (temp != "")
			{
				sscanf(temp.GetBuffer(), TEXT("%d-%d-%d %d:%d:%d.%d"), &nYear, &nMonth, &nDate, &nHour, &nMin, &nSec, &nss);

				CTime _t(nYear, nMonth, nDate, nHour, nMin, nSec); 
				RoomRead.i64TimeStart = _t.GetTime();
			}
			else
			{
				RoomRead.i64TimeStart = 0;
			}
		//	sqlGetValue(hDatabase, hRecord, TEXT("EndTime"), temp);
			if (temp != "")
			{
				sscanf(temp.GetBuffer(), TEXT("%d-%d-%d %d:%d:%d.%d"), &nYear, &nMonth, &nDate, &nHour, &nMin, &nSec, &nss);

				CTime _t(nYear, nMonth, nDate, nHour, nMin, nSec); 
				RoomRead.i64TimeEnd = _t.GetTime();
			}
			else
			{
				RoomRead.i64TimeEnd = 0;
			}
			sqlGetValue(hDatabase, hRecord, TEXT("ContestID"), RoomRead.iContestID);
		}
		
		char szRoomName[61];
		memset(szRoomName,0,sizeof(szRoomName));
		if (RoomRead.uBattleRoomID > 0)
		{
			HANDLE hRecordTmp=sqlSPSetName(hDatabase,"SP_GetBattleRoomName");
			sqlSPAddPara(hDatabase,hRecordTmp,"@BattleRoomID",RoomRead.uBattleRoomID);
			sqlSPExec(hDatabase,hRecordTmp,true);
			sqlGetValue(hDatabase,hRecordTmp,TEXT("RoomName"),szRoomName,sizeof(szRoomName));
			sqlSPClose(hDatabase,hRecordTmp); // zxj dfw 2009-11-12
		}
		else
		{
			sqlGetValue(hDatabase,hRecord,TEXT("RoomName"),szRoomName,sizeof(szRoomName));
		}
		memcpy(RoomRead.szGameRoomName,szRoomName,sizeof(szRoomName));  
		//--end

		int iVIPRoom = 0;
		b=sqlGetValue(hDatabase,hRecord,TEXT("VIPRoom"),iVIPRoom);
		RoomRead.bVIPRoom = (iVIPRoom!=0);
		b=sqlGetValue(hDatabase,hRecord,TEXT("Version"),RoomRead.uVer);

		//{{Add by lxl 20101008 判断该房间是否具有密码
		char szPassWord[61];
		memset(szPassWord, 0, sizeof(szPassWord));
		b=sqlGetValue(hDatabase,hRecord,TEXT("RoomPassword"), szPassWord, sizeof(szPassWord));
		if (strlen(szPassWord)>10) // MD5密码一定有16字节以上，如果没这么多，表示是个无效的字符串
		{
			RoomRead.bHasPassword = true;
		}
		else
		{
			RoomRead.bHasPassword = false;
		}

		//租赁房间ID与时间
		sqlGetValue(hDatabase,hRecord,TEXT("tenancyID"), RoomRead.iLeaseID);
		if (RoomRead.iLeaseID != 0)
		{
			int nYear = 0, nMonth = 0, nDate = 0, nHour = 0, nMin = 0, nSec = 0, nss = 0;  
			CString temp;
			sqlGetValue(hDatabase, hRecord, TEXT("OpenRoomDate"), temp);
			if (temp != "")
			{
				sscanf(temp.GetBuffer(), TEXT("%d-%d-%d %d:%d:%d.%d"), &nYear, &nMonth, &nDate, &nHour, &nMin, &nSec, &nss);

				CTime _t(nYear, nMonth, nDate, nHour, nMin, nSec); 
				RoomRead.i64LeaseTimeStart = _t.GetTime();
			}
			else
			{
				RoomRead.i64LeaseTimeStart = 0;
			}
			int idays = 0;
			sqlGetValue(hDatabase,hRecord,TEXT("AppleyDays"),idays);
			RoomRead.i64LeaseTimeEnd = RoomRead.i64LeaseTimeStart + idays * 24 * 60 * 60;
		}


		*(m_pRoomPtr+(iCount++))=RoomRead;
		sqlMoveNext(hDatabase,hRecord);
	}
	sqlSPClose(hDatabase,hRecord);
	return true;

}


//析构函数
CAFCGameListItem::CAFCGameListItem()
{
	return;
}

//析构函数
CAFCGameListItem::~CAFCGameListItem()
{
	for (int i=0;i<m_ItemPtrArray.GetCount();i++)
	{
		delete (CAFCGameListItem *)m_ItemPtrArray.GetAt(i);
		m_ItemPtrArray.SetAt(i,NULL);
	}
	return;
}

//加入游戏类型
bool CAFCGameListItem::AddGameKind(ComKindInfo * pGameKind)
{
	try
	{
		CAFCKindItem * pGameKindItem=new CAFCKindItem;
		if (pGameKindItem!=NULL)
		{
			//设置数据
			pGameKindItem->m_KindInfo=*pGameKind;
			m_ItemPtrArray.Add(pGameKindItem);
			return true;
		}
	}
	catch (...) {TRACE("CATCH:%s with %s\n",__FILE__,__FUNCTION__);}
	return false;
}

//加入游戏名字
bool CAFCGameListItem::AddGameName(ComNameInfo * pGameName)
{
	try
	{
		CAFCNameItem * pGameNameItem=new CAFCNameItem;
		if (pGameNameItem!=NULL)
		{
			//设置数据
			pGameNameItem->m_NameInfo=*pGameName;
			m_ItemPtrArray.Add(pGameNameItem);
			return true;
		}
	}
	catch (...) {TRACE("CATCH:%s with %s\n",__FILE__,__FUNCTION__);}
	return false;
}

//加入游戏房间
bool CAFCGameListItem::AddGameRoom(ComRoomInfo * pGameRoom)
{
	try
	{
		CAFCRoomItem * pGameRoomItem=new CAFCRoomItem;
		if (pGameRoomItem!=NULL)
		{
			//设置数据
			pGameRoomItem->m_RoomInfo=*pGameRoom;
			m_ItemPtrArray.Add(pGameRoomItem);
			return true;
		}
	}
	catch (...) {TRACE("CATCH:%s with %s\n",__FILE__,__FUNCTION__);}
	return false;
}

/*******************************************************************************************************/

//构造函数
CServerGameListManage0::CServerGameListManage0()
{
	m_pRoomPtr=NULL;
	m_pNamePtr=NULL;
	m_pKindPtr=NULL;
	m_dwRoomCount=0;		
	m_dwNameCount=0;		
	m_dwKindCount=0;		
	m_dwRoomBufCount=0;	
	m_dwNameBufCount=0;	
	m_dwKindBufCount=0;	
}

//析构函数
CServerGameListManage0::~CServerGameListManage0()
{
	delete [] m_pRoomPtr;
	delete [] m_pNamePtr;
	delete [] m_pKindPtr;
	for (int i=0;i<m_KindArray.GetCount();i++)
	{
		delete (CAFCGameListItem *)m_KindArray.GetAt(i);
		m_KindArray.SetAt(i,NULL);
	}
	return;
}

//获取在线人数 
INT_PTR CServerGameListManage0::FillOnLineCount(char * pOnLineBuffer, INT_PTR uBufferSize, INT_PTR uBeginPos, bool & bFinish)
{
	//寻找拷贝数据
	INT_PTR uCopyPos=0;
	CAFCNameItem * pNameItem=NULL;
	while ((INT_PTR)((uCopyPos+1)*sizeof(MSG_GP_SR_OnLineStruct))<=uBufferSize)
	{
		if ((uBeginPos+uCopyPos)>=m_NameArray.GetCount()) break;
		pNameItem=(CAFCNameItem *)m_NameArray.GetAt(uBeginPos+uCopyPos);
		if (pNameItem!=NULL) 
		{
			MSG_GP_SR_OnLineStruct * pOnLineBuf=(MSG_GP_SR_OnLineStruct *)(pOnLineBuffer+uCopyPos*sizeof(MSG_GP_SR_OnLineStruct));
			pOnLineBuf->uKindID=pNameItem->m_NameInfo.uKindID;
			pOnLineBuf->uNameID=pNameItem->m_NameInfo.uNameID;
			pOnLineBuf->uOnLineCount=pNameItem->m_NameInfo.m_uOnLineCount;
		}
		uCopyPos++;
	}

	//拷贝完成
	bFinish=((uBeginPos+uCopyPos)>=m_NameArray.GetCount());
	return uCopyPos;
}

//获取游戏类型
INT_PTR CServerGameListManage0::FillGameKind(char * pKindBuffer, INT_PTR uBufferSize, INT_PTR uBeginPos, bool & bFinish)
{
	//拷贝数据
	INT_PTR uCopyPos=0;
	CAFCKindItem * pKindItem=NULL;
	while ((INT_PTR)((uCopyPos+1)*sizeof(ComKindInfo))<=uBufferSize)
	{
		if ((uBeginPos+uCopyPos)>=m_KindArray.GetCount()) 
			break;
		pKindItem=(CAFCKindItem *)m_KindArray.GetAt(uBeginPos+uCopyPos);
		if (pKindItem!=NULL) 
		{
			CopyMemory(pKindBuffer+uCopyPos*sizeof(ComKindInfo),&pKindItem->m_KindInfo,sizeof(ComKindInfo));
			uCopyPos++;
		}
	}

	//拷贝完成
	bFinish=((uBeginPos+uCopyPos)>=m_KindArray.GetCount());
	return uCopyPos;
}

//获取游戏名字
INT_PTR CServerGameListManage0::FillGameName(char * pNamdBuffer, INT_PTR uBufferSize, INT_PTR uBeginPos, bool & bFinish)
{
	//寻找拷贝数据
	INT_PTR uCopyPos=0;
	CAFCNameItem * pNameItem=NULL;
	while ((INT_PTR)((uCopyPos+1)*sizeof(ComNameInfo))<=uBufferSize)
	{
		if ((uBeginPos+uCopyPos)>=m_NameArray.GetCount()) break;
		pNameItem=(CAFCNameItem *)m_NameArray.GetAt(uBeginPos+uCopyPos);

		if (pNameItem!=NULL) 
		{
			CopyMemory(pNamdBuffer+uCopyPos*sizeof(ComNameInfo),&pNameItem->m_NameInfo,sizeof(ComNameInfo));
			uCopyPos++;
		}
	}
	//拷贝完成
	bFinish=((uBeginPos+uCopyPos)>=m_NameArray.GetCount());
	return uCopyPos;
}

//获取房间名字
INT_PTR CServerGameListManage0::FillGameRoom(char * pRoomBuffer, INT_PTR uBufferSize, UINT uKindID, UINT uNameID, INT_PTR uBeginPos, bool & bFinish)
{
	INT_PTR uCopyPos=0;
	CAFCNameItem * pNameItem=FindNameItem(FindKindItem(uKindID),uNameID);
	if (pNameItem!=NULL)
	{
		CAFCRoomItem * pRoomItem=NULL;
		while ((INT_PTR)((uCopyPos+1)*sizeof(ComRoomInfo))<=uBufferSize)
		{
			if ((uBeginPos+uCopyPos)>=pNameItem->m_ItemPtrArray.GetCount()) break;
			pRoomItem=(CAFCRoomItem *)pNameItem->m_ItemPtrArray.GetAt(uBeginPos+uCopyPos);
			if (pRoomItem!=NULL) 
			{
				CopyMemory(pRoomBuffer+uCopyPos*sizeof(ComRoomInfo),&pRoomItem->m_RoomInfo,sizeof(ComRoomInfo));
				uCopyPos++;
			}
		}
	}

	//拷贝完成
	bFinish=((pNameItem==NULL)||((uBeginPos+uCopyPos)>=pNameItem->m_ItemPtrArray.GetCount()));
	return uCopyPos;
}

//寻找类型
CAFCKindItem * CServerGameListManage0::FindKindItem(UINT uKindID)
{
	for (INT_PTR i=0;i<m_KindArray.GetCount();i++)
	{
		CAFCKindItem * pKindItem=(CAFCKindItem *)m_KindArray.GetAt(i);
		if ((pKindItem!=NULL)&&(pKindItem->m_KindInfo.uKindID==uKindID)) return pKindItem;
	}
	return NULL;
}

//寻找名字
CAFCNameItem * CServerGameListManage0::FindNameItem(CAFCKindItem * pKindItem, UINT uNameID)
{
	if (pKindItem!=NULL)
	{
		for (INT_PTR i=0;i<pKindItem->m_ItemPtrArray.GetCount();i++)
		{
			CAFCNameItem * pNameItem=(CAFCNameItem *)pKindItem->m_ItemPtrArray.GetAt(i);
			if ((pNameItem!=NULL)&&(pNameItem->m_NameInfo.uNameID==uNameID)) return pNameItem;
		}
	}
	return NULL;
}

//寻找房间
CAFCRoomItem * CServerGameListManage0::FindRoomItem(CAFCNameItem * pNameItem, UINT uRoomID)
{
	if (pNameItem!=NULL)
	{
		for (INT_PTR i=0;i<pNameItem->m_ItemPtrArray.GetCount();i++)
		{
			CAFCRoomItem * pRoomItem=(CAFCRoomItem *)pNameItem->m_ItemPtrArray.GetAt(i);
			if ((pRoomItem!=NULL)&&(pRoomItem->m_RoomInfo.uRoomID==uRoomID)) return pRoomItem;
		}
	}
	return NULL;
}


//读取类型列表
bool CServerGameListManage0::UpdateGameList()
{
	try
	{
		//保存旧数组
		CPtrArray m_TempKindArray;
		m_TempKindArray.SetSize(m_KindArray.GetCount());
		for (INT_PTR i=0;i<m_KindArray.GetCount();i++)
		{
			m_TempKindArray.SetAt(i,m_KindArray.GetAt(i));
			m_KindArray.SetAt(i,NULL);
		}

		//读取新信息
		ComKindInfo * pKindRead=NULL;
		CAFCKindItem * pKindItem=NULL;
		for (UINT i=0;i<m_dwKindCount;i++)
		{
			//设置数据
			pKindItem=NULL;
			pKindRead=m_pKindPtr+i;

			//寻找数据
			INT_PTR j;//VC2005
			for (j=0;j<m_TempKindArray.GetCount();j++)
			{
				pKindItem=(CAFCKindItem *)m_TempKindArray.GetAt(j);
				if ((pKindItem!=NULL)&&(pKindItem->m_KindInfo.uKindID==pKindRead->uKindID))
				{
					m_TempKindArray.SetAt(j,NULL);
					break;
				}
			}
			if (j==m_TempKindArray.GetCount()) 
				pKindItem=NULL;

			//新建数据
			if (pKindItem==NULL)
			{
				pKindItem=new CAFCKindItem();
				if (pKindItem==NULL) throw DTR_GP_ERROR_UNKNOW;
			}

			//写入数据
			pKindItem->m_KindInfo=*pKindRead;
			m_KindArray.SetAtGrow(i,pKindItem);
		}
		m_KindArray.SetSize(m_dwKindCount);

		//清理数据 
		for (INT_PTR i=0;i<m_TempKindArray.GetCount();i++)
		{
			pKindItem=(CAFCKindItem *)m_TempKindArray.GetAt(i);
			delete pKindItem;
		}

		//更新游戏名字
		UINT uNameCount=0;
		for (INT_PTR i=0;i<m_KindArray.GetCount();i++)
		{
			pKindItem=(CAFCKindItem *)m_KindArray.GetAt(i);
			if (pKindItem!=NULL) 
			{
				UpdateGameName(pKindItem);
				for (INT_PTR i=0;i<pKindItem->m_ItemPtrArray.GetCount();i++)
				{
					CAFCNameItem * pNameItem=(CAFCNameItem *)pKindItem->m_ItemPtrArray.GetAt(i);
					if (pNameItem!=NULL)
					{
						m_NameArray.SetAtGrow(uNameCount++,pNameItem);
					}
				}
			}
		}
		m_NameArray.SetSize(uNameCount);
		return true;
	}
	catch (...) {TRACE("CATCH:%s with %s\n",__FILE__,__FUNCTION__);}

	//清理工作
	for (int i=0;i<m_KindArray.GetCount();i++)
	{
		delete (CAFCGameListItem *)m_KindArray.GetAt(i);
	}
	m_NameArray.SetSize(0);
	m_KindArray.SetSize(0);

	return false;
}

//更新名字列表
bool CServerGameListManage0::UpdateGameName(CAFCKindItem * pKindItem)
{
	//保存旧数组
	CPtrArray m_TempNameArray;
	m_TempNameArray.SetSize(pKindItem->m_ItemPtrArray.GetCount());
	for (INT_PTR i=0;i<pKindItem->m_ItemPtrArray.GetCount();i++)
	{
		m_TempNameArray.SetAt(i,pKindItem->m_ItemPtrArray.GetAt(i));
		pKindItem->m_ItemPtrArray.SetAt(i,NULL);
	}

	//读取新信息
	UINT uNewCount=0;
	ComNameInfo * pNameRead=NULL;
	CAFCNameItem * pNameItem=NULL;
	for (UINT i=0;i<m_dwNameCount;i++)
	{
		//设置数据
		pNameItem=NULL;
		pNameRead=m_pNamePtr+i;
		if (pNameRead->uKindID!=pKindItem->m_KindInfo.uKindID) continue;

		//寻找数据
		INT_PTR j;//VC2005
		for (j=0;j<m_TempNameArray.GetCount();j++)
		{
			pNameItem=(CAFCNameItem *)m_TempNameArray.GetAt(j);
			if ((pNameItem!=NULL)&&(pNameItem->m_NameInfo.uNameID==pNameRead->uNameID))
			{
				m_TempNameArray.SetAt(j,NULL);
				break;
			}
		}
		if (j==m_TempNameArray.GetCount()) pNameItem=NULL;

		//新建数据
		if (pNameItem==NULL)
		{
			pNameItem=new CAFCNameItem();
			if (pNameItem==NULL) throw DTR_GP_ERROR_UNKNOW;
		}

		//写入数据
		pNameItem->m_NameInfo=*pNameRead;
		pKindItem->m_ItemPtrArray.SetAtGrow(uNewCount++,pNameItem);
	}
	pKindItem->m_ItemPtrArray.SetSize(uNewCount);

	//清理数据 
	for (INT_PTR i=0;i<m_TempNameArray.GetCount();i++)
	{
		pNameItem=(CAFCNameItem *)m_TempNameArray.GetAt(i);
		delete pNameItem;
	}

	//更新游戏房间
	for (INT_PTR i=0;i<pKindItem->m_ItemPtrArray.GetCount();i++)
	{
		pNameItem=(CAFCNameItem *)pKindItem->m_ItemPtrArray.GetAt(i);
		if (pNameItem!=NULL) UpdateGameRoom(pNameItem);
	}

	return true;
}

//更新房间列表
bool CServerGameListManage0::UpdateGameRoom(CAFCNameItem * pNameItem)
{
	//保存旧数组
	CPtrArray m_TempRoomArray;
	m_TempRoomArray.SetSize(pNameItem->m_ItemPtrArray.GetCount());
	for (INT_PTR i=0;i<pNameItem->m_ItemPtrArray.GetCount();i++)
	{
		m_TempRoomArray.SetAt(i,pNameItem->m_ItemPtrArray.GetAt(i));
		pNameItem->m_ItemPtrArray.SetAt(i,NULL);
	}

	//读取新信息
	UINT uNewCount=0;
	ComRoomInfo * pRoomRead=NULL;
	CAFCRoomItem * pRoomItem=NULL;
	pNameItem->m_NameInfo.m_uOnLineCount=0L;
	for (UINT i=0;i<m_dwRoomCount;i++)
	{
		//设置数据
		pRoomItem=NULL;
		pRoomRead=m_pRoomPtr+i;
		if ((pRoomRead->uNameID!=pNameItem->m_NameInfo.uNameID)||(pRoomRead->uKindID!=pNameItem->m_NameInfo.uKindID)) continue;

		//寻找数据
		INT_PTR j;////VC2005
		for ( j=0;j<m_TempRoomArray.GetCount();j++)
		{
			pRoomItem=(CAFCRoomItem *)m_TempRoomArray.GetAt(j);
			if (pRoomItem!=NULL)
			{
				m_TempRoomArray.SetAt(j,NULL);
				break;
			}
		}
		if (j==m_TempRoomArray.GetCount()) pRoomItem=NULL;

		//新建数据
		if (pRoomItem==NULL)
		{
			pRoomItem=new CAFCRoomItem();
			if (pRoomItem==NULL) throw DTR_GP_ERROR_UNKNOW;
		}

		//写入数据
		pRoomItem->m_RoomInfo=*pRoomRead;

		bool bSame = false;
		for (INT_PTR k=0; k<pNameItem->m_ItemPtrArray.GetCount(); k++)
		{
			CAFCRoomItem * pRoomItemSame = (CAFCRoomItem *)pNameItem->m_ItemPtrArray.GetAt(k);
			if (NULL != pRoomItemSame && pRoomItemSame->m_RoomInfo.uRoomID == pRoomItem->m_RoomInfo.uRoomID)
			{
				bSame = true;
				pNameItem->m_NameInfo.m_uOnLineCount += (pRoomItem->m_RoomInfo.uPeopleCount-pRoomItemSame->m_RoomInfo.uPeopleCount);
				break;
			}
		}

		if (bSame)
			continue;

		pNameItem->m_NameInfo.m_uOnLineCount+=pRoomItem->m_RoomInfo.uPeopleCount;
		pNameItem->m_ItemPtrArray.SetAtGrow(uNewCount++,pRoomItem);
	}
	pNameItem->m_ItemPtrArray.SetSize(uNewCount);

	//清理数据 
	for (INT_PTR i=0;i<m_TempRoomArray.GetCount();i++)
	{
		CAFCRoomItem * pRoomItem=(CAFCRoomItem *)m_TempRoomArray.GetAt(i);
		delete pRoomItem;
	}

	return true;
}

/*******************************************************************************************************/
