#include "stdafx.h"
#include "PlayVideo.h"
#include "ClientView.h"
 
CPlayVideo*  CPlayVideo::m_pPlayVideo=NULL;

CPlayVideo::CPlayVideo()
{
	m_pPlayVideo=this;
}
CPlayVideo::~CPlayVideo()
{ 
    if( (NULL != m_pPlayVideo) && (NULL != m_pPlayVideo->m_hWnd))
        Video_Release();
	m_pPlayVideo = NULL;
};

CPlayVideo*  CPlayVideo::CreatePlayVideo()
{
	 
	if(NULL != m_pPlayVideo)
		return m_pPlayVideo;
	else
	{
        if (NULL == m_pPlayVideo->m_hWnd)
        {
            m_pPlayVideo = NULL;
        }
		 return NULL;
	}
}

bool  CPlayVideo::Video_Initialize( ST_INITVIDEO& stInitVideo)
{
	return __super::Video_Initialize( stInitVideo);
}

void	CPlayVideo::Video_Release()
{
	__super::Video_Release();   

}
 

/*****************************************
	(IN)  name, 玩家昵称;
	(OUT) rect, 此玩家视频窗口坐标;
	当成功输出　rect时，返回true,否则false;
******************************************/
#ifdef	VIDEO
bool	CPlayVideo::GetVideoRectByName( CString name, RECT& rect, CString& pic)
{
	CClientPlayView *pPlayView=(CClientPlayView*)m_pParentWnd;
	ASSERT(pPlayView);
	if(pPlayView)
	{
		return pPlayView->GetVideoRect( name, rect, pic);
		
	}
	else
		return false;
	 
}
#endif