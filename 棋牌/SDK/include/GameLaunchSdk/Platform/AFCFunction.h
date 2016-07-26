#pragma once

#ifndef CAFCFUNCTION_HEAD_FILE
#define CAFCFUNCTION_HEAD_FILE

#include "PublicDefine.h"
enum EBUBLEPOSITION
{
	BUBLE_ON_BANK_OK_BTN
};

//AFC 函数
	//显示信息框
//	EXT_CLASS UINT AFCMessage(UINT uResourceID, HINSTANCE hInstance, const TCHAR * const szTitle, UINT uType, CWnd * pParentWnd);
	//显示信息框
//	EXT_CLASS UINT AFCMessage(const TCHAR * const szText, const TCHAR * const szTitle, UINT uType, CWnd * pParentWnd);
	//播放声音
	EXT_CLASS bool AFCPlaySound(HINSTANCE hInstance, TCHAR * szResourceName);
	//位图转换到区域
	EXT_CLASS HRGN AFCBmpToRgn(HBITMAP hBmp, COLORREF cTransparentColor, COLORREF cTolerance);
	//绘画透明图
	EXT_CLASS bool AFCStretchBlt(CDC * pDesDC, int iXDes, int iYDes, int iDesWidth, int iDesHeight, CDC * pScrDC, int iXScr, int iYScr, int iScrWidth, int iScrHeight, COLORREF crColor);
	//绘画透明图
	EXT_CLASS bool AFCStretchImage(CDC * pDesDC, int iXDes, int iYDes, int iDesWidth, int iDesHeight,HBITMAP hBitBmp, int iXScr, int iYScr, int iScrWidth, int iScrHeight, COLORREF crColor);
	//bmp转换成区域
	EXT_CLASS HRGN CreateRegionFromBitmap(HBITMAP hBitmap, COLORREF transparentColor);
	//显示半透明不规则图片
	EXT_CLASS bool AFCAlphaBlend(HDC destdc, int dx, int dy, int cx, int cy, HDC srcdc, int sx, int sy, COLORREF crTransparent, int SourceConstantAlpha);
	//截图
	EXT_CLASS void SaveScreen();
	//拷背屏
	EXT_CLASS HBITMAP CopyScreenToBitmap() ;
	//保存文件
	EXT_CLASS int SaveBitmapToFile(HBITMAP   hBitmap  ,LPTSTR   lpFileName);
	//根据字符串长度来载取字符
	EXT_CLASS bool GetChar(TCHAR * src, TCHAR * dest, int count);
	//写以error开头的错误日志
	EXT_CLASS void WriteError(TCHAR *pMessage,int iRoomID = 0,int iSmallRoomID = 0);
	//写正常字串
	EXT_CLASS void WriteStr(TCHAR *str,int iRoomID = 0,int iSmallRoomID = 0);
	EXT_CLASS void WriteStr(int iValue);
	EXT_CLASS void WriteStr(long int iValue);
	EXT_CLASS void WriteStr(DWORD iValue);
	//删除日志
	EXT_CLASS void Dellog(int iRoomID = 0,int iSmallRoomID = 0);
	//下载函数
	EXT_CLASS int InternetGetFile(TCHAR szUrl[], TCHAR szFileName[],CString &filename);
	//异常处理
	EXT_CLASS LONG WINAPI ExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo);

	//wushuqun 2008.9.24
	//自定义消息框
	UINT EXT_CLASS AFCMessageBox(CString strInfo,CString strTitel = "系统提示",UINT style = 500,CWnd* pParent = NULL);

	//消息框,DirectUI版

	/// 提示
	/// @param pToCtrl 在某一控件上提示
	/// @param hParentWnd 父窗口句柄
	/// @param style 样式，与WinAPI的MessageBox提供的样式一至
	/// @param lpTitle 消息框的标题
	/// @param lpMainText 消息框主文本，支持文本的格式化
	/// @return UINT 返回询问结果，与WinAPI的MessageBox提供的返回值一至
	/// @author JianGuankun,2012.7.25
	///
	EXT_CLASS UINT DUIMessageBox(HWND hParentWnd,UINT style,LPCTSTR lpTitle,LPCTSTR lpMainText,...);

	//只有一个OK按钮，而且固定标题
	EXT_CLASS UINT DUIOkeyMsgBox(HWND hParentWnd,LPCTSTR lpMainText,...);

	//冒泡提示
	EXT_CLASS void ShowBubleMessageTip(const HWND hParentWnd,const CString& strMsg,EBUBLEPOSITION pos);
#endif