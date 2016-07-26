/** @file	MJFanCounter.h
 *	@brief	番的管理类
 *	@author duanxiaohui
 *	@date 2010.09.28
 *
 *  算番用的函数，包含了大多数现有番型，并可以通过自由组合而形成新的番型
 */
#ifndef ___MJFANCOUNTER_H___
#define ___MJFANCOUNTER_H___

#include "mjcfgdef.h"
#include "MJFrameBaseData.h"

namespace MyGame
{
/* 游戏总共定义128种番型
*  框架实现的番型占用ID位置 0-103种, 105-127的ID位置留给游戏开发程序员扩充特色番型使用
*/

// 函数的ID映射
#define FAN_ID_QINGYISE       0     // 清一色
#define FAN_ID_HUNYISE		  1	    // 混一色
#define FAN_ID_QIDUI		  2   	// 7对
#define FAN_ID_DUAN19		  3	    // 断19
#define FAN_ID_BUQIUREN		  4	    // 不求人
#define FAN_ID_QUAN19		  5	    // 混19
#define FAN_ID_QUANQIUREN	  6	    // 全求人
#define FAN_ID_PINGHU		  7	    // 平和
#define FAN_ID_ZIMO			  8	    // 自摸
#define FAN_ID_KANZHANG		  9	    // 坎张
#define FAN_ID_BIANZHANG	  10	// 边张
#define FAN_ID_WUZI			  11	// 无字
#define FAN_ID_QUEYIMEN		  12	// 缺一门
#define FAN_ID_HUAPAI		  13	// 计算花牌
#define FAN_ID_DANDIAO		  14	// 单钓
#define FAN_ID_MINGGANG		  15	// 明杠
#define FAN_ID_ANGANG		  16	// 暗杠
#define FAN_ID_LIANLIU		  17	// 连六
#define FAN_ID_LAOSHAOFU	  18	// 老少副
#define FAN_ID_1SE2TONGSHUN	  19	// 1色2同顺(一般高)
#define FAN_ID_1SE3TONGSHUN	  20	// 1色3同顺
#define FAN_ID_1SE4TONGSHUN	  21	// 1色4同顺
#define FAN_ID_2SE2TONGSHUN	  22	// 2色2同顺
#define FAN_ID_3SE3TONGSHUN	  23	// 3色3同顺
#define FAN_ID_19KE			  24	// 19刻(序数牌)
#define FAN_ID_QUANSHUN		  25	// 全顺
#define FAN_ID_2ANKE		  26	// 双暗刻
#define FAN_ID_3ANKE		  27	// 三暗刻
#define FAN_ID_3FENGKE		  28	// 3风刻
#define FAN_ID_2TONGKE		  29	// 2同刻
#define FAN_ID_3TONGKE		  30	// 3同刻
#define FAN_ID_ALLKE		  31	// 碰碰胡
#define FAN_ID_4GUI1		  32	// 4归1
#define FAN_ID_MENFENGKE	  33	// 门风刻
#define FAN_ID_QUANFENGKE	  34	// 圈风刻
#define FAN_ID_JIANKE		  35	// 箭刻
#define FAN_ID_JUEZHANG		  36	// 绝张
#define FAN_ID_2MINGGANG	  37	// 双明杠
#define FAN_ID_3GANG		  38	// 3杠
#define FAN_ID_4GANG		  39	// 4杠
#define FAN_ID_5MENQI		  40	// 5门齐
#define FAN_ID_1SE3BU		  41	// 1色3步高
#define FAN_ID_1SE4BU		  42	// 1色4步高
#define FAN_ID_3SE3BU		  43	// 3色3步高
#define FAN_ID_2ANGANG		  44	// 双暗杠
#define FAN_ID_QIANGGANG	  45	// 抢杠
#define FAN_ID_GANGSHANGHUA	  46	// 杠上花
#define FAN_ID_GANGSHANGPAO	  47	// 杠上炮
#define FAN_ID_HAIDIPAO		  48	// 海底炮胡
#define FAN_ID_HAIDIZIMO	  49	// 海底自摸
#define FAN_ID_TUIDAOHU		  50	// 推倒胡
#define FAN_ID_TUIBUDAO		  51	// 推不倒
#define FAN_ID_3SELONG		  52	// 花龙
#define FAN_ID_1SELONG		  53	// 青龙
#define FAN_ID_QUANXIAO		  54	// 小
#define FAN_ID_QUANZHONG	  55	// 中
#define FAN_ID_QUANDA		  56	// 大
#define FAN_ID_2SE1TONGSHUN	  57	// 2色1同顺
#define FAN_ID_QUANSHUANG	  58	// 全双
#define FAN_ID_QUANDAN		  59	// 全单
#define FAN_ID_QUANBUKAO	  60	// 全不靠
#define FAN_ID_1SE3JIEGAO	  61	// 一色3节高
#define FAN_ID_1SE4JIEGAO	  62	// 一色4节高
#define FAN_ID_3SE3JIEGAO	  63	// 3色3节高
#define FAN_ID_QUANZI		  64	// 字一色
#define FAN_ID_XIAOSHANYUAN	  65	// 小三元
#define FAN_ID_DASHANYUAN	  66	// 大三元
#define FAN_ID_XIAOSIXI		  67	// 小四喜
#define FAN_ID_DASIXI		  68	// 大四喜
#define FAN_ID_131			  69	// 131
#define FAN_ID_9LIAN		  70	// 9莲宝灯
#define FAN_ID_LVYISE		  71	// 绿一色
#define FAN_ID_XIAOYU5		  72	// 小于5
#define FAN_ID_DAYU5		  73	// 大于5
#define FAN_ID_ZUHELONG		  74	// 组合龙
#define FAN_ID_7XING		  75	// 7星不靠
#define FAN_ID_QUANDAI5		  76	// 全带5
#define FAN_ID_SHUANGLONG	  77	// 双龙
#define FAN_ID_TIANHU		  78	// 天胡
#define FAN_ID_DIHU			  79	// 地胡
#define FAN_ID_RENHU		  80	// 人胡
#define FAN_ID_BAIWANSHI	  81	// 百万石
#define FAN_ID_HONGKONGQUE	  82	// 红孔雀
#define FAN_ID_DONGBEI		  83	// 东北新干线
#define FAN_ID_KONGQUEDNF     84	// 孔雀东南飞
#define FAN_ID_SHUANGJIANKE	  85	// 双箭刻
#define FAN_ID_YISHUANGLONG	  86	// 一色双龙会
#define FAN_ID_LIANQIDUI	  87	// 连7对
#define FAN_ID_TING			  88	// 听牌
#define FAN_ID_TIANTING		  89	// 天听
#define FAN_ID_LONGQIDUI	  90	// 龙7对
#define FAN_ID_YOUZI		  91	// 有字
#define FAN_ID_PAOHU		  92	// 炮胡
#define FAN_ID_DUTING		  93	// 独听
#define FAN_ID_DUYIDUI		  94	// 独一对
#define FAN_ID_258JIANG		  95	// 258将
#define FAN_ID_ZHUANGJIA	  96	// 庄家
#define FAN_ID_DADIAOCHE	  97	// 大吊车
#define FAN_ID_QING19		  98	// 青幺九
#define FAN_ID_4ANKE          99    // 四暗刻
#define FAN_ID_XIXIANGFENG   100    // 喜相逢
#define FAN_ID_MQQ           101    // 门前清
#define FAN_ID_QUANDAI1		 102    // 全带幺
#define FAN_ID_SANSHUANGLONG 103    // 三色双龙会
#define FAN_ID_FENG19KE		 104	// 19刻(风牌)
#define FAN_ID_PIHU          105    // 屁胡

/**                          
 * @ingroup MJFrame          
 * @brief 算番用的环境数据结 构
 */	                         
typedef	struct tagENVIRONMENT
{                            
	BYTE	byChair;		 				// 检查谁的
	BYTE	byTurn;			 				// 轮到谁，如果是点炮，则是点炮的那个人
	BYTE	tHand[4][MAX_HAND_TILES];		// 四家手上的牌
	BYTE	byHandCount[4];	 				// 手上有几张牌
                             
	BYTE	tSet[4][MAX_SET_TILES][3];		// 四家，4手牌，flag、tile、chair
	BYTE	bySetCount[4];	 				// set有几手牌
                             
	BYTE	tGive[4][40];	 				// 四家出过的牌
	BYTE	byGiveCount[4];	 				// 每人出了几张牌
                             
	BYTE	tLast;			 				// 最后和的那张牌
	BYTE	tGodTile;		 				// 万能牌
	BYTE	byFlag;			 				// 0自摸、1点炮、2杠上花、3抢杠
	BYTE    ucDealer;		 				// 庄家
	BYTE    ucDice[DICE_NBR];				// 色子
	BYTE	byRoundWind;	 				// 圈风
	BYTE	byPlayerWind;	 				// 门风
	BYTE	byTilesLeft;	 				// 还剩多少张牌，用来计算海底等
	BYTE    byCollectWind;	 				// 风牌也可以吃(东南西北)
    BYTE    byCollectArrow;                 // 箭头牌也可以吃(中发白)

	BYTE	byFlowerCount[4];				// 4家各有多少张花	
	BYTE	byTing[4];						// 听牌的玩家
	BYTE	byOtherInfo[4];					// 其他消息
	BYTE    byOtherInfo2[4];				// 备用消息
}ENVIRONMENT;


/**
 * @ingroup MJFrame
 * @brief CMJFanCounter
 *
 * <p>　 
 *		CMJFanCounter, 番的管理类
 * </p>
 * @sa
 */	
class /*SERAPI*/ CMJFanCounter  
{
public:
	CMJFanCounter();
	virtual ~CMJFanCounter();

	// 规则使用的函数指针
	typedef BOOL (*CHECKFUNC)(CMJFanCounter*);

	/**
	 *  @brief 获取番数
	 *  @return  计算出来的总番数
	 */	
	virtual int GetFanCount();

	/**
	 *  @brief 设置算番的环境
	 *  @param[in]  env 环境的数据
	 */	
	virtual void SetEnv( ENVIRONMENT& env );

	/**
	 *  @brief 设置所有结点信息
	 *  @param[in]  tagFanCount 番结点的数据
	 */	
	virtual void SetFanCount( FAN_COUNT& tagFanCount );

	/**
	 *  @brief 重新设置CHECK结点
	 *
	 *  重新设置结点的CHECK属性,以确保下次算番的时候都能遍历到
	 */	
	virtual void ResetFanCount();

	/**
	 *  @brief 获得指定玩家的手牌
	 *  @param[out]  mjHand 以引用方式把牌设置给它
	 *  @param[in]  nChair 请求获取的玩家
	 * 
	 *  获取数据的来源是设置的环境数据结构
	 */	
	virtual void GetHandTiles( CMJHand& mjHand, int nChair );

	/**
	 *  @brief 获得指定玩家的全部手牌
	 *  @param[out]  mjAll 以引用方式把牌设置给它
	 *  @param[in]  nChair 请求获取的玩家
	 *
	 *  获取数据的来源是设置的环境数据结构,此函数会把玩家的拦牌整合到手牌中一起返回,杠被整合会刻子
	 */	
	virtual void GetAllTiles( CMJHand& mjAll, int nChair );

	/**
	 *  @brief 检查一个番的结点
	 *  @param[in/out]  tagFan 结点的数据结构
	 *
	 *  如果成立，自动设置结果为TRUE，并关闭所有配置的成立后就关闭的番型
	 */	
	virtual void CheckFanNode( FAN_NODE &tagFan );

	/**
	 *  @brief 检查参数ID的影射函数是否成立
	 *  @param[in]  nID 被影射的番ID
	 *  @return 如果成立返回TRUE,否则返回FALSE
	 */	
	virtual BOOL CheckFanByID( int nID );

	/**
	 *  @brief 检查普通胡牌
	 *  @param[in]  tilesHand 手牌数据
	 *  @param[in]  bNoJiang 是否要判断将牌,如果为TRUE,则不允许有将牌
	 *  @return 如果成立返回TRUE,否则返回FALSE]
	 */	
	BOOL CheckWinNormal(CMJHand tilesHand, BOOL bNoJiang = FALSE );

	/**
	 *  @brief 检查全顺子胡牌
	 *  @param[in]  tilesHand 手牌数据
	 *  @return 如果成立返回TRUE,否则返回FALSE
	 */	
	BOOL CheckWinCollect(CMJHand tilesHand);

	/**
	 *  @brief 检查全刻子胡牌
	 *  @param[in]  tilesHand 手牌数据
	 *  @return 如果成立返回TRUE,否则返回FALSE
	 */	
	static BOOL CheckWinTriplet(CMJHand tilesHand);

    /**
    *  @brief 检查全带X胡牌(X为某个牌点)
    *  @param[in]  tilesHand 手牌数据
    *  @param[in]  byNum     判断每组及将牌要包含的牌点
    *  @return 如果成立返回TRUE,否则返回FALSE
    */	
    static BOOL CheckWinWithX(CMJHand tilesHand, BYTE byNum);
	
	/**
	 *  @brief 检查纯全19胡牌
	 *  @param[in]  tilesHand 手牌数据
	 *  @return 如果成立返回TRUE,否则返回FALSE
	 */	
	static BOOL CheckWinWith19(CMJHand tilesHand);

	// 各种番
	static BOOL Check000(CMJFanCounter* pCounter);
	static BOOL Check001(CMJFanCounter* pCounter);
	static BOOL Check002(CMJFanCounter* pCounter);
	static BOOL Check003(CMJFanCounter* pCounter);
	static BOOL Check004(CMJFanCounter* pCounter);
	static BOOL Check005(CMJFanCounter* pCounter);
	static BOOL Check006(CMJFanCounter* pCounter);
	static BOOL Check007(CMJFanCounter* pCounter);
	static BOOL Check008(CMJFanCounter* pCounter);
	static BOOL Check009(CMJFanCounter* pCounter);

	static BOOL Check010(CMJFanCounter* pCounter);
	static BOOL Check011(CMJFanCounter* pCounter);
	static BOOL Check012(CMJFanCounter* pCounter);
	static BOOL Check013(CMJFanCounter* pCounter);
	static BOOL Check014(CMJFanCounter* pCounter);
	static BOOL Check015(CMJFanCounter* pCounter);
	static BOOL Check016(CMJFanCounter* pCounter);
	static BOOL Check017(CMJFanCounter* pCounter);
	static BOOL Check018(CMJFanCounter* pCounter);
	static BOOL Check019(CMJFanCounter* pCounter);

	static BOOL Check020(CMJFanCounter* pCounter);
	static BOOL Check021(CMJFanCounter* pCounter);
	static BOOL Check022(CMJFanCounter* pCounter);
	static BOOL Check023(CMJFanCounter* pCounter);
	static BOOL Check024(CMJFanCounter* pCounter);
	static BOOL Check025(CMJFanCounter* pCounter);
	static BOOL Check026(CMJFanCounter* pCounter);
	static BOOL Check027(CMJFanCounter* pCounter);
	static BOOL Check028(CMJFanCounter* pCounter);
	static BOOL Check029(CMJFanCounter* pCounter);

	static BOOL Check030(CMJFanCounter* pCounter);
	static BOOL Check031(CMJFanCounter* pCounter);
	static BOOL Check032(CMJFanCounter* pCounter);
	static BOOL Check033(CMJFanCounter* pCounter);
	static BOOL Check034(CMJFanCounter* pCounter);
	static BOOL Check035(CMJFanCounter* pCounter);
	static BOOL Check036(CMJFanCounter* pCounter);
	static BOOL Check037(CMJFanCounter* pCounter);
	static BOOL Check038(CMJFanCounter* pCounter);
	static BOOL Check039(CMJFanCounter* pCounter);

	static BOOL Check040(CMJFanCounter* pCounter);
	static BOOL Check041(CMJFanCounter* pCounter);
	static BOOL Check042(CMJFanCounter* pCounter);
	static BOOL Check043(CMJFanCounter* pCounter);
	static BOOL Check044(CMJFanCounter* pCounter);
	static BOOL Check045(CMJFanCounter* pCounter);
	static BOOL Check046(CMJFanCounter* pCounter);
	static BOOL Check047(CMJFanCounter* pCounter);
	static BOOL Check048(CMJFanCounter* pCounter);
	static BOOL Check049(CMJFanCounter* pCounter);

	static BOOL Check050(CMJFanCounter* pCounter);
	static BOOL Check051(CMJFanCounter* pCounter);
	static BOOL Check052(CMJFanCounter* pCounter);
	static BOOL Check053(CMJFanCounter* pCounter);
	static BOOL Check054(CMJFanCounter* pCounter);
	static BOOL Check055(CMJFanCounter* pCounter);
	static BOOL Check056(CMJFanCounter* pCounter);
	static BOOL Check057(CMJFanCounter* pCounter);
	static BOOL Check058(CMJFanCounter* pCounter);
	static BOOL Check059(CMJFanCounter* pCounter);

	static BOOL Check060(CMJFanCounter* pCounter);
	static BOOL Check061(CMJFanCounter* pCounter);
	static BOOL Check062(CMJFanCounter* pCounter);
	static BOOL Check063(CMJFanCounter* pCounter);
	static BOOL Check064(CMJFanCounter* pCounter);
	static BOOL Check065(CMJFanCounter* pCounter);
	static BOOL Check066(CMJFanCounter* pCounter);
	static BOOL Check067(CMJFanCounter* pCounter);
	static BOOL Check068(CMJFanCounter* pCounter);
	static BOOL Check069(CMJFanCounter* pCounter);

	static BOOL Check070(CMJFanCounter* pCounter);
	static BOOL Check071(CMJFanCounter* pCounter);
	static BOOL Check072(CMJFanCounter* pCounter);
	static BOOL Check073(CMJFanCounter* pCounter);
	static BOOL Check074(CMJFanCounter* pCounter);
	static BOOL Check075(CMJFanCounter* pCounter);
	static BOOL Check076(CMJFanCounter* pCounter);
	static BOOL Check077(CMJFanCounter* pCounter);
	static BOOL Check078(CMJFanCounter* pCounter);
	static BOOL Check079(CMJFanCounter* pCounter);
	
	static BOOL Check080(CMJFanCounter* pCounter);
	static BOOL Check081(CMJFanCounter* pCounter);
	static BOOL Check082(CMJFanCounter* pCounter);
	static BOOL Check083(CMJFanCounter* pCounter);
	static BOOL Check084(CMJFanCounter* pCounter);
	static BOOL Check085(CMJFanCounter* pCounter);
	static BOOL Check086(CMJFanCounter* pCounter);
	static BOOL Check087(CMJFanCounter* pCounter);
	static BOOL Check088(CMJFanCounter* pCounter);
	static BOOL Check089(CMJFanCounter* pCounter);
	
	static BOOL Check090(CMJFanCounter* pCounter);
	static BOOL Check091(CMJFanCounter* pCounter);
	static BOOL Check092(CMJFanCounter* pCounter);
	static BOOL Check093(CMJFanCounter* pCounter);
	static BOOL Check094(CMJFanCounter* pCounter);
	static BOOL Check095(CMJFanCounter* pCounter);
	static BOOL Check096(CMJFanCounter* pCounter);
	static BOOL Check097(CMJFanCounter* pCounter);
	static BOOL Check098(CMJFanCounter* pCounter);
	static BOOL Check099(CMJFanCounter* pCounter);

	static BOOL Check100(CMJFanCounter* pCounter);
	static BOOL Check101(CMJFanCounter* pCounter);
	static BOOL Check102(CMJFanCounter* pCounter);
	static BOOL Check103(CMJFanCounter* pCounter);
	static BOOL Check104(CMJFanCounter* pCounter);
	static BOOL Check105(CMJFanCounter* pCounter);
	static BOOL Check106(CMJFanCounter* pCounter);
	static BOOL Check107(CMJFanCounter* pCounter);
	static BOOL Check108(CMJFanCounter* pCounter);
	static BOOL Check109(CMJFanCounter* pCounter);

	static BOOL Check110(CMJFanCounter* pCounter);
	static BOOL Check111(CMJFanCounter* pCounter);
	static BOOL Check112(CMJFanCounter* pCounter);
	static BOOL Check113(CMJFanCounter* pCounter);
	static BOOL Check114(CMJFanCounter* pCounter);
	static BOOL Check115(CMJFanCounter* pCounter);
	static BOOL Check116(CMJFanCounter* pCounter);
	static BOOL Check117(CMJFanCounter* pCounter);
	static BOOL Check118(CMJFanCounter* pCounter);
	static BOOL Check119(CMJFanCounter* pCounter);

	static BOOL Check120(CMJFanCounter* pCounter);
	static BOOL Check121(CMJFanCounter* pCounter);
	static BOOL Check122(CMJFanCounter* pCounter);
	static BOOL Check123(CMJFanCounter* pCounter);
	static BOOL Check124(CMJFanCounter* pCounter);
	static BOOL Check125(CMJFanCounter* pCounter);
	static BOOL Check126(CMJFanCounter* pCounter);
	static BOOL Check127(CMJFanCounter* pCounter);

	// 算番的环境
	ENVIRONMENT m_tagEnv;
	FAN_COUNT m_tagFanCount;

    // 当前判断的番型ID
    BYTE m_byFanID;

	// 记录的函数指针
	CHECKFUNC m_fpCheckFan[MAX_FAN_NUMBER];

};

}

#endif
