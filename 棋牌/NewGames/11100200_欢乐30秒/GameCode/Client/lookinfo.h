#pragma once
#include "Resource.h"

/// Clookinfo 对话框,显示路子信息
/// 按两种方式显示路子信息
class Clookinfo : public CDialog
{
	DECLARE_DYNAMIC(Clookinfo)

public:
	Clookinfo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~Clookinfo();

// 对话框数据
	enum { IDD = IDD_LUZI };

	bool			bMouseDown;
public:
	/// 背景图
	CGameImage				m_bk;				
	//CNormalBitmapButtonEx   m_BmCancel;			// 按钮
	//CNormalBitmapButtonEx   m_BmOk;				// 按钮

	//////////////////////////////////////////////////////////
	CGameImage  m_imgheduizi;
	CGameImage  m_imgzhuangduizi;
	CGameImage  m_imgxianduizi;

	CGameImage  m_imghe1_dot;
	CGameImage  m_imgzhuang1_dot;
	CGameImage  m_imgxian1_dot;



	/////////////////////////////////////////////////////////
	/// 庄小图
	CGameImage				m_imgzhuang;			
	/// 庄大图
	CGameImage				m_imgzhuang1;		
	/// 闲小图
	CGameImage				m_imgxian;			
		/// 闲大图
	CGameImage				m_imgxian1;		
	/// 和小图
	CGameImage				m_imghe;			
	/// 和大图
	CGameImage				m_imghe1;			

	///为72把一局新添加按钮图片/// 第一页
	CNormalBitmapButtonEx				m_BtnYiye;				
	/// 第二页图片
	CNormalBitmapButtonEx				m_BtnErye;				
	/// 关闭按钮
	CNormalBitmapButtonEx				m_BtnClose;			

	///大图基础位置
	CPoint      m_pDaTu;
	///小图基础位置
	CPoint		m_pXiaoTu;

	///大图距离
	int         m_iDaWidth;
	///小图距离
	int         m_iXiaoWidth;

	///庄家赢，为了用整数保存放大100被，绘制是前后两为用小数点隔开
	int			m_iZwin;
	///闲家赢
	int			m_iXwin;
	///和赢
	int			m_iHwin;
	///庄对子
	int			m_iZDuizi;
	///闲对子
	int         m_iXDuizi;
	///庄天王
	int			m_iZTianwan;
	///闲天王
	int			m_iXTianwang;
	///保存游戏的总局数（>=1 并且 <=30）
	int			m_iPlayCount;
	///最近开牌的赢方：0没有使用的数据，1，庄，2，闲，3和
	int			m_iWinType[MAXCOUNT];
	///小路子的每一列数据
	BYTE		m_bySLuziData[72][6];
	///是否显示下一页的图片
	bool		m_bShowNext;

	int			m_iNowJuShu;				//本局进行的局数，进行到第几局（72把一局）

	int         m_iGamePictureType; 

	//当前已经有多少列已经有值了
	int         m_iLuziCol;


	int		m_iZPaiXing[5];			///<庄家牌型,元素0前两张牌的值，元素1总牌值，元素2天王，元素3对子，元素4和
	int		m_iXPaiXing[5];			///<闲家牌型
	
	///最近开牌的赢方：是否开的是对子
	///小路子的每一列数据
	int		m_bKaiPaiData[72][6];
	int     m_iKaiPaiType[MAXCOUNT];

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	///消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//初始化函数
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
public:
//	afx_msg void OnBnClickedOk();
//public:
//	afx_msg void OnBnClickedCancel();
public:
//	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
public:
	///初始化数据
	void Init();
public:
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
public:
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	///绘制路子
	void DrawLuzi(CDC*pDC);
	///数据转换,将路子数据转换成数组形式lym2010-03-18
	void ChangeData();

public:
	afx_msg void OnBnClickedButYiye();
public:
	afx_msg void OnBnClickedButErye();
public:
	afx_msg void OnBnClickedButClose();
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
