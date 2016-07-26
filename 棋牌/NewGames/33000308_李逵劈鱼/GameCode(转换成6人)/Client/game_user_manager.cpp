
#include "StdAfx.h"
#include "game_user_manager.h"

class GameUserItem : public IGameUserItem 
{
  friend GameUserManager;
 public:
  GameUserItem();
  virtual ~GameUserItem() {}

  virtual DWORD GetUserID() const { return user_info_.dwUserID; }
  virtual DWORD GetGameID() const { return user_info_.dwGameID; }
  virtual LPCTSTR GetNickName() const { return user_info_.szNickName; }
  virtual BYTE GetGender() const { return user_info_.cbGender; }
  virtual DWORD GetUserMedal() const { return user_info_.dwUserMedal; }
  virtual BYTE GetMemberOrder() const { return user_info_.cbMemberOrder; }
  virtual BYTE GetMasterOrder() const { return user_info_.cbMasterOrder; }
  virtual WORD GetTableID() const { return user_info_.wTableID; }
  virtual WORD GetChairID() const { return user_info_.wChairID; }
  virtual BYTE GetUserStatus() const { return user_info_.cbUserStatus; }
  virtual SCORE GetUserScore() const { return user_info_.lScore; }
  virtual SCORE GetUserInsure() const { return user_info_.lInsure; }
  virtual tagUserInfo* GetUserInfo() { return &user_info_; }

private:
  tagUserInfo user_info_;
};

GameUserItem::GameUserItem() {
  memset(&user_info_, 0, sizeof(user_info_));
  user_info_.wTableID = INVALID_TABLE;
  user_info_.wChairID = INVALID_CHAIR;
}

GameUserManager::GameUserManager() : game_user_manager_sink_(NULL) {
  memset(game_user_item_, 0, sizeof(game_user_item_));
}

GameUserManager::~GameUserManager() {
  GameUserItemVector::iterator iter;
  for (iter = lookon_user_vector_.begin(); iter != lookon_user_vector_.end(); ++iter) {
    delete (*iter);
  }
  lookon_user_vector_.clear();
  for (WORD i = 0; i < MAX_CHAIR; ++i) {
    if (game_user_item_[i] != NULL) delete game_user_item_[i];
  }
  for (iter = storage_user_vector_.begin(); iter != storage_user_vector_.end(); ++iter) {
    delete (*iter);
  }
  storage_user_vector_.clear();
}

bool GameUserManager::SetGameUserManagerSink(IGameUserManagerSink* game_user_manager_sink) {
  game_user_manager_sink_ = game_user_manager_sink;
  return true;
}

bool GameUserManager::ResetUserManager() {
  std::copy(lookon_user_vector_.begin(), lookon_user_vector_.end(), std::back_inserter(storage_user_vector_));
  lookon_user_vector_.clear();

  for (WORD i = 0; i < MAX_CHAIR; ++i) {
    if (game_user_item_[i] != NULL) storage_user_vector_.push_back(game_user_item_[i]);
  }
  memset(game_user_item_, 0, sizeof(game_user_item_));
  return true;
}

bool GameUserManager::DeleteUserItem(IGameUserItem* game_user_item) {
  if (game_user_item == NULL || game_user_item->GetChairID() >= MAX_CHAIR) return false;

  if (game_user_item->GetUserStatus() != US_LOOKON) {
    WORD chair_id = game_user_item->GetChairID();
    GameUserItem* game_user_item_remove = static_cast<GameUserItem*>(game_user_item);
    if (game_user_item_[chair_id] != game_user_item) return false;

    game_user_item_[chair_id] = NULL;
    storage_user_vector_.push_back(game_user_item_remove);

    if (game_user_manager_sink_ != NULL) game_user_manager_sink_->OnUserItemDelete(game_user_item);

    memset(&game_user_item_remove->user_info_, 0, sizeof(game_user_item_remove->user_info_));

    return true;
  }

  GameUserItem* game_user_item_active = NULL;
  GameUserItemVector::iterator iter;
  for (iter = lookon_user_vector_.begin(); iter != lookon_user_vector_.end(); ++iter) {
    game_user_item_active = *iter;
    if (game_user_item_active == game_user_item) {
      lookon_user_vector_.erase(iter);
      storage_user_vector_.push_back(game_user_item_active);

      if (game_user_manager_sink_ != NULL) game_user_manager_sink_->OnUserItemDelete(game_user_item);

      memset(&game_user_item_active->user_info_, 0, sizeof(game_user_item_active->user_info_));
      return true;
    }
  }

  return false;
}

IGameUserItem* GameUserManager::ActiveUserItem(tagUserInfo& user_info) {
  if (user_info.wChairID >= MAX_CHAIR) return NULL;

  GameUserItem* game_user_item = NULL;
  if (storage_user_vector_.size() > 0) {
    game_user_item = storage_user_vector_.back();
    storage_user_vector_.pop_back();
  } else {
    game_user_item = new GameUserItem();
    if (game_user_item == NULL) return NULL;
  }

  if (user_info.cbUserStatus == US_LOOKON) lookon_user_vector_.push_back(game_user_item);
  else game_user_item_[user_info.wChairID] = game_user_item;

  memcpy(&game_user_item->user_info_, &user_info, sizeof(game_user_item->user_info_));

  if (game_user_manager_sink_ != NULL) game_user_manager_sink_->OnUserItemAcitve(static_cast<IGameUserItem*>(game_user_item));

  return game_user_item;
}

bool GameUserManager::UpdateUserItemScore(IGameUserItem* game_user_item, tagUserScore* user_score) {
  tagUserInfo* user_info = game_user_item->GetUserInfo();

  tagUserScore last_score;
  last_score.lScore = user_info->lScore;
  last_score.lGrade = user_info->lGrade;
  last_score.lInsure = user_info->lInsure;
  last_score.dwWinCount = user_info->dwWinCount;
  last_score.dwLostCount = user_info->dwLostCount;
  last_score.dwDrawCount = user_info->dwDrawCount;
  last_score.dwFleeCount = user_info->dwFleeCount;
  last_score.dwUserMedal = user_info->dwUserMedal;
  last_score.dwExperience = user_info->dwExperience;
  last_score.lLoveLiness = user_info->lLoveLiness;

  user_info->lScore = user_score->lScore;
  user_info->lGrade = user_score->lGrade;
  user_info->lInsure = user_score->lInsure;
  user_info->dwWinCount = user_score->dwWinCount;
  user_info->dwLostCount = user_score->dwLostCount;
  user_info->dwDrawCount = user_score->dwDrawCount;
  user_info->dwFleeCount = user_score->dwFleeCount;
  user_info->dwUserMedal = user_score->dwUserMedal;
  user_info->dwExperience = user_score->dwExperience;
  user_info->lLoveLiness = user_score->lLoveLiness;

  if (game_user_manager_sink_ != NULL) game_user_manager_sink_->OnUserItemUpdate(game_user_item, last_score);

  return true;
}

bool GameUserManager::UpdateUserItemStatus(IGameUserItem* game_user_item, tagUserStatus* user_status) {
  tagUserInfo* user_info = game_user_item->GetUserInfo();

  tagUserStatus last_status;
  last_status.wTableID = user_info->wTableID;
  last_status.wChairID = user_info->wChairID;
  last_status.cbUserStatus = user_info->cbUserStatus;

  user_info->wTableID = user_status->wTableID;
  user_info->wChairID = user_status->wChairID;
  user_info->cbUserStatus = user_status->cbUserStatus;

  if (game_user_manager_sink_ != NULL) game_user_manager_sink_->OnUserItemUpdate(game_user_item, last_status);

  return true;
}

IGameUserItem* GameUserManager::GetTableUserItem(WORD chair_id) {
  if (chair_id >= arraysize(game_user_item_)) return NULL;

  return game_user_item_[chair_id];
}

const GameUserItemVector& GameUserManager::GetLookonUserVector() const {
  return lookon_user_vector_;
}

IGameUserItem* GameUserManager::SearchUserItemByUserID(DWORD user_id) {
  GameUserItem* game_user_item;
  for (WORD i = 0; i < MAX_CHAIR; ++i) {
    game_user_item = game_user_item_[i];
    if (game_user_item != NULL && game_user_item->GetUserID() == user_id) return game_user_item;
  }

  GameUserItemVector::iterator iter;
  for (iter = lookon_user_vector_.begin(); iter != lookon_user_vector_.end(); ++iter) {
    game_user_item = *iter;
    if (game_user_item->GetUserID() == user_id) return game_user_item;
  }

  return NULL;
}

IGameUserItem* GameUserManager::SearchUserItemByGameID(DWORD game_id) {
  GameUserItem* game_user_item;
  for (WORD i = 0; i < MAX_CHAIR; ++i) {
    game_user_item = game_user_item_[i];
    if (game_user_item != NULL && game_user_item->GetGameID() == game_id) return game_user_item;
  }

  GameUserItemVector::iterator iter;
  for (iter = lookon_user_vector_.begin(); iter != lookon_user_vector_.end(); ++iter) {
    game_user_item = *iter;
    if (game_user_item->GetGameID() == game_id) return game_user_item;
  }

  return NULL;
}

IGameUserItem* GameUserManager::SearchUserItemByNickName(LPCTSTR nick_name) {
  GameUserItem* game_user_item;
  for (WORD i = 0; i < MAX_CHAIR; ++i) {
    game_user_item = game_user_item_[i];
    if (game_user_item != NULL && !lstrcmp(nick_name, game_user_item->GetNickName())) return game_user_item;
  }

  GameUserItemVector::iterator iter;
  for (iter = lookon_user_vector_.begin(); iter != lookon_user_vector_.end(); ++iter) {
    game_user_item = *iter;
    if (!lstrcmp(nick_name, game_user_item->GetNickName())) return game_user_item;
  }

  return NULL;
}
