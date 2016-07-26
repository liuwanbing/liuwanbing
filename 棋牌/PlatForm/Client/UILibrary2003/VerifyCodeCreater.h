#pragma once

#ifndef CVERIFYCODECREATER_HEAD_FILE
#define CVERIFYCODECREATER_HEAD_FILE

class EXT_CLASS CVerifyCodeCreater
{
public:
	CVerifyCodeCreater();
	virtual ~CVerifyCodeCreater();

public:
	static CVerifyCodeCreater* GetInstance();
	void SetParent(const HWND& hHwnd);
	void ReCreateBMP(int iWidth,int iHeight,COLORREF crBackGround);
	void Draw(int x,int y);
	bool IsVerify(LPCTSTR lpszInput);

private:
	void AddBackGroundPicture();
	void BackgroundDisturb();
	void FontDisturb(const HDC &dc,const CString & strCode);
	void PixelDisturb(const HDC &dc,const HBITMAP &hBitmap);
	void PixelDisturbOnFont(const HDC &dc,const HBITMAP &hBitmap);

	void BackGroundClear(DWORD color);

	bool CreateTexture(char* FileName,const HWND &m_hWnd);

	//产生随机验证码算法
	void RandomCode(BYTE* pChar,int iNums);

private:

	//图片的高各宽
	int m_iWidth;
	int m_iHeight;

	//图片的宽和高信息
	int m_textrueWidth;
	int m_textrueHeight;

	//图片的颜色等信息
	DWORD* m_pTextureData;

	//设计一个相应大小的背景,随机填充颜色进行干扰
	DWORD* m_backPixel;

	//生成的bitmap的相关信息
	BITMAPINFOHEADER bih;

	LOGFONT logFont;  
	HFONT hFont;

	//字体数组
	CString m_FontArray[23];

	//父窗口的DC
	HDC m_dcParentWnd;

	//验证码图片缓存DC
	HDC m_dcCodeMem;

private:

    static CVerifyCodeCreater* m_pInstance;

	//随机字符串
	unsigned char m_szCode[5];
};

#endif // !defined(CVERIFYCODECREATER_HEAD_FILE)
