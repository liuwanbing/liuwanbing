#pragma once

#include <Windows.h>
#include <tchar.h>

#ifdef UI_EXPORTS
#define UI_API __declspec(dllexport)
#else
#define UI_API __declspec(dllimport)
#endif UI_EXPORTS

interface IUserInterface;
struct TUIMessage;

class XUIBase
{
public:
	XUIBase(void){};
	virtual ~XUIBase(void){};
	virtual int Run(void)=0;
	virtual int Initial(HWND hwnd, const TCHAR *szAppPath=NULL)=0;
	virtual void OnInputEvent(UINT message, WPARAM wParam, LPARAM lParam)=0;
	virtual void ClearMessage(TUIMessage *p) = 0;

};
extern UI_API bool CreateUI(XUIBase **ppUIBase, IUserInterface **ppIUI);