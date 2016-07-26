#pragma once

#ifndef AFCDIALOG_HEAD_FILE
#define AFCDIALOG_HEAD_FILE

#include "StdAfx.h"

using namespace AFC::AFCImage;

namespace AFC
{
	namespace AFCDialog
	{
		/*******************************************************************************************************/

		//基础对话框类
		class EXT_CLASS CAFCBaseDialog : public CDialog
		{
			//函数定义
		public:
			//构造函数
			CAFCBaseDialog(UINT uTemplate, CWnd * pParent=NULL);
			//析构函数
			virtual ~CAFCBaseDialog();
			//初始化函数
			virtual BOOL OnInitDialog();

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

			//消息映射
		protected:
			DECLARE_MESSAGE_MAP()

			//功能函数
		protected:
			//绘画背景
			void DrawBackFace(CDC * pDC, int iXPos, int iYPos, int iWdith, int iHeight);

			//消息函数
		public:
			//重画函数
			afx_msg void OnPaint();
			//控件颜色消息
			LRESULT OnControlColor(WPARAM wparam, LPARAM lparam);
		};

		/*******************************************************************************************************/

		//皮肤对话框类
		class EXT_CLASS CAFCSkinDialog : public CAFCBaseDialog
		{
			//变量定义
		private:
			bool						m_bMaxSize;						//最大化
			bool						m_bTitleBar;					//标题标志
			HICON						m_hIcon;						//图标句柄
			BYTE						m_bNowDownButton;				//按下按钮 
			BYTE						m_bNowHotButton;				//现在焦点
			BYTE						m_ButtonState[3];				//按钮状态
			CRect						m_rcButton[3];					//按钮位置
			CRect						m_rcNormalSize;					//普通位置
			CString						m_strTitleText;					//窗口文字

			//函数定义
		public:
			//构造函数
			CAFCSkinDialog(UINT uTemplate, CWnd * pParent=NULL);
			//析构函数
			virtual ~CAFCSkinDialog();
			//初始化函数
			virtual BOOL OnInitDialog();

			//功能函数
		public:
			//显示最大化窗口 
			void ShowMaxSize();
			//显示还原窗口 
			void ShowRestoreSize();
			//设置图标
			bool SetDialogIcon(HICON hIcon, BOOL bBigIcon);
			//设置标题
			bool SetWindowTitle(const TCHAR * szTitle);
			//设置标题
			bool EnableTitleBar(bool bEnableBar);
			//获取标题高度
			UINT GetTitleHight();

			//消息映射
		protected:
			DECLARE_MESSAGE_MAP()

			//内部函数
		protected:
			//绘画框架
			void DrawFrame(CDC * pDC, CRect & ClientRect);
			//绘画标题栏
			void DrawTitleBar(CDC * pDC, CRect & ClientRect);
			//绘画按钮
			void DrawCloseButton(CDC * pDC);
			//绘画按钮
			void DrawMaxRestoreButton(CDC * pDC);
			//绘画按钮
			void DrawMinButton(CDC * pDC);
			//更新标题
			bool UpdateTitleBar();

			//消息函数
		public:
			//重画函数
			afx_msg void OnPaint();
			//鼠标移动消息
			afx_msg void OnMouseMove(UINT nFlags, CPoint point);
			//鼠标右键按下消息
			afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
			//鼠标右键按起消息
			afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
			//鼠标双击消息
			afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
			//位置变化 
			afx_msg void OnSize(UINT nType, int cx, int cy);
			//窗口销毁
			afx_msg void OnDestroy();
		};

		/*******************************************************************************************************/

		//区域对话框类
		class EXT_CLASS CAFCRgnDialog : public CAFCBaseDialog
		{
			//变量定义
		protected:
			CRgn						m_DialogRgn;					//对话框区域
			CAFCImage					m_BackBitmap;					//对话框背景图

			//函数定义
		public:
			//构造函数
			CAFCRgnDialog(UINT uTemplate, CWnd * pParent=NULL);
			//析构函数
			virtual ~CAFCRgnDialog();
			//初始化函数
			virtual BOOL OnInitDialog();

			//消息映射
		protected:
			DECLARE_MESSAGE_MAP()

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
		};

		/*******************************************************************************************************/
	};
};

#endif