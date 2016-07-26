#pragma once

//////////////////////////////////////////////////////////////////////////
/// 网络协议定义
/// 所有客户端与服务器端的网络协议由本文件提供
/// 按服务器功能不同分成三个部分，中心服务器，登录服务器，游戏服务器，简称B，Z，W
/// 本文件由ZXD于2010年6月2日创建
//////////////////////////////////////////////////////////////////////////


#include "StructDefines.h"	///< 以下各协议中用到的数据结构都在这里定义

//////////////////////////////////////////////////////////////////////////
/// B服务器协议
//////////////////////////////////////////////////////////////////////////
#define MDM_CONNECT						1			//连接消息类型
#define		ASS_NET_TEST					1			//网络测试
#define		ASS_CONNECT_SUCCESS 			3			//连接成功
#define MDM_GP_REQURE_GAME_PARA			102			// 请求游戏全局参数，无ASS标识
#define	MDM_GP_REQURE_GAME_CHECK		107			// 请求游戏全局参数，无ASS标识

//////////////////////////////////////////////////////////////////////////
/// Z服务器协议
//////////////////////////////////////////////////////////////////////////

#define	MDM_GP_LOGON					100									//大厅登陆
#define		ASS_GP_LOGON_BY_NAME			1									//通过用户名字登陆
#define		ASS_GP_LOGON_BY_ACC				2									//通过用户ACC 登陆
#define		ASS_GP_LOGON_BY_MOBILE			3									//通过用户手机登陆
#define		ASS_GP_LOGON_REG				4									//用户注册
#define		ASS_GP_LOGON_SUCCESS			5									//登陆成功
#define		ASS_GP_LOGON_ERROR				6									//登陆失败

#define	MDM_GP_LIST						101									//游戏列表
#define		ASS_GP_LIST_KIND				1									//获取游戏类型列表
#define		ASS_GP_LIST_NAME				2									//获取游戏名字列表
#define		ASS_GP_LIST_ROOM				3									//获取游戏房间列表
#define		ASS_GP_LIST_COUNT				4									//获取游戏人数列表
#define		ASS_GP_ROOM_LIST_PEOCOUNT		5									//获取游戏人数列表
#define		ASS_GP_ROOM_PASSWORD			6									//发送房间密码 Add by zxd，试图进入密码房间时发送此消息
#define		ASS_GP_ROOM_CREATE				7									//使用开房卡创建房间


#define	MDM_GP_MESSAGE					103									//消息接发
#define		ASS_GP_NEWS_SYSMSG				1									//新闻和系统消息
#define		ASS_GP_DUDU						2									//小喇叭

#define	MDM_GP_SYSMSG					104									//系统消息
#define		ASS_GP_NEWS_SYSMSG				1									//新闻和系统消息
#define		ASS_GP_DUDU						2									//小喇叭

#define	MDM_GP_USER_OPT					105									//大厅增加好友
#define		ASS_UPDATE_FRIEND				1									//更新好友
#define		ASS_UPDATE_EMENY				2									//更新坏人



