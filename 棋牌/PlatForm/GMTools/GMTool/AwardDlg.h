#pragma once

//////////////////////////////////////////////////////////////////////////
///奖励弹出窗口，让用户填写相应数据，然后通知主窗口
///2010.8.12 rongqiufen
//////////////////////////////////////////////////////////////////////////

#include "afxwin.h"

#include "GM_MessageDefine.h"
#include "CommandMessage.h"
#include "comstruct.h"

#include <map>
#include <vector>
using namespace std;

// CAwardDlg 对话框

class CAwardDlg : public CDialog
{
	DECLARE_DYNAMIC(CAwardDlg)

public:
	CAwardDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAwardDlg();

	///显示房间前必须设置这些信息

	///设置用户信息（用于大厅当前房间）
	void SetUserInfo(UserInfoStruct *userinfo, int iRoomID);
	///设置用户信息（用于大厅非当前房间，从后面连接W的）
	void SetUserInfo(GM_OperatedPlayerInfo_t *userinfo);
	///设置奖励方案表
	void SetAwardProjectList(map<int, GM_AwardType_t*> vAwardProjectList);
	///设置道具名称表
	void SetAwardList(map<int, GM_GetPropInfo_t*> vAwardList);

// 对话框数据
	enum { IDD = IDD_DIALOG4 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	UINT m_UserID;							///用户ID
	CString m_UserNickName;					///用户昵称
	CString m_UserLoginName;				///用户登录名
	CComboBox m_AwardCmb;					///奖励方案下拉框
	CString m_Reason;						///原因
	CString m_Details;						///方案详情	
	int m_RoomID;							///房间ID

	vector<GM_AwardType_t*> m_AwardType;	///奖励方案表
	map<int, GM_GetPropInfo_t*> m_PropList;	///道具名称表

	GM_OperateByID_t	m_GMOperate;		///存储奖励操作数据
public:
	afx_msg void OnBnClickedOk();
public:
	afx_msg void OnCbnSelchangeCombo1();
protected:
	virtual void OnOK();
public:
	virtual BOOL OnInitDialog();

};
