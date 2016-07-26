#pragma once
#include <d3dx9.h>
#include <d3d9.h>
#pragma comment (lib,"d3dx9.lib")
#pragma comment (lib,"d3d9.lib")
#include "BassMusicManager.h"
class CConfigWnd
{
public:
	CConfigWnd(void);
	~CConfigWnd(void);
	//位置X坐标
	int m_ptx;
	//位置Y坐标
	int m_pty;
	//是否显示
	bool m_isvisable;
	//
	LPDIRECT3DDEVICE9 m_pd3dDevice;
	//初始化
	void InitDlg(LPDIRECT3DDEVICE9 g_pd3dDevice,int x,int y);
	//开始绘制
	void InitRender(LPD3DXSPRITE  m_pSprite);
	//
	//载入背景
	LPDIRECT3DTEXTURE9      m_LoadBgTexture;
	//按扭
	LPDIRECT3DTEXTURE9      m_btText[2];
	//滚动条背景
	LPDIRECT3DTEXTURE9      g_sliderbgTex ;  
	//滚动条进度
	LPDIRECT3DTEXTURE9      g_sliderperTex ;  
	//滚动条按扭
	LPDIRECT3DTEXTURE9      g_sliderbuttonTex ;  
	void  LMouseDown(int ptx,int pty);
	void  LMouseUp(int ptx,int pty);
	void  LMouseMove(int ptx,int pty);
	float   m_value1;
	float   m_value2;
	int     m_ptx1;
	int     m_ptx2;
	int   m_oldptx;
	bool  m_s1;
	bool  m_s2;

	//游戏音效设置
	bool	m_bBgSound;
	bool	m_bGameSound;
	bool	m_bShakeWindow;
	bool	m_bTipMsg;			//是否给出提示
};
