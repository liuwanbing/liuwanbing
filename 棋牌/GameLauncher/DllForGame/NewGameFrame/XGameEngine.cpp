#include "stdafx.h"
#include "XGameEngine.h"
#include "XGameLogicBase.h"
#include "ipc/ChannelService.h"
#include "UI_ForLogic/IUserInterface.h"
#include "common/UIMessageDefines.h"
#include "ui/XUIBase.h"
#include "Graph/XGraphEngine.h"
#include <imm.h>
#include "xLogic.h"
#include "XRightFrame.h"
typedef HICON (BeginInitGameIcon)(int iNameID,bool bFlag);

extern void DebugPrintfA(const char *p, ...);

RECT rt = {0, 0, 1024, 738};

const TCHAR *XGameEngine::GetAppPath()
{
    static TCHAR s_Path[MAX_PATH];
    static bool s_bIsReady = false;
    if (!s_bIsReady)
    {
        ZeroMemory(s_Path,sizeof(s_Path));
        DWORD dwLength=GetModuleFileName(GetModuleHandle(NULL), s_Path, sizeof(s_Path));
        TCHAR *p = _tcsrchr(s_Path, TEXT('\\'));
        ++p;
        *p = TEXT('\0');
        s_bIsReady = true;
    }
    return s_Path;
}


XGameEngine *g_pEngine=NULL;
IGameEngine *CreateGameEngine()
{
    if (NULL==g_pEngine)
    {
        g_pEngine = new XGameEngine();
    }
    return g_pEngine;
}
void CloseGameEngine(IGameEngine **ppGameEngine)
{
    if (NULL!=g_pEngine)
    {
        delete g_pEngine;
        g_pEngine = NULL;
    }
    *ppGameEngine = NULL;
}

XGameEngine::XGameEngine()
{
    m_pLogic = NULL;
    m_pGameInstance = NULL;
    m_pIPCMod = NULL;

    m_pGraphEngine = NULL;
    m_pUIBase = NULL;
    m_pIUI = NULL;

    m_bUIReady = false;
    m_bIsInitial = false;
	m_bDUIShow = false;

}


XGameEngine::~XGameEngine()
{
    DestroyGameLogic(&m_pLogic);
    SafeDelete(m_pUIBase);
    SafeDelete(m_pGraphEngine);    
}

/// 初始化
int XGameEngine::Initial(EngineInit_t &init)
{
    m_pIPCMod = (CChannelService *)init.pIPC;
    m_pGameInstance = init.hInstance;
    m_pLogic = CreateGameLogic();    
	
	GameInfoStructInExe* pGameInfo = (GameInfoStructInExe*) init.pGameInfo;
	
	BeginInitGameIcon* pBeginInitGameIcon = (BeginInitGameIcon*)GetProcAddress(m_pGameInstance,"BeginInitGameIcon");
    BOOL bRet = false;
	if (pBeginInitGameIcon)
	{
		OutputDebugString("cq::pBeginInitGameIcon");
		bRet = CreateEx(0, AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW,0,0,pBeginInitGameIcon(pGameInfo->uNameID,false)), pGameInfo->szGameName, 
			WS_VISIBLE | WS_POPUP | WS_SYSMENU | WS_MINIMIZEBOX,
			0, 0, 1024, 738, NULL, NULL);
	}
	else
	{
		OutputDebugString("cq::pBeginInitGameIcon=NULL");
		bRet = CreateEx(0, AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW,0,0,0), pGameInfo->szGameName, 
			WS_VISIBLE | WS_POPUP | WS_SYSMENU | WS_MINIMIZEBOX,
			0, 0, 1024, 738, NULL, NULL);
	}

    CenterWindow();
    m_pGraphEngine = GlbGraphEngine();
    HRESULT hr = m_pGraphEngine->Initial(m_hWnd, NULL, NULL);
    if (FAILED(hr))
    {
        return -2;
    }
    /// 创建UI对象，同时返回接口    
    if (!CreateUI(&m_pUIBase, &m_pIUI))
    {
        return -1;
    }
    /// 初始化UI
    m_pUIBase->Initial(m_hWnd, GetAppPath());
    /// 初始化逻辑
    LogicInitParam_t initlogic;
    initlogic.hInstance = m_pGameInstance;
    initlogic.hwndHall = m_hWnd;
    initlogic.pIpc = m_pIPCMod;
    initlogic.pUI = m_pIUI;
    initlogic.pGameInfo = init.pGameInfo;
    initlogic.pEngine = dynamic_cast<IGameEngine*>(this);
    m_pLogic->Initial(initlogic);

    m_bIsInitial = true;
    return 0;
}
/// IPC消息
bool XGameEngine::OnIPCMessage(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)
{
    if (!m_bIsInitial)
    {
        //
        DebugPrintfA("未初始化而丢失了IPC消息");
        return false;
    }
    m_pLogic->OnIPCMessage(pHead, pIPCBuffer, wDataSize, hWndSend);
    return 0;
}
/// IPC心跳检测
bool XGameEngine::OnIPCHeartBeat()
{
    return m_pLogic->OnIPCHeartBeat();
}
/// 绘画
int XGameEngine::DrawAll()
{
    if (m_pGraphEngine == NULL)
    {
        return -1;
    }
    /// 绘制实际内容
    if (NULL != m_pUIBase)
    {
        m_pUIBase->Run();
    }
    if (!m_bUIReady)
    {
        return -1;
    }
    /// 准备绘制
    if (FAILED(m_pGraphEngine->Start2Draw()))
    {
        return -1;
    }

    /// 绘制结束
    m_pGraphEngine->FinishDraw();

	if (!m_bDUIShow)
	{
		XRightFrame* pRightFrame = ((XLogic*)m_pLogic)->m_pRightFrame;
		if (pRightFrame)
		{
			if (pRightFrame->m_uShowState == SW_SHOW)
			{
				pRightFrame->ShowWindow(SW_SHOW);

				WINDOWPLACEMENT place;
				GetWindowPlacement(&place);
				((XLogic *)m_pLogic)->OnMessageProc(UM_FIXRIGHTFRAME,place.showCmd,0);
			}
		}

		m_bDUIShow = true;
	}

	
    return 0;
}
///设置时钟
void XGameEngine::SetTimer(int nEventID, DWORD dwTime)
{
    DebugPrintfA("GameEngine Settimer");
    CWnd::SetTimer(nEventID, dwTime, NULL);
}
///关闭时钟
void XGameEngine::KillTimer(int nEventID)
{
    CWnd::KillTimer(nEventID);
}
///获取HWND
CWnd* XGameEngine::GetCWnd()
{
    return this;
}
///切换最大化与还原
void XGameEngine::SwitchSize()
{
    WINDOWPLACEMENT place;
    GetWindowPlacement(&place);

	int iWindowsWidth = GetSystemMetrics(SM_CXSCREEN);
	int iWindowsHeight = GetSystemMetrics(SM_CYSCREEN);
	XRightFrame* pXRightFrame = ((XLogic *)m_pLogic)->m_pRightFrame;

    if (SW_SHOWMAXIMIZED==place.showCmd)
    {
		RECT rc = {0,0,0,0};

		if (pXRightFrame->m_uShowState == SW_HIDE)
		{
			rc.right = - pXRightFrame->m_uOldWidth;
		}

		if (m_pIUI)
		{
			m_pIUI->SetControlsShrinkArea(rc);
		}
		
		ShowWindow(SW_RESTORE);
    }
    else
    {
		if (pXRightFrame)
		{
			
			RECT rc = {0,0,0,0};

			if (pXRightFrame->m_uShowState == SW_HIDE)
			{
				rc.right = - pXRightFrame->m_uOldWidth;
			}
			else if (pXRightFrame->m_uShowState == SW_SHOW)
			{
				if (iWindowsWidth >= 1280)
				{
					int iRightFrameWidth = pXRightFrame->m_uMaxWidth - pXRightFrame->m_uOldWidth;
					rc.right = iRightFrameWidth;
				}
				else
				{
					int iRightFrameWidth = pXRightFrame->m_uMinWidth - pXRightFrame->m_uOldWidth;
					rc.right = iRightFrameWidth;
				}
			}

			if (m_pIUI)
			{
				m_pIUI->SetControlsShrinkArea(rc);
			}
		}

		ShowWindow(SW_SHOWMAXIMIZED);
		RECT rt;
		SystemParametersInfo(SPI_GETWORKAREA,0,(PVOID)&rt,0);
		MoveWindow(&rt);
    }

	XRightFrame* pRightFrame = ((XLogic*)m_pLogic)->m_pRightFrame;
	if (pRightFrame)
	{
		pRightFrame->HideWindow();
	}

	m_bDUIShow = false;
}

/// 程序函数
LRESULT XGameEngine::WindowProc(UINT message,WPARAM wParam,LPARAM lParam )
{
    switch(message)
    {
    case WM_TIMER:
        {
            OnTimer(wParam);
        }
        break;
	case WM_ACTIVATE:
		{
			if (m_pLogic!=NULL)
			{
				if (wParam != WA_INACTIVE)
				{
					BYTE Active = 0;
					((XLogic *)m_pLogic)->SendIpcData(IPC_MAIN_USER, IPC_SUB_ACTIVE_TO_ROOM,&Active,sizeof(BYTE));
				}
			}
		}
	
		break;
    case WM_SIZE:
        {            
            if (NULL!=m_pGraphEngine)
            {
                HRESULT hr = m_pGraphEngine->OnResetDevice();
            }
            if (m_pUIBase != NULL)
            {
                m_pUIBase->OnInputEvent(message, wParam, lParam);
            }

			if (m_pLogic!=NULL)
			{
				BYTE Active = 1;
				((XLogic *)m_pLogic)->SendIpcData(IPC_MAIN_USER, IPC_SUB_ACTIVE_TO_ROOM,&Active,sizeof(BYTE));

			}
			WINDOWPLACEMENT place;
			GetWindowPlacement(&place);
			if (SW_SHOWMAXIMIZED==place.showCmd)
			{
				RECT rt;
				SystemParametersInfo(SPI_GETWORKAREA,0,(PVOID)&rt,0);
				MoveWindow(&rt);
				
			}
			
			RECT rt;
			GetClientRect(&rt);

			//最小化后不响应，修改聊天框错乱问题
			if (wParam == SIZE_MINIMIZED)
			{
				return 1;
			}
            if (m_pLogic!=NULL)
            {
                m_pLogic->OnSize(rt);
				Sleep(10);
				BYTE Active = 0;
				((XLogic *)m_pLogic)->SendIpcData(IPC_MAIN_USER, IPC_SUB_ACTIVE_TO_ROOM,&Active,sizeof(BYTE));
            }

			((XLogic *)m_pLogic)->OnMessageProc(UM_FIXRIGHTFRAME,place.showCmd,0);

            return 1;
        }
        break;
    /*case WM_ERASEBKGND:
        {
            return 1;
        }
        break;*/
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            BeginPaint(&ps);
            EndPaint(&ps);            
        }
        break;
	case WM_CLOSE:
		{
			m_pLogic->QuitGame(true,true);
			return 0;
		}
    case WM_DESTROY:
        {
            DebugPrintfA("窗口销毁，退出游戏");
            m_pLogic->QuitGame();
            return 0;
        }
        break;
    case WM_IME_COMPOSITION:
    case WM_IME_CHAR:
        {
            if (m_pUIBase != NULL && (lParam & GCS_RESULTSTR) )
            {
                HIMC hIMC = ImmGetContext(m_hWnd); 
                char szCompReadStr[200];
                memset(szCompReadStr, 0, sizeof(szCompReadStr));
                ImmGetCompositionString(hIMC, GCS_RESULTSTR,szCompReadStr,sizeof(szCompReadStr)); 
                wchar_t *p = CA2W(szCompReadStr);
                if (NULL!=p)
                {
                    int nLen = (int)wcslen(p);
                    for (int i=0; i<nLen; ++i)
                    {
                        m_pUIBase->OnInputEvent(WM_CHAR, p[i], lParam);
                    }
                }
            }
        }
        break;
    case WM_CHAR:   // 解决中文问题
        {
            unsigned char c=(unsigned char)wParam;            
            if (c<127)
            {
                if (m_pUIBase != NULL)
                {
                    m_pUIBase->OnInputEvent(message, wParam, lParam);
                }
            }        
        }
        break;
     case WM_MOVE:
		{
			if (m_pLogic!=NULL)
			{
				
				BYTE Active = 1;
				((XLogic *)m_pLogic)->SendIpcData(IPC_MAIN_USER, IPC_SUB_ACTIVE_TO_ROOM,&Active,sizeof(BYTE));
				if (m_pUIBase != NULL)
				{
					m_pUIBase->OnInputEvent(message, wParam, lParam);
				}

				Sleep(10);
				Active = 0;
				((XLogic *)m_pLogic)->SendIpcData(IPC_MAIN_USER, IPC_SUB_ACTIVE_TO_ROOM,&Active,sizeof(BYTE));

				WINDOWPLACEMENT place;
				GetWindowPlacement(&place);
				((XLogic *)m_pLogic)->OnMessageProc(UM_FIXRIGHTFRAME,place.showCmd,0);

				((XLogic *)m_pLogic)->OnMessageProc(message, wParam, lParam);
			}
			break;
		}
	case WM_MOVING:
    case WM_WINDOWPOSCHANGED:
    case WM_KEYUP:    
    case WM_KEYDOWN:
    case WM_SYSKEYUP:
    case WM_SYSKEYDOWN:
    case WM_SYSCHAR:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MOUSEMOVE:
    case WM_LBUTTONDBLCLK:    
        {
            if (m_pUIBase != NULL)
            {
                m_pUIBase->OnInputEvent(message, wParam, lParam);
            }
        }
        break;
    case WM_UI_MSG:
        {
            TUIMessage *p = (TUIMessage *)lParam;
            if ((NULL != p) && (NULL != m_pLogic))
            {
                if (p->_uMessageType == UI_LOADFINISHED)
                {
                    m_bUIReady = true;
                    ShowWindow(SW_SHOW);  
					
                }
                else
                {
					if (m_pLogic)
					{
						m_pLogic->OnUIMessage(p);
					}
                }
				if (m_pUIBase)
				{
                m_pUIBase->ClearMessage(p);
				}
            }

            break;
        }
    case WM_PROP_BUY:   // 购买道具 
        {
            m_pLogic->OnPannelBuyProp(wParam, lParam);
        }
        break;
    case WM_GIVE_PROP:  // 赠送道具
        {
            m_pLogic->OnPannelGiveProp(wParam, lParam);
        }
        break;
    case WM_USE_PROP:   // 使用道具
        {
            m_pLogic->OnPannelUseProp(wParam, lParam);
        }
        break;
    //过滤取消消息和回车消息
    case UM_USEITEM_USERSELECTED:
        {
            m_pLogic->OnSelOtherUseProp(wParam, lParam);            
        }
        break;
    case WM_USE_BOARDCAST:
        {   
            m_pLogic->OnUseBoardcast(wParam, lParam);
        }
        break;
    //银行消息
    case WM_TRANSFER_MONEY:
    case WM_TRANSFER_RECORD:
    case WM_CHECK_MONEY:
    case WM_BANK_GET_NICKNAME_ONID:
    case WM_CHANGE_PASSWD:
	case WM_CHECK_MONEY2:
	case WM_BANK2_CLOSE:
	case WM_BANK2_CHECK_SUC:
        {
            m_pLogic->OnBankMessage(message, wParam, lParam);
        }
        break;
	case WM_BUY_VIP:
		{
			m_pLogic->OnPropBuyVIP(message, wParam, lParam);
		}
		break;
    default:
        break;
    }

    return CWnd::DefWindowProc(message, wParam, lParam);
}

void XGameEngine::OnTimer(UINT_PTR nIDEvent)
{
    m_pLogic->OnTimer((int)nIDEvent);
}