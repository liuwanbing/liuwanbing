#ifndef _100125_PNG_ANIMAL_H
#define _100125_PNG_ANIMAL_H

#include <vector>
using std::vector;

const int TIME_RENDER = 1001;		/// 渲染定时器
const int TIME_FRAME = 1002;		/// 变帧定时器
const int RENDER_FLIP = 5;			/// 多少ms渲染一次

/// Memdc类
/// 注意：	如果调用了 Create()， 请一定要调用 Delete()
/// 注意：	创建一个	CMemdc A;
///			再创建一个	CMemdc B;
///			然后		B = A;
///			最后，调用	B.Delete();就可以了。
class CMemdc
{
public:
	///
	/// 构造函数
	CMemdc(void);
	///
	/// 析构函数
	~CMemdc(void);
	///
	/// 创建内存DC
	/// @param[in]	HDC		hSrcDC		源泉
	/// @param[in]	int		iWidth		画布的大小
	/// @param[in]	int		iHeight		画布的高度
	/// @return void
	void Create(HDC hSrcDC, int iWidth, int iHeight);
	///
	/// 得到兼容DC
	/// @param	void
	/// @return	const	HDC		兼容DC
	const HDC GetCompatibleDC(void) const;
	/// 
	/// 得到宽度
	/// @param	void
	/// @return	int		返回宽度
	int GetWidth();
	/// 得到高度
	/// @param	void
	/// @return	int		返回高度
	int GetHeight();
	///
	/// 删除内存DC
	/// param	void
	/// return	void
	void Delete(void);
	///
	/// 赋值函数
	CMemdc	&operator = (const CMemdc &other);
	///
	/// 复制构造函数
	CMemdc(const CMemdc &other);


private:
	HBITMAP				m_hCompatibleBmp;				///< 兼容位图
	HDC					m_hCompatibleDC;				///< 兼容DC
	int					m_iWidth;						///< 兼容位图的宽
	int					m_iHeight;						///< 兼容位图的高

	void				*m_pData;						///< 位图数据 
	BITMAPINFO			m_BmpInfo;						///< 位图图像
};


/// 动画类
class EXT_CLASS CWndAnimate : public CWnd
{
public:
	///
	/// 构造函数
	CWndAnimate(void);
	///
	/// 析构函数
	virtual ~CWndAnimate(void);
private:
	///
	/// 复制构造函数
	CWndAnimate(const CWndAnimate &other);
	///
	/// 赋值函数
	CWndAnimate &operator = (const CWndAnimate &other);

public:

	/// 从包中读取Image、注意要手动释放 
	static Image *InitImageFromPkg(const wchar_t *pszFileName);

	///
	/// 创建
	///@param[in] CWnd *pParent 父窗口
	///@return bool true 成功 false 失败
	bool Create(CWnd *pParent);

	///
	/// 加入一张PNG图片
	/// @param[in]		const wchar_t pszFileName*			图片文件名(Unicode)
	void AppendPNG(const wchar_t *pszFileName);	

	///最大化时动画的位置可能会变，此函数进行调整
	void SetAnimalPos(int iX,int iY) ; 
	/// 
	/// 加入一张PNG图片，提定大小和位置
	/// @param[in]		const wchar_t	pszFileName*		图片文件名(Unicode)
	/// @param[in]		int				nSrcX
	/// @param[in]		int				nSrcY
	/// @param[in]		int				nWidth
	/// @param[in]		int				nHeight
	void AppendPNG(const wchar_t *pszFileName, int nSrcX, int nSrcY, int nWidth, int nHeight);

	/// 加入一张图片，图片横向分成几帧
	/// @param[in]		const wchar_t	pszFileName*		图片文件名(Unicode)
	/// @param[in]		int				nCutInHor			横向分成几帧
	void AppendPNG(const wchar_t *pszFileName, int nCutInHor);

	/// 加入一张图片，图片横向分成几帧，带关键色
	/// @param[in]		const wchar_t	pszFileName*		图片文件名(Unicode)
	/// @param[in]		int				nCutInHor			横向分成几帧
	void AppendPNG(const wchar_t *pszFileName, COLORREF clrKey, int nCutInHor);

	///
	/// 开始动画
	///@param[in] iX, iY 座标
	///@param[in] iWidth, iHeight 宽度高度
	///@param[in] nRound 播放多少次
	///@param[in] iFlip 每帧间隔
	///@return void
	void BeginAnimate(int iX, int iY, int iWidth, int iHeight, int nRound = 1, int iFlip = 100);

	/// 停止动画
	void StopAnimate(void);

	/// 显示与否
	void SetShowWindow(bool bShow)
	{
		m_bShowWindow = bShow;
	}

	/// 得到显示与否状态
	bool GetIsShow()
	{
		return m_bShowWindow;
	}

	/// 得到某一帧的DC，用来自己画
	HDC GetOneFrameDC(int nPos)
	{
		if (nPos>m_FrameList.size())
		{
			return NULL;
		}

		return m_FrameList.at(nPos).GetCompatibleDC();		
	}

	/// 加入空帧
	int InsertBlankFrame(int iWidth, int iHeight)
	{
		HDC hScreendc = GetDC()->m_hDC;

		CMemdc dc;
		dc.Create(hScreendc, iWidth, iHeight);
		m_FrameList.push_back(dc);
		m_nFrameCount ++;

		return m_FrameList.size();
	}

	/// 调整位置
	void CordinatePos()
	{		
		if (NULL==m_pParent)
		{
			return;
		}
		/// 窗口座标
		POINT dst;
		dst.x = m_iX;//rct.left;
		dst.y = m_iY;//rct.top;
		m_pParent->ClientToScreen(&dst);

		SetWindowPos(m_pParent,dst.x,dst.y,0,0,SWP_NOSIZE);
	}

	/// 是否正播放
	bool GetIsPlaying()
	{
		return m_bPlaying;
	}
protected:
	/**
	 * @brief 不允许重载
	 */
	virtual afx_msg void OnPaint();
	/**
	 * @brief 在这里控制动画的位置、播放哪一帧等等
	 */
	virtual void OnFrameMove();	

	/// 
	/// 定时器响应消息
	/// @param[in]		UINT_PTR nIDEvent		定时器ID
	afx_msg void OnTimer(UINT_PTR nIDEvent);

private:
	/// 渲染动画
	void RenderAnimate(void);

	/// 
	/// 绘画这个
	///
	/// 动画线程
	//static DWORD WINAPI AnimateThread(LPVOID lpParam);

protected:
	int							m_iX;
	int							m_iY;
	int							m_iWidth;
	int							m_iHeight;
	int							m_nPos;							///< 播放哪一帧
	bool						m_bShowWindow;					///< 是否显示


	bool						m_bPlaying;						/// 是否正在播放

	CWnd						*m_pParent;						///< 父窗口

private:
	vector<CMemdc>				m_FrameList;					///< 帧队列

	int							m_iFlip;						///< 帧间间隔	
	int							m_nFrameCount;					///< 共有几帧
	int							m_nTime;						///< 播放几次
	int							m_nTotalRound;					///< 总共播放多少次

	DECLARE_MESSAGE_MAP()
};

#endif