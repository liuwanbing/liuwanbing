#ifndef OPENGL_ENGINE_HEAD_HEAD_FILE
#define OPENGL_ENGINE_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//包含文件

//MFC 文件
#include <Afxmt.h>
#include <AtlBase.h>

//OPEN 库
#include <GL/GL.h>		
#include <GL/GLU.h>
#include <GL/GLAux.h>

//平台文件
#include "Platform.h"

//////////////////////////////////////////////////////////////////////////////////
//公共定义

//导出定义
#ifndef OPENGL_ENGINE_CLASS
	#ifdef  OPENGL_ENGINE_DLL
		#define OPENGL_ENGINE_CLASS _declspec(dllexport)
	#else
		#define OPENGL_ENGINE_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifndef _DEBUG
	#define OPENGL_ENGINE_DLL_NAME	TEXT("OpenglEngine.dll")			//组件名字
#else
	#define OPENGL_ENGINE_DLL_NAME	TEXT("OpenglEngineD.dll")			//组件名字
#endif

//////////////////////////////////////////////////////////////////////////////////
//结构定义

#define  GL_PI			((FLOAT)  3.141592654f)

#define  GLCOLOR_RGB(r,g,b)	    ((COLORREF)((((255)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#define  GLCOLOR_RGBA(r,g,b,a)  ((COLORREF)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#define  GLCOLOR_ARGB(a,r,g,b)  ((COLORREF)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#define  GLCOLOR_GetR(rgb)      ((BYTE)((rgb)>>16))
#define  GLCOLOR_GetG(rgb)      ((BYTE)((rgb)>> 8))
#define  GLCOLOR_GetB(rgb)      ((BYTE)((rgb)>> 0))
#define  GLCOLOR_GetA(rgb)      ((BYTE)((rgb)>>24))

/////////////////////////////////////////////////////////////////////////////
// 纹理类型
enum ENUM_TEXTURE_FORMATS{
	TEXTURE_FORMAT_UNKNOWN = 0,
	TEXTURE_FORMAT_BMP = 1,
	TEXTURE_FORMAT_GIF = 2,
	TEXTURE_FORMAT_JPG = 3,
	TEXTURE_FORMAT_PNG = 4,
	TEXTURE_FORMAT_ICO = 5,
	TEXTURE_FORMAT_TIF = 6,
	TEXTURE_FORMAT_TGA = 7,
	TEXTURE_FORMAT_PCX = 8,
	TEXTURE_FORMAT_WBMP = 9,
	TEXTURE_FORMAT_WMF = 10,
	TEXTURE_FORMAT_JP2 = 11,
	TEXTURE_FORMAT_JPC = 12,
	TEXTURE_FORMAT_PGX = 13,
	TEXTURE_FORMAT_PNM = 14,
	TEXTURE_FORMAT_RAS = 15,
	TEXTURE_FORMAT_JBG = 16,
	TEXTURE_FORMAT_MNG = 17,
	TEXTURE_FORMAT_SKA = 18,
	TEXTURE_FORMAT_RAW = 19,
};



//////////////////////////////////////////////////////////////////////////////////
//导出文件

#ifndef OPENGL_ENGINE_DLL
	#include "OpenGL.h"
	#include "GLTexture.h"
	#include "GLFont.h"
	#include "Particles.h"
	#include "GLVirtualWindow.h"
	#include "GLVirtualButton.h"
#endif

//////////////////////////////////////////////////////////////////////////

#endif