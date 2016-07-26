#ifndef _MY_LIST_CTRL_H_INCLUDE
#define _MY_LIST_CTRL_H_INCLUDE

//宏定义
#define USER_STATE_COUNT						6						//状态数目
#define USER_STATE_WIDTH						27						//状态宽度
#define	USER_STATE_HEIGHT						17						//状态高度

#include <string>
#include <vector>
#include <map>

/**
 * 列表头
 */
class CComBodyClassT : public CHeaderCtrl
{
public:
	/// 构造函数
	CComBodyClassT();
	/// 析构函数
	virtual ~CComBodyClassT();

protected:
	int			m_Type;		//界面类型,两种
	CImage  m_bkimg;
	CImage  m_bkimg2;
	COLORREF    m_color;
	COLORREF    m_textcolor;
	CFont                       m_Font;                         /**< 当前标签使用的字体 */

public:
	void SetBkImage(HINSTANCE hInstance,const char *path,const char *path2,COLORREF color,COLORREF tcolor, int type=0);

protected:
	/// 重画函数
	afx_msg void OnPaint();
	/// 背景函数
	afx_msg BOOL OnEraseBkgnd(CDC * pDC);

	DECLARE_MESSAGE_MAP()
};

/**
 * 列表视图基础类
 */
class CHappyGoDlgT : public CListCtrl
{
public:
	/**
	 * 列表头结构
	 */
	struct ListHeader
	{
		std::string m_Name;              // 列名称
		int m_Fmt;                       // 列的显示位置
		int m_Width;                     // 列的显示宽度
	};

	/**
	 * 每行的头结构
	 */
	struct ItemHeader
	{
		std::string m_Name;              // 行头的名称
		int m_ImageId;                   // 这行使用的图片ID
	};

	static CImageList           m_UserStateList;

protected:
	CComBodyClassT               m_ListHeader;
	COLORREF                    m_bkColor;
	COLORREF                    m_TextColor;
	COLORREF                    m_FocusbkColor;
	COLORREF                    m_FocusTextColor;
	std::vector<ListHeader>     m_ListHeaderData;
	std::vector<ItemHeader>     m_ItemHeaderData;
	CFont                       m_Font;                         /**< 当前标签使用的字体 */
	CImage                      m_bgImage;                      /**< 背景图片 */
	CImage                      m_probgImage,m_proImage;        /**< 滚动条图片 */
	std::map<int,int> m_ProgressList;
	int                         m_processpos;
	int                         m_sudu;
	int                         m_curSel;

public:
	/// 构造函数
	CHappyGoDlgT();
	/// 析构函数
	virtual ~CHappyGoDlgT();
	/// 设置要显示的背景图片
	void SetBgImage(HINSTANCE hInstance,const char *path);
	/// 设置滚动条位置
	inline void setProcessPos(int pos) { m_processpos = pos; }
	/// 设置滚动条资源文件
	void SetProcessImage(HINSTANCE hInstance,const char *path,const char *path2);
	/// 初始列表控件头属性
	void InitListHeader(HINSTANCE hInstance,const char *path,const char *path2,COLORREF color,COLORREF tcolor, int type=0);
	/// 初始列表属性
	void InitListCtrl(COLORREF bkColor,COLORREF TextColor,COLORREF FocusbkColor,COLORREF FocusTextColor,
		              std::string resourceimage);
	/// 解析列表的头
	void ParseListHeader(std::string mHeader);
	/// 添加一个行头
	void AddItemHeader(int pImageId,std::string pName);
	/// 更新行头
	void UpdateItemHeader(int pIndex,int pImageId,std::string pName);
	/// 设置指定行的进度
	void SetProcess(int row,float pro);
	/// 设置下载文件的大小
	void SetFileSize(int row,int size);
	/// 设置下载文件的速度
	void SetFileDownload(int row,int size);
	/// 刷新选择的控件
	void RefreshSelectItem(void);

protected:
	//获取颜色
	virtual bool GetDrawColor(COLORREF & crTextColor, COLORREF & crBackColor, UINT uRow, bool bSelect);
	//控件绑定
	virtual void PreSubclassWindow();
	//绘画函数
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

protected:
	//背景函数
	afx_msg BOOL OnEraseBkgnd(CDC * pDC);
	afx_msg void OnPaint();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT nIDEvent);

	DECLARE_MESSAGE_MAP()
};

#endif
