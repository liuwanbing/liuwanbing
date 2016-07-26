#pragma once

#include "Stdafx.h"
//#include "afcdevhead.h" 
#include "AFCImage.h"
#include "AFCResource.h"

/**
作者：yyf
创建日期：2012.10.15
功能描述：用于mfc做的对话框画底图。只要读入配套的设置文件，
          可以把一张底图用9宫格的方式拉伸到想要的大小，
		  而且对话框的大小在设置文件里也可以配置。

----------设置文件 configDlg.bcf内容-----------

[ConfirmDlg]
;是否用九宫底图 0:不用，1用.
b_UseConfig=1
;源底图九宫格中，中心方格 左上角 x 偏移
i_corner_x1=10
;源底图九宫格中，中心方格 右下角 x 偏移
i_corner_x2=254
;源底图九宫格中，中心方格 左上角 y 偏移
i_corner_y1=40
;源底图九宫格中，中心方格 右下角 y 偏移
i_corner_y2=140
;对话框的高
;410
i_Dlg_High=194
;对话框的宽 
;250
i_Dlg_Wide=282
;标题图片x坐标
i_title_x=10
;标题图片y坐标
i_title_y=5
;底图 (图片路径)
path_bk=dialog\dialog_bk.png
;标题图片
path_title=dialog\addfrend_title.png

--------------------------------------

使用方法：
         
(1) 引用 #include "..\UILibrary2003\CconfigDlg.h" 
(2) 在对话框类中 声明 
	  CconfigDlg  m_configDlg;
	  
(3) 在对话框中初始化 m_configDlg
    例如：
	 
	// -------初始化 m_configDlg---------  
	TCHAR pathConfig[MAX_PATH];
	TCHAR PathDefault[MAX_PATH];
	
	wsprintf(pathConfig,"%s%sdialog\\configDlg\\XXXXXXXXXX\\configDlg.bcf",s,skinfolder);// 设置文件的路径
	wsprintf(PathDefault,"%s%s",s,skinfolder);//图片的路径dialog目录的上级目录
	m_configDlg.LoadConfig(pathConfig,PathDefault);	

	int cx,cy; 
	cx=m_configDlg.GetDlgWide();
	cy=m_configDlg.GetDlgHigh(); 
	SetWindowPos(NULL,0,0,cx,cy,SWP_NOMOVE|SWP_NOZORDER); 
	//----------------------------------------------------- 

(4) 在 OnPaint() 函数中调用绘画底图.
void XXXX::OnPaint()
{
	CPaintDC dc(this); // device context for painting  

	//绘画框架底图  
	m_configDlg.DrawViewFrame(&dc);
}

(5) 把 对话框中原来用的 例如：CGameImage	m_bk; 这个底图，
    在函数方法中出现 m_bk.GetPixel(xxx,yyy) 的地方，用m_configDlg.GetBKImageEX().GetPixel()代替掉。
*/
class EXT_CLASS CconfigDlg
{
public:
	//是否用九宫底图 0:不用，1用.
	bool m_bUseConfig;

	//对话框的高
	int m_iDlgHigh;

	//对话框的宽
	int m_iDlgWide;

	// 图片默认在 image\\client\\skinX\\ 目录下开始 
	//配置文件路径
	TCHAR m_Str_path_Config[MAX_PATH];
	TCHAR m_Str_PathDefault[MAX_PATH];// 图片默认在 image\\client\\skinX\\ 目录下开始 
	TCHAR m_Str_path_Title[MAX_PATH];//标题图片路径

	//;不用九宫底图时，调用这个底图  
	TCHAR m_Str_path_9Speed[MAX_PATH];
	CGameImageEx m_img_Defaultbk;//默认底图 CGameImage
	CGameImageEx m_img_Title;//标题图  
	//CGameImage m_img_Defaultbk;//默认底图 CGameImage
	 
	////底图九宫格中，左上角
	int m_iCorner_x1 ;//x 偏移
	int m_iCorner_y1;// y 偏移
	
	//底图九宫格中，右下角 
	int m_iCorner_x2;//x 偏移
	int m_iCorner_y2;// y 偏移

	//标题图 的位置
	int m_iTitle_x;
	int m_iTitle_y;

public:
	CconfigDlg(void);
	virtual ~CconfigDlg(void);
	// 加载配置文件。
	void LoadConfig(TCHAR * Path,TCHAR * PathDefault);
	// 画底图
	void DrawViewFrame(CDC * pDC, int iWidth=0, int iHeight=0);
	// 返回对话框的宽
	inline int GetDlgWide(void)
	{ 
		return m_iDlgWide; 
	};
	// 返回对话框的高
	inline int GetDlgHigh(void)
	{ 
		return m_iDlgHigh; 
	};
	////返回背景 image
	inline CGameImageEx& GetBKImageEX(void)
	{ 
		return m_img_Defaultbk;
	};
	//返回背景 image
	inline CGameImage& GetBKImage(void)
	{ 
		return m_img_Defaultbk;
	};
};