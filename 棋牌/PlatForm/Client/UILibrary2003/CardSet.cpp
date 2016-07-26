// MjSet.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "CardSet.h"
#include "GameFrameDlg.h"
// CCardSet dialog

IMPLEMENT_DYNAMIC(CCardSet, CDialog)

CCardSet::CCardSet(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_CARD_SET, pParent)
{
	::memset(&m_paistruct,0,sizeof(m_paistruct));
	//坐号
	 m_iCurrentDesk = 0;
	 //
	 ::memset(m_RemainCard,0,sizeof(m_RemainCard));
	//
	 for(int i = 0; i < 8; i ++)
		 m_paistruct.paicount [i] = 0;
}

CCardSet::~CCardSet()
{
}

void CCardSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCardSet, CDialog)
	ON_COMMAND_RANGE(IDC_CARD_1,IDC_CARD_4,OnChoiceCard)
	ON_COMMAND_RANGE(IDC_DESK_0,IDC_DESK_6,OnChoiceDesk)
END_MESSAGE_MAP()


// CCardSet message handlers
 
BOOL CCardSet::OnInitDialog()
{
	CDialog::OnInitDialog();
	::PostMessage(::GetDlgItem(m_hWnd,IDC_CARD_1),BM_SETCHECK,1,1);

	::PostMessage(::GetDlgItem(m_hWnd,IDC_DESK_0),BM_SETCHECK,1,1);

	//更新控件
	OnChoiceCard(IDC_CARD_1);
	//
	m_iOldDesk = m_iCurrentDesk = 0;
	return TRUE;
}

//设置当前数据
void  CCardSet::SetCurrentData(int iDesk)
{
	//控件数组
	int ComboBoxArray[54] =
	{
		IDC_FANGKUAI_2,IDC_FANGKUAI_3,IDC_FANGKUAI_4,
		IDC_FANGKUAI_5,IDC_FANGKUAI_6,IDC_FANGKUAI_7,IDC_FANGKUAI_8,
		IDC_FANGKUAI_9,IDC_FANGKUAI_10,IDC_FANGKUAI_J,IDC_FANGKUAI_Q,
		IDC_FANGKUAI_K,IDC_FANGKUAI_1,
		IDC_MEIHUA_2,IDC_MEIHUA_3,IDC_MEIHUA_4,
		IDC_MEIHUA_5,IDC_MEIHUA_6,IDC_MEIHUA_7,IDC_MEIHUA_8,
		IDC_MEIHUA_9,IDC_MEIHUA_10,IDC_MEIHUA_J,IDC_MEIHUA_Q,
		IDC_MEIHUA_K,IDC_MEIHUA_1,
		IDC_HONGTAO_2,IDC_HONGTAO_3,IDC_HONGTAO_4,
		IDC_HONGTAO_5,IDC_HONGTAO_6,IDC_HONGTAO_7,IDC_HONGTAO_8,
		IDC_HONGTAO_9,IDC_HONGTAO_10,IDC_HONGTAO_J,IDC_HONGTAO_Q,
		IDC_HONGTAO_K,IDC_HONGTAO_1,
		IDC_HEITAO_2,IDC_HEITAO_3,IDC_HEITAO_4,
		IDC_HEITAO_5,IDC_HEITAO_6,IDC_HEITAO_7,IDC_HEITAO_8,
		IDC_HEITAO_9,IDC_HEITAO_10,IDC_HEITAO_J,IDC_HEITAO_Q,
		IDC_HEITAO_K,IDC_HEITAO_1,
		IDC_WANG_1,IDC_WANG_2
	};
	//某位置清零
	m_paistruct.paicount[iDesk] = 0;
	
	for(int i = 0; i < 54; i++)
	{
		int sel = ((CComboBox *)GetDlgItem(ComboBoxArray[i]))->GetCurSel();
		
		for(int j = 0; j < sel; j ++)
		{
			//某种牌剩余张数
			m_RemainCard[i] -= 1;

			m_paistruct.pai[iDesk][m_paistruct.paicount[iDesk]++] = GetCardByValue(i);
			
			if(m_paistruct.paicount[iDesk] >= 54)
				break;
		}
			
		if(m_paistruct.paicount[iDesk] >= 54)
				break;	
	}
}
//设置完毕
void CCardSet::OnOK()
{
	SetCurrentData(m_iCurrentDesk);

	((CLoveSendClass *)GetParent())->SendGameData(&m_paistruct,sizeof(m_paistruct),MDM_GM_GAME_NOTIFY,79,0);
	CDialog::OnOK();
}

//选牌数
void CCardSet::OnChoiceCard(UINT nID)
{
	UpdateData(TRUE);
	//牌副数
	m_iCardCount = (nID - IDC_CARD_1) + 1;
	
	for(int i  = 0; i < 54; i++)
			m_RemainCard[i] = m_iCardCount;

	//清
	 for(int i = 0; i < 8; i ++)
		 m_paistruct.paicount [i] = 0;
	//重新设置
	::PostMessage(::GetDlgItem(m_hWnd,IDC_DESK_0),BM_SETCHECK,1,1);
	::PostMessage(::GetDlgItem(m_hWnd,IDC_DESK_1),BM_SETCHECK,0,0);
	::PostMessage(::GetDlgItem(m_hWnd,IDC_DESK_2),BM_SETCHECK,0,0);
	::PostMessage(::GetDlgItem(m_hWnd,IDC_DESK_3),BM_SETCHECK,0,0);
	::PostMessage(::GetDlgItem(m_hWnd,IDC_DESK_4),BM_SETCHECK,0,0);
	::PostMessage(::GetDlgItem(m_hWnd,IDC_DESK_5),BM_SETCHECK,0,0);

	////剩余数组
	//::memset(m_RemainCard,m_iCardCount,sizeof(m_RemainCard));
	//更新控件
	UpdateComboBox();
	return ;
}

//选牌数
void CCardSet::OnChoiceDesk(UINT nID)
{
	UpdateData(TRUE);
	//当前选项ID
	m_iCurrentDesk = (nID - IDC_DESK_0);

	//设置当前牌
	SetCurrentData(m_iOldDesk);
	
	//换位置中出现默认选项清零
	if(m_iOldDesk == 0 || m_iCurrentDesk == 0)
		for(int i  = 0; i < 54; i++)
			m_RemainCard[i] = m_iCardCount;
	//清零
	if(m_iCurrentDesk == 0)
		::memset(&m_paistruct,0,sizeof(m_paistruct));
	//保存历史坐号
	m_iOldDesk = m_iCurrentDesk;
	//更新控件
	UpdateComboBox();
	return ;
}
//更新控件
BOOL CCardSet::UpdateComboBox()
{
	int ComboBoxArray[54] =
	{
		IDC_FANGKUAI_2,IDC_FANGKUAI_3,IDC_FANGKUAI_4,
		IDC_FANGKUAI_5,IDC_FANGKUAI_6,IDC_FANGKUAI_7,IDC_FANGKUAI_8,
		IDC_FANGKUAI_9,IDC_FANGKUAI_10,IDC_FANGKUAI_J,IDC_FANGKUAI_Q,
		IDC_FANGKUAI_K,IDC_FANGKUAI_1,
		IDC_MEIHUA_2,IDC_MEIHUA_3,IDC_MEIHUA_4,
		IDC_MEIHUA_5,IDC_MEIHUA_6,IDC_MEIHUA_7,IDC_MEIHUA_8,
		IDC_MEIHUA_9,IDC_MEIHUA_10,IDC_MEIHUA_J,IDC_MEIHUA_Q,
		IDC_MEIHUA_K,IDC_MEIHUA_1,
		IDC_HONGTAO_2,IDC_HONGTAO_3,IDC_HONGTAO_4,
		IDC_HONGTAO_5,IDC_HONGTAO_6,IDC_HONGTAO_7,IDC_HONGTAO_8,
		IDC_HONGTAO_9,IDC_HONGTAO_10,IDC_HONGTAO_J,IDC_HONGTAO_Q,
		IDC_HONGTAO_K,IDC_HONGTAO_1,
		IDC_HEITAO_2,IDC_HEITAO_3,IDC_HEITAO_4,
		IDC_HEITAO_5,IDC_HEITAO_6,IDC_HEITAO_7,IDC_HEITAO_8,
		IDC_HEITAO_9,IDC_HEITAO_10,IDC_HEITAO_J,IDC_HEITAO_Q,
		IDC_HEITAO_K,IDC_HEITAO_1,
		IDC_WANG_1,IDC_WANG_2
	};

	//添加新数组
	TCHAR sz[100];
	for(int i = 0; i < 54; i ++)
	{
		//清除原有数据
		((CComboBox *)GetDlgItem(ComboBoxArray[i]))->ResetContent();
		
		for(int j = 0; j <= m_RemainCard[i]; j ++)
		{
			wsprintf(sz,"%d",j);
			((CComboBox *)GetDlgItem(ComboBoxArray[i]))->AddString(sz);
		}
		((CComboBox *)GetDlgItem(ComboBoxArray[i]))->SetCurSel(0);
	};

	UpdateData(FALSE);

	return TRUE;
}

//从值得到牌
BYTE CCardSet::GetCardByValue(int Value)
{
	BYTE CardArray[54]={
		0x01,0x02, 0x03,0x04,
		0x05, 0x06 ,0x07,0x08 ,
		0x09,0x0A, 0x0B,0x0C,
		0x0D,
		0x11,0x12, 0x13,0x14,
		0x15, 0x16 ,0x17,0x18 ,
		0x19,0x1A, 0x1B,0x1C,
		0x1D,
		0x21,0x22, 0x23,0x24,
		0x25, 0x26 ,0x27,0x28 ,
		0x29,0x2A, 0x2B,0x2C,
		0x2D,
		0x31,0x32, 0x33,0x34,
		0x35, 0x36 ,0x37,0x38 ,
		0x39,0x3A, 0x3B,0x3C,
		0x3D,
		0x4E, 0x4F
	};

	return CardArray[Value];
}