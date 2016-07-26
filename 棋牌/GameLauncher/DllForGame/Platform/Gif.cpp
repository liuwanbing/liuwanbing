#include "../stdafx.h"
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
	if(0 != m_pImageList.empty())
	{
		std::vector<Gdiplus::Image *>::iterator iter = m_pImageList.begin();
		for(; iter != m_pImageList.end(); ++iter)
		{
			if(NULL != (*iter))
				delete (*iter);
			iter = m_pImageList.erase(iter);
		}
	}
	
	m_pImage = NULL;

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
	DebugPrintf("道具GIF 头=%s ",lpstrFileName);
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

		AfxMessageBox(str);
		TempFile.Close();
		return E_OUTOFMEMORY;
	}

	TempFile.SeekToBegin();
	void	*pBuf = GlobalLock(hGlobalMem);
	int		iReadSize = TempFile.Read(pBuf, static_cast<UINT>(llFileSize));
	GlobalUnlock(hGlobalMem);
	TempFile.Close();
#endif

#ifdef BZ_ZLIB


	TCHAR pchar[MAX_PATH];
	ZeroMemory(pchar,MAX_PATH);
	memcpy(pchar,lpstrFileName,strlen(lpstrFileName));
	CUnZipRes unZipRes;

	int		iReadSize;
	void	*pBuf = unZipRes.LoadFileBufferFromZip(pchar, iReadSize);

	//DebugPrintf("道具GIF pchar=%s",pchar);
	//DebugPrintf("道具GIF pIStream=%d",pIStream);

	if (NULL == pBuf)
	{
		CString	 str("LoadFromFile() failed , reason : Stream = NULL ");
		WriteToFile(ErrorFileName, str);
		return E_FAIL;
	}

#endif

	//从内存中加载动画文件
	m_animator.OpenAnimatorFromMemory((BYTE *)pBuf, iReadSize);

	//获取帧数量 
	m_uiFrameCount = m_animator.GetFrameCount();

	//加载每个帧图片到帧列表
	LoadImageListFromAnimator();

	//设置每个帧的延时
	m_pFrameDelay = new UINT[m_uiFrameCount];	//new帧延时
	if (NULL == m_pFrameDelay)
	{
		//OutputDebugString("zhtout:NULL == m_pFrameDelay");
		return E_FAIL;
	}

	for(int i = 0; i < m_uiFrameCount; i++)
	{
		m_pFrameDelay[i] = m_animator.GetSpeed();
	}

	m_uiCurSelFrameID =  0;
	m_LastTimeSwitchFrame = GetTickCount();  //获取第一次计时值

	//设置第一张图片
	m_pImage = GetFrameData(0); 

	CString str;

	m_animator.GetWidthAndHeight(&m_nWidth, &m_nHeight);

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

	BOOL status; 

	//选择图片
	m_pImage =  GetFrameData(dwFrameIndex);

	if (NULL != m_pImage)
	{
		//绘图
		status = DrawImage(pDestDC,ixDest,iyDest);

		if (TRUE != status)
		{
			return E_FAIL;
		}
	}
	else
	{
		//CString  strError("Set Frame Failed;");
		//AfxMessageBox(strError);
		return E_FAIL;
	}

	return NOERROR;
}

HRESULT CGif::Bitblt(CDC *pDestDC, int ixDest, int iyDest)
{
	if (NULL == m_pImage)
	{
		//OutputDebugString("zhtout:NULL == m_pImage");

		return E_FAIL;
	}

	DWORD  dwCurTime = GetTickCount();

	/*CString  strError;
	strError.Format("zhtout:m_LastTimeSwitchFramem = %u, Delay = %u, dwCurTime = %u", m_LastTimeSwitchFrame, m_pFrameDelay[m_uiCurSelFrameID], dwCurTime);
	OutputDebugString(strError);*/

	//判断是否到底下一帧的时间 
	if (m_LastTimeSwitchFrame + m_pFrameDelay[m_uiCurSelFrameID]  < dwCurTime)
	{
		m_LastTimeSwitchFrame += (m_pFrameDelay[m_uiCurSelFrameID] );
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

	BOOL status; 

	//选择图片
	m_pImage =  GetFrameData(dwFrameIndex);

	if (NULL != m_pImage)
	{
		//绘图
		status = DrawImage(pDestDC,ixDest,iyDest);

		if (TRUE != status)
		{
			return E_FAIL;
		}
	}
	else
	{
		//CString  strError("Set Frame Failed;");
		//AfxMessageBox(strError);
		return E_FAIL;
	}

	return NOERROR;
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


bool CGif::IsNull()
{
	if (NULL == m_pImage) return true;
	if (Ok != m_pImage->GetLastStatus())
	{
		delete m_pImage;
		m_pImage = NULL;
		return true;
	}
	return false;
}

BOOL CGif::DrawImage(CDC * pDC, POINT * pPoint, int srcx, int srcy, int srcwidth, int srcheight, float * pfClrAttr)
{
	ASSERT(NULL != m_pImage);
	if (NULL == m_pImage) return 1;

	if (IsNull()) return 2;

	Graphics graph(pDC->GetSafeHdc());

	Gdiplus::Point points[3];

	memset(points, 0, sizeof (points));
	if (NULL == pPoint)
	{
		points[1].X = m_nWidth;
		points[2].Y = m_nHeight;
	}
	else
	{
		for (BYTE i = 0;i < 3;i++)
		{
			points[i].X = pPoint[i].x;
			points[i].Y = pPoint[i].y;
		}
	}
	if ((0 == srcwidth)||(0 == srcheight))
	{
		srcwidth = m_nWidth;
		srcheight = m_nHeight;
	}
	if (NULL == pfClrAttr)
	{
		graph.DrawImage(m_pImage, points, 3, srcx, srcy, srcwidth, srcheight, UnitPixel);
	}
	else
	{
#ifdef _DEBUG
		for (BYTE i = 0;i < 25;i++)
		{
			ASSERT(0 <= pfClrAttr[i]);
		}
#endif
		ColorMatrix colorMatrix;
		memcpy(&colorMatrix, pfClrAttr, sizeof (colorMatrix));

		ImageAttributes imageAttr;
		imageAttr.SetColorMatrix(&colorMatrix);

		graph.DrawImage(m_pImage, points, 3, srcx, srcy, srcwidth, srcheight, UnitPixel, &imageAttr);
	}

	graph.ReleaseHDC(pDC->GetSafeHdc());

	return TRUE;
}

BOOL CGif::DrawImage(CDC * pDC, int xDest, int yDest, int nDestWidth, int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, float fAlpha)
{
	if ((0 == m_nWidth)||(0 == m_nHeight)||(0 == nDestWidth)||(0 == nDestHeight)||(0 == nSrcWidth)||(0 == nSrcHeight))
	{
		ASSERT(FALSE);
		return FALSE;
	}

	POINT ptShow[3];
	ptShow[0].x = xDest;
	ptShow[0].y = yDest;
	ptShow[1].x = nDestWidth + xDest;
	ptShow[1].y = yDest;
	ptShow[2].x = xDest;
	ptShow[2].y = nDestHeight + yDest;
	if (-1 == fAlpha)
	{
		return DrawImage(pDC, ptShow, xSrc, ySrc, nSrcWidth, nSrcHeight);
	}
	else
	{
		ASSERT(0 <=fAlpha);
		float fColorArray[25] =
		{
			1, 0, 0, 0, 0,
			0, 1, 0, 0, 0,
			0, 0, 1, 0, 0,
			0, 0, 0, fAlpha, 0,
			0, 0, 0, 0, 1
		};
		return DrawImage(pDC, ptShow, xSrc, ySrc, nSrcWidth, nSrcHeight, fColorArray);
	}
}

BOOL CGif::DrawImage(CDC * pDC, int xDest, int yDest, float fAlpha)
{
	return DrawImage(pDC, xDest, yDest, m_nWidth, m_nHeight, 0, 0, m_nWidth, m_nHeight, fAlpha);
}

Gdiplus::Image* CGif::GetFrameData(int index)
{
	DebugPrintf(strDebugView);
	if(m_pImageList.empty() || 
		index < 0 || index >= (int)m_pImageList.size())
		return NULL;

	return m_pImageList[index];
}

//从动画类里加载每个帧数据到帧列表
BOOL CGif::LoadImageListFromAnimator()
{
	for(int i = 0; i < m_uiFrameCount; i++)
	{
			// 从动画文件中得到这帧的数据，然后加入到动画显示器中
			ULONG uLength = 0;
			Byte *pBuffer = m_animator.GetFrameById(i, &uLength);		

			LoadImageFromMemory(pBuffer, uLength);
	}
	return TRUE;
}

/**
 * 从内存中导入图片数据
 *
 * @param data 要导入的图片数据
 * @param length 要导入的图片数据大小
 */
BOOL CGif::LoadImageFromMemory(unsigned char *data,unsigned long length)
{
	if(!data || length <= 0) 
		return FALSE;

	HGLOBAL hGlobal = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_NODISCARD, length);
	if ( !hGlobal )
	{
		return FALSE;
	}

	unsigned char* lpBuffer = reinterpret_cast<unsigned char*> ( ::GlobalLock(hGlobal) );
	
	memcpy(lpBuffer,data,length);

	::GlobalUnlock(hGlobal);

	IStream* pStream = NULL;
	if ( ::CreateStreamOnHGlobal(hGlobal,FALSE,&pStream) != S_OK )
	{
		::GlobalFree(hGlobal);
		return FALSE;
	}

	Gdiplus::Image *pNewImage;
	pNewImage = Gdiplus::Image::FromStream(pStream);

	m_pImageList.push_back(pNewImage);

	::GlobalFree(hGlobal);

	free(data);
	data = NULL;

	return TRUE;
}
