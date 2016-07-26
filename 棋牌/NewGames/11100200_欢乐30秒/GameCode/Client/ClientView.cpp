#include "StdAfx.h"
#include "ClientView.h"
#include "ClientGameDlg.h"
#include "resource.h"
#define IDR_FAPAI						1100	// 发牌定时器（5秒中内完成所有动画并显示结果）
#define IDR_XIANPAI						1200	// 发牌发闲家的牌
#define IDR_ZHUANGPAI					1300	// 发庄家的牌
#define IDR_UPDATEXIAZHU				1301	// 更新界面
#define IDR_CTL_ZHUANG_BTN				1302	// lym控制点击上庄按钮的计时器


void MsgPrint(const char *lpszFormat, ...);



// 消息映射
BEGIN_MESSAGE_MAP(CClientPlayView, CUsingSkinClass)

	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BTN_START, OnBtnClickedStart)
	ON_BN_CLICKED(IDC_BTN_LEAVE, OnBtnClickedLeave)
	ON_BN_CLICKED(IDD_CHOUMA_100, OnBtnClicked100)//100筹码按钮
	ON_BN_CLICKED(IDD_CHOUMA_1000, OnBtnClicked1000)//1000筹码按钮
	ON_BN_CLICKED(IDD_CHOUMA_W, OnBtnClicked1w)//1w筹码按钮
	ON_BN_CLICKED(IDD_CHOUMA_10W, OnBtnClicked10w)//10w
	ON_BN_CLICKED(IDD_CHOUMA_100W, OnBtnClicked100w)//100w
	ON_BN_CLICKED(IDD_CHOUMA_500W, OnBtnClicked500w)//500w
	ON_BN_CLICKED(IDD_CHOUMA_LUZI, OnBtnClickedluzi)//看路子事件
	ON_BN_CLICKED(IDD_CHOUMA_LEFT, OnBtnClickedInfoLeft)//信息左按钮事件
	ON_BN_CLICKED(IDD_CHOUMA_RIGHT, OnBtnClickedInfoRight)//信息右按钮事件
	ON_BN_CLICKED(IDD_CHOUMA_SHANGZ, OnBtnClickedShangZhuang)//上庄按钮事件
	ON_BN_CLICKED(IDD_CHOUMA_BANK, OnBtnClickedBank)//银行按钮事件IDD_HIDE_MONEY
	ON_BN_CLICKED(IDD_HIDE_MONEY, OnBtnClickedHideMoney)//超级客户端屏蔽机器人下注的按钮
	ON_BN_CLICKED(IDD_SHOW_AI_INFO, OnBtnClickedShowAIInfo)//超级客户端显示机器人控制信息的按钮

	ON_BN_CLICKED(IDD_SUPER_ZHUANG, OnBtnClickedCtrZhuang)//超级客户端控制开庄的按钮
	ON_BN_CLICKED(IDD_SUPER_XIAN, OnBtnClickedCtrXian)//超级客户端控制开闲的按钮
	ON_BN_CLICKED(IDD_SUPER_HE, OnBtnClickedCtrHe)//超级客户端控制开和的按钮
	ON_BN_CLICKED(IDD_SUPER_NORMAL, OnBtnClickedCtrNormal)//超级客户端取消控制的按钮
	ON_BN_CLICKED(IDD_SUPER_DLG, OnBtnClickedSuper)//显示超级客户端对话框

	ON_MESSAGE(WM_SHOW_ROBOT_CHIP, OnShowRobotChip)


	ON_BN_CLICKED(IDD_CHOUMA_TOP, OnBtnClickedNTTop)//上庄队列上翻按钮事件
	ON_BN_CLICKED(IDD_CHOUMA_BOTTOM, OnBtnClickedNTBottom)//上庄队列下翻按钮事件

	ON_WM_TIMER()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()
CClientPlayView::CClientPlayView(void)
:CUsingSkinClass(),m_Result(this)/*,m_LuziDlg(this)*/
{
	////OutputDebugString("CClientPlayView::CClientPlayView");
	/////lym0512百家乐客户端信息数据
	m_Cursor[0]=LoadCursor(GetModuleHandle(NULL),MAKEINTRESOURCE(IDC_HAND1));
	m_bShowResult = false; //是否显示结算信息
	m_bShowRect = false ;//控制赢区域的闪烁
	m_iMoveTime =0 ;  //动画时间
	m_iMoveType = -1;  //动画的绘制区域
	m_bNowGameState = GS_WAIT_AGREE;        //当前游戏状态
	m_iZwin =0 ;//庄家赢的百分比，为了用整数保存放大100被，绘制是前后两为用小数点隔开
	m_iXwin =0 ;//闲家赢的百分比
	m_iHwin =0 ;//和赢的百分比
	m_iZDuizi =0 ;//庄对子
	m_iXDuizi =0 ;//闲对子
	m_iZTianwan =0 ;//庄天王
	m_iXTianwang =0 ;//闲天王
	m_iPlayCount=0;
	m_iGameCount =0 ;			//总共进行了几把
	m_iShowCard = 0;//显示牌
	m_iAllJiBiCount=0;        //当前已经押了多少金币；
	m_bFullNoteDisplay = false;

	m_iMessageCount = 0;

	m_iWinPercent =0;       //当前玩家的胜率
	m_iNowNTstation=-1;		//当前庄家的位置
	m_iNTju =0 ;			//当前庄家进行的局数
	m_iNTmoney=0;			//当前庄家的金币
	m_iNTwin=0;				//当前庄家的成绩（赢的金币）
	//自己的信息
	m_iWinmoney=0;			//当前玩家输赢的金币
	m_iMeWinCount=0;//玩家自己赢的局数
	m_iMePlayCount=0;//玩家自己进行的局数

	m_iLimitMoney=0;		//上庄需要的最少金币
	m_iZhongZhu=0;			//本把当前总注额
	m_Cursor[0] = LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW));//光标1
	m_Cursor[1] = LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW));//光标2
	m_Cursor[2] = LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW));//光标3
	m_iCardZhen =-1;   //当前发牌第几帧
	m_iShowInfoIndex = 0; //信息显示的索引（从0到20）

	m_iZhuangFen=0;		//本把庄家的得分
	m_iXianFen=0;			//本把闲家的得分
	m_iMeFen=0;			//本把玩家自己的得分
	m_iMeFanFen = 0;		//本把玩家返还的分，开和时出现
	m_nPowerOfGold=1;///百亿问题
	m_isWinRed =false;
	m_Rcolor =0;
	m_Gcolor =0;
	m_Bcolor =0;

	m_SpeCount=4;

	int i = 0;
	for (i=0; i<PLAY_COUNT; ++i)
	{
		m_PtLogo[i] = CPoint(0,0);                // 头像
		m_PtCardPoint[i] = CPoint(0,0);           // 打出牌
		m_PtHandCard[i] = CPoint(0,0);            // 手上牌坐标
		m_PtTime[i] = CPoint(0,0);                // 计时器
		m_PtCurOrientation[i] = CPoint(0,0);      // 指示方向坐标
		m_PtName[i] = CPoint(0,0);                // 昵称
		m_PtAgree[i] = CPoint(0,0);               // 举手
		m_PtPass[i] = CPoint(0,0);                // 放弃
		m_PtSequence[i] = CPoint(0,0);            // 出完牌顺序
		m_PtTrustee[i] = CPoint(0,0);                // 托管
		m_PtNt[i] = CPoint(0,0);                  // 庄家坐标
		m_PtAwardPoint[i] = CPoint(0,0);          // AwardPoint
		m_PtSendCardStart[i] = CPoint(0,0);       // 发牌起始位置桌面正中间
		m_PtSendCardAnimalEnd[i] = CPoint(0,0);   // 发牌终点位置用户手中牌位置
		m_PtGif[i] = CPoint(0,0);                 // 送花
		m_iAwardPoint[i] = 0;                     // 奖励分
		m_bIsTrustee[i] = false;                  // 是否托管
		m_bySequence[i] = PLAY_COUNT;             // 出完牌顺序默认为最后一名
		m_bGiveUp[i] = false;
	}

	memset(m_iGameInfo, -1, sizeof(m_iGameInfo));//游戏时庄闲牌点数及输赢信息
    for(int i=0;i<MAXCOUNT;i++)
	{		
		if(i<8)
		{
			m_iJinBiNum[i] = 0;//每个区域的金币数
			for(int j=0;j<MAXJINBI;j++)
			{
				m_iJinBi[i][j]=0;    //玩家在八个区域下的金币情况，记录前2000个
				m_pJinBi[i][j].x=0;  //玩家在八个区域下的金的位置情况，记录前2000个
				m_pJinBi[i][j].y=0;  //玩家在八个区域下的金的位置情况，记录前2000个
			}
		}
	}
	memset(m_iZhuangList,-1,sizeof(m_iZhuangList));//庄家列表
	m_bNowNtListIndex=0;

	ZRect.SetRect(555,235,760,432);  // 庄下注区域
	ZTRect.SetRect(555,432,760,506); // 庄天王下注区域
	ZDRcet.SetRect(555,165,760,237); // 庄对子下注区域
	XRect.SetRect(38,235,245,432); //  闲下注区域
	XTRect.SetRect(38,432,245,506); // 闲天王下注区域
	XDRcet.SetRect(38,165,245,237); // 闲对子下注区域
	HRect.SetRect(245,309,554,433); //  和下注区域
	THRect.SetRect(245,433,554,505);//  同点和下注区域

	m_pCardZEnd.SetPoint(245,215);         //庄家牌结束位置
	m_pCardXEnd.SetPoint(397,215);        //闲家牌结束位置
	m_pTextStation.SetPoint(270,90);		//上方文字描述位置
	m_pMeInfo.SetPoint(8,597);		   //玩家自己信息显示的初始位置
	m_pNtInfo.SetPoint(325,24);		   //庄家信息显示的初始位置
	m_pXing.SetPoint(275,597);		   //星星图片的初始位置
	m_p10ResultInfo.SetPoint(38,89);   //最近10局的牌点信息初始位置
	m_pXZXianZhi.SetPoint(90,16);		 //下分限制显示的初始位置
	m_pNtList.SetPoint(0,0);		   //庄家列表的初始位置
	m_pZXcount.SetPoint(0,0);        //最近30局开庄开闲次数的信息初始位置
	m_pDitu.SetPoint(0,0);          //结算底图的初始位置
	m_pViewCenter.SetPoint(0,0);          //视图中心位置
	//m_pCursor.SetPoint(0,0);        //筹码光标的位置
	//m_pCursor->x=0;
	//m_pCursor->y=0;

	m_pQYText[0].x = ZRect.left + ZRect.Width()/2 - 8;		//绘制8个区域下注额的位置
	m_pQYText[0].y = ZRect.top + ZRect.Height()/2 -22;		//绘制8个区域下注额的位置

	m_pQYText[1].x = ZTRect.left + ZTRect.Width()/2 - 8;		//绘制8个区域下注额的位置
	m_pQYText[1].y = ZTRect.top + ZTRect.Height()/2 -22;		//绘制8个区域下注额的位置

	m_pQYText[2].x = ZDRcet.left + ZDRcet.Width()/2 - 8;		//绘制8个区域下注额的位置
	m_pQYText[2].y = ZDRcet.top + ZDRcet.Height()/2 -22;		//绘制8个区域下注额的位置

	m_pQYText[3].x = XRect.left + XRect.Width()/2 - 8;		//绘制8个区域下注额的位置
	m_pQYText[3].y = XRect.top + XRect.Height()/2 -22;		//绘制8个区域下注额的位置

	m_pQYText[4].x = XTRect.left + XTRect.Width()/2 - 8;		//绘制8个区域下注额的位置
	m_pQYText[4].y = XTRect.top + XTRect.Height()/2 -22;		//绘制8个区域下注额的位置

	m_pQYText[5].x = XDRcet.left + XDRcet.Width()/2 - 8;		//绘制8个区域下注额的位置
	m_pQYText[5].y = XDRcet.top + XDRcet.Height()/2 -22;		//绘制8个区域下注额的位置

	m_pQYText[6].x = HRect.left + HRect.Width()/2 - 8;		//绘制8个区域下注额的位置
	m_pQYText[6].y = HRect.top + HRect.Height()/2 -22;		//绘制8个区域下注额的位置

	m_pQYText[7].x = THRect.left + THRect.Width()/2 - 8;		//绘制8个区域下注额的位置
	m_pQYText[7].y = THRect.top + THRect.Height()/2 -22;		//绘制8个区域下注额的位置

	m_iYUcenter[0].x = ZRect.left + ZRect.Width()/2;		
	m_iYUcenter[0].y = ZRect.top + ZRect.Height()/2;		//

	m_iYUcenter[1].x = ZTRect.left + ZTRect.Width()/2;		//
	m_iYUcenter[1].y = ZTRect.top + ZTRect.Height()/2;		//

	m_iYUcenter[2].x = ZDRcet.left + ZDRcet.Width()/2;		//绘制8个区域下注额的位置
	m_iYUcenter[2].y = ZDRcet.top + ZDRcet.Height()/2;		//绘制8个区域下注额的位置

	m_iYUcenter[3].x = XRect.left + XRect.Width()/2;		//绘制8个区域下注额的位置
	m_iYUcenter[3].y = XRect.top + XRect.Height()/2;		//绘制8个区域下注额的位置

	m_iYUcenter[4].x = XTRect.left + XTRect.Width()/2;		//绘制8个区域下注额的位置
	m_iYUcenter[4].y = XTRect.top + XTRect.Height()/2;		//绘制8个区域下注额的位置

	m_iYUcenter[5].x = XDRcet.left + XDRcet.Width()/2;		//绘制8个区域下注额的位置
	m_iYUcenter[5].y = XDRcet.top + XDRcet.Height()/2;		//绘制8个区域下注额的位置

	m_iYUcenter[6].x = HRect.left + HRect.Width()/2;		//绘制8个区域下注额的位置
	m_iYUcenter[6].y = HRect.top + HRect.Height()/2;		//绘制8个区域下注额的位置

	m_iYUcenter[7].x = THRect.left + THRect.Width()/2;		//绘制8个区域下注额的位置
	m_iYUcenter[7].y = THRect.top + THRect.Height()/2;		//绘制8个区域下注额的位置

	for(int i=0;i<8;i++)
	{
		//////////百家乐信息////////////////////////////////////////////////////////
		m_iQuYuZhu[i]=0;    //本把每个区域下的注额
		m_iMaxZhu[i]=0;     //本把每个区域最大能下的注额
		m_iMyZhu[i]=0;    //本把玩家自己在每个区域下的注额
		//m_iYUcenter[i].x=0;     //每个区域的中心点位置  
		//m_iYUcenter[i].y=0;     //每个区域的中心点位置  
		///////////////////////////////////////////////////////////////////////////
	}

	m_iCardNum=0;			//播放第几张牌的动画
	m_bZhuang=false;			//区别是庄家还是闲家的牌
	m_bCardMove = false;				//是否正在进行发牌
	m_bSendCard = false;			//确定是否发牌结束
	memset(&m_iNowCard, 0, sizeof(m_iNowCard));		 //当前牌点  
	
	memset(&m_CardCount, 0, sizeof(m_CardCount));		 //庄闲牌的张数

	memset(&m_iResult, 0, sizeof(m_iResult));		 ////保存本局：庄牌点，闲牌点，庄牌型，闲牌型，赢牌区域，自己得金币 

	memset(&m_iZPaiXing, 0, sizeof(m_iZPaiXing)); //庄家牌型,元素0前两张牌的值，元素1总牌值，元素2天王，元素3对子，元素4和
	memset(&m_iXPaiXing, 0, sizeof(m_iXPaiXing));//闲家牌型
	memset(&m_iMePlayData, 0, sizeof(m_iMePlayData));	 //玩家自己的输赢情况
	memset(&m_byMePlayData, 0, sizeof(m_byMePlayData));	 //玩家自己的输赢情况

	memset(&m_iWinQuYu, 0, sizeof(m_iWinQuYu));	//游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和
	memset(&m_iMyZhu, 0, sizeof(m_iMyZhu));	////本把玩家自己在每个区域下的注额

	m_iWinner = 0 ;				//赢家1 庄，2闲，3和，本赢方
	m_iKaiPai = 0 ;				//本把开牌区域：1庄，2庄天王，3庄对子，4闲，5闲天王，6闲对子，和，同点和

#ifdef VIDEO
	m_bWatch = true;    // 自己是否为观战
#endif

	SetNameColor(RGB(255,255,0));	//设置默认用户名颜色
	m_iNtStation = -1;
	m_byCurOrientation = 255;
	m_byWaitingType = NO_WAITING;
	memset(m_uTime,0,sizeof(m_uTime));	
	m_iChooseNote =0;//当前选择的筹码 0没有，1：100，2：1000，3：1w，4：10w，5：100w,6：500w
	m_iChooseRect = -1; //鼠标所在的区域代号
	m_byMyStation =255;//自己的位置
	m_iSendCardZhen = 25;
///lym2010-03-19能否点击上庄按钮
	m_bCanShangZhuang = true;
/////////////////////////////////////////////////
	m_iNowJuShu = 0;	//本局进行的局数，进行到第几局（72把一局）
	memset(&m_iAIMoney, 0, sizeof(m_iAIMoney));	///机器人的下注
	m_bHideAIMoney = true;			///是否隐藏机器人下注
	m_sSuperInfo = "";			///超级客户端控制信息
	m_bShowSuper = false;			//显示超级客户端控制消息

	///当前房间最大能下的总注
	m_iMaxNote = 0;	

	//玩家最近72把下注信息及输赢情况
	memset(m_iMeXiaZhuInfo, 0, sizeof(m_iMeXiaZhuInfo));
	memset(m_iHistoryPlayData, 0, sizeof(m_iHistoryPlayData));
	memset(m_iHistoryDuiZi,0,sizeof(m_iHistoryDuiZi));


	CString s = CINIFile::GetAppPath ();/////本地路径
	CBcfFile f( s + SKIN_FOLDER + _T(".bcf"));
	CString strKey = TEXT("config");
	m_iGamePictureType = f.GetKeyVal(strKey, "GamePictureType", 0);
	m_ZhuangStr = f.GetKeyVal(strKey, "ZhuangStr", "庄");
	m_XiangStr = f.GetKeyVal(strKey, "XiangStr", "闲");
	m_bUse500WJeton =f.GetKeyVal("config","Use500WJeton",0)>0?true:false;

	m_bShowPromptOpenCard = false;	///< 是否显示提示开牌图片
	m_bShowPromptChangeNt = false;	///< 是否显示提示换庄图片
}

CClientPlayView::~CClientPlayView(void)
{
	// 有事要走
	if (NULL != m_HaveThing.GetSafeHwnd())
	{
		m_HaveThing.DestroyWindow();
	}

	// 有事要走答复对话框
	if (NULL != m_ThingMessage.GetSafeHwnd())
	{
		m_ThingMessage.DestroyWindow();
	}
}

///更新用户资料
void CClientPlayView::UpdateUserInfo(BYTE byViewStation)
{
	UpdateViewFace(NULL);
}

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
HRESULT  CClientPlayView::GetRelativePos(CPoint &PtItem, CBcfFile &f, const CString &csSecName,
						const CString &csKeyName, const int iUserView, const CRect &rectClient)
{
	PtItem.SetPoint(0,0);           // 初始化原始坐标为0
	CPoint PtTemp = CPoint(0,0);    // 使用临时的坐标点
	CString csValueName = _T("");   // 关键字名

	// x 相对位置
	csValueName.Format("%s%dx_type", csKeyName, iUserView);
	CString csTypeX = f.GetKeyVal(csSecName, csValueName, _T(""));
	csTypeX.Trim(_T(" "));
	if (0 == csTypeX.CompareNoCase(_T("Left")))                     // 相对左边框
	{
		PtTemp.x = rectClient.left;
	}
	else if (0 == csTypeX.CompareNoCase(_T("Right")))               // 相对右边框
	{
		PtTemp.x = rectClient.right;
	}
	else if ((0 == csTypeX.CompareNoCase(_T("Mid")))
		|| (0 == csTypeX.CompareNoCase(_T("Mid.x"))))               // 居中
	{
		PtTemp.x = rectClient.Width()/2;
	}
	else if (!csTypeX.IsEmpty())     // 相对其他元素位置，采用递归方式，在配置文件中不能存在循环链
	{
		CPoint PtItemTemp(0,0);
		GetRelativePos(PtItemTemp, f, csSecName, csTypeX, iUserView, rectClient);
		PtTemp.x = PtItemTemp.x;
	}
	
	// 获取y坐标
	csValueName.Format("%s%dy_type", csKeyName, iUserView);
	CString csTypeY = f.GetKeyVal(csSecName, csValueName, _T(""));
	csTypeY.Trim(_T(" "));
	if (0 == csTypeY.CompareNoCase(_T("Top")))                     // 相对顶部
	{
		PtTemp.y = rectClient.top;
	}
	else if (0 == csTypeY.CompareNoCase(_T("Bottom")))             // 相对底部
	{
		PtTemp.y = rectClient.bottom;
	}
	else if ((0 == csTypeY.CompareNoCase(_T("Mid")))
		|| (0 == csTypeY.CompareNoCase(_T("Mid.y"))))              // 居中
	{
		PtTemp.y = rectClient.Height()/2;
	}	
	else if (!csTypeY.IsEmpty())    // 相对其他元素位置，采用递归方式，在配置文件中不能存在循环链
	{
		CPoint PtItemTemp(0,0);
		GetRelativePos(PtItemTemp, f, csSecName, csTypeY, iUserView, rectClient);
		PtTemp.y= PtItemTemp.y;
	}

	// 获取x的坐标
	csValueName.Format("%s%dx", csKeyName, iUserView);
	PtTemp.x += f.GetKeyVal(csSecName, csValueName, rectClient.Width()/2);

	// 获取y的坐标
	csValueName.Format("%s%dy", csKeyName, iUserView);
	PtTemp.y += f.GetKeyVal(csSecName, csValueName, rectClient.Height()/2);
	PtItem = PtTemp;
	return 0;
}

///界面位置调整
void CClientPlayView::FixControlSize(int iWidth, int iHeight)
{
	//OutputDebugString("CClientPlayView::FixControlSize");
	if (NULL == m_hWnd)
	{
		return ;
	}

	CRect rect(0,0,iWidth, iHeight);
	//移动坐标
	int iMidLine=iWidth/2;			// x中间线
	int iBaseLine=iHeight/2;		// y中间线

	m_pViewCenter.SetPoint(iMidLine,iBaseLine);          //视图中心位置

	//计时器位置lym0515
	for(int i=0;i<180;i++)
	{
		m_PtTime[i].x=iMidLine-55;
		m_PtTime[i].y=iBaseLine+200;
	}	

	ZRect.SetRect(iMidLine+156,iBaseLine-110,iMidLine+360,iBaseLine+82);  // 庄下注区域
	ZTRect.SetRect(iMidLine+156,iBaseLine+85,iMidLine+360,iBaseLine+155); // 庄天王下注区域
	ZDRcet.SetRect(iMidLine+156,iBaseLine-181,iMidLine+360,iBaseLine-113); // 庄对子下注区域
	XRect.SetRect(iMidLine-359,iBaseLine-110,iMidLine-155,iBaseLine+82); //  闲下注区域
	XTRect.SetRect(iMidLine-359,iBaseLine+85,iMidLine-155,iBaseLine+155); // 闲天王下注区域
	XDRcet.SetRect(iMidLine-359,iBaseLine-181,iMidLine-155,iBaseLine-113); // 闲对子下注区域
	HRect.SetRect(iMidLine-152,iBaseLine-39,iMidLine+153,iBaseLine+82); //  和下注区域
	THRect.SetRect(iMidLine-152,iBaseLine+85,iMidLine+153,iBaseLine+155);//  同点和下注区域


 //XiaZhuRect;//下注区域：：0庄，1庄天王，2庄对子，3闲，4闲天王，5闲对子，6和，7同点和

	m_pMoveStart.SetPoint(iMidLine+247,iBaseLine-294);       //发牌动画的开始位置
	m_pCardZEnd.SetPoint(iMidLine+14,iBaseLine-138);         //庄家牌结束位置
	m_pCardXEnd.SetPoint(iMidLine-138+14,iBaseLine-138);        //闲家牌结束位置
	m_pTextStation.SetPoint(iMidLine-130,iBaseLine-260);		//上方文字描述位置
	m_pMeInfo.SetPoint(iMidLine-391,iBaseLine+250);		   //玩家自己信息显示的初始位置
	m_pNtList.SetPoint(iMidLine +170 ,iBaseLine -285 );		   //庄家列表的初始位置

	m_pNtInfo.SetPoint(iMidLine-75,iBaseLine-324);		   //庄家信息显示的初始位置
	m_pXing.SetPoint(iMidLine-125,iBaseLine+251);		   //星星图片的初始位置
	m_p10ResultInfo.SetPoint(iMidLine-362,iBaseLine-260);   //最近10局的牌点信息初始位置
	m_pXZXianZhi.SetPoint(iMidLine-310,iBaseLine-330);		 //下分限制显示的初始位置
	m_pZXcount.SetPoint(iMidLine +342 ,iBaseLine + 251);        //最近30局开庄开闲次数的信息初始位置
	m_pDitu.SetPoint(iMidLine-155  ,iBaseLine - 183 );          //结算底图的初始位置


	m_pQYText[0].x = ZRect.left + ZRect.Width()/2 - 8;		//绘制8个区域下注额的位置
	m_pQYText[0].y = ZRect.top + ZRect.Height()/2 -22;		//绘制8个区域下注额的位置

	m_pQYText[1].x = ZTRect.left + ZTRect.Width()/2 - 8;		//绘制8个区域下注额的位置
	m_pQYText[1].y = ZTRect.top + ZTRect.Height()/2 -22;		//绘制8个区域下注额的位置

	m_pQYText[2].x = ZDRcet.left + ZDRcet.Width()/2 - 8;		//绘制8个区域下注额的位置
	m_pQYText[2].y = ZDRcet.top + ZDRcet.Height()/2 -22;		//绘制8个区域下注额的位置

	m_pQYText[3].x = XRect.left + XRect.Width()/2 - 8;		//绘制8个区域下注额的位置
	m_pQYText[3].y = XRect.top + XRect.Height()/2 -22;		//绘制8个区域下注额的位置

	m_pQYText[4].x = XTRect.left + XTRect.Width()/2 - 8;		//绘制8个区域下注额的位置
	m_pQYText[4].y = XTRect.top + XTRect.Height()/2 -22;		//绘制8个区域下注额的位置

	m_pQYText[5].x = XDRcet.left + XDRcet.Width()/2 - 8;		//绘制8个区域下注额的位置
	m_pQYText[5].y = XDRcet.top + XDRcet.Height()/2 -22;		//绘制8个区域下注额的位置

	m_pQYText[6].x = HRect.left + HRect.Width()/2 - 8;		//绘制8个区域下注额的位置
	m_pQYText[6].y = HRect.top + HRect.Height()/2 -22;		//绘制8个区域下注额的位置

	m_pQYText[7].x = THRect.left + THRect.Width()/2 - 8;		//绘制8个区域下注额的位置
	m_pQYText[7].y = THRect.top + THRect.Height()/2 -22;		//绘制8个区域下注额的位置

	m_iYUcenter[0].x = ZRect.left + ZRect.Width()/2 ;		//
	m_iYUcenter[0].y = ZRect.top + ZRect.Height()/2 ;		//

	m_iYUcenter[1].x = ZTRect.left + ZTRect.Width()/2 ;		//
	m_iYUcenter[1].y = ZTRect.top + ZTRect.Height()/2 ;		//

	m_iYUcenter[2].x = ZDRcet.left + ZDRcet.Width()/2 ;		//	
	m_iYUcenter[2].y = ZDRcet.top + ZDRcet.Height()/2 ;		//

	m_iYUcenter[3].x = XRect.left + XRect.Width()/2;		//
	m_iYUcenter[3].y = XRect.top + XRect.Height()/2;		//

	m_iYUcenter[4].x = XTRect.left + XTRect.Width()/2;		//
	m_iYUcenter[4].y = XTRect.top + XTRect.Height()/2;		//

	m_iYUcenter[5].x = XDRcet.left + XDRcet.Width()/2;		//
	m_iYUcenter[5].y = XDRcet.top + XDRcet.Height()/2;		//

	m_iYUcenter[6].x = HRect.left + HRect.Width()/2;		//
	m_iYUcenter[6].y = HRect.top + HRect.Height()/2;		//

	m_iYUcenter[7].x = THRect.left + THRect.Width()/2;		//
	m_iYUcenter[7].y = THRect.top + THRect.Height()/2;		//


	CRect rectClient;
	GetClientRect(&rectClient);

	//改变积分榜,x坐标
	m_showcoredlg.GetClientRect(&rect);
	m_showcoredlg.SetWindowPos(NULL,rectClient.right-10-rect.Width(),-m_showcoredlg.m_showh,0,0,SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOSIZE);

	// 移动按钮
	// 开始按钮
	m_btnStart.GetClientRect(&rect);
	m_btnStart.MoveWindow(iMidLine-rect.Width()-20,iHeight-200,rect.Width(),rect.Height());
	m_btnLeave.MoveWindow(iMidLine+20,iHeight-200,rect.Width(),rect.Height());

	// 移动出牌、不出、提示按钮
	m_btnOutCard.GetClientRect(&rect);
	m_btnOutCard.MoveWindow(iMidLine-2*rect.Width(), iHeight-200, rect.Width(),rect.Height());
	m_btnPass.MoveWindow(iMidLine-rect.Width()/2, iHeight-200, rect.Width(),rect.Height());
	m_btnHint.MoveWindow(iMidLine+rect.Width(), iHeight-200, rect.Width(),rect.Height());


///72把一局
#ifdef SET72 
 	//筹码按钮位置
	m_btn100.GetClientRect(&rect);
	m_btn100.MoveWindow(iWidth/2+27 , iHeight/2+234-rect.Height(),rect.Width(),rect.Height());

	m_btn1000.GetClientRect(&rect);
	m_btn1000.MoveWindow(iWidth/2+90 , iHeight/2+234-rect.Height(),rect.Width(),rect.Height());
	m_btn1w.GetClientRect(&rect);
	m_btn1w.MoveWindow(iWidth/2+152 , iHeight/2+234-rect.Height(),rect.Width(),rect.Height());
	m_btn10w.GetClientRect(&rect);
	m_btn10w.MoveWindow(iWidth/2+213 , iHeight/2+234-rect.Height(),rect.Width(),rect.Height());
	m_btn100w.GetClientRect(&rect);
	m_btn100w.MoveWindow(iWidth/2+275 , iHeight/2+234-rect.Height(),rect.Width(),rect.Height());
	m_btn500w.GetClientRect(&rect);
	m_btn500w.MoveWindow(iWidth/2+336 , iHeight/2+234-rect.Height(),rect.Width(),rect.Height());    


#else

	//筹码按钮位置
	m_btn100.GetClientRect(&rect);
	m_btn100.MoveWindow(iWidth/2+43 , iHeight/2+185,rect.Width(),rect.Height());
	m_btn1000.GetClientRect(&rect);
	m_btn1000.MoveWindow(iWidth/2+94 , iHeight/2+181,rect.Width(),rect.Height());
	m_btn1w.GetClientRect(&rect);
	m_btn1w.MoveWindow(iWidth/2+149 , iHeight/2+181,rect.Width(),rect.Height());
	m_btn10w.GetClientRect(&rect);
	m_btn10w.MoveWindow(iWidth/2+203 , iHeight/2+177,rect.Width(),rect.Height());
	m_btn100w.GetClientRect(&rect);
	m_btn100w.MoveWindow(iWidth/2+261 , iHeight/2+174,rect.Width(),rect.Height());
	m_btn500w.GetClientRect(&rect);
	m_btn500w.MoveWindow(iWidth/2+322 , iHeight/2+170,rect.Width(),rect.Height());
#endif

	//筹码按钮位置
	m_btn100.GetClientRect(&rect);
	m_btn100.MoveWindow(iWidth/2+27 , iHeight/2+234-rect.Height(),rect.Width(),rect.Height());
	m_btn1000.GetClientRect(&rect);
	m_btn1000.MoveWindow(iWidth/2+90 , iHeight/2+234-rect.Height(),rect.Width(),rect.Height());
	m_btn1w.GetClientRect(&rect);
	m_btn1w.MoveWindow(iWidth/2+152 , iHeight/2+234-rect.Height(),rect.Width(),rect.Height());
	m_btn10w.GetClientRect(&rect);
	m_btn10w.MoveWindow(iWidth/2+213 , iHeight/2+234-rect.Height(),rect.Width(),rect.Height());
	m_btn100w.GetClientRect(&rect);
	m_btn100w.MoveWindow(iWidth/2+275 , iHeight/2+234-rect.Height(),rect.Width(),rect.Height());
	m_btn500w.GetClientRect(&rect);
	m_btn500w.MoveWindow(iWidth/2+336 , iHeight/2+234-rect.Height(),rect.Width(),rect.Height());  

	NoMouseRect.SetRect(iWidth/2+41-16,iHeight/2+234-rect.Height()-16,iWidth/2+315+rect.Width()+16,iHeight/2+224+16);			///<鼠标禁止进入区域

	//int iMidLine=iWidth/2;			// x中间线
	//int iBaseLine=iHeight/2;		// y中间线

	// 信息移动左按钮
	m_btnleft.GetClientRect(&rect);
	m_btnleft.MoveWindow(iMidLine-200, iBaseLine+270,rect.Width(),rect.Height());
	// 信息移动右按钮
	m_btnright.GetClientRect(&rect);
	m_btnright.MoveWindow(iMidLine+302, iBaseLine+270,rect.Width(),rect.Height());
	//看路子按钮
	m_btnluzi.GetClientRect(&rect);
	m_btnluzi.MoveWindow(iMidLine-400, iBaseLine+201,rect.Width(),rect.Height());
	NoMouseRect1.SetRect(iMidLine-400-16,iBaseLine+201-16,iMidLine-395+rect.Width()+16,iBaseLine+201+rect.Height()+16);

	//申请上庄按钮
	m_btnshangz.GetClientRect(&rect);
	m_btnshangz.MoveWindow(iMidLine+305, iBaseLine-340,rect.Width(),rect.Height());
	NoMouseRect2.SetRect(iMidLine+305-16,iBaseLine-340-16,iMidLine+305+rect.Width()+16,iBaseLine-340+rect.Height()+16);

	//银行按钮
	m_btnbank.GetClientRect(&rect);
	m_btnbank.MoveWindow(iMidLine-395+78, iBaseLine+201,rect.Width(),rect.Height());
	NoMouseRect3.SetRect(iMidLine-295-16,iBaseLine+201-16,iMidLine-295+rect.Width()+16,iBaseLine+201+rect.Height()+16);


	//上庄列表上翻按钮
	m_btTop.GetClientRect(&rect);
	m_btTop.MoveWindow(m_pNtList.x+242-23, m_pNtList.y-27,rect.Width(),rect.Height());

	//上庄列表下翻按钮
	m_btBottom.GetClientRect(&rect);
	m_btBottom.MoveWindow(m_pNtList.x+242-23, m_pNtList.y+61-20,rect.Width(),rect.Height());



	///<鼠标禁止进入区域

	//超级客户端屏蔽机器人下注的按钮
	m_btnHideAIMoney.GetClientRect(&rect);
	m_btnHideAIMoney.MoveWindow(iMidLine-395+170, iBaseLine+218,rect.Width(),rect.Height());

	//超级客户端屏显示机器人控制信息按钮
	m_btnShowAIInfo.GetClientRect(&rect);
	m_btnShowAIInfo.MoveWindow(iMidLine-395+255, iBaseLine+218,rect.Width(),rect.Height());


	//超级客户端控制开庄的按钮
	m_btnCtrZhuang.GetClientRect(&rect);
	m_btnCtrZhuang.MoveWindow(iMidLine-300, iBaseLine+218-45,rect.Width(),rect.Height());

	//超级客户端控制开闲的按钮
	m_btnCtrXian.GetClientRect(&rect);
	m_btnCtrXian.MoveWindow(iMidLine-300+60, iBaseLine+218-45,rect.Width(),rect.Height());

	//超级客户端控制开和的按钮
	m_btnCtrHe.GetClientRect(&rect);
	m_btnCtrHe.MoveWindow(iMidLine-300+120, iBaseLine+218-45,rect.Width(),rect.Height());

	///超级客户端恢复控制的按钮
	m_btnCtrNormal.GetClientRect(&rect);
	m_btnCtrNormal.MoveWindow(iMidLine-300+180, iBaseLine+218-45,rect.Width(),rect.Height());

	///显示超级对话框的按钮
	m_btnCtrSuper.GetClientRect(&rect);
	m_btnCtrSuper.MoveWindow(iMidLine-300+180 + 100, iBaseLine+218-45,rect.Width(),rect.Height());


/////////设置各个下注区域

	CGameImage reimg;
	TCHAR path[MAX_PATH];
	wsprintf(path,TEXT("image\\result.bmp"));
	reimg.SetLoadInfo(path,true);	 
	CPoint point(m_pDitu.x,m_pDitu.y);
	//ClientToScreen(&point);
	//CRect rect(0,0,0,0);
	CClientGameDlg *pGameDlg=CONTAINING_RECORD(this,CClientGameDlg, m_PlayView);
	if (NULL != pGameDlg)
	{
		pGameDlg->GetWindowRect(&rect);
	}

}

int CClientPlayView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	////OutputDebugString("CClientPlayView::OnCreate");

	if (CUsingSkinClass::OnCreate(lpCreateStruct) == -1)
		return -1;

	////OutputDebugString("CClientPlayView::OnCreate _22");

	// 加载图片
	TCHAR szPath[MAX_PATH]={0};

	m_Result.Create(IDD_RESULT,this);
	//m_Info.Create(IDD_RESULT,this);

	//m_Result.ClearAll();

#ifdef VIDEO
	_stprintf_s(szPath, sizeof(szPath), TEXT("image\\VideoFrame.bmp"), SKIN_FOLDER);
	m_pVideoFrame.SetLoadInfo(szPath,false);
#else
	if(m_iUserFrame.GetWidth() <= 0)
	{
		_stprintf_s(szPath, sizeof(szPath), TEXT("image\\logoframe.bmp"),SKIN_FOLDER);
		SetFrameImage(szPath);
	}
#endif

	// 背景图
	_stprintf_s(szPath, sizeof(szPath), _T("image\\BackColorOnly.bmp"), SKIN_FOLDER);
	m_imgBackground.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);

	// 背景中心图片
	_stprintf_s(szPath, sizeof(szPath), _T("image\\centerlogo.bmp"), SKIN_FOLDER);
	m_imgCenterLogo.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);

	///< 界面底图图片CenterLogo2.bmp
	_stprintf_s(szPath, sizeof(szPath), _T("image\\centerlogo2.bmp"), SKIN_FOLDER);
	m_imgCenterLogo2.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);

	// 奖励分图片
	_stprintf_s(szPath, sizeof(szPath), _T("image\\AwardPoint.bmp"), SKIN_FOLDER);
	m_imgAwardPint.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);

	// 庄家图片
	_stprintf_s(szPath, sizeof(szPath), _T("image\\nt.bmp"), SKIN_FOLDER);
	m_imgNT.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);

	// 托管图片
	_stprintf_s(szPath, sizeof(szPath), _T("image\\tuoguan.bmp"), SKIN_FOLDER);
	m_imgTrusteeLogo.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);

	// 不出图片
	_stprintf_s(szPath, sizeof(szPath), _T("image\\pass.bmp"), SKIN_FOLDER);
	m_imgPass.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);

	// 出完牌顺序
	_stprintf_s(szPath, sizeof(szPath), _T("image\\finish_out1.bmp"), SKIN_FOLDER);
	m_imgPicSequence.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);

	// 方向
	_stprintf_s(szPath, sizeof(szPath), _T("image\\CurOutCard.bmp"),SKIN_FOLDER);
	m_imgFangXiang.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);	

	// 等待玩家开始图片
	_stprintf_s(szPath, sizeof(szPath), _T("image\\waiting.bmp"),SKIN_FOLDER);
	m_imgWaiting.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);	

	//星星图片
	_stprintf_s(szPath, sizeof(szPath), _T("image\\xingxing.bmp"),SKIN_FOLDER);
	m_imgXing.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);	
              
	//押注区域的数组图片 // 筹码数字图片
	_stprintf_s(szPath, sizeof(szPath), _T("image\\xiazhu.bmp"),SKIN_FOLDER);
	m_imgChoumaNum.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);	

	// 玩家自己下注的筹码数字图片green_num.bmp
	_stprintf_s(szPath, sizeof(szPath), _T("image\\green_num.bmp"),SKIN_FOLDER);
	m_imgMyNum.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);	

	// 王图片xiazhu.bmp
	_stprintf_s(szPath, sizeof(szPath), _T("cardskin\\game_CARD_BACK_SELECT.bmp"),SKIN_FOLDER);
	m_imgWang.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);	

	// 普通牌图片xiazhu.bmp
	_stprintf_s(szPath, sizeof(szPath), _T("image\\%s\\GAME_CARD_SELECT.bmp"),SKIN_CARD);
	m_imgCard.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);	

	// 筹码图片
	if(m_bUse500WJeton)
	{
    	_stprintf_s(szPath, sizeof(szPath), _T("image\\jeton1.bmp"),SKIN_FOLDER);
	}
	else
	{
		_stprintf_s(szPath, sizeof(szPath), _T("image\\jeton.bmp"),SKIN_FOLDER);
	}

	m_imgchouma.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);	

	// 结算底图ditu.bmp
	_stprintf_s(szPath, sizeof(szPath), _T("image\\ditu.bmp"),SKIN_FOLDER);
	m_imgditu.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);
	// 结算点数值
	_stprintf_s(szPath, sizeof(szPath), _T("image\\dianshu.bmp"),SKIN_FOLDER);
	m_imgPointValue.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);
	// 结算天王对子
	_stprintf_s(szPath, sizeof(szPath), _T("image\\TwoKing.bmp"),SKIN_FOLDER);
	m_imgTwoKing.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);
	

	// 光标图片cursor.bmp m_imgCursor
	_stprintf_s(szPath, sizeof(szPath), _T("image\\cursor.bmp"),SKIN_FOLDER);
	m_imgCursor.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);	

    // 结算图片result.bmp
	_stprintf_s(szPath, sizeof(szPath), _T("image\\result.bmp"),SKIN_FOLDER);
	m_imgResult.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);	

    // 结算图片result1.bmp
	_stprintf_s(szPath, sizeof(szPath), _T("image\\result1.bmp"),SKIN_FOLDER);
	m_imgResult1.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);	


	/////////////////////////////////////////////////////Jing

	_stprintf_s(szPath, sizeof(szPath), _T("image\\show_zhuang_dot.bmp"),SKIN_FOLDER);
	m_imgShowZ_dot.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);	

	///< 下方显示闲圈图片show_xian.bmp
	_stprintf_s(szPath, sizeof(szPath), _T("image\\show_xian_dot.bmp"),SKIN_FOLDER);
	m_imgShowX_dot.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);	

	///< 下方显示和圈图片show_he.bmp
	_stprintf_s(szPath, sizeof(szPath), _T("image\\show_he_dot.bmp"),SKIN_FOLDER);
	m_imgShowH_dot.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);	

	_stprintf_s(szPath, sizeof(szPath), _T("image\\show_zhuang_win_dot.bmp"),SKIN_FOLDER);
	m_imgShowZ_Win_dot.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);	

	///< 下方显示闲圈图片show_xian.bmp
	_stprintf_s(szPath, sizeof(szPath), _T("image\\show_xian_win_dot.bmp"),SKIN_FOLDER);
	m_imgShowX_Win_dot.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);	

	///< 下方显示和圈图片show_he.bmp
	_stprintf_s(szPath, sizeof(szPath), _T("image\\show_he_win_dot.bmp"),SKIN_FOLDER);
	m_imgShowH_Win_dot.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);	

	///< 下方显示庄圈图片show_zhuang.bmp
	_stprintf_s(szPath, sizeof(szPath), _T("image\\show_zhuang_x_dot.bmp"),SKIN_FOLDER);
	m_imgShowZ_x_dot.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);	

	///< 下方显示闲圈图片show_xian.bmp
	_stprintf_s(szPath, sizeof(szPath), _T("image\\show_xian_x_dot.bmp"),SKIN_FOLDER);
	m_imgShowX_x_dot.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);	

	///< 下方显示和圈图片show_he.bmp
	_stprintf_s(szPath, sizeof(szPath), _T("image\\show_he_x_dot.bmp"),SKIN_FOLDER);
	m_imgShowH_x_dot.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);	

	///< 下方显示庄圈图片我赢 show_zhuang_lost_x.bmp
	_stprintf_s(szPath, sizeof(szPath), _T("image\\show_zhuang_lost_x_dot.bmp"),SKIN_FOLDER);
	m_imgShowZ_Lost_x_dot.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);	

	///< 下方显示闲圈图片我赢 show_xian_lost_x.bmp
	_stprintf_s(szPath, sizeof(szPath), _T("image\\show_xian_lost_x_dot.bmp"),SKIN_FOLDER);
	m_imgShowX_Lost_x_dot.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);	

	///< 下方显示和圈图片我赢 show_he_lost_x.bmp
	_stprintf_s(szPath, sizeof(szPath), _T("image\\show_he_lost_x_dot.bmp"),SKIN_FOLDER);
	m_imgShowH_Lost_x_dot.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);	
	
	///< 提示开牌图片PromptOpenCard.bmp add xyh 2011-12-7
	_stprintf_s(szPath, sizeof(szPath), _T("image\\PromptOpenCard.bmp"),SKIN_FOLDER);
	m_imgPromptOpenCard.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);	

	///< 提示换庄图片PromptChangeNt.bmp add xyh 2011-12-7
	_stprintf_s(szPath, sizeof(szPath), _T("image\\PromptChangeNt.bmp"),SKIN_FOLDER);
	m_imgPromptChangeNt.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);


	/////////////////////////////////////////////////////

	///< 下方显示庄圈图片show_zhuang.bmp
	_stprintf_s(szPath, sizeof(szPath), _T("image\\show_zhuang.bmp"),SKIN_FOLDER);
	m_imgShowZ.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);	

	///< 下方显示闲圈图片show_xian.bmp
	_stprintf_s(szPath, sizeof(szPath), _T("image\\show_xian.bmp"),SKIN_FOLDER);
	m_imgShowX.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);	

	///< 下方显示和圈图片show_he.bmp
	_stprintf_s(szPath, sizeof(szPath), _T("image\\show_he.bmp"),SKIN_FOLDER);
	m_imgShowH.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);	

	///< 下方显示庄圈图片我赢 show_zhuang_win.bmp
	_stprintf_s(szPath, sizeof(szPath), _T("image\\show_zhuang_win.bmp"),SKIN_FOLDER);
	m_imgShowZ_Win.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);	

	///< 下方显示闲圈图片我赢 show_xian_win.bmp
	_stprintf_s(szPath, sizeof(szPath), _T("image\\show_xian_win.bmp"),SKIN_FOLDER);
	m_imgShowX_Win.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);	

	///< 下方显示和圈图片我赢 show_he_win.bmp
	_stprintf_s(szPath, sizeof(szPath), _T("image\\show_he_win.bmp"),SKIN_FOLDER);
	m_imgShowH_Win.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);	


	///< 下方显示庄圈图片show_zhuang.bmp
	_stprintf_s(szPath, sizeof(szPath), _T("image\\show_zhuang_x.bmp"),SKIN_FOLDER);
	m_imgShowZ_x.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);	

	///< 下方显示闲圈图片show_xian.bmp
	_stprintf_s(szPath, sizeof(szPath), _T("image\\show_xian_x.bmp"),SKIN_FOLDER);
	m_imgShowX_x.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);	

	///< 下方显示和圈图片show_he.bmp
	_stprintf_s(szPath, sizeof(szPath), _T("image\\show_he_x.bmp"),SKIN_FOLDER);
	m_imgShowH_x.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);	

	///< 下方显示庄圈图片我赢 show_zhuang_lost_x.bmp
	_stprintf_s(szPath, sizeof(szPath), _T("image\\show_zhuang_lost_x.bmp"),SKIN_FOLDER);
	m_imgShowZ_Lost_x.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);	

	///< 下方显示闲圈图片我赢 show_xian_lost_x.bmp
	_stprintf_s(szPath, sizeof(szPath), _T("image\\show_xian_lost_x.bmp"),SKIN_FOLDER);
	m_imgShowX_Lost_x.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);	

	///< 下方显示和圈图片我赢 show_he_lost_x.bmp
	_stprintf_s(szPath, sizeof(szPath), _T("image\\show_he_lost_x.bmp"),SKIN_FOLDER);
	m_imgShowH_Lost_x.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);	


	// 创建按钮
	m_btnStart.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS,CRect(0,0,72,31),this,IDC_BTN_START);   // 开始按钮
	m_btnLeave.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS,CRect(0,0,72,31),this,IDC_BTN_LEAVE);   // 离开按钮

	//m_LuziDlg.SetWindowPos(&m_btnStart, 0,0,0,0, SWP_NOSIZE);
	//m_LuziDlg.MoveWindow(0,0,100,100);
	// 隐藏游戏开始、离开按钮
	m_btnStart.ShowWindow(SW_HIDE);
	m_btnLeave.ShowWindow(SW_HIDE);

	///百家乐筹码按钮
	m_btn100.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE|WS_DISABLED|BS_OWNERDRAW,CRect(0,0,72,31),this,IDD_CHOUMA_100);
	m_btn1000.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE|WS_DISABLED|BS_OWNERDRAW,CRect(0,0,72,31),this,IDD_CHOUMA_1000);
	m_btn1w.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE|WS_DISABLED|BS_OWNERDRAW,CRect(0,0,72,31),this,IDD_CHOUMA_W);
	m_btn10w.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE|WS_DISABLED|BS_OWNERDRAW,CRect(0,0,72,31),this,IDD_CHOUMA_10W);
	m_btn100w.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE|WS_DISABLED|BS_OWNERDRAW,CRect(0,0,72,31),this,IDD_CHOUMA_100W);
	m_btn500w.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE|WS_DISABLED|BS_OWNERDRAW,CRect(0,0,72,31),this,IDD_CHOUMA_500W);
	/// 信息移动左按钮
	m_btnleft.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE|WS_DISABLED|BS_OWNERDRAW,CRect(0,0,72,31),this,IDD_CHOUMA_LEFT);
	/// 信息移动右按钮
	m_btnright.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE|WS_DISABLED|BS_OWNERDRAW,CRect(0,0,72,31),this,IDD_CHOUMA_RIGHT);

	m_btnluzi.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE|WS_DISABLED|BS_OWNERDRAW,CRect(0,0,72,31),this,IDD_CHOUMA_LUZI);
	///申请上庄按钮
	m_btnshangz.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE|WS_DISABLED|BS_OWNERDRAW,CRect(0,0,72,31),this,IDD_CHOUMA_SHANGZ);
	///银行按钮
	m_btnbank.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE|WS_DISABLED|BS_OWNERDRAW,CRect(0,0,72,31),this,IDD_CHOUMA_BANK);
	///超级客户端屏蔽机器人下注按钮
	m_btnHideAIMoney.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE|WS_DISABLED|BS_OWNERDRAW,CRect(0,0,72,31),this,IDD_HIDE_MONEY);
	///超级客户端显示机器人控制信息的按钮
	m_btnShowAIInfo.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE|WS_DISABLED|BS_OWNERDRAW,CRect(0,0,72,31),this,IDD_SHOW_AI_INFO);


	///超级客户端控制开庄的按钮
	m_btnCtrZhuang.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE|WS_DISABLED|BS_OWNERDRAW,CRect(0,0,72,31),this,IDD_SUPER_ZHUANG);
	///超级客户端控制开闲的按钮
	m_btnCtrXian.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE|WS_DISABLED|BS_OWNERDRAW,CRect(0,0,72,31),this,IDD_SUPER_XIAN);
	///超级客户端控制开和的按钮
	m_btnCtrHe.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE|WS_DISABLED|BS_OWNERDRAW,CRect(0,0,72,31),this,IDD_SUPER_HE);
	///超级客户端恢复控制的按钮
	m_btnCtrNormal.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE|WS_DISABLED|BS_OWNERDRAW,CRect(0,0,72,31),this,IDD_SUPER_NORMAL);

	///显示超级对话框的按钮
	m_btnCtrSuper.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE|WS_DISABLED|BS_OWNERDRAW,CRect(0,0,72,31),this,IDD_SUPER_DLG);


	m_btTop.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE|WS_DISABLED|BS_OWNERDRAW,CRect(0,0,72,31),this,IDD_CHOUMA_TOP);

	m_btBottom.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE|WS_DISABLED|BS_OWNERDRAW,CRect(0,0,72,31),this,IDD_CHOUMA_BOTTOM);

	m_btn100.ShowWindow(SW_SHOW);
	m_btn1000.ShowWindow(SW_SHOW);
	m_btn1w.ShowWindow(SW_SHOW);
	m_btn10w.ShowWindow(SW_SHOW);
	m_btn100w.ShowWindow(SW_SHOW);
	m_btn500w.ShowWindow(SW_SHOW);
	m_btnleft.ShowWindow(SW_SHOW);
	m_btnluzi.ShowWindow(SW_SHOW);
	m_btnright.ShowWindow(SW_SHOW);
	m_btnshangz.ShowWindow(SW_SHOW);
	m_btnbank.ShowWindow(SW_SHOW);
	m_btnHideAIMoney.ShowWindow(SW_HIDE);
	m_btnCtrZhuang.ShowWindow(SW_HIDE);
	m_btnCtrXian.ShowWindow(SW_HIDE);
	m_btnCtrHe.ShowWindow(SW_HIDE);
	m_btnCtrNormal.ShowWindow(SW_HIDE);
	m_btnCtrSuper.ShowWindow(SW_HIDE);
	m_btnShowAIInfo.ShowWindow(SW_HIDE);

	m_btn100.EnableWindow(true);
	m_btn1000.EnableWindow(true);
	m_btn1w.EnableWindow(true);
	m_btn10w.EnableWindow(true);
	m_btn100w.EnableWindow(true);
	m_btn500w.EnableWindow(true);
	m_btnleft.EnableWindow(true);
	m_btnright.EnableWindow(true);
	m_btnluzi.EnableWindow(true);
	m_btnshangz.EnableWindow(false);
	m_btnbank.EnableWindow(true);
	m_btnHideAIMoney.EnableWindow(false);
	m_btnShowAIInfo.EnableWindow(false);
	m_btnCtrZhuang.EnableWindow(false);
	m_btnCtrXian.EnableWindow(false);
	m_btnCtrHe.EnableWindow(false);
	m_btnCtrNormal.EnableWindow(false);
	m_btnCtrSuper.EnableWindow(false);
	m_btTop.EnableWindow(SW_SHOW);
	m_btBottom.EnableWindow(SW_SHOW);

	/// 出牌/不出牌/提示按钮
	m_btnOutCard.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE|WS_DISABLED|BS_OWNERDRAW,CRect(0,0,72,31),this,IDC_BTN_OUTCARD);
	m_btnPass.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE|WS_DISABLED|BS_OWNERDRAW,CRect(0,0,72,31),this,IDC_BTN_PASS);
	m_btnHint.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE|WS_DISABLED|BS_OWNERDRAW,CRect(0,0,72,31),this,IDC_BTN_HINT);
	m_btnOutCard.ShowWindow(SW_HIDE);
	m_btnPass.ShowWindow(SW_HIDE);
	m_btnHint.ShowWindow(SW_HIDE);


	/// 中心牌背(一堆需要发送的牌)
	m_SendBaseCard.Create(NULL,NULL,WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS,CRect(0,0,0,0), this, IDD_CENTER_CARD,NULL);
	m_SendBaseCard.SetShowCard(true);
	m_SendBaseCard.SetCardShowNum(1);
	m_SendBaseCard.InitGameCard(GetParent());
	m_SendBaseCard.SetUseMouse(false);
	m_SendBaseCard.SetCardParameter(2,1, 1);   // 设置牌间距离
	
	/// 一张明牌
	m_MingCard.Create(NULL,NULL,WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS,CRect(0,0,0,0), this, IDD_CENTER_CARD+1,NULL);
	m_MingCard.SetShowCard(true);
	m_MingCard.InitGameCard(GetParent());
	m_MingCard.SetUseMouse(false);

	CGameImage imgTemp;
	HRGN hRgn = NULL;
	/// 开始按钮
	_stprintf_s(szPath, sizeof(szPath), _T("image\\start_bt.bmp"), SKIN_FOLDER);
	m_btnStart.LoadButtonBitmap(szPath,false);
	imgTemp.SetLoadInfo(szPath,true);
	hRgn=AFCBmpToRgn(imgTemp,RGB(255,0,255),RGB(1,1,1));
	if(NULL != hRgn)
	{
		m_btnStart.SetWindowRgn(hRgn,true);
	}

	// 离开按钮
	_stprintf_s(szPath, sizeof(szPath), _T("image\\leave_bt.bmp"), SKIN_FOLDER);
	m_btnLeave.LoadButtonBitmap(szPath,false);
	imgTemp.SetLoadInfo(szPath,true);
	hRgn=AFCBmpToRgn(imgTemp,RGB(255,0,255),RGB(1,1,1));
	if(NULL != hRgn)
	{
		m_btnLeave.SetWindowRgn(hRgn,true);
	}

	// 出牌按钮
	_stprintf_s(szPath, sizeof(szPath), _T("image\\outcard_bt.bmp"), SKIN_FOLDER);
	m_btnOutCard.LoadButtonBitmap(szPath,false);
	imgTemp.SetLoadInfo(szPath,true);
	hRgn=AFCBmpToRgn(imgTemp,RGB(255,0,255),RGB(1,1,1));
	if(NULL != hRgn)
	{
		m_btnOutCard.SetWindowRgn(hRgn,true);
	}

	// 不出按钮
	_stprintf_s(szPath, sizeof(szPath), _T("image\\pass_bt.bmp"), SKIN_FOLDER);
	m_btnPass.LoadButtonBitmap(szPath,false);
	imgTemp.SetLoadInfo(szPath,true);
	hRgn=AFCBmpToRgn(imgTemp,RGB(255,0,255),RGB(1,1,1));
	if(NULL != hRgn)
	{
		m_btnPass.SetWindowRgn(hRgn,true);
	}

	// 提示
	_stprintf_s(szPath, sizeof(szPath), _T("image\\hint_bt.bmp"), SKIN_FOLDER);
	m_btnHint.LoadButtonBitmap(szPath,false);
	imgTemp.SetLoadInfo(szPath,true);
	hRgn=AFCBmpToRgn(imgTemp,RGB(255,0,255),RGB(1,1,1));
	if(NULL != hRgn)
	{
		m_btnHint.SetWindowRgn(hRgn,true);
	}
///////筹码按钮
		// 100按钮
	_stprintf_s(szPath, sizeof(szPath), _T("image\\100_bt.bmp"), SKIN_FOLDER);
	m_btn100.LoadButtonBitmap(szPath,false);
	imgTemp.SetLoadInfo(szPath,true);
	hRgn=AFCBmpToRgn(imgTemp,RGB(255,0,255),RGB(1,1,1));
	if(NULL != hRgn)
	{
		m_btn100.SetWindowRgn(hRgn,true);
	}
		// 1000按钮
	_stprintf_s(szPath, sizeof(szPath), _T("image\\1000_bt.bmp"), SKIN_FOLDER);
	m_btn1000.LoadButtonBitmap(szPath,false);
	imgTemp.SetLoadInfo(szPath,true);
	hRgn=AFCBmpToRgn(imgTemp,RGB(255,0,255),RGB(1,1,1));
	if(NULL != hRgn)
	{
		m_btn1000.SetWindowRgn(hRgn,true);
	}
		// 1w按钮
	_stprintf_s(szPath, sizeof(szPath), _T("image\\1W_bt.bmp"), SKIN_FOLDER);
	m_btn1w.LoadButtonBitmap(szPath,false);
	imgTemp.SetLoadInfo(szPath,true);
	hRgn=AFCBmpToRgn(imgTemp,RGB(255,0,255),RGB(1,1,1));
	if(NULL != hRgn)
	{
		m_btn1w.SetWindowRgn(hRgn,true);
	}
		// 10w按钮
	_stprintf_s(szPath, sizeof(szPath), _T("image\\10W_bt.bmp"), SKIN_FOLDER);
	m_btn10w.LoadButtonBitmap(szPath,false);
	imgTemp.SetLoadInfo(szPath,true);
	hRgn=AFCBmpToRgn(imgTemp,RGB(255,0,255),RGB(1,1,1));
	if(NULL != hRgn)
	{
		m_btn10w.SetWindowRgn(hRgn,true);
	}
		// 100w按钮
	_stprintf_s(szPath, sizeof(szPath), _T("image\\100W_bt.bmp"), SKIN_FOLDER);
	m_btn100w.LoadButtonBitmap(szPath,false);
	imgTemp.SetLoadInfo(szPath,true);
	hRgn=AFCBmpToRgn(imgTemp,RGB(255,0,255),RGB(1,1,1));
	if(NULL != hRgn)
	{
		m_btn100w.SetWindowRgn(hRgn,true);
	}
	// 500w按钮
#ifdef SET72
	if(m_bUse500WJeton)
	{
		_stprintf_s(szPath, sizeof(szPath), _T("image\\500W_bt.bmp"), SKIN_FOLDER);
	}
	else
	{
		_stprintf_s(szPath, sizeof(szPath), _T("image\\1000W_bt.bmp"), SKIN_FOLDER); 
		MsgPrint("zhangjing::BBBBm_bUse500WJeton=%d",m_bUse500WJeton);
	}

	
#else
	_stprintf_s(szPath, sizeof(szPath), _T("image\\500W_bt.bmp"), SKIN_FOLDER);	
#endif
	
	m_btn500w.LoadButtonBitmap(szPath,false);
	imgTemp.SetLoadInfo(szPath,true);
	hRgn=AFCBmpToRgn(imgTemp,RGB(255,0,255),RGB(1,1,1));
	if(NULL != hRgn)
	{
		m_btn500w.SetWindowRgn(hRgn,true);
	}
//
	// 信息移动左键按钮
	_stprintf_s(szPath, sizeof(szPath), _T("image\\GoAhead_bt.bmp"), SKIN_FOLDER);
	m_btnleft.LoadButtonBitmap(szPath,false);
	imgTemp.SetLoadInfo(szPath,true);
	hRgn=AFCBmpToRgn(imgTemp,RGB(255,0,255),RGB(1,1,1));
	if(NULL != hRgn)
	{
		m_btnleft.SetWindowRgn(hRgn,true);
	}

	//信息移动右键按钮
	_stprintf_s(szPath, sizeof(szPath), _T("image\\GoNext_bt.bmp"), SKIN_FOLDER);
	m_btnright.LoadButtonBitmap(szPath,false);
	imgTemp.SetLoadInfo(szPath,true);
	hRgn=AFCBmpToRgn(imgTemp,RGB(255,0,255),RGB(1,1,1));
	if(NULL != hRgn)
	{
		m_btnright.SetWindowRgn(hRgn,true);
	}

	// 查看路子按钮
	_stprintf_s(szPath, sizeof(szPath), _T("image\\chakan_bt.bmp"), SKIN_FOLDER);
	m_btnluzi.LoadButtonBitmap(szPath,false);
	imgTemp.SetLoadInfo(szPath,true);
	hRgn=AFCBmpToRgn(imgTemp,RGB(255,0,255),RGB(1,1,1));
	if(NULL != hRgn)
	{
		m_btnluzi.SetWindowRgn(hRgn,true);
	}

	//申请上庄按钮
	_stprintf_s(szPath, sizeof(szPath), _T("image\\shenqing_bt.bmp"), SKIN_FOLDER);
	m_btnshangz.LoadButtonBitmap(szPath,false);
	imgTemp.SetLoadInfo(szPath,true);
	hRgn=AFCBmpToRgn(imgTemp,RGB(255,0,255),RGB(1,1,1));
	if(NULL != hRgn)
	{
		m_btnshangz.SetWindowRgn(hRgn,true);
	}
	
	///银行按钮
	_stprintf_s(szPath, sizeof(szPath), _T("image\\bank_bt.bmp"), SKIN_FOLDER);
	m_btnbank.LoadButtonBitmap(szPath,false);
	imgTemp.SetLoadInfo(szPath,true);
	hRgn=AFCBmpToRgn(imgTemp,RGB(255,0,255),RGB(1,1,1));
	if(NULL != hRgn)
	{
		m_btnbank.SetWindowRgn(hRgn,true);
	}

	//上翻按钮
	_stprintf_s(szPath, sizeof(szPath), _T("image\\top_bt.bmp"), SKIN_FOLDER);
	m_btTop.LoadButtonBitmap(szPath,false);
	imgTemp.SetLoadInfo(szPath,true);
	hRgn=AFCBmpToRgn(imgTemp,RGB(255,0,255),RGB(1,1,1));
	if(NULL != hRgn)
	{
		m_btTop.SetWindowRgn(hRgn,true);
	}

	
	//下翻按钮
	_stprintf_s(szPath, sizeof(szPath), _T("image\\bottom_bt.bmp"), SKIN_FOLDER);
	m_btBottom.LoadButtonBitmap(szPath,false);
	imgTemp.SetLoadInfo(szPath,true);
	hRgn=AFCBmpToRgn(imgTemp,RGB(255,0,255),RGB(1,1,1));
	if(NULL != hRgn)
	{
		m_btBottom.SetWindowRgn(hRgn,true);
	}

	////OutputDebugString("CClientPlayView::OnCreate 3");
	/// 超级客户端屏蔽机器人下注的按钮
	_stprintf_s(szPath, sizeof(szPath), _T("image\\hide_bt.bmp"), SKIN_FOLDER);
	m_btnHideAIMoney.LoadButtonBitmap(szPath,false);
	imgTemp.SetLoadInfo(szPath,true);
	hRgn=AFCBmpToRgn(imgTemp,RGB(255,0,255),RGB(1,1,1));
	if(NULL != hRgn)
	{
		m_btnHideAIMoney.SetWindowRgn(hRgn,true);
	}
	m_btnHideAIMoney.SetWindowTextA("隐藏机器人注");

	/// 超级客户端显示机器人控制信息的按钮
	_stprintf_s(szPath, sizeof(szPath), _T("image\\hide_bt.bmp"), SKIN_FOLDER);
	m_btnShowAIInfo.LoadButtonBitmap(szPath,false);
	imgTemp.SetLoadInfo(szPath,true);
	hRgn=AFCBmpToRgn(imgTemp,RGB(255,0,255),RGB(1,1,1));
	if(NULL != hRgn)
	{
		m_btnShowAIInfo.SetWindowRgn(hRgn,true);
	}
	m_btnShowAIInfo.SetWindowTextA("机器人信息");


	///超级客户端控制开庄的按钮
	_stprintf_s(szPath, sizeof(szPath), _T("image\\zhuang_bt.bmp"), SKIN_FOLDER);
	m_btnCtrZhuang.LoadButtonBitmap(szPath,false);
	imgTemp.SetLoadInfo(szPath,true);
	hRgn=AFCBmpToRgn(imgTemp,RGB(255,0,255),RGB(1,1,1));
	if(NULL != hRgn)
	{
		m_btnCtrZhuang.SetWindowRgn(hRgn,true);
	}

	///超级客户端控制开闲的按钮
	_stprintf_s(szPath, sizeof(szPath), _T("image\\xian_bt.bmp"), SKIN_FOLDER);
	m_btnCtrXian.LoadButtonBitmap(szPath,false);
	imgTemp.SetLoadInfo(szPath,true);
	hRgn=AFCBmpToRgn(imgTemp,RGB(255,0,255),RGB(1,1,1));
	if(NULL != hRgn)
	{
		m_btnCtrXian.SetWindowRgn(hRgn,true);
	}
	///超级客户端控制开和的按钮
	_stprintf_s(szPath, sizeof(szPath), _T("image\\he_bt.bmp"), SKIN_FOLDER);
	m_btnCtrHe.LoadButtonBitmap(szPath,false);
	imgTemp.SetLoadInfo(szPath,true);
	hRgn=AFCBmpToRgn(imgTemp,RGB(255,0,255),RGB(1,1,1));
	if(NULL != hRgn)
	{
		m_btnCtrHe.SetWindowRgn(hRgn,true);
	}

	///超级客户端恢复控制的按钮
	_stprintf_s(szPath, sizeof(szPath), _T("image\\normal_bt.bmp"), SKIN_FOLDER);
	m_btnCtrNormal.LoadButtonBitmap(szPath,false);
	imgTemp.SetLoadInfo(szPath,true);
	hRgn=AFCBmpToRgn(imgTemp,RGB(255,0,255),RGB(1,1,1));
	if(NULL != hRgn)
	{
		m_btnCtrNormal.SetWindowRgn(hRgn,true);
	}

	///显示超级对话框的按钮
	_stprintf_s(szPath, sizeof(szPath), _T("image\\superdlg_bt.bmp"), SKIN_FOLDER);
	m_btnCtrSuper.LoadButtonBitmap(szPath,false);
	imgTemp.SetLoadInfo(szPath,true);
	hRgn=AFCBmpToRgn(imgTemp,RGB(255,0,255),RGB(1,1,1));
	if(NULL != hRgn)
	{
		m_btnCtrSuper.SetWindowRgn(hRgn,true);
	}

	m_SuperDlg.Create(IDD_SUPERGM,this);
	m_SuperDlg.ShowWindow(SW_HIDE);	
	m_SuperDlg.ShowWindow(false);

	m_LuziDlg.Create(IDD_LUZI,this);
	m_LuziDlg.ShowWindow(SW_HIDE);	
	m_LuziDlg.ShowWindow(false);

	///显示机器人信息对话框
	m_ShowAIInfo.Create(IDD_SHOW_AI_INFO,this);
	m_ShowAIInfo.ShowWindow(SW_HIDE);	
	m_ShowAIInfo.ShowWindow(false);

	return 0;
}

///绘画界面函数
void CClientPlayView::DrawViewFace(CDC * pDC, int iWidth, int iHeight)
{
	if (NULL == pDC)
	{
		return ;
	}
	
	m_pDC = pDC;
	m_iWidth = iWidth;
	m_iHeight = iHeight;

	m_showcoredlg.ShowWindow(false);//隐藏积分榜
	/// 先画背景
	DrawBackground(pDC, iWidth, iHeight);	
	CFont font, *pOldFont;
	font.CreateFont(12,0,0,0,0,0,0,0,134,3,2,1,2,TEXT("宋体")); 
	pOldFont = pDC->SelectObject(&font);
	pDC->SetTextColor(RGB(255,255,102));
	/// 绘制人物头像
	CRect rectName(0,0,0,0);
	int i=0;
	for(i=0; i<PLAY_COUNT; i++)  // 本身为视图
	{
		if (NULL != m_pUserItem[i])
		{
			UserInfoStruct * pUserInfo=&m_pUserItem[i]->GameUserInfo;
			if (NULL == pUserInfo)
			{
				continue ;
			}
		}
	}
	///lym0512绘制金币
	DrawJinBi(pDC, iWidth, iHeight);

	DrawRect(pDC, iWidth, iHeight);
	///绘制庄信息，庄列表
	DrawNtInfo(pDC, iWidth, iHeight);

	///绘制玩家自己的信息：金币，命中率，总计成绩
	DrawMeInfo(pDC, iWidth, iHeight);

	//绘制历史结果信息：最近10局的庄闲点数，最近30局庄闲和开牌
	DrawResultInfo(pDC, iWidth, iHeight);

	///绘制下注信息：八个区域的下注，庄闲和可下注的限额
	DrawXiaZhuInfo(pDC, iWidth, iHeight);

	///绘制动态信息：无庄（提示），下注（提示下注），开牌（逐个提示开牌信息），空闲时间
	DrawActiveInfo(pDC, iWidth, iHeight);

	///绘制牌
	DrawCard(pDC, iWidth, iHeight);

	///发牌动画
	DrawMoveCard(pDC, iWidth, iHeight);
	
	/// 视图变化了
	if (m_byMyStation !=255 && 0 != m_uTime[m_byMyStation])
	{
		DrawTimer(pDC, m_PtTime[m_byMyStation].x, m_PtTime[m_byMyStation].y, m_uTime[m_byMyStation]);
	}

	DrawCursor(pDC, iWidth, iHeight);//绘制光标图片

	//绘制透明结算框
	DrawResultFrame(pDC, m_pDitu.x-5, m_pDitu.y+42);

	//是超级客户端就绘制控制信息
	if(m_bShowSuper)
	{
		CFont font1, *pFont2;
		font1.CreateFont(12,0,0,0,0,0,0,0,134,3,2,1,2,TEXT("宋体")); 
		pFont2 = pDC->SelectObject(&font1);
		CRect rect;
		rect.SetRect(iWidth/2-300+60,iHeight/2+218-60,iWidth/2-300+220,iHeight/2+218-45);
		pDC->DrawText(m_sSuperInfo,m_sSuperInfo.GetLength(),rect,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
		pDC->SelectObject(pFont2);
		font1.DeleteObject();
	}
	Graphics *pGDIAdd = Graphics::FromHDC(pDC->GetSafeHdc());
	DrawFullNoteBubble(pGDIAdd, iWidth, iHeight);
	pGDIAdd->ReleaseHDC(pDC->GetSafeHdc());
	pDC->SelectObject(pOldFont);	
	font.DeleteObject();

	///提示开牌图片
	DrawPromptOpenCard(pDC,iWidth,iHeight);

	///提示换庄图片
	DrawPromptChangeNt(pDC,iWidth,iHeight);
}
void CClientPlayView::DrawFullNoteBubble(Graphics* pGDIAdd, int iWidth, int iHeight)
{
	if (false == m_bFullNoteDisplay)
	{
		return;
	}
	TCHAR m_sz[MAX_PATH];
	sprintf(m_sz, "image\\fullnote_bubble.png");
#ifndef NO_PICTURE_COMPRESSION
	WCHAR wPath[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, m_sz, sizeof(m_sz), wPath, sizeof(wPath));
	Bitmap * pbmTimerType = new Bitmap(wPath);
#else
	CUnZipRes m_unZip;
	Bitmap * pbmTimerType = new Bitmap(m_unZip.LoadFileResFromZip(m_sz));
#endif

	RectF rFXY(REAL((iWidth-pbmTimerType->GetWidth())/2-20), REAL((iHeight-pbmTimerType->GetHeight())/2+170),pbmTimerType->GetWidth(),pbmTimerType->GetHeight());	
	pGDIAdd->DrawImage(pbmTimerType, rFXY);
	delete pbmTimerType;
	pbmTimerType = NULL;
	return;
}

///  设置gif
void CClientPlayView::SetGif(BYTE bDeskView,int iValue,int iTimer)
{
	if (NULL == m_pUserItem[bDeskView])
	{
		return ;
	}
	// 视图转换为绝对位置
	UserInfoStruct * pUserInfo=&m_pUserItem[bDeskView]->GameUserInfo;
	if (NULL != pUserInfo)
	{
		bDeskView =  pUserInfo->bDeskStation;   // 将原来的视图位置转换为逻辑位置
	}
	__super::SetGif(bDeskView, iValue, iTimer);
}

bool CClientPlayView::SetUserTimer(BYTE bViewStation, UINT uTime)
{
	// 视图转换为绝对位置
	if (NULL == m_pUserItem[bViewStation])
	{
		return false;
	}
	else
	{
		UserInfoStruct * pUserInfo=&m_pUserItem[bViewStation]->GameUserInfo;
		if (NULL != pUserInfo)
		{
			bViewStation =  pUserInfo->bDeskStation;   // 将原来的视图位置转换为逻辑位置
		}
	}
	return __super::SetUserTimer(bViewStation, uTime);
}

/// 桌号换算到视图位置
BYTE CClientPlayView::GetViewSeat(BYTE byDeskStation)
{
	CClientGameDlg *pGameDlg=CONTAINING_RECORD(this,CClientGameDlg, m_PlayView);
	if (NULL == pGameDlg)
	{
		return 0;
	}
	byDeskStation = byDeskStation%PLAY_COUNT;
	return pGameDlg->GetViewSeat(byDeskStation);
}


///名称：DrawBackground
///描述：绘制游戏背景
///@param pDC 绘图dc  iWidth 宽度  iHeight 高度
///@return  
void CClientPlayView::DrawBackground(CDC * pDC, int iWidth, int iHeight)
{
	if (NULL == pDC)
	{
		return ;
	}
	int x = 0;
	int y = 0;

	// 在有宽高的情况下绘制背景
	CGameImageHelper imgHelper(&m_imgBackground);

	if ((imgHelper.GetWidth() > 0) && (imgHelper.GetHeight() > 0))
	{		
		for (x=0; x<iWidth; (x += imgHelper.GetWidth()))
		{
			for (y=0; y<iHeight; (y += imgHelper.GetHeight()))
			{
				imgHelper.BitBlt(pDC->GetSafeHdc(), x, y);
			}
		}
	}
	// 绘制界面背景图
	CGameImageHelper imgCenterLogo2(&m_imgCenterLogo2);
	x = (iWidth - imgCenterLogo2.GetWidth())/2;
	y = (iHeight - imgCenterLogo2.GetHeight())/2;
	imgCenterLogo2.BitBlt(pDC->GetSafeHdc(), x, y);	



	// 绘制中心图片
	CGameImageHelper imgCenterLogo(&m_imgCenterLogo);
	x = (iWidth - imgCenterLogo.GetWidth())/2;
	y = (iHeight - imgCenterLogo.GetHeight())/2;
	imgCenterLogo.BitBlt(pDC->GetSafeHdc(), x, y);	
}


///名称：DrawAwardPoint
///描述：绘制奖分
///@param pDC 绘图dc 。 x 位置 。 y 位置 。 iNum 数量。
///@return  
void CClientPlayView::DrawAwardPoint(CDC * pDC, int x, int y,BYTE iNum)
{
	if (NULL == pDC)
	{
		return ;
	}
	CGameImageHelper	imgAwardPint(&m_imgAwardPint);
	CDC dc;
	dc.CreateCompatibleDC(pDC);
	CBitmap *pOldBmp = (CBitmap*)dc.SelectObject(imgAwardPint);
	for(int i=0; i<iNum; ++i)
	{
		::TransparentBlt(pDC->GetSafeHdc(),x+i*30,y,imgAwardPint.GetWidth(),imgAwardPint.GetHeight(),
			dc.GetSafeHdc(),0,0,imgAwardPint.GetWidth(),imgAwardPint.GetHeight(),dc.GetPixel(0,0));
	}
	dc.SelectObject(pOldBmp);
	dc.DeleteDC();
}

///名称：DrawNt
///描述：绘画 NT 标志
///@param pDC 绘图dc 。 x 位置 。 y 位置 。 
///@return  
void CClientPlayView::DrawNt(CDC * pDC, int x, int y)
{
	if (NULL == pDC)
	{
		return ;
	}

	CGameImageHelper	imgNt(&m_imgNT);
	CDC dc;
	dc.CreateCompatibleDC(pDC);
	CBitmap *OldBmp = (CBitmap*)dc.SelectObject(imgNt);
	::TransparentBlt(pDC->GetSafeHdc(),x,y,imgNt.GetWidth(),imgNt.GetHeight(),
		dc.GetSafeHdc(),0,0,imgNt.GetWidth(),imgNt.GetHeight(),dc.GetPixel(0,0));
	dc.SelectObject(OldBmp);
	dc.DeleteDC();
	return;
}

///名称：DrawTrustee
///描述：绘制托管标志
///@param pDC 绘图dc 。 x 位置 。 y 位置 。 
///@return  
void CClientPlayView::DrawTrustee(CDC * pDC, int x, int y)
{
	if (NULL == pDC)
	{
		return ;
	}

	CGameImageHelper	imgTrustee(&m_imgTrusteeLogo);
	CDC dc;
	dc.CreateCompatibleDC(pDC);
	CBitmap *pOldBmp = (CBitmap*)dc.SelectObject(imgTrustee);
	::TransparentBlt(pDC->GetSafeHdc(),x,y,imgTrustee.GetWidth(),imgTrustee.GetHeight(),
		dc.GetSafeHdc(),0,0,imgTrustee.GetWidth(),imgTrustee.GetHeight(),dc.GetPixel(0,0));
	dc.SelectObject(pOldBmp);
	dc.DeleteDC();
}

///绘制出牌先后顺序
void CClientPlayView::DrawSequence(CDC * pDC, int x, int y,int iSequence)
{
	TCHAR szName[MAX_PATH] = {0};
	CDC dc;
	dc.CreateCompatibleDC(pDC);
	_stprintf_s(szName, sizeof(szName), _T("image\\finish_out%d.bmp"),SKIN_FOLDER, iSequence);
	m_imgPicSequence.SetLoadInfo(szName,false);		// 出完牌顺序图片
	CGameImageHelper	PicSequence(&m_imgPicSequence);
	CBitmap *pOldBmp = (CBitmap*)dc.SelectObject(PicSequence);
	::TransparentBlt(pDC->GetSafeHdc(), x, y,PicSequence.GetWidth(),PicSequence.GetHeight(),
		dc.GetSafeHdc(),0,0,PicSequence.GetWidth(),PicSequence.GetHeight(),dc.GetPixel(0,0));
	dc.SelectObject(pOldBmp);
	dc.DeleteDC();
}

/// 绘制不出标志
void CClientPlayView::DrawPass(CDC * pDC, int x, int y)
{
	if (NULL == pDC)
	{
		return ;
	}
	
	CGameImageHelper	imgPass(&m_imgPass);
	CDC dc;
	dc.CreateCompatibleDC(pDC);
	CBitmap *pOldBmp = (CBitmap*)dc.SelectObject(imgPass);
	::TransparentBlt(pDC->GetSafeHdc(),x,y,imgPass.GetWidth(),imgPass.GetHeight(),
		dc.GetSafeHdc(),0,0,imgPass.GetWidth(),imgPass.GetHeight(),dc.GetPixel(0,0));
	dc.SelectObject(pOldBmp);
	dc.DeleteDC();
}

/// 设置方向
void CClientPlayView::SetCurOrientation(BYTE byCurOrientationViewSeat)
{
	m_byCurOrientation = byCurOrientationViewSeat;
	UpdateViewFace(NULL);
}

/// 绘制方向
void CClientPlayView::DrawOrientation(CDC *pDC, int x, int y, int iView)
{
	if (NULL == pDC)
	{
		return ;
	}

	CDC dc;
	dc.CreateCompatibleDC(pDC);
	CGameImageHelper img(&m_imgFangXiang);
	CBitmap *pOldBmp = (CBitmap*)dc.SelectObject(img);
	TransparentBlt(pDC->GetSafeHdc(),m_PtCurOrientation[iView].x,m_PtCurOrientation[iView].y,
			img.GetWidth()/8,img.GetHeight(),dc.GetSafeHdc(),iView*img.GetWidth()/8,0,
			img.GetWidth()/8,img.GetHeight(),RGB(255,0,255));
	dc.SelectObject(pOldBmp);
	dc.DeleteDC();
}

/// 设置等待状态
void CClientPlayView::SetWaitingType(BYTE byType)
{
	// 隐藏游戏开始、离开按钮
	m_btnStart.ShowWindow(SW_HIDE);
	m_btnLeave.ShowWindow(SW_HIDE);

	// 更新游戏等待状态
	m_byWaitingType=byType;
	UpdateViewFace(NULL);
}

///名称：DrawWaitingImg
///描述：绘制等待状态图片
///@param pDC 绘图dc 。 iWidth 窗口宽度 。 iHeight 窗口高度 。 
///@return  
void CClientPlayView::DrawWaitingImg(CDC * pDC, int iWidth, int iHeight)
{
	if (NULL == pDC)
	{
		return ;
	}
	CGameImageHelper	imgWaiting(&m_imgWaiting);

	int x = (iWidth - imgWaiting.GetWidth())/2;
	int y = (iHeight - imgWaiting.GetHeight())/2;
	CDC dc;
	dc.CreateCompatibleDC(pDC);
	CBitmap *pOldBmp = (CBitmap*)dc.SelectObject(imgWaiting);
	::TransparentBlt(pDC->GetSafeHdc(),x,y,imgWaiting.GetWidth(),imgWaiting.GetHeight(),
		dc.GetSafeHdc(),0,0,imgWaiting.GetWidth(),imgWaiting.GetHeight(),dc.GetPixel(0,0));
	dc.SelectObject(pOldBmp);
	dc.DeleteDC();
}

void CClientPlayView::SendCard(BYTE byViewSeat, int iTime)
{
	WORD wCardList[1]={0};	
	m_SendCardAnimal[byViewSeat].MoveAnimal(m_PtSendCardStart[byViewSeat], m_PtSendCardAnimalEnd[byViewSeat], (long)iTime);
	m_SendCardAnimal[byViewSeat].SetCard(wCardList, NULL, 1);
}

/// 设置托管状态
void CClientPlayView::SetTrustee(BYTE byViewSeat, bool bIsTrustee)
{
	m_bIsTrustee[byViewSeat] = bIsTrustee;

	CRect rect;
	rect.SetRect(m_PtTrustee[byViewSeat].x,m_PtTrustee[byViewSeat].y,
		m_PtTrustee[byViewSeat].x+m_imgTrusteeLogo.GetWidth(),
		m_PtTrustee[byViewSeat].y+m_imgTrusteeLogo.GetHeight());
	UpdateViewFace(rect);

	// 如果是自己发送的托管消息,更改托管按钮图片
	if ((PLAY_COUNT/2) == byViewSeat)
	{
		TCHAR szName[MAX_PATH]={0};
		if (bIsTrustee)
		{
			_stprintf_s(szName, sizeof(szName), _T("image\\Trustee_less.bmp"),SKIN_FOLDER);
		}
		else
		{
			_stprintf_s(szName, sizeof(szName), _T("image\\Trustee_bt.bmp"),SKIN_FOLDER);
		}
		//m_btnTrustee.LoadButtonBitmap(szName,false);
	}
}


void CClientPlayView::OnBtnClickedLeave(void)
{
	CClientGameDlg *pGameDlg=CONTAINING_RECORD(this,CClientGameDlg,m_PlayView);
	if (NULL != pGameDlg)
	{
		pGameDlg->SendMessage(WM_CLOSE,0,0);
	}
}

///100筹码消息
void CClientPlayView::OnBtnClicked100(void)
{
	if(m_bNowGameState != GS_SEND_CARD)//非下注时间不响应
		return;
	if(	m_byMyStation==m_iNowNTstation)//自己的位置,庄家不能下注
		return;

	//OnShowCursor(FALSE);
	m_iChooseNote =1;//当前选择的筹码 0没有，1：100，2：1000，3：1w，4：10w，5：100w,6：500w
	EnableXiaZhu(true);
	m_btn100.EnableWindow(false);	// 100筹码按钮

}
///1000筹码消息
void CClientPlayView::OnBtnClicked1000(void)
{
	if(m_bNowGameState != GS_SEND_CARD)//非下注时间不响应
		return;
	if(	m_byMyStation==m_iNowNTstation)//自己的位置,庄家不能下注
		return;
	
	//SetCursor(LoadCursor(NULL, IDC_CROSS)); //设置光标、
	//SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(32649)));
	//OnShowCursor(FALSE);
	m_iChooseNote =2;//当前选择的筹码 0没有，1：100，2：1000，3：1w，4：10w，5：100w,6：500w
	EnableXiaZhu(true);
	m_btn1000.EnableWindow(false);	// 1000筹码按钮

}
///1万筹码消息
void CClientPlayView::OnBtnClicked1w(void)
{
	if(m_bNowGameState != GS_SEND_CARD)//非下注时间不响应
		return;
	if(	m_byMyStation==m_iNowNTstation)//自己的位置,庄家不能下注
		return;

	//SetCursor(LoadCursor(NULL, IDC_CROSS)); //设置光标
	//SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(32649)));
	//OnShowCursor(FALSE);
	m_iChooseNote =3;//当前选择的筹码 0没有，1：100，2：1000，3：1w，4：10w，5：100w,6：500w
	EnableXiaZhu(true);
	m_btn1w.EnableWindow(false);		// 1万筹码按钮

}
///10万筹码消息
void CClientPlayView::OnBtnClicked10w(void)
{
	if(m_bNowGameState != GS_SEND_CARD)//非下注时间不响应
		return;
	if(	m_byMyStation==m_iNowNTstation)//自己的位置,庄家不能下注
		return;

	//SetCursor(LoadCursor(NULL, IDC_CROSS)); //设置光标
	//SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(32649)));
	//OnShowCursor(FALSE);
	m_iChooseNote =4;//当前选择的筹码 0没有，1：100，2：1000，3：1w，4：10w，5：100w,6：500w
	EnableXiaZhu(true);
	m_btn10w.EnableWindow(false);	// 10万筹码按钮
}
///100万筹码消息
void CClientPlayView::OnBtnClicked100w(void)
{
	if(m_bNowGameState != GS_SEND_CARD)//非下注时间不响应
		return;
	if(	m_byMyStation==m_iNowNTstation)//自己的位置,庄家不能下注
		return;
	
	//SetCursor(LoadCursor(NULL, IDC_CROSS)); //设置光标
	//SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(32649)));
	//OnShowCursor(FALSE);
	m_iChooseNote =5;//当前选择的筹码 0没有，1：100，2：1000，3：1w，4：10w，5：100w,6：500w
	EnableXiaZhu(true);
	m_btn100w.EnableWindow(false);	// 100万筹码按钮
}
///500万筹码消息
void CClientPlayView::OnBtnClicked500w(void)
{
	if(m_bNowGameState != GS_SEND_CARD)//非下注时间不响应
		return;
	if(	m_byMyStation == m_iNowNTstation)//自己的位置,庄家不能下注
		return;

	//SetCursor(LoadCursor(NULL, IDC_CROSS)); //设置光标
	//SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(32649)));
	//OnShowCursor(FALSE);
	m_iChooseNote =6;//当前选择的筹码 0没有，1：100，2：1000，3：1w，4：10w，5：100w,6：500w
	EnableXiaZhu(true);
	m_btn500w.EnableWindow(false);	// 500万筹码按钮
}

///名称：OnBtnClickedInfoLeft
///描述：点击历史记录信息左按钮（信息后前移动）
///@param 
///@return  
void CClientPlayView::OnBtnClickedInfoLeft(void)
{//信息
	int count=0;
	for(int i=0;i<MAXCOUNT;i++)
	{
		if(m_iGameInfo[i][2]>0)
			count++;
	}
	if(m_iShowInfoIndex<(count-10))
	{
		m_iShowInfoIndex++;
		m_btnright.EnableWindow(true);
	}
	else
	{
		if((count-10)>0)
		{	
			m_iShowInfoIndex = count-10;
		}
		else
		{
			m_iShowInfoIndex = 0;
		}
		m_btnleft.EnableWindow(false);

	}
	UpdateViewFace(NULL);
}
///名称：OnBtnClickedInfoRight
///描述：点击历史记录信息右按钮（信息向前移动）
///@param 
///@return  
void CClientPlayView::OnBtnClickedInfoRight(void)
{
	if(m_iShowInfoIndex>0)
	{
		m_iShowInfoIndex--;
		m_btnleft.EnableWindow(true);
	}
	else 
	{
		m_btnright.EnableWindow(false);
		m_iShowInfoIndex=0;
	}
	UpdateViewFace(NULL);
}
///名称：OnBtnClickedluzi
///描述：点击查看路子按钮，弹出路子对话框
///@param 
///@return  
void CClientPlayView::OnBtnClickedluzi(void)
{
	//m_Info.ShowWindow(true);
	//if(m_Info.DoModal()==IDOK)
	//{
	//}
	//弹出查看路子对话框
	CRect rect;
	GetClientRect(&rect);
	//m_LuziDlg.MoveWindow(rect.Width()/2-m_LuziDlg.m_bk.GetWidth()/2,rect.Height()/2-m_LuziDlg.m_bk.GetHeight()/2,m_LuziDlg.m_bk.GetWidth(),m_LuziDlg.m_bk.GetHeight(),TRUE);
	UpdateLuziData();
	if(m_LuziDlg.IsWindowVisible())
	{
		m_LuziDlg.ShowWindow(false);
	}
	else 
	{
		m_LuziDlg.ShowWindow(true);
	}
}
void CClientPlayView::OnBtnClickedStart(void)
{
	CClientGameDlg *pGameDlg=CONTAINING_RECORD(this,CClientGameDlg, m_PlayView);
	if (NULL != pGameDlg)
	{
		pGameDlg->SendMessage(WM_USER_GAME_BEGIN,0,0);
	}
}

///名称：OnBtnClickedShangZhuang
///描述：点击上庄按钮事件
///@param 
///@return  
void CClientPlayView::OnBtnClickedShangZhuang(void)
{
	CClientGameDlg *pGameDlg=CONTAINING_RECORD(this,CClientGameDlg,m_PlayView);
	if(!m_bCanShangZhuang)
	{
		if (m_iMessageCount>=3)
		{
			return ;			
		}
		else
		{
			m_iMessageCount++;
		}
		pGameDlg->SendMessage(WM_USER_SHANG_ZHUANG,0,10);
		return ;
	}
///lym2010-03-19能否点击上庄按钮
	SetTimer(IDR_CTL_ZHUANG_BTN,2000,NULL);
	m_bCanShangZhuang = false;

	if (NULL != pGameDlg)
	{
		TCHAR szName[MAX_PATH];
		if(!CheckMeZhuang())
		{
			pGameDlg->SendMessage(WM_USER_SHANG_ZHUANG,0,0);
			//变为下庄图片					
			SetApplyBanker(false);				
		}
		else
		{
			if(m_pUserItem[m_byMyStation]!=NULL)
			{
				pGameDlg->SendMessage(WM_USER_SHANG_ZHUANG,0,1);
				//变为上庄图片
				SetApplyBanker(true);
			}
		}
	}
}

///名称：OnBtnClickedBank
///描述：点击银行按钮事件
///@param 
///@return  
void CClientPlayView::OnBtnClickedBank(void)
{
	CClientGameDlg *pGameDlg=CONTAINING_RECORD(this,CClientGameDlg,m_PlayView);
	if (NULL != pGameDlg)
	{
		pGameDlg->SendMessage(WM_USER_BANK,0,0);
	}
}

///名称：OnBtnClickedHideMoney
///描述：点击超级客户端屏蔽机器人下注的按钮
///@param 
///@return  
void CClientPlayView::OnBtnClickedHideMoney(void)
{
	m_bHideAIMoney = m_bHideAIMoney;///是否隐藏机器人下注
	UpdateViewFace(NULL);
}

///超级客户端显示机器人控制信息的按钮
///@param 
///@return  
void CClientPlayView::OnBtnClickedShowAIInfo(void)
{
	if(m_ShowAIInfo.IsWindowVisible())
	{
		m_ShowAIInfo.ShowWindow(false);
	}
	else 
	{
		m_ShowAIInfo.ShowAIInfoWindow();

	}

	UpdateViewFace(NULL);
}

///名称：OnBtnClickedCtrZhuang
///描述：点击超级客户端控制开庄的按钮
///@param 
///@return  
void CClientPlayView::OnBtnClickedCtrZhuang(void)
{
	CClientGameDlg *pGameDlg=CONTAINING_RECORD(this,CClientGameDlg,m_PlayView);
	if(pGameDlg!=NULL)
	{
		pGameDlg->SendMessage(WM_SUPER_CTR,1,0);
	}

}

///名称：OnBtnClickedCtrXian
///描述：点击超级客户端控制开闲的按钮
///@param 
///@return  
void CClientPlayView::OnBtnClickedCtrXian(void)
{
	
	CClientGameDlg *pGameDlg=CONTAINING_RECORD(this,CClientGameDlg,m_PlayView);
	if(pGameDlg!=NULL)
	{
		pGameDlg->SendMessage(WM_SUPER_CTR,2,0);
	}
}

///名称：OnBtnClickedCtrHe
///描述：点击超级客户端控制开和的按钮
///@param 
///@return  
void CClientPlayView::OnBtnClickedCtrHe(void)
{
	CClientGameDlg *pGameDlg=CONTAINING_RECORD(this,CClientGameDlg,m_PlayView);
	if(pGameDlg!=NULL)
	{
		pGameDlg->SendMessage(WM_SUPER_CTR,3,0);
	}	
}

///名称：OnBtnClickedCtrNormal
///描述：点击超级客户端取消控制的按钮
///@param 
///@return  
void CClientPlayView::OnBtnClickedCtrNormal(void)
{
	CClientGameDlg *pGameDlg=CONTAINING_RECORD(this,CClientGameDlg,m_PlayView);
	if(pGameDlg!=NULL)
	{
		pGameDlg->SendMessage(WM_SUPER_CTR,0,0);
	}	
}

///名称：CheckMeZhuang
///描述：检测自己是否是庄家或在庄家列表中
///@param 
///@return  
bool CClientPlayView::CheckMeZhuang(void)
{

	//if(m_iNowNTstation<0)
		//return false;
	if(m_byMyStation == m_iNowNTstation)
		return true;
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if (m_iZhuangList[i]<0)
		{
			continue;
		}
		if(m_byMyStation == m_iZhuangList[i])//我在庄家列表
			return true; 
	}
	return false;
}

///名称：UpdateNowData
///描述：更新当前开牌的信息信息
///@param 
///@return  
void CClientPlayView::UpdateNowData(void)
{
	m_iGameCount++;
	m_iPlayCount++;
	
	/*CClientGameDlg *pGameDlg=CONTAINING_RECORD(this,CClientGameDlg, m_PlayView);
	if (NULL != pGameDlg)
	{
		for(int i=0;i<MAXCOUNT;i++)
		{ 
			for(int j=0;j<4;j++)
			{
				m_iGameInfo[i][j] = pGameDlg->m_iGameInfo[i][j];
			}
		}
	}*/

	if(m_iGameInfo[MAXCOUNT-1][2]>0)
	{
		for(int i=1;i<MAXCOUNT;i++)
		{
			m_iMePlayData[i-1]=m_iMePlayData[i];
		}
	}

	for(int i=MAXCOUNT-1;i>=0;i--)
	{
		if(m_iGameInfo[i][2]>0)
		{
			m_iMePlayData[i] = 1;
			m_byMePlayData[i][0] = 1;
			m_byMePlayData[i][1] = 1;
			m_byMePlayData[i][2] = 1;
			if(m_iKaiPai == 1)
			{
				if(m_iMyZhu[0]>0)
				{
					m_iMePlayData[i] = 2;//我赢
					m_byMePlayData[i][0] = 2;
				}
				else if(m_iMyZhu[1]>0 || m_iMyZhu[2]>0)//其他区域有下注就是输
				{
					m_byMePlayData[i][0] = 3;
				}
				//闲
				if(m_iMyZhu[3]>0 || m_iMyZhu[4]>0 || m_iMyZhu[5]>0)
				{
					m_byMePlayData[i][1] = 3;
				}
				//和
				if(m_iMyZhu[6]>0 || m_iMyZhu[7]>0)
				{
					m_byMePlayData[i][2] = 3;
				}
			}
			else if(m_iKaiPai == 2)
			{
				if(m_iMyZhu[0]>0 || m_iMyZhu[1]>0)
				{
					m_iMePlayData[i] = 2;//我赢
					m_byMePlayData[i][0] = 2;
				}
				else if(m_iMyZhu[2]>0 )//其他区域有下注就是输
				{
					m_byMePlayData[i][0] = 3;
				}

				//闲
				if(m_iMyZhu[3]>0 || m_iMyZhu[4]>0 || m_iMyZhu[5]>0)
				{
					m_byMePlayData[i][1] = 3;
				}
				//和
				if(m_iMyZhu[6]>0 || m_iMyZhu[7]>0)
				{
					m_byMePlayData[i][2] = 3;
				}
			}
			else if(m_iKaiPai == 3)
			{
				if(m_iMyZhu[0]>0 || m_iMyZhu[2]>0)
				{
					m_iMePlayData[i] = 2;//我赢
					m_byMePlayData[i][0] = 2;
				}
				else if(m_iMyZhu[1]>0 )//其他区域有下注就是输
				{
					m_byMePlayData[i][0] = 3;
				}
				//闲
				if(m_iMyZhu[3]>0 || m_iMyZhu[4]>0 || m_iMyZhu[5]>0)
				{
					m_byMePlayData[i][1] = 3;
				}
				//和
				if(m_iMyZhu[6]>0 || m_iMyZhu[7]>0)
				{
					m_byMePlayData[i][2] = 3;
				}

			}
			else if(m_iKaiPai == 4)
			{
				if(m_iMyZhu[3]>0)
				{
					m_iMePlayData[i] = 2;//我赢
					m_byMePlayData[i][1] = 2;
				}
				else if(m_iMyZhu[4]>0 || m_iMyZhu[5]>0)//其他区域有下注就是输
				{
					m_byMePlayData[i][1] = 3;//我输
				}				
				//庄
				if(m_iMyZhu[0]>0 || m_iMyZhu[1]>0 || m_iMyZhu[2]>0)
				{
					m_byMePlayData[i][0] = 3;
				}
				//和
				if(m_iMyZhu[6]>0 || m_iMyZhu[7]>0)
				{
					m_byMePlayData[i][2] = 3;
				}

			}
			else if(m_iKaiPai == 5)
			{
				if(m_iMyZhu[3]>0 || m_iMyZhu[4]>0)
				{
					m_iMePlayData[i] = 2;//我赢
					m_byMePlayData[i][1] = 2;
				}
				else if(m_iMyZhu[5]>0 )//其他区域有下注就是输
				{
					m_byMePlayData[i][1] = 3;
				}	
				//庄
				if(m_iMyZhu[0]>0 || m_iMyZhu[1]>0 || m_iMyZhu[2]>0)
				{
					m_byMePlayData[i][0] = 3;
				}
				//和
				if(m_iMyZhu[6]>0 || m_iMyZhu[7]>0)
				{
					m_byMePlayData[i][2] = 3;
				}
			}
			else if(m_iKaiPai == 6)
			{
				if(m_iMyZhu[3]>0 || m_iMyZhu[5]>0)
				{
					m_iMePlayData[i] = 2;//我赢
					m_byMePlayData[i][1] = 2;
				}
				else if(m_iMyZhu[4]>0 )//其他区域有下注就是输
				{
					m_byMePlayData[i][1] = 3;
				}	
				//庄
				if(m_iMyZhu[0]>0 || m_iMyZhu[1]>0 || m_iMyZhu[2]>0)
				{
					m_byMePlayData[i][0] = 3;
				}
				//和
				if(m_iMyZhu[6]>0 || m_iMyZhu[7]>0)
				{
					m_byMePlayData[i][2] = 3;
				}
			}
			else if(m_iKaiPai == 7)//7和
			{
				if(m_iMyZhu[6]>0)
				{
					m_iMePlayData[i] = 2;//我赢
					m_byMePlayData[i][2] = 2;
				}
				else if(m_iMyZhu[7]>0 )//其他区域有下注就是输
				{
					m_byMePlayData[i][2] = 3;
				}	

				//庄
				if(m_iMyZhu[0]>0 || m_iMyZhu[1]>0 || m_iMyZhu[2]>0)
				{
					m_byMePlayData[i][0] = 3;
				}
				//闲
				if(m_iMyZhu[3]>0 || m_iMyZhu[4]>0 || m_iMyZhu[5]>0)
				{
					m_byMePlayData[i][1] = 3;
				}
			}
			else if(m_iKaiPai == 8)//8同点和
			{
				if(m_iMyZhu[6]>0 || m_iMyZhu[7]>0)
				{
					m_iMePlayData[i] = 2;//我赢
					m_byMePlayData[i][2] = 2;
				}
				//庄
				if(m_iMyZhu[0]>0 || m_iMyZhu[1]>0 || m_iMyZhu[2]>0)
				{
					m_byMePlayData[i][0] = 3;
				}
				//闲
				if(m_iMyZhu[3]>0 || m_iMyZhu[4]>0 || m_iMyZhu[5]>0)
				{
					m_byMePlayData[i][1] = 3;
				}
			}
			/*CString str="";
			str.Format("---xqm输赢数据：索引 %d   : %d %d %d ",i,m_byMePlayData[i][0],m_byMePlayData[i][1],m_byMePlayData[i][2]);
			OutputDebugString(str);*/
			break;
		}
	}
	//m_iWinmoney += m_iMeFen;//自己的成绩

	//命中率 
	m_iMePlayCount = 0;
	m_iMeWinCount = 0;
	bool bMePlayCountAdd=false;
	m_iMePlayCount=0;
	m_iMeWinCount=0;
	for(int i=0;i<MAXCOUNT;i++)
	{
		if(m_iMePlayData[i]>0)
		{
			m_iMePlayCount++;
		}
		if(m_iMePlayData[i]>=2)
		{
			m_iMeWinCount++;
		}
	}

	int count=0;
	for(int i=0;i<MAXCOUNT;i++)
	{
		if(m_iGameInfo[i][2]>0)
			count++;
	}
	if(m_iShowInfoIndex<=(count-10) && m_iShowInfoIndex>0)
	{
		m_btnright.EnableWindow(true);
	}
	if(m_iShowInfoIndex<(count-10) )
	{
		m_btnleft.EnableWindow(true);
	}

	UpdateLuziData();//更新看路子信息

	m_btnright.EnableWindow(false);
	m_iShowInfoIndex=0;

	UpdateViewFace(NULL);
}

///名称：UpdateLuziData
///描述：更新路子信息
///@param 
///@return  
void CClientPlayView::UpdateLuziData(void)
{
	m_LuziDlg.Init();	
	m_LuziDlg.m_iPlayCount = MAXCOUNT;
	memset(m_LuziDlg.m_iWinType, 0, sizeof(m_LuziDlg.m_iWinType));

	memcpy(m_LuziDlg.m_iZPaiXing,m_iZPaiXing,sizeof(m_iZPaiXing)); //庄家牌型,元素0前两张牌的值，元素1总牌值，元素2天王，元素3对子，元素4和
	memcpy(m_LuziDlg.m_iXPaiXing,m_iXPaiXing,sizeof(m_iXPaiXing));//闲家牌型
	for(int i=0; i<MAXCOUNT/*m_iPlayCount*/; i++)
	{
		m_LuziDlg.m_iWinType[i] = m_iHistoryPlayData[i];
		if (m_iHistoryPlayData[i] == 1)
		{
			m_LuziDlg.m_iZwin++;
		}
		else if (m_iHistoryPlayData[i] == 2)
		{
			m_LuziDlg.m_iXwin++;	//闲家赢
		}
		else if (m_iHistoryPlayData[i] == 3)
		{
			m_LuziDlg.m_iHwin++;		//和赢
		}

		
		m_LuziDlg.m_iKaiPaiType[i]=m_iHistoryDuiZi[i];
		//if(m_iXPaiXing[3]>0)
		//{
		//	m_LuziDlg.m_iKaiPaiType[i]=6;
		//	m_iXPaiXing[3]=0;
		//	MsgPrint("zhangjing::m_iXPaiXing=%d",m_iXPaiXing[3]);
		//}

	}

	m_LuziDlg.ChangeData();
	m_LuziDlg.m_iNowJuShu = m_iNowJuShu;	//本局进行的局数，进行到第几局（72把一局）
	m_LuziDlg.Invalidate(true);
	
}

///名称：OnTimer
///描述：计时器：发牌，闪烁开牌区域等
///@param nIDEvent 计时器 id
///@return  
void CClientPlayView::OnTimer(UINT nIDEvent)
{		
	CString strLog;
	switch(nIDEvent)
	{
	case IDR_UPDATEXIAZHU:
		{
			KillTimer(IDR_UPDATEXIAZHU);
			UpdateViewFace(NULL);

			break;
		}
	case IDR_FAPAI:
		{
			m_iMoveTime--;  //动画时间
			if(	m_iMoveTime <= 0 )
			{
				m_iMoveType = -1;
				m_iMoveTime=0;
				KillTimer(IDR_FAPAI);
			}	
			//strLog.Format("帧数 %d ",m_iMoveTime);
			////OutputDebugString(strLog);
		}
		break;
	case IDR_XIANPAI:
		{

		}
		break;
	case IDR_ZHUANGPAI:
		{

		}
		UpdateViewFace(NULL);
		return;
		break;
	case IDR_CTL_ZHUANG_BTN:///控制上庄按钮点击次数
		{
		///lym2010-03-19能否点击上庄按钮			
			m_bCanShangZhuang = true;
			//m_iMessageCount = 0;
			KillTimer(IDR_CTL_ZHUANG_BTN);
		}
		break;
	default:
		break;
	}
	
	__super::OnTimer(nIDEvent);
}

///名称：DrawMoveCard
///描述：绘制牌动画
///@param pDC 绘图dc 。 iWidth 窗口宽度。 iHeight 窗口高度。
///@return  
void CClientPlayView::DrawMoveCard(CDC*pDC,int iWidth, int iHeight)
{
	if(m_iCardZhen<0)
		return ;
	if(!m_bCardMove)
		return;
	if(m_bZhuang)//庄的发牌动画
	{
		if(m_iCardZhen>0)
		{
			int x=0,y=0;
			int num = 0;
			x=m_pCardZEnd.x + m_iCardZhen*(m_pMoveStart.x - m_pCardZEnd.x)/m_iSendCardZhen;//发牌动画的开始位置
			y=m_pCardZEnd.y + m_iCardZhen*(m_pMoveStart.y - m_pCardZEnd.y)/m_iSendCardZhen;//发牌动画的开始位置
			if((m_iCardNum-1)>=0 && (m_iCardNum-1)<=2)
				num = m_iCardNum-1;
			DrawACard(pDC,iWidth,iHeight,m_iNowCard[0][num],x,y);

		}
	}
	else //闲家的发牌动画
	{
		if(m_iCardZhen>0)
		{
			int x=0,y=0;
			int num = 0;
			x=m_pCardXEnd.x+m_iCardZhen*(m_pMoveStart.x - m_pCardXEnd.x)/m_iSendCardZhen;//发牌动画的开始位置
			y=m_pCardXEnd.y+m_iCardZhen*(m_pMoveStart.y - m_pCardXEnd.y)/m_iSendCardZhen;//发牌动画的开始位置
			if((m_iCardNum-1)>=0 && (m_iCardNum-1)<=2)
				num = m_iCardNum-1;
			DrawACard(pDC,iWidth,iHeight,m_iNowCard[1][num],x,y);
		}
	}

}

///名称：StartMove
///描述：启动发牌动画
///@param zhuang是否庄家 num第几张牌 zhen 帧数
///@return 
void CClientPlayView::StartMove(bool zhuang,int num,int zhen)
{
	m_iNowCard[0][0] = CPokerGameLogic::CardToWord(1, E_CARD_SHAPE::ECS_SHAPE_DIAMONDS, E_CARD_NUMBER::ECN_NUM_A);
	m_iNowCard[0][1] = CPokerGameLogic::CardToWord(1, E_CARD_SHAPE::ECS_SHAPE_DIAMONDS, E_CARD_NUMBER::ECN_NUM_2);
	m_iNowCard[0][2] = CPokerGameLogic::CardToWord(1, E_CARD_SHAPE::ECS_SHAPE_DIAMONDS, E_CARD_NUMBER::ECN_NUM_3);
	m_iNowCard[1][0] = CPokerGameLogic::CardToWord(1, E_CARD_SHAPE::ECS_SHAPE_DIAMONDS, E_CARD_NUMBER::ECN_NUM_4);
	m_iNowCard[1][1] = CPokerGameLogic::CardToWord(1, E_CARD_SHAPE::ECS_SHAPE_DIAMONDS, E_CARD_NUMBER::ECN_NUM_5);
	m_iNowCard[1][2] = CPokerGameLogic::CardToWord(1, E_CARD_SHAPE::ECS_SHAPE_DIAMONDS, E_CARD_NUMBER::ECN_NUM_6);

	m_iCardZhen = 5;//共几帧动画
	m_iCardNum = num;

	if(zhuang)//绘制庄的发牌动画
	{	
		//AfxMessageBox("kaishi");
		m_bZhuang = true;
		CWnd::SetTimer(IDR_ZHUANGPAI,50,NULL);
		//m_bCardMove = true;
	}
	else //启动闲的发牌动画
	{
		m_bZhuang = false;
		SetTimer(IDR_XIANPAI,50,NULL);
		//m_bCardMove = true;
	}
}

///名称：DrawACard
///描述：在x,y绘制一张牌Card
///@param pDC 绘图dc 。 iWidth 窗口宽度。 iHeight 窗口高度。Card 牌数据。 x 位置 。y 位置
///@return  
void CClientPlayView::DrawACard(CDC*pDC,int iWidth, int iHeight,WORD Card,int x,int y)
{
	// 王图片xiazhu.bmp
	// 普通牌图片xiazhu.bmp
	CDC dc;
	dc.CreateCompatibleDC(pDC);
	int value = 0;
	int type = 0;
	value = CPokerGameLogic::GetCardNumber(Card)-1;//获取牌点
	type = CPokerGameLogic::GetCardShape(Card)-1;//获取牌的花色
	if(value==13)//小王
	{
		CGameImageHelper	imgwang(&m_imgWang);
		CBitmap *pOldBmp = (CBitmap*)dc.SelectObject(imgwang);
		::TransparentBlt(pDC->GetSafeHdc(),x,y-20,imgwang.GetWidth()/3,imgwang.GetHeight(),
			dc.GetSafeHdc(),imgwang.GetWidth()/3,0,imgwang.GetWidth()/3,imgwang.GetHeight(),RGB(255,0,255));
		dc.SelectObject(pOldBmp);

	}
	else if(value==14)//大王
	{
		CGameImageHelper	imgwang(&m_imgWang);
		CBitmap *pOldBmp = (CBitmap*)dc.SelectObject(imgwang);
		::TransparentBlt(pDC->GetSafeHdc(),x,y-20,imgwang.GetWidth()/3,imgwang.GetHeight(),
			dc.GetSafeHdc(),2*imgwang.GetWidth()/3,0,imgwang.GetWidth()/3,imgwang.GetHeight(),RGB(255,0,255));
		dc.SelectObject(pOldBmp);
	}
	else
	{
		CGameImageHelper	imgcard(&m_imgCard);
		CBitmap *pOldBmp = (CBitmap*)dc.SelectObject(imgcard);
		::TransparentBlt(pDC->GetSafeHdc(),x,y-20,imgcard.GetWidth()/13,imgcard.GetHeight()/4,
			dc.GetSafeHdc(),value*imgcard.GetWidth()/13,type*imgcard.GetHeight()/4,imgcard.GetWidth()/13,imgcard.GetHeight()/4,RGB(255,0,255));
		dc.SelectObject(pOldBmp);
	}
	dc.DeleteDC();
}

///名称：OnMouseMove
///描述：鼠标消息(移到)，下注时间判断鼠标在那个下注区域，设置相应的状态
///@param nFlags 消息类型 。 point 鼠标位置。
///@return  
void CClientPlayView::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_bNowGameState != GS_SEND_CARD)//非下注时间返回
		return;

	if(	m_byMyStation==m_iNowNTstation)//自己的位置,庄家不能下注
		return;

	m_iChooseRect = -1;
	if(ZRect.PtInRect(point))//庄下注区域
	{
		m_iChooseRect = 0;
	}
	else if(ZTRect.PtInRect(point))// 庄天王下注区域
	{
		m_iChooseRect = 1;
	}
	else if(ZDRcet.PtInRect(point))// 庄对子下注区域
	{
		m_iChooseRect = 2;
	}
	else if(XRect.PtInRect(point))//  闲下注区域
	{
		m_iChooseRect = 3;
	}
	else if(XTRect.PtInRect(point))// 闲天王下注区域
	{
		m_iChooseRect = 4;
	}
	else if(XDRcet.PtInRect(point))// 闲对子下注区域
	{
		m_iChooseRect = 5;
	}
	else if(HRect.PtInRect(point))//  和下注区域
	{
		m_iChooseRect = 6;
	}
	else if(THRect.PtInRect(point))//  同点和下注区域
	{
		m_iChooseRect = 7;
	}

	if(m_iChooseNote>0)
	{

		if(	NoMouseRect.PtInRect(point)||NoMouseRect1.PtInRect(point)||NoMouseRect2.PtInRect(point)||NoMouseRect3.PtInRect(point))///<鼠标禁止进入区域
		{
			m_pCursor.x = -100;
			m_pCursor.y = -100;
		}	
		else
		{
			//SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND1))); //设置光标
			GetCursorPos(&m_pCursor);        //筹码光标的位置);
			ScreenToClient(&m_pCursor);
		}
		UpdateViewFace(NULL);
	}
	else
	{
		ShowCursor(TRUE);
	}
	UpdateViewFace(NULL);
}

///名称：OnSetCursor
///描述：设置鼠标函数消息，鼠标在别的控件上时显示禁用标志
///@param 
///@return  
BOOL CClientPlayView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{

	if (m_bNowGameState == GS_SEND_CARD && m_byMyStation != m_iNowNTstation && m_iChooseNote > 0)
	{
		
		if (m_iChooseRect != -1)
		{
			SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(32649))); //设置光标
			return TRUE;
		}
		else
		{
			SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(32648))); //设置光标
			return TRUE;
		}
	}

	return CUsingSkinClass::OnSetCursor(pWnd, nHitTest, message);
}

///名称：OnMouseMove
///描述：鼠标左键抬起消息，下注时间里下注
///@param nFlags 消息类型 。 point 鼠标位置。
///@return  
void CClientPlayView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_bNowGameState != GS_SEND_CARD)//非下注时间返回
		return;
	if(	m_byMyStation == m_iNowNTstation)//自己的位置,庄家不能下注
		return;
	m_iChooseRect = -1;
	if(ZRect.PtInRect(point))//庄下注区域
	{
		m_iChooseRect = 0;
	}
	else if(ZTRect.PtInRect(point))// 庄天王下注区域
	{
		m_iChooseRect = 1;
	}
	else if(ZDRcet.PtInRect(point))// 庄对子下注区域
	{
		m_iChooseRect = 2;
	}
	else if(XRect.PtInRect(point))//  闲下注区域
	{
		m_iChooseRect = 3;
	}
	else if(XTRect.PtInRect(point))// 闲天王下注区域
	{
		m_iChooseRect = 4;
	}
	else if(XDRcet.PtInRect(point))// 闲对子下注区域
	{
		m_iChooseRect = 5;
	}
	else if(HRect.PtInRect(point))//  和下注区域
	{
		m_iChooseRect = 6;
	}
	else if(THRect.PtInRect(point))//  同点和下注区域
	{
		m_iChooseRect = 7;
	}
	//发送下注消息
	if(m_iChooseRect>=0 && m_iChooseNote>0)
	{
		CClientGameDlg *pGameDlg=CONTAINING_RECORD(this,CClientGameDlg,m_PlayView);
		if (NULL != pGameDlg)
		{
			pGameDlg->SendMessage(WM_USER_XIA_ZHU,m_iChooseNote,m_iChooseRect);
		}
	}

	if((m_iMaxZhu[m_iChooseRect]-GetMoneyFromType(m_iChooseNote))<GetMoneyFromType(m_iChooseNote))
	{//当前区域不能下这么大的注，就换一个小的筹码
		switch(m_iChooseNote)
		{
		case 2:
			{
				OnBtnClicked100();
			}
			break;
		case 3:
			{
				OnBtnClicked1000();
			}
			break;
		case 4:
			{
				OnBtnClicked1w();
			}
			break;
		case 5:
			{
				OnBtnClicked10w();
			}
			break;
		case 6:
			{
				OnBtnClicked100w();
			}
			break;
		}
	}

	UpdateViewFace(NULL);
}
///名称：GetMoneyFromType
///描述：通过筹码类型获取筹码金额
///@param moneytype 筹码类型
///@return 筹码对应的金额
__int64 CClientPlayView::GetMoneyFromType(__int64 moneytype)
{
	__int64 money = 0;
	__int64 nTempMoney=1; 
	for(int i=0; i<Glb().m_nPowerOfGold; i++)
	{
		nTempMoney*=10;
	}
	if(moneytype==1)
	{
		money = 100;
	}
	else if(moneytype==2)
	{
		money = 1000;
	}
	else if(moneytype==3)
	{
		money = 10000;
	}
	else if(moneytype==4)
	{
		money = 100000;
	}
	else if(moneytype==5)
	{
		money = 1000000;
	}
	else if(moneytype==6)
	{
///72把一局
#ifdef SET72 
		money = 10000000;
#else
		money = 5000000;
#endif
	}
	money = money/nTempMoney;
	return money;
}
///名称：OnRButtonDown
///描述：右键按下取消筹码选取功能
///@param nFlags 消息类型 。 point 鼠标位置。
///@return  
void CClientPlayView::OnRButtonDown(UINT nFlags, CPoint point)
{
	OnRestoreCursor();

	if(m_bNowGameState != GS_SEND_CARD)//非下注时间不响应
		return;
	if(	m_byMyStation==m_iNowNTstation)//自己的位置,庄家不能下注
		return;

	m_iChooseNote = 0;
	EnableXiaZhu(true);
	UpdateViewFace(NULL);

}

///名称：OnHandleKaiPai
///描述：处理系统开牌消息
///@param pNetData 消息包 。 uDataSize 包长度。
///@return  
void CClientPlayView::OnHandleKaiPai(void * pNetData, UINT uDataSize)
{
	KAI_PAI *paidata = (KAI_PAI *)pNetData;
	//接收牌数据
	m_iNowCard[0][0] = paidata->pai[0][0];
	m_iNowCard[0][1] = paidata->pai[0][1];
	m_iNowCard[0][2] = paidata->pai[0][2];
	m_iNowCard[1][0] = paidata->pai[1][0];
	m_iNowCard[1][1] = paidata->pai[1][1];
	m_iNowCard[1][2] = paidata->pai[1][2];
	//m_CardCount
	m_CardCount[0]=paidata->m_CardCount[0];
	m_CardCount[1]=paidata->m_CardCount[1];

	m_iWinner = paidata->m_iWinner;				//赢家1 庄，2闲，3和，本赢方
	//m_iKaiPai = paidata->m_iKaiPai;	

	for(int i=0;i<5;i++)//庄闲牌型
	{
		m_iZPaiXing[i] =  paidata->m_iZPaiXing[i];
		m_iXPaiXing[i] =  paidata->m_iXPaiXing[i];
	}
	//获取赢牌区域
	for(int i=0;i<8;i++)
	{
		m_iWinQuYu[i] = paidata->m_iWinQuYu[i];			//游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和
	}

	m_bShowResult = false; //是否显示结算信息
	m_bShowRect = false ;//控制赢区域的闪烁

	for(int i=0;i<5;i++)
	{
		m_iZCardInfo[i] = paidata->m_iZPaiXing[i];	//庄家牌型,元素0前两张牌的值，元素1总牌值，元素2天王，元素3对子，元素4和
		m_iXCardInfo[i] = paidata->m_iXPaiXing[i];	//闲家牌型,元素0前两张牌的值，元素1总牌值，元素2天王，元素3对子，元素4和
	}

	for (int i=0; i<MAXCOUNT; i++)
	{
		m_iHistoryPlayData[i] = paidata->m_iHistoryPlayData[i];
	}	

	// 恢复鼠标正常状态
	OnRestoreCursor();
}


///名称：OnHandleXiazhu
///描述：处理下注消息
///@param pNetData 消息包 。 uDataSize 包长度。
///@return  
void CClientPlayView::OnHandleXiazhu(void * pNetData, UINT uDataSize)
{
	XIA_ZHU *pXiazhu = (XIA_ZHU *)pNetData;
	int count=0;//金币个数

	for(int i=0;i<MAXJINBI-1;i++)
	{
		if(m_iJinBi[pXiazhu->type][i]<=0)
			break;
		count++;
	}
	if(count>=MAXJINBI-1)
	{
		for(int i=0;i<MAXJINBI-1;i++)
		{
			m_iJinBi[pXiazhu->type][i] = m_iJinBi[pXiazhu->type][i+1];
			m_pJinBi[pXiazhu->type][i] = m_pJinBi[pXiazhu->type][i+1];
		}
	}
	for(int i=0;i<8;i++)
	{
		m_iMaxZhu[i] = pXiazhu->m_iMaxZhu[i];     //本把每个区域最大能下的注额
	}

	m_iJinBi[pXiazhu->type][count] = pXiazhu->moneytype; //玩家在八个区域下的金币情况，记录前2000个  
	m_pJinBi[pXiazhu->type][count].SetPoint(pXiazhu->point.x,pXiazhu->point.y);  //玩家在八个区域下的金的位置情况，记录前2000个

	for(int i=0;i<8;i++)
	{
		m_iQuYuZhu[i] = pXiazhu->m_iQuYuZhu[i];//区域注
		m_iAIMoney[i] = pXiazhu->m_iAIMoney[i];	///机器人的下注
	}

	m_iMyZhu[8] = pXiazhu->m_iMeZhongZhu;
	if(pXiazhu->station == m_byMyStation)//下注的人是自己
	{
		m_iMyZhu[pXiazhu->type] += pXiazhu->money;	
		SetXiaZhuBtn();
	}

	m_SuperDlg.UpDataNoteRateInfo(m_iQuYuZhu[0]*2,m_iQuYuZhu[3]*2,m_iQuYuZhu[6]*9) ; 
	
	switch (pXiazhu->type)
	{
	case 0:	UpdateViewFace(ZRect);	break;	// 庄
	case 1:	UpdateViewFace(ZTRect);	break;	// 庄天王
	case 2:	UpdateViewFace(ZDRcet);	break;	// 庄对子

	case 3:	UpdateViewFace(XRect);	break;	// 闲
	case 4:	UpdateViewFace(XTRect);	break;	// 闲天王
	case 5:	UpdateViewFace(XDRcet);	break;	// 庄对子

	case 6:	UpdateViewFace(HRect);	break;	// 和
	case 7:	UpdateViewFace(THRect);	break;	// 同点和
	}
}

///名称：DrawJinBi
///描述：绘制金币图片
///@param pDC 绘制dc 。 iWidth 窗口宽度。iHeight 窗口高度。
///@return  
void CClientPlayView::DrawJinBi(CDC*pDC, int iWidth, int iHeight)
{
	CGameImageHelper	imgchouma(&m_imgchouma);
	CDC dc;
	dc.CreateCompatibleDC(pDC);
	CBitmap *pOldBmp = (CBitmap*)dc.SelectObject(imgchouma);

	//根据区域绘制金币
	for(int i=0;i<8;i++)
	{
		for(int j=0;j<MAXJINBI;j++)
		{
			if(m_iJinBi[i][j]<=0)
				break;
			//绘制金币
			CPoint dp(0,0);
			dp.x = m_iYUcenter[i].x + m_pJinBi[i][j].x;     //每个区域的中心点位置  
			dp.y = m_iYUcenter[i].y + m_pJinBi[i][j].y;     //每个区域的中心点位置  

			::TransparentBlt(pDC->GetSafeHdc(), dp.x, dp.y, imgchouma.GetWidth()/6,imgchouma.GetHeight(),
			dc.GetSafeHdc(), (m_iJinBi[i][j]-1)*imgchouma.GetWidth()/6,0, imgchouma.GetWidth()/6,imgchouma.GetHeight(),RGB(255,0,255));
		}
	}
	dc.SelectObject(pOldBmp);
	dc.DeleteDC();
}

///名称：DrawJinBi
///描述：绘制高亮矩形：下注时鼠标移到的区域，结算时闪烁的赢牌区域
///@param pDC 绘制dc 。 iWidth 窗口宽度。iHeight 窗口高度。
///@return  
void CClientPlayView::DrawRect(CDC*pDC, int iWidth, int iHeight)
{
	CRect rect(0,0,0,0);
	if(m_bNowGameState==GS_SEND_CARD)
	{
		switch(m_iChooseRect)
		{	
		case 0:
			{
				rect = ZRect;
			}
			break;
		case 1:
			{
				rect= ZTRect;
			}
			break;
		case 2:
			{
				rect= ZDRcet;
			}
			break;
		case 3:
			{
				rect= XRect;
			}
			break;
		case 4:
			{
				rect= XTRect;
			}
			break;
		case 5:
			{
				rect= XDRcet;
			}
			break;
		case 6:
			{
				rect= HRect;
			}
			break;
		case 7:
			{
				rect= THRect;
			}
			break;
		default:
			break;
		}
		///lym2010-03-19不在选择区域内部绘制
		if(m_iChooseRect>=0 && m_iChooseRect<8)
		{
			pDC->Draw3dRect(&rect,RGB(255,255,0),RGB(255,255,0));
			rect.top+=1;
			rect.bottom-=1;
			rect.left+=1;
			rect.right-=1;
			pDC->Draw3dRect(&rect,RGB(255,255,0),RGB(255,255,0));
			rect.top+=1;
			rect.bottom-=1;
			rect.left+=1;
			rect.right-=1;
			pDC->Draw3dRect(&rect,RGB(255,255,0),RGB(255,255,0));
		}
	}

	if(m_bNowGameState==GS_PLAY_GAME && !m_bSendCard && m_bShowRect)
	{
		for(int i=0;i<8;i++)
		{
			if(m_iWinQuYu[i]<=1)
				continue;
			switch(i)
			{	
			case 0:
				{
					rect = ZRect;
				}
				break;
			case 1:
				{
					rect= ZTRect;
				}
				break;
			case 2:
				{
					rect= ZDRcet;
				}
				break;
			case 3:
				{
					rect= XRect;
				}
				break;
			case 4:
				{
					rect= XTRect;
				}
				break;
			case 5:
				{
					rect= XDRcet;
				}
				break;
			case 6:
				{
					rect= HRect;
				}
				break;
			case 7:
				{
					rect= THRect;
				}
				break;
			default:
				break;
			}
			pDC->Draw3dRect(&rect,RGB(255,255,0),RGB(255,255,0));
			rect.top+=1;
			rect.bottom-=1;
			rect.left+=1;
			rect.right-=1;
			pDC->Draw3dRect(&rect,RGB(255,255,0),RGB(255,255,0));
			rect.top+=1;
			rect.bottom-=1;
			rect.left+=1;
			rect.right-=1;
			pDC->Draw3dRect(&rect,RGB(255,255,0),RGB(255,255,0));
		}
	}
}
///名称：DrawNtInfo
///绘制玩家自己的信息：金币，命中率，总计成绩DrawMeInfo
///描述：
///绘制历史结果信息：最近10局的庄闲点数，最近30局庄闲和开牌DrawResultInfo
///绘制下注信息：八个区域的下注，庄闲和可下注的限额DrawXiaZhuInfo
///绘制动态信息：无庄（提示），下注（提示下注），开牌（逐个提示开牌信息），空闲时间DrawActiveInfo
///绘制庄信息，庄列表DrawNtInfo
///@param pDC 绘制dc 。 iWidth 窗口宽度。iHeight 窗口高度。
///@return  
void CClientPlayView::DrawNtInfo(CDC *pDC, int iWidth, int iHeight)
{

	pDC->SetTextColor(RGB(85,155,236));
	CString str="--";
	CString stra="";
	CString strRank ; 
	int temp=0;
	CRect  rectName(0,0,0,0);

	//庄家信息：呢称，进行局数，金币数，成绩（总输赢多少金币）m_iNowNTstation
	rectName.top= m_pNtInfo.y;//呢称
	rectName.bottom = rectName.top+15;
	rectName.left = m_pNtInfo.x ;
	rectName.right = rectName.left + 100;
	if(m_pUserItem[m_iNowNTstation]!=NULL)
	{
		str.Format("%s",m_pUserItem[m_iNowNTstation]->GameUserInfo.nickName);
	}
	pDC->DrawText(str,str.GetLength(),&rectName,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

	str="0";
	if(m_pUserItem[m_iNowNTstation]!=NULL)
	{
		str.Format("%d",m_iNTju);//局数
	}
	rectName.top= m_pNtInfo.y;
	rectName.bottom = rectName.top+15;
	rectName.left = m_pNtInfo.x + 145;
	rectName.right = rectName.left + 80;
	pDC->DrawText(str,str.GetLength(),&rectName,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

	__int64 ntmoney=0;
	if(m_pUserItem[m_iNowNTstation]!=NULL)
	{
		ntmoney = m_pUserItem[m_iNowNTstation]->GameUserInfo.i64Money;
	}
	str="";
	//NumToChar(ntmoney,&str);
	ChangeNumToCStringByPower(ntmoney ,true ,"," ,str) ;

	rectName.top= m_pNtInfo.y +29;
	rectName.bottom = rectName.top+15;
	rectName.left = m_pNtInfo.x ;
	rectName.right = rectName.left + 100;
	pDC->DrawText(str,str.GetLength(),&rectName,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

	//lym1115放大100倍,显示到100亿
	str="0";
	if(m_pUserItem[m_iNowNTstation]!=NULL)
	{
		//NumToChar(m_iNTwin,&str);;//成绩
		ChangeNumToCStringByPower(m_iNTwin ,true ,"," ,str) ;
	}
	rectName.top= m_pNtInfo.y +29;
	rectName.bottom = rectName.top+15;
	rectName.left = m_pNtInfo.x + 145;
	rectName.right = rectName.left + 80;
	pDC->DrawText(str,str.GetLength(),&rectName,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

	//庄家列表，呢称，金币
	//m_iZhuangList//m_pNtList
	if (m_iZhuangList[PLAY_COUNT] <= 5)
	{
		m_bNowNtListIndex = 0;
	}
	for(int i=m_bNowNtListIndex,j=0;i<PLAY_COUNT&&j<5;i++)//只显示5个
	{
		if(m_iZhuangList[i]>=0)
		{
			if (NULL != m_pUserItem[m_iZhuangList[i]])
			{
				__int64 money=0;
				TCHAR szText[_MAX_PATH]={0};
				rectName.top= m_pNtList.y + (j++)*15-7;
				rectName.bottom = rectName.top+15;
				rectName.left = m_pNtList.x +10;
				rectName.right = rectName.left + 100;

				strRank.Format("%d" , i + 1) ; 
				pDC->DrawText(strRank,strRank.GetLength(),&rectName,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

				rectName.OffsetRect(40 ,0) ; 

				str.Format("%s",m_pUserItem[m_iZhuangList[i]]->GameUserInfo.nickName);
				pDC->DrawText(str,str.GetLength(),&rectName,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

				money=m_pUserItem[m_iZhuangList[i]]->GameUserInfo.i64Money;
				//lym1115放大100倍,显示到100亿
				str="";
				//NumToChar(money,&str);
				ChangeNumToCStringByPower(money ,true ,"," ,str) ;

				rectName.OffsetRect(85 , 0) ;
				
				pDC->DrawText(str,str.GetLength(),&rectName,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
			}
		}
		else 
		{
			break;
		}
	}
	pDC->SetTextColor(RGB(255,255,102));
}
///上庄队列下翻按钮事件
void CClientPlayView::OnBtnClickedNTTop(void)
{
	m_bNowNtListIndex--;
	if (m_bNowNtListIndex < 0)
	{
		m_bNowNtListIndex = 0;
	}
	CRect rect;
	rect.SetRect(m_pNtList.x,m_pNtList.y,m_pNtList.x+253,m_pNtList.y+90);
	UpdateViewFace(NULL);
}
///上庄把队列下翻按钮事件
void CClientPlayView::OnBtnClickedNTBottom(void)
{


	if (m_iZhuangList[PLAY_COUNT] <= 5)
	{
		m_bNowNtListIndex = 0;
	}
	else if (m_iZhuangList[PLAY_COUNT] - m_bNowNtListIndex > 5)
	{
		m_bNowNtListIndex++;
	}
	CRect rect;
	rect.SetRect(m_pNtList.x,m_pNtList.y,m_pNtList.x+253,m_pNtList.y+90);
	UpdateViewFace(NULL);
	
}
///名称：DrawXiaZhuInfo
///描述：绘制下注信息：八个区域的下注，庄闲和可下注的限额DrawXiaZhuInfo
///@param pDC 绘制dc 。 iWidth 窗口宽度。iHeight 窗口高度。
///@return  
void CClientPlayView::DrawXiaZhuInfo(CDC*pDC, int iWidth, int iHeight)
{
	//非下注状态
	//return;
	CRect  rectName(0,0,0,0);
	CString stra="";
	int temp=0;

	__int64 nTempMoney=1; 
	for(int i=0; i<m_nPowerOfGold; i++)
	{
		nTempMoney*=10;
	}

	pDC->SetTextColor(RGB(255,255,255));

	//庄家信息：庄闲和可下注的限额//输出文本	m_pXZXianZhi.SetPoint(85,17);		 //下分限制显示的初始位置
	CString str="";
	temp=0;
	
	str="";
	//NumToChar(m_iMaxZhu[0],&str);//庄下注限额
	ChangeNumToCStringByPower(m_iMaxZhu[0] ,true ,"," ,str) ;

	rectName.top= m_pXZXianZhi.y ;
	rectName.bottom = rectName.top+15;
	rectName.left = m_pXZXianZhi.x ;
	rectName.right = rectName.left + 150;
	pDC->DrawText(str,str.GetLength(),&rectName,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);


	//lym1115放大100倍,显示到100亿
	str="";
	//NumToChar(m_iMaxZhu[6],&str);//闲下注限额	
	ChangeNumToCStringByPower(m_iMaxZhu[6] ,true ,"," ,str) ;

	rectName.top= m_pXZXianZhi.y +21;
	rectName.bottom = rectName.top+15;
	rectName.left = m_pXZXianZhi.x ;
	rectName.right = rectName.left + 150;
	pDC->DrawText(str,str.GetLength(),&rectName,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

	//lym1115放大100倍,显示到100亿
	str="";
	//NumToChar(m_iMaxZhu[3],&str);//和下注限额
	ChangeNumToCStringByPower(m_iMaxZhu[3] ,true ,"," ,str) ;

	rectName.top= m_pXZXianZhi.y + 42;
	rectName.bottom = rectName.top+15;
	rectName.left = m_pXZXianZhi.x ;
	rectName.right = rectName.left + 150;
	pDC->DrawText(str,str.GetLength(),&rectName,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);


	if(m_iNowNTstation>=0 && m_iNowNTstation<PLAY_COUNT && m_byMyStation != m_iNowNTstation)
	{
		__int64 llMyTotleNote = 0 ; 

		for(int i = 0 ; i< 8 ; i++)
		{
			llMyTotleNote += m_iMyZhu[i] ; 
		}

		CString strInfo ; 
		strInfo.Format("当前自己下注总额：") ; 
		str = "" ;
		ChangeNumToCStringByPower(llMyTotleNote ,true ,"," ,str) ;
		str = strInfo + str ;  

		rectName.top= m_pXZXianZhi.y + 505;
		rectName.bottom = rectName.top + 15;
		rectName.left = m_pXZXianZhi.x -80;
		rectName.right = rectName.left + 250;
		pDC->DrawText(str,str.GetLength(),&rectName,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
	}

	pDC->SetTextColor(RGB(255,255,102));

	//八个区域的下注//贴图
	for(int i=0;i<8;i++)
	{
		if(m_iQuYuZhu[i]>0)
		{
			//__int64 iTempmoney=nTempMoney*m_iQuYuZhu[i];

			__int64 iTempmoney = ChangeMoneyByPower(m_iQuYuZhu[i] ,true);

			if(m_bHideAIMoney && m_SuperDlg.IsWindowVisible())///隐藏机器人下注
			{
				//iTempmoney=nTempMoney*(m_iQuYuZhu[i] - m_iAIMoney[i]);
				iTempmoney=ChangeMoneyByPower(m_iQuYuZhu[i] - m_iAIMoney[i] , true);
			}
			DrawXiaZhuNum(pDC,iTempmoney,iWidth,iHeight,m_pQYText[i].x,m_pQYText[i].y-5,false);
		}
	}
	//玩家自己的下注
	for(int i=0;i<8;i++)
	{
		if(m_iMyZhu[i]>0)
		{
			//int iTempmoney=nTempMoney*m_iMyZhu[i];
			int iTempmoney=ChangeMoneyByPower(m_iMyZhu[i] ,true);

			DrawXiaZhuNum(pDC,iTempmoney,iWidth,iHeight,m_pQYText[i].x,m_pQYText[i].y+25,true);
		}
	}
}

///名称：DrawXiaZhuNum
///描述：绘制各个区域的下注数字：总的下注或自己的下注
///@param pDC 绘制dc 。num 下注额。 iWidth 窗口宽度。iHeight 窗口高度。x位置 。int 位置。 my 是否自己的下注
///@return  
void CClientPlayView::DrawXiaZhuNum(CDC*pDC,int num, int iWidth, int iHeight,int x,int y,bool my)
{	
	if(my)//绘制自己的下注
	{	
		CGameImageHelper	imgnum(&m_imgMyNum);
		CDC dc;
		dc.CreateCompatibleDC(pDC);
		CBitmap *pOldBmp = (CBitmap*)dc.SelectObject(imgnum);
		int temp = num; 
		int value=0;//每一位的值
		int index=0;
		int width=15;//间隔16
		int len=0;//相对长度
		//x+=30;
		CString str="";
		str.Format("%d",num);
		int strlen = str.GetLength();
		x+=strlen*8;

		for(int i=0;i<12;i++)
		{
			value = temp%10;
			temp/=10;
			if(index>=8)
			{
				len=16;
			}
			else if(index>=4)
			{
				len=8;
			}
			::TransparentBlt(pDC->GetSafeHdc(), x-len-index*width,y, imgnum.GetWidth()/10,imgnum.GetHeight(),
				dc.GetSafeHdc(), value*imgnum.GetWidth()/10,0, imgnum.GetWidth()/10,imgnum.GetHeight(),dc.GetPixel(0,0));
			if(temp<=0)
				break;
			index++;
		}
		dc.SelectObject(pOldBmp);
		dc.DeleteDC();
	}
	else
	{
		CGameImageHelper	imgnum(&m_imgChoumaNum);
		CDC dc;
		dc.CreateCompatibleDC(pDC);
		CBitmap *pOldBmp = (CBitmap*)dc.SelectObject(imgnum);
		int temp = num; 

		int value=0;//每一位的值
		int index=0;
		int width=16;//间隔16
		int len=0;//相对长度
		//x+=30;
		CString str="";
		str.Format("%d",num);
		int strlen = str.GetLength();
		x+=strlen*8;

		for(int i=0;i<12;i++)
		{
			value = temp%10;
			temp/=10;
			if(index>=8)
			{
				len=20;
			}
			else if(index>=4)
			{
				len=10;
			}
			::TransparentBlt(pDC->GetSafeHdc(), x-len-index*width,y, imgnum.GetWidth()/10,imgnum.GetHeight(),
				dc.GetSafeHdc(), value*imgnum.GetWidth()/10,0, imgnum.GetWidth()/10,imgnum.GetHeight(),dc.GetPixel(0,0));
			if(temp<=0)
				break;
			index++;
		}
		dc.SelectObject(pOldBmp);
		dc.DeleteDC();
	}

}

///名称：DrawPointValue
///描述：绘制开牌点数值
///@param pDC 绘制dc 。x位置 。int 位置。 my 是否自己的下注
///@return  
void CClientPlayView::DrawPointValue(CDC * pDC, CDC &dc, int ix, int iy)
{
	// 闲点数
	BYTE value = 0, temp = 0;
	for (BYTE i=0; i<m_ContinueShowCardCount[1]; i++)
	{
		temp = CPokerGameLogic::GetCardNumber(m_iNowCard[1][i]);
		value += (temp>=10 ? 0 : temp);
	}

	// 庄点数
	BYTE valueBanker = 0;
	for (BYTE i=0; i<m_ContinueShowCardCount[0]; i++)
	{
		temp = CPokerGameLogic::GetCardNumber(m_iNowCard[0][i]);
		valueBanker += (temp>=10 ? 0 : temp);
	}


	switch(m_iShowCard)
	{
	case 1:
		{
			temp =  CPokerGameLogic::GetCardNumber(m_iNowCard[1][0]);
			value=(temp>=10 ? 0 : temp);
		}
		break;
	case 2:
		{
			temp =  CPokerGameLogic::GetCardNumber(m_iNowCard[1][0]);
			value=(temp>=10 ? 0 : temp);

			temp = CPokerGameLogic::GetCardNumber(m_iNowCard[0][0]);
			valueBanker=(temp>=10 ? 0 : temp);
		}
		break;
	case 3:
		{
			value = m_iXPaiXing[0];
			temp = CPokerGameLogic::GetCardNumber(m_iNowCard[0][0]);
			valueBanker = (temp>=10 ? 0 : temp);
		}
		break;
	case 4:
		{
			value = m_iXPaiXing[0];
			valueBanker = m_iZPaiXing[0];
		}
		break;
	case 5://闲家3张牌
		{
			value = m_iXPaiXing[1];
			valueBanker = m_iZPaiXing[0];
		}
		break;
	case 6://闲家没有三张牌，庄家有
		{
			value = m_iXPaiXing[1];
			valueBanker = m_iZPaiXing[1];
		}
		break;
	case 7:
		{
			value = m_iXPaiXing[1];
			valueBanker = m_iZPaiXing[1];
		}
		break;
	}

	value>=10 ? value%=10 : value;
	valueBanker>=10 ? valueBanker%=10 : valueBanker;

	// 闲、庄两家
	for (BYTE i=0;i<2;i++)
	{
		if (1>m_iShowCard || (1==i && 2>m_iShowCard)) break; 
		if (1==i)
		{
			value = valueBanker;
			ix += 139;
		}

		if (0==i && 0>=m_ContinueShowCardCount[1]) continue;
		if (1==i && 0>=m_ContinueShowCardCount[0]) continue;

		if ((0==i && 6==m_iKaiPai && 8<=value && m_iShowCard>=3)  ||  (1==i && 3==m_iKaiPai && 8<=value && m_iShowCard>=4) )
		{
			// 绘制天王对子
			CGameImageHelper ImgTwoKing(&m_imgTwoKing);
			CBitmap *pOldBmp = (CBitmap*)dc.SelectObject(ImgTwoKing);
			::TransparentBlt(pDC->GetSafeHdc(), ix+32, iy+9, ImgTwoKing.GetWidth(), \
				ImgTwoKing.GetHeight(), dc.GetSafeHdc(), 0, 0, ImgTwoKing.GetWidth(), \
				ImgTwoKing.GetHeight(), RGB(255,0,255));
			dc.SelectObject(pOldBmp);
		}
		else if ((0==i && 6==m_iKaiPai&& m_iShowCard>=3)  ||  (1==i && 3==m_iKaiPai&& m_iShowCard>=4))
		{
			// 闲、庄对子
			CGameImageHelper ImgTwoKing(&m_imgTwoKing);
			CBitmap *pOldBmp = (CBitmap*)dc.SelectObject(ImgTwoKing);
			::TransparentBlt(pDC->GetSafeHdc(), ix+32+20, iy+9, ImgTwoKing.GetWidth()/2, \
				ImgTwoKing.GetHeight(), dc.GetSafeHdc(), 40, 0, ImgTwoKing.GetWidth()/2, \
				ImgTwoKing.GetHeight(), RGB(255,0,255));
			dc.SelectObject(pOldBmp);
		}
		else if (((0==i && 5==m_iKaiPai&& m_iShowCard>=3) || (1==i && 2==m_iKaiPai&& m_iShowCard>=4))   &&   8<=value)
		{
			// 闲、庄天王
			CGameImageHelper ImgTwoKing(&m_imgTwoKing);
			CBitmap *pOldBmp = (CBitmap*)dc.SelectObject(ImgTwoKing);
			::TransparentBlt(pDC->GetSafeHdc(), ix+32+20, iy+9, ImgTwoKing.GetWidth()/2, \
				ImgTwoKing.GetHeight(), dc.GetSafeHdc(), 0, 0, ImgTwoKing.GetWidth()/2, \
				ImgTwoKing.GetHeight(), RGB(255,0,255));
			dc.SelectObject(pOldBmp);
		}
		else
		{
			// 绘制点值
			CGameImageHelper ImgPointValue(&m_imgPointValue);
			CBitmap *pOldBmp = (CBitmap*)dc.SelectObject(ImgPointValue);
			::TransparentBlt(pDC->GetSafeHdc(), ix+52, iy+9, ImgPointValue.GetWidth()/10, \
				ImgPointValue.GetHeight(), dc.GetSafeHdc(), value*ImgPointValue.GetWidth()/10, \
				0, ImgPointValue.GetWidth()/10, ImgPointValue.GetHeight(), RGB(255,0,255));
			dc.SelectObject(pOldBmp);
		}
	}
	dc.DeleteDC();
}
///名称：DrawResultInfo
///描述：绘制历史结果信息：最近10局的庄闲点数，最近30局庄闲和开牌
///@param pDC 绘制dc 。iWidth 窗口宽度。iHeight 窗口高度。
///@return 
void CClientPlayView::DrawResultInfo(CDC*pDC, int iWidth, int iHeight)
{	
	
	if(m_bNowGameState==GS_PLAY_GAME && m_iShowCard!=0)
	{
		///绘制本局的结算底图
		CGameImageHelper	ditu(&m_imgditu);	// 光标图片cursor.bmp m_imgCursor
		CDC dc;
		dc.CreateCompatibleDC(pDC);
		CBitmap *OldBmp1 = dc.SelectObject(ditu);
		::TransparentBlt(pDC->GetSafeHdc(), m_pDitu.x+14, m_pDitu.y-20, ditu.GetWidth(), ditu.GetHeight(),	\
			dc.GetSafeHdc(), 0, 0, ditu.GetWidth(),ditu.GetHeight(), RGB(255,0,255));

		// 绘制本局牌点数及天王
		DrawPointValue(pDC, dc, m_pDitu.x+14,m_pDitu.y-20);
		dc.SelectObject(OldBmp1);
		dc.DeleteDC();
	}
	CRect  rectName(0,0,0,0);
	CString str="";
	int index=0;
	int CountNum = 0x00;
	int iEndNumber = 0x00;
	for(int i = 0x00;i < MAXCOUNT;i++)
	{
		if(m_iGameInfo[i][2] > 0 && m_iGameInfo[i][2] !=255)//有数据
		{
			CountNum ++;
		}
	}
	iEndNumber = CountNum;
	if(CountNum > 10)
	{
		CountNum = CountNum - 10;
	}
	else
	{
		CountNum = 0x00;
	}
	//最近10局的庄闲点数
	for(int i=0x00; i < 10; i++)
	{
		if(m_iGameInfo[CountNum][2] > 0 && m_iGameInfo[CountNum][2] !=255)//有数据
		{
			if(CountNum==iEndNumber - 0x01)
			{
				pDC->SetTextColor(RGB(128,169,209));
			}

			str.Format("%d",m_iGameInfo[CountNum][0]);	//庄点数
			rectName.top = m_p10ResultInfo.y;
			rectName.bottom = rectName.top+18;
			rectName.left = m_p10ResultInfo.x + i*22;
			rectName.right = rectName.left + 18;
			pDC->DrawText(str,str.GetLength(),&rectName,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

			str.Format("%d",m_iGameInfo[CountNum][1]);	//闲点数
			rectName.top = m_p10ResultInfo.y + 22;
			rectName.bottom = rectName.top+18;
			pDC->DrawText(str,str.GetLength(),&rectName,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
			CountNum++;
			pDC->SetTextColor(RGB(255,255,102));
		}
	}
	pDC->SetTextColor(RGB(255,255,102));
	//最近30局庄闲和开牌(贴图星号)
	CGameImageHelper	imgXing(&m_imgXing);

#ifdef SET72 
	//沟图片
	CGameImageHelper	imgZhuang(&m_imgShowZ);
	CGameImageHelper	imgXian(&m_imgShowX);
	CGameImageHelper	imgHe(&m_imgShowH);

	//叉图片
	CGameImageHelper	imgZhuang_x(&m_imgShowZ_x);
	CGameImageHelper	imgXian_x(&m_imgShowX_x);
	CGameImageHelper	imgHe_x(&m_imgShowH_x);


	///出对子打点
	CGameImageHelper	imgZhuang_dot(&m_imgShowZ_dot);
	CGameImageHelper	imgXian_dot(&m_imgShowX_dot);
	CGameImageHelper	imgHe_dot(&m_imgShowH);

	//叉图片
	CGameImageHelper	imgZhuang_x_dot(&m_imgShowZ_x_dot);
	CGameImageHelper	imgXian_x_dot(&m_imgShowX_x_dot);
	CGameImageHelper	imgHe_x_dot(&m_imgShowH_x_dot);


#endif

	index=0;
	int x = (iWidth - imgXing.GetWidth())/2;
	int y = (iHeight - imgXing.GetHeight())/2;
	CDC dc;
	dc.CreateCompatibleDC(pDC);
	CBitmap *OldBmp = dc.SelectObject(imgXing);
	int allcount=0;
	for(int i=0;i<MAXCOUNT;i++)
	{
		if (m_iResultInfoArea[i] > 0)
			allcount++;									//路子信息有多少个
	}
	if(allcount > 0x0a)
	{
		allcount = allcount - 10 - m_iShowInfoIndex;
	}
	else
	{
		allcount = 0x00;
	}
	int show=0;	
	int i = allcount;
	for(int j = 0x00;j < 0x0a;j ++,i++)
	{
		if(m_iResultInfoArea[i] == 1)//第三个元素是赢方，1庄，2闲，3和
		{
			x=m_pXing.x + (index)*42;			
			y=m_pXing.y-3;
#ifdef SET72 
			if (m_iMeXiaZhuInfo[i] == 1)
			{
				if(m_iHistoryDuiZi[i]==3||m_iHistoryDuiZi[i]==6)
				{
					imgZhuang_dot.OpenGDIHandle(&m_imgShowZ_Win_dot);
				}
				else
				{
					imgZhuang.OpenGDIHandle(&m_imgShowZ_Win);
				}				
			}
			else if (m_iMeXiaZhuInfo[i] == 2)
			{
				if(m_iHistoryDuiZi[i]==3||m_iHistoryDuiZi[i]==6)
				{
					imgZhuang_dot.OpenGDIHandle(&m_imgShowZ_Lost_x_dot);
				}
				else
				{
				imgZhuang.OpenGDIHandle(&m_imgShowZ_Lost_x);
				}
			}
			else
			{
				if(m_iHistoryDuiZi[i]==3||m_iHistoryDuiZi[i]==6)
				{
					imgZhuang_dot.OpenGDIHandle(&m_imgShowZ_dot);
				}
				else
				{
				imgZhuang.OpenGDIHandle(&m_imgShowZ);
				}
			}
			if(m_iHistoryDuiZi[i]==3||m_iHistoryDuiZi[i]==6)
			{
				x+=(42-imgXian_dot.GetWidth())/2;
				dc.SelectObject(imgZhuang_dot);
				::TransparentBlt(pDC->GetSafeHdc(),x,y,imgZhuang_dot.GetWidth(),imgZhuang_dot.GetHeight(),
					dc.GetSafeHdc(),0,0,imgZhuang_dot.GetWidth(),imgZhuang_dot.GetHeight(),RGB(255,0,255));
			}
			else
			{
				x+=(42-imgXian.GetWidth())/2;
				dc.SelectObject(imgZhuang);
				::TransparentBlt(pDC->GetSafeHdc(),x,y,imgZhuang.GetWidth(),imgZhuang.GetHeight(),
					dc.GetSafeHdc(),0,0,imgZhuang.GetWidth(),imgZhuang.GetHeight(),RGB(255,0,255));
			}



#else
			//如果自己赢
			if(m_iMeXiaZhuInfo[i]==2)
			{
				::TransparentBlt(pDC->GetSafeHdc(),x,y,imgXing.GetWidth()/2,imgXing.GetHeight(),
					dc.GetSafeHdc(),imgXing.GetWidth()/2,0,imgXing.GetWidth()/2,imgXing.GetHeight(),dc.GetPixel(0,0));
			}
			else
			{
				//否则
				::TransparentBlt(pDC->GetSafeHdc(),x,y,imgXing.GetWidth()/2,imgXing.GetHeight(),
					dc.GetSafeHdc(),0,0,imgXing.GetWidth()/2,imgXing.GetHeight(),dc.GetPixel(0,0));
			}
#endif
		}
		else if(m_iResultInfoArea[i] == 2)//闲
		{
			x=m_pXing.x + (index)*42;			
			y=m_pXing.y+30-2;

#ifdef SET72 
			
			if (m_iMeXiaZhuInfo[i] == 1)
			{
				if(m_iHistoryDuiZi[i]==3||m_iHistoryDuiZi[i]==6)
				{
					imgXian_dot.OpenGDIHandle(&m_imgShowX_Win_dot);
				}
				else
				{
				imgXian.OpenGDIHandle(&m_imgShowX_Win);
				}
			}
			else if (m_iMeXiaZhuInfo[i] == 2)
			{
				if(m_iHistoryDuiZi[i]==3||m_iHistoryDuiZi[i]==6)
				{
					imgXian_dot.OpenGDIHandle(&m_imgShowX_Lost_x_dot);
				}
				else
				{
				imgXian.OpenGDIHandle(&m_imgShowX_Lost_x);
				}
			}
			else
			{
				if(m_iHistoryDuiZi[i]==3||m_iHistoryDuiZi[i]==6)
				{
					imgXian_dot.OpenGDIHandle(&m_imgShowX_dot);
				}
				else
				{
				imgXian.OpenGDIHandle(&m_imgShowX);
				}
			}
			if(m_iHistoryDuiZi[i]==3||m_iHistoryDuiZi[i]==6)
			{
				x+=(42-imgXian_dot.GetWidth())/2;
				dc.SelectObject(imgXian_dot);
				::TransparentBlt(pDC->GetSafeHdc(),x,y,imgXian_dot.GetWidth(),imgXian_dot.GetHeight(),
					dc.GetSafeHdc(),0,0,imgXian_dot.GetWidth(),imgXian_dot.GetHeight(),RGB(255,0,255));
			}
			else
			{
			x+=(42-imgXian.GetWidth())/2;
			dc.SelectObject(imgXian);
			::TransparentBlt(pDC->GetSafeHdc(),x,y,imgXian.GetWidth(),imgXian.GetHeight(),
				dc.GetSafeHdc(),0,0,imgXian.GetWidth(),imgXian.GetHeight(),RGB(255,0,255));
			}

#else
			//如果自己赢
			if(m_iMeXiaZhuInfo[i]==2)
			{
				::TransparentBlt(pDC->GetSafeHdc(),x,y,imgXing.GetWidth()/2,imgXing.GetHeight(),
					dc.GetSafeHdc(),imgXing.GetWidth()/2,0,imgXing.GetWidth()/2,imgXing.GetHeight(),dc.GetPixel(0,0));
			}
			else
			{
			//否则
				::TransparentBlt(pDC->GetSafeHdc(),x,y,imgXing.GetWidth()/2,imgXing.GetHeight(),
					dc.GetSafeHdc(),0,0,imgXing.GetWidth()/2,imgXing.GetHeight(),dc.GetPixel(0,0));
			}
#endif
		}
		else if(m_iResultInfoArea[i] == 3)//和
		{
			x=m_pXing.x + (index)*42;			
			y=m_pXing.y + 60 - 2;
#ifdef SET72 	

			if (m_iMeXiaZhuInfo[i] == 1)
			{
				if(m_iHistoryDuiZi[i]==3||m_iHistoryDuiZi[i]==6)
				{
					imgHe_dot.OpenGDIHandle(&m_imgShowH_Win_dot);
				}
				else
				{
				imgHe.OpenGDIHandle(&m_imgShowH_Win);
				}
			}
			else if (m_iMeXiaZhuInfo[i] == 2)
			{
				if(m_iHistoryDuiZi[i]==3||m_iHistoryDuiZi[i]==6)
				{
					imgHe_dot.OpenGDIHandle(&m_imgShowH_Lost_x_dot);
				}
				else
				{
				imgHe.OpenGDIHandle(&m_imgShowH_Lost_x);
				}
			}
			else
			{
				if(m_iHistoryDuiZi[i]==3||m_iHistoryDuiZi[i]==6)
				{
					imgHe_dot.OpenGDIHandle(&m_imgShowH_dot);
				}
				else
				{
				imgHe.OpenGDIHandle(&m_imgShowH);
				}
			}
			if(m_iHistoryDuiZi[i]==3||m_iHistoryDuiZi[i]==6)
			{
				x+=(42-imgXian_dot.GetWidth())/2;
				dc.SelectObject(imgHe_dot);
				::TransparentBlt(pDC->GetSafeHdc(),x,y,imgHe_dot.GetWidth(),imgHe_dot.GetHeight(),
					dc.GetSafeHdc(),0,0,imgHe_dot.GetWidth(),imgHe_dot.GetHeight(),RGB(255,0,255));
			}
			else
			{

			x+=(42-imgXian.GetWidth())/2;
			dc.SelectObject(imgHe);
			::TransparentBlt(pDC->GetSafeHdc(),x,y,imgHe.GetWidth(),imgHe.GetHeight(),
				dc.GetSafeHdc(),0,0,imgHe.GetWidth(),imgHe.GetHeight(),RGB(255,0,255));
			}

#else
			//如果自己赢
			if(m_iMeXiaZhuInfo[i]>=2)
			{
				::TransparentBlt(pDC->GetSafeHdc(),x,y,imgXing.GetWidth()/2,imgXing.GetHeight(),
					dc.GetSafeHdc(),imgXing.GetWidth()/2,0,imgXing.GetWidth()/2,imgXing.GetHeight(),dc.GetPixel(0,0));
			}
			else
			{
				//否则
				::TransparentBlt(pDC->GetSafeHdc(),x,y,imgXing.GetWidth()/2,imgXing.GetHeight(),
					dc.GetSafeHdc(),0,0,imgXing.GetWidth()/2,imgXing.GetHeight(),dc.GetPixel(0,0));
			}
#endif
		}
		index++;
		if (index > 9)
		{
			break;
		}
	}

    dc.SelectObject(OldBmp);
	dc.DeleteDC();
	//最近30局开庄闲和的次数，文本输出
	int count[3];
	count[0]=0;
	count[1]=0;
	count[2]=0;
	for(int i=0;i<MAXCOUNT;i++)
	{
		if(m_iResultInfoArea[i] == 1)//庄
		{
			count[0]++;
		}
		else if(m_iResultInfoArea[i] == 2)//闲
		{
			count[1]++;
		}
		else if(m_iResultInfoArea[i] == 3)//和
		{
			count[2]++;
		}		
	}
	str.Format("%d",count[0]);//开庄次数
	rectName.SetRect(m_pZXcount.x, m_pZXcount.y-3, m_pZXcount.x+48, m_pZXcount.y+32-3);
	pDC->DrawText(str,str.GetLength(),&rectName,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

	str.Format("%d",count[1]);//开闲次数
	rectName.SetRect(m_pZXcount.x,m_pZXcount.y+32-4,m_pZXcount.x+48, m_pZXcount.y+64-4);
	pDC->DrawText(str,str.GetLength(),&rectName,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

	str.Format("%d",count[2]);//开和次数
	rectName.SetRect(m_pZXcount.x,m_pZXcount.y+64-6,m_pZXcount.x+48, m_pZXcount.y+96-6);
	pDC->DrawText(str,str.GetLength(),&rectName,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

}

///名称：DrawMeInfo
///描述：绘制玩家自己的信息：金币，命中率，总计成绩
///@param pDC 绘制dc 。iWidth 窗口宽度。iHeight 窗口高度。
///@return 
void CClientPlayView::DrawMeInfo(CDC*pDC, int iWidth, int iHeight)
{
	CRect rectName(0,0,0,0);
	pDC->SetTextColor(RGB(255,255,255));

	char Text[128];
	memset(&Text,0,sizeof(Text));
	//NumToChar(m_iNTmoney,Text);

	//玩家自己的信息
	//昵称
	//m_pUserItem[m_byMyStation]
	CString str="--";
	//m_PlayView.m_byMyStation
	if(m_pUserItem[m_byMyStation]!=NULL && m_byMyStation!=255)
	{
		str.Format("%s",m_pUserItem[m_byMyStation]->GameUserInfo.nickName);
	}
	rectName.top=m_pMeInfo.y;
	rectName.bottom = rectName.top+28;
	rectName.left = m_pMeInfo.x + 55;
	rectName.right = rectName.left + 130;
	pDC->DrawText(str,str.GetLength(),&rectName,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

	//金币
	__int64 mymoney=0;      //注意替换该值
	int temp=0;
	if(m_pUserItem[m_byMyStation]!=NULL)
	{
		if((!m_bShowResult && m_bNowGameState==GS_PLAY_GAME) || m_bNowGameState==GS_SEND_CARD)
		{
			mymoney = m_pUserItem[m_byMyStation]->GameUserInfo.i64Money - GetMyAllNote();
		}
		else
		{
			mymoney = m_pUserItem[m_byMyStation]->GameUserInfo.i64Money;
		}
	}
	rectName.top=m_pMeInfo.y +24 ;
	rectName.bottom = rectName.top+22;
	rectName.left = m_pMeInfo.x+55;
	rectName.right = rectName.left + 130;
	str="";
	
	//NumToChar(mymoney,&str);
     ChangeNumToCStringByPower(mymoney ,true ,"," ,str) ; 
	 pDC->DrawText(str,str.GetLength(),&rectName,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

	//m_iWinmoney=0;			//当前玩家输赢的金币
	//m_iMeWinCount=0;		//玩家自己赢的局数
	//m_iMePlayCount=1;		//玩家自己进行的局数


	if(m_iMePlayCount>0&&m_byMyStation!=m_iNowNTstation)
	{
		temp=0;
		temp = m_iMeWinCount*100/m_iMePlayCount;
		CString str1="",str2="";
		str1.Format("%d",temp);
		temp = m_iMeWinCount*100*100/m_iMePlayCount;
		temp %= 100; 
		if(temp<10)
		{
			str2.Format("0%d",temp);
		}
		else
		{
			str2.Format("%d",temp);
		}

		str=str1+ "."+str2 + "%";
		CString outstr;
		outstr.Format("zhtout:%s m_iMeWinCount=%d m_iMePlayCount=%d %s",str,m_iMeWinCount,m_iMePlayCount,m_pUserItem[m_byMyStation]->GameUserInfo.nickName);
		OutputDebugString(outstr);
	}
	else
	{
		str="0.00%";
	}
	rectName.top=m_pMeInfo.y +48 ;
	rectName.bottom = rectName.top+20;
	rectName.left = m_pMeInfo.x+55;
	rectName.right = rectName.left + 100;
	pDC->DrawText(str,str.GetLength(),&rectName,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);


	  // 输出
	//合计（总输赢金币）
	str="";
	//NumToChar(m_iWinmoney,&str);
	 ChangeNumToCStringByPower(m_iWinmoney ,true ,"," ,str) ; 

	rectName.top=m_pMeInfo.y +68 ;
	rectName.bottom = rectName.top+20;
	rectName.left = m_pMeInfo.x+55;
	rectName.right = rectName.left + 100;
	pDC->DrawText(str,str.GetLength(),&rectName,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

	pDC->SetTextColor(RGB(255,255,102));
}


///名称：DrawActiveInfo
///描述：绘制动态信息：无庄（提示），下注（提示下注），开牌（逐个提示开牌信息），空闲时间
///@param pDC 绘制dc 。iWidth 窗口宽度。iHeight 窗口高度。
///@return 
void CClientPlayView::DrawActiveInfo(CDC*pDC, int iWidth, int iHeight)
{
	CString str="";

	//无庄
	if(m_iNowNTstation<0)
	{//当前无庄，申请即可上庄
		str="当前无庄...";//m_pTextStation
		pDC->TextOutA(m_pTextStation.x ,m_pTextStation.y,str);
		str="申请即可上庄";//
		pDC->TextOutA(m_pTextStation.x ,m_pTextStation.y+16,str);
	}
	else if(m_bNowGameState==GS_SEND_CARD)////下注
	{//z游戏开始，请您下分
		if(m_byMyStation == m_iNowNTstation)
		{
			str="游戏开始了，请您等待闲家下注...";//m_pTextStation
		}
		else
		{
			str="游戏开始了，请您下注...";//m_pTextStation
		}
		pDC->TextOutA(m_pTextStation.x ,m_pTextStation.y,str);
	}
	else if(m_bNowGameState==GS_PLAY_GAME)//开牌
	{
		if(m_iShowCard==0)//根据开牌个数
		{
			str="开始开牌...";//m_pTextStation
			pDC->TextOutA(m_pTextStation.x ,m_pTextStation.y,str);
		}
		else if(m_iShowCard==1)//闲1张
		{
			str="开始开牌...";//m_pTextStation
			pDC->TextOutA(m_pTextStation.x ,m_pTextStation.y,str);
		}
		else if(m_iShowCard==2)//庄闲各1张
		{
			str="开始开牌...";//m_pTextStation
			pDC->TextOutA(m_pTextStation.x ,m_pTextStation.y,str);

		}
		else if(m_iShowCard==3)//闲两张，庄1张
		{
			str="开始开牌...";//m_pTextStation
			pDC->TextOutA(m_pTextStation.x ,m_pTextStation.y,str);
		}
		else if(m_iShowCard==4)//庄闲两张牌
		{
			int value1 = CPokerGameLogic::GetCardNumber(m_iNowCard[1][0]);//获取牌点
			int value2 = CPokerGameLogic::GetCardNumber(m_iNowCard[1][1]);//获取牌点

			if(value1>=10) {value1=0;}
			if(value2>=10) {value2=0;}
			value1 += value2;
			if(value1>=10) {value1%=10;}

			value1 = m_iXPaiXing[0];//直接取闲前两张牌点lym2010-03-18

			if(value1>=8)
			{
				str.Format("%s点数：%d 天王  双方停牌",m_XiangStr,value1);
				if (m_iGamePictureType)
				{
					str.Format("地点数：%d 天王  双方停牌",value1);
				}
			}
			else if(value1>=6)
			{
				str.Format("%s点数：%d  停牌",m_XiangStr,value1);
				if (m_iGamePictureType)
				{
					str.Format("地点数：%d  停牌",value1);
				}
			}
			else 
			{
				str.Format("%s点数：%d ",m_XiangStr,value1);
				if (m_iGamePictureType)
				{
					str.Format("地点数：%d ",value1);
				}				
			}
			pDC->TextOutA(m_pTextStation.x ,m_pTextStation.y,str);

			int value4 = CPokerGameLogic::GetCardNumber(m_iNowCard[0][0]);//获取牌点
			int value5 = CPokerGameLogic::GetCardNumber(m_iNowCard[0][1]);//获取牌点
			if(value4>=10) {value4=0;}
			if(value5>=10) {value5=0;}
			value4 += value5;
			if(value4>=10) {value4 %= 10;}

			value4 = m_iZPaiXing[0];//直接取庄前两张牌点lym2010-03-18

			if(value4>=8)
			{
				str.Format("%s点数：%d 天王  双方停牌",m_ZhuangStr,value4);
				if (m_iGamePictureType)
				{
					str.Format("天点数：%d 天王  双方停牌",value4);
				}
			}
			else if(value4>=6 && value1>=6)
			{
				str.Format("%s点数：%d  停牌",m_ZhuangStr,value4);
				if (m_iGamePictureType)
				{
					str.Format("天点数：%d  停牌",value4);
				}
			}
			else 
			{
				str.Format("%s点数：%d ",m_ZhuangStr,value4);
				if (m_iGamePictureType)
				{
					str.Format("天点数：%d ",value4);
				}
			}
			pDC->TextOutA(m_pTextStation.x ,m_pTextStation.y+16,str);
			str="";
			if(value1>=6 && value1<=7 && value4<=5)
			{
				str.Format("%s拿牌",m_ZhuangStr);
				if (m_iGamePictureType)
				{
					str.Format("天拿牌");
				}
			}
			if(m_CardCount[0]==2 && m_CardCount[1]==2)//不用拿牌了
			{
				str="";
				if(m_iWinner==1)//庄赢
				{
					str.Format("%s赢",m_ZhuangStr);
					if (m_iGamePictureType)
					{
						str.Format("天赢");
					}
				}
				else if(m_iWinner==2)//闲赢
				{
					str.Format("%s赢",m_XiangStr);
					if (m_iGamePictureType)
					{
						str.Format("地赢");
					}
				}
				else if(m_iWinner==3)
				{
					str.Format("和赢");
				}

			}
			pDC->TextOutA(m_pTextStation.x ,m_pTextStation.y+32,str);
		}
		else if(m_iShowCard==5)//闲3张牌，
		{
			int value1 = CPokerGameLogic::GetCardNumber(m_iNowCard[1][0]);//获取牌点
			int value2 = CPokerGameLogic::GetCardNumber(m_iNowCard[1][1]);//获取牌点
			int value3 = CPokerGameLogic::GetCardNumber(m_iNowCard[1][2]);//获取牌点
			if(value1>=10) {value1=0;}
			if(value2>=10) {value2=0;}
			if(value3>=10) {value3=0;}
			value1 += (value2+value3);
			if(value1>=10) 
			{
				value1%=10;
			}
			value1 = m_iXPaiXing[1];//直接取闲前两张牌点lym2010-03-18
			if (value1>=8)
			{
				str.Format("%s点数：%d 天王 ",m_XiangStr,value1);
				if (m_iGamePictureType)
				{
					str.Format("地点数：%d 天王 ",value1);
				}
			}
			else
			{
				str.Format("%s点数：%d ",m_XiangStr,value1);
				if (m_iGamePictureType)
				{
					str.Format("地点数：%d ",value1);
				}
			}	
			pDC->TextOutA(m_pTextStation.x ,m_pTextStation.y,str);
			int value4 = CPokerGameLogic::GetCardNumber(m_iNowCard[0][0]);//获取牌点
			int value5 = CPokerGameLogic::GetCardNumber(m_iNowCard[0][1]);//获取牌点
			if(value4>=10) {value4=0;}
			if(value5>=10) {value5=0;}
			value4 += value5;
			if(value4>=10) {value4 %= 10;}

			value4 = m_iZPaiXing[1];//直接取庄前两张牌点lym2010-03-18

			str.Format("%s点数：%d ",m_ZhuangStr,value4);
			if (m_iGamePictureType)
			{
				str.Format("天点数：%d ",value4);
			}
			pDC->TextOutA(m_pTextStation.x ,m_pTextStation.y+16,str);
			if(m_bSendCard)//发牌没有结束,说明庄家还能拿牌
			{
				str.Format("%s第三张牌点：%d  %s拿牌",m_XiangStr,value3,m_XiangStr);
				if (m_iGamePictureType)
				{
					str.Format("地第三张牌点：%d  天拿牌",value3);
				}
				pDC->TextOutA(m_pTextStation.x ,m_pTextStation.y+32,str);
			}
			else 
			{
				str="";
				if(m_iWinner==1)//庄赢
				{
					str.Format("%s赢",m_ZhuangStr);
					if (m_iGamePictureType)
					{
						str.Format("天赢");
					}
				}
				else if(m_iWinner==2)//闲赢
				{
					str.Format("%s赢",m_XiangStr);
					if (m_iGamePictureType)
					{
						str.Format("地赢");
					}
				}
				else if(m_iWinner==3)
				{
					str.Format("和赢");
				}
				pDC->TextOutA(m_pTextStation.x ,m_pTextStation.y+32,str);
			}
		}
		else if(m_iShowCard==6)//闲2张牌，庄3张牌
		{
			int value1 = CPokerGameLogic::GetCardNumber(m_iNowCard[1][0]);//获取牌点
			int value2 = CPokerGameLogic::GetCardNumber(m_iNowCard[1][1]);//获取牌点
			if(value1>=10) {value1=0;}
			if(value2>=10) {value2=0;}
			value1 += value2;
			value1 = m_iXPaiXing[1];//直接取闲前两张牌点lym2010-03-18

			if(value1>=10) {value1%=10;}
			str.Format("%s点数：%d ",m_XiangStr,value1);
			if (m_iGamePictureType)
			{	
				str.Format("地点数：%d ",value1);
			}
			pDC->TextOutA(m_pTextStation.x ,m_pTextStation.y,str);

			int value4 = CPokerGameLogic::GetCardNumber(m_iNowCard[0][0]);//获取牌点
			int value5 = CPokerGameLogic::GetCardNumber(m_iNowCard[0][1]);//获取牌点
			int value6 = CPokerGameLogic::GetCardNumber(m_iNowCard[0][2]);//获取牌点
			if(value4>=10) {value4=0;}
			if(value5>=10) {value5=0;}
			if(value6>=10) {value6=0;}
			value4 += (value5+value6);
			value4 = m_iZPaiXing[1];//直接取闲前两张牌点lym2010-03-18
			if(value4>=10) {value4%=10;}
			if (value4>=8)
			{
				str.Format("%s点数：%d 天王 ",m_ZhuangStr,value4);
				if (m_iGamePictureType)
				{	
					str.Format("天点数：%d 天王 ",value4);
				}
			}
			else
			{
				str.Format("%s点数：%d ",m_ZhuangStr,value4);
				if (m_iGamePictureType)
				{	
					str.Format("天点数：%d ",value4);
				}
			}			
			pDC->TextOutA(m_pTextStation.x ,m_pTextStation.y+16,str);
			str="";
			if(m_iWinner==1)//庄赢
			{
				str.Format("%s赢",m_ZhuangStr);
				if (m_iGamePictureType)
				{	
					str.Format("天赢");
				}
			}
			else if(m_iWinner==2)//闲赢
			{
				str.Format("%s赢",m_XiangStr);
				if (m_iGamePictureType)
				{	
					str.Format("地赢");
				}
			}
			else if(m_iWinner==3)
			{
				str.Format("和赢");
			}
			pDC->TextOutA(m_pTextStation.x ,m_pTextStation.y+32,str);
		}
		else if(m_iShowCard==7)//庄闲3张牌
		{
			int value1 = CPokerGameLogic::GetCardNumber(m_iNowCard[1][0]);//获取牌点
			int value2 = CPokerGameLogic::GetCardNumber(m_iNowCard[1][1]);//获取牌点
			int value3 = CPokerGameLogic::GetCardNumber(m_iNowCard[1][2]);//获取牌点
			if(value1>=10) {value1=0;}
			if(value2>=10) {value2=0;}
			if(value3>=10) {value3=0;}
			value1 += (value2+value3);
			value1 = m_iXPaiXing[1];//直接取闲前两张牌点lym2010-03-18
			if(value1>=10) {value1 %= 10;}
			if (value1>=8)
			{
				str.Format("%s点数：%d 天王 ",m_XiangStr,value1);
				if (m_iGamePictureType)
				{	
					str.Format("地点数：%d 天王 ",value1);
				}
			}
			else
			{
				str.Format("%s点数：%d ",m_XiangStr,value1);
				if (m_iGamePictureType)
				{	
					str.Format("地点数：%d ",value1);
				}
			}			
			pDC->TextOutA(m_pTextStation.x ,m_pTextStation.y,str);

			value1 = CPokerGameLogic::GetCardNumber(m_iNowCard[0][0]);//获取牌点
			value2 = CPokerGameLogic::GetCardNumber(m_iNowCard[0][1]);//获取牌点
			value3 = CPokerGameLogic::GetCardNumber(m_iNowCard[0][2]);//获取牌点
			if(value1>=10) {value1=0;}
			if(value2>=10) {value2=0;}
			if(value3>=10) {value3=0;}
			value1 += (value2+value3);
			value1 = m_iZPaiXing[1];//直接取闲前两张牌点lym2010-03-18
			if(value1>=10) 
			{
				value1 %= 10;
			}
			if (value1>=8)
			{
				str.Format("%s点数：%d 天王",m_ZhuangStr,value1);
				if (m_iGamePictureType)
				{	
					str.Format("天点数：%d 天王",value1);
				}
			}
			else
			{
				str.Format("%s点数：%d ",m_ZhuangStr,value1);
				if (m_iGamePictureType)
				{	
					str.Format("天点数：%d ",value1);
				}
			}			
			pDC->TextOutA(m_pTextStation.x ,m_pTextStation.y+16,str);
			str="";
			if(m_iWinner==1)//庄赢
			{
				str.Format("%s赢",m_ZhuangStr);
				if (m_iGamePictureType)
				{	
					str.Format("天赢");
				}
			}
			else if(m_iWinner==2)//闲赢
			{
				str.Format("%s赢",m_XiangStr);
				if (m_iGamePictureType)
				{	
					str.Format("地赢");
				}
			}
			else if(m_iWinner==3)
			{
				str.Format("和赢");
			}
			pDC->TextOutA(m_pTextStation.x ,m_pTextStation.y+32,str);
		}
	}
	else if(m_bNowGameState==GS_WAIT_NEXT)//空闲状态
	{
		str="空闲时间...";//m_pTextStation
		pDC->TextOutA(m_pTextStation.x ,m_pTextStation.y,str);
	}
}

///名称：DrawCard
///描述：绘制牌，最终显示的牌
///@param pDC 绘制dc 。iWidth 窗口宽度。iHeight 窗口高度。
///@return 
void CClientPlayView::DrawCard(CDC*pDC,int iWidth, int iHeight)
{
	if(m_iShowCard==0)
		return;
	int x=0,y=0;
	if(m_iShowCard==1)//
	{
		DrawACard(pDC,iWidth,iHeight,m_iNowCard[1][0],m_pCardXEnd.x,m_pCardXEnd.y);
	}
	else if(m_iShowCard==2)
	{
		DrawACard(pDC,iWidth,iHeight,m_iNowCard[1][0],m_pCardXEnd.x,m_pCardXEnd.y);
		DrawACard(pDC,iWidth,iHeight,m_iNowCard[0][0],m_pCardZEnd.x,m_pCardZEnd.y);
	}
	else if(m_iShowCard==3)
	{
		DrawACard(pDC,iWidth,iHeight,m_iNowCard[1][0],m_pCardXEnd.x,m_pCardXEnd.y);
		DrawACard(pDC,iWidth,iHeight,m_iNowCard[1][1],m_pCardXEnd.x+20,m_pCardXEnd.y);
		DrawACard(pDC,iWidth,iHeight,m_iNowCard[0][0],m_pCardZEnd.x,m_pCardZEnd.y);
	}
	else if(m_iShowCard==4)
	{
		DrawACard(pDC,iWidth,iHeight,m_iNowCard[1][0],m_pCardXEnd.x,m_pCardXEnd.y);
		DrawACard(pDC,iWidth,iHeight,m_iNowCard[1][1],m_pCardXEnd.x+20,m_pCardXEnd.y);
		DrawACard(pDC,iWidth,iHeight,m_iNowCard[0][0],m_pCardZEnd.x,m_pCardZEnd.y);
		DrawACard(pDC,iWidth,iHeight,m_iNowCard[0][1],m_pCardZEnd.x+20,m_pCardZEnd.y);
	}
	else if(m_iShowCard==5)//闲3张牌，庄两张牌
	{
		DrawACard(pDC,iWidth,iHeight,m_iNowCard[1][0],m_pCardXEnd.x,m_pCardXEnd.y);
		DrawACard(pDC,iWidth,iHeight,m_iNowCard[1][1],m_pCardXEnd.x+20,m_pCardXEnd.y);
		DrawACard(pDC,iWidth,iHeight,m_iNowCard[1][2],m_pCardXEnd.x+40,m_pCardXEnd.y);
		DrawACard(pDC,iWidth,iHeight,m_iNowCard[0][0],m_pCardZEnd.x,m_pCardZEnd.y);
		DrawACard(pDC,iWidth,iHeight,m_iNowCard[0][1],m_pCardZEnd.x+20,m_pCardZEnd.y);
	}
	else if(m_iShowCard==6)//闲2张牌，庄3张牌
	{
		DrawACard(pDC,iWidth,iHeight,m_iNowCard[1][0],m_pCardXEnd.x,m_pCardXEnd.y);
		DrawACard(pDC,iWidth,iHeight,m_iNowCard[1][1],m_pCardXEnd.x+20,m_pCardXEnd.y);
		DrawACard(pDC,iWidth,iHeight,m_iNowCard[0][0],m_pCardZEnd.x,m_pCardZEnd.y);
		DrawACard(pDC,iWidth,iHeight,m_iNowCard[0][1],m_pCardZEnd.x+20,m_pCardZEnd.y);
		DrawACard(pDC,iWidth,iHeight,m_iNowCard[0][2],m_pCardZEnd.x+40,m_pCardZEnd.y);
	}
	else if(m_iShowCard==7)//庄闲3张牌
	{
		DrawACard(pDC,iWidth,iHeight,m_iNowCard[1][0],m_pCardXEnd.x,m_pCardXEnd.y);
		DrawACard(pDC,iWidth,iHeight,m_iNowCard[1][1],m_pCardXEnd.x+20,m_pCardXEnd.y);
		DrawACard(pDC,iWidth,iHeight,m_iNowCard[1][2],m_pCardXEnd.x+40,m_pCardXEnd.y);
		DrawACard(pDC,iWidth,iHeight,m_iNowCard[0][0],m_pCardZEnd.x,m_pCardZEnd.y);
		DrawACard(pDC,iWidth,iHeight,m_iNowCard[0][1],m_pCardZEnd.x+20,m_pCardZEnd.y);
		DrawACard(pDC,iWidth,iHeight,m_iNowCard[0][2],m_pCardZEnd.x+40,m_pCardZEnd.y);
	}
}

///名称：EnableXiaZhu
///描述：屏蔽下注或开启下注
///@param enable true 使按钮可用 ，false 使按钮不可用
///@return 
void CClientPlayView::EnableXiaZhu(bool enable)
{
	///20100207zht修改百亿
	__int64 maxcannote = 0;
	for(int i=0;i<8;i++)
	{
		if(maxcannote<m_iMaxZhu[i])
			maxcannote = m_iMaxZhu[i];///<本把每个区域最大能下的注额	
	}
	__int64 nTempMoney=1; 
	for(int i=0; i<Glb().m_nPowerOfGold; i++)
	{
		nTempMoney*=10;
	}
	if(enable && m_iNowNTstation>=0)//开启下注
	{

		__int64 money = GetMyAllNote();
		if(m_pUserItem[m_byMyStation]!=NULL)
		{
				OutputDebugString("申请上庄处理44444");

		CString str="";
		str.Format("申请上庄处理 ：mymoney=%i64d  money=%i64d m_iMaxNote=%i64d   maxcannote=%i64d   %i64d",m_pUserItem[m_byMyStation]->GameUserInfo.i64Money ,money,m_iMaxNote,maxcannote,m_iMyZhu[8]);
		OutputDebugString(str);

		str.Format("申请上庄处理 000 ：%d    %d    %d ",(m_pUserItem[m_byMyStation]->GameUserInfo.i64Money - money) >= 100/nTempMoney,(m_iMaxNote<=0 ||(m_iMaxNote-m_iMyZhu[8])>=100/nTempMoney ),maxcannote >= 100/nTempMoney);
		OutputDebugString(str);

			int cannote=0;
			//EnableXiaZhu(false);
			if((m_pUserItem[m_byMyStation]->GameUserInfo.i64Money - money) >= 100/nTempMoney  && (m_iMaxNote<=0 ||(m_iMaxNote-m_iMyZhu[8])>=100/nTempMoney ) && maxcannote >= 100/nTempMoney )
			{
				OutputDebugString("申请上庄处理33333");
				m_btn100.EnableWindow(true);	// 100筹码按钮
				cannote=1;
			}
			else
			{
				m_btn100.EnableWindow(false);	// 100筹码按钮
			}
			if((m_pUserItem[m_byMyStation]->GameUserInfo.i64Money - money) >= 1000/nTempMoney && (m_iMaxNote<=0 ||(m_iMaxNote-m_iMyZhu[8])>=1000/nTempMoney) && maxcannote >= 1000/nTempMoney )
			{
				m_btn1000.EnableWindow(true);	// 1000筹码按钮
				cannote=2;
			}
			else
			{
				m_btn1000.EnableWindow(false);	// 1000筹码按钮
			}
			if((m_pUserItem[m_byMyStation]->GameUserInfo.i64Money - money) >= 10000/nTempMoney && (m_iMaxNote<=0 ||(m_iMaxNote-m_iMyZhu[8])>=10000/nTempMoney) && maxcannote >= 10000/nTempMoney )
			{
				m_btn1w.EnableWindow(true);		// 1万筹码按钮
				cannote=3;
			}
			else
			{
				m_btn1w.EnableWindow(false);		// 1万筹码按钮
			}
			if((m_pUserItem[m_byMyStation]->GameUserInfo.i64Money - money) >= 100000/nTempMoney && (m_iMaxNote<=0 ||(m_iMaxNote-m_iMyZhu[8])>=100000/nTempMoney) && maxcannote >= 100000/nTempMoney )
			{
				m_btn10w.EnableWindow(true);	// 10万筹码按钮
				cannote=4;
			}
			else
			{
				m_btn10w.EnableWindow(false);	// 10万筹码按钮
			}
			if((m_pUserItem[m_byMyStation]->GameUserInfo.i64Money - money) >= 1000000/nTempMoney && (m_iMaxNote<=0 ||(m_iMaxNote-m_iMyZhu[8])>=1000000/nTempMoney)  && maxcannote >= 1000000/nTempMoney )
			{
				CString str="";
				str.Format("下注测试：m_iMaxNote=%i64d  tt=%i64d  gg=%i64d  m_iMyZhu[8]=%i64d",m_iMaxNote,m_iMaxNote-m_iMyZhu[8],1000000/nTempMoney,m_iMyZhu[8]);
				OutputDebugString(str);
				m_btn100w.EnableWindow(true);	// 100万筹码按钮
				cannote=5;
			}
			else
			{
				m_btn100w.EnableWindow(false);	// 100万筹码按钮
			}
///72把一局
#ifdef SET72 

			int iMaxjeton = 10000000 ;
			/*if(m_bUse500WJeton)
			{
				iMaxjeton = 5000000 ; 
			}*/

			if((m_pUserItem[m_byMyStation]->GameUserInfo.i64Money - money) >= iMaxjeton/nTempMoney && (m_iMaxNote<=0 ||(m_iMaxNote-m_iMyZhu[8])>=iMaxjeton/nTempMoney ) && maxcannote >= iMaxjeton/nTempMoney )
			{
				m_btn500w.EnableWindow(true);	// 500万筹码按钮
				cannote=6;
			}
			else
			{
				m_btn500w.EnableWindow(false);	// 500万筹码按钮
			}
#else
			if((m_pUserItem[m_byMyStation]->GameUserInfo.i64Money - money) >= 5000000/nTempMoney && (m_iMaxNote<=0 ||(m_iMaxNote-m_iMyZhu[8])>=5000000/nTempMoney ) && maxcannote >= 5000000/nTempMoney )
			{
				m_btn500w.EnableWindow(true);	// 500万筹码按钮
				cannote=6;
			}
			else
			{
				m_btn500w.EnableWindow(false);	// 500万筹码按钮
			}
#endif

			if(m_iChooseNote>cannote)
			{				
				//m_iChooseNote=cannote;
				//OnShowCursor(FALSE);
				OnRestoreCursor();
				switch(cannote)
				{
				case 1:
					OnBtnClicked100();	// 100筹码按钮
					break;
				case 2:
					OnBtnClicked1000();	// 1000筹码按钮
					break;
				case 3:
					OnBtnClicked1w();		// 1万筹码按钮
					break;
				case 4:
					OnBtnClicked10w();	// 100万筹码按钮
					break;
				case 5:
					OnBtnClicked100w();	// 500万筹码按钮
					break;
				}

			}
		}
		else
		{
			m_btn100.EnableWindow(true);	// 100筹码按钮
			m_btn1000.EnableWindow(true);	// 1000筹码按钮
			m_btn1w.EnableWindow(true);		// 1万筹码按钮
			m_btn10w.EnableWindow(true);	// 10万筹码按钮
			m_btn100w.EnableWindow(true);	// 100万筹码按钮
			m_btn500w.EnableWindow(true);	// 500万筹码按钮
		}
	}
	else //屏蔽下注
	{
		m_btn100.EnableWindow(false);	// 100筹码按钮
		m_btn1000.EnableWindow(false);	// 1000筹码按钮
		m_btn1w.EnableWindow(false);		// 1万筹码按钮
		m_btn10w.EnableWindow(false);	// 10万筹码按钮
		m_btn100w.EnableWindow(false);	// 100万筹码按钮
		m_btn500w.EnableWindow(false);	// 500万筹码按钮
	}
}
///启动超过500万的动画
void CClientPlayView::StartChao500Move(int type)
{
	//SetTimer(IDR_FAPAI,100,NULL);
	//m_iMoveTime=10;
	m_iMoveType = type+1;

}
///绘制超过500万的动画
void CClientPlayView::DrawChao500Move(CDC*pDC,int iWidth, int iHeight)
{
	if(m_iMoveTime>0)
	{
		CPoint piont(0,0);
		piont=m_iYUcenter[m_iMoveType-1];
	/////绘制动画帧
	}
}

///名称：DrawCursor
///描述：绘制光标动画
///@param pDC 绘制dc 。iWidth 窗口宽度。iHeight 窗口高度。
///@return 
void CClientPlayView::DrawCursor(CDC*pDC,int iWidth, int iHeight)
{
	if(m_bNowGameState != GS_SEND_CARD)//非下注时间不响应
		return;
	if(	m_byMyStation==m_iNowNTstation)//自己的位置,庄家不能下注
		return;

	if(m_iChooseNote>0 && m_byMyStation!=m_iNowNTstation)
	{
		CDC dc;
		dc.CreateCompatibleDC(pDC);
		CGameImageHelper	cursor(&m_imgchouma);
		CBitmap *pOldBmp = dc.SelectObject(cursor);
		::TransparentBlt(pDC->GetSafeHdc(),m_pCursor.x-16,m_pCursor.y-16,cursor.GetWidth()/6,cursor.GetHeight(),
			dc.GetSafeHdc(),(m_iChooseNote-1)*cursor.GetWidth()/6,0,cursor.GetWidth()/6,cursor.GetHeight(),RGB(255,0,255));
		dc.SelectObject(pOldBmp);
		dc.DeleteDC();
	}
	else
	{
	//	ShowCursor(TRUE);
	}
}
///更新结算窗口的位置
void CClientPlayView::UpdateResultWindow(int x,int y)
{
//m_pViewCenter
	//CRect rect(0,0,0,0);
	//GetClientRect(&rect);
	//CGameImage reimg;
	//TCHAR path[MAX_PATH];
	//wsprintf(path,TEXT("image\\result.bmp"));
	//reimg.SetLoadInfo(path,true);
	//CPoint point(m_pDitu.x+135-126,m_pDitu.y +35+y+48);
	//ClientToScreen(&point);
	//SafeMoveWindow(&m_Result, point.x,m_pDitu.y +35+y+48/*m_pDitu.y +35+y+48*/ , reimg.GetWidth(),reimg.GetHeight());
	//UpdateViewFace(NULL);
}

///名称：DrawResultFrame
///描述：绘制透明结算框
///@param pDC 绘制dc 。iXPos 位置。iYPos 位置。
///@return 
void CClientPlayView::DrawResultFrame(CDC*pDC, int iXPos, int iYPos)
{
	if(!m_bShowResult)
		return;
	CGameImageHelper help(&m_imgResult);
	CGameImageHelper help1(&m_imgResult1);
	CDC dc;
	dc.CreateCompatibleDC(pDC);
	CBitmap *pOldBmp = (CBitmap*)dc.SelectObject(help);

	CDC memdc;
	memdc.CreateCompatibleDC(pDC);	
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(pDC, help.GetWidth(), help.GetHeight());
	CBitmap *pOldBmp1 = (CBitmap*)memdc.SelectObject( &bitmap );
	memdc.SetBkMode(TRANSPARENT);

	// 贴上背景图片
	memdc.BitBlt(0,0, help.GetWidth(), help.GetHeight(), pDC, iXPos, iYPos, SRCCOPY);

	// 结算框框
	::TransparentBlt(memdc.GetSafeHdc(),0,0,help.GetWidth(), help.GetHeight(),dc.GetSafeHdc(),0,0,help.GetWidth(),help.GetHeight(),RGB(255,0,255));	

	// 设置透明
	BLENDFUNCTION rBlendProps;
	rBlendProps.BlendOp = AC_SRC_OVER;
	rBlendProps.BlendFlags = 0;
	rBlendProps.AlphaFormat = 0;
	rBlendProps.SourceConstantAlpha = 150;//
	AlphaBlend( pDC->GetSafeHdc(), iXPos, iYPos, help.GetWidth(), help.GetHeight(), memdc.GetSafeHdc(), 0, 0, 
		help.GetWidth(), help.GetHeight(), rBlendProps );
	dc.SelectObject(help1);
	// 结算框框
	::TransparentBlt(pDC->GetSafeHdc(),iXPos+1,iYPos,help1.GetWidth(), help1.GetHeight(),dc.GetSafeHdc(),0,0,help1.GetWidth(),help1.GetHeight(),RGB(255,0,255));	

	memdc.SelectObject(pOldBmp1);
	memdc.DeleteDC();
	dc.SelectObject(pOldBmp);
	dc.DeleteDC();
/////////////////////////////////////////
	//CString strme="",strnt="",strxian="",stra=""; 
	//int money=0;
	//int temp=0;
	////money = m_iMeFen;
	//strme="";
	//NumToChar(m_iMeFen,&strme);
	//strnt="";
	//NumToChar(m_iZhuangFen,&strnt);
	//strxian="";
	//NumToChar(m_iXianFen,&strxian);


	//绘制文字信息m_pDitu
	CFont font, *pOldFont;
	font.CreateFont(16,0,0,0,700,NULL,NULL,NULL,134,3,2,1,2,TEXT("黑体"));
	pOldFont = (CFont*)pDC->SelectObject(&font);
	pDC->SetBkMode(TRANSPARENT);
	CRect rectName(0,0,0,0);
	
	CString str="";
	str.Format("结算：庄分=%d 我分=%d",m_iZhuangFen,m_iMeFen);
	OutputDebugString(str);

///////////////////////////////////////////////////////////
	if(m_byMyStation == m_iNowNTstation)//自己是庄家
	{
		pDC->SetTextColor(RGB(255,255,100));//自家用红色
		CString str="";
		char szChar[MAX_PATH] ={ 0 } ; 
		//NumToChar(m_iMeFen,&str);

		 ChangeNumToCStringByPower(m_iMeFen ,true ,"," ,str) ; 

		//绘制本方的得分
		rectName.SetRect(iXPos+48,iYPos+38,iXPos+150+25,iYPos+67);
		if(m_isWinRed && m_iMeFen>0)
			pDC->SetTextColor(RGB(255,0,0));//自家用红色
		else
			pDC->SetTextColor(RGB(m_Rcolor,m_Gcolor,m_Bcolor));
		pDC->DrawText(str,str.GetLength(),&rectName,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

		//绘制本方的返还分m_iMeFanFen
		//NumToChar(m_iMeFanFen,&str);
		 ChangeNumToCStringByPower(m_iMeFanFen ,true ,"," ,str) ; 

		rectName.SetRect(iXPos+154+20,iYPos+38,iXPos+278+40,iYPos+67);
		pDC->DrawText(str,str.GetLength(),&rectName,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

		//////////
		//庄家得分
		//NumToChar(m_iZhuangFen,&str);
		ChangeNumToCStringByPower(m_iZhuangFen ,true ,"," ,str) ;  

		//绘制本方的得分
		rectName.SetRect(iXPos+48,iYPos+71+2,iXPos+150+25,iYPos+100+2);
		if(m_isWinRed && m_iZhuangFen>0)
			pDC->SetTextColor(RGB(255,0,0));//自家用红色
		else
			pDC->SetTextColor(RGB(m_Rcolor,m_Gcolor,m_Bcolor));
		pDC->DrawText(str,str.GetLength(),&rectName,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
	}
	else 
	{
		pDC->SetTextColor(RGB(255,255,100));//自家用红色
		CString str="";
		char szChar[MAX_PATH] ={ 0 } ; 

		//NumToChar(m_iMeFen,&str);
		ChangeNumToCStringByPower(m_iMeFen ,true ,"," ,str) ;

		//绘制本方的得分
		rectName.SetRect(iXPos+48,iYPos+38,iXPos+150+25,iYPos+67);
		if(m_isWinRed && m_iMeFen>0)
			pDC->SetTextColor(RGB(255,0,0));//自家用红色
		else
			pDC->SetTextColor(RGB(m_Rcolor,m_Gcolor,m_Bcolor));
		pDC->DrawText(str,str.GetLength(),&rectName,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
		//绘制本方的返还分m_iMeFanFen
		//NumToChar(m_iMeFanFen,&str);
		ChangeNumToCStringByPower(m_iMeFanFen ,true ,"," ,str) ;

		rectName.SetRect(iXPos+154+20,iYPos+38,iXPos+278+40,iYPos+67);
		pDC->DrawText(str,str.GetLength(),&rectName,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
		//////////
		//庄家得分
		//NumToChar(m_iZhuangFen,&str);
		ChangeNumToCStringByPower(m_iZhuangFen ,true ,"," ,str) ; 

		//绘制本方的得分
		rectName.SetRect(iXPos+48,iYPos+71+2,iXPos+150+25,iYPos+100+2);
		if(m_isWinRed && m_iZhuangFen>0)
			pDC->SetTextColor(RGB(255,0,0));//自家用红色
		else
			pDC->SetTextColor(RGB(m_Rcolor,m_Gcolor,m_Bcolor));
		pDC->DrawText(str,str.GetLength(),&rectName,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
	}
	pDC->SelectObject(pOldFont);
	font.DeleteObject();

}

///名称：ShowNowNote
///描述：断线重连后显示当前下注的筹码
///@param 
///@return 
void CClientPlayView::ShowNowNote()
{ 
	///20100207zht修改百亿
	__int64 nTempMoney=1; 
	for(int i=0; i<Glb().m_nPowerOfGold; i++)
	{
		nTempMoney*=10;
	}

				
	for(int i=0;i<8;i++)//八个区域的金币
	{
		if(m_iQuYuZhu[i]>0)
		{
			__int64 temp=0;
			__int64 num500w=0,num100w=0,num10w=0,num1w=0,num1000=0,num100=0;
			temp = m_iQuYuZhu[i];
			num500w = temp/(5000000/nTempMoney);
			temp%=(5000000/nTempMoney);
			num100w = temp/(1000000/nTempMoney);
			temp%=(1000000/nTempMoney);
			num10w = temp/(100000/nTempMoney);
			temp%=(100000/nTempMoney);
			num1w = temp/(10000/nTempMoney);
			temp%=(10000/nTempMoney);
			num1000 = temp/(1000/nTempMoney);
			temp%=(1000/nTempMoney);
			num100 = temp/(100/nTempMoney);
			SetNowNoteData(i,num500w,num100w,num10w,num1w,num1000,num100);
			CString str;
			str.Format("zhtout:m_iQuYuZhu[%d]=%i64d %i64d,%i64d,%i64d,%i64d,%i64d,%i64d",i,m_iQuYuZhu[i],num500w,num100w,num10w,num1w,num1000,num100);
			OutputDebugString(str);
		}
	}


}

///名称：SetNowNoteData
///描述：断线重连后显示当前下注。设置当前下注的数据
///@param type区域 。num5bw 500万的筹码数量。num1bw 100万筹码数量。num10w 10万筹码数量。num1w 1万筹码数量。numq 1000筹码数量。numb 100筹码数量。
///@return 
void CClientPlayView::SetNowNoteData(int type,int num5bw,int num1bw,int num10w,int num1w,int numq,int numb)
{
	__int64 numnote = 0;
	numnote = num5bw + num1bw + num10w + num1w + numq + numb;
	CPoint point(0,0);
	for(int i=0;i<numnote;i++)
	{
		point.SetPoint(0,0);
		switch(type)
		{
		case 0://庄
			{
				point.x=(rand()%20 - 10)*5;
				point.y=(rand()%16 - 8)*4;
			}
			break;
		case 1://庄天王
			{
				point.x=(rand()%20 - 10)*5;
				point.y=(rand()%6 - 3)-5;
			}
			break;
		case 2://庄对子
			{
				point.x=(rand()%25 - 12)*5;
				point.y=(rand()%6 - 3)*3;
			}
			break;
		case 3://闲
			{
				point.x=(rand()%20 - 10)*5;
				point.y=(rand()%16 - 8)*4;
			}
			break;
		case 4://闲天王
			{
				point.x=(rand()%20 - 10)*5;
				point.y=(rand()%6 - 3)-5;
			}
			break;
		case 5://闲对子
			{
				point.x=(rand()%20 - 10)*5;
				point.y=(rand()%6 - 3)*3;
			}
			break;
		case 6://和
			{
				point.x=(rand()%20 - 10)*5;
				point.y=(rand()%12 - 6)*4;
			}
			break;
		case 7://同点和
			{
				point.x=(rand()%20 - 10)*5;
				point.y=(rand()%6 - 3)*3;
			}
			break;
		}
		m_pJinBi[type][i].x = point.x;     //金币的偏移量 
		m_pJinBi[type][i].y = point.y;     //金币的偏移量  

		if(num5bw>0 && i<num5bw)//有500万的金币
		{
			m_iJinBi[type][i] = 6;
		}
		else if(num1bw && i<(num5bw + num1bw))
		{
			m_iJinBi[type][i] = 5;
		}
		else if(num10w && i<(num5bw + num1bw + num10w))
		{
			m_iJinBi[type][i] = 4;
		}
		else if(num1w && i<(num5bw + num1bw + num10w + num1w))
		{
			m_iJinBi[type][i] = 3;
		}
		else if(numq && i<(num5bw + num1bw + num10w + num1w + numq))
		{
			m_iJinBi[type][i] = 2;
		}
		else
		{
			m_iJinBi[type][i] = 1;
		}
	}

}

///名称：Init
///描述：每局结束后初始化视图数据
///@param 
///@return 
void CClientPlayView::Init()
{
	//m_LuziDlg.ShowWindow(false);
	m_bShowRect = false ;//控制赢区域的闪烁
	memset(&m_iZPaiXing, 0, sizeof(m_iZPaiXing)); //庄家牌型,元素0前两张牌的值，元素1总牌值，元素2天王，元素3对子，元素4和
	memset(&m_iXPaiXing, 0, sizeof(m_iXPaiXing));//闲家牌型
	m_iWinner = 0 ;				//赢家1 庄，2闲，3和，本赢方
	m_iKaiPai = 0 ;				//本把开牌区域：1庄，2庄天王，3庄对子，4闲，5闲天王，6闲对子，和，同点和
	m_bShowResult = false; //是否显示结算信息
	memset(&m_CardCount, 0, sizeof(m_CardCount));		 //庄闲牌的张数
	memset(&m_iNowCard, 0, sizeof(m_iNowCard));		 //当前牌点  
	memset(&m_iResult, -1, sizeof(m_iResult));		 //保存本局：庄牌点，闲牌点，庄牌型，闲牌型，赢牌区域，自己得金币
	memset(&m_iZCardInfo, -1, sizeof(m_iZCardInfo));		  //庄的牌信息：庄家的牌数，1张牌的点数，前两张牌的点数 
	memset(&m_iXCardInfo, -1, sizeof(m_iXCardInfo));		  //闲的牌信息：庄家的牌数，闲家的牌数，
	memset(&m_iQuYuZhu, 0, sizeof(m_iQuYuZhu));		//本把每个区域下的注额：0庄，1庄天王，2庄对子，3闲，4闲天王，5闲对子，6和，7同点和
	memset(&m_iMyZhu, 0, sizeof(m_iMyZhu));			//本把玩家自己在每个区域下的注额：0庄，1庄天王，2庄对子，3闲，4闲天王，5闲对子，6和，7同点和
	memset(&m_iMaxZhu, 0, sizeof(m_iMaxZhu));	//本把每个区域最大能下的注额
	memset(&m_iJinBi, 0, sizeof(m_iJinBi));	//玩家在八个区域下的金币情况，记录前2000个
	memset(&m_pJinBi, 0, sizeof(m_pJinBi));	//玩家在八个区域下的金的位置情况，记录前2000个
	memset(&m_iJinBiNum, 0, sizeof(m_iJinBiNum));	 //八个区域，每个区域的金币数
	//memset(&m_iMePlayData, 0, sizeof(m_iMePlayData));	 //玩家自己的输赢情况
	memset(&m_iWinQuYu, 0, sizeof(m_iWinQuYu));	//游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和
	// 清空显示牌数据
	::ZeroMemory(m_ContinueShowCardCount, sizeof(m_ContinueShowCardCount));

	::ZeroMemory(m_iTempGameInfo,sizeof(m_iTempGameInfo));
	
	m_iMoveTime =0 ;  //动画时间
	m_iMoveType = -1;  //动画的绘制区域

	m_iShowCard = 0;//显示牌
	m_iCardNum =0;				//播放第几张牌的动画
	m_bZhuang=false;				//区别是庄家还是闲家的牌
	m_bCardMove=false;			//是否正在进行发牌
	m_bSendCard =false;			//确定是否发牌结束
	m_iChooseNote=0;			///当前选择的筹码


	m_iChooseRect=-1;			//鼠标所在的区域代号
	m_iCardZhen =-1;			//当前发牌第几帧
	m_iShowCard=0;			//显示牌,0不显示，1显示一张，2显示
	m_iZhongZhu =0;			 //本把当前总注额	
	m_iAllJiBiCount=0;        //当前已经押了多少金币；

	m_iZhuangFen=0;		//本把庄家的得分
	m_iXianFen=0;			//本把闲家的得分
	m_iMeFen=0;			//本把玩家自己的得分
	m_iMeFanFen = 0;		//本把玩家返还的分，开和时出现
	
	if(!CheckMeZhuang())
	{
		//变为下庄图片
		SetApplyBanker(true);
	}
	else
	{
		//变为上庄图片
		SetApplyBanker(false);
	}
///lym2010-03-19能否点击上庄按钮
	m_bCanShangZhuang = true;
	m_iMessageCount = 0;
	//m_iNowJuShu = 0;				//本局进行的局数，进行到第几局（72把一局）
	memset(&m_iAIMoney, 0, sizeof(m_iAIMoney));	///机器人的下注
	m_bHideAIMoney = true;			///是否隐藏机器人下注
	for(int i=0;i<8;i++)
	{
		m_iJinBiNum[i] = 0;//每个区域的金币数
		for(int j=0;j<MAXJINBI;j++)
		{
			m_iJinBi[i][j]=0;    //玩家在八个区域下的金币情况，记录前2000个
			m_pJinBi[i][j].x=0;  //玩家在八个区域下的金的位置情况，记录前2000个
			m_pJinBi[i][j].y=0;  //玩家在八个区域下的金的位置情况，记录前2000个
		}
	}
	
}

/// 设置申请上庄按钮
///@param bApply true上庄 ,false 下庄
void CClientPlayView::SetApplyBanker(bool bApply)
{
	/*CString fdsa;
	fdsa.Format("MSG_%d,%d_____123",m_bNowGameState,ff);
	OutputDebugString(fdsa);*/

	TCHAR szName[MAX_PATH];
	wsprintf(szName, bApply ? TEXT("image\\shenqing_bt.bmp") : TEXT("image\\xiazhuan_bt.bmp"));
	m_btnshangz.LoadButtonBitmap(szName,false);
}

/// 恢复鼠标正常状态
void CClientPlayView::OnRestoreCursor()
{ 
	OnShowCursor(TRUE);
	SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND1))); //设置光标
	GetCursorPos(&m_pCursor);        //筹码光标的位置);
	ScreenToClient(&m_pCursor);
}

/// 设置鼠标状态
///@param bShow true显示鼠标 ,false 隐藏鼠标
void CClientPlayView::OnShowCursor(BOOL bShow)
{
	int iCount=ShowCursor(bShow);
	while ((bShow && 0>iCount) || (!bShow && 0<=iCount))
	{
		iCount = ShowCursor(bShow);
	}
	
}

///将数字转成带字符串
///@param result 要转的数字  szText 转换后的字符串
void CClientPlayView::NumToChar(__int64 result,CString* szText)
{
	CString str1="0",str2="";
	CString str="";
	str.Format("%I64d",result);

	if (m_nPowerOfGold<=0||0 == result)
	{
		str2="";
	}
	else
	{
		for (int i=m_nPowerOfGold;i>0;i--)
		{
			str2+=str1;
		}
	}

// 	if(result==0)
// 	{
// 		str2 = "";
// 	}
	*szText = str + str2;


	
	
	//添加“，”分隔符

	if(m_SpeCount==4)
	{

		if(result>0)//负数
		{
			if(szText->GetLength()>8)
			{
				szText->Insert(szText->GetLength()-4,',');
				szText->Insert(szText->GetLength()-9,',');
			}
			else if(szText->GetLength()>4)
			{
				szText->Insert(szText->GetLength()-4,',');
			}
		}
		else
		{
			if(szText->GetLength()>9)
			{
				szText->Insert(szText->GetLength()-4,',');
				szText->Insert(szText->GetLength()-9,',');
			}
			else if(szText->GetLength()>5)
			{
				szText->Insert(szText->GetLength()-4,',');
			}
		}

	}

	else if(m_SpeCount== 3)
	{

		if(result>0)//负数
		{
			if(szText->GetLength()>9)
			{		
				szText->Insert(szText->GetLength()-3,',');
				szText->Insert(szText->GetLength()-7,',');
				szText->Insert(szText->GetLength()-11,',');
			}
			else if(szText->GetLength()>6)
			{
				szText->Insert(szText->GetLength()-3,',');
				szText->Insert(szText->GetLength()-7,',');
			}
			else if(szText->GetLength()>3)
			{
				szText->Insert(szText->GetLength()-3,',');
			}
		}
		else
		{
			if(szText->GetLength()>9)
			{		
				szText->Insert(szText->GetLength()-3,',');
				szText->Insert(szText->GetLength()-7,',');
				szText->Insert(szText->GetLength()-11,',');
			}
			else if(szText->GetLength()>7)
			{
				szText->Insert(szText->GetLength()-3,',');
				szText->Insert(szText->GetLength()-7,',');
			}
			else if(szText->GetLength()>4)
			{
				szText->Insert(szText->GetLength()-3,',');
			}
		}

	}
	else
	{

		if(result>0)//负数
		{
			if(szText->GetLength()>8)
			{
				szText->Insert(szText->GetLength()-4,',');
				szText->Insert(szText->GetLength()-9,',');
			}
			else if(szText->GetLength()>4)
			{
				szText->Insert(szText->GetLength()-4,',');
			}
		}
		else
		{
			if(szText->GetLength()>9)
			{
				szText->Insert(szText->GetLength()-4,',');
				szText->Insert(szText->GetLength()-9,',');
			}
			else if(szText->GetLength()>5)
			{
				szText->Insert(szText->GetLength()-4,',');
			}
		}

	}

}
///获取玩家自己当前的下注总额
///@return 返回玩家当前下注的总额
__int64 CClientPlayView::GetMyAllNote()
{
	__int64 note=0;
	for(int i=0;i<8;i++)
	{
		note+=m_iMyZhu[i];
	}
	return note;
}
///即时设置自己能下注按钮和鼠标
void CClientPlayView::SetXiaZhuBtn()
{
	//if(m_iChooseNote == 0)//当前选择的筹码 0没有，1：100，2：1000，3：1w，4：10w，5：100w,6：500w
	//	return;
	if(m_byMyStation==m_iNowNTstation)
	{
		EnableXiaZhu(false);
		return;
	}
	///20100207zht修改百亿
	__int64 nTempMoney=1; 
	for(int i=0; i<Glb().m_nPowerOfGold; i++)
	{
		nTempMoney*=10;
	}

	__int64 money = GetMyAllNote();
	__int64 nownote = 0;
	switch(m_iChooseNote)
	{
	case 1:
		nownote = 100/nTempMoney;
		break;
	case 2:
		nownote = 1000/nTempMoney;
		break;
	case 3:
		nownote = 10000/nTempMoney;
		break;
	case 4:
		nownote = 100000/nTempMoney;
		break;
	case 5:
		nownote = 1000000/nTempMoney;
		break;
	case 6:
///72把一局
#ifdef SET72 
		/*if(m_bUse500WJeton)
		{
			nownote = 5000000/nTempMoney;
		}
		else
		{*/
			nownote = 10000000/nTempMoney;
		//}
		
#else
		nownote = 5000000/nTempMoney;
#endif
		break;
	}
	if(m_pUserItem[m_byMyStation] != NULL)
	{
		CString str="";
		str.Format("申请上庄处理 下注测试111： m_iMyZhu[8]=%i64d m_iMaxNote=%i64d money=%i64d  nownote=%i64d  st=%d nt=%i64d",m_iMyZhu[8],m_iMaxNote,m_iMaxNote-m_iMyZhu[8],nownote,m_byMyStation,m_iNowNTstation);
		OutputDebugString(str);
		if(m_byMyStation!=m_iNowNTstation && m_iNowNTstation>=0)
			EnableXiaZhu(true);
	}

	UpdateViewFace(NULL);
}

#ifdef VIDEO
///将此昵称的玩家视频窗口矩形写入参数rect,并返回TRUE;没找到返回falst
bool CClientPlayView::GetVideoRect(CString  strNickname, RECT& rect, CString& pic)
{
	CString strLocalName = _T("");
	CString logopathstr = _T("");
	for (int iCount=0; iCount<MAX_PEOPLE; ++iCount)
	{
		if (m_pUserItem[iCount])
		{    
			strLocalName = m_pUserItem[iCount]->GameUserInfo.nickName;
			if(strLocalName.Left(MAX_VIDEO_NICKNAME_LEN) == strNickname)
			{
				rect=m_rectVideoFrame[GetViewSeat(m_pUserItem[iCount]->GameUserInfo.bDeskStation)];
				logopathstr.Format("image\\log\\gamelog%d.bmp", m_pUserItem[iCount]->GameUserInfo.bLogoID);
				CString  tempstr = CBcfFile::GetAppPath()+logopathstr;
				pic = tempstr;
				return true;
			}
		}
	}
	return false; //没找到
}
#endif


///提示开牌图片
void CClientPlayView::DrawPromptOpenCard(CDC * pDC, int x, int y)
{
	if (NULL == pDC)
	{
		return ;
	}

	CGameImageHelper imgPromptOpenCard(&m_imgPromptOpenCard);
	CDC dc;
	dc.CreateCompatibleDC(pDC);
	CBitmap *pOldBmp = (CBitmap*)dc.SelectObject(imgPromptOpenCard);
	if(m_bShowPromptOpenCard)
	{
		::TransparentBlt(pDC->GetSafeHdc(),m_pDitu.x + 5,m_pDitu.y + 100,imgPromptOpenCard.GetWidth(),imgPromptOpenCard.GetHeight(),
			dc.GetSafeHdc(),0,0,imgPromptOpenCard.GetWidth(),imgPromptOpenCard.GetHeight(),dc.GetPixel(0,0));
	}
	dc.SelectObject(pOldBmp);
	dc.DeleteDC();
}

///提示换庄图片
void CClientPlayView::DrawPromptChangeNt(CDC * pDC, int x, int y)
{
	if (NULL == pDC)
	{
		return ;
	}

	CGameImageHelper imgPromptChangeNt(&m_imgPromptChangeNt);
	CDC dc;
	dc.CreateCompatibleDC(pDC);
	CBitmap *pOldBmp = (CBitmap*)dc.SelectObject(imgPromptChangeNt);
	if(m_bShowPromptChangeNt)
	{
		::TransparentBlt(pDC->GetSafeHdc(),m_pDitu.x + 5,m_pDitu.y + 100,imgPromptChangeNt.GetWidth(),imgPromptChangeNt.GetHeight(),
			dc.GetSafeHdc(),0,0,imgPromptChangeNt.GetWidth(),imgPromptChangeNt.GetHeight(),dc.GetPixel(0,0));
	}
	dc.SelectObject(pOldBmp);
	dc.DeleteDC();
}

///设置开牌提示图片
void CClientPlayView::SetPromptOpenCard(bool bShowFlag)
{
	m_bShowPromptOpenCard = bShowFlag;
	UpdateViewFace(NULL);
}

///设置换庄提示图片
void CClientPlayView::SetPromptChangeNt(bool bShowFlag)
{
	m_bShowPromptChangeNt = bShowFlag;
	UpdateViewFace(NULL);
}


///显示隐藏超级客户端
void CClientPlayView::OnBtnClickedSuper(void)
{
//m_LuziDlg
	if(m_SuperDlg.IsWindowVisible())
	{
		m_SuperDlg.ShowWindow(false);
	}
	else 
	{
		m_SuperDlg.ShowWindow(true);
	}
}
//获取所有玩家的昵称
void CClientPlayView::GetAllUserNicheng()
{
	for(int i=0;i<PLAY_COUNT;++i)
	{
		if(m_pUserItem[i] == NULL)
			continue;
		memcpy(m_SuperData.sUserInfo[i].cUserName,m_pUserItem[i]->GameUserInfo.nickName,sizeof(m_pUserItem[i]->GameUserInfo.nickName));
	}
}
///界面上的钱和真实的钱转化
__int64 CClientPlayView::ChangeMoneyByPower(__int64  nMoney , bool  bUIMoney) 
{
	__int64  nTempMoney = nMoney ; 

	__int64   nPower = 1 ;

	for(int i = 0 ; i < abs(m_nPowerOfGold) ; i++ )
	{
		nPower *= 10 ;
	}

	if(m_nPowerOfGold >=0)
	{
		if(bUIMoney)
		{
			nTempMoney*=nPower ; 
		}
		else
		{
			nTempMoney /=nPower ;  
		}
	}
	else
	{
		if(bUIMoney)
		{
			nTempMoney/=nPower ; 
		}
		else
		{
			nTempMoney *=nPower ;  
		}
	}

	return nTempMoney ; 
}
///转化字符串
void  CClientPlayView::ChangeNumToCStringByPower(__int64 nMoney ,bool bHaveSpace ,TCHAR *chSpace, CString &strRet)
{
	char szChar[MAX_PATH] = {0} ; 
	GlbGetNumString(szChar ,nMoney ,m_nPowerOfGold ,bHaveSpace ,chSpace) ;
	strRet.Format("%s" , szChar) ; 

	return ; 
}

LRESULT CClientPlayView::OnShowRobotChip(WPARAM wParam, LPARAM lParam)
{
	 m_bHideAIMoney = (int) wParam;
	// UpdateViewFace(NULL);
	return 0;
}