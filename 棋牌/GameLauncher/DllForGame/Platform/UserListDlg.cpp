//用户列表
#include "../stdafx.h"
#include "UserListDlg.h"
#include "afcdevhead.h"

#include "detours.h"
#include "skinsb.h"

using namespace BzDui;

BEGIN_MESSAGE_MAP(CGameUserListCtrl, CHappyGoDlg)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CPictureHelper, CGameFaceGo)
	ON_WM_SIZE()
	ON_NOTIFY(NM_RCLICK, IDC_USER_LIST, OnNMRclickUserList)
	ON_NOTIFY(NM_CLICK, IDC_USER_LIST, OnNMLclickUserList)
	ON_NOTIFY(NM_DBLCLK, IDC_USER_LIST, OnNMDBclickUserList)
END_MESSAGE_MAP()

#ifndef GET_FILENAME
#define GET_FILENAME(path, FormatStr,skinfolder) \
	wsprintf(path,FormatStr,skinfolder);\
	if (!CBcfFile::IsFileExist(path))\
{\
	wsprintf(path,FormatStr,skinHall);\
}
#endif//GET_FILENAME


extern EXT_CLASS TCHAR *GlbGetNumString(TCHAR str[], __int64 nNum, int nPower, bool bHasSpace, TCHAR* chSpace);

/*******************************************************************************************************/

//是否字符串
UINT CGameUserListCtrl::m_uItemKind[12]=
{
	IK_NAME,			//用户名
	IK_MONEY,			//金币
	IK_WEALTH_LEVEL,	//财富等级
	IK_POINT,			//积分
	IK_LEVEL,			//积分等级
	IK_ALL_COUNT,		//总局数
	IK_WINCOUNT,		//胜局
	IK_LOSTCOUNT,		//输局
	IK_MIDCOUNT,		//和局
	IK_WATCH,			//旁观对象
	IK_CHARM,			//魅力
	IK_CHARM_LEVEL,		//魅力等级
};

//默认列名和列宽度
const TCHAR COLOMN_NAME[][14] = 
{
	"昵称,100", "金币,60","财富级别,90", "积分,60", "积分级别,90", "魅力,60", "魅力级别,90", "性别,40",
	"总局数,50", "胜局数,50", "输局数,50", "和局数,50", "逃跑率,50","旁观对象,100"
};

//构造函数
CGameUserListCtrl::CGameUserListCtrl() : CHappyGoDlg(m_uItemKind)
{
	CString strGameSkinFilename = m_skinmgr.GetGameSkinBcfFileName();
	if (CBcfFile::IsFileExist(CBcfFile::GetAppPath() + "Image\\game\\" + strGameSkinFilename))
	{
		m_iniFile= CBcfFile::GetAppPath() + "Image\\game\\" + strGameSkinFilename;
		m_bMode = TRUE;
	}
	else
	{
		m_iniFile= CBcfFile::GetAppPath() + "..\\image\\Client\\" + m_skinmgr.GetKeyVal(m_skinmgr.GetSkinBcfFileName())+"\\game\\";
		m_iniFile += m_skinmgr.GetGameSkinBcfFileName();
		m_bMode = FALSE;
	}

	//加载列设置
	LoadColumnConfig();
}

//析构函数
CGameUserListCtrl::~CGameUserListCtrl()
{
}

///
///加载列设置
///
///@param void
///
void CGameUserListCtrl::LoadColumnConfig()
{
	CString strColumnName;
	CString strColumnIndex;
	CString strColumnIndexAll;
	CString strConfig;
	BOOL    bConfigExist;

	CString strDefaultColumnIndexAll = "Nickname,Coin,WealthLevel,Score,ScoreLevel,Charm,CharmLevel,Sex,GameCount,WinGame,LoseGame,Peace,Escape,WatchUser,";
	bConfigExist = TRUE;

	CBcfFile fUserList(CBcfFile::GetAppPath () + "..\\GameUserInforList.bcf");
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

///
///获取用户插入的位置
///
///@param pUserItem 用户信息
///
///@return int 插入的位置
///
int CGameUserListCtrl::GetInsertPos(UserItemStruct * pUserItem)
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
///准备要填充的数据,此数据放在MAP中
///
///@param pUserItem 用户信息
///
///@return void
///
void CGameUserListCtrl::PrepareDate(UserItemStruct *pUserItem)
{
	CString strBuffer;
	CString strTemp;
	MapColumn::iterator ite;

	UserInfoStruct * pUserInfo = &pUserItem->GameUserInfo;
	UINT uAllCount   = pUserInfo->uWinCount + pUserInfo->uLostCount + pUserInfo->uMidCount;
	UINT uCutPercent = (pUserInfo->uCutCount>0)?(UINT)((double)(pUserInfo->uCutCount*100)/(double)uAllCount):0;
	int dwPoint,dwFascination;
    __int64 i64Money;
	i64Money       = pUserInfo->i64Money;
	dwPoint       = pUserInfo->dwPoint;
	dwFascination = pUserInfo->dwFascination; 

	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");


	if (pUserInfo->dwUserID >= 0)
		strTemp.Format(TEXT("%d"),pUserInfo->dwUserID); ///< 玩家ID 

	strBuffer = pUserInfo->nickName; ///< 玩家昵称
	ite = m_MapColumn.find("Nickname"); 
	if (m_MapColumn.end() != ite)
	{
		strncpy(ite->second.Value,strBuffer,32);
	}

	ite = m_MapColumn.find("UserID"); ///< 玩家ID
	if (m_MapColumn.end() != ite)
	{
		strncpy(ite->second.Value,strTemp,32);
	}

	//金币
	TCHAR szNum[128];
	GlbGetNumString(szNum, i64Money, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
	strBuffer.Format(TEXT("%s"),szNum);

	ite = m_MapColumn.find("Coin"); ///< 金币
	if (m_MapColumn.end() != ite)
	{
		strncpy(ite->second.Value,strBuffer,32);
	}

	//财富级别
	strBuffer = GetMoneyOrderName(i64Money);
	ite = m_MapColumn.find("WealthLevel"); ///< 财富级别
	if (m_MapColumn.end() != ite)
	{
		strncpy(ite->second.Value,strBuffer,32);
	}

	///< 积分
	strBuffer.Format(TEXT("%d"),dwPoint-GetNormalOrderBasePoint(dwPoint));
	
	////Kylin 20090119 赛币场显示处理   
	if(m_uComType==TY_MATCH_GAME)
		ite = m_MapColumn.find("Score2"); ///< 赛币
	else 
		ite = m_MapColumn.find("Score");  ///< 积分

	if (m_MapColumn.end() != ite)
	{
		strncpy(ite->second.Value,strBuffer,32);
	}

	///< 积分级别
	strBuffer = m_pPointOrderFun(dwPoint);
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

	//< 旁观对象
	ite = m_MapColumn.find("WatchUser"); ///< 旁观对象
	if (m_MapColumn.end() != ite)
	{
		strncpy(ite->second.Value,"",32);
	}

}

void CGameUserListCtrl::DecodeString(CString str, CString &strColumn, int &width)
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



//初始化函数
void CGameUserListCtrl::InitListCtrl(GetOrderName * pOrderFunction, UINT uComType)
{
	CHappyGoDlg::InitListCtrl(pOrderFunction,uComType);
	m_uComType=uComType;

	CHappyGoDlg::InitListCtrl(pOrderFunction,uComType);

	m_uComType = uComType;

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

	SetRedraw(TRUE);

	return;
}

//设置旁观对象
void CGameUserListCtrl::SetWatchUser(UserItemStruct * pUserItem, TCHAR * szWatchName)
{
	if (pUserItem==NULL) return;

	//查找用户
	LVFINDINFO FindInfo;
	memset(&FindInfo,0,sizeof(FindInfo));
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pUserItem;
	int iItem=FindItem(&FindInfo);
	if (iItem==-1) return;

	// PengJiLin, 2010-6-11, 可配置成不同的样式。鸥朴
	//BYTE byType = GetUserListDefineType();
	//int nSubItem = 0;
	//switch(byType)
	//{
	//case 1:     // 鸥朴样式
	//    nSubItem = 4;
	//    break;
	//default:
	//    nSubItem = 13;
	//    break;
	//}

	//旁观对象
	//if(m_uComType!=TY_MONEY_GAME)
	//{
	//	if (szWatchName!=NULL) 
	//		SetItem(iItem,11,1,szWatchName,0,0,0,0);
	//	else 
	//		SetItem(iItem,11,1,TEXT(""),0,0,0,0);
	//}
	//else if(m_uComType==TY_MONEY_GAME)
	//{

	MapColumn::iterator ite;

	ite = m_MapColumn.find("WatchUser"); ///< 旁观者
	if (m_MapColumn.end() != ite)
	{
		strncpy(ite->second.Value,szWatchName,32);

		if (szWatchName!=NULL) 
			SetItemText(iItem,ite->second.index,szWatchName);
		else 
			SetItemText(iItem,ite->second.index,"Ok");//TEXT(IDOK)
	}

	//}
	return;
}

//是否允许查看 IP
void CGameUserListCtrl::EnableIPView(bool bLookIP)
{
	//if (bLookIP&&m_uComType!=TY_MONEY_GAME)
	//	InsertColumn(12,"用户IP",LVCFMT_LEFT,120);
	//else 
	if(bLookIP)//&&m_uComType==TY_MONEY_GAME)
		InsertColumn(16,"用户IP",LVCFMT_LEFT,120);
	//else if(!bLookIP&&m_uComType!=TY_MONEY_GAME)
	//	DeleteColumn(12);
	else 
		if(!bLookIP)//&&m_uComType==TY_MONEY_GAME)
			DeleteColumn(16);
	return;
}

//增加用户
bool CGameUserListCtrl::AddGameUser(UserItemStruct * pUserItem, bool bIsMySelf)
{
	//if (pUserItem==NULL) return false;

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

	if (!bIsMySelf)
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

//删除用户
bool CGameUserListCtrl::DeleteGameUser(UserItemStruct * pUserItem)
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

//更新状态
bool CGameUserListCtrl::UpdateGameUser(UserItemStruct * pUserItem)
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

void CGameUserListCtrl::SetIniFile(LPCTSTR path, BOOL bMode)
{
	m_iniFile = path;
	m_bMode = bMode;
}


void CGameUserListCtrl::LoadSkin(void)
{

	//设置信息
	CString s = CINIFile::GetAppPath ();/////本地路径

	CBcfFile f( m_iniFile);

	CString key=TEXT("config");
	CString skinfolder= m_skinmgr.GetSkinPath();
	skinfolder.Format("..\\image\\client\\%s\\",m_skinmgr.GetKeyVal(m_skinmgr.GetSkinBcfFileName()));
	skinfolder=f.GetKeyVal(key,"skinfolder",".\\image\\game\\");
	CString skinHall = CBcfFile::GetAppPath()+"..\\" + m_skinmgr.GetSkinPath();

	CString strGameSkinPath = s + "Image\\game\\" + m_skinmgr.GetGameSkinBcfFileName();
	if (CBcfFile::IsFileExist(strGameSkinPath))
	{ 
		skinfolder = ".\\image\\game\\"; 
	} 
	else 
	{ 
		skinfolder=f.GetKeyVal(key,"skinfolder",m_skinmgr.GetGameSkinPath()); 
		skinfolder=s+"..\\"+skinfolder; 
	} 


	int r,g,b;
	r=f.GetKeyVal(key,"glisttextclr",0);
	g=f.GetKeyVal(key,"glisttextclg",255);
	b=f.GetKeyVal(key,"glisttextclb",255);
	COLORREF textcr=RGB(r,g,b);

	r=f.GetKeyVal(key,"glistbkclr",0);
	g=f.GetKeyVal(key,"glistbkclg",233);
	b=f.GetKeyVal(key,"glistbkclb",233);
	COLORREF bkcr=RGB(r,g,b);
	//m_bkColor = bkcr;
	r=f.GetKeyVal(key,"gflisttextclr",0);
	g=f.GetKeyVal(key,"gflisttextclg",255);
	b=f.GetKeyVal(key,"gflisttextclb",255);
	COLORREF ftextcr=RGB(r,g,b);

	r=f.GetKeyVal(key,"gflistbkclr",0);
	g=f.GetKeyVal(key,"gflistbkclg",233);
	b=f.GetKeyVal(key,"gflistbkclb",233);
	COLORREF fbkcr=RGB(r,g,b);
	SetColor(bkcr,textcr,fbkcr,ftextcr);

	//列表头
	r=f.GetKeyVal(key,"glistheadr",0);
	g=f.GetKeyVal(key,"glistheadg",59);
	b=f.GetKeyVal(key,"glistheadb",108);
	COLORREF color=RGB(r,g,b);
	TCHAR path[MAX_PATH],path1[MAX_PATH];

	if (m_bMode)
	{
		wsprintf(path,".\\image\\game\\list_head1.bmp");
		wsprintf(path1,".\\image\\game\\list_head2.bmp");
	}
	else
	{
		GET_FILENAME(path,"%sroom_right\\list_head1.bmp",skinfolder);
		GET_FILENAME(path1,"%sroom_right\\list_head2.bmp",skinfolder);
	}
#ifndef BZ_ZLIB
	if (!CBcfFile::IsFileExist(path))
	{
		wsprintf (path,"%sroom_right\\list_head1.bmp",skinHall);
	}
	if (!CBcfFile::IsFileExist(path1))
	{
		wsprintf (path1,"%sroom_right\\list_head2.bmp",skinHall);
	}
#else
	CUnZipRes unZipRes;
	IStream * pStream=NULL;
	pStream=unZipRes.LoadFileResFromZip(path);
	if(pStream==NULL)
	{
		wsprintf (path,"%sroom_right\\list_head1.bmp",skinHall);

	}else
	{
		pStream->Release();
		pStream=NULL;
	}
	pStream=unZipRes.LoadFileResFromZip(path1);
	if(pStream==NULL)
	{
		wsprintf (path1,"%sroom_right\\list_head2.bmp",skinHall);

	}else
	{
		pStream->Release();
		pStream=NULL;
	}
#endif

	m_ListHeader.SetBkImage(path,path1,color,color,1);

	SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_ONECLICKACTIVATE|LVS_EX_INFOTIP);
	SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,LVS_EX_FULLROWSELECT,LVS_EX_FULLROWSELECT);

	//设置图片
	//if (m_UserStateList.GetSafeHandle()==NULL)
	{
		//装载位图
		BITMAP BmpInfo;
		CGameImage	UserStateBmp;
		//CBitmap m_NewUserStateBmp;
		AfxSetResourceHandle(GetModuleHandle(FACE_DLL_NAME));
		UserStateBmp.SetLoadInfo("..\\image\\user_state.bmp",true);;
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
		//m_NewUserStateBmp.Detach();

		TargetDC.DeleteDC();
		ReleaseDC(pDC);
	}
	SetImageList(&m_UserStateList,LVSIL_SMALL);

	// PengJiLin, 2010-6-11, 获取用户列表的样式。鸥朴
	m_byUserListType = f.GetKeyVal(key, "UserListDefine", 0);

}

CGameUserListCtrlDu::CGameUserListCtrlDu(BzDui::CListUI* pListCtrl)
{
	//列表控件
	m_pListCtrl = pListCtrl;

	//初始化列表
	m_TempItem.column.clear();
	m_vTable.clear();

	//列表头顺序
	m_vtHeader.clear();

	//自己的信息
	m_pMeUser = NULL;

	//加载列设置
	LoadColumnConfig();

	//游戏ID
	m_uGameNameID = 0;

	//等级函数
	m_pPointOrderFun = NULL;
}

//初始化函数
void CGameUserListCtrlDu::InitListCtrl(GetOrderName * pOrderFunction,UINT uComType)
{
	//CHappyGoDlg::InitListCtrl(pOrderFunction,uComType);
	m_uComType=uComType;

	//CHappyGoDlg::InitListCtrl(pOrderFunction,uComType);
	m_uComType = uComType;

	m_pPointOrderFun = pOrderFunction;

	if (m_pListCtrl)
	{
		MapColumn* pColumn = &m_TempItem.column;

		if (pColumn->size() > 0)
		{

			//排序column
			MapColumn::iterator* pIte = new MapColumn::iterator[pColumn->size()];
			MapColumn::iterator ite = pColumn->begin();
			while(ite != pColumn->end())
			{
				pIte[ite->second.index] = ite;
				ite++;
			}

			CListHeaderUI* pListHeader = m_pListCtrl->GetHeader();
			CListHeaderItemUI* pEle = NULL;

			//状态logo
			pEle = new CListHeaderItemUI();
			if (pEle)
			{
				pEle->SetFixedWidth(pIte[0]->second.Width);
				pEle->SetDragable(false);
				pListHeader->Add(pEle);
			}

			for (int i = 1; i < pColumn->size(); ++i)
			{
				pEle = new CListHeaderItemUI();
				if (pEle)
				{
					pEle->SetSepImage("file='game\\list_sep.png'");
					pEle->SetFixedWidth(pIte[i]->second.Width);
					pEle->SetText(pIte[i]->second.ColumnName);
					pEle->SetTextColor(0x00FFFFFF);
					pEle->SetDragable(true);
					pEle->SetSepWidth(2);
					pEle->SetFont(1);
					
					pListHeader->Add(pEle);
				}
			}

			delete[] pIte;
		}
	}

	return;
}

//是否允许查看 IP
void CGameUserListCtrlDu::EnableIPView(bool bLookIP)
{
	;
}

//设置旁观对象
void CGameUserListCtrlDu::SetWatchUser(UserItemStruct* pUserItem, TCHAR* szWatchName)
{

	if (!pUserItem)
	{
		return;
	}

// 	CString str;
// 	str.Format("DClient:CGameUserListCtrlDu::SetWatchUser p = 0x%08x id = %d",pUserItem,pUserItem->GameUserInfo.dwUserID);
// 	OutputDebugString(str);

	VtDuTable::iterator itFound;

	if (FindItem(pUserItem,&itFound) == -1)
	{
		return;
	}

	MapColumn::iterator ite;
	ite = itFound->column.find("WatchUser"); //旁观者

	if (ite != itFound->column.end())
	{
		if (szWatchName != NULL)
		{
			strncpy(ite->second.Value,szWatchName,32);
		}
		else 
		{
			::strcpy(ite->second.Value,"OK");
		}
			
	}

	if (m_pListCtrl)
	{
		m_pListCtrl->NeedUpdate();
	}

	return;
}

//获取用户状态 ID
UINT CGameUserListCtrlDu::GetStateImageIndex(UserItemStruct* pUserItem)
{
	//定义状态
	static BYTE bUserState[USER_STATE_COUNT]={USER_LOOK_STATE,USER_SITTING,USER_ARGEE,
		USER_WATCH_GAME,USER_CUT_GAME,USER_PLAY_GAME};

	//寻找状态
	UserInfoStruct * pUserInfo=&pUserItem->GameUserInfo;
	for (int i=0;i<USER_STATE_COUNT;i++)
	{
		if (pUserInfo->bUserState==bUserState[i])
		{
			if (pUserInfo->bTrader == 1) return USER_STATE_COUNT*7 + i; 
			if (pUserItem->bMatchUser==true) return USER_STATE_COUNT*6+i;
			if (pUserItem->uConnection==CM_FRIEND) return USER_STATE_COUNT*2+i;
			if (pUserInfo->bGameMaster>=2) return USER_STATE_COUNT*4+i;
			if (pUserInfo->bGameMaster==1) return USER_STATE_COUNT*5+i;
			if (pUserInfo->bMember>=2) return USER_STATE_COUNT*3+i;
			if (pUserInfo->bMember==1) return USER_STATE_COUNT*1+i;
			return i;
		}
	}

	return 0;
}

//获取第n行的用户数据
UserItemStruct* CGameUserListCtrlDu::GetUser(int nRow)
{
	if (nRow < 0 || nRow >= m_vTable.size())
	{
		return NULL;
	}

	return (UserItemStruct*)m_vTable[nRow].data;
}

//列表文本重画
void CGameUserListCtrlDu::OnGetItemText(UserItemStruct* pUserItem,int iIndex,int iSubItem,char* szout,bool bSecret, bool bshowname/* = false*/)
{
	if (m_vTable.size() < iIndex || iIndex < 0)
	{
		::strcpy(szout,_T(""));
		return;
	}

	MapColumn* pColumn = &m_vTable[iIndex].column;

	if (iSubItem >= pColumn->size())
	{
		strcpy(szout,"");
		return;
	}


 	MapColumn::iterator it = pColumn->find(m_vtHeader[iSubItem]);

	if (it == pColumn->end())
	{
		strcpy(szout,"");
		return;
	}

	//状态logo
	if (it->first == "StatLogo")
	{
		_stprintf(szout,_T("<i game//list_state.png 48 %s>"),it->second.Value);
		return;
	}

	//玩家昵称
	if (it->first == "Nickname")
	{
		_stprintf(szout,_T("%s"),bshowname?it->second.Value:"玩家");
		return;
	}

	//玩家昵称
	if (it->first == "Coin")
	{
		_stprintf(szout,_T("%s"),bSecret?"???":it->second.Value);
		return;
	}

	//财富级别
	if (it->first == "WealthLevel")
	{
		_stprintf(szout,_T("%s"),bSecret?"???":it->second.Value);
		return;
	}

	//积分
	if(m_uComType==TY_MATCH_GAME)
	{
		if (it->first == "Score2")
		{
			_stprintf(szout,_T("%s"),bSecret?"???":it->second.Value);
			return;
		}
	}
	else
	{
		if (it->first == "Score")
		{
			_stprintf(szout,_T("%s"),bSecret?"???":it->second.Value);
			return;
		}
	}

	//积分级别
	if (it->first == "ScoreLevel")
	{
		_stprintf(szout,_T("%s"),bSecret?"???":it->second.Value);
		return;
	}


	//总局数
	if (it->first == "GameCount")
	{
		_stprintf(szout,_T("%s"),bSecret?"???":it->second.Value);
		return;
	}

	//胜局
	if (it->first == "WinGame")
	{
		_stprintf(szout,_T("%s"),bSecret?"???":it->second.Value);
		return;
	}

	//输局
	if (it->first == "LoseGame")
	{
		_stprintf(szout,_T("%s"),bSecret?"???":it->second.Value);
		return;
	}

	//和局
	if (it->first == "Peace")
	{
		_stprintf(szout,_T("%s"),bSecret?"???":it->second.Value);
		return;
	}

	//旁观对象
	if (it->first == "WatchUser")
	{
		_stprintf(szout,_T("%s"),it->second.Value);
		return;
	}

	//魅力值
	if (it->first == "Charm")
	{
		_stprintf(szout,_T("%s"),bSecret?"???":it->second.Value);
		return;
	}

	//魅力等级
	if (it->first == "CharmLevel")
	{
		_stprintf(szout,_T("%s"),bSecret?"???":it->second.Value);
		return;
	}

	strcpy(szout,"");
	
	return;
}

//查找玩家数据在列表的位置
int CGameUserListCtrlDu::FindItem(UserItemStruct* pUserItem,VtDuTable::iterator* _out_it_found/* = NULL*/)
{
	int size = m_vTable.size();

	if (_out_it_found)
	{
		*_out_it_found = m_vTable.end();
	}

	if (size == 0)
	{
		return -1;
	}

	VtDuTable::iterator ite = m_vTable.begin();

	for (int i = 0; i < size; ++i,ite++)
	{
		if ((UINT_PTR)pUserItem == m_vTable[i].data)
		{
			if (_out_it_found)
			{
				*_out_it_found = ite;
			}

			return i;
		}
	}

	return -1;

}

int CGameUserListCtrlDu::FindItem(BzDui::CControlUI* pItemCtrl)
{
	int size = m_vTable.size();

	VtDuTable::iterator ite = m_vTable.begin();

	for (int i = 0; i < size; ++i,ite++)
	{
		if ((UINT_PTR)pItemCtrl == m_vTable[i].item_ptr)
		{
			return i;
		}
	}

	return -1;
}

//查找玩家列表项的插入位置
int CGameUserListCtrlDu::GetInsertPos(UserItemStruct* pUserItem,VtDuTable::iterator* _out_it_found/* = NULL*/)
{
	int size = m_vTable.size();

	if (_out_it_found)
	{
		*_out_it_found = m_vTable.begin();
	}

	if (size == 0)
	{
		return 0;
	}

	VtDuTable::iterator ite = m_vTable.begin();
	UserItemStruct* pUser = NULL;

	for (int i = 0; i < size; ++i,ite++)
	{
		pUser = (UserItemStruct*)m_vTable[i].data;
		if (!pUser)
		{
			continue;
		}

		if (i == 0 && pUser == m_pMeUser)//自己的item置顶
		{
			continue;
		}

		if (pUserItem->GameUserInfo.bMember > pUser->GameUserInfo.bMember)
		{
			if (_out_it_found)
			{
				*_out_it_found = ite;
			}
			return i;
		}
		else if (pUserItem->GameUserInfo.bMember == pUser->GameUserInfo.bMember) ///< 同等级VIP，按照钱数排名
		{
			if (pUserItem->GameUserInfo.i64Money >= pUser->GameUserInfo.i64Money)
			{
				if (_out_it_found)
				{
					*_out_it_found = ite;
				}
				return i;
			}
		}
	}

	//值最小，排到链表的最后
	if (_out_it_found)
	{
		*_out_it_found = m_vTable.end();
	}
	return size;
}

//增加用户
bool CGameUserListCtrlDu::AddGameUser(UserItemStruct* pUserItem, bool bIsMySelf/* = false*/)
{
	if(!pUserItem)
	{
		return false;
	}

// 	CString str;
// 	str.Format("DClient:CGameUserListCtrlDu::AddGameUser p = 0x%08x id = %d",pUserItem,pUserItem->GameUserInfo.dwUserID);
// 	OutputDebugString(str);

	if (FindItem(pUserItem) >= 0)
	{
 		UpdateGameUser(pUserItem);
		return true;
	}

	CListTextElementUI* pNewItem = new CListTextElementUI();

	//将数据填充到TempItem中
	
	PrepareDate(pUserItem,pNewItem); 

	VtDuTable::iterator itFound;
	int nLastPos = 0;
	
	if (bIsMySelf) //自己的信息置顶
	{
		nLastPos = 0;
		m_vTable.insert(m_vTable.begin(),m_TempItem);
		m_pMeUser = pUserItem;
	}
	else
	{
		nLastPos = GetInsertPos(pUserItem,&itFound);
		m_vTable.insert(itFound,m_TempItem);
	}

	pNewItem->SetFixedHeight(22);

	if (m_pListCtrl)
	{
		m_pListCtrl->AddAt(pNewItem,nLastPos);
		m_pListCtrl->NeedUpdate();
	}

	return true;
}


//更新状态
bool CGameUserListCtrlDu::UpdateGameUser(UserItemStruct * pUserItem)
{

	if (!pUserItem)
	{
		return false;
	}

// 	CString str;
// 	str.Format("DClient:CGameUserListCtrlDu::UpdateGameUser p = 0x%08x id = %d",pUserItem,pUserItem->GameUserInfo.dwUserID);
// 	OutputDebugString(str);

	VtDuTable::iterator itFound;

	if (FindItem(pUserItem,&itFound) == -1)
	{
		return false;
	}

	//查找数据结点

	//将数据填充到MAP中
	PrepareDate(pUserItem,(CListTextElementUI*)itFound->item_ptr); 

	itFound->column = m_TempItem.column;
	itFound->data = (UINT_PTR)pUserItem;

	m_pListCtrl->NeedUpdate();

	return true;
}


//删除用户
bool CGameUserListCtrlDu::DeleteGameUser(UserItemStruct * pUserItem)
{
	if(!pUserItem)
	{
		return false;
	}

// 	CString str;
// 	str.Format("DClient:CGameUserListCtrlDu::DeleteGameUser p = 0x%08x id = %d",pUserItem,pUserItem->GameUserInfo.dwUserID);
// 	OutputDebugString(str);

	VtDuTable::iterator itFound;

	if (FindItem(pUserItem,&itFound) == -1)
	{
		return false;
	}

	//删除列表框item
	CListTextElementUI* pEle = reinterpret_cast<CListTextElementUI*>(itFound->item_ptr);
	if (m_pListCtrl && pEle)
	{
		m_pListCtrl->Remove(pEle);
		m_pListCtrl->NeedUpdate();
	}

	//删除数据结点
	m_vTable.erase(itFound);

	return true;
}

//删除所有
void CGameUserListCtrlDu::DeleteAllItems()
{
	m_vTable.clear();

	if (m_pListCtrl)
	{
		m_pListCtrl->RemoveAll();
	}
}

//加载列设置
void CGameUserListCtrlDu::LoadColumnConfig()
{
	CString strColumnName;
	CString strColumnIndex;
	CString strColumnIndexAll;
	CString strConfig;
	BOOL    bConfigExist;

	CString strDefaultColumnIndexAll = "Nickname,Coin,WealthLevel,Score,ScoreLevel,Charm,CharmLevel,Sex,GameCount,WinGame,LoseGame,Peace,Escape,WatchUser,";
	bConfigExist = TRUE;

	CBcfFile fUserList(CBcfFile::GetAppPath () + "..\\GameUserInforList.bcf");
	strColumnIndexAll = fUserList.GetKeyVal("InforList","_ColumnOrder","");
	if (strColumnIndexAll.IsEmpty())
	{
		bConfigExist = FALSE;
		strColumnIndexAll = strDefaultColumnIndexAll;
	}

	int iIndex = 0;
	int iWidth = 0;
	int nColCount = 0;

	ColumnStruct ColStr;
	ColStr.index = iIndex;
	ColStr.Width = 35;
	strcpy(ColStr.ColumnName,"");
	m_TempItem.column.insert(pair<string,ColumnStruct>("StatLogo",ColStr));
	m_vtHeader.push_back("StatLogo");
	iIndex++;

	int nHeadPos = 0;
	int nTailPos = strColumnIndexAll.Find(',', nHeadPos);
	
	while (nTailPos > nHeadPos)
	{
		strColumnIndex = strColumnIndexAll.Mid(nHeadPos, nTailPos-nHeadPos);
		if (bConfigExist)
			strConfig = fUserList.GetKeyVal("InforList",strColumnIndex,"");
		else
			strConfig = COLOMN_NAME[iIndex - 1];

		if (!strConfig.IsEmpty())
		{
			DecodeString(strConfig,strColumnName,iWidth);
			ColStr.index = iIndex;
			ColStr.Width = iWidth;
			strncpy(ColStr.ColumnName,strColumnName,MAX_PATH/2);
			m_TempItem.column.insert(pair<string,ColumnStruct>(strColumnIndex.GetBuffer(),ColStr));
// 			CString str;
// 			str.Format("DClient:str = %s",strColumnIndex.GetBuffer());
// 			OutputDebugString(str);

			m_vtHeader.push_back(strColumnIndex.GetBuffer());

			iIndex++;
		}
		

		nHeadPos = nTailPos + 1;
		nTailPos = strColumnIndexAll.Find(',', nHeadPos);
	}

	m_TempItem.data = (UINT_PTR)0;

}

/// 将字符串解析出 顺序号，列名，列宽
void CGameUserListCtrlDu::DecodeString(CString str,CString &strColumn,int &width)
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

/// 设置每一行的值
void CGameUserListCtrlDu::PrepareDate(UserItemStruct * pUserItem,BzDui::CListTextElementUI* pElementItem)
{
	CString strBuffer;
	CString strTemp;
	MapColumn::iterator ite;

	UserInfoStruct * pUserInfo = &pUserItem->GameUserInfo;
	UINT uAllCount   = pUserInfo->uWinCount + pUserInfo->uLostCount + pUserInfo->uMidCount;
	UINT uCutPercent = (pUserInfo->uCutCount>0)?(UINT)((double)(pUserInfo->uCutCount*100)/(double)uAllCount):0;
	int dwPoint,dwFascination;
	__int64 i64Money;
	i64Money       = pUserInfo->i64Money;
	dwPoint       = pUserInfo->dwPoint;
	dwFascination = pUserInfo->dwFascination; 

	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");

	if (pUserInfo->dwUserID >= 0)
		strTemp.Format(TEXT("%d"),pUserInfo->dwUserID); ///< 玩家ID 

	ite = m_TempItem.column.find("StatLogo");  //状态Logo
	if (m_TempItem.column.end() != ite)
	{
		_stprintf(ite->second.Value,"%d",GetStateImageIndex(pUserItem));
	}

	strBuffer = pUserInfo->nickName; ///< 玩家昵称
	ite = m_TempItem.column.find("Nickname"); 
	if (m_TempItem.column.end() != ite)
	{
		strncpy(ite->second.Value,strBuffer,32);
	}

	ite = m_TempItem.column.find("UserID"); ///< 玩家ID
	if (m_TempItem.column.end() != ite)
	{
		strncpy(ite->second.Value,strTemp,32);
	}

	//金币
	TCHAR szNum[128];
	GlbGetNumString(szNum, i64Money, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
	strBuffer.Format(TEXT("%s"),szNum);

	//金币
	ite = m_TempItem.column.find("Coin"); 
	if (m_TempItem.column.end() != ite)
	{
		strncpy(ite->second.Value,strBuffer,32);
	}

	//财富级别
	strBuffer = GetMoneyOrderName(i64Money);
	ite = m_TempItem.column.find("WealthLevel");
	if (m_TempItem.column.end() != ite)
	{
		strncpy(ite->second.Value,strBuffer,32);
	}

	//积分
	strBuffer.Format(TEXT("%d"),dwPoint-GetNormalOrderBasePoint(dwPoint));

	//赛币
	if(m_uComType==TY_MATCH_GAME)
		ite = m_TempItem.column.find("Score2"); 
	else 
		ite = m_TempItem.column.find("Score");

	if (m_TempItem.column.end() != ite)
	{
		strncpy(ite->second.Value,strBuffer,32);
	}

	//积分级别
	if (m_pPointOrderFun)
	{
		strBuffer = m_pPointOrderFun(dwPoint);
		ite = m_TempItem.column.find("ScoreLevel"); ///< 积分级别
		if (m_TempItem.column.end() != ite)
		{
			strncpy(ite->second.Value,strBuffer,32);
		}
	}

	//魅力值
	strBuffer.Format(TEXT("%d"),dwFascination);
	ite = m_TempItem.column.find("Charm"); ///< 魅力值
	if (m_TempItem.column.end() != ite)
	{
		strncpy(ite->second.Value,strBuffer,32);
	}

	//魅力等级
	strBuffer.Format(TEXT("%s"),GetCharmGrade(dwFascination));
	ite = m_TempItem.column.find("CharmLevel");
	if (m_TempItem.column.end() != ite)
	{
		strncpy(ite->second.Value,strBuffer,32);
	}

	//性别
	strBuffer = pUserItem->GameUserInfo.bBoy?fMsg.GetKeyVal("UserListCtrl","Male","男"):fMsg.GetKeyVal("UserListCtrl","Female","女");
	ite = m_TempItem.column.find("Sex");
	if (m_TempItem.column.end() != ite)
	{
		strncpy(ite->second.Value,strBuffer,32);
	}

	//总局数
	strBuffer.Format(TEXT("%ld"),uAllCount);
	ite = m_TempItem.column.find("GameCount");
	if (m_TempItem.column.end() != ite)
	{
		strncpy(ite->second.Value,strBuffer,32);
	}

	//胜局
	strBuffer.Format(TEXT("%ld"),pUserInfo->uWinCount);
	ite = m_TempItem.column.find("WinGame");
	if (m_TempItem.column.end() != ite)
	{
		strncpy(ite->second.Value,strBuffer,32);
	}

	//输局
	strBuffer.Format(TEXT("%d"),pUserInfo->uLostCount);
	ite = m_TempItem.column.find("LoseGame");
	if (m_TempItem.column.end() != ite)
	{
		strncpy(ite->second.Value,strBuffer,32);
	}

	//和局
	strBuffer.Format(TEXT("%d"),pUserInfo->uMidCount);
	ite = m_TempItem.column.find("Peace");
	if (m_TempItem.column.end() != ite)
	{
		strncpy(ite->second.Value,strBuffer,32);
	}

	//断线率
	if (uCutPercent>0) 
		strBuffer.Format(TEXT("%2ld%%"),uCutPercent);
	else
		strBuffer.Empty();

	 //断线率
	ite = m_TempItem.column.find("Escape");
	if (m_TempItem.column.end() != ite)
	{
		strncpy(ite->second.Value,strBuffer,32);
	}

	//旁观对象
	ite = m_TempItem.column.find("WatchUser");
	if (m_TempItem.column.end() != ite)
	{
		strncpy(ite->second.Value,"",32);
	}

	m_TempItem.data     = (UINT_PTR)pUserItem;
	m_TempItem.item_ptr = (UINT_PTR)pElementItem;

	return;
}


//构造函数
CPictureHelper::CPictureHelper() : CGameFaceGo(IDD_USER_LIST)
{
	m_pUserListDu = NULL;

}
//析构函数
CPictureHelper::~CPictureHelper()
{
}

//倥件绑定函数
void CPictureHelper::DoDataExchange(CDataExchange * pDX)
{
	CGameFaceGo::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_USER_LIST, m_UserList);
}

//初始化
void CPictureHelper::InitListCtrl(GetOrderName * pOrderFunction, UINT uComType)
{ 
	m_UserList.InitListCtrl(pOrderFunction,uComType);
	if (m_pUserListDu)
	{
		m_pUserListDu->InitListCtrl(pOrderFunction,uComType);
	}

	return ; 
}

//增加用户
bool CPictureHelper::AddGameUser(UserItemStruct * pUserItem, bool bIsMySelf/* = false*/) 
{ 
	bool bRet = m_UserList.AddGameUser(pUserItem, bIsMySelf);
	if (m_pUserListDu)
	{
		bRet = m_pUserListDu->AddGameUser(pUserItem,bIsMySelf); 
	}
	return bRet; 
};

//删除用户
bool CPictureHelper::DeleteGameUser(UserItemStruct * pUserItem)
{
	bool bRet = m_UserList.DeleteGameUser(pUserItem);
	if (m_pUserListDu)
	{
		bRet = m_pUserListDu->DeleteGameUser(pUserItem); 
	}
	return bRet; 
};

//更新状态
bool CPictureHelper::UpdateGameUser(UserItemStruct * pUserItem)
{ 
	bool bRet = m_UserList.UpdateGameUser(pUserItem);
	if (m_pUserListDu)
	{
		bRet = m_pUserListDu->UpdateGameUser(pUserItem); 
	}
	return bRet; 
}

//是否允许查看 IP
void CPictureHelper::EnableIPView(bool bLookIP)
{
	m_UserList.EnableIPView(bLookIP);
	if (m_pUserListDu)
	{
		m_pUserListDu->EnableIPView(bLookIP); 
	}
};

//删除所有
void CPictureHelper::DeleteAllUser()
{ 
	m_UserList.DeleteAllItems();
	if (m_pUserListDu)
	{
		m_pUserListDu->DeleteAllItems(); 
	}
};

//设置旁观对象
void CPictureHelper::SetWatchUser(UserItemStruct* pUserItem, TCHAR* szWatchName)
{
	m_UserList.SetWatchUser(pUserItem,szWatchName); 
	if (m_pUserListDu)
	{
		m_pUserListDu->SetWatchUser(pUserItem,szWatchName); 
	}
}


//位置变化
void CPictureHelper::OnSize(UINT nType, int cx, int cy)
{
	CGameFaceGo::OnSize(nType, cx, cy);
	//移动控件
	SafeMoveWindow(&m_UserList,0,0,cx,cy);

	CString s=CBcfFile::GetAppPath ();/////本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + "..\\" + strSkin);
	TCHAR path[MAX_PATH];
	CString skinfolder = f.GetKeyVal(m_skinmgr.GetKeyVal(strSkin),"skinfolder",m_skinmgr.GetSkinPath());
	wsprintf(path,"%sDialog\\scroll.bmp",skinfolder);
	if(!CBcfFile::IsFileExist(path))
	{
		wsprintf(path,"%sDialog\\scroll.bmp",CString("..\\") + skinfolder);
	}
	HBITMAP hbmScorll = (HBITMAP)LoadImage(NULL, path, IMAGE_BITMAP, 0,0, LR_LOADFROMFILE);
	SkinSB_Init(m_UserList.GetSafeHwnd(), hbmScorll);
	return;
}

//鼠标右键用户列表消息
void CPictureHelper::OnNMRclickUserList(NMHDR * pNMHDR, LRESULT * pResult)
{
	NMITEMACTIVATE * pListNotify=(NMITEMACTIVATE *)pNMHDR;
	if (pListNotify->iItem!=-1)
	{
		UserItemStruct * pUserItem=(UserItemStruct *)m_UserList.GetItemData(pListNotify->iItem);
		((CLoveSendClassInExe *)GetParent())->OpenUserMenu(pUserItem);
	}
	*pResult=0;
}


//鼠标左键用户列表消息
void CPictureHelper::OnNMLclickUserList(NMHDR * pNMHDR, LRESULT * pResult)
{
	NMITEMACTIVATE * pListNotify=(NMITEMACTIVATE *)pNMHDR;
	if (pListNotify->iItem!=-1)
	{
		UserItemStruct * pUserItem=(UserItemStruct *)m_UserList.GetItemData(pListNotify->iItem);
		((CLoveSendClassInExe *)GetParent())->OnLBClickUserList(pUserItem);
	}
	*pResult=0;
}

/*******************************************************************************************************/

void CPictureHelper::OnNMDBclickUserList(NMHDR * pNMHDR, LRESULT * pResult)
{
	NMITEMACTIVATE * pListNotify=(NMITEMACTIVATE *)pNMHDR;
	if (pListNotify->iItem!=-1)
	{
		UserItemStruct * pUserItem=(UserItemStruct *)m_UserList.GetItemData(pListNotify->iItem);
		((CLoveSendClassInExe *)GetParent())->OnLBDBClickUserList(pUserItem);
	}

	*pResult=0;
}
//换肤
void CGameUserListCtrl::InitIniFile()
{
	CString strGameSkinFilename = m_skinmgr.GetGameSkinBcfFileName();
	if (CBcfFile::IsFileExist(CBcfFile::GetAppPath() + "Image\\game\\" + strGameSkinFilename))
	{
		m_iniFile= CBcfFile::GetAppPath() + "Image\\game\\" + strGameSkinFilename;
		m_bMode = TRUE;
	}
	else
	{
		m_iniFile= CBcfFile::GetAppPath() + "..\\image\\Client\\" + m_skinmgr.GetKeyVal(m_skinmgr.GetSkinBcfFileName())+"\\game\\";
		m_iniFile += m_skinmgr.GetGameSkinBcfFileName();
		m_bMode = FALSE;
	}
}
