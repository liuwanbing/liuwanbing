#pragma once
#ifndef IGIF_HEAD_H
#define IGIF_HEAD_H
class EXT_CLASS IGif
{
protected:
	IGif(void);

	virtual ~IGif(void);

public:
	// 销毁自己；
	virtual ULONG Release() = 0;

public:
	// 作用		：	从文件装载Gif；
	// 参数1	：	文件名；
	// 返回		：	成功返回NOERROR,失败返回E_FAIL;
	virtual HRESULT LoadFromFile(LPCTSTR lpstrFileName) = 0;

	// 作用		：	从文件装载Gif；
	// 参数1	：	Gif在资源中的目录名.比如WAV装载后表现在"WAVE"中,则输入WAVE；
	// 参数2	：	资源名；
	// 返回		：	成功返回NOERROR,失败返回E_FAIL;
	virtual HRESULT LoadFromRes(LPCTSTR lpstrResDirectory, LPCTSTR lpstrResName) = 0;

	// 作用		：	不伸缩地绘制gif的某一桢；
	// 参数1	：	gif的桢ID；
	// 参数2	：	目标DC指针；
	// 参数3	：	目标位置左上角X轴坐标；
	// 参数4	：	目标位置左上角Y轴坐标；
	// 返回		：	成功返回NOERROR,失败返回E_FAIL;
	virtual HRESULT Bitblt(DWORD dwFrameId, CDC *pDestDC, int ixDest, int iyDest) = 0;


	// 作用		：	不伸缩地绘制gif的某一桢；
	// 参数1	：	已经存在的时间；
	// 参数2	：	目标DC指针；
	// 参数3	：	目标位置左上角X轴坐标；
	// 参数4	：	目标位置左上角Y轴坐标；
	// 返回		：	成功返回NOERROR,失败返回E_FAIL;
	virtual HRESULT Bitblt(CDC *pDestDC, int ixDest, int iyDest) = 0;

	// 作用		：	伸缩地绘制gif的某一桢；
	// 参数1	：	gif的桢ID；
	// 参数2	：	目标DC指针；
	// 参数3	：	目标位置左上角X轴坐标；
	// 参数4	：	目标位置左上角Y轴坐标；
	// 参数5	：	伸缩后的宽；
	// 参数6	：	伸缩后的高；
	// 返回		：	成功返回NOERROR,失败返回E_FAIL;
	virtual HRESULT StretchBlt(DWORD dwFrameId, CDC *pDestDC, int ixDest, int iyDest, int iWidth, int iHeight) = 0;

	virtual DWORD GetImageWidth() = 0;

	virtual DWORD GetImageHeight() = 0;

	virtual DWORD GetFrameCount() = 0;
	
	// 作用		：	gif动画重置；
	virtual void Reset() = 0;
};

#endif
//class IGif
//{
//protected:
//	IGif(void);
//
//	virtual ~IGif(void);
//
//public:
//	virtual ULONG Release() = 0;
//
//public:
//	// 作用		：	从文件装载Gif；
//	// 参数1	：	文件名；
//	// 返回		：	成功返回NOERROR,失败返回E_FAIL;
//	virtual HRESULT LoadFromFile(LPCSTR lpstrFileName) = 0;
//
//	// 作用		：	从文件装载Gif；
//	// 参数1	：	文件名；
//	// 返回		：	成功返回NOERROR,失败返回E_FAIL;
//	virtual HRESULT LoadFromFile(LPCWSTR lpstrFileName) = 0;
//
//	// 作用		：	从文件装载Gif；
//	// 参数1	：	Gif在资源中的目录名.比如WAV装载后表现在"WAVE"中,则输入WAVE；
//	// 参数2	：	资源名；
//	// 返回		：	成功返回NOERROR,失败返回E_FAIL;
//	virtual HRESULT LoadFromRes(LPCSTR lpstrResDirectory, LPCTSTR lpstrResName) = 0;
//
//	// 作用		：	从文件装载Gif；
//	// 参数1	：	Gif在资源中的目录名.比如WAV装载后表现在"WAVE"中,则输入WAVE；
//	// 参数2	：	资源名；
//	// 返回		：	成功返回NOERROR,失败返回E_FAIL;
//	virtual HRESULT LoadFromRes(LPCWSTR lpstrResDirectory, LPCTSTR lpstrResName) = 0;
//
//	// 作用		：	不伸缩地绘制gif的某一桢；
//	// 参数1	：	gif的桢ID；
//	// 参数2	：	目标DC指针；
//	// 参数3	：	目标位置左上角X轴坐标；
//	// 参数4	：	目标位置左上角Y轴坐标；
//	// 返回		：	成功返回NOERROR,失败返回E_FAIL;
//	virtual HRESULT Bitblt(DWORD dwFrameId, CDC *pDestDC, int ixDest, int iyDest) = 0;
//
//	// 作用		：	伸缩地绘制gif的某一桢；
//	// 参数1	：	gif的桢ID；
//	// 参数2	：	目标DC指针；
//	// 参数3	：	目标位置左上角X轴坐标；
//	// 参数4	：	目标位置左上角Y轴坐标；
//	// 参数5	：	伸缩后的宽；
//	// 参数6	：	伸缩后的高；
//	// 返回		：	成功返回NOERROR,失败返回E_FAIL;
//	virtual HRESULT StretchBlt(DWORD dwFrameId, CDC *pDestDC, int ixDest, int iyDest, int iWidth, int iHeight) = 0;
//
//	virtual DWORD GetImageWidth() = 0;
//
//	virtual DWORD GetImageHeight() = 0;
//
//	virtual DWORD GetFrameCount() = 0;
//};
