#pragma once

#include "OpenGL.h"
#include "OpenglEngineHead.h"

//资源信息
struct tagGLResourceInfo
{
	DWORD		dwSize;								//资源大小
	LPVOID		pcbBuffer;							//资源内存
};


//GL TGA纹理类
class OPENGL_ENGINE_CLASS CGLTexture
{
	//图片信息
protected:
	GLuint							m_nWidth;										
	GLuint							m_nHeight;										
	GLuint							m_nTextureID;											

	//旋转信息
protected:
	FLOAT							m_fAngle;									//旋转变量
	FLOAT							m_fRotationX;								//旋转变量
	FLOAT							m_fRotationY;								//旋转变量

	//对称信息
protected:
	BOOL							m_bSymmetrical;								//对称信息

	//变量定义
private:
	GLboolean						m_bLoadSuccess;								// 载入是否成功

public:
	CGLTexture(void);
	~CGLTexture(void);

	//图片信息
public:
	//加载判断
	GLboolean IsNull();
	//精灵宽度
	INT GetWidth() { return m_nWidth; }
	//精灵高度
	INT GetHeight() { return m_nHeight; }
	//纹理ID
	INT GetTextureID() { return m_nTextureID; }

	//纹理管理
public:
	//销毁纹理
	GLboolean Destory();

	//控制函数
public:
	//设置旋转
	GLvoid SetRotation(FLOAT fRotationX, FLOAT fRotationY, FLOAT fAngle);
	//设置对称
	GLvoid SetSymmetrical(BOOL bSymmetrical);

	//功能函数
public:
	//加载纹理
	GLboolean LoadImage( CGLDevice* pOpenGL, HINSTANCE hInstance, LPCTSTR pszResource, LPCTSTR pszTypeName, DWORD wImageType);

	//绘画函数
public:
	//绘画图片
	GLboolean DrawImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest);
	//绘画图片
	GLboolean DrawImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource);
	//绘画图片
	GLboolean DrawImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource, INT nSourceWidth, INT nSourceHeight);

public:
	//绘画图片
	GLboolean DrawImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest, COLORREF GLColor);
	//绘画图片
	GLboolean DrawImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource, COLORREF GLColor);
	//绘画图片
	GLboolean DrawImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource, INT nSourceWidth, INT nSourceHeight, COLORREF GLColor);


	//绘画函数
public:
	//绘画图片
	GLboolean DrawImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest, BYTE cbAlpha);
	//绘画图片
	GLboolean DrawImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource, BYTE cbAlpha);
	//绘画图片
	GLboolean DrawImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource, INT nSourceWidth, INT nSourceHeight, BYTE cbAlpha);

	//绘画函数
public:
	//绘画图片
	GLboolean DrawColourFilterImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest);
	//绘画图片
	GLboolean DrawColourFilterImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource);
	//绘画图片
	GLboolean DrawColourFilterImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource, INT nSourceWidth, INT nSourceHeight);

	//绘画函数
public:
	//绘画图片
	GLboolean DrawColourFilterImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest, BYTE cbAlpha);
	//绘画图片
	GLboolean DrawColourFilterImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource, BYTE cbAlpha);
	//绘画图片
	GLboolean DrawColourFilterImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource, INT nSourceWidth, INT nSourceHeight, BYTE cbAlpha);


	//内部函数
protected:
	//纹理贴图
	GLvoid	  TextureMap(GLfloat fImageBeginX, GLfloat fImageBeginY, GLfloat fImageEndX, GLfloat fImageEndY, INT nDestWidth, INT nDestHeight);
	//获取资源
	GLboolean GetResourceInfo(HINSTANCE hInstance, LPCTSTR pszResource, LPCTSTR pszTypeName, tagGLResourceInfo & ResourceInfo);
	

};



