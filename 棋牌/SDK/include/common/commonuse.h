#pragma once
#define MAX_STRING_SIZE 128
#include <map>
#include "cfgEngine.h"
#include "Encrypt.h"
#include "BZZlib.h"
#include <algorithm>
#include <string>
#include <direct.h>
#include <Windows.h>

using namespace std;
//#include "DecryptFile.h"

///定义输出
#ifndef EXT_CLASS
#	ifdef  AFC_FACE_DLL				/// 客户端平台BZW.dll类库
#		define EXT_CLASS _declspec(dllexport)
#	elif defined(_AFC_COM_DLL_EX)	/// 服务器平台类库AfcComDll.dll
#		define EXT_CLASS _declspec(dllexport)
#	elif defined(THIS_IS_DLL)		/// 游戏框架DllForGame.dll
#		define EXT_CLASS _declspec(dllexport)
#	else
#		define EXT_CLASS _declspec(dllimport)
#	endif
#endif

///
/// {{Modified by zxd 20090810　
/// 新建的CBcfFile类封装，每个文件只读取一次，以后各次都直接从内存中取值
/// 使用者在进程结束时须显式调用CBcfFile::ClearMap()函数来释放，否则MFC会报内存泄漏

/// 因为类中使用到了静态成员，须在工程中包含有本头文件的cpp中定义下面一行代码
/// std::map<CString, CBcfFileNameItem*> CBcfFile::mapFile;

/// 我们的游戏模式用到了动态库，在动态库和EXE中都用到了CBcfFile，因此，把这个类放到动态库中导出
/// 上述静态成员应在动态库中定义，即BZW.dll，AFCComDllEx.dll，DllForGame.dll

///
/// 节点基类
class CBcfListItem
{
public:
	std::map<CString, CBcfListItem*> m_ChildMap;
	CBcfListItem()
	{
		m_ChildMap.clear();
	}
	virtual ~CBcfListItem()
	{
		std::map<CString, CBcfListItem*>::iterator iter = m_ChildMap.begin();
		while (iter != m_ChildMap.end())
		{
			CBcfListItem* pListItem = iter->second;
			if (pListItem != NULL)
			{
				delete pListItem;
			}
			++iter;
		}
		m_ChildMap.clear();
	}
	virtual void SetData(void* pData,UINT uSize)=0;
};
/// 键值map
class CBcfKeyItem : public CBcfListItem
{
	CString m_strKey;
	CString m_strValue;
	CString m_strMemo;
	CBcfKeyItem();///默认构造函数是不允许执行的
public:
	CBcfKeyItem(CString szKey, CString szValue, CString szMemo) : m_strKey(szKey), m_strValue(szValue), m_strMemo(szMemo){}
	virtual void SetData(void* pData,UINT uSize){};
	CString GetValue(){return m_strValue;}
	CString GetMemo(){return m_strMemo;}
};
/// 段map
class CBcfSectionItem : public CBcfListItem
{
	CString m_strSection;	/// 用来保存这个段的名字
public:
	CBcfSectionItem(CString szSection) : m_strSection(szSection){}
	virtual void SetData(void* pData,UINT uSize){};
};
/// 文件map
class CBcfFileNameItem : public CBcfListItem
{
	CString m_strFileName;	/// 用来保存这个文件名字
public:
	CBcfFileNameItem(CString szFileName) : m_strFileName(szFileName){}
	virtual void SetData(void* pData,UINT uSize){};
};
/// 供使用者调用的类
class EXT_CLASS CBcfFile
{
	static std::map<CString, CBcfFileNameItem*> mapFile;
	CBcfFileNameItem *m_pFile;
	bool m_bIsFileExist;
public:

	static __int64 my_atoi(const char *str)
	{
		__int64 result = 0;
		int signal = 1;
		/* 默认为正数 */
		if((*str>='0'&&*str<='9')||*str=='-'||*str=='+')
		{
			if(*str=='-'||*str=='+')
		 {
			 if(*str=='-')
				 signal = -1; /* 输入负数 */
			 str++;
		 }
		}
		else return 0;/* 开始转换 */
		while(*str>='0'&&*str<='9')
			result = result*10+(*str++ -'0');
		return signal*result;
	}
	static void ClearMap()
	{
		std::map<CString, CBcfFileNameItem*>::iterator iter = mapFile.begin();
		while (iter != mapFile.end())
		{
			CBcfFileNameItem* pListItem = iter->second;
			if (pListItem != NULL)
			{
				delete pListItem;
			}
			++iter;
		}
		mapFile.clear();
	}
	static CString GetWinSysPath()
	{
		char lpBuffer[MAX_STRING_SIZE];
		GetSystemDirectory( lpBuffer, MAX_STRING_SIZE);
		CString ss=lpBuffer;
		ss += "\\";
		return ss;

	}
	static bool IsFileExist(CString fileName)
	{
		WIN32_FIND_DATA  d;
		HANDLE  hd= FindFirstFile(fileName,&d);
		bool bRet = (hd != INVALID_HANDLE_VALUE);
		FindClose(hd);
		return bRet;
	}
	static CString GetAppPath(bool bFource=true)
	{
		bFource = true;
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

	/// 构造函数，创建或者给成员赋值
	CBcfFile(CString szFileName)
	{
		if (!IsFileExist(szFileName))
		{
			m_bIsFileExist = false;
			return;
		}
		m_bIsFileExist = false;
		/// 如果不存在，就创建之
		m_pFile = mapFile[szFileName];
		if (NULL == m_pFile)
		{
			bool bRet = ReadBcfFile(szFileName);
			if (bRet)	/// 如果文件存在，则赋值
			{
				m_bIsFileExist = true;
				mapFile[szFileName] = m_pFile;
			}
		}
		else
		{
			m_bIsFileExist = true;
		}
	}
	/// 析构函数，其中不能释放指针，否则会出现野指针
	~CBcfFile()
	{
		m_pFile = NULL;
	}
	/// 设置键值，内存读取时无效
	void SetKeyValString(CString secName,CString keyName,CString Val){}
	/// 关闭文件，不需要关闭
	void CloseFile(){}
	/// 取整数值
	int GetKeyVal(CString secName,CString keyName,int lpDefault)
	{
		if (!m_bIsFileExist)
		{
			return lpDefault;
		}
		TCHAR szSec[MAX_PATH];
		TCHAR szKey[MAX_PATH];
		UpperString(szSec, secName.GetBuffer());
		UpperString(szKey, keyName.GetBuffer());
		CBcfSectionItem *pSectionItem = (CBcfSectionItem *)m_pFile->m_ChildMap[szSec];
		if (NULL != pSectionItem)
		{
			CBcfKeyItem *pKeyItem = (CBcfKeyItem *)pSectionItem->m_ChildMap[szKey];
			if (NULL != pKeyItem)
			{
				return atoi(pKeyItem->GetValue());
			}
		}
		return lpDefault;
	}

	__int64 GetKeyVal(CString secName,CString keyName,__int64 lpDefault)
	{

		char str[255];
		sprintf(str, "%I64d", lpDefault); 

		CString __int64Str=GetKeyVal(secName,keyName,str);
		return my_atoi(__int64Str);
		
	}
	/// 取字符串
	CString GetKeyVal(CString secName,CString keyName,LPCTSTR lpDefault)
	{
		if (!m_bIsFileExist)
		{
			return lpDefault;
		}
		TCHAR szSec[MAX_PATH];
		TCHAR szKey[MAX_PATH];
		UpperString(szSec, secName.GetBuffer());
		UpperString(szKey, keyName.GetBuffer());
		CBcfSectionItem *pSectionItem = (CBcfSectionItem *)m_pFile->m_ChildMap[szSec];
		if (NULL != pSectionItem)
		{
			CBcfKeyItem *pKeyItem = (CBcfKeyItem *)pSectionItem->m_ChildMap[szKey];
			if (NULL != pKeyItem)
			{
				return pKeyItem->GetValue();
			}
		}
		return lpDefault;
	}
	/// 取注释
	CString GetKeyMemo(CString secName,CString keyName,LPCTSTR lpDefault)
	{
		if (!m_bIsFileExist)
		{
			return lpDefault;
		}
		TCHAR szSec[MAX_PATH];
		TCHAR szKey[MAX_PATH];
		UpperString(szSec, secName.GetBuffer());
		UpperString(szKey, keyName.GetBuffer());
		CBcfSectionItem *pSectionItem = (CBcfSectionItem *)m_pFile->m_ChildMap[szSec];
		if (NULL != pSectionItem)
		{
			CBcfKeyItem *pKeyItem = (CBcfKeyItem *)pSectionItem->m_ChildMap[szKey];
			if (NULL != pKeyItem)
			{
				return pKeyItem->GetMemo();
			}
		}
		return lpDefault;
	}
private:
	/// 转换成大写字符
	TCHAR *UpperString(TCHAR *szDes, LPCTSTR szSrc)
	{
		if ((szDes == NULL) || (NULL == szSrc))
		{
			return NULL;
		}
		int nLen = _tcslen(szSrc);
		//_tcscpy_s(szDes, nLen+1, szSrc);
		_tcscpy(szDes, szSrc);
		for (int i=0; i<nLen+1; ++i)
		{
			if (szDes[i]>='a' && szDes[i]<='z')
			{
				szDes[i] = szDes[i]-0x20;
			}
		}
		return szDes;
	}
	/// 只能在构造函数中当文件还没有读取时调用
	bool ReadBcfFile(CString szFileName)
	{
		DWORD hFileHandle = cfgOpenFile(szFileName);
		if(hFileHandle<0x10)
			return false;
		/// 打开文件成功，进行读取过程
		POSITION posSection = cfgFindFirstSectionPosition(hFileHandle);
		if (NULL==posSection)
		{
			return false;
		}

		m_pFile = new CBcfFileNameItem(szFileName);
		/// 循环外面声明循环里用到的变量
		LPCTSTR szAnyCaseSection,szAnyCaseKeyName,szKeyValue,szKeyMemo;
		TCHAR szSection[MAX_PATH], szKeyName[MAX_PATH];
		int		nKeyAttrib;
		while (NULL != posSection)
		{
			szAnyCaseSection = cfgFindNextSection(hFileHandle, posSection);	/// 读一次就会改变一次posSection，直到尾部时，变成NULL
			/// 把字符串转换成大写
			UpperString(szSection, szAnyCaseSection);
			/// 创建一个段
			CBcfSectionItem *pSectionItem = new CBcfSectionItem(szSection);
			/// 放到文件map中
			m_pFile->m_ChildMap[szSection] = pSectionItem;
			POSITION posKey = cfgFindFistKeyPosition(hFileHandle, szSection);
			while (NULL != posKey)
			{
				cfgFindNextKey(hFileHandle, szSection, posKey, szAnyCaseKeyName, szKeyValue, nKeyAttrib);
				UpperString(szKeyName, szAnyCaseKeyName);
				szKeyMemo = cfgGetKeyMemo(hFileHandle, szSection, szKeyName);
				CBcfKeyItem *pKeyItem = new CBcfKeyItem(szKeyName, szKeyValue, szKeyMemo);
				pSectionItem->m_ChildMap[szKeyName] = pKeyItem;
			}
		}
		cfgClose(hFileHandle);
		return true;
	}
};

/// Modified by zxd 20090810 }}
///


class CINIFile  
{
  CString lpFileName ;
  DWORD mMaxSize ;
public:

	static __int64 my_atoi(const char *str)
	{
		__int64 result = 0;
		int signal = 1;
		/* 默认为正数 */
		if((*str>='0'&&*str<='9')||*str=='-'||*str=='+')
		{
			if(*str=='-'||*str=='+')
		 {
			 if(*str=='-')
				 signal = -1; /* 输入负数 */
			 str++;
		 }
		}
		else return 0;/* 开始转换 */
		while(*str>='0'&&*str<='9')
			result = result*10+(*str++ -'0');
		return signal*result;
	}
	static bool IsFileExist(CString fileName)
	{
		WIN32_FIND_DATA  d;
		HANDLE  hd= FindFirstFile(fileName,&d);
		bool bRet = (hd != INVALID_HANDLE_VALUE);
		FindClose(hd);
		return bRet;
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
         // PengJiLin, 2010-6-7, 固定使用第二种方式获取程序路径，
         // 第一种方式在某些情况下会出现问题。
         bFource = true;

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
		return (int) GetPrivateProfileInt(
					secName,/// points to section name
					keyName,/// points to key name
					lpDefault,       /// return value if key name not found
					lpFileName///LPCTSTR lpFileName  /// initialization file name
					);

	}

	UINT GetKeyVal(CString secName,CString keyName,UINT lpDefault)
	{
		return (UINT) GetPrivateProfileInt(
					secName,/// points to section name
					keyName,/// points to key name
					lpDefault,       /// return value if key name not found
					lpFileName///LPCTSTR lpFileName  /// initialization file name
					);

	}

	__int64 GetKeyVal(CString secName,CString keyName,__int64 lpDefault)
	{
		
		char str[255];
		sprintf(str, "%I64d", lpDefault); 
	    CString __int64Str=GetKeyVal(secName,keyName,str);
		return my_atoi(__int64Str);
	}
	

	CString GetKeyVal(CString secName,CString keyName,LPCTSTR lpDefault)
	{ 
		char* re = new char[mMaxSize + 1];
		ZeroMemory(re,mMaxSize);
		GetPrivateProfileString(
								secName,/// points to section name
								keyName,/// points to key name
								lpDefault,/// points to default string
								re,/// points to destination buffer
								mMaxSize,/// size of destination buffer
								lpFileName /// points to initialization filename
							);
		CString ss=re;
		delete []re;
		return ss;
	}
		
	void SetKeyValString(CString secName,CString keyName,CString Val)
	{ 

		WritePrivateProfileString(
								secName,/// pointer to section name
								keyName,/// pointer to key name
								Val,/// pointer to string to add
								lpFileName/// pointer to initialization filename
								);

	}

		
	CINIFile(CString FileName,int maxsize=MAX_STRING_SIZE)
	{
		lpFileName=FileName;
		mMaxSize = maxsize;

	}

	~CINIFile()	{}
	void SetINIFileName(CString fileName){lpFileName=fileName;}
	
};

template<class T>
static void CopyPtrList(CPtrList &dest,CPtrList &src)
{
   for(POSITION pos=src.GetHeadPosition ();pos !=NULL;)
	{
		T *pi=(T *)src.GetNext (pos);
		if(pi)
		{
			T *pii=new T(); *pii=*pi;
			dest.AddTail (pii);
		}
	}
}

template<class T> 
static void EmptyPtrList(CPtrList &src)
{
	 while(!src.IsEmpty ())
	{
		T *pi=(T *)src.RemoveHead  ();
		delete pi;
	}
	src.RemoveAll ();
}


template<class T>
static void AddToPtrListToTail(CPtrList &dest,T &add)
{
  T *pii=new T(); *pii=add;
  dest.AddTail (pii);
}

template<class T>
static void AddToPtrListToHead(CPtrList &dest,T &add)
{
  T *pii=new T();*pii=add;
  dest.AddHead (pii);
}

#define MAX_PASSWORD_LENGTH    255
struct EXT_CLASS FileItem
{
	void * _pBufFile;
	ULONG  _nFileLen;
	unsigned char _digest[MAX_PASSWORD_LENGTH];
	FileItem()
	{
		_pBufFile=NULL;
		_nFileLen=0;
		ZeroMemory(_digest,MAX_PASSWORD_LENGTH);
	}
	~FileItem()
	{
		if(_pBufFile!=NULL)
		{
			delete []_pBufFile;
			_pBufFile=NULL;
		}
	}
};


class CMemIO
{
public:
	CMemIO(unsigned long ulSize)
	{
		m_pBuffer = new char[ulSize];
		m_ulSize = ulSize;
		m_ulSeek = NULL;
		m_bNeedDel = true;
	}
	CMemIO(const char *pBuffer, unsigned long ulSize)
	{
		m_pBuffer = (char*)pBuffer;
		m_ulSize = ulSize;
		m_ulSeek = NULL;
		m_bNeedDel = false;
	}
	~CMemIO()
	{
		if (m_bNeedDel)
			delete m_pBuffer;
	}
private:
	CMemIO(const CMemIO &other);
	CMemIO &operator = (const CMemIO &other);

public:
	int Write(char *pbuf, unsigned long ulSize)
	{
		if ((m_ulSize-m_ulSeek)<ulSize)
		{
			return 0;
		}
		memcpy(m_pBuffer+m_ulSeek, pbuf, ulSize);
		m_ulSeek += ulSize;
		return ulSize;
	}
	const char *GetMemBlock(unsigned long ulSize, unsigned long *ulSizeGetted)
	{
		if ((m_ulSize-m_ulSeek)<ulSize)
		{
			return NULL;
		}
		*ulSizeGetted = ulSize;
		char *pRet = m_pBuffer+m_ulSeek;
		m_ulSeek += ulSize;
		return pRet;
	}
	unsigned long Read(char *pDst, unsigned long ulSize)
	{
		if ((m_ulSize-m_ulSeek)<ulSize)
		{
			return 0;
		}        
		memcpy(pDst, m_pBuffer+m_ulSeek, ulSize);
		//pDst = m_pBuffer+m_ulSeek;
		m_ulSeek += ulSize;
		return ulSize;
	}
	unsigned long Read(char **pDst, unsigned long ulSize)
	{
		if ((m_ulSize-m_ulSeek)<ulSize)
		{
			return 0;
		}                
		*pDst = m_pBuffer+m_ulSeek;
		m_ulSeek += ulSize;
		return ulSize;
	}
	void SeekToBegin()
	{
		m_ulSeek = 0;
	}

private:
	char            *m_pBuffer;
	unsigned long   m_ulSize;
	unsigned long   m_ulSeek;
	bool            m_bNeedDel;

};
/**	@brief 该类用于将一个文件夹的内容写成一个文件，并带还原功能
*/
struct FileInfo
{
	char          szFileName[MAX_PATH];
	unsigned long ulStartPos;
	unsigned long ulLen;       
};

struct FilePos
{
	unsigned long ulStartPos;
	unsigned long ulLen;       
};

typedef std::map<std::string, FilePos> PackPosInfo;

struct Zipmen
{
	void *pBuffer;
	unsigned long uLen;
	PackPosInfo posInfo;
	Zipmen()
	{
		pBuffer = NULL;
		uLen = 0;
	}
};

/**
* @brief 得到打内存的PosInfo
*/
static void GetPosInfo(const char *pPacked, const unsigned long ulMemLen, PackPosInfo &posInfo)
{
	CMemIO mem(pPacked, ulMemLen);    
	mem.SeekToBegin();

	int nSize = 0;
	mem.Read((char*)&nSize, sizeof(nSize));

	FileInfo *pPos = new FileInfo[nSize];
	if (mem.Read((char**)&pPos, sizeof(FileInfo)*nSize)<sizeof(FileInfo)*nSize)
	{
		return ;
	}

	for (int i=0; i<nSize; ++i)
	{
		FilePos pos;
		pos.ulLen = pPos[i].ulLen;
		pos.ulStartPos = pPos[i].ulStartPos;
		std::string strFileName = pPos[i].szFileName;      
		transform(strFileName.begin(), strFileName.end(), strFileName.begin(), tolower);
		posInfo[strFileName] = pos;
	}
};

// static HRESULT ToIStream(void *pBuf,ULONG cb,IStream * & pStream)
//{
//	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, cb);
//
//	if(hGlobal == NULL)
//	{
//		return(FALSE);
//	}
//
//	void* pData = GlobalLock(hGlobal);
//	memcpy(pData, pBuf, cb);
//	GlobalUnlock(hGlobal);
//
//	return CreateStreamOnHGlobal(hGlobal, TRUE, &pStream);
//}
//static ULONG LoadFileFromZip(TCHAR *fileName,TCHAR *ZipFile,char **pBuf,char*password=NULL)
//{
//	return 0;
//}

//static ULONG LoadFileFromZip(TCHAR *fileName,void *BufZip,unsigned int lenBufZip,char **pBuf,char*password=NULL)
//{
//	memcpy(*pBuf, BufZip, lenBufZip);
//	return lenBufZip;
//}

///资源文件解密类,主要是读取图片文件。
/// 供使用者调用的类
class EXT_CLASS  CUnZipRes
{
	static std::map<CString,FileItem *> mapFile;
	bool m_bIsFileExist;
	static unsigned char m_bPassWord[MAX_PASSWORD_LENGTH];
public:
	CUnZipRes(){}
	~CUnZipRes(){}
	static void ClearMap()
	{
		std::map<CString,FileItem *>::iterator iter=mapFile.begin();
		while (iter != mapFile.end())
		{
			FileItem * pFileIterm = iter->second;
			if (pFileIterm != NULL)
			{
				delete pFileIterm;
			}
			++iter;
		}
		mapFile.clear();
	}
	
	static void SetPassWord(char * password)
	{
		memset(m_bPassWord,0,MAX_PASSWORD_LENGTH);
		if(password!=NULL)
		{
			memcpy(m_bPassWord,password,min(strlen(password) ,MAX_PASSWORD_LENGTH));
		}
	}

	static bool ReadFile(TCHAR * pPath, unsigned char* password)
	{
		CString strPath(pPath);
		strPath.Replace("/","\\");
		strPath.Replace("//","\\");
		strPath.Replace("\\\\","\\");
		strPath.Replace("Image","image");
		//DebugPrintf("传入文件名%s",pPath);

		CString strPathTmp=GetAppPath(false);
		strPathTmp=strPathTmp.Left(strPathTmp.GetLength()-1);  ///<取得当前进程路径
		int pos=strPath.Find(strPathTmp);
		CString strMap;
		char * pchar=NULL;
		CString strP;
		if(pos==-1)
		{
			///判断是上级目录的文件还是当前目录的文件
			strMap=strPathTmp;
			int pos=strPath.Find("..\\");
			CString strFileName=strPath;
			if(pos!=-1)  ///<如果是上级目录的文件，则在当前进程目录的路径下除去最后面一个文件夹
			{
				int  posMap=strPathTmp.ReverseFind('\\');
				strMap=strPathTmp.Left(posMap);
				strFileName=strPath.Right(strPath.GetLength()-3);
			}


			strP=strMap+"\\"+strFileName;     ///<将处理的过的路径再加上文件名为真正文件的全路径
			pchar=strP.GetBuffer(strP.GetLength() + 1);
		}else
		{
			pchar=strPath.GetBuffer(strPath.GetLength()+1);     ///<如果是传的全路径，则直接处理
			int  posMap=strPath.ReverseFind('\\');
			strMap=strPath.Left(posMap);
		}	
		
		///<如果是已经读取过的文件，则不再读取了
		if(mapFile.find(strMap)!=mapFile.end() )
		{
			return true;
		}

		unsigned char digest[MAX_PASSWORD_LENGTH];
		ZeroMemory(digest,MAX_PASSWORD_LENGTH);
		//encryptPW(password,strlen((char *)password),digest);
		FileItem * pFileItem = new FileItem();
		unsigned long len;
		pFileItem->_pBufFile = DecryptZipFile(pchar,(char*)password,&len);
		pFileItem->_nFileLen=len;


		Zipmen m_FileMem;
		GetPosInfo((char*)pFileItem->_pBufFile, len, m_FileMem.posInfo);
		std::map<std::string, FilePos>::iterator it = m_FileMem.posInfo.begin();
		for (;it != m_FileMem.posInfo.end();it++)
		{
			//char sz[MAX_PATH];
			//_stprintf(sz,"Package:%s",const_cast<char *>(it->first.c_str()));
			//OutputDebugString(sz);
			

			//char *pBuf = new char[it->second.ulLen]; 
			//memcpy(pBuf, ((BYTE*)pFileItem->_pBufFile + it->second.ulStartPos), it->second.ulLen);
		//m_FileMem.pBuffer = DecryptZipFile(pchar, (char*)password, &m_FileMem.uLen);
		//GetPosInfo((char*)m_FileMem.pBuffer, m_FileMem.uLen, m_FileMem.posInfo);
			FILE *s = NULL;
			string sf = "d:\\test\\" + it->first;

			for (int i = 0; i < (int)sf.size(); i++)
			{
				if (sf[i] == '\\')
				{
					{
						//ouf<<"创建目录："<<tmppath<<endl;
						mkdir(sf.substr(0, i).c_str());
					}
				}
			}

			if( !(s = fopen(sf.c_str(), "wb"))) 
			{
				//ouf<<"写文件："<<soufilename<<"失败"<<endl;
				//AfxMessageBox(sf.c_str());
				continue;
			}

			fwrite(((BYTE*)pFileItem->_pBufFile + it->second.ulStartPos), it->second.ulLen, 1, s);
			fclose(s);
		}

		if(len==0||pFileItem->_pBufFile==NULL)
		{
			return false;
		}
		memcpy(pFileItem->_digest,digest,MAX_PASSWORD_LENGTH);
		mapFile[strMap]=pFileItem;

		return true;
	}

	static bool IsFileExist(CString fileName)
	{
		//CString str=GetAppPath(false);
		//CString  strName=str+fileName;
		WIN32_FIND_DATA  d;
		HANDLE  hd= FindFirstFile(fileName,&d);
		bool bRet = (hd != INVALID_HANDLE_VALUE);
		FindClose(hd);
		return bRet;
	}

	static IStream * LoadFileResFromZip(TCHAR *fileName)
	{
		if(fileName==NULL)
		{
			return NULL;
		}
		CString strFile(fileName);
		//DebugPrintf("传入全路径%s",fileName);
		strFile.Replace("/","\\");  ///<将斜干统一成"\"
		strFile.Replace("//","\\");
		strFile.Replace("\\\\","\\");
		strFile.Replace("Image","image");
		//DebugPrintf("图片名称%s",fileName);

		std::map<CString,FileItem *>::iterator iter;
		///先判断是否是全路径，如果是，则直接取路径看在MAP中有没，如果有，则直接读该文件，如果没有，则进行其它判断
		int posAll=strFile.Find("image");
		if(posAll==-1)
		{
			return NULL;
		}
		CString strMapAll=strFile.Left(posAll-1);
		//DebugPrintf("传入中的Image左边部分%s",strMapAll);
		iter=mapFile.find(strMapAll);
		char * pchar=NULL;
		CString strP;
		if(iter==mapFile.end())///<如果本来就有该路径，说明传的就的全路径
		{

			CString strPathTmp=GetAppPath(false);   ///<获取当前进程路径
			int posfile=strFile.Find(strPathTmp);   
			if(posfile!=-1)///<判断传入路径包含磁盘路径没，若有，则减去
			{
				strFile=strFile.Right(strFile.GetLength()-strPathTmp.GetLength());
			}
			strPathTmp=strPathTmp.Left(strPathTmp.GetLength()-1);   ///<这是因为取得的当前进程路径最后会有一个“\”，所以长度减一
			
			

			///<下面是找出图片所在文件的内存文件名
			int pos1=strFile.Find("image");
			strP=strFile.Right(strFile.GetLength()-pos1);
			pchar=strP.GetBuffer(strP.GetLength() + 1);   ///<先读出文件名，保存

			int pos=strFile.Find("..\\"); 
			CString strMap;
			if(pos!=-1)  ///<如果是上级目录，则将最后面的一个文件夹除去
			{
				int  posMap=strPathTmp.ReverseFind('\\');
				strMap=strPathTmp.Left(posMap);
				iter=mapFile.find(strMap);

			}else
			{
				CString strAll=GetAllPath(strFile); ///<获取全路径
				int posTmp=strAll.Find("image");
				strMap=strAll.Left(posTmp-1);
				iter=mapFile.find(strMap);
				if(iter==mapFile.end())
				{
					CString strMapAll=strMap+"\\"+"image.r";
					TCHAR * pTmp=strMapAll.GetBuffer(strMapAll.GetLength()+1);
					ReadFile(pTmp,m_bPassWord);
					iter=mapFile.find(strMap);
				}
				//iter=mapFile.find(strPathTmp);
			} 

			
		}else
		{
			int pos1=strFile.Find("image");
			strP=strFile.Right(strFile.GetLength()-pos1);
			pchar=strP.GetBuffer(strP.GetLength() + 1);
		}


		if(iter==mapFile.end())
		{
			return NULL;
		}

		///<如果找到了，则将图片相对路径找出来
		//int pos1=strFile.Find("image");
		//CString strP=strFile.Right(strFile.GetLength()-pos1);
		//TCHAR * pchar=strP.GetBuffer(strP.GetLength() + 1);
		//DebugPrintf("图片相对路径%s",pchar);
		//CString strPath(pchar);



		void * pBufFile=iter->second->_pBufFile;
		unsigned long len=iter->second->_nFileLen;
		ULONG size=0;
		unsigned char digest[MAX_PASSWORD_LENGTH]={0};
		memcpy(digest,iter->second->_digest,MAX_PASSWORD_LENGTH);
		char *pBuf = NULL;
		/*int pos=strFile.Find("image");*/
		//strFile=strFile.Right(strFile.GetLength()-pos);
		//TCHAR * pcharPath=strFile.GetBuffer(strPath.GetLength() + 1);  ///<剥去当前EXE的路径，取出相对路径来找图片内存
		if(len==0||pBufFile==NULL)
		{
			return NULL;
		}
		//size = LoadFileFromZip(pchar,pBufFile,len,&pBuf,(char*)digest);
		Zipmen m_FileMem;
		GetPosInfo((char*)pBufFile, len, m_FileMem.posInfo);

		string sTolower = pchar;
        transform(sTolower.begin(), sTolower.end(), sTolower.begin(), tolower); 
		std::map<std::string, FilePos>::iterator it = m_FileMem.posInfo.find(sTolower);

		if (it != m_FileMem.posInfo.end())
		{
			char *pBuf = new char[it->second.ulLen]; 
			memcpy(pBuf, ((BYTE*)pBufFile + it->second.ulStartPos), it->second.ulLen);
			IStream *pStream = NULL;
			ToIStream(pBuf,it->second.ulLen,pStream);
			if(pBuf!=NULL)
			{
				delete []pBuf;
				pBuf=NULL;
			}
			return pStream;
		}
		return NULL;

		//DebugPrintf("读取图片大小%d",size);
		if(size==0 || pBuf==NULL)
		{
			return NULL;
		}
		IStream *pStream = NULL;
		ToIStream(pBuf,size,pStream);
		if(pBuf!=NULL)
		{
			delete []pBuf;
			pBuf=NULL;
		}
		return pStream;
	}
	
	static char * LoadFileBufferFromZip(TCHAR *fileName,int & BuffLen)
	{
		if(fileName==NULL)
		{
			return NULL;
		}
		CString strFile(fileName);
		//DebugPrintf("传入全路径%s",fileName);
		strFile.Replace("/","\\");  ///<将斜干统一成"\"
		strFile.Replace("//","\\");
		strFile.Replace("\\\\","\\");
		strFile.Replace("Image","image");
		//DebugPrintf("图片名称%s",fileName);

		std::map<CString,FileItem *>::iterator iter;
		///先判断是否是全路径，如果是，则直接取路径看在MAP中有没，如果有，则直接读该文件，如果没有，则进行其它判断
		int posAll=strFile.Find("image");
		if(posAll==-1)
		{
			return NULL;
		}
		CString strMapAll=strFile.Left(posAll-1);
		//DebugPrintf("传入中的Image左边部分%s",strMapAll);
		iter=mapFile.find(strMapAll);
		char  * pchar=NULL;
		CString strP;
		if(iter==mapFile.end())///<如果本来就有该路径，说明传的就的全路径
		{

			CString strPathTmp=GetAppPath(false);   ///<获取当前进程路径
			int posfile=strFile.Find(strPathTmp);   
			if(posfile!=-1)///<判断传入路径包含磁盘路径没，若有，则减去
			{
				strFile=strFile.Right(strFile.GetLength()-strPathTmp.GetLength());
			}
			strPathTmp=strPathTmp.Left(strPathTmp.GetLength()-1);   ///<这是因为取得的当前进程路径最后会有一个“\”，所以长度减一



			///<下面是找出图片所在文件的内存文件名
			int pos1=strFile.Find("image");
			strP=strFile.Right(strFile.GetLength()-pos1);
			pchar=strP.GetBuffer(strP.GetLength() + 1);   ///<先读出文件名，保存

			int pos=strFile.Find("..\\"); 
			CString strMap;
			if(pos!=-1)  ///<如果是上级目录，则将最后面的一个文件夹除去
			{
				int  posMap=strPathTmp.ReverseFind('\\');
				strMap=strPathTmp.Left(posMap);
				iter=mapFile.find(strMap);

			}else
			{
				CString strAll=GetAllPath(strFile); ///<获取全路径
				int posTmp=strAll.Find("image");
				strMap=strAll.Left(posTmp-1);
				iter=mapFile.find(strMap);
				if(iter==mapFile.end())
				{
					CString strMapAll=strMap+"\\"+"image.r";
					TCHAR * pTmp=strMapAll.GetBuffer(strMapAll.GetLength()+1);
					ReadFile(pTmp,m_bPassWord);
					iter=mapFile.find(strMap);
				}
				//iter=mapFile.find(strPathTmp);
			} 


		}else
		{
			int pos1=strFile.Find("image");
			strP=strFile.Right(strFile.GetLength()-pos1);
			pchar=strP.GetBuffer(strP.GetLength() + 1);
		}


		if(iter==mapFile.end())
		{
			return NULL;
		}

		///<如果找到了，则将图片相对路径找出来
		//int pos1=strFile.Find("image");
		//CString strP=strFile.Right(strFile.GetLength()-pos1);
		//TCHAR * pchar=strP.GetBuffer(strP.GetLength() + 1);
		//DebugPrintf("图片相对路径%s",pchar);
		//CString strPath(pchar);



		void * pBufFile=iter->second->_pBufFile;
		UINT len=iter->second->_nFileLen;
		ULONG size=0;
		unsigned char digest[MAX_PASSWORD_LENGTH]={0};
		memcpy(digest,iter->second->_digest,MAX_PASSWORD_LENGTH);
		char *pBuf = NULL;
		/*int pos=strFile.Find("image");*/
		//strFile=strFile.Right(strFile.GetLength()-pos);
		//TCHAR * pcharPath=strFile.GetBuffer(strPath.GetLength() + 1);  ///<剥去当前EXE的路径，取出相对路径来找图片内存
		if(pBufFile==NULL || len==0)
		{
			return NULL;
		}
		//size = LoadFileFromZip(pchar,pBufFile,len,&pBuf,(char*)digest);

		Zipmen m_FileMem;
		GetPosInfo((char*)pBufFile, len, m_FileMem.posInfo);
		std::map<std::string, FilePos>::iterator it = m_FileMem.posInfo.find(pchar);
		if (it != m_FileMem.posInfo.end())
		{
			char *pBuf = new char[it->second.ulLen]; 
			memcpy(pBuf, ((BYTE*)pBufFile + it->second.ulStartPos), it->second.ulLen);
			return pBuf;
		}
		return NULL;
		
		//DebugPrintf("读取图片大小%d",size);
		if(size==0 || pBuf==NULL)
		{
			return NULL;
		}
		BuffLen=size;
		return pBuf;
	}



	static CString GetAppPath(bool bFource=true)
	{
		bFource = true;
		if(!bFource)
		{
			TCHAR lpBuffer[1024];
			ZeroMemory(lpBuffer,sizeof(lpBuffer));
			GetCurrentDirectory(1024,lpBuffer);
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
				TCHAR lpBuffer[1024];
				ZeroMemory(lpBuffer,sizeof(lpBuffer));
				GetCurrentDirectory(1024,lpBuffer);
				CString ss=lpBuffer;
				ss += "\\";
				return ss;
			}

		}
	}
	
	static CString GetAllPath(CString strCurPath)
	{
		CString strPathTmp=GetAppPath(false);   ///<获取当前进程路径
		strPathTmp=strPathTmp.Left(strPathTmp.GetLength()-1);   ///<这是因为取得的当前进程路径最后会有一个“\”，所以长度减一
		
		int posCur=strCurPath.Find(strPathTmp);
		
		if(posCur!=-1)
		{
			strCurPath.Replace("\\\\","\\");
			return strCurPath;
		}else
		{
			int pos=strCurPath.Find("..\\");  
			if(pos!=-1)  ///<如果是上级目录，则将最后面的一个文件夹除去
			{
				int  posMap=strPathTmp.ReverseFind('\\');
				strPathTmp=strPathTmp.Left(posMap);
			}
			int posfile=strCurPath.Find(".\\");
			if(posfile!=-1)
			{
				strCurPath=strCurPath.Right(strCurPath.GetLength()-posfile-1);
			}
			CString strAll=strPathTmp+"\\"+strCurPath;

			strAll.Replace("\\\\","\\");
			return strAll;
		}
	}
};
