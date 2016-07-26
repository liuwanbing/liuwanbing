#include "StdAfx.h"
#include ".\gif.h"

using namespace Gdiplus;

ULONG_PTR CGif::m_GdiplusToken = NULL;
int CGif::m_iInitializeCount = 0;

CString const ErrorFileName("GifRendererError.txt");

BOOL WriteToFile(LPCTSTR lpstrFilePathName, LPCTSTR lpstrErrorText)
{
	return true;
	CFile	tempFile;
	if (! tempFile.Open(lpstrFilePathName, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite|CFile::shareDenyWrite|CFile::typeBinary))
	{
		return FALSE;
	}

	tempFile.SeekToEnd();

	char szNewLine[] = "\r\n";
	tempFile.Write(szNewLine, strlen(szNewLine));			//换行	
	
#ifdef UNICODE
	//char	szErrorText[255]= {0};
	//::WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, lpstrErrorText, wcslen(lpstrErrorText), szErrorText, 255, NULL, NULL);
	//tempFile.Write(szErrorText, strlen(szErrorText));
	//tempFile.Write(lpstrErrorText, strlen((char *)lpstrErrorText));
#else
	tempFile.Write(lpstrErrorText, strlen(lpstrErrorText));
#endif

	//写入游戏信息
	tempFile.Close();
	return TRUE;
}

CGif::CGif(void)
{
	m_pImage = NULL;
	m_uiFrameCount = 1;
	m_pFrameDelay = NULL;
}

CGif::~CGif(void)
{
	if (NULL != m_pImage)
	{
		delete m_pImage;
	}

	if (NULL != m_pFrameDelay)
	{
		delete m_pFrameDelay;
	}

	m_LastTimeSwitchFrame = -1;
}

HRESULT CGif::Initialize()
{
	if (m_iInitializeCount > 0)
	{
		return E_FAIL;
	}

   GdiplusStartupInput gdiplusStartupInput;
   
   if (Ok == GdiplusStartup(&m_GdiplusToken, &gdiplusStartupInput, NULL))
   {
	   ++m_iInitializeCount;
	   return NOERROR;
   }
   else
   {
	   return E_FAIL;
   }
}

HRESULT CGif::Uninitialize()
{
	if (m_iInitializeCount < 1)
	{
		return E_FAIL;
	}

	GdiplusShutdown(m_GdiplusToken);
	--m_iInitializeCount;
	return NOERROR;
}

ULONG CGif::Release()
{
	delete this;
	return 0;
}

//TCHAR	wszFilePathName[255] = {0};
//::MultiByteToWideChar(CP_ACP, MB_COMPOSITE, lpstrFileName, strlen((LPSTR)lpstrFileName), wszFilePathName, 255);

HRESULT CGif::LoadFromFile(LPCTSTR lpstrFileName)
{
	//DebugPrintf("道具GIF 头=%s ",lpstrFileName);
	if (NULL != m_pImage)
	{
		delete m_pImage;
		m_pImage = NULL;
	}

#ifndef BZ_ZLIB
	CFile	TempFile;
	if (! TempFile.Open(lpstrFileName, CFile::modeRead))
	{
		CString	 str(lpstrFileName);
		str.Format(_TEXT("LoadFromFile() failed , reason : open file \" %s \"Failed"), lpstrFileName);
		//WriteToFile(ErrorFileName, str);

		return E_FAIL;
	}



	LONGLONG  llFileSize = TempFile.GetLength();
	HGLOBAL		hGlobalMem = GlobalAlloc(GPTR, static_cast<SIZE_T>(llFileSize));
	if (NULL == hGlobalMem)
	{
		CString	 str("LoadFromFile() failed , reason : malloc memory failed");
		WriteToFile(ErrorFileName, str);

		TempFile.Close();
		return E_OUTOFMEMORY;
	}

	TempFile.SeekToBegin();
	void	*pBuf = GlobalLock(hGlobalMem);
	int		iReadSize = TempFile.Read(pBuf, static_cast<UINT>(llFileSize));
	GlobalUnlock(hGlobalMem);
	TempFile.Close();
#endif

	IStream		*pIStream = NULL;

#ifdef BZ_ZLIB

	TCHAR pchar[MAX_PATH];
	ZeroMemory(pchar,MAX_PATH);
	memcpy(pchar,lpstrFileName,strlen(lpstrFileName));
	CUnZipRes unZipRes;
	pIStream=unZipRes.LoadFileResFromZip(pchar);
	//DebugPrintf("道具GIF pchar=%s",pchar);
	//DebugPrintf("道具GIF pIStream=%d",pIStream);

#else

	HRESULT		hr = CreateStreamOnHGlobal(hGlobalMem, TRUE, &pIStream);
	//GlobalFree(hGlobalMem);
	if (S_OK != hr)
	{
		CString	 str("LoadFromFile() failed , reason : Create Stream failed");
		WriteToFile(ErrorFileName, str);

		return E_FAIL;
	}

//	DebugPrintf("道具GIF S_OK");
#endif

	if (NULL == pIStream)
	{
		CString	 str("LoadFromFile() failed , reason : Stream = NULL ");
		WriteToFile(ErrorFileName, str);
		return E_FAIL;
	}

	m_pImage = Image::FromStream(pIStream, FALSE);
	if (NULL == m_pImage)
	{

		CString	 str("LoadFromFile() failed , reason : form finally object failed!");
		WriteToFile(ErrorFileName, str);
		return E_FAIL;
	}

	if (m_pImage->GetLastStatus() != Ok)
	{
		CString	 str("LoadFromFile() failed , reason : status Error ");
		WriteToFile(ErrorFileName, str);

		delete m_pImage;
		m_pImage = NULL;
		return E_FAIL;
	}

	int			iDimensionsCount = m_pImage->GetFrameDimensionsCount();
	GUID		*pDimensionIDs = new GUID[iDimensionsCount];
	m_pImage->GetFrameDimensionsList(pDimensionIDs, iDimensionsCount);
	m_uiFrameCount = m_pImage->GetFrameCount(pDimensionIDs);
	iDimensionsCount > 1 ? delete []pDimensionIDs : delete pDimensionIDs;

	try
	{
		UINT uiPropertyItemSize = m_pImage->GetPropertyItemSize(PropertyTagFrameDelay);
		PropertyItem *pPropertyItem = (PropertyItem *)malloc(uiPropertyItemSize);
		m_pImage->GetPropertyItem(PropertyTagFrameDelay, uiPropertyItemSize, pPropertyItem);
		//m_uiFrameDelay = *(UINT *)(pPropertyItem->value);
		m_pFrameDelay = new UINT[m_uiFrameCount];
		if (NULL == m_pFrameDelay)
		{
			return E_FAIL;
		}

		memcpy(m_pFrameDelay, pPropertyItem->value, sizeof(UINT) * m_uiFrameCount);

		//char szInfo[255] = {0};
		//sprintf(szInfo, "%u, %u, frame count = %u,%u", sizeof(*pPropertyItem), uiPropertyItemSize, m_uiFrameCount, m_uiFrameDelay);
		//AfxMessageBox(szInfo);

		free(pPropertyItem);
	}
	catch (...)
	{
		AfxMessageBox("ERrror");
	}

	m_uiCurSelFrameID =  0;
	m_LastTimeSwitchFrame = GetTickCount();
	if(pIStream!=NULL)
	{
		pIStream->Release();
		pIStream=NULL;
	}


	return NOERROR;
}

void CGif::Reset()
{
	m_uiCurSelFrameID =  0;
	m_LastTimeSwitchFrame = GetTickCount();
}

HRESULT CGif::LoadFromRes(LPCTSTR lpstrResDirectory, LPCTSTR lpstrResName)
{
	return E_NOTIMPL;
}

HRESULT CGif::Bitblt(DWORD dwFrameIndex, CDC *pDestDC, int ixDest, int iyDest)
{
	if (NULL == pDestDC)
	{
		CString  strError("NULL == pDestDC");
		//AfxMessageBox(strError);
		return E_POINTER;
	}

	if (NULL == m_pImage)
	{
		//CString  strError("Image NotLoad");
		//AfxMessageBox(strError);
		return E_FAIL;
	}

	////////////////////
	int			iDimensionsCount = m_pImage->GetFrameDimensionsCount();
	GUID		*pDimensionIDs = new GUID[iDimensionsCount];
	if (NULL != pDimensionIDs)
	{
		//// Get the list of frame dimensions from the Image object.
		m_pImage->GetFrameDimensionsList(pDimensionIDs, iDimensionsCount);

		// Get the number of frames in the first dimension.
		UINT		uiFrameCount = m_pImage->GetFrameCount(pDimensionIDs);
		UINT		uiDrawFrameID = dwFrameIndex % uiFrameCount;

		Status  status = m_pImage->SelectActiveFrame(pDimensionIDs, uiDrawFrameID);
		if (Ok == status)
		{
			Graphics	graphics(pDestDC->GetSafeHdc());
			status = graphics.DrawImage(m_pImage, ixDest, iyDest);
			graphics.ReleaseHDC(pDestDC->GetSafeHdc());

			//if (Ok != status)
			//{
			//	CString  strError("draw Failed;");
			//	AfxMessageBox(strError);
			//}
		}
		else
		{
			//CString  strError("Set Frame Failed;");
			//AfxMessageBox(strError);
		}

		iDimensionsCount > 1 ? delete []pDimensionIDs : delete pDimensionIDs;
		if (Ok == status)
		{
			return NOERROR;
		}
		else
		{
			return E_FAIL;
		}
	}
	else
	{
		CString  strError("not enough memory");
		AfxMessageBox(strError);
		return E_OUTOFMEMORY;
	}	
}

HRESULT CGif::Bitblt(CDC *pDestDC, int ixDest, int iyDest)
{
	if (NULL == m_pImage)
	{
		return E_FAIL;
	}

	DWORD  dwCurTime = GetTickCount();

	//CString  strError;
	//strError.Format("m_LastTimeSwitchFramem = %u, Delay = %u, dwCurTime = %u", m_LastTimeSwitchFrame, m_pFrameDelay[m_uiCurSelFrameID], dwCurTime);
	//AfxMessageBox(strError);

	if (m_LastTimeSwitchFrame + m_pFrameDelay[m_uiCurSelFrameID] * 10 < dwCurTime)
	{
		m_LastTimeSwitchFrame += (m_pFrameDelay[m_uiCurSelFrameID] * 10);

		++m_uiCurSelFrameID;
		if (m_uiCurSelFrameID == m_uiFrameCount)
		{
			m_uiCurSelFrameID = 0;
		}
	}

	return Bitblt(m_uiCurSelFrameID, pDestDC, ixDest, iyDest);
}

HRESULT CGif::StretchBlt(DWORD dwFrameIndex, CDC *pDestDC, int ixDest, int iyDest, int iWidth, int iHeight)
{
	if (NULL == m_pImage)
	{
		return E_FAIL;
	}

	////////////////////
	int			iDimensionsCount = m_pImage->GetFrameDimensionsCount();
	GUID		*pDimensionIDs = new GUID[iDimensionsCount];
	if (NULL == pDimensionIDs)
	{
		CString  strError("not enough memory");
		AfxMessageBox(strError);
		return E_OUTOFMEMORY;
	}
	else
	{
		//// Get the list of frame dimensions from the Image object.
		m_pImage->GetFrameDimensionsList(pDimensionIDs, iDimensionsCount);

		// Get the number of frames in the first dimension.
		UINT		uiFrameCount = m_pImage->GetFrameCount(pDimensionIDs);
		UINT		uiDrawFrameID = dwFrameIndex % uiFrameCount;

		Status		status = m_pImage->SelectActiveFrame(pDimensionIDs, uiDrawFrameID);
		if (Ok != status)
		{
			CString  strError("Set Frame Failed;");
			AfxMessageBox(strError);
		}
		else
		{
			Graphics	graphics(pDestDC->GetSafeHdc());
			Status		status = graphics.DrawImage(m_pImage, ixDest, iyDest, iWidth, iHeight);
			graphics.ReleaseHDC(pDestDC->GetSafeHdc());

			if (Ok != status)
			{
				CString  strError("draw Failed;");
				AfxMessageBox(strError);
			}
		}
		
		iDimensionsCount > 1 ? delete []pDimensionIDs : delete pDimensionIDs;
		return NOERROR;
	}
}

DWORD CGif::GetImageWidth()
{
	if (NULL == m_pImage)
	{
		return 0;
	}

	return m_pImage->GetWidth();
}

DWORD CGif::GetImageHeight()
{
	if (NULL == m_pImage)
	{
		return 0;
	}

	return m_pImage->GetHeight();
}

DWORD CGif::GetFrameCount()
{
	if (NULL == m_pImage)
	{
		return 0;
	}

	return m_uiFrameCount;
}
