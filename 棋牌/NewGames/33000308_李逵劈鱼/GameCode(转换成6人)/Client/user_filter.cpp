
#include "StdAfx.h"
#include "user_filter.h"
#include "gui_manager.h"
#include "game_manager.h"

UserFilter::UserFilter()
  : hge_(hgeCreate(HGE_VERSION)),
    filter_window_(NULL),
    black_list_(NULL),
    white_list_(NULL),
    cancel_list_(NULL),
    fish20_config_(NULL),
    input_tips_(NULL),
    tex_field_(NULL),
    fish20_probability_tips_(NULL),
    fish20_probability_(NULL),
    fish20_capture_count_tips_(NULL),
    fish20_capture_count_(NULL),
    ok_(NULL),
    cancel_(NULL) {
}

UserFilter::~UserFilter() {
  SAFE_DELETE(filter_window_);
  SAFE_DELETE(black_list_);
  SAFE_DELETE(white_list_);
  SAFE_DELETE(cancel_list_);
  SAFE_DELETE(fish20_config_);
  SAFE_DELETE(input_tips_);
  SAFE_DELETE(tex_field_);
  SAFE_DELETE(fish20_probability_tips_);
  SAFE_DELETE(fish20_probability_);
  SAFE_DELETE(fish20_probability_);
  SAFE_DELETE(fish20_capture_count_);
  SAFE_DELETE(ok_);
  SAFE_DELETE(cancel_);
  hge_->Release();
}

//class FishListModel : public gcn::ListModel {
// public:
//  int getNumberOfElements() {
//    return FISH_KIND_COUNT;
//  }
//
//  std::string getElementAt(int i) {
//    if (i < FISH_KIND_COUNT) {
//      std::string str_ret("fish");
//      char temp[32] = { 0 };
//      _snprintf(temp, 31, "%d", i + 1);
//      str_ret += temp;
//      return str_ret;
//    }
//    return std::string("");
//  }
//};
//
//FishListModel fish_list_model;

bool UserFilter::Initialize() {
  gcn::Container* top = static_cast<gcn::Container*>(GuiManager::GetInstance().GetGui()->getTop());

  filter_window_ = new gcn::Window("黑名单白名单设置");
  filter_window_->setWidth(400);
  filter_window_->setHeight(300);
  filter_window_->setPosition((hge_->System_GetState(HGE_SCREENWIDTH) - filter_window_->getWidth()) / 2,
    (hge_->System_GetState(HGE_SCREENHEIGHT) - filter_window_->getHeight()) / 2);
  filter_window_->setVisible(false);

  black_list_ = new gcn::RadioButton("黑名单", "1", true);
  black_list_->adjustSize();
  black_list_->setPosition(50, 20);
  filter_window_->add(black_list_);

  white_list_ = new gcn::RadioButton("白名单", "1");
  white_list_->adjustSize();
  white_list_->setPosition(150, 20);
  filter_window_->add(white_list_);

  cancel_list_ = new gcn::RadioButton("从黑白名单删除", "1");
  cancel_list_->adjustSize();
  cancel_list_->setPosition(250, 20);
  filter_window_->add(cancel_list_);

  fish20_config_ = new gcn::RadioButton("企鹅设置", "1");
  fish20_config_->adjustSize();
  fish20_config_->setPosition(50, 40);
  filter_window_->add(fish20_config_);

  input_tips_ = new gcn::Label("在下面的输入框中输入GameID：");
  input_tips_->setSize(250, 20);
  input_tips_->setPosition(50, 60);
  filter_window_->add(input_tips_);

  tex_field_ = new gcn::TextField();
  tex_field_->setSize(200, 22);
  tex_field_->setPosition(50, 100);
  filter_window_->add(tex_field_);

  fish20_probability_tips_ = new gcn::Label("企鹅的捕获概率(0-1)：");
  fish20_probability_tips_->setSize(220, 20);
  fish20_probability_tips_->setPosition(50, 135);
  filter_window_->add(fish20_probability_tips_);

  fish20_probability_ = new gcn::TextField();
  fish20_probability_->setSize(80, 22);
  fish20_probability_->setPosition(180, 135);
  filter_window_->add(fish20_probability_);

  fish20_capture_count_tips_ = new gcn::Label("企鹅的捕获数量：");
  fish20_capture_count_tips_->setSize(220, 20);
  fish20_capture_count_tips_->setPosition(50, 155);
  filter_window_->add(fish20_capture_count_tips_);

  fish20_capture_count_ = new gcn::TextField();
  fish20_capture_count_->setSize(80, 22);
  fish20_capture_count_->setPosition(180, 155);
  filter_window_->add(fish20_capture_count_);

  ok_ = new gcn::Button("确定");
  ok_->setSize(50, 20);
  ok_->setPosition(100, 200);
  ok_->addActionListener(this);
  filter_window_->add(ok_);

  cancel_ = new gcn::Button("取消");
  cancel_->setSize(50, 20);
  cancel_->setPosition(200, 200);
  cancel_->addActionListener(this);
  filter_window_->add(cancel_);

  top->add(filter_window_);

  return true;
}

void UserFilter::ShowUserFilter(bool show) {
  filter_window_->setVisible(show);
  hge_->System_SetState(HGE_HIDEMOUSE, !show);
}

void UserFilter::action(const gcn::ActionEvent& actionEvent) {
  if (actionEvent.getSource() == ok_) {
    if (fish20_config_->isSelected()) {
      do {
        std::string text = tex_field_->getText();
        if (text.empty()) break;

        std::string count = fish20_capture_count_->getText();
        if (count.empty()) break;

        std::string probability = fish20_probability_->getText();
        if (probability.empty()) break;

        DWORD game_id = strtoul(text.c_str(), NULL, 10);
        int num = strtol(count.c_str(), NULL, 10);
        double pro = strtod(probability.c_str(), NULL);
        GameManager::GetInstance().SendFish20Config(game_id, num, pro);
      } while(0);
    } else {
      unsigned char operate_code = 0;
      if (white_list_->isSelected()) {
        operate_code = 1;
      } else if (cancel_list_->isSelected()) {
        operate_code = 2;
      }
      std::string text = tex_field_->getText();
      if (!text.empty()) {
        DWORD game_id = strtoul(text.c_str(), NULL, 10);
        GameManager::GetInstance().SendUserFilter(game_id, operate_code);
      }
    }
    ShowUserFilter(false);
  } else if (actionEvent.getSource() == cancel_) {
    ShowUserFilter(false);
  }
}
