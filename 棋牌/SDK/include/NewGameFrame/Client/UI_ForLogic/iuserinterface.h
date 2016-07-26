///// 用来提供给效果实现模块的接口定义
#pragma once
#ifndef  _IUSERINTERFACE_H
#define  _IUSERINTERFACE_H
#include <iostream>

#ifdef UI_EXPORTS
#define UI_API __declspec(dllexport)
#else
#define UI_API __declspec(dllimport)
#endif UI_EXPORTS
#include <Windows.h>
enum BLONGED{NONE=1,CONTAIN=2};          ///<控件所属父类，none表示不属于任何控件，contain表示属于包容器控件
enum FONTSHOWSTYLE{AlignmentLEFT,AlignmentCenter,AlignmentFar, AlignmentVerticalCenter};    ///<字体对齐模式
enum FONTSHOWDIRECTION {FormatFlagsLevel,FormatFlagsVertical};
enum ControlKind{C_Contain,C_Button,C_Image,C_Card,C_Cartoon,C_Text,C_EditorText,C_ComBox,C_Progress,C_ScrollBar,
	C_ImageFlash,C_Web,C_RadioButton,C_ListControl,C_Beacon,C_Talk,C_Num,C_NoMoveCard,C_LightCard,C_CanOperateCard,
    C_Slider,C_InputUserName,C_Menu,C_Sudoku,C_Time,C_SiTuanOperateCard,C_SiTuanNoMoveCard,C_SiTuanLightCard,C_ExAnimate, 
    C_MjCtrl, C_MjHandCtrl, C_MjGiveCtrl, C_MjWallCtrl, C_MjBlockCtrl,C_MoveAction,C_ShCtrl, C_FlashPlay, C_RSLamp, C_HuziCard,
	C_TopPaper}; ///<控件类型
enum PLAYSTATE{START,PLAYING,PAUSE,CONTINUE,STOP};///<播放状态
enum CARD_SHOW_DIR{MYSELF,LEFT,PARTNER,RIGHT};   ///<牌显示方向 
typedef enum enmCardShape
{
	ECS_SHAPE_SPECIAL =5,	/*!< 特别牌（给lym用） */
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
	virtual void SetControlVisible(bool bflag)=0;
	virtual bool    GetVisible()=0;           ///<获得控件是否显示
	virtual int GetZorder()=0;///<获取控件层次
	virtual void SetZorder(INT16 zorder)=0;///<设置控件层次
	virtual void SetGameXY(int x,int y)=0;   ///<这个主要给游戏用的
	
	virtual int GetGameX()=0;
	virtual int GetGameY()=0;
	virtual int GetGameW()=0;
	virtual int GetGameH()=0;
	virtual void SetGameW(int w)=0;
	virtual void SetGameH(int h)=0;
	virtual void SetTooltip(wchar_t * pchar)=0;   ///<设置控件的鼠标移上提示字符串
	virtual wchar_t * GetToolTip()=0;             ///<获取控件的鼠标提示字符串
	virtual void SetIsMove(bool bFlag)=0;
	virtual bool GetIsMove()=0;

	virtual int GetGameControlID()=0; ///<获取控件DI
	virtual void SetTipDrawAtMouse(bool bDrawAtMouse = true)=0;
	virtual void SetTipPath(wchar_t *p)=0;//自定义TIP背景图片
	virtual wchar_t * GetTipPath()=0;

	virtual void SetHaveShrinkArea(bool bFlag)=0;


    ///** @brief 每个控件都有这个方法，将控件存储到内存中
    // *  存控件时，调用此方法即可
    // *  在此函数内部实现版本控制
    // *  @param[in]          *pstream          要写的stream流，可以是ofstream，也可以是ostringstream
    // *  @return bool 成功与否
    // */ 
    //virtual bool ToMemory(std::ostream *pstream) = 0;
    ///** @brief 每个控件都有这个方法，从内存中读取控件
    // *  在此函数内部实现版本控制
    // *  @param[in]          *pstream          要读的stream流，可以是ofstream，也可以是ostringstream
    // *  @return bool 成功与否
    // */ 
    //virtual bool FromMemory(std::istream *pstream) = 0;
    
};

/// 图像控件接口
interface  IImage :  virtual public IBCInterFace
{
public:
    virtual IBCInterFace * Clone(int nId)=0;
    virtual int LoadPic(wchar_t *szFileName)=0;

    ///获取IMG信息,获取加载的图片宽度
    virtual int GetPicWidth()=0;
    ///获取图片高度
    virtual int GetPicHeight()=0;
    virtual void SetSrcX(int x)=0; ///<设置此图在原材质中的位置
    virtual void SetSrcY(int y)=0; ///<设置此图在原材质中的位置
    virtual void SetSrcW(int w)=0; ///<设置此图在原材质中的位置
    virtual void SetSrcH(int h)=0; ///<设置此图在原材质中的位置

    /** @brief 设置是否左右镜像显示
     *	@param[in]      bCrossMirror
     *	@return 无
     */
    virtual void SetCrossMirror(bool bCrossMirror)=0;
    virtual void SetDir(int degree, POINT pt)=0;

    /** 
     * @brief 设置鼠标进入图片范围中是否显示为手形光标
     * @param[in] bShow true:显示, false:不显示
     */
    virtual void SetIsHandCursor(bool bShow) = 0;

	///<是否鼠标穿透
	virtual void SetMouseThough(bool bThough)=0;    

	///< 是否响应鼠标移动消息
	virtual void SetMouseMove(bool bMouseMove) = 0;

	/// 设置图片四个顶点的颜色，按左上，右上，左下、右下的次序
	virtual void SetColor(DWORD c1, DWORD c2, DWORD c3, DWORD c4) = 0;

	//获取图片旋转角度
	virtual int GetDir() = 0;
};

/// 按钮控件接口
interface  IButton : virtual public IImage
{
public:
    ///特有的函数 给用户接口
    virtual void SetEnable(bool flag)=0;              ///<设置控件是否可用
    virtual bool GetEnable()=0;;                      ///<获取控件是否可用
    virtual void SetClickState(bool bFlag)=0;         ///<设置是否是点击状态 
    virtual void SetSelectState(bool bFlag)=0;        ///<设置是否是选择状态
    virtual void SetText(wchar_t *pchar)=0;			  ///<设置鼠标在按钮显示静态文字
    virtual IBCInterFace * Clone(int nId)=0;
	virtual void CreateRgn() = 0;                     ///<创建区域
    virtual void SetParameter(void * pParam) = 0;     ///<设置按钮中的参数
    virtual void * GetParameter() = 0;                ///<获取按钮中的参数
};


///文本控件
interface  IText: virtual public IBCInterFace
{
public:
	virtual wchar_t * GetText()=0;                      ///<获取控件显示内容
	virtual void SetText(wchar_t * pText)=0;///<设置控件显示内容
	virtual void SetFontColor(COLORREF crFontColor)=0; ///<设置控件字体显示颜色
	virtual IBCInterFace * Clone(int nId)=0;
	virtual void SetFontShowStyle(FONTSHOWSTYLE showStyle)=0;   ///<设置控件字体显示模式，居左，居中，居右
    virtual void SetMutiShow(bool bFlag)=0;             ///<设置是否多行显示
    virtual void SetMouseThough(bool bThough)=0;    ///<是否鼠标穿透
	virtual int GetFontSize()=0; 
	virtual void SetFontSelectColor(DWORD color) = 0;	 ///设置字体选择颜色
	virtual bool GetIsShowOutline()=0;	///是否显示描边——LCF
	virtual DWORD GetOutlineColor()=0;	///描边颜色——LCF
	virtual void SetFontOutlineInfo(bool isOutline, COLORREF color) = 0;	 ///设置字体是否描边及描边颜色——LCF
};
///文本编辑框接口
interface  IEditorText : virtual public IText
{
	virtual void SecondInit()=0;
	virtual void Clear()=0;
	virtual IBCInterFace * Clone(int nId)=0;

    /** @brief 设置允许输入某范围的ASCII
     *  如果只允许输入'0'~'9'，则调用 SetEnableASCII('0', '9');
     *  @param[in]      nType       允许模式，0表全部允许，1表允许部分，2表不允许部分
     *	@return
     */
    virtual void SetEnableASCII(int nType, wchar_t cStart=' ', wchar_t cEnd=' ') = 0;
    /** @brief 设置最大输入字符
     *	@param[in]      nMaxChar    最大允许字符
     *	@return 无
     */
    virtual void SetMaxTypeIn(int nMaxChar) = 0;
    /// 设置是否可用
    virtual void SetEnable(bool bEnable) = 0;

    /** 
    * @brief 设置此编辑框允许的数值范围
    * @param[in] bEnable 此功能是否启用
    * @param[in] nMinNum 要限制的最小数值范围
    * @param[in] nMaxNum 要限制的最大数值范围
    */
    virtual void SetNumberLimit(bool bEnable, int nMinNum = 0, int nMaxNum = 0) = 0;
	virtual bool GetIsPassWord()=0;
	virtual void SetSendKeyCharMessage(bool bSendKeyCharMessage) = 0;///<是否发送字符消息
	virtual bool GetSendKeyCharMessage() = 0;///<获取是否发送字符消息
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
	virtual int SetItemHeight( int cyItemHeight)=0;         ///<设置的有条目的高度，宽度和控件宽度一样
	virtual void SetAddItemWidth(int iAddWidth)=0;			///<增加宽度
	virtual int GetAddItemWidth()=0;						///<获取增加宽度

	virtual void SetControlMoveDirect(BYTE bDir)=0;  ///<设置下拉条显示的方向 默认 为0向下， 1向上
	virtual IBCInterFace * Clone(int nId)=0;

    virtual void SetEditable(bool bCanEdit)=0;
	virtual wchar_t * GetText()=0;							///<获取控件显示内容
	virtual void SetText(wchar_t * pText)=0;				///<设置控件显示内容   
	virtual void SetMaxTypeIn(int nMaxChar) = 0;
	virtual void TextClear()=0;								///<清除Combox的Edit里面的文字
	virtual void SetFontShowStyle(FONTSHOWSTYLE style)=0;
	virtual void SetFontSelectColor(DWORD color) = 0;					///设置字体选择颜色
	virtual DWORD GetFontSelectColor() = 0;								///获取字体选择颜色

    virtual void SetComboxFontSize(int fontsize) = 0;       ///<设置下拉列表框中的字体大小
    virtual void SetComboxFontColor(COLORREF crColor) = 0;  ///<设置下拉列表框中的字体颜色
    virtual void SetTooltipEnable(bool bEnable) = 0;        ///<设置是否启动工具提示
};

interface  IInputUserName: virtual public ICombox
{
public:
    virtual void SetMaxNum(int nMaxNum)=0;
	virtual void SetDeleteButton(bool bFlag) =0;			///<设置是否显示删除按钮
	virtual bool GetDeleteButton() =0;						///<获取删除按钮显示状态
	virtual void SetAddSelectW(int iAddSelect) =0;			///<设置下拉框加长宽度
	virtual int GetAddSelectW() =0;							///<获取下拉框加长宽度
	virtual void SetFontSelectColor(DWORD color) = 0;		///<设置编辑框选择字体底色
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
    virtual void SetNoPagecontentID(int nIDs[], int nSize)=0;   ///< 设置10个不需要分页的控件ID

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

    /**
    * @brief 移除内容
    */
    virtual void RemovePagecontent(IBCInterFace * pcontrol) = 0;
};
///容器
interface  IContain: virtual public IBCInterFace
{
public:
	virtual void MoveControl(int DistanceX,int DistanceY)=0;              ///< 移动容器，其中的所有控件也要一起移动
	virtual void SetChildVisable(IBCInterFace *pChild, bool bVisable)=0;  ///< 设置子控件的原本显示属性 
	virtual IBCInterFace * Clone(int nId)=0;
	virtual int SetPic(int nindex,wchar_t * pwhcar)=0;                    ///< 给容器中控件设置图片，这里传的是控件ID，成功返回0 失败返回-1

	virtual int GetControlSize()=0;                                       ///< 所含控件个数
	virtual int GetControlBlongContainID(int id)=0;                       ///< 获取指定ID的上层容器控件ID
	virtual void IInsertControl(IBCInterFace * bControl)=0;               ///< 向窗口中添加控件
	virtual void IDeleteControl(IBCInterFace * pcontrol)=0;               ///< 移除某个控件
	///用户接口
//	virtual BControlInterface * GetControl(POINT point)=0;                ///< 根据位置和控件层次来确定获取的控件指针
//	virtual BControlInterface * GetControl(int id)=0;                     ///< 根据位置和控件层次来确定获取的控件指针
    virtual IContainPaginator *GetContainPaginator() = 0;                 ///< 获取分页操作	

    virtual void SortAllSubControlByZord() = 0;                           ///< 重新全体排序
	virtual bool GetControlShowFlag(IBCInterFace *pBcInterface)=0;        ///< 控件是否随容器的显示显示，因为有些控件不需要显示，需要特定时期才显示
    virtual void SetEnable(bool bEnable) = 0;                             ///< 设置容器是否禁用, 有时候根据功能需求容器不需要响应任何鼠标事件(true: 启用,  false:禁用)

	virtual void SetAllZorder(INT16 zorder) = 0;					  /// 设置空间层次接口			向凌华 2012.11.29
};

///<静态动画
interface ICartoon:virtual public IBCInterFace
{
public:
	virtual void SetSpeed(int speed)=0;  ///<设置动画速度
	virtual void SetState(PLAYSTATE bstate)=0; ///<设置动画状态
	virtual void SetPlayNum(int iplaynum)=0; ///<设置播放次数
	virtual byte GetFrameNum()=0;             ///<获取帧数
	//virtual bool GetVisble()=0;              ///<获取控件是否显示
	virtual int GetSpeed()=0;                ///<获取播放速度
	virtual PLAYSTATE GetState()=0;                ///<获取播放状态
	virtual int GetPlayNum()=0;             ///<获取播放次数
	virtual byte GetCurrentFrame()=0;  ///<获取当前播放的帧索引

	virtual void SetShowIndex(int nIndex)=0; ///<设置要显示的帧的索引
	virtual IBCInterFace * Clone(int nId)=0;

	virtual void SetPlayStarFrameNum(int nindex)=0;
	virtual void SetPlayEndNum(int nNum)=0;
	
	virtual void SetCurrentShowFrameNum(int nNum)=0;
	virtual int GetPlayEndNum()=0;
	virtual int GetCurrentShowFrameNum()=0;
	virtual int GetPlayStartNum()=0;
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
	virtual IBCInterFace * Clone(int nId)=0;
	virtual void SetBarHeightFixed(bool bFlag)=0;
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
	virtual IBCInterFace * Clone(int nId)=0;
};
//web 事件回调接口 2012.12.03 yyf
interface IWebBrowserEvent
{
public:
	//网页在跳转前的信息 2012.12.03 yyf
	//wchar_t * pWcharURL ：传入判断的网址
	//short &bCancel: 如果网址符合，被设为 TRUE 代表这个网址不跳转。
	virtual void BeforeNavigate2(wchar_t * pWcharURL,short &bCancel) = 0; 

	//截取弹出网页网址。2012.12.05 yyf
	//wchar_t * pWcharURL ：传入判断的网址
	//short &bCancel: 如果网址符合，被设为 TRUE 代表这个网址不跳转。
	virtual void NewWindow3(wchar_t * pWcharURL,short &bCancel) = 0; 
	
};
interface IWeb:virtual public IBCInterFace, virtual public IManual
{
public:
	///<基本方法
	virtual void SetShowWebAddress(wchar_t* pchar)=0;
    /// 使用窗口
    /// 使用窗口即将网页控件以子窗口的形式弹出，支持各种输入操作
    /// 在设置成焦点控制后，此函数无效
    virtual void SetUseWindow(bool bUse = true) = 0;
    /// 设置焦点控制
    /// 有焦点控制时，会在获得焦点时切换成窗口模式，盖住所有控件，并且SetUseWindow函数无效化
    /// 在无焦点时切换成普通模式，可被其它控件盖住
    virtual void SetFocusSwitching(bool bSwitch) = 0;

	//设置需要回调的指针 2012.12.03 yyf
	virtual void SetBindWebBrowserEvent(IWebBrowserEvent * pWebBrowserEvent)=0;
};

interface IRadioButton:virtual public IBCInterFace
{
public:
	virtual void SetText(wchar_t * pchar)=0;                 ///<设置显示字符
	virtual void SetTextFontSize(int nSize)=0;               ///<设置显示字体大小
	virtual int GetTextFontSize()=0;                         ///<获取显示字体大小
	virtual void SetTextFontColor(COLORREF crColor)=0;       ///<设置字体颜色
	virtual COLORREF GetTextFontColor()=0;                   ///<获取字体颜色
	virtual bool GetIsSelect()=0;                             ///<获取是否是选中了
	virtual void SetIsSelect(bool bFlag)=0;                  ///<设置是否是选取状态
	virtual IBCInterFace * Clone(int nId)=0;
	virtual void SetBkPic(wchar_t * pwhcar)=0;
	virtual void SetSelPic(wchar_t * pwhcar)=0;
};
interface ITalk:virtual public IBCInterFace
{
	virtual  void SetText(wchar_t *  wchar)=0; ///<设置显示的文字
	virtual int  GetRowCount()=0;            ///<获取当前显示的行数
	virtual wchar_t * GetText(int nIndex)=0;   ///<获取某一行的显示字符数
	virtual IBCInterFace * Clone(int nId)=0;
	virtual void ClearText()=0;
	virtual bool GetIsAutoScroll()=0;
	virtual void SetIsAutoScroll(bool bAuto)=0;
	virtual void CopyAllText()=0;
};
interface IBeacon:virtual public IBCInterFace
{

};

interface IListControl:virtual public IBCInterFace
{
public:
	///<基本方法
	virtual void SetRowHeight(int nHeight)=0;                  ///<设置每行的高度
	virtual int  GetRowHeight()=0;							   ///<获取每行的间隔
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

	virtual void InsertColumn(int nIndex,wchar_t * pText,int nWidth,int nHeight)=0;              ///<添加列头

    /** 
     * @brief 插入节点
     * @param [in] nRow     插入的行
     * @param [in] nCol     插入的列
     * @param [in] pText    插入的文本内容
     * @param [in] pContain 插入的容器(主要是在单元格中嵌入一个容器, 容器中的内容无限制), 插入容器时, 第3个参数pText无效(填充: L"")
     * @return void
     */
	virtual void InsertInItem(int nRow, int nCol, wchar_t * pText, IContain * pContain = NULL) = 0;

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
	
	virtual void SetHeadObjHeight(int nH)=0;  ///<设置表头对像的高
	virtual int GetHeadObjHeight()=0;    ///<获取表头对像的高度

	virtual void Clear()=0;               ///<清空所有行，但不清空表头
	virtual void SetHeadFontStyle(wchar_t * pFontStyle)=0;
	virtual void SetHeadFontSize(int fontsize)=0;
	virtual void SetHeadFontColor(COLORREF crColor)=0;
	virtual void SetRowFontStyle(int nRowIndex,wchar_t * pFontStyle)=0;
	virtual void SetRowFontSize(int nRowIndex,int fontsize)=0;
	virtual void SetRowFontColor(int nRowIndex,COLORREF crColor)=0;
	virtual void SetRowFontColor(int nRowIndex,int nColIndex,COLORREF crColor)=0;


	virtual void SetSelectRectColor(COLORREF crSelect)=0;            ///<设置选中行的颜色
	virtual COLORREF GetSelectRectColor()=0; ///<返回选中行的颜色

	virtual void SetRowFontIsBold(int nRowIndex,bool bFlag)=0;
	virtual void SetHeadFontIsBold(bool bFlag)=0;
	virtual IBCInterFace * Clone(int nId)=0;

	virtual wchar_t * GetItemText(int nRow,int nCol)=0;  ///<不算表头，下标都从0开始的
	virtual void ModifyItemText(int nRow,int nCol,wchar_t * pchar)=0; ///<修改某列值
	virtual int GetRowNum()=0;
	virtual int GetColNum()=0;
	virtual void SetTextShowStyle(int nCol,FONTSHOWSTYLE style)=0; ///<设置某列的显示方式
	virtual void ModifyHeadFontText(int nIndex,wchar_t * pFontText)=0;///<设置表头显示的内容
	virtual int GetSelectRowIndex()=0;                                ///<获取选中的行下标，从0开始，没选中则返回-1

	virtual void  DeleteRow(int nIndex)=0;        ///<删除某一行  不能删除表头，只能修改

    virtual void SetIsMouseSelectColor(bool bFlag) = 0;       ///< 是否启用鼠标移动选择颜色
    virtual void SetMouseSelectColor(COLORREF crSelect) = 0;  ///< 设置鼠标当前所在行的颜色
    virtual COLORREF GetMouseSelectColor() = 0;               ///< 返回鼠标当前所在行的颜色

    virtual void SetCellButton(bool bShow) = 0;               ///< 设置是否显示单元格按钮
    virtual void SetButtonImage(wchar_t * path) = 0;          ///< 设置单元格中右边指针按钮图片

    virtual bool IsBtnRect() = 0;                             ///< 判断是否点在按钮区域
	virtual void SetVerBarHeightFixed(bool bFlag) = 0;		  ///< 设置垂直滚动条是否根据内容多少自动调整高度

	virtual void SetColumnMultiLine(int nRowIndex,int nColIndex,bool bEnable = true) = 0;
};

interface ICardBase:virtual public IBCInterFace
{
	//virtual void SetCardWH(int nW,int nH)=0;  ///<设置牌的宽高
	//virtual void GetCardWH(int & nW,int & nH)=0; ///<获取牌的宽高
	//virtual int GetCardHSpace()=0;           ///<获取牌的水平间距
	//virtual void SetCardFlushStyle(int nIndex)=0; ///<设置排序方式
	//virtual int GetCardFlushStyle()=0;        ///<获取排序方式
	virtual void SetCardList(BYTE BList[] ,int nNum)=0;     ///<设置显示的牌
	//virtual void SetCardPic(BYTE nIndex,wchar_t * pchar)=0;  ///<设置牌的资源.顺便加载进去
	virtual int GetCardList(BYTE BCardList[])=0;             ///<获取显示的牌 
	//virtual wchar_t * GetCardPic(int nIndex)=0;              ///<获取牌资的路径
	virtual int GetCardNum()=0;                          ///<获取牌的数量
	virtual void RemoveCard(BYTE BCardList[],int nNum)=0;                   ///<移除其中的牌
	//virtual void SetCardAlignStyle(BYTE bStyle)=0;    ///<设置对齐模式 0-2  0表示左对齐，1表示居中，2表示右对齐
	//virtual BYTE GetCardAlignStyle()=0;

	//virtual ENM_CARD_NUMBER GetCardPointByIndex(int nIndex)=0;           ///<获取指定索引的牌的点数
	//virtual ENM_CARD_SHAPE GetCardFigureByIndex(int nIndex)=0;          ///<获取指定索引的牌的花色
	//virtual void RemoveCard(BYTE BCardList[],int nNum)=0;                   ///<移除其中的牌
    virtual void SetCardHSpace(int nSpace)=0;  ///<设置牌的水平间距

    /** 
     * @brief 设置赖子牌图片
     * @param 牌图片资源路径
     */
    virtual void SetLaiZiImagePath(wchar_t * pchar) = 0;

    /** 
     * @brief 设置赖子牌
     * @param bCards       赖子牌数组
     * @param byLaiziCount 数组大小
     */
    virtual void SetLaiZiCard(BYTE bCards[], BYTE byLaiziCount) = 0;

	/** 
	* @brief 设置特别牌图片
	* @param 牌图片资源路径
	*/
	virtual void SetSpacialImagePath(wchar_t * pchar) = 0;

	//设置是否发送鼠标移动消息
	virtual void SetMoveCardMessage(bool bSetMoveCardMessage) = 0;

	//设置是否发送鼠标移动消息
	virtual bool GetMoveCardMessage() = 0;

	//获取当前移动牌值
	virtual BYTE GetMoveCardNum() = 0;

};

interface INoMoveCard:virtual public ICardBase
{
	virtual void SetIsShowCardNum(bool bFlag)=0;     ///<设置是否显示剩余牌的数量
	//virtual bool GetIsShowCardNum()=0;     ///<获取是否显示牌的数量
	//virtual void SetCardShowDir(CARD_SHOW_DIR cardDir)=0;       ///<设置牌的显示方向
	//virtual CARD_SHOW_DIR GetCardShowDir()=0;       ///<获取牌的显示方向
	virtual IBCInterFace * Clone(int nId)=0;
    /** @brief 设置某值的牌变灰
     *  @param[in]      ucDarkCard      数组
     *  @param[in]      ucCount         数组大小
     */
    virtual void SetDarkCard(BYTE ucDarkCard[], BYTE ucCount, COLORREF clr = 0x66FFFFFF)=0;
    /** @brief 设置某值的牌弹起
     *  @param[in]      ucUpCard        数组
     *  @param[in]      ucCount         数组大小
     *  @param[in]      ucFlag          对应的索引是往上还是往下 0 不动 1往上 2往下
     */
    virtual void SetUpCard(BYTE ucUpCard[], BYTE ucCount, BYTE ucFlag[])=0;

    virtual void SetCardShowMode(bool bVertical) = 0;  ///< 牌的摆放方式 true: 竖放,  false:横放
    virtual bool GetCardShowMode() = 0;                ///< 获取牌的摆放方式 true: 竖放,  false:横放
};

interface ILightCard:virtual public INoMoveCard
{
	//virtual void SetShowCardWH(int nW,int nH)=0;
	//virtual void GetShowCardWH(int & nW,int & nH)=0;
	//virtual void SetShowCardRowNum(int nNum)=0;
	//virtual int  GetShowCardRowNum()=0;
	//virtual void SetRowSpace(int nSpace)=0;
	//virtual int  GetRowSpace()=0;
	virtual IBCInterFace * Clone(int nId)=0;
};

interface IOperateCard:virtual public ICardBase
{
	//virtual void SetUpSpace(int nSpace)=0; ///<设置牌升起的高度
	//virtual int  GetUpSpace()=0;           ///<获取牌升起的高度
	
	virtual void SetUpCardList(BYTE BCardList[],int nNum)=0;
	virtual int GetUpCardList(BYTE BCardList[])=0 ;   ///<获取升起的牌的值


	//virtual void SetSelectImagePath(wchar_t * pchar)=0; ///<设置选择标记的图片
	//virtual wchar_t * GetSelectImagePath()=0;           ///<获取选择标记的图片路径
	virtual IBCInterFace * Clone(int nId)=0;

    virtual void SetIsDragble(bool bDragble) = 0;

	virtual int GetClickStartIndex(void) = 0;  ///<开始点下的牌的下标

	virtual const BYTE *GetUpCardSign(void) = 0;	///<获得记录控件升起的牌的标志

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
    virtual void SetImagePath(wchar_t * pchar)=0;
	virtual void SetZeroImagePath(wchar_t* pchar)=0;
	virtual wchar_t* GetZeroImagePath()=0;
	virtual void SetNumUint(int iNum)=0;
	virtual int GetNumUint()=0;
	virtual void SetRankType(int iRankType)=0;
	virtual int GetRankType()=0;
	//设置新形式数字包
	virtual void SetNewNumStruct(NEW_NUM_STRUCT NumStruct) = 0;
	//获取新形式数字包
	virtual NEW_NUM_STRUCT GetNewNumStruct() = 0;
	//设置新形式数字
	virtual void SetNewNum(__int64 nNum) = 0;
	//获取新形式数字
	virtual __int64 GetNewNum() = 0;
	virtual IBCInterFace * Clone(int nId)=0;
};

interface ITime:virtual public IBCInterFace
{
	virtual void SetShowMaxNum(int nNum)=0;         ///<设置显示最大时间
	virtual int  GetShowMaxNum()=0;                 ///<获取显示最大时间
	virtual void SetShowMinNum(int nNum)=0;        ///<设置显示最小时间 
	virtual int  GetShowMinNum()=0;              ///<获取显示最小时间
	virtual void SetShowAllTime(DWORD dtTime)=0; ///<设置显示总时间
	virtual DWORD GetShowAllTime()=0;    ///<获取显示总时间
	virtual int  GetCurrentTimeNum()=0;     ///<获取当前时间
	virtual void SetPLay(bool bFlag)=0;
	virtual void SetPlaySoundNum(int iLessTimeNum) = 0;
	virtual int  GetPlaySoundNum() = 0;
};


interface ISlider:virtual public IBCInterFace
{
	virtual void SetMaxValue(int nMax)=0;
	virtual int GetMaxValue()=0;
	virtual void SetCurrentValue(int nCurrentValue)=0;
	virtual int GetCurrentValue()=0;
	virtual IBCInterFace * Clone(int nId)=0;
	virtual void SetFillPath(wchar_t * pchar)=0;
	virtual void SetButtonSliderPath(wchar_t * pchar)=0;
};
interface IMenu:virtual public IBCInterFace
{
	virtual void AddString(wchar_t * pchar,int nKind=0)=0; ///<是添加的TEXT还是分隔线，0表示TEXT，1表示是分隔线

	virtual void SetItemHeight(int nH)=0;
	virtual void SetLineColor(COLORREF crColor)=0;


	virtual void SetFontStyle(wchar_t * pchar)=0;
	virtual void SetFontSize(int nSize)=0;                    ///<获取控件字体大小
	virtual void SetFontColor(COLORREF crColor)=0;
	virtual void SetFontShowStyle(FONTSHOWSTYLE style)=0;

	virtual void SetBKImagePath(wchar_t * pchar)=0;

	virtual void SetMoveColor(COLORREF crColor)=0;
	virtual int GetSelectIndex()=0;
	virtual int GetItemHeight()=0;

	virtual void SetMoveFramColor(COLORREF crColor)=0;
	virtual void Clear()=0;
	virtual void DeleteItem(int index)=0;
	virtual wchar_t * GetItemText(int index)=0;
};

interface ISudoku:virtual public IBCInterFace
{
	virtual RECT GetSrcRect(int nIdx)=0;	///< 获取指定索引图片大小
	virtual int SetPicByIndex(int nIdx,wchar_t *szFileName,RECT& rc) = 0; ///<设置九宫阵图
};

interface ISiTuanOperateCard:virtual public IOperateCard
{
    ///// 设置大小王排在一起
    //virtual void SetJoJOEqual(bool bEqual) = 0;
};
interface ISiTuanNomoveCard:virtual public INoMoveCard
{
	
};
interface ISiTuanLightCard:virtual public ILightCard
{

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

    
    virtual void SetMoveTotalTime(int atime)=0;                 // 设置移动时间
    virtual int GetMoveTotalTime()=0;                           // 获取移动时间

    
    virtual void SetFrameNum(int nNum)=0;                       // 设置动画帧数
    virtual int GetFrameNum()=0;                                // 获取动画帧数

    virtual void SetUseMouse(bool bFlag) = 0;                   // 是否鼠标穿透
    virtual void SetDisplayColor(COLORREF clr) = 0;             // 显示的颜色

    virtual void SetPlayTotalTime(int atime)=0;                 // 设置播放总时间
    virtual int GetPlayTotalTime()=0;

    virtual IBCInterFace * Clone(int nId)=0;

    virtual const wchar_t * GetExtFilePath() = 0;
    virtual void SetExtFilePath(wchar_t *pwszFileName) = 0;
    virtual bool LoadFromFile() = 0;

    virtual void SetCurveInstance(CURVE_TYPE curType) = 0;      // 设置曲线类型
    virtual CURVE_TYPE GetCurveType() = 0;                      // 获得曲线类型

    virtual void SetIsSymmetrical(bool bIsSymmetrical) = 0;     // 是否对称
    virtual bool GetIsSymmetrical() = 0;                        // 是否对称

    virtual void SetIsLoop(bool bIsLoop = true) = 0;            // 是否循环播放
    virtual bool GetIsLoop() = 0;                               // 是否循环播放

    virtual void SetShowFrame(int nCount) = 0;                  // 显示某一帧

    /** @brief 设置是否左右镜像显示
     *	@param[in]      bCrossMirror
     *	@return 无
     */
    virtual void SetCrossMirror(bool bCrossMirror)=0;

	virtual void SetMouseThough(bool bThough) =0;    ///<是否鼠标穿透
};


////////////////////////////////////////////////////////////////////////////////
// 麻将控件接口begin{{

const int MAX_TILES_INCTRL = 200;

const int MAX_GOD_TILE = 16;
const int MAX_GIVE_TILE = 100;

const int MAX_HAND_TILE = 20;

const int MAX_BLOCK_COUNT = 10;

// UI表现类型 
enum MJ_UI_TYPE
{
    UT_NONE=-1          ,   // 空牌（没牌）（不画）
    UT_SELF=0           ,   // 自己打出的小牌(出牌)
    UT_FOLLOW=1         ,   // 下家放倒牌(出牌)
    UT_OPPOSITE=2       ,   // 对家放倒牌(出牌)
    UT_AHEAD=3          ,   // 上家放倒牌(出牌)
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
                                ///             1位：0:普通牌    ，1:万能牌
    DWORD           dwReserve2; ///     

    MJ_TILE_UIINFO()
    {
        memset(this, 0, sizeof(MJ_TILE_UIINFO));
    }
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
	/**
    /* @brief 显示梭哈叠放筹码值
    */
    virtual void SetShStackNote(int iStackNote)=0;

    /// 对应的get方法
    virtual int GetTileCount() = 0;
    virtual bool GetShPic(wchar_t *pszPicpath, int nSrcSize) = 0;
    virtual int GetShTileWidth() = 0;
    virtual int GetShTileHeight() = 0;
    virtual bool GetIsUseMouse() = 0;
    virtual int GetShNote() =0;//获得显示梭哈筹码值
	virtual int GetShNoteType()=0;//获得筹码摆放方式
	virtual int GetShStackNote()=0;//获得显示梭哈叠放筹码值

    virtual void SetTrimNoteMode(int nTrimNote)=0;
	/** @brief 设置当前是几个人的筹码摆放方式
	 *  @param[in]    例人数为4 摆放方式m_iShNoteType:0对家，1右家，2自己，3左家
	*/
	virtual void SetShNumber(int iNumber)=0;
	virtual int GetShNumber()=0;//获取当前是几个人的筹码摆放方式
};
/////////////////////////////////////////////梭哈控件

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
    virtual int GetMjTileWidth() = 0;
    virtual int GetMjTileHeight() = 0;
    virtual bool GetIsUseMouse() = 0;
};


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
    /** @brief 设置某张牌变成万能牌的颜色
     *  如果该索引上的牌是万能牌，则此接口对其无效
     */
    virtual void SetGodColor(int nIndex, bool bIs=true) = 0;

    /** @brief 设置锁定大小
    */
    virtual void SetLockSize(bool bLock)=0;

    /** 
     * @brief 设置手动使用鼠标选择牌弹起, 此时鼠标移动效果无效
     * @param [in] bSelect 是否手动选择
     */
    virtual void SetHandSelect(bool bSelect) = 0;

    /**  
     * @brief 设置手动使用鼠标选择牌弹起, 此时鼠标移动效果无效
     * @param [in] nUpTiles 控件中弹起的牌数据
     * @param [in] nUpCount 弹起的牌数组长度
     * @return 返回实际的弹起牌的数量
     */
    virtual int GetAllUpTile(int nUpTiles[], int nUpCount) = 0;
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
    /** @brief 设置牌墙最大长度
     *  @param[in]      nMaxLength
     */
    virtual void SetMaxLength(int nMaxLength) = 0;
};

interface IMjBlockCtrl : virtual public IMjCtrl
{
    // 接口废弃不用，与IMjHandCtrl合并了
//public:
//    /**
//    * @brief 增加一组拦牌信息
//    */
//    virtual void AddOneBlock(MJ_LOGIC_TYPE type, BYTE ucTiles[4], BYTE ucChair) = 0;
//    /**
//    * @brief 清除所有拦牌显示
//    */
//    virtual void ClearAllDis() = 0;
//    /**
//    * @brief 设置视图方位
//    */
//    virtual void SetViewDirection(BYTE nViewStation) = 0;
//
//    /**
//    * @brief 得到方位
//    */
//    virtual BYTE GetViewDirection() = 0;
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


/** @brief FLASH播放控件
 */
interface IFlashPlay : virtual public IBCInterFace,virtual public IManual
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

    /// 调用callfunction
    virtual bool CallFunction(const char *pszFunctionXml) = 0;
    /** @brief 设置透明关键色
     */
    virtual void SetColorKey(COLORREF rgb) = 0;
    /// 使用窗口
    /// 使用窗口即将网页控件以子窗口的形式弹出，支持各种输入操作
    virtual void SetUseWindow(bool bUse = true) = 0;
};

/** @brief 走马灯控件
 *  revolving scenic lamp
 */
interface IRSLamp : virtual public IText
{
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
    /** @brief 从文本文件中载入跑马灯内容
     *	@param[in]      *pwszFileName
     *	@return 正常返回0，要是不返回0....
     */
    virtual int LoadContentFromFile(wchar_t *pwszFileName) = 0;
    /** @brief 设置两条之间的间隔(ms)     
     */
    virtual void SetRowInterval(DWORD dwInterval) = 0;

};

/** @brief 排名榜控件接口
 */
/// 排名榜控件接口
interface ITopPaper : virtual public IBCInterFace
{
	/** @brief 设置/获取榜单上的文本
	*/
	virtual void SetPaperText(int nRow,int nCol,wchar_t* wszText) = 0;
	virtual wchar_t* GetPaperText(int nRow,int nCol) = 0;

	/** @brief //清理所有文本内容
	*/
	virtual void ClearAllText() = 0;

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
    /** @brief 是否可见
     */
    virtual bool GetControlVisible() = 0;
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
	virtual int LoadData(wchar_t *szFileName)=0;
	virtual HWND GetWindowsHwnd()=0;///<获取当前窗口句柄
	virtual void IInsertControl(IBCInterFace * bControl)=0; ///<向窗口中添加控件
	virtual void IDeleteControl(IBCInterFace * bControl)=0;      ///<删除队列中的控件
	virtual void InsertControlID(int nId)=0;          ///<记录添加的控件ID  
	virtual void DeleteControlID(int nId)=0;          ///<删除添加的控件ID
    /** @brief 从文件中增加控件，ID号不变，如果有ID号重复了，则不增加
     */
    virtual int AddControlFromFile(wchar_t *szFileName, IContain *pParent = NULL)=0;
    /** @brief 设置光标ID     
     *  可以是以下值
     *  IDC_ARROW           32512
     *  IDC_IBEAM           32513
     *  IDC_WAIT            32514
     *  IDC_CROSS           32515
     *  IDC_UPARROW         32516
     *  IDC_HAND            32649
     *  IDC_HELP            32651
     */
    virtual bool SetUICursor(int nID)=0;
    /** @brief 获取光标控件
    */
    virtual IXUICursor *GetICursor() = 0;
    /** @brief 设置选中的控件
     */
    virtual void SetFocousID(int id) = 0;

    /// duanxiaohui 设置窗口是否显示/隐藏(主要解决全屏大厅UI卡死问题)
    virtual void SetShow(bool bShow) = 0;
    /// end duanxiaohui

	/*设置控件的对齐边界
	  @param rect 相对窗口四边框的收缩值
	        ={0,0,0,0} 表示相对窗口四边框
	        ={10,20,30,40}表示相对窗口左边框+10px,相对上边框+20px,相对右边框-30px,相对下边框-40px
	  @author:jianguakun 2012.8.28
	*/
	virtual void SetControlsShrinkArea(RECT& rect) = 0;
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
    /** @brief 获取当前选中的牌
     */
    virtual BYTE GetSelectCard()=0;
    /** @brief 获取当前选中的牌索引
    *  二维的  [x][y]
    */
    virtual POINT GetSelectIndex()=0;
    /** @brief 设置居中显示
    */
    virtual void SetAlignCenter(bool bCenter = true)=0;
    /** @brief 设置是否可拖动
    */
    virtual void SetCanDrag(bool bCan)=0;
    /** @brief 设置某列变灰
    */
    virtual void SetDarkRow(int nIndex)=0;
	/** 设置传入牌边框为蓝色
    */
	virtual void SetActiveBorder(BYTE iCard[], BYTE iCardCount[],int iCardLength ,DWORD iColor[]) = 0;
	/** @brief 设置移动图片路径
	*/
	virtual void SetMoveImagePath(TCHAR *path) = 0;
	/** @brief 设置动画路径
	*/
	virtual void SetAnimatePath(TCHAR *path) = 0;
	/** @brief 设置移动到牌上是否变灰色
	*/
	virtual void SetMoveDarkIndex(bool bMoveDark) = 0;
public:
    /** @brief Getter方法
    */
    //@{{
    virtual int GetVerInterval()=0;
    virtual const wchar_t *GetImagePath()=0;
    virtual void GetCardList(BYTE ucCard[][MAX_VER_COUNT], int nSize, int &nReadSize)=0;
    virtual int GetVerCutCount()=0;
    virtual bool GetCanSelectCard()=0;
    virtual bool GetCanDrag()=0;
	virtual const TCHAR *GetMoveImagePath()=0;
	virtual const TCHAR *GetAnimatePath()=0;
	virtual bool GetMoveDarkIndex()=0;
    //@}}
}; 
 #endif
