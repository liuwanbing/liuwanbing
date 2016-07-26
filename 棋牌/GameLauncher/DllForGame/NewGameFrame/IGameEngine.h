#pragma once

#ifdef FRAME_EXPORTS
#define FRAME_API _declspec(dllexport)
#else
#define FRAME_API _declspec(dllimport)
#endif //FRAME_EXPORT

struct IPC_Head;
struct EngineInit_t
{
    HINSTANCE   hInstance;      /// 游戏ico句柄
    void        *pIPC;          /// IPC模块
    void        *pGameInfo;     /// 游戏信息
};
/// 
class IGameEngine
{
public:
    /// 初始化
    virtual int Initial(EngineInit_t &init)=0;
    /// IPC消息
    virtual bool OnIPCMessage(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)=0;
    /// IPC心跳检测
    virtual bool OnIPCHeartBeat()=0;
    /// 绘画
    virtual int DrawAll()=0;
    ///设置时钟
    virtual void SetTimer(int nEventID, DWORD dwTime)=0;
    ///关闭时钟
    virtual void KillTimer(int nEventID)=0;
    ///获取HWND
    virtual CWnd* GetCWnd()=0;
    ///切换最大化与还原
    virtual void SwitchSize()=0;
};


extern FRAME_API IGameEngine *CreateGameEngine();
extern FRAME_API void CloseGameEngine(IGameEngine **ppGameEngine);