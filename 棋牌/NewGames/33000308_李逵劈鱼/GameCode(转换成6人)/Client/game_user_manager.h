
#ifndef GAME_USER_MANAGER_H_
#define GAME_USER_MANAGER_H_
#pragma once

#include <vector>

class __declspec(novtable) IGameUserItem {
 public:
  virtual DWORD GetUserID() const = 0;
  virtual DWORD GetGameID() const = 0;
  virtual LPCTSTR GetNickName() const = 0;
  virtual BYTE GetGender() const = 0;
  virtual DWORD GetUserMedal() const = 0;
  virtual BYTE GetMemberOrder() const = 0;
  virtual BYTE GetMasterOrder() const = 0;
  virtual WORD GetTableID() const = 0;
  virtual WORD GetChairID() const = 0;
  virtual BYTE GetUserStatus() const = 0;
  virtual SCORE GetUserScore() const = 0;
  virtual SCORE GetUserInsure() const = 0;
  virtual tagUserInfo* GetUserInfo() = 0;
};

class __declspec(novtable) IGameUserManagerSink {
 public:
  virtual void OnUserItemAcitve(IGameUserItem* game_user_item) = 0;
  virtual void OnUserItemDelete(IGameUserItem* game_user_item) = 0;
  virtual void OnUserItemUpdate(IGameUserItem* game_user_item, tagUserScore& last_score) = 0;
  virtual void OnUserItemUpdate(IGameUserItem* game_user_item, tagUserStatus& last_status) = 0;
};

class GameUserItem;
typedef std::vector<GameUserItem*> GameUserItemVector;

class GameUserManager {
 public:
  GameUserManager();
  ~GameUserManager();

  bool SetGameUserManagerSink(IGameUserManagerSink* game_user_manager_sink);

  bool ResetUserManager();
  bool DeleteUserItem(IGameUserItem* game_user_item);
  IGameUserItem* ActiveUserItem(tagUserInfo& user_info);

  bool UpdateUserItemScore(IGameUserItem* game_user_item, tagUserScore* user_score);
  bool UpdateUserItemStatus(IGameUserItem* game_user_item, tagUserStatus* user_status);

  IGameUserItem* GetTableUserItem(WORD chair_id);
  const GameUserItemVector& GetLookonUserVector() const;

  IGameUserItem* SearchUserItemByUserID(DWORD user_id);
  IGameUserItem* SearchUserItemByGameID(DWORD game_id);
  IGameUserItem* SearchUserItemByNickName(LPCTSTR nick_name);

 private:
  GameUserItem* game_user_item_[MAX_CHAIR];
  GameUserItemVector lookon_user_vector_;
  GameUserItemVector storage_user_vector_;

  IGameUserManagerSink* game_user_manager_sink_;
};

#endif // GAME_USER_MANAGER_H_
