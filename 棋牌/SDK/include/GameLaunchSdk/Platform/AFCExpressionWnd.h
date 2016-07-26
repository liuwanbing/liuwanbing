#ifndef CAFCEXPRESSIONWND_HEAD_FILE
#define CAFCEXPRESSIONWND_HEAD_FILE

#include "PublicDefine.h"
#include "AFCResource.h"

//名字空间
//using namespace AFC;
//using namespace AFC::AFCResource;

//宏定义
#define MAX_EXPRESS_ITEM			68										//最大表情数目
#define WM_HIT_EXPMESSTION			WM_USER+11								//按动表情标志
#define	WM_HITDESK					WM_USER+12								//比赛时点击桌子
#define WM_INVITE_PLAY				WM_USER+113								//邀请
#define WM_CHATING_WITH					WM_USER+114								//聊天
#define WM_WATCH_GAME					WM_USER+115								//旁观

//表情子项
class EXT_CLASS CTrueItem//CTrueItem
{
	//变量定义
public:
	CBitmap							m_ExpPicImage;							//表情图片
	UINT							m_uTranceLength;						//解释长度
	TCHAR							m_szTrancelate[11];						//解释字符
	TCHAR							m_szChineseName[11];					//中文解释

	//函数定义
public:
	//构造函数
	CTrueItem();
	//析构函数
	~CTrueItem();
	//输入解释
	void SetExpressInfo(TCHAR * szChineseName, TCHAR * szTrancelate, UINT uExpPicID);
};

//表情动作窗口类
class EXT_CLASS CGameTalkText/*CGameTalkText*/ : public CDialog
{
	//变量定义
protected:
	UINT							m_uSelectIndex;							//选择索引

	//父窗口句柄
	HWND						m_hParentHWND;

	//全局变量
protected:
	static UINT						m_uMaxRow;								//最大列数目
	static UINT						m_uMaxLine;								//最大行数目

public:
	static CTrueItem			m_ExpreesionItem[MAX_EXPRESS_ITEM];		//表情子项

	//函数定义
public:
	//构造函数
	CGameTalkText();
	//析构函数
	virtual ~CGameTalkText();

	//功能函数
public:
	//建立函数
	//建立函数
	bool CreateExpressionWnd(CWnd * pParentWnd, HWND pParentHWND = NULL);
	//获取适合大小
	bool GetFixSize(CSize & WndSize);
	//获取适合大小
	bool GetFixSize(SIZE & WndSize);
	//获取表情
	static CTrueItem * GetExpressItem(UINT uIndex);

	//消息映射
protected:
	DECLARE_MESSAGE_MAP()

	//辅助函数
protected:
	//初始化资源
	static bool InitExpressItem();
	//绘画头像
	void DrawItem(CDC * pDC, UINT iXPos, UINT iYPos, bool bSelect);
	//按键测试
	UINT HitItemTest(CPoint MousePoint);

	//消息函数
public:
	//重画消息
	afx_msg void OnPaint();
	//失去焦点消息
	afx_msg void OnKillFocus(CWnd * pNewWnd);
	//鼠标移动消息
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//鼠标按下消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

#endif