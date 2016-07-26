// MjSet.cpp : implementation file
//

#include "../stdafx.h"
#include "MjSet.h"
//#include "GameFrameDlg.h"
#include "mj.h"

// CMjSet dialog

IMPLEMENT_DYNAMIC(CMjSet, CDialog)

CMjSet::CMjSet(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_MJSET, pParent)
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

CMjSet::~CMjSet()
{
}

void CMjSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMjSet, CDialog)
	ON_COMMAND_RANGE(IDC_MJDESK_0,IDC_MJDESK_6,OnChoiceDesk)
END_MESSAGE_MAP()


// CMjSet message handlers

BOOL CMjSet::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	::PostMessage(::GetDlgItem(m_hWnd,IDC_MJDESK_0),BM_SETCHECK,1,1);

	m_iOldDesk = m_iCurrentDesk = 0;
	
	OnChoiceDesk(IDC_MJDESK_0);
	return TRUE;
}

void CMjSet::OnOK()
{
	//int sel,count=0;
	//SetPaiStruct paistruct;
	//int pai[42]=
	//{
	//	CMjEnum::MJ_TYPE_W1,CMjEnum::MJ_TYPE_W2,CMjEnum::MJ_TYPE_W3,CMjEnum::MJ_TYPE_W4,
	//	CMjEnum::MJ_TYPE_W5,CMjEnum::MJ_TYPE_W6,CMjEnum::MJ_TYPE_W7,CMjEnum::MJ_TYPE_W8,CMjEnum::MJ_TYPE_W9,

	//	CMjEnum::MJ_TYPE_B1,CMjEnum::MJ_TYPE_B2,CMjEnum::MJ_TYPE_B3,CMjEnum::MJ_TYPE_B4,
	//	CMjEnum::MJ_TYPE_B5,CMjEnum::MJ_TYPE_B6,CMjEnum::MJ_TYPE_B7,CMjEnum::MJ_TYPE_B8,CMjEnum::MJ_TYPE_B9,

	//	CMjEnum::MJ_TYPE_T1,CMjEnum::MJ_TYPE_T2,CMjEnum::MJ_TYPE_T3,CMjEnum::MJ_TYPE_T4,
	//	CMjEnum::MJ_TYPE_T5,CMjEnum::MJ_TYPE_T6,CMjEnum::MJ_TYPE_T7,CMjEnum::MJ_TYPE_T8,CMjEnum::MJ_TYPE_T9,

	//	CMjEnum::MJ_TYPE_FD,CMjEnum::MJ_TYPE_FN,CMjEnum::MJ_TYPE_FX,CMjEnum::MJ_TYPE_FB,CMjEnum::MJ_TYPE_FZ,
	//	CMjEnum::MJ_TYPE_FFC,CMjEnum::MJ_TYPE_FGB,

	//	CMjEnum::MJ_TYPE_FCHUN,CMjEnum::MJ_TYPE_FXIA,CMjEnum::MJ_TYPE_FQIU,CMjEnum::MJ_TYPE_FDONG,
	//	CMjEnum::MJ_TYPE_FMEI,CMjEnum::MJ_TYPE_FLAN,CMjEnum::MJ_TYPE_FZHU,CMjEnum::MJ_TYPE_FJU	
	//};
	//for(int i=0;i<42;i++)
	//{
	//	sel=((CComboBox *)GetDlgItem(m_id[i]))->GetCurSel();
	//	for(int j=0;j<sel;j++)
	//	{
	//		paistruct.pai[count++]=pai[i];
	//		if(count==20)
	//			break;
	//	}
	//	if(count==20)
	//		break;
	//}
	//if(count<13)
	//	return ;
	SetCurrentData(m_iCurrentDesk);

	((CLoveSendClassInExe *)GetParent())->SendGameData(&m_paistruct,sizeof(m_paistruct),MDM_GM_GAME_NOTIFY,79,0);
	CDialog::OnOK();
}
//选牌数
void CMjSet::OnChoiceDesk(UINT nID)
{
	UpdateData(TRUE);
	//当前选项ID
	m_iCurrentDesk = (nID - IDC_MJDESK_0);

	//设置当前牌
	SetCurrentData(m_iOldDesk);
	
	//换位置中出现默认选项清零
	if(m_iOldDesk == 0 || m_iCurrentDesk == 0)
		for(int i  = 0; i < 54; i++)
			m_RemainCard[i] = 4;
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
BOOL CMjSet::UpdateComboBox()
{
	int ComboBoxArray[42] =
	{
		IDC_WAN_1,IDC_WAN_2,IDC_WAN_3,IDC_WAN_4,IDC_WAN_5,IDC_WAN_6,IDC_WAN_7,IDC_WAN_8,IDC_WAN_9,
		IDC_BIN_1,IDC_BIN_2,IDC_BIN_3,IDC_BIN_4,IDC_BIN_5,IDC_BIN_6,IDC_BIN_7,IDC_BIN_8,IDC_BIN_9,
		IDC_TIAO_1,IDC_TIAO_2,IDC_TIAO_3,IDC_TIAO_4,IDC_TIAO_5,IDC_TIAO_6,IDC_TIAO_7,IDC_TIAO_8,IDC_TIAO_9,
		IDC_DENG,IDC_NAN,IDC_XI,IDC_BEI,IDC_ZHONG,IDC_FA,IDC_BAI,
		IDC_CUN,IDC_XIA,IDC_QIU,IDC_DONG,
		IDC_MEI,IDC_LAN,IDC_ZHU,IDC_JU
	};

	//添加新数组
	TCHAR sz[100];
	for(int i = 0; i < 42; i ++)
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

//设置当前数据
void  CMjSet::SetCurrentData(int iDesk)
{
	//控件数组
	int ComboBoxArray[42] =
	{
		IDC_WAN_1,IDC_WAN_2,IDC_WAN_3,IDC_WAN_4,IDC_WAN_5,IDC_WAN_6,IDC_WAN_7,IDC_WAN_8,IDC_WAN_9,
		IDC_BIN_1,IDC_BIN_2,IDC_BIN_3,IDC_BIN_4,IDC_BIN_5,IDC_BIN_6,IDC_BIN_7,IDC_BIN_8,IDC_BIN_9,
		IDC_TIAO_1,IDC_TIAO_2,IDC_TIAO_3,IDC_TIAO_4,IDC_TIAO_5,IDC_TIAO_6,IDC_TIAO_7,IDC_TIAO_8,IDC_TIAO_9,
		IDC_DENG,IDC_NAN,IDC_XI,IDC_BEI,IDC_ZHONG,IDC_FA,IDC_BAI,
		IDC_CUN,IDC_XIA,IDC_QIU,IDC_DONG,
		IDC_MEI,IDC_LAN,IDC_ZHU,IDC_JU
	};
	//某位置清零
	m_paistruct.paicount[iDesk] = 0;
	
	for(int i = 0; i < 42; i++)
	{
		int sel = ((CComboBox *)GetDlgItem(ComboBoxArray[i]))->GetCurSel();
		
		for(int j = 0; j < sel; j ++)
		{
			//某种牌剩余张数
			m_RemainCard[i] -= 1;

			m_paistruct.pai[iDesk][m_paistruct.paicount[iDesk]++] = GetCardByValue(i);
			
			if(m_paistruct.paicount[iDesk] >= 20)
				break;
		}
			
		if(m_paistruct.paicount[iDesk] >= 20)
				break;	
	}
}

//从值得到牌
int CMjSet::GetCardByValue(int Value)
{
	int pai[42]=
	{
		CMjEnum::MJ_TYPE_W1,CMjEnum::MJ_TYPE_W2,CMjEnum::MJ_TYPE_W3,CMjEnum::MJ_TYPE_W4,
		CMjEnum::MJ_TYPE_W5,CMjEnum::MJ_TYPE_W6,CMjEnum::MJ_TYPE_W7,CMjEnum::MJ_TYPE_W8,CMjEnum::MJ_TYPE_W9,

		CMjEnum::MJ_TYPE_B1,CMjEnum::MJ_TYPE_B2,CMjEnum::MJ_TYPE_B3,CMjEnum::MJ_TYPE_B4,
		CMjEnum::MJ_TYPE_B5,CMjEnum::MJ_TYPE_B6,CMjEnum::MJ_TYPE_B7,CMjEnum::MJ_TYPE_B8,CMjEnum::MJ_TYPE_B9,

		CMjEnum::MJ_TYPE_T1,CMjEnum::MJ_TYPE_T2,CMjEnum::MJ_TYPE_T3,CMjEnum::MJ_TYPE_T4,
		CMjEnum::MJ_TYPE_T5,CMjEnum::MJ_TYPE_T6,CMjEnum::MJ_TYPE_T7,CMjEnum::MJ_TYPE_T8,CMjEnum::MJ_TYPE_T9,

		CMjEnum::MJ_TYPE_FD,CMjEnum::MJ_TYPE_FN,CMjEnum::MJ_TYPE_FX,CMjEnum::MJ_TYPE_FB,CMjEnum::MJ_TYPE_FZ,
		CMjEnum::MJ_TYPE_FFC,CMjEnum::MJ_TYPE_FGB,

		CMjEnum::MJ_TYPE_FCHUN,CMjEnum::MJ_TYPE_FXIA,CMjEnum::MJ_TYPE_FQIU,CMjEnum::MJ_TYPE_FDONG,
		CMjEnum::MJ_TYPE_FMEI,CMjEnum::MJ_TYPE_FLAN,CMjEnum::MJ_TYPE_FZHU,CMjEnum::MJ_TYPE_FJU	
	};
	return pai[Value];
}