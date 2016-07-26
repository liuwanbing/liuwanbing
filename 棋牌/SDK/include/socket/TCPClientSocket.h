#pragma once

#ifndef CTCPCLIENTSOCKET_HEAD_FILE
#define CTCPCLIENTSOCKET_HEAD_FILE


#include <Windows.h>
#include <WinSock2.h>

#include "basemessage.h"

//#include "AFCResource.h"

//缓存定义
#define SED_SIZE				60000									//缓冲区大小
#define RCV_SIZE				30000									//缓冲区大小

//状态定义 
#define NO_CONNECT				0								//没有连接
#define CONNECTING				1								//正在连接
#define CONNECTED				2								//成功连接

/*namespace AFC
{
	namespace AFCSocket
	{*/
		//类说明
		class CTCPClientSocket;

		/*******************************************************************************************************/

		//客户端网络服务接口
		interface  IClientSocketService
		{
			//接口函数 
		public:
			//网络读取消息
			virtual bool OnSocketReadEvent(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)=0;
			//网络连接消息
			virtual bool OnSocketConnectEvent(UINT uErrorCode, CTCPClientSocket * pClientSocket)=0;
			//网络关闭消息
			virtual bool OnSocketCloseEvent()=0;
		};

		/*******************************************************************************************************/

		//客户端 SOKET
		class AFX_EXT_CLASS CTCPClientSocket : public CWnd
		{
			DECLARE_MESSAGE_MAP()

			//变量定义
		private:
			int									m_iReadBufLen;					//缓存区长度
			BYTE								m_bConnectState;				//连接状态
			SOCKET								m_hSocket;						//SOCKET 句柄
			IClientSocketService				* m_pIService;					//处理接口
			BYTE								m_szBuffer[RCV_SIZE];			//数据缓存区
			
			int									m_iCheckCode;
			//函数定义
		public:
			//构造函数
			CTCPClientSocket(IClientSocketService * pIService);
			//析构函数
			virtual ~CTCPClientSocket();
			//初始化 SOCKET 窗口
			bool InitSocketWnd();
			//是否已经连接
			BYTE GetSocketState() { return m_bConnectState; }
			//关闭 SOCKET
			bool CloseSocket(bool bNotify=true);
			//连接服务器
			bool Connect(const TCHAR * szServerIP, UINT uPort);
			//连接服务器
			bool Connect(long int dwServerIP, UINT uPort);
			//发送函数
			int SendData(void * pData, UINT uSize, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode);
			//简单命令发送函数
			int SendData(BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode);
			//解释错误
			CString TranslateError(UINT uErrorCode);
			//设置校验码，密文与密钥
			void SetCheckCode(__int64 iCheckCode, int isecretkey);

			//内部函数
		private:
			//关闭 SOCKET 窗口
			bool CloseSocketWnd();
			//SOCKET 消息处理程序
			LRESULT	OnSocketNotifyMesage(WPARAM wParam, LPARAM lParam);

		};
	/*};
};*/




/*******************************************************************************************************/

#endif