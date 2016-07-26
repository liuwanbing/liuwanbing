

#include "Stdafx.h"
#include "SkinMgr.h"
//#include "MainRoomEx.h"
//#include "FindUserDlg.h"
//#include "GamePlaceDlg.h"
#include "..\\UILibrary2003\\AFCTabCtrl.h"

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
	DWORD dwCfgFile = ::cfgOpenFile(s + _T("UserSelSkin.bcf"));

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
//获取默认皮肤
CString CSkinMgr::GetDefaultSelSkin()
{
	m_skinIndex = _T("2");
	return m_skinIndex;


	//1.先打开bcf
	CString s=CBcfFile::GetAppPath();
	DWORD dwCfgFile = ::cfgOpenFile(s + _T("UserSelSkin.bcf"));

	//2.bcf文件打开失败返回
	if(dwCfgFile < 0x10)
	{
		m_skinIndex = _T("1");
		return m_skinIndex;
	}

	//3.获取bcf文件中的值
	m_skinIndex = ::cfgGetValue(dwCfgFile,_T("DefaultSkin"),_T("skin"),_T("1"));

	//4.关闭bcf文件
	::cfgClose(dwCfgFile);

	return m_skinIndex;
}

//启动时软件设置的用户名
void CSkinMgr::SetLogOnUserName(const CString& strLogonName)
{
	m_strUserName = strLogonName;
}

//获取用户选择的皮肤路径
CString CSkinMgr::GetSkinPath(const CString& strLogonUserID)
{
	return _T("image\\client\\skin1\\");



	CString strUserID = GetSkinBcfFileName(strLogonUserID);
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
//获取key值
CString CSkinMgr::GetKeyVal(const CString& strSkinBcfFileName)
{
	return _T("skin1");


	if (_T("skin0.bcf") == strSkinBcfFileName)
		return _T("skin0");
	else if (_T("skin1.bcf") == strSkinBcfFileName)
		return _T("skin1");
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
	//int skinIndex = atoi(m_skinIndex);
	//CString strSkinFileName = GetSkinBcfFileName();
	//CMainRoomEx *pMainRoom = ::GetMainRoom();

	////1.主窗口 
	//if (pMainRoom == NULL) return;
	//	pMainRoom->PostMessage(WM_EXCHANGE_SKIN,skinIndex,0);//网页窗口

	////3.游戏列表 
	//if (pMainRoom->m_pGameListWnd != NULL)
	//	pMainRoom->m_pGameListWnd->PostMessage(WM_EXCHANGE_SKIN,skinIndex,0);

	////4.下方公告 
	//if (pMainRoom->m_pActWnd != NULL)
	//	pMainRoom->m_pActWnd->PostMessage(WM_EXCHANGE_SKIN,skinIndex,0);

	////5.上方窗口 
	//if (pMainRoom->m_TopWnd != NULL)
	//	pMainRoom->m_TopWnd->PostMessage(WM_EXCHANGE_SKIN,skinIndex,0);

	//if (((CGamePlaceDlg *)AfxGetMainWnd()) != NULL)
	//{
	//	((CGamePlaceDlg*)AfxGetMainWnd())->PostMessage(WM_EXCHANGE_SKIN,skinIndex,0);
	//	((CGamePlaceDlg*)AfxGetMainWnd())->m_topDuWnd.PostMessage(WM_EXCHANGE_SKIN,skinIndex,0);
	//	((CGamePlaceDlg*)AfxGetMainWnd())->m_RoomTabWnd.OnExchangeSkin(GetSkinBcfFileName(),GetKeyVal(strSkinFileName),GetSkinPath());
	//}

	////9.
	//if (pMainRoom->m_pAppealDlg != NULL)
	//	pMainRoom->m_pAppealDlg->PostMessage(WM_EXCHANGE_SKIN,skinIndex,0);

	////10.
	//if (pMainRoom->hBulletin != NULL)
	//	pMainRoom->hBulletin->PostMessage(WM_EXCHANGE_SKIN,skinIndex,0);

	////11.
	//if (pMainRoom->CharmPannel != NULL)
	//	pMainRoom->CharmPannel->PostMessage(WM_EXCHANGE_SKIN,skinIndex,0);

	////12.呼叫GM对话框 
	//if (pMainRoom->m_pGMListDlg != NULL)
	//	pMainRoom->m_pGMListDlg->PostMessage(WM_EXCHANGE_SKIN,skinIndex,0);

	////13.头像上传对象 
	//if (pMainRoom->logoUpload != NULL)
	//	pMainRoom->logoUpload->PostMessage(WM_EXCHANGE_SKIN,skinIndex,0);

	////14.游戏列表 
	//if (pMainRoom->m_pGameListWnd != NULL)
	//{
	//	if (pMainRoom->m_pGameListWnd->m_GameList.IsWindowVisible())
	//		pMainRoom->m_pGameListWnd->m_GameList.PostMessage(WM_EXCHANGE_SKIN,skinIndex,0);
	//	if (pMainRoom->m_pGameListWnd->m_HelpList.IsWindowVisible())
	//		pMainRoom->m_pGameListWnd->m_HelpList.PostMessage(WM_EXCHANGE_SKIN,skinIndex,0);
	//}
	//pMainRoom->m_Connect.PostMessage(WM_EXCHANGE_SKIN,skinIndex,0);

	////15
	//if (pMainRoom->m_pGameListWnd != NULL && pMainRoom->m_pGameListWnd->m_pIMWnd != NULL)
	//{
	//	pMainRoom->m_pGameListWnd->m_pIMWnd->PostMessage(WM_EXCHANGE_SKIN,skinIndex,0);
	//	if (pMainRoom->m_pGameListWnd->m_pIMWnd->addFriendDlg.IsWindowVisible())
	//		pMainRoom->m_pGameListWnd->m_pIMWnd->addFriendDlg.PostMessage(WM_EXCHANGE_SKIN,skinIndex,0);
	//	if (pMainRoom->m_pGameListWnd->m_pIMWnd->setFriendDlg.IsWindowVisible())
	//		pMainRoom->m_pGameListWnd->m_pIMWnd->setFriendDlg.PostMessage(WM_EXCHANGE_SKIN,skinIndex,0);
	//}
	//if (pMainRoom->m_pGameListWnd != NULL && pMainRoom->m_pGameListWnd->m_IMList.IsWindowVisible())
	//	pMainRoom->m_pGameListWnd->m_IMList.PostMessage(WM_EXCHANGE_SKIN,skinIndex,0);

	////17.
	//if (pMainRoom->pdDlg.IsWindowVisible())
	//	pMainRoom->pdDlg.PostMessage(WM_EXCHANGE_SKIN,skinIndex,0);
	//
	////18.退出软件窗口
	//if (pMainRoom->m_TopDuWnd != NULL && pMainRoom->m_TopDuWnd->m_quitAskdlg.IsWindowVisible())
	//	pMainRoom->m_TopDuWnd->m_quitAskdlg.PostMessage(WM_EXCHANGE_SKIN,skinIndex,0);

	////19.退出软件窗口
	//if (pMainRoom->m_TopWnd != NULL && pMainRoom->m_TopWnd->m_quitAskdlg.IsWindowVisible())
	//	pMainRoom->m_TopWnd->m_quitAskdlg.PostMessage(WM_EXCHANGE_SKIN,skinIndex,0);

	////20.游戏房间
	//for(int i = 0; i < MAX_GAME_ROOM; i ++)
	//{
	//	if(pMainRoom->m_RoomInfo[i].bAccess)
	//	{
	//		// 关闭游戏房间
	//		CGameRoomEx* pGameRoomEx = (CGameRoomEx*)pMainRoom->m_RoomInfo[i].pGameRoomWnd;
	//		if (NULL != pGameRoomEx)
	//		{
	//			pGameRoomEx->PostMessage(WM_EXCHANGE_SKIN,skinIndex,0);
	//			pGameRoomEx->m_vipDlg.PostMessage(WM_EXCHANGE_SKIN,skinIndex,0);
 //				if (pGameRoomEx->boardCast != NULL)
 //					pGameRoomEx->boardCast->PostMessage(WM_EXCHANGE_SKIN,skinIndex,0);
	//			if (pGameRoomEx->m_pCallGMDlg.IsWindowVisible())
	//				pGameRoomEx->m_pCallGMDlg.PostMessage(WM_EXCHANGE_SKIN,skinIndex,0);
	//			if (pGameRoomEx->dlg.IsWindowVisible())
	//				pGameRoomEx->dlg.PostMessage(WM_EXCHANGE_SKIN,skinIndex,0);
	//			if (pGameRoomEx->m_pGetHelpMoneyDlg != NULL)
	//				pGameRoomEx->m_pGetHelpMoneyDlg->PostMessage(WM_EXCHANGE_SKIN,skinIndex,0);
	//			if (pGameRoomEx->m_pPersonBank != NULL)
	//				pGameRoomEx->m_pPersonBank->PostMessage(WM_EXCHANGE_SKIN,skinIndex,0);
	//			if (pGameRoomEx->m_pManageDlg != NULL)
	//				pGameRoomEx->m_pManageDlg->PostMessage(WM_EXCHANGE_SKIN,skinIndex,0);
	//			if (pGameRoomEx->m_pQueueInRoomDlg != NULL)
	//				pGameRoomEx->m_pQueueInRoomDlg->PostMessage(WM_EXCHANGE_SKIN,skinIndex,0);
	//			if (pGameRoomEx->m_pUserList != NULL)
	//				pGameRoomEx->m_pUserList->PostMessage(WM_EXCHANGE_SKIN,skinIndex,0);
	//			if (pGameRoomEx->m_pUserDetailsDlg != NULL)
	//				pGameRoomEx->m_pUserDetailsDlg->PostMessage(WM_EXCHANGE_SKIN,skinIndex,0);
	//			if (pGameRoomEx->m_pFindUser != NULL)
	//			{
	//				pGameRoomEx->m_pFindUser->PostMessage(WM_EXCHANGE_SKIN,skinIndex,0);
	//				pGameRoomEx->m_pFindUser->m_UserList.PostMessage(WM_EXCHANGE_SKIN,skinIndex,0);
	//			}
	//			if (pGameRoomEx->m_IGameFrame != NULL)
	//				pGameRoomEx->m_IGameFrame->OnControlMessage(CM_EX_SKIN,&(::GetMainRoom()->m_PlaceUserInfo),sizeof(MSG_GP_R_LogonResult));
	//		}

	//		//房间右边窗口
	//		CRoomRightWnd* pRightWnd = pMainRoom->m_RoomInfo[i].pRoomInfoWnd;
	//		if (NULL != pRightWnd)
	//		{
	//			pRightWnd->PostMessage(WM_EXCHANGE_SKIN,skinIndex,0);
	//			pRightWnd->m_ChatControl.PostMessage(WM_EXCHANGE_SKIN,skinIndex,0);
	//			pRightWnd->m_UserList.PostMessage(WM_EXCHANGE_SKIN,skinIndex,0);
	//			if (pRightWnd->m_TalkMessage.m_DiyChat.IsWindowVisible())
	//				pRightWnd->m_TalkMessage.m_DiyChat.OnExchangeSkin(strSkinFileName,GetKeyVal(strSkinFileName),GetSkinPath());
	//		}
	//	}
	//}
}
