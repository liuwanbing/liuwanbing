#pragma once
#define     GAME_ID_START                  1000000

#define     PLAYER_LOGO0                  1000100

//结算框
#define     RESULT_WND                    1001000 //结算框
#define     RESULT_NAME                   1001001 // 结算昵称1

#define		RESULT_NOTE_MONEY			  800200023	//下注的金币
#define		RESULT_WIN_MONEY			  800200029 //赢得的金币
#define		RESULT_NET_INCOME_MONEY_US	  800200035	//正净收入
//#define		RESULT_NET_INCOME_MONEY_S	  800200041	//负净收入
										  
#define     RESULT_FEN                    1001011 // 结算积分1
#define     RESULT_WIN                    1001021 // 结算胜1
#define     RESULT_LOSE                   1001031 // 结算败1
#define     RESULT_MONEY                  1001041 // 结算金币1

#define     RESULT_CONTINUE               1001100 // 结算继续
#define     RESULT_LEAVE                  1001200 // 结算离开



//手牌
#define     CARD_HAND_0                   1002000 //0号 0的手牌
#define     CARD_HAND_1                   1002001 //1号 1的手牌
#define     CARD_HAND_2                   1002002 //2号（自已）的手牌
#define     CARD_HAND_3                   1002003 //3号 3的手牌



//游戏房间相关信息
#define IDD_ZX_SMALL_BOBO_VALUE     1002100   //最小簸簸(最小要带进的钱)
#define IDD_ZX_DI_BI_VALUE			1002101   //底皮(底注)

#define IDD_ZJH_USER_NAME           1002105		//用户名
#define IDD_ZJH_USER_POINT          1002106		//余额
#define IDD_ZJH_USER_LEFT_MONEY     1002107		//还有多少金额可用

//玩家属性
#define IDD_PLAY_BG_0               1002200
#define IDD_PLAY_MONEY_0            1002220
#define IDD_PLAY_USERID_0           1002210

#define IDD_PLAY_BG_1               1002201
#define IDD_PLAY_MONEY_1            1002221
#define IDD_PLAY_USERID_1           1002211

#define IDD_PLAY_BG_2               1002202
#define IDD_PLAY_MONEY_2            1002222
#define IDD_PLAY_USERID_2           1002212

#define IDD_PLAY_BG_3               1002203
#define IDD_PLAY_MONEY_3            1002223
#define IDD_PLAY_USERID_3           1002213

#define IDD_PLAY_BG_4               1002204
#define IDD_PLAY_MONEY_4            1002224
#define IDD_PLAY_USERID_4           1002214

#define IDD_PLAY_BG_5               1002205
#define IDD_PLAY_MONEY_5            1002225
#define IDD_PLAY_USERID_5           1002215





#define     IDD_START_BT                      1003020 //开始按钮
#define     IDD_SOUND_OPEN_BT                 1003021 //打开音效
#define     IDD_SOUND_CLOSE_BT                1003022 //关闭音效

//庄控件
#define IDD_CTR_PLAY_NT_0             1003100
#define IDD_CTR_PLAY_NT_1             1003101
#define IDD_CTR_PLAY_NT_2             1003102
#define IDD_CTR_PLAY_NT_3             1003103
#define IDD_CTR_PLAY_NT_4             1003104
#define IDD_CTR_PLAY_NT_5             1003105

//---------------------------提示信息---------------------


//计时器背景
#define IDD_CTR_CLOCK_BG_0         1003200
#define IDD_CTR_CLOCK_BG_1         1003201
#define IDD_CTR_CLOCK_BG_2         1003202
#define IDD_CTR_CLOCK_BG_3         1003203
#define IDD_CTR_CLOCK_BG_4         1003204
#define IDD_CTR_CLOCK_BG_5         1003205
//闹钟控件
#define IDD_CTR_PLAY_CLOCK_0       1003210
#define IDD_CTR_PLAY_CLOCK_1       1003211
#define IDD_CTR_PLAY_CLOCK_2       1003212
#define IDD_CTR_PLAY_CLOCK_3       1003213
#define IDD_CTR_PLAY_CLOCK_4       1003214
#define IDD_CTR_PLAY_CLOCK_5       1003215


#define IDD_SET_THING_BTN               1003500 ///设置按钮

#define IDD_SET_THING_DLG               1003510 /// 设置框

#define IDD_SET_THING_SOUND             1003520 /// 设置框音效单选

#define IDD_SET_BG_SOUND				800200096 //设置背景音乐

#define IDD_SET_THING_OK_BTN            1003540 /// 设置框OK

#define IDD_SET_THING_CANCEL_BTN        1003550 /// 设置框Cancel



#define IDD_CM_SHCTRL_CENTER            1003700 //中心筹码控件
#define IDD_CM_SHCTRL_PLAY_NOTE         1003701 //玩家下注筹码控件
#define IDD_CM_SHCTRL_PLAY_NOTE_1       1003702 //玩家下注筹码控件
#define IDD_CM_SHCTRL_PLAY_NOTE_2       1003703 //玩家下注筹码控件
#define IDD_CM_SHCTRL_PLAY_NOTE_3       1003704 //玩家下注筹码控件
#define IDD_CM_SHCTRL_PLAY_NOTE_4       1003705 //玩家下注筹码控件
#define IDD_CM_SHCTRL_PLAY_NOTE_5       1003706 //玩家下注筹码控件

#define IDD_MOV_CONTROL_0                1003800 //移动推控件
#define IDD_MOV_CONTROL_1               1003801 //移动推控件
#define IDD_MOV_CONTROL_2               1003802 //移动推控件
#define IDD_MOV_CONTROL_3               1003803 //移动推控件
#define IDD_MOV_CONTROL_4               1003804 //移动推控件
#define IDD_MOV_CONTROL_5               1003805 //移动推控件
#define IDD_MOV_CENTER                  1003806 //收集中心筹码动画

#define IDD_MOV_SEND_CARD               1003820 //发牌动画
#define IDD_SEND_CARD					1003830	 //发牌动画的牌

//==================================================================================
#define IMG_READY_MARK_0				800200008	//准备
#define IMG_READY_MARK_1				800200009	//准备
#define IMG_READY_MARK_2				800200010	//准备
#define IMG_READY_MARK_3				800200011	//准备
#define IMG_READY_MARK_4				800200012	//准备
#define IMG_READY_MARK_5				800200013	//准备
#define IMG_READY_MARK_6				800200014	//准备

//==================================================================================
//牌型图片
#define IMG_CARD_SHAP_LEFT_0			100210719	//玩家00 左边牌型图片
#define IMG_CARD_SHAP_LEFT_1			100210720	//玩家01 左边牌型图片	
#define IMG_CARD_SHAP_LEFT_2			100210721	//玩家02 左边牌型图片	
#define IMG_CARD_SHAP_LEFT_3			100210722	//玩家03 左边牌型图片	
#define IMG_CARD_SHAP_LEFT_4			100210723	//玩家04 左边牌型图片	
#define IMG_CARD_SHAP_LEFT_5			100210724	//玩家05 左边牌型图片	

#define IMG_CARD_SHAP_RIGHT_0			100210731	//玩家00 右边牌型图片
#define IMG_CARD_SHAP_RIGHT_1			100210732	//玩家01 右边牌型图片	
#define IMG_CARD_SHAP_RIGHT_2			100210733	//玩家02 右边牌型图片	
#define IMG_CARD_SHAP_RIGHT_3			100210734	//玩家03 右边牌型图片	
#define IMG_CARD_SHAP_RIGHT_4			100210735	//玩家04 右边牌型图片	
#define IMG_CARD_SHAP_RIGHT_5			100210736	//玩家05 右边牌型图片	


#define IMG_CARD_SHAP_RIGHT_30			800200016	//临时显示牌型
//下注提示图牌
#define	 IMG_OPER_TYPE_00				100210701	//玩家00 玩家操作提示
#define	 IMG_OPER_TYPE_01				100210702	//玩家01 玩家操作提示	
#define	 IMG_OPER_TYPE_02				100210703	//玩家02 玩家操作提示	
#define	 IMG_OPER_TYPE_03				100210704	//玩家03 玩家操作提示	
#define	 IMG_OPER_TYPE_04				100210705	//玩家04 玩家操作提示	
#define	 IMG_OPER_TYPE_05				100210706	//玩家05 玩家操作提示	

//筹码按键
#define  BNT_NOTE_10					100200001	//筹码10
#define  BNT_NOTE_100					100200002	//筹码100
#define  BNT_NOTE_1000					100200003	//筹码1000
#define  BNT_NOTE_1W					100200004	//筹码1W
#define  BNT_NOTE_10W					100200005	//筹码10W
#define  BNT_NOTE_100W					100200006	//筹码100W
#define  BNT_NOTE_1000W					100200007	//筹码1000W

//簸簸按扭,分牌按钮,重新下注按扭
#define  BNT_BOBO						800200005	//簸簸按扭
#define	 BNT_SEPARATE_CARD				800200006	//分牌按扭
#define  BNT_RESET_NOTE					800200007	//重新下注按钮

//跟,敲,大,休,丢
#define BNT_FOLLOW						100000001	
#define BNT_SET_ALL						100000002
#define BNT_BIG							100000003
#define BNT_STOP						100000004
#define BNT_GIVE_UP						100000005

#define OPERATOR_CARD_ONE				100210707		//可操作牌控件1
#define OPERATOR_CARD_TWO				800200014		//可操作牌控件2
#define IMG_BOTTOM_OPERATOR_CARD		800200015		//图牌底图

#define SEPARAT_CARD_LEFT_00			100210725		//分牌后的左边牌
#define SEPARAT_CARD_LEFT_01			100210726		//分牌后的左边牌
#define SEPARAT_CARD_LEFT_02			100210727		//分牌后的左边牌
#define SEPARAT_CARD_LEFT_03			100210728		//分牌后的左边牌
#define SEPARAT_CARD_LEFT_04			100210729		//分牌后的左边牌
#define SEPARAT_CARD_LEFT_05			100210730		//分牌后的左边牌

#define SEPARAT_CARD_RIGHT_00			100210713		//分牌后的右边牌
#define SEPARAT_CARD_RIGHT_01			100210714		//分牌后的右边牌
#define SEPARAT_CARD_RIGHT_02			100210715		//分牌后的右边牌
#define SEPARAT_CARD_RIGHT_03			100210716		//分牌后的右边牌
#define SEPARAT_CARD_RIGHT_04			100210717		//分牌后的右边牌
#define SEPARAT_CARD_RIGHT_05			100210718		//分牌后的右边牌

#define IMG_OK_00						800200017		//OK图片00
#define IMG_OK_01						800200018		//OK图片01
#define IMG_OK_02						800200019		//OK图片02
#define IMG_OK_03						800200020		//OK图片03
#define IMG_OK_04						800200021		//OK图片04
#define IMG_OK_05						800200022		//OK图片05

#define IMGNUM_NOTE_MONEY				800200023		//下注金币
#define IMGNUM_WIN_MONEY				800200029		//赢得金币
#define IMGNUM_NET_INCOME_MONEY_A		800200035		//净收入


#define		IMG_NOTE_MOVE_START_00		  800200041		//筹码移动的起始坐标
#define		IMG_NOTE_MOVE_END_00		  800200062		//筹码移动的终点坐标
#define		IMG_MOVE_NOTE_00			  800200047		//移动的筹码

#define		IMG_NOTE_PLAY_ONE_00		  800200059		//底皮筹码图片
#define		IMG_NOTE_PLAY_TWO_00		  800200060		//第一次下注筹码图片
#define		IMG_NOTE_PLAY_THR_00		  800200061		//第二次下注筹码图片

#define		INUM_NOTE_VALUE				  800200083		//显示玩下要下注数

#define		IMG_ALREADY_NOET			  800200077		//已经下注的提示图片
#define		TXT_ALL_NOTE				  800200084		//玩家已下注

#define		IMP_GIVE_UP_BACK_CARD		  800200090		//放弃图牌图片



