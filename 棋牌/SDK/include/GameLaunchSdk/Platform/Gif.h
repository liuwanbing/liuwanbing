#pragma once
#ifndef GIF_HEAD_H
#define GIF_HEAD_H
#include <gdiplus.h>
#pragma comment (lib, "gdiplus.lib")

#include "IGif.h"
#include "animator.h"
#pragma comment (lib, "Packager.lib")

class CObjectFactory;

class EXT_CLASS CGif : public IGif
{
	friend class CObjectFactory;

protected:
	CGif(void);

	virtual ~CGif(void);

public:
	static HRESULT Initialize();

	static HRESULT Uninitialize();

public:

	virtual ULONG Release();

	virtual HRESULT LoadFromFile(LPCTSTR lpstrFileName) ;

	virtual HRESULT LoadFromRes(LPCTSTR lpstrResDirectory, LPCTSTR lpstrResName);

	virtual HRESULT Bitblt(DWORD dwFrameIndex, CDC *pDestDC, int ixDest, int iyDest);

	virtual HRESULT Bitblt(CDC *pDestDC, int ixDest, int iyDest);

	virtual HRESULT StretchBlt(DWORD dwFrameIndex, CDC *pDestDC, int ixDest, int iyDest, int iWidth, int iHeight) ;

	virtual DWORD GetImageWidth();

	virtual DWORD GetImageHeight();

	virtual DWORD GetFrameCount();

	virtual void Reset();

private:
	Gdiplus::Image		*m_pImage;			//当前帧图片
	std::vector<Gdiplus::Image *> m_pImageList;	//帧的指针列表
	UINT				m_uiFrameCount;		//帧的数量
	UINT				*m_pFrameDelay;		//帧的延时
	UINT				m_uiCurSelFrameID;	//当前帧ID
	DWORD				m_LastTimeSwitchFrame;	//上次切换帧的时间

	static ULONG_PTR    m_GdiplusToken;
	static int			m_iInitializeCount;


	int                             m_nWidth;
	int                             m_nHeight;

	Animator						m_animator;                        // 用于读取动画文件

	//调试用的字符串
	CString strDebugView;

private:
	BOOL DrawImage(CDC * pDC, POINT * pPoint = NULL, int srcx = NULL, int srcy = NULL, int srcwidth = NULL, int srcheight = NULL, float * pfClrAttr = NULL);
	BOOL DrawImage(CDC * pDC, int xDest, int yDest, int nDestWidth, int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, float fAlpha = -1.0);
	BOOL DrawImage(CDC * pDC, int xDest, int yDest, float fAlpha = -1.0);
	Gdiplus::Image* GetFrameData(int index);
	BOOL LoadImageListFromAnimator();
	BOOL ClearImageList();
	BOOL LoadImageFromMemory(unsigned char *pData,unsigned long length);

public:
	bool IsNull();
};
#endif
//
//class CGif : public IGif
//{
//	friend class CObjectFactory;
//
//protected:
//	CGif(void);
//
//	virtual ~CGif(void);
//
//public:
//	static HRESULT Initialize();
//
//	static HRESULT Uninitialize();
//
//public:
//
//	virtual ULONG Release();
//
//	virtual HRESULT LoadFromFile(LPCSTR lpstrFileName) ;
//
//	virtual HRESULT LoadFromFile(LPCWSTR lpstrFileName) ;
//
//	virtual HRESULT LoadFromRes(LPCSTR lpstrResDirectory, LPCTSTR lpstrResName);
//
//	virtual HRESULT LoadFromRes(LPCWSTR lpstrResDirectory, LPCTSTR lpstrResName);
//
//	virtual HRESULT Bitblt(DWORD dwFrameIndex, CDC *pDestDC, int ixDest, int iyDest) ;
//
//	virtual HRESULT StretchBlt(DWORD dwFrameIndex, CDC *pDestDC, int ixDest, int iyDest, int iWidth, int iHeight) ;
//
//	virtual DWORD GetImageWidth();
//
//	virtual DWORD GetImageHeight();
//
//	virtual DWORD GetFrameCount();
//
//private:
//	Gdiplus::Image		*m_pImage;
//
//
//	static ULONG_PTR    m_GdiplusToken;
//	static int			m_iInitializeCount;
//};
