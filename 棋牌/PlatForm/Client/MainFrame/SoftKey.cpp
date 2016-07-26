// SoftKey.cpp : 实现文件
//

#include "stdafx.h"
#include "SoftKey.h"


// CSoftKey 对话框

IMPLEMENT_DYNAMIC(CSoftKey, CDialog)

CSoftKey::CSoftKey(CWnd* pParent /*=NULL*/)
	: CDialog(CSoftKey::IDD, pParent)
{
	ZeroMemory(m_char,sizeof(m_char));
	ZeroMemory(m_letter,sizeof(m_letter));
	m_bCaplock=false;
	m_bShift=false;
	m_bkBrush=NULL;

}

CSoftKey::~CSoftKey()
{
	DeleteObject(m_bkBrush);
}

void CSoftKey::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_Shift, m_shift);
	DDX_Control(pDX, IDC_CAPLOCK, m_caplock);
	DDX_Control(pDX, IDC_BUTTON1, m_bnC1);
	DDX_Control(pDX, IDC_BUTTON2, m_bnC2);
	DDX_Control(pDX, IDC_BUTTON3, m_bnC3);
	DDX_Control(pDX, IDC_BUTTON4, m_bnC4);
	DDX_Control(pDX, IDC_BUTTON5, m_bnC5);
	DDX_Control(pDX, IDC_BUTTON6, m_bnC6);
	DDX_Control(pDX, IDC_BUTTON7, m_bnC7);
	DDX_Control(pDX, IDC_BUTTON8, m_bnC8);
	DDX_Control(pDX, IDC_BUTTON9, m_bnC9);
	DDX_Control(pDX, IDC_BUTTON10, m_bnC10);
	DDX_Control(pDX, IDC_BUTTON11, m_bnC11);
	DDX_Control(pDX, IDC_BUTTON12, m_bnC12);
	DDX_Control(pDX, IDC_BUTTON13, m_bnC13);
	DDX_Control(pDX, IDC_BUTTON14, m_bnC14);
	DDX_Control(pDX, IDC_BUTTON15, m_bnC15);
	DDX_Control(pDX, IDC_BUTTON16, m_bnC16);
	DDX_Control(pDX, IDC_BUTTON17, m_bnC17);
	DDX_Control(pDX, IDC_BUTTON18, m_bnC18);
	DDX_Control(pDX, IDC_BUTTON19, m_bnC19);
	DDX_Control(pDX, IDC_BUTTON20, m_bnC20);
	DDX_Control(pDX, IDC_BUTTON21, m_bnC21);

	DDX_Control(pDX, IDC_BUTTON23, m_bnL1);
	DDX_Control(pDX, IDC_BUTTON24, m_bnL2);
	DDX_Control(pDX, IDC_BUTTON25, m_bnL3);
	DDX_Control(pDX, IDC_BUTTON26, m_bnL4);
	DDX_Control(pDX, IDC_BUTTON27, m_bnL5);
	DDX_Control(pDX, IDC_BUTTON28, m_bnL6);
	DDX_Control(pDX, IDC_BUTTON29, m_bnL7);
	DDX_Control(pDX, IDC_BUTTON30, m_bnL8);
	DDX_Control(pDX, IDC_BUTTON31, m_bnL9);
	DDX_Control(pDX, IDC_BUTTON32, m_bnL10);
	DDX_Control(pDX, IDC_BUTTON33, m_bnL11);
	DDX_Control(pDX, IDC_BUTTON34, m_bnL12);
	DDX_Control(pDX, IDC_BUTTON35, m_bnL13);
	DDX_Control(pDX, IDC_BUTTON36, m_bnL14);
	DDX_Control(pDX, IDC_BUTTON37, m_bnL15);
	DDX_Control(pDX, IDC_BUTTON38, m_bnL16);
	DDX_Control(pDX, IDC_BUTTON39, m_bnL17);
	DDX_Control(pDX, IDC_BUTTON40, m_bnL18);
	DDX_Control(pDX, IDC_BUTTON41, m_bnL19);
	DDX_Control(pDX, IDC_BUTTON42, m_bnL20);
	DDX_Control(pDX, IDC_BUTTON43, m_bnL21);
	DDX_Control(pDX, IDC_BUTTON44, m_bnL22);
	DDX_Control(pDX, IDC_BUTTON45, m_bnL23);
	DDX_Control(pDX, IDC_BUTTON46, m_bnL24);
	DDX_Control(pDX, IDC_BUTTON47, m_bnL25);
	DDX_Control(pDX, IDC_BUTTON48, m_bnL26);

}


BEGIN_MESSAGE_MAP(CSoftKey, CDialog)
	ON_BN_CLICKED(IDC_Shift, &CSoftKey::OnBnClickedShift)
	ON_BN_CLICKED(IDC_CAPLOCK, &CSoftKey::OnBnClickedCaplock)
	ON_BN_CLICKED(IDC_BUTTON1, &CSoftKey::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CSoftKey::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CSoftKey::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CSoftKey::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CSoftKey::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CSoftKey::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CSoftKey::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CSoftKey::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON9, &CSoftKey::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON10, &CSoftKey::OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON11, &CSoftKey::OnBnClickedButton11)
	ON_BN_CLICKED(IDC_BUTTON12, &CSoftKey::OnBnClickedButton12)
	ON_BN_CLICKED(IDC_BUTTON13, &CSoftKey::OnBnClickedButton13)
	ON_BN_CLICKED(IDC_BUTTON14, &CSoftKey::OnBnClickedButton14)
	ON_BN_CLICKED(IDC_BUTTON15, &CSoftKey::OnBnClickedButton15)
	ON_BN_CLICKED(IDC_BUTTON16, &CSoftKey::OnBnClickedButton16)
	ON_BN_CLICKED(IDC_BUTTON20, &CSoftKey::OnBnClickedButton20)
	ON_BN_CLICKED(IDC_BUTTON18, &CSoftKey::OnBnClickedButton18)
	ON_BN_CLICKED(IDC_BUTTON19, &CSoftKey::OnBnClickedButton19)
	ON_BN_CLICKED(IDC_BUTTON17, &CSoftKey::OnBnClickedButton17)
	ON_BN_CLICKED(IDC_BUTTON21, &CSoftKey::OnBnClickedButton21)
	ON_BN_CLICKED(IDC_BUTTON23, &CSoftKey::OnBnClickedButton23)
	ON_BN_CLICKED(IDC_BUTTON24, &CSoftKey::OnBnClickedButton24)
	ON_BN_CLICKED(IDC_BUTTON25, &CSoftKey::OnBnClickedButton25)
	ON_BN_CLICKED(IDC_BUTTON26, &CSoftKey::OnBnClickedButton26)
	ON_BN_CLICKED(IDC_BUTTON27, &CSoftKey::OnBnClickedButton27)
	ON_BN_CLICKED(IDC_BUTTON28, &CSoftKey::OnBnClickedButton28)
	ON_BN_CLICKED(IDC_BUTTON29, &CSoftKey::OnBnClickedButton29)
	ON_BN_CLICKED(IDC_BUTTON30, &CSoftKey::OnBnClickedButton30)
	ON_BN_CLICKED(IDC_BUTTON31, &CSoftKey::OnBnClickedButton31)
	ON_BN_CLICKED(IDC_BUTTON32, &CSoftKey::OnBnClickedButton32)
	ON_BN_CLICKED(IDC_BUTTON33, &CSoftKey::OnBnClickedButton33)
	ON_BN_CLICKED(IDC_BUTTON34, &CSoftKey::OnBnClickedButton34)
	ON_BN_CLICKED(IDC_BUTTON35, &CSoftKey::OnBnClickedButton35)
	ON_BN_CLICKED(IDC_BUTTON36, &CSoftKey::OnBnClickedButton36)
	ON_BN_CLICKED(IDC_BUTTON37, &CSoftKey::OnBnClickedButton37)
	ON_BN_CLICKED(IDC_BUTTON38, &CSoftKey::OnBnClickedButton38)
	ON_BN_CLICKED(IDC_BUTTON39, &CSoftKey::OnBnClickedButton39)
	ON_BN_CLICKED(IDC_BUTTON40, &CSoftKey::OnBnClickedButton40)
	ON_BN_CLICKED(IDC_BUTTON41, &CSoftKey::OnBnClickedButton41)
	ON_BN_CLICKED(IDC_BUTTON42, &CSoftKey::OnBnClickedButton42)
	ON_BN_CLICKED(IDC_BUTTON43, &CSoftKey::OnBnClickedButton43)
	ON_BN_CLICKED(IDC_BUTTON44, &CSoftKey::OnBnClickedButton44)
	ON_BN_CLICKED(IDC_BUTTON45, &CSoftKey::OnBnClickedButton45)
	ON_BN_CLICKED(IDC_BUTTON46, &CSoftKey::OnBnClickedButton46)
	ON_BN_CLICKED(IDC_BUTTON47, &CSoftKey::OnBnClickedButton47)
	ON_BN_CLICKED(IDC_BUTTON48, &CSoftKey::OnBnClickedButton48)
END_MESSAGE_MAP()


// CSoftKey 消息处理程序

BOOL CSoftKey::OnInitDialog()
{
	CDialog::OnInitDialog();

	//初始化各个按键的索引
	m_char[0]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON1);
	m_char[1]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON2);
	m_char[2]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON3);
	m_char[3]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON4);
	m_char[4]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON5);
	m_char[5]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON6);
	m_char[6]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON7);
	m_char[7]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON8);
	m_char[8]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON9);
	m_char[9]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON10);
	m_char[10]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON11);
	m_char[11]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON12);
	m_char[12]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON13);
	m_char[13]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON14);
	m_char[14]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON15);
	m_char[15]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON16);
	m_char[16]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON17);
	m_char[17]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON18);
	m_char[18]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON19);
	m_char[19]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON20);
	m_char[20]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON21);

	m_letter[0]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON23);
	m_letter[1]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON24);
	m_letter[2]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON25);
	m_letter[3]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON26);
	m_letter[4]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON27);
	m_letter[5]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON28);
	m_letter[6]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON29);
	m_letter[7]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON30);
	m_letter[8]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON31);
	m_letter[9]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON32);
	m_letter[10]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON33);
	m_letter[11]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON34);
	m_letter[12]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON35);
	m_letter[13]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON36);
	m_letter[14]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON37);
	m_letter[15]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON38);
	m_letter[16]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON39);
	m_letter[17]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON40);
	m_letter[18]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON41);
	m_letter[19]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON42);
	m_letter[20]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON43);
	m_letter[21]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON44);
	m_letter[22]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON45);
	m_letter[23]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON46);
	m_letter[24]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON47);
	m_letter[25]=(CNormalBitmapButtonEx*)GetDlgItem(IDC_BUTTON48);
	
	//初始化各个符号，[][0]为一般符号
	m_sChar[0][0]="`";	m_sChar[1][0]="1";	m_sChar[2][0]="2";	m_sChar[3][0]="3";	m_sChar[4][0]="4";
	m_sChar[5][0]="5";	m_sChar[6][0]="6";	m_sChar[7][0]="7";	m_sChar[8][0]="8";	m_sChar[9][0]="9";
	m_sChar[10][0]="0";	m_sChar[11][0]="-";	m_sChar[12][0]="=";	m_sChar[13][0]="[";	m_sChar[14][0]="]";
	m_sChar[15][0]=";";	m_sChar[16][0]="'";	m_sChar[17][0]="\\";	m_sChar[18][0]=",";	m_sChar[19][0]=".";
	m_sChar[20][0]="/";
	//[][1]为上档符号
	m_sChar[0][1]="~";	m_sChar[1][1]="!";	m_sChar[2][1]="@";	m_sChar[3][1]="#";	m_sChar[4][1]="$";
	m_sChar[5][1]="%";	m_sChar[6][1]="^";	m_sChar[7][1]="&&";	m_sChar[8][1]="*";	m_sChar[9][1]="(";
	m_sChar[10][1]=")";	m_sChar[11][1]="_";	m_sChar[12][1]="+";	m_sChar[13][1]="{";	m_sChar[14][1]="}";
	m_sChar[15][1]=":";	m_sChar[16][1]="\"";	m_sChar[17][1]="|";	m_sChar[18][1]="<";	m_sChar[19][1]=">";
	m_sChar[20][1]="?";

	//初始化字母
	CString s;
	for(int i=0;i<26;i++)
	{
		m_sLeffer[i][0].Format("%c",i+97);//小写字母
		m_sLeffer[i][1].Format("%c",i+65);//大写字母
	}	


	//实始化按键排序索引
	for(int i=0;i<21;i++)
		m_charIndex[i]=i;
	for(int i=0;i<26;i++)
		m_letterIndex[i]=i;

	//随机调整数字符号索引位置
	srand((unsigned int )time(0));
	for(int i=0;i<21;i++)
	{
		int r=rand()%(21-i);
		int t=m_charIndex[i];
		m_charIndex[i]=m_charIndex[r];
		m_charIndex[r]=t;
	} 
	//随机调整字母索引位置
	srand((unsigned int )time(0));
	for(int i=0;i<26;i++)
	{
		int r=rand()%(26-i);
		int t=m_letterIndex[i];
		m_letterIndex[i]=m_letterIndex[r];
		m_letterIndex[r]=t;
	} 

	//设置各个按钮显示
	for(int i=0;i<21;i++)
		m_char[i]->SetWindowText(m_sChar[m_charIndex[i]][0]);
	for(int i=0;i<26;i++)
		m_letter[i]->SetWindowText(m_sLeffer[m_letterIndex[i]][0]);

	s=CBcfFile::GetAppPath ();/////本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + strSkin);
	CString key=TEXT("SoftKey");
	TCHAR path[MAX_PATH];
	CString skinfolder;
	if ("skin0.bcf" == strSkin)
		skinfolder=f.GetKeyVal("skin0","skinfolder",m_skinmgr.GetSkinPath());
	else
		skinfolder=f.GetKeyVal("skin1","skinfolder",m_skinmgr.GetSkinPath());
	int r=f.GetKeyVal(key,"softkeybgr",216);	
	int g=f.GetKeyVal(key,"softkeybgg",234);	
	int b=f.GetKeyVal(key,"softkeybgb",255);	
	m_bkcolor=RGB(r,g,b);

	LoadSkin();

	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	CString strMessage = fMsg.GetKeyVal("LogonDlg","SoftKey","软键盘输入");
	SetWindowText(strMessage);

	m_bkBrush=CreateSolidBrush(m_bkcolor);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

/*
Function : OnBnClickedShift()
Parameter: none
Return	 : none
Author	 : Fred Huang 2008-05-15
Memo	 : 按下了Shift按钮，按下后显示 SHIFT，再按一次为Shift，模拟按住了上档键
*/
void CSoftKey::OnBnClickedShift()
{
	m_bShift=!m_bShift;
	m_shift.SetWindowText(m_bShift?"SHIFT":"Shift");
	//如果按下了shift，则显示符号，否则显示数字
	for(int i=0;i<21;i++)
		m_char[i]->SetWindowText(m_sChar[m_charIndex[i]][m_bShift?1:0]);
	//字母的显示还要与CAPS LOCK相关连判断
	//都没有按下，或都按下：小写字母
	//按下了其中一个：大写字母
	for(int i=0;i<26;i++)
		m_letter[i]->SetWindowText(m_sLeffer[m_letterIndex[i]][((m_bShift && !m_bCaplock) || (!m_bShift && m_bCaplock))?1:0]);
}
/*
Function : OnBnClickedCaplock()
Parameter: none
Return	 : none
Author	 : Fred Huang 2008-05-15
Memo	 : 按下了Caps lock按钮
*/
void CSoftKey::OnBnClickedCaplock()
{
	m_bCaplock=!m_bCaplock;
	m_caplock.SetWindowText(m_bCaplock?"CAPS LOCK":"Caps lock");
	//设置字母的显示，字母的显示还要与SHIFT相关连判断
	//都没有按下，或都按下：小写字母
	//按下了其中一个：大写字母
	for(int i=0;i<26;i++)
		m_letter[i]->SetWindowText(m_sLeffer[m_letterIndex[i]][((m_bShift && !m_bCaplock) || (!m_bShift && m_bCaplock))?1:0]);
}

//以下操作为按下了各个按钮
void CSoftKey::OnBnClickedButton1(){	inputChar(0);}
void CSoftKey::OnBnClickedButton2(){	inputChar(1);}
void CSoftKey::OnBnClickedButton3(){	inputChar(2);}
void CSoftKey::OnBnClickedButton4(){	inputChar(3);}
void CSoftKey::OnBnClickedButton5(){	inputChar(4);}
void CSoftKey::OnBnClickedButton6(){	inputChar(5);}
void CSoftKey::OnBnClickedButton7(){	inputChar(6);}
void CSoftKey::OnBnClickedButton8(){	inputChar(7);}
void CSoftKey::OnBnClickedButton9(){	inputChar(8);}
void CSoftKey::OnBnClickedButton10(){	inputChar(9);}
void CSoftKey::OnBnClickedButton11(){	inputChar(10);}
void CSoftKey::OnBnClickedButton12(){	inputChar(11);}
void CSoftKey::OnBnClickedButton13(){	inputChar(12);}
void CSoftKey::OnBnClickedButton14(){	inputChar(13);}
void CSoftKey::OnBnClickedButton15(){	inputChar(14);}
void CSoftKey::OnBnClickedButton16(){	inputChar(15);}
void CSoftKey::OnBnClickedButton17(){	inputChar(16);}
void CSoftKey::OnBnClickedButton18(){	inputChar(17);}
void CSoftKey::OnBnClickedButton19(){	inputChar(18);}
void CSoftKey::OnBnClickedButton20(){	inputChar(19);}
void CSoftKey::OnBnClickedButton21(){	inputChar(20);}

void CSoftKey::OnBnClickedButton23(){	inputLetter(0);}
void CSoftKey::OnBnClickedButton24(){	inputLetter(1);}
void CSoftKey::OnBnClickedButton25(){	inputLetter(2);}
void CSoftKey::OnBnClickedButton26(){	inputLetter(3);}
void CSoftKey::OnBnClickedButton27(){	inputLetter(4);}
void CSoftKey::OnBnClickedButton28(){	inputLetter(5);}
void CSoftKey::OnBnClickedButton29(){	inputLetter(6);}
void CSoftKey::OnBnClickedButton30(){	inputLetter(7);}
void CSoftKey::OnBnClickedButton31(){	inputLetter(8);}
void CSoftKey::OnBnClickedButton32(){	inputLetter(9);}
void CSoftKey::OnBnClickedButton33(){	inputLetter(10);}
void CSoftKey::OnBnClickedButton34(){	inputLetter(11);}
void CSoftKey::OnBnClickedButton35(){	inputLetter(12);}
void CSoftKey::OnBnClickedButton36(){	inputLetter(13);}
void CSoftKey::OnBnClickedButton37(){	inputLetter(14);}
void CSoftKey::OnBnClickedButton38(){	inputLetter(15);}
void CSoftKey::OnBnClickedButton39(){	inputLetter(16);}
void CSoftKey::OnBnClickedButton40(){	inputLetter(17);}
void CSoftKey::OnBnClickedButton41(){	inputLetter(18);}
void CSoftKey::OnBnClickedButton42(){	inputLetter(19);}
void CSoftKey::OnBnClickedButton43(){	inputLetter(20);}
void CSoftKey::OnBnClickedButton44(){	inputLetter(21);}
void CSoftKey::OnBnClickedButton45(){	inputLetter(22);}
void CSoftKey::OnBnClickedButton46(){	inputLetter(23);}
void CSoftKey::OnBnClickedButton47(){	inputLetter(24);}
void CSoftKey::OnBnClickedButton48(){	inputLetter(25);}

/*
Function : inputChar()
Parameter: 
	index	:按钮索引
Return	 : none
Author	 : Fred Huang 2008-05-15
Memo	 : 按下了符号键，要从索引中取得相对应的符号，并发消息给登录窗口，这里使用了WM_USER+100作为消息，WPARAM为字符的ASCII码
*/
void CSoftKey::inputChar(int index)
{
	CString s=m_sChar[m_charIndex[index]][m_bShift?1:0];
	int c=s[0];
	pWnd->PostMessage(WM_USER+100,c,0);
}
/*
Function : inputLetter()
Parameter: 
index	:按钮索引
Return	 : none
Author	 : Fred Huang 2008-05-15
Memo	 : 按下了字母键，要从索引中取得相对应的字母，并发消息给登录窗口，这里使用了WM_USER+100作为消息，WPARAM为字母的ASCII码，区分大小写
*/
void CSoftKey::inputLetter(int index)
{
	CString s=m_sLeffer[m_letterIndex[index]][((m_bShift && !m_bCaplock) || (!m_bShift && m_bCaplock))?1:0];
	int c=s[0];
	pWnd->PostMessage(WM_USER+100,c,0);
}

void CSoftKey::LoadSkin(void)
{
	CString s=CBcfFile::GetAppPath();/////本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + strSkin);
	CString key=TEXT("SoftKey");
	TCHAR path[MAX_PATH];
	CString skinfolder;
	if ("skin0.bcf" == strSkin)
		skinfolder=f.GetKeyVal("skin0","skinfolder",m_skinmgr.GetSkinPath());
	else
		skinfolder=f.GetKeyVal("skin1","skinfolder",m_skinmgr.GetSkinPath());

	ZeroMemory(path,sizeof(path));
	wsprintf(path,"%slogon\\shift_bt.bmp",skinfolder);
	m_shift.LoadButtonBitmap(path,false);
	ZeroMemory(path,sizeof(path));
	wsprintf(path,"%slogon\\caps_bt.bmp",skinfolder);
	m_caplock.LoadButtonBitmap(path,false);
	ZeroMemory(path,sizeof(path));
	wsprintf(path,"%slogon\\char_bt.bmp",skinfolder);

	m_bnC1.LoadButtonBitmap(path,false);
	m_bnC2.LoadButtonBitmap(path,false);
	m_bnC3.LoadButtonBitmap(path,false);
	m_bnC4.LoadButtonBitmap(path,false);
	m_bnC5.LoadButtonBitmap(path,false);
	m_bnC6.LoadButtonBitmap(path,false);
	m_bnC7.LoadButtonBitmap(path,false);
	m_bnC8.LoadButtonBitmap(path,false);
	m_bnC9.LoadButtonBitmap(path,false);
	m_bnC10.LoadButtonBitmap(path,false);
	m_bnC11.LoadButtonBitmap(path,false);
	m_bnC12.LoadButtonBitmap(path,false);
	m_bnC13.LoadButtonBitmap(path,false);
	m_bnC14.LoadButtonBitmap(path,false);
	m_bnC15.LoadButtonBitmap(path,false);
	m_bnC16.LoadButtonBitmap(path,false);
	m_bnC17.LoadButtonBitmap(path,false);
	m_bnC18.LoadButtonBitmap(path,false);
	m_bnC19.LoadButtonBitmap(path,false);
	m_bnC20.LoadButtonBitmap(path,false);
	m_bnC21.LoadButtonBitmap(path,false);

	m_bnL1.LoadButtonBitmap(path,false);
	m_bnL2.LoadButtonBitmap(path,false);
	m_bnL3.LoadButtonBitmap(path,false);
	m_bnL4.LoadButtonBitmap(path,false);
	m_bnL5.LoadButtonBitmap(path,false);
	m_bnL6.LoadButtonBitmap(path,false);
	m_bnL7.LoadButtonBitmap(path,false);
	m_bnL8.LoadButtonBitmap(path,false);
	m_bnL9.LoadButtonBitmap(path,false);
	m_bnL10.LoadButtonBitmap(path,false);
	m_bnL11.LoadButtonBitmap(path,false);
	m_bnL12.LoadButtonBitmap(path,false);
	m_bnL13.LoadButtonBitmap(path,false);
	m_bnL14.LoadButtonBitmap(path,false);
	m_bnL15.LoadButtonBitmap(path,false);
	m_bnL16.LoadButtonBitmap(path,false);
	m_bnL17.LoadButtonBitmap(path,false);
	m_bnL18.LoadButtonBitmap(path,false);
	m_bnL19.LoadButtonBitmap(path,false);
	m_bnL20.LoadButtonBitmap(path,false);
	m_bnL21.LoadButtonBitmap(path,false);
	m_bnL22.LoadButtonBitmap(path,false);
	m_bnL23.LoadButtonBitmap(path,false);
	m_bnL24.LoadButtonBitmap(path,false);
	m_bnL25.LoadButtonBitmap(path,false);
	m_bnL26.LoadButtonBitmap(path,false);
	
	return;

	//for(int i=0;i<21;i++)
	//	m_char[i]->LoadButtonBitmap(path,false);
	//for(int i=0;i<26;i++)
	//	m_letter[i]->LoadButtonBitmap(path,false);

}

LRESULT CSoftKey::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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
		return (LRESULT)m_bkBrush;
	}

	return CDialog::WindowProc(message, wParam, lParam);
}
