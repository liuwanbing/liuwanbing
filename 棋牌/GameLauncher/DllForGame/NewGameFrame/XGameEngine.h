#pragma once
#include "IGameEngine.h"


class CChannelService;
class XGraphEngine;
class XUIBase;
interface IUserInterface;
class XGameLogicBase;

/// 游戏引擎
/// 用于处理 UI类 逻辑类之间的关系
class XGameEngine 
    : public CWnd
    , public virtual IGameEngine
{
public:
    XGameEngine();
    ~XGameEngine();
private:
    XGameEngine(const XGameEngine &other);
    XGameEngine &operator = (const XGameEngine &other);

public:
    /// 初始化
    virtual int Initial(EngineInit_t &init);
    /// IPC消息
    virtual bool OnIPCMessage(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend);
    /// IPC心跳检测
    virtual bool OnIPCHeartBeat();
    /// 绘画
    virtual int DrawAll();
    ///设置时钟
    virtual void SetTimer(int nEventID, DWORD dwTime);
    ///关闭时钟
    virtual void KillTimer(int nEventID);
    ///获取HWND
    virtual CWnd* GetCWnd();
    ///切换最大化与还原
    virtual void SwitchSize();

protected:
    /// 程序函数
    virtual LRESULT WindowProc(UINT message,WPARAM wParam,LPARAM lParam );

private:
    //
    const TCHAR *GetAppPath();

    void OnTimer(UINT_PTR nIDEvent);

private:
    XGameLogicBase              *m_pLogic;
    HINSTANCE                   m_pGameInstance;
    CChannelService             *m_pIPCMod;

    XGraphEngine                *m_pGraphEngine;
    XUIBase                     *m_pUIBase;
    IUserInterface              *m_pIUI;
    bool                        m_bUIReady;
    bool                        m_bIsInitial;

	bool						m_bDUIShow;			//为了防止DirectUI的窗口渲染比bbwUI快，造成先显示右边框再显示游戏框的不良体现，\
													  特设计m_bRightFrameShow，目的是DirectUI渲染完成后让其隐藏，待bbwUI渲染完成后再让DirectUI窗口显示。\
													  jianguankun 2012.9.7

};