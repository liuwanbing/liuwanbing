se//聊天控制
#include "StdAfx.h"
#include "Resource.h"
#include "ChatControl.h"
#include "GameRoomEx.h"
#include "MainRoomEx.h"

//宏定义
//#define PIC_BT_WIDE						20							//位图按钮宽度
#define PIC_BT_WIDE							20							//位图按钮宽度
#define PIC_BT_HIGHT						20							//位图按钮高度
#define CONTROL_SPACE						2							//控件间距

//控件 ID
#define IDC_INPUT							10							//输入信息	
#define IDC_EXPRES							11							//表情按钮
#define IDC_ACTION							12							//动作按钮
#define IDC_ATTRIB							13							//属性按钮
#define IDC_SEND							14							//发送按钮

#define IDC_BOARDCAST						15							//喇叭按键

BEGIN_MESSAGE_MAP(CChatControl, CWnd)/////////消息函数映射 
	ON_WM_SIZE()////////消息响应函数系统 都有,可以自己改写
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_SEND, OnBnClickedSend)
	ON_BN_CLICKED(IDC_EXPRES, OnBnClickedExpression)
	ON_BN_CLICKED(IDC_BOARDCAST, OnBnClickedBoardCast)
	ON_MESSAGE(WM_EXCHANGE_SKIN,OnExchangeSkin)
END_MESSAGE_MAP()
//构造函数
CChatControl::CChatControl()
{
	m_BoardCastWnd = NULL;
}
//析构函数
CChatControl::~CChatControl()
{
	m_exp.Detach();
	m_send.Detach();
	m_boardcast.Detach();
}
//建立消息
int CChatControl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (__super::OnCreate(lpCreateStruct) == -1) return -1;
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	//设置界面
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);
	//建立控件
	m_InputMessage.Create(CBS_DROPDOWN|CBS_AUTOHSCROLL|WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IDC_INPUT);//聊天输入框 下拉列表
	m_BtSend.Create(TEXT(""),WS_CHILD|WS_VISIBLE|BS_OWNERDRAW,CRect(0,0,0,0),this,IDC_SEND);//发送的按钮
	m_BtExpression.Create(NULL,WS_CHILD|WS_VISIBLE|BS_OWNERDRAW,CRect(0,0,0,0),this,IDC_EXPRES);//表情选择的图形按钮

	m_BtBoardCast.Create(NULL,WS_CHILD|WS_VISIBLE|BS_OWNERDRAW,CRect(0,0,0,0),this, IDC_BOARDCAST);//表情选择的图形按钮
	

	//建立提示控件
	m_ToolTip.Create(this);
	m_ToolTip.Activate(TRUE);
	m_ToolTip.AddTool(GetDlgItem(IDC_SEND),fMsg.GetKeyVal("MainRoom","SendChatMsg","发送聊天信息"));
	m_ToolTip.AddTool(GetDlgItem(IDC_EXPRES),fMsg.GetKeyVal("MainRoom","SelectEmotionHead","选择表情头像"));
	//设置背景图
	m_InputMessage.SetFont(&CGameImageLink::m_HFont);//聊天输入框
	LoadSkin();
	return 0;
}
//
void CChatControl::LoadSkin()
{
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + strSkin);
	CString key= TEXT("ChatControl");
	CString skinfolder =f.GetKeyVal(m_skinmgr.GetKeyVal(strSkin),"skinfolder",m_skinmgr.GetSkinPath());;

	//表情窗口
	m_iExpX = f.GetKeyVal(key,"ExpX",0);
	m_iExpY = f.GetKeyVal(key,"ExpY",0);
	//发送窗口
	m_iSendX = f.GetKeyVal(key,"sendX",0);
	m_iSendY = f.GetKeyVal(key,"sendY",0);
}
//换肤
LRESULT CChatControl::OnExchangeSkin(WPARAM wpara,LPARAM lpara)
{
	LoadSkin();
	Invalidate();
	return LRESULT(0);
}

//位置变化
void CChatControl::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	//喇叭
	if (m_BtBoardCast.GetSafeHwnd() != NULL)
	{
		m_BtBoardCast.MoveWindow(0, (cy - m_boardcast.GetHeight()) / 2, m_boardcast.GetWidth() / 4, m_boardcast.GetHeight());
	}
	//移动控件
	if (m_BtExpression.GetSafeHwnd()!=NULL)
	{
		m_BtExpression.MoveWindow(m_boardcast.GetWidth() / 4,(cy-m_exp.GetHeight())/2,m_exp.GetWidth()/4,m_exp.GetHeight());
	}
	if (m_InputMessage.GetSafeHwnd()!=NULL) 
	{
		CRect rect;
		m_InputMessage.GetClientRect(&rect);
		m_InputMessage.MoveWindow((m_boardcast.GetWidth() + m_exp.GetWidth())/4, (cy-20)/2, cx-(m_boardcast.GetWidth() + m_exp.GetWidth())/4-m_send.GetWidth()/4,200);
	}
	if (m_BtSend.GetSafeHwnd()!=NULL)
	{
		m_BtSend.MoveWindow(cx-m_send.GetWidth()/4,(cy-m_send.GetHeight())/2,m_send.GetWidth()/4,m_send.GetHeight());
	}

	return;
}
//重画函数
void CChatControl::OnPaint()
{
	CPaintDC dc(this);
	//绘画背景
	CRect ClientRect;
	GetClientRect(&ClientRect);

	//dc.FillSolidRect(&ClientRect,m_bkColor);
	//CGameImage						temp;
	//temp.SetLoadInfo("image\\client\\skin0\\room_right\\room_talkbc.bmp",CGameImageLink::m_bAutoLock);
	//CGameImageHelper	help(&temp);
	//help.BitBlt(dc.GetSafeHdc(),0,0);
	//CRect ClientRect;
	//GetClientRect(&ClientRect);
	//CGameImageHelper handle(&m_bkImg);
	//for(int i=0;i<ClientRect.Width();i+=handle.GetWidth())
	//	handle.BitBlt(dc.GetSafeHdc(),i,0);
	
	return;
}
//发送按钮
void CChatControl::OnBnClickedSend()
{
	GetParent()->SendMessage(IDM_SEND_ROOMMSG,1,1);
//	CGameRoomEx * pGameRoom=(CGameRoomEx *)GetParent();
//	pGameRoom->OnSendRoomTalk();
	return;
}
//表情按钮
void CChatControl::OnBnClickedExpression()
{
	GetParent()->SendMessage(IDM_SEND_ROOMMSG,2,2);

//	CGameRoomEx * pGameRoom=(CGameRoomEx *)GetParent();
//	pGameRoom->OnBnClickedExpression();
	return;
}
//消息解释
BOOL CChatControl::PreTranslateMessage(MSG * pMsg)
{
	m_ToolTip.RelayEvent(pMsg);
	return CWnd::PreTranslateMessage(pMsg);
}
void CChatControl::SetBkColor(COLORREF color)
{
	m_bkColor=color;
}

//喇叭按钮
void CChatControl::OnBnClickedBoardCast()
{
	if (GetMainRoom()->m_TopDuWnd != NULL)
		GetMainRoom()->m_TopDuWnd->SetMaxOrRestoreBtn(false);

	m_BoardCastWnd = new CUseBoardCastWnd();
	m_BoardCastWnd->Create(m_hWnd,NULL,UI_WNDSTYLE_DIALOG,0,0,0,0,0,NULL);
	m_BoardCastWnd->SetParentWnd(m_pGameRoom);
	m_BoardCastWnd->CenterWindow();
	m_BoardCastWnd->ShowModal();
	if (m_BoardCastWnd != NULL)
		delete m_BoardCastWnd;

	if (GetMainRoom()->m_TopDuWnd != NULL)
		GetMainRoom()->m_TopDuWnd->SetMaxOrRestoreBtn(true);
}

void CChatControl::UseProp_Buy(int nPropIndex)
{
	if (m_BoardCastWnd != NULL)
	{
		m_BoardCastWnd->UseProp_Buy(nPropIndex);
	}
}

void CChatControl::SetImage(TCHAR path1[],TCHAR path2[], TCHAR path3[])
{
	m_exp.SetLoadInfo(path1,CGameImageLink::m_bAutoLock);
	m_send.SetLoadInfo(path2,CGameImageLink::m_bAutoLock);

	CString s=CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f(s + m_skinmgr.GetSkinBcfFileName());
	bool _use = f.GetKeyVal("BoardCast","TalkUseBoardCast", 0);
	if (_use)
		m_boardcast.SetLoadInfo(path3, CGameImageLink::m_bAutoLock);
	else
		m_boardcast.SetLoadInfo("", CGameImageLink::m_bAutoLock);

	m_BtExpression.LoadButtonBitmap(path1,false);
	m_BtSend.LoadButtonBitmap(path2,false);
	m_BtBoardCast.LoadButtonBitmap(path3, false);
}

void CChatControl::SetImage(TCHAR bkimg[],TCHAR path1[],TCHAR path2[], TCHAR path3[])
{
	m_bkImg.SetLoadInfo(bkimg,CGameImageLink::m_bAutoLock);
	m_exp.SetLoadInfo(path1,CGameImageLink::m_bAutoLock);
	m_send.SetLoadInfo(path2,CGameImageLink::m_bAutoLock);
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f(s + m_skinmgr.GetSkinBcfFileName());
	bool _use = f.GetKeyVal("BoardCast","TalkUseBoardCast", 0);
	if (_use)
		m_boardcast.SetLoadInfo(path3, CGameImageLink::m_bAutoLock);
	else
		m_boardcast.SetLoadInfo("", CGameImageLink::m_bAutoLock);

	m_BtExpression.LoadButtonBitmap(path1,false);
	m_BtSend.LoadButtonBitmap(path2,false);
	m_BtBoardCast.LoadButtonBitmap(path3, false);
}

//加载聊天
void CChatControl::ReLoadTalkINI()
{
	m_InputMessage.ResetContent();
	//从talk.ini文件中读取聊天条
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f( s + "Talk.bcf");
	CString key="BZW_TALK";
//	TCHAR path[MAX_PATH];
	CString szTalk;
	CString szNumber;
	int i=1;
	do
	{
		szNumber.Format("%d",i++);
		szTalk=f.GetKeyVal(key,"T"+szNumber,"");
		if(!szTalk.IsEmpty())
			m_InputMessage.AddString(szTalk);
	}while(!szTalk.IsEmpty());
}


void CChatControl::SetGameRoomWnd(CWnd* pWnd)
{
	m_pGameRoom = pWnd;
}