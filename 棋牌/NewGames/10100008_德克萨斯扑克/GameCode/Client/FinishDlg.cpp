#include "stdafx.h"
#include "FinishDlg.h"
#include "MiniGameEngine.h"

extern TCHAR *GlbGetFloatString(TCHAR pRet[], int nValue); 
#define IDT_NOTICES 900

CFinishDlg::CFinishDlg(DWORD pId,CWnd* pParent)
: BModalContainer(pId, pParent)
{
	m_pWndParent = pParent;
	m_bGameFinished = false;
}

CFinishDlg::~CFinishDlg()
{

}

/**
* 处理当前包容器中的控件消息
*
* @param pId 要处理的控件
* @param pParam 要处理的事件消息
*/
bool CFinishDlg::OnPostMessage(UINT pId, BBaseParameter* pParam)
{
	switch(pId)
	{
	case IDC_SHUT:
		{
			EndModalLoop(EVENT_CLOSE_MAINFRAME);
		}
		break;
	}
	return __super::OnPostMessage(pId,pParam);
}

/// 加载对话框
bool CFinishDlg::LoadConfig(std::string path)
{
	return __super::LoadConfig(path);
}

void CFinishDlg::SetNotice(std::string str,bool bShowNextBnt)
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

//设置成服务器游戏已Finish的状态
void CFinishDlg::SetGameFinish(bool bFinish)
{
	m_bGameFinished = bFinish;
	return;
}

void CFinishDlg::SetMessage(const TResult& Result)
{
	for (int i = 0; i < PLAY_COUNT; i++)
	{
		if (Result.nMoney[i] > 0)
		{
			for (int j = 0; j < 8; j++)
			{
				if (Result.nUserBet[i][j] > 0)
				{
					CString str;
					str.Format("dxh: 第%d个注池下注: %d", j+1, Result.nUserBet[i][j]);
					OutputDebugString(str);
				}
			}
		}
	}

	int nCount = 0;
	for (int i = 0; i < PLAY_COUNT; i++)
	{
		if (Result.nMoney[i] > 0)
		{
			nCount++;
		}
	}

	CRect rect;
	GetClientRect(&rect);
	rect.bottom = 56 + nCount * 25 + 33;
	MoveWindow(&rect);

	BzImage *pImage = dynamic_cast<BzImage*>(GetControlById(IDC_DIBIAN));
	if (pImage != NULL)
	{
		CRect rectControl;
		pImage->GetClientRect(&rectControl);
		int nHeight = rectControl.Height();
		rectControl.top = rect.bottom - 33;
		rectControl.bottom = rectControl.top + nHeight;
		pImage->MoveWindow(&rectControl);
	}

	CenterWindow();

	ShowAllControl(SW_SHOW);

	CString strTemp, strTemp1;
	CTransparentStatic* pStatic = NULL;

	nCount = 0;
	for (int i = 0; i < PLAY_COUNT; i++)
	{	
		BYTE bDeskStation = Result.bOrder[i];

		// 只显示赢家信息
		if (Result.nMoney[bDeskStation] > 0)
		{
			// 显示赢家ID
			pStatic = dynamic_cast<CTransparentStatic *>(GetControlById(IDC_VICTORY_ID + nCount));
			if (pStatic != NULL)
			{
				strTemp.Format("%d", Result.nbyUserID[bDeskStation]);
				pStatic->SetMyText(strTemp.GetBuffer());
				pStatic->Invalidate();
			}

			// 显示赢的金币
			pStatic = dynamic_cast<CTransparentStatic *>(GetControlById(IDC_WIN_MONEY + nCount));
			if (pStatic != NULL)
			{
				strTemp.Format("%d", Result.nMoneyEx[bDeskStation]);
				pStatic->SetMyText(strTemp.GetBuffer());
				pStatic->Invalidate();
			}
			
			// 显示所有下注池信息
			pStatic = dynamic_cast<CTransparentStatic *>(GetControlById(IDC_INFO + nCount));
			if (pStatic != NULL)
			{
				strTemp = "";
				for (int j = 0; j < 8; j++)
				{
					if (Result.bWinBetPools[bDeskStation][j])
					{
						// 在这个下注池赢了
						if (j == 0)
						{
							strTemp1.Format(" 主池: %d", Result.nWinPoolsMoney[bDeskStation][j]);
							strTemp = strTemp1;
						}
						else
						{
							strTemp1.Format(" 边池%d: %d", j, Result.nWinPoolsMoney[bDeskStation][j]);
							strTemp += strTemp1;
						}
					}
				}
			
				pStatic->SetMyText(strTemp.GetBuffer());
				pStatic->Invalidate();
			}	

			nCount++;
		}
	}

	if (Result.nSelfMoney)
	{
		SetNotice(_TEXT("您不足够金额继续在本桌进行游戏。"),false);
	}
	return;
}
