// CreateValidateCode.cpp: implementation of the CVerifyCodeCreater class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "VerifyCodeCreater.h"
#include <mmsystem.h>


CVerifyCodeCreater* CVerifyCodeCreater::m_pInstance = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVerifyCodeCreater::CVerifyCodeCreater()
{

	m_dcParentWnd = NULL;
	m_dcCodeMem = NULL;

	m_iWidth = 0;
	m_iHeight = 0;
	m_pTextureData = NULL;

	m_backPixel = NULL;

	//干扰码的图片基本信息初始化
	bih.biBitCount=32;//24; 
	bih.biClrImportant=0;  
	bih.biClrUsed=0; 
	bih.biCompression=BI_RGB; 
	bih.biHeight=m_iHeight; 
	bih.biPlanes=1; 
	bih.biSize=sizeof(BITMAPINFOHEADER); 
	bih.biSizeImage=m_iWidth*m_iHeight; 
	bih.biWidth=m_iWidth; 
	bih.biXPelsPerMeter=0; 
	bih.biYPelsPerMeter=0; 
	bih.biSizeImage = 0;

	//字体的相关信息
	memset(&logFont,   0,   sizeof(LOGFONT));   
	logFont.lfHeight   =  30;     
	logFont.lfWidth    =  20;

	lstrcpy(logFont.lfFaceName,   _T("新宋体"));   


	m_FontArray[0] = "Arial";
	m_FontArray[1] = "Arial Narrow";
	m_FontArray[2] = "Batang";
	m_FontArray[3] = "BatangChe";
	m_FontArray[4] = "Book Antiqua";
	m_FontArray[5] = "Bookman Old Style";
	m_FontArray[6] = "Dotum";
	m_FontArray[7] = "Garamond";
	m_FontArray[8] = "Gulim";
	m_FontArray[9] = "Kartika";
	m_FontArray[10] = "Monotype Corsiva";
	m_FontArray[11] = "System";
	m_FontArray[12] = "方正姚体";
	m_FontArray[13] = "仿宋_GB2312";
	m_FontArray[14] = "黑体";
	m_FontArray[15] = "华文新魏";
	m_FontArray[16] = "华文行楷";
	m_FontArray[17] = "隶书";
	m_FontArray[18] = "楷体_GB2312";
	m_FontArray[19] = "新宋体";
	m_FontArray[20] = "幼圆";
	m_FontArray[21] = "Lucida Console";
	m_FontArray[22] =  "Monotype Corsiva"; 

}

CVerifyCodeCreater::~CVerifyCodeCreater()
{
	if (m_backPixel)
	{
		delete[] m_backPixel;
		m_backPixel= NULL;
	}

	if (m_pTextureData)
	{
		delete[] m_pTextureData;
		m_pTextureData = NULL;
	}
}

//初始化背景颜色
void CVerifyCodeCreater::BackGroundClear(DWORD color)
{
	for (int i=0;i<m_iWidth*m_iHeight;i++)
	{
	  //四个字节，从左往右的信息分别为：rgbReserved,r,g,b
	 m_backPixel[i] = color;//初始化背景色
	}
}

//背景的像素造成其的干扰
void CVerifyCodeCreater::BackgroundDisturb()
{
	srand((unsigned)timeGetTime());//产生随机数种子
	
	DWORD randNum;
	DWORD randNum2;
	//干扰一：每一个像素点重新绘色
   for (int i=0;i<(m_iWidth*m_iHeight);i++)
   {
	  //四个字节，从左往右的信息分别为：rgbReserved,r,g,b
	    randNum = rand()%0x00ffffff;
		randNum2 = rand()%0x00ffffff;
	    m_backPixel[i] =randNum & randNum2;
   }
}

//在背景上注入斑点
void CVerifyCodeCreater::PixelDisturb(const HDC &dc,const HBITMAP &hBitmap)
{
   //干扰二：随机总的像素点是：m_iWidth*m_iHeight=220*70某些点是白色
   DWORD totalPixel = m_iWidth*m_iHeight-1;
   DWORD pixelPos = 1;
   for (DWORD j=0;j<((DWORD)(totalPixel*0.1));j++)
   {
         pixelPos = rand()%totalPixel;
		 m_backPixel[pixelPos] =0x00ffffff;

   }   
}

//在字体上面注入斑点
void CVerifyCodeCreater::PixelDisturbOnFont(const HDC &dc,const HBITMAP &hBitmap)
{
	srand((unsigned)timeGetTime());//产生随机数种子


	//干扰三：随机生成约十个点,然后不断扩散成一个小方块和圈状
	int x_pos = 0;
	int y_pos = 0;
	int r_x_pos = 0;
	int r_y_pos = 0;

	for (int j=0;j<10;j++)
	{
	    x_pos = rand()%(m_iWidth-16);
		y_pos = rand()%(m_iHeight-16);

		for(int i=0;i<70;i++)
		{
			r_x_pos=x_pos + rand()%15;
			r_y_pos =y_pos + rand()%15;
			m_backPixel[r_y_pos*m_iWidth+r_x_pos] =0x00ffffff;
		}
	}   
}


//让字体变化以造成干扰
void CVerifyCodeCreater::FontDisturb(const HDC &dcMem, const CString & strCode)
{
  srand((unsigned)timeGetTime());//产生随机数种子
  //字体随机
  lstrcpy(logFont.lfFaceName,   _T(m_FontArray[rand()%22]));   
 // lstrcpy(logFont.lfFaceName,   _T(m_FontArray[19]));   
    
  //字体大小随机
  logFont.lfHeight   =  15+(rand()%15);     
  logFont.lfWidth    =  15+(rand()%10);
  logFont.lfWeight   =  100*(rand()%7);//Specifies the weight of the font in the range 0 through 1000. For example, 400 is normal and 700 is bold. If this value is zero, a default weight is used. 
  logFont.lfItalic   =   TRUE;
  logFont.lfEscapement = rand()%80;
 }


//创建背景纹理
bool CVerifyCodeCreater::CreateTexture(char* FileName,const HWND &m_hWnd)
{
   HBITMAP BitHandle = (HBITMAP)LoadImage(NULL, FileName, IMAGE_BITMAP, 0,0, LR_LOADFROMFILE);
   if(BitHandle == NULL)
   {
      MessageBox(0, "加载指定的位图文件出错", "程序错误!",
      MB_ICONERROR | MB_SYSTEMMODAL | MB_OK);
   }
    BITMAP bm;
	HDC hdc = GetDC(m_hWnd);
	HDC dc = CreateCompatibleDC(hdc);
	SelectObject(dc, BitHandle);
	GetObject(BitHandle, sizeof(BITMAP), &bm);
    m_textrueWidth=bm.bmWidth;
    m_textrueHeight=bm.bmHeight;

	//这里只是做了一个简单的处理：图片得比内存中的位图大
	if (m_textrueWidth<m_iWidth || m_textrueHeight<m_iHeight )
	{
		MessageBox(NULL,"图片得比内存中的位图小，请选择更大的图片!","",0);
		return false;
	}

    m_pTextureData=new DWORD[m_textrueWidth*m_textrueHeight];
    for (int i=0;i<m_textrueHeight;i++)
	  for (int j=0;j<m_textrueWidth;j++)
	   	   m_pTextureData[i*m_textrueWidth+j]=GetPixel(dc,j,i);	  
  
  // BitBlt(hdc, 0, 0,m_iWidth,m_iHeight , dc, 0,0, SRCCOPY);
   ReleaseDC(m_hWnd, hdc);
   return true;
}

//产生随机验证码算法
void CVerifyCodeCreater::RandomCode(BYTE* pChar,int iNums)
{
	unsigned char szCodeList[33] = {'1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','G','H',
								    'J','K','L','M','N','P','Q','R','S','T','U','V','W','X','Y','Z'},

	::srand(::GetTickCount());

	BYTE temp = 255;
	for(int i = 0; i < 33; ++i)
	{
		temp = ::rand() % (33 - i) + i;
		char a = szCodeList[temp];
		szCodeList[temp] = szCodeList[i];
		szCodeList[i] = a;
	}

	::memcpy(pChar,szCodeList,iNums);
	pChar[iNums] = 0;

	return;
}

void CVerifyCodeCreater::AddBackGroundPicture()
{
	DWORD tempColor,r,g,b;

    for (int i=0;i<m_iHeight;i++)
	{
	   for(int j=0; j<m_iWidth; j++)
	   {
		   tempColor = m_pTextureData[i*m_textrueWidth+j];
		   r = GetRValue(tempColor);   
           g = GetGValue(tempColor);   
           b = GetBValue(tempColor);   
       	   m_backPixel[i*m_iWidth+j] = RGB(b,g,r);
	   } 
	}
     
}

CVerifyCodeCreater* CVerifyCodeCreater::GetInstance()
{
	if (m_pInstance==NULL)
	{
		m_pInstance=new CVerifyCodeCreater();
	}
	return m_pInstance;
}

void CVerifyCodeCreater::SetParent(const HWND& hHwnd)
{
	if (m_dcParentWnd)
	{
		::ReleaseDC(hHwnd,m_dcParentWnd);
		m_dcParentWnd = NULL;
	}

	m_dcParentWnd = ::GetDC(hHwnd);
	m_dcCodeMem = ::CreateCompatibleDC(m_dcParentWnd);

	return;
}

void CVerifyCodeCreater::ReCreateBMP(int iWidth,int iHeight,COLORREF crBackGround)
{
	if (m_backPixel)
	{
		return;
	}

	m_iWidth = iWidth;
	m_iHeight = iHeight;

	HBITMAP hBitmap = ::CreateCompatibleBitmap(m_dcParentWnd,m_iWidth,m_iHeight);  
	HBITMAP hOldBm = (HBITMAP)::SelectObject(m_dcCodeMem,hBitmap);
	SetBkMode(m_dcCodeMem,TRANSPARENT);

	HBRUSH hBrush = CreateSolidBrush(crBackGround);
	::SelectObject(m_dcCodeMem,hBrush);
	HPEN hPen = CreatePen(PS_SOLID,1,crBackGround);
	::SelectObject(m_dcCodeMem,hPen);
	::Rectangle(m_dcCodeMem,0,0,m_iWidth,m_iHeight);

	//产生随机字符串
	RandomCode(m_szCode,4);

	int iXPos = 0;
	int iYPos = 0;
	::srand(::GetTickCount());
	for (int i = 0; i < 4; ++i)
	{
		//字体随机
		lstrcpy(logFont.lfFaceName,_T(m_FontArray[rand()%22]));   
		logFont.lfHeight = 15 + (rand() % 15);     
		logFont.lfWidth = 15 + (rand() % 10);
		logFont.lfWeight = 100 * (rand() % 7);
		logFont.lfItalic = TRUE;
		logFont.lfEscapement = rand() % 360;

		hFont = CreateFontIndirect(&logFont);
		HFONT oldFont = (HFONT)SelectObject(m_dcCodeMem,hFont); 

		SetTextColor(m_dcCodeMem,RGB(rand() % 225,rand() % 225,rand() % 225)); 

		TCHAR szChar[2] = {m_szCode[i],0};

		iYPos = (iHeight - logFont.lfHeight) / 2 + 7;
		TextOut(m_dcCodeMem,iXPos,iYPos,szChar,1); 
		iXPos += logFont.lfWidth + 5;

		DeleteObject(hFont); 
	}

	DeleteObject(hBitmap);   
}

void CVerifyCodeCreater::Draw(int x,int y)
{
	BitBlt(m_dcParentWnd,x,y,x + m_iWidth,y + m_iHeight,m_dcCodeMem,0,0,SRCCOPY);  
}

bool CVerifyCodeCreater::IsVerify(LPCTSTR lpszInput)
{
	unsigned char temp[10];
	::memset(temp,NULL,sizeof(temp));

	unsigned char* p = (unsigned char*)lpszInput;
	unsigned char* q = temp;
	while(*p)
	{
		if (*p>='a' && *p<='z')
		{
			*p -= 32;
		}
		*(q++) = *(p++);
	}

	return (strcmp((const char*)m_szCode,(const char*)temp) == 0);
}



