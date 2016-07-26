#pragma once

#ifndef AFCWINDOW_HEAD_FILE
#define AFCWINDOW_HEAD_FILE

#include "PublicDefine.h"
#include "AFCImage.h"

//using namespace AFC::AFCImageGoGoGo;

//消息定义
#define WM_SPLIT_WND						WM_USER+10						//拆分条消息

/*namespace AFC
{
	namespace AFCWindow
	{*/
		/*******************************************************************************************************/

		//标题空间类
		class EXT_CLASS CWorldEdit/*C-AFCTitleBar*/ : public CStatic
		{
			//变量定义
		protected:
			HICON							m_hIcon;						//图标
			CGameImage						m_TitlePic;						//背景底图

			//函数定义
		public:
			//构造函数
			CWorldEdit();
			//析构函数
			virtual ~CWorldEdit();
			//设置图标
			bool SetTitleIcon(HICON hIcon);

			DECLARE_MESSAGE_MAP()

			//消息函数
		public:
			//重画函数
			afx_msg void OnPaint();
		};

		/*******************************************************************************************************/

		//消息窗口
		class EXT_CLASS CNewMapEx/*C-AFCTipWnd*/ : public CWnd
		{
			//变量定义
		protected:
			CString							m_strMessage;							//提示消息
			CSize							m_FixSize;								//合适大小

			//函数定义 
		public:
			//构造函数
			CNewMapEx() {}
			//析构函数
			virtual ~CNewMapEx() {}

			//功能函数
		public:
			//建立函数
			bool CreateTipWnd(CWnd * pParentWnd, UINT uWndID);
			//设置窗口消息
			CSize SetTipText(const TCHAR * szTipText, bool bFixSize);
			//获取适合大小
			CSize & GetFixSize() { return m_FixSize; };

			DECLARE_MESSAGE_MAP()

			//消息函数
		public:
			//重画函数
			afx_msg void OnPaint();
		};

		/*******************************************************************************************************/

		//拆分条窗口
		class CAFCSplitWnd : public CWnd
		{
			friend class CControlMessage;

			//函数定义 
		protected:
			//构造函数
			CAFCSplitWnd() {}
			//析构函数
			virtual ~CAFCSplitWnd() {}

			DECLARE_MESSAGE_MAP()

			//消息函数
		public:
			//重画函数
			afx_msg void OnPaint();
		};

		//拆分条类
		class EXT_CLASS CControlMessage/*C-AFCSplitBar*/ : public CWnd
		{
			//变量定义
		protected:
			bool						m_bHorSplit;						//是否竖立
			CAFCSplitWnd				m_SplitWnd;							//拆分条窗口
			int							m_iLessPos;							//最小位置
			int							m_iMaxPos;							//最大位置
			CGameImage					m_bkimage;							//背景图片
			CGameImage					m_bkimagel;							//背景图片
			CGameImage					m_bkimager;							//背景图片
			//函数定义
		public:
			//构造函数
			CControlMessage();
			//析构函数
			virtual ~CControlMessage();
			//初始化拆分条
			bool InitSplitBar(UINT uLessPos, UINT uMaxPos, bool bHorSplit);
			void SetbkImage(TCHAR imagepath1[],TCHAR imagepath2[],TCHAR imagepath3[]);
			COLORREF					m_Color;			//表面颜色
			DECLARE_MESSAGE_MAP()

			//消息函数
		public:
			//重画函数
			afx_msg void OnPaint();
			//鼠标移动消息
			afx_msg void OnMouseMove(UINT nFlags, CPoint point);
			//鼠标按下消息
			afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
			//鼠标按起消息
			afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
			//位置消息 
			afx_msg void OnSize(UINT nType, int cx, int cy);
			//设置光标消息
			afx_msg BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message);
			//失去焦点
			afx_msg void OnKillFocus(CWnd * pNewWnd);
		};

		/*******************************************************************************************************/

		//超级连接类
		class EXT_CLASS CHyperLink/*C-AFCHyperLink*/ : public CStatic
		{
			//变量定义
		protected:
			bool							m_bOverControl;					//是否越过
			bool							m_bVisited;						//是否访问
			bool							m_bUnderline;					//是否画线
			bool							m_bAdjustToFit;					//是否调整
			CFont							m_Font;							//显示字体
			HCURSOR							m_hLinkCursor;					//鼠标光标
			CString							m_strURL;						//连接地址
			COLORREF						m_crLinkColour;					//连接颜色
			COLORREF						m_crVisitedColour;				//访问颜色
			COLORREF						m_crHoverColour;				//越过颜色
			CToolTipCtrl					m_ToolTip;						//提示控件

			//函数定义
		public:
			//构造函数
			CHyperLink();
			//析构函数
			virtual ~CHyperLink();

			//功能函数
		public:
			//设置超级连接
			void SetURL(CString strURL);
			//获取超级连接
			CString GetURL() const;
			//设置颜色
			void SetColours(COLORREF crLinkColour, COLORREF crVisitedColour, COLORREF crHoverColour = -1);
			//获取颜色
			COLORREF GetLinkColour() const;
			//获取颜色
			COLORREF GetVisitedColour() const;
			//获取颜色
			COLORREF GetHoverColour() const;
			//设置是否访问		
			void SetVisited(bool bVisited=true);
			//获取是否访问		
			bool GetVisited() const;
			//设置光标
			void SetLinkCursor(HCURSOR hCursor);
			//获取光标
			HCURSOR GetLinkCursor() const;
			//设置默认光标
			void SetDefaultCursor();
			//设置是否有下画线
			void SetUnderline(bool bUnderline=true);
			//获取是否有下画线
			bool GetUnderline() const;
			//设置是否自动调节大小
			void SetAutoSize(bool bAutoSize=true);
			//获取是否自动调节大小
			bool GetAutoSize() const;
			//转到网页
			HINSTANCE GotoURL(const TCHAR * szURL, int iShowCmd);
			//绘画函数
			//virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
			
			afx_msg void OnPaint();
			//内部函数
		protected:
			//报告错误
			void ReportError(int iError);
			//获取注册表数值
			long int GetRegKey(HKEY hRegKey, TCHAR * szSubKey, TCHAR * szRetData);
			//移动窗口
			void PositionWindow();
			//窗口子类化函数 
			virtual void PreSubclassWindow();
			//消息解释函数
			virtual BOOL PreTranslateMessage(MSG * pMsg);

			//消息函数
		protected:
			//设置颜色消息
			afx_msg HBRUSH CtlColor(CDC * pDC, UINT nCtlColor);
			//设置光标消息
			afx_msg BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message);
			//鼠标移动消息
			afx_msg void OnMouseMove(UINT nFlags, CPoint point);
			//单击消息
			afx_msg void OnClicked();

			DECLARE_MESSAGE_MAP()
		};

		/*******************************************************************************************************/

		//区域对话框类
		class EXT_CLASS CBitmapRgnWindow/*C-AFCRgnWindow*/ : public CWnd
		{
			//变量定义
		protected:
			CRgn						m_WindowRgn;					//窗口区域
			CGameImage					m_BackBitmap;					//窗口背景

			//函数定义
		public:
			//构造函数
			CBitmapRgnWindow();
			//析构函数
			virtual ~CBitmapRgnWindow();

			//消息映射
		protected:
			DECLARE_MESSAGE_MAP()

			//功能函数
		public:
			//获取窗口
			bool SafeGetClientRect(UINT uID, CRect & ClientRect);
			//获取窗口
			bool SafeGetClientRect(CWnd * pControlWnd, CRect & ClientRect);
			//移动窗口
			bool SafeMoveWindow(UINT uID, int x, int y, int iWidth, int iHeight);
			//移动窗口
			bool SafeMoveWindow(CWnd * pControlWnd, int x, int y, int iWidth, int iHeight);

			//功能函数
		public:
			//加载位图
			bool LoadDialogBitmap(HINSTANCE hInstance, UINT uBitmapID, COLORREF crTransparentColor=RGB(0,0,0), COLORREF crTolerance=RGB(0,0,0));
			//加载位图
			bool LoadDialogBitmap(TCHAR * szFileName, COLORREF crTransparentColor=RGB(0,0,0), COLORREF crTolerance=RGB(0,0,0));

			//消息函数
		public:
			//重画函数
			afx_msg void OnPaint();
			//鼠标右键按下消息
			afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
			//建立消息
			afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		};

		/*******************************************************************************************************/
	/*};
};*/

#endif