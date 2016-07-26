#include "IGameImpl.h"
#include <irrKlang.h>

/*-----------------------------------------------------------------------*/
//定时器 ID
#define ID_BEGIN_TIME				    100					//开始定时器
#define ID_OUT_CARD_TIME				110					//出牌等待时间
#define ID_TIME_CLOCK                   115                 //倒数闹钟计时器
#define ID_PLAY_MOVE_VSCARD             116                 //开始移回比牌的牌计时器
#define ID_TIME_LOOKCARD                117                 //开始移回比牌的牌计时器
/*-----------------------------------------------------------------------*/

using namespace irrklang;
interface IGameFrame;
interface IUserInterface;

class CNewGameClient : public IGameImpl
{
public:
    CNewGameClient(IGameFrame *pGameFrame, IUserInterface *pUI);
    virtual ~CNewGameClient();
private:
    CNewGameClient(const CNewGameClient &other);
    CNewGameClient &operator = (const CNewGameClient &other);

public:
	/// 初始化
	virtual int		Initial();
	/// 初始化UI
	virtual int		InitUI();
	/// 游戏消息
	virtual int		HandleGameMessage(WORD nMainID,WORD nSubId,BYTE * buffer,int nLen);
	/// 界面消息
	virtual int OnUIMessage(TUIMessage* pMessage);
	/// 
	virtual int ResetGameFrame(void);
	/// 玩家进入
	virtual int GameUserCome(void);
	/// 玩家离开
	virtual int GameUserLeft(int nIndex); 
	/// 玩家断线
	virtual int GameUserOffLine(void);
	// 是否正在游戏，由游戏实现
	// 旁观玩家返回false，游戏玩家正在游戏中返回true，其它状态返回false
	virtual bool IsPlayingGame();          
	/// 定时器消息
	virtual void OnTimer(int nTimerID);
	/// 置状态
	virtual void SetStationParameter(BYTE bGameStation);
	///设置游戏状态，主要是断线重连来后处理的消息
	virtual 	void SetGameStation(void * pBuffer,int nLen);

	virtual		bool OnGameQuiting();



public:
	//玩家上下分
	void	AddOrRemoveScore(bool bAdd,int iScore);
	//玩家发射子弹
	void	OnUserShot(bool bIsSuper,float fRoalation);
	//玩家加减火力
	void	OnUserAddFire(bool bAdd);
	//击中鱼儿
	void	OnUserHitFish(int iFishID,int iBulletID,int iShootUSER,bool IsAndroid);


private:
	//玩家上下分结果
	void	CDealUserBuyBullet(BYTE * buffer,int nLen);
	//玩家发射子弹消息
	void	CDealUserShot(BYTE * buffer,int nLen);
	//玩家发射子弹错误
	void	CDealUserShotError(BYTE * buffer,int nLen);
	//玩家加减炮火
	void	CDealUserAddFire(BYTE * buffer,int nLen);
	//玩家击中了鱼儿
	void	CDealUserHitFish(BYTE * buffer,int nLen);
	//取消超级炮弹
	void	CDealCanCelSuperPao(BYTE * buffer,int nLen);
	//捕获大鱼广播
	void	CDealCatchBroadCast(BYTE * buffer,int nLen);

private:
	//切换座位
	int		SwitchChairID(int m_Currchairid);

public:/*移动窗口所需的函数*/
	//记录鼠标按下时候的坐标
	CPoint	m_point;

	//移动窗口
	void	OnMoveWindow(CPoint);
	//最小化窗口
	void	OnMinWondow();
	//退出关闭游戏
	void	OnCloseWindow();

	//读取配置文件
	void	LoadGameSet();
	//保存游戏配置
	void	SaveGameSet();

private:
	//游戏场景类
	GameScreenWnd		m_GameScreen;
	//游戏框架类
    IGameFrame          *m_pGameFrame;
public:
    IUserInterface              *m_pUI;

	//游戏数据
	GameStation_Base	m_GameData;



	ISoundEngine               *m_SoundEngine;

	BYTE                         m_bGameStation;               ///<游戏状态

	int			m_iMyDeskStation;	//玩家自己的逻辑位置

};
