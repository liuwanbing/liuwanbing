#ifndef CAFCRoomRightWnd_HEAD_FILE
#define CAFCRoomRightWnd_HEAD_FILE



#include "UserListCtrl.h"
#include "ChatControl.h"
#include "WebWnd.h"
#include "afxwin.h"
#include "SkinMgr.h"

#define WM_LOADSKIN					WM_USER+2
#define IDM_SKIN					WM_USER+3
#define IDM_SEND_ROOMMSG			WM_USER+5							//发送房间聊天消息
#define CHAR_HEIGHT					22							//聊天高度
#define IDM_HIDE_RIGHT				WM_USER+10					//隐藏右边
//#define PROP_PEER_PAGE	            4       // PengJiLin, 2010-10-12
#define PROP_PEER_PAGE	            8       // PengJiLin, 2010-10-12
// PengJiLin, 2010-10-12
#define PROP_PEER_PAGE_S	        3

class  CRoomRightWnd:public CGameFaceGo
{
	CRgn		m_Rgn;		///< 绘制区域
	int pannelHeight;		///< 道具按钮一栏的高度
	COLORREF PropBgcolor;
	int	m_curPropPage;
	bool m_bInit;

	/// 道具相关的按钮的横坐标
	int yPropRow1;  ///第一行道具Y位置
	int yPropRow2;  ///第二行道具Y位置
	int xPropBuy;	///< 购买按钮
	int xPropPrev;	///< 前一页按钮
	int xProp1;		///< 第一个道具按钮
	int xProp2;		///< 第二个道具按钮
	int xProp3;		///< 第三个道具按钮
    int xProp4;     // PengJiLin, 2010-10-11, 第四道具按钮
	int xPropNext;	///< 下一页按钮

	/// 子窗口的位置，用来确定各边框的位置
	CRect			m_rcUserList;		///<用户列表所在矩形
	CRect			m_rcTalkMessage;	///<聊天记录所在矩形
	CRect			m_rcAdvertise;		///<广告控件所在矩形
	CRect			m_rcSendTalk;		///<聊天输入框所在矩形


	//信息变量
protected:
	int										m_advHeight;												//广告位的高度
	CString									m_strAdvertise;												//广告地址
	CWebWnd									*webWnd;

	CGameImage								m_userlc,m_userrc,m_usertc,m_userbc;						//玩家例表框架
	CGameImage								m_userlt,m_userrt,m_userlb,m_userrb;						//玩空例表框架四角
	CGameImage								m_talklc,m_talkrc,m_talktc,m_talkbc;						//聊天框架
	CGameImage								m_talklt,m_talkrt,m_talklb,m_talkrb;						//聊天框架四角
	CGameImage								m_rightspace;												//边际
	CGameImage								m_rightspace2;												//边际

	bool									m_bHideList;						//隐藏列表


    // PengJiLin, 2010-9-28
    CGameImage  m_propBackImage;        // 道具栏背景图及其矩形
    CRect       m_propBackRect;


    // PengJiLin, 2010-10-8
    UINT        m_adjPropHeight;        // 道具栏高度调节
    int         m_iBackImageLeft;       // 背景图的位置
    int         m_iBackImageWidth;
    int         m_iBackImageHeight;


    // PengJiLin, 2010-10-8, 商场道具按钮
    CNormalBitmapButtonEx   m_btPropPrev_S;
    CNormalBitmapButtonEx   m_btPropNext_S;
    CNormalBitmapButtonEx   m_btProp1_S;
    CNormalBitmapButtonEx   m_btProp2_S;
    CNormalBitmapButtonEx   m_btProp3_S;

    int m_curPropPage_S;

    // PengJiLin, 2010-10-11, 商场道具位置
    int xPropPrev_S;
    int xProp1_S;
    int xProp2_S;
    int xProp3_S;
    int xPropNext_S;

	//CGameImage								m_infolc,m_inforc,m_infotc,m_infobc;	//个人头像框边界
	//CGameImage								m_infolt,m_infort,m_infolb,m_inforb;	//个人头像框边界
	//CGameImage								m_infol,m_infor;						//人物头像框左右
	//CGameImage								m_LogoImg,m_PersonalInfo;				//个人头像框和头像

	enum {IDD=IDD_ROOM_RIGHT_WND};
//	CComboBox								m_InputMessage;							//输入信息
public:
	CWnd									*m_pMainWnd;											//主窗口指针
	CUserListCtrl							m_UserList;								//用户列表
	CNewMiniDlg								m_TalkMessage;						//聊天信息
	CNormalBitmapButtonEx					m_BtHideRight;						//隐藏按钮
	CToolTipCtrl							m_ToolTip;							//提示信息

	TCHAR									szPropEmptyImage[MAX_PATH];
	CNormalBitmapButtonEx					m_btPropBuy;
	CNormalBitmapButtonEx					m_btPropPrev;
	CNormalBitmapButtonEx					m_btProp1;
	CNormalBitmapButtonEx					m_btProp2;
	CNormalBitmapButtonEx					m_btProp3;
    CNormalBitmapButtonEx                   m_btProp4;      // PengJiLin, 2010-10-11, 排列4个道具按钮
	CNormalBitmapButtonEx                   m_btProp5;
	CNormalBitmapButtonEx                   m_btProp6;
	CNormalBitmapButtonEx                   m_btProp7;
	CNormalBitmapButtonEx                   m_btProp8;
	CNormalBitmapButtonEx					m_btPropNext;
	UINT									m_BtPropArray[PROP_PEER_PAGE]; // PengJiLin, 2010-10-12. //为了方便,所以将四个按钮放到数组里


	void OnBnClickedPropBuy();
	void OnBnClickedPropPrev();	//道具面板第一排向前
	void OnBnClickedPropNext();	//							
    void OnBnClickedLeftArrow(void);//第二排  向前
    void OnBnClickedRightArrow(void);
	void OnBnClickedProp1();
	void OnBnClickedProp2();
	void OnBnClickedProp3();
    void OnBnClickedProp4();    
	void OnBnClickedProp5(); 
	void OnBnClickedProp6(); 
	void OnBnClickedProp7(); 
	void OnBnClickedProp8(); 


	//根据传入的id号，显示不同的道具  yjj 090309
	void OnPropBuy(UINT uPropId = 1);


	CChatControl							m_ChatControl;						//聊天控制
//	CGameTalkText							m_ExpressWnd;						//表情窗口
	CAFCColorResource						m_colorresource;
	CLookingClass							m_MessageHandle;					//消息处理
	//函数定义
public:
	//构造函数
	CRoomRightWnd();
	//构造函数
	CRoomRightWnd(UINT uDialogTemplata); 
	//析构函数
	virtual ~CRoomRightWnd();
	DECLARE_MESSAGE_MAP()
	//功能函数
public:
	//内部函数
private:
	/// 调整大小
	bool FixControlStation(bool bShow=true);
	/// 画界面，在OnPaint中调用
	/// @param pDC 设备上下文
	/// @param cx 客户区宽
	/// @param cy 客户区高
	bool DrawViewFrame(CDC *pDC, int cx, int cy);
	//数据绑定函数
	virtual void DoDataExchange(CDataExchange * pDX);

	//消息函数 
public:
	//重画函数
	afx_msg void OnPaint();
	afx_msg void OnNcPaint();
	//位置消息
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC){return true;}

	LRESULT OnLoadSkin(WPARAM wparam, LPARAM lparam);
public:
	void LoadSkin(void);
	//调整窗口位置
	//void AutoSize(int cx,int cy);
public:
	//隐藏列表按钮
	void OnBnClickedHideList();
	//菜单命令
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//初始化函数
	virtual BOOL OnInitDialog();
	//房间消息
	LRESULT OnSendRoomMsg(WPARAM wparam, LPARAM lparam);

	//换肤
	LRESULT OnExchangeSkin(WPARAM wparam, LPARAM lparam);
	
	//鼠标右键用户列表消息
	afx_msg void OnNMRclickUserList(NMHDR * pNMHDR, LRESULT * pResult);
	//鼠标左键用户列表消息
	afx_msg void OnNMLblclkUserList(NMHDR * pNMHDR, LRESULT * pResult);
	//鼠标双击用户列表消息
	afx_msg void OnNMDblclkUserList(NMHDR * pNMHDR, LRESULT * pResult);


	BOOL OnToolTipNotify(UINT id, NMHDR * pTTTStruct, LRESULT * pResult );	//响应ToolTip消息
	////鼠标右键好友列表消息
	//afx_msg void OnNMRclickFriendList(NMHDR * pNMHDR, LRESULT * pResult);
	////鼠标双击好友列表消息
	//afx_msg void OnNMDblclkFriendList(NMHDR * pNMHDR, LRESULT * pResult);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	//重读talk.ini
	LRESULT OnReLoadTalkINI(WPARAM wparam, LPARAM lparam);

public:
	void OnSetPropImage(void);

    // PengJiLin, 2010-10-12, 商场道具功能
    void OnSetPropImage_S(void);

    void OnBnClickedBtnProp1S(void);
    void OnBnClickedBtnProp2S(void);
    void OnBnClickedBtnProp3S(void);
    void BuyAndUseProp(int nPropIndex);

public:
	void UserProp(int nPropIndex);

	void UserProp_Buy(int nPropIndex);
private:
	CSkinMgr m_skinmgr;


public:
	CWnd*	m_pGameRoom;
	void SetGameRoomWnd(CWnd* pWnd);
};

#endif