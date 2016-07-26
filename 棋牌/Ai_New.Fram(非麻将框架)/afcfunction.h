#pragma once

#ifndef CAFCFUNCTION_HEAD_FILE
#define CAFCFUNCTION_HEAD_FILE

//#include "PublicDefine.h"

//AFC 函数
	////显示信息框
	//EXT_CLASS UINT AFCMessage(UINT uResourceID, HINSTANCE hInstance, const TCHAR * const szTitle, UINT uType, CWnd * pParentWnd);
	////显示信息框
	//EXT_CLASS UINT AFCMessage(const TCHAR * const szText, const TCHAR * const szTitle, UINT uType, CWnd * pParentWnd);
	////播放声音
	//EXT_CLASS bool AFCPlaySound(HINSTANCE hInstance, TCHAR * szResourceName);
	////位图转换到区域
	//EXT_CLASS HRGN AFCBmpToRgn(HBITMAP hBmp, COLORREF cTransparentColor, COLORREF cTolerance);
	////绘画透明图
	//EXT_CLASS bool AFCStretchBlt(CDC * pDesDC, int iXDes, int iYDes, int iDesWidth, int iDesHeight, CDC * pScrDC, int iXScr, int iYScr, int iScrWidth, int iScrHeight, COLORREF crColor);
	////绘画透明图
	//EXT_CLASS bool AFCStretchImage(CDC * pDesDC, int iXDes, int iYDes, int iDesWidth, int iDesHeight,HBITMAP hBitBmp, int iXScr, int iYScr, int iScrWidth, int iScrHeight, COLORREF crColor);
	////bmp转换成区域
	//EXT_CLASS HRGN CreateRegionFromBitmap(HBITMAP hBitmap, COLORREF transparentColor);
	////显示半透明不规则图片
	//EXT_CLASS bool AFCAlphaBlend(HDC destdc, int dx, int dy, int cx, int cy, HDC srcdc, int sx, int sy, COLORREF crTransparent, int SourceConstantAlpha);
	////截图
	//EXT_CLASS void SaveScreen();
	////拷背屏
	//EXT_CLASS HBITMAP CopyScreenToBitmap() ;
	////保存文件
	//EXT_CLASS int SaveBitmapToFile(HBITMAP   hBitmap  ,LPTSTR   lpFileName);
	////根据字符串长度来载取字符
	//EXT_CLASS bool GetChar(TCHAR * src, TCHAR * dest, int count);
	//写以error开头的错误日志
//	EXT_CLASS void WriteError(TCHAR *pMessage,int iRoomID = 0,int iSmallRoomID = 0);
	//写正常字串
	EXT_CLASS void WriteStr(TCHAR *str,int iRoomID = 0,int iSmallRoomID = 0);
	//写正常字串
	EXT_CLASS void WriteStr(CString str,int iRoomID = 0,int iSmallRoomID = 0);
	//删除日志
//	EXT_CLASS void Dellog(int iRoomID = 0,int iSmallRoomID = 0);
#endif