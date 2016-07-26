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

// IEHelper.cpp: implementation of the IEHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IEHelper.h"
#include "StringHelper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMyInternetExplorer::CMyInternetExplorer (LPCTSTR sBrowserID) : 
m_pWebBrowser (NULL)
{
	if (FAILED (CoInitialize (NULL)))
	{
		MessageBox (NULL, _T ("Failed to initialize the COM Library to create the Internet Explorer object.\r\n\r\nPlease contact to support@macroangel.com for more information."), _T ("Macro Angel"), MB_ICONERROR);
		ASSERT (FALSE);
	}

	m_sBrowserID = sBrowserID;
	m_sBrowserID.MakeLower ();
}

CMyInternetExplorer::~CMyInternetExplorer ()
{
	if (m_pWebBrowser != NULL)
	{
		m_pWebBrowser->Release ();
		m_pWebBrowser = NULL;
	}

	CoUninitialize ();
}

CString CMyInternetExplorer::GetBrowserID () const
{
	return m_sBrowserID;
}

bool CMyInternetExplorer::CreateNewInstance ()
{
	if (m_pWebBrowser != NULL)
	{
		m_pWebBrowser->Release ();
		m_pWebBrowser = NULL;
	}
	
	HRESULT hr;
	IWebBrowser2* pWebBrowser = NULL;
	hr = CoCreateInstance (CLSID_InternetExplorer, NULL, CLSCTX_SERVER, IID_IWebBrowser2, (LPVOID*)&pWebBrowser);
	
	if (SUCCEEDED (hr) && (pWebBrowser != NULL))
	{
		m_pWebBrowser = pWebBrowser;
		m_pWebBrowser->put_Visible (VARIANT_TRUE);
		return true;
	}
	else
	{
		if (pWebBrowser)
			pWebBrowser->Release ();
		return false;
	}
	return false;
}

bool CMyInternetExplorer::IsValid ()
{
	if (m_pWebBrowser == NULL)
		return false;
	
	// TODO: I dont know how to check whether it is still valid or not!
	return true;
}

bool CMyInternetExplorer::FindUsingTitle (const CString & sTitleToSearch)
{
	if (m_pWebBrowser != NULL)
	{
		m_pWebBrowser->Release ();
		m_pWebBrowser = NULL;
	}

	HRESULT hr;
	SHDocVw::IShellWindowsPtr spSHWinds; 
	hr = spSHWinds.CreateInstance (__uuidof(SHDocVw::ShellWindows)); 
	
	if (FAILED (hr))
		return false;

	ASSERT (spSHWinds != NULL);
	
	long nCount = spSHWinds->GetCount ();

	IDispatchPtr spDisp;
	
	for (long i = 0; i < nCount; i++)
	{
		_variant_t va (i, VT_I4);
		spDisp = spSHWinds->Item (va);
		
		IWebBrowser2 * pWebBrowser = NULL;
		hr = spDisp.QueryInterface (IID_IWebBrowser2, & pWebBrowser);
		
		if (pWebBrowser != NULL)
		{
			HRESULT hr;
			IDispatch* pHtmlDocDispatch = NULL;
			IHTMLDocument2 * pHtmlDoc = NULL;
			
			// Retrieve the document object.
			hr = pWebBrowser->get_Document (&pHtmlDocDispatch);
			
			if (SUCCEEDED (hr) && (pHtmlDocDispatch != NULL))
			{
				// Query for IPersistStreamInit.
				hr = pHtmlDocDispatch->QueryInterface (IID_IHTMLDocument2,  (void**)&pHtmlDoc);
				if (SUCCEEDED (hr) && (pHtmlDoc != NULL))
				{
					CString sTitle;

					HWND hWnd = NULL;
					pWebBrowser->get_HWND ((long*)(&hWnd));
					if (::IsWindow (hWnd))
					{
						int nLen = ::GetWindowTextLength (hWnd);
						::GetWindowText (hWnd, sTitle.GetBufferSetLength (nLen), nLen + 1);
						sTitle.ReleaseBuffer ();
					}
					
					// If I cannot get the window title (should never happen though)
					// So, lets just use the title of the document
					if (sTitle.IsEmpty ())
					{
						BSTR bstrTitle;
						hr = pHtmlDoc->get_title (&bstrTitle);
						if (!FAILED (hr))
						{
							sTitle = bstrTitle;
							SysFreeString (bstrTitle); 
						}
					}
					
					if (StringHelper::WildcardCompareNoCase (sTitleToSearch, sTitle))
					{
						m_pWebBrowser = pWebBrowser;
						pHtmlDoc->Release ();
						pHtmlDocDispatch->Release ();
						// Exit the method safely!
						return true;
					}
					pHtmlDoc->Release();
				}
				pHtmlDocDispatch->Release ();
			}
			pWebBrowser->Release ();
		}
	}
	
	return false;
}

CString CMyInternetExplorer::GetFullName() const
{
    ASSERT (m_pWebBrowser != NULL);
	if (m_pWebBrowser == NULL)
		return CString ();
	
    BSTR bstr;
    m_pWebBrowser->get_FullName (&bstr);
    CString retVal (bstr);
    
	SysFreeString (bstr);
	return retVal;
}

CString CMyInternetExplorer::GetType() const
{
    ASSERT (m_pWebBrowser != NULL);
	if (m_pWebBrowser == NULL)
		return CString ();
	
    BSTR bstr;
    m_pWebBrowser->get_Type (&bstr);
    CString retVal(bstr);

    SysFreeString(bstr);
    return retVal;
}

CString CMyInternetExplorer::GetLocationName() const
{
    ASSERT(m_pWebBrowser != NULL);
	if (m_pWebBrowser == NULL)
		return CString ();
	
    BSTR bstr;
    m_pWebBrowser->get_LocationName (&bstr);
    CString retVal (bstr);

    SysFreeString (bstr); // Added this line to prevent leak.
    return retVal;
}
 
CString CMyInternetExplorer::GetLocationURL() const
{
	ASSERT (m_pWebBrowser != NULL);
	if (m_pWebBrowser == NULL)
		return CString ();
	
    BSTR bstr;
    m_pWebBrowser->get_LocationURL (&bstr);
    CString retVal (bstr);

    SysFreeString (bstr); // Added this line to prevent leak.
    return retVal;
}

void CMyInternetExplorer::Navigate(LPCTSTR lpszURL, DWORD dwFlags /* = 0 */,
	LPCTSTR lpszTargetFrameName /* = NULL */ ,
	LPCTSTR lpszHeaders /* = NULL */, LPVOID lpvPostData /* = NULL */,
	DWORD dwPostDataLen /* = 0 */)
{
    CString strURL (lpszURL);
    BSTR bstrURL = strURL.AllocSysString ();
	
    COleSafeArray vPostData;
    if (lpvPostData != NULL)
    {
		if (dwPostDataLen == 0)
			dwPostDataLen = lstrlen ((LPCTSTR) lpvPostData);
		
		vPostData.CreateOneDim (VT_UI1, dwPostDataLen, lpvPostData);
    }
	
    m_pWebBrowser->Navigate (bstrURL, COleVariant ((long) dwFlags, VT_I4), COleVariant (lpszTargetFrameName, VT_BSTR), 
							 vPostData, COleVariant (lpszHeaders, VT_BSTR));
	
    SysFreeString (bstrURL);
}

bool CMyInternetExplorer::WaitTillLoaded (int nTimeout)
{
	READYSTATE result;
	DWORD nFirstTick = GetTickCount ();

	do
	{
		m_pWebBrowser->get_ReadyState (&result);
		
		if (result != READYSTATE_COMPLETE)
			Sleep (250);
		
		if (nTimeout > 0)
		{
			if ((GetTickCount () - nFirstTick) > nTimeout)
				break;
		}
	} while (result != READYSTATE_COMPLETE);

	if (result == READYSTATE_COMPLETE)
		return true;
	else
		return false;
}

void CMyInternetExplorer::Refresh ()
{
	ASSERT (m_pWebBrowser != NULL);
	if (m_pWebBrowser == NULL)
		return;

	m_pWebBrowser->Refresh ();
}

void CMyInternetExplorer::Stop ()
{
	ASSERT (m_pWebBrowser != NULL);
	if (m_pWebBrowser == NULL)
		return;
	
	m_pWebBrowser->Stop ();
}

void CMyInternetExplorer::GoBack ()
{
	ASSERT (m_pWebBrowser != NULL);
	if (m_pWebBrowser == NULL)
		return;
	
	m_pWebBrowser->GoBack ();
}

void CMyInternetExplorer::GoForward ()
{
	ASSERT (m_pWebBrowser != NULL);
	if (m_pWebBrowser == NULL)
		return;
	
	m_pWebBrowser->GoForward ();
}

void CMyInternetExplorer::Quit ()
{
	ASSERT (m_pWebBrowser != NULL);
	if (m_pWebBrowser == NULL)
		return;
	
	m_pWebBrowser->Quit ();
	m_pWebBrowser->Release ();
	m_pWebBrowser = NULL;
}

bool CMyInternetExplorer::GetBusy() const
{
	ASSERT (m_pWebBrowser != NULL);
	if (m_pWebBrowser == NULL)
		return false;
	
	VARIANT_BOOL result;
	m_pWebBrowser->get_Busy (&result);
	return result ? true : false;
}

READYSTATE CMyInternetExplorer::GetReadyState() const
{
	ASSERT (m_pWebBrowser != NULL);
	if (m_pWebBrowser == NULL)
		return READYSTATE_UNINITIALIZED;
	
	READYSTATE result;
	m_pWebBrowser->get_ReadyState (&result);
	return result;
}

LPDISPATCH CMyInternetExplorer::GetHtmlDocument() const
{
	ASSERT (m_pWebBrowser != NULL);
	if (m_pWebBrowser == NULL)
		return NULL;
	
	LPDISPATCH result;
	m_pWebBrowser->get_Document (&result);
	return result;
}

bool CMyInternetExplorer::FindAnchor (bool bClick, bool bFocus,
									  bool bName, bool bOuterText, bool bTooltip, bool bURL,
									  LPCTSTR sName, LPCTSTR sOuterText, LPCTSTR sTooltip, LPCTSTR sURL)
{
	ASSERT (m_pWebBrowser != NULL);
	if (m_pWebBrowser == NULL)
		return false;
	
	HRESULT hr;
	IDispatch* pHtmlDocDispatch = NULL;
	IHTMLDocument2 * pHtmlDoc = NULL;
	bool bSearch = true;

	// Retrieve the document object.
	hr = m_pWebBrowser->get_Document (&pHtmlDocDispatch);
	if (SUCCEEDED (hr) && (pHtmlDocDispatch != NULL))
	{
		hr = pHtmlDocDispatch->QueryInterface (IID_IHTMLDocument2,  (void**)&pHtmlDoc);
		if (SUCCEEDED (hr) && (pHtmlDoc != NULL))
		{
			IHTMLElementCollection* pColl = NULL;
			hr = pHtmlDoc->get_all (&pColl);

			if (SUCCEEDED (hr) && (pColl != NULL))
			{
				// Obtained the Anchor Collection...
				long nLength = 0;
				pColl->get_length (&nLength);
				
				for (int i = 0; i < nLength && bSearch; i++)
				{
					COleVariant vIdx ((long)i, VT_I4);
					
					IDispatch* pElemDispatch = NULL;
					IHTMLElement * pElem = NULL;
					
					hr = pColl->item (vIdx, vIdx, &pElemDispatch);
					
					if (SUCCEEDED (hr) && (pElemDispatch != NULL))
					{
						hr = pElemDispatch->QueryInterface (IID_IHTMLElement, (void**)&pElem);
						
						if (SUCCEEDED (hr) && (pElem != NULL))
						{
							BSTR bstrTagName;
							CString sTempTagName;
							if (!FAILED (pElem->get_tagName (&bstrTagName)))
							{
								sTempTagName = bstrTagName;
								SysFreeString (bstrTagName);
							}
							
							if (sTempTagName == _T ("a") || sTempTagName == _T ("A"))
							{
								IHTMLAnchorElement * pAnchor = NULL;
								hr = pElemDispatch->QueryInterface(IID_IHTMLAnchorElement, (void**)&pAnchor);
								
								if (SUCCEEDED (hr) && (pAnchor != NULL))
								{
									BSTR bstrName, bstrOuterText, bstrURL, bstrTooltip;
									CString sTempName, sTempOuter, sTempURL, sTempTooltip;
									
									if (!FAILED (pElem->get_outerText (&bstrOuterText)))
									{
										sTempOuter = bstrOuterText;
										SysFreeString (bstrOuterText);
									}
									if (!FAILED (pElem->get_title (&bstrTooltip)))
									{
										sTempTooltip = bstrTooltip;
										SysFreeString (bstrTooltip);
									}
									if (!FAILED (pAnchor->get_name (&bstrName)))
									{
										sTempName = bstrName;
										SysFreeString (bstrName);
									}
									if (!FAILED (pAnchor->get_href (&bstrURL)))
									{
										sTempURL = bstrURL;
										SysFreeString (bstrURL);
									}

									// Do the comparison here!
									bool bMatches = true;
									if (bMatches && bName)
									{
										if (!StringHelper::WildcardCompareNoCase (sName, sTempName))
											bMatches = false;
									}
									if (bMatches && bOuterText)
									{
										if (!StringHelper::WildcardCompareNoCase (sOuterText, sTempOuter))
											bMatches = false;
									}
									if (bMatches && bURL)
									{
										if (!StringHelper::WildcardCompareNoCase (sURL, sTempURL))
											bMatches = false;
									}
									if (bMatches && bTooltip)
									{
										if (!StringHelper::WildcardCompareNoCase (sTooltip, sTempTooltip))
											bMatches = false;
									}
									
									if (bMatches)
									{
										// No need to search more!
										bSearch = false;
										
										if (bFocus)
											pAnchor->focus ();
										if (bClick)
											pElem->click ();
									}
									pAnchor->Release ();
								}
							}
							pElem->Release ();
						}
						pElemDispatch->Release ();
					}		
				}
				pColl->Release ();
			}
			pHtmlDoc->Release();
		}
		pHtmlDocDispatch->Release ();
	}
	
	if (bSearch == false)
		return true;

	return false;
}

bool CMyInternetExplorer::FindInput  (IDispatch* pHtmlDocDispatch,bool bClick, bool bSelect, bool bChangeValue, bool bSetCheck,
									  bool bType, bool bName, bool bValue, 
									  LPCTSTR sTypeToLook, LPCTSTR sNameToLook, LPCTSTR sValueToLook,
									  bool bNewCheckValue, LPCTSTR sNewValue)
{
//	ASSERT (m_pWebBrowser != NULL);
//	if (m_pWebBrowser == NULL)
//		return false;
	
	HRESULT hr;
//	IDispatch* pHtmlDocDispatch = NULL;
	IHTMLDocument2 * pHtmlDoc = NULL;
	bool bSearch = true;

	// Retrieve the document object.
//	hr = m_pWebBrowser->get_Document (&pHtmlDocDispatch);
	if (true && (pHtmlDocDispatch != NULL))
	{
		hr = pHtmlDocDispatch->QueryInterface (IID_IHTMLDocument2,  (void**)&pHtmlDoc);
		if (SUCCEEDED (hr) && (pHtmlDoc != NULL))
		{
			IHTMLElementCollection* pColl = NULL;
			hr = pHtmlDoc->get_all (&pColl);

			if (SUCCEEDED (hr) && (pColl != NULL))
			{
				// Obtained the Anchor Collection...
				long nLength = 0;
				pColl->get_length (&nLength);
				
				for (int i = 0; i < nLength && bSearch; i++)
				{
					COleVariant vIdx ((long)i, VT_I4);
					
					IDispatch* pElemDispatch = NULL;
					IHTMLElement * pElem = NULL;
					
					hr = pColl->item (vIdx, vIdx, &pElemDispatch);
					
					if (SUCCEEDED (hr) && (pElemDispatch != NULL))
					{
						hr = pElemDispatch->QueryInterface (IID_IHTMLElement, (void**)&pElem);
						
						if (SUCCEEDED (hr) && (pElem != NULL))
						{
							BSTR bstrTagName;
							CString sTempTagName;
							if (!FAILED (pElem->get_tagName (&bstrTagName)))
							{
								sTempTagName = bstrTagName;
								sTempTagName.MakeLower ();
								//AfxMessageBox (sTempTagName);
								SysFreeString (bstrTagName);
							}
							if (sTempTagName == _T ("input"))
							{
								IHTMLInputElement * pInputElem = NULL;
								hr = pElemDispatch->QueryInterface (IID_IHTMLInputElement, (void**)&pInputElem);
								
								if (SUCCEEDED (hr) && (pInputElem != NULL))
								{
									BSTR bstrType, bstrName, bstrValue;
									CString sTempType, sTempName, sTempValue;
									
									if (!FAILED (pInputElem->get_type (&bstrType)))
									{
										sTempType = bstrType;
										SysFreeString (bstrType);
									}
									if (!FAILED (pInputElem->get_name (&bstrName)))
									{
										sTempName = bstrName;
										SysFreeString (bstrName);
									}
									if (!FAILED (pInputElem->get_value (&bstrValue)))
									{
										sTempValue = bstrValue;
										SysFreeString (bstrValue);
									}
									//AfxMessageBox (_T ("Name: ") + sTempName + _T ("\r\nType: ") + sTempType + _T ("\r\nValue: ") + sTempValue);
									// Do the comparison here!
									bool bMatches = true;
									if (bMatches && bType)
									{
										if (!StringHelper::WildcardCompareNoCase (sTypeToLook, sTempType))
											bMatches = false;
									}
									if (bMatches && bName)
									{
										if (!StringHelper::WildcardCompareNoCase (sNameToLook, sTempName))
											bMatches = false;
									}
									if (bMatches && bValue)
									{
										if (!StringHelper::WildcardCompareNoCase (sValueToLook, sTempValue))
											bMatches = false;
									}
									
									if (bMatches)
									{
										// No need to search more!
										bSearch = false;

										if (bSetCheck)
										{
											if (bNewCheckValue)
												pInputElem->put_checked (VARIANT_TRUE);
											else
												pInputElem->put_checked (VARIANT_FALSE);
										}
										if (bChangeValue)
										{
											CString sTemp (sNewValue);
											BSTR bstrNewValue = sTemp.AllocSysString ();
											pInputElem->put_value (bstrNewValue);
											SysFreeString (bstrNewValue);
										}
										if (bSelect)
											pInputElem->select ();

										if (bClick)
											pElem->click ();
									}
									pInputElem->Release ();
								}
							}
							pElem->Release ();
						}
						pElemDispatch->Release ();
					}		
				}
				pColl->Release ();
			}
			pHtmlDoc->Release();
		}
		pHtmlDocDispatch->Release ();
	}

	if (bSearch == false)
		return true;
	
	return false;
}

bool CMyInternetExplorer::GetInput  (IDispatch* pHtmlDocDispatch,bool bClick, bool bSelect, bool bChangeValue, bool bSetCheck,
									  bool bType, bool bName, bool bValue, 
									  LPCTSTR sTypeToLook, LPCTSTR sNameToLook, LPCTSTR sValueToLook,
									  bool bNewCheckValue, LPCTSTR sNewValue,LPTSTR szValue)
{
//	ASSERT (m_pWebBrowser != NULL);
//	if (m_pWebBrowser == NULL)
//		return false;
	
	HRESULT hr;
//	IDispatch* pHtmlDocDispatch = NULL;
	IHTMLDocument2 * pHtmlDoc = NULL;
	bool bSearch = true;

	// Retrieve the document object.
//	hr = m_pWebBrowser->get_Document (&pHtmlDocDispatch);
	if (true && (pHtmlDocDispatch != NULL))
	{
		hr = pHtmlDocDispatch->QueryInterface (IID_IHTMLDocument2,  (void**)&pHtmlDoc);
		if (SUCCEEDED (hr) && (pHtmlDoc != NULL))
		{
			IHTMLElementCollection* pColl = NULL;
			hr = pHtmlDoc->get_all (&pColl);

			if (SUCCEEDED (hr) && (pColl != NULL))
			{
				// Obtained the Anchor Collection...
				long nLength = 0;
				pColl->get_length (&nLength);
				
				for (int i = 0; i < nLength && bSearch; i++)
				{
					COleVariant vIdx ((long)i, VT_I4);
					
					IDispatch* pElemDispatch = NULL;
					IHTMLElement * pElem = NULL;
					
					hr = pColl->item (vIdx, vIdx, &pElemDispatch);
					
					if (SUCCEEDED (hr) && (pElemDispatch != NULL))
					{
						hr = pElemDispatch->QueryInterface (IID_IHTMLElement, (void**)&pElem);
						
						if (SUCCEEDED (hr) && (pElem != NULL))
						{
							BSTR bstrTagName;
							CString sTempTagName;
							if (!FAILED (pElem->get_tagName (&bstrTagName)))
							{
								sTempTagName = bstrTagName;
								sTempTagName.MakeLower ();
								//AfxMessageBox (sTempTagName);
								SysFreeString (bstrTagName);
							}
							if (sTempTagName == _T ("input"))
							{
								IHTMLInputElement * pInputElem = NULL;
								hr = pElemDispatch->QueryInterface (IID_IHTMLInputElement, (void**)&pInputElem);
								
								if (SUCCEEDED (hr) && (pInputElem != NULL))
								{
									BSTR bstrType, bstrName, bstrValue;
									CString sTempType, sTempName, sTempValue;
									
									if (!FAILED (pInputElem->get_type (&bstrType)))
									{
										sTempType = bstrType;
										SysFreeString (bstrType);
									}
									if (!FAILED (pInputElem->get_name (&bstrName)))
									{
										sTempName = bstrName;
										SysFreeString (bstrName);
									}
									if (!FAILED (pInputElem->get_value (&bstrValue)))
									{
										sTempValue = bstrValue;
										SysFreeString (bstrValue);
									}
									//AfxMessageBox (_T ("Name: ") + sTempName + _T ("\r\nType: ") + sTempType + _T ("\r\nValue: ") + sTempValue);
									// Do the comparison here!
									lstrcpy(szValue,sTempValue);
									bool bMatches = true;
									if (bMatches && bType)
									{
										if (!StringHelper::WildcardCompareNoCase (sTypeToLook, sTempType))
											bMatches = false;
									}
									if (bMatches && bName)
									{
										if (!StringHelper::WildcardCompareNoCase (sNameToLook, sTempName))
											bMatches = false;
									}
									if (bMatches && bValue)
									{
										if (!StringHelper::WildcardCompareNoCase (sValueToLook, sTempValue))
											bMatches = false;
									}
									
									if (bMatches)
									{
										// No need to search more!
										bSearch = false;

										if (bSetCheck)
										{
											if (bNewCheckValue)
												pInputElem->put_checked (VARIANT_TRUE);
											else
												pInputElem->put_checked (VARIANT_FALSE);
										}
										if (bChangeValue)
										{
											CString sTemp (sNewValue);
											BSTR bstrNewValue = sTemp.AllocSysString ();
											pInputElem->put_value (bstrNewValue);
											SysFreeString (bstrNewValue);
										}
										if (bSelect)
											pInputElem->select ();

										if (bClick)
											pElem->click ();
									}
									pInputElem->Release ();
								}
							}
							pElem->Release ();
						}
						pElemDispatch->Release ();
					}		
				}
				pColl->Release ();
			}
			pHtmlDoc->Release();
		}
		pHtmlDocDispatch->Release ();
	}

	if (bSearch == false)
		return true;
	
	return false;
}

bool CMyInternetExplorer::FindOption (bool bClick, 
									  bool bValue, bool bText,
									  LPCTSTR sValue, LPCTSTR sText)
{
	ASSERT (m_pWebBrowser != NULL);
	if (m_pWebBrowser == NULL)
		return false;
	
	HRESULT hr;
	IDispatch* pHtmlDocDispatch = NULL;
	IHTMLDocument2 * pHtmlDoc = NULL;
	bool bSearch = true;

	// Retrieve the document object.
	hr = m_pWebBrowser->get_Document (&pHtmlDocDispatch);
	if (SUCCEEDED (hr) && (pHtmlDocDispatch != NULL))
	{
		hr = pHtmlDocDispatch->QueryInterface (IID_IHTMLDocument2,  (void**)&pHtmlDoc);
		if (SUCCEEDED (hr) && (pHtmlDoc != NULL))
		{
			IHTMLElementCollection* pColl = NULL;
			hr = pHtmlDoc->get_all (&pColl);

			if (SUCCEEDED (hr) && (pColl != NULL))
			{
				// Obtained the Anchor Collection...
				long nLength = 0;
				pColl->get_length (&nLength);
				
				for (int i = 0; i < nLength && bSearch; i++)
				{
					COleVariant vIdx ((long)i, VT_I4);
					
					IDispatch* pElemDispatch = NULL;
					IHTMLElement * pElem = NULL;
					
					hr = pColl->item (vIdx, vIdx, &pElemDispatch);
					
					if (SUCCEEDED (hr) && (pElemDispatch != NULL))
					{
						hr = pElemDispatch->QueryInterface(IID_IHTMLElement, (void**)&pElem);
						
						if (SUCCEEDED (hr) && (pElem != NULL))
						{
							BSTR bstrTagName;
							CString sTempTagName;
							if (!FAILED (pElem->get_tagName (&bstrTagName)))
							{
								sTempTagName = bstrTagName;
								sTempTagName.MakeLower ();
								SysFreeString (bstrTagName);
							}

							if (sTempTagName == _T ("option"))
							{
								IHTMLOptionElement * pOption = NULL;
								hr = pElemDispatch->QueryInterface (IID_IHTMLOptionElement, (void**)&pOption);
								
								if (SUCCEEDED (hr) && (pOption != NULL))
								{
									BSTR bstrValue, bstrText;
									CString sTempValue, sTempText;
									
									if (!FAILED (pOption->get_value (&bstrValue)))
									{
										sTempValue = bstrValue;
										SysFreeString (bstrValue);
									}
									if (!FAILED (pOption->get_text (&bstrText)))
									{
										sTempText = bstrText;
										SysFreeString (bstrText);
									}
									
									//if (IDCANCEL == AfxMessageBox (_T ("Value: ") + sTempValue + _T ("\r\nText: ") + sTempText, MB_OKCANCEL))
									//	bSearch = false;

									// Do the comparison here!
									bool bMatches = true;
									if (bMatches && bValue)
									{
										if (!StringHelper::WildcardCompareNoCase (sValue, sTempValue))
											bMatches = false;
									}
									if (bMatches && bText)
									{
										if (!StringHelper::WildcardCompareNoCase (sText, sTempText))
											bMatches = false;
									}
									
									if (bMatches)
									{
										// No need to search more!
										bSearch = false;
										
										pOption->put_selected (VARIANT_TRUE);
										if (bClick)
											pElem->click ();
									}
									pOption->Release ();
								}
							}
							pElem->Release ();
						}
						pElemDispatch->Release ();
					}		
				}
				pColl->Release ();
			}
			pHtmlDoc->Release();
		}
		pHtmlDocDispatch->Release ();
	}
	
	if (bSearch == false)
		return true;

	return false;
}

bool CMyInternetExplorer::FindElement (bool bExactMatch, bool bWildcardMatch, bool bCaseSensitive, bool bClick,
									   bool bTagName, bool bOuterHTML, bool bInnerHTML, bool bTooltip,
									   LPCTSTR sTagNameOrg, LPCTSTR sOuterHTMLOrg, LPCTSTR sInnerHTMLOrg, LPCTSTR sTooltipOrg, 
									   CString & sRetTagName, CString & sRetOuterHTML, CString & sRetInnerHTML, CString & sRetTooltip)
{
	ASSERT (m_pWebBrowser != NULL);
	if (m_pWebBrowser == NULL)
		return false;
	
	HRESULT hr;
	IDispatch* pHtmlDocDispatch = NULL;
	IHTMLDocument2 * pHtmlDoc = NULL;
	bool bSearch = true;

	// Retrieve the document object.
	hr = m_pWebBrowser->get_Document (&pHtmlDocDispatch);
	if (SUCCEEDED (hr) && (pHtmlDocDispatch != NULL))
	{
		hr = pHtmlDocDispatch->QueryInterface (IID_IHTMLDocument2,  (void**)&pHtmlDoc);
		if (SUCCEEDED (hr) && (pHtmlDoc != NULL))
		{
			IHTMLElementCollection* pColl = NULL;
			hr = pHtmlDoc->get_all (&pColl);

			if (SUCCEEDED (hr) && (pColl != NULL))
			{
				CString sTagName (sTagNameOrg), sOuterHTML (sOuterHTMLOrg), sInnerHTML (sInnerHTMLOrg), sTooltip (sTooltipOrg);
				if (!bCaseSensitive)
				{
					sTagName.MakeLower ();
					sOuterHTML.MakeLower ();
					sInnerHTML.MakeLower ();
					sTooltip.MakeLower ();
				}

				// Obtained the Anchor Collection...
				long nLength = 0;
				pColl->get_length (&nLength);
				
				for (int i = 0; i < nLength && bSearch; i++)
				{
					COleVariant vIdx ((long)i, VT_I4);
					
					IDispatch* pElemDispatch = NULL;
					IHTMLElement * pElem = NULL;
					
					hr = pColl->item (vIdx, vIdx, &pElemDispatch);
					
					if (SUCCEEDED (hr) && (pElemDispatch != NULL))
					{
						hr = pElemDispatch->QueryInterface(IID_IHTMLElement, (void**)&pElem);
						
						if (SUCCEEDED (hr) && (pElem != NULL))
						{
							BSTR bstrTagName, bstrOuterHTML, bstrInnerHTML, bstrTooltip;
							CString sTempTagName, sTempOuterHTML, sTempInnerHTML, sTempTooltip;
							if (!FAILED (pElem->get_tagName (&bstrTagName)))
							{
								sTempTagName = bstrTagName;
								SysFreeString (bstrTagName);
							}
							if (!FAILED (pElem->get_outerHTML (&bstrOuterHTML)))
							{
								sTempOuterHTML = bstrOuterHTML;
								SysFreeString (bstrOuterHTML);
							}
							if (!FAILED (pElem->get_innerHTML (&bstrInnerHTML)))
							{
								sTempInnerHTML = bstrInnerHTML;
								SysFreeString (bstrInnerHTML);
							}
							if (!FAILED (pElem->get_title (&bstrTooltip)))
							{
								sTempTooltip = bstrTooltip;
								SysFreeString (bstrTooltip);
							}
							
							sRetTagName = sTempTagName;
							sRetOuterHTML = sTempOuterHTML;
							sRetInnerHTML = sTempInnerHTML;
							sRetTooltip = sTempTooltip;

							if (!bCaseSensitive)
							{
								sTempTagName.MakeLower ();
								sTempOuterHTML.MakeLower ();
								sTempInnerHTML.MakeLower ();
								sTempTooltip.MakeLower ();
							}

							// Do the comparison here!
							bool bMatches = true;
							if (bMatches && bTagName)
							{
								if (bExactMatch)
								{
									if (sTempTagName != sTagName)
										bMatches = false;
								}
								else if (bWildcardMatch)
								{
									if (bCaseSensitive)
									{
										if (!StringHelper::WildcardCompareNoCase (sTagName, sTempTagName))
											bMatches = false;
									}
									else
									{
										if (!StringHelper::WildcardCompare (sTagName, sTempTagName))
											bMatches = false;
									}
								}
								// Just locate the text inside!
								else
								{
									if (sTempTagName.Find (sTagName) == -1)
										bMatches = false;
								}
							}
							if (bMatches && bOuterHTML)
							{
								if (bExactMatch)
								{
									if (sTempOuterHTML != sOuterHTML)
										bMatches = false;
								}
								else if (bWildcardMatch)
								{
									if (bCaseSensitive)
									{
										if (!StringHelper::WildcardCompareNoCase (sOuterHTML, sTempOuterHTML))
											bMatches = false;
									}
									else
									{
										if (!StringHelper::WildcardCompare (sOuterHTML, sTempOuterHTML))
											bMatches = false;
									}
								}
								// Just locate the text inside!
								else
								{
									if (sTempOuterHTML.Find (sOuterHTML) == -1)
										bMatches = false;
								}
							}
							if (bMatches && bInnerHTML)
							{
								if (bExactMatch)
								{
									if (sTempInnerHTML != sInnerHTML)
										bMatches = false;
								}
								else if (bWildcardMatch)
								{
									if (bCaseSensitive)
									{
										if (!StringHelper::WildcardCompareNoCase (sInnerHTML, sTempInnerHTML))
											bMatches = false;
									}
									else
									{
										if (!StringHelper::WildcardCompare (sInnerHTML, sTempInnerHTML))
											bMatches = false;
									}
								}
								// Just locate the text inside!
								else
								{
									if (sTempInnerHTML.Find (sInnerHTML) == -1)
										bMatches = false;
								}
							}
							if (bMatches && bTooltip)
							{
								if (bExactMatch)
								{
									if (sTempTooltip != sTooltip)
										bMatches = false;
								}
								else if (bWildcardMatch)
								{
									if (bCaseSensitive)
									{
										if (!StringHelper::WildcardCompareNoCase (sTooltip, sTempTooltip))
											bMatches = false;
									}
									else
									{
										if (!StringHelper::WildcardCompare (sTooltip, sTempTooltip))
											bMatches = false;
									}
								}
								// Just locate the text inside!
								else
								{
									if (sTempTooltip.Find (sTooltip) == -1)
										bMatches = false;
								}
							}
							// Done with the comparisons!
							if (bMatches)
							{
								//if (IDCANCEL == AfxMessageBox (_T ("Tag: ") + sTempTagName + _T ("\r\nOuter: ") + sTempOuterHTML + _T ("\r\nInner: ") + sTempInnerHTML + _T ("\r\nTooltip: ") + sTempTooltip, MB_OKCANCEL))
								//	bSearch = false;

								// No need to search more!
								bSearch = false;
								if (bClick)
									pElem->click ();
							}
							pElem->Release ();
						}
						pElemDispatch->Release ();
					}		
				}
				pColl->Release ();
			}
			pHtmlDoc->Release();
		}
		pHtmlDocDispatch->Release ();
	}

	sRetTagName.Empty ();
	sRetOuterHTML.Empty ();
	sRetInnerHTML.Empty ();
	sRetTooltip.Empty ();
	
	if (bSearch == false)
		return true;
	
	return false;
}

