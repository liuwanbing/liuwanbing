#pragma once

#pragma comment(lib, "../Client/Fish/bass/bass.lib")


#include "bass/bass.h"
#include <map>
#include <assert.h>

///声音资源结构体
typedef struct{
	///HSTREAM
	HSTREAM hStream;
	///是否循环播放, 注意：此参数不能与bDeleteWhenStop同时为true
	bool bLoop;
} tagMusicStream;


/**
* @brief 声音资源管理类
* @detail 此类以单件模式实现的,可以管理程序中所有声音资源的播放、暂停等操作
* @author Ettan
*/
class CBassMusicManager
{
public:
	///析构函数
	~CBassMusicManager(void);
	//
    CBassMusicManager(void);
	/**
	* @brief 获取声音管理类的指针
	*/
	static CBassMusicManager* GetInstance();

	///设置窗口句柄
	bool Init(HWND hWnd);

	///释放资源
	void ReleaseAll();
	
	/**
	* @brief 从文件中加载资源
	* @param nUid	资源标志ID，根据此标志进行以后相应的操作，如播放，暂停等
	* @param lpszFileName	文件路径
	* @param bLoop	是否循环，默认：false
	* @param bDeleteWhenStop	播放结束后是否立即删除资源，默认true
	*/
	bool LoadFromFile(const int nUid, const LPCSTR lpszFileName, const bool bLoop=false);

	/**
	* @brief 播放指定标志的声音
	* @param nUid 声音标志ID
	* @param bDeleteWhenStop 播放完成后是否删除资源，默认false
	*/
	bool Play(int nUid, bool bDeleteWhenStop=true);

	/**
	* @brief 从头重新播放指定标志的声音
	* @param nUid 声音标志ID
	* @param bDeleteWhenStop 播放完成后是否删除资源，默认false
	*/
	bool RePlay(int nUid, bool bDeleteWhenStop=true);

	///暂停
	bool Pause(int nUid);

	///停止
	bool Stop(int nUid);

	///是否在播放
	bool IsPlaying(int nUid);

	/**
	* @brief 设置声音大小
	* @param nVolumn 声音大小，取值范围0~100
	*/
	bool SetVolumn(int nVolumn);

protected:
	///回调函数
	static void CALLBACK MySyncProc(HSYNC handle, DWORD channel, DWORD data, void *user);

	bool _Play(int nUid, bool bReplay=false, bool bDeleteWhenStop=true);

protected:
	///构造函数
	

private:
	///HSTREAM容器
	static std::map<int, tagMusicStream> _mapPlayingStream;	
	///需要释放HSTREAM资源的容器
	static std::map<int, tagMusicStream> _mapNeedDeleteStream;
	///类指针
	static CBassMusicManager* _instance;
	///窗口指针
	HWND _hWnd;
};
