#include "StdAfx.h"
#include "ResUtility.h"

CString GetFileExtension(LPBYTE lpDate)
{
	CString FileExt=_T("");
	if (memcmp(lpDate,PngHeader,sizeof(PngHeader))==0)
		FileExt = _T(".png");
	else if (memcmp(lpDate,BmpHeader,sizeof(BmpHeader))==0)
		FileExt = _T(".bmp");
	else if (memcmp(lpDate,GifHeader,sizeof(GifHeader))==0)
		FileExt = _T(".gif");
	else if (memcmp(lpDate,JpgHeader,sizeof(JpgHeader))==0)
		FileExt = _T(".jpg");
	else if (memcmp(lpDate,WaveHeader,sizeof(WaveHeader))==0)
		FileExt = _T(".wav");
	else if (memcmp(lpDate,Mp3Header,sizeof(Mp3Header))==0)
		FileExt = _T(".mp3");
	else if (memcmp(lpDate,Mp3StreamHeader,sizeof(Mp3StreamHeader))==0)
		FileExt = _T(".mp3");
	return FileExt;
}
