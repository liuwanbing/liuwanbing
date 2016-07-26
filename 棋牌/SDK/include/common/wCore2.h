#pragma  once

// CClientSocket 命令目标
typedef struct
{
	long	CustomID;
	long	ServerID;
	long	CustomServerTime;
}_TAG_LICENSEREQUEST;

typedef struct
{
	long	CustomID;
	long	ServerID;
	long	CustomServerTime;
	long	LicenseServerTime;
	long	ExpiresTime;
	short	Expiresed;
	short	Invalidated;
}_TAG_LICENSERESPONSE;

#define WM_LICENSE_RESPONSE	WM_USER+100

class CClientSocket : public CSocket
{
public:
	CClientSocket();
	virtual ~CClientSocket();
	HWND		m_Hpwnd;
public:
	int iCount;
public:
	virtual void OnReceive(int nErrorCode);
};
class CCore : public CWnd
{
	DECLARE_DYNAMIC(CCore)

public:
	CCore();
	virtual ~CCore();

	CWnd *	pWnd;				//主窗口
	UINT	msg;				//发给主窗口的消息
	CString m_currentPath;		//当前目录

	int		m_licenseCountIntime;	//每次认证的第几次

	int		m_licenseServerCount;	//认证服务器数量
	CString m_licenseServer[10];	//认证服务器地址
	short	m_licensePort[10];		//认证服务器端口
	long	m_customID[10];			//CustomID
	long	m_serverID[10];			//serverID


	CClientSocket	m_socket;		//socket

	_TAG_LICENSEREQUEST reqData;	//认证请求结构

	CFile	logFile;				//写日志文件

protected:
	DECLARE_MESSAGE_MAP()
public:
	void StartLicense(void);
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
public:
	void Initialize(void);
public:
	void NetLicense(void);
public:
	void StopService(int errCode);
	CString GetIniString(CString szApp, CString szKey, CString szDefault, CString szIniFile=TEXT(""));
	int GetIniLong(CString szApp, CString szKey, long lDefault, CString szIniFile=TEXT(""));
public:
	LRESULT OnLicenseResponse(WPARAM wParam, LPARAM lParam);
public:
	void LoadProfile(void);
};

bool coreProcess(LPCSTR szMsg);

bool coreInitialize(CWnd *pWnd,UINT msg);

CString coreGetCode();

CString coreGetString();

long coreGetLicenseExpires();

long coreGetTimeStamp();

long coreGetGameLicense(long gameID);

CString coreGetGameLicense(CString gameID);