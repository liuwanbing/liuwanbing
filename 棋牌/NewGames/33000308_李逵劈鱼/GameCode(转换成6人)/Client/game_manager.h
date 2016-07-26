
#ifndef GAME_MANAGER_H_
#define GAME_MANAGER_H_
#pragma once

#include <vector>

//#include "client_kernel.h"

#include "hge.h"
#include "hgesprite.h"
#include "hgeanim.h"
#include "hgeresource.h"
#include "FontCN/FontSprite.h"
#include "game_user_manager.h"

class GameScene;
class Fish;
class FishManager;
class CannonManager;
class BulletManager;
class CoinManager;
class JettonManager;
class Message;
class Bingo;
class SoundSetting;
class LockFishManager;
class UserFilter;

struct FishTraceInfo {
  float init_x_pos[5];
  float init_y_pos[5];
  int init_count;

  Fish* fish;
};

class GameManager //: public IClientKernelSink
{
 public:
  virtual ~GameManager();

  static GameManager& GetInstance();
  static GameManager* GetpGameManager();
  static void DelInstance();

  virtual void ResetGameClient();
  virtual void CloseGameClient();
  virtual bool OnEventSocket(WORD main_cmdid, WORD sub_cmdid, void* data, WORD data_size);
  virtual bool OnEventSceneMessage(BYTE game_status, bool lookon_user, void* data, WORD data_size);
  virtual void OnEventUserEnter(IGameUserItem* game_user_item, bool lookon_user);
  virtual void OnEventUserLeave(IGameUserItem* game_user_item, bool lookon_user);
  virtual void OnEventUserScore(IGameUserItem* game_user_item, bool lookon_user);
  virtual void OnEventUserStatus(IGameUserItem* game_user_item, bool lookon_user);
  virtual void OnSystemMessage(wchar_t sys_msg[], WORD msg_len, WORD type);

 // CNewGameClient* GetClientKernel() const;
  hgeResourceManager* GetResourceManager() const;
  FishManager* GetFishManager() const;
  CoinManager* GetCoinManager() const;
  CannonManager* GetCannonManager() const;
  LockFishManager* GetLockFishManager() const;

  bool LoadGameResource(int& progress);
  bool InitClientKernel(LPTSTR cmd_line);
  bool OnFrame(float delta_time);
  bool OnRender(float hscale, float vscale);

  // 框架移动
  void HandleFrameMoveEvent();

  int current_bullet_mulriple() const { return current_bullet_mulriple_; }
  float bomb_range_width() const { return bomb_range_width_; }
  float bomb_range_height() const { return bomb_range_height_; }
  void ShakeScreen();
  void AllowFire(bool allow) { allow_fire_ = allow; }
  bool lock() const { return lock_; }
  void set_lock(bool lock) { lock_ = lock; }
  void set_game_active(bool active) { game_active_ = active; }
  int max_bullet_multiple() const { return max_bullet_multiple_; }

 public:
  void SendExchangeFishScore(bool increase);
  void SendUserFire(BulletKind bullet_kind, float angle, int bullet_mulriple, int lock_fishid);
  void SendCatchFish(int fish_id, WORD firer_chair_id, int bullet_id, BulletKind bullet_kind, int bullet_mulriple);
  void SendCatchSweepFish(CMD_C_CatchSweepFish* catch_sweep_fish);
  void SendHitFishLK(WORD firer_chair_id, int fish_id);
  void SendStockOperate(unsigned char operate_code);
  void SendUserFilter(DWORD game_id, unsigned char operate_code);
  void SendFish20Config(DWORD game_id, int catch_count, double catch_probability);
  //sdp 判断是否资源加载完毕
  bool	GetGameBeReady(){return game_ready_;}
 private:
  bool SendSocketData(WORD sub_cmdid);
  bool SendSocketData(WORD sub_cmdid, void* data, WORD data_size);

 private:
  bool OnSubSystem_messInfo(void* data, WORD data_size);//接收系统消息
  bool OnSubGameConfig(void* data, WORD data_size);
  bool OnSubFishTrace(void* data, WORD data_size);
  bool OnSubExchangeFishScore(void* data, WORD data_size);
  bool OnSubUserFire(void* data, WORD data_size);
  bool OnSubCatchFish(void* data, WORD data_size);
  bool OnSubBulletIonTimeout(void* data, WORD data_size);
  bool OnSubLockTimeout(void* data, WORD data_size);
  bool OnSubCatSweepFish(void* data, WORD data_size);
  bool OnSubCatSweepFishResult(void* data, WORD data_size);
  bool OnSubHitFishLK(void* data, WORD data_size);
  bool OnSubSwitchScene(void* data, WORD data_size);
  bool OnSubStockOperateResult(void* data, WORD data_size);
  bool OnSubSceneEnd(void* data, WORD data_size);

 private:
  bool CanFire(WORD chair_id, FPoint& mouse_pos);

 private:
  GameManager();
  GameManager(const GameManager&);
  void operator=(const GameManager&);

  static unsigned __stdcall CalcTraceThread(void* param);

  void UpdateShakeScreen(float delta_time);

  void RenderUserScoreNum(SCORE num, float x, float y, float rot, float hscale, float vscale);

 private:
  static GameManager* game_manager_;
  //ClientKernel* client_kernel_;

  HGE* hge_;
  hgeResourceManager* resource_manager_;

  GameScene* game_scene_;
  FishManager* fish_manager_;
  CannonManager* cannon_manager_;
  BulletManager* bullet_manager_;
  CoinManager* coin_manager_;
  JettonManager* jetton_manager_;
  Message* message_;
  Bingo* bingo_;
  FontSprite* stock_font_;
  FontSprite* self_info_font_;
  hgeSprite* spr_self_info_;
  hgeAnimation* spr_user_score_num_;
  hgeSprite* spr_cursor_;
  SoundSetting* sound_setting_;
  LockFishManager* lock_fish_manager_;
  UserFilter* user_filter_;
  hgeSprite* spr_help_tip_;
  bool show_help_tip_;
  WORD show_user_info_chairid_;

  std::vector<FishTraceInfo> fish_trace_info_buffer_;
  std::vector<FishTraceInfo> fish_trace_info_calc_buffer_;

  HANDLE calc_trace_thread_;
  HANDLE calc_trace_event_;
  CRITICAL_SECTION critial_section_;
  volatile bool exit_game_;

  bool game_ready_;
  bool game_active_;

  FPoint offset_;
  float since_last_frame_;
  int current_shake_frame_;
  bool shake_screen_;

  int exchange_ratio_userscore_;
  int exchange_ratio_fishscore_;
  int exchange_count_;

  int min_bullet_multiple_;
  int max_bullet_multiple_;

  float bomb_range_width_;
  float bomb_range_height_;

  int fish_multiple_[FISH_KIND_COUNT];
  float fish_speed_[FISH_KIND_COUNT];
  float fish_bounding_box_width_[FISH_KIND_COUNT];
  float fish_bounding_box_height_[FISH_KIND_COUNT];
  float fish_hit_radius_[FISH_KIND_COUNT];

  float bullet_speed_[BULLET_KIND_COUNT];
  float net_radius_[BULLET_KIND_COUNT];

  BulletKind current_bullet_kind_;
  int current_bullet_mulriple_;
  bool allow_fire_;
  bool lock_;

  SCORE exchange_fish_score_[GAME_PLAYER];

  wchar_t stock_message_[64];

  DWORD last_fire_time_;//最后一次开火时间
  SceneKind last_scene_kind_;

  int m_iKeyTag;//点击键盘持续记录
};

#endif // GAME_MANAGER_H_
