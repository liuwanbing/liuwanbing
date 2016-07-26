// SetupDialog.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "SetupDialog.h"


// CSetupDialog dialog

IMPLEMENT_DYNAMIC(CSetupDialog, CDialog)

CSetupDialog::CSetupDialog(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_SETGAME_DIALOG, pParent)
	, serInfoID(112)
	, nameID(112)
	, deskPeople(4)
	, supportType(2)
	, sver(2)
	, gameName(_T("麻将"))
	, tableDbName(_T("dll.gametable"))
	, dllFileName(_T("Srv.dll"))
	, writer(_T("szbzw"))
	, dllNote(_T(""))
{
	CTime t=CTime::GetCurrentTime();
	dllNote = t.Format("%Y年%m月%d日%H点%M");
}

CSetupDialog::~CSetupDialog()
{
}

void CSetupDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SIID, serInfoID);
	DDX_Text(pDX, IDC_EDIT_NID, nameID);
	DDX_Text(pDX, IDC_EDIT_DP, deskPeople);
	DDX_Text(pDX, IDC_EDIT_ST, supportType);
	DDV_MinMaxInt(pDX, supportType, 1, 4);
	DDX_Text(pDX, IDC_EDIT_SV, sver);
	DDV_MinMaxInt(pDX, sver, 1, 100);
	DDX_Text(pDX, IDC_EDIT_GN, gameName);
	DDX_Text(pDX, IDC_EDIT_GT, tableDbName);
	DDX_Text(pDX, IDC_EDIT_DLLNAME, dllFileName);
	DDX_Text(pDX, IDC_EDIT_W, writer);
	DDX_Text(pDX, IDC_EDIT_DLLNOTE, dllNote);
	DDX_Control(pDX, IDC_EDIT_SIID, editSiid);
}


BEGIN_MESSAGE_MAP(CSetupDialog, CDialog)
END_MESSAGE_MAP()


// CSetupDialog message handlers
