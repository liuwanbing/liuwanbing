#ifndef __USER_INFOR_ITEM_H__
#define __USER_INFOR_ITEM_H__

#include "afxwin.h"
#include "..\UserInfo.h"
#include "IUserInfor_i.h"
#include "SoUserManager_i.h"
/*******************************************************************************************************/


/*******************************************************************************************************/
///用户资料类
class  CGameUserInfoEx : public CGameUserInfo, public IUserInfor
{
    ///变量定义
    MyGame::ISoPlayer  *m_pSoPlayer;  /// 指向用户向外暴的接口类                                  
};

/*********************************************************************************************************************/

#endif