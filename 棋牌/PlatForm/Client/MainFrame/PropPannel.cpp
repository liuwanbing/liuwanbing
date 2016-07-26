// PropPannel.cpp : 实现文件
//

#include "stdafx.h"
#include "PropPannel.h"

#include "MainRoomEx.h"
#include "GameRoomEx.h"
#include "comutil.h"

#include "../UILibrary2003/ConfirmDlg.h"
// CPropPannel 对话框

//#define  BUTTON_BMP  "image\\client\\skin0\\logon\\button.bmp"

// 用来输出调试信息
extern void DebugPrintf(const char *p, ...);


IMPLEMENT_DYNAMIC(CPropPannel, CDialog)

CPropPannel::CPropPannel(CWnd* pParent /*=NULL*/)
	: CDialog(CPropPannel::IDD, pParent)
	, m_nHoldCount(0)
	, m_nBuyCount(0)
	, m_strBuyMoney(_T(""))
	, m_sGiveUser(_T(""))
	, m_nGiveCount(0)
{
	pParentWnd=NULL;
	imgDialog=0;
	

	CString s=CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f( s + "skin0.bcf");
	CString key=TEXT("PropPannel");
	int r,g,b;
	m_bkcolor_R = f.GetKeyVal(key,"PropPannelR",44);
	m_bkcolor_G = f.GetKeyVal(key,"PropPannelG",114);
	m_bkcolor_B = f.GetKeyVal(key,"PropPannelB",233);
	m_bkcolor=RGB(m_bkcolor_R,m_bkcolor_G,m_bkcolor_B);

	CBcfFile fini(s+"bzgame.bcf");
	m_strPropUrl=fini.GetKeyVal("BZW","PropPannelUrl","http://game.bzw.cn/app/Prop/PropInfo.asp");
	m_nOpenFrom=0;

	curPropID = 0;
	curPropPrice = 0;
	curPropVipPrice = 0;

	m_bkBrush=NULL;
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
	DDX_Control(pDX, IDC_USE, m_bnUse);
	DDX_Control(pDX, IDC_BUY, m_bnBuy);
	DDX_Control(pDX, IDC_GIVE, m_bnGive);
	//DDX_Control(pDX, IDCANCEL, m_cancelBtn);
	DDX_Control(pDX, IDC_BUYCOUNT, m_ctlBuyCount);
	DDX_Text(pDX, IDC_GIVEUSER, m_sGiveUser);
	DDV_MaxChars(pDX, m_sGiveUser, 30);
	DDX_Control(pDX, IDC_GIVECOUNT, m_ctlGiveCount);
	DDX_Text(pDX, IDC_GIVECOUNT, m_nGiveCount);
}


BEGIN_MESSAGE_MAP(CPropPannel, CDialog)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_CLOSE, &CPropPannel::OnBnClickedClose)
	ON_WM_LBUTTONDOWN()
	ON_EN_CHANGE(IDC_BUYCOUNT, &CPropPannel::OnEnChangeBuycount)
	ON_EN_CHANGE(IDC_GIVEUSER, &CPropPannel::OnEnChangeGiveuser)
	ON_EN_CHANGE(IDC_GIVECOUNT, &CPropPannel::OnEnChangeGivecount)
	ON_BN_CLICKED(IDC_BUY, &CPropPannel::OnBnClickedBuy)
	ON_BN_CLICKED(IDC_USE, &CPropPannel::OnBnClickedUse)
	ON_BN_CLICKED(IDC_GIVE, &CPropPannel::OnBnClickedGive)
END_MESSAGE_MAP()


// CPropPannel 消息处理程序

BOOL CPropPannel::OnInitDialog()
{
	CDialog::OnInitDialog();

	setSkinIndex(0);

	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
 //	SetWindowText(fMsg.GetKeyVal("PropPanelDlg","Panel","道具面板"));

	TCHAR szButton[MAX_PATH];
	sprintf(szButton, "%s\\%slogon\\button.bmp",CBcfFile::GetAppPath(true),m_skinmgr.GetSkinPath());
	m_bnUse.LoadButtonBitmap(szButton,false);
	m_bnBuy.LoadButtonBitmap(szButton,false);
	m_bnGive.LoadButtonBitmap(szButton,false);
	//m_cancelBtn.LoadButtonBitmap(szButton,false);

// 	SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOSIZE);   //对话框置顶  yjj 090309

	SetDlgItemText(IDC_STATIC_OWN,fMsg.GetKeyVal("PropPanelDlg","OwnNo","拥有数量："));
	SetDlgItemText(IDC_STATIC_BUY,fMsg.GetKeyVal("PropPanelDlg","BuyNo","购买数量："));
	SetDlgItemText(IDC_STATIC_GIF,fMsg.GetKeyVal("PropPanelDlg","Gift","赠送对象(昵称)："));
	SetDlgItemText(IDC_STATIC_NO,fMsg.GetKeyVal("PropPanelDlg","Number","数量："));
	//SetDlgItemText(IDC_STATIC_NOTE,fMsg.GetKeyVal("PropPanelDlg","Note","赠送道具需要先购买后才能赠送，对象是您要赠送的玩家的昵称。"));
	m_bnUse.SetWindowText(fMsg.GetKeyVal("PropPanelDlg","BTNUse","使用"));
	m_bnBuy.SetWindowText(fMsg.GetKeyVal("PropPanelDlg","BTNBuy","购买"));
	m_bnGive.SetWindowText(fMsg.GetKeyVal("PropPanelDlg","BTNGive","赠送"));
	//m_cancelBtn.SetWindowText(fMsg.GetKeyVal("PropPanelDlg","BTNCancel","关闭窗口"));

	CRect rc;
	GetWindowRect(&rc);
	m_bnClose.MoveWindow(rc.Width()-36,0,36,20);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CPropPannel::setSkinIndex(int sk)
{
	try
	{
		
		CString s;
		s.Format("%s\\%sdialog\\PropPanel_bk_mr.bmp",CBcfFile::GetAppPath(true),m_skinmgr.GetSkinPath());

		//CString s;
		//s.Format("%s\\%sdialog\\PropPanel_bk_mr.bmp",CBcfFile::GetAppPath(true),m_skinmgr.GetSkinPath());
		////	s.Format("%s\\image\\skin%d\\dialog\\dialog.bmp",CBcfFile::GetAppPath(true),m_skinIndex);
		//CGameImage bk;
		//CRect cRect;
		//GetWindowRect(&cRect);
		//bk.SetLoadInfo(s.GetBuffer(s.GetLength()),CGameImageLink::m_bAutoLock);
		//MoveWindow(cRect.left, cRect.top, bk.GetWidth(), bk.GetHeight(), TRUE);
		
		//*-------初始化 m_configDlg--------- 2012.10.17 yyf
		TCHAR pathConfig[MAX_PATH];
		TCHAR PathDefault[MAX_PATH];
	
		wsprintf(pathConfig,"%s%sdialog\\configDlg\\CPropPannelConfig\\configDlg.bcf",CBcfFile::GetAppPath(true),m_skinmgr.GetSkinPath());// 
		wsprintf(PathDefault,"%s%s",CBcfFile::GetAppPath(true),m_skinmgr.GetSkinPath());
		m_configDlg.LoadConfig(pathConfig,PathDefault);	

		int cx,cy; 
		cx=m_configDlg.GetDlgWide();
		cy=m_configDlg.GetDlgHigh(); 
	    SetWindowPos(NULL,0,0,cx,cy,SWP_NOMOVE|SWP_NOZORDER);
		//-----------------------------------------------------*/
#ifndef BZ_ZLIB
		imgDialog=Image::FromFile(s.AllocSysString());
#else
		CUnZipRes unZipRes;
		IStream * pStream=NULL;
		TCHAR * pchar=s.GetBuffer(s.GetLength()+1);
		pStream=unZipRes.LoadFileResFromZip(pchar);
		imgDialog=new Image(pStream);
#endif
		

		TCHAR path[MAX_PATH];

		wsprintf(path,"%s\\%sdialog\\log_exit_bt.bmp",CBcfFile::GetAppPath(true),m_skinmgr.GetSkinPath());//Rect_Close.bmp
		m_bnClose.LoadButtonBitmap(path,false);
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"PROP_PANNEL_TRY_CATCH : [%s]",__FUNCTION__);
		
	}
	// TODO:  在此添加额外的初始化


}
void CPropPannel::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	//绘画框架底图 2012.10.17 yyf
	m_configDlg.DrawViewFrame(&dc);

	drawSkin();
	/*if(!m_ie.get_Busy())
		m_ie.Refresh();*/
	__super::OnPaint();
}

void CPropPannel::drawSkin(void)
{
	try
	{
//// 2012.12.17 yyf 屏蔽
//		if(!imgDialog)
//			return;
//		CDC *pdc=this->GetWindowDC();
//		if(!pdc)return;
//		CRect rc;
//		this->GetWindowRect(&rc);
//
//		Graphics g(pdc->m_hDC);
//
//		g.DrawImage(imgDialog,Rect(0,0,imgDialog->GetWidth(),imgDialog->GetHeight()),0,0,imgDialog->GetWidth(),imgDialog->GetHeight(),UnitPixel);
//
//// 		CString s="宋体";
//// 
//// 		Font font(s.AllocSysString(),12,0,UnitPixel);
//// 		SolidBrush brush(Color(255,255,255));
//// 		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
//// 
//// 		s = fMsg.GetKeyVal("PropPanelDlg","Panel","道具面板");
//// 		g.DrawString(s.AllocSysString(),-1,&font,PointF(5,5),0,&brush);
//		g.ReleaseHDC(pdc->m_hDC);

		m_bnClose.Invalidate(FALSE);

	    m_ctlTip.GetDC()->SetTextColor(RGB(255,0,0));   //yjj 修改提示字体颜色
		processLocalProp();
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
		{
			static COLORREF s_bkcolor = m_bkcolor;
			SetTextColor((HDC)wParam, RGB(0,0,0)) ;
			SetBkColor((HDC)wParam, m_bkcolor);
			if( m_bkBrush == NULL )
			{
				m_bkBrush = CreateSolidBrush(m_bkcolor);
			}
			if (m_bkcolor != s_bkcolor)
			{
				s_bkcolor = m_bkcolor;
				DeleteObject(m_bkBrush); m_bkBrush = NULL;
				m_bkBrush = CreateSolidBrush(m_bkcolor);
			}
			return (LRESULT)m_bkBrush;
		}
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

void CPropPannel::openPannel(int openFrom,CString strGiveUser, int propID)
{
	CMainRoomEx *pWnd=(CMainRoomEx*)pParentWnd;
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	CString strMessage;

	if(!pParentWnd)
	{
		for(int i = 0; i < MAX_GAME_ROOM; i ++)
		{
			if(pWnd->m_RoomInfo[i].pGameRoomWnd != NULL)
			{
				strMessage = fMsg.GetKeyVal("PropPanelDlg","ErrorOpenToolBox","对不起，打开道具箱出错");
				((CGameRoomEx*)pWnd->m_RoomInfo[i].pGameRoomWnd)->InsertSystemMessageWithGame(strMessage.GetBuffer());
			}
		}
		return ;
	}
	if(openFrom!=-1)
		m_nOpenFrom=openFrom;
	CString stip;
	TCHAR szNum[128]; 
    GetNumString(szNum, pWnd->m_PlaceUserInfo.i64Bank, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
	
	strMessage = fMsg.GetKeyVal("PropPanelDlg","BuyToolsWithCoin","购买道具使用的是您银行中的金币，您现在银行中有 %s 金币。");
	
	stip.Format(strMessage, szNum);
	m_ctlTip.SetWindowText(stip);
	m_ctlGiveUser.SetWindowText(strGiveUser);
	CenterWindow();
	this->ShowWindow(SW_SHOW);
	CString surl;

	//surl.Format("%s?userid=%d&bgcolor=%X%X%X",m_strPropUrl,pWnd->m_PlaceUserInfo.dwUserID,m_bkcolor&0xFF,(m_bkcolor&0xFF00)>>8,m_bkcolor>>16);
	surl.Format("%s?propid=%d&bgcolor=%X%X%X",m_strPropUrl,propID,m_bkcolor_R,m_bkcolor_G,m_bkcolor_B);
	m_ie.Navigate(surl,0,0,0,0);

	m_ctlBuyCount.SetFocus();

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
			processLocalProp();
		}
	}
}

void CPropPannel::NavigateErrorExplorer1(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Frame, VARIANT* StatusCode, BOOL* Cancel)
{
	this->ShowWindow(SW_HIDE);
	CMainRoomEx *pWnd=(CMainRoomEx*)pParentWnd;
	for(int i = 0; i < MAX_GAME_ROOM; i ++)
	{
		if(pWnd->m_RoomInfo[i].pGameRoomWnd != NULL)
		{
			CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
			CString strMessage = fMsg.GetKeyVal("PropPanelDlg","ErrorOpenToolBox","对不起，打开道具箱出错");
			((CGameRoomEx*)pWnd->m_RoomInfo[i].pGameRoomWnd)->InsertSystemMessageWithGame(strMessage.GetBuffer());
		}
	}
}

void CPropPannel::processLocalProp()
{
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
	CMainRoomEx *pWnd=(CMainRoomEx*)pParentWnd;
	CString stime;
	stime.Format("%d",CTime::GetCurrentTime());
	long curtime=atol(stime);
	int nPrice=curPropPrice;
	if(pWnd->m_PlaceUserInfo.iVipTime>curtime)
		nPrice=curPropVipPrice;
	m_bnUse.EnableWindow(FALSE);
	m_bnGive.EnableWindow(FALSE);
	m_ctlGiveUser.EnableWindow(FALSE);
	m_ctlGiveCount.EnableWindow(FALSE);
	if(bFoundProp)
	{
		m_nHoldCount=userProp->nHoldCount;
		if(m_nHoldCount>0)
		{
			m_bnUse.EnableWindow(TRUE);
			m_ctlGiveUser.EnableWindow(TRUE);
			CString rString;
			m_ctlGiveUser.GetWindowText(rString);
			if(rString.Trim()!="")
			{
				m_bnGive.EnableWindow(TRUE);
				m_ctlGiveCount.EnableWindow(TRUE);
			}
		}
	}
	else
		m_nHoldCount=0;
	m_nBuyCount=1;
	TCHAR szNum[128]; 
    GetNumString(szNum, nPrice*m_nBuyCount, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	CString strMessage = fMsg.GetKeyVal("PropPanelDlg","NeedCoins","(您是%s,需要 %s 金币)");
	m_strBuyMoney.Format(strMessage,(pWnd->m_PlaceUserInfo.iVipTime>curtime)?
	fMsg.GetKeyVal("PropPanelDlg","VIPMember","VIP会员"):
	fMsg.GetKeyVal("PropPanelDlg","NormalUser","普通用户"),szNum);
	UpdateData(FALSE);
	m_ctlGiveCount.SetWindowText("1");
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
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	CString strMessage;

	if(m_nBuyCount>255)
		m_nBuyCount=255;
	//wushuqun 2009.9.3
	//不允许购买负道具
	if (m_nBuyCount < 0)
	{
		strMessage = fMsg.GetKeyVal("PropPanelDlg","ErrorToolNumber","您好，您的输入道具数量不正确，请重新输入!");
		
		//AFCMessageBox(strMessage);
        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",strMessage);
		return;
	}
	CMainRoomEx *pWnd=(CMainRoomEx*)pParentWnd;
	CString stime;
	stime.Format("%d",CTime::GetCurrentTime());
	long curtime=atol(stime);
	int nPrice=curPropPrice;
	if(pWnd->m_PlaceUserInfo.iVipTime>curtime)
		nPrice=curPropVipPrice;
	TCHAR szNum[128]; 
    GetNumString(szNum, nPrice*m_nBuyCount, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
//	m_strBuyMoney.Format("(您是%s,需要 %s 金币)",(pWnd->m_PlaceUserInfo.iVipTime>curtime)?"VIP会员":"普通用户",szNum);
	strMessage = fMsg.GetKeyVal("PropPanelDlg","NeedCoins","(您是%s,需要 %s 金币)");
	m_strBuyMoney.Format(strMessage,(pWnd->m_PlaceUserInfo.iVipTime>curtime)?
		fMsg.GetKeyVal("PropPanelDlg","VIPMember","VIP会员"):
	fMsg.GetKeyVal("PropPanelDlg","NormalUser","普通用户"),szNum);
	UpdateData(FALSE);
	
	if(nPrice*m_nBuyCount>pWnd->m_PlaceUserInfo.i64Bank)
		m_bnBuy.EnableWindow(FALSE);
	else
		m_bnBuy.EnableWindow(TRUE);
}

void CPropPannel::OnEnChangeGiveuser()
{
	CString sGiveUser;
	m_ctlGiveUser.GetWindowText(sGiveUser);
	sGiveUser.Trim();
	if(sGiveUser=="")
	{
		m_ctlGiveCount.EnableWindow(FALSE);
		m_bnGive.EnableWindow(FALSE);
		UpdateData();
	}	
	else
	{
		m_ctlGiveCount.EnableWindow(TRUE);
		m_bnGive.EnableWindow(TRUE);
		OnEnChangeGivecount();
	}
}

void CPropPannel::OnEnChangeGivecount()
{
	CString rString;
	m_ctlGiveCount.GetWindowText(rString);
	if(rString.Trim()=="")
		m_ctlGiveCount.SetWindowText("0");
	UpdateData();
	if(m_nGiveCount>m_nHoldCount)
		m_nGiveCount=m_nHoldCount;
	CString sGiveUser;
	m_ctlGiveUser.GetWindowText(sGiveUser);
	sGiveUser.Trim();
	if(m_nHoldCount>0 && sGiveUser!="")
		m_bnGive.EnableWindow(TRUE);
	else
		m_bnGive.EnableWindow(FALSE);

	UpdateData(FALSE);
}

void CPropPannel::OnBnClickedBuy()
{
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	CString strMessage;
	//ConfirmDlg confirmDlg("您确定要购买吗？","提示",AFC_YESNO,this);
	//if (confirmDlg.DoModal() == IDQUXIAO)
	//	return;

	/*if(AFCMessageBox("您确定要购买吗？","提示",AFC_YESNO) == IDQUXIAO)
		return;*/
	CMainRoomEx *pWnd=(CMainRoomEx*)pParentWnd;
	_TAG_PROP_BUY propBuy;
	propBuy.dwUserID=pWnd->m_PlaceUserInfo.dwUserID;
	propBuy.nPropID=curPropID;

	CString stime;
	stime.Format("%d",CTime::GetCurrentTime());
	long curtime=atol(stime);
	int nPrice=curPropPrice;
	if(pWnd->m_PlaceUserInfo.iVipTime>curtime)
		nPrice=curPropVipPrice;
	if (m_nBuyCount < 0)
	{
		strMessage = fMsg.GetKeyVal("PropPanelDlg","ErrorToolNumber","您好，您的输入道具数量不正确，请重新输入!");
		
		//AFCMessageBox(strMessage);
        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",strMessage);
		return;
	}

	propBuy.iPropPayMoney=nPrice*m_nBuyCount;
	propBuy.nPropBuyCount=m_nBuyCount;
	pWnd->SendData(&propBuy,sizeof(_TAG_PROP_BUY),MDM_GP_PROP,ASS_PROP_BUY,0);
}

void CPropPannel::OnGetPropInformation(_TAG_USERPROP* userProp)
{
	// 此处是从服务器传来的消息，与本地的数据有可能不一致

	if(userProp->nPropID==curPropID)
	{
		UpdateData();
		m_nHoldCount=userProp->nHoldCount;
		UpdateData(FALSE);
	}

	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	//需要修改本地金币
	CMainRoomEx *pWnd=(CMainRoomEx*)pParentWnd;
	CString stip;
	pWnd->m_PlaceUserInfo.i64Bank-=userProp->dwCost;//由ZXD修改
	TCHAR szNum[128];
	GetNumString(szNum, pWnd->m_PlaceUserInfo.i64Bank, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
	CString strMessage = fMsg.GetKeyVal("PropPanelDlg","BuyToolsWithCoin","购买道具使用的是您银行中的金币，您现在银行中有 %s 金币。");
	stip.Format(strMessage, szNum);
	//stip.Format("购买道具使用的是您银行中的金币，您现在银行中有 %d 金币。",pWnd->m_PlaceUserInfo.dwBank);
	m_ctlTip.SetWindowText(stip);
	for(int i = 0; i < MAX_GAME_ROOM; i ++)
	{
		if(pWnd->m_RoomInfo[i].pGameRoomWnd != NULL)
		{
			pWnd->m_RoomInfo[i].pGameRoomWnd->PostMessage(WM_USER+151,userProp->dwCost,0);
			pWnd->m_RoomInfo[i].pRoomInfoWnd->PostMessage(WM_USER+151,userProp->dwCost,0);
//			((CGameRoomEx*)pWnd->m_RoomInfo[i].pGameRoomWnd)->InsertSystemMessageWithGame(stip);//
		}
	}
	if(m_nHoldCount>0)
	{
		m_bnUse.EnableWindow(TRUE);
		m_ctlGiveUser.EnableWindow(TRUE);
		CString rString;
		m_ctlGiveUser.GetWindowText(rString);
		if(rString.Trim()!="")
		{
			m_bnGive.EnableWindow(TRUE);
			m_ctlGiveCount.EnableWindow(TRUE);
		}
	}
}

void CPropPannel::OnBnClickedUse()
{
	int nPropIndex=-1;
	int propCount=Glb().userPropLibrary.GetCount();
	_TAG_USERPROP * userProp=NULL;
	for(int i=0;i<propCount;i++)
	{
		userProp=Glb().userPropLibrary.GetAt(i);
		if(userProp->nPropID==curPropID)
		{
			nPropIndex=i;
			break;
		}
	}
	CMainRoomEx *pWnd=(CMainRoomEx*)pParentWnd;
	if(userProp->attribAction & 0x80 || userProp->attribAction &0x100)
	{//大喇叭和小喇叭
		for(int i = 0; i < MAX_GAME_ROOM; i ++)
			if(pWnd->m_RoomInfo[i].pGameRoomWnd != NULL)
			{
				//pWnd->m_RoomInfo[i].pGameRoomWnd->PostMessage(WM_USER+156,nPropIndex,0);
				pWnd->m_RoomInfo[i].pGameRoomWnd->PostMessage(WM_USER+156,(WPARAM)userProp,0);
				this->ShowWindow(SW_HIDE);
			}
		return;
	}
	if(userProp->attribAction&0x1 && m_nOpenFrom==1)
	{

		//AFCMessageBox("对不起，该道具只能在游戏中使用！","提示");
		CMainRoomEx *pWnd=(CMainRoomEx*)pParentWnd;
		for(int i = 0; i < MAX_GAME_ROOM; i ++)
		{
			if(pWnd->m_RoomInfo[i].pGameRoomWnd != NULL)
			{
				CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
				CString strMessage = fMsg.GetKeyVal("PropPanelDlg","BeUsedOnlyInGame","对不起，该道具只能在游戏中使用！");
				((CGameRoomEx*)pWnd->m_RoomInfo[i].pGameRoomWnd)->InsertSystemMessageWithGame(strMessage.GetBuffer());//
			}
		}
		return;
	}
	if(nPropIndex==-1)
		return;
	for(int i = 0; i < MAX_GAME_ROOM; i ++)
		if(pWnd->m_RoomInfo[i].pGameRoomWnd != NULL)
		{
			if(m_nOpenFrom==0)
				pWnd->m_RoomInfo[i].pGameRoomWnd->PostMessage(WM_USER+152,nPropIndex,0);
			else
				pWnd->m_RoomInfo[i].pRoomInfoWnd->PostMessage(WM_USER+152,nPropIndex,0);
			this->ShowWindow(SW_HIDE);
		}

}

void CPropPannel::OnBnClickedGive()
{
	UpdateData();
	if(m_nGiveCount<1)
		return;
	CMainRoomEx *pWnd=(CMainRoomEx*)pParentWnd;
	//if(_strcmpi(m_sGiveUser,pWnd->m_PlaceUserInfo.nickName)==0)
	//{
	//	AFCMessageBox("没有必要将道具送给自己吧！","提示");
	//	CMainRoomEx *pWnd=(CMainRoomEx*)pParentWnd;
	//	for(int i = 0; i < MAX_GAME_ROOM; i ++)
	//	{
	//		if(pWnd->m_RoomInfo[i].pGameRoomWnd != NULL)
	//		{
	//			CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	//			CString strMessage = fMsg.GetKeyVal("PropPanelDlg","NotSendToYourself","没有必要将道具送给自己！");
	//			((CGameRoomEx*)pWnd->m_RoomInfo[i].pGameRoomWnd)->InsertSystemMessageWithGame(strMessage.GetBuffer());//
	//		}
	//	}
	//	return;
	//}
	if(_strcmpi(m_sGiveUser,pWnd->m_PlaceUserInfo.nickName)==0)
	{
		//AFCMessageBox("没有必要将道具送给自己吧！","提示");
        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","没有必要将道具送给自己吧！");
		return;
	}
	CString str;
	str.Format("您确定要送将 %d 个该道具送给玩家 %s 吗？",m_nGiveCount,m_sGiveUser);
	//if(AFCMessageBox(str,"提示",AFC_YESNO)==IDQUXIAO)
	if(DUIMessageBox(m_hWnd,MB_YESNO|MB_ICONINFORMATION,"提示",str)==IDQUXIAO)
		return;
	_TAG_PROP_GIVE propGive;
	ZeroMemory(&propGive,sizeof(_TAG_PROP_GIVE));
	propGive.dwUserID=pWnd->m_PlaceUserInfo.dwUserID;
	strcpy(propGive.szUser,pWnd->m_PlaceUserInfo.nickName);
	strcpy(propGive.szTargetUser,m_sGiveUser);
	propGive.nPropID=curPropID;
	propGive.nGiveCount=m_nGiveCount;
	propGive.iPropValue=m_nGiveCount*curPropPrice;


	pWnd->SendData(&propGive,sizeof(_TAG_PROP_GIVE),MDM_GP_PROP,ASS_PROP_GIVE,0);
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
	//需要修改本地金币
	CString stip;
	TCHAR szNum[128]; 
    GetNumString(szNum, propGive->iPropValue, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	CString strMessage = fMsg.GetKeyVal("PropPanelDlg","PlayerGiveTool","玩家 %s 送给您 %d 个 %s 道具，价值 %s 金币。");
	stip.Format(strMessage,propGive->szUser,propGive->nGiveCount,userProp->szPropName,szNum);
	CMainRoomEx *pWnd=(CMainRoomEx*)pParentWnd;
	for(int i = 0; i < MAX_GAME_ROOM; i ++)
	{
		if(pWnd->m_RoomInfo[i].pGameRoomWnd != NULL)
		{
			pWnd->m_RoomInfo[i].pGameRoomWnd->PostMessage(WM_USER+151,0,0);
			pWnd->m_RoomInfo[i].pRoomInfoWnd->PostMessage(WM_USER+151,0,0);
			((CGameRoomEx*)pWnd->m_RoomInfo[i].pGameRoomWnd)->InsertSystemMessageWithGame(stip);//
		}
	}
}

void CPropPannel::OnNetMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize)
{
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	CString strMessage;
	//从服务器返回购买/赠送VIP消息
	
	if( pNetHead->bAssistantID == ASS_PROP_BUY_VIP )
	{
		switch (pNetHead->bHandleCode)
		{
		case DTK_GR_VIP_GIVE_SUCCESS:
			{
				MSG_PROP_S_BUYVIP *propVIP = (MSG_PROP_S_BUYVIP *)pNetData;
				CMainRoomEx *pWnd=(CMainRoomEx*)pParentWnd;
				for(int i = 0; i < MAX_GAME_ROOM; i ++)
				{
					if(pWnd->m_RoomInfo[i].pGameRoomWnd != NULL)
					{
						CString stip;
						strMessage = fMsg.GetKeyVal("PropPanelDlg","GIVEVIPSUC",
							"成功赠送VIP给玩家[%s]");
						stip.Format(strMessage, propVIP->szTargetUser);
						((CGameRoomEx*)pWnd->m_RoomInfo[i].pGameRoomWnd)->InsertSystemMessageWithGame(stip.GetBuffer());
					}
				}
			}
			break;
		case DTK_GP_VIP_BUY_NOMONEY:
			{
				MSG_PROP_S_BUYVIP *propVIP = (MSG_PROP_S_BUYVIP *)pNetData;
				CMainRoomEx *pWnd=(CMainRoomEx*)pParentWnd;
				for(int i = 0; i < MAX_GAME_ROOM; i ++)
				{
					if(pWnd->m_RoomInfo[i].pGameRoomWnd != NULL)
					{
						CString stip;
						strMessage = fMsg.GetKeyVal("PropPanelDlg","GIVEVIPNOMONEY",
							"赠送VIP失败，您的金币不够！");
						((CGameRoomEx*)pWnd->m_RoomInfo[i].pGameRoomWnd)->InsertSystemMessageWithGame(strMessage.GetBuffer());
					}
				}
			}

			break;
		case DTK_GR_VIP_BUY_NOUSER:
			{
				MSG_PROP_S_BUYVIP *propVIP = (MSG_PROP_S_BUYVIP *)pNetData;
				CMainRoomEx *pWnd=(CMainRoomEx*)pParentWnd;
				for(int i = 0; i < MAX_GAME_ROOM; i ++)
				{
					if(pWnd->m_RoomInfo[i].pGameRoomWnd != NULL)
					{
						CString stip;
						strMessage = fMsg.GetKeyVal("PropPanelDlg","GIVEVIPSUCNOUSER",
							"赠送VIP失败，赠送的玩家不存在！");
						((CGameRoomEx*)pWnd->m_RoomInfo[i].pGameRoomWnd)->InsertSystemMessageWithGame(strMessage.GetBuffer());
					}
				}
			}
			break;
		case DTK_GP_VIP_BUY_ERROR:
			{
				MSG_PROP_S_BUYVIP *propVIP = (MSG_PROP_S_BUYVIP *)pNetData;
				CMainRoomEx *pWnd=(CMainRoomEx*)pParentWnd;
				for(int i = 0; i < MAX_GAME_ROOM; i ++)
				{
					if(pWnd->m_RoomInfo[i].pGameRoomWnd != NULL)
					{
						CString stip;
						strMessage = fMsg.GetKeyVal("PropPanelDlg","GIVEERR",
							"赠送VIP失败");
						((CGameRoomEx*)pWnd->m_RoomInfo[i].pGameRoomWnd)->InsertSystemMessageWithGame(strMessage.GetBuffer());
					}
				}
			}
			break;
		case DTK_GR_VIP_BUY_SUCCEED:
			{
				MSG_PROP_S_BUYVIP *propVIP = (MSG_PROP_S_BUYVIP *)pNetData;
				CMainRoomEx *pWnd=(CMainRoomEx*)pParentWnd;
				for(int i = 0; i < MAX_GAME_ROOM; i ++)
				{
					if(pWnd->m_RoomInfo[i].pGameRoomWnd != NULL)
					{
						CString stip;
						strMessage = fMsg.GetKeyVal("PropPanelDlg","BUYSUC",
							"成功购买VIP");
						((CGameRoomEx*)pWnd->m_RoomInfo[i].pGameRoomWnd)->InsertSystemMessageWithGame(strMessage.GetBuffer());
					}
				}
			}
		break;
		default:
			{
				break;
			}
		}
		
	}
	//从服务器返回了用户的道具
	if((pNetHead->bAssistantID==ASS_PROP_BUY || pNetHead->bAssistantID==ASS_PROP_BUY_NEW) && 
        pNetHead->bHandleCode!=DTK_GP_PROP_BUY_SUCCEED && pNetHead->bHandleCode!=DTK_GP_PROP_BUYANDUSE_SUCCEED)
	{
		//{{Modify By Zxd，取消弹出对话框的表现形式
		//AFCMessageBox("购买道具失败！您的游戏银行帐号中金币不足，请充值或存入足够的金币到银行后再购买！");
		if (pNetHead->bHandleCode == DTK_GP_PROP_BUY_NOMONEY)
		{
			CMainRoomEx *pWnd=(CMainRoomEx*)pParentWnd;
			for(int i = 0; i < MAX_GAME_ROOM; i ++)
			{
				if(pWnd->m_RoomInfo[i].pGameRoomWnd != NULL)
				{
					strMessage = fMsg.GetKeyVal("PropPanelDlg","BuyErrorRechargeCoin",
						"购买道具失败！您的游戏银行帐号中金币不足，请充值或存入足够的金币到银行后再购买！");
					BZSoundPlay(this, "music/错误提示.mp3", 0);
					CString csTemp = strMessage; 
					// 在发送表情的时候会改变csTemp的内容 
					CGameRoomEx* _p = ((CGameRoomEx*)pWnd->m_RoomInfo[i].pGameRoomWnd);
					_p->m_pRightWnd->m_MessageHandle.InsertSystemMessageM(csTemp.GetBuffer()); 
					csTemp.ReleaseBuffer(); 
					if (_p->m_IGameFrame!=NULL) 
					{ 
						csTemp = strMessage;   // 重新初始化数据 
						_p->m_IGameFrame->ShowMsgInGame(csTemp.GetBuffer(), 3); 
						csTemp.ReleaseBuffer(); 

					}
				}
			}
		}
		else
		{
			CMainRoomEx *pWnd=(CMainRoomEx*)pParentWnd;
			for(int i = 0; i < MAX_GAME_ROOM; i ++)
			{
				if(pWnd->m_RoomInfo[i].pGameRoomWnd != NULL)
				{
					strMessage = fMsg.GetKeyVal("PropPanelDlg","BuyPropOtherError",
						"购买道具失败！");

					BZSoundPlay(this, "music/错误提示.mp3", 0);
					CString csTemp = strMessage; 
					// 在发送表情的时候会改变csTemp的内容 
					CGameRoomEx* _p = ((CGameRoomEx*)pWnd->m_RoomInfo[i].pGameRoomWnd);
					_p->m_pRightWnd->m_MessageHandle.InsertSystemMessageM(csTemp.GetBuffer()); 
					csTemp.ReleaseBuffer(); 
					if (_p->m_IGameFrame!=NULL) 
					{ 
						csTemp = strMessage;   // 重新初始化数据 
						_p->m_IGameFrame->ShowMsgInGame(csTemp.GetBuffer(), 3); 
						csTemp.ReleaseBuffer(); 

					}
					//((CGameRoomEx*)pWnd->m_RoomInfo[i].pGameRoomWnd)->InsertSystemMessageWithGame(strMessage.GetBuffer());
				}
			}
		}

		//Modify By Zxd}}
		return;
	}

	if((pNetHead->bAssistantID==ASS_PROP_BUY || pNetHead->bAssistantID==ASS_PROP_BUY_NEW) && 
		pNetHead->bHandleCode == DTK_GP_PROP_BUY_SUCCEED)
	{
		CMainRoomEx *pWnd=(CMainRoomEx*)pParentWnd;
		for(int i = 0; i < MAX_GAME_ROOM; i ++)
		{
			if(pWnd->m_RoomInfo[i].pGameRoomWnd != NULL)
			{
				strMessage = fMsg.GetKeyVal("PropPanelDlg","BuyPropSuc",
					"购买道具成功！");
				((CGameRoomEx*)pWnd->m_RoomInfo[i].pGameRoomWnd)->InsertSystemMessageWithGame(strMessage.GetBuffer());
			}
		}
	}
	if(pNetHead->bAssistantID==ASS_PROP_GETUSERPROP || pNetHead->bAssistantID==ASS_PROP_BUY)
	{
		MSG_PROP_S_GETPROP * msg=(MSG_PROP_S_GETPROP*)pNetData;
		//DebugPrintf("用户[%d]道具[%d]数量[%d]花费[%d]", msg->dwUserID, msg->nPropID, msg->nHoldCount, msg->dwCost);
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
		//{{Modified by ZXD 原先购买道具数量和单价是根据大厅道具面板的数值来的，在用
		// EXE形式游戏客户端时，游戏中道具面板与大厅不一致，会造成误算银行余额，为此
		// 在结构中加入一个变量成员dwCost，用来传递本次购买实际花费的钱
		if(!bPropExist)
		{
			userProp=new _TAG_USERPROP;
			memcpy(userProp,msg,sizeof(_TAG_USERPROP));
			Glb().userPropLibrary.Add(userProp);
		}
		else
		{
			userProp->nHoldCount=msg->nHoldCount;
			userProp->dwCost = msg->dwCost;
		}
		if(pNetHead->bAssistantID==ASS_PROP_BUY)
			OnGetPropInformation(userProp);

		//Modified by ZXD}}
	}
    if(pNetHead->bAssistantID==ASS_PROP_BUY_NEW)
    {
        MSG_PROP_S_GETPROP * msg=(MSG_PROP_S_GETPROP*)pNetData;

        int propCount=Glb().userPropLibrary.GetCount();
        _TAG_USERPROP *userProp=NULL;
        bool bPropExist=false;
        int iPropIndex = 0;
        for(int i=0;i<propCount;i++)
        {
            userProp=Glb().userPropLibrary.GetAt(i);
            if(userProp->nPropID==msg->nPropID)
            {
                bPropExist=true;
                iPropIndex = i;
                break;
            }
        }

        if(true == bPropExist)  // 找到
        {
            userProp->nHoldCount=msg->nHoldCount;
            userProp->dwCost = msg->dwCost;         // 本次购买实际花费的钱
        }

        // 更新
        OnGetPropInformation(userProp);

        // 操作属性此处不需要，使用这个值区分道具是在房间还是游戏端购买的
        // 0表示在房间购买，1表示在游戏端购买
        CMainRoomEx *pWnd=(CMainRoomEx*)pParentWnd;
        if(0 == msg->attribAction)      // 房间功能
        {
            for(int i = 0; i < MAX_GAME_ROOM; i ++)
            {
                if(pWnd->m_RoomInfo[i].pGameRoomWnd != NULL && true == bPropExist)
                {
					if (pNetHead->bHandleCode == DTK_GP_PROP_BUYANDUSE_SUCCEED)
						pWnd->m_RoomInfo[i].pRoomInfoWnd->UserProp_Buy(iPropIndex);
					else
						pWnd->m_RoomInfo[i].pRoomInfoWnd->UserProp(iPropIndex);
                }
            }
        }
        else if(1 == msg->attribAction) // 游戏端功能
        {
            CGameRoomEx* pGameRoomEx = NULL;
            for(int i = 0; i < MAX_GAME_ROOM; i ++)
            {
                if(pWnd->m_RoomInfo[i].pGameRoomWnd != NULL && true == bPropExist)
                {
                    pGameRoomEx = dynamic_cast<CGameRoomEx*>(pWnd->m_RoomInfo[i].pGameRoomWnd);
                    pGameRoomEx->SendDataToGame(userProp, sizeof(_TAG_USERPROP),
                                                MDM_GR_ROOM2GAME, ASS_GR_R2G_USEPROP, pNetHead->bHandleCode);
                }
            }
        }
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
                    if(userProp->nHoldCount < 0)userProp->nHoldCount = 0;
					//if(userProp->nHoldCount<1)    // PengJiLin, 2010-10-14, 新的道具系统数量可以为0，屏蔽
					//{
					//	Glb().userPropLibrary.RemoveAt(i);
					//	delete userProp;
					//	userProp=NULL;
					//}
					//AFCMessageBox("成功赠送道具给其它玩家","提示"); //Modify By Zxd，取消弹出对话框的表现形式
					CString stip;
					strMessage = fMsg.GetKeyVal("PropPanelDlg","SuccessGive","成功赠送道具给玩家[%s]");
					stip.Format(strMessage, propGive->szTargetUser);
					CMainRoomEx *pWnd=(CMainRoomEx*)pParentWnd;

					for(int i = 0; i < MAX_GAME_ROOM; i ++)
					{
						if(pWnd->m_RoomInfo[i].pGameRoomWnd != NULL)
						{
							pWnd->m_RoomInfo[i].pGameRoomWnd->PostMessage(WM_USER+151,0,0);
							pWnd->m_RoomInfo[i].pRoomInfoWnd->PostMessage(WM_USER+151,0,0);
							//{{Add By Zxd 用来代替上面的AFCMessageBox
							((CGameRoomEx*)pWnd->m_RoomInfo[i].pGameRoomWnd)->InsertSystemMessageWithGame(stip);
							//Add By Zxd}}
						}
					}
					if(curPropID==propGive->nPropID)
					{
						if(userProp)
						{
							m_nHoldCount=userProp->nHoldCount;
						}
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
				stip = fMsg.GetKeyVal("PropPanelDlg","PlayerNotExist","对不起，您要赠送的玩家不存在");
			else
				stip = fMsg.GetKeyVal("PropPanelDlg","ErrorGive","对不起，赠送道具失败！");
			//{{Modify By Zxd，取消弹出对话框的表现形式
			//AFCMessageBox(stip,"提示");
			{
				CMainRoomEx *pWnd=(CMainRoomEx*)pParentWnd;
				for(int i = 0; i < MAX_GAME_ROOM; i ++)
				{
					if(pWnd->m_RoomInfo[i].pGameRoomWnd != NULL)
					{
						((CGameRoomEx*)pWnd->m_RoomInfo[i].pGameRoomWnd)->InsertSystemMessageWithGame(stip);
					}
				}
			}
			//Modify By Zxd}}
		}
	}
	if (pNetHead->bAssistantID==ASS_PROP_BIG_BOARDCASE)
	{//大喇叭，如果成功，都要减少道具数量
		_TAG_BOARDCAST *boardCast=(_TAG_BOARDCAST*)pNetData;

		//add xqm 20010-11-23
		BZSoundPlay(this, "image\\prop\\Prop5A.mp3", 0);

		CMainRoomEx *pWnd=(CMainRoomEx*)pParentWnd;
		if(pNetHead->bHandleCode!=DTK_GR_PROP_USE_SUCCEED && boardCast->dwUserID==pWnd->m_PlaceUserInfo.dwUserID)
		{
			CMainRoomEx *pWnd=(CMainRoomEx*)pParentWnd;
			for(int i = 0; i < MAX_GAME_ROOM; i ++)
			{
				if(pWnd->m_RoomInfo[i].pGameRoomWnd != NULL)
				{
					CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
					CString strMessage = fMsg.GetKeyVal("PropPanelDlg","ErrorOpenToolBox","对不起，打开道具箱出错");
					((CGameRoomEx*)pWnd->m_RoomInfo[i].pGameRoomWnd)->InsertSystemMessageWithGame(strMessage.GetBuffer());
				}
			}
			return;
		}	
		if(boardCast->dwUserID==pWnd->m_PlaceUserInfo.dwUserID)
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
					//if(userProp->nHoldCount <=0)  // PengJiLin, 2010-10-14, 新的道具系统数量可以为0，屏蔽
					//{
					//	userProp->nHoldCount = 0;
					//	Glb().userPropLibrary.RemoveAt(i);
					//	delete userProp;
					//	userProp=NULL;
					//}
					for(int i = 0; i < MAX_GAME_ROOM; i ++)
					{
						if(pWnd->m_RoomInfo[i].pGameRoomWnd != NULL)
						{
							pWnd->m_RoomInfo[i].pGameRoomWnd->PostMessage(WM_USER+151,0,0);
							pWnd->m_RoomInfo[i].pRoomInfoWnd->PostMessage(WM_USER+151,0,0);
						}
					}
					if(curPropID==boardCast->nPropID)
					{
						if(userProp)
							m_nHoldCount=userProp->nHoldCount;
						else
							m_nHoldCount=0;
						UpdateData(FALSE);
					}
					break;
				}
			}
		}
		//将大喇叭消息显示出来
		for(int i = 0; i < MAX_GAME_ROOM; i ++)
		{
			if(pWnd->m_RoomInfo[i].pGameRoomWnd != NULL)
				((CGameRoomEx*)pWnd->m_RoomInfo[i].pGameRoomWnd)->Dudu(boardCast, pNetHead->bAssistantID==ASS_PROP_BIG_BOARDCASE);
		}
	}
}

// 设置窗口圆角 2012.10.17 yyf
void CPropPannel::SetWndRgn(void)
{
	POINT m_arPoints[8];
	CRect winRect;
	GetWindowRect(&winRect);
	ScreenToClient(&winRect);
 
    //左上角
	m_arPoints[0].x = winRect.left;
	m_arPoints[0].y = winRect.top+3;
	m_arPoints[1].x = winRect.left+3;
	m_arPoints[1].y = winRect.top;

	//右上角
	m_arPoints[2].x = winRect.right-3;
	m_arPoints[2].y = winRect.top;
	m_arPoints[3].x = winRect.right;
	m_arPoints[3].y = winRect.top+3;

	//右下角
	m_arPoints[4].x = winRect.right;
	m_arPoints[4].y = winRect.bottom-4;
	m_arPoints[5].x = winRect.right-4;
	m_arPoints[5].y = winRect.bottom;

	//左下角
	m_arPoints[6].x = winRect.left+4;
	m_arPoints[6].y = winRect.bottom;
	m_arPoints[7].x = winRect.left;
	m_arPoints[7].y = winRect.bottom-4;

	HRGN hRgn = CreatePolygonRgn(m_arPoints, 8, WINDING);
	if (hRgn!=NULL)
	{
		SetWindowRgn(hRgn,TRUE);
		DeleteObject(hRgn);
	}
}