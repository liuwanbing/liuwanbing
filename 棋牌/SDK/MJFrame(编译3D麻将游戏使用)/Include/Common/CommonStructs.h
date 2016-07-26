
#ifndef __COMMON_STRUCTS_H__
#define __COMMON_STRUCTS_H__


#include "typeDef.h"

#include "playerManagerCommon.h"

namespace MyGame
{

	//以下定义踢人的原因值, 其中0 ~ 999为框架使用的空间, 1000 ~ 32767 为游戏使用的空间
	enum KICK_REASONS
	{
		kick_upgrade = 0,			        //由于系统升级，请玩家暂时离开
		kick_multilogin = 1,		        //重复登陆通知使用的原因值
		kick_dull		= 2,		        //由于玩家发呆被请求离开
		kick_clear		= 3,	        	//竞技活动专用，清除桌子
		
		kick_clear_in_dogfight_room = 4,    //混战房间专用，清除桌子

		kick_offline = 1000,	        	//由于玩家掉线，游戏请玩家离开
		kick_nomoney = 1001,		        //由于玩家财富值不够，游戏请玩家离开
		kick_notallowholdon = 1004,	        //
		kick_bekicked_byuser = 1010,        //被玩家T
		kick_fail_lock_money = 1011,	    //锁定游戏币失败
		kick_fail_lock_happybean = 1012,	//锁定欢乐豆失败
		kick_more_than_Upper     = 1013,	//超出上限
		kick_less_than_ticket    = 1014,    //少于门票
		kick_less_than_lower     = 1015,	//低于下限
		kick_game_has_start      = 1016,	//游戏已经开始

		Kick_Vip_kick 			= 1017,     //vip 踢人
		Kick_gm_kick			= 1018,	    //GM 踢人
		kick_user_normal_exit	 = 1020		//玩家正常请求退出，怕导致后来的游戏开始，一定要先kick
    };

	struct TCMD_HEADER
	{
		UINT    cCmdID;         // 消息标识
		UINT    cCmdPara;
		UINT    nDataLen;       // 整个消息的长度
	};
	

}


#endif

