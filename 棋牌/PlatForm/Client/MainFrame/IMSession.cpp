// IMSession.cpp : 实现文件
//

#include "stdafx.h"
#include "IMSession.h"
#include <comutil.h>
#include "MainRoomEx.h"

// CIMSession 对话框

IMPLEMENT_DYNAMIC(CIMSession, CDialog)

CIMSession::CIMSession(CWnd* pParent /*=NULL*/)
: CDialog(CIMSession::IDD, pParent)
, m_strSend(_T(""))
, m_MessageHandle(&m_chat)
{
	m_dwUserId=0;
	pHwnd=0;
	m_szName="";
	sendOnleyEnter=false;
	m_bMale=true;
	m_bOnline=false;
	imgDialog=NULL;
	m_skinIndex=0;

	//wushuqun 2009.5.20
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + strSkin);
	CString key=m_skinmgr.GetKeyVal(strSkin);
	int r,g,b;
	r=f.GetKeyVal(key,"imbgcr",44);
	g=f.GetKeyVal(key,"imbgcg",114);
	b=f.GetKeyVal(key,"imbgcb",233);

	m_bkcolor=RGB(r,g,b);
	m_bkBrush=NULL;
}

CIMSession::~CIMSession()
{
	DeleteObject(m_bkBrush);
}

void CIMSession::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHATMSG, m_chat);
	DDX_Control(pDX, IDC_COMBOFONTNAME, m_cmbFontname);
	DDX_Control(pDX, IDC_COMBOFONTSIZE, m_cmbFontsize);
	DDX_Control(pDX, IDC_COMBOCOLOR, m_cmbColor);
	DDX_Text(pDX, IDC_INPUT, m_strSend);
	DDV_MaxChars(pDX, m_strSend, 128);
	DDX_Control(pDX, IDC_INPUT, m_ctlSend);
	DDX_Control(pDX, IDC_CLOSESESSION, m_bnClose);
	DDX_Control(pDX, IDC_CLOSEBTN, m_bnClose2);
	DDX_Control(pDX, IDC_MINSESSION, m_bnMin);
	DDX_Control(pDX, IDC_SEND, m_bnSend);
	DDX_Control(pDX, IDC_SENDKEY, m_bnSendKey);
	DDX_Control(pDX, IDC_EXPRESSION, m_BtExpression);
}


BEGIN_MESSAGE_MAP(CIMSession, CDialog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_SEND, &CIMSession::OnBnClickedSend)
	ON_BN_CLICKED(IDC_SENDKEY, &CIMSession::OnBnClickedSendkey)
	ON_CBN_SELCHANGE(IDC_COMBOCOLOR, &CIMSession::OnCbnSelchangeCombocolor)
	ON_CBN_SELCHANGE(IDC_COMBOFONTSIZE, &CIMSession::OnCbnSelchangeCombofontsize)
	ON_CBN_SELCHANGE(IDC_COMBOFONTNAME, &CIMSession::OnCbnSelchangeCombofontname)
	ON_BN_CLICKED(IDC_CLOSESESSION, &CIMSession::OnBnClickedClosesession)
	ON_BN_CLICKED(IDC_MINSESSION, &CIMSession::OnBnClickedMinsession)
	ON_BN_CLICKED(IDC_CLOSEBTN, &CIMSession::OnBnClickedClosesession)
	ON_BN_CLICKED(IDC_EXPRESSION, &CIMSession::OnBnClickedExpression)
	ON_MESSAGE(WM_HIT_EXPMESSTION,&CIMSession::OnExpression)
	ON_MESSAGE(WM_EXCHANGE_SKIN,&CIMSession::OnExchangeSkin)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// CIMSession 消息处理程序

BOOL CIMSession::OnInitDialog()
{
	CDialog::OnInitDialog();

	Init();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CIMSession::Init()
{
	try
	{
		m_chat.Init();
		//颜色选择框
		for(int i=0;i<15;i++)
			m_cmbColor.InsertString(i,"");
		m_cmbColor.SetCurSel(0);

		//字体枚举
		InstalledFontCollection ifc;
		int fontCount=ifc.GetFamilyCount();
		WCHAR fontFamily[50];//存放字体名
		FontFamily pFontFamily[500];
		int fontFound;
		ifc.GetFamilies(fontCount,pFontFamily,&fontFound);
		int defaultIndex=0;
		for(INT i=0;i<fontCount;++i)
		{
			pFontFamily[i].GetFamilyName(fontFamily);
			CString sFont;
			sFont.Format("%s",(char*)_bstr_t((BSTR)fontFamily));
			int index=m_cmbFontname.AddString(sFont);
			if(sFont=="宋体")
				defaultIndex=index;
		}
		if(m_cmbFontname.GetCount())
			m_cmbFontname.SetCurSel(defaultIndex);

		//字体大小
		m_cmbFontsize.AddString("6");
		m_cmbFontsize.AddString("8");
		m_cmbFontsize.AddString("9");
		m_cmbFontsize.AddString("10");
		m_cmbFontsize.AddString("11");
		m_cmbFontsize.AddString("12");
		m_cmbFontsize.AddString("14");
		m_cmbFontsize.AddString("16");
		m_cmbFontsize.AddString("18");
		m_cmbFontsize.AddString("24");
		m_cmbFontsize.AddString("36");
		m_cmbFontsize.AddString("72");
		m_cmbFontsize.SetCurSel(2);

		//初始化字体
		charFormat.cbSize		= sizeof(CHARFORMAT);
		charFormat.dwMask		= CFM_FACE|CFM_SIZE|CFM_COLOR|CFM_BOLD|CFE_STRIKEOUT|CFE_ITALIC|CFE_UNDERLINE;
		strcpy(charFormat.szFaceName,"宋体");
		charFormat.yHeight		=9*20;
		charFormat.dwEffects	=(unsigned long)0;
		charFormat.crTextColor=RGB(0,0,0);

		m_ctlSend.SetSel(0,0);
		m_ctlSend.SetSelectionCharFormat(charFormat);

		charTipFormat.cbSize		= sizeof(CHARFORMAT);
		charTipFormat.dwMask		= CFM_FACE|CFM_SIZE|CFM_COLOR|CFM_BOLD|CFE_STRIKEOUT|CFE_ITALIC|CFE_UNDERLINE;
		strcpy(charFormat.szFaceName,"宋体");
		charTipFormat.yHeight		=9*20;
		charTipFormat.dwEffects	=(unsigned long)0;
		charTipFormat.crTextColor=RGB(0,0,255);

		CString skinfolder;
		TCHAR path[MAX_PATH];
		CString strSkin = m_skinmgr.GetSkinBcfFileName();
		CBcfFile f( CBcfFile::GetAppPath () + strSkin);
		if ("skin0.bcf" == strSkin)
			skinfolder=f.GetKeyVal("skin0","skinfolder",m_skinmgr.GetSkinPath());
		else
			skinfolder=f.GetKeyVal("skin1","skinfolder",m_skinmgr.GetSkinPath());

		wsprintf(path,"%sdialog\\sendmessage_bk.bmp",skinfolder);
		m_bkimage.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
		CGameImageHelper ImageHandle(&m_bkimage);
		HRGN hRgn=AFCBmpToRgn(ImageHandle,m_bkimage.GetPixel(0,0),RGB(1,1,1));
		if (hRgn!=NULL)
		{
			SetWindowRgn(hRgn,TRUE);
			DeleteObject(hRgn);
		}
		CRect rc,btnRc;
		GetWindowRect(&rc);

		CString key=TEXT("ShortMessage");
		int r,g,b;
		r=f.GetKeyVal(key,"talkbkr",0);
		g=f.GetKeyVal(key,"talkbkg",0);
		b=f.GetKeyVal(key,"talkbkb",0);
		m_colorresource.m_crTalkBkColor=RGB(r,g,b);
		m_chat.m_bkColor=RGB(r,g,b);
		m_chat.SetBackgroundColor(false,RGB(r,g,b));
		r=f.GetKeyVal(key,"talknamer",255);
		g=f.GetKeyVal(key,"talknameg",255);
		b=f.GetKeyVal(key,"talknameb",255);
		m_colorresource.m_crTalkNameColor=RGB(r,g,b);
		r=f.GetKeyVal(key,"systemr",255);
		g=f.GetKeyVal(key,"systemg",255);
		b=f.GetKeyVal(key,"systemb",255);
		m_colorresource.m_crSystemTxColor=RGB(r,g,b);
		r=f.GetKeyVal(key,"systembdr",255);
		g=f.GetKeyVal(key,"systembdg",0);
		b=f.GetKeyVal(key,"systembdb",0);
		m_colorresource.m_crSystemBdColor=RGB(r,g,b);
		r=f.GetKeyVal(key,"talknormalr",255);
		g=f.GetKeyVal(key,"talknormalg",255);
		b=f.GetKeyVal(key,"talknormalb",255);
		m_colorresource.m_crNormalTxColor=RGB(r,g,b);	
		r=f.GetKeyVal(key,"talkr",255);
		g=f.GetKeyVal(key,"talkg",255);
		b=f.GetKeyVal(key,"talkb",255);
		m_colorresource.m_crTalkTxColor=RGB(r,g,b);	

		m_MessageHandle.SetColor(m_colorresource);

		/// 按着底图大小调整窗口大小
		MoveWindow(rc.left, rc.top, m_bkimage.GetWidth(), m_bkimage.GetHeight(), TRUE);

		setSkinIndex(0);

		m_bkBrush=CreateSolidBrush(m_bkcolor);

		CString szMsg = f.GetKeyVal(key,"xtgg","欢迎您来到面对面游戏中心，请不要在聊天框中透露帐号和密码，切记不要相信聊天框中的任何活动或中奖信息。拒绝虚拟货币交易，加强自我防范意识。警惕上当受骗，后果自负。一切活动以官方网站首页公告为准。");
		//SendSystemBoard(" " + szMsg + "\n", m_colorresource.m_crNormalTxColor);
		m_MessageHandle.InsertSystemBoard(szMsg.GetBuffer(szMsg.GetLength()));


	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"IMSESSION_TRY_CATCH : [%s]",__FUNCTION__);
	}
}

void CIMSession::LoadSkin()
{
	try
	{
		CString skinfolder;
		TCHAR path[MAX_PATH];
		CString strSkin = m_skinmgr.GetSkinBcfFileName();
		CBcfFile f( CBcfFile::GetAppPath () + strSkin);
		skinfolder=f.GetKeyVal(m_skinmgr.GetKeyVal(strSkin),"skinfolder",m_skinmgr.GetSkinPath());

		wsprintf(path,"%sdialog\\sendmessage_bk.bmp",skinfolder);
		m_bkimage.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
		CGameImageHelper ImageHandle(&m_bkimage);
		HRGN hRgn=AFCBmpToRgn(ImageHandle,m_bkimage.GetPixel(0,0),RGB(1,1,1));
		if (hRgn!=NULL)
		{
			SetWindowRgn(hRgn,TRUE);
			DeleteObject(hRgn);
		}
		CRect rc,btnRc;
		GetWindowRect(&rc);

		CString key=TEXT("ShortMessage");
		int r,g,b;
		r=f.GetKeyVal(key,"talkbkr",0);
		g=f.GetKeyVal(key,"talkbkg",0);
		b=f.GetKeyVal(key,"talkbkb",0);
		m_colorresource.m_crTalkBkColor=RGB(r,g,b);
		m_chat.m_bkColor=RGB(r,g,b);
		m_chat.SetBackgroundColor(false,RGB(r,g,b));
		r=f.GetKeyVal(key,"talknamer",255);
		g=f.GetKeyVal(key,"talknameg",255);
		b=f.GetKeyVal(key,"talknameb",255);
		m_colorresource.m_crTalkNameColor=RGB(r,g,b);
		r=f.GetKeyVal(key,"systemr",255);
		g=f.GetKeyVal(key,"systemg",255);
		b=f.GetKeyVal(key,"systemb",255);
		m_colorresource.m_crSystemTxColor=RGB(r,g,b);
		r=f.GetKeyVal(key,"systembdr",255);
		g=f.GetKeyVal(key,"systembdg",0);
		b=f.GetKeyVal(key,"systembdb",0);
		m_colorresource.m_crSystemBdColor=RGB(r,g,b);
		r=f.GetKeyVal(key,"talknormalr",255);
		g=f.GetKeyVal(key,"talknormalg",255);
		b=f.GetKeyVal(key,"talknormalb",255);
		m_colorresource.m_crNormalTxColor=RGB(r,g,b);	
		r=f.GetKeyVal(key,"talkr",255);
		g=f.GetKeyVal(key,"talkg",255);
		b=f.GetKeyVal(key,"talkb",255);
		m_colorresource.m_crTalkTxColor=RGB(r,g,b);	

		m_MessageHandle.SetColor(m_colorresource);

		/// 按着底图大小调整窗口大小
		MoveWindow(rc.left, rc.top, m_bkimage.GetWidth(), m_bkimage.GetHeight(), TRUE);

		setSkinIndex(0);
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"IMSESSION_TRY_CATCH : [%s]",__FUNCTION__);
	}
}
void CIMSession::OnClose()
{
	if(pHwnd)
		::PostMessage(pHwnd,WM_CLOSE_SESSION,(WPARAM)this,0);
}

void CIMSession::setWindowOwner(HWND pParentHwnd)
{
	pHwnd=pParentHwnd;
}

void CIMSession::OnBnClickedSend()
{
	try
	{
		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");

		if(!m_bOnline)
		{
			//AFCMessageBox(fMsg.GetKeyVal("IMDlg","NotSendToOffLineUser","对不起，您不能给不在线用户发消息！"),fMsg.GetKeyVal("IMDlg","Tip","提示"));
	        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,fMsg.GetKeyVal("IMDlg","Tip","提示"),fMsg.GetKeyVal("IMDlg","NotSendToOffLineUser","对不起，您不能给不在线用户发消息！"));
			return;
		}
		UpdateData();
		m_strSend.TrimLeft();
		m_strSend.TrimRight();
		if(m_strSend.GetLength()==0)
		{
			//AFCMessageBox(fMsg.GetKeyVal("IMDlg","NotSendNullMsg","对不起，您不能发送空的消息"),fMsg.GetKeyVal("IMDlg","Tip","提示"));
	        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,fMsg.GetKeyVal("IMDlg","Tip","提示"),fMsg.GetKeyVal("IMDlg","NotSendNullMsg","对不起，您不能发送空的消息"));
			return;
		}
		int lines=m_chat.GetLineCount();
		if(lines>100)
		{
			CString s;
			int num=m_chat.GetLine(0,s.GetBuffer(MAX_PATH),20);
			m_chat.SetSel(0,num);
			m_chat.ReplaceSel("");
		}

		//发往服务器
		CMainRoomEx *parentWnd=(CMainRoomEx*)pWnd;

		MSG_IM_C_MSG msg;
		ZeroMemory(&msg,sizeof(msg));
		msg.dwUserID=parentWnd->m_PlaceUserInfo.dwUserID;
		memcpy(&msg.szUserName,parentWnd->m_PlaceUserInfo.nickName ,sizeof(parentWnd->m_PlaceUserInfo.nickName));
		msg.dwRemoteUserID=m_dwUserId;
		memcpy(&msg.cf,&charFormat,sizeof(CHARFORMAT));
		memcpy(&msg.szFontname,m_charFontName,m_charFontName.GetLength());
		msg.szMsgLength=m_strSend.GetLength();
		memcpy(&msg.szMsg,m_strSend,m_strSend.GetLength());
		msg.cbSize=sizeof(MSG_IM_C_MSG)-sizeof(msg.szMsg)+msg.szMsgLength+1;
		parentWnd->SendData(&msg,msg.cbSize,MDM_GP_IM,ASS_IMC_MSG,0);

// 		m_chat.SetSel(m_chat.GetWindowTextLength(),m_chat.GetWindowTextLength());
// 		strcpy(charTipFormat.szFaceName,"宋体");
// 		m_chat.SetSelectionCharFormat(charTipFormat);
// 		CString strInput;
// 		CString str = fMsg.GetKeyVal("IMDlg","ISay","[%s] 我 说：\n");
// 		strInput.Format(str,CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S"));
// 		m_chat.ReplaceSel(strInput);
// 		m_strSend+="\n";
// 		m_chat.SetSel(m_chat.GetWindowTextLength(),m_chat.GetWindowTextLength());
// 		strcpy(charFormat.szFaceName,m_charFontName);
// 		m_chat.SetSelectionCharFormat(charFormat);
// 		m_chat.ReplaceSel(m_strSend);
// 		m_strSend="";
// 		UpdateData(FALSE);
// 
// 		m_chat.LineScroll(2);
// 
// 		m_ctlSend.SetSel(0,m_ctlSend.GetWindowTextLength());
// 		strcpy(charFormat.szFaceName,m_charFontName);
// 		m_ctlSend.SetSelectionCharFormat(charFormat);

		m_MessageHandle.InsertUserTalk(msg.szUserName,NULL,msg.szMsg,RGB(0,0,0),true);
		::SendMessage(m_chat.m_hWnd,WM_VSCROLL,SB_PAGEDOWN,NULL);
		this->SetForegroundWindow();

	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"IMSESSION_TRY_CATCH : [%s]",__FUNCTION__);
	}

}

//表情按钮
void CIMSession::OnBnClickedExpression()
{
	//建立表情窗口
	if (m_ExpressWnd.GetSafeHwnd()==NULL) m_ExpressWnd.CreateExpressionWnd(this);

	//移动窗口
	CRect ButtonRect;
	CSize ExpressSize;
	m_ExpressWnd.GetFixSize(ExpressSize);
	m_BtExpression.GetWindowRect(&ButtonRect);
	m_ExpressWnd.MoveWindow(ButtonRect.left,ButtonRect.top-ExpressSize.cy,ExpressSize.cx,ExpressSize.cy);
	m_ExpressWnd.ShowWindow(SW_SHOW);
	m_ExpressWnd.SetFocus();

	return;
}

//表情消息
LRESULT CIMSession::OnExpression(WPARAM wparam, LPARAM lparam)
{
	CTrueItem * pExpItem=m_ExpressWnd.GetExpressItem((UINT)wparam);
	if (pExpItem!=NULL)
	{
		CString strBuffer;
		m_ctlSend.GetWindowText(strBuffer);
		strBuffer+=pExpItem->m_szTrancelate;
		m_ctlSend.SetWindowText(strBuffer);
		m_ctlSend.SetFocus();
		m_ctlSend.SetSel(strBuffer.GetLength(),strBuffer.GetLength());	
	}
	return 0;
}

void CIMSession::OnBnClickedSendkey()
{
	try
	{
		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
		CMenu menu;
		menu.CreatePopupMenu();
		menu.AppendMenu(MF_STRING,10 ,fMsg.GetKeyVal("IMDlg","CtrlEnterSend","Ctrl+Enter 发送"));
		menu.AppendMenu(MF_STRING,20,fMsg.GetKeyVal("IMDlg","EnterSend","Enter 发送"));
		if(sendOnleyEnter)
		{
			menu.CheckMenuItem(20,MF_CHECKED);
			menu.CheckMenuItem(10,MF_UNCHECKED);
		}
		else
		{
			menu.CheckMenuItem(10,MF_CHECKED);
			menu.CheckMenuItem(20,MF_UNCHECKED);
		}
		CPoint p;
		GetCursorPos(&p);
		int ret=TrackPopupMenuEx(menu.m_hMenu,TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RETURNCMD,p.x,p.y,this->m_hWnd,NULL);
		DestroyMenu(menu.m_hMenu);
		if(ret==10)
		{
			sendOnleyEnter=false;
			GetDlgItem(IDC_SENDSTATE)->SetWindowText(fMsg.GetKeyVal("IMDlg","CtrlEnterSend","Ctrl+Enter 发送"));
		}
		else
		{
			sendOnleyEnter=true;
			GetDlgItem(IDC_SENDSTATE)->SetWindowText(fMsg.GetKeyVal("IMDlg","EnterSend","Enter 发送"));
		}
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"IMSESSION_TRY_CATCH : [%s]",__FUNCTION__);
	}

}

void CIMSession::setUserOnline(bool bOnline)
{
	try
	{
		if(!this->m_hWnd)return;
		m_bOnline=bOnline;
		CString sCaption;
		this->GetWindowText(sCaption);
		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
		sCaption.Format(fMsg.GetKeyVal("IMDlg","ChatWithSB","与 [%s] (%s) 对话中"),
			m_szName,m_bOnline?fMsg.GetKeyVal("IMDlg","OnLine","在线"):fMsg.GetKeyVal("IMDlg","OffLine","不在线"));
		this->SetWindowText("     "+sCaption); /// 前面的空格是为了留出Title栏的Icon的位置
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"IMSESSION_TRY_CATCH : [%s]",__FUNCTION__);
	}
}

void CIMSession::setUser(int dwUserID, CString szName,CWnd *pParentWnd , bool bMale, bool bOnline)
{
	try
	{
		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
		m_dwUserId=dwUserID;
		pWnd=pParentWnd;
		m_szName=szName;
		CString sCaption;	/// 流出Title栏的Icon的位置
		//	sCaption.Format("与 [%s] 对话中",szName);
		sCaption = "     " + fMsg.GetKeyVal("IMDlg","ChatWith","ChatWith ");
		sCaption += szName;
		SetWindowText(sCaption);
		setUserOnline(bOnline);
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"IMSESSION_TRY_CATCH : [%s]",__FUNCTION__);
	}
}

void CIMSession::OnCbnSelchangeCombocolor()
{
	try
	{
		int itemId=m_cmbColor.GetCurSel();
		switch(itemId)
		{
		case 0:	charFormat.crTextColor=RGB(0,0,0);				break;
		case 1:	charFormat.crTextColor=RGB(128,0,0);			break;
		case 2:	charFormat.crTextColor=RGB(0,128,0);			break;
		case 3:	charFormat.crTextColor=RGB(128,128,0);			break;
		case 4:	charFormat.crTextColor=RGB(0,0,128);			break;
		case 5:	charFormat.crTextColor=RGB(128,0,128);			break;
		case 6:	charFormat.crTextColor=RGB(0,128,128);			break;
		case 7:	charFormat.crTextColor=RGB(128,128,128);		break;
		case 8:	charFormat.crTextColor=RGB(255,0,0);			break;
		case 9:	charFormat.crTextColor=RGB(0,255,0);			break;
		case 10:	charFormat.crTextColor=RGB(255,255,0);		break;
		case 11:	charFormat.crTextColor=RGB(0,0,255);		break;
		case 12:	charFormat.crTextColor=RGB(255,0,255);		break;
		case 13:	charFormat.crTextColor=RGB(0,255,255);		break;
		case 14:	charFormat.crTextColor=RGB(255,255,255);	break;
		}
		m_ctlSend.SetSel(0,m_ctlSend.GetWindowTextLength());
		m_ctlSend.SetSelectionCharFormat(charFormat);
		m_ctlSend.SetSel(m_ctlSend.GetWindowTextLength(),m_ctlSend.GetWindowTextLength());

	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"IMSESSION_TRY_CATCH : [%s]",__FUNCTION__);
	}
}

void CIMSession::OnCbnSelchangeCombofontsize()
{
	try
	{
		CString sFontSize;
		m_cmbFontsize.GetLBText(m_cmbFontsize.GetCurSel(),sFontSize);
		charFormat.yHeight=atoi(sFontSize)*20;
		m_ctlSend.SetSel(0,m_ctlSend.GetWindowTextLength());
		m_ctlSend.SetSelectionCharFormat(charFormat);
		m_ctlSend.SetSel(m_ctlSend.GetWindowTextLength(),m_ctlSend.GetWindowTextLength());

	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"IMSESSION_TRY_CATCH : [%s]",__FUNCTION__);
	}
}

void CIMSession::OnCbnSelchangeCombofontname()
{
	try
	{
		m_cmbFontname.GetLBText(m_cmbFontname.GetCurSel(),m_charFontName);
		strcpy(charFormat.szFaceName,m_charFontName);	
		m_ctlSend.SetSel(0,m_ctlSend.GetWindowTextLength());
		m_ctlSend.SetSelectionCharFormat(charFormat);
		m_ctlSend.SetSel(m_ctlSend.GetWindowTextLength(),m_ctlSend.GetWindowTextLength());

	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"IMSESSION_TRY_CATCH : [%s]",__FUNCTION__);
	}

}

BOOL CIMSession::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam==VK_ESCAPE || pMsg->wParam == VK_F1)
			return TRUE;
		if(pMsg->wParam==VK_RETURN)
		{
			if(this->GetFocus()!=&m_ctlSend)
				return TRUE;
			if(GetKeyState(VK_CONTROL)<0)
			{
				if(!sendOnleyEnter)
				{
					OnBnClickedSend();
					return TRUE;
				}
			}
			else
			{
				if(sendOnleyEnter)
				{
					OnBnClickedSend();
					return TRUE;
				}
			}
		}
	}	
	return CDialog::PreTranslateMessage(pMsg);
}


void CIMSession::OnGetMessage(MSG_IM_S_MSG * msg)
{
	try
	{
		//m_chat.SetSel(m_chat.GetWindowTextLength(),m_chat.GetWindowTextLength());
		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");

// 		strcpy(charTipFormat.szFaceName,"宋体");
// 		m_chat.SetSelectionCharFormat(charTipFormat);
// 		CString strInput;
// 		CString str = fMsg.GetKeyVal("IMDlg","Say","[%s] %s 说：\n");
// 		strInput.Format(str,CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S"),msg->szUserName);
// 		m_chat.ReplaceSel(strInput);
// 		CString smsg=msg->szMsg;
// 		smsg+="\n";

// 		m_chat.SetSel(m_chat.GetWindowTextLength(),m_chat.GetWindowTextLength());
// 		strcpy(msg->cf.szFaceName,msg->szFontname);
// 		m_chat.SetSelectionCharFormat(msg->cf);
// 		m_chat.ReplaceSel(smsg);
// 		FlashWindow(TRUE);//Ex(FLASHW_TIMERNOFG|FLASHW_TRAY, 1, 0);
		m_MessageHandle.InsertUserTalk(msg->szUserName,NULL,msg->szMsg,RGB(0,0,0),true);
		::SendMessage(m_chat.m_hWnd,WM_VSCROLL,SB_PAGEDOWN,NULL);
		this->SetForegroundWindow();

	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"IMSESSION_TRY_CATCH : [%s]",__FUNCTION__);
	}
}

void CIMSession::OnBnClickedClosesession()
{
	CDialog::OnCancel();
	// TODO: 在此添加控件通知处理程序代码
}

void CIMSession::OnBnClickedMinsession()
{
	// TODO: 在此添加控件通知处理程序代码
	::SendMessage(this->GetParent()->m_hWnd, WM_SIZE, SIZE_MINIMIZED, 0);
}


void CIMSession::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()
	drawSkin();
}

void CIMSession::setSkinIndex(int sk)
{
	try
	{
		if(sk>=0 && sk<4)
			m_skinIndex=sk;
		else
			m_skinIndex=0;

		CString s;
		s.Format("%s\\%sdialog\\dialog.bmp",CBcfFile::GetAppPath(true),m_skinmgr.GetSkinPath());
#ifndef BZ_ZLIB  //< add by wxx 
		imgDialog=Image::FromFile(s.AllocSysString());
#else

		CUnZipRes unZipRes;
		IStream * pStream=NULL;
		TCHAR * pchar=s.GetBuffer(s.GetLength()+1);
		pStream=unZipRes.LoadFileResFromZip(pchar);

		imgDialog=new Image(pStream);
		if(pStream!=NULL)
		{
			pStream->Release();
			pStream=NULL;
		}

#endif

		CGameImage bt;
		CRect btnRc;
		TCHAR path[MAX_PATH];

		wsprintf(path,"%s\\%sdialog\\log_exit_bt.bmp",CBcfFile::GetAppPath(true),m_skinmgr.GetSkinPath());//win_close.bmp
		m_bnClose.LoadButtonBitmap(path,false);
		m_bnClose.GetClientRect(&btnRc);
		int iCloseWidth = btnRc.Width();
		m_bnClose.MoveWindow(m_bkimage.GetWidth()-iCloseWidth,0,btnRc.Width(),btnRc.Height());

		wsprintf(path,"%s\\%sdialog\\close_bt.bmp",CBcfFile::GetAppPath(true),m_skinmgr.GetSkinPath());
		m_bnClose2.LoadButtonBitmap(path,false);
		bt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
		m_bnClose2.MoveWindow(260, 400, bt.GetWidth()/4,bt.GetHeight());

		wsprintf(path,"%s\\%sdialog\\top_min_bt.bmp",CBcfFile::GetAppPath(true),m_skinmgr.GetSkinPath());
		m_bnMin.LoadButtonBitmap(path,false);
		m_bnMin.GetClientRect(&btnRc);
		bt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
		m_bnMin.MoveWindow(m_bkimage.GetWidth()-btnRc.Width()-iCloseWidth,0,bt.GetWidth()/4,bt.GetHeight());

		wsprintf(path,"%s\\%sdialog\\send_bt.bmp",CBcfFile::GetAppPath(true),m_skinmgr.GetSkinPath());
		m_bnSend.LoadButtonBitmap(path,false);
		bt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
		m_bnSend.MoveWindow(330, 400, bt.GetWidth()/4,bt.GetHeight());

		wsprintf(path,"%s\\%sdialog\\imsendkey.bmp",CBcfFile::GetAppPath(true),m_skinmgr.GetSkinPath());
		m_bnSendKey.LoadButtonBitmap(path,false);

		wsprintf(path,"%s\\%sdialog\\face03.bmp",CBcfFile::GetAppPath(true),m_skinmgr.GetSkinPath());
		m_BtExpression.LoadButtonBitmap(path,false);//表情选择的
		bt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
		m_BtExpression.MoveWindow(9, 259, bt.GetWidth()/4,bt.GetHeight());

		m_chat.MoveWindow(3, 30, 394, 229);
		GetDlgItem(IDC_INPUT)->MoveWindow(3, 283, 394, 107);
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"IMSESSION_TRY_CATCH : [%s]",__FUNCTION__);
	}


	//m_bkcolor=RGB(44,114,233);
}

void CIMSession::drawSkin(void)
{
	try
	{
		if(!imgDialog)
			return;
		CDC *pdc=this->GetWindowDC();
		if(!pdc)return;
		CRect rc;
		this->GetWindowRect(&rc);
		Graphics g(pdc->m_hDC);
// 		int topHeight=27;//GetSystemMetrics(SM_CYFRAME)+GetSystemMetrics(SM_CYSIZE)+6;
// 		g.DrawImage(imgDialog,RectF(0.0f,0.0f,10.0f,topHeight),0.0f,0.0f,10.0f,topHeight,UnitPixel);
// 		g.DrawImage(imgDialog,RectF(10.0f,0.0f,rc.Width()-20,topHeight),9.0f,0.0f,1.0f,topHeight,UnitPixel);
// 		g.DrawImage(imgDialog,RectF((REAL)rc.Width()-10,0.0f,10.0f,topHeight),15.0f,0.0f,10.0f,topHeight,UnitPixel);
// 		//m_bkcolor=pdc->GetPixel(9,topHeight-1);
// 
// 		int bottomHeight=5;
// 		g.DrawImage(imgDialog,RectF(0.0f,topHeight,5.0f,rc.Height()-topHeight-bottomHeight),0.0f,topHeight,5.0f,60.0f,UnitPixel);
// 		g.DrawImage(imgDialog,RectF(rc.Width()-5.0f,topHeight,5.0f,rc.Height()-topHeight-bottomHeight),20,topHeight,5.0f,60.0f,UnitPixel);
// 		//g.DrawImage(imgDialog,RectF(rc.Width()-15,30,15,rc.Height()-40),10,30,15,60,UnitPixel);
// 
// 		g.DrawImage(imgDialog,RectF(0,rc.Height()-bottomHeight,10,bottomHeight),0,100-bottomHeight,10,bottomHeight,UnitPixel);
// 		g.DrawImage(imgDialog,RectF(10,rc.Height()-bottomHeight,rc.Width()-20,bottomHeight),10,100-bottomHeight,1,bottomHeight,UnitPixel);
// 		g.DrawImage(imgDialog,RectF(rc.Width()-10,rc.Height()-bottomHeight,10,bottomHeight),15,100-bottomHeight,10,bottomHeight,UnitPixel);

		CPaintDC dc(this); // device context for painting
		CGameImageHelper helplt(&m_bkimage);
		helplt.BitBlt(dc.GetSafeHdc(),0,0);

		CString s="宋体";

		Font font(s.AllocSysString(),12,0,UnitPixel);
		SolidBrush brush(Color(255,255,255));
		GetWindowText(s);
		//s="我的好友列表";
		g.DrawString(s.AllocSysString(),-1,&font,PointF(5,5),0,&brush);
		g.ReleaseHDC(pdc->m_hDC);
		m_bnClose.Invalidate(FALSE);
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"IMSESSION_TRY_CATCH : [%s]",__FUNCTION__);
	}

}

void CIMSession::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
	CDialog::OnLButtonDown(nFlags, point);
	//	CDialog::OnLButtonDown(nFlags, point);
}

LRESULT CIMSession::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT lrst=CDialog::DefWindowProc(message, wParam, lParam);

	if (!::IsWindow(m_hWnd))
		return lrst;

	if (message==WM_MOVE||message==WM_PAINT||message==WM_NCPAINT||message==WM_NCACTIVATE)// ||message == WM_NOTIFY)
	{
		drawSkin();
	}
	return lrst;
}

LRESULT CIMSession::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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
		return (LRESULT)m_bkBrush;
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

//换肤
LRESULT CIMSession::OnExchangeSkin(WPARAM wpara,LPARAM lpara)
{
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + strSkin);
	CString key=m_skinmgr.GetKeyVal(strSkin);
	int r,g,b;
	r=f.GetKeyVal(key,"imbgcr",44);
	g=f.GetKeyVal(key,"imbgcg",114);
	b=f.GetKeyVal(key,"imbgcb",233);

	m_bkcolor=RGB(r,g,b);

	LoadSkin();
	Invalidate();
	return LRESULT(0);
}
