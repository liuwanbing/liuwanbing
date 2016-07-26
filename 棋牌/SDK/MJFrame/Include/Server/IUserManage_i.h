#ifndef __IUSER_MANAGE_I_H__
#define __IUSER_MANAGE_I_H__
#include "GameObject_i.h"
/**
* @ingroup GameSDK
* @brief IGameUserInfoManger
*
* <p>　 
*		IGameUserInfoManger 此文件定义给，在SerLib里桌子对主模块里的用户进行有限的操作
*							如找到某个用户呀，等等
* </p>
* @sa
*/	

class CGameUserInfo;

class IGameUserInfoManger : public MyGame::IGameObject
{
public:
   /**
	*  @brief				找到现在已断线的用户
	*  @dwUserID			输入用户ID
	*  @return				返回单个用户信息
	*  @sa 
	*/
	virtual CGameUserInfo * FindNetCutUser(int dwUserID) = 0;

   /**
	*  @brief				找到现在已断线的用户
	*  @dwUserID			输入用户名
	*  @return				返回单个用户信息结构
	*  @sa 
	*/
	virtual CGameUserInfo * FindNetCutUser(char * szName) = 0;

   /**
	*  @brief				找到现在已在线的用户
	*  @dwUserID			输入用户名
	*  @return				返回单个用户信息结构
	*  @sa 
	*/
	virtual CGameUserInfo * FindOnLineUser(char * szName) = 0;

   /**
	*  @brief				找到现在已在线的用户
	*  @dwUserID			输入用户ID
	*  @return				返回单个用户信息结构
	*  @sa 
	*/
	virtual CGameUserInfo * FindOnLineUser(int dwUserID) = 0;

   /**
	*  @brief				找到现在已在线的用户
	*  @dwUserID			输入用户ID
	*  @return				返回单个用户信息结构
	*  @sa 
	*/
	virtual CGameUserInfo * GetOnLineUserInfo(UINT uIndex) = 0;

   /**
	*  @brief				释放现在一个用户，从断线用户，与现在的用户里都这样
	*  @pGameUserInfo		输入现在放掉的用户指针
	*  @return				返回成功与失败
	*  @sa 
	*/
	virtual BOOL ClearUser(CGameUserInfo * pGameUserInfo) = 0;
};

#endif