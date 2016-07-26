#pragma once

#include "resource.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "UserlogoMessage.h"
#include "SkinMgr.h"
// CLogoUpload 对话框
using namespace Gdiplus;

class CLogoUpload : public CDialog
{
	int x,y;						//记录截取图片的左上角位置
	CPoint		m_savePoint;
	DECLARE_DYNAMIC(CLogoUpload)
//	CImage m_image;	
	Image  *img;
	bool		bMoving;	//是否在移动图片
	CWnd		*pParentWnd;
	int m_skinIndex;
	Image			*imgDialog;
	COLORREF		m_bkcolor;

	HBRUSH			m_bkBrush;
public:
	CLogoUpload(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLogoUpload();

// 对话框数据
	enum { IDD = IDD_LOGOUPLOAD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

private:
	void Init();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	afx_msg void OnBnClickedLoadimage();
public:
	afx_msg void OnBnClickedUpload();
public:
	afx_msg void OnBnClickedCancel();
public:
	afx_msg void OnBnClickedMoveup();
public:
	afx_msg void OnBnClickedMovedown();
public:
	afx_msg void OnBnClickedMoveleft();
public:
	afx_msg void OnBnClickedMoveright();
public:
	afx_msg void OnPaint();
public:
	void drawImage(void);
public:
	CStatic m_smallImage;
public:
	CStatic m_largeImage;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
public:
	afx_msg void OnBnClickedReset();
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
public:
	CStatic m_status;
public:
	CProgressCtrl m_progress;
public:
	bool uploadCustomFace(int nBlockIndex);
public:
	void SetParentWnd(CWnd* pWnd);
	void OnNetMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize);
	void OnUploadSucceed(void *pData);
public:
	void checkUserLogo(int dwUserID);
public:
	void OnCheckLocalLogo(void * pData);
public:
	void OnDownloadFace(void * pData);
public:
	void drawSkin(void);
public:
	void setSkinIndex(int sk);
public:
	afx_msg void OnBnClickedCloselogo();
public:
	CNormalBitmapButtonEx m_bnClose;
	CNormalBitmapButtonEx m_bnBrowse;
	CNormalBitmapButtonEx m_bnLoad;
	CNormalBitmapButtonEx m_bnBack;

	CNormalBitmapButtonEx m_bnMoveUp;
	CNormalBitmapButtonEx m_bnMoveLeft;
	CNormalBitmapButtonEx m_bnMoveDown;
	CNormalBitmapButtonEx m_bnMoveRight;
	CNormalBitmapButtonEx m_bnReset;

protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	int m_cutType;
public:
	afx_msg void OnBnClickedPiccut();
public:
	afx_msg void OnBnClickedPicscale();

private:
	CSkinMgr m_skinmgr;

	void LoadSkin();
public:
	LRESULT OnExchangeSkin(WPARAM wpara,LPARAM lpara);
};
