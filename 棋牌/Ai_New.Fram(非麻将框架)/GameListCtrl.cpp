#include "StdAfx.h"
//#include "GamePlace.h"
#include "GameListCtrl.h"
//#include "PlaceResource.h"
//#include "..\\..\\消息文件\\GamePlaceMessage.h"

BEGIN_MESSAGE_MAP(CGameListCtrl, CTreeCtrl)
END_MESSAGE_MAP()

#define SET_LINE_COLOR          (TV_FIRST+40)		//设置颜色消息						
#define LS_TIME_OUT				60L		//列表超时数									

//树控件指针
CGameListCtrl					* CAFCGameListItem::m_pTreeCtrl=NULL;
void CGameListCtrl::IEnterARoomOrLeave(UINT uRoomID,bool bEnter)//进入或离开一个房间。计算各个子服务器的在线人数和名称
{
	CAFCRoomItem * pGameRoomItem=FindRoomItem(uRoomID);//获得房间的列表的指针
	if(!pGameRoomItem)return;
	int dd=0;
	if(bEnter)dd=1;//pGameRoomItem->m_RoomInfo.uPeopleCount ++;
	else dd=-1;//pGameRoomItem->m_RoomInfo.uPeopleCount --;

	pGameRoomItem->m_RoomInfo.uPeopleCount+=dd;
	
	if(pGameRoomItem->m_RoomInfo.uPeopleCount  < 0)pGameRoomItem->m_RoomInfo.uPeopleCount= 0;
	
	{
		TCHAR szBuffer[100];
		//各个服务器的在线人数和名称
		wsprintf(szBuffer,TEXT("(%3d)%s"),pGameRoomItem->m_RoomInfo.uPeopleCount,pGameRoomItem->m_RoomInfo.szGameRoomName);
		SetItemText(pGameRoomItem->m_hCurrentItem,szBuffer);
	}
	CAFCNameItem *p=FindNameItem(pGameRoomItem->m_RoomInfo.uRoomID);
	if(p)
	{
		p->m_NameInfo.m_uOnLineCount += dd;
		if(p->m_NameInfo.m_uOnLineCount < 0)p->m_NameInfo.m_uOnLineCount =0;
		///p->UpdateOnLineCount();
		UpdateOnLineCount();
		//ExpandItem(2);
	}
}

//更新总在线人数
bool CGameListCtrl::UpdateOnLineCount()
{
	//统计人数
	UINT uOnLineCount=0L;
	CAFCGameListItem * pGameKindItem=GetKindParentItem();//获取根项
	for (INT_PTR i=0;i<pGameKindItem->m_ItemPtrArray.GetCount();i++)
	{
		CAFCKindItem * pKindItem=(CAFCKindItem *)pGameKindItem->m_ItemPtrArray.GetAt(i);
		if (pKindItem==NULL)continue;
		UINT kd=0;
		for (INT_PTR j=0;j<pKindItem->m_ItemPtrArray.GetCount();j++)
		{
			CAFCNameItem * pNameItem=(CAFCNameItem *)pKindItem->m_ItemPtrArray.GetAt(j);
			if (pNameItem!=NULL)
			{
				if(pNameItem->m_NameInfo.m_uOnLineCount < 0)pNameItem->m_NameInfo.m_uOnLineCount=0;
				pNameItem->UpdateOnLineCount();
				///uOnLineCount += pNameItem->m_uOnLineCount;
				kd += pNameItem->m_NameInfo.m_uOnLineCount;
			}
		}

		uOnLineCount +=kd;
		if(kd<0)
			kd=0;
		{
			TCHAR szBuffer[100];
			wsprintf(szBuffer,TEXT("(%3d)%s"),kd,pKindItem->m_KindInfo.szKindName);//m_RoomInfo.szGameRoomName);
			SetItemText(pKindItem->m_hCurrentItem,szBuffer);
		}
	}
	
	
	//更新显示
	//TCHAR szBuffer[100];
	//wsprintf(szBuffer,TEXT(Glb().m_Tree[0]),uOnLineCount);//第一个树。多少人在线
	//SetItemText(pGameKindItem->m_hCurrentItem,szBuffer);

return true;
}

/*
//处理数据
void CGameListCtrl::UpdateAllOnLineData()
{
	
	//统计人数
	UINT uOnLineCount=0L;
	CAFCGameListItem * pGameKindItem=GetKindParentItem();
	for (INT_PTR i=0;i<pGameKindItem->m_ItemPtrArray.GetCount();i++)
	{
		CAFCKindItem * pKindItem=(CAFCKindItem *)pGameKindItem->m_ItemPtrArray.GetAt(i);
		if (pKindItem==NULL)continue;
		UINT kd=0;
		for (INT_PTR j=0;j<pKindItem->m_ItemPtrArray.GetCount();j++)
		{
			CAFCNameItem * pNameItem=(CAFCNameItem *)pKindItem->m_ItemPtrArray.GetAt(j);
			if (pNameItem!=NULL)
			{
				///uOnLineCount += pNameItem->m_uOnLineCount;
				kd += uOnLineCount;
			}
		}

		uOnLineCount +=kd;

			
		{
			TCHAR szBuffer[100];
			wsprintf(szBuffer,TEXT("(%3d) %s"),kd,pKindItem->m_KindInfo.szKindName);//m_RoomInfo.szGameRoomName);
			SetItemText(pKindItem->m_hCurrentItem,szBuffer);
		}
	}
	
	
	//更新显示
	TCHAR szBuffer[100];
	wsprintf(szBuffer,TEXT("游戏广场 ( 当前在线人数 %d 人 )"),uOnLineCount);
	SetItemText(pGameKindItem->m_hCurrentItem,szBuffer);


}
*/

//处理数据
bool CGameListCtrl::HandleNameOnLineData(DL_GP_RoomListPeoCountStruct * pOnLineCountPtr, UINT uCount)
{
	
	for (UINT i=0;i<uCount;i++) 
	{
		CAFCNameItem * pNameItem=FindNameItem((pOnLineCountPtr+i)->uID);
		if (pNameItem!=NULL)
		{
			pNameItem->m_NameInfo.m_uOnLineCount=(pOnLineCountPtr+i)->uOnLineCount;
            pNameItem->UpdateOnLineCount();
			pNameItem->ExpandItem (1);
		}
	}
    UpdateOnLineCount();


	return true;
}

CAFCNameItem *CGameListCtrl::FindNameItem(UINT uID)
{
	//统计人数
	UINT uOnLineCount=0L;
	CAFCGameListItem * pGameKindItem=GetKindParentItem();//获取根项
	for (INT_PTR i=0;i<pGameKindItem->m_ItemPtrArray.GetCount();i++)
	{
		CAFCKindItem * pKindItem=(CAFCKindItem *)pGameKindItem->m_ItemPtrArray.GetAt(i);
		if (pKindItem!=NULL)
		{
			for (INT_PTR j=0;j<pKindItem->m_ItemPtrArray.GetCount();j++)
			{
				CAFCNameItem * pNameItem=(CAFCNameItem *)pKindItem->m_ItemPtrArray.GetAt(j);
				if (pNameItem!=NULL)
				{
					if(pNameItem->m_NameInfo.uNameID == uID)return pNameItem;
				}
			}
		}
	}
	return NULL;
}

//处理数据
bool CGameListCtrl::HandleIOnLineData(MSG_GP_SR_OnLineStruct * pOnLineCountPtr, UINT uCount)
{
	for (UINT i=0;i<uCount;i++) 
	{
//		CAFCNameItem * pNameItem=FindNameItem(FindKindItem((pOnLineCountPtr+i)->uKindID),(pOnLineCountPtr+i)->uNameID);
		//if (pNameItem!=NULL) pNameItem->m_uOnLineCount=(pOnLineCountPtr+i)->uOnLineCount;
	}
	return true;
}

//展开数据
bool CGameListCtrl::ExpandItemFirstRoom()
{
	CAFCGameListItem * pGameKindItem=GetKindParentItem();//获取根项

	for (INT_PTR i=0;i<pGameKindItem->m_ItemPtrArray.GetCount();i++)
	{
		CAFCKindItem * pKindItem=(CAFCKindItem *)pGameKindItem->m_ItemPtrArray.GetAt(i);
		if (pKindItem!=NULL)
		{
			for (INT_PTR j=0;j<pKindItem->m_ItemPtrArray.GetCount();j++)
			{
				CAFCNameItem * pNameItem=(CAFCNameItem *)pKindItem->m_ItemPtrArray.GetAt(j);
				if (pNameItem!=NULL)
				{
					pNameItem->ExpandItem (4);
					return true;
				}
			}
		}
	}
	return true;
}

/*******************************************************************************************************/
CAFCRoomItem *CGameListCtrl::FindRoomItem(UINT uRoomID)
{
	//统计人数
	UINT uOnLineCount=0L;
	CAFCGameListItem * pGameKindItem=GetKindParentItem();//获取根项
	for (INT_PTR i=0;i<pGameKindItem->m_ItemPtrArray.GetCount();i++)
	{
		CAFCKindItem * pKindItem=(CAFCKindItem *)pGameKindItem->m_ItemPtrArray.GetAt(i);
		if (pKindItem!=NULL)
		{
			for (INT_PTR j=0;j<pKindItem->m_ItemPtrArray.GetCount();j++)
			{
				CAFCNameItem * pNameItem=(CAFCNameItem *)pKindItem->m_ItemPtrArray.GetAt(j);
				if (pNameItem!=NULL)
				{
					for (INT_PTR rr=0;rr<pNameItem->m_ItemPtrArray.GetCount();rr++)
					{
						CAFCRoomItem * proomItem=(CAFCRoomItem *)pNameItem->m_ItemPtrArray.GetAt (rr);
						if(proomItem->m_RoomInfo.uRoomID == uRoomID)
						{							
							return proomItem;
						}
						else if (proomItem->m_bIsContest)
						{//比赛场
							for (int i=0; i!=proomItem->m_RoomContest.size(); ++i)
							{
								ComRoomInfo oContestInfo = proomItem->m_RoomContest[i];
								
								if (oContestInfo.uRoomID == uRoomID && uRoomID != 0)
								{
									return proomItem;
								}
							}
						}
					}
				}
			}
		}
	}
	return NULL;
}

//查找房间子项
CAFCRoomItem * CGameListCtrl::FindRoomItemByContest(CAFCNameItem * pNameItem, ComRoomInfo* pRoomInfo)
{
	if (pNameItem!=NULL)
	{
		if (pRoomInfo->dwRoomRule & GRR_CONTEST)
		{
			for (INT_PTR i=0;i<pNameItem->m_ItemPtrArray.GetCount();i++)
			{
				CAFCRoomItem * pRoomItem=(CAFCRoomItem *)pNameItem->m_ItemPtrArray.GetAt(i);
				if (pRoomItem != NULL && pRoomItem->m_bIsContest && pRoomInfo->iContestID == pRoomItem->m_RoomInfo.iContestID)
				{
					return pRoomItem;
				}
			}
		}
	}
	return NULL;
}


//查找比赛房间子项
ComRoomInfo * CGameListCtrl::FindContestRoomItem(CAFCRoomItem * pRoomItem, int iRoomID)
{
	if (pRoomItem != NULL && pRoomItem->m_bIsContest)
	{
		for (int i = 0; i < pRoomItem->m_RoomContest.size(); i++)
		{
			if (pRoomItem->m_RoomContest[i].uRoomID == iRoomID)
				return &(pRoomItem->m_RoomContest[i]);
		}
	}
	return NULL;
}

//处理数据
bool CGameListCtrl::HandleRoomOnLineData(DL_GP_RoomListPeoCountStruct * pOnLineCountPtr, UINT uCount)
{
	
	for (UINT i=0;i<uCount;i++) 
	{
		CAFCRoomItem * pRoomItem=FindRoomItem((pOnLineCountPtr+i)->uID);
		if (pRoomItem!=NULL)
		{
			pRoomItem->m_RoomInfo.uPeopleCount=(pOnLineCountPtr+i)->uOnLineCount;

			{
				TCHAR szBuffer[100];
				if(pRoomItem->m_RoomInfo.uPeopleCount<0)
					pRoomItem->m_RoomInfo.uPeopleCount=0;
				wsprintf(szBuffer,TEXT("(%3d)%s"),pRoomItem->m_RoomInfo.uPeopleCount,pRoomItem->m_RoomInfo.szGameRoomName);
				SetItemText(pRoomItem->m_hCurrentItem,szBuffer);
			}
		}
	}
	return true;
}
//析构函数
CAFCGameListItem::CAFCGameListItem()
{
	m_uUpdateTime=0;
	m_hCurrentItem=NULL;
	m_pParentItem=NULL;
	m_uDataType=GLK_NO_DATA;
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

//初始化项
bool CAFCGameListItem::Init(CAFCGameListItem * pParentItem)
{
	m_pParentItem=pParentItem;
	return true;
}

//在树上插入一个叶子接点数据
HTREEITEM CAFCGameListItem::AddTreeData(TCHAR * szText, UINT uIconIndex, LPARAM lParam)
{
	TV_INSERTSTRUCT InsertInf;
	memset(&InsertInf,0,sizeof(InsertInf));
	InsertInf.item.cchTextMax=100;
	InsertInf.hParent=m_hCurrentItem;///////父接点!!!!!!!!!!!!!!!!!!
	InsertInf.hInsertAfter=TVI_LAST;
	InsertInf.item.iImage=uIconIndex;
	InsertInf.item.iSelectedImage=uIconIndex;
	InsertInf.item.pszText=szText;
	InsertInf.item.lParam=lParam;
	InsertInf.item.mask=TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVIF_TEXT|TVIF_PARAM;
	return m_pTreeCtrl->InsertItem(&InsertInf);
}

//展开项数据
bool CAFCGameListItem::ExpandItem(int iExpendCount)
{
	if ((iExpendCount==-1)||(iExpendCount>0))//如果项序号等于-1或者项序号大于0（即项序号不等于0）
	{
		if (m_hCurrentItem!=NULL) m_pTreeCtrl->Expand(m_hCurrentItem,TVE_EXPAND);//如果当前项不为空，那么展开当前项
		if ((iExpendCount>1)||(iExpendCount==-1))//如果项序号等于-1或者项序号大于1（即项序号不等于1，不等于0）
		{
			if (iExpendCount>0) iExpendCount--;//如果项序号大于0，那么项序号减1
			for (int i=0;i<m_ItemPtrArray.GetCount();i++)//子项数组总数
			{
				CAFCGameListItem * pListItem=(CAFCGameListItem *)m_ItemPtrArray.GetAt(i);//////////得到 游戏列表子项
				if (pListItem!=NULL) pListItem->ExpandItem(iExpendCount);//如果游戏列表子项不为空，那么展开游戏列表子项
			}
		}
		return true;
	}
	return false;
}
//加入从服务器获取的类型
bool CAFCGameListItem::UpdateGetListItem()//////////更新列表AddTreeData
{
	//if (m_uDataType == GLK_GAME_NAME)/////////////如果数据类型是游戏名字
	//{
	//	if (m_ItemPtrArray.GetCount()==0)////无子项
	//	{
	//		TCHAR szBuffer[100];
	//		if ((m_uUpdateTime+LS_TIME_OUT) <= (LONG)time(NULL)) 
	//			wsprintf(szBuffer,TEXT("读取信息..."));//如果更新时间+列表超时数小于当前时间，那么就读取信息
	//		else 
	//			wsprintf(szBuffer,TEXT("没有房间"));//否则就算超时，显示没有实例
	//		TCHAR sz[200];
	//		wsprintf(sz,"%d,%d",m_uUpdateTime,(LONG)time(NULL));
	//		//Writelog(sz);
	//		if (m_hUpdateItem==NULL) 
	//			m_hUpdateItem=AddTreeData(szBuffer,IND_NO_ROOM,NULL);///如果获取索引等于空，那么获取索引=增加项，显示//没有房间的图标
	//		else
	//			m_pTreeCtrl->SetItemText(m_hUpdateItem,szBuffer);//树控件指针设置显示文字
	//	}
	//	else if (m_hUpdateItem!=NULL) ///有子项
	//	{
	//		for (INT_PTR i=0;i<m_ItemPtrArray.GetCount();i++)//循环所有子项
	//		{
	//			CAFCRoomItem * pRoomItem=(CAFCRoomItem *)m_ItemPtrArray.GetAt(i);///游戏房间子项等于树子项
	//			pRoomItem->m_uUpdateTime=(LONG)time(NULL);//更新时间=当前时间
	//		}
	//		m_pTreeCtrl->DeleteItem(m_hUpdateItem);//删除获取索引
	//		m_hUpdateItem=NULL;////////获取索引为空
	//	}
	//}
	return true;
}
//是否需要更新 
bool CAFCGameListItem::IsNeedUpdate()//是否需要更新
{
	if ((m_uDataType==GLK_GAME_NAME)&&((m_uUpdateTime+LS_TIME_OUT)<=time(NULL))) return true;////////如果数据类型是游戏名字，并且更新时间+列表超时数小于当前时间
	return false;
}
//删除过时房间
bool CAFCGameListItem::DeleteOutTimeItem()
{
	if (m_uDataType==GLK_GAME_NAME)//如果数据类型是游戏名字
	{
		for (INT_PTR i=0;i<m_ItemPtrArray.GetCount();i++)//////////循环所有子项
		{
			CAFCRoomItem * pRoomItem=(CAFCRoomItem *)m_ItemPtrArray.GetAt(i);//游戏房间子项等于树子项
			if ((pRoomItem!=NULL)&&((pRoomItem->m_uUpdateTime+LS_TIME_OUT)<=(LONG)time(NULL)))//如果游戏房间子项不为空，并且且更新时间+列表超时数小于当前时间
			{
				m_ItemPtrArray.RemoveAt(i--);///////删除子项数组
				m_pTreeCtrl->DeleteItem(pRoomItem->m_hCurrentItem);/////删除树子项
				delete pRoomItem;////删除游戏房间子项
			}
		}
		UpdateGetListItem();/////更新列表
	}
	return true;
}
//加入游戏类型AddTreeData
bool CAFCGameListItem::AddGameKind(ComKindInfo * pGameKind)
{
	try
	{
		CAFCKindItem * pGameKindItem=new CAFCKindItem;//新游戏类型子项
		if (pGameKindItem!=NULL)/////////如果游戏类型子项不为空
		{
			//设置数据

			pGameKindItem->Init(this);////
			pGameKindItem->m_KindInfo=*pGameKind;/////////类型数据

			m_ItemPtrArray.Add(pGameKindItem);////增加子项
			pGameKindItem->m_hCurrentItem=AddTreeData(pGameKindItem->m_KindInfo.szKindName,0,(LPARAM)pGameKindItem);///////加入游戏类型AddTreeData
			return true;
		}
	}
	catch (...) {}
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
			pGameNameItem->Init(this);
			pGameNameItem->m_NameInfo=*pGameName;
			m_ItemPtrArray.Add(pGameNameItem);
			pGameNameItem->m_hCurrentItem=AddTreeData(pGameNameItem->m_NameInfo.szGameName,0,(LPARAM)pGameNameItem);
			return true;
		}
	}
	catch (...) {}
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
			pGameRoomItem->Init(this);
			pGameRoomItem->m_RoomInfo=*pGameRoom;
			pGameRoomItem->m_uUpdateTime=(LONG)time(NULL);
			m_ItemPtrArray.Add(pGameRoomItem);
			TCHAR szBuffer[100];
			wsprintf(szBuffer,TEXT("(%3d)%s"),pGameRoom->uPeopleCount,pGameRoom->szGameRoomName);///显示文字
			pGameRoomItem->m_hCurrentItem=AddTreeData(szBuffer,0,(LPARAM)pGameRoomItem);
			return true;
		}
	}
	catch (...) {}
	return false;
}

//加入比赛房间
bool CAFCGameListItem::AddContestItem(ComRoomInfo * pGameRoom)
{
	try
	{
		CAFCRoomItem * pGameRoomItem=new CAFCRoomItem;
		if (pGameRoomItem!=NULL)
		{
			//设置数据		
			pGameRoomItem->Init(this);
			pGameRoomItem->m_RoomInfo=*pGameRoom;
			pGameRoomItem->m_uUpdateTime=(LONG)time(NULL);
			m_ItemPtrArray.Add(pGameRoomItem);
			TCHAR szBuffer[100];
			memset(szBuffer,0,sizeof(szBuffer));

			pGameRoomItem->m_bIsContest = true;
			pGameRoomItem->m_RoomContest.push_back(*pGameRoom);
			
			wsprintf(szBuffer,TEXT("(%3d)%s"),pGameRoom->uPeopleCount,pGameRoom->szGameRoomName);///显示文字

			pGameRoomItem->m_hCurrentItem=AddTreeData(szBuffer,0,(LPARAM)pGameRoomItem);

			return true;
		}
	}
	catch (...) {}
	return false;
}

//加入内部数据
bool CAFCGameListItem::AddInsideData(GameInsideInfo * pInsideData, UINT uIconIndex)
{
	try
	{
		CAFCInsideItem * pInsideItem=new CAFCInsideItem;
		if (pInsideItem!=NULL)
		{
			//设置数据
			pInsideItem->Init(this);
			pInsideItem->m_InsideInfo=*pInsideData;
			m_ItemPtrArray.Add(pInsideItem);
			pInsideItem->m_hCurrentItem=AddTreeData(pInsideItem->m_InsideInfo.szDisplayName,uIconIndex,(LPARAM)pInsideItem);
			return true;
		}
	}
	catch (...) {}
	return false;
}
	
/*******************************************************************************************************/

//设置单个游戏的总在线人数
bool CAFCNameItem::UpdateOnLineCount()
{
	TCHAR szBuffer[100];
	if(m_NameInfo.m_uOnLineCount<0)
		m_NameInfo.m_uOnLineCount=0;
	wsprintf(szBuffer,TEXT("%s(%d)"),m_NameInfo.szGameName,m_NameInfo.m_uOnLineCount);
	m_pTreeCtrl->SetItemText(m_hCurrentItem,szBuffer);
	return true;
}

/*******************************************************************************************************/

//构造函数
CGameListCtrl::CGameListCtrl()
{
}

//析构函数
CGameListCtrl::~CGameListCtrl()
{
	for (int i=0;i<m_InsideArray.GetCount();i++)
	{
		delete (CAFCGameListItem *)m_InsideArray.GetAt(i);
		m_InsideArray.SetAt(i,NULL);
	}
	return;
}

//初始化函数,同时增加根接点
bool CGameListCtrl::Init()
{
	////设置游戏列表数据
	//UINT uIconIndex[]=
	//{
	//	IND_LIST_ROOT,IND_ROOM_NO_ICON,IND_LIST_ROOT,IND_LIST_NO_INSTALL,
	//	IND_LIST_ROOT,IND_LIST_ROOT,IND_LIST_ROOT,
	//};
	//在树上插入根数据
	CAFCGameListItem::m_pTreeCtrl=this;
	//for (int i=0;i<Glb().m_TreeCount;i++)
	//{	
		GameInsideInfo InsideInfo;
		InsideInfo.Head.uSize=sizeof(InsideInfo);
		InsideInfo.Head.bDataType=GLK_GAME_USE;
		InsideInfo.uTrunID = 1000;
		lstrcpy(InsideInfo.szDisplayName,"root");
		if (!AddRootItem(&InsideInfo,100))//uIconIndex[0])) 
			return false;
	//}
	return true;
}
//插入根项
bool CGameListCtrl::AddRootItem(GameInsideInfo * pInsideData, UINT uIconIndex)
{
	try
	{
		CAFCInsideItem * pInsideItem=new CAFCInsideItem();
		if (pInsideItem!=NULL)
		{
			//设置数据
			pInsideItem->Init(NULL);
			pInsideItem->m_InsideInfo=*pInsideData;
			m_InsideArray.Add(pInsideItem);

			//插入数据
			TV_INSERTSTRUCT InsertInf;
			memset(&InsertInf,0,sizeof(InsertInf));
			InsertInf.item.cchTextMax=100;
			InsertInf.hInsertAfter=TVI_LAST;
			InsertInf.item.mask=TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVIF_TEXT|TVIF_PARAM;
			InsertInf.item.iImage=uIconIndex;
			InsertInf.item.iSelectedImage=uIconIndex;//选择根图标
			InsertInf.item.pszText=pInsideData->szDisplayName;
			InsertInf.item.lParam=(LPARAM)pInsideItem;
			pInsideItem->m_hCurrentItem=InsertItem(&InsertInf);
			return true;
		}
	}
	catch (...) {}
	return false;
}


//处理数据//在树上加入游戏类型AddTreeData
bool CGameListCtrl::HandleKindData(ComKindInfo * pKindInfoPtr, UINT uKindCount)
{
	CAFCGameListItem * pGameKindItem=GetKindParentItem();//获取根项
	for (UINT i=0;i<uKindCount;i++) 
	{
		if (FindKindItem((pKindInfoPtr+i)->uKindID)==NULL)
			pGameKindItem->AddGameKind(pKindInfoPtr+i);//在树上加入游戏类型AddTreeData
	}
	return true;
}
//处理数据/////在树上加入游戏名字
bool CGameListCtrl::HandleNameData(ComNameInfo * pNameInfoPtr, UINT uNameCount)
{
	CAFCGameListItem * pGameKindItem=GetKindParentItem();//获取根项
	for (UINT i=0;i<uNameCount;i++)
	{
		CAFCKindItem * pKindItem=FindKindItem((pNameInfoPtr+i)->uKindID);
		if ((pKindItem!=NULL)&&(FindNameItem(pKindItem,(pNameInfoPtr+i)->uNameID)==NULL))
			pKindItem->AddGameName(pNameInfoPtr+i);///////在树上加入游戏名字
	}
	return true;
}
//处理数据
bool CGameListCtrl::HandleRoomData(ComRoomInfo * pRoomInfoPtr, UINT uRoomCount, UINT uKindID, UINT uNameID)
{
	CAFCNameItem * pNameItem=FindNameItem(FindKindItem(uKindID),uNameID);///查找游戏名称
	if (pNameItem!=NULL)
	{
		CAFCRoomItem * pRoomItem = NULL;
		for (UINT i=0;i<uRoomCount;i++)
		{
			if ((pRoomInfoPtr+i)->dwRoomRule & GRR_CONTEST)
			{//机器人进入万人房
				pRoomItem = FindRoomItemByContest(pNameItem, pRoomInfoPtr+i);
				if (pRoomItem != NULL)
				{
					ComRoomInfo* _p = FindContestRoomItem(pRoomItem, (pRoomInfoPtr+i)->uRoomID);
					if (_p != NULL)
					{
						pRoomItem->m_uUpdateTime=(LONG)time(NULL);
						lstrcpy(pRoomItem->m_RoomInfo.szGameRoomName,(pRoomInfoPtr+i)->szGameRoomName);
					}
					else
					{
						pRoomItem->m_RoomContest.push_back(*(pRoomInfoPtr+i));
					}
				}
				else
				{
					pNameItem->AddContestItem(pRoomInfoPtr+i);
				}
			}
			else
			{
				pRoomItem=FindRoomItem(pNameItem,(pRoomInfoPtr+i)->uRoomID);/////////在这个游戏名称下面再查找房间ID

				if (pRoomItem!=NULL)
				{
					TCHAR szBuffer[100];
					pRoomItem->m_uUpdateTime=(LONG)time(NULL);
					lstrcpy(pRoomItem->m_RoomInfo.szGameRoomName,(pRoomInfoPtr+i)->szGameRoomName);
					wsprintf(szBuffer,TEXT("(%3d)%s"),(pRoomInfoPtr+i)->uPeopleCount,(pRoomInfoPtr+i)->szGameRoomName);
					SetItemText(pRoomItem->m_hCurrentItem,szBuffer);
				}
				else pNameItem->AddGameRoom(pRoomInfoPtr+i);////////在这个游戏名称下面增加查找房间ID
			}			
		}
	}
	return true;
}
//查找类型子项
CAFCKindItem * CGameListCtrl::FindKindItem(UINT uKindID)
{
	CAFCGameListItem * pGameKindItem=GetKindParentItem();//获取根项
	for (INT_PTR i=0;i<pGameKindItem->m_ItemPtrArray.GetCount();i++)
	{
		CAFCKindItem * pKindItem=(CAFCKindItem *)pGameKindItem->m_ItemPtrArray.GetAt(i);
		if ((pKindItem!=NULL)&&(pKindItem->m_KindInfo.uKindID==uKindID)) return pKindItem;
	}
	return NULL;
}

//查找名字子项
CAFCNameItem * CGameListCtrl::FindNameItem(CAFCKindItem * pKindItem, UINT uNameID)
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

//查找房间子项
CAFCRoomItem * CGameListCtrl::FindRoomItem(CAFCNameItem * pNameItem, UINT uRoomID)
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
//插入获取数据
bool CGameListCtrl::FillGetListItem()
{
	CAFCGameListItem * pGameKindItem=GetKindParentItem();//获取根项
	for (INT_PTR i=0;i<pGameKindItem->m_ItemPtrArray.GetCount();i++)
	{
		CAFCKindItem * pKindItem=(CAFCKindItem *)pGameKindItem->m_ItemPtrArray.GetAt(i);
		if (pKindItem!=NULL) 
		{
			for (INT_PTR j=0;j<pKindItem->m_ItemPtrArray.GetCount();j++)
			{
				CAFCNameItem * pNameItem=(CAFCNameItem *)pKindItem->m_ItemPtrArray.GetAt(j);
				if (pNameItem!=NULL) 
				{
					pNameItem->m_uUpdateTime=0;
					pNameItem->UpdateGetListItem();
				}
			}
		}			
	}
	return true;
}
//删除过时房间 
bool CGameListCtrl::DeleteOutTimeRoom(UINT uKindID, UINT uNameID)
{
	CAFCNameItem * pNameItem=FindNameItem(FindKindItem(uKindID),uNameID);
	if (pNameItem!=NULL) pNameItem->DeleteOutTimeItem();
	return true;
}
//获取进程名字
bool CGameListCtrl::GetProcessName(CAFCRoomItem * pGameRoomItem, TCHAR * szProcessName, UINT uBufferSize)
{
	if ((pGameRoomItem->m_pParentItem!=NULL)&&(pGameRoomItem->m_pParentItem->m_uDataType==GLK_GAME_NAME))
	{
		CAFCNameItem * pGameName=(CAFCNameItem *)pGameRoomItem->m_pParentItem;
		CopyMemory(szProcessName,pGameName->m_NameInfo.szGameProcess,__min(uBufferSize,sizeof(pGameName->m_NameInfo.szGameProcess)));
		szProcessName[uBufferSize/sizeof(TCHAR)-1]=0;
		return true;
	}
	return false;
}
//获取游戏名字
bool CGameListCtrl::GetGameName(CAFCRoomItem * pGameRoomItem, TCHAR * szGameName, UINT uBufferSize)
{
	if ((pGameRoomItem->m_pParentItem!=NULL)&&(pGameRoomItem->m_pParentItem->m_uDataType==GLK_GAME_NAME))
	{
		CAFCNameItem * pGameName=(CAFCNameItem *)pGameRoomItem->m_pParentItem;
		CopyMemory(szGameName,pGameName->m_NameInfo.szGameName,__min(uBufferSize,sizeof(pGameName->m_NameInfo.szGameName)));
		szGameName[uBufferSize/sizeof(TCHAR)-1]=0;
		return true;
	}
	return false;
}



//展开数据
bool CGameListCtrl::ExpandItem(int iExpendCount)
{
	CAFCGameListItem * pGameKindItem=GetKindParentItem();//获取根项
	pGameKindItem->ExpandItem(iExpendCount);
	return true;
}

void CGameListCtrl::UpdataItem(CAFCGameListItem* pItem)
{
	CAFCNameItem * pNameItem;
	int i;
	for(i=0;i<=pItem->m_pParentItem->m_ItemPtrArray.GetCount();i++)
	{
		pNameItem = (CAFCNameItem *)(pItem->m_pParentItem->m_ItemPtrArray.GetAt(i));
		if(pNameItem->m_hCurrentItem==pItem->m_hCurrentItem)
			break;
	}
	//int selimage=CPlaceResource::GetListIconIndex(&(pNameItem->m_NameInfo));
	//TVITEM item;
	//item.hItem=pItem->m_hCurrentItem;
	//item.mask=TVIF_IMAGE|TVIF_SELECTEDIMAGE;
	//item.iImage=selimage;
	//item.iSelectedImage=selimage;
	//TreeView_SetItem(m_hWnd,&item);
}

void CGameListCtrl::UpdateAllIcon(void)
{
	//int selimage;
	//CAFCGameListItem * pGameKindItem=GetKindParentItem();//获取根项
	//for (INT_PTR i=0;i<pGameKindItem->m_ItemPtrArray.GetCount();i++)
	//{
	//	CAFCKindItem * pKindItem=(CAFCKindItem *)pGameKindItem->m_ItemPtrArray.GetAt(i);
	//	if (pKindItem!=NULL)
	//	{
	//		for (INT_PTR i=0;i<pKindItem->m_ItemPtrArray.GetCount();i++)
	//		{
	//			CAFCNameItem * pNameItem=(CAFCNameItem *)pKindItem->m_ItemPtrArray.GetAt(i);
	//			if (pNameItem!=NULL)
	//			{
	//				selimage=CPlaceResource::GetListIconIndex(&(pNameItem->m_NameInfo));
	//				TVITEM item;
	//				item.hItem=pNameItem->m_hCurrentItem;
	//				item.mask=TVIF_IMAGE|TVIF_SELECTEDIMAGE;
	//				item.iImage=selimage;
	//				item.iSelectedImage=selimage;
	//				TreeView_SetItem(m_hWnd,&item);
	//			}
	//		}
	//	}
	//}
}
void CGameListCtrl::JoinAllItem(void)
{
	CAFCGameListItem * pGameKindItem=GetKindParentItem();//获取根项
	for (INT_PTR i=0;i<pGameKindItem->m_ItemPtrArray.GetCount();i++)
	{
		CAFCKindItem * pKindItem=(CAFCKindItem *)pGameKindItem->m_ItemPtrArray.GetAt(i);
		if (pKindItem!=NULL)
		{
			for (INT_PTR i=0;i<pKindItem->m_ItemPtrArray.GetCount();i++)
			{
				CAFCNameItem * pNameItem=(CAFCNameItem *)pKindItem->m_ItemPtrArray.GetAt(i);
				if (pNameItem!=NULL)
				{
					pNameItem->m_pTreeCtrl->Expand(pNameItem->m_hCurrentItem,TVE_COLLAPSE);
				}
			}
			pKindItem->m_pTreeCtrl->Expand(pKindItem->m_hCurrentItem,TVE_COLLAPSE);
		}
	}
}
bool CGameListCtrl::ExpandAll(bool ExpName)
{
	CAFCGameListItem * pGameKindItem=GetKindParentItem();//获取根项
	for (INT_PTR i=0;i<pGameKindItem->m_ItemPtrArray.GetCount();i++)
	{
		CAFCKindItem * pKindItem=(CAFCKindItem *)pGameKindItem->m_ItemPtrArray.GetAt(i);
		if (pKindItem!=NULL)
		{
			for (INT_PTR i=0;i<pKindItem->m_ItemPtrArray.GetCount()&&ExpName;i++)
			{
				CAFCNameItem * pNameItem=(CAFCNameItem *)pKindItem->m_ItemPtrArray.GetAt(i);
				if (pNameItem!=NULL)
				{
					pNameItem->m_pTreeCtrl->Expand(pNameItem->m_hCurrentItem,TVE_EXPAND);
				}
			}
			pKindItem->m_pTreeCtrl->Expand(pKindItem->m_hCurrentItem,TVE_EXPAND);
		}
	}
	return true;
}
