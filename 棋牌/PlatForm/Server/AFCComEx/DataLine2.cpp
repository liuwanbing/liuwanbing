#include "StdAfx.h"
#include "DataLine.h"
#include "AFCFunction.h"

//构造函数
CDataLine::CDataLine(UINT uStep, UINT uMaxDataLine, UINT uMaxLength) : m_uMaxLength(uMaxLength)
{
	//设置数据
	m_uEndPos=0;
	m_uAddPos=0;
	m_uDataSize=0;
	m_uGetDataPos=0;
	m_uStep=uStep;
	m_uBufferLen=0;
	m_pDataBuffer=NULL;
	m_hCompletionPort=NULL;
	m_uMaxDataLine=uMaxDataLine;
}

//析构函数
CDataLine::~CDataLine(void)
{
	SafeDeleteArray(m_pDataBuffer);
}

//加入消息队列
UINT CDataLine::AddData(DataLineHead * pDataInfo, UINT uAddSize, UINT uDataKind, void * pAppendData, UINT uAppendAddSize)
{
	if ((m_hCompletionPort!=NULL)||((uAddSize+uAppendAddSize)<=m_uMaxDataLine))
	{
		CSignedLockObject LockObject(&m_csLock,true);

		//初始化数据
		pDataInfo->uSize=uAddSize;
		pDataInfo->uDataKind=uDataKind;
		if (pAppendData!=NULL) pDataInfo->uSize+=uAppendAddSize;
		else uAppendAddSize=0;

		try
		{
			//效验缓冲区
			if ((m_uDataSize+pDataInfo->uSize)>m_uBufferLen)  throw TEXT("缓冲区不足");
			else if ((m_uAddPos==m_uEndPos)&&((m_uAddPos+pDataInfo->uSize)>m_uBufferLen)) 
			{
				if (m_uGetDataPos>=pDataInfo->uSize) m_uAddPos=0;
				else throw TEXT("缓冲区不足");
			}
			else if ((m_uAddPos<m_uEndPos)&&((m_uAddPos+pDataInfo->uSize)>m_uGetDataPos)) throw TEXT("缓冲区不足");
		}
		catch (...)
		{
			//效验是否超过最大长度
			if ((m_uMaxLength>0)&&((m_uDataSize+pDataInfo->uSize)>m_uMaxLength)) return 0;

			try
			{
				//申请内存
				UINT uNewBufferLen=__min(m_uMaxLength,__max(m_uBufferLen+m_uStep,m_uDataSize+pDataInfo->uSize));
				BYTE * pNewBuffer=new BYTE [uNewBufferLen];
				if (pNewBuffer==NULL) return 0;

				//拷贝数据
				if (m_pDataBuffer!=NULL) 
				{
					::CopyMemory(pNewBuffer,m_pDataBuffer+m_uGetDataPos,m_uEndPos-m_uGetDataPos);
					if (m_uEndPos-m_uGetDataPos<m_uDataSize)
						::CopyMemory(pNewBuffer+m_uEndPos-m_uGetDataPos,m_pDataBuffer,m_uAddPos);
				}

				//调整数据
				SafeDeleteArray(m_pDataBuffer);
				m_pDataBuffer=pNewBuffer;
				m_uBufferLen=uNewBufferLen;
				m_uGetDataPos=0;
				m_uAddPos=m_uDataSize;
				m_uEndPos=m_uDataSize;
			}
			catch (...) { return 0; }
		}

		//拷贝数据
		try
		{
			CopyMemory(m_pDataBuffer+m_uAddPos,pDataInfo,uAddSize);
			if (pAppendData!=NULL) ::CopyMemory(m_pDataBuffer+m_uAddPos+uAddSize,pAppendData,uAppendAddSize);
			m_uAddPos+=pDataInfo->uSize;
			m_uDataSize+=pDataInfo->uSize;
			m_uEndPos=__max(m_uEndPos,m_uAddPos);
			::PostQueuedCompletionStatus(m_hCompletionPort,pDataInfo->uSize,NULL,NULL);
			return pDataInfo->uSize;
		}
		catch (...) { }
	}

	return 0;
}

//提取消息数据
UINT CDataLine::GetData(DataLineHead * pDataBuffer, UINT uBufferSize)
{
	CSignedLockObject LockObject(&m_csLock,true);

	if ((m_uDataSize>0)&&(m_pDataBuffer!=NULL))
	{
		//效验数据
		if (m_uGetDataPos==m_uEndPos)
		{
			m_uGetDataPos=0;
			m_uEndPos=m_uAddPos;
		}
		
		//指向数据
		DataLineHead * pDataInfo=(DataLineHead *)(m_pDataBuffer+m_uGetDataPos);
		if (pDataInfo->uSize>uBufferSize)
		{
			m_uGetDataPos+=pDataInfo->uSize;
			m_uDataSize-=pDataInfo->uSize;
			return 0;
		}

		//拷贝数据
		try
		{
			CopyMemory(pDataBuffer,pDataInfo,pDataInfo->uSize);
			m_uGetDataPos+=pDataInfo->uSize;
			m_uDataSize-=pDataInfo->uSize;
			return pDataInfo->uSize;
		}
		catch (...) { }
	}

	return 0;
}

//获取缓冲区信息
bool CDataLine::GetBufferInfo(UINT & uDataSize, UINT & uBufferLen, UINT & uMaxLength)
{
	CSignedLockObject LockObject(&m_csLock,true);

	uDataSize=m_uDataSize;
	uBufferLen=m_uBufferLen;
	uMaxLength=m_uMaxLength;

	return true;
}

//清理所有数据
bool CDataLine::CleanLineData()
{
	CSignedLockObject LockObject(&m_csLock,true);

	//设置数据 
	m_uBufferLen=0;
	m_uAddPos=0;
	m_uEndPos=0;
	m_uGetDataPos=0;
	m_uDataSize=0;
	SafeDeleteArray(m_pDataBuffer);

	return true;
}
