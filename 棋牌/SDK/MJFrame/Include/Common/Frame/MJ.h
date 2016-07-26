/** @file	MJ.h
 *	@brief	存储和控制麻将数据的类

 *	包括4个类
 *	手牌类：CMJHand
 *	出牌类：CMJGive
 *	城墙类：CMJWall
 *	3者的核心数据都保存在各自的m_ucTile数组中，但长度不同
 *	CMJHand 长度 18
 *	CMJGive 长度 36
 *	CMJWall 长度 144
 *	拦牌类：CMJSet
 *	核心数据保存在结构TILE_SET中，长度4
 */

#ifndef MJ_H_
#define MJ_H_


#define PLAYER_NBR	4

#include <string.h>
#include <cassert>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "mjdef.h"

namespace MyGame
{
	/**
	* @ingroup MJFrame
	* @brief CMJHand
	*
	* <p>　 
	*		CMJHand, 手牌数据存储类
	* </p>
	* @sa
	*/	
    class CMJSet;
	class /*SERAPI*/ CMJHand
	{
	public:
		CMJHand();
		virtual ~CMJHand();

		/**
		*  @brief 设置指定位置上的手牌
		*  @param[in]  nPos 需要设置的位置
		*  @param[in]  ucTile 设置的牌数据
		*  @warning  如果设置的位置超过当前手牌的实际长度，或者设置的牌数据超过最大值TILE_END都会设置失败
		*/	
		void SetTile( int nPos, TILE ucTile );

		/**
		*  @brief 获取指定位置上的牌
		*  @param[in]  nPos 需要获取的位置
		*  @return  如果获取的位置超过当前手牌的实际长度，就会返回TILE_BEGIN
		*/	
		TILE GetTile( int nPos );

		/**
		*  @brief 添加一张手牌
		*  @param[in]  ucTile 添加的牌数据
		*  @param[in]  nNbr 添加的牌数
		*  @param[in]  bIncrease 是否要增序增加
		*  @return  如果设置的位置超过手牌的实际最大长度，或者设置的牌数据超过最大值TILE_END都会添加失败
		*/	
		BOOL AddTile( TILE ucTile, int nNbr = 1, int bIncrease = FALSE);

		/**
		*  @brief 删除一张手牌
		*  @param[in]  ucTile 指定要删除的牌
		*  @param[in]  nNbr 删除的牌数
		*  @param[in]  bIncrease 是否要增序删除
		*  @return  如果没有这张牌就会返回FALSE
		*	@warning 删除操作会打乱牌的顺序
		*/	
		BOOL DelTile( TILE ucTile, int nNbr = 1, int bIncrease = FALSE );

		/**
		*  @brief 添加一组手牌
		*  @param[in]  tiles 手牌数据
		*  @return  如果添加中途长度超过最大长度,返回FALSE
		*/	
		BOOL AddTiles( CMJHand &tiles );

		/**
		*  @brief 删除一组手牌
		*  @param[in]  tiles 手牌数据
		*  @return  如果找不到牌组中的牌,返回FALSE
		*/	
		BOOL DelTiles( CMJHand &tiles );

		/**
		*  @brief 是否被指定的手牌对象包含
		*  @param[in]  tiles 手牌对象
		*  @return  如果本身手牌全部存在于对象tiles中,返回TRUE,否则返回FALSE
		*/	
		BOOL IsSubSet( CMJHand &tiles );

		/**
		*  @brief 是否包含指定的手牌
		*  @param[in]  ucTile    指定的牌
		*  @param[in]  nNbr      包含的数量
		*  @param[in]  bIncrease 是否是增序，最高可查连续数为9，如果检测一个顺子，可以传入参数nNbr = 3, bIncrease = TRUE
		*  @return 如果包含指定数量的牌,返回TRUE,否则返回FALSE
		*/	
		BOOL IsHave( TILE ucTile, int nNbr = 1, int bIncrease = FALSE );

		/**
		*  @brief 是否有花牌
		*  @return 花牌的数量
		*/	
		int IsHaveFlower();

		/**
		*  @brief 排序
		*  @param[in]  nMod 0:升序, 1:降序, 2:按牌张数升序排序, 3:按牌张数降序排序
		*  @param[in]  nGod 0:万能牌不做特殊排序处理,1:万能牌放在序列的头部,2:万能牌放在序列的尾部
		*  @param[in]  tilesGod 万能牌组
		*/	
		void Sort( int nMod = 0, int nGod = 0, CMJHand* tilesGod = NULL );

		/**
		*  @brief 清空数据
		*/	
		void ReleaseAll( void );

		/**
		*  @brief 是否可吃牌
		*  @param[in]  ucTile 指定吃的牌
		*  @param[in]  bWind  风牌是否可吃
        *  @param[in]  bArrow 字牌是否可吃
        *  @param[in/out] mjSet 返回可以吃的牌数据
		*  @return  100:头, 10:中, 1:尾
		*
		*  返回的是10进制数据,如果同时可以做头和尾吃,就会返回101.
		*/	
		int IsCanCollect( TILE ucTile, int bWind, int bArrow, CMJSet & mjSet);

		/**
		*  @brief 交换
		*  @param[in]  nPos1 第一个位置
		*  @param[in]  nPos2 第二个位置
		*  @return  交换成功返回TRUE,否则返回FALSE
		*
		*  交换必须在实际的长度中进行,否则失败
		*/	
		BOOL Swap( int nPos1, int nPos2 );

		/**
		*  @brief 设置当前长度
		*  @param[in]  nCurLength 长度
		*/	
		void CurrentLength( const int& nCurLength );

		/**
		*  @brief 获取长度
		*  @return  当前手牌的实际长度
		*/	
		int CurrentLength( void ) const { return m_nCurrentLength; }

		/**
		*  @brief 获取指定牌的类型
		*  @param[in]  ucTile 牌数据
		*  @return  万:0，筒:1，条:2，字:3，花:4，其他:5
		*
		*  返回值有宏定义可参考
		*/	
		int GetTileGenre( TILE ucTile );

		/**
		*  @brief 获取指定牌的牌点
		*  @param[in]  ucTile 牌数据
		*  @return  去除类型的牌值
		*
		*  返回值有宏定义可参考
		*/	
		int GetTileNumber( TILE ucTile );

        /**
         *  @brief 获取指定牌的拥有的数量
         *  @param[in]  ucTile 牌数据
         *  @return  此牌的数量
         */	
         int GetTileCount( TILE ucTile );

		/**
		*  @brief 弹出手牌的最后一张
		*  @return  如果没牌了返回TILE_BEGIN
		*/	
		TILE PopTile();

		/**
		*  @brief 添加一个刻子
		*  @param[in]  ucTile 刻子的数据
		*
		*  相当于执行了3次添加同一张牌
		*/	
		void AddTriplet( TILE ucTile );

		/**
		*  @brief 添加一个顺子
		*  @param[in]  ucTile 顺子第一张牌数据
		*  @warning 如果越界依然会添加进去,比如添加9万,会实际添加9,10,11(一条)这3个数据
		*/	
		void AddCollect( TILE ucTile );

		/**
		*  @brief 将TILE数据类型装换为BYTE
		*  @param[in]  ucTile 牌数据
		*  @return  BYTE类型的数据
		*/	
		BYTE GetTileToBYTE( TILE ucTile ) { return (BYTE)ucTile; }

		/**
		*  @brief 获取所有牌数据
		*  @param[in] nHandTiles 牌数据
		*  @return  牌长度
		*/	
		int GetAllTile( int nHandTiles[] );

        /**
        *  @brief 重载CMJHand操作符
        *  @param[in] mjHands 给定的手牌
        *	@return 操作后的手牌
        *  @sa
        */		
        //const CMJHand operator = (const CMJHand& mjHands);
        CMJHand operator + (const CMJHand& mjHands);
        CMJHand operator - (const CMJHand& mjHands);
        CMJHand operator += (const CMJHand& mjHands);
        CMJHand operator -= (const CMJHand& mjHands);
	private:
		// 实际数据
		TILE m_ucTile[MAX_TOTAL_TILES];

		// 当前牌长
		int  m_nCurrentLength;
	};

	/**
	* @ingroup MJFrame
	* @brief TILE_SET
	*
	* <p>　 
	*		TILE_SET, 拦牌的数据结构
	* </p>
	*
	*/	
	struct TILE_SET
	{
		TILE m_ucTile[4]; // 吃碰杠的牌数据
		BYTE m_ucChair;   // 吃碰杠谁的
		BYTE m_ucFlag;    // 标记（记录吃碰杠的牌用）

        TILE_SET()
        {
            memset(this, 0, sizeof(TILE_SET));
        }
	};

	/**
	* @ingroup MJFrame
	* @brief CMJSet
	*
	* <p>　 
	*		CMJSet, 拦牌的数据存储和操作
	* </p>
	*
	*/	
	class /*SERAPI*/ CMJSet
	{
	public:
		CMJSet();
		virtual ~CMJSet();

		/**
		*  @brief 添加一组拦牌
		*  @param[in]  ucTile  吃碰杠的牌数据
		*  @param[in]  ucFlag  拦牌标记(吃,碰,杠)
		*  @param[in]  ucChair 拦的目标玩家(如果是吃牌就标记的是吃的哪一张)
		*  @return 成功TRUE,失败FALSE
		*/	
		BOOL AddSet( TILE ucTile[4], BYTE ucFlag, BYTE ucChair );

		/**
		*  @brief 添加一组拦牌
		*  @param[in]  tagSet 拦牌属性
		*  @return 成功TRUE,失败FALSE
		*/	
		BOOL AddSet( TILE_SET tagSet );

		/**
		*  @brief 添加一组拦牌
		*  @param[in]  mjSet 要添加的拦牌数据结构
		*  @return 成功TRUE,失败FALSE
		*/	
		BOOL AddSet( CMJSet& mjSet );

		/**
		*  @brief 获取一组拦牌
		*  @param[in]  nPos 要获取的位置
		*  @param[out]  tileSet 获取的数据结构
		*  @return 成功TRUE,失败FALSE
		*/	
		BOOL GetSet( int nPos, TILE_SET &tileSet );

		/**
		*  @brief 设定一组拦牌
		*  @param[in]  nPos 要设定的位置
		*  @param[in]  tileSet 设定的数据结构
		*  @return 成功TRUE,失败FALSE
		* 
		*  位置不能超过MAX_SET_TILES, 否则设定失败
		*/	
		void SetSet( int nPos, TILE_SET &tileSet );

		/**
		*  @brief 将拦牌数据中的碰牌升级到明杠
		*  @param[in]  ucTile 碰升到杠的牌
		*  @return 如果拦牌中存在这个牌的碰就成功TRUE,否则失败FALSE
		*/	
		BOOL TripletToQuadruplet( TILE ucTile );

		/**
		*  @brief 清空数据
		*/	
		void ReleaseAll( void );

		/**
		*  @brief 设置当前长度
		*  @param[in]  nCurLength 长度
		*/	
		void CurrentLength( const int& nCurLength );

		/**
		*  @brief 获取长度
		*  @return  当前拦牌的实际长度
		*/	
		int CurrentLength( void ) const { return m_nCurrentLength; }

		/**
		*  @brief 设置拦牌中的附属信息
		*  @param[in]  nPos 要设定的位置
		*  @param[in]  pParam 要设置的拦牌附属信息
		*  @return 成功TRUE, 失败FALSE
		*/	
		BOOL SetParam( int nPos, void * pParam );

		/**
		*  @brief 获取拦牌中设置的附属信息
		*  @param[in]  nPos 要获取的位置
		*  @return 失败返回NULL
		*/	
		void * GetParam( int nPos);
	private:
		// 数据
		TILE_SET m_TileSet[MAX_SET_TILES];

		// 扩展数据(提供给地方麻将存储有用的信息, 开发人员自己转换类型), 例: 如延边麻将的杠上杠、杠后杠、东南西北(杠后面追加任意风牌再杠)
		void * m_pParam[MAX_SET_TILES];

		// 长度
		int m_nCurrentLength;
	};

	/**
	* @ingroup MJFrame
	* @brief CMJGive
	*
	* <p>　 
	*		CMJGive, 出牌的数据存储和操作
	* </p>
	*
	* 出牌的数据和手牌除了长度,其他基本一样
	*/	
	class /*SERAPI*/ CMJGive
	{
	public:
		CMJGive();
		~CMJGive();

		/**
		*  @brief 设置出牌数据
		*  @sa CMJGive
		*/	
		void SetTile( int nPos, TILE ucTile );

		/**
		*  @brief 获取出牌数据
		*  @sa CMJGive
		*/	
		TILE GetTile( int nPos );

        /** 
        *  @brief 设置X位置的拦牌信息
        *  @sa CMJGive
        */
        void SetBlock(int nPos, TILE_SET &tileSet);

        /** 
        *  @brief 获取X位置的拦牌信息
        *  @sa CMJGive
        */
        void GetBlock(int nPos, TILE_SET &tileSet);

		/**
		*  @brief 添加一张牌
		*  @param[in]  ucTile 添加的牌数据
		*  @param[in]  nNbr 添加的牌数
		*  @param[in]  bIncrease 是否要增序增加
		*  @return  如果设置的位置超过牌的实际最大长度，或者设置的牌数据超过最大值TILE_END都会添加失败
		*/	
		BOOL AddTile( TILE ucTile, int nNbr = 1, int bIncrease = FALSE);

		/**
		*  @brief 删除一张手牌
		*  @param[in]  ucTile 指定要删除的牌
		*  @param[in]  nNbr 删除的牌数
		*  @param[in]  bIncrease 是否要增序删除
		*  @return  如果没有这张牌就会返回FALSE
		*  @warning 删除操作会打乱牌的顺序
		*/	
		BOOL DelTile( TILE ucTile, int nNbr = 1, int bIncrease = FALSE );

		/**
		*  @brief 添加一组出牌
		*  @sa CMJGive
		*/	
		BOOL AddTiles( CMJGive &tiles );

		/**
		*  @brief 删除一组出牌数据
		*  @sa CMJGive
		*/	
		BOOL DelTiles( CMJGive &tiles );

		/**
		*  @brief 是否包含指定的牌
		*  @param[in]  ucTile    指定的牌
		*  @param[in]  nNbr      包含的数量
		*  @param[in]  bIncrease 是否是增序，最高可查连续数为9，如果检测一个顺子，可以传入参数nNbr = 3, bIncrease = TRUE
		*  @return 如果包含指定数量的牌,返回TRUE,否则返回FALSE
		*/	
		BOOL IsHave( TILE ucTile, int nNbr = 1, int bIncrease = FALSE );

		/**
		*  @brief 清空牌组
		*  @sa CMJGive
		*/	
		void ReleaseAll( void );

		/**
		*  @brief 设置出牌的长度
		*  @sa CMJGive
		*/	
		void CurrentLength( const int& nCurLength );

		/**
		*  @brief 获取出牌数据的长度
		*  @sa CMJGive
		*/	
		int CurrentLength( void ) const { return m_nCurrentLength; }

		/**
		*  @brief 交换指定2个位置上的牌数据
		*  @sa CMJGive
		*/	
		BOOL Swap( int nPos1, int nPos2 );

		/**
		*  @brief 获取所有牌数据
		*  @param[in] nGiveTiles 牌数据
		*  @return  牌长度
		*/	
		int GetAllTile( int nGiveTiles[] );

	private:
		// 实际数据
		TILE m_ucTile[MAX_GIVE_TILES];

        // 拦牌数据, 配合3D界面
        TILE_SET m_TileSet[MAX_GIVE_TILES];

		// 当前牌长
		int  m_nCurrentLength;
	};

	/**
	* @ingroup MJFrame
	* @brief CMJWall
	*
	* <p>　 
	*		CMJWall, 城墙的数据存储和操作
	* </p>
	*
	* 城墙的长度达144张牌,具有自己的洗牌功能,其他操作基本和手牌一样
	*/	
	class /*SERAPI*/ CMJWall
	{
	public:
		CMJWall();
		virtual ~CMJWall();

		/**
		*  @brief 清空数据
		*  @sa CMJWall
		*/	
		void ReleaseAll();

		/**
		*  @brief 填充数据
		*  @param[in]  nLength 要填充的长度
		*  @param[in]  ucClear 是否要全部填充成指定的牌,为0表示自动按顺序填充
		*
		*  如果ucClear是默认为0就会将指定长度的牌按万,条,筒,字,花的顺序填充144张,但当前长度会设置为指定的nLength
		*/	
		void FillTiles( int nLength = 0, BYTE ucClear = 0 );

		/**
		*  @brief 洗牌
		*
		*  打乱当前长度的牌的顺序
		*/	
		void RandomTiles();

		/**
		*  @brief 交换指定2个位置的牌
		*  @sa CMJWall
		*/	
		BOOL Swap( int nPos1, int nPos2 );

		/**
		*  @brief 添加一张牌
		*  @param[in]  ucTile 添加的牌数据
		*  @param[in]  nNbr 添加的牌数
		*  @param[in]  bIncrease 是否要增序增加
		*  @return  如果设置的位置超过牌的实际最大长度，或者设置的牌数据超过最大值TILE_END都会添加失败
		*/	
		BOOL AddTile( TILE ucTile, int nNbr = 1, int bIncrease = FALSE);

		/**
		*  @brief 删除一张手牌
		*  @param[in]  ucTile 指定要删除的牌
		*  @param[in]  nNbr 删除的牌数
		*  @param[in]  bIncrease 是否要增序删除
		*  @return  如果没有这张牌就会返回FALSE
		*  @warning 删除操作会打乱牌的顺序
		*/	
		BOOL DelTile( TILE ucTile, int nNbr = 1, int bIncrease = FALSE );

		/**
		*  @brief 添加一组牌
		*  @sa CMJWall
		*/	
		BOOL AddTiles( CMJWall &tiles );

		/**
		*  @brief 删除一组牌
		*  @sa CMJWall
		*/	
		BOOL DelTiles( CMJWall &tiles );

		/**
		*  @brief 设定指定位置上的牌
		*  @sa CMJWall
		*/	
		void SetTile( int nPos, TILE ucTile );

		/**
		*  @brief 获取指定位置上的牌
		*  @sa CMJWall
		*/	
		TILE GetTile( int nPos );

		/**
		*  @brief 是否包含指定的牌
		*  @param[in]  ucTile    指定的牌
		*  @param[in]  nNbr      包含的数量
		*  @param[in]  bIncrease 是否是增序，最高可查连续数为9，如果检测一个顺子，可以传入参数nNbr = 3, bIncrease = TRUE
		*  @return 如果包含指定数量的牌,返回TRUE,否则返回FALSE
		*/	
		BOOL IsHave( TILE ucTile, int nNbr = 1, int bIncrease = FALSE );

		/**
		*  @brief 弹出最后一张牌
		*  @sa CMJWall
		*/	
		TILE PopTile();

        /**
        *  @brief 弹出最前一张牌
		*  @param[in] nPos 弹牌的位置
        *  @sa CMJWall
        */	
        TILE PopFrontTile(int nPos = -1);

		/**
		*  @brief 设置城墙长度
		*  @sa CMJWall
		*/	
		void CurrentLength( const int &nCurLength );

		/**
		*  @brief 获取当前城墙长度
		*  @sa CMJWall
		*/	
		//int CurrentLength( void ) const { return m_nCurrentLength; }
		int CurrentLength();

		/**
		*  @brief 设置抓牌与补牌的位置
		*  @param[in] nStartPos 抓牌位置
		*  @param[in] nEndPos   补牌位置
		*  @sa CMJWall
		*/	
		void SetPos(int nStartPos, int nEndPos);

		/**
		*  @brief 设置抓牌位置
		*  @sa CMJWall
		*/	
		void SetStartPos(int nPos);

		/**
		*  @brief 设置补牌位置
		*  @sa CMJWall
		*/	
		void SetEndPos(int nPos);

		/**
		*  @brief 获取抓牌位置
		*  @sa CMJWall
		*/	
		int GetStartPos();

		/**
		*  @brief 设置补牌位置
		*  @param[in] nOffset 获取补牌的偏移位置
		*  @sa CMJWall
		*/	
		int GetEndPos(int nOffset = 0);
	private:
		// 数据
		TILE m_ucTile[MAX_TOTAL_TILES];

		// 长度
		int m_nCurrentLength;

		// 当前抓牌位置
		int m_nStartPos;

		// 补牌位置(结束位置)
		int m_nEndPos;
	};
}

#endif
