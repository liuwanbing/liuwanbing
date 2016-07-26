#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif



#import "..\..\lib\Server\IsLicense50.dll"
using namespace IsLicense50;

class LicenseValidator
{
public:
	LicenseValidator(void);
	virtual ~LicenseValidator(void);

	bool ValidateLicense(CString activationKey, CString computerKey, CString computerID, bool &needsActivation, CString &errorMsg);

private:
	IIsLicenseMgrPtr license;

	bool IsTrue (int nVal1, int nVal2);
};
