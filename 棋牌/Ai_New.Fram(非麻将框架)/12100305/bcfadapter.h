#pragma once

#define MAX_STRING_SIZE 128

#include "CfgEngine.h"
#ifdef _DEBUG
#pragma comment(lib,"cfgengine")
#else
#pragma comment(lib,"cfgengine")
#endif






class CBcfFile  
{
  CString lpFileName ;
  DWORD mMaxSize ;
  DWORD m_hFileHandle;
public:
	static bool IsFileExist	(CString fileName)
	{
		WIN32_FIND_DATA  d;
		HANDLE  hd= FindFirstFile(fileName,&d);
		if(hd != INVALID_HANDLE_VALUE)
			return true;

		return false;
	}
	static void CopyFileTo(CString destFileName,CString srcFileName)
	{
		bool bb=IsFileExist(srcFileName);
		if(!bb)return;
		CopyFile( srcFileName, destFileName, FALSE);

	}
		
	static CString GetWinSysPath()
	{
		char lpBuffer[MAX_STRING_SIZE];
		GetSystemDirectory( lpBuffer, MAX_STRING_SIZE);
		CString ss=lpBuffer;
		ss += "\\";
		return ss;

	}
		
	static CString GetAppPath(bool bFource=true)
	 {
		if(!bFource)
		{
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

	int GetKeyVal(CString secName,CString keyName,int lpDefault)
	{
		if(m_hFileHandle<0x10)
			return lpDefault;
		int iRet = cfgGetValue(m_hFileHandle,secName.GetBuffer(),keyName.GetBuffer(), (int)lpDefault);
		secName.ReleaseBuffer();
		keyName.ReleaseBuffer();
		return iRet;


		/*
		return (int) GetPrivateProfileInt(
					secName,// points to section name
					keyName,// points to key name
					lpDefault,       // return value if key name not found
					lpFileName//LPCTSTR lpFileName  // initialization file name
					);
		*/

	}
	

	CString GetKeyVal(CString secName,CString keyName,LPCTSTR lpDefault)
	{ 
		CString ss = lpDefault;
		if(m_hFileHandle <0x10)
			return ss;
		ss = cfgGetValue(m_hFileHandle,secName,keyName,lpDefault);


		/*
		char* re = new char[mMaxSize + 1];
		ZeroMemory(re,mMaxSize);
		GetPrivateProfileString(
								secName,// points to section name
								keyName,// points to key name
								lpDefault,// points to default string
								re,// points to destination buffer
								mMaxSize,// size of destination buffer
								lpFileName // points to initialization filename
							);
		ss=re;
		delete []re;
		*/
		return ss;
	}
		
	void SetKeyValString(CString secName,CString keyName,CString Val)
	{ 
		if(m_hFileHandle<0x10)
			return;
		
		cfgSetValue(m_hFileHandle,secName,keyName,Val);

		/*
		WritePrivateProfileString(
								secName,// pointer to section name
								keyName,// pointer to key name
								Val,// pointer to string to add
								lpFileName// pointer to initialization filename
								);
		*/

	}

		
	CBcfFile(CString FileName,int maxsize=MAX_STRING_SIZE)
	{
		lpFileName=FileName;
		mMaxSize = maxsize;

		m_hFileHandle = cfgOpenFile(FileName);
		if(m_hFileHandle<0x10)
			return;

	}

	~CBcfFile()	
	{ 
		if(m_hFileHandle >= 0x10)  
			cfgClose(m_hFileHandle);
	}
	void SetINIFileName(CString fileName)
	{
		lpFileName=fileName;
		m_hFileHandle = cfgOpenFile(fileName);
		if(m_hFileHandle<0x10)
			return;
	}
	
};





