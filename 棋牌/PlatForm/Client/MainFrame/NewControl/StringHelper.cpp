/*********************************************************************
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

// StringHelper.cpp: implementation of the CStringHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StringHelper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

bool StringHelper::WildcardCompareNoCase (LPCTSTR strWild, LPCTSTR strText)
{
	int cp = 0, mp = 0;
	int idxWild = 0, idxString = 0;
	int nStrLen = _tcslen (strText);
	int nWildLen = _tcslen (strWild);

	while (idxString < nStrLen)
	{
		if (idxWild >= nWildLen)
			break;
		if (strWild [idxWild] == _T ('*'))
			break;
		
		if ((toupper (strWild [idxWild]) != toupper (strText [idxString]) && (strWild [idxWild] != _T ('?'))))
			return false;
		
		++ idxWild;
		++ idxString;
	}
	
	while (idxString < nStrLen) 
	{
		if (idxWild >= nWildLen)
			break;
		
		if (strWild [idxWild] == _T ('*')) 
		{
			++ idxWild;
			if (idxWild >= nWildLen)
				return true;
			
			mp = idxWild;
			cp = idxString + 1;
		} 
		else if ((toupper (strWild [idxWild]) == toupper (strText [idxString])) || (strWild [idxWild] == _T ('?'))) 
		{
			++ idxWild;
			++ idxString;
		}
		else 
		{
			idxWild = mp;
			idxString = cp ++;
		}
	}
	
	while (idxWild < nWildLen)
	{
		if (strWild [idxWild] != _T ('*'))
			break;
		++ idxWild;
	}
	
	if (idxWild < nWildLen)
		return false;
	else
		return true;
}

bool StringHelper::WildcardCompare (LPCTSTR strWild, LPCTSTR strText)
{
	int cp = 0, mp = 0;
	int idxWild = 0, idxString = 0;
	int nStrLen = _tcslen (strText);
	int nWildLen = _tcslen (strWild);
	
	while (idxString < nStrLen)
	{
		if (idxWild >= nWildLen)
			break;
		if (strWild [idxWild] == _T ('*'))
			break;
		
		if ((strWild [idxWild] != strText [idxString] && (strWild [idxWild] != _T ('?'))))
			return false;
		
		++ idxWild;
		++ idxString;
	}
	
	while (idxString < nStrLen) 
	{
		if (idxWild >= nWildLen)
			break;
		
		if (strWild [idxWild] == _T ('*')) 
		{
			++ idxWild;
			if (idxWild >= nWildLen)
				return true;
			
			mp = idxWild;
			cp = idxString + 1;
		} 
		else if ((strWild [idxWild] == strText [idxString]) || (strWild [idxWild] == _T ('?'))) 
		{
			++ idxWild;
			++ idxString;
		}
		else 
		{
			idxWild = mp;
			idxString = cp ++;
		}
	}
	
	while (idxWild < nWildLen)
	{
		if (strWild [idxWild] != _T ('*'))
			break;
		++ idxWild;
	}
	
	if (idxWild < nWildLen)
		return false;
	else
		return true;
}
