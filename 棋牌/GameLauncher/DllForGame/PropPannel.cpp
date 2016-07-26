// PropPannel.cpp : 实现文件
//

#include "stdafx.h"
#include "PropPannel.h"
#include <comutil.h>
#include "Platform/confirmdlg.h"
#include "resource.h"
#include "DlgNotify.h"
#include "Platform/bzsound.h"

#include "BoardCast.h"//
#include "IGameImpl.h"
#include "NewGameFrame\XLogic.h"
#include "IPC\ChannelService.h"

#define  MAXINPUTCOUNT										29
#ifdef _DEBUG
#	pragma comment (lib,"comsupp.lib")
#else
#	pragma comment (lib,"comsuppw.lib")
#endif

struct MSG_PROP_RESULT
{
	DWORD dwResult;
};

extern EXT_CLASS TCHAR *GlbGetNumString(TCHAR str[], __int64 nNum, int nPower, bool bHasSpace, TCHAR* chSpace);

// CPropPannel 对话框

//
//#define  BUTTON_BMP  "..\\image\\client\\skin0\\logon\\button.bmp"

#define  VIPPROP								9999      //vip道具

IMPLEMENT_DYNAMIC(CPropPannel, CDialog)

CPropPannel::CPropPannel(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_PROPPANNEL, pParent)
	, m_nHoldCount(0)
	, m_nBuyCount(0)
	, m_strBuyMoney(_T(""))
	, m_sGiveUser(_T(""))
	, m_sUseUser(_T(""))
	, m_nGiveCount(0)
{
	curBuyCount = 1;
	curVIPType = 0;
	imgDialog=0;
	m_bGiveOrCancle = false;
	m_bGibe = false;
	CString s=CINIFile::GetAppPath ();/////本地路径
	CBcfFile f( s + "..\\" + m_skinmgr.GetSkinBcfFileName());
	CString key=TEXT("PropPannel");
	int r,g,b;
	r=f.GetKeyVal(key,"PropPannelR",215);
	g=f.GetKeyVal(key,"PropPannelG",238);
	b=f.GetKeyVal(key,"PropPannelB",253);
	m_bkcolor=RGB(r,g,b);

	CBcfFile fini(s+"..\\bzgame.bcf");
	m_strPropUrl=fini.GetKeyVal("BZW","PropPannelUrl","http://game.bzw.cn/app/Prop/PropInfo.asp");
	m_strPropVIPUrl=fini.GetKeyVal("BZW","PropVIPUrl","http://game.bzw.cn/app/Prop/VipApply.aspx?");
	m_nOpenFrom=0;
	m_bkBrush=NULL;

	m_bIsVIPButton = FALSE;
	m_bClickCancelGive = FALSE;
	m_bClickCancelUse = FALSE;

	m_UsingGive = false;
	m_UsingUse = false;
	m_UsingBuy = false;

	m_bUsingNewUI = false;

	m_pLogic=NULL;
}

CPropPannel::~CPropPannel()
{
	DeleteObject(m_bkBrush);
}

void CPropPannel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TIP, m_ctlTip);
	DDX_Control(pDX, IDC_CLOSE, m_bnClose);
	DDX_Control(pDX, IDC_EXPLORER1, m_ie);

	DDX_Text(pDX, IDC_HOLDCOUNT, m_nHoldCount);
	DDX_Text(pDX, IDC_BUYCOUNT, m_nBuyCount);
	DDX_Text(pDX, IDC_NEEDMONEY, m_strBuyMoney);
	DDX_Control(pDX, IDC_GIVEUSER, m_ctlGiveUser);
	DDX_Control(pDX, IDC_USEUSER, m_ctlUseUser);
	DDX_Control(pDX, IDC_USE, m_bnUse);
	DDX_Control(pDX, IDC_BUY, m_bnBuy);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_bnClose2);
	DDX_Control(pDX, IDC_GIVE, m_bnGive);
	//DDX_Control(pDX, IDCANCEL, m_cancelBtn);
	DDX_Control(pDX, IDC_BUYCOUNT, m_ctlBuyCount);
	
	DDV_MaxChars(pDX, m_sGiveUser, 30);
	DDX_Text(pDX, IDC_GIVECOUNT, m_nGiveCount);
	DDX_Control(pDX, IDC_HOLDCOUNT, m_uCount);
	DDX_Control(pDX, IDC_STATIC_MOUNT, m_uCountTip);

	DDX_Text(pDX, IDC_GIVEUSER, m_sGiveUser);
	DDX_Text(pDX, IDC_USEUSER, m_sUseUser);
	DDX_Control(pDX, IDC_GIVECOUNT, m_ctlGiveCount);
	DDX_Control(pDX, IDC_STATIC_COUNT, m_szGiveCount);
	DDX_Control(pDX, IDC_STATIC_BUYCOUNT, m_szBuyCount);
	DDX_Control(pDX, IDC_STATIC_GAVEUSER, m_szGiveName);
	DDX_Control(pDX, IDC_GIVEUSER_NEW, m_GiveName);
	DDX_Control(pDX, IDC_TIP2, m_tip2);
	DDX_Control(pDX, IDC_BUTTON_CANCLEBUY, m_bnCancleBuy);
	DDX_Control(pDX, IDC_BUTTON_CANCLEGIVE, m_bnCancleGive);
	DDX_Control(pDX, IDC_BUTTON_CANCLEUSE, m_bnCancleUse);
	DDX_Control(pDX, IDC_BUTTON_SUB, m_bnAdd);
	DDX_Control(pDX, IDC_BUTTON_ADD, m_bnSub);

	//DDX_Control(pDX, IDC_HOLDCOUNT, m_nHoldCountCtr);
}

BEGIN_MESSAGE_MAP(CPropPannel, CDialog)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_CLOSE, &CPropPannel::OnBnClickedClose)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CPropPannel::OnBnClickedClose2)
	ON_WM_LBUTTONDOWN()
	ON_EN_CHANGE(IDC_BUYCOUNT, &CPropPannel::OnEnChangeBuycount)
	ON_EN_CHANGE(IDC_GIVECOUNT, &CPropPannel::OnEnChangeGivecount)
	ON_BN_CLICKED(IDC_BUY, &CPropPannel::OnBnClickedBuy)
	ON_BN_CLICKED(IDC_USE, &CPropPannel::OnBnClickedUse)
	ON_BN_CLICKED(IDC_GIVE, &CPropPannel::OnBnClickedGive)
	ON_BN_CLICKED(IDC_BUTTON_CANCLEGIVE, &CPropPannel::OnBnClickedCancleGive)
	ON_BN_CLICKED(IDC_BUTTON_CANCLEUSE, &CPropPannel::OnBnClickedCancleUse)
	ON_BN_CLICKED(IDC_BUTTON_CANCLEBUY, &CPropPannel::OnBnClickedCancleBuy)
	ON_BN_CLICKED(IDC_BUTTON_SUB, &CPropPannel::OnBnClickedSubCount)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CPropPannel::OnBnClickedAddCount)
	ON_CBN_SELCHANGE(IDC_USEUSER, &CPropPannel::OnCbnSelChangeUseUser)
	ON_CBN_SELCHANGE(IDC_GIVEUSER, &CPropPannel::OnCbnSelChangeGiveUser)
	ON_CBN_EDITCHANGE(IDC_GIVEUSER, &CPropPannel::OnEnChangeGiveuser)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CPropPannel 消息处理程序

BOOL CPropPannel::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect rc;

	setSkinIndex(0);
//	SetWindowText("道具面板");

	TCHAR path[MAX_PATH];
	wsprintf(path,"..\\%slogon\\button.bmp",m_skinmgr.GetSkinPath());
	m_bnUse.LoadButtonBitmap(path,false);
	m_bnBuy.LoadButtonBitmap(path,false);
	m_bnGive.LoadButtonBitmap(path,false);
	m_bnClose2.LoadButtonBitmap(path,false);
	m_bnCancleGive.LoadButtonBitmap(path,false);
	m_bnCancleUse.LoadButtonBitmap(path,false);
	m_bnCancleBuy.LoadButtonBitmap(path,false);
	wsprintf(path,"..\\%slogon\\Left.bmp",m_skinmgr.GetSkinPath());
	m_bnAdd.LoadButtonBitmap(path,false);
	//Rect rc;

	m_ctlGiveCount.GetWindowRect(&rc);
	ScreenToClient(&rc);
	rc.left = rc.left - 17 - 7;
	rc.right = rc.left + 17;
	m_bnAdd.MoveWindow(&rc);

	wsprintf(path,"..\\%slogon\\Right.bmp",m_skinmgr.GetSkinPath());
	m_bnSub.LoadButtonBitmap(path,false);

	m_ctlGiveCount.GetWindowRect(&rc);
	ScreenToClient(&rc);
	rc.left = rc.right + 7;
	rc.right = rc.left + 17;
	m_bnSub.MoveWindow(&rc);

	//m_cancelBtn.LoadButtonBitmap(path,false);

	CString s=CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f( s + "..\\" + m_skinmgr.GetSkinBcfFileName());
	CString key=TEXT("PropPannel");
	m_bkcolor_R = f.GetKeyVal(key,"PropPannelR",255);
	m_bkcolor_G = f.GetKeyVal(key,"PropPannelG",255);
	m_bkcolor_B = f.GetKeyVal(key,"PropPannelB",255);
	m_bkcolor=RGB(m_bkcolor_R,m_bkcolor_G,m_bkcolor_B);

	SetIcon(AfxGetApp()->LoadIcon(IDI_CLIENT_LIST),FALSE);

	m_ctlGiveUser.LimitText(MAXINPUTCOUNT);
	//m_bkBrush=CreateSolidBrush(m_bkcolor);

	GetWindowRect(&rc);
	m_bnClose.MoveWindow(rc.Width()-36,0,36,20);

	/// 获取按钮标准位置
	m_bnCancleUse.GetWindowRect(&m_BtnRect1);
	ScreenToClient(&m_BtnRect1);
	m_bnCancleBuy.GetWindowRect(&m_BtnRect2);
	ScreenToClient(&m_BtnRect2);
	m_bnCancleGive.GetWindowRect(&m_BtnRect3);
	ScreenToClient(&m_BtnRect3);
	m_bnClose2.GetWindowRect(&m_BtnRect4);
	ScreenToClient(&m_BtnRect4);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CPropPannel::setSkinIndex(int sk)
{
	try
	{
		CString s=CBcfFile::GetAppPath()+"..\\";//本地路径
		CString strSkin = m_skinmgr.GetSkinBcfFileName();
		CBcfFile f( s + strSkin);
		TCHAR path[MAX_PATH];
		CString skinfolder= s + f.GetKeyVal(m_skinmgr.GetKeyVal(strSkin),"skinfolder",m_skinmgr.GetSkinPath());

		wsprintf(path,"%sdialog\\PropPanel_bk_gr.bmp",skinfolder);
		CRect cRect;
		GetWindowRect(&cRect);
		m_bk.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
		MoveWindow(cRect.left, cRect.top, m_bk.GetWidth(), m_bk.GetHeight(), TRUE);

#ifndef BZ_ZLIB
		imgDialog=Image::FromFile(s.AllocSysString());
#else
		CUnZipRes unZipRes;
		IStream * pStream=NULL;
		char * pchar=s.GetBuffer(s.GetLength()+1);
		pStream=unZipRes.LoadFileResFromZip(pchar);

		imgDialog=new Image(pStream);
		if(pStream!=NULL)
		{
			pStream->Release();
			pStream=NULL;
		}
#endif
		
		wsprintf(path,"..\\%sdialog\\Rect_Close.bmp",m_skinmgr.GetSkinPath());
		m_bnClose.LoadButtonBitmap(path,false);
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"PROP_PANNEL_TRY_CATCH : [%s]",__FUNCTION__);
		////OutputDebugString(smsg);
	}
	// TODO:  在此添加额外的初始化


}
void CPropPannel::OnPaint()
{
//	CPaintDC dc(this); // device context for painting
	drawSkin();
	/*if(!m_ie.get_Busy())
		m_ie.Refresh();*/
//	__super::OnPaint();
}

void CPropPannel::drawSkin(void)
{
	try
	{
// 		if(!imgDialog)
// 			return;
// 		CDC *pdc=this->GetWindowDC();
// 		if(!pdc) return;

// 		CRect rc;
// 		this->GetWindowRect(&rc);
// 		Graphics g(pdc->m_hDC);
// 		g.DrawImage(imgDialog,Rect(0,0,imgDialog->GetWidth(),imgDialog->GetHeight()),0,0,imgDialog->GetWidth(),imgDialog->GetHeight(),UnitPixel);


		CPaintDC dc(this); // device context for painting
		CGameImageHelper help(&m_bk);
		help.BitBlt(dc.GetSafeHdc(),0,0);


// 		CString s="宋体";
// 
// 		Font font(s.AllocSysString(),12,0,UnitPixel);
// 		SolidBrush brush(Color(255,255,255));
// // 		s="道具面板";
// // 		g.DrawString(s.AllocSysString(),-1,&font,PointF(5,5),0,&brush);
// 		g.ReleaseHDC(pdc->m_hDC);
// 		m_bnClose.Invalidate(FALSE);
		processLocalProp();

// 		m_bnGive.Invalidate(); // test
// 		m_bnClose2.Invalidate();
// 		m_bnCancleGive.Invalidate();
// 		m_bnBuy.Invalidate();
// 		m_bnUse.Invalidate();

// 		UpdateWindow();
	
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"LOGO_UP_TRY_CATCH : [%s]",__FUNCTION__);
	}
}

void CPropPannel::OnBnClickedClose()
{
	this->ShowWindow(SW_HIDE);
}

void CPropPannel::OnLButtonDown(UINT nFlags, CPoint point)
{
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
	CDialog::OnLButtonDown(nFlags, point);
}

LRESULT CPropPannel::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		//case WM_CTLCOLORSCROLLBAR :
		//case WM_CTLCOLORBTN:
		//case WM_CTLCOLORMSGBOX:
		//case WM_CTLCOLOREDIT:
		//case WM_CTLCOLORLISTBOX:
	case WM_CTLCOLORDLG:
	case WM_CTLCOLORSTATIC :
		SetTextColor((HDC)wParam, RGB(0,0,0)) ;
		SetBkColor((HDC)wParam, m_bkcolor);

		if (m_bkBrush == NULL) m_bkBrush=CreateSolidBrush(m_bkcolor);
		return (LRESULT)m_bkBrush;
	}
	return CDialog::WindowProc(message, wParam, lParam);

}

LRESULT CPropPannel::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT lrst=CDialog::DefWindowProc(message, wParam, lParam);

	if (!::IsWindow(m_hWnd))
		return lrst;

	//if (message==WM_MOVE||message==WM_PAINT||message==WM_NCPAINT||message==WM_NCACTIVATE)// ||message == WM_NOTIFY)
	//{
	//	drawSkin();
	//}
	return lrst;
}
void CPropPannel::ResetData(void)
{
	CString stip;
	TCHAR szNum[128]; 
    GlbGetNumString(szNum, m_pGameInfo->uisMeUserInfo.GameUserInfo.i64Bank, Glb().m_nPowerOfGold, Glb().m_bUseSpace, Glb().m_strSpaceChar);
	stip.Format("购买道具使用的是您银行中的金币，您现在银行中有 %s 金币。", szNum);
	m_ctlTip.SetWindowText(stip);
	processLocalProp();
}
void CPropPannel::openPannel(int openFrom,CString strGiveUser, int propID)
{
	//CMainRoomEx *pWnd=(CMainRoomEx*)pParentWnd;
	//if(!pParentWnd)
	//{
	//	for(int i = 0; i < MAX_GAME_ROOM; i ++)
	//	{
	//		if(pWnd->m_RoomInfo[i].pGameRoomWnd != NULL)
	//			((CGameRoomEx*)pWnd->m_RoomInfo[i].pGameRoomWnd)->InsertSystemMessageWithGame("对不起，打开道具箱出错");
	//	}
	//	return ;
	//}
	curPropID = propID;
	// test， 先取消赠送功能
// 	if(TRUE == m_UsingGive)
// 		OnBnClickedCancleGive();
// 	if(TRUE == m_UsingUse)
// 		OnBnClickedCancleUse();
// 	if(TRUE == m_UsingBuy)
// 		OnBnClickedCancleBuy();

	m_UsingUse = FALSE;
	m_UsingBuy = FALSE;
	m_UsingGive = FALSE;
	FixDefaultCtl();

	m_bIsVIPButton = FALSE;
	if (propID == VIPPROP)                            //modify by wyl     11-5-18
	{
		m_bnCancleUse.ShowWindow(SW_HIDE);
		m_ctlGiveCount.ShowWindow(SW_HIDE);
		m_bnSub.ShowWindow(SW_HIDE);
		m_bnAdd.ShowWindow(SW_HIDE);
		m_szGiveCount.ShowWindow(SW_HIDE);
		m_ctlBuyCount.ShowWindow(SW_HIDE);
		m_szBuyCount.ShowWindow(SW_HIDE);
		m_bIsVIPButton = TRUE;

		GetDlgItem(IDC_HOLDCOUNT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_MOUNT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_VIPINFO)->ShowWindow(SW_SHOW);

		m_nHoldCount = 100;                          //VIP最多赠送100个
	}
	else
	{
		m_bnCancleUse.ShowWindow(SW_SHOW);
		GetDlgItem(IDC_HOLDCOUNT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_MOUNT)->ShowWindow(SW_SHOW);

		GetDlgItem(IDC_STATIC_VIPINFO)->ShowWindow(SW_HIDE);
	}

	CBcfFile fMsg(CBcfFile::GetAppPath()+"..\\ClientMessage.bcf");
	m_tip2.SetWindowText(fMsg.GetKeyVal("PropPanelDlg","TipUseBankCoin","提示：购买道具使用的是您银行中的金币"));

	//判断是否是会员卡道具，propID为1 默认为会员卡道具
	if(openFrom!=-1)
		m_nOpenFrom=openFrom;
	CString stip;
	TCHAR szNum[128]; 
    GlbGetNumString(szNum, m_pGameInfo->uisMeUserInfo.GameUserInfo.i64Bank, Glb().m_nPowerOfGold, Glb().m_bUseSpace, Glb().m_strSpaceChar);
	stip.Format("购买道具使用的是您银行中的金币，您现在银行中有 %s 金币。", szNum);
	m_ctlTip.SetWindowText(stip);
	m_ctlGiveUser.SetWindowText(strGiveUser);
	this->ShowWindow(SW_SHOW);
	
	if (propID != VIPPROP)
	{
		//surl.Format("%s?propid=%d&bgcolor=%X%X%X",m_strPropUrl,propID,m_bkcolor_R,m_bkcolor_G,m_bkcolor_B);
		m_surl.Format("%s?userid=%d&token=%s&propid=%d&bgcolor=%X%X%X",m_strPropUrl,
			m_pGameInfo->uisMeUserInfo.GameUserInfo.dwUserID, Glb().m_TML_SN,
			propID,m_bkcolor_R,m_bkcolor_G,m_bkcolor_B);
	}
	else
	{
		//surl.Format("%s?propid=%d&bgcolor=%X%X%X",m_strPropVIPUrl,propID,m_bkcolor_R,m_bkcolor_G,m_bkcolor_B);
		m_surl.Format("%s?userid=%d&token=%s&propid=%d&bgcolor=%X%X%X",m_strPropVIPUrl,
			m_pGameInfo->uisMeUserInfo.GameUserInfo.dwUserID, Glb().m_TML_SN,
			propID,m_bkcolor_R,m_bkcolor_G,m_bkcolor_B);
	}
	OutputDebugString(m_surl);

	SetIEOnPropDlg();

	if(m_bUsingNewUI)
	{
		if (m_pLogic != NULL)
		{
			CChannelService *p = m_pLogic->GetChannelServer();
			p->SendData(IPC_FRIEND, IPC_ASS_GETFRIENDLIST);
		}
	}
	else
	{
		this->m_pParent->PostMessage(WM_GETFRIENDLIST, 0, 0);
	}
}

BEGIN_EVENTSINK_MAP(CPropPannel, CDialog)
	ON_EVENT(CPropPannel, IDC_EXPLORER1, 250, CPropPannel::BeforeNavigate2Explorer1, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
	ON_EVENT(CPropPannel, IDC_EXPLORER1, 271, CPropPannel::NavigateErrorExplorer1, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
END_EVENTSINK_MAP()

void CPropPannel::BeforeNavigate2Explorer1(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers, BOOL* Cancel)
{
	if(URL->vt!=VT_BSTR)
		return;
	CString surl=(char*)_bstr_t(URL->bstrVal);
	surl.MakeLower();


	if(surl.Left(6)=="bzw://")
	{
		*Cancel=TRUE;
		if(surl.Find("prop/change")==6)
		{
			curPropID=atoi(GetUrlValue(surl,"propid"));
			curPropPrice=atoi(GetUrlValue(surl,"price"));
			curPropVipPrice=atoi(GetUrlValue(surl,"vipprice"));
			curVIPType = atoi(GetUrlValue(surl,"viptype"));
			curBuyCount = atoi(GetUrlValue(surl,"num"));
			processLocalProp();
		}
	}
}

void CPropPannel::NavigateErrorExplorer1(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Frame, VARIANT* StatusCode, BOOL* Cancel)
{
	//this->ShowWindow(SW_HIDE);
	//CMainRoomEx *pWnd=(CMainRoomEx*)pParentWnd;
	//for(int i = 0; i < MAX_GAME_ROOM; i ++)
	//{
	//	if(pWnd->m_RoomInfo[i].pGameRoomWnd != NULL)
	//	{
	//		((CGameRoomEx*)pWnd->m_RoomInfo[i].pGameRoomWnd)->InsertSystemMessageWithGame("对不起，打开道具箱出错");
	//	}
	//}
}

void CPropPannel::processLocalProp()
{
	UpdateData(TRUE);	//将DDX_Text里面的内容赋值到控件中，否则后面执行UpdateData(FALSE)时会导致CComboBox选择无效。
	int propCount=Glb().userPropLibrary.GetCount();
	_TAG_USERPROP * userProp=NULL;
	bool bFoundProp=false;
	for(int i=0;i<propCount;i++)
	{
		userProp=Glb().userPropLibrary.GetAt(i);
		if(userProp->nPropID==curPropID)
		{
			bFoundProp=true;
			break;
		}
	}
//	CMainRoomEx *pWnd=(CMainRoomEx*)pParentWnd;
	CString stime;
	stime.Format("%d",CTime::GetCurrentTime());
	long curtime=atol(stime);
	int nPrice=curPropPrice;
	if(m_pGameInfo->uisMeUserInfo.GameUserInfo.iVipTime>curtime)
		nPrice=curPropVipPrice;
	m_bnUse.EnableWindow(FALSE);
	if (curPropID != VIPPROP)
	{
		m_bnGive.EnableWindow(FALSE);
	}
	
	m_bnGive.EnableWindow(TRUE);
	if (curPropID == VIPPROP)
	{	
		m_ctlGiveUser.EnableWindow(TRUE);
		m_ctlUseUser.EnableWindow(TRUE);
	}
	else
	{
		m_ctlGiveUser.EnableWindow(FALSE);
		m_ctlUseUser.EnableWindow(FALSE);
	}

	//m_ctlGiveCount.EnableWindow(FALSE);
	if (curPropID != VIPPROP)
	{
		if(bFoundProp)
		{
			m_nHoldCount=userProp->nHoldCount;
			if(m_nHoldCount>0)
			{
				m_ctlGiveUser.EnableWindow(TRUE);
				m_ctlUseUser.EnableWindow(TRUE);
				if(m_UsingGive)
				{
					m_ctlGiveUser.ShowWindow(SW_SHOW);
					m_ctlUseUser.ShowWindow(SW_HIDE);
				}
				else if(m_UsingUse)
				{
					m_ctlGiveUser.ShowWindow(SW_HIDE);
					m_ctlUseUser.ShowWindow(SW_SHOW);
				}

				CString rString;
				m_ctlGiveUser.GetWindowText(rString);
				if(rString.Trim()!="")
				{
					m_bnGive.EnableWindow(TRUE);
					m_ctlGiveCount.EnableWindow(TRUE);
				}
				m_ctlUseUser.GetWindowText(rString);
				if(rString.Trim()!="")
				{
					m_bnUse.EnableWindow(TRUE);
				}
			}
		}
		else
			m_nHoldCount=0;
	}
	m_nBuyCount=1;
	TCHAR szNum[128]; 
    GlbGetNumString(szNum, nPrice*m_nBuyCount, Glb().m_nPowerOfGold, Glb().m_bUseSpace, Glb().m_strSpaceChar);
	m_strBuyMoney.Format("(您是%s,需要 %s 金币)",(m_pGameInfo->uisMeUserInfo.GameUserInfo.iVipTime>curtime)?"VIP会员":"普通用户", szNum);
	UpdateData(FALSE);
	//m_ctlGiveCount.SetWindowText("1");
}

CString CPropPannel::GetUrlValue(CString szUrl, CString szField)
{
	szField=szField+"=";
	CString sValue="";
	int nStart=szUrl.Find(szField);
	if(nStart==-1)
		return "";
	nStart+=szField.GetLength();
	int nStop=szUrl.Find("&",nStart);
	if(nStop==-1)
		nStop=szUrl.GetLength();
	sValue= szUrl.Mid(nStart,nStop-nStart);
	return sValue;
}

void CPropPannel::OnEnChangeBuycount()
{
	CString rString;
	m_ctlBuyCount.GetWindowText(rString);
	if(atoi(rString)==0)
		m_ctlBuyCount.SetWindowText("1");
	UpdateData();
	if(m_nBuyCount>255)
		m_nBuyCount=255;
//	CMainRoomEx *pWnd=(CMainRoomEx*)pParentWnd;
	CString stime;
	stime.Format("%d",CTime::GetCurrentTime());
	long curtime=atol(stime);
	int nPrice=curPropPrice;
	if(m_pGameInfo->uisMeUserInfo.GameUserInfo.iVipTime>curtime)
		nPrice=curPropVipPrice;
	TCHAR szNum[128]; 
    GlbGetNumString(szNum, nPrice*m_nBuyCount, Glb().m_nPowerOfGold, Glb().m_bUseSpace, Glb().m_strSpaceChar);
	m_strBuyMoney.Format("(您是%s,需要 %s 金币)",
		(m_pGameInfo->uisMeUserInfo.GameUserInfo.iVipTime>curtime)?"VIP会员":"普通用户",
		szNum);
	UpdateData(FALSE);
	
	__int64 _Price = nPrice;
	if(_Price*m_nBuyCount>m_pGameInfo->uisMeUserInfo.GameUserInfo.i64Bank)
		m_bnBuy.EnableWindow(FALSE);
	else
		m_bnBuy.EnableWindow(TRUE);
}

void CPropPannel::OnEnChangeGiveuser()
{
	UpdateData();
	m_bnCancleGive.Invalidate();
	m_bnGive.Invalidate();
	m_bnClose2.Invalidate();
	CString sGiveUser;
	m_ctlGiveUser.GetWindowText(sGiveUser);
	sGiveUser.Trim();
	if(sGiveUser=="")
	{
		m_ctlGiveCount.EnableWindow(FALSE);
		if (curPropID != VIPPROP)
		{
			m_bnGive.EnableWindow(FALSE);
			m_bnUse.EnableWindow(FALSE);
		}
		UpdateData();
	}	
	else
	{
		m_ctlGiveCount.EnableWindow(TRUE);
		m_bnGive.EnableWindow(TRUE);
		m_bnUse.EnableWindow(TRUE);
		OnEnChangeGivecount();
	}
	
}

void CPropPannel::OnEnChangeGivecount()
{
	try
	{
		CString rString;
		m_ctlGiveCount.GetWindowText(rString);

		if("" == rString.Trim())
			m_ctlGiveCount.SetWindowText("0");

		UpdateData(TRUE);

		if (curPropID != VIPPROP)
		{
			if(m_nGiveCount>m_nHoldCount)
				m_nGiveCount=m_nHoldCount;
		}

		CString sGiveUser;
		m_ctlGiveUser.GetWindowText(sGiveUser);
		sGiveUser.Trim();
		if(m_nHoldCount>0 && sGiveUser!="")
			m_bnGive.EnableWindow(TRUE);
		else if (curPropID != VIPPROP)
		{
			m_bnGive.EnableWindow(FALSE);
		}
		UpdateData(FALSE);
	}
	catch(...)
	{
		DUIMessageBox(GetSafeHwnd(),MB_OK|MB_ICONINFORMATION,"提示","赠送道具异常");
		return;
	}
}

void CPropPannel::OnCbnSelChangeUseUser()
{
	CString rString;
	m_ctlUseUser.GetWindowText(rString);
	if(rString.Trim()!="")
	{
		m_bnUse.EnableWindow(TRUE);
	}
}

void CPropPannel::OnCbnSelChangeGiveUser()
{
	OnEnChangeGiveuser();
}


void CPropPannel::OnCbnEditUpdateGiveuser()
{
	OnEnChangeGiveuser();
}

void CPropPannel::OnBnClickedBuy()
{
	UpdateData();

	if (curPropID == VIPPROP)     //如果ID= 9999为VIP购买，即买即用
 	{ 
		_TAG_PROP_BUY_VIP *pPropVIPBuy = new _TAG_PROP_BUY_VIP;
		CString stime;
		stime.Format("%d",CTime::GetCurrentTime());
		long curtime=atol(stime);
		int nPrice=curPropPrice;
		if(m_pGameInfo->uisMeUserInfo.GameUserInfo.iVipTime>curtime)
		{
			BZSoundPlay(this, "..\\music\\错误提示.mp3", 0);
			AFCMessageBox("您已经是VIP会员了，不需要重复购买","提示");
			return;
		}
		
		pPropVIPBuy->dwUserID=m_pGameInfo->uisMeUserInfo.GameUserInfo.dwUserID;		//购买者ID
		pPropVIPBuy->nPropType = curVIPType;										//道具类型 ，需要取网页返回
		_tcscpy_s(pPropVIPBuy->szTargetUser, 32, m_sGiveUser);						//被赠送者ID
		
		this->m_pParent->PostMessage(WM_BUY_VIP, sizeof(_TAG_PROP_BUY_VIP), (LPARAM)pPropVIPBuy);
		UpdateData(FALSE);
		ShowWindow(SW_HIDE);
		return;
 	}


	//ConfirmDlg confirmDlg("您确定要购买吗？","提示",AFC_YESNO,this);
	//if (confirmDlg.DoModal() == IDQUXIAO)
	//	return;

	/*if(AFCMessageBox("您确定要购买吗？","提示",AFC_YESNO) == IDQUXIAO)
		return;*/
//	CMainRoomEx *pWnd=(CMainRoomEx*)pParentWnd;

	_TAG_PROP_BUY *pPropBuy = new _TAG_PROP_BUY;
	pPropBuy->dwUserID=m_pGameInfo->uisMeUserInfo.GameUserInfo.dwUserID;
	pPropBuy->nPropID=curPropID;
	CString stime;
	stime.Format("%d",CTime::GetCurrentTime());
	long curtime=atol(stime);
	int nPrice=curPropPrice;
	if(m_pGameInfo->uisMeUserInfo.GameUserInfo.iVipTime>curtime)
		nPrice=curPropVipPrice;
	pPropBuy->iPropPayMoney=nPrice*m_nBuyCount;
	CString s=CBcfFile::GetAppPath();
	CBcfFile fini(s+"..\\bzgame.bcf");
	int bBuyCount=fini.GetKeyVal("BZW","bBuyCount",1);
	
	pPropBuy->nPropBuyCount=m_nBuyCount;//curBuyCount;

	//pWnd->m_TCPSocket.SendData(&propBuy,sizeof(_TAG_PROP_BUY),MDM_GP_PROP,ASS_PROP_BUY,0);
	char temp[100]= {0};
	this->m_pParent->PostMessage(WM_PROP_BUY, sizeof(_TAG_PROP_BUY), (LPARAM)pPropBuy);
	UpdateData(FALSE);
	ShowWindow(SW_HIDE);
}

void CPropPannel::OnGetPropInformation(_TAG_USERPROP* userProp)
{
	if(userProp->nPropID==curPropID)
	{
		UpdateData();
		if (curPropID != VIPPROP)
		{
			m_nHoldCount=userProp->nHoldCount;
		}
		

		UpdateData(FALSE);
	}		
	//需要修改本地金币
//	CMainRoomEx *pWnd=(CMainRoomEx*)pParentWnd;
	CString stime;
	stime.Format("%d",CTime::GetCurrentTime());
	long curtime=atol(stime);
	int nPrice=curPropPrice;
	if(m_pGameInfo->uisMeUserInfo.GameUserInfo.iVipTime>curtime)
		nPrice=curPropVipPrice;
	m_pGameInfo->uisMeUserInfo.GameUserInfo.i64Bank-=nPrice*m_nBuyCount;
	CString stip;
	TCHAR szNum[128]; 
    GlbGetNumString(szNum, m_pGameInfo->uisMeUserInfo.GameUserInfo.i64Bank, Glb().m_nPowerOfGold, Glb().m_bUseSpace, Glb().m_strSpaceChar);
	stip.Format("购买道具使用的是您银行中的金币，您现在银行中有 %s 金币。", szNum);
	m_ctlTip.SetWindowText(stip);

	if(m_nHoldCount>0)
	{
		m_bnUse.EnableWindow(TRUE);
		m_ctlGiveUser.EnableWindow(TRUE);
		m_ctlUseUser.EnableWindow(TRUE);
		CString rString;
		m_ctlGiveUser.GetWindowText(rString);
		if(rString.Trim()!="")
		{
			m_bnGive.EnableWindow(TRUE);
			m_ctlGiveCount.EnableWindow(TRUE);
		}
		m_ctlUseUser.GetWindowText(rString);
		if(rString.Trim()!="")
		{
			m_bnUse.EnableWindow(TRUE);
		}
	}
}

void CPropPannel::OnBnClickedUse()
{
	UpdateData();
	
	_TAG_USERPROP * userProp=NULL;
	int nPropIndex=-1;
	int propCount=Glb().userPropLibrary.GetCount();
	for(int i=0;i<propCount;i++)
	{
		userProp=Glb().userPropLibrary.GetAt(i);
		if(userProp->nPropID==curPropID)
		{
			nPropIndex=i;
			break;
		}
	}

	if(IsPropUseTarget(curPropID))
	{//对其它人使用
		if(m_sUseUser == "")
		{
			AFCMessageBox("使用对象不能为空！","提示");
		}
		else
		{
			for(int i=0; i<GetMaxPeopleCount(); i++)
			{
				BOOL bFind = FALSE;
				UserItemStruct *pUser = GetUserItem(i);
				if((pUser != NULL)
					&&(!strcmp(m_sUseUser, pUser->GameUserInfo.nickName)))///在用户列表中查找匹配的使用目标
				{
					if(nPropIndex<0 || nPropIndex>=Glb().userPropLibrary.GetCount())
						return ;

					_TAG_USERPROP * userProp=Glb().userPropLibrary.GetAt(nPropIndex);
					if(userProp->nHoldCount<=0)
						return ;

					_TAG_USINGPROP  usingProp;
					ZeroMemory(&usingProp,sizeof(_TAG_USINGPROP));
					usingProp.dwUserID=m_pGameInfo->uisMeUserInfo.GameUserInfo.dwUserID;
					usingProp.dwTargetUserID=pUser->GameUserInfo.dwUserID;
					usingProp.nPropID=curPropID;
					usingProp.nPropActionAttrib=userProp->attribAction;
					usingProp.nPropValueAttrib=userProp->attribValue;

					memcpy(usingProp.szPropName,userProp->szPropName,sizeof(userProp->szPropName));
					SendGameData(&usingProp,sizeof(_TAG_USINGPROP),MDM_GR_PROP,ASS_PROP_USEPROP,0);

					break;
				}
			}
		}
	}
	else
	{
		// 通知父窗口，使用第几个道具
		m_pParent->PostMessage(WM_USE_PROP, 0, nPropIndex);
	}

	this->ShowWindow(SW_HIDE);
}

void CPropPannel::OnBnClickedGive()
{
	GetDlgItem(IDC_GIVEUSER)->ShowWindow(SW_SHOW);
	UpdateData();

	if (m_sGiveUser == "")
	{
		AFCMessageBox("赠送昵称不能为空！","提示");
		return;
	}

	m_nGiveCount = (m_nGiveCount == 0) ? m_nBuyCount:m_nGiveCount;
	if(m_nGiveCount<1)
		return;

	if(_strcmpi(m_sGiveUser,m_pGameInfo->uisMeUserInfo.GameUserInfo.nickName)==0)
	{
		AFCMessageBox("没有必要将道具送给自己吧！","提示");
		return;
	}
	// 找到相应的项
	int nPropIndex=-1;
	int propCount=Glb().userPropLibrary.GetCount();
	_TAG_USERPROP * userProp=NULL;
	for(int i=0;i<propCount;i++)
	{
		userProp=Glb().userPropLibrary.GetAt(i);
		if(userProp->nPropID==curPropID)
		{
			nPropIndex = i;
			break;
		}
	}
	if (userProp->nHoldCount<m_nGiveCount && curPropID != VIPPROP)
	{
		AFCMessageBox("您没有这么多道具！","提示");
		CString str;
		str.Format("您确定要送将 %d 个该道具送给玩家 %s 吗？",m_nGiveCount,m_sGiveUser);

		if(IDOK != AFCMessageBox(str,"提示",AFC_YESNO))
		{
			return;
		}
	}
	if (curPropID != VIPPROP)
	{
		_TAG_PROP_GIVE *pPropGive = new _TAG_PROP_GIVE;
		ZeroMemory(pPropGive,sizeof(_TAG_PROP_GIVE));
		pPropGive->dwUserID				= m_pGameInfo->uisMeUserInfo.GameUserInfo.dwUserID;
		_tcscpy_s(pPropGive->szUser, 32, m_pGameInfo->uisMeUserInfo.GameUserInfo.nickName);
		_tcscpy_s(pPropGive->szTargetUser, 32, m_sGiveUser);
		pPropGive->nPropID				= curPropID;
		pPropGive->nGiveCount			= m_nGiveCount;
		pPropGive->iPropValue			= m_nGiveCount*curPropPrice;


		//// 扣除道具数量
		//userProp->nHoldCount -= m_nGiveCount;
		//if (userProp->nHoldCount<1)
		//{
		//	Glb().userPropLibrary.RemoveAt(nPropIndex);
		//	delete userProp;
		//	userProp=NULL;
		//}
		m_pParent->PostMessage(WM_GIVE_PROP, sizeof(_TAG_PROP_GIVE), (LPARAM)pPropGive);
		ShowWindow(SW_HIDE);
	}
	else
	{
		_TAG_PROP_BUY_VIP *pPropVIPBuy = new _TAG_PROP_BUY_VIP;
		CString stime;
		stime.Format("%d",CTime::GetCurrentTime());
		long curtime=atol(stime);
		int nPrice=curPropPrice;
		if(m_pGameInfo->uisMeUserInfo.GameUserInfo.iVipTime>curtime)
			nPrice=curPropVipPrice;

		pPropVIPBuy->dwUserID=m_pGameInfo->uisMeUserInfo.GameUserInfo.dwUserID;		//购买者ID
		pPropVIPBuy->nPropType = curVIPType;										//道具类型 ，需要取网页返回
		_tcscpy_s(pPropVIPBuy->szTargetUser, 32, m_sGiveUser);						//被赠送者ID

		this->m_pParent->PostMessage(WM_BUY_VIP, sizeof(_TAG_PROP_BUY_VIP), (LPARAM)pPropVIPBuy);
		UpdateData(FALSE);
		ShowWindow(SW_HIDE);
		return;
	}

}

void CPropPannel::OnGivenProp(_TAG_PROP_GIVE_FOR * propGiveFor)
{
	_TAG_PROP_GIVE * propGive=(_TAG_PROP_GIVE*)&propGiveFor->propGiveInfo;
	_TAG_USERPROP * userProp=(_TAG_USERPROP*)&propGiveFor->propInfo;
	
	if(userProp->nPropID==curPropID)
	{
		UpdateData();
		m_nHoldCount=userProp->nHoldCount;
		UpdateData(FALSE);
	}		
	////需要修改本地金币
	//CMainRoomEx *pWnd=(CMainRoomEx*)pParentWnd;
	//CString stip;
	//stip.Format("玩家 %s 送给您 %d 个 %s 道具，价值 %d 金币。",propGive->szUser,propGive->nGiveCount,userProp->szPropName,propGive->iPropValue);
	//for(int i = 0; i < MAX_GAME_ROOM; i ++)
	//{
	//	if(pWnd->m_RoomInfo[i].pGameRoomWnd != NULL)
	//	{
	//		pWnd->m_RoomInfo[i].pGameRoomWnd->PostMessage(WM_USER+151,0,0);
	//		pWnd->m_RoomInfo[i].pRoomInfoWnd->PostMessage(WM_USER+151,0,0);
	//		((CGameRoomEx*)pWnd->m_RoomInfo[i].pGameRoomWnd)->InsertSystemMessageWithGame(stip);//
	//	}
	//}
}

class CA
{
public:
	CA(){}
	~CA(){}
};

void CPropPannel::OnNetMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize)
{
	CBcfFile fMsg(CBcfFile::GetAppPath()+"..\\ClientMessage.bcf");
	CString strMessage;
	CA dlg;
	if (pNetHead->bAssistantID == ASS_PROP_RESULT)
	{
		switch(((MSG_PROP_RESULT *)pNetData)->dwResult)
		{
		case 0:
			{
				strMessage = fMsg.GetKeyVal("PropPanelDlg","PropResult0",
					"购买道具成功！");
				AFCMessageBox(strMessage,"提示");
			}
			break;
		case 1:
			{
				strMessage = fMsg.GetKeyVal("PropPanelDlg","PropResult1",
					"购买道具失败！");
				AFCMessageBox(strMessage,"提示");
			}
			break;
		case 2:
			{
				strMessage = fMsg.GetKeyVal("PropPanelDlg","PropResult2",
					"赠送道具成功！");
				AFCMessageBox(strMessage,"提示");
			}
			break;
		case 3:
			{
				strMessage = fMsg.GetKeyVal("PropPanelDlg","PropResult3",\
					"赠送道具失败，赠送玩家不存在！");
				AFCMessageBox(strMessage,"提示");
			}

			break;
		case 4:
			{
				strMessage = fMsg.GetKeyVal("PropPanelDlg","PropResult4",
					"赠送道具失败！");
				AFCMessageBox(strMessage,"提示");
			}
			break;
		case 5:
			{
				strMessage = fMsg.GetKeyVal("PropPanelDlg","PropResult5",
					"购买VIP失败！");
				AFCMessageBox(strMessage,"提示");
			}
			break;
		case 6:
			{
				strMessage = fMsg.GetKeyVal("PropPanelDlg","PropResult6",
					"购买VIP失败，您的钱不够！");
				AFCMessageBox(strMessage,"提示");
			}
			break;
		case 7:
			{
				strMessage = fMsg.GetKeyVal("PropPanelDlg","PropResult7",
					"购买VIP成功！");
				AFCMessageBox(strMessage,"提示");
			}
			break;
		case 8:
			{
				strMessage = fMsg.GetKeyVal("PropPanelDlg","PropResult8",
					"赠送VIP失败，赠送玩家不存在！");
				AFCMessageBox(strMessage,"提示");
			}
			break;
		case 9:
			{
				strMessage = fMsg.GetKeyVal("PropPanelDlg","PropResult9",
					"赠送VIP成功！");
				AFCMessageBox(strMessage,"提示");
			}
			break;
		case 10:
			{
				strMessage = fMsg.GetKeyVal("PropPanelDlg","PropResult10",
					"购买道具失败！您的钱不够！");
				AFCMessageBox(strMessage,"提示");
			}
		}
	}
	return;
	//从服务器返回了用户的道具
	if(pNetHead->bAssistantID==ASS_PROP_BUY && pNetHead->bHandleCode!=DTK_GP_PROP_BUY_SUCCEED)
	{
		AFCMessageBox("购买道具失败！您的游戏银行帐号中金币不足，请充值或存入足够的金币到银行后再购买！");
		return;
	}
	if(pNetHead->bAssistantID==ASS_PROP_GETUSERPROP || pNetHead->bAssistantID==ASS_PROP_BUY)
	{
		MSG_PROP_S_GETPROP * msg=(MSG_PROP_S_GETPROP*)pNetData;
		int propCount=Glb().userPropLibrary.GetCount();
		_TAG_USERPROP *userProp=NULL;
		bool bPropExist=false;
		for(int i=0;i<propCount;i++)
		{
			userProp=Glb().userPropLibrary.GetAt(i);
			if(userProp->nPropID==msg->nPropID)
			{
				bPropExist=true;
				break;
			}
		}
		if(!bPropExist)
		{
			userProp=new _TAG_USERPROP;
			memcpy(userProp,msg,sizeof(_TAG_USERPROP));
			Glb().userPropLibrary.Add(userProp);
		}
		else
			userProp->nHoldCount=msg->nHoldCount;
		if(pNetHead->bAssistantID==ASS_PROP_BUY)
			OnGetPropInformation(userProp);
	}
	//赠送道具
	if(pNetHead->bAssistantID==ASS_PROP_GIVE)
	{
		//有人赠送给自己
		if(pNetHead->bHandleCode==DTK_GP_PROP_GIVE_SUCCEED_BEGIVER)
		{
			_TAG_PROP_GIVE_FOR* propGiveFor=(_TAG_PROP_GIVE_FOR*)pNetData;
			_TAG_USERPROP * msg=(_TAG_USERPROP*)&propGiveFor->propInfo;
			int propCount=Glb().userPropLibrary.GetCount();
			_TAG_USERPROP *userProp=NULL;
			bool bPropExist=false;
			for(int i=0;i<propCount;i++)
			{
				userProp=Glb().userPropLibrary.GetAt(i);
				if(userProp->nPropID==msg->nPropID)
				{
					bPropExist=true;
					break;
				}
			}
			if(!bPropExist)
			{
				userProp=new _TAG_USERPROP;
				memcpy(userProp,msg,sizeof(_TAG_USERPROP));
				Glb().userPropLibrary.Add(userProp);
			}
			else
				userProp->nHoldCount=msg->nHoldCount;

			OnGivenProp(propGiveFor);

		}
		else if(pNetHead->bHandleCode==DTK_GP_PROP_GIVE_SUCCEED_GIVER)//赠送成功
		{
			_TAG_PROP_GIVE* propGive=(_TAG_PROP_GIVE*)pNetData;
			int propCount=Glb().userPropLibrary.GetCount();
			_TAG_USERPROP *userProp=NULL;
			for(int i=0;i<propCount;i++)
			{
				userProp=Glb().userPropLibrary.GetAt(i);
				if(userProp->nPropID==propGive->nPropID)
				{
					userProp->nHoldCount-=propGive->nGiveCount;
					//if(userProp->nHoldCount<1)    // PengJiLin, 2010-10-14, 新的道具系统数量可以为0，屏蔽
					//{
					//	Glb().userPropLibrary.RemoveAt(i);
					//	delete userProp;
					//	userProp=NULL;
					//}
					AFCMessageBox("成功赠送道具给其它玩家","提示");
					//CMainRoomEx *pWnd=(CMainRoomEx*)pParentWnd;

					//for(int i = 0; i < MAX_GAME_ROOM; i ++)
					//{
					//	if(pWnd->m_RoomInfo[i].pGameRoomWnd != NULL)
					//	{
					//		pWnd->m_RoomInfo[i].pGameRoomWnd->PostMessage(WM_USER+151,0,0);
					//		pWnd->m_RoomInfo[i].pRoomInfoWnd->PostMessage(WM_USER+151,0,0);
					//	}
					//}
					if(curPropID==propGive->nPropID)
					{
						
						if(userProp)
							m_nHoldCount=userProp->nHoldCount;
						else
						{
							m_nHoldCount=0;
							m_bnUse.EnableWindow(FALSE);
							m_ctlGiveUser.EnableWindow(FALSE);
						}
						UpdateData(FALSE);
					}
					break;
				}
			}
		}
		else//赠送失败
		{
			CString stip;
			if(pNetHead->bHandleCode==DTK_GP_PROP_GIVE_NOTARGETUSER)
				stip="对不起，您要赠送的玩家不存在";
			else
				stip="对不起，赠送道具失败！";
			AFCMessageBox(stip,"提示");
		}
	}
	if(pNetHead->bAssistantID==ASS_PROP_BIG_BOARDCASE)
	{//大喇叭
		_TAG_BOARDCAST *boardCast=(_TAG_BOARDCAST*)pNetData;
		//CMainRoomEx *pWnd=(CMainRoomEx*)pParentWnd;
		//if(pNetHead->bHandleCode!=DTK_GR_PROP_USE_SUCCEED && boardCast->dwUserID==pWnd->m_PlaceUserInfo.dwUserID)
		//{
		//	CMainRoomEx *pWnd=(CMainRoomEx*)pParentWnd;
		//	for(int i = 0; i < MAX_GAME_ROOM; i ++)
		//	{
		//		if(pWnd->m_RoomInfo[i].pGameRoomWnd != NULL)
		//		{
		//			((CGameRoomEx*)pWnd->m_RoomInfo[i].pGameRoomWnd)->InsertSystemMessageWithGame("对不起，打开道具箱出错");
		//		}
		//	}
		//	return;
		//}	
		if(boardCast->dwUserID==m_pGameInfo->uisMeUserInfo.GameUserInfo.dwUserID)
		{//减少道具数量
			int propCount=Glb().userPropLibrary.GetCount();
			_TAG_USERPROP *userProp=NULL;
			for(int i=0;i<propCount;i++)
			{
				userProp=Glb().userPropLibrary.GetAt(i);
				if(userProp->nPropID==boardCast->nPropID)
				{
					userProp->nHoldCount-=1;
                    if(userProp->nHoldCount < 0)userProp->nHoldCount = 0;
					//if(userProp->nHoldCount<1)    // PengJiLin, 2010-10-14, 新的道具系统数量可以为0，屏蔽
					//{
					//	Glb().userPropLibrary.RemoveAt(i);
					//	delete userProp;
					//	userProp=NULL;
					//}
					//for(int i = 0; i < MAX_GAME_ROOM; i ++)
					//{
					//	if(pWnd->m_RoomInfo[i].pGameRoomWnd != NULL)
					//	{
					//		pWnd->m_RoomInfo[i].pGameRoomWnd->PostMessage(WM_USER+151,0,0);
					//		pWnd->m_RoomInfo[i].pRoomInfoWnd->PostMessage(WM_USER+151,0,0);
					//	}
					//}
					if(curPropID==boardCast->nPropID)
					{
						if(userProp)
							m_nHoldCount=userProp->nHoldCount;
						else
							m_nHoldCount=0;
						UpdateData(FALSE);
					}
					//需要修改本地金币
					break;
				}
			}
		}
		////将大喇叭消息显示出来
		//for(int i = 0; i < MAX_GAME_ROOM; i ++)
		//{
		//	if(pWnd->m_RoomInfo[i].pGameRoomWnd != NULL)
		//		((CGameRoomEx*)pWnd->m_RoomInfo[i].pGameRoomWnd)->Dudu(boardCast,BRD_MSG_BIG);
		//}
	}
}

void CPropPannel::OnBnClickedClose2()
{
	this->ShowWindow(SW_HIDE);
}

void CPropPannel::FixDefaultCtl()
{
	/// 首先隐藏所有控件
	m_bnCancleUse.ShowWindow(SW_HIDE);
	m_bnCancleBuy.ShowWindow(SW_HIDE);
	m_bnCancleGive.ShowWindow(SW_HIDE);
	m_bnUse.ShowWindow(SW_HIDE);
	m_bnBuy.ShowWindow(SW_HIDE);
	m_bnGive.ShowWindow(SW_HIDE);

	m_szGiveName.ShowWindow(SW_HIDE);
	m_ctlGiveUser.ShowWindow(SW_HIDE);
	m_ctlUseUser.ShowWindow(SW_HIDE);
	m_szGiveCount.ShowWindow(SW_HIDE);
	m_szBuyCount.ShowWindow(SW_HIDE);
	m_ctlBuyCount.ShowWindow(SW_HIDE);
	m_ctlGiveCount.ShowWindow(SW_HIDE);
	m_bnAdd.ShowWindow(SW_HIDE);
	m_bnSub.ShowWindow(SW_HIDE);

	m_bnCancleGive.SetWindowText("赠送");
	m_bnCancleUse.SetWindowText("使用");
	m_UsingGive = false;
	m_UsingBuy = false;
	m_UsingUse = false;

	FixNormalBtnPos();
}

void CPropPannel::FixNormalBtnPos()
{
	/// 按初始化状态需求排放首屏控件
	m_ctlBuyCount.ShowWindow(SW_SHOW);
	m_szBuyCount.ShowWindow(SW_SHOW);
	m_bnAdd.ShowWindow(SW_SHOW);
	m_bnSub.ShowWindow(SW_SHOW);

	m_bnCancleUse.ShowWindow(SW_SHOW);
	m_bnCancleGive.ShowWindow(SW_SHOW);
	m_bnBuy.ShowWindow(SW_SHOW);

	m_bnCancleUse.MoveWindow(&m_BtnRect1);
	m_bnBuy.MoveWindow(&m_BtnRect2);
	m_bnCancleGive.MoveWindow(&m_BtnRect3);
	CRect rect;
	m_szGiveCount.GetWindowRect(&rect);
	ScreenToClient(&rect);
	m_szBuyCount.MoveWindow(&rect);
	m_ctlGiveCount.GetWindowRect(&rect);
	ScreenToClient(&rect);
	m_ctlBuyCount.MoveWindow(&rect);

	m_bnBuy.EnableWindow(TRUE);
	m_UsingBuy = true;
}

void CPropPannel::FixButtonOnGive()
{
	CRect rect;
	if (m_UsingGive)															//赠送昵称和赠送数量控件显示
	{
		try
		{
			m_szGiveName.ShowWindow(SW_SHOW);
			m_ctlGiveUser.ShowWindow(SW_SHOW);
			m_szGiveName.SetWindowText("赠送对象(昵称)：");
			m_szGiveCount.SetWindowText("赠送数量：");

			if(FALSE == m_bIsVIPButton)
			{
				m_ctlGiveCount.ShowWindow(SW_SHOW);
				m_szGiveCount.ShowWindow(SW_SHOW);
				m_bnSub.ShowWindow(SW_SHOW);
				m_bnAdd.ShowWindow(SW_SHOW);
			}

			m_bGibe = true;

			m_bnCancleGive.SetWindowText("取消赠送");

			m_bnBuy.ShowWindow(SW_HIDE);
			m_bnBuy.EnableWindow(FALSE);
			m_bnGive.ShowWindow(SW_SHOW);
			m_bnGive.EnableWindow(TRUE);

			m_bnCancleUse.ShowWindow(SW_HIDE);
			m_bnCancleBuy.ShowWindow(SW_HIDE);
			m_bnCancleGive.ShowWindow(SW_SHOW);
			m_bnGive.MoveWindow(&m_BtnRect2);
			m_bnCancleGive.MoveWindow(&m_BtnRect3);

			m_bnUse.ShowWindow(SW_HIDE);										//隐藏使用按钮

			m_ctlGiveUser.ResetContent();
			for(int i=0;i<MAX_PEOPLE;i++)
			{
				UserItemStruct *p = GetUserItem(i);
				if (NULL == p || NULL == m_pGameInfo) continue;
				if((0 == m_ctlGiveUser.GetCount() 
					&& 0 != strcmp(p->GameUserInfo.nickName, m_pGameInfo->uisMeUserInfo.GameUserInfo.nickName))
					|| (0 != strcmp(p->GameUserInfo.nickName, m_pGameInfo->uisMeUserInfo.GameUserInfo.nickName)
					&& (CB_ERR == m_ctlGiveUser.FindString(0, p->GameUserInfo.nickName))))
				{
					m_ctlGiveUser.AddString(p->GameUserInfo.nickName);
				}
			}

			if(m_bUsingNewUI && m_pLogic != NULL)
			{
				for(vector<MSG_IM_C_GETFRIENDLIST>::iterator it = m_pLogic->m_FriendList.begin();
					it != m_pLogic->m_FriendList.end();
					it++)
				{
					if(strcmp(it->sNickName, m_pGameInfo->uisMeUserInfo.GameUserInfo.nickName)
						&& (m_ctlGiveUser.FindString(0, it->sNickName) == CB_ERR))
					{
						m_ctlGiveUser.AddString(it->sNickName);
					}
				}
			}
			else
			{
				if (NULL == ((CLoveSendClassInExe *)GetParent()))
					return;

				for(vector<MSG_IM_C_GETFRIENDLIST>::iterator it = ((CLoveSendClassInExe *)GetParent())->m_FriendList.begin();
					it != ((CLoveSendClassInExe *)GetParent())->m_FriendList.end();
					it ++)
				{
					if(strcmp(it->sNickName, m_pGameInfo->uisMeUserInfo.GameUserInfo.nickName)
						&& (m_ctlGiveUser.FindString(0, it->sNickName) == CB_ERR))
					{
						m_ctlGiveUser.AddString(it->sNickName);
					}
				}
			}

			if(m_ctlGiveUser.GetCount() > 0)
			{
				m_ctlGiveUser.SetCurSel(0);
			}
		}
		catch (...)
		{
			DUIMessageBox(GetSafeHwnd(),MB_OK|MB_ICONINFORMATION,"提示","赠送道具异常");
			return;
		}
	}
	else
	{
		m_szGiveName.ShowWindow(SW_HIDE);
		m_ctlGiveUser.ShowWindow(SW_HIDE);
		m_bGibe = false;

		m_ctlGiveCount.ShowWindow(SW_HIDE);
		m_szGiveCount.ShowWindow(SW_HIDE);

		m_bnGive.ShowWindow(SW_HIDE);
		if (curPropID != VIPPROP)
		{
			m_bnGive.EnableWindow(FALSE);
		}

		m_bnCancleGive.SetWindowText("赠送");

		FixNormalBtnPos();
	}
}

void CPropPannel::OnBnClickedCancleGive()
{

	if (m_UsingUse)
	{
		m_UsingUse = !m_UsingUse;
		FixButtonOnUse();
	}
	if (m_UsingBuy)
	{
		m_UsingBuy = !m_UsingBuy;
		FixButtonOnBuy();
	}

	m_UsingGive = !m_UsingGive;
	FixButtonOnGive();
	GetDlgItem(IDC_GIVEUSER)->EnableWindow(TRUE);
	SetIEOnPropDlg();
	Invalidate();
	return;
}

void CPropPannel::FixButtonOnUse()
{
	if (m_UsingUse)
	{
		m_szGiveName.ShowWindow(SW_SHOW);
		m_ctlGiveUser.ShowWindow(SW_HIDE);
		m_ctlUseUser.ShowWindow(SW_SHOW);
		m_szGiveName.SetWindowText("使用对象(昵称)：");

		m_bnCancleUse.SetWindowText("取消使用");

		m_bnBuy.ShowWindow(SW_HIDE);
		m_bnBuy.EnableWindow(FALSE);
		m_bnUse.ShowWindow(SW_SHOW);

		m_bnCancleGive.ShowWindow(SW_HIDE);
		m_bnCancleBuy.ShowWindow(SW_HIDE);
		m_bnCancleUse.ShowWindow(SW_SHOW);
		m_bnUse.MoveWindow(&m_BtnRect2);
		m_bnCancleUse.MoveWindow(&m_BtnRect3);

		m_szBuyCount.ShowWindow(SW_HIDE);
		m_ctlBuyCount.ShowWindow(SW_HIDE);
		m_bnAdd.ShowWindow(SW_HIDE);
		m_bnSub.ShowWindow(SW_HIDE);

		m_ctlUseUser.ResetContent();
		//在此判断是否新版游戏
		/// 获取函数指针
		int nMax = GetMaxPeopleCount();
		for(int i=0;i<nMax;i++)
		{
			UserItemStruct *p = GetUserItem(i);
			int n = m_ctlUseUser.GetCount();
			if(p != NULL 
				&& strcmp(p->GameUserInfo.nickName, m_pGameInfo->uisMeUserInfo.GameUserInfo.nickName)
				&& (m_ctlUseUser.FindString(0, p->GameUserInfo.nickName) == CB_ERR))
			{
				m_ctlUseUser.InsertString(n, p->GameUserInfo.nickName);
			}
		}
	}
	else
	{
		m_szGiveName.ShowWindow(SW_HIDE);
		m_ctlUseUser.ShowWindow(SW_HIDE);

		m_bnUse.ShowWindow(SW_HIDE);

		m_bnCancleUse.SetWindowText("使用");

		m_bnCancleUse.MoveWindow(&m_BtnRect1);

		FixNormalBtnPos();
	}
}

void CPropPannel::OnBnClickedCancleUse()
{
	if (m_bUsingNewUI)
	{
		if (m_pLogic != NULL && m_pLogic->GetIsWatching())
		{
			DUIMessageBox(GetSafeHwnd(),MB_OK|MB_ICONINFORMATION,"温馨提示","对不起，旁观时不能使用道具！");
			return;
		}
	}
	else
	{
		CLoveSendClassInExe* pTemp = (CLoveSendClassInExe *)(GetParent());
		if ((pTemp != NULL) 
			&& (pTemp->isWatchingMode() == USER_WATCH_GAME))
		{
			DUIMessageBox(GetSafeHwnd(),MB_OK|MB_ICONINFORMATION,"温馨提示","对不起，旁观时不能使用道具！");
			return;
		}
	}

	if(curPropID == VIPPROP)
	{
		OnBnClickedUse();
		return;
	}

	if(IsPropUseTarget(curPropID))
	{
		m_UsingUse = !m_UsingUse;

		if (m_UsingGive)
		{
			m_UsingGive = !m_UsingGive;
			FixButtonOnGive();
		}
		if (m_UsingBuy)
		{
			m_UsingBuy = !m_UsingBuy;
			FixButtonOnBuy();
		}

		FixButtonOnUse();
		GetDlgItem(IDC_GIVEUSER)->EnableWindow(TRUE);
		SetIEOnPropDlg();
		Invalidate();
	}
	else
	{
		OnBnClickedUse();
	}

	return;
}

void CPropPannel::FixButtonOnBuy()
{
	CRect rect;
	if (m_UsingBuy)
	{
		m_szBuyCount.SetWindowText("购买数量：");
		m_szBuyCount.EnableWindow(TRUE);

		m_ctlBuyCount.ShowWindow(SW_SHOW);
		m_szBuyCount.ShowWindow(SW_SHOW);
		m_bnSub.ShowWindow(SW_SHOW);
		m_bnAdd.ShowWindow(SW_SHOW);

		m_bnCancleBuy.SetWindowText("取消购买");

		m_bnBuy.ShowWindow(SW_SHOW);
		m_bnBuy.EnableWindow(TRUE);

		GetDlgItem(IDC_STATIC_COUNT)->GetWindowRect(&rect);
		ScreenToClient(&rect);
		m_szBuyCount.MoveWindow(&rect);
		GetDlgItem(IDC_GIVECOUNT)->GetWindowRect(&rect);
		ScreenToClient(&rect);
		m_ctlBuyCount.MoveWindow(&rect);

	}
	else
	{
		m_szGiveName.ShowWindow(SW_HIDE);
		m_ctlUseUser.ShowWindow(SW_HIDE);

		m_ctlBuyCount.ShowWindow(SW_HIDE);
		m_szBuyCount.ShowWindow(SW_HIDE);
		m_bnSub.ShowWindow(SW_HIDE);
		m_bnAdd.ShowWindow(SW_HIDE);

		m_bnBuy.ShowWindow(SW_HIDE);

		m_bnCancleBuy.SetWindowText("购买");

		FixNormalBtnPos();
	}
}

void CPropPannel::OnBnClickedCancleBuy()
{
	if(curPropID != VIPPROP)
	{
		m_UsingBuy = !m_UsingBuy;

		if (m_UsingGive)
		{
			m_UsingGive = !m_UsingGive;
			FixButtonOnGive();
		}
		if (m_UsingUse)
		{
			m_UsingUse = !m_UsingUse;
			FixButtonOnUse();
		}

		FixButtonOnBuy();
		GetDlgItem(IDC_GIVEUSER)->EnableWindow(TRUE);
		SetIEOnPropDlg();
		Invalidate();
		m_ie.Refresh();
	}
	else
	{
		OnBnClickedBuy();
	}

	return;
}

void CPropPannel::OnBnClickedAddCount()
{
	if(m_UsingGive)
	{
		if (++m_nGiveCount <= m_nHoldCount)
		{
			CString num;
			num.Format("%d",m_nGiveCount);
			GetDlgItem(IDC_GIVECOUNT)->SetWindowText(num);
			return;
		}
		m_nGiveCount--;
	}
	else if(m_UsingBuy)
	{
		++m_nBuyCount;
		CString num;
		num.Format("%d",m_nBuyCount);
		GetDlgItem(IDC_BUYCOUNT)->SetWindowText(num);
	}
}

void CPropPannel::OnBnClickedSubCount()
{
	if(m_UsingGive)
	{
		if (m_nGiveCount)
			m_nGiveCount--;
		CString num;
		num.Format("%d",m_nGiveCount);
		GetDlgItem(IDC_GIVECOUNT)->SetWindowText(num);
	}
	else if(m_UsingBuy)
	{
		if (m_nBuyCount)
			m_nBuyCount--;
		CString num;
		num.Format("%d",m_nBuyCount);
		GetDlgItem(IDC_BUYCOUNT)->SetWindowText(num);
	}
}

BOOL CPropPannel::IsPropUseTarget(int iPropID)
{
	if(iPropID != VIPPROP)
	{
		_TAG_USERPROP * userProp=NULL;
		int nPropIndex=-1;
		int propCount=Glb().userPropLibrary.GetCount();
		for(int i=0;i<propCount;i++)
		{
			userProp=Glb().userPropLibrary.GetAt(i);
			if(userProp->nPropID==iPropID)
			{
				nPropIndex=i;
				break;
			}
		}
		if(nPropIndex != -1)
		{
			_TAG_USERPROP * userProp=Glb().userPropLibrary.GetAt(nPropIndex);
			return IsPropUseTarget(userProp);
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}


/*具体的规则不可考，此处参考“CLoveSendClassInExe::UsePropItem”*/
BOOL CPropPannel::IsPropUseTarget(_TAG_USERPROP * pProp)
{
	DebugPrintf("IsPropUseTarget() :: nPropID = %d, attribAction=%x", pProp->nPropID, pProp->attribAction);

	if(pProp->attribAction & 0x80 
		|| pProp->attribAction & 0x100
		|| pProp->attribAction & 0x200
		|| pProp->attribAction & 0x400
		|| pProp->attribAction & 0x800)
	{
		return FALSE;
	}
	else if(pProp->attribAction & 0x01)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

UserItemStruct * CPropPannel::GetUserItem(int iIdx)
{
	if(m_bUsingNewUI)
	{
		if(m_pLogic != NULL)
		{
			return m_pLogic->FindOnLineUserByIdx(iIdx);
		}
	}
	else
	{
		CLoveSendClassInExe *pParent = (CLoveSendClassInExe *)(GetParent());
		if(pParent != NULL)
		{
			return pParent->GetUserItemInfo(iIdx);
		}
	}
	return NULL;
}

int CPropPannel::GetMaxPeopleCount()
{
	if(m_bUsingNewUI)
	{
		if(m_pLogic != NULL)
		{
			return m_pLogic->GetOnlineUserCount();
		}
	}
	else
	{
		return MAX_PEOPLE;
	}
	return 0;
}

int CPropPannel::SendGameData(void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode)
{
	if(m_bUsingNewUI)
	{
		if(m_pLogic != NULL)
		{
			return m_pLogic->SendGameData(pData,uBufLen,bMainID,bAssistantID,bHandleCode);
		}
	}
	else
	{
		CLoveSendClassInExe *pParent = (CLoveSendClassInExe *)(GetParent());
		if(pParent != NULL)
		{
			return pParent->SendGameData(pData,uBufLen,bMainID,bAssistantID,bHandleCode);
		}
	}
	return 0;
}
//换肤
void CPropPannel::LoadSkin()
{
	CString s=CINIFile::GetAppPath ();/////本地路径
	CBcfFile f( s + "..\\" + m_skinmgr.GetSkinBcfFileName());
	CString key=TEXT("PropPannel");
	int r,g,b;
	r=f.GetKeyVal(key,"PropPannelR",215);
	g=f.GetKeyVal(key,"PropPannelG",238);
	b=f.GetKeyVal(key,"PropPannelB",253);
	m_bkcolor=RGB(r,g,b);

	setSkinIndex(0);

	TCHAR path[MAX_PATH];
	wsprintf(path,"..\\%slogon\\button.bmp",m_skinmgr.GetSkinPath());
	m_bnUse.LoadButtonBitmap(path,false);
	m_bnBuy.LoadButtonBitmap(path,false);
	m_bnGive.LoadButtonBitmap(path,false);
	m_bnClose2.LoadButtonBitmap(path,false);
	m_bnCancleGive.LoadButtonBitmap(path,false);
	m_bnCancleUse.LoadButtonBitmap(path,false);
	m_bnCancleBuy.LoadButtonBitmap(path,false);
	wsprintf(path,"..\\%slogon\\Left.bmp",m_skinmgr.GetSkinPath());
	m_bnAdd.LoadButtonBitmap(path,false);

	wsprintf(path,"..\\%slogon\\Right.bmp",m_skinmgr.GetSkinPath());
	m_bnSub.LoadButtonBitmap(path,false);

	key=TEXT("PropPannel");
	m_bkcolor_R = f.GetKeyVal(key,"PropPannelR",255);
	m_bkcolor_G = f.GetKeyVal(key,"PropPannelG",255);
	m_bkcolor_B = f.GetKeyVal(key,"PropPannelB",255);
	m_bkcolor=RGB(m_bkcolor_R,m_bkcolor_G,m_bkcolor_B);
}
//刷新道具窗口IE浏览器
void CPropPannel::SetIEOnPropDlg()
{
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f( s + "..\\" + m_skinmgr.GetSkinBcfFileName());
	CString key=TEXT("PropPannel");
	int x,y,w,h;
	x = f.GetKeyVal(key,"iex",4);
	y = f.GetKeyVal(key,"iey",31);
	w = f.GetKeyVal(key,"iew",390);
	h = f.GetKeyVal(key,"ieh",188);
	m_ie.MoveWindow(x, y, w, h, TRUE);
	m_ie.Navigate(m_surl,0,0,0,0);
}
//获取道具定时播放的时间
UINT CPropPannel::GetPropDisplayTime()
{
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f( s + "..\\" + "bzgame.bcf");
	CString key=TEXT("GameSet");
	return f.GetKeyVal(key,"DisplayPropTime",500);
}
HBRUSH CPropPannel::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);	
	// TODO:  在此更改 DC 的任何特性
	if (nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)GetStockObject(HOLLOW_BRUSH);
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}