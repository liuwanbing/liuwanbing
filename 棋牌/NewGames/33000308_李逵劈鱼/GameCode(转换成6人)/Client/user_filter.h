
#ifndef USER_FILTER_H_
#define USER_FILTER_H_
#pragma once

#include "hge.h"
#include "guichan.hpp"

class UserFilter : public gcn::ActionListener
{
 public:
  UserFilter();
  virtual ~UserFilter();

  bool Initialize();

  void ShowUserFilter(bool show);

 private:
  void action(const gcn::ActionEvent& actionEvent);

 private:
  HGE* hge_;
  gcn::Window* filter_window_;
  gcn::RadioButton* black_list_;
  gcn::RadioButton* white_list_;
  gcn::RadioButton* cancel_list_;
  gcn::RadioButton* fish20_config_;
  gcn::Label* input_tips_;
  gcn::TextField* tex_field_;
  gcn::Label* fish20_probability_tips_;
  gcn::TextField* fish20_probability_;
  gcn::Label* fish20_capture_count_tips_;
  gcn::TextField* fish20_capture_count_;
  gcn::Button* ok_;
  gcn::Button* cancel_;
};

#endif  // USER_FILTER_H_
