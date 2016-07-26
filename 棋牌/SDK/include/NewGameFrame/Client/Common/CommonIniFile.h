#pragma once
#define MAX_STRING_SIZE 128
#include "CommonDefines.h"
#include <map>
#include <cstring>

//#include <tchar.h>

class CINIFile  
{
  Tstring lpFileName;
  DWORD mMaxSize ;
public:
	static bool IsFileExist(TCHAR *fileName)
	{
		WIN32_FIND_DATA  d;
		HANDLE  hd= FindFirstFile(fileName,&d);
		bool bRet = (hd != INVALID_HANDLE_VALUE);
		FindClose(hd);
		return bRet;
	}
	static void CopyFileTo(TCHAR *destFileName, TCHAR *srcFileName)
	{
		bool bb=IsFileExist(srcFileName);
		if(!bb)return;
		CopyFile( srcFileName, destFileName, FALSE);

	}
		
	static Tstring GetWinSysPath()
	{
		TCHAR lpBuffer[MAX_STRING_SIZE];
		GetSystemDirectory(lpBuffer, MAX_STRING_SIZE);
		Tstring ss=lpBuffer;
		ss += TEXT("\\");
		return ss;

	}
		
	static Tstring GetAppPath(bool bFource=true)
	 {
         // PengJiLin, 2010-6-7, 固定使用第二种方式获取程序路径，
         // 第一种方式在某些情况下会出现问题。
         bFource = true;

		if(!bFource)
		{
			TCHAR lpBuffer[MAX_STRING_SIZE];
			ZeroMemory(lpBuffer,sizeof(lpBuffer));
			GetCurrentDirectory(MAX_STRING_SIZE,lpBuffer);
			Tstring ss=lpBuffer;
			ss += TEXT("\\");
			return ss;
		}
		else
		{
			TCHAR szModuleName[MAX_PATH];
			ZeroMemory(szModuleName,sizeof(szModuleName));
			DWORD dwLength=GetModuleFileName(GetModuleHandle(NULL), szModuleName, sizeof(szModuleName));
			Tstring filepath=szModuleName;
			Tstring path;
			int nSlash = filepath.ReverseFind(_T('/'));
			if (nSlash == -1)
				nSlash = filepath.ReverseFind(_T('\\'));
			if (nSlash != -1 && filepath.GetLength() > 1)
			{
				path = filepath.Left(nSlash+1);
				//SetCurrentDirectory(path);
				return path;
			}
			else
			{
				TCHAR lpBuffer[MAX_STRING_SIZE];
				ZeroMemory(lpBuffer,sizeof(lpBuffer));
				GetCurrentDirectory(MAX_STRING_SIZE,lpBuffer);
				Tstring ss=lpBuffer;
				ss += TEXT("\\");
				return ss;
			}

		}
	}

	int GetKeyVal(Tstring secName,Tstring keyName,int lpDefault)
	{
		return (int) GetPrivateProfileInt(
					secName,/// points to section name
					keyName,/// points to key name
					lpDefault,       /// return value if key name not found
					lpFileName///LPCTSTR lpFileName  /// initialization file name
					);

	}
	

	Tstring GetKeyVal(Tstring secName,Tstring keyName,LPCTSTR lpDefault)
	{ 
		TCHAR* re = new TCHAR[mMaxSize + 1];
		ZeroMemory(re,mMaxSize);
		GetPrivateProfileString(
								secName,/// points to section name
								keyName,/// points to key name
								lpDefault,/// points to default string
								re,/// points to destination buffer
								mMaxSize,/// size of destination buffer
								lpFileName /// points to initialization filename
							);
		Tstring ss=re;
		delete []re;
		return ss;
	}
		
	void SetKeyValString(Tstring secName,Tstring keyName,Tstring Val)
	{ 

		WritePrivateProfileString(
								secName,/// pointer to section name
								keyName,/// pointer to key name
								Val,/// pointer to string to add
								lpFileName/// pointer to initialization filename
								);

	}

		
	CINIFile(Tstring FileName,int maxsize=MAX_STRING_SIZE)
	{
		lpFileName=FileName;
		mMaxSize = maxsize;

	}

	~CINIFile()	{}
	void SetINIFileName(Tstring fileName){lpFileName=fileName;}
	
};


#define MAX_PASSWORD_LENGTH    255
struct FileItem
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

/////资源文件解密类,主要是读取图片文件。
///// 供使用者调用的类
//class CUnZipRes
//{
//	static std::map<Tstring,FileItem *> mapFile;
//	bool m_bIsFileExist;
//	static unsigned char m_bPassWord[MAX_PASSWORD_LENGTH];
//public:
//	CUnZipRes(){}
//	~CUnZipRes(){}
//	static void ClearMap()
//	{
//		std::map<Tstring,FileItem *>::iterator iter=mapFile.begin();
//		while (iter != mapFile.end())
//		{
//			FileItem * pFileIterm = iter->second;
//			if (pFileIterm != NULL)
//			{
//				delete pFileIterm;
//			}
//			++iter;
//		}
//		mapFile.clear();
//	}
//	
//	static void SetPassWord(char * password)
//	{
//		memset(m_bPassWord,0,MAX_PASSWORD_LENGTH);
//		if(password!=NULL)
//		{
//			memcpy(m_bPassWord,password,min(strlen(password) ,MAX_PASSWORD_LENGTH));
//		}
//	}
//
//	static bool ReadFile(TCHAR * pPath, unsigned char* password)
//	{
//		Tstring strPath(pPath);
//		strPath.Replace("/","\\");
//		strPath.Replace("//","\\");
//		strPath.Replace("\\\\","\\");
//		strPath.Replace("Image","image");
//		//DebugPrintf("传入文件名%s",pPath);
//
//		Tstring strPathTmp=GetAppPath(false);
//		strPathTmp=strPathTmp.Left(strPathTmp.GetLength()-1);  ///<取得当前进程路径
//		int pos=strPath.Find(strPathTmp);
//		Tstring strMap;
//		TCHAR * pchar=NULL;
//		Tstring strP;
//		if(pos==-1)
//		{
//			///判断是上级目录的文件还是当前目录的文件
//			strMap=strPathTmp;
//			int pos=strPath.Find("..\\");
//			Tstring strFileName=strPath;
//			if(pos!=-1)  ///<如果是上级目录的文件，则在当前进程目录的路径下除去最后面一个文件夹
//			{
//				int  posMap=strPathTmp.ReverseFind('\\');
//				strMap=strPathTmp.Left(posMap);
//				strFileName=strPath.Right(strPath.GetLength()-3);
//			}
//
//
//			strP=strMap+"\\"+strFileName;     ///<将处理的过的路径再加上文件名为真正文件的全路径
//			pchar=strP.GetBuffer(strP.GetLength() + 1);
//		}else
//		{
//			pchar=strPath.GetBuffer(strPath.GetLength()+1);     ///<如果是传的全路径，则直接处理
//			int  posMap=strPath.ReverseFind('\\');
//			strMap=strPath.Left(posMap);
//		}	
//		
//		///<如果是已经读取过的文件，则不再读取了
//		if(mapFile.find(strMap)!=mapFile.end() )
//		{
//			return true;
//		}
//
//		unsigned char digest[MAX_PASSWORD_LENGTH];
//		ZeroMemory(digest,MAX_PASSWORD_LENGTH);
//		encryptPW(password,strlen((char *)password),digest);
//		FileItem * pFileItem=new FileItem();
//		UINT len;
//		pFileItem->_pBufFile = DecryptZipFile(pchar,(char*)digest,&len);
//		pFileItem->_nFileLen=len;
//
//		if(len==0||pFileItem->_pBufFile==NULL)
//		{
//			return false;
//		}
//		memcpy(pFileItem->_digest,digest,MAX_PASSWORD_LENGTH);
//		mapFile[strMap]=pFileItem;
//
//		return true;
//	}
//
//	static bool IsFileExist(Tstring fileName)
//	{
//		//Tstring str=GetAppPath(false);
//		//Tstring  strName=str+fileName;
//		WIN32_FIND_DATA  d;
//		HANDLE  hd= FindFirstFile(fileName,&d);
//		bool bRet = (hd != INVALID_HANDLE_VALUE);
//		FindClose(hd);
//		return bRet;
//	}
//
//	static IStream * LoadFileResFromZip(TCHAR *fileName)
//	{
//		if(fileName==NULL)
//		{
//			return NULL;
//		}
//		Tstring strFile(fileName);
//		//DebugPrintf("传入全路径%s",fileName);
//		strFile.Replace("/","\\");  ///<将斜干统一成"\"
//		strFile.Replace("//","\\");
//		strFile.Replace("\\\\","\\");
//		strFile.Replace("Image","image");
//		//DebugPrintf("图片名称%s",fileName);
//
//		std::map<Tstring,FileItem *>::iterator iter;
//		///先判断是否是全路径，如果是，则直接取路径看在MAP中有没，如果有，则直接读该文件，如果没有，则进行其它判断
//		int posAll=strFile.Find("image");
//		if(posAll==-1)
//		{
//			return NULL;
//		}
//		Tstring strMapAll=strFile.Left(posAll-1);
//		//DebugPrintf("传入中的Image左边部分%s",strMapAll);
//		iter=mapFile.find(strMapAll);
//		TCHAR  * pchar=NULL;
//		Tstring strP;
//		if(iter==mapFile.end())///<如果本来就有该路径，说明传的就的全路径
//		{
//
//			Tstring strPathTmp=GetAppPath(false);   ///<获取当前进程路径
//			int posfile=strFile.Find(strPathTmp);   
//			if(posfile!=-1)///<判断传入路径包含磁盘路径没，若有，则减去
//			{
//				strFile=strFile.Right(strFile.GetLength()-strPathTmp.GetLength());
//			}
//			strPathTmp=strPathTmp.Left(strPathTmp.GetLength()-1);   ///<这是因为取得的当前进程路径最后会有一个“\”，所以长度减一
//			
//			
//
//			///<下面是找出图片所在文件的内存文件名
//			int pos1=strFile.Find("image");
//			strP=strFile.Right(strFile.GetLength()-pos1);
//			pchar=strP.GetBuffer(strP.GetLength() + 1);   ///<先读出文件名，保存
//
//			int pos=strFile.Find("..\\"); 
//			Tstring strMap;
//			if(pos!=-1)  ///<如果是上级目录，则将最后面的一个文件夹除去
//			{
//				int  posMap=strPathTmp.ReverseFind('\\');
//				strMap=strPathTmp.Left(posMap);
//				iter=mapFile.find(strMap);
//
//			}else
//			{
//				Tstring strAll=GetAllPath(strFile); ///<获取全路径
//				int posTmp=strAll.Find("image");
//				strMap=strAll.Left(posTmp-1);
//				iter=mapFile.find(strMap);
//				if(iter==mapFile.end())
//				{
//					Tstring strMapAll=strMap+"\\"+"image.r";
//					TCHAR * pTmp=strMapAll.GetBuffer(strMapAll.GetLength()+1);
//					ReadFile(pTmp,m_bPassWord);
//					iter=mapFile.find(strMap);
//				}
//				//iter=mapFile.find(strPathTmp);
//			} 
//
//			
//		}else
//		{
//			int pos1=strFile.Find("image");
//			strP=strFile.Right(strFile.GetLength()-pos1);
//			pchar=strP.GetBuffer(strP.GetLength() + 1);
//		}
//
//
//		if(iter==mapFile.end())
//		{
//			return NULL;
//		}
//
//		///<如果找到了，则将图片相对路径找出来
//		//int pos1=strFile.Find("image");
//		//Tstring strP=strFile.Right(strFile.GetLength()-pos1);
//		//TCHAR * pchar=strP.GetBuffer(strP.GetLength() + 1);
//		//DebugPrintf("图片相对路径%s",pchar);
//		//Tstring strPath(pchar);
//
//
//
//		void * pBufFile=iter->second->_pBufFile;
//		UINT len=iter->second->_nFileLen;
//		ULONG size=0;
//		unsigned char digest[MAX_PASSWORD_LENGTH]={0};
//		memcpy(digest,iter->second->_digest,MAX_PASSWORD_LENGTH);
//		char *pBuf = NULL;
//		/*int pos=strFile.Find("image");*/
//		//strFile=strFile.Right(strFile.GetLength()-pos);
//		//TCHAR * pcharPath=strFile.GetBuffer(strPath.GetLength() + 1);  ///<剥去当前EXE的路径，取出相对路径来找图片内存
//		if(len==0||pBufFile==NULL)
//		{
//			return NULL;
//		}
//		size = LoadFileFromZip(pchar,pBufFile,len,&pBuf,(char*)digest);
//
//		//DebugPrintf("读取图片大小%d",size);
//		if(size==0 || pBuf==NULL)
//		{
//			return NULL;
//		}
//		IStream *pStream = NULL;
//		ToIStream(pBuf,size,pStream);
//		if(pBuf!=NULL)
//		{
//			delete []pBuf;
//			pBuf=NULL;
//		}
//		return pStream;
//	}
//	
//	static char * LoadFileBufferFromZip(TCHAR *fileName,int & BuffLen)
//	{
//		if(fileName==NULL)
//		{
//			return NULL;
//		}
//		Tstring strFile(fileName);
//		//DebugPrintf("传入全路径%s",fileName);
//		strFile.Replace("/","\\");  ///<将斜干统一成"\"
//		strFile.Replace("//","\\");
//		strFile.Replace("\\\\","\\");
//		strFile.Replace("Image","image");
//		//DebugPrintf("图片名称%s",fileName);
//
//		std::map<Tstring,FileItem *>::iterator iter;
//		///先判断是否是全路径，如果是，则直接取路径看在MAP中有没，如果有，则直接读该文件，如果没有，则进行其它判断
//		int posAll=strFile.Find("image");
//		if(posAll==-1)
//		{
//			return NULL;
//		}
//		Tstring strMapAll=strFile.Left(posAll-1);
//		//DebugPrintf("传入中的Image左边部分%s",strMapAll);
//		iter=mapFile.find(strMapAll);
//		TCHAR  * pchar=NULL;
//		Tstring strP;
//		if(iter==mapFile.end())///<如果本来就有该路径，说明传的就的全路径
//		{
//
//			Tstring strPathTmp=GetAppPath(false);   ///<获取当前进程路径
//			int posfile=strFile.Find(strPathTmp);   
//			if(posfile!=-1)///<判断传入路径包含磁盘路径没，若有，则减去
//			{
//				strFile=strFile.Right(strFile.GetLength()-strPathTmp.GetLength());
//			}
//			strPathTmp=strPathTmp.Left(strPathTmp.GetLength()-1);   ///<这是因为取得的当前进程路径最后会有一个“\”，所以长度减一
//
//
//
//			///<下面是找出图片所在文件的内存文件名
//			int pos1=strFile.Find("image");
//			strP=strFile.Right(strFile.GetLength()-pos1);
//			pchar=strP.GetBuffer(strP.GetLength() + 1);   ///<先读出文件名，保存
//
//			int pos=strFile.Find("..\\"); 
//			Tstring strMap;
//			if(pos!=-1)  ///<如果是上级目录，则将最后面的一个文件夹除去
//			{
//				int  posMap=strPathTmp.ReverseFind('\\');
//				strMap=strPathTmp.Left(posMap);
//				iter=mapFile.find(strMap);
//
//			}else
//			{
//				Tstring strAll=GetAllPath(strFile); ///<获取全路径
//				int posTmp=strAll.Find("image");
//				strMap=strAll.Left(posTmp-1);
//				iter=mapFile.find(strMap);
//				if(iter==mapFile.end())
//				{
//					Tstring strMapAll=strMap+"\\"+"image.r";
//					TCHAR * pTmp=strMapAll.GetBuffer(strMapAll.GetLength()+1);
//					ReadFile(pTmp,m_bPassWord);
//					iter=mapFile.find(strMap);
//				}
//				//iter=mapFile.find(strPathTmp);
//			} 
//
//
//		}else
//		{
//			int pos1=strFile.Find("image");
//			strP=strFile.Right(strFile.GetLength()-pos1);
//			pchar=strP.GetBuffer(strP.GetLength() + 1);
//		}
//
//
//		if(iter==mapFile.end())
//		{
//			return NULL;
//		}
//
//		///<如果找到了，则将图片相对路径找出来
//		//int pos1=strFile.Find("image");
//		//Tstring strP=strFile.Right(strFile.GetLength()-pos1);
//		//TCHAR * pchar=strP.GetBuffer(strP.GetLength() + 1);
//		//DebugPrintf("图片相对路径%s",pchar);
//		//Tstring strPath(pchar);
//
//
//
//		void * pBufFile=iter->second->_pBufFile;
//		UINT len=iter->second->_nFileLen;
//		ULONG size=0;
//		unsigned char digest[MAX_PASSWORD_LENGTH]={0};
//		memcpy(digest,iter->second->_digest,MAX_PASSWORD_LENGTH);
//		char *pBuf = NULL;
//		/*int pos=strFile.Find("image");*/
//		//strFile=strFile.Right(strFile.GetLength()-pos);
//		//TCHAR * pcharPath=strFile.GetBuffer(strPath.GetLength() + 1);  ///<剥去当前EXE的路径，取出相对路径来找图片内存
//		if(pBufFile==NULL || len==0)
//		{
//			return NULL;
//		}
//		size = LoadFileFromZip(pchar,pBufFile,len,&pBuf,(char*)digest);
//		
//		//DebugPrintf("读取图片大小%d",size);
//		if(size==0 || pBuf==NULL)
//		{
//			return NULL;
//		}
//		BuffLen=size;
//		return pBuf;
//	}
//
//
//
//	static Tstring GetAppPath(bool bFource=true)
//	{
//		bFource = true;
//		if(!bFource)
//		{
//			TCHAR lpBuffer[1024];
//			ZeroMemory(lpBuffer,sizeof(lpBuffer));
//			GetCurrentDirectory(1024,lpBuffer);
//			Tstring ss=lpBuffer;
//			ss += "\\";
//			return ss;
//		}
//		else
//		{
//			TCHAR szModuleName[MAX_PATH];
//			ZeroMemory(szModuleName,sizeof(szModuleName));
//			DWORD dwLength=GetModuleFileName(AfxGetInstanceHandle(),szModuleName,sizeof(szModuleName));
//			Tstring filepath=szModuleName;
//			Tstring path;
//			int nSlash = filepath.ReverseFind(_T('/'));
//			if (nSlash == -1)
//				nSlash = filepath.ReverseFind(_T('\\'));
//			if (nSlash != -1 && filepath.GetLength() > 1)
//			{
//				path = filepath.Left(nSlash+1);
//				SetCurrentDirectory(path);
//				return path;
//			}
//			else
//			{
//				TCHAR lpBuffer[1024];
//				ZeroMemory(lpBuffer,sizeof(lpBuffer));
//				GetCurrentDirectory(1024,lpBuffer);
//				Tstring ss=lpBuffer;
//				ss += "\\";
//				return ss;
//			}
//
//		}
//	}
//	
//	static Tstring GetAllPath(Tstring strCurPath)
//	{
//		Tstring strPathTmp=GetAppPath(false);   ///<获取当前进程路径
//		strPathTmp=strPathTmp.Left(strPathTmp.GetLength()-1);   ///<这是因为取得的当前进程路径最后会有一个“\”，所以长度减一
//		
//		int posCur=strCurPath.Find(strPathTmp);
//		
//		if(posCur!=-1)
//		{
//			strCurPath.Replace("\\\\","\\");
//			return strCurPath;
//		}else
//		{
//			int pos=strCurPath.Find("..\\");  
//			if(pos!=-1)  ///<如果是上级目录，则将最后面的一个文件夹除去
//			{
//				int  posMap=strPathTmp.ReverseFind('\\');
//				strPathTmp=strPathTmp.Left(posMap);
//			}
//			int posfile=strCurPath.Find(".\\");
//			if(posfile!=-1)
//			{
//				strCurPath=strCurPath.Right(strCurPath.GetLength()-posfile-1);
//			}
//			Tstring strAll=strPathTmp+"\\"+strCurPath;
//
//			strAll.Replace("\\\\","\\");
//			return strAll;
//		}
//	}
//};
