#pragma once
/**
*  视频消息定义
*/
#define	WM_GV	WM_USER + 200

#define WM_GV_CONNECT				WM_GV + 1		///< 客户端连接服务器，wParam（BOOL）表示是否连接成功
#define WM_GV_LOGINSYSTEM			WM_GV + 2		///< 客户端登录系统，wParam（INT）表示自己的用户ID号，lParam（INT）表示登录结果：0 成功，否则为出错代码，参考出错代码定义
#define WM_GV_ENTERROOM				WM_GV + 3		///< 客户端进入房间，wParam（INT）表示所进入房间的ID号，lParam（INT）表示是否进入房间：0成功进入，否则为出错代码
#define WM_GV_MICSTATECHANGE		WM_GV + 4		///< 用户的Mic状态变化消息，wParam（INT）表示用户ID号，lParam（BOOL）表示该用户是否持有Mic
#define WM_GV_USERATROOM			WM_GV + 5		///< 用户进入（离开）房间，wParam（INT）表示用户ID号，lParam（BOOL）表示该用户是进入（TRUE）或离开（FALSE）房间
#define WM_GV_LINKCLOSE				WM_GV + 6		///< 网络连接已关闭，该消息只有在客户端连接服务器成功之后，网络异常中断之时触发
#define WM_GV_ONLINEUSER			WM_GV + 7		///< 收到当前房间的在线用户信息，进入房间后触发一次，wParam（INT）表示在线用户数（包含自己），lParam（INT）表示房间ID
#define WM_GV_FORTUNEMENU			WM_GV + 8		///< 用户选择了一项财富菜单项，wParam（INT）表示用户ID号，lParam（INT）表示财富菜单标记，指示是选择了哪一项菜单

#pragma pack(push, 1)

/**
 *	视频窗口工具条按钮定义
 */
#define GV_VFT_BTN_BTN_VIDEOCTRL	0x00000001L	///< 视频控制按钮
#define GV_VFT_BTN_BTN_AUDIOCTRL	0x00000002L	///< 音频控制按钮
#define GV_VFT_BTN_BTN_CLONEVIDEO	0x00000004L	///< 弹出视频窗口按钮
#define GV_VFT_BTN_BTN_FORTUNE		0x00000008L	///< 财富菜单按钮
#define GV_VFT_BTN_BTN_SNAPSHOT		0x00000010L	///< 快照控制按钮
#define GV_VFT_BTN_BTN_RECORDFILE	0x00000020L	///< 录像控制按钮
#define GV_VFT_BTN_BTN_PLAYRECORD	0x00000040L	///< 播放录像控制按钮


/**
 *	视频窗体工具条显示模式
 */
typedef enum{
	GV_VFTDM_AUTOSHOW = 0,						///< 自动模式，有鼠标活动时显示，否则隐藏
	GV_VFTDM_STILLSHOW,							///< 静止模式，始终显示，不隐藏
	GV_VFTDM_VIDEOHIDE							///< 视频隐藏模式，打开视频之前始终显示（静止模式），打开视频之后为自动模式
}GV_VFT_DISPMODE;

/**
 *	视频窗体工具条风格结构体定义
 */
typedef struct  
{
	DWORD		cbSize;							///< 结构体大小
	COLORREF	bkColor;						///< 工具条背景颜色
	DWORD		maxTransPercent;				///< 工具条最大透明度（0-!00，默认为70，值越小，工具条越透明）
	DWORD		animateDuration;				///< 工具条变化（由隐藏到显示，或由显示到隐藏）时长，单位毫秒，默认值3000
	DWORD		dwBtnMark;						///< 在工具条上显示哪些按钮，由GV_VFT_BTN_BTN_XXXX组合而成
	DWORD		dwBtnSpacePix;					///< 按钮之间的间隔，像素点，默认为5，值越小，按钮排列越密
	GV_VFT_DISPMODE		vftDispMode;			///< 视频窗体工具条显示模式
}GV_VFT_STYLE,*LPGV_VFT_STYLE;


/**
 *	视频窗口边框风格定义
 */
typedef enum{
	GV_VF_STYLE_HIDEFRAME = 0,					///< 无边框
	GV_VF_STYLE_SMALLFRAME,						///< 小边框
	GV_VF_STYLE_TOOLBAR							///< 附带工具栏显示的边框
}GV_VFS;

/**
 *	视频窗体皮肤样式定义
 */
typedef enum{
	GV_VF_SKIN_DEFAULT = 0,						///< 默认皮肤样式
	GV_VF_SKIN_CRYSTALBLUE,						///< 水晶蓝皮肤
	GV_VF_SKIN_CLASSIC,							///< 古典皮肤
}GV_VF_SKIN;
/**
 *	视频窗体风格结构体定义
 */
typedef struct  
{
	DWORD		cbSize;							///< 结构体的大小
	COLORREF	bkColor;						///< 视频窗体背景颜色
	COLORREF	selfTitleColor;					///< 自己的标题颜色
	COLORREF	otherTitleColor;				///< 其他人的标题颜色
	GV_VFS		vfStyle;						///< 边框显示风格
	BOOL		bMoveWindow;					///< 是否可以移动视频窗体
	BOOL		bShowTitle;						///< 是否显示标题，通常为用户名
	BOOL		bShowVolume;					///< 是否显示音量
	GV_VF_SKIN	vfSkin;							///< 视频窗体皮肤样式（注：仅当边框风格设置为：GV_VF_STYLE_SMALLFRAME 时有效）
}GV_VW_STYLE,*LPGV_VW_STYLE;




#pragma pack(pop)