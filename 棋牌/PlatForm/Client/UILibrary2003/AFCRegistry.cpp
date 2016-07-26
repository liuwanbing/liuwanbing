#include "StdAfx.h"
#include "AfcRegistry.h"

//构造函数
CGameGoMemory::CGameGoMemory(HKEY hRegKey) : m_hRegKey(hRegKey)
{
}

//析构函数
CGameGoMemory::~CGameGoMemory(void)
{
	Close();
}

//打开
bool CGameGoMemory::Open(TCHAR * szSubKey)
{
	HKEY hRegKey=NULL;
	long int lReturn=RegOpenKeyEx(m_hRegKey,szSubKey,0L,KEY_ALL_ACCESS,&hRegKey);
	if (lReturn==ERROR_SUCCESS)
	{
        m_hRegKey=hRegKey;
		return true;
	}
	return false;
}

//关闭
void CGameGoMemory::Close()
{
	if (m_hRegKey)
	{
		RegCloseKey(m_hRegKey);
		m_hRegKey=NULL;
	}
	return;
}

//建立子键
bool CGameGoMemory::CreateKey(TCHAR * szSubKey)
{
	HKEY hRegKey=NULL;
	DWORD dwDispostion=0;
	long int lReturn=RegCreateKeyEx(m_hRegKey,szSubKey,0L,NULL,REG_OPTION_VOLATILE,KEY_ALL_ACCESS,NULL,&hRegKey,&dwDispostion);
	if (lReturn==ERROR_SUCCESS)
	{
		m_hRegKey=hRegKey;
		return true;
	}
	return false;
}

//删除
bool CGameGoMemory::DeleteKey(HKEY hRegKey, TCHAR * szSubKey)
{
	long int lReturn=RegDeleteKey(hRegKey,szSubKey);
	return (lReturn==ERROR_SUCCESS);
}

//删除
bool CGameGoMemory::DeleteValue(TCHAR * szValueName)
{
	long int lReturn=RegDeleteValue(m_hRegKey,szValueName);
	return (lReturn==ERROR_SUCCESS);
}

//保存
bool CGameGoMemory::SaveKey(TCHAR * szFileName)
{
	long int lReturn=RegSaveKey(m_hRegKey,szFileName,NULL);
	return (lReturn==ERROR_SUCCESS);
}

//恢复
bool CGameGoMemory::RestoreKey(TCHAR * szFileName)
{
	long int lReturn=RegRestoreKey(m_hRegKey,szFileName,REG_WHOLE_HIVE_VOLATILE);
	return (lReturn==ERROR_SUCCESS);
}

//读取
bool CGameGoMemory::Read(TCHAR * szValueName, CString & strOutVal)
{
	return false;
}

//读取
bool CGameGoMemory::Read(TCHAR * szValueName, TCHAR * szOutVal, UINT uBufferSize)
{
	return false;
}

//读取
bool CGameGoMemory::Read(TCHAR * szValueName, DWORD & dwOutVal)
{
	DWORD dwType=0,dwDest=0;
	DWORD dwSize=sizeof(DWORD);
	long int lReturn=RegQueryValueEx(m_hRegKey,szValueName,NULL,&dwType,(BYTE *)&dwDest,&dwSize);
	if (lReturn==ERROR_SUCCESS)
	{
		dwOutVal=dwDest;
		return true;
	}
	return false;
}

//读取
bool CGameGoMemory::Read(TCHAR * szValueName, int & iOutVal)
{
	DWORD dwType=0,dwDest=0;
	DWORD dwSize=sizeof(DWORD);
	long int lReturn=RegQueryValueEx(m_hRegKey,szValueName,NULL,&dwType,(BYTE *)&dwDest,&dwSize);
	if (lReturn==ERROR_SUCCESS)
	{
		iOutVal=(int)dwDest;
		return true;
	}
	return false;
}

//写入
bool CGameGoMemory::Write(TCHAR * szSubKey, TCHAR * szWriteVal)
{
	long int lReturn=RegSetValueEx(m_hRegKey,szSubKey,0L,REG_SZ,(const BYTE *)szWriteVal,lstrlen(szWriteVal)+1);
   	return (lReturn==ERROR_SUCCESS);
}

//写入
bool CGameGoMemory::Write(TCHAR * szSubKey, DWORD dwWriteVal)
{
	long int lReturn=RegSetValueEx(m_hRegKey,szSubKey,0L,REG_DWORD,(const BYTE *)&dwWriteVal,sizeof(DWORD));
   	return (lReturn==ERROR_SUCCESS);
}

//写入
bool CGameGoMemory::Write(TCHAR * szSubKey, int nWriteVal)
{
	DWORD dwValue=(DWORD)nWriteVal;
	long int lReturn=RegSetValueEx(m_hRegKey,szSubKey,0L,REG_DWORD,(const BYTE *)&dwValue,sizeof(DWORD));
   	return (lReturn==ERROR_SUCCESS);
}

