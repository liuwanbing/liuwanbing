#ifndef _CONTROLID_FOR_LOGIC_UI8_
#define _CONTROLID_FOR_LOGIC_UI8_
///这是给逻辑用的控件信息，程序员可以按自己的习惯改里面的宏
///文件名格式为：生成的相应的控件信息文件名+ “_”+ControlDefines 
///宏    +    ID    +  控件名字  + 控件类型  + 资源路径

///游戏结算框信息
const   int  BTN_MING_START      =  1000001 ; 


///游戏设置相关
const   int      CONTAIN_GAME_SET             = 1000380 ; 
const   int      BTN_SET_VOICE                = 1000300 ; 
const   int      BTN_SET_BG_VOICE             = 1000310 ; 
const   int      BTN_SET_WATCH                = 1000320 ; 
const   int      BTN_SET_GAME_OK              = 1000330 ; 
const   int      BTN_SET_GAME_CANCEL          = 1000340 ; 
const   int      BTN_SET_GAME_INFO            = 1003500 ;
///游戏设置相关

///游戏结算内容
const   int  CONTAIN_RESULT       = 1001000 ; 

const   int  TEXT_NAME_0          = 1001001 ; //玩家昵称
const   int  IMAGE_ADD_DOUBLE_0   = 1001010 ; //加倍
const   int  IMAGE_WIN_LOST_0     = 1001020 ; //胜利和失败
const   int  NUM_USER_SCORE_0     = 1001030 ;  //玩家得分
const   int  EXT_ANI_FARMER_WIN  = 1001050 ;   ///农民胜利
const   int  EXT_ANI_FARMER_LOST = 1001051 ;   ///农民输
const   int  EXT_ANI_OWNER_WIN   = 1001052 ;   ///地主赢
const   int  EXT_ANI_OWNER_LOST  = 1001053 ;   ///地主输

const   int  BTN_RESULT_QD       = 1040354 ; 
const   int  IMAGE_NT_FLAG       = 1040355 ; 

const   int  IMAGE_GAME_MUTIPLE= 1040343 ;   ///游戏倍数图片
const   int  NUM_GAME_MUTIPLE  = 1040477 ;   ///游戏数字
const   int  NUM_PUBLIC_POINT  = 1040363 ; 

const   int  TEXT_PRIMARY_MUTI = 1040364 ; 
const   int  TEXT_ROB_NT_MUTI  = 1040365 ; 
const   int  TEXT_MING_MUTI    = 1040366 ; 
const   int  TEXT_BACK_MUTI    = 1040367 ; 
const   int  TEXT_BOMB_MUTI    = 1040368 ; 
const   int  TEXT_SPRING_MUTI  = 1040369 ; 
const   int  TEXT_TASK_MUTI    = 1040370 ; 
const   int	 IMAGE_MYSELF_BG   = 1001054 ;    //自己的地图框
///结算框内容

const   int  TEXT_TASK_NAME  = 1031000 ;  ///任务名称
const   int  IMG_TASK_FINISH = 1031001 ;    ///任务完成

///动画
const   int	 EXT_BOMB_ANI        = 1040484 ; 
const   int  EXT_FIRE_ANI        = 1040485 ; 
const   int  EXT_ADD_DOUBLE_ANI  = 1040486 ;
const   int  EXT_STRAINT_ANI_0   = 1040487 ; 
const   int  EXT_BOMB_ANI_0      = 1040490 ; 
const   int  EXT_ANI_TASK        = 1040493 ;   ///任务动画
const   int  EXT_WARNING_ANI_0   = 1040494 ;   ///报警动画  
const   int  EXT_ANI_MING_PAI_0  = 1040497 ; ///明牌动画
const   int	 EXT_PLANE_ANI       = 1040500 ; ///飞机动画
const   int	 EXT_ROCKET_ANI      = 1040501 ; //火箭动画

const   int  EXT_LIANDUI_ANI     = 1040600 ; ///连对
const   int  EXT_NO_BIG_CARD     = 1040603 ; ///没有牌打过上家
const   int  EXT_ONLY_TWO_CARD   = 1040605 ; ///只剩一张牌

const   int  EXT_NOT_RULE         = 1040610 ;
const   int  EXT_NOT_ENOGH_MONEY  = 1040611 ; 
const   int  EXT_NT_NOT_ENOGH_MONEY =1040612 ; 

///玩家游戏倍数内容
const   int  CTN_MUTIPLE_CONTENT  = 1040502 ;

const   int  TEXT_PRIMATE_MUTIPLE = 1040503 ; 
const   int  TEXT_MING_MUTIPLE    = 1040504 ; 
const   int  TEXT_ROB_MUTIPLE     = 1040505 ;

const	int  TEXT_BASE_CARD_MUTIPLE = 1040506 ; 
const   int  TEXT_BOMB_MUTIPLE      = 1040507 ; 
const   int  TEXT_SPRINT_MUTIPLE    = 1040508 ; 
const   int  TEXT_TASK_MUTIPLE      = 1040509 ;

const   int  TEXT_NICK_NAME_0   = 1040510 ; 
const   int  TEXT_NICK_NAME_1   = 1040511 ; 
const   int  TEXT_NICK_NAME_2   = 1040512 ; 

const   int  TEXT_ADD_MUTIPLE_0   = 1040513 ; 
const   int  TEXT_ADD_MUTIPLE_1   = 1040514 ; 
const   int  TEXT_ADD_MUTIPLE_2   = 1040515 ; 

const   int  TEXT_PLAYER_NAME_0   = 1040516 ; 
const   int  TEXT_PLAYER_NAME_1   = 1040517 ; 
const   int  TEXT_PLAYER_NAME_2   = 1040518 ;

const   int  TEXT_PUBLIC_MUTIPLE_0   = 1040519 ; 
const   int  TEXT_PUBLIC_MUTIPLE_1   = 1040520 ; 
const   int  TEXT_PUBLIC_MUTIPLE_2   = 1040521 ;

const   int  TEXT_ADD_RESULT_0   = 1040522 ; 
const   int  TEXT_ADD_RESULT_1   = 1040523 ; 
const   int  TEXT_ADD_RESULT_2   = 1040524 ;

const   int  TEXT_NT_ADD_RESULT_0   = 1040525 ; 
const   int  TEXT_NT_ADD_RESULT_1   = 1040526 ; 
const   int  TEXT_NT_ADD_RESULT_2   = 1040527 ;
const   int  TEXT_NT_ADD_RESULT_3   = 1040528 ;

const   int  TEXT_MUTIPLE_RESULT_0   = 1040529 ; 
const   int  TEXT_MUTIPLE_RESULT_1   = 1040530 ; 
const   int  TEXT_MUTIPLE_RESULT_2   = 1040531 ;
///玩家游戏倍数end

const   int  IMAGE_READY_0        = 1001500 ; 
const   int  IMAGE_READY_1        = 1001501 ; 
const   int  IMAGE_READY_2        = 1001502 ; 

///玩家的手牌
const   int  HAND_CARD_0          = 1002000 ; 
const   int  HAND_CARD_1          = 1002001 ; 
const   int  HAND_CARD_2          = 1002002 ; 
const   int  BACK_CARD            = 1002003 ; 
       
const   int  DESK_CARD_0          = 1002100 ;
const   int  DESK_CARD_1          = 1002101 ;
const   int  DESK_CARD_2          = 1002102 ;

const   int  IMG_SCORE_0          = 1002203 ; 
const   int  IMG_SCORE_1          = 1002204 ; 
const   int  IMG_SCORE_2          = 1002205 ; 

///出牌按钮
const   int  BTN_OUT_CARD         = 1003000 ; 
const   int  BTN_PASS             = 1003001 ;
const   int  BTN_CUE              = 1003002 ;

///功能按钮
const   int  BTN_ATUO              = 1003005 ; 
const   int  BTN_UNATUO            = 1003006 ; 
const   int  BTN_SORT_CARD         = 1003007 ; 
const   int  BTN_LAST_TURN         = 1003008 ; 

///叫分按钮
const   int  BTN_CALL_SCORE_0     = 1003013 ;    
const   int  BTN_CALL_SCORE_1     = 1003014 ;    
const   int  BTN_CALL_SCORE_2     = 1003015 ;    
const   int  BTN_CALL_SCORE_3     = 1003016 ;  

const   int   BTN_NOT_ROB_NT      = 1003017 ; 
const   int   BTN_ROB_NT          = 1003018 ; 

///明牌
const   int  BTN_SHOW_CARD        = 1003021 ; 

const   int  BTN_NOT_ADD_DOUBLE   = 1003030 ; 
const   int  BTN_ADD_DOUBLE       = 1003031 ; 

const   int	 IMG_ADD_FLAG_0       = 1003032 ;
const   int	 IMG_ADD_FLAG_1       = 1003033 ;
const   int	 IMG_ADD_FLAG_2       = 1003034 ;


const   int  IMAGE_PASS_0         = 1004010 ; 
const   int  IMAGE_PASS_1         = 1004011 ; 
const   int  IMAGE_PASS_2         = 1004012 ; 

const   int  IMG_NT_FLAG_0        = 1004020 ; 
const   int  IMG_NT_FLAG_1        = 1004021 ; 
const   int  IMG_NT_FLAG_2        = 1004022 ; 

const   int  IMG_AUTO_FLAG_0      = 1004030 ; 
const   int  IMG_AUTO_FLAG_1      = 1004031 ; 
const   int  IMG_AUTO_FLAG_2      = 1004032 ; 

const   int  IMG_ROB_NT_0         = 1004033 ; 
const   int  IMG_ROB_NT_1         = 1004034 ; 
const   int  IMG_ROB_NT_2         = 1004035 ; 

const   int  IMG_SHOW_CARD_0      = 1004036 ; 
const   int  IMG_SHOW_CARD_1      = 1004037 ; 
const   int  IMG_SHOW_CARD_2      = 1004038 ; 

const int CTN_1000000_ExAnimate_1004040 = 1004040;  ///<右侧玩家手牌  外部动画控件 
const int CTN_1000000_ExAnimate_1004042 = 1004042;  ///<左侧玩家手牌  外部动画控件 

///游戏中的任务
const   int  IMG_BACK_SHAPE  = 1040480 ;    ///底牌的牌型
const   int  IMG_BACK_POINT  = 1040481 ;   ///底牌的倍数






//剩余牌的张数
const   int  CTN_LAEVE_COUNT_0    = 1051000 ;
const   int  INUM_LEAVE_COUNT_0   = 1051010 ; 
const   int  IMG_LEAVE_COUNT_0    = 1051020 ; 



///定时器
const   int	 TIMER_IMG_NUM        =  1004000  ;   ///时间框图片
const   int	 TIMER_CONTAIN        =  1005000  ;   ///时间背景图片


#endif ///< _CONTROLID_FOR_LOGIC


