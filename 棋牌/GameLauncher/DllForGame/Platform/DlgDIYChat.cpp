// DlgDIYChat.cpp : 实现文件
//

#include "../stdafx.h"
#include "DlgDIYChat.h"
//#include "gameFrameDlg.h"
#include "AFCFunction.h"


/// 用来确保读取到文件的宏，如果在游戏目录下读取不到，则在大厅目录下读取文件
/// 调用前须事先准备好skinHall，因为文件有时在skin0\下，有时在skin0\game\下
#ifndef GET_FILENAME
#define GET_FILENAME(path, FormatStr,skinfolder) \
	wsprintf(path,FormatStr,skinfolder);\
	if (!CBcfFile::IsFileExist(path))\
	{\
	wsprintf(path,FormatStr,skinHall);\
	}
#endif//GET_FILENAME


// CDlgDIYChat 对话框

IMPLEMENT_DYNAMIC(CDlgDIYChat, CDialog)

CDlgDIYChat::CDlgDIYChat(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIY_CHAT, pParent)
	, m_EditSzBuffer(_T(""))
	, m_GameOrHall(0)
{
	m_bkBrush=NULL;
}

CDlgDIYChat::~CDlgDIYChat()
{
	DeleteObject(m_bkBrush);
}

void CDlgDIYChat::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_EditSzBuffer);
	DDV_MaxChars(pDX, m_EditSzBuffer, 60);
	DDX_Control(pDX, IDCANCEL, m_btcancel);
	DDX_Control(pDX, IDOK, m_btOk);
	DDX_Control(pDX, IDC_MODIFY, m_btModify);
	DDX_Control(pDX, IDC_ADD, m_btAdd);
	DDX_Control(pDX, IDC_DELETE, m_btDelete);
}
BOOL CDlgDIYChat::OnInitDialog()
{
	__super::OnInitDialog();

	m_listBox=(CListBox*)GetDlgItem(IDC_LIST_CHAT);
	ReadTalkINI();

	//贴图
	//CString s="..\\";//CINIFile::GetAppPath ();/////本地路径
	CString strBcf = "";
	CString strAppPath = CBcfFile::GetAppPath();
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CString strGameRoomSkin = m_skinmgr.GetGameSkinBcfFileName();
	CString strBcfPath = CBcfFile::GetAppPath() + "Image\\game\\" + strGameRoomSkin;

	if (CBcfFile::IsFileExist(strBcfPath))
		strBcf = strBcfPath;
	else
		strBcf = strAppPath+"..\\image\\Client\\"+ m_skinmgr.GetKeyVal(strSkin) + "\\game\\" + strGameRoomSkin;
	
	CBcfFile f( strBcf );
	TCHAR path[MAX_PATH];
	CString skinfolder=f.GetKeyVal(m_skinmgr.GetKeyVal(strSkin),"skinfolder",m_skinmgr.GetSkinPath());
	CString skinHall = strAppPath + "..\\" + m_skinmgr.GetSkinPath();
	CGameImage	m_bt;

	GET_FILENAME(path,"%sdialog\\DIYChat.bmp",skinfolder);
	m_bkimage.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	CGameImageHelper ImageHandle(&m_bkimage);
	int cx,cy;
	HRGN hRgn=AFCBmpToRgn(ImageHandle,m_bkimage.GetPixel(0,0),RGB(1,1,1));
	if (hRgn!=NULL)
	{
		SetWindowRgn(hRgn,TRUE);
	}
	cx=ImageHandle.GetWidth();
	cy=ImageHandle.GetHeight();
	SetWindowPos(NULL,0,0,cx,cy,SWP_NOMOVE|SWP_NOZORDER);

	//取消
	GET_FILENAME(path,"%sdialog\\cancel_bt.BMP",skinfolder);
	m_bt.SetLoadInfo(path,true);
	m_btcancel.LoadButtonBitmap(path,false);
	m_btcancel.SetWindowPos(NULL,0,0,m_bt.GetWidth()/4,m_bt.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	//ok
	GET_FILENAME(path,"%sdialog\\ok_bt.bmp",skinfolder);
	m_bt.SetLoadInfo(path,true);
	m_btOk.LoadButtonBitmap(path,false);
	m_btOk.SetWindowPos(NULL,0,0,m_bt.GetWidth()/4,m_bt.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	//增加
	GET_FILENAME(path,"%sdialog\\Add_bt.bmp",skinfolder);
	m_bt.SetLoadInfo(path,true);
	m_btAdd.LoadButtonBitmap(path,false);
	m_btAdd.SetWindowPos(NULL,0,0,m_bt.GetWidth()/4,m_bt.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);
	
	//删除
	GET_FILENAME(path,"%sdialog\\Delete_bt.bmp",skinfolder);
	m_bt.SetLoadInfo(path,true);
	m_btDelete.LoadButtonBitmap(path,false);
	m_btDelete.SetWindowPos(NULL,0,0,m_bt.GetWidth()/4,m_bt.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);
	
	//修改
	GET_FILENAME(path,"%sdialog\\Modify_bt.bmp",skinfolder);
	m_bt.SetLoadInfo(path,true);
	m_btModify.LoadButtonBitmap(path,false);
	m_btModify.SetWindowPos(NULL,0,0,m_bt.GetWidth()/4,m_bt.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);


	m_bkBrush=CreateSolidBrush(m_bkimage.GetPixel(32,55));

	return true;

}
void CDlgDIYChat::LoadSkin()
{
	m_listBox=(CListBox*)GetDlgItem(IDC_LIST_CHAT);
	//ReadTalkINI();

	//贴图
	//CString s="..\\";//CINIFile::GetAppPath ();/////本地路径
	CString strBcf = "";
	CString strAppPath = CBcfFile::GetAppPath();
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CString strGameRoomSkin = m_skinmgr.GetGameSkinBcfFileName();
	CString strBcfPath = CBcfFile::GetAppPath() + "Image\\game\\" + strGameRoomSkin;

	if (CBcfFile::IsFileExist(strBcfPath))
		strBcf = strBcfPath;
	else
		strBcf = strAppPath+"..\\image\\Client\\"+ m_skinmgr.GetKeyVal(strSkin) + "\\game\\" + strGameRoomSkin;

	CBcfFile f( strBcf );
	TCHAR path[MAX_PATH];
	CString skinfolder=f.GetKeyVal(m_skinmgr.GetKeyVal(strSkin),"skinfolder",m_skinmgr.GetSkinPath());
	CString skinHall = strAppPath + "..\\" + m_skinmgr.GetSkinPath();
	CGameImage	m_bt;

	GET_FILENAME(path,"%sdialog\\DIYChat.bmp",skinfolder);
	m_bkimage.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	CGameImageHelper ImageHandle(&m_bkimage);
	int cx,cy;
	HRGN hRgn=AFCBmpToRgn(ImageHandle,m_bkimage.GetPixel(0,0),RGB(1,1,1));
	if (hRgn!=NULL)
	{
		SetWindowRgn(hRgn,TRUE);
	}
	//cx=ImageHandle.GetWidth();
	//cy=ImageHandle.GetHeight();
	//SetWindowPos(NULL,0,0,cx,cy,SWP_NOMOVE|SWP_NOZORDER);

	//取消
	GET_FILENAME(path,"%sdialog\\cancel_bt.BMP",skinfolder);
	m_bt.SetLoadInfo(path,true);
	m_btcancel.LoadButtonBitmap(path,false);
	//m_btcancel.SetWindowPos(NULL,0,0,m_bt.GetWidth()/4,m_bt.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	//ok
	GET_FILENAME(path,"%sdialog\\ok_bt.bmp",skinfolder);
	m_bt.SetLoadInfo(path,true);
	m_btOk.LoadButtonBitmap(path,false);
	//m_btOk.SetWindowPos(NULL,0,0,m_bt.GetWidth()/4,m_bt.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	//增加
	GET_FILENAME(path,"%sdialog\\Add_bt.bmp",skinfolder);
	m_bt.SetLoadInfo(path,true);
	m_btAdd.LoadButtonBitmap(path,false);
	//m_btAdd.SetWindowPos(NULL,0,0,m_bt.GetWidth()/4,m_bt.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	//删除
	GET_FILENAME(path,"%sdialog\\Delete_bt.bmp",skinfolder);
	m_bt.SetLoadInfo(path,true);
	m_btDelete.LoadButtonBitmap(path,false);
	//m_btDelete.SetWindowPos(NULL,0,0,m_bt.GetWidth()/4,m_bt.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	//修改
	GET_FILENAME(path,"%sdialog\\Modify_bt.bmp",skinfolder);
	m_bt.SetLoadInfo(path,true);
	m_btModify.LoadButtonBitmap(path,false);
	//m_btModify.SetWindowPos(NULL,0,0,m_bt.GetWidth()/4,m_bt.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);
}

void CDlgDIYChat::OnPaint()
{
	CPaintDC dc(this);
	CGameImageHelper	handle(&m_bkimage);
	handle.BitBlt(dc.GetSafeHdc(),0,0);

	CFont Font;//,*OldFont;
	Font.CreateFont(15,10,0,0,0,0,0,0,GB2312_CHARSET
	,3,2,1,2,TEXT("宋体"));
	CWnd *pWnd=GetDlgItem(IDC_LIST_CHAT);
	CDC *pDC=pWnd->GetDC();
	pDC->SelectObject(&Font);
	pWnd->SetFont(&Font,true);
//	DeleteObject(pDC->SelectObject(OldFont));
	ReleaseDC(pDC);

	pWnd=GetDlgItem(IDC_EDIT1);
	pDC=pWnd->GetDC();
	pDC->SelectObject(&Font);
	pWnd->SetFont(&Font,true);
//	DeleteObject(pDC->SelectObject(OldFont));
	ReleaseDC(pDC);
	Font.DeleteObject();
}

LRESULT CDlgDIYChat::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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
		SetBkColor((HDC)wParam, m_bkimage.GetPixel(32,55));
		return (LRESULT)m_bkBrush;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

BEGIN_MESSAGE_MAP(CDlgDIYChat, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgDIYChat::OnBnClickedOk)
	ON_BN_CLICKED(IDC_MODIFY, &CDlgDIYChat::OnBnClickedModify)
	ON_BN_CLICKED(IDC_DELETE, &CDlgDIYChat::OnBnClickedDelete)
	ON_BN_CLICKED(IDC_ADD, &CDlgDIYChat::OnBnClickedAdd)
	ON_LBN_DBLCLK(IDC_LIST_CHAT, &CDlgDIYChat::OnLbnDblclkListChat)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// CDlgDIYChat 消息处理程序

void CDlgDIYChat::OnBnClickedOk()
{
	WriteTalkINI();
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
}

void CDlgDIYChat::OnBnClickedModify()
{
	// TODO: 在此添加控件通知处理程序代码
	int number=m_listBox->GetCurSel();
	if(LB_ERR!=number)
	{
		UpdateData();
		if(m_EditSzBuffer.IsEmpty())
		{	
			OnLbnDblclkListChat();
		}
		else
		{
			m_listBox->DeleteString(number);
			m_listBox->InsertString(number,m_EditSzBuffer);		
		}
	}
}

void CDlgDIYChat::OnBnClickedDelete()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strString;
	int number=m_listBox->GetCurSel();
	if(number!=LB_ERR)
	{
		m_listBox->DeleteString(number);
	}
}

void CDlgDIYChat::OnBnClickedAdd()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();//
	if(!m_EditSzBuffer.IsEmpty())
		m_listBox->InsertString(-1,m_EditSzBuffer);
}

void CDlgDIYChat::OnLbnDblclkListChat()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strString;
	if(LB_ERR!=m_listBox->GetCurSel())
	{
		m_listBox->GetText(m_listBox->GetCurSel(),strString);
		m_EditSzBuffer=strString;
		UpdateData(FALSE);
	}
}

//读talk.ini
void CDlgDIYChat::ReadTalkINI()
{
	CString s=CINIFile::GetAppPath ();/////本地路径
	CString szName;
	if(m_GameOrHall)//游戏房间
		szName=	"TalkInGame.bcf";
	else szName="Talk.bcf";
	CBcfFile f( s +szName );
	CString sec="BZW_TALK";
//	TCHAR path[MAX_PATH];
	CString szTalk;
	CString szNumber;
	int i=1;
	do
	{
		szNumber.Format("%d",i++);
		szTalk=f.GetKeyVal(sec,"T"+szNumber,"");
		if(!szTalk.IsEmpty())
			m_listBox->InsertString(-1,szTalk);
	}while(!szTalk.IsEmpty());
}

void CDlgDIYChat::WriteTalkINI()
{
	CFile file;
	TCHAR szFileName[50],szBuffer[200],szNumber[20];
	memset(szFileName,0,sizeof(szFileName));
	memset(szNumber,0,sizeof(szNumber));
	memset(szBuffer,0,sizeof(szBuffer));
	TCHAR szNewLine[]="\r\n";
	if(m_GameOrHall)//游戏房间
		wsprintf(szFileName,"TalkInGame.bcf");
	else wsprintf(szFileName,"Talk.bcf");

	CString s = CBcfFile::GetAppPath();


	DWORD cfgHandle=cfgOpenFile(s + szFileName);
	if(cfgHandle>0x10)
	{
		CString sec="BZW_TALK";
		//if(m_GameOrHall)
		//	sec="GameWords";
		CString key,val;
		int i=0;
		for(i=0;i<m_listBox->GetCount();i++)
		{
			m_listBox->GetText(i,val);
			key.Format("T%d",i+1);
			if(val!="")
				cfgSetValue(cfgHandle,sec,key,val);
		}
		key.Format("T%d",i+1);
		cfgSetValue(cfgHandle,sec,key,"");
		cfgClose(cfgHandle);
	}


	//file.Open(szFileName,CFile::modeCreate|CFile::modeWrite);//建立新文件
	//file.SeekToBegin();
	//
	//file.Write("[BZW_TALK]",11);			
	//file.Write(szNewLine,strlen(szNewLine));			//换行	
	//
	//for(int i=0;i<m_listBox->GetCount();i++)
	//{
	//	m_listBox->GetText(i,szBuffer);
	//	if(::lstrlen(szBuffer)!=0)
	//	{
	//		wsprintf(szNumber,"T%d=",i+1);
	//		file.Write(szNumber,lstrlen(szNumber));
	//		file.Write(szBuffer,lstrlen(szBuffer));	
	//		file.Write(szNewLine,lstrlen(szNewLine));			//换行	
	//	}
	//}
	//file.Close();
}
void CDlgDIYChat::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
	
}
