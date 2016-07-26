
#ifndef MESSAGE_H_
#define MESSAGE_H_
#pragma once

#include <deque>

#include "hge.h"
#include "hgesprite.h"
#include "FontCN/FontSprite.h"

struct MessageInfo {
  bool fade;
  DWORD tick_count;
  wchar_t msg[1024];
};

class Message {
public:
  Message();
  ~Message();

  static bool LoadGameResource();
  bool OnFrame(float delta_time);
  bool OnRender(float hscale, float vscale);

  void AddMessage(wchar_t* msg);

private:
  HGE* hge_;

  FontSprite* message_font_;
  static hgeSprite* spr_message_bg_;
  static DWORD message_bg_color_;
  wchar_t message_info_[1024];

  std::deque<MessageInfo> message_queue_;
};

#endif  // MESSAGE_H_