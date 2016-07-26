#pragma once

#define MAKE_A_MJ(hs,pd)  ( (hs) * 10 +(pd))
///lym2010-05-07重新定义麻将值
///万：1-9
///条：11-19
///同：21-29
///字：31-37
///花牌：41-48 或者更多

class CMjEnum
{
	
public:
			
	enum MJ_TYPE_HUA_SE
	{
		MJ_TYPE_HUA_SE_NONE=-10,//lu无花色
		MJ_TYPE_HUA_SE_BIN=3,//lu同
		MJ_TYPE_HUA_SE_WAN=4,//lu万
		MJ_TYPE_HUA_SE_TIAO=2,//lu条
		MJ_TYPE_HUA_SE_FENG=1//lu风
	};

	enum MJ_TYPE_PAI//牌类型
	{
		MJ_TYPE_PAI_NONE=-10,//lu无花色
		MJ_TYPE_PAI_WAN=0,//lu万
		MJ_TYPE_PAI_TIAO=1,//lu条
		MJ_TYPE_PAI_BING=2,//lu同
		MJ_TYPE_PAI_FENG=3,//lu风
		MJ_TYPE_PAI_HUA=4//lu花
	};

	enum MJ_TYPE_PAI_DIAN//lu牌点
	{
		MJ_TYPE_PAI_DIAN_NONE=-1,

		MJ_TYPE_PAI_DIAN_1 = 1,
		MJ_TYPE_PAI_DIAN_2 = 2,
		MJ_TYPE_PAI_DIAN_3 = 3,
		MJ_TYPE_PAI_DIAN_4 = 4,
		MJ_TYPE_PAI_DIAN_5 = 5,
		MJ_TYPE_PAI_DIAN_6 = 6,
		MJ_TYPE_PAI_DIAN_7 = 7,
		MJ_TYPE_PAI_DIAN_8 = 8,
		MJ_TYPE_PAI_DIAN_9 = 9
	};

	enum MJ_TYPE
	{
		MJ_TYPE_NONE=0,

		MJ_TYPE_FCHUN=MAKE_A_MJ(MJ_TYPE_PAI_FENG,11),
		MJ_TYPE_FXIA=MAKE_A_MJ(MJ_TYPE_PAI_FENG,12),
		MJ_TYPE_FQIU=MAKE_A_MJ(MJ_TYPE_PAI_FENG,13),
		MJ_TYPE_FDONG=MAKE_A_MJ(MJ_TYPE_PAI_FENG,14),
		MJ_TYPE_FMEI=MAKE_A_MJ(MJ_TYPE_PAI_FENG,15),
		MJ_TYPE_FLAN=MAKE_A_MJ(MJ_TYPE_PAI_FENG,16),
		MJ_TYPE_FZHU=MAKE_A_MJ(MJ_TYPE_PAI_FENG,17),
		MJ_TYPE_FJU=MAKE_A_MJ(MJ_TYPE_PAI_FENG,18),
		

		MJ_TYPE_CAISHEN=MAKE_A_MJ(MJ_TYPE_PAI_FENG,19),
		MJ_TYPE_YUANBAO=MAKE_A_MJ(MJ_TYPE_PAI_FENG,20),
		MJ_TYPE_MAO=MAKE_A_MJ(MJ_TYPE_PAI_FENG,21),
		MJ_TYPE_LAOXU=MAKE_A_MJ(MJ_TYPE_PAI_FENG,22),


		MJ_TYPE_FD=MAKE_A_MJ(MJ_TYPE_PAI_FENG,MJ_TYPE_PAI_DIAN_1),///东
		MJ_TYPE_FN=MAKE_A_MJ(MJ_TYPE_PAI_FENG,MJ_TYPE_PAI_DIAN_2),///南
		MJ_TYPE_FX=MAKE_A_MJ(MJ_TYPE_PAI_FENG,MJ_TYPE_PAI_DIAN_3),///西
		MJ_TYPE_FB=MAKE_A_MJ(MJ_TYPE_PAI_FENG,MJ_TYPE_PAI_DIAN_4),///北
		MJ_TYPE_ZHONG=MAKE_A_MJ(MJ_TYPE_PAI_FENG,MJ_TYPE_PAI_DIAN_5),///红中
		MJ_TYPE_FA=MAKE_A_MJ(MJ_TYPE_PAI_FENG,MJ_TYPE_PAI_DIAN_6),///发财
		MJ_TYPE_BAI=MAKE_A_MJ(MJ_TYPE_PAI_FENG,MJ_TYPE_PAI_DIAN_7),///白板


		MJ_TYPE_W1=MAKE_A_MJ(MJ_TYPE_PAI_WAN,MJ_TYPE_PAI_DIAN_1),///万
		MJ_TYPE_W2=MAKE_A_MJ(MJ_TYPE_PAI_WAN,MJ_TYPE_PAI_DIAN_2),
		MJ_TYPE_W3=MAKE_A_MJ(MJ_TYPE_PAI_WAN,MJ_TYPE_PAI_DIAN_3),
		MJ_TYPE_W4=MAKE_A_MJ(MJ_TYPE_PAI_WAN,MJ_TYPE_PAI_DIAN_4),
		MJ_TYPE_W5=MAKE_A_MJ(MJ_TYPE_PAI_WAN,MJ_TYPE_PAI_DIAN_5),
		MJ_TYPE_W6=MAKE_A_MJ(MJ_TYPE_PAI_WAN,MJ_TYPE_PAI_DIAN_6),
		MJ_TYPE_W7=MAKE_A_MJ(MJ_TYPE_PAI_WAN,MJ_TYPE_PAI_DIAN_7),
		MJ_TYPE_W8=MAKE_A_MJ(MJ_TYPE_PAI_WAN,MJ_TYPE_PAI_DIAN_8),
		MJ_TYPE_W9=MAKE_A_MJ(MJ_TYPE_PAI_WAN,MJ_TYPE_PAI_DIAN_9),

		MJ_TYPE_T1=MAKE_A_MJ(MJ_TYPE_PAI_TIAO,MJ_TYPE_PAI_DIAN_1),///条
		MJ_TYPE_T2=MAKE_A_MJ(MJ_TYPE_PAI_TIAO,MJ_TYPE_PAI_DIAN_2),
		MJ_TYPE_T3=MAKE_A_MJ(MJ_TYPE_PAI_TIAO,MJ_TYPE_PAI_DIAN_3),
		MJ_TYPE_T4=MAKE_A_MJ(MJ_TYPE_PAI_TIAO,MJ_TYPE_PAI_DIAN_4),
		MJ_TYPE_T5=MAKE_A_MJ(MJ_TYPE_PAI_TIAO,MJ_TYPE_PAI_DIAN_5),
		MJ_TYPE_T6=MAKE_A_MJ(MJ_TYPE_PAI_TIAO,MJ_TYPE_PAI_DIAN_6),
		MJ_TYPE_T7=MAKE_A_MJ(MJ_TYPE_PAI_TIAO,MJ_TYPE_PAI_DIAN_7),
		MJ_TYPE_T8=MAKE_A_MJ(MJ_TYPE_PAI_TIAO,MJ_TYPE_PAI_DIAN_8),
		MJ_TYPE_T9=MAKE_A_MJ(MJ_TYPE_PAI_TIAO,MJ_TYPE_PAI_DIAN_9),

		MJ_TYPE_B1=MAKE_A_MJ(MJ_TYPE_PAI_BING,MJ_TYPE_PAI_DIAN_1),///同
		MJ_TYPE_B2=MAKE_A_MJ(MJ_TYPE_PAI_BING,MJ_TYPE_PAI_DIAN_2),
		MJ_TYPE_B3=MAKE_A_MJ(MJ_TYPE_PAI_BING,MJ_TYPE_PAI_DIAN_3),
		MJ_TYPE_B4=MAKE_A_MJ(MJ_TYPE_PAI_BING,MJ_TYPE_PAI_DIAN_4),
		MJ_TYPE_B5=MAKE_A_MJ(MJ_TYPE_PAI_BING,MJ_TYPE_PAI_DIAN_5),
		MJ_TYPE_B6=MAKE_A_MJ(MJ_TYPE_PAI_BING,MJ_TYPE_PAI_DIAN_6),
		MJ_TYPE_B7=MAKE_A_MJ(MJ_TYPE_PAI_BING,MJ_TYPE_PAI_DIAN_7),
		MJ_TYPE_B8=MAKE_A_MJ(MJ_TYPE_PAI_BING,MJ_TYPE_PAI_DIAN_8),
		MJ_TYPE_B9=MAKE_A_MJ(MJ_TYPE_PAI_BING,MJ_TYPE_PAI_DIAN_9)
	
	};

};

#define MAX_MJ_PAI   (CMjEnum::MJ_TYPE_B9)


class CMjRef
{
	public:
		//
		static int MakeACard(int hs,int pd) { return  MAKE_A_MJ(hs,pd); }
   public:

	   int m_iPs;
	   static bool IsValidateMj(BYTE iCard)
	   {
			return iCard >= CMjEnum::MJ_TYPE_FCHUN && iCard <= CMjEnum::MJ_TYPE_LAOXU ||  
				iCard >= CMjEnum::MJ_TYPE_FD && iCard <= CMjEnum::MJ_TYPE_BAI ||
				iCard >= CMjEnum::MJ_TYPE_B1 && iCard <= CMjEnum::MJ_TYPE_B9  ||
				iCard >= CMjEnum::MJ_TYPE_W1 && iCard <= CMjEnum::MJ_TYPE_W9  ||
				iCard >= CMjEnum::MJ_TYPE_T1 && iCard <= CMjEnum::MJ_TYPE_T9 
				;
	   }
	   //获取牌的花色
	   static BYTE GetHuaSe(BYTE byCard)
	   {
		   return (byCard) / 10;
	   }
	   //获取牌的牌点
	   static BYTE GetCardPaiDian(BYTE byCard) 
	   { 
		   return (byCard) % 10 ;
	   }

	   //检测2张牌是否同一花色
	   static bool CheckIsTongSe(BYTE Card1,BYTE Card2)
	   { 
		   return (Card1/10==Card2/10);
	   }
	   //检测2张牌是否一相连
	   static  bool CheckIsYiXiangLian(BYTE Card1,BYTE Card2,bool big=false)
	   { 
			if(!CheckIsTongSe(Card1,Card2))
				return false;
			if(!big)
				return (Card1==Card2+1);
			return (Card1==Card2-1);
	   }
	   //检测2张牌是否二相连
	   static  bool CheckIsErXiangLian(BYTE Card1,BYTE Card2,bool big=false)
	   { 
			if(!CheckIsTongSe(Card1,Card2))
				return false;
			if(!big)
				return (Card1==Card2+1 || Card1==Card2+2 );
			return (Card1==Card2-1 || Card1==Card2-2 );
	   }

		static BYTE LogicToCtr(BYTE byMJ)//逻辑数据转换成控件数据
		{
			return 255;
		}

		static BYTE CtrToLogic(BYTE byMJ)//控件数据转换成逻辑数据
		{
			return 255;
		}

		static bool LogicToCtrEx(BYTE* byT,const BYTE* byS,int nMJSize)//逻辑麻将转换成控件麻将数据（批量转换）
		{
			if (!byT || !byS)
				return false;

			for (int i = 0; i < nMJSize; i++)
			{
				byT[i] = CMjRef::CtrToLogic(byS[i]);
			}
			return true;
		}

		///牌型检测

		///描述：添加一种糊牌类型
		///@param   type 要添加的类型 , hupai[] 糊牌类型数组
		static void SetAHupaiType(BYTE type,BYTE hupai[],BYTE maxtype)
		{
			if(CheckHupaiType(type,hupai,maxtype))//有了就不添加了
				return;
			for(int i=0;i<maxtype;i++)
			{
				if(hupai[i] == 255)
				{
					hupai[i] = type;
					break;
				}
			}
		}

		///描述：撤销一种糊牌类型
		///@param type 要删除的类型 , hupai[] 糊牌类型数组
		static void ReSetAHupaiType(BYTE type,BYTE hupai[],BYTE maxtype)
		{
			int index = 0;
			for(int i=0;i<maxtype;i++)
			{
				if(hupai[i] == type)
				{
					hupai[i] = 255;
					break;
				}
			}
			//去除中间无效元素
			BYTE temp[255];
			memset(temp,255,sizeof(temp));
			for(int i=0;i<maxtype;i++)
			{
				if(hupai[i] != 255)
				{
					temp[index] = hupai[i];
					index++;
				}
			}
			for(int i=0;i<maxtype;i++)
			{
				hupai[i] = temp[i];
			}
		}

		///描述：清除所有糊牌类型
		///@param type 要删除的类型 , hupai[] 糊牌类型数组
		static void ClearHupaiType(BYTE hupai[],BYTE maxtype)
		{
			for(int i=0;i<maxtype;i++)
			{
				hupai[i] = 255;
			}
		}

		///描述：检测是否存在某种糊牌类型
		///@param type 要检测的类型 , hupai[] 糊牌类型数组
		static bool CheckHupaiType(BYTE type,BYTE hupai[],BYTE maxtype)
		{
			int index = 0;
			for(int i=0;i<maxtype;i++)
			{
				if(hupai[i] == type && type!=255)
				{
					return true;
				}
			}
			return false;
		}

		static BYTE ExDToB(BYTE byMJ)//逻辑数据转换成控件数据
		{
			if (byMJ == 0)
			{
				return 34;
			}
			else if(byMJ <= 9)	//“万”转换
			{
				return (byMJ - 1);
			}
			else if(byMJ <= 19)	//“同”转换
			{
				return (byMJ + 7);
			}
			else if(byMJ <= 29)	//“条”转换
			{
				return (byMJ - 12);
			}
			else if(byMJ <= 37)	//风牌、“中发白”转换
			{
				if(byMJ == 31)//东
				{
					return 27;
				}
				else if(byMJ == 32)//南
				{
					return 29;
				}
				else if(byMJ == 33)//西
				{
					return 28;
				}
				else if(byMJ == 34)//北
				{
					return 30;
				}
				else 
				{
					return byMJ - 4;
				}
			}
			else if(byMJ <= 48)	//“春夏秋冬梅兰竹菊”转换
			{
				return (byMJ - 5);
			}
			return 255;
		}

		static BYTE ExBToD(BYTE byMJ)//控件数据转换成逻辑数据
		{
			if (byMJ < 0)
			{
				return byMJ;
			}
			else if(byMJ <= 8)	//“万”转换
			{
				return (byMJ + 1);
			}
			else if(byMJ <= 17)	//“条”转换
			{
				return (byMJ + 12);
			}
			else if(byMJ <= 26)	//“同”转换
			{
				return (byMJ - 7);
			}
			else if(byMJ <= 33)	//风牌、“中发白”转换
			{
				if(byMJ == 27)//东
				{
					return 31;					
				}
				else if(byMJ == 29)//南
				{
					return 32;					
				}
				else if(byMJ == 28)//西
				{
					return 33;					
				}
				else if(byMJ == 30)//北
				{
					return 34;
				}
				else 
				{
					return (byMJ + 4);
				}
			}
			else if(byMJ == 34) //空牌与背牌
			{
				return 0;
			}
			else if(byMJ <= 43)	//“春夏秋冬梅兰竹菊”转换
			{
				return (byMJ + 5);
			}

			return 255;
		}

		static bool ExDToB(BYTE* byT,const BYTE* byS,int nMJSize)//逻辑麻将转换成控件麻将数据（批量转换）
		{
			if (!byT || !byS)
				return false;

			for (int i = 0; i < nMJSize; i++)
			{
				byT[i] = CMjRef::ExDToB(byS[i]);
			}

			return true;
		}
	   CMjRef(int card = 0){m_iPs =card;}
	   virtual ~CMjRef(){m_iPs =0;}
	   
};


#define MJ_H   36
#define MJ_W   24


#define	A_FU_MJ_JINZHOU_NUM	28			//锦州麻将的一副麻将(27+红中)
#define A_FU_MJ_NUM  34
#define A_FU_MJ_OTHER  8

static int stcArr_A_Direction[] = 
{
	CMjEnum::MJ_TYPE_FD,
	CMjEnum::MJ_TYPE_FN,
	CMjEnum::MJ_TYPE_FX,
	CMjEnum::MJ_TYPE_FB
};

static int stcArr_A_Mj[]=
{
	CMjEnum::MJ_TYPE_W1,
	CMjEnum::MJ_TYPE_W2,
	CMjEnum::MJ_TYPE_W3,
	CMjEnum::MJ_TYPE_W4,
	CMjEnum::MJ_TYPE_W5,
	CMjEnum::MJ_TYPE_W6,
	CMjEnum::MJ_TYPE_W7,
	CMjEnum::MJ_TYPE_W8,
	CMjEnum::MJ_TYPE_W9,
	
	CMjEnum::MJ_TYPE_B1 ,
	CMjEnum::MJ_TYPE_B2 ,
	CMjEnum::MJ_TYPE_B3 ,
	CMjEnum::MJ_TYPE_B4,
	CMjEnum::MJ_TYPE_B5,
	CMjEnum::MJ_TYPE_B6,
	CMjEnum::MJ_TYPE_B7,
	CMjEnum::MJ_TYPE_B8,
	CMjEnum::MJ_TYPE_B9,


	CMjEnum::MJ_TYPE_T1,
	CMjEnum::MJ_TYPE_T2,
	CMjEnum::MJ_TYPE_T3,
	CMjEnum::MJ_TYPE_T4,
	CMjEnum::MJ_TYPE_T5,
	CMjEnum::MJ_TYPE_T6,
	CMjEnum::MJ_TYPE_T7,
	CMjEnum::MJ_TYPE_T8,
	CMjEnum::MJ_TYPE_T9,

	CMjEnum::MJ_TYPE_FD,
	CMjEnum::MJ_TYPE_FN  ,
	CMjEnum::MJ_TYPE_FX ,
	CMjEnum::MJ_TYPE_FB ,
	CMjEnum::MJ_TYPE_ZHONG,	
	CMjEnum::MJ_TYPE_FA ,
	CMjEnum::MJ_TYPE_BAI
	
};

//麻将的万同条
static	int	stcArr_A_Mj_WTT[]=
{
	CMjEnum:: MJ_TYPE_W1,
	CMjEnum::MJ_TYPE_W2,
	CMjEnum::MJ_TYPE_W3,
	CMjEnum::MJ_TYPE_W4,
	CMjEnum::MJ_TYPE_W5,
	CMjEnum::MJ_TYPE_W6,
	CMjEnum::MJ_TYPE_W7,
	CMjEnum::MJ_TYPE_W8,
	CMjEnum::MJ_TYPE_W9,
	
	CMjEnum::MJ_TYPE_B1 ,
	CMjEnum::MJ_TYPE_B2 ,
	CMjEnum::MJ_TYPE_B3 ,
	CMjEnum::MJ_TYPE_B4,
	CMjEnum::MJ_TYPE_B5,
	CMjEnum::MJ_TYPE_B6,
	CMjEnum::MJ_TYPE_B7,
	CMjEnum::MJ_TYPE_B8,
	CMjEnum::MJ_TYPE_B9,

	CMjEnum::MJ_TYPE_T1,
	CMjEnum::MJ_TYPE_T2,
	CMjEnum::MJ_TYPE_T3,
	CMjEnum::MJ_TYPE_T4,
	CMjEnum::MJ_TYPE_T5,
	CMjEnum::MJ_TYPE_T6,
	CMjEnum::MJ_TYPE_T7,
	CMjEnum::MJ_TYPE_T8,
	CMjEnum::MJ_TYPE_T9

};
//麻将的风牌
static	int	stcArr_A_Mj_FENG[]=
{
	CMjEnum::MJ_TYPE_FD,
	CMjEnum::MJ_TYPE_FN,
	CMjEnum::MJ_TYPE_FX,
	CMjEnum::MJ_TYPE_FB 

};
//麻将的字牌
static	int	stcArr_A_Mj_ZI[]=
{
	CMjEnum::MJ_TYPE_ZHONG,		//红中
	CMjEnum::MJ_TYPE_FA,//发财
	CMjEnum::MJ_TYPE_BAI//白板

};
////加上春夏秋冬梅兰竹菊
static int stcArr_A_Mj_Other[]=
{
	CMjEnum::MJ_TYPE_FCHUN,
	CMjEnum::MJ_TYPE_FXIA,
	CMjEnum::MJ_TYPE_FQIU,
	CMjEnum::MJ_TYPE_FDONG,

	CMjEnum::MJ_TYPE_FMEI,
	CMjEnum::MJ_TYPE_FLAN,
	CMjEnum::MJ_TYPE_FZHU,
	CMjEnum::MJ_TYPE_FJU
};









