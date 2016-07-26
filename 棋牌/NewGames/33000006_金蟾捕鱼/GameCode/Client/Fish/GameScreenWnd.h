/*
主屏幕场景类,这个类负责所有图片的显示
*/
#pragma once
#include "afxwin.h"
#include "gamestruct.h"
#include "../resource.h"
//#include "..\resource.h"
#include "configwnd.h"
#include "BassMusicManager.h"
#include "factory.h"
#include <deque>



//引入DX文件

#include <d3dx9.h>
#include <d3d9.h>
#pragma comment (lib,"d3dx9.lib")
#pragma comment (lib,"d3d9.lib")
#include <vector>

using namespace std;
class CNewGameClient;


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
	void SetGameClientP(CNewGameClient *p);

public:
	//设置窗口
	CConfigWnd              m_configDlg;
	CNewGameClient			*m_pGameClient;
private:

	//成员变量
	//鱼儿游动图数量
	static const  int              m_FishmoveCount[24];
	///鱼儿死亡图数量
	static const  int              m_FishdeadCount[24];
	//活图大小
	static const  int              m_FishmoveRec[24][2];
	//死图大小
    static const  int              m_FishDeadRec[24][2];

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
	//按扭区域
	CRect                   m_BtRect[5];
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
	int                     m_CursorInt;
	//
	Dingaction              m_DingStruc;

	deque	<BroadCastData> m_dqBroadCast;

	bool		m_bHitFlag;					//是否存在机器人击中鱼儿的判断
	BYTE		m_bHitDesk;						//机器人击中鱼儿的判断的位置号

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
	//DX设备
    LPDIRECT3D9             m_pD3D;
	//字
	ID3DXFont*              m_Dxfont;
	//广播字
	ID3DXFont*              m_DxCastFont;
	//
	LPDIRECT3DDEVICE9       m_pd3dDevice;
	//创建绘制精灵
	LPD3DXSPRITE            m_pSprite;
	//背景声音
	//

	//
//资源TEXTURE
private:
	//背景图片TEXT
	LPDIRECT3DTEXTURE9      m_bgImgTexture[3];   
	//游戏边框TEXT
	LPDIRECT3DTEXTURE9      m_BorderImgTexture;   
	//炮底座
    LPDIRECT3DTEXTURE9      m_PaoConImgTexture;   
	//炮背景
	LPDIRECT3DTEXTURE9      m_PaoBgImgTexture;   
	//2根炮
	LPDIRECT3DTEXTURE9      m_PaoTwoImgTexture;   
	//
	LPDIRECT3DTEXTURE9      m_PaoTwoFireImgTexture;   
	//3根炮
    LPDIRECT3DTEXTURE9      m_PaoThreeImgTexture;   
	//2根超级炮
	LPDIRECT3DTEXTURE9      m_PaoThreeFireImgTexture;  
	//3根超级炮
	//上下分的背景图
    LPDIRECT3DTEXTURE9      m_allscoreBgImgTexture[PLAY_COUNT];   
	//数字图片
	LPDIRECT3DTEXTURE9      m_NumImgTexture[5];
	//金币图片
	LPDIRECT3DTEXTURE9      m_GoldImgTexture;
	//2根子弹图
    LPDIRECT3DTEXTURE9      m_BulltTwoTexture[2];
	//2根炮子弹网图
	LPDIRECT3DTEXTURE9      m_PaoTwoNetImgTexture;   
	//鱼游动图
	LPDIRECT3DTEXTURE9      m_FishMoveTexture[23][20];
	//鱼死亡图
	LPDIRECT3DTEXTURE9      m_FishDeadTexture[23][10];
	//大把钞票图
	LPDIRECT3DTEXTURE9      m_MuchMoneyTexture[22];
	///专圈动画
	LPDIRECT3DTEXTURE9      m_QuanTexture[3];
	///按扭图
	LPDIRECT3DTEXTURE9      m_BtTexture[5];
	//小钱币
	LPDIRECT3DTEXTURE9      m_BiTexture;
	//载入背景
	LPDIRECT3DTEXTURE9      m_LoadBgTexture;
	//浪潮背景
	LPDIRECT3DTEXTURE9      m_WaterWaveTexture[2];
	//波浪背景
	LPDIRECT3DTEXTURE9      m_WaterTexture[32];
	//载入进度
	LPDIRECT3DTEXTURE9      m_LoadTexture;
	//超级炮子弹
	LPDIRECT3DTEXTURE9      m_SupButTexture[2];
	//超级状态图片
	LPDIRECT3DTEXTURE9      m_SupStatTexture;
	//超级状态图片
	LPDIRECT3DTEXTURE9      m_OutTexture;
	//鼠标样式
	LPDIRECT3DTEXTURE9      m_CursorTexture;
	//定动画图
	LPDIRECT3DTEXTURE9      m_DingTexture[10];


//调用函数
public:
  
    //发射子弹
    void	UserShoot(int PlayerID,float Roation,int RealChairID,bool isAndroid);
	//加钱
	void	UserAddMoney(int PlayerID,int Fishid,int Money,int FishKindScord,bool ishavaction,bool bMoneyPile = true);
	//切换场景
	void	ChangeScreen(int bgindex);
	//玩家进入或者出去
	void	UserComeInorLeave(int PlayerID,bool ComeOrLeave);
	//设置自己信息
	void	SetMeInformation(int MePlayerid,int MeRellid ,CString sNickName , __int64 i64money);
	//设置玩家倍率
	void	SetBeiLv(int PlayerId,int BeiScore){m_BeiLv[PlayerId] = BeiScore;if(m_configDlg.m_ptx2>0)PlayGameSound(71);};
	//设置玩家分数
	void	SetUserScore(int PlayerId,__int64 Score){m_UserScore[PlayerId] = Score;};
	//单元分
	void	SetCellScore(int cellscore){m_CellScore = cellscore;};
	//更新自己积分
	void	SetBaseScore(__int64 myscore){m_i64MyMoney =  myscore;}
	//设置机器人逻辑客户端
	void	SetRobotLogic(bool bHitFlag, BYTE byHitDesk){m_bHitFlag = bHitFlag; m_bHitDesk = byHitDesk;};
	//添加鱼
	void	AddFish(int traceX,int traceY,float roation,float movetime,float changetime,int fishtype,int ptindex,int nowindex,int fishid,int smallfishtype,int fishspeed);
	//设置玩家上下分
	//清理所有数据
	void	CleanAllMemory();
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

private:
	//大怪爆钱
	void GiveMuchMoney(int Fishindex,int PlayerID,int Money,Fish *Deadfish,int fishscore);
	//载入图片纹理
	void	LoadImg(const char* m_path,LPDIRECT3DTEXTURE9*  ppTexture);
    //载入声音
	void	LoadSound();

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
	//渲染界面
	void	Render();
	//载入纹理
	void	LoadImg();
	//绘制按扭
	void	DrawBt();

	//播放背景音乐
	void	PlayBgSound(int iIndex);
	//停止背景音效
	void	StopBgSound();
	//音效是否正在播放
	bool	IsPlaying(int iIndex);
	//播放游戏音效
	void	PlayGameSound(int iIndex);
	//停止游戏音效
	void	StopGameSound();

	//鼠标追踪
	BOOL	_bMouseTrack;

	DECLARE_MESSAGE_MAP()
    //时间事件
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void	OnMouseMove(UINT, CPoint);
	afx_msg void	OnLButtonDown(UINT, CPoint);
	afx_msg void	OnLButtonUp(UINT, CPoint);
	afx_msg void	OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg	void	OnkeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void	OnMouseHover(UINT, CPoint);
	afx_msg void	OnMouseLeave();
  
};
