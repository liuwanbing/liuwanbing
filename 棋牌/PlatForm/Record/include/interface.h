// ***************************************************************
//  interface   version:  1.0   ·  date: 08/21/2008
//  -------------------------------------------------------------
//  接口文件，给游戏大厅和播放器调用调用
//  -------------------------------------------------------------
//  Copyright (C) 2008 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************
/********************************************************************
created:	2008/08/21
created:	21:8:2008   9:53
filename: 	d:\工作目录\录像系统\project\include\interface.h
file path:	d:\工作目录\录像系统\project\include
file base:	interface
file ext:	h
author:		yjj	
purpose:	为游戏平台客户端或服务器端提供记录游戏过程的接口
*********************************************************************/

#pragma once

//给大厅调用的接口
#define GETFORPLATFORM   "GetRecordForPlatForm"   //得到为平台调用的接口字符串
#define GETFORPLAYER     "GetRecordForPlayer"     // 得到为播放器调用的接口的字符串
#include "struct.h"

#define  MAX_MSG_HEAD_LEN   50
#define  MAX_MSG_DATA_LEN   (2000)   //最大一个消息数据





//公用接口
class IBaseInterFace
{
public:
	virtual void Release() = 0;//  释放对象

};
//给游戏平台调用的接口
class IRecorderForPlatform : public IBaseInterFace
{
public:
	/**
	*添加游戏玩家信息
	* @param [in] pUsrIterInfo 所要添加的玩家信息
	*/
	virtual void AddUserInfo(RecordUserItemStruct* pUsrIterInfo) = 0;  //添加某一座位的玩家
	/**
	*添加调用录像插件的玩家座位号
	* @param [in] bDeskStation 自己的座位号
	* 如果在服务器端使用的话，自己的座位号要设为2
	*/
    virtual void AddMeDeskStation(BYTE bDeskStation) = 0;  
	/**
	* 添加游戏房间信息
	* @param [in] pGameInfo 房间设置信息
	*/
	virtual void AddRoomGameInfo(RecordGameInfoStruct* pGameInfo) = 0;
	/**
	* 向插件传入消息，最好是游戏消息，还没想好播放器都负责解析哪些消息，初步设想是从四人都举手作为保存消息的起始点
	* 不用传入socket句柄，播放软件应该是不支持向外放松消息的
	* @param [in] pNetHead 消息头数据
	* @param [in] uHeadSize 消息头大小
	* @param [in] pNetData 消息数据
	* @param [in] uDataSize 消息大小
	*/
	virtual bool AddMsg2Record(void* pNetHead,UINT uHeadSize,void * pNetData, UINT uDataSize) = 0;
	/**
	*  对所有变量进行初始化和清楚，包含玩家信息，游戏房间信息，和游戏消息,相当于调用ClearAllMsg ClearAllUerInfo和ClearRoomGameInfo

	*/
	virtual void InitAll() = 0;
	/**
	* 清空所有的游戏消息信息
	*/
	virtual void ClearAllMsg() = 0;
	/**
	* 清空所有的用户信息
	*/
    virtual void ClearAllUserInfo() = 0;
	/**
	* 清空 游戏房间信息  
	*/
	virtual void ClearRoomGameInfo() = 0;
	/**
	*   压缩，保存文件  主要用在服务器保存，节省空间
	*/
	virtual bool SaveFileWithZip(const TCHAR* pFileName) = 0;
	/**
	*   压缩，加md5校验保存文件，用在客户端保存，防止文件被破坏
	*/
	virtual bool SaveFileWithPassZip(const TCHAR* pFileName) = 0;
	/**
	* 明文保存文件，主要在测试时用
	*/
	virtual bool SaveFileWithNone(const TCHAR* pFileName) = 0;
	/**
	* 反馈压缩加密后字符串的长度
	*/
	virtual UINT GetParDataSize() = 0;
	/**
	*  用户取出所有文件信息，自己决定如何写入文件或别的地方,压缩，加密后的，用户决定已什么方式存储,
	*  存入前请不要改写数据内容
	*/
	virtual void GetParsedData(unsigned char* szStr,UINT uInSize,UINT& uOutSize) = 0;

	/**
	*  生成文件名，在客户端生成应该会遵守一定的命名规则，从而生成文件名，个人感觉放在调用端会更好一些
	*  因此，目前只留一个接口，没有实现改功能
	*/
	virtual void CreateFileName(TCHAR* szFileName) = 0;
	/**
	* 管理客户端的日志文件，防止写的文件太多，目前只预留一种接口，但没有实现
	* @param [in] iFileNum 在客户端保留多少个日志文件
	*/
	virtual void MangRecordFileInClient(int iFileNum) = 0;



};

//给播放器调用的接口
class IRecorderForPlayer  : public IBaseInterFace
{
public:

	/**
	*  获取自己的用户信息
	*/
	virtual RecordUserItemStruct* GetMeUserInfo() = 0;
	/**
	* 获取某个位置的玩家信息  
	*/
	virtual RecordUserItemStruct* GetUserInfo(BYTE bDeskStation) = 0;
	/**
	*  获取玩家的数量
	*/
	virtual UINT GetGamePlayerNum() const = 0;
	/**
	*  得到游戏房间的设置信息
	*/
	virtual RecordGameInfoStruct* GetRoomGameInfo() = 0;
	/**
	*  得到消息总数
	*/
	virtual int GetTotalMsgCount() const= 0;
	/**
	*  得到消息的内容
	*/
	virtual void GetMsgInfo(int iIndex,void* ppMsgHead,void* ppMsgData,UINT& uHeadSize,UINT& uDataSize,UINT& iTimeSlot) = 0;
	/**
	*  装载压缩过的文件
	* @param [in] iIndex 所要的第几条消息
	* @param [out] ppMsgHead 消息头的指针
	* @param [out] ppMsgData 消息数据指针
	* @param [out in] uHeadSize 传入ppMsgHead的大小，传出赋值真实大小
	* @param [out in] uDataSize 传入ppMsgData的大小，传出赋值真实大小
	* @param [out] iTimeSlot 与上一消息的时间差
	* @return 返回值说明
	*/
	virtual bool LoadFileWithZip(const TCHAR* filename) = 0;
	/**
	*  装载压缩加密过的文件
	*/
	virtual bool LoadFileWithPassZip(const TCHAR* filename) = 0;
	/**
	*  装载没有进行过任何处理的文件（明文）
	*/
	virtual bool LoadFileWithNone(const TCHAR* filename) = 0;

	/**
	*  通过字符串来加载信息，可能是从数据库中读后直接装入播放器，是加密压缩方式
	*/
	virtual bool LoadStringBuffer(unsigned char * pBuffer,UINT uSize) = 0;
	/**
	* 得到客户端加载库的名字.ico 文件
	*/
	virtual TCHAR* GetClientDllName() = 0;




};

