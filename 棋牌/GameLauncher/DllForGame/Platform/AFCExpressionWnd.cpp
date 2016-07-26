#include "../stdafx.h"
#include "AFCExpressionWnd.h"

//聊天表情图像大小
#define PIC_SPACE					6//点聊天表情出来的图片的间隔
#define EP_PIC_WHIDE				24								
#define EP_PIC_HIGHT				24		

UINT					CGameTalkText::m_uMaxRow=10;							//最大列数目
UINT					CGameTalkText::m_uMaxLine=MAX_EXPRESS_ITEM/10+1;							//最大行数目
CTrueItem				CGameTalkText::m_ExpreesionItem[MAX_EXPRESS_ITEM];		//表情子项

BEGIN_MESSAGE_MAP(CGameTalkText, CDialog)
	ON_WM_KILLFOCUS()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

/*******************************************************************************************************/

//构造函数
CTrueItem::CTrueItem() 
{ 
	m_uTranceLength=0;
	m_szTrancelate[0]=0;
	m_szChineseName[0]=0;
};

//析构函数
CTrueItem::~CTrueItem() 
{
}

//输入解释
void CTrueItem::SetExpressInfo(TCHAR * szChineseName, TCHAR * szTrancelate, UINT uExpPicID)
{
	m_ExpPicImage.LoadBitmap(uExpPicID);
	lstrcpy(m_szTrancelate,szTrancelate);
	lstrcpy(m_szChineseName,szChineseName);	
	m_uTranceLength=lstrlen(szTrancelate);
	return;
};

/*******************************************************************************************************/

//构造函数
CGameTalkText::CGameTalkText()
{
	m_pParentWnd=NULL;
	m_uSelectIndex=MAX_EXPRESS_ITEM;
	InitExpressItem();
}

//析构函数
CGameTalkText::~CGameTalkText()
{
}

//初始化资源
bool CGameTalkText::InitExpressItem()
{
	static bool bInit=false;
	if (bInit==false)
	{
		//HMODULE hResDllHandle=AfxLoadLibrary(TEXT("BZW_Res.dll"));
		//if (hResDllHandle!=NULL)
		//{
		//	//设置信息      表情图片
		//	AfxSetResourceHandle(hResDllHandle);
			char s[5];
			memset(s, 0,5);
			for (int i=0;i<MAX_EXPRESS_ITEM;i++)
			{
				wsprintf(s,"/:%02d",i);
				m_ExpreesionItem[i].SetExpressInfo(TEXT("BZW"),s,IDB_EP_001+i);
			}
			//FreeLibrary(hResDllHandle);
			//AfxSetResourceHandle(::GetModuleHandle(NULL));
			bInit=true;
		//}
		return true;
	}
	return bInit;
}


//建立函数
bool CGameTalkText::CreateExpressionWnd(CWnd * pParentWnd, HWND pParentHWND)
{
	//建立窗口
	m_pParentWnd=pParentWnd;
	if (m_pParentWnd != NULL)
		m_hParentHWND = pParentWnd->GetSafeHwnd();
	else
		m_hParentHWND = pParentHWND;
	//建立窗口
	m_pParentWnd=pParentWnd;
	AfxSetResourceHandle(GetModuleHandle(FACE_DLL_NAME));
	Create(IDD_NULL,pParentWnd);
	AfxSetResourceHandle(GetModuleHandle(NULL));
	if (GetSafeHwnd()==NULL) return false;
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);
	BringWindowToTop();

	return true;
}

//获取适合大小
bool CGameTalkText::GetFixSize(CSize & WndSize)
{
	WndSize.cx=m_uMaxRow*(EP_PIC_WHIDE+PIC_SPACE)+PIC_SPACE;
	WndSize.cy=m_uMaxLine*(EP_PIC_HIGHT+PIC_SPACE)+PIC_SPACE;
	return true;
}

//获取适合大小
bool CGameTalkText::GetFixSize(SIZE & WndSize)
{
	WndSize.cx=m_uMaxRow*(EP_PIC_WHIDE+PIC_SPACE)+PIC_SPACE;
	WndSize.cy=m_uMaxLine*(EP_PIC_HIGHT+PIC_SPACE)+PIC_SPACE;
	return true;
}


//获取表情
CTrueItem * CGameTalkText::GetExpressItem(UINT uIndex)
{
	if (uIndex<MAX_EXPRESS_ITEM) return &m_ExpreesionItem[uIndex];
	return NULL;
}

//失去焦点消息
void CGameTalkText::OnKillFocus(CWnd * pNewWnd)
{
	CDialog::OnKillFocus(pNewWnd);
	DestroyWindow();
}

//绘画头像
void CGameTalkText::DrawItem(CDC * pDC, UINT iXPos, UINT iYPos, bool bSelect)
{
	CTrueItem * pItem=GetExpressItem(iYPos*m_uMaxRow+iXPos);
	if (pItem!=NULL)
	{
		if (bSelect)
		{
			pDC->FillSolidRect(iXPos*(EP_PIC_WHIDE+PIC_SPACE)+PIC_SPACE/2,iYPos*(EP_PIC_HIGHT+PIC_SPACE)+PIC_SPACE/2,
				EP_PIC_WHIDE+PIC_SPACE,EP_PIC_HIGHT+PIC_SPACE,RGB(233,233,233));//以前的表情背景RGB(107,142,198)
			pDC->Draw3dRect(iXPos*(EP_PIC_WHIDE+PIC_SPACE)+PIC_SPACE/2,iYPos*(EP_PIC_HIGHT+PIC_SPACE)+PIC_SPACE/2,
				EP_PIC_WHIDE+PIC_SPACE,EP_PIC_HIGHT+PIC_SPACE,RGB(28,57,208),RGB(28,57,208));
		}
		else
		{
			pDC->FillSolidRect(iXPos*(EP_PIC_WHIDE+PIC_SPACE)+PIC_SPACE/2,iYPos*(EP_PIC_HIGHT+PIC_SPACE)+PIC_SPACE/2,
				EP_PIC_WHIDE+PIC_SPACE,EP_PIC_HIGHT+PIC_SPACE,RGB(233,233,233));//以前的表情背景RGB(107,142,198)
		}
		CDC MemDC;
		MemDC.CreateCompatibleDC(pDC);
		MemDC.SelectObject(&pItem->m_ExpPicImage);
		::TransparentBlt(pDC->GetSafeHdc(),iXPos*(EP_PIC_WHIDE+PIC_SPACE)+PIC_SPACE,iYPos*(EP_PIC_HIGHT+PIC_SPACE)+PIC_SPACE,
			EP_PIC_WHIDE,EP_PIC_HIGHT,MemDC.GetSafeHdc(),0,0,EP_PIC_WHIDE,EP_PIC_HIGHT,MemDC.GetPixel(0,0));
		MemDC.DeleteDC();
	}
	return;
}

//重画消息
void CGameTalkText::OnPaint()
{
	//绘画背景
	CPaintDC dc(this);
	CRect ClientRect;
	GetClientRect(&ClientRect);
	dc.FillSolidRect(&ClientRect,RGB(233,233,233));//以前的表情背景RGB(107,142,198)
	dc.Draw3dRect(&ClientRect,RGB(0,75,151),RGB(0,75,151));

	//绘画表情头像
	for (UINT i=0;i<m_uMaxLine*m_uMaxRow;i++) 
	{
		if(i<MAX_EXPRESS_ITEM)
			DrawItem(&dc,i%m_uMaxRow,i/m_uMaxRow,(m_uSelectIndex==i));
	}
}

//鼠标移动消息
void CGameTalkText::OnMouseMove(UINT nFlags, CPoint point)
{
	UINT SelectIndex=HitItemTest(point);
	if (SelectIndex!=m_uSelectIndex)
	{
		CDC * pDC=GetDC();
		if (m_uSelectIndex!=MAX_EXPRESS_ITEM)
		{
			//清除旧选择
			DrawItem(pDC,m_uSelectIndex%m_uMaxRow,m_uSelectIndex/m_uMaxRow,false);
			m_uSelectIndex=MAX_EXPRESS_ITEM;
		}
		m_uSelectIndex=SelectIndex;
		if (m_uSelectIndex!=MAX_EXPRESS_ITEM)
		{
			//显示新选择
			CTrueItem * pItem=GetExpressItem(m_uSelectIndex);
			if (pItem!=NULL) DrawItem(pDC,m_uSelectIndex%m_uMaxRow,m_uSelectIndex/m_uMaxRow,true);
		}
		ReleaseDC(pDC);
	}

	return;
}
//鼠标按下消息
void CGameTalkText::OnLButtonDown(UINT nFlags, CPoint point)
{
	if ((m_uSelectIndex!=MAX_EXPRESS_ITEM)&&(m_hParentHWND!=NULL))
	{
		::SendMessage(m_hParentHWND, WM_HIT_EXPMESSTION,(WPARAM)m_uSelectIndex,(LPARAM)m_uSelectIndex);
		DestroyWindow();
	}
	return;
}
//按键测试
UINT CGameTalkText::HitItemTest(CPoint MousePoint)
{
	UINT uXPos=MousePoint.x/(PIC_SPACE+EP_PIC_WHIDE);
	UINT uYPos=MousePoint.y/(PIC_SPACE+EP_PIC_HIGHT);
	uXPos=__min(m_uMaxRow-1,uXPos);
	uYPos=__min(m_uMaxLine-1,uYPos);
	return uYPos*m_uMaxRow+uXPos;
}

/*******************************************************************************************************/