#include "../stdafx.h"
#include "AFCImage.h"

//using namespace AFC;
//using namespace AFC::AFCImageGoGoGo;

/*******************************************************************************************************/

//构造函数
CGameViewImage::CGameViewImage()
{
	m_hFileMap=NULL;
	m_hDrawDib=NULL;
	m_pImageHead=NULL;
	m_pImageInfo=NULL;
}

//析构函数
CGameViewImage::~CGameViewImage()
{
	Destroy();
}

//加载位图
bool CGameViewImage::Load(const TCHAR * szFileName)
{
	if (m_pImageHead==NULL)
	{
		try
		{
			//读取文件
			CFile ImageFile;
			if (ImageFile.Open(szFileName,CFile::modeRead)==FALSE) throw TEXT("OpenFileError");
			m_hFileMap=CreateFileMapping(ImageFile.m_hFile,NULL,PAGE_READONLY,0,(DWORD)ImageFile.GetLength(),NULL);
			if (m_hFileMap==NULL) throw TEXT("CreateFileMapping Error");
			m_pImageHead=(BITMAPFILEHEADER *)MapViewOfFile(m_hFileMap,FILE_MAP_READ,0,0,0);
			if (m_pImageHead==NULL) throw TEXT("MapViewOfFile Error");
			m_pImageInfo=(BITMAPINFOHEADER *)(m_pImageHead+1);
			m_hDrawDib=DrawDibOpen();
			if (m_hDrawDib==NULL) throw TEXT("DrawDibOpen Error");
			return true;
		}
		catch (...) {TRACE("%s\n",__FUNCTION__);}
		
		//清理资源
		Destroy();
	}
	return false;
}
//销毁位图
bool CGameViewImage::Destroy()
{
	if (m_pImageHead!=NULL) UnmapViewOfFile(m_pImageHead);
	if (m_hFileMap!=NULL) CloseHandle(m_hFileMap);
	if (m_hDrawDib!=NULL) DrawDibClose(m_hDrawDib);
	m_pImageHead=NULL;
	m_pImageInfo=NULL;
	return true;
}
//获取宽度
UINT CGameViewImage::GetWidth()
{
	if (m_pImageInfo==NULL) return 0;
	return m_pImageInfo->biWidth;
}

//获取高度
UINT CGameViewImage::GetHeight()
{
	if (m_pImageInfo==NULL) return 0;
	return m_pImageInfo->biHeight;
}
//绘画函数
bool CGameViewImage::BitBlt(CDC * pDesDC, UINT uXDes, UINT uYDes, UINT uDesWidth, UINT uDesHeight, UINT uXScr, UINT uYScr, UINT uScrWidth, UINT uScrHeight)
{
	DrawDibDraw(m_hDrawDib,pDesDC->GetSafeHdc(),uXDes,uYDes,uDesWidth,uDesHeight,
		(LPBITMAPINFOHEADER)((BYTE *)m_pImageHead+sizeof(BITMAPFILEHEADER)),
		((BYTE *)m_pImageHead)+m_pImageHead->bfOffBits,uXScr,uYScr,uScrWidth,uScrHeight,0);
	return true;
}
/*******************************************************************************************************/

//构造函数
CGameImageHelper::CGameImageHelper(IAFC_GDI_Resource * pGDIObject)
{
	m_pAFCImage=NULL;
	OpenGDIHandle(pGDIObject);
}

//析构函数
CGameImageHelper::~CGameImageHelper()
{
	CloseGDIHandle();
}

//打开句柄
bool CGameImageHelper::OpenGDIHandle(IAFC_GDI_Resource * pGDIObject)
{
	CloseGDIHandle();
	if ((pGDIObject!=NULL)&&(pGDIObject->GetResourceKind()==AFC_IMAGE))
	{
		m_pAFCImage=static_cast<CGameImage *>(pGDIObject);
		m_pAFCImage->LockResource();
		return true;
	}
	return false;
}

//关闭句柄
bool CGameImageHelper::CloseGDIHandle()
{
	if (m_pAFCImage!=NULL) 
	{
		m_pAFCImage->FreeResource();
		return true;
	} 
	return false;
}

//获取类型
AFCResourceType CGameImageHelper::GetResourceKind()
{
	if (m_pAFCImage!=NULL) return m_pAFCImage->GetResourceKind();
	return AFC_NO_OBJECT;
}

//绘图函数
BOOL CGameImageHelper::BitBlt(HDC hDestDC, int xDest, int yDest, DWORD dwROP)
{
	return m_pAFCImage->BitBlt(hDestDC,xDest,yDest,dwROP);
}

//绘图函数
BOOL CGameImageHelper::BitBlt(HDC hDestDC, const POINT & pointDest, DWORD dwROP)
{
	return m_pAFCImage->BitBlt(hDestDC,pointDest,dwROP);
}

//绘图函数
BOOL CGameImageHelper::BitBlt(HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, int xSrc, int ySrc, DWORD dwROP)
{
	return m_pAFCImage->BitBlt(hDestDC,xDest,yDest,nDestWidth,nDestHeight,xSrc,ySrc,dwROP);
}

//绘图函数
BOOL CGameImageHelper::BitBlt(HDC hDestDC,const RECT & rectDest, const POINT & pointSrc, DWORD dwROP)
{
	return m_pAFCImage->BitBlt(hDestDC,rectDest,pointSrc,dwROP);
}

//绘图函数
BOOL CGameImageHelper::StretchBlt(HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, DWORD dwROP)
{
	return m_pAFCImage->StretchBlt(hDestDC,xDest,yDest,nDestWidth,nDestHeight,dwROP);
}

//绘图函数
BOOL CGameImageHelper::StretchBlt(HDC hDestDC, const RECT& rectDest, DWORD dwROP)
{
	return m_pAFCImage->StretchBlt(hDestDC,rectDest,dwROP);
}

//绘图函数
BOOL CGameImageHelper::StretchBlt(HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, DWORD dwROP)
{
	return m_pAFCImage->StretchBlt(hDestDC,xDest,yDest,nDestWidth,nDestHeight,xSrc,ySrc,nSrcWidth,nSrcHeight,dwROP);
}

//绘图函数
BOOL CGameImageHelper::StretchBlt(HDC hDestDC, const RECT& rectDest, const RECT& rectSrc, DWORD dwROP)
{
	return m_pAFCImage->StretchBlt(hDestDC,rectDest,rectSrc,dwROP);
}

//操作重载
CGameImageHelper & CGameImageHelper::operator = (CGameImageHelper & ImageHandle)
{
	m_pAFCImage->FreeResource();
	m_pAFCImage=ImageHandle.m_pAFCImage;
	m_pAFCImage->LockResource();
	return (*this);
}

//操作重载
CGameImageHelper::operator HBITMAP ()
{
	if (IsOpenHandle()==false) return NULL;
	return (HBITMAP)m_pAFCImage->m_Image;
}

//获取对象
CImage * CGameImageHelper::GetImage()
{
	if (IsOpenHandle()==false) return NULL;
	return &m_pAFCImage->m_Image;
}

//获取对象
CBitmap * CGameImageHelper::GetBitmap()
{
	if (IsOpenHandle()==false) return NULL;
	return CBitmap::FromHandle(m_pAFCImage->m_Image);
}

/*******************************************************************************************************/

//构造函数
CGameImage::CGameImage(HINSTANCE hResourceDLL, UINT uResourceID, TCHAR * szFileName, bool bLock)
{
	m_uLock=0;
	m_bAutoLock=bLock;
	m_strFileName=szFileName;
	m_uResourceID=uResourceID;
	m_hResourceDLL=hResourceDLL;
	if (m_bAutoLock==true) LockResource();
}

//析构函数
CGameImage::~CGameImage()
{
}

//附加对象
bool CGameImage::Attach(CGameImage * pImageObject, bool bLock)
{
	if (m_uLock==0)
	{
		m_bAutoLock=bLock;
		m_uResourceID=pImageObject->m_uResourceID;
		m_hResourceDLL=pImageObject->m_hResourceDLL;
		m_strFileName=pImageObject->m_strFileName;
		if (m_bAutoLock) LockResource();
		return true;
	}
	return false;
}

//分离对象
bool CGameImage::Detach()
{
	if (IsNull()==false)
	{
		m_uResourceID=0L;
		m_hResourceDLL=NULL;
		m_strFileName.Empty();
		if (m_bAutoLock==true) 
		{
			m_bAutoLock=false;
			FreeResource();
		}
		return true;
	}
	return false;
}

//增加引用
UINT CGameImage::LockResource()
{
	if (m_uLock==0)
	{
		if (m_hResourceDLL!=NULL) m_Image.LoadFromResource(m_hResourceDLL,m_uResourceID);
		else 
		{
#ifndef BZ_ZLIB
			m_Image.Load(m_strFileName);
#else
			CUnZipRes unZipRes;
			IStream * pStream=NULL;
			///DebugPrintf("LockResource() [m_strFileName =%s][%d行]",m_strFileName,__LINE__);
			char * pchar=m_strFileName.GetBuffer(m_strFileName.GetLength()+1);
			pStream=unZipRes.LoadFileResFromZip(pchar);
			if(pStream!=NULL)
			{
				m_Image.Load(pStream);
			}else
			{
				m_Image.Load("");
			}
			if(pStream!=NULL)
			{
				
				pStream->Release();
				pStream=NULL;
			}
			
#endif
		}

	}
	return ++m_uLock;
}

//减少引用
UINT CGameImage::FreeResource()
{
	if (m_uLock>0) m_uLock--;
	if (m_uLock==0) m_Image.Destroy();
	return m_uLock;
}

//是否空
bool CGameImage::IsNull()
{
	return ((m_hResourceDLL==NULL)&&(m_strFileName.IsEmpty()==true));
}

//设置加载参数
bool CGameImage::SetLoadInfo(TCHAR * szFileName, bool bLock)
{
	Detach();
	m_bAutoLock=bLock;
	m_strFileName=szFileName;
	if (m_bAutoLock==true) 
		LockResource();
	return true;
}

//设置加载参数
bool CGameImage::SetLoadInfo(HINSTANCE hResourceDLL, UINT uResourceID, bool bLock)
{
	Detach();
	m_bAutoLock=bLock;
	m_uResourceID=uResourceID;
	m_hResourceDLL=hResourceDLL;
	if (m_bAutoLock==true) LockResource();
	return true;
}

//绘图函数
BOOL CGameImage::BitBlt(HDC hDestDC, int xDest, int yDest, DWORD dwROP)
{
	if(m_Image.IsNull())
		return FALSE;
	else
		return m_Image.BitBlt(hDestDC,xDest,yDest,dwROP);
}

//绘图函数
BOOL CGameImage::BitBlt(HDC hDestDC, const POINT & pointDest, DWORD dwROP)
{
	if(m_Image.IsNull())
		return FALSE;
	else
		return m_Image.BitBlt(hDestDC,pointDest,dwROP);
}

//绘图函数
BOOL CGameImage::BitBlt(HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, int xSrc, int ySrc, DWORD dwROP)
{
	if(m_Image.IsNull())
		return FALSE;
	else
		return m_Image.BitBlt(hDestDC,xDest,yDest,nDestWidth,nDestHeight,xSrc,ySrc,dwROP);
}

//绘图函数
BOOL CGameImage::BitBlt(HDC hDestDC,const RECT & rectDest, const POINT & pointSrc, DWORD dwROP)
{
	if(m_Image.IsNull())
		return FALSE;
	else
		return m_Image.BitBlt(hDestDC,rectDest,pointSrc,dwROP);
}

//绘图函数
BOOL CGameImage::StretchBlt(HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, DWORD dwROP)
{
	if(m_Image.IsNull())
		return FALSE;
	else
		return m_Image.StretchBlt(hDestDC,xDest,yDest,nDestWidth,nDestHeight,dwROP);
}

//绘图函数
BOOL CGameImage::StretchBlt(HDC hDestDC, const RECT& rectDest, DWORD dwROP)
{
	if(m_Image.IsNull())
		return FALSE;
	else
		return m_Image.StretchBlt(hDestDC,rectDest,dwROP);
}

//绘图函数
BOOL CGameImage::StretchBlt(HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, DWORD dwROP)
{
	if(m_Image.IsNull())
		return FALSE;
	else
		return m_Image.StretchBlt(hDestDC,xDest,yDest,nDestWidth,nDestHeight,xSrc,ySrc,nSrcWidth,nSrcHeight,dwROP);
}

//绘图函数
BOOL CGameImage::StretchBlt(HDC hDestDC, const RECT& rectDest, const RECT& rectSrc, DWORD dwROP)
{
	if(m_Image.IsNull())
		return FALSE;
	else
		return m_Image.StretchBlt(hDestDC,rectDest,rectSrc,dwROP);
}

/*******************************************************************************************************/
