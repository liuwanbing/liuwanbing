#pragma once

#ifndef AFCRESOURCE_HEAD_FILE
#define AFCRESOURCE_HEAD_FILE

#include "PublicDefine.h"
#include "AFCImage.h"

//using namespace AFC;
//using namespace AFC::AFCImageGoGoGo;

//宏定义
#define ICON_SIZE				24													//图标宽度
#define FONT_SIZE				12													//字体宽度

/*namespace AFC
{
	namespace AFCResource
	{*/
		/*******************************************************************************************************/

		//标题栏 GDI 资源结构
		struct CAFCTitleResource
		{
			CGameImage						m_BackPic;						//默认背景
			COLORREF						m_crTextColor;					//字体颜色
		};

		//属性页 GDI 资源结构
		struct CAFCTabResource
		{
			CGameImage						m_ActivePic;					//激活标签
			CGameImage						m_NormalPic;					//常规标签
			CGameImage						m_BackPic;						//默认背景
			CGameImage						m_BackPicNew;
			//甄亮添加
			CGameImage						m_BackBottom;				//窗口最下的一条
			CGameImage						m_BackBottomLeft;				//窗口最下的一条
			CGameImage						m_BackBottomRight;				//窗口最下的一条

			COLORREF						m_crFrameHeight;				//框架上色
			COLORREF						m_crFrameShadow;				//框架下色
			COLORREF						m_crFrameBKColor;				//框架底色
			COLORREF						m_crNormalTXColor;				//字体颜色
			COLORREF						m_crSelectTXColor;				//字体颜色
		};

		//按钮 GDI 资源结构
		struct CAFCButtonResource
		{
			//基础按钮类
			bool							m_bSound;						//启用声音
			HCURSOR							m_hMouneCursor;					//鼠标光标

			//颜色按钮部分
			COLORREF						m_ActiveColor;					//活动颜色
			COLORREF						m_ActiveTextColor;				//活动字体颜色
			COLORREF						m_NormalTextColor;				//常规字体颜色
			COLORREF						m_SelectTextColor;				//选择字体颜色
			COLORREF						m_FrameColor;					//框架颜色
			COLORREF						m_FrameHeight;					//框架上色
			COLORREF						m_FrameShadow;					//框架下色
			COLORREF						m_NormalColorTop;				//常规上色
			COLORREF						m_NormalColorBottom;			//常规下色

			//公共位图按钮部分
			CGameImage						m_BackPic;						//按钮底图
		};

		//列表视图类 GDI 资源结构
		struct CAFCListResource
		{
			CGameImage						m_TitlePic;						//列表头图片
			COLORREF						m_crListColor;					//列表列名颜色
			COLORREF						m_crBkColor;					//列表背景颜色
			COLORREF						m_crFousBkColor;				//列表背景颜色
			COLORREF						m_crFousTxColor;				//列表字体颜色
			COLORREF						m_crEmemyTxColor;				//列表字体颜色
			COLORREF						m_crNormalTxColor;				//列表字体颜色
			COLORREF						m_crNorMemTxColor;				//列表字体颜色
			COLORREF						m_crHighMemTxColor;				//列表字体颜色
			COLORREF						m_crFriendTxColor;				//列表字体颜色
			COLORREF						m_crMasterTxColor;				//列表字体颜色
			COLORREF						m_crMeTxColor;					//列表字体颜色
		};

		//对话框 GDI 资源结构
		struct CAFCDialogResource
		{
			//颜色对话框部分
			CBrush							m_BackBrush;					//背景刷
			COLORREF						m_crTextColor;					//字体颜色
			COLORREF						m_crBackColor;					//背景颜色

			//自画对话框部分
			CGameImage						m_Title;						//标题图
			CGameImage						m_MaxNor;						//最大化按钮
			CGameImage						m_MaxFoc;						//最大化按钮
			CGameImage						m_MaxDis;						//最大化按钮
			CGameImage						m_ResNor;						//还原按钮
			CGameImage						m_ResFoc;						//还原按钮
			CGameImage						m_ResDis;						//还原按钮
			CGameImage						m_MinNor;						//最小化按钮
			CGameImage						m_MinFoc;						//最小化按钮
			CGameImage						m_MinDis;						//最小化按钮
			CGameImage						m_CloseNor;						//关闭按钮
			CGameImage						m_CloseFoc;						//关闭按钮
			CGameImage						m_CloseDis;						//关闭按钮
			CGameImage						m_BackPic;						//窗口底图
			COLORREF						m_rcTopFrameColor;				//框架颜色
			COLORREF						m_rcButFrameColor;				//框架颜色
			COLORREF						m_rcTitleTextColor;				//标题颜色
		};

		//编辑控件 GDI 资源
		struct CAFCEditResource
		{
			CBrush							m_brFouns;						//焦点颜色
			CBrush							m_brNoFocus;					//失去焦点颜色
			CBrush							m_brDiable;						//不可改写颜色
			COLORREF						m_crFocusBKRGB;					//焦点颜色
			COLORREF						m_crNoFocusBKRGB;				//失去焦点颜色
			COLORREF						m_crDisFocusBKRGB;				//不可改写颜色
			COLORREF						m_crFocusTXRGB;					//焦点颜色
			COLORREF						m_crNoFocusTXRGB;				//失去焦点颜色
			COLORREF						m_crDisFocusTXRGB;				//不可改写颜色
		};

		//拆分条 GDI 资源
		struct CAFCSplitResource
		{
			HCURSOR							m_hHorCur;						//横光标
			HCURSOR							m_hVorCur;						//竖光标
			HCURSOR							m_hDisableCur;					//禁止使用
		};

		//游戏界面颜色
		struct CAFCColorResource
		{
			COLORREF						m_crListTxColor;				//游戏列表字体颜色
			COLORREF						m_crListBkColor;				//游戏列表背景颜色
			COLORREF						m_crDeskTxColor;				//大厅桌子字体颜色
			COLORREF						m_crSysHeadTxColor;				//系统前导颜色
			COLORREF						m_crSystemTxColor;				//系统信息颜色
			COLORREF						m_crSystemBdColor;				//系统公告颜色
			COLORREF						m_crNormalTxColor;				//普通信息颜色
			COLORREF						m_crTalkNameColor;				//聊天用户名字颜色
			COLORREF						m_crTalkTxColor;				//聊天信息字体颜色
			COLORREF						m_crTalkBkColor;				//聊天信息背景颜色
			COLORREF						m_crNewsTxColor;				//新闻颜色
			COLORREF						m_crSentTxColor;
			COLORREF						m_crReceivedTxColor;
		};

		/*******************************************************************************************************/

		//AFC 资源类
		class EXT_CLASS CGameImageLink//C-AFCResourceManage
		{
			//全局使用变量
		public:
			static CFont					m_VFont;						//显示字体
			static CFont					m_HFont;						//显示字体
			static HCURSOR					m_hHandCursor;					//鼠标光标
			static bool						m_bAutoLock;					//是否加载

			//资源变量
		public:
			static CAFCTabResource			m_TabResource;					//属性页资源
			static CAFCTitleResource		m_TitleResource;				//标题资源
			static CAFCButtonResource		m_ButtonResource;				//按钮资源
			static CAFCListResource			m_ListResource;					//列表资源
			static CAFCDialogResource		m_DialogResource;				//对话框资源
			static CAFCEditResource			m_EditResource;					//编辑控件资源
			static CAFCSplitResource		m_SplitResource;				//拆分条资源
			static CAFCColorResource		m_ColorResource;				//颜色资源

			//功能函数
		public:
			//初始化资源
			static bool InitResource(TCHAR * szSinkFileName=NULL);
		};

#endif