// PersonalSet.cpp : 实现文件
//

#include "stdafx.h"
#include "PersonalSet.h"
#include "afcgamelistwnd.h"

// CPersonalSet 对话框

IMPLEMENT_DYNAMIC(CPersonalSet, CDialog)

CPersonalSet::CPersonalSet(CWnd* pParent /*=NULL*/)
	: CDialog(CPersonalSet::IDD, pParent)
{
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f( s + "skin.bcf");
	CString key=TEXT("PersonalSet");
	//int r,g,b;
	//r=f.GetKeyVal(key,"PropPannelColor",44);
	//g=f.GetKeyVal(key,"PropPannelG",114);
	//b=f.GetKeyVal(key,"PropPannelB",233);
	m_bkcolor=(COLORREF)f.GetKeyVal(key,"PersonalBKColor",44);//RGB(r,g,b);

	CBcfFile fini(s+"bzgame.bcf");
	m_strSetUrl=fini.GetKeyVal("BZW","PersonalSetUrl","http://game.bzw.cn/app/PersonalSet/BaseInfo.asp");
}

CPersonalSet::~CPersonalSet()
{
}

void CPersonalSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXPLORER1, m_ie);
	DDX_Control(pDX, IDC_CLOSE, m_bnClose);
}


BEGIN_MESSAGE_MAP(CPersonalSet, CDialog)
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_CLOSE, &CPersonalSet::OnBnClickedClose)
END_MESSAGE_MAP()


// CPersonalSet 消息处理程序

void CPersonalSet::OnLButtonDown(UINT nFlags, CPoint point)
{
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));

	CDialog::OnLButtonDown(nFlags, point);
}

BOOL CPersonalSet::OnInitDialog()
{
	CDialog::OnInitDialog();
	CRect rc;
	GetWindowRect(&rc);
	m_bnClose.MoveWindow(rc.Width()-25,2,17,17);
	m_ie.MoveWindow(5,26,/*480*/rc.Width() - 10,rc.Height() - 29);
	setSkinIndex(0);
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	SetWindowText(fMsg.GetKeyVal("MainRoom","PersonalConfig","个人设置"));	

	//SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOSIZE);   //对话框置顶  yjj 090309
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CPersonalSet::setSkinIndex(int sk)
{
	try
	{
		CString s;
		s.Format("%s\\%sdialog\\dialog.bmp",CBcfFile::GetAppPath(true),m_skinmgr.GetSkinPath());
		//	s.Format("%s\\image\\skin%d\\dialog\\dialog.bmp",CBcfFile::GetAppPath(true),m_skinIndex);

#ifndef BZ_ZLIB
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

		TCHAR path[MAX_PATH];

		wsprintf(path,"%s\\%sdialog\\log_exit_bt.bmp",CBcfFile::GetAppPath(true),m_skinmgr.GetSkinPath());//win_close.bmp
		m_bnClose.LoadButtonBitmap(path,false);
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"PERSONAL_SET_TRY_CATCH : [%s]",__FUNCTION__);
		
	}
}

void CPersonalSet::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	drawSkin();
	//wuhsuqun 2009.6.3
	//修改个人设置页面，“密码保护”页面内容会被刷新掉
	//if(!m_ie.get_Busy())
	//	m_ie.Refresh();
}

void CPersonalSet::drawSkin(void)
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
		int topHeight=GetSystemMetrics(SM_CYFRAME)+GetSystemMetrics(SM_CYSIZE)+6;
		g.DrawImage(imgDialog,RectF(0.0f,0.0f,10.0f,topHeight),0.0f,0.0f,10.0f,topHeight,UnitPixel);
		g.DrawImage(imgDialog,RectF(10.0f,0.0f,rc.Width()-20,topHeight),9.0f,0.0f,1.0f,topHeight,UnitPixel);
		g.DrawImage(imgDialog,RectF((REAL)rc.Width()-10,0.0f,10.0f,topHeight),15.0f,0.0f,10.0f,topHeight,UnitPixel);
		m_bkcolor=pdc->GetPixel(9,topHeight-1);

		int bottomHeight=GetSystemMetrics(SM_CYFRAME)+3;
		g.DrawImage(imgDialog,RectF(0.0f,topHeight,GetSystemMetrics(SM_CXFRAME)+5.0f,rc.Height()-topHeight-bottomHeight),0.0f,30.0f,GetSystemMetrics(SM_CXFRAME)+5.0f,60.0f,UnitPixel);
		g.DrawImage(imgDialog,RectF(rc.Width()-GetSystemMetrics(SM_CXFRAME)-5.0f,topHeight,GetSystemMetrics(SM_CXFRAME)+5.0f,rc.Height()-topHeight-bottomHeight),20-GetSystemMetrics(SM_CXFRAME),30.0f,GetSystemMetrics(SM_CXFRAME)+5.0f,60.0f,UnitPixel);
		//g.DrawImage(imgDialog,RectF(rc.Width()-15,30,15,rc.Height()-40),10,30,15,60,UnitPixel);

		g.DrawImage(imgDialog,RectF(0,rc.Height()-bottomHeight,10,bottomHeight),0,100-bottomHeight,10,bottomHeight,UnitPixel);
		g.DrawImage(imgDialog,RectF(10,rc.Height()-bottomHeight,rc.Width()-20,bottomHeight),10,100-bottomHeight,1,bottomHeight,UnitPixel);
		g.DrawImage(imgDialog,RectF(rc.Width()-10,rc.Height()-bottomHeight,10,bottomHeight),15,100-bottomHeight,10,bottomHeight,UnitPixel);

		CString s="宋体";

		Font font(s.AllocSysString(),12,0,UnitPixel);
		SolidBrush brush(Color(255,255,255));
		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
		s = fMsg.GetKeyVal("MainRoom","PersonalConfig","个人设置");
//		s="个人设置";
		g.DrawString(s.AllocSysString(),-1,&font,PointF(5,5),0,&brush);
		g.ReleaseHDC(pdc->m_hDC);
		m_bnClose.Invalidate(FALSE);
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"LOGO_UP_TRY_CATCH : [%s]",__FUNCTION__);
	}

}

void CPersonalSet::openPersonalSet(void)
{
	CGameListWnd * pWnd=(CGameListWnd*)pParentWnd;
	CString surl;//http://game.bzw.cn/app/PersonalSet/BaseInfo.asp
	//?UserID=%d&token+%s&bgcolor=%s
	surl.Format("%s?UserID=%d&token=%s&bgcolor=%2x%2x%2x",m_strSetUrl,pWnd->m_PlaceUserInfo->dwUserID,Glb().m_TML_SN,m_bkcolor&0xFF,(m_bkcolor&0xFF00)>>8,m_bkcolor>>16);
	//surl.Format("%s?userid=%d&token=%s&bgcolor=%d",m_strSetUrl,pWnd->m_PlaceUserInfo->dwUserID,Glb().m_TML_SN,m_bkcolor&0xFF,(m_bkcolor&0xFF00)>>8,m_bkcolor>>16);
	//surl.Format("user,%d",pWnd->m_PlaceUserInfo->dwUserID);
	m_ie.Navigate(surl,0,0,0,0);
	this->ShowWindow(SW_SHOW);
}

void CPersonalSet::OnBnClickedClose()
{
	this->ShowWindow(SW_HIDE);
}
