// AddRoomDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "AddRoomDialog.h"


// CAddRoomDialog dialog

IMPLEMENT_DYNAMIC(CAddRoomDialog, CDialog)

CAddRoomDialog::CAddRoomDialog(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DLL_SET, pParent)
	, m_roomID(0)
	, m_szGameRoomName(_T(""))
	, m_uComType(0)
	, m_uListenPort(0)
	, m_uKindID(0)
	, m_uDeskCount(0)
	, m_uMaxPeople(0)
	, m_uBasePoint(0)
	, m_uLessPoint(0)
	, m_dwRoomRule(0)
{

	EnableAutomation();

}

CAddRoomDialog::~CAddRoomDialog()
{
}

void CAddRoomDialog::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CDialog::OnFinalRelease();
}

void CAddRoomDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ROOM_ID, m_roomID);
	DDX_Text(pDX, IDC_ROOM_NAME, m_szGameRoomName);
	DDX_Control(pDX, IDC_USER_SQL_IP, m_LogonSQLIP);
	DDX_Control(pDX, IDC_GAME_SQL_IP, m_SQLIP);
	DDX_Control(pDX, IDC_NATIVE_SQL_IP, m_NativeSQLIP);
	DDX_CBIndex(pDX, IDC_SET_COM, m_uComType);
	DDX_Text(pDX, IDC_SOCKET_PORT, m_uListenPort);
	DDX_CBIndex(pDX, IDC_GAME_TYPE, m_uKindID);
	DDX_Text(pDX, IDC_DESK_COUNT, m_uDeskCount);
	DDV_MinMaxInt(pDX, m_uDeskCount, 0, 200);
	DDX_Text(pDX, IDC_MAX_CONNECT, m_uMaxPeople);
	DDV_MinMaxInt(pDX, m_uMaxPeople, 0, 800);
	DDX_Text(pDX, IDC_BASE_POINT, m_uBasePoint);
	DDX_Text(pDX, IDC_LESS_POINT, m_uLessPoint);
}


BEGIN_MESSAGE_MAP(CAddRoomDialog, CDialog)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CAddRoomDialog, CDialog)
END_DISPATCH_MAP()

// Note: we add support for IID_IAddRoomDialog to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .IDL file.

// {12E8DDA9-69A5-4453-97B4-E9A0D8DF598C}
static const IID IID_IAddRoomDialog =
{ 0x12E8DDA9, 0x69A5, 0x4453, { 0x97, 0xB4, 0xE9, 0xA0, 0xD8, 0xDF, 0x59, 0x8C } };

BEGIN_INTERFACE_MAP(CAddRoomDialog, CDialog)
	INTERFACE_PART(CAddRoomDialog, IID_IAddRoomDialog, Dispatch)
END_INTERFACE_MAP()


// CAddRoomDialog message handlers
