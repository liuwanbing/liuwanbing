#pragma once
#ifndef DESKFRAMEVIEW_HEAD_H
#define DESKFRAMEVIEW_HEAD_H
#include "GamePlace.h"
class CGameRoomEx;
//#include "PersonalInfo.h"

#include <string>
using namespace std;

#define		IDR_CLICKDESK					1							//点击桌子计时器
#define		TIME_QUEUE						2							//点击桌子计时器

//宏定义
#define		YH_DESK_FACE					1							//远航桌子
#define		EX_DESK_FACE					2							//扩展桌子
#define		BJL_DESK_FACE					3							//百家乐桌子 add by wlr 20090713
#define		QUEUE_DESK_FACE					4							//排队机房间界面

#define		HUNDRED_PEOPLE_DESK_GAME_NAME   10301800					//百家乐桌游戏编号 add by ramon
#define		SUOHA_PEOPLE_DESK_GAME_NAME		30501800					//百变骰子梭哈ID
#define		NIUNIU_PEOPLE_DESK_GAME_NAME	10901800					//百变牛牛ID
#define		ERZHANGPAI_PEOPLE_DESK_GAME_NAME	11901800				//百变二张牌ID

#define		WM_LEFT_MOUSE_HIT_DESK			WM_USER+214					//按动游戏桌子
#define		WM_RIGHT_MOUSE_HIT_DESK			WM_USER+215					//按动游戏桌子
#define		WM_HIT_DESK_BEGIN				WM_USER+216					//按开始

#define		WM_HIT_BJL_BEGIN				WM_USER+217					//百家乐按开始 add by wlr 20090713

#define		WM_HIT_QUEUE_STOP				WM_USER+218					//百家乐按开始 add by wlr 20090713

template	<UINT uPeopleCount, BYTE bDeskType> class CDesk;

typedef		CDesk<1,YH_DESK_FACE>	CDesk1;
typedef		CDesk<2,YH_DESK_FACE>	CDesk2;
typedef		CDesk<3,YH_DESK_FACE>	CDesk3;
typedef		CDesk<4,YH_DESK_FACE>	CDesk4;
typedef		CDesk<5,YH_DESK_FACE>	CDesk5;
typedef		CDesk<6,YH_DESK_FACE>	CDesk6;
typedef		CDesk<7,YH_DESK_FACE>	CDesk7;
typedef		CDesk<8,YH_DESK_FACE>	CDesk8;
typedef		CDesk<16,YH_DESK_FACE>	CDesk16;

typedef		CDesk<2,EX_DESK_FACE>	CDeskEx2;
typedef		CDesk<3,EX_DESK_FACE>	CDeskEx3;
typedef		CDesk<4,EX_DESK_FACE>	CDeskEx4;
typedef		CDesk<5,EX_DESK_FACE>	CDeskEx5;
typedef		CDesk<6,EX_DESK_FACE>	CDeskEx6;
typedef		CDesk<7,EX_DESK_FACE>	CDeskEx7;
typedef		CDesk<8,EX_DESK_FACE>	CDeskEx8;
typedef		CDesk<16,EX_DESK_FACE>	CDeskEx16;

typedef		CDesk<180,BJL_DESK_FACE>	CDeskBJL;//百家乐 add by wlr 20090713

class CDeskFrameView;

//Avatar系统相关
#define BzGetAvatarIndex(obj) \
	GetAvatarIndexInRoom((obj)->GameUserInfo.bLogoID,(obj)->GameUserInfo.i64Bank,(obj)->GameUserInfo.i64Money,\
	(obj)->GameUserInfo.dwPoint,m_uGameNameID,m_uComType)

/*******************************************************************************************************/

//比赛用户信息结构
struct MatchUserInfo
{
	long int					dwUserID;								//用户 ID
	TCHAR						szUserName[61];							//用户名字
};

/*******************************************************************************************************/

//游戏桌基础类
class CBaseDeskView
{
	//友元定义
	friend class CDeskFrameView;

	//变量定义
protected:
	bool						m_bLock;								//是否锁
	bool						m_bPlayGame;							//是否游戏
	bool						m_bDownMouse;							//是否按下鼠标
	bool						m_bFocusFrame;							//焦点框架
	bool						m_bBasePoint;							//是否设置倍数							
	int							m_iBasePoint;							//倍数大小
	BYTE						m_bDeskIndex;							//游戏桌子索引
	BYTE						m_bHoverIndex;							//框架索引
	BYTE						m_bDeskStation;							//座位索引
	UserItemStruct				* m_pUserItem[MAX_PEOPLE];				//用户资料

	Font						* m_pFont;								// 用来显示文字

	bool					m_bCanInput;								//接受输入 


	//比赛信息
protected:
	MatchUserInfo				m_MatchInfo[MAX_PEOPLE];				//比赛信息

public:

	CString						m_deskClass;							//桌子图片目录
	bool						m_deskFlat;								//人物头像是否是扩展的平面的，当m_deskClass=""时，强制=1

	UINT						m_uComType;								//游戏类型
	static UINT					m_uGameNameID;							//游戏ID
	//函数定义
public:
	//构造函数
	CBaseDeskView();
	//析构函数
	virtual ~CBaseDeskView();

	//功能函数
public:

	//按键测试
	virtual BYTE MouseHitTest(POINT MousePoint)=0;	
	virtual bool DrawViewFacePng(Graphics &graphics, const CRect &DeskRect, Image *pAgree, Image *pLock, Image *pFrame, Image *pWaitSit, bool bIsVip = false)=0;

	//功能函数
public:
	//初始化函数
	static bool InitDesk(CString deskClass,bool deskFloat,UINT uNameID, bool bIsVip = false);
	//设置游戏桌子索引
	void SetDeskIndex(BYTE bDeskIndex) { m_bDeskIndex=bDeskIndex; };
	void SetDeskClass(CString deskClass,bool deskFlat){m_deskClass=deskClass;m_deskFlat=deskFlat;};
	//设置游戏类型
	void SetComType(int uComType) { m_uComType=uComType; };
	//加载默认游戏桌
	static bool LoadDeskImage(BYTE bEveryCount, CGameImage & AFCDeskImage, BYTE bDeskType,CString deskClass,bool deskFlat,UINT uNameID);

	//功能函数 
public:
	//是否接受输入
	void  SetCanInput(bool bCanInput) {m_bCanInput = bCanInput;};
	//是否游戏
	bool IsPlayGame(void) { return m_bPlayGame; };
	//是否人都同意
	virtual bool IsAllAgree(void);
	//同意人数
	virtual BYTE CountAgree(void);
	//是否设置密码
	bool IsSetPass(void) { return m_bLock; }
	//得到桌子倍数
	int GetBasePoint(void) { return m_iBasePoint;}
	//设置框架
	void SetShowFrame(bool bFocusFrame, CDeskFrameView * pDeskFrame);
	//设置游戏桌子倍数
	void SetBasePoint(bool bSetBasePoint, int iBasePoint, CDeskFrameView * pDeskFrame);
	//设置盘旋
	void SetHoverIndex(BYTE bHoverIndex, CDeskFrameView * pDeskFrame);
	//设置按下
	void SetMouseDown(bool bMouseDown, CDeskFrameView * pDeskFrame);
	//设置锁标志
	void SetLock(bool bLock, CDeskFrameView * pDeskFrame);
	//设置开始标志
	void SetPlayGame(bool bPlaying, CDeskFrameView * pDeskFrame);
	//设置用户信息
	bool SetUserItem(BYTE bDeskStation, UserItemStruct * pUserItem, CDeskFrameView * pDeskFrame);
	//设置比赛信息
	bool SetMatchInfo(BYTE bDeskStation, long int dwUserID, TCHAR * szUserName, CDeskFrameView * pDeskFrame);
	//获取用户信息
	UserItemStruct * GetUserItem(BYTE bDeskStation);
	//获取比赛信息
	long int GetMatchInfo(BYTE bDeskStation);
};

/*******************************************************************************************************/

//游戏桌类
template <UINT uPeopleCount, BYTE bDeskType> class CDesk : public CBaseDeskView
{
	//变量定义:
public:
	static CPoint				m_PtLock;								//锁位置
	static CPoint				m_PtBegin;								//开始位置
	static CRect				m_IndexRect;							//游戏桌号码
	static CPoint				m_PtBasePoint;							//桌子倍数位置
	static CPoint				m_PtAgree[uPeopleCount];				//同意位置
	static CRect				m_NameRect[uPeopleCount];				//名字框架
	static CRect				m_ChairRect[uPeopleCount];				//桌子位置
	static UINT					m_TextFormat[uPeopleCount];				//格式参数
	static CPoint				m_PtLog[uPeopleCount];					//人物位置
	static CPoint				m_BtnAgree[uPeopleCount];				//同意位置
	static int					m_draw[uPeopleCount];					//显示顺序
	static Color				m_UserNameColor;					///< 玩家名
	static Color				m_VipNameColor;						///< VIP玩家名
	static Color				m_DeskNoColor;						///< 桌号字体颜色
	static Color				m_FocusColor;						///< 焦点框颜色

	//函数定义
public:
	//构造函数
	CDesk(void) {};
	//析构函数
	virtual ~CDesk(void) {};

	//重载函数
public:
	//按键测试
	virtual BYTE MouseHitTest(POINT MousePoint)
	{
		if (m_deskClass == "queue" && !m_bCanInput) return 255;
		for (UINT i=0;i<uPeopleCount;i++)
		{
			if (m_ChairRect[i].PtInRect(MousePoint)) return i;
		}
		return 255;
	};
	virtual bool IsAllAgree(void)
	{
		for (UINT i=0;i<uPeopleCount;i++)///当前桌子，单个桌子的人的循环
		{
			if (m_pUserItem[i]!=NULL)
			{
				if(m_pUserItem[i]->GameUserInfo.bUserState!=USER_DESK_AGREE&&
					m_pUserItem[i]->GameUserInfo.bUserState!=USER_ARGEE&&
					m_pUserItem[i]->GameUserInfo.bUserState!=USER_CUT_GAME&&
					m_pUserItem[i]->GameUserInfo.bUserState!=USER_PLAY_GAME)
					return false;
			}else return false;
		}
		return true;
	};
	//同意人数
	virtual BYTE CountAgree(void)
	{
		int count = 0;
		for (UINT i=0;i<uPeopleCount;i++)///当前桌子，单个桌子的人的循环
		{
			if (m_pUserItem[i]!=NULL)
			{
				if(m_pUserItem[i]->GameUserInfo.bUserState == USER_DESK_AGREE)
					count ++;
			}
		}
		return count;
	};

	/// 画桌子上的人像，同意标志，锁，玩家昵称等属于桌子的内容
	virtual bool DrawViewFacePng(Graphics &graphics, const CRect &DeskRect, Image *pAgree, Image *pLock, Image *pFrame, Image *pWaitSit, bool bIsVip = false)
	{
		bool isneedpeople=false;
		StringFormat sf;
		sf.SetTrimming(StringTrimmingEllipsisWord);
		sf.SetAlignment(StringAlignmentCenter);
		SolidBrush brush(Color(255,0,0));
		RectF rcF;

	/*	if (uPeopleCount>3)
		{
			if (m_pUserItem[3]!=NULL)
			{
				UINT ChairIndex=3; 
				int iSitGrade = 0;
				iSitGrade = BzGetAvatarIndex(m_pUserItem[3]);

				TCHAR   szLogoPath[MAX_PATH]={0};//图片文件名

				TCHAR   szLogoBKPath[MAX_PATH]={0};//图片文件名
				//CString strLogoPath;
				BOOL    bFileExist=false;//文件是否存在，缺省为不存在
				CFileFind fileFind;	//查找文件
				int     uLogoID=m_pUserItem[3]->GameUserInfo.bLogoID;//为下面的代码省写

				if (m_deskClass == "2d" || m_deskClass == "queue")
				{
					wsprintf(szLogoPath,"image\\log\\%s\\gamelog%d_small.png",m_pUserItem[3]->GameUserInfo.bBoy?"boy":"girl",iSitGrade);
					wsprintf(szLogoBKPath,"image\\log\\LogoBk.png");
				}
				else
				{
					switch(uPeopleCount )
					{
					case 2:
						{
							if (bIsVip)
							{
								wsprintf(szLogoPath,"image\\log\\%s\\sitelog\\vip\\%s_%d.png",m_deskClass,m_pUserItem[3]->GameUserInfo.bBoy?"boy":"girl",ChairIndex);
							}
							else
							{
								wsprintf(szLogoPath,"image\\log\\%s\\sitelog2\\%s%d\\desk%dsit.png",m_deskClass,m_pUserItem[3]->GameUserInfo.bBoy?"boy":"girl",iSitGrade,ChairIndex);
							}
							break;
						}
					case 5:
						{
							wsprintf(szLogoPath,"image\\log\\%s\\sitelog5\\%s%d\\desk%dsit.png",m_deskClass,m_pUserItem[3]->GameUserInfo.bBoy?"boy":"girl",iSitGrade,ChairIndex);
							break;
						}
					case 6:
						{
							wsprintf(szLogoPath,"image\\log\\%s\\sitelog6\\%s%d\\desk%dsit.png",m_deskClass,m_pUserItem[3]->GameUserInfo.bBoy?"boy":"girl",iSitGrade,ChairIndex);
							break;
						}
					case 8:
						{
							wsprintf(szLogoPath,"image\\log\\%s\\sitelog8\\%s%d\\desk%dsit.png",m_deskClass,m_pUserItem[3]->GameUserInfo.bBoy?"boy":"girl",iSitGrade,ChairIndex);
							break;
						}
					default:
						{
							wsprintf(szLogoPath,"image\\log\\%s\\sitelog\\%s%d\\desk%dsit.png",m_deskClass,m_pUserItem[3]->GameUserInfo.bBoy?"boy":"girl",iSitGrade,ChairIndex);
							break;
						}
					}
				}
				

				
				Image *iLogo;
				Image *iLogoBk;
#ifdef BZ_ZLIB 

				CUnZipRes unZipRes;
				IStream * pStream=NULL;
				
				pStream=unZipRes.LoadFileResFromZip(szLogoPath);
				iLogo=new Image(pStream);
				if(pStream!=NULL)
				{
					pStream->Release();
					pStream=NULL;
				}

				pStream=unZipRes.LoadFileResFromZip(szLogoBKPath);
				iLogoBk=new Image(pStream);
				if(pStream!=NULL)
				{
					pStream->Release();
					pStream=NULL;
				}
#else
				iLogo=Image::FromFile(CA2W(szLogoPath));
				iLogoBk=Image::FromFile(CA2W(szLogoBKPath));
#endif

				int x = m_PtLog[3].x;
				int y = m_PtLog[3].y;

				if (m_deskClass == "queue")
				{
					graphics.DrawImage(iLogoBk, DeskRect.left + x, DeskRect.top + y, iLogoBk->GetWidth(),iLogoBk->GetHeight());
					int _x = (iLogoBk->GetWidth() - iLogo->GetWidth()) / 2;
					int _y = (iLogoBk->GetHeight() - iLogo->GetHeight()) / 2;
					graphics.DrawImage(iLogo, DeskRect.left + x + _x, DeskRect.top + y + _y, iLogo->GetWidth(),iLogo->GetHeight());
				}
				else
				{
					graphics.DrawImage(iLogo, DeskRect.left + x, DeskRect.top + y, iLogo->GetWidth(),iLogo->GetHeight());
				}

				delete iLogo;
				delete iLogoBk;
			}
		}
		*/
		int peopleCnt = 0; ///< 查看一张桌子有多少个人，如果没有人则不会画“锁”的图像

		for (UINT i=0;i<uPeopleCount;i++)///当前桌子，单个桌子的人的循环
		{
			int j=i;//m_draw[i];
			//绘画用户信息
			if (m_pUserItem[j]!=NULL)
			{
				isneedpeople=true;
				UINT ChairIndex=j; 
				if (ChairIndex==1&&uPeopleCount==2)
				{//2人
					ChairIndex=2;
				}
				
				peopleCnt++;
				//WCHAR	m_wszBuffer[100]={0};
				int iSitGrade = 0;
				//iSitGrade = GetMoneyGrade(m_pUserItem[j]->GameUserInfo.i64Money);
				iSitGrade = BzGetAvatarIndex(m_pUserItem[j]);

				TCHAR   szLogoPath[MAX_PATH]={0};//图片文件名
				//CString strLogoPath;
				BOOL    bFileExist=false;//文件是否存在，缺省为不存在
				CFileFind fileFind;	//查找文件
				int     uLogoID=m_pUserItem[j]->GameUserInfo.bLogoID;//为下面的代码省写			
				wsprintf(szLogoPath,"image\\log\\DeskLogos\\Logo_Small_%d.png",uLogoID);
			/*	CString str;
				str.Format("cq::----用户:%d ,LOGOID=%d", m_pUserItem[j]->GameUserInfo.dwUserID, uLogoID);
				OutputDebugString(str);*/
			//	if (j != 3 )
				{
					Image *iLogo;

#ifdef BZ_ZLIB 

					CUnZipRes unZipRes;
					IStream * pStream=NULL;
					
					pStream=unZipRes.LoadFileResFromZip(szLogoPath);
					iLogo=new Image(pStream);
					if(pStream!=NULL)
					{
						pStream->Release();
						pStream=NULL;
					}
#else
					iLogo=Image::FromFile(CA2W(szLogoPath));					
#endif

					int x = m_PtLog[j].x;
					int y = m_PtLog[j].y;

					graphics.DrawImage(iLogo, DeskRect.left + x, DeskRect.top + y, iLogo->GetWidth(),iLogo->GetHeight());
					delete iLogo;
				}
				rcF.X = m_NameRect[j].left + DeskRect.left;
				rcF.Y = m_NameRect[j].top + DeskRect.top;
				rcF.Width = m_NameRect[j].right - m_NameRect[j].left;
				rcF.Height = m_NameRect[j].bottom - m_NameRect[j].top;
				brush.SetColor(GetNameColor(&m_pUserItem[j]->GameUserInfo));
				graphics.DrawString(CA2W(m_pUserItem[j]->GameUserInfo.nickName), wcslen(CA2W(m_pUserItem[j]->GameUserInfo.nickName)),
					m_pFont, rcF, &sf, &brush);
			}
		}
		if (m_bDeskStation!=255 && !m_bDownMouse)
		{
			int x = m_PtLog[m_bDeskStation].x;
			int y = m_PtLog[m_bDeskStation].y;
			graphics.DrawImage(pWaitSit, DeskRect.left + x-3, DeskRect.top + y-3, pWaitSit->GetWidth(),pWaitSit->GetHeight());
		}
		
		//绘画号码,桌子的号码m_bDeskIndex
		TCHAR szBuffer[20];
		memset(szBuffer,0,sizeof(szBuffer));
		wsprintf(szBuffer,TEXT("- %d -"),m_bDeskIndex+1);
		brush.SetColor(m_DeskNoColor);


		//绘画同意标志pAgree
		if (bDeskType==YH_DESK_FACE)
		{
			if (m_bPlayGame==false)	// 已经开始就不用画同意标志了
			{
				for (UINT i=0;i<uPeopleCount;i++)
				{
					if ((m_pUserItem[i]!=NULL)&&(pAgree!=NULL)&&
						(m_pUserItem[i]->GameUserInfo.bUserState==USER_ARGEE||m_pUserItem[i]->GameUserInfo.bUserState==USER_DESK_AGREE))
					{
						//绘画透明图
						graphics.DrawImage(pAgree, DeskRect.left + m_PtAgree[i].x, DeskRect.top + m_PtAgree[i].y, pAgree->GetWidth(), pAgree->GetHeight());
					}
				}
			}
		}

		rcF.X = m_IndexRect.left + DeskRect.left;
		rcF.Y = m_IndexRect.top + DeskRect.top;
		rcF.Width = m_IndexRect.right - m_IndexRect.left;
		rcF.Height = m_IndexRect.bottom - m_IndexRect.top;

		if (m_deskClass != "queue")
		{
			graphics.DrawString(CA2W(szBuffer), lstrlen(szBuffer),
				m_pFont, rcF, &sf, &brush);
		}

		if (0 == peopleCnt)
			m_bLock = false;
		//绘画锁m_bLock 
		if ((bDeskType==YH_DESK_FACE) && (m_bLock==true) && (pLock!=NULL))
		{
			graphics.DrawImage(pLock,
				DeskRect.left + m_PtLock.x,
				DeskRect.top + m_PtLock.y,
				pLock->GetWidth(),
				pLock->GetHeight());
		}
		//绘画焦点框
		if (m_bFocusFrame&&m_uComType!=TY_MATCH_GAME)
		{
			graphics.DrawImage(pFrame, DeskRect.left, DeskRect.top, DeskRect.Width(), DeskRect.Height()+4);
		}
		return true;
	}
	Color GetNameColor(UserInfoStruct *pUserInfo)
	{
		CString stime;
		stime.Format("%d",CTime::GetCurrentTime());        //当前的时间，String格式
		int curtime=atoi(stime);                        //转换为int
		if(pUserInfo->iVipTime>curtime)
			return m_VipNameColor;
		return m_UserNameColor;
	}
};

/*******************************************************************************************************/

//游戏桌子框架类
/*

*/
class CDeskFrameView : public CWnd
{
	Graphics						*m_pGraph;
	Bitmap							*m_pBackBmp;

	//变量定义
protected:
	//{{透明底图
	Image	*m_pImgFrame;
	Image	*m_pImgWaitSit;
	Image	*m_pImgDesk;
	Image	*m_pImgLock;
	Image	*m_pImgTableBasePoint;
	Image	*m_pImgBegin;
	Image	*m_pImgAgree;
	BYTE	m_byDeskType;	/// 桌子类型，是不是百家乐类型
	int		m_r1, m_g1, m_b1;	// 渐变色的第一种
	int		m_r2, m_g2, m_b2;	// 渐变色的第二种
	//}}

	CToolTipCtrl				m_Tooltip;								//提示工具条
	UINT						m_uClassSize;							//类大小
	BYTE						m_bDeskCount;							//游戏桌数目
	BYTE						m_bEveryCount;							//每桌游戏人数
	HCURSOR						m_hMouseCursor;							//光标句柄
	CGameImage					m_DeskImage;							//游戏桌底图
	CGameImage					m_LockImage;							//游戏桌锁图
	CGameImage					m_TableBasePoint;						//游戏桌基础分图
	CGameImage					m_BeginImage;							//游戏开始图
	CGameImage					m_AgreeImage;							//游戏同意图
	CBaseDeskView				* m_pDeskArray;							//游戏桌数组指针
	CNormalBitmapButtonEx		m_BeginAgree;							//同意游戏按钮
	CNormalBitmapButtonEx		m_BJLBegin;								//百家乐开始游戏按钮 add by wlr 20090713
	CNormalBitmapButtonEx		m_BJLPlaying;							//百家乐游戏进行中按钮 add by wlr 20090713

//排队机专用 
	CNormalBitmapButtonEx		m_QueueBegin;								//排队机排队
	CNormalBitmapButtonEx		m_QueueStop;								//排队机离开
	bool						m_bQueueBegin;

	Image*						m_pImageQueueTip;
	Image*						m_pImageQueueTip1;			
	Image*						m_pImageQueueTip2;
	int							m_XTip, m_YTip;
	int							m_XTip1, m_YTip1;
	int							m_XDesk, m_YDesk;
	Image*						m_pImageQueueTime;
	Image*						m_pImageQueueplan;
	int							m_SumTime;
	int							m_CurTime;



	COLORREF					clrSitDeskFrameLeftTop;					//桌子上有人时外框左边和上边颜色
	COLORREF					clrSitDeskFrameRightBottom;				//桌子上有人时外框右边和下边颜色

	int							m_BtnAgree0X,m_BtnAgree0Y;				//同意按钮位置
	int							m_BtnAgree1X,m_BtnAgree1Y;				//同意按钮位置
	int							m_BtnAgree2X,m_BtnAgree2Y;				//同意按钮位置
	int							m_BtnAgree3X,m_BtnAgree3Y;				//同意按钮位置

	//状态信息
protected:
	bool						m_bLeftMouseDown;						//是否鼠标左键按下
	bool						m_bRightMouseDown;						//是否鼠标右键按下
	BYTE						m_bDownDeskIndex;						//按下桌子索引
	BYTE						m_bDownDeskStation;						//按下位置索引

	BYTE                        m_bCurDownDesk;
	BYTE                        m_bCurDownStation;

	BYTE						m_bUserFrameIndex;						//用户边框索引
	BYTE						m_bFindFrameIndex;						//查找边框索引
	int							m_uComType;								//游戏类型
	bool						m_bIsClick;								//限制点击桌子的频率
	CString						m_deskClass;							//桌子类型
	bool						m_deskFlat;
	long						m_UserID;								//自己的用户ID
	CPersonalInfo				m_personalInfo;							//显示坐下用户信息对话框
public:
	GetOrderName				* pOrderName;							//对应房间等级函数指针

	bool						m_bShowPlaying;							//是否显示游戏进行中按钮 百家乐 add by wlr 20090717

    // PengJiLin, 2011-6-23, 激活消息
    BOOL        m_bActive;

	//滚动信息
protected:
	bool				m_bIsVip;									///< 是否VIP房间
	int							m_iPos;									//当前位置
	int							m_iMaxPos;								//最大位置
	int							m_iXBasePos;							//X 基础点
	int							m_iYBasePos;							//Y 基础点
	int							m_iPicWidth;							//图像宽度
	int							m_iPicHight;							//图像高度
	int							m_iRowCount;							//行桌子数目
	int							m_iAllLineCount;						//桌子总行数
	int							m_iScreenLineCount;						//屏桌子行数
	static int					m_iWhellTimes;							//滚轮倍数
	static int					m_iScrollPos;							//滚动象素

	GameInfoStruct				*pGameInfo;
	CGameRoomEx					*m_pRoom;
	
	CDC m_dcMem;
	HBITMAP* m_bmp;

	//函数定义
public:
	//构造函数
	CDeskFrameView();
	//析构函数
	virtual ~CDeskFrameView();

	//消息映射
protected:
	DECLARE_MESSAGE_MAP()

	//辅助函数
protected:
	//按键测试
	BYTE MouseHitTest(POINT point);
	//取消初始化
	bool UnInitDeskFrame();

	// 分别用于新旧类型，Bmp图片和Png图片
	void OnPaintPng();
	void OnPaintBmp();


	void PaintContestInfo();

	//功能函数
public:
	//得到自己用户 ID
	bool GetMeUserID(long MeUserID);
	//初始化函数
	bool InitDeskFramePng(BYTE bDeskCount, BYTE bEveryCount,
		const wchar_t * pFrameImage,const wchar_t * pWaitSitImage, const wchar_t * pDeskImage, const wchar_t * pLockImage, const wchar_t* pBasePoint, const wchar_t * pBeginImage, const wchar_t * pAgreeImage,
		BYTE bDeskType,int uComType,GameInfoStruct *GameInfo,CGameRoomEx* pRoom,CString deskClass,bool deskFlat,COLORREF clrSitDeskFrameLeftTop,COLORREF clrSitDeskFrameRightBottom
		, bool bIsVip = false);	
	//获取游戏人数
	BYTE GetEveryDeskCount() { return m_bEveryCount; }
	//是否设置密码
	bool IsSetPass(BYTE bDeskIndex);
	//获取游戏桌指针
	CBaseDeskView * GetDeskArrayPtr(UINT uIndex);	
	//获取比赛信息
	long int GetMatchInfo(BYTE bDeskIndex, BYTE bDeskStation);
	//设置视图桌子
	void SetViewDesk(BYTE bDeskIndex);
	//设置游戏桌子倍数图片
	void SetBasePoint(BYTE bDeskIndex,int iBasePoint,bool bSet);
	//设置桌子接受输入
	void SetCanInput(BYTE bDeskIndex, bool bCanInput);
	//建立函数
	bool CreateDeskFrame(CWnd * pParentWnd, UINT uWndID);
	//获取游戏桌数目
	BYTE GetDeskCount() { return m_bDeskCount; }
	//设置比赛信息
	bool SetMatchInfo(BYTE bDeskIndex, BYTE bDeskStation, long int dwUserID, TCHAR * szUserName);
	//设置开始标志
	void SetPlayGame(BYTE bDeskIndex, bool bPlaying);
	//更新游戏桌					
	void UpdateDeskView(BYTE bDeskIndex);
	void	OnHitpagedown();//上翻
	void	OnHitpageup();	//下翻
	//单击开始按钮
	void    OnHitBegin();	//
	//显示开始游戏按钮
	void ShowBeginBtn(BYTE bDeskIndex,BYTE bDeskStation,bool bShow);

	//add by wlr 20090713
	// 单击百家乐开始按钮
	void	OnHitBJLBegin();
	//显示百家乐开始按钮 
	void ShowBJLBeginBtn();
	//显示百家乐开始按钮还是游戏进行中按钮
	void showBJLBeginOrPlayingBtn(bool bShowPlaying = false); 
	//end of add by wlr 20090713
		
	//排队机显示排队和离开
	void showQueueBtn(bool bShowPlaying = false); 

	//排队机开始
	void OnHitQueueBegin();
	//排队机停止
	void OnHitQueueStop();
	//排除机显示时间
	void ShowQueueTime(int x, int y,  int inumber);
	//排除机显示进度
	void ShowQueuePlan(int x, int y, int iPlan);
	//排除机设置时间
	void SetQueueTime(int iTime, bool setType = true);



	//功能函数
public:
	//是否游戏
	bool IsPlayGame(BYTE bDeskIndex);
	//是否全部开始同意
	bool IsAllAgree(BYTE bDeskIndex);
	//大厅同意人员数量
	BYTE CountAgreePeople(BYTE bDeskIndex);
	//设置锁标志
	void SetLock(BYTE bDeskIndex, bool bLock);
	//获取用户信息
	UserItemStruct * GetUserItem(BYTE bDeskIndex, BYTE bDeskStation);
	//设置用户边框
	void SetUserFrame(BYTE bDeskIndex);
	//设置查找边框
	void SetFindFrame(BYTE bDeskIndex);
	//设置用户信息
	bool SetUserItem(BYTE bDeskIndex, BYTE bDeskStation, UserItemStruct * pUserItem);

	//消息函数
public:
	//重画函数
	afx_msg void OnPaint();
	//位置消息
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//滚动条信息
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar * pScrollBar);
	//鼠标滚轮消息
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint MousePoint);
	//鼠标右键按下消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//键盘按下
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//鼠标右键按起消息
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//鼠标移动消息
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//设置光标
	afx_msg BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message);
	//右键按下消息
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//右键按起消息
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//查找函数
public:
	//获取空位置数目
	BYTE GetNullCount(BYTE bDeskIndex);
	//查找位置
	BYTE FindGameDesk(BYTE bLastFind, bool bOneNull, bool bTwoNull, bool bNoAllNull, bool bAllNull);
	//获取空位置
	BYTE GetNullStation(BYTE bDeskIndex);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//加载
	virtual void LoadBtnAgree(CString deskClass);
	//wushuqun 2009.7.13
	//修改游戏房间中退出游戏时闪屏问题
	afx_msg BOOL OnEraseBkgnd(CDC* pDC)
	{
		return true;
		//
		return CWnd::OnEraseBkgnd(pDC);
	}
};

#endif
/*******************************************************************************************************/