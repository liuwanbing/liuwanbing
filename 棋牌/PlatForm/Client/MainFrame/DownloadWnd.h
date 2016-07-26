#pragma once
#include "../utility/http_request_manager.h"
#include "DownloadListUI.h"

class CDownloadWnd :
	public BzDui::CWindowWnd,
	public BzDui::INotifyUI,
	public BzDui::IMessageFilterUI,
	public FCHttpRequestManager
{
public:
	explicit CDownloadWnd();

	~CDownloadWnd();

	LPCTSTR GetWindowClassName() const;

	UINT GetClassStyle() const;

	void OnFinalMessage(HWND hWnd);

	void Init();

	void Notify(BzDui::TNotifyUI& msg);

public:

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

public:

	//添加下载任务
	bool AddDownloadEx(ComNameInfo* pNameInfo);

	void DownloadNext();

	CString GetFormatKB (long long nByte)
	{
		TCHAR strRes[128] ;
		StrFormatKBSize(nByte, strRes, sizeof(strRes)) ;
		return strRes;
	}

protected:

	//下载请求发送完成后调用此函数
	virtual void OnAfterRequestSend(FCHttpRequest& rTask);

	//文件下载完成后调用此函数
	virtual void OnAfterRequestFinish(FCHttpRequest& rTask);

	//解析错误码
	CString GetStatusText(FCHttpRequest& rTask);

	S_DowndloadItme* GetDownloadItemByTask(int iTaskID);

	void GetDownloadProcess(int iTaskID);

	static unsigned int __stdcall Thread_Install(LPVOID lparam);

protected:

	BzDui::CPaintManagerUI m_pm;

private:

	std::deque<ComNameInfo*>   m_wait_download;

	map<int, ComNameInfo*> m_mapNameTask;				

	bool m_bDownloading;								//标识现在是否正在下载

	int m_iCurTaskID;									//当前正在下载的任务ID

	std::string m_str_recv_data;						//保存下载的数据
	
	CDownloadListUI* m_pList;
	
};