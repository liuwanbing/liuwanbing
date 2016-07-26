// ***************************************************************
//  SoundsDef   version:  1.0   ·  date: 7/06/2011
//  -------------------------------------------------------------

//	麻将游戏基本数据定义,适合大部分麻将游戏开发使用\

//  -------------------------------------------------------------
//  Copyright (C) 2011 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************
#pragma once

#ifndef MJCommon_DEF_HEAD_FILE

#define TAG_INIT(tagName) \
	tagName(){Clear();} \
	void Clear()

//游戏数据长度相关

#define MJ_MAX_CARDS				136		//麻将张数
#define MJ_MAX_WALLCARD				34		//牌墙能容纳的麻将控件数
#define MJ_MAX_WALLDUN				17		//牌墙能容纳的麻将墩数(数值是MJ_MAX_WALLCARD的一半)
#define MJ_MAX_HANDCARD				17		//手牌能容纳的麻将控件数
#define MJ_MAX_HUA					24		//花牌的数量
#define MJ_MAX_OUTED				68		//最大的出牌数量
#define MJ_MAX_CGP					7		//最大的栏牌数
#define MJ_MAX_HUTYPE				8		//牌型种数

namespace MjBase
{
	//关于风位的定义
	namespace MjDirect
	{
		const BYTE East	 = 0;	//东
		const BYTE South = 1;	//南
		const BYTE West	 = 2;	//西
		const BYTE North = 3;	//北
		const BYTE Unknown = 255;	//未知方向
	}
	
	//关于麻将牌值的定义
	namespace MjKind
	{
		const BYTE Wan	= 0;	//万
		const BYTE Tiao	= 10;	//条
		const BYTE Bing	= 20;	//饼
		const BYTE Feng	= 30;	//风
		const BYTE Hua	= 40;	//花 
		const BYTE Unknown = 255;	//未知花型
	}

	namespace MjFeng
	{
		const BYTE Dong	 = 1;	//东
		const BYTE Nan	 = 2;	//南
		const BYTE Xi	 = 3;	//西
		const BYTE Bei	 = 4;	//北
		const BYTE Zhong = 5;	//中
		const BYTE Fa	 = 6;	//发
		const BYTE Bai	 = 7;	//白
	}

	namespace MjHua
	{
		const BYTE Chun	= 1;	//春
		const BYTE Xia	= 2;	//夏
		const BYTE Qiu	= 3;	//秋
		const BYTE Dong	= 4;	//冬
		const BYTE Mei	= 5;	//梅
		const BYTE Lan	= 6;	//兰
		const BYTE Zhu	= 7;	//竹
		const BYTE Ju	= 8;	//菊
	}

	//通过牌值返回牌的类型
	static BYTE GetKind(BYTE byPs)
	{
		if (byPs >= 1 && byPs <= 9)
		{
			return MjKind::Wan;
		}
		else if (byPs >= 11 && byPs <= 19)
		{
			return MjKind::Tiao;
		}
		else if (byPs >= 21 && byPs <= 29)
		{
			return MjKind::Bing;
		}
		else if (byPs >= 31 && byPs <= 37)
		{
			return MjKind::Feng;
		}
		else if (byPs >= 41 && byPs <= 48)
		{
			return MjKind::Hua;
		}

		return MjKind::Unknown;
	}

	//判断一张牌是否有效
	static bool Verify(BYTE byPs)
	{
		return (MjKind::Unknown != GetKind(byPs));
	}

	//通过牌值返回牌的点数
	static BYTE GetDian(BYTE byPs)
	{
		return byPs % 10;
	}

	//取牌的信息，从牌墙取牌后，建议返回这个结构。
	struct TMjFetch
	{
		BYTE val;	//牌值
		BYTE from;	//从那个玩家门前的牌城取
		BYTE x;
		BYTE y;
		BYTE left; //牌墙剩余的牌数

		TMjFetch()
		{
			::memset(this,0,sizeof(TMjFetch));
		}
	};

	//游戏中动作的标识，一般来说动作优先级别越高，开关位越高
	namespace MjActFlag
	{
		const USHORT usGuo  = 0x0002L;  //过
		const USHORT usChi  = 0x0004L;  //吃
		const USHORT usPeng = 0x0008L;  //碰
		const USHORT usGang = 0x0010L;  //杠
		const USHORT usTing = 0x0020L;  //听
		const USHORT usHu   = 0x0040L;  //胡

		const USHORT usWaitting = 0x0001L;  //等待动作比较位，玩家请求动作前为0，\
									          请求动作后为1
	};

	namespace MjChiFlag						//温州二人麻将独有
	{
		const int usT		= 1;			//吃头
		const int usZ		= 2;			//吃中
		const int usW		= 3;			//吃尾
		const int usTZ		= 4;			//吃头 吃中
		const int usTW		= 5;			//吃头 吃尾
		const int usZW		= 6;			//吃中 吃尾
		const int usTZW		= 7;			//吃头 吃中 吃尾
	};

	//玩家的动作详细信息结构////////////////////////////////////////////////////////////////////
	struct TMjActInfo
	{
		USHORT usFlags;	//动作标识

		/*-有关吃牌-----------------------------------------------------*/
		BYTE   byChiSels[3];  //可以吃牌的选择
		int    iChiNums;	  //可以吃牌选择数量
		int	   fgChi;		  //吃的状态(吃头 吃中 吃尾  温州二人麻将独有)

		/*-有关杠牌-----------------------------------------------------*/
		BYTE   byGangSels[4]; //可以杠的选择
		int    iGangNums;	  //可以杠选择数量
		int    nBuGangInPeng; //补杠在建立在那一个碰牌上

		enum emGangType
		{
			gt_Unknown	= 0,	//未知类型
			gt_MingGang	= 1,	//明杠
			gt_BuGang	= 2,	//补杠
			gt_AnGang	= 3,	//暗杠
			gt_HuaGang	= 4,	//花杠(兴化麻将特有)

		} nGType[4]; //杠类型

		/*-有关听牌-----------------------------------------------------*/
		BYTE   byTingSels[MJ_MAX_HANDCARD]; //听，可打出牌听的选择
		int    iTingNums; //听，可打出牌听的选择数量

		/*-有关胡牌-----------------------------------------------------*/

		int  iHuTypeNums;	//胡牌类型数量

		enum emHuType
		{
			ht_Unknown	= 0,	//未知标记
			ht_Long		= 1,	//一条龙
			ht_HunYiSe	= 2,	//混一色
			ht_PengPeng = 3,	//对对碰
			ht_Dui7		= 4,	//七对
			ht_Dui7Bomb = 5,	//炸弹七对
			ht_QingYiSe = 6,	//清一色

		} nHuType[8]; //牌型表

		enum emHuSpecialType
		{
			hst_Unknown		= 0,  //未知标记
			hst_QiangGang	= 1,  //抢杠
			hst_GangKai		= 2,  //杠上开花
			hst_GangChong	= 3,  //杠冲

		} nHuSpecType; //特殊胡类型

		int  nHuGangIdx;	//抢杠控件的位置(用于抢杠)

		/*-其它-----------------------------------------------------*/

		int nIndexSel; //玩家作出的选择

		TAG_INIT(TMjActInfo)
		{
			::memset(this,0,sizeof(TMjActInfo));
		}

		//一些操作////////////////////////////////////////////////////////////
		public:
		
			//查找杠牌选择的索引
			int FindGangIndex(BYTE byCard) const
			{
				for (int i = 0; i < iGangNums; ++i)
				{
					if (byCard == byGangSels[i])
					{
						return i;
					}
				}
				return -1;
			}

			//查找杠牌选择的索引
			int FindTingIndex(BYTE byCard) const
			{
				for (int i = 0; i < iTingNums; ++i)
				{
					if (byCard == byTingSels[i])
					{
						return i;
					}
				}
				return -1;
			}

		////////////////////////////////////////////////////////////
	};

	//一个栏牌结点
	struct TMjCGPNode
	{
		enum emType
		{
			UNKNOWN		= 0,	//未知类型
			Chi			= 1,	//吃
			Peng		= 2,	//碰
			MingGang	= 3,	//明杠
			BuGang		= 4,	//补杠
			AnGang		= 5,	//暗杠

		} nType; //结点类型

		BYTE val[4]; //栏牌数值
		BYTE from;	//栏牌的来源
		int  nIdx;  //栏牌的位置

		TAG_INIT(TMjCGPNode)
		{
			::memset(this,0,sizeof(TMjCGPNode));
		}
	};

	//麻将游戏结果标记定义
	namespace MjResult
	{
		const BYTE Unknown	= 0;  //未知标记
		const BYTE Huang	= 1;  //荒庄
		const BYTE FangPao	= 2;  //放炮
		const BYTE JiePao	= 3;  //接炮
		const BYTE ZiMo		= 4;  //自摸
	}

	///糊牌检测临时数据
	struct CheckHuStruct
	{
		BYTE  data[MJ_MAX_HANDCARD][2];   //牌数据，0牌值，1张数
		BYTE  count;
		void Add(BYTE pai)
		{
			if(GetPaiCount(pai)>0)
			{
				for(int i=0;i<MJ_MAX_HANDCARD;i++)
				{
					if(data[i][0] == pai && pai != 255)
						data[i][1]++;
				}
			}
			else
			{
				data[count][0] = pai;
				data[count][1] = 1;
				count++;
			}
		};
		BYTE GetPaiCount(BYTE pai)
		{
			for(int i=0;i<MJ_MAX_HANDCARD;i++)
			{
				if(data[i][0] == pai && pai != 255)
					return data[i][1];
			}
			return 0;
		};
		BYTE GetDataCount()
		{
			count = 0;
			for(int i=0;i<MJ_MAX_HANDCARD;i++)
			{
				if(data[i][0] != 255)
					count++;
			}
			return count;
		};
		BYTE GetAllPaiCount()
		{
			BYTE num = 0;
			for(int i=0;i<MJ_MAX_HANDCARD;i++)
			{
				if(data[i][0] != 255)
					num += data[i][1];
			}
			return num;
		};
		void SetPaiCount(BYTE pai,BYTE num)
		{
			for(int i=0;i<MJ_MAX_HANDCARD;i++)
			{
				if(data[i][0] == pai && pai != 255)
				{
					data[i][1] = num;
				}
			}
		}
		void Init()
		{
			memset(data,255,sizeof(data));
			//memset(data[1],0,sizeof(data[1]));
			for(int i=0;i<MJ_MAX_HANDCARD;i++)
			{
				data[i][1] = 0;
			}
			count=0;
		};
		CheckHuStruct()
		{
			Init();
		}
	};
	///平糊组牌数据结构
	struct PingHuStruct
	{
		BYTE  byType[6];		//组牌类型
		BYTE  data[6][4];		//数据
		bool  bUsedJing[6][4];	//精牌充当在data中的位置，true为有精牌充当，false为没精牌充当
		BYTE  count;			//节点数
		PingHuStruct()
		{
			Init();
		};
		void AddData(BYTE type,BYTE pai[],bool bJing[4] = NULL)
		{
			for(int i=0;i<6;i++)
			{
				if(byType[i] == 255)
				{
					byType[i] = type;
					data[i][0] = pai[0];
					data[i][1] = pai[1];
					data[i][2] = pai[2];
					data[i][3] = pai[3];
					if (bJing)
					{
						bUsedJing[i][0] = bJing[0];
						bUsedJing[i][1] = bJing[1];
						bUsedJing[i][2] = bJing[2];
						bUsedJing[i][3] = bJing[3];
					}
					count++;
					break;
				}
			}
		};
		void Init()
		{
			memset(this,255,sizeof(PingHuStruct));
			memset(bUsedJing,false,sizeof(bUsedJing));
			count = 0;
		};
	};
}


#define MJCommon_DEF_HEAD_FILE
#endif