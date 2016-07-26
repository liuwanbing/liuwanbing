/*
  文件名:bz2wui.h
  功能：BZ2wui SDK 接口声明
  作者：Fred Huang
  日期：2009.2.2

  修改历史 ：
      1.修改人：wushuqun
        日期：  2009.2.3
        修改处：1>为所有接口函数增加标准注释;
                2>接口函数 wuiPaintControl() 增加CDC * pDC 参数，这样来确保控件的正确绘图，尤其在对话框
                  背景图绘制时，若只传HWND参数，会导致对话框绘图错误，需传入CPaintDC指针。
      2.wushuqun 2009.2.5
        修改处：1>为函数 wuiPaintControl() 新加居中绘图功能
      3.wuhsuqun 2009.2.6
         1>新加wuiInit 和wuiUnInit接口，以为独立的程序使用该库
      4. wushuqun 2009.02.25
		   重写该库实现方式，原来采用CList 性能太慢，采用 map
     5. wushuqun 2009.2.27
	   取消外部 wuiSetButtonStatus 接口，修改  wuiPaintControl 接口

		 函数名   : wuiPaintControl
		 说明     : 绘制控件
					所有控件的绘图操作，只要指定控件在buc文件中的ID号，它会自动识别控件类型
		 返回类型 : void
		 参数     :  hWnd,窗口控件句柄
				  :  pDC，窗口的CDC指针
				  :  hHandle,已打开的buc文件句柄
				  :  uID,该控件在buc文件中的ID号
				  :  exParam,附加参数
						按钮控件：表示按钮当前状态,只有是按钮控件时才起作用
						窗口控件：若为九宫格绘图方式
							0,表示完全按照九宫格绘图方式，如对话框
							1,表示中间宫格的平铺部分不绘制，如游戏框架（外边框）
							2,不以九宫格绘图，整图平铺窗口
				  :  bForceVisible,是否强制显示,默认为false,若为true则强制使控件重绘

		 extern BZ2WUI_API void wuiPaintControl(HWND hWnd,CDC * pDC,DWORD hHandle,UINT uID,BYTE exParam = 0,bool bForceVisible=false,bool bCenterDraw=false);
	 
	 6.wushuqun 2009.3.4
	   新加wuiDrawImageById绘图接口
		 
		 函数名   : wuiDrawImageById
		 作者     : wushuqun 
		 日期     : 2009.3.4
		 说明     : 直接绘图，绘图大小为uID代表图的大小,可透明绘图
		 返回类型 : void 
		 参数     : hHandle,已打开的buc文件句柄
				  : uID,该控件在buc文件中的ID号
				  : desX,目标X起始坐标
				  : desY,目标Y起始坐标
				  : pDC,目的DC
				  : transColor,指定透明色，默认为RGB(255,0,255);
		extern BZ2WUI_API void wuiDrawImageById(DWORD hHandle,UINT uID,CDC * pDC,int desX,int desY,COLORREF transColor = 0xFF00FF);

		 函数名   : wuiDrawImageById
		 作者     : wushuqun 
		 日期     : 2009.3.4
		 说明     : 在pDC上绘图，可以缩放，设定透明色
		 返回类型 : void 
		 参数     : hHandle,已打开的buc文件句柄
				  : uID,该控件在buc文件中的ID号
				  : desX,目标X起始坐标
				  : desY,目标Y起始坐标
				  : desWidth,预绘制宽度
				  : desHeight,预绘制高度
				  : pDC,目的DC
				  : transColor,指定透明色，默认为RGB(255,0,255);
		extern BZ2WUI_API void wuiDrawImageById(DWORD hHandle,UINT uID,CDC * pDC,int desX,int desY,int desWidth,int desHeight,COLORREF transColor = 0xFF00FF);

*/
#pragma once

#ifdef BZ2WUI_EXPORTS
#define BZ2WUI_API __declspec(dllexport)
#else
#define BZ2WUI_API __declspec(dllimport)
#endif

#define WUI_ERR_SUCCEED		0x00
#define WUI_ERR_NOTFOUNDWUI	0x01
#define WUI_ERR_ADDNULLCONTROL	0x02
#define WUI_ERR_NOCONTROLDEFINE	0x03
#define WUI_ERR_CONTROLEXIST	0x04
#define WUI_ERR_CONTROLNOMATCH	0x05

////////////////////////////////////////
// BZ2wuiSDK  外部接口

/*
 函数名   : wuiInit
 说明     : 初始化GDI+环境,在泊众游戏中该步骤已作处理，无需再次调用，
            若为单独程序，则需要在应用程序的InitInstance函数中调用些函数,否则无法完成BZ2wui SDK
			指定的功能
 返回类型 : bool
             = true 时，初始化成功
			 = false 时，初始化失败
 */
extern BZ2WUI_API bool wuiInit();

/*
 函数名   : wuiUnInit
 说明     : 关闭GDI+绘图环境
            泊众游戏中已处理该步骤，无需再次调用
			若为单独程序，则需在应用程序进程退出时调用此函数
 返回类型 : void 
 */
extern BZ2WUI_API void wuiUnInit();

/*
 函数名   : wuiOpen
 说明     : 打开一个buc文件
 返回类型 : DWORD 
            != 0 时，为打开文件成功时的文件句柄
			 = 0 时，打开文件失败
 参数     : 
           szFilename,buc文件名
 */
extern BZ2WUI_API DWORD wuiOpen(LPCTSTR szFilename);

/*
 函数名   : wuiOpen
 说明     : 关闭一个文件句柄为hHandle所指的buc文件
 返回类型 : void 
 参数     : 
           hHandle,buc文件句柄
 */
extern BZ2WUI_API void wuiClose(DWORD hHandle);

/*
 函数名   : wuiSetControlRgn
 说明     : 设置控件窗口区域,自动屏蔽掉颜色值为0xFF00FF 的颜色 
 返回类型 : void 
 参数     : hWnd,控件窗口句柄
          : hHandle,已打开的buc文件句柄
          : uID,该控件在buc文件中指定的ID号
 */
extern BZ2WUI_API void wuiSetControlRgn(HWND hWnd,DWORD hHandle,UINT uID);


/*
 函数名   : wuiPaintControl
 说明     : 绘制控件
            所有控件的绘图操作，只要指定控件在buc文件中的ID号，它会自动识别控件类型
 返回类型 : void
 参数     :  hWnd,窗口控件句柄
          :  pDC，窗口的CDC指针
          :  hHandle,已打开的buc文件句柄
	  :  uID,该控件在buc文件中的ID号
	  :  exParam,附加参数
		按钮控件：表示按钮当前状态,只有是按钮控件时才起作用
		窗口控件：若为九宫格绘图方式
			0,表示完全按照九宫格绘图方式，如对话框
			1,表示中间宫格的平铺部分不绘制，如游戏框架（外边框）
			2,不以九宫格绘图，整图平铺窗口
          :  bForceVisible,是否强制显示,默认为false,若为true则强制使控件重绘
*/
extern BZ2WUI_API void wuiPaintControl(HWND hWnd,CDC * pDC,DWORD hHandle,UINT uID,BYTE exParam = 0,bool bForceVisible=false,bool bCenterDraw=false);


/*
 函数名   : wuiSetVisiable
 说明     : 设置控件是否可以显示，对所有文本起效
 返回类型 : void 
 参数     :  hHandle,已打开的buc文件句柄
          :  uID,该控件在buc文件中的ID号
          :  bVisiable,是否显示,默认true为显示,false为不显示
 */
extern BZ2WUI_API void wuiSetVisiable(DWORD hHandle,UINT uID,BOOL bVisiable=TRUE);

/*
 函数名   : wuiSetText
 说明     : 设置控件的文本，只对文本控件起效
 返回类型 : void 
 参数     :  hHandle,已打开的buc文件句柄
          :  uID,该控件在buc文件中的ID号
          :  strText,控件上的文本
*/
extern BZ2WUI_API void wuiSetText(DWORD hHandle,UINT uID,CString strText);

/*
 函数名   : wuiSetButtonStatus
 说明     : 设置控件的按钮状态，只对按钮控件起效
            按钮状态与按钮图片的对应顺序以该按钮图片为准		                       
 返回类型 : void 
 参数     :  hHandle,已打开的buc文件句柄
          :  uID,该控件在buc文件中的ID号
          :  uStatus,状态数字，最多可以有四个值:
			    0，代表第一个状态
				1，代表第二个状态
				2，代表第三个状态
				3，代表第四个状态	
 */
extern BZ2WUI_API void wuiSetButtonStatus(DWORD hHandle,UINT uID,int uStatus);

/*
 函数名   : wuiSetRuntimePath
 说明     : 反向设置文件路径，当在当前目录下找不到相应的资源文件时可以从sPath所指
            的目录下去找
 返回类型 : void 
 参数     :  hHandle,已打开的buc文件句柄
          :  sPath,所指定目录字符串
*/
extern BZ2WUI_API void wuiSetRuntimePath(DWORD hHandle,CString sPath);

/*
 函数名   : wuiGetControlImageSizeById
 作者     : wushuqun 
 日期     : 2009.2.4
 说明     : 得到控件图片的左上角位置及尺寸
 返回类型 : void 
 参数     : hHandle,已打开的buc文件句柄
          : uID,该控件在buc文件中的ID号
          : leftPt,传入的CPoint 对象引用，将该控件图片在整张图中的左上角位置写入该变量
          : size,传入的CSize对象引用，将该控件图片的尺寸(宽、高)写入该变量
*/
extern BZ2WUI_API void wuiGetControlImageSizeById(DWORD hHandle,UINT uID,CPoint & leftPt,CSize & size);

/*
 函数名   : wuiDrawImageById
 作者     : wushuqun 
 日期     : 2009.3.4
 说明     : 直接绘图，绘图大小为uID代表图的大小,可透明绘图
 返回类型 : BOOL 
 参数     : hHandle,已打开的buc文件句柄
          : uID,该控件在buc文件中的ID号
          : desX,目标X起始坐标
          : desY,目标Y起始坐标
		  : pDC,目的DC
		  : transColor,指定透明色，默认为RGB(255,0,255);
*/
extern BZ2WUI_API BOOL wuiDrawImageById(DWORD hHandle,UINT uID,CDC * pDC,int desX,int desY,COLORREF transColor = 0xFF00FF);

/*
 函数名   : wuiDrawImageById
 作者     : wushuqun 
 日期     : 2009.3.4
 说明     : 在pDC上绘图，可以缩放，设定透明色
 返回类型 : BOOL 
 参数     : hHandle,已打开的buc文件句柄
          : uID,该控件在buc文件中的ID号
          : desX,目标X起始坐标
          : desY,目标Y起始坐标
		  : desWidth,预绘制宽度
		  : desHeight,预绘制高度
		  : pDC,目的DC
		  : transColor,指定透明色，默认为RGB(255,0,255);
*/
extern BZ2WUI_API BOOL wuiDrawImageById(DWORD hHandle,UINT uID,CDC * pDC,int desX,int desY,int desWidth,int desHeight,COLORREF transColor = 0xFF00FF);

