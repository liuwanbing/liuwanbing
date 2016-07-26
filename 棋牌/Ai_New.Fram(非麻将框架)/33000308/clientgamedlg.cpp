#include "StdAfx.h"
#include "ClientGamedlg.h"

BEGIN_MESSAGE_MAP(CClientGameDlg, CLoveSendClass)
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// 定时器标识
#define IDI_SWITCH_SCENE		  100					// 切换场景不能打炮
#define IDI_EXCHANGE_FISHSCORE    101                 // 交换子弹
#define IDI_FIRE                  102
#define IDI_STAND_UP              103

#define IDI_FIRE_INTERVAL		  104					//开火间歇时间

// 时间标识
#define TIME_SWITCH_SCENE		  7				// 切换场景不能打炮 大概7秒钟 客户端有修改 这个应该也要修改
#define TIME_EXCHANGE_FISHSCORE   2
#define TIME_FIRE                 600			// 射速，毫秒级.(不要低于500，否则客户端会崩溃)
#define TIME_FIRE_INTERVAL		  60			//开火间歇时间长度

//构造函数
CClientGameDlg::CClientGameDlg() : CLoveSendClass(IDD_GAME_FRAME)
{
	OutputDebugString("sdp_ AI 构造函数");
	m_tag_fire = true;
	return;
} 

//析构函数
CClientGameDlg::~CClientGameDlg()
{

}

int CClientGameDlg::GetGameStatus()
{
	if (this)
	{
		return GetStationParameter();
	}
	else
	{
		return 0;
	}    

}

//初始化函数
BOOL CClientGameDlg::OnInitDialog()
{
	CLoveSendClass::OnInitDialog();
	return TRUE;
}

//重新设置界面
void CClientGameDlg::ResetGameFrame()
{
	return __super::ResetGameFrame();
}

//开始按钮函数
LRESULT	CClientGameDlg::OnHitBegin(WPARAM wparam, LPARAM lparam)
{
	OnControlHitBegin();
	return 0;
}

bool CClientGameDlg::OnControlHitBegin()
{
	return true;
}

//清除所有定时器
void CClientGameDlg::KillAllTimer()
{
	return;
}

//计时器
void CClientGameDlg::OnTimer(UINT nIDEvent)
{
	CLoveSendClass::OnTimer(nIDEvent);
}

//能否离开桌子,如果能,就在客户端收到消息后马上离开.
BOOL CClientGameDlg::CanLeftDesk()
{
	return TRUE;
}

//退出
void CClientGameDlg::OnCancel()
{
	KillAllTimer();
	AFCCloseFrame();
	__super::OnCancel();
}

//桌号换算到视图位置
BYTE CClientGameDlg::ViewStation(BYTE bDeskStation)
{
	return __super::ViewStation(bDeskStation);
}

BOOL CClientGameDlg::PreTranslateMessage(MSG* pMsg)
{
	return CLoveSendClass::PreTranslateMessage(pMsg);
}

//定时器消息
bool CClientGameDlg::OnGameTimer(BYTE bDeskStation, UINT uTimeID, UINT uTimeCount)
{
	CString sdp;
	sdp.Format("sdp_ AI 定时器消息 %d", uTimeID);
	OutputDebugString(sdp);

	switch (uTimeID)
	{
	case IDI_FIRE_INTERVAL:
		{
			KillGameTimer(IDI_FIRE_INTERVAL);
			m_tag_fire = !m_tag_fire;
			SetGameTimer(GetMeUserInfo()->bDeskStation, MyRand() % TIME_FIRE_INTERVAL + 20, IDI_FIRE_INTERVAL);
		}
	case IDI_SWITCH_SCENE:
		{
			allow_fire_ = true;
			return true;
		}
	case IDI_EXCHANGE_FISHSCORE:
		ExchangeFishScore();
		return true;
	case IDI_FIRE:
		Fire();
		return true;
	case IDI_STAND_UP:
		//SendGameData(NULL, 0, MDM_GM_GAME_NOTIFY, SUB_C_ANDROID_STAND_UP, 0);
		SendGameData(MDM_GM_GAME_FRAME,ASS_GM_FORCE_QUIT,0);
		return true;
	}

	return true;
}

bool CClientGameDlg::OnSubGameConfig(void* data, WORD data_size) 
{
	ASSERT(data_size == sizeof(CMD_S_GameConfig));
	if (data_size != sizeof(CMD_S_GameConfig)) 
		return false;
	CMD_S_GameConfig* game_config = static_cast<CMD_S_GameConfig*>(data);

	exchange_ratio_userscore_ = game_config->exchange_ratio_userscore;
	exchange_ratio_fishscore_ = game_config->exchange_ratio_fishscore;
	exchange_count_ = game_config->exchange_count;

	min_bullet_multiple_ = game_config->min_bullet_multiple;
	max_bullet_multiple_ = game_config->max_bullet_multiple;

	return true;
}

bool CClientGameDlg::OnSubExchangeFishScore(void* data, WORD data_size) 
{
	ASSERT(data_size == sizeof(CMD_S_ExchangeFishScore));
	if (data_size != sizeof(CMD_S_ExchangeFishScore)) 
		return false;
	CMD_S_ExchangeFishScore* exchange_fishscore = static_cast<CMD_S_ExchangeFishScore*>(data);

	if (exchange_fishscore->chair_id == GetMeUserInfo()->bDeskStation) 
	{
		fish_score_ = exchange_fishscore->swap_fish_score;

		CString sdp;
		sdp.Format("sdp_ 机器人钱 ------ 换算鱼币收到消息 %d",fish_score_);
		OutputDebugString(sdp);

		exchange_fish_score_ = exchange_fishscore->exchange_fish_score;
		--exchange_times_;
		if (exchange_times_ <= 0) 
		{
			SetGameTimerEx(GetMeUserInfo()->bDeskStation, TIME_FIRE, IDI_FIRE);
		} 
		else 
		{
			SetGameTimer(GetMeUserInfo()->bDeskStation, TIME_EXCHANGE_FISHSCORE, IDI_EXCHANGE_FISHSCORE);
		}
	}

	return true;
}

bool CClientGameDlg::OnSubUserFire(void* data, WORD data_size) 
{
	ASSERT(data_size == sizeof(CMD_S_UserFire));
	if (data_size != sizeof(CMD_S_UserFire)) 
		return false;
	CMD_S_UserFire* user_fire = static_cast<CMD_S_UserFire*>(data);

	if (user_fire->chair_id == GetMeUserInfo()->bDeskStation) 
	{
		fish_score_ += user_fire->fish_score;

		CString sdp;
		sdp.Format("sdp_ 机器人钱 ----- 子弹消耗 %d",fish_score_);
		OutputDebugString(sdp);
	}

	return true;
}

bool CClientGameDlg::OnSubCatchFish(void* data, WORD data_size) 
{
	ASSERT(data_size == sizeof(CMD_S_CatchFish));
	if (data_size != sizeof(CMD_S_CatchFish)) return false;
	CMD_S_CatchFish* catch_fish = static_cast<CMD_S_CatchFish*>(data);

	if (catch_fish->chair_id == GetMeUserInfo()->bDeskStation) 
	{
		fish_score_ += catch_fish->fish_score;

		CString sdp;
		sdp.Format("sdp_ 机器人钱 ------ 抓到鱼 %d",fish_score_);
		OutputDebugString(sdp);

		if (catch_fish->bullet_ion) 
		{
			if (current_bullet_mulriple_ < 100) 
			{
				current_bullet_kind_ = BULLET_KIND_1_ION;
			}
			else if (current_bullet_mulriple_ >= 100 && current_bullet_mulriple_ < 1000) 
			{
				current_bullet_kind_ = BULLET_KIND_2_ION;
			} 
			else if (current_bullet_mulriple_ >= 1000 && current_bullet_mulriple_ < 5000) 
			{
				current_bullet_kind_ = BULLET_KIND_3_ION;
			} 
			else 
			{
				current_bullet_kind_ = BULLET_KIND_4_ION;
			}
		}
	}

	return true;
}

bool CClientGameDlg::OnSubBulletIonTimeout(void* data, WORD data_size) 
{
	ASSERT(data_size == sizeof(CMD_S_BulletIonTimeout));
	if (data_size != sizeof(CMD_S_BulletIonTimeout)) 
		return false;
	CMD_S_BulletIonTimeout* bullet_timeout = static_cast<CMD_S_BulletIonTimeout*>(data);
	if (bullet_timeout->chair_id == GetMeUserInfo()->bDeskStation) 
	{
		if (current_bullet_mulriple_ < 100) 
		{
			current_bullet_kind_ = BULLET_KIND_1_NORMAL;
		} 
		else if (current_bullet_mulriple_ >= 100 && current_bullet_mulriple_ < 1000) 
		{
			current_bullet_kind_ = BULLET_KIND_2_NORMAL;
		} 
		else if (current_bullet_mulriple_ >= 1000 && current_bullet_mulriple_ < 5000) 
		{
			current_bullet_kind_ = BULLET_KIND_3_NORMAL;
		} 
		else 
		{
			current_bullet_kind_ = BULLET_KIND_4_NORMAL;
		}
	}

	return true;
}

bool CClientGameDlg::OnSubCatSweepFishResult(void* data, WORD data_size) 
{
	ASSERT(data_size == sizeof(CMD_S_CatchSweepFishResult));
	if (data_size != sizeof(CMD_S_CatchSweepFishResult)) 
		return false;
	CMD_S_CatchSweepFishResult* catch_sweep_result = static_cast<CMD_S_CatchSweepFishResult*>(data);

	if (catch_sweep_result->chair_id == GetMeUserInfo()->bDeskStation) 
	{
		fish_score_ += catch_sweep_result->fish_score;

		CString sdp;
		sdp.Format("sdp_ 机器人钱 ------ 抓到大鱼 %d",fish_score_);
		OutputDebugString(sdp);
	}

	return true;
}

bool CClientGameDlg::OnSubSwitchScene(void* data, WORD data_size) 
{
	ASSERT(data_size == sizeof(CMD_S_SwitchScene));
	if (data_size != sizeof(CMD_S_SwitchScene)) 
		return false;
	CMD_S_SwitchScene* switch_scene = static_cast<CMD_S_SwitchScene*>(data);

	allow_fire_ = false;
	SetGameTimer(GetMeUserInfo()->bDeskStation, TIME_SWITCH_SCENE, IDI_SWITCH_SCENE);

	return true;
}

void CClientGameDlg::ExchangeFishScore() 
{
	CMD_C_ExchangeFishScore exchange_fishscore;
	exchange_fishscore.increase = true;

	SendGameData(&exchange_fishscore, sizeof(exchange_fishscore), MDM_GM_GAME_NOTIFY, SUB_C_EXCHANGE_FISHSCORE, 0);
}

void CClientGameDlg::Fire() 
{

	int need_fish_score = current_bullet_mulriple_;
	if (fish_score_ < need_fish_score) 
	{
		SCORE android_score = GetMeUserInfo()->i64Money;
		SCORE need_user_score = exchange_ratio_userscore_ * exchange_count_ / exchange_ratio_fishscore_;
		SCORE user_leave_score = android_score - exchange_fish_score_ * exchange_ratio_userscore_ / exchange_ratio_fishscore_;
		int times = MyRand() % 5 + 5;
		exchange_times_ = max(1, min(times, (int)(user_leave_score / need_user_score)));
		ExchangeFishScore();
	} 
	else 
	{
		if (allow_fire_ && m_tag_fire) 
		{
			CMD_C_UserFire user_fire;
			user_fire.bullet_kind = current_bullet_kind_;
			user_fire.angle = GetAngle(GetMeUserInfo()->bDeskStation);
			user_fire.bullet_mulriple = current_bullet_mulriple_;
			user_fire.lock_fishid = (MyRand() % 100) > 70 ? -1 : 0;

			SendGameData(&user_fire, sizeof(user_fire), MDM_GM_GAME_NOTIFY, SUB_C_USER_FIRE, 0);
		}

		SetGameTimerEx(GetMeUserInfo()->bDeskStation, TIME_FIRE, IDI_FIRE);
	}
}

//计时器
bool CClientGameDlg::SetGameTimerEx(BYTE bDeskStation,UINT uTimeCount,UINT uTimeID)
{
	if(m_pGameRoom != NULL)
		m_pGameRoom->SendMessage(IDM_SET_GAME_TIME,(WPARAM)uTimeID,(LPARAM)uTimeCount);
	//	OnGameTimer(bDeskStation,uTimeID,uTimeCount);
	return true;
}

float CClientGameDlg::GetAngle(WORD chair_id) 
{
	//chair_id 0, 1: M_PI_2, M_PI_2 + M_PI;
	//chair_id 3: M_PI, M_PI * 2;
	//chair_id 5, 4: M_PI_2 + M_PI, M_PI_2 + M_PI + M_PI;
	//chair_id 7: M_PI * 2 , M_PI * 2 + M_PI_2, M_PI_2, M_PI;
	static const float kFireAngle[] = { 1.87f, 2.17f, 2.47f, 2.77f, 3.07f, 3.14f, 3.21f, 3.51f, 3.81f, 4.11f, 4.41f, 4.61f };
	static const float kFireAngleR[] = { 5.98f, 5.68f, 5.38f, 5.08f, 4.91f, 4.71f, 4.61f, 4.51f, 4.34f, 4.04f, 3.74f, 3.44f };
	static const float kFireAngleL[] = { 6.58f, 6.88f, 7.18f, 7.40f, 7.60f, 7.80f, 1.57f, 1.77f, 2.08f, 2.38f, 2.48f, 2.78f };
	float angle;
	int idx = 0;
	if (chair_id == 3) 
	{
		for (int i = 0; i < CountArray(kFireAngleR); ++i) 
		{
			if (last_fire_angle_ == kFireAngleR[i]) 
			{
				idx = i;
				break;
			}
		}

		int n = MyRand() % 5 - 2;
		if (idx - n < 0) 
		{
			idx = MyRand() % 5;
		} 
		else if (idx + n >= CountArray(kFireAngleR)) 
		{
			idx = CountArray(kFireAngleR) - MyRand() % 5 - 1;
		} 
		else 
		{
			idx = idx + n;
		}

		last_fire_angle_ = angle = kFireAngleR[idx];
	} 
	else if (chair_id == 7) 
	{
		for (int i = 0; i < CountArray(kFireAngleL); ++i) 
		{
			if (last_fire_angle_ == kFireAngleL[i]) 
			{
				idx = i;
				break;
			}
		}

		int n = MyRand() % 5 - 2;
		if (idx - n < 0) 
		{
			idx = MyRand() % 5;
		} 
		else if (idx + n >= CountArray(kFireAngleL)) 
		{
			idx = CountArray(kFireAngleL) - MyRand() % 5 - 1;
		} 
		else 
		{
			idx = idx + n;
		}

		last_fire_angle_ = angle = kFireAngleL[idx];
	} 
	else 
	{
		for (int i = 0; i < CountArray(kFireAngle); ++i) 
		{
			if (last_fire_angle_ == kFireAngle[i]) 
			{
				idx = i;
				break;
			}
		}

		int n = MyRand() % 5 - 2;
		if (idx - n < 0) {
			idx = MyRand() % 5;
		} 
		else if (idx + n >= CountArray(kFireAngle)) 
		{
			idx = CountArray(kFireAngle) - MyRand() % 5 - 1;
		} 
		else 
		{
			idx = idx + n;
		}

		angle = kFireAngle[idx];
		if (chair_id == 4 || chair_id == 5 || chair_id == 6) 
		{
			angle += M_PI;
		}

		last_fire_angle_ = angle;
	}

	return angle;
}

//游戏消息处理函数
bool CClientGameDlg::HandleGameMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	if (pNetHead->bMainID == MDM_GM_GAME_NOTIFY)
	{
		CString sdp;
		sdp.Format("sdp_ AI 游戏消息 %d", pNetHead->bAssistantID);
		OutputDebugString(sdp);

		OnEventGameMessage(pNetHead->bAssistantID, pNetData, uDataSize);
	}

	return __super::HandleGameMessage(pNetHead,pNetData,uDataSize,pClientSocket);
}

//游戏消息
bool CClientGameDlg::OnEventGameMessage(WORD sub_cmdid, void * data, WORD data_size)
{
	switch (sub_cmdid) 
	{
	case SUB_S_EXCHANGE_FISHSCORE:
		return OnSubExchangeFishScore(data, data_size);
	case SUB_S_USER_FIRE:
		return OnSubUserFire(data, data_size);
	case SUB_S_CATCH_FISH:
		return OnSubCatchFish(data, data_size);
	case SUB_S_BULLET_ION_TIMEOUT:
		return OnSubBulletIonTimeout(data, data_size);
	case SUB_S_CATCH_SWEEP_FISH_RESULT:
		return OnSubCatSweepFishResult(data, data_size);
	case SUB_S_SWITCH_SCENE:
		return OnSubSwitchScene(data, data_size);
	}

	return true;
}

//设置游戏状态
bool CClientGameDlg::SetGameStation(void * pStationData, UINT uDataSize)
{
	GameStation * pCmd=(GameStation *)pStationData;
	SetStationParameter(pCmd->game_status);

	//配置在此处获得
	OnSubGameConfig(&pCmd->GameConfig, sizeof(CMD_S_GameConfig));

	OutputDebugString("sdp_ AI 设置游戏状态");

	return OnEventSceneMessage(GetStationParameter(), false, &pCmd->GameStatus, sizeof(CMD_S_GameStatus));
}

//场景消息
bool CClientGameDlg::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, void * data, WORD data_size)
{
	switch (cbGameStatus) 
	{
	case GAME_STATUS_FREE:
	case GAME_STATUS_PLAY:
		{
			ASSERT(data_size == sizeof(CMD_S_GameStatus));
			if (data_size != sizeof(CMD_S_GameStatus)) 
				return false;
			CMD_S_GameStatus* gamestatus = static_cast<CMD_S_GameStatus*>(data);
			if (gamestatus->game_version != GAME_VERSION) 
				return false;

			fish_score_ = 0;
			exchange_fish_score_ = 0;

			srand((DWORD)time(NULL));
			if (max_bullet_multiple_ >= 100 && max_bullet_multiple_ <= 1000) 
			{
				int n = max_bullet_multiple_ / 100;
				int nRand = MyRand() % (n + 1);
				if (nRand == 0) 
				{
					current_bullet_mulriple_ = min_bullet_multiple_;
				} 
				else 
				{
					current_bullet_mulriple_ = nRand * 100;
					if (current_bullet_mulriple_ > max_bullet_multiple_) 
						current_bullet_mulriple_ = max_bullet_multiple_;
				}
			} 
			else if (max_bullet_multiple_ > 1000) 
			{
				int n = max_bullet_multiple_ / 1000;
				int nRand = MyRand() % (n + 2);
				if (nRand == 0) 
				{
					current_bullet_mulriple_ = min_bullet_multiple_;
				} 
				else 
				{
					current_bullet_mulriple_ = nRand * 1000;
					if (current_bullet_mulriple_ > max_bullet_multiple_) 
						current_bullet_mulriple_ = max_bullet_multiple_;
				}
			} 
			else 
			{
				current_bullet_mulriple_ = min_bullet_multiple_;
			}
			//current_bullet_mulriple_ = min_bullet_multiple_;

			if (current_bullet_mulriple_ < 100) 
			{
				current_bullet_kind_ = BULLET_KIND_1_NORMAL;
			} 
			else if (current_bullet_mulriple_ >= 100 && current_bullet_mulriple_ < 1000) 
			{
				current_bullet_kind_ = BULLET_KIND_2_NORMAL;
			} 
			else if (current_bullet_mulriple_ >= 1000 && current_bullet_mulriple_ < 5000) 
			{
				current_bullet_kind_ = BULLET_KIND_3_NORMAL;
			} 
			else 
			{
				current_bullet_kind_ = BULLET_KIND_4_NORMAL;
			}

			allow_fire_ = true;

			SCORE android_score = GetMeUserInfo()->i64Money;
			SCORE need_user_score = exchange_ratio_userscore_ * exchange_count_ / exchange_ratio_fishscore_;
			SCORE user_leave_score = android_score - exchange_fish_score_ * exchange_ratio_userscore_ / exchange_ratio_fishscore_;
			int times = MyRand() % 5 + 5;
			exchange_times_ = max(1, min(times, (int)(user_leave_score / need_user_score)));

			DWORD play_time = 10 * 60 + (MyRand() % 21) * 60;
			//play_time = 30;

			SetGameTimer(GetMeUserInfo()->bDeskStation, play_time, IDI_STAND_UP);
			SetGameTimer(GetMeUserInfo()->bDeskStation, TIME_EXCHANGE_FISHSCORE, IDI_EXCHANGE_FISHSCORE);

			SetGameTimer(GetMeUserInfo()->bDeskStation, MyRand() % TIME_FIRE_INTERVAL + 20, IDI_FIRE_INTERVAL);
			return true;
		}

	}
	return false;
}

/*************************************************
*Function: 生成随机数
*explain:该函数利用一个固定的值来获取随机值，有效避免大量随机运算时出现规律性数据
*writer:
*		帅东坡
*Parameters:
*		void
*Return:
*		返回随机数
*************************************************/
int CClientGameDlg::MyRand()
{
	static int innerSeed = GetTickCount();

	return (((innerSeed = innerSeed * 214013L + 2531011L) >> 16) & 0x7fff );
}