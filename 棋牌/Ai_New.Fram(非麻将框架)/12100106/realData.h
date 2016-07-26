
#include <list>
#include <vector>
#include <algorithm>
using namespace std;

#include "clientgamedlg.h"

///实例化的结点数据
CUpGradeGameLogic logic;
class realData
{
public:
	realData& operator= (realData& d)
	{

		memcpy(m_MyCard,d.m_MyCard,sizeof(m_MyCard));

		isLeaf = d.isLeaf;
		shengli = d.shengli;
		shibai = d.shibai;
        
		return *this;
	}
	realData()
	{
		isLeaf = false;
		shengli = 0;
		shibai = 0;
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
		shengli += d.shengli;
		shibai += d.shibai;
	}

	void born(list<realData> &lData)            ///用于生成自己的孩子结点的数据
	{
		///自由当牌数大于等于3时形势才开始明了，考虑3张牌以内的情况在手里的时候的胜率意义不大
        

	}


	BYTE					m_MyCard[3];			//自己的三张牌
	BYTE                    m_DuiShouCard[3];       //对手的牌
	
	///记录是否为叶子节点
    bool                    isLeaf;

	///胜利的次数
	int                     shengli;
	///失败的次数
	int                     shibai;
		
	///因为实际运用当中只有三张牌，且情况比较明了，主要靠猜心思，所以可以直接计算胜率，不必通过树的方法计算胜利的可能性
	void computeShengLv()                      
	{
		vector<BYTE> vCard;
		getLeftCard(vCard);
        
		int leftCardNum = vCard.size();
		for(int i=0;i<leftCardNum-2;i++)
		{
			m_DuiShouCard[0] = vCard[i];
			for(int j=i+1;j<leftCardNum-1;j++)
			{
				m_DuiShouCard[1] = vCard[j];
				for(int k=j+1;k<leftCardNum;k++)
				{
					m_DuiShouCard[2] = vCard[k];
					if(logic.CompareCard(m_MyCard,3,m_DuiShouCard,3)>0)
					{
						shengli++;
					}
					else
					{
						shibai++;
					}
				}
			}
		}
	}

private:
	///用于内部计算
	

    
	///计算剩下的牌
	void getLeftCard(vector<BYTE>& vCard)
	{
		static const BYTE m_CardArray[52] =
		{
			0x01, 0x02 ,0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,		//方块 2 - A
			0x11, 0x12 ,0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,		//梅花 2 - A
			0x21, 0x22 ,0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,		//红桃 2 - A
			0x31, 0x32 ,0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D 		//黑桃 2 - A
		};
		vCard.resize(52);
        for (int i=0;i<52;i++)
		{
			vCard[i] = m_CardArray[i];
		}	
		/// 去掉现有的牌
		vector<BYTE>::iterator it;
		for(int i=0;i<3;i++)
		{
			it = find(vCard.begin(),vCard.end(),m_MyCard[i]);
			vCard.erase(it);
		}
	}
};

