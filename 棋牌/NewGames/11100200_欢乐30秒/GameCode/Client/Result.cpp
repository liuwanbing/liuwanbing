// Result.cpp : 实现文件
//

#include "stdafx.h"
#include "Result.h"


// CResult 对话框

IMPLEMENT_DYNAMIC(CResult, CDialog)

CResult::CResult(CWnd* pParent /*=NULL*/)
	: CDialog(CResult::IDD, pParent)
{
	TCHAR szName[MAX_PATH];
	wsprintf(szName,TEXT(".\\image\\result.bmp"),SKIN_FOLDER);//lu载入背景图
	m_img.SetLoadInfo(szName,CGameImageLink::m_bAutoLock);
}

CResult::~CResult()
{
}

void CResult::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CResult, CDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CResult 消息处理程序
BOOL CResult::OnInitDialog()
{
	CDialog::OnInitDialog();
#ifndef FINISH_RECORD
  	;;SetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE,GetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE)^0x80000);
 	//-------------------------窗体透明----------------------
	HINSTANCE hInst = LoadLibrary(TEXT("User32.DLL"));
	if(hInst != NULL) 
	{ 
	typedef BOOL (WINAPI *MYFUNC)(HWND,COLORREF,BYTE,DWORD); 
		MYFUNC fun = NULL;
		//取得SetLayeredWindowAttributes函数指针 
		fun=(MYFUNC)GetProcAddress(hInst, "SetLayeredWindowAttributes");
		if(fun)fun(this->GetSafeHwnd(),0,220,2); 
		FreeLibrary(hInst); 
	}
#endif
	CGameImageHelper help(&m_img);
	HRGN hRgn=AFCBmpToRgn(help,RGB(255,0,255),RGB(1,1,1));     //过滤掉RGB(255,0,255)的颜色区域
	if (hRgn!=NULL)
	{
 		SetWindowRgn(hRgn,TRUE);        //设置为没有过滤掉的区域
	}
	CenterWindow();
	return true;
}

void CResult::OnPaint()
{
	CPaintDC dc(this); // device context for painting
//	// TODO: 在此处添加消息处理程序代码
//	// 不为绘图消息调用 CDialog::OnPaint()
//	CFont font;
//	font.CreateFont(-13,0,0,0,400,NULL,NULL,NULL,134,3,2,1,2,TEXT("宋体"));
//	dc.SelectObject(&font);
//	CDC srcdc;
//	CGameImageHelper help(&m_img);
//	srcdc.CreateCompatibleDC(&dc);
//	srcdc.SelectObject(help);//lu粘贴背景图
// 	::TransparentBlt(dc.GetSafeHdc(),0,0,help.GetWidth(),help.GetHeight(),srcdc,0,0,help.GetWidth(),help.GetHeight(),srcdc.GetPixel(0,0));
//	dc.SetBkMode(TRANSPARENT);
//	CRect rectName(0,0,0,0);
//
//	if(m_iMeStation == m_iNtStation)//自己是庄家
//	{
//		dc.SetTextColor(RGB(255,0,0));//自家用红色
//		CString strname="",str="";
//		//输出名称
//		rectName.SetRect(11,54,57,76);
//		strname="自家";
//		dc.DrawText(strname,strname.GetLength(),&rectName,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
//		//输出牌点
//		rectName.SetRect(104,54,160,76);
//		str.Format("%d",m_iPaiDian[0]);
//		dc.DrawText(str,str.GetLength(),&rectName,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
//		//输出牌型
//		rectName.SetRect(56,54,104,76);
//		if(m_iZPaiXing[0]>0)
//		{
//			if(m_iZPaiXing[1]>0)
//			{
//				str="天王对子";
//			}
//			else 
//			{
//				str="天王";
//			}
//			dc.DrawText(str,str.GetLength(),&rectName,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
//		}
//		else if(m_iZPaiXing[1]>0)
//		{
//			str="对 子";
//			dc.DrawText(str,str.GetLength(),&rectName,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
//		}  
//
//		//输出金币
//		rectName.SetRect(160,54,273,76);
//		str.Format("%d",m_iMeWinMoney);
//		dc.DrawText(str,str.GetLength(),&rectName,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
//
////////////////////////////////////////////////////////////////
//		dc.SetTextColor(RGB(255,255,102));//别家用金色
//
//		rectName.SetRect(11,76,57,97);
//		strname="闲家";
//		dc.DrawText(strname,strname.GetLength(),&rectName,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
//		//输出牌点
//		rectName.SetRect(104,76,160,97);
//		str.Format("%d",m_iPaiDian[1]);
//		dc.DrawText(str,str.GetLength(),&rectName,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
//		//输出牌型
//		rectName.SetRect(56,76,104,97);
//		if(m_iXPaiXing[0]>0)
//		{
//			if(m_iXPaiXing[1]>0)
//			{
//				str="天王对子";
//			}
//			else 
//			{
//				str="天王";
//			}
//			dc.DrawText(str,str.GetLength(),&rectName,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
//		}
//		else if(m_iXPaiXing[1]>0)
//		{
//			str="对 子";
//			dc.DrawText(str,str.GetLength(),&rectName,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
//		}  
//
//		//输出金币
//		rectName.SetRect(160,76,273,97);
//		str.Format("%d",m_iXianMoney);
//		dc.DrawText(str,str.GetLength(),&rectName,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
//	}
//	else 
//	{
//		dc.SetTextColor(RGB(255,0,0));//自家用红色
//		CString strname="",str="";
//		//输出名称
//		rectName.SetRect(11,54,57,76);
//		strname="自家";
//		dc.DrawText(strname,strname.GetLength(),&rectName,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
//		//输出牌点
//		rectName.SetRect(104,54,160,76);
//		str.Format("%d",m_iPaiDian[1]);
//		dc.DrawText(str,str.GetLength(),&rectName,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
//		//输出牌型
//		rectName.SetRect(56,54,104,76);
//		if(m_iXPaiXing[0]>0)
//		{
//			if(m_iXPaiXing[1]>0)
//			{
//				str="天王对子";
//			}
//			else 
//			{
//				str="天王";
//			}
//			dc.DrawText(str,str.GetLength(),&rectName,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
//		}
//		else if(m_iXPaiXing[1]>0)
//		{
//			str="对 子";
//			dc.DrawText(str,str.GetLength(),&rectName,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
//		}  
//
//		//输出金币
//		rectName.SetRect(160,54,273,76);
//		str.Format("%d",m_iMeWinMoney);
//		dc.DrawText(str,str.GetLength(),&rectName,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
//
//
////////////////////////////////////////////////////////////////
//		dc.SetTextColor(RGB(255,255,102));//别家用金色
//
//		rectName.SetRect(11,76,57,97);
//		strname="庄家";
//		dc.DrawText(strname,strname.GetLength(),&rectName,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
//		//输出牌点
//		rectName.SetRect(104,76,160,97);
//		str.Format("%d",m_iPaiDian[0]);
//		dc.DrawText(str,str.GetLength(),&rectName,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
//		//输出牌型
//		rectName.SetRect(56,76,104,97);
//		if(m_iZPaiXing[0]>0)
//		{
//			if(m_iZPaiXing[1]>0)
//			{
//				str="天王对子";
//			}
//			else 
//			{
//				str="天王";
//			}
//			dc.DrawText(str,str.GetLength(),&rectName,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
//		}
//		else if(m_iZPaiXing[1]>0)
//		{
//			str="对 子";
//			dc.DrawText(str,str.GetLength(),&rectName,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
//		}  
//
//		//输出金币
//		rectName.SetRect(160,76,273,97);
//		str.Format("%d",m_iNtWinMoney);
//		dc.DrawText(str,str.GetLength(),&rectName,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
//	}

}
//数据清零
void CResult::ClearAll()
{
	m_iNtStation = -1;//庄家位置
	m_iMeStation = -1;//自己的位置
	m_iNtWinMoney = 0;//庄赢的钱
	m_iXianMoney = 0;//闲赢的钱
	m_iMeWinMoney = 0;//自己赢的钱
	m_iPaiDian[0] = 0;  //庄的牌点
	m_iPaiDian[1] = 0;  //闲的牌点

	m_iZPaiXing[0] = 0; //庄家的牌型
	m_iZPaiXing[1] = 0; //庄家的牌型

	m_iXPaiXing[0] = 0; //闲家的牌型
	m_iXPaiXing[1] = 0; //闲家的牌型
}