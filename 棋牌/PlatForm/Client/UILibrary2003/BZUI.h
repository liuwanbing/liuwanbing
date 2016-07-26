#ifndef _BZW_UI_LIBRARY_HEADER_FRED_HUANG_2008_07_ 
#define _BZW_UI_LIBRARY_HEADER_FRED_HUANG_2008_07_ 
// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 BZUI_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// BZUI_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef BZUI_EXPORTS
#define BZUI_API __declspec(dllexport)
#else
#define BZUI_API __declspec(dllimport)
#endif

// 此类是从 BZUI.dll 导出的
class BZUI_API CBZUI {
public:
	CBZUI(void);
	// TODO: 在此添加您的方法。
};

extern BZUI_API int nBZUI;

BZUI_API int fnBZUI(void);


/************************************************************************
Function	:PlayMoiveBMP
Notes		:播放BMP动画，该动画是由多张BMP拼接起来的
Parameter	:
pWnd		:动画播放的父窗口
szFilename	:BMP文件名
bRgn		:是否要去除背景，注意，背景色一定是RGB(255,0,255)，不再给配置
nFrames		:该动画有多少帧
nInterval	:每帧之间间隔多少时间，单位为ms
如果nFrames=1 或nInterval=0，则只无动画，只显示一张整图或第一帧
nPlayMode	:播放模式
0		:播放完一遍就结束
1		:播放完停在最后一帧，直到使用 BZEngineStopBMP(DWORD)
2		:循环播放，直到使用 BZEngineStopBMP(DWORD)
************************************************************************/
BZUI_API DWORD BZUIPlayBMP(CWnd *pWnd,LPCSTR szFilename,bool bRgn,int nFrames,int nInterval,int nPlayMode);

BZUI_API bool BZUIDestoryMoive(DWORD dwHandle);

BZUI_API void BZUIDestoryAll();

BZUI_API void BZUIInitial();
BZUI_API void BZUIShutDown();


#endif