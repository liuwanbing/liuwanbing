#ifndef __FILEVERSION_H_
#define __FILEVERSION_H_

//文件版本对比类
class CFileVersion
{ 
	//变量定义
protected:
	BYTE					m_bBuffer[2000];				//缓冲区
    DWORD					m_dwLengCharset;				//信息长度

	//函数定义
public: 
	//构造函数
    CFileVersion();
	//析构函数
    ~CFileVersion(); 

	//功能函数
public: 
	//打开文件
    bool Open(LPCTSTR lpszModuleName);
	//关闭文件
    void Close();
	//获取版本
    bool GetFixedFileVersion(DWORD & dwMSVer, DWORD & dwLSVer);
	//获取版本
    bool GetFixedProductVersion(DWORD & dwMSVer, DWORD & dwLSVer);
	//获取版本
	CString GetFixedFileVersion();
	//获取版本
	CString GetFixedProductVersion();
	//辅助函数
    bool GetFixedInfo(VS_FIXEDFILEINFO & vsffi);
	//辅助函数
    CString QueryValue(LPCTSTR lpszValueName, DWORD dwLengCharset=0);
	//辅助函数
    CString GetFileDescription()  { return QueryValue(TEXT("FileDescription")); }
	//辅助函数
    CString GetFileVersion() { return QueryValue(TEXT("FileVersion")); }
	//辅助函数
    CString GetInternalName() { return QueryValue(TEXT("InternalName")); }
	//辅助函数
    CString GetCompanyName() { return QueryValue(TEXT("CompanyName")); } 
	//辅助函数
    CString GetLegalCopyright() { return QueryValue(TEXT("LegalCopyright")); }
	//辅助函数
    CString GetOriginalFilename() { return QueryValue(TEXT("OriginalFilename")); }
	//辅助函数
    CString GetProductName() { return QueryValue(TEXT("ProductName")); };
	//辅助函数
    CString GetProductVersion() { return QueryValue(TEXT("ProductVersion")); }
}; 

#endif

