
#include "Stdafx.h"
#include "UserListCtrl.h"
#include "GameRoomEx.h"	//为排队机添加

BEGIN_MESSAGE_MAP(CUserListCtrl, CHappyGoDlg)
	ON_WM_TIMER()
END_MESSAGE_MAP()

//是否字符串
UINT CUserListCtrl::m_uItemKind[15]=
{
	IK_NAME,			//用户名
	IK_USERID,			//用户ID
	IK_DESKNO,			//桌号
	IK_MONEY,			//金币
	IK_WEALTH_LEVEL,	//财富等级
	IK_POINT,			//积分
	IK_LEVEL,			//积分等级
	IK_CHARM,			//魅力
	IK_CHARM_LEVEL,		//魅力等级
	IK_SEX,				//性别
	IK_ALL_COUNT,		//总局数
	IK_WINCOUNT,		//胜局
	IK_LOSTCOUNT,		//输局
	IK_MIDCOUNT,		//和局
	IK_CUT_PERCENT		//逃跑率
};

//构造函数
CUserListCtrl::CUserListCtrl() : CHappyGoDlg(m_uItemKind)
{
	m_uVirtualUserCount=0;
	LoadColumnConfig();
}

//析构函数
CUserListCtrl::~CUserListCtrl()
{
}
static int VirtualList[BUFSIZ]={0};

const TCHAR COLOMN_NAME[][16] = 
{
	"昵称,160", "ID,40", "桌号,40", "金币,60", "金币级别,90", "积分,60", "积分级别,90", "魅力,60", "魅力级别,90", "性别,40", "总局数,50", "胜,50", "输,50", "和,50", "逃跑,50"
};
///
///加载列设置
///
///@param void
///
void CUserListCtrl::LoadColumnConfig()
{
	CString strColumnName;
	CString strColumnIndex;
	CString strColumnIndexAll;
	CString strConfig;
	BOOL    bConfigExist;
	
	CString strDefaultColumnIndexAll = "Nickname,UserID,DeskNo,Coin,WealthLevel,Score,ScoreLevel,Charm,CharmLevel,Sex,GameCount,WinGame,LoseGame,Peace,Escape,";
	bConfigExist = TRUE;

	CBcfFile fUserList(CBcfFile::GetAppPath()+"UserInforList.bcf");
	strColumnIndexAll = fUserList.GetKeyVal("InforList","_ColumnOrder","");
	if (strColumnIndexAll.IsEmpty())
	{
		bConfigExist = FALSE;
		strColumnIndexAll = strDefaultColumnIndexAll;
	}

	int iIndex = 0;
	int iWidth = 0;
	int nColCount = 0;
	int nHeadPos = 0;
	int nTailPos = strColumnIndexAll.Find(',', nHeadPos);
	while (nTailPos > nHeadPos)
	{
		ColumnStruct ColStr;

		strColumnIndex = strColumnIndexAll.Mid(nHeadPos, nTailPos-nHeadPos);
		if (bConfigExist)
			strConfig = fUserList.GetKeyVal("InforList",strColumnIndex,"");
		else
			strConfig = COLOMN_NAME[iIndex];

		if (!strConfig.IsEmpty())
		{
			DecodeString(strConfig,strColumnName,iWidth);
			ColStr.index = iIndex;
			ColStr.Width = iWidth;
			strncpy(ColStr.ColumnName,strColumnName,MAX_PATH/2);
			m_MapColumn.insert(pair<string,ColumnStruct>(strColumnIndex.GetBuffer(),ColStr));
			iIndex++;
		}
		
		nHeadPos = nTailPos + 1;
		nTailPos = strColumnIndexAll.Find(',', nHeadPos);
	}
}

int GetModuleStatus(char* module,char* compoment,int defval/* = 0*/);

///
///将字符串解析出 顺序号，列名，列宽
///
///@param [in]str 输入值字符串，[out]strColumn 返回值 列名，[out]width 返回值 列宽
///
///@return void
///
void CUserListCtrl::DecodeString(CString str, CString &strColumn, int &width)
{
	int pos;
	CString strTemp;
	
	strTemp = str;
	pos = strTemp.Find(',');
	if (pos >= 0)
	{
		strColumn = strTemp.Left(pos);

		strTemp = strTemp.Right(strTemp.GetLength()-pos-1);
		if (!strTemp.IsEmpty())
		{
			width = atoi(strTemp);
		}
	}	
}


///
///初始化函数
///
///@param [in]pOrderFunction 函数地址， [in]GameInfo 用户信息
///
///@return void
///
void CUserListCtrl::InitListCtrl(GetOrderName * pOrderFunction, UINT uComType,GameInfoStruct *GameInfo)
{
	CHappyGoDlg::InitListCtrl(pOrderFunction,uComType);
	
	m_uComType = uComType;
	pGameInfo  = GameInfo;

	MapColumn::iterator *pIte;
	pIte = new MapColumn::iterator[m_MapColumn.size()];

	MapColumn::iterator ite;
	for (ite=m_MapColumn.begin(); ite!=m_MapColumn.end(); ite++)
	{
		pIte[ite->second.index] = ite;
	}

	int iCnt = m_MapColumn.size();
	for (int i=0; i<iCnt; i++)
	{
		InsertColumn(pIte[i]->second.index,pIte[i]->second.ColumnName,LVCFMT_CENTER,pIte[i]->second.Width);
	}
	delete [] pIte;

	SetRedraw(FALSE);
	return;
}

//是否允许查看 IP
void CUserListCtrl::EnableIPView(bool bLookIP)
{
	if(bLookIP)
	{
		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
		InsertColumn(16,fMsg.GetKeyVal("UserListCtrl","UserIP","用户IP"),LVCFMT_LEFT,120);
	}
	else if(!bLookIP)
	{
		DeleteColumn(16);
	}
	return;
}

///
///获取用户插入的位置
///
///@param pUserItem 用户信息
///
///@return int 插入的位置
///
int CUserListCtrl::GetInsertPos(UserItemStruct * pUserItem)
{
	int iItemCnt;
	int iCnt = 1;
	UserItemStruct *pUser = NULL;
	iItemCnt= GetItemCount();

	if (0 == iItemCnt)
		return 0;
	while (iCnt<iItemCnt)
	{
		pUser = (UserItemStruct *)GetItemData(iCnt++);
		if (NULL != pUser)
		{
			if (pUserItem->GameUserInfo.bMember > pUser->GameUserInfo.bMember)
			{
				iCnt--;
				return iCnt;
			}
			else if (pUserItem->GameUserInfo.bMember == pUser->GameUserInfo.bMember) ///< 同等级VIP，按照钱数排名
			{
				if (pUserItem->GameUserInfo.i64Money >= pUser->GameUserInfo.i64Money)
				{
					iCnt--;
					return iCnt;
				}
			}
		}
	}
	return iCnt;
}

///
///增加用户
///
///@param pUserItem 用户信息
///
///@return bool
///
bool CUserListCtrl::AddGameUser(UserItemStruct * pUserItem)
{
	if (NULL == pUserItem)
		return false;

	int iItem;
	LVFINDINFO FindInfo;
	::memset(&FindInfo,0,sizeof(FindInfo));
	FindInfo.flags  = LVFI_PARAM;
	FindInfo.lParam = (LPARAM)pUserItem;
	iItem = FindItem(&FindInfo);///< 查找用户

	if (-1 != iItem) 
	{
		UpdateGameUser(pUserItem);
		return true;
	}

	PrepareDate(pUserItem); ///< 将数据填充到MAP中
	//CGameRoomEx* pGameRoomEx = GetGameRoom();
	CGameRoomEx* pGameRoomEx = GetGameRoom(pGameInfo->uRoomID);
	if (!pGameRoomEx)
	{
		return false;
	}

	bool bNotMySelf = (pGameRoomEx->m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID
		!= pUserItem->GameUserInfo.dwUserID);
	if (bNotMySelf)
	{
		iItem = InsertItem(GetInsertPos(pUserItem),NULL,0); ///< 排序
	}
	else
	{
		iItem = InsertItem(0,NULL,0); ///< 排序
	}

	MapColumn::iterator ite;
	for (ite=m_MapColumn.begin(); ite!= m_MapColumn.end(); ite++)
	{
		if (-1 != ite->second.index)///< 隐藏的列顺序号为-1
		{
			SetItemText(iItem,ite->second.index,ite->second.Value);
		}
	}
	///< 设置扩展信息
	SetItemData(iItem,(DWORD_PTR)pUserItem);

	return true;
}

///
///更新状态
///
///@param pUserItem 用户信息
///
///@return bool
///
bool CUserListCtrl::UpdateGameUser(UserItemStruct * pUserItem)
{
	if (pUserItem==NULL && this->m_hWnd == NULL)
		return false;

	int iItem;
	LVFINDINFO FindInfo;

	::memset(&FindInfo,0,sizeof(FindInfo));
	FindInfo.flags  = LVFI_PARAM;
	FindInfo.lParam = (LPARAM)pUserItem;
	iItem = FindItem(&FindInfo);///< 查找用户
	if (iItem==-1)
		return false;

	PrepareDate(pUserItem); ///< 将数据填充到MAP中

	MapColumn::iterator ite;
	for (ite=m_MapColumn.begin(); ite!= m_MapColumn.end(); ite++)
	{
		if (-1 != ite->second.index)///< 隐藏的列顺序号为-1
		{
			SetItem(iItem,ite->second.index,1,ite->second.Value,0,0,0,0);
		}
	}

	return true;
} 

/// 获取玩家ID信息，是否散户，特别账号，指派账号
/// @param UserID 玩家ID,NomalIDFrom散户ID开始，NomalIDEnd散户ID结束
/// @return 0到10是指派账号后面的0的个数，-1为散户，-2为无效账号，-5为5连号，-6为6连号
int CUserListCtrl::GetIDInformation(long UserID,long NomalIDFrom,long NomalIDEnd)
{
	char BufID[12]     = {0};
	int CntZero        = 0;   ///< 0的个数
	int len            = 0;
	BOOL sameNumID     = TRUE;  ///< 是否连号ID，11111 444444。自己的ID号

	if (UserID>NomalIDFrom && UserID<NomalIDEnd) ///< 散户
	{
		return -1;
	}

	if (UserID < NomalIDFrom)
	{
		sprintf(BufID,"%ld",UserID);

		for (int i=0; 0!=BufID[i]; i++)
		{
			if ('0'==BufID[i])
			{
				CntZero++;
			}
			else if ('0'!=BufID[i] && 0!=CntZero) ///< 无效账号
			{
				return -2;
			}
			
			if (i>0 && BufID[0]!=BufID[i] && sameNumID)
			{
				sameNumID = FALSE;
			}
			len++;
		}

		if (sameNumID) ///< 是连号ID
		{
			if (len >= 5)
				return -len;
			else
				return -2; ///< 无效账号
		}

		char BufIDFrom[12] = {0};
		sprintf(BufIDFrom,"%ld",NomalIDFrom);
		if (strlen(BufIDFrom) == len)
		{
			if ('4' == BufID[len-CntZero-1]) ///< 副职
			{
				return CntZero+1; ///< 4在倒数第CntZero+1位
			}
			return CntZero;
		}
		else
			return -2; ///< 无效账号
	}

	return -2; ///< 无效账号
}

///
///准备要填充的数据,此数据放在MAP中
///
///@param pUserItem 用户信息
///
///@return void
///
void CUserListCtrl::PrepareDate(UserItemStruct *pUserItem)
{
	CString strBuffer;
	CString strTemp;
	MapColumn::iterator ite;

	UserInfoStruct * pUserInfo = &pUserItem->GameUserInfo;
	UINT uAllCount   = pUserInfo->uWinCount + pUserInfo->uLostCount + pUserInfo->uMidCount;
	UINT uCutPercent = (pUserInfo->uCutCount>0)?(UINT)((double)(pUserInfo->uCutCount*100)/(double)uAllCount):0;
	int dwPoint = 0;
	int	dwFascination = 0;
    __int64 i64Money = 0;
	i64Money       = pUserInfo->i64Money;
	dwPoint       = pUserInfo->dwPoint;
	dwFascination = pUserInfo->dwFascination; 

	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");

	if (pGameInfo)
	{
		///< 为排队机添加
		//CGameRoomEx* pGameRoomEx = GetGameRoom();

		CGameRoomEx* pGameRoomEx = GetGameRoom(pGameInfo->uRoomID);
		if (!pGameRoomEx)
		{
			return;
		}

		//if(((pGameInfo->dwRoomRule & GRR_AUTO_SIT_DESK) || ((pGameInfo->dwRoomRule & GRR_QUEUE_GAME) > 0))
		//	&& (pGameRoomEx->m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID != pUserInfo->dwUserID)
		//	)
		if ((pGameInfo->dwRoomRule & GRR_NOTCHEAT) && pGameRoomEx->m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID != pUserInfo->dwUserID)
		{
			strBuffer = fMsg.GetKeyVal("UserListCtrl","Player","玩家"); ///< 玩家昵称 隐藏
			strTemp   = "???"; ///< 玩家ID 隐藏
		}
		else
		{
			strBuffer = pUserInfo->nickName; ///< 玩家昵称 显示
			if (pUserInfo->dwUserID >= 0)
			strTemp.Format(TEXT("%d"),pUserInfo->dwUserID); ///< 玩家ID 显示
		}
		
		ite = m_MapColumn.find("Nickname"); ///< 玩家昵称
		if (m_MapColumn.end() != ite)
		{
			strncpy(ite->second.Value,strBuffer,32);
		}

		ite = m_MapColumn.find("UserID"); ///< 玩家ID
		if (m_MapColumn.end() != ite)
		{
			strncpy(ite->second.Value,strTemp,32);
		}
		///< end of 为排队机添加
	}
	else
	{
		strBuffer = pUserInfo->nickName; ///< 玩家昵称 显示
		if (pUserInfo->dwUserID >= 0)
			strTemp.Format(TEXT("%d"),pUserInfo->dwUserID); ///< 玩家ID 显示

		ite = m_MapColumn.find("Nickname"); ///< 玩家昵称
		if (m_MapColumn.end() != ite)
		{
			strncpy(ite->second.Value,strBuffer,32);
		}

		ite = m_MapColumn.find("UserID"); ///< 玩家ID
		if (m_MapColumn.end() != ite)
		{
			strncpy(ite->second.Value,strTemp,32);
		}
	}

	if (1&Glb().m_CenterServerPara.m_nFunction) ///< 是否金葫芦2代功能
	{
		ite = m_MapColumn.find("VIPLevel"); ///< VIP等级
		if (m_MapColumn.end() != ite)
		{
			int iVIP = GetIDInformation(pUserInfo->dwUserID,
				Glb().m_CenterServerPara.m_lNomalIDFrom,
				Glb().m_CenterServerPara.m_lNomalIDEnd); ///< 根据ID查找VIP等级
			if (iVIP>=0 && iVIP<7)
			{
				strBuffer.Format("VIPLevel%d",iVIP);
				strBuffer = fMsg.GetKeyVal("UserListCtrl",strBuffer,strBuffer);
			}
			else if(iVIP>-9 && iVIP<-4) ///< 5连号到8连号
			{
				strBuffer.Format("VIPLevel%d",-iVIP-2);
				strBuffer = fMsg.GetKeyVal("UserListCtrl",strBuffer,strBuffer);
			}
			else
			{
				strBuffer.Empty();
			}
			if (strBuffer.IsEmpty())
				ite->second.Value[0] = 0;
			else
			strncpy(ite->second.Value,strBuffer,32);
		}
	}

	//桌号
	if (pUserInfo->bDeskNO!=255 && m_uComType!=TY_MATCH_GAME)
	{
        // PengJiLin, 2010-5-19, 排队机不显示桌号。2010-9-16: 增加判断避免出错
        //if((NULL != pGameInfo) && ((pGameInfo->dwRoomRule & GRR_AUTO_SIT_DESK) || ((pGameInfo->dwRoomRule & GRR_QUEUE_GAME) > 0)))
		if (pGameInfo->dwRoomRule & GRR_NOTCHEAT)
            strBuffer = "???";
        else
		    strBuffer.Format(TEXT("%d"),pUserInfo->bDeskNO+1);
	}
	else
		strBuffer = "";

	ite = m_MapColumn.find("DeskNo"); ///< 桌号
	if (m_MapColumn.end() != ite)
	{
		strncpy(ite->second.Value,strBuffer,32);
	}
		
	//CGameRoomEx* pGameRoomEx = GetGameRoom();

	CGameRoomEx* pGameRoomEx = GetGameRoom(pGameInfo->uRoomID);
	if (!pGameRoomEx)
	{
		return;
	}

	//金币
	TCHAR szNum[128];
	GetNumString(szNum, i64Money, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
	strBuffer.Format(TEXT("%s"),szNum);
    //if((NULL != pGameInfo) && ((pGameInfo->dwRoomRule & GRR_AUTO_SIT_DESK) || ((pGameInfo->dwRoomRule & GRR_QUEUE_GAME) > 0)) && (pGameRoomEx->m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID != pUserInfo->dwUserID))
	
	if (((pGameInfo->dwRoomRule & GRR_NOTCHEAT) && pGameRoomEx->m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID != pUserInfo->dwUserID) || pGameInfo->dwRoomRule & GRR_CONTEST)
	{
        strBuffer = "???";
	}
	ite = m_MapColumn.find("Coin"); ///< 金币
	if (m_MapColumn.end() != ite)
	{
		strncpy(ite->second.Value,strBuffer,32);
	}

	//财富级别
	strBuffer = GetMoneyOrderName(i64Money,pGameInfo->uNameID);
	ite = m_MapColumn.find("WealthLevel"); ///< 财富级别
	if (m_MapColumn.end() != ite)
	{
		strncpy(ite->second.Value,strBuffer,32);
	}

	///< 积分
	strBuffer.Format(TEXT("%d"),dwPoint-GetNormalOrderBasePoint(dwPoint));
	if (((pGameInfo->dwRoomRule & GRR_NOTCHEAT) && pGameRoomEx->m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID != pUserInfo->dwUserID) || pGameInfo->dwRoomRule & GRR_CONTEST)
	{
        strBuffer = "???";
	}
	ite = m_MapColumn.find("Score"); ///< 积分
	if (m_MapColumn.end() != ite)
	{
		strncpy(ite->second.Value,strBuffer,32);
	}

	///< 积分级别
	strBuffer = GetPointOrderName(dwPoint,pGameInfo->uNameID);
	ite = m_MapColumn.find("ScoreLevel"); ///< 积分级别
	if (m_MapColumn.end() != ite)
	{
		strncpy(ite->second.Value,strBuffer,32);
	}

	///< 魅力值
	strBuffer.Format(TEXT("%d"),dwFascination);
	ite = m_MapColumn.find("Charm"); ///< 魅力值
	if (m_MapColumn.end() != ite)
	{
		strncpy(ite->second.Value,strBuffer,32);
	}

	///< 魅力等级
	strBuffer.Format(TEXT("%s"),GetCharmGrade(dwFascination));
	ite = m_MapColumn.find("CharmLevel"); ///< 魅力等级
	if (m_MapColumn.end() != ite)
	{
		strncpy(ite->second.Value,strBuffer,32);
	}

	///< 性别
	strBuffer = pUserItem->GameUserInfo.bBoy?fMsg.GetKeyVal("UserListCtrl","Male","男"):fMsg.GetKeyVal("UserListCtrl","Female","女");
	ite = m_MapColumn.find("Sex"); ///< 性别
	if (m_MapColumn.end() != ite)
	{
		strncpy(ite->second.Value,strBuffer,32);
	}

	//总局数
	strBuffer.Format(TEXT("%ld"),uAllCount);
	ite = m_MapColumn.find("GameCount"); ///< 总局数
	if (m_MapColumn.end() != ite)
	{
		strncpy(ite->second.Value,strBuffer,32);
	}

	///< 胜局
	strBuffer.Format(TEXT("%ld"),pUserInfo->uWinCount);
	ite = m_MapColumn.find("WinGame"); ///< 胜局
	if (m_MapColumn.end() != ite)
	{
		strncpy(ite->second.Value,strBuffer,32);
	}

	///< 输局
	strBuffer.Format(TEXT("%d"),pUserInfo->uLostCount);
	ite = m_MapColumn.find("LoseGame"); ///< 输局
	if (m_MapColumn.end() != ite)
	{
		strncpy(ite->second.Value,strBuffer,32);
	}

	///< 和局
	strBuffer.Format(TEXT("%d"),pUserInfo->uMidCount);
	ite = m_MapColumn.find("Peace"); ///< 和局
	if (m_MapColumn.end() != ite)
	{
		strncpy(ite->second.Value,strBuffer,32);
	}

	///< 断线率
	if (uCutPercent>0) 
		strBuffer.Format(TEXT("%2ld%%"),uCutPercent);
	else
		strBuffer.Empty();
	
	ite = m_MapColumn.find("Escape"); ///< 断线率
	if (m_MapColumn.end() != ite)
	{
		strncpy(ite->second.Value,strBuffer,32);
	}
}

//删除用户
bool CUserListCtrl::DeleteGameUser(UserItemStruct * pUserItem)
{
	if (pUserItem==NULL) return false;

	//查找用户
	LVFINDINFO FindInfo;
	::memset(&FindInfo,0,sizeof(FindInfo));
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pUserItem;
	int iItem=FindItem(&FindInfo);
	if (iItem==-1) return false;

	//删除用户
	DeleteItem(iItem);
	return true;
}
//换肤
void CUserListCtrl::ChangeSkin()
{
	//设置信息
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + strSkin);
	CString key=TEXT("UserList");
	CString skinfolder = f.GetKeyVal(m_skinmgr.GetKeyVal(strSkin),"skinfolder",m_skinmgr.GetSkinPath());;
	int r = 0,g = 0,b = 0;
	r=f.GetKeyVal(key,"rlisttextclr",255);
	g=f.GetKeyVal(key,"rlisttextclg",255);
	b=f.GetKeyVal(key,"rlisttextclb",255);
	COLORREF textcr=RGB(r,g,b);

	r=f.GetKeyVal(key,"rlistbkclr",233);
	g=f.GetKeyVal(key,"rlistbkclg",233);
	b=f.GetKeyVal(key,"rlistbkclb",233);
	COLORREF bkcr=RGB(r,g,b);

	r=f.GetKeyVal(key,"rflisttextclr",255);
	g=f.GetKeyVal(key,"rflisttextclg",255);
	b=f.GetKeyVal(key,"rflisttextclb",255);
	COLORREF ftextcr=RGB(r,g,b);

	r=f.GetKeyVal(key,"rflistbkclr",233);
	g=f.GetKeyVal(key,"rflistbkclg",233);
	b=f.GetKeyVal(key,"rflistbkclb",233);
	COLORREF fbkcr=RGB(r,g,b);
	SetColor(bkcr,textcr,fbkcr,ftextcr);

	//列表头
	r=f.GetKeyVal(key,"rlistheadr",0);
	g=f.GetKeyVal(key,"rlistheadg",59);
	b=f.GetKeyVal(key,"rlistheadb",108);
	COLORREF color=RGB(r,g,b);

	TCHAR path[MAX_PATH],path1[MAX_PATH];
	wsprintf(path,"%sroom_right\\list_head1.bmp",skinfolder);
	wsprintf(path1,"%sroom_right\\list_head2.bmp",skinfolder);
	m_ListHeader.SetBkImage(path,path1,color,color,1);

	SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_ONECLICKACTIVATE|LVS_EX_INFOTIP);
	SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,LVS_EX_FULLROWSELECT,LVS_EX_FULLROWSELECT);

	//设置图片
	//if (m_UserStateList.GetSafeHandle()==NULL)
	{
		//装载位图
		BITMAP BmpInfo;
		CGameImage	UserStateBmp;
		//CBitmap NewUserStateBmp;
		AfxSetResourceHandle(GetModuleHandle(FACE_DLL_NAME));
		UserStateBmp.SetLoadInfo(".\\image\\user_state.bmp",true);;
		AfxSetResourceHandle(GetModuleHandle(NULL));
		//UserStateBmp.GetBitmap(&BmpInfo);
		BmpInfo.bmWidth=UserStateBmp.GetWidth();
		BmpInfo.bmHeight=UserStateBmp.GetHeight();

		//建立位图
		CDC TargetDC, * pDC=GetDC();
		TargetDC.CreateCompatibleDC(pDC);
		 //{{Modified by Zxd 20100715 这个对象必须作为成员函数才能释放
		if (m_NewUserStateBmp.m_hObject != NULL)
		{
			m_NewUserStateBmp.DeleteObject();
		}
		/// Modified by Zxd 20100715 这个对象必须作为成员函数才能释放}}
		m_NewUserStateBmp.CreateCompatibleBitmap(pDC,BmpInfo.bmWidth,BmpInfo.bmHeight);
		CBitmap * pOldBmp=TargetDC.SelectObject(&m_NewUserStateBmp);
		TargetDC.FillSolidRect(0,0,BmpInfo.bmWidth,BmpInfo.bmHeight,m_bkColor);
		AFCStretchImage(&TargetDC,0,0,BmpInfo.bmWidth,BmpInfo.bmHeight,UserStateBmp,0,0,BmpInfo.bmWidth,BmpInfo.bmHeight,RGB(255,255,255));
		TargetDC.SelectObject(pOldBmp);
		TargetDC.DeleteDC();

		//建立资源列

		//if(m_UserStateList.m_hImageList)
			//m_UserStateList.Detach();
		//m_UserStateList.Create(USER_STATE_WIDTH,USER_STATE_HEIGHT,ILC_COLOR16,0,0);

		//m_UserStateList.Add(&m_NewUserStateBmp,RGB(0,0,255));

		//清理资源
		UserStateBmp.Detach();
		ReleaseDC(pDC);
	}
	//SetImageList(&m_UserStateList,LVSIL_SMALL);

	//InitUserTypeImage();

	//InitUserDiamondTypeImage();//邮游钻石身份标识 add by huangYuanSong 09.07.14
}
void CUserListCtrl::LoadSkin(void)
{
	//设置信息
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + strSkin);
	CString key=TEXT("UserList");
	CString skinfolder = f.GetKeyVal(m_skinmgr.GetKeyVal(strSkin),"skinfolder",m_skinmgr.GetSkinPath());;

	int r,g,b;
	r=f.GetKeyVal(key,"rlisttextclr",255);
	g=f.GetKeyVal(key,"rlisttextclg",255);
	b=f.GetKeyVal(key,"rlisttextclb",255);
	COLORREF textcr=RGB(r,g,b);

	r=f.GetKeyVal(key,"rlistbkclr",233);
	g=f.GetKeyVal(key,"rlistbkclg",233);
	b=f.GetKeyVal(key,"rlistbkclb",233);
	COLORREF bkcr=RGB(r,g,b);

	r=f.GetKeyVal(key,"rflisttextclr",255);
	g=f.GetKeyVal(key,"rflisttextclg",255);
	b=f.GetKeyVal(key,"rflisttextclb",255);
	COLORREF ftextcr=RGB(r,g,b);

	r=f.GetKeyVal(key,"rflistbkclr",233);
	g=f.GetKeyVal(key,"rflistbkclg",233);
	b=f.GetKeyVal(key,"rflistbkclb",233);
	COLORREF fbkcr=RGB(r,g,b);
	SetColor(bkcr,textcr,fbkcr,ftextcr);

	//列表头
	r=f.GetKeyVal(key,"rlistheadr",0);
	g=f.GetKeyVal(key,"rlistheadg",59);
	b=f.GetKeyVal(key,"rlistheadb",108);
	COLORREF color=RGB(r,g,b);
	TCHAR path[MAX_PATH],path1[MAX_PATH];
	wsprintf(path,"%sroom_right\\list_head1.bmp",skinfolder);
	wsprintf(path1,"%sroom_right\\list_head2.bmp",skinfolder);
	m_ListHeader.SetBkImage(path,path1,color,color,1);

	SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_ONECLICKACTIVATE|LVS_EX_INFOTIP);
	SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,LVS_EX_FULLROWSELECT,LVS_EX_FULLROWSELECT);

	//设置图片
	//if (m_UserStateList.GetSafeHandle()==NULL)
	{
		//装载位图
		BITMAP BmpInfo;
		CGameImage	UserStateBmp;
		//CBitmap NewUserStateBmp;
		AfxSetResourceHandle(GetModuleHandle(FACE_DLL_NAME));
		UserStateBmp.SetLoadInfo(".\\image\\user_state.bmp",true);;
		AfxSetResourceHandle(GetModuleHandle(NULL));
		//UserStateBmp.GetBitmap(&BmpInfo);
		BmpInfo.bmWidth=UserStateBmp.GetWidth();
		BmpInfo.bmHeight=UserStateBmp.GetHeight();

		//建立位图
		CDC TargetDC, * pDC=GetDC();
		TargetDC.CreateCompatibleDC(pDC);
		/// {{Modified by Zxd 20100715 这个对象必须作为成员函数才能释放
		if (m_NewUserStateBmp.m_hObject != NULL)
		{
			m_NewUserStateBmp.DeleteObject();
		}
		/// Modified by Zxd 20100715 这个对象必须作为成员函数才能释放}}
		m_NewUserStateBmp.CreateCompatibleBitmap(pDC,BmpInfo.bmWidth,BmpInfo.bmHeight);
		CBitmap * pOldBmp=TargetDC.SelectObject(&m_NewUserStateBmp);
		TargetDC.FillSolidRect(0,0,BmpInfo.bmWidth,BmpInfo.bmHeight,m_bkColor);
		AFCStretchImage(&TargetDC,0,0,BmpInfo.bmWidth,BmpInfo.bmHeight,UserStateBmp,0,0,BmpInfo.bmWidth,BmpInfo.bmHeight,RGB(255,255,255));
		TargetDC.SelectObject(pOldBmp);
		TargetDC.DeleteDC();

		//建立资源列

		if(m_UserStateList.m_hImageList)
			m_UserStateList.Detach();
		m_UserStateList.Create(USER_STATE_WIDTH,USER_STATE_HEIGHT,ILC_COLOR16,0,0);
		
		m_UserStateList.Add(&m_NewUserStateBmp,RGB(0,0,255));

		//清理资源
		UserStateBmp.Detach();
		ReleaseDC(pDC);
	}
	SetImageList(&m_UserStateList,LVSIL_SMALL);

	InitUserTypeImage();

	InitUserDiamondTypeImage();//邮游钻石身份标识 add by huangYuanSong 09.07.14
}

/// 填充虚拟玩家
///	@param uVirtualUserCount虚拟玩家人数
///	@return void
void CUserListCtrl::fillVirtualUser(int uVirtualUserCount)
{
	//return;///< 不添加虚拟玩家

	//wushuqun 2009.6.6
	//实时增加虚拟玩家
	m_uVirtualUserCount += uVirtualUserCount;

	if(uVirtualUserCount < 1)
		return;

	DWORD dwName = Glb().m_hRandVirUser;
	int uCount = 0;
	if(dwName)
		uCount = cfgGetValue(dwName,"USERNAME","COUNT",0);

	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	CString strMessage;

	CString str[7]=
	{  "深圳","东莞","广州","中山","汕头","珠海","茂名" };
	//str[0] = fMsg.GetKeyVal("UserListCtrl","City1","深圳");
	//str[1] = fMsg.GetKeyVal("UserListCtrl","City2","东莞");
	//str[2] = fMsg.GetKeyVal("UserListCtrl","City3","广州");
	//str[3] = fMsg.GetKeyVal("UserListCtrl","City4","中山");
	//str[4] = fMsg.GetKeyVal("UserListCtrl","City5","汕头");
	//str[5] = fMsg.GetKeyVal("UserListCtrl","City6","珠海");
	//str[6] = fMsg.GetKeyVal("UserListCtrl","City7","茂名");

	int nCityCnt = fMsg.GetKeyVal("UserListCtrl","CityCount",0);
	
	Sleep(20);///< 保证随机种子不一样 wushuqun 2009.6.15
	srand((unsigned)GetTickCount());	//随机设置数据

	/*用户形象更换方式 
	0-用户使用金币购买方式 
	1-用户拥有金币决定 
	2-用户拥有金币或积分决定*/
	int nChangeMethod = GetModuleStatus("Avatar","ChangeMethod",0);

	for(int c=0; c<uVirtualUserCount; c++)
	{
		UserItemStruct * pUserItem = new UserItemStruct;
		ZeroMemory(pUserItem,sizeof(UserItemStruct));

		//定义变量
		CString strBuffer;
		UserInfoStruct * pUserInfo = &pUserItem->GameUserInfo;

		if (0 == nCityCnt)
		{
			strMessage = fMsg.GetKeyVal("UserListCtrl","Province","广东省");
			wsprintf(pUserInfo->szProvince,strMessage.GetBuffer());
			int nCityIndex = rand() % 7;
			wsprintf(pUserInfo->szCity,str[nCityIndex]);
		}
		else
		{
			int nCityIndex = rand()%nCityCnt;
			strMessage.Format(_T("City%d"),nCityIndex);
			strMessage = fMsg.GetKeyVal("UserListCtrl",strMessage,_T("广东省,深圳"));
			wsprintf(pUserInfo->szProvince,strMessage.Left(strMessage.Find(',')));
			wsprintf(pUserInfo->szCity,strMessage.Right(strMessage.GetLength()-strMessage.Find(',')-1));
			
		}

		pUserInfo->uWinCount=rand()/(32767/300);
		pUserInfo->uLostCount=rand()/(32767/200);
		pUserInfo->uCutCount=rand()/(32767/30);
		pUserInfo->uMidCount=rand()/(32767/100);
		if (NULL != msgWnd)
			pUserInfo->i64Money = rand()/2+((CGameRoomEx*)msgWnd)->m_GameInfo.uLessPoint;
		else
			pUserInfo->i64Money = rand()/2+1000;

		pUserInfo->bUserState = USER_NO_STATE;       ///< 玩家状态

		if(uCount>0)
		{
			CString s;
			s.Format("NAME%d",rand()%uCount);
			wsprintf(pUserInfo->nickName,"%s",cfgGetValue(dwName,"USERNAME",s,"玩家"));
		}
		else
			wsprintf(pUserInfo->nickName,"Player%d",c+10000);

		pUserInfo->dwUserID			 = -1000-c -1; ///< 玩家ID
		pUserInfo->isVirtual = 1;
		pUserInfo->dwPoint = rand()-16383;	//随机设置数据
		pUserInfo->dwFascination=rand()/(32767/25);	//随机设置数据
		pUserInfo->bDeskNO = 0xFF;  	//设置初始数据
		pUserItem->GameUserInfo.bBoy = rand()%2;  ///< 性别
	
		if (nChangeMethod == 0) //用户使用金币购买方式 
		{
			pUserInfo->bLogoID	= rand()%60+1; ///< 头像ID

			if(pUserItem->GameUserInfo.bBoy)
			{
				if(pUserInfo->bLogoID > 5)
					pUserInfo->bLogoID -= 6;
			}
			else
			{
				if(pUserInfo->bLogoID < 6)
					pUserInfo->bLogoID += 6;
			}
		}
		else if (nChangeMethod == 1) //用户拥有金币决定 
		{
			//虚拟玩家的头像由金币决定
			pUserInfo->bLogoID = rand()%60+1;
		}
		else if (nChangeMethod == 2) //用户拥有金币或积分决定
		{
			//虚拟玩家的头像由金币决定
			pUserInfo->bLogoID = rand()%60+1;
		}
		AddGameUser(pUserItem);

		if(msgWnd)
			SetTimer(c+1000,rand()*(270000/32767),NULL);
	}

	BYTE iSend=0,iStation=0,iTemp=0;
	srand((unsigned)time(NULL));

	do
	{
		iStation=rand() % uVirtualUserCount;
		iTemp=VirtualList[iSend];
		VirtualList[iSend]=VirtualList[iStation];
		VirtualList[iStation]=iTemp;
		iSend++;
	}
	while (iSend<uVirtualUserCount);

}


//////////////////////////////////////////////////////////////////////////
//函数 fillVirtualUser 用于添加虚拟人物
void CUserListCtrl::OnTimer(UINT nIDEvent)
{
	if(nIDEvent>=1000 && msgWnd)
	{
		KillTimer(nIDEvent);
	//	DWORD_PTR dw=GetItemData(nIDEvent-1000+1);
		DWORD_PTR dw = (DWORD_PTR)GetIdelVirtualUser();
		if(dw)
		{
		//	UserItemStruct * pUserItem=(UserItemStruct * )dw;
			msgWnd->PostMessage(WM_USER+5,10,(LPARAM)dw);
			SetTimer(nIDEvent,rand()*(270000/32767),NULL);
		}

	}
	__super::OnTimer(nIDEvent);

}

//////////////////////////////////////////////////////////////////////////
////20081211 , Fred Huang
void CUserListCtrl::SetMSGReceiveWnd(CWnd * pWnd)
{
	msgWnd=pWnd;
}

///
/// 查找空闲的虚拟玩家
///
///@param void
///
///@return UserItemStruct 虚拟玩家信息
///
UserItemStruct * CUserListCtrl::GetIdelVirtualUser(void)
{
	int iCnt;
	UserItemStruct * pUserItem = NULL;
	iCnt = GetItemCount();

	for(int i=0; i<((iCnt+1)>>1); i++)
	{
		pUserItem = (UserItemStruct * )GetItemData(i);
		if(pUserItem &&
			pUserItem->GameUserInfo.dwUserID<0 &&
			pUserItem->GameUserInfo.bUserState!=USER_PLAY_GAME &&
			1==pUserItem->GameUserInfo.isVirtual)
			break;
		else
			pUserItem = NULL;

		pUserItem = (UserItemStruct * )GetItemData(iCnt-i-1);
		if(pUserItem &&
			pUserItem->GameUserInfo.dwUserID<0 &&
			pUserItem->GameUserInfo.bUserState!=USER_PLAY_GAME &&
			1==pUserItem->GameUserInfo.isVirtual)
			break;
		else
			pUserItem = NULL;
	}

	return pUserItem;
}

///
/// 查找指定虚拟玩家
///
///@param long dwUserID 需要查找的用户ID
///
///@return UserItemStruct 玩家信息
///
UserItemStruct * CUserListCtrl::GetUserFromID(long dwUserID)
{
	int iCnt;
	UserItemStruct * pUserItem = NULL;
	iCnt = GetItemCount();

	for(int i=0; i<((iCnt+1)>>1); i++)
	{
		pUserItem = (UserItemStruct * )GetItemData(i);
		if(pUserItem && pUserItem->GameUserInfo.dwUserID == dwUserID)
			break;
		else
			pUserItem = NULL;

		pUserItem = (UserItemStruct * )GetItemData(iCnt-i-1);
		if(pUserItem && pUserItem->GameUserInfo.dwUserID == dwUserID)
			break;
		else
			pUserItem = NULL;
	}

	return pUserItem;
}

int GetModuleStatus(char* module,char* compoment,int defval/* = 0*/)
{
	//本地路径
	DWORD dwCfgTalk = ::cfgOpenFile("Module.bcf",_T("szbzw!!@@##"));

	if ( dwCfgTalk >= 0x10)
	{
		int retVal = ::cfgGetValue(dwCfgTalk,module,compoment,defval); 
		::cfgClose(dwCfgTalk);
		return retVal;

	}

	return 0;
}
