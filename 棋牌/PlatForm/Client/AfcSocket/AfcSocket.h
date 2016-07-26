// AfcSocket.h : main header file for the AfcSocket DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

//#include "resource.h"		// main symbols


// CAfcSocketApp
// See AfcSocket.cpp for the implementation of this class
//

class CAfcSocketApp : public CWinApp
{
public:
	CAfcSocketApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
