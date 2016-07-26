// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 RECORDMODULE_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// RECORDMODULE_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef RECORDMODULE_EXPORTS
#define RECORDMODULE_API __declspec(dllexport)
#else
#define RECORDMODULE_API __declspec(dllimport)
#endif

// 此类是从 RecordModule.dll 导出的
class RECORDMODULE_API CRecordModule {
public:
	CRecordModule(void);
	// TODO: 在此添加您的方法。
};

extern RECORDMODULE_API int nRecordModule;

RECORDMODULE_API int fnRecordModule(void);
#include "Recorder.h"
//#include "../../include/interface.h"
//定义导出的函数
extern "C" __declspec(dllexport) IRecorderForPlatform* GetRecordForPlatForm()
{ 
	return new CRecorder();
}
extern "C" __declspec(dllexport) IRecorderForPlayer* GetRecordForPlayer()
{ 
	return new CRecorder();
}