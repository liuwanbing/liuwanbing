#ifndef WRITELOG_H
#define WRITELOG_H
#include<stdio.h>
//写以error开头的错误日志
_declspec(dllexport) void WriteError(TCHAR *pMessage,int iRoomID = 0,int iSmallRoomID = 0);
//写正常字串
_declspec(dllexport)  void WriteStr(TCHAR *str,int iRoomID = 0,int iSmallRoomID = 0);
//删除日志
_declspec(dllexport)  void Dellog(int iRoomID = 0,int iSmallRoomID = 0);

_declspec(dllexport)  void WriteStr(int iValue);

_declspec(dllexport)  void WriteStr(long int iValue);
#endif