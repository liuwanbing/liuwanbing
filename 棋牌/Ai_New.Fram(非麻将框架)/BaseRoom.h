#pragma once

//#include "GamePlace.h"
//#include "NewControl\\TStatic.h"
#include "../SDK/include/socket/TCPClientSocket.h"

/*******************************************************************************************************/

/*******************************************************************************************************/

//房间基础类接口
interface IBaseGameRoom
{
	//函数定义
public:
	//询问关闭
	virtual bool CanCloseNow()=0;
	//关闭窗口
	virtual bool CloseGameRoom()=0;
	//取消连接消息
	virtual bool OnCancelConnectEvent()=0;
	//位置变化
	virtual bool OnSizeEvent(UINT nSizeType, int cx, int cy)=0;
	//拆分条消息
	virtual bool OnSplitEvent(UINT uSplitID, UINT uXPos, UINT uYPos)=0;
	//调整位置
	virtual void FixControlStation(int iWidth, int iHeight)=0;
};
//网络消息发送接口
interface ISocketSendInterface
{
	//函数定义
public:
	//发送网络消息函数
	virtual int SendGameData(BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode)=0;
	//发送网络消息函数
	virtual int SendGameData(void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode)=0;
};
/*******************************************************************************************************/

//房间基础类 
class CBaseRoom  : public CDialog , public IClientSocketService, public ISocketSendInterface//, public IBaseGameRoom
{
	//变量定义
public:
	CTCPClientSocket*							m_TCPSocket;					//网络 SOCKET
	TCHAR										m_ErrorMsg[MAX_PATH];

	//函数定义
public:
	//构造函数
	CBaseRoom(UINT uTemplate); 
	//析构函数
	virtual ~CBaseRoom();

	//重载函数
public:
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual void OnOK() { return; }
	//取消函数
	virtual void OnCancel() { return; }

	//消息函数
public:
	//接口函数
protected:
	//发送网络消息函数
	inline virtual int SendGameData(BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode);
	//发送网络消息函数
	inline virtual int SendGameData(void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode);

	DECLARE_MESSAGE_MAP()
};

/*******************************************************************************************************/
