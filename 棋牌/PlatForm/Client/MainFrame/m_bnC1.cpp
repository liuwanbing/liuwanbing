// m_bnC1.cpp : 实现文件
//

#include "stdafx.h"
#include "m_bnC1.h"


// m_bnC1 对话框

IMPLEMENT_DYNAMIC(m_bnC1, CDialog)

m_bnC1::m_bnC1(CWnd* pParent /*=NULL*/)
	: CDialog(m_bnC1::IDD, pParent)
{

}

m_bnC1::~m_bnC1()
{
}

void m_bnC1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(m_bnC1, CDialog)
END_MESSAGE_MAP()


// m_bnC1 消息处理程序
