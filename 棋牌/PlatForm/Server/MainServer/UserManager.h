#pragma once

#include <vector>
#include <map>
#include <list>
using namespace std;

typedef struct user
{
	int iSocketIndex;		///< Socket端口序号
	int UserID;				///< 玩家ID
	int iAddFriendType;
	bool bIsGameManager;	///< 是否游戏管理员
	bool bIsValid;
	char UserName[50];
	user() {ZeroMemory(this, sizeof(user));}
}USERS, *LPUSER;


class CUserManager
{
public:
	CUserManager(void);
public:
	~CUserManager(void);
public:
	vector<USERS>	m_dataPloor;		//相当于内存池一样
	map<int, LPUSER> m_Users;			//存储用户数据
	int				m_iSize;
public:
	void Add(int index, LPUSER data);
	void Del(int index);
	int Find(int index);
};
