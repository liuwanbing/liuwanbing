#ifndef __IMAIN_MANAGE_I_H__
#define __IMAIN_MANAGE_I_H__
/**
* @ingroup GameSDK
* @brief IGameUserInfoManger
*
* <p>　 
*		IGameUserInfoManger 定义在桌子逻辑里，对IMainManager里的一些方法进行有限的操作
* </p>
* @sa
*/	

#include "ComStruct.h"
#include "gameobject_i.h"

class IMainManger : public MyGame::IGameObject
{
public:

   /**
	*  @brief				取得 comStruct.h 里面定义的 ManageInfoStruct 结构
	*  @return				返回 ManageInfoStruct 结构
	*  @sa 
	*/
	virtual ManageInfoStruct* GetManageInforStr() = 0;

 //  /**
	//*  @brief				设置定时器
	//*  @timeId		        设置定时器ID
	//*  @arg	                定时器的参数
	//*  @intervalTime        定时器的中间间隔时间	
	//*  @return				
	//*  @sa 
	//*/
	//virtual VOID SetTimerEx(UINT tableIndex, UINT timeId, VOID* arg, UINT intervalTime)= 0;

 //  /**
	//*  @brief				删除定时器
	//*  @timeId			    定时器ID
	//*  @return				
	//*  @sa 
	//*/
	//virtual VOID DelTimerEx(UINT tableIndex, UINT timeId)= 0;

 //  /**
	//*  @brief				清除某个用户的内存信息
	//*  @pUserInfo			指向用户信息指针
	//*  @return				是否成功清除，没有不成功的，一般都会成功
	//*  @sa 
	//*/
	//virtual BOOL CleanUserInfoEx(IUserInfor *pUserInfo)= 0;
};

#endif