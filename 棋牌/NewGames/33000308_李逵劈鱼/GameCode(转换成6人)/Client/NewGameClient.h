#include "IGameImpl.h"
#include <irrKlang.h>

#include "IGameFrame.h"
#include "hge.h"
#include "hgesprite.h"
#include "game_manager.h"
#include "gui_manager.h"
#include "MiniDumper.h"
#include "magic/platform_win.h"
#include "magic/mp_wrap.h"

using namespace irrklang;

/*------------------------------------------------------------------------*/
#define   TIME_WAIT_LOAD     101 ///等待游戏资源加载完成
#define	  TIME_CLOSE		 102 ///关闭计时器
/*------------------------------------------------------------------------*/
interface IGameFrame;
interface IUserInterface;
/*------------------------------------------------------------------------*/
class CNewGameClient : public IGameImpl
{
public:
    CNewGameClient(IGameFrame *pGameFrame, IUserInterface *pUI);
    virtual ~CNewGameClient();
private:
    CNewGameClient(const CNewGameClient &other);
    CNewGameClient &operator = (const CNewGameClient &other);

public:
	///框架游戏指针
	IGameFrame         *m_pGameFrame;
	//标记是否关闭客户端
	bool				m_bClose;
private:
	
	///界面游戏指针
	IUserInterface     *m_pUI;

	ISoundEngine       *m_SoundEngineGame;    //游戏音效

	ISoundEngine       *m_SoundEnginBg ;      ///背景音效

	//变量定义
private:
	int						m_iVersion;					//内部开发版本号
	int						m_iVersion2;				//内部开发版本号
	BYTE					m_iPalyCard;				//游戏扑克副数		DWJ
	int						m_iPlayCount;				//游戏扑克总数		DWJ
	BYTE					m_iBackCount;				//底牌数
	int						m_iRoomBasePoint;			//房间倍数			DWJ

	GameStation				m_bufferForGameStation;		//临时存储数据包
	bool					m_GetGameStation;			//断线重回操作完成
public: 

	BYTE                m_byGameStation ;         ///当前的游戏状态
	int					m_iLogicDesk;				//玩家的逻辑位置
	int					m_iViewDesk;				//视图位置
	int                 m_iMyDeskStation ;       ///玩家自己的游戏位子
	UserInfoForGame		m_UserInfo[PLAY_COUNT];			/**<玩家信息*/
	bool				m_bIsPlaying;			//游戏是否已经开始


	bool                m_bSoundPlay ;		//是否可以播放声音
	bool                m_bSoundBgPlay ;	//背景音效

	bool			m_bIsWatching;//是否为旁观玩家
 public:
    /// 初始化
    virtual int	Initial();
    /// 初始化UI
    virtual int InitUI();
    /// 游戏消息
    virtual int HandleGameMessage(WORD nMainID,WORD nSubId,BYTE * buffer,int nLen);
    /// 界面消息
    virtual int OnUIMessage(TUIMessage* pMessage);
    /// 重新设置状态
    virtual int ResetGameFrame(void);
    /// 玩家进入
    virtual int GameUserCome(void);
    /// 玩家离开
    virtual int	GameUserLeft(int nIndex);
    /// 玩家断线
    virtual int GameUserOffLine(void);
    // 是否正在游戏，由游戏实现
    // 旁观玩家返回false，游戏玩家正在游戏中返回true，其它状态返回false
    virtual bool IsPlayingGame();          
    /// 定时器消息
    virtual void OnTimer(int nTimerID);
    /// 置状态
    virtual void SetStationParameter(BYTE bGameStation);
    ///设置游戏状态
	virtual BYTE GetStationParameter();
	///游戏结束
	virtual bool OnGameQuiting();

public:
	static	CNewGameClient *GetMySelf();

	static CNewGameClient* m_pNewGameClient;

public:

	///初始化游戏中的变量
	void	InitialGameData();  
	///初始化部分数据
	void	ResetGameData();
	//读取游戏设置
	void	LoadGameSet();
	//保存游戏设置
	void	SaveGameSet();

	///玩家进入游戏
	void	SetGameStation();
	//播放背景音效
	void	PlayBgSound(bool bISLoop, int imusictag = 0);

	//获取玩家信息
	//玩家视图位置
	int ViewStation(int desk);
	//是否存在该玩家
	bool isPlayer(int desk);
	//玩家数据
	bool GetPlayerInfo(int desk, UserInfoForGame& UserInfo);


public:
		//切换座位
	int		SwitchChairID(int m_Currchairid);

};