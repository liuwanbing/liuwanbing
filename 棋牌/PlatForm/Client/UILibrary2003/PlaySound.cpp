#include "StdAfx.h"
#include "PlaySound.h"

BEGIN_MESSAGE_MAP(CPlaySound, CWnd)
	ON_MESSAGE(MCIWNDM_NOTIFYMODE, OnNotifyMode)
END_MESSAGE_MAP()

CPlaySound::CPlaySound(void)
{
	m_CurPos=-1;
	m_PlayMode=-1;
	lstrcpy(m_filename,"");
}

CPlaySound::~CPlaySound(void)
{
	if(hmciwnd)
	{		
		MCIWndStop(hmciwnd);
		MCIWndDestroy(hmciwnd);
		hmciwnd=NULL;		
	}
}
//hwnd 播放完一遍后接收通知的窗口
//如果你指定了窗口,则要你自己控制播放次数,-1表示无限次播放,0或1只放一次
//你也可以直接调用LRESULT  CPlaySound::OnNotifyMode(WPARAM wParam,LPARAM lParam), 进行下一次播放
//filenam 要播放的文件路径,格式为mp3或wav,wma未测试
//count播放次数
BOOL CPlaySound::PlayMusic(HWND hwnd,TCHAR *filename,int count)
{
	if(!filename)
		return false;
	if(!hwnd)
		hwnd=m_hWnd;
	m_playCount=0;
	m_count=count;
	m_PlayMode=-1;
	StopMusic();
	hmciwnd=MCIWndCreate(hwnd,AfxGetInstanceHandle(),WS_CHILD|MCIWNDF_NOMENU|MCIWNDF_NOOPEN|MCIWNDF_NOPLAYBAR|MCIWNDF_NOERRORDLG,filename);
	if(hmciwnd)
	{
		::ShowWindow(hmciwnd,false);
		MCIWndPlay(hmciwnd);
	}
	else
	{
	}	
	return true;
}
//hwnd 作用和前面一样
//filename ini文件路径
//配置文件的播放,请参考config.ini里面的注释
//如果前面已经传递过一次参数了, 则可以为空, 如果为空key必须也为空,表示用前面的参数继续播放
//key ini文件的session
//如果前面已经传递过一次参数了, 则可以为空, 如果为空filename必须也为空,表示用前面的参数继续播放
//IsPlay 是否马上开始播放
BOOL CPlaySound::PlayMusic(HWND hwnd,TCHAR *filename,TCHAR *key,bool IsPlay)
{
	if(!hwnd)
		hwnd=m_hWnd;
	if(filename&&key)
	{
		lstrcpy(m_filename,filename);
		lstrcpy(m_key,key);
		CINIFile f(m_filename);
		CString key=m_key;
		m_PlayMode=f.GetKeyVal(key,"playmode",0);
		m_AllCount=f.GetKeyVal(key,"count",0);
		if(m_AllCount<=0)
		{
			m_PlayMode=-1;
			return false;
		}
		if(m_CurPos>=m_AllCount)
			m_CurPos=-1;
	}
	if(lstrlen(m_filename)<=0)
		return false;
	m_PlayWin=hwnd;
	if(IsPlay)
	{
		Play(true);
	}
	return true;
}

bool CPlaySound::StopMusic()
{
	if(hmciwnd)
	{		
		MCIWndStop(hmciwnd);
		MCIWndDestroy(hmciwnd);
		hmciwnd=NULL;		
	}
	return true;
}

bool CPlaySound::Play(bool IsFirst)
{
	if(m_PlayMode==-1)
	{
		m_playCount++;
		if(-1==m_count)
		{
			MCIWndHome(hmciwnd);
			MCIWndPlay(hmciwnd);
			return true;
		}
		else if(m_playCount<m_count)
		{
			MCIWndHome(hmciwnd);
			MCIWndPlay(hmciwnd);
			return true;
		}
		return false;
	}
	if(m_AllCount<=0)
	{
		return false;
	}
	CString list;				
	CINIFile f(m_filename);
	CString key=m_key;
	CString soundname;
	switch(m_PlayMode)
	{
	case 0:
		//重复播放
		if(!IsFirst)
		{
			MCIWndHome(hmciwnd);
			MCIWndPlay(hmciwnd);
			return true;
		}
		else
		{
			soundname=f.GetKeyVal(m_key,"list0","");
		}		
		break;
	case 1:
		m_CurPos++;
		if(m_CurPos==m_AllCount)
		{
			StopMusic();
			return true;
		}
		list.Format("list%d",m_CurPos);
		soundname=f.GetKeyVal(m_key,list,"");
		break;
	case 2:
		list.Format("list%d",rand()%m_AllCount);
		soundname=f.GetKeyVal(m_key,list,"");
		break;
	default:
		break;
	}
	StopMusic();
	hmciwnd=MCIWndCreate(m_PlayWin,AfxGetInstanceHandle(),WS_CHILD|MCIWNDF_NOTIFYMODE|MCIWNDF_NOMENU|MCIWNDF_NOOPEN|MCIWNDF_NOPLAYBAR|MCIWNDF_NOERRORDLG,soundname);
	if(hmciwnd)
	{
		MCIWndPlay(hmciwnd);
	}
	return true;
}

LRESULT  CPlaySound::OnNotifyMode(WPARAM wParam,LPARAM lParam)
{
	if(MCI_MODE_STOP==lParam)
		Play(false);
	return true;
}

//建立函数
bool CPlaySound::CreateWnd(CWnd * pParentWnd, UINT uWndID)
{
	//建立窗口
	if (Create(AfxRegisterWndClass(CS_DBLCLKS|CS_HREDRAW|CS_VREDRAW,
			   AfxGetApp()->LoadStandardCursor(IDC_ARROW),
			   (HBRUSH)GetStockObject(NULL_BRUSH),NULL),
			   NULL,WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,
			   CRect(0,0,0,0),pParentWnd,uWndID,NULL)==FALSE)
			   return false;
	ShowWindow(false);
	return true;
}
