#include "StdAfx.h"
#include ".\configwnd.h"

CConfigWnd::CConfigWnd(void)
{
	m_isvisable = false;
	m_bBgSound	= false;
	m_bGameSound= false;
	m_bShakeWindow = false;
	m_bTipMsg = false;
	m_value1 = 0;
	m_value2 = 0;
	m_ptx1 = 320;
	m_ptx2 = 320;
}

CConfigWnd::~CConfigWnd(void)
{
}
//
void CConfigWnd::InitDlg(int x,int y)
{
	

	m_ptx = x;
	m_pty = y;

};

void  CConfigWnd::LMouseDown(int ptx,int pty)
{
	if(!m_isvisable)return;
	POINT m_point;
	m_point.x = ptx;
	m_point.y = pty;

    CRect m_rect;
	m_rect.top = m_pty+330;
	m_rect.bottom = m_rect.top + 52;
	m_rect.left = m_ptx+200;
	m_rect.right = m_rect.left + 150;
	if(m_rect.PtInRect(m_point))m_isvisable=false;
	//
	m_rect.top = m_pty+330;
	m_rect.bottom = m_rect.top + 52;
	m_rect.left = m_ptx+50;
	m_rect.right = m_rect.left + 150;
	if(m_rect.PtInRect(m_point))m_isvisable=false;
	//
	m_rect.top = m_pty+140;
	m_rect.bottom = m_rect.top + 30;
	m_rect.left = m_ptx+35+m_ptx1;
	m_rect.right = m_rect.left + 30;
	if(m_rect.PtInRect(m_point)){m_s1 =  true;}
	//
	m_rect.top = m_pty+205;
	m_rect.bottom = m_rect.top + 30;
	m_rect.left = m_ptx+35+(m_ptx2);
	m_rect.right = m_rect.left + 30;
	if(m_rect.PtInRect(m_point))m_s2 =  true;
	m_oldptx = ptx;

	//m_value1 = m_value1 + 0.1;

	

}
void  CConfigWnd::LMouseUp(int ptx,int pty)
{
	m_s1 =  false;
	m_s2 =  false;

  
}
void  CConfigWnd::LMouseMove(int ptx,int pty)
{
	if(!m_isvisable)return;
	if(m_s1)
	{
		m_ptx1 =  ptx - m_ptx-40;
		if(m_ptx1<0)m_ptx1=0;
		if(m_ptx1>310)m_ptx1=310;
	

	}
	if(m_s2)
	{
		m_ptx2 =  ptx - m_ptx-40;
		if(m_ptx2<0)m_ptx2=0;
		if(m_ptx2>310)m_ptx2=310;


	}
   
}