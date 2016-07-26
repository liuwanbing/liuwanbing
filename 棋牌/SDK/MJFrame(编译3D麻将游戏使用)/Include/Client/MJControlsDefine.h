#ifndef _CONTROLID_FOR_LOGIC_UIGameComm_
#define _CONTROLID_FOR_LOGIC_UIGameComm_

/// 麻将框架UI控件定义
/// 这是给逻辑用的控件信息，程序员可以按自己的习惯改里面的宏
/// 文件名格式为：生成的相应的控件信息文件名+ “_”+ControlDefines 
/// 宏    +    ID    +  控件名字  + 控件类型  + 资源路径
const int CTN_1000000_CTN_2 = 2;                          ///< 上边框  容器 
const int CTN_BTN_MIN = 10;                               ///< 最小化按钮
const int CTN_BTN_CLOSE = 11;                             ///< 关闭按钮
const int CTN_1000000_CTN_2_IMG_1002444 = 1002444;        ///< 网络品质
const int CTN_1000000_CTN_2_Sudoku_1 = 1;                 ///< 边框
const int CTN_TEXT_TILE = 1002440;       ///< 欢迎文字

const int CTN_1002611 = 1002611;                          ///< 确定是否离开容器
const int CTN_1002611_BTN_1002612 = 1002612;              ///< 确定
const int CTN_1002611_BTN_1002615 = 1002615;              ///< 取消
const int CTN_1002611_TEXT_1002616 = 1002616;             ///< 静态文字控件

// 动画ID定义范围 2000000 - 2001000
const int ExAnimate_HuPai      = 2000000;                 ///< 胡牌动画
const int ExAnimate_Ready_Wait = 2000002;                 ///< 准备后的等待动画

const int ExAnimate_0_Saizi_1 = 2000010;                  ///< 0位置玩家骰子1
const int ExAnimate_1_Saizi_1 = ExAnimate_0_Saizi_1 + 1;  ///< 1位置玩家骰子1
const int ExAnimate_2_Saizi_1 = ExAnimate_0_Saizi_1 + 2;  ///< 2位置玩家骰子1
const int ExAnimate_3_Saizi_1 = ExAnimate_0_Saizi_1 + 3;  ///< 3位置玩家骰子1

const int ExAnimate_0_Saizi_2 = 2000020;                  ///< 0位置玩家骰子2
const int ExAnimate_1_Saizi_2 = ExAnimate_0_Saizi_2 + 1;  ///< 1位置玩家骰子2
const int ExAnimate_2_Saizi_2 = ExAnimate_0_Saizi_2 + 2;  ///< 2位置玩家骰子2
const int ExAnimate_3_Saizi_2 = ExAnimate_0_Saizi_2 + 3;  ///< 3位置玩家骰子2
// 动画ID定义 end


// 结算框ID范围定义 2001000 - 2002000
const int CTN_JieSuanKuang          = 2001000;                        ///< 结算框容器
const int CTN_JSK_BTN_Exit          = 2001001;                        ///< 退出
const int CTN_JSK_BTN_Continue      = 2001002;                        ///< 继续
const int CTN_JSK_IMG_ZiMo          = 2001003;                        ///< 自摸
const int CTN_JSK_IMG_DianPao       = 2001004;                        ///< 点炮
const int CTN_JSK_TEXT_Win_NickName = 2001005;                        ///< 胡牌玩家昵称
const int CTN_JSK_MJHandCtrl        = 2001006;                        ///< 胡牌麻将手牌控件
const int CTN_JSK_NUM_AllFen        = 2001007;                        ///< 番型总分
const int CTN_JSK_IMG_pxhj          = 2001008;                        ///< 牌型合计图片
const int CTN_JSK_IMG_LiuJu         = 2001009;                        ///< 流局
const int CTN_JSK_IMG_Line1         = 2001010;                        ///< 装饰白线1
const int CTN_JSK_IMG_Line2         = 2001011;                        ///< 装饰白线2

const int CTN_JSK_TEXT_NickName_0   = 2001100;                        ///< 玩家0昵称
const int CTN_JSK_TEXT_NickName_1   = CTN_JSK_TEXT_NickName_0 + 1;    ///< 玩家1昵称
const int CTN_JSK_TEXT_NickName_2   = CTN_JSK_TEXT_NickName_0 + 2;    ///< 玩家2昵称
const int CTN_JSK_TEXT_NickName_3   = CTN_JSK_TEXT_NickName_0 + 3;    ///< 玩家3昵称

const int CTN_JSK_TEXT_HuFen_0      = 2001200;                        ///< 玩家0胡分
const int CTN_JSK_TEXT_HuFen_1      = CTN_JSK_TEXT_HuFen_0 + 1;       ///< 玩家1胡分
const int CTN_JSK_TEXT_HuFen_2      = CTN_JSK_TEXT_HuFen_0 + 2;       ///< 玩家2胡分
const int CTN_JSK_TEXT_HuFen_3      = CTN_JSK_TEXT_HuFen_0 + 3;       ///< 玩家3胡分

const int CTN_JSK_TEXT_SJGangFen_0  = 2001300;                        ///< 玩家0上精与杠分
const int CTN_JSK_TEXT_SJGangFen_1  = CTN_JSK_TEXT_SJGangFen_0 + 1;   ///< 玩家1上精与杠分
const int CTN_JSK_TEXT_SJGangFen_2  = CTN_JSK_TEXT_SJGangFen_0 + 2;   ///< 玩家2上精与杠分
const int CTN_JSK_TEXT_SJGangFen_3  = CTN_JSK_TEXT_SJGangFen_0 + 3;   ///< 玩家3上精与杠分

const int CTN_JSK_TEXT_Tax_0        = 2001350;                        ///< 玩家0税收
const int CTN_JSK_TEXT_Tax_1        = CTN_JSK_TEXT_Tax_0 + 1;         ///< 玩家1税收
const int CTN_JSK_TEXT_Tax_2        = CTN_JSK_TEXT_Tax_0 + 2;         ///< 玩家2税收
const int CTN_JSK_TEXT_Tax_3        = CTN_JSK_TEXT_Tax_0 + 3;         ///< 玩家3税收

const int CTN_JSK_NUM_AllFen_0      = 2001400;                        ///< 玩家0总分
const int CTN_JSK_NUM_AllFen_1      = CTN_JSK_NUM_AllFen_0 + 1;       ///< 玩家1总分
const int CTN_JSK_NUM_AllFen_2      = CTN_JSK_NUM_AllFen_0 + 2;       ///< 玩家2总分
const int CTN_JSK_NUM_AllFen_3      = CTN_JSK_NUM_AllFen_0 + 3;       ///< 玩家3总分

const int CTN_JSK_IMG_NT_0          = 2001500;                        ///< 玩家0庄
const int CTN_JSK_IMG_NT_1          = CTN_JSK_IMG_NT_0 + 1;           ///< 玩家1庄
const int CTN_JSK_IMG_NT_2          = CTN_JSK_IMG_NT_0 + 2;           ///< 玩家2庄
const int CTN_JSK_IMG_NT_3          = CTN_JSK_IMG_NT_0 + 3;           ///< 玩家3庄

const int CTN_JSK_IMG_PAO_0         = 2001600;                        ///< 玩家0炮
const int CTN_JSK_IMG_PAO_1         = CTN_JSK_IMG_PAO_0 + 1;          ///< 玩家1炮
const int CTN_JSK_IMG_PAO_2         = CTN_JSK_IMG_PAO_0 + 2;          ///< 玩家2炮
const int CTN_JSK_IMG_PAO_3         = CTN_JSK_IMG_PAO_0 + 3;          ///< 玩家3炮

const int CTN_JSK_IMG_HU_0          = 2001650;                        ///< 玩家0胡
const int CTN_JSK_IMG_HU_1          = CTN_JSK_IMG_HU_0 + 1;           ///< 玩家1胡
const int CTN_JSK_IMG_HU_2          = CTN_JSK_IMG_HU_0 + 2;           ///< 玩家2胡
const int CTN_JSK_IMG_HU_3          = CTN_JSK_IMG_HU_0 + 3;           ///< 玩家3胡


const int CTN_JSK_CTN_FanXing       = 2001700;                        ///< 胡牌翻型
const int CTN_JSK_CTN_FanXing_Name  = 2001701;                        ///< 胡牌翻型名称
const int CTN_JSK_CTN_FanXing_Num   = 2001702;                        ///< 胡牌翻型数值
// 结算框ID定义 end
								  
								  
// 游戏信息ID范围定义 2002000 - 2003000
const int CTN_GameInfo = 2002000;                             ///< 游戏信息
const int CTN_Self_Money_BG = 2002001;                        ///< 自己的金币底图
const int CTN_Self_Level_BG = 2002002;                        ///< 自己的等级底图
const int CTN_Self_Medal_BG = 2002003;                        ///< 自己的勋章底图
const int CTN_TEXT_UserID   = 2002004;                        ///< 自己的用户ID
const int CTN_EDITORTEXT_GM = 2002005;                        ///< GM命令控件
const int CTN_IMG_LANZHUANG = 2002006;                        ///< 烂庄

const int CTN_ExAnimate_Logo_0 = 2002100;                     ///< 人物动画控件1  外部动画控件 
const int CTN_ExAnimate_Logo_1 = CTN_ExAnimate_Logo_0 + 1;    ///< 人物动画控件2  外部动画控件 
const int CTN_ExAnimate_Logo_2 = CTN_ExAnimate_Logo_0 + 2;    ///< 人物动画控件3  外部动画控件 
const int CTN_ExAnimate_Logo_3 = CTN_ExAnimate_Logo_0 + 3;    ///< 人物动画控件4  外部动画控件 

const int CTN_IMG_BK_0 = 2002110;                             /// 玩家0底图
const int CTN_IMG_BK_1 = CTN_IMG_BK_0 + 1;                    /// 玩家1底图
const int CTN_IMG_BK_2 = CTN_IMG_BK_0 + 2;                    /// 玩家2底图
const int CTN_IMG_BK_3 = CTN_IMG_BK_0 + 3;                    /// 玩家3底图

const int CTN_TEXT_NickName_0 = 2002120;                      /// 玩家0昵称
const int CTN_TEXT_NickName_1 = CTN_TEXT_NickName_0 + 1;      /// 玩家1昵称
const int CTN_TEXT_NickName_2 = CTN_TEXT_NickName_0 + 2;      /// 玩家2昵称
const int CTN_TEXT_NickName_3 = CTN_TEXT_NickName_0 + 3;      /// 玩家3昵称

const int CTN_TEXT_Money_0 = 2002130;                         /// 玩家0金币
const int CTN_TEXT_Money_1 = CTN_TEXT_Money_0 + 1;            /// 玩家1金币
const int CTN_TEXT_Money_2 = CTN_TEXT_Money_0 + 2;            /// 玩家2金币
const int CTN_TEXT_Money_3 = CTN_TEXT_Money_0 + 3;            /// 玩家3金币

const int CTN_TEXT_Level_0 = 2002140;                         /// 玩家0等级
const int CTN_TEXT_Level_1 = CTN_TEXT_Level_0 + 1;            /// 玩家1等级
const int CTN_TEXT_Level_2 = CTN_TEXT_Level_0 + 2;            /// 玩家2等级
const int CTN_TEXT_Level_3 = CTN_TEXT_Level_0 + 3;            /// 玩家3等级

const int CTN_TEXT_IP_0 = 2002150;                            /// 玩家0IP
const int CTN_TEXT_IP_1 = CTN_TEXT_IP_0 + 1;                  /// 玩家1IP
const int CTN_TEXT_IP_2 = CTN_TEXT_IP_0 + 2;                  /// 玩家2IP
const int CTN_TEXT_IP_3 = CTN_TEXT_IP_0 + 3;                  /// 玩家3IP

const int CTN_IMG_NT_0 = 2002160;                             /// 玩家0庄家
const int CTN_IMG_NT_1 = CTN_IMG_NT_0 + 1;                    /// 玩家1庄家
const int CTN_IMG_NT_2 = CTN_IMG_NT_0 + 2;                    /// 玩家2庄家
const int CTN_IMG_NT_3 = CTN_IMG_NT_0 + 3;                    /// 玩家3庄家

const int CTN_IMG_TuoGuan_0 = 2002170;                        /// 玩家0托管
const int CTN_IMG_TuoGuan_1 = CTN_IMG_TuoGuan_0 + 1;          /// 玩家1托管
const int CTN_IMG_TuoGuan_2 = CTN_IMG_TuoGuan_0 + 2;          /// 玩家2托管
const int CTN_IMG_TuoGuan_3 = CTN_IMG_TuoGuan_0 + 3;          /// 玩家3托管
								 
const int CTN_IMG_BaWang_0 = 2002180;                         /// 玩家0霸王
const int CTN_IMG_BaWang_1 = CTN_IMG_BaWang_0 + 1;            /// 玩家1霸王
const int CTN_IMG_BaWang_2 = CTN_IMG_BaWang_0 + 2;            /// 玩家2霸王
const int CTN_IMG_BaWang_3 = CTN_IMG_BaWang_0 + 3;            /// 玩家3霸王

const int CTN_NUM_JingFen_0 = 2002190;                        /// 玩家0下精分
const int CTN_NUM_JingFen_1 =  CTN_NUM_JingFen_0 + 1;         /// 玩家1下精分
const int CTN_NUM_JingFen_2 =  CTN_NUM_JingFen_0 + 2;         /// 玩家2下精分
const int CTN_NUM_JingFen_3 =  CTN_NUM_JingFen_0 + 3;         /// 玩家3下精分

const int CTN_IMG_Money_0 = 2002200;                          /// 玩家0金币图片
const int CTN_IMG_Money_1 =  CTN_IMG_Money_0 + 1;             /// 玩家1金币图片
const int CTN_IMG_Money_2 =  CTN_IMG_Money_0 + 2;             /// 玩家2金币图片
const int CTN_IMG_Money_3 =  CTN_IMG_Money_0 + 3;             /// 玩家3金币图片

const int CTN_NUM_Money_0 = 2002210;                          /// 玩家0金币数字
const int CTN_NUM_Money_1 =  CTN_NUM_Money_0 + 1;             /// 玩家1金币数字
const int CTN_NUM_Money_2 =  CTN_NUM_Money_0 + 2;             /// 玩家2金币数字
const int CTN_NUM_Money_3 =  CTN_NUM_Money_0 + 3;             /// 玩家3金币数字

const int CTN_ChongGuan_0    = 2002300;                       /// 冲关容器0
const int CTN_ChongGuan_1    = CTN_ChongGuan_0 + 1;           /// 冲关容器1
const int CTN_ChongGuan_2    = CTN_ChongGuan_0 + 2;           /// 冲关容器2
const int CTN_ChongGuan_3    = CTN_ChongGuan_0 + 3;           /// 冲关容器3
const int CTN_ExAnimate_CG_0 = 2002310;				          /// 冲关动画0
const int CTN_ExAnimate_CG_1 = CTN_ExAnimate_CG_0 + 1;        /// 冲关动画1
const int CTN_ExAnimate_CG_2 = CTN_ExAnimate_CG_0 + 2;        /// 冲关动画2
const int CTN_ExAnimate_CG_3 = CTN_ExAnimate_CG_0 + 3;        /// 冲关动画3
const int CTN_NUM_CG_0       = 2002320;				          /// 冲关数字0
const int CTN_NUM_CG_1       = CTN_NUM_CG_0 + 1;              /// 冲关数字1
const int CTN_NUM_CG_2       = CTN_NUM_CG_0 + 2;              /// 冲关数字2
const int CTN_NUM_CG_3       = CTN_NUM_CG_0 + 3;              /// 冲关数字3
															  
const int CTN_Alarm_0           = 2002400;                    /// 报警容器0
const int CTN_Alarm_1           = CTN_Alarm_0 + 1;            /// 报警容器1
const int CTN_Alarm_2           = CTN_Alarm_0 + 2;            /// 报警容器2
const int CTN_Alarm_3           = CTN_Alarm_0 + 3;            /// 报警容器3
const int CTN_ExAnimate_Alarm_0 = 2002410;				      /// 报警动画0
const int CTN_ExAnimate_Alarm_1 = CTN_ExAnimate_Alarm_0 + 1;  /// 报警动画1
const int CTN_ExAnimate_Alarm_2 = CTN_ExAnimate_Alarm_0 + 2;  /// 报警动画2
const int CTN_ExAnimate_Alarm_3 = CTN_ExAnimate_Alarm_0 + 3;  /// 报警动画3
const int CTN_NUM_Alarm_0       = 2002420;				      /// 报警数字0
const int CTN_NUM_Alarm_1       = CTN_NUM_Alarm_0 + 1;        /// 报警数字1
const int CTN_NUM_Alarm_2       = CTN_NUM_Alarm_0 + 2;        /// 报警数字2
const int CTN_NUM_Alarm_3       = CTN_NUM_Alarm_0 + 3;        /// 报警数字3
// 游戏信息ID定义 end										 
															  
															  
// 吃碰杠面板ID范围定义 2003000 - 2004000					  
const int CTN_Block = 2003000;                                ///< 拦牌(吃碰杠听胡)面板
const int CTN_Block_BTN_Cancel = 2003001;                     ///< 取消拦牌
const int CTN_Block_BTN_Chi    = 2003010;                     ///< 吃
const int CTN_Block_BTN_Peng   = CTN_Block_BTN_Chi + 1;       ///< 碰
const int CTN_Block_BTN_Gang   = CTN_Block_BTN_Chi + 2;       ///< 杠
const int CTN_Block_BTN_Ting   = CTN_Block_BTN_Chi + 3;       ///< 听
const int CTN_Block_BTN_Hu     = CTN_Block_BTN_Chi + 4;       ///< 胡
															
// 吃碰杠面板ID定义 end										
															
															
// 正副精面板ID范围定义 2004000 - 2004500					
const int CTN_JingPai_Panel = 2004000;                        ///< 正副精牌面板
const int CTN_JP_TongYongMJCtrl_ZhengJing = 2004001;          ///< 正精
const int CTN_JP_TongYongMJCtrl_FuJing = 2004002;             ///< 副精
// 正副精牌面板ID定义 end									
															
															
// 上下精牌面板ID范围定义 2004500 - 2004800					
const int CTN_JingPai_Center_Panel = 2004500;                 ///< 上下精牌面板
const int CTN_JP_TongYongMJCtrl_Jing = 2004501;               ///< 精牌通用麻将控制
const int CTN_JP_IMG_XiaJing = 2004502;                       ///< 下精底图
// 上下精牌面板ID定义 end									
															
															
// 拦牌提示面板ID范围定义 2004800 - 2005000					
const int CTN_Block_Panel = 2004800;                         ///< 拦牌提示面板
const int CTN_MJCtrl_Block_3 = 2004801;                      ///< 拦牌提示麻将控制3个位置的麻将
const int CTN_MJCtrl_Block_4 = 2004802;                      ///< 拦牌提示麻将控制4个位置的麻将
// 拦牌提示面板ID定义 end


// 麻将控件ID定义 2005000 - 2006000
const int CTN_MJCtrl                    = 2005000;			 ///< 麻将控件容器
const int CTN_MJCtrl_ZhuaPai            = 2005001;           ///< 抓牌动作容器
const int CTN_MJCtrl_ZhuaPai_MoveAction = 2005002;           ///< 抓牌直线移动动作
const int CTN_MJCtrl_ZhuaPai_WallCtrl   = 2005003;           ///< 抓牌动画麻将
const int CTN_IMG_Hand                  = 2005004;           ///< 小手图片
const int CTN_ExAnimate_GiveTag         = 2005005;           ///< 出牌标记动画

const int CTN_MJCtrl_HandCtrl_0 = 2005010;					 ///< 玩家0手牌控件
const int CTN_MJCtrl_HandCtrl_1 = CTN_MJCtrl_HandCtrl_0 + 1; ///< 玩家1手牌控件
const int CTN_MJCtrl_HandCtrl_2 = CTN_MJCtrl_HandCtrl_0 + 2; ///< 玩家2手牌控件
const int CTN_MJCtrl_HandCtrl_3 = CTN_MJCtrl_HandCtrl_0 + 3; ///< 玩家3手牌控件

const int CTN_MJCtrl_GiveCtrl_0 = 2005020;					 ///< 玩家0出牌控件
const int CTN_MJCtrl_GiveCtrl_1 = CTN_MJCtrl_GiveCtrl_0 + 1; ///< 玩家1出牌控件
const int CTN_MJCtrl_GiveCtrl_2 = CTN_MJCtrl_GiveCtrl_0 + 2; ///< 玩家2出牌控件
const int CTN_MJCtrl_GiveCtrl_3 = CTN_MJCtrl_GiveCtrl_0 + 3; ///< 玩家3出牌控件

const int CTN_MJCtrl_WallCtrl_0 = 2005030;					 ///< 玩家0牌墙控件
const int CTN_MJCtrl_WallCtrl_1 = CTN_MJCtrl_WallCtrl_0 + 1; ///< 玩家1牌墙控件
const int CTN_MJCtrl_WallCtrl_2 = CTN_MJCtrl_WallCtrl_0 + 2; ///< 玩家2牌墙控件
const int CTN_MJCtrl_WallCtrl_3 = CTN_MJCtrl_WallCtrl_0 + 3; ///< 玩家3牌墙控件

const int CTN_MJCtrl_ExAnimate_Hand_0 = 2005040;                         ///< 玩家0手指动画
const int CTN_MJCtrl_ExAnimate_Hand_1 = CTN_MJCtrl_ExAnimate_Hand_0 + 1; ///< 玩家1手指动画
const int CTN_MJCtrl_ExAnimate_Hand_2 = CTN_MJCtrl_ExAnimate_Hand_0 + 2; ///< 玩家2手指动画
const int CTN_MJCtrl_ExAnimate_Hand_3 = CTN_MJCtrl_ExAnimate_Hand_0 + 3; ///< 玩家3手指动画
// 麻将控件ID定义 end


// 定时器控件ID定义 2006000 - 2007000
const int CTN_Clock = 2006000;								   ///< 定时器控件容器
const int CTN_Clock_Time = 2006001;							   ///< 时间控件
const int CTN_Clock_IMG_Arrows_0 = 2006010;					   ///< 玩家0位置箭头
const int CTN_Clock_IMG_Arrows_1 = CTN_Clock_IMG_Arrows_0 + 1; ///< 玩家1位置箭头
const int CTN_Clock_IMG_Arrows_2 = CTN_Clock_IMG_Arrows_0 + 2; ///< 玩家2位置箭头
const int CTN_Clock_IMG_Arrows_3 = CTN_Clock_IMG_Arrows_0 + 3; ///< 玩家3位置箭头
// 定时器控件ID定义 end


// 其他控件ID定义 2010000 - 3000000
const int CTN_IMG_Ready_0 = 2010000;                  ///< 玩家0准备
const int CTN_IMG_Ready_1 = CTN_IMG_Ready_0 + 1;      ///< 玩家1准备
const int CTN_IMG_Ready_2 = CTN_IMG_Ready_0 + 2;      ///< 玩家2准备
const int CTN_IMG_Ready_3 = CTN_IMG_Ready_0 + 3;      ///< 玩家3准备

const int CTN_BTN_Start          = 2010100;           ///< 开始
const int CTN_BTN_TuoGuan        = 2010101;		      ///< 托管
const int CTN_BTN_Cancel_TuoGuan = 2010102;		      ///< 取消托管
const int CTN_BTN_OpenSound      = 2010103;           ///< 打开音效
const int CTN_BTN_CloseSound     = 2010104;           ///< 关闭音效
// 其他控件ID定义 end


////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif