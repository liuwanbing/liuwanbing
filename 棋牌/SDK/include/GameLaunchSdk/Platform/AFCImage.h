#pragma once

#ifndef CAFCIMAGE_HEAD_FILE
#define CAFCIMAGE_HEAD_FILE

#include "PublicDefine.h"
#include "Vfw.h"
#include "AtlImage.h"

/*namespace AFC
{
	namespace AFCImageGoGoGo
	{*/
		/*******************************************************************************************************/

		//资源类型
		enum AFCResourceType
		{
			AFC_NO_OBJECT,
			AFC_IMAGE,
			AFC_VIEW_IMAGE,
		};

		/*******************************************************************************************************/

		//接口说明
		interface IAFC_GDI_Resource;

		//AFC 资源句柄接口
		interface EXT_CLASS IAFC_GDI_Handle 
		{
			//函数定义
		public:
			//是否打开句柄
			virtual bool IsOpenHandle()=0;
			//获取类型
			virtual AFCResourceType GetResourceKind()=0;
			//打开句柄
			virtual bool OpenGDIHandle(IAFC_GDI_Resource * pGDIObject)=0;
			//关闭句柄
			virtual bool CloseGDIHandle()=0;
		};

		//AFC GDI 资源接口
		interface EXT_CLASS IAFC_GDI_Resource
		{
			//函数定义
		protected:
			//增加引用
			virtual UINT LockResource()=0;
			//减少引用
			virtual UINT FreeResource()=0;

		public:
			//获取类型
			virtual AFCResourceType GetResourceKind()=0;
		};

		/*******************************************************************************************************/

		//视频位图
		class EXT_CLASS CGameViewImage//CAFCViewImage
		{
			//变量定义
		protected:
			HANDLE								m_hFileMap;						//文件句柄
			HDRAWDIB							m_hDrawDib;						//视频句柄
			BITMAPFILEHEADER					* m_pImageHead;					//文件指针
			BITMAPINFOHEADER					* m_pImageInfo;					//图像信息

			//函数定义
		public:
			//构造函数
			CGameViewImage();
			//析构函数
			virtual ~CGameViewImage();

			void SetWidth(){;};
			void SetHeight(){;};

			//功能函数
		public:
			//加载位图
			bool Load(const TCHAR * szFileName);
			void LoadFromJpg(const TCHAR * szFileName){;};
			void LoadFromBmp(const TCHAR * szFileName){;};
			//销毁位图
			bool Destroy();
			//获取宽度
			inline UINT GetWidth();
			//获取高度
			inline UINT GetHeight();
			//绘画函数
			inline bool BitBlt(CDC * pDesDC, UINT uXDes, UINT uYDes, UINT uDesWidth, UINT uDesHeight, UINT uXScr, UINT uYScr, UINT uScrWidth, UINT uScrHeight);
			void Save(){;};
		};

		/*******************************************************************************************************/

		//类说明
		class CGameImageHelper;

		//AFC 图像资源
		class EXT_CLASS CGameImage/*C-AFCImage*/ : public IAFC_GDI_Resource
		{
			friend class CGameImageHelper;

			//变量定义
		private:
			bool								m_bAutoLock;					//自动锁定
			UINT								m_uLock;						//锁定计数
			UINT								m_uResourceID;					//资源 ID
			CImage								m_Image;						//资源图片
			CString								m_strFileName;					//文件名字
			HINSTANCE							m_hResourceDLL;					//资源句柄

			//函数定义
		public:
			//构造函数
			CGameImage(HINSTANCE hResourceDLL=NULL, UINT uResourceID=0, TCHAR * szFileName=NULL, bool bLock=false);
			//析构函数
			virtual ~CGameImage();

			//接口函数
		protected:
			//增加引用
			virtual UINT LockResource();
			//减少引用
			virtual UINT FreeResource();

			//接口函数
		public:
			//获取类型
			virtual AFCResourceType GetResourceKind() { return AFC_IMAGE; };

			//功能函数
		public:
			//附加对象
			bool Attach(CGameImage * pImageObject, bool bLock);
			//分离对象
			bool Detach();
			//是否空
			inline bool IsNull();
			//设置加载参数
			bool SetLoadInfo(TCHAR * szFileName, bool bLock);
			//设置加载参数
			bool SetLoadInfo(HINSTANCE hResourceDLL, UINT uResourceID, bool bLock);

			//操作重载
		public:
			//操作重载
			inline operator HBITMAP () { return m_Image; };

			//操作函数
		public:
			//获取颜色
			inline COLORREF GetPixel(int x, int y) { return m_Image.GetPixel(x,y); };
			//获取宽度
			inline int GetWidth() {  return m_Image.IsNull()?0:m_Image.GetWidth();}
			//获取高度
			inline int GetHeight() { return m_Image.IsNull()?0:m_Image.GetHeight(); }
			//绘图函数
			inline BOOL BitBlt(HDC hDestDC, int xDest, int yDest, DWORD dwROP=SRCCOPY);
			//绘图函数
			inline BOOL BitBlt(HDC hDestDC, const POINT & pointDest, DWORD dwROP=SRCCOPY);
			//绘图函数
			inline BOOL BitBlt(HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, int xSrc, int ySrc, DWORD dwROP=SRCCOPY);
			//绘图函数
			inline BOOL BitBlt(HDC hDestDC,const RECT & rectDest, const POINT & pointSrc, DWORD dwROP=SRCCOPY);
			//绘图函数
			inline BOOL StretchBlt(HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, DWORD dwROP=SRCCOPY);
			//绘图函数
			inline BOOL StretchBlt(HDC hDestDC, const RECT& rectDest, DWORD dwROP=SRCCOPY);
			//绘图函数
			inline BOOL StretchBlt(HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, DWORD dwROP=SRCCOPY);
			//绘图函数
			inline BOOL StretchBlt(HDC hDestDC, const RECT& rectDest, const RECT& rectSrc, DWORD dwROP=SRCCOPY);
		};

		/*******************************************************************************************************/

		//AFC 资源句柄
		class EXT_CLASS CGameImageHelper/*C-AFCImageHandle*/ : public IAFC_GDI_Handle
		{
			//变量定义
		private:
			CGameImage							* m_pAFCImage;					//资源指针

			//函数定义
		public:
			//构造函数
			CGameImageHelper(IAFC_GDI_Resource * pGDIObject);
			//析构函数
			virtual ~CGameImageHelper();

			//接口函数
		public:
			//是否打开句柄
			virtual bool IsOpenHandle() { return (m_pAFCImage!=NULL); };
			//获取类型
			virtual AFCResourceType GetResourceKind();
			//打开句柄
			virtual bool OpenGDIHandle(IAFC_GDI_Resource * pGDIObject);
			//关闭句柄
			virtual bool CloseGDIHandle();

			//操作函数
		public:
			//获取颜色
			inline COLORREF GetPixel(int x, int y) { return m_pAFCImage->GetPixel(x,y); };
			//获取宽度
			inline int GetWidth() { return m_pAFCImage->GetWidth(); }
			//获取高度
			inline int GetHeight() { return m_pAFCImage->GetHeight(); }
			//绘图函数
			inline BOOL BitBlt(HDC hDestDC, int xDest, int yDest, DWORD dwROP=SRCCOPY);
			//绘图函数
			inline BOOL BitBlt(HDC hDestDC, const POINT & pointDest, DWORD dwROP=SRCCOPY);
			//绘图函数
			inline BOOL BitBlt(HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, int xSrc, int ySrc, DWORD dwROP=SRCCOPY);
			//绘图函数
			inline BOOL BitBlt(HDC hDestDC,const RECT & rectDest, const POINT & pointSrc, DWORD dwROP=SRCCOPY);
			//绘图函数
			inline BOOL StretchBlt(HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, DWORD dwROP=SRCCOPY);
			//绘图函数
			inline BOOL StretchBlt(HDC hDestDC, const RECT& rectDest, DWORD dwROP=SRCCOPY);
			//绘图函数
			inline BOOL StretchBlt(HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, DWORD dwROP=SRCCOPY);
			//绘图函数
			inline BOOL StretchBlt(HDC hDestDC, const RECT& rectDest, const RECT& rectSrc, DWORD dwROP=SRCCOPY);

			//操作重载
		public:
			//操作重载
			inline CGameImageHelper & operator = (CGameImageHelper & ImageHandle);
			//操作重载
			inline operator CImage * () { return GetImage(); }
			//操作重载
			inline operator CBitmap * () { return GetBitmap(); }
			//操作重载
			inline operator HBITMAP ();
			//获取对象
			inline CImage * GetImage();
			//获取对象
			inline CBitmap * GetBitmap();
		};

		/*******************************************************************************************************/
	/*};
};*/

#endif