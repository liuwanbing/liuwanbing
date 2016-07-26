#include "StdAfx.h"
#include "ServerManage.h"
#include "tinyxml\tinyxml.h"
#include "tinyxml\tinystr.h"

void DebugPrintf( const char *p, ...)
{
	CTime time = CTime::GetCurrentTime();
	char szFilename[256];
	sprintf(szFilename, "%d-%d%d%d-log.txt", NAME_ID,time.GetYear(), time.GetMonth(), time.GetDay());
	FILE *fp = fopen(szFilename, "a");
	if (NULL == fp)
	{
		return;
	}
	va_list arg;
	va_start(arg, p);
	vfprintf(fp, p, arg);
	fprintf(fp,"\n");

	fclose(fp);
}

bool CheckFish20Config(DWORD game_id, int* catch_count, double* catch_probability) {
	std::vector<Fish20Config>::iterator iter;
	for (iter = g_fish20_config_.begin(); iter != g_fish20_config_.end(); ++iter) {
		Fish20Config& config = *iter;
		if (game_id == config.game_id) {
			if (config.catch_count <= 0) {
				g_fish20_config_.erase(iter);
				return false;
			}
			*catch_count = config.catch_count;
			*catch_probability = config.catch_probability;
			return true;
		}
	}
	return false;
}

// 返回值 0：黑名单  1 白名单 -1 正常
int CheckUserFilter(DWORD game_id) 
{
	std::vector<DWORD>::iterator iter;
	for (iter = g_balck_list_.begin(); iter != g_balck_list_.end(); ++iter) 
	{
		if ((*iter) == game_id) return 0;
	}

	for (iter = g_white_list_.begin(); iter != g_white_list_.end(); ++iter) 
	{
		if ((*iter) == game_id) return 1;
	}

	return -1;
}

void AddFish20Config(DWORD game_id, int catch_count, double catch_probability) {
	std::vector<Fish20Config>::iterator iter;
	for (iter = g_fish20_config_.begin(); iter != g_fish20_config_.end(); ++iter) {
		Fish20Config& config = *iter;
		if (game_id == config.game_id) {
			if (catch_count == 0) {
				g_fish20_config_.erase(iter);
			} else {
				config.catch_count = catch_count;
				config.catch_probability = catch_probability;
			}
			return;
		}
	}

	Fish20Config config;
	config.game_id = game_id;
	config.catch_count = catch_count;
	config.catch_probability = catch_probability;
	g_fish20_config_.push_back(config);
}

void AddUserFilter(DWORD game_id, unsigned char operate_code) {
	std::vector<DWORD>::iterator iter;
	if (operate_code == 0) {
		for (iter = g_balck_list_.begin(); iter != g_balck_list_.end(); ++iter) {
			if ((*iter) == game_id) return;
		}
		g_balck_list_.push_back(game_id);
	} else if (operate_code == 1) {
		for (iter = g_white_list_.begin(); iter != g_white_list_.end(); ++iter) {
			if ((*iter) == game_id) return;
		}
		g_white_list_.push_back(game_id);
	} else {
		for (iter = g_balck_list_.begin(); iter != g_balck_list_.end(); ++iter) {
			if ((*iter) == game_id) {
				iter = g_balck_list_.erase(iter);
				break;
			}
		}
		for (iter = g_white_list_.begin(); iter != g_white_list_.end(); ++iter) {
			if ((*iter) == game_id) {
				iter = g_white_list_.erase(iter);
				break;
			}
		}
	}
}

///拦截框架消息
bool CServerGameDesk::HandleFrameMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser)
{
	//switch(pNetHead->bAssistantID)
	//{
	//case ASS_GM_FORCE_QUIT:		//强行退出//安全退出
	//	{
	//		//UserFourceQuit(bDeskStation);
	//		return true;
	//	}
	//}
	return __super::HandleFrameMessage( bDeskStation,  pNetHead,  pData,  uSize,  uSocketID,  bWatchUser);
}

//游戏数据包处理函数
bool CServerGameDesk::HandleNotifyMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser)
{
	if (bWatchUser)
	{
		return false;
	}
	switch (pNetHead->bAssistantID)
	{
	case SUB_C_EXCHANGE_FISHSCORE: 
		{
		assert(uSize == sizeof(CMD_C_ExchangeFishScore));
		if (uSize != sizeof(CMD_C_ExchangeFishScore))
			return true;
		CMD_C_ExchangeFishScore* exchange_fishscore = static_cast<CMD_C_ExchangeFishScore*>(pData);
		
		return OnSubExchangeFishScore(bDeskStation, exchange_fishscore->increase);
		}
	case SUB_C_USER_FIRE: 
		{

			assert(uSize == sizeof(CMD_C_UserFire));
			if (uSize != sizeof(CMD_C_UserFire)) 
			{
				return true;
			}
			CMD_C_UserFire* user_fire = static_cast<CMD_C_UserFire*>(pData);

			return OnSubUserFire(bDeskStation, user_fire->bullet_kind, user_fire->angle, user_fire->bullet_mulriple, user_fire->lock_fishid);
		}
	case SUB_C_CATCH_FISH: 
		{

		assert(uSize == sizeof(CMD_C_CatchFish));
		if (uSize != sizeof(CMD_C_CatchFish)) 
			return true;
		CMD_C_CatchFish* hit_fish = static_cast<CMD_C_CatchFish*>(pData);

		//if (bWatchUser) 
		//	return true;
		//通过真人发来的消息帮助机器人打到鱼
		if (hit_fish->chair_id < 0 || hit_fish->chair_id >= PLAY_COUNT)
		{
			return true;
		}
		if (m_pUserInfo[hit_fish->chair_id] != NULL)
		{
			if (m_pUserInfo[hit_fish->chair_id]->m_UserData.isVirtual)
			{
				bDeskStation = hit_fish->chair_id;
			}
		}
		else
		{
			return true;
		}
		OutputDebugString("dwjlkpy::集中了鱼儿");
		return OnSubCatchFish(bDeskStation, hit_fish->fish_id, hit_fish->bullet_kind, hit_fish->bullet_id, hit_fish->bullet_mulriple);
		}
	case SUB_C_CATCH_SWEEP_FISH: 
		{
		assert(uSize == sizeof(CMD_C_CatchSweepFish));
		if (uSize != sizeof(CMD_C_CatchSweepFish)) 
			return true;
		CMD_C_CatchSweepFish* catch_sweep = static_cast<CMD_C_CatchSweepFish*>(pData);
	
		return OnSubCatchSweepFish(bDeskStation, catch_sweep->fish_id, catch_sweep->catch_fish_id, catch_sweep->catch_fish_count);
		}
	case SUB_C_HIT_FISH_I: 
		{
		assert(uSize == sizeof(CMD_C_HitFishLK));
		if (uSize != sizeof(CMD_C_HitFishLK)) 
			return true;
		CMD_C_HitFishLK* hit_fish = static_cast<CMD_C_HitFishLK*>(pData);
		//if (bWatchUser) 
		//	return true;
		return OnSubHitFishLK(bDeskStation, hit_fish->fish_id);
		}
	case SUB_C_STOCK_OPERATE: 
		{
		assert(uSize == sizeof(CMD_C_StockOperate));
		if (uSize != sizeof(CMD_C_StockOperate)) 
			return true;
		CMD_C_StockOperate* stock_operate = static_cast<CMD_C_StockOperate*>(pData);
		
		return OnSubStockOperate(bDeskStation, stock_operate->operate_code);
		}
	case SUB_C_USER_FILTER: 
		{
		assert(uSize == sizeof(CMD_C_UserFilter));
		if (uSize != sizeof(CMD_C_UserFilter)) 
			return true;
		CMD_C_UserFilter* user_filter = static_cast<CMD_C_UserFilter*>(pData);
	
		return OnSubUserFilter(bDeskStation, user_filter->game_id, user_filter->operate_code);
		}
	case SUB_C_ANDROID_STAND_UP: 
		{
		if (!m_pUserInfo[bDeskStation]->m_UserData.isVirtual) return true;
		//强退
		//table_frame_->PerformStandUpAction(server_user_item);
		return true;
		}
	case SUB_C_FISH20_CONFIG:
		{
		assert(uSize == sizeof(CMD_C_Fish20Config));
		if (uSize != sizeof(CMD_C_Fish20Config)) return true;
		CMD_C_Fish20Config* fish20_config = static_cast<CMD_C_Fish20Config*>(pData);
		return OnSubFish20Config(bDeskStation, fish20_config->game_id, fish20_config->catch_count, fish20_config->catch_probability);
		}
	case SUB_C_ANDROID_BULLET_MUL: 
		{
		assert(uSize == sizeof(CMD_C_AndroidBulletMul));
		if (uSize != sizeof(CMD_C_AndroidBulletMul)) return true;
		CMD_C_AndroidBulletMul* android_bullet_mul = static_cast<CMD_C_AndroidBulletMul*>(pData);
		if (!m_pUserInfo[bDeskStation]->m_UserData.isVirtual) 
			return true;
		ServerBulletInfo* bullet_info = GetBulletInfo(android_bullet_mul->chair_id, android_bullet_mul->bullet_id);
		if (bullet_info == NULL)
			return true;
		if (fish_score_[android_bullet_mul->chair_id]/* + bullet_info->bullet_mulriple*/ < android_bullet_mul->bullet_mulriple) 
		{
			//退出
			//table_frame_->PerformStandUpAction(user_item);
			return true;
		}

		CMD_S_UserFire user_fire;
		user_fire.bullet_kind = bullet_info->bullet_kind;
		user_fire.bullet_id = bullet_info->bullet_id;
		user_fire.angle = 0.f;
		user_fire.chair_id = bDeskStation;
		user_fire.android_chairid = android_chairid_;
		user_fire.bullet_mulriple = bullet_info->bullet_mulriple;
		user_fire.fish_score = bullet_info->bullet_mulriple - android_bullet_mul->bullet_mulriple;
		user_fire.lock_fishid = 0;
		//SendDataToPlayer(bDeskStation, &user_fire, sizeof(CMD_S_UserFire), SUB_S_USER_FIRE);

		SendDataToAllPlayers(&user_fire, sizeof(CMD_S_UserFire), SUB_S_USER_FIRE);
		SendWatchData(m_bMaxPeople,&user_fire, sizeof(CMD_S_UserFire),MDM_GM_GAME_NOTIFY,SUB_S_USER_FIRE,0);

		//fish_score_[android_bullet_mul->chair_id] += bullet_info->bullet_mulriple;
		fish_score_[android_bullet_mul->chair_id] -= android_bullet_mul->bullet_mulriple;

		bullet_info->bullet_kind = android_bullet_mul->bullet_kind;
		bullet_info->bullet_mulriple = android_bullet_mul->bullet_mulriple;

		return true;
		}
	}
	return __super::HandleNotifyMessage(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);
}

bool CServerGameDesk::OnSubFish20Config(BYTE bDeskStation, DWORD game_id, int catch_count, double catch_probability) 
{
	/*if (!CUserRight::IsGameCheatUser(server_user_item->GetUserRight())) return false;*/

	AddFish20Config(game_id, catch_count, catch_probability);

	return true;
}

bool CServerGameDesk::OnSubUserFilter(BYTE bDeskStation, DWORD game_id, unsigned char operate_code) 
{
	//if (!CUserRight::IsGameCheatUser(server_user_item->GetUserRight())) return false;

	AddUserFilter(game_id, operate_code);

	return true;
}

bool CServerGameDesk::OnSubHitFishLK(BYTE bDeskStation, int fish_id) {
	FishTraceInfo* fish_trace_info = GetFishTraceInfo(fish_id);
	if (fish_trace_info == NULL) return true;
	if (fish_trace_info->fish_kind != FISH_KIND_21) return true;
	if (current_fish_lk_multiple_ >= fishLK_max_multiple_) return true;

	++current_fish_lk_multiple_;
	CMD_S_HitFishLK hit_fish;
	hit_fish.chair_id = bDeskStation;
	hit_fish.fish_id = fish_id;
	hit_fish.fish_mulriple = current_fish_lk_multiple_;

	SendDataToAllPlayers(&hit_fish, sizeof(hit_fish),SUB_S_HIT_FISH_LK);
	SendWatchData(m_bMaxPeople,&hit_fish, sizeof(hit_fish),MDM_GM_GAME_NOTIFY,SUB_S_HIT_FISH_LK,0);

	return true;
}

//超端请求
bool CServerGameDesk::OnSubStockOperate(BYTE bDeskStation, unsigned char operate_code) 
{
	if (!IsSuperMan(bDeskStation)) 
		return true;

	CMD_S_StockOperateResult stock_operate_result;
	stock_operate_result.operate_code = operate_code;
	if (operate_code == 0) 
	{
		stock_operate_result.stock_score = g_stock_score_;
	} 
	else if (operate_code == 1) 
	{
		stock_operate_result.stock_score = g_stock_score_ = 0;
	} 
	else if (operate_code == 2) 
	{
		int iscore = GetSuperCellscore(bDeskStation);
		g_stock_score_ += (SCORE)iscore;//单位积分

		CString sdp;
		sdp.Format("sdp_ 超端 抓鱼概率 (Cellscore=%d,g=%I64d)",GetSuperCellscore(bDeskStation),g_stock_score_);
		OutputDebugString(sdp);

		stock_operate_result.stock_score = g_stock_score_;
	} 
	else if (operate_code == 3) 
	{
		stock_operate_result.stock_score = g_revenue_score;
	}

	SendDataToPlayer(bDeskStation,&stock_operate_result, sizeof(CMD_S_StockOperateResult), SUB_S_STOCK_OPERATE_RESULT);
	return true;
}

bool CServerGameDesk::OnSubCatchSweepFish(BYTE bDeskStation, int fish_id, int* catch_fish_id, int catch_fish_count) 
{
	SweepFishInfo* sweep_fish_info = GetSweepFish(fish_id);
	if (sweep_fish_info == NULL) 
		return true;
	assert(sweep_fish_info->fish_kind == FISH_KIND_23 || sweep_fish_info->fish_kind == FISH_KIND_24 || (sweep_fish_info->fish_kind >= FISH_KIND_31 && sweep_fish_info->fish_kind <= FISH_KIND_40));
	if (!(sweep_fish_info->fish_kind == FISH_KIND_23 || sweep_fish_info->fish_kind == FISH_KIND_24 || (sweep_fish_info->fish_kind >= FISH_KIND_31 && sweep_fish_info->fish_kind <= FISH_KIND_40))) 
		return true;

	WORD chair_id = bDeskStation;

	SCORE fish_score = fish_multiple_[sweep_fish_info->fish_kind] * sweep_fish_info->bullet_mulriple;
	if (fish_score <= 0)//没分就判定打不死
		return true;
	FishTraceInfoVecor::iterator iter;
	FishTraceInfo* fish_trace_info = NULL;
	for (int i = 0; i < catch_fish_count; ++i) 
	{
		for (iter = active_fish_trace_vector_.begin(); iter != active_fish_trace_vector_.end(); ++iter) 
		{
			fish_trace_info = *iter;
			if (fish_trace_info->fish_id == catch_fish_id[i]) 
			{
				fish_score += fish_multiple_[fish_trace_info->fish_kind] * sweep_fish_info->bullet_mulriple;
				active_fish_trace_vector_.erase(iter);
				storage_fish_trace_vector_.push_back(fish_trace_info);
				break;
			}
		}
	}
	if (sweep_fish_info->bullet_kind >= BULLET_KIND_1_ION) fish_score *= 2;
	fish_score_[chair_id] += fish_score;

	if (!m_pUserInfo[bDeskStation]->m_UserData.isVirtual)
		g_stock_score_ -= fish_score;

	CMD_S_CatchSweepFishResult catch_sweep_result;
	memset(&catch_sweep_result, 0, sizeof(catch_sweep_result));
	catch_sweep_result.fish_id = fish_id;
	catch_sweep_result.chair_id = chair_id;
	catch_sweep_result.fish_score = fish_score;
	catch_sweep_result.catch_fish_count = catch_fish_count;
	memcpy(catch_sweep_result.catch_fish_id, catch_fish_id, catch_fish_count * sizeof(int));

	SendDataToAllPlayers(&catch_sweep_result, sizeof(catch_sweep_result),SUB_S_CATCH_SWEEP_FISH_RESULT);
	SendWatchData(m_bMaxPeople,&catch_sweep_result, sizeof(catch_sweep_result),MDM_GM_GAME_NOTIFY,SUB_S_CATCH_SWEEP_FISH_RESULT,0);

	//发送打到大鱼的系统消息
	CMD_S_CatchSweepFishResultEx cmd;
	memset(&cmd, 0, sizeof(cmd));
	if (m_pUserInfo[chair_id] != NULL)
	{
		CopyMemory(cmd.szNickName, m_pUserInfo[chair_id]->m_UserData.nickName, sizeof(cmd.szNickName));//昵称
		CopyMemory(cmd.cRoomName, m_pDataManage->m_InitData.szGameRoomName, sizeof(cmd.szNickName));//房间名字
		CopyMemory(cmd.cFishName, m_cFishName[sweep_fish_info->fish_kind], sizeof(cmd.cFishName));//鱼名字
		
		cmd.exchange_ratio_userscore_ = exchange_ratio_userscore_;	//换算比例中用户金币
		cmd.exchange_ratio_fishscore_ = exchange_ratio_fishscore_;	//换算比例中鱼币
		cmd.iDeskID = m_pUserInfo[chair_id]->m_UserData.bDeskNO;			//桌子ID
		cmd.fish_kind = sweep_fish_info->fish_kind;				//鱼类型ID
		cmd.fish_score = fish_score;		//打中鱼赚多少钱

		//发送广播消息，通知所有桌子，某某玩家打中Boss鱼
		m_pDataManage->m_TCPSocket.SendDataBatch(&cmd,sizeof(cmd),MDM_GM_GAME_NOTIFY,SUB_S_CATCH_SWEEP_FISH_RESULT_EX,0);

	}


	FreeSweepFish(fish_id);

	return true;
}

bool CServerGameDesk::OnSubCatchFish(BYTE bDeskStation, int fish_id, BulletKind bullet_kind, int bullet_id, int bullet_mul) 
{
	CString sdp;
	if (bullet_mul < min_bullet_multiple_ || bullet_mul > max_bullet_multiple_) 
	{
		return true;
	}

	FishTraceInfo* fish_trace_info = GetFishTraceInfo(fish_id);
	if (fish_trace_info == NULL) 
	{
		return true;
	}

	if (fish_trace_info->fish_kind >= FISH_KIND_COUNT) 
	{
		return true;
	}

	ServerBulletInfo* bullet_info = GetBulletInfo(bDeskStation, bullet_id);
	if (bullet_info == NULL) 
	{
		return true;
	}
	ASSERT(bullet_info->bullet_mulriple == bullet_mul && bullet_info->bullet_kind == bullet_kind);
	if (!(bullet_info->bullet_mulriple == bullet_mul && bullet_info->bullet_kind == bullet_kind)) 
	{
		return true;
	}


#ifndef TEST
	if (/*!m_pUserInfo[bDeskStation]->m_UserData.isVirtual &&*/ g_stock_score_ < 0) //sdp机器人和真人一样消耗子弹2014.05.08
	{
		return true;
	}
#endif

	int fish_multiple = fish_multiple_[fish_trace_info->fish_kind];
	SCORE fish_score = fish_multiple_[fish_trace_info->fish_kind] * bullet_mul;
	if (fish_trace_info->fish_kind == FISH_KIND_18) 
	{
		int fish18_mul = fish_multiple_[fish_trace_info->fish_kind] + rand() % (fish18_max_multiple_ - fish_multiple_[fish_trace_info->fish_kind] + 1);
		fish_multiple = fish18_mul;
		fish_score = fish18_mul * bullet_mul;
	} 
	else if (fish_trace_info->fish_kind == FISH_KIND_19) 
	{
		int fish19_mul = fish_multiple_[fish_trace_info->fish_kind] + rand() % (fish19_max_multiple_ - fish_multiple_[fish_trace_info->fish_kind] + 1);
		fish_score = fish19_mul * bullet_mul;
		fish_multiple = fish19_mul;
	} 
	else if (fish_trace_info->fish_kind == FISH_KIND_21) 
	{
		fish_score = current_fish_lk_multiple_ * bullet_mul;
		fish_multiple = current_fish_lk_multiple_;
	}
	if (bullet_kind >= BULLET_KIND_1_ION) 
		fish_score *= 2;

	if (fish_score <= 0)//没分就判定打不死
	{
		return true;
	}

#ifndef TEST
	if (/*!m_pUserInfo[bDeskStation]->m_UserData.isVirtual &&*/ g_stock_score_ - fish_score < 0) //sdp机器人和真人一样消耗子弹2014.05.08
	{
		return true;
	}

	int change_probability = -1;
	//if (!m_pUserInfo[bDeskStation]->m_UserData.isVirtual) //sdp机器人和真人一样消耗子弹2014.05.08
		change_probability = CheckUserFilter(bDeskStation);
	double probability = static_cast<double>((rand() % 1000 + 1)) / 1000;
	int stock_crucial_count = stock_crucial_count_;
	double fish_probability = fish_capture_probability_[fish_trace_info->fish_kind];
	int fish20_catch_count = 0;
	double fish20_catch_probability = 0.0;
	bool fish20_config = false;
	if (fish_trace_info->fish_kind == FISH_KIND_20 && !m_pUserInfo[bDeskStation]->m_UserData.isVirtual)
	{
		fish20_config = CheckFish20Config(bDeskStation, &fish20_catch_count, &fish20_catch_probability);
		if (fish20_config) 
		{
			fish_probability = fish20_catch_probability;
		}
	}

	// 机器人能打中企鹅
	if (fish_trace_info->fish_kind == FISH_KIND_20 && m_pUserInfo[bDeskStation]->m_UserData.isVirtual) 
		fish_probability = 0.02;

	if (change_probability == 0) 
	{
		fish_probability *= 0.2;
	} 
	else if (change_probability == 1) 
	{
		fish_probability *= 1.3;
	}
	if (special_scene_ && (fish_trace_info->fish_kind == FISH_KIND_1 || fish_trace_info->fish_kind == FISH_KIND_2)) 
		fish_probability *= 0.7;

	// 炸弹库根据库存调整概率
	if (!m_pUserInfo[bDeskStation]->m_UserData.isVirtual && fish_trace_info->fish_kind == FISH_KIND_23 && g_stock_score_ < bomb_stock_)
		fish_probability = 0;
	if (!m_pUserInfo[bDeskStation]->m_UserData.isVirtual && fish_trace_info->fish_kind == FISH_KIND_24 && g_stock_score_ < super_bomb_stock_)
		fish_probability = 0;

	//////////////////////////////////////////////////////新加对3-9号鱼的难度处理
	if (!m_pUserInfo[bDeskStation]->m_UserData.isVirtual)
	{
		static int nFish=0;	//捕中过,10次经过逻辑一次
		static bool bRunFish=true;	//控制周期内是否捕中
		static double m_dRand[7]={0.2,0.3,0.5,0.6,0.8,0.4,1.0};//几率随机



		if (bRunFish==false)
		{
			if(fish_trace_info->fish_kind == FISH_KIND_3 || fish_trace_info->fish_kind == FISH_KIND_4 || fish_trace_info->fish_kind == FISH_KIND_5 || fish_trace_info->fish_kind == FISH_KIND_6 || fish_trace_info->fish_kind == FISH_KIND_7 || fish_trace_info->fish_kind == FISH_KIND_8 || fish_trace_info->fish_kind == FISH_KIND_9)
			{
				nFish++;

				if(nFish>=10)
				{
					nFish=0;
					bRunFish = true;//10次后走正常逻辑，不在走概率随机
				}
				else
				{  
					fish_probability *= m_dRand[rand()%7];
				}
			}
		}

		if(bRunFish)
		{
			if(fish_trace_info->fish_kind == FISH_KIND_3 || fish_trace_info->fish_kind == FISH_KIND_4 || fish_trace_info->fish_kind == FISH_KIND_5 || fish_trace_info->fish_kind == FISH_KIND_6 || fish_trace_info->fish_kind == FISH_KIND_7 || fish_trace_info->fish_kind == FISH_KIND_8 || fish_trace_info->fish_kind == FISH_KIND_9)
			{
				bRunFish = false;
			}
		}
	}
	//////////////////////////////////////////////////////

	// 机器人打中几率增加
	/*if (m_pUserInfo[bDeskStation]->m_UserData.isVirtual) 
		fish_probability *= 1.3;*/

	while ((--stock_crucial_count) >= 0) 
	{
		if (true/*!m_pUserInfo[bDeskStation]->m_UserData.isVirtual*/) //真人才如此判断
		{
			if (g_stock_score_ >= stock_crucial_score_[stock_crucial_count]) 
			{
				if (probability > (fish_probability * (stock_increase_probability_[stock_crucial_count] + 1))) 
				{
					
					return true;//失败
				} 
				else 
				{
					break;
				}
			}
		}
		else//机器人放宽，不要受 g_stock_score_ 干扰sdp2014-05-09
		{
			if (probability > (fish_probability * (stock_increase_probability_[stock_crucial_count] + 1))) 
			{
				return true;//失败
			} 
			else 
			{
				break;
			}
		}
	}

	if (fish20_config) 
		AddFish20Config(bDeskStation, fish20_catch_count - 1, fish20_catch_probability);
#endif

	WORD chair_id = bDeskStation;

	if (fish_trace_info->fish_kind == FISH_KIND_23 
		|| fish_trace_info->fish_kind == FISH_KIND_24 
		|| (fish_trace_info->fish_kind >= FISH_KIND_31 && fish_trace_info->fish_kind <= FISH_KIND_40)) 
	{
		SaveSweepFish(fish_trace_info->fish_kind, fish_id, bullet_kind, bullet_mul);
		CMD_S_CatchSweepFish catch_sweep_fish;
		catch_sweep_fish.chair_id = chair_id;
		catch_sweep_fish.fish_id = fish_id;
		//sdp机器人和真人一样消耗子弹2014.05.08
		SendDataToPlayer(/*m_pUserInfo[bDeskStation]->m_UserData.isVirtual ? android_chairid_ : */chair_id, &catch_sweep_fish, sizeof(CMD_S_CatchSweepFish), SUB_S_CATCH_SWEEP_FISH);
		SendWatchData(m_bMaxPeople,&catch_sweep_fish, sizeof(CMD_S_CatchSweepFish),MDM_GM_GAME_NOTIFY,SUB_S_CATCH_SWEEP_FISH,0);
	} 
	else 
	{
		fish_score_[chair_id] += fish_score;

		if (!m_pUserInfo[bDeskStation]->m_UserData.isVirtual)
			g_stock_score_ -= fish_score;

		CMD_S_CatchFish catch_fish;
		catch_fish.bullet_ion = fish_multiple >= 15 && (rand() % 100 < 10);
		catch_fish.chair_id = bDeskStation;
		catch_fish.fish_id = fish_id;
		catch_fish.fish_kind = fish_trace_info->fish_kind;
		catch_fish.fish_score = fish_score;
		if (fish_trace_info->fish_kind == FISH_KIND_22) 
		{
			SetTimer(kLockTimer, kLockTime * 1000);
			KillAllTimer();
		}

		if (catch_fish.bullet_ion) 
		{
			SetTimer(kBulletIonTimer + chair_id, kBulletIonTime * 1000);
		}

		SendDataToAllPlayers(&catch_fish, sizeof(catch_fish), SUB_S_CATCH_FISH);
		SendWatchData(m_bMaxPeople,&catch_fish, sizeof(catch_fish),MDM_GM_GAME_NOTIFY,SUB_S_CATCH_FISH,0);

		if (fish_trace_info->fish_kind == FISH_KIND_21) 
		{
			current_fish_lk_multiple_ = fish_multiple_[FISH_KIND_21];

			//发送打到李逵的系统消息
			CMD_S_CatchSweepFishResultEx cmd;
			memset(&cmd, 0, sizeof(cmd));
			if (m_pUserInfo[chair_id] != NULL)
			{
				CopyMemory(cmd.szNickName, m_pUserInfo[chair_id]->m_UserData.nickName, sizeof(cmd.szNickName));//昵称
				CopyMemory(cmd.cRoomName, m_pDataManage->m_InitData.szGameRoomName, sizeof(cmd.szNickName));//房间名字
				CopyMemory(cmd.cFishName, m_cFishName[fish_trace_info->fish_kind], sizeof(cmd.cFishName));//鱼名字
				cmd.exchange_ratio_userscore_ = exchange_ratio_userscore_;	//换算比例中用户金币
				cmd.exchange_ratio_fishscore_ = exchange_ratio_fishscore_;	//换算比例中鱼币
				cmd.iDeskID = m_pUserInfo[chair_id]->m_UserData.bDeskNO;			//桌子ID
				cmd.fish_kind = fish_trace_info->fish_kind;				//鱼类型ID
				cmd.fish_score = fish_score;		//打中鱼赚多少钱

				//发送广播消息，通知所有桌子，某某玩家打中Boss鱼
				m_pDataManage->m_TCPSocket.SendDataBatch(&cmd,sizeof(cmd),MDM_GM_GAME_NOTIFY,SUB_S_CATCH_SWEEP_FISH_RESULT_EX,0);
			}

		} else if (fish_trace_info->fish_kind == FISH_KIND_20) 
		{
			TCHAR tips_msg[1024] = { 0 };
		}
	}

	FreeFishTrace(fish_trace_info);
	FreeBulletInfo(chair_id, bullet_info);

	return true;
}

//计算累积达到最大上分时所用的次数
int CServerGameDesk::GetTopFen(BYTE bDeskStation)
{
	WORD chair_id = bDeskStation;
	SCORE exchange_fish_score = 0;
	//sdp2014-05-09 为0 点击退币则上最大分，但是为了美观，充满显示框就可以了
	int iMaxNum = 9999999;//设置不能超出显示框的最大值
	int inum = 0;//上分次数
	SCORE need_user_score = 0;
	SCORE user_leave_score = 0;

	need_user_score = exchange_ratio_userscore_ * exchange_count_ / exchange_ratio_fishscore_;
	do 
	{
		user_leave_score = m_pUserInfo[bDeskStation]->m_UserData.i64Money - (exchange_fish_score_[bDeskStation] + exchange_fish_score) * exchange_ratio_userscore_ / exchange_ratio_fishscore_;

		if (need_user_score > user_leave_score || need_user_score > iMaxNum) 
		{
			return inum;
		}
		exchange_fish_score += exchange_count_;

		++inum;
	} while (1);
}

//换算捕鱼币
bool CServerGameDesk::OnSubExchangeFishScore(BYTE bDeskStation, bool increase) 
{
	WORD chair_id = bDeskStation;
	CString sdp;
	CMD_S_ExchangeFishScore exchange_fish_score;
	exchange_fish_score.chair_id = chair_id;

	SCORE need_user_score = exchange_ratio_userscore_ * exchange_count_ / exchange_ratio_fishscore_;
	SCORE user_leave_score = m_pUserInfo[chair_id]->m_UserData.i64Money - exchange_fish_score_[chair_id] * exchange_ratio_userscore_ / exchange_ratio_fishscore_;
	if (increase) 
	{
		if (need_user_score > user_leave_score) 
		{
			return true;
		}
		fish_score_[chair_id] += exchange_count_;
		exchange_fish_score_[chair_id] += exchange_count_;
		exchange_fish_score.swap_fish_score = fish_score_[chair_id];
	} 
	else 
	{
		if (fish_score_[chair_id] == 0) 
		{
			//sdp2014-05-09 为0 点击退币则上最大分，但是为了美观，充满显示框就可以了
			int count = GetTopFen(chair_id);
			fish_score_[chair_id] += count*exchange_count_;
			exchange_fish_score_[chair_id] += count*exchange_count_;
			exchange_fish_score.swap_fish_score = fish_score_[chair_id];
			exchange_fish_score.exchange_fish_score = exchange_fish_score_[chair_id];

			SendDataToAllPlayers(&exchange_fish_score, sizeof(CMD_S_ExchangeFishScore),SUB_S_EXCHANGE_FISHSCORE);
			SendWatchData(m_bMaxPeople,&exchange_fish_score, sizeof(CMD_S_ExchangeFishScore),MDM_GM_GAME_NOTIFY,SUB_S_EXCHANGE_FISHSCORE,0);
			////////////////////////////////////////////////////////////////////////
			return true;
		}
		//正常减分
		exchange_fish_score_[chair_id] -= fish_score_[chair_id];

		exchange_fish_score.swap_fish_score = 0;
		fish_score_[chair_id] = 0;
	}

	exchange_fish_score.exchange_fish_score = exchange_fish_score_[chair_id];

	SendDataToAllPlayers(&exchange_fish_score, sizeof(CMD_S_ExchangeFishScore),SUB_S_EXCHANGE_FISHSCORE);
	SendWatchData(m_bMaxPeople,&exchange_fish_score, sizeof(CMD_S_ExchangeFishScore),MDM_GM_GAME_NOTIFY,SUB_S_EXCHANGE_FISHSCORE,0);

	return true;
}

//构造函数
CServerGameDesk::CServerGameDesk(void):CGameDesk(FULL_BEGIN)
{
	m_bIsplaying = false;
	SetGameStatus(GAME_STATUS_FREE);
	
	fish_id_ = 0;
	m_iFishNum = 0;
	fish_id_LK = -1;
	memset(fish_score_,0,sizeof(fish_score_));
	memset(exchange_fish_score_,0,sizeof(exchange_fish_score_));
	memset(m_bExitWatcher,0,sizeof(m_bExitWatcher));

	kBuildSmallFishTraceElasped			=	4;//小鱼出现时长
	kBuildMediumFishTraceElasped		=	5;//中等鱼出现时长
	kBuildFish18TraceElasped			=	33;//18号鱼出现时长
	kBuildFish19TraceElasped			=	43;//19号鱼出现时长
	kBuildFish20TraceElasped			=	41;//20号鱼出现时长
	kBuildFishLKTraceElasped			=	58;//李逵出现时长
	kBuildFishBombTraceElasped			=	68;//炸弹鱼出现时长
	kBuildFishSuperBombTraceElasped		=	77;//超级炸弹鱼出现时长
	kBuildFishLockBombTraceElasped		=	64 + 10;//定屏炸弹鱼出现时长
	kBuildFishSanTraceElasped			=	80 + 28;//大三元鱼出现时长
	kBuildFishSiTraceElasped			=	90 + 17;//大四喜鱼出现时长
	kBuildFishKingTraceElasped			=	34;//鱼王出现时长

	//加载游戏配置
	LoadConfig();
}


//析构函数
CServerGameDesk::~CServerGameDesk(void)
{
	FishTraceInfoVecor::iterator iter;
	for (iter = storage_fish_trace_vector_.begin(); iter != storage_fish_trace_vector_.end(); ++iter) {
		delete (*iter);
	}
	storage_fish_trace_vector_.clear();
	for (iter = active_fish_trace_vector_.begin(); iter != active_fish_trace_vector_.end(); ++iter) {
		delete (*iter);
	}
	active_fish_trace_vector_.clear();

	ServerBulletInfoVector::iterator it;
	for (it = storage_bullet_info_vector_.begin(); it != storage_bullet_info_vector_.end(); ++it) 
	{
		delete (*it);
	}
	storage_bullet_info_vector_.clear();
	for (WORD i = 0; i < GAME_PLAYER; ++i) 
	{
		for (it = server_bullet_info_vector_[i].begin(); it != server_bullet_info_vector_[i].end(); ++it) {
			delete (*it);
		}
		server_bullet_info_vector_[i].clear();
	}

	if (g_revenue_score > 0) 
	{
		g_revenue_score = 0;
	}
	m_SuperMan.clear();
}

//定时器消息
bool CServerGameDesk::OnTimer(UINT uTimerID)
{
	switch (uTimerID) 
	{
	case kBuildSmallFishTraceTimer:
		{
			OnTimerBuildSmallFishTrace();
			return true;
		}
	case kBuildMediumFishTraceTimer:
		{
			OnTimerBuildMediumFishTrace();
			return true;
		}
	case kBuildFish18TraceTimer:
		{
			OnTimerBuildFish18Trace();
			return true;
		}
	case kBuildFish19TraceTimer:
		{
			OnTimerBuildFish19Trace();
			return true;
		}
	case kBuildFish20TraceTimer:
		{
			OnTimerBuildFish20Trace();
			return true;
		}
	case kBuildFishLKTraceTimer:
		{
			OnTimerBuildFishLKTrace();
			return true;
		}
	case kBuildFishBombTraceTimer:
		{
			OnTimerBuildFishBombTrace();
			return true;
		}
	case kBuildFishSuperBombTraceTimer:
		{
			OnTimerBuildFishSuperBombTrace();
			return true;
		}
	case kBuildFishLockBombTraceTimer:
		{
			OnTimerBuildFishLockBombTrace();
			return true;
		}
	case kBuildFishSanTraceTimer:
		{
			OnTimerBuildFishSanTrace();
			return true;
		}
	case kBuildFishSiTraceTimer:
		{
			OnTimerBuildFishSiTrace();
			return true;
		}
	case kBuildFishKingTraceTimer:
		{
			OnTimerBuildFishKingTrace();
			return true;
		}
	case kClearTraceTimer:
		{
			OnTimerClearTrace();
			return true;
		}
	case kBulletIonTimer:
	case kBulletIonTimer + 1:
	case kBulletIonTimer + 2:
	case kBulletIonTimer + 3:
	case kBulletIonTimer + 4:
	case kBulletIonTimer + 5:
	case kBulletIonTimer + 6:
	case kBulletIonTimer + 7:
		{
			OnTimerBulletIonTimeout(WPARAM(uTimerID - kBulletIonTimer));
			return true;
		}
	case kLockTimer:
		{
			OnTimerLockTimeout();
			return true;
		}
	case kSwitchSceneTimer:
		{
			OnTimerSwitchScene();
			return true;
		}
	case kSceneEndTimer:
		{
			OnTimerSceneEnd();
			return true;
		}
	case kLKScoreTimer:
		{
			OnTimerLKScore();
			return true;
		}
	default:
		{
			ASSERT(FALSE);
			break;
		}
	}

	return __super::OnTimer(uTimerID);
}

//创建小鱼轨迹
bool CServerGameDesk::OnTimerBuildSmallFishTrace() 
{
	//if (CServerRule::IsForfendGameEnter(game_service_option_->dwServerRule)) {
	//	OnEventGameConclude(GAME_PLAYER, GER_DISMISS);
	//	return true;
	//}sdp

	BuildFishTrace(4 + rand() % 8, FISH_KIND_1, FISH_KIND_10);
	
	return true;
}

//创建中等鱼轨迹
bool CServerGameDesk::OnTimerBuildMediumFishTrace() 
{
	BuildFishTrace(1 + rand() % 5, FISH_KIND_11, FISH_KIND_17);
	return true;
}

//创建第18号鱼轨迹
bool CServerGameDesk::OnTimerBuildFish18Trace() 
{
	BuildFishTrace(1, FISH_KIND_18, FISH_KIND_18);
	return true;
}

//创建第19号鱼轨迹
bool CServerGameDesk::OnTimerBuildFish19Trace() 
{
	BuildFishTrace(1, FISH_KIND_19, FISH_KIND_19);
	return true;
}

//创建第20号鱼轨迹
bool CServerGameDesk::OnTimerBuildFish20Trace() 
{
	BuildFishTrace(1, FISH_KIND_20, FISH_KIND_20);
	return true;
}

//创建李逵轨迹
bool CServerGameDesk::OnTimerBuildFishLKTrace() 
{
	CMD_S_FishTrace fish_trace;
	m_iFishNum += 1;
	DWORD build_tick = GetTickCount();
	FishTraceInfo* fish_trace_info = ActiveFishTrace();
	fish_trace_info->fish_kind = FISH_KIND_21;
	fish_trace_info->build_tick = build_tick;
	fish_trace_info->fish_id = GetNewFishID();

	fish_trace.fish_id = fish_trace_info->fish_id;
	fish_trace.fish_kind = fish_trace_info->fish_kind;
	fish_trace.init_count = 3;
	fish_trace.trace_type = TRACE_BEZIER;
	BuildInitTrace(fish_trace.init_pos, fish_trace.init_count, fish_trace.fish_kind, fish_trace.trace_type);

	SendDataToAllPlayers(&fish_trace, sizeof(fish_trace),SUB_S_FISH_TRACE);
	SendWatchData(m_bMaxPeople,&fish_trace,sizeof(fish_trace),MDM_GM_GAME_NOTIFY,SUB_S_FISH_TRACE,0);

	current_fish_lk_multiple_ = fish_multiple_[FISH_KIND_21];
	fish_id_LK = fish_trace_info->fish_id;
	SetTimer(kLKScoreTimer, kLKScoreElasped * 1000);

	return true;
}

int CServerGameDesk::GetNewFishID() 
{
	++fish_id_;
	if (fish_id_ <= 0) fish_id_ = 1;
	return fish_id_;
}

bool CServerGameDesk::OnTimerBuildFishBombTrace() 
{
	BuildFishTrace(1, FISH_KIND_23, FISH_KIND_23);
	return true;
}

bool CServerGameDesk::OnTimerBuildFishLockBombTrace() 
{
	BuildFishTrace(1, FISH_KIND_22, FISH_KIND_22);
	return true;
}

bool CServerGameDesk::OnTimerBuildFishSuperBombTrace() 
{
	BuildFishTrace(1, FISH_KIND_24, FISH_KIND_24);
	return true;
}

bool CServerGameDesk::OnTimerBuildFishSanTrace() 
{
	BuildFishTrace(2, FISH_KIND_25, FISH_KIND_27);
	return true;
}

bool CServerGameDesk::OnTimerBuildFishSiTrace() 
{
	BuildFishTrace(2, FISH_KIND_28, FISH_KIND_30);
	return true;
}

bool CServerGameDesk::OnTimerBuildFishKingTrace() 
{
	BuildFishTrace(1, FISH_KIND_31, FISH_KIND_40);
	return true;
}

bool CServerGameDesk::OnTimerClearTrace() 
{
	ClearFishTrace();
	return true;
}

bool CServerGameDesk::OnTimerBulletIonTimeout(WPARAM bind_param) 
{
	WORD chair_id = static_cast<WORD>(bind_param);
	CMD_S_BulletIonTimeout bullet_timeout;
	bullet_timeout.chair_id = chair_id;

	SendDataToAllPlayers(&bullet_timeout, sizeof(bullet_timeout),SUB_S_BULLET_ION_TIMEOUT);
	SendWatchData(m_bMaxPeople,&bullet_timeout,sizeof(bullet_timeout),MDM_GM_GAME_NOTIFY,SUB_S_BULLET_ION_TIMEOUT,0);

	return true;
}

bool CServerGameDesk::OnTimerLockTimeout() 
{
	SendDataToAllPlayers(NULL, 0,SUB_S_LOCK_TIMEOUT);
	SendWatchData(m_bMaxPeople,NULL, 0,MDM_GM_GAME_NOTIFY,SUB_S_LOCK_TIMEOUT,0);

	StartAllGameTimer();

	KillTimer(kLockTimer);
	return true;
}

//计时器_转换场景
bool CServerGameDesk::OnTimerSwitchScene()
{
	KillAllTimer();
	ClearFishTrace(true);
	special_scene_ = true;
	SetTimer(kSceneEndTimer, kSceneEndElasped * 1000);
	if (next_scene_kind_ == SCENE_KIND_1) 
	{
		BuildSceneKind1();
	} 
	else if (next_scene_kind_ == SCENE_KIND_2) 
	{
		BuildSceneKind2();
	} 
	else if (next_scene_kind_ == SCENE_KIND_3) 
	{
		BuildSceneKind3();
	} 
	else if (next_scene_kind_ == SCENE_KIND_4) 
	{
		BuildSceneKind4();
	} 
	else if (next_scene_kind_ == SCENE_KIND_5) 
	{
		BuildSceneKind5();
	}

	next_scene_kind_ = static_cast<SceneKind>((next_scene_kind_ + 1) % SCENE_KIND_COUNT);
	return true;
}

bool CServerGameDesk::OnTimerSceneEnd() 
{
	special_scene_ = false;
	StartAllGameTimer();
	KillTimer(kSceneEndTimer);
	SendDataToAllPlayers(NULL, 0,SUB_S_SCENE_END);
	SendWatchData(m_bMaxPeople,NULL, 0,MDM_GM_GAME_NOTIFY,SUB_S_SCENE_END,0);
	return true;
}

bool CServerGameDesk::OnTimerLKScore() 
{
	if (current_fish_lk_multiple_ >= fishLK_max_multiple_) 
	{
		KillTimer(kLKScoreTimer);
		return true;
	}
	CMD_S_HitFishLK hit_fish;
	hit_fish.chair_id = 3;
	hit_fish.fish_id = fish_id_LK;
	hit_fish.fish_mulriple = ++current_fish_lk_multiple_;

	SendDataToAllPlayers(&hit_fish, sizeof(hit_fish),SUB_S_HIT_FISH_LK);
	SendWatchData(m_bMaxPeople,&hit_fish, sizeof(hit_fish),MDM_GM_GAME_NOTIFY,SUB_S_HIT_FISH_LK,0);

	if (current_fish_lk_multiple_ == fishLK_max_multiple_) 
	{
		KillTimer(kLKScoreTimer);
	}
	return true;
}


//清除所的计时器
void CServerGameDesk::KillAllTimer()
{
	KillTimer(kBuildSmallFishTraceTimer);
	KillTimer(kBuildMediumFishTraceTimer);
	KillTimer(kBuildFish18TraceTimer);
	KillTimer(kBuildFish19TraceTimer);
	KillTimer(kBuildFish20TraceTimer);
	KillTimer(kBuildFishLKTraceTimer);
	KillTimer(kBuildFishBombTraceTimer);
	KillTimer(kBuildFishLockBombTraceTimer);
	KillTimer(kBuildFishSuperBombTraceTimer);
	KillTimer(kBuildFishSanTraceTimer);
	KillTimer(kBuildFishSiTraceTimer);
	KillTimer(kBuildFishKingTraceTimer);
}

//重置游戏状态
bool CServerGameDesk::ReSetGameState(BYTE bLastStation)
{
	KillAllTimer();
	KillTimer(kBulletIonTimer);
	KillTimer(kLockTimer);
	KillTimer(kClearTraceTimer);
	KillTimer(kSwitchSceneTimer);
	KillTimer(kSceneEndTimer);
	KillTimer(kLKScoreTimer);

	m_bIsplaying = false;
	SetGameStatus(GAME_STATUS_FREE);

	fish_id_ = 0;
	m_iFishNum = 0;
	fish_id_LK = -1;
	memset(fish_score_,0,sizeof(fish_score_));
	memset(exchange_fish_score_,0,sizeof(exchange_fish_score_));
	
	for (int i = 0; i < FISH_KIND_COUNT; ++i)
	{
		FreeSweepFish(i);
	}

	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		FreeAllBulletInfo(i);
	}

	return TRUE;
}

//游戏开始
bool CServerGameDesk::GameBegin(BYTE bBeginFlag)
{
	if (__super::GameBegin(bBeginFlag)==false) 
		return false;

	SetGameStatus(GAME_STATUS_PLAY);
	StartAllGameTimer();

	SetTimer(kSwitchSceneTimer, kSwitchSceneElasped * 1000);
	SetTimer(kClearTraceTimer, kClearTraceElasped * 1000);

	return TRUE;
}

//发给所有人
void CServerGameDesk::SendDataToAllPlayers(void * pData, UINT uSize, BYTE bAssID)
{
	for (int i=0;i < PLAY_COUNT;i++) 
	{
		SendGameData(i,pData,uSize,MDM_GM_GAME_NOTIFY,bAssID,0);
	}
}
//发给指定玩家
void CServerGameDesk::SendDataToPlayer(BYTE bDeskStation, void * pData, UINT uSize, BYTE bAssID)
{
	SendGameData(bDeskStation,pData,uSize,MDM_GM_GAME_NOTIFY,bAssID,0);
}

//游戏结束
bool CServerGameDesk::GameFinish(BYTE bDeskStation, BYTE bCloseFlag)
{
	//重置数据
	ReSetGameState(bCloseFlag);
	__super::GameFinish(bDeskStation,bCloseFlag);
	return true;
}

////////////////////////////////////////////////////////////////
/////////////////////游戏辅助相关///////////////////////////////
////////////////////////////////////////////////////////////////

//判断是否正在游戏
bool CServerGameDesk::IsPlayGame(BYTE bDeskStation)
{
	return false;
	//__super::IsPlayGame(bDeskStation);
}


//用户离开游戏桌
BYTE CServerGameDesk::UserLeftDesk(BYTE bDeskStation, CGameUserInfo * pUserInfo)
{
	m_bExitWatcher[bDeskStation] = false;

	int inum = 0;//玩家人数
	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		if (NULL == m_pUserInfo[i] || bDeskStation == i)//排除自己
			continue;
		++inum;
	}

	CalcScore(bDeskStation);//算分

	if (inum == 0)//房间已经无人
	{
		GameFinish(0,GF_SALE);
	}

	return __super::UserLeftDesk(bDeskStation,pUserInfo);
}

//提前结束
BOOL CServerGameDesk::UserStop(BYTE bDeskStation, BOOL bArgee)
{
	//SendGameData((bDeskStation+2)%4,MDM_GM_GAME_NOTIFY,ASS_STOP_THING,0);
	return true;
}

bool CServerGameDesk::UserNetCut(BYTE bDeskStation, CGameUserInfo *pLostUserInfo)
{
	//断线也做离开处理 
	UserLeftDesk(bDeskStation,pLostUserInfo);
	return true;
	//m_bExitWatcher[bDeskStation] = false;

	//int inum = 0;//玩家人数
	//for (int i = 0; i < PLAY_COUNT; ++i)
	//{
	//	if (NULL == m_pUserInfo[i] || bDeskStation == i)//排除自己
	//		continue;
	//	++inum;
	//}

	//CalcScore(bDeskStation);//算分

	//if (inum == 0)//房间已经无人
	//{
	//	GameFinish(0,GF_SALE);
	//}

	//__super:: UserNetCut(bDeskStation, pLostUserInfo);
	//return true;
}
bool CServerGameDesk::UserReCome(BYTE bDeskStation, CGameUserInfo *pLostUserInfo)
{
	__super::UserReCome(bDeskStation, pLostUserInfo);
	return true ;
}

///用户坐到游戏桌
BYTE CServerGameDesk::UserSitDesk(MSG_GR_S_UserSit * pUserSit, CGameUserInfo * pUserInfo)
{
	return __super::UserSitDesk(pUserSit,pUserInfo);
}

//统计玩游戏玩家
BYTE CServerGameDesk::CountPlayer()
{
	BYTE count=0;
	for(int i=0;i<m_bMaxPeople;i++)
	{
		if(m_pUserInfo[i])
			count++;
	}
	return count;
}

void CServerGameDesk::StartAllGameTimer() 
{
	SetTimer(kBuildSmallFishTraceTimer, kBuildSmallFishTraceElasped * 1000);
	SetTimer(kBuildMediumFishTraceTimer, kBuildMediumFishTraceElasped * 1000);
	SetTimer(kBuildFish18TraceTimer, kBuildFish18TraceElasped * 1000);
	SetTimer(kBuildFish19TraceTimer, kBuildFish19TraceElasped * 1000);
	SetTimer(kBuildFish20TraceTimer, kBuildFish20TraceElasped * 1000);
	SetTimer(kBuildFishLKTraceTimer, kBuildFishLKTraceElasped * 1000);
	SetTimer(kBuildFishBombTraceTimer, kBuildFishBombTraceElasped * 1000);
	SetTimer(kBuildFishLockBombTraceTimer, kBuildFishLockBombTraceElasped * 1000);
	SetTimer(kBuildFishSuperBombTraceTimer, kBuildFishSuperBombTraceElasped * 1000);
	SetTimer(kBuildFishSanTraceTimer, kBuildFishSanTraceElasped * 1000);
	SetTimer(kBuildFishSiTraceTimer, kBuildFishSiTraceElasped * 1000);
	SetTimer(kBuildFishKingTraceTimer, kBuildFishKingTraceElasped * 1000);
}

void CServerGameDesk::BuildInitTrace(FPoint init_pos[5], int init_count, FishKind fish_kind, TraceType trace_type) {
	assert(init_count >= 2 && init_count <= 3);
	srand(GetTickCount() + rand() % kResolutionWidth);
	WORD chair_id = rand() % GAME_PLAYER;
	int center_x = kResolutionWidth / 2;
	int center_y = kResolutionHeight / 2;
	int factor = rand() % 2 == 0 ? 1 : -1;
	switch (chair_id) {
	case 0:
	case 1:
	case 2:
		init_pos[0].x = static_cast<float>(center_x + factor * (rand() % center_x));
		init_pos[0].y = 0.f - static_cast<float>(fish_bounding_box_height_[fish_kind]) * 2;
		init_pos[1].x = static_cast<float>(center_x + factor * (rand() % center_x));
		init_pos[1].y = static_cast<float>(center_y + (rand() % center_y));
		init_pos[2].x = static_cast<float>(center_x - factor * (rand() % center_x));
		init_pos[2].y = static_cast<float>(kResolutionHeight + fish_bounding_box_height_[fish_kind] * 2);
		break;
	case 3:
		init_pos[0].x = static_cast<float>(kResolutionWidth + fish_bounding_box_width_[fish_kind] * 2);
		init_pos[0].y = static_cast<float>(center_y + factor* (rand() % center_y));
		init_pos[1].x = static_cast<float>(center_x - (rand() % center_x));
		init_pos[1].y = static_cast<float>(center_y + factor* (rand() % center_y));
		init_pos[2].x = -static_cast<float>(fish_bounding_box_width_[fish_kind] * 2);
		init_pos[2].y = static_cast<float>(center_y - factor* (rand() % center_y));
		break;
	case 5:
	case 6:
	case 4:
		init_pos[0].x = static_cast<float>(center_x + factor * (rand() % center_x));
		init_pos[0].y = kResolutionHeight + static_cast<float>(fish_bounding_box_height_[fish_kind] * 2);
		init_pos[1].x = static_cast<float>(center_x + factor * (rand() % center_x));
		init_pos[1].y = static_cast<float>(center_y - (rand() % center_y));
		init_pos[2].x = static_cast<float>(center_x - factor * (rand() % center_x));
		init_pos[2].y = static_cast<float>(-fish_bounding_box_height_[fish_kind] * 2);
		break;
	case 7:
		init_pos[0].x = static_cast<float>(-fish_bounding_box_width_[fish_kind] * 2);
		init_pos[0].y = static_cast<float>(center_y + factor* (rand() % center_y));
		init_pos[1].x = static_cast<float>(center_x + (rand() % center_x));
		init_pos[1].y = static_cast<float>(center_y + factor* (rand() % center_y));
		init_pos[2].x = static_cast<float>(kResolutionWidth + fish_bounding_box_width_[fish_kind] * 2);
		init_pos[2].y = static_cast<float>(center_y - factor* (rand() % center_y));
		break;
	}

	if (trace_type == TRACE_LINEAR && init_count == 2) {
		init_pos[1].x = init_pos[2].x;
		init_pos[1].y = init_pos[2].y;
	}
}

void CServerGameDesk::BuildFishTrace(int fish_count, FishKind fish_kind_start, FishKind fish_kind_end) 
{
	//fish_count = 1;
	//if (m_iFishNum > 5)
	//{
	//	return;
	//}
	if (FISH_KIND_24 == fish_kind_start)
	{
		OutputDebugString("dwjlkpy1::创建炸弹鱼儿0");
	}
	
	m_iFishNum += fish_count;

	BYTE tcp_buffer[SOCKET_TCP_PACKET] = { 0 };
	WORD send_size = 0;
	CMD_S_FishTrace* fish_trace = reinterpret_cast<CMD_S_FishTrace*>(tcp_buffer);

	DWORD build_tick = GetTickCount();
	srand(build_tick + fish_count * 123321);
	for (int i = 0; i < fish_count; ++i) 
	{
		if (send_size + sizeof(CMD_S_FishTrace) > sizeof(tcp_buffer)) 
		{
			SendDataToAllPlayers(tcp_buffer, send_size,SUB_S_FISH_TRACE);
			SendWatchData(m_bMaxPeople,tcp_buffer, send_size,MDM_GM_GAME_NOTIFY,SUB_S_FISH_TRACE,0);

			send_size = 0;
		}

		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = static_cast<FishKind>(fish_kind_start + (rand() + i) % (fish_kind_end - fish_kind_start + 1));
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		fish_trace->fish_id = fish_trace_info->fish_id;
		fish_trace->fish_kind = fish_trace_info->fish_kind;
		if (fish_trace_info->fish_kind == FISH_KIND_1 || fish_trace_info->fish_kind == FISH_KIND_2) 
		{
			fish_trace->init_count = 2;
			fish_trace->trace_type = TRACE_LINEAR;
		} 
		else 
		{
			fish_trace->init_count = 3;
			fish_trace->trace_type = TRACE_BEZIER;
		}
		BuildInitTrace(fish_trace->init_pos, fish_trace->init_count, fish_trace->fish_kind, fish_trace->trace_type);

		send_size += sizeof(CMD_S_FishTrace);
		++fish_trace;
	}
	if (FISH_KIND_24 == fish_kind_start)
	{
		OutputDebugString("dwjlkpy1::创建炸弹鱼儿1");
	}
	if (send_size > 0) 
	{
		if (FISH_KIND_24 == fish_kind_start)
		{
			OutputDebugString("dwjlkpy1::创建炸弹鱼儿2");
		}
		SendDataToAllPlayers(tcp_buffer, send_size,SUB_S_FISH_TRACE);
		SendWatchData(m_bMaxPeople,tcp_buffer, send_size,MDM_GM_GAME_NOTIFY,SUB_S_FISH_TRACE,0);
	}
}

void CServerGameDesk::BuildSceneKind1() 
{

//	BYTE tcp_buffer[SOCKET_TCP_PACKET] = { 0 };
	CMD_S_SwitchScene TSwitchScene;
	CMD_S_SwitchScene* switch_scene = &TSwitchScene;/*reinterpret_cast<CMD_S_SwitchScene*>(tcp_buffer);*/
	switch_scene->scene_kind = next_scene_kind_;
	DWORD build_tick = GetTickCount();
	switch_scene->fish_count = 0;

	for (int i = 0; i < 100; ++i) 
	{
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_1;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[i] = fish_trace_info->fish_kind;
	}
	OutputDebugString("dwjlkpy2::BuildSceneKind1-2");
	switch_scene->fish_count += 100;
	for (int i = 0; i < 17; ++i) 
	{
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_3;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	OutputDebugString("dwjlkpy2::BuildSceneKind1-3");
	switch_scene->fish_count += 17;
	for (int i = 0; i < 17; ++i) 
	{
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_5;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	OutputDebugString("dwjlkpy2::BuildSceneKind1-4");
	switch_scene->fish_count += 17;
	for (int i = 0; i < 30; ++i) 
	{
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_2;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	OutputDebugString("dwjlkpy2::BuildSceneKind1-5");
	switch_scene->fish_count += 30;
	for (int i = 0; i < 30; ++i) 
	{
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_4;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	OutputDebugString("dwjlkpy2::BuildSceneKind1-6");
	switch_scene->fish_count += 30;
	for (int i = 0; i < 15; ++i) 
	{
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_6;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	OutputDebugString("dwjlkpy2::BuildSceneKind1-7");
	switch_scene->fish_count += 15;

	FishTraceInfo* fish_trace_info = ActiveFishTrace();
	fish_trace_info->fish_kind = FISH_KIND_20;
	fish_trace_info->build_tick = build_tick;
	fish_trace_info->fish_id = GetNewFishID();
	OutputDebugString("dwjlkpy2::BuildSceneKind1-8");
	switch_scene->fish_id[switch_scene->fish_count] = fish_trace_info->fish_id;
	switch_scene->fish_kind[switch_scene->fish_count] = fish_trace_info->fish_kind;
	switch_scene->fish_count += 1;
	OutputDebugString("dwjlkpy2::BuildSceneKind1-9");
	SendDataToAllPlayers(switch_scene, sizeof(CMD_S_SwitchScene),SUB_S_SWITCH_SCENE);
	SendWatchData(m_bMaxPeople,switch_scene, sizeof(CMD_S_SwitchScene),MDM_GM_GAME_NOTIFY,SUB_S_SWITCH_SCENE,0);
	OutputDebugString("dwjlkpy2::BuildSceneKind1-10");
}

void CServerGameDesk::BuildSceneKind2() 
{
	//	BYTE tcp_buffer[SOCKET_TCP_PACKET] = { 0 };
	CMD_S_SwitchScene TSwitchScene;
	CMD_S_SwitchScene* switch_scene = &TSwitchScene;/*reinterpret_cast<CMD_S_SwitchScene*>(tcp_buffer);*/
	switch_scene->scene_kind = next_scene_kind_;
	DWORD build_tick = GetTickCount();
	switch_scene->fish_count = 0;
	for (int i = 0; i < 200; ++i) 
	{
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_1;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 200;
	for (int i = 0; i < 14; ++i) 
	{
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = static_cast<FishKind>(FISH_KIND_12 + i % 7);
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 14;

	SendDataToAllPlayers(switch_scene, sizeof(CMD_S_SwitchScene),SUB_S_SWITCH_SCENE);
	SendWatchData(m_bMaxPeople,switch_scene, sizeof(CMD_S_SwitchScene),MDM_GM_GAME_NOTIFY,SUB_S_SWITCH_SCENE,0);
}

void CServerGameDesk::BuildSceneKind3() 
{
	//	BYTE tcp_buffer[SOCKET_TCP_PACKET] = { 0 };
	CMD_S_SwitchScene TSwitchScene;
	CMD_S_SwitchScene* switch_scene = &TSwitchScene;/*reinterpret_cast<CMD_S_SwitchScene*>(tcp_buffer);*/
	switch_scene->scene_kind = next_scene_kind_;
	DWORD build_tick = GetTickCount();
	switch_scene->fish_count = 0;
	for (int i = 0; i < 50; ++i) 
	{
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_1;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 50;

	for (int i = 0; i < 40; ++i) 
	{
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_3;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 40;

	for (int i = 0; i < 30; ++i) 
	{
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_4;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 30;

	FishTraceInfo* fish_trace_info = ActiveFishTrace();
	fish_trace_info->fish_kind = FISH_KIND_16;
	fish_trace_info->build_tick = build_tick;
	fish_trace_info->fish_id = GetNewFishID();

	switch_scene->fish_id[switch_scene->fish_count] = fish_trace_info->fish_id;
	switch_scene->fish_kind[switch_scene->fish_count] = fish_trace_info->fish_kind;
	switch_scene->fish_count += 1;

	for (int i = 0; i < 50; ++i) 
	{
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_1;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 50;

	for (int i = 0; i < 40; ++i) 
	{
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_2;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 40;

	for (int i = 0; i < 30; ++i) 
	{
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_5;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 30;

	fish_trace_info = ActiveFishTrace();
	fish_trace_info->fish_kind = FISH_KIND_17;
	fish_trace_info->build_tick = build_tick;
	fish_trace_info->fish_id = GetNewFishID();

	switch_scene->fish_id[switch_scene->fish_count] = fish_trace_info->fish_id;
	switch_scene->fish_kind[switch_scene->fish_count] = fish_trace_info->fish_kind;
	switch_scene->fish_count += 1;

	SendDataToAllPlayers(switch_scene, sizeof(CMD_S_SwitchScene),SUB_S_SWITCH_SCENE);
	SendWatchData(m_bMaxPeople,switch_scene, sizeof(CMD_S_SwitchScene),MDM_GM_GAME_NOTIFY,SUB_S_SWITCH_SCENE,0);
}

void CServerGameDesk::BuildSceneKind4() 
{
	//	BYTE tcp_buffer[SOCKET_TCP_PACKET] = { 0 };
	CMD_S_SwitchScene TSwitchScene;
	CMD_S_SwitchScene* switch_scene = &TSwitchScene;/*reinterpret_cast<CMD_S_SwitchScene*>(tcp_buffer);*/
	switch_scene->scene_kind = next_scene_kind_;
	DWORD build_tick = GetTickCount();
	switch_scene->fish_count = 0;
	for (int i = 0; i < 8; ++i) 
	{
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_11;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 8;
	for (int i = 0; i < 8; ++i) 
	{
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_12;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 8;
	for (int i = 0; i < 8; ++i) 
	{
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_13;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 8;
	for (int i = 0; i < 8; ++i) 
	{
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_14;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 8;
	for (int i = 0; i < 8; ++i) 
	{
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_15;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 8;
	for (int i = 0; i < 8; ++i) 
	{
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_16;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 8;
	for (int i = 0; i < 8; ++i) 
	{
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_17;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 8;
	for (int i = 0; i < 8; ++i) 
	{
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_18;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 8;

	SendDataToAllPlayers(switch_scene, sizeof(CMD_S_SwitchScene),SUB_S_SWITCH_SCENE);
	SendWatchData(m_bMaxPeople,switch_scene, sizeof(CMD_S_SwitchScene),MDM_GM_GAME_NOTIFY,SUB_S_SWITCH_SCENE,0);
}

void CServerGameDesk::BuildSceneKind5() 
{
	//	BYTE tcp_buffer[SOCKET_TCP_PACKET] = { 0 };
	CMD_S_SwitchScene TSwitchScene;
	CMD_S_SwitchScene* switch_scene = &TSwitchScene;/*reinterpret_cast<CMD_S_SwitchScene*>(tcp_buffer);*/
	switch_scene->scene_kind = next_scene_kind_;
	DWORD build_tick = GetTickCount();
	switch_scene->fish_count = 0;
	for (int i = 0; i < 40; ++i) 
	{
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_1;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 40;
	for (int i = 0; i < 40; ++i) 
	{
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_2;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 40;
	for (int i = 0; i < 40; ++i) 
	{
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_5;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 40;
	for (int i = 0; i < 40; ++i) 
	{
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_3;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 40;
	for (int i = 0; i < 24; ++i) 
	{
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_4;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 24;
	for (int i = 0; i < 24; ++i) 
	{
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_6;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 24;
	for (int i = 0; i < 13; ++i) {
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_7;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 13;
	for (int i = 0; i < 13; ++i) 
	{
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_6;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 13;

	FishTraceInfo* fish_trace_info = ActiveFishTrace();
	fish_trace_info->fish_kind = FISH_KIND_18;
	fish_trace_info->build_tick = build_tick;
	fish_trace_info->fish_id = GetNewFishID();
	switch_scene->fish_id[switch_scene->fish_count] = fish_trace_info->fish_id;
	switch_scene->fish_kind[switch_scene->fish_count] = fish_trace_info->fish_kind;
	switch_scene->fish_count += 1;

	fish_trace_info = ActiveFishTrace();
	fish_trace_info->fish_kind = FISH_KIND_17;
	fish_trace_info->build_tick = build_tick;
	fish_trace_info->fish_id = GetNewFishID();
	switch_scene->fish_id[switch_scene->fish_count] = fish_trace_info->fish_id;
	switch_scene->fish_kind[switch_scene->fish_count] = fish_trace_info->fish_kind;
	switch_scene->fish_count += 1;

	SendDataToAllPlayers(switch_scene, sizeof(CMD_S_SwitchScene),SUB_S_SWITCH_SCENE);
	SendWatchData(m_bMaxPeople,switch_scene, sizeof(CMD_S_SwitchScene),MDM_GM_GAME_NOTIFY,SUB_S_SWITCH_SCENE,0);
}

void CServerGameDesk::ClearFishTrace(bool force) 
{
	if (force) 
	{
		std::copy(active_fish_trace_vector_.begin(), active_fish_trace_vector_.end(), std::back_inserter(storage_fish_trace_vector_));
		active_fish_trace_vector_.clear();
	} 
	else 
	{
		FishTraceInfoVecor::iterator iter;
		FishTraceInfo* fish_trace_info = NULL;
		DWORD now_tick = GetTickCount();
		for (iter = active_fish_trace_vector_.begin(); iter != active_fish_trace_vector_.end();) 
		{
			fish_trace_info = *iter;
			if (now_tick >= (fish_trace_info->build_tick + kFishAliveTime)) 
			{
				iter = active_fish_trace_vector_.erase(iter);
				storage_fish_trace_vector_.push_back(fish_trace_info);
			} 
			else 
			{
				++iter;
			}
		}
	}
}

void CServerGameDesk::CalcScore(BYTE bDeskStation) 
{
	if (m_pUserInfo[bDeskStation] == NULL) return;
	//给离开的玩家结算
	//写入数据库				
	__int64 temp_point[PLAY_COUNT];
	__int64 iChangeMoney[PLAY_COUNT];
	bool temp_cut[PLAY_COUNT];
	memset(temp_point,0,sizeof(temp_point));
	memset(iChangeMoney,0,sizeof(iChangeMoney));
	memset(temp_cut,0,sizeof(temp_cut));

	temp_point[bDeskStation] = (fish_score_[bDeskStation] - exchange_fish_score_[bDeskStation]) * exchange_ratio_userscore_ / exchange_ratio_fishscore_;

	ChangeUserPointint64(temp_point,temp_cut);
	__super::RecoderGameInfo(iChangeMoney);

	//数据清零
	fish_score_[bDeskStation] = 0;
	exchange_fish_score_[bDeskStation] = 0;
}

bool CServerGameDesk::OnEventGameStart() 
{
	return true;
}

//游戏结束事件
bool CServerGameDesk::OnEventGameConclude(WORD chair_id, BYTE reason) 
{
	//if (reason == GER_DISMISS) 
	//{
	//	for (WORD i = 0; i < GAME_PLAYER; ++i) 
	//	{
	//		if (m_pUserInfo[i] == NULL)
	//			continue;
	//		CalcScore(i);
	//	}

	//	GameFinish(0,GF_NORMAL);
	//	KillAllTimer();
	//	KillTimer(kSwitchSceneTimer);
	//	KillTimer(kClearTraceTimer);
	//	ClearFishTrace(true);
	//	next_scene_kind_ = SCENE_KIND_1;
	//	special_scene_ = false;
	//	android_chairid_ = INVALID_CHAIR;
	//} 
	//else if (chair_id < GAME_PLAYER && m_pUserInfo[chair_id] != NULL) 
	//{
	//	CalcScore(chair_id);
	//}
	return true;
}

//获取游戏状态信息
bool CServerGameDesk::OnGetGameStation(BYTE bDeskStation, UINT uSocketID, bool bWatchUser)
{
	if (!m_bIsplaying)
	{
		m_bIsplaying = true;
		GameBegin(ALL_ARGEE);
	}
	
	GameStation cmd;
	cmd.game_status = GetGameStatus();

	//不允许旁观
	if (bWatchUser)
	{
		return false;
		m_bExitWatcher[bDeskStation] = true;
	}
	
	if (!OnEventSendGameScene(bDeskStation, cmd.game_status,cmd))
	{
		return true;
	}
	SendGameStation(bDeskStation,uSocketID,bWatchUser,&cmd,sizeof(GameStation));
	return true;
}

bool CServerGameDesk::OnEventSendGameScene(WORD chair_id, BYTE game_status, GameStation& cmd) 
{
	switch (game_status) 
	{
	case GAME_STATUS_FREE:
	case GAME_STATUS_PLAY:
		//配置数据
		SendGameConfig(chair_id, cmd);

		//状态数据
		cmd.GameStatus.game_version = GAME_VERSION;
		memcpy(cmd.GameStatus.fish_score, fish_score_, sizeof(cmd.GameStatus.fish_score));
		memcpy(cmd.GameStatus.exchange_fish_score, exchange_fish_score_, sizeof(cmd.GameStatus.exchange_fish_score));
		//游戏消息
		BYTE mess[] = TEXT("键盘↑↓键加减炮，→←键上下分，空格键或鼠标左键发射子弹，F4关闭声音!");
		CopyMemory(cmd.GameMessage,mess,sizeof(BYTE)*MAX_PATH);
		return true;
	}
	return false;
}

bool CServerGameDesk::SendGameConfig(BYTE bDeskStation, GameStation& cmd) 
{
	cmd.GameConfig.exchange_ratio_userscore = exchange_ratio_userscore_;
	cmd.GameConfig.exchange_ratio_fishscore = exchange_ratio_fishscore_;
	cmd.GameConfig.exchange_count = exchange_count_;
	cmd.GameConfig.min_bullet_multiple = min_bullet_multiple_;
	cmd.GameConfig.max_bullet_multiple = max_bullet_multiple_;
	cmd.GameConfig.bomb_range_width = bomb_range_width_;
	cmd.GameConfig.bomb_range_height = bomb_range_height_;
	for (int i = 0; i < FISH_KIND_COUNT; ++i) 
	{
		cmd.GameConfig.fish_multiple[i] = fish_multiple_[i];
		cmd.GameConfig.fish_speed[i] = fish_speed_[i];
		cmd.GameConfig.fish_bounding_box_width[i] = fish_bounding_box_width_[i];
		cmd.GameConfig.fish_bounding_box_height[i] = fish_bounding_box_height_[i];
		cmd.GameConfig.fish_hit_radius[i] = fish_hit_radius_[i];
	}

	for (int i = 0; i < BULLET_KIND_COUNT; ++i) 
	{
		cmd.GameConfig.bullet_speed[i] = bullet_speed_[i];
		cmd.GameConfig.net_radius[i] = net_radius_[i];
	}

	return true;
}

FishTraceInfo* CServerGameDesk::GetFishTraceInfo(int fish_id) {
	FishTraceInfoVecor::iterator iter;
	FishTraceInfo* fish_trace_info = NULL;
	for (iter = active_fish_trace_vector_.begin(); iter != active_fish_trace_vector_.end(); ++iter) {
		fish_trace_info = *iter;
		if (fish_trace_info->fish_id == fish_id) return fish_trace_info;
	}

	return NULL;
}

void CServerGameDesk::SaveSweepFish(FishKind fish_kind, int fish_id, BulletKind bullet_kind, int bullet_mulriple) {
	SweepFishInfo sweep_fish;
	sweep_fish.fish_kind = fish_kind;
	sweep_fish.fish_id = fish_id;
	sweep_fish.bullet_kind = bullet_kind;
	sweep_fish.bullet_mulriple = bullet_mulriple;
	sweep_fish_info_vector_.push_back(sweep_fish);
}

bool CServerGameDesk::FreeSweepFish(int fish_id) {
	std::vector<SweepFishInfo>::iterator iter;
	for (iter = sweep_fish_info_vector_.begin(); iter != sweep_fish_info_vector_.end(); ++iter) {
		if ((*iter).fish_id == fish_id) {
			sweep_fish_info_vector_.erase(iter);
			return true;
		}
	}

	assert(!"FreeSweepFish Failed");
	return false;
}

//获得
SweepFishInfo* CServerGameDesk::GetSweepFish(int fish_id) {
	std::vector<SweepFishInfo>::iterator iter;
	for (iter = sweep_fish_info_vector_.begin(); iter != sweep_fish_info_vector_.end(); ++iter) {
		if ((*iter).fish_id == fish_id) {
			return &(*iter);
		}
	}
	return NULL;
}

//从仓库中激活一颗子弹数据给指定玩家
ServerBulletInfo* CServerGameDesk::ActiveBulletInfo(WORD chairid) {
	ServerBulletInfo* bullet_info = NULL;
	if (storage_bullet_info_vector_.size() > 0) {
		bullet_info = storage_bullet_info_vector_.back();
		storage_bullet_info_vector_.pop_back();
		server_bullet_info_vector_[chairid].push_back(bullet_info);
	}

	if (bullet_info == NULL) {
		bullet_info = new ServerBulletInfo();
		server_bullet_info_vector_[chairid].push_back(bullet_info);
	}

	return bullet_info;
}

//清除指定玩家指定子弹信息，并将清除的数据保存到仓库中
bool CServerGameDesk::FreeBulletInfo(WORD chairid, ServerBulletInfo* bullet_info) {
	ServerBulletInfoVector::iterator iter;
	for (iter = server_bullet_info_vector_[chairid].begin(); iter != server_bullet_info_vector_[chairid].end(); ++iter) {
		if (bullet_info == *iter) {
			server_bullet_info_vector_[chairid].erase(iter);
			storage_bullet_info_vector_.push_back(bullet_info);
			return true;
		}
	}

	assert(!"FreeBulletInfo Failed");
	return false;
}

//清理指定玩家所有子弹信息
void CServerGameDesk::FreeAllBulletInfo(WORD chairid) {
	std::copy(server_bullet_info_vector_[chairid].begin(), server_bullet_info_vector_[chairid].end(), std::back_inserter(storage_bullet_info_vector_));
	server_bullet_info_vector_[chairid].clear();
}

//获取子弹信息
ServerBulletInfo* CServerGameDesk::GetBulletInfo(WORD chairid, int bullet_id) {
	ServerBulletInfoVector::iterator iter;
	ServerBulletInfo* bullet_info = NULL;
	for (iter = server_bullet_info_vector_[chairid].begin(); iter != server_bullet_info_vector_[chairid].end(); ++iter) {
		bullet_info = *iter;
		if (bullet_info->bullet_id == bullet_id) return bullet_info;
	}
	//assert(!"GetBulletInfo:not found");
	return NULL;
}
void CServerGameDesk::UTF_8ToUnicode(wchar_t* pOut,char *pText)  
{     
	char* uchar = (char *)pOut;   
	uchar[1] = ((pText[0] & 0x0F) << 4) + ((pText[1] >> 2) & 0x0F);  
	uchar[0] = ((pText[1] & 0x03) << 6) + (pText[2] & 0x3F);   
}   

void CServerGameDesk::UnicodeToGB2312(char* pOut,wchar_t uData)  
{   
	WideCharToMultiByte(CP_ACP,NULL,&uData,1,pOut,sizeof(wchar_t),NULL,NULL);   
} 

void CServerGameDesk::UTF_8ToGB2312(char*pOut, char *pText, int pLen)    
{     
	char Ctemp[4];     
	memset(Ctemp,0,4);   
	int i =0 ,j = 0;   
	while(i < pLen)   
	{    
		if(pText[i] >= 0)    
		{    
			pOut[j++] = pText[i++];   
		}   
		else   
		{    
			WCHAR Wtemp;   
			UTF_8ToUnicode(&Wtemp,pText + i);  
			UnicodeToGB2312(Ctemp,Wtemp);   
			pOut[j] = Ctemp[0];  
			pOut[j + 1] = Ctemp[1];    
			i += 3;     
			j += 2;    
		}     
	}
	
	pOut[j] ='\0';
	return;   
}   

bool CServerGameDesk::LoadConfig() 
{
	TCHAR file_name[MAX_PATH] = { 0 };
	_sntprintf(file_name, CountArray(file_name), TEXT("lkpy_config_%s.xml"), SKIN_FOLDER);

	TiXmlDocument xml_doc;
	if (!xml_doc.LoadFile(CT2A(file_name), TIXML_ENCODING_UTF8)) 
	{
		return false;
	}
	const TiXmlElement* xml_element = xml_doc.FirstChildElement("Config");
	if (xml_element == NULL) 
	{
		return false;
	}
	
	m_SuperMan.clear();
	const TiXmlElement* xml_child = NULL;
	int fish_count = 0, bullet_kind_count = 0;
	for (xml_child = xml_element->FirstChildElement(); xml_child; xml_child = xml_child->NextSiblingElement()) 
	{
		if (!strcmp(xml_child->Value(), "Stock")) 
		{
			//库存(不能超过10个):stockScore库存分数,大于些值执行相应概率,从最大的开始判断,小于0将打不中鱼
			for (const TiXmlElement* xml_stock = xml_child->FirstChildElement(); xml_stock; xml_stock = xml_stock->NextSiblingElement()) 
			{
				xml_stock->Attribute("stockScore", &stock_crucial_score_[stock_crucial_count_]);
				xml_stock->Attribute("increaseProbability", &stock_increase_probability_[stock_crucial_count_]);
				++stock_crucial_count_;
				if (stock_crucial_count_ >= 10) 
					break;
			}
		} 
		else if (!strcmp(xml_child->Value(), "ScoreExchange")) 
		{
			//金币和渔币的兑换(金币:渔币) exchangeCount:每次兑换数量
			const char* attri = xml_child->Attribute("exchangeRatio");
			char* temp = NULL;
			exchange_ratio_userscore_ = strtol(attri, &temp, 10);
			exchange_ratio_fishscore_ = strtol(temp + 1, &temp, 10);
			xml_child->Attribute("exchangeCount", &exchange_count_);
		} 
		else if (!strcmp(xml_child->Value(), "Cannon")) 
		{
			const char* attri = xml_child->Attribute("cannonMultiple");//大炮倍数
			char* temp = NULL;
			min_bullet_multiple_ = strtol(attri, &temp, 10);
			max_bullet_multiple_ = strtol(temp + 1, &temp, 10);
		} 
		else if (!strcmp(xml_child->Value(), "Bomb"))//炸弹(局部炸弹,超级炸弹) 当库存小于此值炸弹被击中的几率为0
		{
			const char* attri = xml_child->Attribute("BombProbability");
			char* temp = NULL;
			bomb_stock_ = strtol(attri, &temp, 10);
			super_bomb_stock_ = strtol(temp + 1, &temp, 10);
		} 
		else if (!strcmp(xml_child->Value(), "Fish")) 
		{
			//<!-- kind: 对应 CMD_Fish.h文件的FishKind的枚举值 不能更改 
			// name:只作为描述 speed:鱼的速度 multiple:鱼的倍数 BoundingBox:鱼的边框 hitRadius:击中半径 captureProbability:捕获概率(0-1)  -->
			//<!-- 特殊鱼 鱼王和超级炸弹是圆的 用BoundingBox的第1个参数作为直径 -->
			//<!-- 特殊鱼 局部炸弹的BoundingBox的后2个参数表示炸弹的涉及范围 -->
			//<!-- 特殊鱼 FISH_KIND_18(鲨鱼)FISH_KIND_19(龙)和FISH_KIND_21(李逵)的multiple是随机倍数的2个参数表示最小和最大 -->
			int fish_kind;
			
			xml_child->Attribute("kind", &fish_kind);
			if (fish_kind >= FISH_KIND_COUNT || fish_kind < 0) 
				return false;

			const char* cFishName = xml_child->Attribute("name");
			UTF_8ToGB2312(m_cFishName[fish_kind], (char*)cFishName, strlen(cFishName));

			xml_child->Attribute("speed", &fish_speed_[fish_kind]);
			if (fish_kind == FISH_KIND_18) 
			{
				const char* attri = xml_child->Attribute("multiple");
				char* temp = NULL;
				fish_multiple_[fish_kind] = strtol(attri, &temp, 10);
				fish18_max_multiple_ = strtol(temp + 1, &temp, 10);
			} 
			else if (fish_kind == FISH_KIND_19) 
			{
				const char* attri = xml_child->Attribute("multiple");
				char* temp = NULL;
				fish_multiple_[fish_kind] = strtol(attri, &temp, 10);
				fish19_max_multiple_ = strtol(temp + 1, &temp, 10);
			} 
			else if (fish_kind == FISH_KIND_21) 
			{
				const char* attri = xml_child->Attribute("multiple");
				char* temp = NULL;
				fish_multiple_[fish_kind] = strtol(attri, &temp, 10);
				fishLK_max_multiple_ = strtol(temp + 1, &temp, 10);
			} 
			else 
			{
				xml_child->Attribute("multiple", &fish_multiple_[fish_kind]);
			}
			const char* attri = xml_child->Attribute("BoundingBox");
			char* temp = NULL;
			fish_bounding_box_width_[fish_kind] = strtol(attri, &temp, 10);
			fish_bounding_box_height_[fish_kind] = strtol(temp + 1, &temp, 10);
			if (fish_kind == FISH_KIND_23) 
			{
				bomb_range_width_ = strtol(temp + 1, &temp, 10);
				bomb_range_height_ = strtol(temp + 1, &temp, 10);
			}
			xml_child->Attribute("hitRadius", &fish_hit_radius_[fish_kind]);
			xml_child->Attribute("captureProbability", &fish_capture_probability_[fish_kind]);
			++fish_count;
		} 
		else if (!strcmp(xml_child->Value(), "Bullet")) 
		{
			//kind:对应BulletKind name:描述 speed:子弹速度 netRadius:渔网的半径
			int bullet_kind;
			xml_child->Attribute("kind", &bullet_kind);
			if (bullet_kind >= BULLET_KIND_COUNT || bullet_kind < 0) 
				return false;
			xml_child->Attribute("speed", &bullet_speed_[bullet_kind]);
			xml_child->Attribute("netRadius", &net_radius_[bullet_kind]);
			++bullet_kind_count;
		}
		else if(!strcmp(xml_child->Value(), "SuperMan")) 
		{
			//UserID:玩家ID CellScore:单位分数

			SuperManInfo SuperMan;
			memset(&SuperMan,0,sizeof(SuperMan));
			xml_child->Attribute("UserID", &SuperMan.iUserID);
			int SuperMan_CellScore;
			xml_child->Attribute("CellScore", &SuperMan.iCellScore);

			m_SuperMan.push_back(SuperMan);
		}
		else if(!strcmp(xml_child->Value(), "BuildFish")) 
		{
			//<!-- BuildFish:各种鱼生成间歇时间 BuildTime 单位秒-->
			//<!-- 鱼依次为0：小鱼出现时长、1：中等鱼出现时长、2：18号鱼出现时长、3：19号鱼出现时长、4：20号鱼出现时长-->
			//<!--        5：李逵出现时长、6：炸弹鱼出现时长、7：超级炸弹鱼出现时长、8：锁定炸弹鱼出现时长-->

			int iBuildID = -1;
			int iBuildTime = 0;
			xml_child->Attribute("BuildID", &iBuildID);
			xml_child->Attribute("BuildTime", &iBuildTime);
			if (iBuildID == 0)
			{
				if (iBuildTime > 0)
				{
					kBuildSmallFishTraceElasped = iBuildTime;
				}
			}
			else if (iBuildID == 1)
			{
				if (iBuildTime > 0)
				{
					kBuildMediumFishTraceElasped = iBuildTime;
				}
			}
			else if (iBuildID == 2)
			{
				if (iBuildTime > 0)
				{
					kBuildFish18TraceElasped = iBuildTime;
				}
			}
			else if (iBuildID == 3)
			{
				if (iBuildTime > 0)
				{
					kBuildFish19TraceElasped = iBuildTime;
				}
			}
			else if (iBuildID == 4)
			{
				if (iBuildTime > 0)
				{
					kBuildFish20TraceElasped = iBuildTime;
				}
			}
			else if (iBuildID == 5)
			{
				if (iBuildTime > 0)
				{
					kBuildFishLKTraceElasped = iBuildTime;
				}
			}
			else if (iBuildID == 6)
			{
				if (iBuildTime > 0)
				{
					kBuildFishBombTraceElasped = iBuildTime;
				}
			}
			else if (iBuildID == 7)
			{
				if (iBuildTime > 0)
				{
					kBuildFishSuperBombTraceElasped = iBuildTime;
				}
			}
			else if (iBuildID == 8)
			{
				if (iBuildTime > 0)
				{
					kBuildFishLockBombTraceElasped = iBuildTime;
				}
			}
			else if (iBuildID == 9)
			{
				if (iBuildTime > 0)
				{
					kBuildFishSanTraceElasped = iBuildTime;
				}
			}
			else if (iBuildID == 10)
			{
				if (iBuildTime > 0)
				{
					kBuildFishSiTraceElasped = iBuildTime;
				}
			}
			else if (iBuildID == 11)
			{
				if (iBuildTime > 0)
				{
					kBuildFishKingTraceElasped = iBuildTime;
				}
			}
		}
	}

	if (fish_count != FISH_KIND_COUNT) 
		return false;
	if (bullet_kind_count != BULLET_KIND_COUNT) 
		return false;

	return true;
}

//是否是超端用户
bool CServerGameDesk::IsSuperMan(BYTE bDesk)
{
	if (m_pUserInfo[bDesk] == NULL)
		return false;

	for (int i = 0; i < m_SuperMan.size();++i)
	{
		if (m_SuperMan.at(i).iUserID == m_pUserInfo[bDesk]->m_UserData.dwUserID)
		{
			return true;
		}
	}

	return false;
}

//获取该玩家的单位分数
int CServerGameDesk::GetSuperCellscore(BYTE bDesk)
{
	if (!IsSuperMan(bDesk))
		return 0;

	for (int i = 0; i < m_SuperMan.size();++i)
	{
		if (m_SuperMan.at(i).iUserID == m_pUserInfo[bDesk]->m_UserData.dwUserID)
		{
			return m_SuperMan.at(i).iCellScore;
		}
	}
}

//根据房间ID加载ini配置文件
BOOL CServerGameDesk::LoadExtIni(UINT uRoomID)
{
	//CString s = CINIFile::GetAppPath ();/////本地路径
	//CINIFile f( s +SKIN_FOLDER +".ini");

	//CString str;
	//str.Format("11100400_%d",uRoomID);
	//CString key =str;
	//m_iBeginNt = f.GetKeyVal(key,"beginnt",m_iBeginNt);
	//m_iEndNt = f.GetKeyVal(key,"endnt",m_iEndNt);

	//::memset(m_iPlayNtNum,m_iBeginNt,sizeof(m_iPlayNtNum));
	return TRUE;
}

//加载ini配置文件
BOOL CServerGameDesk::LoadIni()
{
	//CString s = CINIFile::GetAppPath ();/////本地路径
	//CINIFile f( s +SKIN_FOLDER +".ini");

	//
	//CString key = TEXT("game");
	//m_bHaveKing = f.GetKeyVal(key,"haveking",1);
	//m_iPlayCard = f.GetKeyVal(key,"card",2);
	//m_iPlayCount = f.GetKeyVal(key,"cardcount",108);
	//m_iSendCount = f.GetKeyVal(key,"sendcard",108);
	//m_iBackCount = f.GetKeyVal(key,"backcard",0);
	//m_iUserCount = f.GetKeyVal(key,"usercount",27);
	//m_iLimitUpGrade = f.GetKeyVal(key,"limitupgrade",5);
	//m_bKingCanReplace = f.GetKeyVal(key,"kingcanreplace",0);
	//m_bAutoBegin = f.GetKeyVal(key,"AutoBegin",0);
	//key=TEXT("other");
	//m_iBeginTime = f.GetKeyVal(key,"begintime",15);
	//m_iThinkTime = f.GetKeyVal(key,"thinktime",30);
	//m_EndWaitTime = f.GetKeyVal(key,"endwaittime",15);
	//m_ContributeTime = f.GetKeyVal(key,"contributetime",30);
	//
	//key = TEXT("cardshape");
	//m_iCardShape1 &=0x00000000;
	//m_iCardShape2 &=0x00000000;
	//m_iCardShape1 |= (f.GetKeyVal(key,"one",1)<<1&0xFFFFFFFF);					//单张
	//m_iCardShape1 |= ((f.GetKeyVal(key,"two",1)<<2)&0xFFFFFFFF);				//对
	//
	//m_iCardShape1 |= ((f.GetKeyVal(key,"varstraight",1)<<3)&0xFFFFFFFF);		//变种单顺
	//m_iCardShape1 |= ((f.GetKeyVal(key,"traight",1)<<4)&0xFFFFFFFF);			//单顺
	//m_iCardShape1 |= ((f.GetKeyVal(key,"flush",0)<<5)&0xFFFFFFFF);				//同花
	//m_iCardShape2 |= ((f.GetKeyVal(key,"straightflush",1)<<16)&0xFFFFFFFF);		//同花顺(移45位换成45-30=15

	//m_iCardShape2 |= ((f.GetKeyVal(key,"varstraightflush",1)<<17)&0xFFFFFFFF);	//变种同花顺
	//m_iCardShape1 |= ((f.GetKeyVal(key,"three",1)<<8)&0xFFFFFFFF);				//三条
	//m_iCardShape1 |= ((f.GetKeyVal(key,"threeandone",0)<<9)&0xFFFFFFFF);		//三带单
	//m_iCardShape1 |= ((f.GetKeyVal(key,"threeandtwo",0)<<10)&0xFFFFFFFF);		//三带二
	////以上1-10(同花顺改成46,变种同花顺改成47)

	//m_iCardShape1 |= ((f.GetKeyVal(key,"threeanddouble",1)<<11)&0xFFFFFFFF);	//三带带对
	//m_iCardShape1 |= ((f.GetKeyVal(key,"vardoubleseq",1)<<12)&0xFFFFFFFF);		//变种双顺
	//m_iCardShape1 |= ((f.GetKeyVal(key,"doubleseq",1)<<13)&0xFFFFFFFF);			//双顺
	//m_iCardShape1 |= ((f.GetKeyVal(key,"varthreeseq",1)<<14)&0xFFFFFFFF);		//变种三顺
	//m_iCardShape1 |= ((f.GetKeyVal(key,"threeseq",1)<<15)&0xFFFFFFFF);			//三顺
	//m_iCardShape1 |= ((f.GetKeyVal(key,"varthreeoneseq",0)<<16)&0xFFFFFFFF);	//变种三带单顺
	//m_iCardShape1 |= ((f.GetKeyVal(key,"threeoneseq",0)<<17)&0xFFFFFFFF);		//三带单顺

	//m_iCardShape1 |= ((f.GetKeyVal(key,"varthreetwoseq",0)<<18)&0xFFFFFFFF);	//变种三带二顺
	//m_iCardShape1 |= ((f.GetKeyVal(key,"threetwoseq",0)<<19)&0xFFFFFFFF);		//三带二顺
	//m_iCardShape1 |= ((f.GetKeyVal(key,"varthreedoubleseq",0)<<20)&0xFFFFFFFF);	//变种三带对顺
	////以上11-20位填充

	//m_iCardShape1 |= ((f.GetKeyVal(key,"threedoubleseq",0)<<21)&0xFFFFFFFF);		//三带对顺
	//m_iCardShape1 |= ((f.GetKeyVal(key,"varthreeseqdoubleseq",0)<<22)&0xFFFFFFFF);	//变种三顺带二顺
	//m_iCardShape1 |= ((f.GetKeyVal(key,"threeseqdoubleseq",0)<<23)&0xFFFFFFFF);		//三顺带二顺
	//m_iCardShape1 |= ((f.GetKeyVal(key,"fourandone",0)<<24)&0xFFFFFFFF);			//四带单
	//m_iCardShape1 |= ((f.GetKeyVal(key,"fourandtwo",0)<<25)&0xFFFFFFFF);			//四带二单
	//m_iCardShape1 |= ((f.GetKeyVal(key,"fourandonedouble",0)<<26)&0xFFFFFFFF);		//四带对
	//m_iCardShape1 |= ((f.GetKeyVal(key,"fourandtwodouble",0)<<27)&0xFFFFFFFF);		//四带二对
	//m_iCardShape1 |= ((f.GetKeyVal(key,"varfourseq",0)<<28)&0xFFFFFFFF);			//变种四顺
	//m_iCardShape1 |= ((f.GetKeyVal(key,"fourseq",0)<<29)&0xFFFFFFFF);				//四顺
	//m_iCardShape1 |= ((f.GetKeyVal(key,"varfouroneseq",0)<<30)&0xFFFFFFFF);			//变种四带单顺
	////以上21-30位填充
	//m_iCardShape2 |= ((f.GetKeyVal(key,"fouroneseq",0)<<0)&0xFFFFFFFF);				//四带单顺(30-30=0位
	//m_iCardShape2 |= ((f.GetKeyVal(key,"varfourtwoseq",0)<<1)&0xFFFFFFFF);			//变种四带二顺
	//m_iCardShape2 |= ((f.GetKeyVal(key,"fourtwoseq",0)<<2)&0xFFFFFFFF);				//四带二顺
	//m_iCardShape2 |= ((f.GetKeyVal(key,"varfouronedoubleseq",0)<<3)&0xFFFFFFFF);	//变种四带单顺
	//m_iCardShape2 |= ((f.GetKeyVal(key,"fouronedoubleseq",0)<<4)&0xFFFFFFFF);		//单对顺
	//m_iCardShape2 |= ((f.GetKeyVal(key,"varfourtwodoubleseq",0)<<5)&0xFFFFFFFF);	//变种四带二对顺
	//m_iCardShape2 |= ((f.GetKeyVal(key,"fourtwodoubleseq",0)<<6)&0xFFFFFFFF);		//四带二对顺
	//m_iCardShape2 |= ((f.GetKeyVal(key,"slave510k",0)<<7)&0xFFFFFFFF);				//非同花510k
	//m_iCardShape2 |= ((f.GetKeyVal(key,"master510k",0)<<8)&0xFFFFFFFF);				//同花510k
	////以上31-39位填充
	//m_iCardShape2 |= ((f.GetKeyVal(key,"bomb",1)<<15)&0xFFFFFFFF);					//炸弹
	//m_iCardShape2 |= ((f.GetKeyVal(key,"samehuabomb",0)<<19)&0xFFFFFFFF);			//同花炸弹
	//m_iCardShape2 |= ((f.GetKeyVal(key,"kingbomb",1)<<20)&0xFFFFFFFF);				//王炸

	return true;
}

FishTraceInfo* CServerGameDesk::ActiveFishTrace() 
{
	FishTraceInfo* fish_trace_info = NULL;
	if (storage_fish_trace_vector_.size() > 0) 
	{
		fish_trace_info = storage_fish_trace_vector_.back();
		storage_fish_trace_vector_.pop_back();
		active_fish_trace_vector_.push_back(fish_trace_info);
	}

	if (fish_trace_info == NULL) 
	{
		fish_trace_info = new FishTraceInfo;
		active_fish_trace_vector_.push_back(fish_trace_info);
	}

	return fish_trace_info;
}

bool CServerGameDesk::FreeFishTrace(FishTraceInfo* fish_trace_info) {
	FishTraceInfoVecor::iterator iter;
	for (iter = active_fish_trace_vector_.begin(); iter != active_fish_trace_vector_.end(); ++iter) {
		if (fish_trace_info == *iter) {
			active_fish_trace_vector_.erase(iter);
			storage_fish_trace_vector_.push_back(fish_trace_info);

			m_iFishNum--;
			return true;
		}
	}

	assert(!"FreeFishTrace Failed");
	return false;
}

//存在真人
bool CServerGameDesk::ExistUnAI()
{
	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		if(m_pUserInfo[i] == NULL)
			continue;
		if (m_pUserInfo[i]->m_UserData.isVirtual == 0 || m_bExitWatcher[i])//存在真人
		{
			return true;
		}
	}

	return false;
}

bool CServerGameDesk::OnSubUserFire(BYTE bDeskStation, BulletKind bullet_kind, float angle, int bullet_mul, int lock_fishid) 
{
	//sdp机器人和真人一样消耗子弹2014.05.08
	// 没真实玩家机器人不打炮
	if (!ExistUnAI() ) 
	{
		return true;
	}

	if (bullet_mul < min_bullet_multiple_ || bullet_mul > max_bullet_multiple_) 
	{
		return true;
	}

	WORD chair_id = bDeskStation;
	assert(fish_score_[chair_id] >= bullet_mul);
	if (fish_score_[chair_id] < bullet_mul) 
	{
			return true;
	}

	fish_score_[chair_id] -= bullet_mul;

	if (!m_pUserInfo[bDeskStation]->m_UserData.isVirtual)
	{
		int revenue = /*game_service_option_->wRevenueRatio*/10 * bullet_mul / 100;//税收比例
		g_stock_score_ += bullet_mul - revenue;
		g_revenue_score += revenue;
	}
	if (lock_fishid > 0 && GetFishTraceInfo(lock_fishid) == NULL) 
	{
		lock_fishid = 0;
	}

	CMD_S_UserFire user_fire;
	user_fire.bullet_kind = bullet_kind;
	user_fire.bullet_id = GetBulletID(chair_id);
	user_fire.angle = angle;
	user_fire.chair_id = bDeskStation;
	user_fire.android_chairid = /*m_pUserInfo[bDeskStation]->m_UserData.isVirtual ? android_chairid_ :*/ INVALID_CHAIR;//sdp机器人和真人一样消耗子弹2014.05.08
	user_fire.bullet_mulriple = bullet_mul;
	user_fire.fish_score = -bullet_mul;
	user_fire.lock_fishid = lock_fishid;
	SendDataToAllPlayers(&user_fire, sizeof(user_fire), SUB_S_USER_FIRE);
	SendWatchData(m_bMaxPeople,&user_fire, sizeof(user_fire),MDM_GM_GAME_NOTIFY,SUB_S_USER_FIRE,0);
	ServerBulletInfo* bullet_info = ActiveBulletInfo(chair_id);
	bullet_info->bullet_id = user_fire.bullet_id;
	bullet_info->bullet_kind = user_fire.bullet_kind;
	bullet_info->bullet_mulriple = user_fire.bullet_mulriple;
	
	return true;
}

int CServerGameDesk::GetBulletID(WORD chairid) 
{
	ASSERT(chairid < GAME_PLAYER);
	++bullet_id_[chairid];
	if (bullet_id_[chairid] <= 0) bullet_id_[chairid] = 1;
	return bullet_id_[chairid];
}
//---------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//修改奖池数据1(平台->游戏)
/// @param bAIWinAndLostAutoCtrl 机器人控制输赢开关
/// @return 是否成功
bool CServerGameDesk::SetRoomPond(bool	bAIWinAndLostAutoCtrl)
{
	return false;
}

//修改奖池数据2(平台->游戏)
/// @param m_iAIWantWinMoney 机器人输赢控制：机器人赢钱区域1,2,3 4所用断点
/// @param m_iAIWinLuckyAt 机器人输赢控制：机器人在区域1,2,3,4赢钱的概率
/// @return 是否成功
bool CServerGameDesk::SetRoomPondEx(__int64	iAIWantWinMoney[], int	iAIWinLuckyAt[])
{
	return false;
}

//修改奖池数据3(平台->游戏)
/// @param iReSetAIHaveWinMoney 机器人输赢控制：重置机器人已经赢钱的数目
/// @return 是否成功
bool CServerGameDesk::SetRoomPondEx(__int64	iReSetAIHaveWinMoney)
{
	return false;
}

//判断
BOOL CServerGameDesk::Judge()
{
	if(JudgeWiner())		//先处理胜者
	{
		return TRUE;
	}

	if (JudgeLoser())		//处理输者
	{
		return TRUE;
	}		

	return FALSE;
}
//================================================
//判断胜者
//================================================
BOOL CServerGameDesk::JudgeLoser()
{
	return FALSE;
}
//================================================
//判断胜者
//================================================
BOOL CServerGameDesk::JudgeWiner()
{
	return FALSE;
}
//GRM函数
//更新奖池数据(游戏->平台)
/// @return 机器人赢钱数
void CServerGameDesk::UpDataRoomPond(__int64 iAIHaveWinMoney)
{

}