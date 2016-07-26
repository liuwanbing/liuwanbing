#include "stdafx.h"
#include "DownloadListUI.h"

using namespace BzDui;

CDownloadListUI::CDownloadListUI(void)
{
}

CDownloadListUI::~CDownloadListUI(void)
{
}

bool CDownloadListUI::AddGame(ComNameInfo* pNameInfo, const char* lpstrUrl)
{
	if (!pNameInfo)
	{
		return false;
	}

	//判断该玩家是否已经添加	
	CListContainerElementUI* pListElement = m_mapGame[pNameInfo->uNameID];
	if (NULL != pListElement)
	{
		return true;
	}
		
	CDialogBuilderCallbackEx_DownLoad cb;
	CDialogBuilder builder;
	pListElement = static_cast<CListContainerElementUI*>(builder.Create(_T("left/DownloadItem.xml"), (UINT)0,  &cb, m_pManager));
	if (pListElement == NULL)
	{
		return false;
	}
	
	//添加新下载项
	S_DowndloadItme* pNewItem = new S_DowndloadItme;
	pNewItem->uNameID = pNameInfo->uNameID;
	memcpy_s(pNewItem->szGameName, sizeof(pNewItem->szGameName), pNameInfo->szGameName, sizeof(pNameInfo->szGameName));
	memcpy_s(pNewItem->szUrl, sizeof(pNewItem->szUrl), lpstrUrl, sizeof(lpstrUrl));

	CListUI::Add(pListElement);
	m_mapGame[pNameInfo->uNameID] = pListElement;
	pListElement->SetTag((UINT_PTR)pNewItem);

	UpdateData(pListElement);

	return true;
}

void CDownloadListUI::UpdateData(CListContainerElementUI *pElement)
{
	if (!pElement)
	{
		return;
	}

	S_DowndloadItme *pItem = (S_DowndloadItme*)pElement->GetTag();
	if (!pItem)
	{
		return;
	}
	CString strValue;
	
	//游戏名
	CLabelUI *pLab = static_cast<CLabelUI*>(m_pManager->FindSubControlByName(pElement, _T("Lab_GameName")));
	if (pLab)
	{
		pLab->SetText(pItem->szGameName);
	}

	//文件大小
	pLab = static_cast<CLabelUI*>(m_pManager->FindSubControlByName(pElement, _T("Lab_Size")));
	if (pLab)
	{
		strValue.Format(_T("%.1fMB"), pItem->fSize);
		pLab->SetText(strValue);
	}

	//已经下载大小
	pLab = static_cast<CLabelUI*>(m_pManager->FindSubControlByName(pElement, _T("Lab_DownSize")));
	if (pLab)
	{
		strValue.Format(_T("%.1fMB"), pItem->fDownSize);
		pLab->SetText(strValue);
	}

	//速度
	pLab = static_cast<CLabelUI*>(m_pManager->FindSubControlByName(pElement, _T("Lab_Speed")));
	if (pLab)
	{
		strValue.Format(_T("%.1fKB/s"), pItem->fSpeed);
		pLab->SetText(strValue);
	}

	//进度
	CProgressExUI* pPro = static_cast<CProgressExUI*>(m_pManager->FindSubControlByName(pElement, _T("Pro_Install")));
	if (pPro)
	{
		pPro->SetValue(pItem->uiProgress);
	}

	//错误信息
	pLab = static_cast<CLabelUI*>(m_pManager->FindSubControlByName(pElement, _T("Lab_Err")));
	if (pLab)
	{
		pLab->SetText(pItem->szErr);
	}

	//状态
	CControlUI* pCtrl  = m_pManager->FindSubControlByName(pElement, _T("Ctrl_State"));
	if (pCtrl)
	{
		strValue.Format(_T("file='Left/download/download_state.png' source='%d,0,%d,30'"), pItem->e_state*30, (pItem->e_state+1)*30);
		pCtrl->SetBkImage(strValue);
	}
}

bool CDownloadListUI::UpdateGame(S_DowndloadItme* pDownloadItem)
{
	if (!pDownloadItem)
	{
		return false;
	}
	CListContainerElementUI* pListElement = m_mapGame[pDownloadItem->uNameID];
	if (!pListElement)
	{
		return false;
	}

	S_DowndloadItme *pItem = (S_DowndloadItme *)pListElement->GetTag();	
	memcpy(pItem, pDownloadItem, sizeof(S_DowndloadItme));

	UpdateData(pListElement);

	pListElement->NeedUpdate();

	return true;
}

S_DowndloadItme* CDownloadListUI::GetItem(UINT uNameID)
{
	S_DowndloadItme* pItem = NULL;
	CListContainerElementUI* pListElement = m_mapGame[uNameID];
	if (pListElement)
	{
		pItem = (S_DowndloadItme *)pListElement->GetTag();	
	}
	return pItem;
}

void CProgressExUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	CProgressUI::SetAttribute(pstrName, pstrValue);

	if( _tcscmp(pstrName, _T("foreimage")) == 0 )
		m_sFgImage = pstrValue;
}