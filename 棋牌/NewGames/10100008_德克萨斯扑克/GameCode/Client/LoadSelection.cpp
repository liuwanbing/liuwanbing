#include "stdafx.h"
#include "LoadSelection.h"
#include "MiniGameEngine.h"

extern TCHAR *GlbGetFloatString(TCHAR pRet[], int nValue); 

#define IDT_NOTICES		900


BEGIN_MESSAGE_MAP(CLoadSelectionDlg, BModalContainer)
	ON_WM_LBUTTONUP()
	ON_EN_CHANGE(IDC_PLAN_MONEY, &CLoadSelectionDlg::OnEnChangeEdit)
	ON_WM_TIMER()
END_MESSAGE_MAP()


CLoadSelectionDlg::CLoadSelectionDlg(DWORD pId,CWnd* pParent)
	: BModalContainer(pId, pParent)
{
	m_pWndParent = pParent;
	m_bGameLoadSelection = false;
	m_strEdit = "";
}

void CLoadSelectionDlg::DoDataExchange(CDataExchange* pDX)
{
	BModalContainer::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PLAN_MONEY, m_strEdit);
}


CLoadSelectionDlg::~CLoadSelectionDlg()
{

}

/**
 * 处理当前包容器中的控件消息
 *
 * @param pId 要处理的控件
 * @param pParam 要处理的事件消息
 */
bool CLoadSelectionDlg::OnPostMessage(UINT pId, BBaseParameter* pParam)
{
	switch(pId)
	{
	case IDB_OK_LOAD:
		{
			CTrueEdit * pEdit = dynamic_cast<CTrueEdit *>(GetControlById(IDC_PLAN_MONEY));
			if (pEdit != NULL)
			{
				// 获取当前代入金币
				CString strMoney;
				pEdit->GetWindowText(strMoney);
				int nCurrentMoney = atoi(strMoney);

				if (m_subMoney.nCurrentMoney <m_subMoney.nMinMoney ||  m_subMoney.nCurrentMoney > m_subMoney.nMaxMoney)
				{
					m_subMoney.nCurrentMoney = m_subMoney.nMinMoney;
					m_subMoney.nSelfMoney = m_nOldSelfMoney - m_subMoney.nCurrentMoney;
				}

				m_subMoney.nCurrentMoney = atoi(strMoney);
				
				CMiniGameEngine* pGameDlg = (CMiniGameEngine*)m_pWndParent;
				EndModalLoop(EVENT_CONTINUE_MAINFRAME);
				pGameDlg->PlayBaseSound(DZPKSound::BT_OK);
			}
		}
		break;
	
	case IDB_EXIT_GAME:
		{
			EndModalLoop(EVENT_CONTINUE_MAINFRAME);
			CMiniGameEngine* pGameDlg = (CMiniGameEngine*)m_pWndParent;
			pGameDlg->OnCancel();
		}
		break;
	case IDB_ADD:
		{
			CTrueEdit * pEdit = dynamic_cast<CTrueEdit *>(GetControlById(IDC_PLAN_MONEY));
			CTransparentStatic * pStatic = dynamic_cast<CTransparentStatic *>(GetControlById(IDC_TOTAL_MONEY));

			if (pEdit == NULL || pStatic == NULL)
			{
				AfxMessageBox("IDC_PLAN_MONEY OR IDC_TOTAL_MONEY 控件不存在");
				break;
			}

			// 获取当前代入金币
			CString strMoney;
			pEdit->GetWindowText(strMoney);
			m_subMoney.nCurrentMoney = atoi(strMoney);

			if ((m_subMoney.nCurrentMoney + m_subMoney.nMinusMoney) > m_subMoney.nMaxMoney)
			{
				m_subMoney.nCurrentMoney = m_subMoney.nMaxMoney;
				m_subMoney.nSelfMoney = m_nOldSelfMoney - m_subMoney.nCurrentMoney;
			}	
			else
			{
				m_subMoney.nCurrentMoney += m_subMoney.nMinusMoney;
				m_subMoney.nSelfMoney -= m_subMoney.nMinusMoney;
			}
			
			// 防止代入金币超过自身金币
			if (m_subMoney.nCurrentMoney > m_nOldSelfMoney)
			{
				m_subMoney.nCurrentMoney = m_nOldSelfMoney;
				m_subMoney.nSelfMoney = 0;
			}

			// 设置当前代入金币
			strMoney.Format("%d",m_subMoney.nCurrentMoney);
			pEdit->SetWindowText(strMoney.GetBuffer());
			pEdit->ShowWindow(SW_SHOW);

			// 设置自己本身金币
			strMoney.Format("%d", m_subMoney.nSelfMoney);
			pStatic->SetMyText(strMoney.GetBuffer());
			pStatic->ShowWindow(SW_SHOW);
		}
		break;
	case IDB_SUB:
		{
			CTrueEdit * pEdit = dynamic_cast<CTrueEdit *>(GetControlById(IDC_PLAN_MONEY));
			CTransparentStatic * pStatic = dynamic_cast<CTransparentStatic *>(GetControlById(IDC_TOTAL_MONEY));

			if (pEdit == NULL || pStatic == NULL)
			{
				AfxMessageBox("IDC_PLAN_MONEY OR IDC_TOTAL_MONEY 控件不存在");
				break;
			}

			// 获取当前代入金币
			CString strMoney;
			pEdit->GetWindowText(strMoney);
			m_subMoney.nCurrentMoney = atoi(strMoney);

			if ((m_subMoney.nCurrentMoney - m_subMoney.nMinusMoney) < m_subMoney.nMinMoney)
			{
				m_subMoney.nCurrentMoney = m_subMoney.nMinMoney;
				m_subMoney.nSelfMoney = m_nOldSelfMoney - m_subMoney.nCurrentMoney;
			}	
			else
			{
				m_subMoney.nCurrentMoney -= m_subMoney.nMinusMoney;
				m_subMoney.nSelfMoney += m_subMoney.nMinusMoney;
			}

			// 设置当前代入金币
			strMoney.Format("%d",m_subMoney.nCurrentMoney);
			pEdit->SetWindowText(strMoney.GetBuffer());
			pEdit->ShowWindow(SW_SHOW);

			// 设置自己本身金币
			strMoney.Format("%d", m_subMoney.nSelfMoney);
			pStatic->SetMyText(strMoney.GetBuffer());
			pStatic->ShowWindow(SW_SHOW);
		}
		break;
	case IDB_STOP:
		{
			EndModalLoop(EVENT_CLOSE_MAINFRAME);
		}
		break;
	}
	return __super::OnPostMessage(pId,pParam);
}

/// 加载对话框
bool CLoadSelectionDlg::LoadConfig(std::string path)
{
	return __super::LoadConfig(path);
}

// 写入金额
void CLoadSelectionDlg::WriteMoney()
{
	
	return;
}

void CLoadSelectionDlg::SetNotice(std::string str,bool bShowNextBnt)
{
	if (bShowNextBnt)
	{
		ShowControl(EVENT_CONTINUE_MAINFRAME,SW_SHOW);
	}
	else
	{
		ShowControl(EVENT_CONTINUE_MAINFRAME,SW_HIDE);
	}

	CTransparentStatic* pStatic = dynamic_cast<CTransparentStatic*>(GetControlById(IDT_NOTICES));

	if (pStatic)
	{
		pStatic->SetMyText(str);
		pStatic->ShowWindow(SW_SHOW);
	}

	return;
}

//设置成服务器游戏已LoadSelection的状态
void CLoadSelectionDlg::SetGameLoadSelection(bool bLoadSelection)
{
	m_bGameLoadSelection = bLoadSelection;
	return;
}

void CLoadSelectionDlg::SetMessage(SubMoney stData)
{
	CString strTotalMoney;
	CTransparentStatic* pStatic = NULL;
	CTransparentStatic* pStatic1 = NULL;
	CTransparentStatic* pStatic2 = NULL;
	CTransparentStatic* pStatic3 = NULL;


	m_subMoney = stData;
	m_nOldSelfMoney = m_subMoney.nSelfMoney;
	
	//if (m_subMoney.nSelfMoney < m_subMoney.nMaxMoney)
	//{
	//	// 防止金币没有最大代入多
	//	m_subMoney.nCurrentMoney = m_subMoney.nSelfMoney;
	//}

	//if (m_subMoney.nSelfMoney < m_subMoney.nMinMoney)
	//{
	//	m_subMoney.nCurrentMoney = m_subMoney.nSelfMoney;
	//}
	
	// 防止玩家本身金币，少于带入金币
	if (m_subMoney.nSelfMoney < m_subMoney.nCurrentMoney)
	{
		m_subMoney.nCurrentMoney = m_subMoney.nSelfMoney;
	}

	m_subMoney.nSelfMoney = m_nOldSelfMoney - m_subMoney.nCurrentMoney;
	
	ShowAllControl(SW_SHOW);
	ShowControl(IDT_NOTICES,SW_HIDE);
	pStatic = dynamic_cast<CTransparentStatic *>(GetControlById(IDC_TOTAL_MONEY));
	pStatic1 = dynamic_cast<CTransparentStatic *>(GetControlById(IDC_MAX_MONEY));
	pStatic2 = dynamic_cast<CTransparentStatic *>(GetControlById(IDC_MIN_MONEY));
	CTrueEdit *pEdit = dynamic_cast<CTrueEdit	*>(GetControlById(IDC_PLAN_MONEY));

	// 显示自己本身金额
	if (pStatic != NULL)
	{
		strTotalMoney.Format("%d", m_subMoney.nSelfMoney);
		pStatic->SetMyText(strTotalMoney.GetBuffer());
		pStatic->ShowWindow(SW_SHOW);
	}
	// 显示最高代入金额
	if (pStatic1 != NULL)
	{
		strTotalMoney.Format("%d", m_subMoney.nMaxMoney);
		pStatic1->SetMyText(strTotalMoney.GetBuffer());
		pStatic1->ShowWindow(SW_SHOW);		
	}
	// 显示最低代入金额
	if (pStatic2 != NULL)
	{
		strTotalMoney.Format("%d", m_subMoney.nMinMoney);
		pStatic2->SetMyText(strTotalMoney.GetBuffer());
		pStatic2->ShowWindow(SW_SHOW);
	}	

	// 显示准备代入金额
	if (pEdit != NULL)
	{
		// 设置编辑框只能输入数字
		DWORD dwStyle = pEdit->GetStyle(); 
		dwStyle |= ES_NUMBER; 
		SetWindowLong(pEdit->m_hWnd, GWL_STYLE, dwStyle);

		strTotalMoney.Format("%d", m_subMoney.nCurrentMoney);
		pEdit->SetWindowText(strTotalMoney.GetBuffer());
		pEdit->ShowWindow(SW_SHOW);
	}
	
	if (m_subMoney.bAuto)
	{
		// 设置定时确定代入
		SetAutoStart();
	}
	return;
}

void CLoadSelectionDlg::OnEnChangeEdit()
{
	CTrueEdit *pEdit = dynamic_cast<CTrueEdit *>(GetControlById(IDC_PLAN_MONEY));
	if (pEdit == NULL)
	{
		return ;
	}
	
	CString strMoney;
	pEdit->GetWindowText(strMoney);
	int nCurrentMoney = atoi(strMoney);

	// 金币数据不相等就更新
	m_subMoney.nCurrentMoney = nCurrentMoney;
	if (m_subMoney.nCurrentMoney < m_subMoney.nMinMoney)
	{
		m_subMoney.nCurrentMoney = m_subMoney.nMinMoney;
	}
	else if (m_subMoney.nCurrentMoney > m_subMoney.nMaxMoney)
	{
		m_subMoney.nCurrentMoney = m_subMoney.nMaxMoney;
	}

	// 防止代入金币超过自身金币
	if (m_subMoney.nCurrentMoney > m_nOldSelfMoney)
	{
		m_subMoney.nCurrentMoney = m_nOldSelfMoney;
		m_subMoney.nSelfMoney = 0;
	}
	else
	{
		m_subMoney.nSelfMoney = m_nOldSelfMoney - m_subMoney.nCurrentMoney;
	}

	// 设置代入金币
	strMoney.Format("%d", m_subMoney.nCurrentMoney);

	m_strEdit = strMoney;
	UpdateData(FALSE);  // 使用 pEdit->SetWindowText 会崩溃, 所以采用CString m_strEdit绑定对象使用
	//pEdit->SetWindowText(strMoney);

	// 设置光标位置
	pEdit->SetSel(m_strEdit.GetLength(), m_strEdit.GetLength());

	CTransparentStatic * pStatic = dynamic_cast<CTransparentStatic *>(GetControlById(IDC_TOTAL_MONEY));
	if (pStatic != NULL)
	{
		// 设置自己本身金币
		strMoney.Format("%d", m_subMoney.nSelfMoney);
		pStatic->SetMyText(strMoney.GetBuffer());
		pStatic->ShowWindow(SW_SHOW);
	}
}

void CLoadSelectionDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (IDT_MODIFY_MONEY == nIDEvent)
	{
		KillTimer(IDT_MODIFY_MONEY);

		// 定时调用确认带入按钮
		OnPostMessage(IDB_OK_LOAD, NULL);
	}
	BModalContainer::OnTimer(nIDEvent);
}

// 自动开始
void CLoadSelectionDlg::SetAutoStart()
{
	KillTimer(IDT_MODIFY_MONEY);
	SetTimer(IDT_MODIFY_MONEY, 5000, 0);
}