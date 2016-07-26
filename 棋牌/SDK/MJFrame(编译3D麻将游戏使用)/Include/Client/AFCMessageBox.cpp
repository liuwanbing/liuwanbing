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

void CAFCMessageBox::SetMessage_New(CString message)
{
    CString strInfo = "";
    CString strInfo_1 = "";
    CString strInfo_2 = "";

    CTransparentStatic *pStatic = NULL;

    int iIndex = 0;
    int iFind = 0;
    int iID = 315;
    while((iFind = message.Find("\n", iIndex)) != -1)
    {
        strInfo = message.Mid(iIndex, iFind+1-iIndex);
        iIndex = iFind+1;

        pStatic = dynamic_cast<CTransparentStatic*>(GetControlById(iID++));
        if(NULL != pStatic) pStatic->SetMyText(strInfo.GetBuffer());
    }
}