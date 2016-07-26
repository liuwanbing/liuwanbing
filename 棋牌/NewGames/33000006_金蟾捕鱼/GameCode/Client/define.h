#pragma once
#define     GAME_ID_START                  1000000

#define     PLAYER_LOGO0                  1000100

//结算框
#define     RESULT_WND                    1001000 //结算框
#define     RESULT_NAME                   1001001 // 结算昵称1
#define     RESULT_FEN                    1001011 // 结算积分1
#define     RESULT_WIN                    1001021 // 结算胜1
#define     RESULT_LOSE                   1001031 // 结算败1
#define     RESULT_MONEY                  1001041 // 结算金币1
#define     RESULT_CONTINUE               1001100 // 结算继续
#define     RESULT_LEAVE                  1001200 // 结算继续



//手牌
#define     CARD_HAND_0                   1002000 //0号 0的手牌
#define     CARD_HAND_1                   1002001 //1号 1的手牌
#define     CARD_HAND_2                   1002002 //2号（自已）的手牌
#define     CARD_HAND_3                   1002003 //3号 3的手牌



//游戏房间相关信息
#define IDD_ZJH_GUODI_VALUE         1002100   //锅底值
#define IDD_ZJH_MAXLIMIT_VALUE      1002101   //上限值
#define IDD_ZJH_DIZHU_VALUE         1002102   //底注值
#define IDD_ZJH_MAX_ZHU_VALUE       1002103   //最大下注值

#define IDD_ZJH_USER_NAME           1002105  //用户名
#define IDD_ZJH_USER_POINT          1002106  //余额
#define IDD_ZJH_USER_LEFT_MONEY     1002107 //还有多少金额可用
#define IDD_ZJH_TOTAL_MONEY         1002108 //总注

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

//按钮
//-----------------游戏操作
#define IDD_ZJH_BTN_ADD             1003000   //加注
#define IDD_ZJH_BTN_FOLLOW          1003001   //跟注
#define IDD_ZJH_BTN_GIVEUP          1003002   //放弃
#define IDD_ZJH_BTN_LOOK            1003004   //看牌
#define IDD_ZJH_BTN_CHIPIN          1003005   //下注
#define IDD_ZJH_BTN_OPEN            1003006   //开牌

#define IDD_ZJH_BTN_ADD_1           1003007    //	加一
#define IDD_ZJH_BTN_ADD_2           1003008    // 加2
#define IDD_ZJH_BTN_ADD_3           1003009    // 加3
#define IDD_ZJH_BTN_ADD_CANCLE      1003010    //取消加
#define IDD_ZJH_TEXT_MONEY          1002999    //下注额

#define IDD_ZJH_VS_0                1003011    //比牌0
#define IDD_ZJH_VS_1                1003012    //比牌0
#define IDD_ZJH_VS_2                1003013    //比牌0
#define IDD_ZJH_VS_3                1003014    //比牌0
#define IDD_ZJH_VS_4                1003015    //比牌0
#define IDD_ZJH_VS_5                1003016    //比牌0
#define IDD_ZJH_BIPAI_BACKGROUND    1003019    //比牌背景

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

//下注内容提示
#define IDD_TIPS_PLAYER_TOTAL_NOTE_0     1003110
#define IDD_TIPS_PLAYER_TOTAL_NOTE_1     1003111
#define IDD_TIPS_PLAYER_TOTAL_NOTE_2     1003112
#define IDD_TIPS_PLAYER_TOTAL_NOTE_3     1003113
#define IDD_TIPS_PLAYER_TOTAL_NOTE_4     1003114
#define IDD_TIPS_PLAYER_TOTAL_NOTE_5     1003114

//跟注
#define IDD_TIPS_PLAYER_FOLLOW_0   1003120
#define IDD_TIPS_PLAYER_FOLLOW_1   1003121
#define IDD_TIPS_PLAYER_FOLLOW_2   1003122
#define IDD_TIPS_PLAYER_FOLLOW_3   1003123
#define IDD_TIPS_PLAYER_FOLLOW_4   1003124
#define IDD_TIPS_PLAYER_FOLLOW_5   1003125

//开牌
#define IDD_TIPS_PLAYER_OPEN_0     1003130
#define IDD_TIPS_PLAYER_OPEN_1     1003131
#define IDD_TIPS_PLAYER_OPEN_2     1003132
#define IDD_TIPS_PLAYER_OPEN_3     1003133
#define IDD_TIPS_PLAYER_OPEN_4     1003134
#define IDD_TIPS_PLAYER_OPEN_5     1003135

//下注
#define IDD_TIPS_PLAYER_CHIPIN_0   1003140
#define IDD_TIPS_PLAYER_CHIPIN_1   1003141
#define IDD_TIPS_PLAYER_CHIPIN_2   1003142
#define IDD_TIPS_PLAYER_CHIPIN_3   1003143
#define IDD_TIPS_PLAYER_CHIPIN_4   1003144
#define IDD_TIPS_PLAYER_CHIPIN_5   1003145
//加注
#define IDD_TIPS_PLAYER_ADD_0     1003150
#define IDD_TIPS_PLAYER_ADD_1     1003151
#define IDD_TIPS_PLAYER_ADD_2     1003152
#define IDD_TIPS_PLAYER_ADD_3     1003153
#define IDD_TIPS_PLAYER_ADD_4     1003154
#define IDD_TIPS_PLAYER_ADD_5     1003155




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


//放弃
#define IDD_TIPS_PLAYER_GIVEUP_0   1003220
#define IDD_TIPS_PLAYER_GIVEUP_1   1003221
#define IDD_TIPS_PLAYER_GIVEUP_2   1003222
#define IDD_TIPS_PLAYER_GIVEUP_3   1003223
#define IDD_TIPS_PLAYER_GIVEUP_4   1003224
#define IDD_TIPS_PLAYER_GIVEUP_5   1003225



//看牌
#define IDD_TIPS_PLAYER_LOOK_0     1003230
#define IDD_TIPS_PLAYER_LOOK_1     1003231
#define IDD_TIPS_PLAYER_LOOK_2     1003232
#define IDD_TIPS_PLAYER_LOOK_3     1003233
#define IDD_TIPS_PLAYER_LOOK_4     1003234
#define IDD_TIPS_PLAYER_LOOK_5     1003235

//比牌胜利
#define IDD_TIPS_PLAYER_OPEN_WIN_0     1003240
#define IDD_TIPS_PLAYER_OPEN_WIN_1     1003241
#define IDD_TIPS_PLAYER_OPEN_WIN_2     1003242
#define IDD_TIPS_PLAYER_OPEN_WIN_3     1003243
#define IDD_TIPS_PLAYER_OPEN_WIN_4     1003244
#define IDD_TIPS_PLAYER_OPEN_WIN_5     1003245
//比牌失败
#define IDD_TIPS_PLAYER_OPEN_LOSE_0     1003250
#define IDD_TIPS_PLAYER_OPEN_LOSE_1     1003251
#define IDD_TIPS_PLAYER_OPEN_LOSE_2     1003252
#define IDD_TIPS_PLAYER_OPEN_LOSE_3     1003253
#define IDD_TIPS_PLAYER_OPEN_LOSE_4     1003254
#define IDD_TIPS_PLAYER_OPEN_LOSE_5     1003255
//达到上限
#define IDD_TIPS_PLAYER_MAX_LIMIT_0     1003260
#define IDD_TIPS_PLAYER_MAX_LIMIT_1     1003261
#define IDD_TIPS_PLAYER_MAX_LIMIT_2     1003262
#define IDD_TIPS_PLAYER_MAX_LIMIT_3     1003263
#define IDD_TIPS_PLAYER_MAX_LIMIT_4     1003264
#define IDD_TIPS_PLAYER_MAX_LIMIT_5     1003265

#define IDD_SET_THING_BTN               1003500 ///设置按钮

#define IDD_SET_THING_DLG               1003510 /// 设置框

#define IDD_SET_THING_SOUND             1003520 /// 设置框音效单选

#define IDD_SET_THING_OK_BTN            1003540 /// 设置框OK

#define IDD_SET_THING_CANCEL_BTN        1003550 /// 设置框Cancel



#define IDD_CM_SHCTRL_CENTER            1003700 //中心筹码控件
#define IDD_CM_SHCTRL_PLAY_NOTE         1003701 //玩家下注筹码控件
#define IDD_CM_SHCTRL_PLAY_NOTE_1       1003702 //玩家下注筹码控件
#define IDD_CM_SHCTRL_PLAY_NOTE_2       1003703 //玩家下注筹码控件
#define IDD_CM_SHCTRL_PLAY_NOTE_3       1003704 //玩家下注筹码控件
#define IDD_CM_SHCTRL_PLAY_NOTE_4       1003705 //玩家下注筹码控件
#define IDD_CM_SHCTRL_PLAY_NOTE_5       1003706 //玩家下注筹码控件

#define IDD_MOV_CONTROL                 1003800 //移动推控件
#define IDD_MOV_CONTROL_1               1003801 //移动推控件
#define IDD_MOV_CONTROL_2               1003802 //移动推控件
#define IDD_MOV_CONTROL_3               1003803 //移动推控件
#define IDD_MOV_CONTROL_4               1003804 //移动推控件
#define IDD_MOV_CONTROL_5               1003805 //移动推控件
#define IDD_MOV_CENTER                  1003806 //收集中心筹码动画

#define IDD_MOV_VS_CARD_0               1003810 //比牌移动控件
#define IDD_MOV_VS_CARD_1               1003811 //比牌移动控件
#define IDD_PLAY_VS_ANIMAL              1003812 //比牌动画

#define IDD_MOV_SEND_CARD               1003820 //发牌动画
#define IDD_SEND_CARD					1003830	 //发牌动画的牌




//
#define IDD_LOSE_CARD_0                 1003900 //失败的牌0
#define IDD_LOSE_CARD_1                 1003901 //失败的牌1
#define IDD_LOSE_CARD_2                 1003902 //失败的牌2
#define IDD_LOSE_CARD_3                 1003903 //失败的牌3
#define IDD_LOSE_CARD_4                 1003904 //失败的牌4
#define IDD_LOSE_CARD_5                 1003905 //失败的牌5