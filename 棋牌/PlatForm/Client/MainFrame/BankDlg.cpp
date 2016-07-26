// BankDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "BankDlg.h"
#include "MainRoomEx.h"


// CBankDlg dialog

IMPLEMENT_DYNAMIC(CBankDlg, CDialog)

CBankDlg::CBankDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOGBANK, pParent)
	, m_BankMoney(0)
	, m_RoomMoney(0)
	, m_UserID(0)
	, m_RoomName("")
	, m_uComType(0)
	, m_pMeUserInfo(NULL)
	, m_Connection(NULL)
{
	m_bkBrush = NULL;
}

CBankDlg::~CBankDlg()
{
	DeleteObject(m_bkBrush);
}

void CBankDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDCANCEL, m_BtCancel);
	DDX_Control(pDX, IDC_CHECKOUT,m_btCheckOut);
	DDX_Control(pDX, IDC_CHECKIN,m_btCheckIn);
	DDX_Control(pDX, IDC_BTN_TRANSFER,m_btTransfer);
	DDX_Control(pDX, IDC_QUERY,m_btQuery);
}

void CBankDlg::SetBankMoney(int count)
{
	m_BankMoney=count;
	//TCHAR buffer[50];
	//ltoa(m_BankMoney,buffer,10);
	//SetDlgItemTextA(IDC_EDIT_MONEY,buffer);
}
void CBankDlg::SetRoomMoney(int count)
{
	m_RoomMoney=count;
	//TCHAR buffer[50];
	//ltoa(m_RoomMoney,buffer,10);
	//SetDlgItemTextA(IDC_EDIT_MONEY_ROOM,buffer);
}
int CBankDlg::GetBankMoney()
{
	return m_BankMoney;
}
void CBankDlg::GetUserID(long UserID)
{
	m_UserID=UserID;
}
//初始化函数
BOOL CBankDlg::OnInitDialog()
{
	__super::OnInitDialog();
	TCHAR buffer[128], szTip[50];

	

	//贴图
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f( s + "skin0.bcf");
	CString key=TEXT("Bank");
	TCHAR path[MAX_PATH];
	CString skinfolder;
	skinfolder=f.GetKeyVal(Glb().m_skin,"skinfolder","image\\client\\skin0\\");
	CGameImage	m_bt;

	/*// 2012.12.17 yyf 屏蔽
	wsprintf(path,"%sdialog\\msg_bk.bmp",skinfolder);
	m_bkimage.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	CGameImageHelper ImageHandle(&m_bkimage);
	int cx,cy;
	HRGN hRgn=AFCBmpToRgn(ImageHandle,m_bkimage.GetPixel(0,0),RGB(1,1,1));
	if (hRgn!=NULL)
	{
		SetWindowRgn(hRgn,TRUE);
		///{{ Added by zxd 20100709 释放区域对象
		DeleteObject(hRgn);
		///Added by zxd 20100709 释放区域对象}}
	}
	cx=ImageHandle.GetWidth();
	cy=ImageHandle.GetHeight();*/
	
	//*-------初始化 m_configDlg--------- 2012.10.17 yyf
	TCHAR pathConfig[MAX_PATH];
	TCHAR PathDefault[MAX_PATH];
	
	wsprintf(pathConfig,"%s%sdialog\\configDlg\\CBankDlgConfig\\configDlg.bcf",s,skinfolder);// 
	wsprintf(PathDefault,"%s%s",s,skinfolder);
	m_configDlg.LoadConfig(pathConfig,PathDefault);	

	int cx,cy; 
	cx=m_configDlg.GetDlgWide();
	cy=m_configDlg.GetDlgHigh(); 
	//-----------------------------------------------------*/
	SetWindowPos(NULL,0,0,cx,cy,SWP_NOMOVE|SWP_NOZORDER);

	//wsprintf(path,"%sdialog\\WalletCancle_bt.BMP",skinfolder);
	//m_bt.SetLoadInfo(path,true);
	//m_btcancel.LoadButtonBitmap(path,false);
//	CGameImage		m_bt;
	HRGN hRgn2;
	wsprintf(path,"%sdialog\\log_exit_bt.bmp",skinfolder);//win_close.bmp
	m_BtCancel.LoadButtonBitmap(path,false);
	m_bt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	//hRgn2=AFCBmpToRgn(m_bt,m_bt.GetPixel(0,0),RGB(1,1,1));
	//if(hRgn2)
	//	m_BtCancel.SetWindowRgn(hRgn2,true);
	m_BtCancel.SetWindowPos(NULL,0,0,m_bt.GetWidth()/4,m_bt.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	//wsprintf(path,"%sdialog\\OutMoney_bt.bmp",skinfolder);
	//m_bt.SetLoadInfo(path,true);
	wsprintf(path,"%slogon\\button.bmp",skinfolder);
	m_bt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
//	CGameImageHelper bthelp(&m_bt);
	hRgn2=AFCBmpToRgn(m_bt,m_bt.GetPixel(0,0),RGB(1,1,1));
	m_btCheckOut.LoadButtonBitmap(path,false);
	if(hRgn2)
		m_btCheckOut.SetWindowRgn(hRgn2,true);
	//m_btCheckOut.SetWindowPos(NULL,0,0,m_bt.GetWidth()/4,m_bt.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	//wsprintf(path,"%sdialog\\InMoney_bt.bmp",skinfolder);
	//m_bt.SetLoadInfo(path,true);
	m_btCheckIn.LoadButtonBitmap(path,false);
	if(hRgn2)
		m_btCheckIn.SetWindowRgn(hRgn2,true);

	m_btQuery.LoadButtonBitmap(path,false);
	if(hRgn2)
		m_btQuery.SetWindowRgn(hRgn2,true);
	//m_btCheckIn.SetWindowPos(NULL,0,0,m_bt.GetWidth()/4,m_bt.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);
	
	m_btTransfer.LoadButtonBitmap(path,false);
	if(hRgn2)
		m_btTransfer.SetWindowRgn(hRgn2,true);
	//m_btTransfer.SetWindowPos(NULL,0,0,m_bt.GetWidth()/4,m_bt.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	ZeroMemory(buffer,sizeof(buffer));

	GetNumString(buffer, m_BankMoney, Glb().m_nPowerOfGold/*true*/,Glb().m_bUseSpace, Glb().m_strSpaceChar);
	SetDlgItemTextA(IDC_EDIT_MONEY,buffer);
	GetNumString(buffer, m_RoomMoney, Glb().m_nPowerOfGold/*true*/,Glb().m_bUseSpace, Glb().m_strSpaceChar);
	SetDlgItemTextA(IDC_EDIT_MONEY_ROOM,buffer);
	ZeroMemory(buffer,sizeof(buffer));

	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	CString str;

	switch(Glb().m_nPowerOfGold)
	{
	case 0:
		_tcscat( buffer, TEXT("1金币"));
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

	str = fMsg.GetKeyVal("BankDlg","SaveUnit","存取最小单位：百");
	_tcscpy(szTip,str.GetBuffer());
	_tcscat(szTip, buffer);
	str = fMsg.GetKeyVal("BankDlg","Coin","金币");
	_tcscat( buffer, str.GetBuffer());
	SetDlgItemTextA(IDC_STATIC_TIP, szTip);

	m_btCheckOut.SetWindowText(fMsg.GetKeyVal("BankDlg","BTNGet","取出"));
	m_btCheckIn.SetWindowText(fMsg.GetKeyVal("BankDlg","BTNSave","存入"));
	m_btQuery.SetWindowText(fMsg.GetKeyVal("BankDlg","BTNInquire","查询"));
	m_btTransfer.SetWindowText(fMsg.GetKeyVal("BankDlg","BTNTransfer","转账"));

	SetDlgItemText(IDC_STATIC_BANK,fMsg.GetKeyVal("BankDlg","Bank","银行："));
	SetDlgItemText(IDC_STATIC_BCOIN,fMsg.GetKeyVal("BankDlg","Coin","金币"));
	SetDlgItemText(IDC_STATIC_WALLET,fMsg.GetKeyVal("BankDlg","Coin","钱包："));
	SetDlgItemText(IDC_STATIC_WCOIN,fMsg.GetKeyVal("BankDlg","Coin","金币"));
	SetDlgItemText(IDC_STATIC_COUNT,fMsg.GetKeyVal("BankDlg","SaveGetCount","存/取数量："));
	SetDlgItemText(IDC_UNIT,fMsg.GetKeyVal("BankDlg","Coin","金币"));
	SetDlgItemText(IDC_STATIC_TIP,fMsg.GetKeyVal("BankDlg","SaveUnit","存取最小单位：百"));


	//m_bkBrush = CreateSolidBrush(m_bkimage.GetPixel(32,55));//
	m_bkBrush = CreateSolidBrush(m_configDlg.GetBKImageEX().GetPixel(32,55));//2012.10.17 yyf
	//_tcscpy( buffer, TEXT("1"));
	//for (int i=0; i<Glb().m_nPowerOfGold; ++i)
	//{
	//	_tcscat( buffer, TEXT("0"));
	//}
	//wsprintf( szTip, "X %s", buffer);
	//SetDlgItemTextA(IDC_UNIT, szTip);
	
	SetWndRgn();// 设置窗口圆角2012.10.17 yyf
	return true;
}
BEGIN_MESSAGE_MAP(CBankDlg, CDialog)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_CHECKOUT, &CBankDlg::OnBnClickedCheckout)
	ON_BN_CLICKED(IDC_CHECKIN, &CBankDlg::OnBnClickedCheckin)
	ON_BN_CLICKED(IDC_BTN_TRANSFER, &CBankDlg::OnBnClickedTransfer)
	ON_BN_CLICKED(IDC_QUERY, &CBankDlg::OnBnClickedQuery)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// CBankDlg message handlers

void CBankDlg::OnBnClickedCheckout()
{
	// TODO: Add your control notification handler code here
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");

	if(m_BankMoney<=0)
	{
		//if(IDOK == AFCMessageBox(fMsg.GetKeyVal("BankDlg","NoCoinCharge","您已无金币，请充值!"),
		//	fMsg.GetKeyVal("BankDlg","TipBank","银行"),AFC_YESNO))
		if(IDYES == DUIMessageBox(m_hWnd,MB_YESNO|MB_ICONINFORMATION,fMsg.GetKeyVal("BankDlg","TipBank","银行"),fMsg.GetKeyVal("BankDlg","NoCoinCharge","您已无金币，请充值!")))
		{
			CString s=CBcfFile::GetAppPath ();/////本地路径
			CBcfFile f( s + "bzgame.bcf");
			CString key="BZW";
			CString url;
			TCHAR szUrl[20];
			wsprintf(szUrl,"bankurl");
			url=f.GetKeyVal(key,szUrl,Glb().m_CenterServerPara.m_strHomeADDR);
			//IE(url);
			ShellExecute(NULL,"open",url,NULL,NULL,SW_MAXIMIZE|SW_SHOW);
		}
		return;
	}
//	CDlgOutMoney Out;
//	Out.m_RoomCheckOut=m_RoomName;
//	if(Out.DoModal()==IDOK)
	//{

	//	long Out = GetDlgItemInt(IDC_CHECK_NUM);
	//	MSG_GR_S_CheckMoney checkMoney;
	//	memset(&checkMoney,0,sizeof(checkMoney));
	//	checkMoney.uCount=Out;
	//	checkMoney.UserID=m_UserID;	
	//	if(checkMoney.uCount>UINT(m_BankMoney)||checkMoney.uCount<=0)
	//	{
	//		AFCMessageBox("您无法取出此数额的金币!");
	//		return ;
	//	}

		/// 取得编辑框里的字符串
		TCHAR szMsg[256];
		CString strNum;
		GetDlgItemText(IDC_CHECK_NUM, strNum);
		TCHAR szTail[16];
		_tcscpy(szTail, TEXT(""));
		for (int i=0; i<Glb().m_nPowerOfGold; ++i)
		{
			_tcscat( szTail, TEXT("0"));
		}
		/// 如果输入太短
		if (strNum.GetLength()<Glb().m_nPowerOfGold)
		{
			CString str = fMsg.GetKeyVal("BankDlg","NotGetCoin","您无法取出此数额的金币，请输入至少%d位，且末%d位为 %s 的正整数！");
			wsprintf(szMsg, str, Glb().m_nPowerOfGold+1, Glb().m_nPowerOfGold, szTail);
			
			//AFCMessageBox(szMsg);
		    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",szMsg);

			return;
		}
		/// 如果输入太长
		if (strNum.GetLength()>10+Glb().m_nPowerOfGold)
		{
			CString str = fMsg.GetKeyVal("BankDlg","NumberTooLarge","您输入的数字太大，超出允许范围，请输入不大于%d%s，且末%d位为 %s 的正整数！");
			wsprintf(szMsg, str, 0x7FFFFFFF, szTail, Glb().m_nPowerOfGold, szTail);
			
			//AFCMessageBox(szMsg);
		    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",szMsg);

			return;
		}

		/// 如果末几位不是零
		CString strSubNum = strNum.Right(Glb().m_nPowerOfGold);
		if (strSubNum != szTail)
		{
			TCHAR szMsg[256];
			CString str = fMsg.GetKeyVal("BankDlg","NotGetCoin2","您无法取出此数额的金币，请输入末%d位为 %s 的正整数！");
			wsprintf(szMsg, str, Glb().m_nPowerOfGold, szTail);
			
			//AFCMessageBox(szMsg);
		    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",szMsg);
			return;
		}
		/// 截取前N位
		strSubNum = strNum.Left(strNum.GetLength()-Glb().m_nPowerOfGold);
		__int64 i64Num = _atoi64(strSubNum.GetBuffer());
		if (i64Num>0x7FFFFFFF)
		{
			CString str = fMsg.GetKeyVal("BankDlg","NumberTooLarge","您输入的数字太大，超出允许范围，请输入不大于%d%s，且末%d位为 %s 的正整数！");
			wsprintf(szMsg, str, 0x7FFFFFFF, szTail, Glb().m_nPowerOfGold, szTail);
			
			//AFCMessageBox(szMsg);
		    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",szMsg);
			return;
		}

		/// 取得字符串并发出
		MSG_GR_S_CheckMoney checkMoney;
		memset(&checkMoney,0,sizeof(checkMoney));
		checkMoney.i64Count = i64Num;
		checkMoney.UserID = m_UserID;
		GetDlgItem(IDC_CHECKOUT)->EnableWindow(false);
		GetDlgItem(IDC_CHECKIN)->EnableWindow(false);
		m_TCPSocket->SendData(&checkMoney,sizeof(checkMoney),MDM_GR_MONEY,ASS_GR_CHECK_OUT,0);

}

void CBankDlg::OnBnClickedCheckin()
{
	// TODO: Add your control notification handler code here

//	CDlgInMoney In;
//	In.m_RoomCheckIn=m_RoomName;
//	if(In.DoModal()==IDOK)

	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	{
		//本房间或其他已经开始游戏，则不能存钱
		RoomInfoStruct* pRoomInfo;
		if(GetMainRoom()->GetRoomInfoStruct(&pRoomInfo))
		{
			for (BYTE i=0;i<MAX_GAME_ROOM;i++)
			{
				if (pRoomInfo[i].bAccess)//判断是否在游戏中
				{
					CGameRoomEx *p = (CGameRoomEx*)pRoomInfo[i].pGameRoomWnd;
					if(!p->CanCloseNow())
					{
						//AFCMessageBox(fMsg.GetKeyVal("BankDlg","StartingNotSave","游戏已经开始无法存入金币!"));
		                DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",fMsg.GetKeyVal("BankDlg","StartingNotSave","游戏已经开始无法存入金币!"));
						return ;
					}
				}
			}
		}


		//防止在游戏时进行银行操作
		if (m_pMeUserInfo->GameUserInfo.bUserState == USER_PLAY_GAME)
		{
			//AFCMessageBox(fMsg.GetKeyVal("BankDlg","PlayingNotSave","您当前正在游戏中，无法进行存入金币操作!"));
		    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",fMsg.GetKeyVal("BankDlg","PlayingNotSave","您当前正在游戏中，无法进行存入金币操作!"));
			return ;
		}

		/// 取得编辑框里的字符串
		TCHAR szMsg[256];
		CString strNum;
		GetDlgItemText(IDC_CHECK_NUM, strNum);
		TCHAR szTail[16];
		_tcscpy(szTail, TEXT(""));
		for (int i=0; i<Glb().m_nPowerOfGold; ++i)
		{
			_tcscat( szTail, TEXT("0"));
		}
		/// 如果输入太短
		if (strNum.GetLength()<Glb().m_nPowerOfGold)
		{
			CString str = fMsg.GetKeyVal("BankDlg","NotSaveCoin","您无法存入此数额的金币，请输入至少%d位，且末%d位为 %s 的正整数！");
			wsprintf(szMsg,str, Glb().m_nPowerOfGold+1, Glb().m_nPowerOfGold, szTail);
			
			//AFCMessageBox(szMsg);
		    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",szMsg);
			return;
		}
		/// 如果输入太长
		if (strNum.GetLength()>10+Glb().m_nPowerOfGold)
		{
			CString str = fMsg.GetKeyVal("BankDlg","NumberTooLarge","您输入的数字太大，超出允许范围，请输入不大于%d%s，且末%d位为 %s 的正整数！");
			wsprintf(szMsg,str, 0x7FFFFFFF, szTail, Glb().m_nPowerOfGold, szTail);
			
			//AFCMessageBox(szMsg);
		    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",szMsg);
			return;
		}

		/// 如果末几位不是零
		CString strSubNum = strNum.Right(Glb().m_nPowerOfGold);
		if (strSubNum != szTail)
		{
			TCHAR szMsg[256];
			CString str = fMsg.GetKeyVal("BankDlg","NotSaveCoin2","您无法存入此数额的金币，请输入末%d位为 %s 的正整数！");
			wsprintf(szMsg,str, Glb().m_nPowerOfGold, szTail);
			
			//AFCMessageBox(szMsg);
		    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",szMsg);
			return;
		}
		/// 截取前N位
		strSubNum = strNum.Left(strNum.GetLength()-Glb().m_nPowerOfGold);
		__int64 i64Num = _atoi64(strSubNum.GetBuffer());
		if (i64Num>0x7FFFFFFF)
		{
			CString str = fMsg.GetKeyVal("BankDlg","NumberTooLarge","您输入的数字太大，超出允许范围，请输入不大于%d%s，且末%d位为 %s 的正整数！");
			wsprintf(szMsg,str, 0x7FFFFFFF, szTail, Glb().m_nPowerOfGold, szTail);
			
			//AFCMessageBox(szMsg);
		    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",szMsg);
			return;
		}

		/// 取得字符串并发出
		MSG_GR_S_CheckMoney checkMoney;
		memset(&checkMoney,0,sizeof(checkMoney));
		checkMoney.i64Count = i64Num;
		checkMoney.UserID = m_UserID;
		GetDlgItem(IDC_CHECKOUT)->EnableWindow(false);
		GetDlgItem(IDC_CHECKIN)->EnableWindow(false);
		m_TCPSocket->SendData(&checkMoney,sizeof(checkMoney),MDM_GR_MONEY,ASS_GR_CHECK_IN,0);
	}
}
//转帐
void CBankDlg::OnBnClickedTransfer()
{
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f( s + "bzgame.bcf");
	CString key="BZW";
	CString url;
	TCHAR szUrl[20];
	wsprintf(szUrl,"bankurl");
	url=f.GetKeyVal(key,szUrl,Glb().m_CenterServerPara.m_strHomeADDR);
	TCHAR surl[512];
	wsprintf( surl, "%s?userid=%d&token=%s&x=%d", url, m_pMeUserInfo->GameUserInfo.dwUserID, Glb().m_TML_SN, rand()%0x7FFF);
	ShellExecute(NULL,"open",surl,NULL,NULL,SW_MAXIMIZE|SW_SHOW);
	return;
}


///查询
void CBankDlg::OnBnClickedQuery()
{
	CString s=CBcfFile::GetAppPath();/////本地路径
	CBcfFile f( s + "bzgame.bcf");
	CString key="BZW";
	CString url;
	TCHAR szUrl[20];
	wsprintf(szUrl,"bankurl_Query");
	url=f.GetKeyVal(key,szUrl,Glb().m_CenterServerPara.m_strHomeADDR);
	TCHAR surl[512];
	wsprintf( surl, "%s?userid=%d&token=%s", url, m_pMeUserInfo->GameUserInfo.dwUserID, Glb().m_TML_SN);
	ShellExecute(NULL,"open",surl,NULL,NULL,SW_MAXIMIZE|SW_SHOW);
	return;
}

void CBankDlg::SetSocketPoint(CTCPClientSocket* m_TCPSocket)	
{
	this->m_TCPSocket=m_TCPSocket;
}
void CBankDlg::GetRoomName(TCHAR * roomName)
{
	m_RoomName=roomName;
}
void CBankDlg::GetConnectPoint(CConnectDlg *connection)
{
	m_Connection=new CConnectDlg;
	m_Connection=connection;
}
void CBankDlg::getUserItem(UserItemStruct * pMeUserInf)
{
	m_pMeUserInfo=pMeUserInf;
}
bool CBankDlg::GetRoomType(int uComType)
{
	if(uComType<1||uComType>3)
	{
		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");

		//AFCMessageBox(fMsg.GetKeyVal("BankDlg","RoomTypeError","房间类型错误!"));
		DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",fMsg.GetKeyVal("BankDlg","RoomTypeError","房间类型错误!"));
		return false;
	}
	m_uComType=uComType;
	return true;
}

void CBankDlg::UpdateUserMoney(int OptType, long nMoney)
{
	if( OptType == 0 ) //取出
	{
		m_BankMoney -= nMoney;
		m_RoomMoney +=nMoney;
	}
	else //存入
	{
		m_BankMoney += nMoney;
		m_RoomMoney -= nMoney;
	}
	if( GetSafeHwnd() != NULL )
	{
		CString strBank, strRoom;
		TCHAR szNum[128];
		GetNumString(szNum, m_BankMoney, Glb().m_nPowerOfGold/*true*/,Glb().m_bUseSpace, Glb().m_strSpaceChar);
		strBank.Format("%s", szNum);
		GetNumString(szNum, m_RoomMoney, Glb().m_nPowerOfGold/*true*/,Glb().m_bUseSpace, Glb().m_strSpaceChar);
		strRoom.Format("%s", szNum);
		GetDlgItem(IDC_EDIT_MONEY)->SetWindowText(strBank);
		GetDlgItem(IDC_EDIT_MONEY_ROOM)->SetWindowText(strRoom);
	}
}

void CBankDlg::OnPaint()
{
	CPaintDC dc(this);

	/*// 2012.12.17 yyf 屏蔽
	CGameImageHelper	handle(&m_bkimage);
	handle.BitBlt(dc.GetSafeHdc(),0,0);*/
	
	//绘画框架底图 2012.10.17 yyf
	m_configDlg.DrawViewFrame(&dc);

//	CFont Font;//,*OldFont;
//	Font.CreateFont(15,10,0,0,0,0,0,0,GB2312_CHARSET
//	,3,2,1,2,TEXT("宋体"));
//	CWnd *pWnd=GetDlgItem(IDC_EDIT_MONEY);
//	CDC *pDC=pWnd->GetDC();
//	pDC->SelectObject(&Font);
//	pWnd->SetFont(&Font,true);
////	DeleteObject(pDC->SelectObject(OldFont));
//	ReleaseDC(pDC);
//
//	pWnd=GetDlgItem(IDC_EDIT_MONEY_ROOM);
//	pDC=pWnd->GetDC();
//	pDC->SelectObject(&Font);
//	pWnd->SetFont(&Font,true);
////	DeleteObject(pDC->SelectObject(OldFont));
//	ReleaseDC(pDC);

	CDC *pdc=this->GetWindowDC();
	if(!pdc)
		return;
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");

	CRect rc;
	this->GetWindowRect(&rc);
	Graphics g(pdc->m_hDC);

	CString s="宋体";
	Font font(s.AllocSysString(),12,0,UnitPixel);
	SolidBrush brush(Color(255,255,255));
	s = fMsg.GetKeyVal("BankDlg","PersonalBank","个人银行");
//	s="个人银行";
	g.DrawString(s.AllocSysString(),-1,&font,PointF(8,8),0,&brush);
	g.ReleaseHDC(pdc->m_hDC);

}
LRESULT CBankDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	//case WM_CTLCOLORSCROLLBAR :
	//case WM_CTLCOLORBTN:
	//case WM_CTLCOLORMSGBOX:
	//case WM_CTLCOLOREDIT:
	//case WM_CTLCOLORLISTBOX:
	//case WM_CTLCOLORDLG:
	case WM_CTLCOLORSTATIC :
		SetTextColor((HDC)wParam, RGB(0,0,0)) ;
		//SetBkColor((HDC)wParam, m_bkimage.GetPixel(32,55));
		SetBkColor((HDC)wParam, m_configDlg.GetBKImageEX().GetPixel(32,55));//2012.10.17 yyf
		return (LRESULT)m_bkBrush;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void CBankDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
	
}

// 设置窗口圆角 2012.10.15 yyf
void CBankDlg::SetWndRgn(void)
{
	POINT m_arPoints[8];
	CRect winRect;
	GetWindowRect(&winRect);
	ScreenToClient(&winRect);
 
    //左上角
	m_arPoints[0].x = winRect.left;
	m_arPoints[0].y = winRect.top+3;
	m_arPoints[1].x = winRect.left+3;
	m_arPoints[1].y = winRect.top;

	//右上角
	m_arPoints[2].x = winRect.right-3;
	m_arPoints[2].y = winRect.top;
	m_arPoints[3].x = winRect.right;
	m_arPoints[3].y = winRect.top+3;

	//右下角
	m_arPoints[4].x = winRect.right;
	m_arPoints[4].y = winRect.bottom-4;
	m_arPoints[5].x = winRect.right-4;
	m_arPoints[5].y = winRect.bottom;

	//左下角
	m_arPoints[6].x = winRect.left+4;
	m_arPoints[6].y = winRect.bottom;
	m_arPoints[7].x = winRect.left;
	m_arPoints[7].y = winRect.bottom-4;

	HRGN hRgn = CreatePolygonRgn(m_arPoints, 8, WINDING);
	if (hRgn!=NULL)
	{
		SetWindowRgn(hRgn,TRUE);
		DeleteObject(hRgn);
	}
}