#pragma once

#include "GameLanchSDK/bzwUiInclude/bzwUI.h"
#include "defines.h"
//#include "../../../../sdk/include/GameLanchSdk/LoveSendClassInExe.h"
#include "..\GameMessage\UpGradeMessage.h"
#include "Common\commonuse.h"

class CSoundCfgDlg : public BModalContainer
{
public:

	CSoundCfgDlg(DWORD pId,CWnd* pParent=NULL);

	~CSoundCfgDlg();

	/// 处理当前包容器中的控件消息
	virtual bool OnPostMessage(UINT pId,BBaseParameter* pParam);

	/// 加载对话框
	virtual bool LoadConfig(std::string path);

public:

	/// 地方语言的选择事件响应
	void OnSelectDiFang(UINT uiID);

	/// 对话框确定
	void OnClickedOK(BBaseParameter* pBBPOK);

private:
	CSoundCfgDlg();

private:

	CWnd* m_pWndParent;

	//INI文件对象
	CINIFile m_iniFile;

	//背景音乐开关选择控件
	BzRadioButton* m_pChkBgMusic;

	//游戏音效开关选择控件
	BzRadioButton* m_pChkGameSound;

	//标准语音选择控件
	BzRadioButton* m_pRdoStander;

	//地方语音选择控件
	BzRadioButton* m_pRdoDiFang;

};