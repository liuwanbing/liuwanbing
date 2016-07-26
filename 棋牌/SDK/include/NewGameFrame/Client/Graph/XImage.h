#pragma once

/***********************************************************************
* Module:  XImage.h
* Author:  Administrator
* Modified: 2010年4月28日 14:24:10
* Purpose: Declaration of the class XImage
***********************************************************************/

#if !defined(__Hall_XImage_h)
#define __Hall_XImage_h

#include "CommonDefines.h"
#include <Windows.h>
#include <tchar.h>
#include <set>

class X3DEngine;
class XTexture;
class XImageContainer;
class ImageChange_t_Greater;

/// XImage类，引擎提供给UI模块使用的接口
class GRAPH_API XImage
{
	friend class ImageChange_t_Greater;
	friend class XImageContainer;
	friend class X3DEngine;
public:
	/// 构造函数
	XImage();
	/// 析构函数
	virtual ~XImage();
	/// 拷贝构造函数
	XImage(const XImage& oldXImage);
	/// 重载赋值操作符
	XImage &operator= (const XImage& oldXImage);
	/// 载入纹理，此时开始在引擎中生存对象
	int LoadFromFile(TCHAR *szFileName);
	/// 载入纹理，此时开始在引擎中生存对象
	/// @param pData 指向文件内容的指针
	/// @param nSize pData指针所指内存中，属于本文件内容的大小，单位为BYTE
	/// @param szIdentifiedName 用来识别两次载入是否相同纹理的字符串
	int LoadFromFileInMemory(const void *pData, int nSize, const TCHAR *szIdentifiedName);
    /// 更新纹理，将纹理删除，重建
    int LoadTextureFrom32Bitmap(const void *pData, int nWidth, int nHeight, const TCHAR *szIdentifiedName);
	/// 设置该图片在纹理中的坐标，纹理坐标须由外部传入
	int SetPosInTexture(int tx, int ty, int tw, int th);
	/// 设置该图片在窗口中显示的坐标
	int SetPosition(int x, int y, int w, int h, int z);
	/// 设置该图片的方向
	int SetDir(int direction);
	/// 获得纹理的尺寸
	int GetTextureSize(int &w, int &h);
	/// 设置图片四个顶点的颜色，按左上，右上，左下、右下的次序
	int SetColor(DWORD c1, DWORD c2, DWORD c3, DWORD c4);
	/// 显示或隐藏图片
	void ShowHide(bool bShow = true);
	/// 提交之前所作的更改
	void CommitChanges();
	/// 设置裁剪范围
	void SetClipBox(short l, short r, short t, short b){m_sClipBox[0]=l; m_sClipBox[1]=r;m_sClipBox[2]=t; m_sClipBox[3]=b;}
    /// 设置镜像
    void SetCrossMirror(bool bMirror){m_bCorssMirror = bMirror;}
    void SetVerticalMirror(bool bMirror){m_bVerticalMirror = bMirror;}
protected:
private:
	int m_x, m_y, m_w, m_h;		///< 图片在窗口中的位置
	int m_z;					///< 图片的层次，前后关系，越小越靠上层
	int m_tx, m_ty, m_tw, m_th;	///< 图片在纹理中的位置
	int m_direction;			///< 图片的方向，单位为度
	DWORD m_color[4];			///< 图片显示时用的四个顶点的颜色
	short m_sClipBox[4];		///< 裁剪范围，0,1,2,3对应左，右，上，下
    
    bool m_bCorssMirror;             /// 是否镜像
    bool m_bVerticalMirror;          /// 是否镜像

	XTexture* m_pTexture;		///< 所使用的纹理

	static XImageContainer* sm_pImageContainer;	///< 图片容器
};

/// 元素修改状况记录，当发生变化时，设备重置需要记下已经发生过的变化
/// 当某图片处于隐藏状态时，不需要写入显存
struct ImageChange_t
{
	XImage * _pImage;		///< 修改的元素
	bool	_bHide;			///< 修改是否为隐藏

	ImageChange_t(){_pImage = NULL; _bHide = false;}
	ImageChange_t(XImage *pImage){_pImage = pImage; _bHide = false;}
	bool operator==(const ImageChange_t rhs){return this->_pImage == rhs._pImage;}
};
/// 排序规则，容器中使用的节点是这样的类型
class ImageChange_t_Greater : public std::binary_function<const ImageChange_t &, const ImageChange_t &, bool>
{
public:
	bool operator()(const ImageChange_t &lhs, const ImageChange_t &rhs)
	{
		if (lhs._pImage->m_z == rhs._pImage->m_z)
		{
			if (lhs._pImage->m_y == rhs._pImage->m_y)
			{
				if (lhs._pImage->m_x == rhs._pImage->m_x)
				{
					if (lhs._pImage->m_pTexture == rhs._pImage->m_pTexture)
					{
						if (lhs._pImage->m_tx == rhs._pImage->m_tx)
						{
							if (lhs._pImage->m_ty == rhs._pImage->m_ty)
							{
								if (lhs._pImage->m_tw == rhs._pImage->m_tw)
								{
									return lhs._pImage->m_pTexture >rhs._pImage->m_pTexture;
								}
								return (lhs._pImage->m_tw > rhs._pImage->m_tw);
							}
							return (lhs._pImage->m_ty > rhs._pImage->m_ty);
						}
						return lhs._pImage->m_tx > rhs._pImage->m_tx;
					}
					return (lhs._pImage->m_pTexture >rhs._pImage->m_pTexture);
				}
				return (lhs._pImage->m_x > rhs._pImage->m_x);
			}
			return (lhs._pImage->m_y > rhs._pImage->m_y);
		}
		else
		{
			return (lhs._pImage->m_z > rhs._pImage->m_z);
		}
	}
};
#endif
