//http://www.codeproject.com/shell/iehelper.asp
/*********************************************************************
	Copyright (C) 2002 Mustafa Demirhan
	
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.

	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.

	3. This notice may not be removed or altered from any source distribution.

	emindemirhan@yahoo.com
**********************************************************************/

// IEHelper.h: interface for the IEHelper class.
//
//////////////////////////////////////////////////////////////////////

#ifndef IEHELPER_HEAD_H
#define IEHELPER_HEAD_H

#if !defined(AFX_IEHELPER_H__022E25DB_3205_4310_991E_94EB80D064DB__INCLUDED_)
#define AFX_IEHELPER_H__022E25DB_3205_4310_991E_94EB80D064DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <ExDisp.h>
#include <ExDispID.h>
#include <shlwapi.h>
#import <shdocvw.dll> 

#define __IHTMLControlElement_INTERFACE_DEFINED__
#include <msHtml.h>
#undef __IHTMLControlElement_INTERFACE_DEFINED__
//用于查找页面中的元素
class CMyInternetExplorer
{
public:

	CMyInternetExplorer (LPCTSTR sBrowserID);
	~CMyInternetExplorer ();
	
	CString GetBrowserID () const;
	
	bool IsValid ();
	
	bool CreateNewInstance ();

	bool FindUsingTitle (const CString & sTitle);

	CString GetFullName() const;
	CString GetType() const;
	CString GetLocationName() const;
	CString GetLocationURL() const;
	void	Navigate (LPCTSTR lpszURL, DWORD dwFlags = 0,
					  LPCTSTR lpszTargetFrameName = NULL,
					  LPCTSTR lpszHeaders = NULL, 
					  LPVOID lpvPostData = NULL,
					  DWORD dwPostDataLen = 0);
	
	void Refresh ();
	void Stop ();
	void GoBack ();
	void GoForward ();
	void Quit ();

	bool WaitTillLoaded (int nTimeout);

	bool GetBusy () const;
	READYSTATE GetReadyState () const;
	LPDISPATCH GetHtmlDocument() const;

	bool FindAnchor		(bool bClick, bool bFocus,
						 bool bName, bool bOuterText, bool bTooltip, bool bURL,
						 LPCTSTR sName, LPCTSTR sOuterText, LPCTSTR sTooltip, LPCTSTR sURL);
	
static	bool FindInput		(IDispatch* pHtmlDocDispatch,bool bClick, bool bSelect, bool bChangeValue, bool bSetCheck,
						 bool bType, bool bName, bool bValue, 
						 LPCTSTR sTypeToLook, LPCTSTR sNameToLook, LPCTSTR sValueToLook,
						 bool bNewCheckValue, LPCTSTR sNewValue);
static	bool GetInput		(IDispatch* pHtmlDocDispatch,bool bClick, bool bSelect, bool bChangeValue, bool bSetCheck,
						 bool bType, bool bName, bool bValue, 
						 LPCTSTR sTypeToLook, LPCTSTR sNameToLook, LPCTSTR sValueToLook,
						 bool bNewCheckValue, LPCTSTR sNewValue,LPTSTR szValue);
	
	bool FindOption		(bool bClick, 
						 bool bValue, bool bText,
						 LPCTSTR sValue, LPCTSTR sText);

	bool FindElement	(bool bExactMatch, bool bWildcardMatch, bool bCaseSensitive, bool bClick,
						 bool bTagName, bool bOuterHTML, bool bInnerHTML, bool bTooltip,
						 LPCTSTR sTagNameOrg, LPCTSTR sOuterHTMLOrg, LPCTSTR sInnerHTMLOrg, LPCTSTR sTooltipOrg,
						 CString & sRetTagName, CString & sOuterHTML, CString & sRetInnerHTML, CString & sTooltip);
	
protected:
	IWebBrowser2 * m_pWebBrowser;
	CString m_sBrowserID;
};


#endif // !defined(AFX_IEHELPER_H__022E25DB_3205_4310_991E_94EB80D064DB__INCLUDED_)

#endif