#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///玩家信息 50~120
#define     PLAYINFOSTART          50                           ///< 玩家信息开始序号
#define     PLAYNUM                10                           ///< 玩家个数
#define		TEXT_LEV              PLAYINFOSTART                 ///< 50     等级  相当于id0的等级
#define		TEXT_ACCOUNT          (TEXT_LEV+PLAYNUM)            ///< 60     ID1帐号
#define		TEXT_NICKNAME         (TEXT_ACCOUNT+PLAYNUM)        ///< 70     昵称
#define     IMAGE_NICKNAME_BK     (TEXT_NICKNAME+PLAYNUM)       ///< 80     昵称显示底图   可能有些有，有些没有
#define		IMAGE_LOGO            (IMAGE_NICKNAME_BK+PLAYNUM)   ///< 90     玩家图像标志
#define		TEXT_MONEY1           (IMAGE_LOGO+PLAYNUM)          ///< 100    金钱1   这里表示的是金币
#define		TEXT_MONEY2           (TEXT_MONEY1+PLAYNUM)         ///< 110    金钱2   这里表示的是元宝
#define		TEXT_MONEY3           (TEXT_MONEY2+PLAYNUM)         ///< 120    金钱3   备用
#define		IMAGE_LOGO_FRAME      (TEXT_MONEY3+PLAYNUM)         ///< 130    玩家图像底图

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///右边栏 700~800
#define     RIGHTBOARD_TEXT_NICKNAME        700        ///<昵称
#define     RIGHTBOARD_TEXT_ID              701        ///<ID
#define     RIGHTBOARD_TEXT_JIFEN           702        ///<积分
#define     RIGHTBOARD_TEXT_JINGBI          703        ///<金币
#define     RIGHTBOARD_TEXT_MEILI           704        ///<魅力
#define     RIGHTBOARD_TEXT_DENJI           705        ///<等级

#define     RIGHTBOARD_USERLIST             710        ///<用户列表
#define     RIGHTBOARD_USERMENU             711        ///<用户菜单

#define     RIGHTBOARD_PROP_CTN             720        ///<道具容器
#define     RIGHTBOARD_PROP_PROP_1          721        ///<单个道具容器
#define     RIGHTBOARD_PROP_PROP_1_BTN_USE  722        ///<单个道具使用按钮
#define     RIGHTBOARD_PROP_PROP_1_TEXT     723        ///<单个道具数量显示
#define     RIGHTBOARD_PROP_BTN_NEXT1       724        ///<道具容器
#define     RIGHTBOARD_PROP_BTN_PRE1        725        ///<道具容器
#define     RIGHTBOARD_PROP_BTN_NEXT2       726        ///<道具容器
#define     RIGHTBOARD_PROP_BTN_PRE2        727        ///<道具容器

#define     RIGHTBOARD_BTN_SHOP             750        ///<商城按钮
#define     RIGHTBOARD_BTN_VIP              751        ///<VIP按钮

#define     RIGHTBOARD_BTN_SET              760        ///<设置按钮
#define     RIGHTBOARD_BTN_HOME             761        ///<主页按钮
#define     RIGHTBOARD_BTN_CHARG            762        ///<充值按钮 


#define     RIGHTBOARD_BTN_SHOW             798        ///<显示右边栏按钮
#define     RIGHTBOARD_BTN_HIDE             799        ///<隐藏右边栏按钮
#define     RIGHTBOARD_CONTAIN              800        ///<右边栏容器


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///聊天框   ID区间 200-249
#define		TALK_CONATINER         220      ///<放置所有聊天控件的容器
#define		TALK_SHOW              200      ///<聊天信息显示栏
#define		ETEXT_TALK_INPUT       201      ///<聊天输入框
#define		BN_TALK_SEND           202      ///<聊天表发送按扭
#define		BN_TALK_EXPRESSIO      203      ///<聊天表情按扭
#define		ETEXT_TALK_COMBO       204      ///<聊天输入下拉框
#define		TALK_MENU			   210      ///<聊天右键菜单

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///大小喇叭信息输入框
#define CTN_MsgInput		300	
#define CTN_MsgInput_OK		302	
#define CTN_MsgInput_Close  305 
#define CTN_MsgInput_Cancel 303 
#define CTN_MsgInput_Edit   301 
#define CTN_MsgInput_Tip    304 


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///查看玩家菜单 400
#define		MEUN_LOOK			400



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///对话框  500~510
#define		CTN_DIALOG			500					///对话框
#define		BN_DLALOG_OK		501					///对话框确认
#define		BN_DLALOG_CANCEL	504					///对话框取消
#define     BN_DLALOG_CLOSE		505					///对话框关闭
#define		TX_DLALOG_MSG		506					///对话框信息


///设置框  401~405
#define     BTN_SET             520                 ///设置按钮
#define     SET_ID_BEGIN        401
#define     CTN_SET_DLG         401                 ///设置框
#define     BTN_SET_CLOSE       402                 ///设置框关闭
#define     BTN_SET_OK          403                 ///确定按钮
#define     BTN_SET_CANCEL      404                 ///取消按钮
#define     SLIDER_SET_VOL      408                 ///音量
#define     CHECK_ENABLEWATCH   405                 ///允许旁观
#define     SET_ID_END          420

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///表情框 599~650
#define CTN_BIAOQING_KUANG 599  ///表情
#define IMG_BIAOQING_0  600     ///第0个表情
#define IMG_BIAOQING_49 649     ///第49个表情
#define IMG_BIAOQING_BJ 650     ///表情背景



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///道具购买容器 801~850
#define     BUYPROP_CTN                     801     ///< 道具购买容器
#define     BUYPROP_WEB_PROPINFO            802     ///< 道具信息（WEB控件）
#define     BUYPROP_TEXT_OWNNUM             803     ///< 拥有数量 
#define     BUYPROP_TEXT_BUYNUM             804     ///< 购买数量 
#define     BUYPROP_BTN_USE                 805     ///< 使用按钮
#define     BUYPROP_BTN_BUY                 806     ///< 购买按钮
#define     BUYPROP_BTN_CLOSE               809     ///< 关闭按钮
#define     BUYPROP_TEXT_NEEDGOLD           807     ///< 购买所需金币
#define     BUYPROP_TEXT_HAVEGOLD           808     ///< 银行金币
#define     BN_BUY_VIP                      810


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///克隆道具按钮 1000种
#define     PROP_BTN_BEGIN                  1000
#define     PROP_BTN_END                    2000


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///游戏通用ID 1~50
#define     BN_START_GAME		   10001   ///< 开始游戏
#define     BN_RESET_FRAME         9       ///还原按钮         
#define		BN_MIN_FRAME           10      ///< 最小化窗口
#define		BN_SET_FRAME           12      ///< 设置游戏音量等信息
#define		BN_CLOSE_FRAME         11      ///< 关闭游戏窗口

#define     BN_VOLUM_UP            13      ///< 音量加
#define     BN_VOLUM_DOWN          14      ///< 音量减
#define     BN_VOLUM_OFF           15      ///< 音乐关
#define     BN_VOLUM_ON            16      ///< 音乐开

#define     IMAGE_NET_SIGNAL       17      ///< 网络信号

#define     BN_SHOW_RIGHTFRAME     20      ///< 显示可边框

#define     USERPROP_ANIMAL_0      3500     ///用户道俱动画

#define		CTN_CONTEST_BK			2100
#define		IMAGE_AUTO_DESK			2101	//比赛配桌背景
#define		IMAGE_CONTEST_NOAWARD	2102	//比赛结束没有获取奖励
#define		IMAGE_CONTEST_KICK		2103	//比赛用户被淘汰
#define		CTN_CONTEST_AWARDS		2104	//比赛用户获得奖励
#define		LABEL_RANK				2105	//比赛用户的排名
#define		LABEL_AWARDS			2106	//比赛用户获得的奖励描述
#define		IMAGE_CONTEST_WAIT		2107	//比赛用户等待其他玩家结束最后一局分配排名


