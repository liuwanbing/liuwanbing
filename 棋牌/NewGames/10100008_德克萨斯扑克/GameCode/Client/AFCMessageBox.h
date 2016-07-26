#pragma once
#include "GameLanchSDK/bzwUiInclude/bzwUI.h"
#include "defines.h"
#define DLG_MSG_MB_OK     1
#define DLG_MSG_MB_YESNO  2

class CAFCMessageBox : public BModalContainer
{
public:
	CAFCMessageBox(void);

	~CAFCMessageBox(void);

	virtual bool OnPostMessage(UINT pId,BBaseParameter* pParam);

	void SetMessage(CString message);

};

//extern UINT AFCMessageBox(CString strInfo,CString strTitel,CWnd* pParentWnd,UINT style=DLG_MSG_MB_OK);

