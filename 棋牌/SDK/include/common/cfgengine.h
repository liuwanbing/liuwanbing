// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 CFGENGINE_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// CFGENGINE_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#pragma once
#ifdef CFGENGINE_EXPORTS
#define CFGENGINE_API __declspec(dllexport)
#else
#define CFGENGINE_API __declspec(dllimport)
#endif


// 此类是从 CfgEngine.dll 导出的
class CFGENGINE_API CCfgEngine {
public:
	CCfgEngine(void);
	// TODO: 在此添加您的方法。
};

extern CFGENGINE_API int nCfgEngine;

CFGENGINE_API int fnCfgEngine(void);

extern CFGENGINE_API DWORD cfgOpenFile(LPCTSTR szFileName,LPCTSTR szPassword=TEXT(""));

extern CFGENGINE_API LPCTSTR cfgGetValue(DWORD hHandle,LPCTSTR szSection,LPCTSTR szKeyName,LPCTSTR szDefault);

extern CFGENGINE_API long	cfgGetValue(DWORD hHandle,LPCTSTR szSection,LPCTSTR szKeyName,long lDefault);

extern CFGENGINE_API int	cfgGetValue(DWORD hHandle,LPCTSTR szSection,LPCTSTR szKeyName,int lDefault);

extern CFGENGINE_API LPCTSTR	cfgGetMemo(DWORD hHandle);

extern CFGENGINE_API void		cfgSetMemo(DWORD hHandle,LPCTSTR memo);

extern CFGENGINE_API LPCTSTR	cfgGetSectionMemo(DWORD hHandle,LPCTSTR szSection);

extern CFGENGINE_API void  cfgSetSectionMemo(DWORD hHandle,LPCTSTR szSection,LPCTSTR memo);

extern CFGENGINE_API LPCTSTR	cfgGetKeyMemo(DWORD hHandle,LPCTSTR szSection,LPCTSTR szKeyName);

extern CFGENGINE_API void	cfgSetKeyMemo(DWORD hHandle,LPCTSTR szSection,LPCTSTR szKeyName,LPCTSTR memo);

extern CFGENGINE_API void	cfgClose(DWORD hHandle);

extern CFGENGINE_API void	cfgCloseAll();

extern CFGENGINE_API bool	cfgSetValue(DWORD hHandle,LPCTSTR szSection,LPCTSTR szKeyName,LPCTSTR szValue,int attribValue=0);

extern CFGENGINE_API bool	cfgSetValue(DWORD hHandle,LPCTSTR szSection,LPCTSTR szKeyName,long lDefault);

extern CFGENGINE_API bool	cfgEraseKey(DWORD hHandle,LPCTSTR szSection,LPCTSTR szKeyName);

extern CFGENGINE_API bool   cfgEraseSection(DWORD hHandle,LPCTSTR szSection);

extern CFGENGINE_API POSITION cfgFindFirstSectionPosition(DWORD hHandle);

extern CFGENGINE_API LPCTSTR cfgFindNextSection(DWORD hHandle,POSITION &pos);

extern CFGENGINE_API POSITION cfgFindFistKeyPosition(DWORD hHandle,LPCTSTR szSection);

extern CFGENGINE_API void cfgFindNextKey(DWORD hHandle,LPCTSTR szSection,POSITION &pos,LPCTSTR &szKeyName,LPCTSTR &szKeyValue,int &nKeyAttrib);

extern CFGENGINE_API void cfgEditSectionName(DWORD hHandle,LPCTSTR szOldSection,LPCTSTR szSection);

extern CFGENGINE_API void cfgEditKeyName(DWORD hHandle,LPCTSTR szSection,LPCTSTR szOldKey,LPCTSTR szKey);

extern CFGENGINE_API bool cfgExportIni(DWORD hHandle,LPCTSTR iniFile);

extern CFGENGINE_API bool	cfgSetPassword(DWORD hHandle,LPCTSTR szPassword);