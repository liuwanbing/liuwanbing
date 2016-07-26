//用户列表
#include "StdAfx.h"
#include "Resource.h"
#include "UserListDlg.h"
#include "GameFrameDlg.h"
#include "afcdevhead.h"

BEGIN_MESSAGE_MAP(CGameUserListCtrl, CHappyGoDlg)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CPictureHelper, CGameFaceGo)
	ON_WM_SIZE()
	ON_NOTIFY(NM_RCLICK, IDC_USER_LIST, OnNMRclickUserList)
	ON_NOTIFY(NM_CLICK, IDC_USER_LIST, OnNMLclickUserList)
	ON_NOTIFY(NM_DBLCLK, IDC_USER_LIST, OnNMDBclickUserList)
END_MESSAGE_MAP()

/*******************************************************************************************************/

//是否字符串
UINT CGameUserListCtrl::m_uItemKind[15]=
	{
		IK_NAME,			//用户名
		IK_USERID,
		IK_DESKNO,			//桌号
		IK_MONEY,			//金币
		IK_WEALTH_LEVEL,
		IK_POINT,			//等级
		IK_LEVEL,
		IK_CHARM,
		IK_CHARM_LEVEL,
		IK_SEX,
		IK_ALL_COUNT,		//总局数
		IK_WINCOUNT,		//胜局
		IK_LOSTCOUNT,		//输局
		IK_MIDCOUNT,		//和局
		IK_CUT_PERCENT		//逃跑率
	};

//构造函数
CGameUserListCtrl::CGameUserListCtrl() : CHappyGoDlg(m_uItemKind)
{
		m_iniFile="image\\client\\"+Glb().m_skin+"\\game\\gameskini.bcf";// TEXT("image\\game\\skin.ini");
}

//析构函数
CGameUserListCtrl::~CGameUserListCtrl()
{
	m_NewUserStateBmp.DeleteObject();
}

//初始化函数
void CGameUserListCtrl::InitListCtrl(GetOrderName * pOrderFunction, UINT uComType)
{
	CHappyGoDlg::InitListCtrl(pOrderFunction,uComType);
	m_uComType=uComType;
	//定义数据
	TCHAR szBuffer[10]=TEXT("积分");
	//if (uComType==TY_MONEY_GAME) lstrcpy(szBuffer,TEXT("金币"));
	if (uComType==TY_MATCH_GAME) 
		lstrcpy(szBuffer,TEXT("金币"));

	//插入数据
	SetRedraw(FALSE);
	//InsertColumn(0,"用户名",LVCFMT_LEFT,115);
	
	//if (uComType==TY_MONEY_GAME)
	{
		int i = 0;
		InsertColumn(i++,"昵称",LVCFMT_LEFT,115);


		InsertColumn(i++,"金币",LVCFMT_LEFT,60);
		InsertColumn(i++,"财富级别",LVCFMT_LEFT,85);
		/////////////////////////////////
		////Kylin 20090119 赛币场显示处理
		if(uComType==TY_MATCH_GAME)
		{
			InsertColumn(i++,"赛币",LVCFMT_LEFT,55);
		}
		else
		{
			InsertColumn(i++,"积分",LVCFMT_LEFT,55);
		}
		/////////////////////////////////
		InsertColumn(i++,"积分等级",LVCFMT_LEFT,85);
		InsertColumn(i++,"魅力值",LVCFMT_LEFT,60);
		InsertColumn(i++,"魅力级别",LVCFMT_LEFT,55);
		InsertColumn(i++,"性别",LVCFMT_LEFT,40);
		////JMod-添加玩家职业和所在区域信息  yjj 090506
		//InsertColumn(i++,"职业",LVCFMT_LEFT,80);
		//InsertColumn(i++,"所在区域",LVCFMT_LEFT,80);
		InsertColumn(i++,"总局数",LVCFMT_LEFT,50);
		InsertColumn(i++,"胜局数",LVCFMT_LEFT,50);
		InsertColumn(i++,"输局数",LVCFMT_LEFT,50);
		InsertColumn(i++,"和局数",LVCFMT_LEFT,50);
		InsertColumn(i++,"逃跑率",LVCFMT_LEFT,50);
		InsertColumn(i++,"旁观对象",LVCFMT_CENTER,100);
	}
	//else
	//{
	//	InsertColumn(0,"昵称",LVCFMT_LEFT,115);
	//	InsertColumn(1,"积分",LVCFMT_LEFT,55);
	//	InsertColumn(2,"积分等级",LVCFMT_LEFT,85);
	//	InsertColumn(3,"性别",LVCFMT_LEFT,40);
	//	InsertColumn(4,"总局数",LVCFMT_LEFT,50);
	//	InsertColumn(5,"胜局数",LVCFMT_LEFT,50);
	//	InsertColumn(6,"输局数",LVCFMT_LEFT,50);
	//	InsertColumn(7,"和局数",LVCFMT_LEFT,50);
	//	InsertColumn(8,"逃跑率",LVCFMT_LEFT,50);
	//	InsertColumn(9,"旁观对象",LVCFMT_CENTER,100);
	//}
	//InsertColumn(10,"用户IP",LVCFMT_LEFT,120);

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
		if (szWatchName!=NULL) 
			SetItem(iItem,13,1,szWatchName,0,0,0,0);
		else 
			SetItem(iItem,13,1,"Ok",0,0,0,0);//TEXT(IDOK)
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
bool CGameUserListCtrl::AddGameUser(UserItemStruct * pUserItem)
{
	if (pUserItem==NULL) return false;

	//查找用户
	LVFINDINFO FindInfo;
	::memset(&FindInfo,0,sizeof(FindInfo));
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pUserItem;
	int iItem=FindItem(&FindInfo);
	if (iItem!=-1) 
	{
		UpdateGameUser(pUserItem);
		return false;
	}
	
	//定义变量
	CString strBuffer;
	UserInfoStruct * pUserInfo=&pUserItem->GameUserInfo;
	UINT uAllCount=pUserInfo->uWinCount+pUserInfo->uLostCount+pUserInfo->uMidCount;
	UINT uCutPercent=(pUserInfo->uCutCount>0)?(UINT)((double)(pUserInfo->uCutCount*100)/(double)uAllCount):0;
	
	//用户名字
	iItem=InsertItem(GetItemCount(),pUserInfo->szName,GetStateImageIndex(pUserItem));
	//妮称
	SetItem(iItem,0,1,pUserInfo->nickName,0,0,0,0);
	
	{		
		
		int dwPoint=0,dwFascination;
        __int64 i64Money=0;
		i64Money=pUserInfo->i64Money;
		dwPoint=pUserInfo->dwPoint;
		dwFascination=pUserInfo->dwFascination;
		int i = 1;

		//金币
		TCHAR szNum[128];
        GetNumString(szNum, i64Money, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
		strBuffer.Format(TEXT("%s"),szNum);
		SetItem(iItem,i++,1,strBuffer,0,0,0,0);
		//财富级别
		SetItem(iItem,i++,1,GetMoneyOrderName(i64Money,0),0,0,0,0);
			
		//积分级别
		strBuffer.Format(TEXT("%d"),dwPoint);
		SetItem(iItem,i++,1,strBuffer,0,0,0,0);
		
		//等级
		SetItem(iItem,i++,1,m_pGetOrderFun(dwPoint,0),0,0,0,0);	

		//魅力
		strBuffer.Format(TEXT("%d"),dwFascination);
		SetItem(iItem,i++,1,strBuffer,0,0,0,0);

		//魅力级别
		strBuffer.Format(TEXT("%s"),GetCharmGrade(dwFascination));
		SetItem(iItem,i++,1,strBuffer,0,0,0,0);		
		
		//性别
		SetItem(iItem,i++,1,pUserItem->GameUserInfo.bBoy?"男":"女",0,0,0,0);
		////wushuqun 2009.5.19
		////所在区域改为“省.市"
		//SetItemText(iItem,i++,pUserInfo->szOccupation);
		//CString strAre;
		//strAre = pUserInfo->szProvince + CString(".") + pUserInfo->szCity;
		//SetItemText(iItem,i++,strAre);

		//总局数
		strBuffer.Format(TEXT("%ld"),uAllCount);
		SetItem(iItem,i++,1,strBuffer,0,0,0,0);
		//胜局
		strBuffer.Format(TEXT("%ld"),pUserInfo->uWinCount);
		SetItem(iItem,i++,1,strBuffer,0,0,0,0);
		//输局
		strBuffer.Format(TEXT("%d"),pUserInfo->uLostCount);
		SetItem(iItem,i++,1,strBuffer,0,0,0,0);
		//和局
		strBuffer.Format(TEXT("%d"),pUserInfo->uMidCount);
		SetItem(iItem,i++,1,strBuffer,0,0,0,0);
		//断线率
		if (uCutPercent>0)
			strBuffer.Format(TEXT("%2ld%%"),uCutPercent);
		else
			strBuffer.Empty();
		SetItem(iItem,i++,1,strBuffer,0,0,0,0);
		//游戏社团
		//SetItem(iItem,8,1,pUserInfo->szClassName,0,0,0,0);
		////游戏 IP
		//strBuffer.Format(TEXT("%d.%d.%d.%d"),FOURTH_IPADDRESS(pUserInfo->dwUserIP),THIRD_IPADDRESS(pUserInfo->dwUserIP),
		//SECOND_IPADDRESS(pUserInfo->dwUserIP),FIRST_IPADDRESS(pUserInfo->dwUserIP));
		//SetItem(iItem,i++,1,strBuffer,0,0,0,0);
		
		//设置扩展信息
		SetItemData(iItem,(DWORD_PTR)pUserItem);
	}

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
	if (pUserItem==NULL) return false;

	//查找用户
	LVFINDINFO FindInfo;
	memset(&FindInfo,0,sizeof(FindInfo));
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pUserItem;
	int iItem=FindItem(&FindInfo);
	if (iItem==-1) return false;

	//定义变量
	CString strBuffer;
	UserInfoStruct * pUserInfo=&pUserItem->GameUserInfo;
	UINT uAllCount=pUserInfo->uWinCount+pUserInfo->uLostCount+pUserInfo->uMidCount;
	UINT uCutPercent=(pUserInfo->uCutCount>0)?(UINT)((double)(pUserInfo->uCutCount*100)/(double)uAllCount):0;

	//设置图标
	SetItem(iItem,0,LVIF_IMAGE,NULL,GetStateImageIndex(pUserItem),0,0,0,0);
	//妮称
	SetItemText(iItem,0,pUserItem->GameUserInfo.nickName);
	
	//得分
	int dwPoint;
    __int64 i64Money;
	dwPoint=pUserInfo->dwPoint;
	i64Money=pUserInfo->i64Money;
	int dwFascination=pUserInfo->dwFascination;
	//if(m_uComType==TY_MONEY_GAME)
	{
		int i = 1;


		//财富
		TCHAR szNum[128];
        GetNumString(szNum, i64Money, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
		strBuffer.Format(TEXT("%d"),szNum);
		SetItemText(iItem,i++,strBuffer);
		//财富级别
		SetItemText(iItem,i++,GetMoneyOrderName(i64Money,0));
		//积分值
		strBuffer.Format(TEXT("%d"),dwPoint);
		SetItemText(iItem,i++,strBuffer);
	
		//等级
		SetItemText(iItem,i++,m_pGetOrderFun(dwPoint,0));

		//魅力值
		strBuffer.Format(TEXT("%d"),dwFascination);
		SetItemText(iItem,i++,strBuffer);

		//魅力级别
		strBuffer.Format(TEXT("%s"),GetCharmGrade(dwFascination));
		SetItemText(iItem,i++,strBuffer);		
		//性别
		SetItemText(iItem,i++,pUserItem->GameUserInfo.bBoy?"男":"女");

		////wushuqun 2009.5.19
		////所在区域改为“省.市"
		//SetItemText(iItem,i++,pUserInfo->szOccupation);
		//CString strAre;
		//strAre = pUserInfo->szProvince + CString(".") + pUserInfo->szCity;
		//SetItemText(iItem,i++,strAre);

		//总局数
		strBuffer.Format(TEXT("%ld"),uAllCount);
		SetItemText(iItem,i++,strBuffer);
		//胜局
		strBuffer.Format(TEXT("%ld"),pUserInfo->uWinCount);
		SetItemText(iItem,i++,strBuffer);
		//输局
		strBuffer.Format(TEXT("%d"),pUserInfo->uLostCount);
		SetItemText(iItem,i++,strBuffer);
		//和局
		strBuffer.Format(TEXT("%d"),pUserInfo->uMidCount);
		SetItemText(iItem,i++,strBuffer);
		//断线率
		if (uCutPercent>0) 
			strBuffer.Format(TEXT("%2ld%%"),uCutPercent);
		else strBuffer.Empty();
		SetItemText(iItem,i++,strBuffer);
		
		if (pUserInfo->bGameMaster>0)
		{
			strBuffer.Format(TEXT("%d.%d.%d.%d"),FOURTH_IPADDRESS(pUserInfo->dwUserIP),THIRD_IPADDRESS(pUserInfo->dwUserIP),
				SECOND_IPADDRESS(pUserInfo->dwUserIP),FIRST_IPADDRESS(pUserInfo->dwUserIP));
			SetItemText(iItem,14,strBuffer);
		}
	}

	return true;
}

void CGameUserListCtrl::SetIniFile(LPCTSTR path)
{
	m_iniFile = path;
}

void CGameUserListCtrl::LoadSkin(void)
{
	//设置信息
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f( s + m_iniFile);
//	CINIFile f( s + "skin.ini");
	//CString key=Glb().m_skin;
	CString skinfolder= "image\\client\\skin0\\";
	skinfolder.Format("%s\\image\\client\\%s\\",CBcfFile::GetAppPath(),Glb().m_skin);
	//skinfolder=f.GetKeyVal(key,"skinfolder",);
	CString key=TEXT("config");
	//CString skinfolder;
	//skinfolder=f.GetKeyVal(key,"skinfolder1","image\\client\\skin0\\");
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
		m_NewUserStateBmp.CreateCompatibleBitmap(pDC,BmpInfo.bmWidth,BmpInfo.bmHeight);
		CBitmap * pOldBmp=TargetDC.SelectObject(&m_NewUserStateBmp);
		TargetDC.FillSolidRect(0,0,BmpInfo.bmWidth,BmpInfo.bmHeight,m_bkColor);

		AFCStretchImage(&TargetDC,0,0,BmpInfo.bmWidth,BmpInfo.bmHeight,UserStateBmp,0,0,BmpInfo.bmWidth,BmpInfo.bmHeight,RGB(255,255,255));
		TargetDC.SelectObject(pOldBmp);
		TargetDC.DeleteDC();

		//建立资源列
		if(m_UserStateList.m_hImageList)
		{
			for (int i=0; i<m_UserStateList.GetImageCount(); ++i)
			{
				m_UserStateList.Remove(0);
			}
			m_UserStateList.DeleteImageList();
		}
		if (m_UserStateList.GetImageCount()<=0)
		{
			m_UserStateList.Create(USER_STATE_WIDTH,USER_STATE_HEIGHT,ILC_COLOR16,0,0);
			m_UserStateList.Add(&m_NewUserStateBmp,RGB(0,0,255));
		}

		//清理资源
		UserStateBmp.Detach();
		//NewUserStateBmp.Detach();
		ReleaseDC(pDC);
	}
	SetImageList(&m_UserStateList,LVSIL_SMALL);
}
//构造函数
CPictureHelper::CPictureHelper() : CGameFaceGo(IDD_USER_LIST)
{
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

//位置变化
void CPictureHelper::OnSize(UINT nType, int cx, int cy)
{
	CGameFaceGo::OnSize(nType, cx, cy);
	//移动控件
	SafeMoveWindow(&m_UserList,0,0,cx,cy);

	return;
}

//鼠标右键用户列表消息
void CPictureHelper::OnNMRclickUserList(NMHDR * pNMHDR, LRESULT * pResult)
{
	NMITEMACTIVATE * pListNotify=(NMITEMACTIVATE *)pNMHDR;
	if (pListNotify->iItem!=-1)
	{
		UserItemStruct * pUserItem=(UserItemStruct *)m_UserList.GetItemData(pListNotify->iItem);
		((CLoveSendClass *)GetParent())->OpenUserMenu(pUserItem);
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
		((CLoveSendClass *)GetParent())->OnLBClickUserList(pUserItem);
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
		((CLoveSendClass *)GetParent())->OnLBDBClickUserList(pUserItem);
	}

	*pResult=0;
}
