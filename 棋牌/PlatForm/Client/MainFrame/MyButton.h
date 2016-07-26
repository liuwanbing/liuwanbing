#ifndef _MY_BUTTON_H_INCLUDE
#define _MY_BUTTON_H_INCLUDE

#include <string>

class BzButton : public CButton
{
public:
	typedef enum state
	{
		NORMAL,
		HOVER,
		DOWN,
		DISABLE
	}state;

	/// 构造函数
	BzButton();
	/// 析构函数
	virtual ~BzButton();

	/// 设置我们要显示的图片
	void SetImage(CString path);
	/// 从资源文件中导入图片
	void LoadImageFromeResource(HINSTANCE hInstance,UINT nIDResource );
	/// 改变按钮的显示位置
	void SetPosition(CPoint pos);
	void SetOwnerDraw(bool IsDraw);
	/// 设置我们要显示的文字
	void SetText(std::string pText) { m_Text = pText; }
	/// 得到我们要显示的文字
	std::string GetText() { return m_Text; }
	/// 设置文字颜色
	void SetTextColor(COLORREF pColor) { m_TextColor = pColor; }
    /// 得到文字的颜色
	DWORD GetTextColor()  { return m_TextColor; }
    /// 检测当前按键是否在按钮的可见范围内
	bool PointInRect(CPoint pPoint);
	/// 清空背景图
	void ClearBackground();

private:
	/// 画按钮
	void DrawButton(void);

protected:
	CImage m_image;            /**< 这个按钮使用的图片 */
	state    m_state;                  /**< 当前按钮的状态 */
	bool     m_bMouseOver;             /**< 鼠标是否经过 */   
	CDC      m_memDC;
	int      m_Mousex,m_Mousey;        /**< 鼠标X和Y */
	int      m_Width,m_Height;         /**< 按钮高宽 */
	bool m_bDCStored,m_isRgn;//是否已经保存背景图
	std::string m_Text;
	COLORREF m_TextColor;

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	DECLARE_MESSAGE_MAP()

protected:
	virtual void PreSubclassWindow();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
};

#endif
