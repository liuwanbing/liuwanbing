#pragma once

#include "GameLanchSDK/bzwUiInclude/bzwUI.h"
#include "defines.h"
//#include "../../../../sdk/include/GameLanchSdk/LoveSendClassInExe.h"
#include "..\GameMessage\UpGradeMessage.h"

#define  IDT_MODIFY_MONEY	1	//  等待玩家修改带入金额

struct SubMoney
{
	int nMinMoney;     // 最小代入金币
	int nMaxMoney;     // 最大代入金币
	int nCurrentMoney; // 当前要代入的金币
	int nSelfMoney;    // 自己本人拥有的金币
	int nMinusMoney ;  // 修改注额加减金币
	bool bAuto;        // 自动代入
	SubMoney()
	{
		memset(this, 0, sizeof(SubMoney));
	}
};

class CLoadSelectionDlg : public BModalContainer
{
public:

	CLoadSelectionDlg(DWORD pId,CWnd* pParent=NULL);

	~CLoadSelectionDlg();

	void SetMessage(SubMoney stData);

	/// 处理当前包容器中的控件消息
	virtual bool OnPostMessage(UINT pId,BBaseParameter* pParam);

	/// 加载对话框
	virtual bool LoadConfig(std::string path);

	// 写入金额
	void WriteMoney();

public:

	//设置成只能退出的状态，因为没有玩家，只有自己在桌的话，只能退出
	void SetNotice(std::string path,bool bShowNextBnt);

	//设置成服务器游戏已Finish的状态
	void SetGameLoadSelection(bool bLoadSelection);

	// 自动开始
	void SetAutoStart();
private:
	CLoadSelectionDlg();

private:
	CWnd* m_pWndParent;
	bool m_bGameLoadSelection;
	CString m_strEdit;        // 当前代入金币

public:
	SubMoney m_subMoney;      // 代入数据
	int      m_nOldSelfMoney; // 自己本人拥有的金币备份
	
public:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	afx_msg void OnEnChangeEdit();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};