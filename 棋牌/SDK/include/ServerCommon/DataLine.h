#pragma once

#include "PublicDefine.h"
#include "AFCLock.h"

//变量宏定义
#define LD_DEF_STEP				4096								//数据队列缓冲步长
#define LD_MAX_PART				8192								//数据包最大长度//3096 -> 1024*5 百家乐 modify by wlr 20090716
#define LD_MAX_DATA				650000								//数据队列最大长度

 
#ifndef STRUCT_DATALINEHEAD
#define STRUCT_DATALINEHEAD
//数据队列信息头
struct DataLineHead
{
	UINT						uSize;								//数据大小
	UINT						uDataKind;							//数据类型
};
#endif //STRUCT_DATALINEHEAD
#ifndef STRUCT_DATABASERESULTLINE
#define STRUCT_DATABASERESULTLINE
///数据库结果消息结构定义
struct DataBaseResultLine
{
	DataLineHead						LineHead;					///队列头
	UINT								uHandleRusult;				///结果结果
	UINT								uHandleKind;				///处理类型
	UINT								uIndex;						///对象索引
	DWORD								dwHandleID;					///对象标识
};
#endif //STRUCT_DATABASERESULTLINE


/*
Struct		:ListItemData
Memo		:链表项数据结构
Author		:Fred Huang
Add Data	:2008-3-4
Modify Data	:none
Parameter	:
	stDataHead	:数据包头
	pData		:每个链表项数据的指针，使用用new方式申请的内存，注意，在出队列时，要显式的delete 该内存
*/
struct ListItemData
{
	DataLineHead				stDataHead;
	BYTE						* pData;
};
//数据队列类
class EXT_CLASS CDataLine
{
	//变量定义
private:
	CList <ListItemData*,ListItemData*> m_DataList;

	HANDLE						m_hCompletionPort;					//完成端口
	CAFCSignedLock				m_csLock;							//同步锁

	//函数定义
public:
	//构造函数
	CDataLine();
	//析构函数
	virtual ~CDataLine(void);

	//功能函数
public:
	//获取锁
	CAFCSignedLock * GetLock() { return &m_csLock; };
	//清理所有数据
	bool CleanLineData();
	//设置完成端口
	void SetCompletionHandle(HANDLE hCompletionPort) { m_hCompletionPort=hCompletionPort; }
	//加入消息队列
	virtual UINT AddData(DataLineHead * pDataInfo, UINT uAddSize, UINT uDataKind, void * pAppendData=NULL, UINT uAppendAddSize=0);
	//提取消息数据
	virtual UINT GetData(DataLineHead * pDataBuffer, UINT uBufferSize);
public:
	INT_PTR GetDataCount(void);
};

