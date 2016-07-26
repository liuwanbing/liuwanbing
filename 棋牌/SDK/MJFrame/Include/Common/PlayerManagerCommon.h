#if !defined(PLAYERMANAGER_COMMON_H)
#define PLAYERMANAGER_COMMON_H


namespace MyGame
{

#ifndef IPC_VERSION
//////////////////////////////////////////////////////////////////////////
//简体部分：begin.

	#define MAX_NICK_NAME_LENGTH		20

//简体部分：end.
//////////////////////////////////////////////////////////////////////////

#else //define IPC_VERSION

//////////////////////////////////////////////////////////////////////////
//IPC部分：begin.

	#define MAX_NICK_NAME_LENGTH		30

//IPC部分：end.
//////////////////////////////////////////////////////////////////////////

#endif
	#define MAX_NUMBER_PLAYER	10
	#define MAX_NUMBER_WATCHER	16

	//玩家的组队信息
	typedef enum enmPlayerTeam
	{
		EPT_TEAM_NULL,
		//玩家为自由人的时候使用 EPT_TEAM_0到EPT_TEAM_9对应于大厅的座位号
		EPT_TEAM_0,
		EPT_TEAM_1,
		EPT_TEAM_2,
		EPT_TEAM_3,
		EPT_TEAM_4,
		EPT_TEAM_5,
		EPT_TEAM_6,
		EPT_TEAM_7,
		EPT_TEAM_8,
		EPT_TEAM_9,
		//玩家选择队伍后使用的队伍标识
		EPT_TEAM_A,
		EPT_TEAM_B,
		EPT_TEAM_C,
		EPT_TEAM_D,
		EPT_TEAM_E,
		EPT_TEAM_F,
		EPT_TEAM_G,
		EPT_TEAM_H,
		EPT_TEAM_I,
		EPT_TEAM_END
	}ENM_PLAYER_TEAM;

	typedef enum enmUserStatus
	{
		EUS_GETOUT     = 0, // 0,离开了
		EUS_FREE       = 1, // 1,在房间站立
		EUS_SITDOWN    = 2, // 2,坐在椅子上，没按开始
		EUS_READY      = 3, // 3,同意游戏开始
		EUS_PLAYING    = 4, // 4,正在玩
		EUS_OFFLINE    = 5, // 5,断线等待续玩
		EUS_SPECTATING = 6, // 6,旁观
	} ENMUSERSTATUS;
	

	//用户身份
	typedef enum enmUserIdentifier
	{
		EUI_PLAYER_ADMIN = 0x40, 
		EUI_VIP_PLAYER   = 0x80
	}ENM_USER_IDENTIFIER;
	
	//!游戏房间的类型
	typedef enum enmRoomType
	{
		ERT_NORMAL   = 0,		/*!< 普通场*/
		ERT_MONEY,				/*!< 游戏币场*/
		ERT_HAPPYBEAN			/*!< 欢乐豆场*/
	}ENM_ROOM_TYPE;
}
#endif

