#include "Stdafx.h"
#include "FileVersion.h"

#pragma comment(lib, "version")

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif

//构造函数
CFileVersion::CFileVersion() 
{ 
    m_dwLengCharset=0;
}

//析构函数
CFileVersion::~CFileVersion() 
{ 
    Close();
} 

//关闭文件
void CFileVersion::Close()
{
    m_dwLengCharset=0;
}

//打开文件
bool CFileVersion::Open(LPCTSTR lpszModuleName)
{
	CString apppath=CBcfFile::GetAppPath(true);

	CString icopath=lpszModuleName;
	int p=icopath.ReverseFind('\\');
	icopath=icopath.Left(p+1);
	::SetCurrentDirectory(icopath);

    DWORD dwHandle=0;
    DWORD dwDataSize=::GetFileVersionInfoSize((LPTSTR)lpszModuleName,&dwHandle);
    if ((dwDataSize==0)||(dwDataSize>sizeof(m_bBuffer))) 
		return FALSE;

    if (!::GetFileVersionInfo((LPTSTR)lpszModuleName,dwHandle,dwDataSize,(void * *)m_bBuffer)) 
		return false;

    UINT nQuerySize=0;
    DWORD * pTransTable=NULL;
    if (!::VerQueryValue(m_bBuffer,TEXT("\\VarFileInfo\\Translation"),(void * *)&pTransTable,&nQuerySize)) 
		return false;

    m_dwLengCharset=MAKELONG(HIWORD(pTransTable[0]),LOWORD(pTransTable[0]));

	::SetCurrentDirectory(apppath);
    return true;
}

//辅助函数
CString CFileVersion::QueryValue(LPCTSTR lpszValueName, DWORD dwLengCharset)
{
    if (dwLengCharset==0) dwLengCharset=m_dwLengCharset;

    UINT nQuerySize=0;
    LPVOID lpData=NULL;
    CString strValue,strBlockName;
    strBlockName.Format(TEXT("\\StringFileInfo\\%08lx\\%s"),dwLengCharset, lpszValueName);
    if (::VerQueryValue(m_bBuffer, strBlockName.GetBuffer(0),&lpData,&nQuerySize)) strValue=(LPCTSTR)lpData;
    strBlockName.ReleaseBuffer();

    return strValue;
}

//辅助函数
bool CFileVersion::GetFixedInfo(VS_FIXEDFILEINFO & vsffi)
{
    UINT nQuerySize;
	VS_FIXEDFILEINFO * pVsffi;
    if (::VerQueryValue(m_bBuffer,TEXT("\\"),(void * *)&pVsffi, &nQuerySize))
    {
        vsffi=*pVsffi;
        return true;
    }
    return false;
}

//获取版本
CString CFileVersion::GetFixedFileVersion()
{
    CString strVersion;
	VS_FIXEDFILEINFO vsffi;

    if ( GetFixedInfo(vsffi) )
    {
        strVersion.Format ("%u,%u,%u,%u",HIWORD(vsffi.dwFileVersionMS),
            LOWORD(vsffi.dwFileVersionMS),
            HIWORD(vsffi.dwFileVersionLS),
            LOWORD(vsffi.dwFileVersionLS));
    }
    return strVersion;
}

//获取版本
CString CFileVersion::GetFixedProductVersion()
{
    CString strVersion;
	VS_FIXEDFILEINFO vsffi;

    if ( GetFixedInfo(vsffi) )
    {
        strVersion.Format("%u.%u.%u.%u",HIWORD(vsffi.dwProductVersionMS),
            LOWORD(vsffi.dwProductVersionMS),
            HIWORD(vsffi.dwProductVersionLS),
            LOWORD(vsffi.dwProductVersionLS));
    }
    return strVersion;
}

//获取版本
bool CFileVersion::GetFixedFileVersion(DWORD & dwMSVer, DWORD & dwLSVer)
{
	VS_FIXEDFILEINFO vsffi;
    if (GetFixedInfo(vsffi))
    {
		dwMSVer=vsffi.dwFileVersionMS;
		dwLSVer=vsffi.dwFileVersionLS;
		return true;
    }
    return false;
}

//获取版本
bool CFileVersion::GetFixedProductVersion(DWORD & dwMSVer, DWORD & dwLSVer)
{
	VS_FIXEDFILEINFO vsffi;
    if (GetFixedInfo(vsffi))
    {
		dwMSVer=vsffi.dwProductVersionMS;
		dwLSVer=vsffi.dwProductVersionLS;
		return true;
    }
    return false;
}


