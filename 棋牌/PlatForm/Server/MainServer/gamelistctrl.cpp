#include "StdAfx.h"

#include "GameListCtrl.h"
#include "GameLogonDT.h"




//获取在线人数 
INT_PTR CServerGameListManage::FillAllNameOnLineCount(char * pOnLineBuffer, INT_PTR uBufferSize, INT_PTR uBeginPos, bool & bFinish)
{
	
	//寻找拷贝数据
	INT_PTR uCopyPos=0;
	CAFCNameItem * pNameItem=NULL;
	while ((INT_PTR)((uCopyPos+1)*sizeof(DL_GP_RoomListPeoCountStruct))<=uBufferSize)
	{
		if ((uBeginPos+uCopyPos)>=m_NameArray.GetCount()) break;
		pNameItem=(CAFCNameItem *)m_NameArray.GetAt(uBeginPos+uCopyPos);
		if (pNameItem!=NULL) 
		{
			DL_GP_RoomListPeoCountStruct * pOnLineBuf=(DL_GP_RoomListPeoCountStruct *)(pOnLineBuffer+uCopyPos*sizeof(DL_GP_RoomListPeoCountStruct));
			pOnLineBuf->uID=pNameItem->m_NameInfo.uNameID;
			pOnLineBuf->uOnLineCount=pNameItem->m_NameInfo.m_uOnLineCount;
		}
		uCopyPos++;
	}

	//拷贝完成
	bFinish=((uBeginPos+uCopyPos)>=m_NameArray.GetCount());
	return uCopyPos;



}

void CServerGameListManage::UpDateFromRoomPeoList()
{////第二步:mRoomPeoChangeList->CAFCNameItem,CAFCRoomItem
	if(mRoomPeoChangeList.IsEmpty ())
		return;

    for (INT_PTR i=0;i<m_KindArray.GetCount();i++)
	{
		CAFCKindItem *pKindItem = (CAFCKindItem *)m_KindArray.GetAt(i);
		if (NULL == pKindItem)
			continue;

		for (INT_PTR j=0; j<pKindItem->m_ItemPtrArray.GetCount(); j++)
		{
			CAFCNameItem *pNameItem = (CAFCNameItem *)pKindItem->m_ItemPtrArray.GetAt(j);
			pNameItem->m_NameInfo.m_uOnLineCount = 0;

			if (NULL == pNameItem)
				continue;

			for (INT_PTR k=0; k<pNameItem->m_ItemPtrArray.GetCount(); k++)
			{
				CAFCRoomItem * pRoomItem = (CAFCRoomItem *)pNameItem->m_ItemPtrArray.GetAt(k);
				if (NULL == pRoomItem)
					continue;

				DL_GP_RoomListPeoCountStruct* pr = FindRoom(pRoomItem->m_RoomInfo.uRoomID);
				if (NULL == pr)
					continue;
				
				int dd = pr->uOnLineCount - pRoomItem->m_RoomInfo.uPeopleCount ;
				//////////if(dd == 0)continue;
				pRoomItem->m_RoomInfo.uPeopleCount = pr->uOnLineCount;
			//	pNameItem->m_NameInfo.m_uOnLineCount += dd;

				if(pRoomItem->m_RoomInfo.uPeopleCount < 0)
					pRoomItem->m_RoomInfo.uPeopleCount = 0;

				pNameItem->m_NameInfo.m_uOnLineCount += pRoomItem->m_RoomInfo.uPeopleCount;

				if(pNameItem->m_NameInfo.m_uOnLineCount < 0)
					pNameItem->m_NameInfo.m_uOnLineCount = 0;

				bool bb=false;
				for(POSITION pos=mNamePeoChangeList.GetHeadPosition (); pos !=NULL;)
				{
					DL_GP_RoomListPeoCountStruct* p = (DL_GP_RoomListPeoCountStruct *)mNamePeoChangeList.GetNext (pos);
					if(p->uID == pNameItem->m_NameInfo.uNameID)
					{
						bb= true;
						p->uOnLineCount = pNameItem->m_NameInfo.m_uOnLineCount;
						//return;
						break;  //yjj 090325  修改统计人数不准确问题
					}
				}
				if(!bb)
				{
					DL_GP_RoomListPeoCountStruct* p = new DL_GP_RoomListPeoCountStruct;
					p->uOnLineCount = pNameItem->m_NameInfo.m_uOnLineCount;
					p->uID = pNameItem->m_NameInfo.uNameID ;
					mNamePeoChangeList.AddTail (p);
				}
			}
		}
	}
}

//获取在线人数 
INT_PTR CServerGameListManage::FillNameOnLineCount(char * pOnLineBuffer, INT_PTR uBufferSize, INT_PTR uBeginPos, bool & bFinish)
{
	INT_PTR uCopyPos=0;

	while(!mNamePeoChangeList.IsEmpty ())////for(POSITION pos=mRoomPeoChangeList.GetHeadPosition ();pos !=NULL;)//!mRoomPeoChangeList.IsEmpty ())
	{
		if ((INT_PTR)((uCopyPos+1)*sizeof(DL_GP_RoomListPeoCountStruct))>=uBufferSize)break;
		DL_GP_RoomListPeoCountStruct* p=(DL_GP_RoomListPeoCountStruct *)mNamePeoChangeList.RemoveHead ();//.GetNext (pos);
		if (p!=NULL) 
		{
			for (INT_PTR i=0;i<m_KindArray.GetCount();i++)
			{
				CAFCKindItem *pKindItem = (CAFCKindItem *)m_KindArray.GetAt(i);
				if (NULL == pKindItem)
					continue;

				bool bb = false;

				for (INT_PTR j=0; j<pKindItem->m_ItemPtrArray.GetCount(); j++)
				{
					CAFCNameItem *pNameItem = (CAFCNameItem *)pKindItem->m_ItemPtrArray.GetAt(j);
					p->uOnLineCount = 0;

					if (NULL == pNameItem)
						continue;

					if (pNameItem->m_NameInfo.uNameID == p->uID)
					{
						for (INT_PTR k=0; k<pNameItem->m_ItemPtrArray.GetCount(); k++)
						{
							CAFCRoomItem * pRoomItem = (CAFCRoomItem *)pNameItem->m_ItemPtrArray.GetAt(k);
							if (NULL == pRoomItem)
								continue;
							
							for (INT_PTR l=k+1; l<pNameItem->m_ItemPtrArray.GetCount(); l++) ///< 删除相同房间ID
							{
								CAFCRoomItem * pRoomItemSame = (CAFCRoomItem *)pNameItem->m_ItemPtrArray.GetAt(l);
								if (pRoomItemSame->m_RoomInfo.uRoomID == pRoomItem->m_RoomInfo.uRoomID)
								{
									p->uOnLineCount -= pRoomItem->m_RoomInfo.uPeopleCount;
									break;
								}
							}

							if (pRoomItem->m_RoomInfo.uNameID == p->uID)
							{
								p->uOnLineCount += pRoomItem->m_RoomInfo.uPeopleCount;
							}
						}
						bb = true;
						break;
					}
					
				}
				if (bb)
					break;
			}
			
			DL_GP_RoomListPeoCountStruct * pOnLineBuf=(DL_GP_RoomListPeoCountStruct *)(pOnLineBuffer+uCopyPos*sizeof(DL_GP_RoomListPeoCountStruct));
			pOnLineBuf->uID=p->uID;
			pOnLineBuf->uOnLineCount=p->uOnLineCount;

			uCopyPos++;
			delete p;
		}
		
	}
	bFinish=mRoomPeoChangeList.IsEmpty ();//((uBeginPos+uCopyPos)>=mRoomPeoChangeList.GetCount ());
	
	return uCopyPos;

}

//获取在线人数 
INT_PTR CServerGameListManage::FillRoomOnLineCount(char * pOnLineBuffer, INT_PTR uBufferSize, INT_PTR uBeginPos, bool & bFinish)
{
	INT_PTR uCopyPos=0;

	while(!mRoomPeoChangeList.IsEmpty ())////for(POSITION pos=mRoomPeoChangeList.GetHeadPosition ();pos !=NULL;)//!mRoomPeoChangeList.IsEmpty ())
	{
		if ((INT_PTR)((uCopyPos+1)*sizeof(DL_GP_RoomListPeoCountStruct))>=uBufferSize)break;
		DL_GP_RoomListPeoCountStruct* p=(DL_GP_RoomListPeoCountStruct *)mRoomPeoChangeList.RemoveHead ();//.GetNext (pos);
		if (p!=NULL) 
		{
			DL_GP_RoomListPeoCountStruct * pOnLineBuf=(DL_GP_RoomListPeoCountStruct *)(pOnLineBuffer+uCopyPos*sizeof(DL_GP_RoomListPeoCountStruct));
			pOnLineBuf->uID=p->uID;
			pOnLineBuf->uOnLineCount=p->uOnLineCount;

			uCopyPos++;
			delete p;
		}
		
	}
	bFinish=mRoomPeoChangeList.IsEmpty ();//((uBeginPos+uCopyPos)>=mRoomPeoChangeList.GetCount ());
	return uCopyPos;
}
//更新游戏列表
bool CServerGameListManage::UpdateRoomListPeoCount(HANDLE hDatabase)
{////第一步:数据库->mRoomPeoChangeList,m_pRoomPtr
	
	//try
	//{
		ClearList();
		//读取游戏类型列表

		HANDLE hRecord=sqlSPSetName(hDatabase,"SP_GetRoomOnlineUserCount");
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

		DWORD dwReadRoomCount=sqlGetRecordCount(hDatabase,hRecord);

		bool change = false;

		//读取数据库,房间和在线人数
		DL_GP_RoomListPeoCountStruct RoomRead;
		while(!sqlEndOfFile(hDatabase,hRecord))
		{
			bool bb=false;
			sqlGetValue(hDatabase,hRecord,"RoomID",RoomRead.uID);
			sqlGetValue(hDatabase,hRecord,"OnLineCount",RoomRead.uOnLineCount);
			
			sqlGetValue(hDatabase,hRecord,"VirtualUser",RoomRead.uVirtualUser);//20081211 , Fred Huang
			RoomRead.uOnLineCount=RoomRead.uOnLineCount+RoomRead.uVirtualUser;	//20081211 , Fred Huang

			for (UINT j=0;j<m_dwRoomCount;j++)//dwReadRoomCount;j++)
			{
				ComRoomInfo* p=m_pRoomPtr+j;
				if(!p) continue;
				if(p && p->uRoomID == RoomRead.uID)
				{
					int dd =  RoomRead.uOnLineCount - p->uPeopleCount;

					if(dd != 0)
					{
						change = true;
						AddRoomList(RoomRead);
					}
				    p->uPeopleCount =RoomRead.uOnLineCount;
					break;
				}
			}
			sqlMoveNext(hDatabase,hRecord);
		}
		sqlSPClose(hDatabase,hRecord);
		
		UpDateFromRoomPeoList();

		return change;

	return false;
}
