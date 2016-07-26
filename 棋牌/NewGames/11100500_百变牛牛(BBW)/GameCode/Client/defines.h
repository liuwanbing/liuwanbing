// 用来定义界面编辑器编辑出来的控件列表中的ID号，其数值应当与界面编辑器中保持一致
// 部分由框架处理的ID号应当另行安排，可以复制到框架中代码中去，为保证框架的灵活性
// 不将此文件放到SDK目录下，而是放到具体游戏中
#ifndef _DEFINES_H
#define _DEFINES_H


/////////////////////结算框/////////////////////
#define		RESULT_CONTAIN				1111100 //结算容器


//昵称		
#define		TEXT_RESULT_NAME_0			1111105	//昵称 0
#define		TEXT_RESULT_NAME_1			1111106	//昵称 1
#define		TEXT_RESULT_NAME_2			1111107	//昵称 2
#define		TEXT_RESULT_NAME_3			1111108	//昵称 3
#define		TEXT_RESULT_NAME_4			1111109	//昵称 3

//游戏中玩家获得的积分

#define		INUM_USER_POINT_0			1111110	//游戏中玩家获得的积分 0
#define		INUM_USER_POINT_1			1111111	//游戏中玩家获得的积分 1
#define		INUM_USER_POINT_2			1111112	//游戏中玩家获得的积分 2
#define		INUM_USER_POINT_3			1111113	//游戏中玩家获得的积分 3
#define		INUM_USER_POINT_4			1111114	//游戏中玩家获得的积分 4

//自己成绩控件
#define		TEXT_MY_NAME				1111115	//自己的名字
#define		TEXT_MY_CARD_COUNT			1111116	//自己的牌张数
#define		TEXT_MY_MULTIPLE			1111117	//自己的倍数
#define		TEXT_MULTIPLE_NEWS			1111118	//自己的倍数信息
#define		TEXT_MY_POINT				1111119	//自己成绩

/////////////////////游戏/////////////////////

///<超端
#define		CHAO_DUAN_1				  12112368 ///<超端容器

#define		CHAO_DUAN_BU1			  12112384 ///<庄赢
#define		CHAO_DUAN_BU2			  12112385 ///<庄输
#define		CHAO_DUAN_BU3			  12112386 ///<天赢
#define		CHAO_DUAN_BU4			  12112387 ///<天输
#define		CHAO_DUAN_BU5			  12112388 ///<地赢
#define		CHAO_DUAN_BU6			  12112389 ///<地输
#define		CHAO_DUAN_BU7			  12112390 ///<玄赢
#define		CHAO_DUAN_BU8			  12112391 ///<玄输
#define		CHAO_DUAN_BU9			  12112392 ///<黄赢
#define		CHAO_DUAN_BU10			  12112393 ///<黄输

#define		CHAO_DUAN_OK			  12112380 ///<按钮确定
#define		CHAO_DUAN_CANCEL		  12112381 ///<按钮取消

#define		CHAO_DUAN_IMG1			  11112058 ///<成功提示
#define		CHAO_DUAN_IMG2			  11112059 ///<失败提示
#define		CHAO_DUAN_IMG3			  11112060///<按钮提示

#define		HAND_CARD_1				  12112405	//庄家牌12112405
#define		HAND_CARD_2				  12112406	//天区域底牌
#define		HAND_CARD_3				  12112407	//地区域底牌
#define		HAND_CARD_4				  12112408	//玄区域底牌
#define		HAND_CARD_5				  12112409	//黄区域底牌
									  
#define		UP_CARD_1				  12112362  //庄家升起的牌
#define		UP_CARD_2				  12112363  //天升起的牌
#define		UP_CARD_3				  12112364  //地升起的牌
#define		UP_CARD_4				  12112365  //玄升起的牌
#define		UP_CARD_5				  12112366  //黄升起的牌

#define		HAND_IMG0				  12112400	//手图片
#define		HAND_IMG1				  12112401  //手图片
#define		HAND_IMG2				  12112402	//手图片
#define		HAND_IMG3				  12112403	//手图片
#define		HAND_IMG4				  12112404	//手图片

#define		HUNDRED_BUTTON			  12112221  ///<一百按钮  按扭 resources\image\100_bt.bmp
#define		THOUSAND_BUTTON			  12112222  ///<一千按钮  按扭 resources\image\1000_bt.bmp
#define		TEN_THOUSAND_BUTTON		  12112223  ///<一万按钮  按扭 resources\image\1w_bt.bmp
#define		HUNDRED_THOUSAND_BUTTON	  12112224  ///<十万按钮  按扭 resources\image\10w_bt.bmp
#define		MILLION_BUTTON			  12112225  ///<百万按钮  按扭 resources\image\100w_bt.bmp
#define		TEN_MILLION_BUTTON		  12112226  ///<千万按钮  按扭 resources\image\1000w_bt.bmp

#define IDD_CHUOMA_QIANGZ			  12112310		/// 申请上庄
#define IDD_CHUOMA_XIAZ				  12112311		/// 我要下庄
#define BANK_BUTTON					  12112204		///银行按钮
const int CTN_11112000_BTN_12112205 = 12112205;  ///<自动搓牌按扭  按扭 resources\image\auto_chuopai_bt.bmp
#define LEFTBUTTON					  12112219		//左路子按钮
#define RIGHTBUTTON					  12112220		//右路子按钮

const int CTN_11112000_CTN_1000200 = 1000200;  ///<输赢记录容器  容器 

const int CTN_11112000_CTN_1000200_IMG_10002001 = 10002001;  ///<天图片1  图片 resources\image\gc.bmp
const int CTN_11112000_CTN_1000200_IMG_10002002 = 10002002;  ///<天图片2  图片 resources\image\gc.bmp
const int CTN_11112000_CTN_1000200_IMG_10002003 = 10002003;  ///<天图片3  图片 resources\image\gc.bmp
const int CTN_11112000_CTN_1000200_IMG_10002004 = 10002004;  ///<天图片4  图片 resources\image\gc.bmp
const int CTN_11112000_CTN_1000200_IMG_10002005 = 10002005;  ///<天图片5  图片 resources\image\gc.bmp
const int CTN_11112000_CTN_1000200_IMG_10002006 = 10002006;  ///<天图片6  图片 resources\image\gc.bmp
const int CTN_11112000_CTN_1000200_IMG_10002007 = 10002007;  ///<天图片7  图片 resources\image\gc.bmp
const int CTN_11112000_CTN_1000200_IMG_10002008 = 10002008;  ///<天图片8  图片 resources\image\gc.bmp
const int CTN_11112000_CTN_1000200_IMG_10002009 = 10002009;  ///<天图片9  图片 resources\image\gc.bmp

const int CTN_11112000_CTN_1000200_IMG_10002011 = 10002011;  ///<地图片1  图片 resources\image\gc.bmp
const int CTN_11112000_CTN_1000200_IMG_10002012 = 10002012;  ///<地图片2  图片 resources\image\gc.bmp
const int CTN_11112000_CTN_1000200_IMG_10002013 = 10002013;  ///<地图片3  图片 resources\image\gc.bmp
const int CTN_11112000_CTN_1000200_IMG_10002014 = 10002014;  ///<地图片4  图片 resources\image\gc.bmp
const int CTN_11112000_CTN_1000200_IMG_10002015 = 10002015;  ///<地图片5  图片 resources\image\gc.bmp
const int CTN_11112000_CTN_1000200_IMG_10002016 = 10002016;  ///<地图片6  图片 resources\image\gc.bmp
const int CTN_11112000_CTN_1000200_IMG_10002017 = 10002017;  ///<地图片7  图片 resources\image\gc.bmp
const int CTN_11112000_CTN_1000200_IMG_10002018 = 10002018;  ///<地图片8  图片 resources\image\gc.bmp
const int CTN_11112000_CTN_1000200_IMG_10002019 = 10002019;  ///<地图片9  图片 resources\image\gc.bmp

const int CTN_11112000_CTN_1000200_IMG_10002021 = 10002021;  ///<玄图片1  图片 resources\image\gc.bmp
const int CTN_11112000_CTN_1000200_IMG_10002022 = 10002022;  ///<玄图片2  图片 resources\image\gc.bmp
const int CTN_11112000_CTN_1000200_IMG_10002023 = 10002023;  ///<玄图片3  图片 resources\image\gc.bmp
const int CTN_11112000_CTN_1000200_IMG_10002024 = 10002024;  ///<玄图片4  图片 resources\image\gc.bmp
const int CTN_11112000_CTN_1000200_IMG_10002025 = 10002025;  ///<玄图片5  图片 resources\image\gc.bmp
const int CTN_11112000_CTN_1000200_IMG_10002026 = 10002026;  ///<玄图片6  图片 resources\image\gc.bmp
const int CTN_11112000_CTN_1000200_IMG_10002027 = 10002027;  ///<玄图片7  图片 resources\image\gc.bmp
const int CTN_11112000_CTN_1000200_IMG_10002028 = 10002028;  ///<玄图片8  图片 resources\image\gc.bmp
const int CTN_11112000_CTN_1000200_IMG_10002029 = 10002029;  ///<玄图片9  图片 resources\image\gc.bmp

const int CTN_11112000_CTN_1000200_IMG_10002031 = 10002031;  ///<黄图片1  图片 resources\image\gc.bmp
const int CTN_11112000_CTN_1000200_IMG_10002032 = 10002032;  ///<黄图片2  图片 resources\image\gc.bmp
const int CTN_11112000_CTN_1000200_IMG_10002033 = 10002033;  ///<黄图片3  图片 resources\image\gc.bmp
const int CTN_11112000_CTN_1000200_IMG_10002034 = 10002034;  ///<黄图片4  图片 resources\image\gc.bmp
const int CTN_11112000_CTN_1000200_IMG_10002035 = 10002035;  ///<黄图片5  图片 resources\image\gc.bmp
const int CTN_11112000_CTN_1000200_IMG_10002036 = 10002036;  ///<黄图片6  图片 resources\image\gc.bmp
const int CTN_11112000_CTN_1000200_IMG_10002037 = 10002037;  ///<黄图片7  图片 resources\image\gc.bmp
const int CTN_11112000_CTN_1000200_IMG_10002038 = 10002038;  ///<黄图片8  图片 resources\image\gc.bmp
const int CTN_11112000_CTN_1000200_IMG_10002039 = 10002039;  ///<黄图片9  图片 resources\image\gc.bmp


const int CTN_11112000_CTN_100800 = 100800;  ///<超端容器  容器 
const int CTN_11112000_CTN_100800_BTN_1008001 = 1008001;  ///<确定按钮  按扭 resources\image\SuperSet\queding_bt.png
const int CTN_11112000_CTN_100800_BTN_1008002 = 1008002;  ///<取消按钮  按扭 resources\image\SuperSet\guanbi_bt.png
const int CTN_11112000_CTN_100800_TEXT_1008003 = 1008003;  ///<设置提示  静态文本 
const int CTN_11112000_CTN_100800_RButton_1008004 = 1008004;  ///<屏蔽机器人单选框  单选框 
const int CTN_11112000_CTN_100800_TEXT_1008005 = 1008005;  ///<设置成功提示  静态文本 
const int CTN_11112000_CTN_100800_RButton_1008011 = 1008011;  ///<天赢单选框  单选框 
const int CTN_11112000_CTN_100800_RButton_1008012 = 1008012;  ///<地赢单选框  单选框 
const int CTN_11112000_CTN_100800_RButton_1008013 = 1008013;  ///<玄赢单选框  单选框 
const int CTN_11112000_CTN_100800_RButton_1008014 = 1008014;  ///<黄赢单选框  单选框 
const int CTN_11112000_CTN_100800_RButton_1008021 = 1008021;  ///<天输单选框  单选框 
const int CTN_11112000_CTN_100800_RButton_1008022 = 1008022;  ///<地输单选框  单选框 
const int CTN_11112000_CTN_100800_RButton_1008023 = 1008023;  ///<玄输单选框  单选框 
const int CTN_11112000_CTN_100800_RButton_1008024 = 1008024;  ///<黄输单选框  单选框 

#define STATIC_NT_NAME				  11112108				 ///<庄家名称  静态文本 
#define STATIC_USER_NAME			  11112110				 ///<用户名称  静态文本 

#define TIAN_SH					  12112227				///天梭哈控件12112227
#define DI_SH					  12112228				///地梭哈控件
#define XUAN_SH					  12112229				///玄梭哈控件
#define HUANG_SH				  12112230				///黄梭哈控件

#define TIAN_XIAZHU_AREA_PIC	  12112321				///天下注区域闪图片
#define DI_XIAZHU_AREA_PIC		  12112322				///地下注区域闪图片
#define XUAN_XIAZHU_AREA_PIC	  12112323				///玄下注区域闪图片
#define HUANG_XIAZHU_AREA_PIC	  12112324				///黄下注区域闪图片

#define TIAN_XIAZHU_AREA_BNT		  12112312				///天下注区域按钮
#define DI_XIAZHU_AREA_BNT			  12112313				///地下注区域按钮
#define XUAN_XIAZHU_AREA_BNT		  12112314				///玄下注区域按钮
#define HUANG_XIAZHU_AREA_BNT		  12112315				///黄下注区域按钮

#define TIAN_ALLMONEY_NUM		      12112287				///天所有钱的数字控件
#define DI_ALLMONEY_NUM				  12112288				///地所有钱的数字控件
#define XUAN_ALLMONEY_NUM			  12112289				///玄所有钱的数字控件
#define HUANG_ALLMONEY_NUM			  12112290				///黄所有钱的数字控件

#define TIAN_OWERMONEY_NUM			  12112291				///天自已下注的数字控件
#define DI_OWERMONEY_NUM			  12112292				///地自已下注的数字控件
#define XUAN_OWERMONEY_NUM			  12112293				///玄自已下注的数字控件
#define HUANG_OWERMONEY_NUM			  12112294				///黄自已下注的数字控件

#define CLOCKPIC					  12112215				///时钟图片
#define CLOCK						  12112216				///时间框
#define CLOCK_NUM					  12112216				///时钟值

#define STATICWORD					  12112295				///状态提示文字	

#define SETTLEMENTFRAME_PIC			  12112317				///结算框容器图片
#define SETTLEMENT_NUN_01			  12112318				///本家得分
#define SETTLEMENT_NUN_02			  12112319				///本家返还得分
#define SETTLEMENT_NUN_03			  12112320				///庄家得分


//-------------------------------------------------------------------------------------
//庄家信息
#define ZHUANGNAME					  12112206				///庄家名称
#define ZHUANGMONEY					  12112207				///庄家金币
#define ZHUANGPLAYCOUNT				  12112208				///庄家局数
#define ZHUANGSOCE					  12112209				///庄家成绩
//-------------------------------------------------------------------------------------
//玩家信息
#define USERNAME					  12112210				///闲家名称
#define USERMONEY					  12112211				///闲家金币
#define USERSOCE					  12112212				///闲家成绩

//-------------------------------------------------------------------------------------
//当前信息（当前局数和当前把数）
#define CURRENT_IMG					  12112236              //当前图片
#define CURRENTJUSHU				  12112213              //当前局数
#define CURRENTBUSHU				  12112214				//当前把数

//-------------------------------------------------------------------------------------
#define XIAZHU_FULL_PIC				  11112170				///下注已满,提前开始	
#define HUANGZHUANG_PIC				  11112218				///正在换庄

//-------------------------------------------------------------------------------------
const int CTN_11112000_CTN_1000100 = 1000100;  ///<上庄列表容器  容器 
const int CTN_11112000_CTN_1000100_TEXT_10001001 = 10001001;  ///<上庄玩家0  静态文本 
const int CTN_11112000_CTN_1000100_TEXT_10001002 = 10001002;  ///<上庄玩家1  静态文本 
const int CTN_11112000_CTN_1000100_TEXT_10001003 = 10001003;  ///<上庄玩家2  静态文本 
const int CTN_11112000_CTN_1000100_TEXT_10001004 = 10001004;  ///<上庄玩家3  静态文本 
const int CTN_11112000_CTN_1000100_TEXT_10001005 = 10001005;  ///<上庄玩家4  静态文本 
const int CTN_11112000_CTN_1000100_TEXT_10001011 = 10001011;  ///<上庄玩家金币0  静态文本 
const int CTN_11112000_CTN_1000100_TEXT_10001012 = 10001012;  ///<上庄玩家金币1  静态文本 
const int CTN_11112000_CTN_1000100_TEXT_10001013 = 10001013;  ///<上庄玩家金币2  静态文本 
const int CTN_11112000_CTN_1000100_TEXT_10001014 = 10001014;  ///<上庄玩家金币3  静态文本 
const int CTN_11112000_CTN_1000100_TEXT_10001015 = 10001015;  ///<上庄玩家金币4  静态文本 

//------------------------------------------------------------------------------------
#define BANK_DIALOG_CONTAINER		  11112188				///银行对话框容器
#define CURRENT_MONEY_TEXT			  11112195				///当前游戏的金币
#define BANK_MONEY_TEXT				  11112190				///当前银行的金币
#define OK_BUTTON					  11112191				///确定按钮
#define CANCAL_BUTTON				  11112192				///取消按钮				  
#define SAVE_MONEY					  11112204				///存钱按钮
#define TAKE_MONEY					  11112205				///取钱按钮
#define WREATER_MONEY				  11112197				///输入金币值
#define	PASSWORDNUM					  11112201				///输入密码
#define BANK_WORD_TEXT				  11112200				///银行输入密码字文本

//-----------------------------------------------------------------------------------

#define SHUANGZHUANGLIST			  12112243				///上庄列表
#define UP_WAITING_LIST_BUTUUON		  12112217				///向上翻按钮
#define DOWN_WAITING_LIST_BUTUUON	  12112218				///向下翻按钮

#define BTN_SET_GAME_INFO             1003500				//游戏设置容器
#define      CONTAIN_GAME_SET         1111501				//容器
#define      BTN_SET_VOICE            1111502				//小勾勾
#define		 BTN_SET_BKVOICE		  1111503				//背景小勾勾
#define      BTN_SET_GAME_OK          1111504				//确定按钮
#define      BTN_SET_GAME_CANCEL      1111505				//取消按钮

//动画
#define      EXT_WIN_ALL_ANI		  11112247				//通杀
#define		 EXT_LOST_ALL_ANI		  11112249				//通赔 

//显示牌形
#define		 SHAPE_TOP				  12112231			//庄家
#define		 SHAPE_TIAN				  12112232			//天
#define		 SHAPE_DI				  12112233			//地
#define		 SHAPE_XUAN				  12112234			//玄
#define		 SHAPE_HUANG			  12112235			//黄

#define		ZHONG_ZHU_TITU				11112134			//总注低图
#define		NT_FLAG						11112137			//自已下注的底图


#define		LIST_USER_NOTE_INFO			11120041	//列表控件

#endif