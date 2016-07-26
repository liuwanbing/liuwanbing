#include "stdafx.h"
#include "NewGameClient.h"
#include "UI_ForLogic/IUserInterface.h"
#include "UI_ForLogic/GameIDDefines.h"
#include "Common/UIMessageDefines.h"
#include "LoveSendClassInExe.h"

/*------------------------------------------------------------------------*/
extern int screen_width_ = DEF_SCREEN_WIDTH;
extern int screen_height_ = DEF_SCREEN_HEIGHT;

extern	float g_hscale = 1.0f;
extern	float g_vscale = 1.0f;
/*------------------------------------------------------------------------*/
HGE* hge_ = NULL;
//MiniDumper* g_mini_dumper = NULL;

// 资源进度条相关
volatile int loading_result = 0; // 0 正在加载 1 加载成功 -1 加载失败
int loading_progress_ = 0;
hgeSprite* spr_logo_ = NULL;
HTEXTURE tex_logo_ = NULL;
hgeSprite* spr_loading_bar_ = NULL;
HTEXTURE tex_loading_bar_ = NULL;
extern	hgeSprite* spr_GameFrame_Top_ = NULL;
HTEXTURE tex_GameFrame_Top_ = NULL;
extern	hgeSprite* spr_GameFrame_Min_ = NULL;
HTEXTURE tex_GameFrame_Min_ = NULL;
extern	hgeSprite* spr_GameFrame_Exit_ = NULL;
HTEXTURE tex_GameFrame_Exit_ = NULL;

HANDLE loading_handle_ = NULL;
HANDLE StartHge_hanle_ = NULL;


bool   gbEndGame = false;
/*-----------------------------------------------------------------------------*/
CNewGameClient* CNewGameClient::m_pNewGameClient = NULL;
/*-----------------------------------------------------------------------------*/

bool FrameFunc() 
{
	float delta_time = hge_->Timer_GetDelta();

	GuiManager::GetInstance().GetGui()->logic();

	if (loading_result == 1) 
	{
		if (GameManager::GetInstance().OnFrame(delta_time)) 
		{
			gbEndGame = true;
			return true;
		}
	} 
	else if (loading_result == -1) 
	{
		hge_->System_Log("资源加载失败");
		return true;
	}

	MP_Manager::GetInstance().UpdateByTimer();

	return false;
}
/*-----------------------------------------------------------------------------*/
bool RenderFunc() 
{
	hge_->Gfx_BeginScene();
	hge_->Gfx_Clear(0);


	if (loading_result == 1) 
	{
		GameManager::GetInstance().OnRender(static_cast<float>(screen_width_) / kResolutionWidth, static_cast<float>(screen_height_) / kResolutionHeight);
	} 
	else 
	{
		
		spr_logo_->RenderEx(0.0f, 0.0f, 0.0f, g_hscale, g_vscale);
		float bar_ypos = static_cast<float>(screen_height_) - hge_->Texture_GetHeight(tex_loading_bar_) * g_vscale;
		float bar_width = static_cast<float>(hge_->Texture_GetWidth(tex_loading_bar_));

		for (int i = 0; i < loading_progress_; ++i) 
		{
			spr_loading_bar_->RenderEx(i * bar_width * g_hscale, bar_ypos, 0.0f, g_hscale);
		}		
	}

	

	GuiManager::GetInstance().GetGui()->draw();

	// 只有几个效果 直接写这了.
	MP_Manager& MP=MP_Manager::GetInstance();
	HM_EMITTER hmEmitter=MP.GetFirstEmitter();
	while (hmEmitter) 
	{
		MP_Emitter* emitter=MP.GetEmitter(hmEmitter);
		emitter->Render();
		hmEmitter=MP.GetNextEmitter(hmEmitter);
	}

	hge_->Gfx_EndScene();

	return false;
}
/*-----------------------------------------------------------------------------*/
// 资源加载线程 暂时性处理
// TODO:把content.res里的resgroup设置为不同(由小到大),然后调用Precache来实现进度
unsigned __stdcall LoadingThread(void* param) 
{

	//if (GameManager::GetInstance().LoadGameResource(loading_progress_)) 
	//{
	//	loading_progress_ = 144;
	//	//Sleep(1000);
	//	loading_result = 1;
	//} 
	//else 
	//{
	//	loading_result = -1;
	//}

	int inum = 0;
	do 
	{
		if (GameManager::GetInstance().LoadGameResource(loading_progress_))
		{
			loading_progress_ = 144;
			loading_result = 1;
		}

		inum++;
	} while (inum > 2 || loading_result == 0);

	if (loading_result != 1)
	{
		loading_result = -1;
	}
	return 0;
}
/*-----------------------------------------------------------------------------*/
bool FocusGainFunc() 
{
		GameManager::GetInstance().set_game_active(true);
	return false;
}
/*-----------------------------------------------------------------------------*/
bool FocusLostFunc() 
{
		GameManager::GetInstance().set_game_active(false);
	return false;
}
/*-----------------------------------------------------------------------------*/
unsigned __stdcall StartHge(void* param) 
{
	//初始化添加DUMP文件
	//g_mini_dumper = new MiniDumper(true);

	hge_ = hgeCreate(HGE_VERSION);

	//assert(0);  // for debug
	//设置窗口大小
	RECT work_area;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &work_area, 0);
	int screen_real_height = work_area.bottom - work_area.top - GetSystemMetrics(SM_CYFIXEDFRAME) * 2 - GetSystemMetrics(SM_CYCAPTION);
	screen_width_ = GetSystemMetrics(SM_CXSCREEN);
	screen_height_ = GetSystemMetrics(SM_CXSCREEN) * kResolutionHeight / kResolutionWidth;
	if (screen_height_ > screen_real_height) 
	{
		screen_width_ = screen_real_height * kResolutionWidth / kResolutionHeight;
		screen_height_ = screen_real_height;
	}

	g_hscale = static_cast<float>(screen_width_) / kResolutionWidth;
	g_vscale = static_cast<float>(screen_height_) / kResolutionHeight;

	
	//g_rgnNoteRect[0].CreatePolygonRgn(ptPonit1,4,ALTERNATE);

	hge_->System_SetState(HGE_LOGFILE, "lkpy.log");		//HGE日志
	hge_->System_SetState(HGE_SHOWSPLASH, false);		//关闭LOG
	hge_->System_SetState(HGE_FRAMEFUNC, FrameFunc);
	hge_->System_SetState(HGE_RENDERFUNC, RenderFunc);
	hge_->System_SetState(HGE_FOCUSGAINFUNC, FocusGainFunc);
	hge_->System_SetState(HGE_FOCUSLOSTFUNC, FocusLostFunc);
	hge_->System_SetState(HGE_TITLE, "李逵劈鱼");		//标题
	hge_->System_SetState(HGE_ICON,MAKEINTRESOURCEA(IDI_CLIENT_ROOM));
	hge_->System_SetState(HGE_USESOUND, true);				//使用音效
	hge_->System_SetState(HGE_WINDOWED, true);				//窗口化
	hge_->System_SetState(HGE_SCREENWIDTH, screen_width_);	//窗口宽度
	hge_->System_SetState(HGE_SCREENHEIGHT, screen_height_);//窗口高度
	hge_->System_SetState(HGE_SCREENBPP, 32);				//渲染色彩深度
	hge_->System_SetState(HGE_HIDEMOUSE, true);				//显示系统鼠标,如果你想自己渲染鼠标位图就应该设为true.否则显示系统鼠标.类型为bool.默认为true.
	hge_->System_SetState(HGE_FPS, 30);
	hge_->System_SetState(HGE_DONTSUSPEND, true);
	hge_->System_SetState(HGE_RESENCRYPT, true);			//资源是否加密

	
	// 资源包
	//hge_->Resource_AttachPack("fish\\resource.pak", "123456");
	//引擎初始化
	if (hge_->System_Initiate()) 
	{
		HWND hwnd = hge_->System_GetState(HGE_HWND);
		//设置窗口大小固定
		RECT rect;
		GetWindowRect(hwnd, &rect);
		SetWindowPos(hwnd, NULL, rect.left, 0, 0, 0, SWP_NOSIZE);
		//ShowWindow(hge_->System_GetState(HGE_HWND), SW_MINIMIZE);

		DWORD size;
		void* data;

		//背景LOG0
		data = hge_->Resource_Load("lkpy\\images\\loading\\logo.fish", &size);
		tex_logo_ = hge_->Texture_Load(static_cast<const char*>(data), size);
		hge_->Resource_Free(data);
		spr_logo_ = new hgeSprite(tex_logo_, 0.f, 0.f, static_cast<float>(hge_->Texture_GetWidth(tex_logo_)),static_cast<float>(hge_->Texture_GetHeight(tex_logo_)));

		//进度条
		data = hge_->Resource_Load("lkpy\\images\\loading\\loading_bar.fish", &size);
		tex_loading_bar_ = hge_->Texture_Load(static_cast<const char*>(data), size);
		hge_->Resource_Free(data);
		spr_loading_bar_ = new hgeSprite(tex_loading_bar_, 0.f, 0.f, static_cast<float>(hge_->Texture_GetWidth(tex_loading_bar_)),static_cast<float>(hge_->Texture_GetHeight(tex_loading_bar_)));

		//上边框条
		data = hge_->Resource_Load("lkpy\\images\\Frame\\game_title_bg.fish", &size);
		tex_GameFrame_Top_ = hge_->Texture_Load(static_cast<const char*>(data), size);
		hge_->Resource_Free(data);
		spr_GameFrame_Top_ = new hgeSprite(tex_GameFrame_Top_, 0.f, 0.f, static_cast<float>(hge_->Texture_GetWidth(tex_GameFrame_Top_)),static_cast<float>(hge_->Texture_GetHeight(tex_GameFrame_Top_)));
		//最小化
		data = hge_->Resource_Load("lkpy\\images\\Frame\\min_window.fish", &size);
		tex_GameFrame_Min_ = hge_->Texture_Load(static_cast<const char*>(data), size);
		hge_->Resource_Free(data);
		spr_GameFrame_Min_ = new hgeSprite(tex_GameFrame_Min_, 0.f, 0.f, static_cast<float>(hge_->Texture_GetWidth(tex_GameFrame_Min_)),static_cast<float>(hge_->Texture_GetHeight(tex_GameFrame_Min_)));
		spr_GameFrame_Min_->SetTextureRect(0,0,42,31);
		//关闭spr_GameFrame_Exit_
		data = hge_->Resource_Load("lkpy\\images\\Frame\\close_window.fish", &size);
		tex_GameFrame_Exit_ = hge_->Texture_Load(static_cast<const char*>(data), size);
		hge_->Resource_Free(data);
		spr_GameFrame_Exit_ = new hgeSprite(tex_GameFrame_Exit_, 0.f, 0.f, static_cast<float>(hge_->Texture_GetWidth(tex_GameFrame_Exit_)),static_cast<float>(hge_->Texture_GetHeight(tex_GameFrame_Exit_)));
		spr_GameFrame_Exit_->SetTextureRect(0,0,42,31);

		GuiManager::GetInstance().Initialize(screen_width_, screen_real_height);

		LPTSTR cmd_line;
		//初始化客户端
		//GameManager::GetInstance().InitClientKernel(cmd_line);

		//启动加载资源线程
		loading_handle_ = (HANDLE)::_beginthreadex(NULL, 0, LoadingThread, NULL, 0, NULL);
		//创建驱动
		MP_Device_WRAP device(screen_width_,screen_height_, hge_);
		device.Create();

		MP_Manager& MP=MP_Manager::GetInstance();

		MP_Platform* platform=new MP_Platform_WINDOWS;
		MP.Initialization(platform, MAGIC_INTERPOLATION_ENABLE, MAGIC_NOLOOP, MAGIC_CHANGE_EMITTER_DEFAULT, 1366, 768, 1,(float)screen_width_ / (float)kResolutionWidth, 0.1f, true);
		//加载所有元素
		MP.LoadAllEmitters();

		MP.RefreshAtlas();

		MP.CloseFiles();

		MP.Stop();

		hge_->System_Start();

		MP.Destroy();
		device.Destroy();

		delete spr_loading_bar_;
		hge_->Texture_Free(tex_loading_bar_);
		delete spr_logo_;
		hge_->Texture_Free(tex_logo_);
		delete spr_GameFrame_Top_;
		hge_->Texture_Free(tex_GameFrame_Top_);
		delete spr_GameFrame_Min_;
		hge_->Texture_Free(tex_GameFrame_Min_);
		if (loading_result == 0) TerminateThread(loading_handle_, 0); // 暂时性处理
		if (loading_handle_ != NULL) ::CloseHandle(loading_handle_);

			GuiManager::GetInstance().DelInstance();
			GameManager::GetInstance().DelInstance();
	}

	//标记客户端关闭;
	(CNewGameClient::GetMySelf())->m_bClose = true;

	hge_->System_Log("标记客户端关闭");

	hge_->System_Shutdown();
	hge_->Release();

	return 0;
}
//-----------------------------------------------------------------------------------



void MsgPrint(const char *lpszFormat, ...)
{
	va_list pArg;
	va_start(pArg, lpszFormat);
	char szMessage[1024] = { 0 };
#ifdef WIN32
	_vsnprintf(szMessage, 1023, lpszFormat, pArg);
#else
	vsnprintf(szMessage, 1023, lpszFormat, pArg);
#endif
	va_end(pArg);
	OutputDebugString(szMessage);
}
//-----------------------------------------------------------------------------------
IGameImpl *CreateNewGameImpl(IGameFrame* pIBase, IUserInterface *pIUI)
{
	return dynamic_cast<IGameImpl*>(new CNewGameClient(pIBase, pIUI));
}
//-----------------------------------------------------------------------------------
void DestroyNewGameImpl(IGameImpl **ppImp)
{
	delete *ppImp;
	*ppImp = NULL;
}
//-----------------------------------------------------------------------------------
CNewGameClient::CNewGameClient(IGameFrame *pGameFrame, IUserInterface *pUI)
{
	m_pGameFrame = pGameFrame;
	m_pUI = pUI;
	m_SoundEngineGame = createIrrKlangDevice();
	m_SoundEnginBg = createIrrKlangDevice() ; 
	m_pNewGameClient = this;
	m_bClose = false;
	m_iMyDeskStation = -1;
	m_GetGameStation = false;
	m_bIsWatching = false;//是否为旁观玩家
	::memset(m_UserInfo,NULL,sizeof(m_UserInfo));	/**<玩家信息*/
	InitialGameData() ; 

}
//-----------------------------------------------------------------------------------
CNewGameClient::~CNewGameClient()
{
	if(m_SoundEngineGame)
	{
		m_SoundEngineGame->drop();
	}

	if(m_SoundEnginBg)
	{
		m_SoundEnginBg->drop() ; 
	} 

}
//-----------------------------------------------------------------------------------
CNewGameClient * CNewGameClient::GetMySelf()
{
	if(m_pNewGameClient != NULL)
	{
		OutputDebugString("dwjgd::指针不为空");
		return m_pNewGameClient;
	}
	else 
	{
		OutputDebugString("dwjgd::指针错误");
		return NULL;
	}
}
//-----------------------------------------------------------------------------------
/// 初始化
int		CNewGameClient::Initial()
{
	m_pUI->SetShow(false);
	return 0;
}
//-----------------------------------------------------------------------------------
/// 初始化UI
int CNewGameClient::InitUI()
{	

	//启动加载资源线程
	StartHge_hanle_ = (HANDLE)::_beginthreadex(NULL, 0, StartHge, NULL, 0, NULL);
	m_pUI->SetShow(false);
	SetThreadPriority(loading_handle_,THREAD_PRIORITY_HIGHEST);
	m_pGameFrame->SetTimer(TIME_CLOSE,1000);
	return 0;
}

//--------------------------------------------------------------------------------
/// 重设游戏状态
int CNewGameClient::ResetGameFrame(void)
{
	OutputDebugString("dwjlkpy::ResetGameFrame(0)");
	//启动加载资源线程
	//StartHge_hanle_ = (HANDLE)::_beginthreadex(NULL, 0, StartHge, NULL, 0, NULL);
	//m_pUI->SetShow(false);
	//SetThreadPriority(loading_handle_,THREAD_PRIORITY_ABOVE_NORMAL);
	//m_pGameFrame->SetTimer(TIME_CLOSE,10);
	return 0;
}

//---------------------------------------------------------------------------------------
///初始化游戏中的数据
void	CNewGameClient::InitialGameData()
{
	
	m_iVersion=DEV_HEIGHT_VERSION;
	m_iVersion2=DEV_LOW_VERSION;
	m_bIsPlaying = false;

	//游戏设置
	LoadGameSet();
}
//---------------------------------------------------------------------------------------
///初始化部分数据
void	CNewGameClient::ResetGameData()
{
	m_bIsPlaying = false;
	
}
//---------------------------------------------------------------------------------------
//读取游戏设置
void	CNewGameClient::LoadGameSet()
{
	CString nid;
	nid.Format("%d",NAME_ID);
	CString s = CINIFile::GetAppPath ();/////本地路径
	CINIFile f( s +nid +"_c.ini");
	CString key = TEXT("Game");	
	//背景音效
	m_bSoundBgPlay	= ("1" == f.GetKeyVal(key,"BgSound","1"));
	//游戏音效
	m_bSoundPlay	= ("1" == f.GetKeyVal(key,"GameSound","1"));

}
//---------------------------------------------------------------------------------------
//保存游戏设置
void	CNewGameClient::SaveGameSet()
{
	CString nid;
	nid.Format("%d",NAME_ID);
	CString s = CINIFile::GetAppPath ();/////本地路径
	CINIFile f( s +nid +"_c.ini");
	CString key = TEXT("Game");	

	IRadioButton *pRadioButton = NULL;

	if (NULL != pRadioButton)
	{
		if (pRadioButton->GetIsSelect())
		{
			//本来没有播放 那么这里就要开始播放背景音乐了
			if (!m_bSoundBgPlay)
			{
				m_bSoundBgPlay = true;
				PlayBgSound(true);
			}
			
			f.SetKeyValString(key,"BgSound","1");
		}
		else
		{
			//如果正在播放 那么就要停止了
			if (m_bSoundBgPlay)
			{
				m_bSoundBgPlay = false;
				if (NULL != m_SoundEnginBg)
				{
					m_SoundEnginBg->stopAllSounds();
				}
			}
			f.SetKeyValString(key,"BgSound","0");
		}
	}
	//游戏音效

	if (NULL != pRadioButton)
	{
		if (pRadioButton->GetIsSelect())
		{
			m_bSoundPlay = true;
			f.SetKeyValString(key,"GameSound","1");
		}
		else
		{
			//如果正在播放音效 此时就要停止
			if (m_bSoundPlay)
			{
				m_bSoundPlay = false;
				if (NULL != m_SoundEngineGame)
				{
					m_SoundEngineGame->stopAllSounds();
				}
			}
			f.SetKeyValString(key,"GameSound","0");
		}
	}
}
//---------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
/// 设置游戏的当前状态
void CNewGameClient::SetGameStation()
{
	m_iMyDeskStation = m_pGameFrame->GetMyDeskStation() ;

	SetStationParameter(m_bufferForGameStation.game_status);

	switch(GetStationParameter())
	{
	case GAME_STATUS_FREE:
	case GAME_STATUS_PLAY:
		{
			if (NULL != GameManager::GetpGameManager())
			{
				//配置数据
				CMD_S_GameConfig* pCmd1 = &m_bufferForGameStation.GameConfig;
				GameManager::GetInstance().OnEventSocket(MDM_GM_GAME_NOTIFY, SUB_S_GAME_CONFIG, (void *)pCmd1, sizeof(CMD_S_GameConfig));
				//状态数据
				CMD_S_GameStatus* pCmd2 = &m_bufferForGameStation.GameStatus;
				GameManager::GetInstance().OnEventSceneMessage(GetStationParameter(), false, (void *)pCmd2, sizeof(CMD_S_GameStatus));
			}

			break;
		}
	default:
		{
			break;
		}
	}

	m_GetGameStation=true;
	//播放背景音效
	//PlayBgSound(true);
	return  ; 
}
//-----------------------------------------------------------------------------------------
/// 游戏消息
int CNewGameClient::HandleGameMessage(WORD nMainID,WORD nSubId,BYTE * buffer,int nLen)
{
	m_iMyDeskStation = m_pGameFrame->GetMyDeskStation() ; 

	if (!m_GetGameStation && nSubId != ASS_GM_GAME_STATION)//在等待资源加载时，如果收到的不是断线重回消息则丢弃
	{
		return true;
	}

	if (gbEndGame)
	{
		OutputDebugString("sdp_ 游戏窗口关闭 客户端 仍然收到消息");
		return true;
	}

	//GameManager::GetInstance().ShakeScreen();

	switch(nSubId)
	{
	case SUB_S_GAME_CONFIG:
	case SUB_S_FISH_TRACE://生成新鱼的轨迹
	case SUB_S_EXCHANGE_FISHSCORE:
	case SUB_S_USER_FIRE://玩家射击
	case SUB_S_CATCH_FISH:
	case SUB_S_BULLET_ION_TIMEOUT://子弹范围失效
	case SUB_S_LOCK_TIMEOUT://锁定鱼失效时间
	case SUB_S_CATCH_SWEEP_FISH:
	case SUB_S_CATCH_SWEEP_FISH_RESULT:
	case SUB_S_HIT_FISH_LK:
	case SUB_S_SWITCH_SCENE:
	case SUB_S_STOCK_OPERATE_RESULT:
	case SUB_S_SCENE_END:
	case SUB_S_GAMEOUT://服务端强行让客户端退出
	case SUB_S_CATCH_SWEEP_FISH_RESULT_EX:
		{
			if (loading_result != 1) //资源还没有加载完成
			{
				return true;
			}

			if (NULL != GameManager::GetpGameManager())
			{
				GameManager::GetInstance().OnEventSocket(MDM_GM_GAME_NOTIFY, nSubId,(void*)buffer, nLen);
			}
			return TRUE;
		}
	case ASS_GM_GAME_STATION:   ///<设置状态
		{
			if(nLen != sizeof(GameStation))
			{
				return TRUE;
			}

			GameStation * pCmd=(GameStation *)buffer;
			m_bIsWatching = m_pGameFrame->GetIsWatching();
			CopyMemory(&m_bufferForGameStation, pCmd, sizeof(GameStation));
			m_pGameFrame->SetTimer(TIME_WAIT_LOAD,50);
			return TRUE;      
		}
	case ASS_GM_AGREE_GAME:
		{		
			return TRUE;
		}
	default:
		{
			break;
		}
		
	}
	return 0;
}
//------------------------------------------------------------------------------
/// 界面消息
int CNewGameClient::OnUIMessage(TUIMessage* pMessage)
{
	return 0;
}
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------

//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------

/// 玩家进入
int CNewGameClient::GameUserCome(void)
{
	m_iMyDeskStation = m_pGameFrame->GetMyDeskStation() ; 	
	
	//获取玩家信息 并记录玩家是否在游戏中
	for(int i = 0; i < PLAY_COUNT; ++i)
	{
		m_pGameFrame->GetUserInfo(i,m_UserInfo[i]);
	}

	return 0;
}
/*--------------------------------------------------------------------------*/
/// 玩家离开
int		CNewGameClient::GameUserLeft(int nIndex)
{
	return 0;
}
/*--------------------------------------------------------------------------*/

/// 玩家断线
int CNewGameClient::GameUserOffLine(void)
{
	return 0;
}
// 是否正在游戏，由游戏实现
// 旁观玩家返回false，游戏玩家正在游戏中返回true，其它状态返回false
bool CNewGameClient::IsPlayingGame()
{
	if (m_bIsPlaying)	//游戏是否已经开始
	{
		return true;
	}
	return false;
}
//--------------------------------------------------------------------
/// 定时器消息（游戏中的定时器处理）
void CNewGameClient::OnTimer(int nTimerID)
{
 	switch(nTimerID)
 	{
	case TIME_WAIT_LOAD:
		{
			m_pGameFrame->KillTimer(TIME_WAIT_LOAD);
			if (NULL != GameManager::GetpGameManager())
			{
				if (GameManager::GetInstance().GetGameBeReady())
				{
					SetGameStation();
					return;
				}
			}


			m_pGameFrame->SetTimer(TIME_WAIT_LOAD,50);
		}
		break;
	case TIME_CLOSE:
		{
			m_pGameFrame->KillTimer(TIME_CLOSE);
			if(m_bClose)
			{
				if (StartHge_hanle_ == 0) TerminateThread(StartHge_hanle_, 0); // 暂时性处理
				if (StartHge_hanle_ != NULL) ::CloseHandle(StartHge_hanle_);
				m_pUI->SetShow(true);
				m_pGameFrame->KillTimer(TIME_CLOSE);
				m_pGameFrame->CloseClient();

				return;
			}
			m_pGameFrame->SetTimer(TIME_CLOSE,1000);
			break;
		}
	default:
		{
			break;
		}
	}
	return ; 

}
//-------------------------------------------------------------------------------
/// 置状态
void CNewGameClient::SetStationParameter(BYTE bGameStation)
{
	m_byGameStation = bGameStation ; 
}
//-------------------------------------------------------------------------------
///获取游戏状态
BYTE CNewGameClient::GetStationParameter()
{
	return m_byGameStation;
}
//-------------------------------------------------------------------------------
///玩家退出游戏
bool CNewGameClient::OnGameQuiting()
{
 	if(m_pGameFrame->GetIsWatching())
 	{
 		return true ; 
 	}
	return true;
}
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
//播放背景音效
void	CNewGameClient::PlayBgSound(bool bISLoop, int imusictag)
{
	if(!m_SoundEnginBg)
	{
		return ;
	}
	if(!m_bSoundBgPlay)
	{
		return ;
	}

	char szFileName[MAX_PATH];
	memset(szFileName,0,sizeof(szFileName)) ; 
	wsprintf(szFileName,"lkpy\\sounds\\bgm\\bgm%d.wav",imusictag+1);

	m_SoundEnginBg->setSoundVolume(0.5);
	m_SoundEnginBg->play2D(szFileName,bISLoop);
}
//-------------------------------------------------------------------------
//视图位置
int CNewGameClient::ViewStation(int desk)
{
	if (NULL != m_pGameFrame)
	{
		return m_pGameFrame->DeskStation2View(desk);
	}

	return 0;
}

//玩家是否存在
bool CNewGameClient::isPlayer(int desk)
{
	UserInfoForGame UserInfo;
	if (NULL != m_pGameFrame)
	{
		if (m_pGameFrame->GetUserInfo(desk, UserInfo))
		{
			return true;
		}
	}

	return false;
}

bool CNewGameClient::GetPlayerInfo(int desk, UserInfoForGame& UserInfo)
{
	UserInfoForGame UserInfo2;
	if (NULL != m_pGameFrame)
	{
		if (m_pGameFrame->GetUserInfo(desk, UserInfo2))
		{
			CopyMemory(&UserInfo, &UserInfo2, sizeof(UserInfoForGame));
			return true;
		}
	}

	return false;
}



/*-------------------------------------------------------------------*/
//转换座位
int		CNewGameClient::SwitchChairID(int m_Currchairid)
{
	int changChairID = m_Currchairid;
	switch(m_pGameFrame->GetMyDeskStation())
	{
	case 0:
	case 1:
	case 2:
		{
			switch(m_Currchairid)
			{
			case 0:return 3;
			case 1:return 4;
			case 2:return 5;
			case 3:return 0;
			case 4:return 1;
			case 5:return 2;
			}

		}
	case 3:
	case 4:
	case 5:
		{
			switch(m_Currchairid)
			{
			case 0:return 0;
			case 1:return 1;
			case 2:return 2;
			case 3:return 3;
			case 4:return 4;
			case 5:return 5;
			}

		}
	}
	return changChairID;
}