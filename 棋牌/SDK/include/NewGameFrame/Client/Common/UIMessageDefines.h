#pragma once
#ifndef __UI_MESSAGE_H
#define  __UI_MESSAGE_H

#include <Windows.h>
#include <tchar.h>

///<UI消息大消息头
#define  WM_UI_MSG               WM_USER+1000
#define	 WM_USER_SOFTKEY		 WM_USER+1500

///UI消息小消息头，TUIMessage的_uMessageType成员的取值
#define  UI_LOADFINISHED             200			///< UI界面载入完成
#define  UI_LBUTTONDOWN              300       
#define  UI_LBUTTONUP                301
#define  UI_RBUTTONDOWN              302
#define  UI_RBUTTONUP                303
#define  UI_CATOONFINISH             304         ///<动画播放完成，也作牌控件弹起/降下消息，_ch 为 1 表弹起， _ch 为 0 表降下
#define  UI_KEYDOWN                  305
#define  UI_KEYUP                    306
#define  UI_MJCTRL_MOVEONMJ          307
#define  UI_MJCTRL_LBUTTON           308
#define  UI_DELETE_ITEM				 309		 /// 组合框删除下拉列表中的元素，消息结构中_mouseX表示删除的第几项，0基
#define  UI_MOUSEENTER               310         /// 鼠标移入/移出控件
#define  UI_LBUTTONDBDOWN            311       	 /// 鼠标左键双击
#define  UI_COMBOBOX_SELITEM         312         /// 组合框选中某一项
#define  UI_HUZICARD_DRAGUP          313         /// 
#define  UI_MOUSEMOVE                314         /// 鼠标移动消息, 每个控件需要设置鼠标移动消息才会响应
#define  UI_COMBOBOX_CLKDROP         315		 /// 点击下拉按钮
#define  UI_TIME_RUN				  316			///时间控件发送消息，用来播放倒计时声音
#define  UI_KEY_CHAR				  317			///字符消息，主要用来文本框输入消息
#define  UI_BANK4GAME_CHECK			  318			///游戏银行消息 _ch 为 1 表取钱成功， _ch 为 0 表存钱成功

#define  UI_WINDOW_MODE_CHANGE		  319			///游戏窗口模式的变化，如最大化，还原，最小化等 
														//_ch 为 模式值如下
														// #define SW_HIDE             0
														// #define SW_SHOWNORMAL       1
														// #define SW_NORMAL           1
														// #define SW_SHOWMINIMIZED    2
														// #define SW_SHOWMAXIMIZED    3
														// #define SW_MAXIMIZE         3
														// #define SW_SHOWNOACTIVATE   4
														// #define SW_SHOW             5
														// #define SW_MINIMIZE         6
														// #define SW_SHOWMINNOACTIVE  7
														// #define SW_SHOWNA           8
														// #define SW_RESTORE          9
														// #define SW_SHOWDEFAULT      10
														// #define SW_FORCEMINIMIZE    11
														// #define SW_MAX              11

typedef signed char         INT8, *PINT8;
typedef signed short        INT16, *PINT16;
typedef signed int          INT32, *PINT32;
typedef signed __int64      INT64, *PINT64;
typedef unsigned char       UINT8, *PUINT8;
typedef unsigned short      UINT16, *PUINT16;
typedef unsigned int        UINT32, *PUINT32;
typedef unsigned __int64    UINT64, *PUINT64;


#pragma pack(4)
/// UI模块向主窗口发送的消息格式定义
struct TUIMessage 
{
	TUIMessage(){memset(this, 0, sizeof(TUIMessage));}
	UINT32 _uControlID;			///< 控件ID
	UINT32 _uMessageType;			///< 消息类型
	UINT16 _mouseX, _mouseY;	///< 如有，鼠标的窗口坐标，非屏幕坐标
	TCHAR _ch;					///< 如有，输入的键盘字符
};
#pragma pack()

#endif// __UI_MESSAGE_H
