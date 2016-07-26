
#ifndef _BZZLIB_H
#define _BZZLIB_H

#include "Encrypt.h"

// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 BZZLIB_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// BZZLIB_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef BZZLIB_EXPORTS
#define BZZLIB_API __declspec(dllexport)
#else
#define BZZLIB_API __declspec(dllimport)
#endif

//BZZLIB_API ULONG LoadFileFromZip(TCHAR *fileName,TCHAR *ZipFile,char **pBuf,char*password=NULL);
//BZZLIB_API ULONG LoadFileFromZip(char *fileName,void *BufZip,unsigned int lenBufZip,char **pBuf,char*password=NULL);
BZZLIB_API void * DecryptZipFile(char *fileName,char*passWord,unsigned long *pLen);
BZZLIB_API HRESULT ToIStream(void *pBuf,ULONG cb,IStream * & pStream);
#endif
