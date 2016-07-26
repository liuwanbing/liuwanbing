

#include "Stdafx.h"
#include "SkinMgr.h"
#include "LoveSendClassInExe.h"

//设置的皮肤选项索引
CString CSkinMgr::m_skinIndex;

//用户名
CString CSkinMgr::m_strUserName;

CSkinMgr::CSkinMgr()
{
}
CSkinMgr::~CSkinMgr()
{
}

bool CSkinMgr::IsUserCountFirstLogOn(const CString& strLogonUserID)
{
	m_skinIndex = _T("2");
	return true;


	bool ret = false;

	//2.先打开bcf
	CString s=CBcfFile::GetAppPath();
	DWORD dwCfgFile = ::cfgOpenFile(s + _T("UserSelSkin.bcf"));

	//3.bcf文件打开失败返回
	if(dwCfgFile < 0x10)
		return true;

	//4.获取bcf文件中的值
	m_skinIndex = ::cfgGetValue(dwCfgFile,strLogonUserID,_T("skin"),_T(""));
	if ("" == m_skinIndex)
		ret = true;

	//5.关闭bcf文件
	::cfgClose(dwCfgFile);

	return ret;
}

CString CSkinMgr::GetValOfSkinBcf(const CString& strLogonUserID)
{
	m_skinIndex = _T("2");
	return m_skinIndex;


	//1.获取登陆名
	CString key = _T("");
	if (0 != strLogonUserID.Compare(_T("")))
		key = strLogonUserID;
	else if (0 != m_strUserName.Compare(_T("")))
		key = m_strUserName;

	if (0 == key.Compare(_T("")))
	{
		m_skinIndex = _T("1");
		return m_skinIndex;
	}

	//2.先打开bcf
	CString s=CBcfFile::GetAppPath();
	DWORD dwCfgFile = ::cfgOpenFile(s + _T("..\\UserSelSkin.bcf"));

	//3.bcf文件打开失败返回
	if(dwCfgFile < 0x10)
	{
		m_skinIndex = _T("1");
		return m_skinIndex;
	}

	//4.获取bcf文件中的值
	m_skinIndex = ::cfgGetValue(dwCfgFile,key,_T("skin"),_T(""));

	//5.关闭bcf文件
	::cfgClose(dwCfgFile);

	return m_skinIndex;
}

//启动时软件设置的用户名
void CSkinMgr::SetLogOnUserName(const CString& strLogonName)
{
	m_strUserName = strLogonName;
}
//获取当前设置的皮肤
CString CSkinMgr::GetUserSelSkinIndex() const
{
	return m_skinIndex = _T("2");

	return m_skinIndex;
}

//获取用户选择的皮肤路径
CString CSkinMgr::GetSkinPath(const CString& strLogonUserID)
{
	return _T("image\\client\\skin1\\");


	CString strUserID = GetSkinBcfFileName();
	if (_T("skin0.bcf") == strUserID)
	{
		return _T("image\\client\\skin0\\");
	}
	else if (_T("skin1.bcf") == strUserID)
	{
		return _T("image\\client\\skin1\\");
	}
	else
		return _T("image\\client\\skin0\\");
}
CString CSkinMgr::GetGameSkinPath()
{
	return _T("skin1.bcf");

	CString strUserID = GetSkinBcfFileName();
	if (_T("skin0.bcf") == strUserID)
	{
		return _T("image\\client\\skin0\\game\\");
	}
	else if (_T("skin1.bcf") == strUserID)
	{
		return _T("image\\client\\skin1\\game\\");
	}
	else
		return _T("image\\client\\skin0\\game\\");
}
//获取皮肤路径下的对应文件名
CString CSkinMgr::GetSkinBcfFileName(const CString& strLogonUserID)
{
	return _T("skin1.bcf");

	GetValOfSkinBcf(strLogonUserID);
	if (m_skinIndex == _T("1"))
		return _T("skin0.bcf");
	else if (m_skinIndex == _T("2"))
		return _T("skin1.bcf");
	else
		return _T("skin0.bcf");
}
//获取游戏房间路径对应的文件名
CString CSkinMgr::GetGameSkinBcfFileName(const CString& strLogonUserID)
{
	return _T("gameskin1.bcf");


	GetValOfSkinBcf(strLogonUserID);
	if (m_skinIndex == _T("1"))
		return _T("gameskin0.bcf");
	else if (m_skinIndex == _T("2"))
		return _T("gameskin1.bcf");
	else
		return _T("gameskin0.bcf");
}
//获取key值
CString CSkinMgr::GetKeyVal(const CString& strSkinBcfFileName)
{
	return _T("skin1");

	if (_T("skin0.bcf") == strSkinBcfFileName)
		return _T("skin0");
	else if (_T("skin1.bcf") == strSkinBcfFileName)
		return _T("skin1");

	return "skin0";
}

//设置当前用户选择的皮肤
void CSkinMgr::SetCurUserSelSkin(const CString& strindex)
{
	m_skinIndex = _T("2");


	//1.设置皮肤索引到bcf,先打开bcf
	CString s=CBcfFile::GetAppPath();
	DWORD dwCfgFile = ::cfgOpenFile(s + _T("UserSelSkin.bcf"));

	//2.bcf文件打开失败返回
	if(dwCfgFile < 0x10 || 0 == m_strUserName.Compare(_T(""))) 
		return;

	::cfgSetValue(dwCfgFile,m_strUserName,_T("skin"),m_skinIndex);
	::cfgSetKeyMemo(dwCfgFile,m_strUserName,_T("skin"),_T("用户选择的皮肤"));

	//5.关闭bcf文件
	::cfgClose(dwCfgFile);

	return;


	//3.设置值与bcf文件中的值相等，不再设置
	if (0 != strindex.Compare(_T("")) && strindex == ::cfgGetValue(dwCfgFile,m_strUserName,_T("skin"),_T(""))) 
		return;

	//4.用户所选皮肤选项值设置
	if (0 == strindex.Compare(_T("")))
		m_skinIndex = _T("1");
	else
		m_skinIndex = strindex;

	::cfgSetValue(dwCfgFile,m_strUserName,_T("skin"),m_skinIndex);
	::cfgSetKeyMemo(dwCfgFile,m_strUserName,_T("skin"),_T("用户选择的皮肤"));

	//5.关闭bcf文件
	::cfgClose(dwCfgFile);
}
//通知界面刷新皮肤
void CSkinMgr::Notify()
{
	CString s = CBcfFile::GetAppPath ();/////本地路径
	CString strSkin = GetSkinBcfFileName();
	CBcfFile f(s + strSkin);
	TCHAR szUIPath[MAX_PATH];
	CString skinfolder;

	skinfolder = f.GetKeyVal(GetKeyVal(strSkin),"skinfolder","..\\" + GetSkinPath());
	wsprintf(szUIPath,"%s",skinfolder);

	BzDui::CPaintManagerUI::SetResourcePath(szUIPath);
	BzDui::CPaintManagerUI::ReloadSkin();
}