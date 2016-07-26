#pragma once

//#include "mj.h"
#include "../ChangeFile/UpgradeMessageEx.h"

//基础逻辑类逻辑类 
//处理：数据基本操作，插入删除，查询等
class CLogicBase
{
public:
	CLogicBase(void);
	~CLogicBase(void);

public:
	
	
	///向数组添加一个数据
	///@param  arr[] 数组, data 要添加的数据, ArrLen 数组的长度
	///@return true 添加成功，false 数组满了，添加不成功
	static bool AddOneDataToArr(BYTE arr[],BYTE data,BYTE ArrLen)
	{
		for(int i=0;i<ArrLen;i++)
		{
			if(arr[i]==255)
			{
				arr[i] = data;
				return true;
			}
		}
		return false;
	}

	///从数组添删除一个数据
	///@param  arr[] 数组, data 要删除的数据, ArrLen 数组的长度
	///@return true 添加成功，false 数组满了，添加不成功
	static bool DeleteOneDataFromArr(BYTE arr[],BYTE data,BYTE ArrLen)
	{
		for(int i=0;i<ArrLen;i++)
		{
			if(arr[i]==data)
			{
				arr[i] = 255;
				return true;
			}
		}
		return false;
	}

	///获取掷色子的随机数据
	///@param  data0 色子1, data1 色子2, data2 色子3
	static void GetSeziData(BYTE *data0 = NULL, BYTE *data1 = NULL, BYTE *data2 = NULL)
	{
		srand((unsigned)time(NULL));
		if(data0 != NULL)
		{
			*data0 = rand()%6+1;
		}
		if(data1 != NULL)
		{
			Sleep(50);
			*data1 = rand()%6+1;
		}
		if(data2 != NULL)
		{
			Sleep(50);
			*data2 = rand()%6+1;
		}
	}
	
	///获取数组的有效元素个数
	///@param  arr[] 目标数组,data 无效值, ArrLen 数组长度
	///@return 有效元素个数
	static int GetArrNum(BYTE arr[],BYTE data,BYTE ArrLen)
	{
		int num = 0;
		for(int i=0;i<ArrLen;i++)
		{
			if(arr[i] != data)
			{
				num++;
			}
		}
		return num;
	}

	///判断数组中是否存在某个元素
	///@param  arr[] 目标数组,data 无效值, ArrLen 数组长度
	///@return 有效元素个数
	static bool IsHaveInArr(BYTE arr[],BYTE data,BYTE ArrLen)
	{
		for(int i=0;i<ArrLen;i++)
		{
			if(arr[i] == data)
			{
				return true;
			}
		}
		return false;
	}
	
	///获取下家位置
	static BYTE GetNextStation(BYTE station,bool shun=false,bool hu[] = NULL)
	{
		BYTE re = 0;
		if(station == 0)
		{
			re = 1;
		}
		return re;
	}

	//获取最近的糊牌玩家
	static BYTE GetCanHuUser(BYTE outst,bool hu[],BYTE hust,BYTE fan[]=NULL)//获取离糊牌玩家近的玩家
	{
		//BYTE station = outst;
		//BYTE maxfan =0,st=hust;
		//if(fan != NULL)//有输入番数，先选糊牌番数大的
		//{
		//	for(int i=0;i<PLAY_COUNT;++i)
		//	{
		//		if(i == station || !hu[i])
		//			continue;
		//		if(fan[i] > maxfan)
		//		{
		//			maxfan = fan[i];
		//			st = i;
		//		}
		//	}
		//	return st;
		//}
		//for(int i=0;i<PLAY_COUNT-1;++i)
		//{
		//	station = GetNextStation(station);
		//	if(hu[station])
		//		return station;
		//}
		return hust;
	}

	//吃牌数据拷贝
	static void CopyChiData(GCPStructEx &dest,tagChiPaiEx &sorce)//获取离糊牌玩家近的玩家
	{
		dest.Init();
		dest.byType = ACTION_CHI;
		dest.iBeStation = sorce.byBeChi;
		dest.iOutpai = sorce.byPs;
		dest.iStation = sorce.byUser;
		dest.byData[0] = sorce.byChiPs[0];
		dest.byData[1] = sorce.byChiPs[1];
		dest.byData[2] = sorce.byChiPs[2];
	}

	//碰牌数据拷贝
	static void CopyPengData(GCPStructEx &dest,tagPengPaiEx &sorce)//获取离糊牌玩家近的玩家
	{
		dest.Init();
		dest.byType = ACTION_PENG;
		dest.iBeStation = sorce.byBePeng;
		dest.iOutpai = sorce.byPs;
		dest.iStation = sorce.byUser;
		dest.byData[0] = dest.byData[1] = dest.byData[2] = sorce.byPs;
	}

	//杠牌数据拷贝
	static void CopyGangData(GCPStructEx &dest,tagGangPaiEx &sorce)//获取离糊牌玩家近的玩家
	{
		dest.Init();
		dest.byType = sorce.byType;
		dest.iBeStation = sorce.byBeGang;
		dest.iOutpai = sorce.byPs;
		dest.iStation = sorce.byUser;
		dest.byData[0] = dest.byData[1] = dest.byData[2] = dest.byData[3] = sorce.byPs;
	}

	//获取下一局的庄家
	static void GetNextNt(tagCountFenEx& CountFen,BYTE &newNt,BYTE &LianZhuang,BYTE type)
	{
		bool lian = true;
		LianZhuang++;
		switch(type)
		{
		case 0://庄赢连庄，否则下家坐庄
			{
				if(!CountFen.bHu[newNt] || CountFen.byUser==255)//糊牌中没有庄家或者流局
				{
					newNt = GetNextStation(newNt);
					lian = false;
				}
			}
			break;
		case 1:///庄赢或流局连庄，否则下家坐庄
			{
				if(!CountFen.bHu[newNt] && CountFen.byUser!=255)//糊牌中没有庄家或者流局
				{
					newNt = GetNextStation(newNt);
					lian = false;
				}
			}
			break;
		case 2:///谁赢谁坐庄，流局或庄赢庄家连庄
			{
				if(CountFen.byUser!=newNt)
				{
					newNt = CountFen.byUser;
					lian = false;
				}
			}
			break;
		case 3:///谁赢谁坐庄，流局下家坐庄
			{
				if(CountFen.byUser==255)//流局
				{
					newNt = GetNextStation(newNt);
					lian = false;
				}
				else
				{
					if(newNt != CountFen.byUser)
					{
						lian = false;
					}
					newNt= CountFen.byUser;
				}
			}
			break;
		case 4://不管结构如果都是下家坐庄
			{
				newNt = GetNextStation(newNt);
				lian = false;
			}
			break;
		}
		if(!lian)
		{
			LianZhuang=1;
		}
	}
	




};
