// 用来定义界面编辑器编辑出来的控件列表中的ID号，其数值应当与界面编辑器中保持一致
// 部分由框架处理的ID号应当另行安排，可以复制到框架中代码中去，为保证框架的灵活性
// 不将此文件放到SDK目录下，而是放到具体游戏中
#ifndef _DEFINES_H
#define _DEFINES_H
#include "../GameMessage/UpGradeMessage.h"

// 下列为框架处理部分
#define DDZ_EXIT					10001						// 退出
#define DDZ_MIN_FRAME				10003						// 最小化游戏窗口	
#define DDZ_EXPRESSION				10008						// 表情
#define DDZ_LIAOTIAN				10009						// 聊天
#define DDZ_LIAOTIAN_LIEBIAO		10010						// 聊天内容选择框
#define DDZ_LIAOTIANJILU			10011						// 聊天记录
#define DDZ_FASONG					10013						// 发送按钮
#define DDZ_LABA					14003						// 大喇叭
#define DDZ_PLAY_MUSIC				154                         // 播放声音
#define DDZ_STOP_MUSIC				155                         // 停止声音
//#define DDZ_SOUND_CONFIG			155						    // 停止声音
#define DDZ_PHIZ					156							// 表情功能
#define DDZ_CLOSE_PHIZ				156							// 关闭表情
#define DDZ_NET_QUALITY				141                         // 网络品质
#define DDZ_CAPTION					103							// 窗口标题

#define SH_SINGLE_NOTE				501							// 底注
#define SH_TOP_NOTE					502							// 顶注

// 下列为游戏中自行处理部分

// 静态控件
#define SH_GAME_TITLE				103							// 游戏标题
//#define SH_NET_QUALITY			501							// 网络质量，由框架处理
#define SH_TOTAL_NOTE				504							// 总注
#define SH_MY_NOTE					505							// 下注框里的数值

//吹牛游戏

#define IDPIC_DLG_BG				800 //设置底注对话框的背景
#define IDB_CFG_DESK_BASE			801 //设置底注按钮

#define IDB_AGREE_DESK_CFG			808 //同意底注按钮
#define IDB_DISAGREE_DESK_CFG		809 //不同意底注按钮

#define IDC_DI_ZHU					1810//底注
#define IDC_DING_ZHU				1811//顶注

#define IDT_SHOW_BASE_CARD			506	//显示底牌
#define IDB_BURYCARD				507 //扣底牌
#define IDB_REVOLU					508 //我要革命按钮

#define IDT_GRADES					811 //牌级显示
#define IDT_NT_NAME					812	//庄家玩家名显示
#define IDT_RIOT_NT_NAME			813 //亮主玩家名显示
#define IDPIC_HT_HUAKIND_BASE		814 //当前打的花色显示
#define IDT_510K_POINTS				818 //当前得分
#define IDPIC_510K_LIST_BASE		821 //当前得分表

#define IDT_DI_ZHU					850	//底注
#define IDT_DING_ZHU				851	//顶注
#define IDPIC_WAIT_DIZHU			860 //等待底注的提示
#define IDPIC_WAIT_AGREE_DIZHU1		861 //等待同意底注提示
#define IDPIC_WAIT_AGREE_DIZHU2		862 //等待同意底注提示

#define IDVB_HIT_CARD				509 //虚拟点击牌控件
#define IDB_OUT_CARD				510 //出牌按钮					
#define IDB_CUE						511 //提示按钮
#define IDB_TUO_GUAN_ON				512 //游戏托管开按钮
#define IDB_TUO_GUAN_OFF			513 //游戏托管关按钮
#define IDM_DLBCLICK				514 //虚拟双击事件

#define IDPIC_SEL_HUA_KIND			527 //花色选择按钮框
#define IDB_SEL_HUA_KIND			528 //花色选择按钮
#define IDB_NEXT_ROUND_REQ			534 //下一回合开始
#define IDC_SEND_CARD				535 //发牌动画显示的牌控件

#define IDC_CLOCK_BASE				541 //时钟位置
#define IDC_PLAYER_LOGO_BASE		551	//玩家Logo
#define IDT_PLAYER_ID_BASE			561	//玩家ID
#define IDPIC_PLAYER_BG				571 //玩家底图
#define IDT_PLAYER_MONEY_BASE		581 //玩家点数
#define IDC_OUTCARD_BASE			591 //玩家手牌控件
#define IDPIC_COINT_BASE			601 //金币图标
#define IDC_JETON_PRE_BASE			611	//未掷筹码位置区域
#define IDC_JETON_BASE				621	//已掷筹码位置区域
#define IDC_CARD_POINT_VALUE_BASE	631 //牌面的点值
#define IDT_CARD_POINT_VALUE_BASE	1631//牌面的点值
#define IDPIC_TIPS_BASE				641 //提示
#define IDC_HANDCARD_BASE			651 //自己手上的牌
#define IDPIC_NT_FLAG_BASE			661 //庄家标记图
#define IDT_PLAYER_IP				671	//玩家IP所属地
#define IDC_BASE_SHOW				119 //底牌显示控件
#define IDB_LAST_ROUND				135 //上一轮显示控件


/*********************************************************************************************************
*										以下是德克萨斯扑克按钮											 *
**********************************************************************************************************/
#define	IDB_BEGIN_GAME				200		// 游戏开始按钮
#define IDB_ALTER_STAKE				201		// 修改注额按钮
#define IDB_BET						202		// 下注按钮
#define	IDB_CALL					203		// 跟注按钮
#define IDB_RAISE					204		// 加注按钮
#define IDB_CHECK					205		// 过牌按钮
#define IDB_FOLD					206		// 弃牌按钮
#define IDB_ALL_IN					207		// 全下
#define IDB_BUTTON_MONEY			211		// 金额按钮
#define IDB_SLIDING_AXLE			220		// 滑动轴
#define IDB_ADDMONEY				221		// 加注（+）按钮
#define IDB_REDUCE					222		// 加注（-）按钮
#define IDB_CALLMANDATE				230		// 跟注托管按钮
#define IDB_CHECKMANDATE			231		// 过牌托管按钮
#define IDB_CALLANYMANDATE			232		// 跟任何注按钮
#define IDB_PASSABANDONMANDATE		233		// 过牌/弃牌托管按钮
#define IDB_CALLMANDATE1			750		// 打钩跟注托管按钮
#define IDB_CHECKMANDATE1			751		// 打钩过牌托管按钮
#define IDB_CALLANYMANDATE1			752		// 打钩跟任何注按钮
#define IDB_PASSABANDONMANDATE1		753		// 打钩过牌/弃牌托管按钮

#define IDC_REAL_MONEY				235		// 真实金币显示
#define IDC_SHOW_BASE_CARD			236		// 桌面公共牌
#define IDC_DINAR_MAP				237		// 真实金币LOGO
#define IDC_REAL_DINAR				238		// 真实金币显示
#define IDC_ADDRESS					239		// 地址显示IP
#define IDC_USER_LOGO				241		// 用户LOGO * 8
#define IDC_USER_NAME				251		// 用户名称 * 8
#define IDC_UDER_ID					261		// 用户ID	* 8
#define IDC_USER_MONEY				271		// 用户金额 * 8
#define IDC_ZHUANG					304		// 庄
#define IDC_SMALL_BLIND				305		// 小盲注
#define IDC_BIG_BLIND				306		// 大盲注
#define IDC_VICE_CARD				307		// 一副牌
#define IDC_SHOW_IMAGE				311		// 显示位置
#define IDC_SHOW_CARD				331		// 显示底牌 * 8
#define IDC_POT_DINAR				341		// 显示边池金币 * 8
#define IDC_SIDE_POT				351		// 显示边池 * 8
#define IDC_JIN_BI					361		// 金币LOGO显示 * 8
#define IDC_TABLE_BOARD_MONEY		371		// 桌面金币 * 8
#define IDC_MONEY					381		// 金币显示 * 8
#define IDC_CALL					391		// 跟注显示 * 8
#define IDC_RAISE					401		// 加注显示 * 8
#define IDC_CHECK					411		// 过牌显示 * 8
#define IDC_FOLD					421		// 弃牌显示 * 8
#define IDC_ALLIN					431		// 全下显示 * 8
#define IDC_BET						441		// 下注显示 * 8
#define IDC_BALANCE_POT				451		// 边池结算显示 * 8
#define IDC_BALANCE_POT_MONEY		461		// 边池结算显示金币 * 8
#define IDC_SHOW_SMALL_CARD			471		// 小牌显示 * 8
#define IDC_CHAT_BUBBLE				481		// 聊天汽泡显示 * 8
#define IDC_CHAT_TYPE				491		// 聊天汽泡文字显示 * 8
#define IDC_SHOW_IMAGE1				501		// 小盲注显示
#define IDC_USER_BASE_MAP			511		// 玩家底图
#define IDC_USER_THIS_ID			520		// 玩家自身ID
#define IDC_USER_CHAIR				531		// 玩家椅子
#define IDC_MONEY_MAP				541		// 桌面金币图片
#define IDC_SHOW_CARDTYPE_NAME      550     // 牌型名称
#define IDPIC_BACK_CARD_BASE	    561     // 底牌显示区域（非自己的）
#define IDC_GAME_HELP				760		// 新手引导
#define IDC_READY_GO				780		// 已准备游戏
#define IDC_READY					790		// 准备图片
#define IDC_PROMPT					800		// 提示带入金额不足
#define IDC_CLCKTIME				801		// 倒计时时钟
#define IDC_MONEYLACK				830		// 玩家本身金额不足
#define IDT_MONEY_LACK				831		// 玩家本身金额不足定时器
#define IDC_SENDCARD_ANIM			841		// 发牌ID(841-860被占用)
#define IDC_YOU_WIN					840		// 你赢了图片 * 8
#define IDC_REFRESH_RECT            870     // 刷新区域
/*********************************************************************************************************
*											修改带入金额对话框											 *
**********************************************************************************************************/
#define IDB_OK_LOAD					600		// 确定代入
#define IDB_EXIT_GAME				601		// 退出游戏
#define IDB_ADD						602		// 加入金币
#define IDB_SUB						603		// 减除金币
#define IDB_STOP					604		// 关闭按钮

#define IDC_TOTAL_MONEY				630		// 总金额数
#define IDC_MAX_MONEY				631		// 最高金额数
#define IDC_MIN_MONEY				632		// 最小金额数
#define IDC_PLAN_MONEY				633		// 代入多少金额数

#define IDD_LOADSELECTION_DLG       "resources\\config\\Confirm.xml"        //修改注额对话框
/*********************************************************************************************************
*											结算对话框													 *
**********************************************************************************************************/
#define IDC_VICTORY_ID				701		// 赢家ID
#define IDC_WIN_MONEY				711		// 共赢得多少金币
#define IDC_INFO					721		// 输赢详情
#define IDC_SHUT					730		// 关闭结算按钮
#define IDC_DIBIAN                  740     // 游戏窗口底边图

#define IDD_FINISH_DLG				"resources\\config\\FinishDlg.xml"      //结算对话框
/*********************************************************************************************************
*											设置对话框													 *
**********************************************************************************************************/
#define IDB_SET_BUTTON				820		// 设置按钮
#define IDB_MUSIC_CLOSE				821		// 音效开关
#define IDB_BACKDROP_MUSIC			822		// 背景音乐开关
#define IDB_CHAT_SWITCH				823		// 聊天汽泡开关
#define IDB_CLOSE					824		// 关闭结算按钮
#define IDB_MAKE_SURE				825		// 确定设置按钮
#define IDB_CALL_OFF				826		// 取消设置按钮

#define IDD_SOUND_DLG				"resources\\config\\SoundDlg.xml"      //声音设置对话框
#define IDB_SOUND_CFG_OK			1010	//声音设置OK
/*********************************************************************************************************
*																										 *
**********************************************************************************************************/


#define IDPIC_QIPAO_BASE			2044 //汽泡提示背景
#define IDT_QIPAO_BASE			    2048 //汽泡提示

#define CN_FINISH_DLG				"resources\\config\\FinishDlg.xml"		// 结算框路径

#define IDT_FINISH_ID_BASE			301 //玩家ID
#define IDT_FINISH_WIN_LOSE_BASE	311 //玩家输赢情况
#define IDT_FINISH_POINTS_BASE		321 //玩家点数
#define IDT_FINISH_COINT_BASE		1331 //玩家金币
#define IDT_FINISH_RATE_INFO		901	//结算消息


// 消息提示对话框定义 
#define IDD_MSG_DLG_RES_CONFIRM		"resources\\config\\Confirm.xml"	// 确认对话框路径 
#define IDD_MSG_DLG_RES_SELECT		"resources\\config\\Select.xml"			// 选择对话框路径 
#define IDD_DLG_BTN_OK				10001	// 确定按钮
#define IDD_DLG_BTN_CANCLE			10005	// 取消按钮
#define IDD_DLG_TIP_LABLE			12345	// 标签控件

#endif

