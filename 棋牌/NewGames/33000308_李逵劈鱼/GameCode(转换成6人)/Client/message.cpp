
#include "stdafx.h"
#include "message.h"
#include "game_manager.h"

const DWORD kMsgShowTime = 2000;
const int kFadeFactor = 10;
const DWORD kDefaultColor = 0xFFFFFFFF;

hgeSprite* Message::spr_message_bg_ = NULL;
DWORD Message::message_bg_color_ = 0xFFFFFFFF;

Message::Message() : hge_(hgeCreate(HGE_VERSION)) {
  char font_dir[MAX_PATH] = { 0 };
  GetWindowsDirectoryA(font_dir, arraysize(font_dir));
  strcat(font_dir, "\\fonts\\SIMSUN.TTC");
  message_font_ = FontSprite::CreateEx(font_dir, 30);
  message_font_->SetColor(kDefaultColor);

  message_info_[0] = 0;
}

Message::~Message() {
  message_queue_.clear();
  message_font_->Release();
  hge_->Release();
}

bool Message::LoadGameResource() {
  hgeResourceManager* resource_manager = GameManager::GetInstance().GetResourceManager();
  spr_message_bg_ = resource_manager->GetSprite("message_bg");
  message_bg_color_ = spr_message_bg_->GetColor();
  return spr_message_bg_ != NULL;
}

bool Message::OnFrame(float delta_time) {
  if (message_queue_.size() == 0) return false;

  DWORD now_tick = timeGetTime();
  MessageInfo& msg_info = message_queue_.front();
  if (msg_info.tick_count + kMsgShowTime < now_tick) {
    msg_info.fade = true;
  }
  if (msg_info.fade) {
    DWORD color = message_font_->GetColor();
    DWORD color_bg = spr_message_bg_->GetColor();
    int alpha = GETA(color);
    int alpha_bg = GETA(color_bg);
    alpha -= kFadeFactor;
    alpha_bg -= kFadeFactor;
    if (alpha <= 5) {
      message_queue_.pop_front();
      message_font_->SetColor(kDefaultColor);
      spr_message_bg_->SetColor(message_bg_color_);
      if (message_queue_.size() > 0) {
        MessageInfo& msg_info = message_queue_.front();
        msg_info.tick_count = timeGetTime();
      }
    } else {
      color = SETA(color, alpha);
      color_bg = SETA(color_bg, color_bg);
      message_font_->SetColor(color);
      spr_message_bg_->SetColor(color_bg);
    }
  }

  return false;
}

bool Message::OnRender(float hscale, float vscale) {
  if (message_queue_.size() == 0) return false;

  float screen_width = static_cast<float>(hge_->System_GetState(HGE_SCREENWIDTH));
  float screen_height = static_cast<float>(hge_->System_GetState(HGE_SCREENHEIGHT));

  MessageInfo& msg_info = message_queue_.front();
  spr_message_bg_->RenderEx(screen_width / 2, screen_height / 2, 0.f, hscale, vscale);
  _snwprintf(message_info_, arraysize(message_info_), L"系统消息:%s", msg_info.msg);
  SIZE size = message_font_->GetTextSize(message_info_);
  message_font_->RenderEx((screen_width - size.cx * hscale) / 2, (screen_height - size.cy * hscale) / 2, message_info_, hscale);

  return false;
}

void Message::AddMessage(wchar_t* msg) {
  MessageInfo msg_info;
  memset(&msg_info, 0, sizeof(msg_info));
  if (message_queue_.size() == 0) msg_info.tick_count = timeGetTime();
  wcsncpy(msg_info.msg, msg, arraysize(msg_info.msg));
  message_queue_.push_back(msg_info);
}