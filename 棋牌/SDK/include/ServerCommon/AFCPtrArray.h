#pragma once

#ifndef AFCPTRARRAY_H_FILE_2003_07
#define AFCPTRARRAY_H_FILE_2003_07

#include "AFCLock.h"
#include "AFCFunction.h"

///指针数据类（离散方式）
template <class ClassType> class CAFCPtrArray
{
	///变量定义
protected:
	UINT						m_uActiveCount;						///当前有效数目
	UINT						m_uPermitCount;						///最大容纳数目
	ClassType					* * m_pActivePtr;					///有效索引
	ClassType					* * m_pSleepPtr;					///无效索引
	ClassType					* m_pClassArray;					///资料数组
	CAFCSignedLock				m_csLock;							///同步锁
	///
	///Kylin 20090116 Vip登陆功能
	UINT						m_uVipReseverCount;						///Vip保留登陆数目
	///
	///函数定义
public:
	///构造函数
	CAFCPtrArray();
	///析构函数
	~CAFCPtrArray();

	///功能函数
public:
	///初始化
	bool Init(UINT uPermitCount,UINT uVipReseverCount);
	///卸载
	bool UnInit();
	///获取项（离散方式）
	ClassType * NewItem();
	///释放项（离散方式）
	bool FreeItem(ClassType * pItem);
	///获取项（顺序方式）
	ClassType * NewItem_Order(UINT uIndex);
	///释放项（顺序方式）
	bool FreeItem_Order(UINT uIndex);

	///辅助函数
public:
	///获取有效数目
	UINT GetActiveCount() { return m_uActiveCount; };
	///获取容纳数目
	UINT GetPermitCount() { return m_uPermitCount; };
	///
	///Kylin 20090116 Vip登陆功能	
	UINT GetVipReseverCount() { return m_uVipReseverCount; };
	///
	///通过活动索引获取项（激活项）
	ClassType * GetActiveItem(UINT uIndex) { return *(m_pActivePtr+uIndex); };
	///通过索引获取项（内存项）
	ClassType * GetArrayItem(UINT uIndex) { return (m_pClassArray+uIndex); };
	///通过项获取索引（内存项）
	UINT GetItemIndex(ClassType * pFindItem) { return (UINT)(pFindItem-m_pClassArray); };
	///获取内存数组
	ClassType * GetClassArrayPtr() { return m_pClassArray; };
	///获取锁
	CAFCSignedLock * GetArrayLock() { return &m_csLock; };
};

///构造函数
template <class ClassType> CAFCPtrArray<ClassType>::CAFCPtrArray()
{
	m_uPermitCount=0;
	m_uActiveCount=0;
	m_pActivePtr=NULL;
	m_pSleepPtr=NULL;
	m_pClassArray=NULL;
	return;
}

///析构函数
template <class ClassType> CAFCPtrArray<ClassType>::~CAFCPtrArray()
{
	UnInit();
	return;
}

///初始化函数
///
///Kylin 20090116 Vip登陆功能
template<class ClassType> bool CAFCPtrArray<ClassType>::Init(UINT uPermitCount,UINT uVipReseverCount)
{
	try
	{
		///申请内存
		m_pClassArray=new ClassType [uPermitCount];
		m_pSleepPtr=new ClassType * [uPermitCount];
		m_pActivePtr=new ClassType * [uPermitCount];

		///效验错误
		if ((m_pClassArray==NULL)||(m_pSleepPtr==NULL)||(m_pActivePtr==NULL))
		{
			UnInit();
			return false;
		}

		///设置参数
		memset(m_pActivePtr,0,sizeof(ClassType *)*uPermitCount);
		for (UINT i=0;i<uPermitCount;i++) *(m_pSleepPtr+i)=m_pClassArray+i;
		m_uActiveCount=0;
		m_uPermitCount=uPermitCount;
		m_uVipReseverCount=uVipReseverCount;
		
///	
		return true;
	}
	catch (...) { TRACE("CATCH:%s with %s\n",__FILE__,__FUNCTION__);UnInit(); }
	
	return false;
}

///卸载函数
template<class ClassType> bool CAFCPtrArray<ClassType>::UnInit()
{
	SafeDeleteArray(m_pActivePtr);
	SafeDeleteArray(m_pSleepPtr);
	SafeDeleteArray(m_pClassArray);
	m_uPermitCount=0;
	m_uActiveCount=0;
	m_pActivePtr=NULL;
	m_pSleepPtr=NULL;

	return true;
}

///获取新项（离散方式）
template<class ClassType> ClassType * CAFCPtrArray<ClassType>::NewItem()
{
	if (m_uActiveCount<m_uPermitCount)
	{
		*(m_pActivePtr+m_uActiveCount)=*m_pSleepPtr;
		MoveMemory(m_pSleepPtr,m_pSleepPtr+1,(m_uPermitCount-m_uActiveCount-1)*sizeof(ClassType *));
		*(m_pSleepPtr+m_uPermitCount-m_uActiveCount-1)=NULL;
		return *(m_pActivePtr+m_uActiveCount++);
	}
	return NULL;
}

///释放项（离散方式）
template<class ClassType> bool CAFCPtrArray<ClassType>::FreeItem(ClassType * pItem)
{
	if (pItem!=NULL)
	{
		for (UINT i=0;i<m_uActiveCount;i++)
		{
			if (*(m_pActivePtr+i)==pItem)
			{
				*(m_pSleepPtr+m_uPermitCount-m_uActiveCount)=*(m_pActivePtr+i);
				*(m_pActivePtr+i)=NULL;
				*(m_pActivePtr+i)=*(m_pActivePtr+(--m_uActiveCount));
				return true;
			}
		}
	}
	return false;
}

#endif