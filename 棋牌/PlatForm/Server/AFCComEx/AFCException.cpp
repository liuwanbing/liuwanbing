#include "StdAfx.h"
#include "AFCException.h"

//静态变量定义
bool		CAFCException::m_bShowError=false;					//是否显示错误

//构造函数
CAFCException::CAFCException(TCHAR * szErrorMessage, UINT uErrorCode, bool bAutoDelete)
{
	m_uErrorCode=uErrorCode;
	m_bAutoDelete=bAutoDelete;
	if ((szErrorMessage!=NULL)&&(szErrorMessage[0]!=0))
	{
		if (lstrlen(szErrorMessage)<(sizeof(m_szMessage)/sizeof(m_szMessage[0])))	
			lstrcpy(m_szMessage,szErrorMessage);
		else lstrcpy(m_szMessage,TEXT("异常错误信息太长"));
	}
	else lstrcpy(m_szMessage,TEXT("没有异常信息"));
	if (m_bShowError==true)	AfxMessageBox(m_szMessage,MB_ICONSTOP);
	return;
}

//析构函数
CAFCException::~CAFCException(void)
{
}

//获取错误代码
UINT CAFCException::GetErrorCode() const
{
	if (this!=NULL) return m_uErrorCode;
	return 0;
}

//删除函数
bool CAFCException::Delete()
{
	if ((this!=NULL)&&(m_bAutoDelete==true))
	{
		delete this;
		return true;
	}
	return false;
}

//获取错误信息
TCHAR const * CAFCException::GetErrorMessage(TCHAR * szBuffer, int iBufLength) const
{
	if (this!=NULL)
	{
		//拷贝错误信息
		if ((szBuffer!=NULL)&&(iBufLength>0))
		{
			int iCopyLength=__min(iBufLength-1,::lstrlen(m_szMessage));
			szBuffer[iCopyLength]=0;
			::CopyMemory(szBuffer,m_szMessage,iCopyLength*sizeof(TCHAR));
		}
		return m_szMessage;
	}
	return NULL;
}

//设置是否显示错误
bool CAFCException::ShowErrorMessage(bool bShowError)
{
	m_bShowError=bShowError;
	return m_bShowError;
}
