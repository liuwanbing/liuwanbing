#include "Stdafx.h"
#include "AFCLoader.h"
#include "ChildView.h"
#include ".\childview.h"

BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

//构造函数
CChildView::CChildView()
{
}

//析构函数
CChildView::~CChildView()
{
}

//初始化建立参数
BOOL CChildView::PreCreateWindow(CREATESTRUCT & cs) 
{
	if (__super::PreCreateWindow(cs)==FALSE) return FALSE;

	//修改参数
	cs.dwExStyle|=WS_EX_CLIENTEDGE;
	cs.style&=~WS_BORDER;
	cs.lpszClass=AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS,
		::LoadCursor(NULL,IDC_ARROW),reinterpret_cast<HBRUSH>(COLOR_WINDOW+1),NULL);

	return TRUE;
}

//重画函数
void CChildView::OnPaint()
{
	CPaintDC dc(this);
}

//建立消息
int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//建立控件
	m_ServiceControl.Create(IDD_SERVICE_CONTROL,this);
	m_ServiceControl.ShowWindow(SW_SHOW);
CStatic* pStatic1=new CStatic();
pStatic1->Create(TMLcopyright,WS_CHILD|WS_VISIBLE,CRect(0,0,2000,16),this,10100);
	return 0;
}

//位置消息
void CChildView::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	//移动控件
	m_ServiceControl.MoveWindow(0,16,cx,cy-16);
	
	return;
}
