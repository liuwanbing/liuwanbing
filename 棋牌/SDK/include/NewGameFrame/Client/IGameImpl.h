#pragma once

#ifdef GAMEIMPLEMENT_EXPORTS
#define GAMEIMPLEMENT_API __declspec(dllexport)
#else
#define GAMEIMPLEMENT_API __declspec(dllimport)
#endif //GAMELOGIC_EXPORTS

class IGameFrame;
struct IUserInterface;
struct TUIMessage;
class IGameImpl
{
public:
    virtual ~IGameImpl(){}
    /// 初始化
    virtual int Initial()=0;
    /// 初始化UI
    virtual int InitUI()=0;
    /// 游戏消息
    virtual int HandleGameMessage(WORD nMainID,WORD nSubId,BYTE * buffer,int nLen)=0;
    /// 界面消息
    virtual int OnUIMessage(TUIMessage* pMessage)=0;
    /// 
    virtual int ResetGameFrame(void)=0;
    /// 玩家进入
    virtual int GameUserCome(void)=0;
    /// 玩家离开
    virtual int GameUserLeft(int nIndex)=0;
    /// 玩家断线
    virtual int GameUserOffLine(void)=0;
    // 是否正在游戏，由游戏实现
    // 旁观玩家返回false，游戏玩家正在游戏中返回true，其它状态返回false
    virtual bool IsPlayingGame()=0;          
    /// 定时器消息
    virtual void OnTimer(int nTimerID) = 0;
    /// 置状态
    virtual void SetStationParameter(BYTE bGameStation) = 0;
    //virtual void GameInfoChange(TGameInfo GameInfo) = 0;
    virtual bool OnGameQuiting() = 0;
};


extern "C" GAMEIMPLEMENT_API IGameImpl *CreateNewGameImpl(IGameFrame* pIBase, IUserInterface *pIUI);
extern "C" GAMEIMPLEMENT_API void DestroyNewGameImpl(IGameImpl **ppImp);

typedef IGameImpl *(* CREATEGAMEIMPLEMENT)(IGameFrame* pIBase, IUserInterface *pIUI);
typedef void (* DESTROYGAMEIMPLEMENT)(IGameImpl **ppImp);