#ifndef __IUSER_INFOR_I_H__
#define __IUSER_INFOR_I_H__

#include "typeDef.h"
/**
* @ingroup GameSDK
* @brief IUserInfor
*
* <p>　 
*		IUserInfor 用户信息接口
* </p>
* @sa
*/	

class IUserInfor
{
public:
   /**
    *  @brief               取得用户ID
    *  @return              返回用户ID
    *  @sa 
    */
	virtual UINT GetUserID(void) = 0;
};

#endif