#pragma once

class ID2DInterface
{
	//创建函数
public:
	//载入资源
	virtual LPCTSTR LaodImage( IN CWnd* pWnd, IN LPCTSTR strPath, OUT TCHAR* szOutInfo ) = NULL;
	//载入资源
	virtual LPCTSTR LaodImage( IN CWnd* pWnd, IN HINSTANCE hinstance, IN LPCTSTR strNmae,  IN LPCTSTR strType, OUT TCHAR* szOutInfo ) = NULL;
	//载入资源
	virtual LPCTSTR LaodImage( IN CWnd* pWnd, IN HINSTANCE hinstance, IN UINT uResID, OUT TCHAR* szOutInfo ) = NULL;
	//载入资源
	virtual LPCTSTR LaodImage( IN CWnd* pWnd, IN HINSTANCE hinstance, IN LPCTSTR strResID, OUT TCHAR* szOutInfo ) = NULL;

	//主体函数
public:
	//开始绘画
	virtual void BeginDraw( IN CWnd* pWnd ) = NULL;
	//结束绘画
	virtual void EndDraw( IN CWnd* pWnd ) = NULL;

	//操作函数
public:
	//释放接口
	virtual void Release() = NULL;
	//删除资源
	virtual void DeleteImage( IN CWnd* pWnd, IN LPCTSTR strInfo) = NULL;
	//获取宽度
	virtual UINT GetWidth( IN CWnd* pWnd, IN LPCTSTR strInfo ) = NULL;
	//获取高度
	virtual UINT GetHeight( IN CWnd* pWnd, IN LPCTSTR strInfo ) = NULL;

	//绘画函数
public:
	//绘画函数
	virtual bool DrawImage( IN CWnd* pWnd, IN LPCTSTR strInfo, IN INT nXDest, IN INT nYDest, IN BYTE cbAlpha) = NULL;
	//绘画函数
	virtual bool DrawImage( IN CWnd* pWnd, IN LPCTSTR strInfo, IN INT nXDest, IN INT nYDest, IN INT nDestWidth, INT nDestHeight, IN INT nXScr, IN INT nYSrc, IN BYTE cbAlpha) = NULL;
	//绘画函数
	virtual bool DrawImage( IN CWnd* pWnd, IN LPCTSTR strInfo, IN INT nXDest, IN INT nYDest, IN INT nDestWidth, IN INT nDestHeight, IN INT nXScr, IN INT nYSrc, IN INT nSrcWidth, IN INT nSrcHeight, IN BYTE cbAlpha ) = NULL;
	//绘画函数
	virtual bool DrawImage( IN CWnd* pWnd, IN LPCTSTR strInfo, IN INT nXDest, IN INT nYDest, IN INT nDestWidth, IN INT nDestHeight, IN INT nXScr, IN INT nYSrc, IN INT nSrcWidth, IN INT nSrcHeight, COLORREF colorTransparent, IN BYTE cbAlpha ) = NULL;

	//文字函数
public:
	//创建文字
	virtual void* CreateFont( IN LPCTSTR strInfo, IN int nWidth, IN int nWeight) = NULL;
	//绘画文字
	virtual bool DrawText( IN CWnd* pWnd, IN void* pFont, IN LPCTSTR strInfo, IN PRECT rect, IN COLORREF colorref, IN UINT uFormat) = NULL;
	//绘画文字
	virtual bool DrawText( IN CWnd* pWnd, IN void* pFont, IN LPCTSTR strInfo, IN int nXPos, IN int nYPos, IN COLORREF colorref, IN UINT uFormat) = NULL;
	//绘画文字
	virtual bool DrawText( IN CWnd* pWnd, IN int nWidth, IN int nWeight, IN LPCTSTR strInfo, IN PRECT rect, IN COLORREF colorref, IN UINT uFormat) = NULL;
	//绘画文字
	virtual bool DrawText( IN CWnd* pWnd, IN int nWidth, IN int nWeight, IN LPCTSTR strInfo, IN int nXPos, int nYPos, IN COLORREF colorref, IN UINT uFormat) = NULL;
	//删除文字
	virtual bool DeleteFont( IN void* pVoid) = NULL;

	//绘画函数
public:	
	//自定义绘画函数
	virtual bool DrawCustomImage( IN CWnd* pWnd, IN INT nXDest, IN INT nYDest, IN INT nDestWidth, IN INT nDestHeight, IN DWORD* pImageData, IN INT nImageWidth, IN INT nImageHeight, IN BYTE cbAlpha ) = NULL;

};