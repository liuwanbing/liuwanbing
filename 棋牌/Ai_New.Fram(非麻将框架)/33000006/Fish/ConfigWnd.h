#pragma once

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

	//初始化
	void InitDlg(int x,int y);

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
