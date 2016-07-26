// 用来定义界面编辑器编辑出来的控件列表中的ID号，其数值应当与界面编辑器中保持一致
// 部分由框架处理的ID号应当另行安排，可以复制到框架中代码中去，为保证框架的灵活性
// 不将此文件放到SDK目录下，而是放到具体游戏中
#ifndef _DEFINES_H
#define _DEFINES_H

//////////////////////////浦城炸弹//////////////////////////////

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

//按钮
#define		BN_RESULT_GOON				1111125 //继续按钮
#define		BN_RESULT_LEAVE				1111126 //离开按钮


/////////////////////游戏/////////////////////


//准备图标 1111150	—— 1111159
#define     IMG_READY_MARK_0			1111150 //准备图标

//手牌控件 1111160	—— 1111169
#define     HAND_CARD_0					1111160 

//玩家升起牌控件 1111170	—— 1111179
#define     UP_CARD_0					1111170 

//玩家降下牌控件 1111180	—— 1111189
#define     DOWN_CARD_0					1111180

//摊牌图片	1111190	—— 1111199
#define		IMG_TAN_PAI_0				1111190	

//计时器	1111200	—— 1111209
#define     TIME_0						1111200 //计时器

//计时器容器	1111205	—— 1111209
#define     TIME_CONTAINER_0			1111205 //计时器


//牌型图标 1111210	—— 1111219
#define     IMG_CARD_SHAPE_0			1111210 //牌型图标

//底框 1111220	—— 1111229
#define     IMG_DI_KUANG_0				1111220 //牌型图标

//托管 1111230	—— 1111239
#define     IMG_AUTO_0					1111230 //牌型图标




//按钮控件
#define		BN_GAME_START				1111400 //开始按钮
#define		BN_CUE						1111403 //提示按钮
#define		BN_CHONG_BAI				1111405 //重摆按钮
#define		BN_TAN_PAI					1111406 //摊牌按钮
#define		BN_TUO_GUAN					1111407 //托管按钮
#define		BN_CTUO_GUAN				1111408 //取消托管按钮

#define		INM_BASE_POINT				1111450	//底分
#define		IMG_BASE_POINT_MARK			1111451	//底分标志图片


/////////////////////设置框/////////////////////
#define		BN_GAME_SET					  1003500 //游戏设置按钮
#define		SET_CONTAIN					  1111501 //游戏设置容器
#define     SOUND_EFFECT				  1111502 //声音效果
#define		PANG_GUAN					  1111503 //旁观
#define		BN_SET_OK					  1111504 //确定按钮
#define		BN_SET_CANCEL				  1111505 //取消按钮

///<单牛动画
#define		BN_ONE_CATTLE_ANIME_0			  1111600 ///<单牛动画0
#define		BN_ONE_CATTLE_ANIME_1			  1111601 ///<单牛动画1
#define		BN_ONE_CATTLE_ANIME_2			  1111602 ///<单牛动画2
#define		BN_ONE_CATTLE_ANIME_3			  1111603 ///<单牛动画3
#define		BN_ONE_CATTLE_ANIME_4			  1111604 ///<单牛动画4
#define		BN_ONE_CATTLE_ANIME_5			  1111605 ///<单牛动画5
///<双牛动画
#define		BN_TWO_CATTLE_ANIME			  1111606 ///<双牛动画

///<超端
#define		CHAO_DUAN_1				  11112042 ///<超端容器
#define		CHAO_DUAN_BU1			  11112043 ///<按钮玩家1赢
#define		CHAO_DUAN_BU2			  11112044 ///<按钮玩家2赢
#define		CHAO_DUAN_BU3			  11112045 ///<按钮玩家3赢
#define		CHAO_DUAN_BU4			  11112046 ///<按钮玩家4赢
#define		CHAO_DUAN_BU5			  11112047 ///<按钮玩家5赢
#define		CHAO_DUAN_BU6			  11112048 ///<按钮玩家6赢

#define		CHAO_DUAN_BU7			  11112049 ///<按钮玩家1输
#define		CHAO_DUAN_BU8			  11112050 ///<按钮玩家2输
#define		CHAO_DUAN_BU9			  11112051 ///<按钮玩家3输
#define		CHAO_DUAN_BU10			  11112052 ///<按钮玩家4输
#define		CHAO_DUAN_BU11			  11112053 ///<按钮玩家5输
#define		CHAO_DUAN_BU12			  11112054 ///<按钮玩家6输

#define		CHAO_DUAN_BU13			  11112055 ///<按钮重置
#define		CHAO_DUAN_BU14			  11112056 ///<按钮确定
#define		CHAO_DUAN_BU15			  11112057 ///<按钮超端

#define		CHAO_DUAN_IMG1			  11112058///<成功提示
#define		CHAO_DUAN_IMG2			  11112059///<失败提示

#define		CHAO_DUAN_IMG3			  11112060///<按钮提示

#endif