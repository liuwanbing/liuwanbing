#pragma once
#include <Windows.h>

#ifdef GAMELOGIC_EXPORTS
#define GAMELOGIC_API __declspec(dllexport)
#else
#define GAMELOGIC_API __declspec(dllimport)
#endif //GAMELOGIC_EXPORTS


struct TUIMessage;
struct IPC_Head;
class CChannelService;
/// 游戏逻辑类初始化参数结构
struct LogicInitParam_t
{
    HINSTANCE   hInstance;  ///< 游戏实例
    void *pUI;				///< UI接口指针
    void *pIpc;				///< IPC模块接口指针
    void *pGameInfo;        ///< 游戏信息
    void *pEngine;          ///< 引擎接口
    HWND hwndHall;			///< 大厅窗口句柄  
    
    LogicInitParam_t(){ZeroMemory(this, sizeof(LogicInitParam_t)); }
};
/// 逻辑模块基类
/// 供框架模块调用的接口类
class XGameLogicBase
{
public:
    /// 虚拟析构函数，用于删除时释放资源
    virtual ~XGameLogicBase(){};    
    /// 初始化
    virtual int Initial(LogicInitParam_t &lip)=0;
    /// IPC消息
    virtual bool OnIPCMessage(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)=0;
    /// IPC心跳检测
    virtual bool OnIPCHeartBeat()=0;
    /// UI消息
    virtual int OnUIMessage(TUIMessage *p)=0;
    /// 时钟
    virtual void OnTimer(int nEventID)=0;
    /// 离开游戏
    virtual void QuitGame(bool bNotify = true,bool bNeedCheck = false/*是否要弹出提示框*/ )=0;
    /// 窗口大小改变
    virtual void OnSize(RECT &rect)=0;

public:
    /// 使用道具
    virtual void OnPannelUseProp(WPARAM wParam, LPARAM lParam)=0;    
    /// 购买道具
    virtual void OnPannelBuyProp(WPARAM wParam, LPARAM lParam)=0;
    /// 赠送道具
    virtual void OnPannelGiveProp(WPARAM wParam, LPARAM lParam)=0;
    /// 对他人使用道具
    virtual void OnSelOtherUseProp(WPARAM wParam, LPARAM lParam)=0;
    //用户使用广播，对话框里确定后，在这里给用户ID和用户名赋值
    virtual void OnUseBoardcast(WPARAM wparam, LPARAM lparam)=0;   
    //银行界面发来的消息
    virtual void OnBankMessage(UINT message,WPARAM wParam,LPARAM lParam)=0;
    //买 VIP
    virtual void OnPropBuyVIP(UINT message,WPARAM wParam,LPARAM lParam)=0;
};

extern "C" GAMELOGIC_API XGameLogicBase *CreateGameLogic();
extern "C" GAMELOGIC_API void DestroyGameLogic(XGameLogicBase **ppLogic);

typedef XGameLogicBase *(* CREATEGAMELOGIC)();
typedef void (* DESTROYGAMELOGIC)(XGameLogicBase **ppLogic);