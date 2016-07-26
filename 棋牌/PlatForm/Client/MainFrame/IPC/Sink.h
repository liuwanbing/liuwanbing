#pragma once

#include "common.h"

//信道消息钩子接口
interface IChannelMessageSink
{
	//信道数据处理
	virtual bool __cdecl OnChannelMessage(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)=NULL;
};


//信道事件钩子接口
interface IChannelEventSink
{
	//连接消息
	virtual bool __cdecl OnChannelConnect(HWND hWndSend)=NULL;
	//应答消息
	virtual bool __cdecl OnChannelAccept(HWND hWndSend)=NULL;
	//断开消息
	virtual bool __cdecl OnChannelClose(HWND hWndSend, bool bInitiative)=NULL;
};


//信道模块接口
interface IChannelService
{
	//设置事件
	virtual bool __cdecl SetChannelEventSink(IChannelEventSink * pIUnknownEx)=NULL;
	//设置消息
	virtual bool __cdecl SetChannelMessageSink(IChannelMessageSink * pIUnknownEx)=NULL;
	//获取事件
	virtual void * __cdecl GetChannelEventSink(const IID & Guid, DWORD dwQueryVer)=NULL;
	//获取消息
	virtual void * __cdecl GetChannelMessageSink(const IID & Guid, DWORD dwQueryVer)=NULL;
	//是否连接
	virtual bool __cdecl IsConnect(bool bEfficacy)=NULL;
	//创建函数
	virtual bool __cdecl CreateChannel(HWND hWndServer)=NULL;
	//关闭函数
	virtual bool __cdecl CloseChannel(bool bNotifyServer, bool bNotifySink)=NULL;
	//发送数据
	virtual bool __cdecl SendData(WORD wMainCmdID, WORD wSubCmdID)=NULL;
	//发送数据
	virtual bool __cdecl SendData(WORD wMainCmdID, WORD wSubCmdID, void * pBuffer, WORD wDataSize)=NULL;
};
