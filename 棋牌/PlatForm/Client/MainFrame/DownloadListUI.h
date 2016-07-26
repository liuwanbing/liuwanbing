#pragma once

enum E_DOWNLOAD_STATE
{
	E_OK,	//下载完成
	E_ERROR,//下载错误
	E_WAIT,	//等待下载
	E_ING	//正在现在	
};
struct S_DowndloadItme
{	
	char	szGameName[61];	//游戏名称	
	char szUrl[MAX_PATH];	//下载地址
	char	szErr[50];		//错误信息
	float	fSize;			//文件总大小
	float	fDownSize;		//已下载大小	
	float	fSpeed;		//下载速度
	UINT	uNameID;		//游戏名称 ID 号码
	UINT	uiProgress;		//下载进度
	E_DOWNLOAD_STATE e_state;
	S_DowndloadItme()
	{ 
		ZeroMemory(this,sizeof(S_DowndloadItme));
	}
};

typedef map<UINT, BzDui::CListContainerElementUI*> map_GameItem;

class CDownloadListUI: public BzDui::CListUI
{
public:
	CDownloadListUI(void);
	~CDownloadListUI(void);

	bool AddGame(ComNameInfo* pNameInfo, const char* lpstrUrl);
		
	void UpdateData(BzDui::CListContainerElementUI *pElement);

	bool UpdateGame(S_DowndloadItme* pDownloadItem);

	S_DowndloadItme* GetItem(UINT uNameID);

private:

	map_GameItem m_mapGame;
};

class CProgressExUI : public BzDui::CProgressUI
{
public:
	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
};

class CDialogBuilderCallbackEx_DownLoad : public BzDui::IDialogBuilderCallback
{
public:
	BzDui::CControlUI* CreateControl(LPCTSTR pstrClass) 
	{
		if(_tcscmp(pstrClass, _T("DownloadList")) == 0)
		{			
			return new CDownloadListUI;
		}
		else if(_tcscmp(pstrClass, _T("ProgressEx")) == 0)
		{
			return new CProgressExUI;
		}
		return NULL;
	}
};
