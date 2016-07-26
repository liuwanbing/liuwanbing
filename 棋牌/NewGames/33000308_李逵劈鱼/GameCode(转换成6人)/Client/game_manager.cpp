
#include "StdAfx.h"
#include "game_manager.h"

#include <process.h>

#include "math_aide.h"
#include "game_scene.h"
#include "fish_manager.h"
#include "cannon_manager.h"
#include "bullet_manager.h"
#include "coin_manager.h"
#include "jetton_manager.h"
#include "sound_manager.h"
#include "message.h"
#include "bingo.h"
#include "scene_fish_trace.h"
#include "sound_setting.h"
#include "lock_fish_manager.h"
#include "user_filter.h"
#include "IGameFrame.h"
#include "NewGameClient.h"

extern int screen_width_ ;
extern int screen_height_ ;

extern	hgeSprite* spr_GameFrame_Top_;
extern	hgeSprite* spr_GameFrame_Min_;
extern	hgeSprite* spr_GameFrame_Exit_;


extern	float g_hscale ;
extern	float g_vscale ;


const float kShakeFPS = 30.f;
const int kShakeFrames = 20;
const DWORD kKickOutTime = 60000;

GameManager* GameManager::game_manager_ = NULL;

GameManager::GameManager()
  : game_scene_(NULL),
    show_help_tip_(true),
    show_user_info_chairid_(INVALID_CHAIR),
    calc_trace_thread_(NULL),
    calc_trace_event_(NULL),
    exit_game_(false),
    game_ready_(false),
    game_active_(true),
    since_last_frame_(-1.f),
    current_shake_frame_(0),
    shake_screen_(false),
    current_bullet_kind_(BULLET_KIND_3_NORMAL),
    current_bullet_mulriple_(1000),
    allow_fire_(false),
    lock_(false),
    exchange_ratio_userscore_(1),
    exchange_ratio_fishscore_(1),
    exchange_count_(1),
    min_bullet_multiple_(1),
    max_bullet_multiple_(9900),
    bomb_range_width_(0.f),
    bomb_range_height_(0.f),
    last_fire_time_(0),
    last_scene_kind_(SCENE_KIND_1) 
{
  InitializeCriticalSection(&critial_section_);
  hge_ = hgeCreate(HGE_VERSION);
  //client_kernel_ = new ClientKernel();
  resource_manager_ = new hgeResourceManager();
  game_scene_ = new GameScene();
  fish_manager_ = new FishManager();
  cannon_manager_ = new CannonManager();
  bullet_manager_ = new BulletManager();
  coin_manager_ = new CoinManager();
  jetton_manager_ = new JettonManager();
  SoundManager::GetInstance();
  message_ = new Message();
  bingo_ = new Bingo();
  sound_setting_ = new SoundSetting();
  lock_fish_manager_ = new LockFishManager();
  user_filter_ = new UserFilter();

  offset_.x = 0;
  offset_.y = 0;

  for (int i = 0; i < FISH_KIND_COUNT; ++i) 
  {
    fish_multiple_[i] = 0;
    fish_speed_[i] = 0.f;
    fish_bounding_box_width_[i] = 0.f;
    fish_bounding_box_height_[i] = 0.f;
    fish_hit_radius_[i] = 0.f;
  }

  for (int i = 0; i < BULLET_KIND_COUNT; ++i) 
  {
    bullet_speed_[i] = 0.f;
    net_radius_[i] = 0.f;
  }

  memset(exchange_fish_score_, 0, sizeof(exchange_fish_score_));
  stock_message_[0] = 0;
  last_fire_time_ = timeGetTime();
  m_iKeyTag = 0;
}

GameManager::~GameManager() 
{
  if (calc_trace_thread_ != NULL) 
  {
    if (calc_trace_event_ != NULL && !exit_game_) 
	{
      exit_game_ = true;
      SetEvent(calc_trace_event_);
      WaitForSingleObject(calc_trace_thread_, INFINITE);
    }
    ::CloseHandle(calc_trace_thread_);
  }

  SAFE_DELETE(user_filter_);
  SAFE_DELETE(lock_fish_manager_);
  SAFE_DELETE(sound_setting_);
  
  stock_font_->Release();
  self_info_font_->Release();
  SoundManager::GetInstance().DelInstance();
  SAFE_DELETE(bingo_);
  SAFE_DELETE(message_);
  SAFE_DELETE(jetton_manager_);
  SAFE_DELETE(coin_manager_);
  SAFE_DELETE(bullet_manager_);
  SAFE_DELETE(cannon_manager_);
  SAFE_DELETE(fish_manager_);
  SAFE_DELETE(game_scene_);
  SAFE_DELETE(resource_manager_);

  if (calc_trace_event_ != NULL) ::CloseHandle(calc_trace_event_);
  //SAFE_DELETE(client_kernel_);
  hge_->Release();
  DeleteCriticalSection(&critial_section_);
}

GameManager& GameManager::GetInstance() 
{
  if (game_manager_ == NULL) 
	  game_manager_ = new GameManager;
  return *game_manager_;
}

GameManager* GameManager::GetpGameManager() 
{
	return game_manager_;
}

void GameManager::DelInstance() {
  SAFE_DELETE(game_manager_);
}

void GameManager::ResetGameClient() {
}

//关闭窗口sdp_Test
void GameManager::CloseGameClient() {
  PostMessage(hge_->System_GetState(HGE_HWND), WM_CLOSE, 0, 0);
}

bool GameManager::OnEventSocket(WORD main_cmdid, WORD sub_cmdid, void* data, WORD data_size) 
{
  if (main_cmdid != MDM_GM_GAME_NOTIFY) return false;
  if (!game_ready_) return true;
  if (game_manager_ == NULL)
  {
	  return true;
  }

  switch (sub_cmdid) 
  {
	case SUB_S_CATCH_SWEEP_FISH_RESULT_EX:
	  return OnSubSystem_messInfo(data, data_size);
    case SUB_S_GAME_CONFIG:
      return OnSubGameConfig(data, data_size);
    case SUB_S_FISH_TRACE:
      return OnSubFishTrace(data, data_size);
    case SUB_S_EXCHANGE_FISHSCORE:
      return OnSubExchangeFishScore(data, data_size);
    case SUB_S_USER_FIRE:
      return OnSubUserFire(data, data_size);
    case SUB_S_CATCH_FISH:
      return OnSubCatchFish(data, data_size);
    case SUB_S_BULLET_ION_TIMEOUT:
      return OnSubBulletIonTimeout(data, data_size);
    case SUB_S_LOCK_TIMEOUT:
      return OnSubLockTimeout(data, data_size);
    case SUB_S_CATCH_SWEEP_FISH:
      return OnSubCatSweepFish(data, data_size);
    case SUB_S_CATCH_SWEEP_FISH_RESULT:
      return OnSubCatSweepFishResult(data, data_size);
    case SUB_S_HIT_FISH_LK:
      return OnSubHitFishLK(data, data_size);
    case SUB_S_SWITCH_SCENE:
      return OnSubSwitchScene(data, data_size);
    case SUB_S_STOCK_OPERATE_RESULT:
      return OnSubStockOperateResult(data, data_size);
    case  SUB_S_SCENE_END:
      return OnSubSceneEnd(NULL, 0);
  }

  return false;
}

bool GameManager::OnEventSceneMessage(BYTE game_status, bool lookon_user, void* data, WORD data_size) {
  switch (game_status) {
    case GAME_STATUS_FREE:
    case GAME_STATUS_PLAY:
      assert(data_size == sizeof(CMD_S_GameStatus));
      if (data_size != sizeof(CMD_S_GameStatus)) 
	  {
		  return false;
	  }
	CMD_S_GameStatus* gamestatus = static_cast<CMD_S_GameStatus*>(data);
      if (gamestatus->game_version != GAME_VERSION) 
	 {
			  return false;
	  }
      for (WORD i = 0; i < GAME_PLAYER; ++i) 
	  {
        cannon_manager_->SetFishScore(i, gamestatus->fish_score[i]);
        exchange_fish_score_[i] = gamestatus->exchange_fish_score[i];
      }
      allow_fire_ = true;

      last_fire_time_ = timeGetTime();
	
      show_user_info_chairid_ = (CNewGameClient::GetMySelf())->m_iMyDeskStation;

      return true;
  }

  return false;
}

void GameManager::OnEventUserEnter(IGameUserItem* game_user_item, bool lookon_user) {

}

void GameManager::OnEventUserLeave(IGameUserItem* game_user_item, bool lookon_user) {
  cannon_manager_->ResetFishScore(game_user_item->GetChairID());
}

void GameManager::OnEventUserScore(IGameUserItem* game_user_item, bool lookon_user) {

}

void GameManager::OnEventUserStatus(IGameUserItem* game_user_item, bool lookon_user) {

}

void GameManager::OnSystemMessage(wchar_t sys_msg[], WORD msg_len, WORD type) {
  message_->AddMessage(sys_msg);
}

//CNewGameClient* GameManager::GetClientKernel() const {
//  //return client_kernel_;
//	return (CNewGameClient::GetMySelf());
//}

hgeResourceManager* GameManager::GetResourceManager() const {
  return resource_manager_;
}

FishManager* GameManager::GetFishManager() const {
  return fish_manager_;
}

CoinManager* GameManager::GetCoinManager() const {
  return coin_manager_;
}

CannonManager* GameManager::GetCannonManager() const {
  return cannon_manager_;
}

LockFishManager* GameManager::GetLockFishManager() const {
  return lock_fish_manager_;
}

bool GameManager::LoadGameResource(int& progress) 
{
	bool ret = true;
	do 
	{
		progress = 0;

		char font_dir[MAX_PATH] = { 0 };
		GetWindowsDirectoryA(font_dir, arraysize(font_dir));
		strcat(font_dir, "\\fonts\\SIMSUN.TTC");
		stock_font_ = FontSprite::CreateEx(font_dir, 30);
		stock_font_->SetColor(0xFFFF0000);

		self_info_font_ = FontSprite::CreateEx(font_dir, 30);
		self_info_font_->SetColor(0xFFFFFF00);

		resource_manager_->ChangeScript("lkpy\\content.fish");

		ret = game_scene_->LoadGameResource();
		hge_->System_Log("场景资源加载结果=%d",(ret == true)?1:0);
		if (!ret)
			break;
		progress = 10;

		ret = fish_manager_->LoadGameResource();
		hge_->System_Log("鱼资源加载结果=%d",(ret == true)?1:0);
		if (!ret) 
			break;
		progress = 30;

		ret = cannon_manager_->LoadGameResource();
		hge_->System_Log("大炮资源加载结果=%d",(ret == true)?1:0);
		if (!ret) 
			break;
		progress = 40;

		ret = bullet_manager_->LoadGameResource();
		hge_->System_Log("子弹资源加载结果=%d",(ret == true)?1:0);
		if (!ret) 
			break;
		progress = 60;

		ret = coin_manager_->LoadGameResource();
		hge_->System_Log("货币资源加载结果=%d",(ret == true)?1:0);
		if (!ret) 
			break;

		ret = jetton_manager_->LoadGameResource();
		hge_->System_Log("筹码资源加载结果=%d",(ret == true)?1:0);
		if (!ret) 
			break;
		ret = Message::LoadGameResource();
		hge_->System_Log("系统消息资源加载结果=%d",(ret == true)?1:0);
		if (!ret) 
			break;
		progress = 80;

		ret = bingo_->LoadGameResource();
		hge_->System_Log("bingo资源加载结果=%d",(ret == true)?1:0);
		if (!ret) 
			break;
		spr_self_info_ = resource_manager_->GetSprite("selfinfo");
		spr_user_score_num_ = resource_manager_->GetAnimation("userscore_num");
		hge_->System_Log("自己信息精灵创建结果=%d",(spr_self_info_ != NULL)?1:0);
		hge_->System_Log("自己分数动画创建结果=%d",(spr_user_score_num_ != NULL)?1:0);
		if (spr_self_info_ == NULL || spr_user_score_num_ == NULL) 
		{
			ret = false;
			break;
		}
		spr_cursor_ = resource_manager_->GetSprite("cursor");
		hge_->System_Log("指针精灵创建结果=%d",(spr_cursor_ != NULL)?1:0);
		if (spr_cursor_ == NULL) 
		{
			ret = false;
			break;
		}
		spr_help_tip_ = resource_manager_->GetSprite("help_tip");
		hge_->System_Log("帮助精灵创建结果=%d",(spr_help_tip_ != NULL)?1:0);
		if (spr_help_tip_ == NULL) 
		{
			ret = false;
			break;
		}
		bool tag = lock_fish_manager_->LoadGameResource();
		hge_->System_Log("锁定鱼资源加载结果=%d",(tag == true)?1:0);
		if (tag == NULL)
		{
			break;
		}
		progress = 90;

		ret = SoundManager::GetInstance().LoadGameResource();
		hge_->System_Log("声音资源加载结果=%d",(ret == true)?1:0);
		if (!ret) 
			break;
		progress = 110;

		float screen_width = static_cast<float>(hge_->System_GetState(HGE_SCREENWIDTH));
		float screen_height = static_cast<float>(hge_->System_GetState(HGE_SCREENHEIGHT));

		BuildSceneKind1Trace(screen_width, screen_height);
		BuildSceneKind2Trace(screen_width, screen_height);
		BuildSceneKind3Trace(screen_width, screen_height);
		BuildSceneKind4Trace(screen_width, screen_height);
		BuildSceneKind5Trace(screen_width, screen_height);
		progress = 130;

		// GUI
		ret = sound_setting_->Initialize();
		hge_->System_Log("声音初始化结果=%d",(ret == true)?1:0);
		if (!ret) 
			break;
		ret = user_filter_->Initialize();
		hge_->System_Log("用户过滤初始化结果=%d",(ret == true)?1:0);
		if (!ret) 
			break;

		if (calc_trace_thread_ == NULL) 
		{
			calc_trace_thread_ = (HANDLE)::_beginthreadex(NULL, 0, CalcTraceThread, static_cast<void*>(this), 0, NULL);
		}
		if (calc_trace_event_ == NULL) 
		{
			calc_trace_event_ = ::CreateEvent(NULL, TRUE, FALSE, NULL);
		}
		progress = 144;

		//client_kernel_->SendGameOption();
		game_ready_ = true;
		SoundManager::GetInstance().PlayBackMusic();
	} while (0);

	return ret;
}

bool GameManager::InitClientKernel(LPTSTR cmd_line) 
{
//  if (!client_kernel_->InitClientKernel(cmd_line)) return false;
//  client_kernel_->SetClientKernelSink(this);
  //client_kernel_->SetGameAttribute(KIND_ID, GAME_PLAYER, VERSION_CLIENT, GAME_NAME);
  //if (!client_kernel_->IsSingleMode()) client_kernel_->CreateConnect();
  return true;
}

void GameManager::HandleFrameMoveEvent() 
{
	if (!game_active_)
		return;

	float mx, my;
	hge_->Input_GetMousePos(&mx, &my);

	static bool mouse_down = false;
	static POINT last_cursor_pos = { 0, 0 };
	if (mx > screen_width_ - 89*g_hscale && mx <= (screen_width_-89*g_hscale + 42*g_hscale) && my<=31*g_vscale)
	{
		spr_GameFrame_Min_->SetTextureRect(42,0,42,31);
		spr_GameFrame_Exit_->SetTextureRect(0,0,42,31);
		if (hge_->Input_KeyDown(HGEK_LBUTTON))
		{
			ShowWindow(hge_->System_GetState(HGE_HWND), SW_MINIMIZE);
			spr_GameFrame_Min_->SetTextureRect(0,0,42,31);
		}
	}
	else if (mx > (screen_width_-89*g_hscale + 42*g_hscale) && mx <= (screen_width_-89*g_hscale + 42*g_hscale*2) && my<=31*g_vscale)
	{
		spr_GameFrame_Exit_->SetTextureRect(42,0,42,31);
		spr_GameFrame_Min_->SetTextureRect(0,0,42,31);
		if (hge_->Input_KeyDown(HGEK_LBUTTON))
		{
			PostMessage(hge_->System_GetState(HGE_HWND), WM_CLOSE, 0, 0);
			spr_GameFrame_Exit_->SetTextureRect(0,0,42,31);
		}
	}
	else if (/*mx < (kScreenWidth - 164) * g_hscale && */my <= 32 && hge_->Input_KeyDown(HGEK_LBUTTON)) 
	{
		GetCursorPos(&last_cursor_pos);
		mouse_down = true;
		spr_GameFrame_Min_->SetTextureRect(0,0,42,31);
		spr_GameFrame_Exit_->SetTextureRect(0,0,42,31);
	}
	else
	{
		spr_GameFrame_Min_->SetTextureRect(0,0,42,31);
		spr_GameFrame_Exit_->SetTextureRect(0,0,42,31);
	}

	if (hge_->Input_KeyUp(HGEK_LBUTTON))
		mouse_down = false;
	if (mouse_down) 
	{
		hgeInputEvent input_event;
		while (hge_->Input_GetEvent(&input_event)) 
		{
			if (input_event.type == INPUT_MOUSEMOVE) 
			{
				POINT current_pos;
				GetCursorPos(&current_pos);
				if (current_pos.x != last_cursor_pos.x || current_pos.y != last_cursor_pos.y) 
				{
					HWND hwnd = hge_->System_GetState(HGE_HWND);
					RECT rect;
					GetWindowRect(hwnd, &rect);
					OffsetRect(&rect, current_pos.x - last_cursor_pos.x, current_pos.y - last_cursor_pos.y);
					SetWindowPos(hwnd, NULL, rect.left, rect.top, 0, 0, SWP_NOSIZE);
				}
				last_cursor_pos.x = current_pos.x;
				last_cursor_pos.y = current_pos.y;
			}
		}
	}
}

bool GameManager::OnFrame(float delta_time) 
{
	m_iKeyTag++;
	if (m_iKeyTag >= 10)
	{
		m_iKeyTag = 0;
	}

	bool bIsWatching = (CNewGameClient::GetMySelf())->m_bIsWatching;//是否是旁观者
	DWORD now_time = timeGetTime();
	WORD me_chair_id = (CNewGameClient::GetMySelf())->m_iMyDeskStation;

	HandleFrameMoveEvent();
	//WORD me_chair_id = client_kernel_->GetMeChairID();

	if (me_chair_id < GAME_PLAYER && !bIsWatching) 
	{
		if (hge_->Input_KeyDown(HGEK_F1)) 
		{
			SendStockOperate(0);
		} 
		else if (hge_->Input_KeyDown(HGEK_F2)) 
		{
			SendStockOperate(1);
		} 
		else if ((hge_->Input_GetKeyState(HGEK_F3) && m_iKeyTag == 0) || hge_->Input_KeyDown(HGEK_F3)) 
		{
			SendStockOperate(2);
		} 
		else if (hge_->Input_KeyDown(HGEK_F4)) 
		{
			sound_setting_->ShowSoundSetting(true);
		} 
		else if (hge_->Input_KeyDown(HGEK_S)) 
		{
			FishKind lock_fish_kind;
			lock_fish_manager_->SetLockFishID(me_chair_id, fish_manager_->LockFish(&lock_fish_kind, lock_fish_manager_->GetLockFishID(me_chair_id),
				lock_fish_manager_->GetLockFishKind(me_chair_id)));
			lock_fish_manager_->SetLockFishKind(me_chair_id, lock_fish_kind);
		} 
		else if (hge_->Input_KeyDown(HGEK_Q)) 
		{
			lock_fish_manager_->ClearLockTrace(me_chair_id);
		} 
		else if (hge_->Input_KeyDown(HGEK_F5)) 
		{
			//if (CUserRight::IsGameCheatUser(client_kernel_->GetUserAttribute().user_right)) 
			{
				//user_filter_->ShowUserFilter(true);
			}
		} 
		else if (hge_->Input_KeyDown(HGEK_F6)) 
		{
			SendStockOperate(3);
		}
	}

	if (allow_fire_ && me_chair_id < GAME_PLAYER  && !bIsWatching) 
	{
		if ((hge_->Input_GetKeyState(HGEK_UP) && m_iKeyTag == 0) || hge_->Input_KeyDown(HGEK_UP)) 
		{
			if (current_bullet_mulriple_ == max_bullet_multiple_) 
			{
				current_bullet_mulriple_ = min_bullet_multiple_;
			} 
			else if (current_bullet_mulriple_ < 100) 
			{
				current_bullet_mulriple_ += 10;
				if (current_bullet_mulriple_ > max_bullet_multiple_)
					current_bullet_mulriple_ = max_bullet_multiple_;
			} 
			else if (current_bullet_mulriple_ >= 100 && current_bullet_mulriple_ < 1000) 
			{
				current_bullet_mulriple_ += 100;
				if (current_bullet_mulriple_ > max_bullet_multiple_)
					current_bullet_mulriple_ = max_bullet_multiple_;
			} 
			else 
			{
				current_bullet_mulriple_ += 1000;
				if (current_bullet_mulriple_ > max_bullet_multiple_)
					current_bullet_mulriple_ = max_bullet_multiple_;
			}

			if (current_bullet_mulriple_ > 10)
			{
				//大于10的数字清除掉个位数
				current_bullet_mulriple_ = current_bullet_mulriple_ / 10 * 10;
			}

			if (current_bullet_mulriple_ < 100) 
			{
				current_bullet_kind_ = static_cast<BulletKind>((current_bullet_kind_ / 4) * 4);
			} 
			else if (current_bullet_mulriple_ >= 100 && current_bullet_mulriple_ < 1000) 
			{
				current_bullet_kind_ = static_cast<BulletKind>((current_bullet_kind_ / 4) * 4 + 1);
			} 
			else if (current_bullet_mulriple_ >= 1000 && current_bullet_mulriple_ < 5000) 
			{
				current_bullet_kind_ = static_cast<BulletKind>((current_bullet_kind_ / 4) * 4 + 2);
			} 
			else 
			{
				current_bullet_kind_ = static_cast<BulletKind>((current_bullet_kind_ / 4) * 4 + 3);
			}
			cannon_manager_->Switch(me_chair_id, current_bullet_kind_);
			cannon_manager_->SetCannonMulriple(me_chair_id, current_bullet_mulriple_);
			SoundManager::GetInstance().PlayGameEffect(CANNON_SWITCH);
		} 
		else if ((hge_->Input_GetKeyState(HGEK_DOWN) && m_iKeyTag == 0) || hge_->Input_KeyDown(HGEK_DOWN)) 
		{
			if (current_bullet_mulriple_ == min_bullet_multiple_) 
			{
				current_bullet_mulriple_ = max_bullet_multiple_;
			}
			else if (current_bullet_mulriple_ <= 100) 
			{
				current_bullet_mulriple_ -= 10;
				if (current_bullet_mulriple_ < min_bullet_multiple_)
					current_bullet_mulriple_ = min_bullet_multiple_;
			}
			else if (current_bullet_mulriple_ > 100 && current_bullet_mulriple_ <= 1000) 
			{
				current_bullet_mulriple_ -= 100;
				if (current_bullet_mulriple_ < min_bullet_multiple_)
					current_bullet_mulriple_ = min_bullet_multiple_;
			} 
			else 
			{
				current_bullet_mulriple_ -= 1000;
				if (current_bullet_mulriple_ < min_bullet_multiple_)
					current_bullet_mulriple_ = min_bullet_multiple_;
			}

			if (current_bullet_mulriple_ < 100) 
			{
				current_bullet_kind_ = static_cast<BulletKind>((current_bullet_kind_ / 4) * 4);
			} 
			else if (current_bullet_mulriple_ >= 100 && current_bullet_mulriple_ < 1000) 
			{
				current_bullet_kind_ = static_cast<BulletKind>((current_bullet_kind_ / 4) * 4 + 1);
			} 
			else if (current_bullet_mulriple_ >= 1000 && current_bullet_mulriple_ < 5000) 
			{
				current_bullet_kind_ = static_cast<BulletKind>((current_bullet_kind_ / 4) * 4 + 2);
			} 
			else 
			{
				current_bullet_kind_ = static_cast<BulletKind>((current_bullet_kind_ / 4) * 4 + 3);
			}
			cannon_manager_->Switch(me_chair_id, current_bullet_kind_);
			cannon_manager_->SetCannonMulriple(me_chair_id, current_bullet_mulriple_);
			SoundManager::GetInstance().PlayGameEffect(CANNON_SWITCH);
		} 
		else if ((hge_->Input_GetKeyState(HGEK_LEFT) && m_iKeyTag == 0) || hge_->Input_KeyDown(HGEK_LEFT)) //减少
		{
			SendExchangeFishScore(false);
		} else if ((hge_->Input_GetKeyState(HGEK_RIGHT) && m_iKeyTag == 0) || hge_->Input_KeyDown(HGEK_RIGHT))//增加
		{
			SendExchangeFishScore(true);
		}

		FPoint mouse_pos, muzzle_pos;
		if (lock_fish_manager_->GetLockFishID(me_chair_id) != 0) 
		{
			mouse_pos = lock_fish_manager_->LockPos(me_chair_id);
		} 
		else 
		{
			hge_->Input_GetMousePos(&mouse_pos.x, &mouse_pos.y);
		}

		if (mouse_pos.y <= 32.0f*g_hscale)
		{
			
		}
		else
		{
			FPoint cannon_pos = cannon_manager_->GetCannonPos((CNewGameClient::GetMySelf())->SwitchChairID(me_chair_id)/*, &muzzle_pos*/);
			bool can_fire = CanFire((CNewGameClient::GetMySelf())->SwitchChairID(me_chair_id), mouse_pos);
			float angle = MathAide::CalcAngle(mouse_pos.x, mouse_pos.y, cannon_pos.x, cannon_pos.y);
			//设置炮台角度
			cannon_manager_->SetCurrentAngle(me_chair_id, angle);
			if (game_active_ 
				&& can_fire 
				&& ( (hge_->Input_GetKeyState(HGEK_LBUTTON) && m_iKeyTag == 0) || hge_->Input_KeyDown(HGEK_LBUTTON)
				|| (hge_->Input_GetKeyState(HGEK_SPACE) && m_iKeyTag == 0) || hge_->Input_KeyDown(HGEK_SPACE))) 
			{
		
				static const DWORD kFireInterval = 300;
				if (now_time - last_fire_time_ >= kFireInterval) 
				{
					SCORE me_fish_score = cannon_manager_->GetFishScore(me_chair_id);
					if (current_bullet_mulriple_ < min_bullet_multiple_ || current_bullet_mulriple_ > max_bullet_multiple_) 
					{
						current_bullet_mulriple_ = min_bullet_multiple_;
						current_bullet_kind_ = static_cast<BulletKind>((current_bullet_kind_ / 4) * 4);
						cannon_manager_->Switch(me_chair_id, current_bullet_kind_);
						cannon_manager_->SetCannonMulriple(me_chair_id, current_bullet_mulriple_);
					}
					int bullet_mulriple = current_bullet_mulriple_;
					if (me_fish_score >= bullet_mulriple) 
					{
						//cannon_manager_->SetFishScore(me_chair_id, -bullet_mulriple);//sdp_收到消息后做了重复处理
						//cannon_manager_->Fire(me_chair_id, current_bullet_kind_);//sdp_收到消息后做了重复处理

						//bullet_manager_->Fire(muzzle_pos.x, muzzle_pos.y, angle, current_bullet_kind_,
						//1, current_bullet_mulriple_, me_chair_id, bullet_speed_[current_bullet_kind_],
						//net_radius_[current_bullet_kind_], INVALID_CHAIR);
	
						SendUserFire(current_bullet_kind_, angle, current_bullet_mulriple_, lock_fish_manager_->GetLockFishID(me_chair_id));
						//SoundManager::GetInstance().PlayGameEffect(FIRE);//sdp_收到消息后做了重复处理
						last_fire_time_ = now_time;

					}
				}
			}
		}
	}

	
	UpdateShakeScreen(delta_time);



	bool switch_finish = game_scene_->OnFrame(delta_time);
	CString dwjlog;
	dwjlog.Format("dwjlkpy1::switch_finish=%d",switch_finish);
	OutputDebugString(dwjlog);
	if (switch_finish) 
	{
		
		fish_manager_->FreeSceneSwitchFish();
		AllowFire(true);
		SoundManager::GetInstance().PlayBackMusic();
	}

	fish_manager_->OnFrame(delta_time, lock_);

	cannon_manager_->OnFrame(delta_time);

	bullet_manager_->OnFrame(delta_time);

	lock_fish_manager_->OnFrame(delta_time);

	coin_manager_->OnFrame(delta_time);

	jetton_manager_->OnFrame(delta_time);
	
	message_->OnFrame(delta_time);
	bingo_->OnFrame(delta_time);
	if (!bIsWatching)
	{
		if (now_time - last_fire_time_ >0 && now_time - last_fire_time_ >= kKickOutTime + 10000) 
		{
			hge_->System_Log("GameManager::OnFrame(监测坐标10:now_time=%d------last_fire_time_=%d------kKickOutTime=%d---------------)",now_time,last_fire_time_,kKickOutTime);
			return true;
		}
	}
	return false;
}

bool GameManager::OnSubSystem_messInfo(void* data, WORD data_size)
{
	assert(data_size == sizeof(CMD_S_CatchSweepFishResultEx));
	if (data_size != sizeof(CMD_S_CatchSweepFishResultEx)) 
	{
		return false;
	}
	CMD_S_CatchSweepFishResultEx* pCmd = static_cast<CMD_S_CatchSweepFishResultEx*>(data);
	//玩家昵称转换
	DWORD dwNum = MultiByteToWideChar (CP_ACP, 0, pCmd->szNickName, -1, NULL, 0);
	wchar_t *userName = NULL;
	userName = new wchar_t[dwNum];
	if(!userName)
		delete []userName;
	MultiByteToWideChar (CP_ACP, 0, pCmd->szNickName, -1,  userName, dwNum);
	//房间名字转换
	dwNum = MultiByteToWideChar (CP_ACP, 0, pCmd->cRoomName, -1, NULL, 0);
	wchar_t *RoomName = NULL;
	RoomName = new wchar_t[dwNum];
	if(!RoomName)
		delete []RoomName;
	MultiByteToWideChar (CP_ACP, 0, pCmd->cRoomName, -1,  RoomName, dwNum);
	//鱼名字转换
	dwNum = MultiByteToWideChar (CP_ACP, 0, pCmd->cFishName, -1, NULL, 0);
	wchar_t *FishName = NULL;
	FishName = new wchar_t[dwNum];
	if(!FishName)
		delete []FishName;
	MultiByteToWideChar (CP_ACP, 0, pCmd->cFishName, -1,  FishName, dwNum);


	wchar_t sys_msg[MAX_PATH] = { 0 };
		_snwprintf(sys_msg, arraysize(sys_msg), L"%s(%d:%d)%d号桌的[%s]被[%s]打死了，获得%I64d鱼币奖励！",
		RoomName,pCmd->exchange_ratio_userscore_,pCmd->exchange_ratio_fishscore_,pCmd->iDeskID,FishName,userName,pCmd->fish_score);

	OnSystemMessage(sys_msg, 0, 0);

	delete []userName;
	delete []RoomName;
	delete []FishName;
	return true;
}

bool GameManager::OnRender(float hscale, float vscale) 
{
	bool bIsWatching = (CNewGameClient::GetMySelf())->m_bIsWatching;//是否是旁观者

	float screen_width = static_cast<float>(hge_->System_GetState(HGE_SCREENWIDTH));
	float screen_height = static_cast<float>(hge_->System_GetState(HGE_SCREENHEIGHT));

	game_scene_->OnRender(offset_.x, offset_.y, hscale, vscale);//场景
	fish_manager_->OnRender(offset_.x, offset_.y, hscale, vscale);//鱼
	bullet_manager_->OnRender(offset_.x, offset_.y, hscale, vscale);//子弹

	game_scene_->OnRender(0.f, 0.f, hscale, vscale, true);//场景
	cannon_manager_->OnRender(0.f, 0.f, hscale, vscale);//火炮
	lock_fish_manager_->OnRender(offset_.x, offset_.y, hscale, vscale);//锁定的鱼
	coin_manager_->OnRender(offset_.x, offset_.y, hscale, vscale);//鱼币
	jetton_manager_->OnRender(0.f, 0.f, hscale, vscale);//筹码

	spr_GameFrame_Top_->RenderEx(0.0f, 0.0f, 0.0f, hscale, vscale);//20141219上边框绘制
	float Min_xPos = screen_width - 89.0*hscale;
	spr_GameFrame_Min_->RenderEx(Min_xPos,0.0f,0.0f,hscale, vscale);
	float Eixt_xPos = screen_width - 89.0*hscale+42*hscale;
	spr_GameFrame_Exit_->RenderEx(Eixt_xPos,0.0f,0.0f,hscale, vscale);

	bingo_->OnRender(0.f, 0.f, hscale, vscale);


	if (!bIsWatching)
	{
		DWORD leave_time = timeGetTime() - last_fire_time_ ;
		if (leave_time > kKickOutTime && leave_time - kKickOutTime < 10000) 
		{
			wchar_t tip_msg[128] = { 0 };
			_snwprintf(tip_msg, arraysize(tip_msg), L"由于您一分钟未发射子弹系统将在 %u 秒后离开游戏！", (10000 - (leave_time - kKickOutTime)) / 1000);
			SIZE text_size = stock_font_->GetTextSize(tip_msg);
			stock_font_->RenderEx((screen_width - text_size.cx) / 2, (screen_height - text_size.cy) / 2, tip_msg, hscale);
		} 
		else 
		{
			message_->OnRender(hscale, vscale);
		}
	}

	// 用户信息
	spr_self_info_->RenderEx(0.f, screen_height, 0.f, hscale, vscale);
	if ((CNewGameClient::GetMySelf())->isPlayer(show_user_info_chairid_)) 
	{
		UserInfoForGame UserInfo;
		(CNewGameClient::GetMySelf())->GetPlayerInfo(show_user_info_chairid_, UserInfo);
		DWORD dwNum = MultiByteToWideChar (CP_ACP, 0, UserInfo.szNickName, -1, NULL, 0);
		wchar_t *userName = NULL;
		userName = new wchar_t[dwNum];
		if(!userName)
		{
			delete []userName;
		}

		MultiByteToWideChar (CP_ACP, 0, UserInfo.szNickName, -1,  userName, dwNum);

		self_info_font_->Render(70.f * hscale, screen_height - 34.f * vscale, userName);
		SCORE user_leave_score = (SCORE)UserInfo.i64Money - exchange_fish_score_[show_user_info_chairid_] * exchange_ratio_userscore_ / exchange_ratio_fishscore_;
		RenderUserScoreNum(user_leave_score, 518.f * hscale, screen_height - 20 * vscale, 0.f, hscale, vscale);

		delete []userName;
	}
	else
	{
		CString dwjlog;
		dwjlog.Format("dwjlkpy2::show_user_info_chairid_=%d",show_user_info_chairid_);
		OutputDebugString(dwjlog);
		wchar_t userName[MAX_PATH] = { 0 };
		_snwprintf(userName, arraysize(userName),L" ");
		self_info_font_->Render(70.f * hscale, screen_height - 34.f * vscale, userName);
		RenderUserScoreNum(0, 518.f * hscale, screen_height - 20 * vscale, 0.f, hscale, vscale);
	}

	if (/*CUserRight::IsGameCheatUser(client_kernel_->GetUserAttribute().user_right) && */stock_message_[0] != 0) 
	{
		stock_font_->RenderEx(0.f, screen_height - stock_font_->GetFontSize() - 100, stock_message_, hscale);
	}

	if (show_help_tip_ && (hge_->Input_GetKeyState(HGEK_LBUTTON) && m_iKeyTag == 0) || hge_->Input_KeyDown(HGEK_LBUTTON)) 
	{
		float tip_hotx, tip_hoty;
		spr_help_tip_->GetHotSpot(&tip_hotx, &tip_hoty);
		hgeRect rect;
		rect.Set(screen_width / 2 - 50, screen_height / 2 + tip_hoty - 50, screen_width / 2 + 50, screen_height / 2 + tip_hoty - 10);
		FPoint mouse_pos;
		hge_->Input_GetMousePos(&mouse_pos.x, &mouse_pos.y);
		if (rect.TestPoint(mouse_pos.x, mouse_pos.y))
			show_help_tip_ = false;
	}
	if (show_help_tip_)
	{
		spr_help_tip_->Render(screen_width / 2, screen_height / 2);
	}
	
	FPoint mouse_pos;
	hge_->Input_GetMousePos(&mouse_pos.x, &mouse_pos.y);
	spr_cursor_->RenderEx(mouse_pos.x, mouse_pos.y, 0.f, hscale, vscale);

	
	return false;
}

void GameManager::SendExchangeFishScore(bool increase) 
{
	//if (client_kernel_->IsLookonMode() || client_kernel_->IsSingleMode()) return;
	if ((CNewGameClient::GetMySelf())->m_bIsWatching)
	{
		return;
	}

	WORD me_chair_id = (CNewGameClient::GetMySelf())->m_iMyDeskStation;

	CMD_C_ExchangeFishScore exchange_fishscore;
	exchange_fishscore.increase = increase;

	UserInfoForGame UserInfo;
	(CNewGameClient::GetMySelf())->GetPlayerInfo(me_chair_id, UserInfo);

	SCORE me_fish_score = cannon_manager_->GetFishScore(me_chair_id);
	SCORE need_user_score = exchange_ratio_userscore_ * exchange_count_ / exchange_ratio_fishscore_;
	SCORE user_leave_score = (SCORE)UserInfo.i64Money - exchange_fish_score_[me_chair_id] * exchange_ratio_userscore_ / exchange_ratio_fishscore_;
	if (increase) 
	{
		if (need_user_score > user_leave_score) //玩家钱不够扣则离开
		{
			return;
		}
		//cannon_manager_->SetFishScore(me_chair_id, exchange_count_);
		//exchange_fish_score_[me_chair_id] += exchange_count_;
	} 
	else 
	{
		if (me_fish_score <= 0) //玩家鱼币已经为0,不能再退币了 sdp2014-05-09 为0 点击退币则上最大分，但是为了美观，充满显示框就可以了
		{
			/*return;*/
		}
		//exchange_fish_score_[me_chair_id] -= me_fish_score;
		//cannon_manager_->SetFishScore(me_chair_id, -me_fish_score);
	}
	SendSocketData(SUB_C_EXCHANGE_FISHSCORE, &exchange_fishscore, sizeof(exchange_fishscore));
}

void GameManager::SendUserFire(BulletKind bullet_kind, float angle, int bullet_mulriple, int lock_fishid) 
{
  CMD_C_UserFire user_fire;
  user_fire.bullet_kind = bullet_kind;
  user_fire.angle = angle;//视角
  user_fire.bullet_mulriple = bullet_mulriple;//子弹倍率
  user_fire.lock_fishid = lock_fishid;//锁定鱼ID

  SendSocketData(SUB_C_USER_FIRE, &user_fire, sizeof(user_fire));
}

void GameManager::SendCatchFish(int fish_id, WORD firer_chair_id, int bullet_id, BulletKind bullet_kind, int bullet_mulriple) 
{

	//不是本人或者此人不是机器人的子弹不传（机器人打中鱼是通过真人客户端告诉给服务端的）
	if (firer_chair_id != (CNewGameClient::GetMySelf())->m_iMyDeskStation && !(CNewGameClient::GetMySelf())->m_UserInfo[firer_chair_id].bIsRobot) 
		return;

	CMD_C_CatchFish catch_fish;
	catch_fish.chair_id = firer_chair_id;
	catch_fish.bullet_id = bullet_id;
	catch_fish.bullet_kind = bullet_kind;
	catch_fish.fish_id = fish_id;
	catch_fish.bullet_mulriple = bullet_mulriple;
	OutputDebugString("sdp_ 机器人打鱼   SendCatchFish");
	SendSocketData(SUB_C_CATCH_FISH, &catch_fish, sizeof(catch_fish));
}

void GameManager::SendCatchSweepFish(CMD_C_CatchSweepFish* catch_sweep_fish) {
  SendSocketData(SUB_C_CATCH_SWEEP_FISH, catch_sweep_fish, sizeof(CMD_C_CatchSweepFish));
}

void GameManager::SendHitFishLK(WORD firer_chair_id, int fish_id) {
  //if (firer_chair_id != client_kernel_->GetMeChairID()) return;
  CMD_C_HitFishLK hit_fish;
  hit_fish.fish_id = fish_id;
  SendSocketData(SUB_C_HIT_FISH_I, &hit_fish, sizeof(hit_fish));
}

void GameManager::SendStockOperate(unsigned char operate_code) {
  //if (!CUserRight::IsGameCheatUser(client_kernel_->GetUserAttribute().user_right)) return;

  CMD_C_StockOperate stock_operate;
  stock_operate.operate_code = operate_code;
  SendSocketData(SUB_C_STOCK_OPERATE, &stock_operate, sizeof(stock_operate));
}

void GameManager::SendUserFilter(DWORD game_id, unsigned char operate_code) {
  //if (!CUserRight::IsGameCheatUser(client_kernel_->GetUserAttribute().user_right)) return;

  CMD_C_UserFilter user_filter;
  user_filter.game_id = game_id;
  user_filter.operate_code = operate_code;
  SendSocketData(SUB_C_USER_FILTER, &user_filter, sizeof(user_filter));
}

void GameManager::SendFish20Config(DWORD game_id, int catch_count, double catch_probability) {
  //if (!CUserRight::IsGameCheatUser(client_kernel_->GetUserAttribute().user_right)) return;

  CMD_C_Fish20Config fish20_config;
  fish20_config.game_id = game_id;
  fish20_config.catch_count = catch_count;
  fish20_config.catch_probability = catch_probability;
  SendSocketData(SUB_C_FISH20_CONFIG, &fish20_config, sizeof(fish20_config));
}

bool GameManager::SendSocketData(WORD sub_cmdid) 
{
	(CNewGameClient::GetMySelf())->m_pGameFrame->SendGameData(NULL,0,MDM_GM_GAME_NOTIFY,sub_cmdid,0);
	return true;
}

bool GameManager::SendSocketData(WORD sub_cmdid, void* data, WORD data_size) 
{
	(CNewGameClient::GetMySelf())->m_pGameFrame->SendGameData(data,data_size,MDM_GM_GAME_NOTIFY,sub_cmdid,0);
	return true;
}

bool GameManager::OnSubGameConfig(void* data, WORD data_size) {
  assert(data_size == sizeof(CMD_S_GameConfig));
  if (data_size != sizeof(CMD_S_GameConfig)) 
  {
	  return false;
  }
  CMD_S_GameConfig* game_config = static_cast<CMD_S_GameConfig*>(data);

  float screen_width = static_cast<float>(hge_->System_GetState(HGE_SCREENWIDTH));
  float hscale = screen_width / kResolutionWidth;

  exchange_ratio_userscore_ = game_config->exchange_ratio_userscore;
  exchange_ratio_fishscore_ = game_config->exchange_ratio_fishscore;
  exchange_count_ = game_config->exchange_count;

  min_bullet_multiple_ = game_config->min_bullet_multiple;
  max_bullet_multiple_ = game_config->max_bullet_multiple;
  current_bullet_mulriple_ = min_bullet_multiple_;
  if (current_bullet_mulriple_ < 100) 
  {
    current_bullet_kind_ = static_cast<BulletKind>((current_bullet_kind_ / 4) * 4);
  } 
  else if (current_bullet_mulriple_ >= 100 && current_bullet_mulriple_ < 1000) 
  {
    current_bullet_kind_ = static_cast<BulletKind>((current_bullet_kind_ / 4) * 4 + 1);
  } 
  else if (current_bullet_mulriple_ >= 1000 && current_bullet_mulriple_ < 5000) 
  {
    current_bullet_kind_ = static_cast<BulletKind>((current_bullet_kind_ / 4) * 4 + 2);
  } 
  else 
  {
    current_bullet_kind_ = static_cast<BulletKind>((current_bullet_kind_ / 4) * 4 + 3);
  }
  for (WORD i = 0; i < GAME_PLAYER; ++i) 
  {
    cannon_manager_->Switch(i, current_bullet_kind_);
    cannon_manager_->SetCannonMulriple(i, current_bullet_mulriple_);
  }

  bomb_range_width_ = static_cast<float>(game_config->bomb_range_width);
  bomb_range_height_ = static_cast<float>(game_config->bomb_range_height);

  for (int i = 0; i < FISH_KIND_COUNT; ++i) 
  {
    fish_multiple_[i] = game_config->fish_multiple[i];
    fish_speed_[i] = static_cast<float>(game_config->fish_speed[i]) * hscale;
    fish_bounding_box_width_[i] = static_cast<float>(game_config->fish_bounding_box_width[i]) * hscale;
    fish_bounding_box_height_[i] = static_cast<float>(game_config->fish_bounding_box_height[i]) * hscale;
    fish_hit_radius_[i] = static_cast<float>(game_config->fish_hit_radius[i]) * hscale;
  }
  for (int i = 0; i < BULLET_KIND_COUNT; ++i) 
  {
    bullet_speed_[i] = static_cast<float>(game_config->bullet_speed[i]) * hscale;
    net_radius_[i] = static_cast<float>(game_config->net_radius[i]) * hscale;
  }

  return true;
}

bool GameManager::OnSubFishTrace(void* data, WORD data_size) 
{
	assert(data_size % sizeof(CMD_S_FishTrace) == 0);
	if (data_size % sizeof(CMD_S_FishTrace) != 0) 
		return false;
	if (game_scene_ == NULL)
	{
		return false;
	}
	if (game_scene_->IsSwitchingScene()) 
		return true;

	float screen_width = static_cast<float>(hge_->System_GetState(HGE_SCREENWIDTH));
	float screen_height = static_cast<float>(hge_->System_GetState(HGE_SCREENHEIGHT));
	float hscale = screen_width / kResolutionWidth;
	float vscale = screen_height / kResolutionHeight;

	CMD_S_FishTrace* fish_trace = static_cast<CMD_S_FishTrace*>(data);
	WORD fish_trace_count = data_size / sizeof(CMD_S_FishTrace);

	FishTraceInfo fish_trace_info = { 0 };
	EnterCriticalSection(&critial_section_);

	for (WORD i = 0; i < fish_trace_count; ++i) 
	{
		fish_trace_info.init_count = fish_trace->init_count;
		for (int j = 0; j < fish_trace_info.init_count; ++j) 
		{
			fish_trace_info.init_x_pos[j] = fish_trace->init_pos[j].x * hscale;
			fish_trace_info.init_y_pos[j] = fish_trace->init_pos[j].y * vscale;
		}
		fish_trace_info.fish = NULL;
		fish_trace_info.fish = fish_manager_->ActiveFish(fish_trace->fish_kind, fish_trace->fish_id, fish_multiple_[fish_trace->fish_kind],
			fish_speed_[fish_trace->fish_kind], fish_bounding_box_width_[fish_trace->fish_kind], fish_bounding_box_height_[fish_trace->fish_kind], fish_hit_radius_[fish_trace->fish_kind]);
		if(fish_trace_info.fish != NULL)
		{
			fish_trace_info.fish->set_trace_type(fish_trace->trace_type);
		}
		fish_trace_info_buffer_.push_back(fish_trace_info);
		++fish_trace;
	}
	LeaveCriticalSection(&critial_section_);
	SetEvent(calc_trace_event_);
	return true;
}

bool GameManager::OnSubExchangeFishScore(void* data, WORD data_size) 
{
  assert(data_size == sizeof(CMD_S_ExchangeFishScore));
  if (data_size != sizeof(CMD_S_ExchangeFishScore)) 
	  return false;
  CMD_S_ExchangeFishScore* exchange_fishscore = static_cast<CMD_S_ExchangeFishScore*>(data);

  cannon_manager_->ResetFishScore(exchange_fishscore->chair_id);
  cannon_manager_->SetFishScore(exchange_fishscore->chair_id, exchange_fishscore->swap_fish_score);
  exchange_fish_score_[exchange_fishscore->chair_id] = exchange_fishscore->exchange_fish_score;

  return true;
}

bool GameManager::OnSubUserFire(void* data, WORD data_size) 
{
	if (lock_fish_manager_ == NULL)
	{
		return true;
	}
  assert(data_size == sizeof(CMD_S_UserFire));
  if (data_size != sizeof(CMD_S_UserFire)) 
	  return false;
  CMD_S_UserFire* user_fire = static_cast<CMD_S_UserFire*>(data);

  float angle = user_fire->angle;
  int lock_fish_id = user_fire->lock_fishid;
  if (lock_fish_id == -1) 
  {
    lock_fish_id = lock_fish_manager_->GetLockFishID(user_fire->chair_id);
    if (lock_fish_id == 0) 
		lock_fish_id = fish_manager_->LockFish();
  }
  lock_fish_manager_->SetLockFishID(user_fire->chair_id, lock_fish_id);
  if (lock_fish_id > 0) 
  {
    Fish* fish = fish_manager_->GetFish(lock_fish_id);
    if (fish == NULL) 
	{
      lock_fish_manager_->SetLockFishID(user_fire->chair_id, 0);
      lock_fish_manager_->SetLockFishKind(user_fire->chair_id, FISH_KIND_COUNT);
    } 
	else 
	{
      lock_fish_manager_->SetLockFishKind(user_fire->chair_id, fish->fish_kind());
      if (user_fire->lock_fishid == -1 && lock_fish_id > 0) 
	  {
        FPoint mouse_pos = lock_fish_manager_->LockPos(user_fire->chair_id);
        if (!CanFire(user_fire->chair_id, mouse_pos)) 
		{
          lock_fish_id = 0;
          //lock_fish_manager_->SetLockFishID(user_fire->chair_id, 0);
          //lock_fish_manager_->SetLockFishKind(user_fire->chair_id, FISH_KIND_COUNT);
        } 
		else 
		{
          FPoint cannon_pos = cannon_manager_->GetCannonPos((CNewGameClient::GetMySelf())->SwitchChairID(user_fire->chair_id));
          angle = MathAide::CalcAngle(mouse_pos.x, mouse_pos.y, cannon_pos.x, cannon_pos.y);
        }
        // 机器人有锁定时调整炮的倍数
		if (lock_fish_id > 0 && user_fire->android_chairid != INVALID_CHAIR 
			&& user_fire->android_chairid == (CNewGameClient::GetMySelf())->m_iMyDeskStation) 
		{
          // 钱是否够
          if (cannon_manager_->GetFishScore(user_fire->chair_id) >= max_bullet_multiple_) 
		  {
            user_fire->fish_score = -max_bullet_multiple_;
            CMD_C_AndroidBulletMul android_bullet_mul;
            android_bullet_mul.chair_id = user_fire->chair_id;
            android_bullet_mul.bullet_id = user_fire->bullet_id;
            android_bullet_mul.bullet_mulriple = user_fire->bullet_mulriple = max_bullet_multiple_;
            if (max_bullet_multiple_ < 100) 
			{
              android_bullet_mul.bullet_kind = user_fire->bullet_kind = static_cast<BulletKind>((user_fire->bullet_kind / 4) * 4);
            } 
			else if (max_bullet_multiple_ >= 100 && max_bullet_multiple_ < 1000) 
			{
              android_bullet_mul.bullet_kind = user_fire->bullet_kind = static_cast<BulletKind>((user_fire->bullet_kind / 4) * 4 + 1);
            } 
			else if (max_bullet_multiple_ >= 1000 && max_bullet_multiple_ < 5000) 
			{
              android_bullet_mul.bullet_kind = user_fire->bullet_kind = static_cast<BulletKind>((user_fire->bullet_kind / 4) * 4 + 2);
            } 
			else 
			{
              android_bullet_mul.bullet_kind = user_fire->bullet_kind = static_cast<BulletKind>((user_fire->bullet_kind / 4) * 4 + 3);
            }
            SendSocketData(SUB_C_ANDROID_BULLET_MUL, &android_bullet_mul, sizeof(android_bullet_mul));
          }
        }
      }
    }
  }

  //if (user_fire->chair_id != (CNewGameClient::GetMySelf())->m_iMyDeskStation) sdp2014-05-08
  {
    cannon_manager_->SetCannonMulriple(user_fire->chair_id, user_fire->bullet_mulriple);
    cannon_manager_->SetFishScore(user_fire->chair_id, user_fire->fish_score);
    cannon_manager_->Fire(user_fire->chair_id, user_fire->bullet_kind);
    SoundManager::GetInstance().PlayGameEffect(FIRE);
  }

  cannon_manager_->SetCurrentAngle(user_fire->chair_id, angle);
  /*int iView = (CNewGameClient::GetMySelf())->SwitchChairID(user_fire->chair_id);
  float fAngle = 0.0f;
  if (iView <= 2)
  {
	  if (angle > M_PI_2 && angle <= M_PI + M_PI_2)
	  {
	  }
	  else
	  {
		  angle -= M_PI;
	  }
  }
  else
  {
	  if (angle > M_PI_2 && angle <= M_PI + M_PI_2)
	  {
		  angle +=  M_PI;
	  }
  }*/
  FPoint muzzle_pos;
  FPoint cannon_pos = cannon_manager_->GetCannonPos((CNewGameClient::GetMySelf())->SwitchChairID(user_fire->chair_id),user_fire->chair_id, &muzzle_pos);

  bullet_manager_->Fire(muzzle_pos.x, muzzle_pos.y, cannon_manager_->GetCurrentAngle(user_fire->chair_id), user_fire->bullet_kind,user_fire->bullet_id, user_fire->bullet_mulriple, user_fire->chair_id,
						bullet_speed_[user_fire->bullet_kind],net_radius_[user_fire->bullet_kind], user_fire->android_chairid, lock_fish_id);

  return true;
}

bool GameManager::OnSubCatchFish(void* data, WORD data_size) {
  assert(data_size == sizeof(CMD_S_CatchFish));
  if (data_size != sizeof(CMD_S_CatchFish)) return false;
  CMD_S_CatchFish* catch_fish = static_cast<CMD_S_CatchFish*>(data);

  fish_manager_->CatchFish(catch_fish->chair_id, catch_fish->fish_id, catch_fish->fish_score);
  jetton_manager_->AddJetton(catch_fish->chair_id, catch_fish->fish_score);
  cannon_manager_->SetFishScore(catch_fish->chair_id, catch_fish->fish_score);
  if ((catch_fish->fish_kind >= FISH_KIND_18 && catch_fish->fish_kind <= FISH_KIND_21) ||
      (catch_fish->fish_kind >= FISH_KIND_25 && catch_fish->fish_kind <= FISH_KIND_30)) {
    bingo_->SetBingoInfo(catch_fish->chair_id, catch_fish->fish_score);
  }

  if (catch_fish->bullet_ion) {
	  if (catch_fish->chair_id == (CNewGameClient::GetMySelf())->m_iMyDeskStation) {
      if (current_bullet_mulriple_ < 100) {
        current_bullet_kind_ = BULLET_KIND_1_ION;
      } else if (current_bullet_mulriple_ >= 100 && current_bullet_mulriple_ < 1000) {
        current_bullet_kind_ = BULLET_KIND_2_ION;
      } else if (current_bullet_mulriple_ >= 1000 && current_bullet_mulriple_ < 5000) {
        current_bullet_kind_ = BULLET_KIND_3_ION;
      } else {
        current_bullet_kind_ = BULLET_KIND_4_ION;
      }
      cannon_manager_->Switch(catch_fish->chair_id, current_bullet_kind_);
    } else {
      BulletKind bullet_kind = cannon_manager_->GetCurrentBulletKind(catch_fish->chair_id);
      if (bullet_kind < BULLET_KIND_1_ION) {
        bullet_kind = static_cast<BulletKind>(bullet_kind + BULLET_KIND_1_ION);
      }
      cannon_manager_->Switch(catch_fish->chair_id, bullet_kind);
    }
  }

  return true;
}

bool GameManager::OnSubBulletIonTimeout(void* data, WORD data_size) 
{
  assert(data_size == sizeof(CMD_S_BulletIonTimeout));
  if (data_size != sizeof(CMD_S_BulletIonTimeout)) return false;
  CMD_S_BulletIonTimeout* bullet_timeout = static_cast<CMD_S_BulletIonTimeout*>(data);
  if (bullet_timeout->chair_id == (CNewGameClient::GetMySelf())->m_iMyDeskStation) 
  {
    if (current_bullet_mulriple_ < 100) 
	{
      current_bullet_kind_ = BULLET_KIND_1_NORMAL;
    } 
	else if (current_bullet_mulriple_ >= 100 && current_bullet_mulriple_ < 1000) 
	{
      current_bullet_kind_ = BULLET_KIND_2_NORMAL;
    } else if (current_bullet_mulriple_ >= 1000 && current_bullet_mulriple_ < 5000) 
	{
      current_bullet_kind_ = BULLET_KIND_3_NORMAL;
    } 
	else 
	{
      current_bullet_kind_ = BULLET_KIND_4_NORMAL;
    }
    cannon_manager_->Switch(bullet_timeout->chair_id, current_bullet_kind_);
  } 
  else 
  {
    BulletKind bullet_kind = cannon_manager_->GetCurrentBulletKind(bullet_timeout->chair_id);
    cannon_manager_->Switch(bullet_timeout->chair_id, BulletKind(bullet_kind % 4));
  }

  return true;
}

bool GameManager::OnSubLockTimeout(void* data, WORD data_size) {
  lock_ = false;
  return true;
}

bool GameManager::OnSubCatSweepFish(void* data, WORD data_size) {
  assert(data_size == sizeof(CMD_S_CatchSweepFish));
  if (data_size != sizeof(CMD_S_CatchSweepFish)) return false;
  CMD_S_CatchSweepFish* catch_sweep_fish = static_cast<CMD_S_CatchSweepFish*>(data);
  if (catch_sweep_fish->chair_id != (CNewGameClient::GetMySelf())->m_iMyDeskStation) return true;

  fish_manager_->CatchSweepFish(catch_sweep_fish->chair_id, catch_sweep_fish->fish_id);

  return true;
}
bool GameManager::OnSubCatSweepFishResult(void* data, WORD data_size) {
  assert(data_size == sizeof(CMD_S_CatchSweepFishResult));
  if (data_size != sizeof(CMD_S_CatchSweepFishResult)) return false;
  CMD_S_CatchSweepFishResult* catch_sweep_result = static_cast<CMD_S_CatchSweepFishResult*>(data);

  fish_manager_->CatchSweepFishResult(catch_sweep_result->chair_id, catch_sweep_result->fish_id, catch_sweep_result->fish_score,
    catch_sweep_result->catch_fish_id, catch_sweep_result->catch_fish_count);
  jetton_manager_->AddJetton(catch_sweep_result->chair_id, catch_sweep_result->fish_score);
  cannon_manager_->SetFishScore(catch_sweep_result->chair_id, catch_sweep_result->fish_score);
  bingo_->SetBingoInfo(catch_sweep_result->chair_id, catch_sweep_result->fish_score);

  return true;
}

bool GameManager::OnSubHitFishLK(void* data, WORD data_size) 
{
	if (NULL == fish_manager_)
	{
		return true;
	}
  assert(data_size == sizeof(CMD_S_HitFishLK));
  if (data_size != sizeof(CMD_S_HitFishLK)) return false;
  CMD_S_HitFishLK* hit_fish = static_cast<CMD_S_HitFishLK*>(data);
  fish_manager_->HitFishLK(hit_fish->chair_id, hit_fish->fish_id, hit_fish->fish_mulriple);

  return true;
}

bool GameManager::OnSubSwitchScene(void* data, WORD data_size) 
{
	assert(data_size == sizeof(CMD_S_SwitchScene));
	if (data_size != sizeof(CMD_S_SwitchScene)) return false;
	CMD_S_SwitchScene* switch_scene = static_cast<CMD_S_SwitchScene*>(data);
	OutputDebugString("dwjlkpy1::不能发泡");
	AllowFire(false);
	game_scene_->SetSwitchSceneStyle();
	fish_manager_->SceneSwitchIterator();
	last_scene_kind_ = switch_scene->scene_kind;
	if (switch_scene->scene_kind == SCENE_KIND_1) 
	{
		assert(switch_scene->fish_count == arraysize(scene_kind_1_trace_));
		if (switch_scene->fish_count != arraysize(scene_kind_1_trace_)) 
			return false;
		for (int i = 0; i < switch_scene->fish_count; ++i) 
		{
			Fish* fish = NULL;
			fish = fish_manager_->ActiveFish(switch_scene->fish_kind[i], switch_scene->fish_id[i], fish_multiple_[switch_scene->fish_kind[i]],
				fish_speed_[switch_scene->fish_kind[i]], fish_bounding_box_width_[switch_scene->fish_kind[i]], fish_bounding_box_height_[switch_scene->fish_kind[i]],
				fish_hit_radius_[switch_scene->fish_kind[i]]);
			if(fish != NULL)
			{
				fish->set_trace_type(TRACE_LINEAR);
				std::copy(scene_kind_1_trace_[i].begin(), scene_kind_1_trace_[i].end(), std::back_inserter(fish->trace_vector()));
				//fish->set_active(true);
			}
		}
	} 
	else if (switch_scene->scene_kind == SCENE_KIND_2) 
	{
		assert(switch_scene->fish_count == arraysize(scene_kind_2_trace_));
		if (switch_scene->fish_count != arraysize(scene_kind_2_trace_)) 
			return false;
		for (int i = 0; i < switch_scene->fish_count; ++i) 
		{
			Fish* fish = NULL;
			fish = fish_manager_->ActiveFish(switch_scene->fish_kind[i], switch_scene->fish_id[i], fish_multiple_[switch_scene->fish_kind[i]],
				fish_speed_[switch_scene->fish_kind[i]], fish_bounding_box_width_[switch_scene->fish_kind[i]], fish_bounding_box_height_[switch_scene->fish_kind[i]],
				fish_hit_radius_[switch_scene->fish_kind[i]]);
			if(fish != NULL)
			{
				fish->set_trace_type(TRACE_LINEAR);
				std::copy(scene_kind_2_trace_[i].begin(), scene_kind_2_trace_[i].end(), std::back_inserter(fish->trace_vector()));
				if (i < 200) 
				{
					fish->SetFishStop(scene_kind_2_small_fish_stop_index_[i], scene_kind_2_small_fish_stop_count_);
				} else {
					fish->SetFishStop(scene_kind_2_big_fish_stop_index_, scene_kind_2_big_fish_stop_count_);
				}
				//fish->set_active(true);
			}
		}
	} 
	else if (switch_scene->scene_kind == SCENE_KIND_3) 
	{
		assert(switch_scene->fish_count == arraysize(scene_kind_3_trace_));
		if (switch_scene->fish_count != arraysize(scene_kind_3_trace_)) 
			return false;
		for (int i = 0; i < switch_scene->fish_count; ++i) 
		{
			Fish* fish = NULL;
			fish = fish_manager_->ActiveFish(switch_scene->fish_kind[i], switch_scene->fish_id[i], fish_multiple_[switch_scene->fish_kind[i]],
				fish_speed_[switch_scene->fish_kind[i]], fish_bounding_box_width_[switch_scene->fish_kind[i]], fish_bounding_box_height_[switch_scene->fish_kind[i]],
				fish_hit_radius_[switch_scene->fish_kind[i]]);
			if(fish != NULL)
			{
				fish->set_trace_type(TRACE_LINEAR);
				std::copy(scene_kind_3_trace_[i].begin(), scene_kind_3_trace_[i].end(), std::back_inserter(fish->trace_vector()));
				//fish->set_active(true);
			}
		}
	} 
	else if (switch_scene->scene_kind == SCENE_KIND_4) 
	{
		assert(switch_scene->fish_count == arraysize(scene_kind_4_trace_));
		if (switch_scene->fish_count != arraysize(scene_kind_4_trace_)) 
			return false;
		for (int i = 0; i < switch_scene->fish_count; ++i) 
		{
			Fish* fish = NULL;
			fish = fish_manager_->ActiveFish(switch_scene->fish_kind[i], switch_scene->fish_id[i], fish_multiple_[switch_scene->fish_kind[i]],
				fish_speed_[switch_scene->fish_kind[i]], fish_bounding_box_width_[switch_scene->fish_kind[i]], fish_bounding_box_height_[switch_scene->fish_kind[i]],
				fish_hit_radius_[switch_scene->fish_kind[i]]);
			if(fish != NULL)
			{
				fish->set_trace_type(TRACE_LINEAR);
				std::copy(scene_kind_4_trace_[i].begin(), scene_kind_4_trace_[i].end(), std::back_inserter(fish->trace_vector()));
				//fish->set_active(true);
			}
		}
	} 
	else if (switch_scene->scene_kind == SCENE_KIND_5) 
	{
		assert(switch_scene->fish_count == arraysize(scene_kind_5_trace_));
		if (switch_scene->fish_count != arraysize(scene_kind_5_trace_)) 
			return false;
		for (int i = 0; i < switch_scene->fish_count; ++i) 
		{
			Fish* fish = NULL;
			fish = fish_manager_->ActiveFish(switch_scene->fish_kind[i], switch_scene->fish_id[i], fish_multiple_[switch_scene->fish_kind[i]],
				fish_speed_[switch_scene->fish_kind[i]], fish_bounding_box_width_[switch_scene->fish_kind[i]], fish_bounding_box_height_[switch_scene->fish_kind[i]],
				fish_hit_radius_[switch_scene->fish_kind[i]]);
			if(fish != NULL)
			{
				fish->set_trace_type(TRACE_LINEAR);
				std::copy(scene_kind_5_trace_[i].begin(), scene_kind_5_trace_[i].end(), std::back_inserter(fish->trace_vector()));
				//fish->set_active(true);
			}
		}
	}
	return true;
}

bool GameManager::OnSubStockOperateResult(void* data, WORD data_size) {
  assert(data_size == sizeof(CMD_S_StockOperateResult));
  if (data_size != sizeof(CMD_S_StockOperateResult)) return false;
  CMD_S_StockOperateResult* stock_operate_result = static_cast<CMD_S_StockOperateResult*>(data);
  //if (!CUserRight::IsGameCheatUser(client_kernel_->GetUserAttribute().user_right)) return true;
  if (stock_operate_result->operate_code == 0) {
    _snwprintf(stock_message_, arraysize(stock_message_), L"当前库存:%I64d", stock_operate_result->stock_score);
  } else if (stock_operate_result->operate_code == 3) {
    _snwprintf(stock_message_, arraysize(stock_message_), L"当前抽水:%I64d", stock_operate_result->stock_score);
  } else {
    _snwprintf(stock_message_, arraysize(stock_message_), L"操作成功(当前库存:%I64d)", stock_operate_result->stock_score);
  }
  return true;
}

bool GameManager::OnSubSceneEnd(void* data, WORD data_size) {
  return true;
}

bool GameManager::CanFire(WORD chair_id, FPoint& mouse_pos) {
  FPoint cannon_pos = cannon_manager_->GetCannonPos(chair_id);
  if (chair_id == 0 || chair_id == 1 || chair_id == 2) 
  {
    if (mouse_pos.y < cannon_pos.y) return false;
  } 
  else if (chair_id == 6) 
  {
    if (mouse_pos.x > cannon_pos.x) return false;
  } 
  else if (chair_id == 7) 
  {
    if (mouse_pos.x < cannon_pos.x) return false;
  }
  else 
  {
    if (mouse_pos.y > cannon_pos.y) return false;
  }

  return true;
}

unsigned __stdcall GameManager::CalcTraceThread(void* param) 
{
	GameManager* game_manager = reinterpret_cast<GameManager*>(param);

	while (true) 
	{
		WaitForSingleObject(game_manager->calc_trace_event_, INFINITE);
		ResetEvent(game_manager->calc_trace_event_);

		if (game_manager->exit_game_) 
			break;

		EnterCriticalSection(&game_manager->critial_section_);
		std::copy(game_manager->fish_trace_info_buffer_.begin(), game_manager->fish_trace_info_buffer_.end(), 
			std::back_inserter(game_manager->fish_trace_info_calc_buffer_));
		game_manager->fish_trace_info_buffer_.clear();
		LeaveCriticalSection(&game_manager->critial_section_);

		while (game_manager->fish_trace_info_calc_buffer_.size() > 0) 
		{
			FishTraceInfo* fish_trace_info = &game_manager->fish_trace_info_calc_buffer_.back();
			Fish* fish = fish_trace_info->fish;
			if (fish == NULL) 
			{
				game_manager->fish_trace_info_calc_buffer_.pop_back();
				continue;
			}

			if (fish_trace_info->init_count < 2 || fish_trace_info->init_count > 5) 
			{
				game_manager->fish_trace_info_calc_buffer_.pop_back();
				continue;
			}

			if (game_manager->exit_game_) break;

			if (fish->trace_type() == TRACE_LINEAR) 
			{
				MathAide::BuildLinear(fish_trace_info->init_x_pos, fish_trace_info->init_y_pos, fish_trace_info->init_count, fish->trace_vector(), fish->fish_speed());
			} 
			else 
			{
				MathAide::BuildBezier(fish_trace_info->init_x_pos, fish_trace_info->init_y_pos, fish_trace_info->init_count, fish->trace_vector(), fish->fish_speed());
			}

			if (fish->trace_vector().size() < 2) 
				continue;

			if (game_manager->exit_game_) 
				break;
			if (game_manager->fish_trace_info_calc_buffer_.size() == 0) 
				break;

			game_manager->fish_trace_info_calc_buffer_.pop_back();
			fish->set_active(true);
		}
		if (game_manager->exit_game_) 
			break;
		//else ResetEvent(game_manager->calc_trace_event_);
	}
	return 0;
}

void GameManager::ShakeScreen() {
  offset_.x = 0.f;
  offset_.y = 0.f;
  since_last_frame_ = -1.f;
  current_shake_frame_ = 0;
  shake_screen_ = true;
}

void GameManager::UpdateShakeScreen(float delta_time) {
  if (!shake_screen_) return;

  if (since_last_frame_ == -1.0f) since_last_frame_ = 0.0f;
  else since_last_frame_ += delta_time;

  static const float kSpeed = 1.0f / kShakeFPS;
  while (since_last_frame_ >= kSpeed) {
    since_last_frame_ -= kSpeed;
    if (current_shake_frame_ + 1 == kShakeFrames) {
      shake_screen_ = false;
      offset_.x = 0.f;
      offset_.y = 0.f;
      break;
    } else {
      ++current_shake_frame_;
      offset_.x = rand() % 2 == 0 ? (10.f + hge_->Random_Float(0.f, 5.f)) : (-10.f + hge_->Random_Float(-5.f, 0.f));
      offset_.y = rand() % 2 == 1 ? (10.f + hge_->Random_Float(0.f, 5.f)) : (-10.f + hge_->Random_Float(-5.f, 0.f));
    }
  }
}

void GameManager::RenderUserScoreNum(SCORE num, float x, float y, float rot, float hscale, float vscale) {
  if (num < 0) return;

  static const int kMaxShowNumCount = 14;

  float num_width = spr_user_score_num_->GetWidth();
  float num_height = spr_user_score_num_->GetHeight();

  int num_count = 0;
  SCORE num_temp = num;
  do {
    num_count++;
    num_temp /= 10;
  } while (num_temp > 0);
  if (num_count > kMaxShowNumCount) {
    hscale *= (float)(kMaxShowNumCount) / (float)(num_count);
  }

  for (int i = 0; i < num_count; ++i) {
    spr_user_score_num_->SetFrame(static_cast<int>(num % 10));
    spr_user_score_num_->RenderEx(x, y, rot, hscale, vscale);
    x -= num_width * hscale;
    num /= 10;
  }
}
