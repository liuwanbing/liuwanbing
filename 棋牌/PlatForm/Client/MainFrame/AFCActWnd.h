#ifndef CAFCACTWND_HEAD_FILE
#define CAFCACTWND_HEAD_FILE


#define WM_SHOWGAMEWINDOW			WM_USER+1					//发送给房间, 决定是否显示游戏
#define WM_LOADSKIN					WM_USER+2

#define TIMER_AD					1							//广告移动计时器

#include "SkinMgr.h"
class  CActWnd: public CWnd
{
	//信息变量
protected:
	CHyperLink						m_copyright;				//版权处理
	CGameImage						m_actl,m_actr,m_actc;		//走马灯背景
	int								m_actadx,m_actady;			//活动坐标
	CString							m_ad;						//走马灯文字
	CRect							m_adRect,m_addrawrect;		//广告区域
	CSize							m_adsize;					//广告域大小

	bool							m_bCopyRight;				//是否版权
	HCURSOR							m_cursor;
	TCHAR							m_szURL[300];
	//skin
	int								m_x,m_y;
	TCHAR							m_pathn[MAX_PATH],m_patha[MAX_PATH];
	COLORREF						m_cn,m_ca,m_actadcolor;
	//函数定义
public:
	//构造函数
	CActWnd();
	//析构函数
	virtual ~CActWnd();

	DECLARE_MESSAGE_MAP()

	//功能函数
public:
	//建立函数
	bool CreateWnd(CWnd * pParentWnd, UINT uWndID);
	//显示版权信息
	bool ShowCopyRight(bool bShow=true);

	//内部函数
private:
	//调整大小
	bool FixControlStation();
	//绘画框架
	void DrawFrame(CDC * pDC);
	//消息函数 
public:
	//重画函数
	afx_msg void OnPaint();
	//位置消息
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//鼠标按下消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//得到焦点
	afx_msg void OnSetFocus(CWnd * pOldWnd);

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	afx_msg void OnTimer(UINT nIDEvent);
	LRESULT OnLoadSkin(WPARAM wparam, LPARAM lparam);

	LRESULT OnExchangeSkin(WPARAM wparam, LPARAM lparam);
	
public:
	void LoadSkin(void);
public:
	void SetAd(TCHAR ad[]);
	//调整窗口位置
	void AutoSize(int cx,int cy);
private:
	//换肤
	CSkinMgr m_skinmgr;
};

#endif