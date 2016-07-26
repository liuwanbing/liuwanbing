
// LKFishToolDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LKFishTool.h"
#include "LKFishToolDlg.h"
#include "afxdialogex.h"
#include "ResUtility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLKFishToolDlg 对话框




CLKFishToolDlg::CLKFishToolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLKFishToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLKFishToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CLKFishToolDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CLKFishToolDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CLKFishToolDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CLKFishToolDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CLKFishToolDlg::OnBnClickedButton4)
END_MESSAGE_MAP()


// CLKFishToolDlg 消息处理程序

BOOL CLKFishToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CLKFishToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CLKFishToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CLKFishToolDlg::OnBnClickedButton1()
{
	this->GetDlgItemTextW(IDC_MFCEDITBROWSE2,m_strSource);
	// TODO: 在此添加控件通知处理程序代码
	{
		CString dwjlog;
		dwjlog.Format(L"dwjlog::m_strSource=%s",m_strSource);
		OutputDebugString(dwjlog);
	}
	if (m_strSource == "")
	{
		MessageBox(L"加密文件为空",L"提示",0);
		return;
	}
	if (EncryptFishFile(m_strSource, true))
	{
		MessageBox(L"加密文件完成",L"提示",0);
	}	
}
void CLKFishToolDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	this->GetDlgItemTextW(IDC_MFCEDITBROWSE2,m_strSource);
	// TODO: 在此添加控件通知处理程序代码
	{
		CString dwjlog;
		dwjlog.Format(L"dwjlog::m_strSource=%s",m_strSource);
		OutputDebugString(dwjlog);
	}
	if (m_strSource == "")
	{
		MessageBox(L"加密文件为空",L"提示",0);
		return;
	}
	if (EncryptFishFile(m_strSource, false))
	{
		MessageBox(L"加密文件完成",L"提示",0);
	}
	
}




void	CLKFishToolDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	this->GetDlgItemTextW(IDC_MFCEDITBROWSE2,m_strSource);
	// TODO: 在此添加控件通知处理程序代码
	{
		CString dwjlog;
		dwjlog.Format(L"dwjlog::m_strSource=%s",m_strSource);
		OutputDebugString(dwjlog);
	}
	if (m_strSource == "")
	{
		MessageBox(L"解密文件为空",L"提示",0);
		return;
	}
	if (DecryptFishFile(m_strSource, true))
	{
		MessageBox(L"解密文件完成",L"提示",0);
	}
}

void CLKFishToolDlg::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	this->GetDlgItemTextW(IDC_MFCEDITBROWSE2,m_strSource);
	// TODO: 在此添加控件通知处理程序代码
	{
		CString dwjlog;
		dwjlog.Format(L"dwjlog::m_strSource=%s",m_strSource);
		OutputDebugString(dwjlog);
	}
	if (m_strSource == "")
	{
		MessageBox(L"解密文件为空",L"提示",0);
		return;
	}
	if (DecryptFishFile(m_strSource, false))
	{
		MessageBox(L"解密文件完成",L"提示",0);
	}	
}



bool	CLKFishToolDlg::EncryptFishFile(const CString& strFile, BOOL bDeleteSourceFile)
{
	FILE* file = NULL;
	_tfopen_s(&file, strFile, _T("rb"));
	if (file == NULL) {
		MessageBox(_T("文件打开失败"));
		return false;
	}

	int pos = strFile.ReverseFind(_T('.'));
	if (pos < 0)
	{
		MessageBox(_T("文件名错误"));
		return false;
	}
	CString strDstFile = strFile.Left(pos);
	strDstFile += _T(".fish");

	fseek(file, 0, SEEK_END);
	long file_len = ftell(file);

	fseek(file, 0, SEEK_SET);
	BYTE* file_data = new BYTE[file_len];
	fread(file_data, file_len, 1, file);
	for (long i = 0; i < file_len; ++i)
	{
		file_data[i] = g_EncryptByteMap[file_data[i]];
	}

	// 加密数据
	DWORD dwXorKey = g_dwKey;
	WORD* pwSeed = (WORD*)file_data;
	DWORD* pdwXor = (DWORD*)file_data;
	WORD wEncrypCount = (WORD)(file_len / sizeof(DWORD));
	for (WORD i = 0; i < wEncrypCount; i++)
	{
		*pdwXor++ ^= dwXorKey;
		dwXorKey = SeedRandMap(*pwSeed++);
		dwXorKey |= ((DWORD)SeedRandMap(*pwSeed++)) << 16;
		dwXorKey ^= g_dwKey;
	}

	FILE* wfile = NULL;
	_tfopen_s(&wfile, strDstFile, _T("wb"));
	fwrite(file_data, file_len, 1, wfile);

	delete[] file_data;
	fclose(file);
	fclose(wfile);

	if (bDeleteSourceFile)
	{
		DeleteFile(strFile);
	}

	return true;
}




bool	CLKFishToolDlg::DecryptFishFile(const CString& strFile, BOOL bDeleteSourceFile)
{
	FILE* file = NULL;
	_tfopen_s(&file, strFile, _T("rb"));
	if (file == NULL) 
	{
		MessageBox(_T("文件打开失败"));
		return false;
	}

	int pos = strFile.ReverseFind(_T('.'));
	if (pos < 0)
	{
		MessageBox(_T("文件名错误"));
		return false;
	}
	CString strDstFile = strFile.Left(pos);

	fseek(file, 0, SEEK_END);
	long file_len = ftell(file);

	fseek(file, 0, SEEK_SET);
	BYTE* file_data = new BYTE[file_len];
	fread(file_data, file_len, 1, file);

	// 解密数据
	DWORD dwXorKey = g_dwKey;
	DWORD dwDecryptKey = g_dwKey;
	WORD* pwSeed = (WORD*)file_data;
	DWORD* pdwXor = (DWORD*)file_data;
	WORD wDecrypCount = (WORD)(file_len / sizeof(DWORD));
	for (WORD i = 0; i < wDecrypCount; i++)
	{
		dwXorKey = SeedRandMap(*pwSeed++);
		dwXorKey |= ((DWORD)SeedRandMap(*pwSeed++)) << 16;
		dwXorKey ^= g_dwKey;
		*pdwXor++ ^= dwDecryptKey;
		dwDecryptKey = dwXorKey;
	}

	for (long i = 0; i < file_len; ++i)
	{
		file_data[i] = g_DecryptByteMap[file_data[i]];
	}

	strDstFile += GetFileExtension(file_data);

	FILE* wfile = NULL;
	_tfopen_s(&wfile, strDstFile, _T("wb"));
	fwrite(file_data, file_len, 1, wfile);

	delete[] file_data;
	fclose(file);
	fclose(wfile);

	if (bDeleteSourceFile)
	{
		DeleteFile(strFile);
	}

	return true;
}
