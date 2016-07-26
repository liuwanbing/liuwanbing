/// lookinfo.cpp : 实现文件
///

#include "stdafx.h"
#include "lookinfo.h"

void MsgPrint(const char *lpszFormat, ...);


/// Clookinfo 对话框

IMPLEMENT_DYNAMIC(Clookinfo, CDialog)
///名称：Clookinfo
///描述：路子对话框构造函数，载入图片，初始化数据
///@param 
///@return 
Clookinfo::Clookinfo(CWnd* pParent /*=NULL*/): CDialog(Clookinfo::IDD, pParent)
{
	// 下注的背景框
	TCHAR path[200];
	TCHAR skin[MAX_NAME_INFO];
	wsprintf(path,TEXT(".\\image\\luzi\\luzi_bk.bmp"));	//载入看路子背景图
	m_bk.SetLoadInfo(path,CGameImageLink::m_bAutoLock);


//////////////////////////////////////////////////////////

	wsprintf(path,TEXT(".\\image\\luzi\\zhuang_dot.bmp"));	// 庄小图
	m_imgzhuangduizi.SetLoadInfo(path,CGameImageLink::m_bAutoLock);

	wsprintf(path,TEXT(".\\image\\luzi\\xian_dot.bmp"));	// 闲小图
	m_imgxianduizi.SetLoadInfo(path,CGameImageLink::m_bAutoLock);

	wsprintf(path,TEXT(".\\image\\luzi\\he_dot.bmp"));		// 和小图
	m_imgheduizi.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	
	wsprintf(path,TEXT(".\\image\\luzi\\zhuang1_dot.bmp"));	// 庄小图
	m_imgzhuang1_dot.SetLoadInfo(path,CGameImageLink::m_bAutoLock);

	wsprintf(path,TEXT(".\\image\\luzi\\xian1_dot.bmp"));	// 闲小图
	m_imgxian1_dot.SetLoadInfo(path,CGameImageLink::m_bAutoLock);

	wsprintf(path,TEXT(".\\image\\luzi\\he1_dot.bmp"));		// 和小图
	m_imghe1_dot.SetLoadInfo(path,CGameImageLink::m_bAutoLock);


	//////////////////////////////////////////////

	wsprintf(path,TEXT(".\\image\\luzi\\zhuang.bmp"));	// 庄小图
	m_imgzhuang.SetLoadInfo(path,CGameImageLink::m_bAutoLock);

	wsprintf(path,TEXT(".\\image\\luzi\\zhuang1.bmp"));	// 庄大图
	m_imgzhuang1.SetLoadInfo(path,CGameImageLink::m_bAutoLock);

	wsprintf(path,TEXT(".\\image\\luzi\\xian.bmp"));	// 闲小图
	m_imgxian.SetLoadInfo(path,CGameImageLink::m_bAutoLock);

	wsprintf(path,TEXT(".\\image\\luzi\\xian1.bmp"));	// 闲大图
	m_imgxian1.SetLoadInfo(path,CGameImageLink::m_bAutoLock);

	wsprintf(path,TEXT(".\\image\\luzi\\he.bmp"));		// 和小图
	m_imghe.SetLoadInfo(path,CGameImageLink::m_bAutoLock);

	wsprintf(path,TEXT(".\\image\\luzi\\he1.bmp"));		// 和大图
	m_imghe1.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
		
	m_BtnYiye;				// 第一页
	m_BtnErye;				// 第二页图片

	m_iZwin =0 ;//庄家赢的百分比，为了用整数保存放大100被，绘制是前后两为用小数点隔开
	m_iXwin =0 ;//闲家赢的百分比
	m_iHwin =0 ;//和赢的百分比
	m_iZDuizi =0 ;//庄对子
	m_iXDuizi =0 ;//闲对子
	m_iZTianwan =0 ;//庄天王
	m_iXTianwang =0 ;//闲天王
	m_iPlayCount=0;

#ifdef SET72 

	m_pDaTu.SetPoint(49,216);//大图基础位置
	m_pXiaoTu.SetPoint(17,93);//小图基础位置

	m_iDaWidth = 32;//大图距离
	m_iXiaoWidth = 16;//小图距离

#else
	m_pDaTu.SetPoint(31,198);//大图基础位置
	m_pXiaoTu.SetPoint(32,95);//小图基础位置

	m_iDaWidth = 31;//大图距离
	m_iXiaoWidth = 16;//小图距离

#endif

	///是否显示下一页的图片
	m_bShowNext = false;
	m_iNowJuShu = 0;	//本局进行的局数，进行到第几局（72把一局）

	CString s = CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f( s + SKIN_FOLDER + _T(".bcf"));
	CString strKey = TEXT("config");
	CPoint ptPoint = CPoint(0,0);
	TCHAR szValueName[_MAX_PATH] = {_T("GamePictureType")};
	m_iGamePictureType = f.GetKeyVal(strKey, szValueName, 0);

	m_iLuziCol = 0;
}

Clookinfo::~Clookinfo()
{
}

void Clookinfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDOK, m_BmCancel);
	//DDX_Control(pDX, IDCANCEL, m_BmOk);
	DDX_Control(pDX, IDC_BUT_YIYE, m_BtnYiye);
	DDX_Control(pDX, IDC_BUT_ERYE, m_BtnErye);
	DDX_Control(pDX, IDC_BUT_CLOSE, m_BtnClose);
}


BEGIN_MESSAGE_MAP(Clookinfo, CDialog)
	ON_WM_PAINT()
	//ON_BN_CLICKED(IDOK, &Clookinfo::OnBnClickedOk)
	//ON_BN_CLICKED(IDCANCEL, &Clookinfo::OnBnClickedCancel)
	ON_WM_ERASEBKGND()
//	ON_WM_WINDOWPOSCHANGED()
ON_WM_WINDOWPOSCHANGING()
ON_WM_WINDOWPOSCHANGED()
ON_WM_LBUTTONDOWN()
ON_BN_CLICKED(IDC_BUT_YIYE, &Clookinfo::OnBnClickedButYiye)
ON_BN_CLICKED(IDC_BUT_ERYE, &Clookinfo::OnBnClickedButErye)
ON_BN_CLICKED(IDC_BUT_CLOSE, &Clookinfo::OnBnClickedButClose)
ON_WM_CREATE()
END_MESSAGE_MAP()

///名称：OnInitDialog
///描述：初始化函数，设置按钮的形状，移动按钮位置
///@param 
///@return 
BOOL Clookinfo::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CGameImageHelper ImageHandle(&m_bk);
	HRGN hRgn=AFCBmpToRgn(ImageHandle,RGB(255,0,255),RGB(1,0,1));

	// 设置窗口形状
	if (hRgn!=NULL)
	{
	//	SetWindowPos(NULL,0,0,ImageHandle.GetWidth(),ImageHandle.GetHeight(),SWP_NOSIZE);
		SetWindowRgn(hRgn,TRUE);
	}

	CRect ClientRect;
	GetClientRect(&ClientRect);

	CGameImage m_tempbk;
	HRGN hRgn2;
	TCHAR szName[MAX_PATH]={0};
	CRect rect;

	TCHAR skin[MAX_NAME_INFO];

	wsprintf(szName,".\\image\\luzi\\diyiye.bmp");	//第一页按钮
	m_BtnYiye.LoadButtonBitmap(szName,false);	
	m_tempbk.SetLoadInfo(szName,true);
	hRgn2=AFCBmpToRgn(m_tempbk,RGB(255,0,255),RGB(1,1,1));
	if(hRgn2)
		m_BtnYiye.SetWindowRgn(hRgn2,true);

	wsprintf(szName,".\\image\\luzi\\dierye.bmp");	//第二页按钮
	m_BtnErye.LoadButtonBitmap(szName,false);	
	m_tempbk.SetLoadInfo(szName,true);
	hRgn2=AFCBmpToRgn(m_tempbk,RGB(255,0,255),RGB(1,1,1));
	if(hRgn2)
		m_BtnErye.SetWindowRgn(hRgn2,true);
	
	wsprintf(szName,".\\image\\luzi\\game_close_bt.bmp");	//关闭按钮
	m_BtnClose.LoadButtonBitmap(szName,false);	
	m_tempbk.SetLoadInfo(szName,true);
	hRgn2=AFCBmpToRgn(m_tempbk,RGB(255,0,255),RGB(1,1,1));
	if(hRgn2)
		m_BtnClose.SetWindowRgn(hRgn2,true);

	this->MapWindowPoints(this->GetParent(),&ClientRect);
	CGameImageHelper	help(&m_bk);
	this->MoveWindow(ClientRect.left,ClientRect.top,help.GetWidth(),help.GetHeight());
	CenterWindow();	

	m_BtnYiye.GetClientRect(&rect);
	m_BtnYiye.MoveWindow(m_pXiaoTu.x,m_pXiaoTu.y-rect.Height()-2,rect.Width(),rect.Height());
	m_BtnYiye.ShowWindow(true);

	m_BtnErye.GetClientRect(&rect);
	m_BtnErye.MoveWindow(m_pXiaoTu.x+rect.Width(),m_pXiaoTu.y-rect.Height()-2,rect.Width(),rect.Height());
	m_BtnErye.ShowWindow(true);   

	CRect iRect;
	GetClientRect(&iRect);
	m_BtnClose.MoveWindow(iRect.Width()-27,5,20,16);

	//m_BmCancel.EnableWindow(TRUE);		
	//m_BmOk.EnableWindow(TRUE);
	return TRUE;
}
int Clookinfo::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	// TODO:  在此添加您专用的创建代码	

	return 0;
}
///消息解释
BOOL Clookinfo::PreTranslateMessage(MSG * pMsg)
{
	if(pMsg->message==WM_LBUTTONUP)
		bMouseDown=false;
	return __super::PreTranslateMessage(pMsg);
}


///名称：OnPaint
///描述：路子对话框绘图按钮
///@param 
///@return 
void Clookinfo::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()
	CGameImageHelper	bk(&m_bk);
	bk.BitBlt(dc.GetSafeHdc(),0,0);
//////////////////////////
	CDC*pDC;
	pDC = GetDC();	
	pDC->SetTextColor(RGB(0,0,0));
	pDC->SetBkMode(TRANSPARENT);
//绘制72把一局的路子lym2010-03-18
#ifdef SET72 

	DrawLuzi(pDC);

#else

	CRect  rectName(0,0,0,0);
///绘制输赢比例信息
	///庄赢比例
	if(m_iPlayCount<=0)
	{
		m_iPlayCount=1;
	}
	else if(m_iPlayCount>1)
	{
		m_iPlayCount-=1;
	}
	int count =0;
	count=m_iZwin+m_iXwin+m_iHwin;
	if(count<=0)
		count=1;
	int temp=0;
	CString stb="",stp="",str="";
	temp = m_iZwin*100/count;
	stp.Format("%d",temp);//前两位
	temp = m_iZwin*100*100/count;
	temp %= 100;//放大100倍,再取后面的数 
	stb.Format("%d",temp);//前两位
	str=stp+"."+stb+"%";
	//输出str;
	//pDC->DrawText(str,str.GetLength(),&rectName,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
	rectName.SetRect(435,44,485,60);
	pDC->DrawText(str,str.GetLength(),&rectName,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
	str.Format("%d",m_iZwin);//庄赢了几局
	//输出str;	
	rectName.SetRect(400,44,426,60);
	pDC->DrawText(str,str.GetLength(),&rectName,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

	///闲赢比例
	temp = m_iXwin*100/count;
	stp.Format("%d",temp);//前两位
	temp = m_iXwin*100*100/count;
	temp %= 100;//放大100倍,再取后面的数 
	stb.Format("%d",temp);//前两位
	str=stp+"."+stb+"%";;
	//输出str;
	rectName.SetRect(105,44,155,60);
	pDC->DrawText(str,str.GetLength(),&rectName,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
	str.Format("%d",m_iXwin);//庄赢了几局
	//输出str;
	rectName.SetRect(65,44,95,60);
	pDC->DrawText(str,str.GetLength(),&rectName,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

	///和赢的比例
	temp = m_iHwin*100/count;
	stp.Format("%d",temp);//前两位
	temp = m_iHwin*100*100/count;
	temp %= 100;//放大100倍,再取后面的数 
	stb.Format("%d",temp);//前两位
	str=stp+"."+stb+"%";;
	///输出str;
	rectName.SetRect(275,44,325,60);
	pDC->DrawText(str,str.GetLength(),&rectName,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
	str.Format("%d",m_iHwin);//庄赢了几局
	///输出str;
	rectName.SetRect(240,44,265,60);
	pDC->DrawText(str,str.GetLength(),&rectName,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

	///闲对子
	rectName.SetRect(95,75,130,90);
	str.Format("%d",m_iXDuizi);//庄赢了几局
	pDC->DrawText(str,str.GetLength(),&rectName,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

	///庄对子
	rectName.SetRect(215,75,265,90);
	str.Format("%d",m_iZDuizi);//庄赢了几局
	pDC->DrawText(str,str.GetLength(),&rectName,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

	///闲天王
	rectName.SetRect(355,75,385,90);
	str.Format("%d",m_iXTianwang);//庄赢了几局
	pDC->DrawText(str,str.GetLength(),&rectName,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

	///庄天王
	rectName.SetRect(450,75,490,90);
	str.Format("%d",m_iZTianwan);//庄赢了几局
	pDC->DrawText(str,str.GetLength(),&rectName,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

	CGameImageHelper	zhuang(&m_imgzhuang);
	CGameImageHelper	xian(&m_imgxian);
	CGameImageHelper	he(&m_imghe);
	
///粘贴上方路子信息
	int numx=0,numy=0,type=0;
	for(int i=0;i<count;i++)
	{
		if(m_iWinType[i] != type && type!=0 && m_iWinType[i]!=3)
		{
			type = m_iWinType[i];
			numx++;//换列
			numy=0;
		}
		else if(m_iWinType[i] != type && type==0)
		{
			type = m_iWinType[i];
		}
		else
		{
			numy++;//行
		}
		if(numy==6)
		{
			numy=0;
			numx++;
		}

		//根据numy，numx黏贴图片
		if(m_iWinType[i]==1 )//开庄
		{
			zhuang.BitBlt(dc.GetSafeHdc(),m_pXiaoTu.x + numx*m_iXiaoWidth,m_pXiaoTu.y + numy*m_iXiaoWidth);
		}
		else if(m_iWinType[i]==2)//开闲
		{
			xian.BitBlt(dc.GetSafeHdc(),m_pXiaoTu.x + numx*m_iXiaoWidth,m_pXiaoTu.y + numy*m_iXiaoWidth);
		}
		else if(m_iWinType[i]==3)//开和
		{
			he.BitBlt(dc.GetSafeHdc(),m_pXiaoTu.x + numx*m_iXiaoWidth,m_pXiaoTu.y + numy*m_iXiaoWidth);
		}
	}
	CGameImageHelper	zhuang1(&m_imgzhuang1);
	CGameImageHelper	xian1(&m_imgxian1);
	CGameImageHelper	he1(&m_imghe1);
///粘贴下方路子信息
	numy=0;
	numx=0;
	for(int i=0;i<count;i++)
	{
		numy = i%6;
		numx = i/6;
		//根据m_iWinType[i]的值贴图
		if(m_iWinType[i]==1)//开庄
		{
			zhuang1.BitBlt(dc.GetSafeHdc(),m_pDaTu.x + numx*m_iDaWidth,m_pDaTu.y + numy*m_iDaWidth);
		}
		else if(m_iWinType[i]==2)//开闲
		{
			xian1.BitBlt(dc.GetSafeHdc(),m_pDaTu.x + numx*m_iDaWidth,m_pDaTu.y + numy*m_iDaWidth);
		}
		else if(m_iWinType[i]==3)//开和
		{
			he1.BitBlt(dc.GetSafeHdc(),m_pDaTu.x + numx*m_iDaWidth,m_pDaTu.y + numy*m_iDaWidth);
		}
	}
#endif
	pDC->DeleteDC();

}

///名称：Init
///描述：初始化数据，庄闲开牌信息和百分比
///@param 
///@return 
void Clookinfo::Init()
{
	m_iZwin =0 ;//庄家赢的百分比，为了用整数保存放大100被，绘制是前后两为用小数点隔开
	m_iXwin =0 ;//闲家赢的百分比
	m_iHwin =0 ;//和赢的百分比
	m_iZDuizi =0 ;//庄对子
	m_iXDuizi =0 ;//闲对子
	m_iZTianwan =0 ;//庄天王
	m_iXTianwang =0 ;//闲天王
	m_iPlayCount=0;
	memset(m_bySLuziData,255,sizeof(m_bySLuziData));
	//lym2010-03-17
	memset(m_iWinType,0,sizeof(m_iWinType));
	///是否显示下一页的图片

	memset(m_bKaiPaiData,255,sizeof(m_bKaiPaiData));
	memset(m_iKaiPaiType,0,sizeof(m_iKaiPaiType));

	m_bShowNext = false;

	memset(&m_iZPaiXing, 0, sizeof(m_iZPaiXing)); //庄家牌型,元素0前两张牌的值，元素1总牌值，元素2天王，元素3对子，元素4和
	memset(&m_iXPaiXing, 0, sizeof(m_iXPaiXing));//闲家牌型
	//m_iNowJuShu = 0;	//本局进行的局数，进行到第几局（72把一局）
	
}
BOOL Clookinfo::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return TRUE; //CDialog::OnEraseBkgnd(pDC);
}

void Clookinfo::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	CDialog::OnWindowPosChanging(lpwndpos);

	// TODO: 在此处添加消息处理程序代码
	Invalidate();
}

void Clookinfo::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CDialog::OnWindowPosChanged(lpwndpos);

	// TODO: 在此处添加消息处理程序代码
}

void Clookinfo::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
	bMouseDown=true;
	//savePoint=point;

	CDialog::OnLButtonDown(nFlags, point);
}
///名称：DrawLuzi
///描述：绘制路子的图片和数字信息
///@param pDC 绘图设备
///@return 
void Clookinfo::DrawLuzi(CDC*pDC)
{
	CRect  rectName(0,0,0,0);
	//////绘制输赢比例信息
	///庄赢比例
	if(m_iPlayCount <= 0)
	{
		m_iPlayCount = 1;
	}
	else if(m_iPlayCount > 1)
	{
		m_iPlayCount -= 1;
	}
	int count = 0;
	count = m_iZwin+m_iXwin+m_iHwin;
	if(count <= 0)
		count = 1;
	CString str = "";
	rectName.SetRect(155,43,212,61);
	int iSmallCount = 38; //
	int iBigCount = 18;
	int iBigPicHeight = 49;
	if (m_iGamePictureType > 0)
	{
		rectName.SetRect(155-62,43-27+2,212-62,61-27+2);
		///闲赢了几局
		str.Format("%d",m_iXwin);		
		pDC->DrawText(str, str.GetLength(), &rectName, DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

		///和赢了几局
		str.Format("%d",m_iHwin);		
		rectName.OffsetRect(170,0);
		pDC->DrawText(str, str.GetLength(), &rectName, DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
		///庄赢了几局	
		str.Format("%d",m_iZwin);		
		rectName.OffsetRect(163,0);
		pDC->DrawText(str, str.GetLength(), &rectName, DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

		m_pDaTu.SetPoint(49-24+4-1,216-20+2-31);	//大图基础位置
		m_pXiaoTu.SetPoint(17+13-1,93-29);			//小图基础位置
		m_iDaWidth = 31;
	   iSmallCount = 29;
	   iBigCount = 15;
	   iBigPicHeight = 31;
	}
	else
	{
		///庄赢了几局	
		str.Format("%d",m_iZwin);
		rectName.SetRect(488,43,544,61);
		pDC->DrawText(str,str.GetLength(),&rectName,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

		///闲赢了几局
		str.Format("%d",m_iXwin);
		rectName.SetRect(155,43,212,61);
		pDC->DrawText(str,str.GetLength(),&rectName,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

		///和赢了几局
		str.Format("%d",m_iHwin);
		rectName.SetRect(325,43,381,61);
		pDC->DrawText(str,str.GetLength(),&rectName,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
	}
	CGameImageHelper	zhuang(&m_imgzhuang);
	CGameImageHelper	xian(&m_imgxian);
	CGameImageHelper	he(&m_imghe);


	CGameImageHelper	zhuangduizi(&m_imgzhuangduizi);
	CGameImageHelper	xianduizi(&m_imgxianduizi);
	CGameImageHelper	heduizi(&m_imgheduizi);



	///粘贴上方路子信息
	//重写画路子圆圈的部份
	int iBeginCol = 0; // 起始列位置
	if (m_iLuziCol > iSmallCount)
	{
		iBeginCol = m_iLuziCol - iSmallCount;
	}
	else
	{
		iBeginCol = 0;
	}
	for(int i=iBeginCol; i<iBeginCol+iSmallCount; i++)
	{
		for(int j=0; j<6; j++)
		{
			
			if(m_bySLuziData[i][j] == 1)//开庄
			{
				//MsgPrint("zhangjing::m_bKaiPaiData[i][j]=%d",m_bKaiPaiData[i][j]);
				
				if(m_bKaiPaiData[i][j]==3||m_bKaiPaiData[i][j]==6)
				{
				zhuangduizi.BitBlt(pDC->GetSafeHdc(),m_pXiaoTu.x + (i - iBeginCol) * m_iXiaoWidth,m_pXiaoTu.y + j*m_iXiaoWidth);
				}
				else
				{
				zhuang.BitBlt(pDC->GetSafeHdc(),m_pXiaoTu.x + (i - iBeginCol) * m_iXiaoWidth,m_pXiaoTu.y + j*m_iXiaoWidth);
				}
				
			}
			else if(m_bySLuziData[i][j] == 2)//开闲
			{
				//MsgPrint("zhangjing::AAAAAm_bKaiPaiData[i][j]=%d",m_bKaiPaiData[i][j]);
				if(m_bKaiPaiData[i][j]==3||m_bKaiPaiData[i][j]==6)
				{
					xianduizi.BitBlt(pDC->GetSafeHdc(),m_pXiaoTu.x + (i - iBeginCol) * m_iXiaoWidth,m_pXiaoTu.y + j*m_iXiaoWidth);
				}
				else
				{
					xian.BitBlt(pDC->GetSafeHdc(),m_pXiaoTu.x + (i - iBeginCol) * m_iXiaoWidth,m_pXiaoTu.y + j*m_iXiaoWidth);
				}
			}
			else if(m_bySLuziData[i][j] == 3)//开和
			{
				//MsgPrint("zhangjing::m_bKaiPaiData[i][j]=%d",m_bKaiPaiData[i][j]);
				if(m_bKaiPaiData[i][j]==3||m_bKaiPaiData[i][j]==6)
				{
					heduizi.BitBlt(pDC->GetSafeHdc(),m_pXiaoTu.x + (i - iBeginCol) * m_iXiaoWidth,m_pXiaoTu.y + j*m_iXiaoWidth);
				}
				else
				{
					he.BitBlt(pDC->GetSafeHdc(),m_pXiaoTu.x + (i - iBeginCol) * m_iXiaoWidth,m_pXiaoTu.y + j*m_iXiaoWidth);
				}
				
			}
		}			
	}	

	CGameImageHelper	zhuang1(&m_imgzhuang1);
	CGameImageHelper	xian1(&m_imgxian1);
	CGameImageHelper	he1(&m_imghe1);

	CGameImageHelper	zhuang1_dot(&m_imgzhuang1_dot);
	CGameImageHelper	xian1_dot(&m_imgxian1_dot);
	CGameImageHelper	he1_dot(&m_imghe1_dot);


	///粘贴下方路子信息
	CFont font, *OldFont = NULL;
	font.CreateFont(11,0,0,0,0,0,0,0,134,3,2,1,2,TEXT("宋体")); 
	OldFont = (CFont*)pDC->SelectObject(&font);
	pDC->SetTextColor(RGB(0,0,0));
	count = 0;
	count = m_iZwin + m_iXwin + m_iHwin;
	if(count <= 0)
		count = 1;

	int numy = 0, numx = 0, ju = 0;	
	CString strJ = "", strT1 = "", strJ2 = "";	
	ju = m_iNowJuShu % 100;
	if(ju >= 10)
	{
		strJ.Format("%d",ju);
	}
	else
	{
		strT1.Format("%d",ju);
		strJ = "0" + strT1;
	}
	for(int i=0; i<MAXCOUNT/*count*/; i++)
	{
		numy = i % iBigCount;
		numx = i / iBigCount;
		//根据m_iWinType[i]的值贴图
		if(m_iWinType[i] == 1)		//开庄
		{
			if(m_iKaiPaiType[i]==3||m_iKaiPaiType[i]==6)
			{
				zhuang1_dot.BitBlt(pDC->GetSafeHdc(),m_pDaTu.x + numy*m_iDaWidth,m_pDaTu.y + numx*iBigPicHeight);
			}
			else
			{
				zhuang1.BitBlt(pDC->GetSafeHdc(),m_pDaTu.x + numy*m_iDaWidth,m_pDaTu.y + numx*iBigPicHeight);
			}
			
		}
		else if(m_iWinType[i] == 2)	//开闲
		{
			if(m_iKaiPaiType[i]==3||m_iKaiPaiType[i]==6)
			{
				xian1_dot.BitBlt(pDC->GetSafeHdc(),m_pDaTu.x + numy*m_iDaWidth,m_pDaTu.y + numx*iBigPicHeight);
			}
			else
			{
			xian1.BitBlt(pDC->GetSafeHdc(),m_pDaTu.x + numy*m_iDaWidth,m_pDaTu.y + numx*iBigPicHeight);
			}
		}
		else if(m_iWinType[i] == 3)	//开和
		{
			if(m_iKaiPaiType[i]==3||m_iKaiPaiType[i]==6)
			{
				he1_dot.BitBlt(pDC->GetSafeHdc(),m_pDaTu.x + numy*m_iDaWidth,m_pDaTu.y + numx*iBigPicHeight);
			}
			else
			{
			he1.BitBlt(pDC->GetSafeHdc(),m_pDaTu.x + numy*m_iDaWidth,m_pDaTu.y + numx*iBigPicHeight);
			}
		}
		if (0 == m_iGamePictureType)
		{
			///绘制将号m_iNowJuShu
			if(m_iWinType[i] > 0)
			{
				strJ2 = "";
				if(i >= 9)
				{
					strT1.Format("%d", i+1);
					strJ2 = strJ + strT1;
				}
				else
				{
					strT1.Format("%d", i+1);
					strJ2 = strJ + "0" + strT1;
				}
				rectName.SetRect(49+numy*m_iDaWidth,199+numx*49,79+numy*m_iDaWidth,214+numx*49);
				pDC->DrawText(strJ2,strJ2.GetLength(),&rectName,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
				OutputDebugString(strJ2);
			}
		}	
	}
	pDC->SelectObject(OldFont);
	font.DeleteObject();
}

///名称：ChangeData
///描述：数据转换，将开牌信息转换成另一种存放格式，方便绘图使用
///@param 
///@return 
void Clookinfo::ChangeData()
{
	memset(m_bySLuziData, 255, sizeof(m_bySLuziData));
	memset(m_bKaiPaiData,255,sizeof(m_bKaiPaiData));
	int numx=0,numy=0,type=0;
	type = m_iWinType[0];
	for(int i=0; i<m_iPlayCount; i++)
	{
		if ( m_iWinType[i] == 0)
			continue;
		if(type != m_iWinType[i] && m_iWinType[i] != 3)
		{
			numx++;
			numy = 0;
			type = m_iWinType[i];
		}
		else
		{
			if(i != 0)
			{
				numy++;//行
			}
		}
		if(numy == 6)
		{
			numy = 0;
			numx++;
		}
		m_bySLuziData[numx][numy] = m_iWinType[i];
		m_bKaiPaiData[numx][numy] = m_iKaiPaiType[i];
	}
	m_iLuziCol = numx + 1;	
	///如果第一页不满，第二页就没有必要显示了
	if(m_bySLuziData[37][0] == 255)
	{
		m_bShowNext = false;
		m_BtnYiye.EnableWindow(false);				// 第一页
		m_BtnErye.EnableWindow(false);				// 第二页图片
	}
}

///点击第一页事件，显示第一页的路子信息
void Clookinfo::OnBnClickedButYiye()
{
	// TODO: 在此添加控件通知处理程序代码
	///是否显示下一页的图片
	m_bShowNext = false;
	m_BtnYiye.EnableWindow(false);				// 第一页
	m_BtnErye.EnableWindow(true);				// 第二页图片
}
///点击第二页事件，显示第二页的路子信息
void Clookinfo::OnBnClickedButErye()
{
	// TODO: 在此添加控件通知处理程序代码
	///是否显示下一页的图片
	m_bShowNext = true;
	m_BtnYiye.EnableWindow(true);				// 第一页
	m_BtnErye.EnableWindow(false);				// 第二页图片
}
///关闭路子对话框
void Clookinfo::OnBnClickedButClose()
{
	// TODO: 在此添加控件通知处理程序代码
	this->ShowWindow(false);
}

