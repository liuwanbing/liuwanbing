#pragma once
#include <Windows.h>


/// MDM_GP_REQURE_GAME_PARA协议用到的数据结构
//class CCenterHead
//{
//public:
//	enum enumCenterType
//	{
//		CenterServerPort=65320//6688
//	};
//	class Message
//	{
//	public:
//		char m_strGameSerialNO[20];////客户端当前版本系列号
//		char m_strMainserverIPAddr[20];			////主服务器IP地址
//		long m_iMainserverPort;					////主服务器端口号
//		int  m_iOnlineUserCount;                ////当前游戏在线人数
//		char m_strWebRootADDR[50],				////网站根路径，在程序中涉及的文件子目录根据这个地址来扩展
//			m_strHomeADDR[50],					////大厅登录后第一次显示的页面
//			m_strGoldADDR[500],				///金币不足时提示话语
//			m_strHelpADDR[50],					////帮助页WEB地址
//			m_strDownLoadSetupADDR[50],		////客户端安装程序下载页WEB地址
//			m_strDownLoadUpdatepADDR[50],		////客户端更新程序下载页WEB地址
//			m_strRallAddvtisFlashADDR[50],		//客户端大厅FLASH广告下载页WEB地址
//			m_strRoomRollADDR[100];			//客户端大厅滚动广告
//		int                                gscheck;
//		int	 m_nHallInfoShowClass;
//		Message( )
//		{
//			m_iMainserverPort=0;
//			m_nHallInfoShowClass=0;
//			m_iOnlineUserCount = 0;
//			memset(m_strGameSerialNO,0,20);
//			gscheck=0;
//			memset(m_strMainserverIPAddr,0,20);
//			memset(m_strHomeADDR,0,50);
//			memset(m_strWebRootADDR,0,50);
//			memset(m_strHelpADDR,0,50);
//			memset(m_strDownLoadSetupADDR,0,50);
//			memset(m_strDownLoadUpdatepADDR,0,50);
//			memset(m_strRallAddvtisFlashADDR,0,50);
//			memset(m_strRoomRollADDR,0,100);
//
//		}
//		~Message(){}
//	};
//};
class CCenterHead
{
public:
	enum enumCenterType
	{
		CenterServerPort=65320//6688
	};
	class Message
	{
	public:
		int m_is_haveZhuanZhang;
		char m_strGameSerialNO[20];////客户端当前版本系列号
		char m_strMainserverIPAddr[20];			////主服务器IP地址
		long m_iMainserverPort;					////主服务器端口号
		char m_strWebRootADDR[50],				////网站根路径，在程序中涉及的文件子目录根据这个地址来扩展
			m_strHomeADDR[50],					////大厅登录后第一次显示的页面
			m_strHelpADDR[50],					////帮助页WEB地址
			m_strDownLoadSetupADDR[50],		////客户端安装程序下载页WEB地址
			m_strDownLoadUpdatepADDR[50],		////客户端更新程序下载页WEB地址
			m_strRallAddvtisFlashADDR[50],		//客户端大厅FLASH广告下载页WEB地址
			//wushuqun 2009.6.5
			m_strRoomRollADDR[200];			//客户端大厅滚动广告

		int	 m_nHallInfoShowClass;
		int	 m_nEncryptType;					//平台所采用的加密方式，1位MD5，2位SHA1，默认为2; 2009-5-30 zxj
		unsigned int m_nFunction;               ///< 功能控制 Y.C.YAN
		long m_lNomalIDFrom;                    ///< 金葫芦的散户ID开始
		long m_lNomalIDEnd;						///< 金葫芦的散户ID结束
		Message( )
		{
			m_iMainserverPort    = 0;
			m_nHallInfoShowClass = 0;
			m_nEncryptType       = 2;					//平台所采用的加密方式，1位MD5，2位SHA1，默认为2; 2009-5-30 zxj
			m_nFunction          = 0;
			memset(m_strGameSerialNO,0,20);
			memset(m_strMainserverIPAddr,0,20);
			memset(m_strHomeADDR,0,50);
			memset(m_strWebRootADDR,0,50);
			memset(m_strHelpADDR,0,50);
			memset(m_strDownLoadSetupADDR,0,50);
			memset(m_strDownLoadUpdatepADDR,0,50);
			memset(m_strRallAddvtisFlashADDR,0,50);
			//wushuqun 2009.6.5
			memset(m_strRoomRollADDR,0,200);

		}
		~Message(){}
	};
};
typedef   CCenterHead::Message     CenterServerMsg;
