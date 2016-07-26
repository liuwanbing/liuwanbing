#include "stdafx.h"

#include "SoundCfgDlg.h"
#include "MiniGameEngine.h"

extern TCHAR *GlbGetFloatString(TCHAR pRet[], int nValue); 

#define IDT_NOTICES		900

#define IDC_RDO_FIFANG_BASE		201
#define IDC_CHK_SOUND_BASE		211


CSoundCfgDlg::CSoundCfgDlg(DWORD pId,CWnd* pParent)
	: BModalContainer(pId, pParent),m_iniFile(NULL)
{

	//获取父窗口指针
	m_pWndParent = pParent;

	//背景音乐开关选择控件
	m_pChkBgMusic = NULL;

	//游戏音效开关选择控件
	m_pChkGameSound = NULL;

	//标准语音选择控件
	m_pRdoStander = NULL;

	//地方语音选择控件
	m_pRdoDiFang = NULL;
	
}

CSoundCfgDlg::~CSoundCfgDlg()
{

}

/**
 * 处理当前包容器中的控件消息
 *
 * @param pId 要处理的控件
 * @param pParam 要处理的事件消息
 */
bool CSoundCfgDlg::OnPostMessage(UINT pId, BBaseParameter* pParam)
{
	switch(pId)
	{
	case EVENT_CONTINUE_MAINFRAME: //确定消息响应
		{
			OnClickedOK(pParam);
		}
		break;
	case EVENT_CLOSE_MAINFRAME:	//取消消息响应
		{

		}
		break;
	}

	//地方语言选择
	if (pId >= IDC_RDO_FIFANG_BASE && pId <= IDC_RDO_FIFANG_BASE + 1)
	{
		OnSelectDiFang(pId);
	}


	return __super::OnPostMessage(pId,pParam);
}

/// 加载对话框
bool CSoundCfgDlg::LoadConfig(std::string path)
{
	if (__super::LoadConfig(path))
	{
		m_pChkBgMusic	= dynamic_cast<BzRadioButton*>(GetControlById(IDC_CHK_SOUND_BASE));
		m_pChkGameSound = dynamic_cast<BzRadioButton*>(GetControlById(IDC_CHK_SOUND_BASE + 1));

		m_pRdoStander = dynamic_cast<BzRadioButton*>(GetControlById(IDC_RDO_FIFANG_BASE));
		m_pRdoDiFang  = dynamic_cast<BzRadioButton*>(GetControlById(IDC_RDO_FIFANG_BASE + 1));


		CString strFileName;
		strFileName.Format("%sbzGame.ini",CINIFile::GetAppPath());
		m_iniFile.SetINIFileName(strFileName);

		int nChkBgMusic		= m_iniFile.GetKeyVal(_TEXT("SoundCfg"),_TEXT("BK_Music"),1);
		int nChkGameSound	= m_iniFile.GetKeyVal(_TEXT("SoundCfg"),_TEXT("Game_Sound"),1);

		if (m_pChkBgMusic)
		{
			m_pChkBgMusic->SetChecked(nChkBgMusic == 1);
		}

		if (m_pChkGameSound)
		{
			m_pChkGameSound->SetChecked(nChkGameSound == 1);

		}

		int nSelDiFang = m_iniFile.GetKeyVal(_TEXT("SoundCfg"),_TEXT("DiFang_Select"),0);

		if(m_pRdoStander)
		{
			m_pRdoStander->SetChecked(nSelDiFang == 0);
		}

		if (m_pRdoDiFang)
		{
			m_pRdoDiFang->SetChecked(nSelDiFang == 1);
		}

		return true;
	}

	return false;
}

/// 地方语言的选择事件响应
void CSoundCfgDlg::OnSelectDiFang(UINT uiID)
{
	if (m_pRdoStander)
	{
		m_pRdoStander->SetChecked(uiID == IDC_RDO_FIFANG_BASE);
	}

	if (m_pRdoDiFang)
	{
		m_pRdoDiFang->SetChecked(uiID == IDC_RDO_FIFANG_BASE + 1);
	}

	return;
}

/// 对话框确定
void CSoundCfgDlg::OnClickedOK(BBaseParameter* pBBPOK)
{
	CMiniGameEngine* pGameDlg = (CMiniGameEngine*)m_pWndParent;

	CString strValue = "0";
	BYTE  byCfgType = 0;
	DWORD dwValue = 0;

	//保存并生效游戏声音设置

	if (m_pChkBgMusic)
	{
		if (m_pChkBgMusic->isChecked())
		{
			strValue = "1";
			dwValue = dwValue | 0x0001L;
		}
		else
		{
			strValue = "0";
		}

		m_iniFile.SetKeyValString(_TEXT("SoundCfg"),_TEXT("BK_Music"),strValue);
	}

	if (m_pChkGameSound)
	{
		if (m_pChkGameSound->isChecked())
		{
			strValue = "1";
			dwValue = dwValue | 0x0002L;
		}
		else
		{
			strValue = "0";
		}
		m_iniFile.SetKeyValString(_TEXT("SoundCfg"),_TEXT("Game_Sound"),strValue);
	}

	byCfgType = 0x01;
	pBBPOK->SetValue(IDB_SOUND_CFG_OK,(BYTE)byCfgType);
	pBBPOK->SetValue(IDB_SOUND_CFG_OK,(DWORD)dwValue);
	pGameDlg->OnPostMessage(IDB_SOUND_CFG_OK,pBBPOK);

	byCfgType = 0;
	dwValue = 0;

	////保存并生效地方语音设置

	//if(m_pRdoStander)
	//{
	//	if (m_pRdoStander->isChecked())
	//	{
	//		strValue = "0";
	//		dwValue = 0;
	//	}
	//}

	//if(m_pRdoDiFang)
	//{
	//	if (m_pRdoDiFang->isChecked())
	//	{
	//		strValue = "1";
	//		dwValue = 1;
	//	}
	//}

	//m_iniFile.SetKeyValString(_TEXT("SoundCfg"),_TEXT("DiFang_Select"),strValue);

	//byCfgType = 0x02;
	//pBBPOK->SetValue(IDB_SOUND_CFG_OK,(BYTE)byCfgType);
	//pBBPOK->SetValue(IDB_SOUND_CFG_OK,(DWORD)dwValue);
	//pGameDlg->OnPostMessage(IDB_SOUND_CFG_OK,pBBPOK);


	return;
}