#ifndef PNG_IMAGE_EX_HEAD_FILE
#define PNG_IMAGE_EX_HEAD_FILE

#pragma once

#include "WHImageHead.h"

//////////////////////////////////////////////////////////////////////////////////

//图片对象
class WH_IMAGE_CLASS CPngImageEx
{
	//变量定义
protected:
	Image *							m_pImage;							//图片对象
	CWnd *							m_pWnd;								//图片显示窗口
	TCHAR							m_strImageName[MAX_PATH];			//图片名称

	//函数定义
public:
	//构造函数
	CPngImageEx();
	//析构函数
	virtual ~CPngImageEx();

	//状态函数
public:
	//是否加载
	bool IsNull();
	//获取宽度
	INT GetWidth();
	//获取高度
	INT GetHeight();

	//管理函数
public:
	//销毁图片
	bool DestroyImage();
	//加载图片
	bool LoadImage(CWnd* pWnd, LPCTSTR pszFileName);
	//加载图片
	bool LoadImage(CWnd* pWnd, HINSTANCE hInstance, LPCTSTR pszResourceName);

	//绘画函数
public:
	//绘画图像
	bool DrawImage(CDC * pDC, INT nXPos, INT nYPos);
	//绘画图像
	bool DrawImage(CDC * pDC, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXScr, INT nYSrc);
	//绘画图像
	bool DrawImage(CDC * pDC, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXScr, INT nYSrc, INT nSrcWidth, INT nSrcHeight);

	//透明绘画
public:
	//混合绘画
	bool AlphaDrawImage(CDC * pDestDC, INT xDest, INT yDest, BYTE cbAlphaDepth);
	//混合绘画
	bool AlphaDrawImage(CDC * pDestDC, INT xDest, INT yDest, INT cxDest, INT cyDest, INT xSrc, INT ySrc, BYTE cbAlphaDepth);
	//混合绘画
	bool AlphaDrawImage(CDC * pDestDC, INT xDest, INT yDest, INT cxDest, INT cyDest, INT xSrc, INT ySrc, INT cxSrc, INT cySrc, BYTE cbAlphaDepth);
};

//////////////////////////////////////////////////////////////////////////////////

#endif