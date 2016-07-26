// head.h : 中心服务器和与之相联的客户端公共定义

#pragma once

#define  CenterType_ClientSend  0xCE9DE9FEC885411d
#define CenterType_ClientEnter 0x831E2B518659DECF

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
		char m_strGameSerialNO[20];				////客户端当前版本系列号
		char m_strMainserverIPAddr[20];			////主服务器IP地址
		long m_iMainserverPort;					////主服务器端口号
		char m_strWebRootADDR[128],				////网站根路径，在程序中涉及的文件子目录根据这个地址来扩展
			 m_strHomeADDR[128],				////大厅登录后第一次显示的页面
			 m_strHelpADDR[128],				////帮助页WEB地址
			 m_strDownLoadSetupADDR[128],		////客户端安装程序下载页WEB地址
			 m_strDownLoadUpdatepADDR[128],		////客户端更新程序下载页WEB地址
			 m_strRallAddvtisFlashADDR[128],	////客户端大厅FLASH广告下载页WEB地址
			 m_strRoomRollADDR[200];			////客户端大厅滚动广告

		int	 m_nHallInfoShowClass;
		int	 m_nEncryptType;					////平台所采用的加密方式，1位MD5，2位SHA1，默认为2; 2009-5-30 zxj
		unsigned int m_nFunction;               ////< 功能控制 Y.C.YAN
		long m_lNomalIDFrom;                    ////< 金葫芦的散户ID开始
		long m_lNomalIDEnd;						////< 金葫芦的散户ID结束
		int  m_nIsUsingIMList;					////< 是否使用好友列表

		Message( )
		{
			m_iMainserverPort    = 0;
			m_nHallInfoShowClass = 0;
			m_nEncryptType       = 2;					//平台所采用的加密方式，1位MD5，2位SHA1，默认为2; 2009-5-30 zxj
			m_nFunction          = 0;
			m_nIsUsingIMList     = 1;					//默认为显示好友列表
			memset(m_strGameSerialNO,0,sizeof(m_strGameSerialNO));
			memset(m_strMainserverIPAddr,0,sizeof(m_strMainserverIPAddr));
			memset(m_strHomeADDR,0,sizeof(m_strHomeADDR));
			memset(m_strWebRootADDR,0,sizeof(m_strWebRootADDR));
			memset(m_strHelpADDR,0,sizeof(m_strHelpADDR));
			memset(m_strDownLoadSetupADDR,0,sizeof(m_strDownLoadSetupADDR));
			memset(m_strDownLoadUpdatepADDR,0,sizeof(m_strDownLoadUpdatepADDR));
			memset(m_strRallAddvtisFlashADDR,0,sizeof(m_strRallAddvtisFlashADDR));
			memset(m_strRoomRollADDR,0,sizeof(m_strRoomRollADDR));

		}
		~Message(){}
   };
};

typedef   CCenterHead::Message     CenterServerMsg;

