#include "stdafx.h"
#include "MyListCtrl.h"

#include "resource.h"

BEGIN_MESSAGE_MAP(CComBodyClassT, CHeaderCtrl)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

CImageList				CHappyGoDlgT::m_UserStateList;				//用户状态列表
static WNDPROC          m_pWndProc = 0;

BEGIN_MESSAGE_MAP(CHappyGoDlgT, CListCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	//ON_WM_PAINT()
END_MESSAGE_MAP()

//构造函数
CComBodyClassT::CComBodyClassT()
{
	m_Type = 0;
	m_Font.CreateFont(-12,0,0,0,400,0,0,0,134,3,2,1,2,TEXT("新宋体"));
}

//析构函数
CComBodyClassT::~CComBodyClassT()
{

}

void CComBodyClassT::SetBkImage(HINSTANCE hInstance,const char *path,const char *path2,COLORREF color,COLORREF tcolor, int type)
{
	m_bkimg.LoadFromResource(hInstance,path);
	m_bkimg2.LoadFromResource(hInstance,path2);
	m_color=color;
	m_textcolor=tcolor;
	m_Type = type;
}

//重画函数
void CComBodyClassT::OnPaint()
{
	CPaintDC dc(this);

	//建立缓冲图
	CRect ClientRect;
	GetClientRect(&ClientRect);
	CBitmap BufferBmp;
	BufferBmp.CreateCompatibleBitmap(&dc,ClientRect.Width(),ClientRect.Height());
	CDC BufferDC;
	BufferDC.CreateCompatibleDC(&dc);
	BufferDC.SelectObject(&BufferBmp);
	BufferDC.SetBkMode(TRANSPARENT);
	BufferDC.SelectObject(m_Font);
	//BufferDC.SetTextColor(CGameImageLink::m_ListResource.m_crListColor);
	BufferDC.SetTextColor(RGB(0,0,0));
	BufferDC.FillSolidRect(&ClientRect,m_color);

	HDITEM HDItem;
	CRect rcItemRect;
	TCHAR szBuffer[30];
	memset(szBuffer,0,sizeof(szBuffer));
	
	for (int i=0;i<GetItemCount();i++)
	{
		//获取信息
		HDItem.pszText=szBuffer;
		HDItem.mask=HDI_TEXT|HDI_FORMAT;
		HDItem.cchTextMax=sizeof(szBuffer)/sizeof(szBuffer[0])-1;
		GetItemRect(i,&rcItemRect);
		GetItem(i,&HDItem);
		//绘画背景
		if(m_Type == 0)
		{//第一种界面类型
			if(i%2==0)
				if(!m_bkimg.IsNull()) m_bkimg.Draw(BufferDC.GetSafeHdc(),rcItemRect);
			else
				if(!m_bkimg2.IsNull()) m_bkimg2.Draw(BufferDC.GetSafeHdc(),rcItemRect);
			CPen pen,*oldpen;
			pen.CreatePen(PS_SOLID,3,m_color);
			oldpen=BufferDC.SelectObject(&pen);
			BufferDC.MoveTo(rcItemRect.left,rcItemRect.bottom);
			BufferDC.LineTo(rcItemRect.right,rcItemRect.bottom);
			if(i!=(GetItemCount()-1))
				BufferDC.LineTo(rcItemRect.right,rcItemRect.top);
			::DeleteObject(BufferDC.SelectObject(*oldpen));
		}
		else if(m_Type == 1)
		{//第二种界面类型
			if(!m_bkimg.IsNull()) m_bkimg.Draw(BufferDC.GetSafeHdc(),rcItemRect);
			if(i!=(GetItemCount()-1))
				if(!m_bkimg2.IsNull()) m_bkimg2.Draw(BufferDC.GetSafeHdc(),rcItemRect.right-m_bkimg2.GetWidth(),rcItemRect.top);
		}
		rcItemRect.DeflateRect(6,2,6,2);

		//绘画标题头
		UINT uFormat=DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS|DT_CENTER;
		//if (HDItem.fmt&HDF_CENTER) uFormat|=DT_CENTER;
		//else if (HDItem.fmt&HDF_LEFT) uFormat|=DT_LEFT;
		//else if (HDItem.fmt&HDF_RIGHT) uFormat|=DT_RIGHT;
		BufferDC.SetTextColor(m_textcolor);
		BufferDC.DrawText(HDItem.pszText,lstrlen(HDItem.pszText),&rcItemRect,uFormat);
	}

	//绘画界面
	dc.BitBlt(0,0,ClientRect.Width(),ClientRect.Height(),&BufferDC,0,0,SRCCOPY);
	BufferDC.DeleteDC();
	BufferBmp.DeleteObject();
	return;
}

//背景函数
BOOL CComBodyClassT::OnEraseBkgnd(CDC * pDC)
{
	return FALSE;
}

/****************************************************************************************************/

/// 构造函数
CHappyGoDlgT::CHappyGoDlgT()
{
	m_sudu = 0;
	m_curSel = -1;
	m_processpos = -1;
	m_FocusbkColor=RGB(255,255,255);//设置颜色
	m_FocusTextColor=RGB(0,0,0);
	m_bkColor=RGB(192,210,212);//设置颜色
	m_TextColor=RGB(0,0,0);
	m_Font.CreateFont(-12,0,0,0,400,0,0,0,134,3,2,1,2,TEXT("新宋体"));
}

//析构函数
CHappyGoDlgT::~CHappyGoDlgT()
{
	m_ProgressList.clear();
	m_Font.DeleteObject();
}

/**
 * 设置要显示的背景图片
 */
void CHappyGoDlgT::SetBgImage(HINSTANCE hInstance,const char *path)
{
	//if(!m_bgImage.IsNull())
	//	m_bgImage.Destroy();

	m_bgImage.LoadFromResource(hInstance,path);
}

/**
 * 初始列表控件头属性
 *
 * @param path,path2 列表头要使用的图片路径
 * @param color 列表头的背景颜色
 * @param tcolor 列表头的字体颜色
 * @param type 列表头的绘制类型
 */
void CHappyGoDlgT::InitListHeader(HINSTANCE hInstance,const char *path,const char *path2,COLORREF color,COLORREF tcolor, int type)
{
	m_ListHeader.SetBkImage(hInstance,path,path2,color,tcolor,type);   
}

/** 设置滚动条资源文件
 */
void CHappyGoDlgT::SetProcessImage(HINSTANCE hInstance,const char *path,const char *path2)
{
	m_probgImage.LoadFromResource(hInstance,path);
	m_proImage.LoadFromResource(hInstance,path2);
}

/**
 * 设置下载文件的大小
 */
void CHappyGoDlgT::SetFileSize(int row,int size)
{
	char str[256];
	sprintf(str,"%d",size);

	SetItemText(row,1,str);
}

/// 设置下载文件的速度
void CHappyGoDlgT::SetFileDownload(int row,int size)
{
	if(m_sudu != size)
	{
		m_sudu = size;

		char str[256];
		sprintf(str,"%d",size);

		SetItemText(row,3,str);
	}
}

/**
 * 解析列表的头
 *
 * @param mHeader 要解析的字符串
 */
void CHappyGoDlgT::ParseListHeader(std::string mHeader)
{
	if(mHeader.empty()) return;

	std::string m_first,m_last;

	ListHeader mListHeader;

	mListHeader.m_Name = mHeader.substr(0,mHeader.find_first_of(","));
	m_first = mHeader.substr(mHeader.find_first_of(",")+1,mHeader.length());
	
	switch(atoi(m_first.substr(0,m_first.find_first_of(",")).c_str()))
	{
	case 0:
		mListHeader.m_Fmt = LVCFMT_LEFT;
		break;
	case 1:
		mListHeader.m_Fmt = LVCFMT_RIGHT;
	case 2:
		mListHeader.m_Fmt = LVCFMT_CENTER;
	default:
		break;
	}

	m_last = m_first.substr(m_first.find_first_of(",")+1,m_first.length());

	mListHeader.m_Width = atoi(m_last.c_str());

	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	lvc.iSubItem = (int)m_ListHeaderData.size();
	lvc.pszText = (LPSTR)mListHeader.m_Name.c_str();
	lvc.cx = mListHeader.m_Width;
	lvc.fmt = mListHeader.m_Fmt;
	InsertColumn((int)m_ListHeaderData.size(),&lvc);

	m_ListHeaderData.push_back(mListHeader);
}

/**
 * 添加一个行头
 *
 * @param pImageId 这个行使用的图像ID
 * @param pName 这个行的名称
 */
void CHappyGoDlgT::AddItemHeader(int pImageId,std::string pName)
{
	ItemHeader m_ItemHeader;

	m_ItemHeader.m_Name = pName;
	m_ItemHeader.m_ImageId = pImageId;

	LV_ITEM lvi;
	lvi.mask = LVIF_TEXT | LVIF_STATE ;

	lvi.iItem = (int)m_ItemHeaderData.size();
	lvi.iSubItem = 0;
	lvi.state = LVIS_SELECTED | LVIS_FOCUSED ;
	lvi.stateMask = LVIS_SELECTED | LVIS_FOCUSED ;
	lvi.pszText = (LPSTR)pName.c_str();
	InsertItem(&lvi);

	m_ItemHeaderData.push_back(m_ItemHeader);
}

/**
 * 设置指定行的进度
 *
 * @param row 要设置的列行
 * @param pro 要设置的进度
 */
void CHappyGoDlgT::SetProcess(int row,float pro)
{
	if(m_ProgressList.empty()) return;

	if(m_curSel != row)
	{
		m_curSel = row;
		SetItemState(-1,0,LVIS_SELECTED);
		SetItemState(row,LVIS_SELECTED,LVIS_SELECTED);
	}

	int temp = (int)(pro * 100.0f);

	if(temp >= 99) temp = 100;
	m_ProgressList[row] = temp;
}

/**
 * 更新行头
 *
 * @param pIndex 行索引
 * @param pImageId 这个行要显示的图片ID
 * @param pName 这个行的名称
 */
void CHappyGoDlgT::UpdateItemHeader(int pIndex,int pImageId,std::string pName)
{
	if(pIndex >= 0 && pIndex < (int)m_ItemHeaderData.size())
	{
		m_ItemHeaderData[pIndex].m_ImageId = pImageId;
		m_ItemHeaderData[pIndex].m_Name = pName;
	}

	SetItemText(pIndex,0,(LPCTSTR)pName.c_str());
}

//控件绑定
void CHappyGoDlgT::PreSubclassWindow()
{
    if(GetHeaderCtrl())
		m_ListHeader.SubclassWindow(GetHeaderCtrl()->GetSafeHwnd());

	CListCtrl::PreSubclassWindow();
}

//绘画函数
void CHappyGoDlgT::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (lpDrawItemStruct->CtlType==ODT_LISTVIEW)
	{
		//获取数据
		UINT iWidth=0;
		TCHAR szBuffer[30];
		CSize TextSize;
		memset(szBuffer,0,sizeof(szBuffer));
		CDC * pDC=CDC::FromHandle(lpDrawItemStruct->hDC);

		CRect	rect;
		GetClientRect(&rect);

		CDC memDC;
		memDC.CreateCompatibleDC(pDC);

		CBitmap Screen;
		Screen.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
		memDC.SelectObject(&Screen);
		CFont *pOldFont = memDC.SelectObject(&m_Font);
		Screen.DeleteObject();

		memDC.SetBkMode(TRANSPARENT);
		memDC.FillSolidRect(&rect,RGB(255,0,255));
	
		//设置颜色
		COLORREF crTextColor,crBackColor;
		bool bSelect=(lpDrawItemStruct->itemState&ODS_SELECTED);//||(lpDrawItemStruct->itemState&ODS_FOCUS);
		GetDrawColor(crTextColor,crBackColor,0,bSelect);
		memDC.SetBkColor(crBackColor);
		memDC.SetTextColor(crTextColor);
		//memDC.SelectObject(m_Font);
		memDC.SetBkMode(TRANSPARENT);

		// 绘制背景
		if(lpDrawItemStruct->itemID % 2 == 0)
			memDC.FillSolidRect(lpDrawItemStruct->rcItem.left,lpDrawItemStruct->rcItem.top,
				lpDrawItemStruct->rcItem.right-lpDrawItemStruct->rcItem.left,lpDrawItemStruct->rcItem.bottom-lpDrawItemStruct->rcItem.top,RGB(154,169,200));
		else
			memDC.FillSolidRect(lpDrawItemStruct->rcItem.left,lpDrawItemStruct->rcItem.top,
				lpDrawItemStruct->rcItem.right-lpDrawItemStruct->rcItem.left,lpDrawItemStruct->rcItem.bottom-lpDrawItemStruct->rcItem.top,RGB(213,218,238));

		//绘画信息
		if(bSelect)
		{
			memDC.FillSolidRect(lpDrawItemStruct->rcItem.left,lpDrawItemStruct->rcItem.top,
				lpDrawItemStruct->rcItem.right-lpDrawItemStruct->rcItem.left,lpDrawItemStruct->rcItem.bottom-lpDrawItemStruct->rcItem.top,crBackColor);
		}

		for (int i=0;i<m_ListHeader.GetItemCount();i++)
		{
			int iTemp=GetColumnWidth(i);

			if(m_processpos == i) 
			{
				TextSize=pDC->GetTextExtent(szBuffer,lstrlen(szBuffer));
				GetItemText(lpDrawItemStruct->itemID,i,szBuffer,sizeof(szBuffer));
				CRect TextRect(lpDrawItemStruct->rcItem.left+iWidth,lpDrawItemStruct->rcItem.top,lpDrawItemStruct->rcItem.left+iWidth+iTemp,lpDrawItemStruct->rcItem.bottom);

				if(!m_probgImage.IsNull() &&
					!m_proImage.IsNull())
				{
					m_probgImage.Draw(memDC,
						              CRect(TextRect.left,TextRect.top,TextRect.left+m_probgImage.GetWidth(),TextRect.top+m_probgImage.GetHeight()),
						              CRect(0,0,m_probgImage.GetWidth(),m_probgImage.GetHeight()));
					
					int length = (int)(m_probgImage.GetWidth() * (m_ProgressList[lpDrawItemStruct->itemID] / 100.0f));

					int count = length / m_proImage.GetWidth() - 2;

					for(int i=0;i<count;i++)
					{
						m_proImage.Draw(memDC,TextRect.left+i*m_proImage.GetWidth()+1,TextRect.top);
					}

					char buff[128];
					sprintf(buff,"%d",m_ProgressList[lpDrawItemStruct->itemID]);
					strcat(buff," %");

					memDC.DrawText(buff,lstrlen(buff),TextRect,DT_RIGHT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
				}

				iWidth+=iTemp;

				continue;
			}
			else
			{
				TextSize=pDC->GetTextExtent(szBuffer,lstrlen(szBuffer));
				GetItemText(lpDrawItemStruct->itemID,i,szBuffer,sizeof(szBuffer));
				CRect TextRect(lpDrawItemStruct->rcItem.left+iWidth,lpDrawItemStruct->rcItem.top,lpDrawItemStruct->rcItem.left+iWidth+iTemp,lpDrawItemStruct->rcItem.bottom);
				memDC.DrawText(szBuffer,lstrlen(szBuffer),&TextRect,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
				iWidth+=iTemp;
			}
		}

		pDC->TransparentBlt(rect.left, rect.top, rect.Width(), rect.Height(), &memDC, rect.left,rect.top,rect.Width(),rect.Height(),RGB(255,0,255));
		memDC.SelectObject(pOldFont);
		//dc.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);

		memDC.DeleteDC();
	}
	return;
}

/**
 * 标签的绘制
 */
void CHappyGoDlgT::OnPaint()
{
	CListCtrl::OnPaint();
}

//背景函数
BOOL CHappyGoDlgT::OnEraseBkgnd(CDC * pDC)
{
	CRect DrawRect,HeadRect;
	pDC->GetClipBox(&DrawRect);
	if (m_ListHeader.GetSafeHwnd()!=NULL)
	{
		m_ListHeader.GetWindowRect(&HeadRect);
		if (DrawRect.top<HeadRect.Height()) DrawRect.top=HeadRect.Height();
	}
	COLORREF crTextColor,crBackColor;
	GetDrawColor(crTextColor,crBackColor,0,false);
	pDC->FillSolidRect(&DrawRect,crBackColor);
	return TRUE;
}

//获取颜色
bool CHappyGoDlgT::GetDrawColor(COLORREF & crTextColor, COLORREF & crBackColor, UINT uRow, bool bSelect)
{
	if (bSelect)
	{
		crTextColor=m_FocusTextColor;
		crBackColor=m_FocusbkColor;
	}
	else
	{
		crTextColor=m_TextColor;
		crBackColor=m_bkColor;
	}

	return true;
}

void CHappyGoDlgT::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CListCtrl::OnShowWindow(bShow, nStatus);

	// TODO: 在此处添加消息处理程序代码
	if(GetHeaderCtrl())
		m_ListHeader.SubclassWindow(GetHeaderCtrl()->m_hWnd);
}

/**
 * 初始列表属性
 */
void CHappyGoDlgT::InitListCtrl(COLORREF bkColor,COLORREF TextColor,COLORREF FocusbkColor,COLORREF FocusTextColor,
		              std::string resourceimage)
{
	m_bkColor = bkColor;
	m_TextColor = TextColor;
	m_FocusbkColor = FocusbkColor;
	m_FocusTextColor = FocusTextColor;

	SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_ONECLICKACTIVATE|LVS_EX_INFOTIP);
	SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,LVS_EX_FULLROWSELECT,LVS_EX_FULLROWSELECT);

	////设置图片
	//if (m_UserStateList.GetSafeHandle()==NULL)
	//{
	//	m_UserStateList.DeleteImageList();
	//	//装载位图
	//	BITMAP BmpInfo;
	//	CImage	UserStateBmp;
	//	CBitmap NewUserStateBmp;

	//	UserStateBmp.Load((TCHAR*)resourceimage.c_str());
	//	if(UserStateBmp.IsNull()) return;

	//	BmpInfo.bmWidth=UserStateBmp.GetWidth();
	//	BmpInfo.bmHeight=UserStateBmp.GetHeight();

	//	//建立位图
	//	CDC TargetDC, * pDC=GetDC();
	//	TargetDC.CreateCompatibleDC(pDC);
	//	NewUserStateBmp.CreateCompatibleBitmap(pDC,BmpInfo.bmWidth,BmpInfo.bmHeight);
	//	CBitmap * pOldBmp=TargetDC.SelectObject(&NewUserStateBmp);
	//	TargetDC.FillSolidRect(0,0,BmpInfo.bmWidth,BmpInfo.bmHeight,m_bkColor);

	//	CDC MemDC;
	//	MemDC.CreateCompatibleDC(&TargetDC);
	//	MemDC.SelectObject(UserStateBmp);
	//	
	//	TransparentBlt(TargetDC.GetSafeHdc(),0,0,BmpInfo.bmWidth,BmpInfo.bmHeight,MemDC.GetSafeHdc(),0,0,BmpInfo.bmWidth,BmpInfo.bmHeight,RGB(255,255,255));

	//	TargetDC.SelectObject(pOldBmp);
	//	TargetDC.DeleteDC();

	//	//建立资源列
	//	m_UserStateList.Create(USER_STATE_WIDTH,USER_STATE_HEIGHT,ILC_COLOR16,0,0);
	//	m_UserStateList.Add(&NewUserStateBmp,RGB(0,0,255));

	//	//清理资源
	//	UserStateBmp.Detach();
	//	NewUserStateBmp.Detach();
	//	ReleaseDC(pDC);
	//}
	//SetImageList(&m_UserStateList,LVSIL_SMALL);
}

void CHappyGoDlgT::OnTimer(UINT nIDEvent)
{
	if(nIDEvent == 100 &&
		m_curSel > -1)
	{
		SetItemState(-1,0,LVIS_SELECTED);
		SetItemState(m_curSel,LVIS_SELECTED,LVIS_SELECTED);
	}
}

/// 刷新选择的控件
void CHappyGoDlgT::RefreshSelectItem(void)
{
	if(m_curSel > -1)
	{
		SetItemState(-1,0,LVIS_SELECTED);
		SetItemState(m_curSel,LVIS_SELECTED,LVIS_SELECTED);
	}
}