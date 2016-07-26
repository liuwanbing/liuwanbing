#pragma once

//定义显示类
class COutputDisplay
{
public:
	COutputDisplay(){};
	virtual ~COutputDisplay(){};
	enum ErrorLevel 
	{//定义显示时的消息类型
		Message,//普通信息
		Success, //执行成功
		GeneralWarn,
		FatalWarn,
		GeneralError,
		FatalError,
	};
public:
	//输出
	virtual BOOL PutLine(ErrorLevel eLevel,LPCSTR pszOutput){return FALSE;};
	//得到错误类型的描述
	//pszOut用于输出描速内容，字符串长度必须大于20
	static void GetErrorLevelString(ErrorLevel eLevel,char* pszOut)
	{
		ASSERT(eLevel<=FatalError);
		static char szDesc[6][12]={"信息","执行成功","普通警告","严重警告","普通错误","严重错误"};
		lstrcpy(pszOut,szDesc[(int)eLevel]);
	};
};

/////////////////////////////////////////////////////////////////////////////
// CSysInfoList window
class CSysInfoList : public CListCtrl
{
// Construction
public:
	CSysInfoList();

// Attributes
public:
	CImageList m_cImageListSmall;
// Operations
public:
	//调用此函数显示信息
	void DisplayMsg(int eMsgLevel,LPCTSTR szDisplay);
	void SetMaxItems(int nMaxItem);
protected:
	//初始化列表头
	void InitListCtrlCols(void);
	//初始化图象列表
	void InitImageList(void);
	//最大显示条目
	int m_nShowMaxLine;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSysInfoList)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSysInfoList();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSysInfoList)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
