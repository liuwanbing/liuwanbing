#ifndef CAFCGAMELISTWND_HEAD_FILE
#define CAFCGAMELISTWND_HEAD_FILE

#include "GameListCtrl.h"
#include "GamePlaceMessage.h"
#include "PersonalSet.h"
#include "IMMain.h"
#include "UserInfoWnd.h"
#include "DUIUserTabWnd.h"
#include "FinanceWnd.h"
#include "LogoChangeDlg.h"
#include "LeftWnd.h"

#include "IMWnd.h"
#include "SkinMgr.h"
#include <queue>
using namespace std;

#define WM_LOADSKIN					WM_USER+2
#define IDM_SKIN					WM_USER+3
#define IDM_HIDE_LEFT				WM_USER+4

#define MESSAGE_TITLE					TEXT("系统消息")		//游戏消息标题

//#define IDM_HIDE_RIGHT				WM_USER+5
class  CGameListWnd:public CWnd
{
	bool m_bInit;
	CToolTipCtrl							m_toolTip;
	bool									bAddLogoTool;
	//信息变量
protected:
	CGameImage								m_gamelistlc,m_gamelistrc,m_gamelisttc,m_gamelistbc;	//底部背景
	CGameImage								m_gamelistlt,m_gamelistlb,m_gamelistrt,m_gamelistrb;	//四角
	CGameImage								m_gamelistAdorn;						
	//装饰用马钉
public:
	CLeftWnd								*m_pLeftWnd;
	CGameListCtrl							m_GameList;												//游戏列表
	CIMMain									m_IMList;												//游戏列表
	CGameListCtrl							m_HelpList;												//服务中心
	CPersonalSet							*personalSet;
	CRect									logoRect;
	MSG_GP_R_LogonResult					*m_PlaceUserInfo;										//用户登陆资料
	CUserInfoWnd*							m_pUserInfoWnd;											//用户资料管理框
	CUserTabDuWnd							m_UserTabDuWnd;
	CFinanceWnd*							m_pFinanceWnd;											//财务状况窗口
	CLogoChangeDlg*							m_pLogoChangeWnd;										//更换头像
	CIMWnd									*m_pIMWnd;
	bool									m_bShowEnterRoom;										//用户点击官网的比赛报名按钮，但是如果没有刷新游戏房间则获取房间后要自动进入

	int										m_iShowBtRSpace;										//显示隐藏按扭右则保留的空隙
	CRect									m_rcBtns[5];											//下方按钮群的位置
	CRect									m_rcFinanceBtn;											//财务按钮群的位置

		//////////////////////////////////////////////////////////////////////////
	////////Kylin 20090107 添加功能 TAB 控件
	//CNewGameClass			m_FunListTabWnd;								
	CNormalBitmapButtonEx	m_btGameList;//游戏

	CNormalBitmapButtonNew	m_btUserInfo;//用户信息
	CNormalBitmapButtonNew	m_btIMList;	 //好友
	CNormalBitmapButtonNew	m_btFascX;	 //魅力值兑换
	CNormalBitmapButtonNoState  m_btPLogo;	 //人物头像
	CNormalBitmapButtonNew	m_btMail;	 //邮件
	CNormalBitmapButtonNew	m_btFinance;	 //财务状况

	void OnGameList();
	void OnUserInfo();
	void OnIMList();
	void OnFacExchange();
	void OnMailClick();
	void OnLogoClick();
	LRESULT	OnSetLogo(WPARAM wParam, LPARAM lParam);
	CFunListCtrl			m_FunList;								//功能列表
	void FoucsTab(int id);
	afx_msg void OnNMDblclkFunList(NMHDR * pNMHDR, LRESULT * pResult);
	
	//////////////////////////////////////////////////////////////////////////
protected:
	CNormalBitmapButtonEx					m_Bt1;													//
	CNormalBitmapButtonEx					m_Bt2;													//
//	CNormalBitmapButtonEx					m_Bt3;													//
	//CNormalBitmapButtonEx					m_Bt4;													//

	bool									m_bHideList;											//隐藏列表

	CGameImage								m_infolc,m_inforc,m_infotc,m_infobc;	//个人头像框边界
	CGameImage								m_infolt,m_infort,m_infolb,m_inforb;	//个人头像框边界
	CGameImage								m_infol,m_infor,m_infoc;				//人物头像框左右
	CGameImage								m_space;								//间隙
	CGameImage								m_LogoImg,m_PersonalInfo;	//个人头像框和头像
	int										m_infox,m_infoy;			//背景框位置
	///////////////////////////////////////
	///////Kylin 20090105 增加昵称，魅力值y偏移	
	int										m_nickdy,m_meilidy;			//昵称，魅力值y偏移
	///////////////////////////////////////
	
	int										m_picx,m_picy;				//人物头像位置
	WCHAR									m_wszBuffer[MAX_PATH];		//宽字符数组（绘画png）
	COLORREF								m_titlecolor;
	CGameImage								m_leftspace,m_rightspace;												//边际


	queue<TMailItem>						m_Mails;
	//函数定义
public:
	//构造函数
	CGameListWnd();
	//析构函数
	virtual ~CGameListWnd();

	DECLARE_MESSAGE_MAP()

	//功能函数
public:
	//建立函数
	bool CreateWnd(CWnd * pParentWnd, UINT uWndID);
	//内部函数
public:
	//调整大小
	bool FixControlStation();
private:
	//绘画框架
	void DrawFrame(CDC * pDC);
	//消息函数 
public:
	//重画函数
	afx_msg void OnPaint();
	//位置消息
	afx_msg void OnSize(UINT nType, int cx, int cy);
	LRESULT OnLoadSkin(WPARAM wparam, LPARAM lparam);
	LRESULT OnExchangeSkin(WPARAM wparam, LPARAM lparam);
	
private:
	//绘制框架
	void DrawViewFrame(CDC * pDC, int iWidth, int iHeight);
public:
	void LoadSkin(void);
	void ReloadLogo();
	//调整窗口位置
	void AutoSize(int cx,int cy);
	//在大厅调用该函数，进入指定的游戏
	bool EnterAssignRoom(UINT uKindID, UINT uNameID);

	//返回消息条数
	int GetMailCount();
public:
	afx_msg void OnMoving(UINT nSide, LPRECT lpRect);
	//左击游戏列表
	afx_msg void OnNMClickGameList(NMHDR *pNMHDR, LRESULT *pResult);
	//右击游戏列表
	afx_msg void OnNMRclickGameList(NMHDR * pNMHDR, LRESULT * pResult);
	//双击游戏列表
	afx_msg void OnNMDblclkGameList(NMHDR * pNMHDR, LRESULT * pResult);
	//游戏列表项改变
	afx_msg void OnTvnSelchangedGameList(NMHDR * pNMHDR, LRESULT * pResult);
	//列表正在展开
	afx_msg void OnTvnItemexpandingGameList(NMHDR * pNMHDR, LRESULT * pResult);
	//列表正在展开
	afx_msg void OnTvnItemexpandedIMList(NMHDR * pNMHDR, LRESULT * pResult);
	//列表正在展开
	afx_msg void OnNMDblclkHelpList(NMHDR * pNMHDR, LRESULT * pResult);
	////左击IM列表
	//afx_msg void OnNMClickIMList(NMHDR *pNMHDR, LRESULT *pResult);
	////右击IM列表
	//afx_msg void OnNMRclickIMList(NMHDR * pNMHDR, LRESULT * pResult);
	////双击IM列表
	//afx_msg void OnNMDblclkIMList(NMHDR * pNMHDR, LRESULT * pResult);
	//列表信息
	bool OnListMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//按钮
	void OnBnClickedBt1();
	//个人设置
	void OnBnClickedBt2();

	//显示/隐藏个人财务信息
	void OnShowFinance();
	void OnHideFinance();

	
	//void OnBnClickedBt3();
	//
	//void OnBnClickedBt4();
	//菜单命令
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//设置用户信息
	void  SetUserInfoWnd(MSG_GP_R_LogonResult *logoResult);
	//更新个人银行信息
	void UpdateMeInfo(__int64 i64Bank,BOOL IsUpdateBank);
	//更新个人财富
	void UpdateMeFortune(__int64 i64Money,__int64 i64Bank);
	//获取左边是隐藏还是显示
	BOOL GetLeftShowOrHide(){return m_bHideList;}
	/// 强制隐藏列表，并更改状态
	void ShowHideList(bool bHide)
	{
		m_Bt1.EnableWindow(!bHide);
	}

	//JianGuankun 2011.11.19 加载各种类型的游戏列表
	//加载Web游戏
	void AddWebGameToList();
	//加载单机Exe游戏
	void AddExeGameToList();
	//加载单机3D游戏
	void Add3DGameToList();

	/////////////////////////////////////////////////////////

	//获取游戏列表是否被隐藏
	bool GetIsHide();

	void AddMail(TMailItem mail);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

private:
	CSkinMgr m_skinmgr;

	void ChangeSkin();
};

#endif