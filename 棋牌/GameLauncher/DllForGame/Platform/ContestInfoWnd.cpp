// ContestInfoWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "ContestInfoWnd.h"
#include "Resource.h"

// CContestInfoWnd 对话框

IMPLEMENT_DYNAMIC(CContestInfoWnd, CDialog)

CContestInfoWnd::CContestInfoWnd(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_CONTESTINFO, pParent)
{
	m_pGameInfo = NULL;
}

CContestInfoWnd::~CContestInfoWnd()
{
}

void CContestInfoWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CContestInfoWnd, CDialog)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


BOOL CContestInfoWnd::OnInitDialog()
{
	CDialog::OnInitDialog();
	TCHAR path[MAX_PATH]={0};


	lstrcpy(path,".\\image\\Contestinfobk.bmp");

	SetBkImage(path);
	return TRUE;
}


void CContestInfoWnd::OnCancel()
{
	return ;
	CDialog::OnCancel();
}

void CContestInfoWnd::OnOK()
{
	return ;
	CDialog::OnOK();
}
void CContestInfoWnd::SetBkImage(TCHAR  path[])
{
	m_bk.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	CGameImageHelper ImageHandle(&m_bk);
	HRGN hRgn=AFCBmpToRgn(ImageHandle,RGB(255,0,255),RGB(1,1,1));

	if (hRgn!=NULL)
	{
		SetWindowPos(NULL,0,0,ImageHandle.GetWidth(),ImageHandle.GetHeight(),SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOMOVE);
		SetWindowRgn(hRgn,TRUE);
	}

	Invalidate(false);
}

// CContestInfoWnd 消息处理程序
void CContestInfoWnd::OnPaint()
{
	CPaintDC dc(this);
	CGameImageHelper handle(&m_bk);
	handle.BitBlt(dc.GetSafeHdc(),0,0);

	if (m_pGameInfo != NULL)
	{
		CFont font;
		font.CreateFont(-16,0,0,0,700,NULL,NULL,NULL,134,3,2,1,2,TEXT("宋体"));
		CFont font1;
		font1.CreateFont(-12,0,0,0,400,NULL,NULL,NULL,134,3,2,1,2,TEXT("宋体"));
		dc.SelectObject(&font);
		dc.SetBkMode(TRANSPARENT);
		dc.SetTextColor(RGB(255,255,255));
		CRect rect[6];

		rect[0].SetRect(40, 14, 400, 34);
		for (int i = 1; i < 6; i++)
		{
			rect[i].SetRect(40, 34 + (i-1) * 20, 400, 44 + i * 20 + 30);
		}

		CString szInfo;
		if (m_pUserItem != NULL)
		{
			szInfo.Format("第 %d 局", m_pUserItem->GameUserInfo.iContestCount);
		}
		else
		{
			szInfo = "第 1 局";
		}
		dc.DrawText(szInfo,szInfo.GetLength(),rect[0],DT_TOP|DT_LEFT|DT_END_ELLIPSIS);
		szInfo = "积分方式: 积分 * 倍数";
		dc.SelectObject(&font1);
		dc.DrawText(szInfo,szInfo.GetLength(),rect[1],DT_TOP|DT_LEFT|DT_END_ELLIPSIS);

		szInfo.Format("游戏倍数: %d", m_pGameInfo->iBasePoint);
		dc.DrawText(szInfo,szInfo.GetLength(),rect[2],DT_TOP|DT_LEFT|DT_END_ELLIPSIS);
		szInfo.Format("满 %d 局以上可以参与排名", m_pGameInfo->iLowCount);
		dc.DrawText(szInfo,szInfo.GetLength(),rect[3],DT_TOP|DT_LEFT|DT_END_ELLIPSIS);
		szInfo.Format("低于 %I64d 分将被淘汰", m_pGameInfo->i64LowChip);
		dc.DrawText(szInfo,szInfo.GetLength(),rect[4],DT_TOP|DT_LEFT|DT_END_ELLIPSIS);	
		if (m_pUserItem != NULL)
		{
			szInfo.Format("积分: %I64d", m_pUserItem->GameUserInfo.i64ContestScore);
		}
		else
		{
			szInfo.Format("积分: %I64d", 0);
		}
		dc.DrawText(szInfo,szInfo.GetLength(),rect[5],DT_TOP|DT_LEFT|DT_END_ELLIPSIS);
	}
}

void CContestInfoWnd::SetGameInfo(GameInfoStructInExe* pGameInfo)
{
	m_pGameInfo = pGameInfo;
	Invalidate(FALSE);
}

void CContestInfoWnd::UpdateUser(UserItemStruct * pUserItem)
{
	m_pUserItem = pUserItem;
	Invalidate(FALSE);
}

void CContestInfoWnd::ShowWnd()
{
	m_bShow = true;
	CRect rect;
	GetClientRect(&rect);
	MapWindowPoints(GetParent(),&rect);
	CDialog::SetWindowPos(NULL,rect.left,0,0,0,SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOSIZE);
}
void CContestInfoWnd::HideWnd()
{
	m_bShow = false;
	CRect rect;
	GetClientRect(&rect);
	MapWindowPoints(GetParent(),&rect);
	CDialog::SetWindowPos(NULL,rect.left, -rect.bottom + 22 ,0,0,SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOSIZE);
}

void CContestInfoWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (!m_bShow)
		ShowWnd();
	else
		HideWnd();
	CDialog::OnLButtonDown(nFlags, point);
}