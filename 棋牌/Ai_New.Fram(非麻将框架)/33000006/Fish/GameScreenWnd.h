/*
主屏幕场景类,这个类负责所有图片的显示
*/
#pragma once
#include "afxwin.h"
#include "gamestruct.h"
#include "../resource.h"
#include "configwnd.h"

#include "factory.h"
#include <deque>
#include <vector>



//引入DX文件

using namespace std;
class CClientGameDlg;




struct BroadCastData
{
	bool				bHave;	//是否有广播
	BYTE				byStation;					//玩家座位
	char				szNickName[61];				///<昵称  
	char				szRoomName[61];				///<房间名字 
	char				szFishName[30];				///<房间名字  
	int					iDeskNo;					///<桌子ID号
	__int64				i64FishScore;				//鱼群得分
};


//引入完毕
class GameScreenWnd :public CWnd
{
public:
	//创建函数
	GameScreenWnd();
	//解析函数
	~GameScreenWnd(void);

public:
	//获得平台指针
	void SetGameClientP(CClientGameDlg *p);

public:
	//设置窗口
	CConfigWnd              m_configDlg;
public:

	//成员变量
	//鱼儿游动图数量
	static const  int              m_FishmoveCount[24];
	///鱼儿死亡图数量
	static const  int              m_FishdeadCount[24];
	//活图大小
	static const  int              m_FishmoveRec[24][2];
	//死图大小
    static const  int              m_FishDeadRec[24][2];
	//鱼儿分数
	static const  int              m_fishscore[24];
	//鱼儿生成金币种类
	static const  int              m_iFishMoneyCount[24];
	//玩家炮坐标
	POINT                   m_UserPT[PLAY_COUNT]; 
	//炮管位置
	POINT                   m_UserPaoPT[PLAY_COUNT]; 
	//自己座位序列号
	int                     m_MeChariID;
	//自己座位真实号
	int                     m_MeRellayChairID;
	//单元分
	int                     m_CellScore;
	//玩家数组
	bool                    m_HaveUser[PLAY_COUNT];
	//炮管角度
	float                   m_UserPaoJiaodu[PLAY_COUNT];
	//发射状态
	bool                    m_UserSendstate[PLAY_COUNT];
	//自己子弹
	Bullet                  m_MeBullet[200];
	//别人子弹
    Bullet                  m_OterBullet[100];
	//上分数据
	__int64                 m_UserScore[PLAY_COUNT];
	//鱼儿数组
	Fish                    m_FishArr[250];
	//飘落金币数组
	Gold                    m_GoldArr[100];
	//坐标数组
	FishTrace               m_FishTrace[250][5];
	//大怪凋落动画数组
	MuchMoneyaction         m_MuchmoneyAct[PLAY_COUNT];
	//
	Numstruct               m_NumArr[100];
	//鼠标是否移动到按扭上
	bool                    m_IsMouseOver[5];

	//玩家倍率
	int                     m_BeiLv[PLAY_COUNT];
	//玩家钱堆
	MoneyDui                m_UserStruct[PLAY_COUNT][3];
	//是否定住
	bool                    m_IsDingzhu;
	//是否超级泡
	bool                    m_UserSuperPao[PLAY_COUNT];
	//已经加载数量
	int                     m_HaveLoadNum;
	//是否进入游戏
	bool                    m_InitScreen;
	
	//是否处于切换场景状态
	bool                    m_IsChangeScreen;
	//
	Soundmstruct            m_SoundArr[100]; 
	//浪潮数据 
	WaterWavestruct         m_ChanwaveSt;
	//备用数据,在整个程序里面递增，可以根据这个数据来设置子弹颜色切换等
	int                     m_ActionIndex;
	//
	int                     m_ActionIndex1;
	//按下时间用来设置发射子弹间隔时间
	int                     m_NowTime;
	//是否鼠标按下
	bool                    m_MouseDown;
	//用户名
	CString                 m_sUserName;
	//积分信息
	__int64                 m_i64MyMoney;
	//
	int                     m_SoundIndex;
	//
	CFactoryTemplate<Fish>	m_FishFactory;							//鱼群工厂
	
	//是否显示时间
	bool                    m_IsMustOut;
	//当前发泡剩余退出时间
	int                     m_OutTime;
	//
	CursorStruct            m_cursor;

	//
	Dingaction              m_DingStruc;



private:
	//抖屏参数
	int		m_iShakex;
	int		m_iShakey;
	int		m_iShakeCount;

	int		m_iMoveY;
	int		m_iColorR;
	int		m_iColorB;
	int		m_iColorG;

	

private:
	//窗口句柄
	HWND					m_hWnd;
	//绘制界面线程句柄
	HANDLE 					m_hDxThread;	
	//判定捕获线程句柄
	HANDLE 					m_hWorkThread;	
	//
	HANDLE 					m_hWorkThread1;	


	//背景声音
	//


//调用函数
public:
  
    //发射子弹
    void	UserShoot(int PlayerID,float Roation,int RealChairID,bool isAndroid);
	//加钱
	void	UserAddMoney(int PlayerID,int Fishid,int Money,int FishKindScord,bool ishavaction );
	//切换场景
	void	ChangeScreen(int bgindex);
	//玩家进入或者出去
	void	UserComeInorLeave(int PlayerID,bool ComeOrLeave);
	//设置自己信息
	void	SetMeInformation(int MePlayerid,int MeRellid ,CString sNickName , __int64 i64money);
	//设置玩家倍率
	void	SetBeiLv(int PlayerId,int BeiScore){m_BeiLv[PlayerId] = BeiScore;};
	//设置玩家分数
	void	SetUserScore(int PlayerId,__int64 Score){m_UserScore[PlayerId] = Score;};
	//单元分
	void	SetCellScore(int cellscore){m_CellScore = cellscore;};
	//更新自己积分
	void	SetBaseScore(__int64 myscore){m_i64MyMoney =  myscore;}
	//添加鱼
	void	AddFish(int traceX,int traceY,float roation,float movetime,float changetime,int fishtype,int ptindex,int nowindex,int fishid,int smallfishtype,int fishspeed);
	//设置玩家上下分

	//设置超级炮玩家
	void	SetSuperPao(int PlayerId,int Fishid){m_UserSuperPao[PlayerId]=true;}
	//取消超级炮玩家
	void	CancelSuperPao(int PlayerId){m_UserSuperPao[PlayerId]=false;}
	//猴子被打中爆炸
	void	SetBomAction(int PlayerId,int Fishid,int fishscore);
	//棒子定住
	void	SetDingAction(Fish *tfish);
	//捕获鱼广播
	void	CatchBroadCast(int iDeskNo,char szNickName[],char szRoomName[],int iFishType,__int64 i64FishScore);

 
public:
	//
	bool        m_RunRender;
	//是否已经和服务端通信了
	bool		m_bHaveLinked;
	//背景序列
	int         m_Bgindex;

	//初始化窗口
	bool		InitDxDlog();
	//创建DX窗口及载入资源
	HRESULT		InitD3D( HWND hWnd );
	
	//
	void	CheckHit();
	//时间事件
	void	OnGameTimer(UINT nIDEvent);

	//移动窗体
	bool	m_bMoveWindow;
	//鼠标追踪
	BOOL	_bMouseTrack;

	DECLARE_MESSAGE_MAP()
    
	afx_msg void	OnMouseMove(UINT, CPoint);
	afx_msg void	OnLButtonDown(UINT, CPoint);
	afx_msg void	OnLButtonUp(UINT, CPoint);
	afx_msg void	OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg	void	OnkeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void	OnMouseHover(UINT, CPoint);
	afx_msg void	OnMouseLeave();
  
};
