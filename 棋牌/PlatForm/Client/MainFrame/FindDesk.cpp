#include "Stdafx.h"
#include "FindDesk.h"
#include "GameRoomEx.h"
#include "DeskFrameView.h"

BEGIN_MESSAGE_MAP(CFindDesk, CAFCBaseDialog)
	ON_BN_CLICKED(IDC_SITDOWN, OnBnClickedSitdown)
END_MESSAGE_MAP()

//构造函数
CFindDesk::CFindDesk(CWnd * pParent) : CAFCBaseDialog(IDD_FIND_DESK,pParent)
{
	m_bFindOne=false;			
	m_bFindTwo=false;			
	m_bFindAllNull=false;		
	m_bFindNoAllNull=false;		
	m_bLastFindDesk=255;
	m_pParent=pParent;
}

//析构函数
CFindDesk::~CFindDesk()
{
}

//控件绑定函数
void CFindDesk::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_BtFind);
	DDX_Control(pDX, IDCANCEL, m_BtCancel);
	DDX_Control(pDX, IDC_SITDOWN, m_BtSitDown);
}

//初始化函数
BOOL CFindDesk::OnInitDialog() 
{
	__super::OnInitDialog();

	//设置变量
	if (m_bFindOne) ((CButton *)GetDlgItem(IDC_FIND_ONE_NULL))->SetCheck(BST_CHECKED);
	if (m_bFindTwo) ((CButton *)GetDlgItem(IDC_FIND_TWO_NULL))->SetCheck(BST_CHECKED);
	if (m_bFindNoAllNull) ((CButton *)GetDlgItem(IDC_NOT_ALL_NULL))->SetCheck(BST_CHECKED);
	if (m_bFindAllNull) ((CButton *)GetDlgItem(IDC_ALL_NULL))->SetCheck(BST_CHECKED);

	return TRUE; 
}

//取消
void CFindDesk::OnCancel() 
{
	m_bLastFindDesk=255;
	DestroyWindow();
}

//找座位
void CFindDesk::OnOK() 
{
	//定义变量
	CGameRoomEx * pGameRoom=(CGameRoomEx *)m_pParent;
	CDeskFrameView * pDeskFrame=&pGameRoom->m_DeskFrame;
	bool bFindOne=(((CButton *)GetDlgItem(IDC_FIND_ONE_NULL))->GetCheck()!=0)?true:false;
	bool bFindTwo=(((CButton *)GetDlgItem(IDC_FIND_TWO_NULL))->GetCheck()!=0)?true:false;
	bool bFindNoAllNull=(((CButton *)GetDlgItem(IDC_NOT_ALL_NULL))->GetCheck()!=0)?true:false;
	bool bFindAllNull=(((CButton *)GetDlgItem(IDC_ALL_NULL))->GetCheck()!=0)?true:false;

	//效验参数
	if ((bFindOne==false)&&(bFindTwo==false)&&(bFindNoAllNull==false)&&(bFindAllNull==false))
	{
		//AfxMessageBox(TEXT("你要找座位，都要给个条件的，选择你要的条件吧！"),MB_ICONQUESTION);
		AfxMessageBox(TEXT("请先选择条件！"),MB_ICONQUESTION);
		return;
	}

	//保存查找条件
	if (bFindOne!=m_bFindOne)				{ m_bFindOne=bFindOne;m_bLastFindDesk=255; }
	if (bFindTwo!=m_bFindTwo)				{ m_bFindTwo=bFindTwo;m_bLastFindDesk=255; }
	if (bFindNoAllNull!=m_bFindNoAllNull)	{ m_bFindNoAllNull=bFindNoAllNull;m_bLastFindDesk=255; }
	if (bFindAllNull!=m_bFindAllNull)		{ m_bFindAllNull=bFindAllNull;m_bLastFindDesk=255; }

	//开始查找位置
	m_bLastFindDesk=pDeskFrame->FindGameDesk(m_bLastFindDesk,bFindOne,bFindTwo,bFindNoAllNull,bFindAllNull);
	if (m_bLastFindDesk!=255)
	{
		CString strBuffer;
		strBuffer.Format(TEXT("找到 %2d 号游戏桌符合条件"),m_bLastFindDesk+1);
		GetDlgItem(IDC_RESULT_MESSAGE)->SetWindowText(strBuffer);
	}
	else GetDlgItem(IDC_RESULT_MESSAGE)->SetWindowText(TEXT("没有符合条件的桌子，请修改条件查找！"));

	return;	
}

//坐下
void CFindDesk::OnBnClickedSitdown()
{
	if (m_bLastFindDesk!=255)
	{
		CGameRoomEx * pGameRoom=(CGameRoomEx *)m_pParent;
		pGameRoom->m_dwCurrentUserID=0L;
		BYTE bDeskStation=pGameRoom->m_DeskFrame.GetNullStation(m_bLastFindDesk);
		if (bDeskStation!=255)
		{
			pGameRoom->PostMessage(WM_LEFT_MOUSE_HIT_DESK,m_bLastFindDesk,bDeskStation);
			OnCancel();
		}
		else 
			//GetDlgItem(IDC_RESULT_MESSAGE)->SetWindowText("你的位置已经被人捷足先登了！");
			GetDlgItem(IDC_RESULT_MESSAGE)->SetWindowText("你的位置已经被人捷足先登了！");
		return;
	}

	//AfxMessageBox(TEXT("没有符合要求的位置，请修改条件查找！"),MB_ICONQUESTION);
	AfxMessageBox(TEXT("没能找到符合要求的位置，请修改条件查找！"),MB_ICONQUESTION);

	return;
}

