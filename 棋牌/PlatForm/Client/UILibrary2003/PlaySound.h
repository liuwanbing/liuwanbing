#pragma once
#ifndef PLAYSOUND_HEAD_H
#define  PLAYSOUND_HEAD_H

#include "PublicDefine.h"
#include<Vfw.h>
// 背景音乐播放类
class EXT_CLASS CPlaySound :public CWnd
{
public:
	CPlaySound(void);
public:
	~CPlaySound(void);

private:
	HWND		hmciwnd;		// MCI窗口
	int			m_count;		// 需要播放的次数
	int			m_playCount;	// 已经播放的次数
	//////////////////////////
	HWND		m_PlayWin;		// 播放结束后, 收到通知的窗口
	int			m_AllCount;		// 总的音乐类型
	int			m_CurPos;		// 当前播放第几个音乐
	int			m_PlayMode;		// 播放模式
	TCHAR		m_filename[MAX_PATH];	// ini文件路径
	TCHAR		m_key[20];				// ini里的关键字

public:
	// 播放filename, count次
	BOOL PlayMusic(HWND hwnd,TCHAR *filename,int count=-1);
	// 根据filename ini文件里的key section, 来播放音乐
	BOOL PlayMusic(HWND hwnd,TCHAR *filename,TCHAR *key,bool IsPlay);
	// 结束播放
	bool StopMusic();
	// 开始播放
	bool Play(bool IsFirst);
	// 创建窗口
	bool CreateWnd(CWnd * pParentWnd, UINT uWndID);

public:
	DECLARE_MESSAGE_MAP()

public:	
	// 回调函数
	afx_msg LRESULT OnNotifyMode(WPARAM wParam,LPARAM lParam);	
};

#endif