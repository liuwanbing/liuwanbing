#include "Stdafx.h"
#include "ShortMessage.h"
#include "..\MainFrame\GameRoomEx.h"//此种代价是惨重的??
#include "..\MainFrame\IMMain.h"//此种代价是惨重的??
#include ".\shortmessage.h"
#include ".\gameframedlg.h"
//#include "userpower.h"

#include "detours.h"
#include "skinsb.h"


#define CONTROL_SPACE						2							//控件间距
//#define PIC_BT_WIDE							20							//按钮宽度
#define PIC_BT_WIDE							20							//按钮宽度
#define PIC_BT_HIGHT						20							//按钮高度
#define BT_BUTTON_WIDE						65							//按钮宽度
#define BT_BUTTON_HIGHT						21							//按钮高度

BEGIN_MESSAGE_MAP(CShortMessage, CGameFaceGo)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_GETMINMAXINFO()
	ON_MESSAGE(WM_HIT_LOGO,OnHitLogo)
	ON_MESSAGE(WM_HIT_EXPMESSTION,OnExpression)
	ON_BN_CLICKED(IDC_SEND_SHORT, OnBnClickedSendShort)
	ON_BN_CLICKED(IDC_CLOSESM, OnBnClickedClose)
	ON_BN_CLICKED(IDC_MINSESMT, OnBnClickedMin)
	ON_BN_CLICKED(IDC_CLOSESMT, OnBnClickedClose)
///	ON_BN_CLICKED(IDC_HISTORY, OnBnClickedHistory)
	ON_BN_CLICKED(IDC_EXPRESSION, OnBnClickedExpression)
    ON_BN_CLICKED(IDC_CHATRECORD,OnBnClickedRecordChat)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

//构造函数
CShortMessage::CShortMessage(CWnd * pParentWnd) : CGameFaceGo(IDD_SHORT_MESSAGE), m_MessageHandle(&m_RecvMessage)
{
	m_pParentWnd=NULL;
	m_dwTargerAccID=0;
	m_dwTargetUserID=0L;
	m_dwLastTalkTime=0L;
	m_pParentWnd=pParentWnd;
	m_bIsInRoom = true;
	//m_bIsInRoom = false;
	m_bkBrush=NULL;
	m_dwGamePower = 0;
}

//析构函数
CShortMessage::~CShortMessage()
{
	DeleteObject(m_bkBrush);
}

//控件绑定函数
void CShortMessage::DoDataExchange(CDataExchange* pDX)
{
	CGameFaceGo::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SEND_SHORT, m_BtSend);
	DDX_Control(pDX, IDC_EXPRESSION, m_BtExpression);
///	DDX_Control(pDX, IDC_HISTORY, m_BtHistory);
	DDX_Control(pDX, IDC_CLOSESM, m_BtClose2);
	DDX_Control(pDX, IDC_CLOSESMT, m_BtClose);
	DDX_Control(pDX, IDC_MINSESMT, m_BtMin);
	DDX_Control(pDX, IDC_SEND_MESSAGE, m_SendMessage);
	DDX_Control(pDX, IDC_RECV_MESSAGE, m_RecvMessage);
}
//设置登录账户
void CShortMessage::SetUserAccount(const CString &strUserAccount)
{
    m_strUserAccount = strUserAccount;
}
//初始化函数
BOOL CShortMessage::OnInitDialog()
{
	__super::OnInitDialog();

	//设置控件
	m_RecvMessage.Init();
	m_SendMessage.SetFocus();

// 	TCHAR szCaption[100];
// 	if (m_iGmID == 0)
// 		wsprintf(szCaption,"与 %s 聊天中",m_strTargerName);
// 	else 
// 		wsprintf(szCaption,"系统消息：管理员%s",m_strTargerName);
// 	
    m_BtChatRecord.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE,CRect(0,0,0,0),this,IDC_CHATRECORD);

	//建立提示控件
	m_ToolTip.Create(this);
	m_ToolTip.Activate(TRUE);
	m_ToolTip.AddTool(GetDlgItem(IDC_SEND_SHORT),TEXT("发送信息"));
	m_ToolTip.AddTool(GetDlgItem(IDC_EXPRESSION),TEXT("选择表情头像"));
	m_ToolTip.AddTool(GetDlgItem(IDC_CLOSESM),TEXT("关闭短信息窗口"));
//	m_ToolTip.AddTool(GetDlgItem(IDC_HISTORY),TEXT("查看聊天历史记录"));

	//加载位图
	ASSERT(m_pTargetUserItem!=NULL);
	int r,g,b;
	CString s=CBcfFile::GetAppPath();//本地路径
    CString strSkin = m_skinmgr.GetSkinBcfFileName(m_strUserAccount);
	CBcfFile f( s + strSkin);
	CString key=TEXT("ShortMessage");
    TCHAR path[MAX_PATH] = {0};
	CString skinfolder;
	CGameImage bt;
	skinfolder=f.GetKeyVal(m_skinmgr.GetKeyVal(strSkin),"skinfolder",m_skinmgr.GetSkinPath());

	wsprintf(path,"%sdialog\\sendmessage_bk.bmp",skinfolder);
	m_bk.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	CGameImageHelper ImageHandle(&m_bk);
	HRGN hRgn=AFCBmpToRgn(ImageHandle,(m_bk.GetPixel(0,0)),RGB(1,1,1));
	if (hRgn!=NULL)
	{
		SetWindowRgn(hRgn,TRUE);
		DeleteObject(hRgn);
	}

	CRect btnRc;
	wsprintf(path,"%sdialog\\log_exit_bt.bmp",skinfolder);//win_close.bmp
	m_BtClose.LoadButtonBitmap(path,false);
	m_BtClose.GetClientRect(&btnRc);
	int iCloseWidth = btnRc.Width();
	m_BtClose.MoveWindow(m_bk.GetWidth()-iCloseWidth,0,btnRc.Width(),btnRc.Height());

	wsprintf(path,"%sdialog\\top_min_bt.bmp",skinfolder);
	m_BtMin.LoadButtonBitmap(path,false);
	m_BtMin.GetClientRect(&btnRc);
	bt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	m_BtMin.MoveWindow(m_bk.GetWidth()-btnRc.Width()-iCloseWidth,0,bt.GetWidth()/4,bt.GetHeight());
	m_BtMin.ShowWindow(SW_HIDE);

	wsprintf(path,"%sdialog\\face03.bmp",skinfolder);
	m_BtExpression.LoadButtonBitmap(path,false);//表情选择的
	bt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	m_BtExpression.MoveWindow(9, 259, bt.GetWidth()/4,bt.GetHeight());

    wsprintf(path,"%sdialog\\bt_ChatRecord.bmp",skinfolder);
    m_BtChatRecord.LoadButtonBitmap(path,false);
    bt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
    m_BtChatRecord.MoveWindow(260, 259, bt.GetWidth()/4,bt.GetHeight());
    m_BtChatRecord.ShowWindow(SW_SHOW);

	wsprintf(path,"%sdialog\\close_bt.bmp",skinfolder);
	m_BtClose2.LoadButtonBitmap(path,false);
	bt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	m_BtClose2.MoveWindow(260, 400, bt.GetWidth()/4,bt.GetHeight());

	wsprintf(path,"%sdialog\\send_bt.bmp",skinfolder);
	m_BtSend.LoadButtonBitmap(path,false);
	bt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	m_BtSend.MoveWindow(330, 400, bt.GetWidth()/4,bt.GetHeight());

	m_RecvMessage.MoveWindow(3, 30, 394, 229);
	m_SendMessage.MoveWindow(3, 283, 394, 107);

	r=f.GetKeyVal(key,"talkbkr",0);
	g=f.GetKeyVal(key,"talkbkg",0);
	b=f.GetKeyVal(key,"talkbkb",0);
	m_colorresource.m_crTalkBkColor=RGB(r,g,b);
	m_RecvMessage.m_bkColor=RGB(r,g,b);
	m_RecvMessage.SetBackgroundColor(false,RGB(r,g,b));
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
		////////////////////////////////////
	////Kylin 20090117 聊天信息颜色设置
	r=f.GetKeyVal(key,"talkr",255);
	g=f.GetKeyVal(key,"talkg",255);
	b=f.GetKeyVal(key,"talkb",255);
	m_colorresource.m_crTalkTxColor=RGB(r,g,b);	
	////////////////////////////////////
	r=f.GetKeyVal(key,"senttextr",0);
	g=f.GetKeyVal(key,"senttextg",0);
	b=f.GetKeyVal(key,"senttextb",255);
	m_colorresource.m_crSentTxColor=RGB(r,g,b);	
	r=f.GetKeyVal(key,"receivedtextr",255);
	g=f.GetKeyVal(key,"receivedtextg",0);
	b=f.GetKeyVal(key,"receivedtextb",0);
	m_colorresource.m_crReceivedTxColor=RGB(r,g,b);	

	m_MessageHandle.SetColor(m_colorresource);

	//绘画界面
	CRect ClientRect;
	GetClientRect(&ClientRect);
	this->MapWindowPoints(this->GetParent(),&ClientRect);
//	DrawBackFace(&dc,0,0,ClientRect.Width(),ClientRect.Height());
	CGameImageHelper	help(&m_bk);
	this->MoveWindow(ClientRect.left,ClientRect.top,help.GetWidth(),help.GetHeight());

	m_bkBrush=CreateSolidBrush(m_bk.GetPixel(267,289));

	CString szMsg = f.GetKeyVal(key,"xtgg","欢迎您来到大恐龙游戏中心，请不要在聊天框中透露帐号和密码，切记不要相信聊天框中的任何活动或中奖信息。拒绝虚拟货币交易，加强自我防范意识。警惕上当受骗，后果自负。一切活动以官方网站首页公告为准。");
	m_MessageHandle.InsertSystemBoard(szMsg.GetBuffer(szMsg.GetLength()));

    CString strfolder(f.GetKeyVal(m_skinmgr.GetKeyVal(strSkin),"skinfolder",m_skinmgr.GetSkinPath()));
    wsprintf(path,"%sDialog\\scroll.bmp",skinfolder);
    HBITMAP hbmScorll = (HBITMAP)LoadImage(NULL, path, IMAGE_BITMAP, 0,0, LR_LOADFROMFILE);
    SkinSB_Uninit(m_SendMessage.GetSafeHwnd());
    SkinSB_Uninit(m_RecvMessage.GetSafeHwnd());
    SkinSB_Init(m_SendMessage.GetSafeHwnd(), hbmScorll);
    SkinSB_Init(m_RecvMessage.GetSafeHwnd(), hbmScorll);

	SetWindowPos(&CWnd::wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE); 

	return false;
}
//换肤
void CShortMessage::ExchangeSkin(const CString& strUserID)
{
    //设置控件
    m_RecvMessage.Init();
    m_SendMessage.SetFocus();

    //加载位图
    int r,g,b;
    CString strSkin = m_skinmgr.GetSkinBcfFileName(strUserID);
    CBcfFile f(CBcfFile::GetAppPath() + strSkin);
    CString key=TEXT("ShortMessage");
    TCHAR path[MAX_PATH] = {0};
    CString skinfolder(f.GetKeyVal(m_skinmgr.GetKeyVal(strSkin),"skinfolder",m_skinmgr.GetSkinPath()));
    CGameImage bt;
    wsprintf(path,"%sdialog\\sendmessage_bk.bmp",skinfolder);
    m_bk.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
    CGameImageHelper ImageHandle(&m_bk);
    HRGN hRgn=AFCBmpToRgn(ImageHandle,(m_bk.GetPixel(0,0)),RGB(1,1,1));
    if (hRgn!=NULL)
    {
        SetWindowRgn(hRgn,TRUE);
        DeleteObject(hRgn);
    }
    CPaintDC dc(this);
    CGameImageHelper	helper(&m_bk);
    helper.BitBlt(dc.GetSafeHdc(),0,0);
    CRect btnRc;
    wsprintf(path,"%sdialog\\log_exit_bt.bmp",skinfolder);//win_close.bmp
    m_BtClose.LoadButtonBitmap(path,false);
    m_BtClose.GetClientRect(&btnRc);
    int iCloseWidth = btnRc.Width();
    m_BtClose.MoveWindow(m_bk.GetWidth()-iCloseWidth,0,btnRc.Width(),btnRc.Height());

    wsprintf(path,"%sdialog\\top_min_bt.bmp",skinfolder);
    m_BtMin.LoadButtonBitmap(path,false);
    m_BtMin.GetClientRect(&btnRc);
    bt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
    m_BtMin.MoveWindow(m_bk.GetWidth()-btnRc.Width()-iCloseWidth,0,bt.GetWidth()/4,bt.GetHeight());
    m_BtMin.ShowWindow(SW_HIDE);

    wsprintf(path,"%sdialog\\face03.bmp",skinfolder);
    m_BtExpression.LoadButtonBitmap(path,false);//表情选择的
    bt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
    m_BtExpression.MoveWindow(9, 259, bt.GetWidth()/4,bt.GetHeight());

    wsprintf(path,"%sdialog\\close_bt.bmp",skinfolder);
    m_BtClose2.LoadButtonBitmap(path,false);
    bt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
    m_BtClose2.MoveWindow(260, 400, bt.GetWidth()/4,bt.GetHeight());

    wsprintf(path,"%sdialog\\send_bt.bmp",skinfolder);
    m_BtSend.LoadButtonBitmap(path,false);
    bt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
    m_BtSend.MoveWindow(330, 400, bt.GetWidth()/4,bt.GetHeight());

    m_RecvMessage.MoveWindow(3, 30, 394, 229);
    m_SendMessage.MoveWindow(3, 283, 394, 107);

    r=f.GetKeyVal(key,"talkbkr",0);
    g=f.GetKeyVal(key,"talkbkg",0);
    b=f.GetKeyVal(key,"talkbkb",0);
    m_colorresource.m_crTalkBkColor=RGB(r,g,b);
    m_RecvMessage.m_bkColor=RGB(r,g,b);
    m_RecvMessage.SetBackgroundColor(false,RGB(r,g,b));
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
    ////////////////////////////////////
    ////Kylin 20090117 聊天信息颜色设置
    r=f.GetKeyVal(key,"talkr",255);
    g=f.GetKeyVal(key,"talkg",255);
    b=f.GetKeyVal(key,"talkb",255);
    m_colorresource.m_crTalkTxColor=RGB(r,g,b);	
    ////////////////////////////////////
    r=f.GetKeyVal(key,"senttextr",0);
    g=f.GetKeyVal(key,"senttextg",0);
    b=f.GetKeyVal(key,"senttextb",255);
    m_colorresource.m_crSentTxColor=RGB(r,g,b);	
    r=f.GetKeyVal(key,"receivedtextr",255);
    g=f.GetKeyVal(key,"receivedtextg",0);
    b=f.GetKeyVal(key,"receivedtextb",0);
    m_colorresource.m_crReceivedTxColor=RGB(r,g,b);	
    m_MessageHandle.SetColor(m_colorresource);

    //绘画界面
    CRect ClientRect;
    GetClientRect(&ClientRect);
    this->MapWindowPoints(this->GetParent(),&ClientRect);
    CGameImageHelper	help(&m_bk);
    this->MoveWindow(ClientRect.left,ClientRect.top,help.GetWidth(),help.GetHeight());
    m_bkBrush=CreateSolidBrush(m_bk.GetPixel(267,289));

    CString szMsg = f.GetKeyVal(key,"xtgg","欢迎您来到面对面游戏中心，请不要在聊天框中透露帐号和密码，切记不要相信聊天框中的任何活动或中奖信息。拒绝虚拟货币交易，加强自我防范意识。警惕上当受骗，后果自负。一切活动以官方网站首页公告为准。");
    m_MessageHandle.InsertSystemBoard(szMsg.GetBuffer(szMsg.GetLength()));

    CString strfolder(f.GetKeyVal(m_skinmgr.GetKeyVal(strSkin),"skinfolder",m_skinmgr.GetSkinPath()));
    wsprintf(path,"%sDialog\\scroll.bmp",skinfolder);
    HBITMAP hbmScorll = (HBITMAP)LoadImage(NULL, path, IMAGE_BITMAP, 0,0, LR_LOADFROMFILE);
    SkinSB_Uninit(m_SendMessage.GetSafeHwnd());
    SkinSB_Uninit(m_RecvMessage.GetSafeHwnd());
    SkinSB_Init(m_SendMessage.GetSafeHwnd(), hbmScorll);
    SkinSB_Init(m_RecvMessage.GetSafeHwnd(), hbmScorll);

    SetWindowPos(&CWnd::wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE); 
}

//绘画函数
void CShortMessage::OnPaint()
{
	CPaintDC dc(this);

	//绘画界面
	CRect ClientRect;
	GetClientRect(&ClientRect);
//	this->MapWindowPoints(this->GetParent(),&ClientRect);
//	DrawBackFace(&dc,0,0,ClientRect.Width(),ClientRect.Height());
	CGameImageHelper	help(&m_bk);
//	this->MoveWindow(ClientRect.left,ClientRect.top,help.GetWidth(),help.GetHeight());
	help.BitBlt(dc.GetSafeHdc(),0,0);
	//绘画用户
	TCHAR szBuffer[100];
	memset(szBuffer,0,sizeof(szBuffer));
	dc.SetTextColor(RGB(255,255,255));
	dc.SetBkMode(TRANSPARENT);
	CFont font;
	font.CreateFont(-12,0,0,0,700,NULL,NULL,NULL,134,3,2,1,2,TEXT("宋体"));
	dc.SelectObject(font);
	if (m_iGmID == 0)
		wsprintf(szBuffer,"与 %s 聊天中",m_strTargerName);
	else 
		wsprintf(szBuffer,"系统消息：%s",m_strTargerName);
	CRect rect(0,10,help.GetWidth(),40);
	dc.DrawText(szBuffer,lstrlen(szBuffer),&rect,DT_CENTER);
	this->SetWindowText(szBuffer);
	return;
}

//调整位置
void CShortMessage::FixControlSize(int iWidth, int iHegiht)
{
	return;
}

//设置信息
void CShortMessage::SetTalkInfo(SendMsgUserStruct * pTargerUserItem, long int dwGamePower, TCHAR * szRoomName)
{
	m_dwGamePower = dwGamePower;
	bool bSendMunh=CUserPower::CanSendMuch(m_dwGamePower);
	m_SendMessage.LimitText(bSendMunh?MAX_TALK_LEN:NORMAL_TALK_LEN);

	//设置变量
	bool bCleanMessage=((m_dwTargetUserID!=0L)&&(m_dwTargetUserID!=pTargerUserItem->dwUserID));
	m_strRoomName=szRoomName;

	m_TargetUserItem.bDeskNO = pTargerUserItem->bDeskNO;
	m_TargetUserItem.bGameMaster = pTargerUserItem->bGameMaster;
	m_TargetUserItem.dwUserID = pTargerUserItem->dwUserID;
	m_TargetUserItem.dwAccID = pTargerUserItem->dwAccID;
	strcpy(m_TargetUserItem.nickName , pTargerUserItem->nickName);
	m_strTargerName=m_TargetUserItem.nickName;
	m_dwTargerAccID=m_TargetUserItem.dwAccID;
	m_dwTargetUserID=m_TargetUserItem.dwUserID;

	//设置控件
	if (GetSafeHwnd()!=NULL)
	{
		if (bCleanMessage==true)
		{
			m_RecvMessage.OnClearAll();
			m_SendMessage.SetWindowText(TEXT(""));
		}
		m_SendMessage.SetFocus();
		Invalidate(FALSE);
	}
	m_iGmID = 0;
	return;
}

void CShortMessage::SetTalkInfo(int iGmID)
{
	m_iGmID = iGmID;
	m_strTargerName.Format("管理员%d", m_iGmID);
}

//鼠标右键按下消息
void CShortMessage::OnLButtonDown(UINT nFlags, CPoint point)
{
	//模拟按标题
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
	return;
}

//确定按钮
void CShortMessage::OnOK()
{
	OnBnClickedSendShort();
}

//退出按钮
void CShortMessage::OnBnClickedClose()
{
    m_MsgRecordMgr.CloseFile();
	if (m_iGmID != 0) m_iGmID = 0;
	DestroyWindow();
	return;
}

//退出按钮
void CShortMessage::OnBnClickedMin()
{
	OnSize(SIZE_MINIMIZED, 0, 0);
	return;
}


//取消按钮
void CShortMessage::OnCancel()
{
	ShowWindow(SW_HIDE);
	return;
}

//历史记录
void CShortMessage::OnBnClickedHistory()
{
	CString strBuffer;
	strBuffer.Format(TEXT("%s\\%s.TXT"),"Talk",m_strTargerName);
	ShellExecute(NULL,TEXT("open"),TEXT("Notepad.exe"),strBuffer,NULL,SW_SHOW);
	return;
}

//发送按钮
void CShortMessage::OnBnClickedSendShort()
{
	if (m_iGmID != 0)
	{
		//过滤消息
		CString strBuffer;
		m_SendMessage.GetWindowText(strBuffer);
		if (strBuffer.Find(TEXT("{\\rtf"))!=-1) throw TEXT("发送失败！");
		for (int i=strBuffer.GetLength();i>0;i--)
			if ((strBuffer[i-1]!=TEXT('\n'))&&(strBuffer[i-1]!=TEXT('\r'))&&(strBuffer[i-1]>=-1)&&(strBuffer[i-1]<30)) throw TEXT("不能发送非法字符，发送失败！");

		m_strLastTalk = strBuffer;
		m_pParentWnd->PostMessage(WM_GM_TALK, WPARAM(&m_strLastTalk), LPARAM(&m_iGmID));
		m_SendMessage.SetWindowText(TEXT(""));
	}
	else
	{
		
		//判断信息
		m_SendMessage.SetFocus();
		if (m_SendMessage.GetWindowTextLength()==0)	return;
		try
		{
			//获取时间
			long int dwNowTime=(long int)time(NULL);
			//判断目标对象
			ASSERT(m_dwTargetUserID!=0L);
			if (m_dwTargetUserID!=m_TargetUserItem.dwUserID)
			{
				//Modify By Fred Huang 2008-06-17
				bool ret = false;
				if(m_bIsInRoom)
					ret = ((CGameRoomEx *)m_pParentWnd)->GetSendMsgUserStruct(m_dwTargetUserID, &m_TargetUserItem);
				else
					ret = ((CIMMain *)m_pParentWnd)->GetSendMsgUserStruct(m_dwTargetUserID, &m_TargetUserItem);

				if (!ret) 
				{
					m_MessageHandle.InsertSystemMessage(TEXT("信息发送失败，对方可能已离开！"));
					return;
				}
			}

			//获取消息
			CString strBuffer;
			m_SendMessage.GetWindowText(strBuffer);

			//是否可以发短信
			if (CUserPower::CanSendMessage(m_dwGamePower)==false)
			{
				m_MessageHandle.InsertSystemMessage(TEXT("信息发送失败！"));
				return;
			}

			//判断是否在同一游戏桌,
			if(m_bIsInRoom)
			{
				if ((m_TargetUserItem.bDeskNO!=255)&&(m_TargetUserItem.bDeskNO==
					((CGameRoomEx *)m_pParentWnd)->m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO)&&
					(((CGameRoomEx *)m_pParentWnd)->m_GameInfo.pMeUserInfo->GameUserInfo.bGameMaster!=6&&m_TargetUserItem.bGameMaster!=6))//上帝可以
				{
					m_MessageHandle.InsertSystemMessage(TEXT("为防止做弊，不能和同桌玩家发短信息！"));
					return;
				}
			}

			//判断信息长度
			int iMaxSendLen=CUserPower::CanSendMuch(m_dwGamePower)?MAX_TALK_LEN:NORMAL_TALK_LEN;
			if (strBuffer.GetLength()>iMaxSendLen) throw TEXT("发送的信息太长，请缩减信息或分多次发送！");

			//过滤消息
			if (strBuffer.Find(TEXT("{\\rtf"))!=-1) throw TEXT("发送失败！");
			for (int i=strBuffer.GetLength();i>0;i--)
				if ((strBuffer[i-1]!=TEXT('\n'))&&(strBuffer[i-1]!=TEXT('\r'))&&(strBuffer[i-1]>=-1)&&(strBuffer[i-1]<30)) throw TEXT("不能发送非法字符，发送失败！");

			//判断速度
			if (CUserPower::CanTalkFast(m_dwGamePower)==false)
			{
				long int dwNowTime=(long int)time(NULL);
				if ((m_dwLastTalkTime+1L)>dwNowTime) throw TEXT("请不要频繁发送短信息！");
				if (((m_dwLastTalkTime+3L)>dwNowTime)&&(m_strLastTalk.Compare(strBuffer)==0))
					throw TEXT("请不要重复发言！");
			}


			//////////////////////////////////////////////////////////////////////////
			//扩展机器人发消息
			//20081211 , Fred Huang
			if(m_dwTargetUserID<0)
			{
				m_MessageHandle.InsertUserTalk(((CGameRoomEx *)m_pParentWnd)->m_pLogonInfo->nickName,NULL,
					strBuffer.GetBuffer(strBuffer.GetLength()),
					m_colorresource.m_crNormalTxColor,true);
				m_strLastTalk=strBuffer;
				m_dwLastTalkTime=(long int)time(NULL);
				m_SendMessage.SetWindowText(TEXT(""));
				::SendMessage(m_RecvMessage.m_hWnd,WM_VSCROLL,SB_PAGEDOWN,NULL);
				return;
			}
			//////////////////////////////////////////////////////////////////////////


			//发送聊天信息
			if(m_bIsInRoom)
			{
				MSG_GR_RS_NormalTalk Talk;
				ZeroMemory(&Talk,sizeof(MSG_GR_RS_NormalTalk));
				Talk.iLength=strBuffer.GetLength();
				Talk.dwSendID=((CGameRoomEx *)m_pParentWnd)->m_pLogonInfo->dwUserID;
				Talk.dwTargetID=m_dwTargetUserID;
				lstrcpy(Talk.szMessage,strBuffer);
				Talk.crColor=m_colorresource.m_crTalkTxColor;
				((CGameRoomEx *)m_pParentWnd)->SendData(&Talk,sizeof(Talk)-sizeof(Talk.szMessage)+Talk.iLength+1,MDM_GR_ROOM,ASS_GR_SHORT_MSG,0);

				m_strLastTalk=Talk.szMessage;
			}
			else
			{
				MSG_IM_C_MSG msg;
				ZeroMemory(&msg,sizeof(msg));
				msg.dwUserID=((CIMMain*)m_pParentWnd)->GetMeUserInfo()->dwUserID;
				memcpy(&msg.szUserName,((CIMMain*)m_pParentWnd)->GetMeUserInfo()->nickName ,sizeof(((CIMMain*)m_pParentWnd)->GetMeUserInfo()->nickName));
				msg.dwRemoteUserID=m_dwTargetUserID;
				msg.szMsgLength=strBuffer.GetLength();
				memcpy(&msg.szMsg,strBuffer,strBuffer.GetLength());
				msg.cbSize=sizeof(MSG_IM_C_MSG)-sizeof(msg.szMsg)+msg.szMsgLength+1;
				((CIMMain *)m_pParentWnd)->SendData(&msg,msg.cbSize,MDM_GP_IM,ASS_IMC_MSG,0);

				m_MessageHandle.InsertUserTalk(((CIMMain*)m_pParentWnd)->GetMeUserInfo()->nickName, NULL, msg.szMsg,m_colorresource.m_crSentTxColor,true);
				m_strLastTalk=msg.szMsg;


                RecordTalkMsg(true,&msg);
			}

			//设置参数
			m_dwLastTalkTime=(long int)time(NULL);
			m_SendMessage.SetWindowText(TEXT(""));
		}
		catch (TCHAR * szErrorMessage)
		{ 
			m_MessageHandle.InsertSystemMessage(szErrorMessage); 
		}
		
		catch (...) 
		{
			m_MessageHandle.InsertSystemMessage(TEXT("消息发送失败！")); 
		}
	}
	return;
}
/*************************************************
Function      :  RecordTalkMsg
Description   :  记录聊天内容
Input         :  bool bSend
Output        :  无
Return        :  void
Author        :  任德
Date          :  2012-02-01
*************************************************/
void CShortMessage::RecordTalkMsg(const bool bSend,const MSG_IM_S_MSG* pMsg)
{
    CString strSystime("");
    CString strChatMsg("");

    SYSTEMTIME SystemTime;
    GetLocalTime(&SystemTime);
    strSystime.Format(TEXT("%02d:%02d:%02d"),SystemTime.wHour,SystemTime.wMinute,SystemTime.wSecond);

    m_MsgRecordMgr.m_bSendMsg = bSend;

    if (bSend)
    {
        m_MsgRecordMgr.m_nOwnerID = pMsg->dwUserID;
        m_MsgRecordMgr.m_strOwnerName = pMsg->szUserName;
        m_MsgRecordMgr.m_nChaterID = pMsg->dwRemoteUserID;
        m_MsgRecordMgr.m_strChaterName = m_strTargerName;
        m_SendMessage.GetWindowText(strChatMsg);
    }
    else
    {
        m_MsgRecordMgr.m_nOwnerID = pMsg->dwRemoteUserID;
        //m_MsgRecordMgr.m_strOwnerName = ;
        m_MsgRecordMgr.m_nChaterID = pMsg->dwUserID;
        m_MsgRecordMgr.m_strChaterName = pMsg->szUserName;
        strChatMsg.Format("%s",pMsg->szMsg);
    }
    m_MsgRecordMgr.m_nMsgLen = strlen(strChatMsg);
    m_MsgRecordMgr.m_strMsg = strChatMsg;
    m_MsgRecordMgr.m_strChatTime = strSystime;
    strSystime.Format(TEXT("%02d-%02d-%02d"),SystemTime.wYear,SystemTime.wMonth,SystemTime.wDay);
    m_MsgRecordMgr.m_strChatDay = strSystime;
    m_MsgRecordMgr.Init();
    if (!m_MsgRecordMgr.WriteOneLineMsg())
    {
        DUIOkeyMsgBox(GetSafeHwnd(),_T("记录消息文件失败!"));
    }
}
//消息解释
BOOL CShortMessage::PreTranslateMessage(MSG * pMsg)
{
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_RETURN))
	{
		if ((::GetAsyncKeyState(VK_LCONTROL)&0xFFF0)||(::GetAsyncKeyState(VK_RCONTROL)&0xFFF0))
		{
			OnBnClickedSendShort();
			return TRUE;
		}
	}
	m_ToolTip.RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

//收到消息
void CShortMessage::OnRecvMessage(TCHAR * szSendName, TCHAR * szMessage, COLORREF crColor)
{
	BZSoundPlay(this, "music/聊天消息提示声.mp3", 0);
	m_MessageHandle.InsertUserTalk(szSendName,NULL,szMessage,crColor,true);
	::SendMessage(m_RecvMessage.m_hWnd,WM_VSCROLL,SB_PAGEDOWN,NULL);
	this->SetForegroundWindow();

	return;
}

//位置消息
void CShortMessage::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	//移动控件
	FixControlSize(cx,cy);

	return;
}

//获取位置
void CShortMessage::OnGetMinMaxInfo(MINMAXINFO * lpMMI)
{
	lpMMI->ptMinTrackSize.x=300;
	lpMMI->ptMinTrackSize.y=250;
	return;
}

//头像消息
LRESULT CShortMessage::OnHitLogo(WPARAM wparam, LPARAM lparam)
{
	return 0;
}

//表情消息
LRESULT CShortMessage::OnExpression(WPARAM wparam, LPARAM lparam)
{
	CTrueItem * pExpItem=m_ExpressWnd.GetExpressItem((UINT)wparam);
	if (pExpItem!=NULL)
	{
		CString strBuffer;
		m_SendMessage.GetWindowText(strBuffer);
		strBuffer+=pExpItem->m_szTrancelate;
		m_SendMessage.SetWindowText(strBuffer);
		m_SendMessage.SetFocus();
		m_SendMessage.SetSel(strBuffer.GetLength(),strBuffer.GetLength());	
	}
	return 0;
}
void CShortMessage::OnBnClickedRecordChat()
{
    vector<CString>vecMsgRecord;
    m_MsgRecordMgr.m_bSendMsg = true;
	if(m_bIsInRoom)
	{
		m_MsgRecordMgr.m_nOwnerID = ((CGameRoomEx*)m_pParentWnd)->GetMeUserInfo()->dwUserID;
		m_MsgRecordMgr.m_strOwnerName = ((CGameRoomEx*)m_pParentWnd)->GetMeUserInfo()->nickName;
	}
	else
	{
		m_MsgRecordMgr.m_nOwnerID = ((CIMMain*)m_pParentWnd)->GetMeUserInfo()->dwUserID;
		m_MsgRecordMgr.m_strOwnerName = ((CIMMain*)m_pParentWnd)->GetMeUserInfo()->nickName;
	}
    m_MsgRecordMgr.m_nChaterID = m_dwTargetUserID;
    m_MsgRecordMgr.m_strChaterName = m_strTargerName;

    m_MsgRecordMgr.Init();
    if (m_MsgRecordMgr.ReadMsg(vecMsgRecord))
    {
        vector<CString>::iterator vecChatIter = vecMsgRecord.begin();
        while (vecChatIter != vecMsgRecord.end())
        {
            m_MessageHandle.InsertChatRecord((*vecChatIter).GetBuffer(),m_colorresource.m_crSentTxColor,false);
            vecChatIter++;
        }
    }
}
//表情按钮
void CShortMessage::OnBnClickedExpression()
{
	//建立表情窗口
	if (m_ExpressWnd.GetSafeHwnd()==NULL) m_ExpressWnd.CreateExpressionWnd(this);

	//移动窗口
	CRect ButtonRect;
	CSize ExpressSize;
	m_ExpressWnd.GetFixSize(ExpressSize);
	m_BtExpression.GetWindowRect(&ButtonRect);
	SafeMoveWindow(&m_ExpressWnd,ButtonRect.left,ButtonRect.top-ExpressSize.cy,ExpressSize.cx,ExpressSize.cy);
	m_ExpressWnd.ShowWindow(SW_SHOW);
	m_ExpressWnd.SetFocus();
	return;
}

void CShortMessage::OnDestroy()
{
	CGameFaceGo::OnDestroy();
	//保存聊天记录
	CString strBuffer;

	CString str;
	char Temp[MAX_PATH];
	str.Format("talk\\%s.txt",m_strTargerName);

	bool b=false;
	CFile file;
	if(file.Open(str,CFile::modeWrite))
	{
		b=true;
	}
	else
	{
		if(file.Open(str,CFile::modeCreate|CFile::modeWrite))
		{
			b=true;
		}
	}

	if(b)
	{
		file.SeekToEnd();
		for(int i=0;i<m_RecvMessage.GetLineCount();i++)
		{
			memset(Temp,0,sizeof(Temp));
			m_RecvMessage.GetLine(i,Temp,MAX_PATH);
			if(strlen(Temp)>0)
				file.Write(Temp,strlen(Temp)-1);
			file.Write("\r\n",strlen("\r\n"));
		}

		file.Close();
	}


}

LRESULT CShortMessage::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		//case WM_CTLCOLORSCROLLBAR :
		//case WM_CTLCOLORBTN:
		//case WM_CTLCOLORMSGBOX:
	//case WM_CTLCOLOREDIT:
		//case WM_CTLCOLORLISTBOX:
		//case WM_CTLCOLORDLG:
	case WM_CTLCOLORSTATIC :
		SetTextColor((HDC)wParam, RGB(0,0,0)) ;
		SetBkColor((HDC)wParam, m_bk.GetPixel(267,289));
		return (LRESULT)m_bkBrush;
	}
	return CGameFaceGo::WindowProc(message, wParam, lParam);
}

void CShortMessage::TransUserInfo(UserItemStruct *pIn, SendMsgUserStruct *pOut)
{
	pOut->bDeskNO = pIn->GameUserInfo.bDeskNO;
	pOut->bGameMaster = pIn->GameUserInfo.bGameMaster;
	pOut->dwUserID = pIn->GameUserInfo.dwUserID;
	pOut->dwAccID = pIn->GameUserInfo.dwAccID;
	strcpy(pOut->nickName , pIn->GameUserInfo.nickName);
}