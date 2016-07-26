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
void CConfigWnd::InitDlg(LPDIRECT3DDEVICE9 g_pd3dDevice,int x,int y)
{
	m_pd3dDevice = g_pd3dDevice;
	//ÔØÈë±³¾°Í¼
	D3DXCreateTextureFromFileEx(m_pd3dDevice,"Jcfish/Ui/Other/setDlg.png",D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,0,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_FILTER_NONE,D3DX_DEFAULT,0xfffffff0,NULL,NULL,&m_LoadBgTexture);
	D3DXCreateTextureFromFileEx(m_pd3dDevice,"Jcfish/Ui/Other/DlgOK2.png",D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,0,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_FILTER_NONE,D3DX_DEFAULT,0xfffffff0,NULL,NULL,&m_btText[0]);
	D3DXCreateTextureFromFileEx(m_pd3dDevice,"Jcfish/Ui/Other/dlgCancle2.png",D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,0,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_FILTER_NONE,D3DX_DEFAULT,0xfffffff0,NULL,NULL,&m_btText[1]);
	D3DXCreateTextureFromFileEx(m_pd3dDevice,"Jcfish/Ui/Other/SetDlgPitch.png",D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,0,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_FILTER_NONE,D3DX_DEFAULT,0xfffffff0,NULL,NULL,&g_sliderbgTex);
	D3DXCreateTextureFromFileEx(m_pd3dDevice,"Jcfish/Ui/Other/setDlgSlider.png",D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,0,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_FILTER_NONE,D3DX_DEFAULT,0xfffffff0,NULL,NULL,&g_sliderperTex);
	D3DXCreateTextureFromFileEx(m_pd3dDevice,"Jcfish/Ui/Other/setDlgThumb.png",D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,0,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_FILTER_NONE,D3DX_DEFAULT,0xfffffff0,NULL,NULL,&g_sliderbuttonTex);

	m_ptx = x;
	m_pty = y;

};
//
void CConfigWnd::InitRender(LPD3DXSPRITE  m_pSprite)
{
	if(!m_isvisable)return;
	//±³¾°
	m_pSprite->Draw(m_LoadBgTexture, NULL, NULL, &D3DXVECTOR3(m_ptx,m_pty, 0), D3DCOLOR_ARGB(255,255,255,255));
	m_pSprite->Draw(g_sliderperTex, NULL, NULL, &D3DXVECTOR3(m_ptx+33,m_pty+146, 0), D3DCOLOR_ARGB(255,255,255,255));
	m_pSprite->Draw(g_sliderbgTex, NULL, NULL, &D3DXVECTOR3(m_ptx+35,m_pty+150, 0), D3DCOLOR_ARGB(255,255,255,255));
	m_pSprite->Draw(g_sliderbuttonTex, NULL, NULL, &D3DXVECTOR3(m_ptx+35+m_ptx1,m_pty+140, 0), D3DCOLOR_ARGB(255,255,255,255));

	m_pSprite->Draw(g_sliderperTex, NULL, NULL, &D3DXVECTOR3(m_ptx+33,m_pty+146+65, 0), D3DCOLOR_ARGB(255,255,255,255));
	m_pSprite->Draw(g_sliderbgTex, NULL, NULL, &D3DXVECTOR3(m_ptx+35,m_pty+150+65, 0), D3DCOLOR_ARGB(255,255,255,255));
	m_pSprite->Draw(g_sliderbuttonTex, NULL, NULL, &D3DXVECTOR3(m_ptx+35+m_ptx2,m_pty+140+65, 0), D3DCOLOR_ARGB(255,255,255,255));

	m_pSprite->Draw(m_btText[0], NULL, NULL, &D3DXVECTOR3(m_ptx+50,m_pty+330, 0), D3DCOLOR_ARGB(255,255,255,255));
	m_pSprite->Draw(m_btText[1], NULL, NULL, &D3DXVECTOR3(m_ptx+200,m_pty+330, 0), D3DCOLOR_ARGB(255,255,255,255));

   
}
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
		//m_BgSound->SetVoliceValue(m_ptx1/3);
		CBassMusicManager::GetInstance()->SetVolumn(m_ptx1/3);

	}
	if(m_s2)
	{
		m_ptx2 =  ptx - m_ptx-40;
		if(m_ptx2<0)m_ptx2=0;
		if(m_ptx2>310)m_ptx2=310;

		CBassMusicManager::GetInstance()->SetVolumn(m_ptx2/3);
		//CocosDenshion::SimpleAudioEngine::sharedEngine()->setEffectsVolume(m_ptx2/3);

	}
   
}