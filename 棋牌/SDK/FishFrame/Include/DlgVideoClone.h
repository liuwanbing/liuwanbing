#pragma once


class CDlgVideoClone : public CDialog
{
	//变量定义
public:
	HICON							m_hIcon;						//图标资源

	//函数定义
public:
	//构造函数
	CDlgVideoClone(CWnd* pParent = NULL);
	//析构函数
	virtual ~CDlgVideoClone();

	//功能函数
public:
	//绘制视频
	void DrawUserVideo(DWORD dwUserid, LPVOID lpBuf, DWORD dwLen, BITMAPINFOHEADER bmiHeader);

	//继承函数
protected:
	//数据绑定
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CDlgVideoClone)
public:
	virtual BOOL OnInitDialog();
};
