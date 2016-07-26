#pragma once

#include "../include/interface.h"
#include "interStruct.h"
#include <vector>
#include "MD5.h"
using namespace std;



//录像插件正文
class CRecorder:public IRecorderForPlatform,public IRecorderForPlayer
{
public:
	CRecorder(void);
	virtual ~CRecorder(void);

public:
	//释放对象
	void Release() {delete this;}

	//*****************************************************************************

	//给平台用户调用的接口
	/**
	*添加游戏玩家信息	
	*/
	virtual void AddUserInfo(RecordUserItemStruct* pUsrIterInfo); //添加某一座位的玩家
	/**
	*添加调用录像插件的玩家座位号
	*/
	virtual void AddMeDeskStation(BYTE bDeskStation);  
	/**
	* 添加游戏房间信息	
	*/
	virtual void AddRoomGameInfo(RecordGameInfoStruct* pGameInfo);
	/**
	* 向插件传入消息，最好是游戏消息，还没想好播放器都负责解析哪些消息，初步设想是从四人都举手作为保存消息的起始点
	* 不用传入socket句柄，播放软件应该是不支持向外放送消息的	
	*/
	virtual bool AddMsg2Record(void* pNetHead,UINT uHeadSize,void * pNetData, UINT uDataSize);
	/**
	*  对所有变量进行初始化和清楚，包含玩家信息，游戏房间信息，和游戏消息,相当于调用ClearAllMsg ClearAllUerInfo和ClearRoomGameInfo
	*/
	virtual void InitAll();
	/**
	* 清空所有的游戏消息信息
	*/
	virtual void ClearAllMsg();
	/**
	* 清空所有的用户信息
	*/
	virtual void ClearAllUserInfo();
	/**
	* 清空 游戏房间信息  
	*/
	virtual void ClearRoomGameInfo();
	/**
	*   压缩，保存文件  主要用在服务器保存，节省空间
	*/
	virtual bool SaveFileWithZip(const TCHAR* pFileName);
	/**
	*   压缩，加md5校验保存文件，用在客户端保存，防止文件被破坏
	*/
	virtual bool SaveFileWithPassZip(const TCHAR* pFileName);
	/**
	* 明文保存文件，主要在测试时用
	*/
	virtual bool SaveFileWithNone(const TCHAR* pFileName);
	/**
	* 反馈压缩加密后字符串的长度
	*/
	virtual UINT GetParDataSize();
	/**
	*  传出字符串，让用户决定怎么样使用这些信息
	*/
	virtual void GetParsedData(unsigned char* szStr,UINT uInSize,UINT& uOutSize);

	/**
	*  生成文件名，在客户端生成应该会遵守一定的命名规则，从而生成文件名，个人感觉放在调用端会更好一些
	*  因此，目前只留一个接口，而没有实现
	*/
	virtual void CreateFileName(TCHAR* szFileName){}
	virtual void MangRecordFileInClient(int iFileNum){}

	//*****************************************************************************
	//给播放器调用的接口
	/**
	*  获取自己的用户信息
	*/
	virtual RecordUserItemStruct* GetMeUserInfo();
	/**
	* 获取某个位置的玩家信息  
	*/
	virtual RecordUserItemStruct* GetUserInfo(BYTE bDeskStation);
	/**
	*  获取玩家的数量
	*/
	virtual UINT GetGamePlayerNum() const;
	/**
	*  得到游戏房间的设置信息
	*/
	virtual RecordGameInfoStruct* GetRoomGameInfo();
	/**
	*  得到消息总数
	*/
	virtual int GetTotalMsgCount() const;
	/**
	*  得到消息的内容
	*/
	virtual void GetMsgInfo(int iIndex,void* ppMsgHead,void* ppMsgData,UINT& uHeadSize,UINT& uDataSize,UINT& iTimeSlot);
	/**
	*  装载压缩过的文件	
	*/
	virtual bool LoadFileWithZip(const TCHAR* filename);
	/**
	*  装载压缩加密过的文件
	*/
	virtual bool LoadFileWithPassZip(const TCHAR* filename);
	/**
	*  装载没有进行过任何处理的文件（明文）
	*/
	virtual bool LoadFileWithNone(const TCHAR* filename);
	/**
	*  通过字符串来加载信息，可能是从数据库中读后直接装入播放器，是加密压缩方式
	*/
	virtual bool LoadStringBuffer(unsigned char * pBuffer,UINT uSize);
	/**
	* 得到客户端加载库的名字.ico 文件
	*/
	virtual TCHAR* GetClientDllName() {return m_FileHead.RoomGameInfo.szClientDLLName;}

private:
	//*****************************************************************************
	//自己需要的功能函数
	void Md5Data(UINT uFileType); //对数据进行md5加密
	
	bool Write2File(const TCHAR* pFileName,const unsigned char* pBuffer,UINT uBufferLen);
    bool Write2File(const TCHAR* pFileName);
	bool LoadZip(const TCHAR* filename,UINT uFileType);
	bool CheckMd5();
	
	
private:
	//成员变量
	//前三项，组成文件头
    FileHeadStruct  m_FileHead;
	
	//文件信息
	vector <RecordNetMsgStruct> m_arryMsgList;

	//插件是否再可用状态
	bool m_bEnable;
    //当前接受消息的数量
	int m_iMsgCount;
	//收到上一条消息的时间
	UINT m_uMsgRecTime;
};




