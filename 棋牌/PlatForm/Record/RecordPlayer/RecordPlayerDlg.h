// RecordPlayerDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"

#include "PlayerLogic.h"

#define  TIMER_ID_PLAY      1


#define  PLAY_MODE_STOP             0
#define  PLAY_MODE_SLOW_PLAY        1
#define  PLAY_MODE_PLAY             2
#define  PLAY_MODE_QUICK_PLAY       3
#define  PLAY_MODE_PALY_ONE_STEP    4
#define  PLAY_MODE_BACK_ONE_STEP    5
 
#define  PLAY_MODE_QUICK_POS        6  //快速定位  



// CRecordPlayerDlg 对话框
class CRecordPlayerDlg : public CDialog
{
// 构造
public:
	CRecordPlayerDlg(CWnd* pParent = NULL);	// 标准构造函数

	CRecordPlayerDlg(TCHAR* pFileName,CWnd* pParent = NULL);

// 对话框数据
	enum { IDD = IDD_RECORDPLAYER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


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
	//拖动按钮
	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);
    //步退
	afx_msg void OnBnClickedBtnBackOne();
    //步进
	afx_msg void OnBnClickedBtnForwardOne();
    //停止
	afx_msg void OnBnClickedBtnStop();
    //慢放
	afx_msg void OnBnClickedBtnPlaySlow();
    //播放
	afx_msg void OnBnClickedBtnPlay1();
    //快速播放
	afx_msg void OnBnClickedBtnPlayQuick();
    //打开文件
	afx_msg void OnBnClickedBtnOpen();
	
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	CString GetAppPath(BOOL bValue=false);
	

protected:
	void KillAllTimer();
	void GetMsgTimeSlot();
	void SetPlayTime(int iTimeSlot);
	void PlayInTimer();
public:
	//还不知道这个应该怎么用呢，先实现几个好做的吧
	CSliderCtrl m_SliderCtrl;
	//wushuqun 2009.7.13
	//进度条不能后退
	//记录当前进度条位置
	int         m_iCurPos;
	
private:
	CPlayerLogic m_PlayLogic;

	int m_iPlayMode;  //播放模式
	bool m_bHasSetTimer;
	TCHAR m_szFileName[1000];


	afx_msg void OnBnClickedButtonCancle();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
