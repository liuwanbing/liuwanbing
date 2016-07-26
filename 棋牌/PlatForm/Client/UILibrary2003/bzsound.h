#ifndef _BZSOUND_INCLUDE_FILE_2008_07_FRED_HUANG_
#define _BZSOUND_INCLUDE_FILE_2008_07_FRED_HUANG_
// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 BZSOUND_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// BZSOUND_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef BZSOUND_EXPORTS
#define BZSOUND_API __declspec(dllexport)
#else
#define BZSOUND_API __declspec(dllimport)
#endif

#define BZSOUND_STOP		0
#define BZSOUND_PLAYING		1
#define BZSOUND_PAUSE		2

BZSOUND_API int fnBZSound(void);

/*************************************************************************
Function	:BZSoundInitial()
Note		:使用前要初始化
Author		:Fred Huang 2008-07-22
*************************************************************************/
BZSOUND_API void BZSoundInitial();

/*************************************************************************
Function	:BZSoundPlay
Note		:播放声音
Author		:Fred Huang 2008-07-22
Parameter	:
CWnd 		*:需要一个窗口句柄来播放声音
szFilename	:声音文件名
nLoop		:是否循环播放

Return		:
int			:返回一个ID号，后续的操作根据该ID号来控制
*************************************************************************/
BZSOUND_API int	BZSoundPlay(CWnd *pWnd,char *szFilename,DWORD dwPlayEnd,int nLoop=1);

/*************************************************************************
Function	:BZSoundStop
Note		:停止播放
Author		:Fred Huang 2008-07-22
Parameter	:
nSoundID			:由BZSoundPlay()返回的ID号
*************************************************************************/
BZSOUND_API void BZSoundStop(int nSoundID);
/*************************************************************************
Function	:BZSoundStopAll
Note		:停止播放所有声音
Author		:Fred Huang 2008-07-22
*************************************************************************/
BZSOUND_API void BZSoundStopAll();

/*************************************************************************
Function	:BZSoundGetVolume
Note		:取声音音量
Author		:Fred Huang 2008-07-22
Parameter	:
nSoundID	:由BZSoundPlay()返回的ID号
*************************************************************************/
BZSOUND_API long BZSoundGetVolume(int nSoundID);

/*************************************************************************
Function	:SetVolume
Note		:取声音音量
Author		:Fred Huang 2008-07-22
Parameter	:
nSoundID	:由BZSoundPlay()返回的ID号
nVolume		:音量大小
*************************************************************************/
BZSOUND_API void BZSoundSetVolume(int nSoundID,long nVolume);

/*************************************************************************
Function	:BZSoundPause
Note		:暂停播放声音
Author		:Fred Huang 2008-07-23
Parameter	:
nSoundID	:由BZSoundPlay()返回的ID号
*************************************************************************/
BZSOUND_API void BZSoundPause(int nSoundID);

/*************************************************************************
Function	:BZSoundContinue
Note		:继续播放被暂停的声音
Author		:Fred Huang 2008-07-23
Parameter	:
nSoundID	:由BZSoundPlay()返回的ID号
*************************************************************************/
BZSOUND_API void BZSoundContinue(int nSoundID);



/*************************************************************************
Function	:BZSoundCreateList
Note		:设置一个列表方式播放通道
Author		:Fred Huang 2008-07-23
Parameter	:
CWnd *		:需要一个窗口句柄来播放声音
dwPlayEnd	:播放完了后给主窗口的消息
Return		:
int			:返回一个ID号，后续的操作根据该ID号来控制
*************************************************************************/
BZSOUND_API int BZSoundCreateList(CWnd *pWnd,DWORD dwPlayEndMsg);


/*************************************************************************
Function	:BZSoundAddList
Note		:在播放列表中，添加声音文件
Author		:Fred Huang 2008-07-23
Parameter	:
nSoundID	:
szFilename	:文件名
Return		:
bool		:是否添加成功
*************************************************************************/
BZSOUND_API bool BZSoundAddList(int nSoundID,char *szFilename);

/*************************************************************************
Function	:BZSoundDestoryList
Note		:清除播放列表，并释放资源
Author		:Fred Huang 2008-07-23
Parameter	:
nSoundID	:
*************************************************************************/
BZSOUND_API void BZSoundDestoryList(int nSoundID);


/*************************************************************************
Function	:BZSoundPlayList
Note		:播放列表中的声音
Author		:Fred Huang 2008-07-23
Parameter	:
nSoundID	:
*************************************************************************/
BZSOUND_API void BZSoundPlayList(int nSoundID);

#endif