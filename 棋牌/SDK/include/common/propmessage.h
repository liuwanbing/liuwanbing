#pragma once

#ifndef BZGAME_PROP_MESSAGE_HEAD_FILE
#define BZGAME_PROP_MESSAGE_HEAD_FILE


/*
文件说明：本文件定义了与IM道具相关的消息和结构体
整合平台时，需要在各项目stdafx.h中包含本文件
文件创建：Fred Huang 2008-08-08
消息格式：MDM_		表示主消息
	ASS_PROP	表示与IM相关子消息
	ASS_PROP	表示由客户端向服务器端发送的消息
	ASS_PROP	表示由服务器端向客户端发送的消息
结构格式：MSG_		表示是消息结构体
	MSG_PROP	与头像相关的消息结构
	MSG_PROP_C_ 由客户端发给服务器端的消息结构体
	MSG_PROP_S_ 由服务器端发给客户端的消息结构体
*/

//主消息
#define MDM_GP_PROP						140					//大厅中道具相关消息
#define MDM_GR_PROP						160					//房间和游戏中道具相关的消息

#define BRD_MSG_BIG						0x1					//大喇叭
#define BRD_MSG_SMALL					0x2					//小喇叭


//大厅子消息
#define ASS_PROP_GETUSERPROP			0x01				//取得服务器上存储的个人道具
#define ASS_PROP_GETPROPINFO			0x02				//取得某个道具的信息
#define ASS_PROP_BUY					0x03				//购买道具
#define ASS_PROP_GIVE					0x04				//赠送道具
#define ASS_PROP_VIP_CHECKTIME			0x05				//检查VIP时间
#define ASS_PROP_KICKUSER				0x06				//踢人
#define ASS_PROP_BIG_BOARDCASE			0x07				//大喇叭消息
#define ASS_PROP_SMALL_BOARDCASE		0x08				//小喇叭消息

// PengJiLin, 2010-9-13, 使用踢人卡踢人
#define ASS_PROP_NEW_KICKUSER           0x09

// PengJiLin, 2010-9-14, 使用踢人卡踢人消息结果
#define ASS_PROP_NEW_KICKISER_INFO          0x0a

// PengJiLin, 2010-10-13, 商店道具的即时购买
#define ASS_PROP_BUY_NEW				0x0b

//游戏子消息
#define ASS_PROP_USEPROP				0x10				//使用道具

//道具操作结果信息                                          //add wyl 
#define ASS_PROP_RESULT					0x11				//道具购买结果

#define ASS_PROP_BUY_VIP				0x12				//购买VIP

#define ASS_PROP_GIVE_VIP				0x13				//赠送VIP


#define ASS_PROP_BIG_BOARDCASE_BUYANDUSE	0x14			//即时购买大喇叭和使用


#define DTR_GR_PROP_USE_ERR_PARAM_INVALID	10
#define DTR_GR_PROP_USE_ERR_UNKNOW			20
#define DTR_GR_PROP_USE_ERR_NOPROP			30
#define DTK_GR_PROP_USE_SUCCEED				80


#define DTK_GP_PROP_BUY_ERROR				10
#define DTK_GP_PROP_BUY_NOMONEY				20
#define DTK_GP_PROP_BUY_SUCCEED				80
#define DTK_GP_PROP_BUYANDUSE_SUCCEED				81

#define DTK_GP_PROP_GIVE_ERROR				10
#define DTK_GP_PROP_GIVE_NOENOUGH			20
#define DTK_GP_PROP_GIVE_FAILEDSEND			50
#define DTK_GP_PROP_GIVE_NOTARGETUSER		60
#define DTK_GP_PROP_GIVE_SUCCEED_BEGIVER	70
#define DTK_GP_PROP_GIVE_SUCCEED_GIVER		80

#define DTK_GP_VIP_BUY_ERROR				10
#define DTK_GP_VIP_BUY_NOMONEY				20
#define DTK_GR_VIP_BUY_SUCCEED				30
#define DTK_GR_VIP_BUY_NOUSER				40
#define DTK_GR_VIP_GIVE_SUCCESS				50

#define MAX_BLOCK_PROP_SIZE				512					//每个消息的最大数量

#define BROADCAST_MESSAGE_MAX			255					//广播消息长度



//=======================数据结构定义=====================
/*
结构：取得服务器上存储的个人道具
内容：用户的ID（防止错误，可以不要的），本地好友数

*/

typedef struct  
{
	long		dwUserID;						//用户ID

}MSG_PROP_C_GETSAVED;

/*
结构：从服务器上取得的个人道具，每一个道具一条消息；
      本地存放的个人道具数据
*/
typedef struct  
{
	long		dwUserID;						//用户ID号
	int			nPropID;						//道具ID号
	int			nHoldCount;						//拥有道具的数量
	TCHAR		szPropName[50];					//道具名称
	int			attribAction;					//操作属性
	int			attribValue;					//取值属性
	int			dwCost;							//当前该笔交易花了多少钱，由ZXD添加，服务器须提供此数据
	int			iPrice;							//道具价格			
	int			iVipPrice;						//VIP道具价格
}MSG_PROP_S_GETPROP,_TAG_USERPROP;

/*
结构：用户使用道具的消息结构体，顺带了一些与道具相关的属性
*/
typedef struct  
{
	long		dwUserID;						//使用道具的用户ID
	long		dwTargetUserID;					//使用道具时的对象用户ID，可以是自己
	int			nPropID;						//道具ID
	int			nPropActionAttrib;				//道具操作属性
	int			nPropValueAttrib;				//道具取值属性
	int			iDoubleTime;					//双倍积分时间
	int			iProtectTime;					//护身符时间
	TCHAR		szPropName[50];					//道具名称
}_TAG_USINGPROP;

//用户购买VIP返回的相关属性     add by wyl   11-5-21
typedef struct  
{
	TCHAR		szTargetUser[32];				//赠送对象的昵称
	long		dwUserID;						//用户ID号
	int			nPropID;						//道具ID号
	int			nHoldCount;						//拥有道具的数量
	TCHAR		szPropName[50];					//道具名称
	int			attribAction;					//操作属性
	int			attribValue;					//取值属性
	int			dwCost;							//当前该笔交易花了多少钱，由ZXD添加，服务器须提供此数据
}MSG_PROP_S_BUYVIP,_TAG_VIPBUG;

/*
结构：在有动画的情况下，向同桌用户发送道具使用消息
*/
typedef struct  
{
	long	dwUserID;
	long	dwTargetUserID;
	int		nUserStation;
	int		nTargetUserStation;
	int		nPropID;
	int		nPropActionAttrib;
	int		nPropValueAttrib;
}_TAG_PROP_MOIVE_USE;

struct _TAG_PROP_BUY
{
	long	dwUserID;					//购买者ID
	int		nPropID;					//道具ID
	long	iPropPayMoney;				//总共的金币
	int		nPropBuyCount;				//道具数量
	bool	bUse;

	_TAG_PROP_BUY()
	{
		ZeroMemory(this, sizeof(_TAG_PROP_BUY));
		bUse = false;
	}
};


typedef struct    //add by wyl  for VIP 
{
	long	dwUserID;					//购买者ID
	int		nPropType;					//道具类型
	TCHAR	szTargetUser[32];			//赠送对象的昵称
}_TAG_PROP_BUY_VIP;


typedef struct  
{
	long	dwUserID;					//赠送的用户ID
	TCHAR	szUser[32];					//赠送者的昵称
	TCHAR	szTargetUser[32];			//赠送对象的昵称
	int		nPropID;					//赠送的道具ID
	int		nGiveCount;					//赠送的道具数量
	int		iPropValue;					//道具价值
}_TAG_PROP_GIVE;

typedef struct  
{	
	_TAG_PROP_GIVE	propGiveInfo;		//道具赠送信息
	_TAG_USERPROP propInfo;				//道具具体信息
}_TAG_PROP_GIVE_FOR;

typedef struct  
{
	int									nPropID;							//道具ID号
	long								dwUserID;							//用户ID
	COLORREF							crColor;							//字体颜色
	SHORT								iLength;							//信息长度
	CHAR								szUserName[32];						//用户名字;		
	CHAR								szMessage[BROADCAST_MESSAGE_MAX+1];//聊天内容
} _TAG_BOARDCAST;

//vip 时间检测
typedef struct
{
	long int						dwUserID;								//玩家ID
	int								iVipTime;								//vip时间
	int								iDoublePointTime;					//双倍积分时间
	int								iProtectTime;						//护身符时间，保留
}_TAG_PROP_TIME;

typedef struct
{
	long int						dwUserID;								//玩家ID
	long int						dwDestID;								//目标人物
}_TAG_KICKUSER;

// PengJiLin, 2010-9-13, 使用踢人卡踢人
typedef struct
{
    long int  dwUserID;
    long int  dwDestID;
}_TAG_NEW_KICKUSER;

typedef struct 
{
	long		dwUserID;						//ID
	char		sNickName[61];					//昵称
}MSG_IM_C_GETFRIENDLIST,MSG_IM_S_GETFRIENDLIST;


struct TPropChange
{
	int	iUserId;
	int iPropId;
	int iPropCount;
	int iOpType;		//1表示获取,2表示设置(表示)
};

#endif

