#include "StdAfx.h"
#include "PBankWndEx.h"
#include "GameListCtrl.h"
#include "MainRoomEx.h"
#include "Bank4ZMessage.h"

using namespace BzDui;

#define SORT_NUMS				3
#define RECORD_PAGE_SIZE		8	//转日志每页的记录数量
#define IDT_TIMEOUT_TO_LOGOUT	1

#define NICK_ID_USE_TO_TRANSFER	11 //用于银行转账
#define NICK_ID_USE_TO_DEPOSIT	12 //用于点卡充值
#define NICK_ID_BYID			0  //通过用户ID查询用户昵称
#define NICK_ID_BYNICKNAME		1  //通过玩家昵称查询用户ID


/*const int WM_BANK_CLOSE= WM_USER + 524;*/

// const int DIGITSIZE = 10;	//数字中文大写
// const int UNITSIZE	= 19;	//金额最大位数
static char* szDigit[DIGITSIZE] = {"零", "壹", "贰", "叁", "肆", "伍", "陆", "柒", "捌", "玖"}; //中文大写数字
static char* szUnit[UNITSIZE]   = {"", "拾", "佰", "仟", "万", "拾", "佰", "仟", "亿", "拾","佰","仟","万", "拾","佰","仟","万","拾","佰"};	  //中文大写单位

//当前实例窗口句柄
HWND CPBankWndEx::m_hInstanceWnd = NULL;

CPBankWndEx::CPBankWndEx(CWnd* pParent,bool bInMainRoom /*= false*/)
{

	m_pRoom = pParent;

	//当前选择的功能
	m_pSelFunNode = NULL;

	//银行里的存款
	m_i64BankMoney = 0;

	if (bInMainRoom)
	{
		//用户ID
		m_dwOpUserID = ((CMainRoomEx*)pParent)->m_PlaceUserInfo.dwUserID;
		m_emParWndType = IsMainRoom;
		
	}
	else
	{
		//用户ID
		m_dwOpUserID = ((CGameRoomEx*)pParent)->GetMeUserInfo()->dwUserID;
		m_emParWndType = ISGameRoom;
	}

	//转账成功
	m_bTransferSuccess = true; 

	//记录表控件指针
	m_pRecordList = NULL;

	//存单记录表控件指针
	m_pDepList = NULL;

	m_mapRecord.clear();

	//转账日志记录表显示位置
	m_iRecDispPos = 0;

	//创建存单成功
	m_bDepositSuccess = true; 

	//存单记录表
	m_mapDeposits.clear();

	//计时用
	m_dwTickCount = ::GetTickCount();

	//是否定时检测
	m_bCheckFrequence = false;
	m_iMinutes = 0;
}

CPBankWndEx::~CPBankWndEx(void)
{
	m_mapRecord.clear();

	//存单记录表
	m_mapDeposits.clear();
}

LPCTSTR CPBankWndEx::GetWindowClassName() const 
{ 
	return _T("UIFrame");
};

UINT CPBankWndEx::GetClassStyle() const 
{ 
	return UI_CLASSSTYLE_DIALOG; 
};

void CPBankWndEx::OnFinalMessage(HWND hWnd) 
{ 
	m_pm.RemovePreMessageFilter(this);
};

LRESULT CPBankWndEx::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_pm.Init(m_hWnd);

	CDialogBuilder builder;
	CControlUI* pRoot = builder.Create(_T("BankWnd2\\BankWndEx.xml"), (UINT)0, NULL, &m_pm);
	ASSERT(pRoot && "Failed to parse XML");
	m_pm.AttachDialog(pRoot);
	m_pm.AddNotifier(this);

	::SetWindowPos(m_hWnd,CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);

	Init();

	CPBankWndEx::m_hInstanceWnd = m_hWnd;

	return 0;
}

LRESULT CPBankWndEx::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CPBankWndEx::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CPBankWndEx::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CPBankWndEx::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CPBankWndEx::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
	::ScreenToClient(*this, &pt);

	RECT rcClient;
	::GetClientRect(*this, &rcClient);

	RECT rcCaption = m_pm.GetCaptionRect();
	if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
		&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) 
	{
		CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(pt));
		if( pControl && _tcscmp(pControl->GetClass(), _T("ButtonUI")) != 0 )
			return HTCAPTION;
	}

	return HTCLIENT;
}

LRESULT CPBankWndEx::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CPBankWndEx::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = false;

	if (wParam == IDT_TIMEOUT_TO_LOGOUT)
	{
		if (m_bCheckFrequence)
		{
			int iMinutesPass = (::GetTickCount() - m_dwTickCount)/60000;

			if (iMinutesPass >= m_iMinutes)
			{
				::KillTimer(m_hWnd,IDT_TIMEOUT_TO_LOGOUT);
				Close();
				m_pRoom->PostMessage(WM_BANK_CLOSE,1,0);
				return 0;
			}			
		}
	}

	return 0;
}

LRESULT CPBankWndEx::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	switch(uMsg) 
	{
	case WM_CREATE:
		{
			lRes = OnCreate(uMsg, wParam, lParam, bHandled); 
		}
		break;

	case WM_DESTROY:       
		{
			lRes = OnDestroy(uMsg, wParam, lParam, bHandled);
		}
		break;

	case WM_NCACTIVATE:    
		{
			lRes = OnNcActivate(uMsg, wParam, lParam, bHandled);
		}
		break;

	case WM_NCCALCSIZE:    
		{
			lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled);
		}
		break;

	case WM_NCPAINT:       
		{
			lRes = OnNcPaint(uMsg, wParam, lParam, bHandled);
		}
		break;

	case WM_NCHITTEST:     
		{
			lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled);
		}
		break;

	case WM_TIMER:
		{
			lRes = OnTimer(uMsg, wParam, lParam, bHandled);
		}
		break;

	case WM_SIZE:          
		{
			lRes = OnSize(uMsg, wParam, lParam, bHandled);
		}
		break;

	default:
		bHandled = FALSE;
	}

	if(bHandled)
	{
		return lRes;
	}

	if(m_pm.MessageHandler(uMsg,wParam,lParam,lRes))
	{
		return lRes;
	}

	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CPBankWndEx::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	if( uMsg == WM_KEYDOWN ) 
	{
		if( wParam == VK_RETURN ) 
		{
			return true;
		}
		else if( wParam == VK_ESCAPE )
		{
			return true;
		}
	}
	return false;
}

LPCTSTR CPBankWndEx::GetItemText(CControlUI* pControl, int iIndex, int iSubItem)
{
	CString strBuffer;

	if (pControl->GetParent()->GetParent() == m_pRecordList) //转账记录表
	{
		std::map<int,MSG_GR_R_TransferRecord_t>::iterator it = m_mapRecord.find(m_iRecDispPos + iIndex);

		if (it == m_mapRecord.end())
		{
			return _T("");
		}

		MSG_GR_R_TransferRecord_t* pTRec = &it->second;

		if (pTRec == NULL)
		{
			return _T("");
		}

		if (pTRec->dwUserID == 0)
		{
			return _T("");
		}

		if (iSubItem == 0) //操作时间
		{
			strBuffer.Format(_T("%02d/%02d %02d:%02d"),
				pTRec->oleDateTime.GetMonth(),
				pTRec->oleDateTime.GetDay(),
				pTRec->oleDateTime.GetHour(),
				pTRec->oleDateTime.GetMinute());
			return strBuffer;
		}
		else if (iSubItem == 1)
		{
			strBuffer.Format(_T(" %d"),pTRec->dwUserID);
			return strBuffer;
		}
		else if (iSubItem == 2)
		{
			strBuffer.Format(_T(" %d"),pTRec->destUserID);
			return strBuffer;
		}
		else if (iSubItem == 3)
		{
			TCHAR szBuffer[32];
			GetNumString(szBuffer,pTRec->i64Money,Glb().m_nPowerOfGold,Glb().m_bUseSpace,Glb().m_strSpaceChar);
			strBuffer.Format(_T(" %s"),szBuffer);
			return strBuffer;
		}
	}

	return _T("");
}

void CPBankWndEx::Init() 
{
	//初始化游戏钱包数据
	InitGameWalletTable();

	TCHAR szCtrlName[32];

	//提交按钮
	m_pSubmitBnt = static_cast<CButtonUI*>(m_pm.FindControl("Btn_OK"));

	for (int i = 1; i <= SORT_NUMS; ++i)
	{
		//得到类型按钮指针
		::sprintf_s(szCtrlName,"Btn_Type_%d",i);
		m_pSortBnt[i - 1] = static_cast<CButtonUI*>(m_pm.FindControl(szCtrlName));

		//得到列表的容器指针
		::sprintf_s(szCtrlName,"Ctn_Type%d",i);
		m_pFunsCtn[i - 1] = static_cast<CContainerUI*>(m_pm.FindControl(szCtrlName));
		m_pFunsCtn[i - 1]->SetVisible(false);

		//得到功能列表按钮的指针,与对功能右则操作UI容器指针，通过TFunsNode绑定
		int idx = 0;
		CStdPtrArray* pCtrls = m_pm.FindSubControlsByClass(m_pFunsCtn[i - 1],_TEXT("ButtonUI"));
		CButtonUI* pBntUI = NULL;
		CControlUI* pImgCtrl = NULL;

		UINT uYPos = 2;

		do 
		{
			pBntUI = static_cast<CButtonUI*>(pCtrls->GetAt(idx++));

			if (pBntUI)
			{
				TFunsNode tagFunsNode;
				tagFunsNode.pFunsBnt = pBntUI;
				::sprintf_s(szCtrlName,"Ctn_Type%d_Ctn%d",i,idx);
				tagFunsNode.pFunsCtn = static_cast<CContainerUI*>(m_pm.FindControl(szCtrlName));
				m_lstFuns[i - 1].push_back(tagFunsNode);
				
				SIZE sz = {pBntUI->GetFixedXY().cx,uYPos};
				pBntUI->SetFixedXY(sz);

				::sprintf_s(szCtrlName,"Img_Type%d_Title%d",i,idx);
				pImgCtrl = m_pm.FindControl(szCtrlName);
				sz.cx = pImgCtrl->GetFixedXY().cx;
				sz.cy += 3;
				pImgCtrl->SetFixedXY(sz);

				if(pBntUI->GetName() == _T("Btn_Type1_Title3") || pBntUI->GetName() == _T("Btn_Type1_Title4"))
				{
					if(CGamePlaceApp::GetModuleStatus("Bank","TransferMoney",1) == 0)
					{
						pBntUI->SetVisible(false);
						pImgCtrl->SetVisible(false);
						continue;
					}
				}

				uYPos += pBntUI->GetFixedHeight();
			}

		} while (pBntUI);
	}

	//记录表控件指针
	m_pRecordList = static_cast<CListUI*>(m_pm.FindControl("Lst_S1_Ctn4_Record"));
	m_pRecordList->SetTextCallback(this);

	//存单记录表控件指针
	m_pDepList = static_cast<CListUI*>(m_pm.FindControl("Lst_S1_Ctn6_Record"));
	m_pDepList->SetTextCallback(this);

	CControlUI* pCtrl = NULL;

	int xPos = m_pSortBnt[0]->GetFixedXY().cx;
	int yPos = m_pSortBnt[0]->GetFixedXY().cy;
	int iW = m_pSortBnt[0]->GetFixedWidth();
	int iH = m_pSortBnt[0]->GetFixedHeight();

	pCtrl = m_pm.FindControl("VerticalLayoutUI1");

	xPos += pCtrl->GetX();
	yPos += pCtrl->GetY();

	for (int i = 0; i < SORT_NUMS; ++i)
	{
		RECT rc = {xPos,yPos,xPos + iW,yPos + iH};
		m_pSortBnt[i]->SetPos(rc);
		yPos += iH + 3;

		RECT rcLstCtn = {xPos - 1,yPos,xPos + m_pFunsCtn[i]->GetFixedWidth(),yPos + m_pFunsCtn[i]->GetFixedHeight()};
		m_pFunsCtn[i]->SetPos(rcLstCtn);
	}

	//关于取出金币
	Init_Ctn1_1();

	//关于存储金币
	Init_Ctn1_2();

	//关于转账
	Init_Ctn1_3();

	//关于修改密码
	Init_Ctn2_1();

	//关于存单办理
	Init_Ctn1_5();

	//关于充值
	Init_Ctn1_7();

	//关于安全设置
	Init_Ctn3_1();

	//加载常用控件的样式
	LoadNormalCtrlSkin();

	//加载各功能UI容器的消息函数
	LoadNotifyFunctions();

	ShowFuns(0);
}

//初始化游戏钱包数据
void CPBankWndEx::InitGameWalletTable()
{
	m_vecGmWallets.clear();

	CGameListCtrl* pGameList = &GetMainRoom()->m_pGameListWnd->m_GameList;

 	CAFCGameListItem * pGameKindItem = pGameList->GetKindParentItem(); //获取根项
 	for (INT_PTR i = 0; i < pGameKindItem->m_ItemPtrArray.GetCount(); ++i)
 	{
 		CAFCKindItem* pKindItem=(CAFCKindItem *)pGameKindItem->m_ItemPtrArray.GetAt(i);
		if (pKindItem!=NULL && pKindItem->m_KindInfo.uKindID != 999)
		{
			for (INT_PTR j = 0; j < pKindItem->m_ItemPtrArray.GetCount(); ++j)
			{
				CAFCNameItem* pNameItem=(CAFCNameItem*)pKindItem->m_ItemPtrArray.GetAt(j);
				if (pNameItem != NULL)
				{
					if (pNameItem->m_uDataType == GLK_GAME_NAME)
					{
						TGmWalletNode tagNode;
						tagNode.uNameID = pNameItem->m_NameInfo.uNameID;
						::strcpy(tagNode.szGameName,pNameItem->m_NameInfo.szGameName);
						tagNode.i64Wallet = 0;
						m_vecGmWallets.push_back(tagNode);
					}
				}
			}
		}
 	}
}

//关于取出金币
void CPBankWndEx::Init_Ctn1_1()
{
	CLabelUI*  pLabCtrl = NULL;
	CEditUI*   pEdtCtrl = NULL;
	CComboUI*  pCobCtrl = NULL;

	CBcfFile fMsg(CBcfFile::GetAppPath() + "ClientMessage.bcf");

	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_S1_Ctn1_OutCoint"));
	pEdtCtrl->SetMaxChar(23);

	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_S1_Ctn1_Pwd"));
	pEdtCtrl->SetMaxChar(20);

	pCobCtrl = static_cast<CComboUI*>(m_pm.FindControl("Cob_S1_Ctn1_Games"));
	pCobCtrl->SetEnabled(false);
	pCobCtrl->RemoveAll();

	vector<TGmWalletNode>::iterator it = m_vecGmWallets.begin();
	while(it != m_vecGmWallets.end())
	{
		CListLabelElementUI* p = new CListLabelElementUI();
		p->SetText(it->szGameName);
		pCobCtrl->Add(p);
		it++;
	}
}

//关于存入金币
void CPBankWndEx::Init_Ctn1_2()
{
	CLabelUI*  pLabCtrl = NULL;
	CEditUI*   pEdtCtrl = NULL;
	CComboUI*  pCobCtrl = NULL;

	CBcfFile fMsg(CBcfFile::GetAppPath() + "ClientMessage.bcf");

	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_S1_Ctn2_OutCoint"));
	pEdtCtrl->SetMaxChar(23);

	pCobCtrl = static_cast<CComboUI*>(m_pm.FindControl("Cob_S1_Ctn2_Game"));
	pCobCtrl->SetEnabled(false);
	pCobCtrl->RemoveAll();
}

//关于转账
void CPBankWndEx::Init_Ctn1_3()
{
	COptionUI* pOptCtrl = NULL;
	CEditUI*   pEdtCtrl = NULL;

	CBcfFile fMsg(CBcfFile::GetAppPath() + "ClientMessage.bcf");

	pOptCtrl = static_cast<COptionUI*>(m_pm.FindControl("Opt_S1_Ctn3_ID"));
	pOptCtrl->Selected(true);

	CLabelUI* pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_S1_Ctn3_1"));
	pLabCtrl->SetText(fMsg.GetKeyVal("PBankDlg","TransUserID","对方ID："));

	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_S1_Ctn3_TargetUser"));
	pEdtCtrl->SetMaxChar(32);

	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_S1_Ctn3_TransCoint"));
	pEdtCtrl->SetMaxChar(23);

	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_S1_Ctn3_Pwd"));
	pEdtCtrl->SetMaxChar(20);

}

//关于存单办理
void CPBankWndEx::Init_Ctn1_5()
{
	COptionUI* pOptCtrl = NULL;
	CEditUI*   pEdtCtrl = NULL;

	CBcfFile fMsg(CBcfFile::GetAppPath() + "ClientMessage.bcf");

	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_S1_Ctn5_OutCoint"));
	pEdtCtrl->SetMaxChar(23);

	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_S1_Ctn5_Pwd"));
	pEdtCtrl->SetMaxChar(20);

	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_S1_Ctn5_Suc_Serial"));
	pEdtCtrl->SetReadOnly(true);

	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_S1_Ctn5_Suc_Pwd"));
	pEdtCtrl->SetReadOnly(true);
}

//关于充值
void CPBankWndEx::Init_Ctn1_7()
{
	CEditUI*   pEdtCtrl = NULL;

	CBcfFile fMsg(CBcfFile::GetAppPath() + "ClientMessage.bcf");

	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_S1_Ctn7_TargetUser"));
	pEdtCtrl->SetMaxChar(16);

	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_S1_Ctn7_Serial"));
	pEdtCtrl->SetMaxChar(16);

	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_S1_Ctn7_Pwd"));
	pEdtCtrl->SetMaxChar(16);
}

//关于修改密码
void CPBankWndEx::Init_Ctn2_1()
{
	CLabelUI*  pLabCtrl = NULL;
	CEditUI*   pEdtCtrl = NULL;

	CBcfFile fMsg(CBcfFile::GetAppPath() + "ClientMessage.bcf");

	pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_S2_Ctn1_1"));
	pLabCtrl->SetText(fMsg.GetKeyVal("PBankDlg","OldPW","旧银行密码："));

	pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_S2_Ctn1_2"));
	pLabCtrl->SetText(fMsg.GetKeyVal("PBankDlg","NewPW","新银行密码："));

	pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_S2_Ctn1_3"));
	pLabCtrl->SetText(fMsg.GetKeyVal("PBankDlg","ConfirmPW","新密码确认："));

	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_S2_Ctn1_OldPwd"));
	pEdtCtrl->SetMaxChar(20);

	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_S2_Ctn1_NewPwd"));
	pEdtCtrl->SetMaxChar(20);

	pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_S2_Ctn1_PwdCfm"));
	pEdtCtrl->SetMaxChar(20);
}

//关于安全设置
void CPBankWndEx::Init_Ctn3_1()
{
	COptionUI* pOptCtrl = NULL;
	CComboUI* pCobCtrl = NULL;

	int nSelect = AfxGetApp()->GetProfileInt(TEXT("BankSafe"),TEXT("Option"),1);
	CString strMin = AfxGetApp()->GetProfileString(TEXT("BankSafe"),TEXT("Minutes"),"5");

	pCobCtrl = static_cast<CComboUI*>(m_pm.FindControl(_T("Cob_S3_Ctn1_Minutes")));
	LPCTSTR szItems[6] = {_TEXT("5"),_TEXT("15"),_TEXT("30"),_TEXT("60"),_TEXT("120")};
	LoadComboItems(pCobCtrl,szItems,5);
	pCobCtrl->SetEnabled(false);

	if (nSelect == 1)
	{
		pOptCtrl = static_cast<COptionUI*>(m_pm.FindControl("Opt_S3_Ctn1_1"));
		pOptCtrl->Selected(true);
	}
	else if (nSelect == 2)
	{
		pOptCtrl = static_cast<COptionUI*>(m_pm.FindControl("Opt_S3_Ctn1_2"));
		pOptCtrl->Selected(true);
		pCobCtrl->SetEnabled(true);

		//是否定时检测
		m_bCheckFrequence = true;
		m_iMinutes = _atoi64((const char*)strMin.GetBuffer());
	}
	else if (nSelect == 3)
	{
		pOptCtrl = static_cast<COptionUI*>(m_pm.FindControl("Opt_S3_Ctn1_3"));
		pOptCtrl->Selected(true);
	}

	::SetTimer(m_hWnd,IDT_TIMEOUT_TO_LOGOUT,5000,NULL);

	return;
}

//加载Combol的列表项
void CPBankWndEx::LoadComboItems(CComboUI* pCtrl,LPCTSTR items[],int nums,int nSelect/* = 0*/)
{
	if (!pCtrl || nums <= 0)
	{
		return;
	}

	for (int i = 0; i < nums; ++i)
	{
		CListLabelElementUI* p = new CListLabelElementUI();
		p->SetText(items[i]);
		pCtrl->Add(p);
	}

	return;
}

//根据文本设置选择列表项
bool CPBankWndEx::SelectComboItem(BzDui::CComboUI* pCtrl,LPCTSTR szText)
{
	if (!pCtrl)
	{
		return false;
	}

	int itNums = pCtrl->GetCount();

	if (itNums <= 0)
	{
		return false;
	}

	for (int i = 0; i < itNums; ++i)
	{
		if (strcmp(szText,pCtrl->GetItemAt(i)->GetText().GetData()) == 0)
		{
			pCtrl->SelectItem(i);
			return true;
		}
	}

	return false;
}

void CPBankWndEx::Notify(TNotifyUI& msg)
{
	m_dwTickCount = ::GetTickCount();

	if(msg.sType == _T("click")) 
	{
		//切换类型Tab页
		for (int i = 0; i < SORT_NUMS; ++i)
		{
			if (msg.pSender == m_pSortBnt[i])
			{
				ShowFuns(i);
				return;
			}
		}

		//点击Tab页列表项
		for (int i = 0; i < SORT_NUMS; ++i)
		{
			list<TFunsNode>::iterator it = m_lstFuns[i].begin();
			while(it != m_lstFuns[i].end())
			{
				if (msg.pSender == it->pFunsBnt)
				{
					if (m_pSelFunNode)
					{
						m_pSelFunNode->pFunsBnt->SetEnabled(true);
						if (m_pSelFunNode->pFunsCtn)
						{
							m_pSelFunNode->pFunsCtn->SetVisible(false);
						}
					}

					m_pSelFunNode = (TFunsNode*)&(*it);
					m_pSelFunNode->pFunsBnt->SetEnabled(false);
					if (m_pSelFunNode->pFunsCtn)
					{
						//显示容器
						m_pSelFunNode->pFunsCtn->SetVisible(true);

						//关于提交按钮的使用
						m_pSubmitBnt->SetVisible(m_pSelFunNode->bNeedSubmit);
						m_pSubmitBnt->SetText(m_pSelFunNode->sBntName);

						//向窗口发送打开消息
						if (m_pSelFunNode->pNotifyFunPtr)
						{
							TNotifyUI tagUIMsg;
							tagUIMsg.pSender = m_pSelFunNode->pFunsCtn;
							tagUIMsg.sType = "on_contain_open";
							(this->*(m_pSelFunNode->pNotifyFunPtr))(tagUIMsg);
						}
					}
					return;
				}
				it++;
			}
		}

		//提交按钮,把响应处理转到对应Notify_x_x函数中处理
		if(msg.pSender->GetName() == _T("Btn_OK"))
		{
			if (m_pSelFunNode)
			{
				if (m_pSelFunNode->pNotifyFunPtr)
				{
					(this->*(m_pSelFunNode->pNotifyFunPtr))(msg);
				}
			}
			return;
		}

		//关闭按钮
		if(msg.pSender->GetName() == _T("Btn_Close"))
		{
			::KillTimer(m_hWnd,IDT_TIMEOUT_TO_LOGOUT);
			Close();
			m_pRoom->PostMessage(WM_BANK_CLOSE,0,0);
			return;
		}

	}

	//功能UI操作容器消息响应检测
	CControlUI* pParentCtrl = msg.pSender->GetParent();
	for (int i = 0; i < SORT_NUMS; ++i)
	{
		list<TFunsNode>::iterator it = m_lstFuns[i].begin();
		while(it != m_lstFuns[i].end())
		{
			if (pParentCtrl == it->pFunsCtn) //找到控件的归属容器
			{
				//转发到相应的消息处理函数
				if (it->pNotifyFunPtr)
				{
					(this->*(it->pNotifyFunPtr))(msg);
				}
				return;
			}
			it++;
		}
	}

	return;

}


void CPBankWndEx::CenterWindow()
{
	RECT rc;
	::GetWindowRect(m_hWnd,&rc);
	int iSrcW = GetSystemMetrics(SM_CXSCREEN);
	int iSrcH = GetSystemMetrics(SM_CYSCREEN);
	int iWndW = rc.right - rc.left;
	int iWndH = rc.bottom - rc.top;
	::MoveWindow(m_hWnd,(iSrcW - iWndW)/2,(iSrcH - iWndH)/2,iWndW,iWndH,true);
}


//加载常规控件皮肤
void CPBankWndEx::LoadNormalCtrlSkin()
{
	int idx = 0;
	CStdPtrArray* pCtrls = NULL;

	for (int i = 1; i <= SORT_NUMS; ++i)
	{
		int idx = 0;
		pCtrls = m_pm.FindSubControlsByClass(m_pFunsCtn[i - 1],_TEXT("ButtonUI"));
		CButtonUI* pBntUI = NULL;
		do 
		{
			pBntUI = static_cast<CButtonUI*>(pCtrls->GetAt(idx++));
			if (pBntUI)
			{
				//修改功能按钮的样式
				pBntUI->SetHotImage(_TEXT("file='BankWnd2\\tree_bnt.png' dest='0,0,200,100' source='0,0,20,20'"));
				pBntUI->SetPushedImage(_TEXT("file='BankWnd2\\tree_bnt.png' dest='0,0,200,100' source='0,0,20,20'"));
				pBntUI->SetDisabledImage(_TEXT("file='BankWnd2\\tree_bnt.png' dest='0,0,200,100' source='0,20,20,40'"));
				pBntUI->SetDisabledTextColor(RGB(0xff,0xff,0xff));
			}
		} while (pBntUI);
	}

	pCtrls = m_pm.FindSubControlsByClass(m_pm.GetRoot(),_TEXT("EditUI"));
	CEditUI* pEditUI = NULL;
	RECT rcText = {4,1,4,0};
	do 
	{
		pEditUI = static_cast<CEditUI*>(pCtrls->GetAt(idx++));

		if (pEditUI)
		{
			if (!pEditUI->IsReadOnly())
			{
				pEditUI->SetNormalImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,0,500,22' corner='3,4,2,2'");
				pEditUI->SetHotImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,22,500,44' corner='3,4,2,2'");
				pEditUI->SetFocusedImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,44,500,66' corner='3,4,2,2'");
				pEditUI->SetDisabledImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,66,500,88' corner='3,4,2,2'");
			}
			else
			{
				pEditUI->SetNormalImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,66,500,88' corner='3,4,2,2'");
				pEditUI->SetHotImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,66,500,88' corner='3,4,2,2'");
				pEditUI->SetFocusedImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,66,500,88' corner='3,4,2,2'");
				pEditUI->SetDisabledImage("file='dialog\\edit.png' dest='0,0,500,22' source='0,66,500,88' corner='3,4,2,2'");
			}
			
			pEditUI->SetTextPadding(rcText);
		}
	} while (pEditUI);

	idx = 0;
	pCtrls = m_pm.FindSubControlsByClass(m_pm.GetRoot(),_TEXT("ComboUI"));
	CComboUI* pComboUI = NULL;
	RECT rc = {6,0,0,0};
	do 
	{
		pComboUI = static_cast<CComboUI*>(pCtrls->GetAt(idx++));
		if (pComboUI)
		{
			pComboUI->SetNormalImage("file='dialog\\combo.png' dest='0,0,500,22' source='0,0,500,22' corner='5,0,22,0'");
			pComboUI->SetHotImage("file='dialog\\combo.png' dest='0,0,500,22' source='0,22,500,44' corner='5,0,22,0'");
			pComboUI->SetPushedImage("file='dialog\\combo.png' dest='0,0,500,22' source='0,44,500,66' corner='5,0,22,0'");
			pComboUI->SetDisabledImage("file='dialog\\combo.png' dest='0,0,500,22' source='0,66,500,88' corner='5,0,22,0'");
			pComboUI->EnableScrollBar(true,false);
			pComboUI->SetItemTextPadding(rc);
		}
	} while (pComboUI);

	//加载所有多选框/单选框的皮肤
	idx = 0;
	pCtrls = m_pm.FindSubControlsByClass(m_pm.GetRoot(),_TEXT("OptionUI"));
	COptionUI* pOptionUI = NULL;
	do 
	{
		pOptionUI = static_cast<COptionUI*>(pCtrls->GetAt(idx++));
		if (pOptionUI)
		{
			if (lstrlen(pOptionUI->GetGroup()) == 0) //多选框
			{
				pOptionUI->SetNormalImage("file='dialog\\check.png' dest='0,0,15,15' source='0,0,15,15' corner='0,0,0,0'");
				pOptionUI->SetHotImage("file='dialog\\check.png' dest='0,0,15,15' source='15,0,30,15' corner='0,0,0,0'");
				pOptionUI->SetSelectedImage("file='dialog\\check.png' dest='0,0,15,15' source='30,0,45,15' corner='0,0,0,0'");
				RECT rc = {9,0,0,0};
				pOptionUI->SetTextPadding(rc);
			}
			else  //单选框
			{
				pOptionUI->SetNormalImage("file='dialog\\radio.png' dest='0,0,15,15' source='0,0,15,15' corner='0,0,0,0'");
				pOptionUI->SetHotImage("file='dialog\\radio.png' dest='0,0,15,15' source='15,0,30,15' corner='0,0,0,0'");
				pOptionUI->SetSelectedImage("file='dialog\\radio.png' dest='0,0,15,15' source='30,0,45,15' corner='0,0,0,0'");
				RECT rc = {18,0,0,0};
				pOptionUI->SetTextPadding(rc);
			}
			
		}
	} while (pOptionUI);

}

//为某一项功能UI容器绑定消息响应函数
//@parm uSortIdx 类型索引\
        strCtnName 功能的在窗口右则的封装容器名称\
		lpFunPtr 消息接收的函数\
		bNeedSubmit 是否需要提交按钮
void CPBankWndEx::BindNotify(UINT uSortIdx,BzDui::CStdString strCtnName,lpNotifyFun lpFunPtr,bool bNeedSubmit/* = true*/,BzDui::CStdString strBntName/* = "提交"*/)
{
	list<TFunsNode>::iterator it = m_lstFuns[uSortIdx].begin();
	while(it != m_lstFuns[uSortIdx].end())
	{
		if (strCtnName == it->pFunsCtn->GetName())
		{
			it->pNotifyFunPtr = lpFunPtr;
			it->bNeedSubmit = bNeedSubmit;
			it->sBntName = strBntName;
			break;
		}
		it++;
	}
}

//展开某类型子项功能列表
void CPBankWndEx::ShowFuns(int nIdx)
{
	CControlUI* pCtrl = NULL;

	int xPos = m_pSortBnt[0]->GetFixedXY().cx;
	int yPos = m_pSortBnt[0]->GetFixedXY().cy;
	int iW = m_pSortBnt[0]->GetFixedWidth();
	int iH = m_pSortBnt[0]->GetFixedHeight();

	//nIdx以上的类型按钮往上靠
	pCtrl = m_pm.FindControl("VerticalLayoutUI1");
	xPos += pCtrl->GetX();
	yPos += pCtrl->GetY();

	for (int i = 0; i <= nIdx; ++i)
	{
		RECT rc = {xPos,yPos,xPos + iW,yPos + iH};
		m_pSortBnt[i]->SetPos(rc);
		yPos += iH + 3;
	}

	//nIdx以上的类型按钮往下靠
	pCtrl = m_pm.FindControl("VerticalLayoutUI1");
	yPos = pCtrl->GetY() + pCtrl->GetFixedHeight() - (iH + 3);

	for (int i = SORT_NUMS - 1; i > nIdx; --i)
	{
		RECT rc = {xPos,yPos,xPos + iW,yPos + iH};
		m_pSortBnt[i]->SetPos(rc);
		yPos -= (iH + 3);
	}

	//功能列表显示在中间
	for (int i = 0; i < SORT_NUMS; ++i)
	{
		m_pFunsCtn[i]->SetVisible(i == nIdx);
	}	

	//每次点击类型按钮后默认选择显示第一个功能
	list<TFunsNode>::iterator it = m_lstFuns[nIdx].begin();

	if (it != m_lstFuns[nIdx].end())
	{
		TNotifyUI tagNotify;
		tagNotify.sType = "click";
		tagNotify.pSender = it->pFunsBnt;
		Notify(tagNotify);
	}
	
}

//金额转中文大写
void CPBankWndEx::ChangeDigitToChinese(__int64 i64Num, CString &strResult)
{
	int i,size,high;

	strResult = "";
	digitArray.clear();
	i = 0;
	size = 0;

	//将数值的数字提出来放在数组中，逆序排序
	while(i64Num)
	{
		digitArray.push_back(i64Num%10);
		i64Num /=10;
		size++;
	}

	for(i = size-1; i >= 0; i--)
	{
		//加入数字
		if(digitArray[i] == 0 )//数字为零情况处理
		{
			if(i-1 == -1) break; 
			if(digitArray[i-1] != 0 && (szUnit[i] != "万" && szUnit[i] != "亿" )) //非亿、万位为0时处理，不连续时加入零。
				strResult += szDigit[digitArray[i]];
		}
		else 
			strResult += szDigit[digitArray[i]];		

		//加入单位
		if(digitArray[i] == 0 && (szUnit[i] != "万" && szUnit[i] != "亿" ) ) //非亿、万位为0时跳过
			continue; 
		strResult += szUnit[i];
	}
	if(strResult.Find("亿万") != -1)//字符串中有亿万连续的特殊处理
		strResult.Replace("亿万", "亿");
}

//把数字输入框，在输入过程中同步转换为中文大写
void CPBankWndEx::ChangeEtToChinese(BzDui::CEditUI* pEditCtrl,BzDui::CTextUI* pTextCtrl)
{
	CString strOutCoin = pEditCtrl->GetText();

	//去除输入框中的分隔符号
	if(Glb().m_bUseSpace)
		strOutCoin.Replace(Glb().m_strSpaceChar, "");	//转帐页面		

	//验证字符串是否是0-9的数字
	for(int i = 0; i < strOutCoin.GetLength(); i++)
	{
		if(strOutCoin[i] < '0' || strOutCoin[i] > '9')
		{
			pEditCtrl->SetText("");
			return;
		}
	}

	__int64 i64Num  = _tstoi64(strOutCoin.GetBuffer());
	TCHAR szNum[128];

	if(bFlag == false)	
	{
		//字符串中加入分隔符
		GetNumString(szNum, i64Num, 0, Glb().m_bUseSpace, Glb().m_strSpaceChar);
		bFlag = true;
		pEditCtrl->SetText(szNum);
		//设置光标到最后面
		pEditCtrl->SetSel(-1);
	}
	else
		bFlag = false;

	ChangeDigitToChinese(i64Num, strOutCoin);	

	pTextCtrl->SetText(strOutCoin);

	return;
}

//通过游戏NameID,得到游戏的名字
char* CPBankWndEx::GetGameNameById(UINT uNameID)
{
	vector<TGmWalletNode>::iterator it = m_vecGmWallets.begin();
	while(it != m_vecGmWallets.end())
	{
		if (it->uNameID == uNameID)
		{
			return it->szGameName;
		}
		it++;
	}

	return NULL;
}

//通过游戏Name,得到游戏的NameID
UINT CPBankWndEx::GetGameIDByName(LPCTSTR pGameName)
{
	vector<TGmWalletNode>::iterator it = m_vecGmWallets.begin();
	while(it != m_vecGmWallets.end())
	{
		if (strcmp(it->szGameName,pGameName) == 0)
		{
			return it->uNameID;
		}
		it++;
	}

	return 0;
}

//通过游戏NameID,得到游戏的索引
int CPBankWndEx::GetWalletIndexById(UINT uNameID)
{
	for (int i = 0; i < m_vecGmWallets.size(); ++i)
	{
		if (uNameID == m_vecGmWallets[i].uNameID)
		{
			return i;
		}
	}

	return -1;
}

//通过游戏NameID,设置其钱包的钱
bool CPBankWndEx::SetGameWalletMoney(UINT uNameID,__int64 NewMoney)
{
	vector<TGmWalletNode>::iterator it = m_vecGmWallets.begin();
	while(it != m_vecGmWallets.end())
	{
		if (it->uNameID == uNameID)
		{
			it->i64Wallet = NewMoney;
			return true;
		}
		it++;
	}

	return false;
}

//通过游戏NameID,设置其钱包添加钱
bool CPBankWndEx::AddGameWalletMoney(UINT uNameID,__int64 AddMoney)
{
	vector<TGmWalletNode>::iterator it = m_vecGmWallets.begin();
	while(it != m_vecGmWallets.end())
	{
		if (it->uNameID == uNameID)
		{
			it->i64Wallet += AddMoney;
			return true;
		}
		it++;
	}

	return false;
}

//通过游戏Name,获取钱包的钱
__int64 CPBankWndEx::GetGameWalletMoney(LPCTSTR pGameName)
{
	vector<TGmWalletNode>::iterator it = m_vecGmWallets.begin();
	while(it != m_vecGmWallets.end())
	{
		if (strcmp(it->szGameName,pGameName) == 0)
		{
			return it->i64Wallet;
		}
		it++;
	}

	return 0;
}

//通过游戏NameID,获取钱包的钱
__int64 CPBankWndEx::GetGameWalletMoney(UINT uNameID)
{
	vector<TGmWalletNode>::iterator it = m_vecGmWallets.begin();
	while(it != m_vecGmWallets.end())
	{
		if (it->uNameID == uNameID)
		{
			return it->i64Wallet;
		}
		it++;
	}

	return 0;
}

///
/// 设置财富信息
/// @param nBank 银行存款
/// @param nWallet 手中现金
/// @return void
///
void CPBankWndEx::SetWealthInfor(__int64 i64Bank, __int64 i64Wallet)
{
	m_i64BankMoney = i64Bank;
	UpdateWealthInfor();
}

///
/// 提示
/// @param pToCtrl 在某一控件上提示
/// @param lpSecName ClientMessage.bcf中的段名
/// @param lpKeyName ClientMessage.bcf中的键值
/// @param lpDefault ClientMessage.bcf没有找到键值时，使用的默认值
/// @return void
///
void CPBankWndEx::ShowTipMessage(BzDui::CControlUI* pToCtrl,LPCTSTR lpSecName,LPCTSTR lpKeyName,LPCTSTR lpDefault,...)
{
	char szFormat[BUFSIZ];
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	strcpy(szFormat,fMsg.GetKeyVal(lpSecName,lpKeyName,lpDefault));

	va_list pArg;
	va_start(pArg,lpDefault);
	char szMessage[BUFSIZ];
	
	#ifdef WIN32
		_vsnprintf(szMessage,BUFSIZ - 1,szFormat,pArg);
	#else
		vsnprintf(szMessage,BUFSIZ - 1,szFormat,pArg);
	#endif

	va_end(pArg);

	if (pToCtrl)
	{
		;
	}
	else
	{
		DUIOkeyMsgBox(CPBankWndEx::m_hInstanceWnd,"%s",szMessage);
	}

	return;

}

//根据用户ID获取昵称
bool CPBankWndEx::OnIDKillFocus(BzDui::CEditUI* pEdtCtrl,BYTE nType,BYTE nUserful)
{
	CLabelUI* pLabCtrl = NULL;
	CString strTemp = "";

	// 对方ID
	strTemp = pEdtCtrl->GetText();

	if (strTemp.GetLength() == 0)
	{
		return true;
	}

	GetNickNameID_t* p = new GetNickNameID_t;

	if (!p)
	{
		return true;
	}

	if(nType == NICK_ID_BYID) //通过ID
	{
		// 无效的输入
		if(atol(strTemp) <= 0)
		{
			return false;
		}

		p->_user_id = atol(strTemp);
	}
	else if(nType == NICK_ID_BYNICKNAME) //通过昵称
	{
		strcpy(p->_nickname,(const char*)strTemp.GetBuffer());
	}
	else
	{
		return true;
	}

	p->_nType = nType;
	p->_nUseful = nUserful;

	GetMainRoom()->PostMessage(WM_BANK_GET_NICKNAMEID,0,LPARAM(p));

	return true;
	//WM_BANK_GET_NICKNAME_ONID
}

/// 转帐成功后更新银行数据
/// @param Opt 0-表示转出给别人，1-表示由别人转入给自己
/// @param nMoney 表示本次所操作的金币数
void CPBankWndEx::UpdateUserMoneyByTransfer(int OptType, __int64 i64Money)
{
	if( OptType == 0 ) //取出
	{
		m_i64BankMoney	-= i64Money;
	}
	else //存入
	{
		m_i64BankMoney	+= i64Money;
	}

	UpdateWealthInfor();

}

///
/// 收到一条转账记录
/// @param void
/// @return void
///
void CPBankWndEx::OnReceivedTransferRecord(MSG_GR_R_TransferRecord_t *pRecord)
{
	m_mapRecord[pRecord->nSeriNo] = *pRecord;
	m_pRecordList->NeedUpdate();
	UpdateRecordPageState();
}

///刷新记录的页码
void CPBankWndEx::UpdateRecordPageState()
{

	int iPageCount = ((m_mapRecord.size() % RECORD_PAGE_SIZE) == 0) ? 
		m_mapRecord.size() / RECORD_PAGE_SIZE : m_mapRecord.size() / RECORD_PAGE_SIZE + 1;
	int iCurrPage = m_iRecDispPos / RECORD_PAGE_SIZE + 1;

	CLabelUI* pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_S1_Ctn4_4"));
	TCHAR szBuffer[32];
	_stprintf(szBuffer,_T("第%d/%d页"),iCurrPage,iPageCount);
	pLabCtrl->SetText(szBuffer);

	//翻页控件按钮
	CButtonUI* pBntCtrl = NULL;
	pBntCtrl = static_cast<CButtonUI*>(m_pm.FindControl("Bnt_S1_Ctn4_First"));
	pBntCtrl->SetEnabled(iCurrPage > 1);
	pBntCtrl = static_cast<CButtonUI*>(m_pm.FindControl("Bnt_S1_Ctn4_Previous"));
	pBntCtrl->SetEnabled(iCurrPage > 1);
	pBntCtrl = static_cast<CButtonUI*>(m_pm.FindControl("Bnt_S1_Ctn4_Next"));
	pBntCtrl->SetEnabled(iCurrPage < iPageCount);
	pBntCtrl = static_cast<CButtonUI*>(m_pm.FindControl("Bnt_S1_Ctn4_Rear"));
	pBntCtrl->SetEnabled(iCurrPage < iPageCount);
}

///更新用户ID昵称的查询结果
void CPBankWndEx::UpdateNicknameIDResult(GetNickNameID_t* pNID)
{
	CLabelUI* pLabCtrl = NULL;
	CLabelUI* pLabCtrl_temp = NULL;
	TCHAR szText[64];

	if (pNID->_nUseful == NICK_ID_USE_TO_TRANSFER)
	{
		pLabCtrl_temp = static_cast<CLabelUI*>(m_pm.FindControl("Lab_S1_Ctn3_NickNameID_temp"));
		pLabCtrl_temp->SetText("");

		pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_S1_Ctn3_NickNameID"));

		if (pNID->_nType == NICK_ID_BYID)
		{
			if (strlen(pNID->_nickname) == 0)
			{
				pLabCtrl->SetText(_T("(玩家不存在)"));
			}
			else
			{
				_stprintf(szText,"(昵称：%s)",pNID->_nickname);
				pLabCtrl->SetText(szText);
				pLabCtrl_temp->SetText(pNID->_nickname);
			}
		}
		else if (pNID->_nType == NICK_ID_BYNICKNAME)
		{
			if (pNID->_user_id == 0)
			{
				pLabCtrl->SetText(_T("(玩家不存在)"));
			}
			else
			{
				_stprintf(szText,"(ID：%d)",pNID->_user_id);
				pLabCtrl->SetText(szText);
				_stprintf(szText,"%d",pNID->_user_id);
				pLabCtrl_temp->SetText(szText);
			}
		}
		
	}
	else if (pNID->_nUseful == NICK_ID_USE_TO_DEPOSIT)
	{
		pLabCtrl_temp = static_cast<CLabelUI*>(m_pm.FindControl("Lab_S1_Ctn7_NickNameID_temp"));
		pLabCtrl_temp->SetText("");

		pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_S1_Ctn7_NickNameID"));

		if (pNID->_nType == NICK_ID_BYID)
		{
			if (strlen(pNID->_nickname) == 0)
			{
				pLabCtrl->SetText(_T("(玩家不存在)"));
			}
			else
			{
				_stprintf(szText,"(昵称：%s)",pNID->_nickname);
				pLabCtrl->SetText(szText);
				pLabCtrl_temp->SetText(pNID->_nickname);
			}
		}
		else if (pNID->_nType == NICK_ID_BYNICKNAME)
		{
			return;
		}
	}

	return;
}

///
/// 更新财富信息
/// @return void
///
void CPBankWndEx::UpdateWealthInfor()
{
	TCHAR szNum[128] = {0};
	TCHAR szTemp[128] = {0};
	TCHAR szBank[128] = {0};
	GetNumString(szTemp,m_i64BankMoney,Glb().m_nPowerOfGold,Glb().m_bUseSpace,Glb().m_strSpaceChar);
	_stprintf(szBank,_T("<b>%s</b>"),szTemp);

	CLabelUI* pLabCtrl = NULL;
	CEditUI*  pEdtCtrl = NULL;
	CComboUI* pCobCtrl = NULL;

	//银行保险箱
	pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_S1_Ctn1_Bank"));
	pLabCtrl->SetText(szBank);

	pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_S1_Ctn3_Bank"));
	pLabCtrl->SetText(szBank);

	pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_S1_Ctn5_Bank"));
	pLabCtrl->SetText(szBank);

	//更新取款显示的游戏钱包
	pCobCtrl = static_cast<CComboUI*>(m_pm.FindControl("Cob_S1_Ctn1_Games"));
	if (pCobCtrl->GetCurSel() != -1)
	{
		GetNumString(szNum,m_vecGmWallets[pCobCtrl->GetCurSel()].i64Wallet,
			Glb().m_nPowerOfGold,Glb().m_bUseSpace,Glb().m_strSpaceChar);
		pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Lab_S1_Ctn1_GameCoint"));
		pEdtCtrl->SetText(szNum);
	}

	//计算总财富
	__int64 i64TotalMoney = m_i64BankMoney;
	vector<TGmWalletNode>::iterator it = m_vecGmWallets.begin();
	while(it != m_vecGmWallets.end())
	{
		if (it->i64Wallet > 0)
		{
			i64TotalMoney += it->i64Wallet;
		}
		it++;
	}

	pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_All_Money"));
	GetNumString(szTemp,i64TotalMoney,Glb().m_nPowerOfGold,Glb().m_bUseSpace,Glb().m_strSpaceChar);
	pLabCtrl->SetText(szTemp);


	//重新初始显示取款用的游戏选择列表，因这m_i64BankMoney有变动，有可能使用选择表有变化
	pCobCtrl = static_cast<CComboUI*>(m_pm.FindControl("Cob_S1_Ctn2_Game"));
	int nLastSel = -1,nIdx = 0;
	BzDui::CStdString sLastSelName = " ";
	if (pCobCtrl->GetCurSel() != -1)
	{
		pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_S1_Ctn2_GameCoint"));
		pLabCtrl->SetText("");

		sLastSelName = pCobCtrl->GetText();
	}

	pCobCtrl->SetEnabled(false);
	pCobCtrl->RemoveAll();

	it = m_vecGmWallets.begin();
	while(it != m_vecGmWallets.end())
	{
		if (it->i64Wallet > 0)
		{
			CListLabelElementUI* p = new CListLabelElementUI();
			p->SetText(it->szGameName);
			pCobCtrl->Add(p);

			if (strcmp(sLastSelName.GetData(),it->szGameName) == 0)
			{
				nLastSel = nIdx;
			}
			nIdx++;
		}
		it++;
	}

	pCobCtrl->SetEnabled(true);

	if (nLastSel != -1)
	{
		pCobCtrl->SelectItem(nLastSel);
	}

	return;
}


/*------------------------------------------------------------------------------------
	以下内容是功能相关UI操作的具体实现。
	控件消息已按功能项区分，消息已分发到Notify_x_x函数
	开发人员需要自行在LoadNotifyFunctions函数制定消息分发方法

  ----------------------------------------------------------------------------------*/

//加载各功能UI容器的消息函数
void CPBankWndEx::LoadNotifyFunctions()
{
	BindNotify(0,_T("Ctn_Type1_Ctn1"),&CPBankWndEx::Notify_1_1,true);
	BindNotify(0,_T("Ctn_Type1_Ctn2"),&CPBankWndEx::Notify_1_2,true);
	BindNotify(0,_T("Ctn_Type1_Ctn3"),&CPBankWndEx::Notify_1_3,true);
	BindNotify(0,_T("Ctn_Type1_Ctn4"),&CPBankWndEx::Notify_1_4,false);
	BindNotify(0,_T("Ctn_Type1_Ctn5"),&CPBankWndEx::Notify_1_5,true);
	BindNotify(0,_T("Ctn_Type1_Ctn6"),&CPBankWndEx::Notify_1_6,false);
	BindNotify(0,_T("Ctn_Type1_Ctn7"),&CPBankWndEx::Notify_1_7,true);
	BindNotify(1,_T("Ctn_Type2_Ctn1"),&CPBankWndEx::Notify_2_1,true);
	BindNotify(2,_T("Ctn_Type3_Ctn1"),&CPBankWndEx::Notify_3_1,true,_T("保存"));
}

//“类型1”中的“标题1”消息处理函数
void CPBankWndEx::Notify_1_1(BzDui::TNotifyUI& msg)
{
	if (msg.sType == "click")
	{
		if (msg.pSender->GetName() == _T("Btn_S1_Ctn1_All")) //全部
		{
			TCHAR szNum[128];
			GetNumString(szNum,m_i64BankMoney,Glb().m_nPowerOfGold,Glb().m_bUseSpace,Glb().m_strSpaceChar);
			CEditUI* pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_S1_Ctn1_OutCoint"));
			pEdtCtrl->SetFocus();
			pEdtCtrl->SetText(szNum);
			return;
		}
		else if (msg.pSender->GetName() == _T("Btn_OK")) //提交
		{
			CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
			CEditUI* pOutCointEdt = static_cast<CEditUI*>(m_pm.FindControl("Edt_S1_Ctn1_OutCoint"));

			CComboUI* pCobCtrl = static_cast<CComboUI*>(m_pm.FindControl("Cob_S1_Ctn1_Games"));
			if (pCobCtrl->GetCurSel() == -1)
			{
				DUIOkeyMsgBox(m_hWnd,"请选择接收游戏。");
				pCobCtrl->SetFocus();
				return;
			}

			UINT uGameID = m_vecGmWallets[pCobCtrl->GetCurSel()].uNameID;

			//如果游戏别的房间已打开，不予以操作
			for (BYTE i = 0; i < MAX_GAME_ROOM; ++i)
			{
				// 游戏中游戏界面右边消息栏
				CGameRoomEx* pGameRoomEx = (CGameRoomEx*)GetMainRoom()->m_RoomInfo[i].pGameRoomWnd;
				if (m_pRoom == pGameRoomEx) //发起消息的房间无需再更新金币数
				{
					continue;
				}
				if (NULL != pGameRoomEx)
				{
					if (pGameRoomEx->m_GameInfo.uNameID == uGameID)
					{
						ShowTipMessage(NULL,"PBankWnd2","OutSv_Room_NoRule",
							"游戏【%s】房间已打开，不予取款，如有需要，请到相应应房间里使用银行进行取款。",
							pGameRoomEx->m_GameInfo.szGameName);
						return;
					}
				}	
			}

			TCHAR szMsg[256];
			CString strNum = pOutCointEdt->GetText();
			CString str;

			TCHAR szTail[16];
			_tcscpy(szTail, TEXT(""));
			for (int i=0; i<Glb().m_nPowerOfGold; ++i)
			{
				_tcscat( szTail, TEXT("0"));
			}

			//如果有分隔符，则去除字符串里的分隔符
			if(Glb().m_bUseSpace)
				strNum.Replace(Glb().m_strSpaceChar,"");

			/// 如果没有输入
			if (strNum.GetLength() == 0)
			{	
				ShowTipMessage(NULL,"PBankWnd2","OutSv_Msg_NotInputCoin",
					"请输入您要存取款的数额。");
				pOutCointEdt->SetFocus();
				return;
			}

			/// 如果输入太短
			if (strNum.GetLength()<Glb().m_nPowerOfGold)
			{
				ShowTipMessage(NULL,"PBankWnd2","OutSv_Msg_NotGetCoin",
					"您输入的金币数额不被系统接受，请输入至少%d位，且末%d位为 %s 的正整数！",
					Glb().m_nPowerOfGold+1,Glb().m_nPowerOfGold,szTail);
				pOutCointEdt->SetFocus();
				return;
			}
			/// 如果输入太长
			if (strNum.GetLength()>18 + Glb().m_nPowerOfGold)
			{
				ShowTipMessage(NULL,"PBankWnd2","OutSv_Msg_NumberTooLarge",
					"您输入的数字太大，超出允许范围，请输入不大于%I64d%s，且末%d位为 %s 的正整数！",
					0x3FFFFFFFFFFFFFFF,szTail,Glb().m_nPowerOfGold,szTail);
				pOutCointEdt->SetFocus();
				return;
			}

			/// 如果末几位不是零
			CString strSubNum = strNum.Right(Glb().m_nPowerOfGold);
			if (strSubNum != szTail)
			{
				ShowTipMessage(NULL,"PBankWnd2","OutSv_Msg_NotGetCoin2",
					"您输入的金币数额不被系统接受，请输入末%d位为 %s 的正整数！",
					Glb().m_nPowerOfGold, szTail);
				pOutCointEdt->SetFocus();
				return;
			}
			/// 截取前N位
			strSubNum = strNum.Left(strNum.GetLength()-Glb().m_nPowerOfGold);
			__int64 i64Num = _atoi64(strSubNum.GetBuffer());
			if (i64Num>0x3FFFFFFFFFFFFFFF)
			{
				ShowTipMessage(NULL,"PBankWnd2","OutSv_Msg_NumberTooLarge",
					"您输入的数字太大，超出允许范围，请输入不大于%I64d%s，且末%d位为 %s 的正整数！",
					0x3FFFFFFFFFFFFFFF,szTail,Glb().m_nPowerOfGold,szTail);
				pOutCointEdt->SetFocus();
				return;
			}
			if (i64Num<=0)
			{
				ShowTipMessage(NULL,"PBankWnd2","OutSv_Msg_NotGetCoin",
					"您输入的金币数额不被系统接受，请输入至少%d位，且末%d位为 %s 的正整数！",
					Glb().m_nPowerOfGold+1,Glb().m_nPowerOfGold,szTail);
				pOutCointEdt->SetFocus();
				return;
			}
			if (i64Num > m_i64BankMoney)
			{
				ShowTipMessage(NULL,"","","您输入的数字已大于超出银行的存额");
				pOutCointEdt->SetFocus();
				return;
			}

			CEditUI* pPwdEdt = static_cast<CEditUI*>(m_pm.FindControl("Edt_S1_Ctn1_Pwd"));
			if (pPwdEdt->GetText().GetLength() == 0)
			{
				ShowTipMessage(NULL,"","","请输入银行密码");
				pPwdEdt->SetFocus();
				return;
			}

			/// 发消息
			if (m_pRoom != NULL)
			{
				CComboUI* pCobCtrl = static_cast<CComboUI*>(m_pm.FindControl("Cob_S1_Ctn1_Games"));

				bank_op_normal* p = new bank_op_normal;
				p->_user_id = m_dwOpUserID;
				p->_operate_type = 1;
				p->_game_id = uGameID;
				p->_money = i64Num;

				
				//提取输入的密码
				CGameRoomEx::GetCryptedPasswd(p->_szMD5Pass,(TCHAR*)pPwdEdt->GetText().GetData());

				m_pRoom->PostMessage(WM_BANK,0,(LPARAM)p);
			}
			
			return;
		}
	}
	else if (msg.sType == _T("textchanged"))
	{
		if (msg.pSender->GetName() == _T("Edt_S1_Ctn1_OutCoint"))
		{
			//转账金额小写转换为中文大写
			ChangeEtToChinese(static_cast<CEditUI*>(msg.pSender),
								static_cast<CTextUI*>(m_pm.FindControl("Txt_S1_Ctn1_Caps")));
			return;
		}
	}
	else if (msg.sType == _T("itemselect"))
	{
		if( msg.pSender->GetName() == _T("Cob_S1_Ctn1_Games")) 
		{
			TCHAR szNum[128];
			__int64 i64GameMoney = m_vecGmWallets[msg.wParam].i64Wallet;
//			UINT uGameID = m_vecGmWallets[msg.wParam].uNameID;
			GetNumString(szNum,i64GameMoney,Glb().m_nPowerOfGold,Glb().m_bUseSpace,Glb().m_strSpaceChar);
			CEditUI* pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Lab_S1_Ctn1_GameCoint"));
			pEdtCtrl->SetText(szNum);

			//如果游戏别的房间已打开，不予以操作
// 			for (BYTE i = 0; i < MAX_GAME_ROOM; ++i)
// 			{
// 				// 游戏中游戏界面右边消息栏
// 				CGameRoomEx* pGameRoomEx = (CGameRoomEx*)GetMainRoom()->m_RoomInfo[i].pGameRoomWnd;
// 				if (m_pRoom == pGameRoomEx) //发起消息的房间无需再更新金币数
// 				{
// 					continue;
// 				}
// 				if (NULL != pGameRoomEx)
// 				{
// 					if (pGameRoomEx->m_GameInfo.uNameID == uGameID)
// 					{
// 						ShowTipMessage(NULL,"PBankWnd2","OutSv_Room_NoRule",
// 							"游戏【%s】房间已打开，不予取款，如有需要，请到相应应房间里使用银行进行取款。",
// 							pGameRoomEx->m_GameInfo.szGameName);
// 						return;
// 					}
// 				}	
// 			}

			return;
		}
	}
}

//“类型1”中的“标题2”消息处理函数
void CPBankWndEx::Notify_1_2(BzDui::TNotifyUI& msg)
{
	if (msg.sType == "click")
	{
		if (msg.pSender->GetName() == _T("Btn_S1_Ctn2_All")) //全部
		{
			CComboUI* pCobCtrl = static_cast<CComboUI*>(m_pm.FindControl("Cob_S1_Ctn2_Game"));
			if (pCobCtrl->GetCurSel() == -1)
			{
				return;
			}

			LPCTSTR lpGameName = pCobCtrl->GetText().GetData();
			UINT uGameID = GetGameIDByName(lpGameName);
			__int64 i64GameWallet = GetGameWalletMoney(lpGameName);

			//选择存款的游戏就是当前房间的游戏一至时，存款有限制
			if (ISGameRoom == m_emParWndType)
			{
				CGameRoomEx* pRoomEx = (CGameRoomEx*)m_pRoom;
				if (pRoomEx->m_GameInfo.uNameID == uGameID)
				{
					if (i64GameWallet >= pRoomEx->m_GameInfo.uLessPoint)
					{
						i64GameWallet -= pRoomEx->m_GameInfo.uLessPoint;
					}
				}
			}

			TCHAR szNum[128];
			GetNumString(szNum,i64GameWallet,Glb().m_nPowerOfGold,Glb().m_bUseSpace,Glb().m_strSpaceChar);
			CEditUI* pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_S1_Ctn2_OutCoint"));
			pEdtCtrl->SetText(szNum);
			pEdtCtrl->SetFocus();

			return;
		}
		else if (msg.pSender->GetName() == _T("Btn_OK")) //提交
		{
			CString strMsg;
			CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");

			CEditUI* pOutCointEdt = static_cast<CEditUI*>(m_pm.FindControl("Edt_S1_Ctn2_OutCoint"));

			CComboUI* pCobCtrl = static_cast<CComboUI*>(m_pm.FindControl("Cob_S1_Ctn2_Game"));
			if (pCobCtrl->GetCurSel() == -1)
			{
				DUIOkeyMsgBox(m_hWnd,"请选择接收游戏。");
				pCobCtrl->SetFocus();
				return;
			}

			LPCTSTR lpGameName = pCobCtrl->GetText().GetData();
			UINT uGameID = GetGameIDByName(lpGameName);

			//如果游戏别的房间已打开，不予以操作
			for (BYTE i = 0; i < MAX_GAME_ROOM; ++i)
			{
				// 游戏中游戏界面右边消息栏
				CGameRoomEx* pGameRoomEx = (CGameRoomEx*)GetMainRoom()->m_RoomInfo[i].pGameRoomWnd;
				if (m_pRoom == pGameRoomEx) //发起消息的房间无需再更新金币数
				{
					continue;
				}
				if (NULL != pGameRoomEx)
				{
					if (pGameRoomEx->m_GameInfo.uNameID == uGameID)
					{
						ShowTipMessage(NULL,"PBankWnd2","SrvSv_Room_NoRule",
							"游戏【%s】房间已打开，不予存款，如有需要，请到相应应房间里使用银行进行存款。",
							pGameRoomEx->m_GameInfo.szGameName);
						return;
					}
				}	
			}

			CString strNum = pOutCointEdt->GetText();
			CString str;

			TCHAR szTail[16];
			_tcscpy(szTail, TEXT(""));
			for (int i=0; i<Glb().m_nPowerOfGold; ++i)
			{
				_tcscat( szTail, TEXT("0"));
			}

			//如果有分隔符，则去除字符串里的分隔符
			if(Glb().m_bUseSpace)
				strNum.Replace(Glb().m_strSpaceChar,"");

			/// 如果没有输入
			if (strNum.GetLength() == 0)
			{	
				ShowTipMessage(NULL,"PBankWnd2","InSv_Msg_NotInputCoin",
									"请输入您要存取款的数额。");
				pOutCointEdt->SetFocus();
				return;
			}

			/// 如果输入太短
			if (strNum.GetLength()<Glb().m_nPowerOfGold)
			{
				ShowTipMessage(NULL,"PBankWnd2","InSv_Msg_NotGetCoin",
								"您输入的金币数额不被系统接受，请输入至少%d位，且末%d位为 %s 的正整数！",
								Glb().m_nPowerOfGold+1,Glb().m_nPowerOfGold,szTail);
				pOutCointEdt->SetFocus();
				return;
			}
			/// 如果输入太长
			if (strNum.GetLength()>18 + Glb().m_nPowerOfGold)
			{
				ShowTipMessage(NULL,"PBankWnd2","InSv_Msg_NumberTooLarge",
								"您输入的数字太大，超出允许范围，请输入不大于%I64d%s，且末%d位为 %s 的正整数！",
								0x3FFFFFFFFFFFFFFF,szTail,Glb().m_nPowerOfGold,szTail);
				pOutCointEdt->SetFocus();
				return;
			}

			/// 如果末几位不是零
			CString strSubNum = strNum.Right(Glb().m_nPowerOfGold);
			if (strSubNum != szTail)
			{
				ShowTipMessage(NULL,"PBankWnd2","InSv_Msg_NotGetCoin2",
								"您输入的金币数额不被系统接受，请输入末%d位为 %s 的正整数！",
								Glb().m_nPowerOfGold, szTail);
				pOutCointEdt->SetFocus();
				return;
			}
			/// 截取前N位
			strSubNum = strNum.Left(strNum.GetLength()-Glb().m_nPowerOfGold);
			__int64 i64Num = _atoi64(strSubNum.GetBuffer());
			if (i64Num>0x3FFFFFFFFFFFFFFF)
			{
				ShowTipMessage(NULL,"PBankWnd2","InSv_Msg_NumberTooLarge",
					"您输入的数字太大，超出允许范围，请输入不大于%I64d%s，且末%d位为 %s 的正整数！",
					0x3FFFFFFFFFFFFFFF,szTail,Glb().m_nPowerOfGold,szTail);
				pOutCointEdt->SetFocus();
				return;
			}
			if (i64Num<=0)
			{
				ShowTipMessage(NULL,"PBankWnd2","InSv_Msg_NotGetCoin",
								"您输入的金币数额不被系统接受，请输入至少%d位，且末%d位为 %s 的正整数！",
								Glb().m_nPowerOfGold+1,Glb().m_nPowerOfGold,szTail);
				pOutCointEdt->SetFocus();
				return;
			}

			__int64 i64GameWallet = GetGameWalletMoney(lpGameName);

			if (i64Num > i64GameWallet)
			{
				ShowTipMessage(NULL,"","","您输入的数字已大于超出银行的存额");
				pOutCointEdt->SetFocus();
				return;
			}
			
			if (uGameID == 0)
			{
				ShowTipMessage(NULL,"","","GameID不对");
				return;
			}

			//选择存款的游戏就是当前房间的游戏一至时，存款有限制
			if (ISGameRoom == m_emParWndType)
			{
				CGameRoomEx* pRoomEx = (CGameRoomEx*)m_pRoom;
				if (pRoomEx->m_GameInfo.uNameID == uGameID)
				{
					if (i64GameWallet - i64Num < pRoomEx->m_GameInfo.uLessPoint)
					{
						TCHAR szNum[128] = {0};
						GetNumString(szNum,i64GameWallet - i64Num,Glb().m_nPowerOfGold,Glb().m_bUseSpace,Glb().m_strSpaceChar);

						ShowTipMessage(NULL,"PBankWnd2","InSv_Msg_LessCoint",
							"存款后游戏【%s】的游戏将剩余金币%s,小于房间下限金币%d。不予以存款,请重新调整存入数据额。",
							lpGameName,szNum,pRoomEx->m_GameInfo.uLessPoint);
						pOutCointEdt->SetFocus();
						return;
					}
				}
			}

			/// 发消息
			if (m_pRoom != NULL)
			{
				bank_op_normal* p = new bank_op_normal;
				::memset(p,0,sizeof(bank_op_normal));
				p->_user_id = m_dwOpUserID;
				p->_operate_type = 2;
				p->_game_id = uGameID;
				p->_money = i64Num;
				m_pRoom->PostMessage(WM_BANK,0,(LPARAM)p);
			}

			return;
		}
	}
	else if (msg.sType == _T("textchanged"))
	{
		if (msg.pSender->GetName() == _T("Edt_S1_Ctn2_OutCoint"))
		{
			//转账金额小写转换为中文大写
			ChangeEtToChinese(static_cast<CEditUI*>(msg.pSender),
				static_cast<CTextUI*>(m_pm.FindControl("Txt_S1_Ctn2_Caps")));
			return;
		}
	}
	else if (msg.sType == _T("itemselect"))
	{
		if( msg.pSender->GetName() == _T("Cob_S1_Ctn2_Game")) 
		{
			TCHAR szNum[128];
			__int64 i64GameMoney = GetGameWalletMoney(msg.pSender->GetText().GetData());
//			UINT uGameID = GetGameIDByName(msg.pSender->GetText().GetData());
			GetNumString(szNum,i64GameMoney,Glb().m_nPowerOfGold,Glb().m_bUseSpace,Glb().m_strSpaceChar);
			CEditUI* pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Lab_S1_Ctn2_GameCoint"));
			pEdtCtrl->SetText(szNum);

			//如果游戏别的房间已打开，不予以操作
// 			for (BYTE i = 0; i < MAX_GAME_ROOM; ++i)
// 			{
// 				// 游戏中游戏界面右边消息栏
// 				CGameRoomEx* pGameRoomEx = (CGameRoomEx*)GetMainRoom()->m_RoomInfo[i].pGameRoomWnd;
// 				if (m_pRoom == pGameRoomEx) //发起消息的房间无需再更新金币数
// 				{
// 					continue;
// 				}
// 				if (NULL != pGameRoomEx)
// 				{
// 					if (pGameRoomEx->m_GameInfo.uNameID == uGameID)
// 					{
// 						ShowTipMessage(NULL,"PBankWnd2","SrvSv_Room_NoRule",
// 							"游戏【%s】房间已打开，不予存款，如有需要，请到相应应房间里使用银行进行存款。",
// 							pGameRoomEx->m_GameInfo.szGameName);
// 						return;
// 					}
// 				}	
// 			}

			return;
		}
	}
}

//“类型1”中的“标题3”消息处理函数
void CPBankWndEx::Notify_1_3(BzDui::TNotifyUI& msg)
{
	if (msg.sType == "on_contain_open")
	{
// 		m_strTransTargetID = "0";
// 		m_strTransTargetNickName = " ";
	}
	else if (msg.sType == "click")
	{
		if (msg.pSender->GetName() == _T("Btn_OK")) //提交
		{
			CEditUI* pEdtOutCoint = NULL;
			pEdtOutCoint = static_cast<CEditUI*>(m_pm.FindControl("Edt_S1_Ctn3_TransCoint"));

			CEditUI* pEdtPwd = NULL;
			pEdtPwd = static_cast<CEditUI*>(m_pm.FindControl("Edt_S1_Ctn3_Pwd"));

			/// 取得编辑框里的字符串
			TCHAR szMsg[256];
			CString strNum, str;
			strNum = pEdtOutCoint->GetText();

			//如果有分隔符，则去除字符串里的分隔符
			if(Glb().m_bUseSpace)
				strNum.Replace(Glb().m_strSpaceChar,"");

			TCHAR szTail[16];
			_tcscpy(szTail, TEXT(""));
			for (int i=0; i<Glb().m_nPowerOfGold; ++i)
			{
				_tcscat( szTail, TEXT("0"));
			}
			/// 如果没有输入
			if (strNum.GetLength() == 0)
			{
				ShowTipMessage(NULL,"PBankWnd2","Trans_Msg_NotInputCoin","请输入您要存取款的数额。");
				pEdtOutCoint->SetFocus();
				return;
			}
			/// 如果输入太短
			if (strNum.GetLength()<Glb().m_nPowerOfGold)
			{
				ShowTipMessage(NULL,"PBankWnd2","Trans_Msg_NotGetCoin",
									"您输入的金币数额不被系统接受，请输入至少%d位，且末%d位为 %s 的正整数！",
									Glb().m_nPowerOfGold+1,Glb().m_nPowerOfGold,szTail);
				pEdtOutCoint->SetFocus();
				return;
			}
			/// 如果输入太长
			if (strNum.GetLength()>18+Glb().m_nPowerOfGold)
			{
				ShowTipMessage(NULL,"PBankWnd2","Trans_Msg_NumberTooLargen",
									"您输入的数字太大，超出允许范围，请输入不大于%I64d%s，且末%d位为 %s 的正整数！",
									0x3FFFFFFFFFFFFFFF,szTail,Glb().m_nPowerOfGold,szTail);
				pEdtOutCoint->SetFocus();
				return;
			}

			/// 如果末几位不是零
			CString strSubNum = strNum.Right(Glb().m_nPowerOfGold);
			if (strSubNum != szTail)
			{
				ShowTipMessage(NULL,"PBankWnd2","Trans_Msg_NotGetCoin2",
									"您输入的金币数额不被系统接受，请输入末%d位为 %s 的正整数！",
									 Glb().m_nPowerOfGold,szTail);
				pEdtOutCoint->SetFocus();
				return;
			}
			/// 截取前N位
			strSubNum = strNum.Left(strNum.GetLength()-Glb().m_nPowerOfGold);
			__int64 i64Num = _atoi64(strSubNum.GetBuffer());
			if (i64Num>0x3FFFFFFFFFFFFFFF)
			{
				ShowTipMessage(NULL,"PBankWnd2","Trans_Msg_NumberTooLargen",
									"您输入的数字太大，超出允许范围，请输入不大于%I64d%s，且末%d位为 %s 的正整数！",
									0x3FFFFFFFFFFFFFFF,szTail,Glb().m_nPowerOfGold,szTail);
				pEdtOutCoint->SetFocus();
				return;
			}

			//获得目标用户
			CEditUI* pEdtTargetUser = static_cast<CEditUI*>(m_pm.FindControl("Edt_S1_Ctn3_TargetUser"));
			strNum = pEdtTargetUser->GetText();

			//二级密码
			str = pEdtPwd->GetText();

			if (str.GetLength() < 1)
			{
				ShowTipMessage(NULL,"","","请输入密码！");
				pEdtPwd->SetFocus();
				return;
			}
			if (i64Num<=0)
			{
				ShowTipMessage(NULL,"PBankWnd2","Trans_Msg_NotGetCoin",
									"您输入的金币数额不被系统接受，请输入至少%d位，且末%d位为 %s 的正整数！",
									Glb().m_nPowerOfGold + 1,Glb().m_nPowerOfGold,szTail);
				pEdtOutCoint->SetFocus();
				return;
			}

			COptionUI* pOptCtrl = static_cast<COptionUI*>(m_pm.FindControl("Opt_S1_Ctn3_ID"));

			if (0 == atol(strNum) && pOptCtrl->IsSelected())
			{
				ShowTipMessage(NULL,"BankDlg","WrongID","请输入正确的玩家ID！");

				CEditUI* pEdtTargetUser = NULL;
				pEdtTargetUser = static_cast<CEditUI*>(m_pm.FindControl("Edt_S1_Ctn3_TargetUser"));
				pEdtTargetUser->SetFocus();
				return;
			}

			CLabelUI* pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_S1_Ctn3_NickNameID_temp"));

			TransferMoney_t *pTransferMoney = new TransferMoney_t();
			pTransferMoney->i64Money = i64Num;
			pTransferMoney->uDestUserID = atol(strNum);
			_tcscpy(pTransferMoney->szPWD, str.Left(20).GetBuffer());
			pOptCtrl = static_cast<COptionUI*>(m_pm.FindControl("Opt_S1_Ctn3_ID"));

			if (pOptCtrl->IsSelected())
			{
				_tcscpy(pTransferMoney->szDestNickName, pLabCtrl->GetText().GetData());
				pTransferMoney->bUseDestID = true;
			}
			else
			{
				_tcscpy(pTransferMoney->szDestNickName, strNum.GetBuffer());
				pTransferMoney->bUseDestID = false;
			}

			/// 提示用户确认转帐
			CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");

			TCHAR szNum[128];
			GetNumString(szNum, pTransferMoney->i64Money, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);

			CString strFormat;
			CString test;
			pOptCtrl = static_cast<COptionUI*>(m_pm.FindControl("Opt_S1_Ctn3_ID"));
			if(pOptCtrl->IsSelected())
			{
				strFormat = fMsg.GetKeyVal("PBankWnd2", "Trans_Msg_ConfirmId",
					"<p 5>转账前请确认以下转账信息，是否确定转帐？<n><y 30>目标用户：<b>%s</b> (ID：<b>%d</b>)</y><n>转帐数额：<b>%s</b> 金币<n></p>");
				wsprintf(szMsg,strFormat,pLabCtrl->GetText().GetData(),pTransferMoney->uDestUserID,szNum);
			}
			else
			{
				if (::atol(pLabCtrl->GetText().GetData()) == m_dwOpUserID)
				{
					DUIOkeyMsgBox(AfxGetMainWnd()->m_hWnd,
						fMsg.GetKeyVal("BankDlg","TransferToSelf","您打算转帐给自己？对不起，系统暂不提供此项服务。"));
					return;
				}

				strFormat = fMsg.GetKeyVal("PBankWnd2", "Trans_Msg_ConfirmNN",
					"<p 5>转账前请确认以下转账信息，是否确定转帐？<n><y 30>目标用户：<b>%s</b> (ID：<b>%d</b>)</y><n>转帐数额：<b>%s</b> 金币<n></p>");
				wsprintf(szMsg,strFormat,pTransferMoney->szDestNickName,pLabCtrl->GetText().GetData(),szNum);
			}	

			if (IDYES != DUIMessageBox(m_hWnd,MB_YESNO|MB_ICONQUESTION,"银行",szMsg))
			{
				delete pTransferMoney;
				pTransferMoney = NULL;
				return;
			}

			m_bTransferSuccess = true; 

			m_pRoom->PostMessage(WM_TRANSFER_MONEY,0,LPARAM(pTransferMoney));
			
			//清空密码框
			pEdtPwd->SetText("");

			return;
		}
	}
	else if(msg.sType == _T("selectchanged"))
	{
		if (msg.pSender->GetName() == _T("Opt_S1_Ctn3_ID"))
		{
			//切换至目标用户ID输入方式
			CBcfFile fMsg(CBcfFile::GetAppPath() + "ClientMessage.bcf");
			CLabelUI* pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_S1_Ctn3_1"));
			pLabCtrl->SetText(fMsg.GetKeyVal("PBankDlg","TransUserID","对方ID："));

			pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_S1_Ctn3_NickNameID"));
			pLabCtrl->SetText("");

			CEditUI* pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_S1_Ctn3_TargetUser"));
			pEdtCtrl->SetText("");
			pEdtCtrl->SetFocus();


			return;
		}
		else if (msg.pSender->GetName() == _T("Opt_S1_Ctn3_NickName"))
		{
			//切换至目标用户昵称输入方式
			CBcfFile fMsg(CBcfFile::GetAppPath() + "ClientMessage.bcf");
			CLabelUI* pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_S1_Ctn3_1"));
			pLabCtrl->SetText(fMsg.GetKeyVal("PBankDlg","TransUserNick","对方昵称："));

			pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_S1_Ctn3_NickNameID"));
			pLabCtrl->SetText("");

			CEditUI* pEdtCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_S1_Ctn3_TargetUser"));
			pEdtCtrl->SetText("");
			pEdtCtrl->SetFocus();
			return;
		}
	}
	else if (msg.sType == _T("textchanged"))
	{
		if (msg.pSender->GetName() == _T("Edt_S1_Ctn3_TransCoint"))
		{
			//转账金额小写转换为中文大写
			ChangeEtToChinese(static_cast<CEditUI*>(msg.pSender),
				static_cast<CTextUI*>(m_pm.FindControl("Txt_S1_Ctn3_Caps")));
			return;
		}
	}
	else if(msg.sType == _T("setfocus"))
	{
		if (msg.pSender->GetName() == _T("Edt_S1_Ctn3_TargetUser"))
		{
			//输入转账目标用户，进行一些检测
			m_strFocusTemp = msg.pSender->GetText();
			return;
		}
	}
	else if(msg.sType == _T("killfocus"))
	{
		if (msg.pSender->GetName() == _T("Edt_S1_Ctn3_TargetUser"))
		{
			//目标用户编辑框没有输入内容或为空都不触发OnIDKillFocus

			CLabelUI* pLabCtrl = NULL;

			if (msg.pSender->GetText().GetLength() == 0)
			{
				pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_S1_Ctn3_NickNameID_temp"));
				pLabCtrl->SetText("");

				pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_S1_Ctn3_NickNameID"));
				pLabCtrl->SetText("");
			}
			else
			{
				//输入转账目标用户，进行一些检测
				if (msg.pSender->GetText().Compare(m_strFocusTemp))
				{
					pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_S1_Ctn3_NickNameID"));
					pLabCtrl->SetText("正在查询...");

					COptionUI* pOptCtrl = static_cast<COptionUI*>(m_pm.FindControl("Opt_S1_Ctn3_ID"));
					if(!OnIDKillFocus(static_cast<CEditUI*>(msg.pSender)
							,(pOptCtrl->IsSelected()?NICK_ID_BYID:NICK_ID_BYNICKNAME)
							,NICK_ID_USE_TO_TRANSFER))
					{
						pLabCtrl->SetText("(玩家不存在)");
					}
				}

				m_strFocusTemp = "";
			}
			
			return;
		}
	}
}

//“类型1”中的“标题4”消息处理函数
void CPBankWndEx::Notify_1_4(BzDui::TNotifyUI& msg)
{
	if (msg.sType == _T("on_contain_open"))
	{
		if(m_bTransferSuccess)
		{
			m_mapRecord.clear();
			m_iRecDispPos = 0;
			m_pRoom->PostMessage(WM_TRANSFER_RECORD,0,0);

			m_bTransferSuccess = false;

			//翻页控件按钮
			CButtonUI* pBntCtrl = NULL;
			pBntCtrl = static_cast<CButtonUI*>(m_pm.FindControl("Bnt_S1_Ctn4_First"));
			pBntCtrl->SetEnabled(false);
			pBntCtrl = static_cast<CButtonUI*>(m_pm.FindControl("Bnt_S1_Ctn4_Previous"));
			pBntCtrl->SetEnabled(false);
			pBntCtrl = static_cast<CButtonUI*>(m_pm.FindControl("Bnt_S1_Ctn4_Next"));
			pBntCtrl->SetEnabled(false);
			pBntCtrl = static_cast<CButtonUI*>(m_pm.FindControl("Bnt_S1_Ctn4_Rear"));
			pBntCtrl->SetEnabled(false);
		}
	}
	else if (msg.sType == _T("click"))
	{
		if (msg.pSender->GetName() == _T("Btn_S1_Ctn4_Refresh"))
		{
			//刷新转账记录
			m_bTransferSuccess = TRUE;

			TNotifyUI tagUIMsg;
			tagUIMsg.sType = _T("on_contain_open");
			Notify_1_4(tagUIMsg);

			return;
		}
		else if (msg.pSender->GetName() == _T("Bnt_S1_Ctn4_First"))
		{
			//记录翻到首页
			m_iRecDispPos = 0;
			m_pRecordList->NeedUpdate();
			UpdateRecordPageState();
			return;
		}
		else if (msg.pSender->GetName() == _T("Bnt_S1_Ctn4_Previous"))
		{
			//记录翻上一页
			if (m_iRecDispPos - RECORD_PAGE_SIZE >= 0)
			{
				m_iRecDispPos -= RECORD_PAGE_SIZE;
				m_pRecordList->NeedUpdate();
				UpdateRecordPageState();
			}
			return;
		}
		else if (msg.pSender->GetName() == _T("Bnt_S1_Ctn4_Next"))
		{
			//记录翻下一页
			if (m_iRecDispPos + RECORD_PAGE_SIZE < m_mapRecord.size())
			{
				m_iRecDispPos += RECORD_PAGE_SIZE;
				m_pRecordList->NeedUpdate();
				UpdateRecordPageState();
			}
			return;
		}
		else if (msg.pSender->GetName() == _T("Bnt_S1_Ctn4_Rear"))
		{
			//记录翻到尾页
			if (m_mapRecord.size() % RECORD_PAGE_SIZE == 0)
			{
				m_iRecDispPos = m_mapRecord.size() - RECORD_PAGE_SIZE;
			}
			else
			{
				m_iRecDispPos = m_mapRecord.size() - m_mapRecord.size() % RECORD_PAGE_SIZE;
			}

			m_pRecordList->NeedUpdate();
			UpdateRecordPageState();
			return;
		}
	}

	return;
}

//“类型1”中的“标题5”消息处理函数
void CPBankWndEx::Notify_1_5(BzDui::TNotifyUI& msg)
{
	if (msg.sType == "click")
	{
		if (msg.pSender->GetName() == _T("Btn_OK")) //提交
		{
			CEditUI* pEdtOutCoint = NULL;
			pEdtOutCoint = static_cast<CEditUI*>(m_pm.FindControl("Edt_S1_Ctn5_OutCoint"));

			CEditUI* pEdtPwd = NULL;
			pEdtPwd = static_cast<CEditUI*>(m_pm.FindControl("Edt_S1_Ctn5_Pwd"));

			/// 取得编辑框里的字符串
			TCHAR szMsg[256];
			CString strNum, str;
			strNum = pEdtOutCoint->GetText();

			//如果有分隔符，则去除字符串里的分隔符
			if(Glb().m_bUseSpace)
				strNum.Replace(Glb().m_strSpaceChar,"");

			TCHAR szTail[16];
			_tcscpy(szTail, TEXT(""));
			for (int i=0; i<Glb().m_nPowerOfGold; ++i)
			{
				_tcscat( szTail, TEXT("0"));
			}
			/// 如果没有输入
			if (strNum.GetLength() == 0)
			{
				ShowTipMessage(NULL,"PBankWnd2","CreateDep_Msg_NotInputCoin","请输入您要存取款的数额。");
				pEdtOutCoint->SetFocus();
				return;
			}
			/// 如果输入太短
			if (strNum.GetLength()<Glb().m_nPowerOfGold)
			{
				ShowTipMessage(NULL,"PBankWnd2","CreateDep_Msg_NotGetCoin",
					"您输入的金币数额不被系统接受，请输入至少%d位，且末%d位为 %s 的正整数！",
					Glb().m_nPowerOfGold+1,Glb().m_nPowerOfGold,szTail);
				pEdtOutCoint->SetFocus();
				return;
			}
			/// 如果输入太长
			if (strNum.GetLength()>18+Glb().m_nPowerOfGold)
			{
				ShowTipMessage(NULL,"PBankWnd2","CreateDep_Msg_NumberTooLargen",
					"您输入的数字太大，超出允许范围，请输入不大于%I64d%s，且末%d位为 %s 的正整数！",
					0x3FFFFFFFFFFFFFFF,szTail,Glb().m_nPowerOfGold,szTail);
				pEdtOutCoint->SetFocus();
				return;
			}

			/// 如果末几位不是零
			CString strSubNum = strNum.Right(Glb().m_nPowerOfGold);
			if (strSubNum != szTail)
			{
				ShowTipMessage(NULL,"PBankWnd2","CreateDep_Msg_NotGetCoin2",
					"您输入的金币数额不被系统接受，请输入末%d位为 %s 的正整数！",
					Glb().m_nPowerOfGold,szTail);
				pEdtOutCoint->SetFocus();
				return;
			}
			/// 截取前N位
			strSubNum = strNum.Left(strNum.GetLength()-Glb().m_nPowerOfGold);
			__int64 i64Num = _atoi64(strSubNum.GetBuffer());
			if (i64Num>0x3FFFFFFFFFFFFFFF)
			{
				ShowTipMessage(NULL,"PBankWnd2","CreateDep_Msg_NumberTooLargen",
					"您输入的数字太大，超出允许范围，请输入不大于%I64d%s，且末%d位为 %s 的正整数！",
					0x3FFFFFFFFFFFFFFF,szTail,Glb().m_nPowerOfGold,szTail);
				pEdtOutCoint->SetFocus();
				return;
			}

			if (i64Num > m_i64BankMoney)
			{
				ShowTipMessage(NULL,"","","您输入的数字已大于超出银行的存额");
				pEdtOutCoint->SetFocus();
				return;
			}

			//二级密码
			str = pEdtPwd->GetText();

			if (str.GetLength() < 1)
			{
				ShowTipMessage(NULL,"","","请输入密码！");
				pEdtPwd->SetFocus();
				return;
			}
			if (i64Num<=0)
			{
				ShowTipMessage(NULL,"PBankWnd2","CreateDep_Msg_NotGetCoin",
					"您输入的金币数额不被系统接受，请输入至少%d位，且末%d位为 %s 的正整数！",
					Glb().m_nPowerOfGold + 1,Glb().m_nPowerOfGold,szTail);
				pEdtOutCoint->SetFocus();
				return;
			}

			/// 发消息
			if (m_pRoom != NULL)
			{
				bank_op_deposit* pDeposit = new bank_op_deposit();
				pDeposit->_user_id = m_dwOpUserID;
				pDeposit->_operate_type = 1;
				pDeposit->_money = i64Num;
				//提取输入的密码
				CGameRoomEx::GetCryptedPasswd(pDeposit->_szMD5Pass,(TCHAR*)pEdtPwd->GetText().GetData());

				m_pRoom->PostMessage(WM_BANK,1,LPARAM(pDeposit));

				m_bDepositSuccess = true;
			}

			return;
		}
		else if (msg.pSender->GetName() == _T("Bnt_S1_Ctn5_Suc_Return"))
		{
			//切换显示容器,显示创建存单的UI容器
			m_pSelFunNode->pFunsCtn->SetVisible(false);
			CContainerUI* pCtnCtrl = static_cast<CContainerUI*>(m_pm.FindControl("Ctn_Type1_Ctn5"));
			m_pSelFunNode->pFunsCtn = pCtnCtrl;
			m_pSelFunNode->bNeedSubmit = true;
			m_pSelFunNode->pFunsCtn = pCtnCtrl;
			pCtnCtrl->SetVisible(true);

			//隐藏提交按钮的使用
			m_pSubmitBnt->SetVisible(true);

			return;
		}
	}
	else if (msg.sType == _T("textchanged"))
	{
		if (msg.pSender->GetName() == _T("Edt_S1_Ctn5_OutCoint"))
		{
			//转账金额小写转换为中文大写
			ChangeEtToChinese(static_cast<CEditUI*>(msg.pSender),
				static_cast<CTextUI*>(m_pm.FindControl("Txt_S1_Ctn5_Caps")));
			return;
		}
	}

	return;
}

//“类型1”中的“标题6”消息处理函数
void CPBankWndEx::Notify_1_6(BzDui::TNotifyUI& msg)
{
	if (msg.sType == _T("on_contain_open"))
	{
		if(m_bDepositSuccess)
		{
			m_mapDeposits.clear();
			m_pDepList->RemoveAll();
			m_bDepositSuccess = false;
			m_pRoom->PostMessage(WM_BANK,3,m_dwOpUserID);
		}
	}
	else if (msg.sType == _T("itemselect"))
	{
		if (msg.pSender->GetName() == _T("Lst_S1_Ctn6_Record"))
		{
			if (msg.wParam < 0)
			{
				return;
			}

			CLabelUI* pLabCtrl = NULL;
			CEditUI* pEditUI = NULL;

			char szBuffer[128];

			bank_op_deposit_node* pDepNode = &m_mapDeposits[msg.wParam];

			pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_S1_Ctn6_Title"));
			pLabCtrl->SetText("存单");

			pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_S1_Ctn6_Creater"));
			_stprintf(szBuffer,"%d",pDepNode->_user_id);
			pLabCtrl->SetText(szBuffer);

// 			pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_S1_Ctn6_Time"));
// 			_stprintf(szBuffer,_T("%02d-%02d-%02d %02d:%02d:%02d"),
// 				pDepNode->_create_time.GetYear(),
// 				pDepNode->_create_time.GetMonth(),
// 				pDepNode->_create_time.GetDay(),
// 				pDepNode->_create_time.GetHour(),
// 				pDepNode->_create_time.GetMinute(),
// 				pDepNode->_create_time.GetSecond()
// 				);
// 			pLabCtrl->SetText(szBuffer);

			pEditUI = static_cast<CEditUI*>(m_pm.FindControl("Edt_S1_Ctn6_Serial"));
			pEditUI->SetText(pDepNode->_deposit_id);

			pEditUI = static_cast<CEditUI*>(m_pm.FindControl("Edt_S1_Ctn6_Pwd"));
			pEditUI->SetText(pDepNode->_deposit_password);

			GetNumString(szBuffer,pDepNode->_money,Glb().m_nPowerOfGold,Glb().m_bUseSpace,Glb().m_strSpaceChar);
			pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_S1_Ctn6_Money"));
			pLabCtrl->SetText(szBuffer);


			pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_S1_Ctn6_State"));
			if (pDepNode->_state == 0)
			{
				pLabCtrl->SetText("未使用");
			}
			else if (pDepNode->_state == 1)
			{
				pLabCtrl->SetText("已使用");
			}

			return;
		}
	}

	return;
}


//“类型1”中的“标题7”消息处理函数
void CPBankWndEx::Notify_1_7(BzDui::TNotifyUI& msg)
{
	if (msg.sType == "on_contain_open")
	{
// 		m_strTransTargetID = "0";
// 		m_strTransTargetNickName = " ";
	}
	else if (msg.sType == "click")
	{
		if (msg.pSender->GetName() == _T("Btn_OK")) //提交
		{
			CEditUI* pTargetUserEdt = static_cast<CEditUI*>(m_pm.FindControl("Edt_S1_Ctn7_TargetUser"));
			CEditUI* pSerialNoEdt = static_cast<CEditUI*>(m_pm.FindControl("Edt_S1_Ctn7_Serial"));
			CEditUI* pPasswordEdt = static_cast<CEditUI*>(m_pm.FindControl("Edt_S1_Ctn7_Pwd"));

			CStdString strTargetUser = pTargetUserEdt->GetText();
			CStdString strSerial = pSerialNoEdt->GetText();
			CStdString strPassword = pPasswordEdt->GetText();

			if (strTargetUser.GetLength() == 0)
			{
				ShowTipMessage(NULL,"","","请输入要充值的用户名ID");
				pTargetUserEdt->SetFocus();
				return;
			}

			if (strSerial.GetLength() == 0)
			{
				ShowTipMessage(NULL,"","","请输入充值卡序列号");
				pSerialNoEdt->SetFocus();
				return;
			}
			
			if (strPassword.GetLength() == 0)
			{
				ShowTipMessage(NULL,"","","请输入充值卡密码");
				pPasswordEdt->SetFocus();
				return;
			}

			if (0 == atol(strTargetUser))
			{
				ShowTipMessage(NULL,"","","请输入正确的玩家ID！");
				pTargetUserEdt->SetFocus();
				return;
			}



			/// 取得编辑框里的字符串
			TCHAR szMsg[256];

			/// 提示用户确认转帐
			CBcfFile fMsg(CBcfFile::GetAppPath() + "ClientMessage.bcf");

			CLabelUI* pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_S1_Ctn7_NickNameID_temp"));

			CString strFormat;
			strFormat = fMsg.GetKeyVal("PBankWnd2", "UseDep_Msg_Confirm","您将要给ID号为[%d]玩家，昵称[%s]进行充值，是否确定转帐？");
			wsprintf(szMsg,strFormat,atol(strTargetUser),pLabCtrl->GetText().GetData());

			if (IDYES != DUIMessageBox(m_hWnd,MB_YESNO|MB_ICONQUESTION,"银行",szMsg))
			{
				return;
			}

			bank_op_deposit* pDeposit = new bank_op_deposit();
			pDeposit->_user_id = (int)atol(strTargetUser);
			pDeposit->_operate_type = 2;
			strcpy_s(pDeposit->_deposit_id,sizeof(pDeposit->_deposit_id),strSerial.GetData());
			strcpy_s(pDeposit->_deposit_password,sizeof(pDeposit->_deposit_password),strPassword.GetData());

			/// 发消息
			if (m_pRoom != NULL)
			{
				m_pRoom->PostMessage(WM_BANK,1,LPARAM(pDeposit));
			}
 
			return;
		}
	}
	else if(msg.sType == _T("setfocus"))
	{
		if (msg.pSender->GetName() == _T("Edt_S1_Ctn7_TargetUser"))
		{
			//输入转账目标用户，进行一些检测
			m_strFocusTemp = msg.pSender->GetText();
			return;
		}
	}
	else if(msg.sType == _T("killfocus"))
	{
		if (msg.pSender->GetName() == _T("Edt_S1_Ctn7_TargetUser"))
		{
			//目标用户编辑框没有输入内容或为空都不触发OnIDKillFocus

			CLabelUI* pLabCtrl = NULL;

			if (msg.pSender->GetText().GetLength() == 0)
			{
				pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_S1_Ctn7_NickNameID_temp"));
				pLabCtrl->SetText("");

				pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_S1_Ctn7_NickNameID"));
				pLabCtrl->SetText("");
			}
			else
			{
				if (msg.pSender->GetText().Compare(m_strFocusTemp))
				{
					pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_S1_Ctn7_NickNameID"));
					pLabCtrl->SetText("正在查询...");

					//输入转账目标用户，进行一些检测
					if(!OnIDKillFocus(static_cast<CEditUI*>(msg.pSender),NICK_ID_BYID,NICK_ID_USE_TO_DEPOSIT))
					{
						pLabCtrl->SetText("(玩家不存在)");
					}
					m_strFocusTemp = "";
				}
			}
			
			return;
		}
	}

	return;
}

//“类型2”中的“标题1”消息处理函数
void CPBankWndEx::Notify_2_1(BzDui::TNotifyUI& msg)
{
	if (msg.sType == _T("click"))
	{
		if (msg.pSender->GetName() == _T("Btn_OK")) //提交
		{

			CEditUI* pOldPwdEdt = static_cast<CEditUI*>(m_pm.FindControl("Edt_S2_Ctn1_OldPwd"));
			CEditUI* pNewPwdEdt = static_cast<CEditUI*>(m_pm.FindControl("Edt_S2_Ctn1_NewPwd"));
			CEditUI* pPwdCfmEdt = static_cast<CEditUI*>(m_pm.FindControl("Edt_S2_Ctn1_PwdCfm"));

			CString strOld, strNew, strNewCfm, str;
			TCHAR szMsg[MAX_PATH];

			strOld = pOldPwdEdt->GetText();//旧密码

			if ((strOld.GetLength() < 6) ||(strOld.GetLength() > 20))
			{
				ShowTipMessage(NULL,"PBankWnd2","ChPwd_Msg_OldBytes","密码由6-20字符组成，请重新输入旧密码！");
				pOldPwdEdt->SetFocus();
				return;
			}

			/// 先比较新密码和确认密码是否相同，如果不同，则直接返回，相同才继续
			strNew = pNewPwdEdt->GetText();//新密码
			strNewCfm = pPwdCfmEdt->GetText();//确认新密码
			if (strNew != strNewCfm)
			{
				ShowTipMessage(NULL,"PBankWnd2","ChPwd_Msg_NotSame","新密码两次输入不一致，请重新输入!");
				pNewPwdEdt->SetFocus();
				return;
			}
			if ((strNew.GetLength() < 6) ||(strNew.GetLength() > 20))
			{
				ShowTipMessage(NULL,"PBankWnd2","ChPwd_Msg_NewBytes","新密码两次输入不一致，请重新输入!");
				pNewPwdEdt->SetFocus();
				return;
			}

			TMSG_GP_BankChPwd* pChPwd = new TMSG_GP_BankChPwd();
			pChPwd->_user_id = m_dwOpUserID;

			CGameRoomEx::GetCryptedPasswd(pChPwd->_MD5Pass_old,(char*)strOld.Left(20).GetBuffer());
			CGameRoomEx::GetCryptedPasswd(pChPwd->_MD5Pass_new,(char*)strNew.Left(20).GetBuffer());

			GetMainRoom()->PostMessage(WM_CHANGE_PASSWD,0,LPARAM(pChPwd));

			pOldPwdEdt->SetText("");//旧密码
			pNewPwdEdt->SetText("");//新密码
			pPwdCfmEdt->SetText("");//确认新密码

			return;
		}
	}
	return;
}

//“类型3”中的“标题1”消息处理函数
void CPBankWndEx::Notify_3_1(BzDui::TNotifyUI& msg)
{
	if (msg.sType == _T("on_contain_open"))
	{
		COptionUI* pOptCtrl = NULL;
		CComboUI* pCobCtrl = NULL;

		int nSelect = AfxGetApp()->GetProfileInt(TEXT("BankSafe"),TEXT("Option"),1);
		CString strMin = AfxGetApp()->GetProfileString(TEXT("BankSafe"),TEXT("Minutes"),_TEXT("5"));

		pOptCtrl = static_cast<COptionUI*>(m_pm.FindControl("Opt_S3_Ctn1_2"));

		if (pOptCtrl->IsSelected())
		{
			pCobCtrl = static_cast<CComboUI*>(m_pm.FindControl(_T("Cob_S3_Ctn1_Minutes")));
			SelectComboItem(pCobCtrl,strMin);
		}

		return;
	}
	else if (msg.sType == _T("click"))
	{
		if (msg.pSender->GetName() == _T("Btn_OK"))
		{
			COptionUI* pOptCtrl = NULL;

			pOptCtrl = static_cast<COptionUI*>(m_pm.FindControl("Opt_S3_Ctn1_1"));
			if (pOptCtrl->IsSelected())
			{
				AfxGetApp()->WriteProfileInt(TEXT("BankSafe"),TEXT("Option"),1);
				m_bCheckFrequence = false;
			}

			pOptCtrl = static_cast<COptionUI*>(m_pm.FindControl("Opt_S3_Ctn1_2"));
			if (pOptCtrl->IsSelected())
			{
				CComboUI* pCobCtrl = static_cast<CComboUI*>(m_pm.FindControl(_T("Cob_S3_Ctn1_Minutes")));
				AfxGetApp()->WriteProfileString(TEXT("BankSafe"),TEXT("Minutes"),pCobCtrl->GetText());
				AfxGetApp()->WriteProfileInt(TEXT("BankSafe"),TEXT("Option"),2);
				m_iMinutes = _atoi64(pCobCtrl->GetText());
				m_bCheckFrequence = true;
			}

			pOptCtrl = static_cast<COptionUI*>(m_pm.FindControl("Opt_S3_Ctn1_3"));
			if (pOptCtrl->IsSelected())
			{
				AfxGetApp()->WriteProfileInt(TEXT("BankSafe"),TEXT("Option"),3);
				m_bCheckFrequence = false;
			}

			ShowTipMessage(NULL,"","","设置成功！");
			
		}
	}
	else if(msg.sType == _T("selectchanged"))
	{
		if (msg.pSender->GetName() == _T("Opt_S3_Ctn1_1"))
		{
			CComboUI* pCobCtrl = NULL;
			pCobCtrl = static_cast<CComboUI*>(m_pm.FindControl("Cob_S3_Ctn1_Minutes"));
			pCobCtrl->SetEnabled(false);
		}
		else if (msg.pSender->GetName() == _T("Opt_S3_Ctn1_2"))
		{
			CComboUI* pCobCtrl = NULL;
			pCobCtrl = static_cast<CComboUI*>(m_pm.FindControl("Cob_S3_Ctn1_Minutes"));
			pCobCtrl->SetEnabled(true);
			if (pCobCtrl->GetCurSel() == -1)
			{
				pCobCtrl->SelectItem(0);
			}
		}
		else if (msg.pSender->GetName() == _T("Opt_S3_Ctn1_3"))
		{
			CComboUI* pCobCtrl = NULL;
			pCobCtrl = static_cast<CComboUI*>(m_pm.FindControl("Cob_S3_Ctn1_Minutes"));
			pCobCtrl->SetEnabled(false);
		}
	}

	return;
}

bool CPBankWndEx::OnNetMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	if (pNetHead->bMainID != MDM_GP_BANK && pNetHead->bMainID != MDM_BANK) return true;

	switch (pNetHead->bAssistantID)
	{
	case ASS_BANK_NORMAL:
	case ASS_GP_BANK_CHECK:
		{
			bank_op_normal *p = (bank_op_normal *)pNetData;
			if (p != NULL)
			{
				if (p->_operate_type == 1) //取钱
				{
					if (pNetHead->bHandleCode == HC_BANK_OP_SUC)
					{
						TCHAR szNum[128];
						GetNumString(szNum,p->_money,Glb().m_nPowerOfGold,Glb().m_bUseSpace,Glb().m_strSpaceChar);

						ShowTipMessage(NULL,"PBankWnd2","OutSv_Msg_Success",
										"<format><p 5><y 25>您成功取出 <b>%s</b> 金币到 <b>【%s】</b> 游戏的钱包中。</y></p>",
										szNum,GetGameNameById(p->_game_id));

						CEditUI* pPwdEdt = static_cast<CEditUI*>(m_pm.FindControl("Edt_S1_Ctn1_Pwd"));
						pPwdEdt->SetText("");

						//更新数据及显示数据
						m_i64BankMoney -= p->_money;
						AddGameWalletMoney(p->_game_id,p->_money);
						UpdateWealthInfor();

						//更新大厅的金币显示
// 						CGameRoomEx* pGameRoomEx = (CGameRoomEx*)m_pGameRoom;
// 						if (p->_game_id == pGameRoomEx->m_GameInfo.uNameID)
// 						{
// 							pGameRoomEx->FlushUserMoney(m_dwOpUserID,p->_money);
// 						}

						return true;

					}
					else if (pNetHead->bHandleCode == HC_BANK_OP_ERR_PWD)
					{
						DUIOkeyMsgBox(m_hWnd,"银行密码不正确");
						CEditUI* pPwdEdt = static_cast<CEditUI*>(m_pm.FindControl("Edt_S1_Ctn1_Pwd"));
						pPwdEdt->SetFocus();

						return true;
					}
				}
				else //存钱
				{
					if (pNetHead->bHandleCode == HC_BANK_OP_SUC)
					{
						TCHAR szNum[128];
						GetNumString(szNum,p->_money,Glb().m_nPowerOfGold,Glb().m_bUseSpace,Glb().m_strSpaceChar);

						ShowTipMessage(NULL,"PBankWnd2","InSv_Msg_Success",
							"<format><p 5><y 25>您成功已从<b>【%s】</b>游戏钱包取出<n><b>%s</b> 到钱柜中。</y></p>",
							GetGameNameById(p->_game_id),szNum);

						CEditUI* pPwdEdt = static_cast<CEditUI*>(m_pm.FindControl("Edt_S1_Ctn2_OutCoint"));
						pPwdEdt->SetFocus();
						pPwdEdt->SetText("");

						//更新数据及显示数据
						m_i64BankMoney += p->_money;
						AddGameWalletMoney(p->_game_id,-(p->_money));
						UpdateWealthInfor();

						//更新大厅的金币显示
// 						CGameRoomEx* pGameRoomEx = (CGameRoomEx*)m_pGameRoom;
// 						if (p->_game_id == pGameRoomEx->m_GameInfo.uNameID)
// 						{
// 							pGameRoomEx->FlushUserMoney(m_dwOpUserID,-(p->_money));
// 						}
					}

					return true;
				}
			}
			else
			{
				if (pNetHead->bHandleCode == HC_BANK_OP_CHECKOUT_PLAYING) 
				{
					ShowTipMessage(NULL,"GameRoom","NoDrawForPlaying","您正在游戏中，不能取款。");
					return true;
				}
				else if (pNetHead->bHandleCode == HC_BANK_OP_CHECKIN_PLAYING) 
				{
					ShowTipMessage(NULL,"GameRoom","NoSaveForPlaying","您正在游戏中，不能存款。");
					return true;
				}
				else if (pNetHead->bHandleCode == HC_BANK_OP_CHECKIN_ZHUANG) 
				{
					ShowTipMessage(NULL,"GameRoom","NoSaveForZhuangjia","您正在上庄中，不能存款。");
					return true;
				}
			}

			break;
		}
	case ASS_BANK_DEPOSIT:
	case ASS_GP_BANK_DEPOSIT:
		{
			bank_op_deposit *p = (bank_op_deposit*)pNetData;
			if (p != NULL)
			{
				if (p->_operate_type == 1) //生成存单
				{
					if (pNetHead->bHandleCode == HC_BANK_OP_SUC)
					{
						//存单生产成功，切换显示容器,显示报喜信息
						m_pSelFunNode->pFunsCtn->SetVisible(false);
						CContainerUI* pCtnCtrl = static_cast<CContainerUI*>(m_pm.FindControl("Ctn_Type1_Ctn5_Suc"));
						m_pSelFunNode->pFunsCtn = pCtnCtrl;
						m_pSelFunNode->bNeedSubmit = false;
						m_pSelFunNode->pFunsCtn = pCtnCtrl;
						//隐藏提交按钮的使用
						m_pSubmitBnt->SetVisible(false);

						CEditUI* pEditCtrl = NULL;
						CLabelUI* pLabCtrl = NULL;

						TCHAR szBuffer[128];

						//显示存单账号
						pEditCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_S1_Ctn5_Suc_Serial"));
						pEditCtrl->SetText(p->_deposit_id);

						//显示存单密码
						pEditCtrl = static_cast<CEditUI*>(m_pm.FindControl("Edt_S1_Ctn5_Suc_Pwd"));
						pEditCtrl->SetText(p->_deposit_password);

						//显示存单价值
						GetNumString(szBuffer,p->_money,Glb().m_nPowerOfGold,Glb().m_bUseSpace,Glb().m_strSpaceChar);
						pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_S1_Ctn5_Suc_Money"));
						pLabCtrl->SetText(szBuffer);

						//显示创建存单的手续费
						__int64 i64Tax = __int64(p->_money * (1.0 * p->_csds/100));
						GetNumString(szBuffer,i64Tax,Glb().m_nPowerOfGold,Glb().m_bUseSpace,Glb().m_strSpaceChar);
						pLabCtrl = static_cast<CLabelUI*>(m_pm.FindControl("Lab_S1_Ctn5_Suc_Tax"));
						pLabCtrl->SetText(szBuffer);

						pCtnCtrl->SetVisible(true);

						//更新本地金额显示
						m_i64BankMoney -= (p->_money + i64Tax);
						UpdateWealthInfor();

					}
					else if (pNetHead->bHandleCode == HC_BANK_OP_ERR_PWD)
					{
						DUIOkeyMsgBox(m_hWnd,"银行密码不正确");
						CEditUI* pPwdEdt = static_cast<CEditUI*>(m_pm.FindControl("Edt_S1_Ctn5_Pwd"));
						pPwdEdt->SetFocus();
					}
					else if (pNetHead->bHandleCode == HC_BANK_OP_NOT_MONEY)
					{

						DUIOkeyMsgBox(m_hWnd,"由于余额不足，不能产生存单");
					}
					else if (pNetHead->bHandleCode == HC_BANK_OP_NO_SERIAL)
					{
						DUIOkeyMsgBox(m_hWnd,"存单序列资源耗尽，请与客服联系");
					}
					return true;
				}
				else if (p->_operate_type == 2) //使用存单，即充值
				{
					if (pNetHead->bHandleCode == HC_BANK_OP_SUC)
					{
						TCHAR szNum[128];
						GetNumString(szNum,p->_money, Glb().m_nPowerOfGold,Glb().m_bUseSpace,Glb().m_strSpaceChar);
						ShowTipMessage(NULL,"PBankWnd2","UseDep_Msg_Success",
											"您转给ID[%d]，昵称[%s]的玩家充值 %s 金币",
											p->_user_id,p->_szNickName,szNum);

						//更新金币
						if (m_dwOpUserID == p->_user_id)
						{
							m_i64BankMoney += p->_money;
							UpdateWealthInfor();
						}
					}
					else if (pNetHead->bHandleCode == HC_BANK_OP_NO_USER)
					{
						ShowTipMessage(NULL,"PBankWnd2","UseDep_Msg_NoUser",
											"未能充值，该用户不存在。");
					}
					else if (pNetHead->bHandleCode == HC_BANK_OP_ERR_PWD)
					{
						ShowTipMessage(NULL,"PBankWnd2","UseDep_Msg_ErrPassword",
											"未能充值，充值卡密码不正确。");
					}
					else if (pNetHead->bHandleCode == HC_BANK_OP_INVDEP)
					{
						ShowTipMessage(NULL,"PBankWnd2","UseDep_Msg_Used",
											"未能充值，该充值卡已被使用。");
					}

					return true;
				}
			}
			break;
		}
	case ASS_BANK_WALLET:
	case ASS_GP_BANK_GETGM_WALLET:
		{
			bank_game_wallet* pWallet = (bank_game_wallet*)pNetData;
			if (pWallet != NULL)
			{
				if (pNetHead->bHandleCode == 0)
				{
					vector<TGmWalletNode>::iterator it = m_vecGmWallets.begin();
					while(it != m_vecGmWallets.end())
					{
						if (it->uNameID == pWallet->_game_id)
						{
							it->i64Wallet = pWallet->_money; //更新游戏钱包
							return true;
						}
						it++;
					}
					//在游戏列表上没有找到，添加一项，还要补充添加到游戏选择的下拉列表中
					TGmWalletNode tagNode;
					tagNode.uNameID = pWallet->_game_id;
					strcpy(tagNode.szGameName,pWallet->_game_name);
					tagNode.i64Wallet = pWallet->_money;
					m_vecGmWallets.push_back(tagNode);

					CComboUI* pCobCtrl = NULL;
					pCobCtrl = static_cast<CComboUI*>(m_pm.FindControl("Cob_S1_Ctn1_Games"));
					CListLabelElementUI* p = new CListLabelElementUI();
					if (p && pCobCtrl)
					{
						p->SetText(it->szGameName);
						pCobCtrl->Add(p);
					}
					
				}
				else //游戏钱包数据更新完成
				{
					CComboUI* pCobCtrl = static_cast<CComboUI*>(m_pm.FindControl("Cob_S1_Ctn1_Games"));

					vector<TGmWalletNode>::iterator it = m_vecGmWallets.begin();
					UINT uInNameID = 0;
					int iPos = 0;
					
					if (CPBankWndEx::ISGameRoom == m_emParWndType)
					{
						uInNameID = ((CGameRoomEx*)m_pRoom)->m_GameInfo.uNameID;
						while(it != m_vecGmWallets.end())
						{
							if (uInNameID == it->uNameID)
							{
								break;
							}
							iPos++;
							it++;
						}
					}
					
					pCobCtrl->SelectItem(iPos);
					pCobCtrl->SetEnabled(true);

					pCobCtrl = static_cast<CComboUI*>(m_pm.FindControl("Cob_S1_Ctn2_Game"));
					it = m_vecGmWallets.begin();
					while(it != m_vecGmWallets.end())
					{
						if (it->i64Wallet > 0) //只显示有钱的游戏
						{
							CListLabelElementUI* p = new CListLabelElementUI();
							p->SetText(it->szGameName);
							pCobCtrl->Add(p);
						}
						it++;
					}
					pCobCtrl->SetEnabled(true);

					UpdateWealthInfor();

				}
			}
			break;
		}
	case ASS_BANK_QUERY_DEPOITS:
	case ASS_GP_BANK_QUERY_DEPOITS:
		{
			bank_op_deposit_node* pDepNode = (bank_op_deposit_node*)pNetData;
			if (pDepNode != NULL)
			{
				if (pNetHead->bHandleCode == 0)
				{
					m_mapDeposits[pDepNode->_serial_no] = *pDepNode;

					CListContainerElementUI* pCtnCtrl = new CListContainerElementUI();
					pCtnCtrl->SetFixedHeight(55);

					CLabelUI* pLabCtrl = new CLabelUI();
					SIZE s = {5,3};
					pLabCtrl->SetFixedXY(s);
					pLabCtrl->SetFixedWidth(120);
					pLabCtrl->SetFixedHeight(15);
					pLabCtrl->SetMouseEnabled(false);
					pLabCtrl->SetFloat();

					char szBuffer[128];
					_stprintf(szBuffer,_T("%02d-%02d-%02d %02d:%02d:%02d"),
						pDepNode->_create_time.GetYear(),
						pDepNode->_create_time.GetMonth(),
						pDepNode->_create_time.GetDay(),
						pDepNode->_create_time.GetHour(),
						pDepNode->_create_time.GetMinute(),
						pDepNode->_create_time.GetSecond()
						);

					pLabCtrl->SetText(szBuffer);

					pCtnCtrl->Add(pLabCtrl);

					pLabCtrl = new CLabelUI();
					s.cx = 65;
					s.cy = 25;
					pLabCtrl->SetFixedXY(s);
					pLabCtrl->SetFixedWidth(60);
					pLabCtrl->SetFixedHeight(15);
					pLabCtrl->SetShowHtml(true);
					pLabCtrl->SetFloat();
					pLabCtrl->SetText("<c #00008080>存单消息</c>");
					pLabCtrl->SetMouseEnabled(false);
					pCtnCtrl->Add(pLabCtrl);


					CControlUI* pCtrl = new CControlUI();
					s.cx = 20;
					s.cy = 18;
					pCtrl->SetBkImage("BankWnd2\\item_ico.png");
					pCtrl->SetFixedXY(s);
					pCtrl->SetFixedWidth(32);
					pCtrl->SetFixedHeight(31);
					pCtrl->SetFloat();
					pCtrl->SetMouseEnabled(false);
					pCtnCtrl->Add(pCtrl);

					m_pDepList->Add(pCtnCtrl);
			
					
				}
				else if (pNetHead->bHandleCode == 1)
				{
					//加载Deposits完成！
					m_pDepList->NeedUpdate();

					bool bCtrlVisible = true;

					if (m_mapDeposits.size() == 0)
					{
						bCtrlVisible = false;
					}

					int idx = 0;
					CStdPtrArray* pCtrlArray = NULL;
					CControlUI* pCtrl = NULL;

					CControlUI* pRoot = m_pm.FindControl("Ctn_Type1_Ctn6");

					pCtrlArray = m_pm.FindSubControlsByClass(pRoot,_TEXT("LabelUI"));
					do 
					{
						pCtrl = static_cast<CButtonUI*>(pCtrlArray->GetAt(idx++));
						if (pCtrl)
						{
							pCtrl->SetVisible(bCtrlVisible);
						}
					} while (pCtrl);

					idx = 0;
					pCtrlArray = m_pm.FindSubControlsByClass(pRoot,_TEXT("EditUI"));
					do 
					{
						pCtrl = static_cast<CButtonUI*>(pCtrlArray->GetAt(idx++));
						if (pCtrl)
						{
							pCtrl->SetVisible(bCtrlVisible);
						}
					} while (pCtrl);

					idx = 0;
					pCtrlArray = m_pm.FindSubControlsByClass(pRoot,_TEXT("ListUI"));
					do 
					{
						pCtrl = static_cast<CButtonUI*>(pCtrlArray->GetAt(idx++));
						if (pCtrl)
						{
							pCtrl->SetVisible(bCtrlVisible);
						}
					} while (pCtrl);

					pCtrl = m_pm.FindControl("Lab_S1_Ctn6_9");
					pCtrl->SetVisible(!bCtrlVisible);
				}
			}
			break;
		}

	case ASS_GP_BANK_CHPWD:	//< 修改密码
		{
			if (pNetHead->bHandleCode == HC_GP_BANK_CHPWD_SUC)
			{
				ShowTipMessage(NULL,"GameRoom","ChangePasswdOK","修改银行密码成功！请妥善保管新密码。");
			}
			else if (pNetHead->bHandleCode == HC_GP_BANK_CHPWD_ERR_PWD)
			{
				ShowTipMessage(NULL,"GameRoom","ChangePasswdFailed","修改银行密码失败，旧银行不正确！");
			}
			break;
		}
	}

	return true;
}