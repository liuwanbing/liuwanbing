#include "StdAfx.h"
#include "AFCMessageBox.h"

CAFCMessageBox::CAFCMessageBox(void)
{
}

CAFCMessageBox::~CAFCMessageBox(void)
{
}

//
void CAFCMessageBox::SetMessage(CString message)
{
	CTransparentStatic *pStatic = dynamic_cast<CTransparentStatic*>(GetControlById(IDD_DLG_TIP_LABLE));
	pStatic->SetMyText(message.GetBuffer());
	//message.ReleaseBuffer();
}
bool CAFCMessageBox::OnPostMessage(UINT pId,BBaseParameter* pParam)
{
	return __super::OnPostMessage(pId,pParam);

}

//UINT AFCMessageBox(CString strInfo,CString strTitel,CWnd* pParentWnd,UINT style)
//{
//	//AfxSetResourceHandle(GetModuleHandle(FACE_DLL_NAME));
//	//ConfirmDlg confirmDlg(strInfo,strTitel,style);
//	CAFCMessageBox MsgDlg;
//
//	 MsgDlg.SetParent(pParentWnd);
//	
//	if (style == DLG_MSG_MB_OK)
//	{
//		MsgDlg.LoadConfig(IDD_MSG_DLG_RES_CONFIRM);		
//	}
//	else
//	{
//		MsgDlg.LoadConfig(IDD_MSG_DLG_RES_SELECT);
//	}
//	MsgDlg.ShowAllControl(SW_SHOW);
//
//	MsgDlg.SetMessage(strInfo);	
//	return MsgDlg.DoModal();
//}
