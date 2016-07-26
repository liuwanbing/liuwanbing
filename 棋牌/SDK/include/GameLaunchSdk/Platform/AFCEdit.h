#ifndef AFCEDIT_HEAD_FILE
#define AFCEDIT_HEAD_FILE

#include "dlgDiyChat.h"

//消息定义
#define WM_RICH_EDIT_HIT					WM_USER+27						//按动文字消息
#define WM_RELOAD_TALKINI					WM_USER+28						//重读talk.ini

/*namespace AFC
{
	namespace AFCEdit
	{*/
		/*******************************************************************************************************/

		//文本输入控件类
		class EXT_CLASS CTrueEdit/*C-AFCEdit*/ : public CEdit
		{
			//变量定义
		protected:
			bool							m_bFocus;						//是否得到焦点

			//函数定义
		public:
			//构造函数
			CTrueEdit();
			//析构函数
			virtual ~CTrueEdit();

			//功能函数
		public:
			//设置颜色
			static void SetTextColor(COLORREF crFocus=RGB(255,255,255), COLORREF crNoFocue=RGB(255,255,255), COLORREF crDisble=RGB(255,0,0));
			//设置颜色
			static void SetBKColor(COLORREF crFocus=RGB(255,255,0), COLORREF crNoFocue=RGB(255,255,255), COLORREF crDisble=RGB(255,255,255));

		protected:
			DECLARE_MESSAGE_MAP()

			//消息函数 
		protected:
			//绘画控件
			afx_msg HBRUSH CtlColor(CDC * pDC, UINT nCtlColor);
			//得到焦点
			afx_msg void OnSetFocus(CWnd * pOldWnd);
			//失去焦点
			afx_msg void OnKillFocus(CWnd * pNewWnd);
		};

		/*******************************************************************************************************/

		//密码输入框
		class EXT_CLASS CTrueEditEx/*C-AFCPassWordEdit*/ : public CTrueEdit
		{
			//变量定义
		protected:
			bool							m_bProtected;					//是否允许得到密码

			//函数定义
		public:
			//构造函数
			CTrueEditEx();
			//析构函数
			virtual ~CTrueEditEx();
			//窗口函数
			virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

			//功能函数
		public:
			//是否保护状态
			bool IsPortected();
			//设置保护
			bool SetProtected(bool bProtected);
			//获取密码
			int GetEditPassWord(TCHAR * szBuffer, UINT uBufferLength);

			//消息映射
		protected:
			DECLARE_MESSAGE_MAP()
		};

		/*******************************************************************************************************/

		//图像 OLE 类
		class CImageDataObject : IDataObject
		{
			//变量定义
		private:
			bool								m_bRelease;						//需要释放
			ULONG								m_ulRefCnt;						//引用计数
			STGMEDIUM 							m_Stgmed;						//结构信息
			FORMATETC 							m_Format;						//结构信息

			//函数定义 
		public:
			//构造函数
			CImageDataObject();
			//析构函数
			~CImageDataObject();

			//功能函数
		public:
			//设置图片
			void SetBitmap(HBITMAP hBitmap);
			//获取对象
			IOleObject * GetOleObject(IOleClientSite * pOleClientSite, IStorage * pStorage);
			//插入图片
			static bool InsertBitmap(IRichEditOle * pRichEditOle, HBITMAP hBitmap);

			//接口函数
		public:
			//查询接口
			STDMETHOD(QueryInterface)(REFIID iid, void * * ppvObject)
			{
				if (iid==IID_IUnknown||iid==IID_IDataObject)
				{
					*ppvObject=this;
					AddRef();
					return S_OK;
				}
				return E_NOINTERFACE;
			}
			//增加引用
			STDMETHOD_(ULONG, AddRef)(void)
			{
				m_ulRefCnt++;
				return m_ulRefCnt;
			}
			//释放对象
			STDMETHOD_(ULONG, Release)(void)
			{
				if (--m_ulRefCnt==0) delete this;
				return m_ulRefCnt;
			}
			//设置数据
			STDMETHOD(SetData)(FORMATETC * pformatetc, STGMEDIUM * pmedium, BOOL  fRelease) 
			{
				m_Format=*pformatetc;
				m_Stgmed=*pmedium;
				return S_OK;
			}
			//获取数据
			STDMETHOD(GetData)(FORMATETC * pformatetcIn, STGMEDIUM * pmedium) 
			{
				HANDLE hDst =::OleDuplicateData(m_Stgmed.hBitmap,CF_BITMAP,NULL);
				if (hDst==NULL) return E_HANDLE;
				pmedium->tymed=TYMED_GDI;
				pmedium->hBitmap=(HBITMAP)hDst;
				pmedium->pUnkForRelease=NULL;
				return S_OK;
			}
			STDMETHOD(EnumFormatEtc)(DWORD dwDirection, IEnumFORMATETC * *  ppenumFormatEtc ) {	return E_NOTIMPL; }
			STDMETHOD(DAdvise)(FORMATETC * pformatetc, DWORD advf, IAdviseSink * pAdvSink,DWORD * pdwConnection) { return E_NOTIMPL; }
			STDMETHOD(DUnadvise)(DWORD dwConnection) { return E_NOTIMPL; }
			STDMETHOD(EnumDAdvise)(IEnumSTATDATA * * ppenumAdvise) { return E_NOTIMPL; }
			STDMETHOD(GetDataHere)(FORMATETC * pformatetc, STGMEDIUM *  pmedium ) { return E_NOTIMPL;	}
			STDMETHOD(QueryGetData)(FORMATETC *  pformatetc) { return E_NOTIMPL; }
			STDMETHOD(GetCanonicalFormatEtc)(FORMATETC * pformatectIn, FORMATETC * pformatetcOut) { return E_NOTIMPL; }
		};

		/*******************************************************************************************************/

		//消息框类
		class EXT_CLASS CNewMiniDlg/*C-AFCRichEdit*/ : public CRichEditCtrl
		{
			//变量定义
		protected:
			bool								m_bMenu;						//是否使用菜单
			bool								m_bAuto;						//是否自动滚动
			HCURSOR								m_Cursor;						//光标
			CString								m_FontName;						//字体名字
			IRichEditOle						* m_pRichEditOle;				//OLE 指针
			bool								m_bCanInsert;
			int									m_GameOrHall;					//大厅和房间保存不同的ini文件，1是游戏房间，0是大厅

		public:
			CDlgDIYChat							m_DiyChat;						//修改聊天常用语对话框
			//函数定义
		public:
			//构造函数
			CNewMiniDlg();
			//析构函数
			virtual ~CNewMiniDlg();

			//功能函数 
		public:
			DWORD							m_bkColor;
			//初始化函数
			bool Init();
			//初始化滚动条皮肤
			void LoadScrollSkin();
			//大厅和房间保存不同的ini文件，1是游戏房间，0是大厅
			void IsGameMessage(){m_GameOrHall=1;};
			void IsHallMessage(){m_GameOrHall=0;};
			//是否自动滚动
			bool IsAutoScroll() { return m_bAuto; };
			//回调函数
			static DWORD CALLBACK SaveCallBack(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG * pcb);

			//加入信息
		public:
			//插入图片
			void InsertBitmap(HBITMAP bBitmap);
			//复制dc到位图
			HBITMAP CopyDCToBitmap(HDC hSrc,int x1,int y1,int x2,int y2);
			//复制位图
			HBITMAP CopyBitmap(HBITMAP   hSourceHbitmap);  
			//插入字符
			void InsertCharString(const TCHAR * szCharString, COLORREF crColor, bool bLink=false, UINT uSize=0, TCHAR * szFontName=NULL,bool bBold=false);

			//消息函数
		public:
			//右键消息
			afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
			//设置光标
			afx_msg BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message);
			//复制
			afx_msg void OnCopy() { Copy(); }
			//全部选择
			afx_msg void OnSelectAll() { SetSel(0, -1); }
			//清除
			afx_msg void OnClear() { ReplaceSel(TEXT("")); }
			//滚动
			afx_msg void OnAutoScroll() { m_bAuto=!m_bAuto; }
			//清除屏幕
			afx_msg void OnClearAll() { SetSel(0,GetWindowTextLength()); ReplaceSel(NULL); }
			//停止显示屏幕上的聊天信息
			afx_msg void OnStopAll() { m_bCanInsert=!m_bCanInsert; }
			//保存
			afx_msg void OnSave();
			//设置字体颜色
			void OnSetColor();
			//自定义聊天用语
			void OnDIYChat();
			//按下连接
			afx_msg void OnEnLink(NMHDR *pNMHDR, LRESULT *pResult);
			//窗口销毁
			afx_msg void OnDestroy();

			afx_msg void OnSize(UINT nType, int cx, int cy);

			BOOL ShowWindow(int nCmdShow);

			DECLARE_MESSAGE_MAP()

		private:
			CSkinMgr	m_skinmgr;

		public:
			LRESULT OnExchangeSkin(WPARAM wpara,LPARAM lpara);
		};

		/*******************************************************************************************************/
	/*};
};*/

#endif