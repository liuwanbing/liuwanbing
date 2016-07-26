// PBankDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PBankDlg.h"
#include "GameRoomEx.h"

static char* szDigit[DIGITSIZE] = {"零", "壹", "贰", "叁", "肆", "伍", "陆", "柒", "捌", "玖"}; //中文大写数字
static char* szUnit[UNITSIZE] = {"", "拾", "佰", "仟", "万", "拾", "佰", "仟", "亿", "拾","佰","仟","万", "拾","佰","仟","万","拾","佰"};	  //中文大写单位

// CPBankDlg 对话框

IMPLEMENT_DYNAMIC(CPBankDlg, CDialog)

CPBankDlg::CPBankDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPBankDlg::IDD, pParent)
{
	m_pbgImg = NULL;
	m_pGameRoom = pParent;
	m_uCheckInOrOut = 0;
	m_bkBrush = NULL;
	m_haveZhuanZhang = Glb().m_CenterServerPara.m_is_haveZhuanZhang;      // 具有转账功能
    m_bTransferSuccess = TRUE/*FALSE*/;
    m_strSaveID = "";
    m_iUseNickName = 0;
	bFlag = false;	
	m_bUseDestID = true;
}

CPBankDlg::~CPBankDlg()
{
	delete m_pbgImg;
	DeleteObject(m_bkBrush);
}

void CPBankDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDCANCEL, m_BtClose);
	DDX_Control(pDX, IDC_BTNINFOR, m_btInfor);
	DDX_Control(pDX, IDC_BTNTRANSFER, m_bnTrans);
	DDX_Control(pDX, IDC_BTNRECORD, m_bnRecord);
	DDX_Control(pDX, IDC_BTNSAFE, m_bnSafe);
	DDX_Control(pDX, IDC_BTNPW, m_bnPW);
	DDX_Control(pDX, IDC_BTN_TRANOK, m_btnTranOK);
	DDX_Control(pDX, IDC_SKINLIST, m_SkinList);
	DDX_Control(pDX, IDC_BTNREFRESH, m_btnRefresh);
	DDX_Control(pDX, IDC_BTNCLOSE, m_btnRcoClose);
	DDX_Control(pDX, IDC_BTNBOK, m_btnBok);
	DDX_Control(pDX, IDC_BTNBACTIVE, m_btnBactive);
	DDX_Control(pDX, IDC_BTNBCHARGE, m_btnBcharge);
	DDX_Control(pDX, IDC_BTNCHGOK, m_btnChOK);
	DDX_Control(pDX, IDC_BTNCHGCLOSE, m_btnChClose);
    DDX_Control(pDX, IDC_BTN_TRANS_ALL, m_Button_Trans_All);
	DDX_Control(pDX, IDC_CHECK_TRANS_ALL, m_Check_TransAll);
	DDX_Control(pDX, IDC_BTN_COIN_ALL, m_btnCoinAll);
}


BEGIN_MESSAGE_MAP(CPBankDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BTNINFOR,&CPBankDlg::OnBnClickedInfor)
	ON_BN_CLICKED(IDC_BTNTRANSFER,&CPBankDlg::OnBnClickedTrans)
	ON_BN_CLICKED(IDC_BTNRECORD,&CPBankDlg::OnBnClickedRecord)
	ON_BN_CLICKED(IDC_BTNSAFE,&CPBankDlg::OnBnClickedSafe)
	ON_BN_CLICKED(IDC_BTNPW,&CPBankDlg::OnBnClickedPassWord)

	ON_BN_CLICKED(IDC_BTNCHGCLOSE,OnCancel)
	ON_BN_CLICKED(IDC_BTNCLOSE,OnCancel)

	ON_BN_CLICKED(IDC_RSAVE,OnBnClickedSave)
	ON_BN_CLICKED(IDC_RGET,OnBnClickedGet)

	ON_BN_CLICKED(IDC_RTRANS_ID,OnBnClickedRTransId)
	ON_BN_CLICKED(IDC_RTRANS_NN,OnBnClickedRTransNn)

	ON_BN_CLICKED(IDC_BTNCHGOK,OnBnClickedChPW)
	ON_BN_CLICKED(IDC_BTN_TRANOK,OnBnClickedAccountOK)

	ON_BN_CLICKED(IDC_BTNREFRESH,RefreshRecordList)

	ON_BN_CLICKED(IDC_BTNBOK,OnBnClickedBankOK)
	ON_BN_CLICKED(IDC_BTNBACTIVE,OnBnClickedBankActive)
	ON_BN_CLICKED(IDC_BTNBCHARGE,OnBnClickedBankRecharge)

    ON_EN_KILLFOCUS(IDC_ETOTHERID, OnIDKillFocus)
    ON_BN_CLICKED(IDC_BTN_TRANS_ALL,OnBnClickedTransAll)
	ON_BN_CLICKED(IDC_CHECK_TRANS_ALL, OnBnClickedCheckTransAll)
	ON_BN_CLICKED(IDC_BTN_COIN_ALL, OnBnClickedCoinAll)
	ON_EN_CHANGE(IDC_ETOUTCOIN, &CPBankDlg::OnEnChangeEtbconnoin)
	ON_EN_CHANGE(IDC_ETBCONNOIN, &CPBankDlg::OnEnChangeMoney)
END_MESSAGE_MAP()


// CPBankDlg 消息处理程序
void CPBankDlg::OnPaint()
{
	CPaintDC dc(this);
	Graphics pGrap(dc.GetSafeHdc());
	ImageAttributes   imAtt; 
	imAtt.SetColorKey( 
		Color(255,   0,   255), 
		Color(255,   0,   255), 
		ColorAdjustTypeBitmap);
	pGrap.DrawImage(m_pbgImg,Rect(0,0,m_pbgImg->GetWidth(),m_pbgImg->GetHeight()),0,0,m_pbgImg->GetWidth(),m_pbgImg->GetHeight(),UnitPixel,&imAtt);
}

BOOL CPBankDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f(CBcfFile::GetAppPath () + strSkin);
	TCHAR path[MAX_PATH];
	CString skinfolder;
	skinfolder = f.GetKeyVal(m_skinmgr.GetKeyVal(strSkin),"skinfolder",m_skinmgr.GetSkinPath());
	
    // PengJiLin, 2010-9-20
    if(m_haveZhuanZhang)
    {
        wsprintf(path,"%sdialog\\BankDlg\\wealth.png",skinfolder);
    }
    else
    {
        wsprintf(path,"%sdialog\\BankDlg\\wealthother.png",skinfolder);
    }

	WCHAR wpath[MAX_PATH] = {0};
	MultiByteToWideChar(CP_ACP,0,path,strlen(path),wpath,256);
	if (NULL == m_pbgImg)
	{
		//m_pbgImg = new Image(wpath);

#ifndef BZ_ZLIB               ///<add by wxx 2010.7.2
		m_pbgImg = new Image(wpath);
#else
		CUnZipRes unZipRes;
		IStream * pStream=NULL;
		pStream=unZipRes.LoadFileResFromZip(path);

		Image img(pStream);
		m_pbgImg = new Image(pStream);
#endif
		
	}
	
	if (NULL != m_pbgImg)
	{
		SetWindowPos(NULL,0,0,m_pbgImg->GetWidth(),m_pbgImg->GetHeight(),SWP_NOMOVE|SWP_NOZORDER);
	}
	{
		DWORD color = GetBackGroundColor();
		m_SkinList.SetColor(color);

		LOGFONT lf;
		memset(&lf, 0, sizeof(LOGFONT));
		lf.lfHeight = 19;
		strcpy(lf.lfFaceName, "Sevenet 7");
		CFont font;
		font.CreateFontIndirect(&lf);
		m_SkinList.SetFont(&font, TRUE);

		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");

		m_SkinList.InsertColumn(0, fMsg.GetKeyVal("PBankDlg","TransferTime","赠送时间"), LVCFMT_LEFT, 82);
		m_SkinList.InsertColumn(1, fMsg.GetKeyVal("PBankDlg","PayerID","支付人ID"), LVCFMT_LEFT, 82);
		m_SkinList.InsertColumn(2, fMsg.GetKeyVal("PBankDlg","PayeeID","收款人ID"), LVCFMT_LEFT, 82);
		m_SkinList.InsertColumn(3, fMsg.GetKeyVal("PBankDlg","Payment","支付金币"), LVCFMT_LEFT, 65);


		m_SkinList.SetRedraw(FALSE);
		font.DeleteObject();
	}
    if(!m_haveZhuanZhang)
	{
		m_bnSafe.ShowWindow(SW_HIDE);
		m_bnPW.ShowWindow(SW_HIDE);
	}
	
	// PengJiLin, 2010-9-16, 是否使用昵称显示
    {
        CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
        m_iUseNickName = fMsg.GetKeyVal("PBankDlg","UseNickName",0);
    }
	
	m_Button_Trans_All.EnableWindow(FALSE);
    m_strSaveID = "";

    if(m_iUseNickName > 0)
    {
        //m_btnTranOK.EnableWindow(FALSE);
    }

	GetDlgItem(IDC_ET_MONEY)->SetWindowText("");
	GetDlgItem(IDC_ETCHINESE)->SetWindowText("");

	m_bkBrush = CreateSolidBrush(GetBackGroundColor());

	OnBnClickedInfor();
	if (m_haveZhuanZhang == 2)
	{
		m_bnPW.ShowWindow(SW_HIDE);
	}



	m_bkimage.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	CGameImageHelper ImageHandle(&m_bkimage);
	HRGN hRgn=AFCBmpToRgn(ImageHandle,m_bkimage.GetPixel(0,0),RGB(1,1,1));
	if (hRgn!=NULL)
	{
		SetWindowRgn(hRgn,TRUE);
		DeleteObject(hRgn);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

LRESULT CPBankDlg::OnExchangeSkin(WPARAM wpara,LPARAM lpara)
{
	LoadSkin();
	//OnPaint();
	Invalidate();
	return LRESULT(0);
}
void CPBankDlg::LoadSkin()
{
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f(CBcfFile::GetAppPath () + strSkin);
	TCHAR path[MAX_PATH];
	CString skinfolder = f.GetKeyVal(m_skinmgr.GetKeyVal(strSkin),"skinfolder",m_skinmgr.GetSkinPath());

	// PengJiLin, 2010-9-20
	if(m_haveZhuanZhang)
		wsprintf(path,"%sdialog\\BankDlg\\wealth.png",skinfolder);
	else
		wsprintf(path,"%sdialog\\BankDlg\\wealthother.png",skinfolder);

	WCHAR wpath[MAX_PATH] = {0};
	MultiByteToWideChar(CP_ACP,0,path,strlen(path),wpath,256);
	if (NULL == m_pbgImg)
	{
		//m_pbgImg = new Image(wpath);

#ifndef BZ_ZLIB               ///<add by wxx 2010.7.2
		m_pbgImg = new Image(wpath);
#else
		CUnZipRes unZipRes;
		IStream * pStream=NULL;
		pStream=unZipRes.LoadFileResFromZip(path);

		Image img(pStream);
		m_pbgImg = new Image(pStream);
#endif

	}

	if (NULL != m_pbgImg)
	{
		SetWindowPos(NULL,0,0,m_pbgImg->GetWidth(),m_pbgImg->GetHeight(),SWP_NOMOVE|SWP_NOZORDER);
	}
	{
		DWORD color = GetBackGroundColor();
		m_SkinList.SetColor(color);

		LOGFONT lf;
		memset(&lf, 0, sizeof(LOGFONT));
		lf.lfHeight = 19;
		strcpy(lf.lfFaceName, "Sevenet 7");
		CFont font;
		font.CreateFontIndirect(&lf);
		m_SkinList.SetFont(&font, TRUE);

		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");

		//m_SkinList.InsertColumn(0, fMsg.GetKeyVal("PBankDlg","TransferTime","赠送时间"), LVCFMT_LEFT, 82);
		//m_SkinList.InsertColumn(1, fMsg.GetKeyVal("PBankDlg","PayerID","支付人ID"), LVCFMT_LEFT, 82);
		//m_SkinList.InsertColumn(2, fMsg.GetKeyVal("PBankDlg","PayeeID","收款人ID"), LVCFMT_LEFT, 82);
		//m_SkinList.InsertColumn(3, fMsg.GetKeyVal("PBankDlg","Payment","支付金币"), LVCFMT_LEFT, 65);

		m_SkinList.SetRedraw(FALSE);
		font.DeleteObject();
	}
	if(!m_haveZhuanZhang)
	{
		m_bnSafe.ShowWindow(SW_HIDE);
		m_bnPW.ShowWindow(SW_HIDE);
	}

	// PengJiLin, 2010-9-16, 是否使用昵称显示
	{
		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
		m_iUseNickName = fMsg.GetKeyVal("PBankDlg","UseNickName",0);
	}

	m_Button_Trans_All.EnableWindow(FALSE);
	m_strSaveID = "";

	if(m_iUseNickName > 0)
	{
		//m_btnTranOK.EnableWindow(FALSE);
	}

	GetDlgItem(IDC_ET_MONEY)->SetWindowText("");
	GetDlgItem(IDC_ETCHINESE)->SetWindowText("");

	//m_bkBrush = CreateSolidBrush(GetBackGroundColor());

	OnBnClickedInfor();
	if (m_haveZhuanZhang == 2)
	{
		m_bnPW.ShowWindow(SW_HIDE);
	}

	m_bkimage.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	//CGameImageHelper ImageHandle(&m_bkimage);
	//HRGN hRgn=AFCBmpToRgn(ImageHandle,m_bkimage.GetPixel(0,0),RGB(1,1,1));
	//if (hRgn!=NULL)
	//{
	//	SetWindowRgn(hRgn,TRUE);
	//	DeleteObject(hRgn);
	//}
}

LRESULT CPBankDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	switch(message)
	{
	case WM_CTLCOLORSTATIC :
		DWORD color = GetBackGroundColor();
		SetTextColor((HDC)wParam, RGB(0,0,0));
		SetBkColor((HDC)wParam,color);
		return (LRESULT)m_bkBrush;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

DWORD CPBankDlg::GetBackGroundColor()
{
	CBcfFile fBF(CBcfFile::GetAppPath () + "ClientMessage.bcf");
	CString str = fBF.GetKeyVal("BankDlg","BackGroundRGB","198, 228, 244");
	int iR,iG,iB;
	CString strTem = str.Left(str.Find(","));
	iR = atoi(strTem);
	str = str.Right(str.GetLength()-str.Find(",")-1);
	strTem = str.Left(str.Find(","));
	iG = atoi(strTem);
	str = str.Right(str.GetLength()-str.Find(",")-1);
	iB = atoi(str);

	return RGB((unsigned char)iR,(unsigned char)iG,(unsigned char)iB);
}

void CPBankDlg::OnBnClickedInfor()
{
	m_nCurPage = 1;
	ShowInterface();
}

void CPBankDlg::OnBnClickedTrans()
{
	if(m_haveZhuanZhang)
	{
		m_nCurPage = 2;		
	}
	else
	{
		m_nCurPage = 4;		
	}
	ShowInterface();
}

void CPBankDlg::OnBnClickedRecord()
{	
	if(m_haveZhuanZhang)
    	m_nCurPage = 3;
	else
		m_nCurPage = 5;
	ShowInterface();

    // PengJiLin, 2010-8-23, 转账成功，则刷新
    if(m_nCurPage == 3&&TRUE == m_bTransferSuccess)
    {
        RefreshRecordList();
        m_bTransferSuccess = FALSE;
    }
}

void CPBankDlg::OnBnClickedSafe()
{
	if(m_haveZhuanZhang)
	{
		m_nCurPage = 4;		
		ShowInterface();
	}
}

void CPBankDlg::OnBnClickedPassWord()
{
	if(m_haveZhuanZhang)
	{
		m_nCurPage = 5;		
		ShowInterface();
	}
}
/// 银行房间关闭后，通知父窗口一个消息
void CPBankDlg::OnCancel()
{
	m_pGameRoom->PostMessage(WM_BANK_CLOSE, 0, 0);
	return CDialog::OnCancel();
}
void CPBankDlg::OnOK()
{
	switch(m_nCurPage)
	{
	case 2:
		return OnBnClickedAccountOK();
		break;
	case 4:
		return OnBnClickedBankOK();
		break;
	case 5:
		return OnBnClickedChPW();
		break;
	default:
		break;
	}
	return __super::OnOK();
}

void CPBankDlg::ShowInterface()
{
	CWnd * pWnd;
	WCHAR PathImg[MAX_PATH]={0};
	CGameImage btn;

    // PengJiLin, 2010-8-23, 全转 按钮，金葫芦二代功能
    m_Button_Trans_All.ShowWindow(SW_HIDE);
    m_Check_TransAll.ShowWindow(SW_HIDE);

	bFlag = false;

	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f(CBcfFile::GetAppPath () + strSkin);
	TCHAR path[MAX_PATH];
	CString skinfolder;
	skinfolder = f.GetKeyVal(m_skinmgr.GetKeyVal(strSkin),"skinfolder",m_skinmgr.GetSkinPath());

	if (1 == m_nCurPage)
	{
		if(m_haveZhuanZhang)
		{
			wsprintf(path,"%sdialog\\BankDlg\\wealth.png",skinfolder);
		}
		else
		{
			wsprintf(path,"%sdialog\\BankDlg\\wealthother.png",skinfolder);
		}
		Graphics *pG = Graphics::FromImage(m_pbgImg);
		wmemset(PathImg,0,MAX_PATH);
		MultiByteToWideChar(CP_ACP,0,path,strlen(path),PathImg,MAX_PATH);
#ifndef BZ_ZLIB
		Image img(PathImg);
#else
		CUnZipRes unZipRes;
		IStream * pStream=NULL;
		pStream=unZipRes.LoadFileResFromZip(path);
		Image img(pStream);
		if(pStream!=NULL)
		{
			pStream->Release();
			pStream=NULL;
		}
		
#endif
		
		pG->DrawImage(&img,Rect(0,0,m_pbgImg->GetWidth(),m_pbgImg->GetHeight()),0,0,m_pbgImg->GetWidth(),m_pbgImg->GetHeight(),UnitPixel);
		delete pG;

		pWnd = GetDlgItem(IDC_EDIT_INFO_GOLD);
		pWnd->MoveWindow(248,84,210,20);
//		pWnd->SetWindowText("0.0");
		pWnd->ShowWindow(SW_SHOW);

		pWnd = GetDlgItem(IDC_EDIT_GAMECOIN);
		pWnd->MoveWindow(248,156,210,20);
//		pWnd->SetWindowText("204");
		pWnd->ShowWindow(SW_SHOW);
		
		pWnd = GetDlgItem(IDC_EDIT_BOXCOIN);
		pWnd->MoveWindow(248,228,210,20);
//		pWnd->SetWindowText("2000");
		pWnd->ShowWindow(SW_SHOW);

		pWnd = GetDlgItem(IDC_EDIT_INFO_GOLD2);
		pWnd->MoveWindow(168,120,290,20);
		pWnd->SetWindowText(fMsg.GetKeyVal("PBankDlg","UseDirectly","在游戏中使用。"));
		pWnd->ShowWindow(SW_SHOW);

		pWnd = GetDlgItem(IDC_EDIT_GAMECOIN2);
		pWnd->MoveWindow(168,192,290,20);
		pWnd->SetWindowText(fMsg.GetKeyVal("PBankDlg","TransInfor","银行中的金币，不能在游戏中直接使用。"));
		pWnd->ShowWindow(SW_SHOW);

		pWnd = GetDlgItem(IDC_EDIT_BOXCOIN2);
		pWnd->MoveWindow(168,262,290,20);
		pWnd->SetWindowText(fMsg.GetKeyVal("PBankDlg","NeedPassWord","银行中的存款可用来支付购买道具的费用。"));
		pWnd->ShowWindow(SW_SHOW);

		ShowMoneyBank();
		m_btInfor.ShowWindow(SW_HIDE);
	}
	else
	{
		GetDlgItem(IDC_EDIT_INFO_GOLD)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_GAMECOIN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_BOXCOIN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_INFO_GOLD2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_GAMECOIN2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_BOXCOIN2)->ShowWindow(SW_HIDE);
		m_btInfor.ShowWindow(SW_SHOW);
	}

	if(2 == m_nCurPage)
	{
		wsprintf(path,"%sdialog\\BankDlg\\transfer.png",skinfolder);
		Graphics *pG = Graphics::FromImage(m_pbgImg);
		wmemset(PathImg,0,MAX_PATH);
		MultiByteToWideChar(CP_ACP,0,path,strlen(path),PathImg,MAX_PATH);
#ifndef BZ_ZLIB
		Image img(PathImg);
#else
		CUnZipRes unZipRes;
		IStream * pStream=NULL;
		pStream=unZipRes.LoadFileResFromZip(path);
		Image img(pStream);
		if(pStream!=NULL)
		{
			pStream->Release();
			pStream=NULL;
		}
#endif
		pG->DrawImage(&img,Rect(0,0,m_pbgImg->GetWidth(),m_pbgImg->GetHeight()),0,0,m_pbgImg->GetWidth(),m_pbgImg->GetHeight(),UnitPixel);
		delete pG;

		wsprintf(path,"%slogon\\button.bmp",skinfolder);
		btn.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
		HRGN hRgn = AFCBmpToRgn(btn,btn.GetPixel(0,0),RGB(1,1,1));
		m_btnTranOK.LoadButtonBitmap(path,false);
		if(hRgn)
		{
			m_btnTranOK.SetWindowRgn(hRgn,true);

            // PengJiLin, 2010-8-23, 全转 按钮，金葫芦二代功能
            if (1&Glb().m_CenterServerPara.m_nFunction)
            {
                m_Button_Trans_All.LoadButtonBitmap(path,false);
                m_Button_Trans_All.SetWindowRgn(hRgn,true);
            }

			///{{ Added by zxd 20100709 释放区域对象
			DeleteObject(hRgn);
			///Added by zxd 20100709 释放区域对象}}
		}
		pWnd = GetDlgItem(IDC_BTN_TRANOK);
		pWnd->MoveWindow(290,269,btn.GetWidth()/4,btn.GetHeight());
		pWnd->SetWindowText(fMsg.GetKeyVal("PBankDlg","OK","确定"));
		pWnd->ShowWindow(SW_SHOW);

        // PengJiLin, 2010-8-23, 全转 按钮，金葫芦二代功能
        if (1&Glb().m_CenterServerPara.m_nFunction)
        {
            //CRect rect;
            //m_btnTranOK.GetWindowRect(&rect);
            //ScreenToClient(&rect);
            //m_btnTranOK.MoveWindow(rect.right, rect.top, rect.Width(),rect.Height());

            //m_Button_Trans_All.MoveWindow(rect.left-rect.Width(), rect.top,rect.Width(),rect.Height());
            //m_Button_Trans_All.SetWindowText(fMsg.GetKeyVal("PBankDlg","TransAllBtn","全转"));
            m_Button_Trans_All.ShowWindow(SW_HIDE/*SW_SHOW*/);
        }

		pWnd = GetDlgItem(IDC_ETUSERCOIN);
		pWnd->MoveWindow(169,76,80,22);
		pWnd->SetWindowText(fMsg.GetKeyVal("PBankDlg","UserCoin","银行金币："));
		pWnd->ShowWindow(SW_SHOW);

		pWnd = GetDlgItem(IDC_ETCOIN);
		pWnd->MoveWindow(254,76,210,20);
		pWnd->ShowWindow(SW_SHOW);

		pWnd = GetDlgItem(IDC_ETTRANSCOIN);
		pWnd->MoveWindow(169,106,80,22);
		pWnd->SetWindowText(fMsg.GetKeyVal("PBankDlg","TransCoinNum","赠送金币数："));
		pWnd->ShowWindow(SW_SHOW);

		pWnd = GetDlgItem(IDC_ETOUTCOIN);
		pWnd->MoveWindow(254,106,210,20);
		pWnd->ShowWindow(SW_SHOW);
		
		//设置输入最大位数
		CEdit* pEdit = (CEdit*)GetDlgItem(IDC_ETOUTCOIN);
		if(Glb().m_bUseSpace)
			pEdit->SetLimitText(Glb().m_iMaxBit  + Glb().m_nPowerOfGold);	
		else
			pEdit->SetLimitText(18 + Glb().m_nPowerOfGold);

		pWnd = GetDlgItem(IDC_ETNUMTOCHINESE);
		pWnd->MoveWindow(169,136,80,22);
		pWnd->SetWindowText(fMsg.GetKeyVal("PBankDlg","NumToChinese","金币数(大写)："));
		pWnd->ShowWindow(SW_SHOW);

		pWnd = GetDlgItem(IDC_ETCHINESE);
		pWnd->MoveWindow(254,136,150,40);
		pWnd->ShowWindow(SW_SHOW);

		pWnd = GetDlgItem(IDC_RTRANS_ID);
		pWnd->MoveWindow(254,180,80,20);
		pWnd->SetWindowText(fMsg.GetKeyVal("PBankDlg","RTransUserID","对方ID"));
		pWnd->ShowWindow(SW_SHOW);

		pWnd = GetDlgItem(IDC_RTRANS_NN);
		pWnd->MoveWindow(365,180,80,20);
		pWnd->SetWindowText(fMsg.GetKeyVal("PBankDlg","RTransUserNick","对方昵称"));
		pWnd->ShowWindow(SW_SHOW);

		pWnd = GetDlgItem(IDC_ETTRANSUSERID);
		pWnd->MoveWindow(170,200,80,20);
		pWnd->SetWindowText(fMsg.GetKeyVal("PBankDlg","TransUserID","对方ID："));
		pWnd->ShowWindow(SW_SHOW);

		pWnd = GetDlgItem(IDC_ETOTHERID);
		pWnd->MoveWindow(254,200,210,20);
		pWnd->ShowWindow(SW_SHOW);

		pWnd = GetDlgItem(IDC_ETSECONDPW);
		pWnd->MoveWindow(170,238,80,20);
		pWnd->SetWindowText(fMsg.GetKeyVal("PBankDlg","SecondPassword","二级密码："));
		pWnd->ShowWindow(SW_SHOW);

		pWnd = GetDlgItem(IDC_ETPW);
		pWnd->MoveWindow(254,238,210,20);
		pWnd->ShowWindow(SW_SHOW);

		pWnd = GetDlgItem(IDC_ETTIP);
		pWnd->MoveWindow(164,280,40,20);
		pWnd->SetWindowText(fMsg.GetKeyVal("PBankDlg","Tip","提示："));
		pWnd->ShowWindow(SW_SHOW);

		pWnd = GetDlgItem(IDC_ETCOINNUM);
		pWnd->MoveWindow(164,300,240,20);
		pWnd->ShowWindow(SW_SHOW);

		pWnd = GetDlgItem(IDC_ETTRANSNUM);		
		pWnd->MoveWindow(164,318,240,20);
		pWnd->ShowWindow(SW_SHOW);

        // PengJiLin, 2010-8-23, 全转 按钮，金葫芦二代功能
        if (1&Glb().m_CenterServerPara.m_nFunction)
        {
            // PengJiLin, 2010-9-27, 全转 勾选框
            CRect rect;
            CRect rectMe;
            m_Check_TransAll.GetWindowRect(&rectMe);
            ScreenToClient(&rectMe);
            pWnd = GetDlgItem(IDC_ETOUTCOIN);
            pWnd->GetWindowRect(&rect);
            ScreenToClient(&rect);
            m_Check_TransAll.MoveWindow(rect.right+10, rect.top, rectMe.Width(), rectMe.Height());
            m_Check_TransAll.ShowWindow(SW_SHOW);
        }

		SetAccountInfor();

		if(m_bUseDestID)
		{
			((CButton*)GetDlgItem(IDC_RTRANS_ID))->SetCheck(BST_CHECKED);
			((CButton*)GetDlgItem(IDC_RTRANS_NN))->SetCheck(BST_UNCHECKED);
			OnBnClickedRTransId();
		}
		else
		{
			((CButton*)GetDlgItem(IDC_RTRANS_ID))->SetCheck(BST_UNCHECKED);
			((CButton*)GetDlgItem(IDC_RTRANS_NN))->SetCheck(BST_CHECKED);
			OnBnClickedRTransNn();
		}

		m_bnTrans.ShowWindow(SW_HIDE);
	}
	else
	{  
		GetDlgItem(IDC_ETCOIN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ETOUTCOIN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ETCHINESE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ETOTHERID)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ETPW)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ETTRANSNUM)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ETCOINNUM)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ETTIP)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_TRANOK)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_ETUSERCOIN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ETTRANSCOIN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ETNUMTOCHINESE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ETTRANSUSERID)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ETSECONDPW)->ShowWindow(SW_HIDE);		

		GetDlgItem(IDC_RTRANS_ID)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RTRANS_NN)->ShowWindow(SW_HIDE);

		m_bnTrans.ShowWindow(SW_SHOW);
	}


	if (3 == m_nCurPage)
	{
		wsprintf(path,"%sdialog\\BankDlg\\record.png",skinfolder);
		Graphics *pG = Graphics::FromImage(m_pbgImg);
		wmemset(PathImg,0,MAX_PATH);
		MultiByteToWideChar(CP_ACP,0,path,strlen(path),PathImg,MAX_PATH);
#ifndef BZ_ZLIB
		Image img(PathImg);
#else
		CUnZipRes unZipRes;
		IStream * pStream=NULL;
		pStream=unZipRes.LoadFileResFromZip(path);
		Image img(pStream);
		if(pStream!=NULL)
		{
			pStream->Release();
			pStream=NULL;
		}
#endif
		pG->DrawImage(&img,Rect(0,0,m_pbgImg->GetWidth(),m_pbgImg->GetHeight()),0,0,m_pbgImg->GetWidth(),m_pbgImg->GetHeight(),UnitPixel);
		delete pG;

		pWnd = GetDlgItem(IDC_SKINLIST);
		pWnd->MoveWindow(157,29,328,278);
		pWnd->ShowWindow(SW_SHOW);

		wsprintf(path,"%slogon\\button.bmp",skinfolder);
		btn.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
		HRGN hRgn = AFCBmpToRgn(btn,btn.GetPixel(0,0),RGB(1,1,1));
		m_btnRefresh.LoadButtonBitmap(path,false);
		if(hRgn)
		{
			m_btnRefresh.SetWindowRgn(hRgn,true);
			///{{ Added by zxd 20100709 释放区域对象
			DeleteObject(hRgn);
			///Added by zxd 20100709 释放区域对象}}
		}
		m_btnRefresh.MoveWindow(225,314,btn.GetWidth()/4,btn.GetHeight());
		m_btnRefresh.SetWindowText(fMsg.GetKeyVal("PBankDlg","Refresh","刷新"));
		m_btnRefresh.ShowWindow(SW_SHOW);

		m_btnRcoClose.LoadButtonBitmap(path,false);
		if(hRgn)
			m_btnRcoClose.SetWindowRgn(hRgn,true);
		m_btnRcoClose.MoveWindow(351,314,btn.GetWidth()/4,btn.GetHeight());
		m_btnRcoClose.SetWindowText(fMsg.GetKeyVal("PBankDlg","Close","关闭"));
		m_btnRcoClose.ShowWindow(SW_SHOW);

		InsertRecordList();
		m_bnRecord.ShowWindow(SW_HIDE);
	}
	else
	{
		GetDlgItem(IDC_SKINLIST)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTNCLOSE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTNREFRESH)->ShowWindow(SW_HIDE);

		m_bnRecord.ShowWindow(SW_SHOW);
	}

	if (4 == m_nCurPage)
	{
		if(m_haveZhuanZhang)
		{
		   wsprintf(path,"%sdialog\\BankDlg\\boxsafe.png",skinfolder);
		}
		else
		{
			wsprintf(path,"%sdialog\\BankDlg\\boxsafeother.png",skinfolder);
		}
		Graphics *pG = Graphics::FromImage(m_pbgImg);
		wmemset(PathImg,0,MAX_PATH);
		MultiByteToWideChar(CP_ACP,0,path,strlen(path),PathImg,MAX_PATH);
#ifndef BZ_ZLIB
		Image img(PathImg);
#else
		CUnZipRes unZipRes;
		IStream * pStream=NULL;
		pStream=unZipRes.LoadFileResFromZip(path);
		Image img(pStream);
		if(pStream!=NULL)
		{
			pStream->Release();
			pStream=NULL;
		}
#endif
		pG->DrawImage(&img,Rect(0,0,m_pbgImg->GetWidth(),m_pbgImg->GetHeight()),0,0,m_pbgImg->GetWidth(),m_pbgImg->GetHeight(),UnitPixel);
		delete pG;

		wsprintf(path,"%slogon\\button.bmp",skinfolder);
		btn.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
		HRGN hRgn = AFCBmpToRgn(btn,btn.GetPixel(0,0),RGB(1,1,1));
		m_btnBok.LoadButtonBitmap(path,false);
		if(hRgn)
		{
			m_btnBok.SetWindowRgn(hRgn,true);
			///{{ Added by zxd 20100709 释放区域对象
			DeleteObject(hRgn);
			///Added by zxd 20100709 释放区域对象}}
		}
		m_btnBok.MoveWindow(325,m_pbgImg->GetHeight() - btn.GetHeight() - 30,btn.GetWidth()/4,btn.GetHeight());
		//m_btnBok.MoveWindow(285,m_pbgImg->GetHeight() - 15 - btn.GetHeight(),btn.GetWidth()/4,btn.GetHeight());
		m_btnBok.SetWindowText(fMsg.GetKeyVal("PBankDlg","OK","确定"));
		m_btnBok.ShowWindow(SW_SHOW);

		m_btnBactive.LoadButtonBitmap(path,false);
		if(hRgn)
			m_btnBactive.SetWindowRgn(hRgn,true);
		m_btnBactive.MoveWindow(285,303,btn.GetWidth()/4,btn.GetHeight());
		m_btnBactive.SetWindowText(fMsg.GetKeyVal("PBankDlg","Active","激活"));
		m_btnBactive.ShowWindow(SW_HIDE);

		m_btnBcharge.LoadButtonBitmap(path,false);
		if(hRgn)
			m_btnBcharge.SetWindowRgn(hRgn,true);
		m_btnBcharge.MoveWindow(378,303,btn.GetWidth()/4,btn.GetHeight());
		m_btnBcharge.SetWindowText(fMsg.GetKeyVal("PBankDlg","Charge","续费"));
		m_btnBcharge.ShowWindow(SW_HIDE);

		pWnd = GetDlgItem(IDC_ETBOXCOIN);
		pWnd->MoveWindow(180,115,200,20);
		//pWnd->SetWindowText(fMsg.GetKeyVal("PBankDlg","AccountCoin","帐户携带游戏币：%s"));
		TCHAR szNum[128];
		CString strMsg;
		CString strTmp;
		GetNumString(szNum, m_i64WalletMoney, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
		strTmp = fMsg.GetKeyVal("PBankDlg","AccountCoin","现有金币：%s");
		strMsg.Format(strTmp, szNum);
		pWnd->SetWindowText(strMsg);
		pWnd->ShowWindow(SW_SHOW);

		pWnd = GetDlgItem(IDC_ETACOIN);
		pWnd->MoveWindow(180,135,200,20);
		//pWnd->SetWindowText(fMsg.GetKeyVal("PBankDlg","BoxCoin","保险箱中的游戏币：%s"));
		GetNumString(szNum, m_i64BankMoney, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
		strTmp = fMsg.GetKeyVal("PBankDlg","BoxCoin","银行存款：%s");
		strMsg.Format(strTmp, szNum);
		pWnd->SetWindowText(strMsg);
		pWnd->ShowWindow(SW_SHOW);

		pWnd = GetDlgItem(IDC_ETBCONNO);
		pWnd->MoveWindow(180,180,100,20);
		pWnd->SetWindowText(fMsg.GetKeyVal("PBankDlg","InputGameCoin","请输入存取金额："));
		pWnd->ShowWindow(SW_SHOW);

		pWnd = GetDlgItem(IDC_ETBPW);
		pWnd->MoveWindow(180,205,100,20);
		pWnd->SetWindowText(fMsg.GetKeyVal("PBankDlg","InputPW","请输入银行密码："));
		pWnd->ShowWindow(SW_SHOW);
		
		pWnd = GetDlgItem(IDC_ETBCONNOIN);
		pWnd->MoveWindow(280,180,135,20);
	//	pWnd->SetWindowText(fMsg.GetKeyVal("PBankDlg","BoxCoin","保险箱中的游戏币："));
		pWnd->ShowWindow(SW_SHOW);

		wsprintf(path,"%slogon\\button.bmp",skinfolder);
		btn.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
		hRgn = AFCBmpToRgn(btn,btn.GetPixel(0,0),RGB(1,1,1));
		m_btnCoinAll.LoadButtonBitmap(path,false);
		if(hRgn)
		{
			m_btnCoinAll.SetWindowRgn(hRgn,true);
			DeleteObject(hRgn);
		}
		pWnd = GetDlgItem(IDC_BTN_COIN_ALL);
		pWnd->MoveWindow(420,178,60,23);
		pWnd->SetWindowText(fMsg.GetKeyVal("PBankDlg","CoinAll","全部"));
		pWnd->ShowWindow(SW_SHOW);

		//设置输入最大位数
		CEdit* pEdit = (CEdit*)GetDlgItem(IDC_ETBCONNOIN);
		if(Glb().m_bUseSpace)
			pEdit->SetLimitText(Glb().m_iMaxBit  + Glb().m_nPowerOfGold);
		else
			pEdit->SetLimitText(18 + Glb().m_nPowerOfGold);

		pWnd = GetDlgItem(IDC_ETBPWIN);
		pWnd->MoveWindow(280,205,120,20);
	//	pWnd->SetWindowText(fMsg.GetKeyVal("PBankDlg","BoxCoin","保险箱中的游戏币："));
		pWnd->ShowWindow(SW_SHOW);

		TCHAR buffer[16];
		memset(buffer,0,16);
		switch(Glb().m_nPowerOfGold)
		{
		case 0:
			_tcscat( buffer, TEXT("1"));
		case 1:
			_tcscat( buffer, TEXT("十"));
			break;
		case 2:
			_tcscat( buffer, TEXT("百"));
			break;
		case 3:
			_tcscat( buffer, TEXT("千"));
			break;
		case 4:
			_tcscat( buffer, TEXT("万"));
			break;
		}

		pWnd = GetDlgItem(IDC_ETBREGPW);
		pWnd->MoveWindow(164,270,230,20);
		//pWnd->SetWindowText(fMsg.GetKeyVal("PBankDlg","RegPW",""));
		pWnd->ShowWindow(SW_SHOW);

		pWnd = GetDlgItem(IDC_RSAVE);
		pWnd->MoveWindow(206,291,80,20);
		pWnd->SetWindowText(fMsg.GetKeyVal("PBankDlg","SaveGameCoin","存入游戏币"));
		pWnd->ShowWindow(SW_SHOW);
		
		pWnd = GetDlgItem(IDC_RGET);
		pWnd->MoveWindow(206,316,80,20);
		//pWnd->MoveWindow(330,286,80,20);
		pWnd->SetWindowText(fMsg.GetKeyVal("PBankDlg","GetGameCoin","取出游戏币"));
		pWnd->ShowWindow(SW_SHOW);
		
		pWnd = GetDlgItem(IDC_MONEY_STATIC);
		pWnd->SetWindowText(fMsg.GetKeyVal("PBankDlg","NumToChinese","金币数(大写)："));
		pWnd->MoveWindow(180,230,80,20);
		pWnd->ShowWindow(SW_SHOW);
		pWnd = GetDlgItem(IDC_ET_MONEY);
		pWnd->MoveWindow(280,230,190,40);	
		//pWnd->SetWindowText("");
		pWnd->ShowWindow(SW_SHOW);

		if (m_uCheckInOrOut == 0)
		{
			((CButton*)GetDlgItem(IDC_RGET))->SetCheck(BST_UNCHECKED);
			((CButton*)GetDlgItem(IDC_RSAVE))->SetCheck(BST_CHECKED);
			OnBnClickedSave();
		}
		else
		{
			((CButton*)GetDlgItem(IDC_RGET))->SetCheck(BST_CHECKED);
			((CButton*)GetDlgItem(IDC_RSAVE))->SetCheck(BST_UNCHECKED);
			OnBnClickedGet();
		}
		m_bnSafe.ShowWindow(SW_HIDE);
		///{{ Added by zxd 20100709 释放区域对象
		if (hRgn != NULL)
		{
			DeleteObject(hRgn);
		}
		///Added by zxd 20100709 释放区域对象}}
	}
	else
	{
		GetDlgItem(IDC_ETACOIN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ETBOXCOIN)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_ETBCONNO)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ETBPW)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ETBCONNOIN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ETBPWIN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ETBREGPW)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_RSAVE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RGET)->ShowWindow(SW_HIDE);
		SetDlgItemText(IDC_ETBREGPW, "");
		m_btnBok.ShowWindow(SW_HIDE);
		m_btnBactive.ShowWindow(SW_HIDE);
		m_btnBcharge.ShowWindow(SW_HIDE);

		GetDlgItem(IDC_MONEY_STATIC)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ET_MONEY)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_BTN_COIN_ALL)->ShowWindow(SW_HIDE);

		if(m_haveZhuanZhang)
	    	m_bnSafe.ShowWindow(SW_SHOW);
	}

	if (5 == m_nCurPage)
	{
		if(m_haveZhuanZhang)
		{
	    	wsprintf(path,"%sdialog\\BankDlg\\Changepw.png",skinfolder);
		}
		else
		{
			wsprintf(path,"%sdialog\\BankDlg\\Changepwother.png",skinfolder);
		}
		Graphics *pG = Graphics::FromImage(m_pbgImg);
		wmemset(PathImg,0,MAX_PATH);
		MultiByteToWideChar(CP_ACP,0,path,strlen(path),PathImg,MAX_PATH);
#ifndef BZ_ZLIB
		Image img(PathImg);
#else
		CUnZipRes unZipRes;
		IStream * pStream=NULL;
		pStream=unZipRes.LoadFileResFromZip(path);
		Image img(pStream);
		if(pStream!=NULL)
		{
			pStream->Release();
			pStream=NULL;
		}
#endif
		pG->DrawImage(&img,Rect(0,0,m_pbgImg->GetWidth(),m_pbgImg->GetHeight()),0,0,m_pbgImg->GetWidth(),m_pbgImg->GetHeight(),UnitPixel);
		delete pG;

		wsprintf(path,"%slogon\\button.bmp",skinfolder);
		btn.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
		HRGN hRgn = AFCBmpToRgn(btn,btn.GetPixel(0,0),RGB(1,1,1));
		m_btnChOK.LoadButtonBitmap(path,false);
		if(hRgn)
		{
			m_btnChOK.SetWindowRgn(hRgn,true);
			///{{ Added by zxd 20100709 释放区域对象
			DeleteObject(hRgn);
			///Added by zxd 20100709 释放区域对象}}
		}
		m_btnChOK.MoveWindow(223,282,btn.GetWidth()/4,btn.GetHeight());
		m_btnChOK.SetWindowText(fMsg.GetKeyVal("PBankDlg","OK","确定"));
		m_btnChOK.ShowWindow(SW_SHOW);
	
		m_btnChClose.LoadButtonBitmap(path,false);
		if(hRgn)
			m_btnChClose.SetWindowRgn(hRgn,true);
		m_btnChClose.MoveWindow(345,282,btn.GetWidth()/4,btn.GetHeight());
		m_btnChClose.SetWindowText(fMsg.GetKeyVal("PBankDlg","Close","关闭"));
		m_btnChClose.ShowWindow(SW_SHOW);
	
		pWnd = GetDlgItem(IDC_ETOLDPW);
		pWnd->MoveWindow(200,117,74,20);
		pWnd->SetWindowText(fMsg.GetKeyVal("PBankDlg","OldPW","旧银行密码："));
		pWnd->ShowWindow(SW_SHOW);

		pWnd = GetDlgItem(IDC_ETOLDPWIN);
		pWnd->MoveWindow(278,115,120,20);
//		pWnd->SetWindowText(fMsg.GetKeyVal("PBankDlg","OldPW","旧银行密码："));
		pWnd->ShowWindow(SW_SHOW);

		pWnd = GetDlgItem(IDC_ETCHPW);
		pWnd->MoveWindow(200,231,260,20);
		pWnd->SetWindowText(fMsg.GetKeyVal("PBankDlg","ChangeYourPW","银行密码初始化为游戏登陆密码，请及时修改！"));
		pWnd->ShowWindow(SW_SHOW);

		pWnd = GetDlgItem(IDC_ETNEWPW);
		pWnd->MoveWindow(200,154,74,20);
		pWnd->SetWindowText(fMsg.GetKeyVal("PBankDlg","NewPW","新银行密码："));
		pWnd->ShowWindow(SW_SHOW);

		pWnd = GetDlgItem(IDC_ETNEWPWIN);
		pWnd->MoveWindow(278,150,120,20);
//		pWnd->SetWindowText(fMsg.GetKeyVal("PBankDlg","NewPW","新银行密码："));
		pWnd->ShowWindow(SW_SHOW);
		
		pWnd = GetDlgItem(IDC_ETCONFIRM);
		pWnd->MoveWindow(200,190,74,20);
		pWnd->SetWindowText(fMsg.GetKeyVal("PBankDlg","ConfirmPW","确认新密码："));
		pWnd->ShowWindow(SW_SHOW);

		pWnd = GetDlgItem(IDC_ETCONFIRMIN);
		pWnd->MoveWindow(278,186,120,20);
//		pWnd->SetWindowText(fMsg.GetKeyVal("PBankDlg","ConfirmPW","确认新密码："));
		pWnd->ShowWindow(SW_SHOW);
			
		m_bnPW.ShowWindow(SW_HIDE);

		///{{ Added by zxd 20100709 释放区域对象
		if (hRgn != NULL)
		{
			DeleteObject(hRgn);
		}
		///Added by zxd 20100709 释放区域对象}}

	}
	else
	{
		GetDlgItem(IDC_BTNCHGOK)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTNCHGCLOSE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ETCHPW)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_ETOLDPW)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ETOLDPWIN)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_ETNEWPW)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ETNEWPWIN)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_ETCONFIRM)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ETCONFIRMIN)->ShowWindow(SW_HIDE);

		m_btnChOK.ShowWindow(SW_HIDE);
		m_btnChClose.ShowWindow(SW_HIDE);
		if(m_haveZhuanZhang && m_haveZhuanZhang != 2)
	    	m_bnPW.ShowWindow(SW_SHOW);
	}

	wsprintf(path,"%sdialog\\log_exit_bt.bmp",skinfolder);//win_close.bmp
	m_BtClose.LoadButtonBitmap(path,false);
	btn.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	m_BtClose.MoveWindow(453, 0, btn.GetWidth()/4,btn.GetHeight(), FALSE);

	Invalidate(FALSE);
}

void CPBankDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (point.y < 28)
	{
		PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
	}
}

void CPBankDlg::OnBnClickedSave()
{
	SetDlgItemText(IDC_ETBREGPW, "");
	m_uCheckInOrOut = 0;
	GetDlgItem(IDC_ETBPW)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_ETBPWIN)->ShowWindow(SW_HIDE);
	//GetDlgItem(IDC_ETBREGPW)->ShowWindow(SW_HIDE);
}

void CPBankDlg::OnBnClickedGet()
{
	SetDlgItemText(IDC_ETBREGPW, "");
	m_uCheckInOrOut = 1;
	GetDlgItem(IDC_ETBPW)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_ETBPWIN)->ShowWindow(SW_SHOW);
	//GetDlgItem(IDC_ETBREGPW)->ShowWindow(SW_SHOW);
}

void CPBankDlg::OnBnClickedRTransId()
{
	m_bUseDestID = true;
	CBcfFile fMsg(CBcfFile::GetAppPath()+"..\\ClientMessage.bcf");
	GetDlgItem(IDC_ETTRANSUSERID)->SetWindowText(fMsg.GetKeyVal("PBankDlg","TransUserID","对方ID："));
}

void CPBankDlg::OnBnClickedRTransNn()
{
	m_bUseDestID = false;
	CBcfFile fMsg(CBcfFile::GetAppPath()+"..\\ClientMessage.bcf");
	GetDlgItem(IDC_ETTRANSUSERID)->SetWindowText(fMsg.GetKeyVal("PBankDlg","TransUserNick","对方昵称："));
}

///
/// 修改密码 的确定按钮响应函数
/// @param void
/// @return void
///
void CPBankDlg::OnBnClickedChPW()
{
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	CString strOld, strNew, str;
	TCHAR szMsg[MAX_PATH];

	/// 先比较新密码和确认密码是否相同，如果不同，则直接返回，相同才继续
	GetDlgItemText(IDC_ETNEWPWIN,strOld);//新密码
	GetDlgItemText(IDC_ETCONFIRMIN,strNew);//确认新密码
	if (strOld != strNew)
	{
		BZSoundPlay(this, "music/错误提示.mp3", 0);
		str = fMsg.GetKeyVal("BankDlg","ConfirmPasswdNotSame","新密码两次输入不一致，请重新输入!");
		wsprintf(szMsg, str.GetBuffer());
		
		//AFCMessageBox(szMsg);
        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",szMsg);
		return;
	}
	if ((strNew.GetLength()<6) ||(strNew.GetLength()>20))
	{
		BZSoundPlay(this, "music/错误提示.mp3", 0);
		str = fMsg.GetKeyVal("BankDlg","NewPasswdBytes","密码由6-20字符组成，请重新输入新密码！");
		wsprintf(szMsg, str.GetBuffer());
		
		//AFCMessageBox(szMsg);
        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",szMsg);
		return;
	}

	/// 新密码确认成功，取旧密码输入后，发送消息到父窗口
	GetDlgItemText(IDC_ETOLDPWIN,strOld);//旧密码

	ChangePasswd_t *pChangePasswd = new ChangePasswd_t();
	_tcscpy( pChangePasswd->szPwdOld, strOld.Left(20).GetBuffer() );
	_tcscpy( pChangePasswd->szPwdNew, strNew.Left(20).GetBuffer() );

	m_pGameRoom->PostMessage(WM_CHANGE_PASSWD, 0, LPARAM(pChangePasswd));
	SetDlgItemText(IDC_ETNEWPWIN,_T(""));//新密码
	SetDlgItemText(IDC_ETCONFIRMIN,_T(""));//确认新密码
	SetDlgItemText(IDC_ETOLDPWIN,_T(""));//旧密码
}
/// 存取钱后更新界面
/// @param Opt 0-表示取出，1-表示存入
/// @param nMoney 表示本次所操作的金币数
void CPBankDlg::UpdateUserMoney(int OptType, __int64 i64Money)
{
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	TCHAR szNum[128];
	GetNumString(szNum, i64Money, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
	CString strMsg;
	CString strFormat = fMsg.GetKeyVal("PBankDlg", "CheckInOutTip", "成功%s %s 金币");;
	if( OptType == 0 ) //取出
	{
		m_i64BankMoney	-= i64Money;
		m_i64WalletMoney += i64Money;
		strMsg.Format(strFormat, "取出", szNum);
	}
	else //存入
	{
		m_i64BankMoney	+= i64Money;
		m_i64WalletMoney -= i64Money;
		strMsg.Format(strFormat, "存入", szNum);
	}
	if (GetSafeHwnd()!=NULL)
	{
		ShowInterface();
		/// 提示玩家存取钱成功，
		SetDlgItemText(IDC_ETBREGPW, strMsg.GetBuffer());
		GetDlgItem(IDC_ETBREGPW)->ShowWindow(SW_SHOW);
	}
}
/// 转帐成功后更新银行数据
/// @param Opt 0-表示转出给别人，1-表示由别人转入给自己
/// @param nMoney 表示本次所操作的金币数
void CPBankDlg::UpdateUserMoneyByTransfer(int OptType, __int64 i64Money)
{
	if( OptType == 0 ) //取出
	{
		m_i64BankMoney	-= i64Money;
	}
	else //存入
	{
		m_i64BankMoney	+= i64Money;
	}
	if (GetSafeHwnd()!=NULL)
	{
		ShowInterface();
	}
}
void CPBankDlg::ShowMoneyBank()
{
	if (GetSafeHwnd()==NULL)
	{
		return;
	}
	TCHAR szNum[128], szBank[128];
	GetNumString(szBank, m_i64BankMoney, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
	GetNumString(szNum, m_i64WalletMoney, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
    SetDlgItemText(/*IDC_EDIT_BOXCOIN*/IDC_EDIT_GAMECOIN,szBank);//银行保险箱
	SetDlgItemText(IDC_EDIT_INFO_GOLD,szNum);//现在金币
}
///
/// 设置财富信息
/// @param nBank 银行存款
/// @param nWallet 手中现金
/// @return void
///
void CPBankDlg::SetWealthInfor(__int64 i64Bank, __int64 i64Wallet)
{
	m_i64BankMoney = i64Bank;
	m_i64WalletMoney = i64Wallet;

	TCHAR szNum[128] = {0};
	TCHAR szBank[128] = {0};
	GetNumString(szBank, m_i64BankMoney, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
	GetNumString(szNum, m_i64WalletMoney, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
	SetDlgItemText(/*IDC_EDIT_BOXCOIN*/IDC_EDIT_GAMECOIN,szBank);//银行保险箱
	SetDlgItemText(IDC_EDIT_INFO_GOLD,szNum);//现在金币
}

///
/// 帐号转账 的确定按钮响应函数
/// @param void
/// @return void
///
void CPBankDlg::OnBnClickedAccountOK()
{
    // PengJiLin, 2010-9-27, 全转 按钮，金葫芦二代功能
    if (1&Glb().m_CenterServerPara.m_nFunction)
    {
        if(m_Check_TransAll.GetCheck())
        {
            return OnBnClickedTransAll();
        }
    }

	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");

	/// 取得编辑框里的字符串
	TCHAR szMsg[256];
	CString strNum, str;
	GetDlgItemText(IDC_ETOUTCOIN, strNum);

	//如果有分隔符，则去除字符串里的分隔符
	if(Glb().m_bUseSpace)
		strNum.Replace(Glb().m_strSpaceChar,"");

	TCHAR szTail[16];
	_tcscpy(szTail, TEXT(""));
	for (int i=0; i<Glb().m_nPowerOfGold; ++i)
	{
		_tcscat( szTail, TEXT("0"));
	}
	/// 如果没有输入
	if (strNum.GetLength() == 0)
	{
		BZSoundPlay(this, "music/错误提示.mp3", 0);
		str = fMsg.GetKeyVal("BankDlg","NotInputCoin","请输入您要存取款的数额。");
		wsprintf(szMsg, str, Glb().m_nPowerOfGold+1, Glb().m_nPowerOfGold, szTail);
		
		//AFCMessageBox(szMsg);
        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",szMsg);
		return;
	}
	/// 如果输入太短
	if (strNum.GetLength()<Glb().m_nPowerOfGold)
	{
		BZSoundPlay(this, "music/错误提示.mp3", 0);
		str = fMsg.GetKeyVal("BankDlg","NotGetCoin","您输入的金币数额不被系统接受，请输入至少%d位，且末%d位为 %s 的正整数！");
		wsprintf(szMsg, str, Glb().m_nPowerOfGold+1, Glb().m_nPowerOfGold, szTail);
		
		//AFCMessageBox(szMsg);
        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",szMsg);
		return;
	}
	/// 如果输入太长
	if (strNum.GetLength()>18+Glb().m_nPowerOfGold)
	{
		BZSoundPlay(this, "music/错误提示.mp3", 0);
		str = fMsg.GetKeyVal("BankDlg","NumberTooLarge","您输入的数字太大，超出允许范围，请输入不大于%I64d%s，且末%d位为 %s 的正整数！");
		wsprintf(szMsg, str, 0x3FFFFFFFFFFFFFFF, szTail, Glb().m_nPowerOfGold, szTail);
		
		//AFCMessageBox(szMsg);
        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",szMsg);
		return;
	}

	/// 如果末几位不是零
	CString strSubNum = strNum.Right(Glb().m_nPowerOfGold);
	if (strSubNum != szTail)
	{
		BZSoundPlay(this, "music/错误提示.mp3", 0);
		TCHAR szMsg[256];
		str = fMsg.GetKeyVal("BankDlg","NotGetCoin2","您输入的金币数额不被系统接受，请输入末%d位为 %s 的正整数！");
		wsprintf(szMsg, str, Glb().m_nPowerOfGold, szTail);
		
		//AFCMessageBox(szMsg);
        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",szMsg);
		return;
	}
	/// 截取前N位
	strSubNum = strNum.Left(strNum.GetLength()-Glb().m_nPowerOfGold);
	__int64 i64Num = _atoi64(strSubNum.GetBuffer());
	if (i64Num>0x3FFFFFFFFFFFFFFF)
	{
		BZSoundPlay(this, "music/错误提示.mp3", 0);
		str = fMsg.GetKeyVal("BankDlg","NumberTooLarge","您输入的数字太大，超出允许范围，请输入不大于%I64d%s，且末%d位为 %s 的正整数！");
		wsprintf(szMsg, str, 0x3FFFFFFFFFFFFFFF, szTail, Glb().m_nPowerOfGold, szTail);
		
		//AFCMessageBox(szMsg);
        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",szMsg);
		return;
	}

	GetDlgItemText(IDC_ETOTHERID, strNum); //对方ID
	GetDlgItemText(IDC_ETPW,str);//二级密码
	if (str.GetLength()<1)
	{
		BZSoundPlay(this, "music/错误提示.mp3", 0);
		str = fMsg.GetKeyVal("BankDlg","PasswdBytes","请输入密码！");
		wsprintf(szMsg, str.GetBuffer());
		
		//AFCMessageBox(szMsg);
        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",szMsg);
		return;
	}
	if (i64Num<0)
	{
		BZSoundPlay(this, "music/错误提示.mp3", 0);
		str = fMsg.GetKeyVal("BankDlg","NotGetCoin","您输入的金币数额不被系统接受，请输入至少%d位，且末%d位为 %s 的正整数！");
		wsprintf(szMsg, str, Glb().m_nPowerOfGold+1, Glb().m_nPowerOfGold, szTail);
		
		//AFCMessageBox(szMsg);
        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",szMsg);
		return;
	}

	if (0 == atol(strNum) && m_bUseDestID)
	{
		BZSoundPlay(this, "music/错误提示.mp3", 0);
		str = fMsg.GetKeyVal("BankDlg","WrongID","请输入正确的玩家ID！");
		
		//AFCMessageBox(str);
        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",str);
		return;
	}

	TransferMoney_t *pTransferMoney = new TransferMoney_t();
	pTransferMoney->i64Money = i64Num;
	pTransferMoney->uDestUserID = atol(strNum);
	_tcscpy(pTransferMoney->szPWD, str.Left(20).GetBuffer());
	if (m_bUseDestID)
	{
		_tcscpy(pTransferMoney->szDestNickName, m_strNickName);
	}
	else
	{
		_tcscpy(pTransferMoney->szDestNickName, strNum.GetBuffer());
	}
	

	pTransferMoney->bUseDestID = m_bUseDestID;

	/// 提示用户确认转帐
	TCHAR szNum[128];
	GetNumString(szNum, pTransferMoney->i64Money, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
    //int iUseNickName = fMsg.GetKeyVal("PBankDlg","UseNickName",0);

	CString strFormat;
	CString test;
	test.Format("lxl is use nick name : %d",m_iUseNickName );
	OutputDebugString(test);
    if(m_bUseDestID)
    {
		BZSoundPlay(this, "music/错误提示.mp3", 0);
		strFormat = fMsg.GetKeyVal("PBankDlg", "TransferConfirmId", "您将要给ID号为[%d]玩家,昵称[%s]转帐%s金币，是否确定转帐？");
		wsprintf( szMsg, strFormat, pTransferMoney->uDestUserID,m_strNickName,szNum);
    }
    else
    {
		BZSoundPlay(this, "music/错误提示.mp3", 0);
        // lxl, 2010-11-17, 增加昵称显示
        strFormat = fMsg.GetKeyVal("PBankDlg", "TransferConfirmNn", "您将要给昵称[%s]的玩家转帐%s金币，是否确定转帐？");
        wsprintf( szMsg, strFormat, pTransferMoney->szDestNickName,szNum );
    }	

	//if (IDOK != AFCMessageBox(szMsg, "银行",AFC_YESNO))
	if (IDYES != DUIMessageBox(m_hWnd,MB_YESNO|MB_ICONINFORMATION,"银行",szMsg))
	{
		BZSoundPlay(this, "music/错误提示.mp3", 0);
		delete pTransferMoney;
		pTransferMoney = NULL;
		return;
	}

    // PengJiLin, 2010-8-23, 此处认为已经转账成功，用于刷新列表
    m_bTransferSuccess = TRUE;

	m_pGameRoom->PostMessage(WM_TRANSFER_MONEY, 0, LPARAM(pTransferMoney));
}

// PengJiLin, 2010-8-23, 全转 功能按钮
void CPBankDlg::OnBnClickedTransAll()
{
    CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");

    CString strID;
    CString strPSW;

    GetDlgItemText(IDC_ETOTHERID, strID);   // 对方ID
    GetDlgItemText(IDC_ETPW,strPSW);        // 二级密码
    if (strPSW.GetLength()<1)
    {
		BZSoundPlay(this, "music/错误提示.mp3", 0);
        
		//AFCMessageBox("请输入密码！");
        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","请输入密码！");
        return;
    }

    if (0 == atol(strID))
    {
		BZSoundPlay(this, "music/错误提示.mp3", 0);
        
		//AFCMessageBox("请输入正确的玩家ID！");
        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","请输入正确的玩家ID！");
        return;
    }

    /// 提示用户确认转帐
    CString strMsg;

    // PengJiLin, 2010-8-23, 增加昵称显示
    CString strFormat = fMsg.GetKeyVal("PBankDlg", "TransferAllConfirm", "您将要给ID[%s],昵称[%s]的玩家转帐银行中的 所有金币，是否确定转帐？");
    strMsg.Format(strFormat,strID, m_strNickName);
    //if (IDOK != AFCMessageBox(strMsg, "银行",AFC_YESNO))
    if (IDYES != DUIMessageBox(m_hWnd,MB_YESNO|MB_ICONINFORMATION,"银行",strMsg))
    {
        return;
    }

    TransferMoney_t *pTransferMoney = new TransferMoney_t();
    pTransferMoney->i64Money = 10000;     // 给个虚假的数
    pTransferMoney->uDestUserID = atol(strID);
    _tcscpy(pTransferMoney->szPWD, strPSW.Left(20).GetBuffer());

    // PengJiLin, 2010-8-23, 此处认为已经转账成功，用于刷新列表
    m_bTransferSuccess = TRUE;

    m_pGameRoom->PostMessage(WM_TRANSFER_ALL_MONEY, 0, LPARAM(pTransferMoney));
}

// PengJiLin, 2010-9-27, 全转 勾选框
void CPBankDlg::OnBnClickedCheckTransAll()
{
    CWnd * pWndBank;
    CWnd * pWndOut;
    pWndBank = GetDlgItem(IDC_ETCOIN);
    pWndOut = GetDlgItem(IDC_ETOUTCOIN);

    int iCheck = m_Check_TransAll.GetCheck();
    if(iCheck)
    {
        CString strMoney = "";
        pWndBank->GetWindowText(strMoney);
        pWndOut->SetWindowText(strMoney);
        pWndOut->EnableWindow(FALSE);
    }
    else
    {
        pWndOut->SetWindowText("");
        pWndOut->EnableWindow(TRUE);
    }
}

void CPBankDlg::OnBnClickedCoinAll()
{
	TCHAR szNum[128];
	if(m_uCheckInOrOut == 0)
	{
		int nLess = 0;
		if(m_pGameRoom != NULL)
		{
			nLess = ((CGameRoomEx*)m_pGameRoom)->m_GameInfo.uLessPoint;
		}
		GetNumString(szNum, m_i64WalletMoney-nLess, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
	}
	else
	{
		GetNumString(szNum, m_i64BankMoney, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
	}
	SetDlgItemText(IDC_ETBCONNOIN, szNum);
}

///
/// 设置 帐号转账 下的金币数
/// @param void
/// @return void
///
void CPBankDlg::SetAccountInfor()
{
	CString str;
	CString strTem;
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");

	TCHAR szNum[128], szBank[128];
	//str = fMsg.GetKeyVal("PBankDlg","UHaveCoin","您现有%s金币，%s游戏币。");
	str = fMsg.GetKeyVal("PBankDlg","UHaveCoin","您钱包中有%s金币。");
	GetNumString(szNum, m_i64WalletMoney, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
	GetNumString(szBank, m_i64BankMoney, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
	strTem.Format(str, szNum);
	SetDlgItemText(IDC_ETCOINNUM,strTem);
	SetDlgItemText(IDC_ETCOIN,szBank);
	str = fMsg.GetKeyVal("PBankDlg","CoinToGameCoin","转帐须扣除一定的手续费。");
	strTem.Format(str);
	SetDlgItemText(IDC_ETTRANSNUM,strTem);
}
/// 取得smallDateTime的字符串
/// @param[out] szBuffer 外部传入的字符串组，大小为256字符
/// @param[in] nSmallDateTime SQL_SERVER数据库里的smallDateTime类型
/// @return 返回szBuffer指针
TCHAR *GetTimeString(TCHAR *szBuffer, COleDateTime smallDateTime)
{
	//__time64_t tAll; 
	//tAll = HIWORD(nSmallDateTime)*24*60*60 + LOWORD(nSmallDateTime)*60; 
	//COleDateTime time(tAll); 
	_stprintf(szBuffer,_T("%02d-%02d-%02d %02d:%02d"),
		smallDateTime.GetYear(),
		smallDateTime.GetMonth(),
		smallDateTime.GetDay(),
		smallDateTime.GetHour(),
		smallDateTime.GetMinute()
		);
	return szBuffer;
}
///
/// 设置 插入转账数据
/// @param void
/// @return void
///
void CPBankDlg::InsertRecordList()
{
	TCHAR szTemp[256];
	if (!m_mapTransferRecord.empty())
	{
		m_SkinList.DeleteAllItems();
		int nCount = m_mapTransferRecord.size();
		for (int i=0; i<nCount; ++i)
		{
			wsprintf(szTemp, "%d", m_mapTransferRecord[i].nSeriNo);
			m_SkinList.InsertItem(i, szTemp);
			GetTimeString(szTemp, m_mapTransferRecord[i].oleDateTime);
			m_SkinList.SetItemText(i, 0, szTemp );
			wsprintf(szTemp, "%d", m_mapTransferRecord[i].dwUserID);
			m_SkinList.SetItemText(i, 1, szTemp );
			wsprintf(szTemp, "%d", m_mapTransferRecord[i].destUserID);
			m_SkinList.SetItemText(i, 2, szTemp );
			wsprintf(szTemp, "%I64d", m_mapTransferRecord[i].i64Money);
			m_SkinList.SetItemText(i, 3, szTemp );
		}
	}

	m_SkinList.SetRedraw(TRUE);
	ListView_SetExtendedListViewStyle(m_SkinList.m_hWnd,LVS_EX_FULLROWSELECT|LVS_EX_FLATSB|LVS_EX_INFOTIP|LVS_EX_GRIDLINES|LVS_EX_ONECLICKACTIVATE);
}

///
/// 刷新转账记录
/// @return void
///
void CPBankDlg::RefreshRecordList()
{
	m_SkinList.DeleteAllItems();
	m_pGameRoom->PostMessage(WM_TRANSFER_RECORD, 0, 0);
}
///
/// 收到一条转账记录
/// @param void
/// @return void
///
void CPBankDlg::OnReceivedTransferRecord(MSG_GR_R_TransferRecord_t *pRecord)
{
	TCHAR szTemp[256];
	m_mapTransferRecord[pRecord->nSeriNo] = *pRecord;
	wsprintf(szTemp, "%d", pRecord->nSeriNo);
	int nPos = m_SkinList.InsertItem(m_SkinList.GetItemCount(), szTemp);
	GetTimeString(szTemp, pRecord->oleDateTime);
	//wsprintf(szTemp, "%d", pRecord->uDateTime);
	m_SkinList.SetItemText(nPos, 0, szTemp );
	wsprintf(szTemp, "%d", pRecord->dwUserID);
	m_SkinList.SetItemText(nPos, 1, szTemp );
	wsprintf(szTemp, "%d", pRecord->destUserID);
	m_SkinList.SetItemText(nPos, 2, szTemp );
	GetNumString(szTemp, pRecord->i64Money, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
	m_SkinList.SetItemText(nPos, 3, szTemp );
}

///
/// 银行 的续费按钮响应函数
/// @param void
/// @return void
///
void CPBankDlg::OnBnClickedBankRecharge()
{
	return;
	CString str;
	str.Format(" Function  OnBnClickedBankRecharge()   Line %d",__LINE__);
	MessageBox(str);
}

///
/// 银行 的激活按钮响应函数
/// @param void
/// @return void
///
void CPBankDlg::OnBnClickedBankActive()
{
	return;
	CString str;
	str.Format(" Function  OnBnClickedBankActive()   Line %d",__LINE__);
	MessageBox(str);
}


///
/// 银行 的确定按钮响应函数
/// 须判断，到底是取钱还是存钱
/// @param void
/// @return void
///
void CPBankDlg::OnBnClickedBankOK()
{
	SetDlgItemText(IDC_ETBREGPW, "");
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");

	/// 取得编辑框里的字符串
	TCHAR szMsg[256];
	CString strNum, str;
	GetDlgItemText(IDC_ETBCONNOIN, strNum);

	//如果有分隔符，则去除字符串里的分隔符
	if(Glb().m_bUseSpace)
		strNum.Replace(Glb().m_strSpaceChar,"");

	TCHAR szTail[16];
	_tcscpy(szTail, TEXT(""));
	for (int i=0; i<Glb().m_nPowerOfGold; ++i)
	{
		_tcscat( szTail, TEXT("0"));
	}

	/// 如果没有输入
	if (strNum.GetLength() == 0)
	{
		BZSoundPlay(this, "music/错误提示.mp3", 0);
		str = fMsg.GetKeyVal("BankDlg","NotInputCoin","请输入您要存取款的数额。");
		wsprintf(szMsg, str, Glb().m_nPowerOfGold+1, Glb().m_nPowerOfGold, szTail);
		
		//AFCMessageBox(szMsg);
        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",szMsg);
		return;
	}
	/// 如果输入太短
	if (strNum.GetLength()<Glb().m_nPowerOfGold)
	{
		BZSoundPlay(this, "music/错误提示.mp3", 0);
		str = fMsg.GetKeyVal("BankDlg","NotGetCoin","您输入的金币数额不被系统接受，请输入至少%d位，且末%d位为 %s 的正整数！");
		wsprintf(szMsg, str, Glb().m_nPowerOfGold+1, Glb().m_nPowerOfGold, szTail);
		
		//AFCMessageBox(szMsg);
        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",szMsg);
		return;
	}
	/// 如果输入太长
	if (strNum.GetLength()>18 + Glb().m_nPowerOfGold)
	{
		BZSoundPlay(this, "music/错误提示.mp3", 0);
		str = fMsg.GetKeyVal("BankDlg","NumberTooLarge","您输入的数字太大，超出允许范围，请输入不大于%I64d%s，且末%d位为 %s 的正整数！");
		wsprintf(szMsg, str, 0x3FFFFFFFFFFFFFFF, szTail, Glb().m_nPowerOfGold, szTail);
		
		//AFCMessageBox(szMsg);
        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",szMsg);
		return;
	}

	/// 如果末几位不是零
	CString strSubNum = strNum.Right(Glb().m_nPowerOfGold);
	if (strSubNum != szTail)
	{
		BZSoundPlay(this, "music/错误提示.mp3", 0);
		TCHAR szMsg[256];
		str = fMsg.GetKeyVal("BankDlg","NotGetCoin2","您输入的金币数额不被系统接受，请输入末%d位为 %s 的正整数！");
		wsprintf(szMsg, str, Glb().m_nPowerOfGold, szTail);
		
		//AFCMessageBox(szMsg);
        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",szMsg);
		return;
	}
	/// 截取前N位
	if (Glb().m_nPowerOfGold >= 0) {
		strSubNum = strNum.Left(strNum.GetLength()-Glb().m_nPowerOfGold);
	} else {
		strSubNum = strNum.Left(strNum.GetLength());
	}
	
	__int64 i64Num = _atoi64(strSubNum.GetBuffer());
	int tmpPowerOfGod = Glb().m_nPowerOfGold;
	if (tmpPowerOfGod < 0) {
		tmpPowerOfGod = -tmpPowerOfGod;
		for (int i = 0; i < tmpPowerOfGod; i ++) {
			i64Num *= 10;
		}
	}
	if (i64Num>0x3FFFFFFFFFFFFFFF)
	{
		BZSoundPlay(this, "music/错误提示.mp3", 0);
		str = fMsg.GetKeyVal("BankDlg","NumberTooLarge","您输入的数字太大，超出允许范围，请输入不大于%I64d%s，且末%d位为 %s 的正整数！");
		wsprintf(szMsg, str, 0x3FFFFFFFFFFFFFFF, szTail, Glb().m_nPowerOfGold, szTail);
		
		//AFCMessageBox(szMsg);
        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",szMsg);
		return;
	}
	if (i64Num<0)
	{
		BZSoundPlay(this, "music/错误提示.mp3", 0);
		str = fMsg.GetKeyVal("BankDlg","NotGetCoin","您输入的金币数额不被系统接受，请输入至少%d位，且末%d位为 %s 的正整数！");
		wsprintf(szMsg, str, Glb().m_nPowerOfGold+1, Glb().m_nPowerOfGold, szTail);
		
		//AFCMessageBox(szMsg);
        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",szMsg);
		return;
	}

	/// 发消息
	if (m_pGameRoom!=NULL)
	{
		/// 无论是取还是存，都只是往父窗口发送一个消息即可
		/// 为统一接口，存钱也把银行密码发送到父窗口
		/// 金额，二级密码，取还是存
		GetDlgItemText(IDC_ETBCONNOIN,str);
		GetDlgItemText(IDC_ETBPWIN,str);
		
		if ((m_uCheckInOrOut==1) && (str.GetLength()<1))
		{
			BZSoundPlay(this, "music/错误提示.mp3", 0);
			str = fMsg.GetKeyVal("BankDlg","PasswdBytes","请输入密码！");
			wsprintf(szMsg, str.GetBuffer());
			
			//AFCMessageBox(szMsg);
            DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",szMsg);
			return;
		}
		CheckMoney_t *pCheckMoney = new CheckMoney_t;
		pCheckMoney->uType = m_uCheckInOrOut;
		pCheckMoney->i64Money = i64Num;
		_tcscpy(pCheckMoney->szPWD, str.Left(20).GetBuffer());
		m_pGameRoom->PostMessage(WM_CHECK_MONEY, 0, (LPARAM)pCheckMoney);
	}
}

// PengJiLin, 2010-8-23, 根据用户ID获取昵称
void CPBankDlg::OnIDKillFocus()
{
    //CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
    //int iUseNickName = fMsg.GetKeyVal("PBankDlg","UseNickName",0);
    if(m_iUseNickName <= 0) return;

    CString strTempID = "";
    GetDlgItemText(IDC_ETOTHERID, strTempID);   // 对方ID

    // 相同的输入
    if(m_strSaveID == strTempID)
    {
        return;
    }

    m_strNickName = " ";

    // 无效的输入
    if(atol(strTempID) <= 0)
    {
        return;
    }

    m_strSaveID = strTempID;

    m_Button_Trans_All.EnableWindow(FALSE);

	//用户选择输入目标用户ID的模式时，需要验证ID的合法性；
	//输入目标用户昵称暂不验证，日后可以完成这方面的骓；
	//Modified by JianGK 2011.11.1 
	if (m_bUseDestID)
	{
		 //m_btnTranOK.EnableWindow(FALSE);

		 // 发送 获取用户昵称 消息
		 GetNickNameOnID_t* pGetNickNameOnID = new GetNickNameOnID_t;
		 pGetNickNameOnID->uUserID = atol(strTempID);
		 m_pGameRoom->PostMessage(WM_BANK_GET_NICKNAME_ONID, 0, LPARAM(pGetNickNameOnID));
	}
	else
	{
		//m_btnTranOK.EnableWindow(TRUE);
	}
}

// PengJiLin, 2010-8-23, 根据用户ID获取的昵称
// 设置获取的昵称
void CPBankDlg::SetNickName(TCHAR* szNickName)
{
    m_strNickName = szNickName;

    m_Button_Trans_All.EnableWindow(TRUE);
    //m_btnTranOK.EnableWindow(TRUE);
}

// lxl 2010-11-19  金额转换为中文大写
void CPBankDlg::ChangeDigitToChinese(__int64 i64Num, CString &strResult)
{
	int i,size,high;

	strResult = "";
	digitArray.clear();
	i = 0;
	size = 0;

	//将数值的数字提出来放在数组中，逆序排序
	while(i64Num)
	{
		digitArray.push_back(i64Num%10);
		i64Num /=10;
		size++;
	}

	for(i = size-1; i >= 0; i--)
	{
		//加入数字
		if(digitArray[i] == 0 )//数字为零情况处理
		{
			if(i-1 == -1) break; 
			if(digitArray[i-1] != 0 && (szUnit[i] != "万" && szUnit[i] != "亿" )) //非亿、万位为0时处理，不连续时加入零。
				strResult += szDigit[digitArray[i]];
		}
		else 
			strResult += szDigit[digitArray[i]];		
	
		//加入单位
		if(digitArray[i] == 0 && (szUnit[i] != "万" && szUnit[i] != "亿" ) ) //非亿、万位为0时跳过
			continue; 
		strResult += szUnit[i];
	}
	if(strResult.Find("亿万") != -1)//字符串中有亿万连续的特殊处理
		strResult.Replace("亿万", "亿");
}
//转账金额大写显示
void CPBankDlg::OnEnChangeMoney()
{	
	CString strGetCoin = "";
	GetDlgItemText(IDC_ETBCONNOIN, strGetCoin);   // 获取金额ID

	//去除输入框中的分隔符号
	if(Glb().m_bUseSpace)
		strGetCoin.Replace(Glb().m_strSpaceChar, "");	//银行页面
		
	//验证字符串是否是0-9的数字
	for(int i = 0; i < strGetCoin.GetLength(); i++)
	{
		if(strGetCoin[i] < '0' || strGetCoin[i] > '9')
		{
			SetDlgItemText(IDC_ETCHINESE, "");
			return;
		}
	}	

	__int64 i64Num  = _tstoi64(strGetCoin.GetBuffer());
	TCHAR szNum[128];

	if(bFlag == false)	
	{		
		//字符串中加入分隔符
		GetNumString(szNum, i64Num, 0, Glb().m_bUseSpace, Glb().m_strSpaceChar);
		bFlag = true;
		SetDlgItemText(IDC_ETBCONNOIN, szNum);
		//设置光标到最后面
		CEdit* pEdit = (CEdit*)GetDlgItem(IDC_ETBCONNOIN);
		pEdit->SetSel(-1);		
	}
	else 
		bFlag = false;

	ChangeDigitToChinese(i64Num, strGetCoin);

	SetDlgItemText(IDC_ET_MONEY, strGetCoin);	
}
//转账金额大写显示
void CPBankDlg::OnEnChangeEtbconnoin()
{
	CString strOutCoin = "";
    GetDlgItemText(IDC_ETOUTCOIN, strOutCoin);   // 转账金额ID	
	
	//去除输入框中的分隔符号
	if(Glb().m_bUseSpace)
		strOutCoin.Replace(Glb().m_strSpaceChar, "");	//转帐页面		
	
	//验证字符串是否是0-9的数字
	for(int i = 0; i < strOutCoin.GetLength(); i++)
	{
		if(strOutCoin[i] < '0' || strOutCoin[i] > '9')
		{
			SetDlgItemText(IDC_ETCHINESE, "");
			return;
		}
	}

	__int64 i64Num  = _tstoi64(strOutCoin.GetBuffer());
	TCHAR szNum[128];

	if(bFlag == false)	
	{
		//字符串中加入分隔符
		GetNumString(szNum, i64Num, 0, Glb().m_bUseSpace, Glb().m_strSpaceChar);
		bFlag = true;
		SetDlgItemText(IDC_ETOUTCOIN, szNum);
		//设置光标到最后面
		CEdit* pEdit = (CEdit*)GetDlgItem(IDC_ETOUTCOIN);
		pEdit->SetSel(-1);				
	}
	else 
		bFlag = false;

	ChangeDigitToChinese(i64Num, strOutCoin);	

	SetDlgItemText(IDC_ETCHINESE, strOutCoin);			
}