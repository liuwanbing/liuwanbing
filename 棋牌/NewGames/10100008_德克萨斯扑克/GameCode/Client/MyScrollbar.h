#ifndef _MY_GAME_SET_DLG_H_
#define _MY_GAME_SET_DLG_H_

#include "stdafx.h"
#include "Resource.h"

#define IDD_MUSIC_SCROLLBAR  100
#define IDD_SOUND_SCROLLBAR  101
#define IDD_SCROLLBAR_SLIDE  102

class MyScrollBar 
{
public:
	/** 
	 * 当前按钮的状态
	 */
	enum ButtonState
	{
		BUTTONSTATE_NORMAL = 0,          /// 正常状态
		BUTTONSTATE_HOVER,               /// 鼠标在其上的状态
		BUTTONSTATE_DOWND,               /// 按下状态
		BUTTONSTATE_NULL,
	};

	/// 构造函数
	MyScrollBar();
	/// 析构函数
	~MyScrollBar();

	/// 导入图片文件
	void LoadImage(CString filepath);
	/// 设置滑动条的显示位置
	void SetDisplayPosition(CPoint pos) 
	{ 
		m_DisplayPos = pos; 
	}
	/// 得到滑动条的显示位置
	CPoint GetDisplayPosition(void) { return m_DisplayPos; }
	/// 设置当前滑条的状态
	inline void SetState(MyScrollBar::ButtonState state) { m_ButtonState = state; }
	/// 得到当前滑条的状态
	inline MyScrollBar::ButtonState GetState(void) { return m_ButtonState; }
	/// 画滚动条
	void Draw(CDC *pDC,int x,int y);
	/// 事件处理
	bool OnEvent(MSG *pMsg);
	/// 检测是否在这个控件中
	bool IsInside(CPoint pos);

private:
	ButtonState m_ButtonState;             /**< 当前按钮的状态 */
	CGameImage  m_buttonImage;             /**< 按钮使用的图片 */
	CPoint      m_DisplayPos;              /**< 滑动条的显示位置 */
};

class MyGameScrollbar : public CWnd
{
public:
	/**
	 * 控制条的类型
	 */
	enum ScrollBarType
	{
		SCROLL_TYPE_MUSIC = 0,          /// 音乐
		SOROLL_TYPE_SOUND,              /// 音效
		SOROLL_TYPE_NULL               
	};

	/// 构造函数
	MyGameScrollbar();
	/// 析构函数
	virtual ~MyGameScrollbar();

	/// 得到当前滚动条的百分比
	float GetCurrentProcess(void);
	/// 设置当前滚动条的百分比
	void SetCurrentProcess(float pro);
	/// 设置滚动条类型
	inline void SetType(MyGameScrollbar::ScrollBarType type)
	{
		m_ScrollBarType = type;	
	}
	/// 得到滚动条类型
	inline MyGameScrollbar::ScrollBarType GetType(void)
	{
		return m_ScrollBarType;
	}

private:
	/// 画滚动条
	void DrawScrollbar(CDC *pDC);

private:
	MyScrollBar			           m_btslide;                      /**< 滑条 */
	bool                           m_IsMoving;                     /**< 是否移动滑条 */
	int                            m_SlideDisPos;                  /**< 滑条的当前显示位置 */

	CDC							   m_memDC;
	bool						   m_bDCStored;		           /**< 是否已经保存背景图 */

	CGameImage                     m_ImageFirst;                  /**< 画滚动条前端  */
	CGameImage                     m_ImageMiddle;                 /**< 画滚动条中端 */
	CGameImage                     m_ImageLast;					  /**< 画滚动条后端 */	

	ScrollBarType                  m_ScrollBarType;               /**< 滚动条的类型 */

protected:
	///消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	/// 鼠标离开
	afx_msg LRESULT OnMouseLeave(WPARAM wParam,LPARAM lParam);  

	DECLARE_MESSAGE_MAP()
};

class MyGameSetDlg : public CLoadFileBmp
{
public:
	/// 构造函数
	MyGameSetDlg(CWnd* pParent=NULL);
	/// 析构函数
	virtual ~MyGameSetDlg();

	/// 是否播放音乐
	inline bool IsPlayMusic(void)
	{
		return m_btCheckMusic.GetCheck() > 0 ? true : false;
	}
	/// 是否播放音效
	inline bool IsPlaySound(void)
	{
		return m_btCheckSound.GetCheck() > 0 ? true : false;		
	}
	/// 得到音乐播放的大小
	inline float GetMusicVolume(void)
	{
		return m_MusicScrollbar.GetCurrentProcess();
	}
	/// 得到音效播放的大小
	inline float GetSoundVolume(void)
	{
		return m_SoundScrollbar.GetCurrentProcess();
	}

	/// 导入声音配置文件
	void LoadConfig(CString path);
	/// 保存声音配置文件
	void SaveConfig(void);

	///初始化函数
	virtual BOOL OnInitDialog();

	enum { IDD = IDD_GAME_SET };

private:
	CGameImage                             m_imgBk;

	CNormalBitmapButtonEx                  m_btOk;
	CNormalBitmapButtonEx                  m_btCancel;
	CButton                                m_btCheckMusic;
	CButton                                m_btCheckSound;
	MyGameScrollbar                        m_MusicScrollbar;
	MyGameScrollbar                        m_SoundScrollbar;
	CString                                m_SoundConfig;

protected:
	///DDX/DDV 支持
	virtual void DoDataExchange(CDataExchange * pDX); 
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();

	///消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	DECLARE_MESSAGE_MAP()
};

#endif
