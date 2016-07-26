// ward.cpp : 实现文件
//

#include "stdafx.h"
#include "TCP.h"
#include "ward.h"


// Cward 对话框

IMPLEMENT_DYNAMIC(Cward, CDialog)

Cward::Cward(CWnd* pParent /*=NULL*/)
	: CDialog(Cward::IDD, pParent)
{

}

Cward::~Cward()
{
}

void Cward::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Cward, CDialog)
END_MESSAGE_MAP()


// Cward 消息处理程序
