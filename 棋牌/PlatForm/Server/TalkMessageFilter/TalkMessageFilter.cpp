#include "StdAfx.h"
#include "TalkMessageFilter.h"
#include "CommonUse.h"

CString GetAppPath(bool bFource=true)
{
    // PengJiLin, 2010-6-7, 固定使用第二种方式获取程序路径，
    // 第一种方式在某些情况下会出现问题。
    bFource = true;

	if(!bFource)
	{
#define  MAX_STRING_SIZE  1024
		char lpBuffer[MAX_STRING_SIZE];
		ZeroMemory(lpBuffer,sizeof(lpBuffer));
		GetCurrentDirectory(MAX_STRING_SIZE,lpBuffer);
		CString ss=lpBuffer;
		ss += "\\";
		return ss;
	}
	else
	{
		TCHAR szModuleName[MAX_PATH];
		ZeroMemory(szModuleName,sizeof(szModuleName));
		DWORD dwLength=GetModuleFileName(AfxGetInstanceHandle(),szModuleName,sizeof(szModuleName));
		CString filepath=szModuleName;
		CString path;
		int nSlash = filepath.ReverseFind(_T('/'));
		if (nSlash == -1)
			nSlash = filepath.ReverseFind(_T('\\'));
		if (nSlash != -1 && filepath.GetLength() > 1)
		{
			path = filepath.Left(nSlash+1);
			SetCurrentDirectory(path);
			return path;
		}
		else
		{
			char lpBuffer[MAX_STRING_SIZE];
			ZeroMemory(lpBuffer,sizeof(lpBuffer));
			GetCurrentDirectory(MAX_STRING_SIZE,lpBuffer);
			CString ss=lpBuffer;
			ss += "\\";
			return ss;
		}

	}
}


CTalkMessageFilter::CTalkMessageFilter(void)
{
	m_dwFileHandle = 0;
	m_lsUnAllowableWord.clear();
	memset(m_strReplaceContent,0,sizeof(m_strReplaceContent));
}

CTalkMessageFilter::~CTalkMessageFilter(void)
{
	if (m_dwFileHandle >= 0x10)
	{
		cfgClose(m_dwFileHandle);
	}
	list<TCHAR*>::iterator iter = m_lsUnAllowableWord.begin();
	while (!m_lsUnAllowableWord.empty())
	{
		TCHAR* pTChar = *(m_lsUnAllowableWord.begin());
		if (pTChar != NULL)
		{
			delete []pTChar;
		}
		m_lsUnAllowableWord.pop_front();
	}
	m_lsUnAllowableWord.clear();
}

//加载不需要过滤的语言
void CTalkMessageFilter::LoadFilterMessage(TCHAR* pstrAppPath)
{
	if (pstrAppPath == NULL)
	{
		return;
	}
	TCHAR szPath[1024];
	wsprintf(szPath,pstrAppPath);
	strcat(szPath,"Talk.bcf");
	if (!CBcfFile::IsFileExist(szPath))
		return;
	CBcfFile f(szPath);
	TCHAR szSec[128];
	wsprintf(szSec,"TaklFilter");
	int iIndex = 0;

	TCHAR szKey[128] = {0};



	TCHAR* pTempStrValue = NULL;
	CString strTemValue;

	strTemValue = f.GetKeyVal(szSec,"RePlaceContent","***屏蔽的消息内容***");
	strcpy(m_strReplaceContent,strTemValue);
	do
	{
		pTempStrValue = NULL;
		wsprintf(szKey,"TFileter%d",++iIndex);
		strTemValue = f.GetKeyVal(szSec,szKey,_T(""));
		if (strTemValue == "")
		{
			break;
		}
		pTempStrValue = new TCHAR[strTemValue.GetLength() + 1];
		strcpy(pTempStrValue,strTemValue.GetBuffer());

		m_lsUnAllowableWord.push_back(pTempStrValue);

	}
	while(pTempStrValue != NULL);
}
//判断是否包含需要过滤的语言
bool CTalkMessageFilter::IsAllowableTalk(TCHAR* pTalkContent,UINT uSize)
{
	if (pTalkContent == NULL || uSize == 0)
	{
		return true;
	}
	list<TCHAR*>::iterator iter = m_lsUnAllowableWord.begin();
	bool bInCludeUnAllowWord = false;
	while (iter != m_lsUnAllowableWord.end())
	{
		TCHAR* pValue = *iter;
		iter++;
		if (pValue == NULL)
		{
			continue;
		}

		if (strstr(pTalkContent,pValue) != NULL)
		{
			bInCludeUnAllowWord = true;
			break;
		}
	}
	return (!bInCludeUnAllowWord);

}
