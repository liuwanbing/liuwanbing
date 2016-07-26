#pragma once

#define MAKE_A_MJ(hs,pd)  ( (hs) * 100 +(pd))

class CMjEnum
{
	
public:
			
	enum MJ_TYPE_HUA_SE
	{
			MJ_TYPE_HUA_SE_NONE=-10,
			MJ_TYPE_HUA_SE_BIN=3,
			MJ_TYPE_HUA_SE_WAN=4,
			MJ_TYPE_HUA_SE_TIAO=2,
			MJ_TYPE_HUA_SE_FENG=1

	};



	enum MJ_TYPE_PAI_DIAN
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

				MJ_TYPE_FCHUN=MAKE_A_MJ(MJ_TYPE_HUA_SE_FENG,11),
				MJ_TYPE_FXIA=MAKE_A_MJ(MJ_TYPE_HUA_SE_FENG,12),
				MJ_TYPE_FQIU=MAKE_A_MJ(MJ_TYPE_HUA_SE_FENG,13),
				MJ_TYPE_FDONG=MAKE_A_MJ(MJ_TYPE_HUA_SE_FENG,14),
				MJ_TYPE_FMEI=MAKE_A_MJ(MJ_TYPE_HUA_SE_FENG,15),
				MJ_TYPE_FLAN=MAKE_A_MJ(MJ_TYPE_HUA_SE_FENG,16),
				MJ_TYPE_FZHU=MAKE_A_MJ(MJ_TYPE_HUA_SE_FENG,17),
				MJ_TYPE_FJU=MAKE_A_MJ(MJ_TYPE_HUA_SE_FENG,18),

				MJ_TYPE_CAISHEN=MAKE_A_MJ(MJ_TYPE_HUA_SE_FENG,19),
				MJ_TYPE_YUANBAO=MAKE_A_MJ(MJ_TYPE_HUA_SE_FENG,20),
				MJ_TYPE_MAO=MAKE_A_MJ(MJ_TYPE_HUA_SE_FENG,21),
				MJ_TYPE_LAOXU=MAKE_A_MJ(MJ_TYPE_HUA_SE_FENG,22),



				MJ_TYPE_FD=MAKE_A_MJ(MJ_TYPE_HUA_SE_FENG,MJ_TYPE_PAI_DIAN_7),
				MJ_TYPE_FX=MAKE_A_MJ(MJ_TYPE_HUA_SE_FENG,MJ_TYPE_PAI_DIAN_6),
				MJ_TYPE_FN=MAKE_A_MJ(MJ_TYPE_HUA_SE_FENG,MJ_TYPE_PAI_DIAN_5),
				MJ_TYPE_FB=MAKE_A_MJ(MJ_TYPE_HUA_SE_FENG,MJ_TYPE_PAI_DIAN_4),
				MJ_TYPE_FZ=MAKE_A_MJ(MJ_TYPE_HUA_SE_FENG,MJ_TYPE_PAI_DIAN_3),
				MJ_TYPE_FFC=MAKE_A_MJ(MJ_TYPE_HUA_SE_FENG,MJ_TYPE_PAI_DIAN_2),
				MJ_TYPE_FGB=MAKE_A_MJ(MJ_TYPE_HUA_SE_FENG,MJ_TYPE_PAI_DIAN_1),


				MJ_TYPE_B1=MAKE_A_MJ(MJ_TYPE_HUA_SE_BIN,MJ_TYPE_PAI_DIAN_1),
				MJ_TYPE_B2=MAKE_A_MJ(MJ_TYPE_HUA_SE_BIN,MJ_TYPE_PAI_DIAN_2),
				MJ_TYPE_B3=MAKE_A_MJ(MJ_TYPE_HUA_SE_BIN,MJ_TYPE_PAI_DIAN_3),
				MJ_TYPE_B4=MAKE_A_MJ(MJ_TYPE_HUA_SE_BIN,MJ_TYPE_PAI_DIAN_4),
				MJ_TYPE_B5=MAKE_A_MJ(MJ_TYPE_HUA_SE_BIN,MJ_TYPE_PAI_DIAN_5),
				MJ_TYPE_B6=MAKE_A_MJ(MJ_TYPE_HUA_SE_BIN,MJ_TYPE_PAI_DIAN_6),
				MJ_TYPE_B7=MAKE_A_MJ(MJ_TYPE_HUA_SE_BIN,MJ_TYPE_PAI_DIAN_7),
				MJ_TYPE_B8=MAKE_A_MJ(MJ_TYPE_HUA_SE_BIN,MJ_TYPE_PAI_DIAN_8),
				MJ_TYPE_B9=MAKE_A_MJ(MJ_TYPE_HUA_SE_BIN,MJ_TYPE_PAI_DIAN_9),

				

				MJ_TYPE_W1=MAKE_A_MJ(MJ_TYPE_HUA_SE_WAN,MJ_TYPE_PAI_DIAN_1),
				MJ_TYPE_W2=MAKE_A_MJ(MJ_TYPE_HUA_SE_WAN,MJ_TYPE_PAI_DIAN_2),
				MJ_TYPE_W3=MAKE_A_MJ(MJ_TYPE_HUA_SE_WAN,MJ_TYPE_PAI_DIAN_3),
				MJ_TYPE_W4=MAKE_A_MJ(MJ_TYPE_HUA_SE_WAN,MJ_TYPE_PAI_DIAN_4),
				MJ_TYPE_W5=MAKE_A_MJ(MJ_TYPE_HUA_SE_WAN,MJ_TYPE_PAI_DIAN_5),
				MJ_TYPE_W6=MAKE_A_MJ(MJ_TYPE_HUA_SE_WAN,MJ_TYPE_PAI_DIAN_6),
				MJ_TYPE_W7=MAKE_A_MJ(MJ_TYPE_HUA_SE_WAN,MJ_TYPE_PAI_DIAN_7),
				MJ_TYPE_W8=MAKE_A_MJ(MJ_TYPE_HUA_SE_WAN,MJ_TYPE_PAI_DIAN_8),
				MJ_TYPE_W9=MAKE_A_MJ(MJ_TYPE_HUA_SE_WAN,MJ_TYPE_PAI_DIAN_9),

				MJ_TYPE_T1=MAKE_A_MJ(MJ_TYPE_HUA_SE_TIAO,MJ_TYPE_PAI_DIAN_1),
				MJ_TYPE_T2=MAKE_A_MJ(MJ_TYPE_HUA_SE_TIAO,MJ_TYPE_PAI_DIAN_2),
				MJ_TYPE_T3=MAKE_A_MJ(MJ_TYPE_HUA_SE_TIAO,MJ_TYPE_PAI_DIAN_3),
				MJ_TYPE_T4=MAKE_A_MJ(MJ_TYPE_HUA_SE_TIAO,MJ_TYPE_PAI_DIAN_4),
				MJ_TYPE_T5=MAKE_A_MJ(MJ_TYPE_HUA_SE_TIAO,MJ_TYPE_PAI_DIAN_5),
				MJ_TYPE_T6=MAKE_A_MJ(MJ_TYPE_HUA_SE_TIAO,MJ_TYPE_PAI_DIAN_6),
				MJ_TYPE_T7=MAKE_A_MJ(MJ_TYPE_HUA_SE_TIAO,MJ_TYPE_PAI_DIAN_7),
				MJ_TYPE_T8=MAKE_A_MJ(MJ_TYPE_HUA_SE_TIAO,MJ_TYPE_PAI_DIAN_8),
				MJ_TYPE_T9=MAKE_A_MJ(MJ_TYPE_HUA_SE_TIAO,MJ_TYPE_PAI_DIAN_9)

		
	};

};


#define MAX_MJ_PAI   (CMjEnum::MJ_TYPE_B9)




class CMjRef
{
	public:
		static int GetHuaSe(int iCard){return (iCard) / 100;}//GET_A_MJ_HUA_SE(iCard);}
		static int GetCardPaiDian(int iCard) { return (iCard) % 100 ;}//GET_A_MJ_PAI_DIAN(iCard) ; }
		static int MakeACard(int hs,int pd) { return  MAKE_A_MJ(hs,pd); }
		static bool IsValidateMj(int iCard) 
		{
			return iCard >= CMjEnum::MJ_TYPE_FCHUN && iCard <= CMjEnum::MJ_TYPE_LAOXU ||  
				iCard >= CMjEnum::MJ_TYPE_FGB && iCard <= CMjEnum::MJ_TYPE_FD ||
				iCard >= CMjEnum::MJ_TYPE_B1 && iCard <= CMjEnum::MJ_TYPE_B9 ||
				iCard >= CMjEnum::MJ_TYPE_W1 && iCard <= CMjEnum::MJ_TYPE_W9 ||
				iCard >= CMjEnum::MJ_TYPE_T1 && iCard <= CMjEnum::MJ_TYPE_T9 
				;
		}
		


   public:

	   int m_iPs;
	   bool IsValidateMj(){return IsValidateMj(m_iPs);}

	   int GetHuaSe(){return GetHuaSe(m_iPs);}
	   ///int GetCardNum() { return GetCardNum(m_iCard); }
	   int GetCardPaiDian() { return GetCardPaiDian(m_iPs); }
	   CMjRef(int card = 0){m_iPs =card;}
	   ///CMjRef(int hs = 0,int pd = 0){m_iPs =MakeACard(hs,pd);}
	   virtual ~CMjRef(){m_iPs =0;}
	   
};


#define MJ_H   36
#define MJ_W   24



#define A_FU_MJ_NUM  34
#define A_FU_MJ_NUM_OTHER  8
static int stcArr_A_Mj[]=
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
			CMjEnum::MJ_TYPE_T9,

			CMjEnum::MJ_TYPE_FD,
			CMjEnum::MJ_TYPE_FN  ,
			CMjEnum::MJ_TYPE_FX ,
			CMjEnum::MJ_TYPE_FB ,
			CMjEnum::MJ_TYPE_FGB,
			CMjEnum::MJ_TYPE_FFC ,
			CMjEnum::MJ_TYPE_FZ		
};
//Ò»¸±Âé½«£º ÎÞ·ç ºÍ ´ºÏÄÇï¶¬£¬Ã·À¼Öñ¾Õ
static int stcArr_A_Mj_NoFeng[]=
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
			CMjEnum::MJ_TYPE_T9,

			//CMjEnum::MJ_TYPE_FD,
			//CMjEnum::MJ_TYPE_FN  ,
			//CMjEnum::MJ_TYPE_FX ,
			//CMjEnum::MJ_TYPE_FB ,
			CMjEnum::MJ_TYPE_FGB,
			CMjEnum::MJ_TYPE_FFC ,
			CMjEnum::MJ_TYPE_FZ		
};
////¼ÓÉÏ´ºÏÄÇï¶¬Ã·À¼Öñ¾Õ
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


static int GetGangHua(int caishen)
{
	int ii=-1;
	for(int i=0;i<A_FU_MJ_NUM;i++)
	{
		if(caishen == stcArr_A_Mj[i])
		{
			ii=i;
			break;
		}
	}
	if(ii < 0)return -1;

	int is=ii+1;
	if(is == A_FU_MJ_NUM)is=is-7;
	else 
	{
		if( is % 9 == 0)is=is-9;
	}
	return stcArr_A_Mj[is];

	
}


static int stc_GangHuaArr[]={
	CMjEnum::MJ_TYPE_FCHUN,
			      CMjEnum::  MJ_TYPE_FXIA,
				CMjEnum::	MJ_TYPE_FQIU,
				CMjEnum::	MJ_TYPE_FDONG,
				CMjEnum::	MJ_TYPE_FMEI,
				CMjEnum::	MJ_TYPE_FLAN,
				CMjEnum::	MJ_TYPE_FZHU,
				CMjEnum::	MJ_TYPE_FJU,
				CMjEnum::	MJ_TYPE_CAISHEN,
				CMjEnum::	MJ_TYPE_YUANBAO,
				CMjEnum::	MJ_TYPE_MAO,
				CMjEnum::	MJ_TYPE_LAOXU
	                };


typedef  int ARRUSERMJ[20];





static int GetCaiShenPai(int csshow)
{
	if(csshow == CMjEnum::MJ_TYPE_FCHUN ||
		csshow == CMjEnum::MJ_TYPE_FMEI || 
		csshow == CMjEnum::MJ_TYPE_CAISHEN
		)
		return CMjEnum::MJ_TYPE_FD;

	if(csshow == CMjEnum::MJ_TYPE_FXIA ||
		csshow == CMjEnum::MJ_TYPE_FLAN || 
		csshow == CMjEnum::MJ_TYPE_YUANBAO
		)
		return CMjEnum::MJ_TYPE_FN;

	if(csshow == CMjEnum::MJ_TYPE_FQIU ||
		csshow == CMjEnum::MJ_TYPE_FZHU || 
		csshow == CMjEnum::MJ_TYPE_MAO
		)
		return  CMjEnum::MJ_TYPE_FX;

	if(csshow == CMjEnum::MJ_TYPE_FDONG ||
		csshow == CMjEnum::MJ_TYPE_FJU || 
		csshow == CMjEnum::MJ_TYPE_LAOXU
		)
		return  CMjEnum::MJ_TYPE_FB;

	return csshow ;
}


