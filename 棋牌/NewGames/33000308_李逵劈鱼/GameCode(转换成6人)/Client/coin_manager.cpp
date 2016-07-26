
#include "StdAfx.h"
#include "coin_manager.h"
#include "math_aide.h"
#include "game_manager.h"
#include "hgeresource.h"

const float kCoinMoveInterval = 10.f;

Coin::Coin() : angle_(0.f), trace_index_(0) {
  hgeResourceManager* resource_manager = GameManager::GetInstance().GetResourceManager();
  //ani_coin_ = resource_manager->GetAnimation("coin1");
  ani_coin_ = new hgeAnimation(*resource_manager->GetAnimation("coin2"));
  ani_coin_->Play();
}

Coin::~Coin() {
  SAFE_DELETE(ani_coin_);
}

bool Coin::OnFrame(float delta_time) {
  if (trace_index_ >= trace_vector_.size()) return true;

  ++trace_index_;
  if (trace_index_ >= trace_vector_.size()) return true;
  ani_coin_->Update(delta_time);

  return false;
}

bool Coin::OnRender(float offset_x, float offset_y, float hscale, float vscale) {
  if (trace_index_ >= trace_vector_.size()) return true;

  FPoint& coin_pos = trace_vector_[trace_index_];
  ani_coin_->RenderEx(offset_x + coin_pos.x, offset_y + coin_pos.y, angle_, hscale, vscale);
  
  return false;
}

CoinManager::CoinManager() {
  hge_ = hgeCreate(HGE_VERSION);
}

CoinManager::~CoinManager() {
  std::vector<Coin*>::iterator iter;
  for (iter = coin_vector_.begin(); iter != coin_vector_.end(); ++iter) {
    SAFE_DELETE(*iter);
  }
  coin_vector_.clear();

  hge_->Release();
}

bool CoinManager::LoadGameResource() {
  hgeResourceManager* resource_manager = GameManager::GetInstance().GetResourceManager();
  ani_coin1_ = resource_manager->GetAnimation("coin1");
  ani_coin2_ = resource_manager->GetAnimation("coin2");

  return true;
}

bool CoinManager::OnFrame(float delta_time) {
  std::vector<Coin*>::iterator iter;
  Coin* coin = NULL;
  for (iter = coin_vector_.begin(); iter != coin_vector_.end();) {
    coin = *iter;
    if (coin->OnFrame(delta_time)) {
      iter = coin_vector_.erase(iter);
      SAFE_DELETE(coin);
    } else {
      ++iter;
    }
  }

  return false;
}

bool CoinManager::OnRender(float offset_x, float offset_y, float hscale, float vscale) {
  std::vector<Coin*>::iterator iter;
  Coin* coin = NULL;
  for (iter = coin_vector_.begin(); iter != coin_vector_.end(); ++iter) {
    coin = *iter;
    coin->OnRender(offset_x, offset_y, hscale, vscale);
  }

  return false;
}

void CoinManager::BuildCoin(const FPointAngle& fish_pos, const FPoint& cannon_pos, WORD chair_id, SCORE score) {
  static const int kCoinCountEnum[] = { 0, 100, 1000, 10000, 100000 };
  static const int kCointCount[] = { 1, 2, 3, 4, 5 };
  static float screen_width = static_cast<float>(hge_->System_GetState(HGE_SCREENWIDTH));
  static float scale = screen_width / kResolutionWidth;
  static float coin_width = ani_coin2_->GetWidth() * scale;

  int coin_count = 0;
  for (int i = arraysize(kCoinCountEnum) - 1; i >= 0; --i) {
    if (score >= kCoinCountEnum[i]) {
      if (i == arraysize(kCoinCountEnum) - 1) {
        coin_count = kCointCount[i] + int((score - kCoinCountEnum[i]) / 10000);
        coin_count = min(20, coin_count);
      } else {
        coin_count = kCointCount[i];
      }
      break;
    }
  }

  float radius = (coin_count - 1) * coin_width / 2;
  FPoint center = { fish_pos.x, fish_pos.y };
  if (coin_count > 10) {
    center.x = fish_pos.x + coin_width * cosf(fish_pos.angle + M_PI_2);
    center.y = fish_pos.y + coin_width * sinf(fish_pos.angle + M_PI_2);
    radius = (10 - 1) * coin_width / 2;
  }

  float x_pos[2] = { fish_pos.x, cannon_pos.x };
  float y_pos[2] = { fish_pos.y, cannon_pos.y };

  for (int i = 0; i < coin_count; ++i) {
    if (radius < 0.f) {
      x_pos[0] = center.x + radius * cosf(fish_pos.angle);
      y_pos[0] = center.y + radius * sinf(fish_pos.angle);
    } else {
      x_pos[0] = center.x - radius * cosf(fish_pos.angle - M_PI);
      y_pos[0] = center.y - radius * sinf(fish_pos.angle - M_PI);
    }
    Coin* coin = new Coin();
    MathAide::BuildLinear(x_pos, y_pos, 2, coin->trace_vector(), kCoinMoveInterval * scale);
    coin->set_angle(fish_pos.angle);
    coin_vector_.push_back(coin);
    radius -= coin_width;
    if (i == 10) {
      center.x = fish_pos.x + coin_width * cosf(fish_pos.angle - M_PI_2);
      center.y = fish_pos.y + coin_width * sinf(fish_pos.angle - M_PI_2);
      radius = (coin_count - 10 - 1) * coin_width / 2;
    }
  }
}

bool CoinManager::FreeCoin(Coin* coin) {
  std::vector<Coin*>::iterator iter;
  for (iter = coin_vector_.begin(); iter != coin_vector_.end(); ++iter) {
    if (coin == *iter) {
      coin_vector_.erase(iter);
      SAFE_DELETE(coin);
      return true;
    }
  }

  assert(!"FreeCoin Failed");
  return false;
}

void CoinManager::FreeAllCoins() {
  std::vector<Coin*>::iterator iter;
  for (iter = coin_vector_.begin(); iter != coin_vector_.end(); ++iter) {
    SAFE_DELETE(*iter);
  }
  coin_vector_.clear();
}