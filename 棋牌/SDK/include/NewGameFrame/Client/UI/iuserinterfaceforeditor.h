#ifndef I_USERINTERFACEFOREDITOR_H
#define  I_USERINTERFACEFOREDITOR_H
#include <vector>
#include "IUICommands.h"


#define WM_BF_CHANGPROP    WM_USER+487
#define WM_BF_RBUTTONDOWN  WM_USER+491
#define WM_BF_KEYDOWN      WM_USER+492

enum BLONGED{NONE=1,CONTAIN=2};          ///<控件所属父类，none表示不属于任何控件，contain表示属于包容器控件
enum FONTSHOWSTYLE{AlignmentLEFT,AlignmentCenter,AlignmentFar,AlignmentVerticalCenter};    ///<字体对齐模式
enum FONTSHOWDIRECTION {FormatFlagsLevel,FormatFlagsVertical};
enum ControlKind{C_Contain,C_Button,C_Image,C_Card,C_Cartoon,C_Text,C_EditorText,C_ComBox,C_Progress,C_ScrollBar,
C_ImageFlash,C_Web,C_RadioButton,C_ListControl,C_Beacon,C_Talk,C_Num,C_NoMoveCard,C_LightCard,C_CanOperateCard,C_Slider,
C_InputUserName,C_Menu,C_Sudoku,C_Time,C_SiTuanOperateCard,C_SiTuanNoMoveCard,C_SiTuanLightCard, C_ExAnimate, 
C_MjCtrl, C_MjHandCtrl, C_MjGiveCtrl, C_MjWallCtrl, C_MjBlockCtrl, C_MoveAction, C_ShCtrl, C_FlashPlay,C_RSLamp,C_HuziCard,
C_TopPaper}; ///<控件类型
enum PLAYSTATE{START,PLAYING,PAUSE,CONTINUE,STOP};///<播放状态
enum CARD_SHOW_DIR{MYSELF,LEFT,PARTNER,RIGHT};   ///<牌显示方向 
typedef enum enmCardShape
{
	ECS_SHAPE_SPECIAL = 5,	/*!< 特别牌 */
	ECS_SHAPE_NONE = 4,		/*!< 大小王或无花色 */
	ECS_SHAPE_SPADE=3,		/*!< 黑桃 */
	ECS_SHAPE_HEARTS=2,		/*!< 红桃 */
	ECS_SHAPE_CLUBS=1,		/*!< 梅花 */
	ECS_SHAPE_DIAMONDS=0		/*!< 方块 */
}ENM_CARD_SHAPE;

//! 牌的点数
typedef enum enmCardNumber
{
	ECN_NUM_NONE = 0,		/*!< 牌背 */				
	ECN_NUM_2,
	ECN_NUM_3,
	ECN_NUM_4,
	ECN_NUM_5,
	ECN_NUM_6,
	ECN_NUM_7,
	ECN_NUM_8,
	ECN_NUM_9,
	ECN_NUM_10,
	ECN_NUM_J,
	ECN_NUM_Q,
	ECN_NUM_K,
	ECN_NUM_A,
	ECN_NUM_Joker,			/*!< 小王 */
	ECN_NUM_JOKER			/*!< 大王 */
}ENM_CARD_NUMBER;


typedef signed char         INT8, *PINT8;
typedef signed short        INT16, *PINT16;
typedef signed int          INT32, *PINT32;
typedef signed __int64      INT64, *PINT64;
typedef unsigned char       UINT8, *PUINT8;
typedef unsigned short      UINT16, *PUINT16;
typedef unsigned int        UINT32, *PUINT32;
typedef unsigned __int64    UINT64, *PUINT64; 


typedef enum enuCurveType
{
    CT_NONE     = 0,
    CT_BEZIER   = 1,
    CT_STRAIGHT = 2
} CURVE_TYPE;

//新形式数字包(数字控件用0~9+-,)
typedef struct newNumStruct
{
	bool	bHavePlusSign;			//加号
	bool	bHaveMinusSign;			//减号
	bool	bHaveComma;				//逗号

	bool	bShowPlusSignOrMinusSign;		//显示加号或减号
	bool	bShowComma;						//显示逗号
	BYTE	iShowCommaIndex;				//隔几位数字增加一个分号
	BYTE	iShowZeroType;					//显示数字为0时显示加号还是减号，还是只显示0（0时加减都不显示，1时显示加号，，2显示减号）
	newNumStruct()
	{
		bHavePlusSign =true;
		bHaveMinusSign =true;
		bHaveComma =true;

		bShowPlusSignOrMinusSign =false;
		bShowComma =false;
		iShowCommaIndex =3;
		iShowZeroType =0;
	}
}NEW_NUM_STRUCT;
/**
* @brief 曲线函数接口
*/
class ICurveLine
{
public:
    /**
    * @brief 设置曲线座标点
    * @param[in]       vPoints         第一个表起点，最后一个表终点，其它全是中间关键点
    * @return  int，正常返回0
    */
    virtual int SetCurvePoints(POINT vPoints[], int nSize) = 0;
    /**
    * @brief 得到曲线由几个点控制
    */
    virtual int GetPointsCount() = 0;
    /**
    * @brief 得到某一刻的点座标
    * @param[in]       nFinishedPersent    完成的百分比
    * @param[in]       bSymmetrical        是否为对称曲线
    * @return  int，正常返回0
    */
    virtual int GetCurrrentPoint(int nFinishedPersent, POINT *pPointOut, bool bSymmetrical = false) = 0;    
    /**
    * @brief 得到曲线类型
    */
    virtual CURVE_TYPE GetCurveType() = 0;
};

/** @brief 需要手动调用的控件
*/
interface IManual
{
public:
    /** @brief 初始化
    *  由客户端自己调用，由于OLE线程不安全
    */
    virtual void Init() = 0;
    /** @brief 绘制
    *  由客户端自己调用，由于OLE线程不安全
    */
    virtual void Draw() = 0;
    /** @brief 反初始化
    *  由客户端自己调用，由于OLE线程不安全
    */
    virtual void UnInit() = 0;
};

interface IBCInterFace
{
public:
	///控件私有属性设置
	virtual int    GetW()=0;                   ///<获取控件显示坐标	
	virtual int    GetH()=0;                    ///<获取控件显示坐标
	virtual int	GetId()=0;                      ///<获得控件ID	
	virtual	byte    GetMode()=0;                ///<控件显示模式，居中，左，右，上，下。。。。
	virtual ControlKind    GetControlKind()=0;       ///<获取控件类型
	virtual void    SetControlKind(ControlKind kind)=0;  ///<设置控件类型
	virtual void    SetId(int m_id)=0;                    ///<设置控件ID
	virtual void    SetName(wchar_t * pchar)=0;            ///<设置控件名字
	virtual void    SetMode(byte bmode)=0;			   ///<设置显示模式，居中，左，右，上，下。。。。
	virtual void    SetVisible(bool Flag)=0;      ///<设置控件是否显示
	virtual void	SetControlVisible(bool bflag)=0;///<设置是否显示
	virtual void    SetModeXY(int x,int y)=0;///<设置控件显示坐标				                            
	virtual void    SetW(int w)=0;					   ///<设置控件显示坐标
	virtual void    SetH(int h)=0;                     ///<设置控件显示坐标
	virtual int     GetModeX()=0;                     ///<获取控件显示坐标	这是获取的相对模式的坐标
	virtual int     GetModeY()=0;                     ///<获取控件显示坐标	这是获取的相对模式的坐标
	virtual wchar_t *  GetName()=0;                ///<获得控件名字
	virtual bool    GetVisible()=0;           ///<获得控件是否显示
	virtual int GetZorder()=0;///<获取控件层次
	virtual void SetZorder(INT16 zorder)=0;///<设置控件层次
	virtual void SetAbsXY(int x,int y)=0;///<设置控件实际坐标
	virtual int GetAbsX()=0; ///<获取绝对的X值
	virtual int GetAbsY()=0; ///<获取绝对的Y值
	virtual void SetControlFocus(bool flag)=0;///<设置控件焦点

	virtual POINT CoordinateOpToAbs()=0; ///<将相对坐标根据模式换成绝对坐标，用于显示
	virtual POINT CoordinateAbsToOp()=0;///<将绝对坐标换成相对坐标

	virtual void SetIsMove(bool bFlag)=0;
	virtual bool GetIsMove()=0;

	virtual void SetTableIndex(int nIndex)=0;   ///<这是设置控件的焦点顺序
	virtual int  GetTableIndex()=0;
	virtual IBCInterFace* GetParentControl()=0;

	virtual void SetHaveShrinkArea(bool bFlag)=0;
};

/// 图像控件接口
interface IImage : virtual public IBCInterFace
{
public:
	virtual wchar_t * GetPath()=0;                   ///<控件加载牌图片路径
	virtual void  SetPath(wchar_t * pchar)=0;         ///<设置控件加载图片路径
	///获取IMG信息,获取加载的图片宽度
	virtual int GetPicWidth()=0;
	///获取图片高度
	virtual int GetPicHeight()=0;

	/// 载入图片
	virtual int LoadPic(wchar_t *szFileName)=0;
	/// 设置图片显示的方向
	virtual void SetDir(int degree, POINT pt)=0;
	virtual void SetSrcPos(int x,int y,int w,int h)=0;

	/// 设置图片四个顶点的颜色，按左上，右上，左下、右下的次序
	virtual void SetColor(DWORD c1, DWORD c2, DWORD c3, DWORD c4)=0;

	virtual int GetSrcX()=0;///<获取此图在原材质中的位置
	virtual int GetSrcY()=0;///<获取此图在原材质中的位置
	virtual int GetSrcW()=0;///<获取此图在原材质中的位置
	virtual int GetSrcH()=0;///<获取此图在原材质中的位置
	virtual void SetSrcX(int x)=0; ///<设置此图在原材质中的位置
	virtual void SetSrcY(int y)=0; ///<设置此图在原材质中的位置
	virtual void SetSrcW(int w)=0; ///<设置此图在原材质中的位置
	virtual void SetSrcH(int h)=0; ///<设置此图在原材质中的位置
	
	virtual int GetShowX()=0;
	virtual int GetShowY()=0;
	virtual int GetShowW()=0;
	virtual int GetShowH()=0;

	virtual IBCInterFace * Clone(int nId)=0;

    /** @brief 设置是否左右镜像显示
     *	@param[in]      bCrossMirror
     *	@return 无
     */
    virtual void SetCrossMirror(bool bCrossMirror)=0;

};

/// 按钮控件接口
interface  IButton : virtual public IImage
{
public:
	///特有的函数 给用户接口
	virtual void SetEnable(bool flag)=0;                ///<设置控件是否可用
	virtual bool GetEnable()=0;                         ///<获取控件是否可用

	virtual void SetText(wchar_t *pchar)=0;				///<设置鼠标在按钮显示静态文字
	virtual wchar_t* GetText()=0;						///<获得在按钮显示静态文字

	virtual void SetFontColor(COLORREF crFontColor)=0;	///<设置控件字体显示颜色
	virtual COLORREF GetFontColor()=0;					///<获取控件字体显示颜色

	virtual void SetFontStyle(wchar_t * pFontStyle)=0;  ///<设置控件字体样式
	virtual wchar_t * GetFontStyle()=0;                 ///<获取控件字体样式

	virtual void SetFontSize(int fontsize)=0;			///<设置控件字体大小
	virtual int GetFontSize()=0;						///<获取控件字体大小

	virtual void SetFontShowStyle(FONTSHOWSTYLE showStyle)=0;   ///<设置控件字体显示模式，居左，居中，居右
	virtual FONTSHOWSTYLE GetFontShowStyle()=0;					///<获取控件字体显示模式，居左，居中，居右

	virtual void SetItalicFlag(bool bFlag)=0;       ///<设置是否是斜体
	virtual bool GetItalicFlag()=0;                 ///<获取字体是否是斜体

	virtual void SetBoldFlag(bool bFlag)=0;         ///<设置是否是粗体
	virtual bool GetBoldFlag()=0;                   ///<获取是否是粗体

	virtual void SetTextTop(int px) = 0;			///<设置文本的垂直位置
	virtual int GetTextTop() = 0;					///<获取文本的垂直位置

	virtual IBCInterFace * Clone(int nId)=0;
};

///文本控件
interface  IText: virtual public IBCInterFace
{
public:
	virtual wchar_t * GetText()=0;                      ///<获取控件显示内容
	virtual void SetText(wchar_t * pText)=0;///<设置控件显示内容
	virtual void SetFontColor(COLORREF crFontColor)=0; ///<设置控件字体显示颜色

	virtual wchar_t * GetFontStyle()=0;                 ///<获取控件字体样式
	virtual bool GetMutiShow()=0;                       ///<获取是否多行显示标志
	virtual int GetFontSize()=0;                    ///<获取控件字体大小
	virtual COLORREF GetFontColor()=0;               ///<获取控件字体显示颜色
	virtual FONTSHOWSTYLE   GetFontShowStyle()=0;             ///<获取控件字体显示模式，居左，居中，居右
	virtual FONTSHOWDIRECTION   GetFontShowDirection()=0;         ///<获取控件字体显示方向，横向，竖向
	
	virtual void SetFontStyle(wchar_t * pFontStyle)=0;  ///<设置控件字体样式
	virtual void SetFontSize(int fontsize)=0;       ///<设置控件字体大小
	virtual void SetMutiShow(bool bFlag)=0;             ///<设置是否多行显示
	
	virtual void SetFontShowStyle(FONTSHOWSTYLE showStyle)=0;   ///<设置控件字体显示模式，居左，居中，居右
	virtual void SetFontShowDirection(FONTSHOWDIRECTION showDirection)=0; ///<设置控件字体显示方向，横向，竖向

	virtual void SetItalicFlag(bool bFlag)=0;                 ///<设置是否是斜体
	virtual bool GetItalicFlag()=0;                 ///<获取字体是否是斜体
	virtual void SetBoldFlag(bool bFlag)=0;                   ///<设置是否是粗体
	virtual bool GetBoldFlag()=0;                   ///<获取是否是粗体
	virtual void SetUnderLineFlag(bool bFlag)=0;              ///<设置是滞有下划线
	virtual bool GetUnderLineFlag()=0;              ///<获取下划线标志
	virtual void SetIsManyChar(bool bFlag)=0;                 ///<设置是否有很多个字
	virtual bool GetIsManyChar()=0;                 ///<获取这种这种字体是否有很多个汉字
	
	virtual IBCInterFace * Clone(int nId)=0;
    virtual void SetMouseThough(bool bThough)=0;    ///<是否鼠标穿透

	virtual bool GetIsShowOutline()=0;	///是否显示描边——LCF
	virtual COLORREF GetOutlineColor()=0;	///描边颜色——LCF
	virtual void SetFontOutlineInfo(bool isOutline, COLORREF color) = 0;	 ///设置字体是否描边及描边颜色——LCF
};
///文本编辑框接口
interface  IEditorText : virtual public IText
{
	virtual void SetRectColor(COLORREF crColor)=0;///<设置处框颜色
	virtual void SetFillColor(COLORREF crColor)=0;///<设置处框底色
	virtual void SetSorColor(COLORREF crColor)=0;///<设置光标颜色

	virtual COLORREF GetRectColor()=0;///<获取处框颜色
	virtual COLORREF GetFillColor()=0;///<获取处框底色
	virtual COLORREF GetSorColor()=0;///<获取光标颜色

	virtual void SetIsPassWord(bool bFlag)=0; ///<设置是否是密码框
	virtual bool GetIsPassWord()=0;
	virtual IBCInterFace * Clone(int nId)=0;
};
///牌类控件接口
interface  ICard: virtual public IBCInterFace
{
public:
	virtual void SetShowNumFlag(bool bFlag)=0; ///<设置是否显示数字
	virtual bool GetShowNumFlag()=0;           ///<获取是否显示数字

	virtual void SetCardSortFlag(bool bFlag)=0; ///<设置排序方式
	virtual bool GetCardSortFlag()=0;           ///<获取排序方式
	///设置手上的牌
	virtual bool SetCard(BYTE bCardList[], BYTE bUpList[], BYTE bCardCount)=0;
	///设置打出去的牌
	virtual void SetMoveCard(BYTE bMoveList[], BYTE bMoveCardCount)=0;
	///获取上一轮打出去的牌信息，返回牌数量
	virtual byte GetMoveCard(BYTE bCardList[])=0;
	///获取升起的扑克
	virtual BYTE GetUpCard(BYTE bCardList[])=0;
	///获取手上剩余扑克信息 返回牌数目
	virtual BYTE GetCard(BYTE bCardList[], BYTE bUpList[])=0;
	///获取扑克数目
	virtual BYTE GetCardCount()=0;
	///获取手上扑克数值
	virtual BYTE GetCardValue(BYTE bIndex)=0 ;
	///获取扑克位置
	virtual POINT GetCardPos(BYTE bIndex)=0;
	///将选择的牌打出去，将剩余的牌显示好  （删除升起的扑克,显示剩余的牌）
	virtual int OutCard()=0;

	virtual void SetCardWH(int w,int h)=0;               ///<设置牌宽高
	virtual int GetCardWH(int &w,int &h)=0;              ///<获取牌的宽，高
	virtual void SetCardPrama(int hSpace,int vSpace,int uSpace)=0; ///<设置牌横向间隔，竖向间隔，跳起高度
	virtual int GetCarddPrama(int & hSpace,int &vSpace,int &uSpace)=0;///<获取牌基本属性

	virtual void SetCardDirLook(int dir)=0;///<设置牌显示方向，有四个方向
	virtual int GetCardDirLook()=0;        ///<获取牌的显示方向
	virtual void SetOwnerFlag(bool bFlag)=0 ;///<设置是否是自己手上牌控件
	virtual bool GetOwnerFlag()=0;           ///<获取是否是自己手上牌控件标志
	
};

interface  ICombox: virtual public IBCInterFace
{
public:
	///用户接口
	virtual wchar_t * GetCurSelValue()=0;                   ///<获得当前选取的项的值
	virtual void AddString(wchar_t * pchar)=0;             ///<插入元素
	virtual wchar_t * SelectString(int vindex)=0;              ///<选择里面的元素
	virtual int DeleteString(int nIndex)=0;                    ///<删除第几项
	virtual int FindString(wchar_t * pchar) const=0;           ///<查找里面的条目
	virtual int GetCount() const=0;                        ///<获取元素个数
	virtual int GetCurSel() const=0;                       ///<获得当前选取的项
	virtual int SetCurSel(int nSelect)=0;                  ///<设置当前选取的项
	virtual int GetTopIndex() const=0;                     ///<获取默认显示的项
	virtual int SetTopIndex(int nIndex)=0;                 ///<设置默认显示的项
	virtual void Clear()=0;                                ///<清空所有项

	virtual int SetItemHeight( int cyItemHeight)=0;       ///<设置的有条目的高度，宽度和控件宽度一样
	virtual int GetItemHeight() const=0;                   ///<获取各个条目的高度

	virtual void SetBoldFlag(bool bFlag)=0;				///< 设置是否粗体
	virtual bool GetBoldFlag() const=0;						///< 获取是否粗体
	virtual void SetItalicFlag(bool bFlag)=0;				///< 设置是否斜体
	virtual bool GetItalicFlag() const=0;					///< 获取是否斜体

	virtual void SetFontStyle(wchar_t * pFontStyle)=0;  ///<设置控件字体样式
	virtual void SetFontSize(int fontsize)=0;       ///<设置控件字体大小

	virtual wchar_t * GetFontStyle()=0;             ///<获取字体样式
	virtual int  GetFontSize()=0;                  ///<获取字体大小
	virtual void SetButtonPicPath(wchar_t * pchar)=0;  ///<设置按扭图片路径
	virtual void LoadButtonPic(wchar_t * pchar)=0;     ///<加载按扭图片
	virtual wchar_t * GetButtonPicPath()=0;            //<获取按扭图片路径

	
	virtual void SetFrameColor(COLORREF crColor)=0; ///<设置控件外框颜色
	virtual COLORREF GetFrameColor()=0;             ///<获取控件外框颜色

	virtual void SetEditorFrameColor(COLORREF crColor)=0; ///<设置编辑框外框颜色
	virtual COLORREF GetEditorFramColor()=0;              ///<获取编辑框外框颜色
	virtual void SetEditorFillColor(COLORREF crColor)=0;  ///<设置编辑框填充色
	virtual COLORREF GetEditorFillColor()=0;              ///<获取编辑框填充色
	
	virtual void SetBkRectFillColor(COLORREF crColor)=0;           ///<设置下拉框颜色
	virtual COLORREF GetBkRectFillColor()=0;                       ///<获取下拉条底色
	virtual void SetBkRectColor(COLORREF crColor)=0;               ///<设置下拉条边框颜色
	virtual COLORREF GetBkRectColor()=0;                           ///<获取下拉条边框颜色
	virtual void SetMoveRectFillColor(COLORREF crColor)=0;         ///<设置鼠标移动选择时的颜色
	virtual COLORREF GetMoveRectFillColor()=0;                     ///<获取鼠标移动选择时的颜色
	virtual void SetSorColor(COLORREF crColor)=0;                  ///<设置光标颜色
	virtual COLORREF GetSorColor()=0;                              ///<获取光标颜色

	virtual void SetFontColor(COLORREF crColor)=0;                 ///<设置字体颜色
	virtual COLORREF GetFontColor()=0;                             ///<获取字体颜色

	virtual IBCInterFace * Clone(int nId)=0;

    virtual void SetEditable(bool bCanEdit)=0;

	virtual void SetFontSelectColor(DWORD color) = 0;					///设置字体选择颜色
	virtual DWORD GetFontSelectColor() = 0;								///获取字体选择颜色
	virtual bool GetIsShowOutline()=0;	///是否显示描边——LCF
	virtual COLORREF GetOutlineColor()=0;	///描边颜色——LCF
	virtual void SetFontOutlineInfo(bool isOutline, COLORREF color) = 0;	 ///设置字体是否描边及描边颜色——LCF
};

interface  IInputUserName: virtual public ICombox
{
public:
	virtual void SetDeleteButtonPic(wchar_t * pchar)=0;
	virtual wchar_t * GetDeleteButtonPicPath()=0;
	virtual void SetDeleteButtonPicPos(int x,int y,int w,int h)=0;
	virtual void SetDeleteButtonW(int nw)=0;
	virtual int GetDeleteButtonW()=0;

    ///加载滚动条图片
    virtual void LoadScrollBkImage(wchar_t * pchar)=0;
    virtual void LoadScrollTButton(wchar_t * pchar)=0;
    virtual void LoadScrollBButton(wchar_t * pchar)=0;
    virtual void LoadScrollBar(wchar_t * pchar)=0;
    virtual wchar_t * GetScrollBkPath()=0;
    virtual wchar_t * GetScrollTButtonPath()=0;
    virtual wchar_t * GetScrollBButtonPath()=0;
    virtual wchar_t * GetScrollBarPath()=0;
	virtual void SetFontSelectColor(DWORD color) = 0;					///设置字体选择颜色
	virtual DWORD GetFontSelectColor() = 0;								///获取字体选择颜色
};

////////////////////////////////////////////////////////////////////////////////
///容器
// 容器的分页功能
interface IContainPaginator
{
public:
    virtual void SetIsPagiStyle(bool bIs) = 0;
    virtual bool GetIsPagiStyle() = 0;
    virtual void InsertPagecontent(IBCInterFace * pcontrol)=0;  ///< 操作内容
    virtual void InsertNormalcontent(IBCInterFace * pcontrol)=0;///< 插入辅助按钮
    virtual bool SetNumPerPage(BYTE nNum) = 0;                  ///< 设置此容器一页放多少
    virtual int GetNumPerPage() = 0;                            ///< 设置此容器一页放多少
    virtual bool NextPage() = 0;                                ///< 向下翻页
    virtual bool PrevPage() = 0;                                ///< 向上翻页
    virtual void FirstPage() = 0;                               ///< 第一页
    virtual void LastPage() = 0;                                ///< 最后一页
    virtual bool SetCurrentPage(int nCup) = 0;                  ///< 移动到某一页
    virtual int GetCurrentPage() = 0;                           ///< 当前页
    virtual int GetTotalPage() = 0;                             ///< 统计页数
    virtual void GetNoPagiID(int nIDs[], int nSize)=0;          ///< 获取不分页的ID
    virtual void SetNoPagecontentID(int nIDs[], int nSize)=0;

    /**
     * @brief 设置与各边的间隔，两个内容之间的间隔
     * @param[in]   nLSpace     与左边的间隔
     * @param[in]   nRSpace     与左边的间隔
     * @param[in]   nTSpace     与左边的间隔
     * @param[in]   nBSpace     与左边的间隔
     * @param[in]   nHBetweenSpace  两个元素内横向间隔
     * @param[in]   nVBetweenSpace  两个元素内竖向间隔
     */
    virtual void SetSpaces(int nLSpace, int nRSpace, int nTSpace, int nBSpace,
        int nHBetweenSpace, int nVBetweenSpace)=0;     
    /**
    * @brief 得到上述边距
    */
    virtual void GetSpaces(int &nLSpace, int &nRSpace, int &nTSpace, int &nBSpace,
        int &nHBetweenSpace, int &nVBetweenSpace)=0;   
};

interface  IContain: virtual public IBCInterFace
{
public:
	virtual void SetBKImagePath(wchar_t * pchar)=0;           ///<设置容器底图路径
	virtual void LoadBkImage(wchar_t * pchar)=0;              ///<加载容器底图路径
	virtual wchar_t * GetBkImagePath()=0;                     ///<获取容器底图路径	
	virtual void MoveControl(int DistanceX,int DistanceY)=0;///<移动容器，其中的所有控件也要一起移动
	virtual int GetControlSize()=0;  ///<所含控件个数
	virtual int GetControlBlongContainID(int id)=0;  ///<获取指定ID的上层容器控件ID
	virtual void IInsertControl(IBCInterFace * bControl)=0;///<向窗口中添加控件
	virtual IBCInterFace *IGetControlbyIndex(int listindex, bool &bVisable)=0;///<返回list表中指定序号的对像指针，及显示属性
	virtual void SetChildVisable(IBCInterFace *pChild, bool bVisable)=0;
	virtual void IDeleteControl(IBCInterFace * pcontrol)=0;    ///< 移除某个控件
    /** @brief 将某个控件从该容器移动到另一个容器中
     *  @param[in]          pcontrol        待移动的控件
     *  @param[in]          pOther          目的容器，如果为空，则不移动到任何容器，交由用户自己管理
     *  @return 如果要移动的控件存在于此容器中，就返回true，否则返回false
     */
    virtual bool IMoveControl(IBCInterFace * pcontrol, IContain *pOtherContain = NULL)=0;
	virtual IBCInterFace * Clone(int nId)=0;
	virtual bool GetControlShowFlag(int nIndex)=0;   ///<控件是否随容器的显示显示，因为有些控件不需要显示，需要特定时期才显示
    virtual bool GetControlShowFlag(IBCInterFace *pBcInterface)=0;

	virtual void SortAllSubControlByZord()=0;  ///<重新全体排序
	virtual void SetAllZorder(INT16 zorder)=0;///<设置控件层次 容器就将容器中的所有层次都相应的作修改，以便编辑 editor by 2010.11.12

    virtual IContainPaginator *GetContainPaginator() = 0;   ///<获取分页操作

	//virtual void SetControlShowFlag(BControlInterface * pBcInterface,bool bFlag)=0;
};

////////////////////////////////////////////////////////////////////////////////



///<静态动画
interface ICartoon:virtual public IBCInterFace
{
public:
	virtual void SetFrameNum(byte framenum)=0;///<设置播放帧数
	//virtual void SetVisble(bool bflag)=0;///<设置是否显示动画
	virtual void SetSpeed(int speed)=0;  ///<设置动画速度
	virtual void SetState(PLAYSTATE bstate)=0; ///<设置动画状态
	virtual void SetPlayNum(int iplaynum)=0; ///<设置播放次数
	virtual byte GetFrameNum()=0;             ///<获取帧数
	//virtual bool GetVisble()=0;              ///<获取控件是否显示
	virtual int GetSpeed()=0;                ///<获取播放速度
	virtual PLAYSTATE GetState()=0;                ///<获取播放状态
	virtual int GetPlayNum()=0;             ///<获取播放次数
	virtual byte GetCurrentFrame()=0;  ///<获取当前播放的帧索引
	virtual bool SetImagePath(int frameindex,wchar_t * path)=0;    ///<设置相应帧的路径
	virtual bool LoadPic(int frameindex,wchar_t * path)=0;         ///<加载所有帧，存入m_vImage
	virtual void SetPicABSXY(int frameindex,int x,int y,int w,int h)=0;     ///<设置每帧的坐标和高宽
	virtual wchar_t * GetPicPath(int frameindex)=0;///<获取每帧路径

	virtual IBCInterFace * Clone(int nId)=0;
};

///进度条
interface  IProgress: virtual public IBCInterFace
{
public:
	///用户接口
	virtual void SetMaxValue(int MaxValue)=0;///<设置进度条的最大的显示值
	virtual void SetCurValue(int CurValue)=0;///<设置进度条现在的值
	virtual int GetMaxValue()=0;             ///<获取进度条的最在显示值
	virtual int GetCurValue()=0;             ///<获取进度条现在的显示值
	virtual void LoadBkImage(wchar_t * pchar)=0;            ///<加载底图
 	virtual void SetBkPath(wchar_t * pchar)=0;              ///<设置底图路径
	virtual void LoadProgressImage(wchar_t * pchar)=0;      ///<加载进度显示
	virtual void SetProgressImage(wchar_t * pchar)=0;       ///<设置进度显示路径
	virtual wchar_t * GetBkImagePath()=0;                   ///<获取底图路径
	virtual wchar_t * GetProgressImagePath()=0;             ///<获取进度条显示路径
	virtual IBCInterFace * Clone(int nId)=0;
};

interface  IScrollBar:virtual public IBCInterFace
{
public:
	///用户接口
	virtual void SetMaxValue(float maxValue)=0;         ///<设置其最大值
	virtual float GetMaxValue()=0;                      ///<获取最大值

	virtual void SetCurValue(float curValue)=0;         ///<设置当前值
	virtual float GetCurValue()=0;                      ///<获取当前值
	virtual void LoadBkImage(wchar_t * pchar)=0;      ///<加载底图
	virtual wchar_t * GetBkImagePath()=0;                 ///<获取底图路径
	virtual void LoadTButton(wchar_t * pchar)=0;      ///<加载顶端按扭的图
	virtual wchar_t * GetTButtonPath()=0;                 ///<获取顶端按扭图片路径
	virtual void LoadBButton(wchar_t * pchar)=0;      ///<加载底端按扭图片
	virtual wchar_t * GetBButtonPath()=0;                 ///<获取底端按扭图片
	virtual void LoadScrollButton(wchar_t * pchar)=0; ///<加载中间滚轮的图片
	virtual wchar_t * GetScrollButtonPath()=0;            ///<获取中间滚轮的图片路径
	virtual void SetBkImagePath(wchar_t * pchar)=0;///<设置底图路径
	virtual void SetTButtonPath(wchar_t * pchar)=0;///<设置底图路径
	virtual void SetBButtonPath(wchar_t * pchar)=0;///<设置底图路径
	virtual void SetScrollButtonPath(wchar_t * pchar)=0;///<设置底图路径
	virtual void SetVerticalFlag(bool flag)=0;         ///<设置是横向滚动条这是竖向滚动条
	virtual bool GetVerticalFlag()=0;                  ///<获取滚动条方向标志
	virtual IBCInterFace * Clone(int nId)=0;
};

///IMAGE移动动画
interface  IImageFlash: virtual public IBCInterFace
{
public:
	virtual void SetPlayNum(int num)=0;                         ///<设置播放次数
	virtual int GetPlayNum()=0;                                 ///<获取播放次数
	virtual void SetPlayState(bool flag)=0;                   ///<设置是否开始播放
	virtual bool GetPlayState()=0;                              ///<获取播放状态

	virtual void SetStartP(POINT point)=0;                                 ///<设置开始X
	virtual void SetEnd(POINT point)=0;                                   ///<设置终点X
	virtual void SetControlPoint(POINT point)=0;                ///<设置中间控制点坐标
	virtual POINT GetControlPoint()=0;                          ///<获取中间控制点坐标
	virtual POINT GetStartP()=0;                                  ///<获取开始x
	virtual POINT GetEnd()=0;                                    ///<获取终点X

	virtual void SetPicPath(int nIndex,wchar_t * pchar)=0;            ///<设置动画图片路径
	//virtual void LoadPic(wchar_t * pchar)=0;               ///<加载动画图片
	virtual wchar_t * GetPicPath(int nIndex)=0;                      ///<获取动画图片的路径

	virtual void SetMoveTotalTime(int atime)=0;                                  ///<设置移动时间
	virtual int GetMoveTotalTime()=0;                                   ///<获取移动时间
	virtual void SetLineKind(bool flag)=0;                      ///<设置是否是直线类型
	virtual bool GetLineKind()=0;                               ///<获取是否是直线类型

	virtual void SetMoveFlag(bool bFlag)=0;  ///<设置是否是移动动画
	virtual bool GetMoveFlag()=0;  ///<获取是否是移动动画

	virtual void SetFrameNum(int nNum)=0;       ///<设置动画帧数
	virtual int GetFrameNum()=0;              ///<获取动画帧数

    virtual void SetPlayTotalTime(int atime)=0;      ///<设置播放总时间
	virtual int GetPlayTotalTime()=0;

	virtual void ClearAllPath()=0;        ///<清空里面的所有图片路径
	virtual IBCInterFace * Clone(int nId)=0;
};

interface IWeb:virtual public IBCInterFace, virtual public IManual
{
public:
	///<基本方法
	virtual void SetShowWebAddress(wchar_t * pchar)=0;
    /// 使用窗口
    /// 使用窗口即将网页控件以子窗口的形式弹出，支持各种输入操作
    virtual void SetUseWindow(bool bUse = true) = 0;
};



interface IListControl:virtual public IBCInterFace
{
public:
	///<基本方法
	virtual void SetRowHeight(int nHeight)=0;                  ///<设置每行的高度
	virtual void SetImageWH(int nWidth,int nHeight)=0;                      ///<设置每个对像中，图片的宽度
	virtual void SetMinColWidth(int nWidth)=0;                    ///<设置每列的最小宽度
	virtual void InserHeadNode(int nindex)=0; ///<添加表头节点
	virtual void SetHeadWH(int nIndex,int nWidth,int nHeight)=0;///<设置表头对像的宽高
	virtual void SetHeadSpaceW(int nWidth)=0;                   ///<设置表头间隔图片的宽

	virtual void InsertObjNode(int nRowIndwx,int nColIndex)=0;           ///<插入一个对像 ，只能在行中插入
	virtual void SetRowFontText(int nRowIndwx,int nColIndex,wchar_t * pFontStyle)=0;///<设置每行每列的显示的内容

	virtual void SetObjImagePath(int nObjRowIndex,int nColIndex,int nImageIndex,wchar_t * path)=0;  ///<设置对像中相应IMAGE控件的图片路径
	//virtual void LoadObjImage(int nObjRowIndex,int nColIndex,int nImageIndex,wchar_t *szFileName)=0;///<为对像中相应IMAGE控件加载图片
	
	virtual void LoadBkImage(wchar_t * Path)=0;     ///<加载控件底图 
	virtual wchar_t * GetBkImage()=0;               ///<获得控件底图
	
	virtual void InsertColumn(int nIndex,wchar_t * pText,int nWidth,int nHeight)=0;   ///<添加列头
	virtual void InsertInItem(int nRow,int nCol,wchar_t * pText)=0;                   ///<插入节点
	///对表头操作的方法
	virtual void SetFontStyle(wchar_t * pFontStyle)=0;  ///<设置表头字体样式
	virtual void SetFontSize(int fontsize)=0;       ///<设置表头字体大小
	virtual void SetHeadFontText(int nIndex,wchar_t * pFontStyle)=0;///<设置表头显示的内容

	virtual wchar_t * GetFontStyle()=0;                       
	virtual int  GetFontSize()=0;  ///<获取字体大小
	virtual COLORREF GetFontColor()=0;               ///<获取控件字体显示颜色
	virtual void SetFontColor(COLORREF crFontColor)=0; ///<设置控件字体显示颜色
	
	virtual void LoadHeadBkImage(wchar_t * path)=0;     ///<加载表头底图
	virtual void LoadHeadSpaceImage(wchar_t * path)=0;///<加载表头间隔图片

	virtual wchar_t * GetHeadBkImagePath()=0;           ///<获取表头图片路径
	virtual wchar_t * GetHeadSpaceImagePath()=0;           ///<获取表头图片路径

	virtual void SetHaveScroll(bool bFalg)=0;   ///<设置是否永远有滚动条
	virtual bool GetHaveScroll()=0;             ///<获取是否是永远有滚动条属性

	//virtual void LoadScrollBkImage(wchar_t * path)=0;     ///<滚轮底图
	//virtual wchar_t * GetScrollBkImage()=0;

	//virtual void LoadScrollTImage(wchar_t * path)=0;   ///<上面或左面的按扭图片
	//virtual wchar_t * GetScrollTImage()=0;

	//virtual void LoadScrollBImage(wchar_t * path)=0;   ///<下面或右面的按扭图片
	//virtual wchar_t * GetScrollBImage()=0;

	//virtual void LoadScrollBarImage(wchar_t * path)=0;  ///<中间滚轮的图片
	//virtual wchar_t * GetScrollBarImage()=0;

	virtual void Clear()=0;               ///<清空所有行，但不清空表头

	virtual void LoadVerScrollBkImage(wchar_t * path)=0;
	virtual wchar_t * GetVerScrollBkImage()=0;
	virtual void LoadVerScrollTImage(wchar_t * path)=0;
	virtual wchar_t * GetVerScrollTImage()=0;
	virtual void LoadVerScrollBImage(wchar_t * path)=0;
	virtual wchar_t * GetVerScrollBImage()=0;
	virtual void LoadVerScrollBarImage(wchar_t * path)=0;
	virtual wchar_t * GetVerScrollBarImage()=0;
	//横的
	virtual void LoadHorizScrollBkImage(wchar_t * path)=0;
	virtual wchar_t * GetHorizScrollBkImage()=0;
	virtual void LoadHorizScrollTImage(wchar_t * path)=0;
	virtual wchar_t * GetHorizScrollTImage()=0;
	virtual void LoadHorizScrollBImage(wchar_t * path)=0;
	virtual wchar_t * GetHorizScrollBImage()=0;
	virtual void LoadHorizScrollBarImage(wchar_t * path)=0;
	virtual wchar_t * GetHorizScrollBarImage()=0;

	virtual void SetHeadObjHeight(int nH)=0;  ///<设置表头对像的高
	virtual int GetHeadObjHeight()=0;  ///<获取表头对像的高度

	virtual void SetSelectRectColor(COLORREF crSelect)=0;            ///<设置选中行的颜色
	virtual COLORREF GetSelectRectColor()=0; ///<返回选中行的颜色
	virtual IBCInterFace * Clone(int nId)=0;
	virtual bool GetIsShowOutline()=0;	///是否显示描边——LCF
	virtual COLORREF GetOutlineColor()=0;	///描边颜色——LCF
	virtual void SetFontOutlineInfo(bool isOutline, COLORREF color) = 0;	 ///设置字体是否描边及描边颜色——LCF
};

interface ITalk:virtual public IBCInterFace
{
	virtual  void SetText(wchar_t *  wchar)=0; ///<设置显示的文字

	virtual void LoadScrollBkImage(wchar_t * pchar)=0;
	virtual void LoadScrollTButton(wchar_t * pchar)=0;
	virtual void LoadScrollBButton(wchar_t * pchar)=0;
	virtual void LoadScrollBar(wchar_t * pchar)=0;

	virtual wchar_t * GetScrollBkPath()=0;
	virtual wchar_t * GetScrollTButtonPath()=0;
	virtual wchar_t * GetScrollBButtonPath()=0;
	virtual wchar_t * GetScrollBarPath()=0;

	virtual void LoadBkPath(wchar_t * pchar)=0;
	virtual wchar_t * GetBkPath()=0;
	virtual IBCInterFace * Clone(int nId)=0;

	virtual void SetFaceImagePath(wchar_t * pchar)=0;  ///<加载表情图片
	virtual wchar_t * GetFaceImagePath()=0;
};


interface IBeacon:virtual public IBCInterFace
{

};

interface IRadioButton:virtual public IBCInterFace
{
public:
	virtual void SetBkImagePath(wchar_t * pchar)=0;          ///<加载底图路径
	virtual void LoadBkImage(wchar_t * pchar)=0;             ///<加载底图
	virtual void SetSelectPath(wchar_t * pchar)=0;           ///<记录选择框的路径
	virtual void LoadSelectImage(wchar_t * pchar)=0;         ///<加载选择框的图片
	virtual wchar_t * GetBkImage()=0;                        ///<获取底图路径
	virtual wchar_t * GetSelectImage()=0;                    ///<获取选择框路径

	virtual void SetText(wchar_t * pchar)=0;                 ///<设置显示字符
	virtual wchar_t * GetText()=0;                           ///<获取显示字符

	virtual void SetTextFontStyle(wchar_t * pchar)=0;        ///<设置显示字体类型
	virtual wchar_t  * GetTextFontStyle()=0;                 ///<获取显示字体类型

	virtual void SetTextFontSize(int nSize)=0;               ///<设置显示字体大小
	virtual int GetTextFontSize()=0;                         ///<获取显示字体大小

	virtual void SetTextFontColor(COLORREF crColor)=0;       ///<设置字体颜色
	virtual COLORREF GetTextFontColor()=0;                   ///<获取字体颜色
	
	virtual void SetBkWith(int w)=0;                         ///<设置底图宽度
	virtual int GetBkWith()=0;                               ///<获取底图宽度

	virtual bool GetIsSelect()=0;                             ///<获取是否是选中了
	virtual IBCInterFace * Clone(int nId)=0;
};

interface ICardBase:virtual public IBCInterFace
{
	virtual void SetCardWH(int nW,int nH)=0;  ///<设置牌的宽高
	virtual void SetCardHSpace(int nSpace)=0;  ///<设置牌的水平间距
	virtual void GetCardWH(int & nW,int & nH)=0; ///<获取牌的宽高
	virtual int GetCardHSpace()=0;           ///<获取牌的水平间距
	virtual void SetCardFlushStyle(int nIndex)=0; ///<设置排序方式
	virtual int GetCardFlushStyle()=0;        ///<获取排序方式
	virtual void SetCardList(BYTE BList[] ,int nNum)=0;     ///<设置显示的牌
	virtual void SetCardPic(BYTE nIndex,wchar_t * pchar)=0;  ///<设置牌的资源.顺便加载进去
	virtual int GetCardList(BYTE BCardList[])=0;             ///<获取显示的牌 
	virtual wchar_t * GetCardPic(int nIndex)=0;              ///<获取牌资的路径
	virtual int GetCardNum()=0;                          ///<获取牌的数量

	virtual void SetCardAlignStyle(BYTE bStyle)=0;    ///<设置对齐模式 0-2  0表示左对齐，1表示居中，2表示右对齐
	virtual BYTE GetCardAlignStyle()=0;
	virtual IBCInterFace * Clone(int nId)=0;

	//virtual ENM_CARD_NUMBER GetCardPointByIndex(int nIndex)=0;           ///<获取指定索引的牌的点数
	//virtual ENM_CARD_SHAPE GetCardFigureByIndex(int nIndex)=0;          ///<获取指定索引的牌的花色
	//virtual void RemoveCard(BYTE BCardList[],int nNum)=0;                   ///<移除其中的牌
};

interface INoMoveCard:virtual public ICardBase
{
	virtual void SetIsShowCardNum(bool bFlag)=0;      ///<设置是否显示剩余牌的数量
	virtual bool GetIsShowCardNum()=0;                ///<获取是否显示牌的数量
	virtual void SetCardShowDir(int cardDir)=0;       ///<设置牌的显示方向
	virtual int GetCardShowDir()=0;                   ///<获取牌的显示方向
    virtual void SetCardShowMode(bool bVertical) = 0; ///<牌的摆放方式 true: 竖放,  false:横放
    virtual bool GetCardShowMode() = 0;               ///<获取牌的摆放方式 true: 竖放,  false:横放
	virtual IBCInterFace * Clone(int nId)=0;

};

interface ILightCard:virtual public INoMoveCard
{
	virtual void SetShowCardWH(int nW,int nH)=0;
	virtual void GetShowCardWH(int & nW,int & nH)=0;
	virtual void SetShowCardRowNum(int nNum)=0;
	virtual int  GetShowCardRowNum()=0;
	virtual void SetRowSpace(int nSpace)=0;
	virtual int  GetRowSpace()=0;
	virtual IBCInterFace * Clone(int nId)=0;
};

interface INum:virtual public IBCInterFace
{
	virtual void SetNum(__int64 nNum)=0;    ///<设置数字
	virtual __int64  GetNum()=0;            ///<获取数字
    /** @brief 数字图片是否有符号
     *  有符号图片格式为 +0123456789/-0123456789，
     *  注意：只有一个正号或一个负号，设置数字时仍然不传正负，默认显示0前面的符号
     */
    virtual void SetHaveSymbol(bool bHave)=0;    
    virtual bool GetHaveSymbol()=0;
	virtual void SetImagePath(wchar_t * pchar)=0;
	virtual wchar_t * GetImagePath()=0;
};
interface ITime:virtual public IBCInterFace
{
	virtual void SetImageNumPath(wchar_t * pchar)=0; ///<设置数字图片
	virtual wchar_t * GetImageNumPath()=0;     ///<获取数字图片
	virtual void SetBkImagePath(wchar_t * pchar)=0;   ///<设置底色图片
	virtual wchar_t * GetBkImagePath()=0;            ///<获取底色图片
	virtual void SetShowMaxNum(int nNum)=0;         ///<设置显示最大时间
	virtual int  GetShowMaxNum()=0;                 ///<获取显示最大时间
	virtual void SetShowMinNum(int nNum)=0;        ///<设置显示最小时间 
	virtual int  GetShowMinNum()=0;              ///<获取显示最小时间
	virtual void SetShowAllTime(DWORD dtTime)=0; ///<设置显示总时间
	virtual DWORD GetShowAllTime()=0;    ///<获取显示总时间
	virtual int  GetCurrentTimeNum()=0;     ///<获取当前时间
	virtual void SetPLay(bool bFlag)=0;
	virtual bool GetPLay()=0;
};
interface IOperateCard:virtual public ICardBase
{
	virtual void SetUpSpace(int nSpace)=0; ///<设置牌升起的高度
	virtual int  GetUpSpace()=0;           ///<获取牌升起的高度

	virtual void SetSelectImagePath(wchar_t * pchar)=0; ///<设置选择标记的图片
	virtual wchar_t * GetSelectImagePath()=0;           ///<获取选择标记的图片路径
	virtual IBCInterFace * Clone(int nId)=0;
};
interface ISlider:virtual public IBCInterFace
{
	virtual void SetFillPath(wchar_t * pchar)=0;
	virtual wchar_t * GetFillPath()=0;
	virtual void SetButtonSliderPath(wchar_t * pchar)=0;
	virtual wchar_t * GetButtonSliderPath()=0;
	virtual void SetSliderStyle(BYTE bStyle)=0;
	virtual BYTE GetSliderStyle()=0;
	virtual IBCInterFace * Clone(int nId)=0;
};
interface IMenu:virtual public IBCInterFace
{
	virtual void SetItemHeight(int nH)=0;
	virtual int GetItemHeight()=0;
	virtual void SetLineColor(COLORREF crColor)=0;
	virtual COLORREF GetLineColor()=0;

	virtual void SetBKImagePath(wchar_t * pchar)=0;;
	virtual wchar_t * GetBkImagePath()=0;

	virtual void SetMoveColor(COLORREF crColor)=0;
	virtual COLORREF GetMoveColor()=0;

	virtual void SetMoveFramColor(COLORREF crColor)=0;
	virtual COLORREF GetMoveFramColor()=0;

	virtual void SetFontStyle(wchar_t * pchar)=0;
	virtual wchar_t * GetFontStyle()=0;                 ///<获取控件字体样式
	virtual void SetFontSize(int nSize)=0;
	virtual int GetFontSize()=0;                    ///<获取控件字体大小
	virtual void SetFontColor(COLORREF crColor)=0;
	virtual COLORREF GetFontColor()=0;               ///<获取控件字体显示颜色
	virtual void SetFontShowStyle(FONTSHOWSTYLE style)=0;
	virtual FONTSHOWSTYLE   GetFontShowStyle()=0;;             ///<获取控件字体显示模式，居左，居中，居右
	virtual bool GetIsShowOutline()=0;	///是否显示描边——LCF
	virtual COLORREF GetOutlineColor()=0;	///描边颜色——LCF
	virtual void SetFontOutlineInfo(bool isOutline, COLORREF color) = 0;	 ///设置字体是否描边及描边颜色——LCF
};

struct SudokuInfo_1;
interface ISudoku:virtual public IBCInterFace
{	
    virtual SudokuInfo_1 *GetSudokuAttribute()=0;                ///< 获取sudoku控件属性
    virtual int UpdateAttribute()=0;                             ///< 更新sudoku控件属性
	virtual int SetPicByIndex(int nIdx,wchar_t *szFileName,RECT& rc) = 0; ///<设置九宫阵图
};

interface ISiTuanOperateCard:virtual public IOperateCard
{
	virtual void SetFoldSpace(int nSpace)=0; ///<设置重叠牌的间隔
	virtual void SetThreeSpace(int nSpace)=0; ///<设置三张牌或两张牌间隔
	virtual void SetFoldToOtherSpace(int nSpace)=0; ///<设置重叠牌到其它牌之间的间隔
	virtual void SetThreeToOtherSpace(int nSpace)=0; ///<设置三张牌到其它牌之间的间隔
	virtual void SetTwoToOtherSpace(int nSpace)=0; ///<设置两张牌到其它牌之间的间隔
	virtual void SetThreeToThreeSpace(int nSpace)=0;        ///<设置一张牌之间的间隔

	virtual int GetFoldSpace()=0; ///<设置重叠牌的间隔
	virtual int GetThreeSpace()=0; ///<设置三张牌或两张牌间隔
	virtual int GetFoldToOtherSpace()=0; ///<设置重叠牌到其它牌之间的间隔
	virtual int GetThreeToOtherSpace()=0; ///<设置三张牌到其它牌之间的间隔
	virtual int GetTwoToOtherSpace()=0; ///<设置两张牌到其它牌之间的间隔
	virtual int GetThreeToThreeSpace()=0;        ///<设置一张牌之间的间隔
	virtual void SetNumPic(wchar_t * Pchar)=0; ///<设置数字图片的路径
	virtual wchar_t * GetNumPic()=0;///<获取数字图片的路径
};
interface ISiTuanNomoveCard:virtual public INoMoveCard
{
	virtual void SetNumPic(wchar_t * pchar )=0; ///<设置数字控件的图片路径
	virtual wchar_t * GetNumPic()=0;            ///<获取数字控件的图片路径
};
interface ISiTuanLightCard:virtual public ILightCard
{
	virtual void SetNumPic(wchar_t * pchar)=0; ///<设置数字图片的路径
	virtual wchar_t *GetNumPic()=0;  ///<获取数字图片的路径
	virtual void SetFoldSpace(int nSpace)=0;
	virtual void SetFoldToOtherSpace(int nSpace)=0;
	virtual int GetFoldSpace()=0;
	virtual int GetFoldToOtherSpace()=0;
};


///IMAGE移动动画
interface  IExAnimate: virtual public IBCInterFace
{
public:
    virtual void SetPlayNum(int num)=0;                         // 设置播放次数
    virtual int GetPlayNum()=0;                                 // 获取播放次数
    virtual void SetPlayState(bool flag)=0;                     // 设置是否开始播放
    virtual bool GetPlayState()=0;                              // 获取播放状态

    virtual void SetStartP(POINT point)=0;                      // 设置开始X
    virtual void SetEnd(POINT point)=0;                         // 设置终点X
    virtual void SetControlPoint(POINT point)=0;                // 设置中间控制点坐标
    virtual POINT GetControlPoint()=0;                          // 获取中间控制点坐标
    virtual POINT GetStartP()=0;                                // 获取开始x
    virtual POINT GetEnd()=0;                                   // 获取终点X

    virtual void SetPicPath(int nIndex,wchar_t * pchar)=0;            ///<设置动画图片路径(相对路径)    
    virtual void SetAbsPicPath(int nIndex,wchar_t * pchar)=0;         ///<设置动画图片路径(绝对路径)  
    virtual wchar_t * GetPicPath(int nIndex)=0;                 // 获取动画图片的路径
    virtual void ClearAllPath()=0;                              // 清空里面的所有图片路径

    virtual void SetMoveTotalTime(int atime)=0;                 // 设置移动时间
    virtual int GetMoveTotalTime()=0;                           // 获取移动时间


    virtual void SetFrameNum(int nNum)=0;                       // 设置动画帧数
    virtual int GetFrameNum()=0;                                // 获取动画帧数

    virtual void SetPlayTotalTime(int atime)=0;                 // 设置播放总时间
    virtual int GetPlayTotalTime()=0;

    
    virtual IBCInterFace * Clone(int nId)=0;

    virtual const wchar_t * GetExtFilePath() = 0;
    virtual void SetExtFilePath(wchar_t *pwszFileName) = 0;
    virtual bool LoadFromFile() = 0;
    virtual bool WriteToFile() = 0;

    virtual void SetCurveInstance(CURVE_TYPE curType) = 0;      // 设置曲线类型
    virtual CURVE_TYPE GetCurveType() = 0;                      // 获得曲线类型
    virtual void SetIsSymmetrical(bool bIsSymmetrical) = 0;     // 是否对称
    virtual bool GetIsSymmetrical() = 0;                        // 是否对称
 

    virtual void SetUseMouse(bool bFlag) = 0;

    virtual void SetShowCurveLine(bool bFlag) = 0;   
    virtual bool GetIsShowCurveLine() = 0;

    virtual void SetIsLoop(bool bIsLoop = true) = 0;            // 是否循环播放
    virtual bool GetIsLoop() = 0;                               // 是否循环播放
    virtual void SetShowFrame(int nCount) = 0;                  // 显示某一帧

    /** @brief 设置是否左右镜像显示
     *	@param[in]      bCrossMirror
     *	@return 无
     */
    virtual void SetCrossMirror(bool bCrossMirror) = 0;
};


////////////////////////////////////////////////////////////////////////////////
// 麻将控件接口begin{{

const int MAX_TILES_INCTRL = 200;

const int MAX_GOD_TILE = 8;
const int MAX_GIVE_TILE = 100;

const int MAX_HAND_TILE = 20;

const int MAX_BLOCK_COUNT = 10;

// UI表现类型 
enum MJ_UI_TYPE
{
    UT_NONE=-1          ,   // 空牌（没牌）（不画）
    UT_SELF=0           ,   // 自己打出的小牌
    UT_FOLLOW=1         ,   // 下家放倒牌
    UT_OPPOSITE=2       ,   // 对家放倒牌
    UT_AHEAD=3          ,   // 上家放倒牌
    UT_WALL_VERL=4      ,   // 竖的左城墙
    UT_WALL_VERR=5      ,   // 竖的右城墙
    UT_WALL_NEAR=6      ,   // 横的近城墙
    UT_WALL_FAR=7       ,   // 横的远城墙
    UT_FOLLOW_BACK=8    ,   // 下家的牌背
    UT_OPPOSITE_BACK=9  ,   // 对家的牌背
    UT_AHEAD_BACK=10    ,   // 上家的牌背
    UT_SELF_LAY=11      ,   // 自己放倒的大牌
    UT_FOLLOW_LAY=12    ,
    UT_OPPOSITE_LAY=13  ,
    UT_AHEAD_LAY=14     ,
    UT_SELF_STAND=15    ,   // 自己站立的牌
    
};

enum MJ_LOGIC_TYPE
{
    LT_EMPTY               ,    // 没有行为（正常手牌）
    LT_COLLECT             ,    // 吃牌
    LT_TRIPLET             ,    // 碰
    LT_QUADRUPLET          ,    // 杠
    LT_QUADRUPLET_CONCEALED,    // 暗杠
    LT_QUADRUPLET_REVEALED ,    // 明杠
    LT_QUANDUPLET_SPECIAL  ,    // 长春麻将特殊杠，四个排一起的
    LT_WIN                 ,    // 胡牌
    LT_TING                ,    // 听牌
    LT_FLOWER              ,    // 补花
    LT_GIVE                     // 要出的牌（最右边的一张）
};


/**
* @brief 麻将控件支持的数据结构
* 注意：
* 作为手牌时一个MJ_TILE_UIINFO只表示一个麻将子, pucTiles只有一个有效
* 作为拦牌时一个MJ_TILE_UIINFO表示一组拦牌，pucTiles可以有多个
*/	
struct MJ_TILE_UIINFO
{
    BYTE            ucTiles[4]; /// 麻将牌值 1~9万 11~29条 21~29筒 31~37东南西北中发白 41~48春夏秋冬梅兰竹菊  

    BOOL            bSelect;    /// 是否被点击（是否弹起）
    MJ_LOGIC_TYPE   ucActFlag;  /// 标记（记录吃碰杠的牌用）
    MJ_UI_TYPE      ucUiType;   /// 类型（见DT_XXXXX）
    BYTE            ucChair;    /// 吃碰杠谁的
    DWORD           dwReserve1; /// 已占用位	0位：0:不能选的牌，1:可以选的牌     
    DWORD           dwReserve2; /// 已占用位    0位：0:普通牌    ，1:万能牌

    MJ_TILE_UIINFO()
    {
        memset(this, 0, sizeof(MJ_TILE_UIINFO));
    }
};


interface IMjCtrl : virtual public IBCInterFace
{
public:
    /**
     * @brief 设置牌信息
     * 注意：
     * 作为手牌时一个 MJ_TILE_UIINFO只表示一个麻将子, pucTiles只有一个有效
     * 作为拦牌时一个MJ_TILE_UIINFO表示一组拦牌，pucTiles可以有多个
     */
    virtual bool SetTilesInfo(MJ_TILE_UIINFO pTilesUIInfo[], int nLen) = 0;
    /**
     * @brief 设置麻将图片资源
     * 目前只有一张
     */
    virtual void SetMjPic(const wchar_t * pchar) = 0;
    /**
    * @brief 设置麻将子宽
    */
    virtual void SetMjTileWidth(int nWidth) = 0;
    /**
    * @brief 设置麻将子高
    */
    virtual void SetMjTileHeight(int nHeight) = 0;
    /**
     * @brief 设置是否使用鼠标
     */
    virtual void SetIsUseMouse(bool bUse = true) = 0;

    /// 对应的get方法
    virtual bool GetTilesInfo(MJ_TILE_UIINFO pTilesUIInfo[], int nSrcLen) = 0;
    virtual int GetTileCount() = 0;
    virtual bool GetMjPic(wchar_t *pszPicpath, int nSrcSize) = 0;
    virtual int GetMjTileWidth() = 0;
    virtual int GetMjTileHeight() = 0;
    virtual bool GetIsUseMouse() = 0;
};

/////////////////////////////////////////////梭哈控件
interface IShCtrl : virtual public IBCInterFace
{
public:
    /**
    * @brief 设置梭哈图片资源
    * 目前只有一张
    */
    virtual void SetShPic(const wchar_t * pchar) = 0;
    /**
    * @brief 设置梭哈子宽
    */
    virtual void SetShTileWidth(int nWidth) = 0;
    /**
    * @brief 设置梭哈子高
    */
    virtual void SetShTileHeight(int nHeight) = 0;
    /**
    * @brief 设置是否使用鼠标
    */
    virtual void SetIsUseMouse(bool bUse = true) = 0;
	/**
	/* @brief 显示梭哈筹码值
	*/
	virtual void SetShNote(int iNote) =0;
	/**
	/* @brief 显示梭哈排列方式
	*/
	virtual void SetShNoteType(int iNoteType)=0;

    /// 对应的get方法
    virtual int GetTileCount() = 0;
    virtual bool GetShPic(wchar_t *pszPicpath, int nSrcSize) = 0;
    virtual int GetShTileWidth() = 0;
    virtual int GetShTileHeight() = 0;
    virtual bool GetIsUseMouse() = 0;
	virtual int GetShNote()=0;//获得显示梭哈筹码值
	virtual int GetShNoteType()=0;//获得筹码摆放方式

    /** @brief 切换筹码显示方式
     *  @param[in]      nTrimNote       不为0则为整齐模式
     */
    virtual void SetTrimNoteMode(int nTrimNote)=0;    
    /** @brief 获取整齐方式
     */
    virtual int GetTrimNote()=0;
};
/////////////////////////////////////////////梭哈控件

interface IMjHandCtrl : virtual public IMjCtrl
{
public:
    /**
     * @brief 设置手牌
     */
    virtual void SetHandInfo(int nHandTiles[], int nCount) = 0;
    /**
     * @brief 设置万能牌
     */
    virtual void SetGodTile(int nGodTiles[], int nCount) = 0;    
    /**
     * @brief 设置视图方位
     */
    virtual void SetViewDirection(BYTE nViewStation) = 0;
    /**
     * @brief 得到弹起的牌
     */
    virtual int GetUpTile() = 0;    
    /**
     * @brief 设置哪几张牌不能选
     */
    virtual void SetInvalidTile(int nInvalidTiles[], int nCount) = 0;
    /**
     * @brief 设置是否最右边一张分开
     */
    virtual void SetGiveTileMode(bool bIsGiving = false) = 0;
    /**
    * @brief 设置是否躺下
    */
    virtual void SetIsLaydown(bool bIsLaydown = false) = 0;
    /**
    * @brief 增加一组拦牌信息
    */
    virtual void AddOneBlock(MJ_LOGIC_TYPE type, BYTE ucTiles[4], BYTE ucChair) = 0;
    /**
    * @brief 增加一组特殊杠
    */
    virtual void AddOneSpecQuad(BYTE ucTiles[4], BYTE ucCount[4]) = 0;
    /**
    * @brief 清除所有拦牌显示
    */
    virtual void ClearAllBlock() = 0;

    /**
     * @brief 得到手牌数据
     */
    virtual void GetHandInfo(int nHandTiles[], int nCount) = 0;
    /**
    * @brief 得到方位
    */
    virtual BYTE GetViewDirection() = 0;

    /**
     * @brief 得到某张手牌的绘制矩形
     */
    virtual RECT GetTileRectByIndex(int nIndex) = 0;
};

interface IMjGiveCtrl : virtual public IMjCtrl
{
public:
    /**
    * @brief 设置出牌
    */
    virtual void SetGiveInfo(int nHandTiles[], int nCount) = 0;
    /**
    * @brief 设置万能牌
    */
    virtual void SetGodTile(int nGodTiles[], int nCount) = 0;    
    /**
    * @brief 设置视图方位
    */
    virtual void SetViewDirection(BYTE nViewStation) = 0;
	/**
	* @brief 设置每行显示张数, 超过行数范围换到下一行显示
	*/
	virtual void SetGiveLineCnt(int nCount) = 0;

    /**
    * @brief 得到某张麻将子的绘制矩形
    */
    virtual RECT GetTileRectByIndex(int nIndex) = 0;

    /**
    * @brief 得到手牌数据
    */
    virtual void GetGiveInfo(int nHandTiles[], int nCount) = 0;
    /**
    * @brief 得到方位
    */
    virtual BYTE GetViewDirection() = 0;
	/**
	* @brief 获取每行显示张数
	*/
	virtual int GetGiveLineCnt() = 0;
};

interface IMjWallCtrl : virtual public IMjCtrl
{
public:
    /**
    * @brief 以牌背填充牌墙
    * @param[in]        nCount  填几张    
    */
    virtual void FillWallBack(int nCount) = 0;  
    /**
     * @brief 设置牌墙上某张显示为什么牌
     * @param[in]       nIndex  从该座位的玩家看起，0表右边第一墩下面一张 1表右边第一墩上面一张 2表右边第二墩下面一张 。。。。。
     * @param[in]       nTile   -1为不显示
     */
    virtual void SetWallTile(int nTile, int nIndex) = 0;
    /**
    * @brief 设置视图方位
    */
    virtual void SetViewDirection(BYTE nViewStation) = 0;
    /**
    * @brief 得到方位
    */
    virtual BYTE GetViewDirection() = 0;
    /**
     * @brief 得到某张麻将子的绘制矩形
     * @param[in] nIndex  从该座位的玩家看起，0表右边第一墩下面一张 1表右边第一墩上面一张 2表右边第二墩下面一张 。。。。。
     */
    virtual RECT GetTileRectByIndex(int nIndex) = 0;
};


interface IMjBlockCtrl : virtual public IMjCtrl
{
    // 接口废弃不用，与IMjHandCtrl合并了
public:
    /**
     * @brief 增加一组拦牌信息
     */
    virtual void AddOneBlock(MJ_LOGIC_TYPE type, BYTE ucTiles[4], BYTE ucChair) = 0;
    /**
     * @brief 清除所有拦牌显示
     */
    virtual void ClearAllDis() = 0;
    /**
    * @brief 设置视图方位
    */
    virtual void SetViewDirection(BYTE nViewStation) = 0;

    /**
    * @brief 得到方位
    */
    virtual BYTE GetViewDirection() = 0;
    /**
     * @brief 得到所有拦牌信息
     */
    virtual void GetAllBlockInfo(MJ_LOGIC_TYPE mtType[], BYTE ucTiles[][4], BYTE ucChairs[], BYTE size) = 0;
};

// 麻将控件接口end}}
////////////////////////////////////////////////////////////////////////////////


interface IMoveAction : virtual public IBCInterFace
{
public:
    /** @brief 设置要控制的控件ID
    *	@param[in]      nID   要控制的控件ID号
    *	@return 布尔类型
    */
    virtual bool SetControlingCtrlID(int nID) = 0;
    /** @name 设置座标
    */
    //@{
    virtual void SetStartP(POINT point) = 0;
    virtual void SetEndP(POINT point) = 0;
    virtual void SetControlP(POINT point) = 0;
    //@}    
    /** @brief 设置曲线类型
    *	@param[in]      curType         曲线类型，具体参见enum CURVE_TYPE(IUserInterface.h)
    *	@return 无
    */
    virtual void SetCurveInstance(CURVE_TYPE curType) = 0;
    /** @brief 设置移动总时间
    *	@param[in]      uTime           移动总时间
    *	@return 无
    */
    virtual void SetTotalMoveTime(unsigned int uTime) = 0;    
    /** @brief 设置是否播放
    *	@param[in]      bool            播放或停止（播放从0开始播，停止会把控件放回原始位置）
    *	@return 无
    */
    virtual bool SetPlayState(bool bPlay) = 0;    
    /** @brief 设置播放状态
    *	@param[in]      bool            播放或暂停
    *	@return 布尔类型，返回播放状态
    */
    virtual bool SetPauseState(bool bPlay) = 0;    

    virtual void SetIsSymmetrical(bool bIsSymmetrical) = 0;     // 是否对称
    virtual bool GetIsSymmetrical() = 0;                                 // 是否对称
    /** @name 一些get方法
    */
    //@{
    virtual CURVE_TYPE GetCurveType() = 0;
    virtual int GetControlingCtrlID() = 0;
    virtual IBCInterFace *GetControlingCtrl() = 0;
    virtual POINT GetControlP() = 0;
    virtual POINT GetStartP() = 0;
    virtual POINT GetEndP() = 0;    
    virtual unsigned int GetTotalMoveTime() = 0;
    virtual bool GetPlayState() = 0;
    //@}
};
///可分页的容器
//interface  IPagContain: virtual public IContain
//{
//public:
//    virtual void IInsertControl(IBCInterFace * bControl)=0;     ///< 向窗口中添加控件    
//    virtual void IDeleteControl(IBCInterFace * pcontrol)=0;     ///< 移除某个控件
//    virtual bool SetNumPerPage(BYTE nNum) = 0;                  ///< 设置此容器一页放多少
//    virtual bool NextPage() = 0;                                ///< 向下翻页
//    virtual bool PrevPage() = 0;                                ///< 向上翻页
//    virtual void FirstPage() = 0;                               ///< 第一页
//    virtual void LastPage() = 0;                                ///< 最后一页
//};

/** @brief FLASH播放控件
*/
interface IFlashPlay : virtual public IBCInterFace, virtual public IManual
{
public:
    /** @brief 载入flash文件并开始播放
     *	@param[in]      pszSwfFile
     *	@return 布尔类型
     */
    virtual bool LoadFlashFile(const wchar_t *pszSwfFile) = 0;
    /** @brief 设置参数
     *	@param[in]      pszName 参数名
     *	@param[in]      pszVal  参数值
     *	@return 布尔类型
     */
    virtual bool SetVariable(const wchar_t *pszName, const wchar_t *pszVal) = 0;
    /** @brief 得到FLASH文件名
     *	@return 文件名
     */
    virtual bool GetSwfFileName(wchar_t *pFileName, int nSize) = 0;
    /// 使用窗口
    /// 使用窗口即将网页控件以子窗口的形式弹出，支持各种输入操作
    virtual void SetUseWindow(bool bUse = true) = 0;
};

/** @brief 走马灯控件
*  revolving scenic lamp
*/
interface IRSLamp : virtual public IText
{
public:
    /** @brief 设置滚动速度，每秒多少像素
    *	@param[in]      nPixelPerSec        如果<，则静止不动
    *	@param[in]      nFreshTime          刷新时间
    *	@return 布尔型
    */
    virtual bool SetPixelPerTime(int nPixelPerSec, int nFreshTime) = 0;
    /** @brief 设置滚动方向
     *	@param[in]      nDir                0向左，1向右
     *	@return 布尔型
     */
    virtual bool SetRevoDir(int nDir) = 0;
    /// 设置底图路径
    virtual void SetBKImage(wchar_t * pchar) = 0;
    /** @brief 从文本文件中载入跑马灯内容
    *	@param[in]      *pwszFileName
    *	@return 正常返回0，要是不返回0....
    */
    virtual int LoadContentFromFile(wchar_t *pwszFileName) = 0;
    /** @brief 设置两条之间的间隔(ms)     
     */
    virtual void SetRowInterval(DWORD dwInterval) = 0;

    /** @brief Get方法
     */
    virtual int GetPixelPerSec() = 0;
    virtual int GetRefreshTime() = 0;
    virtual int GetRevoDir() = 0;
    virtual DWORD GetRowInterval() = 0;
    /// 获取容器底图路径
    virtual wchar_t * GetBkImagePath() = 0;
};

/** @brief 光标
*/
interface IXUICursor
{
public:
    /** @brief 设置光标图片 
    */
    virtual int SetCursorImage(const wchar_t *pCurImg) = 0;
    /** @brief 获取光标图片路径
    */
    virtual const wchar_t *GetCursorImage() = 0;
    /** @brief 设置切图座标
    */    
    //@{{
    virtual void SetSrcX(int x) = 0; 
    virtual void SetSrcY(int y) = 0; 
    virtual void SetSrcW(int w) = 0; 
    virtual void SetSrcH(int h) = 0; 
    //@}}
    /** @brief 获取图片大小
    */
    //@{{
    virtual int GetCursorImageWidth() = 0;
    virtual int GetCursorImageHeight() = 0;
    //@}}
    /** @brief 设置显示时是否隐藏鼠标
    */
    virtual void SetHideCursor(bool bHide) = 0;
    /** @brief 获取显示时是否隐藏显示标志
    */
    virtual bool GetHideCursor() = 0;
    /** @brief 设置座标模式
    *         0表左上角为鼠标
    *         1表中央为鼠标
    *         2表右上角为鼠标     
    */
    virtual int SetPointMode(int nMode) = 0;
    virtual int GetPointMode() = 0;

    /** @brief 设置可见
     */
    virtual void SetControlVisible(bool bVisible) = 0;
};

////////////////////////////////////////////////////////////////////////////
/// 整个UI界面的接口，通过本接口，可以获取指定控件ID的控件接口
interface  IUserInterface
{
public:
	///下面全部是给用户的接口函数
	virtual BLONGED GetControlBlonged(int id)=0;  ///<根据ID判断控件属于何容器，不属于任何容器则返回none
	virtual int GetControlBlongedId(int id)=0;   ///<根据ID求得控件属于容器的ID，如果不属于任何容器，则返回ID=0
	virtual IBCInterFace * GetIControlbyID(int Id)=0; ///<得到控件指针
	virtual IBCInterFace * GetIControlbyName(wchar_t * pName)=0;///<得到控件指针
	virtual int GetOppId(POINT point)=0;
	virtual int GetSelectID()=0;///<获取鼠标点下时的控件ID

	virtual void DeleteAllControl()=0;///<删除界面所有控件
	virtual int GetControlSize()=0;                              ///<获取控件数量
	virtual void SetFocousID(int id)=0;///<设置获取焦点ID
	virtual int GetFocous()=0;              ///<获取当前获取焦点ID
	virtual void SortControl()=0;    ///<将控件按约定规则排序
	virtual void SortControlByID()=0;  ///<按ID顺序来排序
	virtual void IInsertControl(IBCInterFace * bControl)=0; ///<向窗口中添加控件
	virtual void IDeleteControl(IBCInterFace * bControl)=0;      ///<删除队列中的控件
	virtual void IRemoveControl(IBCInterFace * bControl)=0;      ///<移除队列中的控件，只是在该队列中移除，该控件实际还存在
	virtual IBCInterFace * IGetControlByVectorIndex(int index)=0;///<由vector的下标返回指该对像指针
	virtual IBCInterFace * Create(int index)=0;                   ///<创建各个控件方法
	virtual void InsertControlID(int nId)=0;          ///<记录添加的控件ID
	virtual void DeleteControlID(int nId)=0;          ///<删除添加的控件ID
	virtual void DeleteOneControlId(int nId)=0;       ///<删除一个控件的ID，如果是容器也只是在ID列表中删除自己的，这里只在修改层次时用到了。
	virtual void SortControlByZord()=0;  ///<将所有控件的ID按控件的层次重新排序
	virtual void LButtonDown(POINT point)=0;       ///<鼠标左键按下
	virtual void LButtonUp(POINT point)=0;         ///<松开鼠标左键
	virtual void MouseMove(POINT point)=0;         ///<鼠标移动
	virtual void RButtonDown(POINT point)=0;       ///<鼠标右键按下
	virtual void KeyDown(int nChar)=0;            ///<键盘按下事件
	virtual void KeyUp(int nChar)=0;            ///<键盘按下事件
	virtual void KeyChar(wchar_t nChar)=0;            ///<键盘输入事件
	virtual void OnSize()=0;                       ///<界面大小改变事件
	virtual int LoadData(wchar_t *szFileName)=0;
	virtual void SetParentHwnd(HWND hwnd)=0; ///<设置属性窗口的句柄
	virtual int GetIDInBox()=0;
	virtual void SetIdInBox(int id)=0;

	virtual void FreeUI()=0;           ///<释放界面控件


	virtual void SetGameBoxLeft(int x)=0;                     ///<设置编辑框的坐标
	virtual void SetGameBoxTop(int y)=0;                     ///<设置编辑框的坐标
	virtual void SetGameBoxWidth(int w)=0;                     ///<设置编辑框的坐标
	virtual void SetGameBoxHeight(int h)=0;                     ///<设置编辑框的坐标
	virtual int GetGameBoxLeft()=0;                     ///<设置编辑框的坐标
	virtual int GetGameBoxTop()=0;                     ///<设置编辑框的坐标
	virtual int GetGameBoxWidth()=0;                     ///<设置编辑框的坐标
	virtual int GetGameBoxHeight()=0;                     ///<设置编辑框的坐标

    /** @brief 从文件中增加控件
    *	@param[in]          nStartID             起始ID，如果内存有多个控件，则增回此ID
    *	@return 增加后的nStartID值
    */
    virtual int AddControlFromFile(int nStartID, wchar_t *szFileName, IContain *pParent = NULL, CMDCallBack CallBack = NULL) = 0;

    /** @brief 从文件中增加控件，ID号不变，如果有ID号重复了，则不增加
    */
    virtual int AddControlFromFile(wchar_t *szFileName, IContain *pParent = NULL, CMDCallBack CallBack = NULL) = 0;
    /** @brief 取得命令接口
     */
    virtual ICmdInterface *GetCmdInterface() = 0;
    /** @brief 写文件
     */
    virtual bool WriteFile(wchar_t * pFilePath) = 0;
    /** @brief 写文件
     */
    virtual bool WriteFile(wchar_t * pFilePath, IBCInterFace *pInterface) = 0;
    /** @brief 获取光标控件
     */
    virtual IXUICursor *GetICursor() = 0;
    /** @brief 设置客户区域大小
     */
    virtual void SetClientSize(SIZE size) = 0;
    /** @brief 获取客户区域大小
    */
    virtual SIZE GetClientSize() = 0;
};

/** @brief 跑胡子控件接口
 */
interface IHuziCard : virtual public IBCInterFace
{
    enum 
    {
        MAX_VER_COUNT=4
    };
    /// 编辑器接口
public:
    /** @brief 设置垂直间隔
     */
    virtual void SetVerInterval(int nInterval)=0;
    /** @brief 设置胡子图片
     */
    virtual void SetImagePath(const wchar_t *pPath)=0;
    /** @brief 设置牌值
     *  传入二维数组
     */     
    virtual void SetCardList(BYTE ucCard[][MAX_VER_COUNT], int nLen)=0;
    /** @brief 设置垂直切图数目
     */
    virtual void SetVerCutCount(int nCount)=0;
    /** @brief 设置是否可选牌
     */
    virtual void SetCanSelectCard(bool bCan)=0;
    /** @brief 设置控件方向
     */
    virtual void SetControlRotate(int nDegree)=0;
    /** @brief 设置横向间隔
     */
    virtual void SetHorInterval(int nInterval)=0;

public:
    /** @brief Getter方法
     */
    //@{{
    virtual int GetVerInterval()=0;
    virtual int GetHorInterval()=0;
    virtual const wchar_t *GetImagePath()=0;
    virtual void GetCardList(BYTE ucCard[][MAX_VER_COUNT], int nSize, int &nReadSize)=0;
    virtual int GetVerCutCount()=0;
    virtual bool GetCanSelectCard()=0;
    virtual int GetControlRotate()=0;
    //@}}
};

/// 排名榜控件接口
interface ITopPaper : virtual public IBCInterFace
{

public:
	///特有的函数 给用户接口

	virtual int LoadBkPic(wchar_t *szFileName) = 0;   ///<通过路径加载背景图
	virtual wchar_t* GetBkPicPath() = 0;			  ///<获取背景图的路径

	virtual void SetFontStyle(wchar_t * pFontStyle) = 0;  ///<设置控件字体样式
	virtual wchar_t * GetFontStyle() = 0;                 ///<获取控件字体样式

	virtual void SetFontSize(int fontsize) = 0;		///<设置控件字体大小
	virtual int GetFontSize() = 0;					///<获取控件字体大小

	virtual void SetLeanSide(int Side) = 0;			///设置控件的停靠靠方向 左，右，上，下
	virtual int GetLeanSide() = 0;					///获得控件的停靠靠方向

	virtual void SetDefaultFontColor(COLORREF crFontColor) = 0; ///<设置控件默认的字体显示颜色
	virtual COLORREF GetDefaultFontColor() = 0; ///<设置获取控件默认的字体显示颜色

	virtual void SetRowNums(int num) = 0;	///设置行数
	virtual int GetRowNums() = 0;			///获得行数

	virtual void SetColNums(int num) = 0;	///设置列数
	virtual int GetColNums() = 0;			///获得列数

	virtual void SetRowH(int px) = 0;	//设置行高
	virtual int GetRowH() = 0;			//获取行高

	virtual void SetRowSpace(int px) = 0;	///设置行距
	virtual int GetRowSpace() = 0;			///获得行距

	virtual void SetColSpace(wchar_t* pColSpace) = 0;	///设置列距
	virtual wchar_t* GetColSpace() = 0;			///获得列距

	virtual void SetColW(int px) = 0;	///设置列宽
	virtual int GetColW() = 0;			///获得列宽

	virtual void SetTextBeginXY(int x,int y) = 0; //设置第一个文本左上角的xy坐标
	virtual POINT GetTextBeginXY() = 0;		//获取第一个文本左上角的xy坐标

	virtual void SetCornerSpace(int px) = 0;	//设置露出边角的距离
	virtual int GetCornerSpace() = 0;	//获得露出边角的距离

	virtual void SetRowColors(wchar_t* pColorDef) = 0;	///设置行颜色
	virtual wchar_t* GetRowColors() = 0;				///获得行颜色

	virtual void SetTestMode(bool bTest) = 0; //设置测试模式
	virtual bool GetTestMode() = 0;  //设置控件是否在测试模式

	virtual void SetPaperText(int nRow,int nCol,wchar_t* wszText) = 0; //设置榜单上的文本
	virtual wchar_t* GetPaperText(int nRow,int nCol) = 0; //得到榜单上的文本

	virtual void ClearAllText() = 0;	//清理所有文本内容
	
	virtual bool GetIsShowOutline()=0;	///是否显示描边——LCF
	virtual COLORREF GetOutlineColor()=0;	///描边颜色——LCF
	virtual void SetFontOutlineInfo(bool isOutline, COLORREF color) = 0;	 ///设置字体是否描边及描边颜色——LCF
};

#endif



