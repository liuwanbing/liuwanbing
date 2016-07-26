#pragma once
#include "GamePlace.h"
#include "afxwin.h"
#include "SkinMgr.h"
// CSoftKey 对话框

class CSoftKey : public CDialog
{
	DECLARE_DYNAMIC(CSoftKey)

public:
	CSoftKey(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSoftKey();

	CWnd		*pWnd;				//用于保存父窗口的指针，比较奇怪，直接不能使用，但现在这样做成绕开一些报错
	
	CButton		*m_char[21];		//数字和符号的按钮指针，方便调用
	CButton		*m_letter[26];		//字母的按钮指针
	
	int			m_charIndex[21];	//索引
	int			m_letterIndex[26];	//索引

	CString		m_sChar[21][2];		//具体的数字和符号
	CString     m_sLeffer[26][2];	//具体的字母

	bool		m_bShift;				//是否按下了SHIFT
	bool		m_bCaplock;				//是否按下了CAPS CLOCK

	COLORREF						m_bkcolor;//背景色

	HBRUSH m_bkBrush;
// 对话框数据
	enum { IDD = IDD_SOFTKEY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:
	CNormalBitmapButtonEx m_shift;				//shift按钮
public:	
	CNormalBitmapButtonEx m_caplock;				//caps lock 按钮
public:
	afx_msg void OnBnClickedShift();
public:
	afx_msg void OnBnClickedCaplock();
public:
	afx_msg void OnBnClickedButton1();
public:
	afx_msg void OnBnClickedButton2();
public:
	afx_msg void OnBnClickedButton3();
public:
	afx_msg void OnBnClickedButton4();
public:
	afx_msg void OnBnClickedButton5();
public:
	afx_msg void OnBnClickedButton6();
public:
	afx_msg void OnBnClickedButton7();
public:
	afx_msg void OnBnClickedButton8();
public:
	afx_msg void OnBnClickedButton9();
public:
	afx_msg void OnBnClickedButton10();
public:
	afx_msg void OnBnClickedButton11();
public:
	afx_msg void OnBnClickedButton12();
public:
	afx_msg void OnBnClickedButton13();
public:
	afx_msg void OnBnClickedButton14();
public:
	afx_msg void OnBnClickedButton15();
public:
	afx_msg void OnBnClickedButton16();
public:
	afx_msg void OnBnClickedButton20();
public:
	afx_msg void OnBnClickedButton18();
public:
	afx_msg void OnBnClickedButton19();
public:
	afx_msg void OnBnClickedButton17();
public:
	afx_msg void OnBnClickedButton21();
public:
	void inputChar(int index);
public:
	void inputLetter(int index);
public:
	afx_msg void OnBnClickedButton23();
public:
	afx_msg void OnBnClickedButton24();
public:
	afx_msg void OnBnClickedButton25();
public:
	afx_msg void OnBnClickedButton26();
public:
	afx_msg void OnBnClickedButton27();
public:
	afx_msg void OnBnClickedButton28();
public:
	afx_msg void OnBnClickedButton29();
public:
	afx_msg void OnBnClickedButton30();
public:
	afx_msg void OnBnClickedButton31();
public:
	afx_msg void OnBnClickedButton32();
public:
	afx_msg void OnBnClickedButton33();
public:
	afx_msg void OnBnClickedButton34();
public:
	afx_msg void OnBnClickedButton35();
public:
	afx_msg void OnBnClickedButton36();
public:
	afx_msg void OnBnClickedButton37();
public:
	afx_msg void OnBnClickedButton38();
public:
	afx_msg void OnBnClickedButton39();
public:
	afx_msg void OnBnClickedButton40();
public:
	afx_msg void OnBnClickedButton41();
public:
	afx_msg void OnBnClickedButton42();
public:
	afx_msg void OnBnClickedButton43();
public:
	afx_msg void OnBnClickedButton44();
public:
	afx_msg void OnBnClickedButton45();
public:
	afx_msg void OnBnClickedButton46();
public:
	afx_msg void OnBnClickedButton47();
public:
	afx_msg void OnBnClickedButton48();
public:
	void LoadSkin(void);
public:
	CNormalBitmapButtonEx m_bnC1;
	CNormalBitmapButtonEx m_bnC2;
	CNormalBitmapButtonEx m_bnC3;
	CNormalBitmapButtonEx m_bnC4;
	CNormalBitmapButtonEx m_bnC5;
	CNormalBitmapButtonEx m_bnC6;
	CNormalBitmapButtonEx m_bnC7;
	CNormalBitmapButtonEx m_bnC8;
	CNormalBitmapButtonEx m_bnC9;
	CNormalBitmapButtonEx m_bnC10;
	CNormalBitmapButtonEx m_bnC11;
	CNormalBitmapButtonEx m_bnC12;
	CNormalBitmapButtonEx m_bnC13;
	CNormalBitmapButtonEx m_bnC14;
	CNormalBitmapButtonEx m_bnC15;
	CNormalBitmapButtonEx m_bnC16;
	CNormalBitmapButtonEx m_bnC17;
	CNormalBitmapButtonEx m_bnC18;
	CNormalBitmapButtonEx m_bnC19;
	CNormalBitmapButtonEx m_bnC20;
	CNormalBitmapButtonEx m_bnC21;

	CNormalBitmapButtonEx m_bnL1;
	CNormalBitmapButtonEx m_bnL2;
	CNormalBitmapButtonEx m_bnL3;
	CNormalBitmapButtonEx m_bnL4;
	CNormalBitmapButtonEx m_bnL5;
	CNormalBitmapButtonEx m_bnL6;
	CNormalBitmapButtonEx m_bnL7;
	CNormalBitmapButtonEx m_bnL8;
	CNormalBitmapButtonEx m_bnL9;
	CNormalBitmapButtonEx m_bnL10;
	CNormalBitmapButtonEx m_bnL11;
	CNormalBitmapButtonEx m_bnL12;
	CNormalBitmapButtonEx m_bnL13;
	CNormalBitmapButtonEx m_bnL14;
	CNormalBitmapButtonEx m_bnL15;
	CNormalBitmapButtonEx m_bnL16;
	CNormalBitmapButtonEx m_bnL17;
	CNormalBitmapButtonEx m_bnL18;
	CNormalBitmapButtonEx m_bnL19;
	CNormalBitmapButtonEx m_bnL20;
	CNormalBitmapButtonEx m_bnL21;
	CNormalBitmapButtonEx m_bnL22;
	CNormalBitmapButtonEx m_bnL23;
	CNormalBitmapButtonEx m_bnL24;
	CNormalBitmapButtonEx m_bnL25;
	CNormalBitmapButtonEx m_bnL26;

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

private:
	CSkinMgr m_skinmgr;
};
