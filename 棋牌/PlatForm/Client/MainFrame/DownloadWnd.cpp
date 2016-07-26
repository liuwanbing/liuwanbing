#include "stdafx.h"
#include "DownloadWnd.h"
#include <process.h>
#include <Shlobj.h>
#include <io.h>
#include <atlconv.h>
#include <Windows.h>
#include <ShellAPI.h>
#include <Shlwapi.h>

#include "MainRoomEx.h"

using namespace BzDui;
#pragma comment(lib, "Shlwapi.lib")

///////////////////////////////////////////////////////////////////////////////////////
DECLARE_HANDLE(HZIP);	// An HZIP identifies a zip file that has been opened
typedef DWORD ZRESULT;
typedef struct
{ 
	int index;                 // index of this file within the zip
	char name[MAX_PATH];       // filename within the zip
	DWORD attr;                // attributes, as in GetFileAttributes.
	FILETIME atime,ctime,mtime;// access, create, modify filetimes
	long comp_size;            // sizes of item, compressed and uncompressed. These
	long unc_size;             // may be -1 if not yet known (e.g. being streamed in)
} ZIPENTRY;

#define OpenZip OpenZipU
#define CloseZip(hz) CloseZipU(hz)
extern HZIP OpenZipU(void *z,unsigned int len,DWORD flags);
extern ZRESULT CloseZipU(HZIP hz);

#define GetZipItem GetZipItemA
#define FindZipItem FindZipItemA
extern ZRESULT GetZipItemA(HZIP hz, int index, ZIPENTRY *ze);
extern ZRESULT FindZipItemA(HZIP hz, const TCHAR *name, bool ic, int *index, ZIPENTRY *ze);
extern ZRESULT UnzipItem(HZIP hz, int index, void *dst, unsigned int len, DWORD flags);

#define ZIP_OK		 0
#define ZIP_HANDLE   1
#define ZIP_FILENAME 2
#define ZIP_MEMORY   3


#define IDT_DOWNLOAD		1	//下载文件
#define WM_MAKEFILES		WM_USER+1	//转储文件

#define DOWNLOAD_PERCENT 1000	//下载进度

//从大厅拷贝到游戏文件夹的文件
const std::string str_copy_array[] = {_T("Audio.dll"), _T("BzDui.dll"), _T("BZAnimaPlay.dll"), _T("BZSound.dll"), _T("FCRTL.dll"), _T("fmodex.dll"), _T("Frame.dll"), _T("Graph.dll"), 
	_T("GdiPlus.dll"), _T("irrKlang.dll"), _T("RecordModule.dll"), _T("UI.dll"), _T("D3dx9d_43.dll") };

CDownloadWnd::CDownloadWnd()
	:m_bDownloading(false)
	,m_pList(NULL)
{
	m_wait_download.clear();
	m_mapNameTask.clear();
}


CDownloadWnd::~CDownloadWnd(void)
{
	m_wait_download.clear();
	m_mapNameTask.clear();
}

LPCTSTR CDownloadWnd::GetWindowClassName() const 
{ 
	return _T("UIFrame");
};

UINT CDownloadWnd::GetClassStyle() const 
{ 
	return UI_CLASSSTYLE_DIALOG; 
};

void CDownloadWnd::OnFinalMessage(HWND hWnd) 
{ 
	m_pm.RemovePreMessageFilter(this);
};

LRESULT CDownloadWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_pm.Init(m_hWnd);
	m_pm.AddPreMessageFilter(this);
	CDialogBuilderCallbackEx_DownLoad cb;
	CDialogBuilder builder;
	CControlUI* pRoot = builder.Create(_T("left\\DownloadWnd.xml"), (UINT)0, &cb, &m_pm);
	ASSERT(pRoot && "Failed to parse XML");
	m_pm.AttachDialog(pRoot);
	m_pm.AddNotifier(this);

	Init();
	return 0;
}

LRESULT CDownloadWnd::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CDownloadWnd::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CDownloadWnd::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CDownloadWnd::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CDownloadWnd::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
	::ScreenToClient(*this, &pt);

	RECT rcClient;
	::GetClientRect(*this, &rcClient);

	RECT rcCaption = m_pm.GetCaptionRect();
	if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
		&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) 
	{
		CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(pt));
		if( pControl && _tcscmp(pControl->GetClass(), _T("ButtonUI")) != 0 )
			return HTCAPTION;
	}

	return HTCLIENT;
}

LRESULT CDownloadWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CDownloadWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{	
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	switch(uMsg) 
	{
	case WM_CREATE:
		{
			lRes = OnCreate(uMsg, wParam, lParam, bHandled); 
		}
		break;

	case WM_TIMER:
		{
			lRes = OnTimer(uMsg, wParam, lParam, bHandled);
		}
		break;

	case WM_DESTROY:       
		{
			lRes = OnDestroy(uMsg, wParam, lParam, bHandled);
		}
		break;

	case WM_NCACTIVATE:    
		{
			lRes = OnNcActivate(uMsg, wParam, lParam, bHandled);
		}
		break;

	case WM_NCCALCSIZE:    
		{
			lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled);
		}
		break;

	case WM_NCPAINT:       
		{
			lRes = OnNcPaint(uMsg, wParam, lParam, bHandled);
		}
		break;

	case WM_NCHITTEST:     
		{
			lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled);
		}
		break;
	case WM_MAKEFILES:
		{
			if (m_pList)
			{
				S_DowndloadItme* pItem = m_pList->GetItem(wParam);
				if (pItem)
				{
					pItem->uiProgress = lParam;
					m_pList->UpdateGame(pItem);
				}

				//如果下载完成则更新游戏列表里面的游戏图标
				if (pItem->e_state == E_OK)
				{
					GetMainRoom()->m_pGameListWnd->m_GameList.UpdateAllIcon();
				}
			}
		}break;
	default:
		bHandled = FALSE;
	}

	if(bHandled)
	{
		return lRes;
	}
	
	if(32769!=uMsg && m_pm.MessageHandler(uMsg,wParam,lParam,lRes))
	{
		return lRes;
	}

	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CDownloadWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	if( uMsg == WM_KEYDOWN ) 
	{
		if( wParam == VK_RETURN ) 
		{
			return true;
		}
		else if( wParam == VK_ESCAPE ) 
		{
			return true;
		}
	}
	return false;
}

void CDownloadWnd::Init()
{
	m_pList = static_cast<CDownloadListUI*>(m_pm.FindControl(_T("DownloadList")));
}

void CDownloadWnd::Notify(TNotifyUI& msg)
{
	if (msg.sType == _T("click"))
	{
		if (msg.pSender->GetName() == _T("closebtn"))
		{
			Close();
		}
	}
}

LRESULT CDownloadWnd::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (IDT_DOWNLOAD == wParam)
	{
		bHandled = TRUE;
		GetDownloadProcess(m_iCurTaskID);
	}
	return 0;
}

void CDownloadWnd::GetDownloadProcess(int iTaskID)
{
	FCHttpRequest *pTask = FindRequest(iTaskID);
	if (!pTask)
	{
		return ;
	}
	int living_ms = pTask->GetRunningTime();
	long iRecived = pTask->GetTotalReceiveByte()/1024 ;
	long iTotalLen = pTask->GetResponseInfo().m_content_length/1024 ;
	S_DowndloadItme* pItem = GetDownloadItemByTask(iTaskID);
	if (pItem)
	{
		//已下载大小
		pItem->fDownSize = iRecived/1024.0f;

		//进度条
		pItem->uiProgress = iRecived*DOWNLOAD_PERCENT/iTotalLen;

		//速度
		pItem->fSpeed = iRecived*1000.f/living_ms;

		m_pList->UpdateGame(pItem);
	}
}

S_DowndloadItme* CDownloadWnd::GetDownloadItemByTask(int iTaskID)
{
	S_DowndloadItme* pDownloadItem = NULL;
	if (m_pList)
	{
		ComNameInfo* pNameInfo = m_mapNameTask[iTaskID];
		if (pNameInfo)
		{
			pDownloadItem = m_pList->GetItem(pNameInfo->uNameID);
		}
	}
	return pDownloadItem;
}

void CDownloadWnd::DownloadNext()
{	
	//下载下一个
	if (!m_wait_download.empty())
	{
		ComNameInfo* pNameInfo = m_wait_download.front();
		m_wait_download.pop_front();
		//组合下载地址
		CString strUrl;
		strUrl.Format(_T("%s/game/%d.zip"), Glb().m_CenterServerPara.m_strDownLoadUpdatepADDR, pNameInfo->uNameID);
		//判断URL地址是否合法
		if (!UrlIs(strUrl, URLIS_URL))
		{
			return ;
		}

		S_DowndloadItme* pItem = m_pList->GetItem(pNameInfo->uNameID);
		if (pItem)
		{
			pItem->e_state = E_ING;
			m_pList->UpdateGame(pItem);
		}

		m_iCurTaskID = AddDownload(strUrl);
		m_mapNameTask[m_iCurTaskID] = pNameInfo;
	}
	else
	{
		m_bDownloading = false;
		Close();
	}
}

bool CDownloadWnd::AddDownloadEx(ComNameInfo* pNameInfo)
{
	if (!pNameInfo || !m_pList)
	{
		return false;
	}

	S_DowndloadItme* pItem = m_pList->GetItem(pNameInfo->uNameID);

	if (!pItem)
	{
		//组合下载地址
		CString strUrl;
		strUrl.Format(_T("%s/game/%d.zip"), Glb().m_CenterServerPara.m_strDownLoadUpdatepADDR, pNameInfo->uNameID);
		//判断URL地址是否合法
		if (!UrlIs(strUrl, URLIS_URL))
		{
			return false;
		}

		m_pList->AddGame(pNameInfo, strUrl);
		pItem = m_pList->GetItem(pNameInfo->uNameID);
		//判断现在是否正在下载
		if (m_bDownloading)
		{			
			if (pItem)
			{
				pItem->e_state = E_WAIT;
				m_pList->UpdateGame(pItem);
			}
			m_wait_download.push_back(pNameInfo);
			return true;
		}
		else
		{
			if (pItem)
			{
				pItem->e_state = E_ING;
				sprintf_s(pItem->szErr, sizeof(pItem->szErr), _T("正在下载"));
				m_pList->UpdateGame(pItem);
			}
			m_bDownloading = true;
			m_iCurTaskID = AddDownload(strUrl);
			m_mapNameTask[m_iCurTaskID] = pNameInfo;
		}
	}
	return true;
}

void CDownloadWnd::OnAfterRequestSend(FCHttpRequest& rTask)
{
	const HTTP_RESPONSE_INFO & resp = rTask.GetResponseInfo();

	S_DowndloadItme* pDownloadItem = GetDownloadItemByTask(rTask.GetTaskID());
	if (pDownloadItem)
	{
		pDownloadItem->fSize = resp.m_content_length*1.0f/1024/1024;
	}
	CString strErr = GetStatusText(rTask);
	if (!strErr.IsEmpty())
	{//无法连接服务器
		if (pDownloadItem)
		{
			pDownloadItem->e_state = E_ERROR;
			sprintf_s(pDownloadItem->szErr, sizeof(pDownloadItem->szErr)-1, strErr);
			m_pList->UpdateGame(pDownloadItem);
		}
		DeleteRequest(rTask.GetTaskID());
		DownloadNext();
	}
	else
	{
		//启动定时器获取下载进度和下载速度		
		pDownloadItem->e_state = E_ING;	//正在下载		
		m_pList->UpdateGame(pDownloadItem);
		::SetTimer(m_hWnd, IDT_DOWNLOAD, 20, NULL);
	}
}

void CDownloadWnd::OnAfterRequestFinish(FCHttpRequest& rTask)
{
	const HTTP_RESPONSE_INFO &r = rTask.GetResponseInfo();
	if (r.m_status_code == HTTP_STATUS_OK)
	{
		if (r.m_content_length)
		{
			//防止下载过程中网络中断导致数据错误
			if (r.m_content_length == rTask.GetTotalReceiveByte())
			{
			}
		}
		else
		{
			//下载大小为0，说明下载的内容可能是网页：HTML,ASP。。。
			if (r.m_final_read_result)
			{
			}
		}
	}
	m_str_recv_data.clear();
	//获取下载完的数据
	rTask.PopReceived(m_str_recv_data);

	KillTimer(m_hWnd, IDT_DOWNLOAD);

	GetDownloadProcess(rTask.GetTaskID());

	unsigned int uiThreadID = 0;
	_beginthreadex(NULL, 0, Thread_Install, this, 0, &uiThreadID);
}

CString CDownloadWnd::GetStatusText(FCHttpRequest& rTask)
{
	const HTTP_RESPONSE_INFO &resp = rTask.GetResponseInfo();

	CString strMsg;
	switch (resp.m_status_code)
	{
	case HTTP_STATUS_OK:
	case HTTP_STATUS_PARTIAL_CONTENT:
		// range request
		if (rTask.GetRequestHeader().m_start && (resp.m_status_code == HTTP_STATUS_OK))
		{
			strMsg += " ( not support range )";
		}
		break;

	case HTTP_STATUS_NOT_FOUND:
		strMsg = "404 错误 : 文件不存在";
		break;

	case HTTP_STATUS_FORBIDDEN :
		strMsg = "403 错误 : 操作被禁止";
		break;

	case HTTP_STATUS_PROXY_AUTH_REQ:
		strMsg = "407 错误 : 要求代理身份认证";
		break;

	default:
		strMsg.Empty();
		break;
	}
	return strMsg;
}

unsigned int __stdcall CDownloadWnd::Thread_Install(LPVOID lparam)
{
	CDownloadWnd *pWnd = (CDownloadWnd*)lparam;
	std::string receive_data;
	S_DowndloadItme* pItem = pWnd->GetDownloadItemByTask(pWnd->m_iCurTaskID);
	try
	{
		 if (pWnd->m_str_recv_data.empty())
		 {
			 throw 1;
		 }
		 if (!pItem)
		 {
			 throw 2;
		 }

		 pItem->e_state = E_ING;
		 sprintf_s(pItem->szErr, sizeof(pItem->szErr), _T("正在安装"));	
		 ::PostMessage(*pWnd, WM_MAKEFILES, pItem->uNameID, DOWNLOAD_PERCENT);
		 //打开压缩文件
		 HZIP hz = OpenZip((char*)(pWnd->m_str_recv_data.c_str()), pWnd->m_str_recv_data.size(), ZIP_MEMORY);
		 ZIPENTRY ze; 

		 //获取压缩包中文件个数
		 GetZipItem(hz,-1,&ze);
		 int numitems=ze.index;
		 ZRESULT zr;	
		 int iprogress = DOWNLOAD_PERCENT;
		 for (int i=0; i!=numitems; ++i)
		 {
			 zr = GetZipItem(hz, i, &ze);
			 if (0 != zr)
			 {
				 break; //没有更多文件了
			 }
			 //解压第i个文件
			 zr = UnzipItem(hz, i, ze.name, 0, ZIP_FILENAME);		
		 }
		 CloseZip(hz);

		 //拷贝文件
		 TCHAR strTemp[MAX_PATH];
		 int copynums = sizeof(str_copy_array) / sizeof(str_copy_array[0]);
		 for (int i=0; i!=copynums; ++i)
		 {//拷贝文件
			 sprintf_s(strTemp, MAX_PATH, _T(".\\%d\\%s"), pItem->uNameID, str_copy_array[i].c_str());
			 CopyFile(str_copy_array[i].c_str(), strTemp, FALSE);
		 }

		 pItem->e_state = E_OK;
		 pWnd->m_pList->UpdateGame(pItem);

		 sprintf_s(pItem->szErr, sizeof(pItem->szErr), _T("安装完成"));	
		 ::PostMessage(*pWnd, WM_MAKEFILES, pItem->uNameID, DOWNLOAD_PERCENT);
	}
	catch (int &iErrorNum)
	{
		if (pItem && pWnd)
		{
			//已下载大小
			pItem->fDownSize = pItem->fSize;

			//进度条
			pItem->uiProgress = 1000;

			//状态
			pItem->e_state = E_ERROR;

			//错误
			sprintf_s(pItem->szErr, sizeof(pItem->szErr), _T("安装错误: %d"), iErrorNum);

			pWnd->m_pList->UpdateGame(pItem);
		}
	}

	pWnd->DownloadNext();

	return 0;
}