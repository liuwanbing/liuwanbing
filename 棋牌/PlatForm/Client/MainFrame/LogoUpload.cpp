// LogoUpload.cpp : 实现文件
//

#include "stdafx.h"
#include "LogoUpload.h"
#include "MainRoomEx.h"

// CLogoUpload 对话框

/*
Notes	:自定义头像的大小，包括了大头像和小头像
放在本文件中，而不放在头文件当中，是因为，更改值后编译时只需要编译本文件即可，而放在头文件中，会对整个PROJECT重新编译，而在业务流程上没有必要
Author	:Fred Huang
Date	:2008-03-19
*/

//#define LARGE_WIDTH		75
//#define LARGE_HEIGHT	105
//#define SMALL_WIDTH		52
//#define SMALL_HEIGHT	73
//2008.3.28李庆惠美工全部调整

#define LARGE_WIDTH		50
#define LARGE_HEIGHT	50
#define SMALL_WIDTH		45
#define SMALL_HEIGHT	52

IMPLEMENT_DYNAMIC(CLogoUpload, CDialog)

CLogoUpload::CLogoUpload(CWnd* pParent /*=NULL*/)
: CDialog(CLogoUpload::IDD, pParent)
, m_cutType(1)
{
	x=y=0;
	bMoving=false;
	pParentWnd=NULL;
	m_skinIndex=0;
	imgDialog=0;
	img=0;

	CString s=CBcfFile::GetAppPath ();/////本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + strSkin);
	CString key= m_skinmgr.GetKeyVal(strSkin);
	int r,g,b;
	r=f.GetKeyVal(key,"logobgcr",44);
	g=f.GetKeyVal(key,"logobgcg",114);
	b=f.GetKeyVal(key,"logobgcb",233);
	m_bkcolor=RGB(r,g,b);
	m_bkBrush = NULL;
}

CLogoUpload::~CLogoUpload()
{
	//如果有图像内存对象，要删除
	if(img && img->GetFlags()!=0)
	{
		delete img;
	}
	DeleteObject(m_bkBrush);
}

void CLogoUpload::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SMALLIMAGE, m_smallImage);
	DDX_Control(pDX, IDC_LARGEIMAGE, m_largeImage);
	DDX_Control(pDX, IDC_STATE, m_status);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress);
	DDX_Control(pDX, IDC_CLOSELOGO, m_bnClose);
	DDX_Control(pDX, IDC_LOADIMAGE, m_bnBrowse);
	DDX_Control(pDX, IDC_UPLOAD, m_bnLoad);
	DDX_Control(pDX, IDCANCEL, m_bnBack);
	DDX_Control(pDX, IDC_MOVEUP,m_bnMoveUp);
	DDX_Control(pDX, IDC_MOVELEFT,m_bnMoveLeft);
	DDX_Control(pDX, IDC_MOVEDOWN,m_bnMoveDown);
	DDX_Control(pDX, IDC_MOVERIGHT,m_bnMoveRight);
	DDX_Control(pDX, IDC_RESET,m_bnReset);

	DDX_Radio(pDX, IDC_PICCUT, m_cutType);
}


BEGIN_MESSAGE_MAP(CLogoUpload, CDialog)
	ON_BN_CLICKED(IDC_LOADIMAGE, &CLogoUpload::OnBnClickedLoadimage)
	ON_BN_CLICKED(IDC_UPLOAD, &CLogoUpload::OnBnClickedUpload)
	ON_BN_CLICKED(IDCANCEL, &CLogoUpload::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_MOVEUP, &CLogoUpload::OnBnClickedMoveup)
	ON_BN_CLICKED(IDC_MOVEDOWN, &CLogoUpload::OnBnClickedMovedown)
	ON_BN_CLICKED(IDC_MOVELEFT, &CLogoUpload::OnBnClickedMoveleft)
	ON_BN_CLICKED(IDC_MOVERIGHT, &CLogoUpload::OnBnClickedMoveright)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RESET, &CLogoUpload::OnBnClickedReset)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_CLOSELOGO, &CLogoUpload::OnBnClickedCloselogo)
	ON_BN_CLICKED(IDC_PICCUT, &CLogoUpload::OnBnClickedPiccut)
	ON_BN_CLICKED(IDC_PICSCALE, &CLogoUpload::OnBnClickedPicscale)
	ON_MESSAGE(WM_EXCHANGE_SKIN,&CLogoUpload::OnExchangeSkin)
END_MESSAGE_MAP()


// CLogoUpload 消息处理程序

BOOL CLogoUpload::OnInitDialog()
{
	CDialog::OnInitDialog();

	Init();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CLogoUpload::Init()
{
	GetDlgItem(IDC_UPLOAD)->EnableWindow(FALSE);
	m_progress.SetRange(0,100);
	m_progress.SetPos(0);

	TCHAR path[MAX_PATH];
	wsprintf(path,"%slogon\\button.bmp",m_skinmgr.GetSkinPath());
	m_bnBrowse.LoadButtonBitmap(path,false);
	m_bnLoad.LoadButtonBitmap(path,false);
	m_bnBack.LoadButtonBitmap(path,false);

	wsprintf(path,"%slogon\\move.bmp",m_skinmgr.GetSkinPath());
	m_bnMoveUp.LoadButtonBitmap(path,false);
	m_bnMoveLeft.LoadButtonBitmap(path,false);
	m_bnMoveDown.LoadButtonBitmap(path,false);
	m_bnMoveRight.LoadButtonBitmap(path,false);
	m_bnReset.LoadButtonBitmap(path,false);

	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");

	SetDlgItemText(IDC_STATIC_HEAD,fMsg.GetKeyVal("LogoUploadDlg","PreviewHead","头像预览"));
	SetDlgItemText(IDC_STATIC_NOTE,fMsg.GetKeyVal("LogoUploadDlg","AdjustHead",
		"      注：可以通过上、下、左、右键头和鼠标在大头像中拖动来移动图片的位置。"));
	SetDlgItemText(IDC_STATIC_POS,fMsg.GetKeyVal("LogoUploadDlg","AdjustPosition","位置调整"));
	SetDlgItemText(IDC_PICCUT,fMsg.GetKeyVal("LogoUploadDlg","CutPic","截取图片"));
	SetDlgItemText(IDC_PICSCALE,fMsg.GetKeyVal("LogoUploadDlg","UploadPicDirectly","直接上传(自动缩放)"));
	SetDlgItemText(IDC_LOADIMAGE,fMsg.GetKeyVal("LogoUploadDlg","BrowsePic","浏览图片"));
	SetDlgItemText(IDC_UPLOAD,fMsg.GetKeyVal("LogoUploadDlg","UploadPic","上传图片"));
	SetDlgItemText(IDCANCEL,fMsg.GetKeyVal("LogoUploadDlg","Cancel","取消返回"));
	SetDlgItemText(IDC_STATE_UN,fMsg.GetKeyVal("LogoUploadDlg","UnuploadPic","未加载图片"));

	CRect rc;
	GetWindowRect(&rc);
	m_bnClose.MoveWindow(rc.Width()-25,2,17,17);
	setSkinIndex(0);
}
//换肤
void CLogoUpload::LoadSkin()
{
	TCHAR path[MAX_PATH];
	wsprintf(path,"%slogon\\button.bmp",m_skinmgr.GetSkinPath());
	m_bnBrowse.LoadButtonBitmap(path,false);
	m_bnLoad.LoadButtonBitmap(path,false);
	m_bnBack.LoadButtonBitmap(path,false);

	wsprintf(path,"%slogon\\move.bmp",m_skinmgr.GetSkinPath());
	m_bnMoveUp.LoadButtonBitmap(path,false);
	m_bnMoveLeft.LoadButtonBitmap(path,false);
	m_bnMoveDown.LoadButtonBitmap(path,false);
	m_bnMoveRight.LoadButtonBitmap(path,false);
	m_bnReset.LoadButtonBitmap(path,false);


	setSkinIndex(0);
}

BOOL CLogoUpload::PreTranslateMessage(MSG* pMsg)
{
	try
	{
		if(pMsg->message==WM_KEYDOWN)
		{ 
			if(pMsg->wParam==VK_RETURN || pMsg->wParam == VK_F1)
				return TRUE;
			if(pMsg->wParam==VK_ESCAPE)
			{
				this->ShowWindow(SW_HIDE);
				return TRUE;
			}
			//如果已加载图片
			if(img && img->GetFlags()!=0)
			{
				int w=img->GetWidth();//图的宽度，这里是临时使用，所以用简单变量
				int h=img->GetWidth();//图的高度，这里是临时使用，所以用简单变量
				switch(pMsg->wParam)
				{
				case VK_DOWN:
					if(y>0)
						y--;
					break;
				case VK_UP:
					if(y<h-LARGE_HEIGHT)
						y++;
					break;
				case VK_RIGHT:
					if(x>0)
						x--;
					break;
				case VK_LEFT:
					if(x<w-LARGE_WIDTH)
						x++;
					break;
				default://如果不是以上几个按键，将w和h设为-1.
					w=h=-1;
					break;
				}
				//当w!=-1，即按键是上、下、左、右时，才刷新重显图片，以节省资源
				if(w!=-1)
					drawImage();
			}


		}
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"LOGO_UP_TRY_CATCH : [%s]",__FUNCTION__);
	}


	return CDialog::PreTranslateMessage(pMsg);
}

//加载图片
void CLogoUpload::OnBnClickedLoadimage()
{
	try
	{
		////////////////////////////////////////////
		////Kylin 20090118 增加图片类型过滤
		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
		CString str;
		str = fMsg.GetKeyVal("LogoUploadDlg","PicFilter","图片文件 (*.bmp;*.jpg;*.jpeg;*.png)|*.bmp;*.jpg;*.jpeg;*.png||");

		CFileDialog fd(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,str.GetBuffer());
		////////////////////////////////////////////

		if(fd.DoModal()==IDCANCEL)
			return;

		if(img)
		{
			delete img;
			img=0;
		}
#ifndef BZ_ZLIB  //< add by wxx 
		img=Image::FromFile(fd.GetPathName().AllocSysString());
#else
		CString strTmp=fd.GetPathName();
		CUnZipRes unZipRes;
		IStream * pStream=NULL;
		TCHAR * pchar=strTmp.GetBuffer(strTmp.GetLength()+1);
		pStream=unZipRes.LoadFileResFromZip(pchar);

		img=new Image(pStream);
		if(pStream!=NULL)
		{
			pStream->Release();
			pStream=NULL;
		}

#endif
		
		if(!img || img->GetFlags()==0)
		{
			GetDlgItem(IDC_UPLOAD)->EnableWindow(FALSE);
			m_status.SetWindowText(fMsg.GetKeyVal("LogoUploadDlg","UnuploadPic","未加载图片"));
			str = fMsg.GetKeyVal("LogoUploadDlg","LoadPicFailed","对不起，加载图片失败，请检查图片格式。\n\n支持BMP、JPG、PNG图");
			
			//AFCMessageBox(str);
	        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",str);
			img=0;
			return;
		}
		x=0;y=0;
		GetDlgItem(IDC_UPLOAD)->EnableWindow(TRUE);
		//可能是由于Image加载的时间，刚加载完后显示不了图片，延时100ms再显示就正常了
		SetTimer(1,200,NULL);
		return;
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"LOGO_UP_TRY_CATCH : [%s]",__FUNCTION__);
	}
	//if(!m_image.IsNull())
	//	m_image.Destroy();
	//if(m_image.Load(fd.GetPathName())!=S_OK)
	//{
	//	GetDlgItem(IDC_UPLOAD)->EnableWindow(FALSE);
	//	m_status.SetWindowText(TEXT("未加载图片"));
	//	MessageBox(TEXT("对不起，加载图片失败，请检查图片格式。\n\n支持BMP、JPG、PNG图"),TEXT("错误"),MB_ICONERROR);
	//	return;
	//}
	//x=0;y=0;
	//GetDlgItem(IDC_UPLOAD)->EnableWindow(TRUE);
	////可能是由于Image加载的时间，刚加载完后显示不了图片，延时100ms再显示就正常了
	//SetTimer(1,200,NULL);
}


void CLogoUpload::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码

	this->ShowWindow(SW_HIDE);
}

//图片上移
void CLogoUpload::OnBnClickedMoveup()
{
	try
	{
		if(!img)
			return;
		if(img->GetFlags()==0)
			return;
		if(y<img->GetHeight()-LARGE_HEIGHT)
			y++;
		drawImage();
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"LOGO_UP_TRY_CATCH : [%s]",__FUNCTION__);
	}

}

//图片下移
void CLogoUpload::OnBnClickedMovedown()
{
	try
	{
		if(!img)
			return;
		if(img->GetFlags()==0)
			return;
		if(y>0)
			y--;
		drawImage();
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"LOGO_UP_TRY_CATCH : [%s]",__FUNCTION__);
	}

}

//图片左移
void CLogoUpload::OnBnClickedMoveleft()
{
	try
	{
		if(!img)
			return;
		if(img->GetFlags()==0)
			return;
		if(x<img->GetWidth()-LARGE_WIDTH)
			x++;
		drawImage();
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"LOGO_UP_TRY_CATCH : [%s]",__FUNCTION__);
	}

}

//图片右移
void CLogoUpload::OnBnClickedMoveright()
{
	try
	{
		if(!img)
			return;
		if(img->GetFlags()==0)
			return;
		if(x>0)
			x--;
		drawImage();
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"LOGO_UP_TRY_CATCH : [%s]",__FUNCTION__);
	}

}

void CLogoUpload::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	drawSkin();
	drawImage();
}

/*
Function	:drawImage
Notes		:在上传前预览图片在2个显示框中显示图片，直接截取图片，小图是直接用大图拉抻的，事实上就1张图
由于图片大小规格在各平台都不一样，现暂时统一为：大图 75*105，小图：52*73
Author		:Fred Huang
Date		:2008-03-18
Parameter	:na
return		:void
*/
void CLogoUpload::drawImage(void)
{
	try
	{
		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
		CString str;

		if(!img)
			return;
		if(img->GetFlags()==0)
		{
			img=0;
			return;
		}
		int w=img->GetWidth()>LARGE_WIDTH?LARGE_WIDTH:img->GetWidth();
		int h=img->GetHeight()>LARGE_HEIGHT?LARGE_HEIGHT:img->GetHeight();

		//显示时大图的位置，只在图片小于大图标准规格时有效
		int l=w<LARGE_WIDTH?(LARGE_WIDTH-w)/2:0;
		int t=h<LARGE_HEIGHT?(LARGE_HEIGHT-h)/2:0;

		Graphics g(this->m_hWnd);
		if(m_cutType==0)
			g.DrawImage(img,Rect(l+30,t+58,w,h),x,y,w,h,UnitPixel);
		else
		{
			g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
			g.DrawImage(img,Rect(30,58,LARGE_WIDTH,LARGE_HEIGHT),0,0,img->GetWidth(),img->GetHeight(),UnitPixel);
		}
		//显示当前的图片信息
		CString status;
		str = fMsg.GetKeyVal("LogoUploadDlg","PicSize","图片大小：%d * %d ，左上角位置(x,y)：%d,%d");
		status.Format(str,img->GetWidth(),img->GetHeight(),x,y);
		m_status.SetWindowText(status);
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"LOGO_UP_TRY_CATCH : [%s]",__FUNCTION__);
	}

}

void CLogoUpload::OnTimer(UINT_PTR nIDEvent)
{
	try
	{
		if(nIDEvent==1)
		{
			//只用于延时显示，所以调用一次以后，删除该定时器
			KillTimer(1);
			Invalidate(TRUE);
		}
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"LOGO_UP_TRY_CATCH : [%s]",__FUNCTION__);
	}
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialog::OnTimer(nIDEvent);
}
/*
Function	:OnBnClickedReset
Notes		:重置显示位置，从图的左上角开始
Author		:Fred Huang
Date		:2008-03-18
Parameter	:na
return		:void
*/
void CLogoUpload::OnBnClickedReset()
{
	x=y=0;
	Invalidate(TRUE);
}

void CLogoUpload::OnMouseMove(UINT nFlags, CPoint point)
{
	try
	{
		if(img && img->GetFlags()!=0 && m_cutType==0)
		{
			CRect rect;
			m_largeImage.GetWindowRect(&rect);
			this->ScreenToClient(&rect);
			if(PtInRect(rect,point))
			{
				//在移动当中
				if(bMoving)
				{
					//鼠标新的位置与旧的位置的差
					int dx=m_savePoint.x-point.x;
					int dy=m_savePoint.y-point.y;
					if(x+dx>=0 && x+dx<img->GetWidth()-LARGE_WIDTH)
						x+=dx;
					if(y+dy>=0 && y+dy<img->GetHeight()-LARGE_HEIGHT)
						y+=dy;
					m_savePoint=point;
					drawImage();
					SetCursor(::LoadCursor(NULL, IDC_SIZEALL));
				}
			}
			else
				bMoving=false;
		}
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"LOGO_UP_TRY_CATCH : [%s]",__FUNCTION__);
	}

	CDialog::OnMouseMove(nFlags, point);
}

void CLogoUpload::OnLButtonDown(UINT nFlags, CPoint point)
{
	try
	{
		if(img && img->GetFlags()!=0 && m_cutType==0)
		{
			CRect rc;
			rc.left=21;rc.top=52;rc.right=rc.left+53;rc.bottom=rc.top+57;
			if(PtInRect(rc,point))
			{
				SetCursor(::LoadCursor(NULL, IDC_SIZEALL));
				m_savePoint=point;
				bMoving=true;
			}
		}
		if(!bMoving)
		{
			PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
		}
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"LOGO_UP_TRY_CATCH : [%s]",__FUNCTION__);
	}


	CDialog::OnLButtonDown(nFlags, point);
}

void CLogoUpload::OnLButtonUp(UINT nFlags, CPoint point)
{
	bMoving=false;
	CDialog::OnLButtonUp(nFlags, point);
}


/*
Function	:OnBnClickedUpload
Notes		:点击上传文件，需要做的工作：
1、截图
2、计算文件的MD5值
3、写入本地文件中
Author		:Fred Huang
Date		:2008-03-18
Parameter	:na
return		:void
*/
void CLogoUpload::OnBnClickedUpload()
{
	try
	{
		if(!img)
			return;
		if(img->GetFlags()==0)
			return;

		//建立一个新的Image对象以保存切割图片
		CImage imageSave;
		imageSave.Create(LARGE_WIDTH,LARGE_HEIGHT,16,BI_RGB);
		HDC hdc=imageSave.GetDC();
		int w=img->GetWidth()>LARGE_WIDTH?LARGE_WIDTH:img->GetWidth();
		int h=img->GetHeight()>LARGE_HEIGHT?LARGE_HEIGHT:img->GetHeight();

		int l=w<LARGE_WIDTH?(LARGE_WIDTH-w)/2:0;
		int t=h<LARGE_HEIGHT?(LARGE_HEIGHT-h)/2:0;

		Graphics g(hdc);
		//保存图片
		CString tempImage;
		tempImage.Format(TEXT("%s\\CustomFace\\TempFace.png"),CBcfFile::GetAppPath());
		if(m_cutType==0)
			g.DrawImage(img,Rect(l,t,w,h),x,y,w,h,UnitPixel);
		//m_image.Draw(hdc,l,t,w,h,x,y,w,h);
		else
		{
			g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
			g.DrawImage(img,Rect(0,0,LARGE_WIDTH,LARGE_HEIGHT),0,0,img->GetWidth(),img->GetHeight(),UnitPixel);
		}
		g.ReleaseHDC(hdc);
		//	m_image.Draw(hdc,0,0,LARGE_WIDTH,LARGE_HEIGHT,0,0,m_image.GetWidth(),m_image.GetHeight());
		//m_image.Draw(hdc,l,t,w,h,x,y,w,h);
		imageSave.Save(tempImage,Gdiplus::ImageFormatPNG);//Gdiplus::ImageFormatBMP);
		imageSave.ReleaseDC();
		imageSave.Destroy();

		//以下计算图片文件的MD5值
		CFile fr(tempImage,CFile::modeRead);
		int len=fr.GetLength();
		byte *buf=new byte[len+1];
		ZeroMemory(buf,len+1);
		fr.Read(buf,len);
		fr.Close();

		//	MD5_CTX md5;
		//	md5.MD5Update(buf,len);
		//	ZeroMemory(md5filename,32);
		//	md5.MD5Final(md5filename);

		CMainRoomEx *pWnd=(CMainRoomEx*)pParentWnd;
		CString md5file;
		//获取当前模块名称
		TCHAR sz[200];
		GetModuleFileName(AfxGetInstanceHandle(),sz,sizeof(sz));
		char *p = strrchr(sz,'\\');
		p++;
		*p = '\0';
		//
		md5file.Format("%sCustomFace\\%d.png",sz,pWnd->m_PlaceUserInfo.dwUserID);

		CFile fw(md5file,CFile::modeCreate|CFile::modeWrite|CFile::modeNoInherit);

		fw.Write(buf,len);

		fw.Close();
		delete buf;

		uploadCustomFace(0);
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"LOGO_UP_TRY_CATCH : [%s]",__FUNCTION__);
	}

	return ;
}

/*
Function	:uploadCustomFace
Notes		:正式上传文件，分块上传
Author		:Fred Huang
Date		:2008-03-18
Parameter	:
int	nBlockIndex	文件块的索引
return		:void
*/
bool CLogoUpload::uploadCustomFace(int nBlockIndex)
{
	try
	{
		CMainRoomEx *pWnd=(CMainRoomEx*)pParentWnd;
		if(!pParentWnd)
		{
			CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
			
			//AFCMessageBox(fMsg.GetKeyVal("LogoUploadDlg","SystemError","系统错误"),fMsg.GetKeyVal("LogoUploadDlg","Tip","提示"));
	        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,fMsg.GetKeyVal("LogoUploadDlg","Tip","提示"),fMsg.GetKeyVal("LogoUploadDlg","SystemError","系统错误"));
			return false;
		}
		//获取当前模块名称
		TCHAR sz[200];
		GetModuleFileName(AfxGetInstanceHandle(),sz,sizeof(sz));
		char *p = strrchr(sz,'\\');
		p++;
		*p = '\0';

		CString logoFile;
		logoFile.Format("%sCustomFace\\%d.png",sz,pWnd->m_PlaceUserInfo.dwUserID);

		char buf[513]={0};
		CFile fr(logoFile,CFile::modeRead);
		int filelen=fr.GetLength();			//文件大小
		int packagesize=512;				//数据块大小
		if(nBlockIndex*512>filelen)			//是否超出了文件的结尾
		{
			fr.Close();
			return false;
		}
		if(nBlockIndex*512+packagesize>filelen)		//最后一块大小是否超出了文件的结尾
			packagesize=filelen-nBlockIndex*512;	//最不足块大小的数据

		fr.Seek(nBlockIndex*512,CFile::begin);		//文件定位
		fr.Read(buf,packagesize);					//读数据
		fr.Close();									//关闭文件

		//填充数据结构
		MSG_UL_C_UPLOAD msg;
		ZeroMemory(&msg,sizeof(MSG_UL_C_UPLOAD));
		msg.dwUserID=pWnd->m_PlaceUserInfo.dwUserID;	//用户ID
		msg.dwUserSourceLogo=pWnd->m_PlaceUserInfo.bLogoID%0x100;
		//if(pWnd->m_PlaceUserInfo.bLogoID%0x100)

		msg.nBlockIndex=nBlockIndex;					//数据块索引
		msg.nBlockSize=512;								//数据块大小
		msg.nPackageSize=packagesize;					//数据包的大小
		msg.nFileSize=filelen;							//文件大小
		msg.nBlockCount=(filelen-1)/512+1;				//数据块数量
		//	memcpy(msg.szLogoFile,md5filename,16);			//文件名称
		memcpy(msg.szData,buf,packagesize);				//数据

		if(nBlockIndex==0)
			m_progress.SetRange(0,msg.nBlockCount);

		pWnd->SendData(&msg,sizeof(msg),MDM_GR_USER_LOGO,ASS_ULC_UPLOAD,0);

	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"LOGO_UP_TRY_CATCH : [%s]",__FUNCTION__);
	}

	//	if(nBlockIndex<msg.nBlockCount-1)
	//		uploadCustomFace(nBlockIndex+1);
	return false;
}

void CLogoUpload::SetParentWnd(CWnd* pWnd)
{
	pParentWnd=pWnd;
}
/*
Function	:OnNetMessage
Notes		:正式上传文件，分块上传服务器传回来的消息
Author		:Fred Huang
Date		:2008-03-18
Parameter	:
NetMessageHead	:网络消息头
void *			:数据指针
UINT			:数据包的大小
return		:void
*/
void CLogoUpload::OnNetMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize)
{
	try
	{
		switch(pNetHead->bAssistantID)
		{
		case ASS_ULS_UPLOADSUCCEED:
			OnUploadSucceed(pNetData);
			break;
		case ASS_ULS_UPLOADFAILED:
			{
			CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
			
			//AFCMessageBox(fMsg.GetKeyVal("LogoUploadDlg","UploadPicFailed","对不起，自定义头像上传失败，请您稍候再试！"),fMsg.GetKeyVal("LogoUploadDlg","Error","错误"));
	         DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,fMsg.GetKeyVal("LogoUploadDlg","Error","错误"),fMsg.GetKeyVal("LogoUploadDlg","UploadPicFailed","对不起，自定义头像上传失败，请您稍候再试！"));
			OnBnClickedCancel();
			}
			break;
		case ASS_ULS_LOGOINFORMATION:
			OnCheckLocalLogo(pNetData);
			break;
		case ASS_ULS_DOWN:
			OnDownloadFace(pNetData);
			break;
		}
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"LOGO_UP_TRY_CATCH : [%s]",__FUNCTION__);
	}

	return;
}

/*
Function	:OnUploadSucceed
Notes		:服务器返回上传成功，但要注意的是这里只是说明了上传了一个文件块成功，而不是整个文件上传成功，所以还要对是否是最后一块数据上传的反馈
Author		:Fred Huang
Date		:2008-03-18
Parameter	:
void *			:返回数据
return		:void
*/
void CLogoUpload::OnUploadSucceed(void *pData)
{
	try
	{
		MSG_UL_S_UPLOADRESULT * msg=(MSG_UL_S_UPLOADRESULT*)pData;
		CMainRoomEx * pWnd=(CMainRoomEx*)pParentWnd;
		if(msg->dwUserID!=pWnd->m_PlaceUserInfo.dwUserID)//不是给自己的消息
			return;
		if(msg->nBlockIndex==msg->nBlockCount-1)
		{
			CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
			m_progress.SetPos(msg->nBlockCount);
			if(msg->bNeedCheck)
			{
				//AFCMessageBox(fMsg.GetKeyVal("LogoUploadDlg","UploadPicSuccessWait","自定义头像已上传完毕\n\n在管理员审核通过后重新登录大厅生效！"),fMsg.GetKeyVal("LogoUploadDlg","Tip","提示"));//文件已上传完成
	            DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,fMsg.GetKeyVal("LogoUploadDlg","Tip","提示"),fMsg.GetKeyVal("LogoUploadDlg","UploadPicSuccessWait","自定义头像已上传完毕\n\n在管理员审核通过后重新登录大厅生效！"));
			}
			else
			{
				//AFCMessageBox(fMsg.GetKeyVal("LogoUploadDlg","UploadPicSuccess","自定义头像已上传完毕！"),fMsg.GetKeyVal("LogoUploadDlg","Tip","提示"));//文件已上传完成
	            DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,fMsg.GetKeyVal("LogoUploadDlg","Tip","提示"),fMsg.GetKeyVal("LogoUploadDlg","UploadPicSuccess","自定义头像已上传完毕！"));
				::PostMessage(theApp.m_pMainWnd->m_hWnd,WM_USER+0x10000,0,0);
			}		
			OnBnClickedCancel();
		}
		else
		{
			m_progress.SetPos(msg->nBlockIndex++);
			uploadCustomFace(msg->nBlockIndex);//继续上传后续的内容
		}
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"LOGO_UP_TRY_CATCH : [%s]",__FUNCTION__);
	}

}

/*
Function	:checkUserLogo
Notes		:向服务器询问用户自定义头像的相关数据，从服务器需要返回用户头像文件的MD5值
Author		:Fred Huang
Date		:2008-03-19
Parameter	:
int			:用户的ID号，自己的头像也要这样判断，主要是为了处理从不同的电脑登录
return		:void
*/
void CLogoUpload::checkUserLogo(int dwUserID)
{
	try
	{
		MSG_UL_C_INFORREQUEST msg;
		ZeroMemory(&msg,sizeof(MSG_UL_C_INFORREQUEST));
		msg.dwRequestUserID=dwUserID;
		CMainRoomEx * pWnd=(CMainRoomEx*)pParentWnd;		//被请求者的ID
		msg.dwUserID=pWnd->m_PlaceUserInfo.dwUserID;		//请求者的ID，即自己的ID

		pWnd->SendData(&msg,sizeof(MSG_UL_C_INFORREQUEST),MDM_GR_USER_LOGO,ASS_ULC_INFOREQUEST,0);

	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"LOGO_UP_TRY_CATCH : [%s]",__FUNCTION__);
	}

}

/*
Function	:OnCheckLocalLogo
Notes		:收到服务器返回的头像文件MD5，和本地的进行匹配，如果不一样，则请求从服务器下载
Author		:Fred Huang
Date		:2008-03-19
Parameter	:
void*		:从服务器返回的数据指针，指向结构体 MSG_UL_S_INFORMATION
return		:void
*/

void CLogoUpload::OnCheckLocalLogo(void * pData)
{
	try
	{
		MSG_UL_S_INFORMATION *UL_Info=(MSG_UL_S_INFORMATION*)pData;

		CString strmsg;

		strmsg.Format("dwUserID=%d LogoMD5Server=%s dwUserLogoID=%d\r\n",UL_Info->dwUserID,UL_Info->szFileMD5,UL_Info->dwUserLogoID);
		OutputDebugString(strmsg);

		if(UL_Info->dwUserLogoID<0xFF || UL_Info->dwUserLogoID>0x1FF)
			return;
		CString LogoMD5Server=UL_Info->szFileMD5;
		if(LogoMD5Server=="")
			return;

		bool bNeedDownload=false;//是否需要下载新的文件
		CString logoFile;
		logoFile.Format("%sCustomFace\\%d.png",CBcfFile::GetAppPath(),UL_Info->dwUserID);
		CFileFind ff;
		if(!ff.FindFile(logoFile))
			bNeedDownload=true;	//本地文件未找到，直接需要请求下载新的文件
		ff.Close();
		if(!bNeedDownload)
		{//本地存在文件，检查MD5值是否一样
			CFile fr(logoFile,CFile::modeRead);
			int len=fr.GetLength();
			byte *buf=new byte[len+1];
			ZeroMemory(buf,len+1);
			fr.Read(buf,len);
			fr.Close();

			MD5_CTX md5;
			md5.MD5Update(buf,len);
			byte *md5buf=new byte[16];
			md5.MD5Final(md5buf);

			CString md5str="",szchar;
			for(int i=0;i<16;i++)
			{
				szchar.Format("%2x",md5buf[i]);
				md5str+=szchar;
			}

			delete buf;
			delete md5buf;

			if(md5str!=LogoMD5Server)
				bNeedDownload=true;
		}

		if(!bNeedDownload)
			return;
		strmsg.Format("ASS_ULC_DOWNREQUEST\r\n");
		OutputDebugString(strmsg);

		//发送下载头像请求
		MSG_UL_C_DOWNREQUEST msg;
		ZeroMemory(&msg,sizeof(MSG_UL_C_DOWNREQUEST));
		msg.dwRequestUserID=UL_Info->dwUserID;
		CMainRoomEx * pWnd=(CMainRoomEx*)pParentWnd;		//被请求者的ID
		msg.dwUserID=pWnd->m_PlaceUserInfo.dwUserID;		//请求者的ID，即自己的ID

		pWnd->SendData(&msg,sizeof(MSG_UL_C_DOWNREQUEST),MDM_GR_USER_LOGO,ASS_ULC_DOWNREQUEST,0);
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"LOGO_UP_TRY_CATCH : [%s]",__FUNCTION__);
	}

	return;
}

/*
Function	:OnDownloadFace
Notes		:收到服务器返回的头像数据，直接写入文件中
Author		:Fred Huang
Date		:2008-03-19
Parameter	:
void*		:从服务器返回的数据指针，指向结构体 MSG_UL_S_DOWN
return		:void
*/
void CLogoUpload::OnDownloadFace(void * pData)
{
	try
	{
		MSG_UL_S_DOWN * msg=(MSG_UL_S_DOWN *)pData;

		CString savePath;
		savePath.Format("%s\\CustomFace\\%d.png",CBcfFile::GetAppPath(),msg->dwUserID);

		UINT dwFlag=CFile::modeWrite;
		if(msg->nBlockIndex==0)
			dwFlag|=CFile::modeCreate;

		MSG_UL_C_DOWNRESULT resultMsg;
		ZeroMemory(&resultMsg,sizeof(MSG_UL_C_DOWNRESULT));
		resultMsg.dwUserID=msg->dwUserID;
		resultMsg.nBlockCount=msg->nBlockCount;
		resultMsg.nBlockIndex=msg->nBlockIndex;
		try
		{
			CFile fw(savePath,dwFlag);
			fw.Seek(msg->nBlockIndex*512,CFile::begin);
			fw.Write(msg->szData,msg->nPackageSize);
			fw.Close();
		}
		catch (...) {
			return;
		}

		//	pWnd->SendData(uIndex,&resultMsg,sizeof(MSG_UL_S_UPLOADRESULT),MDM_GR_USER_LOGO,ASS_ULS_UPLOADSUCCEED,dwHandleID,0);
		TRACE("Receive Custom Logo File %d of %d at %s\n",msg->nBlockIndex,msg->nBlockCount,savePath);
		if(msg->nBlockIndex<msg->nBlockCount-1)
		{
			CMainRoomEx * pWnd=(CMainRoomEx*)pParentWnd;		//被请求者的ID
			pWnd->SendData(&resultMsg,sizeof(MSG_UL_C_DOWNRESULT),MDM_GR_USER_LOGO,ASS_ULC_DOWNRESULT,0);

		}
		else
		{
			TRACE("Download Receive Custom Logo Completed\n");
			//接收完成了
			CMainRoomEx *pWnd=(CMainRoomEx*)pParentWnd;
			if(msg->dwUserID==pWnd->m_PlaceUserInfo.dwUserID)
			{
				::PostMessage(theApp.m_pMainWnd->m_hWnd,WM_USER+0x10000,0,0);
			}
		}
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"LOGO_UP_TRY_CATCH : [%s]",__FUNCTION__);
	}

}

void CLogoUpload::drawSkin(void)
{
	try
	{
		if(!imgDialog)
			return;
		CDC *pdc=this->GetWindowDC();
		if(!pdc)return;
		CRect rc;
		this->GetWindowRect(&rc);
		Graphics g(pdc->m_hDC);
		int topHeight=GetSystemMetrics(SM_CYFRAME)+GetSystemMetrics(SM_CYSIZE)+6;
		g.DrawImage(imgDialog,RectF(0.0f,0.0f,10.0f,topHeight),0.0f,0.0f,10.0f,topHeight,UnitPixel);
		g.DrawImage(imgDialog,RectF(10.0f,0.0f,rc.Width()-20,topHeight),9.0f,0.0f,1.0f,topHeight,UnitPixel);
		g.DrawImage(imgDialog,RectF((REAL)rc.Width()-10,0.0f,10.0f,topHeight),15.0f,0.0f,10.0f,topHeight,UnitPixel);
		m_bkcolor=pdc->GetPixel(9,topHeight-1);

		int bottomHeight=GetSystemMetrics(SM_CYFRAME)+3;
		g.DrawImage(imgDialog,RectF(0.0f,topHeight,GetSystemMetrics(SM_CXFRAME)+5.0f,rc.Height()-topHeight-bottomHeight),0.0f,30.0f,GetSystemMetrics(SM_CXFRAME)+5.0f,60.0f,UnitPixel);
		g.DrawImage(imgDialog,RectF(rc.Width()-GetSystemMetrics(SM_CXFRAME)-5.0f,topHeight,GetSystemMetrics(SM_CXFRAME)+5.0f,rc.Height()-topHeight-bottomHeight),20-GetSystemMetrics(SM_CXFRAME),30.0f,GetSystemMetrics(SM_CXFRAME)+5.0f,60.0f,UnitPixel);
		//g.DrawImage(imgDialog,RectF(rc.Width()-15,30,15,rc.Height()-40),10,30,15,60,UnitPixel);

		g.DrawImage(imgDialog,RectF(0,rc.Height()-bottomHeight,10,bottomHeight),0,100-bottomHeight,10,bottomHeight,UnitPixel);
		g.DrawImage(imgDialog,RectF(10,rc.Height()-bottomHeight,rc.Width()-20,bottomHeight),10,100-bottomHeight,1,bottomHeight,UnitPixel);
		g.DrawImage(imgDialog,RectF(rc.Width()-10,rc.Height()-bottomHeight,10,bottomHeight),15,100-bottomHeight,10,bottomHeight,UnitPixel);

		CString s="宋体";

		Font font(s.AllocSysString(),12,0,UnitPixel);
		SolidBrush brush(Color(255,255,255));

		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
		s = fMsg.GetKeyVal("LogoUploadDlg","UploadYourPic","自定义头像上传");
		g.DrawString(s.AllocSysString(),-1,&font,PointF(5,5),0,&brush);
		g.ReleaseHDC(pdc->m_hDC);
		m_bnClose.Invalidate(FALSE);
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"LOGO_UP_TRY_CATCH : [%s]",__FUNCTION__);
	}

}

void CLogoUpload::setSkinIndex(int sk)
{
	try
	{
		if(sk>=0 && sk<4)
			m_skinIndex=sk;
		else
			m_skinIndex=0;
		
		CString s;
		s.Format("%s\\%sdialog\\dialog.bmp",CBcfFile::GetAppPath(true),m_skinmgr.GetSkinPath());
		//	s.Format("%s\\image\\skin%d\\dialog\\dialog.bmp",CBcfFile::GetAppPath(true),m_skinIndex);

		

#ifndef BZ_ZLIB  //< add by wxx 
		imgDialog=Image::FromFile(s.AllocSysString());
#else
		CUnZipRes unZipRes;
		IStream * pStream=NULL;
		TCHAR * pchar=s.GetBuffer(s.GetLength()+1);
		pStream=unZipRes.LoadFileResFromZip(pchar);

		imgDialog=new Image(pStream);
		if(pStream!=NULL)
		{
			pStream->Release();
			pStream=NULL;
		}

#endif

		TCHAR path[MAX_PATH];

		wsprintf(path,"%s\\%sdialog\\log_exit_bt.bmp",CBcfFile::GetAppPath(true),m_skinmgr.GetSkinPath());//win_close.bmp
		m_bnClose.LoadButtonBitmap(path,false);
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"LOGO_UP_TRY_CATCH : [%s]",__FUNCTION__);
	}


	//m_bkcolor=RGB(44,114,233);
}

void CLogoUpload::OnBnClickedCloselogo()
{
	CDialog::OnCancel();
}

LRESULT CLogoUpload::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT lrst=CDialog::DefWindowProc(message, wParam, lParam);

	if (!::IsWindow(m_hWnd))
		return lrst;

	if (message==WM_MOVE||message==WM_PAINT||message==WM_NCPAINT||message==WM_NCACTIVATE)// ||message == WM_NOTIFY)
	{
		drawSkin();
	}
	return lrst;
}

LRESULT CLogoUpload::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		//case WM_CTLCOLORSCROLLBAR :
		//case WM_CTLCOLORBTN:
		//case WM_CTLCOLORMSGBOX:
		//case WM_CTLCOLOREDIT:
		//case WM_CTLCOLORLISTBOX:
	case WM_CTLCOLORDLG:
	case WM_CTLCOLORSTATIC :
		SetTextColor((HDC)wParam, RGB(0,0,0)) ;
		SetBkColor((HDC)wParam, m_bkcolor);
		if (m_bkBrush == NULL) m_bkBrush = CreateSolidBrush(m_bkcolor); 
		return (LRESULT)m_bkBrush;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void CLogoUpload::OnBnClickedPiccut()
{
	m_cutType=0;
	Invalidate(TRUE);
}

void CLogoUpload::OnBnClickedPicscale()
{
	m_cutType=1;
	Invalidate(TRUE);
}
LRESULT CLogoUpload::OnExchangeSkin(WPARAM wpara,LPARAM lpara)
{
	//初始化
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + strSkin);
	CString key= m_skinmgr.GetKeyVal(strSkin);
	int r,g,b;
	r=f.GetKeyVal(key,"logobgcr",44);
	g=f.GetKeyVal(key,"logobgcg",114);
	b=f.GetKeyVal(key,"logobgcb",233);
	m_bkcolor=RGB(r,g,b);

	LoadSkin();

	//画图部分
	drawSkin();
	drawImage();
	//OnPaint();
	Invalidate();
	return LRESULT(0);
}
