#pragma once
#include   "VideoCtrl.h"

class CPlayVideo : public CVideoCtrl
{
public:
	CPlayVideo();
	virtual ~CPlayVideo();
	virtual  bool			Video_Initialize( ST_INITVIDEO& stInitVideo);
	virtual  void			Video_Release();
#ifdef	VIDEO
	virtual	 bool			GetVideoRectByName( CString name, RECT& rect, CString& pic);
#endif
	static	 CPlayVideo*    CreatePlayVideo();
	static   CPlayVideo*	GetPlayVideo(){return m_pPlayVideo;}
private:
	static   CPlayVideo*    m_pPlayVideo;
	 
};