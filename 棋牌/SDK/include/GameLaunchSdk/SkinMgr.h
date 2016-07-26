#ifndef WM_EXCHANGE_SKIN
#define WM_EXCHANGE_SKIN			WM_USER + 1666				//更换皮肤
#endif

#ifndef	__SKINMGR__H__
#define	__SKINMGR__H__

#include "StdAfx.h"

class EXT_CLASS CSkinMgr
{
public:
	CSkinMgr();
	~CSkinMgr();

public:

	CString GetValOfSkinBcf(const CString& strLogonUserID = "");

public:

	CString GetUserSelSkinIndex() const;

	//获取皮肤路径
	CString GetSkinPath(const CString& strLogonUserID = "");

	//获取游戏房间皮肤路径
	CString GetGameSkinPath();

	//获取皮肤路径下的对应文件名
	CString GetSkinBcfFileName(const CString& strLogonUserID = "");

	//获取游戏房间路径对应的文件名
	CString GetGameSkinBcfFileName(const CString& strLogonUserID = "");

	//设置当前用户选中的皮肤
	void SetCurUserSelSkin(const CString& strindex = "");

	//获取key值
	CString GetKeyVal(const CString& strSkinBcfFileName);

	//通知界面刷新皮肤
	void Notify();

	//启动时软件设置的用户名
	void SetLogOnUserName(const CString& strLogonName);

	//该用户是否第一次登录
	bool IsUserCountFirstLogOn(const CString& strLogonUserID);

private:

	//设置的皮肤选项索引
	static CString m_skinIndex;

	//用户名
	static CString m_strUserName;

};



#endif