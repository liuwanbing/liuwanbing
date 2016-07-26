#pragma once

#define  MAX_INCONTAIN_NUM    500                 ///<包容器最多容纳控件数量
#define  MAX_CARTOONFRAMNUM   50                  ///<动画最大帧数
using namespace std;

/// 当前版本号
//const double g_dbCurVersion = 8.3;
//const double g_dbCurVersion = 8.4;
//const double g_dbCurVersion = 8.5;      // 在dat头中加入初始化信息，加入窗口大小改变
//const double g_dbCurVersion = 8.6;        // 按鈕控件添加文本屬性消息
//const double g_dbCurVersion = 8.7;        // 用户输入控件加入选择字体颜色
//const double g_dbCurVersion = 8.8;        // 字体显示模式保存
const double g_dbCurVersion = 8.9;        // 字体描边

typedef struct _XUIStructInit
{
    int    nWndWidth;
    int    nWndHeight;
} XUIStructInit;

/// 用来识别版本，采用不同类型
struct Type_8_1
{
};

	struct MyRect_t
	{
		WORD x;
		WORD y;
		WORD w;
		WORD h;
		MyRect_t(): x(0), y(0), w(0), h(0){}
		MyRect_t(int _x, int _y, int _w, int _h) : x(_x), y(_y), w(_w), h(_h)
		{
		}
	};
	///配置文件类
	struct ControlCommonInfo 
	{
		int    Id;                    ///<控件ID
		wchar_t pName[MAX_PATH];                   ///<控件名字
		byte    Mode;                  ///<控件显示模式，居中，左，右，上，下。。。。
		bool    Visible;               ///<控件是否显示
		int     X;                     ///<控件显示坐标
		int     Y;                     ///<控件显示坐标
		int    W;                     ///<控件显示坐标
		int    H;                     ///<控件显示坐标
		ControlKind   controlKind;                      ///<控件类型-------和列表中的一样
		int  Zorder;                            ///<控件显示层次
		ControlCommonInfo()
		{
			ZeroMemory(pName,MAX_PATH);
		}
	};

	struct ControlCommonInfo4
	{
		int    Id;                    ///<控件ID
		wchar_t pName[MAX_PATH];                   ///<控件名字
		byte    Mode;                  ///<控件显示模式，居中，左，右，上，下。。。。
		bool    Visible;               ///<控件是否显示
		int     X;                     ///<控件显示坐标
		int     Y;                     ///<控件显示坐标
		int    W;                     ///<控件显示坐标
		int    H;                     ///<控件显示坐标
		ControlKind   controlKind;                      ///<控件类型-------和列表中的一样
		int  Zorder;                            ///<控件显示层次
		int  TableIndex;             ///<焦点索引
		ControlCommonInfo4()
		{
			ZeroMemory(pName,MAX_PATH);
		}
				
	};
	
	///按扭，图片存储信息
	struct ImageButtonInfo //UI.dat8.5及以下版本
	{
		int   SrcX;                    ///<要绘制的源图像部分的左上角的 x 坐标
		int   SrcY;					   ///<要绘制的源图像部分的左上角的 y 坐标
		int   SrcW;                    ///<要绘制的源图像部分的宽
		int   SrcH;                    ///<要绘制的源图像部分的高
		wchar_t  pPath[MAX_PATH];      ///<控件加载图牌地址

		ImageButtonInfo()
		{
			ZeroMemory(this, sizeof(ImageButtonInfo));
		}
	};

	struct ImageButtonInfo_8_6			//UI.dat8.6版本
	{
		int   SrcX;                    ///<要绘制的源图像部分的左上角的 x 坐标
		int   SrcY;					   ///<要绘制的源图像部分的左上角的 y 坐标
		int   SrcW;                    ///<要绘制的源图像部分的宽
		int   SrcH;                    ///<要绘制的源图像部分的高
		wchar_t  pPath[MAX_PATH];      ///<控件加载图牌地址

		wchar_t   Text[MAX_PATH];		///< Text中显示的内容
		wchar_t   FontStyle[MAX_PATH];	///<字体类型    宋体，幼圆。。。
		int		  nFontSize;			///<显示字体的大小
		COLORREF  crFontColor;			///<字体颜色
		FONTSHOWSTYLE bFontShowStyle;	///<字显示方式，0表示左对齐，1表示居中，2表示右对齐
		bool      bItalicFlag;          ///<是否是斜体
		bool      bBoldFlag;            ///<是否加粗
		int       iTextTop;				///<文本的垂直位置

		ImageButtonInfo_8_6()
		{
			ZeroMemory(this, sizeof(ImageButtonInfo_8_6));
		}
	};

	struct CardSpeciaInfo
	{
		CardSpeciaInfo(){ZeroMemory(this, sizeof(CardSpeciaInfo));}
		int								CardHight;					///<扑克高度
		int								CardWidth;					///<扑克宽度
		int								CardHSpace;					///<扑克间隔
		int                             CardVSpace;                 ///<竖向牌间隔
		int								CardUpSpace;				///<跳起高度
		byte							DirLook;                 ///<扑克放置方向
		bool                            OwnerFlag;               ///<是否是自己手上牌控件
		wchar_t                         path[2][MAX_PATH];           ///<用于存放8个IMAGE的路径
	};

	struct CartoonInfo 
	{
		CartoonInfo(){ZeroMemory(this, sizeof(CartoonInfo));}
		byte       bFrameNum;     ///<动画帧数
		int       nSpeed;        ///<播放速度
		int        iPlayNum;      ///<动画播放次数
		wchar_t      vPath[20][MAX_PATH];     ///<动画所有帧的路径
	};
     ///<与上面结构不同，主要是保存的最大帧数加大，以前为20帧，可能不够用。
	struct CartoonNewInfo 
	{
		CartoonNewInfo(){ZeroMemory(this, sizeof(CartoonNewInfo));}
		byte       bFrameNum;     ///<动画帧数
		int       nSpeed;        ///<播放速度
		int        iPlayNum;      ///<动画播放次数
		wchar_t      vPath[MAX_CARTOONFRAMNUM][MAX_PATH];     ///<动画所有帧的路径
	};

	struct TextInfo 
	{
		TextInfo(){ZeroMemory(this, sizeof(TextInfo));}
		wchar_t    Text[MAX_PATH];  ///< Text中显示的内容
		wchar_t    FontStyle[MAX_PATH]; ///<字体类型    宋体，幼圆。。。
		bool      bMutiShow;    ///<是否多行显示
		int      nFontSize;///<显示字体的大小
		COLORREF  crFontColor; ///<字体颜色
		bool      bItalicFlag;             ///<是否是斜体
		bool      bBoldFlag;               ///<是否加粗
		bool      bUnderLineFlag;          ///<是否有下划线
		bool      bIsManyChar;             ///<是否会有很多这种字体的汉字
		//FONTSHOWSTYLE      bFontShowStyle; ///<字显示方式，0表示左对齐，1表示居中，2表示右对齐
		///FONTSHOWDIRECTION      bFontShowDirection; ///<字体显示方向 0表示横向，1表示竖向
	};
	
#pragma pack(1)
	struct TextInfo88 
	{
		TextInfo88(){ZeroMemory(this, sizeof(TextInfo88));}
		wchar_t    Text[MAX_PATH];  ///< Text中显示的内容
		wchar_t    FontStyle[MAX_PATH]; ///<字体类型    宋体，幼圆。。。
		bool      bMutiShow;    ///<是否多行显示
		int      nFontSize;///<显示字体的大小
		COLORREF  crFontColor; ///<字体颜色
		bool      bItalicFlag;             ///<是否是斜体
		bool      bBoldFlag;               ///<是否加粗
		bool      bUnderLineFlag;          ///<是否有下划线
		bool      bIsManyChar;             ///<是否会有很多这种字体的汉字
		FONTSHOWSTYLE      bFontShowStyle; ///<字显示方式，0表示左对齐，1表示居中，2表示右对齐
		///FONTSHOWDIRECTION      bFontShowDirection; ///<字体显示方向 0表示横向，1表示竖向
	};
#pragma pack()

//——LCF
#pragma pack(1)
	struct TextInfo89 
	{
		TextInfo89(){ZeroMemory(this, sizeof(TextInfo89));}
		wchar_t    Text[MAX_PATH];  ///< Text中显示的内容
		wchar_t    FontStyle[MAX_PATH]; ///<字体类型    宋体，幼圆。。。
		bool      bMutiShow;    ///<是否多行显示
		int      nFontSize;///<显示字体的大小
		COLORREF  crFontColor; ///<字体颜色
		bool      bItalicFlag;             ///<是否是斜体
		bool      bBoldFlag;               ///<是否加粗
		bool      bUnderLineFlag;          ///<是否有下划线
		bool      bIsManyChar;             ///<是否会有很多这种字体的汉字
		FONTSHOWSTYLE      bFontShowStyle; ///<字显示方式，0表示左对齐，1表示居中，2表示右对齐
		bool bIsOutline;				   ///字体是否描边——LCF
		COLORREF  crFontOutlineColor;      ///<字体描边颜色——LCF
	};
#pragma pack()
	struct EditorTextInfo
	{
		EditorTextInfo(){ZeroMemory(this, sizeof(EditorTextInfo));}
		wchar_t    _Text[MAX_PATH];  ///< Text中显示的内容
		wchar_t    _FontStyle[MAX_PATH]; ///<字体类型    宋体，幼圆。。。
		bool      _bMutiShow;    ///<是否多行显示
		int      _nFontSize;///<显示字体的大小
		COLORREF  _crFontColor; ///<字体颜色
		bool      _bItalicFlag;             ///<是否是斜体
		bool      _bBoldFlag;               ///<是否加粗
		bool      _bUnderLineFlag;          ///<是否有下划线
		bool      _bIsManyChar;             ///<是否会有很多这种字体的汉字
		COLORREF  _crSorColor;                ///<光标颜色
		COLORREF  _crRectFrame;             ///<外框边框颜色
		COLORREF  _crRectBk;                ///<外杠底色
		bool      _bIsPassWord;             ///<是否是密码框
	};

	struct EditorTextInfo89
	{
		EditorTextInfo89(){ZeroMemory(this, sizeof(EditorTextInfo89));}
		wchar_t    _Text[MAX_PATH];  ///< Text中显示的内容
		wchar_t    _FontStyle[MAX_PATH]; ///<字体类型    宋体，幼圆。。。
		bool      _bMutiShow;    ///<是否多行显示
		int      _nFontSize;///<显示字体的大小
		COLORREF  _crFontColor; ///<字体颜色
		bool      _bItalicFlag;             ///<是否是斜体
		bool      _bBoldFlag;               ///<是否加粗
		bool      _bUnderLineFlag;          ///<是否有下划线
		bool      _bIsManyChar;             ///<是否会有很多这种字体的汉字
		COLORREF  _crSorColor;                ///<光标颜色
		COLORREF  _crRectFrame;             ///<外框边框颜色
		COLORREF  _crRectBk;                ///<外杠底色
		bool      _bIsPassWord;             ///<是否是密码框
		bool bIsOutline;				   ///字体是否描边——LCF
		COLORREF  crFontOutlineColor;      ///<字体描边颜色——LCF
	};

	///包容器存储结构
	struct ContainInfo
	{
		ContainInfo(){ZeroMemory(this, sizeof(ContainInfo));}
		int  controlSize;                             ///包含控件个数
		ControlKind controlKind[MAX_INCONTAIN_NUM];             ///包含控件类型
		wchar_t _vPath[MAX_PATH];                     ///<容器底图路径
	};

	///包容器存储结构
	struct ContainInfoNew 
	{
		ContainInfoNew(){ZeroMemory(this, sizeof(ContainInfoNew));}
		int  controlSize;                             ///包含控件个数
		ControlKind controlKind[MAX_INCONTAIN_NUM];             ///包含控件类型
		wchar_t _vPath[MAX_PATH];                     ///<容器底图路径
		bool   _bIsMove;                              ///<容器是否可移动
	};
	///包容器存储结构 ///用于4版本的
	struct ContainInfo4
	{
		ContainInfo4(){ZeroMemory(this, sizeof(ContainInfo4));}
		int  controlSize;                             ///包含控件个数
		ControlKind controlKind[MAX_INCONTAIN_NUM];             ///包含控件类型
		bool        controlShowFlag[MAX_INCONTAIN_NUM];         ///<是否随控件显示而显示
		wchar_t _vPath[MAX_PATH];                     ///<容器底图路径
		bool   _bIsMove;                              ///<容器是否可移动
	};
    ///包容器存储结构 ///用于7版本的
    struct ContainInfo7
    {
        ContainInfo7(){ZeroMemory(this, sizeof(ContainInfo7));}
        int  controlSize;                             ///包含控件个数
        ControlKind controlKind[MAX_INCONTAIN_NUM];             ///包含控件类型
        bool        controlShowFlag[MAX_INCONTAIN_NUM];         ///<是否随控件显示而显示
        wchar_t _vPath[MAX_PATH];                     ///<容器底图路径
        bool   _bIsMove;                              ///<容器是否可移动
        bool   _bIsPagiStyle;                         ///<是否分页
        int    _nNumperPage;                          ///<一页几个
        int    _nCurrentPage;                         ///<当前页
        int    _nVecNoPagiID[10];                     ///<10个不分页的ID号
    };

	///包容器存储结构 ///用于8版本的（最新版本为8）
	struct ContainInfo8
	{
		ContainInfo8(){ZeroMemory(this, sizeof(ContainInfo8));}
		int  controlSize;                             ///包含控件个数
		ControlKind controlKind[MAX_INCONTAIN_NUM];             ///包含控件类型
		//bool        controlShowFlag[MAX_INCONTAIN_NUM];         ///<是否随控件显示而显示
		wchar_t _vPath[MAX_PATH];                     ///<容器底图路径
		bool   _bIsMove;                              ///<容器是否可移动
		bool   _bIsVisable;							  ///< 是否可见
		bool   _bIsPagiStyle;                         ///<是否分页
		int    _nNumperPage;                          ///<一页几个
		int    _nCurrentPage;                         ///<当前页
		int    _nVecNoPagiID[10];                     ///<10个不分页的ID号
	};

	///下拉条
	struct ComboxInfo
	{
		ComboxInfo(){ZeroMemory(this, sizeof(ComboxInfo));}
		wchar_t    _fontStyle[MAX_PATH]; ///<字体类型    宋体，幼圆。。。
		int      _nFontSize;///<显示字体的大小
		wchar_t      _vPath[MAX_PATH];     ///<下拉按扭的图片路径
		
		COLORREF  _crFontColor;                  ///<字体颜色
		COLORREF  _crMoveRectColor;              ///<移动框的颜色
		COLORREF  _crBkRectFillColor;                ///<控件下拉底色
		COLORREF  _crBkFrameColor;               ///<控件下拉边框颜色

		COLORREF  _crSor;                        ///<光标颜色
		COLORREF  _crEditorFrame;                ///<编辑框边框颜色
		COLORREF  _crEditorFill;                 ///<编辑框填充颜色
		COLORREF  _crFrame;                      ///<控件外框边框颜色

	
	};

	///下拉条
	struct ComboxInfo89
	{
		ComboxInfo89(){ZeroMemory(this, sizeof(ComboxInfo89));}
		wchar_t    _fontStyle[MAX_PATH]; ///<字体类型    宋体，幼圆。。。
		int      _nFontSize;///<显示字体的大小
		wchar_t      _vPath[MAX_PATH];     ///<下拉按扭的图片路径

		COLORREF  _crFontColor;                  ///<字体颜色
		COLORREF  _crMoveRectColor;              ///<移动框的颜色
		COLORREF  _crBkRectFillColor;                ///<控件下拉底色
		COLORREF  _crBkFrameColor;               ///<控件下拉边框颜色

		COLORREF  _crSor;                        ///<光标颜色
		COLORREF  _crEditorFrame;                ///<编辑框边框颜色
		COLORREF  _crEditorFill;                 ///<编辑框填充颜色
		COLORREF  _crFrame;                      ///<控件外框边框颜色
		bool bIsOutline;				   ///字体是否描边——LCF
		COLORREF  crFontOutlineColor;      ///<字体描边颜色——LCF

	};
	
	struct InputeUserNameINfo
	{
		InputeUserNameINfo(){ZeroMemory(this, sizeof(InputeUserNameINfo));}
		wchar_t    _fontStyle[MAX_PATH]; ///<字体类型    宋体，幼圆。。。
		int      _nFontSize;///<显示字体的大小
		wchar_t      _vPath[MAX_PATH];     ///<下拉按扭的图片路径

		COLORREF  _crFontColor;                  ///<字体颜色
		COLORREF  _crMoveRectColor;              ///<移动框的颜色
		COLORREF  _crBkRectFillColor;                ///<控件下拉底色
		COLORREF  _crBkFrameColor;               ///<控件下拉边框颜色

		COLORREF  _crSor;                        ///<光标颜色
		COLORREF  _crEditorFrame;                ///<编辑框边框颜色
		COLORREF  _crEditorFill;                 ///<编辑框填充颜色
		COLORREF  _crFrame;                      ///<控件外框边框颜色
		wchar_t   _vDeleteButtonPath[MAX_PATH];  ///<删除按扭的图片路径
		int       _nDeleteButtonW;               ///<删除按扭的宽
	};
#pragma pack(1)
	struct InputUserNameInfo_8_1
	{
		InputUserNameInfo_8_1(){ZeroMemory(this, sizeof(InputUserNameInfo_8_1));}
		wchar_t    _fontStyle[MAX_PATH]; ///<字体类型    宋体，幼圆。。。
		int      _nFontSize;///<显示字体的大小
		wchar_t      _vPath[MAX_PATH];     ///<下拉按扭的图片路径

		BYTE	  _BoldAndItalic;			///<是否加粗和斜体 0x01表示粗体，0x02表示斜体

		COLORREF  _crFontColor;                  ///<字体颜色
		COLORREF  _crMoveRectColor;              ///<移动框的颜色
		COLORREF  _crBkRectFillColor;                ///<控件下拉底色
		COLORREF  _crBkFrameColor;               ///<控件下拉边框颜色

		COLORREF  _crSor;                        ///<光标颜色
		COLORREF  _crEditorFrame;                ///<编辑框边框颜色
		COLORREF  _crEditorFill;                 ///<编辑框填充颜色
		COLORREF  _crFrame;                      ///<控件外框边框颜色
		wchar_t   _vDeleteButtonPath[MAX_PATH];  ///<删除按扭的图片路径
		int       _nDeleteButtonW;               ///<删除按扭的宽
	};
#pragma pack()

#pragma pack(1)
    struct InputUserNameInfo_8_2
    {
        InputUserNameInfo_8_2(){ZeroMemory(this, sizeof(InputUserNameInfo_8_2));}
        wchar_t    _fontStyle[MAX_PATH]; ///<字体类型    宋体，幼圆。。。
        int      _nFontSize;///<显示字体的大小
        wchar_t      _vPath[MAX_PATH];     ///<下拉按扭的图片路径

        BYTE	  _BoldAndItalic;			///<是否加粗和斜体 0x01表示粗体，0x02表示斜体

        COLORREF  _crFontColor;                  ///<字体颜色
        COLORREF  _crMoveRectColor;              ///<移动框的颜色
        COLORREF  _crBkRectFillColor;                ///<控件下拉底色
        COLORREF  _crBkFrameColor;               ///<控件下拉边框颜色

        COLORREF  _crSor;                        ///<光标颜色
        COLORREF  _crEditorFrame;                ///<编辑框边框颜色
        COLORREF  _crEditorFill;                 ///<编辑框填充颜色
        COLORREF  _crFrame;                      ///<控件外框边框颜色
        wchar_t   _vDeleteButtonPath[MAX_PATH];  ///<删除按扭的图片路径
        int       _nDeleteButtonW;               ///<删除按扭的宽


        wchar_t	  _vScrollBkPath[MAX_PATH];		///<滚动条底图
        wchar_t   _vScrollTPath[MAX_PATH];		///<滚动条上按扭
        wchar_t   _vScrollBPath[MAX_PATH];		///<滚动条下按扭
        wchar_t   _vScrollBarPath[MAX_PATH];	///<滚轮图片
    };
#pragma pack()

#pragma pack(1)
    struct InputUserNameInfo_8_7
    {
        InputUserNameInfo_8_7(){ZeroMemory(this, sizeof(InputUserNameInfo_8_7));}
        wchar_t    _fontStyle[MAX_PATH]; ///<字体类型    宋体，幼圆。。。
        int      _nFontSize;///<显示字体的大小
        wchar_t      _vPath[MAX_PATH];     ///<下拉按扭的图片路径

        BYTE	  _BoldAndItalic;			///<是否加粗和斜体 0x01表示粗体，0x02表示斜体

        COLORREF  _crFontColor;                  ///<字体颜色
        COLORREF  _crMoveRectColor;              ///<移动框的颜色
        COLORREF  _crBkRectFillColor;                ///<控件下拉底色
        COLORREF  _crBkFrameColor;               ///<控件下拉边框颜色

        COLORREF  _crSor;                        ///<光标颜色
        COLORREF  _crEditorFrame;                ///<编辑框边框颜色
        COLORREF  _crEditorFill;                 ///<编辑框填充颜色
        COLORREF  _crFrame;                      ///<控件外框边框颜色
        wchar_t   _vDeleteButtonPath[MAX_PATH];  ///<删除按扭的图片路径
        int       _nDeleteButtonW;               ///<删除按扭的宽


        wchar_t	  _vScrollBkPath[MAX_PATH];		///<滚动条底图
        wchar_t   _vScrollTPath[MAX_PATH];		///<滚动条上按扭
        wchar_t   _vScrollBPath[MAX_PATH];		///<滚动条下按扭
        wchar_t   _vScrollBarPath[MAX_PATH];	///<滚轮图片
		DWORD	  _crFontSelectColor;			///<字体选择颜色
    };
#pragma pack()

#pragma pack(1)
	struct InputUserNameInfo89
	{
		InputUserNameInfo89(){ZeroMemory(this, sizeof(InputUserNameInfo89));}
		wchar_t    _fontStyle[MAX_PATH]; ///<字体类型    宋体，幼圆。。。
		int      _nFontSize;///<显示字体的大小
		wchar_t      _vPath[MAX_PATH];     ///<下拉按扭的图片路径

		BYTE	  _BoldAndItalic;			///<是否加粗和斜体 0x01表示粗体，0x02表示斜体

		COLORREF  _crFontColor;                  ///<字体颜色
		COLORREF  _crMoveRectColor;              ///<移动框的颜色
		COLORREF  _crBkRectFillColor;                ///<控件下拉底色
		COLORREF  _crBkFrameColor;               ///<控件下拉边框颜色

		COLORREF  _crSor;                        ///<光标颜色
		COLORREF  _crEditorFrame;                ///<编辑框边框颜色
		COLORREF  _crEditorFill;                 ///<编辑框填充颜色
		COLORREF  _crFrame;                      ///<控件外框边框颜色
		wchar_t   _vDeleteButtonPath[MAX_PATH];  ///<删除按扭的图片路径
		int       _nDeleteButtonW;               ///<删除按扭的宽


		wchar_t	  _vScrollBkPath[MAX_PATH];		///<滚动条底图
		wchar_t   _vScrollTPath[MAX_PATH];		///<滚动条上按扭
		wchar_t   _vScrollBPath[MAX_PATH];		///<滚动条下按扭
		wchar_t   _vScrollBarPath[MAX_PATH];	///<滚轮图片
		DWORD	  _crFontSelectColor;			///<字体选择颜色
		bool bIsOutline;				   ///字体是否描边——LCF
		COLORREF  crFontOutlineColor;      ///<字体描边颜色——LCF
	};
#pragma pack()
	///进度条
	struct ProgressInfo
	{
		ProgressInfo(){ZeroMemory(this, sizeof(ProgressInfo));}
		//COLORREF  _crBk;        ///<进度条底色
		//COLORREF  _crFrame;     ///<进度条外框颜色
		//COLORREF  _crProgress;  ///<中间的进度条的颜色
		wchar_t    _vBkPath[MAX_PATH]; ///<进度条底色路径
		wchar_t    _vProgressPath[MAX_PATH]; ///<进度条显示图片路径
	};
	///滚动条
	struct ScrollBarInfo
	{
		ScrollBarInfo(){ZeroMemory(this, sizeof(ScrollBarInfo));}
		wchar_t      _bkPath[MAX_PATH];     ///<滚动条底色图片路径
		wchar_t      _tPath[MAX_PATH];     ///<滚动条顶端图片路径
		wchar_t      _bPath[MAX_PATH];     ///<滚动条底端图片路径
		wchar_t      _sPath[MAX_PATH];     ///<滚动条滚轮图片路径
		bool         _verticalFlag;        ///<滚动条方向，横向或竖和，true为竖向
	};
	///IMAGE移动动画
	struct ImageFlashInfo
	{
		ImageFlashInfo(){ZeroMemory(this, sizeof(ImageFlashInfo));}
		//wchar_t    _vpath[MAX_PATH];       ///<IMAGE图片路径
		int        _nFrameNum;               ///<动画总帧数
		POINT      _ptStart;                 ///<动画播放的起始坐标
		POINT      _ptEnd;                   ///<动画播放的终点坐标
		POINT      _ptControl;               ///<中间控制点坐标
		int        _nMoveTotalTime;          ///<动画移动总时间
		int        _nPlayTotalTime;          ///<动画播放总时间
		int        _nPlayNum;                ///<动画播放次数
		bool       _bLineKind;               ///<是否是直线类型的
		bool       _bMoveFlag;               ///<动画是否可以移动
		wchar_t      _vPath[20][MAX_PATH];     ///<动画所有帧的路径
	};

	///单选框
	struct RadioButtonInfo 
	{
		RadioButtonInfo(){ZeroMemory(this, sizeof(RadioButtonInfo));}
		wchar_t _vBkPath[MAX_PATH];       ///<存储底图路径
		wchar_t _vSelectPath[MAX_PATH];   ///<选择框路径
		wchar_t    _Text[MAX_PATH];  ///< Text中显示的内容
		wchar_t    _FontStyle[MAX_PATH]; ///<字体类型    宋体，幼圆。。。
		int      _nFontSize;///<显示字体的大小
		COLORREF  _crFontColor; ///<字体颜色
		int       _nBkWith;    ///<底图宽度
	};
	///控件列表
	struct ListControlInfo
	{
		ListControlInfo(){ZeroMemory(this, sizeof(ListControlInfo));}
		wchar_t _vBkPath[MAX_PATH];       ///<存储底图路径
		wchar_t _vHeadPath[MAX_PATH];     ///<表头图片
		wchar_t _vHeadSpacePath[MAX_PATH]; ///<表头间隔图片
		wchar_t _vVerScrollBkPath[MAX_PATH];  ///<滚动条底图
		wchar_t _vVerScrollTPath[MAX_PATH];   ///<滚动条上按扭
		wchar_t _vVerScrollBPath[MAX_PATH];   ///<滚动条下按扭
		wchar_t _vVerScrollBarPath[MAX_PATH]; ///<滚轮图片

		wchar_t _vHorizScrollBkPath[MAX_PATH];  ///<滚动条底图
		wchar_t _vHorizScrollTPath[MAX_PATH];   ///<滚动条上按扭
		wchar_t _vHorizScrollBPath[MAX_PATH];   ///<滚动条下按扭
		wchar_t _vHorizScrollBarPath[MAX_PATH]; ///<滚轮图片

		bool    _bHaveScroll;              ///<是否一直有滚动条
		wchar_t    _FontStyle[MAX_PATH]; ///<字体类型    宋体，幼圆。。。
		int      _nFontSize;///<显示字体的大小
		COLORREF  _crFontColor; ///<字体颜色
		int       _nHeadHeight; ///<表头对像的高
	};
	///控件列表
	struct ListControlInfo89
	{
		ListControlInfo89(){ZeroMemory(this, sizeof(ListControlInfo89));}
		wchar_t _vBkPath[MAX_PATH];       ///<存储底图路径
		wchar_t _vHeadPath[MAX_PATH];     ///<表头图片
		wchar_t _vHeadSpacePath[MAX_PATH]; ///<表头间隔图片
		wchar_t _vVerScrollBkPath[MAX_PATH];  ///<滚动条底图
		wchar_t _vVerScrollTPath[MAX_PATH];   ///<滚动条上按扭
		wchar_t _vVerScrollBPath[MAX_PATH];   ///<滚动条下按扭
		wchar_t _vVerScrollBarPath[MAX_PATH]; ///<滚轮图片

		wchar_t _vHorizScrollBkPath[MAX_PATH];  ///<滚动条底图
		wchar_t _vHorizScrollTPath[MAX_PATH];   ///<滚动条上按扭
		wchar_t _vHorizScrollBPath[MAX_PATH];   ///<滚动条下按扭
		wchar_t _vHorizScrollBarPath[MAX_PATH]; ///<滚轮图片

		bool    _bHaveScroll;              ///<是否一直有滚动条
		wchar_t    _FontStyle[MAX_PATH]; ///<字体类型    宋体，幼圆。。。
		int      _nFontSize;///<显示字体的大小
		COLORREF  _crFontColor; ///<字体颜色
		int       _nHeadHeight; ///<表头对像的高
		bool bIsOutline;				   ///字体是否描边——LCF
		COLORREF  crFontOutlineColor;      ///<字体描边颜色——LCF
	};
	///聊天框
	struct TalkControlInfo
	{
		TalkControlInfo(){ZeroMemory(this, sizeof(TalkControlInfo));}
		wchar_t _vBkPath[MAX_PATH];       ///<存储底图路径
		wchar_t _vScrollBkPath[MAX_PATH];  ///<滚动条底图
		wchar_t _vScrollTPath[MAX_PATH];   ///<滚动条上按扭
		wchar_t _vScrollBPath[MAX_PATH];   ///<滚动条下按扭
		wchar_t _vScrollBarPath[MAX_PATH]; ///<滚轮图片
	};

	///聊天框
	struct TalkControlInfo6
	{
		TalkControlInfo6(){ZeroMemory(this, sizeof(TalkControlInfo6));}
		wchar_t _vBkPath[MAX_PATH];       ///<存储底图路径
		wchar_t _vScrollBkPath[MAX_PATH];  ///<滚动条底图
		wchar_t _vScrollTPath[MAX_PATH];   ///<滚动条上按扭
		wchar_t _vScrollBPath[MAX_PATH];   ///<滚动条下按扭
		wchar_t _vScrollBarPath[MAX_PATH]; ///<滚轮图片
		wchar_t _vFaceImagePath[MAX_PATH]; ///<表情文件
	};
	///不可移动牌控件
	struct NoMoveCardControlInfo
	{
		NoMoveCardControlInfo(){ZeroMemory(this, sizeof(NoMoveCardControlInfo));}
		wchar_t _vCardJoke[MAX_PATH];    ///<大小王图片路径
		wchar_t _vCard[MAX_PATH];        ///<牌的数字图片路径
		int _nCardH;                     ///<设置牌的高
		int _nCardW;                     ///<设置牌的宽
		int _nCardHSpace;                ///<牌的横向间隔
		BYTE  _bAlignStyle;              ///<对齐方式  0-2  0表示左对齐，1表示居中，2表示右对齐
		int _bShowStyle;                 ///<显示牌的方向，0-3 顺时针显示
		bool _bIsShowNum;                ///<是否显示剩余牌的张数
		bool _bVertical;                 ///<牌的摆放模式: true:竖着摆, false:横着摆
	};

	///明牌牌控件
	struct LightCardControlInfo
	{
		LightCardControlInfo(){ZeroMemory(this, sizeof(LightCardControlInfo));}
		wchar_t _vCardJoke[MAX_PATH];       ///<大小王图片路径
		wchar_t _vCard[MAX_PATH];  ///<牌的数字图片路径
		int _nShowCardW;    ///<牌显示的宽
		int _nShowCardH;    ///<牌显示的高
		int _nShowCardRowNum;  ///<一行显示的牌张数
		int _nRowSpace;     ///<行与行之间的距离
		int _nHSpace;       ///<相邻两张牌的间隔
	};
	struct OperateCardInfo
	{
		OperateCardInfo(){ZeroMemory(this, sizeof(OperateCardInfo));}
		wchar_t _vCardJoke[MAX_PATH];       ///<大小王图片路径
		wchar_t _vCard[MAX_PATH];  ///<牌的数字图片路径
		wchar_t _vCardSelect[MAX_PATH];  ///<牌的选择图片
		int _nCardH;                     ///<设置牌的高
		int _nCardW;                     ///<设置牌的宽
		int _nCardHSpace;                ///<牌的横向间隔
		BYTE  _bAlignStyle;             ///<对齐方式  0-2  0表示左对齐，1表示居中，2表示右对齐
		int _nUpSpace;                  ///<牌的跳起高度
	};
	///数字控件
	struct NumControlInfo
	{
		NumControlInfo(){ZeroMemory(this, sizeof(NumControlInfo));}
		wchar_t _vNumPath[MAX_PATH];       ///<数字图片路径
	};
	///滑动控件
	struct SliderControlInfo
	{	
		SliderControlInfo(){ZeroMemory(this, sizeof(SliderControlInfo));}
		wchar_t _vFillPath[MAX_PATH];       ///<填充图片
		wchar_t _vButtonPath[MAX_PATH];  ///<拖动图片
		BYTE    _bSliderStyle;           ///<滑块样式  横向，竖向
	};
	///菜单框
	struct MenuControlInfo
	{
		MenuControlInfo(){ZeroMemory(this, sizeof(MenuControlInfo));}
		wchar_t _vBkImagePath[MAX_PATH];       ///<底图
		wchar_t   _vFontStyle[MAX_PATH]; ///<字体类型    宋体，幼圆。。。
		int      _nFontSize;///<显示字体的大小
		COLORREF  _crFontColor; ///<字体颜色
		
		int     _nTextHeight;         ///<所有TEXT的高度
		FONTSHOWSTYLE _bFontShowStyle;  ///<对齐模式

		COLORREF  _crLineColor;        ///<分隔线的颜色
		COLORREF  _crMoveColor;        ///<移动显示的颜色
		COLORREF  _crMoveFramColor;
	};

	///菜单框
	struct MenuControlInfo89
	{
		MenuControlInfo89(){ZeroMemory(this, sizeof(MenuControlInfo89));}
		wchar_t _vBkImagePath[MAX_PATH];       ///<底图
		wchar_t   _vFontStyle[MAX_PATH]; ///<字体类型    宋体，幼圆。。。
		int      _nFontSize;///<显示字体的大小
		COLORREF  _crFontColor; ///<字体颜色

		int     _nTextHeight;         ///<所有TEXT的高度
		FONTSHOWSTYLE _bFontShowStyle;  ///<对齐模式

		COLORREF  _crLineColor;        ///<分隔线的颜色
		COLORREF  _crMoveColor;        ///<移动显示的颜色
		COLORREF  _crMoveFramColor;
		bool bIsOutline;				   ///字体是否描边——LCF
		COLORREF  crFontOutlineColor;      ///<字体描边颜色——LCF
	};

	struct SudokuInfo
	{
		SudokuInfo(){ZeroMemory(this, sizeof(SudokuInfo));}
		wchar_t      _vPath[MAX_PATH];
		MyRect_t     _vRectSrcPos[8];
	};
#pragma pack(1)
    struct SudokuInfo_1
    {
        SudokuInfo_1(){ZeroMemory(this, sizeof(SudokuInfo_1));}

        wchar_t      _vPath[MAX_PATH];  ///< 图片路径
        MyRect_t     _vRectSrcPos[9];   ///< 9个PosInfo
        bool         bUseCenter;        ///< 是否显示中间平铺图片   
        bool         bIsFrame;            ///< 是否是边框
        bool         m_bAlighRight;
        bool         m_bAlighBottom;
    };
#pragma pack()

	struct TimeInfo
	{
		TimeInfo(){ZeroMemory(this, sizeof(TimeInfo));}
		wchar_t  _vBKImagePath[MAX_PATH]; ///<底图路径
		wchar_t  _vNumPath[MAX_PATH];   ///<时间图片
		int        _nMaxNum;           ///<显示最大数字
		int        _nMinNum;           ///<显示最小数字
	};
	struct SiTuanOperateCardInfo
	{
		SiTuanOperateCardInfo(){ZeroMemory(this, sizeof(SiTuanOperateCardInfo));}
		wchar_t _vCardJoke[MAX_PATH];       ///<大小王图片路径
		wchar_t _vCard[MAX_PATH];  ///<牌的数字图片路径
		wchar_t _vCardSelect[MAX_PATH];  ///<牌的选择图片
		int _nCardH;                     ///<设置牌的高
		int _nCardW;                     ///<设置牌的宽
		int _nCardHSpace;                ///<牌的横向间隔
		BYTE  _bAlignStyle;             ///<对齐方式  0-2  0表示左对齐，1表示居中，2表示右对齐
		int _nUpSpace;                  ///<牌的跳起高度
		int    _nCardFoldSpace;      ///<重叠间隔
		int    _nCardThreeSpace;     ///<三张或两张扑克之间的间隔
		int    _nCardThreeToThree;   ///<三张到三张，二张到二张的间隔
		int    _nCardFoldToOtherSpace;    ///<重叠牌到普通牌的间隔
		int    _nCardThreeToOtherSpace; ///<三张牌到其它牌的间隔
		int    _nCardTwoToOtherSpace;   ///<二张到其它牌之间的间隔
		wchar_t _vNum[MAX_PATH];        ///<牌叠起数字的路径
	};
	struct SiTuanNoMoveCardInfo
	{
		SiTuanNoMoveCardInfo(){ZeroMemory(this, sizeof(SiTuanNoMoveCardInfo));}
		wchar_t _vCardJoke[MAX_PATH];       ///<大小王图片路径
		wchar_t _vCard[MAX_PATH];  ///<牌的数字图片路径
		wchar_t _vNum[MAX_PATH]; ///<显示的数字图片
		int _nCardH;                     ///<设置牌的高
		int _nCardW;                     ///<设置牌的宽
		int _nCardHSpace;                ///<牌的横向间隔
		BYTE  _bAlignStyle;             ///<对齐方式  0-2  0表示左对齐，1表示居中，2表示右对齐
		int _bShowStyle;      ///<显示牌的方向，0-3 顺时针显示
		bool _bIsShowNum;               ///<是否显示剩余牌的张数
	};
	struct SiTuanLightCardInfo
	{
		SiTuanLightCardInfo(){ZeroMemory(this, sizeof(SiTuanLightCardInfo));}
		wchar_t _vCardJoke[MAX_PATH];       ///<大小王图片路径
		wchar_t _vCard[MAX_PATH];  ///<牌的数字图片路径
		int _nShowCardW;    ///<牌显示的宽
		int _nShowCardH;    ///<牌显示的高
		int _nShowCardRowNum;  ///<一行显示的牌张数
		int _nRowSpace;     ///<行与行之间的距离
		int _nHSpace;       ///<相邻两张牌的间隔
		wchar_t  _pNumPath[MAX_PATH];   ///<数字图片的路径
		int    _nFoldSpace;///叠牌间隔
		int    _nFoldToOther; ///<叠牌到其它牌的间隔
	};

    ///IMAGE移动动画
    struct ExAnimateInfo
    {
        ExAnimateInfo(){ZeroMemory(this, sizeof(ExAnimateInfo));}
        //wchar_t    _vpath[MAX_PATH];       ///<IMAGE图片路径
        int        _nFrameNum;               ///<动画总帧数
        POINT      _ptStart;                 ///<动画播放的起始坐标
        POINT      _ptEnd;                   ///<动画播放的终点坐标
        POINT      _ptControl;               ///<中间控制点坐标
        int        _nMoveTotalTime;          ///<动画移动总时间
        int        _nPlayTotalTime;          ///<动画播放总时间
        int        _nPlayNum;                ///<动画播放次数
        bool       _bLineKind;               ///<是否是直线类型的
        bool       _bMoveFlag;               ///<动画是否可以移动
        wchar_t      _vPath[50][MAX_PATH];   ///<动画所有帧的路径        

        wchar_t     wszExtFilePath[MAX_PATH];     
    };

    ///麻将控件信息
    struct MjCtrlInfo
    {
        MjCtrlInfo(){ZeroMemory(this, sizeof(MjCtrlInfo));}
        int                         m_iMjTileWid;                   /// 麻将子宽
        int                         m_iMjTileHei;                   /// 麻将子高
        MJ_TILE_UIINFO              m_TilesInfo[MAX_TILES_INCTRL];  /// 麻将信息
        int                         m_nTilesCount;                  /// 麻将个数
        wchar_t                     m_MjPicPath[MAX_PATH];          /// 麻将图片路径
    };   


    ///麻将手牌控件信息
    struct MjHandCtrlInfo
    {
        MjHandCtrlInfo(){ZeroMemory(this, sizeof(MjHandCtrlInfo));}

        int                         m_iMjTileWid;                   /// 麻将子宽
        int                         m_iMjTileHei;                   /// 麻将子高
        int                         m_nTilesHand[MAX_TILES_INCTRL]; /// 麻将信息
        int                         m_nTilesCount;                  /// 麻将个数
        BYTE                        m_bViewStation;                 /// 方位
        wchar_t                     m_MjPicPath[MAX_PATH];          /// 麻将图片路径
    };   


    ///麻将出牌控件信息
    struct MjGiveCtrlInfo
    {
        MjGiveCtrlInfo(){ZeroMemory(this, sizeof(MjGiveCtrlInfo));}

        int                         m_iMjTileWid;                   /// 麻将子宽
        int                         m_iMjTileHei;                   /// 麻将子高
        int                         m_nTilesGive[MAX_TILES_INCTRL]; /// 麻将信息
        int                         m_nTilesCount;                  /// 麻将个数
        BYTE                        m_bViewStation;                 /// 方位
        wchar_t                     m_MjPicPath[MAX_PATH];          /// 麻将图片路径
    };   

    ///麻将手牌控件信息
    struct MjWallCtrlInfo
    {
        MjWallCtrlInfo(){ZeroMemory(this, sizeof(MjWallCtrlInfo ));}

        int                         m_iMjTileWid;                   /// 麻将子宽
        int                         m_iMjTileHei;                   /// 麻将子高
        int                         m_nTilesCount;                  /// 麻将个数
        BYTE                        m_bViewStation;                 /// 方位
        wchar_t                     m_MjPicPath[MAX_PATH];          /// 麻将图片路径
    };   

    ///麻将拦牌控件
    struct MjBlockCtrlInfo
    {
        MjBlockCtrlInfo(){ZeroMemory(this, sizeof(MjBlockCtrlInfo ));}

        int                         m_iMjTileWid;                   /// 麻将子宽
        int                         m_iMjTileHei;                   /// 麻将子高
        int                         m_nTilesCount;                  /// 麻将个数
        BYTE                        m_bViewStation;                 /// 方位
        MJ_LOGIC_TYPE               m_mtTypes[MAX_BLOCK_COUNT];
        BYTE                        m_ucTiles[MAX_BLOCK_COUNT][4];
        BYTE                        m_ucChairs[MAX_BLOCK_COUNT];
        wchar_t                     m_MjPicPath[MAX_PATH];          /// 麻将图片路径
    };  
	///梭哈筹码控件信息
	struct ShCtrlInfo
	{
		ShCtrlInfo(){ZeroMemory(this, sizeof(ShCtrlInfo));}
		int                         iShTileWid;                   /// 梭哈筹码子宽
		int                         iShTileHei;                   /// 梭哈筹码子高
		int                         nTilesCount;                  /// 梭哈筹码个数
		wchar_t                     ShPicPath[MAX_PATH];          /// 梭哈筹码图片路径
		int							iShNote;                      /// 梭哈显示筹码值
		int							iShNoteType;				  /// 梭哈显示方式
	};  

#pragma pack(1)
    ///移动动作控件
    struct MoveActionCtrlInfo
    {
        MoveActionCtrlInfo(){ZeroMemory(this, sizeof(MoveActionCtrlInfo ));}

        POINT                       m_ptStart;                      /// 动画播放的起始坐标
        POINT                       m_ptControl;                    /// 中间控制点坐标
        POINT                       m_ptEnd;                        /// 动画播放的终点坐标  
        CURVE_TYPE                  m_eCurveType;                   /// 曲线类型
        unsigned int                m_uTotalMoveTime;               /// 移动总时间
        bool                        m_bIsSymmetrical;               /// 是否对称
        int                         m_nControlingID;                /// 控制的ID
    };
#pragma pack()

#pragma pack(1)
    ///移动动作控件
    struct FlashPlayCtrlInfo
    {
        FlashPlayCtrlInfo(){ZeroMemory(this, sizeof(FlashPlayCtrlInfo ));}

        wchar_t                     m_szSwfFile[MAX_PATH];
    };
#pragma pack()

#pragma pack(1)
    ///走马灯控件
    struct RSLampCtrlInfo : public TextInfo
    {
        RSLampCtrlInfo(){ZeroMemory(this, sizeof(RSLampCtrlInfo));}

        wchar_t                     m_szBkImg[MAX_PATH];
        int                         m_nRefreshTime;     // 刷新时间
        int                         m_nPixelPerSec;     // 设置滚动速度，每m_nFreshTime毫秒多少像素
        int                         m_nDir;             // 0向左，1向右
    };

	///走马灯控件
    struct RSLampCtrlInfo88 : public TextInfo88
    {
        RSLampCtrlInfo88(){ZeroMemory(this, sizeof(RSLampCtrlInfo88));}

        wchar_t                     m_szBkImg[MAX_PATH];
        int                         m_nRefreshTime;     // 刷新时间
        int                         m_nPixelPerSec;     // 设置滚动速度，每m_nFreshTime毫秒多少像素
        int                         m_nDir;             // 0向左，1向右
    };

	///走马灯控件89——LCF
	struct RSLampCtrlInfo89 : public TextInfo89
	{
		RSLampCtrlInfo89(){ZeroMemory(this, sizeof(RSLampCtrlInfo89));}

		wchar_t                     m_szBkImg[MAX_PATH];
		int                         m_nRefreshTime;     // 刷新时间
		int                         m_nPixelPerSec;     // 设置滚动速度，每m_nFreshTime毫秒多少像素
		int                         m_nDir;             // 0向左，1向右
	};
#pragma pack()

#pragma pack(1)
    ///胡子控件信息
    struct HuziCardCtrlInfo
    {
        wchar_t  pPath[MAX_PATH];                   // 控件加载图片地址
        int      nVerInterval;                      // 垂直间隔
        int      nVerCutCount;                      // 垂直切片数
        bool     bCanSelect;                        // 是否可选中
        int      nRotate;                           // 方向


        HuziCardCtrlInfo()
        {
            ZeroMemory(this, sizeof(HuziCardCtrlInfo));
        }
    };

    ///胡子控件信息
    struct HuziCardCtrlInfo2
    {
        wchar_t  pPath[MAX_PATH];                   // 控件加载图片地址
        int      nVerInterval;                      // 垂直间隔
        int      nHorInterval;                      // 横向间隔
        int      nVerCutCount;                      // 垂直切片数
        bool     bCanSelect;                        // 是否可选中
        int      nRotate;                           // 方向

        int      nSpare[4];                         // 空4个字节出来

        HuziCardCtrlInfo2()
        {
            ZeroMemory(this, sizeof(HuziCardCtrlInfo2));
        }
    };
#pragma pack()

#pragma pack(1)

	///排名榜控件信息
	struct TopPaperInfo
	{
		// 控件背景图片地址
		wchar_t  wszBkPath[MAX_PATH];                 

		//字体及大小
		wchar_t wszFontStyle[MAX_PATH];
		int nFontSize;

		//依靠方向
		int m_nLeanSide;

		//默认字体的颜色
		COLORREF crDefFontColor;

		//榜单的行数与列数
		int iRowNums;
		int iColNums;

		//榜单的行高列宽
		int iRowH;
		int iColW;

		//榜单的行距与列距
		int iRowSpace;
		wchar_t wszColSpace[64];

		//榜单着笔坐标
		int iBegTextX;
		int iBegTextY;

		//露角距离
		int m_iCornerSpace;

		//每一行字体颜色描述
		wchar_t wszRowColor[MAX_PATH];

		TopPaperInfo()
		{
			ZeroMemory(this, sizeof(TopPaperInfo));
		}
	};

#pragma pack()

///排名榜控件信息
struct TopPaperInfo89
{
	// 控件背景图片地址
	wchar_t  wszBkPath[MAX_PATH];                 

	//字体及大小
	wchar_t wszFontStyle[MAX_PATH];
	int nFontSize;

	//依靠方向
	int m_nLeanSide;

	//默认字体的颜色
	COLORREF crDefFontColor;

	//榜单的行数与列数
	int iRowNums;
	int iColNums;

	//榜单的行高列宽
	int iRowH;
	int iColW;

	//榜单的行距与列距
	int iRowSpace;
	wchar_t wszColSpace[64];

	//榜单着笔坐标
	int iBegTextX;
	int iBegTextY;

	//露角距离
	int m_iCornerSpace;

	//每一行字体颜色描述
	wchar_t wszRowColor[MAX_PATH];

	bool bIsOutline;				   ///字体是否描边——LCF
	COLORREF  crFontOutlineColor;      ///<字体描边颜色——LCF

	TopPaperInfo89()
	{
		ZeroMemory(this, sizeof(TopPaperInfo89));
	}
};
