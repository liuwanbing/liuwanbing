#ifndef _TRANSPARENTSTATIC_H_INCLUDE
#define _TRANSPARENTSTATIC_H_INCLUDE

#include <string>
#include <vector>

/**
 * 透明标签类
 */
class CTransparentStaticT : public CWnd
{
public:
	/// 构造函数
    CTransparentStaticT();
	/// 析构函数
	virtual ~CTransparentStaticT();
	
	/// 设置字体显示颜色
	void SetTextColor(COLORREF clrFont);
	/// 得到字体颜色
	COLORREF GetTextColor(void) const { return m_clrFontColor; }
	/// 设置标签显示文字
	void SetMyText(std::string pText); 
	/// 设置标签显示数字
	void SetMyNumber(int pNum);

	/// 清空背景图
	void ClearBackground();

	/// 得到标签显示文字
	std::string GetMyText() { return m_Text; }
	/// 设置标签的对齐方式
	void SetHSnapType(int htype) { m_hSnapType = htype; }
	/// 得到标签的对齐方式
	int GetHSnapType(void) { return m_hSnapType; }
	/// 设置控件在父窗口中的显示位置
	void SetPositonByParents(int sx,int sy) { m_StartX = sx; m_StartY = sy; }
	/// 设置是否多行显示
	inline void SetMultiList(bool isMulti) { m_IsMulitList = isMulti; }
	/// 得到是否多行显示
	inline bool GetMultiList(void) { return m_IsMulitList; }

	CFont      m_Font;                         /**< 当前标签使用的字体 */
private:
	int m_StartX,m_StartY;
	COLORREF  m_clrFontColor;                  /**< 字体显示颜色 */
	std::string m_Text;                        /**< 要显示的文字 */
	std::vector<std::string> m_TextList;       /**< 主要用于多行显示 */
	int FontWidth;                             /**< 字体宽度 */
	CDC      m_memDC;
	bool m_bDCStored,m_isRgn;//是否已经保存背景图
	int   m_hSnapType;
	bool  m_IsMulitList;                      /**< 是否多行显示 */

protected:
	/// 计算从指定位置开始的中文字符的个数
	int GetChineseCharCount(std::string str,int startpos);
	/// 拆分字符串以便于多行显示
	void SplitStringToMulitList(void);
	/// 从位图中建立不规则区域
	HRGN CreateRgnFromFile( HBITMAP hBmp, COLORREF color );

	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	DECLARE_MESSAGE_MAP()
};

#endif
