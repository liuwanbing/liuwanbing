// TestDlg.h : 头文件
//

#pragma once

//#include "ChangeFile/CGameDataEx.h"
//#include "ChangeFile/CUserDataEx.h"
//#include "ChangeFile/CCheckHuPaiEx.h"

#include "ChangeFile/CSrvLogicEx.h"


#include "commonuse.h"
#include "resource.h"		// 主符号

// CTestDlg 对话框
class CTestDlg : public CDialog
{
// 构造
public:
	CTestDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	BYTE m_byArHandPai[4][HAND_CARD_NUM];


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()



public:

	BYTE m_byNowStation;				//当前选牌玩家将
	BYTE m_byHandPai[4][HAND_CARD_NUM];	//玩家的手牌
	GCPStruct m_stCPGData[4][5];		//玩家吃碰杠数据

	BYTE m_byHuStation1;//糊牌玩家1
	BYTE m_byHuStation2;//糊牌玩家2
	BYTE m_byHuStation3;//糊牌玩家3

	BYTE m_byDianPao;//点炮玩家
	BYTE m_byLastPai;//最后得到的牌

	BYTE m_byMainJing;//正精
	BYTE m_byLessJing;//副精

	bool m_bZimo;//是否自摸糊牌

	CString m_sData;//数据字符串


	BYTE m_bGameStation;//游戏状态

public:

	///将牌信息添加到列表控件中
	void AddPaiStrToCom(CComboBox *pCtrl);
	///将位置信息添加到列表控件中
	void AddStationStrToCom(CComboBox *pCtrl);
	///将吃碰杠类型信息添加到列表控件中
	void AddCPGTypeStrToCom(CComboBox *pCtrl);

	///获取下拉列表牌的数据(测试用)
	int GetComboPaiData(CComboBox * pCtrl);
	///获取下拉列表位置的数据(测试用)
	int GetComboStationData(CComboBox * pCtrl);
	///获取下拉列表吃碰杠牌类型的数据(测试用)
	int GetComboCPGTypeData(CComboBox * pCtrl);

	///根据当前数据设置控件状态
	void SetCtrlStatue();

	//将字符串解析成牌数据
	BYTE ChangeStrToData(CString st);

	//获取糊牌类型
	void GetHuPaiType(CString & str,BYTE hupaitype[]);

public:
	afx_msg void OnBnClickedCancel();



	afx_msg void OnBnClickedButHupai();
	afx_msg void OnBnClickedButTingpai();
	afx_msg void OnCbnSelchangeCoStCh();
	afx_msg void OnCbnSelchangeCoMainjing();
	afx_msg void OnCbnSelchangeCoLessjing();
	afx_msg void OnEnChangeEditData();
	afx_msg void OnBnClickedButMoreTest();
	afx_msg void OnBnClickedCheckDiaopao();



	CSrvLogicEx		m_SrvLogic;//服务器逻辑

	bool GameFinish(BYTE bDeskStation, BYTE bCloseFlag);

    ///发送游戏状态
    void SendGameStationEx(BYTE bDeskStation, bool bWatchUser, void * pData, UINT uSize);
    ///发送数据函数 （发送消息给游戏者）
    void SendGameDataEx(BYTE bDeskStation, BYTE bMainID, BYTE bAssID, BYTE bHandleCode);
    ///发送旁观数据 （发送消息给旁观者）
    void SendWatchDataEx(BYTE bDeskStation, BYTE bMainID, BYTE bAssID, BYTE bHandleCode);
    ///发送数据函数 （发送消息给游戏者）
    void SendGameDataEx(BYTE bDeskStation, void * pData, UINT uSize, BYTE bMainID, BYTE bAssID, BYTE bHandleCode);
    ///发送旁观数据 （发送消息给旁观者）
    void SendWatchDataEx(BYTE bDeskStation, void * pData, UINT uSize, BYTE bMainID, BYTE bAssID, BYTE bHandleCode);


    void KillTimer(int nTimerID);
    void SetTimer(int nID, int nTime);

	//获取游戏状态
	BYTE GetGameStation(){return m_bGameStation;};
	//设置游戏状态
	void SetGameStation(BYTE state){m_bGameStation = state;};
	//获取玩家的同意状态
	void GetUserAgree(BYTE agree);
	//是否在游戏中
	bool IsPlayGame(int bDeskStation);

};
