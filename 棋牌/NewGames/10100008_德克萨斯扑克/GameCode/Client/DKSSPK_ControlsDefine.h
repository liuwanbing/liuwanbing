#ifndef _CONTROLID_FOR_LOGIC_2uigamecomm_
#define _CONTROLID_FOR_LOGIC_2uigamecomm_
///这是给逻辑用的控件信息，程序员可以按自己的习惯改里面的宏
///文件名格式为：生成的相应的控件信息文件名+ “_”+ControlDefines 
///宏    +    ID    +  控件名字  + 控件类型  + 资源路径

/**************************************************************************************************************
*											 以下是德克萨斯扑克按钮											  *
***************************************************************************************************************/
const int DKSSPK_CONTAINER									= 1000000;					///<德克萨斯扑克容器
const int DZPK_GAME_BEGIN_BUT								= 1000001;					///<游戏开始按钮
const int DZPK_DAI_RU_MONEY_BUT								= 1000002;					///<带入金额按钮
const int DZPK_BET_BUT										= 1000003;					///<下注按钮
const int DZPK_CALL_BUT										= 1000004;					///<跟注按钮
const int DZPK_RAISE_BUT									= 1000005;					///<加注按钮
const int DZPK_CHECK_BUT									= 1000006;					///<过牌按钮
const int DZPK_FOLD_BUT										= 1000007;					///<弃牌按钮
const int DZPK_ALL_IN_BUT									= 1000008;					///<全下按钮

const int DZPK_MONEY_BUT									= 1000010;					///<五个金额按钮 * 5
const int DZPK_SHOW_CARD_BUT								= 1000015;					///<展示牌按钮	

const int DZPK_JSLIDER_BUT									= 1000020;					///<滑动条按钮
const int DZPK_ADD_MONEY									= 1000021;					///<加注(+)按钮
const int DZPK_SUB_MONEY									= 1000022;					///<减注(-)按钮
const int DZPK_JSLIDER_MAP									= 1000023;					///<滑动条底图

const int DZPK_CALL_TUO_GUAN_BUT							= 1000030;					///<跟注托管按钮
const int DZPK_CHECK_TUO_GUAN_BUT							= 1000031;					///<过牌托管按钮
const int DZPK_CALL_ANY_TUO_GUAN_BUT						= 1000032;					///<跟任何注托管按钮
const int DZPK_CHECK_FOLD_TUO_GUAN_BUT						= 1000033;					///<过牌、弃牌托管按钮

const int DZPK_GOU_QU_CALL_TUO_GUAN_BUT						= 1000040;					///<打钩跟注托管按钮
const int DZPK_GOU_QU_CHECK_TUO_GUAN_BUT					= 1000041;					///<打钩过牌托管按钮
const int DZPK_GOU_QU_CALL_ANY_TUO_GUAN_BUT					= 1000042;					///<打钩跟任何注托管按钮
const int DZPK_GOU_QU_CHECK_FOLD_TUO_GUAN_BUT				= 1000043;					///<打钩过牌、弃牌托管按钮

/**************************************************************************************************************
*											以下是德克萨斯扑克LOGO											  *
***************************************************************************************************************/
const int DZPK_USER_LOGO_CONTAINER							= 1000050;					///<用户头像容器
const int DZPK_USER_LOGO									= 1000060;					///<用户头像 * 8

const int DZPK_USER_NAME_CONTAINER							= 1000070;					///<用户昵称容器
const int DZPK_USER_MAP										= 1000080;					///<用户信息底图 * 8
const int DZPK_USER_NAME									= 1000090;					///<用户昵称 * 8
const int DZPK_USER_MONEY									= 1000100;					///<用户金额 * 8

const int DZPK_CARD_CONTAINER								= 1000110;					///<显示底牌容器
const int DZPK_SHOW_CARD									= 1000120;					///<显示底牌 * 8

const int DZPK_SIDE_POT_CONTAINER							= 1000130;					///<显示左下角边池容器
const int DZPK_SHOW_POT_DINAR								= 1000140;					///<显示左下角边池金币 * 8
const int DZPK_SHOW_SIDE_POT								= 1000150;					///<显示左下角边池 * 8

const int DZPK_DESKTOP_MONEY_CONTAINER						= 1000160;					///<桌面金币容器
const int DZPK_SHOW_DESKTOP_MONEY_LOGO						= 1000170;					///<桌面金币LOGO * 8
const int DZPK_SHOW_DESKTOP_MONEY							= 1000180;					///<桌面金币 * 8


const int DZPK_BET_CALL_RAISE_CHECK_FOLD_ALLIN_CONTAINER	= 1000190;					///<下跟加过弃全下容器					
const int DZPK_SHOW_BET_LOGO								= 1000200;					///<下注Logo显示 * 8
const int DZPK_SHOW_CALL_LOGO								= 1000210;					///<跟注Logo显示 * 8
const int DZPK_SHOW_RAISE_LOGO								= 1000220;					///<加注Logo显示 * 8
const int DZPK_SHOW_CHECK_LOGO								= 1000230;					///<过牌Logo显示 * 8
const int DZPK_SHOW_FOLD_LOGO								= 1000240;					///<弃牌Logo显示 * 8
const int DZPK_SHOW_ALLIN_LOGO								= 1000250;					///<全下Logo显示 * 8

const int DZPK_POT_CONTAINER								= 1000260;					///<桌面边池容器
const int DZPK_SHOW_BALANCE_POT_LOGO						= 1000270;					///<桌面边池结算显示 * 8
const int DZPK_SHOW_BALANCE_POT_MONEY						= 1000280;					///<桌面边池结算显示金币 * 8

const int DZPK_CHAT_CONTAINER								= 1000290;					///<聊天容器
const int DZPK_SHOW_CHAT_BUBBLE								= 1000300;					///<聊天气泡显示 * 8
const int DZPK_SHOW_CHAT_TYPE								= 1000310;					///<聊天气泡文字显示 * 8

const int DZPK_NT_DA_XIAO_CONTAINER							= 1000320;					///<庄大小盲注容器
const int DZPK_SHOW_NTUSER_IMAGE							= 1000330;					///<显示庄家 * 8
const int DZPK_SHOW_XIAO_IMAGE								= 1000340;					///<小盲注显示 * 8

const int DZPK_SHOW_CHAIR_CONTAINER							= 1000350;					///<玩家椅子容器
const int DZPK_SHOW_USER_CHAIR								= 1000360;					///<玩家椅子 * 8

const int DZPK_ZHUN_BEI_CONTAINER							= 1000370;					///<玩家准备容器
const int DZPK_READY_MAP									= 1000380;					///<玩家准备图片

const int DZPK_SHOW_GAME_HELP_CONTAINER						= 1000400;					///<新手教程容器
const int DZPK_SHOW_GAME_HELP								= 1000410;					///<新手教程 * 9

const int DZPK_SHOW_TOKEN_CONTAINER							= 1000500;					///<玩家令牌容器
const int DZPK_SHOW_TOKEN									= 1000510;					///<玩家令牌 * 8

const int DZPK_SHOW_USER_TIMER_CONTAINER					= 1000520;					///<玩家时间容器
const int DZPK_SHOW_USER_TIMER								= 1000530;					///<玩家时间 * 8

const int DZPK_SHOW_DA_IMAGE								= 1000540;					///<大盲注显示 * 8	

const int DZPK_SHOW_SMALL_CARD_CONTAINER					= 1000550;					///<小牌显示容器
const int DZPK_SHOW_SMALL_CARD								= 1000560;					///<小牌显示 * 8

const int DZPK_V_PAI_CONTAINER								= 1000570;					///<V牌容器
const int DZPK_V1_PAI										= 1000580;					///<V1牌 * 8
const int DZPK_V2_PAI										= 1000590;					///<V2牌 * 8

const int DZPK_PAI_TYPE_ANIMATION_CONTAINER					= 1000600;					///<牌型动画容器
const int DZPK_PAI_TYPE_ANIMATION							= 1000610;					///<牌型动画 * 8

const int DZPK_PAI_TYPE_MAP_CONTAINER						= 1000620;					///<牌型图片容器
const int DZPK_PAI_TYPE_MAP									= 1000630;					///<牌型图片 * 8

const int DZPK_SHOW_REAL_MONEY								= 1001000;					///<真实金币显示
const int DZPK_SHOW_REAL_DINAR								= 1001001;					///<真实金币显示
const int DZPK_SHOW_USER_THIS_ID							= 1001002;					///<玩家自身ID
const int DZPK_SHOW_ADDRESS									= 1001003;					///<地址IP显示
const int DZPK_SHOW_BASE_CARD								= 1001004;					///<桌面公共牌
const int DZPK_SHOW_ZHUANG_LOGO								= 1001005;					///<庄
const int DZPK_SHOW_SMALL_BLIND								= 1001006;					///<小盲注
const int DZPK_SHOW_BIG_BLIND								= 1001007;					///<大盲注
const int DZPK_SHOW_VICE_CARD								= 1001008;					///<一幅牌

const int DZPK_SHOW_KE_YONG_MONEY							= 1001009;					///<当前可用金额
const int DZPK_SHOW_READY_GO								= 1001010;					///<已准备Logo

const int DZPK_SHOW_PROMPT									= 1001011;					///<提示带入金额不足
const int DZPK_SHOW_CLCKTIME								= 1001012;					///<倒计时时钟
const int DZPK_SHOW_MONEYLACK								= 1001013;					///<玩家本身金额不足
const int DZPK_SHOW_IN_TIME									= 1001014;					///<带入定时器

const int DZPK_SHOW_TIMER_MAP								= 1001015;					///<玩家下注定时底图

const int DZPK_NT_MOVE										= 1001020;					///<庄移动控件
const int DZPK_SMALL_BLIND_MOVE								= 1001030;					///<小盲注移动控件
const int DZPK_BIG_BLIND_MOVE								= 1001040;					///<大盲注移动控件

const int DZPK_HIDE_NT_BEGIN								= 1001050;					///<庄隐藏开始控件
const int DZPK_HIDE_SMALL_BLIND_BEGIN						= 1001051;					///<小盲注隐藏开始控件
const int DZPK_HIDE_BIG_BLIND_BEGIN							= 1001052;					///<大盲注隐藏开始控件

const int DZPK_FA_PAI_ANIMATION								= 1001060;					///<发牌动画控件
const int DZPK_VICE_CARD_BEGIN								= 1001061;					///<一幅牌开始控件


const int DZPK_SHOW_BET_MONEY_CONTAINER						= 1001069;					///<下注金额容器
const int DZPK_SHOW_BET_MONEY								= 1001070;					///<下注金额
const int DZPK_SHOW_CALL_MONEY								= 1001071;					///<跟注金额
const int DZPK_SHOW_RAISE_MONEY								= 1001072;					///<加注金额
const int DZPK_SHOW_FIVE_MONEY								= 1001080;					///<五个金额 * 5

const int DZPK_CHIP_TOTAL_MAP								= 1001090;					///<筹码总图片
const int DZPK_MONEY_COLLECT								= 1001091;					///<金币收集地

const int DZPK_IN_MONEY_BU_ZU								= 1001092;					///<带入金额不足图片
const int DZPK_SELF_MONEY_BU_ZU								= 1001093;					///<自身金额不足图片


/**************************************************************************************************************
*										以下是德克萨斯扑克带入金额容器										  *
***************************************************************************************************************/
const int DZPK_DAI_RU_MONEY_CONTAINER						= 1001100;					///<带入金额容器
const int DZPK_OK_LOAD_BUT									= 1001110;					///<确定带入按钮
const int DZPK_EXIT_GAME_BUT								= 1001111;					///<退出游戏按钮
const int DZPK_ADD_BUT										= 1001112;					///<带入加按钮
const int DZPK_SUB_BUT										= 1001113;					///<带入减按钮
const int DZPK_OFF_BUT										= 1001114;					///<关闭带入金额容器
const int DZPK_TOTAL_MONEY_TEXT								= 1001115;					///<带入总金额数
const int DZPK_MAX_MONEY_TEXT								= 1001116;					///<带入最大金额数
const int DZPK_MIN_MONEY_TEXT								= 1001117;					///<带入最小金额数
const int DZPK_PLAN_MONEY_INPUT_TEXT						= 1001118;					///<带入多少金额数

/**************************************************************************************************************
*											以下是德克萨斯扑克结算容器										  *
***************************************************************************************************************/
const int DZPK_BALANCE_CONTAINER							= 1001500;					///<结算容器
const int DZPK_BALANCE_TOP									= 1001501;					///<结算框顶部
const int DZPK_BALANCE_ZHONG								= 1001510;					///<结算框中部
const int DZPK_BALANCE_BASE									= 1001520;					///<结算框底部
const int DZPK_WIN_USER_ID									= 1001530;					///<赢家ID
const int DZPK_GONG_WIN_MONEY								= 1001540;					///<共赢得多少金币
const int DZPK_LOSE_WIN_INFO								= 1001550;					///<输赢详情
const int DZPK_OFF_BALANCE_CONTAINER						= 1001560;					///<关闭结算容器
/**************************************************************************************************************
*											以下是德克萨斯扑克设置容器										  *
***************************************************************************************************************/
const int DZPK_SET_CONTAINER								= 1001600;					///<设置容器
const int DZPK_MUSIC_SELECT 								= 1001800;					///<声音钩选框
const int DZPK_KE_WATCH										= 1001801;					///<可旁观钩选框
const int DZPK_MUSIC_TEXT									= 1001603;					///<声音信息
const int DZPK_WATCH_TEXT									= 1001604;					///<可旁观信息
const int DZPK_SET_OK_BUT									= 1001605;					///<设置确定按钮
const int DZPK_SET_CANCEL_BUT								= 1001606;					///<设置取消按钮

/**************************************************************************************************************
*											以下是德克萨斯扑克动画容器										  *
***************************************************************************************************************/

const int DZPK_CHIP_RECYCLE_MOVE_CONTAINER					= 1002000;					///<筹码回收颁发移动容器
const int DZPK_CHIP_RECYCLE_MOVE							= 1002010;					///<筹码回收颁发移动控件

const int DZPK_MONEY_ISSUE_MOVE_CONTAINER					= 1002020;					///<金额回收颁发移动容器
const int DZPK_MONEY_ISSUE_MOVE								= 1002030;					///<金额回收颁发移动控件

const int DZPK_CHIP_MONEY_MAP								= 1002040;					///<筹码金额收放隐藏图片

const int DZPK_CHIP_MONEY_CONTAINER							= 1002050;					///<筹码金额颁发容器
const int DZPK_CHIP_MAP										= 1002060;					///<筹码颁发图片
const int DZPK_MONEY_NUM									= 1002070;					///<金额颁发数字

const int DZPK_FOLD_LOGO_CONTAINER							= 1002080;					///<玩家放弃头像图片容器
const int DZPK_FOLD_LOGO									= 1002090;					///<玩家放弃头像图片 * 8

const int DZPK_WIN_DONG_HUA_CONTAINER						= 1002100;					///<胜利动画容器
const int DZPK_WIN_DONG_HUA									= 1002110;					///<胜利动画 * 8

const int DZPK_SET_BUT										= 1003000;					///<设置按钮

const int DZPK_MOVE_COLLECT_CONTAINER						= 1003010;					///<庄大小盲注移动收集容器
const int DZPK_NT_MOVE_COLLECT								= 1003020;					///<庄移动收集
const int DZPK_XIAO_MOVE_COLLECT							= 1003030;					///<小盲注移动收集
const int DZPK_DA_MOVE_COLLECT								= 1003040;					///<大盲注移动收集

const int DZPK_NT_DA_XIAO_MOVE_CONTAINER					= 1003050;					///<庄大小盲移动图片容器
const int DZPK_NT_MOVE_MAP									= 1003060;					///<庄盲移动图片
const int DZPK_DA_XIAO_MOVE_MAP								= 1003070;					///<大小盲移动图片

const int DZPK_TIME_NUM										= 1003115;					///<初始带入显示时间

/**************************************************************************************************************
*											以下是德克萨斯扑看牌画容器										  *
***************************************************************************************************************/
const int DZPK_LOOK_CARD_ANIMA_0							= 1004000;					///<0视图位置看牌动画
const int DZPK_LOOK_CARD_ANIMA_1							= 1004001;					///<1视图位置看牌动画
const int DZPK_LOOK_CARD_ANIMA_2							= 1004002;					///<2视图位置看牌动画
const int DZPK_LOOK_CARD_ANIMA_3							= 1004003;					///<3视图位置看牌动画
const int DZPK_LOOK_CARD_ANIMA_5							= 1004005;					///<5视图位置看牌动画
const int DZPK_LOOK_CARD_ANIMA_6							= 1004006;					///<6视图位置看牌动画
const int DZPK_LOOK_CARD_ANIMA_7							= 1004007;					///<7视图位置看牌动画

#endif 
