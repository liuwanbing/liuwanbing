#pragma once

#include "StdAfx.h"
#include "AFCLock.h"

//变量宏定义
#define LD_DEF_STEP				4096								//数据队列缓冲步长
#define LD_MAX_PART				3096								//数据包最大长度
#define LD_MAX_DATA				650000								//数据队列最大长度

//数据队列信息头
struct DataLineHead
{
	UINT						uSize;								//数据大小
	UINT						uDataKind;							//数据类型
};

//数据队列类
class EXT_CLASS CDataLine
{
	//变量定义
private:
	UINT						m_uStep;							//重申请步长
	UINT						m_uBufferLen;						//缓冲区长度
	UINT						m_uAddPos;							//数据插入点
	UINT						m_uEndPos;							//数据结束点
	UINT						m_uGetDataPos;						//当前获取点
	UINT						m_uDataSize;						//数据大小
	UINT						m_uMaxDataLine;						//最大数据包大小
	BYTE						* m_pDataBuffer;					//数据缓冲区
	HANDLE						m_hCompletionPort;					//完成端口
	const UINT					m_uMaxLength;						//最大数据长度
	CAFCSignedLock				m_csLock;							//同步锁

	//函数定义
public:
	//构造函数
	CDataLine(UINT uStep=LD_DEF_STEP, UINT uMaxDataLine=LD_MAX_PART, UINT uMaxLength=LD_MAX_DATA);
	//析构函数
	virtual ~CDataLine(void);

	//功能函数
public:
	//获取锁
	CAFCSignedLock * GetLock() { return &m_csLock; };
	//获取缓冲区信息
	bool GetBufferInfo(UINT & uDataSize, UINT & uBufferLen, UINT & uMaxLength);
	//清理所有数据
	bool CleanLineData();
	//设置完成端口
	void SetCompletionHandle(HANDLE hCompletionPort) { m_hCompletionPort=hCompletionPort; }
	//加入消息队列
	virtual UINT AddData(DataLineHead * pDataInfo, UINT uAddSize, UINT uDataKind, void * pAppendData=NULL, UINT uAppendAddSize=0);
	//提取消息数据
	virtual UINT GetData(DataLineHead * pDataBuffer, UINT uBufferSize);
};

