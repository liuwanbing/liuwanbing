#pragma once

//消息定义
#define	IDM_LEFT_HITCARD			WM_USER+110						//左击扑克
#define	IDM_RIGHT_HITCARD			WM_USER+111						//右击扑克
//#define IDM_MOUSE_OUT				WM_USER+112
#define IDM_MOUSE_UP				WM_USER+113                     // 拖动的扑克

//操作掩码
#define	UG_HUA_MASK					0x00F0							//花色掩码
#define	UG_VALUE_MASK				0x000F							//数字掩码

//牌型
#define IMAGE_1024_768				0x00							//牌
#define IMAGE_800_600				0x00							//牌
#define IMAGE_SMALL					0x01							//牌
#define IMAGE_VERY_SMALL			0x03							//牌

//扑克控制类
class CUpGradeGameCard : public CWnd
{
	//枚举定义
public:
	typedef enum eXShowMode { EXSM_LEFT=0, EXSM_CENTER, EXSM_RIGHT  } E_XSHOW_MODE;
	typedef enum eYShowMode { EYSM_TOP=0,  EYSM_CENTER, EYSM_BOTTOM } E_YSHOW_MODE; 
	CWordArray						m_wCardArray;					// 扑克数组
	CWordArray						m_wCardUpArray;					// 扑克数组
	CWordArray						m_wMoveSelectCardArray;			// 移动选择扑克数组

	//变量定义
protected:
	int								m_iCardHight;					//扑克高度
	int								m_iCardWidth;					//扑克宽度
	int								m_iCardHSpace;					//扑克间隔
	int								m_iCardVSpace;					//扑克间隔
	int								m_iCardUpSpace;					//跳起高度
//	CRgn							m_CardViewRgn;					//扑克区域
	CWnd							* m_pHandleWnd;					//处理窗口
	//信息变量
protected:
	bool							m_bShowCard;					//显示扑克
	bool							m_bHorzLook;					//是否横放
	bool							m_bUseMouse;					//响应鼠标
	WORD							m_wHotHitCard;					//焦点扑克
	BYTE							m_byCardCount;					//扑克数目
	// 扑克数目(当m_byCardCountView>m_byCardCount 并且 m_byCardCount>0时显示m_byCardCountView)
	// 否则显示m_byCardCount
	BYTE							m_byCardCountView;				
	CPoint							m_PtBasePoint;					//基准坐标
	E_XSHOW_MODE					m_XShowMode;					//显示模式
	E_YSHOW_MODE					m_YShowMode;					//显示模式
	BYTE							m_bShowNum;						//是否显示数量	
	BYTE							m_bCardStyle;					//使用牌是否为小牌true表示使用小牌,false表示使用默认牌
	BOOL							m_bSelect;						//是否在某张牌上
	CWordArray						m_wCardFriendArray;				//扑克数组(如果牌数大多分二组显示)
	BYTE							m_bFriendCardCount;				//友牌总数
//	BOOL							m_bLButtonDown;					//左键被按下
	WORD							m_wFirstCard;					//
	CPoint							m_ptLeftHit;					//左键被按下的坐标
	CPoint							m_ptLeftHitMove;				//左键按下移动所到的坐标
	CPoint							m_ptMovePoint;					//移动鼠标当前的坐标
	BOOL							m_bCanMove;						//是否为移动
	WORD                            m_wCanUp;                       //是否升起备份
	WORD							m_wMoveFirstCard;				//移动的第一张牌即单击的那张牌

	//主动设置分数
	int								m_iSourcePoint;						//原来分数
	int								m_iBackPoint;						//设置捞底分数
	int								m_iBackMultiple;					//设置底倍数

	//批量弹起牌
	bool							m_bMultiSel;
	CPoint							m_pStartPos;					// 开始鼠标位置
	CPoint							m_pEndPos;						// 结束位置
	CPoint                          m_pMovePos;                     //按下左键位置
	// 拖牌功能
	bool							m_bMoveSel;					
	bool							m_bMove;
	int								m_CurCardIndex;					// 鼠标当前所在card index
	WORD							m_wStartIndex;					// 开始移动的位置
	WORD							m_wEndIndex;					// 开始移动的位置
	/*bool                            m_bLDown;
	bool                            m_MouseMove;*/



	//图片变量
protected:
//	CAFCImage						m_CardPic[2];					//扑克控件
	static		CGameImage			m_CardPic[6];//[4];					//扑克控件
	CGameImage						m_Num;								//数字
	CGameImage m_bk;//背景
	CToolTipCtrl					m_ToolTip;						//工具提示

	// qy 动画修改 20090107
	CPoint m_ptStart;    // 移动的开始坐标
	CPoint m_ptEnd;      // 移动目标坐标
	long m_lMoveCount;   // 移动次数
	// end qy 动画修改 20090107

	//函数定义
public:
	//构造函数
	CUpGradeGameCard(BYTE bCardStyle=0);
	//析构函数
	virtual ~CUpGradeGameCard();

	//设置函数
public:
	//初始化函数
	bool InitGameCard(CWnd * pHandleWnd);
	//设置扑克参数
	void SetCardParameter(int iHSpace, int iVSpace, int iUpSpace);
	//设置牌宽和高
	void SetCardWidthHeight(int iWidth,int iHeight);
	//设置基准点函数
	void SetBasePoint(CPoint & BasePoint, E_XSHOW_MODE XShowMode, E_YSHOW_MODE YShowMode);

	// 获取当前位置
	void GetBasePoint(CPoint &ptBasePoint);

	/****************************************************************
	* 函数名: void MoveAnimal(CPoint &ptStart, CPoint &ptEnd, long lTime)
	* 功能: 开始移动筹码，设置移动起始、终点、总时间
	* 参数: CPoint &ptStart    [in]  起始位置
	*       CPoint &ptEnd      [in]  终点
	*       long lTime         [in]  移动总时间 (ms)
	* 返回: N/A
	****************************************************************/
	void MoveAnimal(CPoint &ptStart, CPoint &ptEnd, long lTime);

	// 停止移动
	void StopMove(void);	

	//设置显示扑克
	void SetShowCard(bool bShow);
	//设置使用鼠标
	void SetUseMouse(bool bUserMouse=false);
	//设置显示方式
	void SetLookMode(bool bHorzLook);
	//设置调用牌模式
	void SetCardStyle(BYTE bCardStyle=0){m_bCardStyle=bCardStyle;}
	//调用牌模式
	BYTE GetCardStyle(){return m_bCardStyle;}
	//设置调用牌模式
	void SetCardShowNum(BYTE bShowNum = 0){m_bShowNum = bShowNum;}
	//调用牌模式
	BYTE GetCardShowNum(){return m_bShowNum;}
	//功能函数
public:
	//设置扑克
	bool SetCard(WORD wCardList[], WORD wUpList[], BYTE bCardCount, BYTE byCardCountView=0);
	//删除升起的扑克
	BYTE RemoveUpCard();
	//获取升起的扑克
	BYTE GetUpCard(WORD wCardList[]);
	//获取扑克信息
	BYTE GetCard(WORD wCardList[], WORD wUpList[]);
	//分牌总数
	int GetPoint();
	//主动设置分数
	BOOL SetBackPointAndMultiple(int iSourcePoint = 0,int iBackPoint = 0,int iBackMultiple = 1);
	//内联函数
public:
	//获取扑克数目
	BYTE GetCardCount() { return m_byCardCount; }
	//获取扑克数值
	BYTE GetCardValue(WORD wCard) 
	{ 
		//return (bCard&UG_VALUE_MASK);
		BYTE bkk=(BYTE)(wCard&UG_VALUE_MASK);
		return bkk; 
	}
	//获取扑克花色
	BYTE GetHuaKind(WORD wCard) { return (BYTE)(wCard&UG_HUA_MASK)-1; }
	//获取扑克大小
	BYTE GetCardNum(WORD wCard) { return (BYTE)(wCard&UG_VALUE_MASK); }	
	//获取扑克位置
	BYTE GetCardXPos(WORD wCard) { return GetCardValue(wCard)-1; };
	//获取扑克位置
	BYTE GetCardYPos(WORD wCard) { return GetHuaKind(wCard)>>4; };

	//内部函数
protected:
	//按键测试
	BYTE HitCardTest(CPoint & Point);
	//按键测试
	BYTE HitCardTestAbove(CPoint &Point);
	//调整窗口位置
	void FixCardControl();
	//获取大小函数
	bool GetCardSize(CSize & Size);

	//消息函数
protected:
	//建立消息
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//重画函数
	afx_msg void OnPaint();
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//鼠标消息
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//鼠标消息
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//鼠标移动
	afx_msg void OnMouseMove(UINT nFlags,CPoint Point);
	
	//virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	//设置友牌数组
	BOOL SetFriendArray(WORD wCardList[], WORD wUpList[], BYTE bCardCount);

	void UnUpAllCard();
protected:
	//查找友牌数组中某一个牌的个数
	BYTE FindACardNumInFriendList(BYTE iCardNum);
	//设置图片
	void SetCardImage();
	//交换二张牌位置
	BOOL ChangeIndex(BYTE DestIndex ,BYTE SrcIndex);
	//交换多张
	BOOL ChangeIndexArray(BYTE DestIndex,BYTE SrcIndex);
	//将牌反转
	BOOL Convert(WORD wCardList[],int iCardCount);
	// 画半透明框
	void DrawTransparent(HDC destdc, CRect &rc, COLORREF color, int Alpha);
	// 得到牌index
	BYTE GetCardIndex(CPoint &Point);

	//收回所有的牌

//public:
//	//鼠标移动调整牌
	void MouseMoveAjust(CPoint Point);
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

