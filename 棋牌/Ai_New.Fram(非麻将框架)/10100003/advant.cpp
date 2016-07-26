

#include "stdafx.h"
#include "advant.h"
#include "UpGradeLogic.h"
CUpGradeGameLogic logic;
/// 添加的便利函数
/// 删除牌中的某张牌
/// 其中m_Card，num是手中牌，index是要删除的位置
void del(BYTE *m_Card,int &num,int index)
{
	if(index>=0&&index<num)
	{
		for(int i=index+1;i<num;i++)
		{
			m_Card[i-1] = m_Card[i];
		}
		num--;
	}
}

/// 添加的便利函数
/// 删除牌中的某张牌
/// 其中m_Card，num是手中牌，card是要删除的牌
void del(BYTE *m_Card,int &num,BYTE card)
{
	int index = -1;
	for(int i=0;i<num;i++)
	{
		if(m_Card[i] == card)
		{
			index =i;
			break;
		}
	}
    del(m_Card,num,index);
}

/// 添加便利函数
/// 删除牌中的某些牌
/// m_Card,num为自己的牌，m_delCard,delNum为要删除的牌
void del(BYTE *m_Card,int &num,BYTE *m_delCard,int delNum)
{
	for(int i=0;i<delNum;i++)
	{
		del(m_Card,num,m_delCard[i]);
	}
}

/// 添加便利函数
/// 删除牌中的某些牌
/// m_Card,num为自己的牌，bulk为删除的牌等于
void delBulk(BYTE *m_Card,int &num,int bulk)
{
	for(int i=0;i<num;i++)
	{
		if(logic.GetCardBulk(m_Card[i])==bulk)
		{
			del(m_Card,num,i);
			i--;
		}
	}
}


/// 提取2和大小王的操作
/// 其中m_Card，num是自己手上的牌，myCard为自己配好的牌
void tiDX2Card(BYTE *m_Card,int &num,std::vector<peiOutCardStruct>& myCard)
{
	peiOutCardStruct out2;
	out2.out.iCardCount = 0;
	peiOutCardStruct outDX;
	outDX.out.iCardCount = 0;		
	for(int i=0;i<num;i++)
	{
		BYTE temp = logic.GetCardNum(m_Card[i]);
		if(temp == 2)   //大小王
		{
			out2.out.iCardList[out2.out.iCardCount++] = m_Card[i];
			out2.type = 2;
			del(m_Card,num,i);

			// 因为删除了一个元素，所以需要改变便利的位置
			i--;
		}
		else if(m_Card[i]==0x4E||m_Card[i]==0x4F)   //大小王
		{
			outDX.out.iCardList[outDX.out.iCardCount++] = m_Card[i];
			outDX.type = 1;
			del(m_Card,num,i);

			// 因为删除了一个元素，所以需要改变便利的位置
			i--;
		}
	}
	if(outDX.out.iCardCount>0)
	{
		myCard.push_back(outDX);
	}
	if(out2.out.iCardCount>0)
	{
		myCard.push_back(out2);
	}

}

/// 配牌的操作，主要为配当前手中的牌，其中牌中不包括2和大小王
/// 其中m_Card，num是自己手上的牌，myCard为自己配好的牌
/// @return 当满足要求不需要在计算的时候返回true否则返回false
bool peiCard(BYTE *m_Card,int num,std::vector<peiOutCardStruct>& myCard,bool (*para)(BYTE*,int,std::vector<peiOutCardStruct>&,CClientGameDlg *pClientGameDlg),CClientGameDlg *pClientGameDlg)
{
	tiDX2Card(m_Card,num,myCard);
    bool haveSun = false;          /// 用于记录本地
	///首先判断自己有没有炸弹,注意因为王和2都已经去掉了，所以不必计算有没有王和2的炸弹
    if(zhaPeiCard(m_Card,num,myCard,para,haveSun,pClientGameDlg))
		return true;
	else if(shun3PeiCard(m_Card,num,myCard,para,haveSun,pClientGameDlg))
		return true;
	else if(shun2PeiCard(m_Card,num,myCard,para,haveSun,pClientGameDlg))
		return true;
	else if(shun1PeiCard(m_Card,num,myCard,para,haveSun,pClientGameDlg))
	    return true;
	else if(sanPeiCard(m_Card,num,myCard,para,haveSun,pClientGameDlg))
	    return true;
	else if(duiPeiCard(m_Card,num,myCard,para,haveSun,pClientGameDlg))
	    return true;
	
	if(!haveSun)  //如果是叶子节点的处理
	{
		return para(m_Card,num,myCard,pClientGameDlg);
	}
    return false;

}

/// 生成含有炸弹的孩子
bool zhaPeiCard(BYTE *m_Card,int num,std::vector<peiOutCardStruct>& myCard,bool (*para)(BYTE*,int,std::vector<peiOutCardStruct>&,CClientGameDlg *pClientGameDlg),bool &haveSun,CClientGameDlg *pClientGameDlg)
{
	BYTE iResultCard[CARD_COUNT_PER_PERSON];
    int iResultCardCount = 0;
	if(logic.TackOutBomb(m_Card,num,iResultCard,iResultCardCount))
	{
		
		BYTE tempCard[CARD_COUNT_PER_PERSON];
        int tempNum;
		
        for(int i=0;(i*4)<iResultCardCount;i++)
		{
            haveSun = true;       //因为此时一定有孩子
			memcpy(tempCard,m_Card,sizeof(BYTE)*num);
	        tempNum = num;

			peiOutCardStruct out;
			out.out.iCardCount = 4;                                         //王2去掉之后只能是4个牌组成的炸弹
			memcpy(out.out.iCardList,&iResultCard[i*4],sizeof(BYTE)*4); 
			out.type = 3;   //四个的炸弹
            if(!canPush(myCard,out))
			{
				continue;
			}
			del(tempCard,tempNum,out.out.iCardList,4);
			myCard.push_back(out);

			if(peiCard(tempCard,tempNum,myCard,para,pClientGameDlg))
			{
				return true;                     //找到满足之后停止运算并将结果返回
			}
			myCard.pop_back();
		}
		
	}
	return false;
}

/// 生成含有三顺的孩子
bool shun3PeiCard(BYTE *m_Card,int num,std::vector<peiOutCardStruct>& myCard,bool (*para)(BYTE*,int,std::vector<peiOutCardStruct>&,CClientGameDlg *pClientGameDlg),bool &haveSun,CClientGameDlg *pClientGameDlg)
{
	BYTE iResultCard[CARD_COUNT_PER_PERSON];
    int iResultCardCount = 0;
    BYTE iBaseCard[CARD_COUNT_PER_PERSON];
	int iBaseCount = 6;

	while(logic.TackOutSequence(m_Card,num,iBaseCard,iBaseCount,iResultCard,iResultCardCount,3,true))
	{
		///第一次循环确定顺子的个数
		do{      
			///第二次循环确定顺子在这种个数情况下的大小
			haveSun = true;       //因为此时一定有孩子
			BYTE tempCard[CARD_COUNT_PER_PERSON];
			int tempNum;
			memcpy(tempCard,m_Card,sizeof(BYTE)*num);
			tempNum = num;

			peiOutCardStruct out;
			out.out.iCardCount = iBaseCount;                                         //王2去掉之后只能是4个牌组成的炸弹
			memcpy(out.out.iCardList,iResultCard,sizeof(BYTE)*iBaseCount); 
			out.type = 4;
            if(canPush(myCard,out))
			{
				del(tempCard,tempNum,out.out.iCardList,iBaseCount);
				myCard.push_back(out);
				if(peiCard(tempCard,tempNum,myCard,para,pClientGameDlg))
				{
					return true;                     //找到满足之后停止运算并将结果返回
				}
				myCard.pop_back();				
			}

			memcpy(iBaseCard,iResultCard,sizeof(BYTE)*iBaseCount);
		}while(logic.TackOutSequence(m_Card,num,iBaseCard,iBaseCount,iResultCard,iResultCardCount,3));

		iBaseCount+=3;
	}

	return false;
}
/// 生成含有双顺的孩子
bool shun2PeiCard(BYTE *m_Card,int num,std::vector<peiOutCardStruct>& myCard,bool (*para)(BYTE*,int,std::vector<peiOutCardStruct>&,CClientGameDlg *pClientGameDlg),bool &haveSun,CClientGameDlg *pClientGameDlg)
{
	BYTE iResultCard[CARD_COUNT_PER_PERSON];
    int iResultCardCount = 0;
    BYTE iBaseCard[CARD_COUNT_PER_PERSON];
	int iBaseCount = 6;

	while(logic.TackOutSequence(m_Card,num,iBaseCard,iBaseCount,iResultCard,iResultCardCount,2,true))
	{
		///第一次循环确定顺子的个数
		do{      
			///第二次循环确定顺子在这种个数情况下的大小
			haveSun = true;       //因为此时一定有孩子
			BYTE tempCard[CARD_COUNT_PER_PERSON];
			int tempNum;
			memcpy(tempCard,m_Card,sizeof(BYTE)*num);
			tempNum = num;

			peiOutCardStruct out;
			out.out.iCardCount = iBaseCount;                                         
			memcpy(out.out.iCardList,iResultCard,sizeof(BYTE)*iBaseCount); 
			out.type = 5;
            if(canPush(myCard,out))
			{
				del(tempCard,tempNum,out.out.iCardList,iBaseCount);
				myCard.push_back(out);
				if(peiCard(tempCard,tempNum,myCard,para,pClientGameDlg))
				{
					return true;                     //找到满足之后停止运算并将结果返回
				}
				myCard.pop_back();				
			}

			memcpy(iBaseCard,iResultCard,sizeof(BYTE)*iBaseCount);
		}while(logic.TackOutSequence(m_Card,num,iBaseCard,iBaseCount,iResultCard,iResultCardCount,2));

		iBaseCount+=2;
	}
	return false;
}

/// 生成含有单顺的孩子
bool shun1PeiCard(BYTE *m_Card,int num,std::vector<peiOutCardStruct>& myCard,bool (*para)(BYTE*,int,std::vector<peiOutCardStruct>&,CClientGameDlg *pClientGameDlg),bool &haveSun,CClientGameDlg *pClientGameDlg)
{
	BYTE iResultCard[CARD_COUNT_PER_PERSON];
    int iResultCardCount = 0;
    BYTE iBaseCard[CARD_COUNT_PER_PERSON];
	int iBaseCount = 5;

	while(logic.TackOutSequence(m_Card,num,iBaseCard,iBaseCount,iResultCard,iResultCardCount,1,true))
	{
		///第一次循环确定顺子的个数
		do{      
			///第二次循环确定顺子在这种个数情况下的大小
			haveSun = true;       //因为此时一定有孩子
			BYTE tempCard[CARD_COUNT_PER_PERSON];
			int tempNum;
			memcpy(tempCard,m_Card,sizeof(BYTE)*num);
			tempNum = num;

			peiOutCardStruct out;
			out.out.iCardCount = iBaseCount;                                         
			memcpy(out.out.iCardList,iResultCard,sizeof(BYTE)*iBaseCount); 
			out.type = 6;
            if(canPush(myCard,out))
			{
				del(tempCard,tempNum,out.out.iCardList,iBaseCount);
				myCard.push_back(out);
				if(peiCard(tempCard,tempNum,myCard,para,pClientGameDlg))
				{
					return true;                     //找到满足之后停止运算并将结果返回
				}
				myCard.pop_back();				
			}

			memcpy(iBaseCard,iResultCard,sizeof(BYTE)*iBaseCount);
		}while(logic.TackOutSequence(m_Card,num,iBaseCard,iBaseCount,iResultCard,iResultCardCount,1));

		iBaseCount+=1;
	}
	return false;
}

/// 生成含有三张的孩子
bool sanPeiCard(BYTE *m_Card,int num,std::vector<peiOutCardStruct>& myCard,bool (*para)(BYTE*,int,std::vector<peiOutCardStruct>&,CClientGameDlg *pClientGameDlg),bool &haveSun,CClientGameDlg *pClientGameDlg)
{
	for(int cardNum = 3;cardNum<15;cardNum++)
	{
		///配牌时3个的时候不可以拆炸弹，因为如果将炸弹拆成3个和1个，一定是因为顺子，否则太愚蠢
		if(getSomeCount(m_Card,cardNum,num)>=3)
		{
			haveSun = true;       //因为此时一定有孩子
			BYTE tempCard[CARD_COUNT_PER_PERSON];
			int tempNum;
			memcpy(tempCard,m_Card,sizeof(BYTE)*num);
			tempNum = num;

			peiOutCardStruct out;
			out.out.iCardCount = 0;                                         
			for(int i=0;i<tempNum;i++)
			{
				if(logic.GetCardNum(m_Card[i]) == cardNum)
				{
					out.out.iCardList[out.out.iCardCount++] = m_Card[i];
					if(out.out.iCardCount >= 3)
						break;
				}
			}
			out.type = 7;
            if(!canPush(myCard,out))
			{
				continue;
			}
			del(tempCard,tempNum,out.out.iCardList,3);
			myCard.push_back(out);
			if(peiCard(tempCard,tempNum,myCard,para,pClientGameDlg))
			{
				return true;                     //找到满足之后停止运算并将结果返回
			}
			myCard.pop_back();
		}
	}

	return false;
}

/// 生成含有对子的孩子
bool duiPeiCard(BYTE *m_Card,int num,std::vector<peiOutCardStruct>& myCard,bool (*para)(BYTE*,int,std::vector<peiOutCardStruct>&,CClientGameDlg *pClientGameDlg),bool &haveSun,CClientGameDlg *pClientGameDlg)
{
	int havePeiHand = myCard.size();
	BYTE tempCard[CARD_COUNT_PER_PERSON];
	int tempNum;

	for(int cardNum = 3;cardNum<15;cardNum++)
	{
		int n = getSomeCount(m_Card,cardNum,num);
		/// 配牌时不会因为为了有对子而猜炸弹，但是可以为了因为有对子而拆三张，因为为了有回手牌
		if(n>=2)
		{
			haveSun = true;       //因为此时一定有孩子

			memcpy(tempCard,m_Card,sizeof(BYTE)*num);
			tempNum = num;

			peiOutCardStruct out;
			out.out.iCardCount = 0;                                         
			for(int i=0;i<tempNum;i++)
			{
				if(logic.GetCardBulk(tempCard[i]) == cardNum)
				{
					out.out.iCardList[out.out.iCardCount++] = tempCard[i];
					if(out.out.iCardCount >= 2)
						break;
				}
			}
			out.type = 8;
            if(!canPush(myCard,out))
			{
				continue;
			}
			del(tempCard,tempNum,out.out.iCardList,2);
			myCard.push_back(out);	
			if(peiCard(tempCard,tempNum,myCard,para,pClientGameDlg))
			{
				return true;                     //找到满足之后停止运算并将结果返回
			}
			myCard.pop_back();
		}
	}


	return false;
}
/// 求手中某种牌的个数
int  getSomeCount(BYTE *m_Card,int cardNum,int num)
{
	int result =0;
	for(int i=0;i<num;i++)
	{
		if(logic.GetCardBulk(m_Card[i])==cardNum)
			result++;
	}
	return result;
}

/// 根据自己的运算规则，将可以配成的出牌牌型排序，从而判断一个新的牌型是否可以添加到配好的数据当中
/// myCard是已经配好的牌，out是即将要配上的牌
/// @return 如果返回true就可以配，如果返回false就因为已经配出过这种牌型不必再配
bool canPush(std::vector<peiOutCardStruct>& myCard,peiOutCardStruct &out)
{
	/// 因为从王2到对子配牌时是有一定优先级的，如果破坏了这个优先级就说明这种牌型已经配过了
	if(myCard.empty())
		return true;
	if(myCard.back().type>out.type)
		return false;
	if(myCard.back().type<out.type)
		return true;
    switch (out.type)
	{
	/// 炸弹三张和对子只需比较大小
	case 3:
	case 7:
	case 8:
		if(logic.CompareOnlyOne(out.out.iCardList[0],myCard.back().out.iCardList[0]))
			return false;
		else
			return true;
	case 4:
	case 5:
	case 6:
		if(myCard.back().out.iCardCount>out.out.iCardCount)
			return false;
		else if(myCard.back().out.iCardCount<out.out.iCardCount)
			return true;
		else
		{
			if(logic.GetSequenceStartPostion(myCard.back().out.iCardList,myCard.back().out.iCardCount,7-out.type)>logic.GetSequenceStartPostion(out.out.iCardList,out.out.iCardCount,7-out.type))
				return false;
			else
				return true;
		}
			
	}

	return true;
}

/// 用于打印一张牌的内容
void printCard(BYTE card,CString &str)
{
	char *tableNumStr[13]={"2","3","4","5","6","7","8","9","10","J","Q","K","A"};
	int temp = logic.GetCardNum(card);
	if(card!=0x4E&&card!=0x4F)
	{
		str = tableNumStr[temp-2];
	}
	else
	{
		if(card == 0x4E)
			str = "小鬼";
		if(card == 0x4F)
			str = "大鬼";
	}
}


/// 打印一手牌的内容
void printCard(BYTE *m_Card,int num,CString &str)
{
	CString strTemp;
	for(int i=0;i<num;i++)
	{
		
		printCard(m_Card[i],strTemp);
		str += strTemp;
		str += " ";
	}
}