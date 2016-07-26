///
///ClientView.h: Headers and defines for CClientPlayView
///
///

#if !defined(_CLIENTVIEW_H__INCLUDED_)
#define _CLIENTVIEW_H__INCLUDED_

#include "GameCard.h"
#include "HaveThing.h"
#include "LeaveMessage.h"
#include "lookinfo.h"
#include "Result.h"
#include "ShowAIInfo.h"
#include "SuperGM.h"

#define WM_SHOW_ROBOT_CHIP		 (WM_USER + 155)	    /** 显示机器人筹码 */

/// 每个区域绘制的最大金币数
#define MAXJINBI			250 
/// 保存最大的局数信息
//#define MAXCOUNT			72 

/// 开始按钮
#define IDC_BTN_START      20000 
/// 离开按钮
#define IDC_BTN_LEAVE      20001 
///#define IDC_BTN_SORT       20002 // 排列
///#define IDC_BTN_TRUSTEE    20003 // 托管
///#define IDC_BTN_HAVE_THING 20004 // 有事要走
/// 出牌
#define IDC_BTN_OUTCARD    20005 
/// 不出
#define IDC_BTN_PASS       20006 
/// 提示
#define IDC_BTN_HINT       20007 
/// 玩家手上的牌
#define IDD_HAND_CARD      21000 
/// 玩家所出的牌
#define IDD_OUT_CARD       21010 
/// 发牌动画
#define IDD_CARD_ANIMAL    21020 
/// 中心需要发的牌
#define IDD_CENTER_CARD    21030 

//筹码按钮lym0521
/// 100筹码
#define IDD_CHOUMA_100     21040 
/// 1000筹码
#define IDD_CHOUMA_1000    21041 
/// 1万筹码
#define IDD_CHOUMA_W       21042 
/// 10万筹码
#define IDD_CHOUMA_10W     21043 
/// 100万筹码
#define IDD_CHOUMA_100W    21044 
/// 500万筹码
#define IDD_CHOUMA_500W    21045 
/// 路子信息
#define IDD_CHOUMA_LUZI    21046 
/// 信息左键
#define IDD_CHOUMA_LEFT    21047 
 /// 信息右键
#define IDD_CHOUMA_RIGHT   21048
/// 上庄按钮
#define IDD_CHOUMA_SHANGZ  21049 
/// 银行按钮
#define IDD_CHOUMA_BANK    21050 
/// 超级客户端屏蔽机器人下注的按钮
#define IDD_HIDE_MONEY     21051 
///超级客户端控制开庄的按钮
#define IDD_SUPER_ZHUANG   21052 
///超级客户端控制开闲的按钮
#define IDD_SUPER_XIAN     21053 
///超级客户端控制开和的按钮
#define IDD_SUPER_HE	   21054 
///超级客户端恢复控制的按钮
#define IDD_SUPER_NORMAL   21055 
///上翻按钮
#define IDD_CHOUMA_TOP     21056 
///下翻按钮
#define IDD_CHOUMA_BOTTOM  21057 
/// 超级客户端显示机器人控制信息的按钮
#define IDD_SHOW_AI_INFO   21058
//显示超级对话框按钮
#define IDD_SUPER_DLG      21059

#ifdef VIDEO
#define MAX_VIDEO_NICKNAME_LEN  28  // 视频时昵称不宜过长
#endif
/// CClientPlayView 客户端，游戏界面绘制类

class CClientPlayView :
	public CUsingSkinClass
{
public:
	CClientPlayView(void);
public:
	virtual ~CClientPlayView(void);

	/// 设置昵称颜色
	void SetNameColor(COLORREF rgb)	{m_clrNameColor=rgb;}

	/// 设置等待状态
	void SetWaitingType(BYTE byType);

	/// 设置方向
	void SetCurOrientation(BYTE byCurOrientationViewSeat);

protected:

	DECLARE_MESSAGE_MAP()

public:
	/// 桌号换算到视图位置
	virtual BYTE GetViewSeat(BYTE byDeskStation);

	//更新用户资料
	virtual void UpdateUserInfo(BYTE byViewStation);
	///界面位置调整
	virtual void FixControlSize(int iWidth, int iHeight);

	///绘画界面函数
	virtual void DrawViewFace(CDC * pDC, int iWidth, int iHeight);

	///设置gif
	virtual void SetGif(BYTE bDeskView,int iValue,int iTimer);
	/// 设置显示时间
	virtual bool SetUserTimer(BYTE bViewStation, UINT uTime);

	/// 发牌动画
	void SendCard(BYTE byViewSeat, int iTime);

	/// 设置托管状态
	void SetTrustee(BYTE byViewSeat, bool bIsTrustee);

protected:

	/// 绘制背景
	virtual	void DrawBackground(CDC * pDC, int iWidth, int iHeight);

	/****************************************************************
	* 函数名: HRESULT  GetRelativePos(CPoint &PtItem, CBcfFile &f, const Cstring &csSecName,
	*                  const CString &csKeyName, const int iUserView, const CRect &rectClient)
	* 功能: 获取配置文件中元素的坐标位置
	* 参数: CPoint &PtItem     [out] 读取的坐标
	*       CBcfFile &f        [in]  坐标配置文件
	*       CString &csSecName [in]  读取的字段名
	*       CString &csKeyName [in]  读取的元素名字
	*       int iUserView      [in]  玩家视图位置
	*       CRect &rectClient  [in]  游戏客户端界面区域
	* 返回: HRESULT  0
	****************************************************************/
	HRESULT  GetRelativePos(CPoint &PtItem, CBcfFile &f, const CString &csSecName,
		const CString &csKeyName, const int iUserView, const CRect &rectClient);

	///绘奖分情况统计标志
	void DrawAwardPoint(CDC * pDC, int x, int y,BYTE iNum);

	/// 绘制庄家标志
	void DrawNt(CDC * pDC, int x, int y);

	/// 绘制托管
	void DrawTrustee(CDC * pDC, int x, int y);

	///绘制出牌先后顺序
	void DrawSequence(CDC * pDC, int x, int y,int iSequence);

	/// 绘制不出标志
	void DrawPass(CDC * pDC, int x, int y);

	/// 绘制方向
	void DrawOrientation(CDC *pDC, int x, int y, int iView);

	/// 绘制等待状态图片
	void DrawWaitingImg(CDC * pDC, int iWidth, int iHeight);
    ///绘制下注满时的气泡信息
	void DrawFullNoteBubble(Graphics* pGDIAdd, int iWidth, int iHeight);

protected:
	COLORREF	m_clrNameColor;				///< 显示用户颜色

	CGameImage  m_imgBackground;                ///< 背景
	CGameImage  m_imgCenterLogo;                ///< 中心图片

	CGameImage	m_imgAwardPint;				    ///< 奖分情况(图片)
	CGameImage	m_imgNT;						///< 庄家标识	
	CGameImage	m_imgTrusteeLogo;					///<托管标识
	CGameImage	m_imgPass;						///< pass图示
	CGameImage	m_imgPicSequence;               ///< 出完牌顺序
	CGameImage  m_imgFangXiang;                 ///< 方向标
	CGameImage  m_imgWaiting;                   ///< 等待时的图片
	CGameImage  m_imgXing;                      ///< 星星图片xiazhu.bmp
	CGameImage  m_imgChoumaNum;                 ///< 筹码数字图片xiazhu.bmp
	CGameImage  m_imgMyNum;                     ///< 玩家自己下注的筹码数字图片green_num.bmp

	CGameImage  m_imgWang;                      ///< 王图片xiazhu.bmp
	CGameImage  m_imgCard;						///< 普通牌图片xiazhu.bmp

	CGameImage  m_imgchouma;					///< 筹码图片jeton.bmp

	CGameImage  m_imgditu;						///< 结算底图ditu.bmp
	CGameImage	m_imgPointValue;				///< 结算点数值dianshu.bmp
	CGameImage	m_imgTwoKing;					///< 结算天王对子TwoKing.bmp

	CGameImage  m_imgCursor;					///< 光标图片cursor.bmp

	CGameImage  m_imgResult;					///< 结算图片result.bmp
	CGameImage  m_imgResult1;					///< 结算图片result1.bmp

	////////////////////////////////////////////////////////////////////Jing
	CGameImage  m_imgShowZ_dot;
	CGameImage  m_imgShowX_dot;
	CGameImage  m_imgShowH_dot;

	CGameImage  m_imgShowZ_Win_dot;					///< 下方显示庄圈图片我赢 show_zhuang_win.bmp
	CGameImage  m_imgShowX_Win_dot;					///< 下方显示闲圈图片我赢 show_xian_win.bmp
	CGameImage  m_imgShowH_Win_dot;					///< 下方显示和圈图片我赢 show_he_win.bmp


	CGameImage  m_imgShowZ_x_dot;					///< 下方显示庄圈图片(显示x)show_zhuang_x.bmp
	CGameImage  m_imgShowX_x_dot;					///< 下方显示闲圈图片show_xian_x.bmp
	CGameImage  m_imgShowH_x_dot;					///< 下方显示和圈图片show_he_x.bmp

	CGameImage  m_imgShowZ_Lost_x_dot;				///< 下方显示庄圈图片我下注了(显示x) show_zhuang_lost_x.bmp
	CGameImage  m_imgShowX_Lost_x_dot;				///< 下方显示闲圈图片我下注了(显示x) show_xian_lost_x.bmp
	CGameImage  m_imgShowH_Lost_x_dot;				///< 下方显示和圈图片我下注了(显示x) show_he_lost_x.bmp
	////////////////////////////////////////////////////////////////////

	CGameImage  m_imgShowZ;						///< 下方显示庄圈图片show_zhuang.bmp
	CGameImage  m_imgShowX;						///< 下方显示闲圈图片show_xian.bmp
	CGameImage  m_imgShowH;						///< 下方显示和圈图片show_he.bmp

	CGameImage  m_imgShowZ_Win;					///< 下方显示庄圈图片我赢 show_zhuang_win.bmp
	CGameImage  m_imgShowX_Win;					///< 下方显示闲圈图片我赢 show_xian_win.bmp
	CGameImage  m_imgShowH_Win;					///< 下方显示和圈图片我赢 show_he_win.bmp


	CGameImage  m_imgShowZ_x;					///< 下方显示庄圈图片(显示x)show_zhuang_x.bmp
	CGameImage  m_imgShowX_x;					///< 下方显示闲圈图片show_xian_x.bmp
	CGameImage  m_imgShowH_x;					///< 下方显示和圈图片show_he_x.bmp

	CGameImage  m_imgShowZ_Lost_x;				///< 下方显示庄圈图片我下注了(显示x) show_zhuang_lost_x.bmp
	CGameImage  m_imgShowX_Lost_x;				///< 下方显示闲圈图片我下注了(显示x) show_xian_lost_x.bmp
	CGameImage  m_imgShowH_Lost_x;				///< 下方显示和圈图片我下注了(显示x) show_he_lost_x.bmp

	CGameImage  m_imgCenterLogo2;				///< 界面底图图片CenterLogo2.bmp

	// 位置坐标	
	CPoint m_PtCardPoint[PLAY_COUNT];           // 打出牌
	CPoint m_PtHandCard[PLAY_COUNT];            // 手上牌坐标
	CPoint m_PtCurOrientation[PLAY_COUNT];      // 指示方向坐标	
	CPoint m_PtPass[PLAY_COUNT];                // 放弃
	CPoint m_PtSequence[PLAY_COUNT];            // 出完牌顺序
	CPoint m_PtTrustee[PLAY_COUNT];             // 托管
	CPoint m_PtNt[PLAY_COUNT];                  // 庄家坐标
	CPoint m_PtAwardPoint[PLAY_COUNT];          // AwardPoint
	CPoint m_PtSendCardStart[PLAY_COUNT];       // 发牌起始位置桌面正中间
	CPoint m_PtSendCardAnimalEnd[PLAY_COUNT];   // 发牌终点位置用户手中牌位置
	CPoint m_PtGif[PLAY_COUNT];                 // 送花

	// 数据统一保存视图位置
	int	 m_iAwardPoint[PLAY_COUNT];				// 牌形加分
	int	 m_iNtStation;                          /// 庄家位置
	bool m_bIsTrustee[PLAY_COUNT];              // 托管
	BYTE m_bySequence[PLAY_COUNT];				// 各家出完牌先后顺序
	bool m_bGiveUp[PLAY_COUNT];                 // 是否不出牌
	BYTE m_byCurOrientation;				    // 当前出牌者方向

	BYTE m_byWaitingType;                       // 等待状态

public:

	///固定数据,初始化后就保存不变
	///下注区域位置
	CRect	ZRect,ZTRect,ZDRcet,XRect,XTRect,XDRcet,HRect,THRect;
	CRect	XiaZhuRect;				///<下注区域：：0庄，1庄天王，2庄对子，3闲，4闲天王，5闲对子，6和，7同点和
	CRect	NowResultRect;			///<当前结算框。
	CRect   NoMouseRect,NoMouseRect1,NoMouseRect2,NoMouseRect3,NoMouseRect4;			///<鼠标禁止进入区域
	CPoint	m_iYUcenter[8];			///<每个区域的中心点位置  
	CPoint	m_pQYText[8];			///<绘制8个区域下注额的位置
	CPoint	m_pMoveStart;			///<发牌动画的开始位置
	CPoint	m_pCardZEnd;			///<庄家牌结束位置
	CPoint	m_pCardXEnd;			///<闲家牌结束位置
	CPoint	m_pTextStation;			///<上方文字描述位置
	CPoint	m_pMeInfo;				///<玩家自己信息显示的初始位置
	CPoint	m_pNtInfo;				///<庄家信息显示的初始位置
	CPoint	m_pNtList;				///<庄家列表的初始位置
	CPoint	m_pXZXianZhi;			///<下分限制显示的初始位置
	CPoint	m_pXing;				///<星星图片的初始位置
	CPoint	m_p10ResultInfo;		///<最近10局的牌点信息初始位置
	CPoint	m_pZXcount;				///<最近30局开庄开闲次数的信息初始位置
	CPoint	m_pDitu;				///<结算底图的初始位置
	CPoint	m_pViewCenter;          ///<视图中心位置
	//CPoint  m_pCursor;			///<筹码光标的位置
	POINT  m_pCursor;				///<筹码光标的位置
	__int64     m_iLimitMoney;			///<上庄需要的最少金币
	CResult	m_Result;				///<结算 

	int    m_iMeXiaZhuInfo[MAXCOUNT];	//玩家最近72把下注信息及输赢情况
	int	   m_iResultInfoArea[MAXCOUNT];	//保存历史开庄闭和情况的历史记录

	//相对固定数据，不用每把都清零
	int		m_iGameInfo[MAXCOUNT][4]; ///<最近30局的信息，庄牌点，闲牌点，赢区域
	int		m_iTempGameInfo[MAXCOUNT][4]; ///<最近30局的信息，庄牌点，闲牌点，赢区域
	//int		m_iZhuangList[4];		///<庄家列表
	///庄家列表
	short int	    m_iZhuangList[PLAY_COUNT+1];	
	/// 当前庄列表索引
	int     m_bNowNtListIndex;  

	int		m_iNowNTstation;		///<当前庄家的位置
	BYTE    m_byMyStation;			///<自己的位置
	int		m_iNTju;				///<当前庄家进行的局数
	__int64		m_iNTmoney;				///<当前庄家的金币
	int		m_iWinPercent;			///<当前玩家的胜率
	__int64		m_iNTwin;				///<当前庄家的成绩（赢的金币）
	int		m_iPlayCount;			///<已经进行了本局的第几把
	int		m_iGameCount;			///<总共进行了几把
	int		m_iZwin;				///<庄家赢，为了用整数保存放大100被，绘制是前后两为用小数点隔开
	int		m_iXwin;				///<闲家赢
	int		m_iHwin;				///<和赢
	int		m_iZDuizi;				///<庄对子
	int     m_iXDuizi;				///<闲对子
	int		m_iZTianwan;			///<庄天王
	int		m_iXTianwang;			///<闲天王

	int     m_iHistoryPlayData[MAXCOUNT];	//保存历史开庄闲记录
	int     m_iMePlayData[MAXCOUNT]; ///<玩家自己的输赢情况
	BYTE    m_byMePlayData[MAXCOUNT][3]; ///<玩家自己的输赢情况

	int     m_iHistoryDuiZi[MAXCOUNT];   // 对子数据

	int     m_iMeWinCount;			///<玩家自己赢的局数
	int     m_iMePlayCount;			///<玩家自己进行的局数
	__int64     m_iWinmoney;			///<当前玩家输赢的金币
	int		m_iShowInfoIndex;		///<信息显示的索引（从0到20）

	int		m_iSendCount;			///<接收消息次数
	bool	m_bSetUpdateTime;		///<设置更新计时器

	int     m_iCardNum;				///<播放第几张牌的动画
	bool    m_bZhuang;				///<区别是庄家还是闲家的牌
	bool    m_bCardMove;			///<是否正在进行发牌
	bool    m_bSendCard;			///<确定是否发牌结束
	WORD    m_iNowCard[2][3];		///<当前牌点
	BYTE    m_CardCount[2];			///<庄闲牌的张数
	BYTE    m_ContinueShowCardCount[2]; ///<庄闲牌的张数

	int		m_iZPaiXing[5];			///<庄家牌型,元素0前两张牌的值，元素1总牌值，元素2天王，元素3对子，元素4和
	int		m_iXPaiXing[5];			///<闲家牌型
	int		m_iWinner;				///<赢家1 庄，2闲，3和，本赢方
	int		m_iKaiPai;				///<本把开牌区域：1庄，2庄天王，3庄对子，4闲，5闲天王，6闲对子，7和，8同点和
	int		m_iWinQuYu[8];			///<游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和

	int     m_iResult[6];			///<保存本局：庄牌点，闲牌点，庄牌型，闲牌型，赢牌区域，自己得金币
	int     m_iZCardInfo[5];		///<庄的牌信息：庄家的牌数，1张牌的点数，前两张牌的点数
	int     m_iXCardInfo[5];		///<闲的牌信息：庄家的牌数，闲家的牌数，
	int		m_iChooseNote;			///<当前选择的筹码
	int		m_iChooseRect;			///<鼠标所在的区域代号
	int		m_iCardZhen;			///<当前发牌第几帧
	int		m_iShowCard;			///<显示牌,0不显示，1显示一张，2显示
	__int64		m_iZhongZhu;			///<本把当前总注额
	__int64		m_iQuYuZhu[8];			///<本把每个区域下的注额：0庄，1庄天王，2庄对子，3闲，4闲天王，5闲对子，6和，7同点和
	__int64		m_iMyZhu[9];			///<本把玩家自己在每个区域下的注额：0庄，1庄天王，2庄对子，3闲，4闲天王，5闲对子，6和，7同点和
	__int64		m_iMaxZhu[8];			///<本把每个区域最大能下的注额
	int		m_iJinBi[8][MAXJINBI];  ///<玩家在八个区域下的金币情况，记录前2000个
	CPoint	m_pJinBi[8][MAXJINBI];  ///<玩家在八个区域下的金的位置情况，记录前2000个
	__int64		m_iQuYuJinBi[8][6];		///<每区域下各类（共6类）金币的个数

	__int64		m_iJinBiNum[8];         ///<八个区域，每个区域的金币数
	BYTE    m_bNowGameState;        ///<当前游戏状态
	__int64     m_iAllJiBiCount;        ///<当前已经押了多少金币；
	__int64		m_iZhuangFen;			///<本把庄家的得分
	__int64		m_iXianFen;				///<本把闲家的得分
	__int64		m_iMeFen;				///<本把玩家自己的得分
	__int64		m_iMeFanFen;			///<本把玩家返还的分，开和时出现

	int    m_iMoveTime;				///<动画时间
	int    m_iMoveType;				///<动画的绘制区域

	bool   m_bShowResult;			///<是否显示结算信息
	bool   m_bShowRect;				///<控制赢区域的闪烁

	int     m_nPowerOfGold;         ///<金币倍率
	int		m_iSendCardZhen;		///<发牌帧数

	__int64     m_iAIMoney[8];			///机器人的下注
	bool    m_bHideAIMoney;			///是否隐藏机器人下注
	CString m_sSuperInfo;			///超级客户端控制信息
	bool    m_bShowSuper;			//显示超级客户端控制消息

	CString m_ZhuangStr;;//庄的提示文字 有时可能会叫将相和或天地和等
	CString m_XiangStr;//闲的提示文字
	///当前房间最大能下的总注
	__int64		m_iMaxNote;	

	int     m_iGamePictureType;		//1为天、地、和图片， 2为庄、闲、和图片，主要是开牌后的显示牌值用

	bool    m_isWinRed;		//赢家是否红色字体显示

	int     m_SpeCount;
	BYTE	m_Rcolor;		//红色值
	BYTE	m_Gcolor;		//绿色值
	BYTE	m_Bcolor;		//蓝色值

	HCURSOR				m_Cursor[3];						///<光标组


	bool               m_bUse500WJeton;          
	///////////////////////////////////////////

	int m_iWidth;
	int m_iHeight;
	CDC * m_pDC;
public:
	// 按钮
	CNormalBitmapButtonEx m_btnStart;		    // 开始按钮
	CNormalBitmapButtonEx m_btnLeave;		    // 离开按钮

	CNormalBitmapButtonEx m_btnOutCard;		    // 出牌
	CNormalBitmapButtonEx m_btnPass;		    // 不出牌
	CNormalBitmapButtonEx m_btnHint;		    // 提示

///百家乐筹码按钮,100,1000,1万，10万，100万，500万
	/// 100筹码按钮
	CNormalBitmapButtonEx m_btn100;	
	/// 1000筹码按钮
	CNormalBitmapButtonEx m_btn1000;
	/// 1万筹码按钮
	CNormalBitmapButtonEx m_btn1w;	
	/// 10万筹码按钮
	CNormalBitmapButtonEx m_btn10w;	
	/// 100万筹码按钮
	CNormalBitmapButtonEx m_btn100w;
	/// 500万筹码按钮
	CNormalBitmapButtonEx m_btn500w;
	/// 信息移动左按钮
	CNormalBitmapButtonEx m_btnleft;
	/// 信息移动右按钮
	CNormalBitmapButtonEx m_btnright;
	/// 看路子按钮
	CNormalBitmapButtonEx m_btnluzi;
	/// 上庄申请按钮
	CNormalBitmapButtonEx m_btnshangz;
	/// 银行按钮
	CNormalBitmapButtonEx m_btnbank;
	/// 超级客户端屏蔽机器人下注的按钮
	CNormalBitmapButtonEx m_btnHideAIMoney;
	/// 超级客户端显示机器人控制信息的按钮
	CNormalBitmapButtonEx m_btnShowAIInfo;

	///超级客户端控制开庄的按钮
	CNormalBitmapButtonEx m_btnCtrZhuang;
	///超级客户端控制开闲的按钮
	CNormalBitmapButtonEx m_btnCtrXian;
	///超级客户端控制开和的按钮
	CNormalBitmapButtonEx m_btnCtrHe;
	///超级客户端恢复控制的按钮
	CNormalBitmapButtonEx m_btnCtrNormal;
	///显示超级对话框的按钮
	CNormalBitmapButtonEx m_btnCtrSuper;

	/// 下按钮（上庄队列)
	CNormalBitmapButtonEx m_btTop;      
	/// 下按钮（上庄队列)
	CNormalBitmapButtonEx m_btBottom;    

	CUpGradeGameCard  m_HandCard[PLAY_COUNT];   // 玩家手上的牌
	CUpGradeGameCard  m_OutCard[PLAY_COUNT];    // 玩家所出的牌
	CUpGradeGameCard  m_SendCardAnimal[PLAY_COUNT];   // 发牌动画
	CUpGradeGameCard  m_SendBaseCard;           // 中心牌背(一堆需要发送的牌)
	CUpGradeGameCard  m_MingCard;               // 一张明牌

	CHaveThing		  m_HaveThing;				/// 有事离开
	CLeaveMessage	  m_ThingMessage;			/// 离开消息
///看路子对话框
	Clookinfo		  m_LuziDlg;	

///显示机器人信息对话框
	CShowAIInfo		  m_ShowAIInfo;	

///lym2010-03-19能否点击上庄按钮
	bool			m_bCanShangZhuang;
	//信息条数记录，防止玩家在点击时造成在信息栏过多的信息
	int             m_iMessageCount; 
///本局进行的局数，进行到第几局（72把一局）
	int				m_iNowJuShu;	

	bool          m_bFullNoteDisplay;       /*下注满时是否显示气泡*/ 

#ifdef VIDEO
	////////////////////视频/////////////////////////
	CPlayVideo				m_PlayVideo;
	BOOL					m_bWatch;           // 自己是否为观战
	CGameImage				m_pVideoFrame;      // 视频背景
	CRect					m_rectVideoFrame[MAX_PEOPLE];  ///视频背景坐标
	bool					GetVideoRect(CString  strNickname, RECT& rect, CString& pic);
	/////////////////////////////////////////////////////////////////////////
#endif

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBtnClickedStart(void);
	afx_msg void OnBtnClickedLeave(void);

	///筹码按钮消息
	afx_msg void OnBtnClicked100(void);
	afx_msg void OnBtnClicked1000(void);
	afx_msg void OnBtnClicked1w(void);
	afx_msg void OnBtnClicked10w(void);
	afx_msg void OnBtnClicked100w(void);
	afx_msg void OnBtnClicked500w(void);
	///信息左键事件
	afx_msg void OnBtnClickedInfoLeft(void);
	///信息右键事件
	afx_msg void OnBtnClickedInfoRight(void);
	///看路子按钮事件
	afx_msg void OnBtnClickedluzi(void);

	///上庄按钮事件
	afx_msg void OnBtnClickedShangZhuang(void);

	///银行按钮事件
	afx_msg void OnBtnClickedBank(void);
	///超级客户端屏蔽机器人下注的按钮
	afx_msg void OnBtnClickedHideMoney(void);

	///超级客户端显示机器人控制信息的按钮
	afx_msg void OnBtnClickedShowAIInfo(void);

	///超级客户端控制开庄的按钮
	afx_msg void OnBtnClickedCtrZhuang(void);
	///超级客户端控制开闲的按钮
	afx_msg void OnBtnClickedCtrXian(void);
	///超级客户端控制开和的按钮
	afx_msg void OnBtnClickedCtrHe(void);
	///超级客户端取消控制的按钮
	afx_msg void OnBtnClickedCtrNormal(void);
	///显示隐藏超级客户端
	afx_msg void OnBtnClickedSuper(void);

	///鼠标移动消息
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	///鼠标左键抬起消息
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	///鼠标右键按下
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	///计时器
	afx_msg void OnTimer(UINT nIDEvent);

	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

	///百家乐消息处理lym0512
	///处理下注消息lym0512
	void OnHandleXiazhu(void * pNetData, UINT uDataSize);
	
	///处理系统开牌消息lym0514
	void OnHandleKaiPai(void * pNetData, UINT uDataSize);

	///lym0512绘制金币
	void DrawJinBi(CDC*pDC, int iWidth, int iHeight);

	///lym0521绘制选择区域
	void DrawRect(CDC*pDC, int iWidth, int iHeight);

	///lym0521更新路子信息
	void UpdateLuziData(void);

	///lym0530更新当前开牌的信息信息
	void UpdateNowData(void);

	///绘制庄信息，庄列表
	void DrawNtInfo(CDC*pDC, int iWidth, int iHeight);

	///绘制下注信息：八个区域的下注，庄闲和可下注的限额
	void DrawXiaZhuInfo(CDC*pDC, int iWidth, int iHeight);

	///绘制历史结果信息：最近10局的庄闲点数，最近30局庄闲和开牌
	void DrawResultInfo(CDC*pDC, int iWidth, int iHeight);

	/// 绘制点值
	void DrawPointValue(CDC * pDC, CDC &dc, int ix, int iy);

	///绘制玩家自己的信息：金币，命中率，总计成绩
	void DrawMeInfo(CDC*pDC, int iWidth, int iHeight);

	///绘制动态信息：无庄（提示），下注（提示下注），开牌（逐个提示开牌信息），空闲时间
	void DrawActiveInfo(CDC*pDC, int iWidth, int iHeight);

	///绘制各个区域的下注数字
	void DrawXiaZhuNum(CDC*pDC,int num, int iWidth, int iHeight,int x,int y,bool my);

	///绘制牌，最终显示的牌
	void DrawCard(CDC*pDC,int iWidth, int iHeight);

	///绘制牌，牌动画
	void DrawMoveCard(CDC*pDC,int iWidth, int iHeight);

	//启动发牌动画
	void StartMove(bool zhuang,int num,int zhen);

	///在x,y绘制一张牌Card
	void DrawACard(CDC*pDC,int iWidth, int iHeight,WORD Card,int x,int y);

	///屏蔽下注或开启下注
	void EnableXiaZhu(bool enable);

	///每局结束后初始化视图数据
	void Init();

	///检测自己是否是庄家或在庄家列表中
	bool CheckMeZhuang(void);

	///更新结算窗口的位置
	void UpdateResultWindow(int x,int y);

	///启动超过500万的动画
	void StartChao500Move(int type);

	///绘制超过500万的动画
	void DrawChao500Move(CDC*pDC,int iWidth, int iHeight);

	///绘制光标图片
	void DrawCursor(CDC*pDC,int iWidth, int iHeight);//绘制光标图片

	///绘制透明结算框
	void DrawResultFrame(CDC*pDC, int iXPos, int iYPos);

	/// 设置申请上庄按钮
	void SetApplyBanker(bool bApply);

	/// 恢复鼠标正常状态
	void OnRestoreCursor();

	/// 设置鼠标状态
	void OnShowCursor(BOOL bShow);

	//将数字转成带小数点的字符数组lym1015
	void NumToChar(__int64 result,CString* szText );

	///断线重连后显示当前下注。
	void ShowNowNote();

	///断线重连后显示当前下注。设置当前下注的数据
	void SetNowNoteData(int type,int num5bw,int num1bw,int num10w,int num1w,int numq,int numb);

	///获取玩家自己当前的下注总额
	__int64 GetMyAllNote();

	///即时设置自己能下注按钮和鼠标
	void SetXiaZhuBtn();

	///上庄队列下翻按钮事件
	void OnBtnClickedNTTop(void);

	///上庄把队列下翻按钮事件
	void OnBtnClickedNTBottom(void);

	///描述：通过筹码类型获取筹码金额
	__int64 GetMoneyFromType(__int64 moneytype);

public:  ///add xyh 2011-12-7
		CGameImage  m_imgPromptOpenCard;			///< 提示开牌图片PromptOpenCard.bmp 
		CGameImage  m_imgPromptChangeNt;			///< 提示换庄图片PromptChangeNt.bmp
		bool		m_bShowPromptOpenCard;			///< 是否显示提示开牌图片
		bool		m_bShowPromptChangeNt;			///< 是否显示提示换庄图片	

		///提示开牌图片
		void DrawPromptOpenCard(CDC * pDC, int x, int y);
		
		///提示换庄图片
		void DrawPromptChangeNt(CDC * pDC, int x, int y);

		///设置开牌提示图片
		void SetPromptOpenCard(bool bShowFlag);

		///设置换庄提示图片
		void SetPromptChangeNt(bool bShowFlag);
public:

	//获取所有玩家的昵称
	void GetAllUserNicheng();

	///将金币根据配置转化字符串
	void ChangeNumToCStringByPower(__int64 nMoney ,bool bHaveSpace ,TCHAR *chSpace, CString &strRet) ; 

	///获取真实的钱或者界面上显示的钱true表示界面上显示的钱 , false表示转化成真实的钱
	__int64 ChangeMoneyByPower(__int64  nMoney , bool  bUIMoney ) ;   
	

	SUPERDATA				m_SuperData;				/**< 超级GM工具玩家的数据*/
	CSuperGM				m_SuperDlg;					/**< 超级GM工具显示dialog*/

	LRESULT OnShowRobotChip(WPARAM wParam, LPARAM lParam);
};
#endif // !defined(_CLIENTVIEW_H__INCLUDED_)