#include "StdAfx.h"
#include "CconfigDlg.h"


CconfigDlg::CconfigDlg(void)
{
	//是否用九宫底图 0:不用，1用.
	bool b_Use = 0;

	//对话框的高
	int i_Dlg_High = 0;

	//对话框的宽
	int i_Dlg_Wide = 0;

}


CconfigDlg::~CconfigDlg(void)
{
}


// 加载配置文件。
//Path 配置文件路径
//PathDefault 默认路径
void CconfigDlg::LoadConfig(TCHAR * Path,TCHAR * PathDefault)
{ 
	wsprintf(m_Str_PathDefault,"%s",PathDefault); // 图片默认在 image\\client\\skinX\\ 目录下开始
	 
	//配置文件路径 
	wsprintf(m_Str_path_Config,"%s",Path); 

	CBcfFile forder(m_Str_path_Config);
	 
	//底图 (图片路径) 
	wsprintf(m_Str_path_9Speed,"%s%s",PathDefault,forder.GetKeyVal("ConfirmDlg", "path_bk", "dialog\dialog_bk.png")); 
	m_img_Defaultbk.SetLoadInfo(m_Str_path_9Speed,CGameImageLink::m_bAutoLock);
	
	//标题图   
	wsprintf(m_Str_path_Title,"%s%s",PathDefault,forder.GetKeyVal("ConfirmDlg", "path_title", "dialog\title.png")); // 标题图片路径
	m_img_Title.SetLoadInfo(m_Str_path_Title,CGameImageLink::m_bAutoLock); 

	//是否用九宫底图 0:不用，1用.
	 m_bUseConfig = forder.GetKeyVal("ConfirmDlg", "b_UseConfig", 0);

	//对话框的高
	 m_iDlgHigh = forder.GetKeyVal("ConfirmDlg", "i_Dlg_High", 0);

	//对话框的宽
	 m_iDlgWide = forder.GetKeyVal("ConfirmDlg", "i_Dlg_Wide", 0);
	
	////底图九宫格中，左上角
	 m_iCorner_x1  = forder.GetKeyVal("ConfirmDlg", "i_corner_x1", 0);//x 偏移
	 m_iCorner_y1 = forder.GetKeyVal("ConfirmDlg", "i_corner_y1", 0);// y 偏移
	
	//底图九宫格中，右下角 
	 m_iCorner_x2 = forder.GetKeyVal("ConfirmDlg", "i_corner_x2", 0);//x 偏移
	 m_iCorner_y2 = forder.GetKeyVal("ConfirmDlg", "i_corner_y2", 0);// y 偏移

	//标题图 的位置
	 m_iTitle_x = forder.GetKeyVal("ConfirmDlg", "i_title_x", 0);//x 偏移
	 m_iTitle_y = forder.GetKeyVal("ConfirmDlg", "i_title_y", 0);// y 偏移
	  
}


// 画底图
void CconfigDlg::DrawViewFrame(CDC * pDC, int iWidth, int iHeight)
{
	if(1 == m_bUseConfig)
	{//用九宫底图

		CGameImageHelper ImageHandle(&m_img_Defaultbk); 

		int iSrcBkHeight = ImageHandle.GetHeight();//源背景图高
		int iSrcBkWidth = ImageHandle.GetWidth();//源背景图宽

		if( iSrcBkWidth <=0  || iSrcBkHeight <=0 || NULL == pDC)
		{
			return;
		}
		 
		//	
		////对话框的高
		// m_iDlgHigh;

		////对话框的宽
		// m_iDlgWide;

		//////底图九宫格中，左上角
		//  m_iCorner_x1 ;//x 偏移
		//  m_iCorner_y1;// y 偏移
		//
		////底图九宫格中，右下角 
		//  m_iCorner_x2;//x 偏移
		//  m_iCorner_y2;// y 偏移

		//画 九宫格中，左上角。
		ImageHandle.BitBlt(pDC->GetSafeHdc(),0,0,m_iCorner_x1-0,m_iCorner_y1-0,0,0);
		//画 九宫格中，右上角。
		ImageHandle.BitBlt(pDC->GetSafeHdc(),m_iDlgWide-(iSrcBkWidth - m_iCorner_x2) ,0,iSrcBkWidth - m_iCorner_x2,m_iCorner_y1-0,m_iCorner_x2 ,0);
		//画 九宫格中，左下角。
		ImageHandle.BitBlt(pDC->GetSafeHdc(),0,m_iDlgHigh-(iSrcBkHeight-m_iCorner_y2),m_iCorner_x1,iSrcBkHeight-m_iCorner_y2,0,m_iCorner_y2);
		//画 九宫格中，右下角。
		ImageHandle.BitBlt(pDC->GetSafeHdc(),m_iDlgWide-(iSrcBkWidth - m_iCorner_x2),m_iDlgHigh-(iSrcBkHeight-m_iCorner_y2),iSrcBkWidth - m_iCorner_x2,iSrcBkHeight-m_iCorner_y2,m_iCorner_x2,m_iCorner_y2);
		//画 九宫格中，上中横拉伸
		ImageHandle.StretchBlt(pDC->GetSafeHdc(),m_iCorner_x1,0,m_iDlgWide-(m_iCorner_x1+iSrcBkWidth-m_iCorner_x2),m_iCorner_y1-0,m_iCorner_x1,0,m_iCorner_x2-m_iCorner_x1,m_iCorner_y1-0 );
		//画 九宫格中，下中横拉伸
		ImageHandle.StretchBlt(pDC->GetSafeHdc(),m_iCorner_x1,m_iDlgHigh-(iSrcBkHeight-m_iCorner_y2),m_iDlgWide-(m_iCorner_x1+iSrcBkWidth-m_iCorner_x2),iSrcBkHeight-m_iCorner_y2 ,m_iCorner_x1,m_iCorner_y2,m_iCorner_x2-m_iCorner_x1,iSrcBkHeight-m_iCorner_y2);
		//画 九宫格中，左中竖拉伸
		ImageHandle.StretchBlt(pDC->GetSafeHdc(),0,m_iCorner_y1,m_iCorner_x1-0,m_iDlgHigh-(m_iCorner_y1+(iSrcBkHeight-m_iCorner_y2)),0,m_iCorner_y1,m_iCorner_x1-0,m_iCorner_y2-m_iCorner_y1 );//画 九宫格中，左中竖拉伸
		//画 九宫格中，右中竖拉伸
		ImageHandle.StretchBlt(pDC->GetSafeHdc(),m_iDlgWide-(iSrcBkWidth - m_iCorner_x2),m_iCorner_y1,iSrcBkWidth-m_iCorner_x2,m_iDlgHigh-(m_iCorner_y1+(iSrcBkHeight-m_iCorner_y2)),m_iCorner_x2,m_iCorner_y1,iSrcBkWidth-m_iCorner_x2,m_iCorner_y2-m_iCorner_y1 );
		//画 九宫格中，中心位置拉伸
		ImageHandle.StretchBlt(pDC->GetSafeHdc(),m_iCorner_x1 ,m_iCorner_y1,m_iDlgWide-(m_iCorner_x1+iSrcBkWidth-m_iCorner_x2),m_iDlgHigh-(m_iCorner_y1+(iSrcBkHeight-m_iCorner_y2)),m_iCorner_x1,m_iCorner_y1,m_iCorner_x2-m_iCorner_x1,m_iCorner_y2-m_iCorner_y1 );
		
		
	    ImageHandle.OpenGDIHandle(&m_img_Title);//标题图
		//画标题图
	    ImageHandle.BitBlt(pDC->GetSafeHdc(),m_iTitle_x ,m_iTitle_y ,m_img_Title.GetWidth(),m_img_Title.GetHeight(),0,0);
		
	}
	else
	{//不用九宫底图

		CGameImageHelper ImageHandle(&m_img_Defaultbk);
		ImageHandle.BitBlt(pDC->GetSafeHdc(),0,0,m_iDlgWide,m_iDlgHigh,0,0);

	}
}
