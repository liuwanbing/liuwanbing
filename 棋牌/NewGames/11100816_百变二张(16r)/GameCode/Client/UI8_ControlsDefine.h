#ifndef _CONTROLID_FOR_LOGIC_UI8_
#define _CONTROLID_FOR_LOGIC_UI8_
///这是给逻辑用的控件信息，程序员可以按自己的习惯改里面的宏
///文件名格式为：生成的相应的控件信息文件名+ “_”+ControlDefines 
///宏    +    ID    +  控件名字  + 控件类型  + 资源路径

//const int CTN_1 = 1;  ///<教室  容器 
//const int CTN_1_Sudoku_2 = 2;  ///<边框  边框 
//const int CTN_1_IMG_3 = 3;  ///<游戏LOGO  图片 10003300\GameGrand\image_frame1.png
//const int CTN_1_IMG_4 = 4;  ///<最小化底框  图片 10003300\GameGrand\image_frame2.png
//const int CTN_1_BTN_10 = 10;  ///<最小化  按扭 10003300\GameGrand\Bn_game_zuixiao.png
//const int CTN_1_BTN_11 = 11;  ///<关闭  按扭 10003300\GameGrand\Bn_game_guanbi.png
//const int CTN_1_BTN_12 = 12;  ///<设置  按扭 10003300\GameGrand\Bn_game_shezhi.png
//const int CTN_1_TEXT_51 = 51;  ///<等级  静态文本 
//const int CTN_1_TEXT_61 = 61;  ///<帐号  静态文本 
//const int CTN_1_TEXT_70 = 70;  ///<显示右边昵称  静态文本 
//const int CTN_1_TEXT_71 = 71;  ///<显示昵称文字  静态文本 
//const int CTN_1_TEXT_72 = 72;  ///<显示左边昵称文字  静态文本 
//const int CTN_1_IMG_80 = 80;  ///<右边昵称底图  图片 10003300\GameGrand\Image_nicheng.png
//const int CTN_1_IMG_81 = 81;  ///<自己昵称底图  图片 10003300\GameGrand\Image_nicheng.png
//const int CTN_1_IMG_82 = 82;  ///<左边昵称底图  图片 10003300\GameGrand\Image_nicheng.png
//const int CTN_1_IMG_90 = 90;  ///<右边玩家头像  图片 client\log\Boy\gamelog0.bmp
//const int CTN_1_IMG_91 = 91;  ///<自己logo  图片 client\log\Boy\gamelog2.bmp
//const int CTN_1_IMG_92 = 92;  ///<左边玩家图像  图片 client\log\Boy\gamelog0.bmp
//const int CTN_1_TEXT_101 = 101;  ///<金币  静态文本 
//const int CTN_1_TEXT_111 = 111;  ///<元宝  静态文本 
//const int CTN_1_Talk_200 = 200;  ///<聊天框1  聊天框 
//const int CTN_1_ETEXT_201 = 201;  ///<聊天输入框  文本编辑框 
//const int CTN_1_BTN_202 = 202;  ///<发送  按扭 10003300\GameGrand\Bn_game_fasong.png
//const int CTN_1_BTN_203 = 203;  ///<表情  按扭 10003300\GameGrand\Bn_biaoqing.png
//
//const int CTN_1_CTN_250 = 250;  ///<每局任务  容器 
//const int CTN_1_CTN_250_TEXT_251 = 251;  ///<文字11  静态文本 
//
//const int CTN_1_CTN_401 = 401;  ///<设置面板  容器 
//const int CTN_1_CTN_401_BTN_402 = 402;  ///<关闭  按扭 10003300\GameGrand\set\Bn_set_guanbi.png
//const int CTN_1_CTN_401_BTN_403 = 403;  ///<确定  按扭 10003300\GameGrand\set\Bn_set_qr.png
//const int CTN_1_CTN_401_BTN_404 = 404;  ///<取消  按扭 10003300\GameGrand\set\Bn_set_quxiao.png
//const int CTN_1_CTN_401_RButton_406 = 406;  ///<普通话  单选框 
//const int CTN_1_CTN_401_RButton_407 = 407;  ///<单选框2  单选框 
//const int CTN_1_CTN_401_Slider_401 = 401;  ///<设置面板  滑动控件 
//const int CTN_1_CTN_401_RButton_410 = 410;  ///<单选框4  单选框 
//const int CTN_1_CTN_401_RButton_101011 = 101011;  ///<单选框3  单选框 


const   int  IMG_LUZI_BG        = 1000032  ;
const   int  LOGO_MYSELF        = 1000100  ;   ///自己logo

const   int  TEXT_NT_MONEY_NAME = 1000122 ; 
const   int  TEXT_PLAYER_MONEY_NAME = 1000130 ; 

//按钮类
const   int  BTN_LEAVE          = 1001100  ;   ///离开按钮
const   int	 BTN_RUBED_AUTO     = 1001200  ;   ///搓牌按钮
const   int	 BTN_RUBED_HAND     = 1001201  ;   ///搓牌按钮

const   int	 BTN_BANK_STORE     = 1001300  ;   ///存钱按钮
const   int	 BTN_BANK_ADOPT     = 1001400  ;   ///取钱按钮

const   int  BTN_CALL_NT        = 1001405  ; //叫庄
const   int  BTN_NO_CALL_NT     = 1001406 ;  //不叫庄

const   int	 BTN_NOTE_FINISH    = 1001500  ;   ///下注完成按钮

const   int	 BTN_APPLY_NT       = 1001600  ;   ///申请上庄按钮
const   int	 BTN_APPLY_DOWN_NT  = 1001601  ;   ///申请下庄按钮  


const   int	 BTN_NOTE_B         = 1001700  ;   ///100下注按钮
const   int	 BTN_NOTE_Q         = 1001701  ;   ///1000下注按钮
const   int	 BTN_NOTE_W         = 1001702  ;   ///10000下注按钮
const   int	 BTN_NOTE_SW        = 1001703  ;   ///100000下注按钮
const   int	 BTN_NOTE_BW        = 1001704  ;   ///1000000下注按钮
const   int	 BTN_NOTE_QW        = 1001705  ;   ///10000000下注按钮

const   int	 BTN_NTLIST_UP      = 1001800  ;   ///10000000下注按钮
const   int	 BTN_NTLIST_DOWN    = 1001801  ;   ///10000000下注按钮

const   int	 BTN_INFOLIST_LEFT  = 1001900 ;   ///10000000下注按钮
const   int	 BTN_INFOLIST_RIGHT = 1001901;   ///10000000下注按钮

const   int	 TIMER_IMG          =1002000  ;   ///时间背景图片
const   int	 TIMER_IMG_NUM      =1002001  ;   ///时间框图片

const   int	 HAND_TOP           =1002100  ;   ///上手图片
const   int	 HAND_RIGHT         =1002101 ;   ///右手图片
const   int	 HAND_BOTTOM        =1002102  ;   ///下手图片
const   int	 HAND_LEFT          =1002103  ;   ///左手图片

const   int	 SHAPE_TOP          =1002105  ;   ///牌型上图片
const   int	 SHAPE_RIGHT        =1002106  ;   ///牌型右图片
const   int	 SHAPE_BOTTOM       =1002107  ;   ///牌型下图片
const   int	 SHAPE_LEFT         =1002108  ;   ///牌型左图片

const   int	 CARD_STATIC_TOP    =1002200  ;   ///静态牌控件上
const   int	 CARD_STATIC_RIGHT  =1002201  ;   ///静态牌控件右
const   int	 CARD_STATIC_BOTTOM =1002202  ;   ///静态牌控件下
const   int	 CARD_STATIC_LEFT   =1002203  ;   ///静态牌控件左

const   int	 CARD_MOVE_TOP      =1002205  ;   ///动态牌控件上
const   int	 CARD_MOVE_RIGHT    =1002206  ;   ///动态牌控件右
const   int	 CARD_MOVE_BOTTOM   =1002207  ;   ///动态牌控件下
const   int	 CARD_MOVE_LEFT     =1002208  ;   ///动态牌控件左

///庄家信息
const   int	 STATIC_NT_NAME     =1002300  ;   ///庄家名称
const   int	 STATIC_NT_MONEY    =1002301  ;   ///庄家金币
const   int	 STATIC_NT_JS       =1002302  ;   ///庄家局数
const   int	 STATIC_NT_CJ       =1002303  ;   ///庄家成绩

///玩家信息
const   int	 STATIC_PLAYER_NAME   =1002310  ;   ///玩家姓名
const   int	 STATIC_PLAYER_MONEY  =1002311  ;   ///玩家金币
const   int	 STATIC_PLAYER_CJ     =1002312  ;   ///玩家成绩

///庄家列表信息
const   int	 STATIC_NTLIST_NAME_1   =1002320  ;   ///庄家列表中第一个玩家的名字
const   int	 STATIC_NTLIST_NAME_2   =1002321  ;   ///庄家列表中第二个玩家的名字
const   int	 STATIC_NTLIST_NAME_3   =1002322  ;   ///庄家列表中第三个玩家的名字

const   int	 STATIC_NTLIST_MONEY_1  =1002323  ;   ///庄家列表中第一个玩家的钱
const   int	 STATIC_NTLIST_MONEY_2  =1002324  ;   ///庄家列表中第二个玩家的钱
const   int	 STATIC_NTLIST_MONEY_3  =1002325  ;   ///庄家列表中第三个玩家的钱

///历史信息百人比
const   int	 STATIC_HISTORY_PERCENT_SM  = 1002400  ;   ///上门赢的白分比
const   int	 STATIC_HISTORY_PERCENT_DAOM =1002401  ;   ///倒门赢的百分比
const   int	 STATIC_HISTORY_PERCENT_DUIM =1002402  ;   ///对门赢的百分比

///玩家下注信息
const   int	 STATIC_MYSELF_NOTE_TOTLE    = 1002405  ;   ///自己下的总注 
const   int	 NUM_ALLUSER_NOTE_TOTLE      =1002406  ;   ///所有玩家下的总注 

//通吃和通赔图片
const   int  IMG_WIN_ALL_RECT            =1002411 ; 
const   int  IMG_LOST_ALL_RECT           =1002412 ; 

//等待玩家叫庄图片
const   int   IMAGE_NOTE_FULL            = 1002417 ; 
const   int   IMAGE_WAIT_CALL_NT         = 1002418 ;

///选中框按钮1002500 -- 1002506
const   int    BTN_CHOOSE_RECT           = 1002500 ;
///下注图片框1002600 -- 1002606
const   int    IMG_NOTE_KUANG             = 1002600 ;
///下注筹码图片1002700 -- 1002706
const   int    IMG_NOTE_CHOUMA            = 1002700 ;

///自己下注金额数字1002800 --1002806
const   int    TEXT_MY_NOTE_NUM           = 1002800 ;
///自己下注金额数字1002900 --1002906
const   int    TEXT_ALL_NOTE_NUM           = 1002900 ;

//结算框
const   int    CONTAIN_RESULT                = 1003000;
const   int    BTN_RESULT_CLOSE              = 1003001;
const   int	   IMG_RESULT_BG                  =1003002;

const   int		STATIC_USER_SCORE          =1003100;   ///玩家结算
const   int		STATIC_USER_RETURN_SCORE   =1003101;   ///玩家返还
const   int		STATIC_NT_SCORE            =1003102;   /// 庄家结算

const   int     MOVE_ACTION_CONTROL          = 1003124 ;
const   int     NO_MOVE_ANIMAL_CARD          = 1003125;
const   int     NO_MOVE_CARD_POS             = 1003126 ;


///外部动画色子动画
const   int     EXT_ANIMAL_DICE_LEFT          =1003301; 
const   int     EXT_ANIMAL_DICE_RIGHT         =1003302;

///洗牌动画
const   int		EXT_ANIMAL_XIPAI              = 1003600;
///摊牌动画
const    int    EXT_ANIMAL_TANPAI             = 1003601 ; 

//强制离开信息栏
const   int	    CONTAIN_FORCE_LEAVE_INFO      = 1003400 ; 
const   int     STATIC_FORCE_LEAVE_MSG        = 1003401 ;
const   int		 BTN_FORCE_LEAVE_OK           = 1003402 ; 
const   int      STATIC_LEAVE_NUM             = 1003403 ;
//强制离开信息栏 the end
const   int      BTN_SET_GAME_INFO            = 1003500 ;

///游戏设置相关
const   int      CONTAIN_GAME_SET             = 1003510 ; 
const   int      BTN_SET_VOICE                = 1003520 ; 
const   int      BTN_SET_WATCH                = 1003530 ; 
const   int      BTN_SET_GAME_OK              = 1003540 ; 
const   int      BTN_SET_GAME_CANCEL          = 1003550 ; 

///超端相关设置

const   int      BTN_SM_WIN                    =1004001 ; //顺门赢 
const   int      BTN_SM_LOST                   =1004004 ; ///顺门输
const	int		 TEXT_PROMPT				   =1004008;  ///提示语句
const   int      BTN_SUPER_OK                  = 1004011 ;
const   int      BTN_SUPER_CANCEL              = 1004012 ;
const   int      BTN_SUPER_CLOSE               = 1004013 ;
const   int      CONTAIN_SUPER_SET             =1004015 ; 
const   int      BTN_SHOW_ROBOT_ZHU            = 1004016 ;	//屏蔽机器人下注

const	int		 CONTAIN_CARD_0				   =1004100;	//牌0容器
const	int		 CONTAIN_CARD_1				   =1004101;	//牌1容器
const	int		 CONTAIN_CARD_2				   =1004102;	//牌2容器
const	int		 CONTAIN_CARD_3				   =1004103;	//牌3容器

const	int		 IMG_CARD_0						=1004104;	//牌0容器
const	int		 IMG_CARD_1						=1004105;	//牌1容器
const	int		 IMG_CARD_2						=1004106;	//牌2容器
const	int		 IMG_CARD_3						=1004107;	//牌3容器

///3000000-30000100路子ID占用
const   int     IMG_LUZI_BASE_ID               = 3000000 ; 
///1003300-1003400路子ID占用


const  int  CTN_APPLY_NT_DLG     = 5003012 ; //申请上庄对话框

const  int  BTN_APPLY_NT_OK      = 5003022 ; //同意

const  int  BTN_APPLY_NT_OPPOSE  = 5003023 ; //不同意
#endif ///< _CONTROLID_FOR_LOGIC


