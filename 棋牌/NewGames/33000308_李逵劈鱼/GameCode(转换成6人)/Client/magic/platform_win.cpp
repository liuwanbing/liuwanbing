//-----------------------------------------------------------------------------
// File: platform_win.cpp
//
// Copyright (c) Astralax. All rights reserved.
// Author: Sedov Alexey
//-----------------------------------------------------------------------------

#include "platform_win.h"
#include "mp_string.h"

MP_Platform_WINDOWS::MP_Platform_WINDOWS()
{
}

MP_Platform_WINDOWS::~MP_Platform_WINDOWS()
{
}

// eng: Returns time in milliseconds
// rus: Возвращает время ?милисекундах
unsigned long MP_Platform_WINDOWS::GetTick()
{
	return GetTickCount();
}

// eng: Returns path to folder with emitters
// rus: Возвращает путь ?папк??ptc-файлам?
const wchar_t* MP_Platform_WINDOWS::GetPathToPTC()
{
	if (ptc_path.empty())
	{
		wchar_t path[MAX_PATH];
		
		#ifndef TEST
		GetModuleFileNameW(NULL, path, MAX_PATH); 
		int len=wcslen(path);
		for (int i=len-1;i>=0;i--)
		{
			if (path[i]=='\\')
			{
				path[i]=0;
				break;
			}
		}
		#else
		GetCurrentDirectoryW(MAX_PATH, path);
		#endif

		ptc_path=path;
		ptc_path+='\\';
    ptc_path += L"lkpy\\particle\\";
	}

	return ptc_path.w_str();
}

// eng: Returns path to folder with textures
// rus: Возвращает путь ?папк??текстурами
const wchar_t* MP_Platform_WINDOWS::GetPathToTexture()
{
	if (texture_path.empty())
	{
		texture_path=GetPathToPTC();
		texture_path+="textures\\";
	}

	return texture_path.w_str();
}

// eng: Returns path to folder which could be used by wrapper to store temporary files
// rus: Возвращает путь ?временно?папк?
const wchar_t* MP_Platform_WINDOWS::GetPathToTemp()
{
	if (temp_path.empty())
	{
		wchar_t path[MAX_PATH];
		GetTempPathW(MAX_PATH, path);
		temp_path=path;
	}

	return temp_path.w_str();
}

// eng: Finds the first ptc-file in emitters folder
// rus: Поис?первог?ptc-файл??папк??эмиттерами
const wchar_t* MP_Platform_WINDOWS::GetFirstFile()
{
	MP_StringW mask=GetPathToPTC();
	mask+="*.ptc";
	hFindFile=FindFirstFileW(mask.w_str(),&fd);
	if (hFindFile!=INVALID_HANDLE_VALUE)
	{
		return fd.cFileName;
	}
	FindClose(hFindFile);
	return NULL;
}

// eng: Converts wchar_t string into utf8
// rus: Конвертирует строку типа wchar_t ?строку типа utf8
const wchar_t* MP_Platform_WINDOWS::GetNextFile()
{
	if (FindNextFileW(hFindFile,&fd))
		return fd.cFileName;
	FindClose(hFindFile);
	return NULL;
}

// eng: Converts wchar_t string into utf8
// rus: Конвертирует строку типа wchar_t ?строку типа utf8
const char* MP_Platform_WINDOWS::wchar_to_utf8(const wchar_t* str)
{
	const char* str8=Magic_UTF16to8((const short*)str);
	return str8;
}

// eng: Converts utf8 string into wchar_t
// rus: Конвертирует строку типа utf8 ?строку типа wchar_t
const wchar_t* MP_Platform_WINDOWS::utf8_to_wchar(const char* str)
{
	const short* str16=Magic_UTF8to16(str);
	return (const wchar_t*)str16;
}