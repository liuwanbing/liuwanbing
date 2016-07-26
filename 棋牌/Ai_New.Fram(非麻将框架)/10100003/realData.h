
#include <list>
#include <vector>
#include <algorithm>
using namespace std;

#include "clientgamedlg.h"
#include "advant.h"

///实例化的结点数据
extern CUpGradeGameLogic logic;

struct DaiPaiData
{
	/// 带牌的个数(对子算一个)
	BYTE num;     
	/// 带牌的类型，1为带单牌，2为带对子
	BYTE type;
	/// 可带的牌，主要是标记牌点,
	OutCardStruct outCard;
};

class realData
{
public:

	realData& operator= (realData& d)
	{
		m_heap = d.m_heap;
     	memcpy(m_DeskCard,d.m_DeskCard,sizeof(m_DeskCard));                   
	    vLeftCard = d.vLeftCard;
        m_index = d.m_index;
	    m_iNtPeople = d.m_iNtPeople;	
	    m_myDeskStation = d.m_myDeskStation;
	    m_iNowOutPeople = d.m_iNowOutPeople;		
    	m_iBigOutPeople = d.m_iBigOutPeople;						
	    m_BigOut = d.m_BigOut;
		m_isSureWin = d.m_isSureWin;
		m_myCardCount = d.m_myCardCount;
		memcpy(m_CardCount,d.m_CardCount,sizeof(m_CardCount));

        for(int i=0;i<3;i++)
		{
			//m_DaiDanCard[i] = d.m_DaiDanCard[i];     ///此二者为临时数据，用于记录自己生的孩子的状况
   //         m_DaiDuiCard[i] = d.m_DaiDuiCard[i];
//			vChuCard[i] = d.vChuCard[i];
		}

		return *this;
	}


	realData()
	{
		m_heap = 0;
		m_isDaiPai = false;
		for(int i=0;i<3;i++)
		{
			m_DaiDanCard[i].clear();
			m_DaiDuiCard[i].clear();
		}

		m_IamBomb = false;
		m_bombNum = 0;
		
	}

    ~realData()
	{
		
	}

	bool isRightResult()                       ///用于判断自己是否是一个要找到的解
	{
		return false;
	}


    void acess(realData& d)                     ///访问自己的孩子，用于回溯结果，遍历子孙
	{	
		if(m_iNowOutPeople == m_myDeskStation)   //自己出牌的时候，这时只要他的一个儿子必赢即可，接着选择自己炸弹数量最多的一个
		{
			///在必赢的情况下，需要考虑炸弹的数量，自己出牌，需要考虑自己是否是炸弹
			if(d.m_isSureWin&&((m_bombNum-m_IamBomb?1:0)<d.m_bombNum||!m_isSureWin))
			{
				m_isDaiPai = d.m_isDaiPai;
				m_FirstDaiPai = d.m_FirstDaiPai;
				m_isSureWin = true;
                m_bombNum = d.m_bombNum + m_IamBomb?1:0;
                m_BigOut = d.m_BigOut;
			}
		}
		else         //自己是模拟他人出牌，这时需要所有的孩子节点全部都可赢才行，因为模拟对方运算，所以不必考虑炸弹的数量
		{
			///只有自己还是必赢的状态的时候才计算,此时无法记录对方所出的牌
			if(m_isSureWin)
			{
				m_FirstDaiPai = d.m_FirstDaiPai;
				m_isSureWin = d.m_isSureWin;
				///这时为了分析炸弹状态，也需要将每个节点的炸弹数都上传到父节点
                if(m_bombNum<d.m_bombNum)
			    	m_bombNum = d.m_bombNum;

			}				
		}
	}

	void born(list<realData> &lData)            ///用于生成自己的孩子结点的数据
	{
	//	OutputDebugString("log:我要生孩子");

		for(int i=0;i<PLAY_COUNT;i++)
		{
			if(i!=m_myDeskStation&&m_myDeskStation!=m_iNtPeople&&i!=m_iNtPeople)       ///如果这时同伙把牌打出不能说明什么
				continue;
			if(m_CardCount[i]<=0)      ///已分胜负，此时的节点为叶子了
			{
				if(i==m_myDeskStation)
					m_isSureWin = true;
				else
					m_isSureWin = false;
				return;
			}
		}
		if(m_CardCount[m_myDeskStation]>=8||m_heap>2)
		{
			m_isSureWin = false;
			return;
		}
		initSureWin();        //生孩子前，改好自己的状态
		if(m_iNowOutPeople == m_myDeskStation)
		{
			if(m_iBigOutPeople == m_myDeskStation)
			{
				bornMyFirstSun(lData);
			}
			else
			{
				bornMySun(lData);

			}
		}
		else
		{
			///当出现对手先手出牌的时候就停止计算，因为这种状况被看做是不必赢的情况，所以统一认为停止计算
			bornYouSun(lData);
		}
		if(lData.size()==0)
		{
			m_isSureWin = false;
		}
	}

	///结果处理
	/// 此时的d节点所出的牌是需要带牌的牌，要在出牌中加上要带的牌
	/// 此成员仅为处理出牌结果而已，所以带牌状况由m_FirstDaiPai决定
	void disposeDaiPai()
	{
		if(!m_isDaiPai)
			return;
		if(m_FirstDaiPai.type == 1)  ///单牌
		{
			for(int i=0;i<m_FirstDaiPai.num;i++)
			{
				m_BigOut.iCardList[m_BigOut.iCardCount++] = m_FirstDaiPai.outCard.iCardList[i];
			}
		}
		else          ///对子
		{
			for(int i=0;i<m_FirstDaiPai.num;i++)
			{
				int c = 0;
                for(int j=0;j<m_myCardCount;j++)
				{
					if(logic.GetCardBulk(m_DeskCard[j])==logic.GetCardBulk(m_FirstDaiPai.outCard.iCardList[i]))
					{
						m_BigOut.iCardList[m_BigOut.iCardCount++] = m_DeskCard[j];
						if(c++>0)
							break;
					}
				}
				
			}
		}
	}
public:
	/// 记录自己的位置，是第多少的子孙
	int                           m_heap;
	/// 带牌状况，true是带牌，false是不带牌飞
	bool                          m_isDaiPai;

	/// 标记曾经出过的牌所带的牌，现有数据需要根据当前状态来判断
	std::vector<DaiPaiData>       m_DaiPaiData[PLAY_COUNT];
	
	/// 记录最后计算的第一次带牌的结果,因为其它的带牌的结果并不重要
    DaiPaiData                    m_FirstDaiPai;       
    /// 为所有人手上的牌的真实数量
    BYTE					       m_CardCount[PLAY_COUNT];				//自己手中的牌的真实数量
	/// 为自己的手上可选的牌的牌数
	int					       m_myCardCount;					//自己手中的牌的数量

	/// 
	
	/// 为自己手上的牌
	BYTE                           m_DeskCard[CARD_COUNT_PER_PERSON];                   
	/// 记录自己和其它几人出过的牌，主要用于分析对方手中的牌，和了解对方对自己的认识，不参与出牌
	std::vector<OutCardStruct>     vChuCard[PLAY_COUNT];   
	/// 保留其它二人手中的牌的全部情况，主要用于猜牌
	std::vector<guessCard>              vLeftCard;
    /// 标记自己的位置状态，0为地主，1为地主下家，2为地主上家
	BYTE                                m_index;
    /// 地主的位置
	BYTE					            m_iNtPeople;	
	/// 自己的位置
	BYTE                                m_myDeskStation;
    /// 现在出牌者的位置，模拟的
	BYTE				             	m_iNowOutPeople;		
    /// 当前最大出牌者
	BYTE				            	m_iBigOutPeople;						

	/// 现在桌面上最大的牌
    OutCardStruct                    m_BigOut;
    
	std::vector<OutCardStruct>  m_DaiDanCard[PLAY_COUNT];           /// 记录三个人带单牌的可能的牌
	std::vector<OutCardStruct>  m_DaiDuiCard[PLAY_COUNT];           /// 记录三个人带对子的可能的牌
	
	bool                                m_isSureWin;
	int                                 m_bombNum;
	bool                                m_IamBomb;                  /// 记录自己是不是炸弹

private:
	/// 加强可读性，清晰性
    
	/// 针对生成孩子的时候的处理带牌
	void disposeDaiPai(list<realData> &lData,realData &data,peiOutCardStruct &zongZi,int &danDaiIndex,int &duiDaiIndex)
	{
		data.m_isDaiPai = true;
		if(zongZi.type == 9)
		{
			data.m_FirstDaiPai.type = 1;       //单牌
			data.m_FirstDaiPai.num =1;         //三带一，所以带的一手牌
			data.m_FirstDaiPai.outCard = m_DaiDanCard[data.m_iBigOutPeople][danDaiIndex];
            danDaiIndex += data.m_FirstDaiPai.num;
		}
		else if(zongZi.type == 10)
		{
			data.m_FirstDaiPai.type = 2;       //对子
			data.m_FirstDaiPai.num =1;         //三带一对，所以带的一手牌
			data.m_FirstDaiPai.outCard = m_DaiDuiCard[data.m_iBigOutPeople][duiDaiIndex];
            duiDaiIndex += data.m_FirstDaiPai.num;
		}
		else if(zongZi.type == 11)
		{
			data.m_FirstDaiPai.type = 1;       //单牌
			data.m_FirstDaiPai.num = (zongZi.out.iCardCount/3);         //三顺带单，所以带的一手牌
			data.m_FirstDaiPai.outCard = m_DaiDanCard[data.m_iBigOutPeople][danDaiIndex];
            danDaiIndex += data.m_FirstDaiPai.num;
		}
		else if(zongZi.type == 12)
		{
			data.m_FirstDaiPai.type = 2;       //对子
			data.m_FirstDaiPai.num = (zongZi.out.iCardCount/3);         //三带一对，所以带的一手牌
			data.m_FirstDaiPai.outCard = m_DaiDuiCard[data.m_iBigOutPeople][duiDaiIndex];
            duiDaiIndex += data.m_FirstDaiPai.num;
		}
		else if(zongZi.type == 13)
		{
			data.m_FirstDaiPai.type = 1;       //单牌
			data.m_FirstDaiPai.num =2;         //四带二单，所以带的一手牌
			data.m_FirstDaiPai.outCard = m_DaiDanCard[data.m_iBigOutPeople][danDaiIndex];
            danDaiIndex += data.m_FirstDaiPai.num;
		}
		else if(zongZi.type == 14)
		{
			data.m_FirstDaiPai.type = 2;       //对子
			data.m_FirstDaiPai.num =2;         //四带二对，所以带的一手牌
			data.m_FirstDaiPai.outCard = m_DaiDuiCard[data.m_iBigOutPeople][duiDaiIndex];
            duiDaiIndex += data.m_FirstDaiPai.num;
		}
		else   ///不带
		{
			data.m_isDaiPai = false;
		}
        if(data.m_isDaiPai)
		{
            data.m_CardCount[data.m_iBigOutPeople] -= data.m_FirstDaiPai.type*data.m_FirstDaiPai.num;
	    	data.m_DaiPaiData[data.m_iBigOutPeople].push_back(data.m_FirstDaiPai);
		}
	}
    /// 用于删除不合理带牌数据，返回是否存在必须是某些带牌的数据
	///  daiPai带牌数据的汇总，tempBulk现在手中的牌的牌点数，delDuiZi标记本次是否删除所有的对子
	bool delWrongDaiPai(std::vector<DaiPaiData> &daiPai,int *tempBulk,bool delDuiZi)
	{
		if(delDuiZi)
		{
			for(int i=0;i<daiPai.size();i++)
			{
				if(daiPai[i].type == 2)
				{
					for(int j=0;j<daiPai[i].outCard.iCardCount;j++)
					{
						tempBulk[logic.GetCardBulk(daiPai[i].outCard.iCardList[j])] -= 2;
					}
					daiPai.erase(daiPai.begin()+i);
					i--;
					continue;
				}

			}
		}
        for(int i=0;i<daiPai.size();i++)
		{
			if(daiPai[i].num == daiPai[i].outCard.iCardCount)          ///删掉这些已经确定的牌
			{
				for(int j=0;j<daiPai[i].outCard.iCardCount;j++)
				{
					tempBulk[logic.GetCardBulk(daiPai[i].outCard.iCardList[j])] -= 2;
				}
				daiPai.erase(daiPai.begin()+i);
				return true;
			}
		}
		return false;
	}

	/// 用于判断新产生的牌是否符合要求,着眼于带牌情况
	/// daiPai带牌数据的汇总，zongZi本次出牌的信息，主要为了得知是否带牌，m_DeskCard,num为手中现有的牌
	bool canDaiPai(std::vector<DaiPaiData> &daiPai,BYTE *m_DeskCard,int &num)
	{
		///求目前手中的剩下的牌中可以放入过去带牌中的内容
		int tempBulk[18];
		memset(tempBulk,0,sizeof(tempBulk));
		for(int i=0;i<num;i++)
		{
			tempBulk[logic.GetCardBulk(m_DeskCard[i])]++;
		}
        
		int duiNum = 0;
		int haveNum = 0;
IVERIT:
        duiNum = 0;
		haveNum = 0;
		/// 判断每个可带的牌的状况，如果某个元素不能带出相应多的牌，则返回false
		for(int i=0;i<daiPai.size();i++)
		{
			for(int j=0;j<daiPai[i].outCard.iCardCount;j++)
			{
				if(tempBulk[logic.GetCardBulk(daiPai[i].outCard.iCardList[j])]<daiPai[i].type)
					del(daiPai[i].outCard.iCardList,daiPai[i].outCard.iCardCount,j--);						
			}
			if(daiPai[i].outCard.iCardCount<daiPai[i].num)
				return false;
			if(daiPai[i].type == 2)
				duiNum = daiPai[i].num;
		}
		/// 根据对子的数量判断是否可以带出相应的牌,总的数量一定满足要求不必理会
		for(int cardNum =3;cardNum<18;cardNum++)
		{
			if(tempBulk[cardNum]>=2)
				haveNum++;
		}
        if(duiNum>haveNum)
		{
			return false;
		}
        /// 删除不合理的带牌状况
        if(delWrongDaiPai(daiPai,tempBulk,(duiNum==haveNum)&&haveNum!=0))
		{
			goto IVERIT;
		}
		/// 删了对子
        if((duiNum==haveNum)&&haveNum!=0)
			goto IVERIT;
        for(int i=0;i<num;i++)
		{
			tempBulk[logic.GetCardBulk(m_DeskCard[i])]--;
		}
        for(int cardNum =3;cardNum<18;cardNum++)
		{
			for(int i=0;i<num&&tempBulk[cardNum]!=0;i++)
			{
				if(logic.GetCardBulk(m_DeskCard[i])==cardNum)
				{
					del(m_DeskCard,num,i);
                    tempBulk[cardNum]--;
				}
			}
		}
		return true;
	}
	/// 处理对伙出牌后的手中牌状态
	bool isOenPeople(realData &data)
	{
        bool ishave = false;
		for(int desk=0;desk<3;desk++)
		{
			if(desk == data.m_myDeskStation||(data.m_myDeskStation!=data.m_iNtPeople&&desk!=data.m_iNtPeople))
			{
				continue;
			}
			std::vector<guessCard> leftCard = data.vLeftCard;
			for(int i=0;i<data.m_BigOut.iCardCount;i++)
			{
				ishave = false;
			 //   leftCard = data.vLeftCard;
				for(int j=0;j<leftCard.size();j++)
				{
					if(logic.GetCardBulk(leftCard[j].data) == logic.GetCardBulk(data.m_BigOut.iCardList[i]))
					{
						if(leftCard[j].desk == 255||leftCard[j].desk == desk)
						{
							leftCard.erase(leftCard.begin()+j);
							ishave = true;
							break;
						}	
					}
				}
				if(!ishave)
				{
					break;
				}				
			}
			if(ishave)
			{
				data.vLeftCard = leftCard;
				return true;
			}
		}
		return false;

	}

    /// 用于判断生成的结果是否可行，因为可带牌的因素，开始的时候不能
    bool canBorn(realData &data)
	{
		
		if(data.m_iBigOutPeople == data.m_myDeskStation)  //自己刚出完的时候
		{
			data.m_heap++;
			return canDaiPai(data.m_DaiPaiData[data.m_myDeskStation],data.m_DeskCard,data.m_myCardCount);
		}
		else          ///对方刚出完的时候
		{

			return isOenPeople(data);
		}
		return true;
	}
	///// 用于当轮到自己先手出牌的时候，用于生成自己可出的牌的状况
	//void bornMyFirstSun(list<realData> &lData,peiOutCardStruct &zongZi,int &danDaiIndex,int &duiDaiIndex) 
	//{
	//	realData temp;
 //       temp = *this;
 //       /// 前期处理
 //       temp.m_BigOut = zongZi.out;
	//	/// 其它两人虽然同伙不同伙区别很大，但是在判断是否必赢的情况下区别不大
	//	temp.m_iNowOutPeople = (m_myDeskStation+1)%PLAY_COUNT;       
	//	temp.m_iBigOutPeople = m_myDeskStation;
 //       
	//	/// 将带牌处理好
	//	disposeDaiPai(lData,temp,zongZi,danDaiIndex,duiDaiIndex);
 //  //     temp.m_myCardCount = temp.m_myCardCount - zongZi.out.iCardCount;
 //       temp.m_CardCount[m_myDeskStation] = temp.m_CardCount[m_myDeskStation] - zongZi.out.iCardCount;
	//	del(temp.m_DeskCard,temp.m_myCardCount,zongZi.out.iCardList,zongZi.out.iCardCount);    
 //        
	//	if(zongZi.type == 3||(zongZi.type == 1&&zongZi.out.iCardCount==2)||(zongZi.type == 2&&zongZi.out.iCardCount==4))
	//	{
	//		temp.m_IamBomb = true;
	//		temp.m_bombNum = 1;
	//	}

	//	/// 判断生成的结果是否违法,看带牌

	//	if(canBorn(temp))
	//		lData.push_back(temp);

	//}

	/// 用于当轮到自己先手出牌的时候，用于生成自己可出的牌的状况
	void bornMySun(list<realData> &lData,peiOutCardStruct &zongZi,int &danDaiIndex,int &duiDaiIndex) 
	{
		realData temp;
        temp = *this;
        /// 前期处理
        temp.m_BigOut = zongZi.out;
		/// 其它两人虽然同伙不同伙区别很大，但是在判断是否必赢的情况下区别不大
		temp.m_iNowOutPeople = (m_myDeskStation+1)%PLAY_COUNT;       
		temp.m_iBigOutPeople = m_myDeskStation;
        
		/// 将带牌处理好
		disposeDaiPai(lData,temp,zongZi,danDaiIndex,duiDaiIndex);
   //     temp.m_myCardCount = temp.m_myCardCount - zongZi.out.iCardCount;
        temp.m_CardCount[m_myDeskStation] = temp.m_CardCount[m_myDeskStation] - zongZi.out.iCardCount;
		del(temp.m_DeskCard,temp.m_myCardCount,zongZi.out.iCardList,zongZi.out.iCardCount);    
         
		if(zongZi.type == 3||(zongZi.type == 1&&zongZi.out.iCardCount==2)||(zongZi.type == 2&&zongZi.out.iCardCount==4))
		{
			temp.m_IamBomb = true;
			temp.m_bombNum = 1;
		}

		/// 判断生成的结果是否违法,看带牌

		if(canBorn(temp))
		{
			
		    lData.push_back(temp);
		}
	}
    /// 用于当轮到自己先手出牌的时候，用于生成自己可出的牌的状况,模拟对方出牌
	void bornYouSun(list<realData> &lData,peiOutCardStruct &zongZi,int &danDaiIndex,int &duiDaiIndex) 
	{
		realData temp;
        temp = *this;
        /// 前期处理
        temp.m_BigOut = zongZi.out;
		/// 其它两人虽然同伙不同伙区别很大，但是在判断是否必赢的情况下区别不大
		temp.m_iNowOutPeople = m_myDeskStation;       
		temp.m_iBigOutPeople = (m_myDeskStation+1)%PLAY_COUNT;
        
		/// 将带牌处理好，为了简便处理，对方不考虑带牌
		//disposeDaiPai(lData,temp,zongZi,danDaiIndex,duiDaiIndex);
        int chuNum = zongZi.out.iCardCount;
		if(zongZi.type == 9)
		{
			chuNum++;
		}
		else if(zongZi.type == 10)
		{
			chuNum+=2;
		}
		else if(zongZi.type == 11)
		{
			chuNum+=(zongZi.out.iCardCount/3);
		}
		else if(zongZi.type == 12)
		{
			chuNum+=(zongZi.out.iCardCount/3*2);
		}
		else if(zongZi.type == 13)
		{
			chuNum+=2;
		}
		else if(zongZi.type == 14)
		{
			chuNum+=4;
		}
		for(int i=0;i<3;i++)
		{
			if((m_myDeskStation==m_iNtPeople&&i!=m_myDeskStation)||(m_myDeskStation!=m_iNtPeople&&i==m_iNtPeople))
			{
				if(temp.m_CardCount[i]>=chuNum)
					temp.m_CardCount[i] -= chuNum;
			}
		}
         
		if(zongZi.type == 3||(zongZi.type == 1&&zongZi.out.iCardCount==2)||(zongZi.type == 2&&zongZi.out.iCardCount==4))
		{
			temp.m_IamBomb = true;
			temp.m_bombNum = 1;
		}

		/// 判断生成的结果是否违法,看带牌

		if(canBorn(temp))
			lData.push_back(temp);

	}
	/// 用于当轮到自己先手出牌的时候，用于生成自己可出的牌的状况
	void bornMyFirstSun(list<realData> &lData) 
	{
		std::vector<peiOutCardStruct> iCanOut;
	
		m_DaiDanCard[m_myDeskStation].clear();
		m_DaiDuiCard[m_myDeskStation].clear();
		canAutoChu(m_DeskCard,m_myCardCount,m_CardCount[m_myDeskStation],iCanOut);      
		int danDaiIndex = 0;
		int duiDaiIndex = 0;

		CString str;
		str.Format("log:我有%d出法",iCanOut.size());
		OutputDebugString(str);
		if(iCanOut.size()>=6)
			return;
		for(int i=0;i<iCanOut.size();i++)
		{
			bornMySun(lData,iCanOut[i],danDaiIndex,duiDaiIndex);
		}

	}
	void getOutCard(const peiOutCardStruct &one,OutCardStruct &out)
	{
		out = one.out;
		int tempBuk[18];
		memset(tempBuk,0,sizeof(tempBuk));
		for(int i=0;i<one.out.iCardCount;i++)
		{
			tempBuk[logic.GetCardBulk(one.out.iCardList[i])]++;
		}
		int num = 0;        //个数
		int type = 0;       //1是单牌，2是对子
		out = one.out;
		if(one.type == 9)
		{
			num = 1;
			type = 1;
		}
		else if(one.type == 10)
		{
			num = 1;
			type = 2;
		}
		else if(one.type == 11)
		{
			num = one.out.iCardCount/3;
			type = 1;
		}
		else if(one.type == 12)
		{
			num = one.out.iCardCount/3;
			type = 2;
		}
		else if(one.type == 13)
		{
			num = 2;
			type = 1;
		}
		else if(one.type == 14)
		{
			num = 2;
			type = 2;
		}
		int cardNum = 2;
		for(int i=0;i<num;i++)
		{
			while(cardNum<1&&tempBuk[++cardNum]==3);
			if(type == 1)
			{
				out.iCardList[out.iCardCount++] = (BYTE)(cardNum-1);
			}
			else
			{
				out.iCardList[out.iCardCount++] = (BYTE)(cardNum-1);
				out.iCardList[out.iCardCount++] = (BYTE)(cardNum-1);
			}

		}
	}
	/// 判断这一牌型自己能不能获得老大地位
	bool isCaiJian(peiOutCardStruct &peiOut)
	{
		std::vector<peiOutCardStruct> iCanOut;
        
        OutCardStruct lastOut;
        getOutCard(peiOut,lastOut);	
		BYTE handCard[54];
		int num = 0;
		int haveNum;
		if(m_myDeskStation == m_iNtPeople)
		{
			for(int i=0;i<vLeftCard.size();i++)
			{
				handCard[num++] = vLeftCard[i].data;
			}
			haveNum = m_CardCount[(m_myDeskStation+1)/PLAY_COUNT]>m_CardCount[(m_myDeskStation+2)/PLAY_COUNT]?m_CardCount[(m_myDeskStation+1)/PLAY_COUNT]:m_CardCount[(m_myDeskStation+2)/PLAY_COUNT];
		}
		else
		{
			for(int i=0;i<vLeftCard.size();i++)
			{
				if(vLeftCard[i].desk == m_iNtPeople||vLeftCard[i].desk == 255)
			    	handCard[num++] = vLeftCard[i].data;
			}
			haveNum = m_CardCount[m_iNtPeople];
		}
		
		int danDaiIndex = m_DaiDanCard[m_myDeskStation].size();
		int duiDaiIndex = m_DaiDuiCard[m_myDeskStation].size();
		canChu(handCard,num,haveNum,lastOut,iCanOut);     
		m_DaiDanCard[m_myDeskStation].resize(danDaiIndex);
		m_DaiDuiCard[m_myDeskStation].resize(duiDaiIndex);
		return iCanOut.size()!=0;

	}
    /// 用于当轮到自己出牌管人的时候，用于生成自己可出的牌的状况
	void bornMySun(list<realData> &lData) 
	{
		std::vector<peiOutCardStruct> iCanOut;
        
        OutCardStruct lastOut;
        getMyOut(lastOut);	

		m_DaiDanCard[m_myDeskStation].clear();
		m_DaiDuiCard[m_myDeskStation].clear();
		canChu(m_DeskCard,m_myCardCount,m_CardCount[m_myDeskStation],lastOut,iCanOut);      
		int danDaiIndex = 0;
		int duiDaiIndex = 0;
		//CString str;
		//OutputDebugString("log:我可以出");
		//for(int i=0;i<iCanOut.size();i++)
		//{
		//	str.Format("log:");
		//	printCard(iCanOut[i].out.iCardList,iCanOut[i].out.iCardCount,str);
		//	OutputDebugString(str);
		//}

		if(iCanOut.size() == 0)   ///说明不能赢
		{
			m_isSureWin = false;
			return;
		}
		bool caiDiao = isCaiJian(iCanOut.back());
		int m=iCanOut.size();
		for(int i=0;i<iCanOut.size();i++)
		{
			bornMySun(lData,iCanOut[i],danDaiIndex,duiDaiIndex);
			if(caiDiao&&!lData.empty()&&(lData.back().m_CardCount[m_myDeskStation]>0))
				lData.pop_back();
		}
        

	}
    /// 用于当轮到对方出牌的时候，用于生成自己可出的牌的状况
	void bornYouSun(list<realData> &lData) 
	{
		/// 在模拟对方出牌的地方，只模拟异伙，不模拟同伙，只有对方管牌的情况
		std::vector<peiOutCardStruct> iCanOut;
        
        OutCardStruct lastOut;
        getMyOut(lastOut);	

		m_DaiDanCard[m_myDeskStation].clear();
		m_DaiDuiCard[m_myDeskStation].clear();
		getYouCanChu(lastOut,iCanOut);      
		int danDaiIndex = 0;
		int duiDaiIndex = 0;
        /// 首先看最大的一个是否大于所有的牌，如果是那么需要停止
		int dataSize = lData.size();
		while(!iCanOut.empty()&&dataSize == lData.size())
		{
			bornYouSun(lData,iCanOut.back(),danDaiIndex,duiDaiIndex);
			iCanOut.pop_back();
		} 
		list<realData> canDa;
		if(!lData.empty())
	    	lData.back().born(canDa);
		if(!canDa.empty())
		{
			lData.clear();
			return;
		}
		for(int i=0;i<iCanOut.size();i++)
		{
			bornYouSun(lData,iCanOut[i],danDaiIndex,duiDaiIndex);
		}
		
		realData temp = *this;
		/// 前期处理
		temp.m_iNowOutPeople = m_myDeskStation;       
		temp.m_iBigOutPeople = m_myDeskStation;
		lData.push_back(temp);
	}
private:
	/// 获取对方可以出的牌种类
	void getYouCanChu(OutCardStruct lastOut,std::vector<peiOutCardStruct> &canOut)
	{
		BYTE handCard[54];
		int num = 0;
		int haveNum;
		
	
		if(m_myDeskStation == m_iNtPeople)
		{
			for(int i=0;i<vLeftCard.size();i++)
			{
				handCard[num++] = vLeftCard[i].data;
			}
			haveNum = m_CardCount[(m_myDeskStation+1)/PLAY_COUNT]>m_CardCount[(m_myDeskStation+2)/PLAY_COUNT]?m_CardCount[(m_myDeskStation+1)/PLAY_COUNT]:m_CardCount[(m_myDeskStation+2)/PLAY_COUNT];
		}
		else
		{
			for(int i=0;i<vLeftCard.size();i++)
			{
				if(vLeftCard[i].desk == m_iNtPeople||vLeftCard[i].desk == 255)
			    	handCard[num++] = vLeftCard[i].data;
			}
			haveNum = m_CardCount[m_iNtPeople];
		}

		//CString str;
		//OutputDebugString("log:对方有牌");

		//str.Format("log:");
		//printCard(handCard,num,str);
		//OutputDebugString(str);
		//str.Format("log:我出:");
		//printCard(m_BigOut.iCardList,m_BigOut.iCardCount,str);
		//OutputDebugString(str);	
		canChu(handCard,num,haveNum,lastOut,canOut);      
	}

    /// 获取上一次对手出的最大的牌
	void getMyOut(OutCardStruct &lastOut)
	{
		lastOut = m_BigOut;
		if(m_isDaiPai)
		{
			if(m_FirstDaiPai.type == 1)
			{
				for(int i=0;i<m_FirstDaiPai.num;i++)
				{
					lastOut.iCardList[lastOut.iCardCount++] = m_FirstDaiPai.outCard.iCardList[i];
				}
			}
			else
			{
				for(int i=0;i<m_FirstDaiPai.num;i++)
				{
					lastOut.iCardList[lastOut.iCardCount++] = m_FirstDaiPai.outCard.iCardList[i];
					lastOut.iCardList[lastOut.iCardCount++] = m_FirstDaiPai.outCard.iCardList[i];
				}
			}

		}
	}

	/// 根据手中可能有的牌某人决定出牌的情况汇总
	/// handCard，num为某人可能有的牌，haveNum为某人的牌数，canOut为此人可以出的牌的可能性总和
	void canAutoChu(BYTE *handCard,int num,int haveNum,std::vector<peiOutCardStruct> &canOut)
	{
		canAutoKingBomb(handCard,num,haveNum,canOut);   //王炸
		canAutoDanChu(handCard,num,haveNum,canOut);     //单张牌
		canAutoDuiChu(handCard,num,haveNum,canOut);     //对子
        canAutoSanChu(handCard,num,haveNum,canOut);     //三张
        canAutoSiChu(handCard,num,haveNum,canOut);      //四张
		canAutoShun3Chu(handCard,num,haveNum,canOut);   //三顺
	    canAutoShun2Chu(handCard,num,haveNum,canOut);   //双顺
		canAutoShun1Chu(handCard,num,haveNum,canOut);   //单顺

	}

    /// 管牌操作，对方出了一手牌，现在轮到某人出牌时的操作
	/// hanCard，num为某人手中可能有的牌，haveNum为某人手中的牌数，out为目前桌子上出的最大的牌，canOut为此人可以出的牌的可能性总和
	void canChu(BYTE *handCard,int num,int haveNum,OutCardStruct out,std::vector<peiOutCardStruct> &canOut)
	{
		/// 因为有时候有三带一带对子等带牌出现，所以需要自己去处理带牌，并且针对拆牌的处理，也需要仔细处理
		int iBaseShape = logic.GetCardShape(out.iCardList, out.iCardCount); //桌面上牌的牌型
        switch(iBaseShape)
		{
		case UG_ONLY_ONE: //单张
		case UG_DOUBLE:   //对牌
            canChuDanDui(handCard,num,haveNum,out,canOut,iBaseShape);
			break;
		case UG_THREE:    //三张
        case UG_THREE_ONE: //三带一
        case UG_THREE_DOUBLE:	//三带一对
            canChuSan(handCard,num,haveNum,out,canOut,iBaseShape);
			break;
		case UG_STRAIGHT: //顺子
		case UG_DOUBLE_SEQUENCE: //连对
            canChuShun12(handCard,num,haveNum,out,canOut,iBaseShape);
			break;
		case UG_THREE_SEQUENCE: //连三
        case UG_THREE_ONE_SEQUENCE: //三带一的连牌
		case UG_THREE_DOUBLE_SEQUENCE://三带对连牌
            canChuShun3(handCard,num,haveNum,out,canOut,iBaseShape);
			break;
		case UG_FOUR_TWO:         ///考虑四带两张和两对的情况
		case UG_FOUR_TWO_DOUBLE:
			canChuSi(handCard,num,haveNum,out,canOut,iBaseShape);
		}
        /// 考虑炸弹
        canChuBomb(handCard,num,haveNum,out,canOut,iBaseShape);
		/// 考虑火箭的情况
        canChuKingBomb(handCard,num,haveNum,out,canOut,iBaseShape);

		/// 自己不出的情况视为不能赢对方，对方不出的时候，任何时候都是一种选择，所以这里不作处理
		
	}


	///用于内部计算
	
	/// 管牌操作，对方出了一手牌，现在轮到某人出牌时的操作 仅考虑单张和对子的情况
	/// hanCard，num为某人手中可能有的牌，haveNum为某人手中的牌数，out为目前桌子上出的最大的牌，canOut为此人可以出的牌的可能性总和
	void canChuDanDui(BYTE *handCard,int num,int haveNum,OutCardStruct out,std::vector<peiOutCardStruct> &canOut,int iBaseShape)
	{
		std::vector<peiOutCardStruct> canPeiOut;
		if(haveNum<out.iCardCount)
			return;
		peiOutCardStruct temp;
		
		if(iBaseShape == UG_ONLY_ONE)
		{
			canAutoDanChu(handCard,num,haveNum,canPeiOut);
		}
		else if(iBaseShape == UG_DOUBLE)
		{
			canAutoDuiChu(handCard,num,haveNum,canPeiOut);
		}
		for(int i=0;i<canPeiOut.size();i++)
		{
			if(logic.CompareOnlyOne(out.iCardList[0],canPeiOut[i].out.iCardList[0]))
			{		
				temp.out = canPeiOut[i].out;
				temp.type = 8;
				canOut.push_back(temp);
			}
		}
	}
	/// 管牌操作，对方出了一手牌，现在轮到某人出牌时的操作 仅考虑三张，三带一，三带一对的情况
	/// hanCard，num为某人手中可能有的牌，haveNum为某人手中的牌数，out为目前桌子上出的最大的牌，canOut为此人可以出的牌的可能性总和
	void canChuSan(BYTE *handCard,int num,int haveNum,OutCardStruct out,std::vector<peiOutCardStruct> &canOut,int iBaseShape)
	{
		if(haveNum<out.iCardCount)
			return;
		std::vector<peiOutCardStruct> canPeiOut;
		peiOutCardStruct temp;

		BYTE handTempCard[54];
		int tempNum;
		std::vector<peiOutCardStruct> tempDai;
		int danDai = m_DaiDanCard[m_myDeskStation].size();
		int duiDai = m_DaiDuiCard[m_myDeskStation].size();
		canAutoSanChu(handCard,num,haveNum,canPeiOut);
		int cardNum = logic.GetBulkBySpecifyCardCount(out.iCardList,out.iCardCount,3);
		m_DaiDanCard[m_myDeskStation].resize(danDai);
        m_DaiDuiCard[m_myDeskStation].resize(duiDai);
		for(int i=0;i<canPeiOut.size();i++)
		{
			if(canPeiOut[i].type == 7&&cardNum<logic.GetCardBulk(canPeiOut[i].out.iCardList[0]))
			{
				temp.out = canPeiOut[i].out;
				

			    if(iBaseShape == UG_THREE_ONE)
				{
					tempNum = num;
					memcpy(handTempCard,handCard,num*sizeof(BYTE));
            		tempDai.clear();
					delBulk(handTempCard,tempNum,logic.GetCardBulk(canPeiOut[i].out.iCardList[0]));   ///删掉所有同一权值的牌，避免出现类似333带3的情况
					canAutoDanChu(handTempCard,tempNum,haveNum-3,tempDai);
					OutCardStruct DaiPai;
					DaiPai.iCardCount = 0;
					for(int i=0;i<tempDai.size();i++)
					{
						DaiPai.iCardList[DaiPai.iCardCount++] = tempDai[i].out.iCardList[0];
					}
					if(DaiPai.iCardCount>=1)
					{
						temp.type =9;
                        canOut.push_back(temp);
			    		m_DaiDanCard[m_myDeskStation].push_back(DaiPai);
					}

				}
				else if(iBaseShape == UG_THREE_DOUBLE)
				{
					tempNum = num;
					memcpy(handTempCard,handCard,num*sizeof(BYTE));
            		tempDai.clear();
					delBulk(handTempCard,tempNum,logic.GetCardBulk(canPeiOut[i].out.iCardList[0]));   ///删掉所有同一权值的牌，避免出现类似333带3的情况
					canAutoDuiChu(handTempCard,tempNum,haveNum-3,tempDai);
					OutCardStruct DaiPai;
					DaiPai.iCardCount = 0;
					for(int i=0;i<tempDai.size();i++)
					{
						DaiPai.iCardList[DaiPai.iCardCount++] = tempDai[i].out.iCardList[0];
					}
					if(DaiPai.iCardCount>=1)
					{ 
						temp.type = 10;
						canOut.push_back(temp);
			    		m_DaiDuiCard[m_myDeskStation].push_back(DaiPai);
					}
				}
				else
				{
					temp.type = 7;
					canOut.push_back(temp);
				}
			}

		}
	}

	/// 管牌操作，对方出了一手牌，现在轮到某人出牌时的操作 仅考虑单顺和双顺的情况
	/// hanCard，num为某人手中可能有的牌，haveNum为某人手中的牌数，out为目前桌子上出的最大的牌，canOut为此人可以出的牌的可能性总和
	void canChuShun12(BYTE *handCard,int num,int haveNum,OutCardStruct out,std::vector<peiOutCardStruct> &canOut,int iBaseShape)
	{
        BYTE iResultCard[CARD_COUNT_PER_PERSON];
		int  iResultCardCount;
		int xSequence = 1;
		if(haveNum<out.iCardCount)
			return;
        peiOutCardStruct temp;
		if(UG_STRAIGHT == iBaseShape)
		{
			temp.type = 6;
			xSequence = 1;
		}
		else
		{
			temp.type = 5;
			xSequence = 2;
		}
		while(logic.TackOutSequence(handCard,num,out.iCardList,out.iCardCount,iResultCard,iResultCardCount,xSequence))
		{
			out.iCardCount = iResultCardCount;
			memcpy(out.iCardList,iResultCard,sizeof(BYTE)*iResultCardCount);
			temp.out = out;
			canOut.push_back(temp);
		}
	}



	/// 管牌操作，对方出了一手牌，现在轮到某人出牌时的操作 仅考虑三顺的情况
	/// hanCard，num为某人手中可能有的牌，haveNum为某人手中的牌数，out为目前桌子上出的最大的牌，canOut为此人可以出的牌的可能性总和
	void canChuShun3(BYTE *handCard,int num,int haveNum,OutCardStruct out,std::vector<peiOutCardStruct> &canOut,int iBaseShape)
	{
        BYTE iResultCard[CARD_COUNT_PER_PERSON];
		int  iResultCardCount;
		BYTE handTempCard[54];
		int tempNum;
		std::vector<peiOutCardStruct> tempDai;
		if(haveNum<out.iCardCount)
			return;
		peiOutCardStruct temp;
        if(UG_THREE_ONE_SEQUENCE == iBaseShape||UG_THREE_DOUBLE_SEQUENCE == iBaseShape)
		{
			int tempBulk[18];
			memset(tempBulk,0,sizeof(tempBulk));
			for(int i=0;i<num;i++)
			{
				tempBulk[logic.GetCardBulk(handCard[i])]++;
			}
			for(int cardNum = 3;cardNum<15;cardNum++)
			{
				if(tempBulk[cardNum]<3)
				{
					delBulk(out.iCardList,out.iCardCount,cardNum);      //去掉带的牌
				}
			}
		}

		while(logic.TackOutSequence(handCard,num,out.iCardList,out.iCardCount,iResultCard,iResultCardCount,3))
		{
			out.iCardCount = iResultCardCount;
			memcpy(out.iCardList,iResultCard,sizeof(BYTE)*iResultCardCount);


			if(UG_THREE_ONE_SEQUENCE == iBaseShape)    //带单牌
			{
				tempNum = num;
				memcpy(handTempCard,handCard,num*sizeof(BYTE));
				for(int i=0;i<out.iCardCount;i++)
				{
					delBulk(handTempCard,tempNum,logic.GetCardBulk(out.iCardList[i]));   ///删掉所有同一权值的牌，避免出现类似333带3的情况
				}
				tempDai.clear();
                canAutoDanChu(handTempCard,tempNum,haveNum-iResultCardCount,tempDai);
				OutCardStruct DaiPai;
				DaiPai.iCardCount = 0;
				for(int i=0;i<tempDai.size();i++)
				{
					DaiPai.iCardList[DaiPai.iCardCount++] = tempDai[i].out.iCardList[0];
				}
				if(DaiPai.iCardCount>=(iResultCardCount/3))
				{
					temp.out = out;
					temp.type = 11;
                    canOut.push_back(temp);
					for(int i=0;i<(iResultCardCount/3);i++)	
			        	m_DaiDanCard[m_myDeskStation].push_back(DaiPai);
				}

			}
			else if(UG_THREE_DOUBLE_SEQUENCE == iBaseShape)              // 带对子
			{
				tempNum = num;
				memcpy(handTempCard,handCard,num*sizeof(BYTE));
				for(int i=0;i<out.iCardCount;i++)
				{
					delBulk(handTempCard,tempNum,logic.GetCardBulk(out.iCardList[i]));   ///删掉所有同一权值的牌，避免出现类似333带3的情况
				}
				tempDai.clear();
                canAutoDuiChu(handTempCard,tempNum,haveNum-iResultCardCount,tempDai);
				OutCardStruct DaiPai;
				DaiPai.iCardCount = 0;
				for(int i=0;i<tempDai.size();i++)
				{
					DaiPai.iCardList[DaiPai.iCardCount++] = tempDai[i].out.iCardList[0];
				}
				if(DaiPai.iCardCount>=(iResultCardCount/3))
				{
					temp.out = out;
					temp.type = 12;
                    canOut.push_back(temp);
                    for(int i=0;i<(iResultCardCount/3);i++)			    
					   m_DaiDuiCard[m_myDeskStation].push_back(DaiPai);
				}
			}
			else       //无带牌
			{
				temp.out = out;
				temp.type = 4;
				canOut.push_back(temp);
			}			
		}
	}

	/// 管牌操作，对方出了一手牌，现在轮到某人出牌时的操作 仅考虑四带两单和四带二对的情况
	/// hanCard，num为某人手中可能有的牌，haveNum为某人手中的牌数，out为目前桌子上出的最大的牌，canOut为此人可以出的牌的可能性总和
	void canChuSi(BYTE *handCard,int num,int haveNum,OutCardStruct out,std::vector<peiOutCardStruct> &canOut,int iBaseShape)
	{
		if(haveNum<out.iCardCount)
			return;

		std::vector<peiOutCardStruct> canPeiOut;
		peiOutCardStruct temp;

		BYTE handTempCard[54];
		int tempNum;
		std::vector<peiOutCardStruct> tempDai;
		int danDai = m_DaiDanCard[m_myDeskStation].size();
		int duiDai = m_DaiDuiCard[m_myDeskStation].size();
		canAutoSiChu(handCard,num,haveNum,canPeiOut);
		int cardNum = logic.GetBulkBySpecifyCardCount(out.iCardList,out.iCardCount,4);
		m_DaiDanCard[m_myDeskStation].resize(danDai);
        m_DaiDuiCard[m_myDeskStation].resize(duiDai);


		for(int i=0;i<canPeiOut.size();i++)
		{
			temp.out = canPeiOut[i].out;
			if(UG_FOUR_TWO == iBaseShape)
			{
				tempNum = num;
				memcpy(handTempCard,handCard,num*sizeof(BYTE));
        		tempDai.clear();
				delBulk(handTempCard,tempNum,logic.GetCardBulk(canPeiOut[i].out.iCardList[0]));   ///删掉所有同一权值的牌，避免出现类似333带3的情况
				canAutoDanChu(handTempCard,tempNum,haveNum-4,tempDai);
				OutCardStruct DaiPai;
				DaiPai.iCardCount = 0;
				for(int i=0;i<tempDai.size();i++)
				{
					DaiPai.iCardList[DaiPai.iCardCount++] = tempDai[i].out.iCardList[0];
				}
				if(DaiPai.iCardCount>=2)
				{
					temp.type = 13;
                    canOut.push_back(temp);
		    		m_DaiDanCard[m_myDeskStation].push_back(DaiPai);
					m_DaiDanCard[m_myDeskStation].push_back(DaiPai);
				}
			}
			else if(UG_FOUR_TWO_DOUBLE == iBaseShape)
			{
				tempNum = num;
				memcpy(handTempCard,handCard,num*sizeof(BYTE));
        		tempDai.clear();
				delBulk(handTempCard,tempNum,logic.GetCardBulk(canPeiOut[i].out.iCardList[0]));   ///删掉所有同一权值的牌，避免出现类似333带3的情况
				canAutoDuiChu(handTempCard,tempNum,haveNum-4,tempDai);
				OutCardStruct DaiPai;
				DaiPai.iCardCount = 0;
				for(int i=0;i<tempDai.size();i++)
				{
					DaiPai.iCardList[DaiPai.iCardCount++] = tempDai[i].out.iCardList[0];
				}
				if(DaiPai.iCardCount>=2)
				{
					temp.type = 14;
                    canOut.push_back(temp);
		    		m_DaiDuiCard[m_myDeskStation].push_back(DaiPai);
					m_DaiDuiCard[m_myDeskStation].push_back(DaiPai);
				}
			}
		}
	}
	/// 管牌操作，对方出了一手牌，现在轮到某人出牌时的操作 仅考虑四张炸弹的情况
	/// hanCard，num为某人手中可能有的牌，haveNum为某人手中的牌数，out为目前桌子上出的最大的牌，canOut为此人可以出的牌的可能性总和
	void canChuBomb(BYTE *handCard,int num,int haveNum,OutCardStruct out,std::vector<peiOutCardStruct> &canOut,int iBaseShape)
	{
		/// 计算所有的四张的炸弹
		std::vector<peiOutCardStruct> canPeiOut;
		int danDai = m_DaiDanCard[m_myDeskStation].size();
		int duiDai = m_DaiDuiCard[m_myDeskStation].size();
		canAutoSiChu(handCard,num,haveNum,canPeiOut);
		m_DaiDanCard[m_myDeskStation].resize(danDai);
        m_DaiDuiCard[m_myDeskStation].resize(duiDai);
		peiOutCardStruct temp;
		temp.type = 3;
		if(UG_BOMB == iBaseShape)    //对方出的是四张的炸弹
		{
			int cardNum = logic.GetBulkBySpecifyCardCount(out.iCardList,out.iCardCount,4);
	        for(int i=0;i<canPeiOut.size();i++)
			{
				if(canPeiOut[i].type == 3&&logic.GetCardBulk(canPeiOut[i].out.iCardList[0])>cardNum)
				{
					//我的炸弹比对方的大的情况
					temp.out = canPeiOut[i].out;
					
			    	canOut.push_back(temp);
				}
			}
		}
		else if(UG_KING_BOMB != iBaseShape)  ///对方没出炸弹，我可以出所有的四张炸弹
		{
			for(int i=0;i<canPeiOut.size();i++)
			{
				if(canPeiOut[i].type == 3)
				{
					temp.out = canPeiOut[i].out;
					canOut.push_back(temp);
				}
			}
		}
		///对方出的王炸可定管不上，系统也不让我出牌
	}


	/// 管牌操作，对方出了一手牌，现在轮到某人出牌时的操作 仅考虑四带两单和四带二对的情况
	/// hanCard，num为某人手中可能有的牌，haveNum为某人手中的牌数，out为目前桌子上出的最大的牌，canOut为此人可以出的牌的可能性总和
	void canChuKingBomb(BYTE *handCard,int num,int haveNum,OutCardStruct out,std::vector<peiOutCardStruct> &canOut,int iBaseShape)
	{
		if(haveNum>=2)
		{
			int wangNum = 0;       //记录鬼的数量
			for(int i=0;i<num;i++)
			{
				if(handCard[i]==0x4E||handCard[i]==0x4F)
				{
					wangNum++;
				}
			}
		}
	}

	/// 根据手中可能有的牌某人决定出牌的情况汇总 只考虑王炸的时候
	/// handCard，num为某人可能有的牌，haveNum为某人的牌数，canOut为此人可以出的牌的可能性总和
	void canAutoKingBomb(BYTE *handCard,int num,int haveNum,std::vector<peiOutCardStruct> &canOut)
	{
		if(haveNum>=2)
		{
			peiOutCardStruct out;
			out.out.iCardCount = 0;
			out.type = 1;        //将火箭归结到王2之中
			for(int i=0;i<num;i++)
			{
				if(handCard[i] == 0x4E||handCard[i] == 0x4F)
				{
					out.out.iCardList[out.out.iCardCount++] = handCard[i];
				}
			}
			if(out.out.iCardCount == 2)  //恰好大小鬼
				canOut.push_back(out);
		}
	}

	/// 根据手中可能有的牌某人决定出牌的情况汇总 只考虑单牌
	/// handCard，num为某人可能有的牌，haveNum为某人的牌数，canOut为此人可以出的牌的可能性总和
	void canAutoDanChu(BYTE *handCard,int num,int haveNum,std::vector<peiOutCardStruct> &canOut)
	{
		if(haveNum<1)
			return;
		peiOutCardStruct out;
		out.out.iCardCount = 1;
		out.type = 0;
		int tempBulk[18];
		memset(tempBulk,0,sizeof(tempBulk));
		for(int i=0;i<num;i++)
		{
			tempBulk[logic.GetCardBulk(handCard[i])]++;
		}
		for(int cardnum=3;cardnum<18;cardnum++)
		{
			if(tempBulk[cardnum]>0)
			{
				for(int i=0;i<num;i++)
				{
					if(logic.GetCardBulk(handCard[i])==cardnum)
					{
						out.out.iCardList[0] = handCard[i];
						break;
					}
				}
				canOut.push_back(out);
			}
		}	
	}
 
	/// 根据手中可能有的牌某人决定出牌的情况汇总 只考虑对子
	/// handCard，num为某人可能有的牌，haveNum为某人的牌数，canOut为此人可以出的牌的可能性总和
	void canAutoDuiChu(BYTE *handCard,int num,int haveNum,std::vector<peiOutCardStruct> &canOut)
	{
		
		if(haveNum >= 2)
		{
			int tempBulk[18];
			memset(tempBulk,0,sizeof(tempBulk));
			for(int i=0;i<num;i++)
			{
				tempBulk[logic.GetCardBulk(handCard[i])]++;
			}
			/// 因为大小鬼不可能组成对子，所以不予考虑
			for(int cardnum = 3;cardnum<16;cardnum++)
			{
				if(tempBulk[cardnum]>=2)
				{
					peiOutCardStruct out;
					out.type = 8;
					out.out.iCardCount = 0;
					for(int i=0;i<num;i++)
					{
						if(logic.GetCardBulk(handCard[i])==cardnum)
						{
							out.out.iCardList[out.out.iCardCount++] = handCard[i];
							if(out.out.iCardCount>=2)
								break;
						}
					}
					canOut.push_back(out);
				}
			}
		}
	}

	/// 根据手中可能有的牌某人决定出牌的情况汇总 只考虑三张，三张带一张，三张带二张
	/// handCard，num为某人可能有的牌，haveNum为某人的牌数，canOut为此人可以出的牌的可能性总和
	void canAutoSanChu(BYTE *handCard,int num,int haveNum,std::vector<peiOutCardStruct> &canOut)
	{
		BYTE handTempCard[54];
		int tempNum;
		std::vector<peiOutCardStruct> temp;
		if(haveNum >= 3)
		{
			int tempBulk[18];
			memset(tempBulk,0,sizeof(tempBulk));
			for(int i=0;i<num;i++)
			{
				tempBulk[logic.GetCardBulk(handCard[i])]++;
			}
			/// 因为大小鬼不可能组成对子，所以不予考虑
			for(int cardnum = 3;cardnum<16;cardnum++)
			{
				if(tempBulk[cardnum]>=3)
				{
					tempNum = num;
					memcpy(handTempCard,handCard,num*sizeof(BYTE));
					peiOutCardStruct out;
					out.out.iCardCount = 0;
					out.type = 7;
					for(int i=0;i<num;i++)
					{
						if(logic.GetCardBulk(handCard[i])==cardnum)
						{
							out.out.iCardList[out.out.iCardCount++] = handCard[i];
							if(out.out.iCardCount>=3)
								break;
						}
					}
					canOut.push_back(out);
                    
					// 考虑三张带一和一对的情况
            		temp.clear();
					delBulk(handTempCard,tempNum,cardnum);   ///删掉所有同一权值的牌，避免出现类似333带3的情况
					canAutoDanChu(handTempCard,tempNum,haveNum-3,temp);
					OutCardStruct DaiPai;
					DaiPai.iCardCount = 0;
					for(int i=0;i<temp.size();i++)
					{
						DaiPai.iCardList[DaiPai.iCardCount++] = temp[i].out.iCardList[0];
					}
					if(DaiPai.iCardCount>=1)
					{
						out.type = 9;
                        canOut.push_back(out);
			    		m_DaiDanCard[m_myDeskStation].push_back(DaiPai);
					}
					temp.clear();
					canAutoDuiChu(handTempCard,tempNum,haveNum-3,temp);
                    DaiPai.iCardCount = 0;
					for(int i=0;i<temp.size();i++)
					{
						DaiPai.iCardList[DaiPai.iCardCount++] = temp[i].out.iCardList[0];
					}
					if(DaiPai.iCardCount>=1)
					{
						out.type = 10;
                        canOut.push_back(out);
				    	m_DaiDuiCard[m_myDeskStation].push_back(DaiPai);
					}
				}
			}
		}
	}
	/// 根据手中可能有的牌某人决定出牌的情况汇总 只考虑四张的炸弹，四带两单，和四带两对的情况
	/// handCard，num为某人可能有的牌，haveNum为某人的牌数，canOut为此人可以出的牌的可能性总和
	void canAutoSiChu(BYTE *handCard,int num,int haveNum,std::vector<peiOutCardStruct> &canOut)
	{
		BYTE handTempCard[54];
		int tempNum;
		std::vector<peiOutCardStruct> temp;
		if(haveNum >= 4)
		{
			int tempBulk[18];
			memset(tempBulk,0,sizeof(tempBulk));
			for(int i=0;i<num;i++)
			{
				tempBulk[logic.GetCardBulk(handCard[i])]++;
			}
			/// 因为大小鬼不可能组成对子，所以不予考虑
			for(int cardnum = 3;cardnum<16;cardnum++)
			{
				if(tempBulk[cardnum]>=4)
				{
					tempNum = num;
					memcpy(handTempCard,handCard,num*sizeof(BYTE));
					peiOutCardStruct out;
					out.out.iCardCount = 0;
					out.type = 3;
					for(int i=0;i<num;i++)
					{
						if(logic.GetCardBulk(handCard[i])==cardnum)
						{
							out.out.iCardList[out.out.iCardCount++] = handCard[i];
							if(out.out.iCardCount>=4)
								break;
						}
					}
					canOut.push_back(out);
                    //考虑四带二单和两双的情况
            		temp.clear();
					delBulk(handTempCard,tempNum,cardnum);   ///删掉所有同一权值的牌，避免出现类似333带3的情况
					canAutoDanChu(handTempCard,tempNum,haveNum-4,temp);
					OutCardStruct DaiPai;
					DaiPai.iCardCount = 0;
					for(int i=0;i<temp.size();i++)
					{
						DaiPai.iCardList[DaiPai.iCardCount++] = temp[i].out.iCardList[0];
					}
					if(DaiPai.iCardCount>=2)
					{
						out.type = 13;
						canOut.push_back(out);
                        m_DaiDanCard[m_myDeskStation].push_back(DaiPai);
				    	m_DaiDanCard[m_myDeskStation].push_back(DaiPai);
					}
					temp.clear();
					canAutoDuiChu(handTempCard,tempNum,haveNum-4,temp);
                    DaiPai.iCardCount = 0;
					for(int i=0;i<temp.size();i++)
					{
						DaiPai.iCardList[DaiPai.iCardCount++] = temp[i].out.iCardList[0];
					}
					if(DaiPai.iCardCount>=2)
					{
						out.type = 14;
						canOut.push_back(out);
			    		m_DaiDuiCard[m_myDeskStation].push_back(DaiPai);
						m_DaiDuiCard[m_myDeskStation].push_back(DaiPai);
					}
				}
			}
		}
	}
	/// 根据手中可能有的牌某人决定出牌的情况汇总 只考虑三顺的情况
	/// handCard，num为某人可能有的牌，haveNum为某人的牌数，canOut为此人可以出的牌的可能性总和
	void canAutoShun3Chu(BYTE *handCard,int num,int haveNum,std::vector<peiOutCardStruct> &canOut)
	{
		BYTE iResultCard[CARD_COUNT_PER_PERSON];
		int iResultCardCount = 0;
		BYTE iBaseCard[CARD_COUNT_PER_PERSON];
		int iBaseCount = 6;

		BYTE handTempCard[54];
		int tempNum;
		std::vector<peiOutCardStruct> temp;
		while(haveNum>=iBaseCount&&logic.TackOutSequence(handCard,num,iBaseCard,iBaseCount,iResultCard,iResultCardCount,3,true))
		{
			///第一次循环确定顺子的个数
			do{      
				///第二次循环确定顺子在这种个数情况下的大小
				
				tempNum = num;
				memcpy(handTempCard,handCard,num*sizeof(BYTE));

				peiOutCardStruct out;
				out.out.iCardCount = iBaseCount;                                       
				memcpy(out.out.iCardList,iResultCard,sizeof(BYTE)*iBaseCount); 
				out.type = 4;
				canOut.push_back(out);
				///带牌情况考虑
        		temp.clear();
				for(int i=0;i<out.out.iCardCount;i++)
				{
					delBulk(handTempCard,tempNum,logic.GetCardBulk(out.out.iCardList[i]));   ///删掉所有同一权值的牌，避免出现类似333带3的情况
				}
				canAutoDanChu(handTempCard,tempNum,haveNum-iBaseCount,temp);
				OutCardStruct DaiPai;
				DaiPai.iCardCount = 0;
				for(int i=0;i<temp.size();i++)
				{
					DaiPai.iCardList[DaiPai.iCardCount++] = temp[i].out.iCardList[0];
				}
				if(DaiPai.iCardCount>=(iBaseCount/3))
				{
					out.type = 11;
                    canOut.push_back(out);
                    for(int i=0;i<iBaseCount/3;i++)
			    	    m_DaiDanCard[m_myDeskStation].push_back(DaiPai);
				}
				temp.clear();
				canAutoDuiChu(handTempCard,tempNum,haveNum-iBaseCount,temp);
                DaiPai.iCardCount = 0;
				for(int i=0;i<temp.size();i++)
				{
					DaiPai.iCardList[DaiPai.iCardCount++] = temp[i].out.iCardList[0];
				}
				if(DaiPai.iCardCount>=(iBaseCount/3))
				{
					out.type = 12;
                    canOut.push_back(out);
                    for(int i=0;i<iBaseCount/3;i++)
		    		   m_DaiDuiCard[m_myDeskStation].push_back(DaiPai);
				}
				memcpy(iBaseCard,iResultCard,sizeof(BYTE)*iBaseCount);
			}while(logic.TackOutSequence(handCard,num,iBaseCard,iBaseCount,iResultCard,iResultCardCount,3));

			iBaseCount+=3;
		}
	}
	/// 根据手中可能有的牌某人决定出牌的情况汇总 只考虑双顺的情况
	/// handCard，num为某人可能有的牌，haveNum为某人的牌数，canOut为此人可以出的牌的可能性总和
	void canAutoShun2Chu(BYTE *handCard,int num,int haveNum,std::vector<peiOutCardStruct> &canOut)
	{
		BYTE iResultCard[CARD_COUNT_PER_PERSON];
		int iResultCardCount = 0;
		BYTE iBaseCard[CARD_COUNT_PER_PERSON];
		int iBaseCount = 6;

		while(haveNum>=iBaseCount&&logic.TackOutSequence(handCard,num,iBaseCard,iBaseCount,iResultCard,iResultCardCount,2,true))
		{
			///第一次循环确定顺子的个数
			do{      
				///第二次循环确定顺子在这种个数情况下的大小
		
				peiOutCardStruct out;
				out.out.iCardCount = iBaseCount;                                         
				memcpy(out.out.iCardList,iResultCard,sizeof(BYTE)*iBaseCount); 
				out.type = 5;
	            if(haveNum>=out.out.iCardCount)
			    	canOut.push_back(out);
				memcpy(iBaseCard,iResultCard,sizeof(BYTE)*iBaseCount);
			}while(logic.TackOutSequence(handCard,num,iBaseCard,iBaseCount,iResultCard,iResultCardCount,2));

			iBaseCount+=2;
		}
	}
	/// 根据手中可能有的牌某人决定出牌的情况汇总 只考虑单顺的情况
	/// handCard，num为某人可能有的牌，haveNum为某人的牌数，canOut为此人可以出的牌的可能性总和
	void canAutoShun1Chu(BYTE *handCard,int num,int haveNum,std::vector<peiOutCardStruct> &canOut)
	{
		BYTE iResultCard[CARD_COUNT_PER_PERSON];
		int iResultCardCount = 0;
		BYTE iBaseCard[CARD_COUNT_PER_PERSON];
		int iBaseCount = 5;

		while(haveNum>=iBaseCount&&logic.TackOutSequence(handCard,num,iBaseCard,iBaseCount,iResultCard,iResultCardCount,1,true))
		{
			///第一次循环确定顺子的个数
			do{      
				///第二次循环确定顺子在这种个数情况下的大小


				peiOutCardStruct out;
				out.out.iCardCount = iBaseCount;                                         
				memcpy(out.out.iCardList,iResultCard,sizeof(BYTE)*iBaseCount); 
				out.type = 6;
	            if(haveNum>=out.out.iCardCount)
			    	canOut.push_back(out);
				memcpy(iBaseCard,iResultCard,sizeof(BYTE)*iBaseCount);
			}while(logic.TackOutSequence(handCard,num,iBaseCard,iBaseCount,iResultCard,iResultCardCount,1));

			iBaseCount+=1;
		}
	}

private:
	/// 状态处理
	void initSureWin()
	{
		if(m_iNowOutPeople == m_myDeskStation)
	    	m_isSureWin = false;   ///必须等于true为了以后判断所有的子节点全部都是true
		else 
			m_isSureWin = true;

	}
};

