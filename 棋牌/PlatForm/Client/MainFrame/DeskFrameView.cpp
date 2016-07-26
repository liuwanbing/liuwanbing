//桌子框架视图
#include "StdAfx.h"
#include "GamePlace.h"
#include "DeskFrameView.h"
#include "GameRoomEx.h"

#define IDC_BJLBEGIN	11//百家乐 add by wlr 20090713
#define IDC_BJLPLAYING  12//百家乐 add by wlr 20090713

#define IDC_QUEUEBEGIN	13//百家乐 add by wlr 20090713
#define IDC_QUEUESTOP  14//百家乐 add by wlr 20090713

extern void DebugPrintf(const char *p, ...);

/////新加的桌子，单人游戏
CPoint				CDesk1::m_PtLock;								//锁位置
CPoint				CDesk1::m_PtBasePoint;							//桌子倍数
CPoint				CDesk1::m_PtBegin;								//开始位置//正在在游戏的桌面
CRect				CDesk1::m_IndexRect;							//游戏桌号码
CPoint				CDesk1::m_PtAgree[1];							//同意位置
CRect				CDesk1::m_NameRect[1];							//名字框架
CRect				CDesk1::m_ChairRect[1];							//桌子位置
UINT				CDesk1::m_TextFormat[1];						//格式参数
CPoint				CDesk1::m_PtLog[1];								//人物位置
CPoint				CDesk1::m_BtnAgree[1];								//同意位置
int					CDesk1::m_draw[1];								//显示顺序
/// 颜色值
Color				CDesk1::m_UserNameColor;					///< 玩家名
Color				CDesk1::m_VipNameColor;						///< VIP玩家名
Color				CDesk1::m_DeskNoColor;						///< 桌号字体颜色
Color				CDesk1::m_FocusColor;						///< 焦点框颜色

CPoint				CDesk2::m_PtLock;								//锁位置
CPoint				CDesk2::m_PtBasePoint;							//桌子倍数
CPoint				CDesk2::m_PtBegin;								//开始位置//正在在游戏的桌面
CRect				CDesk2::m_IndexRect;							//游戏桌号码
CPoint				CDesk2::m_PtAgree[2];							//同意位置
CRect				CDesk2::m_NameRect[2];							//名字框架
CRect				CDesk2::m_ChairRect[2];							//桌子位置
UINT				CDesk2::m_TextFormat[2];						//格式参数
CPoint				CDesk2::m_PtLog[2];								//人物位置
CPoint				CDesk2::m_BtnAgree[2];								//同意位置
int					CDesk2::m_draw[2];								//显示顺序
/// 颜色值
Color				CDesk2::m_UserNameColor;					///< 玩家名
Color				CDesk2::m_VipNameColor;						///< VIP玩家名
Color				CDesk2::m_DeskNoColor;						///< 桌号字体颜色
Color				CDesk2::m_FocusColor;						///< 焦点框颜色
 
CPoint				CDesk3::m_PtLock;								//锁位置
CPoint				CDesk3::m_PtBasePoint;							//桌子倍数
CPoint				CDesk3::m_PtBegin;								//开始位置
CRect				CDesk3::m_IndexRect;							//游戏桌号码
CPoint				CDesk3::m_PtAgree[3];							//同意位置
CRect				CDesk3::m_NameRect[3];							//名字框架
CRect				CDesk3::m_ChairRect[3];							//桌子位置
UINT				CDesk3::m_TextFormat[3];						//格式参数
CPoint				CDesk3::m_PtLog[3];								//人物位置
CPoint				CDesk3::m_BtnAgree[3];								//同意位置
int					CDesk3::m_draw[3];								//显示顺序
/// 颜色值
Color				CDesk3::m_UserNameColor;					///< 玩家名
Color				CDesk3::m_VipNameColor;						///< VIP玩家名
Color				CDesk3::m_DeskNoColor;						///< 桌号字体颜色
Color				CDesk3::m_FocusColor;						///< 焦点框颜色

CPoint				CDesk4::m_PtLock;								//锁位置
CPoint				CDesk5::m_PtBasePoint;							//桌子倍数
CPoint				CDesk4::m_PtBasePoint;							//桌子倍数
CPoint				CDesk4::m_PtBegin;								//开始位置
CRect				CDesk4::m_IndexRect;							//游戏桌号码
CPoint				CDesk4::m_PtAgree[4];							//同意位置
CRect				CDesk4::m_NameRect[4];							//名字框架
CRect				CDesk4::m_ChairRect[4];							//桌子位置
UINT				CDesk4::m_TextFormat[4];						//格式参数
CPoint				CDesk4::m_PtLog[4];								//人物位置
CPoint				CDesk4::m_BtnAgree[4];								//同意位置
int					CDesk4::m_draw[4];								//显示顺序
/// 颜色值
Color				CDesk4::m_UserNameColor;					///< 玩家名
Color				CDesk4::m_VipNameColor;						///< VIP玩家名
Color				CDesk4::m_DeskNoColor;						///< 桌号字体颜色
Color				CDesk4::m_FocusColor;						///< 焦点框颜色

CPoint				CDesk5::m_PtLock;								//锁位置
CPoint				CDesk6::m_PtBasePoint;							//桌子倍数
CPoint				CDesk5::m_PtBegin;								//开始位置
CRect				CDesk5::m_IndexRect;							//游戏桌号码
CPoint				CDesk5::m_PtAgree[5];							//同意位置
CRect				CDesk5::m_NameRect[5];							//名字框架
CRect				CDesk5::m_ChairRect[5];							//桌子位置
UINT				CDesk5::m_TextFormat[5];						//格式参数
CPoint				CDesk5::m_PtLog[5];								//人物位置
CPoint				CDesk5::m_BtnAgree[5];								//同意位置
int					CDesk5::m_draw[5];								//显示顺序
/// 颜色值
Color				CDesk5::m_UserNameColor;					///< 玩家名
Color				CDesk5::m_VipNameColor;						///< VIP玩家名
Color				CDesk5::m_DeskNoColor;						///< 桌号字体颜色
Color				CDesk5::m_FocusColor;						///< 焦点框颜色

CPoint				CDesk6::m_PtLock;								//锁位置
CPoint				CDesk6::m_PtBegin;								//开始位置
CRect				CDesk6::m_IndexRect;							//游戏桌号码
CPoint				CDesk6::m_PtAgree[6];							//同意位置
CRect				CDesk6::m_NameRect[6];							//名字框架
CRect				CDesk6::m_ChairRect[6];							//桌子位置
UINT				CDesk6::m_TextFormat[6];						//格式参数
CPoint				CDesk6::m_PtLog[6];								//人物位置
CPoint				CDesk6::m_BtnAgree[6];								//同意位置
int					CDesk6::m_draw[6];								//显示顺序
/// 颜色值
Color				CDesk6::m_UserNameColor;					///< 玩家名
Color				CDesk6::m_VipNameColor;						///< VIP玩家名
Color				CDesk6::m_DeskNoColor;						///< 桌号字体颜色
Color				CDesk6::m_FocusColor;						///< 焦点框颜色

CPoint				CDesk8::m_PtLock;								//锁位置							//8人桌    start
CPoint				CDesk8::m_PtBasePoint;							//桌子倍数
CPoint				CDesk8::m_PtBegin;								//开始位置
CRect				CDesk8::m_IndexRect;							//游戏桌号码
CPoint				CDesk8::m_PtAgree[8];							//同意位置
CRect				CDesk8::m_NameRect[8];							//名字框架
CRect				CDesk8::m_ChairRect[8];							//桌子位置
UINT				CDesk8::m_TextFormat[8];						//格式参数
CPoint				CDesk8::m_PtLog[8];								//人物位置
CPoint				CDesk8::m_BtnAgree[8];								//同意位置
int					CDesk8::m_draw[8];								//显示顺序
/// 颜色值
Color				CDesk8::m_UserNameColor;					///< 玩家名
Color				CDesk8::m_VipNameColor;						///< VIP玩家名
Color				CDesk8::m_DeskNoColor;						///< 桌号字体颜色
Color				CDesk8::m_FocusColor;						///< 焦点框颜色							 //end
//CPoint				CDesk7::m_PtLock;								//锁位置
//CPoint				CDesk7::m_PtBegin;								//开始位置
//CRect				CDesk7::m_IndexRect;							//游戏桌号码
//CPoint				CDesk7::m_PtAgree[7];							//同意位置
//CRect				CDesk7::m_NameRect[7];							//名字框架
//CRect				CDesk7::m_ChairRect[7];							//桌子位置
//UINT				CDesk7::m_TextFormat[7];						//格式参数
//
//CPoint				CDesk8::m_PtLock;								//锁位置
//CPoint				CDesk8::m_PtBegin;								//开始位置
//CRect				CDesk8::m_IndexRect;							//游戏桌号码
//CPoint				CDesk8::m_PtAgree[8];							//同意位置
//CRect				CDesk8::m_NameRect[8];							//名字框架
//CRect				CDesk8::m_ChairRect[8];							//桌子位置
//UINT				CDesk8::m_TextFormat[8];						//格式参数

CPoint				CDesk16::m_PtLock;								//锁位置
CPoint				CDesk16::m_PtBasePoint;							//桌子倍数
CPoint				CDesk16::m_PtBegin;								//开始位置
CRect				CDesk16::m_IndexRect;							//游戏桌号码
CPoint				CDesk16::m_PtAgree[16];							//同意位置
CRect				CDesk16::m_NameRect[16];						//名字框架
CRect				CDesk16::m_ChairRect[16];						//桌子位置
UINT				CDesk16::m_TextFormat[16];						//格式参数
CPoint				CDesk16::m_PtLog[16];							//人物位置
CPoint				CDesk16::m_BtnAgree[16];						//同意位置
int					CDesk16::m_draw[16];							//显示顺序
/// 颜色值
Color				CDesk16::m_UserNameColor;					///< 玩家名
Color				CDesk16::m_VipNameColor;						///< VIP玩家名
Color				CDesk16::m_DeskNoColor;						///< 桌号字体颜色
Color				CDesk16::m_FocusColor;						///< 焦点框颜色

CPoint				CDeskEx2::m_PtLock;								//锁位置
CPoint				CDeskEx2::m_PtBegin;							//开始位置
CRect				CDeskEx2::m_IndexRect;							//游戏桌号码
CPoint				CDeskEx2::m_PtAgree[2];							//同意位置
CRect				CDeskEx2::m_NameRect[2];						//名字框架
CRect				CDeskEx2::m_ChairRect[2];						//桌子位置
UINT				CDeskEx2::m_TextFormat[2];						//格式参数
CPoint				CDeskEx2::m_PtLog[2];							//人物位置
CPoint				CDeskEx2::m_BtnAgree[2];								//同意位置
int					CDeskEx2::m_draw[2];							//显示顺序
/// 颜色值
Color				CDeskEx2::m_UserNameColor;					///< 玩家名
Color				CDeskEx2::m_VipNameColor;						///< VIP玩家名
Color				CDeskEx2::m_DeskNoColor;						///< 桌号字体颜色
Color				CDeskEx2::m_FocusColor;						///< 焦点框颜色

CPoint				CDeskEx3::m_PtLock;								//锁位置
CPoint				CDeskEx3::m_PtBegin;							//开始位置
CRect				CDeskEx3::m_IndexRect;							//游戏桌号码
CPoint				CDeskEx3::m_PtAgree[3];							//同意位置
CRect				CDeskEx3::m_NameRect[3];						//名字框架
CRect				CDeskEx3::m_ChairRect[3];						//桌子位置
UINT				CDeskEx3::m_TextFormat[3];						//格式参数
CPoint				CDeskEx3::m_PtLog[3];							//人物位置
CPoint				CDeskEx3::m_BtnAgree[3];								//同意位置
int					CDeskEx3::m_draw[3];							//显示顺序
/// 颜色值
Color				CDeskEx3::m_UserNameColor;					///< 玩家名
Color				CDeskEx3::m_VipNameColor;						///< VIP玩家名
Color				CDeskEx3::m_DeskNoColor;						///< 桌号字体颜色
Color				CDeskEx3::m_FocusColor;						///< 焦点框颜色

CPoint				CDeskEx4::m_PtLock;								//锁位置
CPoint				CDeskEx4::m_PtBegin;							//开始位置
CRect				CDeskEx4::m_IndexRect;							//游戏桌号码
CPoint				CDeskEx4::m_PtAgree[4];							//同意位置
CRect				CDeskEx4::m_NameRect[4];						//名字框架
CRect				CDeskEx4::m_ChairRect[4];						//桌子位置
UINT				CDeskEx4::m_TextFormat[4];						//格式参数
CPoint				CDeskEx4::m_PtLog[4];							//人物位置
CPoint				CDeskEx4::m_BtnAgree[4];								//同意位置
int					CDeskEx4::m_draw[4];							//显示顺序
/// 颜色值
Color				CDeskEx4::m_UserNameColor;					///< 玩家名
Color				CDeskEx4::m_VipNameColor;						///< VIP玩家名
Color				CDeskEx4::m_DeskNoColor;						///< 桌号字体颜色
Color				CDeskEx4::m_FocusColor;						///< 焦点框颜色

CPoint				CDeskEx5::m_PtLock;								//锁位置
CPoint				CDeskEx5::m_PtBegin;							//开始位置
CRect				CDeskEx5::m_IndexRect;							//游戏桌号码
CPoint				CDeskEx5::m_PtAgree[5];							//同意位置
CRect				CDeskEx5::m_NameRect[5];						//名字框架
CRect				CDeskEx5::m_ChairRect[5];						//桌子位置
UINT				CDeskEx5::m_TextFormat[5];						//格式参数
CPoint				CDeskEx5::m_PtLog[5];							//人物位置
CPoint				CDeskEx5::m_BtnAgree[5];								//同意位置
int					CDeskEx5::m_draw[5];							//显示顺序
/// 颜色值
Color				CDeskEx5::m_UserNameColor;					///< 玩家名
Color				CDeskEx5::m_VipNameColor;						///< VIP玩家名
Color				CDeskEx5::m_DeskNoColor;						///< 桌号字体颜色
Color				CDeskEx5::m_FocusColor;						///< 焦点框颜色

CPoint				CDeskEx6::m_PtLock;								//锁位置
CPoint				CDeskEx6::m_PtBegin;							//开始位置
CRect				CDeskEx6::m_IndexRect;							//游戏桌号码
CPoint				CDeskEx6::m_PtAgree[6];							//同意位置
CRect				CDeskEx6::m_NameRect[6];						//名字框架
CRect				CDeskEx6::m_ChairRect[6];						//桌子位置
UINT				CDeskEx6::m_TextFormat[6];						//格式参数
CPoint				CDeskEx6::m_PtLog[6];							//人物位置
CPoint				CDeskEx6::m_BtnAgree[6];								//同意位置
int					CDeskEx6::m_draw[6];							//显示顺序
/// 颜色值
Color				CDeskEx6::m_UserNameColor;					///< 玩家名
Color				CDeskEx6::m_VipNameColor;						///< VIP玩家名
Color				CDeskEx6::m_DeskNoColor;						///< 桌号字体颜色
Color				CDeskEx6::m_FocusColor;						///< 焦点框颜色

CPoint				CDeskEx8::m_PtLock;								//锁位置							//8人桌    start
CPoint				CDeskEx8::m_PtBasePoint;							//桌子倍数
CPoint				CDeskEx8::m_PtBegin;								//开始位置
CRect				CDeskEx8::m_IndexRect;							//游戏桌号码
CPoint				CDeskEx8::m_PtAgree[8];							//同意位置
CRect				CDeskEx8::m_NameRect[8];							//名字框架
CRect				CDeskEx8::m_ChairRect[8];							//桌子位置
UINT				CDeskEx8::m_TextFormat[8];						//格式参数
CPoint				CDeskEx8::m_PtLog[8];								//人物位置
CPoint				CDeskEx8::m_BtnAgree[8];								//同意位置
int					CDeskEx8::m_draw[8];								//显示顺序
/// 颜色值
Color				CDeskEx8::m_UserNameColor;					///< 玩家名
Color				CDeskEx8::m_VipNameColor;						///< VIP玩家名
Color				CDeskEx8::m_DeskNoColor;						///< 桌号字体颜色
Color				CDeskEx8::m_FocusColor;						///< 焦点框颜色							 //end
//CPoint				CDeskEx7::m_PtLock;								//锁位置
//CPoint				CDeskEx7::m_PtBegin;							//开始位置
//CRect				CDeskEx7::m_IndexRect;							//游戏桌号码
//CPoint				CDeskEx7::m_PtAgree[7];							//同意位置
//CRect				CDeskEx7::m_NameRect[7];						//名字框架
//CRect				CDeskEx7::m_ChairRect[7];						//桌子位置
//UINT				CDeskEx7::m_TextFormat[7];						//格式参数
//
//CPoint				CDeskEx8::m_PtLock;								//锁位置
//CPoint				CDeskEx8::m_PtBegin;							//开始位置
//CRect				CDeskEx8::m_IndexRect;							//游戏桌号码
//CPoint				CDeskEx8::m_PtAgree[8];							//同意位置
//CRect				CDeskEx8::m_NameRect[8];						//名字框架
//CRect				CDeskEx8::m_ChairRect[8];						//桌子位置
//UINT				CDeskEx8::m_TextFormat[8];						//格式参数

CPoint				CDeskEx16::m_PtLock;							//锁位置
CPoint				CDeskEx16::m_PtBegin;							//开始位置
CRect				CDeskEx16::m_IndexRect;							//游戏桌号码
CPoint				CDeskEx16::m_PtAgree[16];						//同意位置
CRect				CDeskEx16::m_NameRect[16];						//名字框架
CRect				CDeskEx16::m_ChairRect[16];						//桌子位置
UINT				CDeskEx16::m_TextFormat[16];					//格式参数
CPoint				CDeskEx16::m_PtLog[16];							//人物位置
CPoint				CDeskEx16::m_BtnAgree[16];						//同意位置
int					CDeskEx16::m_draw[16];							//显示顺序
/// 颜色值
Color				CDeskEx16::m_UserNameColor;						///< 玩家名
Color				CDeskEx16::m_VipNameColor;						///< VIP玩家名
Color				CDeskEx16::m_DeskNoColor;						///< 桌号字体颜色
Color				CDeskEx16::m_FocusColor;						///< 焦点框颜色

//百家乐 add by wlr 20090713
CPoint				CDeskBJL::m_PtLock;								//锁位置
CPoint				CDeskBJL::m_PtBasePoint;						//桌子倍数
CPoint				CDeskBJL::m_PtBegin;							//开始位置
CRect				CDeskBJL::m_IndexRect;							//游戏桌号码
CPoint				CDeskBJL::m_PtAgree[180];						//同意位置
CRect				CDeskBJL::m_NameRect[180];						//名字框架
CRect				CDeskBJL::m_ChairRect[180];						//桌子位置
UINT				CDeskBJL::m_TextFormat[180];					//格式参数
CPoint				CDeskBJL::m_PtLog[180];							//人物位置
CPoint				CDeskBJL::m_BtnAgree[180];						//同意位置
int					CDeskBJL::m_draw[180];							//显示顺序
/// 颜色值
Color				CDeskBJL::m_UserNameColor;					///< 玩家名
Color				CDeskBJL::m_VipNameColor;						///< VIP玩家名
Color				CDeskBJL::m_DeskNoColor;						///< 桌号字体颜色
Color				CDeskBJL::m_FocusColor;						///< 焦点框颜色

//end of 百家乐 add by wlr 20090713

UINT CBaseDeskView::m_uGameNameID = 0;							//游戏ID

BEGIN_MESSAGE_MAP(CDeskFrameView, CWnd)
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_SETCURSOR()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_BN_CLICKED(IDC_DESKPAGEUP, OnHitpageup)
	ON_BN_CLICKED(IDC_DESKPAGEDOWN, OnHitpagedown)
	ON_BN_CLICKED(IDC_BEGINAGREE, OnHitBegin)
	ON_BN_CLICKED(IDC_BJLBEGIN, OnHitBJLBegin)//百家乐 add by wlr 20090713 IDC_BJLPLAYING
	ON_BN_CLICKED(IDC_BJLPLAYING, OnHitBJLBegin)//百家乐 add by wlr 20090713 IDC_BJLPLAYING

	ON_BN_CLICKED(IDC_QUEUEBEGIN, OnHitQueueBegin)//百家乐 add by wlr 20090713 IDC_BJLPLAYING
	ON_BN_CLICKED(IDC_QUEUESTOP, OnHitQueueStop)//百家乐 add by wlr 20090713 IDC_BJLPLAYING

	ON_WM_TIMER()
	//wushuqun 2009.7.13
	//修改游戏房间中退出游戏时闪屏问题
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

//静态变量
int				CDeskFrameView::m_iWhellTimes=2;							//滚轮倍数
int				CDeskFrameView::m_iScrollPos=10;							//滚动象素
/*******************************************************************************************************/

//构造函数
CBaseDeskView::CBaseDeskView()
{
	m_bLock=false;	//是否锁
	m_bDeskIndex=255;//游戏桌子索引
	m_bHoverIndex=255;//框架索引
	m_bDeskStation=255;//座位索引
	m_bPlayGame=false;//是否游戏
	m_iBasePoint=0;
	m_bDownMouse=false;//是否按下鼠标
	m_bFocusFrame=false;//焦点框架
	m_deskClass="";
	m_deskFlat=true;
	memset(m_pUserItem,0,sizeof(m_pUserItem));//用户资料
	memset(m_MatchInfo,0,sizeof(m_MatchInfo));//比赛信息
	m_pFont = new Font(L"宋体", 9, FontStyleRegular);
	m_bCanInput = true;
}

//析构函数
CBaseDeskView::~CBaseDeskView()
{
	delete m_pFont;
	m_pFont = NULL;
}

//初始化函数，桌子椅子的位置
bool CBaseDeskView:: InitDesk(CString deskClass,bool deskFlat,UINT uNameID, bool bIsVip)
{
	bool bInit=false;
	int x1,y1,x2,y2;
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CString path;
	// 同时支持两种方式游戏客户端
	// 要判断到底是ico还是exe，但没有参数输入，只好用笨办法，挨个找文件，先找exe方式，再找ico方式文件
	bool bIsIco = false;
	path.Format("%d\\config\\%s\\deskconfig.bcf",uNameID,deskClass);
	if (CBcfFile::IsFileExist(s + path))
	{
		bIsIco = false;
	}
	else
	{
		path.Format("config\\%s\\deskconfig.bcf",deskClass);
		bIsIco = true;
	}
	/// 状态配置文件
	CBcfFile f( s + path);
	CString key="";

	m_uGameNameID = uNameID;

	/// 初始化桌子里的颜色值
	CString strPrivatePath;
	strPrivatePath.Format("%s%d\\DeskBackGround.bcf", s, uNameID);
	if (CBcfFile::IsFileExist(strPrivatePath))
	{
		path = strPrivatePath;
	}
	else
	{
		path.Format("%sconfig\\%s\\deskconfig.bcf", s,deskClass); 
	}
	CBcfFile fbcf(path);
	{
		BYTE r = fbcf.GetKeyVal("DeskColors", "UserName_R", 255);
		BYTE g = fbcf.GetKeyVal("DeskColors", "UserName_G", 255);
		BYTE b = fbcf.GetKeyVal("DeskColors", "UserName_B", 255);
		CDesk1::m_UserNameColor = Color(r,g,b);
		CDesk2::m_UserNameColor = Color(r,g,b);
		CDesk3::m_UserNameColor = Color(r,g,b);
		CDesk4::m_UserNameColor = Color(r,g,b);
		CDesk5::m_UserNameColor = Color(r,g,b);
		CDesk6::m_UserNameColor = Color(r,g,b);
		CDesk8::m_UserNameColor = Color(r,g,b);
		CDesk16::m_UserNameColor = Color(r,g,b);
		r = fbcf.GetKeyVal("DeskColors", "VipName_R", 255);
		g = fbcf.GetKeyVal("DeskColors", "VipName_G", 255);
		b = fbcf.GetKeyVal("DeskColors", "VipName_B", 255);
		CDesk1::m_VipNameColor = Color(r,g,b);
		CDesk2::m_VipNameColor = Color(r,g,b);
		CDesk3::m_VipNameColor = Color(r,g,b);
		CDesk4::m_VipNameColor = Color(r,g,b);
		CDesk5::m_VipNameColor = Color(r,g,b);
		CDesk6::m_VipNameColor = Color(r,g,b);
		CDesk8::m_VipNameColor = Color(r,g,b);
		CDesk16::m_VipNameColor = Color(r,g,b);
		r = fbcf.GetKeyVal("DeskColors", "DeskNo_R", 255);
		g = fbcf.GetKeyVal("DeskColors", "DeskNo_G", 255);
		b = fbcf.GetKeyVal("DeskColors", "DeskNo_B", 255);
		CDesk1::m_DeskNoColor  = Color(r,g,b);
		CDesk2::m_DeskNoColor  = Color(r,g,b);
		CDesk3::m_DeskNoColor  = Color(r,g,b);
		CDesk4::m_DeskNoColor  = Color(r,g,b);
		CDesk5::m_DeskNoColor  = Color(r,g,b);
		CDesk6::m_DeskNoColor  = Color(r,g,b);
		CDesk8::m_DeskNoColor  = Color(r,g,b);
		CDesk16::m_DeskNoColor  = Color(r,g,b);
		r = fbcf.GetKeyVal("DeskColors", "FocusColor_R", 255);
		g = fbcf.GetKeyVal("DeskColors", "FocusColor_G", 255);
		b = fbcf.GetKeyVal("DeskColors", "FocusColor_B", 255);
		CDesk1::m_FocusColor   = Color(r,g,b);
		CDesk2::m_FocusColor   = Color(r,g,b);
		CDesk3::m_FocusColor   = Color(r,g,b);
		CDesk4::m_FocusColor   = Color(r,g,b);
		CDesk5::m_FocusColor   = Color(r,g,b);
		CDesk6::m_FocusColor   = Color(r,g,b);
		CDesk8::m_FocusColor   = Color(r,g,b);
		CDesk16::m_FocusColor   = Color(r,g,b);
	}

	/// 按不同桌子配置
	if (bInit==false)
	{
		bInit=true;
		CDesk1::m_TextFormat[0]=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_CENTER;

		key="Desk1";
		x1=f.GetKeyVal(key,"lockx",25);
		y1=f.GetKeyVal(key,"locky",20);
		CDesk1::m_PtLock.SetPoint(x1,y1);
		CDesk1::m_PtBegin.SetPoint(94, 98);//正在在游戏的桌面
		x1=f.GetKeyVal(key,"indexx1",25);
		y1=f.GetKeyVal(key,"indexy1",147);
		x2=f.GetKeyVal(key,"indexx2",143);
		y2=f.GetKeyVal(key,"indexy2",165);
		CDesk1::m_IndexRect.SetRect(x1,y1,x2,y2);
		x1=f.GetKeyVal(key,"agree0x",190);
		y1=f.GetKeyVal(key,"agree0y",31);
		CDesk1::m_PtAgree[0].SetPoint(x1, y1);
	   //名称框架
		x1=f.GetKeyVal(key,"name0x1",31);
		y1=f.GetKeyVal(key,"name0y1",43);
		x2=f.GetKeyVal(key,"name0x2",x1+80);
		y2=f.GetKeyVal(key,"name0y2",y2+16);
		CDesk1::m_NameRect[0].SetRect(x1,y1,x2,y2);//右
		x1=f.GetKeyVal(key,"chair0x1",181);
		y1=f.GetKeyVal(key,"chair0y1",35);
		x2=f.GetKeyVal(key,"chair0x2",209);
		y2=f.GetKeyVal(key,"chair0y2",110);
		CDesk1::m_ChairRect[0].SetRect(x1, y1,  x2, y2);//右
		
		x1=f.GetKeyVal(key,"log0x",131);
		y1=f.GetKeyVal(key,"log0y",31);
		CDesk1::m_PtLog[0].SetPoint(x1, y1);
		CString str;
		str.Format("Desk:x1=%d y1=%d",x1,y1);
		OutputDebugString(str);

		//初始化 2 人桌子
		if (bIsVip)
		{
			key="vipDesk";
		}
		else
		{
			key="Desk2";
		}
		x1=f.GetKeyVal(key,"lockx",25);
		y1=f.GetKeyVal(key,"locky",20);
		CDesk2::m_PtLock.SetPoint(x1,y1);
		x1=f.GetKeyVal(key,"basePointx",25);
		y1=f.GetKeyVal(key,"basePointy",140);
		CDesk2::m_PtBasePoint.SetPoint(x1,y1);
		CDesk2::m_PtBegin.SetPoint(94, 98);//正在在游戏的桌面
		x1=f.GetKeyVal(key,"indexx1",25);
		y1=f.GetKeyVal(key,"indexy1",147);
		x2=f.GetKeyVal(key,"indexx2",143);
		y2=f.GetKeyVal(key,"indexy2",165);
		CDesk2::m_IndexRect.SetRect(x1,y1,x2,y2);
		x1=f.GetKeyVal(key,"agree0x",190);
		y1=f.GetKeyVal(key,"agree0y",31);
		CDesk2::m_PtAgree[0].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"agree1x",31);
		y1=f.GetKeyVal(key,"agree1y",43);
		CDesk2::m_PtAgree[1].SetPoint(x1, y1);
		//名称框架
		x1=f.GetKeyVal(key,"name0x1",31);
		y1=f.GetKeyVal(key,"name0y1",43);
		x2=f.GetKeyVal(key,"name0x2",x1+80);
		y2=f.GetKeyVal(key,"name0y2",y2+16);
		CDesk2::m_NameRect[0].SetRect(x1,y1,x2,y2);//右
		x1=f.GetKeyVal(key,"name1x1",31);
		y1=f.GetKeyVal(key,"name1y1",43);
		x2=f.GetKeyVal(key,"name1x2",x1+80);
		y2=f.GetKeyVal(key,"name1y2",y2+16);
		CDesk2::m_NameRect[1].SetRect(x1,y1,x2,y2);//右		

		x1=f.GetKeyVal(key,"chair0x1",181);
		y1=f.GetKeyVal(key,"chair0y1",35);
		x2=f.GetKeyVal(key,"chair0x2",209);
		y2=f.GetKeyVal(key,"chair0y2",110);
		CDesk2::m_ChairRect[0].SetRect(x1, y1,  x2, y2);//右
		x1=f.GetKeyVal(key,"chair1x1",36);
		y1=f.GetKeyVal(key,"chair1y1",60);
		x2=f.GetKeyVal(key,"chair1x2",71);
		y2=f.GetKeyVal(key,"chair1y2",120);
		CDesk2::m_ChairRect[1].SetRect(x1, y1,  x2, y2);//左
		x1=f.GetKeyVal(key,"log0x",131);
		y1=f.GetKeyVal(key,"log0y",31);
		CDesk2::m_PtLog[0].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"log1x",36);
		y1=f.GetKeyVal(key,"log1y",43);
		CDesk2::m_PtLog[1].SetPoint(x1, y1);
		CDesk2::m_draw[0]=0;
		CDesk2::m_draw[1]=1;

		//初始化 3 人桌子012分别为左，右，上
		key="Desk3";
		x1=f.GetKeyVal(key,"lockx",25);
		y1=f.GetKeyVal(key,"locky",20);
		CDesk3::m_PtLock.SetPoint(x1,y1);
		x1=f.GetKeyVal(key,"basePointx",25);
		y1=f.GetKeyVal(key,"basePointy",140);
		CDesk3::m_PtBasePoint.SetPoint(x1,y1);
		CDesk3::m_PtBegin.SetPoint(94, 98);//正在在游戏的桌面
		x1=f.GetKeyVal(key,"indexx1",25);
		y1=f.GetKeyVal(key,"indexy1",147);
		x2=f.GetKeyVal(key,"indexx2",143);
		y2=f.GetKeyVal(key,"indexy2",165);
		CDesk3::m_IndexRect.SetRect(x1,y1,x2,y2);
		x1=f.GetKeyVal(key,"agree0x",190);
		y1=f.GetKeyVal(key,"agree0y",31);
		CDesk3::m_PtAgree[0].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"agree1x",97);
		y1=f.GetKeyVal(key,"agree1y",148);
		CDesk3::m_PtAgree[1].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"agree2x",31);
		y1=f.GetKeyVal(key,"agree2y",43);
		CDesk3::m_PtAgree[2].SetPoint(x1, y1);
		//名字框架位置		
		x1=f.GetKeyVal(key,"name0x1",31);
		y1=f.GetKeyVal(key,"name0y1",43);
		x2=f.GetKeyVal(key,"name0x2",x1+80);
		y2=f.GetKeyVal(key,"name0y2",y2+16);
		CDesk3::m_NameRect[0].SetRect(x1,y1,x2,y2);//右
		x1=f.GetKeyVal(key,"name1x1",31);
		y1=f.GetKeyVal(key,"name1y1",43);
		x2=f.GetKeyVal(key,"name1x2",x1+80);
		y2=f.GetKeyVal(key,"name1y2",y2+16);
		CDesk3::m_NameRect[1].SetRect(x1,y1,x2,y2);//右
		x1=f.GetKeyVal(key,"name2x1",31);
		y1=f.GetKeyVal(key,"name2y1",43);
		x2=f.GetKeyVal(key,"name2x2",x1+80);
		y2=f.GetKeyVal(key,"name2y2",y2+16);
		CDesk3::m_NameRect[2].SetRect(x1,y1,x2,y2);//右

		x1=f.GetKeyVal(key,"chair0x1",181);
		y1=f.GetKeyVal(key,"chair0y1",35);
		x2=f.GetKeyVal(key,"chair0x2",209);
		y2=f.GetKeyVal(key,"chair0y2",110);
		CDesk3::m_ChairRect[0].SetRect(x1, y1,  x2, y2);//右
		x1=f.GetKeyVal(key,"chair1x1",112);
		y1=f.GetKeyVal(key,"chair1y1",92);
		x2=f.GetKeyVal(key,"chair1x2",171);
		y2=f.GetKeyVal(key,"chair1y2",180);
		CDesk3::m_ChairRect[1].SetRect(x1, y1,  x2, y2);//最下方
		x1=f.GetKeyVal(key,"chair2x1",36);
		y1=f.GetKeyVal(key,"chair2y1",60);
		x2=f.GetKeyVal(key,"chair2x2",71);
		y2=f.GetKeyVal(key,"chair2y2",120);
		CDesk3::m_ChairRect[2].SetRect(x1, y1,  x2, y2);//左
		x1=f.GetKeyVal(key,"log0x",131);
		y1=f.GetKeyVal(key,"log0y",31);
		CDesk3::m_PtLog[0].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"log1x",98);
		y1=f.GetKeyVal(key,"log1y",52);
		CDesk3::m_PtLog[1].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"log2x",36);
		y1=f.GetKeyVal(key,"log2y",43);
		CDesk3::m_PtLog[2].SetPoint(x1, y1);
		CDesk3::m_draw[0]=0;
		CDesk3::m_draw[1]=2;
		CDesk3::m_draw[2]=1;

		//初始化 4 人桌子0123分别为右，下,左，上，
		key="Desk4";
		x1=f.GetKeyVal(key,"lockx",25);
		y1=f.GetKeyVal(key,"locky",20);
		CDesk4::m_PtLock.SetPoint(x1,y1);
		x1=f.GetKeyVal(key,"basePointx",25);
		y1=f.GetKeyVal(key,"basePointy",140);
		CDesk4::m_PtBasePoint.SetPoint(x1,y1);
		CDesk4::m_PtBegin.SetPoint(94, 98);//正在在游戏的桌面
		x1=f.GetKeyVal(key,"indexx1",25);
		y1=f.GetKeyVal(key,"indexy1",147);
		x2=f.GetKeyVal(key,"indexx2",143);
		y2=f.GetKeyVal(key,"indexy2",165);
		CDesk4::m_IndexRect.SetRect(x1,y1,x2,y2);
		x1=f.GetKeyVal(key,"agree0x",140);
		y1=f.GetKeyVal(key,"agree0y",31);
		CDesk4::m_PtAgree[0].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"agree1x",97);
		y1=f.GetKeyVal(key,"agree1y",148);
		CDesk4::m_PtAgree[1].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"agree2x",31);
		y1=f.GetKeyVal(key,"agree2y",43);
		CDesk4::m_PtAgree[2].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"agree3x",71);
		y1=f.GetKeyVal(key,"agree3y",29);
		CDesk4::m_PtAgree[3].SetPoint(x1, y1);
		//名字框架位置		
		x1=f.GetKeyVal(key,"name0x1",31);
		y1=f.GetKeyVal(key,"name0y1",43);
		x2=f.GetKeyVal(key,"name0x2",x1+80);
		y2=f.GetKeyVal(key,"name0y2",y2+16);
		CDesk4::m_NameRect[0].SetRect(x1,y1,x2,y2);//右
		x1=f.GetKeyVal(key,"name1x1",31);
		y1=f.GetKeyVal(key,"name1y1",43);
		x2=f.GetKeyVal(key,"name1x2",x1+80);
		y2=f.GetKeyVal(key,"name1y2",y2+16);
		CDesk4::m_NameRect[1].SetRect(x1,y1,x2,y2);//右
		x1=f.GetKeyVal(key,"name2x1",31);
		y1=f.GetKeyVal(key,"name2y1",43);
		x2=f.GetKeyVal(key,"name2x2",x1+80);
		y2=f.GetKeyVal(key,"name2y2",y2+16);
		CDesk4::m_NameRect[2].SetRect(x1,y1,x2,y2);//右
		x1=f.GetKeyVal(key,"name3x1",31);
		y1=f.GetKeyVal(key,"name3y1",43);
		x2=f.GetKeyVal(key,"name3x2",x1+80);
		y2=f.GetKeyVal(key,"name3y2",y2+16);
		CDesk4::m_NameRect[3].SetRect(x1,y1,x2,y2);//右

		x1=f.GetKeyVal(key,"chair0x1",181);
		y1=f.GetKeyVal(key,"chair0y1",35);
		x2=f.GetKeyVal(key,"chair0x2",209);
		y2=f.GetKeyVal(key,"chair0y2",110);
		CDesk4::m_ChairRect[0].SetRect(x1, y1,  x2, y2);//右
		x1=f.GetKeyVal(key,"chair1x1",112);
		y1=f.GetKeyVal(key,"chair1y1",92);
		x2=f.GetKeyVal(key,"chair1x2",171);
		y2=f.GetKeyVal(key,"chair1y2",180);
		CDesk4::m_ChairRect[1].SetRect(x1, y1,  x2, y2);//最下方
		x1=f.GetKeyVal(key,"chair2x1",36);
		y1=f.GetKeyVal(key,"chair2y1",60);
		x2=f.GetKeyVal(key,"chair2x2",71);
		y2=f.GetKeyVal(key,"chair2y2",120);
		CDesk4::m_ChairRect[2].SetRect(x1, y1,  x2, y2);//左
		x1=f.GetKeyVal(key,"chair3x1",85);
		y1=f.GetKeyVal(key,"chair3y1",35);
		x2=f.GetKeyVal(key,"chair3x2",105);
		y2=f.GetKeyVal(key,"chair3y2",82);
		CDesk4::m_ChairRect[3].SetRect(x1, y1,  x2, y2);//最上方
		x1=f.GetKeyVal(key,"log0x",131);
		y1=f.GetKeyVal(key,"log0y",31);
		CDesk4::m_PtLog[0].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"log1x",98);
		y1=f.GetKeyVal(key,"log1y",52);
		CDesk4::m_PtLog[1].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"log2x",36);
		y1=f.GetKeyVal(key,"log2y",43);
		CDesk4::m_PtLog[2].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"log3x",71);
		y1=f.GetKeyVal(key,"log3y",29);
		CDesk4::m_PtLog[3].SetPoint(x1, y1);
		CDesk4::m_draw[0]=3;
		CDesk4::m_draw[1]=0;
		CDesk4::m_draw[2]=2;
		CDesk4::m_draw[3]=1;

		CDesk4::m_TextFormat[0]=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_RIGHT;//右
		CDesk4::m_TextFormat[1]=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_CENTER;//下
		CDesk4::m_TextFormat[2]=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_CENTER;//左
		CDesk4::m_TextFormat[3]=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_LEFT;//上

		//初始化 5 人桌子
		key="Desk5";
		x1=f.GetKeyVal(key,"lockx",25);
		y1=f.GetKeyVal(key,"locky",20);
		CDesk5::m_PtLock.SetPoint(x1,y1);
		x1=f.GetKeyVal(key,"basePointx",25);
		y1=f.GetKeyVal(key,"basePointy",140);
		CDesk5::m_PtBasePoint.SetPoint(x1,y1);
		CDesk5::m_PtBegin.SetPoint(93,103);//正在在游戏的桌面
		x1=f.GetKeyVal(key,"indexx1",25);
		y1=f.GetKeyVal(key,"indexy1",147);
		x2=f.GetKeyVal(key,"indexx2",143);
		y2=f.GetKeyVal(key,"indexy2",165);
		CDesk5::m_IndexRect.SetRect(x1,y1,x2,y2);
		x1=f.GetKeyVal(key,"agree0x",190);
		y1=f.GetKeyVal(key,"agree0y",31);
		CDesk5::m_PtAgree[0].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"agree1x",97);
		y1=f.GetKeyVal(key,"agree1y",148);
		CDesk5::m_PtAgree[1].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"agree2x",31);
		y1=f.GetKeyVal(key,"agree2y",43);
		CDesk5::m_PtAgree[2].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"agree3x",71);
		y1=f.GetKeyVal(key,"agree3y",29);
		CDesk5::m_PtAgree[3].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"agree4x",91);
		y1=f.GetKeyVal(key,"agree4y",29);
		CDesk5::m_PtAgree[4].SetPoint(x1, y1);

		x1=f.GetKeyVal(key,"name0x1",31);
		y1=f.GetKeyVal(key,"name0y1",43);
		x2=f.GetKeyVal(key,"name0x2",x1+80);
		y2=f.GetKeyVal(key,"name0y2",y2+16);
		CDesk5::m_NameRect[0].SetRect(x1,y1,x2,y2);
		x1=f.GetKeyVal(key,"name1x1",31);
		y1=f.GetKeyVal(key,"name1y1",43);
		x2=f.GetKeyVal(key,"name1x2",x1+80);
		y2=f.GetKeyVal(key,"name1y2",y2+16);
		CDesk5::m_NameRect[1].SetRect(x1,y1,x2,y2);
		x1=f.GetKeyVal(key,"name2x1",31);
		y1=f.GetKeyVal(key,"name2y1",43);
		x2=f.GetKeyVal(key,"name2x2",x1+80);
		y2=f.GetKeyVal(key,"name2y2",y2+16);
		CDesk5::m_NameRect[2].SetRect(x1,y1,x2,y2);
		x1=f.GetKeyVal(key,"name3x1",31);
		y1=f.GetKeyVal(key,"name3y1",43);
		x2=f.GetKeyVal(key,"name3x2",x1+80);
		y2=f.GetKeyVal(key,"name3y2",y2+16);
		CDesk5::m_NameRect[3].SetRect(x1,y1,x2,y2);
		x1=f.GetKeyVal(key,"name4x1",31);
		y1=f.GetKeyVal(key,"name4y1",43);
		x2=f.GetKeyVal(key,"name4x2",x1+80);
		y2=f.GetKeyVal(key,"name4y2",y2+16);
		CDesk5::m_NameRect[4].SetRect(x1,y1,x2,y2);
		x1=f.GetKeyVal(key,"chair0x1",181);
		y1=f.GetKeyVal(key,"chair0y1",35);
		x2=f.GetKeyVal(key,"chair0x2",209);
		y2=f.GetKeyVal(key,"chair0y2",110);
		CDesk5::m_ChairRect[0].SetRect(x1, y1,  x2, y2);//右
		x1=f.GetKeyVal(key,"chair1x1",112);
		y1=f.GetKeyVal(key,"chair1y1",92);
		x2=f.GetKeyVal(key,"chair1x2",171);
		y2=f.GetKeyVal(key,"chair1y2",180);
		CDesk5::m_ChairRect[1].SetRect(x1, y1,  x2, y2);//最下方
		x1=f.GetKeyVal(key,"chair2x1",36);
		y1=f.GetKeyVal(key,"chair2y1",60);
		x2=f.GetKeyVal(key,"chair2x2",71);
		y2=f.GetKeyVal(key,"chair2y2",120);
		CDesk5::m_ChairRect[2].SetRect(x1, y1,  x2, y2);//左
		x1=f.GetKeyVal(key,"chair3x1",85);
		y1=f.GetKeyVal(key,"chair3y1",35);
		x2=f.GetKeyVal(key,"chair3x2",105);
		y2=f.GetKeyVal(key,"chair3y2",82);
		CDesk5::m_ChairRect[3].SetRect(x1, y1,  x2, y2);//最上方
		x1=f.GetKeyVal(key,"chair4x1",95);
		y1=f.GetKeyVal(key,"chair4y1",35);
		x2=f.GetKeyVal(key,"chair4x2",105);
		y2=f.GetKeyVal(key,"chair4y2",82);
		CDesk5::m_ChairRect[4].SetRect(x1, y1,  x2, y2);//最上方
		x1=f.GetKeyVal(key,"log0x",131);
		y1=f.GetKeyVal(key,"log0y",31);
		CDesk5::m_PtLog[0].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"log1x",98);
		y1=f.GetKeyVal(key,"log1y",52);
		CDesk5::m_PtLog[1].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"log2x",36);
		y1=f.GetKeyVal(key,"log2y",43);
		CDesk5::m_PtLog[2].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"log3x",71);
		y1=f.GetKeyVal(key,"log3y",29);
		CDesk5::m_PtLog[3].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"log4x",91);
		y1=f.GetKeyVal(key,"log4y",29);
		CDesk5::m_PtLog[4].SetPoint(x1, y1);
		CDesk5::m_draw[0]=0;
		CDesk5::m_draw[1]=1;
		CDesk5::m_draw[2]=2;
		CDesk5::m_draw[3]=3;
		CDesk5::m_draw[4]=4;
		CDesk5::m_TextFormat[0]=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_LEFT;
		CDesk5::m_TextFormat[1]=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_CENTER;
		CDesk5::m_TextFormat[2]=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_RIGHT;
		CDesk5::m_TextFormat[3]=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_CENTER;
		CDesk5::m_TextFormat[4]=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_CENTER;

		//初始化 6 人桌子
		key="Desk6";
		x1=f.GetKeyVal(key,"lockx",25);
		y1=f.GetKeyVal(key,"locky",20);
		CDesk6::m_PtLock.SetPoint(x1,y1);
		x1=f.GetKeyVal(key,"basePointx",25);
		y1=f.GetKeyVal(key,"basePointy",70);
		CDesk6::m_PtBasePoint.SetPoint(x1,y1);
		CDesk6::m_PtBegin.SetPoint(93,103);//正在在游戏的桌面
		x1=f.GetKeyVal(key,"indexx1",25);
		y1=f.GetKeyVal(key,"indexy1",147);
		x2=f.GetKeyVal(key,"indexx2",143);
		y2=f.GetKeyVal(key,"indexy2",165);
		CDesk6::m_IndexRect.SetRect(x1,y1,x2,y2);
		x1=f.GetKeyVal(key,"agree0x",190);
		y1=f.GetKeyVal(key,"agree0y",31);
		CDesk6::m_PtAgree[0].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"agree1x",97);
		y1=f.GetKeyVal(key,"agree1y",148);
		CDesk6::m_PtAgree[1].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"agree2x",31);
		y1=f.GetKeyVal(key,"agree2y",43);
		CDesk6::m_PtAgree[2].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"agree3x",71);
		y1=f.GetKeyVal(key,"agree3y",29);
		CDesk6::m_PtAgree[3].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"agree4x",91);
		y1=f.GetKeyVal(key,"agree4y",29);
		CDesk6::m_PtAgree[4].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"agree5x",91);
		y1=f.GetKeyVal(key,"agree5y",29);
		CDesk6::m_PtAgree[5].SetPoint(x1, y1);

		x1=f.GetKeyVal(key,"name0x1",31);
		y1=f.GetKeyVal(key,"name0y1",43);
		x2=f.GetKeyVal(key,"name0x2",x1+80);
		y2=f.GetKeyVal(key,"name0y2",y2+16);
		CDesk6::m_NameRect[0].SetRect(x1,y1,x2,y2);
		x1=f.GetKeyVal(key,"name1x1",31);
		y1=f.GetKeyVal(key,"name1y1",43);
		x2=f.GetKeyVal(key,"name1x2",x1+80);
		y2=f.GetKeyVal(key,"name1y2",y2+16);
		CDesk6::m_NameRect[1].SetRect(x1,y1,x2,y2);
		x1=f.GetKeyVal(key,"name2x1",31);
		y1=f.GetKeyVal(key,"name2y1",43);
		x2=f.GetKeyVal(key,"name2x2",x1+80);
		y2=f.GetKeyVal(key,"name2y2",y2+16);
		CDesk6::m_NameRect[2].SetRect(x1,y1,x2,y2);
		x1=f.GetKeyVal(key,"name3x1",31);
		y1=f.GetKeyVal(key,"name3y1",43);
		x2=f.GetKeyVal(key,"name3x2",x1+80);
		y2=f.GetKeyVal(key,"name3y2",y2+16);
		CDesk6::m_NameRect[3].SetRect(x1,y1,x2,y2);
		x1=f.GetKeyVal(key,"name4x1",31);
		y1=f.GetKeyVal(key,"name4y1",43);
		x2=f.GetKeyVal(key,"name4x2",x1+80);
		y2=f.GetKeyVal(key,"name4y2",y2+16);
		CDesk6::m_NameRect[4].SetRect(x1,y1,x2,y2);
		x1=f.GetKeyVal(key,"name5x1",31);
		y1=f.GetKeyVal(key,"name5y1",43);
		x2=f.GetKeyVal(key,"name5x2",x1+80);
		y2=f.GetKeyVal(key,"name5y2",y2+16);
		CDesk6::m_NameRect[5].SetRect(x1,y1,x2,y2);
		x1=f.GetKeyVal(key,"chair0x1",181);
		y1=f.GetKeyVal(key,"chair0y1",35);
		x2=f.GetKeyVal(key,"chair0x2",209);
		y2=f.GetKeyVal(key,"chair0y2",110);
		CDesk6::m_ChairRect[0].SetRect(x1, y1,  x2, y2);//右
		x1=f.GetKeyVal(key,"chair1x1",112);
		y1=f.GetKeyVal(key,"chair1y1",92);
		x2=f.GetKeyVal(key,"chair1x2",171);
		y2=f.GetKeyVal(key,"chair1y2",180);
		CDesk6::m_ChairRect[1].SetRect(x1, y1,  x2, y2);//最下方
		x1=f.GetKeyVal(key,"chair2x1",36);
		y1=f.GetKeyVal(key,"chair2y1",60);
		x2=f.GetKeyVal(key,"chair2x2",71);
		y2=f.GetKeyVal(key,"chair2y2",120);
		CDesk6::m_ChairRect[2].SetRect(x1, y1,  x2, y2);//左
		x1=f.GetKeyVal(key,"chair3x1",85);
		y1=f.GetKeyVal(key,"chair3y1",35);
		x2=f.GetKeyVal(key,"chair3x2",105);
		y2=f.GetKeyVal(key,"chair3y2",82);
		CDesk6::m_ChairRect[3].SetRect(x1, y1,  x2, y2);//最上方
		x1=f.GetKeyVal(key,"chair4x1",95);
		y1=f.GetKeyVal(key,"chair4y1",35);
		x2=f.GetKeyVal(key,"chair4x2",105);
		y2=f.GetKeyVal(key,"chair4y2",82);
		CDesk6::m_ChairRect[4].SetRect(x1, y1,  x2, y2);//最上方
		x1=f.GetKeyVal(key,"chair5x1",95);
		y1=f.GetKeyVal(key,"chair5y1",35);
		x2=f.GetKeyVal(key,"chair5x2",105);
		y2=f.GetKeyVal(key,"chair5y2",82);
		CDesk6::m_ChairRect[5].SetRect(x1, y1,  x2, y2);//最上方
		x1=f.GetKeyVal(key,"log0x",131);
		y1=f.GetKeyVal(key,"log0y",31);
		CDesk6::m_PtLog[0].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"log1x",98);
		y1=f.GetKeyVal(key,"log1y",52);
		CDesk6::m_PtLog[1].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"log2x",36);
		y1=f.GetKeyVal(key,"log2y",43);
		CDesk6::m_PtLog[2].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"log3x",71);
		y1=f.GetKeyVal(key,"log3y",29);
		CDesk6::m_PtLog[3].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"log4x",91);
		y1=f.GetKeyVal(key,"log4y",29);
		CDesk6::m_PtLog[4].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"log5x",91);
		y1=f.GetKeyVal(key,"log5y",29);
		CDesk6::m_PtLog[5].SetPoint(x1, y1);
		CDesk6::m_draw[0]=0;
		CDesk6::m_draw[1]=1;
		CDesk6::m_draw[2]=2;
		CDesk6::m_draw[3]=3;
		CDesk6::m_draw[4]=4;
		CDesk6::m_draw[5]=5;
		CDesk6::m_TextFormat[0]=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_CENTER;
		CDesk6::m_TextFormat[1]=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_CENTER;
		CDesk6::m_TextFormat[2]=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_LEFT;
		CDesk6::m_TextFormat[3]=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_CENTER;
		CDesk6::m_TextFormat[4]=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_CENTER;
		CDesk6::m_TextFormat[5]=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_CENTER;


		//初始化8人桌

		key="Desk8";
		x1=f.GetKeyVal(key,"lockx",25);
		y1=f.GetKeyVal(key,"locky",20);
		CDesk8::m_PtLock.SetPoint(x1,y1);
		x1=f.GetKeyVal(key,"basePointx",25);
		y1=f.GetKeyVal(key,"basePointy",70);
		CDesk8::m_PtBasePoint.SetPoint(x1,y1);
		CDesk8::m_PtBegin.SetPoint(93,103);//正在在游戏的桌面
		x1=f.GetKeyVal(key,"indexx1",25);
		y1=f.GetKeyVal(key,"indexy1",147);
		x2=f.GetKeyVal(key,"indexx2",143);
		y2=f.GetKeyVal(key,"indexy2",165);
		CDesk8::m_IndexRect.SetRect(x1,y1,x2,y2);
		x1=f.GetKeyVal(key,"agree0x",190);
		y1=f.GetKeyVal(key,"agree0y",31);
		CDesk8::m_PtAgree[0].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"agree1x",97);
		y1=f.GetKeyVal(key,"agree1y",148);
		CDesk8::m_PtAgree[1].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"agree2x",31);
		y1=f.GetKeyVal(key,"agree2y",43);
		CDesk8::m_PtAgree[2].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"agree3x",71);
		y1=f.GetKeyVal(key,"agree3y",29);
		CDesk8::m_PtAgree[3].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"agree4x",91);
		y1=f.GetKeyVal(key,"agree4y",29);
		CDesk8::m_PtAgree[4].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"agree5x",91);
		y1=f.GetKeyVal(key,"agree5y",29);
		CDesk8::m_PtAgree[5].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"agree6x",91);
		y1=f.GetKeyVal(key,"agree6y",29);
		CDesk8::m_PtAgree[6].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"agree7x",91);
		y1=f.GetKeyVal(key,"agree7y",29);
		CDesk8::m_PtAgree[7].SetPoint(x1, y1);

		x1=f.GetKeyVal(key,"name0x1",31+169);
		y1=f.GetKeyVal(key,"name0y1",43-11);
		x2=f.GetKeyVal(key,"name0x2",x1+80+169);
		y2=f.GetKeyVal(key,"name0y2",y2+16-11);
		CDesk8::m_NameRect[0].SetRect(x1,y1,x2,y2);
		x1=f.GetKeyVal(key,"name1x1",31);
		y1=f.GetKeyVal(key,"name1y1",43);
		x2=f.GetKeyVal(key,"name1x2",x1+80);
		y2=f.GetKeyVal(key,"name1y2",y2+16);
		CDesk8::m_NameRect[1].SetRect(x1,y1,x2,y2);
		x1=f.GetKeyVal(key,"name2x1",31);
		y1=f.GetKeyVal(key,"name2y1",43);
		x2=f.GetKeyVal(key,"name2x2",x1+80);
		y2=f.GetKeyVal(key,"name2y2",y2+16);
		CDesk8::m_NameRect[2].SetRect(x1,y1,x2,y2);
		x1=f.GetKeyVal(key,"name3x1",31);
		y1=f.GetKeyVal(key,"name3y1",43);
		x2=f.GetKeyVal(key,"name3x2",x1+80);
		y2=f.GetKeyVal(key,"name3y2",y2+16);
		CDesk8::m_NameRect[3].SetRect(x1,y1,x2,y2);
		x1=f.GetKeyVal(key,"name4x1",31);
		y1=f.GetKeyVal(key,"name4y1",43);
		x2=f.GetKeyVal(key,"name4x2",x1+80);
		y2=f.GetKeyVal(key,"name4y2",y2+16);
		CDesk8::m_NameRect[4].SetRect(x1,y1,x2,y2);
		x1=f.GetKeyVal(key,"name5x1",31);
		y1=f.GetKeyVal(key,"name5y1",43);
		x2=f.GetKeyVal(key,"name5x2",x1+80);
		y2=f.GetKeyVal(key,"name5y2",y2+16);
		CDesk8::m_NameRect[5].SetRect(x1,y1,x2,y2);
		x1=f.GetKeyVal(key,"name6x1",31);
		y1=f.GetKeyVal(key,"name6y1",43);
		x2=f.GetKeyVal(key,"name6x2",x1+80);
		y2=f.GetKeyVal(key,"name6y2",y2+16);
		CDesk8::m_NameRect[6].SetRect(x1,y1,x2,y2);
		x1=f.GetKeyVal(key,"name7x1",31+210);
		y1=f.GetKeyVal(key,"name7y1",43+40);
		x2=f.GetKeyVal(key,"name7x2",x1+80+210);
		y2=f.GetKeyVal(key,"name7y2",y2+16+40);
		CDesk8::m_NameRect[7].SetRect(x1,y1,x2,y2);

		x1=f.GetKeyVal(key,"chair0x1",181+15);
		y1=f.GetKeyVal(key,"chair0y1",35+58);
		x2=f.GetKeyVal(key,"chair0x2",209+15);
		y2=f.GetKeyVal(key,"chair0y2",110+58);
		CDesk8::m_ChairRect[0].SetRect(x1, y1,  x2, y2);//右
		x1=f.GetKeyVal(key,"chair1x1",181+15-81);
		y1=f.GetKeyVal(key,"chair1y1",35+58);
		x2=f.GetKeyVal(key,"chair1x2",209+15-81);
		y2=f.GetKeyVal(key,"chair1y2",110+58);
		CDesk8::m_ChairRect[1].SetRect(x1, y1,  x2, y2);//最下方
		x1=f.GetKeyVal(key,"chair2x1",181+15-81-81);
		y1=f.GetKeyVal(key,"chair2y1",35+58);
		x2=f.GetKeyVal(key,"chair2x2",209+15-81-81);
		y2=f.GetKeyVal(key,"chair2y2",110+58);
		CDesk8::m_ChairRect[2].SetRect(x1, y1,  x2, y2);//左
		x1=f.GetKeyVal(key,"chair3x1",181+15-81-81-35);
		y1=f.GetKeyVal(key,"chair3y1",35+58-46);
		x2=f.GetKeyVal(key,"chair3x2",209+15-81-81-35);
		y2=f.GetKeyVal(key,"chair3y2",110+58-46);
		CDesk8::m_ChairRect[3].SetRect(x1, y1,  x2, y2);//最上方
		x1=f.GetKeyVal(key,"chair4x1",181+15-81-81-35+62);
		y1=f.GetKeyVal(key,"chair4y1",35+58-46-39);
		x2=f.GetKeyVal(key,"chair4x2",209+15-81-81-35+62);
		y2=f.GetKeyVal(key,"chair4y2",110+58-46-39);
		CDesk8::m_ChairRect[4].SetRect(x1, y1,  x2, y2);//最上方
		x1=f.GetKeyVal(key,"chair5x1",181+15-81-81-35+62+81);
		y1=f.GetKeyVal(key,"chair5y1",35+58-46-39);
		x2=f.GetKeyVal(key,"chair5x2",209+15-81-81-35+62+81);
		y2=f.GetKeyVal(key,"chair5y2",110+58-46-39);
		CDesk8::m_ChairRect[5].SetRect(x1, y1,  x2, y2);//最上方
		x1=f.GetKeyVal(key,"chair6x1",181+15-81-81-35+62+81+81);
		y1=f.GetKeyVal(key,"chair6y1",35+58-46-39);
		x2=f.GetKeyVal(key,"chair6x2",209+15-81-81-35+62+81+81);
		y2=f.GetKeyVal(key,"chair6y2",110+58-46-39);
		CDesk8::m_ChairRect[6].SetRect(x1, y1,  x2, y2);//最上方
		x1=f.GetKeyVal(key,"chair7x1",181+20+30);
		y1=f.GetKeyVal(key,"chair7y1",35);
		x2=f.GetKeyVal(key,"chair7x2",209+20+30);
		y2=f.GetKeyVal(key,"chair7y2",110);
		CDesk8::m_ChairRect[7].SetRect(x1, y1,  x2, y2);//最上方

		x1=f.GetKeyVal(key,"log0x",181+15);
		y1=f.GetKeyVal(key,"log0y",35+58);
		CDesk8::m_PtLog[0].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"log1x",181+15-81);
		y1=f.GetKeyVal(key,"log1y",35+58);
		CDesk8::m_PtLog[1].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"log2x",181+15-81-81);
		y1=f.GetKeyVal(key,"log2y",35+58);
		CDesk8::m_PtLog[2].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"log3x",181+15-81-81-35);
		y1=f.GetKeyVal(key,"log3y",35+58-46);
		CDesk8::m_PtLog[3].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"log4x",181+15-81-81-35+62);
		y1=f.GetKeyVal(key,"log4y",35+58-46-39);
		CDesk8::m_PtLog[4].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"log5x",181+15-81-81-35+62+81);
		y1=f.GetKeyVal(key,"log5y",35+58-46-39);
		CDesk8::m_PtLog[5].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"log6x",181+15-81-81-35+62+81+81);
		y1=f.GetKeyVal(key,"log6y",35+58-46-39);
		CDesk8::m_PtLog[6].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"log7x",181+20+30);
		y1=f.GetKeyVal(key,"log7y",35);
		CDesk8::m_PtLog[7].SetPoint(x1, y1);

		CDesk8::m_draw[0]=0;
		CDesk8::m_draw[1]=1;
		CDesk8::m_draw[2]=2;
		CDesk8::m_draw[3]=3;
		CDesk8::m_draw[4]=4;
		CDesk8::m_draw[5]=5;
		CDesk8::m_draw[6]=6;
		CDesk8::m_draw[7]=7;

		CDesk8::m_TextFormat[0]=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_CENTER;
		CDesk8::m_TextFormat[1]=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_CENTER;
		CDesk8::m_TextFormat[2]=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_LEFT;
		CDesk8::m_TextFormat[3]=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_CENTER;
		CDesk8::m_TextFormat[4]=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_CENTER;
		CDesk8::m_TextFormat[5]=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_CENTER;
		CDesk8::m_TextFormat[6]=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_CENTER;
		CDesk8::m_TextFormat[7]=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_CENTER;
		//DebugPrintf("CDesk6::m_ChairRect[4] = [%d][%d][%d][%d]",
		//	CDesk6::m_ChairRect[4].left,
		//	CDesk6::m_ChairRect[4].top,
		//	CDesk6::m_ChairRect[4].right,
		//	CDesk6::m_ChairRect[4].bottom
		//	);

		//初始化 4 人桌子0123分别为右，下,左，上，
		key="Desk16";
		x1=f.GetKeyVal(key,"lockx",25);
		y1=f.GetKeyVal(key,"locky",20);
		CDesk16::m_PtLock.SetPoint(x1,y1);
		x1=f.GetKeyVal(key,"basePointx",25);
		y1=f.GetKeyVal(key,"basePointy",140);
		CDesk16::m_PtBasePoint.SetPoint(x1,y1);
		CDesk16::m_PtBegin.SetPoint(94, 98);//正在在游戏的桌面
		x1=f.GetKeyVal(key,"indexx1",25);
		y1=f.GetKeyVal(key,"indexy1",147);
		x2=f.GetKeyVal(key,"indexx2",143);
		y2=f.GetKeyVal(key,"indexy2",165);
		CDesk16::m_IndexRect.SetRect(x1,y1,x2,y2);
		x1=f.GetKeyVal(key,"agree0x",140);
		y1=f.GetKeyVal(key,"agree0y",31);
		CDesk16::m_PtAgree[0].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"agree1x",97);
		y1=f.GetKeyVal(key,"agree1y",148);
		CDesk16::m_PtAgree[1].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"agree2x",31);
		y1=f.GetKeyVal(key,"agree2y",43);
		CDesk16::m_PtAgree[2].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"agree3x",71);
		y1=f.GetKeyVal(key,"agree3y",29);
		CDesk16::m_PtAgree[3].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"agree4x",71);
		y1=f.GetKeyVal(key,"agree4y",29);
		CDesk16::m_PtAgree[4].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"agree5x",71);
		y1=f.GetKeyVal(key,"agree5y",29);
		CDesk16::m_PtAgree[5].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"agree6x",71);
		y1=f.GetKeyVal(key,"agree6y",29);
		CDesk16::m_PtAgree[6].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"agree7x",71);
		y1=f.GetKeyVal(key,"agree7y",29);
		CDesk16::m_PtAgree[7].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"agree8x",71);
		y1=f.GetKeyVal(key,"agree8y",29);
		CDesk16::m_PtAgree[8].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"agree9x",71);
		y1=f.GetKeyVal(key,"agree9y",29);
		CDesk16::m_PtAgree[9].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"agree10x",71);
		y1=f.GetKeyVal(key,"agree10y",29);
		CDesk16::m_PtAgree[10].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"agree11x",71);
		y1=f.GetKeyVal(key,"agree11y",29);
		CDesk16::m_PtAgree[11].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"agree12x",71);
		y1=f.GetKeyVal(key,"agree12y",29);
		CDesk16::m_PtAgree[12].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"agree13x",71);
		y1=f.GetKeyVal(key,"agree13y",29);
		CDesk16::m_PtAgree[13].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"agree14x",71);
		y1=f.GetKeyVal(key,"agree14y",29);
		CDesk16::m_PtAgree[14].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"agree15x",71);
		y1=f.GetKeyVal(key,"agree15y",29);
		CDesk16::m_PtAgree[15].SetPoint(x1, y1);
		//名字框架位置		
		x1=f.GetKeyVal(key,"name0x1",31);
		y1=f.GetKeyVal(key,"name0y1",43);
		x2=f.GetKeyVal(key,"name0x2",x1+80);
		y2=f.GetKeyVal(key,"name0y2",y2+16);
		CDesk16::m_NameRect[0].SetRect(x1,y1,x2,y2);//右
		x1=f.GetKeyVal(key,"name1x1",31);
		y1=f.GetKeyVal(key,"name1y1",43);
		x2=f.GetKeyVal(key,"name1x2",x1+80);
		y2=f.GetKeyVal(key,"name1y2",y2+16);
		CDesk16::m_NameRect[1].SetRect(x1,y1,x2,y2);//右
		x1=f.GetKeyVal(key,"name2x1",31);
		y1=f.GetKeyVal(key,"name2y1",43);
		x2=f.GetKeyVal(key,"name2x2",x1+80);
		y2=f.GetKeyVal(key,"name2y2",y2+16);
		CDesk16::m_NameRect[2].SetRect(x1,y1,x2,y2);//右
		x1=f.GetKeyVal(key,"name3x1",31);
		y1=f.GetKeyVal(key,"name3y1",43);
		x2=f.GetKeyVal(key,"name3x2",x1+80);
		y2=f.GetKeyVal(key,"name3y2",y2+16);
		CDesk16::m_NameRect[3].SetRect(x1,y1,x2,y2);//右
		x1=f.GetKeyVal(key,"name4x1",31);
		y1=f.GetKeyVal(key,"name4y1",43);
		x2=f.GetKeyVal(key,"name4x2",x1+80);
		y2=f.GetKeyVal(key,"name4y2",y2+16);
		CDesk16::m_NameRect[4].SetRect(x1,y1,x2,y2);//右
		x1=f.GetKeyVal(key,"name5x1",31);
		y1=f.GetKeyVal(key,"name5y1",43);
		x2=f.GetKeyVal(key,"name5x2",x1+80);
		y2=f.GetKeyVal(key,"name5y2",y2+16);
		CDesk16::m_NameRect[5].SetRect(x1,y1,x2,y2);//右
		x1=f.GetKeyVal(key,"name6x1",31);
		y1=f.GetKeyVal(key,"name6y1",43);
		x2=f.GetKeyVal(key,"name6x2",x1+80);
		y2=f.GetKeyVal(key,"name6y2",y2+16);
		CDesk16::m_NameRect[6].SetRect(x1,y1,x2,y2);//右
		x1=f.GetKeyVal(key,"name7x1",31);
		y1=f.GetKeyVal(key,"name7y1",43);
		x2=f.GetKeyVal(key,"name7x2",x1+80);
		y2=f.GetKeyVal(key,"name7y2",y2+16);
		CDesk16::m_NameRect[7].SetRect(x1,y1,x2,y2);//右
		x1=f.GetKeyVal(key,"name8x1",31);
		y1=f.GetKeyVal(key,"name8y1",43);
		x2=f.GetKeyVal(key,"name8x2",x1+80);
		y2=f.GetKeyVal(key,"name8y2",y2+16);
		CDesk16::m_NameRect[8].SetRect(x1,y1,x2,y2);//右
		x1=f.GetKeyVal(key,"name9x1",31);
		y1=f.GetKeyVal(key,"name9y1",43);
		x2=f.GetKeyVal(key,"name9x2",x1+80);
		y2=f.GetKeyVal(key,"name9y2",y2+16);
		CDesk16::m_NameRect[9].SetRect(x1,y1,x2,y2);//右
		x1=f.GetKeyVal(key,"name10x1",31);
		y1=f.GetKeyVal(key,"name10y1",43);
		x2=f.GetKeyVal(key,"name10x2",x1+80);
		y2=f.GetKeyVal(key,"name10y2",y2+16);
		CDesk16::m_NameRect[10].SetRect(x1,y1,x2,y2);//右
		x1=f.GetKeyVal(key,"name11x1",31);
		y1=f.GetKeyVal(key,"name11y1",43);
		x2=f.GetKeyVal(key,"name11x2",x1+80);
		y2=f.GetKeyVal(key,"name11y2",y2+16);
		CDesk16::m_NameRect[11].SetRect(x1,y1,x2,y2);//右
		x1=f.GetKeyVal(key,"name12x1",31);
		y1=f.GetKeyVal(key,"name12y1",43);
		x2=f.GetKeyVal(key,"name12x2",x1+80);
		y2=f.GetKeyVal(key,"name12y2",y2+16);
		CDesk16::m_NameRect[12].SetRect(x1,y1,x2,y2);//右
		x1=f.GetKeyVal(key,"name13x1",31);
		y1=f.GetKeyVal(key,"name13y1",43);
		x2=f.GetKeyVal(key,"name13x2",x1+80);
		y2=f.GetKeyVal(key,"name13y2",y2+16);
		CDesk16::m_NameRect[13].SetRect(x1,y1,x2,y2);//右
		x1=f.GetKeyVal(key,"name14x1",31);
		y1=f.GetKeyVal(key,"name14y1",43);
		x2=f.GetKeyVal(key,"name14x2",x1+80);
		y2=f.GetKeyVal(key,"name14y2",y2+16);
		CDesk16::m_NameRect[14].SetRect(x1,y1,x2,y2);//右
		x1=f.GetKeyVal(key,"name15x1",31);
		y1=f.GetKeyVal(key,"name15y1",43);
		x2=f.GetKeyVal(key,"name15x2",x1+80);
		y2=f.GetKeyVal(key,"name15y2",y2+16);
		CDesk16::m_NameRect[15].SetRect(x1,y1,x2,y2);//右

		x1=f.GetKeyVal(key,"chair0x1",181);
		y1=f.GetKeyVal(key,"chair0y1",35);
		x2=f.GetKeyVal(key,"chair0x2",209);
		y2=f.GetKeyVal(key,"chair0y2",110);
		CDesk16::m_ChairRect[0].SetRect(x1, y1,  x2, y2);//右
		x1=f.GetKeyVal(key,"chair1x1",112);
		y1=f.GetKeyVal(key,"chair1y1",92);
		x2=f.GetKeyVal(key,"chair1x2",171);
		y2=f.GetKeyVal(key,"chair1y2",180);
		CDesk16::m_ChairRect[1].SetRect(x1, y1,  x2, y2);//最下方
		x1=f.GetKeyVal(key,"chair2x1",36);
		y1=f.GetKeyVal(key,"chair2y1",60);
		x2=f.GetKeyVal(key,"chair2x2",71);
		y2=f.GetKeyVal(key,"chair2y2",120);
		CDesk16::m_ChairRect[2].SetRect(x1, y1,  x2, y2);//左
		x1=f.GetKeyVal(key,"chair3x1",85);
		y1=f.GetKeyVal(key,"chair3y1",35);
		x2=f.GetKeyVal(key,"chair3x2",105);
		y2=f.GetKeyVal(key,"chair3y2",82);
		CDesk16::m_ChairRect[3].SetRect(x1, y1,  x2, y2);//最上方
		x1=f.GetKeyVal(key,"chair4x1",85);
		y1=f.GetKeyVal(key,"chair4y1",35);
		x2=f.GetKeyVal(key,"chair4x2",105);
		y2=f.GetKeyVal(key,"chair4y2",82);
		CDesk16::m_ChairRect[4].SetRect(x1, y1,  x2, y2);//最上方
		x1=f.GetKeyVal(key,"chair5x1",85);
		y1=f.GetKeyVal(key,"chair5y1",35);
		x2=f.GetKeyVal(key,"chair5x2",105);
		y2=f.GetKeyVal(key,"chair5y2",82);
		CDesk16::m_ChairRect[5].SetRect(x1, y1,  x2, y2);//最上方
		x1=f.GetKeyVal(key,"chair6x1",85);
		y1=f.GetKeyVal(key,"chair6y1",35);
		x2=f.GetKeyVal(key,"chair6x2",105);
		y2=f.GetKeyVal(key,"chair6y2",82);
		CDesk16::m_ChairRect[6].SetRect(x1, y1,  x2, y2);//最上方
		x1=f.GetKeyVal(key,"chair7x1",85);
		y1=f.GetKeyVal(key,"chair7y1",35);
		x2=f.GetKeyVal(key,"chair7x2",105);
		y2=f.GetKeyVal(key,"chair7y2",82);
		CDesk16::m_ChairRect[7].SetRect(x1, y1,  x2, y2);//最上方
		x1=f.GetKeyVal(key,"chair8x1",85);
		y1=f.GetKeyVal(key,"chair8y1",35);
		x2=f.GetKeyVal(key,"chair8x2",105);
		y2=f.GetKeyVal(key,"chair8y2",82);
		CDesk16::m_ChairRect[8].SetRect(x1, y1,  x2, y2);//最上方
		x1=f.GetKeyVal(key,"chair9x1",85);
		y1=f.GetKeyVal(key,"chair9y1",35);
		x2=f.GetKeyVal(key,"chair9x2",105);
		y2=f.GetKeyVal(key,"chair9y2",82);
		CDesk16::m_ChairRect[9].SetRect(x1, y1,  x2, y2);//最上方
		x1=f.GetKeyVal(key,"chair10x1",85);
		y1=f.GetKeyVal(key,"chair10y1",35);
		x2=f.GetKeyVal(key,"chair10x2",105);
		y2=f.GetKeyVal(key,"chair10y2",82);
		CDesk16::m_ChairRect[10].SetRect(x1, y1,  x2, y2);//最上方
		x1=f.GetKeyVal(key,"chair11x1",85);
		y1=f.GetKeyVal(key,"chair11y1",35);
		x2=f.GetKeyVal(key,"chair11x2",105);
		y2=f.GetKeyVal(key,"chair11y2",82);
		CDesk16::m_ChairRect[11].SetRect(x1, y1,  x2, y2);//最上方
		x1=f.GetKeyVal(key,"chair12x1",85);
		y1=f.GetKeyVal(key,"chair12y1",35);
		x2=f.GetKeyVal(key,"chair12x2",105);
		y2=f.GetKeyVal(key,"chair12y2",82);
		CDesk16::m_ChairRect[12].SetRect(x1, y1,  x2, y2);//最上方
		x1=f.GetKeyVal(key,"chair13x1",85);
		y1=f.GetKeyVal(key,"chair13y1",35);
		x2=f.GetKeyVal(key,"chair13x2",105);
		y2=f.GetKeyVal(key,"chair13y2",82);
		CDesk16::m_ChairRect[13].SetRect(x1, y1,  x2, y2);//最上方
		x1=f.GetKeyVal(key,"chair14x1",85);
		y1=f.GetKeyVal(key,"chair14y1",35);
		x2=f.GetKeyVal(key,"chair14x2",105);
		y2=f.GetKeyVal(key,"chair14y2",82);
		CDesk16::m_ChairRect[14].SetRect(x1, y1,  x2, y2);//最上方
		x1=f.GetKeyVal(key,"chair15x1",85);
		y1=f.GetKeyVal(key,"chair15y1",35);
		x2=f.GetKeyVal(key,"chair15x2",105);
		y2=f.GetKeyVal(key,"chair15y2",82);
		CDesk16::m_ChairRect[15].SetRect(x1, y1,  x2, y2);//最上方
		x1=f.GetKeyVal(key,"log0x",131);
		y1=f.GetKeyVal(key,"log0y",31);
		CDesk16::m_PtLog[0].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"log1x",98);
		y1=f.GetKeyVal(key,"log1y",52);
		CDesk16::m_PtLog[1].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"log2x",36);
		y1=f.GetKeyVal(key,"log2y",43);
		CDesk16::m_PtLog[2].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"log3x",71);
		y1=f.GetKeyVal(key,"log3y",29);
		CDesk16::m_PtLog[3].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"log4x",71);
		y1=f.GetKeyVal(key,"log4y",29);
		CDesk16::m_PtLog[4].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"log5x",71);
		y1=f.GetKeyVal(key,"log5y",29);
		CDesk16::m_PtLog[5].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"log6x",71);
		y1=f.GetKeyVal(key,"log6y",29);
		CDesk16::m_PtLog[6].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"log7x",71);
		y1=f.GetKeyVal(key,"log7y",29);
		CDesk16::m_PtLog[7].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"log8x",71);
		y1=f.GetKeyVal(key,"log8y",29);
		CDesk16::m_PtLog[8].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"log9x",71);
		y1=f.GetKeyVal(key,"log9y",29);
		CDesk16::m_PtLog[9].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"log10x",71);
		y1=f.GetKeyVal(key,"log10y",29);
		CDesk16::m_PtLog[10].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"log11x",71);
		y1=f.GetKeyVal(key,"log11y",29);
		CDesk16::m_PtLog[11].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"log12x",71);
		y1=f.GetKeyVal(key,"log12y",29);
		CDesk16::m_PtLog[12].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"log13x",71);
		y1=f.GetKeyVal(key,"log13y",29);
		CDesk16::m_PtLog[13].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"log14x",71);
		y1=f.GetKeyVal(key,"log14y",29);
		CDesk16::m_PtLog[14].SetPoint(x1, y1);
		x1=f.GetKeyVal(key,"log15x",71);
		y1=f.GetKeyVal(key,"log15y",29);
		CDesk16::m_PtLog[15].SetPoint(x1, y1);
		CDesk16::m_draw[0]=0;
		CDesk16::m_draw[1]=1;
		CDesk16::m_draw[2]=2;
		CDesk16::m_draw[3]=3;
		CDesk16::m_draw[4]=4;
		CDesk16::m_draw[5]=5;
		CDesk16::m_draw[6]=6;
		CDesk16::m_draw[7]=7;
		CDesk16::m_draw[8]=8;
		CDesk16::m_draw[9]=9;
		CDesk16::m_draw[10]=10;
		CDesk16::m_draw[11]=11;
		CDesk16::m_draw[12]=12;
		CDesk16::m_draw[13]=13;
		CDesk16::m_draw[14]=14;
		CDesk16::m_draw[15]=15;

		CDesk16::m_TextFormat[0]=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_CENTER;//右
		CDesk16::m_TextFormat[1]=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_CENTER;//下
		CDesk16::m_TextFormat[2]=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_CENTER;//左
		CDesk16::m_TextFormat[3]=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_CENTER;//上
		CDesk16::m_TextFormat[4]=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_CENTER;//下
		CDesk16::m_TextFormat[5]=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_CENTER;//下
		CDesk16::m_TextFormat[6]=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_CENTER;//下
		CDesk16::m_TextFormat[7]=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_CENTER;//下
		CDesk16::m_TextFormat[8]=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_CENTER;//下
		CDesk16::m_TextFormat[9]=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_CENTER;//下
		CDesk16::m_TextFormat[10]=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_CENTER;//下
		CDesk16::m_TextFormat[11]=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_CENTER;//下
		CDesk16::m_TextFormat[12]=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_CENTER;//下
		CDesk16::m_TextFormat[13]=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_CENTER;//下
		CDesk16::m_TextFormat[14]=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_CENTER;//下
		CDesk16::m_TextFormat[15]=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_CENTER;//下
	}
	return bInit;
};

//加载默认游戏桌，在扩展桌子没有找到的情况下
bool CBaseDeskView::LoadDeskImage(BYTE bEveryCount, CGameImage & AFCDeskImage, BYTE bDeskType,CString deskClass,bool deskFlat,UINT uNameID)
{
	TCHAR szPath[MAX_PATH];
	wsprintf(szPath,".\\%d\\image\\Desk\\Desk.bmp",uNameID);
	if (!CBcfFile::IsFileExist(szPath))
	{
		wsprintf(szPath, "image\\client\\alluse\\Desk.bmp");
	}
	return AFCDeskImage.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);

}

//设置开始标志
void CBaseDeskView::SetPlayGame(bool bPlaying, CDeskFrameView * pDeskFrame)
{
	m_bPlayGame=bPlaying;
	pDeskFrame->UpdateDeskView(m_bDeskIndex);//更新游戏桌子
	return;
}

//设置盘旋
void CBaseDeskView::SetHoverIndex(BYTE bHoverIndex, CDeskFrameView * pDeskFrame) 
{
	if(!this)
		return;
	if (m_bHoverIndex!=bHoverIndex)
	{
		m_bHoverIndex=bHoverIndex; 
		pDeskFrame->UpdateDeskView(m_bDeskIndex);//更新游戏桌子
	}
	return;
}

//设置按下
void CBaseDeskView::SetMouseDown(bool bMouseDown, CDeskFrameView * pDeskFrame) 
{ 
	if(!this)
		return;
	if (m_bDownMouse!=bMouseDown)
	{
		m_bDownMouse=bMouseDown; 
		pDeskFrame->UpdateDeskView(m_bDeskIndex);//更新游戏桌子
	}
	return;
}

//设置框架
void CBaseDeskView::SetShowFrame(bool bFocusFrame, CDeskFrameView * pDeskFrame)
{ 
	if(!this)
		return;
	if (m_bFocusFrame!=bFocusFrame)
	{
		m_bFocusFrame=bFocusFrame; 
		pDeskFrame->UpdateDeskView(m_bDeskIndex);//更新游戏桌子
	} 
	return;
}

//设置锁标志
void CBaseDeskView::SetLock(bool bLock, CDeskFrameView * pDeskFrame)
{
	if (m_bLock!=bLock)
	{
		m_bLock=bLock;
		pDeskFrame->UpdateDeskView(m_bDeskIndex);//更新游戏桌子
	}
	return;
}
//是否全部同意
bool CBaseDeskView::IsAllAgree(void)
{
	return false;
}
//同意人数
BYTE CBaseDeskView::CountAgree(void)
{
	return 0;
}
void CBaseDeskView::SetBasePoint(bool bSetBasePoint, int iBasePoint, CDeskFrameView * pDeskFrame)
{
	m_bBasePoint=bSetBasePoint;
	m_iBasePoint=iBasePoint;
	pDeskFrame->UpdateDeskView(m_bDeskIndex);//更新游戏桌子
	return;
}
//设置用户信息
bool CBaseDeskView::SetUserItem(BYTE bDeskStation, UserItemStruct * pUserItem, CDeskFrameView * pDeskFrame)
{
	if (bDeskStation<MAX_PEOPLE)
	{
		m_pUserItem[bDeskStation]=pUserItem;
		pDeskFrame->UpdateDeskView(m_bDeskIndex);//更新游戏桌子
		return true;
	}
	return false;
}

//设置比赛信息
bool CBaseDeskView::SetMatchInfo(BYTE bDeskStation, long int dwUserID, TCHAR * szUserName, CDeskFrameView * pDeskFrame)
{
	if (bDeskStation<MAX_PEOPLE)
	{
		m_MatchInfo[bDeskStation].dwUserID=dwUserID;
		if (szUserName!=NULL) lstrcpy(m_MatchInfo[bDeskStation].szUserName,szUserName);
		else m_MatchInfo[bDeskStation].szUserName[0]=0;
		pDeskFrame->UpdateDeskView(m_bDeskIndex);
		return true;
	}
	return false;
}

//获取用户信息
UserItemStruct * CBaseDeskView::GetUserItem(BYTE bDeskStation)
{
	if (bDeskStation<MAX_PEOPLE) 
		return m_pUserItem[bDeskStation];
	return NULL;
}

//获取比赛信息
long int CBaseDeskView::GetMatchInfo(BYTE bDeskStation)
{
	if (bDeskStation<MAX_PEOPLE) return m_MatchInfo[bDeskStation].dwUserID;
	return 0L;
}

/*******************************************************************************************************/

//构造函数
CDeskFrameView::CDeskFrameView()
{
	m_iPos=0;				//当前位置		
	m_iMaxPos=0;			//最大位置		
	m_iRowCount=0;			//行桌子数目
	m_iPicWidth=0;			//图像宽度
	m_iPicHight=0;			//图像高度
	m_iXBasePos=0;			//X 基础点
	m_iYBasePos=0;			//Y 基础点
	m_iAllLineCount=0;		//桌子总行数
	m_iScreenLineCount=0;	//屏桌子行数
	m_uClassSize=0;			//类大小
	m_bDeskCount=0;			//游戏桌数目
	m_bEveryCount=0;		//每桌游戏人数
	m_pDeskArray=NULL;		//游戏桌数组
	m_bDownDeskIndex=255;	//按下桌子索引
	m_bDownDeskStation=255;	//按下位置索引
	m_bUserFrameIndex=255;	//用户边框索引
	m_bFindFrameIndex=255;	//查找边框索引
	m_hMouseCursor=NULL;	//光标句柄
	m_bLeftMouseDown=false;	//是否鼠标左键按下
	m_bRightMouseDown=false;//是否鼠标右键按下
	pOrderName=NULL;		//等级函数

	m_byDeskType = YH_DESK_FACE;
	m_pImgWaitSit=NULL;
	m_pImgFrame = NULL;
	m_pImgDesk = NULL;
	m_pImgLock= NULL;
	m_pImgTableBasePoint = NULL;
	m_pImgBegin= NULL;
	m_pImgAgree= NULL;
	m_pBackBmp = NULL;
	m_pGraph = NULL;
	m_bShowPlaying = false;
	m_pImageQueueTip = NULL;
	m_pImageQueueTip1 = NULL;
	m_pImageQueueTip2 = NULL;
	m_pImageQueueTime = NULL;
	m_pImageQueueplan = NULL;
	m_SumTime = 0;
	m_CurTime = 0;

	m_pRoom = NULL;

	m_bmp = new HBITMAP;
}
//析构函数
CDeskFrameView::~CDeskFrameView()
{
	m_BJLBegin.DestroyWindow();
	m_BJLPlaying.DestroyWindow();
	m_BeginAgree.DestroyWindow();

	m_QueueBegin.DestroyWindow();
	m_QueueStop.DestroyWindow();

	delete m_pImgWaitSit;
	m_pImgWaitSit=NULL;
	delete m_pImgFrame;	
	m_pImgFrame = NULL;
	delete m_pImgDesk;
	m_pImgDesk = NULL;
	delete m_pImgLock;
	m_pImgLock= NULL;
	delete m_pImgTableBasePoint;
	m_pImgTableBasePoint = NULL;
	delete m_pImgBegin;
	m_pImgBegin= NULL;
	delete m_pImgAgree;
	m_pImgAgree= NULL;
	UnInitDeskFrame();
	if (m_hMouseCursor!=NULL) DestroyCursor(m_hMouseCursor);//小手形状的鼠标光标
	delete m_pBackBmp;
	m_pBackBmp = NULL;
	delete m_pGraph;
	m_pGraph = NULL;
	delete m_pImageQueueTip;
	m_pImageQueueTip = NULL;
	delete m_pImageQueueTip1;
	m_pImageQueueTip1 = NULL;
	delete m_pImageQueueTip2;
	m_pImageQueueTip2 = NULL;
	delete m_pImageQueueTime;
	m_pImageQueueTime = NULL;
	delete m_pImageQueueplan;
	m_pImageQueueplan = NULL;
	
	delete m_bmp;
}

//建立函数
bool CDeskFrameView::CreateDeskFrame(CWnd * pParentWnd, UINT uWndID)
{
	if (Create(NULL,NULL,WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,
		CRect(0,0,0,0),pParentWnd,uWndID)==FALSE) return false;
	m_hMouseCursor=LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_HAND));//鼠标经过时变成小手形状的鼠标光标，IDC_HAND是改资源的ID
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);
	m_BeginAgree.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE|BS_OWNERDRAW,CRect(0,0,75,25),this,IDC_BEGINAGREE);
	TCHAR szPath[MAX_PATH];
	///使用公共图片
	wsprintf(szPath, "image\\client\\alluse\\start_bt.bmp",this->pGameInfo->uNameID);
	m_BeginAgree.LoadButtonBitmap(szPath,false);

	CGameImage img;
	img.SetLoadInfo(szPath,true);

	CGameImageHelper ImageHandle(&img);
	HRGN hRgn=AFCBmpToRgn(ImageHandle,img.GetPixel(0,0),RGB(1,1,1));
	if (hRgn!=NULL)
	{
		m_BeginAgree.SetWindowRgn(hRgn,TRUE);
		///{{  释放区域对象
		DeleteObject(hRgn);
		///释放区域对象}}
	}
	
	m_BeginAgree.ShowWindow(FALSE);
	LoadBtnAgree(m_deskClass);

	//百家乐开始按钮
	m_BJLBegin.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE|BS_OWNERDRAW,CRect(0,0,75,25),this,IDC_BJLBEGIN);
	wsprintf(szPath,".\\%d\\image\\Desk\\EnterGame.bmp",this->pGameInfo->uNameID);
	m_BJLBegin.LoadButtonBitmap(szPath,false);
	m_BJLBegin.ShowWindow(SW_HIDE);

	m_BJLPlaying.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE|BS_OWNERDRAW,CRect(0,0,75,25),this,IDC_BJLPLAYING);
	wsprintf(szPath,".\\%d\\image\\Desk\\playingGameBt.bmp",this->pGameInfo->uNameID);
	m_BJLPlaying.LoadButtonBitmap(szPath,false);
	//m_BJLPlaying.EnableWindow(FALSE);
	m_BJLPlaying.ShowWindow(SW_HIDE);
	//end of 百家乐开始按钮

	m_QueueBegin.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE|BS_OWNERDRAW,CRect(0,0,136,51),this,IDC_QUEUEBEGIN);
	wsprintf(szPath,"image\\client\\alluse\\queue\\QueueBegin.bmp");
	m_QueueBegin.LoadButtonBitmap(szPath,false);
	m_QueueBegin.ShowWindow(SW_HIDE);

	m_QueueStop.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE|BS_OWNERDRAW,CRect(0,0,136,51),this,IDC_QUEUESTOP);
	wsprintf(szPath,"image\\client\\alluse\\queue\\QueueStop.bmp");
	m_QueueStop.LoadButtonBitmap(szPath,false);
	m_QueueStop.ShowWindow(SW_HIDE);

	return true;
}


//desc:显示百家乐开始按钮
void CDeskFrameView::ShowBJLBeginBtn()
{
	if (pGameInfo)
	{
		//if (HUNDRED_PEOPLE_DESK_GAME_NAME == pGameInfo->uNameID		//百家乐
		//	||SUOHA_PEOPLE_DESK_GAME_NAME == pGameInfo->uNameID			//百变梭哈
		//	||NIUNIU_PEOPLE_DESK_GAME_NAME == pGameInfo->uNameID		//百变牛牛
		//	||ERZHANGPAI_PEOPLE_DESK_GAME_NAME == pGameInfo->uNameID )	//百变二张牌)// add by ramon 09.05 百家乐)
		char cKey[10];
		CString sPath=CBcfFile::GetAppPath (false);
		CBcfFile fsr( sPath + "SpecialRule.bcf");
		sprintf(cKey, "%d", pGameInfo->uNameID);
		int iResult = fsr.GetKeyVal(_T("BJLType"), cKey, 0);

		if (iResult)
		{
			if(m_BJLBegin.GetSafeHwnd()!=NULL && !m_bShowPlaying)
			{
				m_BJLPlaying.ShowWindow(SW_HIDE);
				m_BJLBegin.ShowWindow(SW_SHOW);
			}
			if (m_BJLPlaying.GetSafeHwnd()!=NULL && m_bShowPlaying)//游戏进行中按钮
			{
				m_BJLBegin.ShowWindow(SW_HIDE);
				m_BJLPlaying.EnableWindow(FALSE);
				m_BJLPlaying.ShowWindow(SW_SHOW);				
			}
		}
	}
}

//百家乐 add by wlr 20090717
//desc:是显示进入游戏按钮还是显示游戏进行中按钮
void CDeskFrameView::showBJLBeginOrPlayingBtn(bool bShowPlaying)
{
	m_BJLPlaying.EnableWindow(TRUE);
	m_BJLBegin.EnableWindow(TRUE);
	if (bShowPlaying)
	{
		m_BJLBegin.ShowWindow(SW_HIDE);
		m_BJLPlaying.EnableWindow(FALSE);
		m_BJLPlaying.ShowWindow(SW_SHOW);
		m_bShowPlaying = true;
	}
	else
	{
		m_BJLPlaying.ShowWindow(SW_HIDE);
		m_BJLBegin.ShowWindow(SW_SHOW);
		m_bShowPlaying = false;
	}
	//m_BJLBegin.UpdateWindow();
	//m_BJLPlaying.UpdateWindow();

	return;
}
//end of 百家乐 add by wlr 20090717


//排队机显示排队和离开
void CDeskFrameView::showQueueBtn(bool bShowPlaying)
{
	m_QueueBegin.EnableWindow(TRUE);
	m_QueueStop.EnableWindow(TRUE);
	if (bShowPlaying)
	{
		m_QueueBegin.ShowWindow(SW_HIDE);
		m_QueueBegin.EnableWindow(FALSE);
		m_QueueStop.ShowWindow(SW_SHOW);
		m_bQueueBegin = true;
	}
	else
	{
		m_QueueBegin.ShowWindow(SW_HIDE);
		m_QueueBegin.EnableWindow(FALSE);
		m_QueueStop.ShowWindow(SW_HIDE);
		m_QueueStop.EnableWindow(FALSE);
		m_bQueueBegin = false;
	}
	//m_BJLBegin.UpdateWindow();
	//m_BJLPlaying.UpdateWindow();

	return;
}

void CDeskFrameView::OnHitQueueBegin()
{
	//AFCMessageBox("OnHitQueueBegin");
	DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","OnHitQueueBegin");
}

void CDeskFrameView::OnHitQueueStop()
{
	CWnd* wnd=GetParent();
	if(wnd!=NULL)
		::PostMessage(wnd->GetSafeHwnd(),WM_HIT_QUEUE_STOP,0,0);
}


void CDeskFrameView::ShowQueueTime(int x, int y, int inumber)
{
	int w = m_pImageQueueTime->GetWidth() / 10;
	int h = m_pImageQueueTime->GetHeight();

	char stnumber[128];
	ZeroMemory(stnumber, sizeof(stnumber));
	sprintf(stnumber, "%d", inumber);
	string _time(stnumber);

	for (int i = 0; i < _time.size(); i++)
	{
		m_pGraph->DrawImage(m_pImageQueueTime, x + i * (w + 5), y, (_time[i] - '0') * w, 0, w, h, UnitPixel);
	}
}

void CDeskFrameView::ShowQueuePlan(int x, int y, int iPlan)
{
	int w = m_pImageQueueplan->GetWidth() / 11;
	int h = m_pImageQueueplan->GetHeight();

	int _p = iPlan / 10;

	for (int i = 0; i < 10; i++)
		m_pGraph->DrawImage(m_pImageQueueplan, x + i * (w + 6), y, 10 * w, 0, w, h, UnitPixel);

	//m_pGraph->DrawImage(m_pImageQueueplan, x, y, 0, 0, w * 10, h, UnitPixel);
	for (int i = 0; i < _p; i++)
	{
		m_pGraph->DrawImage(m_pImageQueueplan, x + i * (w + 6), y, i * w, 0, w, h, UnitPixel);
	}
}

void CDeskFrameView::SetQueueTime(int iTime, bool setType)
{
	if (setType)
		m_SumTime = iTime;
	m_CurTime = iTime;



	//CRect rect,ClientRect;
	//GetClientRect(&ClientRect);
	//int iWidth = m_pImageQueueTip1->GetWidth();
	//int iHeight = m_pImageQueueTip1->GetHeight();
	//m_YTip = 20;
	//m_YTip1 = m_YTip + iHeight + 20;
	//int x = (ClientRect.Width() - iWidth) / 2;

	//ShowQueueTime(x + iWidth / 2 + 20, m_YTip1 + 10, m_CurTime);
	//ShowQueuePlan(x + iWidth / 2 + 20, m_YTip1 + 48, ((m_SumTime-m_CurTime) * 1.0) / (1.0 * m_SumTime) * 100);

	//CRect DeskRect(x, m_YTip1, iWidth, iHeight);
	//InvalidateRect(&DeskRect,TRUE);
	Invalidate();

	KillTimer(TIME_QUEUE);
	SetTimer(TIME_QUEUE, 1000, NULL);

}

//加载坐标
void CDeskFrameView::LoadBtnAgree(CString deskClass)
{
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CString path;
	// 同时支持两种方式游戏客户端
	// 要判断到底是ico还是exe，
	bool bIsIco = (NULL == strstr(pGameInfo->szProcessName, ".exe"));
	if (bIsIco)
	{
		path.Format("config\\%s\\deskconfig.bcf",deskClass); 
	}
	else
	{
		path.Format(".\\%d\\config\\%s\\deskconfig.bcf",pGameInfo->uNameID,deskClass); 
	}

	if (!CBcfFile::IsFileExist(s+path))
	{
		return;
	}
	CBcfFile f( s + path);
	CString key="BtnAgree";
	m_BtnAgree0X=f.GetKeyVal(key,"BtnAgree0X",75);
	m_BtnAgree0Y=f.GetKeyVal(key,"BtnAgree0Y",75);
	m_BtnAgree1X=f.GetKeyVal(key,"BtnAgree1X",75);
	m_BtnAgree1Y=f.GetKeyVal(key,"BtnAgree1Y",75);
	m_BtnAgree2X=f.GetKeyVal(key,"BtnAgree2X",75);
	m_BtnAgree2Y=f.GetKeyVal(key,"BtnAgree2Y",75);
	m_BtnAgree3X=f.GetKeyVal(key,"BtnAgree3X",75);
	m_BtnAgree3Y=f.GetKeyVal(key,"BtnAgree3Y",75);
	return ;
}
/// 初始化桌子桌子内存，各相关图片，PNG格式
bool CDeskFrameView::InitDeskFramePng(BYTE bDeskCount, BYTE bEveryCount,
								   const wchar_t * pFrameImage,
								   const wchar_t * pWaitSitImage,
								   const wchar_t * pDeskImage,
								   const wchar_t * pLockImage,
								   const wchar_t * pBasePoint,
								   const wchar_t * pBeginImage,
								   const wchar_t * pAgreeImage,
								   BYTE bDeskType,int uComType,
								   GameInfoStruct *GameInfo,
								   CGameRoomEx* pRoom,
								   CString deskClass,
								   bool deskFlat,
								   COLORREF iclrSitDeskFrameLeftTop,
								   COLORREF iclrSitDeskFrameRightBottom,
								   bool bIsVip)
{
	m_bIsVip = bIsVip;
	// {{ 从配置文件中读取渐变颜色值
	/// 先从游戏自己的目录下读取
	CString strPrivatePath;
	CString s = CBcfFile::GetAppPath();

	strPrivatePath.Format("%s%d\\DeskBackGround.bcf", s, GameInfo->uNameID);
	CString path;
	if (CBcfFile::IsFileExist(strPrivatePath))
	{
		path = strPrivatePath;
	}
	else
	{
		path.Format("%s\\config\\%s\\deskconfig.bcf",s,deskClass);
	}

	m_pRoom = pRoom;
	m_personalInfo.m_roomWnd = pRoom;

#ifndef BZ_ZLIB
	if (CBcfFile::IsFileExist(path))
	{
		CBcfFile fbcf(path);
		m_r1 = fbcf.GetKeyVal("BackGround", "Color1_R", 255);
		m_g1 = fbcf.GetKeyVal("BackGround", "Color1_G", 255);
		m_b1 = fbcf.GetKeyVal("BackGround", "Color1_B", 204);
		m_r2 = fbcf.GetKeyVal("BackGround", "Color2_R", 204);
		m_g2 = fbcf.GetKeyVal("BackGround", "Color2_G", 153);
		m_b2 = fbcf.GetKeyVal("BackGround", "Color2_B", 51);
		// 从配置文件中读取渐变颜色值}}
	}
#else
	CBcfFile fbcf(path);
	m_r1 = fbcf.GetKeyVal("BackGround", "Color1_R", 255);
	m_g1 = fbcf.GetKeyVal("BackGround", "Color1_G", 255);
	m_b1 = fbcf.GetKeyVal("BackGround", "Color1_B", 204);
	m_r2 = fbcf.GetKeyVal("BackGround", "Color2_R", 204);
	m_g2 = fbcf.GetKeyVal("BackGround", "Color2_G", 153);
	m_b2 = fbcf.GetKeyVal("BackGround", "Color2_B", 51);
#endif
	


	// 兼容原有.ico模式，先判断是否为ICO模式
	bool bIsIco = (NULL == strstr(GameInfo->szProcessName, ".exe"));

	m_UserID=0;
	m_uComType=uComType;
	pGameInfo=GameInfo;
	m_bIsClick=false;
	m_deskClass=deskClass;
	m_deskFlat=deskFlat;
	m_Tooltip.Create(this);
	m_Tooltip.SetDelayTime(50);
	clrSitDeskFrameLeftTop=iclrSitDeskFrameLeftTop;
	clrSitDeskFrameRightBottom=iclrSitDeskFrameRightBottom;

	//清理数据
	UnInitDeskFrame();
	//生成数据
	CBaseDeskView * pDeskArray=NULL;

	//为桌子申请内存
	switch (bEveryCount)
	{
	case 1:///////一把椅子的桌子
		{
			if (bDeskType==YH_DESK_FACE) { pDeskArray=new CDesk1 [bDeskCount]; m_uClassSize=sizeof(CDesk1); }
			//if (bDeskType==EX_DESK_FACE) { pDeskArray=new CDeskEx1 [bDeskCount]; m_uClassSize=sizeof(CDeskEx2); }
			break;
		}
	case 2:///////两把椅子的桌子
		{
			if (bDeskType==YH_DESK_FACE) { pDeskArray=new CDesk2 [bDeskCount]; m_uClassSize=sizeof(CDesk2); }
			if (bDeskType==EX_DESK_FACE) { pDeskArray=new CDeskEx2 [bDeskCount]; m_uClassSize=sizeof(CDeskEx2); }

			break;
		}
	case 3:///////三把椅子的桌子
		{
			if (bDeskType==YH_DESK_FACE) { pDeskArray=new CDesk3 [bDeskCount]; m_uClassSize=sizeof(CDesk3); }
			if (bDeskType==EX_DESK_FACE) { pDeskArray=new CDeskEx3 [bDeskCount]; m_uClassSize=sizeof(CDeskEx3); }
			break;
		}
	case 4:///////四把椅子的桌子
		{
			if (bDeskType==YH_DESK_FACE) { pDeskArray=new CDesk4 [bDeskCount]; m_uClassSize=sizeof(CDesk4); }
			if (bDeskType==EX_DESK_FACE) { pDeskArray=new CDeskEx4 [bDeskCount]; m_uClassSize=sizeof(CDeskEx4); }
			break;
		}
	case 5:
		{
			if (bDeskType==YH_DESK_FACE) { pDeskArray=new CDesk5 [bDeskCount]; m_uClassSize=sizeof(CDesk5); }
			if (bDeskType==EX_DESK_FACE) { pDeskArray=new CDeskEx5 [bDeskCount]; m_uClassSize=sizeof(CDeskEx5); }
			break;
		}
	case 8:
		{
			if (bDeskType==YH_DESK_FACE) { pDeskArray=new CDesk8 [bDeskCount]; m_uClassSize=sizeof(CDesk8); }
			if (bDeskType==EX_DESK_FACE) { pDeskArray=new CDeskEx8 [bDeskCount]; m_uClassSize=sizeof(CDeskEx8); }
			break;
		}
	case 16:
		{
			if (bDeskType==YH_DESK_FACE) { pDeskArray=new CDesk16 [bDeskCount]; m_uClassSize=sizeof(CDesk16); }
			if (bDeskType==EX_DESK_FACE) { pDeskArray=new CDeskEx16 [bDeskCount]; m_uClassSize=sizeof(CDeskEx16); }
			break;
		}
	default:
		{
			if (bDeskType==YH_DESK_FACE) { pDeskArray=new CDesk6 [bDeskCount]; m_uClassSize=sizeof(CDesk6); }
			if (bDeskType==EX_DESK_FACE) { pDeskArray=new CDeskEx6 [bDeskCount]; m_uClassSize=sizeof(CDeskEx6); }
			break;
		}
	}
	//百家乐
	if (pDeskArray==NULL)
	{
		if (bDeskType==BJL_DESK_FACE) 
		{ 
			pDeskArray	=new CDeskBJL [bDeskCount]; 
			m_uClassSize=sizeof(CDeskBJL);
		}
	}
	//end of 百家乐

	if (pDeskArray==NULL)
	{
		UnInitDeskFrame();
		//AFCMessageBox("游戏初始化失败，请访问http://www.hn78.com联系开发者！");
	    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","游戏初始化失败，请访问http://www.hn78.com联系开发者！");
		return false;
	}
	//设置数据
	m_pDeskArray=pDeskArray;//游戏桌数组
	m_bDeskCount=bDeskCount;
	m_bEveryCount=bEveryCount;

	if (pWaitSitImage!=NULL)
	{
		m_pImgWaitSit = new Image(pWaitSitImage, FALSE);
	}
	if (pFrameImage!=NULL)
	{
		m_pImgFrame = new Image(pFrameImage, FALSE);
	}
	//初始化桌子图片
	if (pDeskImage!=NULL)
	{
#ifdef BZ_ZLIB
		CUnZipRes unZipRes;
		IStream * pStream=NULL;
		
		int len = WideCharToMultiByte(CP_ACP, 0, pDeskImage, wcslen(pDeskImage), NULL, 0,NULL,NULL);
		TCHAR * tchar=new char[len+1];
		//宽字节编码转换成多字节编码   
		WideCharToMultiByte(CP_ACP, 0, pDeskImage, wcslen(pDeskImage), tchar, len, NULL, NULL); 
		tchar[len]='\0';

		pStream=unZipRes.LoadFileResFromZip(tchar);
		m_pImgDesk=new Image(pStream);
		if(pStream!=NULL)
		{
			pStream->Release();
			pStream=NULL;
		}
		
		if(tchar!=NULL)
		{
			delete []tchar;
			tchar=NULL;
		}

#else
		m_pImgDesk = new Image(pDeskImage, FALSE);
#endif
	}
	else
	{
		UnInitDeskFrame();
	    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","载入桌子图片失败");
		return false;
	}
	if (m_pImgDesk == NULL) 
	{
		UnInitDeskFrame();
		return false;
	}

	TCHAR szPath[MAX_PATH]={0};
	//初始化锁图
	if (pLockImage!=NULL)
	{
#ifdef BZ_ZLIB 
		CUnZipRes unZipRes;
		IStream * pStream=NULL;
		
		int len = WideCharToMultiByte(CP_ACP, 0, pLockImage, wcslen(pLockImage), NULL, 0,NULL,NULL);
		TCHAR * tchar=new char[len+1];
		//宽字节编码转换成多字节编码   
		WideCharToMultiByte(CP_ACP, 0, pLockImage, wcslen(pLockImage), tchar, len, NULL, NULL); 
		tchar[len]='\0';
		//CString strFileName(tchar);
		//int pos=strFileName.Find("image");
		//strFileName=strFileName.Right(strFileName.GetLength()-pos);

		//TCHAR * pchar=strFileName.GetBuffer(strFileName.GetLength() + 1);

		pStream=unZipRes.LoadFileResFromZip(tchar);
		m_pImgLock=new Image(pStream);
		if(pStream!=NULL)
		{
			pStream->Release();
			pStream=NULL;
		}
		
		if(tchar!=NULL)
		{
			delete []tchar;
			tchar=NULL;
		}

#else
		m_pImgLock = new Image(pLockImage, FALSE);
#endif

	}
	else 
	{
		if (bDeskType!=BJL_DESK_FACE)
		{
			UnInitDeskFrame();
			//AFCMessageBox("载入锁桌图片失败");
	        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","载入锁桌图片失败");
			return false;
		}
	}

	//初始化桌子倍数图
	if (pBasePoint!=NULL)
	{
#ifdef BZ_ZLIB   //add by wxx 
		CUnZipRes unZipRes;
		IStream * pStream=NULL;
		
		int len = WideCharToMultiByte(CP_ACP, 0, pBasePoint, wcslen(pBasePoint), NULL, 0,NULL,NULL); 
		TCHAR * tchar=new char[len+1];
		//宽字节编码转换成多字节编码   
		WideCharToMultiByte(CP_ACP, 0, pBasePoint, wcslen(pBasePoint), tchar, len, NULL, NULL); 
		tchar[len]='\0';
		
		pStream=unZipRes.LoadFileResFromZip(tchar);
		m_pImgTableBasePoint=new Image(pStream);
		if(pStream!=NULL)
		{
			pStream->Release();
			pStream=NULL;
		}
		
		if(tchar!=NULL)
		{
			delete []tchar;
			tchar=NULL;
		}

#else
		m_pImgTableBasePoint = new Image(pBasePoint, FALSE);
#endif
	}
	else 
	{
		m_pImgTableBasePoint = NULL;
	}

	//初始化开始游戏桌图
	if (pBeginImage!=NULL) 
	{
#ifdef BZ_ZLIB   //add by wxx 
		CUnZipRes unZipRes;
		IStream * pStream=NULL;
		
		int len = WideCharToMultiByte(CP_ACP, 0, pBeginImage, wcslen(pBeginImage), NULL, 0,NULL,NULL);
		TCHAR * tchar=new char[len+1];
		//宽字节编码转换成多字节编码   
		WideCharToMultiByte(CP_ACP, 0, pBeginImage, wcslen(pBeginImage), tchar, len, NULL, NULL); 
		tchar[len]='\0';

		pStream=unZipRes.LoadFileResFromZip(tchar);
		m_pImgBegin=new Image(pStream);
		if(pStream!=NULL)
		{
			pStream->Release();
			pStream=NULL;
		}
		
		if(tchar!=NULL)
		{
			delete []tchar;
			tchar=NULL;
		}

#else
		m_pImgBegin = new Image(pBeginImage, FALSE);
#endif
	}
	else 
	{
		if (bDeskType!=BJL_DESK_FACE)
		{
			UnInitDeskFrame();
	        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","载入开始游戏桌图失败");
			return false;
		}
	}
		
	//初始化同意图
	if (pAgreeImage!=NULL)
	{
#ifdef BZ_ZLIB   //add by wxx 
		CUnZipRes unZipRes;
		IStream * pStream=NULL;
		
		int len = WideCharToMultiByte(CP_ACP, 0, pAgreeImage, wcslen(pAgreeImage), NULL, 0,NULL,NULL); 
		TCHAR * tchar=new char[len+1];
		//宽字节编码转换成多字节编码   
		WideCharToMultiByte(CP_ACP, 0, pAgreeImage, wcslen(pAgreeImage), tchar, len, NULL, NULL); 
		tchar[len]='\0';

		pStream=unZipRes.LoadFileResFromZip(tchar);
		m_pImgAgree=new Image(pStream);
		if(pStream!=NULL)
		{
			pStream->Release();
			pStream=NULL;
		}
		if(tchar!=NULL)
		{
			delete []tchar;
			tchar=NULL;
		}

#else
		m_pImgAgree = new Image(pAgreeImage, FALSE);
#endif
	}
	else 
	{
		if (bDeskType!=BJL_DESK_FACE)
		{
			UnInitDeskFrame();
	        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","载入同意图失败");
			return false;
		}
	}	

	wchar_t wcsQueueTip[MAX_PATH];
	wsprintfW(wcsQueueTip, L".\\image\\client\\alluse\\queue\\Queue_BK_Tip.png");
	if (GameInfo->dwRoomRule & GRR_CONTEST)
	{
		wsprintfW(wcsQueueTip, L".\\image\\client\\alluse\\queue\\Contest_BK_Tip.png");
	}
	
	//初始化同意图
	//if (m_pImageQueueTip!=NULL)
	{
#ifdef BZ_ZLIB
		CUnZipRes unZipRes;
		IStream * pStream=NULL;
		
		int len = WideCharToMultiByte(CP_ACP, 0, wcsQueueTip, wcslen(wcsQueueTip), NULL, 0,NULL,NULL); 
		TCHAR * tchar=new char[len+1];
		//宽字节编码转换成多字节编码
		WideCharToMultiByte(CP_ACP, 0, wcsQueueTip, wcslen(wcsQueueTip), tchar, len, NULL, NULL); 
		tchar[len]='\0';

		pStream=unZipRes.LoadFileResFromZip(tchar);
		m_pImageQueueTip=new Image(pStream);
		if(pStream!=NULL)
		{
			pStream->Release();
			pStream=NULL;
		}
		if(tchar!=NULL)
		{
			delete []tchar;
			tchar=NULL;
		}

#else
		m_pImageQueueTip = new Image(wcsQueueTip, FALSE);
#endif
	}
	
	wchar_t wcsQueueTip1[MAX_PATH];
	wsprintfW(wcsQueueTip1, L".\\image\\client\\alluse\\queue\\Queue_BK_Tip1.png");

	wchar_t wcsQueueTip2[MAX_PATH];
	wsprintfW(wcsQueueTip2, L".\\image\\client\\alluse\\queue\\seat_tip.png");
	//初始化同意图
	//if (m_pImageQueueTip!=NULL)
	{
#ifdef BZ_ZLIB
		CUnZipRes unZipRes;
		IStream * pStream=NULL;
		
		int len = WideCharToMultiByte(CP_ACP, 0, wcsQueueTip1, wcslen(wcsQueueTip1), NULL, 0,NULL,NULL); 
		TCHAR * tchar=new char[len+1];
		//宽字节编码转换成多字节编码   
		WideCharToMultiByte(CP_ACP, 0, wcsQueueTip1, wcslen(wcsQueueTip1), tchar, len, NULL, NULL); 
		tchar[len]='\0';

		pStream=unZipRes.LoadFileResFromZip(tchar);
		m_pImageQueueTip1=new Image(pStream);
		if(pStream!=NULL)
		{
			pStream->Release();
			pStream=NULL;
		}
		if(tchar!=NULL)
		{
			delete []tchar;
			tchar=NULL;
		}

		len = WideCharToMultiByte(CP_ACP, 0, wcsQueueTip2, wcslen(wcsQueueTip2), NULL, 0,NULL,NULL); 
		TCHAR * tchar=new char[len+1];
		//宽字节编码转换成多字节编码   
		WideCharToMultiByte(CP_ACP, 0, wcsQueueTip2, wcslen(wcsQueueTip2), tchar, len, NULL, NULL); 
		tchar[len]='\0';

		pStream=unZipRes.LoadFileResFromZip(tchar);
		m_pImageQueueTip2=new Image(pStream);
		if(pStream!=NULL)
		{
			pStream->Release();
			pStream=NULL;
		}
		if(tchar!=NULL)
		{
			delete []tchar;
			tchar=NULL;
		}

#else
		m_pImageQueueTip1 = new Image(wcsQueueTip1, FALSE);
		m_pImageQueueTip2 = new Image(wcsQueueTip2, FALSE);
#endif
	}



	wchar_t wcsQueueTime[MAX_PATH];
	wsprintfW(wcsQueueTime, L".\\image\\client\\alluse\\queue\\queueTime.png");
	//初始化同意图
	//if (m_pImageQueueTip!=NULL)
	{
#ifdef BZ_ZLIB
		CUnZipRes unZipRes;
		IStream * pStream=NULL;
		
		int len = WideCharToMultiByte(CP_ACP, 0, wcsQueueTime, wcslen(wcsQueueTime), NULL, 0,NULL,NULL); 
		TCHAR * tchar=new char[len+1];
		//宽字节编码转换成多字节编码   
		WideCharToMultiByte(CP_ACP, 0, wcsQueueTime, wcslen(wcsQueueTime), tchar, len, NULL, NULL); 
		tchar[len]='\0';
		pStream=unZipRes.LoadFileResFromZip(tchar);
		m_pImageQueueTime=new Image(pStream);
		if(pStream!=NULL)
		{
			pStream->Release();
			pStream=NULL;
		}
		if(tchar!=NULL)
		{
			delete []tchar;
			tchar=NULL;
		}

#else
		m_pImageQueueTime = new Image(wcsQueueTime, FALSE);
#endif
	}
	
	wchar_t wcsQueuePlan[MAX_PATH];
	wsprintfW(wcsQueuePlan, L".\\image\\client\\alluse\\queue\\queuePlan.png");
	//初始化同意图
	//if (m_pImageQueueTip!=NULL)
	{
#ifdef BZ_ZLIB 
		CUnZipRes unZipRes;
		IStream * pStream=NULL;
		
		int len = WideCharToMultiByte(CP_ACP, 0, wcsQueuePlan, wcslen(wcsQueuePlan), NULL, 0,NULL,NULL); 
		TCHAR * tchar=new char[len+1];
		//宽字节编码转换成多字节编码   
		WideCharToMultiByte(CP_ACP, 0, wcsQueuePlan, wcslen(wcsQueuePlan), tchar, len, NULL, NULL); 
		tchar[len]='\0';

		pStream=unZipRes.LoadFileResFromZip(tchar);
		m_pImageQueueplan=new Image(pStream);
		if(pStream!=NULL)
		{
			pStream->Release();
			pStream=NULL;
		}
		if(tchar!=NULL)
		{
			delete []tchar;
			tchar=NULL;
		}

#else
		m_pImageQueueplan = new Image(wcsQueuePlan, FALSE);
#endif
	}
	
	m_pImgDesk->GetHeight();
	m_iPicWidth= m_pImgDesk->GetWidth();
	m_iPicHight=m_pImgDesk->GetHeight();
	for (UINT i=0;i<bDeskCount;i++)
	{
		GetDeskArrayPtr(i)->SetDeskClass(deskClass,deskFlat);
		GetDeskArrayPtr(i)->SetDeskIndex(i);
		GetDeskArrayPtr(i)->SetComType(uComType);
	}

	if (GameInfo->dwRoomRule & GRR_QUEUE_GAME)
		SetTimer(TIME_QUEUE, 1000, NULL);

	return true;
};

//取消初始化
bool CDeskFrameView::UnInitDeskFrame()
{
	m_iPos=0;				
	m_iMaxPos=0;			
	m_iRowCount=0;		
	m_iPicWidth=0;
	m_iPicHight=0;
	m_iAllLineCount=0;	
	m_iScreenLineCount=0;	
	m_uClassSize=0;
	m_bDeskCount=0;
	m_bEveryCount=0;
	delete [] m_pDeskArray;
	m_pDeskArray=NULL;
	m_DeskImage.Detach();


	return true;
}

void CDeskFrameView::SetBasePoint(BYTE bDeskIndex,int iBasePoint,bool bSet)
{
	CBaseDeskView * pDesk=GetDeskArrayPtr(bDeskIndex);
	if(pDesk!=NULL) pDesk->SetBasePoint(bSet,iBasePoint,this);
}

//设置桌子接受输入
void CDeskFrameView::SetCanInput(BYTE bDeskIndex, bool bCanInput)
{
	CBaseDeskView * pDesk=GetDeskArrayPtr(bDeskIndex);
	if(pDesk!=NULL) pDesk->SetCanInput(bCanInput);
}
//获取游戏桌指针
CBaseDeskView * CDeskFrameView::GetDeskArrayPtr(UINT uIndex)
{
	if ((m_pDeskArray!=NULL)&&(uIndex<m_bDeskCount))
	{
		return ((CBaseDeskView *)((BYTE *)m_pDeskArray+uIndex*m_uClassSize));
	}
	return NULL;
}
//设置视图桌子
void CDeskFrameView::SetViewDesk(BYTE bDeskIndex)
{
	if (bDeskIndex<m_bDeskCount)
	{
		//调整数据
		int iLastPos=m_iPos;
		int iXPos=(bDeskIndex%m_iRowCount)*m_iPicWidth;
		int iYPos=(bDeskIndex/m_iRowCount)*m_iPicHight-m_iPos+m_iYBasePos;
		CRect DeskRect,ClientRect;
		GetClientRect(&ClientRect);
		DeskRect.SetRect(iXPos,iYPos,iXPos+m_iPicWidth,iYPos+m_iPicHight);
		if ((DeskRect.bottom>ClientRect.bottom)||(DeskRect.top<0)) m_iPos+=DeskRect.bottom-ClientRect.bottom;
		m_iPos=__max(0,__min(m_iPos,m_iMaxPos-m_iScreenLineCount*m_iPicHight));
		//调整位置
		if (iLastPos!=m_iPos)
		{
			SetScrollPos(SB_VERT,m_iPos);
			ScrollWindow(0,iLastPos-m_iPos,NULL,NULL);
		}
	}

	return;
}
void CDeskFrameView::OnPaint()
{
	if (NULL == GetSafeHwnd())
	{
		return;
	}
	OnPaintPng();
}
/// 用于PNG图片的重画函数
void CDeskFrameView::OnPaintPng()
{
	//绘画背景
	CPaintDC dc(this);
	//Sleep(1000);

	CRect ClientRect,ClipRect;
	dc.GetClipBox(&ClipRect);
	GetClientRect(&ClientRect);
	Gdiplus::RectF rcClipF(ClipRect.left, ClipRect.top, ClipRect.Width(), ClipRect.Height());

	int nWidth = ClientRect.Width(); 
	int nHeight = ClientRect.Height();
	if (m_pBackBmp==NULL)
	{
		return;
	}
	// 真正写显卡的操作只有一次
	Graphics graphThis(dc.GetSafeHdc());
	Rect rcBG(0, -m_iPos, ClientRect.Width(), ClientRect.Height() + m_iMaxPos);
	m_pGraph->SetClip(rcClipF);

	if ((m_r1 == m_r2) 
		||(m_g1 == m_g2)
		||(m_b1 == m_b2)
		)
	{
		SolidBrush solidBrush(Color(255, m_r1, m_g1, m_b1));  //   第一种颜色
		m_pGraph->FillRectangle(&solidBrush, Rect(0, 0, ClientRect.Width(), ClientRect.Height()));
	}
	else
	{
		//渐变色背景
		LinearGradientBrush linGrBrush(rcBG,  //   绘制区域
			Color(255, m_r1, m_g1, m_b1),  //   第一种颜色
			Color(255, m_r2, m_g2, m_b2),  //   第二种颜色
			90);  //   渐变色的角度
		m_pGraph->FillRectangle(&linGrBrush, Rect(0, 0, ClientRect.Width(), ClientRect.Height()));
	}

    ImageAttributes  imAtt;
    imAtt.SetWrapMode(WrapModeClamp);    //平铺图片


	    /////////////平铺背景图
	if (!m_pRoom->IsBJLRoom())
	{
		Bitmap  backgroundtop(L".\\image\\client\\alluse\\queue\\bk.bmp");   //加载PNG图片
		TextureBrush brush(&backgroundtop, WrapModeTile/*FlipXY*/ );
		m_pGraph->FillRectangle(&brush, RectF(0.0f, 0.0f,ClientRect.Width(),ClientRect.Height()));
	}
    
	CBaseDeskView * pDesk=NULL;
	CRect IntersectRect,DeskRect;	

	//只有一张桌子，居中显示
	if ((m_byDeskType==BJL_DESK_FACE) || (1 == m_bDeskCount && m_deskClass != "queue" ))//)
	{
		pDesk=GetDeskArrayPtr(0);
		if (pDesk)
		{
			DeskRect.SetRect((ClientRect.Width() - m_iPicWidth)/2,
				(ClientRect.Height() - m_iPicHight)/2,
				(ClientRect.Width() - m_iPicWidth)/2+m_iPicWidth,
				(ClientRect.Height() - m_iPicHight)/2+m_iPicHight);
			/// 如果在游戏中，则显示游戏中的桌面图片
			if (pDesk->IsPlayGame())
			{
				m_pGraph->DrawImage(m_pImgBegin, DeskRect.left, DeskRect.top, DeskRect.Width(), DeskRect.Height());
			}
			else /// 不在游戏中，则显示平常桌面图片 
			{
				m_pGraph->DrawImage(m_pImgDesk, DeskRect.left, DeskRect.top, DeskRect.Width(), DeskRect.Height());
			}

			if(m_BJLBegin.GetSafeHwnd()!=NULL && m_BJLPlaying.GetSafeHwnd() != NULL)
			{					
				showBJLBeginOrPlayingBtn(m_bShowPlaying);//百家乐
			}				
		}
	}//end of 百家乐 
	else if (m_deskClass == "queue" && 1 == m_bDeskCount)
	{
		m_YTip = 20;
		m_YTip1 = m_YTip + m_pImageQueueTip->GetHeight() + 20;
		m_YDesk = (ClientRect.Height() - m_YTip1 - m_iPicHight) / 2 + m_YTip1;

		//Queue_BK_Tip.png
		int x = (ClientRect.Width() - m_pImageQueueTip->GetWidth()) / 2;
		m_pGraph->DrawImage(m_pImageQueueTip, x, m_YTip, m_pImageQueueTip->GetWidth(), m_pImageQueueTip->GetHeight());

		
		m_pGraph->DrawImage(m_pImageQueueTip1, x, m_YTip1, m_pImageQueueTip1->GetWidth(), m_pImageQueueTip1->GetHeight());

		ShowQueueTime(x + m_pImageQueueTip1->GetWidth() / 2 + 14, m_YTip1 + 3, m_CurTime);
		ShowQueuePlan(x + m_pImageQueueTip1->GetWidth() / 2 + 14, m_YTip1 + 45, ((m_SumTime-m_CurTime) * 1.0) / (1.0 * m_SumTime) * 100);

		//y = y + m_pImageQueueTip1->GetHeight();
		//dc.FillSolidRect(&ClientRect,crBackColor);
		pDesk=GetDeskArrayPtr(0);
		if (pDesk)
		{
			m_iXBasePos = (ClientRect.Width() - m_iPicWidth)/2;
			m_iYBasePos = m_YDesk;
			DeskRect.SetRect(
				m_iXBasePos,
				m_iYBasePos,
				m_iXBasePos+m_iPicWidth,
				m_iYBasePos+m_iPicHight);
			/// 如果在游戏中，则显示游戏中的桌面图片

			// 根据是否在游戏状态画不同桌子图片
			if (pDesk->IsPlayGame())
			{
				m_pGraph->DrawImage(m_pImgBegin, DeskRect.left, DeskRect.top, DeskRect.Width(), DeskRect.Height());
			}
			else
			{
				m_pGraph->DrawImage(m_pImgDesk, DeskRect.left, DeskRect.top, DeskRect.Width(), DeskRect.Height());
			}

			// 画人物，同意标志，桌子号码，锁
			pDesk->DrawViewFacePng(*m_pGraph, DeskRect, m_pImgAgree, m_pImgLock, m_pImgFrame, m_pImgWaitSit, m_bIsVip);

			//showQueueBtn(true);
		}

		if (m_QueueStop.GetSafeHwnd())
		{
			if (!m_QueueStop.IsWindowVisible())
			{
				CRect rect,ClientRect;
				GetClientRect(&ClientRect);
				m_QueueStop.GetClientRect(&rect);
				int iWidth = rect.Width();
				int iHeight = rect.Height();

				m_YTip = 20;
				m_YTip1 = m_YTip + m_pImageQueueTip->GetHeight() + 20;
				m_YDesk = (ClientRect.Height() - m_YTip1 - m_iPicHight) / 2 + m_YTip1;

				int y = (ClientRect.Height() - m_YDesk - m_iPicHight - iHeight)/2 + m_YDesk + m_iPicHight - 60;

				m_pGraph->DrawImage(m_pImageQueueTip2, x + 10, y, m_pImageQueueTip2->GetWidth(), m_pImageQueueTip2->GetHeight());
			}
		}		
	}
	else
	{
		//绘画游戏桌
		int iBeginRow=m_iPos/m_iPicHight-1;
		iBeginRow=__max(0,iBeginRow);

		for (int i=iBeginRow;i<m_iScreenLineCount+iBeginRow+3;i++)
		{
			for (int j=0;j<m_iRowCount+1;j++)
			{
				DeskRect.SetRect(m_iXBasePos+j*m_iPicWidth,m_iYBasePos+i*m_iPicHight-m_iPos,
					m_iXBasePos+(j+1)*m_iPicWidth,m_iYBasePos+(i+1)*m_iPicHight-m_iPos);
				if ((j<m_iRowCount)&&((pDesk=GetDeskArrayPtr(i*m_iRowCount+j))!=NULL))
				{
					// 根据是否在游戏状态画不同桌子图片
					if (pDesk->IsPlayGame())
					{
						m_pGraph->DrawImage(m_pImgBegin, DeskRect.left, DeskRect.top, DeskRect.Width(), DeskRect.Height());
					}
					else
					{
						m_pGraph->DrawImage(m_pImgDesk, DeskRect.left, DeskRect.top, DeskRect.Width(), DeskRect.Height());
					}
					
					// 画人物，同意标志，桌子号码，锁
					pDesk->DrawViewFacePng(*m_pGraph, DeskRect, m_pImgAgree, m_pImgLock, m_pImgFrame, m_pImgWaitSit, m_bIsVip);					
				}
			}
		}
	}

	/////////////////////////// GDI+部分电脑修改卡屏问题
	Color c;
	m_pBackBmp->GetHBITMAP(c, m_bmp);
	CBitmap* p = CBitmap::FromHandle(*m_bmp);
	m_dcMem.SelectObject(p);
	dc.BitBlt(0,0,ClientRect.Width(),ClientRect.Height(), 
		&m_dcMem,0,0,SRCCOPY);

	p->DeleteObject();
}


void CDeskFrameView::PaintContestInfo()
{
	CPaintDC dc(this);
	CFont font;
	font.CreateFont(-16,0,0,0,700,NULL,NULL,NULL,134,3,2,1,2,TEXT("宋体"));
	CFont font1;
	font1.CreateFont(-12,0,0,0,400,NULL,NULL,NULL,134,3,2,1,2,TEXT("宋体"));
	dc.SelectObject(&font);
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(255,255,255));
	CRect rect;
	CString szInfo;
	szInfo = "第一局";
	rect.SetRect(40, 10, 400, 60);
	dc.DrawText(szInfo,szInfo.GetLength(),rect,DT_TOP|DT_LEFT|DT_END_ELLIPSIS);
	szInfo = "积分方式: 积分 * 倍数";
	dc.SelectObject(&font1);
	rect.SetRect(40, 60, 400, 90);
	dc.DrawText(szInfo,szInfo.GetLength(),rect,DT_TOP|DT_LEFT|DT_END_ELLIPSIS);
	szInfo = "游戏倍数: 10";
	rect.SetRect(40, 90, 400, 120);
	dc.DrawText(szInfo,szInfo.GetLength(),rect,DT_TOP|DT_LEFT|DT_END_ELLIPSIS);
	szInfo = "满 20 局以上可以参与排名";
	rect.SetRect(40, 120, 400, 150);
	dc.DrawText(szInfo,szInfo.GetLength(),rect,DT_TOP|DT_LEFT|DT_END_ELLIPSIS);
	szInfo = "低于 500 分将被淘汰";
	rect.SetRect(40, 150, 400, 180);
	dc.DrawText(szInfo,szInfo.GetLength(),rect,DT_TOP|DT_LEFT|DT_END_ELLIPSIS);	
}

//键盘按下信息
void CDeskFrameView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
	case VK_UP://键盘上方向键
		{
			SendMessage(WM_VSCROLL,MAKELONG(SB_LINEUP,m_iPos),NULL);//边起
			return;
		}
	case VK_DOWN://键盘下方向键
		{
			SendMessage(WM_VSCROLL,MAKELONG(SB_LINEDOWN,m_iPos),NULL);//边底
			return;
		}
	case VK_PRIOR://前翻页
		{
			SendMessage(WM_VSCROLL,MAKELONG(SB_PAGEUP,m_iPos),NULL);//前翻页
			return;
		}
	case VK_NEXT://后翻页 
	case VK_SPACE://空格
		{
			SendMessage(WM_VSCROLL,MAKELONG(SB_PAGEDOWN,m_iPos),NULL);
			return;
		}
	case VK_HOME://HOME
		{
			SendMessage(WM_VSCROLL,MAKELONG(SB_TOP,m_iPos),NULL);
			return;
		}
	case VK_END://END
		{
			SendMessage(WM_VSCROLL,MAKELONG(SB_BOTTOM,m_iPos),NULL);
			return;
		}
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);//其他键不管
}
//滚动条信息
void CDeskFrameView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar * pScrollBar)
{
	int iLastPos=m_iPos;
	switch (nSBCode)
	{
	case SB_TOP:
		{
			m_iPos=0;
			break;
		}
	case SB_BOTTOM:
		{
			m_iPos=m_iMaxPos-m_iScreenLineCount*m_iPicHight;
			break;
		}
	case SB_LINEUP:
		{
			m_iPos-=m_iScrollPos;
			break;
		}
	case SB_PAGEUP:
		{
			m_iPos-=m_iScreenLineCount*m_iPicHight;
			break;
		}
	case SB_LINEDOWN:
		{
			m_iPos+=m_iScrollPos;
			break;
		}
	case SB_PAGEDOWN:
		{
			m_iPos+=m_iScreenLineCount*m_iPicHight;
			break;
		}
	case SB_THUMBTRACK:
		{
			m_iPos=nPos;
			break;
		}
	}

	//调整位置
	SetFocus();
	m_iPos=__max(0,__min(m_iPos,m_iMaxPos-m_iScreenLineCount*m_iPicHight));
	if (iLastPos!=m_iPos)
	{
		SetScrollPos(SB_VERT,m_iPos);
		ScrollWindow(0,iLastPos-m_iPos,NULL,NULL);
	}
	//设置按钮位置
/*	if(m_pageup.GetSafeHwnd()!=NULL)
	{
		CRect rect;
		this->GetWindowRect(&rect);
		int cx,cy;
		cx=rect.Width();
		cy=rect.Height();
		m_pageup.MoveWindow(cx-30,cy/3,26,34);
		m_pagedown.MoveWindow(cx-30,cy*2/3,26,34);
	}*/
	return;
}

//鼠标滚轮消息
BOOL CDeskFrameView::OnMouseWheel(UINT nFlags, short zDelta, CPoint MousePoint)
{
	//调整位置
	int iLastPos=m_iPos;
	m_iPos=__max(0,__min(m_iPos-zDelta/m_iWhellTimes,m_iMaxPos-m_iScreenLineCount*m_iPicHight));
	if (iLastPos != m_iPos)
	{
		SetScrollPos(SB_VERT,m_iPos);
		ScrollWindow(0,iLastPos-m_iPos,NULL,NULL);
		//Invalidate();
	}
/*	if(m_pageup.GetSafeHwnd()!=NULL)
	{
		CRect rect;
		this->GetWindowRect(&rect);
		int cx,cy;
		cx=rect.Width();
		cy=rect.Height();
		m_pageup.MoveWindow(cx-30,cy/3,26,34);
		m_pagedown.MoveWindow(cx-30,cy*2/3,26,34);
	}*/
	return TRUE;
}

//位置消息
void CDeskFrameView::OnSize(UINT nType, int cx, int cy)
{
	CString str;
	str.Format("http----------deskframeview: cx=%d, cy=%d", cx, cy);
	OutputDebugString(str);
	CWnd::OnSize(nType, cx, cy);
	if(cx==0 || cy==0)
		return;
	//更改缓冲图
	if (SIZE_MINIMIZED!=nType)
	{
		int w=0, h=0;
		if (m_pBackBmp != NULL)
		{
			w = m_pBackBmp->GetWidth();
			h = m_pBackBmp->GetHeight();
		}
		if ((m_pBackBmp==NULL)||(cx>w) ||(cy>h))
		{
			delete m_pBackBmp;
			m_pBackBmp = new Bitmap(cx, cy);
			delete m_pGraph;
			m_pGraph = new Graphics(m_pBackBmp);
			m_dcMem.DeleteDC();
			m_dcMem.CreateCompatibleDC(GetDC());
		}
	}

	//调整位置
	if(m_iPicWidth==0 || m_iPicHight==0)
	{
		//AFCMessageBox("System Failed ");
	    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","System Failed ");
		return;
	}
	if (m_bDeskCount!=0)
	{
		int iWidth=__max(m_iPicWidth,cx);
		int iHeight=__max(m_iPicHight,cy);
		m_iRowCount=iWidth/m_iPicWidth;
		m_iScreenLineCount=iHeight/m_iPicHight;
		m_iAllLineCount=(m_bDeskCount+m_iRowCount-1)/m_iRowCount;
		m_iXBasePos=(iWidth-m_iRowCount*m_iPicWidth)/2;
	}

	/// 调整百家乐类型的开始按钮的位置
	char cKey[10];
	CString sPath=CBcfFile::GetAppPath (false);
	CBcfFile fsr( sPath + "SpecialRule.bcf");
	sprintf(cKey, "%d", pGameInfo->uNameID);
	int iResult = fsr.GetKeyVal(_T("BJLType"), cKey, 0);

	if (iResult)	/// 如果是百家乐类型
	{
		CRect rect,ClientRect;
		GetClientRect(&ClientRect);
		int iClientWidth = ClientRect.Width();
		int iClientHeight = ClientRect.Height();
		m_BJLBegin.GetClientRect(&rect);
		int iWidth = rect.Width();
		int iHeight = rect.Height();
		//int x = (ClientRect.Width()-m_iPicWidth)/2;
		//if (x<0)
		//{
		//	x=0;
		//}
		//x += m_iPicWidth-iWidth-20;
		//int y = (ClientRect.Height()-m_iPicHight)/2;
		//if (y<0)
		//{
		//	y=0;
		//}
		//y+=m_iPicHight-iHeight-20;
	//	m_BJLBegin.MoveWindow(x,y,iWidth,iHeight);
		m_BJLBegin.MoveWindow(ClientRect.Width()-iWidth-20,ClientRect.Height()-iHeight-20,iWidth,iHeight);
	//	m_BJLPlaying.MoveWindow(x,y,iWidth,iHeight);
		m_BJLPlaying.MoveWindow(ClientRect.Width()-iWidth-20,ClientRect.Height()-iHeight-20,iWidth,iHeight);




		//ShowBJLBeginBtn();// 百家乐 add by wlr 20090713
	}

	if (m_deskClass == "queue")
	{
		CRect rect,ClientRect;
		GetClientRect(&ClientRect);
		m_QueueStop.GetClientRect(&rect);
		int iWidth = rect.Width();
		int iHeight = rect.Height();

	m_YTip = 20;
	m_YTip1 = m_YTip + m_pImageQueueTip->GetHeight() + 20;
	m_YDesk = (ClientRect.Height() - m_YTip1 - m_iPicHight) / 2 + m_YTip1;

		int x = (ClientRect.Width() - iWidth)/2;
		int y = (ClientRect.Height() - m_YDesk - m_iPicHight - iHeight)/2 + m_YDesk + m_iPicHight - 60;

		m_QueueBegin.MoveWindow(x , y,iWidth,iHeight);
		m_QueueStop.MoveWindow(x, y,iWidth,iHeight);
	}
/*	if(m_pageup.GetSafeHwnd()!=NULL)
	{
		m_pageup.MoveWindow(cx-30,cy/3,26,34);
		m_pagedown.MoveWindow(cx-30,cy*2/3,26,34);
	}*/
	//设置滚动数据
	m_iMaxPos=m_iAllLineCount*m_iPicHight;//+m_iYBasePos;
	m_iPos=__max(0,__min(m_iPos,m_iMaxPos-m_iScreenLineCount*m_iPicHight));


	//设置滚动条
	SCROLLINFO ScrollInfo;
	ScrollInfo.cbSize=sizeof(SCROLLINFO);
	ScrollInfo.fMask=SIF_RANGE|SIF_PAGE|SIF_POS;
	ScrollInfo.nMin=0;
	ScrollInfo.nMax=m_iMaxPos;
	ScrollInfo.nPage=m_iScreenLineCount*m_iPicHight;
	ScrollInfo.nPos=m_iPos;
	SetScrollInfo(SB_VERT,&ScrollInfo,TRUE);

	return;
}

//鼠标移动消息
void CDeskFrameView::OnMouseMove(UINT nFlags, CPoint point)
{
	if ((m_bRightMouseDown==false)&&(m_bLeftMouseDown==false))
	{
		//测试按键
		BYTE bNewDeskIndex=255,bNewDeskStation=255;
		bNewDeskIndex=MouseHitTest(point);
		if (bNewDeskIndex!=255)
		{
			CBaseDeskView * pDeskPtr=GetDeskArrayPtr(bNewDeskIndex);
			CPoint DeskPoint((point.x-m_iXBasePos)%m_iPicWidth,(point.y+m_iPos-m_iYBasePos)%m_iPicHight);
			bNewDeskStation=pDeskPtr->MouseHitTest(DeskPoint);
		}

		//更新界面
		if (((bNewDeskIndex!=m_bDownDeskIndex)||(bNewDeskStation!=m_bDownDeskStation))
			&&((bNewDeskStation!=255)||(m_bDownDeskStation!=255)))
		{
			if (m_bDownDeskIndex!=255)
			{
				CBaseDeskView *pDeskPtr = GetDeskArrayPtr(m_bDownDeskIndex);
				pDeskPtr->m_bDeskStation = 255;
				pDeskPtr->SetHoverIndex(255,this);
			}
			if (bNewDeskIndex!=255)
			{
				CBaseDeskView *pDeskPtr = GetDeskArrayPtr(bNewDeskIndex);
				pDeskPtr->SetHoverIndex(bNewDeskStation,this);
			}
		}

		//隐藏右键弹出的用户信息
		if(bNewDeskIndex == 255|| bNewDeskStation == 255)
		{
			m_Tooltip.Activate(false);
			if(m_personalInfo.GetSafeHwnd()!=NULL)
			{
				m_personalInfo.ShowWindow(SW_HIDE);
				Invalidate(FALSE);
				m_personalInfo.close();//DestroyWindow();
				//InvalidateRect(NULL,true);
			}
		}

		//设置数据
		m_bDownDeskIndex=bNewDeskIndex;
		m_bDownDeskStation=bNewDeskStation;
	}
	CWnd::OnMouseMove(nFlags, point);
}
//鼠标右键按下消息
void CDeskFrameView::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();
	if (m_bRightMouseDown==false)
	{
		//测试按键
		BYTE bDeskIndex=MouseHitTest(point);
		if (bDeskIndex!=255)
		{
			CBaseDeskView * pDeskPtr=GetDeskArrayPtr(bDeskIndex);
			CPoint DeskPoint((point.x-m_iXBasePos)%m_iPicWidth,(point.y+m_iPos-m_iYBasePos)%m_iPicHight);
			BYTE bDeskStation=pDeskPtr->MouseHitTest(DeskPoint);
			if (bDeskStation!=255)
			{
				//更新数据
				m_bLeftMouseDown=true;
				pDeskPtr->SetMouseDown(true,this);
				SetCapture();
			}
		}
	}
	CWnd::OnLButtonDown(nFlags, point);
}

//鼠标右键按起消息
void CDeskFrameView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if ((m_bLeftMouseDown==true)&&(m_bRightMouseDown==false))
	{
		//测试按键
		ReleaseCapture();
		m_bLeftMouseDown=false;
		BYTE bDeskIndex=MouseHitTest(point);
		GetDeskArrayPtr(bDeskIndex)->SetMouseDown(false,this);
		if ((bDeskIndex==m_bDownDeskIndex)&&(bDeskIndex!=255))
		{
			if(m_bIsClick)
				return ;
			m_bIsClick=true;
			CBaseDeskView * pDeskPtr=GetDeskArrayPtr(bDeskIndex);
			CPoint DeskPoint((point.x-m_iXBasePos)%m_iPicWidth,(point.y+m_iPos-m_iYBasePos)%m_iPicHight);
			BYTE bDeskStation=pDeskPtr->MouseHitTest(DeskPoint);
			if(m_uComType!=TY_MATCH_GAME && !(pGameInfo->dwRoomRule & GRR_AUTO_SIT_DESK))//还有防作弊场
			{
				if ((bDeskStation==m_bDownDeskStation)&&(bDeskStation!=255))
					GetParent()->SendMessage(WM_LEFT_MOUSE_HIT_DESK,m_bDownDeskIndex,m_bDownDeskStation);
			}
			//if(m_uComType==TY_MATCH_GAME)
			//{
			//	GetParent()->SendMessage(WM_HITDESK,m_bDownDeskIndex,m_bDownDeskStation);
			//}
			SetTimer(IDR_CLICKDESK,100,NULL);
		}
	}
	CWnd::OnLButtonUp(nFlags, point);
}

//右键按下消息
void CDeskFrameView::OnRButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();
	if (m_bLeftMouseDown==false)
	{
		//测试按键
		BYTE bDeskIndex=MouseHitTest(point);
		if (bDeskIndex!=255)
		{
			CBaseDeskView * pDeskPtr=GetDeskArrayPtr(bDeskIndex);
			CPoint DeskPoint((point.x-m_iXBasePos)%m_iPicWidth,(point.y+m_iPos-m_iYBasePos)%m_iPicHight);
			BYTE bDeskStation=pDeskPtr->MouseHitTest(DeskPoint);
			if (bDeskStation!=255)
			{
				//更新数据
				m_bRightMouseDown=true;
				pDeskPtr->SetMouseDown(m_bRightMouseDown,this);
				SetCapture();
			}
		}
	}
	CWnd::OnRButtonDown(nFlags, point);
}
//右键按起消息
void CDeskFrameView::OnRButtonUp(UINT nFlags, CPoint point)
{
	if ((m_bRightMouseDown==true)&&(m_bLeftMouseDown==false))	
	{
		//测试按键
		ReleaseCapture();
		m_bRightMouseDown=false;
		BYTE bDeskIndex=MouseHitTest(point);
		GetDeskArrayPtr(bDeskIndex)->SetMouseDown(m_bRightMouseDown,this);
		if ((bDeskIndex==m_bDownDeskIndex)&&(bDeskIndex!=255))
		{
			CBaseDeskView * pDeskPtr=GetDeskArrayPtr(bDeskIndex);
			CPoint DeskPoint((point.x-m_iXBasePos)%m_iPicWidth,(point.y+m_iPos-m_iYBasePos)%m_iPicHight);
			BYTE bDeskStation=pDeskPtr->MouseHitTest(DeskPoint);

			if ((bDeskStation==m_bDownDeskStation)&&(bDeskStation!=255))
				GetParent()->SendMessage(WM_RIGHT_MOUSE_HIT_DESK,m_bDownDeskIndex,m_bDownDeskStation);

			//单击右键弹出用户信息 add by lxl 2010-11-8
			UserItemStruct *user;
			user =this->GetUserItem(bDeskIndex,bDeskStation);
			if(user!=NULL && user->GameUserInfo.dwUserID!=m_UserID && m_UserID!=0)
			{
				if(m_uComType!=TY_MATCH_GAME && !(pGameInfo->dwRoomRule&GRR_AUTO_SIT_DESK))
				{
					
// 					if(m_personalInfo.GetSafeHwnd()==NULL)
// 					{	
// 						m_personalInfo.Create(IDD_PERSONINFO,this);
// 						m_personalInfo.getRoomPtr(GetParent());
// 
// 						//把坐标转换成屏幕坐标
// 						CPoint p = point;
// 						/// 如果框的位置太低，则需要提高位置，否则个人信息框的下
// 						/// 半截会跑到屏幕外面
// 						int w= m_personalInfo.GetWidth();
// 						int h= m_personalInfo.GetHeight();
// 						RECT rcClient;
// 						GetClientRect(&rcClient);
// 						if (p.y + h > rcClient.bottom)
// 						{
// 							p.y = rcClient.bottom - h;
// 						}
// 						ClientToScreen(&p);
// 						m_personalInfo.MoveWindow(p.x, p.y, w, h);//JMod-185,255);-20090506
// 						
// 						m_personalInfo.UpdateUserItem(user->GameUserInfo);
// 						if(pOrderName!=NULL)
// 							m_personalInfo.pOrderName=pOrderName;
// 
// 						m_personalInfo.ShowWindow(SW_NORMAL);
// 						m_personalInfo.BringWindowToTop();
// 					}else 
// 					{
// 						m_personalInfo.UpdateUserItem(user->GameUserInfo);
// 						m_personalInfo.ShowWindow(SW_NORMAL);
// 						m_personalInfo.BringWindowToTop();
// 					}	

					//JianGuankun 2011.11.19 //右键玩家Logo弹出菜单
					CGameRoomEx* pGameRoomEx = (CGameRoomEx*)GetParent();
					if (pGameRoomEx)
					{
						pGameRoomEx->OpenUserMenuEx(user,true);
					}
					//end add
				}
				m_Tooltip.Update();
				m_Tooltip.Activate(true);
			}
		}
	}
	CWnd::OnRButtonUp(nFlags, point);
}
//按键测试
BYTE CDeskFrameView::MouseHitTest(POINT point)
{
	point.y+=m_iPos;
	if ((m_iPicHight<=0) || (m_iPicWidth<=0))
	{
		return 255;
	}
	int iDeskRow=(point.x-m_iXBasePos)/m_iPicWidth;
	int iDeskLine=(point.y-m_iYBasePos)/m_iPicHight;
	if ((iDeskRow>=0)&&(iDeskRow<m_iRowCount))
	{
		BYTE bDeskIndex=iDeskLine*m_iRowCount+iDeskRow;
		return (bDeskIndex<m_bDeskCount)?bDeskIndex:255;
	}
	return 255;
}
//更新游戏桌子
void CDeskFrameView::UpdateDeskView(BYTE bDeskIndex)
{
	CBaseDeskView * pDeskPtr=GetDeskArrayPtr(bDeskIndex);
	if (m_iRowCount<=0)
	{
		return;
	}
	if (pDeskPtr!=NULL && this->m_hWnd != NULL) 
	{
		int iXPos=(bDeskIndex%m_iRowCount)*m_iPicWidth;
		int iYPos=(bDeskIndex/m_iRowCount)*m_iPicHight-m_iPos;
		CRect DeskRect(iXPos+m_iXBasePos,iYPos+m_iYBasePos,m_iXBasePos+iXPos+m_iPicWidth,m_iYBasePos+iYPos+m_iPicHight);
		InvalidateRect(&DeskRect,FALSE);
	}
	return;
}

//设置光标
BOOL CDeskFrameView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if ((m_bRightMouseDown==false)&&(m_bLeftMouseDown==false))
	{
		CPoint MousePoint;
		GetCursorPos(&MousePoint);
		ScreenToClient(&MousePoint);
		BYTE bDeskIndex=MouseHitTest(MousePoint);
		if (bDeskIndex!=255)
		{
			CBaseDeskView * pDeskPtr=GetDeskArrayPtr(bDeskIndex);
			CPoint DeskPoint((MousePoint.x-m_iXBasePos)%m_iPicWidth,(MousePoint.y+m_iPos-m_iYBasePos)%m_iPicHight);
			pDeskPtr->m_bDeskStation=pDeskPtr->MouseHitTest(DeskPoint);			
			if (pDeskPtr->m_bDeskStation!=255)
			{
				SetCursor(m_hMouseCursor);
				int iXPos=(bDeskIndex%m_iRowCount)*m_iPicWidth;
				int iYPos=(bDeskIndex/m_iRowCount)*m_iPicHight-m_iPos;
				CRect DeskRect(iXPos+m_iXBasePos,iYPos+m_iYBasePos,m_iXBasePos+iXPos+m_iPicWidth,m_iYBasePos+iYPos+m_iPicHight);
				InvalidateRect(&DeskRect,FALSE);				
				return TRUE;
			}
		}
	}
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

//是否游戏
bool CDeskFrameView::IsPlayGame(BYTE bDeskIndex)
{
	CBaseDeskView * pDesk=GetDeskArrayPtr(bDeskIndex);
	if (pDesk!=NULL) return pDesk->IsPlayGame();
	return false;
}
//是否全部开始同意
bool CDeskFrameView::IsAllAgree(BYTE bDeskIndex)
{
	CBaseDeskView * pDeskPtr=GetDeskArrayPtr(bDeskIndex);
	if (pDeskPtr!=NULL) 
	{
		return (pDeskPtr->IsAllAgree());
	}
	return false;
}
//大厅同意人员数量
BYTE CDeskFrameView::CountAgreePeople(BYTE bDeskIndex)
{
	CBaseDeskView * pDeskPtr=GetDeskArrayPtr(bDeskIndex);
	if (pDeskPtr!=NULL) 
	{
		return (pDeskPtr->CountAgree());
	}
	return 0;
}
//是否设置密码
bool CDeskFrameView::IsSetPass(BYTE bDeskIndex)
{
	CBaseDeskView * pDesk=GetDeskArrayPtr(bDeskIndex);
	if (pDesk!=NULL) return pDesk->IsSetPass();
	return false;
}

//设置锁标志
void CDeskFrameView::SetLock(BYTE bDeskIndex, bool bLock)
{
	CBaseDeskView * pDesk=GetDeskArrayPtr(bDeskIndex);
	if (pDesk!=NULL) pDesk->SetLock(bLock,this);
	return;
}

//设置开始标志
void CDeskFrameView::SetPlayGame(BYTE bDeskIndex, bool bPlaying)
{
	CBaseDeskView * pDesk=GetDeskArrayPtr(bDeskIndex);
	if (pDesk!=NULL)
	{	
		pDesk->SetPlayGame(bPlaying,this);
		//showBJLBeginOrPlayingBtn(bPlaying);//百家乐 add by wlr 20090717
	}
	return;
}

//设置用户信息
bool CDeskFrameView::SetUserItem(BYTE bDeskIndex, BYTE bDeskStation, UserItemStruct * pUserItem)
{
	CBaseDeskView * pDesk=GetDeskArrayPtr(bDeskIndex);
	if (pDesk!=NULL) return pDesk->SetUserItem(bDeskStation,pUserItem,this);
	return false;
}

//获取用户信息
UserItemStruct * CDeskFrameView::GetUserItem(BYTE bDeskIndex, BYTE bDeskStation)
{
	CBaseDeskView * pDesk=GetDeskArrayPtr(bDeskIndex);
	if (pDesk!=NULL) return pDesk->GetUserItem(bDeskStation);
	return NULL;
}

//获取比赛信息
long int CDeskFrameView::GetMatchInfo(BYTE bDeskIndex, BYTE bDeskStation)
{
	CBaseDeskView * pDesk=GetDeskArrayPtr(bDeskIndex);
	if (pDesk!=NULL) return pDesk->GetMatchInfo(bDeskStation);
	return 0L;
}

//设置比赛信息
bool CDeskFrameView::SetMatchInfo(BYTE bDeskIndex, BYTE bDeskStation, long int dwUserID, TCHAR * szUserName)
{
	CBaseDeskView * pDesk=GetDeskArrayPtr(bDeskIndex);
	if (pDesk!=NULL) return pDesk->SetMatchInfo(bDeskStation,dwUserID,szUserName,this);
	return false;
}

//设置用户边框
void CDeskFrameView::SetUserFrame(BYTE bDeskIndex)
{
	//清理旧边框
	if (m_bUserFrameIndex!=m_bFindFrameIndex)
	{
		CBaseDeskView * pDesk=GetDeskArrayPtr(m_bUserFrameIndex);
		if (pDesk!=NULL) pDesk->SetShowFrame(false,this);
	}
	m_bUserFrameIndex=255;

	//设置新边框
	m_bUserFrameIndex=bDeskIndex;
	CBaseDeskView * pDesk=GetDeskArrayPtr(m_bUserFrameIndex);
	if (pDesk!=NULL) pDesk->SetShowFrame(true,this);

	return;
}

//设置查找边框
void CDeskFrameView::SetFindFrame(BYTE bDeskIndex)
{
	//清理旧边框
	if (m_bFindFrameIndex!=m_bUserFrameIndex)
	{
		CBaseDeskView * pDesk=GetDeskArrayPtr(m_bFindFrameIndex);
		if (pDesk!=NULL) pDesk->SetShowFrame(false,this);
	}
	m_bFindFrameIndex=255;

	//设置新边框
	m_bFindFrameIndex=bDeskIndex;
	CBaseDeskView * pDesk=GetDeskArrayPtr(m_bFindFrameIndex);
	if (pDesk!=NULL) pDesk->SetShowFrame(true,this);

	return;
}

//查找位置
BYTE CDeskFrameView::FindGameDesk(BYTE bLastFind, bool bOneNull, bool bTwoNull, bool bNoAllNull, bool bAllNull)
{
	BYTE bCheckDesk=bLastFind,bNullCount=0;
	for (BYTE i=0;i<m_bDeskCount;i++)
	{
		bCheckDesk++;
		if (bCheckDesk>=m_bDeskCount) bCheckDesk=0;
		if ((IsPlayGame(bCheckDesk)==false)&&((bNullCount=GetNullCount(bCheckDesk))!=0))
		{
			if (((bOneNull==true)&&(bNullCount==1))||((bTwoNull==true)&&(bNullCount==2))||
				((bNoAllNull==true)&&(bNullCount!=m_bEveryCount))||((bAllNull==true)&&(bNullCount==m_bEveryCount)))
			{
				//找到位置
				if(m_uComType!=TY_MATCH_GAME)
				{
					SetViewDesk(bCheckDesk);
					SetFindFrame(bCheckDesk);
				}
				return bCheckDesk;
			}
		}
	}
	return 255;
}

//获取空位置
BYTE CDeskFrameView::GetNullStation(BYTE bDeskIndex)
{
	CBaseDeskView * pDeskView=GetDeskArrayPtr(bDeskIndex);
	if (pDeskView!=NULL)
	{
		for (BYTE i=0;i<m_bEveryCount;i++)
		{
			if (pDeskView->m_pUserItem[i]==NULL) return i;
		}
	}
	return 255;
}

//获取空位置数目
BYTE CDeskFrameView::GetNullCount(BYTE bDeskIndex)
{
	CBaseDeskView * pDeskView=GetDeskArrayPtr(bDeskIndex);
	if (pDeskView!=NULL)
	{
		BYTE bNullCount=0;
		for (BYTE i=0;i<m_bEveryCount;i++)
		{
			if (pDeskView->m_pUserItem[i]==NULL) bNullCount++;
		}
		return bNullCount;
	}
	return 0;
}

//获得自己用户ID
bool CDeskFrameView::GetMeUserID(long MeUserID) 
{
	if(MeUserID>=0)
	{
		m_UserID=MeUserID;
		return true;
	}else return false;
}
/*******************************************************************************************************/

BOOL CDeskFrameView::PreTranslateMessage(MSG* pMsg)
{
	m_Tooltip.RelayEvent(pMsg);
	return CWnd::PreTranslateMessage(pMsg);
}

//百家乐 add by wlr 20090713
void CDeskFrameView::OnHitBJLBegin()
{
	CWnd* wnd=GetParent();
	if(wnd!=NULL)
		::PostMessage(wnd->GetSafeHwnd(),WM_HIT_BJL_BEGIN,0,0);
	
	////m_bShowPlaying = true;
	//m_BJLPlaying.EnableWindow(FALSE);
	//m_BJLBegin.EnableWindow(FALSE);
	//ShowBJLBeginBtn();
}
//end of add by wlr 20090713

//向上翻页按钮
void CDeskFrameView::OnHitpageup()
{
	::SendMessage(this->m_hWnd,WM_VSCROLL,SB_PAGEUP,0);
	return;
}
//显示开始按钮
void CDeskFrameView::ShowBeginBtn(BYTE bDeskIndex,BYTE bDeskStation,bool bShow)
{
	if(m_BeginAgree.GetSafeHwnd()!=NULL)
	{
		int iXPos=(bDeskIndex%m_iRowCount)*m_iPicWidth;
		int iYPos=(bDeskIndex/m_iRowCount)*m_iPicHight-m_iPos+m_iYBasePos;
		CRect rect;
		m_BeginAgree.GetClientRect(&rect);
		int iWidth = rect.Width();
		int iHeight = rect.Height();

		int startBtX,startBtY;
		/// X居中
		startBtX = iXPos + (m_iPicWidth - iWidth)/2 + m_iXBasePos;
		/// Y靠下
		startBtY = iYPos + m_iPicHight- iHeight;

		m_BeginAgree.MoveWindow(startBtX,startBtY,iWidth,iHeight);

		m_BeginAgree.ShowWindow(bShow?1:0);
#ifdef AI_MACHINE
		if(bShow)
			OnHitBegin();
#endif
	}
}
void CDeskFrameView::OnHitBegin()
{
	CWnd* wnd=GetParent();
	if(wnd!=NULL)
		::PostMessage(wnd->GetSafeHwnd(),WM_HIT_DESK_BEGIN,0,0);
}
//向上翻页按钮
void CDeskFrameView::OnHitpagedown()
{
	::SendMessage(this->GetSafeHwnd(),WM_VSCROLL,SB_PAGEDOWN,0);
	return;
}

void CDeskFrameView::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent==IDR_CLICKDESK)
	{
		KillTimer(IDR_CLICKDESK);
		m_bIsClick=false;
	}
	if (nIDEvent == TIME_QUEUE)
	{
		if (m_CurTime > 0)
			m_CurTime--;
		else
			m_CurTime = m_SumTime;

		//CRect rect,ClientRect;
		//GetClientRect(&ClientRect);
		//int iWidth = m_pImageQueueTip1->GetWidth();
		//int iHeight = m_pImageQueueTip->GetHeight();
		//m_YTip = 20;
		//m_YTip1 = m_YTip + iHeight + 20;
		//int x = (ClientRect.Width() - iWidth) / 2;

		//ShowQueueTime(x + iWidth / 2 + 20, m_YTip1 + 10, m_CurTime);
		//ShowQueuePlan(x + iWidth / 2 + 20, m_YTip1 + 48, ((m_SumTime-m_CurTime) * 1.0) / (1.0 * m_SumTime) * 100);

		////CRect DeskRect(x, m_YTip1, iWidth, iHeight);
		//CRect DeskRect(0, m_YTip1, 800, iHeight);
		////Invalidate();
		//InvalidateRect(&DeskRect, TRUE);
		Invalidate();
	}
	CWnd::OnTimer(nIDEvent);
}
