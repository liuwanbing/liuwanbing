// GVClientStyleDefine.h: interface for the Client Interface.
//
//////////////////////////////////////////////////////////////////////

#if !defined(GV_CLIENTSTYLE_DEFINE__)
#define GV_CLIENTSTYLE_DEFINE__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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
#define GV_VFT_BTN_BTN_EXTEND		0x00000080L	///< 扩展控制按钮，用户可以自定义该按钮的响应事件，按钮被按下的事件将会通过消息通知上层应用


/**
 *	视频窗体工具条显示模式
 */
typedef enum{
	GV_VFTDM_AUTOSHOW = 0,						///< 自动模式，有鼠标活动时显示，否则隐藏
	GV_VFTDM_STILLSHOW,							///< 静止模式，始终显示，不隐藏
	GV_VFTDM_VIDEOHIDE,							///< 视频隐藏模式，打开视频之前始终显示（静止模式），打开视频之后为自动模式
	GV_VFTDM_STILLHIDE,							///< 一直隐藏，不显示
}GV_VFT_DISPMODE;

/**
 *	视频窗体工具条按钮对齐方式
 */
typedef enum{
	GV_VFTBA_LEFT = 0,							///< 靠左对齐（默认）
	GV_VFTBA_RIGHT,								///< 靠右对齐
}GV_VFT_BTNALIGN;

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
	GV_VFT_BTNALIGN		vftBtnAlign;			///< 工具条按钮对齐模式
	BYTE		szExtendBtnTooltip[32];			///< 工具条扩展按钮提示信息
	DWORD		dwEBtnTooltipSize;				///< 工具条扩展按钮提示信息字符串长度
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
	GV_VF_SKIN_CRYSTALGREEN,					///< 水晶绿皮肤
	GV_VF_SKIN_MDMSTYLEDEFAULT,					///< 面对面风格默认皮肤样式
	GV_VF_SKIN_PEARLBLACK,						///< 珍珠黑皮肤
	GV_VF_SKIN_MDMSTYLEYELLOW,					///< 面对面风格黄色皮肤样式
	GV_VF_SKIN_MDMSTYLECLASSIC,					///< 面对面风格古典皮肤样式
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

#endif // !defined(GV_CLIENTSTYLE_DEFINE__)
