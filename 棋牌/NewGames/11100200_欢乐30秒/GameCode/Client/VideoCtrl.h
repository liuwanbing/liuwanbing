#ifndef _VIDEO_CTRL_CLASS_H_
#define _VIDEO_CTRL_CLASS_H_

#include "VideoWndStyle.h"
#include "VideoWnd.h"



///
struct ST_INITVIDEO
{
	CWnd		*pParentWnd;
	CString		strUserName;
	CString		strUserPass;
	UINT		nRoomID;
	CString		strRoomPass;
	int			iRoomPassType;
};

interface I_VIDEO
{
public:
	virtual  bool	Video_Initialize(ST_INITVIDEO& stInitVideo)=0;

	virtual  void	Video_Release()=0;
};

class  CVideoCtrl  : public CWnd	 , public I_VIDEO
{
public:
	CVideoCtrl(void);

	virtual ~CVideoCtrl(void);

	virtual  bool	Video_Initialize( ST_INITVIDEO& stInitVideo); 
	/*****************************************
	(IN)  name, 玩家昵称;
	(OUT) rect, 此玩家视频窗口坐标;
	当成功输出　rect时，返回true,否则false;
	******************************************/
	virtual	 bool	GetVideoRectByName( CString name, RECT& rect, CString& pic)=0;

	virtual  void	Video_Release();
public:
	bool		Init(CWnd *pParentWnd, CString strUserName,CString strUserPass,UINT nRoomID,CString strRoomPass,int iRoomPassType); ///init video , 
	
	///设置视频窗体风格
	void		SetVideoFrameStyle(GV_VW_STYLE *FrameStyle, GV_VFT_STYLE *ToolbarStyle);
	//获取摄像头的状态
	int			GetCameraState();

	///连接服务器
	bool		Connect();
	bool		Login();
	bool		Enterroom();
	bool		OpenLocalVedio();			///打开本地视频
	void		SetUserPos();				///设置玩家视频窗口的位置
	void        Logout();
	void		ShowUserPanelEx(long userid, short bShow, short style, long* pParentWnd);


	///用户进入（离开）房间
	LRESULT     UserAtRoom(WPARAM wParam, LPARAM lParam);
	LRESULT     OnlineUser(WPARAM wParam, LPARAM lParam);
	///处理视频消息
	void VedioMsgProc(UINT msg,WPARAM wParam, LPARAM lParam);   
public:
	CString		m_strVedioServerIP;
	UINT		m_nVedioServerPort;
	UINT		m_nRoomID;
	CString     m_strRoomPass;
	CString		m_strUserName;
	CString		m_strUserPass;			 ///登陆视频服务器的密码
	CString		m_strActiveCode;         ///激活码,激活控件
	int			m_iActiveCodeType;       ///激活码类型（0=明文）
	int			m_iRoomPassType;		 ///房间密码类型

	CVideoWnd   *m_pVideoWnd;
	short		m_PanelStyle;            ///视频窗口是否是子窗口1是，0不是
	long*		m_pParentWnd;

	GV_VW_STYLE	 m_VideoWindowStyle;	 ///  视频窗体风格
	GV_VFT_STYLE  m_VideoToolbarStyle;	 ///< 视频工具条风格 
	int			m_iUserID[MAX_PEOPLE];   ///玩家自己是id[0];id[n]==-1表示此位置无人

	CString		m_strSnapShotSavePath;	    //设置快照保存路径
	CString		m_strRecordFileSavePath;		 //设置录像文件保存路径

	int			m_widthVideo;
	int			m_heightVideo;

	 

protected:
	DECLARE_MESSAGE_MAP() 
	afx_msg LRESULT OnGVClientConnect(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGVClientLogin(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGVClientEnterRoom(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGVClientMicStateChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGVClientUserAtRoom(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGVClientLinkClose(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGVClientOnlineUser(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGVFortuneMenu(WPARAM wParam, LPARAM lParam);
};

#endif