#include "Stdafx.h"
#include "resource.h"
#include "checkPSW.h"

//加密密钥长度
#define KEY_LENGTH		10

char		CCheckPSW::m_szScrPass[61];					//用户密码
char		CCheckPSW::m_szMD5Pass[50];					//加密后的密码

BEGIN_MESSAGE_MAP(CCheckPSW, CLoadFileBmp)

	ON_WM_PAINT()
END_MESSAGE_MAP()


//构造函数
CCheckPSW::CCheckPSW() : CLoadFileBmp(IDD_CHECKPSW)
{

}

//析构函数
CCheckPSW::~CCheckPSW()
{
}

BOOL CCheckPSW::OnInitDialog()
{
	CLoadFileBmp::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CString s=CBcfFile::GetAppPath();/////本地路径
	CBcfFile f( s + "skin0.bcf");
	CString key=Glb().m_skin;
	TCHAR path[MAX_PATH];
	CString skinfolder;
	skinfolder=f.GetKeyVal(Glb().m_skin,"skinfolder","image\\client\\skin0\\");

	wsprintf(path,"%slogon\\check_psw.bmp",skinfolder);
	m_bk.SetLoadInfo(path,TRUE);
	CenterWindow();
	CGameImageHelper help(&m_bk);
	HRGN hRgn;
	hRgn = AFCBmpToRgn(help,m_bk.GetPixel(0,0),RGB(1,1,1));
	if(hRgn)
		SetWindowRgn(hRgn,TRUE);
	
	((CEdit*)GetDlgItem(IDC_EDIT_TWOPSW))->SetLimitText(20);
	((CEdit*)GetDlgItem(IDC_EDIT_TWOPSW))->MoveWindow(130,67,80,16);
	CGameImage		m_bt;
	wsprintf(path,"%slogon\\button.bmp",skinfolder);
	m_BtCancel.LoadButtonBitmap(path,false);
	m_bt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	CGameImageHelper bthelp(&m_bt);
	HRGN hRgn2;
	hRgn2=AFCBmpToRgn(bthelp,m_bt.GetPixel(0,0),RGB(1,1,1));
	if(hRgn2)
		m_BtCancel.SetWindowRgn(hRgn2,true);

	wsprintf(path,"%slogon\\button.bmp",skinfolder);
	m_BtOk.LoadButtonBitmap(path,false);
	m_bt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	hRgn2=AFCBmpToRgn(bthelp,m_bt.GetPixel(0,0),RGB(1,1,1));
	if(hRgn2)
		m_BtOk.SetWindowRgn(hRgn2,true);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CCheckPSW::OnPaint()
{
	CPaintDC dc(this); 
 
	CGameImageHelper	help(&m_bk);
	help.BitBlt(dc.GetSafeHdc(),0,0);
}

void CCheckPSW::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	GetDlgItemText(IDC_EDIT_TWOPSW, m_szScrPass, sizeof(m_szScrPass));
	if(strlen(m_szScrPass) == 0|| strlen(m_szScrPass) > 32)
	{
		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
		
		//AFCMessageBox(fMsg.GetKeyVal("MainRoom","InputWithdrawalPW","请输入取款密码"),fMsg.GetKeyVal("MainRoom","Tip","提示"));
		DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,fMsg.GetKeyVal("MainRoom","Tip","提示"),fMsg.GetKeyVal("MainRoom","InputWithdrawalPW","请输入取款密码"));
		return;
	}	
	//wushuqun 2009.6.1
	//修改支持MD5 和SHA 加密方式
	CenterServerMsg& par=Glb().m_CenterServerPara;
	//md5方式
	if( par.m_nEncryptType == 1)
	{
		unsigned char szMDTemp[16];
		MD5_CTX Md5;
		Md5.MD5Update((unsigned char *)m_szScrPass,lstrlen(m_szScrPass));
		Md5.MD5Final(szMDTemp);
		for (int i = 0; i < 16; i ++) 
		wsprintf(&m_szMD5Pass[i * 2], "%02x", szMDTemp[i] );
	}
	//sha加密方式
	if(par.m_nEncryptType == 2)
	{
		unsigned szMDTemp[5]={0};
		CString strSha=m_szScrPass;
		SHA1 sha;
		sha.Reset();
		sha<<strSha;
		sha.Result(szMDTemp);
		
		CString strSingle;
		strSha="";
		for (int i=0;i<5;i++) 
		{
			strSingle.Format("%.8x",szMDTemp[i]);
			strSha+=strSingle;
		}
		wsprintf(m_szMD5Pass,"%s",strSha);
	}
	//////////////////////////

	CLoadFileBmp::OnOK();
}

void CCheckPSW::DoDataExchange(CDataExchange* pDX)
{
	// TODO: 在此添加专用代码和/或调用基类
	CLoadFileBmp::DoDataExchange(pDX);
	DDX_Control(pDX, IDCANCEL,m_BtCancel);	
	DDX_Control(pDX, IDOK,m_BtOk);
}
